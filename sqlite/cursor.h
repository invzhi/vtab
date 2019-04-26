#ifndef VTAB_SQLITE_CURSOR_H
#define VTAB_SQLITE_CURSOR_H

#include "table/table_iterator.h"

class Cursor {
 public:
  Cursor(VirtualTable *virtual_table) :
      table_iterator_(virtual_table->begin()) {}

  VirtualTable *GetVirtualTable() {
    return reinterpret_cast<VirtualTable *>(base_.pVtab);
  }

  int64_t GetRowID() { return table_iterator_.GetRowID(); }

  Value GetValue(Schema *schema, int N) { return table_iterator_->GetValue(schema, N); }

  void Next() { ++table_iterator_; }

  bool IsEOF() { return table_iterator_ == GetVirtualTable()->end(); }

 private:
  sqlite3_vtab_cursor base_;

  TableIterator table_iterator_;
};

#endif