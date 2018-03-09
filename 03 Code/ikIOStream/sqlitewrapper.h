#ifndef SQLITEWRAPPER_H
#define SQLITEWRAPPER_H
/*
   SQLiteWrapper.h

   Copyright (C) 2004 René Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   René Nyffenegger rene.nyffenegger@adp-gmbh.ch
*/

#include <string>
#include <vector>

#include "sqlite3.h"

class SQLiteStatement {
  private:
    // SQLiteStatement's ctor only to be called by SQLiteWrapper
    friend class SQLiteWrapper;
    SQLiteStatement(std::string const& statement, sqlite3* db);

  public:
    SQLiteStatement();

    enum dataType {
      INT = SQLITE_INTEGER,
      FLT = SQLITE_FLOAT  ,
      TXT = SQLITE_TEXT   ,
      BLB = SQLITE_BLOB   ,
      NUL = SQLITE_NULL   ,
    };

    dataType DataType(int pos_zero_indexed);

    int         ValueInt   (int pos_zero_indexed);
    std::string ValueString(int pos_zero_indexed);

//    SQLiteStatement(const SQLiteStatement&);
   ~SQLiteStatement();

    //SQLiteStatement& operator=(SQLiteStatement const&);

    bool Bind    (int pos_zero_indexed, std::string const& value);
    bool Bind    (int pos_zero_indexed, double             value);
    bool Bind    (int pos_zero_indexed, int                value);
    bool BindNull(int pos_zero_indexed);

    bool Execute();

    bool NextRow();

    /*   Call Reset if not depending on the NextRow cleaning up.
         For example for select count(*) statements*/
    bool Reset();

    bool RestartSelect();

  private:
    //void DecreaseRefCounter();

    //int* ref_counter_; // not yet used...
    sqlite3_stmt* _stmt;
};

class SQLiteWrapper {
  public:
    SQLiteWrapper();

    bool Open(std::string const& db_file);

    class ResultRecord {
      public:
        std::vector<std::string> _fields;
    };

    class ResultTable {
      friend class SQLiteWrapper;
      public:
        ResultTable() : _ptrCurRecord(0) {}

        std::vector<ResultRecord> _records;

         ResultRecord* Next() {
          if (_ptrCurRecord < _records.size()) {
            return &(_records[_ptrCurRecord++]);
          }
          return 0;
        }

      private:
         void Reset() {
           _records.clear();
           _ptrCurRecord=0;
         }

      private:
        unsigned int _ptrCurRecord;
    };

    bool SelectStmt           (std::string const& stmt, ResultTable& res);
    bool DirectStatement      (std::string const& stmt);
    SQLiteStatement* Statement(std::string const& stmt);

    std::string LastError();

    // Transaction related
    bool Begin   ();
    bool Commit  ();
    bool Rollback();

  private:

    static int SelectCallback(void *p_data, int num_fields, char **p_fields, char **p_col_names);

    sqlite3* _db;
};

#endif // SQLITEWRAPPER_H
