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

using નામાકાશ std;

/*
 * This class_property_t class is an abstract base class for
 * representing a property of an instance. The definition keeps and
 * array (of pointers) of these in order to define the the class.
 */
વર્ગ class_property_t {
    જાહેર:
      અંતઃરેખ class_property_t() { offset_ = 0; }
      આભાસી ~class_property_t() =0;
	// How much space does an instance of this property require?
      આભાસી size_t instance_size() અચલ =0;

      શૂન્ય set_offset(size_t off) { offset_ = off; }

    જાહેર:
      આભાસી શૂન્ય construct(અક્ષર*buf) અચલ;
      આભાસી શૂન્ય destruct(અક્ષર*buf) અચલ;

      આભાસી શૂન્ય set_vec4(અક્ષર*buf, અચલ vvp_vector4_t&val);
      આભાસી શૂન્ય get_vec4(અક્ષર*buf, vvp_vector4_t&val);

      આભાસી શૂન્ય set_real(અક્ષર*buf, દ્વિગુણ val);
      આભાસી દ્વિગુણ get_real(અક્ષર*buf);

      આભાસી શૂન્ય set_string(અક્ષર*buf, અચલ std::string&val);
      આભાસી string get_string(અક્ષર*buf);

      આભાસી શૂન્ય set_object(અક્ષર*buf, અચલ vvp_object_t&val, uint64_t element);
      આભાસી શૂન્ય get_object(અક્ષર*buf, vvp_object_t&val, uint64_t element);

	// Implement polymorphic shallow copy.
      આભાસી શૂન્ય copy(અક્ષર*buf, અક્ષર*src) = 0;

    સુરક્ષિત:
      size_t offset_;
};

class_property_t::~class_property_t()
{
}

શૂન્ય class_property_t::construct(અક્ષર*) અચલ
{
}

શૂન્ય class_property_t::destruct(અક્ષર*) અચલ
{
}

શૂન્ય class_property_t::set_vec4(અક્ષર*, અચલ vvp_vector4_t&)
{
      પ્રતિજ્ઞા(0);
}

શૂન્ય class_property_t::get_vec4(અક્ષર*, vvp_vector4_t&)
{
      પ્રતિજ્ઞા(0);
}

શૂન્ય class_property_t::set_real(અક્ષર*, દ્વિગુણ)
{
      પ્રતિજ્ઞા(0);
}

દ્વિગુણ class_property_t::get_real(અક્ષર*)
{
      પ્રતિજ્ઞા(0);
      પાછા 0.0;
}

શૂન્ય class_property_t::set_string(અક્ષર*, અચલ string&)
{
      પ્રતિજ્ઞા(0);
}

string class_property_t::get_string(અક્ષર*)
{
      પ્રતિજ્ઞા(0);
      પાછા "";
}

શૂન્ય class_property_t::set_object(અક્ષર*, અચલ vvp_object_t&, uint64_t)
{
      પ્રતિજ્ઞા(0);
}

શૂન્ય class_property_t::get_object(અક્ષર*, vvp_object_t&, uint64_t)
{
      પ્રતિજ્ઞા(0);
}

/*
 */
template <વર્ગ T> વર્ગ property_atom : જાહેર class_property_t {
    જાહેર:
      અંતઃરેખ explicit property_atom(શૂન્ય) { }
      ~property_atom() override { }

      size_t instance_size() અચલ override { પાછા કદ(T); }

    જાહેર:
      શૂન્ય construct(અક્ષર*buf) અચલ override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0;
      }

      શૂન્ય set_vec4(અક્ષર*buf, અચલ vvp_vector4_t&val) override;
      શૂન્ય get_vec4(અક્ષર*buf, vvp_vector4_t&val) override;

      શૂન્ય copy(અક્ષર*dst, અક્ષર*src) override;
};

વર્ગ property_bit : જાહેર class_property_t {
    જાહેર:
      explicit અંતઃરેખ property_bit(size_t wid): wid_(wid) { }
      ~property_bit() override { }

      size_t instance_size() અચલ override { પાછા કદ(vvp_vector2_t); }

    જાહેર:
      શૂન્ય construct(અક્ષર*buf) અચલ override
      { નવું (buf+offset_) vvp_vector2_t (0, wid_); }

      શૂન્ય destruct(અક્ષર*buf) અચલ override
      { vvp_vector2_t*tmp = reinterpret_cast<vvp_vector2_t*>(buf+offset_);
	tmp->~vvp_vector2_t();
      }

      શૂન્ય set_vec4(અક્ષર*buf, અચલ vvp_vector4_t&val) override;
      શૂન્ય get_vec4(અક્ષર*buf, vvp_vector4_t&val) override;

      શૂન્ય copy(અક્ષર*dst, અક્ષર*src) override;

    ખાનગી:
      size_t wid_;
};

વર્ગ property_logic : જાહેર class_property_t {
    જાહેર:
      explicit અંતઃરેખ property_logic(size_t wid): wid_(wid) { }
      ~property_logic() override { }

      size_t instance_size() અચલ override { પાછા કદ(vvp_vector4_t); }

    જાહેર:
      શૂન્ય construct(અક્ષર*buf) અચલ override
      { નવું (buf+offset_) vvp_vector4_t (wid_); }

      શૂન્ય destruct(અક્ષર*buf) અચલ override
      { vvp_vector4_t*tmp = reinterpret_cast<vvp_vector4_t*>(buf+offset_);
	tmp->~vvp_vector4_t();
      }

      શૂન્ય set_vec4(અક્ષર*buf, અચલ vvp_vector4_t&val) override;
      શૂન્ય get_vec4(અક્ષર*buf, vvp_vector4_t&val) override;

      શૂન્ય copy(અક્ષર*dst, અક્ષર*src) override;

    ખાનગી:
      size_t wid_;
};

template <વર્ગ T> વર્ગ property_real : જાહેર class_property_t {
    જાહેર:
      અંતઃરેખ explicit property_real(શૂન્ય) { }
      ~property_real() override { }

      size_t instance_size() અચલ override { પાછા કદ(T); }

    જાહેર:
      શૂન્ય construct(અક્ષર*buf) અચલ override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0.0;
      }

      શૂન્ય set_real(અક્ષર*buf, દ્વિગુણ val) override;
      દ્વિગુણ get_real(અક્ષર*buf) override;

      શૂન્ય copy(અક્ષર*dst, અક્ષર*src) override;
};

વર્ગ property_string : જાહેર class_property_t {
    જાહેર:
      અંતઃરેખ explicit property_string(શૂન્ય) { }
      ~property_string() override { }

      size_t instance_size() અચલ override { પાછા કદ(std::string); }

    જાહેર:
      શૂન્ય construct(અક્ષર*buf) અચલ override
      { /* string*tmp = */ નવું (buf+offset_) string; }

      શૂન્ય destruct(અક્ષર*buf) અચલ override
      { string*tmp = reinterpret_cast<string*> (buf+offset_);
	tmp->~string();
      }

      શૂન્ય set_string(અક્ષર*buf, અચલ string&) override;
      string get_string(અક્ષર*buf) override;

      શૂન્ય copy(અક્ષર*dst, અક્ષર*src) override;
};

વર્ગ property_object : જાહેર class_property_t {
    જાહેર:
      અંતઃરેખ explicit property_object(uint64_t as): array_size_(as==0? 1 : as) { }
      ~property_object() override { }

      size_t instance_size() અચલ override { પાછા array_size_ * કદ(vvp_object_t); }

    જાહેર:
      શૂન્ય construct(અક્ષર*buf) અચલ override;

      શૂન્ય destruct(અક્ષર*buf) અચલ override;

      શૂન્ય set_object(અક્ષર*buf, અચલ vvp_object_t&, uint64_t) override;
      શૂન્ય get_object(અક્ષર*buf, vvp_object_t&, uint64_t) override;

      શૂન્ય copy(અક્ષર*dst, અક્ષર*src) override;

    ખાનગી:
      size_t array_size_;
};

template <વર્ગ T> શૂન્ય property_atom<T>::set_vec4(અક્ષર*buf, અચલ vvp_vector4_t&val)
{
      T*tmp = reinterpret_cast<T*> (buf+offset_);
      તાર્કિક flag = vector4_to_કિંમત(val, *tmp, સાચું, ખોટું);
      પ્રતિજ્ઞા(flag);
}

template <વર્ગ T> શૂન્ય property_atom<T>::get_vec4(અક્ષર*buf, vvp_vector4_t&val)
{
      T*src = reinterpret_cast<T*> (buf+offset_);
      અચલ size_t tmp_cnt = કદ(T)<કદ(unsigned long)
				       ? 1
				       : કદ(T) / કદ(unsigned long);
      unsigned long tmp[tmp_cnt];
      tmp[0] = src[0];

      માટે (size_t idx = 1 ; idx < tmp_cnt ; idx += 1)
	    tmp[idx] = src[0] >> idx * 8 * કદ(tmp[0]);

      val.resize(8*કદ(T));
      val.setarray(0, val.size(), tmp);
}

template <વર્ગ T> શૂન્ય property_atom<T>::copy(અક્ષર*dst, અક્ષર*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

શૂન્ય property_bit::set_vec4(અક્ષર*buf, અચલ vvp_vector4_t&val)
{
      vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      *obj = val;
}

શૂન્ય property_bit::get_vec4(અક્ષર*buf, vvp_vector4_t&val)
{
      અચલ vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      val = vector2_to_vector4(*obj, obj->size());
}

શૂન્ય property_bit::copy(અક્ષર*dst, અક્ષર*src)
{
      vvp_vector2_t*dst_obj = reinterpret_cast<vvp_vector2_t*> (dst+offset_);
      અચલ vvp_vector2_t*src_obj = reinterpret_cast<vvp_vector2_t*> (src+offset_);
      *dst_obj = *src_obj;
}

શૂન્ય property_logic::set_vec4(અક્ષર*buf, અચલ vvp_vector4_t&val)
{
      vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      *obj = val;
}

શૂન્ય property_logic::get_vec4(અક્ષર*buf, vvp_vector4_t&val)
{
      અચલ vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      val = *obj;
}

શૂન્ય property_logic::copy(અક્ષર*dst, અક્ષર*src)
{
      vvp_vector4_t*dst_obj = reinterpret_cast<vvp_vector4_t*> (dst+offset_);
      અચલ vvp_vector4_t*src_obj = reinterpret_cast<vvp_vector4_t*> (src+offset_);
      *dst_obj = *src_obj;
}

template <વર્ગ T> શૂન્ય property_real<T>::set_real(અક્ષર*buf, દ્વિગુણ val)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      *tmp = val;
}

template <વર્ગ T> દ્વિગુણ property_real<T>::get_real(અક્ષર*buf)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      પાછા *tmp;
}

template <વર્ગ T> શૂન્ય property_real<T>::copy(અક્ષર*dst, અક્ષર*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

શૂન્ય property_string::set_string(અક્ષર*buf, અચલ string&val)
{
      string*tmp = reinterpret_cast<string*>(buf+offset_);
      *tmp = val;
}

string property_string::get_string(અક્ષર*buf)
{
      અચલ string*tmp = reinterpret_cast<string*>(buf+offset_);
      પાછા *tmp;
}

શૂન્ય property_string::copy(અક્ષર*dst, અક્ષર*src)
{
      string*dst_obj = reinterpret_cast<string*> (dst+offset_);
      અચલ string*src_obj = reinterpret_cast<string*> (src+offset_);
      *dst_obj = *src_obj;
}

શૂન્ય property_object::construct(અક્ષર*buf) અચલ
{
      માટે (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    નવું (buf+offset_ + idx*કદ(vvp_object_t)) vvp_object_t;
}

શૂન્ય property_object::destruct(અક્ષર*buf) અચલ
{
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*> (buf+offset_);
      માટે (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    (tmp+idx)->~vvp_object_t();
}

શૂન્ય property_object::set_object(અક્ષર*buf, અચલ vvp_object_t&val, uint64_t idx)
{
      પ્રતિજ્ઞા(idx < array_size_);
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      tmp[idx] = val;
}

શૂન્ય property_object::get_object(અક્ષર*buf, vvp_object_t&val, uint64_t idx)
{
      પ્રતિજ્ઞા(idx < array_size_);
      અચલ vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      val = tmp[idx];
}

શૂન્ય property_object::copy(અક્ષર*dst, અક્ષર*src)
{
      vvp_object_t*dst_obj = reinterpret_cast<vvp_object_t*>(dst+offset_);
      અચલ vvp_object_t*src_obj = reinterpret_cast<vvp_object_t*>(src+offset_);
      માટે (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    dst_obj[idx] = src_obj[idx];
}

/* **** */

class_type::class_type(અચલ string&nam, size_t nprop)
: class_name_(nam), properties_(nprop)
{
      instance_size_ = 0;
}

class_type::~class_type()
{
      માટે (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    delete properties_[idx].type;
}

શૂન્ય class_type::set_property(size_t idx, અચલ string&name, અચલ string&type, uint64_t array_size)
{
      પ્રતિજ્ઞા(idx < properties_.size());
      properties_[idx].name = name;

      જો (type == "b8")
	    properties_[idx].type = નવું property_atom<uint8_t>;
      નહીંતર જો (type == "b16")
	    properties_[idx].type = નવું property_atom<uint16_t>;
      નહીંતર જો (type == "b32")
	    properties_[idx].type = નવું property_atom<uint32_t>;
      નહીંતર જો (type == "b64")
	    properties_[idx].type = નવું property_atom<uint64_t>;
      નહીંતર જો (type == "sb8")
	    properties_[idx].type = નવું property_atom<int8_t>;
      નહીંતર જો (type == "sb16")
	    properties_[idx].type = નવું property_atom<int16_t>;
      નહીંતર જો (type == "sb32")
	    properties_[idx].type = નવું property_atom<int32_t>;
      નહીંતર જો (type == "sb64")
	    properties_[idx].type = નવું property_atom<int64_t>;
      નહીંતર જો (type == "r")
	    properties_[idx].type = નવું property_real<દ્વિગુણ>;
      નહીંતર જો (type == "S")
	    properties_[idx].type = નવું property_string;
      નહીંતર જો (type == "o")
	    properties_[idx].type = નવું property_object(array_size);
      નહીંતર જો (type[0] == 'b') {
	    size_t wid = strtoul(type.c_str()+1, 0, 0);
	    properties_[idx].type = નવું property_bit(wid);
      } નહીંતર જો (type[0] == 'L') {
	    size_t wid = strtoul(type.c_str()+1,0,0);
	    properties_[idx].type = નવું property_logic(wid);
      } નહીંતર જો (type[0] == 's' && type[1] == 'L') {
	    size_t wid = strtoul(type.c_str()+2,0,0);
	    properties_[idx].type = નવું property_logic(wid);
      } નહીંતર {
	    properties_[idx].type = 0;
      }
}

શૂન્ય class_type::finish_setup(શૂન્ય)
{
      map<size_t, vector<size_t> > size_map;
	// Add up all the sizes to get a total instance size. This
	// figures out how much memory a complete instance will need.
      size_t accum = 0;
      માટે (size_t idx = 0 ; idx < properties_.size() ; idx += 1) {
	    પ્રતિજ્ઞા(properties_[idx].type);
	    size_t instance_size = properties_[idx].type->instance_size();
	    accum += instance_size;
	    size_map[instance_size].push_back(idx);
      }

      instance_size_ = accum;

	// Now allocate the properties to offsets within an instance
	// space. Allocate the properties largest objects first so
	// that they are assured better alignment.
      accum = 0;
      માટે (map<size_t, vector<size_t> >::reverse_iterator cur = size_map.rbegin()
		 ; cur != size_map.rend() ; ++ cur) {
	    માટે (size_t idx = 0 ; idx < cur->second.size() ; idx += 1) {
		  size_t pid = cur->second[idx];
		  class_property_t*ptype = properties_[pid].type;
		  પ્રતિજ્ઞા(ptype->instance_size() == cur->first);
		  ptype->set_offset(accum);
		  accum += cur->first;
	    }
      }
}

class_type::inst_t class_type::instance_new() અચલ
{
      અક્ષર*buf = નવું અક્ષર [instance_size_];

      માટે (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->construct(buf);

      પાછા reinterpret_cast<inst_t> (buf);
}

શૂન્ય class_type::instance_delete(class_type::inst_t obj) અચલ
{
      અક્ષર*buf = reinterpret_cast<અક્ષર*> (obj);

      માટે (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->destruct(buf);

      delete[]buf;
}

શૂન્ય class_type::set_vec4(class_type::inst_t obj, size_t pid,
			  અચલ vvp_vector4_t&val) અચલ
{
      અક્ષર*buf = reinterpret_cast<અક્ષર*> (obj);
      પ્રતિજ્ઞા(pid < properties_.size());
      properties_[pid].type->set_vec4(buf, val);
}

શૂન્ય class_type::get_vec4(class_type::inst_t obj, size_t pid,
			  vvp_vector4_t&val) અચલ
{
      અક્ષર*buf = reinterpret_cast<અક્ષર*> (obj);
      પ્રતિજ્ઞા(pid < properties_.size());
      properties_[pid].type->get_vec4(buf, val);
}

શૂન્ય class_type::set_real(class_type::inst_t obj, size_t pid,
			  દ્વિગુણ val) અચલ
{
      અક્ષર*buf = reinterpret_cast<અક્ષર*> (obj);
      પ્રતિજ્ઞા(pid < properties_.size());
      properties_[pid].type->set_real(buf, val);
}

દ્વિગુણ class_type::get_real(class_type::inst_t obj, size_t pid) અચલ
{
      અક્ષર*buf = reinterpret_cast<અક્ષર*> (obj);
      પ્રતિજ્ઞા(pid < properties_.size());
      પાછા properties_[pid].type->get_real(buf);
}

શૂન્ય class_type::set_string(class_type::inst_t obj, size_t pid,
			    અચલ string&val) અચલ
{
      અક્ષર*buf = reinterpret_cast<અક્ષર*> (obj);
      પ્રતિજ્ઞા(pid < properties_.size());
      properties_[pid].type->set_string(buf, val);
}

string class_type::get_string(class_type::inst_t obj, size_t pid) અચલ
{
      અક્ષર*buf = reinterpret_cast<અક્ષર*> (obj);
      પ્રતિજ્ઞા(pid < properties_.size());
      પાછા properties_[pid].type->get_string(buf);
}

શૂન્ય class_type::set_object(class_type::inst_t obj, size_t pid,
			    અચલ vvp_object_t&val, size_t idx) અચલ
{
      અક્ષર*buf = reinterpret_cast<અક્ષર*> (obj);
      પ્રતિજ્ઞા(pid < properties_.size());
      properties_[pid].type->set_object(buf, val, idx);
}

શૂન્ય class_type::get_object(class_type::inst_t obj, size_t pid,
			    vvp_object_t&val, size_t idx) અચલ
{
      અક્ષર*buf = reinterpret_cast<અક્ષર*> (obj);
      પ્રતિજ્ઞા(pid < properties_.size());
      properties_[pid].type->get_object(buf, val, idx);
}

શૂન્ય class_type::copy_property(class_type::inst_t dst, size_t pid, class_type::inst_t src) અચલ
{
      અક્ષર*dst_buf = reinterpret_cast<અક્ષર*> (dst);
      અક્ષર*src_buf = reinterpret_cast<અક્ષર*> (src);

      પ્રતિજ્ઞા(pid < properties_.size());

      properties_[pid].type->copy(dst_buf, src_buf);
}

પૂર્ણાંક class_type::get_type_code(શૂન્ય) અચલ
{
      પાછા vpiClassDefn;
}

સ્થિર class_type*compile_class = 0;

શૂન્ય compile_class_start(અક્ષર*lab, અક્ષર*nam, unsigned ntype)
{
      પ્રતિજ્ઞા(compile_class == 0);
      compile_class = નવું class_type(nam, ntype);
      compile_vpi_પ્રતીક(lab, compile_class);
      મુક્ત(lab);
      delete[]nam;
}

શૂન્ય compile_class_property(unsigned idx, અક્ષર*nam, અક્ષર*typ, uint64_t array_size)
{
      પ્રતિજ્ઞા(compile_class);
      compile_class->set_property(idx, nam, typ, array_size);
      delete[]nam;
      delete[]typ;
}

શૂન્ય compile_class_done(શૂન્ય)
{
      __vpiScope*scope = vpip_peek_current_scope();
      પ્રતિજ્ઞા(scope);
      પ્રતિજ્ઞા(compile_class);
      compile_class->finish_setup();
      scope->classes[compile_class->class_name()] = compile_class;
      compile_class = 0;
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય class_def_delete(class_type *item)
{
      delete item;
}
#endif
