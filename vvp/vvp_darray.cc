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

using नामाकाश std;

नामाकाश {

स्थिर std::mt19937& shuffle_rng(शून्य)
{
      स्थिर std::mt19937 rng(std::random_device{}());
      फर्काऊ rng;
}

स्थिर तार्किक vec4_lt_unsigned(अचर vvp_vector4_t&a, अचर vvp_vector4_t&b)
{
      यदि (a.eeq(b)) फर्काऊ असत्य;
      vvp_bit4_t gt = compare_gtge(b, a, BIT4_1);
      यदि (gt == BIT4_X) फर्काऊ असत्य;
      फर्काऊ gt == BIT4_1;
}

} // namespace

vvp_darray::~vvp_darray()
{
}

शून्य vvp_darray::reverse_elems(शून्य)
{
      cerr << "XXXX reverse_elems() not implemented for "
           << typeid(*एतत्).name() << endl;
}

शून्य vvp_darray::sort_elems(तार्किक /*ascending*/)
{
      cerr << "XXXX sort_elems() not implemented for "
           << typeid(*एतत्).name() << endl;
}

शून्य vvp_darray::shuffle_elems(शून्य)
{
      cerr << "XXXX shuffle_elems() not implemented for "
           << typeid(*एतत्).name() << endl;
}

शून्य vvp_darray::set_word(अचिन्हित, अचर vvp_vector4_t&)
{
      cerr << "XXXX set_word(vvp_vector4_t) not implemented for " << typeid(*एतत्).name() << endl;
}

शून्य vvp_darray::set_word(अचिन्हित, द्विगुण)
{
      cerr << "XXXX set_word(double) not implemented for " << typeid(*एतत्).name() << endl;
}

शून्य vvp_darray::set_word(अचिन्हित, अचर string&)
{
      cerr << "XXXX set_word(string) not implemented for " << typeid(*एतत्).name() << endl;
}

शून्य vvp_darray::set_word(अचिन्हित, अचर vvp_object_t&)
{
      cerr << "XXXX set_word(vvp_object_t) not implemented for " << typeid(*एतत्).name() << endl;
}

शून्य vvp_darray::get_word(अचिन्हित, vvp_vector4_t&)
{
      cerr << "XXXX get_word(vvp_vector4_t) not implemented for " << typeid(*एतत्).name() << endl;
}

शून्य vvp_darray::get_word(अचिन्हित, द्विगुण&)
{
      cerr << "XXXX get_word(double) not implemented for " << typeid(*एतत्).name() << endl;
}

शून्य vvp_darray::get_word(अचिन्हित, string&)
{
      cerr << "XXXX get_word(string) not implemented for " << typeid(*एतत्).name() << endl;
}

शून्य vvp_darray::get_word(अचिन्हित, vvp_object_t&)
{
      cerr << "XXXX get_word(vvp_object_t) not implemented for " << typeid(*एतत्).name() << endl;
}

vvp_vector4_t vvp_darray::get_bitstream(तार्किक)
{
      cerr << "XXXX get_bitstream() not implemented for " << typeid(*एतत्).name() << endl;
      फर्काऊ vvp_vector4_t();
}

ढाँचा <वर्ग TYPE> vvp_darray_atom<TYPE>::~vvp_darray_atom()
{
}

ढाँचा <वर्ग TYPE> माप_प्रकार vvp_darray_atom<TYPE>::get_size() अचर
{
      फर्काऊ array_.size();
}

ढाँचा <वर्ग TYPE> शून्य vvp_darray_atom<TYPE>::reverse_elems(शून्य)
{
      माप_प्रकार n = array_.size();
      लागि (माप_प्रकार idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ढाँचा <वर्ग TYPE> शून्य vvp_darray_atom<TYPE>::sort_elems(तार्किक ascending)
{
      यदि (ascending) std::sort(array_.begin(), array_.end());
      अन्यथा std::sort(array_.begin(), array_.end(), std::greater<TYPE>());
}

ढाँचा <वर्ग TYPE> शून्य vvp_darray_atom<TYPE>::shuffle_elems(शून्य)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

ढाँचा <वर्ग TYPE> शून्य vvp_darray_atom<TYPE>::set_word(अचिन्हित adr, अचर vvp_vector4_t&value)
{
      यदि (adr >= array_.size())
	    फर्काऊ;
      TYPE tmp;
      vector4_to_मान(value, tmp, सत्य, असत्य);
      array_[adr] = tmp;
}

ढाँचा <वर्ग TYPE> शून्य vvp_darray_atom<TYPE>::get_word(अचिन्हित adr, vvp_vector4_t&value)
{
      यदि (adr >= array_.size()) {
	    value = vvp_vector4_t(8*परिमाण(TYPE), BIT4_X);
	    फर्काऊ;
      }

      TYPE word = array_[adr];
      vvp_vector4_t tmp (8*परिमाण(TYPE), BIT4_0);
      लागि (अचिन्हित idx = 0 ; idx < tmp.size() ; idx += 1) {
	    यदि (word&1) tmp.set_bit(idx, BIT4_1);
	    word >>= 1;
      }
      value = tmp;
}

ढाँचा <वर्ग TYPE> शून्य vvp_darray_atom<TYPE>::shallow_copy(अचर vvp_object*obj)
{
      अचर vvp_darray_atom<TYPE>*that = dynamic_cast<अचर vvp_darray_atom<TYPE>*>(obj);
      प्रतिज्ञा(that);

      अचिन्हित अङ्क_वस्तुहरू = min(array_.size(), that->array_.size());
      लागि (अचिन्हित idx = 0 ; idx < अङ्क_वस्तुहरू ; idx += 1)
	    array_[idx] = that->array_[idx];
}

ढाँचा <वर्ग TYPE> vvp_object* vvp_darray_atom<TYPE>::duplicate(शून्य) अचर
{
      vvp_darray_atom<TYPE>*that = नूतन vvp_darray_atom<TYPE>(array_.size());
      लागि (माप_प्रकार idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      फर्काऊ that;
}

ढाँचा <वर्ग TYPE> vvp_vector4_t vvp_darray_atom<TYPE>::get_bitstream(तार्किक)
{
      अचर अचिन्हित word_wid = परिमाण(TYPE) * 8;

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      अचिन्हित adx = 0;
      अचिन्हित vdx = vec.size();
      जबसम्म (vdx > 0) {
            TYPE word = array_[adx++];
            vdx -= word_wid;
            लागि (अचिन्हित bdx = 0; bdx < word_wid; bdx += 1) {
                  यदि (word & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word >>= 1;
            }
      }

      फर्काऊ vec;
}

ढाँचा वर्ग vvp_darray_atom<uint8_t>;
ढाँचा वर्ग vvp_darray_atom<uint16_t>;
ढाँचा वर्ग vvp_darray_atom<uint32_t>;
ढाँचा वर्ग vvp_darray_atom<uint64_t>;
ढाँचा वर्ग vvp_darray_atom<int8_t>;
ढाँचा वर्ग vvp_darray_atom<int16_t>;
ढाँचा वर्ग vvp_darray_atom<int32_t>;
ढाँचा वर्ग vvp_darray_atom<int64_t>;

vvp_darray_vec4::~vvp_darray_vec4()
{
}

माप_प्रकार vvp_darray_vec4::get_size(शून्य) अचर
{
      फर्काऊ array_.size();
}

शून्य vvp_darray_vec4::set_word(अचिन्हित adr, अचर vvp_vector4_t&value)
{
      यदि (adr >= array_.size()) फर्काऊ;
      प्रतिज्ञा(value.size() == word_wid_);
      array_[adr] = value;
}

शून्य vvp_darray_vec4::get_word(अचिन्हित adr, vvp_vector4_t&value)
{
	/*
	 * Return an undefined value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      यदि ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_X);
	    फर्काऊ;
      }
      value = array_[adr];
      प्रतिज्ञा(value.size() == word_wid_);
}

शून्य vvp_darray_vec4::shallow_copy(अचर vvp_object*obj)
{
      अचर vvp_darray_vec4*that = dynamic_cast<अचर vvp_darray_vec4*>(obj);
      प्रतिज्ञा(that);

      अचिन्हित अङ्क_वस्तुहरू = min(array_.size(), that->array_.size());
      लागि (अचिन्हित idx = 0 ; idx < अङ्क_वस्तुहरू ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_object* vvp_darray_vec4::duplicate(शून्य) अचर
{
      vvp_darray_vec4*that = नूतन vvp_darray_vec4(array_.size(), word_wid_);

      लागि (माप_प्रकार idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      फर्काऊ that;
}

vvp_vector4_t vvp_darray_vec4::get_bitstream(तार्किक as_vec4)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      अचिन्हित adx = 0;
      अचिन्हित vdx = vec.size();
      जबसम्म (vdx > 0) {
            vdx -= word_wid_;
            लागि (अचिन्हित bdx = 0; bdx < word_wid_; bdx += 1) {
                  vvp_bit4_t bit = array_[adx].value(bdx);
                  यदि (as_vec4 || (bit == BIT4_1))
                        vec.set_bit(vdx+bdx, bit);
            }
            adx++;
      }

      फर्काऊ vec;
}

शून्य vvp_darray_vec4::reverse_elems(शून्य)
{
      माप_प्रकार n = array_.size();
      लागि (माप_प्रकार idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

शून्य vvp_darray_vec4::sort_elems(तार्किक ascending)
{
      यदि (ascending) {
	    std::sort(array_.begin(), array_.end(), vec4_lt_unsigned);
      } अन्यथा {
	    std::sort(array_.begin(), array_.end(),
		      [](अचर vvp_vector4_t&a, अचर vvp_vector4_t&b) {
			    फर्काऊ vec4_lt_unsigned(b, a);
		      });
      }
}

शून्य vvp_darray_vec4::shuffle_elems(शून्य)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

शून्य vvp_darray_vec2::reverse_elems(शून्य)
{
      माप_प्रकार n = array_.size();
      लागि (माप_प्रकार idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

शून्य vvp_darray_vec2::sort_elems(तार्किक ascending)
{
      यदि (ascending) {
	    std::sort(array_.begin(), array_.end());
      } अन्यथा {
	    std::sort(array_.begin(), array_.end(),
		      [](अचर vvp_vector2_t&a, अचर vvp_vector2_t&b) {
			    फर्काऊ b < a;
		      });
      }
}

शून्य vvp_darray_vec2::shuffle_elems(शून्य)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_darray_vec2::~vvp_darray_vec2()
{
}

माप_प्रकार vvp_darray_vec2::get_size(शून्य) अचर
{
      फर्काऊ array_.size();
}

शून्य vvp_darray_vec2::set_word(अचिन्हित adr, अचर vvp_vector4_t&value)
{
      यदि (adr >= array_.size()) फर्काऊ;
      प्रतिज्ञा(value.size() == word_wid_);
      array_[adr] = value;
}

शून्य vvp_darray_vec2::get_word(अचिन्हित adr, vvp_vector4_t&value)
{
	/*
	 * Return a zero value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      यदि ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_0);
	    फर्काऊ;
      }
      प्रतिज्ञा(array_[adr].size() == word_wid_);
      value.resize(word_wid_);
      लागि (अचिन्हित idx = 0; idx < word_wid_; idx += 1) {
	    value.set_bit(idx, array_[adr].value4(idx));
      }
}

शून्य vvp_darray_vec2::shallow_copy(अचर vvp_object*obj)
{
      अचर vvp_darray_vec2*that = dynamic_cast<अचर vvp_darray_vec2*>(obj);
      प्रतिज्ञा(that);

      अचिन्हित अङ्क_वस्तुहरू = min(array_.size(), that->array_.size());
      लागि (अचिन्हित idx = 0 ; idx < अङ्क_वस्तुहरू ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_vector4_t vvp_darray_vec2::get_bitstream(तार्किक)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      अचिन्हित adx = 0;
      अचिन्हित vdx = vec.size();
      जबसम्म (vdx > 0) {
            vdx -= word_wid_;
            लागि (अचिन्हित bdx = 0; bdx < word_wid_; bdx += 1) {
                  यदि (array_[adx].value(bdx))
                        vec.set_bit(vdx+bdx, BIT4_1);
            }
            adx++;
      }

      फर्काऊ vec;
}

vvp_darray_object::~vvp_darray_object()
{
}

माप_प्रकार vvp_darray_object::get_size() अचर
{
      फर्काऊ array_.size();
}

शून्य vvp_darray_object::set_word(अचिन्हित adr, अचर vvp_object_t&value)
{
      यदि (adr >= array_.size())
	    फर्काऊ;
      array_[adr] = value;
}

शून्य vvp_darray_object::get_word(अचिन्हित adr, vvp_object_t&value)
{
      यदि (adr >= array_.size()) {
	    value = vvp_object_t();
	    फर्काऊ;
      }

      value = array_[adr];
}

शून्य vvp_darray_object::shallow_copy(अचर vvp_object*obj)
{
      अचर vvp_darray_object*that = dynamic_cast<अचर vvp_darray_object*>(obj);
      प्रतिज्ञा(that);

      अचिन्हित अङ्क_वस्तुहरू = min(array_.size(), that->array_.size());
      लागि (अचिन्हित idx = 0 ; idx < अङ्क_वस्तुहरू ; idx += 1)
	    array_[idx] = that->array_[idx];
}

शून्य vvp_darray_object::reverse_elems(शून्य)
{
      माप_प्रकार n = array_.size();
      लागि (माप_प्रकार idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

शून्य vvp_darray_object::sort_elems(तार्किक ascending)
{
      यदि (ascending) {
	    std::sort(array_.begin(), array_.end(),
		      [](अचर vvp_object_t&a, अचर vvp_object_t&b) {
			    फर्काऊ a.sort_order_साँचो() < b.sort_order_साँचो();
		      });
      } अन्यथा {
	    std::sort(array_.begin(), array_.end(),
		      [](अचर vvp_object_t&a, अचर vvp_object_t&b) {
			    फर्काऊ a.sort_order_साँचो() > b.sort_order_साँचो();
		      });
      }
}

शून्य vvp_darray_object::shuffle_elems(शून्य)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_darray_real::~vvp_darray_real()
{
}

माप_प्रकार vvp_darray_real::get_size() अचर
{
      फर्काऊ array_.size();
}

शून्य vvp_darray_real::set_word(अचिन्हित adr, द्विगुण value)
{
      यदि (adr >= array_.size())
	    फर्काऊ;
      array_[adr] = value;
}

शून्य vvp_darray_real::get_word(अचिन्हित adr, द्विगुण&value)
{
      यदि (adr >= array_.size()) {
	    value = 0.0;
	    फर्काऊ;
      }

      value = array_[adr];
}

शून्य vvp_darray_real::shallow_copy(अचर vvp_object*obj)
{
      अचर vvp_darray_real*that = dynamic_cast<अचर vvp_darray_real*>(obj);
      प्रतिज्ञा(that);

      अचिन्हित अङ्क_वस्तुहरू = min(array_.size(), that->array_.size());
      लागि (अचिन्हित idx = 0 ; idx < अङ्क_वस्तुहरू ; idx += 1)
	    array_[idx] = that->array_[idx];
}

शून्य vvp_darray_real::reverse_elems(शून्य)
{
      माप_प्रकार n = array_.size();
      लागि (माप_प्रकार idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

शून्य vvp_darray_real::sort_elems(तार्किक ascending)
{
      यदि (ascending) std::sort(array_.begin(), array_.end());
      अन्यथा std::sort(array_.begin(), array_.end(), std::greater<द्विगुण>());
}

शून्य vvp_darray_real::shuffle_elems(शून्य)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_real::duplicate(शून्य) अचर
{
      vvp_darray_real*that = नूतन vvp_darray_real(array_.size());

      लागि (माप_प्रकार idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      फर्काऊ that;
}

vvp_vector4_t vvp_darray_real::get_bitstream(तार्किक)
{
      अचर अचिन्हित word_wid = परिमाण(द्विगुण) * 8;
      प्रतिज्ञा(word_wid == 64);

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      अचिन्हित adx = 0;
      अचिन्हित vdx = vec.size();
      जबसम्म (vdx > 0) {
            सङ्घ {
                द्विगुण   value;
                uint64_t bits;
            } word;
            word.value = array_[adx++];
            vdx -= word_wid;
            लागि (अचिन्हित bdx = 0; bdx < word_wid; bdx += 1) {
                  यदि (word.bits & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word.bits >>= 1;
            }
      }

      फर्काऊ vec;
}

vvp_darray_string::~vvp_darray_string()
{
}

माप_प्रकार vvp_darray_string::get_size() अचर
{
      फर्काऊ array_.size();
}

शून्य vvp_darray_string::set_word(अचिन्हित adr, अचर string&value)
{
      यदि (adr >= array_.size())
	    फर्काऊ;
      array_[adr] = value;
}

शून्य vvp_darray_string::get_word(अचिन्हित adr, string&value)
{
      यदि (adr >= array_.size()) {
	    value = "";
	    फर्काऊ;
      }

      value = array_[adr];
}

शून्य vvp_darray_string::shallow_copy(अचर vvp_object*obj)
{
      अचर vvp_darray_string*that = dynamic_cast<अचर vvp_darray_string*>(obj);
      प्रतिज्ञा(that);

      अचिन्हित अङ्क_वस्तुहरू = min(array_.size(), that->array_.size());
      लागि (अचिन्हित idx = 0 ; idx < अङ्क_वस्तुहरू ; idx += 1)
	    array_[idx] = that->array_[idx];
}

शून्य vvp_darray_string::reverse_elems(शून्य)
{
      माप_प्रकार n = array_.size();
      लागि (माप_प्रकार idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

शून्य vvp_darray_string::sort_elems(तार्किक ascending)
{
      यदि (ascending) std::sort(array_.begin(), array_.end());
      अन्यथा std::sort(array_.begin(), array_.end(), std::greater<string>());
}

शून्य vvp_darray_string::shuffle_elems(शून्य)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_string::duplicate(शून्य) अचर
{
      vvp_darray_string*that = नूतन vvp_darray_string(array_.size());

      लागि (माप_प्रकार idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      फर्काऊ that;
}

vvp_queue::~vvp_queue()
{
}

शून्य vvp_queue::copy_elems(vvp_object_t, अचिन्हित)
{
      cerr << "Sorry: copy_elems() not implemented for " << typeid(*एतत्).name() << endl;
}

शून्य vvp_queue::set_word_max(अचिन्हित, अचर vvp_vector4_t&, अचिन्हित)
{
      cerr << "XXXX set_word_max(vvp_vector4_t) not implemented for " << typeid(*एतत्).name() << endl;
}

शून्य vvp_queue::insert(अचिन्हित, अचर vvp_vector4_t&, अचिन्हित)
{
      cerr << "XXXX insert(vvp_vector4_t) not implemented for " << typeid(*एतत्).name() << endl;
}

शून्य vvp_queue::push_back(अचर vvp_vector4_t&, अचिन्हित)
{
      cerr << "XXXX push_back(vvp_vector4_t) not implemented for " << typeid(*एतत्).name() << endl;
}

शून्य vvp_queue::push_front(अचर vvp_vector4_t&, अचिन्हित)
{
      cerr << "XXXX push_front(vvp_vector4_t) not implemented for " << typeid(*एतत्).name() << endl;
}

शून्य vvp_queue::set_word_max(अचिन्हित, द्विगुण, अचिन्हित)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*एतत्).name() << endl;
}

शून्य vvp_queue::insert(अचिन्हित, द्विगुण, अचिन्हित)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*एतत्).name() << endl;
}

शून्य vvp_queue::push_back(द्विगुण, अचिन्हित)
{
      cerr << "XXXX push_back(double) not implemented for " << typeid(*एतत्).name() << endl;
}

शून्य vvp_queue::push_front(द्विगुण, अचिन्हित)
{
      cerr << "XXXX push_front(double) not implemented for " << typeid(*एतत्).name() << endl;
}

शून्य vvp_queue::set_word_max(अचिन्हित, अचर string&, अचिन्हित)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*एतत्).name() << endl;
}

शून्य vvp_queue::insert(अचिन्हित, अचर string&, अचिन्हित)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*एतत्).name() << endl;
}

शून्य vvp_queue::push_back(अचर string&, अचिन्हित)
{
      cerr << "XXXX push_back(string) not implemented for " << typeid(*एतत्).name() << endl;
}

शून्य vvp_queue::push_front(अचर string&, अचिन्हित)
{
      cerr << "XXXX push_front(string) not implemented for " << typeid(*एतत्).name() << endl;
}

/*
 * Helper functions used while copying multiple elements into a queue.
 */
स्थिर शून्य print_copy_is_too_big(माप_प्रकार src_size, अचिन्हित max_size, अचर string&qtype)
{
      cerr << get_fileline()
           << "Warning: queue<" << qtype << "> is bounded to have at most "
           << max_size << " elements, source has " << src_size << " elements." << endl;
}

स्थिर शून्य print_copy_is_too_big(द्विगुण&, माप_प्रकार src_size, अचिन्हित max_size)
{
      print_copy_is_too_big(src_size, max_size, "real");
}

स्थिर शून्य print_copy_is_too_big(string&, माप_प्रकार src_size, अचिन्हित max_size)
{
      print_copy_is_too_big(src_size, max_size, "string");
}

स्थिर शून्य print_copy_is_too_big(vvp_vector4_t&, माप_प्रकार src_size, अचिन्हित max_size)
{
      print_copy_is_too_big(src_size, max_size, "vector");
}

ढाँचा <typename ELEM, वर्ग QTYPE, वर्ग SRC_TYPE>
स्थिर शून्य copy_elements(QTYPE*queue, SRC_TYPE*src, अचिन्हित max_size)
{
      माप_प्रकार src_size = src->get_size();
      यदि ((max_size != 0) && (src_size > max_size)) {
	    ELEM tmp;
	    print_copy_is_too_big(tmp, src_size, max_size);
      }
      अचिन्हित copy_size = ((src_size < max_size) ||
                            (max_size == 0)) ? src_size : max_size;
      यदि (copy_size < queue->get_size())
	    queue->erase_tail(copy_size);
      लागि (अचिन्हित idx=0; idx < copy_size; ++idx) {
	    ELEM value;
	    src->get_word(idx, value);
	    queue->set_word_max(idx, value, max_size);
      }
}

शून्य vvp_queue_real::copy_elems(vvp_object_t src, अचिन्हित max_size)
{
      यदि (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<द्विगुण, vvp_queue_real, vvp_queue>(एतत्, src_queue, max_size);
      अन्यथा यदि (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<द्विगुण, vvp_queue_real, vvp_darray>(एतत्, src_darray, max_size);
      अन्यथा
	    cerr << get_fileline() << "Sorry: cannot copy object to real queue." << endl;
}

शून्य vvp_queue_real::set_word_max(अचिन्हित adr, द्विगुण value, अचिन्हित max_size)
{
      यदि (adr == queue.size())
	    यदि (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    अन्यथा
		  cerr << get_fileline()
		       << "Warning: assigning to queue<real>[" << adr << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      अन्यथा
	    set_word(adr, value);
}

शून्य vvp_queue_real::set_word(अचिन्हित adr, द्विगुण value)
{
      यदि (adr < queue.size())
	    queue[adr] = value;
      अन्यथा
	    cerr << get_fileline()
	         << "Warning: assigning to queue<real>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
}

शून्य vvp_queue_real::get_word(अचिन्हित adr, द्विगुण&value)
{
      यदि (adr >= queue.size())
	    value = 0.0;
      अन्यथा
	    value = queue[adr];
}

शून्य vvp_queue_real::insert(अचिन्हित idx, द्विगुण value, अचिन्हित max_size)
{
	// Inserting past the end of the queue
      यदि (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<real>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
	// Inserting at the end
      अन्यथा यदि (idx == queue.size())
	    यदि (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    अन्यथा
		  cerr << get_fileline()
		       << "Warning: inserting to queue<real>[" << idx << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      अन्यथा  {
	    यदि (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<real> ["
		       << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

शून्य vvp_queue_real::push_back(द्विगुण value, अचिन्हित max_size)
{
      यदि (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      अन्यथा
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<real> ["
	         << max_size << "]." << endl;
}

शून्य vvp_queue_real::push_front(द्विगुण value, अचिन्हित max_size)
{
      यदि (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<real> ["
	         << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

शून्य vvp_queue_real::erase(अचिन्हित idx)
{
      प्रतिज्ञा(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

शून्य vvp_queue_real::erase_tail(अचिन्हित idx)
{
      प्रतिज्ञा(queue.size() >= idx);
      यदि (queue.size() > idx)
	    queue.resize(idx);
}

शून्य vvp_queue_string::copy_elems(vvp_object_t src, अचिन्हित max_size)
{
      यदि (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<string, vvp_queue_string, vvp_queue>(एतत्, src_queue, max_size);
      अन्यथा यदि (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<string, vvp_queue_string, vvp_darray>(एतत्, src_darray, max_size);
      अन्यथा
	    cerr << get_fileline() << "Sorry: cannot copy object to string queue." << endl;
}

शून्य vvp_queue_string::set_word_max(अचिन्हित adr, अचर string&value, अचिन्हित max_size)
{
      यदि (adr == queue.size())
	    यदि (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    अन्यथा
		  cerr << get_fileline()
		       << "Warning: assigning to queue<string>[" << adr << "] is"
		          " outside bound (" << max_size << "). \"" << value
		       << "\" was not added." << endl;
      अन्यथा
	    set_word(adr, value);
}

शून्य vvp_queue_string::set_word(अचिन्हित adr, अचर string&value)
{
      यदि (adr < queue.size())
	    queue[adr] = value;
      अन्यथा
	    cerr << get_fileline()
	         << "Warning: assigning to queue<string>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << endl;
}

शून्य vvp_queue_string::get_word(अचिन्हित adr, string&value)
{
      यदि (adr >= queue.size())
	    value = "";
      अन्यथा
	    value = queue[adr];
}

शून्य vvp_queue_string::insert(अचिन्हित idx, अचर string&value, अचिन्हित max_size)
{
	// Inserting past the end of the queue
      यदि (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<string>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << endl;
	// Inserting at the end
      अन्यथा यदि (idx == queue.size())
	    यदि (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    अन्यथा
		  cerr << get_fileline()
		       << "Warning: inserting to queue<string>[" << idx << "] is"
		          " outside bound (" << max_size << "). \"" << value
		       << "\" was not added." << endl;
      अन्यथा  {
	    यदि (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", \"" << value << "\") removed \""
		       << queue.back() << "\" from already full bounded queue<string> ["
		       << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

शून्य vvp_queue_string::push_back(अचर string&value, अचिन्हित max_size)
{
      यदि (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      अन्यथा
	    cerr << get_fileline()
	         << "Warning: push_back(\"" << value
	         << "\") skipped for already full bounded queue<string> ["
	         << max_size << "]." << endl;
}

शून्य vvp_queue_string::push_front(अचर string&value, अचिन्हित max_size)
{
      यदि (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(\"" << value << "\") removed \""
	         << queue.back() << "\" from already full bounded queue<string> ["
	         << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

शून्य vvp_queue_string::erase(अचिन्हित idx)
{
      प्रतिज्ञा(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

शून्य vvp_queue_string::erase_tail(अचिन्हित idx)
{
      प्रतिज्ञा(queue.size() >= idx);
      यदि (queue.size() > idx)
	    queue.resize(idx);
}

शून्य vvp_queue_real::reverse_elems(शून्य)
{
      std::reverse(queue.begin(), queue.end());
}

शून्य vvp_queue_string::reverse_elems(शून्य)
{
      std::reverse(queue.begin(), queue.end());
}

शून्य vvp_queue_real::sort_elems(तार्किक ascending)
{
      यदि (ascending) std::sort(queue.begin(), queue.end());
      अन्यथा std::sort(queue.begin(), queue.end(), std::greater<द्विगुण>());
}

शून्य vvp_queue_real::shuffle_elems(शून्य)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

शून्य vvp_queue_string::sort_elems(तार्किक ascending)
{
      यदि (ascending) std::sort(queue.begin(), queue.end());
      अन्यथा std::sort(queue.begin(), queue.end(), std::greater<string>());
}

शून्य vvp_queue_string::shuffle_elems(शून्य)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

शून्य vvp_queue_vec4::copy_elems(vvp_object_t src, अचिन्हित max_size)
{
      यदि (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_queue>(एतत्, src_queue, max_size);
      अन्यथा यदि (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_darray>(एतत्, src_darray, max_size);
      अन्यथा
	    cerr << get_fileline() << "Sorry: cannot copy object to vector queue." << endl;
}

शून्य vvp_queue_vec4::set_word_max(अचिन्हित adr, अचर vvp_vector4_t&value, अचिन्हित max_size)
{
      यदि (adr == queue.size())
	    यदि (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    अन्यथा
		  cerr << get_fileline()
		       << "Warning: assigning to queue<vector>[" << adr << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      अन्यथा
	    set_word(adr, value);
}

शून्य vvp_queue_vec4::set_word(अचिन्हित adr, अचर vvp_vector4_t&value)
{
      यदि (adr < queue.size())
	    queue[adr] = value;
      अन्यथा
	    cerr << get_fileline()
	         << "Warning: assigning to queue<vector>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
}

शून्य vvp_queue_vec4::get_word(अचिन्हित adr, vvp_vector4_t&value)
{
      यदि (adr >= queue.size())
	    value = vvp_vector4_t(queue[0].size());
      अन्यथा
	    value = queue[adr];
}

शून्य vvp_queue_vec4::insert(अचिन्हित idx, अचर vvp_vector4_t&value, अचिन्हित max_size)
{
	// Inserting past the end of the queue
      यदि (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<vector[" << value.size()
	         << "]>[" << idx << "] is outside of size (" << queue.size()
	         << "). " << value << " was not added." << endl;
	// Inserting at the end
      अन्यथा यदि (idx == queue.size())
	    यदि (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    अन्यथा
		  cerr << get_fileline()
		       << "Warning: inserting to queue<vector[" << value.size()
		       << "]>[" << idx << "] is outside bound (" << max_size
		       << "). " << value << " was not added." << endl;
      अन्यथा  {
	    यदि (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<vector["
		       << value.size() << "]> [" << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

शून्य vvp_queue_vec4::push_back(अचर vvp_vector4_t&value, अचिन्हित max_size)
{
      यदि (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      अन्यथा
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<vector["
	         << value.size() << "]> [" << max_size << "]." << endl;
}

शून्य vvp_queue_vec4::push_front(अचर vvp_vector4_t&value, अचिन्हित max_size)
{
      यदि (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<vector["
	         << value.size() << "]> [" << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

शून्य vvp_queue_vec4::erase(अचिन्हित idx)
{
      प्रतिज्ञा(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

शून्य vvp_queue_vec4::erase_tail(अचिन्हित idx)
{
      प्रतिज्ञा(queue.size() >= idx);
      यदि (queue.size() > idx)
	    queue.resize(idx);
}

शून्य vvp_queue_vec4::reverse_elems(शून्य)
{
      std::reverse(queue.begin(), queue.end());
}

शून्य vvp_queue_vec4::sort_elems(तार्किक ascending)
{
      यदि (ascending) {
	    std::sort(queue.begin(), queue.end(), vec4_lt_unsigned);
      } अन्यथा {
	    std::sort(queue.begin(), queue.end(),
		      [](अचर vvp_vector4_t&a, अचर vvp_vector4_t&b) {
			    फर्काऊ vec4_lt_unsigned(b, a);
		      });
      }
}

शून्य vvp_queue_vec4::shuffle_elems(शून्य)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}
