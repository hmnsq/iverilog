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

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

ਨਾਮਾਕਾਸ਼ {

ਸਥਿਰ std::mt19937& shuffle_rng(ਖਾਲੀ)
{
      ਸਥਿਰ std::mt19937 rng(std::random_device{}());
      ਵਾਪਸ rng;
}

ਸਥਿਰ ਤਾਰਕਿਕ vec4_lt_unsigned(ਅਚਲ vvp_vector4_t&a, ਅਚਲ vvp_vector4_t&b)
{
      ਜੇ (a.eeq(b)) ਵਾਪਸ ਝੂਠ;
      vvp_bit4_t gt = compare_gtge(b, a, BIT4_1);
      ਜੇ (gt == BIT4_X) ਵਾਪਸ ਝੂਠ;
      ਵਾਪਸ gt == BIT4_1;
}

} // namespace

vvp_darray::~vvp_darray()
{
}

ਖਾਲੀ vvp_darray::ਉਲਟ_elems(ਖਾਲੀ)
{
      cerr << "XXXX reverse_elems() not implemented for "
           << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_darray::ਛਾਂਟੋ_elems(ਤਾਰਕਿਕ /*ascending*/)
{
      cerr << "XXXX sort_elems() not implemented for "
           << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_darray::shuffle_elems(ਖਾਲੀ)
{
      cerr << "XXXX shuffle_elems() not implemented for "
           << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_darray::ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਅਚਲ vvp_vector4_t&)
{
      cerr << "XXXX set_word(vvp_vector4_t) not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_darray::ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਦੁੱਗਣਾ)
{
      cerr << "XXXX set_word(double) not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_darray::ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਅਚਲ string&)
{
      cerr << "XXXX set_word(string) not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_darray::ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਅਚਲ vvp_object_t&)
{
      cerr << "XXXX set_word(vvp_object_t) not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_darray::get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ, vvp_vector4_t&)
{
      cerr << "XXXX get_word(vvp_vector4_t) not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_darray::get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਦੁੱਗਣਾ&)
{
      cerr << "XXXX get_word(double) not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_darray::get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ, string&)
{
      cerr << "XXXX get_word(string) not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_darray::get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ, vvp_object_t&)
{
      cerr << "XXXX get_word(vvp_object_t) not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

vvp_vector4_t vvp_darray::get_bitstream(ਤਾਰਕਿਕ)
{
      cerr << "XXXX get_bitstream() not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
      ਵਾਪਸ vvp_vector4_t();
}

ਸਾਂਚਾ <ਵਰਗ TYPE> vvp_darray_atom<TYPE>::~vvp_darray_atom()
{
}

ਸਾਂਚਾ <ਵਰਗ TYPE> size_t vvp_darray_atom<TYPE>::get_ਮਾਪ() ਅਚਲ
{
      ਵਾਪਸ array_.size();
}

ਸਾਂਚਾ <ਵਰਗ TYPE> ਖਾਲੀ vvp_darray_atom<TYPE>::ਉਲਟ_elems(ਖਾਲੀ)
{
      size_t n = array_.size();
      ਲਈ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ਸਾਂਚਾ <ਵਰਗ TYPE> ਖਾਲੀ vvp_darray_atom<TYPE>::ਛਾਂਟੋ_elems(ਤਾਰਕਿਕ ascending)
{
      ਜੇ (ascending) std::sort(array_.begin(), array_.ਸਿਰਾ());
      ਨਹੀਂਤਾਂ std::sort(array_.begin(), array_.ਸਿਰਾ(), std::greater<TYPE>());
}

ਸਾਂਚਾ <ਵਰਗ TYPE> ਖਾਲੀ vvp_darray_atom<TYPE>::shuffle_elems(ਖਾਲੀ)
{
      std::shuffle(array_.begin(), array_.ਸਿਰਾ(), shuffle_rng());
}

ਸਾਂਚਾ <ਵਰਗ TYPE> ਖਾਲੀ vvp_darray_atom<TYPE>::ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ vvp_vector4_t&value)
{
      ਜੇ (adr >= array_.size())
	    ਵਾਪਸ;
      TYPE tmp;
      vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(value, tmp, ਸੱਚ, ਝੂਠ);
      array_[adr] = tmp;
}

ਸਾਂਚਾ <ਵਰਗ TYPE> ਖਾਲੀ vvp_darray_atom<TYPE>::get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, vvp_vector4_t&value)
{
      ਜੇ (adr >= array_.size()) {
	    value = vvp_vector4_t(8*ਆਕਾਰ(TYPE), BIT4_X);
	    ਵਾਪਸ;
      }

      TYPE ਸ਼ਬਦ_2 = array_[adr];
      vvp_vector4_t tmp (8*ਆਕਾਰ(TYPE), BIT4_0);
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < tmp.size() ; idx += 1) {
	    ਜੇ (ਸ਼ਬਦ_2&1) tmp.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_1);
	    ਸ਼ਬਦ_2 >>= 1;
      }
      value = tmp;
}

ਸਾਂਚਾ <ਵਰਗ TYPE> ਖਾਲੀ vvp_darray_atom<TYPE>::shallow_copy(ਅਚਲ vvp_object*obj)
{
      ਅਚਲ vvp_darray_atom<TYPE>*that = dynamic_cast<ਅਚਲ vvp_darray_atom<TYPE>*>(obj);
      ਪ੍ਰਤਿਜ੍ਞਾ(that);

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੰਕ_ਚੀਜ਼ਾਂ = min(array_.size(), that->array_.size());
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਅੰਕ_ਚੀਜ਼ਾਂ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

ਸਾਂਚਾ <ਵਰਗ TYPE> vvp_object* vvp_darray_atom<TYPE>::duplicate(ਖਾਲੀ) ਅਚਲ
{
      vvp_darray_atom<TYPE>*that = ਨਵਾਂ vvp_darray_atom<TYPE>(array_.size());
      ਲਈ (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      ਵਾਪਸ that;
}

ਸਾਂਚਾ <ਵਰਗ TYPE> vvp_vector4_t vvp_darray_atom<TYPE>::get_bitstream(ਤਾਰਕਿਕ)
{
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ_wid = ਆਕਾਰ(TYPE) * 8;

      vvp_vector4_t vec(array_.size() * ਸ਼ਬਦ_wid, BIT4_0);

      ਬਿਨਾਨਿਸ਼ਾਨੀ adx = 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ vdx = vec.size();
      ਜਦੋਂ (vdx > 0) {
            TYPE ਸ਼ਬਦ_2 = array_[adx++];
            vdx -= ਸ਼ਬਦ_wid;
            ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ bdx = 0; bdx < ਸ਼ਬਦ_wid; bdx += 1) {
                  ਜੇ (ਸ਼ਬਦ_2 & 1)
                        vec.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(vdx+bdx, BIT4_1);
                  ਸ਼ਬਦ_2 >>= 1;
            }
      }

      ਵਾਪਸ vec;
}

ਸਾਂਚਾ ਵਰਗ vvp_darray_atom<uint8_t>;
ਸਾਂਚਾ ਵਰਗ vvp_darray_atom<uint16_t>;
ਸਾਂਚਾ ਵਰਗ vvp_darray_atom<uint32_t>;
ਸਾਂਚਾ ਵਰਗ vvp_darray_atom<uint64_t>;
ਸਾਂਚਾ ਵਰਗ vvp_darray_atom<int8_t>;
ਸਾਂਚਾ ਵਰਗ vvp_darray_atom<int16_t>;
ਸਾਂਚਾ ਵਰਗ vvp_darray_atom<int32_t>;
ਸਾਂਚਾ ਵਰਗ vvp_darray_atom<int64_t>;

vvp_darray_vec4::~vvp_darray_vec4()
{
}

size_t vvp_darray_vec4::get_ਮਾਪ(ਖਾਲੀ) ਅਚਲ
{
      ਵਾਪਸ array_.size();
}

ਖਾਲੀ vvp_darray_vec4::ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ vvp_vector4_t&value)
{
      ਜੇ (adr >= array_.size()) ਵਾਪਸ;
      ਪ੍ਰਤਿਜ੍ਞਾ(value.size() == ਸ਼ਬਦ_wid_2);
      array_[adr] = value;
}

ਖਾਲੀ vvp_darray_vec4::get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, vvp_vector4_t&value)
{
	/*
	 * Return an undefined value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      ਜੇ ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(ਸ਼ਬਦ_wid_2, BIT4_X);
	    ਵਾਪਸ;
      }
      value = array_[adr];
      ਪ੍ਰਤਿਜ੍ਞਾ(value.size() == ਸ਼ਬਦ_wid_2);
}

ਖਾਲੀ vvp_darray_vec4::shallow_copy(ਅਚਲ vvp_object*obj)
{
      ਅਚਲ vvp_darray_vec4*that = dynamic_cast<ਅਚਲ vvp_darray_vec4*>(obj);
      ਪ੍ਰਤਿਜ੍ਞਾ(that);

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੰਕ_ਚੀਜ਼ਾਂ = min(array_.size(), that->array_.size());
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਅੰਕ_ਚੀਜ਼ਾਂ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_object* vvp_darray_vec4::duplicate(ਖਾਲੀ) ਅਚਲ
{
      vvp_darray_vec4*that = ਨਵਾਂ vvp_darray_vec4(array_.size(), ਸ਼ਬਦ_wid_2);

      ਲਈ (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      ਵਾਪਸ that;
}

vvp_vector4_t vvp_darray_vec4::get_bitstream(ਤਾਰਕਿਕ as_vec4)
{
      vvp_vector4_t vec(array_.size() * ਸ਼ਬਦ_wid_2, BIT4_0);

      ਬਿਨਾਨਿਸ਼ਾਨੀ adx = 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ vdx = vec.size();
      ਜਦੋਂ (vdx > 0) {
            vdx -= ਸ਼ਬਦ_wid_2;
            ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ bdx = 0; bdx < ਸ਼ਬਦ_wid_2; bdx += 1) {
                  vvp_bit4_t ਦੋਆਧਾਰੀ_ਅੰਕ = array_[adx].value(bdx);
                  ਜੇ (as_vec4 || (ਦੋਆਧਾਰੀ_ਅੰਕ == BIT4_1))
                        vec.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(vdx+bdx, ਦੋਆਧਾਰੀ_ਅੰਕ);
            }
            adx++;
      }

      ਵਾਪਸ vec;
}

ਖਾਲੀ vvp_darray_vec4::ਉਲਟ_elems(ਖਾਲੀ)
{
      size_t n = array_.size();
      ਲਈ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ਖਾਲੀ vvp_darray_vec4::ਛਾਂਟੋ_elems(ਤਾਰਕਿਕ ascending)
{
      ਜੇ (ascending) {
	    std::sort(array_.begin(), array_.ਸਿਰਾ(), vec4_lt_unsigned);
      } ਨਹੀਂਤਾਂ {
	    std::sort(array_.begin(), array_.ਸਿਰਾ(),
		      [](ਅਚਲ vvp_vector4_t&a, ਅਚਲ vvp_vector4_t&b) {
			    ਵਾਪਸ vec4_lt_unsigned(b, a);
		      });
      }
}

ਖਾਲੀ vvp_darray_vec4::shuffle_elems(ਖਾਲੀ)
{
      std::shuffle(array_.begin(), array_.ਸਿਰਾ(), shuffle_rng());
}

ਖਾਲੀ vvp_darray_vec2::ਉਲਟ_elems(ਖਾਲੀ)
{
      size_t n = array_.size();
      ਲਈ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ਖਾਲੀ vvp_darray_vec2::ਛਾਂਟੋ_elems(ਤਾਰਕਿਕ ascending)
{
      ਜੇ (ascending) {
	    std::sort(array_.begin(), array_.ਸਿਰਾ());
      } ਨਹੀਂਤਾਂ {
	    std::sort(array_.begin(), array_.ਸਿਰਾ(),
		      [](ਅਚਲ vvp_vector2_t&a, ਅਚਲ vvp_vector2_t&b) {
			    ਵਾਪਸ b < a;
		      });
      }
}

ਖਾਲੀ vvp_darray_vec2::shuffle_elems(ਖਾਲੀ)
{
      std::shuffle(array_.begin(), array_.ਸਿਰਾ(), shuffle_rng());
}

vvp_darray_vec2::~vvp_darray_vec2()
{
}

size_t vvp_darray_vec2::get_ਮਾਪ(ਖਾਲੀ) ਅਚਲ
{
      ਵਾਪਸ array_.size();
}

ਖਾਲੀ vvp_darray_vec2::ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ vvp_vector4_t&value)
{
      ਜੇ (adr >= array_.size()) ਵਾਪਸ;
      ਪ੍ਰਤਿਜ੍ਞਾ(value.size() == ਸ਼ਬਦ_wid_2);
      array_[adr] = value;
}

ਖਾਲੀ vvp_darray_vec2::get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, vvp_vector4_t&value)
{
	/*
	 * Return a zero value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      ਜੇ ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(ਸ਼ਬਦ_wid_2, BIT4_0);
	    ਵਾਪਸ;
      }
      ਪ੍ਰਤਿਜ੍ਞਾ(array_[adr].size() == ਸ਼ਬਦ_wid_2);
      value.resize(ਸ਼ਬਦ_wid_2);
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < ਸ਼ਬਦ_wid_2; idx += 1) {
	    value.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, array_[adr].value4(idx));
      }
}

ਖਾਲੀ vvp_darray_vec2::shallow_copy(ਅਚਲ vvp_object*obj)
{
      ਅਚਲ vvp_darray_vec2*that = dynamic_cast<ਅਚਲ vvp_darray_vec2*>(obj);
      ਪ੍ਰਤਿਜ੍ਞਾ(that);

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੰਕ_ਚੀਜ਼ਾਂ = min(array_.size(), that->array_.size());
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਅੰਕ_ਚੀਜ਼ਾਂ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_vector4_t vvp_darray_vec2::get_bitstream(ਤਾਰਕਿਕ)
{
      vvp_vector4_t vec(array_.size() * ਸ਼ਬਦ_wid_2, BIT4_0);

      ਬਿਨਾਨਿਸ਼ਾਨੀ adx = 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ vdx = vec.size();
      ਜਦੋਂ (vdx > 0) {
            vdx -= ਸ਼ਬਦ_wid_2;
            ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ bdx = 0; bdx < ਸ਼ਬਦ_wid_2; bdx += 1) {
                  ਜੇ (array_[adx].value(bdx))
                        vec.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(vdx+bdx, BIT4_1);
            }
            adx++;
      }

      ਵਾਪਸ vec;
}

vvp_darray_object::~vvp_darray_object()
{
}

size_t vvp_darray_object::get_ਮਾਪ() ਅਚਲ
{
      ਵਾਪਸ array_.size();
}

ਖਾਲੀ vvp_darray_object::ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ vvp_object_t&value)
{
      ਜੇ (adr >= array_.size())
	    ਵਾਪਸ;
      array_[adr] = value;
}

ਖਾਲੀ vvp_darray_object::get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, vvp_object_t&value)
{
      ਜੇ (adr >= array_.size()) {
	    value = vvp_object_t();
	    ਵਾਪਸ;
      }

      value = array_[adr];
}

ਖਾਲੀ vvp_darray_object::shallow_copy(ਅਚਲ vvp_object*obj)
{
      ਅਚਲ vvp_darray_object*that = dynamic_cast<ਅਚਲ vvp_darray_object*>(obj);
      ਪ੍ਰਤਿਜ੍ਞਾ(that);

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੰਕ_ਚੀਜ਼ਾਂ = min(array_.size(), that->array_.size());
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਅੰਕ_ਚੀਜ਼ਾਂ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

ਖਾਲੀ vvp_darray_object::ਉਲਟ_elems(ਖਾਲੀ)
{
      size_t n = array_.size();
      ਲਈ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ਖਾਲੀ vvp_darray_object::ਛਾਂਟੋ_elems(ਤਾਰਕਿਕ ascending)
{
      ਜੇ (ascending) {
	    std::sort(array_.begin(), array_.ਸਿਰਾ(),
		      [](ਅਚਲ vvp_object_t&a, ਅਚਲ vvp_object_t&b) {
			    ਵਾਪਸ a.ਛਾਂਟੋ_ਕ੍ਰਮ_ਕੁੰਜੀ() < b.ਛਾਂਟੋ_ਕ੍ਰਮ_ਕੁੰਜੀ();
		      });
      } ਨਹੀਂਤਾਂ {
	    std::sort(array_.begin(), array_.ਸਿਰਾ(),
		      [](ਅਚਲ vvp_object_t&a, ਅਚਲ vvp_object_t&b) {
			    ਵਾਪਸ a.ਛਾਂਟੋ_ਕ੍ਰਮ_ਕੁੰਜੀ() > b.ਛਾਂਟੋ_ਕ੍ਰਮ_ਕੁੰਜੀ();
		      });
      }
}

ਖਾਲੀ vvp_darray_object::shuffle_elems(ਖਾਲੀ)
{
      std::shuffle(array_.begin(), array_.ਸਿਰਾ(), shuffle_rng());
}

vvp_darray_real::~vvp_darray_real()
{
}

size_t vvp_darray_real::get_ਮਾਪ() ਅਚਲ
{
      ਵਾਪਸ array_.size();
}

ਖਾਲੀ vvp_darray_real::ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਦੁੱਗਣਾ value)
{
      ਜੇ (adr >= array_.size())
	    ਵਾਪਸ;
      array_[adr] = value;
}

ਖਾਲੀ vvp_darray_real::get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਦੁੱਗਣਾ&value)
{
      ਜੇ (adr >= array_.size()) {
	    value = 0.0;
	    ਵਾਪਸ;
      }

      value = array_[adr];
}

ਖਾਲੀ vvp_darray_real::shallow_copy(ਅਚਲ vvp_object*obj)
{
      ਅਚਲ vvp_darray_real*that = dynamic_cast<ਅਚਲ vvp_darray_real*>(obj);
      ਪ੍ਰਤਿਜ੍ਞਾ(that);

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੰਕ_ਚੀਜ਼ਾਂ = min(array_.size(), that->array_.size());
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਅੰਕ_ਚੀਜ਼ਾਂ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

ਖਾਲੀ vvp_darray_real::ਉਲਟ_elems(ਖਾਲੀ)
{
      size_t n = array_.size();
      ਲਈ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ਖਾਲੀ vvp_darray_real::ਛਾਂਟੋ_elems(ਤਾਰਕਿਕ ascending)
{
      ਜੇ (ascending) std::sort(array_.begin(), array_.ਸਿਰਾ());
      ਨਹੀਂਤਾਂ std::sort(array_.begin(), array_.ਸਿਰਾ(), std::greater<ਦੁੱਗਣਾ>());
}

ਖਾਲੀ vvp_darray_real::shuffle_elems(ਖਾਲੀ)
{
      std::shuffle(array_.begin(), array_.ਸਿਰਾ(), shuffle_rng());
}

vvp_object* vvp_darray_real::duplicate(ਖਾਲੀ) ਅਚਲ
{
      vvp_darray_real*that = ਨਵਾਂ vvp_darray_real(array_.size());

      ਲਈ (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      ਵਾਪਸ that;
}

vvp_vector4_t vvp_darray_real::get_bitstream(ਤਾਰਕਿਕ)
{
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ_wid = ਆਕਾਰ(ਦੁੱਗਣਾ) * 8;
      ਪ੍ਰਤਿਜ੍ਞਾ(ਸ਼ਬਦ_wid == 64);

      vvp_vector4_t vec(array_.size() * ਸ਼ਬਦ_wid, BIT4_0);

      ਬਿਨਾਨਿਸ਼ਾਨੀ adx = 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ vdx = vec.size();
      ਜਦੋਂ (vdx > 0) {
            ਸੰਘ {
                ਦੁੱਗਣਾ   value;
                uint64_t ਬਿੱਟ;
            } ਸ਼ਬਦ_2;
            ਸ਼ਬਦ_2.value = array_[adx++];
            vdx -= ਸ਼ਬਦ_wid;
            ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ bdx = 0; bdx < ਸ਼ਬਦ_wid; bdx += 1) {
                  ਜੇ (ਸ਼ਬਦ_2.ਬਿੱਟ & 1)
                        vec.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(vdx+bdx, BIT4_1);
                  ਸ਼ਬਦ_2.ਬਿੱਟ >>= 1;
            }
      }

      ਵਾਪਸ vec;
}

vvp_darray_string::~vvp_darray_string()
{
}

size_t vvp_darray_string::get_ਮਾਪ() ਅਚਲ
{
      ਵਾਪਸ array_.size();
}

ਖਾਲੀ vvp_darray_string::ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ string&value)
{
      ਜੇ (adr >= array_.size())
	    ਵਾਪਸ;
      array_[adr] = value;
}

ਖਾਲੀ vvp_darray_string::get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, string&value)
{
      ਜੇ (adr >= array_.size()) {
	    value = "";
	    ਵਾਪਸ;
      }

      value = array_[adr];
}

ਖਾਲੀ vvp_darray_string::shallow_copy(ਅਚਲ vvp_object*obj)
{
      ਅਚਲ vvp_darray_string*that = dynamic_cast<ਅਚਲ vvp_darray_string*>(obj);
      ਪ੍ਰਤਿਜ੍ਞਾ(that);

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੰਕ_ਚੀਜ਼ਾਂ = min(array_.size(), that->array_.size());
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਅੰਕ_ਚੀਜ਼ਾਂ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

ਖਾਲੀ vvp_darray_string::ਉਲਟ_elems(ਖਾਲੀ)
{
      size_t n = array_.size();
      ਲਈ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ਖਾਲੀ vvp_darray_string::ਛਾਂਟੋ_elems(ਤਾਰਕਿਕ ascending)
{
      ਜੇ (ascending) std::sort(array_.begin(), array_.ਸਿਰਾ());
      ਨਹੀਂਤਾਂ std::sort(array_.begin(), array_.ਸਿਰਾ(), std::greater<string>());
}

ਖਾਲੀ vvp_darray_string::shuffle_elems(ਖਾਲੀ)
{
      std::shuffle(array_.begin(), array_.ਸਿਰਾ(), shuffle_rng());
}

vvp_object* vvp_darray_string::duplicate(ਖਾਲੀ) ਅਚਲ
{
      vvp_darray_string*that = ਨਵਾਂ vvp_darray_string(array_.size());

      ਲਈ (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      ਵਾਪਸ that;
}

vvp_ਕਤਾਰ::~vvp_ਕਤਾਰ()
{
}

ਖਾਲੀ vvp_ਕਤਾਰ::copy_elems(vvp_object_t, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      cerr << "Sorry: copy_elems() not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_ਕਤਾਰ::ਸੈੱਟ_ਸ਼ਬਦ_max(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਅਚਲ vvp_vector4_t&, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      cerr << "XXXX set_word_max(vvp_vector4_t) not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_ਕਤਾਰ::insert(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਅਚਲ vvp_vector4_t&, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      cerr << "XXXX insert(vvp_vector4_t) not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_ਕਤਾਰ::push_back(ਅਚਲ vvp_vector4_t&, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      cerr << "XXXX push_back(vvp_vector4_t) not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_ਕਤਾਰ::push_front(ਅਚਲ vvp_vector4_t&, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      cerr << "XXXX push_front(vvp_vector4_t) not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_ਕਤਾਰ::ਸੈੱਟ_ਸ਼ਬਦ_max(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਦੁੱਗਣਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_ਕਤਾਰ::insert(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਦੁੱਗਣਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_ਕਤਾਰ::push_back(ਦੁੱਗਣਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      cerr << "XXXX push_back(double) not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_ਕਤਾਰ::push_front(ਦੁੱਗਣਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      cerr << "XXXX push_front(double) not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_ਕਤਾਰ::ਸੈੱਟ_ਸ਼ਬਦ_max(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਅਚਲ string&, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_ਕਤਾਰ::insert(ਬਿਨਾਨਿਸ਼ਾਨੀ, ਅਚਲ string&, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_ਕਤਾਰ::push_back(ਅਚਲ string&, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      cerr << "XXXX push_back(string) not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_ਕਤਾਰ::push_front(ਅਚਲ string&, ਬਿਨਾਨਿਸ਼ਾਨੀ)
{
      cerr << "XXXX push_front(string) not implemented for " << typeid(*ਇਹ).name() << ਸਤਰ_ਅੰਤ;
}

/*
 * Helper functions used while copying multiple elements into a queue.
 */
ਸਥਿਰ ਖਾਲੀ print_copy_is_too_big(size_t src_ਮਾਪ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ, ਅਚਲ string&qtype)
{
      cerr << get_fileline()
           << "Warning: queue<" << qtype << "> is bounded to have at most "
           << max_ਮਾਪ << " elements, source has " << src_ਮਾਪ << " elements." << ਸਤਰ_ਅੰਤ;
}

ਸਥਿਰ ਖਾਲੀ print_copy_is_too_big(ਦੁੱਗਣਾ&, size_t src_ਮਾਪ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ)
{
      print_copy_is_too_big(src_ਮਾਪ, max_ਮਾਪ, "real");
}

ਸਥਿਰ ਖਾਲੀ print_copy_is_too_big(string&, size_t src_ਮਾਪ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ)
{
      print_copy_is_too_big(src_ਮਾਪ, max_ਮਾਪ, "string");
}

ਸਥਿਰ ਖਾਲੀ print_copy_is_too_big(vvp_vector4_t&, size_t src_ਮਾਪ, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ)
{
      print_copy_is_too_big(src_ਮਾਪ, max_ਮਾਪ, "vector");
}

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ ELEM, ਵਰਗ QTYPE, ਵਰਗ SRC_TYPE>
ਸਥਿਰ ਖਾਲੀ copy_elements(QTYPE*ਕਤਾਰ, SRC_TYPE*src, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ)
{
      size_t src_ਮਾਪ = src->get_ਮਾਪ();
      ਜੇ ((max_ਮਾਪ != 0) && (src_ਮਾਪ > max_ਮਾਪ)) {
	    ELEM tmp;
	    print_copy_is_too_big(tmp, src_ਮਾਪ, max_ਮਾਪ);
      }
      ਬਿਨਾਨਿਸ਼ਾਨੀ copy_ਮਾਪ = ((src_ਮਾਪ < max_ਮਾਪ) ||
                            (max_ਮਾਪ == 0)) ? src_ਮਾਪ : max_ਮਾਪ;
      ਜੇ (copy_ਮਾਪ < ਕਤਾਰ->get_ਮਾਪ())
	    ਕਤਾਰ->erase_ਪੂਛ(copy_ਮਾਪ);
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx=0; idx < copy_ਮਾਪ; ++idx) {
	    ELEM value;
	    src->get_ਸ਼ਬਦ(idx, value);
	    ਕਤਾਰ->ਸੈੱਟ_ਸ਼ਬਦ_max(idx, value, max_ਮਾਪ);
      }
}

ਖਾਲੀ vvp_ਕਤਾਰ_real::copy_elems(vvp_object_t src, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ)
{
      ਜੇ (vvp_ਕਤਾਰ*src_ਕਤਾਰ = src.peek<vvp_ਕਤਾਰ>())
	    copy_elements<ਦੁੱਗਣਾ, vvp_ਕਤਾਰ_real, vvp_ਕਤਾਰ>(ਇਹ, src_ਕਤਾਰ, max_ਮਾਪ);
      ਨਹੀਂਤਾਂ ਜੇ (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<ਦੁੱਗਣਾ, vvp_ਕਤਾਰ_real, vvp_darray>(ਇਹ, src_darray, max_ਮਾਪ);
      ਨਹੀਂਤਾਂ
	    cerr << get_fileline() << "Sorry: cannot copy object to real queue." << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_ਕਤਾਰ_real::ਸੈੱਟ_ਸ਼ਬਦ_max(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਦੁੱਗਣਾ value, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ)
{
      ਜੇ (adr == ਕਤਾਰ.size())
	    ਜੇ (!max_ਮਾਪ || (ਕਤਾਰ.size() < max_ਮਾਪ))
		  ਕਤਾਰ.push_back(value);
	    ਨਹੀਂਤਾਂ
		  cerr << get_fileline()
		       << "Warning: assigning to queue<real>[" << adr << "] is"
		          " outside bound (" << max_ਮਾਪ << "). " << value
		       << " was not added." << ਸਤਰ_ਅੰਤ;
      ਨਹੀਂਤਾਂ
	    ਸੈੱਟ_ਸ਼ਬਦ(adr, value);
}

ਖਾਲੀ vvp_ਕਤਾਰ_real::ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਦੁੱਗਣਾ value)
{
      ਜੇ (adr < ਕਤਾਰ.size())
	    ਕਤਾਰ[adr] = value;
      ਨਹੀਂਤਾਂ
	    cerr << get_fileline()
	         << "Warning: assigning to queue<real>[" << adr << "] is outside "
	            "of size (" << ਕਤਾਰ.size() << "). " << value
	         << " was not added." << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_ਕਤਾਰ_real::get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਦੁੱਗਣਾ&value)
{
      ਜੇ (adr >= ਕਤਾਰ.size())
	    value = 0.0;
      ਨਹੀਂਤਾਂ
	    value = ਕਤਾਰ[adr];
}

ਖਾਲੀ vvp_ਕਤਾਰ_real::insert(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਦੁੱਗਣਾ value, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ)
{
	// Inserting past the end of the queue
      ਜੇ (idx > ਕਤਾਰ.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<real>[" << idx << "] is "
	            "outside of size (" << ਕਤਾਰ.size() << "). " << value
	         << " was not added." << ਸਤਰ_ਅੰਤ;
	// Inserting at the end
      ਨਹੀਂਤਾਂ ਜੇ (idx == ਕਤਾਰ.size())
	    ਜੇ (!max_ਮਾਪ || (ਕਤਾਰ.size() < max_ਮਾਪ))
		  ਕਤਾਰ.push_back(value);
	    ਨਹੀਂਤਾਂ
		  cerr << get_fileline()
		       << "Warning: inserting to queue<real>[" << idx << "] is"
		          " outside bound (" << max_ਮਾਪ << "). " << value
		       << " was not added." << ਸਤਰ_ਅੰਤ;
      ਨਹੀਂਤਾਂ  {
	    ਜੇ (max_ਮਾਪ && (ਕਤਾਰ.size() == max_ਮਾਪ)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << ਕਤਾਰ.back() << " from already full bounded queue<real> ["
		       << max_ਮਾਪ << "]." << ਸਤਰ_ਅੰਤ;
		  ਕਤਾਰ.pop_back();
	    }
	    ਕਤਾਰ.insert(ਕਤਾਰ.begin()+idx, value);
      }
}

ਖਾਲੀ vvp_ਕਤਾਰ_real::push_back(ਦੁੱਗਣਾ value, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ)
{
      ਜੇ (!max_ਮਾਪ || (ਕਤਾਰ.size() < max_ਮਾਪ))
	    ਕਤਾਰ.push_back(value);
      ਨਹੀਂਤਾਂ
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<real> ["
	         << max_ਮਾਪ << "]." << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_ਕਤਾਰ_real::push_front(ਦੁੱਗਣਾ value, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ)
{
      ਜੇ (max_ਮਾਪ && (ਕਤਾਰ.size() == max_ਮਾਪ)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << ਕਤਾਰ.back() << " from already full bounded queue<real> ["
	         << max_ਮਾਪ << "]." << ਸਤਰ_ਅੰਤ;
	    ਕਤਾਰ.pop_back();
      }
      ਕਤਾਰ.push_front(value);
}

ਖਾਲੀ vvp_ਕਤਾਰ_real::erase(ਬਿਨਾਨਿਸ਼ਾਨੀ idx)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਕਤਾਰ.size() > idx);
      ਕਤਾਰ.erase(ਕਤਾਰ.begin()+idx);
}

ਖਾਲੀ vvp_ਕਤਾਰ_real::erase_ਪੂਛ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਕਤਾਰ.size() >= idx);
      ਜੇ (ਕਤਾਰ.size() > idx)
	    ਕਤਾਰ.resize(idx);
}

ਖਾਲੀ vvp_ਕਤਾਰ_string::copy_elems(vvp_object_t src, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ)
{
      ਜੇ (vvp_ਕਤਾਰ*src_ਕਤਾਰ = src.peek<vvp_ਕਤਾਰ>())
	    copy_elements<string, vvp_ਕਤਾਰ_string, vvp_ਕਤਾਰ>(ਇਹ, src_ਕਤਾਰ, max_ਮਾਪ);
      ਨਹੀਂਤਾਂ ਜੇ (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<string, vvp_ਕਤਾਰ_string, vvp_darray>(ਇਹ, src_darray, max_ਮਾਪ);
      ਨਹੀਂਤਾਂ
	    cerr << get_fileline() << "Sorry: cannot copy object to string queue." << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_ਕਤਾਰ_string::ਸੈੱਟ_ਸ਼ਬਦ_max(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ string&value, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ)
{
      ਜੇ (adr == ਕਤਾਰ.size())
	    ਜੇ (!max_ਮਾਪ || (ਕਤਾਰ.size() < max_ਮਾਪ))
		  ਕਤਾਰ.push_back(value);
	    ਨਹੀਂਤਾਂ
		  cerr << get_fileline()
		       << "Warning: assigning to queue<string>[" << adr << "] is"
		          " outside bound (" << max_ਮਾਪ << "). \"" << value
		       << "\" was not added." << ਸਤਰ_ਅੰਤ;
      ਨਹੀਂਤਾਂ
	    ਸੈੱਟ_ਸ਼ਬਦ(adr, value);
}

ਖਾਲੀ vvp_ਕਤਾਰ_string::ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ string&value)
{
      ਜੇ (adr < ਕਤਾਰ.size())
	    ਕਤਾਰ[adr] = value;
      ਨਹੀਂਤਾਂ
	    cerr << get_fileline()
	         << "Warning: assigning to queue<string>[" << adr << "] is outside "
	            "of size (" << ਕਤਾਰ.size() << "). \"" << value
	         << "\" was not added." << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_ਕਤਾਰ_string::get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, string&value)
{
      ਜੇ (adr >= ਕਤਾਰ.size())
	    value = "";
      ਨਹੀਂਤਾਂ
	    value = ਕਤਾਰ[adr];
}

ਖਾਲੀ vvp_ਕਤਾਰ_string::insert(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਅਚਲ string&value, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ)
{
	// Inserting past the end of the queue
      ਜੇ (idx > ਕਤਾਰ.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<string>[" << idx << "] is "
	            "outside of size (" << ਕਤਾਰ.size() << "). \"" << value
	         << "\" was not added." << ਸਤਰ_ਅੰਤ;
	// Inserting at the end
      ਨਹੀਂਤਾਂ ਜੇ (idx == ਕਤਾਰ.size())
	    ਜੇ (!max_ਮਾਪ || (ਕਤਾਰ.size() < max_ਮਾਪ))
		  ਕਤਾਰ.push_back(value);
	    ਨਹੀਂਤਾਂ
		  cerr << get_fileline()
		       << "Warning: inserting to queue<string>[" << idx << "] is"
		          " outside bound (" << max_ਮਾਪ << "). \"" << value
		       << "\" was not added." << ਸਤਰ_ਅੰਤ;
      ਨਹੀਂਤਾਂ  {
	    ਜੇ (max_ਮਾਪ && (ਕਤਾਰ.size() == max_ਮਾਪ)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", \"" << value << "\") removed \""
		       << ਕਤਾਰ.back() << "\" from already full bounded queue<string> ["
		       << max_ਮਾਪ << "]." << ਸਤਰ_ਅੰਤ;
		  ਕਤਾਰ.pop_back();
	    }
	    ਕਤਾਰ.insert(ਕਤਾਰ.begin()+idx, value);
      }
}

ਖਾਲੀ vvp_ਕਤਾਰ_string::push_back(ਅਚਲ string&value, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ)
{
      ਜੇ (!max_ਮਾਪ || (ਕਤਾਰ.size() < max_ਮਾਪ))
	    ਕਤਾਰ.push_back(value);
      ਨਹੀਂਤਾਂ
	    cerr << get_fileline()
	         << "Warning: push_back(\"" << value
	         << "\") skipped for already full bounded queue<string> ["
	         << max_ਮਾਪ << "]." << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_ਕਤਾਰ_string::push_front(ਅਚਲ string&value, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ)
{
      ਜੇ (max_ਮਾਪ && (ਕਤਾਰ.size() == max_ਮਾਪ)) {
	    cerr << get_fileline()
	         << "Warning: push_front(\"" << value << "\") removed \""
	         << ਕਤਾਰ.back() << "\" from already full bounded queue<string> ["
	         << max_ਮਾਪ << "]." << ਸਤਰ_ਅੰਤ;
	    ਕਤਾਰ.pop_back();
      }
      ਕਤਾਰ.push_front(value);
}

ਖਾਲੀ vvp_ਕਤਾਰ_string::erase(ਬਿਨਾਨਿਸ਼ਾਨੀ idx)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਕਤਾਰ.size() > idx);
      ਕਤਾਰ.erase(ਕਤਾਰ.begin()+idx);
}

ਖਾਲੀ vvp_ਕਤਾਰ_string::erase_ਪੂਛ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਕਤਾਰ.size() >= idx);
      ਜੇ (ਕਤਾਰ.size() > idx)
	    ਕਤਾਰ.resize(idx);
}

ਖਾਲੀ vvp_ਕਤਾਰ_real::ਉਲਟ_elems(ਖਾਲੀ)
{
      std::reverse(ਕਤਾਰ.begin(), ਕਤਾਰ.ਸਿਰਾ());
}

ਖਾਲੀ vvp_ਕਤਾਰ_string::ਉਲਟ_elems(ਖਾਲੀ)
{
      std::reverse(ਕਤਾਰ.begin(), ਕਤਾਰ.ਸਿਰਾ());
}

ਖਾਲੀ vvp_ਕਤਾਰ_real::ਛਾਂਟੋ_elems(ਤਾਰਕਿਕ ascending)
{
      ਜੇ (ascending) std::sort(ਕਤਾਰ.begin(), ਕਤਾਰ.ਸਿਰਾ());
      ਨਹੀਂਤਾਂ std::sort(ਕਤਾਰ.begin(), ਕਤਾਰ.ਸਿਰਾ(), std::greater<ਦੁੱਗਣਾ>());
}

ਖਾਲੀ vvp_ਕਤਾਰ_real::shuffle_elems(ਖਾਲੀ)
{
      std::shuffle(ਕਤਾਰ.begin(), ਕਤਾਰ.ਸਿਰਾ(), shuffle_rng());
}

ਖਾਲੀ vvp_ਕਤਾਰ_string::ਛਾਂਟੋ_elems(ਤਾਰਕਿਕ ascending)
{
      ਜੇ (ascending) std::sort(ਕਤਾਰ.begin(), ਕਤਾਰ.ਸਿਰਾ());
      ਨਹੀਂਤਾਂ std::sort(ਕਤਾਰ.begin(), ਕਤਾਰ.ਸਿਰਾ(), std::greater<string>());
}

ਖਾਲੀ vvp_ਕਤਾਰ_string::shuffle_elems(ਖਾਲੀ)
{
      std::shuffle(ਕਤਾਰ.begin(), ਕਤਾਰ.ਸਿਰਾ(), shuffle_rng());
}

ਖਾਲੀ vvp_ਕਤਾਰ_vec4::copy_elems(vvp_object_t src, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ)
{
      ਜੇ (vvp_ਕਤਾਰ*src_ਕਤਾਰ = src.peek<vvp_ਕਤਾਰ>())
	    copy_elements<vvp_vector4_t, vvp_ਕਤਾਰ_vec4, vvp_ਕਤਾਰ>(ਇਹ, src_ਕਤਾਰ, max_ਮਾਪ);
      ਨਹੀਂਤਾਂ ਜੇ (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<vvp_vector4_t, vvp_ਕਤਾਰ_vec4, vvp_darray>(ਇਹ, src_darray, max_ਮਾਪ);
      ਨਹੀਂਤਾਂ
	    cerr << get_fileline() << "Sorry: cannot copy object to vector queue." << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_ਕਤਾਰ_vec4::ਸੈੱਟ_ਸ਼ਬਦ_max(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ vvp_vector4_t&value, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ)
{
      ਜੇ (adr == ਕਤਾਰ.size())
	    ਜੇ (!max_ਮਾਪ || (ਕਤਾਰ.size() < max_ਮਾਪ))
		  ਕਤਾਰ.push_back(value);
	    ਨਹੀਂਤਾਂ
		  cerr << get_fileline()
		       << "Warning: assigning to queue<vector>[" << adr << "] is"
		          " outside bound (" << max_ਮਾਪ << "). " << value
		       << " was not added." << ਸਤਰ_ਅੰਤ;
      ਨਹੀਂਤਾਂ
	    ਸੈੱਟ_ਸ਼ਬਦ(adr, value);
}

ਖਾਲੀ vvp_ਕਤਾਰ_vec4::ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਅਚਲ vvp_vector4_t&value)
{
      ਜੇ (adr < ਕਤਾਰ.size())
	    ਕਤਾਰ[adr] = value;
      ਨਹੀਂਤਾਂ
	    cerr << get_fileline()
	         << "Warning: assigning to queue<vector>[" << adr << "] is outside "
	            "of size (" << ਕਤਾਰ.size() << "). " << value
	         << " was not added." << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_ਕਤਾਰ_vec4::get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, vvp_vector4_t&value)
{
      ਜੇ (adr >= ਕਤਾਰ.size())
	    value = vvp_vector4_t(ਕਤਾਰ[0].size());
      ਨਹੀਂਤਾਂ
	    value = ਕਤਾਰ[adr];
}

ਖਾਲੀ vvp_ਕਤਾਰ_vec4::insert(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਅਚਲ vvp_vector4_t&value, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ)
{
	// Inserting past the end of the queue
      ਜੇ (idx > ਕਤਾਰ.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<vector[" << value.size()
	         << "]>[" << idx << "] is outside of size (" << ਕਤਾਰ.size()
	         << "). " << value << " was not added." << ਸਤਰ_ਅੰਤ;
	// Inserting at the end
      ਨਹੀਂਤਾਂ ਜੇ (idx == ਕਤਾਰ.size())
	    ਜੇ (!max_ਮਾਪ || (ਕਤਾਰ.size() < max_ਮਾਪ))
		  ਕਤਾਰ.push_back(value);
	    ਨਹੀਂਤਾਂ
		  cerr << get_fileline()
		       << "Warning: inserting to queue<vector[" << value.size()
		       << "]>[" << idx << "] is outside bound (" << max_ਮਾਪ
		       << "). " << value << " was not added." << ਸਤਰ_ਅੰਤ;
      ਨਹੀਂਤਾਂ  {
	    ਜੇ (max_ਮਾਪ && (ਕਤਾਰ.size() == max_ਮਾਪ)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << ਕਤਾਰ.back() << " from already full bounded queue<vector["
		       << value.size() << "]> [" << max_ਮਾਪ << "]." << ਸਤਰ_ਅੰਤ;
		  ਕਤਾਰ.pop_back();
	    }
	    ਕਤਾਰ.insert(ਕਤਾਰ.begin()+idx, value);
      }
}

ਖਾਲੀ vvp_ਕਤਾਰ_vec4::push_back(ਅਚਲ vvp_vector4_t&value, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ)
{
      ਜੇ (!max_ਮਾਪ || (ਕਤਾਰ.size() < max_ਮਾਪ))
	    ਕਤਾਰ.push_back(value);
      ਨਹੀਂਤਾਂ
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<vector["
	         << value.size() << "]> [" << max_ਮਾਪ << "]." << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ vvp_ਕਤਾਰ_vec4::push_front(ਅਚਲ vvp_vector4_t&value, ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ)
{
      ਜੇ (max_ਮਾਪ && (ਕਤਾਰ.size() == max_ਮਾਪ)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << ਕਤਾਰ.back() << " from already full bounded queue<vector["
	         << value.size() << "]> [" << max_ਮਾਪ << "]." << ਸਤਰ_ਅੰਤ;
	    ਕਤਾਰ.pop_back();
      }
      ਕਤਾਰ.push_front(value);
}

ਖਾਲੀ vvp_ਕਤਾਰ_vec4::erase(ਬਿਨਾਨਿਸ਼ਾਨੀ idx)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਕਤਾਰ.size() > idx);
      ਕਤਾਰ.erase(ਕਤਾਰ.begin()+idx);
}

ਖਾਲੀ vvp_ਕਤਾਰ_vec4::erase_ਪੂਛ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਕਤਾਰ.size() >= idx);
      ਜੇ (ਕਤਾਰ.size() > idx)
	    ਕਤਾਰ.resize(idx);
}

ਖਾਲੀ vvp_ਕਤਾਰ_vec4::ਉਲਟ_elems(ਖਾਲੀ)
{
      std::reverse(ਕਤਾਰ.begin(), ਕਤਾਰ.ਸਿਰਾ());
}

ਖਾਲੀ vvp_ਕਤਾਰ_vec4::ਛਾਂਟੋ_elems(ਤਾਰਕਿਕ ascending)
{
      ਜੇ (ascending) {
	    std::sort(ਕਤਾਰ.begin(), ਕਤਾਰ.ਸਿਰਾ(), vec4_lt_unsigned);
      } ਨਹੀਂਤਾਂ {
	    std::sort(ਕਤਾਰ.begin(), ਕਤਾਰ.ਸਿਰਾ(),
		      [](ਅਚਲ vvp_vector4_t&a, ਅਚਲ vvp_vector4_t&b) {
			    ਵਾਪਸ vec4_lt_unsigned(b, a);
		      });
      }
}

ਖਾਲੀ vvp_ਕਤਾਰ_vec4::shuffle_elems(ਖਾਲੀ)
{
      std::shuffle(ਕਤਾਰ.begin(), ਕਤਾਰ.ਸਿਰਾ(), shuffle_rng());
}
