//
// Created by sascha on 6/2/17.
//

#ifndef MATADOR_RELATION_ENDPOINT_VALUE_REMOVER_HPP
#define MATADOR_RELATION_ENDPOINT_VALUE_REMOVER_HPP

#include "matador/object/has_many_item_holder.hpp"

#include <iostream>

namespace matador {
namespace detail {

template < class Value >
class relation_endpoint_value_remover
{
public:
  template < class Owner >
  void remove(const object_ptr<Owner> &owner, const std::string &field, const has_many_item_holder<Value> &holder);

  template < class T >
  void serialize(T &x)
  {
    matador::access::serialize(*this, x);
  }

  template < class T >
  void serialize(const char *, T &) {}
  void serialize(const char *, char *, size_t) {}
  void serialize(const char *id, object_holder &x, cascade_type);
  template < template < class ... > class Container >
  void serialize(const char *id, has_many<Value, Container> &, const char*, const char*);

private:
  std::string field_;

  has_many_item_holder<Value> holder_;
};

}
}
#endif //MATADOR_RELATION_ENDPOINT_VALUE_REMOVER_HPP
