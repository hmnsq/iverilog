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

ورتوں نطاق std;

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

فراغ vvp_darray::الٹ_elems(فراغ)
{
      cerr << "XXXX reverse_elems() not implemented for "
           << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_darray::ترتیب_دیو_elems(منطقي /*ascending*/)
{
      cerr << "XXXX sort_elems() not implemented for "
           << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_darray::shuffle_elems(فراغ)
{
      cerr << "XXXX shuffle_elems() not implemented for "
           << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_darray::سیٹ_لفظ(بنانشانی, ثابت vvp_vector4_t&)
{
      cerr << "XXXX set_word(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_darray::سیٹ_لفظ(بنانشانی, مزدوج)
{
      cerr << "XXXX set_word(double) not implemented for " << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_darray::سیٹ_لفظ(بنانشانی, ثابت string&)
{
      cerr << "XXXX set_word(string) not implemented for " << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_darray::سیٹ_لفظ(بنانشانی, ثابت vvp_object_t&)
{
      cerr << "XXXX set_word(vvp_object_t) not implemented for " << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_darray::get_لفظ(بنانشانی, vvp_vector4_t&)
{
      cerr << "XXXX get_word(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_darray::get_لفظ(بنانشانی, مزدوج&)
{
      cerr << "XXXX get_word(double) not implemented for " << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_darray::get_لفظ(بنانشانی, string&)
{
      cerr << "XXXX get_word(string) not implemented for " << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_darray::get_لفظ(بنانشانی, vvp_object_t&)
{
      cerr << "XXXX get_word(vvp_object_t) not implemented for " << typeid(*هذا).name() << سطر_انت;
}

vvp_vector4_t vvp_darray::get_bitstream(منطقي)
{
      cerr << "XXXX get_bitstream() not implemented for " << typeid(*هذا).name() << سطر_انت;
      أرجع vvp_vector4_t();
}

سانچا <صنف TYPE> vvp_darray_atom<TYPE>::~vvp_darray_atom()
{
}

سانچا <صنف TYPE> size_t vvp_darray_atom<TYPE>::get_ناپ() ثابت
{
      أرجع array_.size();
}

سانچا <صنف TYPE> فراغ vvp_darray_atom<TYPE>::الٹ_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

سانچا <صنف TYPE> فراغ vvp_darray_atom<TYPE>::ترتیب_دیو_elems(منطقي ascending)
{
      إذا (ascending) std::sort(array_.begin(), array_.سرا());
      وإلا std::sort(array_.begin(), array_.سرا(), std::greater<TYPE>());
}

سانچا <صنف TYPE> فراغ vvp_darray_atom<TYPE>::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.سرا(), shuffle_rng());
}

سانچا <صنف TYPE> فراغ vvp_darray_atom<TYPE>::سیٹ_لفظ(بنانشانی adr, ثابت vvp_vector4_t&value)
{
      إذا (adr >= array_.size())
	    أرجع;
      TYPE tmp;
      vector4_منزل_قدر(value, tmp, سچ, خطأ);
      array_[adr] = tmp;
}

سانچا <صنف TYPE> فراغ vvp_darray_atom<TYPE>::get_لفظ(بنانشانی adr, vvp_vector4_t&value)
{
      إذا (adr >= array_.size()) {
	    value = vvp_vector4_t(8*حجم(TYPE), BIT4_X);
	    أرجع;
      }

      TYPE لفظ = array_[adr];
      vvp_vector4_t tmp (8*حجم(TYPE), BIT4_0);
      لكل (بنانشانی idx = 0 ; idx < tmp.size() ; idx += 1) {
	    إذا (لفظ&1) tmp.سیٹ_ثنائی_ہندسہ(idx, BIT4_1);
	    لفظ >>= 1;
      }
      value = tmp;
}

سانچا <صنف TYPE> فراغ vvp_darray_atom<TYPE>::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_atom<TYPE>*that = dynamic_cast<ثابت vvp_darray_atom<TYPE>*>(obj);
      أكد(that);

      بنانشانی عدد_اشیاء = min(array_.size(), that->array_.size());
      لكل (بنانشانی idx = 0 ; idx < عدد_اشیاء ; idx += 1)
	    array_[idx] = that->array_[idx];
}

سانچا <صنف TYPE> vvp_object* vvp_darray_atom<TYPE>::duplicate(فراغ) ثابت
{
      vvp_darray_atom<TYPE>*that = جديد vvp_darray_atom<TYPE>(array_.size());
      لكل (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      أرجع that;
}

سانچا <صنف TYPE> vvp_vector4_t vvp_darray_atom<TYPE>::get_bitstream(منطقي)
{
      ثابت بنانشانی لفظ_wid = حجم(TYPE) * 8;

      vvp_vector4_t vec(array_.size() * لفظ_wid, BIT4_0);

      بنانشانی adx = 0;
      بنانشانی vdx = vec.size();
      طالما (vdx > 0) {
            TYPE لفظ = array_[adx++];
            vdx -= لفظ_wid;
            لكل (بنانشانی bdx = 0; bdx < لفظ_wid; bdx += 1) {
                  إذا (لفظ & 1)
                        vec.سیٹ_ثنائی_ہندسہ(vdx+bdx, BIT4_1);
                  لفظ >>= 1;
            }
      }

      أرجع vec;
}

سانچا صنف vvp_darray_atom<uint8_t>;
سانچا صنف vvp_darray_atom<uint16_t>;
سانچا صنف vvp_darray_atom<uint32_t>;
سانچا صنف vvp_darray_atom<uint64_t>;
سانچا صنف vvp_darray_atom<int8_t>;
سانچا صنف vvp_darray_atom<int16_t>;
سانچا صنف vvp_darray_atom<int32_t>;
سانچا صنف vvp_darray_atom<int64_t>;

vvp_darray_vec4::~vvp_darray_vec4()
{
}

size_t vvp_darray_vec4::get_ناپ(فراغ) ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_vec4::سیٹ_لفظ(بنانشانی adr, ثابت vvp_vector4_t&value)
{
      إذا (adr >= array_.size()) أرجع;
      أكد(value.size() == لفظ_wid_2);
      array_[adr] = value;
}

فراغ vvp_darray_vec4::get_لفظ(بنانشانی adr, vvp_vector4_t&value)
{
	/*
	 * Return an undefined value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      إذا ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(لفظ_wid_2, BIT4_X);
	    أرجع;
      }
      value = array_[adr];
      أكد(value.size() == لفظ_wid_2);
}

فراغ vvp_darray_vec4::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_vec4*that = dynamic_cast<ثابت vvp_darray_vec4*>(obj);
      أكد(that);

      بنانشانی عدد_اشیاء = min(array_.size(), that->array_.size());
      لكل (بنانشانی idx = 0 ; idx < عدد_اشیاء ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_object* vvp_darray_vec4::duplicate(فراغ) ثابت
{
      vvp_darray_vec4*that = جديد vvp_darray_vec4(array_.size(), لفظ_wid_2);

      لكل (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      أرجع that;
}

vvp_vector4_t vvp_darray_vec4::get_bitstream(منطقي as_vec4)
{
      vvp_vector4_t vec(array_.size() * لفظ_wid_2, BIT4_0);

      بنانشانی adx = 0;
      بنانشانی vdx = vec.size();
      طالما (vdx > 0) {
            vdx -= لفظ_wid_2;
            لكل (بنانشانی bdx = 0; bdx < لفظ_wid_2; bdx += 1) {
                  vvp_bit4_t ثنائی_ہندسہ = array_[adx].value(bdx);
                  إذا (as_vec4 || (ثنائی_ہندسہ == BIT4_1))
                        vec.سیٹ_ثنائی_ہندسہ(vdx+bdx, ثنائی_ہندسہ);
            }
            adx++;
      }

      أرجع vec;
}

فراغ vvp_darray_vec4::الٹ_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_vec4::ترتیب_دیو_elems(منطقي ascending)
{
      إذا (ascending) {
	    std::sort(array_.begin(), array_.سرا(), vec4_lt_unsigned);
      } وإلا {
	    std::sort(array_.begin(), array_.سرا(),
		      [](ثابت vvp_vector4_t&a, ثابت vvp_vector4_t&b) {
			    أرجع vec4_lt_unsigned(b, a);
		      });
      }
}

فراغ vvp_darray_vec4::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.سرا(), shuffle_rng());
}

فراغ vvp_darray_vec2::الٹ_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_vec2::ترتیب_دیو_elems(منطقي ascending)
{
      إذا (ascending) {
	    std::sort(array_.begin(), array_.سرا());
      } وإلا {
	    std::sort(array_.begin(), array_.سرا(),
		      [](ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b) {
			    أرجع b < a;
		      });
      }
}

فراغ vvp_darray_vec2::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.سرا(), shuffle_rng());
}

vvp_darray_vec2::~vvp_darray_vec2()
{
}

size_t vvp_darray_vec2::get_ناپ(فراغ) ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_vec2::سیٹ_لفظ(بنانشانی adr, ثابت vvp_vector4_t&value)
{
      إذا (adr >= array_.size()) أرجع;
      أكد(value.size() == لفظ_wid_2);
      array_[adr] = value;
}

فراغ vvp_darray_vec2::get_لفظ(بنانشانی adr, vvp_vector4_t&value)
{
	/*
	 * Return a zero value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      إذا ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(لفظ_wid_2, BIT4_0);
	    أرجع;
      }
      أكد(array_[adr].size() == لفظ_wid_2);
      value.resize(لفظ_wid_2);
      لكل (بنانشانی idx = 0; idx < لفظ_wid_2; idx += 1) {
	    value.سیٹ_ثنائی_ہندسہ(idx, array_[adr].value4(idx));
      }
}

فراغ vvp_darray_vec2::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_vec2*that = dynamic_cast<ثابت vvp_darray_vec2*>(obj);
      أكد(that);

      بنانشانی عدد_اشیاء = min(array_.size(), that->array_.size());
      لكل (بنانشانی idx = 0 ; idx < عدد_اشیاء ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_vector4_t vvp_darray_vec2::get_bitstream(منطقي)
{
      vvp_vector4_t vec(array_.size() * لفظ_wid_2, BIT4_0);

      بنانشانی adx = 0;
      بنانشانی vdx = vec.size();
      طالما (vdx > 0) {
            vdx -= لفظ_wid_2;
            لكل (بنانشانی bdx = 0; bdx < لفظ_wid_2; bdx += 1) {
                  إذا (array_[adx].value(bdx))
                        vec.سیٹ_ثنائی_ہندسہ(vdx+bdx, BIT4_1);
            }
            adx++;
      }

      أرجع vec;
}

vvp_darray_object::~vvp_darray_object()
{
}

size_t vvp_darray_object::get_ناپ() ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_object::سیٹ_لفظ(بنانشانی adr, ثابت vvp_object_t&value)
{
      إذا (adr >= array_.size())
	    أرجع;
      array_[adr] = value;
}

فراغ vvp_darray_object::get_لفظ(بنانشانی adr, vvp_object_t&value)
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

      بنانشانی عدد_اشیاء = min(array_.size(), that->array_.size());
      لكل (بنانشانی idx = 0 ; idx < عدد_اشیاء ; idx += 1)
	    array_[idx] = that->array_[idx];
}

فراغ vvp_darray_object::الٹ_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_object::ترتیب_دیو_elems(منطقي ascending)
{
      إذا (ascending) {
	    std::sort(array_.begin(), array_.سرا(),
		      [](ثابت vvp_object_t&a, ثابت vvp_object_t&b) {
			    أرجع a.ترتیب_دیو_ترتیب_کلید() < b.ترتیب_دیو_ترتیب_کلید();
		      });
      } وإلا {
	    std::sort(array_.begin(), array_.سرا(),
		      [](ثابت vvp_object_t&a, ثابت vvp_object_t&b) {
			    أرجع a.ترتیب_دیو_ترتیب_کلید() > b.ترتیب_دیو_ترتیب_کلید();
		      });
      }
}

فراغ vvp_darray_object::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.سرا(), shuffle_rng());
}

vvp_darray_real::~vvp_darray_real()
{
}

size_t vvp_darray_real::get_ناپ() ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_real::سیٹ_لفظ(بنانشانی adr, مزدوج value)
{
      إذا (adr >= array_.size())
	    أرجع;
      array_[adr] = value;
}

فراغ vvp_darray_real::get_لفظ(بنانشانی adr, مزدوج&value)
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

      بنانشانی عدد_اشیاء = min(array_.size(), that->array_.size());
      لكل (بنانشانی idx = 0 ; idx < عدد_اشیاء ; idx += 1)
	    array_[idx] = that->array_[idx];
}

فراغ vvp_darray_real::الٹ_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_real::ترتیب_دیو_elems(منطقي ascending)
{
      إذا (ascending) std::sort(array_.begin(), array_.سرا());
      وإلا std::sort(array_.begin(), array_.سرا(), std::greater<مزدوج>());
}

فراغ vvp_darray_real::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.سرا(), shuffle_rng());
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
      ثابت بنانشانی لفظ_wid = حجم(مزدوج) * 8;
      أكد(لفظ_wid == 64);

      vvp_vector4_t vec(array_.size() * لفظ_wid, BIT4_0);

      بنانشانی adx = 0;
      بنانشانی vdx = vec.size();
      طالما (vdx > 0) {
            اتحاد {
                مزدوج   value;
                uint64_t بٹ;
            } لفظ;
            لفظ.value = array_[adx++];
            vdx -= لفظ_wid;
            لكل (بنانشانی bdx = 0; bdx < لفظ_wid; bdx += 1) {
                  إذا (لفظ.بٹ & 1)
                        vec.سیٹ_ثنائی_ہندسہ(vdx+bdx, BIT4_1);
                  لفظ.بٹ >>= 1;
            }
      }

      أرجع vec;
}

vvp_darray_string::~vvp_darray_string()
{
}

size_t vvp_darray_string::get_ناپ() ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_string::سیٹ_لفظ(بنانشانی adr, ثابت string&value)
{
      إذا (adr >= array_.size())
	    أرجع;
      array_[adr] = value;
}

فراغ vvp_darray_string::get_لفظ(بنانشانی adr, string&value)
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

      بنانشانی عدد_اشیاء = min(array_.size(), that->array_.size());
      لكل (بنانشانی idx = 0 ; idx < عدد_اشیاء ; idx += 1)
	    array_[idx] = that->array_[idx];
}

فراغ vvp_darray_string::الٹ_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_string::ترتیب_دیو_elems(منطقي ascending)
{
      إذا (ascending) std::sort(array_.begin(), array_.سرا());
      وإلا std::sort(array_.begin(), array_.سرا(), std::greater<string>());
}

فراغ vvp_darray_string::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.سرا(), shuffle_rng());
}

vvp_object* vvp_darray_string::duplicate(فراغ) ثابت
{
      vvp_darray_string*that = جديد vvp_darray_string(array_.size());

      لكل (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      أرجع that;
}

vvp_قطار::~vvp_قطار()
{
}

فراغ vvp_قطار::copy_elems(vvp_object_t, بنانشانی)
{
      cerr << "Sorry: copy_elems() not implemented for " << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_قطار::سیٹ_لفظ_max(بنانشانی, ثابت vvp_vector4_t&, بنانشانی)
{
      cerr << "XXXX set_word_max(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_قطار::insert(بنانشانی, ثابت vvp_vector4_t&, بنانشانی)
{
      cerr << "XXXX insert(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_قطار::push_back(ثابت vvp_vector4_t&, بنانشانی)
{
      cerr << "XXXX push_back(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_قطار::push_front(ثابت vvp_vector4_t&, بنانشانی)
{
      cerr << "XXXX push_front(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_قطار::سیٹ_لفظ_max(بنانشانی, مزدوج, بنانشانی)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_قطار::insert(بنانشانی, مزدوج, بنانشانی)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_قطار::push_back(مزدوج, بنانشانی)
{
      cerr << "XXXX push_back(double) not implemented for " << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_قطار::push_front(مزدوج, بنانشانی)
{
      cerr << "XXXX push_front(double) not implemented for " << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_قطار::سیٹ_لفظ_max(بنانشانی, ثابت string&, بنانشانی)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_قطار::insert(بنانشانی, ثابت string&, بنانشانی)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_قطار::push_back(ثابت string&, بنانشانی)
{
      cerr << "XXXX push_back(string) not implemented for " << typeid(*هذا).name() << سطر_انت;
}

فراغ vvp_قطار::push_front(ثابت string&, بنانشانی)
{
      cerr << "XXXX push_front(string) not implemented for " << typeid(*هذا).name() << سطر_انت;
}

/*
 * Helper functions used while copying multiple elements into a queue.
 */
ساكن فراغ print_copy_is_too_big(size_t src_ناپ, بنانشانی max_ناپ, ثابت string&qtype)
{
      cerr << get_fileline()
           << "Warning: queue<" << qtype << "> is bounded to have at most "
           << max_ناپ << " elements, source has " << src_ناپ << " elements." << سطر_انت;
}

ساكن فراغ print_copy_is_too_big(مزدوج&, size_t src_ناپ, بنانشانی max_ناپ)
{
      print_copy_is_too_big(src_ناپ, max_ناپ, "real");
}

ساكن فراغ print_copy_is_too_big(string&, size_t src_ناپ, بنانشانی max_ناپ)
{
      print_copy_is_too_big(src_ناپ, max_ناپ, "string");
}

ساكن فراغ print_copy_is_too_big(vvp_vector4_t&, size_t src_ناپ, بنانشانی max_ناپ)
{
      print_copy_is_too_big(src_ناپ, max_ناپ, "vector");
}

سانچا <قسم_ناں ELEM, صنف QTYPE, صنف SRC_TYPE>
ساكن فراغ copy_elements(QTYPE*قطار, SRC_TYPE*src, بنانشانی max_ناپ)
{
      size_t src_ناپ = src->get_ناپ();
      إذا ((max_ناپ != 0) && (src_ناپ > max_ناپ)) {
	    ELEM tmp;
	    print_copy_is_too_big(tmp, src_ناپ, max_ناپ);
      }
      بنانشانی copy_ناپ = ((src_ناپ < max_ناپ) ||
                            (max_ناپ == 0)) ? src_ناپ : max_ناپ;
      إذا (copy_ناپ < قطار->get_ناپ())
	    قطار->erase_پوچھ(copy_ناپ);
      لكل (بنانشانی idx=0; idx < copy_ناپ; ++idx) {
	    ELEM value;
	    src->get_لفظ(idx, value);
	    قطار->سیٹ_لفظ_max(idx, value, max_ناپ);
      }
}

فراغ vvp_قطار_real::copy_elems(vvp_object_t src, بنانشانی max_ناپ)
{
      إذا (vvp_قطار*src_قطار = src.peek<vvp_قطار>())
	    copy_elements<مزدوج, vvp_قطار_real, vvp_قطار>(هذا, src_قطار, max_ناپ);
      وإلا إذا (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<مزدوج, vvp_قطار_real, vvp_darray>(هذا, src_darray, max_ناپ);
      وإلا
	    cerr << get_fileline() << "Sorry: cannot copy object to real queue." << سطر_انت;
}

فراغ vvp_قطار_real::سیٹ_لفظ_max(بنانشانی adr, مزدوج value, بنانشانی max_ناپ)
{
      إذا (adr == قطار.size())
	    إذا (!max_ناپ || (قطار.size() < max_ناپ))
		  قطار.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: assigning to queue<real>[" << adr << "] is"
		          " outside bound (" << max_ناپ << "). " << value
		       << " was not added." << سطر_انت;
      وإلا
	    سیٹ_لفظ(adr, value);
}

فراغ vvp_قطار_real::سیٹ_لفظ(بنانشانی adr, مزدوج value)
{
      إذا (adr < قطار.size())
	    قطار[adr] = value;
      وإلا
	    cerr << get_fileline()
	         << "Warning: assigning to queue<real>[" << adr << "] is outside "
	            "of size (" << قطار.size() << "). " << value
	         << " was not added." << سطر_انت;
}

فراغ vvp_قطار_real::get_لفظ(بنانشانی adr, مزدوج&value)
{
      إذا (adr >= قطار.size())
	    value = 0.0;
      وإلا
	    value = قطار[adr];
}

فراغ vvp_قطار_real::insert(بنانشانی idx, مزدوج value, بنانشانی max_ناپ)
{
	// Inserting past the end of the queue
      إذا (idx > قطار.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<real>[" << idx << "] is "
	            "outside of size (" << قطار.size() << "). " << value
	         << " was not added." << سطر_انت;
	// Inserting at the end
      وإلا إذا (idx == قطار.size())
	    إذا (!max_ناپ || (قطار.size() < max_ناپ))
		  قطار.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: inserting to queue<real>[" << idx << "] is"
		          " outside bound (" << max_ناپ << "). " << value
		       << " was not added." << سطر_انت;
      وإلا  {
	    إذا (max_ناپ && (قطار.size() == max_ناپ)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << قطار.back() << " from already full bounded queue<real> ["
		       << max_ناپ << "]." << سطر_انت;
		  قطار.pop_back();
	    }
	    قطار.insert(قطار.begin()+idx, value);
      }
}

فراغ vvp_قطار_real::push_back(مزدوج value, بنانشانی max_ناپ)
{
      إذا (!max_ناپ || (قطار.size() < max_ناپ))
	    قطار.push_back(value);
      وإلا
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<real> ["
	         << max_ناپ << "]." << سطر_انت;
}

فراغ vvp_قطار_real::push_front(مزدوج value, بنانشانی max_ناپ)
{
      إذا (max_ناپ && (قطار.size() == max_ناپ)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << قطار.back() << " from already full bounded queue<real> ["
	         << max_ناپ << "]." << سطر_انت;
	    قطار.pop_back();
      }
      قطار.push_front(value);
}

فراغ vvp_قطار_real::erase(بنانشانی idx)
{
      أكد(قطار.size() > idx);
      قطار.erase(قطار.begin()+idx);
}

فراغ vvp_قطار_real::erase_پوچھ(بنانشانی idx)
{
      أكد(قطار.size() >= idx);
      إذا (قطار.size() > idx)
	    قطار.resize(idx);
}

فراغ vvp_قطار_string::copy_elems(vvp_object_t src, بنانشانی max_ناپ)
{
      إذا (vvp_قطار*src_قطار = src.peek<vvp_قطار>())
	    copy_elements<string, vvp_قطار_string, vvp_قطار>(هذا, src_قطار, max_ناپ);
      وإلا إذا (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<string, vvp_قطار_string, vvp_darray>(هذا, src_darray, max_ناپ);
      وإلا
	    cerr << get_fileline() << "Sorry: cannot copy object to string queue." << سطر_انت;
}

فراغ vvp_قطار_string::سیٹ_لفظ_max(بنانشانی adr, ثابت string&value, بنانشانی max_ناپ)
{
      إذا (adr == قطار.size())
	    إذا (!max_ناپ || (قطار.size() < max_ناپ))
		  قطار.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: assigning to queue<string>[" << adr << "] is"
		          " outside bound (" << max_ناپ << "). \"" << value
		       << "\" was not added." << سطر_انت;
      وإلا
	    سیٹ_لفظ(adr, value);
}

فراغ vvp_قطار_string::سیٹ_لفظ(بنانشانی adr, ثابت string&value)
{
      إذا (adr < قطار.size())
	    قطار[adr] = value;
      وإلا
	    cerr << get_fileline()
	         << "Warning: assigning to queue<string>[" << adr << "] is outside "
	            "of size (" << قطار.size() << "). \"" << value
	         << "\" was not added." << سطر_انت;
}

فراغ vvp_قطار_string::get_لفظ(بنانشانی adr, string&value)
{
      إذا (adr >= قطار.size())
	    value = "";
      وإلا
	    value = قطار[adr];
}

فراغ vvp_قطار_string::insert(بنانشانی idx, ثابت string&value, بنانشانی max_ناپ)
{
	// Inserting past the end of the queue
      إذا (idx > قطار.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<string>[" << idx << "] is "
	            "outside of size (" << قطار.size() << "). \"" << value
	         << "\" was not added." << سطر_انت;
	// Inserting at the end
      وإلا إذا (idx == قطار.size())
	    إذا (!max_ناپ || (قطار.size() < max_ناپ))
		  قطار.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: inserting to queue<string>[" << idx << "] is"
		          " outside bound (" << max_ناپ << "). \"" << value
		       << "\" was not added." << سطر_انت;
      وإلا  {
	    إذا (max_ناپ && (قطار.size() == max_ناپ)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", \"" << value << "\") removed \""
		       << قطار.back() << "\" from already full bounded queue<string> ["
		       << max_ناپ << "]." << سطر_انت;
		  قطار.pop_back();
	    }
	    قطار.insert(قطار.begin()+idx, value);
      }
}

فراغ vvp_قطار_string::push_back(ثابت string&value, بنانشانی max_ناپ)
{
      إذا (!max_ناپ || (قطار.size() < max_ناپ))
	    قطار.push_back(value);
      وإلا
	    cerr << get_fileline()
	         << "Warning: push_back(\"" << value
	         << "\") skipped for already full bounded queue<string> ["
	         << max_ناپ << "]." << سطر_انت;
}

فراغ vvp_قطار_string::push_front(ثابت string&value, بنانشانی max_ناپ)
{
      إذا (max_ناپ && (قطار.size() == max_ناپ)) {
	    cerr << get_fileline()
	         << "Warning: push_front(\"" << value << "\") removed \""
	         << قطار.back() << "\" from already full bounded queue<string> ["
	         << max_ناپ << "]." << سطر_انت;
	    قطار.pop_back();
      }
      قطار.push_front(value);
}

فراغ vvp_قطار_string::erase(بنانشانی idx)
{
      أكد(قطار.size() > idx);
      قطار.erase(قطار.begin()+idx);
}

فراغ vvp_قطار_string::erase_پوچھ(بنانشانی idx)
{
      أكد(قطار.size() >= idx);
      إذا (قطار.size() > idx)
	    قطار.resize(idx);
}

فراغ vvp_قطار_real::الٹ_elems(فراغ)
{
      std::reverse(قطار.begin(), قطار.سرا());
}

فراغ vvp_قطار_string::الٹ_elems(فراغ)
{
      std::reverse(قطار.begin(), قطار.سرا());
}

فراغ vvp_قطار_real::ترتیب_دیو_elems(منطقي ascending)
{
      إذا (ascending) std::sort(قطار.begin(), قطار.سرا());
      وإلا std::sort(قطار.begin(), قطار.سرا(), std::greater<مزدوج>());
}

فراغ vvp_قطار_real::shuffle_elems(فراغ)
{
      std::shuffle(قطار.begin(), قطار.سرا(), shuffle_rng());
}

فراغ vvp_قطار_string::ترتیب_دیو_elems(منطقي ascending)
{
      إذا (ascending) std::sort(قطار.begin(), قطار.سرا());
      وإلا std::sort(قطار.begin(), قطار.سرا(), std::greater<string>());
}

فراغ vvp_قطار_string::shuffle_elems(فراغ)
{
      std::shuffle(قطار.begin(), قطار.سرا(), shuffle_rng());
}

فراغ vvp_قطار_vec4::copy_elems(vvp_object_t src, بنانشانی max_ناپ)
{
      إذا (vvp_قطار*src_قطار = src.peek<vvp_قطار>())
	    copy_elements<vvp_vector4_t, vvp_قطار_vec4, vvp_قطار>(هذا, src_قطار, max_ناپ);
      وإلا إذا (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<vvp_vector4_t, vvp_قطار_vec4, vvp_darray>(هذا, src_darray, max_ناپ);
      وإلا
	    cerr << get_fileline() << "Sorry: cannot copy object to vector queue." << سطر_انت;
}

فراغ vvp_قطار_vec4::سیٹ_لفظ_max(بنانشانی adr, ثابت vvp_vector4_t&value, بنانشانی max_ناپ)
{
      إذا (adr == قطار.size())
	    إذا (!max_ناپ || (قطار.size() < max_ناپ))
		  قطار.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: assigning to queue<vector>[" << adr << "] is"
		          " outside bound (" << max_ناپ << "). " << value
		       << " was not added." << سطر_انت;
      وإلا
	    سیٹ_لفظ(adr, value);
}

فراغ vvp_قطار_vec4::سیٹ_لفظ(بنانشانی adr, ثابت vvp_vector4_t&value)
{
      إذا (adr < قطار.size())
	    قطار[adr] = value;
      وإلا
	    cerr << get_fileline()
	         << "Warning: assigning to queue<vector>[" << adr << "] is outside "
	            "of size (" << قطار.size() << "). " << value
	         << " was not added." << سطر_انت;
}

فراغ vvp_قطار_vec4::get_لفظ(بنانشانی adr, vvp_vector4_t&value)
{
      إذا (adr >= قطار.size())
	    value = vvp_vector4_t(قطار[0].size());
      وإلا
	    value = قطار[adr];
}

فراغ vvp_قطار_vec4::insert(بنانشانی idx, ثابت vvp_vector4_t&value, بنانشانی max_ناپ)
{
	// Inserting past the end of the queue
      إذا (idx > قطار.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<vector[" << value.size()
	         << "]>[" << idx << "] is outside of size (" << قطار.size()
	         << "). " << value << " was not added." << سطر_انت;
	// Inserting at the end
      وإلا إذا (idx == قطار.size())
	    إذا (!max_ناپ || (قطار.size() < max_ناپ))
		  قطار.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: inserting to queue<vector[" << value.size()
		       << "]>[" << idx << "] is outside bound (" << max_ناپ
		       << "). " << value << " was not added." << سطر_انت;
      وإلا  {
	    إذا (max_ناپ && (قطار.size() == max_ناپ)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << قطار.back() << " from already full bounded queue<vector["
		       << value.size() << "]> [" << max_ناپ << "]." << سطر_انت;
		  قطار.pop_back();
	    }
	    قطار.insert(قطار.begin()+idx, value);
      }
}

فراغ vvp_قطار_vec4::push_back(ثابت vvp_vector4_t&value, بنانشانی max_ناپ)
{
      إذا (!max_ناپ || (قطار.size() < max_ناپ))
	    قطار.push_back(value);
      وإلا
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<vector["
	         << value.size() << "]> [" << max_ناپ << "]." << سطر_انت;
}

فراغ vvp_قطار_vec4::push_front(ثابت vvp_vector4_t&value, بنانشانی max_ناپ)
{
      إذا (max_ناپ && (قطار.size() == max_ناپ)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << قطار.back() << " from already full bounded queue<vector["
	         << value.size() << "]> [" << max_ناپ << "]." << سطر_انت;
	    قطار.pop_back();
      }
      قطار.push_front(value);
}

فراغ vvp_قطار_vec4::erase(بنانشانی idx)
{
      أكد(قطار.size() > idx);
      قطار.erase(قطار.begin()+idx);
}

فراغ vvp_قطار_vec4::erase_پوچھ(بنانشانی idx)
{
      أكد(قطار.size() >= idx);
      إذا (قطار.size() > idx)
	    قطار.resize(idx);
}

فراغ vvp_قطار_vec4::الٹ_elems(فراغ)
{
      std::reverse(قطار.begin(), قطار.سرا());
}

فراغ vvp_قطار_vec4::ترتیب_دیو_elems(منطقي ascending)
{
      إذا (ascending) {
	    std::sort(قطار.begin(), قطار.سرا(), vec4_lt_unsigned);
      } وإلا {
	    std::sort(قطار.begin(), قطار.سرا(),
		      [](ثابت vvp_vector4_t&a, ثابت vvp_vector4_t&b) {
			    أرجع vec4_lt_unsigned(b, a);
		      });
      }
}

فراغ vvp_قطار_vec4::shuffle_elems(فراغ)
{
      std::shuffle(قطار.begin(), قطار.سرا(), shuffle_rng());
}
