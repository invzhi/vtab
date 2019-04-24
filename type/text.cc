#include "type/text.h"

#include <cstdint>
#include <cstring>

void Text::SerializeTo(char *dest, const Value &value) const {
  int32_t variable_length = value.variable_length_;
  std::memcpy(dest, &variable_length, sizeof(int32_t));
  std::memcpy(dest, value.value_.text, variable_length);
}

Value Text::DeserializeFrom(const char *src) const {
  int32_t length = *reinterpret_cast<const int32_t *>(src);
  return Value(type_id_, src + sizeof(int32_t), length);
}