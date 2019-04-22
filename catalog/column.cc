#include "catalog/column.h"

#include "type/type_id.h"

Column::Column(TypeID type_id) : type_id_(type_id) {
  Type *type = Type::GetInstance(type_id);

  is_fixed_ = type->is_fixed();
  fixed_length_ = type->fixed_length();
}