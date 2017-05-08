#ifndef PROGRAM_H
#define PROGRAM_H

#include <vector>
#include <memory>
#include <boost/lockfree/queue.hpp>
#include <cds/urcu/general_buffered.h>
#include <cds/container/skip_list_map_rcu.h>
#include "Build.h"
#include "Const.h"
#include "Type.h"
#include "Function.h"
#include "Process.h"

class Function;

using CompiledFunction = void (*)(Process *);

typedef cds::urcu::gc< cds::urcu::general_buffered<> > rcu_type;

void runProgram(Program *program, size_t workerId);

class Program {
public:
  Program(const std::vector<Type *> &&types, const std::vector<std::shared_ptr<Function>> &&functions);
  ~Program();

  static Program fromStream(BinaryReader &reader);

  const std::vector<Type *> types;
  const std::vector<std::shared_ptr<Const>> consts;
  const std::vector<std::shared_ptr<Function>> functions;

  CompiledFunction getFunction(size_t index);
  void run(size_t workerId);
  void spawn(size_t functionId);
  double runPool(size_t numWorkers, bool spawnMain = true);

  boost::lockfree::queue<Process *, boost::lockfree::capacity<MAX_NUM_PROCESSES>> activeProcesses;
  cds::container::SkipListMap<rcu_type, size_t, Process *> processMap;
  rcu_type rcu;

private:
  
  std::atomic_size_t nextPid;
  std::atomic_size_t numProcesses;
  void *inferior;
  char inferiorPath[18] = "/tmp/cvmXXXXXX.so";
  char sourcePath[19] = "/tmp/cvmXXXXXX.cpp";
};

#endif
