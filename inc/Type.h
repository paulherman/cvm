#ifndef TYPE_H
#define TYPE_H

#include <vector>
#include <memory>
#include <sstream>
#include "IdGen.h"

enum class TypeKind {
  Int,
  Real,
  Struct,
  None
};

class Type {
public:
  Type(TypeKind k);
  Type(TypeKind k, const std::string &label);
  Type(TypeKind k, const std::vector<Type *> &&st);
  Type();

  bool isPrimitive();
  bool isNone();
  bool isStruct();

  size_t size;
  size_t id;
  const TypeKind kind;
  std::vector<Type *> subtypes;
  std::string label;

  size_t getMemberOffset(size_t element);

  static Type intType;
  static Type realType;
  static Type noneType;
  static Type refType;
  static std::vector<Type *> primitives;

  void compile(std::stringstream &buf, IdGen &idGen);
};

#endif
