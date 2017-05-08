#ifndef PROCESS_H
#define PROCESS_H

#include <boost/coroutine2/all.hpp>
#include <boost/lockfree/queue.hpp>
#include <functional>
#include <set>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <atomic>
#include "Spinlock.h"
#include "Program.h"
#include "ConcurrentQueue.h"
#include "Build.h"

class MemoryArea {
public:
  void *base;
  size_t count;
  Type *type;

  bool operator<(const MemoryArea &rhs) const;
  bool operator==(const MemoryArea &rhs) const;
};

class Program;

enum class RunStatus {
  Done,
  Yield,
  Wait
};

template<typename T>
using Coroutine = boost::coroutines2::coroutine<T>;

class Process {
public:
  Process(size_t id, Program *program, size_t functionId);
  void *alloc(Type *type, size_t count);
  void collect();
  void wait();
  void receive(Process *source, Value value);
  RunStatus run();
  void debugStack();
  ~Process();

  std::function<void()> yield;
  boost::lockfree::queue<Value, boost::lockfree::capacity<MAX_NUM_MESSAGES>> messages;
  std::list<Value> stack;
  std::vector<std::vector<Value>> locals;
  Program *program;
  std::clock_t timeStart;
  const size_t id;
private:
  std::atomic<bool> waiting;
  std::shared_ptr<Coroutine<void>::pull_type> coroutine;
  std::set<MemoryArea> allocatedMemory;
};


#endif
