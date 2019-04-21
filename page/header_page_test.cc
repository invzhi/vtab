#include "page/header_page.h"

#include "gtest/gtest.h"

TEST(HeaderPageTest, SampleTest) {
  HeaderPage page;

  page.Init();
  // insert
  EXPECT_EQ(true, page.InsertRecord("table1", 1));
  EXPECT_EQ(true, page.InsertRecord("table2", 2));
  EXPECT_EQ(true, page.InsertRecord("table3", 3));
  EXPECT_EQ(true, page.InsertRecord("table4", 4));
  EXPECT_EQ(true, page.InsertRecord("table5", 5));
  // duplicate
  EXPECT_EQ(false, page.InsertRecord("table1", 1));

  // get
  PageID page_id;
  EXPECT_EQ(false, page.GetPageID("notexist", page_id));
  EXPECT_EQ(true, page.GetPageID("table1", page_id));
  EXPECT_EQ(1, page_id);
  EXPECT_EQ(true, page.GetPageID("table2", page_id));
  EXPECT_EQ(2, page_id);
  EXPECT_EQ(true, page.GetPageID("table3", page_id));
  EXPECT_EQ(3, page_id);
  EXPECT_EQ(true, page.GetPageID("table4", page_id));
  EXPECT_EQ(4, page_id);
  EXPECT_EQ(true, page.GetPageID("table5", page_id));
  EXPECT_EQ(5, page_id);

  // update
  EXPECT_EQ(false, page.UpdateRecord("notexist", 30));
  EXPECT_EQ(true, page.UpdateRecord("table3", 30));
  EXPECT_EQ(true, page.GetPageID("table3", page_id));
  EXPECT_EQ(30, page_id);

  // delete
  EXPECT_EQ(true, page.DeleteRecord("table3"));
  EXPECT_EQ(false, page.GetPageID("table3", page_id));
}