#ifndef VALUE_H
#define VALUE_H

#include <memory>
#include "Type.h"

class Value {
public:
  Value(void *p, Type *ty);
  Value(intptr_t p, Type *ty);
  Value(int64_t i);
  Value(double r);
  Value(const Value &other);
  Value();
  ~Value() = default;


  int64_t getInt();
  double getReal();
  intptr_t getAddr();
  void *getPtr();

  Type *type;
  union {
    intptr_t ptrValue;
    double realValue;
    int64_t intValue;
  };
};

#endif
