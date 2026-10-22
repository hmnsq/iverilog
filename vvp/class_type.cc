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

using ನಾಮಾಕಾಶ std;

/*
 * This class_property_t class is an abstract base class for
 * representing a property of an instance. The definition keeps and
 * array (of pointers) of these in order to define the the class.
 */
ವರ್ಗ class_property_t {
    ಸಾರ್ವಜನಿಕ:
      ಅನ್ತಃಸ್ಥ class_property_t() { offset_ = 0; }
      ಆಭಾಸ ~class_property_t() =0;
	// How much space does an instance of this property require?
      ಆಭಾಸ size_t instance_size() ಧ್ರುವ =0;

      ಶೂನ್ಯ set_offset(size_t off) { offset_ = off; }

    ಸಾರ್ವಜನಿಕ:
      ಆಭಾಸ ಶೂನ್ಯ construct(ವರ್ಣ*buf) ಧ್ರುವ;
      ಆಭಾಸ ಶೂನ್ಯ destruct(ವರ್ಣ*buf) ಧ್ರುವ;

      ಆಭಾಸ ಶೂನ್ಯ set_vec4(ವರ್ಣ*buf, ಧ್ರುವ vvp_vector4_t&val);
      ಆಭಾಸ ಶೂನ್ಯ get_vec4(ವರ್ಣ*buf, vvp_vector4_t&val);

      ಆಭಾಸ ಶೂನ್ಯ set_real(ವರ್ಣ*buf, ದ್ವಿಗುಣ val);
      ಆಭಾಸ ದ್ವಿಗುಣ get_real(ವರ್ಣ*buf);

      ಆಭಾಸ ಶೂನ್ಯ set_string(ವರ್ಣ*buf, ಧ್ರುವ std::string&val);
      ಆಭಾಸ string get_string(ವರ್ಣ*buf);

      ಆಭಾಸ ಶೂನ್ಯ set_object(ವರ್ಣ*buf, ಧ್ರುವ vvp_object_t&val, uint64_t element);
      ಆಭಾಸ ಶೂನ್ಯ get_object(ವರ್ಣ*buf, vvp_object_t&val, uint64_t element);

	// Implement polymorphic shallow copy.
      ಆಭಾಸ ಶೂನ್ಯ copy(ವರ್ಣ*buf, ವರ್ಣ*src) = 0;

    ರಕ್ಷಿತ:
      size_t offset_;
};

class_property_t::~class_property_t()
{
}

ಶೂನ್ಯ class_property_t::construct(ವರ್ಣ*) ಧ್ರುವ
{
}

ಶೂನ್ಯ class_property_t::destruct(ವರ್ಣ*) ಧ್ರುವ
{
}

ಶೂನ್ಯ class_property_t::set_vec4(ವರ್ಣ*, ಧ್ರುವ vvp_vector4_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ class_property_t::get_vec4(ವರ್ಣ*, vvp_vector4_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ class_property_t::set_real(ವರ್ಣ*, ದ್ವಿಗುಣ)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ದ್ವಿಗುಣ class_property_t::get_real(ವರ್ಣ*)
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ 0.0;
}

ಶೂನ್ಯ class_property_t::set_string(ವರ್ಣ*, ಧ್ರುವ string&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

string class_property_t::get_string(ವರ್ಣ*)
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ "";
}

ಶೂನ್ಯ class_property_t::set_object(ವರ್ಣ*, ಧ್ರುವ vvp_object_t&, uint64_t)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ class_property_t::get_object(ವರ್ಣ*, vvp_object_t&, uint64_t)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

/*
 */
template <ವರ್ಗ T> ವರ್ಗ property_atom : ಸಾರ್ವಜನಿಕ class_property_t {
    ಸಾರ್ವಜನಿಕ:
      ಅನ್ತಃಸ್ಥ explicit property_atom(ಶೂನ್ಯ) { }
      ~property_atom() override { }

      size_t instance_size() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(T); }

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ construct(ವರ್ಣ*buf) ಧ್ರುವ override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0;
      }

      ಶೂನ್ಯ set_vec4(ವರ್ಣ*buf, ಧ್ರುವ vvp_vector4_t&val) override;
      ಶೂನ್ಯ get_vec4(ವರ್ಣ*buf, vvp_vector4_t&val) override;

      ಶೂನ್ಯ copy(ವರ್ಣ*dst, ವರ್ಣ*src) override;
};

ವರ್ಗ property_bit : ಸಾರ್ವಜನಿಕ class_property_t {
    ಸಾರ್ವಜನಿಕ:
      explicit ಅನ್ತಃಸ್ಥ property_bit(size_t wid): wid_(wid) { }
      ~property_bit() override { }

      size_t instance_size() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(vvp_vector2_t); }

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ construct(ವರ್ಣ*buf) ಧ್ರುವ override
      { ನೂತನ (buf+offset_) vvp_vector2_t (0, wid_); }

      ಶೂನ್ಯ destruct(ವರ್ಣ*buf) ಧ್ರುವ override
      { vvp_vector2_t*tmp = reinterpret_cast<vvp_vector2_t*>(buf+offset_);
	tmp->~vvp_vector2_t();
      }

      ಶೂನ್ಯ set_vec4(ವರ್ಣ*buf, ಧ್ರುವ vvp_vector4_t&val) override;
      ಶೂನ್ಯ get_vec4(ವರ್ಣ*buf, vvp_vector4_t&val) override;

      ಶೂನ್ಯ copy(ವರ್ಣ*dst, ವರ್ಣ*src) override;

    ಗುಪ್ತ:
      size_t wid_;
};

ವರ್ಗ property_logic : ಸಾರ್ವಜನಿಕ class_property_t {
    ಸಾರ್ವಜನಿಕ:
      explicit ಅನ್ತಃಸ್ಥ property_logic(size_t wid): wid_(wid) { }
      ~property_logic() override { }

      size_t instance_size() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(vvp_vector4_t); }

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ construct(ವರ್ಣ*buf) ಧ್ರುವ override
      { ನೂತನ (buf+offset_) vvp_vector4_t (wid_); }

      ಶೂನ್ಯ destruct(ವರ್ಣ*buf) ಧ್ರುವ override
      { vvp_vector4_t*tmp = reinterpret_cast<vvp_vector4_t*>(buf+offset_);
	tmp->~vvp_vector4_t();
      }

      ಶೂನ್ಯ set_vec4(ವರ್ಣ*buf, ಧ್ರುವ vvp_vector4_t&val) override;
      ಶೂನ್ಯ get_vec4(ವರ್ಣ*buf, vvp_vector4_t&val) override;

      ಶೂನ್ಯ copy(ವರ್ಣ*dst, ವರ್ಣ*src) override;

    ಗುಪ್ತ:
      size_t wid_;
};

template <ವರ್ಗ T> ವರ್ಗ property_real : ಸಾರ್ವಜನಿಕ class_property_t {
    ಸಾರ್ವಜನಿಕ:
      ಅನ್ತಃಸ್ಥ explicit property_real(ಶೂನ್ಯ) { }
      ~property_real() override { }

      size_t instance_size() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(T); }

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ construct(ವರ್ಣ*buf) ಧ್ರುವ override
      { T*tmp = reinterpret_cast<T*> (buf+offset_);
	*tmp = 0.0;
      }

      ಶೂನ್ಯ set_real(ವರ್ಣ*buf, ದ್ವಿಗುಣ val) override;
      ದ್ವಿಗುಣ get_real(ವರ್ಣ*buf) override;

      ಶೂನ್ಯ copy(ವರ್ಣ*dst, ವರ್ಣ*src) override;
};

ವರ್ಗ property_string : ಸಾರ್ವಜನಿಕ class_property_t {
    ಸಾರ್ವಜನಿಕ:
      ಅನ್ತಃಸ್ಥ explicit property_string(ಶೂನ್ಯ) { }
      ~property_string() override { }

      size_t instance_size() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(std::string); }

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ construct(ವರ್ಣ*buf) ಧ್ರುವ override
      { /* string*tmp = */ ನೂತನ (buf+offset_) string; }

      ಶೂನ್ಯ destruct(ವರ್ಣ*buf) ಧ್ರುವ override
      { string*tmp = reinterpret_cast<string*> (buf+offset_);
	tmp->~string();
      }

      ಶೂನ್ಯ set_string(ವರ್ಣ*buf, ಧ್ರುವ string&) override;
      string get_string(ವರ್ಣ*buf) override;

      ಶೂನ್ಯ copy(ವರ್ಣ*dst, ವರ್ಣ*src) override;
};

ವರ್ಗ property_object : ಸಾರ್ವಜನಿಕ class_property_t {
    ಸಾರ್ವಜನಿಕ:
      ಅನ್ತಃಸ್ಥ explicit property_object(uint64_t as): array_size_(as==0? 1 : as) { }
      ~property_object() override { }

      size_t instance_size() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ array_size_ * ಪರಿಮಾಣ(vvp_object_t); }

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ construct(ವರ್ಣ*buf) ಧ್ರುವ override;

      ಶೂನ್ಯ destruct(ವರ್ಣ*buf) ಧ್ರುವ override;

      ಶೂನ್ಯ set_object(ವರ್ಣ*buf, ಧ್ರುವ vvp_object_t&, uint64_t) override;
      ಶೂನ್ಯ get_object(ವರ್ಣ*buf, vvp_object_t&, uint64_t) override;

      ಶೂನ್ಯ copy(ವರ್ಣ*dst, ವರ್ಣ*src) override;

    ಗುಪ್ತ:
      size_t array_size_;
};

template <ವರ್ಗ T> ಶೂನ್ಯ property_atom<T>::set_vec4(ವರ್ಣ*buf, ಧ್ರುವ vvp_vector4_t&val)
{
      T*tmp = reinterpret_cast<T*> (buf+offset_);
      ತಾರ್ಕಿಕ flag = vector4_to_ಮೌಲ್ಯ(val, *tmp, ಸತ್ಯ, ಅಸತ್ಯ);
      ಪ್ರತಿಜ್ಞಾ(flag);
}

template <ವರ್ಗ T> ಶೂನ್ಯ property_atom<T>::get_vec4(ವರ್ಣ*buf, vvp_vector4_t&val)
{
      T*src = reinterpret_cast<T*> (buf+offset_);
      ಧ್ರುವ size_t tmp_cnt = ಪರಿಮಾಣ(T)<ಪರಿಮಾಣ(unsigned long)
				       ? 1
				       : ಪರಿಮಾಣ(T) / ಪರಿಮಾಣ(unsigned long);
      unsigned long tmp[tmp_cnt];
      tmp[0] = src[0];

      ಪ್ರತಿ (size_t idx = 1 ; idx < tmp_cnt ; idx += 1)
	    tmp[idx] = src[0] >> idx * 8 * ಪರಿಮಾಣ(tmp[0]);

      val.resize(8*ಪರಿಮಾಣ(T));
      val.setarray(0, val.size(), tmp);
}

template <ವರ್ಗ T> ಶೂನ್ಯ property_atom<T>::copy(ವರ್ಣ*dst, ವರ್ಣ*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

ಶೂನ್ಯ property_bit::set_vec4(ವರ್ಣ*buf, ಧ್ರುವ vvp_vector4_t&val)
{
      vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      *obj = val;
}

ಶೂನ್ಯ property_bit::get_vec4(ವರ್ಣ*buf, vvp_vector4_t&val)
{
      ಧ್ರುವ vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+offset_);
      val = vector2_to_vector4(*obj, obj->size());
}

ಶೂನ್ಯ property_bit::copy(ವರ್ಣ*dst, ವರ್ಣ*src)
{
      vvp_vector2_t*dst_obj = reinterpret_cast<vvp_vector2_t*> (dst+offset_);
      ಧ್ರುವ vvp_vector2_t*src_obj = reinterpret_cast<vvp_vector2_t*> (src+offset_);
      *dst_obj = *src_obj;
}

ಶೂನ್ಯ property_logic::set_vec4(ವರ್ಣ*buf, ಧ್ರುವ vvp_vector4_t&val)
{
      vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      *obj = val;
}

ಶೂನ್ಯ property_logic::get_vec4(ವರ್ಣ*buf, vvp_vector4_t&val)
{
      ಧ್ರುವ vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+offset_);
      val = *obj;
}

ಶೂನ್ಯ property_logic::copy(ವರ್ಣ*dst, ವರ್ಣ*src)
{
      vvp_vector4_t*dst_obj = reinterpret_cast<vvp_vector4_t*> (dst+offset_);
      ಧ್ರುವ vvp_vector4_t*src_obj = reinterpret_cast<vvp_vector4_t*> (src+offset_);
      *dst_obj = *src_obj;
}

template <ವರ್ಗ T> ಶೂನ್ಯ property_real<T>::set_real(ವರ್ಣ*buf, ದ್ವಿಗುಣ val)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      *tmp = val;
}

template <ವರ್ಗ T> ದ್ವಿಗುಣ property_real<T>::get_real(ವರ್ಣ*buf)
{
      T*tmp = reinterpret_cast<T*>(buf+offset_);
      ಪ್ರತ್ಯಾಗಮ *tmp;
}

template <ವರ್ಗ T> ಶೂನ್ಯ property_real<T>::copy(ವರ್ಣ*dst, ವರ್ಣ*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+offset_);
      T*src_obj = reinterpret_cast<T*> (src+offset_);
      *dst_obj = *src_obj;
}

ಶೂನ್ಯ property_string::set_string(ವರ್ಣ*buf, ಧ್ರುವ string&val)
{
      string*tmp = reinterpret_cast<string*>(buf+offset_);
      *tmp = val;
}

string property_string::get_string(ವರ್ಣ*buf)
{
      ಧ್ರುವ string*tmp = reinterpret_cast<string*>(buf+offset_);
      ಪ್ರತ್ಯಾಗಮ *tmp;
}

ಶೂನ್ಯ property_string::copy(ವರ್ಣ*dst, ವರ್ಣ*src)
{
      string*dst_obj = reinterpret_cast<string*> (dst+offset_);
      ಧ್ರುವ string*src_obj = reinterpret_cast<string*> (src+offset_);
      *dst_obj = *src_obj;
}

ಶೂನ್ಯ property_object::construct(ವರ್ಣ*buf) ಧ್ರುವ
{
      ಪ್ರತಿ (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    ನೂತನ (buf+offset_ + idx*ಪರಿಮಾಣ(vvp_object_t)) vvp_object_t;
}

ಶೂನ್ಯ property_object::destruct(ವರ್ಣ*buf) ಧ್ರುವ
{
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*> (buf+offset_);
      ಪ್ರತಿ (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    (tmp+idx)->~vvp_object_t();
}

ಶೂನ್ಯ property_object::set_object(ವರ್ಣ*buf, ಧ್ರುವ vvp_object_t&val, uint64_t idx)
{
      ಪ್ರತಿಜ್ಞಾ(idx < array_size_);
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      tmp[idx] = val;
}

ಶೂನ್ಯ property_object::get_object(ವರ್ಣ*buf, vvp_object_t&val, uint64_t idx)
{
      ಪ್ರತಿಜ್ಞಾ(idx < array_size_);
      ಧ್ರುವ vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+offset_);
      val = tmp[idx];
}

ಶೂನ್ಯ property_object::copy(ವರ್ಣ*dst, ವರ್ಣ*src)
{
      vvp_object_t*dst_obj = reinterpret_cast<vvp_object_t*>(dst+offset_);
      ಧ್ರುವ vvp_object_t*src_obj = reinterpret_cast<vvp_object_t*>(src+offset_);
      ಪ್ರತಿ (size_t idx = 0 ; idx < array_size_ ; idx += 1)
	    dst_obj[idx] = src_obj[idx];
}

/* **** */

class_type::class_type(ಧ್ರುವ string&nam, size_t nprop)
: class_name_(nam), properties_(nprop)
{
      instance_size_ = 0;
}

class_type::~class_type()
{
      ಪ್ರತಿ (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    delete properties_[idx].type;
}

ಶೂನ್ಯ class_type::set_property(size_t idx, ಧ್ರುವ string&name, ಧ್ರುವ string&type, uint64_t array_size)
{
      ಪ್ರತಿಜ್ಞಾ(idx < properties_.size());
      properties_[idx].name = name;

      ಯದಿ (type == "b8")
	    properties_[idx].type = ನೂತನ property_atom<uint8_t>;
      ಅನ್ಯಥಾ ಯದಿ (type == "b16")
	    properties_[idx].type = ನೂತನ property_atom<uint16_t>;
      ಅನ್ಯಥಾ ಯದಿ (type == "b32")
	    properties_[idx].type = ನೂತನ property_atom<uint32_t>;
      ಅನ್ಯಥಾ ಯದಿ (type == "b64")
	    properties_[idx].type = ನೂತನ property_atom<uint64_t>;
      ಅನ್ಯಥಾ ಯದಿ (type == "sb8")
	    properties_[idx].type = ನೂತನ property_atom<int8_t>;
      ಅನ್ಯಥಾ ಯದಿ (type == "sb16")
	    properties_[idx].type = ನೂತನ property_atom<int16_t>;
      ಅನ್ಯಥಾ ಯದಿ (type == "sb32")
	    properties_[idx].type = ನೂತನ property_atom<int32_t>;
      ಅನ್ಯಥಾ ಯದಿ (type == "sb64")
	    properties_[idx].type = ನೂತನ property_atom<int64_t>;
      ಅನ್ಯಥಾ ಯದಿ (type == "r")
	    properties_[idx].type = ನೂತನ property_real<ದ್ವಿಗುಣ>;
      ಅನ್ಯಥಾ ಯದಿ (type == "S")
	    properties_[idx].type = ನೂತನ property_string;
      ಅನ್ಯಥಾ ಯದಿ (type == "o")
	    properties_[idx].type = ನೂತನ property_object(array_size);
      ಅನ್ಯಥಾ ಯದಿ (type[0] == 'b') {
	    size_t wid = strtoul(type.c_str()+1, 0, 0);
	    properties_[idx].type = ನೂತನ property_bit(wid);
      } ಅನ್ಯಥಾ ಯದಿ (type[0] == 'L') {
	    size_t wid = strtoul(type.c_str()+1,0,0);
	    properties_[idx].type = ನೂತನ property_logic(wid);
      } ಅನ್ಯಥಾ ಯದಿ (type[0] == 's' && type[1] == 'L') {
	    size_t wid = strtoul(type.c_str()+2,0,0);
	    properties_[idx].type = ನೂತನ property_logic(wid);
      } ಅನ್ಯಥಾ {
	    properties_[idx].type = 0;
      }
}

ಶೂನ್ಯ class_type::finish_setup(ಶೂನ್ಯ)
{
      map<size_t, vector<size_t> > size_map;
	// Add up all the sizes to get a total instance size. This
	// figures out how much memory a complete instance will need.
      size_t accum = 0;
      ಪ್ರತಿ (size_t idx = 0 ; idx < properties_.size() ; idx += 1) {
	    ಪ್ರತಿಜ್ಞಾ(properties_[idx].type);
	    size_t instance_size = properties_[idx].type->instance_size();
	    accum += instance_size;
	    size_map[instance_size].push_back(idx);
      }

      instance_size_ = accum;

	// Now allocate the properties to offsets within an instance
	// space. Allocate the properties largest objects first so
	// that they are assured better alignment.
      accum = 0;
      ಪ್ರತಿ (map<size_t, vector<size_t> >::reverse_iterator cur = size_map.rbegin()
		 ; cur != size_map.rend() ; ++ cur) {
	    ಪ್ರತಿ (size_t idx = 0 ; idx < cur->second.size() ; idx += 1) {
		  size_t pid = cur->second[idx];
		  class_property_t*ptype = properties_[pid].type;
		  ಪ್ರತಿಜ್ಞಾ(ptype->instance_size() == cur->first);
		  ptype->set_offset(accum);
		  accum += cur->first;
	    }
      }
}

class_type::inst_t class_type::instance_new() ಧ್ರುವ
{
      ವರ್ಣ*buf = ನೂತನ ವರ್ಣ [instance_size_];

      ಪ್ರತಿ (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->construct(buf);

      ಪ್ರತ್ಯಾಗಮ reinterpret_cast<inst_t> (buf);
}

ಶೂನ್ಯ class_type::instance_delete(class_type::inst_t obj) ಧ್ರುವ
{
      ವರ್ಣ*buf = reinterpret_cast<ವರ್ಣ*> (obj);

      ಪ್ರತಿ (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->destruct(buf);

      delete[]buf;
}

ಶೂನ್ಯ class_type::set_vec4(class_type::inst_t obj, size_t pid,
			  ಧ್ರುವ vvp_vector4_t&val) ಧ್ರುವ
{
      ವರ್ಣ*buf = reinterpret_cast<ವರ್ಣ*> (obj);
      ಪ್ರತಿಜ್ಞಾ(pid < properties_.size());
      properties_[pid].type->set_vec4(buf, val);
}

ಶೂನ್ಯ class_type::get_vec4(class_type::inst_t obj, size_t pid,
			  vvp_vector4_t&val) ಧ್ರುವ
{
      ವರ್ಣ*buf = reinterpret_cast<ವರ್ಣ*> (obj);
      ಪ್ರತಿಜ್ಞಾ(pid < properties_.size());
      properties_[pid].type->get_vec4(buf, val);
}

ಶೂನ್ಯ class_type::set_real(class_type::inst_t obj, size_t pid,
			  ದ್ವಿಗುಣ val) ಧ್ರುವ
{
      ವರ್ಣ*buf = reinterpret_cast<ವರ್ಣ*> (obj);
      ಪ್ರತಿಜ್ಞಾ(pid < properties_.size());
      properties_[pid].type->set_real(buf, val);
}

ದ್ವಿಗುಣ class_type::get_real(class_type::inst_t obj, size_t pid) ಧ್ರುವ
{
      ವರ್ಣ*buf = reinterpret_cast<ವರ್ಣ*> (obj);
      ಪ್ರತಿಜ್ಞಾ(pid < properties_.size());
      ಪ್ರತ್ಯಾಗಮ properties_[pid].type->get_real(buf);
}

ಶೂನ್ಯ class_type::set_string(class_type::inst_t obj, size_t pid,
			    ಧ್ರುವ string&val) ಧ್ರುವ
{
      ವರ್ಣ*buf = reinterpret_cast<ವರ್ಣ*> (obj);
      ಪ್ರತಿಜ್ಞಾ(pid < properties_.size());
      properties_[pid].type->set_string(buf, val);
}

string class_type::get_string(class_type::inst_t obj, size_t pid) ಧ್ರುವ
{
      ವರ್ಣ*buf = reinterpret_cast<ವರ್ಣ*> (obj);
      ಪ್ರತಿಜ್ಞಾ(pid < properties_.size());
      ಪ್ರತ್ಯಾಗಮ properties_[pid].type->get_string(buf);
}

ಶೂನ್ಯ class_type::set_object(class_type::inst_t obj, size_t pid,
			    ಧ್ರುವ vvp_object_t&val, size_t idx) ಧ್ರುವ
{
      ವರ್ಣ*buf = reinterpret_cast<ವರ್ಣ*> (obj);
      ಪ್ರತಿಜ್ಞಾ(pid < properties_.size());
      properties_[pid].type->set_object(buf, val, idx);
}

ಶೂನ್ಯ class_type::get_object(class_type::inst_t obj, size_t pid,
			    vvp_object_t&val, size_t idx) ಧ್ರುವ
{
      ವರ್ಣ*buf = reinterpret_cast<ವರ್ಣ*> (obj);
      ಪ್ರತಿಜ್ಞಾ(pid < properties_.size());
      properties_[pid].type->get_object(buf, val, idx);
}

ಶೂನ್ಯ class_type::copy_property(class_type::inst_t dst, size_t pid, class_type::inst_t src) ಧ್ರುವ
{
      ವರ್ಣ*dst_buf = reinterpret_cast<ವರ್ಣ*> (dst);
      ವರ್ಣ*src_buf = reinterpret_cast<ವರ್ಣ*> (src);

      ಪ್ರತಿಜ್ಞಾ(pid < properties_.size());

      properties_[pid].type->copy(dst_buf, src_buf);
}

ಪೂರ್ಣಾಙ್ಕ class_type::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ vpiClassDefn;
}

ಸ್ಥಿರ class_type*compile_class = 0;

ಶೂನ್ಯ compile_class_start(ವರ್ಣ*lab, ವರ್ಣ*nam, unsigned ntype)
{
      ಪ್ರತಿಜ್ಞಾ(compile_class == 0);
      compile_class = ನೂತನ class_type(nam, ntype);
      compile_vpi_ಚಿಹ್ನೆ(lab, compile_class);
      ಮುಕ್ತ(lab);
      delete[]nam;
}

ಶೂನ್ಯ compile_class_property(unsigned idx, ವರ್ಣ*nam, ವರ್ಣ*typ, uint64_t array_size)
{
      ಪ್ರತಿಜ್ಞಾ(compile_class);
      compile_class->set_property(idx, nam, typ, array_size);
      delete[]nam;
      delete[]typ;
}

ಶೂನ್ಯ compile_class_done(ಶೂನ್ಯ)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ಪ್ರತಿಜ್ಞಾ(scope);
      ಪ್ರತಿಜ್ಞಾ(compile_class);
      compile_class->finish_setup();
      scope->classes[compile_class->class_name()] = compile_class;
      compile_class = 0;
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ class_def_delete(class_type *item)
{
      delete item;
}
#endif
