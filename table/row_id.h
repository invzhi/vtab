#ifndef VTAB_TABLE_ROW_ID_H
#define VTAB_TABLE_ROW_ID_H

#include <cstdint>

#include "page/page.h"

class RowID {
 public:
  RowID() = default;
  RowID(int64_t row_id) : page_id_(row_id >> 32), slot_number_(row_id) {}
  RowID(PageID page_id, int32_t slot_number) :
      page_id_(page_id), slot_number_(slot_number) {}

  int64_t Get() const {
    return int64_t(page_id_) << 32 | slot_number_;
  }

  void Set(int64_t row_id) {
    page_id_ = row_id >> 32;
    slot_number_ = row_id;
  }
  void Set(PageID page_id, int32_t slot_number) {
    page_id_ = page_id;
    slot_number_ = slot_number;
  }

  PageID page_id() const { return page_id_; }
  int32_t slot_number() const { return slot_number_; }

 private:
  PageID page_id_ = INVALID_PAGE_ID;
  int32_t slot_number_ = 0;
};

#endif