//
// Created by sascha on 04.06.19.
//

#include <matador/sql/query.hpp>

#include "../person.hpp"

#include "connections.hpp"

#include "matador/sql/connection.hpp"
#include "matador/sql/sql.hpp"
#include "matador/sql/column.hpp"
#include "matador/sql/dialect_token.hpp"

#include "PostgreSQLDialectTestUnit.hpp"

using namespace matador;

PostgreSQLDialectTestUnit::PostgreSQLDialectTestUnit()
  : unit_test("postgresql_dialect" , "postgresql dialect test")
{
  add_test("placeholder", std::bind(&PostgreSQLDialectTestUnit::test_placeholder, this), "test postgresql placeholder link");
  add_test("placeholder_condition", std::bind(&PostgreSQLDialectTestUnit::test_placeholder_condition, this), "test postgresql placeholder in condition link");
  add_test("update_limit", std::bind(&PostgreSQLDialectTestUnit::test_update_limit, this), "test postgresql limit");
  add_test("update_limit_prepare", std::bind(&PostgreSQLDialectTestUnit::test_update_limit_prepare, this), "test postgresql prepared limit");
  add_test("tablename", std::bind(&PostgreSQLDialectTestUnit::test_table_name, this), "test postgresql extract table name");
}

void PostgreSQLDialectTestUnit::test_placeholder()
{
  matador::connection conn(::connection::postgresql);

  sql s;

  s.append(new detail::insert("person"));

  std::unique_ptr<matador::columns> cols(new columns(columns::WITH_BRACKETS));

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  std::unique_ptr<matador::detail::values> vals(new detail::values);

  unsigned long id(8);
  std::string name("hans");
  unsigned int age(25);

  vals->push_back(std::make_shared<value>(id));
  vals->push_back(std::make_shared<value>(name));
  vals->push_back(std::make_shared<value>(age));

  s.append(vals.release());

  auto result = conn.dialect()->prepare(s);

  UNIT_ASSERT_EQUAL("INSERT INTO \"person\" (\"id\", \"name\", \"age\") VALUES ($1, $2, $3) ", std::get<0>(result));
}

void PostgreSQLDialectTestUnit::test_placeholder_condition()
{
  matador::connection conn(::connection::postgresql);

  sql s;

  s.append(new detail::select());

  std::unique_ptr<matador::columns> cols(new columns(columns::WITHOUT_BRACKETS));

  cols->push_back(std::make_shared<column>("id"));
  cols->push_back(std::make_shared<column>("name"));
  cols->push_back(std::make_shared<column>("age"));

  s.append(cols.release());

  s.append(new detail::from("person"));

  s.append(new detail::where("name"_col == "hans"));

  auto result = conn.dialect()->prepare(s);

  UNIT_ASSERT_EQUAL("SELECT \"id\", \"name\", \"age\" FROM \"person\" WHERE \"name\" = $1 ", std::get<0>(result));
}

void PostgreSQLDialectTestUnit::test_update_limit()
{
  matador::connection conn(::connection::postgresql);

  sql s;

  s.append(new detail::update);
  s.append(new detail::tablename("relation"));
  s.append(new detail::set);

  std::unique_ptr<matador::columns> cols(new columns(columns::WITHOUT_BRACKETS));

  unsigned long owner_id(1);
  cols->push_back(std::make_shared<detail::value_column<unsigned long>>("owner_id", owner_id));

  s.append(cols.release());

  s.append(new detail::where("owner_id"_col == 1 && "item_id"_col == 1));
  s.append(new detail::top(1));

  auto result = conn.dialect()->direct(s);

  UNIT_ASSERT_EQUAL("UPDATE \"relation\" SET \"owner_id\"=1 WHERE \"ctid\" = (SELECT \"ctid\" FROM \"relation\" WHERE (\"owner_id\" = 1 AND \"item_id\" = 1) LIMIT 1 ) ", result);
}

void PostgreSQLDialectTestUnit::test_update_limit_prepare(){
  matador::connection conn(::connection::postgresql);

  sql s;

  s.append(new detail::update);
  s.append(new detail::tablename("relation"));
  s.append(new detail::set);

  std::unique_ptr<matador::columns> cols(new columns(columns::WITHOUT_BRACKETS));

  unsigned long owner_id(1);
  cols->push_back(std::make_shared<detail::value_column<unsigned long>>("owner_id", owner_id));

  s.append(cols.release());

  s.append(new detail::where("owner_id"_col == 1 && "item_id"_col == 1));
  s.append(new detail::top(1));

  auto result = conn.dialect()->prepare(s);

  UNIT_ASSERT_EQUAL("UPDATE \"relation\" SET \"owner_id\"=$1 WHERE \"ctid\" = (SELECT \"ctid\" FROM \"relation\" WHERE (\"owner_id\" = $2 AND \"item_id\" = $3) LIMIT 1 ) ", std::get<0>(result));
}

void PostgreSQLDialectTestUnit::test_table_name()
{
  query<person> q("person");

  column id("id");

  q.select();

  UNIT_ASSERT_EQUAL("person", q.stmt().table_name());
}
