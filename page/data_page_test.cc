#include "page/data_page.h"

#include <string>

#include "gtest/gtest.h"
#include "page/page.h"
#include "table/tuple.h"

Tuple ConstructTuple(int64_t integer, double real, std::string str) {
  std::vector<Column> columns;
  columns.emplace_back(TypeID::INTEGER);
  columns.emplace_back(TypeID::DOUBLE);
  columns.emplace_back(TypeID::TEXT);
  Schema schema(columns);
  
  std::vector<Value> values;
  values.emplace_back(TypeID::INTEGER, integer);
  values.emplace_back(TypeID::DOUBLE, real);
  values.emplace_back(TypeID::TEXT, str);

  return Tuple(&schema, values);
}

TEST(DataPageTest, CRUDTest) {
  DataPage data_page;
  data_page.Init(INVALID_PAGE_ID);

  Tuple tuple0 = ConstructTuple(0, 0.0, "tuple0");
  Tuple tuple1 = ConstructTuple(1, 1.1, "tuple1");
  Tuple tuple2 = ConstructTuple(2, 2.2, "tuple2");
  Tuple tuple3 = ConstructTuple(3, 3.3, "tuple3");
  Tuple tuple4 = ConstructTuple(4, 4.4, "tuple4");
  Tuple updated_tuple = ConstructTuple(5, 5.5, "updated");
  // insert
  RowID row_id;
  EXPECT_EQ(true, data_page.InsertTuple(row_id, tuple0));
  EXPECT_EQ(0, row_id.slot_number());
  EXPECT_EQ(true, data_page.InsertTuple(row_id, tuple1));
  EXPECT_EQ(1, row_id.slot_number());
  EXPECT_EQ(true, data_page.InsertTuple(row_id, tuple2));
  EXPECT_EQ(2, row_id.slot_number());
  EXPECT_EQ(true, data_page.InsertTuple(row_id, tuple3));
  EXPECT_EQ(3, row_id.slot_number());
  EXPECT_EQ(true, data_page.InsertTuple(row_id, tuple4));
  EXPECT_EQ(4, row_id.slot_number());
  // get
  Tuple tuple;
  EXPECT_EQ(false, data_page.GetTuple(10, tuple));
  EXPECT_EQ(true, data_page.GetTuple(0, tuple));
  EXPECT_TRUE(std::strncmp(tuple.data(), tuple0.data(), tuple.length()) == 0);
  EXPECT_EQ(true, data_page.GetTuple(1, tuple));
  EXPECT_TRUE(std::strncmp(tuple.data(), tuple1.data(), tuple.length()) == 0);
  EXPECT_EQ(true, data_page.GetTuple(2, tuple));
  EXPECT_TRUE(std::strncmp(tuple.data(), tuple2.data(), tuple.length()) == 0);
  EXPECT_EQ(true, data_page.GetTuple(3, tuple));
  EXPECT_TRUE(std::strncmp(tuple.data(), tuple3.data(), tuple.length()) == 0);
  EXPECT_EQ(true, data_page.GetTuple(4, tuple));
  EXPECT_TRUE(std::strncmp(tuple.data(), tuple4.data(), tuple.length()) == 0);
  // update
  EXPECT_EQ(false, data_page.UpdateTuple(12, updated_tuple));
  EXPECT_EQ(true, data_page.UpdateTuple(2, updated_tuple));
  EXPECT_EQ(true, data_page.GetTuple(2, tuple));
  EXPECT_TRUE(std::strncmp(tuple.data(), updated_tuple.data(), tuple.length()) == 0);
  // check other tuple again
  EXPECT_EQ(true, data_page.GetTuple(0, tuple));
  EXPECT_TRUE(std::strncmp(tuple.data(), tuple0.data(), tuple.length()) == 0);
  EXPECT_EQ(true, data_page.GetTuple(1, tuple));
  EXPECT_TRUE(std::strncmp(tuple.data(), tuple1.data(), tuple.length()) == 0);
  EXPECT_EQ(true, data_page.GetTuple(3, tuple));
  EXPECT_TRUE(std::strncmp(tuple.data(), tuple3.data(), tuple.length()) == 0);
  EXPECT_EQ(true, data_page.GetTuple(4, tuple));
  EXPECT_TRUE(std::strncmp(tuple.data(), tuple4.data(), tuple.length()) == 0);
  // delete
  EXPECT_EQ(false, data_page.DeleteTuple(10));
  EXPECT_EQ(true, data_page.DeleteTuple(0));
  // check other tuple again
  EXPECT_EQ(true, data_page.GetTuple(0, tuple));
  EXPECT_TRUE(std::strncmp(tuple.data(), tuple1.data(), tuple.length()) == 0);
  EXPECT_EQ(true, data_page.GetTuple(1, tuple));
  EXPECT_TRUE(std::strncmp(tuple.data(), updated_tuple.data(), tuple.length()) == 0);
  EXPECT_EQ(true, data_page.GetTuple(2, tuple));
  EXPECT_TRUE(std::strncmp(tuple.data(), tuple3.data(), tuple.length()) == 0);
  EXPECT_EQ(true, data_page.GetTuple(3, tuple));
  EXPECT_TRUE(std::strncmp(tuple.data(), tuple4.data(), tuple.length()) == 0);
}