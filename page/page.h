#ifndef VTAB_PAGE_PAGE_H
#define VTAB_PAGE_PAGE_H

#include "config.h"

using PageID = int32_t;

class Page {
  friend class BufferPool;
 public:
  PageID page_id() const { return page_id_; }
  char *data() { return data_; }

 private:
  PageID page_id_;
  int pin_count_ = 0;
  bool is_dirty_ = false;
  char data_[PAGE_SIZE];
};

#endif