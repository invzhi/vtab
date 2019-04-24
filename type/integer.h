#ifndef VTAB_TYPE_INTEGER_H
#define VTAB_TYPE_INTEGER_H

#include <cstdint>

#include "type/type.h"
#include "type/value.h"

class Integer : public Type {
 public:
  Integer() : Type(TypeID::INTEGER) {}

  bool is_fixed() const override { return true; };
  int32_t fixed_length() const override { return sizeof(int64_t); };

  void SerializeTo(char *dest, const Value &value) const override;
  Value DeserializeFrom(const char *src) const override;
};

#endif