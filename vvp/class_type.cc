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

using নামাকাশ std;

/*
 * This class_property_t class is an abstract base class for
 * representing a property of an instance. The definition keeps and
 * array (of pointers) of these in order to define the the class.
 */
বর্গ class_property_t {
    সার্বজনিক:
      অন্তঃস্থ class_property_t() { offset_ = 0; }
      আভাস ~class_property_t() =0;
	// How much space does an instance of this property require?
      আভাস size_t instance_size() ধ্রুব =0;

      শূন্য set_offset(size_t off) { offset_ = off; }

    সার্বজনিক:
      আভাস শূন্য construct(বর্ণ*buf) ধ্রুব;
      আভাস শূন্য destruct(বর্ণ*buf) ধ্রুব;

      আভাস শূন্য set_vec4(বর্ণ*buf, ধ্রুব vvp_vector4_t&val);
      আভাস শূন্য get_vec4(বর্ণ*buf, vvp_vector4_t&val);

      আভাস শূন্য set_real(বর্ণ*buf, দ্বিগুণ val);
      আভাস দ্বিগুণ get_real(বর্ণ*buf);

      আভাস শূন্য set_string(বর্ণ*buf, ধ্রুব std::string&val);
      আভাস string get_string(বর্ণ*buf);

      আভাস শূন্য set_object(বর্ণ*buf, ধ্রুব vvp_object_t&val, uint64_t element);
      আভাস শূন্য get_object(বর্ণ*buf, vvp_object_t&val, uint64_t element);

	// Implement polymorphic shallow copy.
      আভাস শূন্য copy(বর্ণ*buf, বর্ণ*src) = 0;

    রক্ষিত:
      size_t offset_;
};

class_property_t::~class_property_t()
{
}

শূন্য class_property_t::construct(বর্ণ*) ধ্রুব
{
}

শূন্য class_property_t::destruct(বর্ণ*) ধ্রুব
{
}

শূন্য class_property_t::set_vec4(বর্ণ*, ধ্রুব vvp_vector4_t&)
{
      প্রতিজ্ঞা(0);
}

শূন্য class_property_t::get_vec4(বর্ণ*, vvp_vector4_t&)
{
      প্রতিজ্ঞা(0);
}

শূন্য class_property_t::set_real(বর্ণ*, দ্বিগুণ)
{
      প্রতিজ্ঞা(0);
}

দ্বিগুণ class_property_t::get_real(বর্ণ*)
{
      প্রতিজ্ঞা(0);
      প্রত্যাগম 0.0;
}

শূন্য class_property_t::set_string(বর্ণ*, ধ্রুব string&)
{
      প্রতিজ্ঞা(0);
}

string class_property_t::get_string(বর্ণ*)
{
      প্রতিজ্ঞা(0);
      প্রত্যাগম "";
}

শূন্য class_property_t::set_object(বর্ণ*, ধ্রুব vvp_object_t&, uint64_t)
{
      প্রতিজ্ঞা(0);
}

শূন্য class_property_t::get_object(বর্ণ*, vvp_object_t&, uint64_t)
{
      প্রতিজ্ঞা(0);
}

/*
 */
template <বর্গ T> বর্গ property_atom : সার্বজনিক class_property_t {
    সার্বজনিক:
      অন্তঃস্থ explicit property_atom(শূন্য) { }
      ~property_atom() override { }

      size_t instance_size() ধ্রুব override { প্রত্যাগম পরিমাণ(T); }

    সার্বজনিক:
      শূন্য construct(বর্ণ*buf) ধ্রুব override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0;
      }

      শূন্য set_vec4(বর্ণ*buf, ধ্রুব vvp_vector4_t&val) override;
      শূন্য get_vec4(বর্ণ*buf, vvp_vector4_t&val) override;

      শূন্য copy(বর্ণ*dst, বর্ণ*src) override;
};

বর্গ property_bit : সার্বজনিক class_property_t {
    সার্বজনিক:
      explicit অন্তঃস্থ property_bit(size_t wid): wid_(wid) { }
      ~property_bit() override { }

      size_t instance_size() ধ্রুব override { প্রত্যাগম পরিমাণ(vvp_vector2_t); }

    সার্বজনিক:
      শূন্য construct(বর্ণ*buf) ধ্রুব override
      { নূতন (buf+offset_) vvp_vector2_t (0, wid_); }

      শূন্য destruct(বর্ণ*buf) ধ্রুব override
      { vvp_vector2_t*tmp = reinterpret_cast<vvp_vector2_t*>(buf+offset_);
	tmp->~vvp_vector2_t();
      }

      শূন্য set_vec4(বর্ণ*buf, ধ্রুব vvp_vector4_t&val) override;
      শূন্য get_vec4(বর্ণ*buf, vvp_vector4_t&val) override;

      শূন্য copy(বর্ণ*dst, বর্ণ*src) override;

    গুপ্ত:
      size_t wid_;
};

বর্গ property_logic : সার্বজনিক class_property_t {
    সার্বজনিক:
      explicit অন্তঃস্থ property_logic(size_t wid): wid_(wid) { }
      ~property_logic() override { }

      size_t instance_size() ধ্রুব override { প্রত্যাগম পরিমাণ(vvp_vector4_t); }

    সার্বজনিক:
      শূন্য construct(বর্ণ*buf) ধ্রুব override
      { নূতন (buf+offset_) vvp_vector4_t (wid_); }

      শূন্য destruct(বর্ণ*buf) ধ্রুব override
      { vvp_vector4_t*tmp = reinterpret_cast<vvp_vector4_t*>(buf+offset_);
	tmp->~vvp_vector4_t();
      }

      শূন্য set_vec4(বর্ণ*buf, ধ্রুব vvp_vector4_t&val) override;
      শূন্য get_vec4(বর্ণ*buf, vvp_vector4_t&val) override;

      শূন্য copy(বর্ণ*dst, বর্ণ*src) override;

    গুপ্ত:
      size_t wid_;
};

template <বর্গ T> বর্গ property_real : সার্বজনিক class_property_t {
    সার্বজনিক:
      অন্তঃস্থ explicit property_real(শূন্য) { }
      ~property_real() override { }

      size_t instance_size() ধ্রুব override { প্রত্যাগম পরিমাণ(T); }

    সার্বজনিক:
      শূন্য construct(বর্ণ*buf) ধ্রুব override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0.0;
      }

      শূন্য set_real(বর্ণ*buf, দ্বিগুণ val) override;
      দ্বিগুণ get_real(বর্ণ*buf) override;

      শূন্য copy(বর্ণ*dst, বর্ণ*src) override;
};

বর্গ property_string : সার্বজনিক class_property_t {
    সার্বজনিক:
      অন্তঃস্থ explicit property_string(শূন্য) { }
      ~property_string() override { }

      size_t instance_size() ধ্রুব override { প্রত্যাগম পরিমাণ(std::string); }

    সার্বজনিক:
      শূন্য construct(বর্ণ*buf) ধ্রুব override
      { /* string*tmp = */ নূতন (buf+offset_) string; }

      শূন্য destruct(বর্ণ*buf) ধ্রুব override
      { string*tmp = reinterpret_cast<string*> (buf+offset_);
	tmp->~string();
      }

      শূন্য set_string(বর্ণ*buf, ধ্রুব string&) override;
      string get_string(বর্ণ*buf) override;

      শূন্য copy(বর্ণ*dst, বর্ণ*src) override;
};

বর্গ property_object : সার্বজনিক class_property_t {
    সার্বজনিক:
      অন্তঃস্থ explicit property_object(uint64_t as): array_size_(as==0? 1 : as) { }
      ~property_object() override { }

      size_t instance_size() ধ্রুব override { প্রত্যাগম array_size_ * পরিমাণ(vvp_object_t); }

    সার্বজনিক:
      শূন্য construct(বর্ণ*buf) ধ্রুব override;

      শূন্য destruct(বর্ণ*buf) ধ্রুব override;

      শূন্য set_object(বর্ণ*buf, ধ্রুব vvp_object_t&, uint64_t) override;
      শূন্য get_object(বর্ণ*buf, vvp_object_t&, uint64_t) override;

      শূন্য copy(বর্ণ*dst, বর্ণ*src) override;

    গুপ্ত:
      size_t array_size_;
};

template <বর্গ T> শূন্য property_atom<T>::set_vec4(বর্ণ*buf, ধ্রুব vvp_vector4_t&val)
{
      T*tmp = reinterpret_cast<T*> (buf+offset_);
      তার্কিক flag = vector4_to_মান(val, *tmp, সত্য, অসত্য);
      প্রতিজ্ঞা(flag);
}

template <বর্গ T> শূন্য property_atom<T>::get_vec4(বর্ণ*buf, vvp_vector4_t&val)
{
      T*src = reinterpret_cast<T*> (buf+offset_);
      ধ্রুব size_t tmp_cnt = পরিমাণ(T)<পরিমাণ(unsigned long)
				       ? 1
				       : পরিমাণ(T) / পরিমাণ(unsigned long);
      unsigned long tmp[tmp_cnt];
      tmp[0] = src[0];

      প্রতি (size_t idx = 1 ; idx < tmp_cnt ; idx += 1)
	    tmp[idx] = src[0] >> idx * 8 * পরিমাণ(tmp[0]);

      val.resize(8*পরিমাণ(T));
      val.setarray(0, val.size(), tmp);
}

template <বর্গ T> শূন্য property_atom<T>::copy(বর্ণ*dst, বর্ণ*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

শূন্য property_bit::set_vec4(বর্ণ*buf, ধ্রুব vvp_vector4_t&val)
{
      vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      *obj = val;
}

শূন্য property_bit::get_vec4(বর্ণ*buf, vvp_vector4_t&val)
{
      ধ্রুব vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      val = vector2_to_vector4(*obj, obj->size());
}

শূন্য property_bit::copy(বর্ণ*dst, বর্ণ*src)
{
      vvp_vector2_t*dst_obj = reinterpret_cast<vvp_vector2_t*> (dst+offset_);
      ধ্রুব vvp_vector2_t*src_obj = reinterpret_cast<vvp_vector2_t*> (src+offset_);
      *dst_obj = *src_obj;
}

শূন্য property_logic::set_vec4(বর্ণ*buf, ধ্রুব vvp_vector4_t&val)
{
      vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      *obj = val;
}

শূন্য property_logic::get_vec4(বর্ণ*buf, vvp_vector4_t&val)
{
      ধ্রুব vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      val = *obj;
}

শূন্য property_logic::copy(বর্ণ*dst, বর্ণ*src)
{
      vvp_vector4_t*dst_obj = reinterpret_cast<vvp_vector4_t*> (dst+offset_);
      ধ্রুব vvp_vector4_t*src_obj = reinterpret_cast<vvp_vector4_t*> (src+offset_);
      *dst_obj = *src_obj;
}

template <বর্গ T> শূন্য property_real<T>::set_real(বর্ণ*buf, দ্বিগুণ val)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      *tmp = val;
}

template <বর্গ T> দ্বিগুণ property_real<T>::get_real(বর্ণ*buf)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      প্রত্যাগম *tmp;
}

template <বর্গ T> শূন্য property_real<T>::copy(বর্ণ*dst, বর্ণ*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

শূন্য property_string::set_string(বর্ণ*buf, ধ্রুব string&val)
{
      string*tmp = reinterpret_cast<string*>(buf+offset_);
      *tmp = val;
}

string property_string::get_string(বর্ণ*buf)
{
      ধ্রুব string*tmp = reinterpret_cast<string*>(buf+offset_);
      প্রত্যাগম *tmp;
}

শূন্য property_string::copy(বর্ণ*dst, বর্ণ*src)
{
      string*dst_obj = reinterpret_cast<string*> (dst+offset_);
      ধ্রুব string*src_obj = reinterpret_cast<string*> (src+offset_);
      *dst_obj = *src_obj;
}

শূন্য property_object::construct(বর্ণ*buf) ধ্রুব
{
      প্রতি (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    নূতন (buf+offset_ + idx*পরিমাণ(vvp_object_t)) vvp_object_t;
}

শূন্য property_object::destruct(বর্ণ*buf) ধ্রুব
{
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*> (buf+offset_);
      প্রতি (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    (tmp+idx)->~vvp_object_t();
}

শূন্য property_object::set_object(বর্ণ*buf, ধ্রুব vvp_object_t&val, uint64_t idx)
{
      প্রতিজ্ঞা(idx < array_size_);
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      tmp[idx] = val;
}

শূন্য property_object::get_object(বর্ণ*buf, vvp_object_t&val, uint64_t idx)
{
      প্রতিজ্ঞা(idx < array_size_);
      ধ্রুব vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      val = tmp[idx];
}

শূন্য property_object::copy(বর্ণ*dst, বর্ণ*src)
{
      vvp_object_t*dst_obj = reinterpret_cast<vvp_object_t*>(dst+offset_);
      ধ্রুব vvp_object_t*src_obj = reinterpret_cast<vvp_object_t*>(src+offset_);
      প্রতি (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    dst_obj[idx] = src_obj[idx];
}

/* **** */

class_type::class_type(ধ্রুব string&nam, size_t nprop)
: class_name_(nam), properties_(nprop)
{
      instance_size_ = 0;
}

class_type::~class_type()
{
      প্রতি (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    delete properties_[idx].type;
}

শূন্য class_type::set_property(size_t idx, ধ্রুব string&name, ধ্রুব string&type, uint64_t array_size)
{
      প্রতিজ্ঞা(idx < properties_.size());
      properties_[idx].name = name;

      যদি (type == "b8")
	    properties_[idx].type = নূতন property_atom<uint8_t>;
      অন্যথা যদি (type == "b16")
	    properties_[idx].type = নূতন property_atom<uint16_t>;
      অন্যথা যদি (type == "b32")
	    properties_[idx].type = নূতন property_atom<uint32_t>;
      অন্যথা যদি (type == "b64")
	    properties_[idx].type = নূতন property_atom<uint64_t>;
      অন্যথা যদি (type == "sb8")
	    properties_[idx].type = নূতন property_atom<int8_t>;
      অন্যথা যদি (type == "sb16")
	    properties_[idx].type = নূতন property_atom<int16_t>;
      অন্যথা যদি (type == "sb32")
	    properties_[idx].type = নূতন property_atom<int32_t>;
      অন্যথা যদি (type == "sb64")
	    properties_[idx].type = নূতন property_atom<int64_t>;
      অন্যথা যদি (type == "r")
	    properties_[idx].type = নূতন property_real<দ্বিগুণ>;
      অন্যথা যদি (type == "S")
	    properties_[idx].type = নূতন property_string;
      অন্যথা যদি (type == "o")
	    properties_[idx].type = নূতন property_object(array_size);
      অন্যথা যদি (type[0] == 'b') {
	    size_t wid = strtoul(type.c_str()+1, 0, 0);
	    properties_[idx].type = নূতন property_bit(wid);
      } অন্যথা যদি (type[0] == 'L') {
	    size_t wid = strtoul(type.c_str()+1,0,0);
	    properties_[idx].type = নূতন property_logic(wid);
      } অন্যথা যদি (type[0] == 's' && type[1] == 'L') {
	    size_t wid = strtoul(type.c_str()+2,0,0);
	    properties_[idx].type = নূতন property_logic(wid);
      } অন্যথা {
	    properties_[idx].type = 0;
      }
}

শূন্য class_type::finish_setup(শূন্য)
{
      map<size_t, vector<size_t> > size_map;
	// Add up all the sizes to get a total instance size. This
	// figures out how much memory a complete instance will need.
      size_t accum = 0;
      প্রতি (size_t idx = 0 ; idx < properties_.size() ; idx += 1) {
	    প্রতিজ্ঞা(properties_[idx].type);
	    size_t instance_size = properties_[idx].type->instance_size();
	    accum += instance_size;
	    size_map[instance_size].push_back(idx);
      }

      instance_size_ = accum;

	// Now allocate the properties to offsets within an instance
	// space. Allocate the properties largest objects first so
	// that they are assured better alignment.
      accum = 0;
      প্রতি (map<size_t, vector<size_t> >::reverse_iterator cur = size_map.rbegin()
		 ; cur != size_map.rend() ; ++ cur) {
	    প্রতি (size_t idx = 0 ; idx < cur->second.size() ; idx += 1) {
		  size_t pid = cur->second[idx];
		  class_property_t*ptype = properties_[pid].type;
		  প্রতিজ্ঞা(ptype->instance_size() == cur->first);
		  ptype->set_offset(accum);
		  accum += cur->first;
	    }
      }
}

class_type::inst_t class_type::instance_new() ধ্রুব
{
      বর্ণ*buf = নূতন বর্ণ [instance_size_];

      প্রতি (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->construct(buf);

      প্রত্যাগম reinterpret_cast<inst_t> (buf);
}

শূন্য class_type::instance_delete(class_type::inst_t obj) ধ্রুব
{
      বর্ণ*buf = reinterpret_cast<বর্ণ*> (obj);

      প্রতি (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->destruct(buf);

      delete[]buf;
}

শূন্য class_type::set_vec4(class_type::inst_t obj, size_t pid,
			  ধ্রুব vvp_vector4_t&val) ধ্রুব
{
      বর্ণ*buf = reinterpret_cast<বর্ণ*> (obj);
      প্রতিজ্ঞা(pid < properties_.size());
      properties_[pid].type->set_vec4(buf, val);
}

শূন্য class_type::get_vec4(class_type::inst_t obj, size_t pid,
			  vvp_vector4_t&val) ধ্রুব
{
      বর্ণ*buf = reinterpret_cast<বর্ণ*> (obj);
      প্রতিজ্ঞা(pid < properties_.size());
      properties_[pid].type->get_vec4(buf, val);
}

শূন্য class_type::set_real(class_type::inst_t obj, size_t pid,
			  দ্বিগুণ val) ধ্রুব
{
      বর্ণ*buf = reinterpret_cast<বর্ণ*> (obj);
      প্রতিজ্ঞা(pid < properties_.size());
      properties_[pid].type->set_real(buf, val);
}

দ্বিগুণ class_type::get_real(class_type::inst_t obj, size_t pid) ধ্রুব
{
      বর্ণ*buf = reinterpret_cast<বর্ণ*> (obj);
      প্রতিজ্ঞা(pid < properties_.size());
      প্রত্যাগম properties_[pid].type->get_real(buf);
}

শূন্য class_type::set_string(class_type::inst_t obj, size_t pid,
			    ধ্রুব string&val) ধ্রুব
{
      বর্ণ*buf = reinterpret_cast<বর্ণ*> (obj);
      প্রতিজ্ঞা(pid < properties_.size());
      properties_[pid].type->set_string(buf, val);
}

string class_type::get_string(class_type::inst_t obj, size_t pid) ধ্রুব
{
      বর্ণ*buf = reinterpret_cast<বর্ণ*> (obj);
      প্রতিজ্ঞা(pid < properties_.size());
      প্রত্যাগম properties_[pid].type->get_string(buf);
}

শূন্য class_type::set_object(class_type::inst_t obj, size_t pid,
			    ধ্রুব vvp_object_t&val, size_t idx) ধ্রুব
{
      বর্ণ*buf = reinterpret_cast<বর্ণ*> (obj);
      প্রতিজ্ঞা(pid < properties_.size());
      properties_[pid].type->set_object(buf, val, idx);
}

শূন্য class_type::get_object(class_type::inst_t obj, size_t pid,
			    vvp_object_t&val, size_t idx) ধ্রুব
{
      বর্ণ*buf = reinterpret_cast<বর্ণ*> (obj);
      প্রতিজ্ঞা(pid < properties_.size());
      properties_[pid].type->get_object(buf, val, idx);
}

শূন্য class_type::copy_property(class_type::inst_t dst, size_t pid, class_type::inst_t src) ধ্রুব
{
      বর্ণ*dst_buf = reinterpret_cast<বর্ণ*> (dst);
      বর্ণ*src_buf = reinterpret_cast<বর্ণ*> (src);

      প্রতিজ্ঞা(pid < properties_.size());

      properties_[pid].type->copy(dst_buf, src_buf);
}

পূর্ণাঙ্ক class_type::get_type_code(শূন্য) ধ্রুব
{
      প্রত্যাগম vpiClassDefn;
}

স্থির class_type*compile_class = 0;

শূন্য compile_class_start(বর্ণ*lab, বর্ণ*nam, unsigned ntype)
{
      প্রতিজ্ঞা(compile_class == 0);
      compile_class = নূতন class_type(nam, ntype);
      compile_vpi_প্রতীক(lab, compile_class);
      মুক্ত(lab);
      delete[]nam;
}

শূন্য compile_class_property(unsigned idx, বর্ণ*nam, বর্ণ*typ, uint64_t array_size)
{
      প্রতিজ্ঞা(compile_class);
      compile_class->set_property(idx, nam, typ, array_size);
      delete[]nam;
      delete[]typ;
}

শূন্য compile_class_done(শূন্য)
{
      __vpiScope*scope = vpip_peek_current_scope();
      প্রতিজ্ঞা(scope);
      প্রতিজ্ঞা(compile_class);
      compile_class->finish_setup();
      scope->classes[compile_class->class_name()] = compile_class;
      compile_class = 0;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য class_def_delete(class_type *item)
{
      delete item;
}
#endif
