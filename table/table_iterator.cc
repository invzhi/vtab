#include "table/table_iterator.h"

#include <cassert>

#include "table/row_id.h"
#include "page/page.h"

TableIterator &TableIterator::operator++() {
  PageID page_id = row_id_.GetPageID();
  int32_t slot_num = row_id_.GetSlotNum();
  assert(page_id != INVALID_PAGE_ID);

  BufferPool *buffer_pool = table_heap_->buffer_pool_;
  auto page = static_cast<DataPage *>(buffer_pool->FetchPage(page_id));
  assert(page);

  page->RLock();
  if (!page->GetNextSlotNum(slot_num)) {
    PageID next_page_id = page->GetNextPageID();
    while (next_page_id != INVALID_PAGE_ID) {
      page->RUnlock();
      buffer_pool->UnpinPage(page_id, false);

      page = static_cast<DataPage *>(buffer_pool->FetchPage(next_page_id));
      page->RLock();
      if (page->GetFirstSlotNum(slot_num))
        break;

      next_page_id = page->GetNextPageID();
    }
    page_id = next_page_id;
  }
  page->RUnlock();

  row_id_.Set(page_id, slot_num);
  if (*this != table_heap_->end()) {
    bool is_get = table_heap_->GetTuple(row_id_, *tuple_);
    assert(is_get);
  }
  // unpin after GetTuple, prevent page from being victim
  buffer_pool->UnpinPage(page->id(), false);
  return *this;
}