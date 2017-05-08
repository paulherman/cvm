#include <exception>
#include "Type.h"

Type::Type(TypeKind k) : kind(k) {
  switch (kind) {
    case TypeKind::Int:
      size = sizeof(int64_t);
      break;
    case TypeKind::Real:
      size = sizeof(double);
      break;
    case TypeKind::None:
      size = 0;
      break;
    case TypeKind::Struct:
      size = sizeof(void *);
      break;
  }
}

Type::Type(TypeKind k, const std::vector<Type *> &&st) : kind(k), subtypes(st) {
  if (k != TypeKind::Struct)
    throw std::runtime_error("Compound type must be either struct");

  switch (k) {
    case TypeKind::Struct:
      size = 0;
      for (Type *subtype : subtypes)
        size += subtype->size;
      break;
    default:
      break;
  }
}

Type::Type() : size(0), kind(TypeKind::None) {
}

Type::Type(TypeKind k, const std::string &label)
  : kind(k), label(label) {
}

Type Type::intType(TypeKind::Int);
Type Type::realType(TypeKind::Real);
Type Type::noneType(TypeKind::None);
Type Type::refType(TypeKind::Struct, "void *");
std::vector<Type *> Type::primitives = { &Type::intType, &Type::realType, &Type::noneType, &Type::refType };

void Type::compile(std::stringstream &buf, IdGen &idGen) {
  if (isStruct()) {
    if (!subtypes.empty()) {
      label = idGen.generate();

      buf << "struct " << label << " {" << std::endl;
      for (size_t i = 0; i < subtypes.size(); i++) {
        Type *subtype = subtypes[i];

        if (subtype->isPrimitive()) {
          buf << subtype->label << " f" << i << ";" << std::endl;
        } else {
          buf << subtype->label << " *f" << i << ";" << std::endl;
        }
      }
      buf << "};" << std::endl;
    } else {
      label = "void *";
    }
  } else {
    switch (kind) {
      case TypeKind::Int:
        label = "int64_t";
        break;
      case TypeKind::Real:
        label = "double";
        break;
      case TypeKind::None:
        label = "unk_type";
        break;
      default:
        throw std::runtime_error("Unable to compile type");
        break;
    }
  }
}

bool Type::isPrimitive() {
  return kind == TypeKind::Int || kind == TypeKind::Real;
}

bool Type::isNone() {
  return kind == TypeKind::None;
}

bool Type::isStruct() {
  return kind == TypeKind::Struct;
}

size_t Type::getMemberOffset(size_t element) {
  size_t offset = 0;
  for (size_t i = 0; i < element; i++)
    offset += subtypes[i]->size;
  return offset;
}
