/*
 * Copyright (c) 2012-2025 Stephen Williams (steve@icarus.com)
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

# include  "vvp_darray.h"
# include  <algorithm>
# include  <functional>
# include  <iostream>
# include  <random>
# include  <typeinfo>

ಬಳಸಿ ನಾಮಾಕಾಶ std;

ನಾಮಾಕಾಶ {

ಸ್ಥಿರ std::mt19937& shuffle_rng(ಶೂನ್ಯ)
{
      ಸ್ಥಿರ std::mt19937 rng(std::random_device{}());
      ಹಿಂತಿರುಗಿ rng;
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ vec4_lt_unsigned(ಸ್ಥಿರಾಂಕ vvp_vector4_t&a, ಸ್ಥಿರಾಂಕ vvp_vector4_t&b)
{
      ಆದರೆ (a.eeq(b)) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      vvp_bit4_t gt = compare_gtge(b, a, BIT4_1);
      ಆದರೆ (gt == BIT4_X) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      ಹಿಂತಿರುಗಿ gt == BIT4_1;
}

} // namespace

vvp_darray::~vvp_darray()
{
}

ಶೂನ್ಯ vvp_darray::ಹಿಮ್ಮುಖ_elems(ಶೂನ್ಯ)
{
      cerr << "XXXX reverse_elems() not implemented for "
           << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_darray::ವಿಂಗಡಿಸು_elems(ತಾರ್ಕಿಕ /*ascending*/)
{
      cerr << "XXXX sort_elems() not implemented for "
           << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_darray::shuffle_elems(ಶೂನ್ಯ)
{
      cerr << "XXXX shuffle_elems() not implemented for "
           << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_darray::ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ, ಸ್ಥಿರಾಂಕ vvp_vector4_t&)
{
      cerr << "XXXX set_word(vvp_vector4_t) not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_darray::ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ, ದ್ವಿಗುಣ)
{
      cerr << "XXXX set_word(double) not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_darray::ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ, ಸ್ಥಿರಾಂಕ string&)
{
      cerr << "XXXX set_word(string) not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_darray::ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ, ಸ್ಥಿರಾಂಕ vvp_object_t&)
{
      cerr << "XXXX set_word(vvp_object_t) not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_darray::get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ, vvp_vector4_t&)
{
      cerr << "XXXX get_word(vvp_vector4_t) not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_darray::get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ, ದ್ವಿಗುಣ&)
{
      cerr << "XXXX get_word(double) not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_darray::get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ, string&)
{
      cerr << "XXXX get_word(string) not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_darray::get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ, vvp_object_t&)
{
      cerr << "XXXX get_word(vvp_object_t) not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

vvp_vector4_t vvp_darray::get_bitstream(ತಾರ್ಕಿಕ)
{
      cerr << "XXXX get_bitstream() not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
      ಹಿಂತಿರುಗಿ vvp_vector4_t();
}

ಮಾದರಿ <ವರ್ಗ TYPE> vvp_darray_atom<TYPE>::~vvp_darray_atom()
{
}

ಮಾದರಿ <ವರ್ಗ TYPE> size_t vvp_darray_atom<TYPE>::get_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ array_.size();
}

ಮಾದರಿ <ವರ್ಗ TYPE> ಶೂನ್ಯ vvp_darray_atom<TYPE>::ಹಿಮ್ಮುಖ_elems(ಶೂನ್ಯ)
{
      size_t n = array_.size();
      ಗಾಗಿ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ಮಾದರಿ <ವರ್ಗ TYPE> ಶೂನ್ಯ vvp_darray_atom<TYPE>::ವಿಂಗಡಿಸು_elems(ತಾರ್ಕಿಕ ascending)
{
      ಆದರೆ (ascending) std::sort(array_.begin(), array_.ಕೊನೆ());
      ಇಲ್ಲದಿದ್ದರೆ std::sort(array_.begin(), array_.ಕೊನೆ(), std::greater<TYPE>());
}

ಮಾದರಿ <ವರ್ಗ TYPE> ಶೂನ್ಯ vvp_darray_atom<TYPE>::shuffle_elems(ಶೂನ್ಯ)
{
      std::shuffle(array_.begin(), array_.ಕೊನೆ(), shuffle_rng());
}

ಮಾದರಿ <ವರ್ಗ TYPE> ಶೂನ್ಯ vvp_darray_atom<TYPE>::ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&value)
{
      ಆದರೆ (adr >= array_.size())
	    ಹಿಂತಿರುಗಿ;
      TYPE tmp;
      vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(value, tmp, ನಿಜ, ಸುಳ್ಳು);
      array_[adr] = tmp;
}

ಮಾದರಿ <ವರ್ಗ TYPE> ಶೂನ್ಯ vvp_darray_atom<TYPE>::get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, vvp_vector4_t&value)
{
      ಆದರೆ (adr >= array_.size()) {
	    value = vvp_vector4_t(8*ಗಾತ್ರ(TYPE), BIT4_X);
	    ಹಿಂತಿರುಗಿ;
      }

      TYPE ಶಬ್ದ = array_[adr];
      vvp_vector4_t tmp (8*ಗಾತ್ರ(TYPE), BIT4_0);
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < tmp.size() ; idx += 1) {
	    ಆದರೆ (ಶಬ್ದ&1) tmp.ಗಣ_ಬಿಟ್(idx, BIT4_1);
	    ಶಬ್ದ >>= 1;
      }
      value = tmp;
}

ಮಾದರಿ <ವರ್ಗ TYPE> ಶೂನ್ಯ vvp_darray_atom<TYPE>::shallow_copy(ಸ್ಥಿರಾಂಕ vvp_object*obj)
{
      ಸ್ಥಿರಾಂಕ vvp_darray_atom<TYPE>*that = dynamic_cast<ಸ್ಥಿರಾಂಕ vvp_darray_atom<TYPE>*>(obj);
      ಪ್ರತಿಜ್ಞಾ(that);

      ಚಿಹ್ನೆರಹಿತ ಅಂಕೆ_ವಸ್ತುಗಳು = min(array_.size(), that->array_.size());
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಅಂಕೆ_ವಸ್ತುಗಳು ; idx += 1)
	    array_[idx] = that->array_[idx];
}

ಮಾದರಿ <ವರ್ಗ TYPE> vvp_object* vvp_darray_atom<TYPE>::duplicate(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{
      vvp_darray_atom<TYPE>*that = ಹೊಸ vvp_darray_atom<TYPE>(array_.size());
      ಗಾಗಿ (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      ಹಿಂತಿರುಗಿ that;
}

ಮಾದರಿ <ವರ್ಗ TYPE> vvp_vector4_t vvp_darray_atom<TYPE>::get_bitstream(ತಾರ್ಕಿಕ)
{
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಶಬ್ದ_wid = ಗಾತ್ರ(TYPE) * 8;

      vvp_vector4_t vec(array_.size() * ಶಬ್ದ_wid, BIT4_0);

      ಚಿಹ್ನೆರಹಿತ adx = 0;
      ಚಿಹ್ನೆರಹಿತ vdx = vec.size();
      ಆಗ (vdx > 0) {
            TYPE ಶಬ್ದ = array_[adx++];
            vdx -= ಶಬ್ದ_wid;
            ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ bdx = 0; bdx < ಶಬ್ದ_wid; bdx += 1) {
                  ಆದರೆ (ಶಬ್ದ & 1)
                        vec.ಗಣ_ಬಿಟ್(vdx+bdx, BIT4_1);
                  ಶಬ್ದ >>= 1;
            }
      }

      ಹಿಂತಿರುಗಿ vec;
}

ಮಾದರಿ ವರ್ಗ vvp_darray_atom<uint8_t>;
ಮಾದರಿ ವರ್ಗ vvp_darray_atom<uint16_t>;
ಮಾದರಿ ವರ್ಗ vvp_darray_atom<uint32_t>;
ಮಾದರಿ ವರ್ಗ vvp_darray_atom<uint64_t>;
ಮಾದರಿ ವರ್ಗ vvp_darray_atom<int8_t>;
ಮಾದರಿ ವರ್ಗ vvp_darray_atom<int16_t>;
ಮಾದರಿ ವರ್ಗ vvp_darray_atom<int32_t>;
ಮಾದರಿ ವರ್ಗ vvp_darray_atom<int64_t>;

vvp_darray_vec4::~vvp_darray_vec4()
{
}

size_t vvp_darray_vec4::get_ಗಾತ್ರ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ array_.size();
}

ಶೂನ್ಯ vvp_darray_vec4::ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&value)
{
      ಆದರೆ (adr >= array_.size()) ಹಿಂತಿರುಗಿ;
      ಪ್ರತಿಜ್ಞಾ(value.size() == ಶಬ್ದ_wid_2);
      array_[adr] = value;
}

ಶೂನ್ಯ vvp_darray_vec4::get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, vvp_vector4_t&value)
{
	/*
	 * Return an undefined value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      ಆದರೆ ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(ಶಬ್ದ_wid_2, BIT4_X);
	    ಹಿಂತಿರುಗಿ;
      }
      value = array_[adr];
      ಪ್ರತಿಜ್ಞಾ(value.size() == ಶಬ್ದ_wid_2);
}

ಶೂನ್ಯ vvp_darray_vec4::shallow_copy(ಸ್ಥಿರಾಂಕ vvp_object*obj)
{
      ಸ್ಥಿರಾಂಕ vvp_darray_vec4*that = dynamic_cast<ಸ್ಥಿರಾಂಕ vvp_darray_vec4*>(obj);
      ಪ್ರತಿಜ್ಞಾ(that);

      ಚಿಹ್ನೆರಹಿತ ಅಂಕೆ_ವಸ್ತುಗಳು = min(array_.size(), that->array_.size());
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಅಂಕೆ_ವಸ್ತುಗಳು ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_object* vvp_darray_vec4::duplicate(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{
      vvp_darray_vec4*that = ಹೊಸ vvp_darray_vec4(array_.size(), ಶಬ್ದ_wid_2);

      ಗಾಗಿ (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      ಹಿಂತಿರುಗಿ that;
}

vvp_vector4_t vvp_darray_vec4::get_bitstream(ತಾರ್ಕಿಕ as_vec4)
{
      vvp_vector4_t vec(array_.size() * ಶಬ್ದ_wid_2, BIT4_0);

      ಚಿಹ್ನೆರಹಿತ adx = 0;
      ಚಿಹ್ನೆರಹಿತ vdx = vec.size();
      ಆಗ (vdx > 0) {
            vdx -= ಶಬ್ದ_wid_2;
            ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ bdx = 0; bdx < ಶಬ್ದ_wid_2; bdx += 1) {
                  vvp_bit4_t ಬಿಟ್ = array_[adx].value(bdx);
                  ಆದರೆ (as_vec4 || (ಬಿಟ್ == BIT4_1))
                        vec.ಗಣ_ಬಿಟ್(vdx+bdx, ಬಿಟ್);
            }
            adx++;
      }

      ಹಿಂತಿರುಗಿ vec;
}

ಶೂನ್ಯ vvp_darray_vec4::ಹಿಮ್ಮುಖ_elems(ಶೂನ್ಯ)
{
      size_t n = array_.size();
      ಗಾಗಿ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ಶೂನ್ಯ vvp_darray_vec4::ವಿಂಗಡಿಸು_elems(ತಾರ್ಕಿಕ ascending)
{
      ಆದರೆ (ascending) {
	    std::sort(array_.begin(), array_.ಕೊನೆ(), vec4_lt_unsigned);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    std::sort(array_.begin(), array_.ಕೊನೆ(),
		      [](ಸ್ಥಿರಾಂಕ vvp_vector4_t&a, ಸ್ಥಿರಾಂಕ vvp_vector4_t&b) {
			    ಹಿಂತಿರುಗಿ vec4_lt_unsigned(b, a);
		      });
      }
}

ಶೂನ್ಯ vvp_darray_vec4::shuffle_elems(ಶೂನ್ಯ)
{
      std::shuffle(array_.begin(), array_.ಕೊನೆ(), shuffle_rng());
}

ಶೂನ್ಯ vvp_darray_vec2::ಹಿಮ್ಮುಖ_elems(ಶೂನ್ಯ)
{
      size_t n = array_.size();
      ಗಾಗಿ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ಶೂನ್ಯ vvp_darray_vec2::ವಿಂಗಡಿಸು_elems(ತಾರ್ಕಿಕ ascending)
{
      ಆದರೆ (ascending) {
	    std::sort(array_.begin(), array_.ಕೊನೆ());
      } ಇಲ್ಲದಿದ್ದರೆ {
	    std::sort(array_.begin(), array_.ಕೊನೆ(),
		      [](ಸ್ಥಿರಾಂಕ vvp_vector2_t&a, ಸ್ಥಿರಾಂಕ vvp_vector2_t&b) {
			    ಹಿಂತಿರುಗಿ b < a;
		      });
      }
}

ಶೂನ್ಯ vvp_darray_vec2::shuffle_elems(ಶೂನ್ಯ)
{
      std::shuffle(array_.begin(), array_.ಕೊನೆ(), shuffle_rng());
}

vvp_darray_vec2::~vvp_darray_vec2()
{
}

size_t vvp_darray_vec2::get_ಗಾತ್ರ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ array_.size();
}

ಶೂನ್ಯ vvp_darray_vec2::ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&value)
{
      ಆದರೆ (adr >= array_.size()) ಹಿಂತಿರುಗಿ;
      ಪ್ರತಿಜ್ಞಾ(value.size() == ಶಬ್ದ_wid_2);
      array_[adr] = value;
}

ಶೂನ್ಯ vvp_darray_vec2::get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, vvp_vector4_t&value)
{
	/*
	 * Return a zero value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      ಆದರೆ ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(ಶಬ್ದ_wid_2, BIT4_0);
	    ಹಿಂತಿರುಗಿ;
      }
      ಪ್ರತಿಜ್ಞಾ(array_[adr].size() == ಶಬ್ದ_wid_2);
      value.resize(ಶಬ್ದ_wid_2);
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < ಶಬ್ದ_wid_2; idx += 1) {
	    value.ಗಣ_ಬಿಟ್(idx, array_[adr].value4(idx));
      }
}

ಶೂನ್ಯ vvp_darray_vec2::shallow_copy(ಸ್ಥಿರಾಂಕ vvp_object*obj)
{
      ಸ್ಥಿರಾಂಕ vvp_darray_vec2*that = dynamic_cast<ಸ್ಥಿರಾಂಕ vvp_darray_vec2*>(obj);
      ಪ್ರತಿಜ್ಞಾ(that);

      ಚಿಹ್ನೆರಹಿತ ಅಂಕೆ_ವಸ್ತುಗಳು = min(array_.size(), that->array_.size());
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಅಂಕೆ_ವಸ್ತುಗಳು ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_vector4_t vvp_darray_vec2::get_bitstream(ತಾರ್ಕಿಕ)
{
      vvp_vector4_t vec(array_.size() * ಶಬ್ದ_wid_2, BIT4_0);

      ಚಿಹ್ನೆರಹಿತ adx = 0;
      ಚಿಹ್ನೆರಹಿತ vdx = vec.size();
      ಆಗ (vdx > 0) {
            vdx -= ಶಬ್ದ_wid_2;
            ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ bdx = 0; bdx < ಶಬ್ದ_wid_2; bdx += 1) {
                  ಆದರೆ (array_[adx].value(bdx))
                        vec.ಗಣ_ಬಿಟ್(vdx+bdx, BIT4_1);
            }
            adx++;
      }

      ಹಿಂತಿರುಗಿ vec;
}

vvp_darray_object::~vvp_darray_object()
{
}

size_t vvp_darray_object::get_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ array_.size();
}

ಶೂನ್ಯ vvp_darray_object::ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ vvp_object_t&value)
{
      ಆದರೆ (adr >= array_.size())
	    ಹಿಂತಿರುಗಿ;
      array_[adr] = value;
}

ಶೂನ್ಯ vvp_darray_object::get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, vvp_object_t&value)
{
      ಆದರೆ (adr >= array_.size()) {
	    value = vvp_object_t();
	    ಹಿಂತಿರುಗಿ;
      }

      value = array_[adr];
}

ಶೂನ್ಯ vvp_darray_object::shallow_copy(ಸ್ಥಿರಾಂಕ vvp_object*obj)
{
      ಸ್ಥಿರಾಂಕ vvp_darray_object*that = dynamic_cast<ಸ್ಥಿರಾಂಕ vvp_darray_object*>(obj);
      ಪ್ರತಿಜ್ಞಾ(that);

      ಚಿಹ್ನೆರಹಿತ ಅಂಕೆ_ವಸ್ತುಗಳು = min(array_.size(), that->array_.size());
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಅಂಕೆ_ವಸ್ತುಗಳು ; idx += 1)
	    array_[idx] = that->array_[idx];
}

ಶೂನ್ಯ vvp_darray_object::ಹಿಮ್ಮುಖ_elems(ಶೂನ್ಯ)
{
      size_t n = array_.size();
      ಗಾಗಿ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ಶೂನ್ಯ vvp_darray_object::ವಿಂಗಡಿಸು_elems(ತಾರ್ಕಿಕ ascending)
{
      ಆದರೆ (ascending) {
	    std::sort(array_.begin(), array_.ಕೊನೆ(),
		      [](ಸ್ಥಿರಾಂಕ vvp_object_t&a, ಸ್ಥಿರಾಂಕ vvp_object_t&b) {
			    ಹಿಂತಿರುಗಿ a.ವಿಂಗಡಿಸು_ಕ್ರಮ_ಕೀಲಿ() < b.ವಿಂಗಡಿಸು_ಕ್ರಮ_ಕೀಲಿ();
		      });
      } ಇಲ್ಲದಿದ್ದರೆ {
	    std::sort(array_.begin(), array_.ಕೊನೆ(),
		      [](ಸ್ಥಿರಾಂಕ vvp_object_t&a, ಸ್ಥಿರಾಂಕ vvp_object_t&b) {
			    ಹಿಂತಿರುಗಿ a.ವಿಂಗಡಿಸು_ಕ್ರಮ_ಕೀಲಿ() > b.ವಿಂಗಡಿಸು_ಕ್ರಮ_ಕೀಲಿ();
		      });
      }
}

ಶೂನ್ಯ vvp_darray_object::shuffle_elems(ಶೂನ್ಯ)
{
      std::shuffle(array_.begin(), array_.ಕೊನೆ(), shuffle_rng());
}

vvp_darray_real::~vvp_darray_real()
{
}

size_t vvp_darray_real::get_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ array_.size();
}

ಶೂನ್ಯ vvp_darray_real::ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ದ್ವಿಗುಣ value)
{
      ಆದರೆ (adr >= array_.size())
	    ಹಿಂತಿರುಗಿ;
      array_[adr] = value;
}

ಶೂನ್ಯ vvp_darray_real::get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ದ್ವಿಗುಣ&value)
{
      ಆದರೆ (adr >= array_.size()) {
	    value = 0.0;
	    ಹಿಂತಿರುಗಿ;
      }

      value = array_[adr];
}

ಶೂನ್ಯ vvp_darray_real::shallow_copy(ಸ್ಥಿರಾಂಕ vvp_object*obj)
{
      ಸ್ಥಿರಾಂಕ vvp_darray_real*that = dynamic_cast<ಸ್ಥಿರಾಂಕ vvp_darray_real*>(obj);
      ಪ್ರತಿಜ್ಞಾ(that);

      ಚಿಹ್ನೆರಹಿತ ಅಂಕೆ_ವಸ್ತುಗಳು = min(array_.size(), that->array_.size());
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಅಂಕೆ_ವಸ್ತುಗಳು ; idx += 1)
	    array_[idx] = that->array_[idx];
}

ಶೂನ್ಯ vvp_darray_real::ಹಿಮ್ಮುಖ_elems(ಶೂನ್ಯ)
{
      size_t n = array_.size();
      ಗಾಗಿ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ಶೂನ್ಯ vvp_darray_real::ವಿಂಗಡಿಸು_elems(ತಾರ್ಕಿಕ ascending)
{
      ಆದರೆ (ascending) std::sort(array_.begin(), array_.ಕೊನೆ());
      ಇಲ್ಲದಿದ್ದರೆ std::sort(array_.begin(), array_.ಕೊನೆ(), std::greater<ದ್ವಿಗುಣ>());
}

ಶೂನ್ಯ vvp_darray_real::shuffle_elems(ಶೂನ್ಯ)
{
      std::shuffle(array_.begin(), array_.ಕೊನೆ(), shuffle_rng());
}

vvp_object* vvp_darray_real::duplicate(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{
      vvp_darray_real*that = ಹೊಸ vvp_darray_real(array_.size());

      ಗಾಗಿ (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      ಹಿಂತಿರುಗಿ that;
}

vvp_vector4_t vvp_darray_real::get_bitstream(ತಾರ್ಕಿಕ)
{
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಶಬ್ದ_wid = ಗಾತ್ರ(ದ್ವಿಗುಣ) * 8;
      ಪ್ರತಿಜ್ಞಾ(ಶಬ್ದ_wid == 64);

      vvp_vector4_t vec(array_.size() * ಶಬ್ದ_wid, BIT4_0);

      ಚಿಹ್ನೆರಹಿತ adx = 0;
      ಚಿಹ್ನೆರಹಿತ vdx = vec.size();
      ಆಗ (vdx > 0) {
            ಒಕ್ಕೂಟ {
                ದ್ವಿಗುಣ   value;
                uint64_t ಬಿಟ್ಟುಗಳು;
            } ಶಬ್ದ;
            ಶಬ್ದ.value = array_[adx++];
            vdx -= ಶಬ್ದ_wid;
            ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ bdx = 0; bdx < ಶಬ್ದ_wid; bdx += 1) {
                  ಆದರೆ (ಶಬ್ದ.ಬಿಟ್ಟುಗಳು & 1)
                        vec.ಗಣ_ಬಿಟ್(vdx+bdx, BIT4_1);
                  ಶಬ್ದ.ಬಿಟ್ಟುಗಳು >>= 1;
            }
      }

      ಹಿಂತಿರುಗಿ vec;
}

vvp_darray_string::~vvp_darray_string()
{
}

size_t vvp_darray_string::get_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ array_.size();
}

ಶೂನ್ಯ vvp_darray_string::ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ string&value)
{
      ಆದರೆ (adr >= array_.size())
	    ಹಿಂತಿರುಗಿ;
      array_[adr] = value;
}

ಶೂನ್ಯ vvp_darray_string::get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, string&value)
{
      ಆದರೆ (adr >= array_.size()) {
	    value = "";
	    ಹಿಂತಿರುಗಿ;
      }

      value = array_[adr];
}

ಶೂನ್ಯ vvp_darray_string::shallow_copy(ಸ್ಥಿರಾಂಕ vvp_object*obj)
{
      ಸ್ಥಿರಾಂಕ vvp_darray_string*that = dynamic_cast<ಸ್ಥಿರಾಂಕ vvp_darray_string*>(obj);
      ಪ್ರತಿಜ್ಞಾ(that);

      ಚಿಹ್ನೆರಹಿತ ಅಂಕೆ_ವಸ್ತುಗಳು = min(array_.size(), that->array_.size());
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಅಂಕೆ_ವಸ್ತುಗಳು ; idx += 1)
	    array_[idx] = that->array_[idx];
}

ಶೂನ್ಯ vvp_darray_string::ಹಿಮ್ಮುಖ_elems(ಶೂನ್ಯ)
{
      size_t n = array_.size();
      ಗಾಗಿ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ಶೂನ್ಯ vvp_darray_string::ವಿಂಗಡಿಸು_elems(ತಾರ್ಕಿಕ ascending)
{
      ಆದರೆ (ascending) std::sort(array_.begin(), array_.ಕೊನೆ());
      ಇಲ್ಲದಿದ್ದರೆ std::sort(array_.begin(), array_.ಕೊನೆ(), std::greater<string>());
}

ಶೂನ್ಯ vvp_darray_string::shuffle_elems(ಶೂನ್ಯ)
{
      std::shuffle(array_.begin(), array_.ಕೊನೆ(), shuffle_rng());
}

vvp_object* vvp_darray_string::duplicate(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{
      vvp_darray_string*that = ಹೊಸ vvp_darray_string(array_.size());

      ಗಾಗಿ (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      ಹಿಂತಿರುಗಿ that;
}

vvp_ಸರತಿ::~vvp_ಸರತಿ()
{
}

ಶೂನ್ಯ vvp_ಸರತಿ::copy_elems(vvp_object_t, ಚಿಹ್ನೆರಹಿತ)
{
      cerr << "Sorry: copy_elems() not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_ಸರತಿ::ಗಣ_ಶಬ್ದ_max(ಚಿಹ್ನೆರಹಿತ, ಸ್ಥಿರಾಂಕ vvp_vector4_t&, ಚಿಹ್ನೆರಹಿತ)
{
      cerr << "XXXX set_word_max(vvp_vector4_t) not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_ಸರತಿ::insert(ಚಿಹ್ನೆರಹಿತ, ಸ್ಥಿರಾಂಕ vvp_vector4_t&, ಚಿಹ್ನೆರಹಿತ)
{
      cerr << "XXXX insert(vvp_vector4_t) not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_ಸರತಿ::push_back(ಸ್ಥಿರಾಂಕ vvp_vector4_t&, ಚಿಹ್ನೆರಹಿತ)
{
      cerr << "XXXX push_back(vvp_vector4_t) not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_ಸರತಿ::push_front(ಸ್ಥಿರಾಂಕ vvp_vector4_t&, ಚಿಹ್ನೆರಹಿತ)
{
      cerr << "XXXX push_front(vvp_vector4_t) not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_ಸರತಿ::ಗಣ_ಶಬ್ದ_max(ಚಿಹ್ನೆರಹಿತ, ದ್ವಿಗುಣ, ಚಿಹ್ನೆರಹಿತ)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_ಸರತಿ::insert(ಚಿಹ್ನೆರಹಿತ, ದ್ವಿಗುಣ, ಚಿಹ್ನೆರಹಿತ)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_ಸರತಿ::push_back(ದ್ವಿಗುಣ, ಚಿಹ್ನೆರಹಿತ)
{
      cerr << "XXXX push_back(double) not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_ಸರತಿ::push_front(ದ್ವಿಗುಣ, ಚಿಹ್ನೆರಹಿತ)
{
      cerr << "XXXX push_front(double) not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_ಸರತಿ::ಗಣ_ಶಬ್ದ_max(ಚಿಹ್ನೆರಹಿತ, ಸ್ಥಿರಾಂಕ string&, ಚಿಹ್ನೆರಹಿತ)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_ಸರತಿ::insert(ಚಿಹ್ನೆರಹಿತ, ಸ್ಥಿರಾಂಕ string&, ಚಿಹ್ನೆರಹಿತ)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_ಸರತಿ::push_back(ಸ್ಥಿರಾಂಕ string&, ಚಿಹ್ನೆರಹಿತ)
{
      cerr << "XXXX push_back(string) not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_ಸರತಿ::push_front(ಸ್ಥಿರಾಂಕ string&, ಚಿಹ್ನೆರಹಿತ)
{
      cerr << "XXXX push_front(string) not implemented for " << typeid(*ಇದು).name() << ಸಾಲು_ಅಂತ್ಯ;
}

/*
 * Helper functions used while copying multiple elements into a queue.
 */
ಸ್ಥಿರ ಶೂನ್ಯ print_copy_is_too_big(size_t src_ಗಾತ್ರ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ, ಸ್ಥಿರಾಂಕ string&qtype)
{
      cerr << get_fileline()
           << "Warning: queue<" << qtype << "> is bounded to have at most "
           << max_ಗಾತ್ರ << " elements, source has " << src_ಗಾತ್ರ << " elements." << ಸಾಲು_ಅಂತ್ಯ;
}

ಸ್ಥಿರ ಶೂನ್ಯ print_copy_is_too_big(ದ್ವಿಗುಣ&, size_t src_ಗಾತ್ರ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ)
{
      print_copy_is_too_big(src_ಗಾತ್ರ, max_ಗಾತ್ರ, "real");
}

ಸ್ಥಿರ ಶೂನ್ಯ print_copy_is_too_big(string&, size_t src_ಗಾತ್ರ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ)
{
      print_copy_is_too_big(src_ಗಾತ್ರ, max_ಗಾತ್ರ, "string");
}

ಸ್ಥಿರ ಶೂನ್ಯ print_copy_is_too_big(vvp_vector4_t&, size_t src_ಗಾತ್ರ, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ)
{
      print_copy_is_too_big(src_ಗಾತ್ರ, max_ಗಾತ್ರ, "vector");
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು ELEM, ವರ್ಗ QTYPE, ವರ್ಗ SRC_TYPE>
ಸ್ಥಿರ ಶೂನ್ಯ copy_elements(QTYPE*ಸರತಿ, SRC_TYPE*src, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ)
{
      size_t src_ಗಾತ್ರ = src->get_ಗಾತ್ರ();
      ಆದರೆ ((max_ಗಾತ್ರ != 0) && (src_ಗಾತ್ರ > max_ಗಾತ್ರ)) {
	    ELEM tmp;
	    print_copy_is_too_big(tmp, src_ಗಾತ್ರ, max_ಗಾತ್ರ);
      }
      ಚಿಹ್ನೆರಹಿತ copy_ಗಾತ್ರ = ((src_ಗಾತ್ರ < max_ಗಾತ್ರ) ||
                            (max_ಗಾತ್ರ == 0)) ? src_ಗಾತ್ರ : max_ಗಾತ್ರ;
      ಆದರೆ (copy_ಗಾತ್ರ < ಸರತಿ->get_ಗಾತ್ರ())
	    ಸರತಿ->erase_ಬಾಲ(copy_ಗಾತ್ರ);
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx=0; idx < copy_ಗಾತ್ರ; ++idx) {
	    ELEM value;
	    src->get_ಶಬ್ದ(idx, value);
	    ಸರತಿ->ಗಣ_ಶಬ್ದ_max(idx, value, max_ಗಾತ್ರ);
      }
}

ಶೂನ್ಯ vvp_ಸರತಿ_real::copy_elems(vvp_object_t src, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ)
{
      ಆದರೆ (vvp_ಸರತಿ*src_ಸರತಿ = src.peek<vvp_ಸರತಿ>())
	    copy_elements<ದ್ವಿಗುಣ, vvp_ಸರತಿ_real, vvp_ಸರತಿ>(ಇದು, src_ಸರತಿ, max_ಗಾತ್ರ);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<ದ್ವಿಗುಣ, vvp_ಸರತಿ_real, vvp_darray>(ಇದು, src_darray, max_ಗಾತ್ರ);
      ಇಲ್ಲದಿದ್ದರೆ
	    cerr << get_fileline() << "Sorry: cannot copy object to real queue." << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_ಸರತಿ_real::ಗಣ_ಶಬ್ದ_max(ಚಿಹ್ನೆರಹಿತ adr, ದ್ವಿಗುಣ value, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ)
{
      ಆದರೆ (adr == ಸರತಿ.size())
	    ಆದರೆ (!max_ಗಾತ್ರ || (ಸರತಿ.size() < max_ಗಾತ್ರ))
		  ಸರತಿ.push_back(value);
	    ಇಲ್ಲದಿದ್ದರೆ
		  cerr << get_fileline()
		       << "Warning: assigning to queue<real>[" << adr << "] is"
		          " outside bound (" << max_ಗಾತ್ರ << "). " << value
		       << " was not added." << ಸಾಲು_ಅಂತ್ಯ;
      ಇಲ್ಲದಿದ್ದರೆ
	    ಗಣ_ಶಬ್ದ(adr, value);
}

ಶೂನ್ಯ vvp_ಸರತಿ_real::ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ದ್ವಿಗುಣ value)
{
      ಆದರೆ (adr < ಸರತಿ.size())
	    ಸರತಿ[adr] = value;
      ಇಲ್ಲದಿದ್ದರೆ
	    cerr << get_fileline()
	         << "Warning: assigning to queue<real>[" << adr << "] is outside "
	            "of size (" << ಸರತಿ.size() << "). " << value
	         << " was not added." << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_ಸರತಿ_real::get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ದ್ವಿಗುಣ&value)
{
      ಆದರೆ (adr >= ಸರತಿ.size())
	    value = 0.0;
      ಇಲ್ಲದಿದ್ದರೆ
	    value = ಸರತಿ[adr];
}

ಶೂನ್ಯ vvp_ಸರತಿ_real::insert(ಚಿಹ್ನೆರಹಿತ idx, ದ್ವಿಗುಣ value, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ)
{
	// Inserting past the end of the queue
      ಆದರೆ (idx > ಸರತಿ.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<real>[" << idx << "] is "
	            "outside of size (" << ಸರತಿ.size() << "). " << value
	         << " was not added." << ಸಾಲು_ಅಂತ್ಯ;
	// Inserting at the end
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx == ಸರತಿ.size())
	    ಆದರೆ (!max_ಗಾತ್ರ || (ಸರತಿ.size() < max_ಗಾತ್ರ))
		  ಸರತಿ.push_back(value);
	    ಇಲ್ಲದಿದ್ದರೆ
		  cerr << get_fileline()
		       << "Warning: inserting to queue<real>[" << idx << "] is"
		          " outside bound (" << max_ಗಾತ್ರ << "). " << value
		       << " was not added." << ಸಾಲು_ಅಂತ್ಯ;
      ಇಲ್ಲದಿದ್ದರೆ  {
	    ಆದರೆ (max_ಗಾತ್ರ && (ಸರತಿ.size() == max_ಗಾತ್ರ)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << ಸರತಿ.back() << " from already full bounded queue<real> ["
		       << max_ಗಾತ್ರ << "]." << ಸಾಲು_ಅಂತ್ಯ;
		  ಸರತಿ.pop_back();
	    }
	    ಸರತಿ.insert(ಸರತಿ.begin()+idx, value);
      }
}

ಶೂನ್ಯ vvp_ಸರತಿ_real::push_back(ದ್ವಿಗುಣ value, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ)
{
      ಆದರೆ (!max_ಗಾತ್ರ || (ಸರತಿ.size() < max_ಗಾತ್ರ))
	    ಸರತಿ.push_back(value);
      ಇಲ್ಲದಿದ್ದರೆ
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<real> ["
	         << max_ಗಾತ್ರ << "]." << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_ಸರತಿ_real::push_front(ದ್ವಿಗುಣ value, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ)
{
      ಆದರೆ (max_ಗಾತ್ರ && (ಸರತಿ.size() == max_ಗಾತ್ರ)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << ಸರತಿ.back() << " from already full bounded queue<real> ["
	         << max_ಗಾತ್ರ << "]." << ಸಾಲು_ಅಂತ್ಯ;
	    ಸರತಿ.pop_back();
      }
      ಸರತಿ.push_front(value);
}

ಶೂನ್ಯ vvp_ಸರತಿ_real::erase(ಚಿಹ್ನೆರಹಿತ idx)
{
      ಪ್ರತಿಜ್ಞಾ(ಸರತಿ.size() > idx);
      ಸರತಿ.erase(ಸರತಿ.begin()+idx);
}

ಶೂನ್ಯ vvp_ಸರತಿ_real::erase_ಬಾಲ(ಚಿಹ್ನೆರಹಿತ idx)
{
      ಪ್ರತಿಜ್ಞಾ(ಸರತಿ.size() >= idx);
      ಆದರೆ (ಸರತಿ.size() > idx)
	    ಸರತಿ.resize(idx);
}

ಶೂನ್ಯ vvp_ಸರತಿ_string::copy_elems(vvp_object_t src, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ)
{
      ಆದರೆ (vvp_ಸರತಿ*src_ಸರತಿ = src.peek<vvp_ಸರತಿ>())
	    copy_elements<string, vvp_ಸರತಿ_string, vvp_ಸರತಿ>(ಇದು, src_ಸರತಿ, max_ಗಾತ್ರ);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<string, vvp_ಸರತಿ_string, vvp_darray>(ಇದು, src_darray, max_ಗಾತ್ರ);
      ಇಲ್ಲದಿದ್ದರೆ
	    cerr << get_fileline() << "Sorry: cannot copy object to string queue." << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_ಸರತಿ_string::ಗಣ_ಶಬ್ದ_max(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ string&value, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ)
{
      ಆದರೆ (adr == ಸರತಿ.size())
	    ಆದರೆ (!max_ಗಾತ್ರ || (ಸರತಿ.size() < max_ಗಾತ್ರ))
		  ಸರತಿ.push_back(value);
	    ಇಲ್ಲದಿದ್ದರೆ
		  cerr << get_fileline()
		       << "Warning: assigning to queue<string>[" << adr << "] is"
		          " outside bound (" << max_ಗಾತ್ರ << "). \"" << value
		       << "\" was not added." << ಸಾಲು_ಅಂತ್ಯ;
      ಇಲ್ಲದಿದ್ದರೆ
	    ಗಣ_ಶಬ್ದ(adr, value);
}

ಶೂನ್ಯ vvp_ಸರತಿ_string::ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ string&value)
{
      ಆದರೆ (adr < ಸರತಿ.size())
	    ಸರತಿ[adr] = value;
      ಇಲ್ಲದಿದ್ದರೆ
	    cerr << get_fileline()
	         << "Warning: assigning to queue<string>[" << adr << "] is outside "
	            "of size (" << ಸರತಿ.size() << "). \"" << value
	         << "\" was not added." << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_ಸರತಿ_string::get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, string&value)
{
      ಆದರೆ (adr >= ಸರತಿ.size())
	    value = "";
      ಇಲ್ಲದಿದ್ದರೆ
	    value = ಸರತಿ[adr];
}

ಶೂನ್ಯ vvp_ಸರತಿ_string::insert(ಚಿಹ್ನೆರಹಿತ idx, ಸ್ಥಿರಾಂಕ string&value, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ)
{
	// Inserting past the end of the queue
      ಆದರೆ (idx > ಸರತಿ.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<string>[" << idx << "] is "
	            "outside of size (" << ಸರತಿ.size() << "). \"" << value
	         << "\" was not added." << ಸಾಲು_ಅಂತ್ಯ;
	// Inserting at the end
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx == ಸರತಿ.size())
	    ಆದರೆ (!max_ಗಾತ್ರ || (ಸರತಿ.size() < max_ಗಾತ್ರ))
		  ಸರತಿ.push_back(value);
	    ಇಲ್ಲದಿದ್ದರೆ
		  cerr << get_fileline()
		       << "Warning: inserting to queue<string>[" << idx << "] is"
		          " outside bound (" << max_ಗಾತ್ರ << "). \"" << value
		       << "\" was not added." << ಸಾಲು_ಅಂತ್ಯ;
      ಇಲ್ಲದಿದ್ದರೆ  {
	    ಆದರೆ (max_ಗಾತ್ರ && (ಸರತಿ.size() == max_ಗಾತ್ರ)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", \"" << value << "\") removed \""
		       << ಸರತಿ.back() << "\" from already full bounded queue<string> ["
		       << max_ಗಾತ್ರ << "]." << ಸಾಲು_ಅಂತ್ಯ;
		  ಸರತಿ.pop_back();
	    }
	    ಸರತಿ.insert(ಸರತಿ.begin()+idx, value);
      }
}

ಶೂನ್ಯ vvp_ಸರತಿ_string::push_back(ಸ್ಥಿರಾಂಕ string&value, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ)
{
      ಆದರೆ (!max_ಗಾತ್ರ || (ಸರತಿ.size() < max_ಗಾತ್ರ))
	    ಸರತಿ.push_back(value);
      ಇಲ್ಲದಿದ್ದರೆ
	    cerr << get_fileline()
	         << "Warning: push_back(\"" << value
	         << "\") skipped for already full bounded queue<string> ["
	         << max_ಗಾತ್ರ << "]." << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_ಸರತಿ_string::push_front(ಸ್ಥಿರಾಂಕ string&value, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ)
{
      ಆದರೆ (max_ಗಾತ್ರ && (ಸರತಿ.size() == max_ಗಾತ್ರ)) {
	    cerr << get_fileline()
	         << "Warning: push_front(\"" << value << "\") removed \""
	         << ಸರತಿ.back() << "\" from already full bounded queue<string> ["
	         << max_ಗಾತ್ರ << "]." << ಸಾಲು_ಅಂತ್ಯ;
	    ಸರತಿ.pop_back();
      }
      ಸರತಿ.push_front(value);
}

ಶೂನ್ಯ vvp_ಸರತಿ_string::erase(ಚಿಹ್ನೆರಹಿತ idx)
{
      ಪ್ರತಿಜ್ಞಾ(ಸರತಿ.size() > idx);
      ಸರತಿ.erase(ಸರತಿ.begin()+idx);
}

ಶೂನ್ಯ vvp_ಸರತಿ_string::erase_ಬಾಲ(ಚಿಹ್ನೆರಹಿತ idx)
{
      ಪ್ರತಿಜ್ಞಾ(ಸರತಿ.size() >= idx);
      ಆದರೆ (ಸರತಿ.size() > idx)
	    ಸರತಿ.resize(idx);
}

ಶೂನ್ಯ vvp_ಸರತಿ_real::ಹಿಮ್ಮುಖ_elems(ಶೂನ್ಯ)
{
      std::reverse(ಸರತಿ.begin(), ಸರತಿ.ಕೊನೆ());
}

ಶೂನ್ಯ vvp_ಸರತಿ_string::ಹಿಮ್ಮುಖ_elems(ಶೂನ್ಯ)
{
      std::reverse(ಸರತಿ.begin(), ಸರತಿ.ಕೊನೆ());
}

ಶೂನ್ಯ vvp_ಸರತಿ_real::ವಿಂಗಡಿಸು_elems(ತಾರ್ಕಿಕ ascending)
{
      ಆದರೆ (ascending) std::sort(ಸರತಿ.begin(), ಸರತಿ.ಕೊನೆ());
      ಇಲ್ಲದಿದ್ದರೆ std::sort(ಸರತಿ.begin(), ಸರತಿ.ಕೊನೆ(), std::greater<ದ್ವಿಗುಣ>());
}

ಶೂನ್ಯ vvp_ಸರತಿ_real::shuffle_elems(ಶೂನ್ಯ)
{
      std::shuffle(ಸರತಿ.begin(), ಸರತಿ.ಕೊನೆ(), shuffle_rng());
}

ಶೂನ್ಯ vvp_ಸರತಿ_string::ವಿಂಗಡಿಸು_elems(ತಾರ್ಕಿಕ ascending)
{
      ಆದರೆ (ascending) std::sort(ಸರತಿ.begin(), ಸರತಿ.ಕೊನೆ());
      ಇಲ್ಲದಿದ್ದರೆ std::sort(ಸರತಿ.begin(), ಸರತಿ.ಕೊನೆ(), std::greater<string>());
}

ಶೂನ್ಯ vvp_ಸರತಿ_string::shuffle_elems(ಶೂನ್ಯ)
{
      std::shuffle(ಸರತಿ.begin(), ಸರತಿ.ಕೊನೆ(), shuffle_rng());
}

ಶೂನ್ಯ vvp_ಸರತಿ_vec4::copy_elems(vvp_object_t src, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ)
{
      ಆದರೆ (vvp_ಸರತಿ*src_ಸರತಿ = src.peek<vvp_ಸರತಿ>())
	    copy_elements<vvp_vector4_t, vvp_ಸರತಿ_vec4, vvp_ಸರತಿ>(ಇದು, src_ಸರತಿ, max_ಗಾತ್ರ);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<vvp_vector4_t, vvp_ಸರತಿ_vec4, vvp_darray>(ಇದು, src_darray, max_ಗಾತ್ರ);
      ಇಲ್ಲದಿದ್ದರೆ
	    cerr << get_fileline() << "Sorry: cannot copy object to vector queue." << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_ಸರತಿ_vec4::ಗಣ_ಶಬ್ದ_max(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&value, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ)
{
      ಆದರೆ (adr == ಸರತಿ.size())
	    ಆದರೆ (!max_ಗಾತ್ರ || (ಸರತಿ.size() < max_ಗಾತ್ರ))
		  ಸರತಿ.push_back(value);
	    ಇಲ್ಲದಿದ್ದರೆ
		  cerr << get_fileline()
		       << "Warning: assigning to queue<vector>[" << adr << "] is"
		          " outside bound (" << max_ಗಾತ್ರ << "). " << value
		       << " was not added." << ಸಾಲು_ಅಂತ್ಯ;
      ಇಲ್ಲದಿದ್ದರೆ
	    ಗಣ_ಶಬ್ದ(adr, value);
}

ಶೂನ್ಯ vvp_ಸರತಿ_vec4::ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&value)
{
      ಆದರೆ (adr < ಸರತಿ.size())
	    ಸರತಿ[adr] = value;
      ಇಲ್ಲದಿದ್ದರೆ
	    cerr << get_fileline()
	         << "Warning: assigning to queue<vector>[" << adr << "] is outside "
	            "of size (" << ಸರತಿ.size() << "). " << value
	         << " was not added." << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_ಸರತಿ_vec4::get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ adr, vvp_vector4_t&value)
{
      ಆದರೆ (adr >= ಸರತಿ.size())
	    value = vvp_vector4_t(ಸರತಿ[0].size());
      ಇಲ್ಲದಿದ್ದರೆ
	    value = ಸರತಿ[adr];
}

ಶೂನ್ಯ vvp_ಸರತಿ_vec4::insert(ಚಿಹ್ನೆರಹಿತ idx, ಸ್ಥಿರಾಂಕ vvp_vector4_t&value, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ)
{
	// Inserting past the end of the queue
      ಆದರೆ (idx > ಸರತಿ.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<vector[" << value.size()
	         << "]>[" << idx << "] is outside of size (" << ಸರತಿ.size()
	         << "). " << value << " was not added." << ಸಾಲು_ಅಂತ್ಯ;
	// Inserting at the end
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx == ಸರತಿ.size())
	    ಆದರೆ (!max_ಗಾತ್ರ || (ಸರತಿ.size() < max_ಗಾತ್ರ))
		  ಸರತಿ.push_back(value);
	    ಇಲ್ಲದಿದ್ದರೆ
		  cerr << get_fileline()
		       << "Warning: inserting to queue<vector[" << value.size()
		       << "]>[" << idx << "] is outside bound (" << max_ಗಾತ್ರ
		       << "). " << value << " was not added." << ಸಾಲು_ಅಂತ್ಯ;
      ಇಲ್ಲದಿದ್ದರೆ  {
	    ಆದರೆ (max_ಗಾತ್ರ && (ಸರತಿ.size() == max_ಗಾತ್ರ)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << ಸರತಿ.back() << " from already full bounded queue<vector["
		       << value.size() << "]> [" << max_ಗಾತ್ರ << "]." << ಸಾಲು_ಅಂತ್ಯ;
		  ಸರತಿ.pop_back();
	    }
	    ಸರತಿ.insert(ಸರತಿ.begin()+idx, value);
      }
}

ಶೂನ್ಯ vvp_ಸರತಿ_vec4::push_back(ಸ್ಥಿರಾಂಕ vvp_vector4_t&value, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ)
{
      ಆದರೆ (!max_ಗಾತ್ರ || (ಸರತಿ.size() < max_ಗಾತ್ರ))
	    ಸರತಿ.push_back(value);
      ಇಲ್ಲದಿದ್ದರೆ
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<vector["
	         << value.size() << "]> [" << max_ಗಾತ್ರ << "]." << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ vvp_ಸರತಿ_vec4::push_front(ಸ್ಥಿರಾಂಕ vvp_vector4_t&value, ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ)
{
      ಆದರೆ (max_ಗಾತ್ರ && (ಸರತಿ.size() == max_ಗಾತ್ರ)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << ಸರತಿ.back() << " from already full bounded queue<vector["
	         << value.size() << "]> [" << max_ಗಾತ್ರ << "]." << ಸಾಲು_ಅಂತ್ಯ;
	    ಸರತಿ.pop_back();
      }
      ಸರತಿ.push_front(value);
}

ಶೂನ್ಯ vvp_ಸರತಿ_vec4::erase(ಚಿಹ್ನೆರಹಿತ idx)
{
      ಪ್ರತಿಜ್ಞಾ(ಸರತಿ.size() > idx);
      ಸರತಿ.erase(ಸರತಿ.begin()+idx);
}

ಶೂನ್ಯ vvp_ಸರತಿ_vec4::erase_ಬಾಲ(ಚಿಹ್ನೆರಹಿತ idx)
{
      ಪ್ರತಿಜ್ಞಾ(ಸರತಿ.size() >= idx);
      ಆದರೆ (ಸರತಿ.size() > idx)
	    ಸರತಿ.resize(idx);
}

ಶೂನ್ಯ vvp_ಸರತಿ_vec4::ಹಿಮ್ಮುಖ_elems(ಶೂನ್ಯ)
{
      std::reverse(ಸರತಿ.begin(), ಸರತಿ.ಕೊನೆ());
}

ಶೂನ್ಯ vvp_ಸರತಿ_vec4::ವಿಂಗಡಿಸು_elems(ತಾರ್ಕಿಕ ascending)
{
      ಆದರೆ (ascending) {
	    std::sort(ಸರತಿ.begin(), ಸರತಿ.ಕೊನೆ(), vec4_lt_unsigned);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    std::sort(ಸರತಿ.begin(), ಸರತಿ.ಕೊನೆ(),
		      [](ಸ್ಥಿರಾಂಕ vvp_vector4_t&a, ಸ್ಥಿರಾಂಕ vvp_vector4_t&b) {
			    ಹಿಂತಿರುಗಿ vec4_lt_unsigned(b, a);
		      });
      }
}

ಶೂನ್ಯ vvp_ಸರತಿ_vec4::shuffle_elems(ಶೂನ್ಯ)
{
      std::shuffle(ಸರತಿ.begin(), ಸರತಿ.ಕೊನೆ(), shuffle_rng());
}
