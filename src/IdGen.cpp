#include <sstream>
#include <IdGen.h>

IdGen::IdGen() : nextId(0) {}

std::string IdGen::generate() {
  std::stringstream idStream;
  idStream << "id" << nextId;
  nextId++;
  return idStream.str();
}
