#include "type/integer.h"

#include <cstdint>

void Integer::SerializeTo(char *dest, const Value &value) const {
  *reinterpret_cast<int64_t *>(dest) = value.value_.integer;
}

Value Integer::DeserializeFrom(const char *src) const {
  int64_t value = *reinterpret_cast<const int64_t *>(src);
  return Value(type_id_, value);
}