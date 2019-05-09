#ifndef VTAB_PAGE_PAGE_H
#define VTAB_PAGE_PAGE_H

#include <mutex>

#include "config.h"

#define INVALID_PAGE_ID -1
#define HEADER_PAGE_ID   0

using PageID = int32_t;

class Page {
  friend class BufferPool;

 public:
  void RLock() { mutex_.lock(); }
  void RUnlock() { mutex_.unlock(); }
  void WLock() { mutex_.lock(); }
  void WUnlock() { mutex_.unlock(); }

  PageID GetID() const { return id_; }
  char *GetData() { return data_; }

 private:
  PageID id_;
  int pin_count_ = 0;
  bool is_dirty_ = false;

  std::mutex mutex_;
  char data_[PAGE_SIZE];
};

#endif