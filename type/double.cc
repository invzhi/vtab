#include "type/double.h"

#include <cstdint>

void Double::SerializeTo(char *dest, const Value &value) const {
  *reinterpret_cast<double *>(dest) = value.value_.real;
}