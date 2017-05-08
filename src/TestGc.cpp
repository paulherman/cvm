#include <fstream>
#include <cds/init.h>
#include <iostream>
#include <ctime>
#include "Program.h"

int main(int argc, char **argv) {
  cds::Initialize();

  std::vector<Instr> instructions = {
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Ret)
  };
  std::shared_ptr<Function> f(new Function(0, 0, std::move(instructions)));

  std::shared_ptr<Function> s(new Function(0, 1, std::move(std::vector<Instr> {
    Instr(Opcode::IConst, (int64_t)1024 * 1024 / 4 / 4),
    Instr(Opcode::RefAlloc, (int64_t)4),
    Instr(Opcode::Pop),
    Instr(Opcode::Yield),
    Instr(Opcode::Ret)
  })));

  std::vector<std::shared_ptr<Function>> functions = { f, s };

  std::vector<Type *> types = Type::primitives;
  types.push_back(new Type(TypeKind::Struct, { &Type::intType }));

  Program p(std::move(types), std::move(functions));

  p.runPool(8, true);

  cds::Terminate();
  return 0;
}
