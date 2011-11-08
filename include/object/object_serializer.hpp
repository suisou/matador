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

#ifndef OBJECT_SERIALIZER_HPP
#define OBJECT_SERIALIZER_HPP

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

#include "object/object_atomizer.hpp"
#include "tools/byte_buffer.hpp"

namespace oos {

class object;

class OOS_API object_serializer : public object_atomizer
{
public:
  object_serializer();
  virtual ~object_serializer();

  bool serialize(const object *o, byte_buffer &buffer);
  bool deserialize(object *o, byte_buffer &buffer);

	virtual void write_char(const char* id, char c);
	virtual void write_float(const char* id, float f);
	virtual void write_double(const char* id, double f);
	virtual void write_int(const char* id, int i);
	virtual void write_long(const char* id, long l);
	virtual void write_unsigned(const char* id, unsigned u);
	virtual void write_bool(const char* id, bool b);
	virtual void write_charptr(const char* id, const char *c);
	virtual void write_string(const char* id, const std::string &s);
	virtual void write_date(const char* id, const Date &s);
	virtual void write_time(const char* id, const Time &s);
	virtual void write_object(const char* id, const base_object_ptr &x);
	virtual void write_object_list(const char* id, const object_list_base &x);

	virtual void read_char(const char* id, char &c);
	virtual void read_float(const char* id, float &f);
	virtual void read_double(const char* id, double &f);
	virtual void read_int(const char* id, int &i);
	virtual void read_long(const char* id, long &l);
	virtual void read_unsigned(const char* id, unsigned &u);
	virtual void read_bool(const char* id, bool &b);
	virtual void read_charptr(const char* id, char *&c);
	virtual void read_string(const char* id, std::string &s);
	virtual void read_date(const char* id, Date &s);
	virtual void read_time(const char* id, Time &s);
	virtual void read_object(const char* id, base_object_ptr &x);
	virtual void read_object_list(const char* id, object_list_base &x);
  
private:
  byte_buffer *buffer_;
};

}

#endif /* OBJECT_SERIALIZER_HPP */
