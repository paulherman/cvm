#include "Instr.h"

Instr::Instr(Opcode opcode)
  : opcode(opcode) {
}

Instr::Instr(Opcode opcode, int64_t valueInt)
  : opcode(opcode), valueInt(valueInt) {
}

Instr::Instr(Opcode opcode, double valueReal)
  : opcode(opcode), valueReal(valueReal) {
}

Instr::Instr(Opcode opcode, void *valuePtr)
  : opcode(opcode), valuePtr(valuePtr) {
}

Instr::Instr(Opcode opcode, RefValue valueRef)
  : opcode(opcode), valueRef(valueRef) {
}

Instr fromStream(BinaryReader &reader) {
  Opcode opcode = static_cast<Opcode>(reader.get<uint8_t>());
  switch (opcode) {
    case Opcode::IConst: {
      int64_t value = reader.get<int64_t>();
      return Instr(opcode, value);
    }
    default:
      return Instr(opcode);
  }
}
