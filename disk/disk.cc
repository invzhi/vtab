#include "disk/disk.h"

#include <iostream>
#include <string>

#include "spdlog/spdlog.h"
#include "page/page.h"

Disk::Disk(const std::string &db_file) {
  auto mode = std::ios_base::in | std::ios_base::out | std::ios_base::binary;
  db_io_.open(db_file, mode);
  if (!db_io_) {
    spdlog::error("cannot open file: {}", db_file);
  }
}

PageID Disk::AllocatePage() {
  return next_page_id_++;
}

void Disk::DeallocatePage(PageID page_id) {
  // TODO: bitmap in page directory, manage free pages
}

void Disk::ReadPage(PageID page_id, char *data) {
  int offset = page_id * PAGE_SIZE;
  db_io_.seekg(offset);
  db_io_.read(data, PAGE_SIZE);
}

void Disk::WritePage(PageID page_id, const char *data) {
  int offset = page_id * PAGE_SIZE;
  db_io_.seekp(offset);
  db_io_.write(data, PAGE_SIZE);
}