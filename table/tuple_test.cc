#include "table/tuple.h"

#include <cstdio>
#include <cstring>
#include <vector>

#include "gtest/gtest.h"
#include "type/type_id.h"
#include "type/value.h"
#include "catalog/column.h"
#include "catalog/schema.h"

TEST(TupleTest, DataTest) {
  std::vector<Column> columns;
  columns.emplace_back(TypeID::INTEGER);
  columns.emplace_back(TypeID::DOUBLE);
  columns.emplace_back(TypeID::TEXT);
  columns.emplace_back(TypeID::INTEGER);
  columns.emplace_back(TypeID::TEXT);

  Schema schema(columns);
  EXPECT_EQ(32, schema.GetFixedLength());

  std::vector<Value> values;
  values.emplace_back(TypeID::INTEGER, int64_t(-1));
  values.emplace_back(TypeID::DOUBLE, 9.9);
  values.emplace_back(TypeID::TEXT, "HELLO");
  values.emplace_back(TypeID::INTEGER, int64_t(16));
  values.emplace_back(TypeID::TEXT, "LAST_COLUMN");

  // store in tuple
  Tuple tuple(&schema, values);
  EXPECT_EQ(58, tuple.length());
  // get from tuple
  std::vector<Value> vals;
  for (size_t i = 0; i < schema.size(); ++i) {
    vals.push_back(tuple.GetValue(&schema, i));
  }
  // check
  EXPECT_EQ(-1, vals[0].GetAs<int64_t>());
  EXPECT_EQ(9.9, vals[1].GetAs<double>());
  EXPECT_TRUE(std::strcmp("HELLO", vals[2].GetAs<char *>()));
  EXPECT_EQ(16, vals[3].GetAs<int64_t>());
  EXPECT_TRUE(std::strcmp("LAST_COLUMN", vals[4].GetAs<char *>()));
}