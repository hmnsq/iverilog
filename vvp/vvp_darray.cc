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

using നാമാകാശ std;

നാമാകാശ {

സ്ഥിതം std::mt19937& shuffle_rng(ശൂന്യം)
{
      സ്ഥിതം std::mt19937 rng(std::random_device{}());
      തിരികെ rng;
}

സ്ഥിതം തർക്കം vec4_lt_unsigned(സ്ഥിരം vvp_vector4_t&a, സ്ഥിരം vvp_vector4_t&b)
{
      എങ്കിൽ (a.eeq(b)) തിരികെ അസത്യം;
      vvp_bit4_t gt = compare_gtge(b, a, BIT4_1);
      എങ്കിൽ (gt == BIT4_X) തിരികെ അസത്യം;
      തിരികെ gt == BIT4_1;
}

} // namespace

vvp_darray::~vvp_darray()
{
}

ശൂന്യം vvp_darray::reverse_elems(ശൂന്യം)
{
      cerr << "XXXX reverse_elems() not implemented for "
           << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_darray::sort_elems(തർക്കം /*ascending*/)
{
      cerr << "XXXX sort_elems() not implemented for "
           << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_darray::shuffle_elems(ശൂന്യം)
{
      cerr << "XXXX shuffle_elems() not implemented for "
           << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_darray::set_word(unsigned, സ്ഥിരം vvp_vector4_t&)
{
      cerr << "XXXX set_word(vvp_vector4_t) not implemented for " << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_darray::set_word(unsigned, ഇരട്ട)
{
      cerr << "XXXX set_word(double) not implemented for " << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_darray::set_word(unsigned, സ്ഥിരം string&)
{
      cerr << "XXXX set_word(string) not implemented for " << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_darray::set_word(unsigned, സ്ഥിരം vvp_object_t&)
{
      cerr << "XXXX set_word(vvp_object_t) not implemented for " << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_darray::get_word(unsigned, vvp_vector4_t&)
{
      cerr << "XXXX get_word(vvp_vector4_t) not implemented for " << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_darray::get_word(unsigned, ഇരട്ട&)
{
      cerr << "XXXX get_word(double) not implemented for " << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_darray::get_word(unsigned, string&)
{
      cerr << "XXXX get_word(string) not implemented for " << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_darray::get_word(unsigned, vvp_object_t&)
{
      cerr << "XXXX get_word(vvp_object_t) not implemented for " << typeid(*ഇത്).name() << endl;
}

vvp_vector4_t vvp_darray::get_bitstream(തർക്കം)
{
      cerr << "XXXX get_bitstream() not implemented for " << typeid(*ഇത്).name() << endl;
      തിരികെ vvp_vector4_t();
}

template <വർഗ്ഗം TYPE> vvp_darray_atom<TYPE>::~vvp_darray_atom()
{
}

template <വർഗ്ഗം TYPE> size_t vvp_darray_atom<TYPE>::get_size() സ്ഥിരം
{
      തിരികെ array_.size();
}

template <വർഗ്ഗം TYPE> ശൂന്യം vvp_darray_atom<TYPE>::reverse_elems(ശൂന്യം)
{
      size_t n = array_.size();
      വേണ്ടി (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

template <വർഗ്ഗം TYPE> ശൂന്യം vvp_darray_atom<TYPE>::sort_elems(തർക്കം ascending)
{
      എങ്കിൽ (ascending) std::sort(array_.begin(), array_.end());
      അല്ലെങ്കിൽ std::sort(array_.begin(), array_.end(), std::greater<TYPE>());
}

template <വർഗ്ഗം TYPE> ശൂന്യം vvp_darray_atom<TYPE>::shuffle_elems(ശൂന്യം)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

template <വർഗ്ഗം TYPE> ശൂന്യം vvp_darray_atom<TYPE>::set_word(unsigned adr, സ്ഥിരം vvp_vector4_t&value)
{
      എങ്കിൽ (adr >= array_.size())
	    തിരികെ;
      TYPE tmp;
      vector4_to_മൂല്യം(value, tmp, സത്യം, അസത്യം);
      array_[adr] = tmp;
}

template <വർഗ്ഗം TYPE> ശൂന്യം vvp_darray_atom<TYPE>::get_word(unsigned adr, vvp_vector4_t&value)
{
      എങ്കിൽ (adr >= array_.size()) {
	    value = vvp_vector4_t(8*വലുപ്പം(TYPE), BIT4_X);
	    തിരികെ;
      }

      TYPE word = array_[adr];
      vvp_vector4_t tmp (8*വലുപ്പം(TYPE), BIT4_0);
      വേണ്ടി (unsigned idx = 0 ; idx < tmp.size() ; idx += 1) {
	    എങ്കിൽ (word&1) tmp.set_bit(idx, BIT4_1);
	    word >>= 1;
      }
      value = tmp;
}

template <വർഗ്ഗം TYPE> ശൂന്യം vvp_darray_atom<TYPE>::shallow_copy(സ്ഥിരം vvp_object*obj)
{
      സ്ഥിരം vvp_darray_atom<TYPE>*that = dynamic_cast<സ്ഥിരം vvp_darray_atom<TYPE>*>(obj);
      പ്രതിജ്ഞാ(that);

      unsigned അക്കം_ഇനങ്ങൾ = min(array_.size(), that->array_.size());
      വേണ്ടി (unsigned idx = 0 ; idx < അക്കം_ഇനങ്ങൾ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

template <വർഗ്ഗം TYPE> vvp_object* vvp_darray_atom<TYPE>::duplicate(ശൂന്യം) സ്ഥിരം
{
      vvp_darray_atom<TYPE>*that = പുതിയ vvp_darray_atom<TYPE>(array_.size());
      വേണ്ടി (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      തിരികെ that;
}

template <വർഗ്ഗം TYPE> vvp_vector4_t vvp_darray_atom<TYPE>::get_bitstream(തർക്കം)
{
      സ്ഥിരം unsigned word_wid = വലുപ്പം(TYPE) * 8;

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      ആകുമ്പോൾ (vdx > 0) {
            TYPE word = array_[adx++];
            vdx -= word_wid;
            വേണ്ടി (unsigned bdx = 0; bdx < word_wid; bdx += 1) {
                  എങ്കിൽ (word & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word >>= 1;
            }
      }

      തിരികെ vec;
}

template വർഗ്ഗം vvp_darray_atom<uint8_t>;
template വർഗ്ഗം vvp_darray_atom<uint16_t>;
template വർഗ്ഗം vvp_darray_atom<uint32_t>;
template വർഗ്ഗം vvp_darray_atom<uint64_t>;
template വർഗ്ഗം vvp_darray_atom<int8_t>;
template വർഗ്ഗം vvp_darray_atom<int16_t>;
template വർഗ്ഗം vvp_darray_atom<int32_t>;
template വർഗ്ഗം vvp_darray_atom<int64_t>;

vvp_darray_vec4::~vvp_darray_vec4()
{
}

size_t vvp_darray_vec4::get_size(ശൂന്യം) സ്ഥിരം
{
      തിരികെ array_.size();
}

ശൂന്യം vvp_darray_vec4::set_word(unsigned adr, സ്ഥിരം vvp_vector4_t&value)
{
      എങ്കിൽ (adr >= array_.size()) തിരികെ;
      പ്രതിജ്ഞാ(value.size() == word_wid_);
      array_[adr] = value;
}

ശൂന്യം vvp_darray_vec4::get_word(unsigned adr, vvp_vector4_t&value)
{
	/*
	 * Return an undefined value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      എങ്കിൽ ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_X);
	    തിരികെ;
      }
      value = array_[adr];
      പ്രതിജ്ഞാ(value.size() == word_wid_);
}

ശൂന്യം vvp_darray_vec4::shallow_copy(സ്ഥിരം vvp_object*obj)
{
      സ്ഥിരം vvp_darray_vec4*that = dynamic_cast<സ്ഥിരം vvp_darray_vec4*>(obj);
      പ്രതിജ്ഞാ(that);

      unsigned അക്കം_ഇനങ്ങൾ = min(array_.size(), that->array_.size());
      വേണ്ടി (unsigned idx = 0 ; idx < അക്കം_ഇനങ്ങൾ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_object* vvp_darray_vec4::duplicate(ശൂന്യം) സ്ഥിരം
{
      vvp_darray_vec4*that = പുതിയ vvp_darray_vec4(array_.size(), word_wid_);

      വേണ്ടി (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      തിരികെ that;
}

vvp_vector4_t vvp_darray_vec4::get_bitstream(തർക്കം as_vec4)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      ആകുമ്പോൾ (vdx > 0) {
            vdx -= word_wid_;
            വേണ്ടി (unsigned bdx = 0; bdx < word_wid_; bdx += 1) {
                  vvp_bit4_t bit = array_[adx].value(bdx);
                  എങ്കിൽ (as_vec4 || (bit == BIT4_1))
                        vec.set_bit(vdx+bdx, bit);
            }
            adx++;
      }

      തിരികെ vec;
}

ശൂന്യം vvp_darray_vec4::reverse_elems(ശൂന്യം)
{
      size_t n = array_.size();
      വേണ്ടി (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ശൂന്യം vvp_darray_vec4::sort_elems(തർക്കം ascending)
{
      എങ്കിൽ (ascending) {
	    std::sort(array_.begin(), array_.end(), vec4_lt_unsigned);
      } അല്ലെങ്കിൽ {
	    std::sort(array_.begin(), array_.end(),
		      [](സ്ഥിരം vvp_vector4_t&a, സ്ഥിരം vvp_vector4_t&b) {
			    തിരികെ vec4_lt_unsigned(b, a);
		      });
      }
}

ശൂന്യം vvp_darray_vec4::shuffle_elems(ശൂന്യം)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

ശൂന്യം vvp_darray_vec2::reverse_elems(ശൂന്യം)
{
      size_t n = array_.size();
      വേണ്ടി (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ശൂന്യം vvp_darray_vec2::sort_elems(തർക്കം ascending)
{
      എങ്കിൽ (ascending) {
	    std::sort(array_.begin(), array_.end());
      } അല്ലെങ്കിൽ {
	    std::sort(array_.begin(), array_.end(),
		      [](സ്ഥിരം vvp_vector2_t&a, സ്ഥിരം vvp_vector2_t&b) {
			    തിരികെ b < a;
		      });
      }
}

ശൂന്യം vvp_darray_vec2::shuffle_elems(ശൂന്യം)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_darray_vec2::~vvp_darray_vec2()
{
}

size_t vvp_darray_vec2::get_size(ശൂന്യം) സ്ഥിരം
{
      തിരികെ array_.size();
}

ശൂന്യം vvp_darray_vec2::set_word(unsigned adr, സ്ഥിരം vvp_vector4_t&value)
{
      എങ്കിൽ (adr >= array_.size()) തിരികെ;
      പ്രതിജ്ഞാ(value.size() == word_wid_);
      array_[adr] = value;
}

ശൂന്യം vvp_darray_vec2::get_word(unsigned adr, vvp_vector4_t&value)
{
	/*
	 * Return a zero value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      എങ്കിൽ ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_0);
	    തിരികെ;
      }
      പ്രതിജ്ഞാ(array_[adr].size() == word_wid_);
      value.resize(word_wid_);
      വേണ്ടി (unsigned idx = 0; idx < word_wid_; idx += 1) {
	    value.set_bit(idx, array_[adr].value4(idx));
      }
}

ശൂന്യം vvp_darray_vec2::shallow_copy(സ്ഥിരം vvp_object*obj)
{
      സ്ഥിരം vvp_darray_vec2*that = dynamic_cast<സ്ഥിരം vvp_darray_vec2*>(obj);
      പ്രതിജ്ഞാ(that);

      unsigned അക്കം_ഇനങ്ങൾ = min(array_.size(), that->array_.size());
      വേണ്ടി (unsigned idx = 0 ; idx < അക്കം_ഇനങ്ങൾ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_vector4_t vvp_darray_vec2::get_bitstream(തർക്കം)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      ആകുമ്പോൾ (vdx > 0) {
            vdx -= word_wid_;
            വേണ്ടി (unsigned bdx = 0; bdx < word_wid_; bdx += 1) {
                  എങ്കിൽ (array_[adx].value(bdx))
                        vec.set_bit(vdx+bdx, BIT4_1);
            }
            adx++;
      }

      തിരികെ vec;
}

vvp_darray_object::~vvp_darray_object()
{
}

size_t vvp_darray_object::get_size() സ്ഥിരം
{
      തിരികെ array_.size();
}

ശൂന്യം vvp_darray_object::set_word(unsigned adr, സ്ഥിരം vvp_object_t&value)
{
      എങ്കിൽ (adr >= array_.size())
	    തിരികെ;
      array_[adr] = value;
}

ശൂന്യം vvp_darray_object::get_word(unsigned adr, vvp_object_t&value)
{
      എങ്കിൽ (adr >= array_.size()) {
	    value = vvp_object_t();
	    തിരികെ;
      }

      value = array_[adr];
}

ശൂന്യം vvp_darray_object::shallow_copy(സ്ഥിരം vvp_object*obj)
{
      സ്ഥിരം vvp_darray_object*that = dynamic_cast<സ്ഥിരം vvp_darray_object*>(obj);
      പ്രതിജ്ഞാ(that);

      unsigned അക്കം_ഇനങ്ങൾ = min(array_.size(), that->array_.size());
      വേണ്ടി (unsigned idx = 0 ; idx < അക്കം_ഇനങ്ങൾ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

ശൂന്യം vvp_darray_object::reverse_elems(ശൂന്യം)
{
      size_t n = array_.size();
      വേണ്ടി (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ശൂന്യം vvp_darray_object::sort_elems(തർക്കം ascending)
{
      എങ്കിൽ (ascending) {
	    std::sort(array_.begin(), array_.end(),
		      [](സ്ഥിരം vvp_object_t&a, സ്ഥിരം vvp_object_t&b) {
			    തിരികെ a.sort_order_താക്കോൽ() < b.sort_order_താക്കോൽ();
		      });
      } അല്ലെങ്കിൽ {
	    std::sort(array_.begin(), array_.end(),
		      [](സ്ഥിരം vvp_object_t&a, സ്ഥിരം vvp_object_t&b) {
			    തിരികെ a.sort_order_താക്കോൽ() > b.sort_order_താക്കോൽ();
		      });
      }
}

ശൂന്യം vvp_darray_object::shuffle_elems(ശൂന്യം)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_darray_real::~vvp_darray_real()
{
}

size_t vvp_darray_real::get_size() സ്ഥിരം
{
      തിരികെ array_.size();
}

ശൂന്യം vvp_darray_real::set_word(unsigned adr, ഇരട്ട value)
{
      എങ്കിൽ (adr >= array_.size())
	    തിരികെ;
      array_[adr] = value;
}

ശൂന്യം vvp_darray_real::get_word(unsigned adr, ഇരട്ട&value)
{
      എങ്കിൽ (adr >= array_.size()) {
	    value = 0.0;
	    തിരികെ;
      }

      value = array_[adr];
}

ശൂന്യം vvp_darray_real::shallow_copy(സ്ഥിരം vvp_object*obj)
{
      സ്ഥിരം vvp_darray_real*that = dynamic_cast<സ്ഥിരം vvp_darray_real*>(obj);
      പ്രതിജ്ഞാ(that);

      unsigned അക്കം_ഇനങ്ങൾ = min(array_.size(), that->array_.size());
      വേണ്ടി (unsigned idx = 0 ; idx < അക്കം_ഇനങ്ങൾ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

ശൂന്യം vvp_darray_real::reverse_elems(ശൂന്യം)
{
      size_t n = array_.size();
      വേണ്ടി (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ശൂന്യം vvp_darray_real::sort_elems(തർക്കം ascending)
{
      എങ്കിൽ (ascending) std::sort(array_.begin(), array_.end());
      അല്ലെങ്കിൽ std::sort(array_.begin(), array_.end(), std::greater<ഇരട്ട>());
}

ശൂന്യം vvp_darray_real::shuffle_elems(ശൂന്യം)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_real::duplicate(ശൂന്യം) സ്ഥിരം
{
      vvp_darray_real*that = പുതിയ vvp_darray_real(array_.size());

      വേണ്ടി (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      തിരികെ that;
}

vvp_vector4_t vvp_darray_real::get_bitstream(തർക്കം)
{
      സ്ഥിരം unsigned word_wid = വലുപ്പം(ഇരട്ട) * 8;
      പ്രതിജ്ഞാ(word_wid == 64);

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      ആകുമ്പോൾ (vdx > 0) {
            സംഘം {
                ഇരട്ട   value;
                uint64_t bits;
            } word;
            word.value = array_[adx++];
            vdx -= word_wid;
            വേണ്ടി (unsigned bdx = 0; bdx < word_wid; bdx += 1) {
                  എങ്കിൽ (word.bits & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word.bits >>= 1;
            }
      }

      തിരികെ vec;
}

vvp_darray_string::~vvp_darray_string()
{
}

size_t vvp_darray_string::get_size() സ്ഥിരം
{
      തിരികെ array_.size();
}

ശൂന്യം vvp_darray_string::set_word(unsigned adr, സ്ഥിരം string&value)
{
      എങ്കിൽ (adr >= array_.size())
	    തിരികെ;
      array_[adr] = value;
}

ശൂന്യം vvp_darray_string::get_word(unsigned adr, string&value)
{
      എങ്കിൽ (adr >= array_.size()) {
	    value = "";
	    തിരികെ;
      }

      value = array_[adr];
}

ശൂന്യം vvp_darray_string::shallow_copy(സ്ഥിരം vvp_object*obj)
{
      സ്ഥിരം vvp_darray_string*that = dynamic_cast<സ്ഥിരം vvp_darray_string*>(obj);
      പ്രതിജ്ഞാ(that);

      unsigned അക്കം_ഇനങ്ങൾ = min(array_.size(), that->array_.size());
      വേണ്ടി (unsigned idx = 0 ; idx < അക്കം_ഇനങ്ങൾ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

ശൂന്യം vvp_darray_string::reverse_elems(ശൂന്യം)
{
      size_t n = array_.size();
      വേണ്ടി (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ശൂന്യം vvp_darray_string::sort_elems(തർക്കം ascending)
{
      എങ്കിൽ (ascending) std::sort(array_.begin(), array_.end());
      അല്ലെങ്കിൽ std::sort(array_.begin(), array_.end(), std::greater<string>());
}

ശൂന്യം vvp_darray_string::shuffle_elems(ശൂന്യം)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_string::duplicate(ശൂന്യം) സ്ഥിരം
{
      vvp_darray_string*that = പുതിയ vvp_darray_string(array_.size());

      വേണ്ടി (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      തിരികെ that;
}

vvp_queue::~vvp_queue()
{
}

ശൂന്യം vvp_queue::copy_elems(vvp_object_t, unsigned)
{
      cerr << "Sorry: copy_elems() not implemented for " << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_queue::set_word_max(unsigned, സ്ഥിരം vvp_vector4_t&, unsigned)
{
      cerr << "XXXX set_word_max(vvp_vector4_t) not implemented for " << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_queue::insert(unsigned, സ്ഥിരം vvp_vector4_t&, unsigned)
{
      cerr << "XXXX insert(vvp_vector4_t) not implemented for " << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_queue::push_back(സ്ഥിരം vvp_vector4_t&, unsigned)
{
      cerr << "XXXX push_back(vvp_vector4_t) not implemented for " << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_queue::push_front(സ്ഥിരം vvp_vector4_t&, unsigned)
{
      cerr << "XXXX push_front(vvp_vector4_t) not implemented for " << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_queue::set_word_max(unsigned, ഇരട്ട, unsigned)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_queue::insert(unsigned, ഇരട്ട, unsigned)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_queue::push_back(ഇരട്ട, unsigned)
{
      cerr << "XXXX push_back(double) not implemented for " << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_queue::push_front(ഇരട്ട, unsigned)
{
      cerr << "XXXX push_front(double) not implemented for " << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_queue::set_word_max(unsigned, സ്ഥിരം string&, unsigned)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_queue::insert(unsigned, സ്ഥിരം string&, unsigned)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_queue::push_back(സ്ഥിരം string&, unsigned)
{
      cerr << "XXXX push_back(string) not implemented for " << typeid(*ഇത്).name() << endl;
}

ശൂന്യം vvp_queue::push_front(സ്ഥിരം string&, unsigned)
{
      cerr << "XXXX push_front(string) not implemented for " << typeid(*ഇത്).name() << endl;
}

/*
 * Helper functions used while copying multiple elements into a queue.
 */
സ്ഥിതം ശൂന്യം print_copy_is_too_big(size_t src_size, unsigned max_size, സ്ഥിരം string&qtype)
{
      cerr << get_fileline()
           << "Warning: queue<" << qtype << "> is bounded to have at most "
           << max_size << " elements, source has " << src_size << " elements." << endl;
}

സ്ഥിതം ശൂന്യം print_copy_is_too_big(ഇരട്ട&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "real");
}

സ്ഥിതം ശൂന്യം print_copy_is_too_big(string&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "string");
}

സ്ഥിതം ശൂന്യം print_copy_is_too_big(vvp_vector4_t&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "vector");
}

template <typename ELEM, വർഗ്ഗം QTYPE, വർഗ്ഗം SRC_TYPE>
സ്ഥിതം ശൂന്യം copy_elements(QTYPE*queue, SRC_TYPE*src, unsigned max_size)
{
      size_t src_size = src->get_size();
      എങ്കിൽ ((max_size != 0) && (src_size > max_size)) {
	    ELEM tmp;
	    print_copy_is_too_big(tmp, src_size, max_size);
      }
      unsigned copy_size = ((src_size < max_size) ||
                            (max_size == 0)) ? src_size : max_size;
      എങ്കിൽ (copy_size < queue->get_size())
	    queue->erase_tail(copy_size);
      വേണ്ടി (unsigned idx=0; idx < copy_size; ++idx) {
	    ELEM value;
	    src->get_word(idx, value);
	    queue->set_word_max(idx, value, max_size);
      }
}

ശൂന്യം vvp_queue_real::copy_elems(vvp_object_t src, unsigned max_size)
{
      എങ്കിൽ (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<ഇരട്ട, vvp_queue_real, vvp_queue>(ഇത്, src_queue, max_size);
      അല്ലെങ്കിൽ എങ്കിൽ (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<ഇരട്ട, vvp_queue_real, vvp_darray>(ഇത്, src_darray, max_size);
      അല്ലെങ്കിൽ
	    cerr << get_fileline() << "Sorry: cannot copy object to real queue." << endl;
}

ശൂന്യം vvp_queue_real::set_word_max(unsigned adr, ഇരട്ട value, unsigned max_size)
{
      എങ്കിൽ (adr == queue.size())
	    എങ്കിൽ (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    അല്ലെങ്കിൽ
		  cerr << get_fileline()
		       << "Warning: assigning to queue<real>[" << adr << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      അല്ലെങ്കിൽ
	    set_word(adr, value);
}

ശൂന്യം vvp_queue_real::set_word(unsigned adr, ഇരട്ട value)
{
      എങ്കിൽ (adr < queue.size())
	    queue[adr] = value;
      അല്ലെങ്കിൽ
	    cerr << get_fileline()
	         << "Warning: assigning to queue<real>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
}

ശൂന്യം vvp_queue_real::get_word(unsigned adr, ഇരട്ട&value)
{
      എങ്കിൽ (adr >= queue.size())
	    value = 0.0;
      അല്ലെങ്കിൽ
	    value = queue[adr];
}

ശൂന്യം vvp_queue_real::insert(unsigned idx, ഇരട്ട value, unsigned max_size)
{
	// Inserting past the end of the queue
      എങ്കിൽ (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<real>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
	// Inserting at the end
      അല്ലെങ്കിൽ എങ്കിൽ (idx == queue.size())
	    എങ്കിൽ (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    അല്ലെങ്കിൽ
		  cerr << get_fileline()
		       << "Warning: inserting to queue<real>[" << idx << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      അല്ലെങ്കിൽ  {
	    എങ്കിൽ (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<real> ["
		       << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

ശൂന്യം vvp_queue_real::push_back(ഇരട്ട value, unsigned max_size)
{
      എങ്കിൽ (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      അല്ലെങ്കിൽ
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<real> ["
	         << max_size << "]." << endl;
}

ശൂന്യം vvp_queue_real::push_front(ഇരട്ട value, unsigned max_size)
{
      എങ്കിൽ (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<real> ["
	         << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

ശൂന്യം vvp_queue_real::erase(unsigned idx)
{
      പ്രതിജ്ഞാ(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

ശൂന്യം vvp_queue_real::erase_tail(unsigned idx)
{
      പ്രതിജ്ഞാ(queue.size() >= idx);
      എങ്കിൽ (queue.size() > idx)
	    queue.resize(idx);
}

ശൂന്യം vvp_queue_string::copy_elems(vvp_object_t src, unsigned max_size)
{
      എങ്കിൽ (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<string, vvp_queue_string, vvp_queue>(ഇത്, src_queue, max_size);
      അല്ലെങ്കിൽ എങ്കിൽ (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<string, vvp_queue_string, vvp_darray>(ഇത്, src_darray, max_size);
      അല്ലെങ്കിൽ
	    cerr << get_fileline() << "Sorry: cannot copy object to string queue." << endl;
}

ശൂന്യം vvp_queue_string::set_word_max(unsigned adr, സ്ഥിരം string&value, unsigned max_size)
{
      എങ്കിൽ (adr == queue.size())
	    എങ്കിൽ (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    അല്ലെങ്കിൽ
		  cerr << get_fileline()
		       << "Warning: assigning to queue<string>[" << adr << "] is"
		          " outside bound (" << max_size << "). \"" << value
		       << "\" was not added." << endl;
      അല്ലെങ്കിൽ
	    set_word(adr, value);
}

ശൂന്യം vvp_queue_string::set_word(unsigned adr, സ്ഥിരം string&value)
{
      എങ്കിൽ (adr < queue.size())
	    queue[adr] = value;
      അല്ലെങ്കിൽ
	    cerr << get_fileline()
	         << "Warning: assigning to queue<string>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << endl;
}

ശൂന്യം vvp_queue_string::get_word(unsigned adr, string&value)
{
      എങ്കിൽ (adr >= queue.size())
	    value = "";
      അല്ലെങ്കിൽ
	    value = queue[adr];
}

ശൂന്യം vvp_queue_string::insert(unsigned idx, സ്ഥിരം string&value, unsigned max_size)
{
	// Inserting past the end of the queue
      എങ്കിൽ (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<string>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << endl;
	// Inserting at the end
      അല്ലെങ്കിൽ എങ്കിൽ (idx == queue.size())
	    എങ്കിൽ (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    അല്ലെങ്കിൽ
		  cerr << get_fileline()
		       << "Warning: inserting to queue<string>[" << idx << "] is"
		          " outside bound (" << max_size << "). \"" << value
		       << "\" was not added." << endl;
      അല്ലെങ്കിൽ  {
	    എങ്കിൽ (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", \"" << value << "\") removed \""
		       << queue.back() << "\" from already full bounded queue<string> ["
		       << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

ശൂന്യം vvp_queue_string::push_back(സ്ഥിരം string&value, unsigned max_size)
{
      എങ്കിൽ (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      അല്ലെങ്കിൽ
	    cerr << get_fileline()
	         << "Warning: push_back(\"" << value
	         << "\") skipped for already full bounded queue<string> ["
	         << max_size << "]." << endl;
}

ശൂന്യം vvp_queue_string::push_front(സ്ഥിരം string&value, unsigned max_size)
{
      എങ്കിൽ (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(\"" << value << "\") removed \""
	         << queue.back() << "\" from already full bounded queue<string> ["
	         << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

ശൂന്യം vvp_queue_string::erase(unsigned idx)
{
      പ്രതിജ്ഞാ(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

ശൂന്യം vvp_queue_string::erase_tail(unsigned idx)
{
      പ്രതിജ്ഞാ(queue.size() >= idx);
      എങ്കിൽ (queue.size() > idx)
	    queue.resize(idx);
}

ശൂന്യം vvp_queue_real::reverse_elems(ശൂന്യം)
{
      std::reverse(queue.begin(), queue.end());
}

ശൂന്യം vvp_queue_string::reverse_elems(ശൂന്യം)
{
      std::reverse(queue.begin(), queue.end());
}

ശൂന്യം vvp_queue_real::sort_elems(തർക്കം ascending)
{
      എങ്കിൽ (ascending) std::sort(queue.begin(), queue.end());
      അല്ലെങ്കിൽ std::sort(queue.begin(), queue.end(), std::greater<ഇരട്ട>());
}

ശൂന്യം vvp_queue_real::shuffle_elems(ശൂന്യം)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

ശൂന്യം vvp_queue_string::sort_elems(തർക്കം ascending)
{
      എങ്കിൽ (ascending) std::sort(queue.begin(), queue.end());
      അല്ലെങ്കിൽ std::sort(queue.begin(), queue.end(), std::greater<string>());
}

ശൂന്യം vvp_queue_string::shuffle_elems(ശൂന്യം)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

ശൂന്യം vvp_queue_vec4::copy_elems(vvp_object_t src, unsigned max_size)
{
      എങ്കിൽ (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_queue>(ഇത്, src_queue, max_size);
      അല്ലെങ്കിൽ എങ്കിൽ (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_darray>(ഇത്, src_darray, max_size);
      അല്ലെങ്കിൽ
	    cerr << get_fileline() << "Sorry: cannot copy object to vector queue." << endl;
}

ശൂന്യം vvp_queue_vec4::set_word_max(unsigned adr, സ്ഥിരം vvp_vector4_t&value, unsigned max_size)
{
      എങ്കിൽ (adr == queue.size())
	    എങ്കിൽ (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    അല്ലെങ്കിൽ
		  cerr << get_fileline()
		       << "Warning: assigning to queue<vector>[" << adr << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      അല്ലെങ്കിൽ
	    set_word(adr, value);
}

ശൂന്യം vvp_queue_vec4::set_word(unsigned adr, സ്ഥിരം vvp_vector4_t&value)
{
      എങ്കിൽ (adr < queue.size())
	    queue[adr] = value;
      അല്ലെങ്കിൽ
	    cerr << get_fileline()
	         << "Warning: assigning to queue<vector>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
}

ശൂന്യം vvp_queue_vec4::get_word(unsigned adr, vvp_vector4_t&value)
{
      എങ്കിൽ (adr >= queue.size())
	    value = vvp_vector4_t(queue[0].size());
      അല്ലെങ്കിൽ
	    value = queue[adr];
}

ശൂന്യം vvp_queue_vec4::insert(unsigned idx, സ്ഥിരം vvp_vector4_t&value, unsigned max_size)
{
	// Inserting past the end of the queue
      എങ്കിൽ (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<vector[" << value.size()
	         << "]>[" << idx << "] is outside of size (" << queue.size()
	         << "). " << value << " was not added." << endl;
	// Inserting at the end
      അല്ലെങ്കിൽ എങ്കിൽ (idx == queue.size())
	    എങ്കിൽ (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    അല്ലെങ്കിൽ
		  cerr << get_fileline()
		       << "Warning: inserting to queue<vector[" << value.size()
		       << "]>[" << idx << "] is outside bound (" << max_size
		       << "). " << value << " was not added." << endl;
      അല്ലെങ്കിൽ  {
	    എങ്കിൽ (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<vector["
		       << value.size() << "]> [" << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

ശൂന്യം vvp_queue_vec4::push_back(സ്ഥിരം vvp_vector4_t&value, unsigned max_size)
{
      എങ്കിൽ (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      അല്ലെങ്കിൽ
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<vector["
	         << value.size() << "]> [" << max_size << "]." << endl;
}

ശൂന്യം vvp_queue_vec4::push_front(സ്ഥിരം vvp_vector4_t&value, unsigned max_size)
{
      എങ്കിൽ (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<vector["
	         << value.size() << "]> [" << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

ശൂന്യം vvp_queue_vec4::erase(unsigned idx)
{
      പ്രതിജ്ഞാ(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

ശൂന്യം vvp_queue_vec4::erase_tail(unsigned idx)
{
      പ്രതിജ്ഞാ(queue.size() >= idx);
      എങ്കിൽ (queue.size() > idx)
	    queue.resize(idx);
}

ശൂന്യം vvp_queue_vec4::reverse_elems(ശൂന്യം)
{
      std::reverse(queue.begin(), queue.end());
}

ശൂന്യം vvp_queue_vec4::sort_elems(തർക്കം ascending)
{
      എങ്കിൽ (ascending) {
	    std::sort(queue.begin(), queue.end(), vec4_lt_unsigned);
      } അല്ലെങ്കിൽ {
	    std::sort(queue.begin(), queue.end(),
		      [](സ്ഥിരം vvp_vector4_t&a, സ്ഥിരം vvp_vector4_t&b) {
			    തിരികെ vec4_lt_unsigned(b, a);
		      });
      }
}

ശൂന്യം vvp_queue_vec4::shuffle_elems(ശൂന്യം)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}
