#ifndef VTAB_PAGE_DATA_PAGE_H
#define VTAB_PAGE_DATA_PAGE_H

#include <cstdint>

#include "page/page.h"
#include "table/row_id.h"
#include "table/tuple.h"

class DataPage : public Page {
 public:
  void Init(PageID prev_page_id);

  bool GetFirstSlotNum(int32_t &slot_num);
  bool GetNextSlotNum(int32_t &slot_num);

  bool GetTuple(const int32_t slot_number, Tuple &tuple);
  bool InsertTuple(int32_t &slot_number, const Tuple &tuple);
  bool MarkDelete(const int32_t slot_number);
  bool UpdateTuple(const int32_t slot_number, const Tuple &tuple);

  PageID GetNextPageID() {
    return *reinterpret_cast<PageID *>(GetData() + 8);
  }

  void SetNextPageID(PageID next_page_id) {
    *reinterpret_cast<PageID *>(GetData() + 8) = next_page_id;
  }

  int32_t GetTupleCount() {
    return *reinterpret_cast<int32_t *>(GetData() + 16);
  }
  
 private:
  PageID GetPrevPageID() {
    return *reinterpret_cast<PageID *>(GetData() + 4);
  }

  int32_t GetFreePointer() {
    return *reinterpret_cast<int32_t *>(GetData() + 12);
  }

  int32_t GetTupleOffset(int32_t i) {
    return *reinterpret_cast<int32_t *>(GetData() + 20 + i*8);
  }

  int32_t GetTupleSize(int32_t i) {
    return *reinterpret_cast<int32_t *>(GetData() + 24 + i*8);
  }

  int32_t GetFreeSpace() {
    return GetFreePointer() - 20 - GetTupleCount()*8;
  }

  void SetPrevPageID(PageID prev_page_id) {
    *reinterpret_cast<PageID *>(GetData() + 4) = prev_page_id;
  }

  void SetFreePointer(int32_t free_pointer) {
    *reinterpret_cast<int32_t *>(GetData() + 12) = free_pointer;
  }

  void SetTupleCount(int32_t tuple_count) {
    *reinterpret_cast<int32_t *>(GetData() + 16) = tuple_count;
  }

  void SetTupleOffset(int32_t i, int32_t tuple_offset) {
    *reinterpret_cast<int32_t *>(GetData() + 20 + i*8) = tuple_offset;
  }

  void SetTupleSize(int32_t i, int32_t tuple_size) {
    *reinterpret_cast<int32_t *>(GetData() + 24 + i*8) = tuple_size;
  }
};

#endif