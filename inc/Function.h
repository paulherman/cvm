#ifndef FUNCTION_H
#define FUNCTION_H

#include <string>
#include <sstream>
#include <ctime>
#include <vector>
#include <map>
#include "Value.h"
#include "Instr.h"
#include "BinaryReader.h"
#include "Program.h"
#include "Process.h"

class Program;
class Process;

class Function {
public:
  Function(const size_t numArgs, const size_t numLocals, const std::vector<Instr> &&code);

  static std::shared_ptr<Function> fromStream(BinaryReader &reader);

  std::string label;
  
  const size_t numArgs;
  const size_t numLocals;
  const std::vector<Instr> code;

  void compile(std::stringstream &buf, IdGen &idGen, Program *program);
  void prototype(std::stringstream &buf, IdGen &idGen);
  void genLabel(IdGen &idGen);
private:
  void genInstr(const Instr &instr, std::stringstream &buf, IdGen &idGen, Program *program);
  void genArgList(std::stringstream &buf);
  void genBinaryOp(std::stringstream &buf, IdGen &idGen, const std::string &op, Type *type);
  std::string genStackPop(std::stringstream &buf, IdGen &idGen, Type *type);

  bool labeled;
};

#endif
