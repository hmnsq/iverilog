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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ਜੇ_ਅੰਤ
# include  <cassert>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

/*
 * This class_property_t class is an abstract base class for
 * representing a property of an instance. The definition keeps and
 * array (of pointers) of these in order to define the the class.
 */
ਵਰਗ class_property_t {
    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ class_property_t() { ਵਿੱਥ = 0; }
      ਆਭਾਸੀ ~class_property_t() =0;
	// How much space does an instance of this property require?
      ਆਭਾਸੀ size_t instance_ਮਾਪ() ਅਚਲ =0;

      ਖਾਲੀ ਸੈੱਟ_ਵਿੱਥ(size_t off) { ਵਿੱਥ = off; }

    ਜਨਤਕ:
      ਆਭਾਸੀ ਖਾਲੀ construct(ਅੱਖਰ*buf) ਅਚਲ;
      ਆਭਾਸੀ ਖਾਲੀ destruct(ਅੱਖਰ*buf) ਅਚਲ;

      ਆਭਾਸੀ ਖਾਲੀ ਸੈੱਟ_vec4(ਅੱਖਰ*buf, ਅਚਲ vvp_vector4_t&val);
      ਆਭਾਸੀ ਖਾਲੀ get_vec4(ਅੱਖਰ*buf, vvp_vector4_t&val);

      ਆਭਾਸੀ ਖਾਲੀ ਸੈੱਟ_real(ਅੱਖਰ*buf, ਦੁੱਗਣਾ val);
      ਆਭਾਸੀ ਦੁੱਗਣਾ get_real(ਅੱਖਰ*buf);

      ਆਭਾਸੀ ਖਾਲੀ ਸੈੱਟ_string(ਅੱਖਰ*buf, ਅਚਲ std::string&val);
      ਆਭਾਸੀ string get_string(ਅੱਖਰ*buf);

      ਆਭਾਸੀ ਖਾਲੀ ਸੈੱਟ_object(ਅੱਖਰ*buf, ਅਚਲ vvp_object_t&val, uint64_t element);
      ਆਭਾਸੀ ਖਾਲੀ get_object(ਅੱਖਰ*buf, vvp_object_t&val, uint64_t element);

	// Implement polymorphic shallow copy.
      ਆਭਾਸੀ ਖਾਲੀ copy(ਅੱਖਰ*buf, ਅੱਖਰ*src) = 0;

    ਸੁਰੱਖਿਅਤ:
      size_t ਵਿੱਥ;
};

class_property_t::~class_property_t()
{
}

ਖਾਲੀ class_property_t::construct(ਅੱਖਰ*) ਅਚਲ
{
}

ਖਾਲੀ class_property_t::destruct(ਅੱਖਰ*) ਅਚਲ
{
}

ਖਾਲੀ class_property_t::ਸੈੱਟ_vec4(ਅੱਖਰ*, ਅਚਲ vvp_vector4_t&)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ class_property_t::get_vec4(ਅੱਖਰ*, vvp_vector4_t&)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ class_property_t::ਸੈੱਟ_real(ਅੱਖਰ*, ਦੁੱਗਣਾ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਦੁੱਗਣਾ class_property_t::get_real(ਅੱਖਰ*)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ 0.0;
}

ਖਾਲੀ class_property_t::ਸੈੱਟ_string(ਅੱਖਰ*, ਅਚਲ string&)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

string class_property_t::get_string(ਅੱਖਰ*)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ "";
}

ਖਾਲੀ class_property_t::ਸੈੱਟ_object(ਅੱਖਰ*, ਅਚਲ vvp_object_t&, uint64_t)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ class_property_t::get_object(ਅੱਖਰ*, vvp_object_t&, uint64_t)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

/*
 */
ਸਾਂਚਾ <ਵਰਗ T> ਵਰਗ property_atom : ਜਨਤਕ class_property_t {
    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ ਸਪਸ਼ਟ property_atom(ਖਾਲੀ) { }
      ~property_atom() override { }

      size_t instance_ਮਾਪ() ਅਚਲ override { ਵਾਪਸ ਆਕਾਰ(T); }

    ਜਨਤਕ:
      ਖਾਲੀ construct(ਅੱਖਰ*buf) ਅਚਲ override
      { T*tmp = reinterpret_cast<T*> (buf+ਵਿੱਥ);
	*tmp = 0;
      }

      ਖਾਲੀ ਸੈੱਟ_vec4(ਅੱਖਰ*buf, ਅਚਲ vvp_vector4_t&val) override;
      ਖਾਲੀ get_vec4(ਅੱਖਰ*buf, vvp_vector4_t&val) override;

      ਖਾਲੀ copy(ਅੱਖਰ*dst, ਅੱਖਰ*src) override;
};

ਵਰਗ property_ਦੋਆਧਾਰੀ_ਅੰਕ : ਜਨਤਕ class_property_t {
    ਜਨਤਕ:
      ਸਪਸ਼ਟ ਸਤਰਅੰਦਰ property_ਦੋਆਧਾਰੀ_ਅੰਕ(size_t wid): wid_(wid) { }
      ~property_ਦੋਆਧਾਰੀ_ਅੰਕ() override { }

      size_t instance_ਮਾਪ() ਅਚਲ override { ਵਾਪਸ ਆਕਾਰ(vvp_vector2_t); }

    ਜਨਤਕ:
      ਖਾਲੀ construct(ਅੱਖਰ*buf) ਅਚਲ override
      { ਨਵਾਂ (buf+ਵਿੱਥ) vvp_vector2_t (0, wid_); }

      ਖਾਲੀ destruct(ਅੱਖਰ*buf) ਅਚਲ override
      { vvp_vector2_t*tmp = reinterpret_cast<vvp_vector2_t*>(buf+ਵਿੱਥ);
	tmp->~vvp_vector2_t();
      }

      ਖਾਲੀ ਸੈੱਟ_vec4(ਅੱਖਰ*buf, ਅਚਲ vvp_vector4_t&val) override;
      ਖਾਲੀ get_vec4(ਅੱਖਰ*buf, vvp_vector4_t&val) override;

      ਖਾਲੀ copy(ਅੱਖਰ*dst, ਅੱਖਰ*src) override;

    ਨਿਜੀ:
      size_t wid_;
};

ਵਰਗ property_logic : ਜਨਤਕ class_property_t {
    ਜਨਤਕ:
      ਸਪਸ਼ਟ ਸਤਰਅੰਦਰ property_logic(size_t wid): wid_(wid) { }
      ~property_logic() override { }

      size_t instance_ਮਾਪ() ਅਚਲ override { ਵਾਪਸ ਆਕਾਰ(vvp_vector4_t); }

    ਜਨਤਕ:
      ਖਾਲੀ construct(ਅੱਖਰ*buf) ਅਚਲ override
      { ਨਵਾਂ (buf+ਵਿੱਥ) vvp_vector4_t (wid_); }

      ਖਾਲੀ destruct(ਅੱਖਰ*buf) ਅਚਲ override
      { vvp_vector4_t*tmp = reinterpret_cast<vvp_vector4_t*>(buf+ਵਿੱਥ);
	tmp->~vvp_vector4_t();
      }

      ਖਾਲੀ ਸੈੱਟ_vec4(ਅੱਖਰ*buf, ਅਚਲ vvp_vector4_t&val) override;
      ਖਾਲੀ get_vec4(ਅੱਖਰ*buf, vvp_vector4_t&val) override;

      ਖਾਲੀ copy(ਅੱਖਰ*dst, ਅੱਖਰ*src) override;

    ਨਿਜੀ:
      size_t wid_;
};

ਸਾਂਚਾ <ਵਰਗ T> ਵਰਗ property_real : ਜਨਤਕ class_property_t {
    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ ਸਪਸ਼ਟ property_real(ਖਾਲੀ) { }
      ~property_real() override { }

      size_t instance_ਮਾਪ() ਅਚਲ override { ਵਾਪਸ ਆਕਾਰ(T); }

    ਜਨਤਕ:
      ਖਾਲੀ construct(ਅੱਖਰ*buf) ਅਚਲ override
      { T*tmp = reinterpret_cast<T*> (buf+ਵਿੱਥ);
	*tmp = 0.0;
      }

      ਖਾਲੀ ਸੈੱਟ_real(ਅੱਖਰ*buf, ਦੁੱਗਣਾ val) override;
      ਦੁੱਗਣਾ get_real(ਅੱਖਰ*buf) override;

      ਖਾਲੀ copy(ਅੱਖਰ*dst, ਅੱਖਰ*src) override;
};

ਵਰਗ property_string : ਜਨਤਕ class_property_t {
    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ ਸਪਸ਼ਟ property_string(ਖਾਲੀ) { }
      ~property_string() override { }

      size_t instance_ਮਾਪ() ਅਚਲ override { ਵਾਪਸ ਆਕਾਰ(std::string); }

    ਜਨਤਕ:
      ਖਾਲੀ construct(ਅੱਖਰ*buf) ਅਚਲ override
      { /* string*tmp = */ ਨਵਾਂ (buf+ਵਿੱਥ) string; }

      ਖਾਲੀ destruct(ਅੱਖਰ*buf) ਅਚਲ override
      { string*tmp = reinterpret_cast<string*> (buf+ਵਿੱਥ);
	tmp->~string();
      }

      ਖਾਲੀ ਸੈੱਟ_string(ਅੱਖਰ*buf, ਅਚਲ string&) override;
      string get_string(ਅੱਖਰ*buf) override;

      ਖਾਲੀ copy(ਅੱਖਰ*dst, ਅੱਖਰ*src) override;
};

ਵਰਗ property_object : ਜਨਤਕ class_property_t {
    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ ਸਪਸ਼ਟ property_object(uint64_t as): array_ਮਾਪ(as==0? 1 : as) { }
      ~property_object() override { }

      size_t instance_ਮਾਪ() ਅਚਲ override { ਵਾਪਸ array_ਮਾਪ * ਆਕਾਰ(vvp_object_t); }

    ਜਨਤਕ:
      ਖਾਲੀ construct(ਅੱਖਰ*buf) ਅਚਲ override;

      ਖਾਲੀ destruct(ਅੱਖਰ*buf) ਅਚਲ override;

      ਖਾਲੀ ਸੈੱਟ_object(ਅੱਖਰ*buf, ਅਚਲ vvp_object_t&, uint64_t) override;
      ਖਾਲੀ get_object(ਅੱਖਰ*buf, vvp_object_t&, uint64_t) override;

      ਖਾਲੀ copy(ਅੱਖਰ*dst, ਅੱਖਰ*src) override;

    ਨਿਜੀ:
      size_t array_ਮਾਪ;
};

ਸਾਂਚਾ <ਵਰਗ T> ਖਾਲੀ property_atom<T>::ਸੈੱਟ_vec4(ਅੱਖਰ*buf, ਅਚਲ vvp_vector4_t&val)
{
      T*tmp = reinterpret_cast<T*> (buf+ਵਿੱਥ);
      ਤਾਰਕਿਕ flag = vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(val, *tmp, ਸੱਚ, ਝੂਠ);
      ਪ੍ਰਤਿਜ੍ਞਾ(flag);
}

ਸਾਂਚਾ <ਵਰਗ T> ਖਾਲੀ property_atom<T>::get_vec4(ਅੱਖਰ*buf, vvp_vector4_t&val)
{
      T*src = reinterpret_cast<T*> (buf+ਵਿੱਥ);
      ਅਚਲ size_t tmp_cnt = ਆਕਾਰ(T)<ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)
				       ? 1
				       : ਆਕਾਰ(T) / ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ);
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp[tmp_cnt];
      tmp[0] = src[0];

      ਲਈ (size_t idx = 1 ; idx < tmp_cnt ; idx += 1)
	    tmp[idx] = src[0] >> idx * 8 * ਆਕਾਰ(tmp[0]);

      val.resize(8*ਆਕਾਰ(T));
      val.setarray(0, val.size(), tmp);
}

ਸਾਂਚਾ <ਵਰਗ T> ਖਾਲੀ property_atom<T>::copy(ਅੱਖਰ*dst, ਅੱਖਰ*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+ਵਿੱਥ);
      T*src_obj = reinterpret_cast<T*> (src+ਵਿੱਥ);
      *dst_obj = *src_obj;
}

ਖਾਲੀ property_ਦੋਆਧਾਰੀ_ਅੰਕ::ਸੈੱਟ_vec4(ਅੱਖਰ*buf, ਅਚਲ vvp_vector4_t&val)
{
      vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+ਵਿੱਥ);
      *obj = val;
}

ਖਾਲੀ property_ਦੋਆਧਾਰੀ_ਅੰਕ::get_vec4(ਅੱਖਰ*buf, vvp_vector4_t&val)
{
      ਅਚਲ vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+ਵਿੱਥ);
      val = vector2_ਮੰਜ਼ਿਲ_vector4(*obj, obj->size());
}

ਖਾਲੀ property_ਦੋਆਧਾਰੀ_ਅੰਕ::copy(ਅੱਖਰ*dst, ਅੱਖਰ*src)
{
      vvp_vector2_t*dst_obj = reinterpret_cast<vvp_vector2_t*> (dst+ਵਿੱਥ);
      ਅਚਲ vvp_vector2_t*src_obj = reinterpret_cast<vvp_vector2_t*> (src+ਵਿੱਥ);
      *dst_obj = *src_obj;
}

ਖਾਲੀ property_logic::ਸੈੱਟ_vec4(ਅੱਖਰ*buf, ਅਚਲ vvp_vector4_t&val)
{
      vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+ਵਿੱਥ);
      *obj = val;
}

ਖਾਲੀ property_logic::get_vec4(ਅੱਖਰ*buf, vvp_vector4_t&val)
{
      ਅਚਲ vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+ਵਿੱਥ);
      val = *obj;
}

ਖਾਲੀ property_logic::copy(ਅੱਖਰ*dst, ਅੱਖਰ*src)
{
      vvp_vector4_t*dst_obj = reinterpret_cast<vvp_vector4_t*> (dst+ਵਿੱਥ);
      ਅਚਲ vvp_vector4_t*src_obj = reinterpret_cast<vvp_vector4_t*> (src+ਵਿੱਥ);
      *dst_obj = *src_obj;
}

ਸਾਂਚਾ <ਵਰਗ T> ਖਾਲੀ property_real<T>::ਸੈੱਟ_real(ਅੱਖਰ*buf, ਦੁੱਗਣਾ val)
{
      T*tmp = reinterpret_cast<T*>(buf+ਵਿੱਥ);
      *tmp = val;
}

ਸਾਂਚਾ <ਵਰਗ T> ਦੁੱਗਣਾ property_real<T>::get_real(ਅੱਖਰ*buf)
{
      T*tmp = reinterpret_cast<T*>(buf+ਵਿੱਥ);
      ਵਾਪਸ *tmp;
}

ਸਾਂਚਾ <ਵਰਗ T> ਖਾਲੀ property_real<T>::copy(ਅੱਖਰ*dst, ਅੱਖਰ*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+ਵਿੱਥ);
      T*src_obj = reinterpret_cast<T*> (src+ਵਿੱਥ);
      *dst_obj = *src_obj;
}

ਖਾਲੀ property_string::ਸੈੱਟ_string(ਅੱਖਰ*buf, ਅਚਲ string&val)
{
      string*tmp = reinterpret_cast<string*>(buf+ਵਿੱਥ);
      *tmp = val;
}

string property_string::get_string(ਅੱਖਰ*buf)
{
      ਅਚਲ string*tmp = reinterpret_cast<string*>(buf+ਵਿੱਥ);
      ਵਾਪਸ *tmp;
}

ਖਾਲੀ property_string::copy(ਅੱਖਰ*dst, ਅੱਖਰ*src)
{
      string*dst_obj = reinterpret_cast<string*> (dst+ਵਿੱਥ);
      ਅਚਲ string*src_obj = reinterpret_cast<string*> (src+ਵਿੱਥ);
      *dst_obj = *src_obj;
}

ਖਾਲੀ property_object::construct(ਅੱਖਰ*buf) ਅਚਲ
{
      ਲਈ (size_t idx = 0 ; idx < array_ਮਾਪ ; idx += 1)
	    ਨਵਾਂ (buf+ਵਿੱਥ + idx*ਆਕਾਰ(vvp_object_t)) vvp_object_t;
}

ਖਾਲੀ property_object::destruct(ਅੱਖਰ*buf) ਅਚਲ
{
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*> (buf+ਵਿੱਥ);
      ਲਈ (size_t idx = 0 ; idx < array_ਮਾਪ ; idx += 1)
	    (tmp+idx)->~vvp_object_t();
}

ਖਾਲੀ property_object::ਸੈੱਟ_object(ਅੱਖਰ*buf, ਅਚਲ vvp_object_t&val, uint64_t idx)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(idx < array_ਮਾਪ);
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+ਵਿੱਥ);
      tmp[idx] = val;
}

ਖਾਲੀ property_object::get_object(ਅੱਖਰ*buf, vvp_object_t&val, uint64_t idx)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(idx < array_ਮਾਪ);
      ਅਚਲ vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+ਵਿੱਥ);
      val = tmp[idx];
}

ਖਾਲੀ property_object::copy(ਅੱਖਰ*dst, ਅੱਖਰ*src)
{
      vvp_object_t*dst_obj = reinterpret_cast<vvp_object_t*>(dst+ਵਿੱਥ);
      ਅਚਲ vvp_object_t*src_obj = reinterpret_cast<vvp_object_t*>(src+ਵਿੱਥ);
      ਲਈ (size_t idx = 0 ; idx < array_ਮਾਪ ; idx += 1)
	    dst_obj[idx] = src_obj[idx];
}

/* **** */

class_type::class_type(ਅਚਲ string&nam, size_t nprop)
: class_name_(nam), properties_(nprop)
{
      instance_ਮਾਪ_2 = 0;
}

class_type::~class_type()
{
      ਲਈ (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    ਮਿਟਾਓ properties_[idx].type;
}

ਖਾਲੀ class_type::ਸੈੱਟ_property(size_t idx, ਅਚਲ string&name, ਅਚਲ string&type, uint64_t array_ਮਾਪ_2)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(idx < properties_.size());
      properties_[idx].name = name;

      ਜੇ (type == "b8")
	    properties_[idx].type = ਨਵਾਂ property_atom<uint8_t>;
      ਨਹੀਂਤਾਂ ਜੇ (type == "b16")
	    properties_[idx].type = ਨਵਾਂ property_atom<uint16_t>;
      ਨਹੀਂਤਾਂ ਜੇ (type == "b32")
	    properties_[idx].type = ਨਵਾਂ property_atom<uint32_t>;
      ਨਹੀਂਤਾਂ ਜੇ (type == "b64")
	    properties_[idx].type = ਨਵਾਂ property_atom<uint64_t>;
      ਨਹੀਂਤਾਂ ਜੇ (type == "sb8")
	    properties_[idx].type = ਨਵਾਂ property_atom<int8_t>;
      ਨਹੀਂਤਾਂ ਜੇ (type == "sb16")
	    properties_[idx].type = ਨਵਾਂ property_atom<int16_t>;
      ਨਹੀਂਤਾਂ ਜੇ (type == "sb32")
	    properties_[idx].type = ਨਵਾਂ property_atom<int32_t>;
      ਨਹੀਂਤਾਂ ਜੇ (type == "sb64")
	    properties_[idx].type = ਨਵਾਂ property_atom<int64_t>;
      ਨਹੀਂਤਾਂ ਜੇ (type == "r")
	    properties_[idx].type = ਨਵਾਂ property_real<ਦੁੱਗਣਾ>;
      ਨਹੀਂਤਾਂ ਜੇ (type == "S")
	    properties_[idx].type = ਨਵਾਂ property_string;
      ਨਹੀਂਤਾਂ ਜੇ (type == "o")
	    properties_[idx].type = ਨਵਾਂ property_object(array_ਮਾਪ_2);
      ਨਹੀਂਤਾਂ ਜੇ (type[0] == 'b') {
	    size_t wid = strtoul(type.c_str()+1, 0, 0);
	    properties_[idx].type = ਨਵਾਂ property_ਦੋਆਧਾਰੀ_ਅੰਕ(wid);
      } ਨਹੀਂਤਾਂ ਜੇ (type[0] == 'L') {
	    size_t wid = strtoul(type.c_str()+1,0,0);
	    properties_[idx].type = ਨਵਾਂ property_logic(wid);
      } ਨਹੀਂਤਾਂ ਜੇ (type[0] == 's' && type[1] == 'L') {
	    size_t wid = strtoul(type.c_str()+2,0,0);
	    properties_[idx].type = ਨਵਾਂ property_logic(wid);
      } ਨਹੀਂਤਾਂ {
	    properties_[idx].type = 0;
      }
}

ਖਾਲੀ class_type::ਅੰਤ_ਤਿਆਰੀ(ਖਾਲੀ)
{
      map<size_t, vector<size_t> > ਮਾਪ_map;
	// Add up all the sizes to get a total instance size. This
	// figures out how much memory a complete instance will need.
      size_t accum = 0;
      ਲਈ (size_t idx = 0 ; idx < properties_.size() ; idx += 1) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(properties_[idx].type);
	    size_t instance_ਮਾਪ = properties_[idx].type->instance_ਮਾਪ();
	    accum += instance_ਮਾਪ;
	    ਮਾਪ_map[instance_ਮਾਪ].push_back(idx);
      }

      instance_ਮਾਪ_2 = accum;

	// Now allocate the properties to offsets within an instance
	// space. Allocate the properties largest objects first so
	// that they are assured better alignment.
      accum = 0;
      ਲਈ (map<size_t, vector<size_t> >::reverse_iterator cur = ਮਾਪ_map.rbegin()
		 ; cur != ਮਾਪ_map.rend() ; ++ cur) {
	    ਲਈ (size_t idx = 0 ; idx < cur->second.size() ; idx += 1) {
		  size_t pid = cur->second[idx];
		  class_property_t*ptype = properties_[pid].type;
		  ਪ੍ਰਤਿਜ੍ਞਾ(ptype->instance_ਮਾਪ() == cur->ਪਹਿਲਾ);
		  ptype->ਸੈੱਟ_ਵਿੱਥ(accum);
		  accum += cur->ਪਹਿਲਾ;
	    }
      }
}

class_type::inst_t class_type::instance_new() ਅਚਲ
{
      ਅੱਖਰ*buf = ਨਵਾਂ ਅੱਖਰ [instance_ਮਾਪ_2];

      ਲਈ (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->construct(buf);

      ਵਾਪਸ reinterpret_cast<inst_t> (buf);
}

ਖਾਲੀ class_type::instance_delete(class_type::inst_t obj) ਅਚਲ
{
      ਅੱਖਰ*buf = reinterpret_cast<ਅੱਖਰ*> (obj);

      ਲਈ (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->destruct(buf);

      ਮਿਟਾਓ[]buf;
}

ਖਾਲੀ class_type::ਸੈੱਟ_vec4(class_type::inst_t obj, size_t pid,
			  ਅਚਲ vvp_vector4_t&val) ਅਚਲ
{
      ਅੱਖਰ*buf = reinterpret_cast<ਅੱਖਰ*> (obj);
      ਪ੍ਰਤਿਜ੍ਞਾ(pid < properties_.size());
      properties_[pid].type->ਸੈੱਟ_vec4(buf, val);
}

ਖਾਲੀ class_type::get_vec4(class_type::inst_t obj, size_t pid,
			  vvp_vector4_t&val) ਅਚਲ
{
      ਅੱਖਰ*buf = reinterpret_cast<ਅੱਖਰ*> (obj);
      ਪ੍ਰਤਿਜ੍ਞਾ(pid < properties_.size());
      properties_[pid].type->get_vec4(buf, val);
}

ਖਾਲੀ class_type::ਸੈੱਟ_real(class_type::inst_t obj, size_t pid,
			  ਦੁੱਗਣਾ val) ਅਚਲ
{
      ਅੱਖਰ*buf = reinterpret_cast<ਅੱਖਰ*> (obj);
      ਪ੍ਰਤਿਜ੍ਞਾ(pid < properties_.size());
      properties_[pid].type->ਸੈੱਟ_real(buf, val);
}

ਦੁੱਗਣਾ class_type::get_real(class_type::inst_t obj, size_t pid) ਅਚਲ
{
      ਅੱਖਰ*buf = reinterpret_cast<ਅੱਖਰ*> (obj);
      ਪ੍ਰਤਿਜ੍ਞਾ(pid < properties_.size());
      ਵਾਪਸ properties_[pid].type->get_real(buf);
}

ਖਾਲੀ class_type::ਸੈੱਟ_string(class_type::inst_t obj, size_t pid,
			    ਅਚਲ string&val) ਅਚਲ
{
      ਅੱਖਰ*buf = reinterpret_cast<ਅੱਖਰ*> (obj);
      ਪ੍ਰਤਿਜ੍ਞਾ(pid < properties_.size());
      properties_[pid].type->ਸੈੱਟ_string(buf, val);
}

string class_type::get_string(class_type::inst_t obj, size_t pid) ਅਚਲ
{
      ਅੱਖਰ*buf = reinterpret_cast<ਅੱਖਰ*> (obj);
      ਪ੍ਰਤਿਜ੍ਞਾ(pid < properties_.size());
      ਵਾਪਸ properties_[pid].type->get_string(buf);
}

ਖਾਲੀ class_type::ਸੈੱਟ_object(class_type::inst_t obj, size_t pid,
			    ਅਚਲ vvp_object_t&val, size_t idx) ਅਚਲ
{
      ਅੱਖਰ*buf = reinterpret_cast<ਅੱਖਰ*> (obj);
      ਪ੍ਰਤਿਜ੍ਞਾ(pid < properties_.size());
      properties_[pid].type->ਸੈੱਟ_object(buf, val, idx);
}

ਖਾਲੀ class_type::get_object(class_type::inst_t obj, size_t pid,
			    vvp_object_t&val, size_t idx) ਅਚਲ
{
      ਅੱਖਰ*buf = reinterpret_cast<ਅੱਖਰ*> (obj);
      ਪ੍ਰਤਿਜ੍ਞਾ(pid < properties_.size());
      properties_[pid].type->get_object(buf, val, idx);
}

ਖਾਲੀ class_type::copy_property(class_type::inst_t dst, size_t pid, class_type::inst_t src) ਅਚਲ
{
      ਅੱਖਰ*dst_buf = reinterpret_cast<ਅੱਖਰ*> (dst);
      ਅੱਖਰ*src_buf = reinterpret_cast<ਅੱਖਰ*> (src);

      ਪ੍ਰਤਿਜ੍ਞਾ(pid < properties_.size());

      properties_[pid].type->copy(dst_buf, src_buf);
}

ਪੂਰਨ class_type::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{
      ਵਾਪਸ vpiClassDefn;
}

ਸਥਿਰ class_type*compile_class = 0;

ਖਾਲੀ compile_class_ਸ਼ੁਰੂ(ਅੱਖਰ*lab, ਅੱਖਰ*nam, ਬਿਨਾਨਿਸ਼ਾਨੀ ntype)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(compile_class == 0);
      compile_class = ਨਵਾਂ class_type(nam, ntype);
      compile_vpi_ਚਿੰਨ੍ਹ(lab, compile_class);
      ਮੁਕਤ(lab);
      ਮਿਟਾਓ[]nam;
}

ਖਾਲੀ compile_class_property(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਅੱਖਰ*nam, ਅੱਖਰ*typ, uint64_t array_ਮਾਪ_2)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(compile_class);
      compile_class->ਸੈੱਟ_property(idx, nam, typ, array_ਮਾਪ_2);
      ਮਿਟਾਓ[]nam;
      ਮਿਟਾਓ[]typ;
}

ਖਾਲੀ compile_class_ਪੂਰਾ(ਖਾਲੀ)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ਪ੍ਰਤਿਜ੍ਞਾ(scope);
      ਪ੍ਰਤਿਜ੍ਞਾ(compile_class);
      compile_class->ਅੰਤ_ਤਿਆਰੀ();
      scope->classes[compile_class->class_name()] = compile_class;
      compile_class = 0;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ class_def_delete(class_type *ਵਸਤੂ)
{
      ਮਿਟਾਓ ਵਸਤੂ;
}
#ਜੇ_ਅੰਤ
