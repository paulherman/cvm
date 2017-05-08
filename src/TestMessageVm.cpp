#include <fstream>
#include <cds/init.h>
#include <iostream>
#include <ctime>
#include "Program.h"

int main(int argc, char **argv) {
  cds::Initialize();

  std::vector<Instr> instructions = {
    Instr(Opcode::IConst, (int64_t)TEST_MAX_NUM_SPAWNS),
    Instr(Opcode::Store, (int64_t)0), // maxReps = 0

    Instr(Opcode::IConst, (int64_t)0), // counter

    Instr(Opcode::Label, (int64_t)0),
    Instr(Opcode::Spawn, (int64_t)1),

    Instr(Opcode::IConst, (int64_t)1),
    Instr(Opcode::IAdd),
    Instr(Opcode::Dup),

    Instr(Opcode::Dup),
    Instr(Opcode::Dup),
    Instr(Opcode::Msg),

    Instr(Opcode::Load, (int64_t)0),
    Instr(Opcode::IEq),
    Instr(Opcode::Jz, (int64_t)0),

    Instr(Opcode::Pop),

    Instr(Opcode::Ret)
  };
  std::shared_ptr<Function> f(new Function(0, 1, std::move(instructions)));

  std::shared_ptr<Function> s(new Function(0, 0, std::move(std::vector<Instr> {
    Instr(Opcode::Recv),
    Instr(Opcode::PrintInt),
    Instr(Opcode::PrintEndl),
    Instr(Opcode::Ret)
  })));

  std::vector<std::shared_ptr<Function>> functions = { f, s };

  std::vector<Type *> types = Type::primitives;

  Program p(std::move(types), std::move(functions));

  p.runPool(8, true);

  cds::Terminate();
  return 0;
}
