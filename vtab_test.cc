#include <vector>
#include <string>
#include <sqlite3.h>
#include <cstdint>
#include <cstring>

#include "gtest/gtest.h"

TEST(VtabTest, SQLTest) {
  sqlite3 *db;
  ASSERT_EQ(SQLITE_OK, sqlite3_open("test.db", &db));
  ASSERT_EQ(SQLITE_OK, sqlite3_enable_load_extension(db, 1));

  char *zErrMsg = nullptr;
  ASSERT_EQ(SQLITE_OK, sqlite3_load_extension(db, "libvtab", 0, &zErrMsg));

  // data
  std::vector<int64_t> ids = {1, 2, 3, 4, 5};
  std::vector<double> ages = {21.5, 22.5, 23.5, 24.5, 25.5};
  std::vector<std::string> names = {"name1", "name2", "name3", "name4", "name5"};

  // create table
  std::string sql = "CREATE VIRTUAL TABLE t USING vtab (id int PRIMARY KEY, age double, name text)";
  ASSERT_EQ(SQLITE_OK, sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg));

  sqlite3_stmt *select_stmt;
  sql = "SELECT * FROM t";
  ASSERT_EQ(SQLITE_OK, sqlite3_prepare_v2(db, sql.c_str(), sql.length()+1, &select_stmt, nullptr));
  EXPECT_EQ(SQLITE_DONE, sqlite3_step(select_stmt)); // no row
  ASSERT_EQ(SQLITE_OK, sqlite3_reset(select_stmt));

  // insert
  sqlite3_stmt *insert_stmt;
  sql = "INSERT INTO t VALUES (?, ?, ?)";
  ASSERT_EQ(SQLITE_OK, sqlite3_prepare_v2(db, sql.c_str(), sql.length()+1, &insert_stmt, nullptr));
  for (int i = 0; i < 5; ++i) {
    ASSERT_EQ(SQLITE_OK, sqlite3_bind_int64(insert_stmt, 1, ids[i]));
    ASSERT_EQ(SQLITE_OK, sqlite3_bind_double(insert_stmt, 2, ages[i]));
    ASSERT_EQ(SQLITE_OK, sqlite3_bind_text(insert_stmt, 3, names[i].c_str(), -1, nullptr));

    ASSERT_EQ(SQLITE_DONE, sqlite3_step(insert_stmt));
    ASSERT_EQ(SQLITE_OK, sqlite3_reset(insert_stmt));
  }
  ASSERT_EQ(SQLITE_OK, sqlite3_finalize(insert_stmt));
  
  // select
  int iRow = 0;
  while (sqlite3_step(select_stmt) != SQLITE_DONE) {
    auto name = reinterpret_cast<const char *>(sqlite3_column_text(select_stmt, 2));
    EXPECT_EQ(sqlite3_column_int64(select_stmt, 0), ids[iRow]);
    EXPECT_EQ(sqlite3_column_double(select_stmt, 1), ages[iRow]);
    EXPECT_EQ(0, std::strcmp(name, names[iRow].c_str()));
    ++iRow;
  }
  ASSERT_EQ(SQLITE_OK, sqlite3_reset(select_stmt));

  // update
  sql = "UPDATE t SET age = 20.5 WHERE id > 3";
  EXPECT_EQ(SQLITE_OK, sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg));
  ages[3] = 20.5;
  ages[4] = 20.5;
  
  // select: check update
  iRow = 0;
  while (sqlite3_step(select_stmt) != SQLITE_DONE) {
    auto name = reinterpret_cast<const char *>(sqlite3_column_text(select_stmt, 2));
    EXPECT_EQ(sqlite3_column_int64(select_stmt, 0), ids[iRow]);
    EXPECT_EQ(sqlite3_column_double(select_stmt, 1), ages[iRow]);
    EXPECT_EQ(0, std::strcmp(name, names[iRow].c_str()));
    ++iRow;
  }
  ASSERT_EQ(SQLITE_OK, sqlite3_reset(select_stmt));

  // delete
  sql = "DELETE FROM t WHERE id < 3";
  EXPECT_EQ(SQLITE_OK, sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg));

  // select: check delete
  iRow = 2;
  while (sqlite3_step(select_stmt) != SQLITE_DONE) {
    auto name = reinterpret_cast<const char *>(sqlite3_column_text(select_stmt, 2));
    EXPECT_EQ(sqlite3_column_int64(select_stmt, 0), ids[iRow]);
    EXPECT_EQ(sqlite3_column_double(select_stmt, 1), ages[iRow]);
    EXPECT_EQ(0, std::strcmp(name, names[iRow].c_str()));
    ++iRow;
  }
  ASSERT_EQ(SQLITE_OK, sqlite3_finalize(select_stmt));

  sqlite3_close(db);

  // test for xConnect
  ASSERT_EQ(SQLITE_OK, sqlite3_open("test.db", &db));
  ASSERT_EQ(SQLITE_OK, sqlite3_enable_load_extension(db, 1));
  ASSERT_EQ(SQLITE_OK, sqlite3_load_extension(db, "libvtab", 0, &zErrMsg));

  sql = "SELECT * FROM t";
  ASSERT_EQ(SQLITE_OK, sqlite3_prepare_v2(db, sql.c_str(), sql.length()+1, &select_stmt, nullptr));
  iRow = 2;
  while (sqlite3_step(select_stmt) != SQLITE_DONE) {
    auto name = reinterpret_cast<const char *>(sqlite3_column_text(select_stmt, 2));
    EXPECT_EQ(sqlite3_column_int64(select_stmt, 0), ids[iRow]);
    EXPECT_EQ(sqlite3_column_double(select_stmt, 1), ages[iRow]);
    EXPECT_EQ(0, std::strcmp(name, names[iRow].c_str()));
    ++iRow;
  }
  ASSERT_EQ(SQLITE_OK, sqlite3_finalize(select_stmt));

  sqlite3_close(db);

  std::remove("db.db"); // filename in vtab.cc
  std::remove("test.db");
}