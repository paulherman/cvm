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
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
    Instr(Opcode::Spawn, (int64_t)1),
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


  std::vector<Instr> yieldInstrs;
  for (size_t i = 0; i < 512; i++) {
    yieldInstrs.push_back(Instr(Opcode::Yield));
  }
  yieldInstrs.push_back(Instr(Opcode::Ret));

  std::shared_ptr<Function> s(new Function(0, 0, std::move(yieldInstrs)));

  std::vector<std::shared_ptr<Function>> functions = { f, s };

  std::vector<Type *> types = Type::primitives;
  types.push_back(new Type(TypeKind::Struct, { &Type::intType }));

  Program p(std::move(types), std::move(functions));

  std::cout << "Total time " << p.runPool(1, true) << std::endl;

  cds::Terminate();
  return 0;
}
