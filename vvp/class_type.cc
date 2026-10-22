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
শ্রেণী class_property_t {
    সর্বজনীন:
      অন্তঃরেখ class_property_t() { offset_ = 0; }
      আভাসী ~class_property_t() =0;
	// How much space does an instance of this property require?
      আভাসী size_t instance_আকার() ধ্রুবক =0;

      শূন্য set_offset(size_t off) { offset_ = off; }

    সর্বজনীন:
      আভাসী শূন্য construct(অক্ষর*buf) ধ্রুবক;
      আভাসী শূন্য destruct(অক্ষর*buf) ধ্রুবক;

      আভাসী শূন্য set_vec4(অক্ষর*buf, ধ্রুবক vvp_vector4_t&val);
      আভাসী শূন্য get_vec4(অক্ষর*buf, vvp_vector4_t&val);

      আভাসী শূন্য set_real(অক্ষর*buf, দ্বিগুণ val);
      আভাসী দ্বিগুণ get_real(অক্ষর*buf);

      আভাসী শূন্য set_string(অক্ষর*buf, ধ্রুবক std::string&val);
      আভাসী string get_string(অক্ষর*buf);

      আভাসী শূন্য set_object(অক্ষর*buf, ধ্রুবক vvp_object_t&val, uint64_t element);
      আভাসী শূন্য get_object(অক্ষর*buf, vvp_object_t&val, uint64_t element);

	// Implement polymorphic shallow copy.
      আভাসী শূন্য copy(অক্ষর*buf, অক্ষর*src) = 0;

    সুরক্ষিত:
      size_t offset_;
};

class_property_t::~class_property_t()
{
}

শূন্য class_property_t::construct(অক্ষর*) ধ্রুবক
{
}

শূন্য class_property_t::destruct(অক্ষর*) ধ্রুবক
{
}

শূন্য class_property_t::set_vec4(অক্ষর*, ধ্রুবক vvp_vector4_t&)
{
      প্রতিজ্ঞা(0);
}

শূন্য class_property_t::get_vec4(অক্ষর*, vvp_vector4_t&)
{
      প্রতিজ্ঞা(0);
}

শূন্য class_property_t::set_real(অক্ষর*, দ্বিগুণ)
{
      প্রতিজ্ঞা(0);
}

দ্বিগুণ class_property_t::get_real(অক্ষর*)
{
      প্রতিজ্ঞা(0);
      ফেরত 0.0;
}

শূন্য class_property_t::set_string(অক্ষর*, ধ্রুবক string&)
{
      প্রতিজ্ঞা(0);
}

string class_property_t::get_string(অক্ষর*)
{
      প্রতিজ্ঞা(0);
      ফেরত "";
}

শূন্য class_property_t::set_object(অক্ষর*, ধ্রুবক vvp_object_t&, uint64_t)
{
      প্রতিজ্ঞা(0);
}

শূন্য class_property_t::get_object(অক্ষর*, vvp_object_t&, uint64_t)
{
      প্রতিজ্ঞা(0);
}

/*
 */
template <শ্রেণী T> শ্রেণী property_atom : সর্বজনীন class_property_t {
    সর্বজনীন:
      অন্তঃরেখ explicit property_atom(শূন্য) { }
      ~property_atom() override { }

      size_t instance_আকার() ধ্রুবক override { ফেরত আকার(T); }

    সর্বজনীন:
      শূন্য construct(অক্ষর*buf) ধ্রুবক override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0;
      }

      শূন্য set_vec4(অক্ষর*buf, ধ্রুবক vvp_vector4_t&val) override;
      শূন্য get_vec4(অক্ষর*buf, vvp_vector4_t&val) override;

      শূন্য copy(অক্ষর*dst, অক্ষর*src) override;
};

শ্রেণী property_bit : সর্বজনীন class_property_t {
    সর্বজনীন:
      explicit অন্তঃরেখ property_bit(size_t wid): wid_(wid) { }
      ~property_bit() override { }

      size_t instance_আকার() ধ্রুবক override { ফেরত আকার(vvp_vector2_t); }

    সর্বজনীন:
      শূন্য construct(অক্ষর*buf) ধ্রুবক override
      { নতুন (buf+offset_) vvp_vector2_t (0, wid_); }

      শূন্য destruct(অক্ষর*buf) ধ্রুবক override
      { vvp_vector2_t*tmp = reinterpret_cast<vvp_vector2_t*>(buf+offset_);
	tmp->~vvp_vector2_t();
      }

      শূন্য set_vec4(অক্ষর*buf, ধ্রুবক vvp_vector4_t&val) override;
      শূন্য get_vec4(অক্ষর*buf, vvp_vector4_t&val) override;

      শূন্য copy(অক্ষর*dst, অক্ষর*src) override;

    ব্যক্তিগত:
      size_t wid_;
};

শ্রেণী property_logic : সর্বজনীন class_property_t {
    সর্বজনীন:
      explicit অন্তঃরেখ property_logic(size_t wid): wid_(wid) { }
      ~property_logic() override { }

      size_t instance_আকার() ধ্রুবক override { ফেরত আকার(vvp_vector4_t); }

    সর্বজনীন:
      শূন্য construct(অক্ষর*buf) ধ্রুবক override
      { নতুন (buf+offset_) vvp_vector4_t (wid_); }

      শূন্য destruct(অক্ষর*buf) ধ্রুবক override
      { vvp_vector4_t*tmp = reinterpret_cast<vvp_vector4_t*>(buf+offset_);
	tmp->~vvp_vector4_t();
      }

      শূন্য set_vec4(অক্ষর*buf, ধ্রুবক vvp_vector4_t&val) override;
      শূন্য get_vec4(অক্ষর*buf, vvp_vector4_t&val) override;

      শূন্য copy(অক্ষর*dst, অক্ষর*src) override;

    ব্যক্তিগত:
      size_t wid_;
};

template <শ্রেণী T> শ্রেণী property_real : সর্বজনীন class_property_t {
    সর্বজনীন:
      অন্তঃরেখ explicit property_real(শূন্য) { }
      ~property_real() override { }

      size_t instance_আকার() ধ্রুবক override { ফেরত আকার(T); }

    সর্বজনীন:
      শূন্য construct(অক্ষর*buf) ধ্রুবক override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0.0;
      }

      শূন্য set_real(অক্ষর*buf, দ্বিগুণ val) override;
      দ্বিগুণ get_real(অক্ষর*buf) override;

      শূন্য copy(অক্ষর*dst, অক্ষর*src) override;
};

শ্রেণী property_string : সর্বজনীন class_property_t {
    সর্বজনীন:
      অন্তঃরেখ explicit property_string(শূন্য) { }
      ~property_string() override { }

      size_t instance_আকার() ধ্রুবক override { ফেরত আকার(std::string); }

    সর্বজনীন:
      শূন্য construct(অক্ষর*buf) ধ্রুবক override
      { /* string*tmp = */ নতুন (buf+offset_) string; }

      শূন্য destruct(অক্ষর*buf) ধ্রুবক override
      { string*tmp = reinterpret_cast<string*> (buf+offset_);
	tmp->~string();
      }

      শূন্য set_string(অক্ষর*buf, ধ্রুবক string&) override;
      string get_string(অক্ষর*buf) override;

      শূন্য copy(অক্ষর*dst, অক্ষর*src) override;
};

শ্রেণী property_object : সর্বজনীন class_property_t {
    সর্বজনীন:
      অন্তঃরেখ explicit property_object(uint64_t as): array_আকার(as==0? 1 : as) { }
      ~property_object() override { }

      size_t instance_আকার() ধ্রুবক override { ফেরত array_আকার * আকার(vvp_object_t); }

    সর্বজনীন:
      শূন্য construct(অক্ষর*buf) ধ্রুবক override;

      শূন্য destruct(অক্ষর*buf) ধ্রুবক override;

      শূন্য set_object(অক্ষর*buf, ধ্রুবক vvp_object_t&, uint64_t) override;
      শূন্য get_object(অক্ষর*buf, vvp_object_t&, uint64_t) override;

      শূন্য copy(অক্ষর*dst, অক্ষর*src) override;

    ব্যক্তিগত:
      size_t array_আকার;
};

template <শ্রেণী T> শূন্য property_atom<T>::set_vec4(অক্ষর*buf, ধ্রুবক vvp_vector4_t&val)
{
      T*tmp = reinterpret_cast<T*> (buf+offset_);
      যৌক্তিক flag = vector4_to_মান(val, *tmp, সত্য, মিথ্যা);
      প্রতিজ্ঞা(flag);
}

template <শ্রেণী T> শূন্য property_atom<T>::get_vec4(অক্ষর*buf, vvp_vector4_t&val)
{
      T*src = reinterpret_cast<T*> (buf+offset_);
      ধ্রুবক size_t tmp_cnt = আকার(T)<আকার(unsigned long)
				       ? 1
				       : আকার(T) / আকার(unsigned long);
      unsigned long tmp[tmp_cnt];
      tmp[0] = src[0];

      জন্য (size_t idx = 1 ; idx < tmp_cnt ; idx += 1)
	    tmp[idx] = src[0] >> idx * 8 * আকার(tmp[0]);

      val.resize(8*আকার(T));
      val.setarray(0, val.size(), tmp);
}

template <শ্রেণী T> শূন্য property_atom<T>::copy(অক্ষর*dst, অক্ষর*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

শূন্য property_bit::set_vec4(অক্ষর*buf, ধ্রুবক vvp_vector4_t&val)
{
      vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      *obj = val;
}

শূন্য property_bit::get_vec4(অক্ষর*buf, vvp_vector4_t&val)
{
      ধ্রুবক vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      val = vector2_to_vector4(*obj, obj->size());
}

শূন্য property_bit::copy(অক্ষর*dst, অক্ষর*src)
{
      vvp_vector2_t*dst_obj = reinterpret_cast<vvp_vector2_t*> (dst+offset_);
      ধ্রুবক vvp_vector2_t*src_obj = reinterpret_cast<vvp_vector2_t*> (src+offset_);
      *dst_obj = *src_obj;
}

শূন্য property_logic::set_vec4(অক্ষর*buf, ধ্রুবক vvp_vector4_t&val)
{
      vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      *obj = val;
}

শূন্য property_logic::get_vec4(অক্ষর*buf, vvp_vector4_t&val)
{
      ধ্রুবক vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      val = *obj;
}

শূন্য property_logic::copy(অক্ষর*dst, অক্ষর*src)
{
      vvp_vector4_t*dst_obj = reinterpret_cast<vvp_vector4_t*> (dst+offset_);
      ধ্রুবক vvp_vector4_t*src_obj = reinterpret_cast<vvp_vector4_t*> (src+offset_);
      *dst_obj = *src_obj;
}

template <শ্রেণী T> শূন্য property_real<T>::set_real(অক্ষর*buf, দ্বিগুণ val)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      *tmp = val;
}

template <শ্রেণী T> দ্বিগুণ property_real<T>::get_real(অক্ষর*buf)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      ফেরত *tmp;
}

template <শ্রেণী T> শূন্য property_real<T>::copy(অক্ষর*dst, অক্ষর*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

শূন্য property_string::set_string(অক্ষর*buf, ধ্রুবক string&val)
{
      string*tmp = reinterpret_cast<string*>(buf+offset_);
      *tmp = val;
}

string property_string::get_string(অক্ষর*buf)
{
      ধ্রুবক string*tmp = reinterpret_cast<string*>(buf+offset_);
      ফেরত *tmp;
}

শূন্য property_string::copy(অক্ষর*dst, অক্ষর*src)
{
      string*dst_obj = reinterpret_cast<string*> (dst+offset_);
      ধ্রুবক string*src_obj = reinterpret_cast<string*> (src+offset_);
      *dst_obj = *src_obj;
}

শূন্য property_object::construct(অক্ষর*buf) ধ্রুবক
{
      জন্য (size_t idx = 0 ; idx < array_আকার ; idx += 1)
	    নতুন (buf+offset_ + idx*আকার(vvp_object_t)) vvp_object_t;
}

শূন্য property_object::destruct(অক্ষর*buf) ধ্রুবক
{
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*> (buf+offset_);
      জন্য (size_t idx = 0 ; idx < array_আকার ; idx += 1)
	    (tmp+idx)->~vvp_object_t();
}

শূন্য property_object::set_object(অক্ষর*buf, ধ্রুবক vvp_object_t&val, uint64_t idx)
{
      প্রতিজ্ঞা(idx < array_আকার);
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      tmp[idx] = val;
}

শূন্য property_object::get_object(অক্ষর*buf, vvp_object_t&val, uint64_t idx)
{
      প্রতিজ্ঞা(idx < array_আকার);
      ধ্রুবক vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      val = tmp[idx];
}

শূন্য property_object::copy(অক্ষর*dst, অক্ষর*src)
{
      vvp_object_t*dst_obj = reinterpret_cast<vvp_object_t*>(dst+offset_);
      ধ্রুবক vvp_object_t*src_obj = reinterpret_cast<vvp_object_t*>(src+offset_);
      জন্য (size_t idx = 0 ; idx < array_আকার ; idx += 1)
	    dst_obj[idx] = src_obj[idx];
}

/* **** */

class_type::class_type(ধ্রুবক string&nam, size_t nprop)
: class_name_(nam), properties_(nprop)
{
      instance_আকার_2 = 0;
}

class_type::~class_type()
{
      জন্য (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    delete properties_[idx].type;
}

শূন্য class_type::set_property(size_t idx, ধ্রুবক string&name, ধ্রুবক string&type, uint64_t array_আকার_2)
{
      প্রতিজ্ঞা(idx < properties_.size());
      properties_[idx].name = name;

      যদি (type == "b8")
	    properties_[idx].type = নতুন property_atom<uint8_t>;
      অন্যথা যদি (type == "b16")
	    properties_[idx].type = নতুন property_atom<uint16_t>;
      অন্যথা যদি (type == "b32")
	    properties_[idx].type = নতুন property_atom<uint32_t>;
      অন্যথা যদি (type == "b64")
	    properties_[idx].type = নতুন property_atom<uint64_t>;
      অন্যথা যদি (type == "sb8")
	    properties_[idx].type = নতুন property_atom<int8_t>;
      অন্যথা যদি (type == "sb16")
	    properties_[idx].type = নতুন property_atom<int16_t>;
      অন্যথা যদি (type == "sb32")
	    properties_[idx].type = নতুন property_atom<int32_t>;
      অন্যথা যদি (type == "sb64")
	    properties_[idx].type = নতুন property_atom<int64_t>;
      অন্যথা যদি (type == "r")
	    properties_[idx].type = নতুন property_real<দ্বিগুণ>;
      অন্যথা যদি (type == "S")
	    properties_[idx].type = নতুন property_string;
      অন্যথা যদি (type == "o")
	    properties_[idx].type = নতুন property_object(array_আকার_2);
      অন্যথা যদি (type[0] == 'b') {
	    size_t wid = strtoul(type.c_str()+1, 0, 0);
	    properties_[idx].type = নতুন property_bit(wid);
      } অন্যথা যদি (type[0] == 'L') {
	    size_t wid = strtoul(type.c_str()+1,0,0);
	    properties_[idx].type = নতুন property_logic(wid);
      } অন্যথা যদি (type[0] == 's' && type[1] == 'L') {
	    size_t wid = strtoul(type.c_str()+2,0,0);
	    properties_[idx].type = নতুন property_logic(wid);
      } অন্যথা {
	    properties_[idx].type = 0;
      }
}

শূন্য class_type::finish_setup(শূন্য)
{
      map<size_t, vector<size_t> > আকার_map;
	// Add up all the sizes to get a total instance size. This
	// figures out how much memory a complete instance will need.
      size_t accum = 0;
      জন্য (size_t idx = 0 ; idx < properties_.size() ; idx += 1) {
	    প্রতিজ্ঞা(properties_[idx].type);
	    size_t instance_আকার = properties_[idx].type->instance_আকার();
	    accum += instance_আকার;
	    আকার_map[instance_আকার].push_back(idx);
      }

      instance_আকার_2 = accum;

	// Now allocate the properties to offsets within an instance
	// space. Allocate the properties largest objects first so
	// that they are assured better alignment.
      accum = 0;
      জন্য (map<size_t, vector<size_t> >::reverse_iterator cur = আকার_map.rbegin()
		 ; cur != আকার_map.rend() ; ++ cur) {
	    জন্য (size_t idx = 0 ; idx < cur->second.size() ; idx += 1) {
		  size_t pid = cur->second[idx];
		  class_property_t*ptype = properties_[pid].type;
		  প্রতিজ্ঞা(ptype->instance_আকার() == cur->first);
		  ptype->set_offset(accum);
		  accum += cur->first;
	    }
      }
}

class_type::inst_t class_type::instance_new() ধ্রুবক
{
      অক্ষর*buf = নতুন অক্ষর [instance_আকার_2];

      জন্য (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->construct(buf);

      ফেরত reinterpret_cast<inst_t> (buf);
}

শূন্য class_type::instance_delete(class_type::inst_t obj) ধ্রুবক
{
      অক্ষর*buf = reinterpret_cast<অক্ষর*> (obj);

      জন্য (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->destruct(buf);

      delete[]buf;
}

শূন্য class_type::set_vec4(class_type::inst_t obj, size_t pid,
			  ধ্রুবক vvp_vector4_t&val) ধ্রুবক
{
      অক্ষর*buf = reinterpret_cast<অক্ষর*> (obj);
      প্রতিজ্ঞা(pid < properties_.size());
      properties_[pid].type->set_vec4(buf, val);
}

শূন্য class_type::get_vec4(class_type::inst_t obj, size_t pid,
			  vvp_vector4_t&val) ধ্রুবক
{
      অক্ষর*buf = reinterpret_cast<অক্ষর*> (obj);
      প্রতিজ্ঞা(pid < properties_.size());
      properties_[pid].type->get_vec4(buf, val);
}

শূন্য class_type::set_real(class_type::inst_t obj, size_t pid,
			  দ্বিগুণ val) ধ্রুবক
{
      অক্ষর*buf = reinterpret_cast<অক্ষর*> (obj);
      প্রতিজ্ঞা(pid < properties_.size());
      properties_[pid].type->set_real(buf, val);
}

দ্বিগুণ class_type::get_real(class_type::inst_t obj, size_t pid) ধ্রুবক
{
      অক্ষর*buf = reinterpret_cast<অক্ষর*> (obj);
      প্রতিজ্ঞা(pid < properties_.size());
      ফেরত properties_[pid].type->get_real(buf);
}

শূন্য class_type::set_string(class_type::inst_t obj, size_t pid,
			    ধ্রুবক string&val) ধ্রুবক
{
      অক্ষর*buf = reinterpret_cast<অক্ষর*> (obj);
      প্রতিজ্ঞা(pid < properties_.size());
      properties_[pid].type->set_string(buf, val);
}

string class_type::get_string(class_type::inst_t obj, size_t pid) ধ্রুবক
{
      অক্ষর*buf = reinterpret_cast<অক্ষর*> (obj);
      প্রতিজ্ঞা(pid < properties_.size());
      ফেরত properties_[pid].type->get_string(buf);
}

শূন্য class_type::set_object(class_type::inst_t obj, size_t pid,
			    ধ্রুবক vvp_object_t&val, size_t idx) ধ্রুবক
{
      অক্ষর*buf = reinterpret_cast<অক্ষর*> (obj);
      প্রতিজ্ঞা(pid < properties_.size());
      properties_[pid].type->set_object(buf, val, idx);
}

শূন্য class_type::get_object(class_type::inst_t obj, size_t pid,
			    vvp_object_t&val, size_t idx) ধ্রুবক
{
      অক্ষর*buf = reinterpret_cast<অক্ষর*> (obj);
      প্রতিজ্ঞা(pid < properties_.size());
      properties_[pid].type->get_object(buf, val, idx);
}

শূন্য class_type::copy_property(class_type::inst_t dst, size_t pid, class_type::inst_t src) ধ্রুবক
{
      অক্ষর*dst_buf = reinterpret_cast<অক্ষর*> (dst);
      অক্ষর*src_buf = reinterpret_cast<অক্ষর*> (src);

      প্রতিজ্ঞা(pid < properties_.size());

      properties_[pid].type->copy(dst_buf, src_buf);
}

পূর্ণসংখ্যা class_type::get_type_code(শূন্য) ধ্রুবক
{
      ফেরত vpiClassDefn;
}

স্থির class_type*compile_class = 0;

শূন্য compile_class_start(অক্ষর*lab, অক্ষর*nam, unsigned ntype)
{
      প্রতিজ্ঞা(compile_class == 0);
      compile_class = নতুন class_type(nam, ntype);
      compile_vpi_প্রতীক(lab, compile_class);
      মুক্ত(lab);
      delete[]nam;
}

শূন্য compile_class_property(unsigned idx, অক্ষর*nam, অক্ষর*typ, uint64_t array_আকার_2)
{
      প্রতিজ্ঞা(compile_class);
      compile_class->set_property(idx, nam, typ, array_আকার_2);
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
