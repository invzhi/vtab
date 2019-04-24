#include "table/tuple.h"

#include <cassert>
#include <cstdint>

#include "type/value.h"

Tuple::Tuple(const Schema *schema, const std::vector<Value> values) : allocated_(true) {
  assert(schema->size() == values.size());

  length_ = schema->GetFixedLength();
  for (const int &i : schema->variable_columns()) {
    length_ += (values[i].variable_length()) + sizeof(int32_t);
  }
  allocated_ = true;
  data_ = new char[length_];

  int32_t offset = schema->GetFixedLength();
  for (size_t i = 0; i < values.size(); ++i) {
    // TODO: change to uniform serialize
    if (schema->IsFixed(i)) {
      values[i].SerializeTo(data_ + schema->GetOffset(i));
    } else {
      *reinterpret_cast<int32_t *>(data_ + schema->GetOffset(i)) = offset;
      values[i].SerializeTo(data_ + offset);
      offset += sizeof(int32_t) + values[i].variable_length();
    }
  }
}

// Tuple::Tuple(const Tuple &other) : length_(other.length_), allocated_(other.allocated_) {
//   if (allocated_ == true) {
//     data_ = new char[length_];
//     std::memcpy(data_, other.data_, length_);
//   } else {
//     data_ = other.data_;
//   }
// }

// Tuple &Tuple::operator=(const Tuple &other) {
//   allocated_ = other.allocated_;
//   length_ = other.length_;
//   if (allocated_ == true) {
//     data_ = new char[length_];
//     std::memcpy(data_, other.data_, length_);
//   } else {
//     data_ = other.data_;
//   }
//   return *this;
// }

Value Tuple::GetValue(Schema *schema, const int column_id) {
  TypeID type_id = schema->GetTypeID(column_id);

  int32_t offset = schema->GetOffset(column_id);
  if (!schema->IsFixed(column_id)) {
    offset = *reinterpret_cast<int32_t *>(data_ + offset);
  }
  return Value::DeserializeFrom(data_ + offset, type_id);
}

