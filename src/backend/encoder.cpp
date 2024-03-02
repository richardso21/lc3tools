/*
 * Copyright 2020 McGraw-Hill Education. All rights reserved. No reproduction or distribution without the prior written consent of McGraw-Hill Education.
 */
#include <algorithm>
#include <array>
#include <cassert>
#include <sstream>

#include "encoder.h"

using namespace lc3::core::asmbl;

Encoder::Encoder(lc3::utils::AssemblerLogger & logger, bool enable_liberal_asm)
    : ISAHandler(), logger(logger), enable_liberal_asm(enable_liberal_asm)
{
    for(PIInstruction inst : instructions) {
        instructions_by_name[inst->getName()].push_back(inst);
    }
}

bool Encoder::isStringPseudo(std::string const & search) const
{
    return search.size() > 0 && search[0] == '.';
}

bool Encoder::isPseudo(Statement const & statement) const
{
    return statement.base && statement.base->type == StatementPiece::Type::PSEUDO;
}

bool Encoder::isInst(Statement const & statement) const
{
    return statement.base && statement.base->type == StatementPiece::Type::INST;
}

bool Encoder::isStringValidReg(std::string const & search) const
{
    std::string lower_search = search;
    std::transform(lower_search.begin(), lower_search.end(), lower_search.begin(), ::tolower);
    return regs.find(lower_search) != regs.end();
}

bool Encoder::isStringInstructionName(std::string const & name) const
{
    return instructions_by_name.count(utils::toLower(name));
}

bool Encoder::isValidPseudoOrig(Statement const & statement, bool log_enable) const
{
    if(isPseudo(statement) && utils::toLower(statement.base->str) == ".orig") {
        bool valid_operands = validatePseudoOperands(statement, ".orig", {StatementPiece::Type::NUM}, 1, log_enable);
        if(valid_operands) {
            return getNum(statement, statement.operands[0], 16, false, logger, log_enable);
        }
        return false;
    }
    return false;
}

bool Encoder::isValidPseudoFill(Statement const & statement, bool log_enable) const
{
    if(isPseudo(statement) && utils::toLower(statement.base->str) == ".fill") {
        bool valid_operands = validatePseudoOperands(statement, ".fill", {StatementPiece::Type::NUM,
            StatementPiece::Type::STRING}, 1, log_enable);
        if(valid_operands && statement.operands[0].type == StatementPiece::Type::NUM) {
            // .fill has implicit sext; if the number is negative, treat as signed, otherwise just treat it as unsigned.
            bool should_sext = static_cast<int32_t>(statement.operands[0].num) < 0;
            auto num = getNum(statement, statement.operands[0], 16, should_sext, logger, log_enable);
            return valid_operands && num;
        }
        // If you make it here, the operand is a string, so defer the check until after the symbol table is formed.
        return valid_operands;
    }
    return false;
}

bool Encoder::isValidPseudoFill(Statement const & statement, lc3::core::SymbolTable const & symbols,
    bool log_enable) const
{
    using namespace lc3::utils;

    if(isValidPseudoFill(statement, log_enable)) {
        if(statement.operands[0].type == StatementPiece::Type::STRING &&
            symbols.find(utils::toLower(statement.operands[0].str)) == symbols.end())
        {
            if(log_enable) {
                logger.asmPrintf(PrintType::P_ERROR, statement, statement.operands[0],
                    "could not find label");
                logger.newline();
            }
            return false;
        }
        return true;
    }
    return false;
}

bool Encoder::isValidPseudoBlock(Statement const & statement, bool log_enable) const
{
    if(isPseudo(statement) && utils::toLower(statement.base->str) == ".blkw") {
        bool valid_operands = validatePseudoOperands(statement, ".blkw", {StatementPiece::Type::NUM}, 1, false);
        if(valid_operands) {
            auto num = getNum(statement, statement.operands[0], 16, false, logger, log_enable);
            if(num) {
                if(log_enable && statement.operands[0].num == 0) {
                    logger.asmPrintf(utils::PrintType::P_ERROR, statement, statement.operands[0],
                        "operand to .blkw must be > 0");
                    logger.newline();
                }
                return statement.operands[0].num != 0;
            }
            return false;
        }
    }
    return false;
}

bool Encoder::isValidPseudoString(Statement const & statement, bool log_enable) const
{
    if(isPseudo(statement) && utils::toLower(statement.base->str) == ".stringz") {
        return validatePseudoOperands(statement, ".stringz", {StatementPiece::Type::STRING}, 1, log_enable);
    }
    return false;
}

bool Encoder::isValidPseudoEnd(Statement const & statement, bool log_enable) const
{
    if(isPseudo(statement) && utils::toLower(statement.base->str) == ".end") {
        return validatePseudoOperands(statement, ".end", {}, 0, log_enable);
    }
    return false;
}

bool Encoder::validatePseudo(Statement const & statement, lc3::core::SymbolTable const & symbols) const
{
    using namespace lc3::utils;

    if(! isPseudo(statement)) { return false; }

    std::string lower_base = utils::toLower(statement.base->str);
    if(lower_base == ".orig") {
        return isValidPseudoOrig(statement, true);
    } else if(lower_base == ".fill") {
        return isValidPseudoFill(statement, symbols, true);
    } else if(lower_base == ".blkw") {
        return isValidPseudoBlock(statement, true);
    } else if(lower_base == ".stringz") {
        return isValidPseudoString(statement, true);
    } else if(lower_base == ".end") {
        return isValidPseudoEnd(statement, true);
    } else {
        if(enable_liberal_asm) {
            logger.asmPrintf(PrintType::P_WARNING, statement, *statement.base, "ignoring invalid pseudo-op");
            logger.newline(PrintType::P_WARNING);
            return true;
        } else {
            logger.asmPrintf(PrintType::P_ERROR, statement, *statement.base, "invalid pseudo-op");
            logger.newline();
            return false;
        }
    }
}

bool Encoder::validatePseudoOperands(Statement const & statement, std::string const & pseudo,
    std::vector<StatementPiece::Type> const & valid_types, uint32_t operand_count, bool log_enable) const
{
    using namespace lc3::utils;

    if(statement.operands.size() < operand_count) {
        // If there are not enough operands, print out simple error message.
        if(log_enable) {
            logger.asmPrintf(PrintType::P_ERROR, statement, "%s requires %d more operand(s)", pseudo.c_str(),
                operand_count - statement.operands.size());
            logger.newline();
        }
        return false;
    } else if(statement.operands.size() > operand_count) {
        // If there are too many operands, print out a warning/error for each extraneous operand.
        if(log_enable) {
            for(uint32_t i = operand_count; i < statement.operands.size(); i += 1) {
                logger.asmPrintf(PrintType::P_ERROR, statement, statement.operands[i], "extraneous operand to %s",
                    pseudo.c_str());
                logger.newline();
            }
        }
        return false;
    } else {
        // If there are the correct number of operands, confirm that they are of the correct type.
        bool all_valid_types = true;
        for(uint32_t i = 0; i < operand_count; i += 1) {
            bool valid_type = false;
            for(StatementPiece::Type const & type : valid_types) {
                if(statement.operands[i].type == type) {
                    valid_type = true;
                    break;
                }
            }
            if(! valid_type) {
                all_valid_types = false;
                if(log_enable) {
                    // Some nonsense to make pretty messages depending on the expected type.
                    std::stringstream error_msg;
                    error_msg << "operand should be";
                    std::string prefix = "";
                    for(StatementPiece::Type const & type : valid_types) {
                        if(type == StatementPiece::Type::NUM) {
                            error_msg << prefix << " numeric";
                        } else {
                            error_msg << prefix << " a string";
                        }
                        prefix = " or";
                    }
                    logger.asmPrintf(PrintType::P_ERROR, statement, statement.operands[i], "%s",
                        error_msg.str().c_str());
                    logger.newline();
                }
            }
        }
        return all_valid_types;
    }

    return true;
}

lc3::optional<lc3::core::PIInstruction> Encoder::validateInstruction(Statement const & statement) const
{
    using namespace lc3::utils;

    if(! isInst(statement)) { return {}; }

    std::string statement_op_string = "";
    for(StatementPiece const & statement_op : statement.operands) {
        switch(statement_op.type) {
            case StatementPiece::Type::NUM: statement_op_string += 'n'; break;
            case StatementPiece::Type::STRING: statement_op_string += 's'; break;
            case StatementPiece::Type::REG: statement_op_string += 'r'; break;
            default: break;
        }
    }

    bool name_match = false;
    PIInstruction match = nullptr;

    for(auto const & candidate_inst_name : instructions_by_name) {
        name_match = utils::toLower(statement.base->str) == candidate_inst_name.first;
        if(name_match) {
            for(PIInstruction candidate_inst : candidate_inst_name.second) {
                // Convert the operand types of the candidate and the statement into a string so
                // that string comparison can be used on the operands too.
                std::string candidate_op_string = "";
                for(PIOperand candidate_op : candidate_inst->getOperands()) {
                    switch(candidate_op->getType()) {
                        case IOperand::Type::NUM: candidate_op_string += 'n'; break;
                        case IOperand::Type::LABEL: candidate_op_string += 'l'; break;
                        case IOperand::Type::REG: candidate_op_string += 'r'; break;
                        default: break;
                    }
                }

                bool full_match = false;

                // If there's a label in the operands, then the statement can either have a numberic or string
                // operand, so check against s and n. Otherwise, just compare normally.
                if(candidate_op_string.find('l') != std::string::npos) {
                    std::string candidate_op_string_copy = candidate_op_string;
                    std::replace(candidate_op_string_copy.begin(), candidate_op_string_copy.end(), 'l', 's');
                    bool op_match_s = statement_op_string == candidate_op_string_copy;

                    candidate_op_string_copy = candidate_op_string;
                    std::replace(candidate_op_string_copy.begin(), candidate_op_string_copy.end(), 'l', 'n');
                    bool op_match_n = statement_op_string == candidate_op_string_copy;

                    full_match = op_match_s || op_match_n;
                } else {
                    full_match = statement_op_string == candidate_op_string;
                }

                if (full_match) {
                    match = candidate_inst;
                    break;
                }
            }

            break;
        }
    }

    // If there are no candidates or if the instruction itself was not a perfect match
    if(!name_match) {
        logger.asmPrintf(PrintType::P_ERROR, statement, *statement.base, "invalid instruction");
        logger.newline();
        return {};
    }

    // If the instruction was a match but the operands weren't
    if(!match) {
        logger.asmPrintf(PrintType::P_ERROR, statement, *statement.base, "invalid usage of \'%s\' instruction",
            statement.base->str.c_str());
        logger.newline();
        return {};
    }

    return match;
}

uint32_t Encoder::getPseudoOrig(Statement const & statement) const
{
#ifdef _ENABLE_DEBUG
    assert(isValidPseudoOrig(statement));
#endif
    auto ret = getNum(statement, statement.operands[0], 16, false, logger, false);
#ifdef _ENABLE_DEBUG
    assert(ret.isValid());
#endif
    return *ret;
}

uint32_t Encoder::getPseudoFill(Statement const & statement,
    lc3::core::SymbolTable const & symbols) const
{
#ifdef _ENABLE_DEBUG
    assert(isValidPseudoFill(statement, symbols));
#endif
    if(statement.operands[0].type == StatementPiece::Type::NUM) {
        bool should_sext = static_cast<int32_t>(statement.operands[0].num) < 0;
        auto ret = getNum(statement, statement.operands[0], 16, should_sext, logger, false);
#ifdef _ENABLE_DEBUG
        assert(ret.isValid());
#endif
        return *ret;
    } else {
        return symbols.at(utils::toLower(statement.operands[0].str));
    }
}

uint32_t Encoder::getPseudoBlockSize(Statement const & statement) const
{
#ifdef _ENABLE_DEBUG
    assert(isValidPseudoBlock(statement));
#endif
    auto ret = getNum(statement, statement.operands[0], 16, false, logger, false);
#ifdef _ENABLE_DEBUG
    assert(ret.isValid());
#endif
    return *ret;
}

uint32_t Encoder::getPseudoStringSize(Statement const & statement) const
{
#ifdef _ENABLE_DEBUG
    assert(isValidPseudoString(statement));
#endif
    return (uint32_t) (getPseudoString(statement).size() + 1);
}

std::string Encoder::getPseudoString(Statement const & statement) const
{
#ifdef _ENABLE_DEBUG
    assert(isValidPseudoString(statement));
#endif
    std::string const & str = statement.operands[0].str;
    std::string ret;
    for(uint32_t i = 0; i < str.size(); i += 1) {
        if(str[i] == '\\' && i + 1 < str.size()) {
            switch(str[i + 1]) {
                case '\\': ret += '\\'; i += 1; break;
                case 'n': ret += '\n'; i += 1; break;
                case 'r': ret += '\r'; i += 1; break;
                case 't': ret += '\t'; i += 1; break;
                case '"': ret += '"'; i += 1; break;
                default: ret += '\\';
            }
        } else {
            ret += str[i];
        }
    }
    return ret;
}

lc3::optional<uint32_t> Encoder::encodeInstruction(Statement const & statement, lc3::core::SymbolTable const & symbols,
    lc3::core::PIInstruction pattern) const
{
    // The first "operand" of an instruction encoding is the op-code.
    optional<uint32_t> inst_encoding = pattern->getOperand(0)->encode(statement, *statement.base, regs, symbols, logger);
    uint32_t encoding;
    if(inst_encoding) {
        encoding = *inst_encoding;
    } else {
        return {};
    }

    // Iterate over the remaining "operands" of the instruction encoding.
    uint32_t operand_idx = 0;
    for(uint32_t i = 1; i < pattern->getOperands().size(); i += 1) {
        PIOperand operand = pattern->getOperand(i);

        encoding <<= operand->getWidth();
        try {
            optional<uint32_t> operand_encoding = {};
            if(operand->getType() == IOperand::Type::FIXED) {
                StatementPiece dummy;
                operand_encoding = operand->encode(statement, dummy, regs, symbols, logger);
            } else {
                operand_encoding = operand->encode(statement, statement.operands[operand_idx], regs, symbols, logger);
            }

            if(operand_encoding) {
                encoding |= *operand_encoding;
            } else {
                return {};
            }
        } catch(lc3::utils::exception const & e) {
            (void) e;
            return {};
        }

        if(operand->getType() != IOperand::Type::FIXED) {
            operand_idx += 1;
        }
    }

    return encoding;
}
