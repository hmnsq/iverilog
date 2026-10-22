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

using नामाकाश std;

/*
 * This class_property_t class is an abstract base class for
 * representing a property of an instance. The definition keeps and
 * array (of pointers) of these in order to define the the class.
 */
वर्ग class_property_t {
    सार्वजनिक:
      अन्तःस्थ class_property_t() { offset_ = 0; }
      आभास ~class_property_t() =0;
	// How much space does an instance of this property require?
      आभास size_t instance_size() ध्रुव =0;

      शून्य set_offset(size_t off) { offset_ = off; }

    सार्वजनिक:
      आभास शून्य construct(वर्ण*buf) ध्रुव;
      आभास शून्य destruct(वर्ण*buf) ध्रुव;

      आभास शून्य set_vec4(वर्ण*buf, ध्रुव vvp_vector4_t&val);
      आभास शून्य get_vec4(वर्ण*buf, vvp_vector4_t&val);

      आभास शून्य set_real(वर्ण*buf, द्विगुण val);
      आभास द्विगुण get_real(वर्ण*buf);

      आभास शून्य set_string(वर्ण*buf, ध्रुव std::string&val);
      आभास string get_string(वर्ण*buf);

      आभास शून्य set_object(वर्ण*buf, ध्रुव vvp_object_t&val, uint64_t element);
      आभास शून्य get_object(वर्ण*buf, vvp_object_t&val, uint64_t element);

	// Implement polymorphic shallow copy.
      आभास शून्य copy(वर्ण*buf, वर्ण*src) = 0;

    रक्षित:
      size_t offset_;
};

class_property_t::~class_property_t()
{
}

शून्य class_property_t::construct(वर्ण*) ध्रुव
{
}

शून्य class_property_t::destruct(वर्ण*) ध्रुव
{
}

शून्य class_property_t::set_vec4(वर्ण*, ध्रुव vvp_vector4_t&)
{
      प्रतिज्ञा(0);
}

शून्य class_property_t::get_vec4(वर्ण*, vvp_vector4_t&)
{
      प्रतिज्ञा(0);
}

शून्य class_property_t::set_real(वर्ण*, द्विगुण)
{
      प्रतिज्ञा(0);
}

द्विगुण class_property_t::get_real(वर्ण*)
{
      प्रतिज्ञा(0);
      प्रत्यागम 0.0;
}

शून्य class_property_t::set_string(वर्ण*, ध्रुव string&)
{
      प्रतिज्ञा(0);
}

string class_property_t::get_string(वर्ण*)
{
      प्रतिज्ञा(0);
      प्रत्यागम "";
}

शून्य class_property_t::set_object(वर्ण*, ध्रुव vvp_object_t&, uint64_t)
{
      प्रतिज्ञा(0);
}

शून्य class_property_t::get_object(वर्ण*, vvp_object_t&, uint64_t)
{
      प्रतिज्ञा(0);
}

/*
 */
template <वर्ग T> वर्ग property_atom : सार्वजनिक class_property_t {
    सार्वजनिक:
      अन्तःस्थ explicit property_atom(शून्य) { }
      ~property_atom() override { }

      size_t instance_size() ध्रुव override { प्रत्यागम परिमाण(T); }

    सार्वजनिक:
      शून्य construct(वर्ण*buf) ध्रुव override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0;
      }

      शून्य set_vec4(वर्ण*buf, ध्रुव vvp_vector4_t&val) override;
      शून्य get_vec4(वर्ण*buf, vvp_vector4_t&val) override;

      शून्य copy(वर्ण*dst, वर्ण*src) override;
};

वर्ग property_bit : सार्वजनिक class_property_t {
    सार्वजनिक:
      explicit अन्तःस्थ property_bit(size_t wid): wid_(wid) { }
      ~property_bit() override { }

      size_t instance_size() ध्रुव override { प्रत्यागम परिमाण(vvp_vector2_t); }

    सार्वजनिक:
      शून्य construct(वर्ण*buf) ध्रुव override
      { नूतन (buf+offset_) vvp_vector2_t (0, wid_); }

      शून्य destruct(वर्ण*buf) ध्रुव override
      { vvp_vector2_t*tmp = reinterpret_cast<vvp_vector2_t*>(buf+offset_);
	tmp->~vvp_vector2_t();
      }

      शून्य set_vec4(वर्ण*buf, ध्रुव vvp_vector4_t&val) override;
      शून्य get_vec4(वर्ण*buf, vvp_vector4_t&val) override;

      शून्य copy(वर्ण*dst, वर्ण*src) override;

    गुप्त:
      size_t wid_;
};

वर्ग property_logic : सार्वजनिक class_property_t {
    सार्वजनिक:
      explicit अन्तःस्थ property_logic(size_t wid): wid_(wid) { }
      ~property_logic() override { }

      size_t instance_size() ध्रुव override { प्रत्यागम परिमाण(vvp_vector4_t); }

    सार्वजनिक:
      शून्य construct(वर्ण*buf) ध्रुव override
      { नूतन (buf+offset_) vvp_vector4_t (wid_); }

      शून्य destruct(वर्ण*buf) ध्रुव override
      { vvp_vector4_t*tmp = reinterpret_cast<vvp_vector4_t*>(buf+offset_);
	tmp->~vvp_vector4_t();
      }

      शून्य set_vec4(वर्ण*buf, ध्रुव vvp_vector4_t&val) override;
      शून्य get_vec4(वर्ण*buf, vvp_vector4_t&val) override;

      शून्य copy(वर्ण*dst, वर्ण*src) override;

    गुप्त:
      size_t wid_;
};

template <वर्ग T> वर्ग property_real : सार्वजनिक class_property_t {
    सार्वजनिक:
      अन्तःस्थ explicit property_real(शून्य) { }
      ~property_real() override { }

      size_t instance_size() ध्रुव override { प्रत्यागम परिमाण(T); }

    सार्वजनिक:
      शून्य construct(वर्ण*buf) ध्रुव override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0.0;
      }

      शून्य set_real(वर्ण*buf, द्विगुण val) override;
      द्विगुण get_real(वर्ण*buf) override;

      शून्य copy(वर्ण*dst, वर्ण*src) override;
};

वर्ग property_string : सार्वजनिक class_property_t {
    सार्वजनिक:
      अन्तःस्थ explicit property_string(शून्य) { }
      ~property_string() override { }

      size_t instance_size() ध्रुव override { प्रत्यागम परिमाण(std::string); }

    सार्वजनिक:
      शून्य construct(वर्ण*buf) ध्रुव override
      { /* string*tmp = */ नूतन (buf+offset_) string; }

      शून्य destruct(वर्ण*buf) ध्रुव override
      { string*tmp = reinterpret_cast<string*> (buf+offset_);
	tmp->~string();
      }

      शून्य set_string(वर्ण*buf, ध्रुव string&) override;
      string get_string(वर्ण*buf) override;

      शून्य copy(वर्ण*dst, वर्ण*src) override;
};

वर्ग property_object : सार्वजनिक class_property_t {
    सार्वजनिक:
      अन्तःस्थ explicit property_object(uint64_t as): array_size_(as==0? 1 : as) { }
      ~property_object() override { }

      size_t instance_size() ध्रुव override { प्रत्यागम array_size_ * परिमाण(vvp_object_t); }

    सार्वजनिक:
      शून्य construct(वर्ण*buf) ध्रुव override;

      शून्य destruct(वर्ण*buf) ध्रुव override;

      शून्य set_object(वर्ण*buf, ध्रुव vvp_object_t&, uint64_t) override;
      शून्य get_object(वर्ण*buf, vvp_object_t&, uint64_t) override;

      शून्य copy(वर्ण*dst, वर्ण*src) override;

    गुप्त:
      size_t array_size_;
};

template <वर्ग T> शून्य property_atom<T>::set_vec4(वर्ण*buf, ध्रुव vvp_vector4_t&val)
{
      T*tmp = reinterpret_cast<T*> (buf+offset_);
      तार्किक flag = vector4_to_मानम्(val, *tmp, सच्च, मिच्छ);
      प्रतिज्ञा(flag);
}

template <वर्ग T> शून्य property_atom<T>::get_vec4(वर्ण*buf, vvp_vector4_t&val)
{
      T*src = reinterpret_cast<T*> (buf+offset_);
      ध्रुव size_t tmp_cnt = परिमाण(T)<परिमाण(unsigned long)
				       ? 1
				       : परिमाण(T) / परिमाण(unsigned long);
      unsigned long tmp[tmp_cnt];
      tmp[0] = src[0];

      प्रति (size_t idx = 1 ; idx < tmp_cnt ; idx += 1)
	    tmp[idx] = src[0] >> idx * 8 * परिमाण(tmp[0]);

      val.resize(8*परिमाण(T));
      val.setarray(0, val.size(), tmp);
}

template <वर्ग T> शून्य property_atom<T>::copy(वर्ण*dst, वर्ण*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

शून्य property_bit::set_vec4(वर्ण*buf, ध्रुव vvp_vector4_t&val)
{
      vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      *obj = val;
}

शून्य property_bit::get_vec4(वर्ण*buf, vvp_vector4_t&val)
{
      ध्रुव vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      val = vector2_to_vector4(*obj, obj->size());
}

शून्य property_bit::copy(वर्ण*dst, वर्ण*src)
{
      vvp_vector2_t*dst_obj = reinterpret_cast<vvp_vector2_t*> (dst+offset_);
      ध्रुव vvp_vector2_t*src_obj = reinterpret_cast<vvp_vector2_t*> (src+offset_);
      *dst_obj = *src_obj;
}

शून्य property_logic::set_vec4(वर्ण*buf, ध्रुव vvp_vector4_t&val)
{
      vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      *obj = val;
}

शून्य property_logic::get_vec4(वर्ण*buf, vvp_vector4_t&val)
{
      ध्रुव vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      val = *obj;
}

शून्य property_logic::copy(वर्ण*dst, वर्ण*src)
{
      vvp_vector4_t*dst_obj = reinterpret_cast<vvp_vector4_t*> (dst+offset_);
      ध्रुव vvp_vector4_t*src_obj = reinterpret_cast<vvp_vector4_t*> (src+offset_);
      *dst_obj = *src_obj;
}

template <वर्ग T> शून्य property_real<T>::set_real(वर्ण*buf, द्विगुण val)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      *tmp = val;
}

template <वर्ग T> द्विगुण property_real<T>::get_real(वर्ण*buf)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      प्रत्यागम *tmp;
}

template <वर्ग T> शून्य property_real<T>::copy(वर्ण*dst, वर्ण*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

शून्य property_string::set_string(वर्ण*buf, ध्रुव string&val)
{
      string*tmp = reinterpret_cast<string*>(buf+offset_);
      *tmp = val;
}

string property_string::get_string(वर्ण*buf)
{
      ध्रुव string*tmp = reinterpret_cast<string*>(buf+offset_);
      प्रत्यागम *tmp;
}

शून्य property_string::copy(वर्ण*dst, वर्ण*src)
{
      string*dst_obj = reinterpret_cast<string*> (dst+offset_);
      ध्रुव string*src_obj = reinterpret_cast<string*> (src+offset_);
      *dst_obj = *src_obj;
}

शून्य property_object::construct(वर्ण*buf) ध्रुव
{
      प्रति (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    नूतन (buf+offset_ + idx*परिमाण(vvp_object_t)) vvp_object_t;
}

शून्य property_object::destruct(वर्ण*buf) ध्रुव
{
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*> (buf+offset_);
      प्रति (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    (tmp+idx)->~vvp_object_t();
}

शून्य property_object::set_object(वर्ण*buf, ध्रुव vvp_object_t&val, uint64_t idx)
{
      प्रतिज्ञा(idx < array_size_);
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      tmp[idx] = val;
}

शून्य property_object::get_object(वर्ण*buf, vvp_object_t&val, uint64_t idx)
{
      प्रतिज्ञा(idx < array_size_);
      ध्रुव vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      val = tmp[idx];
}

शून्य property_object::copy(वर्ण*dst, वर्ण*src)
{
      vvp_object_t*dst_obj = reinterpret_cast<vvp_object_t*>(dst+offset_);
      ध्रुव vvp_object_t*src_obj = reinterpret_cast<vvp_object_t*>(src+offset_);
      प्रति (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    dst_obj[idx] = src_obj[idx];
}

/* **** */

class_type::class_type(ध्रुव string&nam, size_t nprop)
: class_name_(nam), properties_(nprop)
{
      instance_size_ = 0;
}

class_type::~class_type()
{
      प्रति (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    delete properties_[idx].type;
}

शून्य class_type::set_property(size_t idx, ध्रुव string&name, ध्रुव string&type, uint64_t array_size)
{
      प्रतिज्ञा(idx < properties_.size());
      properties_[idx].name = name;

      जइ (type == "b8")
	    properties_[idx].type = नूतन property_atom<uint8_t>;
      अण्णहा जइ (type == "b16")
	    properties_[idx].type = नूतन property_atom<uint16_t>;
      अण्णहा जइ (type == "b32")
	    properties_[idx].type = नूतन property_atom<uint32_t>;
      अण्णहा जइ (type == "b64")
	    properties_[idx].type = नूतन property_atom<uint64_t>;
      अण्णहा जइ (type == "sb8")
	    properties_[idx].type = नूतन property_atom<int8_t>;
      अण्णहा जइ (type == "sb16")
	    properties_[idx].type = नूतन property_atom<int16_t>;
      अण्णहा जइ (type == "sb32")
	    properties_[idx].type = नूतन property_atom<int32_t>;
      अण्णहा जइ (type == "sb64")
	    properties_[idx].type = नूतन property_atom<int64_t>;
      अण्णहा जइ (type == "r")
	    properties_[idx].type = नूतन property_real<द्विगुण>;
      अण्णहा जइ (type == "S")
	    properties_[idx].type = नूतन property_string;
      अण्णहा जइ (type == "o")
	    properties_[idx].type = नूतन property_object(array_size);
      अण्णहा जइ (type[0] == 'b') {
	    size_t wid = strtoul(type.c_str()+1, 0, 0);
	    properties_[idx].type = नूतन property_bit(wid);
      } अण्णहा जइ (type[0] == 'L') {
	    size_t wid = strtoul(type.c_str()+1,0,0);
	    properties_[idx].type = नूतन property_logic(wid);
      } अण्णहा जइ (type[0] == 's' && type[1] == 'L') {
	    size_t wid = strtoul(type.c_str()+2,0,0);
	    properties_[idx].type = नूतन property_logic(wid);
      } अण्णहा {
	    properties_[idx].type = 0;
      }
}

शून्य class_type::finish_setup(शून्य)
{
      map<size_t, vector<size_t> > size_map;
	// Add up all the sizes to get a total instance size. This
	// figures out how much memory a complete instance will need.
      size_t accum = 0;
      प्रति (size_t idx = 0 ; idx < properties_.size() ; idx += 1) {
	    प्रतिज्ञा(properties_[idx].type);
	    size_t instance_size = properties_[idx].type->instance_size();
	    accum += instance_size;
	    size_map[instance_size].push_back(idx);
      }

      instance_size_ = accum;

	// Now allocate the properties to offsets within an instance
	// space. Allocate the properties largest objects first so
	// that they are assured better alignment.
      accum = 0;
      प्रति (map<size_t, vector<size_t> >::reverse_iterator cur = size_map.rbegin()
		 ; cur != size_map.rend() ; ++ cur) {
	    प्रति (size_t idx = 0 ; idx < cur->second.size() ; idx += 1) {
		  size_t pid = cur->second[idx];
		  class_property_t*ptype = properties_[pid].type;
		  प्रतिज्ञा(ptype->instance_size() == cur->first);
		  ptype->set_offset(accum);
		  accum += cur->first;
	    }
      }
}

class_type::inst_t class_type::instance_new() ध्रुव
{
      वर्ण*buf = नूतन वर्ण [instance_size_];

      प्रति (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->construct(buf);

      प्रत्यागम reinterpret_cast<inst_t> (buf);
}

शून्य class_type::instance_delete(class_type::inst_t obj) ध्रुव
{
      वर्ण*buf = reinterpret_cast<वर्ण*> (obj);

      प्रति (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->destruct(buf);

      delete[]buf;
}

शून्य class_type::set_vec4(class_type::inst_t obj, size_t pid,
			  ध्रुव vvp_vector4_t&val) ध्रुव
{
      वर्ण*buf = reinterpret_cast<वर्ण*> (obj);
      प्रतिज्ञा(pid < properties_.size());
      properties_[pid].type->set_vec4(buf, val);
}

शून्य class_type::get_vec4(class_type::inst_t obj, size_t pid,
			  vvp_vector4_t&val) ध्रुव
{
      वर्ण*buf = reinterpret_cast<वर्ण*> (obj);
      प्रतिज्ञा(pid < properties_.size());
      properties_[pid].type->get_vec4(buf, val);
}

शून्य class_type::set_real(class_type::inst_t obj, size_t pid,
			  द्विगुण val) ध्रुव
{
      वर्ण*buf = reinterpret_cast<वर्ण*> (obj);
      प्रतिज्ञा(pid < properties_.size());
      properties_[pid].type->set_real(buf, val);
}

द्विगुण class_type::get_real(class_type::inst_t obj, size_t pid) ध्रुव
{
      वर्ण*buf = reinterpret_cast<वर्ण*> (obj);
      प्रतिज्ञा(pid < properties_.size());
      प्रत्यागम properties_[pid].type->get_real(buf);
}

शून्य class_type::set_string(class_type::inst_t obj, size_t pid,
			    ध्रुव string&val) ध्रुव
{
      वर्ण*buf = reinterpret_cast<वर्ण*> (obj);
      प्रतिज्ञा(pid < properties_.size());
      properties_[pid].type->set_string(buf, val);
}

string class_type::get_string(class_type::inst_t obj, size_t pid) ध्रुव
{
      वर्ण*buf = reinterpret_cast<वर्ण*> (obj);
      प्रतिज्ञा(pid < properties_.size());
      प्रत्यागम properties_[pid].type->get_string(buf);
}

शून्य class_type::set_object(class_type::inst_t obj, size_t pid,
			    ध्रुव vvp_object_t&val, size_t idx) ध्रुव
{
      वर्ण*buf = reinterpret_cast<वर्ण*> (obj);
      प्रतिज्ञा(pid < properties_.size());
      properties_[pid].type->set_object(buf, val, idx);
}

शून्य class_type::get_object(class_type::inst_t obj, size_t pid,
			    vvp_object_t&val, size_t idx) ध्रुव
{
      वर्ण*buf = reinterpret_cast<वर्ण*> (obj);
      प्रतिज्ञा(pid < properties_.size());
      properties_[pid].type->get_object(buf, val, idx);
}

शून्य class_type::copy_property(class_type::inst_t dst, size_t pid, class_type::inst_t src) ध्रुव
{
      वर्ण*dst_buf = reinterpret_cast<वर्ण*> (dst);
      वर्ण*src_buf = reinterpret_cast<वर्ण*> (src);

      प्रतिज्ञा(pid < properties_.size());

      properties_[pid].type->copy(dst_buf, src_buf);
}

पूर्णाङ्क class_type::get_type_code(शून्य) ध्रुव
{
      प्रत्यागम vpiClassDefn;
}

स्थिर class_type*compile_class = 0;

शून्य compile_class_start(वर्ण*lab, वर्ण*nam, unsigned ntype)
{
      प्रतिज्ञा(compile_class == 0);
      compile_class = नूतन class_type(nam, ntype);
      compile_vpi_चिह्नम्(lab, compile_class);
      मुक्त(lab);
      delete[]nam;
}

शून्य compile_class_property(unsigned idx, वर्ण*nam, वर्ण*typ, uint64_t array_size)
{
      प्रतिज्ञा(compile_class);
      compile_class->set_property(idx, nam, typ, array_size);
      delete[]nam;
      delete[]typ;
}

शून्य compile_class_done(शून्य)
{
      __vpiScope*scope = vpip_peek_current_scope();
      प्रतिज्ञा(scope);
      प्रतिज्ञा(compile_class);
      compile_class->finish_setup();
      scope->classes[compile_class->class_name()] = compile_class;
      compile_class = 0;
}

#ifdef CHECK_WITH_VALGRIND
शून्य class_def_delete(class_type *item)
{
      delete item;
}
#endif
