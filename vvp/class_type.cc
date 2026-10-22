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

using فضاینام std;

/*
 * This class_property_t class is an abstract base class for
 * representing a property of an instance. The definition keeps and
 * array (of pointers) of these in order to define the the class.
 */
رده class_property_t {
    عمومی:
      درونخطی class_property_t() { offset_ = 0; }
      مجازی ~class_property_t() =0;
	// How much space does an instance of this property require?
      مجازی size_t instance_size() ثابت =0;

      تهی set_offset(size_t off) { offset_ = off; }

    عمومی:
      مجازی تهی construct(نویسه*buf) ثابت;
      مجازی تهی destruct(نویسه*buf) ثابت;

      مجازی تهی set_vec4(نویسه*buf, ثابت vvp_vector4_t&val);
      مجازی تهی get_vec4(نویسه*buf, vvp_vector4_t&val);

      مجازی تهی set_real(نویسه*buf, دوگانه val);
      مجازی دوگانه get_real(نویسه*buf);

      مجازی تهی set_string(نویسه*buf, ثابت std::string&val);
      مجازی string get_string(نویسه*buf);

      مجازی تهی set_object(نویسه*buf, ثابت vvp_object_t&val, uint64_t element);
      مجازی تهی get_object(نویسه*buf, vvp_object_t&val, uint64_t element);

	// Implement polymorphic shallow copy.
      مجازی تهی copy(نویسه*buf, نویسه*src) = 0;

    محافظتشده:
      size_t offset_;
};

class_property_t::~class_property_t()
{
}

تهی class_property_t::construct(نویسه*) ثابت
{
}

تهی class_property_t::destruct(نویسه*) ثابت
{
}

تهی class_property_t::set_vec4(نویسه*, ثابت vvp_vector4_t&)
{
      أكد(0);
}

تهی class_property_t::get_vec4(نویسه*, vvp_vector4_t&)
{
      أكد(0);
}

تهی class_property_t::set_real(نویسه*, دوگانه)
{
      أكد(0);
}

دوگانه class_property_t::get_real(نویسه*)
{
      أكد(0);
      بازگشت 0.0;
}

تهی class_property_t::set_string(نویسه*, ثابت string&)
{
      أكد(0);
}

string class_property_t::get_string(نویسه*)
{
      أكد(0);
      بازگشت "";
}

تهی class_property_t::set_object(نویسه*, ثابت vvp_object_t&, uint64_t)
{
      أكد(0);
}

تهی class_property_t::get_object(نویسه*, vvp_object_t&, uint64_t)
{
      أكد(0);
}

/*
 */
template <رده T> رده property_atom : عمومی class_property_t {
    عمومی:
      درونخطی explicit property_atom(تهی) { }
      ~property_atom() override { }

      size_t instance_size() ثابت override { بازگشت اندازه(T); }

    عمومی:
      تهی construct(نویسه*buf) ثابت override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0;
      }

      تهی set_vec4(نویسه*buf, ثابت vvp_vector4_t&val) override;
      تهی get_vec4(نویسه*buf, vvp_vector4_t&val) override;

      تهی copy(نویسه*dst, نویسه*src) override;
};

رده property_bit : عمومی class_property_t {
    عمومی:
      explicit درونخطی property_bit(size_t wid): wid_(wid) { }
      ~property_bit() override { }

      size_t instance_size() ثابت override { بازگشت اندازه(vvp_vector2_t); }

    عمومی:
      تهی construct(نویسه*buf) ثابت override
      { جدید (buf+offset_) vvp_vector2_t (0, wid_); }

      تهی destruct(نویسه*buf) ثابت override
      { vvp_vector2_t*tmp = reinterpret_cast<vvp_vector2_t*>(buf+offset_);
	tmp->~vvp_vector2_t();
      }

      تهی set_vec4(نویسه*buf, ثابت vvp_vector4_t&val) override;
      تهی get_vec4(نویسه*buf, vvp_vector4_t&val) override;

      تهی copy(نویسه*dst, نویسه*src) override;

    خصوصی:
      size_t wid_;
};

رده property_logic : عمومی class_property_t {
    عمومی:
      explicit درونخطی property_logic(size_t wid): wid_(wid) { }
      ~property_logic() override { }

      size_t instance_size() ثابت override { بازگشت اندازه(vvp_vector4_t); }

    عمومی:
      تهی construct(نویسه*buf) ثابت override
      { جدید (buf+offset_) vvp_vector4_t (wid_); }

      تهی destruct(نویسه*buf) ثابت override
      { vvp_vector4_t*tmp = reinterpret_cast<vvp_vector4_t*>(buf+offset_);
	tmp->~vvp_vector4_t();
      }

      تهی set_vec4(نویسه*buf, ثابت vvp_vector4_t&val) override;
      تهی get_vec4(نویسه*buf, vvp_vector4_t&val) override;

      تهی copy(نویسه*dst, نویسه*src) override;

    خصوصی:
      size_t wid_;
};

template <رده T> رده property_real : عمومی class_property_t {
    عمومی:
      درونخطی explicit property_real(تهی) { }
      ~property_real() override { }

      size_t instance_size() ثابت override { بازگشت اندازه(T); }

    عمومی:
      تهی construct(نویسه*buf) ثابت override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0.0;
      }

      تهی set_real(نویسه*buf, دوگانه val) override;
      دوگانه get_real(نویسه*buf) override;

      تهی copy(نویسه*dst, نویسه*src) override;
};

رده property_string : عمومی class_property_t {
    عمومی:
      درونخطی explicit property_string(تهی) { }
      ~property_string() override { }

      size_t instance_size() ثابت override { بازگشت اندازه(std::string); }

    عمومی:
      تهی construct(نویسه*buf) ثابت override
      { /* string*tmp = */ جدید (buf+offset_) string; }

      تهی destruct(نویسه*buf) ثابت override
      { string*tmp = reinterpret_cast<string*> (buf+offset_);
	tmp->~string();
      }

      تهی set_string(نویسه*buf, ثابت string&) override;
      string get_string(نویسه*buf) override;

      تهی copy(نویسه*dst, نویسه*src) override;
};

رده property_object : عمومی class_property_t {
    عمومی:
      درونخطی explicit property_object(uint64_t as): array_size_(as==0? 1 : as) { }
      ~property_object() override { }

      size_t instance_size() ثابت override { بازگشت array_size_ * اندازه(vvp_object_t); }

    عمومی:
      تهی construct(نویسه*buf) ثابت override;

      تهی destruct(نویسه*buf) ثابت override;

      تهی set_object(نویسه*buf, ثابت vvp_object_t&, uint64_t) override;
      تهی get_object(نویسه*buf, vvp_object_t&, uint64_t) override;

      تهی copy(نویسه*dst, نویسه*src) override;

    خصوصی:
      size_t array_size_;
};

template <رده T> تهی property_atom<T>::set_vec4(نویسه*buf, ثابت vvp_vector4_t&val)
{
      T*tmp = reinterpret_cast<T*> (buf+offset_);
      منطقی flag = vector4_to_مقدار(val, *tmp, درست, نادرست);
      أكد(flag);
}

template <رده T> تهی property_atom<T>::get_vec4(نویسه*buf, vvp_vector4_t&val)
{
      T*src = reinterpret_cast<T*> (buf+offset_);
      ثابت size_t tmp_cnt = اندازه(T)<اندازه(unsigned long)
				       ? 1
				       : اندازه(T) / اندازه(unsigned long);
      unsigned long tmp[tmp_cnt];
      tmp[0] = src[0];

      برای (size_t idx = 1 ; idx < tmp_cnt ; idx += 1)
	    tmp[idx] = src[0] >> idx * 8 * اندازه(tmp[0]);

      val.resize(8*اندازه(T));
      val.setarray(0, val.size(), tmp);
}

template <رده T> تهی property_atom<T>::copy(نویسه*dst, نویسه*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

تهی property_bit::set_vec4(نویسه*buf, ثابت vvp_vector4_t&val)
{
      vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      *obj = val;
}

تهی property_bit::get_vec4(نویسه*buf, vvp_vector4_t&val)
{
      ثابت vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      val = vector2_to_vector4(*obj, obj->size());
}

تهی property_bit::copy(نویسه*dst, نویسه*src)
{
      vvp_vector2_t*dst_obj = reinterpret_cast<vvp_vector2_t*> (dst+offset_);
      ثابت vvp_vector2_t*src_obj = reinterpret_cast<vvp_vector2_t*> (src+offset_);
      *dst_obj = *src_obj;
}

تهی property_logic::set_vec4(نویسه*buf, ثابت vvp_vector4_t&val)
{
      vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      *obj = val;
}

تهی property_logic::get_vec4(نویسه*buf, vvp_vector4_t&val)
{
      ثابت vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      val = *obj;
}

تهی property_logic::copy(نویسه*dst, نویسه*src)
{
      vvp_vector4_t*dst_obj = reinterpret_cast<vvp_vector4_t*> (dst+offset_);
      ثابت vvp_vector4_t*src_obj = reinterpret_cast<vvp_vector4_t*> (src+offset_);
      *dst_obj = *src_obj;
}

template <رده T> تهی property_real<T>::set_real(نویسه*buf, دوگانه val)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      *tmp = val;
}

template <رده T> دوگانه property_real<T>::get_real(نویسه*buf)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      بازگشت *tmp;
}

template <رده T> تهی property_real<T>::copy(نویسه*dst, نویسه*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

تهی property_string::set_string(نویسه*buf, ثابت string&val)
{
      string*tmp = reinterpret_cast<string*>(buf+offset_);
      *tmp = val;
}

string property_string::get_string(نویسه*buf)
{
      ثابت string*tmp = reinterpret_cast<string*>(buf+offset_);
      بازگشت *tmp;
}

تهی property_string::copy(نویسه*dst, نویسه*src)
{
      string*dst_obj = reinterpret_cast<string*> (dst+offset_);
      ثابت string*src_obj = reinterpret_cast<string*> (src+offset_);
      *dst_obj = *src_obj;
}

تهی property_object::construct(نویسه*buf) ثابت
{
      برای (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    جدید (buf+offset_ + idx*اندازه(vvp_object_t)) vvp_object_t;
}

تهی property_object::destruct(نویسه*buf) ثابت
{
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*> (buf+offset_);
      برای (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    (tmp+idx)->~vvp_object_t();
}

تهی property_object::set_object(نویسه*buf, ثابت vvp_object_t&val, uint64_t idx)
{
      أكد(idx < array_size_);
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      tmp[idx] = val;
}

تهی property_object::get_object(نویسه*buf, vvp_object_t&val, uint64_t idx)
{
      أكد(idx < array_size_);
      ثابت vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      val = tmp[idx];
}

تهی property_object::copy(نویسه*dst, نویسه*src)
{
      vvp_object_t*dst_obj = reinterpret_cast<vvp_object_t*>(dst+offset_);
      ثابت vvp_object_t*src_obj = reinterpret_cast<vvp_object_t*>(src+offset_);
      برای (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    dst_obj[idx] = src_obj[idx];
}

/* **** */

class_type::class_type(ثابت string&nam, size_t nprop)
: class_name_(nam), properties_(nprop)
{
      instance_size_ = 0;
}

class_type::~class_type()
{
      برای (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    delete properties_[idx].type;
}

تهی class_type::set_property(size_t idx, ثابت string&name, ثابت string&type, uint64_t array_size)
{
      أكد(idx < properties_.size());
      properties_[idx].name = name;

      اگر (type == "b8")
	    properties_[idx].type = جدید property_atom<uint8_t>;
      وگرنه اگر (type == "b16")
	    properties_[idx].type = جدید property_atom<uint16_t>;
      وگرنه اگر (type == "b32")
	    properties_[idx].type = جدید property_atom<uint32_t>;
      وگرنه اگر (type == "b64")
	    properties_[idx].type = جدید property_atom<uint64_t>;
      وگرنه اگر (type == "sb8")
	    properties_[idx].type = جدید property_atom<int8_t>;
      وگرنه اگر (type == "sb16")
	    properties_[idx].type = جدید property_atom<int16_t>;
      وگرنه اگر (type == "sb32")
	    properties_[idx].type = جدید property_atom<int32_t>;
      وگرنه اگر (type == "sb64")
	    properties_[idx].type = جدید property_atom<int64_t>;
      وگرنه اگر (type == "r")
	    properties_[idx].type = جدید property_real<دوگانه>;
      وگرنه اگر (type == "S")
	    properties_[idx].type = جدید property_string;
      وگرنه اگر (type == "o")
	    properties_[idx].type = جدید property_object(array_size);
      وگرنه اگر (type[0] == 'b') {
	    size_t wid = strtoul(type.c_str()+1, 0, 0);
	    properties_[idx].type = جدید property_bit(wid);
      } وگرنه اگر (type[0] == 'L') {
	    size_t wid = strtoul(type.c_str()+1,0,0);
	    properties_[idx].type = جدید property_logic(wid);
      } وگرنه اگر (type[0] == 's' && type[1] == 'L') {
	    size_t wid = strtoul(type.c_str()+2,0,0);
	    properties_[idx].type = جدید property_logic(wid);
      } وگرنه {
	    properties_[idx].type = 0;
      }
}

تهی class_type::finish_setup(تهی)
{
      map<size_t, vector<size_t> > size_map;
	// Add up all the sizes to get a total instance size. This
	// figures out how much memory a complete instance will need.
      size_t accum = 0;
      برای (size_t idx = 0 ; idx < properties_.size() ; idx += 1) {
	    أكد(properties_[idx].type);
	    size_t instance_size = properties_[idx].type->instance_size();
	    accum += instance_size;
	    size_map[instance_size].push_back(idx);
      }

      instance_size_ = accum;

	// Now allocate the properties to offsets within an instance
	// space. Allocate the properties largest objects first so
	// that they are assured better alignment.
      accum = 0;
      برای (map<size_t, vector<size_t> >::reverse_iterator cur = size_map.rbegin()
		 ; cur != size_map.rend() ; ++ cur) {
	    برای (size_t idx = 0 ; idx < cur->second.size() ; idx += 1) {
		  size_t pid = cur->second[idx];
		  class_property_t*ptype = properties_[pid].type;
		  أكد(ptype->instance_size() == cur->first);
		  ptype->set_offset(accum);
		  accum += cur->first;
	    }
      }
}

class_type::inst_t class_type::instance_new() ثابت
{
      نویسه*buf = جدید نویسه [instance_size_];

      برای (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->construct(buf);

      بازگشت reinterpret_cast<inst_t> (buf);
}

تهی class_type::instance_delete(class_type::inst_t obj) ثابت
{
      نویسه*buf = reinterpret_cast<نویسه*> (obj);

      برای (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->destruct(buf);

      delete[]buf;
}

تهی class_type::set_vec4(class_type::inst_t obj, size_t pid,
			  ثابت vvp_vector4_t&val) ثابت
{
      نویسه*buf = reinterpret_cast<نویسه*> (obj);
      أكد(pid < properties_.size());
      properties_[pid].type->set_vec4(buf, val);
}

تهی class_type::get_vec4(class_type::inst_t obj, size_t pid,
			  vvp_vector4_t&val) ثابت
{
      نویسه*buf = reinterpret_cast<نویسه*> (obj);
      أكد(pid < properties_.size());
      properties_[pid].type->get_vec4(buf, val);
}

تهی class_type::set_real(class_type::inst_t obj, size_t pid,
			  دوگانه val) ثابت
{
      نویسه*buf = reinterpret_cast<نویسه*> (obj);
      أكد(pid < properties_.size());
      properties_[pid].type->set_real(buf, val);
}

دوگانه class_type::get_real(class_type::inst_t obj, size_t pid) ثابت
{
      نویسه*buf = reinterpret_cast<نویسه*> (obj);
      أكد(pid < properties_.size());
      بازگشت properties_[pid].type->get_real(buf);
}

تهی class_type::set_string(class_type::inst_t obj, size_t pid,
			    ثابت string&val) ثابت
{
      نویسه*buf = reinterpret_cast<نویسه*> (obj);
      أكد(pid < properties_.size());
      properties_[pid].type->set_string(buf, val);
}

string class_type::get_string(class_type::inst_t obj, size_t pid) ثابت
{
      نویسه*buf = reinterpret_cast<نویسه*> (obj);
      أكد(pid < properties_.size());
      بازگشت properties_[pid].type->get_string(buf);
}

تهی class_type::set_object(class_type::inst_t obj, size_t pid,
			    ثابت vvp_object_t&val, size_t idx) ثابت
{
      نویسه*buf = reinterpret_cast<نویسه*> (obj);
      أكد(pid < properties_.size());
      properties_[pid].type->set_object(buf, val, idx);
}

تهی class_type::get_object(class_type::inst_t obj, size_t pid,
			    vvp_object_t&val, size_t idx) ثابت
{
      نویسه*buf = reinterpret_cast<نویسه*> (obj);
      أكد(pid < properties_.size());
      properties_[pid].type->get_object(buf, val, idx);
}

تهی class_type::copy_property(class_type::inst_t dst, size_t pid, class_type::inst_t src) ثابت
{
      نویسه*dst_buf = reinterpret_cast<نویسه*> (dst);
      نویسه*src_buf = reinterpret_cast<نویسه*> (src);

      أكد(pid < properties_.size());

      properties_[pid].type->copy(dst_buf, src_buf);
}

صحیح class_type::get_type_code(تهی) ثابت
{
      بازگشت vpiClassDefn;
}

ایستا class_type*compile_class = 0;

تهی compile_class_start(نویسه*lab, نویسه*nam, unsigned ntype)
{
      أكد(compile_class == 0);
      compile_class = جدید class_type(nam, ntype);
      compile_vpi_نماد(lab, compile_class);
      آزاد(lab);
      delete[]nam;
}

تهی compile_class_property(unsigned idx, نویسه*nam, نویسه*typ, uint64_t array_size)
{
      أكد(compile_class);
      compile_class->set_property(idx, nam, typ, array_size);
      delete[]nam;
      delete[]typ;
}

تهی compile_class_done(تهی)
{
      __vpiScope*scope = vpip_peek_current_scope();
      أكد(scope);
      أكد(compile_class);
      compile_class->finish_setup();
      scope->classes[compile_class->class_name()] = compile_class;
      compile_class = 0;
}

#ifdef CHECK_WITH_VALGRIND
تهی class_def_delete(class_type *item)
{
      delete item;
}
#endif
