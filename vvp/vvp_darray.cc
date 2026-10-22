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
      প্রত্যাগম rng;
}

স্থির তার্কিক vec4_lt_unsigned(ধ্রুব vvp_vector4_t&a, ধ্রুব vvp_vector4_t&b)
{
      যদি (a.eeq(b)) প্রত্যাগম অসত্য;
      vvp_bit4_t gt = compare_gtge(b, a, BIT4_1);
      যদি (gt == BIT4_X) প্রত্যাগম অসত্য;
      প্রত্যাগম gt == BIT4_1;
}

} // namespace

vvp_darray::~vvp_darray()
{
}

শূন্য vvp_darray::reverse_elems(শূন্য)
{
      cerr << "XXXX reverse_elems() not implemented for "
           << typeid(*এতত্).name() << endl;
}

শূন্য vvp_darray::sort_elems(তার্কিক /*ascending*/)
{
      cerr << "XXXX sort_elems() not implemented for "
           << typeid(*এতত্).name() << endl;
}

শূন্য vvp_darray::shuffle_elems(শূন্য)
{
      cerr << "XXXX shuffle_elems() not implemented for "
           << typeid(*এতত্).name() << endl;
}

শূন্য vvp_darray::set_word(unsigned, ধ্রুব vvp_vector4_t&)
{
      cerr << "XXXX set_word(vvp_vector4_t) not implemented for " << typeid(*এতত্).name() << endl;
}

শূন্য vvp_darray::set_word(unsigned, দ্বিগুণ)
{
      cerr << "XXXX set_word(double) not implemented for " << typeid(*এতত্).name() << endl;
}

শূন্য vvp_darray::set_word(unsigned, ধ্রুব string&)
{
      cerr << "XXXX set_word(string) not implemented for " << typeid(*এতত্).name() << endl;
}

শূন্য vvp_darray::set_word(unsigned, ধ্রুব vvp_object_t&)
{
      cerr << "XXXX set_word(vvp_object_t) not implemented for " << typeid(*এতত্).name() << endl;
}

শূন্য vvp_darray::get_word(unsigned, vvp_vector4_t&)
{
      cerr << "XXXX get_word(vvp_vector4_t) not implemented for " << typeid(*এতত্).name() << endl;
}

শূন্য vvp_darray::get_word(unsigned, দ্বিগুণ&)
{
      cerr << "XXXX get_word(double) not implemented for " << typeid(*এতত্).name() << endl;
}

শূন্য vvp_darray::get_word(unsigned, string&)
{
      cerr << "XXXX get_word(string) not implemented for " << typeid(*এতত্).name() << endl;
}

শূন্য vvp_darray::get_word(unsigned, vvp_object_t&)
{
      cerr << "XXXX get_word(vvp_object_t) not implemented for " << typeid(*এতত্).name() << endl;
}

vvp_vector4_t vvp_darray::get_bitstream(তার্কিক)
{
      cerr << "XXXX get_bitstream() not implemented for " << typeid(*এতত্).name() << endl;
      প্রত্যাগম vvp_vector4_t();
}

template <বর্গ TYPE> vvp_darray_atom<TYPE>::~vvp_darray_atom()
{
}

template <বর্গ TYPE> size_t vvp_darray_atom<TYPE>::get_size() ধ্রুব
{
      প্রত্যাগম array_.size();
}

template <বর্গ TYPE> শূন্য vvp_darray_atom<TYPE>::reverse_elems(শূন্য)
{
      size_t n = array_.size();
      প্রতি (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

template <বর্গ TYPE> শূন্য vvp_darray_atom<TYPE>::sort_elems(তার্কিক ascending)
{
      যদি (ascending) std::sort(array_.begin(), array_.end());
      অন্যথা std::sort(array_.begin(), array_.end(), std::greater<TYPE>());
}

template <বর্গ TYPE> শূন্য vvp_darray_atom<TYPE>::shuffle_elems(শূন্য)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

template <বর্গ TYPE> শূন্য vvp_darray_atom<TYPE>::set_word(unsigned adr, ধ্রুব vvp_vector4_t&value)
{
      যদি (adr >= array_.size())
	    প্রত্যাগম;
      TYPE tmp;
      vector4_to_মান(value, tmp, সত্য, অসত্য);
      array_[adr] = tmp;
}

template <বর্গ TYPE> শূন্য vvp_darray_atom<TYPE>::get_word(unsigned adr, vvp_vector4_t&value)
{
      যদি (adr >= array_.size()) {
	    value = vvp_vector4_t(8*পরিমাণ(TYPE), BIT4_X);
	    প্রত্যাগম;
      }

      TYPE word = array_[adr];
      vvp_vector4_t tmp (8*পরিমাণ(TYPE), BIT4_0);
      প্রতি (unsigned idx = 0 ; idx < tmp.size() ; idx += 1) {
	    যদি (word&1) tmp.set_bit(idx, BIT4_1);
	    word >>= 1;
      }
      value = tmp;
}

template <বর্গ TYPE> শূন্য vvp_darray_atom<TYPE>::shallow_copy(ধ্রুব vvp_object*obj)
{
      ধ্রুব vvp_darray_atom<TYPE>*that = dynamic_cast<ধ্রুব vvp_darray_atom<TYPE>*>(obj);
      প্রতিজ্ঞা(that);

      unsigned অঙ্ক_বস্তুসমূহ = min(array_.size(), that->array_.size());
      প্রতি (unsigned idx = 0 ; idx < অঙ্ক_বস্তুসমূহ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

template <বর্গ TYPE> vvp_object* vvp_darray_atom<TYPE>::duplicate(শূন্য) ধ্রুব
{
      vvp_darray_atom<TYPE>*that = নূতন vvp_darray_atom<TYPE>(array_.size());
      প্রতি (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      প্রত্যাগম that;
}

template <বর্গ TYPE> vvp_vector4_t vvp_darray_atom<TYPE>::get_bitstream(তার্কিক)
{
      ধ্রুব unsigned word_wid = পরিমাণ(TYPE) * 8;

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      যাবত্ (vdx > 0) {
            TYPE word = array_[adx++];
            vdx -= word_wid;
            প্রতি (unsigned bdx = 0; bdx < word_wid; bdx += 1) {
                  যদি (word & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word >>= 1;
            }
      }

      প্রত্যাগম vec;
}

template বর্গ vvp_darray_atom<uint8_t>;
template বর্গ vvp_darray_atom<uint16_t>;
template বর্গ vvp_darray_atom<uint32_t>;
template বর্গ vvp_darray_atom<uint64_t>;
template বর্গ vvp_darray_atom<int8_t>;
template বর্গ vvp_darray_atom<int16_t>;
template বর্গ vvp_darray_atom<int32_t>;
template বর্গ vvp_darray_atom<int64_t>;

vvp_darray_vec4::~vvp_darray_vec4()
{
}

size_t vvp_darray_vec4::get_size(শূন্য) ধ্রুব
{
      প্রত্যাগম array_.size();
}

শূন্য vvp_darray_vec4::set_word(unsigned adr, ধ্রুব vvp_vector4_t&value)
{
      যদি (adr >= array_.size()) প্রত্যাগম;
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
	    প্রত্যাগম;
      }
      value = array_[adr];
      প্রতিজ্ঞা(value.size() == word_wid_);
}

শূন্য vvp_darray_vec4::shallow_copy(ধ্রুব vvp_object*obj)
{
      ধ্রুব vvp_darray_vec4*that = dynamic_cast<ধ্রুব vvp_darray_vec4*>(obj);
      প্রতিজ্ঞা(that);

      unsigned অঙ্ক_বস্তুসমূহ = min(array_.size(), that->array_.size());
      প্রতি (unsigned idx = 0 ; idx < অঙ্ক_বস্তুসমূহ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_object* vvp_darray_vec4::duplicate(শূন্য) ধ্রুব
{
      vvp_darray_vec4*that = নূতন vvp_darray_vec4(array_.size(), word_wid_);

      প্রতি (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      প্রত্যাগম that;
}

vvp_vector4_t vvp_darray_vec4::get_bitstream(তার্কিক as_vec4)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      যাবত্ (vdx > 0) {
            vdx -= word_wid_;
            প্রতি (unsigned bdx = 0; bdx < word_wid_; bdx += 1) {
                  vvp_bit4_t bit = array_[adx].value(bdx);
                  যদি (as_vec4 || (bit == BIT4_1))
                        vec.set_bit(vdx+bdx, bit);
            }
            adx++;
      }

      প্রত্যাগম vec;
}

শূন্য vvp_darray_vec4::reverse_elems(শূন্য)
{
      size_t n = array_.size();
      প্রতি (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

শূন্য vvp_darray_vec4::sort_elems(তার্কিক ascending)
{
      যদি (ascending) {
	    std::sort(array_.begin(), array_.end(), vec4_lt_unsigned);
      } অন্যথা {
	    std::sort(array_.begin(), array_.end(),
		      [](ধ্রুব vvp_vector4_t&a, ধ্রুব vvp_vector4_t&b) {
			    প্রত্যাগম vec4_lt_unsigned(b, a);
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
      প্রতি (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

শূন্য vvp_darray_vec2::sort_elems(তার্কিক ascending)
{
      যদি (ascending) {
	    std::sort(array_.begin(), array_.end());
      } অন্যথা {
	    std::sort(array_.begin(), array_.end(),
		      [](ধ্রুব vvp_vector2_t&a, ধ্রুব vvp_vector2_t&b) {
			    প্রত্যাগম b < a;
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

size_t vvp_darray_vec2::get_size(শূন্য) ধ্রুব
{
      প্রত্যাগম array_.size();
}

শূন্য vvp_darray_vec2::set_word(unsigned adr, ধ্রুব vvp_vector4_t&value)
{
      যদি (adr >= array_.size()) প্রত্যাগম;
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
	    প্রত্যাগম;
      }
      প্রতিজ্ঞা(array_[adr].size() == word_wid_);
      value.resize(word_wid_);
      প্রতি (unsigned idx = 0; idx < word_wid_; idx += 1) {
	    value.set_bit(idx, array_[adr].value4(idx));
      }
}

শূন্য vvp_darray_vec2::shallow_copy(ধ্রুব vvp_object*obj)
{
      ধ্রুব vvp_darray_vec2*that = dynamic_cast<ধ্রুব vvp_darray_vec2*>(obj);
      প্রতিজ্ঞা(that);

      unsigned অঙ্ক_বস্তুসমূহ = min(array_.size(), that->array_.size());
      প্রতি (unsigned idx = 0 ; idx < অঙ্ক_বস্তুসমূহ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_vector4_t vvp_darray_vec2::get_bitstream(তার্কিক)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      যাবত্ (vdx > 0) {
            vdx -= word_wid_;
            প্রতি (unsigned bdx = 0; bdx < word_wid_; bdx += 1) {
                  যদি (array_[adx].value(bdx))
                        vec.set_bit(vdx+bdx, BIT4_1);
            }
            adx++;
      }

      প্রত্যাগম vec;
}

vvp_darray_object::~vvp_darray_object()
{
}

size_t vvp_darray_object::get_size() ধ্রুব
{
      প্রত্যাগম array_.size();
}

শূন্য vvp_darray_object::set_word(unsigned adr, ধ্রুব vvp_object_t&value)
{
      যদি (adr >= array_.size())
	    প্রত্যাগম;
      array_[adr] = value;
}

শূন্য vvp_darray_object::get_word(unsigned adr, vvp_object_t&value)
{
      যদি (adr >= array_.size()) {
	    value = vvp_object_t();
	    প্রত্যাগম;
      }

      value = array_[adr];
}

শূন্য vvp_darray_object::shallow_copy(ধ্রুব vvp_object*obj)
{
      ধ্রুব vvp_darray_object*that = dynamic_cast<ধ্রুব vvp_darray_object*>(obj);
      প্রতিজ্ঞা(that);

      unsigned অঙ্ক_বস্তুসমূহ = min(array_.size(), that->array_.size());
      প্রতি (unsigned idx = 0 ; idx < অঙ্ক_বস্তুসমূহ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

শূন্য vvp_darray_object::reverse_elems(শূন্য)
{
      size_t n = array_.size();
      প্রতি (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

শূন্য vvp_darray_object::sort_elems(তার্কিক ascending)
{
      যদি (ascending) {
	    std::sort(array_.begin(), array_.end(),
		      [](ধ্রুব vvp_object_t&a, ধ্রুব vvp_object_t&b) {
			    প্রত্যাগম a.sort_order_চাবি() < b.sort_order_চাবি();
		      });
      } অন্যথা {
	    std::sort(array_.begin(), array_.end(),
		      [](ধ্রুব vvp_object_t&a, ধ্রুব vvp_object_t&b) {
			    প্রত্যাগম a.sort_order_চাবি() > b.sort_order_চাবি();
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

size_t vvp_darray_real::get_size() ধ্রুব
{
      প্রত্যাগম array_.size();
}

শূন্য vvp_darray_real::set_word(unsigned adr, দ্বিগুণ value)
{
      যদি (adr >= array_.size())
	    প্রত্যাগম;
      array_[adr] = value;
}

শূন্য vvp_darray_real::get_word(unsigned adr, দ্বিগুণ&value)
{
      যদি (adr >= array_.size()) {
	    value = 0.0;
	    প্রত্যাগম;
      }

      value = array_[adr];
}

শূন্য vvp_darray_real::shallow_copy(ধ্রুব vvp_object*obj)
{
      ধ্রুব vvp_darray_real*that = dynamic_cast<ধ্রুব vvp_darray_real*>(obj);
      প্রতিজ্ঞা(that);

      unsigned অঙ্ক_বস্তুসমূহ = min(array_.size(), that->array_.size());
      প্রতি (unsigned idx = 0 ; idx < অঙ্ক_বস্তুসমূহ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

শূন্য vvp_darray_real::reverse_elems(শূন্য)
{
      size_t n = array_.size();
      প্রতি (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

শূন্য vvp_darray_real::sort_elems(তার্কিক ascending)
{
      যদি (ascending) std::sort(array_.begin(), array_.end());
      অন্যথা std::sort(array_.begin(), array_.end(), std::greater<দ্বিগুণ>());
}

শূন্য vvp_darray_real::shuffle_elems(শূন্য)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_real::duplicate(শূন্য) ধ্রুব
{
      vvp_darray_real*that = নূতন vvp_darray_real(array_.size());

      প্রতি (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      প্রত্যাগম that;
}

vvp_vector4_t vvp_darray_real::get_bitstream(তার্কিক)
{
      ধ্রুব unsigned word_wid = পরিমাণ(দ্বিগুণ) * 8;
      প্রতিজ্ঞা(word_wid == 64);

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      যাবত্ (vdx > 0) {
            সঙ্ঘ {
                দ্বিগুণ   value;
                uint64_t bits;
            } word;
            word.value = array_[adx++];
            vdx -= word_wid;
            প্রতি (unsigned bdx = 0; bdx < word_wid; bdx += 1) {
                  যদি (word.bits & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word.bits >>= 1;
            }
      }

      প্রত্যাগম vec;
}

vvp_darray_string::~vvp_darray_string()
{
}

size_t vvp_darray_string::get_size() ধ্রুব
{
      প্রত্যাগম array_.size();
}

শূন্য vvp_darray_string::set_word(unsigned adr, ধ্রুব string&value)
{
      যদি (adr >= array_.size())
	    প্রত্যাগম;
      array_[adr] = value;
}

শূন্য vvp_darray_string::get_word(unsigned adr, string&value)
{
      যদি (adr >= array_.size()) {
	    value = "";
	    প্রত্যাগম;
      }

      value = array_[adr];
}

শূন্য vvp_darray_string::shallow_copy(ধ্রুব vvp_object*obj)
{
      ধ্রুব vvp_darray_string*that = dynamic_cast<ধ্রুব vvp_darray_string*>(obj);
      প্রতিজ্ঞা(that);

      unsigned অঙ্ক_বস্তুসমূহ = min(array_.size(), that->array_.size());
      প্রতি (unsigned idx = 0 ; idx < অঙ্ক_বস্তুসমূহ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

শূন্য vvp_darray_string::reverse_elems(শূন্য)
{
      size_t n = array_.size();
      প্রতি (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

শূন্য vvp_darray_string::sort_elems(তার্কিক ascending)
{
      যদি (ascending) std::sort(array_.begin(), array_.end());
      অন্যথা std::sort(array_.begin(), array_.end(), std::greater<string>());
}

শূন্য vvp_darray_string::shuffle_elems(শূন্য)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_string::duplicate(শূন্য) ধ্রুব
{
      vvp_darray_string*that = নূতন vvp_darray_string(array_.size());

      প্রতি (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      প্রত্যাগম that;
}

vvp_queue::~vvp_queue()
{
}

শূন্য vvp_queue::copy_elems(vvp_object_t, unsigned)
{
      cerr << "Sorry: copy_elems() not implemented for " << typeid(*এতত্).name() << endl;
}

শূন্য vvp_queue::set_word_max(unsigned, ধ্রুব vvp_vector4_t&, unsigned)
{
      cerr << "XXXX set_word_max(vvp_vector4_t) not implemented for " << typeid(*এতত্).name() << endl;
}

শূন্য vvp_queue::insert(unsigned, ধ্রুব vvp_vector4_t&, unsigned)
{
      cerr << "XXXX insert(vvp_vector4_t) not implemented for " << typeid(*এতত্).name() << endl;
}

শূন্য vvp_queue::push_back(ধ্রুব vvp_vector4_t&, unsigned)
{
      cerr << "XXXX push_back(vvp_vector4_t) not implemented for " << typeid(*এতত্).name() << endl;
}

শূন্য vvp_queue::push_front(ধ্রুব vvp_vector4_t&, unsigned)
{
      cerr << "XXXX push_front(vvp_vector4_t) not implemented for " << typeid(*এতত্).name() << endl;
}

শূন্য vvp_queue::set_word_max(unsigned, দ্বিগুণ, unsigned)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*এতত্).name() << endl;
}

শূন্য vvp_queue::insert(unsigned, দ্বিগুণ, unsigned)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*এতত্).name() << endl;
}

শূন্য vvp_queue::push_back(দ্বিগুণ, unsigned)
{
      cerr << "XXXX push_back(double) not implemented for " << typeid(*এতত্).name() << endl;
}

শূন্য vvp_queue::push_front(দ্বিগুণ, unsigned)
{
      cerr << "XXXX push_front(double) not implemented for " << typeid(*এতত্).name() << endl;
}

শূন্য vvp_queue::set_word_max(unsigned, ধ্রুব string&, unsigned)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*এতত্).name() << endl;
}

শূন্য vvp_queue::insert(unsigned, ধ্রুব string&, unsigned)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*এতত্).name() << endl;
}

শূন্য vvp_queue::push_back(ধ্রুব string&, unsigned)
{
      cerr << "XXXX push_back(string) not implemented for " << typeid(*এতত্).name() << endl;
}

শূন্য vvp_queue::push_front(ধ্রুব string&, unsigned)
{
      cerr << "XXXX push_front(string) not implemented for " << typeid(*এতত্).name() << endl;
}

/*
 * Helper functions used while copying multiple elements into a queue.
 */
স্থির শূন্য print_copy_is_too_big(size_t src_size, unsigned max_size, ধ্রুব string&qtype)
{
      cerr << get_fileline()
           << "Warning: queue<" << qtype << "> is bounded to have at most "
           << max_size << " elements, source has " << src_size << " elements." << endl;
}

স্থির শূন্য print_copy_is_too_big(দ্বিগুণ&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "real");
}

স্থির শূন্য print_copy_is_too_big(string&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "string");
}

স্থির শূন্য print_copy_is_too_big(vvp_vector4_t&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "vector");
}

template <typename ELEM, বর্গ QTYPE, বর্গ SRC_TYPE>
স্থির শূন্য copy_elements(QTYPE*queue, SRC_TYPE*src, unsigned max_size)
{
      size_t src_size = src->get_size();
      যদি ((max_size != 0) && (src_size > max_size)) {
	    ELEM tmp;
	    print_copy_is_too_big(tmp, src_size, max_size);
      }
      unsigned copy_size = ((src_size < max_size) ||
                            (max_size == 0)) ? src_size : max_size;
      যদি (copy_size < queue->get_size())
	    queue->erase_tail(copy_size);
      প্রতি (unsigned idx=0; idx < copy_size; ++idx) {
	    ELEM value;
	    src->get_word(idx, value);
	    queue->set_word_max(idx, value, max_size);
      }
}

শূন্য vvp_queue_real::copy_elems(vvp_object_t src, unsigned max_size)
{
      যদি (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<দ্বিগুণ, vvp_queue_real, vvp_queue>(এতত্, src_queue, max_size);
      অন্যথা যদি (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<দ্বিগুণ, vvp_queue_real, vvp_darray>(এতত্, src_darray, max_size);
      অন্যথা
	    cerr << get_fileline() << "Sorry: cannot copy object to real queue." << endl;
}

শূন্য vvp_queue_real::set_word_max(unsigned adr, দ্বিগুণ value, unsigned max_size)
{
      যদি (adr == queue.size())
	    যদি (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    অন্যথা
		  cerr << get_fileline()
		       << "Warning: assigning to queue<real>[" << adr << "] is"
		          " outside bound (" << max_size << "). " << value
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

শূন্য vvp_queue_real::insert(unsigned idx, দ্বিগুণ value, unsigned max_size)
{
	// Inserting past the end of the queue
      যদি (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<real>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
	// Inserting at the end
      অন্যথা যদি (idx == queue.size())
	    যদি (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    অন্যথা
		  cerr << get_fileline()
		       << "Warning: inserting to queue<real>[" << idx << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      অন্যথা  {
	    যদি (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<real> ["
		       << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

শূন্য vvp_queue_real::push_back(দ্বিগুণ value, unsigned max_size)
{
      যদি (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      অন্যথা
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<real> ["
	         << max_size << "]." << endl;
}

শূন্য vvp_queue_real::push_front(দ্বিগুণ value, unsigned max_size)
{
      যদি (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<real> ["
	         << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

শূন্য vvp_queue_real::erase(unsigned idx)
{
      প্রতিজ্ঞা(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

শূন্য vvp_queue_real::erase_tail(unsigned idx)
{
      প্রতিজ্ঞা(queue.size() >= idx);
      যদি (queue.size() > idx)
	    queue.resize(idx);
}

শূন্য vvp_queue_string::copy_elems(vvp_object_t src, unsigned max_size)
{
      যদি (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<string, vvp_queue_string, vvp_queue>(এতত্, src_queue, max_size);
      অন্যথা যদি (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<string, vvp_queue_string, vvp_darray>(এতত্, src_darray, max_size);
      অন্যথা
	    cerr << get_fileline() << "Sorry: cannot copy object to string queue." << endl;
}

শূন্য vvp_queue_string::set_word_max(unsigned adr, ধ্রুব string&value, unsigned max_size)
{
      যদি (adr == queue.size())
	    যদি (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    অন্যথা
		  cerr << get_fileline()
		       << "Warning: assigning to queue<string>[" << adr << "] is"
		          " outside bound (" << max_size << "). \"" << value
		       << "\" was not added." << endl;
      অন্যথা
	    set_word(adr, value);
}

শূন্য vvp_queue_string::set_word(unsigned adr, ধ্রুব string&value)
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

শূন্য vvp_queue_string::insert(unsigned idx, ধ্রুব string&value, unsigned max_size)
{
	// Inserting past the end of the queue
      যদি (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<string>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << endl;
	// Inserting at the end
      অন্যথা যদি (idx == queue.size())
	    যদি (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    অন্যথা
		  cerr << get_fileline()
		       << "Warning: inserting to queue<string>[" << idx << "] is"
		          " outside bound (" << max_size << "). \"" << value
		       << "\" was not added." << endl;
      অন্যথা  {
	    যদি (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", \"" << value << "\") removed \""
		       << queue.back() << "\" from already full bounded queue<string> ["
		       << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

শূন্য vvp_queue_string::push_back(ধ্রুব string&value, unsigned max_size)
{
      যদি (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      অন্যথা
	    cerr << get_fileline()
	         << "Warning: push_back(\"" << value
	         << "\") skipped for already full bounded queue<string> ["
	         << max_size << "]." << endl;
}

শূন্য vvp_queue_string::push_front(ধ্রুব string&value, unsigned max_size)
{
      যদি (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(\"" << value << "\") removed \""
	         << queue.back() << "\" from already full bounded queue<string> ["
	         << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

শূন্য vvp_queue_string::erase(unsigned idx)
{
      প্রতিজ্ঞা(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

শূন্য vvp_queue_string::erase_tail(unsigned idx)
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

শূন্য vvp_queue_real::sort_elems(তার্কিক ascending)
{
      যদি (ascending) std::sort(queue.begin(), queue.end());
      অন্যথা std::sort(queue.begin(), queue.end(), std::greater<দ্বিগুণ>());
}

শূন্য vvp_queue_real::shuffle_elems(শূন্য)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

শূন্য vvp_queue_string::sort_elems(তার্কিক ascending)
{
      যদি (ascending) std::sort(queue.begin(), queue.end());
      অন্যথা std::sort(queue.begin(), queue.end(), std::greater<string>());
}

শূন্য vvp_queue_string::shuffle_elems(শূন্য)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

শূন্য vvp_queue_vec4::copy_elems(vvp_object_t src, unsigned max_size)
{
      যদি (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_queue>(এতত্, src_queue, max_size);
      অন্যথা যদি (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_darray>(এতত্, src_darray, max_size);
      অন্যথা
	    cerr << get_fileline() << "Sorry: cannot copy object to vector queue." << endl;
}

শূন্য vvp_queue_vec4::set_word_max(unsigned adr, ধ্রুব vvp_vector4_t&value, unsigned max_size)
{
      যদি (adr == queue.size())
	    যদি (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    অন্যথা
		  cerr << get_fileline()
		       << "Warning: assigning to queue<vector>[" << adr << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      অন্যথা
	    set_word(adr, value);
}

শূন্য vvp_queue_vec4::set_word(unsigned adr, ধ্রুব vvp_vector4_t&value)
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

শূন্য vvp_queue_vec4::insert(unsigned idx, ধ্রুব vvp_vector4_t&value, unsigned max_size)
{
	// Inserting past the end of the queue
      যদি (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<vector[" << value.size()
	         << "]>[" << idx << "] is outside of size (" << queue.size()
	         << "). " << value << " was not added." << endl;
	// Inserting at the end
      অন্যথা যদি (idx == queue.size())
	    যদি (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    অন্যথা
		  cerr << get_fileline()
		       << "Warning: inserting to queue<vector[" << value.size()
		       << "]>[" << idx << "] is outside bound (" << max_size
		       << "). " << value << " was not added." << endl;
      অন্যথা  {
	    যদি (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<vector["
		       << value.size() << "]> [" << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

শূন্য vvp_queue_vec4::push_back(ধ্রুব vvp_vector4_t&value, unsigned max_size)
{
      যদি (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      অন্যথা
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<vector["
	         << value.size() << "]> [" << max_size << "]." << endl;
}

শূন্য vvp_queue_vec4::push_front(ধ্রুব vvp_vector4_t&value, unsigned max_size)
{
      যদি (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<vector["
	         << value.size() << "]> [" << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

শূন্য vvp_queue_vec4::erase(unsigned idx)
{
      প্রতিজ্ঞা(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

শূন্য vvp_queue_vec4::erase_tail(unsigned idx)
{
      প্রতিজ্ঞা(queue.size() >= idx);
      যদি (queue.size() > idx)
	    queue.resize(idx);
}

শূন্য vvp_queue_vec4::reverse_elems(শূন্য)
{
      std::reverse(queue.begin(), queue.end());
}

শূন্য vvp_queue_vec4::sort_elems(তার্কিক ascending)
{
      যদি (ascending) {
	    std::sort(queue.begin(), queue.end(), vec4_lt_unsigned);
      } অন্যথা {
	    std::sort(queue.begin(), queue.end(),
		      [](ধ্রুব vvp_vector4_t&a, ধ্রুব vvp_vector4_t&b) {
			    প্রত্যাগম vec4_lt_unsigned(b, a);
		      });
      }
}

শূন্য vvp_queue_vec4::shuffle_elems(শূন্য)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}
