#include "type/type.h"

#include <stdexcept>

#include "type/integer.h"
#include "type/double.h"
#include "type/text.h"

// types_'s order should be same as enum class TypeID.
Type *Type::types_[] = {
  new Integer(),
  new Double(),
  new Text(),
};

void Type::SerializeTo(char *dest, const Value &value) const {
  throw std::runtime_error("no implementation");
}

Value Type::DeserializeFrom(const char *src) const {
  throw std::runtime_error("no implementation");
}