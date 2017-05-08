#ifndef BINARY_READER_H
#define BINARY_READER_H

#include <string>
#include <fstream>

class BinaryReader {
public:
  BinaryReader(const std::string &path);

  template<typename T>
  T get();
private:
  std::ifstream stream;
};

template<typename T>
T BinaryReader::get() {
  union {
    char raw[sizeof(T)];
    T value;
  } data;

  stream.get(data.raw, sizeof(T));
  return data.value;
}

#endif
