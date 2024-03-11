/*
 * Copyright 2020 McGraw-Hill Education. All rights reserved. No reproduction or
 * distribution without the prior written consent of McGraw-Hill Education.
 */
#ifndef DECODER_H
#define DECODER_H

#include "isa_abstract.h"
#include "utils.h"

namespace lc3 {
namespace core {
namespace sim {
class Decoder : public lc3::core::ISAHandler {
 public:
  Decoder(void);

  optional<PIInstruction> decode(uint16_t value) const;

 private:
  std::map<uint16_t, std::vector<PIInstruction>> instructions_by_opcode;
};
};  // namespace sim
};  // namespace core
};  // namespace lc3

#endif
