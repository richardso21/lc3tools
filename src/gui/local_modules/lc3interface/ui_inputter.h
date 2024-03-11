/*
 * Copyright 2020 McGraw-Hill Education. All rights reserved. No reproduction or
 * distribution without the prior written consent of McGraw-Hill Education.
 */
#ifndef UI_INPUTTER
#define UI_INPUTTER

#include <vector>

namespace utils {
class UIInputter : public lc3::utils::IInputter {
 private:
  std::mutex buffer_mutex;
  std::vector<char> buffer;

 public:
  UIInputter(void) = default;

  virtual void beginInput(void) override {}
  virtual bool getChar(char& c) override;
  virtual void endInput(void) override {}
  virtual bool hasRemaining(void) const override { return false; }

  void clearInput(void);
  void addInput(char c);
};
};  // namespace utils

#endif
