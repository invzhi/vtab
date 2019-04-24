#ifndef VTAB_TABLE_TABLE_HEAP_H
#define VTAB_TABLE_TABLE_HEAP_H

#include "table/row_id.h"
#include "table/tuple.h"
#include "page/page.h"
#include "page/data_page.h"
#include "buffer/buffer_pool.h"

class TableIterator;

class TableHeap {
  friend class TableIterator;

 public:
  TableHeap(BufferPool *buffer_pool, PageID first_page_id)
      : buffer_pool_(buffer_pool), first_page_id_(first_page_id) {}

  // create table
  TableHeap(BufferPool *buffer_pool) : buffer_pool_(buffer_pool) {
    auto data_page = static_cast<DataPage *>(buffer_pool_->NewPage());
    first_page_id_ = data_page->id();
    data_page->Init(INVALID_PAGE_ID);
    buffer_pool_->UnpinPage(first_page_id_, true);
  }

  bool GetTuple(const RowID &row_id, Tuple &tuple);
  bool InsertTuple(RowID &row_id, const Tuple &tuple);
  bool UpdateTuple(const RowID &row_id, const Tuple &tuple);
  bool DeleteTuple(const RowID &row_id);

  TableIterator begin();
  TableIterator end();

  PageID first_page_id() const { return first_page_id_; }

 private:
  BufferPool *buffer_pool_;
  PageID first_page_id_;
};

#endif
