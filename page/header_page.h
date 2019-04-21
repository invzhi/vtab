#ifndef VTAB_PAGE_HEADER_PAGE
#define VTAB_PAGE_HEADER_PAGE

#include <string>

#include "page/page.h"

class HeaderPage : public Page {
 public:
  void Init() { SetRecordCount(0); }

  bool InsertRecord(const std::string &name, const PageID page_id);
  bool UpdateRecord(const std::string &name, const PageID page_id);
  bool DeleteRecord(const std::string &name);
  bool GetPageID(const std::string &name, PageID &page_id);

 private:
  int FindRecordIndex(const std::string &name);
  int32_t GetRecordCount();
  void SetRecordCount(int32_t count);
};

#endif