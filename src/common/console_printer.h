/*
 * Copyright 2020 McGraw-Hill Education. All rights reserved. No reproduction or
 * distribution without the prior written consent of McGraw-Hill Education.
 */
#ifndef CONSOLE_PRINTER_H
#define CONSOLE_PRINTER_H

#include "printer.h"

namespace lc3 {
class ConsolePrinter : public utils::IPrinter {
 public:
  virtual void setColor(utils::PrintColor color) override;
  virtual void print(std::string const& string) override;
  virtual void newline(void) override;
};
};  // namespace lc3

#endif
