#ifndef VTAB_BUFFER_LRU_H
#define VTAB_BUFFER_LRU_H

#include <mutex>
#include <list>
#include <unordered_map>

template <typename T> class LRUReplacer {
 public:
  void Insert(const T &value);
  bool Victim(T &value);
  bool Erase(const T &value);
  size_t Size() const { return list_.size(); }

 private:
  std::mutex mutex_;
  std::list<T> list_;
  std::unordered_map<T, typename std::list<T>::iterator> map_;
};

#endif
