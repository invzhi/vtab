#ifndef VTAB_BUFFER_BUFFER_POOL_H
#define VTAB_BUFFER_BUFFER_POOL_H

#include <list>
#include <mutex>
#include <unordered_map>

#include "page/page.h"
#include "disk/disk.h"
#include "buffer/lru.h"

class BufferPool {
 public:
  BufferPool(size_t size, Disk *disk);
  ~BufferPool();

  Page *NewPage();
  Page *FetchPage(PageID page_id);
  bool UnpinPage(PageID page_id, bool is_dirty);
  bool FlushPage(PageID page_id);
  bool DeletePage(PageID page_id);

 private:
  size_t size_;
  Page *pages_;
  Disk *disk_;
  std::unordered_map<PageID, Page *> *page_table_;
  LRUReplacer<Page *> *replacer_;
  std::list<Page *> *free_list_;
  std::mutex mutex_;
};

#endif
