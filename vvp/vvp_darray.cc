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

उपयुज्य नामाकाश std;

नामाकाश {

स्थिर std::mt19937& shuffle_rng(शून्य)
{
      स्थिर std::mt19937 rng(std::random_device{}());
      प्रत्यागम rng;
}

स्थिर तार्किक vec4_lt_unsigned(ध्रुव vvp_vector4_t&a, ध्रुव vvp_vector4_t&b)
{
      यदि (a.eeq(b)) प्रत्यागम असत्य;
      vvp_bit4_t gt = compare_gtge(b, a, BIT4_1);
      यदि (gt == BIT4_X) प्रत्यागम असत्य;
      प्रत्यागम gt == BIT4_1;
}

} // namespace

vvp_darray::~vvp_darray()
{
}

शून्य vvp_darray::reverse_elems(शून्य)
{
      cerr << "XXXX reverse_elems() not implemented for "
           << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_darray::क्रमय_elems(तार्किक /*ascending*/)
{
      cerr << "XXXX sort_elems() not implemented for "
           << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_darray::shuffle_elems(शून्य)
{
      cerr << "XXXX shuffle_elems() not implemented for "
           << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_darray::समुच्चयः_word(अचिह्नित, ध्रुव vvp_vector4_t&)
{
      cerr << "XXXX set_word(vvp_vector4_t) not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_darray::समुच्चयः_word(अचिह्नित, द्विगुण)
{
      cerr << "XXXX set_word(double) not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_darray::समुच्चयः_word(अचिह्नित, ध्रुव string&)
{
      cerr << "XXXX set_word(string) not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_darray::समुच्चयः_word(अचिह्नित, ध्रुव vvp_object_t&)
{
      cerr << "XXXX set_word(vvp_object_t) not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_darray::get_word(अचिह्नित, vvp_vector4_t&)
{
      cerr << "XXXX get_word(vvp_vector4_t) not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_darray::get_word(अचिह्नित, द्विगुण&)
{
      cerr << "XXXX get_word(double) not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_darray::get_word(अचिह्नित, string&)
{
      cerr << "XXXX get_word(string) not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_darray::get_word(अचिह्नित, vvp_object_t&)
{
      cerr << "XXXX get_word(vvp_object_t) not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

vvp_vector4_t vvp_darray::get_bitstream(तार्किक)
{
      cerr << "XXXX get_bitstream() not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
      प्रत्यागम vvp_vector4_t();
}

प्रतिरूपम् <वर्ग TYPE> vvp_darray_atom<TYPE>::~vvp_darray_atom()
{
}

प्रतिरूपम् <वर्ग TYPE> size_t vvp_darray_atom<TYPE>::get_आकार() ध्रुव
{
      प्रत्यागम array_.size();
}

प्रतिरूपम् <वर्ग TYPE> शून्य vvp_darray_atom<TYPE>::reverse_elems(शून्य)
{
      size_t n = array_.size();
      प्रति (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

प्रतिरूपम् <वर्ग TYPE> शून्य vvp_darray_atom<TYPE>::क्रमय_elems(तार्किक ascending)
{
      यदि (ascending) std::sort(array_.begin(), array_.अन्तः());
      अन्यथा std::sort(array_.begin(), array_.अन्तः(), std::greater<TYPE>());
}

प्रतिरूपम् <वर्ग TYPE> शून्य vvp_darray_atom<TYPE>::shuffle_elems(शून्य)
{
      std::shuffle(array_.begin(), array_.अन्तः(), shuffle_rng());
}

प्रतिरूपम् <वर्ग TYPE> शून्य vvp_darray_atom<TYPE>::समुच्चयः_word(अचिह्नित adr, ध्रुव vvp_vector4_t&value)
{
      यदि (adr >= array_.size())
	    प्रत्यागम;
      TYPE tmp;
      vector4_गन्तव्यम्_मानम्(value, tmp, सत्य, असत्य);
      array_[adr] = tmp;
}

प्रतिरूपम् <वर्ग TYPE> शून्य vvp_darray_atom<TYPE>::get_word(अचिह्नित adr, vvp_vector4_t&value)
{
      यदि (adr >= array_.size()) {
	    value = vvp_vector4_t(8*परिमाण(TYPE), BIT4_X);
	    प्रत्यागम;
      }

      TYPE word = array_[adr];
      vvp_vector4_t tmp (8*परिमाण(TYPE), BIT4_0);
      प्रति (अचिह्नित idx = 0 ; idx < tmp.size() ; idx += 1) {
	    यदि (word&1) tmp.समुच्चयः_bit(idx, BIT4_1);
	    word >>= 1;
      }
      value = tmp;
}

प्रतिरूपम् <वर्ग TYPE> शून्य vvp_darray_atom<TYPE>::shallow_copy(ध्रुव vvp_object*obj)
{
      ध्रुव vvp_darray_atom<TYPE>*that = dynamic_cast<ध्रुव vvp_darray_atom<TYPE>*>(obj);
      प्रतिज्ञा(that);

      अचिह्नित अङ्कः_वस्तूनि = min(array_.size(), that->array_.size());
      प्रति (अचिह्नित idx = 0 ; idx < अङ्कः_वस्तूनि ; idx += 1)
	    array_[idx] = that->array_[idx];
}

प्रतिरूपम् <वर्ग TYPE> vvp_object* vvp_darray_atom<TYPE>::duplicate(शून्य) ध्रुव
{
      vvp_darray_atom<TYPE>*that = नूतन vvp_darray_atom<TYPE>(array_.size());
      प्रति (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      प्रत्यागम that;
}

प्रतिरूपम् <वर्ग TYPE> vvp_vector4_t vvp_darray_atom<TYPE>::get_bitstream(तार्किक)
{
      ध्रुव अचिह्नित word_wid = परिमाण(TYPE) * 8;

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      अचिह्नित adx = 0;
      अचिह्नित vdx = vec.size();
      यावत् (vdx > 0) {
            TYPE word = array_[adx++];
            vdx -= word_wid;
            प्रति (अचिह्नित bdx = 0; bdx < word_wid; bdx += 1) {
                  यदि (word & 1)
                        vec.समुच्चयः_bit(vdx+bdx, BIT4_1);
                  word >>= 1;
            }
      }

      प्रत्यागम vec;
}

प्रतिरूपम् वर्ग vvp_darray_atom<uint8_t>;
प्रतिरूपम् वर्ग vvp_darray_atom<uint16_t>;
प्रतिरूपम् वर्ग vvp_darray_atom<uint32_t>;
प्रतिरूपम् वर्ग vvp_darray_atom<uint64_t>;
प्रतिरूपम् वर्ग vvp_darray_atom<int8_t>;
प्रतिरूपम् वर्ग vvp_darray_atom<int16_t>;
प्रतिरूपम् वर्ग vvp_darray_atom<int32_t>;
प्रतिरूपम् वर्ग vvp_darray_atom<int64_t>;

vvp_darray_vec4::~vvp_darray_vec4()
{
}

size_t vvp_darray_vec4::get_आकार(शून्य) ध्रुव
{
      प्रत्यागम array_.size();
}

शून्य vvp_darray_vec4::समुच्चयः_word(अचिह्नित adr, ध्रुव vvp_vector4_t&value)
{
      यदि (adr >= array_.size()) प्रत्यागम;
      प्रतिज्ञा(value.size() == word_wid_);
      array_[adr] = value;
}

शून्य vvp_darray_vec4::get_word(अचिह्नित adr, vvp_vector4_t&value)
{
	/*
	 * Return an undefined value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      यदि ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_X);
	    प्रत्यागम;
      }
      value = array_[adr];
      प्रतिज्ञा(value.size() == word_wid_);
}

शून्य vvp_darray_vec4::shallow_copy(ध्रुव vvp_object*obj)
{
      ध्रुव vvp_darray_vec4*that = dynamic_cast<ध्रुव vvp_darray_vec4*>(obj);
      प्रतिज्ञा(that);

      अचिह्नित अङ्कः_वस्तूनि = min(array_.size(), that->array_.size());
      प्रति (अचिह्नित idx = 0 ; idx < अङ्कः_वस्तूनि ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_object* vvp_darray_vec4::duplicate(शून्य) ध्रुव
{
      vvp_darray_vec4*that = नूतन vvp_darray_vec4(array_.size(), word_wid_);

      प्रति (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      प्रत्यागम that;
}

vvp_vector4_t vvp_darray_vec4::get_bitstream(तार्किक as_vec4)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      अचिह्नित adx = 0;
      अचिह्नित vdx = vec.size();
      यावत् (vdx > 0) {
            vdx -= word_wid_;
            प्रति (अचिह्नित bdx = 0; bdx < word_wid_; bdx += 1) {
                  vvp_bit4_t bit = array_[adx].value(bdx);
                  यदि (as_vec4 || (bit == BIT4_1))
                        vec.समुच्चयः_bit(vdx+bdx, bit);
            }
            adx++;
      }

      प्रत्यागम vec;
}

शून्य vvp_darray_vec4::reverse_elems(शून्य)
{
      size_t n = array_.size();
      प्रति (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

शून्य vvp_darray_vec4::क्रमय_elems(तार्किक ascending)
{
      यदि (ascending) {
	    std::sort(array_.begin(), array_.अन्तः(), vec4_lt_unsigned);
      } अन्यथा {
	    std::sort(array_.begin(), array_.अन्तः(),
		      [](ध्रुव vvp_vector4_t&a, ध्रुव vvp_vector4_t&b) {
			    प्रत्यागम vec4_lt_unsigned(b, a);
		      });
      }
}

शून्य vvp_darray_vec4::shuffle_elems(शून्य)
{
      std::shuffle(array_.begin(), array_.अन्तः(), shuffle_rng());
}

शून्य vvp_darray_vec2::reverse_elems(शून्य)
{
      size_t n = array_.size();
      प्रति (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

शून्य vvp_darray_vec2::क्रमय_elems(तार्किक ascending)
{
      यदि (ascending) {
	    std::sort(array_.begin(), array_.अन्तः());
      } अन्यथा {
	    std::sort(array_.begin(), array_.अन्तः(),
		      [](ध्रुव vvp_vector2_t&a, ध्रुव vvp_vector2_t&b) {
			    प्रत्यागम b < a;
		      });
      }
}

शून्य vvp_darray_vec2::shuffle_elems(शून्य)
{
      std::shuffle(array_.begin(), array_.अन्तः(), shuffle_rng());
}

vvp_darray_vec2::~vvp_darray_vec2()
{
}

size_t vvp_darray_vec2::get_आकार(शून्य) ध्रुव
{
      प्रत्यागम array_.size();
}

शून्य vvp_darray_vec2::समुच्चयः_word(अचिह्नित adr, ध्रुव vvp_vector4_t&value)
{
      यदि (adr >= array_.size()) प्रत्यागम;
      प्रतिज्ञा(value.size() == word_wid_);
      array_[adr] = value;
}

शून्य vvp_darray_vec2::get_word(अचिह्नित adr, vvp_vector4_t&value)
{
	/*
	 * Return a zero value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      यदि ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_0);
	    प्रत्यागम;
      }
      प्रतिज्ञा(array_[adr].size() == word_wid_);
      value.resize(word_wid_);
      प्रति (अचिह्नित idx = 0; idx < word_wid_; idx += 1) {
	    value.समुच्चयः_bit(idx, array_[adr].value4(idx));
      }
}

शून्य vvp_darray_vec2::shallow_copy(ध्रुव vvp_object*obj)
{
      ध्रुव vvp_darray_vec2*that = dynamic_cast<ध्रुव vvp_darray_vec2*>(obj);
      प्रतिज्ञा(that);

      अचिह्नित अङ्कः_वस्तूनि = min(array_.size(), that->array_.size());
      प्रति (अचिह्नित idx = 0 ; idx < अङ्कः_वस्तूनि ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_vector4_t vvp_darray_vec2::get_bitstream(तार्किक)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      अचिह्नित adx = 0;
      अचिह्नित vdx = vec.size();
      यावत् (vdx > 0) {
            vdx -= word_wid_;
            प्रति (अचिह्नित bdx = 0; bdx < word_wid_; bdx += 1) {
                  यदि (array_[adx].value(bdx))
                        vec.समुच्चयः_bit(vdx+bdx, BIT4_1);
            }
            adx++;
      }

      प्रत्यागम vec;
}

vvp_darray_object::~vvp_darray_object()
{
}

size_t vvp_darray_object::get_आकार() ध्रुव
{
      प्रत्यागम array_.size();
}

शून्य vvp_darray_object::समुच्चयः_word(अचिह्नित adr, ध्रुव vvp_object_t&value)
{
      यदि (adr >= array_.size())
	    प्रत्यागम;
      array_[adr] = value;
}

शून्य vvp_darray_object::get_word(अचिह्नित adr, vvp_object_t&value)
{
      यदि (adr >= array_.size()) {
	    value = vvp_object_t();
	    प्रत्यागम;
      }

      value = array_[adr];
}

शून्य vvp_darray_object::shallow_copy(ध्रुव vvp_object*obj)
{
      ध्रुव vvp_darray_object*that = dynamic_cast<ध्रुव vvp_darray_object*>(obj);
      प्रतिज्ञा(that);

      अचिह्नित अङ्कः_वस्तूनि = min(array_.size(), that->array_.size());
      प्रति (अचिह्नित idx = 0 ; idx < अङ्कः_वस्तूनि ; idx += 1)
	    array_[idx] = that->array_[idx];
}

शून्य vvp_darray_object::reverse_elems(शून्य)
{
      size_t n = array_.size();
      प्रति (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

शून्य vvp_darray_object::क्रमय_elems(तार्किक ascending)
{
      यदि (ascending) {
	    std::sort(array_.begin(), array_.अन्तः(),
		      [](ध्रुव vvp_object_t&a, ध्रुव vvp_object_t&b) {
			    प्रत्यागम a.क्रमय_क्रमः_कुञ्चिका() < b.क्रमय_क्रमः_कुञ्चिका();
		      });
      } अन्यथा {
	    std::sort(array_.begin(), array_.अन्तः(),
		      [](ध्रुव vvp_object_t&a, ध्रुव vvp_object_t&b) {
			    प्रत्यागम a.क्रमय_क्रमः_कुञ्चिका() > b.क्रमय_क्रमः_कुञ्चिका();
		      });
      }
}

शून्य vvp_darray_object::shuffle_elems(शून्य)
{
      std::shuffle(array_.begin(), array_.अन्तः(), shuffle_rng());
}

vvp_darray_real::~vvp_darray_real()
{
}

size_t vvp_darray_real::get_आकार() ध्रुव
{
      प्रत्यागम array_.size();
}

शून्य vvp_darray_real::समुच्चयः_word(अचिह्नित adr, द्विगुण value)
{
      यदि (adr >= array_.size())
	    प्रत्यागम;
      array_[adr] = value;
}

शून्य vvp_darray_real::get_word(अचिह्नित adr, द्विगुण&value)
{
      यदि (adr >= array_.size()) {
	    value = 0.0;
	    प्रत्यागम;
      }

      value = array_[adr];
}

शून्य vvp_darray_real::shallow_copy(ध्रुव vvp_object*obj)
{
      ध्रुव vvp_darray_real*that = dynamic_cast<ध्रुव vvp_darray_real*>(obj);
      प्रतिज्ञा(that);

      अचिह्नित अङ्कः_वस्तूनि = min(array_.size(), that->array_.size());
      प्रति (अचिह्नित idx = 0 ; idx < अङ्कः_वस्तूनि ; idx += 1)
	    array_[idx] = that->array_[idx];
}

शून्य vvp_darray_real::reverse_elems(शून्य)
{
      size_t n = array_.size();
      प्रति (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

शून्य vvp_darray_real::क्रमय_elems(तार्किक ascending)
{
      यदि (ascending) std::sort(array_.begin(), array_.अन्तः());
      अन्यथा std::sort(array_.begin(), array_.अन्तः(), std::greater<द्विगुण>());
}

शून्य vvp_darray_real::shuffle_elems(शून्य)
{
      std::shuffle(array_.begin(), array_.अन्तः(), shuffle_rng());
}

vvp_object* vvp_darray_real::duplicate(शून्य) ध्रुव
{
      vvp_darray_real*that = नूतन vvp_darray_real(array_.size());

      प्रति (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      प्रत्यागम that;
}

vvp_vector4_t vvp_darray_real::get_bitstream(तार्किक)
{
      ध्रुव अचिह्नित word_wid = परिमाण(द्विगुण) * 8;
      प्रतिज्ञा(word_wid == 64);

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      अचिह्नित adx = 0;
      अचिह्नित vdx = vec.size();
      यावत् (vdx > 0) {
            सङ्घ {
                द्विगुण   value;
                uint64_t द्वयङ्काः;
            } word;
            word.value = array_[adx++];
            vdx -= word_wid;
            प्रति (अचिह्नित bdx = 0; bdx < word_wid; bdx += 1) {
                  यदि (word.द्वयङ्काः & 1)
                        vec.समुच्चयः_bit(vdx+bdx, BIT4_1);
                  word.द्वयङ्काः >>= 1;
            }
      }

      प्रत्यागम vec;
}

vvp_darray_string::~vvp_darray_string()
{
}

size_t vvp_darray_string::get_आकार() ध्रुव
{
      प्रत्यागम array_.size();
}

शून्य vvp_darray_string::समुच्चयः_word(अचिह्नित adr, ध्रुव string&value)
{
      यदि (adr >= array_.size())
	    प्रत्यागम;
      array_[adr] = value;
}

शून्य vvp_darray_string::get_word(अचिह्नित adr, string&value)
{
      यदि (adr >= array_.size()) {
	    value = "";
	    प्रत्यागम;
      }

      value = array_[adr];
}

शून्य vvp_darray_string::shallow_copy(ध्रुव vvp_object*obj)
{
      ध्रुव vvp_darray_string*that = dynamic_cast<ध्रुव vvp_darray_string*>(obj);
      प्रतिज्ञा(that);

      अचिह्नित अङ्कः_वस्तूनि = min(array_.size(), that->array_.size());
      प्रति (अचिह्नित idx = 0 ; idx < अङ्कः_वस्तूनि ; idx += 1)
	    array_[idx] = that->array_[idx];
}

शून्य vvp_darray_string::reverse_elems(शून्य)
{
      size_t n = array_.size();
      प्रति (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

शून्य vvp_darray_string::क्रमय_elems(तार्किक ascending)
{
      यदि (ascending) std::sort(array_.begin(), array_.अन्तः());
      अन्यथा std::sort(array_.begin(), array_.अन्तः(), std::greater<string>());
}

शून्य vvp_darray_string::shuffle_elems(शून्य)
{
      std::shuffle(array_.begin(), array_.अन्तः(), shuffle_rng());
}

vvp_object* vvp_darray_string::duplicate(शून्य) ध्रुव
{
      vvp_darray_string*that = नूतन vvp_darray_string(array_.size());

      प्रति (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      प्रत्यागम that;
}

vvp_queue::~vvp_queue()
{
}

शून्य vvp_queue::copy_elems(vvp_object_t, अचिह्नित)
{
      cerr << "Sorry: copy_elems() not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_queue::समुच्चयः_word_max(अचिह्नित, ध्रुव vvp_vector4_t&, अचिह्नित)
{
      cerr << "XXXX set_word_max(vvp_vector4_t) not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_queue::insert(अचिह्नित, ध्रुव vvp_vector4_t&, अचिह्नित)
{
      cerr << "XXXX insert(vvp_vector4_t) not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_queue::push_back(ध्रुव vvp_vector4_t&, अचिह्नित)
{
      cerr << "XXXX push_back(vvp_vector4_t) not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_queue::push_front(ध्रुव vvp_vector4_t&, अचिह्नित)
{
      cerr << "XXXX push_front(vvp_vector4_t) not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_queue::समुच्चयः_word_max(अचिह्नित, द्विगुण, अचिह्नित)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_queue::insert(अचिह्नित, द्विगुण, अचिह्नित)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_queue::push_back(द्विगुण, अचिह्नित)
{
      cerr << "XXXX push_back(double) not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_queue::push_front(द्विगुण, अचिह्नित)
{
      cerr << "XXXX push_front(double) not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_queue::समुच्चयः_word_max(अचिह्नित, ध्रुव string&, अचिह्नित)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_queue::insert(अचिह्नित, ध्रुव string&, अचिह्नित)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_queue::push_back(ध्रुव string&, अचिह्नित)
{
      cerr << "XXXX push_back(string) not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

शून्य vvp_queue::push_front(ध्रुव string&, अचिह्नित)
{
      cerr << "XXXX push_front(string) not implemented for " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

/*
 * Helper functions used while copying multiple elements into a queue.
 */
स्थिर शून्य print_copy_is_too_big(size_t src_आकार, अचिह्नित max_आकार, ध्रुव string&qtype)
{
      cerr << get_fileline()
           << "Warning: queue<" << qtype << "> is bounded to have at most "
           << max_आकार << " elements, source has " << src_आकार << " elements." << पङ्क्त्यन्तः;
}

स्थिर शून्य print_copy_is_too_big(द्विगुण&, size_t src_आकार, अचिह्नित max_आकार)
{
      print_copy_is_too_big(src_आकार, max_आकार, "real");
}

स्थिर शून्य print_copy_is_too_big(string&, size_t src_आकार, अचिह्नित max_आकार)
{
      print_copy_is_too_big(src_आकार, max_आकार, "string");
}

स्थिर शून्य print_copy_is_too_big(vvp_vector4_t&, size_t src_आकार, अचिह्नित max_आकार)
{
      print_copy_is_too_big(src_आकार, max_आकार, "vector");
}

प्रतिरूपम् <प्रकारनाम ELEM, वर्ग QTYPE, वर्ग SRC_TYPE>
स्थिर शून्य copy_elements(QTYPE*queue, SRC_TYPE*src, अचिह्नित max_आकार)
{
      size_t src_आकार = src->get_आकार();
      यदि ((max_आकार != 0) && (src_आकार > max_आकार)) {
	    ELEM tmp;
	    print_copy_is_too_big(tmp, src_आकार, max_आकार);
      }
      अचिह्नित copy_आकार = ((src_आकार < max_आकार) ||
                            (max_आकार == 0)) ? src_आकार : max_आकार;
      यदि (copy_आकार < queue->get_आकार())
	    queue->erase_पुच्छ(copy_आकार);
      प्रति (अचिह्नित idx=0; idx < copy_आकार; ++idx) {
	    ELEM value;
	    src->get_word(idx, value);
	    queue->समुच्चयः_word_max(idx, value, max_आकार);
      }
}

शून्य vvp_queue_real::copy_elems(vvp_object_t src, अचिह्नित max_आकार)
{
      यदि (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<द्विगुण, vvp_queue_real, vvp_queue>(एतत्, src_queue, max_आकार);
      अन्यथा यदि (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<द्विगुण, vvp_queue_real, vvp_darray>(एतत्, src_darray, max_आकार);
      अन्यथा
	    cerr << get_fileline() << "Sorry: cannot copy object to real queue." << पङ्क्त्यन्तः;
}

शून्य vvp_queue_real::समुच्चयः_word_max(अचिह्नित adr, द्विगुण value, अचिह्नित max_आकार)
{
      यदि (adr == queue.size())
	    यदि (!max_आकार || (queue.size() < max_आकार))
		  queue.push_back(value);
	    अन्यथा
		  cerr << get_fileline()
		       << "Warning: assigning to queue<real>[" << adr << "] is"
		          " outside bound (" << max_आकार << "). " << value
		       << " was not added." << पङ्क्त्यन्तः;
      अन्यथा
	    समुच्चयः_word(adr, value);
}

शून्य vvp_queue_real::समुच्चयः_word(अचिह्नित adr, द्विगुण value)
{
      यदि (adr < queue.size())
	    queue[adr] = value;
      अन्यथा
	    cerr << get_fileline()
	         << "Warning: assigning to queue<real>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << पङ्क्त्यन्तः;
}

शून्य vvp_queue_real::get_word(अचिह्नित adr, द्विगुण&value)
{
      यदि (adr >= queue.size())
	    value = 0.0;
      अन्यथा
	    value = queue[adr];
}

शून्य vvp_queue_real::insert(अचिह्नित idx, द्विगुण value, अचिह्नित max_आकार)
{
	// Inserting past the end of the queue
      यदि (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<real>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). " << value
	         << " was not added." << पङ्क्त्यन्तः;
	// Inserting at the end
      अन्यथा यदि (idx == queue.size())
	    यदि (!max_आकार || (queue.size() < max_आकार))
		  queue.push_back(value);
	    अन्यथा
		  cerr << get_fileline()
		       << "Warning: inserting to queue<real>[" << idx << "] is"
		          " outside bound (" << max_आकार << "). " << value
		       << " was not added." << पङ्क्त्यन्तः;
      अन्यथा  {
	    यदि (max_आकार && (queue.size() == max_आकार)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<real> ["
		       << max_आकार << "]." << पङ्क्त्यन्तः;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

शून्य vvp_queue_real::push_back(द्विगुण value, अचिह्नित max_आकार)
{
      यदि (!max_आकार || (queue.size() < max_आकार))
	    queue.push_back(value);
      अन्यथा
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<real> ["
	         << max_आकार << "]." << पङ्क्त्यन्तः;
}

शून्य vvp_queue_real::push_front(द्विगुण value, अचिह्नित max_आकार)
{
      यदि (max_आकार && (queue.size() == max_आकार)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<real> ["
	         << max_आकार << "]." << पङ्क्त्यन्तः;
	    queue.pop_back();
      }
      queue.push_front(value);
}

शून्य vvp_queue_real::erase(अचिह्नित idx)
{
      प्रतिज्ञा(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

शून्य vvp_queue_real::erase_पुच्छ(अचिह्नित idx)
{
      प्रतिज्ञा(queue.size() >= idx);
      यदि (queue.size() > idx)
	    queue.resize(idx);
}

शून्य vvp_queue_string::copy_elems(vvp_object_t src, अचिह्नित max_आकार)
{
      यदि (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<string, vvp_queue_string, vvp_queue>(एतत्, src_queue, max_आकार);
      अन्यथा यदि (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<string, vvp_queue_string, vvp_darray>(एतत्, src_darray, max_आकार);
      अन्यथा
	    cerr << get_fileline() << "Sorry: cannot copy object to string queue." << पङ्क्त्यन्तः;
}

शून्य vvp_queue_string::समुच्चयः_word_max(अचिह्नित adr, ध्रुव string&value, अचिह्नित max_आकार)
{
      यदि (adr == queue.size())
	    यदि (!max_आकार || (queue.size() < max_आकार))
		  queue.push_back(value);
	    अन्यथा
		  cerr << get_fileline()
		       << "Warning: assigning to queue<string>[" << adr << "] is"
		          " outside bound (" << max_आकार << "). \"" << value
		       << "\" was not added." << पङ्क्त्यन्तः;
      अन्यथा
	    समुच्चयः_word(adr, value);
}

शून्य vvp_queue_string::समुच्चयः_word(अचिह्नित adr, ध्रुव string&value)
{
      यदि (adr < queue.size())
	    queue[adr] = value;
      अन्यथा
	    cerr << get_fileline()
	         << "Warning: assigning to queue<string>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << पङ्क्त्यन्तः;
}

शून्य vvp_queue_string::get_word(अचिह्नित adr, string&value)
{
      यदि (adr >= queue.size())
	    value = "";
      अन्यथा
	    value = queue[adr];
}

शून्य vvp_queue_string::insert(अचिह्नित idx, ध्रुव string&value, अचिह्नित max_आकार)
{
	// Inserting past the end of the queue
      यदि (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<string>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << पङ्क्त्यन्तः;
	// Inserting at the end
      अन्यथा यदि (idx == queue.size())
	    यदि (!max_आकार || (queue.size() < max_आकार))
		  queue.push_back(value);
	    अन्यथा
		  cerr << get_fileline()
		       << "Warning: inserting to queue<string>[" << idx << "] is"
		          " outside bound (" << max_आकार << "). \"" << value
		       << "\" was not added." << पङ्क्त्यन्तः;
      अन्यथा  {
	    यदि (max_आकार && (queue.size() == max_आकार)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", \"" << value << "\") removed \""
		       << queue.back() << "\" from already full bounded queue<string> ["
		       << max_आकार << "]." << पङ्क्त्यन्तः;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

शून्य vvp_queue_string::push_back(ध्रुव string&value, अचिह्नित max_आकार)
{
      यदि (!max_आकार || (queue.size() < max_आकार))
	    queue.push_back(value);
      अन्यथा
	    cerr << get_fileline()
	         << "Warning: push_back(\"" << value
	         << "\") skipped for already full bounded queue<string> ["
	         << max_आकार << "]." << पङ्क्त्यन्तः;
}

शून्य vvp_queue_string::push_front(ध्रुव string&value, अचिह्नित max_आकार)
{
      यदि (max_आकार && (queue.size() == max_आकार)) {
	    cerr << get_fileline()
	         << "Warning: push_front(\"" << value << "\") removed \""
	         << queue.back() << "\" from already full bounded queue<string> ["
	         << max_आकार << "]." << पङ्क्त्यन्तः;
	    queue.pop_back();
      }
      queue.push_front(value);
}

शून्य vvp_queue_string::erase(अचिह्नित idx)
{
      प्रतिज्ञा(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

शून्य vvp_queue_string::erase_पुच्छ(अचिह्नित idx)
{
      प्रतिज्ञा(queue.size() >= idx);
      यदि (queue.size() > idx)
	    queue.resize(idx);
}

शून्य vvp_queue_real::reverse_elems(शून्य)
{
      std::reverse(queue.begin(), queue.अन्तः());
}

शून्य vvp_queue_string::reverse_elems(शून्य)
{
      std::reverse(queue.begin(), queue.अन्तः());
}

शून्य vvp_queue_real::क्रमय_elems(तार्किक ascending)
{
      यदि (ascending) std::sort(queue.begin(), queue.अन्तः());
      अन्यथा std::sort(queue.begin(), queue.अन्तः(), std::greater<द्विगुण>());
}

शून्य vvp_queue_real::shuffle_elems(शून्य)
{
      std::shuffle(queue.begin(), queue.अन्तः(), shuffle_rng());
}

शून्य vvp_queue_string::क्रमय_elems(तार्किक ascending)
{
      यदि (ascending) std::sort(queue.begin(), queue.अन्तः());
      अन्यथा std::sort(queue.begin(), queue.अन्तः(), std::greater<string>());
}

शून्य vvp_queue_string::shuffle_elems(शून्य)
{
      std::shuffle(queue.begin(), queue.अन्तः(), shuffle_rng());
}

शून्य vvp_queue_vec4::copy_elems(vvp_object_t src, अचिह्नित max_आकार)
{
      यदि (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_queue>(एतत्, src_queue, max_आकार);
      अन्यथा यदि (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_darray>(एतत्, src_darray, max_आकार);
      अन्यथा
	    cerr << get_fileline() << "Sorry: cannot copy object to vector queue." << पङ्क्त्यन्तः;
}

शून्य vvp_queue_vec4::समुच्चयः_word_max(अचिह्नित adr, ध्रुव vvp_vector4_t&value, अचिह्नित max_आकार)
{
      यदि (adr == queue.size())
	    यदि (!max_आकार || (queue.size() < max_आकार))
		  queue.push_back(value);
	    अन्यथा
		  cerr << get_fileline()
		       << "Warning: assigning to queue<vector>[" << adr << "] is"
		          " outside bound (" << max_आकार << "). " << value
		       << " was not added." << पङ्क्त्यन्तः;
      अन्यथा
	    समुच्चयः_word(adr, value);
}

शून्य vvp_queue_vec4::समुच्चयः_word(अचिह्नित adr, ध्रुव vvp_vector4_t&value)
{
      यदि (adr < queue.size())
	    queue[adr] = value;
      अन्यथा
	    cerr << get_fileline()
	         << "Warning: assigning to queue<vector>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << पङ्क्त्यन्तः;
}

शून्य vvp_queue_vec4::get_word(अचिह्नित adr, vvp_vector4_t&value)
{
      यदि (adr >= queue.size())
	    value = vvp_vector4_t(queue[0].size());
      अन्यथा
	    value = queue[adr];
}

शून्य vvp_queue_vec4::insert(अचिह्नित idx, ध्रुव vvp_vector4_t&value, अचिह्नित max_आकार)
{
	// Inserting past the end of the queue
      यदि (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<vector[" << value.size()
	         << "]>[" << idx << "] is outside of size (" << queue.size()
	         << "). " << value << " was not added." << पङ्क्त्यन्तः;
	// Inserting at the end
      अन्यथा यदि (idx == queue.size())
	    यदि (!max_आकार || (queue.size() < max_आकार))
		  queue.push_back(value);
	    अन्यथा
		  cerr << get_fileline()
		       << "Warning: inserting to queue<vector[" << value.size()
		       << "]>[" << idx << "] is outside bound (" << max_आकार
		       << "). " << value << " was not added." << पङ्क्त्यन्तः;
      अन्यथा  {
	    यदि (max_आकार && (queue.size() == max_आकार)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<vector["
		       << value.size() << "]> [" << max_आकार << "]." << पङ्क्त्यन्तः;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

शून्य vvp_queue_vec4::push_back(ध्रुव vvp_vector4_t&value, अचिह्नित max_आकार)
{
      यदि (!max_आकार || (queue.size() < max_आकार))
	    queue.push_back(value);
      अन्यथा
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<vector["
	         << value.size() << "]> [" << max_आकार << "]." << पङ्क्त्यन्तः;
}

शून्य vvp_queue_vec4::push_front(ध्रुव vvp_vector4_t&value, अचिह्नित max_आकार)
{
      यदि (max_आकार && (queue.size() == max_आकार)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<vector["
	         << value.size() << "]> [" << max_आकार << "]." << पङ्क्त्यन्तः;
	    queue.pop_back();
      }
      queue.push_front(value);
}

शून्य vvp_queue_vec4::erase(अचिह्नित idx)
{
      प्रतिज्ञा(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

शून्य vvp_queue_vec4::erase_पुच्छ(अचिह्नित idx)
{
      प्रतिज्ञा(queue.size() >= idx);
      यदि (queue.size() > idx)
	    queue.resize(idx);
}

शून्य vvp_queue_vec4::reverse_elems(शून्य)
{
      std::reverse(queue.begin(), queue.अन्तः());
}

शून्य vvp_queue_vec4::क्रमय_elems(तार्किक ascending)
{
      यदि (ascending) {
	    std::sort(queue.begin(), queue.अन्तः(), vec4_lt_unsigned);
      } अन्यथा {
	    std::sort(queue.begin(), queue.अन्तः(),
		      [](ध्रुव vvp_vector4_t&a, ध्रुव vvp_vector4_t&b) {
			    प्रत्यागम vec4_lt_unsigned(b, a);
		      });
      }
}

शून्य vvp_queue_vec4::shuffle_elems(शून्य)
{
      std::shuffle(queue.begin(), queue.अन्तः(), shuffle_rng());
}
