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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ಆದರೆ_ಅಂತ್ಯ
# include  <cassert>

ಬಳಸಿ ನಾಮಾಕಾಶ std;

/*
 * This class_property_t class is an abstract base class for
 * representing a property of an instance. The definition keeps and
 * array (of pointers) of these in order to define the the class.
 */
ವರ್ಗ class_property_t {
    ಸಾರ್ವಜನಿಕ:
      ಸಾಲೊಳಗೆ class_property_t() { ಸ್ಥಾನಾಂತರ = 0; }
      ವಾಸ್ತವಿಕ ~class_property_t() =0;
	// How much space does an instance of this property require?
      ವಾಸ್ತವಿಕ size_t instance_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ =0;

      ಶೂನ್ಯ ಗಣ_ಸ್ಥಾನಾಂತರ(size_t off) { ಸ್ಥಾನಾಂತರ = off; }

    ಸಾರ್ವಜನಿಕ:
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ construct(ಅಕ್ಷರ*buf) ಸ್ಥಿರಾಂಕ;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ destruct(ಅಕ್ಷರ*buf) ಸ್ಥಿರಾಂಕ;

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಗಣ_vec4(ಅಕ್ಷರ*buf, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ get_vec4(ಅಕ್ಷರ*buf, vvp_vector4_t&val);

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಗಣ_real(ಅಕ್ಷರ*buf, ದ್ವಿಗುಣ val);
      ವಾಸ್ತವಿಕ ದ್ವಿಗುಣ get_real(ಅಕ್ಷರ*buf);

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಗಣ_string(ಅಕ್ಷರ*buf, ಸ್ಥಿರಾಂಕ std::string&val);
      ವಾಸ್ತವಿಕ string get_string(ಅಕ್ಷರ*buf);

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಗಣ_object(ಅಕ್ಷರ*buf, ಸ್ಥಿರಾಂಕ vvp_object_t&val, uint64_t element);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ get_object(ಅಕ್ಷರ*buf, vvp_object_t&val, uint64_t element);

	// Implement polymorphic shallow copy.
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ copy(ಅಕ್ಷರ*buf, ಅಕ್ಷರ*src) = 0;

    ಸಂರಕ್ಷಿತ:
      size_t ಸ್ಥಾನಾಂತರ;
};

class_property_t::~class_property_t()
{
}

ಶೂನ್ಯ class_property_t::construct(ಅಕ್ಷರ*) ಸ್ಥಿರಾಂಕ
{
}

ಶೂನ್ಯ class_property_t::destruct(ಅಕ್ಷರ*) ಸ್ಥಿರಾಂಕ
{
}

ಶೂನ್ಯ class_property_t::ಗಣ_vec4(ಅಕ್ಷರ*, ಸ್ಥಿರಾಂಕ vvp_vector4_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ class_property_t::get_vec4(ಅಕ್ಷರ*, vvp_vector4_t&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ class_property_t::ಗಣ_real(ಅಕ್ಷರ*, ದ್ವಿಗುಣ)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ದ್ವಿಗುಣ class_property_t::get_real(ಅಕ್ಷರ*)
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ 0.0;
}

ಶೂನ್ಯ class_property_t::ಗಣ_string(ಅಕ್ಷರ*, ಸ್ಥಿರಾಂಕ string&)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

string class_property_t::get_string(ಅಕ್ಷರ*)
{
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ "";
}

ಶೂನ್ಯ class_property_t::ಗಣ_object(ಅಕ್ಷರ*, ಸ್ಥಿರಾಂಕ vvp_object_t&, uint64_t)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ class_property_t::get_object(ಅಕ್ಷರ*, vvp_object_t&, uint64_t)
{
      ಪ್ರತಿಜ್ಞಾ(0);
}

/*
 */
ಮಾದರಿ <ವರ್ಗ T> ವರ್ಗ property_atom : ಸಾರ್ವಜನಿಕ class_property_t {
    ಸಾರ್ವಜನಿಕ:
      ಸಾಲೊಳಗೆ ಸ್ಪಷ್ಟ property_atom(ಶೂನ್ಯ) { }
      ~property_atom() override { }

      size_t instance_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ ಗಾತ್ರ(T); }

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ construct(ಅಕ್ಷರ*buf) ಸ್ಥಿರಾಂಕ override
      { T*tmp = reinterpret_cast<T*> (buf+ಸ್ಥಾನಾಂತರ);
	*tmp = 0;
      }

      ಶೂನ್ಯ ಗಣ_vec4(ಅಕ್ಷರ*buf, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val) override;
      ಶೂನ್ಯ get_vec4(ಅಕ್ಷರ*buf, vvp_vector4_t&val) override;

      ಶೂನ್ಯ copy(ಅಕ್ಷರ*dst, ಅಕ್ಷರ*src) override;
};

ವರ್ಗ property_ಬಿಟ್ : ಸಾರ್ವಜನಿಕ class_property_t {
    ಸಾರ್ವಜನಿಕ:
      ಸ್ಪಷ್ಟ ಸಾಲೊಳಗೆ property_ಬಿಟ್(size_t wid): wid_(wid) { }
      ~property_ಬಿಟ್() override { }

      size_t instance_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ ಗಾತ್ರ(vvp_vector2_t); }

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ construct(ಅಕ್ಷರ*buf) ಸ್ಥಿರಾಂಕ override
      { ಹೊಸ (buf+ಸ್ಥಾನಾಂತರ) vvp_vector2_t (0, wid_); }

      ಶೂನ್ಯ destruct(ಅಕ್ಷರ*buf) ಸ್ಥಿರಾಂಕ override
      { vvp_vector2_t*tmp = reinterpret_cast<vvp_vector2_t*>(buf+ಸ್ಥಾನಾಂತರ);
	tmp->~vvp_vector2_t();
      }

      ಶೂನ್ಯ ಗಣ_vec4(ಅಕ್ಷರ*buf, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val) override;
      ಶೂನ್ಯ get_vec4(ಅಕ್ಷರ*buf, vvp_vector4_t&val) override;

      ಶೂನ್ಯ copy(ಅಕ್ಷರ*dst, ಅಕ್ಷರ*src) override;

    ಖಾಸಗಿ:
      size_t wid_;
};

ವರ್ಗ property_logic : ಸಾರ್ವಜನಿಕ class_property_t {
    ಸಾರ್ವಜನಿಕ:
      ಸ್ಪಷ್ಟ ಸಾಲೊಳಗೆ property_logic(size_t wid): wid_(wid) { }
      ~property_logic() override { }

      size_t instance_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ ಗಾತ್ರ(vvp_vector4_t); }

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ construct(ಅಕ್ಷರ*buf) ಸ್ಥಿರಾಂಕ override
      { ಹೊಸ (buf+ಸ್ಥಾನಾಂತರ) vvp_vector4_t (wid_); }

      ಶೂನ್ಯ destruct(ಅಕ್ಷರ*buf) ಸ್ಥಿರಾಂಕ override
      { vvp_vector4_t*tmp = reinterpret_cast<vvp_vector4_t*>(buf+ಸ್ಥಾನಾಂತರ);
	tmp->~vvp_vector4_t();
      }

      ಶೂನ್ಯ ಗಣ_vec4(ಅಕ್ಷರ*buf, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val) override;
      ಶೂನ್ಯ get_vec4(ಅಕ್ಷರ*buf, vvp_vector4_t&val) override;

      ಶೂನ್ಯ copy(ಅಕ್ಷರ*dst, ಅಕ್ಷರ*src) override;

    ಖಾಸಗಿ:
      size_t wid_;
};

ಮಾದರಿ <ವರ್ಗ T> ವರ್ಗ property_real : ಸಾರ್ವಜನಿಕ class_property_t {
    ಸಾರ್ವಜನಿಕ:
      ಸಾಲೊಳಗೆ ಸ್ಪಷ್ಟ property_real(ಶೂನ್ಯ) { }
      ~property_real() override { }

      size_t instance_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ ಗಾತ್ರ(T); }

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ construct(ಅಕ್ಷರ*buf) ಸ್ಥಿರಾಂಕ override
      { T*tmp = reinterpret_cast<T*> (buf+ಸ್ಥಾನಾಂತರ);
	*tmp = 0.0;
      }

      ಶೂನ್ಯ ಗಣ_real(ಅಕ್ಷರ*buf, ದ್ವಿಗುಣ val) override;
      ದ್ವಿಗುಣ get_real(ಅಕ್ಷರ*buf) override;

      ಶೂನ್ಯ copy(ಅಕ್ಷರ*dst, ಅಕ್ಷರ*src) override;
};

ವರ್ಗ property_string : ಸಾರ್ವಜನಿಕ class_property_t {
    ಸಾರ್ವಜನಿಕ:
      ಸಾಲೊಳಗೆ ಸ್ಪಷ್ಟ property_string(ಶೂನ್ಯ) { }
      ~property_string() override { }

      size_t instance_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ ಗಾತ್ರ(std::string); }

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ construct(ಅಕ್ಷರ*buf) ಸ್ಥಿರಾಂಕ override
      { /* string*tmp = */ ಹೊಸ (buf+ಸ್ಥಾನಾಂತರ) string; }

      ಶೂನ್ಯ destruct(ಅಕ್ಷರ*buf) ಸ್ಥಿರಾಂಕ override
      { string*tmp = reinterpret_cast<string*> (buf+ಸ್ಥಾನಾಂತರ);
	tmp->~string();
      }

      ಶೂನ್ಯ ಗಣ_string(ಅಕ್ಷರ*buf, ಸ್ಥಿರಾಂಕ string&) override;
      string get_string(ಅಕ್ಷರ*buf) override;

      ಶೂನ್ಯ copy(ಅಕ್ಷರ*dst, ಅಕ್ಷರ*src) override;
};

ವರ್ಗ property_object : ಸಾರ್ವಜನಿಕ class_property_t {
    ಸಾರ್ವಜನಿಕ:
      ಸಾಲೊಳಗೆ ಸ್ಪಷ್ಟ property_object(uint64_t as): array_ಗಾತ್ರ(as==0? 1 : as) { }
      ~property_object() override { }

      size_t instance_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ array_ಗಾತ್ರ * ಗಾತ್ರ(vvp_object_t); }

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ construct(ಅಕ್ಷರ*buf) ಸ್ಥಿರಾಂಕ override;

      ಶೂನ್ಯ destruct(ಅಕ್ಷರ*buf) ಸ್ಥಿರಾಂಕ override;

      ಶೂನ್ಯ ಗಣ_object(ಅಕ್ಷರ*buf, ಸ್ಥಿರಾಂಕ vvp_object_t&, uint64_t) override;
      ಶೂನ್ಯ get_object(ಅಕ್ಷರ*buf, vvp_object_t&, uint64_t) override;

      ಶೂನ್ಯ copy(ಅಕ್ಷರ*dst, ಅಕ್ಷರ*src) override;

    ಖಾಸಗಿ:
      size_t array_ಗಾತ್ರ;
};

ಮಾದರಿ <ವರ್ಗ T> ಶೂನ್ಯ property_atom<T>::ಗಣ_vec4(ಅಕ್ಷರ*buf, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val)
{
      T*tmp = reinterpret_cast<T*> (buf+ಸ್ಥಾನಾಂತರ);
      ತಾರ್ಕಿಕ flag = vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(val, *tmp, ನಿಜ, ಸುಳ್ಳು);
      ಪ್ರತಿಜ್ಞಾ(flag);
}

ಮಾದರಿ <ವರ್ಗ T> ಶೂನ್ಯ property_atom<T>::get_vec4(ಅಕ್ಷರ*buf, vvp_vector4_t&val)
{
      T*src = reinterpret_cast<T*> (buf+ಸ್ಥಾನಾಂತರ);
      ಸ್ಥಿರಾಂಕ size_t tmp_cnt = ಗಾತ್ರ(T)<ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)
				       ? 1
				       : ಗಾತ್ರ(T) / ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ);
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp[tmp_cnt];
      tmp[0] = src[0];

      ಗಾಗಿ (size_t idx = 1 ; idx < tmp_cnt ; idx += 1)
	    tmp[idx] = src[0] >> idx * 8 * ಗಾತ್ರ(tmp[0]);

      val.resize(8*ಗಾತ್ರ(T));
      val.setarray(0, val.size(), tmp);
}

ಮಾದರಿ <ವರ್ಗ T> ಶೂನ್ಯ property_atom<T>::copy(ಅಕ್ಷರ*dst, ಅಕ್ಷರ*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+ಸ್ಥಾನಾಂತರ);
      T*src_obj = reinterpret_cast<T*> (src+ಸ್ಥಾನಾಂತರ);
      *dst_obj = *src_obj;
}

ಶೂನ್ಯ property_ಬಿಟ್::ಗಣ_vec4(ಅಕ್ಷರ*buf, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val)
{
      vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+ಸ್ಥಾನಾಂತರ);
      *obj = val;
}

ಶೂನ್ಯ property_ಬಿಟ್::get_vec4(ಅಕ್ಷರ*buf, vvp_vector4_t&val)
{
      ಸ್ಥಿರಾಂಕ vvp_vector2_t*obj = reinterpret_cast<vvp_vector2_t*> (buf+ಸ್ಥಾನಾಂತರ);
      val = vector2_ಗಮ್ಯ_vector4(*obj, obj->size());
}

ಶೂನ್ಯ property_ಬಿಟ್::copy(ಅಕ್ಷರ*dst, ಅಕ್ಷರ*src)
{
      vvp_vector2_t*dst_obj = reinterpret_cast<vvp_vector2_t*> (dst+ಸ್ಥಾನಾಂತರ);
      ಸ್ಥಿರಾಂಕ vvp_vector2_t*src_obj = reinterpret_cast<vvp_vector2_t*> (src+ಸ್ಥಾನಾಂತರ);
      *dst_obj = *src_obj;
}

ಶೂನ್ಯ property_logic::ಗಣ_vec4(ಅಕ್ಷರ*buf, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val)
{
      vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+ಸ್ಥಾನಾಂತರ);
      *obj = val;
}

ಶೂನ್ಯ property_logic::get_vec4(ಅಕ್ಷರ*buf, vvp_vector4_t&val)
{
      ಸ್ಥಿರಾಂಕ vvp_vector4_t*obj = reinterpret_cast<vvp_vector4_t*> (buf+ಸ್ಥಾನಾಂತರ);
      val = *obj;
}

ಶೂನ್ಯ property_logic::copy(ಅಕ್ಷರ*dst, ಅಕ್ಷರ*src)
{
      vvp_vector4_t*dst_obj = reinterpret_cast<vvp_vector4_t*> (dst+ಸ್ಥಾನಾಂತರ);
      ಸ್ಥಿರಾಂಕ vvp_vector4_t*src_obj = reinterpret_cast<vvp_vector4_t*> (src+ಸ್ಥಾನಾಂತರ);
      *dst_obj = *src_obj;
}

ಮಾದರಿ <ವರ್ಗ T> ಶೂನ್ಯ property_real<T>::ಗಣ_real(ಅಕ್ಷರ*buf, ದ್ವಿಗುಣ val)
{
      T*tmp = reinterpret_cast<T*>(buf+ಸ್ಥಾನಾಂತರ);
      *tmp = val;
}

ಮಾದರಿ <ವರ್ಗ T> ದ್ವಿಗುಣ property_real<T>::get_real(ಅಕ್ಷರ*buf)
{
      T*tmp = reinterpret_cast<T*>(buf+ಸ್ಥಾನಾಂತರ);
      ಹಿಂತಿರುಗಿ *tmp;
}

ಮಾದರಿ <ವರ್ಗ T> ಶೂನ್ಯ property_real<T>::copy(ಅಕ್ಷರ*dst, ಅಕ್ಷರ*src)
{
      T*dst_obj = reinterpret_cast<T*> (dst+ಸ್ಥಾನಾಂತರ);
      T*src_obj = reinterpret_cast<T*> (src+ಸ್ಥಾನಾಂತರ);
      *dst_obj = *src_obj;
}

ಶೂನ್ಯ property_string::ಗಣ_string(ಅಕ್ಷರ*buf, ಸ್ಥಿರಾಂಕ string&val)
{
      string*tmp = reinterpret_cast<string*>(buf+ಸ್ಥಾನಾಂತರ);
      *tmp = val;
}

string property_string::get_string(ಅಕ್ಷರ*buf)
{
      ಸ್ಥಿರಾಂಕ string*tmp = reinterpret_cast<string*>(buf+ಸ್ಥಾನಾಂತರ);
      ಹಿಂತಿರುಗಿ *tmp;
}

ಶೂನ್ಯ property_string::copy(ಅಕ್ಷರ*dst, ಅಕ್ಷರ*src)
{
      string*dst_obj = reinterpret_cast<string*> (dst+ಸ್ಥಾನಾಂತರ);
      ಸ್ಥಿರಾಂಕ string*src_obj = reinterpret_cast<string*> (src+ಸ್ಥಾನಾಂತರ);
      *dst_obj = *src_obj;
}

ಶೂನ್ಯ property_object::construct(ಅಕ್ಷರ*buf) ಸ್ಥಿರಾಂಕ
{
      ಗಾಗಿ (size_t idx = 0 ; idx < array_ಗಾತ್ರ ; idx += 1)
	    ಹೊಸ (buf+ಸ್ಥಾನಾಂತರ + idx*ಗಾತ್ರ(vvp_object_t)) vvp_object_t;
}

ಶೂನ್ಯ property_object::destruct(ಅಕ್ಷರ*buf) ಸ್ಥಿರಾಂಕ
{
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*> (buf+ಸ್ಥಾನಾಂತರ);
      ಗಾಗಿ (size_t idx = 0 ; idx < array_ಗಾತ್ರ ; idx += 1)
	    (tmp+idx)->~vvp_object_t();
}

ಶೂನ್ಯ property_object::ಗಣ_object(ಅಕ್ಷರ*buf, ಸ್ಥಿರಾಂಕ vvp_object_t&val, uint64_t idx)
{
      ಪ್ರತಿಜ್ಞಾ(idx < array_ಗಾತ್ರ);
      vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+ಸ್ಥಾನಾಂತರ);
      tmp[idx] = val;
}

ಶೂನ್ಯ property_object::get_object(ಅಕ್ಷರ*buf, vvp_object_t&val, uint64_t idx)
{
      ಪ್ರತಿಜ್ಞಾ(idx < array_ಗಾತ್ರ);
      ಸ್ಥಿರಾಂಕ vvp_object_t*tmp = reinterpret_cast<vvp_object_t*>(buf+ಸ್ಥಾನಾಂತರ);
      val = tmp[idx];
}

ಶೂನ್ಯ property_object::copy(ಅಕ್ಷರ*dst, ಅಕ್ಷರ*src)
{
      vvp_object_t*dst_obj = reinterpret_cast<vvp_object_t*>(dst+ಸ್ಥಾನಾಂತರ);
      ಸ್ಥಿರಾಂಕ vvp_object_t*src_obj = reinterpret_cast<vvp_object_t*>(src+ಸ್ಥಾನಾಂತರ);
      ಗಾಗಿ (size_t idx = 0 ; idx < array_ಗಾತ್ರ ; idx += 1)
	    dst_obj[idx] = src_obj[idx];
}

/* **** */

class_type::class_type(ಸ್ಥಿರಾಂಕ string&nam, size_t nprop)
: class_name_(nam), properties_(nprop)
{
      instance_ಗಾತ್ರ_2 = 0;
}

class_type::~class_type()
{
      ಗಾಗಿ (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    ಅಳಿಸು properties_[idx].type;
}

ಶೂನ್ಯ class_type::ಗಣ_property(size_t idx, ಸ್ಥಿರಾಂಕ string&name, ಸ್ಥಿರಾಂಕ string&type, uint64_t array_ಗಾತ್ರ_2)
{
      ಪ್ರತಿಜ್ಞಾ(idx < properties_.size());
      properties_[idx].name = name;

      ಆದರೆ (type == "b8")
	    properties_[idx].type = ಹೊಸ property_atom<uint8_t>;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == "b16")
	    properties_[idx].type = ಹೊಸ property_atom<uint16_t>;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == "b32")
	    properties_[idx].type = ಹೊಸ property_atom<uint32_t>;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == "b64")
	    properties_[idx].type = ಹೊಸ property_atom<uint64_t>;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == "sb8")
	    properties_[idx].type = ಹೊಸ property_atom<int8_t>;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == "sb16")
	    properties_[idx].type = ಹೊಸ property_atom<int16_t>;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == "sb32")
	    properties_[idx].type = ಹೊಸ property_atom<int32_t>;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == "sb64")
	    properties_[idx].type = ಹೊಸ property_atom<int64_t>;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == "r")
	    properties_[idx].type = ಹೊಸ property_real<ದ್ವಿಗುಣ>;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == "S")
	    properties_[idx].type = ಹೊಸ property_string;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == "o")
	    properties_[idx].type = ಹೊಸ property_object(array_ಗಾತ್ರ_2);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type[0] == 'b') {
	    size_t wid = strtoul(type.c_str()+1, 0, 0);
	    properties_[idx].type = ಹೊಸ property_ಬಿಟ್(wid);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type[0] == 'L') {
	    size_t wid = strtoul(type.c_str()+1,0,0);
	    properties_[idx].type = ಹೊಸ property_logic(wid);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type[0] == 's' && type[1] == 'L') {
	    size_t wid = strtoul(type.c_str()+2,0,0);
	    properties_[idx].type = ಹೊಸ property_logic(wid);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    properties_[idx].type = 0;
      }
}

ಶೂನ್ಯ class_type::ಮುಕ್ತಾಯ_ಸಿದ್ಧತೆ(ಶೂನ್ಯ)
{
      map<size_t, vector<size_t> > ಗಾತ್ರ_map;
	// Add up all the sizes to get a total instance size. This
	// figures out how much memory a complete instance will need.
      size_t accum = 0;
      ಗಾಗಿ (size_t idx = 0 ; idx < properties_.size() ; idx += 1) {
	    ಪ್ರತಿಜ್ಞಾ(properties_[idx].type);
	    size_t instance_ಗಾತ್ರ = properties_[idx].type->instance_ಗಾತ್ರ();
	    accum += instance_ಗಾತ್ರ;
	    ಗಾತ್ರ_map[instance_ಗಾತ್ರ].push_back(idx);
      }

      instance_ಗಾತ್ರ_2 = accum;

	// Now allocate the properties to offsets within an instance
	// space. Allocate the properties largest objects first so
	// that they are assured better alignment.
      accum = 0;
      ಗಾಗಿ (map<size_t, vector<size_t> >::reverse_iterator cur = ಗಾತ್ರ_map.rbegin()
		 ; cur != ಗಾತ್ರ_map.rend() ; ++ cur) {
	    ಗಾಗಿ (size_t idx = 0 ; idx < cur->second.size() ; idx += 1) {
		  size_t pid = cur->second[idx];
		  class_property_t*ptype = properties_[pid].type;
		  ಪ್ರತಿಜ್ಞಾ(ptype->instance_ಗಾತ್ರ() == cur->ಮೊದಲ);
		  ptype->ಗಣ_ಸ್ಥಾನಾಂತರ(accum);
		  accum += cur->ಮೊದಲ;
	    }
      }
}

class_type::inst_t class_type::instance_new() ಸ್ಥಿರಾಂಕ
{
      ಅಕ್ಷರ*buf = ಹೊಸ ಅಕ್ಷರ [instance_ಗಾತ್ರ_2];

      ಗಾಗಿ (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->construct(buf);

      ಹಿಂತಿರುಗಿ reinterpret_cast<inst_t> (buf);
}

ಶೂನ್ಯ class_type::instance_delete(class_type::inst_t obj) ಸ್ಥಿರಾಂಕ
{
      ಅಕ್ಷರ*buf = reinterpret_cast<ಅಕ್ಷರ*> (obj);

      ಗಾಗಿ (size_t idx = 0 ; idx < properties_.size() ; idx += 1)
	    properties_[idx].type->destruct(buf);

      ಅಳಿಸು[]buf;
}

ಶೂನ್ಯ class_type::ಗಣ_vec4(class_type::inst_t obj, size_t pid,
			  ಸ್ಥಿರಾಂಕ vvp_vector4_t&val) ಸ್ಥಿರಾಂಕ
{
      ಅಕ್ಷರ*buf = reinterpret_cast<ಅಕ್ಷರ*> (obj);
      ಪ್ರತಿಜ್ಞಾ(pid < properties_.size());
      properties_[pid].type->ಗಣ_vec4(buf, val);
}

ಶೂನ್ಯ class_type::get_vec4(class_type::inst_t obj, size_t pid,
			  vvp_vector4_t&val) ಸ್ಥಿರಾಂಕ
{
      ಅಕ್ಷರ*buf = reinterpret_cast<ಅಕ್ಷರ*> (obj);
      ಪ್ರತಿಜ್ಞಾ(pid < properties_.size());
      properties_[pid].type->get_vec4(buf, val);
}

ಶೂನ್ಯ class_type::ಗಣ_real(class_type::inst_t obj, size_t pid,
			  ದ್ವಿಗುಣ val) ಸ್ಥಿರಾಂಕ
{
      ಅಕ್ಷರ*buf = reinterpret_cast<ಅಕ್ಷರ*> (obj);
      ಪ್ರತಿಜ್ಞಾ(pid < properties_.size());
      properties_[pid].type->ಗಣ_real(buf, val);
}

ದ್ವಿಗುಣ class_type::get_real(class_type::inst_t obj, size_t pid) ಸ್ಥಿರಾಂಕ
{
      ಅಕ್ಷರ*buf = reinterpret_cast<ಅಕ್ಷರ*> (obj);
      ಪ್ರತಿಜ್ಞಾ(pid < properties_.size());
      ಹಿಂತಿರುಗಿ properties_[pid].type->get_real(buf);
}

ಶೂನ್ಯ class_type::ಗಣ_string(class_type::inst_t obj, size_t pid,
			    ಸ್ಥಿರಾಂಕ string&val) ಸ್ಥಿರಾಂಕ
{
      ಅಕ್ಷರ*buf = reinterpret_cast<ಅಕ್ಷರ*> (obj);
      ಪ್ರತಿಜ್ಞಾ(pid < properties_.size());
      properties_[pid].type->ಗಣ_string(buf, val);
}

string class_type::get_string(class_type::inst_t obj, size_t pid) ಸ್ಥಿರಾಂಕ
{
      ಅಕ್ಷರ*buf = reinterpret_cast<ಅಕ್ಷರ*> (obj);
      ಪ್ರತಿಜ್ಞಾ(pid < properties_.size());
      ಹಿಂತಿರುಗಿ properties_[pid].type->get_string(buf);
}

ಶೂನ್ಯ class_type::ಗಣ_object(class_type::inst_t obj, size_t pid,
			    ಸ್ಥಿರಾಂಕ vvp_object_t&val, size_t idx) ಸ್ಥಿರಾಂಕ
{
      ಅಕ್ಷರ*buf = reinterpret_cast<ಅಕ್ಷರ*> (obj);
      ಪ್ರತಿಜ್ಞಾ(pid < properties_.size());
      properties_[pid].type->ಗಣ_object(buf, val, idx);
}

ಶೂನ್ಯ class_type::get_object(class_type::inst_t obj, size_t pid,
			    vvp_object_t&val, size_t idx) ಸ್ಥಿರಾಂಕ
{
      ಅಕ್ಷರ*buf = reinterpret_cast<ಅಕ್ಷರ*> (obj);
      ಪ್ರತಿಜ್ಞಾ(pid < properties_.size());
      properties_[pid].type->get_object(buf, val, idx);
}

ಶೂನ್ಯ class_type::copy_property(class_type::inst_t dst, size_t pid, class_type::inst_t src) ಸ್ಥಿರಾಂಕ
{
      ಅಕ್ಷರ*dst_buf = reinterpret_cast<ಅಕ್ಷರ*> (dst);
      ಅಕ್ಷರ*src_buf = reinterpret_cast<ಅಕ್ಷರ*> (src);

      ಪ್ರತಿಜ್ಞಾ(pid < properties_.size());

      properties_[pid].type->copy(dst_buf, src_buf);
}

ಪೂರ್ಣಾಂಕ class_type::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ vpiClassDefn;
}

ಸ್ಥಿರ class_type*compile_class = 0;

ಶೂನ್ಯ compile_class_ಆರಂಭ(ಅಕ್ಷರ*lab, ಅಕ್ಷರ*nam, ಚಿಹ್ನೆರಹಿತ ntype)
{
      ಪ್ರತಿಜ್ಞಾ(compile_class == 0);
      compile_class = ಹೊಸ class_type(nam, ntype);
      compile_vpi_ಚಿಹ್ನೆ(lab, compile_class);
      ಬಿಡುಗಡೆ(lab);
      ಅಳಿಸು[]nam;
}

ಶೂನ್ಯ compile_class_property(ಚಿಹ್ನೆರಹಿತ idx, ಅಕ್ಷರ*nam, ಅಕ್ಷರ*typ, uint64_t array_ಗಾತ್ರ_2)
{
      ಪ್ರತಿಜ್ಞಾ(compile_class);
      compile_class->ಗಣ_property(idx, nam, typ, array_ಗಾತ್ರ_2);
      ಅಳಿಸು[]nam;
      ಅಳಿಸು[]typ;
}

ಶೂನ್ಯ compile_class_ಮುಗಿದ(ಶೂನ್ಯ)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ಪ್ರತಿಜ್ಞಾ(scope);
      ಪ್ರತಿಜ್ಞಾ(compile_class);
      compile_class->ಮುಕ್ತಾಯ_ಸಿದ್ಧತೆ();
      scope->classes[compile_class->class_name()] = compile_class;
      compile_class = 0;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ class_def_delete(class_type *ಅಂಶ)
{
      ಅಳಿಸು ಅಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ
