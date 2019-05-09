#ifndef VTAB_SQLITE_CLIENT_H
#define VTAB_SQLITE_CLIENT_H

#include <string>

#include "disk/disk.h"
#include "buffer/buffer_pool.h"

class StorageEngine {
public:
  StorageEngine(const std::string db_name);
  ~StorageEngine() {
    buffer_pool_->FlushAllPages();
    delete disk_;
    delete buffer_pool_;
  }

  Disk *disk_;
  BufferPool *buffer_pool_;
};

#endif