#ifndef VTAB_TABLE_TABLE_ITERATOR_H
#define VTAB_TABLE_TABLE_ITERATOR_H

#include "page/page.h"
#include "table/row_id.h"
#include "table/tuple.h"
#include "table/table_heap.h"

class TableIterator {
 public:
  TableIterator(TableHeap *table_heap, RowID row_id) :
      table_heap_(table_heap), row_id_(row_id) {
    if (row_id_.page_id() != INVALID_PAGE_ID) {
      tuple_ = new Tuple();
      table_heap_->GetTuple(row_id_, *tuple_);
    }
  }
  ~TableIterator() { delete tuple_; }

  bool operator==(const TableIterator &other) const {
    return row_id_.Get() == other.row_id_.Get();
  }
  bool operator!=(const TableIterator &other) const {
    return !(*this == other);
  }

  const Tuple &operator*() { return *tuple_; }
  Tuple *operator->() { return tuple_; }
  TableIterator &operator++();

 private:
  TableHeap *table_heap_;
  RowID row_id_;
  Tuple *tuple_ = nullptr;
};

#endif
