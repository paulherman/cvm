#include <exception>
#include "Const.h"

Const::Const(int64_t i)
  : kind(ConstKind::Int), valueInt(i) {
}

Const::Const(double r)
  : kind(ConstKind::Real), valueReal(r) {
}

Const::Const(std::vector<int64_t> &&ia)
  : kind(ConstKind::IntArr), valueIntArr(ia) {
}

Const::Const(std::vector<double> &&ra)
  : kind(ConstKind::RealArr), valueRealArr(ra) {
}

Const::~Const() {
}

int64_t Const::getInt() {
  return valueInt;
}

double Const::getReal() {
  return valueReal;
}

std::vector<int64_t> &Const::getIntArr() {
  return valueIntArr;
}

std::vector<double> &Const::getRealArr() {
  return valueRealArr;
}

std::string Const::getString() {
  std::vector<int64_t> &chars = getIntArr();
  std::string str(chars.size(), '\0');
  for (size_t i = 0; i < chars.size(); i++)
    str[i] = (char)chars[i];
  return str;
}

Const Const::fromStream(BinaryReader &reader) {
  uint8_t kind = reader.get<uint8_t>();
  switch (kind) {
    case 0: 
      return Const(reader.get<int64_t>());
    case 1:
      return Const(reader.get<double>());
    case 2: {
      size_t length = reader.get<size_t>();
      std::vector<int64_t> elems(length);
      for (size_t i = 0; i < length; i++)
        elems[i] = reader.get<int64_t>();
      return Const(std::move(elems));
    }
    case 3: {
      size_t length = reader.get<size_t>();
      std::vector<double> elems(length);
      for (size_t i = 0; i < length; i++)
        elems[i] = reader.get<double>();
      return Const(std::move(elems));
    }
    default:
      throw std::runtime_error("Unable to parse constant kind");
  }
}

void Const::compile(std::stringstream &buf, IdGen &idGen) {
  label = idGen.generate();

  switch (kind) {
    case ConstKind::Int:
      buf << "int64_t " << label << " = " << getInt() << ";" << std::endl;
      break;
    case ConstKind::Real:
      buf << "double " << label << " = " << getReal() << ";" << std::endl;
      break;
    case ConstKind::IntArr:
      buf << "int64_t " << label << "[] = {";
      for (size_t i = 0; i < getIntArr().size(); i++) {
        if (i > 0) {
          buf << ", ";
        }
        buf << getIntArr().at(i);
      }
      buf << "};" << std::endl;
      break;
    case ConstKind::RealArr:
      buf << "double " << label << "[] = {";
      for (size_t i = 0; i < getRealArr().size(); i++) {
        if (i > 0) {
          buf << ", ";
        }
        buf << getIntArr().at(i);
      }
      buf << "};" << std::endl;
      break;
  }
}
