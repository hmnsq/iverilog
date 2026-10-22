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

استعمال نطاق std;

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

فراغ vvp_darray::ابتو_elems(فراغ)
{
      cerr << "XXXX reverse_elems() not implemented for "
           << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_darray::ترتيب_ڏيو_elems(منطقي /*ascending*/)
{
      cerr << "XXXX sort_elems() not implemented for "
           << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_darray::shuffle_elems(فراغ)
{
      cerr << "XXXX shuffle_elems() not implemented for "
           << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_darray::سيٽ_لفظ(بنانشان, ثابت vvp_vector4_t&)
{
      cerr << "XXXX set_word(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_darray::سيٽ_لفظ(بنانشان, مزدوج)
{
      cerr << "XXXX set_word(double) not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_darray::سيٽ_لفظ(بنانشان, ثابت string&)
{
      cerr << "XXXX set_word(string) not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_darray::سيٽ_لفظ(بنانشان, ثابت vvp_object_t&)
{
      cerr << "XXXX set_word(vvp_object_t) not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_darray::get_لفظ(بنانشان, vvp_vector4_t&)
{
      cerr << "XXXX get_word(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_darray::get_لفظ(بنانشان, مزدوج&)
{
      cerr << "XXXX get_word(double) not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_darray::get_لفظ(بنانشان, string&)
{
      cerr << "XXXX get_word(string) not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_darray::get_لفظ(بنانشان, vvp_object_t&)
{
      cerr << "XXXX get_word(vvp_object_t) not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

vvp_vector4_t vvp_darray::get_bitstream(منطقي)
{
      cerr << "XXXX get_bitstream() not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
      أرجع vvp_vector4_t();
}

سانچو <صنف TYPE> vvp_darray_atom<TYPE>::~vvp_darray_atom()
{
}

سانچو <صنف TYPE> size_t vvp_darray_atom<TYPE>::get_سائيز() ثابت
{
      أرجع array_.size();
}

سانچو <صنف TYPE> فراغ vvp_darray_atom<TYPE>::ابتو_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

سانچو <صنف TYPE> فراغ vvp_darray_atom<TYPE>::ترتيب_ڏيو_elems(منطقي ascending)
{
      إذا (ascending) std::sort(array_.begin(), array_.پڇاڙي());
      وإلا std::sort(array_.begin(), array_.پڇاڙي(), std::greater<TYPE>());
}

سانچو <صنف TYPE> فراغ vvp_darray_atom<TYPE>::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.پڇاڙي(), shuffle_rng());
}

سانچو <صنف TYPE> فراغ vvp_darray_atom<TYPE>::سيٽ_لفظ(بنانشان adr, ثابت vvp_vector4_t&value)
{
      إذا (adr >= array_.size())
	    أرجع;
      TYPE tmp;
      vector4_منزل_قدر(value, tmp, سچ, خطأ);
      array_[adr] = tmp;
}

سانچو <صنف TYPE> فراغ vvp_darray_atom<TYPE>::get_لفظ(بنانشان adr, vvp_vector4_t&value)
{
      إذا (adr >= array_.size()) {
	    value = vvp_vector4_t(8*حجم(TYPE), BIT4_X);
	    أرجع;
      }

      TYPE لفظ_2 = array_[adr];
      vvp_vector4_t tmp (8*حجم(TYPE), BIT4_0);
      لكل (بنانشان idx = 0 ; idx < tmp.size() ; idx += 1) {
	    إذا (لفظ_2&1) tmp.سيٽ_ٻٽو_انگ(idx, BIT4_1);
	    لفظ_2 >>= 1;
      }
      value = tmp;
}

سانچو <صنف TYPE> فراغ vvp_darray_atom<TYPE>::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_atom<TYPE>*that = dynamic_cast<ثابت vvp_darray_atom<TYPE>*>(obj);
      أكد(that);

      بنانشان انگ_شيون = min(array_.size(), that->array_.size());
      لكل (بنانشان idx = 0 ; idx < انگ_شيون ; idx += 1)
	    array_[idx] = that->array_[idx];
}

سانچو <صنف TYPE> vvp_object* vvp_darray_atom<TYPE>::duplicate(فراغ) ثابت
{
      vvp_darray_atom<TYPE>*that = جديد vvp_darray_atom<TYPE>(array_.size());
      لكل (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      أرجع that;
}

سانچو <صنف TYPE> vvp_vector4_t vvp_darray_atom<TYPE>::get_bitstream(منطقي)
{
      ثابت بنانشان لفظ_wid = حجم(TYPE) * 8;

      vvp_vector4_t vec(array_.size() * لفظ_wid, BIT4_0);

      بنانشان adx = 0;
      بنانشان vdx = vec.size();
      طالما (vdx > 0) {
            TYPE لفظ_2 = array_[adx++];
            vdx -= لفظ_wid;
            لكل (بنانشان bdx = 0; bdx < لفظ_wid; bdx += 1) {
                  إذا (لفظ_2 & 1)
                        vec.سيٽ_ٻٽو_انگ(vdx+bdx, BIT4_1);
                  لفظ_2 >>= 1;
            }
      }

      أرجع vec;
}

سانچو صنف vvp_darray_atom<uint8_t>;
سانچو صنف vvp_darray_atom<uint16_t>;
سانچو صنف vvp_darray_atom<uint32_t>;
سانچو صنف vvp_darray_atom<uint64_t>;
سانچو صنف vvp_darray_atom<int8_t>;
سانچو صنف vvp_darray_atom<int16_t>;
سانچو صنف vvp_darray_atom<int32_t>;
سانچو صنف vvp_darray_atom<int64_t>;

vvp_darray_vec4::~vvp_darray_vec4()
{
}

size_t vvp_darray_vec4::get_سائيز(فراغ) ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_vec4::سيٽ_لفظ(بنانشان adr, ثابت vvp_vector4_t&value)
{
      إذا (adr >= array_.size()) أرجع;
      أكد(value.size() == لفظ_wid_2);
      array_[adr] = value;
}

فراغ vvp_darray_vec4::get_لفظ(بنانشان adr, vvp_vector4_t&value)
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

      بنانشان انگ_شيون = min(array_.size(), that->array_.size());
      لكل (بنانشان idx = 0 ; idx < انگ_شيون ; idx += 1)
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

      بنانشان adx = 0;
      بنانشان vdx = vec.size();
      طالما (vdx > 0) {
            vdx -= لفظ_wid_2;
            لكل (بنانشان bdx = 0; bdx < لفظ_wid_2; bdx += 1) {
                  vvp_bit4_t ٻٽو_انگ = array_[adx].value(bdx);
                  إذا (as_vec4 || (ٻٽو_انگ == BIT4_1))
                        vec.سيٽ_ٻٽو_انگ(vdx+bdx, ٻٽو_انگ);
            }
            adx++;
      }

      أرجع vec;
}

فراغ vvp_darray_vec4::ابتو_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_vec4::ترتيب_ڏيو_elems(منطقي ascending)
{
      إذا (ascending) {
	    std::sort(array_.begin(), array_.پڇاڙي(), vec4_lt_unsigned);
      } وإلا {
	    std::sort(array_.begin(), array_.پڇاڙي(),
		      [](ثابت vvp_vector4_t&a, ثابت vvp_vector4_t&b) {
			    أرجع vec4_lt_unsigned(b, a);
		      });
      }
}

فراغ vvp_darray_vec4::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.پڇاڙي(), shuffle_rng());
}

فراغ vvp_darray_vec2::ابتو_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_vec2::ترتيب_ڏيو_elems(منطقي ascending)
{
      إذا (ascending) {
	    std::sort(array_.begin(), array_.پڇاڙي());
      } وإلا {
	    std::sort(array_.begin(), array_.پڇاڙي(),
		      [](ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b) {
			    أرجع b < a;
		      });
      }
}

فراغ vvp_darray_vec2::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.پڇاڙي(), shuffle_rng());
}

vvp_darray_vec2::~vvp_darray_vec2()
{
}

size_t vvp_darray_vec2::get_سائيز(فراغ) ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_vec2::سيٽ_لفظ(بنانشان adr, ثابت vvp_vector4_t&value)
{
      إذا (adr >= array_.size()) أرجع;
      أكد(value.size() == لفظ_wid_2);
      array_[adr] = value;
}

فراغ vvp_darray_vec2::get_لفظ(بنانشان adr, vvp_vector4_t&value)
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
      لكل (بنانشان idx = 0; idx < لفظ_wid_2; idx += 1) {
	    value.سيٽ_ٻٽو_انگ(idx, array_[adr].value4(idx));
      }
}

فراغ vvp_darray_vec2::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_vec2*that = dynamic_cast<ثابت vvp_darray_vec2*>(obj);
      أكد(that);

      بنانشان انگ_شيون = min(array_.size(), that->array_.size());
      لكل (بنانشان idx = 0 ; idx < انگ_شيون ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_vector4_t vvp_darray_vec2::get_bitstream(منطقي)
{
      vvp_vector4_t vec(array_.size() * لفظ_wid_2, BIT4_0);

      بنانشان adx = 0;
      بنانشان vdx = vec.size();
      طالما (vdx > 0) {
            vdx -= لفظ_wid_2;
            لكل (بنانشان bdx = 0; bdx < لفظ_wid_2; bdx += 1) {
                  إذا (array_[adx].value(bdx))
                        vec.سيٽ_ٻٽو_انگ(vdx+bdx, BIT4_1);
            }
            adx++;
      }

      أرجع vec;
}

vvp_darray_object::~vvp_darray_object()
{
}

size_t vvp_darray_object::get_سائيز() ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_object::سيٽ_لفظ(بنانشان adr, ثابت vvp_object_t&value)
{
      إذا (adr >= array_.size())
	    أرجع;
      array_[adr] = value;
}

فراغ vvp_darray_object::get_لفظ(بنانشان adr, vvp_object_t&value)
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

      بنانشان انگ_شيون = min(array_.size(), that->array_.size());
      لكل (بنانشان idx = 0 ; idx < انگ_شيون ; idx += 1)
	    array_[idx] = that->array_[idx];
}

فراغ vvp_darray_object::ابتو_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_object::ترتيب_ڏيو_elems(منطقي ascending)
{
      إذا (ascending) {
	    std::sort(array_.begin(), array_.پڇاڙي(),
		      [](ثابت vvp_object_t&a, ثابت vvp_object_t&b) {
			    أرجع a.ترتيب_ڏيو_ترتيب_ڪنجي() < b.ترتيب_ڏيو_ترتيب_ڪنجي();
		      });
      } وإلا {
	    std::sort(array_.begin(), array_.پڇاڙي(),
		      [](ثابت vvp_object_t&a, ثابت vvp_object_t&b) {
			    أرجع a.ترتيب_ڏيو_ترتيب_ڪنجي() > b.ترتيب_ڏيو_ترتيب_ڪنجي();
		      });
      }
}

فراغ vvp_darray_object::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.پڇاڙي(), shuffle_rng());
}

vvp_darray_real::~vvp_darray_real()
{
}

size_t vvp_darray_real::get_سائيز() ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_real::سيٽ_لفظ(بنانشان adr, مزدوج value)
{
      إذا (adr >= array_.size())
	    أرجع;
      array_[adr] = value;
}

فراغ vvp_darray_real::get_لفظ(بنانشان adr, مزدوج&value)
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

      بنانشان انگ_شيون = min(array_.size(), that->array_.size());
      لكل (بنانشان idx = 0 ; idx < انگ_شيون ; idx += 1)
	    array_[idx] = that->array_[idx];
}

فراغ vvp_darray_real::ابتو_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_real::ترتيب_ڏيو_elems(منطقي ascending)
{
      إذا (ascending) std::sort(array_.begin(), array_.پڇاڙي());
      وإلا std::sort(array_.begin(), array_.پڇاڙي(), std::greater<مزدوج>());
}

فراغ vvp_darray_real::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.پڇاڙي(), shuffle_rng());
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
      ثابت بنانشان لفظ_wid = حجم(مزدوج) * 8;
      أكد(لفظ_wid == 64);

      vvp_vector4_t vec(array_.size() * لفظ_wid, BIT4_0);

      بنانشان adx = 0;
      بنانشان vdx = vec.size();
      طالما (vdx > 0) {
            اتحاد {
                مزدوج   value;
                uint64_t بٽ;
            } لفظ_2;
            لفظ_2.value = array_[adx++];
            vdx -= لفظ_wid;
            لكل (بنانشان bdx = 0; bdx < لفظ_wid; bdx += 1) {
                  إذا (لفظ_2.بٽ & 1)
                        vec.سيٽ_ٻٽو_انگ(vdx+bdx, BIT4_1);
                  لفظ_2.بٽ >>= 1;
            }
      }

      أرجع vec;
}

vvp_darray_string::~vvp_darray_string()
{
}

size_t vvp_darray_string::get_سائيز() ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_string::سيٽ_لفظ(بنانشان adr, ثابت string&value)
{
      إذا (adr >= array_.size())
	    أرجع;
      array_[adr] = value;
}

فراغ vvp_darray_string::get_لفظ(بنانشان adr, string&value)
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

      بنانشان انگ_شيون = min(array_.size(), that->array_.size());
      لكل (بنانشان idx = 0 ; idx < انگ_شيون ; idx += 1)
	    array_[idx] = that->array_[idx];
}

فراغ vvp_darray_string::ابتو_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_string::ترتيب_ڏيو_elems(منطقي ascending)
{
      إذا (ascending) std::sort(array_.begin(), array_.پڇاڙي());
      وإلا std::sort(array_.begin(), array_.پڇاڙي(), std::greater<string>());
}

فراغ vvp_darray_string::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.پڇاڙي(), shuffle_rng());
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

فراغ vvp_قطار::copy_elems(vvp_object_t, بنانشان)
{
      cerr << "Sorry: copy_elems() not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_قطار::سيٽ_لفظ_max(بنانشان, ثابت vvp_vector4_t&, بنانشان)
{
      cerr << "XXXX set_word_max(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_قطار::insert(بنانشان, ثابت vvp_vector4_t&, بنانشان)
{
      cerr << "XXXX insert(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_قطار::push_back(ثابت vvp_vector4_t&, بنانشان)
{
      cerr << "XXXX push_back(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_قطار::push_front(ثابت vvp_vector4_t&, بنانشان)
{
      cerr << "XXXX push_front(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_قطار::سيٽ_لفظ_max(بنانشان, مزدوج, بنانشان)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_قطار::insert(بنانشان, مزدوج, بنانشان)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_قطار::push_back(مزدوج, بنانشان)
{
      cerr << "XXXX push_back(double) not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_قطار::push_front(مزدوج, بنانشان)
{
      cerr << "XXXX push_front(double) not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_قطار::سيٽ_لفظ_max(بنانشان, ثابت string&, بنانشان)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_قطار::insert(بنانشان, ثابت string&, بنانشان)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_قطار::push_back(ثابت string&, بنانشان)
{
      cerr << "XXXX push_back(string) not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

فراغ vvp_قطار::push_front(ثابت string&, بنانشان)
{
      cerr << "XXXX push_front(string) not implemented for " << typeid(*هذا).name() << سٽ_پڄاڻي;
}

/*
 * Helper functions used while copying multiple elements into a queue.
 */
ساكن فراغ print_copy_is_too_big(size_t src_سائيز, بنانشان max_سائيز, ثابت string&qtype)
{
      cerr << get_fileline()
           << "Warning: queue<" << qtype << "> is bounded to have at most "
           << max_سائيز << " elements, source has " << src_سائيز << " elements." << سٽ_پڄاڻي;
}

ساكن فراغ print_copy_is_too_big(مزدوج&, size_t src_سائيز, بنانشان max_سائيز)
{
      print_copy_is_too_big(src_سائيز, max_سائيز, "real");
}

ساكن فراغ print_copy_is_too_big(string&, size_t src_سائيز, بنانشان max_سائيز)
{
      print_copy_is_too_big(src_سائيز, max_سائيز, "string");
}

ساكن فراغ print_copy_is_too_big(vvp_vector4_t&, size_t src_سائيز, بنانشان max_سائيز)
{
      print_copy_is_too_big(src_سائيز, max_سائيز, "vector");
}

سانچو <قسم_نالو ELEM, صنف QTYPE, صنف SRC_TYPE>
ساكن فراغ copy_elements(QTYPE*قطار, SRC_TYPE*src, بنانشان max_سائيز)
{
      size_t src_سائيز = src->get_سائيز();
      إذا ((max_سائيز != 0) && (src_سائيز > max_سائيز)) {
	    ELEM tmp;
	    print_copy_is_too_big(tmp, src_سائيز, max_سائيز);
      }
      بنانشان copy_سائيز = ((src_سائيز < max_سائيز) ||
                            (max_سائيز == 0)) ? src_سائيز : max_سائيز;
      إذا (copy_سائيز < قطار->get_سائيز())
	    قطار->erase_پڇ(copy_سائيز);
      لكل (بنانشان idx=0; idx < copy_سائيز; ++idx) {
	    ELEM value;
	    src->get_لفظ(idx, value);
	    قطار->سيٽ_لفظ_max(idx, value, max_سائيز);
      }
}

فراغ vvp_قطار_real::copy_elems(vvp_object_t src, بنانشان max_سائيز)
{
      إذا (vvp_قطار*src_قطار = src.peek<vvp_قطار>())
	    copy_elements<مزدوج, vvp_قطار_real, vvp_قطار>(هذا, src_قطار, max_سائيز);
      وإلا إذا (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<مزدوج, vvp_قطار_real, vvp_darray>(هذا, src_darray, max_سائيز);
      وإلا
	    cerr << get_fileline() << "Sorry: cannot copy object to real queue." << سٽ_پڄاڻي;
}

فراغ vvp_قطار_real::سيٽ_لفظ_max(بنانشان adr, مزدوج value, بنانشان max_سائيز)
{
      إذا (adr == قطار.size())
	    إذا (!max_سائيز || (قطار.size() < max_سائيز))
		  قطار.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: assigning to queue<real>[" << adr << "] is"
		          " outside bound (" << max_سائيز << "). " << value
		       << " was not added." << سٽ_پڄاڻي;
      وإلا
	    سيٽ_لفظ(adr, value);
}

فراغ vvp_قطار_real::سيٽ_لفظ(بنانشان adr, مزدوج value)
{
      إذا (adr < قطار.size())
	    قطار[adr] = value;
      وإلا
	    cerr << get_fileline()
	         << "Warning: assigning to queue<real>[" << adr << "] is outside "
	            "of size (" << قطار.size() << "). " << value
	         << " was not added." << سٽ_پڄاڻي;
}

فراغ vvp_قطار_real::get_لفظ(بنانشان adr, مزدوج&value)
{
      إذا (adr >= قطار.size())
	    value = 0.0;
      وإلا
	    value = قطار[adr];
}

فراغ vvp_قطار_real::insert(بنانشان idx, مزدوج value, بنانشان max_سائيز)
{
	// Inserting past the end of the queue
      إذا (idx > قطار.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<real>[" << idx << "] is "
	            "outside of size (" << قطار.size() << "). " << value
	         << " was not added." << سٽ_پڄاڻي;
	// Inserting at the end
      وإلا إذا (idx == قطار.size())
	    إذا (!max_سائيز || (قطار.size() < max_سائيز))
		  قطار.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: inserting to queue<real>[" << idx << "] is"
		          " outside bound (" << max_سائيز << "). " << value
		       << " was not added." << سٽ_پڄاڻي;
      وإلا  {
	    إذا (max_سائيز && (قطار.size() == max_سائيز)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << قطار.back() << " from already full bounded queue<real> ["
		       << max_سائيز << "]." << سٽ_پڄاڻي;
		  قطار.pop_back();
	    }
	    قطار.insert(قطار.begin()+idx, value);
      }
}

فراغ vvp_قطار_real::push_back(مزدوج value, بنانشان max_سائيز)
{
      إذا (!max_سائيز || (قطار.size() < max_سائيز))
	    قطار.push_back(value);
      وإلا
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<real> ["
	         << max_سائيز << "]." << سٽ_پڄاڻي;
}

فراغ vvp_قطار_real::push_front(مزدوج value, بنانشان max_سائيز)
{
      إذا (max_سائيز && (قطار.size() == max_سائيز)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << قطار.back() << " from already full bounded queue<real> ["
	         << max_سائيز << "]." << سٽ_پڄاڻي;
	    قطار.pop_back();
      }
      قطار.push_front(value);
}

فراغ vvp_قطار_real::erase(بنانشان idx)
{
      أكد(قطار.size() > idx);
      قطار.erase(قطار.begin()+idx);
}

فراغ vvp_قطار_real::erase_پڇ(بنانشان idx)
{
      أكد(قطار.size() >= idx);
      إذا (قطار.size() > idx)
	    قطار.resize(idx);
}

فراغ vvp_قطار_string::copy_elems(vvp_object_t src, بنانشان max_سائيز)
{
      إذا (vvp_قطار*src_قطار = src.peek<vvp_قطار>())
	    copy_elements<string, vvp_قطار_string, vvp_قطار>(هذا, src_قطار, max_سائيز);
      وإلا إذا (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<string, vvp_قطار_string, vvp_darray>(هذا, src_darray, max_سائيز);
      وإلا
	    cerr << get_fileline() << "Sorry: cannot copy object to string queue." << سٽ_پڄاڻي;
}

فراغ vvp_قطار_string::سيٽ_لفظ_max(بنانشان adr, ثابت string&value, بنانشان max_سائيز)
{
      إذا (adr == قطار.size())
	    إذا (!max_سائيز || (قطار.size() < max_سائيز))
		  قطار.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: assigning to queue<string>[" << adr << "] is"
		          " outside bound (" << max_سائيز << "). \"" << value
		       << "\" was not added." << سٽ_پڄاڻي;
      وإلا
	    سيٽ_لفظ(adr, value);
}

فراغ vvp_قطار_string::سيٽ_لفظ(بنانشان adr, ثابت string&value)
{
      إذا (adr < قطار.size())
	    قطار[adr] = value;
      وإلا
	    cerr << get_fileline()
	         << "Warning: assigning to queue<string>[" << adr << "] is outside "
	            "of size (" << قطار.size() << "). \"" << value
	         << "\" was not added." << سٽ_پڄاڻي;
}

فراغ vvp_قطار_string::get_لفظ(بنانشان adr, string&value)
{
      إذا (adr >= قطار.size())
	    value = "";
      وإلا
	    value = قطار[adr];
}

فراغ vvp_قطار_string::insert(بنانشان idx, ثابت string&value, بنانشان max_سائيز)
{
	// Inserting past the end of the queue
      إذا (idx > قطار.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<string>[" << idx << "] is "
	            "outside of size (" << قطار.size() << "). \"" << value
	         << "\" was not added." << سٽ_پڄاڻي;
	// Inserting at the end
      وإلا إذا (idx == قطار.size())
	    إذا (!max_سائيز || (قطار.size() < max_سائيز))
		  قطار.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: inserting to queue<string>[" << idx << "] is"
		          " outside bound (" << max_سائيز << "). \"" << value
		       << "\" was not added." << سٽ_پڄاڻي;
      وإلا  {
	    إذا (max_سائيز && (قطار.size() == max_سائيز)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", \"" << value << "\") removed \""
		       << قطار.back() << "\" from already full bounded queue<string> ["
		       << max_سائيز << "]." << سٽ_پڄاڻي;
		  قطار.pop_back();
	    }
	    قطار.insert(قطار.begin()+idx, value);
      }
}

فراغ vvp_قطار_string::push_back(ثابت string&value, بنانشان max_سائيز)
{
      إذا (!max_سائيز || (قطار.size() < max_سائيز))
	    قطار.push_back(value);
      وإلا
	    cerr << get_fileline()
	         << "Warning: push_back(\"" << value
	         << "\") skipped for already full bounded queue<string> ["
	         << max_سائيز << "]." << سٽ_پڄاڻي;
}

فراغ vvp_قطار_string::push_front(ثابت string&value, بنانشان max_سائيز)
{
      إذا (max_سائيز && (قطار.size() == max_سائيز)) {
	    cerr << get_fileline()
	         << "Warning: push_front(\"" << value << "\") removed \""
	         << قطار.back() << "\" from already full bounded queue<string> ["
	         << max_سائيز << "]." << سٽ_پڄاڻي;
	    قطار.pop_back();
      }
      قطار.push_front(value);
}

فراغ vvp_قطار_string::erase(بنانشان idx)
{
      أكد(قطار.size() > idx);
      قطار.erase(قطار.begin()+idx);
}

فراغ vvp_قطار_string::erase_پڇ(بنانشان idx)
{
      أكد(قطار.size() >= idx);
      إذا (قطار.size() > idx)
	    قطار.resize(idx);
}

فراغ vvp_قطار_real::ابتو_elems(فراغ)
{
      std::reverse(قطار.begin(), قطار.پڇاڙي());
}

فراغ vvp_قطار_string::ابتو_elems(فراغ)
{
      std::reverse(قطار.begin(), قطار.پڇاڙي());
}

فراغ vvp_قطار_real::ترتيب_ڏيو_elems(منطقي ascending)
{
      إذا (ascending) std::sort(قطار.begin(), قطار.پڇاڙي());
      وإلا std::sort(قطار.begin(), قطار.پڇاڙي(), std::greater<مزدوج>());
}

فراغ vvp_قطار_real::shuffle_elems(فراغ)
{
      std::shuffle(قطار.begin(), قطار.پڇاڙي(), shuffle_rng());
}

فراغ vvp_قطار_string::ترتيب_ڏيو_elems(منطقي ascending)
{
      إذا (ascending) std::sort(قطار.begin(), قطار.پڇاڙي());
      وإلا std::sort(قطار.begin(), قطار.پڇاڙي(), std::greater<string>());
}

فراغ vvp_قطار_string::shuffle_elems(فراغ)
{
      std::shuffle(قطار.begin(), قطار.پڇاڙي(), shuffle_rng());
}

فراغ vvp_قطار_vec4::copy_elems(vvp_object_t src, بنانشان max_سائيز)
{
      إذا (vvp_قطار*src_قطار = src.peek<vvp_قطار>())
	    copy_elements<vvp_vector4_t, vvp_قطار_vec4, vvp_قطار>(هذا, src_قطار, max_سائيز);
      وإلا إذا (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<vvp_vector4_t, vvp_قطار_vec4, vvp_darray>(هذا, src_darray, max_سائيز);
      وإلا
	    cerr << get_fileline() << "Sorry: cannot copy object to vector queue." << سٽ_پڄاڻي;
}

فراغ vvp_قطار_vec4::سيٽ_لفظ_max(بنانشان adr, ثابت vvp_vector4_t&value, بنانشان max_سائيز)
{
      إذا (adr == قطار.size())
	    إذا (!max_سائيز || (قطار.size() < max_سائيز))
		  قطار.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: assigning to queue<vector>[" << adr << "] is"
		          " outside bound (" << max_سائيز << "). " << value
		       << " was not added." << سٽ_پڄاڻي;
      وإلا
	    سيٽ_لفظ(adr, value);
}

فراغ vvp_قطار_vec4::سيٽ_لفظ(بنانشان adr, ثابت vvp_vector4_t&value)
{
      إذا (adr < قطار.size())
	    قطار[adr] = value;
      وإلا
	    cerr << get_fileline()
	         << "Warning: assigning to queue<vector>[" << adr << "] is outside "
	            "of size (" << قطار.size() << "). " << value
	         << " was not added." << سٽ_پڄاڻي;
}

فراغ vvp_قطار_vec4::get_لفظ(بنانشان adr, vvp_vector4_t&value)
{
      إذا (adr >= قطار.size())
	    value = vvp_vector4_t(قطار[0].size());
      وإلا
	    value = قطار[adr];
}

فراغ vvp_قطار_vec4::insert(بنانشان idx, ثابت vvp_vector4_t&value, بنانشان max_سائيز)
{
	// Inserting past the end of the queue
      إذا (idx > قطار.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<vector[" << value.size()
	         << "]>[" << idx << "] is outside of size (" << قطار.size()
	         << "). " << value << " was not added." << سٽ_پڄاڻي;
	// Inserting at the end
      وإلا إذا (idx == قطار.size())
	    إذا (!max_سائيز || (قطار.size() < max_سائيز))
		  قطار.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: inserting to queue<vector[" << value.size()
		       << "]>[" << idx << "] is outside bound (" << max_سائيز
		       << "). " << value << " was not added." << سٽ_پڄاڻي;
      وإلا  {
	    إذا (max_سائيز && (قطار.size() == max_سائيز)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << قطار.back() << " from already full bounded queue<vector["
		       << value.size() << "]> [" << max_سائيز << "]." << سٽ_پڄاڻي;
		  قطار.pop_back();
	    }
	    قطار.insert(قطار.begin()+idx, value);
      }
}

فراغ vvp_قطار_vec4::push_back(ثابت vvp_vector4_t&value, بنانشان max_سائيز)
{
      إذا (!max_سائيز || (قطار.size() < max_سائيز))
	    قطار.push_back(value);
      وإلا
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<vector["
	         << value.size() << "]> [" << max_سائيز << "]." << سٽ_پڄاڻي;
}

فراغ vvp_قطار_vec4::push_front(ثابت vvp_vector4_t&value, بنانشان max_سائيز)
{
      إذا (max_سائيز && (قطار.size() == max_سائيز)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << قطار.back() << " from already full bounded queue<vector["
	         << value.size() << "]> [" << max_سائيز << "]." << سٽ_پڄاڻي;
	    قطار.pop_back();
      }
      قطار.push_front(value);
}

فراغ vvp_قطار_vec4::erase(بنانشان idx)
{
      أكد(قطار.size() > idx);
      قطار.erase(قطار.begin()+idx);
}

فراغ vvp_قطار_vec4::erase_پڇ(بنانشان idx)
{
      أكد(قطار.size() >= idx);
      إذا (قطار.size() > idx)
	    قطار.resize(idx);
}

فراغ vvp_قطار_vec4::ابتو_elems(فراغ)
{
      std::reverse(قطار.begin(), قطار.پڇاڙي());
}

فراغ vvp_قطار_vec4::ترتيب_ڏيو_elems(منطقي ascending)
{
      إذا (ascending) {
	    std::sort(قطار.begin(), قطار.پڇاڙي(), vec4_lt_unsigned);
      } وإلا {
	    std::sort(قطار.begin(), قطار.پڇاڙي(),
		      [](ثابت vvp_vector4_t&a, ثابت vvp_vector4_t&b) {
			    أرجع vec4_lt_unsigned(b, a);
		      });
      }
}

فراغ vvp_قطار_vec4::shuffle_elems(فراغ)
{
      std::shuffle(قطار.begin(), قطار.پڇاڙي(), shuffle_rng());
}
