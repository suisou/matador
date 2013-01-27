/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#include "database/mysql/mysql_statement.hpp"
#include "database/mysql/mysql_database.hpp"
#include "database/mysql/mysql_exception.hpp"

#include <stdexcept>
#include <iostream>
#include <sstream>

#include <mysql/mysql.h>

namespace oos {

namespace mysql {

void throw_error(int ec, st_mysql *db, const std::string &source, const std::string &sql = "")
{
  if (ec == 0) {
    return;
  }
  std::stringstream msg;
  msg << source << ": " << mysql_error(db) << "(" << sql << ")";
  throw mysql_exception(msg.str()); 
}

mysql_statement::mysql_statement(mysql_database &db)
  : stmt_(0)
  , db_(db)
{
}

mysql_statement::~mysql_statement()
{
}

bool mysql_statement::step()
{

//  std::cerr << "executing " << sql() << "\n";

  int ret = mysql_stmt_fetch(stmt_);
  if (ret == 0/*SQLITE_ROW*/) {
    // retrieved new row
    // create row object
    return true;
  } else if (ret == 0/*SQLITE_DONE*/) {
    // no further row available
    return false;
  } else {
    // error, throw exception
    throw_error(ret, db_(), "mysql_stmt_next_result");
  }
  return false;
}

void mysql_statement::prepare(const std::string &sql)
{
  // set new sql statement
  statement::prepare(sql);
  // create statement
  stmt_ = mysql_stmt_init(db_());
  // prepare mysql statement
  int ret = mysql_stmt_prepare(stmt_, sql.c_str(), sql.size());
  throw_error(ret, db_(), "mysql_stmt_prepare", sql);
}

void mysql_statement::reset(bool clear_bindings)
{
  mysql_stmt_reset(stmt_);
}

int mysql_statement::column_count() const
{
  return mysql_stmt_field_count(stmt_);
}

const char* mysql_statement::column_name(int) const
{
  throw mysql_exception("unsupported feature");
}

void mysql_statement::column(int i, bool &value) const
{
  /*
  MYSQL_BIND value;
  value.buffer_type     = MYSQL_TYPE_LONG;
	value.buffer         = (void *) &value;
	value.buffer_length    = 0;
	value.is_null         = 0;
	value.length         = 0;

  mysql_stmt_fetch_column
  
  value = sqlite3_column_int(stmt_, i) > 0;
  */
}

void mysql_statement::column(int i, char &value) const
{
}

void mysql_statement::column(int i, float &value) const
{
}

void mysql_statement::column(int i, double &value) const
{
}

void mysql_statement::column(int i, short &value) const
{
}

void mysql_statement::column(int i, int &value) const
{
}

void mysql_statement::column(int i, long &value) const
{
}

void mysql_statement::column(int i, unsigned char &value) const
{
}

void mysql_statement::column(int i, unsigned short &value) const
{
}

void mysql_statement::column(int i, unsigned int &value) const
{
}

void mysql_statement::column(int i, unsigned long &value) const
{
}

void mysql_statement::column(int i, char *value, int &len) const
{
}

void mysql_statement::column(int i, std::string &value) const
{
}

int mysql_statement::bind(int i, double value)
{
  int ret = 1;
  throw_error(ret, db_(), "sqlite3_bind_double");
  return ret;
}

int mysql_statement::bind(int i, int value)
{
  int ret = 1;
  throw_error(ret, db_(), "sqlite3_bind_int");
  return ret;
}

int mysql_statement::bind(int i, long value)
{
  int ret = 1;
  throw_error(ret, db_(), "sqlite3_bind_int");
  return ret;
}

int mysql_statement::bind(int i, unsigned int value)
{
  int ret = 1;
  throw_error(ret, db_(), "sqlite3_bind_int");
  return ret;
}

int mysql_statement::bind(int i, unsigned long value)
{
  int ret = 1;
  throw_error(ret, db_(), "sqlite3_bind_int");
  return ret;
}

int mysql_statement::bind(int i, const char *value, int len)
{
  int ret = 1;
  throw_error(ret, db_(), "sqlite3_bind_text");
  return ret;
}

int mysql_statement::bind(int i, const std::string &value)
{
  int ret = 1;
  throw_error(ret, db_(), "sqlite3_bind_text");
  return ret;
}

int mysql_statement::bind_null(int i)
{
  int ret = 1;
  throw_error(ret, db_(), "sqlite3_bind_null");
  return ret;
}

database& mysql_statement::db()
{
  return db_;
}

const database& mysql_statement::db() const
{
  return db_;
}

}

}