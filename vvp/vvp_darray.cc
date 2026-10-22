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

using நாமாகாஶ std;

நாமாகாஶ {

நிலையான std::mt19937& shuffle_rng(வெறுமை)
{
      நிலையான std::mt19937 rng(std::random_device{}());
      திரும்பு rng;
}

நிலையான தருக்க vec4_lt_unsigned(மாறிலி vvp_vector4_t&a, மாறிலி vvp_vector4_t&b)
{
      எனில் (a.eeq(b)) திரும்பு பொய்;
      vvp_bit4_t gt = compare_gtge(b, a, BIT4_1);
      எனில் (gt == BIT4_X) திரும்பு பொய்;
      திரும்பு gt == BIT4_1;
}

} // namespace

vvp_darray::~vvp_darray()
{
}

வெறுமை vvp_darray::reverse_elems(வெறுமை)
{
      cerr << "XXXX reverse_elems() not implemented for "
           << typeid(*இது).name() << endl;
}

வெறுமை vvp_darray::sort_elems(தருக்க /*ascending*/)
{
      cerr << "XXXX sort_elems() not implemented for "
           << typeid(*இது).name() << endl;
}

வெறுமை vvp_darray::shuffle_elems(வெறுமை)
{
      cerr << "XXXX shuffle_elems() not implemented for "
           << typeid(*இது).name() << endl;
}

வெறுமை vvp_darray::set_word(unsigned, மாறிலி vvp_vector4_t&)
{
      cerr << "XXXX set_word(vvp_vector4_t) not implemented for " << typeid(*இது).name() << endl;
}

வெறுமை vvp_darray::set_word(unsigned, இரட்டை)
{
      cerr << "XXXX set_word(double) not implemented for " << typeid(*இது).name() << endl;
}

வெறுமை vvp_darray::set_word(unsigned, மாறிலி string&)
{
      cerr << "XXXX set_word(string) not implemented for " << typeid(*இது).name() << endl;
}

வெறுமை vvp_darray::set_word(unsigned, மாறிலி vvp_object_t&)
{
      cerr << "XXXX set_word(vvp_object_t) not implemented for " << typeid(*இது).name() << endl;
}

வெறுமை vvp_darray::get_word(unsigned, vvp_vector4_t&)
{
      cerr << "XXXX get_word(vvp_vector4_t) not implemented for " << typeid(*இது).name() << endl;
}

வெறுமை vvp_darray::get_word(unsigned, இரட்டை&)
{
      cerr << "XXXX get_word(double) not implemented for " << typeid(*இது).name() << endl;
}

வெறுமை vvp_darray::get_word(unsigned, string&)
{
      cerr << "XXXX get_word(string) not implemented for " << typeid(*இது).name() << endl;
}

வெறுமை vvp_darray::get_word(unsigned, vvp_object_t&)
{
      cerr << "XXXX get_word(vvp_object_t) not implemented for " << typeid(*இது).name() << endl;
}

vvp_vector4_t vvp_darray::get_bitstream(தருக்க)
{
      cerr << "XXXX get_bitstream() not implemented for " << typeid(*இது).name() << endl;
      திரும்பு vvp_vector4_t();
}

template <வகுப்பு TYPE> vvp_darray_atom<TYPE>::~vvp_darray_atom()
{
}

template <வகுப்பு TYPE> size_t vvp_darray_atom<TYPE>::get_size() மாறிலி
{
      திரும்பு array_.size();
}

template <வகுப்பு TYPE> வெறுமை vvp_darray_atom<TYPE>::reverse_elems(வெறுமை)
{
      size_t n = array_.size();
      க்கு (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

template <வகுப்பு TYPE> வெறுமை vvp_darray_atom<TYPE>::sort_elems(தருக்க ascending)
{
      எனில் (ascending) std::sort(array_.begin(), array_.end());
      இல்லையெனில் std::sort(array_.begin(), array_.end(), std::greater<TYPE>());
}

template <வகுப்பு TYPE> வெறுமை vvp_darray_atom<TYPE>::shuffle_elems(வெறுமை)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

template <வகுப்பு TYPE> வெறுமை vvp_darray_atom<TYPE>::set_word(unsigned adr, மாறிலி vvp_vector4_t&value)
{
      எனில் (adr >= array_.size())
	    திரும்பு;
      TYPE tmp;
      vector4_to_மதிப்பு(value, tmp, உண்மை, பொய்);
      array_[adr] = tmp;
}

template <வகுப்பு TYPE> வெறுமை vvp_darray_atom<TYPE>::get_word(unsigned adr, vvp_vector4_t&value)
{
      எனில் (adr >= array_.size()) {
	    value = vvp_vector4_t(8*அளவு(TYPE), BIT4_X);
	    திரும்பு;
      }

      TYPE word = array_[adr];
      vvp_vector4_t tmp (8*அளவு(TYPE), BIT4_0);
      க்கு (unsigned idx = 0 ; idx < tmp.size() ; idx += 1) {
	    எனில் (word&1) tmp.set_bit(idx, BIT4_1);
	    word >>= 1;
      }
      value = tmp;
}

template <வகுப்பு TYPE> வெறுமை vvp_darray_atom<TYPE>::shallow_copy(மாறிலி vvp_object*obj)
{
      மாறிலி vvp_darray_atom<TYPE>*that = dynamic_cast<மாறிலி vvp_darray_atom<TYPE>*>(obj);
      ப்ரதிஜ்ஞா(that);

      unsigned இலக்கம்_பொருட்கள் = min(array_.size(), that->array_.size());
      க்கு (unsigned idx = 0 ; idx < இலக்கம்_பொருட்கள் ; idx += 1)
	    array_[idx] = that->array_[idx];
}

template <வகுப்பு TYPE> vvp_object* vvp_darray_atom<TYPE>::duplicate(வெறுமை) மாறிலி
{
      vvp_darray_atom<TYPE>*that = புதிய vvp_darray_atom<TYPE>(array_.size());
      க்கு (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      திரும்பு that;
}

template <வகுப்பு TYPE> vvp_vector4_t vvp_darray_atom<TYPE>::get_bitstream(தருக்க)
{
      மாறிலி unsigned word_wid = அளவு(TYPE) * 8;

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      போது (vdx > 0) {
            TYPE word = array_[adx++];
            vdx -= word_wid;
            க்கு (unsigned bdx = 0; bdx < word_wid; bdx += 1) {
                  எனில் (word & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word >>= 1;
            }
      }

      திரும்பு vec;
}

template வகுப்பு vvp_darray_atom<uint8_t>;
template வகுப்பு vvp_darray_atom<uint16_t>;
template வகுப்பு vvp_darray_atom<uint32_t>;
template வகுப்பு vvp_darray_atom<uint64_t>;
template வகுப்பு vvp_darray_atom<int8_t>;
template வகுப்பு vvp_darray_atom<int16_t>;
template வகுப்பு vvp_darray_atom<int32_t>;
template வகுப்பு vvp_darray_atom<int64_t>;

vvp_darray_vec4::~vvp_darray_vec4()
{
}

size_t vvp_darray_vec4::get_size(வெறுமை) மாறிலி
{
      திரும்பு array_.size();
}

வெறுமை vvp_darray_vec4::set_word(unsigned adr, மாறிலி vvp_vector4_t&value)
{
      எனில் (adr >= array_.size()) திரும்பு;
      ப்ரதிஜ்ஞா(value.size() == word_wid_);
      array_[adr] = value;
}

வெறுமை vvp_darray_vec4::get_word(unsigned adr, vvp_vector4_t&value)
{
	/*
	 * Return an undefined value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      எனில் ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_X);
	    திரும்பு;
      }
      value = array_[adr];
      ப்ரதிஜ்ஞா(value.size() == word_wid_);
}

வெறுமை vvp_darray_vec4::shallow_copy(மாறிலி vvp_object*obj)
{
      மாறிலி vvp_darray_vec4*that = dynamic_cast<மாறிலி vvp_darray_vec4*>(obj);
      ப்ரதிஜ்ஞா(that);

      unsigned இலக்கம்_பொருட்கள் = min(array_.size(), that->array_.size());
      க்கு (unsigned idx = 0 ; idx < இலக்கம்_பொருட்கள் ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_object* vvp_darray_vec4::duplicate(வெறுமை) மாறிலி
{
      vvp_darray_vec4*that = புதிய vvp_darray_vec4(array_.size(), word_wid_);

      க்கு (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      திரும்பு that;
}

vvp_vector4_t vvp_darray_vec4::get_bitstream(தருக்க as_vec4)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      போது (vdx > 0) {
            vdx -= word_wid_;
            க்கு (unsigned bdx = 0; bdx < word_wid_; bdx += 1) {
                  vvp_bit4_t bit = array_[adx].value(bdx);
                  எனில் (as_vec4 || (bit == BIT4_1))
                        vec.set_bit(vdx+bdx, bit);
            }
            adx++;
      }

      திரும்பு vec;
}

வெறுமை vvp_darray_vec4::reverse_elems(வெறுமை)
{
      size_t n = array_.size();
      க்கு (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

வெறுமை vvp_darray_vec4::sort_elems(தருக்க ascending)
{
      எனில் (ascending) {
	    std::sort(array_.begin(), array_.end(), vec4_lt_unsigned);
      } இல்லையெனில் {
	    std::sort(array_.begin(), array_.end(),
		      [](மாறிலி vvp_vector4_t&a, மாறிலி vvp_vector4_t&b) {
			    திரும்பு vec4_lt_unsigned(b, a);
		      });
      }
}

வெறுமை vvp_darray_vec4::shuffle_elems(வெறுமை)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

வெறுமை vvp_darray_vec2::reverse_elems(வெறுமை)
{
      size_t n = array_.size();
      க்கு (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

வெறுமை vvp_darray_vec2::sort_elems(தருக்க ascending)
{
      எனில் (ascending) {
	    std::sort(array_.begin(), array_.end());
      } இல்லையெனில் {
	    std::sort(array_.begin(), array_.end(),
		      [](மாறிலி vvp_vector2_t&a, மாறிலி vvp_vector2_t&b) {
			    திரும்பு b < a;
		      });
      }
}

வெறுமை vvp_darray_vec2::shuffle_elems(வெறுமை)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_darray_vec2::~vvp_darray_vec2()
{
}

size_t vvp_darray_vec2::get_size(வெறுமை) மாறிலி
{
      திரும்பு array_.size();
}

வெறுமை vvp_darray_vec2::set_word(unsigned adr, மாறிலி vvp_vector4_t&value)
{
      எனில் (adr >= array_.size()) திரும்பு;
      ப்ரதிஜ்ஞா(value.size() == word_wid_);
      array_[adr] = value;
}

வெறுமை vvp_darray_vec2::get_word(unsigned adr, vvp_vector4_t&value)
{
	/*
	 * Return a zero value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      எனில் ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_0);
	    திரும்பு;
      }
      ப்ரதிஜ்ஞா(array_[adr].size() == word_wid_);
      value.resize(word_wid_);
      க்கு (unsigned idx = 0; idx < word_wid_; idx += 1) {
	    value.set_bit(idx, array_[adr].value4(idx));
      }
}

வெறுமை vvp_darray_vec2::shallow_copy(மாறிலி vvp_object*obj)
{
      மாறிலி vvp_darray_vec2*that = dynamic_cast<மாறிலி vvp_darray_vec2*>(obj);
      ப்ரதிஜ்ஞா(that);

      unsigned இலக்கம்_பொருட்கள் = min(array_.size(), that->array_.size());
      க்கு (unsigned idx = 0 ; idx < இலக்கம்_பொருட்கள் ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_vector4_t vvp_darray_vec2::get_bitstream(தருக்க)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      போது (vdx > 0) {
            vdx -= word_wid_;
            க்கு (unsigned bdx = 0; bdx < word_wid_; bdx += 1) {
                  எனில் (array_[adx].value(bdx))
                        vec.set_bit(vdx+bdx, BIT4_1);
            }
            adx++;
      }

      திரும்பு vec;
}

vvp_darray_object::~vvp_darray_object()
{
}

size_t vvp_darray_object::get_size() மாறிலி
{
      திரும்பு array_.size();
}

வெறுமை vvp_darray_object::set_word(unsigned adr, மாறிலி vvp_object_t&value)
{
      எனில் (adr >= array_.size())
	    திரும்பு;
      array_[adr] = value;
}

வெறுமை vvp_darray_object::get_word(unsigned adr, vvp_object_t&value)
{
      எனில் (adr >= array_.size()) {
	    value = vvp_object_t();
	    திரும்பு;
      }

      value = array_[adr];
}

வெறுமை vvp_darray_object::shallow_copy(மாறிலி vvp_object*obj)
{
      மாறிலி vvp_darray_object*that = dynamic_cast<மாறிலி vvp_darray_object*>(obj);
      ப்ரதிஜ்ஞா(that);

      unsigned இலக்கம்_பொருட்கள் = min(array_.size(), that->array_.size());
      க்கு (unsigned idx = 0 ; idx < இலக்கம்_பொருட்கள் ; idx += 1)
	    array_[idx] = that->array_[idx];
}

வெறுமை vvp_darray_object::reverse_elems(வெறுமை)
{
      size_t n = array_.size();
      க்கு (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

வெறுமை vvp_darray_object::sort_elems(தருக்க ascending)
{
      எனில் (ascending) {
	    std::sort(array_.begin(), array_.end(),
		      [](மாறிலி vvp_object_t&a, மாறிலி vvp_object_t&b) {
			    திரும்பு a.sort_order_சாவி() < b.sort_order_சாவி();
		      });
      } இல்லையெனில் {
	    std::sort(array_.begin(), array_.end(),
		      [](மாறிலி vvp_object_t&a, மாறிலி vvp_object_t&b) {
			    திரும்பு a.sort_order_சாவி() > b.sort_order_சாவி();
		      });
      }
}

வெறுமை vvp_darray_object::shuffle_elems(வெறுமை)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_darray_real::~vvp_darray_real()
{
}

size_t vvp_darray_real::get_size() மாறிலி
{
      திரும்பு array_.size();
}

வெறுமை vvp_darray_real::set_word(unsigned adr, இரட்டை value)
{
      எனில் (adr >= array_.size())
	    திரும்பு;
      array_[adr] = value;
}

வெறுமை vvp_darray_real::get_word(unsigned adr, இரட்டை&value)
{
      எனில் (adr >= array_.size()) {
	    value = 0.0;
	    திரும்பு;
      }

      value = array_[adr];
}

வெறுமை vvp_darray_real::shallow_copy(மாறிலி vvp_object*obj)
{
      மாறிலி vvp_darray_real*that = dynamic_cast<மாறிலி vvp_darray_real*>(obj);
      ப்ரதிஜ்ஞா(that);

      unsigned இலக்கம்_பொருட்கள் = min(array_.size(), that->array_.size());
      க்கு (unsigned idx = 0 ; idx < இலக்கம்_பொருட்கள் ; idx += 1)
	    array_[idx] = that->array_[idx];
}

வெறுமை vvp_darray_real::reverse_elems(வெறுமை)
{
      size_t n = array_.size();
      க்கு (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

வெறுமை vvp_darray_real::sort_elems(தருக்க ascending)
{
      எனில் (ascending) std::sort(array_.begin(), array_.end());
      இல்லையெனில் std::sort(array_.begin(), array_.end(), std::greater<இரட்டை>());
}

வெறுமை vvp_darray_real::shuffle_elems(வெறுமை)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_real::duplicate(வெறுமை) மாறிலி
{
      vvp_darray_real*that = புதிய vvp_darray_real(array_.size());

      க்கு (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      திரும்பு that;
}

vvp_vector4_t vvp_darray_real::get_bitstream(தருக்க)
{
      மாறிலி unsigned word_wid = அளவு(இரட்டை) * 8;
      ப்ரதிஜ்ஞா(word_wid == 64);

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      போது (vdx > 0) {
            ஒன்றிப்பு {
                இரட்டை   value;
                uint64_t bits;
            } word;
            word.value = array_[adx++];
            vdx -= word_wid;
            க்கு (unsigned bdx = 0; bdx < word_wid; bdx += 1) {
                  எனில் (word.bits & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word.bits >>= 1;
            }
      }

      திரும்பு vec;
}

vvp_darray_string::~vvp_darray_string()
{
}

size_t vvp_darray_string::get_size() மாறிலி
{
      திரும்பு array_.size();
}

வெறுமை vvp_darray_string::set_word(unsigned adr, மாறிலி string&value)
{
      எனில் (adr >= array_.size())
	    திரும்பு;
      array_[adr] = value;
}

வெறுமை vvp_darray_string::get_word(unsigned adr, string&value)
{
      எனில் (adr >= array_.size()) {
	    value = "";
	    திரும்பு;
      }

      value = array_[adr];
}

வெறுமை vvp_darray_string::shallow_copy(மாறிலி vvp_object*obj)
{
      மாறிலி vvp_darray_string*that = dynamic_cast<மாறிலி vvp_darray_string*>(obj);
      ப்ரதிஜ்ஞா(that);

      unsigned இலக்கம்_பொருட்கள் = min(array_.size(), that->array_.size());
      க்கு (unsigned idx = 0 ; idx < இலக்கம்_பொருட்கள் ; idx += 1)
	    array_[idx] = that->array_[idx];
}

வெறுமை vvp_darray_string::reverse_elems(வெறுமை)
{
      size_t n = array_.size();
      க்கு (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

வெறுமை vvp_darray_string::sort_elems(தருக்க ascending)
{
      எனில் (ascending) std::sort(array_.begin(), array_.end());
      இல்லையெனில் std::sort(array_.begin(), array_.end(), std::greater<string>());
}

வெறுமை vvp_darray_string::shuffle_elems(வெறுமை)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_string::duplicate(வெறுமை) மாறிலி
{
      vvp_darray_string*that = புதிய vvp_darray_string(array_.size());

      க்கு (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      திரும்பு that;
}

vvp_queue::~vvp_queue()
{
}

வெறுமை vvp_queue::copy_elems(vvp_object_t, unsigned)
{
      cerr << "Sorry: copy_elems() not implemented for " << typeid(*இது).name() << endl;
}

வெறுமை vvp_queue::set_word_max(unsigned, மாறிலி vvp_vector4_t&, unsigned)
{
      cerr << "XXXX set_word_max(vvp_vector4_t) not implemented for " << typeid(*இது).name() << endl;
}

வெறுமை vvp_queue::insert(unsigned, மாறிலி vvp_vector4_t&, unsigned)
{
      cerr << "XXXX insert(vvp_vector4_t) not implemented for " << typeid(*இது).name() << endl;
}

வெறுமை vvp_queue::push_back(மாறிலி vvp_vector4_t&, unsigned)
{
      cerr << "XXXX push_back(vvp_vector4_t) not implemented for " << typeid(*இது).name() << endl;
}

வெறுமை vvp_queue::push_front(மாறிலி vvp_vector4_t&, unsigned)
{
      cerr << "XXXX push_front(vvp_vector4_t) not implemented for " << typeid(*இது).name() << endl;
}

வெறுமை vvp_queue::set_word_max(unsigned, இரட்டை, unsigned)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*இது).name() << endl;
}

வெறுமை vvp_queue::insert(unsigned, இரட்டை, unsigned)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*இது).name() << endl;
}

வெறுமை vvp_queue::push_back(இரட்டை, unsigned)
{
      cerr << "XXXX push_back(double) not implemented for " << typeid(*இது).name() << endl;
}

வெறுமை vvp_queue::push_front(இரட்டை, unsigned)
{
      cerr << "XXXX push_front(double) not implemented for " << typeid(*இது).name() << endl;
}

வெறுமை vvp_queue::set_word_max(unsigned, மாறிலி string&, unsigned)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*இது).name() << endl;
}

வெறுமை vvp_queue::insert(unsigned, மாறிலி string&, unsigned)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*இது).name() << endl;
}

வெறுமை vvp_queue::push_back(மாறிலி string&, unsigned)
{
      cerr << "XXXX push_back(string) not implemented for " << typeid(*இது).name() << endl;
}

வெறுமை vvp_queue::push_front(மாறிலி string&, unsigned)
{
      cerr << "XXXX push_front(string) not implemented for " << typeid(*இது).name() << endl;
}

/*
 * Helper functions used while copying multiple elements into a queue.
 */
நிலையான வெறுமை print_copy_is_too_big(size_t src_size, unsigned max_size, மாறிலி string&qtype)
{
      cerr << get_fileline()
           << "Warning: queue<" << qtype << "> is bounded to have at most "
           << max_size << " elements, source has " << src_size << " elements." << endl;
}

நிலையான வெறுமை print_copy_is_too_big(இரட்டை&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "real");
}

நிலையான வெறுமை print_copy_is_too_big(string&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "string");
}

நிலையான வெறுமை print_copy_is_too_big(vvp_vector4_t&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "vector");
}

template <typename ELEM, வகுப்பு QTYPE, வகுப்பு SRC_TYPE>
நிலையான வெறுமை copy_elements(QTYPE*queue, SRC_TYPE*src, unsigned max_size)
{
      size_t src_size = src->get_size();
      எனில் ((max_size != 0) && (src_size > max_size)) {
	    ELEM tmp;
	    print_copy_is_too_big(tmp, src_size, max_size);
      }
      unsigned copy_size = ((src_size < max_size) ||
                            (max_size == 0)) ? src_size : max_size;
      எனில் (copy_size < queue->get_size())
	    queue->erase_tail(copy_size);
      க்கு (unsigned idx=0; idx < copy_size; ++idx) {
	    ELEM value;
	    src->get_word(idx, value);
	    queue->set_word_max(idx, value, max_size);
      }
}

வெறுமை vvp_queue_real::copy_elems(vvp_object_t src, unsigned max_size)
{
      எனில் (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<இரட்டை, vvp_queue_real, vvp_queue>(இது, src_queue, max_size);
      இல்லையெனில் எனில் (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<இரட்டை, vvp_queue_real, vvp_darray>(இது, src_darray, max_size);
      இல்லையெனில்
	    cerr << get_fileline() << "Sorry: cannot copy object to real queue." << endl;
}

வெறுமை vvp_queue_real::set_word_max(unsigned adr, இரட்டை value, unsigned max_size)
{
      எனில் (adr == queue.size())
	    எனில் (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    இல்லையெனில்
		  cerr << get_fileline()
		       << "Warning: assigning to queue<real>[" << adr << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      இல்லையெனில்
	    set_word(adr, value);
}

வெறுமை vvp_queue_real::set_word(unsigned adr, இரட்டை value)
{
      எனில் (adr < queue.size())
	    queue[adr] = value;
      இல்லையெனில்
	    cerr << get_fileline()
	         << "Warning: assigning to queue<real>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
}

வெறுமை vvp_queue_real::get_word(unsigned adr, இரட்டை&value)
{
      எனில் (adr >= queue.size())
	    value = 0.0;
      இல்லையெனில்
	    value = queue[adr];
}

வெறுமை vvp_queue_real::insert(unsigned idx, இரட்டை value, unsigned max_size)
{
	// Inserting past the end of the queue
      எனில் (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<real>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
	// Inserting at the end
      இல்லையெனில் எனில் (idx == queue.size())
	    எனில் (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    இல்லையெனில்
		  cerr << get_fileline()
		       << "Warning: inserting to queue<real>[" << idx << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      இல்லையெனில்  {
	    எனில் (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<real> ["
		       << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

வெறுமை vvp_queue_real::push_back(இரட்டை value, unsigned max_size)
{
      எனில் (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      இல்லையெனில்
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<real> ["
	         << max_size << "]." << endl;
}

வெறுமை vvp_queue_real::push_front(இரட்டை value, unsigned max_size)
{
      எனில் (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<real> ["
	         << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

வெறுமை vvp_queue_real::erase(unsigned idx)
{
      ப்ரதிஜ்ஞா(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

வெறுமை vvp_queue_real::erase_tail(unsigned idx)
{
      ப்ரதிஜ்ஞா(queue.size() >= idx);
      எனில் (queue.size() > idx)
	    queue.resize(idx);
}

வெறுமை vvp_queue_string::copy_elems(vvp_object_t src, unsigned max_size)
{
      எனில் (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<string, vvp_queue_string, vvp_queue>(இது, src_queue, max_size);
      இல்லையெனில் எனில் (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<string, vvp_queue_string, vvp_darray>(இது, src_darray, max_size);
      இல்லையெனில்
	    cerr << get_fileline() << "Sorry: cannot copy object to string queue." << endl;
}

வெறுமை vvp_queue_string::set_word_max(unsigned adr, மாறிலி string&value, unsigned max_size)
{
      எனில் (adr == queue.size())
	    எனில் (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    இல்லையெனில்
		  cerr << get_fileline()
		       << "Warning: assigning to queue<string>[" << adr << "] is"
		          " outside bound (" << max_size << "). \"" << value
		       << "\" was not added." << endl;
      இல்லையெனில்
	    set_word(adr, value);
}

வெறுமை vvp_queue_string::set_word(unsigned adr, மாறிலி string&value)
{
      எனில் (adr < queue.size())
	    queue[adr] = value;
      இல்லையெனில்
	    cerr << get_fileline()
	         << "Warning: assigning to queue<string>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << endl;
}

வெறுமை vvp_queue_string::get_word(unsigned adr, string&value)
{
      எனில் (adr >= queue.size())
	    value = "";
      இல்லையெனில்
	    value = queue[adr];
}

வெறுமை vvp_queue_string::insert(unsigned idx, மாறிலி string&value, unsigned max_size)
{
	// Inserting past the end of the queue
      எனில் (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<string>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << endl;
	// Inserting at the end
      இல்லையெனில் எனில் (idx == queue.size())
	    எனில் (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    இல்லையெனில்
		  cerr << get_fileline()
		       << "Warning: inserting to queue<string>[" << idx << "] is"
		          " outside bound (" << max_size << "). \"" << value
		       << "\" was not added." << endl;
      இல்லையெனில்  {
	    எனில் (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", \"" << value << "\") removed \""
		       << queue.back() << "\" from already full bounded queue<string> ["
		       << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

வெறுமை vvp_queue_string::push_back(மாறிலி string&value, unsigned max_size)
{
      எனில் (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      இல்லையெனில்
	    cerr << get_fileline()
	         << "Warning: push_back(\"" << value
	         << "\") skipped for already full bounded queue<string> ["
	         << max_size << "]." << endl;
}

வெறுமை vvp_queue_string::push_front(மாறிலி string&value, unsigned max_size)
{
      எனில் (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(\"" << value << "\") removed \""
	         << queue.back() << "\" from already full bounded queue<string> ["
	         << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

வெறுமை vvp_queue_string::erase(unsigned idx)
{
      ப்ரதிஜ்ஞா(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

வெறுமை vvp_queue_string::erase_tail(unsigned idx)
{
      ப்ரதிஜ்ஞா(queue.size() >= idx);
      எனில் (queue.size() > idx)
	    queue.resize(idx);
}

வெறுமை vvp_queue_real::reverse_elems(வெறுமை)
{
      std::reverse(queue.begin(), queue.end());
}

வெறுமை vvp_queue_string::reverse_elems(வெறுமை)
{
      std::reverse(queue.begin(), queue.end());
}

வெறுமை vvp_queue_real::sort_elems(தருக்க ascending)
{
      எனில் (ascending) std::sort(queue.begin(), queue.end());
      இல்லையெனில் std::sort(queue.begin(), queue.end(), std::greater<இரட்டை>());
}

வெறுமை vvp_queue_real::shuffle_elems(வெறுமை)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

வெறுமை vvp_queue_string::sort_elems(தருக்க ascending)
{
      எனில் (ascending) std::sort(queue.begin(), queue.end());
      இல்லையெனில் std::sort(queue.begin(), queue.end(), std::greater<string>());
}

வெறுமை vvp_queue_string::shuffle_elems(வெறுமை)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

வெறுமை vvp_queue_vec4::copy_elems(vvp_object_t src, unsigned max_size)
{
      எனில் (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_queue>(இது, src_queue, max_size);
      இல்லையெனில் எனில் (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_darray>(இது, src_darray, max_size);
      இல்லையெனில்
	    cerr << get_fileline() << "Sorry: cannot copy object to vector queue." << endl;
}

வெறுமை vvp_queue_vec4::set_word_max(unsigned adr, மாறிலி vvp_vector4_t&value, unsigned max_size)
{
      எனில் (adr == queue.size())
	    எனில் (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    இல்லையெனில்
		  cerr << get_fileline()
		       << "Warning: assigning to queue<vector>[" << adr << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      இல்லையெனில்
	    set_word(adr, value);
}

வெறுமை vvp_queue_vec4::set_word(unsigned adr, மாறிலி vvp_vector4_t&value)
{
      எனில் (adr < queue.size())
	    queue[adr] = value;
      இல்லையெனில்
	    cerr << get_fileline()
	         << "Warning: assigning to queue<vector>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
}

வெறுமை vvp_queue_vec4::get_word(unsigned adr, vvp_vector4_t&value)
{
      எனில் (adr >= queue.size())
	    value = vvp_vector4_t(queue[0].size());
      இல்லையெனில்
	    value = queue[adr];
}

வெறுமை vvp_queue_vec4::insert(unsigned idx, மாறிலி vvp_vector4_t&value, unsigned max_size)
{
	// Inserting past the end of the queue
      எனில் (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<vector[" << value.size()
	         << "]>[" << idx << "] is outside of size (" << queue.size()
	         << "). " << value << " was not added." << endl;
	// Inserting at the end
      இல்லையெனில் எனில் (idx == queue.size())
	    எனில் (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    இல்லையெனில்
		  cerr << get_fileline()
		       << "Warning: inserting to queue<vector[" << value.size()
		       << "]>[" << idx << "] is outside bound (" << max_size
		       << "). " << value << " was not added." << endl;
      இல்லையெனில்  {
	    எனில் (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<vector["
		       << value.size() << "]> [" << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

வெறுமை vvp_queue_vec4::push_back(மாறிலி vvp_vector4_t&value, unsigned max_size)
{
      எனில் (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      இல்லையெனில்
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<vector["
	         << value.size() << "]> [" << max_size << "]." << endl;
}

வெறுமை vvp_queue_vec4::push_front(மாறிலி vvp_vector4_t&value, unsigned max_size)
{
      எனில் (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<vector["
	         << value.size() << "]> [" << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

வெறுமை vvp_queue_vec4::erase(unsigned idx)
{
      ப்ரதிஜ்ஞா(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

வெறுமை vvp_queue_vec4::erase_tail(unsigned idx)
{
      ப்ரதிஜ்ஞா(queue.size() >= idx);
      எனில் (queue.size() > idx)
	    queue.resize(idx);
}

வெறுமை vvp_queue_vec4::reverse_elems(வெறுமை)
{
      std::reverse(queue.begin(), queue.end());
}

வெறுமை vvp_queue_vec4::sort_elems(தருக்க ascending)
{
      எனில் (ascending) {
	    std::sort(queue.begin(), queue.end(), vec4_lt_unsigned);
      } இல்லையெனில் {
	    std::sort(queue.begin(), queue.end(),
		      [](மாறிலி vvp_vector4_t&a, மாறிலி vvp_vector4_t&b) {
			    திரும்பு vec4_lt_unsigned(b, a);
		      });
      }
}

வெறுமை vvp_queue_vec4::shuffle_elems(வெறுமை)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}
