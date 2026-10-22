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

using நாமாகாஶ std;

/*
 * This class_property_t class is an abstract base class for
 * representing a property of an instance. The definition keeps and
 * array (of pointers) of these in order to define the the class.
 */
வகுப்பு class_property_t {
    பொது:
      உள்வரி class_property_t() { offset_ = 0; }
      மெய்நிகர் ~class_property_t() =0;
	// How much space does an instance of this property require?
      மெய்நிகர் size_t instance_size() மாறிலி =0;

      வெறுமை set_offset(size_t off) { offset_ = off; }

    பொது:
      மெய்நிகர் வெறுமை construct(எழுத்து*buf) மாறிலி;
      மெய்நிகர் வெறுமை destruct(எழுத்து*buf) மாறிலி;

      மெய்நிகர் வெறுமை set_vec4(எழுத்து*buf, மாறிலி vvp_vector4_t&val);
      மெய்நிகர் வெறுமை get_vec4(எழுத்து*buf, vvp_vector4_t&val);

      மெய்நிகர் வெறுமை set_real(எழுத்து*buf, இரட்டை val);
      மெய்நிகர் இரட்டை get_real(எழுத்து*buf);

      மெய்நிகர் வெறுமை set_string(எழுத்து*buf, மாறிலி std::string&val);
      மெய்நிகர் string get_string(எழுத்து*buf);

      மெய்நிகர் வெறுமை set_object(எழுத்து*buf, மாறிலி vvp_object_t&val, uint64_t element);
      மெய்நிகர் வெறுமை get_object(எழுத்து*buf, vvp_object_t&val, uint64_t element);

	// Implement polymorphic shallow copy.
      மெய்நிகர் வெறுமை copy(எழுத்து*buf, எழுத்து*src) = 0;

    பாதுகாக்கப்பட்ட:
      size_t offset_;
};

class_property_t::~class_property_t()
{
}

வெறுமை class_property_t::construct(எழுத்து*) மாறிலி
{
}

வெறுமை class_property_t::destruct(எழுத்து*) மாறிலி
{
}

வெறுமை class_property_t::set_vec4(எழுத்து*, மாறிலி vvp_vector4_t&)
{
      ப்ரதிஜ்ஞா(0);
}

வெறுமை class_property_t::get_vec4(எழுத்து*, vvp_vector4_t&)
{
      ப்ரதிஜ்ஞா(0);
}

வெறுமை class_property_t::set_real(எழுத்து*, இரட்டை)
{
      ப்ரதிஜ்ஞா(0);
}

இரட்டை class_property_t::get_real(எழுத்து*)
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு 0.0;
}

வெறுமை class_property_t::set_string(எழுத்து*, மாறிலி string&)
{
      ப்ரதிஜ்ஞா(0);
}

string class_property_t::get_string(எழுத்து*)
{
      ப்ரதிஜ்ஞா(0);
      திரும்பு "";
}

வெறுமை class_property_t::set_object(எழுத்து*, மாறிலி vvp_object_t&, uint64_t)
{
      ப்ரதிஜ்ஞா(0);
}

வெறுமை class_property_t::get_object(எழுத்து*, vvp_object_t&, uint64_t)
{
      ப்ரதிஜ்ஞா(0);
}

/*
 */
template <வகுப்பு T> வகுப்பு property_atom : பொது class_property_t {
    பொது:
      உள்வரி explicit property_atom(வெறுமை) { }
      ~property_atom() override { }

      size_t instance_size() மாறிலி override { திரும்பு அளவு(T); }

    பொது:
      வெறுமை construct(எழுத்து*buf) மாறிலி override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0;
      }

      வெறுமை set_vec4(எழுத்து*buf, மாறிலி vvp_vector4_t&val) override;
      வெறுமை get_vec4(எழுத்து*buf, vvp_vector4_t&val) override;

      வெறுமை copy(எழுத்து*dst, எழுத்து*src) override;
};

வகுப்பு property_bit : பொது class_property_t {
    பொது:
      explicit உள்வரி property_bit(size_t wid): wid_(wid) { }
      ~property_bit() override { }

      size_t instance_size() மாறிலி override { திரும்பு அளவு(vvp_vector2_t); }

    பொது:
      வெறுமை construct(எழுத்து*buf) மாறிலி override
      { புதிய (buf+offset_) vvp_vector2_t (0, wid_); }

      வெறுமை destruct(எழுத்து*buf) மாறிலி override
      { vvp_vector2_t*tmp = reinterpret_cast<vvp_vector2_t*>(buf+offset_);
	tmp->~vvp_vector2_t();
      }

      வெறுமை set_vec4(எழுத்து*buf, மாறிலி vvp_vector4_t&val) override;
      வெறுமை get_vec4(எழுத்து*buf, vvp_vector4_t&val) override;

      வெறுமை copy(எழுத்து*dst, எழுத்து*src) override;

    தனிப்பட்ட:
      size_t wid_;
};

வகுப்பு property_logic : பொது class_property_t {
    பொது:
      explicit உள்வரி property_logic(size_t wid): wid_(wid) { }
      ~property_logic() override { }

      size_t instance_size() மாறிலி override { திரும்பு அளவு(vvp_vector4_t); }

    பொது:
      வெறுமை construct(எழுத்து*buf) மாறிலி override
      { புதிய (buf+offset_) vvp_vector4_t (wid_); }

      வெறுமை destruct(எழுத்து*buf) மாறிலி override
      { vvp_vector4_t*tmp = reinterpret_cast<vvp_vector4_t*>(buf+offset_);
	tmp->~vvp_vector4_t();
      }

      வெறுமை set_vec4(எழுத்து*buf, மாறிலி vvp_vector4_t&val) override;
      வெறுமை get_vec4(எழுத்து*buf, vvp_vector4_t&val) override;

      வெறுமை copy(எழுத்து*dst, எழுத்து*src) override;

    தனிப்பட்ட:
      size_t wid_;
};

template <வகுப்பு T> வகுப்பு property_real : பொது class_property_t {
    பொது:
      உள்வரி explicit property_real(வெறுமை) { }
      ~property_real() override { }

      size_t instance_size() மாறிலி override { திரும்பு அளவு(T); }

    பொது:
      வெறுமை construct(எழுத்து*buf) மாறிலி override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0.0;
      }

      வெறுமை set_real(எழுத்து*buf, இரட்டை val) override;
      இரட்டை get_real(எழுத்து*buf) override;

      வெறுமை copy(எழுத்து*dst, எழுத்து*src) override;
};

வகுப்பு property_string : பொது class_property_t {
    பொது:
      உள்வரி explicit property_string(வெறுமை) { }
      ~property_string() override { }

      size_t instance_size() மாறிலி override { திரும்பு அளவு(std::string); }

    பொது:
      வெறுமை construct(எழுத்து*buf) மாறிலி override
      { /* string*tmp = */ புதிய (buf+offset_) string; }

      வெறுமை destruct(எழுத்து*buf) மாறிலி override
      { string*tmp = reinterpret_cast<string*> (buf+offset_);
	tmp->~string();
      }

      வெறுமை set_string(எழுத்து*buf, மாறிலி string&) override;
      string get_string(எழுத்து*buf) override;

      வெறுமை copy(எழுத்து*dst, எழுத்து*src) override;
};

வகுப்பு property_object : பொது class_property_t {
    பொது:
      உள்வரி explicit property_object(uint64_t as): array_size_(as==0? 1 : as) { }
      ~property_object() override { }

      size_t instance_size() மாறிலி override { திரும்பு array_size_ * அளவு(vvp_object_t); }

    பொது:
      வெறுமை construct(எழுத்து*buf) மாறிலி override;

      வெறுமை destruct(எழுத்து*buf) மாறிலி override;

      வெறுமை set_object(எழுத்து*buf, மாறிலி vvp_object_t&, uint64_t) override;
      வெறுமை get_object(எழுத்து*buf, vvp_object_t&, uint64_t) override;

      வெறுமை copy(எழுத்து*dst, எழுத்து*src) override;

    தனிப்பட்ட:
      size_t array_size_;
};

template <வகுப்பு T> வெறுமை property_atom<T>::set_vec4(எழுத்து*buf, மாறிலி vvp_vector4_t&val)
{
      T*tmp = reinterpret_cast<T*> (buf+offset_);
      தருக்க flag = vector4_to_மதிப்பு(val, *tmp, உண்மை, பொய்);
      ப்ரதிஜ்ஞா(flag);
}

template <வகுப்பு T> வெறுமை property_atom<T>::get_vec4(எழுத்து*buf, vvp_vector4_t&val)
{
      T*src = reinterpret_cast<T*> (buf+offset_);
      மாறிலி size_t tmp_cnt = அளவு(T)<அளவு(unsigned long)
				       ? 1
				       : அளவு(T) / அளவு(unsigned long);
      unsigned long tmp[tmp_cnt];
      tmp[0] = src[0];

      க்கு (size_t idx = 1 ; idx < tmp_cnt ; idx += 1)
	    tmp[idx] = src[0] >> idx * 8 * அளவு(tmp[0]);

      val.resize(8*அளவு(T));
      val.setarray(0, val.size(), tmp);
}

template <வகுப்பு T> வெறுமை property_atom<T>::copy(எழுத்து*dst, எழுத்து*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

வெறுமை property_bit::set_vec4(எழுத்து*buf, மாறிலி vvp_vector4_t&val)
{
      vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      *obj = val;
}

வெறுமை property_bit::get_vec4(எழுத்து*buf, vvp_vector4_t&val)
{
      மாறிலி vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      val = vector2_to_vector4(*obj, obj->size());
}

வெறுமை property_bit::copy(எழுத்து*dst, எழுத்து*src)
{
      vvp_vector2_t*dst_obj = reinterpret_cast<vvp_vector2_t*> (dst+offset_);
      மாறிலி vvp_vector2_t*src_obj = reinterpret_cast<vvp_vector2_t*> (src+offset_);
      *dst_obj = *src_obj;
}

வெறுமை property_logic::set_vec4(எழுத்து*buf, மாறிலி vvp_vector4_t&val)
{
      vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      *obj = val;
}

வெறுமை property_logic::get_vec4(எழுத்து*buf, vvp_vector4_t&val)
{
      மாறிலி vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      val = *obj;
}

வெறுமை property_logic::copy(எழுத்து*dst, எழுத்து*src)
{
      vvp_vector4_t*dst_obj = reinterpret_cast<vvp_vector4_t*> (dst+offset_);
      மாறிலி vvp_vector4_t*src_obj = reinterpret_cast<vvp_vector4_t*> (src+offset_);
      *dst_obj = *src_obj;
}

template <வகுப்பு T> வெறுமை property_real<T>::set_real(எழுத்து*buf, இரட்டை val)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      *tmp = val;
}

template <வகுப்பு T> இரட்டை property_real<T>::get_real(எழுத்து*buf)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      திரும்பு *tmp;
}

template <வகுப்பு T> வெறுமை property_real<T>::copy(எழுத்து*dst, எழுத்து*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

வெறுமை property_string::set_string(எழுத்து*buf, மாறிலி string&val)
{
      string*tmp = reinterpret_cast<string*>(buf+offset_);
      *tmp = val;
}

string property_string::get_string(எழுத்து*buf)
{
      மாறிலி string*tmp = reinterpret_cast<string*>(buf+offset_);
      திரும்பு *tmp;
}

வெறுமை property_string::copy(எழுத்து*dst, எழுத்து*src)
{
      string*dst_obj = reinterpret_cast<string*> (dst+offset_);
      மாறிலி string*src_obj = reinterpret_cast<string*> (src+offset_);
      *dst_obj = *src_obj;
}

வெறுமை property_object::construct(எழுத்து*buf) மாறிலி
{
      க்கு (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    புதிய (buf+offset_ + idx*அளவு(vvp_object_t)) vvp_object_t;
}

வெறுமை property_object::destruct(எழுத்து*buf) மாறிலி
{
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*> (buf+offset_);
      க்கு (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    (tmp+idx)->~vvp_object_t();
}

வெறுமை property_object::set_object(எழுத்து*buf, மாறிலி vvp_object_t&val, uint64_t idx)
{
      ப்ரதிஜ்ஞா(idx < array_size_);
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      tmp[idx] = val;
}

வெறுமை property_object::get_object(எழுத்து*buf, vvp_object_t&val, uint64_t idx)
{
      ப்ரதிஜ்ஞா(idx < array_size_);
      மாறிலி vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      val = tmp[idx];
}

வெறுமை property_object::copy(எழுத்து*dst, எழுத்து*src)
{
      vvp_object_t*dst_obj = reinterpret_cast<vvp_object_t*>(dst+offset_);
      மாறிலி vvp_object_t*src_obj = reinterpret_cast<vvp_object_t*>(src+offset_);
      க்கு (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    dst_obj[idx] = src_obj[idx];
}

/* **** */

class_type::class_type(மாறிலி string&nam, size_t nprop)
: class_name_(nam), properties_(nprop)
{
      instance_size_ = 0;
}

class_type::~class_type()
{
      க்கு (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    delete properties_[idx].type;
}

வெறுமை class_type::set_property(size_t idx, மாறிலி string&name, மாறிலி string&type, uint64_t array_size)
{
      ப்ரதிஜ்ஞா(idx < properties_.size());
      properties_[idx].name = name;

      எனில் (type == "b8")
	    properties_[idx].type = புதிய property_atom<uint8_t>;
      இல்லையெனில் எனில் (type == "b16")
	    properties_[idx].type = புதிய property_atom<uint16_t>;
      இல்லையெனில் எனில் (type == "b32")
	    properties_[idx].type = புதிய property_atom<uint32_t>;
      இல்லையெனில் எனில் (type == "b64")
	    properties_[idx].type = புதிய property_atom<uint64_t>;
      இல்லையெனில் எனில் (type == "sb8")
	    properties_[idx].type = புதிய property_atom<int8_t>;
      இல்லையெனில் எனில் (type == "sb16")
	    properties_[idx].type = புதிய property_atom<int16_t>;
      இல்லையெனில் எனில் (type == "sb32")
	    properties_[idx].type = புதிய property_atom<int32_t>;
      இல்லையெனில் எனில் (type == "sb64")
	    properties_[idx].type = புதிய property_atom<int64_t>;
      இல்லையெனில் எனில் (type == "r")
	    properties_[idx].type = புதிய property_real<இரட்டை>;
      இல்லையெனில் எனில் (type == "S")
	    properties_[idx].type = புதிய property_string;
      இல்லையெனில் எனில் (type == "o")
	    properties_[idx].type = புதிய property_object(array_size);
      இல்லையெனில் எனில் (type[0] == 'b') {
	    size_t wid = strtoul(type.c_str()+1, 0, 0);
	    properties_[idx].type = புதிய property_bit(wid);
      } இல்லையெனில் எனில் (type[0] == 'L') {
	    size_t wid = strtoul(type.c_str()+1,0,0);
	    properties_[idx].type = புதிய property_logic(wid);
      } இல்லையெனில் எனில் (type[0] == 's' && type[1] == 'L') {
	    size_t wid = strtoul(type.c_str()+2,0,0);
	    properties_[idx].type = புதிய property_logic(wid);
      } இல்லையெனில் {
	    properties_[idx].type = 0;
      }
}

வெறுமை class_type::finish_setup(வெறுமை)
{
      map<size_t, vector<size_t> > size_map;
	// Add up all the sizes to get a total instance size. This
	// figures out how much memory a complete instance will need.
      size_t accum = 0;
      க்கு (size_t idx = 0 ; idx < properties_.size() ; idx += 1) {
	    ப்ரதிஜ்ஞா(properties_[idx].type);
	    size_t instance_size = properties_[idx].type->instance_size();
	    accum += instance_size;
	    size_map[instance_size].push_back(idx);
      }

      instance_size_ = accum;

	// Now allocate the properties to offsets within an instance
	// space. Allocate the properties largest objects first so
	// that they are assured better alignment.
      accum = 0;
      க்கு (map<size_t, vector<size_t> >::reverse_iterator cur = size_map.rbegin()
		 ; cur != size_map.rend() ; ++ cur) {
	    க்கு (size_t idx = 0 ; idx < cur->second.size() ; idx += 1) {
		  size_t pid = cur->second[idx];
		  class_property_t*ptype = properties_[pid].type;
		  ப்ரதிஜ்ஞா(ptype->instance_size() == cur->first);
		  ptype->set_offset(accum);
		  accum += cur->first;
	    }
      }
}

class_type::inst_t class_type::instance_new() மாறிலி
{
      எழுத்து*buf = புதிய எழுத்து [instance_size_];

      க்கு (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->construct(buf);

      திரும்பு reinterpret_cast<inst_t> (buf);
}

வெறுமை class_type::instance_delete(class_type::inst_t obj) மாறிலி
{
      எழுத்து*buf = reinterpret_cast<எழுத்து*> (obj);

      க்கு (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->destruct(buf);

      delete[]buf;
}

வெறுமை class_type::set_vec4(class_type::inst_t obj, size_t pid,
			  மாறிலி vvp_vector4_t&val) மாறிலி
{
      எழுத்து*buf = reinterpret_cast<எழுத்து*> (obj);
      ப்ரதிஜ்ஞா(pid < properties_.size());
      properties_[pid].type->set_vec4(buf, val);
}

வெறுமை class_type::get_vec4(class_type::inst_t obj, size_t pid,
			  vvp_vector4_t&val) மாறிலி
{
      எழுத்து*buf = reinterpret_cast<எழுத்து*> (obj);
      ப்ரதிஜ்ஞா(pid < properties_.size());
      properties_[pid].type->get_vec4(buf, val);
}

வெறுமை class_type::set_real(class_type::inst_t obj, size_t pid,
			  இரட்டை val) மாறிலி
{
      எழுத்து*buf = reinterpret_cast<எழுத்து*> (obj);
      ப்ரதிஜ்ஞா(pid < properties_.size());
      properties_[pid].type->set_real(buf, val);
}

இரட்டை class_type::get_real(class_type::inst_t obj, size_t pid) மாறிலி
{
      எழுத்து*buf = reinterpret_cast<எழுத்து*> (obj);
      ப்ரதிஜ்ஞா(pid < properties_.size());
      திரும்பு properties_[pid].type->get_real(buf);
}

வெறுமை class_type::set_string(class_type::inst_t obj, size_t pid,
			    மாறிலி string&val) மாறிலி
{
      எழுத்து*buf = reinterpret_cast<எழுத்து*> (obj);
      ப்ரதிஜ்ஞா(pid < properties_.size());
      properties_[pid].type->set_string(buf, val);
}

string class_type::get_string(class_type::inst_t obj, size_t pid) மாறிலி
{
      எழுத்து*buf = reinterpret_cast<எழுத்து*> (obj);
      ப்ரதிஜ்ஞா(pid < properties_.size());
      திரும்பு properties_[pid].type->get_string(buf);
}

வெறுமை class_type::set_object(class_type::inst_t obj, size_t pid,
			    மாறிலி vvp_object_t&val, size_t idx) மாறிலி
{
      எழுத்து*buf = reinterpret_cast<எழுத்து*> (obj);
      ப்ரதிஜ்ஞா(pid < properties_.size());
      properties_[pid].type->set_object(buf, val, idx);
}

வெறுமை class_type::get_object(class_type::inst_t obj, size_t pid,
			    vvp_object_t&val, size_t idx) மாறிலி
{
      எழுத்து*buf = reinterpret_cast<எழுத்து*> (obj);
      ப்ரதிஜ்ஞா(pid < properties_.size());
      properties_[pid].type->get_object(buf, val, idx);
}

வெறுமை class_type::copy_property(class_type::inst_t dst, size_t pid, class_type::inst_t src) மாறிலி
{
      எழுத்து*dst_buf = reinterpret_cast<எழுத்து*> (dst);
      எழுத்து*src_buf = reinterpret_cast<எழுத்து*> (src);

      ப்ரதிஜ்ஞா(pid < properties_.size());

      properties_[pid].type->copy(dst_buf, src_buf);
}

முழு class_type::get_type_code(வெறுமை) மாறிலி
{
      திரும்பு vpiClassDefn;
}

நிலையான class_type*compile_class = 0;

வெறுமை compile_class_start(எழுத்து*lab, எழுத்து*nam, unsigned ntype)
{
      ப்ரதிஜ்ஞா(compile_class == 0);
      compile_class = புதிய class_type(nam, ntype);
      compile_vpi_குறியீடு(lab, compile_class);
      விடுவி(lab);
      delete[]nam;
}

வெறுமை compile_class_property(unsigned idx, எழுத்து*nam, எழுத்து*typ, uint64_t array_size)
{
      ப்ரதிஜ்ஞா(compile_class);
      compile_class->set_property(idx, nam, typ, array_size);
      delete[]nam;
      delete[]typ;
}

வெறுமை compile_class_done(வெறுமை)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ப்ரதிஜ்ஞா(scope);
      ப்ரதிஜ்ஞா(compile_class);
      compile_class->finish_setup();
      scope->classes[compile_class->class_name()] = compile_class;
      compile_class = 0;
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை class_def_delete(class_type *item)
{
      delete item;
}
#endif
