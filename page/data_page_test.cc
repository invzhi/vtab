#include "page/data_page.h"

#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "page/page.h"
#include "table/tuple.h"

TEST(DataPageTest, CRUDTest) {
  DataPage data_page;
  data_page.Init(INVALID_PAGE_ID);

  Tuple tuple;
  // insert
  int32_t slot_number;
  for (int i = 0; i < 5; ++i) {
    std::string str(10+i, 'a'+i);
    tuple.SetData(str.c_str(), str.length());
    EXPECT_TRUE(data_page.InsertTuple(slot_number, tuple));
    EXPECT_EQ(i, slot_number);
  }
  // get
  EXPECT_FALSE(data_page.GetTuple(10, tuple));
  for (int i = 0; i < 5; ++i) {
    std::string str(10+i, 'a'+i);
    EXPECT_TRUE(data_page.GetTuple(i, tuple));
    EXPECT_TRUE(std::memcmp(tuple.data(), str.c_str(), tuple.length()) == 0);
  }
  // update
  Tuple updated_tuple2;
  updated_tuple2.SetData("updated", 7);
  EXPECT_FALSE(data_page.UpdateTuple(12, updated_tuple2));
  EXPECT_TRUE(data_page.UpdateTuple(2, updated_tuple2));
  Tuple updated_tuple4;
  updated_tuple4.SetData("12345678901234567890", 20);
  EXPECT_TRUE(data_page.UpdateTuple(4, updated_tuple4));
  // get again
  EXPECT_TRUE(data_page.GetTuple(2, tuple));
  EXPECT_TRUE(std::memcmp(tuple.data(), updated_tuple2.data(), tuple.length()) == 0);
  for (int i = 0; i < 4; ++i) {
    if (i == 2) continue;
    std::string str(10+i, 'a'+i);
    EXPECT_TRUE(data_page.GetTuple(i, tuple));
    EXPECT_TRUE(std::memcmp(tuple.data(), str.c_str(), tuple.length()) == 0);
  }
  EXPECT_TRUE(data_page.GetTuple(4, tuple));
  EXPECT_TRUE(std::memcmp(tuple.data(), updated_tuple4.data(), tuple.length()) == 0);
  // delete
  EXPECT_FALSE(data_page.MarkDelete(10));
  EXPECT_TRUE(data_page.MarkDelete(0));
  // get again
  EXPECT_FALSE(data_page.GetTuple(0, tuple));
  EXPECT_FALSE(data_page.UpdateTuple(0, tuple));
  for (int i = 1; i < 4; ++i) {
    if (i == 2) continue;
    std::string str(10+i, 'a'+i);
    EXPECT_TRUE(data_page.GetTuple(i, tuple));
    EXPECT_TRUE(std::memcmp(tuple.data(), str.c_str(), tuple.length()) == 0);
  }
  EXPECT_TRUE(data_page.GetTuple(2, tuple));
  EXPECT_TRUE(std::memcmp(tuple.data(), updated_tuple2.data(), tuple.length()) == 0);
  EXPECT_TRUE(data_page.GetTuple(4, tuple));
  EXPECT_TRUE(std::memcmp(tuple.data(), updated_tuple4.data(), tuple.length()) == 0);
}