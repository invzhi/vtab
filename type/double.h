#ifndef VTAB_TYPE_DOUBLE_H
#define VTAB_TYPE_DOUBLE_H

#include <cstdint>

#include "type/type.h"
#include "type/value.h"

class Double : public Type {
 public:
  Double() : Type(TypeID::DOUBLE) {}

  bool is_fixed() const override { return true; };
  int32_t fixed_length() const override { return sizeof(double); };

  void SerializeTo(char *dest, const Value &value) const override;
  Value DeserializeFrom(const char *src) const override;
};

#endif
