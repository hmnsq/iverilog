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

using نطاق std;

نطاق {

ساكن std::mt19937& shuffle_rng(فراغ)
{
      ساكن std::mt19937 rng(std::random_device{}());
      أرجع rng;
}

ساكن منطقي vec4_lt_unsigned(ثابت vvp_vector4_t&a, ثابت vvp_vector4_t&b)
{
      إذا (a.eeq(b)) أرجع خطأ;
      vvp_bit4_t gt = compare_gtge(b, a, BIT4_1);
      إذا (gt == BIT4_X) أرجع خطأ;
      أرجع gt == BIT4_1;
}

} // namespace

vvp_darray::~vvp_darray()
{
}

فراغ vvp_darray::reverse_elems(فراغ)
{
      cerr << "XXXX reverse_elems() not implemented for "
           << typeid(*هذا).name() << endl;
}

فراغ vvp_darray::sort_elems(منطقي /*ascending*/)
{
      cerr << "XXXX sort_elems() not implemented for "
           << typeid(*هذا).name() << endl;
}

فراغ vvp_darray::shuffle_elems(فراغ)
{
      cerr << "XXXX shuffle_elems() not implemented for "
           << typeid(*هذا).name() << endl;
}

فراغ vvp_darray::set_word(unsigned, ثابت vvp_vector4_t&)
{
      cerr << "XXXX set_word(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << endl;
}

فراغ vvp_darray::set_word(unsigned, مزدوج)
{
      cerr << "XXXX set_word(double) not implemented for " << typeid(*هذا).name() << endl;
}

فراغ vvp_darray::set_word(unsigned, ثابت string&)
{
      cerr << "XXXX set_word(string) not implemented for " << typeid(*هذا).name() << endl;
}

فراغ vvp_darray::set_word(unsigned, ثابت vvp_object_t&)
{
      cerr << "XXXX set_word(vvp_object_t) not implemented for " << typeid(*هذا).name() << endl;
}

فراغ vvp_darray::get_word(unsigned, vvp_vector4_t&)
{
      cerr << "XXXX get_word(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << endl;
}

فراغ vvp_darray::get_word(unsigned, مزدوج&)
{
      cerr << "XXXX get_word(double) not implemented for " << typeid(*هذا).name() << endl;
}

فراغ vvp_darray::get_word(unsigned, string&)
{
      cerr << "XXXX get_word(string) not implemented for " << typeid(*هذا).name() << endl;
}

فراغ vvp_darray::get_word(unsigned, vvp_object_t&)
{
      cerr << "XXXX get_word(vvp_object_t) not implemented for " << typeid(*هذا).name() << endl;
}

vvp_vector4_t vvp_darray::get_bitstream(منطقي)
{
      cerr << "XXXX get_bitstream() not implemented for " << typeid(*هذا).name() << endl;
      أرجع vvp_vector4_t();
}

template <صنف TYPE> vvp_darray_atom<TYPE>::~vvp_darray_atom()
{
}

template <صنف TYPE> size_t vvp_darray_atom<TYPE>::get_size() ثابت
{
      أرجع array_.size();
}

template <صنف TYPE> فراغ vvp_darray_atom<TYPE>::reverse_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

template <صنف TYPE> فراغ vvp_darray_atom<TYPE>::sort_elems(منطقي ascending)
{
      إذا (ascending) std::sort(array_.begin(), array_.end());
      وإلا std::sort(array_.begin(), array_.end(), std::greater<TYPE>());
}

template <صنف TYPE> فراغ vvp_darray_atom<TYPE>::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

template <صنف TYPE> فراغ vvp_darray_atom<TYPE>::set_word(unsigned adr, ثابت vvp_vector4_t&value)
{
      إذا (adr >= array_.size())
	    أرجع;
      TYPE tmp;
      vector4_to_بەها(value, tmp, true, خطأ);
      array_[adr] = tmp;
}

template <صنف TYPE> فراغ vvp_darray_atom<TYPE>::get_word(unsigned adr, vvp_vector4_t&value)
{
      إذا (adr >= array_.size()) {
	    value = vvp_vector4_t(8*حجم(TYPE), BIT4_X);
	    أرجع;
      }

      TYPE word = array_[adr];
      vvp_vector4_t tmp (8*حجم(TYPE), BIT4_0);
      لكل (unsigned idx = 0 ; idx < tmp.size() ; idx += 1) {
	    إذا (word&1) tmp.set_bit(idx, BIT4_1);
	    word >>= 1;
      }
      value = tmp;
}

template <صنف TYPE> فراغ vvp_darray_atom<TYPE>::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_atom<TYPE>*that = dynamic_cast<ثابت vvp_darray_atom<TYPE>*>(obj);
      أكد(that);

      unsigned ژمارە_کاڵاکان = min(array_.size(), that->array_.size());
      لكل (unsigned idx = 0 ; idx < ژمارە_کاڵاکان ; idx += 1)
	    array_[idx] = that->array_[idx];
}

template <صنف TYPE> vvp_object* vvp_darray_atom<TYPE>::duplicate(فراغ) ثابت
{
      vvp_darray_atom<TYPE>*that = جديد vvp_darray_atom<TYPE>(array_.size());
      لكل (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      أرجع that;
}

template <صنف TYPE> vvp_vector4_t vvp_darray_atom<TYPE>::get_bitstream(منطقي)
{
      ثابت unsigned word_wid = حجم(TYPE) * 8;

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      طالما (vdx > 0) {
            TYPE word = array_[adx++];
            vdx -= word_wid;
            لكل (unsigned bdx = 0; bdx < word_wid; bdx += 1) {
                  إذا (word & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word >>= 1;
            }
      }

      أرجع vec;
}

template صنف vvp_darray_atom<uint8_t>;
template صنف vvp_darray_atom<uint16_t>;
template صنف vvp_darray_atom<uint32_t>;
template صنف vvp_darray_atom<uint64_t>;
template صنف vvp_darray_atom<int8_t>;
template صنف vvp_darray_atom<int16_t>;
template صنف vvp_darray_atom<int32_t>;
template صنف vvp_darray_atom<int64_t>;

vvp_darray_vec4::~vvp_darray_vec4()
{
}

size_t vvp_darray_vec4::get_size(فراغ) ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_vec4::set_word(unsigned adr, ثابت vvp_vector4_t&value)
{
      إذا (adr >= array_.size()) أرجع;
      أكد(value.size() == word_wid_);
      array_[adr] = value;
}

فراغ vvp_darray_vec4::get_word(unsigned adr, vvp_vector4_t&value)
{
	/*
	 * Return an undefined value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      إذا ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_X);
	    أرجع;
      }
      value = array_[adr];
      أكد(value.size() == word_wid_);
}

فراغ vvp_darray_vec4::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_vec4*that = dynamic_cast<ثابت vvp_darray_vec4*>(obj);
      أكد(that);

      unsigned ژمارە_کاڵاکان = min(array_.size(), that->array_.size());
      لكل (unsigned idx = 0 ; idx < ژمارە_کاڵاکان ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_object* vvp_darray_vec4::duplicate(فراغ) ثابت
{
      vvp_darray_vec4*that = جديد vvp_darray_vec4(array_.size(), word_wid_);

      لكل (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      أرجع that;
}

vvp_vector4_t vvp_darray_vec4::get_bitstream(منطقي as_vec4)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      طالما (vdx > 0) {
            vdx -= word_wid_;
            لكل (unsigned bdx = 0; bdx < word_wid_; bdx += 1) {
                  vvp_bit4_t bit = array_[adx].value(bdx);
                  إذا (as_vec4 || (bit == BIT4_1))
                        vec.set_bit(vdx+bdx, bit);
            }
            adx++;
      }

      أرجع vec;
}

فراغ vvp_darray_vec4::reverse_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_vec4::sort_elems(منطقي ascending)
{
      إذا (ascending) {
	    std::sort(array_.begin(), array_.end(), vec4_lt_unsigned);
      } وإلا {
	    std::sort(array_.begin(), array_.end(),
		      [](ثابت vvp_vector4_t&a, ثابت vvp_vector4_t&b) {
			    أرجع vec4_lt_unsigned(b, a);
		      });
      }
}

فراغ vvp_darray_vec4::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

فراغ vvp_darray_vec2::reverse_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_vec2::sort_elems(منطقي ascending)
{
      إذا (ascending) {
	    std::sort(array_.begin(), array_.end());
      } وإلا {
	    std::sort(array_.begin(), array_.end(),
		      [](ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b) {
			    أرجع b < a;
		      });
      }
}

فراغ vvp_darray_vec2::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_darray_vec2::~vvp_darray_vec2()
{
}

size_t vvp_darray_vec2::get_size(فراغ) ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_vec2::set_word(unsigned adr, ثابت vvp_vector4_t&value)
{
      إذا (adr >= array_.size()) أرجع;
      أكد(value.size() == word_wid_);
      array_[adr] = value;
}

فراغ vvp_darray_vec2::get_word(unsigned adr, vvp_vector4_t&value)
{
	/*
	 * Return a zero value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      إذا ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_0);
	    أرجع;
      }
      أكد(array_[adr].size() == word_wid_);
      value.resize(word_wid_);
      لكل (unsigned idx = 0; idx < word_wid_; idx += 1) {
	    value.set_bit(idx, array_[adr].value4(idx));
      }
}

فراغ vvp_darray_vec2::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_vec2*that = dynamic_cast<ثابت vvp_darray_vec2*>(obj);
      أكد(that);

      unsigned ژمارە_کاڵاکان = min(array_.size(), that->array_.size());
      لكل (unsigned idx = 0 ; idx < ژمارە_کاڵاکان ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_vector4_t vvp_darray_vec2::get_bitstream(منطقي)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      طالما (vdx > 0) {
            vdx -= word_wid_;
            لكل (unsigned bdx = 0; bdx < word_wid_; bdx += 1) {
                  إذا (array_[adx].value(bdx))
                        vec.set_bit(vdx+bdx, BIT4_1);
            }
            adx++;
      }

      أرجع vec;
}

vvp_darray_object::~vvp_darray_object()
{
}

size_t vvp_darray_object::get_size() ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_object::set_word(unsigned adr, ثابت vvp_object_t&value)
{
      إذا (adr >= array_.size())
	    أرجع;
      array_[adr] = value;
}

فراغ vvp_darray_object::get_word(unsigned adr, vvp_object_t&value)
{
      إذا (adr >= array_.size()) {
	    value = vvp_object_t();
	    أرجع;
      }

      value = array_[adr];
}

فراغ vvp_darray_object::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_object*that = dynamic_cast<ثابت vvp_darray_object*>(obj);
      أكد(that);

      unsigned ژمارە_کاڵاکان = min(array_.size(), that->array_.size());
      لكل (unsigned idx = 0 ; idx < ژمارە_کاڵاکان ; idx += 1)
	    array_[idx] = that->array_[idx];
}

فراغ vvp_darray_object::reverse_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_object::sort_elems(منطقي ascending)
{
      إذا (ascending) {
	    std::sort(array_.begin(), array_.end(),
		      [](ثابت vvp_object_t&a, ثابت vvp_object_t&b) {
			    أرجع a.sort_order_کلیل() < b.sort_order_کلیل();
		      });
      } وإلا {
	    std::sort(array_.begin(), array_.end(),
		      [](ثابت vvp_object_t&a, ثابت vvp_object_t&b) {
			    أرجع a.sort_order_کلیل() > b.sort_order_کلیل();
		      });
      }
}

فراغ vvp_darray_object::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_darray_real::~vvp_darray_real()
{
}

size_t vvp_darray_real::get_size() ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_real::set_word(unsigned adr, مزدوج value)
{
      إذا (adr >= array_.size())
	    أرجع;
      array_[adr] = value;
}

فراغ vvp_darray_real::get_word(unsigned adr, مزدوج&value)
{
      إذا (adr >= array_.size()) {
	    value = 0.0;
	    أرجع;
      }

      value = array_[adr];
}

فراغ vvp_darray_real::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_real*that = dynamic_cast<ثابت vvp_darray_real*>(obj);
      أكد(that);

      unsigned ژمارە_کاڵاکان = min(array_.size(), that->array_.size());
      لكل (unsigned idx = 0 ; idx < ژمارە_کاڵاکان ; idx += 1)
	    array_[idx] = that->array_[idx];
}

فراغ vvp_darray_real::reverse_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_real::sort_elems(منطقي ascending)
{
      إذا (ascending) std::sort(array_.begin(), array_.end());
      وإلا std::sort(array_.begin(), array_.end(), std::greater<مزدوج>());
}

فراغ vvp_darray_real::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_real::duplicate(فراغ) ثابت
{
      vvp_darray_real*that = جديد vvp_darray_real(array_.size());

      لكل (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      أرجع that;
}

vvp_vector4_t vvp_darray_real::get_bitstream(منطقي)
{
      ثابت unsigned word_wid = حجم(مزدوج) * 8;
      أكد(word_wid == 64);

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      طالما (vdx > 0) {
            اتحاد {
                مزدوج   value;
                uint64_t bits;
            } word;
            word.value = array_[adx++];
            vdx -= word_wid;
            لكل (unsigned bdx = 0; bdx < word_wid; bdx += 1) {
                  إذا (word.bits & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word.bits >>= 1;
            }
      }

      أرجع vec;
}

vvp_darray_string::~vvp_darray_string()
{
}

size_t vvp_darray_string::get_size() ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_string::set_word(unsigned adr, ثابت string&value)
{
      إذا (adr >= array_.size())
	    أرجع;
      array_[adr] = value;
}

فراغ vvp_darray_string::get_word(unsigned adr, string&value)
{
      إذا (adr >= array_.size()) {
	    value = "";
	    أرجع;
      }

      value = array_[adr];
}

فراغ vvp_darray_string::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_string*that = dynamic_cast<ثابت vvp_darray_string*>(obj);
      أكد(that);

      unsigned ژمارە_کاڵاکان = min(array_.size(), that->array_.size());
      لكل (unsigned idx = 0 ; idx < ژمارە_کاڵاکان ; idx += 1)
	    array_[idx] = that->array_[idx];
}

فراغ vvp_darray_string::reverse_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_string::sort_elems(منطقي ascending)
{
      إذا (ascending) std::sort(array_.begin(), array_.end());
      وإلا std::sort(array_.begin(), array_.end(), std::greater<string>());
}

فراغ vvp_darray_string::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_string::duplicate(فراغ) ثابت
{
      vvp_darray_string*that = جديد vvp_darray_string(array_.size());

      لكل (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      أرجع that;
}

vvp_queue::~vvp_queue()
{
}

فراغ vvp_queue::copy_elems(vvp_object_t, unsigned)
{
      cerr << "Sorry: copy_elems() not implemented for " << typeid(*هذا).name() << endl;
}

فراغ vvp_queue::set_word_max(unsigned, ثابت vvp_vector4_t&, unsigned)
{
      cerr << "XXXX set_word_max(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << endl;
}

فراغ vvp_queue::insert(unsigned, ثابت vvp_vector4_t&, unsigned)
{
      cerr << "XXXX insert(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << endl;
}

فراغ vvp_queue::push_back(ثابت vvp_vector4_t&, unsigned)
{
      cerr << "XXXX push_back(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << endl;
}

فراغ vvp_queue::push_front(ثابت vvp_vector4_t&, unsigned)
{
      cerr << "XXXX push_front(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << endl;
}

فراغ vvp_queue::set_word_max(unsigned, مزدوج, unsigned)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*هذا).name() << endl;
}

فراغ vvp_queue::insert(unsigned, مزدوج, unsigned)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*هذا).name() << endl;
}

فراغ vvp_queue::push_back(مزدوج, unsigned)
{
      cerr << "XXXX push_back(double) not implemented for " << typeid(*هذا).name() << endl;
}

فراغ vvp_queue::push_front(مزدوج, unsigned)
{
      cerr << "XXXX push_front(double) not implemented for " << typeid(*هذا).name() << endl;
}

فراغ vvp_queue::set_word_max(unsigned, ثابت string&, unsigned)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*هذا).name() << endl;
}

فراغ vvp_queue::insert(unsigned, ثابت string&, unsigned)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*هذا).name() << endl;
}

فراغ vvp_queue::push_back(ثابت string&, unsigned)
{
      cerr << "XXXX push_back(string) not implemented for " << typeid(*هذا).name() << endl;
}

فراغ vvp_queue::push_front(ثابت string&, unsigned)
{
      cerr << "XXXX push_front(string) not implemented for " << typeid(*هذا).name() << endl;
}

/*
 * Helper functions used while copying multiple elements into a queue.
 */
ساكن فراغ print_copy_is_too_big(size_t src_size, unsigned max_size, ثابت string&qtype)
{
      cerr << get_fileline()
           << "Warning: queue<" << qtype << "> is bounded to have at most "
           << max_size << " elements, source has " << src_size << " elements." << endl;
}

ساكن فراغ print_copy_is_too_big(مزدوج&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "real");
}

ساكن فراغ print_copy_is_too_big(string&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "string");
}

ساكن فراغ print_copy_is_too_big(vvp_vector4_t&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "vector");
}

template <typename ELEM, صنف QTYPE, صنف SRC_TYPE>
ساكن فراغ copy_elements(QTYPE*queue, SRC_TYPE*src, unsigned max_size)
{
      size_t src_size = src->get_size();
      إذا ((max_size != 0) && (src_size > max_size)) {
	    ELEM tmp;
	    print_copy_is_too_big(tmp, src_size, max_size);
      }
      unsigned copy_size = ((src_size < max_size) ||
                            (max_size == 0)) ? src_size : max_size;
      إذا (copy_size < queue->get_size())
	    queue->erase_tail(copy_size);
      لكل (unsigned idx=0; idx < copy_size; ++idx) {
	    ELEM value;
	    src->get_word(idx, value);
	    queue->set_word_max(idx, value, max_size);
      }
}

فراغ vvp_queue_real::copy_elems(vvp_object_t src, unsigned max_size)
{
      إذا (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<مزدوج, vvp_queue_real, vvp_queue>(هذا, src_queue, max_size);
      وإلا إذا (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<مزدوج, vvp_queue_real, vvp_darray>(هذا, src_darray, max_size);
      وإلا
	    cerr << get_fileline() << "Sorry: cannot copy object to real queue." << endl;
}

فراغ vvp_queue_real::set_word_max(unsigned adr, مزدوج value, unsigned max_size)
{
      إذا (adr == queue.size())
	    إذا (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: assigning to queue<real>[" << adr << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      وإلا
	    set_word(adr, value);
}

فراغ vvp_queue_real::set_word(unsigned adr, مزدوج value)
{
      إذا (adr < queue.size())
	    queue[adr] = value;
      وإلا
	    cerr << get_fileline()
	         << "Warning: assigning to queue<real>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
}

فراغ vvp_queue_real::get_word(unsigned adr, مزدوج&value)
{
      إذا (adr >= queue.size())
	    value = 0.0;
      وإلا
	    value = queue[adr];
}

فراغ vvp_queue_real::insert(unsigned idx, مزدوج value, unsigned max_size)
{
	// Inserting past the end of the queue
      إذا (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<real>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
	// Inserting at the end
      وإلا إذا (idx == queue.size())
	    إذا (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: inserting to queue<real>[" << idx << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      وإلا  {
	    إذا (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<real> ["
		       << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

فراغ vvp_queue_real::push_back(مزدوج value, unsigned max_size)
{
      إذا (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      وإلا
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<real> ["
	         << max_size << "]." << endl;
}

فراغ vvp_queue_real::push_front(مزدوج value, unsigned max_size)
{
      إذا (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<real> ["
	         << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

فراغ vvp_queue_real::erase(unsigned idx)
{
      أكد(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

فراغ vvp_queue_real::erase_tail(unsigned idx)
{
      أكد(queue.size() >= idx);
      إذا (queue.size() > idx)
	    queue.resize(idx);
}

فراغ vvp_queue_string::copy_elems(vvp_object_t src, unsigned max_size)
{
      إذا (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<string, vvp_queue_string, vvp_queue>(هذا, src_queue, max_size);
      وإلا إذا (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<string, vvp_queue_string, vvp_darray>(هذا, src_darray, max_size);
      وإلا
	    cerr << get_fileline() << "Sorry: cannot copy object to string queue." << endl;
}

فراغ vvp_queue_string::set_word_max(unsigned adr, ثابت string&value, unsigned max_size)
{
      إذا (adr == queue.size())
	    إذا (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: assigning to queue<string>[" << adr << "] is"
		          " outside bound (" << max_size << "). \"" << value
		       << "\" was not added." << endl;
      وإلا
	    set_word(adr, value);
}

فراغ vvp_queue_string::set_word(unsigned adr, ثابت string&value)
{
      إذا (adr < queue.size())
	    queue[adr] = value;
      وإلا
	    cerr << get_fileline()
	         << "Warning: assigning to queue<string>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << endl;
}

فراغ vvp_queue_string::get_word(unsigned adr, string&value)
{
      إذا (adr >= queue.size())
	    value = "";
      وإلا
	    value = queue[adr];
}

فراغ vvp_queue_string::insert(unsigned idx, ثابت string&value, unsigned max_size)
{
	// Inserting past the end of the queue
      إذا (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<string>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << endl;
	// Inserting at the end
      وإلا إذا (idx == queue.size())
	    إذا (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: inserting to queue<string>[" << idx << "] is"
		          " outside bound (" << max_size << "). \"" << value
		       << "\" was not added." << endl;
      وإلا  {
	    إذا (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", \"" << value << "\") removed \""
		       << queue.back() << "\" from already full bounded queue<string> ["
		       << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

فراغ vvp_queue_string::push_back(ثابت string&value, unsigned max_size)
{
      إذا (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      وإلا
	    cerr << get_fileline()
	         << "Warning: push_back(\"" << value
	         << "\") skipped for already full bounded queue<string> ["
	         << max_size << "]." << endl;
}

فراغ vvp_queue_string::push_front(ثابت string&value, unsigned max_size)
{
      إذا (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(\"" << value << "\") removed \""
	         << queue.back() << "\" from already full bounded queue<string> ["
	         << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

فراغ vvp_queue_string::erase(unsigned idx)
{
      أكد(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

فراغ vvp_queue_string::erase_tail(unsigned idx)
{
      أكد(queue.size() >= idx);
      إذا (queue.size() > idx)
	    queue.resize(idx);
}

فراغ vvp_queue_real::reverse_elems(فراغ)
{
      std::reverse(queue.begin(), queue.end());
}

فراغ vvp_queue_string::reverse_elems(فراغ)
{
      std::reverse(queue.begin(), queue.end());
}

فراغ vvp_queue_real::sort_elems(منطقي ascending)
{
      إذا (ascending) std::sort(queue.begin(), queue.end());
      وإلا std::sort(queue.begin(), queue.end(), std::greater<مزدوج>());
}

فراغ vvp_queue_real::shuffle_elems(فراغ)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

فراغ vvp_queue_string::sort_elems(منطقي ascending)
{
      إذا (ascending) std::sort(queue.begin(), queue.end());
      وإلا std::sort(queue.begin(), queue.end(), std::greater<string>());
}

فراغ vvp_queue_string::shuffle_elems(فراغ)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

فراغ vvp_queue_vec4::copy_elems(vvp_object_t src, unsigned max_size)
{
      إذا (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_queue>(هذا, src_queue, max_size);
      وإلا إذا (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_darray>(هذا, src_darray, max_size);
      وإلا
	    cerr << get_fileline() << "Sorry: cannot copy object to vector queue." << endl;
}

فراغ vvp_queue_vec4::set_word_max(unsigned adr, ثابت vvp_vector4_t&value, unsigned max_size)
{
      إذا (adr == queue.size())
	    إذا (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: assigning to queue<vector>[" << adr << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      وإلا
	    set_word(adr, value);
}

فراغ vvp_queue_vec4::set_word(unsigned adr, ثابت vvp_vector4_t&value)
{
      إذا (adr < queue.size())
	    queue[adr] = value;
      وإلا
	    cerr << get_fileline()
	         << "Warning: assigning to queue<vector>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
}

فراغ vvp_queue_vec4::get_word(unsigned adr, vvp_vector4_t&value)
{
      إذا (adr >= queue.size())
	    value = vvp_vector4_t(queue[0].size());
      وإلا
	    value = queue[adr];
}

فراغ vvp_queue_vec4::insert(unsigned idx, ثابت vvp_vector4_t&value, unsigned max_size)
{
	// Inserting past the end of the queue
      إذا (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<vector[" << value.size()
	         << "]>[" << idx << "] is outside of size (" << queue.size()
	         << "). " << value << " was not added." << endl;
	// Inserting at the end
      وإلا إذا (idx == queue.size())
	    إذا (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: inserting to queue<vector[" << value.size()
		       << "]>[" << idx << "] is outside bound (" << max_size
		       << "). " << value << " was not added." << endl;
      وإلا  {
	    إذا (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<vector["
		       << value.size() << "]> [" << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

فراغ vvp_queue_vec4::push_back(ثابت vvp_vector4_t&value, unsigned max_size)
{
      إذا (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      وإلا
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<vector["
	         << value.size() << "]> [" << max_size << "]." << endl;
}

فراغ vvp_queue_vec4::push_front(ثابت vvp_vector4_t&value, unsigned max_size)
{
      إذا (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<vector["
	         << value.size() << "]> [" << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

فراغ vvp_queue_vec4::erase(unsigned idx)
{
      أكد(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

فراغ vvp_queue_vec4::erase_tail(unsigned idx)
{
      أكد(queue.size() >= idx);
      إذا (queue.size() > idx)
	    queue.resize(idx);
}

فراغ vvp_queue_vec4::reverse_elems(فراغ)
{
      std::reverse(queue.begin(), queue.end());
}

فراغ vvp_queue_vec4::sort_elems(منطقي ascending)
{
      إذا (ascending) {
	    std::sort(queue.begin(), queue.end(), vec4_lt_unsigned);
      } وإلا {
	    std::sort(queue.begin(), queue.end(),
		      [](ثابت vvp_vector4_t&a, ثابت vvp_vector4_t&b) {
			    أرجع vec4_lt_unsigned(b, a);
		      });
      }
}

فراغ vvp_queue_vec4::shuffle_elems(فراغ)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}
