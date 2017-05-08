#ifndef CONST_H
#define CONST_H

#include <vector>
#include <string>
#include <sstream>
#include "BinaryReader.h"
#include "IdGen.h"

enum class ConstKind {
  Int,
  Real,
  IntArr,
  RealArr
};

class Const {
public:
  Const(int64_t i);
  Const(double r);
  Const(std::vector<int64_t> &&ia);
  Const(std::vector<double> &&ra);
  Const(const Const &c);
  ~Const();

  int64_t getInt();
  double getReal();
  std::string getString();
  std::vector<int64_t> &getIntArr();
  std::vector<double> &getRealArr();

  const ConstKind kind;
  std::string label;

  void compile(std::stringstream &buf, IdGen &idGen);

  static Const fromStream(BinaryReader &reader);
private:

  union {
    int64_t valueInt;
    double valueReal;
    std::vector<int64_t> valueIntArr;
    std::vector<double> valueRealArr;
  };
};

#endif
