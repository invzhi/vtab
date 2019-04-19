#include "buffer/lru.h"

#include <utility>

#include "page/page.h"

template <typename T> void LRUReplacer<T>::Insert(const T &value) {
  std::lock_guard<std::mutex> guard(mutex_);

  auto it = map_.find(value);
  if (it != map_.end()) {
    list_.splice(list_.begin(), list_, it->second);
  } else {
    list_.push_front(value);
    map_.insert(std::make_pair(value, list_.begin()));
  }
}

template <typename T> bool LRUReplacer<T>::Victim(T &value) {
  std::lock_guard<std::mutex> guard(mutex_);

  if (list_.empty()) {
    return false;
  }

  value = list_.back();
  list_.pop_back();
  map_.erase(value);

  return true;
}

template <typename T> bool LRUReplacer<T>::Erase(const T &value) {
  std::lock_guard<std::mutex> guard(mutex_);

  auto it = map_.find(value);
  if (it == map_.end()) {
    return false;
  }

  map_.erase(value);
  list_.erase(it->second);

  return true;
}

template class LRUReplacer<Page *>;
// for test
template class LRUReplacer<int>;
