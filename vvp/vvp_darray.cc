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

using ନାମାକାଶ std;

ନାମାକାଶ {

ସ୍ଥିର std::mt19937& shuffle_rng(ଶୂନ୍ୟ)
{
      ସ୍ଥିର std::mt19937 rng(std::random_device{}());
      ଫେରନ୍ତୁ rng;
}

ସ୍ଥିର ତାର୍କିକ vec4_lt_unsigned(ଅଚଳ vvp_vector4_t&a, ଅଚଳ vvp_vector4_t&b)
{
      ଯଦି (a.eeq(b)) ଫେରନ୍ତୁ ମିଥ୍ୟା;
      vvp_bit4_t gt = compare_gtge(b, a, BIT4_1);
      ଯଦି (gt == BIT4_X) ଫେରନ୍ତୁ ମିଥ୍ୟା;
      ଫେରନ୍ତୁ gt == BIT4_1;
}

} // namespace

vvp_darray::~vvp_darray()
{
}

ଶୂନ୍ୟ vvp_darray::reverse_elems(ଶୂନ୍ୟ)
{
      cerr << "XXXX reverse_elems() not implemented for "
           << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_darray::sort_elems(ତାର୍କିକ /*ascending*/)
{
      cerr << "XXXX sort_elems() not implemented for "
           << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_darray::shuffle_elems(ଶୂନ୍ୟ)
{
      cerr << "XXXX shuffle_elems() not implemented for "
           << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_darray::set_word(unsigned, ଅଚଳ vvp_vector4_t&)
{
      cerr << "XXXX set_word(vvp_vector4_t) not implemented for " << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_darray::set_word(unsigned, ଦ୍ୱିଗୁଣ)
{
      cerr << "XXXX set_word(double) not implemented for " << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_darray::set_word(unsigned, ଅଚଳ string&)
{
      cerr << "XXXX set_word(string) not implemented for " << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_darray::set_word(unsigned, ଅଚଳ vvp_object_t&)
{
      cerr << "XXXX set_word(vvp_object_t) not implemented for " << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_darray::get_word(unsigned, vvp_vector4_t&)
{
      cerr << "XXXX get_word(vvp_vector4_t) not implemented for " << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_darray::get_word(unsigned, ଦ୍ୱିଗୁଣ&)
{
      cerr << "XXXX get_word(double) not implemented for " << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_darray::get_word(unsigned, string&)
{
      cerr << "XXXX get_word(string) not implemented for " << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_darray::get_word(unsigned, vvp_object_t&)
{
      cerr << "XXXX get_word(vvp_object_t) not implemented for " << typeid(*ଏହି).name() << endl;
}

vvp_vector4_t vvp_darray::get_bitstream(ତାର୍କିକ)
{
      cerr << "XXXX get_bitstream() not implemented for " << typeid(*ଏହି).name() << endl;
      ଫେରନ୍ତୁ vvp_vector4_t();
}

template <ଶ୍ରେଣୀ TYPE> vvp_darray_atom<TYPE>::~vvp_darray_atom()
{
}

template <ଶ୍ରେଣୀ TYPE> size_t vvp_darray_atom<TYPE>::get_size() ଅଚଳ
{
      ଫେରନ୍ତୁ array_.size();
}

template <ଶ୍ରେଣୀ TYPE> ଶୂନ୍ୟ vvp_darray_atom<TYPE>::reverse_elems(ଶୂନ୍ୟ)
{
      size_t n = array_.size();
      ପାଇଁ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

template <ଶ୍ରେଣୀ TYPE> ଶୂନ୍ୟ vvp_darray_atom<TYPE>::sort_elems(ତାର୍କିକ ascending)
{
      ଯଦି (ascending) std::sort(array_.begin(), array_.end());
      ଅନ୍ୟଥା std::sort(array_.begin(), array_.end(), std::greater<TYPE>());
}

template <ଶ୍ରେଣୀ TYPE> ଶୂନ୍ୟ vvp_darray_atom<TYPE>::shuffle_elems(ଶୂନ୍ୟ)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

template <ଶ୍ରେଣୀ TYPE> ଶୂନ୍ୟ vvp_darray_atom<TYPE>::set_word(unsigned adr, ଅଚଳ vvp_vector4_t&value)
{
      ଯଦି (adr >= array_.size())
	    ଫେରନ୍ତୁ;
      TYPE tmp;
      vector4_to_ମୂଲ୍ୟ(value, tmp, ସତ୍ୟ, ମିଥ୍ୟା);
      array_[adr] = tmp;
}

template <ଶ୍ରେଣୀ TYPE> ଶୂନ୍ୟ vvp_darray_atom<TYPE>::get_word(unsigned adr, vvp_vector4_t&value)
{
      ଯଦି (adr >= array_.size()) {
	    value = vvp_vector4_t(8*ଆକାର(TYPE), BIT4_X);
	    ଫେରନ୍ତୁ;
      }

      TYPE word = array_[adr];
      vvp_vector4_t tmp (8*ଆକାର(TYPE), BIT4_0);
      ପାଇଁ (unsigned idx = 0 ; idx < tmp.size() ; idx += 1) {
	    ଯଦି (word&1) tmp.set_bit(idx, BIT4_1);
	    word >>= 1;
      }
      value = tmp;
}

template <ଶ୍ରେଣୀ TYPE> ଶୂନ୍ୟ vvp_darray_atom<TYPE>::shallow_copy(ଅଚଳ vvp_object*obj)
{
      ଅଚଳ vvp_darray_atom<TYPE>*that = dynamic_cast<ଅଚଳ vvp_darray_atom<TYPE>*>(obj);
      ପ୍ରତିଜ୍ଞା(that);

      unsigned ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ = min(array_.size(), that->array_.size());
      ପାଇଁ (unsigned idx = 0 ; idx < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

template <ଶ୍ରେଣୀ TYPE> vvp_object* vvp_darray_atom<TYPE>::duplicate(ଶୂନ୍ୟ) ଅଚଳ
{
      vvp_darray_atom<TYPE>*that = ନୂତନ vvp_darray_atom<TYPE>(array_.size());
      ପାଇଁ (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      ଫେରନ୍ତୁ that;
}

template <ଶ୍ରେଣୀ TYPE> vvp_vector4_t vvp_darray_atom<TYPE>::get_bitstream(ତାର୍କିକ)
{
      ଅଚଳ unsigned word_wid = ଆକାର(TYPE) * 8;

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      ଯେତେବେଳେ (vdx > 0) {
            TYPE word = array_[adx++];
            vdx -= word_wid;
            ପାଇଁ (unsigned bdx = 0; bdx < word_wid; bdx += 1) {
                  ଯଦି (word & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word >>= 1;
            }
      }

      ଫେରନ୍ତୁ vec;
}

template ଶ୍ରେଣୀ vvp_darray_atom<uint8_t>;
template ଶ୍ରେଣୀ vvp_darray_atom<uint16_t>;
template ଶ୍ରେଣୀ vvp_darray_atom<uint32_t>;
template ଶ୍ରେଣୀ vvp_darray_atom<uint64_t>;
template ଶ୍ରେଣୀ vvp_darray_atom<int8_t>;
template ଶ୍ରେଣୀ vvp_darray_atom<int16_t>;
template ଶ୍ରେଣୀ vvp_darray_atom<int32_t>;
template ଶ୍ରେଣୀ vvp_darray_atom<int64_t>;

vvp_darray_vec4::~vvp_darray_vec4()
{
}

size_t vvp_darray_vec4::get_size(ଶୂନ୍ୟ) ଅଚଳ
{
      ଫେରନ୍ତୁ array_.size();
}

ଶୂନ୍ୟ vvp_darray_vec4::set_word(unsigned adr, ଅଚଳ vvp_vector4_t&value)
{
      ଯଦି (adr >= array_.size()) ଫେରନ୍ତୁ;
      ପ୍ରତିଜ୍ଞା(value.size() == word_wid_);
      array_[adr] = value;
}

ଶୂନ୍ୟ vvp_darray_vec4::get_word(unsigned adr, vvp_vector4_t&value)
{
	/*
	 * Return an undefined value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      ଯଦି ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_X);
	    ଫେରନ୍ତୁ;
      }
      value = array_[adr];
      ପ୍ରତିଜ୍ଞା(value.size() == word_wid_);
}

ଶୂନ୍ୟ vvp_darray_vec4::shallow_copy(ଅଚଳ vvp_object*obj)
{
      ଅଚଳ vvp_darray_vec4*that = dynamic_cast<ଅଚଳ vvp_darray_vec4*>(obj);
      ପ୍ରତିଜ୍ଞା(that);

      unsigned ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ = min(array_.size(), that->array_.size());
      ପାଇଁ (unsigned idx = 0 ; idx < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_object* vvp_darray_vec4::duplicate(ଶୂନ୍ୟ) ଅଚଳ
{
      vvp_darray_vec4*that = ନୂତନ vvp_darray_vec4(array_.size(), word_wid_);

      ପାଇଁ (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      ଫେରନ୍ତୁ that;
}

vvp_vector4_t vvp_darray_vec4::get_bitstream(ତାର୍କିକ as_vec4)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      ଯେତେବେଳେ (vdx > 0) {
            vdx -= word_wid_;
            ପାଇଁ (unsigned bdx = 0; bdx < word_wid_; bdx += 1) {
                  vvp_bit4_t bit = array_[adx].value(bdx);
                  ଯଦି (as_vec4 || (bit == BIT4_1))
                        vec.set_bit(vdx+bdx, bit);
            }
            adx++;
      }

      ଫେରନ୍ତୁ vec;
}

ଶୂନ୍ୟ vvp_darray_vec4::reverse_elems(ଶୂନ୍ୟ)
{
      size_t n = array_.size();
      ପାଇଁ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ଶୂନ୍ୟ vvp_darray_vec4::sort_elems(ତାର୍କିକ ascending)
{
      ଯଦି (ascending) {
	    std::sort(array_.begin(), array_.end(), vec4_lt_unsigned);
      } ଅନ୍ୟଥା {
	    std::sort(array_.begin(), array_.end(),
		      [](ଅଚଳ vvp_vector4_t&a, ଅଚଳ vvp_vector4_t&b) {
			    ଫେରନ୍ତୁ vec4_lt_unsigned(b, a);
		      });
      }
}

ଶୂନ୍ୟ vvp_darray_vec4::shuffle_elems(ଶୂନ୍ୟ)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

ଶୂନ୍ୟ vvp_darray_vec2::reverse_elems(ଶୂନ୍ୟ)
{
      size_t n = array_.size();
      ପାଇଁ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ଶୂନ୍ୟ vvp_darray_vec2::sort_elems(ତାର୍କିକ ascending)
{
      ଯଦି (ascending) {
	    std::sort(array_.begin(), array_.end());
      } ଅନ୍ୟଥା {
	    std::sort(array_.begin(), array_.end(),
		      [](ଅଚଳ vvp_vector2_t&a, ଅଚଳ vvp_vector2_t&b) {
			    ଫେରନ୍ତୁ b < a;
		      });
      }
}

ଶୂନ୍ୟ vvp_darray_vec2::shuffle_elems(ଶୂନ୍ୟ)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_darray_vec2::~vvp_darray_vec2()
{
}

size_t vvp_darray_vec2::get_size(ଶୂନ୍ୟ) ଅଚଳ
{
      ଫେରନ୍ତୁ array_.size();
}

ଶୂନ୍ୟ vvp_darray_vec2::set_word(unsigned adr, ଅଚଳ vvp_vector4_t&value)
{
      ଯଦି (adr >= array_.size()) ଫେରନ୍ତୁ;
      ପ୍ରତିଜ୍ଞା(value.size() == word_wid_);
      array_[adr] = value;
}

ଶୂନ୍ୟ vvp_darray_vec2::get_word(unsigned adr, vvp_vector4_t&value)
{
	/*
	 * Return a zero value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      ଯଦି ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_0);
	    ଫେରନ୍ତୁ;
      }
      ପ୍ରତିଜ୍ଞା(array_[adr].size() == word_wid_);
      value.resize(word_wid_);
      ପାଇଁ (unsigned idx = 0; idx < word_wid_; idx += 1) {
	    value.set_bit(idx, array_[adr].value4(idx));
      }
}

ଶୂନ୍ୟ vvp_darray_vec2::shallow_copy(ଅଚଳ vvp_object*obj)
{
      ଅଚଳ vvp_darray_vec2*that = dynamic_cast<ଅଚଳ vvp_darray_vec2*>(obj);
      ପ୍ରତିଜ୍ଞା(that);

      unsigned ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ = min(array_.size(), that->array_.size());
      ପାଇଁ (unsigned idx = 0 ; idx < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_vector4_t vvp_darray_vec2::get_bitstream(ତାର୍କିକ)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      ଯେତେବେଳେ (vdx > 0) {
            vdx -= word_wid_;
            ପାଇଁ (unsigned bdx = 0; bdx < word_wid_; bdx += 1) {
                  ଯଦି (array_[adx].value(bdx))
                        vec.set_bit(vdx+bdx, BIT4_1);
            }
            adx++;
      }

      ଫେରନ୍ତୁ vec;
}

vvp_darray_object::~vvp_darray_object()
{
}

size_t vvp_darray_object::get_size() ଅଚଳ
{
      ଫେରନ୍ତୁ array_.size();
}

ଶୂନ୍ୟ vvp_darray_object::set_word(unsigned adr, ଅଚଳ vvp_object_t&value)
{
      ଯଦି (adr >= array_.size())
	    ଫେରନ୍ତୁ;
      array_[adr] = value;
}

ଶୂନ୍ୟ vvp_darray_object::get_word(unsigned adr, vvp_object_t&value)
{
      ଯଦି (adr >= array_.size()) {
	    value = vvp_object_t();
	    ଫେରନ୍ତୁ;
      }

      value = array_[adr];
}

ଶୂନ୍ୟ vvp_darray_object::shallow_copy(ଅଚଳ vvp_object*obj)
{
      ଅଚଳ vvp_darray_object*that = dynamic_cast<ଅଚଳ vvp_darray_object*>(obj);
      ପ୍ରତିଜ୍ଞା(that);

      unsigned ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ = min(array_.size(), that->array_.size());
      ପାଇଁ (unsigned idx = 0 ; idx < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

ଶୂନ୍ୟ vvp_darray_object::reverse_elems(ଶୂନ୍ୟ)
{
      size_t n = array_.size();
      ପାଇଁ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ଶୂନ୍ୟ vvp_darray_object::sort_elems(ତାର୍କିକ ascending)
{
      ଯଦି (ascending) {
	    std::sort(array_.begin(), array_.end(),
		      [](ଅଚଳ vvp_object_t&a, ଅଚଳ vvp_object_t&b) {
			    ଫେରନ୍ତୁ a.sort_order_ଚାବି() < b.sort_order_ଚାବି();
		      });
      } ଅନ୍ୟଥା {
	    std::sort(array_.begin(), array_.end(),
		      [](ଅଚଳ vvp_object_t&a, ଅଚଳ vvp_object_t&b) {
			    ଫେରନ୍ତୁ a.sort_order_ଚାବି() > b.sort_order_ଚାବି();
		      });
      }
}

ଶୂନ୍ୟ vvp_darray_object::shuffle_elems(ଶୂନ୍ୟ)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_darray_real::~vvp_darray_real()
{
}

size_t vvp_darray_real::get_size() ଅଚଳ
{
      ଫେରନ୍ତୁ array_.size();
}

ଶୂନ୍ୟ vvp_darray_real::set_word(unsigned adr, ଦ୍ୱିଗୁଣ value)
{
      ଯଦି (adr >= array_.size())
	    ଫେରନ୍ତୁ;
      array_[adr] = value;
}

ଶୂନ୍ୟ vvp_darray_real::get_word(unsigned adr, ଦ୍ୱିଗୁଣ&value)
{
      ଯଦି (adr >= array_.size()) {
	    value = 0.0;
	    ଫେରନ୍ତୁ;
      }

      value = array_[adr];
}

ଶୂନ୍ୟ vvp_darray_real::shallow_copy(ଅଚଳ vvp_object*obj)
{
      ଅଚଳ vvp_darray_real*that = dynamic_cast<ଅଚଳ vvp_darray_real*>(obj);
      ପ୍ରତିଜ୍ଞା(that);

      unsigned ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ = min(array_.size(), that->array_.size());
      ପାଇଁ (unsigned idx = 0 ; idx < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

ଶୂନ୍ୟ vvp_darray_real::reverse_elems(ଶୂନ୍ୟ)
{
      size_t n = array_.size();
      ପାଇଁ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ଶୂନ୍ୟ vvp_darray_real::sort_elems(ତାର୍କିକ ascending)
{
      ଯଦି (ascending) std::sort(array_.begin(), array_.end());
      ଅନ୍ୟଥା std::sort(array_.begin(), array_.end(), std::greater<ଦ୍ୱିଗୁଣ>());
}

ଶୂନ୍ୟ vvp_darray_real::shuffle_elems(ଶୂନ୍ୟ)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_real::duplicate(ଶୂନ୍ୟ) ଅଚଳ
{
      vvp_darray_real*that = ନୂତନ vvp_darray_real(array_.size());

      ପାଇଁ (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      ଫେରନ୍ତୁ that;
}

vvp_vector4_t vvp_darray_real::get_bitstream(ତାର୍କିକ)
{
      ଅଚଳ unsigned word_wid = ଆକାର(ଦ୍ୱିଗୁଣ) * 8;
      ପ୍ରତିଜ୍ଞା(word_wid == 64);

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      ଯେତେବେଳେ (vdx > 0) {
            ସଂଘ {
                ଦ୍ୱିଗୁଣ   value;
                uint64_t bits;
            } word;
            word.value = array_[adx++];
            vdx -= word_wid;
            ପାଇଁ (unsigned bdx = 0; bdx < word_wid; bdx += 1) {
                  ଯଦି (word.bits & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word.bits >>= 1;
            }
      }

      ଫେରନ୍ତୁ vec;
}

vvp_darray_string::~vvp_darray_string()
{
}

size_t vvp_darray_string::get_size() ଅଚଳ
{
      ଫେରନ୍ତୁ array_.size();
}

ଶୂନ୍ୟ vvp_darray_string::set_word(unsigned adr, ଅଚଳ string&value)
{
      ଯଦି (adr >= array_.size())
	    ଫେରନ୍ତୁ;
      array_[adr] = value;
}

ଶୂନ୍ୟ vvp_darray_string::get_word(unsigned adr, string&value)
{
      ଯଦି (adr >= array_.size()) {
	    value = "";
	    ଫେରନ୍ତୁ;
      }

      value = array_[adr];
}

ଶୂନ୍ୟ vvp_darray_string::shallow_copy(ଅଚଳ vvp_object*obj)
{
      ଅଚଳ vvp_darray_string*that = dynamic_cast<ଅଚଳ vvp_darray_string*>(obj);
      ପ୍ରତିଜ୍ଞା(that);

      unsigned ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ = min(array_.size(), that->array_.size());
      ପାଇଁ (unsigned idx = 0 ; idx < ଅଙ୍କ_ବସ୍ତୁଗୁଡ଼ିକ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

ଶୂନ୍ୟ vvp_darray_string::reverse_elems(ଶୂନ୍ୟ)
{
      size_t n = array_.size();
      ପାଇଁ (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ଶୂନ୍ୟ vvp_darray_string::sort_elems(ତାର୍କିକ ascending)
{
      ଯଦି (ascending) std::sort(array_.begin(), array_.end());
      ଅନ୍ୟଥା std::sort(array_.begin(), array_.end(), std::greater<string>());
}

ଶୂନ୍ୟ vvp_darray_string::shuffle_elems(ଶୂନ୍ୟ)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_string::duplicate(ଶୂନ୍ୟ) ଅଚଳ
{
      vvp_darray_string*that = ନୂତନ vvp_darray_string(array_.size());

      ପାଇଁ (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      ଫେରନ୍ତୁ that;
}

vvp_queue::~vvp_queue()
{
}

ଶୂନ୍ୟ vvp_queue::copy_elems(vvp_object_t, unsigned)
{
      cerr << "Sorry: copy_elems() not implemented for " << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_queue::set_word_max(unsigned, ଅଚଳ vvp_vector4_t&, unsigned)
{
      cerr << "XXXX set_word_max(vvp_vector4_t) not implemented for " << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_queue::insert(unsigned, ଅଚଳ vvp_vector4_t&, unsigned)
{
      cerr << "XXXX insert(vvp_vector4_t) not implemented for " << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_queue::push_back(ଅଚଳ vvp_vector4_t&, unsigned)
{
      cerr << "XXXX push_back(vvp_vector4_t) not implemented for " << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_queue::push_front(ଅଚଳ vvp_vector4_t&, unsigned)
{
      cerr << "XXXX push_front(vvp_vector4_t) not implemented for " << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_queue::set_word_max(unsigned, ଦ୍ୱିଗୁଣ, unsigned)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_queue::insert(unsigned, ଦ୍ୱିଗୁଣ, unsigned)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_queue::push_back(ଦ୍ୱିଗୁଣ, unsigned)
{
      cerr << "XXXX push_back(double) not implemented for " << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_queue::push_front(ଦ୍ୱିଗୁଣ, unsigned)
{
      cerr << "XXXX push_front(double) not implemented for " << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_queue::set_word_max(unsigned, ଅଚଳ string&, unsigned)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_queue::insert(unsigned, ଅଚଳ string&, unsigned)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_queue::push_back(ଅଚଳ string&, unsigned)
{
      cerr << "XXXX push_back(string) not implemented for " << typeid(*ଏହି).name() << endl;
}

ଶୂନ୍ୟ vvp_queue::push_front(ଅଚଳ string&, unsigned)
{
      cerr << "XXXX push_front(string) not implemented for " << typeid(*ଏହି).name() << endl;
}

/*
 * Helper functions used while copying multiple elements into a queue.
 */
ସ୍ଥିର ଶୂନ୍ୟ print_copy_is_too_big(size_t src_size, unsigned max_size, ଅଚଳ string&qtype)
{
      cerr << get_fileline()
           << "Warning: queue<" << qtype << "> is bounded to have at most "
           << max_size << " elements, source has " << src_size << " elements." << endl;
}

ସ୍ଥିର ଶୂନ୍ୟ print_copy_is_too_big(ଦ୍ୱିଗୁଣ&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "real");
}

ସ୍ଥିର ଶୂନ୍ୟ print_copy_is_too_big(string&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "string");
}

ସ୍ଥିର ଶୂନ୍ୟ print_copy_is_too_big(vvp_vector4_t&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "vector");
}

template <typename ELEM, ଶ୍ରେଣୀ QTYPE, ଶ୍ରେଣୀ SRC_TYPE>
ସ୍ଥିର ଶୂନ୍ୟ copy_elements(QTYPE*queue, SRC_TYPE*src, unsigned max_size)
{
      size_t src_size = src->get_size();
      ଯଦି ((max_size != 0) && (src_size > max_size)) {
	    ELEM tmp;
	    print_copy_is_too_big(tmp, src_size, max_size);
      }
      unsigned copy_size = ((src_size < max_size) ||
                            (max_size == 0)) ? src_size : max_size;
      ଯଦି (copy_size < queue->get_size())
	    queue->erase_tail(copy_size);
      ପାଇଁ (unsigned idx=0; idx < copy_size; ++idx) {
	    ELEM value;
	    src->get_word(idx, value);
	    queue->set_word_max(idx, value, max_size);
      }
}

ଶୂନ୍ୟ vvp_queue_real::copy_elems(vvp_object_t src, unsigned max_size)
{
      ଯଦି (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<ଦ୍ୱିଗୁଣ, vvp_queue_real, vvp_queue>(ଏହି, src_queue, max_size);
      ଅନ୍ୟଥା ଯଦି (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<ଦ୍ୱିଗୁଣ, vvp_queue_real, vvp_darray>(ଏହି, src_darray, max_size);
      ଅନ୍ୟଥା
	    cerr << get_fileline() << "Sorry: cannot copy object to real queue." << endl;
}

ଶୂନ୍ୟ vvp_queue_real::set_word_max(unsigned adr, ଦ୍ୱିଗୁଣ value, unsigned max_size)
{
      ଯଦି (adr == queue.size())
	    ଯଦି (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    ଅନ୍ୟଥା
		  cerr << get_fileline()
		       << "Warning: assigning to queue<real>[" << adr << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      ଅନ୍ୟଥା
	    set_word(adr, value);
}

ଶୂନ୍ୟ vvp_queue_real::set_word(unsigned adr, ଦ୍ୱିଗୁଣ value)
{
      ଯଦି (adr < queue.size())
	    queue[adr] = value;
      ଅନ୍ୟଥା
	    cerr << get_fileline()
	         << "Warning: assigning to queue<real>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
}

ଶୂନ୍ୟ vvp_queue_real::get_word(unsigned adr, ଦ୍ୱିଗୁଣ&value)
{
      ଯଦି (adr >= queue.size())
	    value = 0.0;
      ଅନ୍ୟଥା
	    value = queue[adr];
}

ଶୂନ୍ୟ vvp_queue_real::insert(unsigned idx, ଦ୍ୱିଗୁଣ value, unsigned max_size)
{
	// Inserting past the end of the queue
      ଯଦି (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<real>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
	// Inserting at the end
      ଅନ୍ୟଥା ଯଦି (idx == queue.size())
	    ଯଦି (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    ଅନ୍ୟଥା
		  cerr << get_fileline()
		       << "Warning: inserting to queue<real>[" << idx << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      ଅନ୍ୟଥା  {
	    ଯଦି (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<real> ["
		       << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

ଶୂନ୍ୟ vvp_queue_real::push_back(ଦ୍ୱିଗୁଣ value, unsigned max_size)
{
      ଯଦି (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      ଅନ୍ୟଥା
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<real> ["
	         << max_size << "]." << endl;
}

ଶୂନ୍ୟ vvp_queue_real::push_front(ଦ୍ୱିଗୁଣ value, unsigned max_size)
{
      ଯଦି (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<real> ["
	         << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

ଶୂନ୍ୟ vvp_queue_real::erase(unsigned idx)
{
      ପ୍ରତିଜ୍ଞା(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

ଶୂନ୍ୟ vvp_queue_real::erase_tail(unsigned idx)
{
      ପ୍ରତିଜ୍ଞା(queue.size() >= idx);
      ଯଦି (queue.size() > idx)
	    queue.resize(idx);
}

ଶୂନ୍ୟ vvp_queue_string::copy_elems(vvp_object_t src, unsigned max_size)
{
      ଯଦି (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<string, vvp_queue_string, vvp_queue>(ଏହି, src_queue, max_size);
      ଅନ୍ୟଥା ଯଦି (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<string, vvp_queue_string, vvp_darray>(ଏହି, src_darray, max_size);
      ଅନ୍ୟଥା
	    cerr << get_fileline() << "Sorry: cannot copy object to string queue." << endl;
}

ଶୂନ୍ୟ vvp_queue_string::set_word_max(unsigned adr, ଅଚଳ string&value, unsigned max_size)
{
      ଯଦି (adr == queue.size())
	    ଯଦି (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    ଅନ୍ୟଥା
		  cerr << get_fileline()
		       << "Warning: assigning to queue<string>[" << adr << "] is"
		          " outside bound (" << max_size << "). \"" << value
		       << "\" was not added." << endl;
      ଅନ୍ୟଥା
	    set_word(adr, value);
}

ଶୂନ୍ୟ vvp_queue_string::set_word(unsigned adr, ଅଚଳ string&value)
{
      ଯଦି (adr < queue.size())
	    queue[adr] = value;
      ଅନ୍ୟଥା
	    cerr << get_fileline()
	         << "Warning: assigning to queue<string>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << endl;
}

ଶୂନ୍ୟ vvp_queue_string::get_word(unsigned adr, string&value)
{
      ଯଦି (adr >= queue.size())
	    value = "";
      ଅନ୍ୟଥା
	    value = queue[adr];
}

ଶୂନ୍ୟ vvp_queue_string::insert(unsigned idx, ଅଚଳ string&value, unsigned max_size)
{
	// Inserting past the end of the queue
      ଯଦି (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<string>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << endl;
	// Inserting at the end
      ଅନ୍ୟଥା ଯଦି (idx == queue.size())
	    ଯଦି (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    ଅନ୍ୟଥା
		  cerr << get_fileline()
		       << "Warning: inserting to queue<string>[" << idx << "] is"
		          " outside bound (" << max_size << "). \"" << value
		       << "\" was not added." << endl;
      ଅନ୍ୟଥା  {
	    ଯଦି (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", \"" << value << "\") removed \""
		       << queue.back() << "\" from already full bounded queue<string> ["
		       << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

ଶୂନ୍ୟ vvp_queue_string::push_back(ଅଚଳ string&value, unsigned max_size)
{
      ଯଦି (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      ଅନ୍ୟଥା
	    cerr << get_fileline()
	         << "Warning: push_back(\"" << value
	         << "\") skipped for already full bounded queue<string> ["
	         << max_size << "]." << endl;
}

ଶୂନ୍ୟ vvp_queue_string::push_front(ଅଚଳ string&value, unsigned max_size)
{
      ଯଦି (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(\"" << value << "\") removed \""
	         << queue.back() << "\" from already full bounded queue<string> ["
	         << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

ଶୂନ୍ୟ vvp_queue_string::erase(unsigned idx)
{
      ପ୍ରତିଜ୍ଞା(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

ଶୂନ୍ୟ vvp_queue_string::erase_tail(unsigned idx)
{
      ପ୍ରତିଜ୍ଞା(queue.size() >= idx);
      ଯଦି (queue.size() > idx)
	    queue.resize(idx);
}

ଶୂନ୍ୟ vvp_queue_real::reverse_elems(ଶୂନ୍ୟ)
{
      std::reverse(queue.begin(), queue.end());
}

ଶୂନ୍ୟ vvp_queue_string::reverse_elems(ଶୂନ୍ୟ)
{
      std::reverse(queue.begin(), queue.end());
}

ଶୂନ୍ୟ vvp_queue_real::sort_elems(ତାର୍କିକ ascending)
{
      ଯଦି (ascending) std::sort(queue.begin(), queue.end());
      ଅନ୍ୟଥା std::sort(queue.begin(), queue.end(), std::greater<ଦ୍ୱିଗୁଣ>());
}

ଶୂନ୍ୟ vvp_queue_real::shuffle_elems(ଶୂନ୍ୟ)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

ଶୂନ୍ୟ vvp_queue_string::sort_elems(ତାର୍କିକ ascending)
{
      ଯଦି (ascending) std::sort(queue.begin(), queue.end());
      ଅନ୍ୟଥା std::sort(queue.begin(), queue.end(), std::greater<string>());
}

ଶୂନ୍ୟ vvp_queue_string::shuffle_elems(ଶୂନ୍ୟ)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

ଶୂନ୍ୟ vvp_queue_vec4::copy_elems(vvp_object_t src, unsigned max_size)
{
      ଯଦି (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_queue>(ଏହି, src_queue, max_size);
      ଅନ୍ୟଥା ଯଦି (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_darray>(ଏହି, src_darray, max_size);
      ଅନ୍ୟଥା
	    cerr << get_fileline() << "Sorry: cannot copy object to vector queue." << endl;
}

ଶୂନ୍ୟ vvp_queue_vec4::set_word_max(unsigned adr, ଅଚଳ vvp_vector4_t&value, unsigned max_size)
{
      ଯଦି (adr == queue.size())
	    ଯଦି (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    ଅନ୍ୟଥା
		  cerr << get_fileline()
		       << "Warning: assigning to queue<vector>[" << adr << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      ଅନ୍ୟଥା
	    set_word(adr, value);
}

ଶୂନ୍ୟ vvp_queue_vec4::set_word(unsigned adr, ଅଚଳ vvp_vector4_t&value)
{
      ଯଦି (adr < queue.size())
	    queue[adr] = value;
      ଅନ୍ୟଥା
	    cerr << get_fileline()
	         << "Warning: assigning to queue<vector>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
}

ଶୂନ୍ୟ vvp_queue_vec4::get_word(unsigned adr, vvp_vector4_t&value)
{
      ଯଦି (adr >= queue.size())
	    value = vvp_vector4_t(queue[0].size());
      ଅନ୍ୟଥା
	    value = queue[adr];
}

ଶୂନ୍ୟ vvp_queue_vec4::insert(unsigned idx, ଅଚଳ vvp_vector4_t&value, unsigned max_size)
{
	// Inserting past the end of the queue
      ଯଦି (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<vector[" << value.size()
	         << "]>[" << idx << "] is outside of size (" << queue.size()
	         << "). " << value << " was not added." << endl;
	// Inserting at the end
      ଅନ୍ୟଥା ଯଦି (idx == queue.size())
	    ଯଦି (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    ଅନ୍ୟଥା
		  cerr << get_fileline()
		       << "Warning: inserting to queue<vector[" << value.size()
		       << "]>[" << idx << "] is outside bound (" << max_size
		       << "). " << value << " was not added." << endl;
      ଅନ୍ୟଥା  {
	    ଯଦି (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<vector["
		       << value.size() << "]> [" << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

ଶୂନ୍ୟ vvp_queue_vec4::push_back(ଅଚଳ vvp_vector4_t&value, unsigned max_size)
{
      ଯଦି (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      ଅନ୍ୟଥା
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<vector["
	         << value.size() << "]> [" << max_size << "]." << endl;
}

ଶୂନ୍ୟ vvp_queue_vec4::push_front(ଅଚଳ vvp_vector4_t&value, unsigned max_size)
{
      ଯଦି (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<vector["
	         << value.size() << "]> [" << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

ଶୂନ୍ୟ vvp_queue_vec4::erase(unsigned idx)
{
      ପ୍ରତିଜ୍ଞା(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

ଶୂନ୍ୟ vvp_queue_vec4::erase_tail(unsigned idx)
{
      ପ୍ରତିଜ୍ଞା(queue.size() >= idx);
      ଯଦି (queue.size() > idx)
	    queue.resize(idx);
}

ଶୂନ୍ୟ vvp_queue_vec4::reverse_elems(ଶୂନ୍ୟ)
{
      std::reverse(queue.begin(), queue.end());
}

ଶୂନ୍ୟ vvp_queue_vec4::sort_elems(ତାର୍କିକ ascending)
{
      ଯଦି (ascending) {
	    std::sort(queue.begin(), queue.end(), vec4_lt_unsigned);
      } ଅନ୍ୟଥା {
	    std::sort(queue.begin(), queue.end(),
		      [](ଅଚଳ vvp_vector4_t&a, ଅଚଳ vvp_vector4_t&b) {
			    ଫେରନ୍ତୁ vec4_lt_unsigned(b, a);
		      });
      }
}

ଶୂନ୍ୟ vvp_queue_vec4::shuffle_elems(ଶୂନ୍ୟ)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}
