#include "table/table_heap.h"

#include <cstdio>
#include <cstring>
#include <fstream>

#include "gtest/gtest.h"
#include "config.h"
#include "buffer/buffer_pool.h"
#include "table/row_id.h"
#include "table/tuple.h"

TEST(TableHeapTest, CRUDTest) {
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
  int tuple_count_every_page = (PAGE_SIZE - 20) / (11 + 8);
  // insert
  for (int i = 0; i < 1000; ++i) {
    EXPECT_TRUE(table_heap.InsertTuple(row_id, tuple));
    EXPECT_EQ(i / tuple_count_every_page, row_id.GetPageID());
    EXPECT_EQ(i % tuple_count_every_page, row_id.GetSlotNum());
  }
  // get
  for (int i = 0; i < 1010; ++i) {
    row_id.Set(i / tuple_count_every_page, i % tuple_count_every_page);
    EXPECT_EQ(i < 1000, table_heap.GetTuple(row_id, tuple));
    if (i < 1000)
      EXPECT_TRUE(std::memcmp("hello tuple", tuple.data(), 11) == 0);
  }
  // update
  tuple.SetData("updated", 7);
  row_id.Set(2, 100);
  EXPECT_TRUE(table_heap.UpdateTuple(row_id, tuple));
  // check update
  EXPECT_TRUE(table_heap.GetTuple(row_id, tuple));
  EXPECT_TRUE(std::memcmp("updated", tuple.data(), 7) == 0);
  // delete
  EXPECT_TRUE(table_heap.MarkDelete(row_id));
  // check delete
  EXPECT_FALSE(table_heap.GetTuple(row_id, tuple));

  std::remove("test.db");
}