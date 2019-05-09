#include "table/table_heap.h"

#include <cassert>

#include "page/page.h"
#include "table/table_iterator.h"

bool TableHeap::GetTuple(const RowID &row_id, Tuple &tuple) {
  PageID page_id = row_id.GetPageID();
  auto data_page = static_cast<DataPage *>(buffer_pool_->FetchPage(page_id));
  if (data_page == nullptr)
    return false;

  data_page->RLock();
  bool ok = data_page->GetTuple(row_id.GetSlotNum(), tuple);
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
      buffer_pool_->UnpinPage(data_page->GetID(), false);

      data_page = static_cast<DataPage *>(buffer_pool_->FetchPage(next_page_id));
      if (data_page == nullptr)
        return false;

      data_page->WLock();
    } else {
      auto next_page = static_cast<DataPage *>(buffer_pool_->NewPage());
      if (next_page == nullptr) {
        data_page->WUnlock();
        buffer_pool_->UnpinPage(data_page->GetID(), false);
        return false;
      }

      data_page->SetNextPageID(next_page->GetID());
      data_page->WUnlock();

      next_page->WLock();
      next_page->Init(data_page->GetID());

      buffer_pool_->UnpinPage(data_page->GetID(), true);
      data_page = next_page;
    }
  }
  row_id.Set(data_page->GetID(), slot_number);
  data_page->WUnlock();
  buffer_pool_->UnpinPage(data_page->GetID(), true);
  return true;
}

bool TableHeap::UpdateTuple(const RowID &row_id, const Tuple &tuple) {
  PageID page_id = row_id.GetPageID();
  auto data_page = static_cast<DataPage *>(buffer_pool_->FetchPage(page_id));
  if (data_page == nullptr)
    return false;

  data_page->WLock();
  bool is_updated = data_page->UpdateTuple(row_id.GetSlotNum(), tuple);
  data_page->WUnlock();

  buffer_pool_->UnpinPage(page_id, is_updated);
  return is_updated;
}

bool TableHeap::MarkDelete(const RowID &row_id) {
  PageID page_id = row_id.GetPageID();
  auto data_page = static_cast<DataPage *>(buffer_pool_->FetchPage(page_id));
  if (data_page == nullptr)
    return false;

  data_page->WLock();
  bool is_deleted = data_page->MarkDelete(row_id.GetSlotNum());
  data_page->WUnlock();

  buffer_pool_->UnpinPage(page_id, is_deleted);
  return true;
}

void TableHeap::Drop() {
  PageID page_id = first_page_id_;
  DataPage *data_page = nullptr;

  while (page_id != INVALID_PAGE_ID) {
    data_page = static_cast<DataPage *>(buffer_pool_->FetchPage(page_id));
    if (data_page == nullptr)
      return;
    data_page->RLock();
    PageID next_page_id = data_page->GetNextPageID();
    data_page->RUnlock();

    buffer_pool_->UnpinPage(page_id, false);
    buffer_pool_->DeletePage(page_id);
    page_id = next_page_id;
  }
}

TableIterator TableHeap::begin() {
  PageID page_id = first_page_id_;
  auto page = static_cast<DataPage *>(buffer_pool_->FetchPage(page_id));
  assert(page);

  int32_t slot_num;
  page->RLock();
  while (!page->GetFirstSlotNum(slot_num)) {
    page_id = page->GetNextPageID();
    page->RUnlock();
    buffer_pool_->UnpinPage(page->GetID(), false);

    if (page_id == INVALID_PAGE_ID)
      return TableIterator(this, RowID());
    page = static_cast<DataPage *>(buffer_pool_->FetchPage(page_id));
    assert(page);

    page->RLock();
  }
  page->RUnlock();
  buffer_pool_->UnpinPage(page->GetID(), false);

  return TableIterator(this, RowID(page_id, slot_num));
}

TableIterator TableHeap::end() {
  return TableIterator(this, RowID());
}