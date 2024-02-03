/*
 * Copyright 2020 McGraw-Hill Education. All rights reserved. No reproduction or
 * distribution without the prior written consent of McGraw-Hill Education.
 */
#ifndef NULL_PRINTER_H
#define NULL_PRINTER_H

#include "printer.h"

namespace lc3 {
class NullPrinter : public utils::IPrinter {
public:
  virtual void setColor(utils::PrintColor color) override { return; };
  virtual void print(std::string const &string) override { return; };
  virtual void newline(void) override { return; };
};
}; // namespace lc3

#endif
