#ifndef VTAB_TABLE_ROW_ID_H
#define VTAB_TABLE_ROW_ID_H

#include <cstdint>

#include "page/page.h"

class RowID {
 public:
  RowID() = default;
  RowID(int64_t row_id) : page_id_(row_id >> 32), slot_num_(row_id) {}
  RowID(PageID page_id, int32_t slot_num) :
      page_id_(page_id), slot_num_(slot_num) {}

  int64_t Get() const { return int64_t(page_id_) << 32 | slot_num_; }
  PageID GetPageID() const { return page_id_; }
  int32_t GetSlotNum() const { return slot_num_; }

  void Set(int64_t row_id) {
    page_id_ = row_id >> 32;
    slot_num_ = row_id;
  }

  void Set(PageID page_id, int32_t slot_num) {
    page_id_ = page_id;
    slot_num_ = slot_num;
  }

 private:
  PageID page_id_ = INVALID_PAGE_ID;
  int32_t slot_num_ = 0;
};

#endif