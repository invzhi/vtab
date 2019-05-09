#include "buffer/buffer_pool.h"

#include <cstdio>
#include <cstring>
#include <fstream>

#include "gtest/gtest.h"
#include "page/page.h"
#include "disk/disk.h"

TEST(BufferPoolTest, SampleTest) {
  // create file
  std::ofstream f;
  f.open("test.db");
  f.close();

  Disk disk("test.db");
  BufferPool buffer_pool(10, &disk);

  Page *page_zero = buffer_pool.NewPage();
  ASSERT_NE(nullptr, page_zero);
  EXPECT_EQ(0, page_zero->GetID());

  // change page content
  std::strcpy(page_zero->GetData(), "Hello");

  for (int i = 1; i < 10; ++i) {
    EXPECT_NE(nullptr, buffer_pool.NewPage());
  }
  // all the pages are pinned, the buffer pool is full
  for (int i = 10; i < 15; ++i) {
    EXPECT_EQ(nullptr, buffer_pool.NewPage());
  }
  // unpin the first five pages, add them to LRU list, set as dirty
  for (PageID i = 0; i < 5; ++i) {
    EXPECT_EQ(true, buffer_pool.UnpinPage(i, true));
  }
  // we have 5 empty slots in LRU list, evict page 0-4 out of buffer pool
  for (int i = 0; i < 4; ++i) {
    EXPECT_NE(nullptr, buffer_pool.NewPage());
  }
  // fetch page one again
  page_zero = buffer_pool.FetchPage(0);
  ASSERT_NE(nullptr, page_zero);
  // check read content
  EXPECT_EQ(0, strcmp(page_zero->GetData(), "Hello"));

  std::remove("test.db");
}
