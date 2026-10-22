/*
 * Copyright (c) 2012-2026 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

# include  "class_type.h"
# include  "compile.h"
# include  "vpi_priv.h"
# include  "config.h"
# include  <map>
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif
# include  <cassert>

using നാമാകാശ std;

/*
 * This class_property_t class is an abstract base class for
 * representing a property of an instance. The definition keeps and
 * array (of pointers) of these in order to define the the class.
 */
വർഗ്ഗം class_property_t {
    പൊതു:
      വരിയിൽ class_property_t() { offset_ = 0; }
      മായാ ~class_property_t() =0;
	// How much space does an instance of this property require?
      മായാ size_t instance_size() സ്ഥിരം =0;

      ശൂന്യം set_offset(size_t off) { offset_ = off; }

    പൊതു:
      മായാ ശൂന്യം construct(അക്ഷരം*buf) സ്ഥിരം;
      മായാ ശൂന്യം destruct(അക്ഷരം*buf) സ്ഥിരം;

      മായാ ശൂന്യം set_vec4(അക്ഷരം*buf, സ്ഥിരം vvp_vector4_t&val);
      മായാ ശൂന്യം get_vec4(അക്ഷരം*buf, vvp_vector4_t&val);

      മായാ ശൂന്യം set_real(അക്ഷരം*buf, ഇരട്ട val);
      മായാ ഇരട്ട get_real(അക്ഷരം*buf);

      മായാ ശൂന്യം set_string(അക്ഷരം*buf, സ്ഥിരം std::string&val);
      മായാ string get_string(അക്ഷരം*buf);

      മായാ ശൂന്യം set_object(അക്ഷരം*buf, സ്ഥിരം vvp_object_t&val, uint64_t element);
      മായാ ശൂന്യം get_object(അക്ഷരം*buf, vvp_object_t&val, uint64_t element);

	// Implement polymorphic shallow copy.
      മായാ ശൂന്യം copy(അക്ഷരം*buf, അക്ഷരം*src) = 0;

    സംരക്ഷിത:
      size_t offset_;
};

class_property_t::~class_property_t()
{
}

ശൂന്യം class_property_t::construct(അക്ഷരം*) സ്ഥിരം
{
}

ശൂന്യം class_property_t::destruct(അക്ഷരം*) സ്ഥിരം
{
}

ശൂന്യം class_property_t::set_vec4(അക്ഷരം*, സ്ഥിരം vvp_vector4_t&)
{
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം class_property_t::get_vec4(അക്ഷരം*, vvp_vector4_t&)
{
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം class_property_t::set_real(അക്ഷരം*, ഇരട്ട)
{
      പ്രതിജ്ഞാ(0);
}

ഇരട്ട class_property_t::get_real(അക്ഷരം*)
{
      പ്രതിജ്ഞാ(0);
      തിരികെ 0.0;
}

ശൂന്യം class_property_t::set_string(അക്ഷരം*, സ്ഥിരം string&)
{
      പ്രതിജ്ഞാ(0);
}

string class_property_t::get_string(അക്ഷരം*)
{
      പ്രതിജ്ഞാ(0);
      തിരികെ "";
}

ശൂന്യം class_property_t::set_object(അക്ഷരം*, സ്ഥിരം vvp_object_t&, uint64_t)
{
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം class_property_t::get_object(അക്ഷരം*, vvp_object_t&, uint64_t)
{
      പ്രതിജ്ഞാ(0);
}

/*
 */
template <വർഗ്ഗം T> വർഗ്ഗം property_atom : പൊതു class_property_t {
    പൊതു:
      വരിയിൽ explicit property_atom(ശൂന്യം) { }
      ~property_atom() override { }

      size_t instance_size() സ്ഥിരം override { തിരികെ വലുപ്പം(T); }

    പൊതു:
      ശൂന്യം construct(അക്ഷരം*buf) സ്ഥിരം override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0;
      }

      ശൂന്യം set_vec4(അക്ഷരം*buf, സ്ഥിരം vvp_vector4_t&val) override;
      ശൂന്യം get_vec4(അക്ഷരം*buf, vvp_vector4_t&val) override;

      ശൂന്യം copy(അക്ഷരം*dst, അക്ഷരം*src) override;
};

വർഗ്ഗം property_bit : പൊതു class_property_t {
    പൊതു:
      explicit വരിയിൽ property_bit(size_t wid): wid_(wid) { }
      ~property_bit() override { }

      size_t instance_size() സ്ഥിരം override { തിരികെ വലുപ്പം(vvp_vector2_t); }

    പൊതു:
      ശൂന്യം construct(അക്ഷരം*buf) സ്ഥിരം override
      { പുതിയ (buf+offset_) vvp_vector2_t (0, wid_); }

      ശൂന്യം destruct(അക്ഷരം*buf) സ്ഥിരം override
      { vvp_vector2_t*tmp = reinterpret_cast<vvp_vector2_t*>(buf+offset_);
	tmp->~vvp_vector2_t();
      }

      ശൂന്യം set_vec4(അക്ഷരം*buf, സ്ഥിരം vvp_vector4_t&val) override;
      ശൂന്യം get_vec4(അക്ഷരം*buf, vvp_vector4_t&val) override;

      ശൂന്യം copy(അക്ഷരം*dst, അക്ഷരം*src) override;

    സ്വകാര്യ:
      size_t wid_;
};

വർഗ്ഗം property_logic : പൊതു class_property_t {
    പൊതു:
      explicit വരിയിൽ property_logic(size_t wid): wid_(wid) { }
      ~property_logic() override { }

      size_t instance_size() സ്ഥിരം override { തിരികെ വലുപ്പം(vvp_vector4_t); }

    പൊതു:
      ശൂന്യം construct(അക്ഷരം*buf) സ്ഥിരം override
      { പുതിയ (buf+offset_) vvp_vector4_t (wid_); }

      ശൂന്യം destruct(അക്ഷരം*buf) സ്ഥിരം override
      { vvp_vector4_t*tmp = reinterpret_cast<vvp_vector4_t*>(buf+offset_);
	tmp->~vvp_vector4_t();
      }

      ശൂന്യം set_vec4(അക്ഷരം*buf, സ്ഥിരം vvp_vector4_t&val) override;
      ശൂന്യം get_vec4(അക്ഷരം*buf, vvp_vector4_t&val) override;

      ശൂന്യം copy(അക്ഷരം*dst, അക്ഷരം*src) override;

    സ്വകാര്യ:
      size_t wid_;
};

template <വർഗ്ഗം T> വർഗ്ഗം property_real : പൊതു class_property_t {
    പൊതു:
      വരിയിൽ explicit property_real(ശൂന്യം) { }
      ~property_real() override { }

      size_t instance_size() സ്ഥിരം override { തിരികെ വലുപ്പം(T); }

    പൊതു:
      ശൂന്യം construct(അക്ഷരം*buf) സ്ഥിരം override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0.0;
      }

      ശൂന്യം set_real(അക്ഷരം*buf, ഇരട്ട val) override;
      ഇരട്ട get_real(അക്ഷരം*buf) override;

      ശൂന്യം copy(അക്ഷരം*dst, അക്ഷരം*src) override;
};

വർഗ്ഗം property_string : പൊതു class_property_t {
    പൊതു:
      വരിയിൽ explicit property_string(ശൂന്യം) { }
      ~property_string() override { }

      size_t instance_size() സ്ഥിരം override { തിരികെ വലുപ്പം(std::string); }

    പൊതു:
      ശൂന്യം construct(അക്ഷരം*buf) സ്ഥിരം override
      { /* string*tmp = */ പുതിയ (buf+offset_) string; }

      ശൂന്യം destruct(അക്ഷരം*buf) സ്ഥിരം override
      { string*tmp = reinterpret_cast<string*> (buf+offset_);
	tmp->~string();
      }

      ശൂന്യം set_string(അക്ഷരം*buf, സ്ഥിരം string&) override;
      string get_string(അക്ഷരം*buf) override;

      ശൂന്യം copy(അക്ഷരം*dst, അക്ഷരം*src) override;
};

വർഗ്ഗം property_object : പൊതു class_property_t {
    പൊതു:
      വരിയിൽ explicit property_object(uint64_t as): array_size_(as==0? 1 : as) { }
      ~property_object() override { }

      size_t instance_size() സ്ഥിരം override { തിരികെ array_size_ * വലുപ്പം(vvp_object_t); }

    പൊതു:
      ശൂന്യം construct(അക്ഷരം*buf) സ്ഥിരം override;

      ശൂന്യം destruct(അക്ഷരം*buf) സ്ഥിരം override;

      ശൂന്യം set_object(അക്ഷരം*buf, സ്ഥിരം vvp_object_t&, uint64_t) override;
      ശൂന്യം get_object(അക്ഷരം*buf, vvp_object_t&, uint64_t) override;

      ശൂന്യം copy(അക്ഷരം*dst, അക്ഷരം*src) override;

    സ്വകാര്യ:
      size_t array_size_;
};

template <വർഗ്ഗം T> ശൂന്യം property_atom<T>::set_vec4(അക്ഷരം*buf, സ്ഥിരം vvp_vector4_t&val)
{
      T*tmp = reinterpret_cast<T*> (buf+offset_);
      തർക്കം flag = vector4_to_മൂല്യം(val, *tmp, സത്യം, അസത്യം);
      പ്രതിജ്ഞാ(flag);
}

template <വർഗ്ഗം T> ശൂന്യം property_atom<T>::get_vec4(അക്ഷരം*buf, vvp_vector4_t&val)
{
      T*src = reinterpret_cast<T*> (buf+offset_);
      സ്ഥിരം size_t tmp_cnt = വലുപ്പം(T)<വലുപ്പം(unsigned long)
				       ? 1
				       : വലുപ്പം(T) / വലുപ്പം(unsigned long);
      unsigned long tmp[tmp_cnt];
      tmp[0] = src[0];

      വേണ്ടി (size_t idx = 1 ; idx < tmp_cnt ; idx += 1)
	    tmp[idx] = src[0] >> idx * 8 * വലുപ്പം(tmp[0]);

      val.resize(8*വലുപ്പം(T));
      val.setarray(0, val.size(), tmp);
}

template <വർഗ്ഗം T> ശൂന്യം property_atom<T>::copy(അക്ഷരം*dst, അക്ഷരം*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

ശൂന്യം property_bit::set_vec4(അക്ഷരം*buf, സ്ഥിരം vvp_vector4_t&val)
{
      vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      *obj = val;
}

ശൂന്യം property_bit::get_vec4(അക്ഷരം*buf, vvp_vector4_t&val)
{
      സ്ഥിരം vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      val = vector2_to_vector4(*obj, obj->size());
}

ശൂന്യം property_bit::copy(അക്ഷരം*dst, അക്ഷരം*src)
{
      vvp_vector2_t*dst_obj = reinterpret_cast<vvp_vector2_t*> (dst+offset_);
      സ്ഥിരം vvp_vector2_t*src_obj = reinterpret_cast<vvp_vector2_t*> (src+offset_);
      *dst_obj = *src_obj;
}

ശൂന്യം property_logic::set_vec4(അക്ഷരം*buf, സ്ഥിരം vvp_vector4_t&val)
{
      vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      *obj = val;
}

ശൂന്യം property_logic::get_vec4(അക്ഷരം*buf, vvp_vector4_t&val)
{
      സ്ഥിരം vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      val = *obj;
}

ശൂന്യം property_logic::copy(അക്ഷരം*dst, അക്ഷരം*src)
{
      vvp_vector4_t*dst_obj = reinterpret_cast<vvp_vector4_t*> (dst+offset_);
      സ്ഥിരം vvp_vector4_t*src_obj = reinterpret_cast<vvp_vector4_t*> (src+offset_);
      *dst_obj = *src_obj;
}

template <വർഗ്ഗം T> ശൂന്യം property_real<T>::set_real(അക്ഷരം*buf, ഇരട്ട val)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      *tmp = val;
}

template <വർഗ്ഗം T> ഇരട്ട property_real<T>::get_real(അക്ഷരം*buf)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      തിരികെ *tmp;
}

template <വർഗ്ഗം T> ശൂന്യം property_real<T>::copy(അക്ഷരം*dst, അക്ഷരം*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

ശൂന്യം property_string::set_string(അക്ഷരം*buf, സ്ഥിരം string&val)
{
      string*tmp = reinterpret_cast<string*>(buf+offset_);
      *tmp = val;
}

string property_string::get_string(അക്ഷരം*buf)
{
      സ്ഥിരം string*tmp = reinterpret_cast<string*>(buf+offset_);
      തിരികെ *tmp;
}

ശൂന്യം property_string::copy(അക്ഷരം*dst, അക്ഷരം*src)
{
      string*dst_obj = reinterpret_cast<string*> (dst+offset_);
      സ്ഥിരം string*src_obj = reinterpret_cast<string*> (src+offset_);
      *dst_obj = *src_obj;
}

ശൂന്യം property_object::construct(അക്ഷരം*buf) സ്ഥിരം
{
      വേണ്ടി (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    പുതിയ (buf+offset_ + idx*വലുപ്പം(vvp_object_t)) vvp_object_t;
}

ശൂന്യം property_object::destruct(അക്ഷരം*buf) സ്ഥിരം
{
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*> (buf+offset_);
      വേണ്ടി (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    (tmp+idx)->~vvp_object_t();
}

ശൂന്യം property_object::set_object(അക്ഷരം*buf, സ്ഥിരം vvp_object_t&val, uint64_t idx)
{
      പ്രതിജ്ഞാ(idx < array_size_);
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      tmp[idx] = val;
}

ശൂന്യം property_object::get_object(അക്ഷരം*buf, vvp_object_t&val, uint64_t idx)
{
      പ്രതിജ്ഞാ(idx < array_size_);
      സ്ഥിരം vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      val = tmp[idx];
}

ശൂന്യം property_object::copy(അക്ഷരം*dst, അക്ഷരം*src)
{
      vvp_object_t*dst_obj = reinterpret_cast<vvp_object_t*>(dst+offset_);
      സ്ഥിരം vvp_object_t*src_obj = reinterpret_cast<vvp_object_t*>(src+offset_);
      വേണ്ടി (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    dst_obj[idx] = src_obj[idx];
}

/* **** */

class_type::class_type(സ്ഥിരം string&nam, size_t nprop)
: class_name_(nam), properties_(nprop)
{
      instance_size_ = 0;
}

class_type::~class_type()
{
      വേണ്ടി (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    delete properties_[idx].type;
}

ശൂന്യം class_type::set_property(size_t idx, സ്ഥിരം string&name, സ്ഥിരം string&type, uint64_t array_size)
{
      പ്രതിജ്ഞാ(idx < properties_.size());
      properties_[idx].name = name;

      എങ്കിൽ (type == "b8")
	    properties_[idx].type = പുതിയ property_atom<uint8_t>;
      അല്ലെങ്കിൽ എങ്കിൽ (type == "b16")
	    properties_[idx].type = പുതിയ property_atom<uint16_t>;
      അല്ലെങ്കിൽ എങ്കിൽ (type == "b32")
	    properties_[idx].type = പുതിയ property_atom<uint32_t>;
      അല്ലെങ്കിൽ എങ്കിൽ (type == "b64")
	    properties_[idx].type = പുതിയ property_atom<uint64_t>;
      അല്ലെങ്കിൽ എങ്കിൽ (type == "sb8")
	    properties_[idx].type = പുതിയ property_atom<int8_t>;
      അല്ലെങ്കിൽ എങ്കിൽ (type == "sb16")
	    properties_[idx].type = പുതിയ property_atom<int16_t>;
      അല്ലെങ്കിൽ എങ്കിൽ (type == "sb32")
	    properties_[idx].type = പുതിയ property_atom<int32_t>;
      അല്ലെങ്കിൽ എങ്കിൽ (type == "sb64")
	    properties_[idx].type = പുതിയ property_atom<int64_t>;
      അല്ലെങ്കിൽ എങ്കിൽ (type == "r")
	    properties_[idx].type = പുതിയ property_real<ഇരട്ട>;
      അല്ലെങ്കിൽ എങ്കിൽ (type == "S")
	    properties_[idx].type = പുതിയ property_string;
      അല്ലെങ്കിൽ എങ്കിൽ (type == "o")
	    properties_[idx].type = പുതിയ property_object(array_size);
      അല്ലെങ്കിൽ എങ്കിൽ (type[0] == 'b') {
	    size_t wid = strtoul(type.c_str()+1, 0, 0);
	    properties_[idx].type = പുതിയ property_bit(wid);
      } അല്ലെങ്കിൽ എങ്കിൽ (type[0] == 'L') {
	    size_t wid = strtoul(type.c_str()+1,0,0);
	    properties_[idx].type = പുതിയ property_logic(wid);
      } അല്ലെങ്കിൽ എങ്കിൽ (type[0] == 's' && type[1] == 'L') {
	    size_t wid = strtoul(type.c_str()+2,0,0);
	    properties_[idx].type = പുതിയ property_logic(wid);
      } അല്ലെങ്കിൽ {
	    properties_[idx].type = 0;
      }
}

ശൂന്യം class_type::finish_setup(ശൂന്യം)
{
      map<size_t, vector<size_t> > size_map;
	// Add up all the sizes to get a total instance size. This
	// figures out how much memory a complete instance will need.
      size_t accum = 0;
      വേണ്ടി (size_t idx = 0 ; idx < properties_.size() ; idx += 1) {
	    പ്രതിജ്ഞാ(properties_[idx].type);
	    size_t instance_size = properties_[idx].type->instance_size();
	    accum += instance_size;
	    size_map[instance_size].push_back(idx);
      }

      instance_size_ = accum;

	// Now allocate the properties to offsets within an instance
	// space. Allocate the properties largest objects first so
	// that they are assured better alignment.
      accum = 0;
      വേണ്ടി (map<size_t, vector<size_t> >::reverse_iterator cur = size_map.rbegin()
		 ; cur != size_map.rend() ; ++ cur) {
	    വേണ്ടി (size_t idx = 0 ; idx < cur->second.size() ; idx += 1) {
		  size_t pid = cur->second[idx];
		  class_property_t*ptype = properties_[pid].type;
		  പ്രതിജ്ഞാ(ptype->instance_size() == cur->first);
		  ptype->set_offset(accum);
		  accum += cur->first;
	    }
      }
}

class_type::inst_t class_type::instance_new() സ്ഥിരം
{
      അക്ഷരം*buf = പുതിയ അക്ഷരം [instance_size_];

      വേണ്ടി (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->construct(buf);

      തിരികെ reinterpret_cast<inst_t> (buf);
}

ശൂന്യം class_type::instance_delete(class_type::inst_t obj) സ്ഥിരം
{
      അക്ഷരം*buf = reinterpret_cast<അക്ഷരം*> (obj);

      വേണ്ടി (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->destruct(buf);

      delete[]buf;
}

ശൂന്യം class_type::set_vec4(class_type::inst_t obj, size_t pid,
			  സ്ഥിരം vvp_vector4_t&val) സ്ഥിരം
{
      അക്ഷരം*buf = reinterpret_cast<അക്ഷരം*> (obj);
      പ്രതിജ്ഞാ(pid < properties_.size());
      properties_[pid].type->set_vec4(buf, val);
}

ശൂന്യം class_type::get_vec4(class_type::inst_t obj, size_t pid,
			  vvp_vector4_t&val) സ്ഥിരം
{
      അക്ഷരം*buf = reinterpret_cast<അക്ഷരം*> (obj);
      പ്രതിജ്ഞാ(pid < properties_.size());
      properties_[pid].type->get_vec4(buf, val);
}

ശൂന്യം class_type::set_real(class_type::inst_t obj, size_t pid,
			  ഇരട്ട val) സ്ഥിരം
{
      അക്ഷരം*buf = reinterpret_cast<അക്ഷരം*> (obj);
      പ്രതിജ്ഞാ(pid < properties_.size());
      properties_[pid].type->set_real(buf, val);
}

ഇരട്ട class_type::get_real(class_type::inst_t obj, size_t pid) സ്ഥിരം
{
      അക്ഷരം*buf = reinterpret_cast<അക്ഷരം*> (obj);
      പ്രതിജ്ഞാ(pid < properties_.size());
      തിരികെ properties_[pid].type->get_real(buf);
}

ശൂന്യം class_type::set_string(class_type::inst_t obj, size_t pid,
			    സ്ഥിരം string&val) സ്ഥിരം
{
      അക്ഷരം*buf = reinterpret_cast<അക്ഷരം*> (obj);
      പ്രതിജ്ഞാ(pid < properties_.size());
      properties_[pid].type->set_string(buf, val);
}

string class_type::get_string(class_type::inst_t obj, size_t pid) സ്ഥിരം
{
      അക്ഷരം*buf = reinterpret_cast<അക്ഷരം*> (obj);
      പ്രതിജ്ഞാ(pid < properties_.size());
      തിരികെ properties_[pid].type->get_string(buf);
}

ശൂന്യം class_type::set_object(class_type::inst_t obj, size_t pid,
			    സ്ഥിരം vvp_object_t&val, size_t idx) സ്ഥിരം
{
      അക്ഷരം*buf = reinterpret_cast<അക്ഷരം*> (obj);
      പ്രതിജ്ഞാ(pid < properties_.size());
      properties_[pid].type->set_object(buf, val, idx);
}

ശൂന്യം class_type::get_object(class_type::inst_t obj, size_t pid,
			    vvp_object_t&val, size_t idx) സ്ഥിരം
{
      അക്ഷരം*buf = reinterpret_cast<അക്ഷരം*> (obj);
      പ്രതിജ്ഞാ(pid < properties_.size());
      properties_[pid].type->get_object(buf, val, idx);
}

ശൂന്യം class_type::copy_property(class_type::inst_t dst, size_t pid, class_type::inst_t src) സ്ഥിരം
{
      അക്ഷരം*dst_buf = reinterpret_cast<അക്ഷരം*> (dst);
      അക്ഷരം*src_buf = reinterpret_cast<അക്ഷരം*> (src);

      പ്രതിജ്ഞാ(pid < properties_.size());

      properties_[pid].type->copy(dst_buf, src_buf);
}

പൂർണ്ണസംഖ്യ class_type::get_type_code(ശൂന്യം) സ്ഥിരം
{
      തിരികെ vpiClassDefn;
}

സ്ഥിതം class_type*compile_class = 0;

ശൂന്യം compile_class_start(അക്ഷരം*lab, അക്ഷരം*nam, unsigned ntype)
{
      പ്രതിജ്ഞാ(compile_class == 0);
      compile_class = പുതിയ class_type(nam, ntype);
      compile_vpi_ചിഹ്നം(lab, compile_class);
      സ്വതന്ത്രം(lab);
      delete[]nam;
}

ശൂന്യം compile_class_property(unsigned idx, അക്ഷരം*nam, അക്ഷരം*typ, uint64_t array_size)
{
      പ്രതിജ്ഞാ(compile_class);
      compile_class->set_property(idx, nam, typ, array_size);
      delete[]nam;
      delete[]typ;
}

ശൂന്യം compile_class_done(ശൂന്യം)
{
      __vpiScope*scope = vpip_peek_current_scope();
      പ്രതിജ്ഞാ(scope);
      പ്രതിജ്ഞാ(compile_class);
      compile_class->finish_setup();
      scope->classes[compile_class->class_name()] = compile_class;
      compile_class = 0;
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം class_def_delete(class_type *item)
{
      delete item;
}
#endif
