/*
 * Copyright 2020 McGraw-Hill Education. All rights reserved. No reproduction or
 * distribution without the prior written consent of McGraw-Hill Education.
 */
#ifndef INSTRUCTION_ENCODER_H
#define INSTRUCTION_ENCODER_H

#include <memory>

#include "isa.h"
#include "logger.h"
#include "utils.h"

namespace lc3 {
namespace core {
namespace asmbl {
class Encoder : public ISAHandler {
 public:
  Encoder(lc3::utils::AssemblerLogger& logger, bool enable_liberal_assembly);

  bool isStringPseudo(std::string const& search) const;
  bool isStringValidReg(std::string const& search) const;
  bool isStringInstructionName(std::string const& name) const;
  bool isPseudo(Statement const& statement) const;
  bool isInst(Statement const& statement) const;
  bool isValidAlphaNumLabel(Statement const& statement) const;
  bool isValidPseudoOrig(Statement const& statement,
                         bool log_enable = false) const;
  bool isValidPseudoFill(Statement const& statement,
                         bool log_enable = false) const;
  bool isValidPseudoFill(Statement const& statement, SymbolTable const& symbols,
                         bool log_enable = false) const;
  bool isValidPseudoBlock(Statement const& statement,
                          bool log_enable = false) const;
  bool isValidPseudoString(Statement const& statement,
                           bool log_enable = false) const;
  bool isValidPseudoEnd(Statement const& statement,
                        bool log_enable = false) const;

  bool validatePseudo(Statement const& statement,
                      SymbolTable const& symbols) const;
  optional<PIInstruction> validateInstruction(Statement const& statement) const;

  uint32_t getPseudoOrig(Statement const& statement) const;
  uint32_t getPseudoFill(Statement const& statement,
                         SymbolTable const& symbols) const;
  uint32_t getPseudoBlockSize(Statement const& statement) const;
  uint32_t getPseudoStringSize(Statement const& statement) const;
  std::string getPseudoString(Statement const& statement) const;
  optional<uint32_t> encodeInstruction(Statement const& statement,
                                       SymbolTable const& symbols,
                                       PIInstruction pattern) const;

  void setLiberalAsm(bool enable_liberal_asm) {
    this->enable_liberal_asm = enable_liberal_asm;
  }

 private:
  lc3::utils::AssemblerLogger& logger;
  bool enable_liberal_asm;

  bool validatePseudoOperands(
      Statement const& statement, std::string const& pseudo,
      std::vector<StatementPiece::Type> const& valid_types,
      uint32_t operand_count, bool log_enable) const;

  std::map<std::string, std::vector<PIInstruction>> instructions_by_name;
};
};  // namespace asmbl
};  // namespace core
};  // namespace lc3

#endif
