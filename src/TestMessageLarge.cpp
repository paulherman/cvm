#include <fstream>
#include <cds/init.h>
#include <iostream>
#include <ctime>
#include "Program.h"

#define MESSAGE_SIZE 65536

int main(int argc, char **argv) {
  cds::Initialize();

  std::vector<Instr> instructions = {
    Instr(Opcode::Spawn, (int64_t)1),

    Instr(Opcode::TimeStart),

    Instr(Opcode::IConst, (int64_t)0), // counter
    Instr(Opcode::Label, (int64_t)0),

    Instr(Opcode::IConst, (int64_t)1),
    Instr(Opcode::IAdd),
    Instr(Opcode::Dup),

    Instr(Opcode::IConst, (int64_t)MESSAGE_SIZE),
    Instr(Opcode::RefAlloc, (int64_t)4),

    Instr(Opcode::IConst, (int64_t)1),
    Instr(Opcode::Msg),

    Instr(Opcode::Recv),
    Instr(Opcode::Pop),

    Instr(Opcode::IConst, (int64_t)TEST_MAX_NUM_SPAWNS),
    Instr(Opcode::ILt),
    Instr(Opcode::Jnz, (int64_t)0),

    Instr(Opcode::Pop),

    Instr(Opcode::TimeEnd),
    Instr(Opcode::PrintFloat),
    Instr(Opcode::PrintEndl),

    Instr(Opcode::Ret)
  };
  std::shared_ptr<Function> f(new Function(0, 1, std::move(instructions)));

  std::shared_ptr<Function> s(new Function(0, 1, std::move(std::vector<Instr> {
    Instr(Opcode::TimeStart),

    Instr(Opcode::IConst, (int64_t)0), // counter
    Instr(Opcode::Label, (int64_t)0),

    Instr(Opcode::IConst, (int64_t)1),
    Instr(Opcode::IAdd),
    Instr(Opcode::Dup),

    Instr(Opcode::Recv),
    Instr(Opcode::Pop),

    Instr(Opcode::IConst, (int64_t)MESSAGE_SIZE),
    Instr(Opcode::RefAlloc, (int64_t)4),

    Instr(Opcode::IConst, (int64_t)0),
    Instr(Opcode::Msg),

    Instr(Opcode::IConst, (int64_t)TEST_MAX_NUM_SPAWNS),
    Instr(Opcode::ILt),
    Instr(Opcode::Jnz, (int64_t)0),

    Instr(Opcode::Pop),

    Instr(Opcode::TimeEnd),
    Instr(Opcode::PrintFloat),
    Instr(Opcode::PrintEndl),

    Instr(Opcode::Ret)
  })));

  std::vector<std::shared_ptr<Function>> functions = { f, s };

  std::vector<Type *> types = Type::primitives;
  types.push_back(new Type(TypeKind::Struct, { &Type::intType }));

  Program p(std::move(types), std::move(functions));

  p.runPool(2, true);

  cds::Terminate();
  return 0;
}
