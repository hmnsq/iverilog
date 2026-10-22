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

using নামাকাশ std;

নামাকাশ {

স্থির std::mt19937& shuffle_rng(শূন্য)
{
      স্থির std::mt19937 rng(std::random_device{}());
      ফেরত rng;
}

স্থির যৌক্তিক vec4_lt_unsigned(ধ্রুবক vvp_vector4_t&a, ধ্রুবক vvp_vector4_t&b)
{
      যদি (a.eeq(b)) ফেরত মিথ্যা;
      vvp_bit4_t gt = compare_gtge(b, a, BIT4_1);
      যদি (gt == BIT4_X) ফেরত মিথ্যা;
      ফেরত gt == BIT4_1;
}

} // namespace

vvp_darray::~vvp_darray()
{
}

শূন্য vvp_darray::reverse_elems(শূন্য)
{
      cerr << "XXXX reverse_elems() not implemented for "
           << typeid(*এই).name() << endl;
}

শূন্য vvp_darray::sort_elems(যৌক্তিক /*ascending*/)
{
      cerr << "XXXX sort_elems() not implemented for "
           << typeid(*এই).name() << endl;
}

শূন্য vvp_darray::shuffle_elems(শূন্য)
{
      cerr << "XXXX shuffle_elems() not implemented for "
           << typeid(*এই).name() << endl;
}

শূন্য vvp_darray::set_word(unsigned, ধ্রুবক vvp_vector4_t&)
{
      cerr << "XXXX set_word(vvp_vector4_t) not implemented for " << typeid(*এই).name() << endl;
}

শূন্য vvp_darray::set_word(unsigned, দ্বিগুণ)
{
      cerr << "XXXX set_word(double) not implemented for " << typeid(*এই).name() << endl;
}

শূন্য vvp_darray::set_word(unsigned, ধ্রুবক string&)
{
      cerr << "XXXX set_word(string) not implemented for " << typeid(*এই).name() << endl;
}

শূন্য vvp_darray::set_word(unsigned, ধ্রুবক vvp_object_t&)
{
      cerr << "XXXX set_word(vvp_object_t) not implemented for " << typeid(*এই).name() << endl;
}

শূন্য vvp_darray::get_word(unsigned, vvp_vector4_t&)
{
      cerr << "XXXX get_word(vvp_vector4_t) not implemented for " << typeid(*এই).name() << endl;
}

শূন্য vvp_darray::get_word(unsigned, দ্বিগুণ&)
{
      cerr << "XXXX get_word(double) not implemented for " << typeid(*এই).name() << endl;
}

শূন্য vvp_darray::get_word(unsigned, string&)
{
      cerr << "XXXX get_word(string) not implemented for " << typeid(*এই).name() << endl;
}

শূন্য vvp_darray::get_word(unsigned, vvp_object_t&)
{
      cerr << "XXXX get_word(vvp_object_t) not implemented for " << typeid(*এই).name() << endl;
}

vvp_vector4_t vvp_darray::get_bitstream(যৌক্তিক)
{
      cerr << "XXXX get_bitstream() not implemented for " << typeid(*এই).name() << endl;
      ফেরত vvp_vector4_t();
}

template <শ্রেণী TYPE> vvp_darray_atom<TYPE>::~vvp_darray_atom()
{
}

template <শ্রেণী TYPE> size_t vvp_darray_atom<TYPE>::get_আকার() ধ্রুবক
{
      ফেরত array_.size();
}

template <শ্রেণী TYPE> শূন্য vvp_darray_atom<TYPE>::reverse_elems(শূন্য)
{
      size_t n = array_.size();
      জন্য (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

template <শ্রেণী TYPE> শূন্য vvp_darray_atom<TYPE>::sort_elems(যৌক্তিক ascending)
{
      যদি (ascending) std::sort(array_.begin(), array_.end());
      অন্যথা std::sort(array_.begin(), array_.end(), std::greater<TYPE>());
}

template <শ্রেণী TYPE> শূন্য vvp_darray_atom<TYPE>::shuffle_elems(শূন্য)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

template <শ্রেণী TYPE> শূন্য vvp_darray_atom<TYPE>::set_word(unsigned adr, ধ্রুবক vvp_vector4_t&value)
{
      যদি (adr >= array_.size())
	    ফেরত;
      TYPE tmp;
      vector4_to_মান(value, tmp, সত্য, মিথ্যা);
      array_[adr] = tmp;
}

template <শ্রেণী TYPE> শূন্য vvp_darray_atom<TYPE>::get_word(unsigned adr, vvp_vector4_t&value)
{
      যদি (adr >= array_.size()) {
	    value = vvp_vector4_t(8*আকার(TYPE), BIT4_X);
	    ফেরত;
      }

      TYPE word = array_[adr];
      vvp_vector4_t tmp (8*আকার(TYPE), BIT4_0);
      জন্য (unsigned idx = 0 ; idx < tmp.size() ; idx += 1) {
	    যদি (word&1) tmp.set_bit(idx, BIT4_1);
	    word >>= 1;
      }
      value = tmp;
}

template <শ্রেণী TYPE> শূন্য vvp_darray_atom<TYPE>::shallow_copy(ধ্রুবক vvp_object*obj)
{
      ধ্রুবক vvp_darray_atom<TYPE>*that = dynamic_cast<ধ্রুবক vvp_darray_atom<TYPE>*>(obj);
      প্রতিজ্ঞা(that);

      unsigned অঙ্ক_বস্তুসমূহ = min(array_.size(), that->array_.size());
      জন্য (unsigned idx = 0 ; idx < অঙ্ক_বস্তুসমূহ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

template <শ্রেণী TYPE> vvp_object* vvp_darray_atom<TYPE>::duplicate(শূন্য) ধ্রুবক
{
      vvp_darray_atom<TYPE>*that = নতুন vvp_darray_atom<TYPE>(array_.size());
      জন্য (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      ফেরত that;
}

template <শ্রেণী TYPE> vvp_vector4_t vvp_darray_atom<TYPE>::get_bitstream(যৌক্তিক)
{
      ধ্রুবক unsigned word_wid = আকার(TYPE) * 8;

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      যখন (vdx > 0) {
            TYPE word = array_[adx++];
            vdx -= word_wid;
            জন্য (unsigned bdx = 0; bdx < word_wid; bdx += 1) {
                  যদি (word & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word >>= 1;
            }
      }

      ফেরত vec;
}

template শ্রেণী vvp_darray_atom<uint8_t>;
template শ্রেণী vvp_darray_atom<uint16_t>;
template শ্রেণী vvp_darray_atom<uint32_t>;
template শ্রেণী vvp_darray_atom<uint64_t>;
template শ্রেণী vvp_darray_atom<int8_t>;
template শ্রেণী vvp_darray_atom<int16_t>;
template শ্রেণী vvp_darray_atom<int32_t>;
template শ্রেণী vvp_darray_atom<int64_t>;

vvp_darray_vec4::~vvp_darray_vec4()
{
}

size_t vvp_darray_vec4::get_আকার(শূন্য) ধ্রুবক
{
      ফেরত array_.size();
}

শূন্য vvp_darray_vec4::set_word(unsigned adr, ধ্রুবক vvp_vector4_t&value)
{
      যদি (adr >= array_.size()) ফেরত;
      প্রতিজ্ঞা(value.size() == word_wid_);
      array_[adr] = value;
}

শূন্য vvp_darray_vec4::get_word(unsigned adr, vvp_vector4_t&value)
{
	/*
	 * Return an undefined value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      যদি ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_X);
	    ফেরত;
      }
      value = array_[adr];
      প্রতিজ্ঞা(value.size() == word_wid_);
}

শূন্য vvp_darray_vec4::shallow_copy(ধ্রুবক vvp_object*obj)
{
      ধ্রুবক vvp_darray_vec4*that = dynamic_cast<ধ্রুবক vvp_darray_vec4*>(obj);
      প্রতিজ্ঞা(that);

      unsigned অঙ্ক_বস্তুসমূহ = min(array_.size(), that->array_.size());
      জন্য (unsigned idx = 0 ; idx < অঙ্ক_বস্তুসমূহ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_object* vvp_darray_vec4::duplicate(শূন্য) ধ্রুবক
{
      vvp_darray_vec4*that = নতুন vvp_darray_vec4(array_.size(), word_wid_);

      জন্য (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      ফেরত that;
}

vvp_vector4_t vvp_darray_vec4::get_bitstream(যৌক্তিক as_vec4)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      যখন (vdx > 0) {
            vdx -= word_wid_;
            জন্য (unsigned bdx = 0; bdx < word_wid_; bdx += 1) {
                  vvp_bit4_t bit = array_[adx].value(bdx);
                  যদি (as_vec4 || (bit == BIT4_1))
                        vec.set_bit(vdx+bdx, bit);
            }
            adx++;
      }

      ফেরত vec;
}

শূন্য vvp_darray_vec4::reverse_elems(শূন্য)
{
      size_t n = array_.size();
      জন্য (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

শূন্য vvp_darray_vec4::sort_elems(যৌক্তিক ascending)
{
      যদি (ascending) {
	    std::sort(array_.begin(), array_.end(), vec4_lt_unsigned);
      } অন্যথা {
	    std::sort(array_.begin(), array_.end(),
		      [](ধ্রুবক vvp_vector4_t&a, ধ্রুবক vvp_vector4_t&b) {
			    ফেরত vec4_lt_unsigned(b, a);
		      });
      }
}

শূন্য vvp_darray_vec4::shuffle_elems(শূন্য)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

শূন্য vvp_darray_vec2::reverse_elems(শূন্য)
{
      size_t n = array_.size();
      জন্য (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

শূন্য vvp_darray_vec2::sort_elems(যৌক্তিক ascending)
{
      যদি (ascending) {
	    std::sort(array_.begin(), array_.end());
      } অন্যথা {
	    std::sort(array_.begin(), array_.end(),
		      [](ধ্রুবক vvp_vector2_t&a, ধ্রুবক vvp_vector2_t&b) {
			    ফেরত b < a;
		      });
      }
}

শূন্য vvp_darray_vec2::shuffle_elems(শূন্য)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_darray_vec2::~vvp_darray_vec2()
{
}

size_t vvp_darray_vec2::get_আকার(শূন্য) ধ্রুবক
{
      ফেরত array_.size();
}

শূন্য vvp_darray_vec2::set_word(unsigned adr, ধ্রুবক vvp_vector4_t&value)
{
      যদি (adr >= array_.size()) ফেরত;
      প্রতিজ্ঞা(value.size() == word_wid_);
      array_[adr] = value;
}

শূন্য vvp_darray_vec2::get_word(unsigned adr, vvp_vector4_t&value)
{
	/*
	 * Return a zero value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      যদি ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_0);
	    ফেরত;
      }
      প্রতিজ্ঞা(array_[adr].size() == word_wid_);
      value.resize(word_wid_);
      জন্য (unsigned idx = 0; idx < word_wid_; idx += 1) {
	    value.set_bit(idx, array_[adr].value4(idx));
      }
}

শূন্য vvp_darray_vec2::shallow_copy(ধ্রুবক vvp_object*obj)
{
      ধ্রুবক vvp_darray_vec2*that = dynamic_cast<ধ্রুবক vvp_darray_vec2*>(obj);
      প্রতিজ্ঞা(that);

      unsigned অঙ্ক_বস্তুসমূহ = min(array_.size(), that->array_.size());
      জন্য (unsigned idx = 0 ; idx < অঙ্ক_বস্তুসমূহ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_vector4_t vvp_darray_vec2::get_bitstream(যৌক্তিক)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      যখন (vdx > 0) {
            vdx -= word_wid_;
            জন্য (unsigned bdx = 0; bdx < word_wid_; bdx += 1) {
                  যদি (array_[adx].value(bdx))
                        vec.set_bit(vdx+bdx, BIT4_1);
            }
            adx++;
      }

      ফেরত vec;
}

vvp_darray_object::~vvp_darray_object()
{
}

size_t vvp_darray_object::get_আকার() ধ্রুবক
{
      ফেরত array_.size();
}

শূন্য vvp_darray_object::set_word(unsigned adr, ধ্রুবক vvp_object_t&value)
{
      যদি (adr >= array_.size())
	    ফেরত;
      array_[adr] = value;
}

শূন্য vvp_darray_object::get_word(unsigned adr, vvp_object_t&value)
{
      যদি (adr >= array_.size()) {
	    value = vvp_object_t();
	    ফেরত;
      }

      value = array_[adr];
}

শূন্য vvp_darray_object::shallow_copy(ধ্রুবক vvp_object*obj)
{
      ধ্রুবক vvp_darray_object*that = dynamic_cast<ধ্রুবক vvp_darray_object*>(obj);
      প্রতিজ্ঞা(that);

      unsigned অঙ্ক_বস্তুসমূহ = min(array_.size(), that->array_.size());
      জন্য (unsigned idx = 0 ; idx < অঙ্ক_বস্তুসমূহ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

শূন্য vvp_darray_object::reverse_elems(শূন্য)
{
      size_t n = array_.size();
      জন্য (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

শূন্য vvp_darray_object::sort_elems(যৌক্তিক ascending)
{
      যদি (ascending) {
	    std::sort(array_.begin(), array_.end(),
		      [](ধ্রুবক vvp_object_t&a, ধ্রুবক vvp_object_t&b) {
			    ফেরত a.sort_order_চাবি() < b.sort_order_চাবি();
		      });
      } অন্যথা {
	    std::sort(array_.begin(), array_.end(),
		      [](ধ্রুবক vvp_object_t&a, ধ্রুবক vvp_object_t&b) {
			    ফেরত a.sort_order_চাবি() > b.sort_order_চাবি();
		      });
      }
}

শূন্য vvp_darray_object::shuffle_elems(শূন্য)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_darray_real::~vvp_darray_real()
{
}

size_t vvp_darray_real::get_আকার() ধ্রুবক
{
      ফেরত array_.size();
}

শূন্য vvp_darray_real::set_word(unsigned adr, দ্বিগুণ value)
{
      যদি (adr >= array_.size())
	    ফেরত;
      array_[adr] = value;
}

শূন্য vvp_darray_real::get_word(unsigned adr, দ্বিগুণ&value)
{
      যদি (adr >= array_.size()) {
	    value = 0.0;
	    ফেরত;
      }

      value = array_[adr];
}

শূন্য vvp_darray_real::shallow_copy(ধ্রুবক vvp_object*obj)
{
      ধ্রুবক vvp_darray_real*that = dynamic_cast<ধ্রুবক vvp_darray_real*>(obj);
      প্রতিজ্ঞা(that);

      unsigned অঙ্ক_বস্তুসমূহ = min(array_.size(), that->array_.size());
      জন্য (unsigned idx = 0 ; idx < অঙ্ক_বস্তুসমূহ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

শূন্য vvp_darray_real::reverse_elems(শূন্য)
{
      size_t n = array_.size();
      জন্য (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

শূন্য vvp_darray_real::sort_elems(যৌক্তিক ascending)
{
      যদি (ascending) std::sort(array_.begin(), array_.end());
      অন্যথা std::sort(array_.begin(), array_.end(), std::greater<দ্বিগুণ>());
}

শূন্য vvp_darray_real::shuffle_elems(শূন্য)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_real::duplicate(শূন্য) ধ্রুবক
{
      vvp_darray_real*that = নতুন vvp_darray_real(array_.size());

      জন্য (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      ফেরত that;
}

vvp_vector4_t vvp_darray_real::get_bitstream(যৌক্তিক)
{
      ধ্রুবক unsigned word_wid = আকার(দ্বিগুণ) * 8;
      প্রতিজ্ঞা(word_wid == 64);

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      যখন (vdx > 0) {
            সংঘ {
                দ্বিগুণ   value;
                uint64_t bits;
            } word;
            word.value = array_[adx++];
            vdx -= word_wid;
            জন্য (unsigned bdx = 0; bdx < word_wid; bdx += 1) {
                  যদি (word.bits & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word.bits >>= 1;
            }
      }

      ফেরত vec;
}

vvp_darray_string::~vvp_darray_string()
{
}

size_t vvp_darray_string::get_আকার() ধ্রুবক
{
      ফেরত array_.size();
}

শূন্য vvp_darray_string::set_word(unsigned adr, ধ্রুবক string&value)
{
      যদি (adr >= array_.size())
	    ফেরত;
      array_[adr] = value;
}

শূন্য vvp_darray_string::get_word(unsigned adr, string&value)
{
      যদি (adr >= array_.size()) {
	    value = "";
	    ফেরত;
      }

      value = array_[adr];
}

শূন্য vvp_darray_string::shallow_copy(ধ্রুবক vvp_object*obj)
{
      ধ্রুবক vvp_darray_string*that = dynamic_cast<ধ্রুবক vvp_darray_string*>(obj);
      প্রতিজ্ঞা(that);

      unsigned অঙ্ক_বস্তুসমূহ = min(array_.size(), that->array_.size());
      জন্য (unsigned idx = 0 ; idx < অঙ্ক_বস্তুসমূহ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

শূন্য vvp_darray_string::reverse_elems(শূন্য)
{
      size_t n = array_.size();
      জন্য (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

শূন্য vvp_darray_string::sort_elems(যৌক্তিক ascending)
{
      যদি (ascending) std::sort(array_.begin(), array_.end());
      অন্যথা std::sort(array_.begin(), array_.end(), std::greater<string>());
}

শূন্য vvp_darray_string::shuffle_elems(শূন্য)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_string::duplicate(শূন্য) ধ্রুবক
{
      vvp_darray_string*that = নতুন vvp_darray_string(array_.size());

      জন্য (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      ফেরত that;
}

vvp_queue::~vvp_queue()
{
}

শূন্য vvp_queue::copy_elems(vvp_object_t, unsigned)
{
      cerr << "Sorry: copy_elems() not implemented for " << typeid(*এই).name() << endl;
}

শূন্য vvp_queue::set_word_max(unsigned, ধ্রুবক vvp_vector4_t&, unsigned)
{
      cerr << "XXXX set_word_max(vvp_vector4_t) not implemented for " << typeid(*এই).name() << endl;
}

শূন্য vvp_queue::insert(unsigned, ধ্রুবক vvp_vector4_t&, unsigned)
{
      cerr << "XXXX insert(vvp_vector4_t) not implemented for " << typeid(*এই).name() << endl;
}

শূন্য vvp_queue::push_back(ধ্রুবক vvp_vector4_t&, unsigned)
{
      cerr << "XXXX push_back(vvp_vector4_t) not implemented for " << typeid(*এই).name() << endl;
}

শূন্য vvp_queue::push_front(ধ্রুবক vvp_vector4_t&, unsigned)
{
      cerr << "XXXX push_front(vvp_vector4_t) not implemented for " << typeid(*এই).name() << endl;
}

শূন্য vvp_queue::set_word_max(unsigned, দ্বিগুণ, unsigned)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*এই).name() << endl;
}

শূন্য vvp_queue::insert(unsigned, দ্বিগুণ, unsigned)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*এই).name() << endl;
}

শূন্য vvp_queue::push_back(দ্বিগুণ, unsigned)
{
      cerr << "XXXX push_back(double) not implemented for " << typeid(*এই).name() << endl;
}

শূন্য vvp_queue::push_front(দ্বিগুণ, unsigned)
{
      cerr << "XXXX push_front(double) not implemented for " << typeid(*এই).name() << endl;
}

শূন্য vvp_queue::set_word_max(unsigned, ধ্রুবক string&, unsigned)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*এই).name() << endl;
}

শূন্য vvp_queue::insert(unsigned, ধ্রুবক string&, unsigned)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*এই).name() << endl;
}

শূন্য vvp_queue::push_back(ধ্রুবক string&, unsigned)
{
      cerr << "XXXX push_back(string) not implemented for " << typeid(*এই).name() << endl;
}

শূন্য vvp_queue::push_front(ধ্রুবক string&, unsigned)
{
      cerr << "XXXX push_front(string) not implemented for " << typeid(*এই).name() << endl;
}

/*
 * Helper functions used while copying multiple elements into a queue.
 */
স্থির শূন্য print_copy_is_too_big(size_t src_আকার, unsigned max_আকার, ধ্রুবক string&qtype)
{
      cerr << get_fileline()
           << "Warning: queue<" << qtype << "> is bounded to have at most "
           << max_আকার << " elements, source has " << src_আকার << " elements." << endl;
}

স্থির শূন্য print_copy_is_too_big(দ্বিগুণ&, size_t src_আকার, unsigned max_আকার)
{
      print_copy_is_too_big(src_আকার, max_আকার, "real");
}

স্থির শূন্য print_copy_is_too_big(string&, size_t src_আকার, unsigned max_আকার)
{
      print_copy_is_too_big(src_আকার, max_আকার, "string");
}

স্থির শূন্য print_copy_is_too_big(vvp_vector4_t&, size_t src_আকার, unsigned max_আকার)
{
      print_copy_is_too_big(src_আকার, max_আকার, "vector");
}

template <typename ELEM, শ্রেণী QTYPE, শ্রেণী SRC_TYPE>
স্থির শূন্য copy_elements(QTYPE*queue, SRC_TYPE*src, unsigned max_আকার)
{
      size_t src_আকার = src->get_আকার();
      যদি ((max_আকার != 0) && (src_আকার > max_আকার)) {
	    ELEM tmp;
	    print_copy_is_too_big(tmp, src_আকার, max_আকার);
      }
      unsigned copy_আকার = ((src_আকার < max_আকার) ||
                            (max_আকার == 0)) ? src_আকার : max_আকার;
      যদি (copy_আকার < queue->get_আকার())
	    queue->erase_লেজ(copy_আকার);
      জন্য (unsigned idx=0; idx < copy_আকার; ++idx) {
	    ELEM value;
	    src->get_word(idx, value);
	    queue->set_word_max(idx, value, max_আকার);
      }
}

শূন্য vvp_queue_real::copy_elems(vvp_object_t src, unsigned max_আকার)
{
      যদি (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<দ্বিগুণ, vvp_queue_real, vvp_queue>(এই, src_queue, max_আকার);
      অন্যথা যদি (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<দ্বিগুণ, vvp_queue_real, vvp_darray>(এই, src_darray, max_আকার);
      অন্যথা
	    cerr << get_fileline() << "Sorry: cannot copy object to real queue." << endl;
}

শূন্য vvp_queue_real::set_word_max(unsigned adr, দ্বিগুণ value, unsigned max_আকার)
{
      যদি (adr == queue.size())
	    যদি (!max_আকার || (queue.size() < max_আকার))
		  queue.push_back(value);
	    অন্যথা
		  cerr << get_fileline()
		       << "Warning: assigning to queue<real>[" << adr << "] is"
		          " outside bound (" << max_আকার << "). " << value
		       << " was not added." << endl;
      অন্যথা
	    set_word(adr, value);
}

শূন্য vvp_queue_real::set_word(unsigned adr, দ্বিগুণ value)
{
      যদি (adr < queue.size())
	    queue[adr] = value;
      অন্যথা
	    cerr << get_fileline()
	         << "Warning: assigning to queue<real>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
}

শূন্য vvp_queue_real::get_word(unsigned adr, দ্বিগুণ&value)
{
      যদি (adr >= queue.size())
	    value = 0.0;
      অন্যথা
	    value = queue[adr];
}

শূন্য vvp_queue_real::insert(unsigned idx, দ্বিগুণ value, unsigned max_আকার)
{
	// Inserting past the end of the queue
      যদি (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<real>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
	// Inserting at the end
      অন্যথা যদি (idx == queue.size())
	    যদি (!max_আকার || (queue.size() < max_আকার))
		  queue.push_back(value);
	    অন্যথা
		  cerr << get_fileline()
		       << "Warning: inserting to queue<real>[" << idx << "] is"
		          " outside bound (" << max_আকার << "). " << value
		       << " was not added." << endl;
      অন্যথা  {
	    যদি (max_আকার && (queue.size() == max_আকার)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<real> ["
		       << max_আকার << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

শূন্য vvp_queue_real::push_back(দ্বিগুণ value, unsigned max_আকার)
{
      যদি (!max_আকার || (queue.size() < max_আকার))
	    queue.push_back(value);
      অন্যথা
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<real> ["
	         << max_আকার << "]." << endl;
}

শূন্য vvp_queue_real::push_front(দ্বিগুণ value, unsigned max_আকার)
{
      যদি (max_আকার && (queue.size() == max_আকার)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<real> ["
	         << max_আকার << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

শূন্য vvp_queue_real::erase(unsigned idx)
{
      প্রতিজ্ঞা(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

শূন্য vvp_queue_real::erase_লেজ(unsigned idx)
{
      প্রতিজ্ঞা(queue.size() >= idx);
      যদি (queue.size() > idx)
	    queue.resize(idx);
}

শূন্য vvp_queue_string::copy_elems(vvp_object_t src, unsigned max_আকার)
{
      যদি (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<string, vvp_queue_string, vvp_queue>(এই, src_queue, max_আকার);
      অন্যথা যদি (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<string, vvp_queue_string, vvp_darray>(এই, src_darray, max_আকার);
      অন্যথা
	    cerr << get_fileline() << "Sorry: cannot copy object to string queue." << endl;
}

শূন্য vvp_queue_string::set_word_max(unsigned adr, ধ্রুবক string&value, unsigned max_আকার)
{
      যদি (adr == queue.size())
	    যদি (!max_আকার || (queue.size() < max_আকার))
		  queue.push_back(value);
	    অন্যথা
		  cerr << get_fileline()
		       << "Warning: assigning to queue<string>[" << adr << "] is"
		          " outside bound (" << max_আকার << "). \"" << value
		       << "\" was not added." << endl;
      অন্যথা
	    set_word(adr, value);
}

শূন্য vvp_queue_string::set_word(unsigned adr, ধ্রুবক string&value)
{
      যদি (adr < queue.size())
	    queue[adr] = value;
      অন্যথা
	    cerr << get_fileline()
	         << "Warning: assigning to queue<string>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << endl;
}

শূন্য vvp_queue_string::get_word(unsigned adr, string&value)
{
      যদি (adr >= queue.size())
	    value = "";
      অন্যথা
	    value = queue[adr];
}

শূন্য vvp_queue_string::insert(unsigned idx, ধ্রুবক string&value, unsigned max_আকার)
{
	// Inserting past the end of the queue
      যদি (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<string>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << endl;
	// Inserting at the end
      অন্যথা যদি (idx == queue.size())
	    যদি (!max_আকার || (queue.size() < max_আকার))
		  queue.push_back(value);
	    অন্যথা
		  cerr << get_fileline()
		       << "Warning: inserting to queue<string>[" << idx << "] is"
		          " outside bound (" << max_আকার << "). \"" << value
		       << "\" was not added." << endl;
      অন্যথা  {
	    যদি (max_আকার && (queue.size() == max_আকার)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", \"" << value << "\") removed \""
		       << queue.back() << "\" from already full bounded queue<string> ["
		       << max_আকার << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

শূন্য vvp_queue_string::push_back(ধ্রুবক string&value, unsigned max_আকার)
{
      যদি (!max_আকার || (queue.size() < max_আকার))
	    queue.push_back(value);
      অন্যথা
	    cerr << get_fileline()
	         << "Warning: push_back(\"" << value
	         << "\") skipped for already full bounded queue<string> ["
	         << max_আকার << "]." << endl;
}

শূন্য vvp_queue_string::push_front(ধ্রুবক string&value, unsigned max_আকার)
{
      যদি (max_আকার && (queue.size() == max_আকার)) {
	    cerr << get_fileline()
	         << "Warning: push_front(\"" << value << "\") removed \""
	         << queue.back() << "\" from already full bounded queue<string> ["
	         << max_আকার << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

শূন্য vvp_queue_string::erase(unsigned idx)
{
      প্রতিজ্ঞা(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

শূন্য vvp_queue_string::erase_লেজ(unsigned idx)
{
      প্রতিজ্ঞা(queue.size() >= idx);
      যদি (queue.size() > idx)
	    queue.resize(idx);
}

শূন্য vvp_queue_real::reverse_elems(শূন্য)
{
      std::reverse(queue.begin(), queue.end());
}

শূন্য vvp_queue_string::reverse_elems(শূন্য)
{
      std::reverse(queue.begin(), queue.end());
}

শূন্য vvp_queue_real::sort_elems(যৌক্তিক ascending)
{
      যদি (ascending) std::sort(queue.begin(), queue.end());
      অন্যথা std::sort(queue.begin(), queue.end(), std::greater<দ্বিগুণ>());
}

শূন্য vvp_queue_real::shuffle_elems(শূন্য)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

শূন্য vvp_queue_string::sort_elems(যৌক্তিক ascending)
{
      যদি (ascending) std::sort(queue.begin(), queue.end());
      অন্যথা std::sort(queue.begin(), queue.end(), std::greater<string>());
}

শূন্য vvp_queue_string::shuffle_elems(শূন্য)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

শূন্য vvp_queue_vec4::copy_elems(vvp_object_t src, unsigned max_আকার)
{
      যদি (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_queue>(এই, src_queue, max_আকার);
      অন্যথা যদি (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_darray>(এই, src_darray, max_আকার);
      অন্যথা
	    cerr << get_fileline() << "Sorry: cannot copy object to vector queue." << endl;
}

শূন্য vvp_queue_vec4::set_word_max(unsigned adr, ধ্রুবক vvp_vector4_t&value, unsigned max_আকার)
{
      যদি (adr == queue.size())
	    যদি (!max_আকার || (queue.size() < max_আকার))
		  queue.push_back(value);
	    অন্যথা
		  cerr << get_fileline()
		       << "Warning: assigning to queue<vector>[" << adr << "] is"
		          " outside bound (" << max_আকার << "). " << value
		       << " was not added." << endl;
      অন্যথা
	    set_word(adr, value);
}

শূন্য vvp_queue_vec4::set_word(unsigned adr, ধ্রুবক vvp_vector4_t&value)
{
      যদি (adr < queue.size())
	    queue[adr] = value;
      অন্যথা
	    cerr << get_fileline()
	         << "Warning: assigning to queue<vector>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
}

শূন্য vvp_queue_vec4::get_word(unsigned adr, vvp_vector4_t&value)
{
      যদি (adr >= queue.size())
	    value = vvp_vector4_t(queue[0].size());
      অন্যথা
	    value = queue[adr];
}

শূন্য vvp_queue_vec4::insert(unsigned idx, ধ্রুবক vvp_vector4_t&value, unsigned max_আকার)
{
	// Inserting past the end of the queue
      যদি (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<vector[" << value.size()
	         << "]>[" << idx << "] is outside of size (" << queue.size()
	         << "). " << value << " was not added." << endl;
	// Inserting at the end
      অন্যথা যদি (idx == queue.size())
	    যদি (!max_আকার || (queue.size() < max_আকার))
		  queue.push_back(value);
	    অন্যথা
		  cerr << get_fileline()
		       << "Warning: inserting to queue<vector[" << value.size()
		       << "]>[" << idx << "] is outside bound (" << max_আকার
		       << "). " << value << " was not added." << endl;
      অন্যথা  {
	    যদি (max_আকার && (queue.size() == max_আকার)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<vector["
		       << value.size() << "]> [" << max_আকার << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

শূন্য vvp_queue_vec4::push_back(ধ্রুবক vvp_vector4_t&value, unsigned max_আকার)
{
      যদি (!max_আকার || (queue.size() < max_আকার))
	    queue.push_back(value);
      অন্যথা
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<vector["
	         << value.size() << "]> [" << max_আকার << "]." << endl;
}

শূন্য vvp_queue_vec4::push_front(ধ্রুবক vvp_vector4_t&value, unsigned max_আকার)
{
      যদি (max_আকার && (queue.size() == max_আকার)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<vector["
	         << value.size() << "]> [" << max_আকার << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

শূন্য vvp_queue_vec4::erase(unsigned idx)
{
      প্রতিজ্ঞা(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

শূন্য vvp_queue_vec4::erase_লেজ(unsigned idx)
{
      প্রতিজ্ঞা(queue.size() >= idx);
      যদি (queue.size() > idx)
	    queue.resize(idx);
}

শূন্য vvp_queue_vec4::reverse_elems(শূন্য)
{
      std::reverse(queue.begin(), queue.end());
}

শূন্য vvp_queue_vec4::sort_elems(যৌক্তিক ascending)
{
      যদি (ascending) {
	    std::sort(queue.begin(), queue.end(), vec4_lt_unsigned);
      } অন্যথা {
	    std::sort(queue.begin(), queue.end(),
		      [](ধ্রুবক vvp_vector4_t&a, ধ্রুবক vvp_vector4_t&b) {
			    ফেরত vec4_lt_unsigned(b, a);
		      });
      }
}

শূন্য vvp_queue_vec4::shuffle_elems(শূন্য)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}
