#include "table/table_iterator.h"

#include <cassert>

#include "table/row_id.h"
#include "page/page.h"

TableIterator &TableIterator::operator++() {
  PageID page_id = row_id_.GetPageID();
  assert(page_id != INVALID_PAGE_ID);
  BufferPool *buffer_pool = table_heap_->buffer_pool_;
  auto page = static_cast<DataPage *>(buffer_pool->FetchPage(page_id));
  assert(page);

  int32_t slot_num = row_id_.GetSlotNum();
  page->RLock();
  if (!page->GetNextSlotNum(slot_num)) {
    do {
      page_id = page->GetNextPageID();
      page->RUnlock();
      buffer_pool->UnpinPage(page->GetID(), false);

      if (page_id == INVALID_PAGE_ID) {
        row_id_.Set(INVALID_PAGE_ID, 0);
        return *this;
      }
      page = static_cast<DataPage *>(buffer_pool->FetchPage(page_id));
      assert(page);
      page->RLock();
    } while (!page->GetFirstSlotNum(slot_num));
  }
  page->RUnlock();

  row_id_.Set(page_id, slot_num);
  bool is_get = table_heap_->GetTuple(row_id_, *tuple_);
  assert(is_get);
  // unpin after GetTuple, prevent page from being victim
  buffer_pool->UnpinPage(page->GetID(), false);
  return *this;
}