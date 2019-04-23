#ifndef VTAB_TABLE_ROW_ID_H
#define VTAB_TABLE_ROW_ID_H

#include <cstdint>

#include "page/page.h"

class RowID {
 public:
  RowID(int64_t row_id) : page_id_(row_id >> 32), slot_number_(row_id) {}

  PageID page_id() const { return page_id_; }
  int32_t slot_number() const { return slot_number_; }

  void set_page_id(PageID page_id) { page_id_ = page_id; }
  void set_slot_number(int32_t slot_number) { slot_number_ = slot_number; }

 private:
  PageID page_id_ = INVALID_PAGE_ID;
  int32_t slot_number_;
};

#endif