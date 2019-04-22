#include "page/data_page.h"

#include <cstdint>
#include <cstring>

#include "page/page.h"

void DataPage::Init(PageID prev_page_id) {
  // LSN is not use for now
  SetPrevPageID(prev_page_id);
  SetNextPageID(INVALID_PAGE_ID);
  SetFreePointer(PAGE_SIZE);
  SetTupleCount(0);
}

bool DataPage::InsertTuple(RowID &row_id, const Tuple &tuple) {
  if (GetFreeSpace() < tuple.length() + 8)
    return false;

  // data
  int32_t offset = GetFreePointer() - tuple.length();
  SetFreePointer(offset);
  std::memcpy(data() + offset, tuple.data(), tuple.length());
  // header
  int32_t i = GetTupleCount();
  SetTupleOffset(i, offset);
  SetTupleSize(i, tuple.length());
  SetTupleCount(i+1);
  // rowid
  row_id.Set(id(), i);
  return true;
}

bool DataPage::GetTuple(const RowID &row_id, Tuple &tuple) {
  int32_t slot_number = row_id.slot_number();
  if (slot_number >= GetTupleCount())
    return false;
  
  int32_t offset = GetTupleOffset(slot_number);
  int32_t size = GetTupleSize(slot_number);
  tuple.length_ = size;
  if (tuple.allocated_)
    delete[] tuple.data_;
  tuple.data_ = new char[size];
  tuple.allocated_ = true;
  std::memcpy(tuple.data_, data() + offset, size);
  return true;
}

bool DataPage::DeleteTuple(const RowID &row_id) {
  int32_t slot_number = row_id.slot_number();
  if (slot_number >= GetTupleCount())
    return false;

  int32_t offset = GetTupleOffset(slot_number);
  int32_t size = GetTupleSize(slot_number);
  // header
  int32_t tuple_count = GetTupleCount();
  for (int32_t i = slot_number; i + 1 < tuple_count; ++i) {
    SetTupleOffset(i, GetTupleOffset(i + 1) + size);
    SetTupleSize(i, GetTupleSize(i + 1));
  }
  SetTupleCount(--tuple_count);
  // data
  int32_t free_pointer = GetFreePointer();
  std::memmove(data() + free_pointer + size, data() + free_pointer,
               offset - free_pointer);
  SetFreePointer(free_pointer + size);
  return true;
}

bool DataPage::UpdateTuple(const RowID &row_id, const Tuple &tuple) {
  int32_t slot_number = row_id.slot_number();
  if (slot_number >= GetTupleCount())
    return false;

  int32_t offset = GetTupleOffset(slot_number);
  int32_t size = GetTupleSize(slot_number);
  if (tuple.length() == size) {
    std::memcpy(data() + offset, tuple.data(), size);
    return true;
  }
  // data
  int32_t free_pointer = GetFreePointer();
  int32_t shift = size - tuple.length();
  std::memmove(data() + free_pointer + shift,
               data() + free_pointer, offset - free_pointer);
  // header
  SetTupleOffset(slot_number, offset + shift);
  SetTupleSize(slot_number, tuple.length());
  SetFreePointer(free_pointer + shift);

  int32_t tuple_count = GetTupleCount();
  for (int32_t i = slot_number + 1; i < tuple_count; ++i) {
    SetTupleOffset(i, GetTupleOffset(i) + shift);
  }
  return true;
}
