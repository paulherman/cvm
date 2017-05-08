#include "Value.h"

Value::Value(intptr_t p, Type *ty)
  : type(ty), ptrValue(p) {
}

Value::Value(void *p, Type *ty)
  : type(ty), ptrValue((intptr_t)p) {
}

Value::Value(int64_t i)
  : type(&Type::intType), intValue(i) {
}

Value::Value(double r)
  : type(&Type::realType), realValue(r) {
}

Value::Value(const Value &other)
  : type(other.type) {
  switch (type->kind) {
    case TypeKind::Int:
      intValue = other.intValue;
      break;
    case TypeKind::Real:
      realValue = other.realValue;
      break;
    case TypeKind::Struct:
      ptrValue = other.ptrValue;
      break;
    case TypeKind::None:
      break;
  }
}

Value::Value()
  : type(&Type::noneType) {
}

int64_t Value::getInt() {
  return intValue;
}

double Value::getReal() {
  return realValue;
}

intptr_t Value::getAddr() {
  return ptrValue;
}

void *Value::getPtr() {
  return (void *)getAddr();
}
