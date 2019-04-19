#include "buffer/buffer_pool.h"

#include <utility>

#include "page/page.h"
#include "buffer/lru.h"

BufferPool::BufferPool(size_t size, Disk *disk) : size_(size), disk_(disk) {
  pages_ = new Page[size_];
  page_table_ = new std::unordered_map<PageID, Page *>;
  replacer_ = new LRUReplacer<Page *>;
  free_list_ = new std::list<Page *>;
  
  for (size_t i = 0; i < size_; ++i) {
    free_list_->push_back(&pages_[i]);
  }
}

BufferPool::~BufferPool() {
  delete[] pages_;
  delete page_table_;
  delete replacer_;
  delete free_list_;
}

Page *BufferPool::NewPage() {
  std::lock_guard<std::mutex> lock(mutex_);

  PageID page_id = disk_->AllocatePage();
  Page *page = nullptr;

  if (!free_list_->empty()) {
    page = free_list_->front();
    free_list_->pop_front();
  } else if (replacer_->Victim(page)) {
    page_table_->erase(page->page_id_);
    if (page->is_dirty_) {
      disk_->WritePage(page->page_id_, page->data_);
      page->is_dirty_ = false;
    }
  } else {
    return nullptr;
  }

  page->page_id_ = page_id;
  ++page->pin_count_;
  page_table_->insert(std::make_pair(page_id, page));
  return page;
}

Page *BufferPool::FetchPage(PageID page_id) {
  std::lock_guard<std::mutex> lock(mutex_);

  Page *page = nullptr;

  auto it = page_table_->find(page_id);
  if (it != page_table_->end()) {
    page = it->second;
    ++page->pin_count_;
    replacer_->Erase(page);
    return page;
  }

  if (!free_list_->empty()) {
    page = free_list_->front();
    free_list_->pop_front();
  } else if (replacer_->Victim(page)) {
    page_table_->erase(page->page_id_);
    if (page->is_dirty_) {
      disk_->WritePage(page->page_id_, page->data_);
      page->is_dirty_ = false;
    }
  } else {
    return nullptr;
  }

  page->page_id_ = page_id;
  ++page->pin_count_;
  disk_->ReadPage(page_id, page->data_);
  page_table_->insert(std::make_pair(page_id, page));
  return page;
}

bool BufferPool::UnpinPage(PageID page_id, bool is_dirty) {
  std::lock_guard<std::mutex> lock(mutex_);

  auto it = page_table_->find(page_id);
  if (it == page_table_->end()) {
    return false;
  }

  Page *page = it->second;
  page->is_dirty_ = is_dirty;

  if (page->pin_count_ <= 0) {
    return false;
  }

  if (--page->pin_count_ == 0) {
    replacer_->Insert(page);
  }

  return true;
}


bool BufferPool::FlushPage(PageID page_id) {
  std::lock_guard<std::mutex> lock(mutex_);

  auto it = page_table_->find(page_id);
  if (it == page_table_->end()) {
    return false;
  }

  Page *page = it->second;
  disk_->WritePage(page_id, page->data_);
  return true;
}

bool BufferPool::DeletePage(PageID page_id) {
  std::lock_guard<std::mutex> lock(mutex_);

  auto it = page_table_->find(page_id);
  if (it != page_table_->end()) {
    Page *page = it->second;
    if (page->pin_count_ != 0) {
      return false;
    }
    page->is_dirty_ = false;

    page_table_->erase(page_id);
    replacer_->Erase(page);
    free_list_->push_back(page);
  }

  disk_->DeallocatePage(page_id);
  return true;
}
