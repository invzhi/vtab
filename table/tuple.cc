#include "table/tuple.h"

#include <cassert>
#include <cstdint>

#include "spdlog/spdlog.h"

Tuple::Tuple(const Schema *schema, const std::vector<Value> values) : allocated_(true) {
  assert(schema->num_columns() == values.size());

  length_ = schema->fixed_length();
  for (const int &i : schema->variable_columns()) {
    length_ += (values[i].variable_length()) + sizeof(int32_t);
  }
  data_ = new char[length_];

  int32_t offset = schema->fixed_length();
  for (size_t i = 0; i < values.size(); ++i) {
    // TODO: change to uniform serialize
    if (schema->is_fixed(i)) {
      values[i].SerializeTo(data_ + schema->offset(i));
    } else {
      *reinterpret_cast<int32_t *>(data_ + schema->offset(i)) = offset;
      values[i].SerializeTo(data_ + offset);
      offset += sizeof(int32_t) + values[i].variable_length();
    }
  }
}

