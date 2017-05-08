#ifndef INSTR_H 
#define INSTR_H 

#include <cstddef>
#include <cstdint>
#include "BinaryReader.h"

enum class Opcode {
  IConst,
  IAdd,
  ISub,
  IMul,
  IDiv,
  IMod,
  IToF,
  IEq,
  INeq,
  ILt,
  ILeq,
  FConst,
  FAdd,
  FSub,
  FMul,
  FDiv,
  FToI,
  FEq,
  FNeq,
  FLt,
  FLeq,
  AEq,
  ANeq,
  Dup,
  Pop,
  Call,
  Ret,
  Jmp,
  Jz,
  Jnz,
  Label,
  Spawn,
  Yield,
  Pid,
  TimeStart,
  TimeEnd,
  Msg,
  Recv,
  RefAlloc,
  RefLoad,
  RefStore,
  Load,
  Store,
  PrintChr,
  PrintInt,
  PrintFloat,
  PrintEndl,
  DebugStack,
  OpcodeSize
};

struct AddrOp {
  ptrdiff_t offset;
  size_t index;
};

struct RefValue {
  size_t type;
  size_t member;
};

class Instr {
public:
  Instr(Opcode opcode);
  Instr(Opcode opcode, int64_t valueInt);
  Instr(Opcode opcode, double valueReal);
  Instr(Opcode opcode, void *valuePtr);
  Instr(Opcode opcode, RefValue valueRef);
  const Opcode opcode;

  const union {
    int64_t valueInt;
    double valueReal;
    intptr_t valueAddr;
    void *valuePtr;
    RefValue valueRef;
  };

  static Instr fromStream(BinaryReader &reader);
private:
};

#endif
