#include "catalog/schema.h"

Schema::Schema(const std::vector<Column> &columns) {
  int32_t offset = 0;
  for (size_t i = 0; i < columns.size(); ++i) {
    columns_.push_back(std::move(columns[i]));

    offsets_.push_back(offset);
    offset += columns_[i].fixed_length_;

    if (!columns_[i].is_fixed_) {
      is_fixed_ = false;
      variable_columns_.push_back(i);
    }
  }
  fixed_length_ = offset;
}
