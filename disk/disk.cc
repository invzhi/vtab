#include "disk/disk.h"

#include <iostream>
#include <string>
#include <sys/stat.h>

#include "spdlog/spdlog.h"
#include "page/page.h"

Disk::Disk(const std::string &db_file) : db_name_(db_file) {
  auto mode = std::ios_base::in | std::ios_base::out | std::ios_base::binary;
  db_io_.open(db_name_, mode);
  if (!db_io_.is_open())
    spdlog::error("cannot open file: {}", db_name_);
}

PageID Disk::AllocatePage() {
  return next_page_id_++;
}

void Disk::DeallocatePage(PageID page_id) {
  // TODO: bitmap in page directory, manage free pages
}

void Disk::ReadPage(PageID page_id, char *data) {
  int offset = page_id * PAGE_SIZE;
  if (offset >= GetFileSize()) {
    spdlog::error("cannot read page from disk");
    return;
  }

  db_io_.seekg(offset);
  db_io_.read(data, PAGE_SIZE);

  int read_count = db_io_.gcount();
  if (read_count < PAGE_SIZE) {
    spdlog::debug("read less than page size");
    std::memset(data + read_count, 0, PAGE_SIZE - read_count);
  }
}

void Disk::WritePage(PageID page_id, const char *data) {
  int offset = page_id * PAGE_SIZE;
  db_io_.seekp(offset);
  db_io_.write(data, PAGE_SIZE);
  if (db_io_.bad()) {
    spdlog::error("cannot write page to disk");
    return;
  }
  db_io_.flush();
}

int Disk::GetFileSize() {
  struct stat buf;
  int rc = stat(db_name_.c_str(), &buf);
  return rc == 0 ? buf.st_size : -1;
}