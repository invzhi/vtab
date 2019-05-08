#ifndef VTAB_SQLITE_VIRTUAL_TABLE_H
#define VTAB_SQLITE_VIRTUAL_TABLE_H

#include <sqlite3ext.h>
#include <vector>

#include "page/page.h"
#include "buffer/buffer_pool.h"
#include "catalog/schema.h"
#include "table/table_heap.h"
#include "table/table_iterator.h"

class VirtualTable {
 public:
  // for xCreate
  VirtualTable(BufferPool *buffer_pool, Schema *schema) : schema_(schema) {
    table_heap_ = new TableHeap(buffer_pool);
  }
  // for xConnect
  VirtualTable(BufferPool *buffer_pool, Schema *schema, PageID first_page_id) : schema_(schema) {
    table_heap_ = new TableHeap(buffer_pool, first_page_id);
  }
  ~VirtualTable() {
    delete schema_;
    delete table_heap_;
  }

  TableIterator begin() { return table_heap_->begin(); }
  TableIterator end() { return table_heap_->end(); }

  Schema *schema() { return schema_; }

  bool GetTuple(const RowID &row_id, Tuple &tuple) {
    return table_heap_->GetTuple(row_id, tuple);
  }
  bool InsertTuple(RowID &row_id, const Tuple &tuple) {
    return table_heap_->InsertTuple(row_id, tuple);
  }
  bool UpdateTuple(const RowID &row_id, const Tuple &tuple) {
    return table_heap_->UpdateTuple(row_id, tuple);
  }
  bool DeleteTuple(const RowID &row_id) {
    return table_heap_->MarkDelete(row_id);
  }
  void Drop() { table_heap_->Drop(); }

  PageID GetFirstPageID() const { return table_heap_->first_page_id(); }

 private:
  sqlite3_vtab base_;

  Schema *schema_;
  TableHeap *table_heap_;
};

#endif