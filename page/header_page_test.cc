#include "page/header_page.h"

#include "gtest/gtest.h"

TEST(HeaderPageTest, SampleTest) {
  HeaderPage page;

  page.Init();
  // insert
  EXPECT_TRUE(page.InsertRecord("table1", 1));
  EXPECT_TRUE(page.InsertRecord("table2", 2));
  EXPECT_TRUE(page.InsertRecord("table3", 3));
  EXPECT_TRUE(page.InsertRecord("table4", 4));
  EXPECT_TRUE(page.InsertRecord("table5", 5));
  // duplicate
  EXPECT_FALSE(page.InsertRecord("table1", 1));

  // get
  PageID page_id;
  EXPECT_FALSE(page.GetPageID("notexist", page_id));
  EXPECT_TRUE(page.GetPageID("table1", page_id));
  EXPECT_EQ(1, page_id);
  EXPECT_TRUE(page.GetPageID("table2", page_id));
  EXPECT_EQ(2, page_id);
  EXPECT_TRUE(page.GetPageID("table3", page_id));
  EXPECT_EQ(3, page_id);
  EXPECT_TRUE(page.GetPageID("table4", page_id));
  EXPECT_EQ(4, page_id);
  EXPECT_TRUE(page.GetPageID("table5", page_id));
  EXPECT_EQ(5, page_id);

  // update
  EXPECT_FALSE(page.UpdateRecord("notexist", 30));
  EXPECT_TRUE(page.UpdateRecord("table3", 30));
  EXPECT_TRUE(page.GetPageID("table3", page_id));
  EXPECT_EQ(30, page_id);

  // delete
  EXPECT_TRUE(page.DeleteRecord("table3"));
  EXPECT_FALSE(page.GetPageID("table3", page_id));
}