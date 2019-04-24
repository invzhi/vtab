#ifndef VTAB_TABLE_TUPLE_H
#define VTAB_TABLE_TUPLE_H

#include <cstdint>

#include "catalog/schema.h"
#include "type/value.h"

class Tuple {
 public:
  Tuple() = default;
  Tuple(const Schema *schema, const std::vector<Value> values);
  ~Tuple() {
    if (allocated_)
      delete[] data_;
  }

  // Tuple(const Tuple &other);
  // Tuple &operator=(const Tuple &other);

  int32_t length() const { return length_; }
  const char *data() const { return data_; }

  Value GetValue(Schema *schema, const int column_id);

  void SetData(const void *src, int32_t length) {
    length_ = length;
    if (allocated_)
      delete[] data_;
    data_ = new char[length];
    allocated_ = true;
    std::memcpy(data_, src, length);
  }

 private:
  int32_t length_ = 0;
  char *data_ = nullptr;
  bool allocated_ = false;
};

#endif