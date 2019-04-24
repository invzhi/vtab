#include "type/value.h"

#include "type/type_id.h"

Value::Value(const Value &other) {
  type_id_ = other.type_id_;
  if (type_id_ == TypeID::TEXT) {
    variable_length_ = other.variable_length_;
    value_.text = new char[variable_length_];
  } else {
    value_ = other.value_;
  }
}
