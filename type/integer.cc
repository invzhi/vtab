#include "type/integer.h"

#include <cstdint>

void Integer::SerializeTo(char *dest, const Value &value) const {
  *reinterpret_cast<int64_t *>(dest) = value.value_.integer;
}