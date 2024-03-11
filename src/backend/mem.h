/*
 * Copyright 2020 McGraw-Hill Education. All rights reserved. No reproduction or
 * distribution without the prior written consent of McGraw-Hill Education.
 */
#ifndef MEM_NEW_H
#define MEM_NEW_H

#include <iostream>
#include <string>

namespace lc3 {
namespace core {
class MemLocation {
 public:
  MemLocation(void) : MemLocation(0x0000, "") {}
  MemLocation(uint16_t value, std::string const& line)
      : MemLocation(value, line, false) {}
  MemLocation(uint16_t value, std::string const& line, bool is_orig)
      : value(value), line(line), is_orig(is_orig) {}

  uint16_t getValue(void) const { return value; }
  std::string const& getLine(void) const { return line; }
  bool isOrig(void) const { return is_orig; }
  void setValue(uint16_t value) { this->value = value; }
  void setLine(std::string const& line) { this->line = line; }
  void setIsOrig(bool is_orig) { this->is_orig = is_orig; }

  friend std::ostream& operator<<(std::ostream& out, MemLocation const& in);
  friend std::istream& operator>>(std::istream& in, MemLocation& out);

 private:
  uint16_t value;
  std::string line;
  bool is_orig;
};

std::ostream& operator<<(std::ostream& out, MemLocation const& in);
std::istream& operator>>(std::istream& in, MemLocation& out);
};  // namespace core
};  // namespace lc3

#endif
