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

#ifndef OBJECT_PTR_HPP
#define OBJECT_PTR_HPP

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define OOS_API
#endif

#include "object/object_proxy.hpp"

#include <memory>
#include <iostream>
#include <typeinfo>

namespace oos {

class object;

/**
 * @class object_base_ptr
 * @brief Base class for the object pointer and reference class
 * 
 * This is the base class for the object pointer
 * and reference class. The class holds the proxy
 * of the object and the id of the object.
 */
class OOS_API object_base_ptr {
protected:
  /**
   * @brief Creates and empty base pointer.
   * 
   * Creates and empty base pointer. The boolean
   * tells the class if the object is handled
   * as a reference or an pointer. The difference
   * is that the reference couldn't be deleted
   * from the object_store and the pointer can.
   * 
   * @param is_ref If true the object is handled as a reference.
   */
	explicit object_base_ptr(bool is_ref);
  
  /**
   * Copies from another object_base_ptr
   * 
   * @param x the object_base_ptr to copy from.
   */
	object_base_ptr(const object_base_ptr &x);

  /**
   * Assign operator.
   * 
   * @param x The object_base_ptr to assign from.
   */
	object_base_ptr& operator=(const object_base_ptr &x);

  /**
   * @brief Creates an object_base_ptr with a given object
   * 
   * Creates an object_base_ptr with a given object. The
   * boolean tells the object_base_ptr if it should be
   * handled as a reference or a pointer.
   * 
   * @param o The object of the object_base_ptr
   * @param is_ref If true the object is handled as a reference.
   */
	object_base_ptr(object* o, bool is_ref);
  
  /**
   * @brief Creates an object_base_ptr with a given object_proxy
   * 
   * Creates an object_base_ptr with a given object_proxy. The
   * boolean tells the object_base_ptr if it should be
   * handled as a reference or a pointer.
   * 
   * @param op The object_proxy of the object_base_ptr
   * @param is_ref If true the object is handled as a reference.
   */
	object_base_ptr(object_proxy *op, bool is_ref);

	virtual ~object_base_ptr();

public:

  /**
   * Equal to operator for the object_base_ptr
   * 
   * @param x The object_base_ptr to check equality with.
   */
	bool operator==(const object_base_ptr &x) const;

  /**
   * Not equal to operator for the object_base_ptr
   * 
   * @param x The object_base_ptr to check unequality with.
   */
	bool operator!=(const object_base_ptr &x) const;

  /**
   * Returns the type string of the object
   * 
   * @return The type string of the object.
   */
  virtual const char* type() const = 0;

  /**
   * Resets the object_base_ptr with the given object.
   * 
   * @param o The new object for the object_base_ptr.
   */
	void reset(const object *o = 0);

  /**
   * Returns if the object is loaded.
   * 
   * @return True if the object is loaded.
   */
	bool is_loaded() const;

  /**
   * Returns the object id.
   * 
   * @return The id of the object.
   */
	long id() const;

  /**
   * Returns the object
   * 
   * @return The object.
   */
	object* ptr() const;

  /**
   * Returns the object
   * 
   * @return The object.
   */
	object* lookup_object() const;

  /**
   * Returns if the object is treated as a reference.
   * 
   * @return True if the object is treated like a reference.
   */
  virtual bool is_reference() const;

  /**
   * Returns if this object_base_ptr is inside
   * of the object_store. This is important
   * to calculate the reference and pointer
   * counter.
   * 
   * @return True if the object_base_ptr internal
   */
  bool is_internal() const;

  /**
   * Returns the reference count.
   * 
   * @return The reference count.
   */
  unsigned long ref_count() const;

  /**
   * Returns the pointer count.
   * 
   * @return The pointer count.
   */
  unsigned long ptr_count() const;

private:
	friend class object_atomizer;
  friend class object_creator;
  friend class object_serializer;
  friend struct object_proxy;

  template < class T > friend class object_ref;
  template < class T > friend class object_ptr;

	long id_;
  object_proxy *proxy_;
  bool is_reference_;
  bool is_internal_;
};

/*
template < class T, bool IR >
class object_wrapper : public object_base_ptr
{
public:
	object_wrapper() : object_base_ptr(false) {}
	template < class Y >
	object_wrapper(const object_wrapper<Y, true> &x) {}
	template < class Y >
	object_wrapper(const object_wrapper<Y, false> &x) {}
	template < class Y >
	object_wrapper& operator=(const object_wrapper<Y, true> &x) { return *this; }
	template < class Y >
	object_wrapper& operator=(const object_wrapper<Y, false> &x) { return *this; }
	explicit object_wrapper(object* o) : object_base_ptr(o, false) {}

  virtual const char* type() const
  {
    return typeid(T).name();
  }

	T* operator->() const {
	  if (proxy_) {
	    return dynamic_cast<T*>(lookup_object());
	  }
	  return NULL;
	}
	T& operator*() const {
		if (proxy_) {
      return *dynamic_cast<T*>(lookup_object());
 		}
    return NULL;
	}
	T* get() const {
		if (proxy_) {
      return dynamic_cast<T*>(lookup_object());
 		}
    return NULL;
	}
  virtual bool is_reference() const
  {
    return IR;
  }
};
*/

template < class T >
class object_ref;

/**
 * @class object_ptr
 * @brief The object_ptr holds a pointer to an object.
 * @tparam T The type of the object.
 * 
 * The object_ptr holds a pointer to an object. The
 * object_ptr is a wrapper class for the object class
 * It has a reference count mechanism.
 * The objects inserted into the object_store are returned
 * as a object_ptr and should be used through the
 * object_ptr class.
 */
template < class T >
class object_ptr : public object_base_ptr
{
public:
  /**
   * Create an empty object_ptr
   */
	object_ptr() : object_base_ptr(false) {}
  
  /**
   * Copies object_ptr
   * 
   * @tparam Y The type of the object_ptr to copy
   * @param x The object_ptr to copy
   */
	template < class Y >
	object_ptr(const object_ptr<Y> &x) {}

  /**
   * Copies a object_ref
   * 
   * @tparam Y The type of the object_ref to copy
   * @param x The object_ref to copy
   */
	template < class Y >
  object_ptr(const object_ref<Y> &x) : object_base_ptr(x.proxy_, false) {}

  /**
   * Assign an object_ptr
   * 
   * @tparam Y The type of the object_ptr to assign.
   * @param x The object_ptr to assign from
   * @return The assign object_ptr
   */
	template < class Y >
	object_ptr& operator=(const object_ptr<Y> &x) { return *this; }

  /**
   * Create an object_ptr from an object
   * 
   * @param o The object.
   */
	explicit object_ptr(object* o) : object_base_ptr(o, false) {}

  /**
   * Create an object_ptr from an object_proxy
   * 
   * @param proxy The object_proxy.
   */
  explicit object_ptr(object_proxy *proxy) : object_base_ptr(proxy, false) {}

  /**
   * Return the type string of the object
   * 
   * @return The type string of the object.
   */
  virtual const char* type() const
  {
    return typeid(T).name();
  }

  
	T* operator->() const {
	  if (proxy_) {
	    return dynamic_cast<T*>(lookup_object());
	  }
	  return NULL;
	}
	T& operator*() const {
		if (proxy_) {
      return *dynamic_cast<T*>(lookup_object());
    } else {
      return *(T*)0;
    }
	}
	T* get() const {
    if (proxy_) {
      T* t = dynamic_cast<T*>(lookup_object());
      return t;
    }
    return NULL;
  }
};

template < class T >
class object_ref : public object_base_ptr
{
public:
	object_ref() : object_base_ptr(true) {}
	//  object_ptr(const object_ptr &x) {}
	template < class Y >
	object_ref(const object_ref<Y> &x) {}
	template < class Y >
  object_ref(const object_ptr<Y> &x) : object_base_ptr(x.proxy_, true) {}
	//  object_ptr& operator=(const object_ptr &x) { return *this; }
	template < class Y >
	object_ref& operator=(const object_ref<Y> &x) { return *this; }
	explicit object_ref(object* o) : object_base_ptr(o, true) {}
  explicit object_ref(object_proxy *proxy) : object_base_ptr(proxy, true) {}

  virtual const char* type() const
  {
    return typeid(T).name();
  }

	T* operator->() const {
	  if (proxy_) {
	    return dynamic_cast<T*>(lookup_object());
	  }
	  return NULL;
	}
	T& operator*() const {
		if (proxy_) {
      return *dynamic_cast<T*>(lookup_object());
 		}
    return NULL;
	}
	T* get() const {
		if (proxy_) {
      return dynamic_cast<T*>(lookup_object());
 		}
    return NULL;
	}
};

}

#endif /* OBJECT_PTR_HPP */
