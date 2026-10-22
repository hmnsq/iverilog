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

using ನಾಮಾಕಾಶ std;

ನಾಮಾಕಾಶ {

ಸ್ಥಿರ std::mt19937& shuffle_rng(ಶೂನ್ಯ)
{
      ಸ್ಥಿರ std::mt19937 rng(std::random_device{}());
      ಪ್ರತ್ಯಾಗಮ rng;
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ vec4_lt_unsigned(ಧ್ರುವ vvp_vector4_t&a, ಧ್ರುವ vvp_vector4_t&b)
{
      ಯದಿ (a.eeq(b)) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      vvp_bit4_t gt = compare_gtge(b, a, BIT4_1);
      ಯದಿ (gt == BIT4_X) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      ಪ್ರತ್ಯಾಗಮ gt == BIT4_1;
}

} // namespace

vvp_darray::~vvp_darray()
{
}

ಶೂನ್ಯ vvp_darray::reverse_elems(ಶೂನ್ಯ)
{
      cerr << "XXXX reverse_elems() not implemented for "
           << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_darray::sort_elems(ತಾರ್ಕಿಕ /*ascending*/)
{
      cerr << "XXXX sort_elems() not implemented for "
           << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_darray::shuffle_elems(ಶೂನ್ಯ)
{
      cerr << "XXXX shuffle_elems() not implemented for "
           << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_darray::set_word(unsigned, ಧ್ರುವ vvp_vector4_t&)
{
      cerr << "XXXX set_word(vvp_vector4_t) not implemented for " << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_darray::set_word(unsigned, ದ್ವಿಗುಣ)
{
      cerr << "XXXX set_word(double) not implemented for " << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_darray::set_word(unsigned, ಧ್ರುವ string&)
{
      cerr << "XXXX set_word(string) not implemented for " << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_darray::set_word(unsigned, ಧ್ರುವ vvp_object_t&)
{
      cerr << "XXXX set_word(vvp_object_t) not implemented for " << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_darray::get_word(unsigned, vvp_vector4_t&)
{
      cerr << "XXXX get_word(vvp_vector4_t) not implemented for " << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_darray::get_word(unsigned, ದ್ವಿಗುಣ&)
{
      cerr << "XXXX get_word(double) not implemented for " << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_darray::get_word(unsigned, string&)
{
      cerr << "XXXX get_word(string) not implemented for " << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_darray::get_word(unsigned, vvp_object_t&)
{
      cerr << "XXXX get_word(vvp_object_t) not implemented for " << typeid(*ಏತತ್).name() << endl;
}

vvp_vector4_t vvp_darray::get_bitstream(ತಾರ್ಕಿಕ)
{
      cerr << "XXXX get_bitstream() not implemented for " << typeid(*ಏತತ್).name() << endl;
      ಪ್ರತ್ಯಾಗಮ vvp_vector4_t();
}

template <ವರ್ಗ TYPE> vvp_darray_atom<TYPE>::~vvp_darray_atom()
{
}

template <ವರ್ಗ TYPE> size_t vvp_darray_atom<TYPE>::get_size() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ array_.size();
}

template <ವರ್ಗ TYPE> ಶೂನ್ಯ vvp_darray_atom<TYPE>::reverse_elems(ಶೂನ್ಯ)
{
      size_t n = array_.size();
      ಪ್ರತಿ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

template <ವರ್ಗ TYPE> ಶೂನ್ಯ vvp_darray_atom<TYPE>::sort_elems(ತಾರ್ಕಿಕ ascending)
{
      ಯದಿ (ascending) std::sort(array_.begin(), array_.end());
      ಅನ್ಯಥಾ std::sort(array_.begin(), array_.end(), std::greater<TYPE>());
}

template <ವರ್ಗ TYPE> ಶೂನ್ಯ vvp_darray_atom<TYPE>::shuffle_elems(ಶೂನ್ಯ)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

template <ವರ್ಗ TYPE> ಶೂನ್ಯ vvp_darray_atom<TYPE>::set_word(unsigned adr, ಧ್ರುವ vvp_vector4_t&value)
{
      ಯದಿ (adr >= array_.size())
	    ಪ್ರತ್ಯಾಗಮ;
      TYPE tmp;
      vector4_to_ಮೌಲ್ಯ(value, tmp, ಸತ್ಯ, ಅಸತ್ಯ);
      array_[adr] = tmp;
}

template <ವರ್ಗ TYPE> ಶೂನ್ಯ vvp_darray_atom<TYPE>::get_word(unsigned adr, vvp_vector4_t&value)
{
      ಯದಿ (adr >= array_.size()) {
	    value = vvp_vector4_t(8*ಪರಿಮಾಣ(TYPE), BIT4_X);
	    ಪ್ರತ್ಯಾಗಮ;
      }

      TYPE word = array_[adr];
      vvp_vector4_t tmp (8*ಪರಿಮಾಣ(TYPE), BIT4_0);
      ಪ್ರತಿ (unsigned idx = 0 ; idx < tmp.size() ; idx += 1) {
	    ಯದಿ (word&1) tmp.set_bit(idx, BIT4_1);
	    word >>= 1;
      }
      value = tmp;
}

template <ವರ್ಗ TYPE> ಶೂನ್ಯ vvp_darray_atom<TYPE>::shallow_copy(ಧ್ರುವ vvp_object*obj)
{
      ಧ್ರುವ vvp_darray_atom<TYPE>*that = dynamic_cast<ಧ್ರುವ vvp_darray_atom<TYPE>*>(obj);
      ಪ್ರತಿಜ್ಞಾ(that);

      unsigned ಅಂಕೆ_ವಸ್ತುಗಳು = min(array_.size(), that->array_.size());
      ಪ್ರತಿ (unsigned idx = 0 ; idx < ಅಂಕೆ_ವಸ್ತುಗಳು ; idx += 1)
	    array_[idx] = that->array_[idx];
}

template <ವರ್ಗ TYPE> vvp_object* vvp_darray_atom<TYPE>::duplicate(ಶೂನ್ಯ) ಧ್ರುವ
{
      vvp_darray_atom<TYPE>*that = ನೂತನ vvp_darray_atom<TYPE>(array_.size());
      ಪ್ರತಿ (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      ಪ್ರತ್ಯಾಗಮ that;
}

template <ವರ್ಗ TYPE> vvp_vector4_t vvp_darray_atom<TYPE>::get_bitstream(ತಾರ್ಕಿಕ)
{
      ಧ್ರುವ unsigned word_wid = ಪರಿಮಾಣ(TYPE) * 8;

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      ಯಾವತ್ (vdx > 0) {
            TYPE word = array_[adx++];
            vdx -= word_wid;
            ಪ್ರತಿ (unsigned bdx = 0; bdx < word_wid; bdx += 1) {
                  ಯದಿ (word & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word >>= 1;
            }
      }

      ಪ್ರತ್ಯಾಗಮ vec;
}

template ವರ್ಗ vvp_darray_atom<uint8_t>;
template ವರ್ಗ vvp_darray_atom<uint16_t>;
template ವರ್ಗ vvp_darray_atom<uint32_t>;
template ವರ್ಗ vvp_darray_atom<uint64_t>;
template ವರ್ಗ vvp_darray_atom<int8_t>;
template ವರ್ಗ vvp_darray_atom<int16_t>;
template ವರ್ಗ vvp_darray_atom<int32_t>;
template ವರ್ಗ vvp_darray_atom<int64_t>;

vvp_darray_vec4::~vvp_darray_vec4()
{
}

size_t vvp_darray_vec4::get_size(ಶೂನ್ಯ) ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ array_.size();
}

ಶೂನ್ಯ vvp_darray_vec4::set_word(unsigned adr, ಧ್ರುವ vvp_vector4_t&value)
{
      ಯದಿ (adr >= array_.size()) ಪ್ರತ್ಯಾಗಮ;
      ಪ್ರತಿಜ್ಞಾ(value.size() == word_wid_);
      array_[adr] = value;
}

ಶೂನ್ಯ vvp_darray_vec4::get_word(unsigned adr, vvp_vector4_t&value)
{
	/*
	 * Return an undefined value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      ಯದಿ ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_X);
	    ಪ್ರತ್ಯಾಗಮ;
      }
      value = array_[adr];
      ಪ್ರತಿಜ್ಞಾ(value.size() == word_wid_);
}

ಶೂನ್ಯ vvp_darray_vec4::shallow_copy(ಧ್ರುವ vvp_object*obj)
{
      ಧ್ರುವ vvp_darray_vec4*that = dynamic_cast<ಧ್ರುವ vvp_darray_vec4*>(obj);
      ಪ್ರತಿಜ್ಞಾ(that);

      unsigned ಅಂಕೆ_ವಸ್ತುಗಳು = min(array_.size(), that->array_.size());
      ಪ್ರತಿ (unsigned idx = 0 ; idx < ಅಂಕೆ_ವಸ್ತುಗಳು ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_object* vvp_darray_vec4::duplicate(ಶೂನ್ಯ) ಧ್ರುವ
{
      vvp_darray_vec4*that = ನೂತನ vvp_darray_vec4(array_.size(), word_wid_);

      ಪ್ರತಿ (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      ಪ್ರತ್ಯಾಗಮ that;
}

vvp_vector4_t vvp_darray_vec4::get_bitstream(ತಾರ್ಕಿಕ as_vec4)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      ಯಾವತ್ (vdx > 0) {
            vdx -= word_wid_;
            ಪ್ರತಿ (unsigned bdx = 0; bdx < word_wid_; bdx += 1) {
                  vvp_bit4_t bit = array_[adx].value(bdx);
                  ಯದಿ (as_vec4 || (bit == BIT4_1))
                        vec.set_bit(vdx+bdx, bit);
            }
            adx++;
      }

      ಪ್ರತ್ಯಾಗಮ vec;
}

ಶೂನ್ಯ vvp_darray_vec4::reverse_elems(ಶೂನ್ಯ)
{
      size_t n = array_.size();
      ಪ್ರತಿ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ಶೂನ್ಯ vvp_darray_vec4::sort_elems(ತಾರ್ಕಿಕ ascending)
{
      ಯದಿ (ascending) {
	    std::sort(array_.begin(), array_.end(), vec4_lt_unsigned);
      } ಅನ್ಯಥಾ {
	    std::sort(array_.begin(), array_.end(),
		      [](ಧ್ರುವ vvp_vector4_t&a, ಧ್ರುವ vvp_vector4_t&b) {
			    ಪ್ರತ್ಯಾಗಮ vec4_lt_unsigned(b, a);
		      });
      }
}

ಶೂನ್ಯ vvp_darray_vec4::shuffle_elems(ಶೂನ್ಯ)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

ಶೂನ್ಯ vvp_darray_vec2::reverse_elems(ಶೂನ್ಯ)
{
      size_t n = array_.size();
      ಪ್ರತಿ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ಶೂನ್ಯ vvp_darray_vec2::sort_elems(ತಾರ್ಕಿಕ ascending)
{
      ಯದಿ (ascending) {
	    std::sort(array_.begin(), array_.end());
      } ಅನ್ಯಥಾ {
	    std::sort(array_.begin(), array_.end(),
		      [](ಧ್ರುವ vvp_vector2_t&a, ಧ್ರುವ vvp_vector2_t&b) {
			    ಪ್ರತ್ಯಾಗಮ b < a;
		      });
      }
}

ಶೂನ್ಯ vvp_darray_vec2::shuffle_elems(ಶೂನ್ಯ)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_darray_vec2::~vvp_darray_vec2()
{
}

size_t vvp_darray_vec2::get_size(ಶೂನ್ಯ) ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ array_.size();
}

ಶೂನ್ಯ vvp_darray_vec2::set_word(unsigned adr, ಧ್ರುವ vvp_vector4_t&value)
{
      ಯದಿ (adr >= array_.size()) ಪ್ರತ್ಯಾಗಮ;
      ಪ್ರತಿಜ್ಞಾ(value.size() == word_wid_);
      array_[adr] = value;
}

ಶೂನ್ಯ vvp_darray_vec2::get_word(unsigned adr, vvp_vector4_t&value)
{
	/*
	 * Return a zero value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      ಯದಿ ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_0);
	    ಪ್ರತ್ಯಾಗಮ;
      }
      ಪ್ರತಿಜ್ಞಾ(array_[adr].size() == word_wid_);
      value.resize(word_wid_);
      ಪ್ರತಿ (unsigned idx = 0; idx < word_wid_; idx += 1) {
	    value.set_bit(idx, array_[adr].value4(idx));
      }
}

ಶೂನ್ಯ vvp_darray_vec2::shallow_copy(ಧ್ರುವ vvp_object*obj)
{
      ಧ್ರುವ vvp_darray_vec2*that = dynamic_cast<ಧ್ರುವ vvp_darray_vec2*>(obj);
      ಪ್ರತಿಜ್ಞಾ(that);

      unsigned ಅಂಕೆ_ವಸ್ತುಗಳು = min(array_.size(), that->array_.size());
      ಪ್ರತಿ (unsigned idx = 0 ; idx < ಅಂಕೆ_ವಸ್ತುಗಳು ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_vector4_t vvp_darray_vec2::get_bitstream(ತಾರ್ಕಿಕ)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      ಯಾವತ್ (vdx > 0) {
            vdx -= word_wid_;
            ಪ್ರತಿ (unsigned bdx = 0; bdx < word_wid_; bdx += 1) {
                  ಯದಿ (array_[adx].value(bdx))
                        vec.set_bit(vdx+bdx, BIT4_1);
            }
            adx++;
      }

      ಪ್ರತ್ಯಾಗಮ vec;
}

vvp_darray_object::~vvp_darray_object()
{
}

size_t vvp_darray_object::get_size() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ array_.size();
}

ಶೂನ್ಯ vvp_darray_object::set_word(unsigned adr, ಧ್ರುವ vvp_object_t&value)
{
      ಯದಿ (adr >= array_.size())
	    ಪ್ರತ್ಯಾಗಮ;
      array_[adr] = value;
}

ಶೂನ್ಯ vvp_darray_object::get_word(unsigned adr, vvp_object_t&value)
{
      ಯದಿ (adr >= array_.size()) {
	    value = vvp_object_t();
	    ಪ್ರತ್ಯಾಗಮ;
      }

      value = array_[adr];
}

ಶೂನ್ಯ vvp_darray_object::shallow_copy(ಧ್ರುವ vvp_object*obj)
{
      ಧ್ರುವ vvp_darray_object*that = dynamic_cast<ಧ್ರುವ vvp_darray_object*>(obj);
      ಪ್ರತಿಜ್ಞಾ(that);

      unsigned ಅಂಕೆ_ವಸ್ತುಗಳು = min(array_.size(), that->array_.size());
      ಪ್ರತಿ (unsigned idx = 0 ; idx < ಅಂಕೆ_ವಸ್ತುಗಳು ; idx += 1)
	    array_[idx] = that->array_[idx];
}

ಶೂನ್ಯ vvp_darray_object::reverse_elems(ಶೂನ್ಯ)
{
      size_t n = array_.size();
      ಪ್ರತಿ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ಶೂನ್ಯ vvp_darray_object::sort_elems(ತಾರ್ಕಿಕ ascending)
{
      ಯದಿ (ascending) {
	    std::sort(array_.begin(), array_.end(),
		      [](ಧ್ರುವ vvp_object_t&a, ಧ್ರುವ vvp_object_t&b) {
			    ಪ್ರತ್ಯಾಗಮ a.sort_order_ಕೀಲಿ() < b.sort_order_ಕೀಲಿ();
		      });
      } ಅನ್ಯಥಾ {
	    std::sort(array_.begin(), array_.end(),
		      [](ಧ್ರುವ vvp_object_t&a, ಧ್ರುವ vvp_object_t&b) {
			    ಪ್ರತ್ಯಾಗಮ a.sort_order_ಕೀಲಿ() > b.sort_order_ಕೀಲಿ();
		      });
      }
}

ಶೂನ್ಯ vvp_darray_object::shuffle_elems(ಶೂನ್ಯ)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_darray_real::~vvp_darray_real()
{
}

size_t vvp_darray_real::get_size() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ array_.size();
}

ಶೂನ್ಯ vvp_darray_real::set_word(unsigned adr, ದ್ವಿಗುಣ value)
{
      ಯದಿ (adr >= array_.size())
	    ಪ್ರತ್ಯಾಗಮ;
      array_[adr] = value;
}

ಶೂನ್ಯ vvp_darray_real::get_word(unsigned adr, ದ್ವಿಗುಣ&value)
{
      ಯದಿ (adr >= array_.size()) {
	    value = 0.0;
	    ಪ್ರತ್ಯಾಗಮ;
      }

      value = array_[adr];
}

ಶೂನ್ಯ vvp_darray_real::shallow_copy(ಧ್ರುವ vvp_object*obj)
{
      ಧ್ರುವ vvp_darray_real*that = dynamic_cast<ಧ್ರುವ vvp_darray_real*>(obj);
      ಪ್ರತಿಜ್ಞಾ(that);

      unsigned ಅಂಕೆ_ವಸ್ತುಗಳು = min(array_.size(), that->array_.size());
      ಪ್ರತಿ (unsigned idx = 0 ; idx < ಅಂಕೆ_ವಸ್ತುಗಳು ; idx += 1)
	    array_[idx] = that->array_[idx];
}

ಶೂನ್ಯ vvp_darray_real::reverse_elems(ಶೂನ್ಯ)
{
      size_t n = array_.size();
      ಪ್ರತಿ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ಶೂನ್ಯ vvp_darray_real::sort_elems(ತಾರ್ಕಿಕ ascending)
{
      ಯದಿ (ascending) std::sort(array_.begin(), array_.end());
      ಅನ್ಯಥಾ std::sort(array_.begin(), array_.end(), std::greater<ದ್ವಿಗುಣ>());
}

ಶೂನ್ಯ vvp_darray_real::shuffle_elems(ಶೂನ್ಯ)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_real::duplicate(ಶೂನ್ಯ) ಧ್ರುವ
{
      vvp_darray_real*that = ನೂತನ vvp_darray_real(array_.size());

      ಪ್ರತಿ (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      ಪ್ರತ್ಯಾಗಮ that;
}

vvp_vector4_t vvp_darray_real::get_bitstream(ತಾರ್ಕಿಕ)
{
      ಧ್ರುವ unsigned word_wid = ಪರಿಮಾಣ(ದ್ವಿಗುಣ) * 8;
      ಪ್ರತಿಜ್ಞಾ(word_wid == 64);

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      ಯಾವತ್ (vdx > 0) {
            ಸಙ್ಘ {
                ದ್ವಿಗುಣ   value;
                uint64_t bits;
            } word;
            word.value = array_[adx++];
            vdx -= word_wid;
            ಪ್ರತಿ (unsigned bdx = 0; bdx < word_wid; bdx += 1) {
                  ಯದಿ (word.bits & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word.bits >>= 1;
            }
      }

      ಪ್ರತ್ಯಾಗಮ vec;
}

vvp_darray_string::~vvp_darray_string()
{
}

size_t vvp_darray_string::get_size() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ array_.size();
}

ಶೂನ್ಯ vvp_darray_string::set_word(unsigned adr, ಧ್ರುವ string&value)
{
      ಯದಿ (adr >= array_.size())
	    ಪ್ರತ್ಯಾಗಮ;
      array_[adr] = value;
}

ಶೂನ್ಯ vvp_darray_string::get_word(unsigned adr, string&value)
{
      ಯದಿ (adr >= array_.size()) {
	    value = "";
	    ಪ್ರತ್ಯಾಗಮ;
      }

      value = array_[adr];
}

ಶೂನ್ಯ vvp_darray_string::shallow_copy(ಧ್ರುವ vvp_object*obj)
{
      ಧ್ರುವ vvp_darray_string*that = dynamic_cast<ಧ್ರುವ vvp_darray_string*>(obj);
      ಪ್ರತಿಜ್ಞಾ(that);

      unsigned ಅಂಕೆ_ವಸ್ತುಗಳು = min(array_.size(), that->array_.size());
      ಪ್ರತಿ (unsigned idx = 0 ; idx < ಅಂಕೆ_ವಸ್ತುಗಳು ; idx += 1)
	    array_[idx] = that->array_[idx];
}

ಶೂನ್ಯ vvp_darray_string::reverse_elems(ಶೂನ್ಯ)
{
      size_t n = array_.size();
      ಪ್ರತಿ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ಶೂನ್ಯ vvp_darray_string::sort_elems(ತಾರ್ಕಿಕ ascending)
{
      ಯದಿ (ascending) std::sort(array_.begin(), array_.end());
      ಅನ್ಯಥಾ std::sort(array_.begin(), array_.end(), std::greater<string>());
}

ಶೂನ್ಯ vvp_darray_string::shuffle_elems(ಶೂನ್ಯ)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_string::duplicate(ಶೂನ್ಯ) ಧ್ರುವ
{
      vvp_darray_string*that = ನೂತನ vvp_darray_string(array_.size());

      ಪ್ರತಿ (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      ಪ್ರತ್ಯಾಗಮ that;
}

vvp_queue::~vvp_queue()
{
}

ಶೂನ್ಯ vvp_queue::copy_elems(vvp_object_t, unsigned)
{
      cerr << "Sorry: copy_elems() not implemented for " << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_queue::set_word_max(unsigned, ಧ್ರುವ vvp_vector4_t&, unsigned)
{
      cerr << "XXXX set_word_max(vvp_vector4_t) not implemented for " << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_queue::insert(unsigned, ಧ್ರುವ vvp_vector4_t&, unsigned)
{
      cerr << "XXXX insert(vvp_vector4_t) not implemented for " << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_queue::push_back(ಧ್ರುವ vvp_vector4_t&, unsigned)
{
      cerr << "XXXX push_back(vvp_vector4_t) not implemented for " << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_queue::push_front(ಧ್ರುವ vvp_vector4_t&, unsigned)
{
      cerr << "XXXX push_front(vvp_vector4_t) not implemented for " << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_queue::set_word_max(unsigned, ದ್ವಿಗುಣ, unsigned)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_queue::insert(unsigned, ದ್ವಿಗುಣ, unsigned)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_queue::push_back(ದ್ವಿಗುಣ, unsigned)
{
      cerr << "XXXX push_back(double) not implemented for " << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_queue::push_front(ದ್ವಿಗುಣ, unsigned)
{
      cerr << "XXXX push_front(double) not implemented for " << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_queue::set_word_max(unsigned, ಧ್ರುವ string&, unsigned)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_queue::insert(unsigned, ಧ್ರುವ string&, unsigned)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_queue::push_back(ಧ್ರುವ string&, unsigned)
{
      cerr << "XXXX push_back(string) not implemented for " << typeid(*ಏತತ್).name() << endl;
}

ಶೂನ್ಯ vvp_queue::push_front(ಧ್ರುವ string&, unsigned)
{
      cerr << "XXXX push_front(string) not implemented for " << typeid(*ಏತತ್).name() << endl;
}

/*
 * Helper functions used while copying multiple elements into a queue.
 */
ಸ್ಥಿರ ಶೂನ್ಯ print_copy_is_too_big(size_t src_size, unsigned max_size, ಧ್ರುವ string&qtype)
{
      cerr << get_fileline()
           << "Warning: queue<" << qtype << "> is bounded to have at most "
           << max_size << " elements, source has " << src_size << " elements." << endl;
}

ಸ್ಥಿರ ಶೂನ್ಯ print_copy_is_too_big(ದ್ವಿಗುಣ&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "real");
}

ಸ್ಥಿರ ಶೂನ್ಯ print_copy_is_too_big(string&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "string");
}

ಸ್ಥಿರ ಶೂನ್ಯ print_copy_is_too_big(vvp_vector4_t&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "vector");
}

template <typename ELEM, ವರ್ಗ QTYPE, ವರ್ಗ SRC_TYPE>
ಸ್ಥಿರ ಶೂನ್ಯ copy_elements(QTYPE*queue, SRC_TYPE*src, unsigned max_size)
{
      size_t src_size = src->get_size();
      ಯದಿ ((max_size != 0) && (src_size > max_size)) {
	    ELEM tmp;
	    print_copy_is_too_big(tmp, src_size, max_size);
      }
      unsigned copy_size = ((src_size < max_size) ||
                            (max_size == 0)) ? src_size : max_size;
      ಯದಿ (copy_size < queue->get_size())
	    queue->erase_tail(copy_size);
      ಪ್ರತಿ (unsigned idx=0; idx < copy_size; ++idx) {
	    ELEM value;
	    src->get_word(idx, value);
	    queue->set_word_max(idx, value, max_size);
      }
}

ಶೂನ್ಯ vvp_queue_real::copy_elems(vvp_object_t src, unsigned max_size)
{
      ಯದಿ (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<ದ್ವಿಗುಣ, vvp_queue_real, vvp_queue>(ಏತತ್, src_queue, max_size);
      ಅನ್ಯಥಾ ಯದಿ (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<ದ್ವಿಗುಣ, vvp_queue_real, vvp_darray>(ಏತತ್, src_darray, max_size);
      ಅನ್ಯಥಾ
	    cerr << get_fileline() << "Sorry: cannot copy object to real queue." << endl;
}

ಶೂನ್ಯ vvp_queue_real::set_word_max(unsigned adr, ದ್ವಿಗುಣ value, unsigned max_size)
{
      ಯದಿ (adr == queue.size())
	    ಯದಿ (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    ಅನ್ಯಥಾ
		  cerr << get_fileline()
		       << "Warning: assigning to queue<real>[" << adr << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      ಅನ್ಯಥಾ
	    set_word(adr, value);
}

ಶೂನ್ಯ vvp_queue_real::set_word(unsigned adr, ದ್ವಿಗುಣ value)
{
      ಯದಿ (adr < queue.size())
	    queue[adr] = value;
      ಅನ್ಯಥಾ
	    cerr << get_fileline()
	         << "Warning: assigning to queue<real>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
}

ಶೂನ್ಯ vvp_queue_real::get_word(unsigned adr, ದ್ವಿಗುಣ&value)
{
      ಯದಿ (adr >= queue.size())
	    value = 0.0;
      ಅನ್ಯಥಾ
	    value = queue[adr];
}

ಶೂನ್ಯ vvp_queue_real::insert(unsigned idx, ದ್ವಿಗುಣ value, unsigned max_size)
{
	// Inserting past the end of the queue
      ಯದಿ (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<real>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
	// Inserting at the end
      ಅನ್ಯಥಾ ಯದಿ (idx == queue.size())
	    ಯದಿ (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    ಅನ್ಯಥಾ
		  cerr << get_fileline()
		       << "Warning: inserting to queue<real>[" << idx << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      ಅನ್ಯಥಾ  {
	    ಯದಿ (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<real> ["
		       << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

ಶೂನ್ಯ vvp_queue_real::push_back(ದ್ವಿಗುಣ value, unsigned max_size)
{
      ಯದಿ (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      ಅನ್ಯಥಾ
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<real> ["
	         << max_size << "]." << endl;
}

ಶೂನ್ಯ vvp_queue_real::push_front(ದ್ವಿಗುಣ value, unsigned max_size)
{
      ಯದಿ (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<real> ["
	         << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

ಶೂನ್ಯ vvp_queue_real::erase(unsigned idx)
{
      ಪ್ರತಿಜ್ಞಾ(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

ಶೂನ್ಯ vvp_queue_real::erase_tail(unsigned idx)
{
      ಪ್ರತಿಜ್ಞಾ(queue.size() >= idx);
      ಯದಿ (queue.size() > idx)
	    queue.resize(idx);
}

ಶೂನ್ಯ vvp_queue_string::copy_elems(vvp_object_t src, unsigned max_size)
{
      ಯದಿ (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<string, vvp_queue_string, vvp_queue>(ಏತತ್, src_queue, max_size);
      ಅನ್ಯಥಾ ಯದಿ (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<string, vvp_queue_string, vvp_darray>(ಏತತ್, src_darray, max_size);
      ಅನ್ಯಥಾ
	    cerr << get_fileline() << "Sorry: cannot copy object to string queue." << endl;
}

ಶೂನ್ಯ vvp_queue_string::set_word_max(unsigned adr, ಧ್ರುವ string&value, unsigned max_size)
{
      ಯದಿ (adr == queue.size())
	    ಯದಿ (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    ಅನ್ಯಥಾ
		  cerr << get_fileline()
		       << "Warning: assigning to queue<string>[" << adr << "] is"
		          " outside bound (" << max_size << "). \"" << value
		       << "\" was not added." << endl;
      ಅನ್ಯಥಾ
	    set_word(adr, value);
}

ಶೂನ್ಯ vvp_queue_string::set_word(unsigned adr, ಧ್ರುವ string&value)
{
      ಯದಿ (adr < queue.size())
	    queue[adr] = value;
      ಅನ್ಯಥಾ
	    cerr << get_fileline()
	         << "Warning: assigning to queue<string>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << endl;
}

ಶೂನ್ಯ vvp_queue_string::get_word(unsigned adr, string&value)
{
      ಯದಿ (adr >= queue.size())
	    value = "";
      ಅನ್ಯಥಾ
	    value = queue[adr];
}

ಶೂನ್ಯ vvp_queue_string::insert(unsigned idx, ಧ್ರುವ string&value, unsigned max_size)
{
	// Inserting past the end of the queue
      ಯದಿ (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<string>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << endl;
	// Inserting at the end
      ಅನ್ಯಥಾ ಯದಿ (idx == queue.size())
	    ಯದಿ (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    ಅನ್ಯಥಾ
		  cerr << get_fileline()
		       << "Warning: inserting to queue<string>[" << idx << "] is"
		          " outside bound (" << max_size << "). \"" << value
		       << "\" was not added." << endl;
      ಅನ್ಯಥಾ  {
	    ಯದಿ (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", \"" << value << "\") removed \""
		       << queue.back() << "\" from already full bounded queue<string> ["
		       << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

ಶೂನ್ಯ vvp_queue_string::push_back(ಧ್ರುವ string&value, unsigned max_size)
{
      ಯದಿ (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      ಅನ್ಯಥಾ
	    cerr << get_fileline()
	         << "Warning: push_back(\"" << value
	         << "\") skipped for already full bounded queue<string> ["
	         << max_size << "]." << endl;
}

ಶೂನ್ಯ vvp_queue_string::push_front(ಧ್ರುವ string&value, unsigned max_size)
{
      ಯದಿ (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(\"" << value << "\") removed \""
	         << queue.back() << "\" from already full bounded queue<string> ["
	         << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

ಶೂನ್ಯ vvp_queue_string::erase(unsigned idx)
{
      ಪ್ರತಿಜ್ಞಾ(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

ಶೂನ್ಯ vvp_queue_string::erase_tail(unsigned idx)
{
      ಪ್ರತಿಜ್ಞಾ(queue.size() >= idx);
      ಯದಿ (queue.size() > idx)
	    queue.resize(idx);
}

ಶೂನ್ಯ vvp_queue_real::reverse_elems(ಶೂನ್ಯ)
{
      std::reverse(queue.begin(), queue.end());
}

ಶೂನ್ಯ vvp_queue_string::reverse_elems(ಶೂನ್ಯ)
{
      std::reverse(queue.begin(), queue.end());
}

ಶೂನ್ಯ vvp_queue_real::sort_elems(ತಾರ್ಕಿಕ ascending)
{
      ಯದಿ (ascending) std::sort(queue.begin(), queue.end());
      ಅನ್ಯಥಾ std::sort(queue.begin(), queue.end(), std::greater<ದ್ವಿಗುಣ>());
}

ಶೂನ್ಯ vvp_queue_real::shuffle_elems(ಶೂನ್ಯ)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

ಶೂನ್ಯ vvp_queue_string::sort_elems(ತಾರ್ಕಿಕ ascending)
{
      ಯದಿ (ascending) std::sort(queue.begin(), queue.end());
      ಅನ್ಯಥಾ std::sort(queue.begin(), queue.end(), std::greater<string>());
}

ಶೂನ್ಯ vvp_queue_string::shuffle_elems(ಶೂನ್ಯ)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

ಶೂನ್ಯ vvp_queue_vec4::copy_elems(vvp_object_t src, unsigned max_size)
{
      ಯದಿ (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_queue>(ಏತತ್, src_queue, max_size);
      ಅನ್ಯಥಾ ಯದಿ (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_darray>(ಏತತ್, src_darray, max_size);
      ಅನ್ಯಥಾ
	    cerr << get_fileline() << "Sorry: cannot copy object to vector queue." << endl;
}

ಶೂನ್ಯ vvp_queue_vec4::set_word_max(unsigned adr, ಧ್ರುವ vvp_vector4_t&value, unsigned max_size)
{
      ಯದಿ (adr == queue.size())
	    ಯದಿ (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    ಅನ್ಯಥಾ
		  cerr << get_fileline()
		       << "Warning: assigning to queue<vector>[" << adr << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      ಅನ್ಯಥಾ
	    set_word(adr, value);
}

ಶೂನ್ಯ vvp_queue_vec4::set_word(unsigned adr, ಧ್ರುವ vvp_vector4_t&value)
{
      ಯದಿ (adr < queue.size())
	    queue[adr] = value;
      ಅನ್ಯಥಾ
	    cerr << get_fileline()
	         << "Warning: assigning to queue<vector>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
}

ಶೂನ್ಯ vvp_queue_vec4::get_word(unsigned adr, vvp_vector4_t&value)
{
      ಯದಿ (adr >= queue.size())
	    value = vvp_vector4_t(queue[0].size());
      ಅನ್ಯಥಾ
	    value = queue[adr];
}

ಶೂನ್ಯ vvp_queue_vec4::insert(unsigned idx, ಧ್ರುವ vvp_vector4_t&value, unsigned max_size)
{
	// Inserting past the end of the queue
      ಯದಿ (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<vector[" << value.size()
	         << "]>[" << idx << "] is outside of size (" << queue.size()
	         << "). " << value << " was not added." << endl;
	// Inserting at the end
      ಅನ್ಯಥಾ ಯದಿ (idx == queue.size())
	    ಯದಿ (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    ಅನ್ಯಥಾ
		  cerr << get_fileline()
		       << "Warning: inserting to queue<vector[" << value.size()
		       << "]>[" << idx << "] is outside bound (" << max_size
		       << "). " << value << " was not added." << endl;
      ಅನ್ಯಥಾ  {
	    ಯದಿ (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<vector["
		       << value.size() << "]> [" << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

ಶೂನ್ಯ vvp_queue_vec4::push_back(ಧ್ರುವ vvp_vector4_t&value, unsigned max_size)
{
      ಯದಿ (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      ಅನ್ಯಥಾ
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<vector["
	         << value.size() << "]> [" << max_size << "]." << endl;
}

ಶೂನ್ಯ vvp_queue_vec4::push_front(ಧ್ರುವ vvp_vector4_t&value, unsigned max_size)
{
      ಯದಿ (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<vector["
	         << value.size() << "]> [" << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

ಶೂನ್ಯ vvp_queue_vec4::erase(unsigned idx)
{
      ಪ್ರತಿಜ್ಞಾ(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

ಶೂನ್ಯ vvp_queue_vec4::erase_tail(unsigned idx)
{
      ಪ್ರತಿಜ್ಞಾ(queue.size() >= idx);
      ಯದಿ (queue.size() > idx)
	    queue.resize(idx);
}

ಶೂನ್ಯ vvp_queue_vec4::reverse_elems(ಶೂನ್ಯ)
{
      std::reverse(queue.begin(), queue.end());
}

ಶೂನ್ಯ vvp_queue_vec4::sort_elems(ತಾರ್ಕಿಕ ascending)
{
      ಯದಿ (ascending) {
	    std::sort(queue.begin(), queue.end(), vec4_lt_unsigned);
      } ಅನ್ಯಥಾ {
	    std::sort(queue.begin(), queue.end(),
		      [](ಧ್ರುವ vvp_vector4_t&a, ಧ್ರುವ vvp_vector4_t&b) {
			    ಪ್ರತ್ಯಾಗಮ vec4_lt_unsigned(b, a);
		      });
      }
}

ಶೂನ್ಯ vvp_queue_vec4::shuffle_elems(ಶೂನ್ಯ)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}
