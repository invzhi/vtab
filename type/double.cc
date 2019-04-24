#include "type/double.h"

#include <cstdint>

void Double::SerializeTo(char *dest, const Value &value) const {
  *reinterpret_cast<double *>(dest) = value.value_.real;
}

Value Double::DeserializeFrom(const char *src) const {
  double value = *reinterpret_cast<const double *>(src);
  return Value(type_id_, value);
}