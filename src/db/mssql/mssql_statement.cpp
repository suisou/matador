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

#include "matador/db/mssql/mssql_statement.hpp"
#include "matador/db/mssql/mssql_connection.hpp"
#include "matador/db/mssql/mssql_result.hpp"

#include "matador/utils/identifiable_holder.hpp"
#include "matador/utils/memory.hpp"

namespace matador {

namespace mssql {

mssql_statement::mssql_statement(mssql_connection &db, const matador::sql &stmt)
  : statement_impl(db.dialect(), stmt)
  , db_(db.handle())
{
  if (!db_) {
    throw_error("mssql", "no odbc connection established");
  }

  create_statement();
}

mssql_statement::~mssql_statement()
{
  clear();
}

void mssql_statement::reset()
{
}

void mssql_statement::clear()
{
  reset();
  SQLFreeHandle(SQL_HANDLE_STMT, stmt_);
}

detail::result_impl* mssql_statement::execute()
{
  SQLRETURN ret = SQLExecute(stmt_);
  // check if data is needed
  if (ret == SQL_NEED_DATA) {
    // put needed data from host_data

    // get first data to put
    PTR pid{ nullptr };
    ret = SQLParamData(stmt_, &pid);
    // get data from map
    auto it = binder_->data_to_put_map().find(pid);
    if (it == binder_->data_to_put_map().end()) {
      throw_error("mssql", "couldn't find data to put");
    }
    mssql_parameter_binder::value_t *val = it->second;
    // Todo
    // put data as long if it is requested
    while (ret == SQL_NEED_DATA) {
      while (val->len > 256) {
        ret = SQLPutData(stmt_, val->data, 256);
        val->len -= 256;
        val->data += 256;
      }
      ret = SQLPutData(stmt_, val->data, val->len);
      ret = SQLParamData(stmt_, &pid);
      if (!is_success(ret) && ret != SQL_NEED_DATA) {
        // error
        throw_error(ret, SQL_HANDLE_STMT, stmt_, "execute->SQLParamData", str());
      } else if (ret == SQL_NEED_DATA) {
        // determine next column data pointer
        it = binder_->data_to_put_map().find(pid);
        if (it == binder_->data_to_put_map().end()) {
          throw_error("mssql", "couldn't find data to put");
        }
        val = it->second;
      }
    }
  } else {
    // check result
    throw_error(ret, SQL_HANDLE_STMT, stmt_, str(), "error on query execute");
  }

  binder_->reset();

  auto *res = new mssql_result(stmt_);

  create_statement();

  return res;
}

void mssql_statement::create_statement()
{
  // create statement handle
  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, db_, &stmt_);
  throw_error(ret, SQL_HANDLE_DBC, db_, "mssql", "error on creating sql statement");

  ret = SQLPrepare(stmt_, (SQLCHAR*)str().c_str(), SQL_NTS);
  throw_error(ret, SQL_HANDLE_STMT, stmt_, str());

  binder_ = matador::make_unique<mssql_parameter_binder>(stmt_);
}

detail::parameter_binder_impl* mssql_statement::binder() const
{
  return binder_.get();
}

}

}
