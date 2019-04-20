#ifndef VTAB_TYPE_TEXT_H
#define VTAB_TYPE_TEXT_H

#include <cstdint>

#include "type/type.h"
#include "type/value.h"

class Text : public Type {
 public:
  // Text() : Type(TypeID::TEXT) {}

  bool is_fixed() const override { return false; };
  int32_t fixed_length() const override { return sizeof(int32_t); };
  void SerializeTo(char *dest, const Value &value) const override;
};

#endif
