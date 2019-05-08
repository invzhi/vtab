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

bool DataPage::GetFirstSlotNum(int32_t &slot_num) {
  int32_t tuple_count = GetTupleCount();
  for (int32_t i = 0; i < tuple_count; ++i) {
    if (GetTupleSize(i) > 0) {
      slot_num = i;
      return true;
    }
  }
  return false;
}

bool DataPage::GetNextSlotNum(int32_t &slot_num) {
  int32_t tuple_count = GetTupleCount();
  for (int32_t i = slot_num + 1; i < tuple_count; ++i) {
    if (GetTupleSize(i) > 0) {
      slot_num = i;
      return true;
    }
  }
  return false;
}

bool DataPage::InsertTuple(int32_t &slot_number, const Tuple &tuple) {
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

  slot_number = i;
  return true;
}

bool DataPage::GetTuple(const int32_t slot_number, Tuple &tuple) {
  if (slot_number >= GetTupleCount())
    return false;
  
  int32_t offset = GetTupleOffset(slot_number);
  int32_t size = GetTupleSize(slot_number);
  if (size < 0)
    return false;
  tuple.SetData(data() + offset, size);
  return true;
}

bool DataPage::MarkDelete(const int32_t slot_number) {
  if (slot_number >= GetTupleCount())
    return false;

  int32_t tuple_size = GetTupleSize(slot_number);
  SetTupleSize(slot_number, -tuple_size);
  return true;
}

bool DataPage::UpdateTuple(const int32_t slot_number, const Tuple &tuple) {
  if (slot_number >= GetTupleCount())
    return false;

  int32_t offset = GetTupleOffset(slot_number);
  int32_t size = GetTupleSize(slot_number);
  if (size < 0)
    return false;
  if (tuple.length() == size) {
    std::memcpy(data() + offset, tuple.data(), size);
    return true;
  }
  if (GetFreeSpace() < tuple.length() - size)
    return false;
  // data
  int32_t free_pointer = GetFreePointer();
  int32_t shift = size - tuple.length();
  std::memmove(data() + free_pointer + shift,
               data() + free_pointer, offset - free_pointer);
  std::memcpy(data() + offset + shift, tuple.data(), tuple.length());
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
