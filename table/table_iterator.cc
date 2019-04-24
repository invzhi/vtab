#include "table/table_iterator.h"

#include <cassert>

#include "table/row_id.h"
#include "page/page.h"

TableIterator &TableIterator::operator++() {
  assert(row_id_.page_id() != INVALID_PAGE_ID);

  BufferPool *buffer_pool = table_heap_->buffer_pool_;
  auto page = static_cast<DataPage *>(buffer_pool->FetchPage(row_id_.page_id()));
  assert(page);

  row_id_.Set(row_id_.page_id(), row_id_.slot_number() + 1);

  page->RLock();
  while (row_id_.slot_number() == page->GetTupleCount()) {
    row_id_.Set(page->GetNextPageID(), 0);
    page->RUnlock();
    buffer_pool->UnpinPage(page->id(), false);
    if (row_id_.page_id() == INVALID_PAGE_ID) {
      return *this;
    }

    page = static_cast<DataPage *>(buffer_pool->FetchPage(row_id_.page_id()));
    assert(page);
    page->RLock();
  }

  page->RUnlock();
  bool is_get = table_heap_->GetTuple(row_id_, *tuple_);
  assert(is_get);
  buffer_pool->UnpinPage(page->id(), false);
  return *this;
}