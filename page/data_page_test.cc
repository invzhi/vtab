#include "page/data_page.h"

#include <string>
#include <vector>
#include <sstream>

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
    EXPECT_TRUE(std::strncmp(tuple.data(), str.c_str(), tuple.length()) == 0);
  }
  // update
  Tuple updated_tuple;
  updated_tuple.SetData("updated", 7);
  EXPECT_FALSE(data_page.UpdateTuple(12, updated_tuple));
  EXPECT_TRUE(data_page.UpdateTuple(2, updated_tuple));
  // get again
  EXPECT_TRUE(data_page.GetTuple(2, tuple));
  EXPECT_TRUE(std::strncmp(tuple.data(), updated_tuple.data(), tuple.length()) == 0);
  for (int i = 0; i < 5; ++i) {
    if (i == 2) continue;
    std::string str(10+i, 'a'+i);
    EXPECT_TRUE(data_page.GetTuple(i, tuple));
    EXPECT_TRUE(std::strncmp(tuple.data(), str.c_str(), tuple.length()) == 0);
  }
  // delete
  EXPECT_FALSE(data_page.DeleteTuple(10));
  EXPECT_TRUE(data_page.DeleteTuple(0));
  // get again
  for (int i = 1; i < 5; ++i) {
    if (i == 2) continue;
    std::string str(10+i, 'a'+i);
    EXPECT_TRUE(data_page.GetTuple(i-1, tuple));
    EXPECT_TRUE(std::strncmp(tuple.data(), str.c_str(), tuple.length()) == 0);
  }
}