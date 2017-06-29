#include "matador/object/relation_endpoint_value_inserter.hpp"
#include "matador/object/object_holder.hpp"
#include "matador/object/has_many.hpp"

namespace matador {
namespace detail {

template < class Value >
template<class Owner>
void relation_endpoint_value_inserter<Value>::insert(const object_ptr <Owner> &owner, const std::string &field, const has_many_item_holder<Value> &holder)
{
  field_ = field;
  holder_ = holder;

  matador::access::serialize(*this, *owner);

  field_.clear();
}

template < class Value >
void relation_endpoint_value_inserter<Value>::serialize(const char *id, object_holder &x, cascade_type cascade)
{
  if (field_ != id) {
    return;
  }
  x.reset(holder_.value().proxy_, cascade, false);
}

template < class Value >
template < template < class ... > class Container >
void relation_endpoint_value_inserter<Value>::serialize(const char *id, has_many<Value, Container> &x, const char *, const char *)
{
  if (field_ != id) {
    return;
  }
  x.insert_holder(holder_);
}

}
}