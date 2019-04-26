#include "type/text.h"

#include <cstdint>
#include <cstring>

void Text::SerializeTo(char *dest, const Value &value) const {
  *reinterpret_cast<int32_t *>(dest) = value.variable_length();
  std::memcpy(dest + sizeof(int32_t), value.value_.text, value.variable_length());
}

Value Text::DeserializeFrom(const char *src) const {
  int32_t length = *reinterpret_cast<const int32_t *>(src);
  return Value(type_id_, src + sizeof(int32_t), length);
}