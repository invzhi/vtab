#ifndef VTAB_CATALOG_COLUMN_H
#define VTAB_CATALOG_COLUMN_H

#include <cstdint>

#include "type/type.h"

class Column {
friend class Schema;
 public:
  Column(TypeID type_id);

 private:
  TypeID type_id_;
  bool is_fixed_;
  int32_t fixed_length_;
};

#endif