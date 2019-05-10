#include <sqlite3ext.h>
#include <cctype>
#include <algorithm>

#include "spdlog/spdlog.h"
#include "type/type_id.h"
#include "page/page.h"
#include "page/header_page.h"
#include "sqlite/client.h"
#include "sqlite/virtual_table.h"
#include "sqlite/cursor.h"
#include "catalog/column.h"

SQLITE_EXTENSION_INIT1

TypeID typeName(std::string column_def) {
  size_t pos = column_def.find(' ', 0);
  while (column_def[pos] == ' ') ++pos;
  size_t end = column_def.find(' ', pos);

  std::string type_string = column_def.substr(pos, end-pos);
  std::transform(type_string.begin(), type_string.end(), type_string.begin(), ::tolower);
  spdlog::debug(type_string);
  if (type_string.find("int") != std::string::npos) {
    return TypeID::INTEGER;
  }
  if (type_string.find("doub") != std::string::npos) {
    return TypeID::DOUBLE;
  }
  if (type_string.find("text") != std::string::npos) {
    return TypeID::TEXT;
  }
  return TypeID::TEXT;
}

Schema *GetSchema(std::string &str, int argc, const char *const *argv) {
  std::vector<Column> columns;

  str += "CREATE TABLE x(";
  std::string column_def = argv[3];
  str += argv[3];
  columns.emplace_back(typeName(column_def));
  for (int i = 4; i < argc; ++i) {
    str += ", ";
    column_def = argv[i];
    str += column_def;
    columns.emplace_back(typeName(column_def));
  }
  str += ")";
  
  return new Schema(columns);
}

Tuple ConstructTuple(Schema *schema, int argc, sqlite3_value **argv) {
  std::vector<Value> values;
  for (int i = 0; i < argc; ++i) {
    TypeID type_id = schema->GetTypeID(i);
    switch (type_id) {
      case TypeID::DOUBLE:
        values.emplace_back(type_id, sqlite3_value_double(argv[i]));
        break;
      case TypeID::INTEGER:
        values.emplace_back(type_id, (int64_t)sqlite3_value_int64(argv[i]));
        break;
      case TypeID::TEXT:
        std::string s(reinterpret_cast<const char *>(sqlite3_value_text(argv[i])));
        values.emplace_back(type_id, s);
        break;
    }
  }
  return Tuple(schema, values);
}

int xCreate(sqlite3 *db, void *pAux, int argc, const char *const *argv,
            sqlite3_vtab **ppVtab, char **pzErr) {
  spdlog::trace(__func__);

  if (argc <= 3) {
    // pzErr
    return SQLITE_ERROR;
  }

  std::string schema_string;
  Schema *schema = GetSchema(schema_string, argc, argv);
  int rc = sqlite3_declare_vtab(db, schema_string.c_str());
  // spdlog::debug(schema_string);
  assert(rc == SQLITE_OK);

  auto storage_engine = reinterpret_cast<StorageEngine *>(pAux);
  BufferPool *buffer_pool = storage_engine->buffer_pool_;
  auto virtual_table = new VirtualTable(buffer_pool, schema);
  *ppVtab = reinterpret_cast<sqlite3_vtab *>(virtual_table);
  spdlog::debug("new table heap first page id: {}", virtual_table->GetFirstPageID());

  auto header_page = static_cast<HeaderPage *>(buffer_pool->FetchPage(HEADER_PAGE_ID));
  assert(header_page);
  header_page->WLock();
  header_page->InsertRecord(argv[2], virtual_table->GetFirstPageID());
  header_page->WUnlock();
  buffer_pool->UnpinPage(HEADER_PAGE_ID, true);

  return SQLITE_OK;
}

int xConnect(sqlite3 *db, void *pAux, int argc, const char *const *argv,
             sqlite3_vtab **ppVtab, char **pzErr) {
  spdlog::trace(__func__);

  std::string schema_string;
  Schema *schema = GetSchema(schema_string, argc, argv);
  int rc = sqlite3_declare_vtab(db, schema_string.c_str());
  // spdlog::debug(schema_string);
  assert(rc == SQLITE_OK);

  auto storage_engine = reinterpret_cast<StorageEngine *>(pAux);
  BufferPool *buffer_pool = storage_engine->buffer_pool_;

  auto header_page = static_cast<HeaderPage *>(buffer_pool->FetchPage(HEADER_PAGE_ID));
  assert(header_page);
  PageID page_id;
  header_page->RLock();
  spdlog::debug("table name: {}", argv[2]);
  bool geted = header_page->GetPageID(argv[2], page_id);
  header_page->RUnlock();
  buffer_pool->UnpinPage(HEADER_PAGE_ID, false);
  if (!geted) {
    spdlog::debug("cannot get first page id");
    return SQLITE_ERROR;
  }

  auto virtual_table = new VirtualTable(buffer_pool, schema, page_id);
  *ppVtab = reinterpret_cast<sqlite3_vtab *>(virtual_table);

  return SQLITE_OK;
}

int xBestIndex(sqlite3_vtab *pVTab, sqlite3_index_info *pIdxInfo) {
  spdlog::trace(__func__);

  for (int i = 0; i < pIdxInfo->nConstraint; ++i) {
    spdlog::debug("iColumn: {}, op: {}, usable: {}",
                  pIdxInfo->aConstraint[i].iColumn,
                  pIdxInfo->aConstraint[i].op,
                  pIdxInfo->aConstraint[i].usable);
  }

  return SQLITE_OK;
}

int xDisconnect(sqlite3_vtab *pVTab) {
  spdlog::trace(__func__);
  
  auto virtual_table = reinterpret_cast<VirtualTable *>(pVTab);
  delete virtual_table;
  return SQLITE_OK;
}

int xDestroy(sqlite3_vtab *pVTab) {
  spdlog::trace(__func__);

  auto virtual_table = reinterpret_cast<VirtualTable *>(pVTab);
  virtual_table->Drop();
  delete virtual_table;
  return SQLITE_OK;
}

int xOpen(sqlite3_vtab *pVTab, sqlite3_vtab_cursor **ppCursor) {
  spdlog::trace(__func__);

  auto virtual_table = reinterpret_cast<VirtualTable *>(pVTab);
  auto cursor = new Cursor(virtual_table);
  *ppCursor = reinterpret_cast<sqlite3_vtab_cursor *>(cursor);
  return SQLITE_OK;
}

int xClose(sqlite3_vtab_cursor *pCur) {
  spdlog::trace(__func__);

  auto cursor = reinterpret_cast<Cursor *>(pCur);
  delete cursor;
  return SQLITE_OK;
}

int xEof(sqlite3_vtab_cursor *pCur) {
  spdlog::trace(__func__);

  auto cursor = reinterpret_cast<Cursor *>(pCur);
  return cursor->IsEOF();
}

int xFilter(sqlite3_vtab_cursor *pCur, int idxNum, const char *idxStr, int argc,
            sqlite3_value **argv) {
  spdlog::trace(__func__);

  return SQLITE_OK;
}

int xNext(sqlite3_vtab_cursor *pCur) {
  spdlog::trace(__func__);

  auto cursor = reinterpret_cast<Cursor *>(pCur);
  cursor->Next();
  return SQLITE_OK;
}

int xColumn(sqlite3_vtab_cursor *pCur, sqlite3_context *pCtx, int N) {
  spdlog::trace(__func__);

  auto cursor = reinterpret_cast<Cursor *>(pCur);
  Schema *schema = cursor->GetVirtualTable()->schema();
  TypeID type_id = schema->GetTypeID(N);
  Value value = cursor->GetValue(schema, N);

  switch (type_id) {
    case TypeID::DOUBLE:
      sqlite3_result_double(pCtx, value.GetAs<double>());
      break;
    case TypeID::INTEGER:
      sqlite3_result_int64(pCtx, value.GetAs<int64_t>());
      break;
    case TypeID::TEXT:
      char *data = value.GetAs<char *>();
      sqlite3_result_text(pCtx, data, -1, SQLITE_TRANSIENT);
      break;
  }
  return SQLITE_OK;
}

int xRowid(sqlite3_vtab_cursor *pCur, sqlite_int64 *pRowid) {
  spdlog::trace(__func__);

  auto cursor = reinterpret_cast<Cursor *>(pCur);
  *pRowid = cursor->GetRowID();
  spdlog::debug("row id: {} {}", cursor->GetRowID() >> 32, int32_t(cursor->GetRowID()));
  return SQLITE_OK;
}

int xUpdate(sqlite3_vtab *pVTab, int argc, sqlite3_value **argv,
            sqlite_int64 *pRowid) {
  spdlog::trace(__func__);

  auto virtual_table = reinterpret_cast<VirtualTable *>(pVTab);

  RowID row_id;
  bool res;
  if (argc == 1) { // delete
    row_id.Set(sqlite3_value_int64(argv[0]));
    spdlog::debug("delete rowID: {} {}", row_id.GetPageID(), row_id.GetSlotNum());
    res = virtual_table->DeleteTuple(row_id);
    assert(res);
  } else {
    Tuple tuple = ConstructTuple(virtual_table->schema(), argc-2, argv+2);

    if (sqlite3_value_type(argv[0]) == SQLITE_NULL) { // insert
      int64_t rowid = sqlite3_value_int64(argv[1]);
      spdlog::debug("argv[1]: {} {}", rowid >> 32, int32_t(rowid));
      res = virtual_table->InsertTuple(row_id, tuple);
      *pRowid = row_id.Get();
      spdlog::debug("inserted rowID: {} {}", row_id.GetPageID(), row_id.GetSlotNum());
    } else if (sqlite3_value_int64(argv[0]) == sqlite3_value_int64(argv[1])) { // update inplace
      row_id.Set(sqlite3_value_int64(argv[0]));
      if (!virtual_table->UpdateTuple(row_id, tuple)) {
        res = virtual_table->DeleteTuple(row_id);
        assert(res);
        res = virtual_table->InsertTuple(row_id, tuple);
        assert(res);
      }
      spdlog::debug("updated rowID: {} {}", row_id.GetPageID(), row_id.GetSlotNum());
    } else { // update not inplace
      spdlog::debug("update(not inplace) start");

      row_id.Set(sqlite3_value_int64(argv[0]));
      res = virtual_table->DeleteTuple(row_id);
      spdlog::debug("deleted rowID: {} {}", row_id.GetPageID(), row_id.GetSlotNum());
      assert(res);

      row_id.Set(sqlite3_value_int64(argv[1]));
      spdlog::debug("insert rowID: {} {}", row_id.GetPageID(), row_id.GetSlotNum());
      res = virtual_table->InsertTuple(row_id, tuple);
      spdlog::debug("inserted rowID: {} {}", row_id.GetPageID(), row_id.GetSlotNum());
      assert(res);
    }
  }

  return SQLITE_OK;
}

sqlite3_module virtual_table_module = {
  0,
  xCreate,
  xConnect,
  xBestIndex,
  xDisconnect,
  xDestroy,
  xOpen,
  xClose,
  xFilter,
  xNext,
  xEof,
  xColumn,
  xRowid,
  xUpdate,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
};

void Destroy(void *pClientData) {
  spdlog::trace(__func__);

  auto storage_engine = reinterpret_cast<StorageEngine *>(pClientData);
  delete storage_engine;
}

#ifdef _WIN32
__declspec(dllexport)
#endif
extern "C" int sqlite3_vtab_init(sqlite3 *db, char **pzErrMsg,
                                 const sqlite3_api_routines *pApi) {
  SQLITE_EXTENSION_INIT2(pApi);

  auto storage_engine = new StorageEngine("db.db");
  return sqlite3_create_module_v2(db, "vtab", &virtual_table_module,
                                  storage_engine, Destroy);
}
