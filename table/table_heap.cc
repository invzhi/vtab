#include "table/table_heap.h"

#include "page/page.h"
#include "table/table_iterator.h"

bool TableHeap::GetTuple(const RowID &row_id, Tuple &tuple) {
  PageID page_id = row_id.page_id();
  auto data_page = static_cast<DataPage *>(buffer_pool_->FetchPage(page_id));
  if (data_page == nullptr)
    return false;

  data_page->RLock();
  bool ok = data_page->GetTuple(row_id.slot_number(), tuple);
  data_page->RUnlock();

  buffer_pool_->UnpinPage(page_id, false);
  return ok;
}

bool TableHeap::InsertTuple(RowID &row_id, const Tuple &tuple) {
  // TODO: remove hard code
  if (28 + tuple.length() > PAGE_SIZE)
    return false;

  auto data_page = static_cast<DataPage *>(buffer_pool_->FetchPage(first_page_id_));
  if (data_page == nullptr) {
    return false;
  }

  int32_t slot_number = -1;
  data_page->WLock();
  while (!data_page->InsertTuple(slot_number, tuple)) {
    PageID next_page_id = data_page->GetNextPageID();

    if (next_page_id != INVALID_PAGE_ID) {
      data_page->WUnlock();
      buffer_pool_->UnpinPage(data_page->id(), false);

      data_page = static_cast<DataPage *>(buffer_pool_->FetchPage(next_page_id));
      if (data_page == nullptr)
        return false;

      data_page->WLock();
    } else {
      auto next_page = static_cast<DataPage *>(buffer_pool_->NewPage());
      if (next_page == nullptr) {
        data_page->WUnlock();
        buffer_pool_->UnpinPage(data_page->id(), false);
        return false;
      }

      data_page->SetNextPageID(next_page->id());
      data_page->WUnlock();

      next_page->WLock();
      next_page->Init(data_page->id());

      buffer_pool_->UnpinPage(data_page->id(), true);
      data_page = next_page;
    }
  }
  row_id.Set(data_page->id(), slot_number);
  data_page->WUnlock();
  buffer_pool_->UnpinPage(data_page->id(), true);
  return true;
}

bool TableHeap::UpdateTuple(const RowID &row_id, const Tuple &tuple) {
  PageID page_id = row_id.page_id();
  auto data_page = static_cast<DataPage *>(buffer_pool_->FetchPage(page_id));
  if (data_page == nullptr)
    return false;

  data_page->WLock();
  bool is_updated = data_page->UpdateTuple(row_id.slot_number(), tuple);
  data_page->WUnlock();

  buffer_pool_->UnpinPage(page_id, is_updated);
  return is_updated;
}

bool TableHeap::DeleteTuple(const RowID &row_id) {
  PageID page_id = row_id.page_id();
  auto data_page = static_cast<DataPage *>(buffer_pool_->FetchPage(page_id));
  if (data_page == nullptr)
    return false;

  data_page->WLock();
  bool is_deleted = data_page->DeleteTuple(row_id.slot_number());
  data_page->WUnlock();

  buffer_pool_->UnpinPage(page_id, is_deleted);
  return true;
}

TableIterator TableHeap::begin() {
  RowID row_id(first_page_id_, 0);
  return TableIterator(this, row_id);
}

TableIterator TableHeap::end() {
  RowID row_id;
  return TableIterator(this, row_id);
}