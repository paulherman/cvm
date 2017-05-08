#include <cassert>
#include "Function.h"

Function::Function(const size_t numArgs, const size_t numLocals, const std::vector<Instr> &&code)
  : numArgs(numArgs), numLocals(numLocals), code(code) {
}

void Function::compile(std::stringstream &buf, IdGen &idGen, Program *program) {
  if (!labeled) {
    label = idGen.generate();
    labeled = true;
  }

  buf << "void " << label << "(";
  genArgList(buf);
  buf << ") {" << std::endl;
  buf << "process->locals.push_back(std::vector<Value>(" << numLocals << "));" << std::endl;

  for (size_t i = 0; i < code.size(); i++) {
    genInstr(code[i], buf, idGen, program);
  }

  buf << "}" << std::endl;
}

void Function::prototype(std::stringstream &buf, IdGen &idGen) {
  if (!labeled) {
    label = idGen.generate();
    labeled = true;
  }

  buf << "void " << label << "(";
  genArgList(buf);
  buf << ");" << std::endl;
}

void Function::genArgList(std::stringstream &buf) {
  buf << "Process *process";
}

void Function::genLabel(IdGen &idGen) {
  if (!labeled) {
    labeled = true;
    label = idGen.generate();
  }
}

void Function::genInstr(const Instr &instr, std::stringstream &buf, IdGen &idGen, Program *program) {
  switch (instr.opcode) {
    case Opcode::RefAlloc: {
      size_t typeId = instr.valueInt;
      assert(typeId >= Type::primitives.size() && "Can only allocate structure types");
      std::string typeLabel = idGen.generate();
      std::string countLabel = genStackPop(buf, idGen, &Type::intType);

      buf << "Type *" << typeLabel << " = process->program->types[" << typeId << "];" << std::endl;
      buf << "process->stack.push_back(Value(process->alloc(" << typeLabel << ", " << countLabel << "), " << typeLabel << "));" << std::endl;
      break;
    }
    case Opcode::RefLoad: {
      size_t typeId = instr.valueRef.type;
      size_t member = instr.valueRef.member;

      Type *type = program->types[typeId];
      Type *memberType = type->subtypes[member];
      size_t typeSize = type->size;

      std::string index = genStackPop(buf, idGen, &Type::intType); 
      std::string base = genStackPop(buf, idGen, type); 

      std::string value = idGen.generate();
      buf << memberType->label << " " << value << " = *(" << memberType->label << ")(" << base << " + " << index << " * " << typeSize << " + " << type->getMemberOffset(member) << ");" << std::endl;

      if (memberType->isPrimitive()) {
        buf << "process->stack.push_back(Value(" << value << "))" << std::endl;
      } else if (memberType->isStruct()) {
        buf << "process->stack.push_back(Value((void *)" << value << ", process->program->types[" << memberType->id << "];" << std::endl;
      }
      break;
    }
    case Opcode::RefStore: {
      size_t typeId = instr.valueRef.type;
      size_t member = instr.valueRef.member;

      Type *type = program->types[typeId];
      Type *memberType = type->subtypes[member];
      size_t typeSize = type->size;

      std::string index = genStackPop(buf, idGen, &Type::intType); 
      std::string base = genStackPop(buf, idGen, type); 
      std::string value = genStackPop(buf, idGen, memberType);

      buf << "*(" << memberType << "*)(" << base << " + " << index << " * " << typeSize << " + " << type->getMemberOffset(member) << ") = " << value << ";" << std::endl;
      break;
    }
    case Opcode::Call: {
      size_t functionId = 0;
      std::shared_ptr<Function> function = program->functions[functionId];
      function->genLabel(idGen);
      buf << function->label << "(process);" << std::endl;
      break;
    }
    case Opcode::Msg: {
      std::string pid = genStackPop(buf, idGen, &Type::intType);
      std::string val = genStackPop(buf, idGen, &Type::noneType);
      buf << "{" << std::endl;
      buf << "cds::container::SkipListMap<cds::urcu::gc< cds::urcu::general_buffered<>>, size_t, Process *>::rcu_lock lock;" << std::endl;
      buf << "cds::container::SkipListMap<cds::urcu::gc< cds::urcu::general_buffered<>>, size_t, Process *>::raw_ptr recv = process->program->processMap.get(" << pid << ");" << std::endl;
      buf << "if (recv) recv->second->receive(process, " << val << ");" << std::endl;
      buf << "}" << std::endl;
      break;
    }
    case Opcode::Spawn: 
      buf << "process->program->spawn(" << instr.valueInt << ");" << std::endl;
      break;
    case Opcode::Recv:
      buf << "process->wait();" << std::endl;
      break;
    case Opcode::Load: 
      buf << "process->stack.push_back(process->locals.back()[" << instr.valueInt << "]);" << std::endl;
      break;
    case Opcode::Store:
      buf << "process->locals.back()[" << instr.valueInt << "] = process->stack.back();" << std::endl;
      buf << "process->stack.pop_back();" << std::endl;
      break;
    case Opcode::Jz: 
      buf << "if (" << genStackPop(buf, idGen, &Type::intType) << " == 0) goto lab" << instr.valueInt << ";" << std::endl;
      break;
    case Opcode::Jnz: 
      buf << "if (" << genStackPop(buf, idGen, &Type::intType) << " != 0) goto lab" << instr.valueInt << ";" << std::endl;
      break;
    case Opcode::TimeStart:
      buf << "process->timeStart = std::clock();" << std::endl;
      break;
    case Opcode::TimeEnd:
      buf << "process->stack.push_back(Value(double(std::clock() - process->timeStart) / CLOCKS_PER_SEC));" << std::endl;
      break;
    case Opcode::DebugStack:
      buf << "process->debugStack();" << std::endl;
      break;
    case Opcode::IConst:
      buf << "process->stack.push_back(Value((int64_t)" << instr.valueInt << "));" << std::endl;
      break;
    case Opcode::FConst:
      buf << "process->stack.push_back(Value((double)" << instr.valueReal << "));" << std::endl;
      break;
    case Opcode::Dup:
      buf << "process->stack.push_back(process->stack.back());" << std::endl;
      break;
    case Opcode::Pop:
      buf << "process->stack.pop_back();" << std::endl;
      break;
    case Opcode::IToF:
      buf << "process->stack.back().realValue = (double)process->stack.back().intValue;" << std::endl;
      buf << "process->stack.back().type = &Type::intType;" << std::endl;
      break;
    case Opcode::FToI:
      buf << "process->stack.back().intValue = (int64_t)process->stack.back().realValue;" << std::endl;
      buf << "process->stack.back().type = Type::realType;" << std::endl;
      break;
    case Opcode::Yield:
      buf << "process->yield();" << std::endl;
      break;
    case Opcode::Ret:
      buf << "process->locals.pop_back();" << std::endl;
      buf << "return;" << std::endl;
      break;
    case Opcode::Jmp:
      buf << "goto lab" << instr.valueInt << ";" << std::endl;
      break;
    case Opcode::Label:
      buf << "lab" << instr.valueInt << ":" << std::endl;
      break;
    case Opcode::PrintChr:
      buf << "std::cout << (char)" << genStackPop(buf, idGen, &Type::intType) << ";" << std::endl;
      break;
    case Opcode::PrintInt:
      buf << "std::cout << " << genStackPop(buf, idGen, &Type::intType) << ";" << std::endl;
      break;
    case Opcode::PrintFloat:
      buf << "std::cout << " << genStackPop(buf, idGen, &Type::realType) << ";" << std::endl;
      break;
    case Opcode::PrintEndl:
      buf << "std::cout << std::endl;" << std::endl;
      break;
    case Opcode::IEq:
      genBinaryOp(buf, idGen, "==", &Type::intType);
      break;
    case Opcode::INeq:
      genBinaryOp(buf, idGen, "!=", &Type::intType);
      break;
    case Opcode::ILt:
      genBinaryOp(buf, idGen, "<", &Type::intType);
      break;
    case Opcode::ILeq:
      genBinaryOp(buf, idGen, "<=", &Type::intType);
      break;
    case Opcode::IAdd:
      genBinaryOp(buf, idGen, "+", &Type::intType);
      break;
    case Opcode::ISub:
      genBinaryOp(buf, idGen, "+", &Type::intType);
      break;
    case Opcode::IMul:
      genBinaryOp(buf, idGen, "*", &Type::intType);
      break;
    case Opcode::IDiv:
      genBinaryOp(buf, idGen, "/", &Type::intType);
      break;
    case Opcode::IMod:
      genBinaryOp(buf, idGen, "%", &Type::intType);
      break;
    case Opcode::AEq:
      genBinaryOp(buf, idGen, "==", &Type::refType);
      break;
    case Opcode::ANeq:
      genBinaryOp(buf, idGen, "!=", &Type::refType);
      break;
    case Opcode::FEq:
      genBinaryOp(buf, idGen, "==", &Type::realType);
      break;
    case Opcode::FNeq:
      genBinaryOp(buf, idGen, "!=", &Type::realType);
      break;
    case Opcode::FLt:
      genBinaryOp(buf, idGen, "<", &Type::realType);
      break;
    case Opcode::FLeq:
      genBinaryOp(buf, idGen, "<=", &Type::realType);
      break;
    case Opcode::FAdd:
      genBinaryOp(buf, idGen, "+", &Type::realType);
      break;
    case Opcode::FSub:
      genBinaryOp(buf, idGen, "+", &Type::realType);
      break;
    case Opcode::FMul:
      genBinaryOp(buf, idGen, "*", &Type::realType);
      break;
    case Opcode::FDiv:
      genBinaryOp(buf, idGen, "/", &Type::realType);
      break;
    case Opcode::Pid:
      buf << "process->stack.push_back(Value((int64_t)process->id));" << std::endl;
      break;
    case Opcode::OpcodeSize:
      break;
  }
}

void Function::genBinaryOp(std::stringstream &buf, IdGen &idGen, const std::string &op, Type *type) {
  std::string rhsLabel = genStackPop(buf, idGen, type);
  std::string lhsLabel = genStackPop(buf, idGen, type);
  buf << "process->stack.push_back(Value((" << type->label << ")(" << lhsLabel << " " << op << " " << rhsLabel << ")));" << std::endl;
}

std::string Function::genStackPop(std::stringstream &buf, IdGen &idGen, Type *type) {
  std::string label = idGen.generate();
  if (type->kind != TypeKind::None) {
    buf << type->label << " " << label << " = (" << type->label << ")process->stack.back()";
    switch (type->kind) {
      case TypeKind::Real:
        buf << ".getReal();";
        break;
      case TypeKind::Struct:
        buf << ".getAddr();";
        break;
      case TypeKind::Int:
        buf << ".getInt();";
        break;
      case TypeKind::None:
        throw std::runtime_error("Unable to generate stack pop for type None");
        break;
    }
  } else {
    buf << "Value " << label << " = process->stack.back();";
  }
  buf << std::endl << "process->stack.pop_back();" << std::endl;
  return label;
}
