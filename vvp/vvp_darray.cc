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

using namespace std;

namespace {

अटल std::mt19937& shuffle_rng(व्योम)
{
      अटल std::mt19937 rng(std::random_device{}());
      वापस rng;
}

अटल bool vec4_lt_unsigned(स्थिर vvp_vector4_t&a, स्थिर vvp_vector4_t&b)
{
      अगर (a.eeq(b)) वापस false;
      vvp_bit4_t gt = compare_gtge(b, a, BIT4_1);
      अगर (gt == BIT4_X) वापस false;
      वापस gt == BIT4_1;
}

} // namespace

vvp_darray::~vvp_darray()
{
}

व्योम vvp_darray::reverse_elems(व्योम)
{
      अ_त्रुटि << "XXXX reverse_elems() not implemented for "
           << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_darray::sort_elems(bool /*ascending*/)
{
      अ_त्रुटि << "XXXX sort_elems() not implemented for "
           << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_darray::shuffle_elems(व्योम)
{
      अ_त्रुटि << "XXXX shuffle_elems() not implemented for "
           << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_darray::set_word(अचिन्हित, स्थिर vvp_vector4_t&)
{
      अ_त्रुटि << "XXXX set_word(vvp_vector4_t) not implemented for " << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_darray::set_word(अचिन्हित, द्विगुन)
{
      अ_त्रुटि << "XXXX set_word(double) not implemented for " << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_darray::set_word(अचिन्हित, स्थिर string&)
{
      अ_त्रुटि << "XXXX set_word(string) not implemented for " << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_darray::set_word(अचिन्हित, स्थिर vvp_object_t&)
{
      अ_त्रुटि << "XXXX set_word(vvp_object_t) not implemented for " << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_darray::get_word(अचिन्हित, vvp_vector4_t&)
{
      अ_त्रुटि << "XXXX get_word(vvp_vector4_t) not implemented for " << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_darray::get_word(अचिन्हित, द्विगुन&)
{
      अ_त्रुटि << "XXXX get_word(double) not implemented for " << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_darray::get_word(अचिन्हित, string&)
{
      अ_त्रुटि << "XXXX get_word(string) not implemented for " << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_darray::get_word(अचिन्हित, vvp_object_t&)
{
      अ_त्रुटि << "XXXX get_word(vvp_object_t) not implemented for " << typeid(*यह).name() << प_पूर्ण;
}

vvp_vector4_t vvp_darray::get_bitstream(bool)
{
      अ_त्रुटि << "XXXX get_bitstream() not implemented for " << typeid(*यह).name() << प_पूर्ण;
      वापस vvp_vector4_t();
}

ढाँचा <श्रेणी TYPE> vvp_darray_atom<TYPE>::~vvp_darray_atom()
{
}

ढाँचा <श्रेणी TYPE> माप_प्रकार vvp_darray_atom<TYPE>::get_आकार() स्थिर
{
      वापस array_.size();
}

ढाँचा <श्रेणी TYPE> व्योम vvp_darray_atom<TYPE>::reverse_elems(व्योम)
{
      माप_प्रकार n = array_.size();
      क्रम (माप_प्रकार idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

ढाँचा <श्रेणी TYPE> व्योम vvp_darray_atom<TYPE>::sort_elems(bool ascending)
{
      अगर (ascending) std::sort(array_.begin(), array_.end());
      अन्यथा std::sort(array_.begin(), array_.end(), std::greater<TYPE>());
}

ढाँचा <श्रेणी TYPE> व्योम vvp_darray_atom<TYPE>::shuffle_elems(व्योम)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

ढाँचा <श्रेणी TYPE> व्योम vvp_darray_atom<TYPE>::set_word(अचिन्हित adr, स्थिर vvp_vector4_t&value)
{
      अगर (adr >= array_.size())
	    वापस;
      TYPE tmp;
      vector4_to_मूल्य(value, tmp, true, false);
      array_[adr] = tmp;
}

ढाँचा <श्रेणी TYPE> व्योम vvp_darray_atom<TYPE>::get_word(अचिन्हित adr, vvp_vector4_t&value)
{
      अगर (adr >= array_.size()) {
	    value = vvp_vector4_t(8*माप(TYPE), BIT4_X);
	    वापस;
      }

      TYPE word = array_[adr];
      vvp_vector4_t tmp (8*माप(TYPE), BIT4_0);
      क्रम (अचिन्हित idx = 0 ; idx < tmp.size() ; idx += 1) {
	    अगर (word&1) tmp.set_bit(idx, BIT4_1);
	    word >>= 1;
      }
      value = tmp;
}

ढाँचा <श्रेणी TYPE> व्योम vvp_darray_atom<TYPE>::shallow_copy(स्थिर vvp_object*obj)
{
      स्थिर vvp_darray_atom<TYPE>*that = dynamic_cast<स्थिर vvp_darray_atom<TYPE>*>(obj);
      निश्चित(that);

      अचिन्हित अंक_वस्तुएँ = min(array_.size(), that->array_.size());
      क्रम (अचिन्हित idx = 0 ; idx < अंक_वस्तुएँ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

ढाँचा <श्रेणी TYPE> vvp_object* vvp_darray_atom<TYPE>::duplicate(व्योम) स्थिर
{
      vvp_darray_atom<TYPE>*that = नया vvp_darray_atom<TYPE>(array_.size());
      क्रम (माप_प्रकार idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      वापस that;
}

ढाँचा <श्रेणी TYPE> vvp_vector4_t vvp_darray_atom<TYPE>::get_bitstream(bool)
{
      स्थिर अचिन्हित word_wid = माप(TYPE) * 8;

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      अचिन्हित adx = 0;
      अचिन्हित vdx = vec.size();
      जबतक (vdx > 0) {
            TYPE word = array_[adx++];
            vdx -= word_wid;
            क्रम (अचिन्हित bdx = 0; bdx < word_wid; bdx += 1) {
                  अगर (word & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word >>= 1;
            }
      }

      वापस vec;
}

ढाँचा श्रेणी vvp_darray_atom<uint8_t>;
ढाँचा श्रेणी vvp_darray_atom<uint16_t>;
ढाँचा श्रेणी vvp_darray_atom<uint32_t>;
ढाँचा श्रेणी vvp_darray_atom<uint64_t>;
ढाँचा श्रेणी vvp_darray_atom<int8_t>;
ढाँचा श्रेणी vvp_darray_atom<int16_t>;
ढाँचा श्रेणी vvp_darray_atom<int32_t>;
ढाँचा श्रेणी vvp_darray_atom<int64_t>;

vvp_darray_vec4::~vvp_darray_vec4()
{
}

माप_प्रकार vvp_darray_vec4::get_आकार(व्योम) स्थिर
{
      वापस array_.size();
}

व्योम vvp_darray_vec4::set_word(अचिन्हित adr, स्थिर vvp_vector4_t&value)
{
      अगर (adr >= array_.size()) वापस;
      निश्चित(value.size() == word_wid_);
      array_[adr] = value;
}

व्योम vvp_darray_vec4::get_word(अचिन्हित adr, vvp_vector4_t&value)
{
	/*
	 * Return an undefined value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      अगर ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_X);
	    वापस;
      }
      value = array_[adr];
      निश्चित(value.size() == word_wid_);
}

व्योम vvp_darray_vec4::shallow_copy(स्थिर vvp_object*obj)
{
      स्थिर vvp_darray_vec4*that = dynamic_cast<स्थिर vvp_darray_vec4*>(obj);
      निश्चित(that);

      अचिन्हित अंक_वस्तुएँ = min(array_.size(), that->array_.size());
      क्रम (अचिन्हित idx = 0 ; idx < अंक_वस्तुएँ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_object* vvp_darray_vec4::duplicate(व्योम) स्थिर
{
      vvp_darray_vec4*that = नया vvp_darray_vec4(array_.size(), word_wid_);

      क्रम (माप_प्रकार idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      वापस that;
}

vvp_vector4_t vvp_darray_vec4::get_bitstream(bool as_vec4)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      अचिन्हित adx = 0;
      अचिन्हित vdx = vec.size();
      जबतक (vdx > 0) {
            vdx -= word_wid_;
            क्रम (अचिन्हित bdx = 0; bdx < word_wid_; bdx += 1) {
                  vvp_bit4_t bit = array_[adx].value(bdx);
                  अगर (as_vec4 || (bit == BIT4_1))
                        vec.set_bit(vdx+bdx, bit);
            }
            adx++;
      }

      वापस vec;
}

व्योम vvp_darray_vec4::reverse_elems(व्योम)
{
      माप_प्रकार n = array_.size();
      क्रम (माप_प्रकार idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

व्योम vvp_darray_vec4::sort_elems(bool ascending)
{
      अगर (ascending) {
	    std::sort(array_.begin(), array_.end(), vec4_lt_unsigned);
      } अन्यथा {
	    std::sort(array_.begin(), array_.end(),
		      [](स्थिर vvp_vector4_t&a, स्थिर vvp_vector4_t&b) {
			    वापस vec4_lt_unsigned(b, a);
		      });
      }
}

व्योम vvp_darray_vec4::shuffle_elems(व्योम)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

व्योम vvp_darray_vec2::reverse_elems(व्योम)
{
      माप_प्रकार n = array_.size();
      क्रम (माप_प्रकार idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

व्योम vvp_darray_vec2::sort_elems(bool ascending)
{
      अगर (ascending) {
	    std::sort(array_.begin(), array_.end());
      } अन्यथा {
	    std::sort(array_.begin(), array_.end(),
		      [](स्थिर vvp_vector2_t&a, स्थिर vvp_vector2_t&b) {
			    वापस b < a;
		      });
      }
}

व्योम vvp_darray_vec2::shuffle_elems(व्योम)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_darray_vec2::~vvp_darray_vec2()
{
}

माप_प्रकार vvp_darray_vec2::get_आकार(व्योम) स्थिर
{
      वापस array_.size();
}

व्योम vvp_darray_vec2::set_word(अचिन्हित adr, स्थिर vvp_vector4_t&value)
{
      अगर (adr >= array_.size()) वापस;
      निश्चित(value.size() == word_wid_);
      array_[adr] = value;
}

व्योम vvp_darray_vec2::get_word(अचिन्हित adr, vvp_vector4_t&value)
{
	/*
	 * Return a zero value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      अगर ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_0);
	    वापस;
      }
      निश्चित(array_[adr].size() == word_wid_);
      value.resize(word_wid_);
      क्रम (अचिन्हित idx = 0; idx < word_wid_; idx += 1) {
	    value.set_bit(idx, array_[adr].value4(idx));
      }
}

व्योम vvp_darray_vec2::shallow_copy(स्थिर vvp_object*obj)
{
      स्थिर vvp_darray_vec2*that = dynamic_cast<स्थिर vvp_darray_vec2*>(obj);
      निश्चित(that);

      अचिन्हित अंक_वस्तुएँ = min(array_.size(), that->array_.size());
      क्रम (अचिन्हित idx = 0 ; idx < अंक_वस्तुएँ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_vector4_t vvp_darray_vec2::get_bitstream(bool)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      अचिन्हित adx = 0;
      अचिन्हित vdx = vec.size();
      जबतक (vdx > 0) {
            vdx -= word_wid_;
            क्रम (अचिन्हित bdx = 0; bdx < word_wid_; bdx += 1) {
                  अगर (array_[adx].value(bdx))
                        vec.set_bit(vdx+bdx, BIT4_1);
            }
            adx++;
      }

      वापस vec;
}

vvp_darray_object::~vvp_darray_object()
{
}

माप_प्रकार vvp_darray_object::get_आकार() स्थिर
{
      वापस array_.size();
}

व्योम vvp_darray_object::set_word(अचिन्हित adr, स्थिर vvp_object_t&value)
{
      अगर (adr >= array_.size())
	    वापस;
      array_[adr] = value;
}

व्योम vvp_darray_object::get_word(अचिन्हित adr, vvp_object_t&value)
{
      अगर (adr >= array_.size()) {
	    value = vvp_object_t();
	    वापस;
      }

      value = array_[adr];
}

व्योम vvp_darray_object::shallow_copy(स्थिर vvp_object*obj)
{
      स्थिर vvp_darray_object*that = dynamic_cast<स्थिर vvp_darray_object*>(obj);
      निश्चित(that);

      अचिन्हित अंक_वस्तुएँ = min(array_.size(), that->array_.size());
      क्रम (अचिन्हित idx = 0 ; idx < अंक_वस्तुएँ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

व्योम vvp_darray_object::reverse_elems(व्योम)
{
      माप_प्रकार n = array_.size();
      क्रम (माप_प्रकार idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

व्योम vvp_darray_object::sort_elems(bool ascending)
{
      अगर (ascending) {
	    std::sort(array_.begin(), array_.end(),
		      [](स्थिर vvp_object_t&a, स्थिर vvp_object_t&b) {
			    वापस a.sort_order_कुंजी() < b.sort_order_कुंजी();
		      });
      } अन्यथा {
	    std::sort(array_.begin(), array_.end(),
		      [](स्थिर vvp_object_t&a, स्थिर vvp_object_t&b) {
			    वापस a.sort_order_कुंजी() > b.sort_order_कुंजी();
		      });
      }
}

व्योम vvp_darray_object::shuffle_elems(व्योम)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_darray_real::~vvp_darray_real()
{
}

माप_प्रकार vvp_darray_real::get_आकार() स्थिर
{
      वापस array_.size();
}

व्योम vvp_darray_real::set_word(अचिन्हित adr, द्विगुन value)
{
      अगर (adr >= array_.size())
	    वापस;
      array_[adr] = value;
}

व्योम vvp_darray_real::get_word(अचिन्हित adr, द्विगुन&value)
{
      अगर (adr >= array_.size()) {
	    value = 0.0;
	    वापस;
      }

      value = array_[adr];
}

व्योम vvp_darray_real::shallow_copy(स्थिर vvp_object*obj)
{
      स्थिर vvp_darray_real*that = dynamic_cast<स्थिर vvp_darray_real*>(obj);
      निश्चित(that);

      अचिन्हित अंक_वस्तुएँ = min(array_.size(), that->array_.size());
      क्रम (अचिन्हित idx = 0 ; idx < अंक_वस्तुएँ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

व्योम vvp_darray_real::reverse_elems(व्योम)
{
      माप_प्रकार n = array_.size();
      क्रम (माप_प्रकार idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

व्योम vvp_darray_real::sort_elems(bool ascending)
{
      अगर (ascending) std::sort(array_.begin(), array_.end());
      अन्यथा std::sort(array_.begin(), array_.end(), std::greater<द्विगुन>());
}

व्योम vvp_darray_real::shuffle_elems(व्योम)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_real::duplicate(व्योम) स्थिर
{
      vvp_darray_real*that = नया vvp_darray_real(array_.size());

      क्रम (माप_प्रकार idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      वापस that;
}

vvp_vector4_t vvp_darray_real::get_bitstream(bool)
{
      स्थिर अचिन्हित word_wid = माप(द्विगुन) * 8;
      निश्चित(word_wid == 64);

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      अचिन्हित adx = 0;
      अचिन्हित vdx = vec.size();
      जबतक (vdx > 0) {
            जोड़ {
                द्विगुन   value;
                uint64_t bits;
            } word;
            word.value = array_[adx++];
            vdx -= word_wid;
            क्रम (अचिन्हित bdx = 0; bdx < word_wid; bdx += 1) {
                  अगर (word.bits & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word.bits >>= 1;
            }
      }

      वापस vec;
}

vvp_darray_string::~vvp_darray_string()
{
}

माप_प्रकार vvp_darray_string::get_आकार() स्थिर
{
      वापस array_.size();
}

व्योम vvp_darray_string::set_word(अचिन्हित adr, स्थिर string&value)
{
      अगर (adr >= array_.size())
	    वापस;
      array_[adr] = value;
}

व्योम vvp_darray_string::get_word(अचिन्हित adr, string&value)
{
      अगर (adr >= array_.size()) {
	    value = "";
	    वापस;
      }

      value = array_[adr];
}

व्योम vvp_darray_string::shallow_copy(स्थिर vvp_object*obj)
{
      स्थिर vvp_darray_string*that = dynamic_cast<स्थिर vvp_darray_string*>(obj);
      निश्चित(that);

      अचिन्हित अंक_वस्तुएँ = min(array_.size(), that->array_.size());
      क्रम (अचिन्हित idx = 0 ; idx < अंक_वस्तुएँ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

व्योम vvp_darray_string::reverse_elems(व्योम)
{
      माप_प्रकार n = array_.size();
      क्रम (माप_प्रकार idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

व्योम vvp_darray_string::sort_elems(bool ascending)
{
      अगर (ascending) std::sort(array_.begin(), array_.end());
      अन्यथा std::sort(array_.begin(), array_.end(), std::greater<string>());
}

व्योम vvp_darray_string::shuffle_elems(व्योम)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_string::duplicate(व्योम) स्थिर
{
      vvp_darray_string*that = नया vvp_darray_string(array_.size());

      क्रम (माप_प्रकार idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      वापस that;
}

vvp_queue::~vvp_queue()
{
}

व्योम vvp_queue::copy_elems(vvp_object_t, अचिन्हित)
{
      अ_त्रुटि << "Sorry: copy_elems() not implemented for " << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_queue::set_word_max(अचिन्हित, स्थिर vvp_vector4_t&, अचिन्हित)
{
      अ_त्रुटि << "XXXX set_word_max(vvp_vector4_t) not implemented for " << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_queue::insert(अचिन्हित, स्थिर vvp_vector4_t&, अचिन्हित)
{
      अ_त्रुटि << "XXXX insert(vvp_vector4_t) not implemented for " << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_queue::push_back(स्थिर vvp_vector4_t&, अचिन्हित)
{
      अ_त्रुटि << "XXXX push_back(vvp_vector4_t) not implemented for " << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_queue::push_front(स्थिर vvp_vector4_t&, अचिन्हित)
{
      अ_त्रुटि << "XXXX push_front(vvp_vector4_t) not implemented for " << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_queue::set_word_max(अचिन्हित, द्विगुन, अचिन्हित)
{
      अ_त्रुटि << "XXXX set_word_max(double) not implemented for " << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_queue::insert(अचिन्हित, द्विगुन, अचिन्हित)
{
      अ_त्रुटि << "XXXX set_word_max(double) not implemented for " << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_queue::push_back(द्विगुन, अचिन्हित)
{
      अ_त्रुटि << "XXXX push_back(double) not implemented for " << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_queue::push_front(द्विगुन, अचिन्हित)
{
      अ_त्रुटि << "XXXX push_front(double) not implemented for " << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_queue::set_word_max(अचिन्हित, स्थिर string&, अचिन्हित)
{
      अ_त्रुटि << "XXXX set_word_max(string) not implemented for " << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_queue::insert(अचिन्हित, स्थिर string&, अचिन्हित)
{
      अ_त्रुटि << "XXXX set_word_max(string) not implemented for " << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_queue::push_back(स्थिर string&, अचिन्हित)
{
      अ_त्रुटि << "XXXX push_back(string) not implemented for " << typeid(*यह).name() << प_पूर्ण;
}

व्योम vvp_queue::push_front(स्थिर string&, अचिन्हित)
{
      अ_त्रुटि << "XXXX push_front(string) not implemented for " << typeid(*यह).name() << प_पूर्ण;
}

/*
 * Helper functions used while copying multiple elements into a queue.
 */
अटल व्योम print_copy_is_too_big(माप_प्रकार src_आकार, अचिन्हित max_आकार, स्थिर string&qtype)
{
      अ_त्रुटि << get_fileline()
           << "Warning: queue<" << qtype << "> is bounded to have at most "
           << max_आकार << " elements, source has " << src_आकार << " elements." << प_पूर्ण;
}

अटल व्योम print_copy_is_too_big(द्विगुन&, माप_प्रकार src_आकार, अचिन्हित max_आकार)
{
      print_copy_is_too_big(src_आकार, max_आकार, "real");
}

अटल व्योम print_copy_is_too_big(string&, माप_प्रकार src_आकार, अचिन्हित max_आकार)
{
      print_copy_is_too_big(src_आकार, max_आकार, "string");
}

अटल व्योम print_copy_is_too_big(vvp_vector4_t&, माप_प्रकार src_आकार, अचिन्हित max_आकार)
{
      print_copy_is_too_big(src_आकार, max_आकार, "vector");
}

ढाँचा <typename ELEM, श्रेणी QTYPE, श्रेणी SRC_TYPE>
अटल व्योम copy_elements(QTYPE*queue, SRC_TYPE*src, अचिन्हित max_आकार)
{
      माप_प्रकार src_आकार = src->get_आकार();
      अगर ((max_आकार != 0) && (src_आकार > max_आकार)) {
	    ELEM tmp;
	    print_copy_is_too_big(tmp, src_आकार, max_आकार);
      }
      अचिन्हित copy_आकार = ((src_आकार < max_आकार) ||
                            (max_आकार == 0)) ? src_आकार : max_आकार;
      अगर (copy_आकार < queue->get_आकार())
	    queue->erase_पुच्छ(copy_आकार);
      क्रम (अचिन्हित idx=0; idx < copy_आकार; ++idx) {
	    ELEM value;
	    src->get_word(idx, value);
	    queue->set_word_max(idx, value, max_आकार);
      }
}

व्योम vvp_queue_real::copy_elems(vvp_object_t src, अचिन्हित max_आकार)
{
      अगर (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<द्विगुन, vvp_queue_real, vvp_queue>(यह, src_queue, max_आकार);
      अन्यथा अगर (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<द्विगुन, vvp_queue_real, vvp_darray>(यह, src_darray, max_आकार);
      अन्यथा
	    अ_त्रुटि << get_fileline() << "Sorry: cannot copy object to real queue." << प_पूर्ण;
}

व्योम vvp_queue_real::set_word_max(अचिन्हित adr, द्विगुन value, अचिन्हित max_आकार)
{
      अगर (adr == queue.size())
	    अगर (!max_आकार || (queue.size() < max_आकार))
		  queue.push_back(value);
	    अन्यथा
		  अ_त्रुटि << get_fileline()
		       << "Warning: assigning to queue<real>[" << adr << "] is"
		          " outside bound (" << max_आकार << "). " << value
		       << " was not added." << प_पूर्ण;
      अन्यथा
	    set_word(adr, value);
}

व्योम vvp_queue_real::set_word(अचिन्हित adr, द्विगुन value)
{
      अगर (adr < queue.size())
	    queue[adr] = value;
      अन्यथा
	    अ_त्रुटि << get_fileline()
	         << "Warning: assigning to queue<real>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << प_पूर्ण;
}

व्योम vvp_queue_real::get_word(अचिन्हित adr, द्विगुन&value)
{
      अगर (adr >= queue.size())
	    value = 0.0;
      अन्यथा
	    value = queue[adr];
}

व्योम vvp_queue_real::insert(अचिन्हित idx, द्विगुन value, अचिन्हित max_आकार)
{
	// Inserting past the end of the queue
      अगर (idx > queue.size())
	    अ_त्रुटि << get_fileline()
	         << "Warning: inserting to queue<real>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). " << value
	         << " was not added." << प_पूर्ण;
	// Inserting at the end
      अन्यथा अगर (idx == queue.size())
	    अगर (!max_आकार || (queue.size() < max_आकार))
		  queue.push_back(value);
	    अन्यथा
		  अ_त्रुटि << get_fileline()
		       << "Warning: inserting to queue<real>[" << idx << "] is"
		          " outside bound (" << max_आकार << "). " << value
		       << " was not added." << प_पूर्ण;
      अन्यथा  {
	    अगर (max_आकार && (queue.size() == max_आकार)) {
		  अ_त्रुटि << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<real> ["
		       << max_आकार << "]." << प_पूर्ण;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

व्योम vvp_queue_real::push_back(द्विगुन value, अचिन्हित max_आकार)
{
      अगर (!max_आकार || (queue.size() < max_आकार))
	    queue.push_back(value);
      अन्यथा
	    अ_त्रुटि << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<real> ["
	         << max_आकार << "]." << प_पूर्ण;
}

व्योम vvp_queue_real::push_front(द्विगुन value, अचिन्हित max_आकार)
{
      अगर (max_आकार && (queue.size() == max_आकार)) {
	    अ_त्रुटि << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<real> ["
	         << max_आकार << "]." << प_पूर्ण;
	    queue.pop_back();
      }
      queue.push_front(value);
}

व्योम vvp_queue_real::erase(अचिन्हित idx)
{
      निश्चित(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

व्योम vvp_queue_real::erase_पुच्छ(अचिन्हित idx)
{
      निश्चित(queue.size() >= idx);
      अगर (queue.size() > idx)
	    queue.resize(idx);
}

व्योम vvp_queue_string::copy_elems(vvp_object_t src, अचिन्हित max_आकार)
{
      अगर (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<string, vvp_queue_string, vvp_queue>(यह, src_queue, max_आकार);
      अन्यथा अगर (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<string, vvp_queue_string, vvp_darray>(यह, src_darray, max_आकार);
      अन्यथा
	    अ_त्रुटि << get_fileline() << "Sorry: cannot copy object to string queue." << प_पूर्ण;
}

व्योम vvp_queue_string::set_word_max(अचिन्हित adr, स्थिर string&value, अचिन्हित max_आकार)
{
      अगर (adr == queue.size())
	    अगर (!max_आकार || (queue.size() < max_आकार))
		  queue.push_back(value);
	    अन्यथा
		  अ_त्रुटि << get_fileline()
		       << "Warning: assigning to queue<string>[" << adr << "] is"
		          " outside bound (" << max_आकार << "). \"" << value
		       << "\" was not added." << प_पूर्ण;
      अन्यथा
	    set_word(adr, value);
}

व्योम vvp_queue_string::set_word(अचिन्हित adr, स्थिर string&value)
{
      अगर (adr < queue.size())
	    queue[adr] = value;
      अन्यथा
	    अ_त्रुटि << get_fileline()
	         << "Warning: assigning to queue<string>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << प_पूर्ण;
}

व्योम vvp_queue_string::get_word(अचिन्हित adr, string&value)
{
      अगर (adr >= queue.size())
	    value = "";
      अन्यथा
	    value = queue[adr];
}

व्योम vvp_queue_string::insert(अचिन्हित idx, स्थिर string&value, अचिन्हित max_आकार)
{
	// Inserting past the end of the queue
      अगर (idx > queue.size())
	    अ_त्रुटि << get_fileline()
	         << "Warning: inserting to queue<string>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << प_पूर्ण;
	// Inserting at the end
      अन्यथा अगर (idx == queue.size())
	    अगर (!max_आकार || (queue.size() < max_आकार))
		  queue.push_back(value);
	    अन्यथा
		  अ_त्रुटि << get_fileline()
		       << "Warning: inserting to queue<string>[" << idx << "] is"
		          " outside bound (" << max_आकार << "). \"" << value
		       << "\" was not added." << प_पूर्ण;
      अन्यथा  {
	    अगर (max_आकार && (queue.size() == max_आकार)) {
		  अ_त्रुटि << get_fileline()
		       << "Warning: insert("<< idx << ", \"" << value << "\") removed \""
		       << queue.back() << "\" from already full bounded queue<string> ["
		       << max_आकार << "]." << प_पूर्ण;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

व्योम vvp_queue_string::push_back(स्थिर string&value, अचिन्हित max_आकार)
{
      अगर (!max_आकार || (queue.size() < max_आकार))
	    queue.push_back(value);
      अन्यथा
	    अ_त्रुटि << get_fileline()
	         << "Warning: push_back(\"" << value
	         << "\") skipped for already full bounded queue<string> ["
	         << max_आकार << "]." << प_पूर्ण;
}

व्योम vvp_queue_string::push_front(स्थिर string&value, अचिन्हित max_आकार)
{
      अगर (max_आकार && (queue.size() == max_आकार)) {
	    अ_त्रुटि << get_fileline()
	         << "Warning: push_front(\"" << value << "\") removed \""
	         << queue.back() << "\" from already full bounded queue<string> ["
	         << max_आकार << "]." << प_पूर्ण;
	    queue.pop_back();
      }
      queue.push_front(value);
}

व्योम vvp_queue_string::erase(अचिन्हित idx)
{
      निश्चित(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

व्योम vvp_queue_string::erase_पुच्छ(अचिन्हित idx)
{
      निश्चित(queue.size() >= idx);
      अगर (queue.size() > idx)
	    queue.resize(idx);
}

व्योम vvp_queue_real::reverse_elems(व्योम)
{
      std::reverse(queue.begin(), queue.end());
}

व्योम vvp_queue_string::reverse_elems(व्योम)
{
      std::reverse(queue.begin(), queue.end());
}

व्योम vvp_queue_real::sort_elems(bool ascending)
{
      अगर (ascending) std::sort(queue.begin(), queue.end());
      अन्यथा std::sort(queue.begin(), queue.end(), std::greater<द्विगुन>());
}

व्योम vvp_queue_real::shuffle_elems(व्योम)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

व्योम vvp_queue_string::sort_elems(bool ascending)
{
      अगर (ascending) std::sort(queue.begin(), queue.end());
      अन्यथा std::sort(queue.begin(), queue.end(), std::greater<string>());
}

व्योम vvp_queue_string::shuffle_elems(व्योम)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

व्योम vvp_queue_vec4::copy_elems(vvp_object_t src, अचिन्हित max_आकार)
{
      अगर (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_queue>(यह, src_queue, max_आकार);
      अन्यथा अगर (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_darray>(यह, src_darray, max_आकार);
      अन्यथा
	    अ_त्रुटि << get_fileline() << "Sorry: cannot copy object to vector queue." << प_पूर्ण;
}

व्योम vvp_queue_vec4::set_word_max(अचिन्हित adr, स्थिर vvp_vector4_t&value, अचिन्हित max_आकार)
{
      अगर (adr == queue.size())
	    अगर (!max_आकार || (queue.size() < max_आकार))
		  queue.push_back(value);
	    अन्यथा
		  अ_त्रुटि << get_fileline()
		       << "Warning: assigning to queue<vector>[" << adr << "] is"
		          " outside bound (" << max_आकार << "). " << value
		       << " was not added." << प_पूर्ण;
      अन्यथा
	    set_word(adr, value);
}

व्योम vvp_queue_vec4::set_word(अचिन्हित adr, स्थिर vvp_vector4_t&value)
{
      अगर (adr < queue.size())
	    queue[adr] = value;
      अन्यथा
	    अ_त्रुटि << get_fileline()
	         << "Warning: assigning to queue<vector>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << प_पूर्ण;
}

व्योम vvp_queue_vec4::get_word(अचिन्हित adr, vvp_vector4_t&value)
{
      अगर (adr >= queue.size())
	    value = vvp_vector4_t(queue[0].size());
      अन्यथा
	    value = queue[adr];
}

व्योम vvp_queue_vec4::insert(अचिन्हित idx, स्थिर vvp_vector4_t&value, अचिन्हित max_आकार)
{
	// Inserting past the end of the queue
      अगर (idx > queue.size())
	    अ_त्रुटि << get_fileline()
	         << "Warning: inserting to queue<vector[" << value.size()
	         << "]>[" << idx << "] is outside of size (" << queue.size()
	         << "). " << value << " was not added." << प_पूर्ण;
	// Inserting at the end
      अन्यथा अगर (idx == queue.size())
	    अगर (!max_आकार || (queue.size() < max_आकार))
		  queue.push_back(value);
	    अन्यथा
		  अ_त्रुटि << get_fileline()
		       << "Warning: inserting to queue<vector[" << value.size()
		       << "]>[" << idx << "] is outside bound (" << max_आकार
		       << "). " << value << " was not added." << प_पूर्ण;
      अन्यथा  {
	    अगर (max_आकार && (queue.size() == max_आकार)) {
		  अ_त्रुटि << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<vector["
		       << value.size() << "]> [" << max_आकार << "]." << प_पूर्ण;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

व्योम vvp_queue_vec4::push_back(स्थिर vvp_vector4_t&value, अचिन्हित max_आकार)
{
      अगर (!max_आकार || (queue.size() < max_आकार))
	    queue.push_back(value);
      अन्यथा
	    अ_त्रुटि << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<vector["
	         << value.size() << "]> [" << max_आकार << "]." << प_पूर्ण;
}

व्योम vvp_queue_vec4::push_front(स्थिर vvp_vector4_t&value, अचिन्हित max_आकार)
{
      अगर (max_आकार && (queue.size() == max_आकार)) {
	    अ_त्रुटि << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<vector["
	         << value.size() << "]> [" << max_आकार << "]." << प_पूर्ण;
	    queue.pop_back();
      }
      queue.push_front(value);
}

व्योम vvp_queue_vec4::erase(अचिन्हित idx)
{
      निश्चित(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

व्योम vvp_queue_vec4::erase_पुच्छ(अचिन्हित idx)
{
      निश्चित(queue.size() >= idx);
      अगर (queue.size() > idx)
	    queue.resize(idx);
}

व्योम vvp_queue_vec4::reverse_elems(व्योम)
{
      std::reverse(queue.begin(), queue.end());
}

व्योम vvp_queue_vec4::sort_elems(bool ascending)
{
      अगर (ascending) {
	    std::sort(queue.begin(), queue.end(), vec4_lt_unsigned);
      } अन्यथा {
	    std::sort(queue.begin(), queue.end(),
		      [](स्थिर vvp_vector4_t&a, स्थिर vvp_vector4_t&b) {
			    वापस vec4_lt_unsigned(b, a);
		      });
      }
}

व्योम vvp_queue_vec4::shuffle_elems(व्योम)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}
