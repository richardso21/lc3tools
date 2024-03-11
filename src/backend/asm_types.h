/*
 * Copyright 2020 McGraw-Hill Education. All rights reserved. No reproduction or
 * distribution without the prior written consent of McGraw-Hill Education.
 */
#ifndef ASM_TYPES_H
#define ASM_TYPES_H

#include <iostream>
#include <string>
#include <vector>

#include "utils.h"

namespace lc3 {
namespace utils {
class AssemblerLogger;
};

namespace core {
namespace asmbl {

struct Token {
  enum class Type { NUM = 0, STRING, EOL, INVALID } type;

  std::string str;
  int32_t num;

  uint32_t row, col, len;
  std::string line;

  Token(void) : type(Token::Type::INVALID), row(0), col(0), len(0) {}
};

struct StatementPiece {
  enum class Type { INST = 0, PSEUDO, LABEL, REG, STRING, NUM, INVALID } type;

  std::string str;
  uint32_t num;

  uint32_t col, len;

  StatementPiece(void) : type(Type::INVALID), num(0), col(0), len(0) {}
  StatementPiece(Token const& token, Type type)
      : type(type), col(token.col), len(token.len) {
    if (type == Type::NUM) {
      num = token.num;
    } else {
      str = token.str;
    }
  }
};

struct Statement {
  optional<StatementPiece> label;
  optional<StatementPiece> base;
  std::vector<StatementPiece> operands;

  uint32_t pc;

  std::string line;
  uint32_t row;

  bool valid;

  Statement(void) : pc(0), line(""), row(0), valid(true) {}
};

optional<uint32_t> getNum(Statement const& statement,
                          StatementPiece const& piece, uint32_t width,
                          bool sext, lc3::utils::AssemblerLogger& logger,
                          bool log_enable);
};  // namespace asmbl
};  // namespace core
};  // namespace lc3

std::ostream& operator<<(std::ostream& out,
                         lc3::core::asmbl::Token const& token);
std::ostream& operator<<(std::ostream& out,
                         lc3::core::asmbl::StatementPiece const& piece);
std::ostream& operator<<(std::ostream& out,
                         lc3::core::asmbl::Statement const& statement);

#endif
