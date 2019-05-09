#include "disk/disk.h"

#include <cstring>

#include "gtest/gtest.h"

TEST(DiskTest, SampleTest) {
  // create file
  std::ofstream f;
  f.open("disktest.db");
  f.close();

  Page page;
  std::memset(page.GetData(), 1, PAGE_SIZE);
  auto disk = new Disk("disktest.db");
  disk->WritePage(0, page.GetData());
  delete disk;

  Page page2;
  auto disk2 = new Disk("disktest.db");
  disk->ReadPage(0, page2.GetData());
  delete disk2;
  
  EXPECT_EQ(0, std::memcmp(page.GetData(), page2.GetData(), PAGE_SIZE));

  std::remove("disktest.db");
}