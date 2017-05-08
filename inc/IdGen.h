#ifndef ID_GEN_H
#define ID_GEN_H

#include <string>

class IdGen {
public:
  std::string generate();
  IdGen();
private:
  size_t nextId;
};

#endif
