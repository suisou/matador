//
// Created by sascha on 5/17/16.
//

#include "matador/sql/field.hpp"

namespace matador {


field::field() { }

field::field(const char *name)
  : name_(name)
{}

field::field(const std::string &name)
  : name_(name)
{}

field::~field() { }

size_t field::index() const
{
  return index_;
}

void field::index(size_t i)
{
  index_ = i;
}

std::string field::name() const
{
  return name_;
}

void field::name(const std::string &n)
{
  name_ = n;
}

database_type field::type() const
{
  return type_;
}

void field::type(database_type t)
{
  type_ = t;
}

std::size_t field::size() const
{
  return size_;
}

void field::size(std::size_t s)
{
  size_ = s;
}

std::size_t field::precision() const
{
  return precision_;
}

void field::precision(std::size_t p)
{
  precision_ = p;
}

bool field::is_not_null() const
{
  return not_null_;
}

void field::not_null(bool nn)
{
  not_null_ = nn;
}

std::string field::default_value() const
{
  return default_value_;
}

void field::default_value(const std::string &value)
{
  default_value_ = value;
}

}