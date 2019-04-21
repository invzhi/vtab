#include "page/header_page.h"

#include <cassert>
#include <cstdint>
#include <cstring>

bool HeaderPage::InsertRecord(const std::string &name, const PageID page_id) {
  assert(name.length() < 32);

  int32_t record_count = GetRecordCount();
  int32_t offset = 4 + record_count*36;
  if (FindRecordIndex(name) != -1)
    return false;

  std::memcpy(data() + offset, name.c_str(), name.length() + 1);
  *reinterpret_cast<PageID *>(data() + offset + 32) = page_id;
  SetRecordCount(++record_count);
  return true;
}

bool HeaderPage::UpdateRecord(const std::string &name, const PageID page_id) {
  int index = FindRecordIndex(name);
  if (index == -1)
    return false;

  *reinterpret_cast<PageID *>(data() + 4 + index*36 + 32) = page_id;
  return true;
}

bool HeaderPage::DeleteRecord(const std::string &name) {
  int index = FindRecordIndex(name);
  if (index == -1)
    return false;

  int32_t record_count = GetRecordCount();
  if (index != record_count - 1)
    std::memcpy(data() + 4 + index*36, data() + 4 + (record_count - 1)*36, 36);
  SetRecordCount(--record_count);
  return true;
}

bool HeaderPage::GetPageID(const std::string &name, PageID &page_id) {
  int index = FindRecordIndex(name);
  if (index == -1)
    return false;
  page_id = *reinterpret_cast<PageID *>(data() + 4 + index*36 + 32);
  return true;
}

int HeaderPage::FindRecordIndex(const std::string &name) {
  int32_t record_count = GetRecordCount();
  for (int i = 0; i < record_count; ++i) {
    if (std::strcmp(data() + 4 + i*36, name.c_str()) == 0) {
      return i;
    }
  }
  return -1;
}

int32_t HeaderPage::GetRecordCount() {
  return *reinterpret_cast<int32_t *>(data());
}

void HeaderPage::SetRecordCount(int32_t count) {
  *reinterpret_cast<int32_t *>(data()) = count;
}

