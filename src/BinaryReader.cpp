#include "BinaryReader.h"

BinaryReader::BinaryReader(const std::string &path) {
  stream.open(path, std::ifstream::in | std::ifstream::binary);
}
