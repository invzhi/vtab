#ifndef VTAB_CATALOG_SCHEMA_H
#define VTAB_CATALOG_SCHEMA_H

#include <cstdint>
#include <vector>

#include "type/type_id.h"
#include "catalog/column.h"

class Schema {
 public:
  Schema(const std::vector<Column> &columns);

  TypeID GetTypeID(int i) const { return columns_[i].type_id_; }
  int32_t GetFixedLength() const { return fixed_length_; }
  bool IsFixed(size_t i) const { return columns_[i].is_fixed_; }
  int32_t GetOffset(int i) const { return offsets_[i]; }
  size_t size() const { return columns_.size(); }
  const std::vector<int> &variable_columns() const { return variable_columns_; }

 private:
  bool is_fixed_ = true;
  int32_t fixed_length_;
  std::vector<Column> columns_;
  std::vector<int32_t> offsets_;
  std::vector<int> variable_columns_;
};

#endif