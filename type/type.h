#ifndef VTAB_TYPE_TYPE_H
#define VTAB_TYPE_TYPE_H

#include <cstdint>

#include "type/type_id.h"

class Value;

class Type {
 public:
  Type(TypeID type_id) : type_id_(type_id) {}
  virtual ~Type() {}

  virtual bool is_fixed() const { return false; }
  virtual int32_t fixed_length() const { return 0; }
  virtual void SerializeTo(char *dest, const Value &value) const;
  virtual Value DeserializeFrom(const char *src) const;

  static Type *GetInstance(TypeID type_id) { return types_[int(type_id)]; }

 protected:
  TypeID type_id_;

 private:
  // Singleton types
  static Type *types_[];
};

#endif
