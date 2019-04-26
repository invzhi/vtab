#include "sqlite/client.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cassert>
#include <string>
#include <iostream>
#include <fstream>

#include "spdlog/spdlog.h"
#include "config.h"
#include "disk/disk.h"
#include "page/header_page.h"

StorageEngine::StorageEngine(const std::string db_name) {
  struct stat buf;
  bool is_file_exist = (stat(db_name.c_str(), &buf) == 0);
  if (!is_file_exist) {
    spdlog::info("create new file: {}", db_name);

    std::fstream f(db_name, std::ios_base::out);
    f.close();
  }

  disk_ = new Disk(db_name);
  buffer_pool_ = new BufferPool(BUFFER_POOL_SIZE, disk_);

  if (!is_file_exist) {
    auto header_page = static_cast<HeaderPage *>(buffer_pool_->NewPage());
    assert(header_page->id() == HEADER_PAGE_ID);
    header_page->WLock();
    header_page->Init();
    header_page->WUnlock();
    buffer_pool_->UnpinPage(header_page->id(), true);
  }
}