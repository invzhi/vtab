#include "buffer/lru.h"

#include "gtest/gtest.h"

TEST(LRUTest, SampleTest) {
  LRUReplacer<int> lru;
  
  lru.Insert(1);
  lru.Insert(2);
  lru.Insert(3);
  lru.Insert(4);
  lru.Insert(5);
  lru.Insert(2);
  EXPECT_EQ(5, lru.Size());

  int value;
  EXPECT_EQ(true, lru.Victim(value));
  EXPECT_EQ(1, value);
  EXPECT_EQ(true, lru.Victim(value));
  EXPECT_EQ(3, value);
  EXPECT_EQ(true, lru.Victim(value));
  EXPECT_EQ(4, value);

  EXPECT_EQ(false, lru.Erase(1));
  EXPECT_EQ(false, lru.Erase(3));
  EXPECT_EQ(false, lru.Erase(4));
  EXPECT_EQ(true, lru.Erase(2));
  EXPECT_EQ(true, lru.Erase(5));

  EXPECT_EQ(false, lru.Victim(value));
}