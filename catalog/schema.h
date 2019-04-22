#ifndef VTAB_CATALOG_SCHEMA_H
#define VTAB_CATALOG_SCHEMA_H

#include <cstdint>
#include <vector>

#include "catalog/column.h"

class Schema {
 public:
  Schema(const std::vector<Column> &columns);

  int32_t fixed_length() const { return fixed_length_; }
  bool is_fixed(size_t i) const { return columns_[i].is_fixed_; }
  int32_t offset(int i) const { return offsets_[i]; }
  size_t num_columns() const { return columns_.size(); }
  const std::vector<int> &variable_columns() const { return variable_columns_; }

 private:
  bool is_fixed_ = true;
  int32_t fixed_length_;
  std::vector<Column> columns_;
  std::vector<int32_t> offsets_;
  std::vector<int> variable_columns_;
};

#endif