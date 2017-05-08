#include <sstream>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <dlfcn.h>
#include <cstdio>
#include "Program.h"

Program::Program(const std::vector<Type *> &&types, const std::vector<std::shared_ptr<Function>> &&functions)
  : types(types), functions(functions), nextPid(0), numProcesses(0) {

  size_t typeId = 0;
  for (Type *t : types)
    t->id = typeId++;

  std::stringstream buf;
  IdGen idGen;

  buf << "#include \"" << BUILD_DIR << "inc/Type.h\"" << std::endl;
  buf << "#include \"" << BUILD_DIR << "inc/Process.h\"" << std::endl;
  buf << "#include \"" << BUILD_DIR << "inc/Program.h\"" << std::endl;
  buf << "#include \"" << BUILD_DIR << "inc/Function.h\"" << std::endl;
  buf << "#include <iostream>" << std::endl;

  buf << "extern \"C\" {" << std::endl;
  
  for (Type *t : types) {
    t->compile(buf, idGen);
  }

  for (std::shared_ptr<Function> f : functions) {
    f->compile(buf, idGen, this);
  }

  buf << "}" << std::endl;

  mkstemps(sourcePath, 4);
  mkstemps(inferiorPath, 3);

  std::ofstream sourceFile(sourcePath);
  sourceFile << buf.str();
  DEBUG("%s\n", buf.str().c_str());
  sourceFile.close();

  std::stringstream cmd;
  cmd << "g++ -std=c++11 -fpic -shared -g " << sourcePath << " -o " << inferiorPath;
  system(cmd.str().c_str());

  inferior = dlopen(inferiorPath, RTLD_LAZY | RTLD_LOCAL);
}

Program::~Program() {
  dlclose(inferior);
  remove(inferiorPath);
  remove(sourcePath);

  for (size_t i = Type::primitives.size(); i < types.size(); i++)
    delete types[i];
}

CompiledFunction Program::getFunction(size_t index) {
  return (CompiledFunction)dlsym(inferior, functions[index]->label.c_str());
}

void Program::run(size_t workerId) {
  cds::threading::Manager::attachThread();

  Process *p = nullptr;
  while (true) {
    if (activeProcesses.pop(p)) {
      RunStatus ps = p->run();
      if (ps == RunStatus::Yield) {
        p->collect();
        activeProcesses.push(p);
      } else if (ps == RunStatus::Wait) {
        p->collect();
      } else if (ps == RunStatus::Done) {
        numProcesses.fetch_sub(1);
        processMap.erase(p->id);
        delete p;

        if (activeProcesses.empty() || numProcesses.load() == 0)
          break;
      } else if (activeProcesses.empty() || numProcesses.load() == 0)
        break;
    }
  }

  cds::threading::Manager::detachThread();
}

void Program::spawn(size_t functionId) {
  numProcesses.fetch_add(1);
  Process *process = new Process(nextPid.fetch_add(1), this, functionId);
  processMap.insert(process->id, process);
  activeProcesses.push(process);
}

double Program::runPool(size_t numWorkers, bool spawnMain) {
  assert(numWorkers > 0);
  cds::threading::Manager::attachThread();

  if (spawnMain) 
    spawn(0);

  std::clock_t init = std::clock();
  std::vector<std::thread> workers;
  for (size_t workerId = 0; workerId < numWorkers; workerId++) {
    workers.push_back(std::thread(runProgram, this, workerId));
  }

  for (std::thread &worker: workers)
    worker.join();

  std::clock_t end = std::clock();
  double totalTime = double(end - init) / CLOCKS_PER_SEC;

  cds::threading::Manager::detachThread();
  return totalTime;
}

void runProgram(Program *program, size_t workerId) {
  program->run(workerId);
}
