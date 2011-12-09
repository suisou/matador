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

#ifndef OBJECT_LIST_HPP
#define OBJECT_LIST_HPP

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
#else
  #define OOS_API
#endif

#include "object/object.hpp"
#include "object/object_ptr.hpp"
#include "object/object_store.hpp"
#include "object/object_linker.hpp"
#include "object/object_atomizer.hpp"

#ifdef WIN32
#include <functional>
#else
#include <tr1/functional>
#endif

#include <list>
#include <algorithm>

/*
 *   ObjectList layout example:
 * 
 *   ID | FIRST | LAST |PREV | NEXT | data
 *   ---+-------+------+-----+------+------+
 *    1 |   1   |   5  |  0  |  2   |    0 |
 *    2 |   1   |   5  |  1  |  3   |    7 |
 *    3 |   1   |   5  |  2  |  4   |    8 |
 *    4 |   1   |   5  |  3  |  5   |    9 |
 *    5 |   1   |   5  |  4  |  0   |    0 |
 */
 
namespace oos {

template < class T > class linked_object_list;
template < class T > class object_list_iterator;
template < class T > class const_object_list_iterator;

/**
 * @class object_list_base_node
 * @brief Base list node class for all linked list types
 * 
 * This is the base list node class for all further
 * linked list type classes storing objects
 */
class object_list_base_node : public object
{
public:
  object_list_base_node() {}
  virtual ~object_list_base_node() {}

  /**
   * Returns list nodes previous list node
   * 
   * @return The previous list node
   */
  virtual object_list_base_node *next_node() const = 0;

  /**
   * Returns list nodes next list node
   * 
   * @return The next list node
   */
  virtual object_list_base_node *prev_node() const = 0;
};

/**
 * @class object_list_node
 * @brief List node class for linked list
 * @tparam T List node type.
 * 
 * This class represents the list node class
 * for the linked list. It contains already
 * fields for next and preivious nodes as well
 * as fields for the first and last list node.
 */
template < class T >
class object_list_node : public object_list_base_node
{
public:
  object_list_node() {}
  virtual ~object_list_node() {}

  /**
   * @brief Reads the data from the object_atomizer
   * 
   * All data next, previous, first and last node
   * is read in from the given object_atomizer.
   * 
   * @param reader The object_atomizer object to read from.
   */
	void read_from(object_atomizer *reader)
  {
    object::read_from(reader);
    reader->read_object("first", first_);
    reader->read_object("last", last_);
    reader->read_object("prev", prev_);
    reader->read_object("next", next_);
  }

  /**
   * @brief Writes the data to the object_atomizer
   * 
   * All data next, previous, first and last node
   * is written to the given object_atomizer.
   * 
   * @param writer The object_atomizer object to write to.
   */
	void write_to(object_atomizer *writer) const
  {
    object::write_to(writer);
    writer->write_object("first", first_);
    writer->write_object("last", last_);
    writer->write_object("prev", prev_);
    writer->write_object("next", next_);
  }

  /**
   * Returns nodes next node as object_list_base_node.
   * 
   * @return Nodes next node.
   */
  virtual object_list_base_node *next_node() const
  {
    return next_.get();
  }

  /**
   * Returns nodes previous node as object_list_base_node.
   * 
   * @return Nodes previous node.
   */
  virtual object_list_base_node *prev_node() const
  {
    return prev_.get();
  }

  /**
   * Returns lists first node.
   * 
   * @return Lists first node.
   */
  object_ref<T> first() const
  {
    return first_;
  }

  /**
   * Returns lists last node.
   * 
   * @return Lists last node.
   */
  object_ref<T> last() const
  {
    return last_;
  }

  /**
   * Returns nodes previous node.
   * 
   * @return Nodes previous node.
   */
  object_ref<T> prev() const
  {
    return prev_;
  }

  /**
   * Returns nodes next node.
   * 
   * @return Nodes next node.
   */
  object_ref<T> next() const
  {
    return next_;
  }

protected:
  /**
   * Sets lists first node.
   * 
   * @param f Lists first node.
   */
  void first(const object_ref<T> &f)
  {
    mark_modified();
    first_ = f;
  }

  /**
   * Sets lists last node.
   * 
   * @param f Lists last node.
   */
  void last(const object_ref<T> &l)
  {
    mark_modified();
    last_ = l;
  }

  /**
   * Sets nodes previous node.
   * 
   * @param f Nodes previous node.
   */
  void prev(const object_ref<T> &p)
  {
    mark_modified();
    prev_ = p;
  }

  /**
   * Sets nodes next node.
   * 
   * @param f Nodes next node.
   */
  void next(const object_ref<T> &n)
  {
    mark_modified();
    next_ = n;
  }

private:
  friend class linked_object_list<T>;
  friend class object_list_iterator<T>;
  friend class const_object_list_iterator<T>;

  object_ref<T> first_;
  object_ref<T> last_;
  object_ref<T> prev_;
  object_ref<T> next_;
};

/**
 * @class object_ptr_list_node
 * @brief Node type for linked lists with an object_ptr as value.
 * @tparam T The object type of the object_ptr class.
 * 
 * This node class is specialization for the object_list_node class
 * for linked object lists. It contains only an object_ptr and wraps
 * it with direct accessors.
 */
template < class T >
class object_ptr_list_node : public object_list_node<object_ptr_list_node<T> >
{
public:
  typedef T value_type; /**< Shortcut for the nodes value type. */

  /**
   * @brief Creates a new object_ptr_list_node
   *
   * Creates a new linked list node for object_ptr
   * the default parameter name for the object is
   * "object".
   */
  object_ptr_list_node() : name_("object") {}

  /**
   * @brief Creates a new object_ptr_list_node
   *
   * Creates a new linked list node for object_ptr
   * the parameter name for the object must be given.
   * 
   * @param name Name of the parameter for object_atomizer.
   */
  object_ptr_list_node(const std::string &name)
    : name_(name)
  {}

  /**
   * @brief Creates a new object_ptr_list_node
   *
   * Creates a new linked list node for object_ptr
   * the parameter name for the object must be given.
   * Also the value object_ptr will be set.
   * 
   * @param optr The object value for this node.
   * @param name Name of the parameter for object_atomizer.
   */
  object_ptr_list_node(object_ptr<T> optr, const std::string &name)
    : object_(optr)
    , name_(name)
  {}

  virtual ~object_ptr_list_node() {}

  /**
   * @brief Reads the data from the object_atomizer.
   * 
   * In addition to the base fields, the object_ptr is read
   * from the object_atomizer.
   * 
   * @param reader The object_reader to read from.
   */
	void read_from(object_atomizer *reader)
  {
    object_list_node<object_ptr_list_node<T> >::read_from(reader);
    reader->read_object(name_.c_str(), object_);
  }

  /**
   * @brief Writes the data to the object_atomizer.
   * 
   * In addition to the base fields, the object_ptr is written
   * to the object_atomizer.
   * 
   * @param reader The object_reader to write to.
   */
	void write_to(object_atomizer *writer) const
  {
    object_list_node<object_ptr_list_node<T> >::write_to(writer);
    writer->write_string(name_.c_str(), object_);
  }

  /**
   * Returns the object value.
   * 
   * @return The object_ptr value.
   */
  object_ptr<T> optr() const {
    return object_;
  }

private:
  object_ptr<T> object_;
  std::string name_;
};

/**
 * @class object_ref_list_node
 * @brief Node type for linked lists with an object_ref as value.
 * @tparam T The object type of the object_ref class.
 * 
 * This node class is specialization for the object_list_node class
 * for linked object lists. It contains only an object_ref and wraps
 * it with direct accessors.
 */
template < class T >
class object_ref_list_node : public object_list_node<object_ref_list_node<T> >
{
public:
  typedef T value_type; /**< Shortcut for the nodes value type. */

  /**
   * @brief Creates a new object_ref_list_node
   *
   * Creates a new linked list node for object_ref
   * the default parameter name for the object is
   * "object".
   */
  object_ref_list_node() : name_("object") {}

  /**
   * @brief Creates a new object_ref_list_node
   *
   * Creates a new linked list node for object_ref
   * the parameter name for the object must be given.
   * 
   * @param name Name of the parameter for object_atomizer.
   */
  object_ref_list_node(const std::string &name)
    : name_(name)
  {}

  /**
   * @brief Creates a new object_ref_list_node
   *
   * Creates a new linked list node for object_ref
   * the parameter name for the object must be given.
   * Also the value object_ref will be set.
   * 
   * @param optr The object_ref value for this node.
   * @param name Name of the parameter for object_atomizer.
   */
  object_ref_list_node(object_ref<T> oref, const std::string &name)
    : object_(oref)
    , name_(name)
  {}

  virtual ~object_ref_list_node() {}

  /**
   * @brief Reads the data from the object_atomizer.
   * 
   * In addition to the base fields, the object_ref is read
   * from the object_atomizer.
   * 
   * @param reader The object_reader to read from.
   */
	void read_from(object_atomizer *reader)
  {
    object_list_node<object_ref_list_node<T> >::read_from(reader);
    reader->read_object(name_.c_str(), object_);
  }

  /**
   * @brief Writes the data to the object_atomizer.
   * 
   * In addition to the base fields, the object_ref is written
   * to the object_atomizer.
   * 
   * @param reader The object_reader to write to.
   */
	void write_to(object_atomizer *writer) const
  {
    object_list_node<object_ref_list_node<T> >::write_to(writer);
    writer->write_object(name_.c_str(), object_);
  }

  /**
   * Returns the object value.
   * 
   * @return The object_ref value.
   */
  object_ref<T> oref() const {
    return object_;
  }

private:
  object_ref<T> object_;
  std::string name_;
};

/**
 * @class object_list_iterator
 * @brief Iterator class for all linked lists.
 * @tparam T Containing list node type of the iterator.
 * 
 * This is the iterator class used by all linked
 * list types.
 */
template < class T >
class object_list_iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
public:
  typedef object_list_iterator<T> self;	   /**< Shortcut for this iterator type. */
  typedef T* pointer;                      /**< Shortcut for the pointer type. */
  typedef object_ptr<T> value_type;        /**< Shortcut for the value type. */
  typedef value_type& reference ;          /**< Shortcut for the reference to the value type. */
  typedef linked_object_list<T> list_type; /**< Shortcut for the list type. */

  /**
   * Creates an empty iterator
   */
  object_list_iterator()
    : node_(NULL)
    , list_(NULL)
  {}
  
  /**
   * @brief Creates an iterator from a list node for a list
   * 
   * The constructor creates an iterator for a given
   * list node in a given list.
   * 
   * @param node The linked list node.
   * @param l The pointer to the list.
   */
  object_list_iterator(const value_type &node, list_type *l)
    : node_(node)
    , list_(l)
  {}

  /**
   * Copies an iterator.
   * 
   * @param x The iterator to copy from.
   */
  object_list_iterator(const object_list_iterator &x)
    : node_(x.node_)
    , list_(x.list_)
  {}

  /**
   * Assign an iterator to this iterator.
   * 
   * @param x The iterator to assign from.
   */
  object_list_iterator& operator=(const object_list_iterator &x)
  {
    node_ = x.node_;
    list_ = x.list_;
    return *this;
  }

  ~object_list_iterator() {}

  /**
   * @brief Compares this iterator with another on equality.
   * 
   * The equal operator compares this iterator
   * with a given one. The two iterators are equal
   * if the two nodes are the same.
   * 
   * @param i The iterator to compare with.
   * @return Return true if the iterators are equal.
   */
  bool operator==(const object_list_iterator &i) const {
    return (node_ == i.node_);
  }

  /**
   * @brief Compares this iterator with another on unequality.
   * 
   * The not equal operator compares this iterator
   * with a given one. The two iterators are not equal
   * if the two nodes aren't the same.
   * 
   * @param i The iterator to compare with.
   * @return Return true if the iterators are not equal.
   */
  bool operator!=(const object_list_iterator &i) const {
    return (node_ != i.node_);
  }

  /**
   * Pre increments the iterator
   * 
   * @return Returns iterators successor.
   */
  self& operator++() {
    increment();
    return *this;
  }

  /**
   * Post increments the iterator
   * 
   * @return Returns iterator before incrementing.
   */
  self operator++(int) {
    self tmp = *this;
    increment();
    return tmp;
  }

  /**
   * Pre decrements the iterator
   * 
   * @return Returns iterators predeccessor.
   */
  self& operator--() {
    decrement();
    return *this;
  }

  /**
   * Post decrements the iterator
   * 
   * @return Returns iterator before decrementing.
   */
  self operator--(int) {
    self tmp = *this;
    decrement();
    return tmp;
  }

  /**
   * Returns the pointer to the node.
   * 
   * @return The pointer to the node.
   */
  pointer operator->() const {
    return node_.get();
  }

  /**
   * Returns the node.
   * 
   * @return The iterators underlaying node.
   */
  value_type operator*() const {
    return this->optr();
  }

  /**
   * Returns the node as object_ptr.
   * 
   * @return The iterators underlaying node as object_ptr.
   */
  object_ptr<T> optr() const {
    return node_;
  }

  /**
   * Returns the node as object_ref.
   * 
   * @return The iterators underlaying node as object_ref.
   */
  object_ref<T> oref() const {
    return node_;
  }

private:
  void increment() {
    if (node_ != list_->last_) {
      node_ = node_->next_;
    }
  }
  void decrement() {
    if (node_ != list_->last_) {
      node_ = node_->prev_;
    }
  }

private:
  friend class const_object_list_iterator<T>;

  value_type node_;
  list_type *list_;
};

/**
 * @class const_object_list_iterator
 * @brief Constant Iterator class for all linked lists.
 * @tparam T Containing list node type of the iterator.
 * 
 * This is the iterator class used by all linked
 * list types. The containing object is treated
 * as a constant.
 */
template < class T >
class const_object_list_iterator : public std::iterator<std::bidirectional_iterator_tag, T, std::ptrdiff_t, const T*, const T&> {
public:
  typedef const_object_list_iterator<T> self;	/**< Shortcut for this iterator type. */
  typedef T* pointer;                         /**< Shortcut for the pointer type. */
  typedef object_ptr<T> value_type;           /**< Shortcut for the value type. */
  typedef value_type& reference;              /**< Shortcut for the reference to the value type. */
  typedef linked_object_list<T> list_type;    /**< Shortcut for the list type. */

  /**
   * Creates an empty const_iterator
   */
  const_object_list_iterator()
    : node_(NULL)
    , list_(NULL)
  {}

  /**
   * @brief Creates an const_iterator from a list node for a list
   * 
   * The constructor creates an const_iterator for a given
   * list node in a given list.
   * 
   * @param node The linked list node.
   * @param l The pointer to the list.
   */
  const_object_list_iterator(value_type node, const list_type *l)
    : node_(node)
    , list_(l)
  {}

  /**
   * Copies an const_iterator.
   * 
   * @param x The iterator to copy from.
   */
  const_object_list_iterator(const object_list_iterator<T> &x)
    : node_(x.node_)
    , list_(x.list_)
  {}

  /**
   * Assign an iterator to this iterator.
   * 
   * @param x The iterator to assign from.
   */
  const_object_list_iterator& operator=(const object_list_iterator<T> &x)
  {
    node_ = x.node_;
    list_ = x.list_;
    return *this;
  }

  /**
   * Assign an const_iterator to this const_iterator.
   * 
   * @param x The const_iterator to assign from.
   */
  const_object_list_iterator& operator=(const const_object_list_iterator &x)
  {
    node_ = x.node_;
    list_ = x.list_;
    return *this;
  }

  ~const_object_list_iterator() {}

  /**
   * @brief Compares this iterator with another on equality.
   * 
   * The equal operator compares this iterator
   * with a given one. The two iterators are equal
   * if the two nodes are the same.
   * 
   * @param i The iterator to compare with.
   * @return Return true if the iterators are equal.
   */
  bool operator==(const const_object_list_iterator &i) const {
    return (node_ == i.node_);
  }

  /**
   * @brief Compares this iterator with another on unequality.
   * 
   * The not equal operator compares this iterator
   * with a given one. The two iterators are not equal
   * if the two nodes aren't the same.
   * 
   * @param i The iterator to compare with.
   * @return Return true if the iterators are not equal.
   */
  bool operator!=(const const_object_list_iterator &i) const {
    return (node_ != i.node_);
  }

  /**
   * Pre increments the iterator
   * 
   * @return Returns iterators successor.
   */
  self& operator++() {
    increment();
    return *this;
  }

  /**
   * Post increments the iterator
   * 
   * @return Returns iterator before incrementing.
   */
  self operator++(int) {
    self tmp = *this;
    increment();
    return tmp;
  }

  /**
   * Pre decrements the iterator
   * 
   * @return Returns iterators predeccessor.
   */
  self& operator--() {
    decrement();
    return *this;
  }

  /**
   * Post decrements the iterator
   * 
   * @return Returns iterator before decrementing.
   */
  self operator--(int) {
    self tmp = *this;
    decrement();
    return tmp;
  }

  /**
   * Returns the pointer to the node.
   * 
   * @return The pointer to the node.
   */
  pointer operator->() const {
    pointer p = node_.get();
    return p;
  }

  /**
   * Returns the node.
   * 
   * @return The iterators underlaying node.
   */
  value_type operator*() const {
    return this->oref();
  }

  /**
   * Returns the node as object_ref.
   * 
   * @return The iterators underlaying node as object_ref.
   */
  object_ref<T> oref() const {
    return node_;
  }

  /**
   * Returns the node as object_ptr.
   * 
   * @return The iterators underlaying node as object_ptr.
   */
  object_ptr<T> optr() const {
    return node_;
  }

private:
  void increment() {
    if (node_ != list_->last_) {
      node_ = node_->next_;
    }
  }
  void decrement() {
    if (node_ != list_->last_) {
      node_ = node_->prev_;
    }
  }

private:
  value_type node_;
  const list_type *list_;
};

/**
 * @class object_list_base
 * @brief Base class of all list types
 * 
 * This is the base class of all list types.
 */
class OOS_API object_list_base
{
public:
  typedef std::tr1::function<void (object *)> node_func; /**< Shortcut to the function type of the for_each method. */

public:
  /**
   * @brief Creates an empty object_list_base object.
   * 
   * The constructor creates an empty object_list_base
   * object which is part of the given parent object. This
   * means that the parent object contains this list and
   * each node object must have a reference field/parameter
   * for the parent object set automatically on insert.
   * 
   * @param parent The parent object of this list.
   */
  object_list_base(object *parent);

	virtual ~object_list_base();

  /**
   * Reads the data in from the given object_atomizer.
   * 
   * @param ao The object_atomizer to read from.
   */
	//virtual void read_from(object_atomizer *ao) = 0;

  /**
   * Writes the data to the given object_atomizer.
   * 
   * @param ao The object_atomizer to write to.
   */
	//virtual void write_to(object_atomizer *ao) const = 0;

  /**
   * Tell wether the list is empty or not.
   * 
   * @return Returns true if the list is empty.
   */
  virtual bool empty() const = 0;

  /**
   * @brief Clears the list.
   * 
   * Removes every element from the list and
   * subsequently from the object_store.
   */
  virtual void clear() = 0;
  
  /**
   * Returns the size of the list.
   * 
   * @return The size of the list.
   */
  virtual size_t size() const = 0;

  /**
   * @brief Returns the containing object_store.
   * 
   * Returns the containing object_store. If the list
   * isn't inserted into an object_store NULL is
   * returned.
   * 
   * @return The containing object_store.
   */
  object_store* ostore() const { return ostore_; }

protected:
  friend class object_store;
  friend class object_creator;
  friend class object_deleter;
  friend class object_serializer;

  /**
   * @param 
   */
  virtual void push_front_proxy(object_proxy *) = 0;
  virtual void push_back_proxy(object_proxy *) = 0;

  // mark modified object containig the list
  void mark_modified(object *o);

  /**
   * @brief Executes the given function object for all elements.
   *
   * Executes the given function object for all elements.
   *
   * @param nf Function object used to be executed on each element.
   */
  virtual void for_each(const node_func &nf) const = 0;

  /**
   * Returns the list containing object.
   *
   * @return The list containing object.
   */
  object* parent_object() const { return parent_; }

  /**
   * Sets the list containing object.
   *
   * @param parent The list containing object.
   */
  void parent_object(object *parent) { parent_ = parent; }

  /**
   * Provides an interface which is called
   * when inserting the list into the object_store.
   */
  virtual void install(object_store *ostore);

  /**
   * Provides an interface which is called
   * when removing the list from the object_store.
   */
  virtual void uninstall();  

private:
  virtual void reset() {}

private:
  object_store *ostore_;
  object *parent_;
};

/**
 * @class object_list
 * @brief An object list class.
 * @tparam T The concrete object type.
 * @tparam W The object pointer or reference class.
 * 
 * The object_list class is the base class for
 * The object_ptr_list class and the object_ref_list
 * class.
 * It stores a wrapper (object_ptr or object_ref) object
 * around the concrete object type inside a list.
 * The class provides STL like behaviour and the order of
 * the elements is not reliable.
 */
template < typename T, class W >
class object_list : public object_list_base
{
public:
  typedef object_list_base base_list;                         /**< Shortcut for the object_base_list class. */
  typedef T value_type;                                       /**< Shortcut for the value type. */
  typedef W value_type_wrapper;                               /**< Shortcut for the wrapper class around the value type. */
  typedef std::list<value_type_wrapper> list_type;            /**< Shortcut for the list class member. */
  typedef typename list_type::iterator iterator;              /**< Shortcut for the list iterator. */
  typedef typename list_type::const_iterator const_iterator;  /**< Shortcut for the list const iterator. */

  /**
   * @brief Creates a new object_list.
   * 
   * A new object_list is created. The list is part
   * of the given parent object and therefor a reference
   * to the parent object must be found inside the value
   * type object with the given list_ref_name.
   * 
   * @param parent The containing list object.
   * @param list_ref_name The name of the parent in the value type object.
   */
  object_list(object *parent, const std::string &list_ref_name)
    : object_list_base(parent)
    , list_name_(list_ref_name)
  {}

  virtual ~object_list() {}
  
  /*
	virtual void read_from(object_atomizer *) {}
	virtual void write_to(object_atomizer *) const {}
  */

  /**
   * Return the begin iterator of the list.
   * 
   * @return The begin iterator.
   */
  iterator begin() {
    return object_list_.begin();
  }

  /**
   * Return the begin iterator of the list.
   * 
   * @return The begin iterator.
   */
  const_iterator begin() const {
    return object_list_.begin();
  }

  /**
   * Return the end iterator of the list.
   * 
   * @return The end iterator.
   */
  iterator end() {
    return object_list_.end();
  }

  /**
   * Return the end iterator of the list.
   * 
   * @return The end iterator.
   */
  const_iterator end() const {
    return object_list_.end();
  }

  /**
   * Returns true if the list is empty.
   * 
   * @return True if the list is empty.
   */
  virtual bool empty() const {
    return object_list_.empty();
  }

  /**
   * Clears the list
   */
  virtual void clear()
  {
    base_list::clear();
    erase(begin(), end());
  }

  /**
   * Returns the size of the list.
   * 
   * @return The size of the list.
   */
  virtual size_t size() const
  {
    return object_list_.size();
  }

  /**
   * @brief Inserts a new element.
   * 
   * Insert a new element at a given iterator position.
   * 
   * @param pos The position where to insert.
   * @param x The element to insert.
   * @return The new position iterator.
   */
  iterator insert(iterator pos, const value_type_wrapper &x)
  {
    return object_list_.insert(pos, x);
  };

  /**
   * @brief Interface to erase an element
   *
   * This is the interface for derived object_list
   * classes to implement thier erase element method.
   *
   * @param i The iterator to erase.
   * @return Returns the next iterator.
   */
  virtual iterator erase(iterator i) = 0;

  /**
   * @brief Erases a range defines by iterators
   *
   * Erase a range of elements from the list. The
   * range is defined by a first and a last iterator.
   * 
   * @param first The first iterator of the range to erase.
   * @param last The last iterator of the range to erase.
   * @return Returns the next iterator.
   */
  iterator erase(iterator first, iterator last)
  {
    while (first != last) {
      first = erase(first);
    }
    return first;
  }

protected:
  /**
   * @brief Executes the given function object for all elements.
   *
   * Executes the given function object for all elements.
   *
   * @param nf Function object used to be executed on each element.
   */
  virtual void for_each(const node_func &nf) const
  {
    const_iterator first = object_list_.begin();
    const_iterator last = object_list_.end();
    while (first != last) {
      nf((*first++).ptr());
    }
  }

  /**
   * @brief Sets the list reference into the child object.
   *
   * Sets the reference parent object into the given
   * element object to the one to many relationship.
   *
   * @param elem Element to set the parent reference object in.
   * @param o The parent list object to be set in the child element.
   * @return True if the value could be set.
   */
  bool set_reference(value_type *elem, const object_base_ptr &o)
  {
    object_linker ol(elem, o, list_name_);
    elem->read_from(&ol);
    return ol.success();
  }

  /**
   * Return the name of the list parameter.
   *
   * @return The name of the list parameter.
   */
  std::string list_name() const { return list_name_; }

  /**
   * Adds an element to the beginning of the list.
   *
   * @param x The element to be pushed front.
   */
  void push_front(const value_type_wrapper &x) { object_list_.push_front(x); }

  /**
   * Adds an element to the end of the list.
   *
   * @param x The element to be pushed back.
   */
  void push_back(const value_type_wrapper &x) { object_list_.push_back(x); }

  /**
   * Erases a single element from the list.
   *
   * @param i The element to remove.
   * @return Returns the element at the following position.
   */
  iterator erase_i(iterator i) { return object_list_.erase(i); }
  
  virtual void uninstall()
  {
    base_list::uninstall();
    object_list_.clear();
  }

private:
  virtual void reset()
  {
    object_list_.clear();
  }

  virtual void push_front_proxy(object_proxy *proxy)
  {
    object_list_.push_front(value_type_wrapper(proxy));
  }

  virtual void push_back_proxy(object_proxy *proxy)
  {
    object_list_.push_back(value_type_wrapper(proxy));
  }

private:
  list_type object_list_;
  std::string list_name_;
};

/**
 * @class object_ptr_list
 * @brief An object list class for object_ptr
 * @tparam T The concrete object type.
 * 
 * This is a specialisation of the object_list class.
 * It stores object_ptr in a list. The raw objects
 * are inserted into the list and than subsequently
 * inserted into the object_store. The reference link
 * to the parent class is done automatically.
 */
template < typename T >
class object_ptr_list : public object_list<T, object_ptr<T> >
{
public:
  typedef object_list<T, object_ptr<T> > base_list;              /**< Shortcut for the object_list class. */
  typedef typename base_list::value_type value_type;             /**< Shortcut for the value type. */
  typedef typename base_list::value_type_wrapper value_type_ptr; /**< Shortcut for the wrapper class around the value type. */
  typedef typename base_list::iterator iterator;                 /**< Shortcut for the list iterator. */
  typedef typename base_list::const_iterator const_iterator;     /**< Shortcut for the list const iterator. */

  /**
   * @brief Creates an empty list.
   * 
   * A new object_ptr_list is created. The list is part
   * of the given parent object and therefor a reference
   * to the parent object must be found inside the value
   * type object with the given list_ref_name.
   * 
   * @param parent The containing list object.
   * @param list_ref_name The name of the parent in the value type object.
   */
  object_ptr_list(object *parent, const std::string &list_ref_name)
    : base_list(parent, list_ref_name)
  {}
  
  virtual ~object_ptr_list() {}

  /**
   * @brief Push a new object to the front of the list.
   * 
   * An object not inserted into the object_store will
   * be pushed front to the list and inserted to the
   * object_store. Furthermore the reference link to the
   * list object is done automatilcally.
   *
   * @param elem The element to be pushed front
   * @param o The list containing object (parent)
   */
  virtual void push_front(T *elem, const object_base_ptr &o)
  {
    if (!base_list::ostore()) {
      //throw object_exception();
    } else {
      // mark list object as modified
      base_list::mark_modified(o.ptr());
      // set reference
      if (!set_reference(elem, o)) {
        // throw object_exception();
      } else {
        // insert new item object
        value_type_ptr optr = base_list::ostore()->insert(elem);
        // and call base list class push back
        base_list::push_front(optr);
      }
    }
  }

  /**
   * @brief Push a new object to the end of the list.
   * 
   * An object not inserted into the object_store will
   * be pushed back to the list and inserted to the
   * object_store. Furthermore the reference link to the
   * list object is done automatilcally.
   *
   * @param elem The element to be pushed back
   * @param o The list containing object (parent)
   */
  virtual void push_back(T* elem, const object_base_ptr &o)
  {
    if (!base_list::ostore()) {
      //throw object_exception();
    } else {
      // mark list object as modified
      base_list::mark_modified(o.ptr());
      // set reference
      if (!set_reference(elem, o)) {
        // throw object_exception();
      } else {
        // insert new item object
        value_type_ptr optr = base_list::ostore()->insert(elem);
        // and call base list class push back
        base_list::push_back(optr);
      }
    }
  }

  /**
   * @brief Erase the object at iterators position.
   * 
   * The object inside the iterator will first be
   * removed from the object_store and second be erased
   * from the list.
   * The next iterator position is returned.
   *
   * @param i The object to be erased containing iterator.
   * @return The next iterator position
   */
  virtual iterator erase(iterator i)
  {
    if (!base_list::ostore()) {
      // if list is not in ostore
      // throw exception
      //throw object_exception();
      return i;
    }
    // update predeccessor and successor
    value_type_ptr optr = *i;
    if (!base_list::ostore()->remove(optr)) {
      // throw exception
      return i;
    } else {
      // object was successfully deleted
      return base_list::erase_i(i);
    }
  }
};

/**
 * @class object_ref_list
 * @brief An object list class for object_ref
 * @tparam T The concrete object type.
 * 
 * This is a specialisation of the object_list class.
 * It stores object_ref in a list. The raw objects
 * are inserted into the list and than subsequently
 * inserted into the object_store. The reference link
 * to the parent class is done automatically.
 */
template < typename T >
class object_ref_list : public object_list<T, object_ref<T> >
{
public:
  typedef object_list<T, object_ref<T> > base_list;              /**< Shortcut for the object_list class. */
  typedef typename base_list::value_type value_type;             /**< Shortcut for the value type. */
  typedef typename base_list::value_type_wrapper value_type_ref; /**< Shortcut for the wrapper class around the value type. */
  typedef typename base_list::iterator iterator;                 /**< Shortcut for the list iterator. */
  typedef typename base_list::const_iterator const_iterator;     /**< Shortcut for the list const iterator. */

  /**
   * @brief Creates an empty list.
   * 
   * A new object_ref_list is created. The list is part
   * of the given parent object and therefor a reference
   * to the parent object must be found inside the value
   * type object with the given list_ref_name.
   * 
   * @param parent The containing list object.
   * @param list_ref_name The name of the parent in the value type object.
   */
  object_ref_list(object *parent, const std::string &list_ref_name)
    : base_list(parent, list_ref_name)
  {}

  virtual ~object_ref_list() {}

  /**
   * @brief Push an existing object reference to the front of the list.
   * 
   * An object reference is pushed front to the list.
   * Furthermore the reference link to the list object
   * is done automatilcally.
   *
   * @param elem The element to be pushed front
   * @param o The list containing object (parent)
   */
  virtual void push_front(const value_type_ref &elem, const object_base_ptr &o)
  {
    if (!base_list::ostore()) {
      //throw object_exception();
    } else {
      // set link to list object
      if (!set_reference(elem.get(), o)) {
        //throw object_exception();
      } else {
        base_list::push_front(elem);
      }
    }
  }

  /**
   * @brief Push an existing object reference to the end of the list.
   * 
   * An object reference is pushed back to the list.
   * Furthermore the reference link to the list object
   * is done automatilcally.
   *
   * @param elem The element to be pushed back
   * @param o The list containing object (parent)
   */
  virtual void push_back(const value_type_ref &elem, const object_base_ptr &o)
  {
    if (!base_list::ostore()) {
      //throw object_exception();
    } else {
      // set link to list object
      if (!set_reference(elem.get(), o)) {
        //throw object_exception();
      } else {
        base_list::push_back(elem);
      }
    }
  }

  /**
   * @brief Erase the object at iterators position.
   * 
   * The object inside the iterator will first be erased
   * from the list.
   * The next iterator position is returned.
   *
   * @param i The object to be erased containing iterator.
   * @return The next iterator position
   */
  iterator erase(iterator i)
  {
    /***************
     * 
     * object references can't be
     * deleted from object store
     * only delete them from lists
     * internal object refenence list
     * 
     ***************/
    return base_list::erase_i(i);
  }
};

/**
 * @class linked_object_list
 * @brief An linked object list class.
 * @tparam T The concrete object type.
 * 
 * The linked_object_list class stores object of
 * type T where T must be derived from object_list_node.
 * This base class contains the previous and next links as
 * well as the first and last element of the linked list.
 * Therefor the order of the elements is persistent and
 * reliable.
 */
template < class T >
class linked_object_list : public object_list_base
{
public:
//  typedef std::tr1::function<void (T*, &T::G)> set_ref_func_t;
//  typedef std::tr1::function<object_ref< (T*, &T::G)> get_ref_func_t;
  typedef object_list_base base_list;                   /**< Shortcut for the object_list class. */
	typedef T value_type;                                 /**< Shortcut for the value type. */
	typedef object_ptr<value_type> value_type_ptr;        /**< Shortcut for the value type pointer. */
  typedef object_list_iterator<T> iterator;             /**< Shortcut for the list iterator. */
  typedef const_object_list_iterator<T> const_iterator; /**< Shortcut for the list const iterator. */


  /**
   * @brief Creates an empty linked list.
   * 
   * A new linked_object_list is created. The list is part
   * of the given parent object and therefor a reference
   * to the parent object must be found inside the value
   * type object with the given list_ref_name.
   * 
   * @param parent The containing list object.
   * @param list_ref_name The name of the parent in the value type object.
   */
  linked_object_list(object *parent, const std::string &list_ref_name)
    : object_list_base(parent)
    , list_name_(list_ref_name)
  {}

	virtual ~linked_object_list() {}

  /*
	virtual void read_from(object_atomizer *) {}
	virtual void write_to(object_atomizer *) const {}
  */

  /**
   * Return the begin iterator of the list.
   * 
   * @return The begin iterator.
   */
  iterator begin() {
    return ++iterator(first_, this);
  }

  /**
   * Return the begin iterator of the list.
   * 
   * @return The begin iterator.
   */
  const_iterator begin() const {
    return ++const_iterator(first_, this);
  }

  /**
   * Return the end iterator of the list.
   * 
   * @return The end iterator.
   */
  iterator end() {
    return iterator(last_, this);
  }

  /**
   * Return the end iterator of the list.
   * 
   * @return The end iterator.
   */
  const_iterator end() const {
    return const_iterator(last_, this);
  }

  /**
   * Returns wether the list is empty or not.
   * 
   * @return True if list is empty.
   */
  virtual bool empty() const {
    return first_->next_ == last_->prev_;
  }

  /**
   * @brief Clears the list.
   * 
   * All elements are removed from the list
   * and also removed from the object_store.
   */
  virtual void clear()
  {
    base_list::clear();
    erase(begin(), end());
  }

  /**
   * Return the size of the list.
   * 
   * @return The size of the list.
   */
  virtual size_t size() const
  {
    return std::distance(begin(), end());
  }

  /**
   * @brief Push a new object to the front of the list.
   * 
   * An object not inserted into the object_store will
   * be pushed front to the list and inserted to the
   * object_store. Furthermore the reference link to the
   * list object and the links between the surrounding nodes
   * is done automatilcally.
   *
   * @param elem The element to be pushed front
   * @param o The list containing object (parent)
   */
  virtual void push_front(T *elem, const object_base_ptr &ref_list)
  {
    if (!ostore()) {
      //throw object_exception();
    } else {
      if (!set_reference(elem, ref_list)) {
        // throw object_exception()
      } else {
        value_type_ptr optr = ostore()->insert(elem);
        optr->next_ = first_->next_;
        first_->next_ = optr;
        optr->prev_ = first_;
        first_->next_ = optr;
        optr->first_ = first_;
        optr->last_ = last_;
      }
    }
  }

  /**
   * @brief Push a new object to the end of the list.
   * 
   * An object not inserted into the object_store will
   * be pushed back to the list and inserted to the
   * object_store. Furthermore the reference link to the
   * list object and the links between the surrounding nodes
   * is done automatilcally.
   *
   * @param elem The element to be pushed back
   * @param o The list containing object (parent)
   */
  virtual void push_back(T* elem, const object_base_ptr &ref_list)
  {
    if (!ostore()) {
      //throw object_exception();
    } else {
      if (!set_reference(elem, ref_list)) {
        // throw object_exception()
      } else {
        value_type_ptr optr = ostore()->insert(elem);
        optr->prev_ = last_->prev_;
        last_->prev_->next_ = optr;
        optr->next_ = last_;
        last_->prev_ = optr;
        optr->first_ = first_;
        optr->last_ = last_;
      }
    }
  }

  /**
   * @brief Erase the object at iterators position.
   * 
   * The object inside the iterator will first be
   * removed from the object_store and second be unlinked
   * and erased from the list.
   * The next iterator position is returned.
   *
   * @param i The object to be erased containing iterator.
   * @return The next iterator position
   */
  iterator erase(iterator i)
  {
    if (!ostore()) {
      // if list is not in ostore
      // throw exception
      //throw object_exception();
      return i;
    }
    // update predeccessor and successor
    value_type_ptr node = (i++).optr();
    node->prev()->next(node->next());
    node->next()->prev(node->prev());
    // delete node
    if (!ostore()->remove(node)) {
//      std::cout << "couldn't remove node (proxy: " << *node->proxy().get() << ")\n";
      node->prev()->next(node);
      node->next()->prev(node);
      // throw exception ?
      return ++iterator(node, this);
    }
    // return i's successor
    return i;
  }

  /**
   * @brief Erases a range defines by iterators
   *
   * Erase a range of elements from the list. The
   * range is defined by a first and a last iterator.
   * 
   * @param first The first iterator of the range to erase.
   * @param last The last iterator of the range to erase.
   * @return Returns the next iterator.
   */
  iterator erase(iterator first, iterator last)
  {
    while (first != last) {
      first = erase(first);
    }
    return first;
  }

protected:
  /**
   * @brief Executes the given function object for all elements.
   *
   * Executes the given function object for all elements.
   *
   * @param nf Function object used to be executed on each element.
   */
  virtual void for_each(const node_func &nf) const
  {
    value_type_ptr node = first_;
    while(node.get()) {
      nf(node.get());
      node = node->next();
    }
  }

  /**
   * @brief Sets the list reference into the child object.
   *
   * Sets the reference parent object into the given
   * element object to the one to many relationship.
   *
   * @param elem Element to set the parent reference object in.
   * @param o The parent list object to be set in the child element.
   * @return True if the value could be set.
   */
  virtual bool set_reference(value_type *elem, const object_base_ptr &o)
  {
    object_linker ol(elem, o, list_name_);
    elem->read_from(&ol);
    return ol.success();
  }
  
  /**
   * Return the name of the list parameter.
   *
   * @return The name of the list parameter.
   */
  std::string list_name() const
  {
    return list_name_;
  }

private:
  virtual void push_front_proxy(object_proxy *) {};
  virtual void push_back_proxy(object_proxy *) {};

  virtual void install(object_store *os)
  {
    object_list_base::install(os);
    
    // create first and last element
    first_ = ostore()->insert(new value_type(list_name_));
    last_ = ostore()->insert(new value_type(list_name_));
    // link object elements
    first_->first_ = first_;
    first_->last_ = last_;
    first_->next_ = last_;
    last_->first_ = first_;
    last_->last_ = last_;
    last_->prev_ = first_;
  }

  virtual void uninstall()
  {
    object_list_base::uninstall();
    first_.reset();
    last_.reset();
  }

  virtual void reset()
  {
    // link object elements
    first_->first_ = first_;
    first_->last_ = last_;
    first_->next_ = last_;
    last_->first_ = first_;
    last_->last_ = last_;
    last_->prev_ = first_;
  }

private:
  friend class object_store;
  friend class object_list_iterator<T>;
  friend class const_object_list_iterator<T>;

  std::string list_name_;

  value_type_ptr first_;
  value_type_ptr last_;
};

/**
 * @class linked_object_ptr_list
 * @brief An linked object list class for object_ptr.
 * @tparam T The concrete object type.
 * 
 * The linked_object_ptr_list class stores object of
 * type T. T is the only single element in teach node and
 * is stored in an object_ptr.
 * This base class contains the previous and next links as
 * well as the first and last element of the linked list.
 * Therefor the order of the elements is persistent and
 * reliable.
 */
template < class T >
class linked_object_ptr_list : public linked_object_list<object_ptr_list_node<T> >
{
public:
  typedef object_ptr_list_node<T> value_type;       /**< Shortcut for the value type */
  typedef object_ptr<value_type> value_type_ptr;    /**< Shortcut for the value type pointer */
  typedef linked_object_list<value_type> base_list; /**< Shortcut for the base list class */

  /**
   * @brief Creates an empty linked object ptr list.
   *
   * Creates an empty linked object ptr list. The list
   * is part of the given parent object and each element
   * in this has a reference to the parent within a parameter
   * of the given name.
   *
   * @param parent The list containing object.
   * @param list_ref_name The name of the reference list object in each node.
   */
  linked_object_ptr_list(object *parent, const std::string &list_ref_name)
    : linked_object_list<value_type>(parent, list_ref_name)
  {}

  virtual ~linked_object_ptr_list() {}

  /**
   * @brief Insert an element at the beginning of the list.
   *
   * Insert an element at the beginning of the list. The
   * reference link the object containing list is done
   * automatically.
   *
   * @param optr The pointer object to be pushed front.
   * @param ref_list The reference list object.
   */
  void push_front(const object_ptr<T> &optr, const object_base_ptr &ref_list)
  {
    base_list::push_front(new value_type(optr, base_list::list_name()), ref_list);
  }

  /**
   * @brief Insert an element at the end of the list.
   *
   * Insert an element at the end of the list. The
   * reference link to the object containing list is done
   * automatically.
   *
   * @param optr The pointer object to be pushed back.
   * @param ref_list The reference list object.
   */
  void push_back(const object_ptr<T> &optr, const object_base_ptr &ref_list)
  {
    base_list::push_front(new value_type(optr, base_list::list_name()), ref_list);
  }
  
protected:
  /**
   * @brief Sets the list reference into the child object.
   *
   * Sets the reference parent object into the given
   * element object to the one to many relationship.
   *
   * @param elem Element to set the parent reference object in.
   * @param o The parent list object to be set in the child element.
   * @return True if the value could be set.
   */
  virtual bool set_reference(value_type *elem, const object_base_ptr &o)
  {
    object_ptr<T> optr = elem->optr();
    object_linker ol(optr, o, base_list::list_name());
    optr->read_from(&ol);
    return ol.success();
  }
};

/**
 * @class linked_object_ref_list
 * @brief An linked object list class for object_ref.
 * @tparam T The concrete object type.
 * 
 * The linked_object_ref_list class stores object of
 * type T. T is the only single element in teach node and
 * is stored in an object_ref.
 * This base class contains the previous and next links as
 * well as the first and last element of the linked list.
 * Therefor the order of the elements is persistent and
 * reliable.
 */
template < class T >
class linked_object_ref_list : public linked_object_list<object_ref_list_node<T> >
{
public:
  typedef object_ref_list_node<T> value_type;       /**< Shortcut for the value type */
  typedef object_ptr<value_type> value_type_ptr;    /**< Shortcut for the value type pointer */
  typedef linked_object_list<value_type> base_list; /**< Shortcut for the base list class */

  /**
   * @brief Creates an empty linked object ref list.
   *
   * Creates an empty linked object ref list. The list
   * is part of the given parent object and each element
   * in this has a reference to the parent within a parameter
   * of the given name.
   *
   * @param parent The list containing object.
   * @param list_ref_name The name of the reference list object in each node.
   */
  linked_object_ref_list(object *parent, const std::string &list_ref_name)
    : linked_object_list<value_type>(parent, list_ref_name)
  {}

  virtual ~linked_object_ref_list() {}

  /**
   * @brief Insert an element at the beginning of the list.
   *
   * Insert an element at the beginning of the list. The
   * reference link to the object containing list is done
   * automatically.
   *
   * @param oref The reference object to be pushed front.
   * @param ref_list The reference list object.
   */
  void push_front(const object_ref<T> &oref, const object_base_ptr &ref_list)
  {
    base_list::push_front(new value_type(oref, base_list::list_name()), ref_list);
  }

  /**
   * @brief Insert an element at the end of the list.
   *
   * Insert an element at the end of the list. The
   * reference link to the object containing list is done
   * automatically.
   *
   * @param oref The reference object to be pushed back.
   * @param ref_list The reference list object.
   */
  void push_back(const object_ref<T> &oref, const object_base_ptr &ref_list)
  {
    base_list::push_back(new value_type(oref, base_list::list_name()), ref_list);
  }
  
protected:
  /**
   * @brief Sets the list reference into the child object.
   *
   * Sets the reference parent object into the given
   * element object to the one to many relationship.
   *
   * @param elem Element to set the parent reference object in.
   * @param o The parent list object to be set in the child element.
   * @return True if the value could be set.
   */
  virtual bool set_reference(value_type *elem, const object_base_ptr &o)
  {
    object *oref = elem->oref().ptr();
    object_linker ol(oref, o, base_list::list_name());
    oref->read_from(&ol);
    return ol.success();
  }
};

}

#endif /* OBJECT_LIST_HPP */
