#ifndef VTAB_DISK_DISK_H
#define VTAB_DISK_DISK_H

#include <string>
#include <fstream>

#include "page/page.h"

class Disk {
 public:
  Disk(const std::string &db_name);

  PageID AllocatePage();
  void DeallocatePage(PageID page_id);

  void ReadPage(PageID page_id, char *data);
  void WritePage(PageID page_id, const char *data);

 private:
  int GetFileSize();

  std::string db_name_;
  std::fstream db_io_; // TODO: change to O_DIRECT, without internal buffer
  PageID next_page_id_ = 0;
};

#endif