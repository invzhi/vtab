#include "table/table_iterator.h"

#include <cstdio>
#include <cstring>
#include <fstream>

#include "gtest/gtest.h"
#include "buffer/buffer_pool.h"
#include "table/row_id.h"
#include "table/tuple.h"
#include "table/table_heap.h"

TEST(TableIteratorTest, ScanTest) {
  // create file
  std::ofstream f;
  f.open("test.db");
  f.close();

  Disk disk("test.db");
  BufferPool buffer_pool(100, &disk);
  TableHeap table_heap(&buffer_pool);

  Tuple tuple;
  tuple.SetData("hello tuple", 11);
  RowID row_id;
  // build table first
  for (int i = 0; i < 1000; ++i) {
    ASSERT_TRUE(table_heap.InsertTuple(row_id, tuple));
  }

  int cnt = 0;
  for (auto &tuple : table_heap) {
    ++cnt;
    EXPECT_TRUE(std::memcmp(tuple.data(), "hello tuple", 11) == 0);
  }
  EXPECT_EQ(1000, cnt);

  std::remove("test.db");
}