#include "type/type.h"

#include "type/integer.h"
#include "type/double.h"
#include "type/text.h"

// types_'s order should be same as enum class TypeID.
Type *Type::types_[] = {
  new Integer(),
  new Double(),
  new Text(),
};