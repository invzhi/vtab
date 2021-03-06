#ifndef VTAB_TYPE_VALUE_H
#define VTAB_TYPE_VALUE_H

#include <cstdint>
#include <cstring>
#include <string>

#include "type/type_id.h"
#include "type/type.h"

class Value {
  // access value_ for serialize
  friend class Integer;
  friend class Double;
  friend class Text;

 public:
  Value(TypeID type_id, int64_t value) : type_id_(type_id) {
    value_.integer = value;
  }

  Value(TypeID type_id, double value) : type_id_(type_id) {
    value_.real = value;
  }

  Value(TypeID type_id, const char *src, int32_t length) : type_id_(type_id) {
    variable_length_ = length;
    value_.text = new char[variable_length_];
    std::memcpy(value_.text, src, variable_length_);
  }
  Value(TypeID type_id, const std::string str) : type_id_(type_id) {
    variable_length_ = str.length() + 1;
    value_.text = new char[variable_length_];
    std::memcpy(value_.text, str.c_str(), variable_length_);
  }

  ~Value() {
    if (type_id_ == TypeID::TEXT) {
      delete[] value_.text;
    }
  }

  Value(const Value &other);
  // Value &operator=(Value other);

  int32_t variable_length() const { return variable_length_; }

  void SerializeTo(char *dest) const {
    Type::GetInstance(type_id_)->SerializeTo(dest, *this);
  }

  static Value DeserializeFrom(const char *src, const TypeID type_id) {
    return Type::GetInstance(type_id)->DeserializeFrom(src);
  }

  template <class T> inline T GetAs() const {
    return *reinterpret_cast<const T *>(&value_);
  }
 
 private:
  TypeID type_id_;
  union {
    int64_t integer;
    double real;
    char *text;
  } value_;
  int32_t variable_length_ = 0;
};

#endif