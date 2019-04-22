#ifndef VTAB_TABLE_TUPLE_H
#define VTAB_TABLE_TUPLE_H

#include <cstdint>

#include "catalog/schema.h"
#include "type/value.h"

class Tuple {
  friend class DataPage;

 public:
  Tuple() = default;
  Tuple(const Schema *schema, const std::vector<Value> values);
  ~Tuple() {
    if (allocated_)
      delete[] data_;
  }

  int32_t length() const { return length_; }
  const char *data() const { return data_; }

 private:
  int32_t length_;
  char *data_;
  bool allocated_ = false;
};

#endif