#include <map>
#include <stack>
#include <iostream>
#include "Process.h"

bool MemoryArea::operator<(const MemoryArea &rhs) const {
  intptr_t lhsAddr = (intptr_t)base;
  intptr_t rhsAddr = (intptr_t)rhs.base;
  return lhsAddr < rhsAddr;
}

bool MemoryArea::operator==(const MemoryArea &rhs) const {
  intptr_t lhsAddr = (intptr_t)base;
  intptr_t rhsAddr = (intptr_t)rhs.base;
  return lhsAddr == rhsAddr;
}

Process::Process(size_t id, Program *program, size_t functionId)
  : program(program), id(id), waiting(false) {
  CompiledFunction functionPtr = program->getFunction(functionId);
  coroutine = std::make_shared<Coroutine<void>::pull_type>(Coroutine<void>::pull_type([this, functionPtr](Coroutine<void>::push_type &parent) {
    this->yield = [&parent] { parent(); };
    this->yield();
    functionPtr(this);
  }));
}

Process::~Process() {
  for (const MemoryArea &ma : allocatedMemory) {
    free(ma.base);
  }
}

void *Process::alloc(Type *type, size_t count) {
  void *ptr = calloc(count, type->size);
  allocatedMemory.insert(MemoryArea { ptr, count, type });
  return ptr;
}

void Process::debugStack() {
  for (const Value &v : stack) {
    switch (v.type->kind) {
      case TypeKind::Int:
        std::cout << v.intValue << std::endl;
        break;
      case TypeKind::Real:
        std::cout << v.realValue << std::endl;
        break;
      case TypeKind::Struct:
        std::cout << v.ptrValue << std::endl;
        break;
      case TypeKind::None:
        std::cout << "none" << std::endl;
        break;
    }
  }
  exit(0);
}

void Process::wait() {
  if (messages.empty()) {
    bool expected = false;
    waiting.compare_exchange_strong(expected, true);
    yield();
  }

  Value v;
  if (!messages.pop(v)) {
    assert(false && "Message inbox has a single consumer");
  }

  stack.push_back(v);
}

void Process::receive(Process *source, Value value) {
  Type *type = value.type;
  
  if (type->isPrimitive()) {
    messages.bounded_push(value);
  } else if (type->isStruct()) {
  }

  bool expected = true;
  if (waiting.compare_exchange_strong(expected, false)) {
    program->activeProcesses.push(this);
  }
}

void Process::collect() {
  std::set<intptr_t> livePointers;

  std::stack<std::pair<intptr_t, Type *>> frontier;

  for (auto it = stack.begin(); it != stack.end(); it++) {
    Value *v = &*it;
    if (v->type->isStruct() && v->getAddr() != 0) {
      frontier.push(std::make_pair((intptr_t)v->ptrValue, v->type));
      livePointers.insert(v->ptrValue);
    }
  }

  for (auto &frame : locals) {
    for (auto it = frame.begin(); it != frame.end(); it++) {
      Value *v = &*it;
      if (v->type->isStruct() && v->getAddr() != 0) {
        frontier.push(std::make_pair((intptr_t)v->ptrValue, v->type));
        livePointers.insert(v->ptrValue);
      }
    }
  }

  std::set<MemoryArea> newAllocatedMemory;
  while (!frontier.empty()) {
    std::pair<intptr_t, Type *> elem = frontier.top();
    frontier.pop();

    intptr_t ptrValue = elem.first;
    Type *type = elem.second;

    MemoryArea fakeMemoryArea = { (void *)ptrValue, 0, type };
    MemoryArea memoryArea = *allocatedMemory.find(fakeMemoryArea);

    newAllocatedMemory.insert(memoryArea);

    for (size_t index = 0; index < memoryArea.count; index++) {
      for (size_t member = 0; member < type->subtypes.size(); member++) {
        Type *subtype = type->subtypes[member];
        if (subtype->isStruct()) {
          intptr_t memberAddr = (intptr_t)memoryArea.base + index * memoryArea.type->size + type->getMemberOffset(member);
          intptr_t *memberPtr = (intptr_t *)memberAddr;
          if (*memberPtr != 0 && newAllocatedMemory.find(MemoryArea { (void *)*memberPtr, 0, nullptr }) == newAllocatedMemory.end()) {
            frontier.push(std::make_pair(*memberPtr, subtype));
          }
        }
      }
    }
  }

  for (const MemoryArea &ma : allocatedMemory) {
    if (newAllocatedMemory.find(ma) == newAllocatedMemory.end()) {
      free(ma.base);
    }
  }
  allocatedMemory.swap(newAllocatedMemory);
}

RunStatus Process::run() {
  if (!coroutine)
    return RunStatus::Done;

  if (coroutine->operator!())
    return RunStatus::Done;

  coroutine->operator()();

  if (waiting.load())
    return RunStatus::Wait;

  if (coroutine->operator!())
    return RunStatus::Done;

  return RunStatus::Yield;
}
