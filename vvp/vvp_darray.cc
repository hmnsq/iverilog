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

استعمال نامخلا std;

نامخلا {

ساکن std::mt19937& shuffle_rng(خالی)
{
      ساکن std::mt19937 rng(std::random_device{}());
      واپس rng;
}

ساکن منطقی vec4_lt_unsigned(ثابت vvp_vector4_t&a, ثابت vvp_vector4_t&b)
{
      اگر (a.eeq(b)) واپس جھوٹ;
      vvp_bit4_t gt = compare_gtge(b, a, BIT4_1);
      اگر (gt == BIT4_X) واپس جھوٹ;
      واپس gt == BIT4_1;
}

} // namespace

vvp_darray::~vvp_darray()
{
}

خالی vvp_darray::الٹ_elems(خالی)
{
      cerr << "XXXX reverse_elems() not implemented for "
           << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_darray::ترتیب_دو_elems(منطقی /*ascending*/)
{
      cerr << "XXXX sort_elems() not implemented for "
           << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_darray::shuffle_elems(خالی)
{
      cerr << "XXXX shuffle_elems() not implemented for "
           << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_darray::سیٹ_لفظ(غیرعلامتی, ثابت vvp_vector4_t&)
{
      cerr << "XXXX set_word(vvp_vector4_t) not implemented for " << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_darray::سیٹ_لفظ(غیرعلامتی, دوہرا)
{
      cerr << "XXXX set_word(double) not implemented for " << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_darray::سیٹ_لفظ(غیرعلامتی, ثابت string&)
{
      cerr << "XXXX set_word(string) not implemented for " << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_darray::سیٹ_لفظ(غیرعلامتی, ثابت vvp_object_t&)
{
      cerr << "XXXX set_word(vvp_object_t) not implemented for " << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_darray::get_لفظ(غیرعلامتی, vvp_vector4_t&)
{
      cerr << "XXXX get_word(vvp_vector4_t) not implemented for " << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_darray::get_لفظ(غیرعلامتی, دوہرا&)
{
      cerr << "XXXX get_word(double) not implemented for " << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_darray::get_لفظ(غیرعلامتی, string&)
{
      cerr << "XXXX get_word(string) not implemented for " << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_darray::get_لفظ(غیرعلامتی, vvp_object_t&)
{
      cerr << "XXXX get_word(vvp_object_t) not implemented for " << typeid(*یہ).name() << سطر_ختم;
}

vvp_vector4_t vvp_darray::get_bitstream(منطقی)
{
      cerr << "XXXX get_bitstream() not implemented for " << typeid(*یہ).name() << سطر_ختم;
      واپس vvp_vector4_t();
}

سانچہ <جماعت TYPE> vvp_darray_atom<TYPE>::~vvp_darray_atom()
{
}

سانچہ <جماعت TYPE> size_t vvp_darray_atom<TYPE>::get_سائز() ثابت
{
      واپس array_.size();
}

سانچہ <جماعت TYPE> خالی vvp_darray_atom<TYPE>::الٹ_elems(خالی)
{
      size_t n = array_.size();
      برائے (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

سانچہ <جماعت TYPE> خالی vvp_darray_atom<TYPE>::ترتیب_دو_elems(منطقی ascending)
{
      اگر (ascending) std::sort(array_.begin(), array_.انتہا());
      ورنہ std::sort(array_.begin(), array_.انتہا(), std::greater<TYPE>());
}

سانچہ <جماعت TYPE> خالی vvp_darray_atom<TYPE>::shuffle_elems(خالی)
{
      std::shuffle(array_.begin(), array_.انتہا(), shuffle_rng());
}

سانچہ <جماعت TYPE> خالی vvp_darray_atom<TYPE>::سیٹ_لفظ(غیرعلامتی adr, ثابت vvp_vector4_t&value)
{
      اگر (adr >= array_.size())
	    واپس;
      TYPE tmp;
      vector4_مقصد_قدر(value, tmp, سچ, جھوٹ);
      array_[adr] = tmp;
}

سانچہ <جماعت TYPE> خالی vvp_darray_atom<TYPE>::get_لفظ(غیرعلامتی adr, vvp_vector4_t&value)
{
      اگر (adr >= array_.size()) {
	    value = vvp_vector4_t(8*حجم(TYPE), BIT4_X);
	    واپس;
      }

      TYPE لفظ = array_[adr];
      vvp_vector4_t tmp (8*حجم(TYPE), BIT4_0);
      برائے (غیرعلامتی idx = 0 ; idx < tmp.size() ; idx += 1) {
	    اگر (لفظ&1) tmp.سیٹ_ثنائی_ہندسہ(idx, BIT4_1);
	    لفظ >>= 1;
      }
      value = tmp;
}

سانچہ <جماعت TYPE> خالی vvp_darray_atom<TYPE>::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_atom<TYPE>*that = dynamic_cast<ثابت vvp_darray_atom<TYPE>*>(obj);
      أكد(that);

      غیرعلامتی عدد_اشیاء = min(array_.size(), that->array_.size());
      برائے (غیرعلامتی idx = 0 ; idx < عدد_اشیاء ; idx += 1)
	    array_[idx] = that->array_[idx];
}

سانچہ <جماعت TYPE> vvp_object* vvp_darray_atom<TYPE>::duplicate(خالی) ثابت
{
      vvp_darray_atom<TYPE>*that = نیا vvp_darray_atom<TYPE>(array_.size());
      برائے (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      واپس that;
}

سانچہ <جماعت TYPE> vvp_vector4_t vvp_darray_atom<TYPE>::get_bitstream(منطقی)
{
      ثابت غیرعلامتی لفظ_wid = حجم(TYPE) * 8;

      vvp_vector4_t vec(array_.size() * لفظ_wid, BIT4_0);

      غیرعلامتی adx = 0;
      غیرعلامتی vdx = vec.size();
      جب (vdx > 0) {
            TYPE لفظ = array_[adx++];
            vdx -= لفظ_wid;
            برائے (غیرعلامتی bdx = 0; bdx < لفظ_wid; bdx += 1) {
                  اگر (لفظ & 1)
                        vec.سیٹ_ثنائی_ہندسہ(vdx+bdx, BIT4_1);
                  لفظ >>= 1;
            }
      }

      واپس vec;
}

سانچہ جماعت vvp_darray_atom<uint8_t>;
سانچہ جماعت vvp_darray_atom<uint16_t>;
سانچہ جماعت vvp_darray_atom<uint32_t>;
سانچہ جماعت vvp_darray_atom<uint64_t>;
سانچہ جماعت vvp_darray_atom<int8_t>;
سانچہ جماعت vvp_darray_atom<int16_t>;
سانچہ جماعت vvp_darray_atom<int32_t>;
سانچہ جماعت vvp_darray_atom<int64_t>;

vvp_darray_vec4::~vvp_darray_vec4()
{
}

size_t vvp_darray_vec4::get_سائز(خالی) ثابت
{
      واپس array_.size();
}

خالی vvp_darray_vec4::سیٹ_لفظ(غیرعلامتی adr, ثابت vvp_vector4_t&value)
{
      اگر (adr >= array_.size()) واپس;
      أكد(value.size() == لفظ_wid_2);
      array_[adr] = value;
}

خالی vvp_darray_vec4::get_لفظ(غیرعلامتی adr, vvp_vector4_t&value)
{
	/*
	 * Return an undefined value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      اگر ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(لفظ_wid_2, BIT4_X);
	    واپس;
      }
      value = array_[adr];
      أكد(value.size() == لفظ_wid_2);
}

خالی vvp_darray_vec4::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_vec4*that = dynamic_cast<ثابت vvp_darray_vec4*>(obj);
      أكد(that);

      غیرعلامتی عدد_اشیاء = min(array_.size(), that->array_.size());
      برائے (غیرعلامتی idx = 0 ; idx < عدد_اشیاء ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_object* vvp_darray_vec4::duplicate(خالی) ثابت
{
      vvp_darray_vec4*that = نیا vvp_darray_vec4(array_.size(), لفظ_wid_2);

      برائے (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      واپس that;
}

vvp_vector4_t vvp_darray_vec4::get_bitstream(منطقی as_vec4)
{
      vvp_vector4_t vec(array_.size() * لفظ_wid_2, BIT4_0);

      غیرعلامتی adx = 0;
      غیرعلامتی vdx = vec.size();
      جب (vdx > 0) {
            vdx -= لفظ_wid_2;
            برائے (غیرعلامتی bdx = 0; bdx < لفظ_wid_2; bdx += 1) {
                  vvp_bit4_t ثنائی_ہندسہ = array_[adx].value(bdx);
                  اگر (as_vec4 || (ثنائی_ہندسہ == BIT4_1))
                        vec.سیٹ_ثنائی_ہندسہ(vdx+bdx, ثنائی_ہندسہ);
            }
            adx++;
      }

      واپس vec;
}

خالی vvp_darray_vec4::الٹ_elems(خالی)
{
      size_t n = array_.size();
      برائے (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

خالی vvp_darray_vec4::ترتیب_دو_elems(منطقی ascending)
{
      اگر (ascending) {
	    std::sort(array_.begin(), array_.انتہا(), vec4_lt_unsigned);
      } ورنہ {
	    std::sort(array_.begin(), array_.انتہا(),
		      [](ثابت vvp_vector4_t&a, ثابت vvp_vector4_t&b) {
			    واپس vec4_lt_unsigned(b, a);
		      });
      }
}

خالی vvp_darray_vec4::shuffle_elems(خالی)
{
      std::shuffle(array_.begin(), array_.انتہا(), shuffle_rng());
}

خالی vvp_darray_vec2::الٹ_elems(خالی)
{
      size_t n = array_.size();
      برائے (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

خالی vvp_darray_vec2::ترتیب_دو_elems(منطقی ascending)
{
      اگر (ascending) {
	    std::sort(array_.begin(), array_.انتہا());
      } ورنہ {
	    std::sort(array_.begin(), array_.انتہا(),
		      [](ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b) {
			    واپس b < a;
		      });
      }
}

خالی vvp_darray_vec2::shuffle_elems(خالی)
{
      std::shuffle(array_.begin(), array_.انتہا(), shuffle_rng());
}

vvp_darray_vec2::~vvp_darray_vec2()
{
}

size_t vvp_darray_vec2::get_سائز(خالی) ثابت
{
      واپس array_.size();
}

خالی vvp_darray_vec2::سیٹ_لفظ(غیرعلامتی adr, ثابت vvp_vector4_t&value)
{
      اگر (adr >= array_.size()) واپس;
      أكد(value.size() == لفظ_wid_2);
      array_[adr] = value;
}

خالی vvp_darray_vec2::get_لفظ(غیرعلامتی adr, vvp_vector4_t&value)
{
	/*
	 * Return a zero value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      اگر ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(لفظ_wid_2, BIT4_0);
	    واپس;
      }
      أكد(array_[adr].size() == لفظ_wid_2);
      value.resize(لفظ_wid_2);
      برائے (غیرعلامتی idx = 0; idx < لفظ_wid_2; idx += 1) {
	    value.سیٹ_ثنائی_ہندسہ(idx, array_[adr].value4(idx));
      }
}

خالی vvp_darray_vec2::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_vec2*that = dynamic_cast<ثابت vvp_darray_vec2*>(obj);
      أكد(that);

      غیرعلامتی عدد_اشیاء = min(array_.size(), that->array_.size());
      برائے (غیرعلامتی idx = 0 ; idx < عدد_اشیاء ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_vector4_t vvp_darray_vec2::get_bitstream(منطقی)
{
      vvp_vector4_t vec(array_.size() * لفظ_wid_2, BIT4_0);

      غیرعلامتی adx = 0;
      غیرعلامتی vdx = vec.size();
      جب (vdx > 0) {
            vdx -= لفظ_wid_2;
            برائے (غیرعلامتی bdx = 0; bdx < لفظ_wid_2; bdx += 1) {
                  اگر (array_[adx].value(bdx))
                        vec.سیٹ_ثنائی_ہندسہ(vdx+bdx, BIT4_1);
            }
            adx++;
      }

      واپس vec;
}

vvp_darray_object::~vvp_darray_object()
{
}

size_t vvp_darray_object::get_سائز() ثابت
{
      واپس array_.size();
}

خالی vvp_darray_object::سیٹ_لفظ(غیرعلامتی adr, ثابت vvp_object_t&value)
{
      اگر (adr >= array_.size())
	    واپس;
      array_[adr] = value;
}

خالی vvp_darray_object::get_لفظ(غیرعلامتی adr, vvp_object_t&value)
{
      اگر (adr >= array_.size()) {
	    value = vvp_object_t();
	    واپس;
      }

      value = array_[adr];
}

خالی vvp_darray_object::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_object*that = dynamic_cast<ثابت vvp_darray_object*>(obj);
      أكد(that);

      غیرعلامتی عدد_اشیاء = min(array_.size(), that->array_.size());
      برائے (غیرعلامتی idx = 0 ; idx < عدد_اشیاء ; idx += 1)
	    array_[idx] = that->array_[idx];
}

خالی vvp_darray_object::الٹ_elems(خالی)
{
      size_t n = array_.size();
      برائے (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

خالی vvp_darray_object::ترتیب_دو_elems(منطقی ascending)
{
      اگر (ascending) {
	    std::sort(array_.begin(), array_.انتہا(),
		      [](ثابت vvp_object_t&a, ثابت vvp_object_t&b) {
			    واپس a.ترتیب_دو_ترتیب_کلید() < b.ترتیب_دو_ترتیب_کلید();
		      });
      } ورنہ {
	    std::sort(array_.begin(), array_.انتہا(),
		      [](ثابت vvp_object_t&a, ثابت vvp_object_t&b) {
			    واپس a.ترتیب_دو_ترتیب_کلید() > b.ترتیب_دو_ترتیب_کلید();
		      });
      }
}

خالی vvp_darray_object::shuffle_elems(خالی)
{
      std::shuffle(array_.begin(), array_.انتہا(), shuffle_rng());
}

vvp_darray_real::~vvp_darray_real()
{
}

size_t vvp_darray_real::get_سائز() ثابت
{
      واپس array_.size();
}

خالی vvp_darray_real::سیٹ_لفظ(غیرعلامتی adr, دوہرا value)
{
      اگر (adr >= array_.size())
	    واپس;
      array_[adr] = value;
}

خالی vvp_darray_real::get_لفظ(غیرعلامتی adr, دوہرا&value)
{
      اگر (adr >= array_.size()) {
	    value = 0.0;
	    واپس;
      }

      value = array_[adr];
}

خالی vvp_darray_real::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_real*that = dynamic_cast<ثابت vvp_darray_real*>(obj);
      أكد(that);

      غیرعلامتی عدد_اشیاء = min(array_.size(), that->array_.size());
      برائے (غیرعلامتی idx = 0 ; idx < عدد_اشیاء ; idx += 1)
	    array_[idx] = that->array_[idx];
}

خالی vvp_darray_real::الٹ_elems(خالی)
{
      size_t n = array_.size();
      برائے (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

خالی vvp_darray_real::ترتیب_دو_elems(منطقی ascending)
{
      اگر (ascending) std::sort(array_.begin(), array_.انتہا());
      ورنہ std::sort(array_.begin(), array_.انتہا(), std::greater<دوہرا>());
}

خالی vvp_darray_real::shuffle_elems(خالی)
{
      std::shuffle(array_.begin(), array_.انتہا(), shuffle_rng());
}

vvp_object* vvp_darray_real::duplicate(خالی) ثابت
{
      vvp_darray_real*that = نیا vvp_darray_real(array_.size());

      برائے (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      واپس that;
}

vvp_vector4_t vvp_darray_real::get_bitstream(منطقی)
{
      ثابت غیرعلامتی لفظ_wid = حجم(دوہرا) * 8;
      أكد(لفظ_wid == 64);

      vvp_vector4_t vec(array_.size() * لفظ_wid, BIT4_0);

      غیرعلامتی adx = 0;
      غیرعلامتی vdx = vec.size();
      جب (vdx > 0) {
            اتحاد {
                دوہرا   value;
                uint64_t بٹ;
            } لفظ;
            لفظ.value = array_[adx++];
            vdx -= لفظ_wid;
            برائے (غیرعلامتی bdx = 0; bdx < لفظ_wid; bdx += 1) {
                  اگر (لفظ.بٹ & 1)
                        vec.سیٹ_ثنائی_ہندسہ(vdx+bdx, BIT4_1);
                  لفظ.بٹ >>= 1;
            }
      }

      واپس vec;
}

vvp_darray_string::~vvp_darray_string()
{
}

size_t vvp_darray_string::get_سائز() ثابت
{
      واپس array_.size();
}

خالی vvp_darray_string::سیٹ_لفظ(غیرعلامتی adr, ثابت string&value)
{
      اگر (adr >= array_.size())
	    واپس;
      array_[adr] = value;
}

خالی vvp_darray_string::get_لفظ(غیرعلامتی adr, string&value)
{
      اگر (adr >= array_.size()) {
	    value = "";
	    واپس;
      }

      value = array_[adr];
}

خالی vvp_darray_string::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_string*that = dynamic_cast<ثابت vvp_darray_string*>(obj);
      أكد(that);

      غیرعلامتی عدد_اشیاء = min(array_.size(), that->array_.size());
      برائے (غیرعلامتی idx = 0 ; idx < عدد_اشیاء ; idx += 1)
	    array_[idx] = that->array_[idx];
}

خالی vvp_darray_string::الٹ_elems(خالی)
{
      size_t n = array_.size();
      برائے (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

خالی vvp_darray_string::ترتیب_دو_elems(منطقی ascending)
{
      اگر (ascending) std::sort(array_.begin(), array_.انتہا());
      ورنہ std::sort(array_.begin(), array_.انتہا(), std::greater<string>());
}

خالی vvp_darray_string::shuffle_elems(خالی)
{
      std::shuffle(array_.begin(), array_.انتہا(), shuffle_rng());
}

vvp_object* vvp_darray_string::duplicate(خالی) ثابت
{
      vvp_darray_string*that = نیا vvp_darray_string(array_.size());

      برائے (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      واپس that;
}

vvp_قطار::~vvp_قطار()
{
}

خالی vvp_قطار::copy_elems(vvp_object_t, غیرعلامتی)
{
      cerr << "Sorry: copy_elems() not implemented for " << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_قطار::سیٹ_لفظ_max(غیرعلامتی, ثابت vvp_vector4_t&, غیرعلامتی)
{
      cerr << "XXXX set_word_max(vvp_vector4_t) not implemented for " << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_قطار::insert(غیرعلامتی, ثابت vvp_vector4_t&, غیرعلامتی)
{
      cerr << "XXXX insert(vvp_vector4_t) not implemented for " << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_قطار::push_back(ثابت vvp_vector4_t&, غیرعلامتی)
{
      cerr << "XXXX push_back(vvp_vector4_t) not implemented for " << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_قطار::push_front(ثابت vvp_vector4_t&, غیرعلامتی)
{
      cerr << "XXXX push_front(vvp_vector4_t) not implemented for " << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_قطار::سیٹ_لفظ_max(غیرعلامتی, دوہرا, غیرعلامتی)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_قطار::insert(غیرعلامتی, دوہرا, غیرعلامتی)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_قطار::push_back(دوہرا, غیرعلامتی)
{
      cerr << "XXXX push_back(double) not implemented for " << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_قطار::push_front(دوہرا, غیرعلامتی)
{
      cerr << "XXXX push_front(double) not implemented for " << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_قطار::سیٹ_لفظ_max(غیرعلامتی, ثابت string&, غیرعلامتی)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_قطار::insert(غیرعلامتی, ثابت string&, غیرعلامتی)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_قطار::push_back(ثابت string&, غیرعلامتی)
{
      cerr << "XXXX push_back(string) not implemented for " << typeid(*یہ).name() << سطر_ختم;
}

خالی vvp_قطار::push_front(ثابت string&, غیرعلامتی)
{
      cerr << "XXXX push_front(string) not implemented for " << typeid(*یہ).name() << سطر_ختم;
}

/*
 * Helper functions used while copying multiple elements into a queue.
 */
ساکن خالی print_copy_is_too_big(size_t src_سائز, غیرعلامتی max_سائز, ثابت string&qtype)
{
      cerr << get_fileline()
           << "Warning: queue<" << qtype << "> is bounded to have at most "
           << max_سائز << " elements, source has " << src_سائز << " elements." << سطر_ختم;
}

ساکن خالی print_copy_is_too_big(دوہرا&, size_t src_سائز, غیرعلامتی max_سائز)
{
      print_copy_is_too_big(src_سائز, max_سائز, "real");
}

ساکن خالی print_copy_is_too_big(string&, size_t src_سائز, غیرعلامتی max_سائز)
{
      print_copy_is_too_big(src_سائز, max_سائز, "string");
}

ساکن خالی print_copy_is_too_big(vvp_vector4_t&, size_t src_سائز, غیرعلامتی max_سائز)
{
      print_copy_is_too_big(src_سائز, max_سائز, "vector");
}

سانچہ <قسم_نام ELEM, جماعت QTYPE, جماعت SRC_TYPE>
ساکن خالی copy_elements(QTYPE*قطار, SRC_TYPE*src, غیرعلامتی max_سائز)
{
      size_t src_سائز = src->get_سائز();
      اگر ((max_سائز != 0) && (src_سائز > max_سائز)) {
	    ELEM tmp;
	    print_copy_is_too_big(tmp, src_سائز, max_سائز);
      }
      غیرعلامتی copy_سائز = ((src_سائز < max_سائز) ||
                            (max_سائز == 0)) ? src_سائز : max_سائز;
      اگر (copy_سائز < قطار->get_سائز())
	    قطار->erase_دم(copy_سائز);
      برائے (غیرعلامتی idx=0; idx < copy_سائز; ++idx) {
	    ELEM value;
	    src->get_لفظ(idx, value);
	    قطار->سیٹ_لفظ_max(idx, value, max_سائز);
      }
}

خالی vvp_قطار_real::copy_elems(vvp_object_t src, غیرعلامتی max_سائز)
{
      اگر (vvp_قطار*src_قطار = src.peek<vvp_قطار>())
	    copy_elements<دوہرا, vvp_قطار_real, vvp_قطار>(یہ, src_قطار, max_سائز);
      ورنہ اگر (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<دوہرا, vvp_قطار_real, vvp_darray>(یہ, src_darray, max_سائز);
      ورنہ
	    cerr << get_fileline() << "Sorry: cannot copy object to real queue." << سطر_ختم;
}

خالی vvp_قطار_real::سیٹ_لفظ_max(غیرعلامتی adr, دوہرا value, غیرعلامتی max_سائز)
{
      اگر (adr == قطار.size())
	    اگر (!max_سائز || (قطار.size() < max_سائز))
		  قطار.push_back(value);
	    ورنہ
		  cerr << get_fileline()
		       << "Warning: assigning to queue<real>[" << adr << "] is"
		          " outside bound (" << max_سائز << "). " << value
		       << " was not added." << سطر_ختم;
      ورنہ
	    سیٹ_لفظ(adr, value);
}

خالی vvp_قطار_real::سیٹ_لفظ(غیرعلامتی adr, دوہرا value)
{
      اگر (adr < قطار.size())
	    قطار[adr] = value;
      ورنہ
	    cerr << get_fileline()
	         << "Warning: assigning to queue<real>[" << adr << "] is outside "
	            "of size (" << قطار.size() << "). " << value
	         << " was not added." << سطر_ختم;
}

خالی vvp_قطار_real::get_لفظ(غیرعلامتی adr, دوہرا&value)
{
      اگر (adr >= قطار.size())
	    value = 0.0;
      ورنہ
	    value = قطار[adr];
}

خالی vvp_قطار_real::insert(غیرعلامتی idx, دوہرا value, غیرعلامتی max_سائز)
{
	// Inserting past the end of the queue
      اگر (idx > قطار.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<real>[" << idx << "] is "
	            "outside of size (" << قطار.size() << "). " << value
	         << " was not added." << سطر_ختم;
	// Inserting at the end
      ورنہ اگر (idx == قطار.size())
	    اگر (!max_سائز || (قطار.size() < max_سائز))
		  قطار.push_back(value);
	    ورنہ
		  cerr << get_fileline()
		       << "Warning: inserting to queue<real>[" << idx << "] is"
		          " outside bound (" << max_سائز << "). " << value
		       << " was not added." << سطر_ختم;
      ورنہ  {
	    اگر (max_سائز && (قطار.size() == max_سائز)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << قطار.back() << " from already full bounded queue<real> ["
		       << max_سائز << "]." << سطر_ختم;
		  قطار.pop_back();
	    }
	    قطار.insert(قطار.begin()+idx, value);
      }
}

خالی vvp_قطار_real::push_back(دوہرا value, غیرعلامتی max_سائز)
{
      اگر (!max_سائز || (قطار.size() < max_سائز))
	    قطار.push_back(value);
      ورنہ
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<real> ["
	         << max_سائز << "]." << سطر_ختم;
}

خالی vvp_قطار_real::push_front(دوہرا value, غیرعلامتی max_سائز)
{
      اگر (max_سائز && (قطار.size() == max_سائز)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << قطار.back() << " from already full bounded queue<real> ["
	         << max_سائز << "]." << سطر_ختم;
	    قطار.pop_back();
      }
      قطار.push_front(value);
}

خالی vvp_قطار_real::erase(غیرعلامتی idx)
{
      أكد(قطار.size() > idx);
      قطار.erase(قطار.begin()+idx);
}

خالی vvp_قطار_real::erase_دم(غیرعلامتی idx)
{
      أكد(قطار.size() >= idx);
      اگر (قطار.size() > idx)
	    قطار.resize(idx);
}

خالی vvp_قطار_string::copy_elems(vvp_object_t src, غیرعلامتی max_سائز)
{
      اگر (vvp_قطار*src_قطار = src.peek<vvp_قطار>())
	    copy_elements<string, vvp_قطار_string, vvp_قطار>(یہ, src_قطار, max_سائز);
      ورنہ اگر (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<string, vvp_قطار_string, vvp_darray>(یہ, src_darray, max_سائز);
      ورنہ
	    cerr << get_fileline() << "Sorry: cannot copy object to string queue." << سطر_ختم;
}

خالی vvp_قطار_string::سیٹ_لفظ_max(غیرعلامتی adr, ثابت string&value, غیرعلامتی max_سائز)
{
      اگر (adr == قطار.size())
	    اگر (!max_سائز || (قطار.size() < max_سائز))
		  قطار.push_back(value);
	    ورنہ
		  cerr << get_fileline()
		       << "Warning: assigning to queue<string>[" << adr << "] is"
		          " outside bound (" << max_سائز << "). \"" << value
		       << "\" was not added." << سطر_ختم;
      ورنہ
	    سیٹ_لفظ(adr, value);
}

خالی vvp_قطار_string::سیٹ_لفظ(غیرعلامتی adr, ثابت string&value)
{
      اگر (adr < قطار.size())
	    قطار[adr] = value;
      ورنہ
	    cerr << get_fileline()
	         << "Warning: assigning to queue<string>[" << adr << "] is outside "
	            "of size (" << قطار.size() << "). \"" << value
	         << "\" was not added." << سطر_ختم;
}

خالی vvp_قطار_string::get_لفظ(غیرعلامتی adr, string&value)
{
      اگر (adr >= قطار.size())
	    value = "";
      ورنہ
	    value = قطار[adr];
}

خالی vvp_قطار_string::insert(غیرعلامتی idx, ثابت string&value, غیرعلامتی max_سائز)
{
	// Inserting past the end of the queue
      اگر (idx > قطار.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<string>[" << idx << "] is "
	            "outside of size (" << قطار.size() << "). \"" << value
	         << "\" was not added." << سطر_ختم;
	// Inserting at the end
      ورنہ اگر (idx == قطار.size())
	    اگر (!max_سائز || (قطار.size() < max_سائز))
		  قطار.push_back(value);
	    ورنہ
		  cerr << get_fileline()
		       << "Warning: inserting to queue<string>[" << idx << "] is"
		          " outside bound (" << max_سائز << "). \"" << value
		       << "\" was not added." << سطر_ختم;
      ورنہ  {
	    اگر (max_سائز && (قطار.size() == max_سائز)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", \"" << value << "\") removed \""
		       << قطار.back() << "\" from already full bounded queue<string> ["
		       << max_سائز << "]." << سطر_ختم;
		  قطار.pop_back();
	    }
	    قطار.insert(قطار.begin()+idx, value);
      }
}

خالی vvp_قطار_string::push_back(ثابت string&value, غیرعلامتی max_سائز)
{
      اگر (!max_سائز || (قطار.size() < max_سائز))
	    قطار.push_back(value);
      ورنہ
	    cerr << get_fileline()
	         << "Warning: push_back(\"" << value
	         << "\") skipped for already full bounded queue<string> ["
	         << max_سائز << "]." << سطر_ختم;
}

خالی vvp_قطار_string::push_front(ثابت string&value, غیرعلامتی max_سائز)
{
      اگر (max_سائز && (قطار.size() == max_سائز)) {
	    cerr << get_fileline()
	         << "Warning: push_front(\"" << value << "\") removed \""
	         << قطار.back() << "\" from already full bounded queue<string> ["
	         << max_سائز << "]." << سطر_ختم;
	    قطار.pop_back();
      }
      قطار.push_front(value);
}

خالی vvp_قطار_string::erase(غیرعلامتی idx)
{
      أكد(قطار.size() > idx);
      قطار.erase(قطار.begin()+idx);
}

خالی vvp_قطار_string::erase_دم(غیرعلامتی idx)
{
      أكد(قطار.size() >= idx);
      اگر (قطار.size() > idx)
	    قطار.resize(idx);
}

خالی vvp_قطار_real::الٹ_elems(خالی)
{
      std::reverse(قطار.begin(), قطار.انتہا());
}

خالی vvp_قطار_string::الٹ_elems(خالی)
{
      std::reverse(قطار.begin(), قطار.انتہا());
}

خالی vvp_قطار_real::ترتیب_دو_elems(منطقی ascending)
{
      اگر (ascending) std::sort(قطار.begin(), قطار.انتہا());
      ورنہ std::sort(قطار.begin(), قطار.انتہا(), std::greater<دوہرا>());
}

خالی vvp_قطار_real::shuffle_elems(خالی)
{
      std::shuffle(قطار.begin(), قطار.انتہا(), shuffle_rng());
}

خالی vvp_قطار_string::ترتیب_دو_elems(منطقی ascending)
{
      اگر (ascending) std::sort(قطار.begin(), قطار.انتہا());
      ورنہ std::sort(قطار.begin(), قطار.انتہا(), std::greater<string>());
}

خالی vvp_قطار_string::shuffle_elems(خالی)
{
      std::shuffle(قطار.begin(), قطار.انتہا(), shuffle_rng());
}

خالی vvp_قطار_vec4::copy_elems(vvp_object_t src, غیرعلامتی max_سائز)
{
      اگر (vvp_قطار*src_قطار = src.peek<vvp_قطار>())
	    copy_elements<vvp_vector4_t, vvp_قطار_vec4, vvp_قطار>(یہ, src_قطار, max_سائز);
      ورنہ اگر (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<vvp_vector4_t, vvp_قطار_vec4, vvp_darray>(یہ, src_darray, max_سائز);
      ورنہ
	    cerr << get_fileline() << "Sorry: cannot copy object to vector queue." << سطر_ختم;
}

خالی vvp_قطار_vec4::سیٹ_لفظ_max(غیرعلامتی adr, ثابت vvp_vector4_t&value, غیرعلامتی max_سائز)
{
      اگر (adr == قطار.size())
	    اگر (!max_سائز || (قطار.size() < max_سائز))
		  قطار.push_back(value);
	    ورنہ
		  cerr << get_fileline()
		       << "Warning: assigning to queue<vector>[" << adr << "] is"
		          " outside bound (" << max_سائز << "). " << value
		       << " was not added." << سطر_ختم;
      ورنہ
	    سیٹ_لفظ(adr, value);
}

خالی vvp_قطار_vec4::سیٹ_لفظ(غیرعلامتی adr, ثابت vvp_vector4_t&value)
{
      اگر (adr < قطار.size())
	    قطار[adr] = value;
      ورنہ
	    cerr << get_fileline()
	         << "Warning: assigning to queue<vector>[" << adr << "] is outside "
	            "of size (" << قطار.size() << "). " << value
	         << " was not added." << سطر_ختم;
}

خالی vvp_قطار_vec4::get_لفظ(غیرعلامتی adr, vvp_vector4_t&value)
{
      اگر (adr >= قطار.size())
	    value = vvp_vector4_t(قطار[0].size());
      ورنہ
	    value = قطار[adr];
}

خالی vvp_قطار_vec4::insert(غیرعلامتی idx, ثابت vvp_vector4_t&value, غیرعلامتی max_سائز)
{
	// Inserting past the end of the queue
      اگر (idx > قطار.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<vector[" << value.size()
	         << "]>[" << idx << "] is outside of size (" << قطار.size()
	         << "). " << value << " was not added." << سطر_ختم;
	// Inserting at the end
      ورنہ اگر (idx == قطار.size())
	    اگر (!max_سائز || (قطار.size() < max_سائز))
		  قطار.push_back(value);
	    ورنہ
		  cerr << get_fileline()
		       << "Warning: inserting to queue<vector[" << value.size()
		       << "]>[" << idx << "] is outside bound (" << max_سائز
		       << "). " << value << " was not added." << سطر_ختم;
      ورنہ  {
	    اگر (max_سائز && (قطار.size() == max_سائز)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << قطار.back() << " from already full bounded queue<vector["
		       << value.size() << "]> [" << max_سائز << "]." << سطر_ختم;
		  قطار.pop_back();
	    }
	    قطار.insert(قطار.begin()+idx, value);
      }
}

خالی vvp_قطار_vec4::push_back(ثابت vvp_vector4_t&value, غیرعلامتی max_سائز)
{
      اگر (!max_سائز || (قطار.size() < max_سائز))
	    قطار.push_back(value);
      ورنہ
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<vector["
	         << value.size() << "]> [" << max_سائز << "]." << سطر_ختم;
}

خالی vvp_قطار_vec4::push_front(ثابت vvp_vector4_t&value, غیرعلامتی max_سائز)
{
      اگر (max_سائز && (قطار.size() == max_سائز)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << قطار.back() << " from already full bounded queue<vector["
	         << value.size() << "]> [" << max_سائز << "]." << سطر_ختم;
	    قطار.pop_back();
      }
      قطار.push_front(value);
}

خالی vvp_قطار_vec4::erase(غیرعلامتی idx)
{
      أكد(قطار.size() > idx);
      قطار.erase(قطار.begin()+idx);
}

خالی vvp_قطار_vec4::erase_دم(غیرعلامتی idx)
{
      أكد(قطار.size() >= idx);
      اگر (قطار.size() > idx)
	    قطار.resize(idx);
}

خالی vvp_قطار_vec4::الٹ_elems(خالی)
{
      std::reverse(قطار.begin(), قطار.انتہا());
}

خالی vvp_قطار_vec4::ترتیب_دو_elems(منطقی ascending)
{
      اگر (ascending) {
	    std::sort(قطار.begin(), قطار.انتہا(), vec4_lt_unsigned);
      } ورنہ {
	    std::sort(قطار.begin(), قطار.انتہا(),
		      [](ثابت vvp_vector4_t&a, ثابت vvp_vector4_t&b) {
			    واپس vec4_lt_unsigned(b, a);
		      });
      }
}

خالی vvp_قطار_vec4::shuffle_elems(خالی)
{
      std::shuffle(قطار.begin(), قطار.انتہا(), shuffle_rng());
}
