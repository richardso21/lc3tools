/*
 * Copyright 2020 McGraw-Hill Education. All rights reserved. No reproduction or
 * distribution without the prior written consent of McGraw-Hill Education.
 */
#include "callback.h"

std::string lc3::core::callbackTypeToString(CallbackType type) {
  switch (type) {
    case CallbackType::PRE_INST:
      return "pre-instruction";
    case CallbackType::POST_INST:
      return "post-instruction";
    case CallbackType::SUB_ENTER:
      return "subroutine-enter";
    case CallbackType::SUB_EXIT:
      return "subroutine-exit";
    case CallbackType::EX_ENTER:
      return "exception-enter";
    case CallbackType::EX_EXIT:
      return "exception-exit";
    case CallbackType::INT_ENTER:
      return "interrupt-enter";
    case CallbackType::INT_EXIT:
      return "interrupt-exit";
    case CallbackType::BREAKPOINT:
      return "breakpoint";
    case CallbackType::INPUT_REQUEST:
      return "input-request";
    case CallbackType::INPUT_POLL:
      return "input-poll";
    default:
      return "unknown";
  }
}

lc3::core::CallbackTypeUnderlying lc3::core::callbackTypeToUnderlying(
    CallbackType type) {
  return static_cast<CallbackTypeUnderlying>(type);
}
