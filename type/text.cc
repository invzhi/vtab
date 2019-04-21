#include "type/text.h"

#include <cstdint>
#include <cstring>

void Text::SerializeTo(char *dest, const Value &value) const {
  int32_t variable_length = value.variable_length_;
  std::memcpy(dest, &variable_length, sizeof(int32_t));
  std::memcpy(dest, value.value_.text, variable_length);
}