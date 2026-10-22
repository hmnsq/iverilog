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

using فضاینام std;

فضاینام {

ایستا std::mt19937& shuffle_rng(تهی)
{
      ایستا std::mt19937 rng(std::random_device{}());
      بازگشت rng;
}

ایستا منطقی vec4_lt_unsigned(ثابت vvp_vector4_t&a, ثابت vvp_vector4_t&b)
{
      اگر (a.eeq(b)) بازگشت نادرست;
      vvp_bit4_t gt = compare_gtge(b, a, BIT4_1);
      اگر (gt == BIT4_X) بازگشت نادرست;
      بازگشت gt == BIT4_1;
}

} // namespace

vvp_darray::~vvp_darray()
{
}

تهی vvp_darray::reverse_elems(تهی)
{
      cerr << "XXXX reverse_elems() not implemented for "
           << typeid(*این).name() << endl;
}

تهی vvp_darray::sort_elems(منطقی /*ascending*/)
{
      cerr << "XXXX sort_elems() not implemented for "
           << typeid(*این).name() << endl;
}

تهی vvp_darray::shuffle_elems(تهی)
{
      cerr << "XXXX shuffle_elems() not implemented for "
           << typeid(*این).name() << endl;
}

تهی vvp_darray::set_word(unsigned, ثابت vvp_vector4_t&)
{
      cerr << "XXXX set_word(vvp_vector4_t) not implemented for " << typeid(*این).name() << endl;
}

تهی vvp_darray::set_word(unsigned, دوگانه)
{
      cerr << "XXXX set_word(double) not implemented for " << typeid(*این).name() << endl;
}

تهی vvp_darray::set_word(unsigned, ثابت string&)
{
      cerr << "XXXX set_word(string) not implemented for " << typeid(*این).name() << endl;
}

تهی vvp_darray::set_word(unsigned, ثابت vvp_object_t&)
{
      cerr << "XXXX set_word(vvp_object_t) not implemented for " << typeid(*این).name() << endl;
}

تهی vvp_darray::get_word(unsigned, vvp_vector4_t&)
{
      cerr << "XXXX get_word(vvp_vector4_t) not implemented for " << typeid(*این).name() << endl;
}

تهی vvp_darray::get_word(unsigned, دوگانه&)
{
      cerr << "XXXX get_word(double) not implemented for " << typeid(*این).name() << endl;
}

تهی vvp_darray::get_word(unsigned, string&)
{
      cerr << "XXXX get_word(string) not implemented for " << typeid(*این).name() << endl;
}

تهی vvp_darray::get_word(unsigned, vvp_object_t&)
{
      cerr << "XXXX get_word(vvp_object_t) not implemented for " << typeid(*این).name() << endl;
}

vvp_vector4_t vvp_darray::get_bitstream(منطقی)
{
      cerr << "XXXX get_bitstream() not implemented for " << typeid(*این).name() << endl;
      بازگشت vvp_vector4_t();
}

template <رده TYPE> vvp_darray_atom<TYPE>::~vvp_darray_atom()
{
}

template <رده TYPE> size_t vvp_darray_atom<TYPE>::get_size() ثابت
{
      بازگشت array_.size();
}

template <رده TYPE> تهی vvp_darray_atom<TYPE>::reverse_elems(تهی)
{
      size_t n = array_.size();
      برای (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

template <رده TYPE> تهی vvp_darray_atom<TYPE>::sort_elems(منطقی ascending)
{
      اگر (ascending) std::sort(array_.begin(), array_.end());
      وگرنه std::sort(array_.begin(), array_.end(), std::greater<TYPE>());
}

template <رده TYPE> تهی vvp_darray_atom<TYPE>::shuffle_elems(تهی)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

template <رده TYPE> تهی vvp_darray_atom<TYPE>::set_word(unsigned adr, ثابت vvp_vector4_t&value)
{
      اگر (adr >= array_.size())
	    بازگشت;
      TYPE tmp;
      vector4_to_مقدار(value, tmp, درست, نادرست);
      array_[adr] = tmp;
}

template <رده TYPE> تهی vvp_darray_atom<TYPE>::get_word(unsigned adr, vvp_vector4_t&value)
{
      اگر (adr >= array_.size()) {
	    value = vvp_vector4_t(8*اندازه(TYPE), BIT4_X);
	    بازگشت;
      }

      TYPE word = array_[adr];
      vvp_vector4_t tmp (8*اندازه(TYPE), BIT4_0);
      برای (unsigned idx = 0 ; idx < tmp.size() ; idx += 1) {
	    اگر (word&1) tmp.set_bit(idx, BIT4_1);
	    word >>= 1;
      }
      value = tmp;
}

template <رده TYPE> تهی vvp_darray_atom<TYPE>::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_atom<TYPE>*that = dynamic_cast<ثابت vvp_darray_atom<TYPE>*>(obj);
      أكد(that);

      unsigned رقم_اقلام = min(array_.size(), that->array_.size());
      برای (unsigned idx = 0 ; idx < رقم_اقلام ; idx += 1)
	    array_[idx] = that->array_[idx];
}

template <رده TYPE> vvp_object* vvp_darray_atom<TYPE>::duplicate(تهی) ثابت
{
      vvp_darray_atom<TYPE>*that = جدید vvp_darray_atom<TYPE>(array_.size());
      برای (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      بازگشت that;
}

template <رده TYPE> vvp_vector4_t vvp_darray_atom<TYPE>::get_bitstream(منطقی)
{
      ثابت unsigned word_wid = اندازه(TYPE) * 8;

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      وقتی (vdx > 0) {
            TYPE word = array_[adx++];
            vdx -= word_wid;
            برای (unsigned bdx = 0; bdx < word_wid; bdx += 1) {
                  اگر (word & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word >>= 1;
            }
      }

      بازگشت vec;
}

template رده vvp_darray_atom<uint8_t>;
template رده vvp_darray_atom<uint16_t>;
template رده vvp_darray_atom<uint32_t>;
template رده vvp_darray_atom<uint64_t>;
template رده vvp_darray_atom<int8_t>;
template رده vvp_darray_atom<int16_t>;
template رده vvp_darray_atom<int32_t>;
template رده vvp_darray_atom<int64_t>;

vvp_darray_vec4::~vvp_darray_vec4()
{
}

size_t vvp_darray_vec4::get_size(تهی) ثابت
{
      بازگشت array_.size();
}

تهی vvp_darray_vec4::set_word(unsigned adr, ثابت vvp_vector4_t&value)
{
      اگر (adr >= array_.size()) بازگشت;
      أكد(value.size() == word_wid_);
      array_[adr] = value;
}

تهی vvp_darray_vec4::get_word(unsigned adr, vvp_vector4_t&value)
{
	/*
	 * Return an undefined value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      اگر ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_X);
	    بازگشت;
      }
      value = array_[adr];
      أكد(value.size() == word_wid_);
}

تهی vvp_darray_vec4::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_vec4*that = dynamic_cast<ثابت vvp_darray_vec4*>(obj);
      أكد(that);

      unsigned رقم_اقلام = min(array_.size(), that->array_.size());
      برای (unsigned idx = 0 ; idx < رقم_اقلام ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_object* vvp_darray_vec4::duplicate(تهی) ثابت
{
      vvp_darray_vec4*that = جدید vvp_darray_vec4(array_.size(), word_wid_);

      برای (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      بازگشت that;
}

vvp_vector4_t vvp_darray_vec4::get_bitstream(منطقی as_vec4)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      وقتی (vdx > 0) {
            vdx -= word_wid_;
            برای (unsigned bdx = 0; bdx < word_wid_; bdx += 1) {
                  vvp_bit4_t bit = array_[adx].value(bdx);
                  اگر (as_vec4 || (bit == BIT4_1))
                        vec.set_bit(vdx+bdx, bit);
            }
            adx++;
      }

      بازگشت vec;
}

تهی vvp_darray_vec4::reverse_elems(تهی)
{
      size_t n = array_.size();
      برای (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

تهی vvp_darray_vec4::sort_elems(منطقی ascending)
{
      اگر (ascending) {
	    std::sort(array_.begin(), array_.end(), vec4_lt_unsigned);
      } وگرنه {
	    std::sort(array_.begin(), array_.end(),
		      [](ثابت vvp_vector4_t&a, ثابت vvp_vector4_t&b) {
			    بازگشت vec4_lt_unsigned(b, a);
		      });
      }
}

تهی vvp_darray_vec4::shuffle_elems(تهی)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

تهی vvp_darray_vec2::reverse_elems(تهی)
{
      size_t n = array_.size();
      برای (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

تهی vvp_darray_vec2::sort_elems(منطقی ascending)
{
      اگر (ascending) {
	    std::sort(array_.begin(), array_.end());
      } وگرنه {
	    std::sort(array_.begin(), array_.end(),
		      [](ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b) {
			    بازگشت b < a;
		      });
      }
}

تهی vvp_darray_vec2::shuffle_elems(تهی)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_darray_vec2::~vvp_darray_vec2()
{
}

size_t vvp_darray_vec2::get_size(تهی) ثابت
{
      بازگشت array_.size();
}

تهی vvp_darray_vec2::set_word(unsigned adr, ثابت vvp_vector4_t&value)
{
      اگر (adr >= array_.size()) بازگشت;
      أكد(value.size() == word_wid_);
      array_[adr] = value;
}

تهی vvp_darray_vec2::get_word(unsigned adr, vvp_vector4_t&value)
{
	/*
	 * Return a zero value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      اگر ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_0);
	    بازگشت;
      }
      أكد(array_[adr].size() == word_wid_);
      value.resize(word_wid_);
      برای (unsigned idx = 0; idx < word_wid_; idx += 1) {
	    value.set_bit(idx, array_[adr].value4(idx));
      }
}

تهی vvp_darray_vec2::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_vec2*that = dynamic_cast<ثابت vvp_darray_vec2*>(obj);
      أكد(that);

      unsigned رقم_اقلام = min(array_.size(), that->array_.size());
      برای (unsigned idx = 0 ; idx < رقم_اقلام ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_vector4_t vvp_darray_vec2::get_bitstream(منطقی)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      وقتی (vdx > 0) {
            vdx -= word_wid_;
            برای (unsigned bdx = 0; bdx < word_wid_; bdx += 1) {
                  اگر (array_[adx].value(bdx))
                        vec.set_bit(vdx+bdx, BIT4_1);
            }
            adx++;
      }

      بازگشت vec;
}

vvp_darray_object::~vvp_darray_object()
{
}

size_t vvp_darray_object::get_size() ثابت
{
      بازگشت array_.size();
}

تهی vvp_darray_object::set_word(unsigned adr, ثابت vvp_object_t&value)
{
      اگر (adr >= array_.size())
	    بازگشت;
      array_[adr] = value;
}

تهی vvp_darray_object::get_word(unsigned adr, vvp_object_t&value)
{
      اگر (adr >= array_.size()) {
	    value = vvp_object_t();
	    بازگشت;
      }

      value = array_[adr];
}

تهی vvp_darray_object::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_object*that = dynamic_cast<ثابت vvp_darray_object*>(obj);
      أكد(that);

      unsigned رقم_اقلام = min(array_.size(), that->array_.size());
      برای (unsigned idx = 0 ; idx < رقم_اقلام ; idx += 1)
	    array_[idx] = that->array_[idx];
}

تهی vvp_darray_object::reverse_elems(تهی)
{
      size_t n = array_.size();
      برای (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

تهی vvp_darray_object::sort_elems(منطقی ascending)
{
      اگر (ascending) {
	    std::sort(array_.begin(), array_.end(),
		      [](ثابت vvp_object_t&a, ثابت vvp_object_t&b) {
			    بازگشت a.sort_order_کلید() < b.sort_order_کلید();
		      });
      } وگرنه {
	    std::sort(array_.begin(), array_.end(),
		      [](ثابت vvp_object_t&a, ثابت vvp_object_t&b) {
			    بازگشت a.sort_order_کلید() > b.sort_order_کلید();
		      });
      }
}

تهی vvp_darray_object::shuffle_elems(تهی)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_darray_real::~vvp_darray_real()
{
}

size_t vvp_darray_real::get_size() ثابت
{
      بازگشت array_.size();
}

تهی vvp_darray_real::set_word(unsigned adr, دوگانه value)
{
      اگر (adr >= array_.size())
	    بازگشت;
      array_[adr] = value;
}

تهی vvp_darray_real::get_word(unsigned adr, دوگانه&value)
{
      اگر (adr >= array_.size()) {
	    value = 0.0;
	    بازگشت;
      }

      value = array_[adr];
}

تهی vvp_darray_real::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_real*that = dynamic_cast<ثابت vvp_darray_real*>(obj);
      أكد(that);

      unsigned رقم_اقلام = min(array_.size(), that->array_.size());
      برای (unsigned idx = 0 ; idx < رقم_اقلام ; idx += 1)
	    array_[idx] = that->array_[idx];
}

تهی vvp_darray_real::reverse_elems(تهی)
{
      size_t n = array_.size();
      برای (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

تهی vvp_darray_real::sort_elems(منطقی ascending)
{
      اگر (ascending) std::sort(array_.begin(), array_.end());
      وگرنه std::sort(array_.begin(), array_.end(), std::greater<دوگانه>());
}

تهی vvp_darray_real::shuffle_elems(تهی)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_real::duplicate(تهی) ثابت
{
      vvp_darray_real*that = جدید vvp_darray_real(array_.size());

      برای (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      بازگشت that;
}

vvp_vector4_t vvp_darray_real::get_bitstream(منطقی)
{
      ثابت unsigned word_wid = اندازه(دوگانه) * 8;
      أكد(word_wid == 64);

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      وقتی (vdx > 0) {
            اجتماع {
                دوگانه   value;
                uint64_t bits;
            } word;
            word.value = array_[adx++];
            vdx -= word_wid;
            برای (unsigned bdx = 0; bdx < word_wid; bdx += 1) {
                  اگر (word.bits & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word.bits >>= 1;
            }
      }

      بازگشت vec;
}

vvp_darray_string::~vvp_darray_string()
{
}

size_t vvp_darray_string::get_size() ثابت
{
      بازگشت array_.size();
}

تهی vvp_darray_string::set_word(unsigned adr, ثابت string&value)
{
      اگر (adr >= array_.size())
	    بازگشت;
      array_[adr] = value;
}

تهی vvp_darray_string::get_word(unsigned adr, string&value)
{
      اگر (adr >= array_.size()) {
	    value = "";
	    بازگشت;
      }

      value = array_[adr];
}

تهی vvp_darray_string::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_string*that = dynamic_cast<ثابت vvp_darray_string*>(obj);
      أكد(that);

      unsigned رقم_اقلام = min(array_.size(), that->array_.size());
      برای (unsigned idx = 0 ; idx < رقم_اقلام ; idx += 1)
	    array_[idx] = that->array_[idx];
}

تهی vvp_darray_string::reverse_elems(تهی)
{
      size_t n = array_.size();
      برای (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

تهی vvp_darray_string::sort_elems(منطقی ascending)
{
      اگر (ascending) std::sort(array_.begin(), array_.end());
      وگرنه std::sort(array_.begin(), array_.end(), std::greater<string>());
}

تهی vvp_darray_string::shuffle_elems(تهی)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_string::duplicate(تهی) ثابت
{
      vvp_darray_string*that = جدید vvp_darray_string(array_.size());

      برای (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      بازگشت that;
}

vvp_queue::~vvp_queue()
{
}

تهی vvp_queue::copy_elems(vvp_object_t, unsigned)
{
      cerr << "Sorry: copy_elems() not implemented for " << typeid(*این).name() << endl;
}

تهی vvp_queue::set_word_max(unsigned, ثابت vvp_vector4_t&, unsigned)
{
      cerr << "XXXX set_word_max(vvp_vector4_t) not implemented for " << typeid(*این).name() << endl;
}

تهی vvp_queue::insert(unsigned, ثابت vvp_vector4_t&, unsigned)
{
      cerr << "XXXX insert(vvp_vector4_t) not implemented for " << typeid(*این).name() << endl;
}

تهی vvp_queue::push_back(ثابت vvp_vector4_t&, unsigned)
{
      cerr << "XXXX push_back(vvp_vector4_t) not implemented for " << typeid(*این).name() << endl;
}

تهی vvp_queue::push_front(ثابت vvp_vector4_t&, unsigned)
{
      cerr << "XXXX push_front(vvp_vector4_t) not implemented for " << typeid(*این).name() << endl;
}

تهی vvp_queue::set_word_max(unsigned, دوگانه, unsigned)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*این).name() << endl;
}

تهی vvp_queue::insert(unsigned, دوگانه, unsigned)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*این).name() << endl;
}

تهی vvp_queue::push_back(دوگانه, unsigned)
{
      cerr << "XXXX push_back(double) not implemented for " << typeid(*این).name() << endl;
}

تهی vvp_queue::push_front(دوگانه, unsigned)
{
      cerr << "XXXX push_front(double) not implemented for " << typeid(*این).name() << endl;
}

تهی vvp_queue::set_word_max(unsigned, ثابت string&, unsigned)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*این).name() << endl;
}

تهی vvp_queue::insert(unsigned, ثابت string&, unsigned)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*این).name() << endl;
}

تهی vvp_queue::push_back(ثابت string&, unsigned)
{
      cerr << "XXXX push_back(string) not implemented for " << typeid(*این).name() << endl;
}

تهی vvp_queue::push_front(ثابت string&, unsigned)
{
      cerr << "XXXX push_front(string) not implemented for " << typeid(*این).name() << endl;
}

/*
 * Helper functions used while copying multiple elements into a queue.
 */
ایستا تهی print_copy_is_too_big(size_t src_size, unsigned max_size, ثابت string&qtype)
{
      cerr << get_fileline()
           << "Warning: queue<" << qtype << "> is bounded to have at most "
           << max_size << " elements, source has " << src_size << " elements." << endl;
}

ایستا تهی print_copy_is_too_big(دوگانه&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "real");
}

ایستا تهی print_copy_is_too_big(string&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "string");
}

ایستا تهی print_copy_is_too_big(vvp_vector4_t&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "vector");
}

template <typename ELEM, رده QTYPE, رده SRC_TYPE>
ایستا تهی copy_elements(QTYPE*queue, SRC_TYPE*src, unsigned max_size)
{
      size_t src_size = src->get_size();
      اگر ((max_size != 0) && (src_size > max_size)) {
	    ELEM tmp;
	    print_copy_is_too_big(tmp, src_size, max_size);
      }
      unsigned copy_size = ((src_size < max_size) ||
                            (max_size == 0)) ? src_size : max_size;
      اگر (copy_size < queue->get_size())
	    queue->erase_tail(copy_size);
      برای (unsigned idx=0; idx < copy_size; ++idx) {
	    ELEM value;
	    src->get_word(idx, value);
	    queue->set_word_max(idx, value, max_size);
      }
}

تهی vvp_queue_real::copy_elems(vvp_object_t src, unsigned max_size)
{
      اگر (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<دوگانه, vvp_queue_real, vvp_queue>(این, src_queue, max_size);
      وگرنه اگر (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<دوگانه, vvp_queue_real, vvp_darray>(این, src_darray, max_size);
      وگرنه
	    cerr << get_fileline() << "Sorry: cannot copy object to real queue." << endl;
}

تهی vvp_queue_real::set_word_max(unsigned adr, دوگانه value, unsigned max_size)
{
      اگر (adr == queue.size())
	    اگر (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    وگرنه
		  cerr << get_fileline()
		       << "Warning: assigning to queue<real>[" << adr << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      وگرنه
	    set_word(adr, value);
}

تهی vvp_queue_real::set_word(unsigned adr, دوگانه value)
{
      اگر (adr < queue.size())
	    queue[adr] = value;
      وگرنه
	    cerr << get_fileline()
	         << "Warning: assigning to queue<real>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
}

تهی vvp_queue_real::get_word(unsigned adr, دوگانه&value)
{
      اگر (adr >= queue.size())
	    value = 0.0;
      وگرنه
	    value = queue[adr];
}

تهی vvp_queue_real::insert(unsigned idx, دوگانه value, unsigned max_size)
{
	// Inserting past the end of the queue
      اگر (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<real>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
	// Inserting at the end
      وگرنه اگر (idx == queue.size())
	    اگر (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    وگرنه
		  cerr << get_fileline()
		       << "Warning: inserting to queue<real>[" << idx << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      وگرنه  {
	    اگر (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<real> ["
		       << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

تهی vvp_queue_real::push_back(دوگانه value, unsigned max_size)
{
      اگر (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      وگرنه
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<real> ["
	         << max_size << "]." << endl;
}

تهی vvp_queue_real::push_front(دوگانه value, unsigned max_size)
{
      اگر (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<real> ["
	         << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

تهی vvp_queue_real::erase(unsigned idx)
{
      أكد(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

تهی vvp_queue_real::erase_tail(unsigned idx)
{
      أكد(queue.size() >= idx);
      اگر (queue.size() > idx)
	    queue.resize(idx);
}

تهی vvp_queue_string::copy_elems(vvp_object_t src, unsigned max_size)
{
      اگر (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<string, vvp_queue_string, vvp_queue>(این, src_queue, max_size);
      وگرنه اگر (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<string, vvp_queue_string, vvp_darray>(این, src_darray, max_size);
      وگرنه
	    cerr << get_fileline() << "Sorry: cannot copy object to string queue." << endl;
}

تهی vvp_queue_string::set_word_max(unsigned adr, ثابت string&value, unsigned max_size)
{
      اگر (adr == queue.size())
	    اگر (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    وگرنه
		  cerr << get_fileline()
		       << "Warning: assigning to queue<string>[" << adr << "] is"
		          " outside bound (" << max_size << "). \"" << value
		       << "\" was not added." << endl;
      وگرنه
	    set_word(adr, value);
}

تهی vvp_queue_string::set_word(unsigned adr, ثابت string&value)
{
      اگر (adr < queue.size())
	    queue[adr] = value;
      وگرنه
	    cerr << get_fileline()
	         << "Warning: assigning to queue<string>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << endl;
}

تهی vvp_queue_string::get_word(unsigned adr, string&value)
{
      اگر (adr >= queue.size())
	    value = "";
      وگرنه
	    value = queue[adr];
}

تهی vvp_queue_string::insert(unsigned idx, ثابت string&value, unsigned max_size)
{
	// Inserting past the end of the queue
      اگر (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<string>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << endl;
	// Inserting at the end
      وگرنه اگر (idx == queue.size())
	    اگر (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    وگرنه
		  cerr << get_fileline()
		       << "Warning: inserting to queue<string>[" << idx << "] is"
		          " outside bound (" << max_size << "). \"" << value
		       << "\" was not added." << endl;
      وگرنه  {
	    اگر (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", \"" << value << "\") removed \""
		       << queue.back() << "\" from already full bounded queue<string> ["
		       << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

تهی vvp_queue_string::push_back(ثابت string&value, unsigned max_size)
{
      اگر (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      وگرنه
	    cerr << get_fileline()
	         << "Warning: push_back(\"" << value
	         << "\") skipped for already full bounded queue<string> ["
	         << max_size << "]." << endl;
}

تهی vvp_queue_string::push_front(ثابت string&value, unsigned max_size)
{
      اگر (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(\"" << value << "\") removed \""
	         << queue.back() << "\" from already full bounded queue<string> ["
	         << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

تهی vvp_queue_string::erase(unsigned idx)
{
      أكد(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

تهی vvp_queue_string::erase_tail(unsigned idx)
{
      أكد(queue.size() >= idx);
      اگر (queue.size() > idx)
	    queue.resize(idx);
}

تهی vvp_queue_real::reverse_elems(تهی)
{
      std::reverse(queue.begin(), queue.end());
}

تهی vvp_queue_string::reverse_elems(تهی)
{
      std::reverse(queue.begin(), queue.end());
}

تهی vvp_queue_real::sort_elems(منطقی ascending)
{
      اگر (ascending) std::sort(queue.begin(), queue.end());
      وگرنه std::sort(queue.begin(), queue.end(), std::greater<دوگانه>());
}

تهی vvp_queue_real::shuffle_elems(تهی)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

تهی vvp_queue_string::sort_elems(منطقی ascending)
{
      اگر (ascending) std::sort(queue.begin(), queue.end());
      وگرنه std::sort(queue.begin(), queue.end(), std::greater<string>());
}

تهی vvp_queue_string::shuffle_elems(تهی)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

تهی vvp_queue_vec4::copy_elems(vvp_object_t src, unsigned max_size)
{
      اگر (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_queue>(این, src_queue, max_size);
      وگرنه اگر (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_darray>(این, src_darray, max_size);
      وگرنه
	    cerr << get_fileline() << "Sorry: cannot copy object to vector queue." << endl;
}

تهی vvp_queue_vec4::set_word_max(unsigned adr, ثابت vvp_vector4_t&value, unsigned max_size)
{
      اگر (adr == queue.size())
	    اگر (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    وگرنه
		  cerr << get_fileline()
		       << "Warning: assigning to queue<vector>[" << adr << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      وگرنه
	    set_word(adr, value);
}

تهی vvp_queue_vec4::set_word(unsigned adr, ثابت vvp_vector4_t&value)
{
      اگر (adr < queue.size())
	    queue[adr] = value;
      وگرنه
	    cerr << get_fileline()
	         << "Warning: assigning to queue<vector>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
}

تهی vvp_queue_vec4::get_word(unsigned adr, vvp_vector4_t&value)
{
      اگر (adr >= queue.size())
	    value = vvp_vector4_t(queue[0].size());
      وگرنه
	    value = queue[adr];
}

تهی vvp_queue_vec4::insert(unsigned idx, ثابت vvp_vector4_t&value, unsigned max_size)
{
	// Inserting past the end of the queue
      اگر (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<vector[" << value.size()
	         << "]>[" << idx << "] is outside of size (" << queue.size()
	         << "). " << value << " was not added." << endl;
	// Inserting at the end
      وگرنه اگر (idx == queue.size())
	    اگر (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    وگرنه
		  cerr << get_fileline()
		       << "Warning: inserting to queue<vector[" << value.size()
		       << "]>[" << idx << "] is outside bound (" << max_size
		       << "). " << value << " was not added." << endl;
      وگرنه  {
	    اگر (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<vector["
		       << value.size() << "]> [" << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

تهی vvp_queue_vec4::push_back(ثابت vvp_vector4_t&value, unsigned max_size)
{
      اگر (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      وگرنه
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<vector["
	         << value.size() << "]> [" << max_size << "]." << endl;
}

تهی vvp_queue_vec4::push_front(ثابت vvp_vector4_t&value, unsigned max_size)
{
      اگر (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<vector["
	         << value.size() << "]> [" << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

تهی vvp_queue_vec4::erase(unsigned idx)
{
      أكد(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

تهی vvp_queue_vec4::erase_tail(unsigned idx)
{
      أكد(queue.size() >= idx);
      اگر (queue.size() > idx)
	    queue.resize(idx);
}

تهی vvp_queue_vec4::reverse_elems(تهی)
{
      std::reverse(queue.begin(), queue.end());
}

تهی vvp_queue_vec4::sort_elems(منطقی ascending)
{
      اگر (ascending) {
	    std::sort(queue.begin(), queue.end(), vec4_lt_unsigned);
      } وگرنه {
	    std::sort(queue.begin(), queue.end(),
		      [](ثابت vvp_vector4_t&a, ثابت vvp_vector4_t&b) {
			    بازگشت vec4_lt_unsigned(b, a);
		      });
      }
}

تهی vvp_queue_vec4::shuffle_elems(تهی)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}
