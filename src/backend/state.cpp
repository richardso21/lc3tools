/*
 * Copyright 2020 McGraw-Hill Education. All rights reserved. No reproduction or
 * distribution without the prior written consent of McGraw-Hill Education.
 */
#include "state.h"

#include "device.h"
#include "device_regs.h"

using namespace lc3::core;

MachineState::MachineState(void)
    : reset_pc(RESET_PC),
      pc(0),
      ir(0),
      decoded_ir(nullptr),
      ssp(0),
      ignore_privilege(false),
      first_init(true) {
  reinitialize();

  registerDeviceReg(PSR, std::make_shared<RWReg>(PSR));
  registerDeviceReg(MCR, std::make_shared<RWReg>(MCR));
}

void MachineState::reinitialize(void) {
  reset_pc = RESET_PC;
  first_init = true;

  mem.clear();
  mem.resize(USER_END - SYSTEM_START + 1);

  rf.clear();
  rf.resize(16);
}

void MachineState::setIgnorePrivilege(bool ignore_privilege) {
  this->ignore_privilege = ignore_privilege;
}
bool MachineState::getIgnorePrivilege(void) const { return ignore_privilege; }

std::pair<uint16_t, PIMicroOp> MachineState::readMem(uint16_t addr) const {
  if (MMIO_START <= addr && addr <= MMIO_END) {
    auto search = mmio.find(addr);
    if (search != mmio.end()) {
      return search->second->read(addr);
    } else {
      return std::make_pair(0x0000, nullptr);
    }
  } else {
    return std::make_pair(mem[addr].getValue(), nullptr);
  }
}

PIMicroOp MachineState::writeMem(uint16_t addr, uint16_t value) {
  if (MMIO_START <= addr && addr <= MMIO_END) {
    auto search = mmio.find(addr);
    if (search != mmio.end()) {
      return search->second->write(addr, value);
    }
  } else {
    mem[addr].setValue(value);
    // change line with new character if we are storing an ascii value to a line
    // that's part of a .stringz
    if (value <= 127 && getMemLine(addr).length() == 1) {
      char val_char = value;
      std::string val_ascii_to_string;
      val_ascii_to_string += (char)value;
      setMemLine(addr, val_ascii_to_string);
    }
  }

  return nullptr;
}

std::string MachineState::getMemLine(uint16_t addr) const {
  if (addr < MMIO_START) {
    return mem[addr].getLine();
  }

  return "";
}

void MachineState::setMemLine(uint16_t addr, std::string const& value) {
  if (addr < MMIO_START) {
    mem[addr].setLine(value);
  }
}

void MachineState::registerDeviceReg(uint16_t mem_addr, PIDevice device) {
  mmio[mem_addr] = device;
}

InterruptType MachineState::peekInterrupt(void) const {
  if (pending_interrupts.size() == 0) {
    return InterruptType::INVALID;
  }

  return pending_interrupts.front();
}

InterruptType MachineState::dequeueInterrupt(void) {
  if (pending_interrupts.size() == 0) {
    return InterruptType::INVALID;
  }

  InterruptType type = pending_interrupts.front();
  pending_interrupts.pop();
  return type;
}

FuncType MachineState::peekFuncTraceType(void) const {
  if (func_trace.size() == 0) {
    return FuncType::INVALID;
  }

  return func_trace.top();
}

FuncType MachineState::popFuncTraceType(void) {
  if (func_trace.size() == 0) {
    return FuncType::INVALID;
  }

  FuncType type = func_trace.top();
  func_trace.pop();
  return type;
}
