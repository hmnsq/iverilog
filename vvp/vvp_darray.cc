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

استفاده نطاق std;

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

فراغ vvp_darray::معکوس_elems(فراغ)
{
      cerr << "XXXX reverse_elems() not implemented for "
           << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_darray::مرتب_سازی_elems(منطقي /*ascending*/)
{
      cerr << "XXXX sort_elems() not implemented for "
           << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_darray::shuffle_elems(فراغ)
{
      cerr << "XXXX shuffle_elems() not implemented for "
           << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_darray::مجموعه_کلمه(بیعلامت, ثابت vvp_vector4_t&)
{
      cerr << "XXXX set_word(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_darray::مجموعه_کلمه(بیعلامت, مزدوج)
{
      cerr << "XXXX set_word(double) not implemented for " << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_darray::مجموعه_کلمه(بیعلامت, ثابت string&)
{
      cerr << "XXXX set_word(string) not implemented for " << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_darray::مجموعه_کلمه(بیعلامت, ثابت vvp_object_t&)
{
      cerr << "XXXX set_word(vvp_object_t) not implemented for " << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_darray::get_کلمه(بیعلامت, vvp_vector4_t&)
{
      cerr << "XXXX get_word(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_darray::get_کلمه(بیعلامت, مزدوج&)
{
      cerr << "XXXX get_word(double) not implemented for " << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_darray::get_کلمه(بیعلامت, string&)
{
      cerr << "XXXX get_word(string) not implemented for " << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_darray::get_کلمه(بیعلامت, vvp_object_t&)
{
      cerr << "XXXX get_word(vvp_object_t) not implemented for " << typeid(*هذا).name() << پایان_سطر;
}

vvp_vector4_t vvp_darray::get_bitstream(منطقي)
{
      cerr << "XXXX get_bitstream() not implemented for " << typeid(*هذا).name() << پایان_سطر;
      أرجع vvp_vector4_t();
}

قالب <صنف TYPE> vvp_darray_atom<TYPE>::~vvp_darray_atom()
{
}

قالب <صنف TYPE> size_t vvp_darray_atom<TYPE>::get_اندازه() ثابت
{
      أرجع array_.size();
}

قالب <صنف TYPE> فراغ vvp_darray_atom<TYPE>::معکوس_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

قالب <صنف TYPE> فراغ vvp_darray_atom<TYPE>::مرتب_سازی_elems(منطقي ascending)
{
      إذا (ascending) std::sort(array_.begin(), array_.انتها());
      وإلا std::sort(array_.begin(), array_.انتها(), std::greater<TYPE>());
}

قالب <صنف TYPE> فراغ vvp_darray_atom<TYPE>::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.انتها(), shuffle_rng());
}

قالب <صنف TYPE> فراغ vvp_darray_atom<TYPE>::مجموعه_کلمه(بیعلامت adr, ثابت vvp_vector4_t&value)
{
      إذا (adr >= array_.size())
	    أرجع;
      TYPE tmp;
      vector4_مقصد_مقدار(value, tmp, درست, خطأ);
      array_[adr] = tmp;
}

قالب <صنف TYPE> فراغ vvp_darray_atom<TYPE>::get_کلمه(بیعلامت adr, vvp_vector4_t&value)
{
      إذا (adr >= array_.size()) {
	    value = vvp_vector4_t(8*حجم(TYPE), BIT4_X);
	    أرجع;
      }

      TYPE کلمه = array_[adr];
      vvp_vector4_t tmp (8*حجم(TYPE), BIT4_0);
      لكل (بیعلامت idx = 0 ; idx < tmp.size() ; idx += 1) {
	    إذا (کلمه&1) tmp.مجموعه_بیت(idx, BIT4_1);
	    کلمه >>= 1;
      }
      value = tmp;
}

قالب <صنف TYPE> فراغ vvp_darray_atom<TYPE>::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_atom<TYPE>*that = dynamic_cast<ثابت vvp_darray_atom<TYPE>*>(obj);
      أكد(that);

      بیعلامت رقم_اقلام = min(array_.size(), that->array_.size());
      لكل (بیعلامت idx = 0 ; idx < رقم_اقلام ; idx += 1)
	    array_[idx] = that->array_[idx];
}

قالب <صنف TYPE> vvp_object* vvp_darray_atom<TYPE>::duplicate(فراغ) ثابت
{
      vvp_darray_atom<TYPE>*that = جديد vvp_darray_atom<TYPE>(array_.size());
      لكل (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      أرجع that;
}

قالب <صنف TYPE> vvp_vector4_t vvp_darray_atom<TYPE>::get_bitstream(منطقي)
{
      ثابت بیعلامت کلمه_wid = حجم(TYPE) * 8;

      vvp_vector4_t vec(array_.size() * کلمه_wid, BIT4_0);

      بیعلامت adx = 0;
      بیعلامت vdx = vec.size();
      طالما (vdx > 0) {
            TYPE کلمه = array_[adx++];
            vdx -= کلمه_wid;
            لكل (بیعلامت bdx = 0; bdx < کلمه_wid; bdx += 1) {
                  إذا (کلمه & 1)
                        vec.مجموعه_بیت(vdx+bdx, BIT4_1);
                  کلمه >>= 1;
            }
      }

      أرجع vec;
}

قالب صنف vvp_darray_atom<uint8_t>;
قالب صنف vvp_darray_atom<uint16_t>;
قالب صنف vvp_darray_atom<uint32_t>;
قالب صنف vvp_darray_atom<uint64_t>;
قالب صنف vvp_darray_atom<int8_t>;
قالب صنف vvp_darray_atom<int16_t>;
قالب صنف vvp_darray_atom<int32_t>;
قالب صنف vvp_darray_atom<int64_t>;

vvp_darray_vec4::~vvp_darray_vec4()
{
}

size_t vvp_darray_vec4::get_اندازه(فراغ) ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_vec4::مجموعه_کلمه(بیعلامت adr, ثابت vvp_vector4_t&value)
{
      إذا (adr >= array_.size()) أرجع;
      أكد(value.size() == کلمه_wid_2);
      array_[adr] = value;
}

فراغ vvp_darray_vec4::get_کلمه(بیعلامت adr, vvp_vector4_t&value)
{
	/*
	 * Return an undefined value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      إذا ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(کلمه_wid_2, BIT4_X);
	    أرجع;
      }
      value = array_[adr];
      أكد(value.size() == کلمه_wid_2);
}

فراغ vvp_darray_vec4::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_vec4*that = dynamic_cast<ثابت vvp_darray_vec4*>(obj);
      أكد(that);

      بیعلامت رقم_اقلام = min(array_.size(), that->array_.size());
      لكل (بیعلامت idx = 0 ; idx < رقم_اقلام ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_object* vvp_darray_vec4::duplicate(فراغ) ثابت
{
      vvp_darray_vec4*that = جديد vvp_darray_vec4(array_.size(), کلمه_wid_2);

      لكل (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      أرجع that;
}

vvp_vector4_t vvp_darray_vec4::get_bitstream(منطقي as_vec4)
{
      vvp_vector4_t vec(array_.size() * کلمه_wid_2, BIT4_0);

      بیعلامت adx = 0;
      بیعلامت vdx = vec.size();
      طالما (vdx > 0) {
            vdx -= کلمه_wid_2;
            لكل (بیعلامت bdx = 0; bdx < کلمه_wid_2; bdx += 1) {
                  vvp_bit4_t بیت = array_[adx].value(bdx);
                  إذا (as_vec4 || (بیت == BIT4_1))
                        vec.مجموعه_بیت(vdx+bdx, بیت);
            }
            adx++;
      }

      أرجع vec;
}

فراغ vvp_darray_vec4::معکوس_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_vec4::مرتب_سازی_elems(منطقي ascending)
{
      إذا (ascending) {
	    std::sort(array_.begin(), array_.انتها(), vec4_lt_unsigned);
      } وإلا {
	    std::sort(array_.begin(), array_.انتها(),
		      [](ثابت vvp_vector4_t&a, ثابت vvp_vector4_t&b) {
			    أرجع vec4_lt_unsigned(b, a);
		      });
      }
}

فراغ vvp_darray_vec4::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.انتها(), shuffle_rng());
}

فراغ vvp_darray_vec2::معکوس_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_vec2::مرتب_سازی_elems(منطقي ascending)
{
      إذا (ascending) {
	    std::sort(array_.begin(), array_.انتها());
      } وإلا {
	    std::sort(array_.begin(), array_.انتها(),
		      [](ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b) {
			    أرجع b < a;
		      });
      }
}

فراغ vvp_darray_vec2::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.انتها(), shuffle_rng());
}

vvp_darray_vec2::~vvp_darray_vec2()
{
}

size_t vvp_darray_vec2::get_اندازه(فراغ) ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_vec2::مجموعه_کلمه(بیعلامت adr, ثابت vvp_vector4_t&value)
{
      إذا (adr >= array_.size()) أرجع;
      أكد(value.size() == کلمه_wid_2);
      array_[adr] = value;
}

فراغ vvp_darray_vec2::get_کلمه(بیعلامت adr, vvp_vector4_t&value)
{
	/*
	 * Return a zero value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      إذا ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(کلمه_wid_2, BIT4_0);
	    أرجع;
      }
      أكد(array_[adr].size() == کلمه_wid_2);
      value.resize(کلمه_wid_2);
      لكل (بیعلامت idx = 0; idx < کلمه_wid_2; idx += 1) {
	    value.مجموعه_بیت(idx, array_[adr].value4(idx));
      }
}

فراغ vvp_darray_vec2::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_vec2*that = dynamic_cast<ثابت vvp_darray_vec2*>(obj);
      أكد(that);

      بیعلامت رقم_اقلام = min(array_.size(), that->array_.size());
      لكل (بیعلامت idx = 0 ; idx < رقم_اقلام ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_vector4_t vvp_darray_vec2::get_bitstream(منطقي)
{
      vvp_vector4_t vec(array_.size() * کلمه_wid_2, BIT4_0);

      بیعلامت adx = 0;
      بیعلامت vdx = vec.size();
      طالما (vdx > 0) {
            vdx -= کلمه_wid_2;
            لكل (بیعلامت bdx = 0; bdx < کلمه_wid_2; bdx += 1) {
                  إذا (array_[adx].value(bdx))
                        vec.مجموعه_بیت(vdx+bdx, BIT4_1);
            }
            adx++;
      }

      أرجع vec;
}

vvp_darray_object::~vvp_darray_object()
{
}

size_t vvp_darray_object::get_اندازه() ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_object::مجموعه_کلمه(بیعلامت adr, ثابت vvp_object_t&value)
{
      إذا (adr >= array_.size())
	    أرجع;
      array_[adr] = value;
}

فراغ vvp_darray_object::get_کلمه(بیعلامت adr, vvp_object_t&value)
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

      بیعلامت رقم_اقلام = min(array_.size(), that->array_.size());
      لكل (بیعلامت idx = 0 ; idx < رقم_اقلام ; idx += 1)
	    array_[idx] = that->array_[idx];
}

فراغ vvp_darray_object::معکوس_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_object::مرتب_سازی_elems(منطقي ascending)
{
      إذا (ascending) {
	    std::sort(array_.begin(), array_.انتها(),
		      [](ثابت vvp_object_t&a, ثابت vvp_object_t&b) {
			    أرجع a.مرتب_سازی_ترتیب_کلید() < b.مرتب_سازی_ترتیب_کلید();
		      });
      } وإلا {
	    std::sort(array_.begin(), array_.انتها(),
		      [](ثابت vvp_object_t&a, ثابت vvp_object_t&b) {
			    أرجع a.مرتب_سازی_ترتیب_کلید() > b.مرتب_سازی_ترتیب_کلید();
		      });
      }
}

فراغ vvp_darray_object::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.انتها(), shuffle_rng());
}

vvp_darray_real::~vvp_darray_real()
{
}

size_t vvp_darray_real::get_اندازه() ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_real::مجموعه_کلمه(بیعلامت adr, مزدوج value)
{
      إذا (adr >= array_.size())
	    أرجع;
      array_[adr] = value;
}

فراغ vvp_darray_real::get_کلمه(بیعلامت adr, مزدوج&value)
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

      بیعلامت رقم_اقلام = min(array_.size(), that->array_.size());
      لكل (بیعلامت idx = 0 ; idx < رقم_اقلام ; idx += 1)
	    array_[idx] = that->array_[idx];
}

فراغ vvp_darray_real::معکوس_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_real::مرتب_سازی_elems(منطقي ascending)
{
      إذا (ascending) std::sort(array_.begin(), array_.انتها());
      وإلا std::sort(array_.begin(), array_.انتها(), std::greater<مزدوج>());
}

فراغ vvp_darray_real::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.انتها(), shuffle_rng());
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
      ثابت بیعلامت کلمه_wid = حجم(مزدوج) * 8;
      أكد(کلمه_wid == 64);

      vvp_vector4_t vec(array_.size() * کلمه_wid, BIT4_0);

      بیعلامت adx = 0;
      بیعلامت vdx = vec.size();
      طالما (vdx > 0) {
            اتحاد {
                مزدوج   value;
                uint64_t بیتها;
            } کلمه;
            کلمه.value = array_[adx++];
            vdx -= کلمه_wid;
            لكل (بیعلامت bdx = 0; bdx < کلمه_wid; bdx += 1) {
                  إذا (کلمه.بیتها & 1)
                        vec.مجموعه_بیت(vdx+bdx, BIT4_1);
                  کلمه.بیتها >>= 1;
            }
      }

      أرجع vec;
}

vvp_darray_string::~vvp_darray_string()
{
}

size_t vvp_darray_string::get_اندازه() ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_string::مجموعه_کلمه(بیعلامت adr, ثابت string&value)
{
      إذا (adr >= array_.size())
	    أرجع;
      array_[adr] = value;
}

فراغ vvp_darray_string::get_کلمه(بیعلامت adr, string&value)
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

      بیعلامت رقم_اقلام = min(array_.size(), that->array_.size());
      لكل (بیعلامت idx = 0 ; idx < رقم_اقلام ; idx += 1)
	    array_[idx] = that->array_[idx];
}

فراغ vvp_darray_string::معکوس_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_string::مرتب_سازی_elems(منطقي ascending)
{
      إذا (ascending) std::sort(array_.begin(), array_.انتها());
      وإلا std::sort(array_.begin(), array_.انتها(), std::greater<string>());
}

فراغ vvp_darray_string::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.انتها(), shuffle_rng());
}

vvp_object* vvp_darray_string::duplicate(فراغ) ثابت
{
      vvp_darray_string*that = جديد vvp_darray_string(array_.size());

      لكل (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      أرجع that;
}

vvp_صف::~vvp_صف()
{
}

فراغ vvp_صف::copy_elems(vvp_object_t, بیعلامت)
{
      cerr << "Sorry: copy_elems() not implemented for " << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_صف::مجموعه_کلمه_max(بیعلامت, ثابت vvp_vector4_t&, بیعلامت)
{
      cerr << "XXXX set_word_max(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_صف::insert(بیعلامت, ثابت vvp_vector4_t&, بیعلامت)
{
      cerr << "XXXX insert(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_صف::push_back(ثابت vvp_vector4_t&, بیعلامت)
{
      cerr << "XXXX push_back(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_صف::push_front(ثابت vvp_vector4_t&, بیعلامت)
{
      cerr << "XXXX push_front(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_صف::مجموعه_کلمه_max(بیعلامت, مزدوج, بیعلامت)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_صف::insert(بیعلامت, مزدوج, بیعلامت)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_صف::push_back(مزدوج, بیعلامت)
{
      cerr << "XXXX push_back(double) not implemented for " << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_صف::push_front(مزدوج, بیعلامت)
{
      cerr << "XXXX push_front(double) not implemented for " << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_صف::مجموعه_کلمه_max(بیعلامت, ثابت string&, بیعلامت)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_صف::insert(بیعلامت, ثابت string&, بیعلامت)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_صف::push_back(ثابت string&, بیعلامت)
{
      cerr << "XXXX push_back(string) not implemented for " << typeid(*هذا).name() << پایان_سطر;
}

فراغ vvp_صف::push_front(ثابت string&, بیعلامت)
{
      cerr << "XXXX push_front(string) not implemented for " << typeid(*هذا).name() << پایان_سطر;
}

/*
 * Helper functions used while copying multiple elements into a queue.
 */
ساكن فراغ print_copy_is_too_big(size_t src_اندازه, بیعلامت max_اندازه, ثابت string&qtype)
{
      cerr << get_fileline()
           << "Warning: queue<" << qtype << "> is bounded to have at most "
           << max_اندازه << " elements, source has " << src_اندازه << " elements." << پایان_سطر;
}

ساكن فراغ print_copy_is_too_big(مزدوج&, size_t src_اندازه, بیعلامت max_اندازه)
{
      print_copy_is_too_big(src_اندازه, max_اندازه, "real");
}

ساكن فراغ print_copy_is_too_big(string&, size_t src_اندازه, بیعلامت max_اندازه)
{
      print_copy_is_too_big(src_اندازه, max_اندازه, "string");
}

ساكن فراغ print_copy_is_too_big(vvp_vector4_t&, size_t src_اندازه, بیعلامت max_اندازه)
{
      print_copy_is_too_big(src_اندازه, max_اندازه, "vector");
}

قالب <نام_نوع ELEM, صنف QTYPE, صنف SRC_TYPE>
ساكن فراغ copy_elements(QTYPE*صف, SRC_TYPE*src, بیعلامت max_اندازه)
{
      size_t src_اندازه = src->get_اندازه();
      إذا ((max_اندازه != 0) && (src_اندازه > max_اندازه)) {
	    ELEM tmp;
	    print_copy_is_too_big(tmp, src_اندازه, max_اندازه);
      }
      بیعلامت copy_اندازه = ((src_اندازه < max_اندازه) ||
                            (max_اندازه == 0)) ? src_اندازه : max_اندازه;
      إذا (copy_اندازه < صف->get_اندازه())
	    صف->erase_دم(copy_اندازه);
      لكل (بیعلامت idx=0; idx < copy_اندازه; ++idx) {
	    ELEM value;
	    src->get_کلمه(idx, value);
	    صف->مجموعه_کلمه_max(idx, value, max_اندازه);
      }
}

فراغ vvp_صف_real::copy_elems(vvp_object_t src, بیعلامت max_اندازه)
{
      إذا (vvp_صف*src_صف = src.peek<vvp_صف>())
	    copy_elements<مزدوج, vvp_صف_real, vvp_صف>(هذا, src_صف, max_اندازه);
      وإلا إذا (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<مزدوج, vvp_صف_real, vvp_darray>(هذا, src_darray, max_اندازه);
      وإلا
	    cerr << get_fileline() << "Sorry: cannot copy object to real queue." << پایان_سطر;
}

فراغ vvp_صف_real::مجموعه_کلمه_max(بیعلامت adr, مزدوج value, بیعلامت max_اندازه)
{
      إذا (adr == صف.size())
	    إذا (!max_اندازه || (صف.size() < max_اندازه))
		  صف.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: assigning to queue<real>[" << adr << "] is"
		          " outside bound (" << max_اندازه << "). " << value
		       << " was not added." << پایان_سطر;
      وإلا
	    مجموعه_کلمه(adr, value);
}

فراغ vvp_صف_real::مجموعه_کلمه(بیعلامت adr, مزدوج value)
{
      إذا (adr < صف.size())
	    صف[adr] = value;
      وإلا
	    cerr << get_fileline()
	         << "Warning: assigning to queue<real>[" << adr << "] is outside "
	            "of size (" << صف.size() << "). " << value
	         << " was not added." << پایان_سطر;
}

فراغ vvp_صف_real::get_کلمه(بیعلامت adr, مزدوج&value)
{
      إذا (adr >= صف.size())
	    value = 0.0;
      وإلا
	    value = صف[adr];
}

فراغ vvp_صف_real::insert(بیعلامت idx, مزدوج value, بیعلامت max_اندازه)
{
	// Inserting past the end of the queue
      إذا (idx > صف.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<real>[" << idx << "] is "
	            "outside of size (" << صف.size() << "). " << value
	         << " was not added." << پایان_سطر;
	// Inserting at the end
      وإلا إذا (idx == صف.size())
	    إذا (!max_اندازه || (صف.size() < max_اندازه))
		  صف.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: inserting to queue<real>[" << idx << "] is"
		          " outside bound (" << max_اندازه << "). " << value
		       << " was not added." << پایان_سطر;
      وإلا  {
	    إذا (max_اندازه && (صف.size() == max_اندازه)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << صف.back() << " from already full bounded queue<real> ["
		       << max_اندازه << "]." << پایان_سطر;
		  صف.pop_back();
	    }
	    صف.insert(صف.begin()+idx, value);
      }
}

فراغ vvp_صف_real::push_back(مزدوج value, بیعلامت max_اندازه)
{
      إذا (!max_اندازه || (صف.size() < max_اندازه))
	    صف.push_back(value);
      وإلا
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<real> ["
	         << max_اندازه << "]." << پایان_سطر;
}

فراغ vvp_صف_real::push_front(مزدوج value, بیعلامت max_اندازه)
{
      إذا (max_اندازه && (صف.size() == max_اندازه)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << صف.back() << " from already full bounded queue<real> ["
	         << max_اندازه << "]." << پایان_سطر;
	    صف.pop_back();
      }
      صف.push_front(value);
}

فراغ vvp_صف_real::erase(بیعلامت idx)
{
      أكد(صف.size() > idx);
      صف.erase(صف.begin()+idx);
}

فراغ vvp_صف_real::erase_دم(بیعلامت idx)
{
      أكد(صف.size() >= idx);
      إذا (صف.size() > idx)
	    صف.resize(idx);
}

فراغ vvp_صف_string::copy_elems(vvp_object_t src, بیعلامت max_اندازه)
{
      إذا (vvp_صف*src_صف = src.peek<vvp_صف>())
	    copy_elements<string, vvp_صف_string, vvp_صف>(هذا, src_صف, max_اندازه);
      وإلا إذا (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<string, vvp_صف_string, vvp_darray>(هذا, src_darray, max_اندازه);
      وإلا
	    cerr << get_fileline() << "Sorry: cannot copy object to string queue." << پایان_سطر;
}

فراغ vvp_صف_string::مجموعه_کلمه_max(بیعلامت adr, ثابت string&value, بیعلامت max_اندازه)
{
      إذا (adr == صف.size())
	    إذا (!max_اندازه || (صف.size() < max_اندازه))
		  صف.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: assigning to queue<string>[" << adr << "] is"
		          " outside bound (" << max_اندازه << "). \"" << value
		       << "\" was not added." << پایان_سطر;
      وإلا
	    مجموعه_کلمه(adr, value);
}

فراغ vvp_صف_string::مجموعه_کلمه(بیعلامت adr, ثابت string&value)
{
      إذا (adr < صف.size())
	    صف[adr] = value;
      وإلا
	    cerr << get_fileline()
	         << "Warning: assigning to queue<string>[" << adr << "] is outside "
	            "of size (" << صف.size() << "). \"" << value
	         << "\" was not added." << پایان_سطر;
}

فراغ vvp_صف_string::get_کلمه(بیعلامت adr, string&value)
{
      إذا (adr >= صف.size())
	    value = "";
      وإلا
	    value = صف[adr];
}

فراغ vvp_صف_string::insert(بیعلامت idx, ثابت string&value, بیعلامت max_اندازه)
{
	// Inserting past the end of the queue
      إذا (idx > صف.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<string>[" << idx << "] is "
	            "outside of size (" << صف.size() << "). \"" << value
	         << "\" was not added." << پایان_سطر;
	// Inserting at the end
      وإلا إذا (idx == صف.size())
	    إذا (!max_اندازه || (صف.size() < max_اندازه))
		  صف.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: inserting to queue<string>[" << idx << "] is"
		          " outside bound (" << max_اندازه << "). \"" << value
		       << "\" was not added." << پایان_سطر;
      وإلا  {
	    إذا (max_اندازه && (صف.size() == max_اندازه)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", \"" << value << "\") removed \""
		       << صف.back() << "\" from already full bounded queue<string> ["
		       << max_اندازه << "]." << پایان_سطر;
		  صف.pop_back();
	    }
	    صف.insert(صف.begin()+idx, value);
      }
}

فراغ vvp_صف_string::push_back(ثابت string&value, بیعلامت max_اندازه)
{
      إذا (!max_اندازه || (صف.size() < max_اندازه))
	    صف.push_back(value);
      وإلا
	    cerr << get_fileline()
	         << "Warning: push_back(\"" << value
	         << "\") skipped for already full bounded queue<string> ["
	         << max_اندازه << "]." << پایان_سطر;
}

فراغ vvp_صف_string::push_front(ثابت string&value, بیعلامت max_اندازه)
{
      إذا (max_اندازه && (صف.size() == max_اندازه)) {
	    cerr << get_fileline()
	         << "Warning: push_front(\"" << value << "\") removed \""
	         << صف.back() << "\" from already full bounded queue<string> ["
	         << max_اندازه << "]." << پایان_سطر;
	    صف.pop_back();
      }
      صف.push_front(value);
}

فراغ vvp_صف_string::erase(بیعلامت idx)
{
      أكد(صف.size() > idx);
      صف.erase(صف.begin()+idx);
}

فراغ vvp_صف_string::erase_دم(بیعلامت idx)
{
      أكد(صف.size() >= idx);
      إذا (صف.size() > idx)
	    صف.resize(idx);
}

فراغ vvp_صف_real::معکوس_elems(فراغ)
{
      std::reverse(صف.begin(), صف.انتها());
}

فراغ vvp_صف_string::معکوس_elems(فراغ)
{
      std::reverse(صف.begin(), صف.انتها());
}

فراغ vvp_صف_real::مرتب_سازی_elems(منطقي ascending)
{
      إذا (ascending) std::sort(صف.begin(), صف.انتها());
      وإلا std::sort(صف.begin(), صف.انتها(), std::greater<مزدوج>());
}

فراغ vvp_صف_real::shuffle_elems(فراغ)
{
      std::shuffle(صف.begin(), صف.انتها(), shuffle_rng());
}

فراغ vvp_صف_string::مرتب_سازی_elems(منطقي ascending)
{
      إذا (ascending) std::sort(صف.begin(), صف.انتها());
      وإلا std::sort(صف.begin(), صف.انتها(), std::greater<string>());
}

فراغ vvp_صف_string::shuffle_elems(فراغ)
{
      std::shuffle(صف.begin(), صف.انتها(), shuffle_rng());
}

فراغ vvp_صف_vec4::copy_elems(vvp_object_t src, بیعلامت max_اندازه)
{
      إذا (vvp_صف*src_صف = src.peek<vvp_صف>())
	    copy_elements<vvp_vector4_t, vvp_صف_vec4, vvp_صف>(هذا, src_صف, max_اندازه);
      وإلا إذا (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<vvp_vector4_t, vvp_صف_vec4, vvp_darray>(هذا, src_darray, max_اندازه);
      وإلا
	    cerr << get_fileline() << "Sorry: cannot copy object to vector queue." << پایان_سطر;
}

فراغ vvp_صف_vec4::مجموعه_کلمه_max(بیعلامت adr, ثابت vvp_vector4_t&value, بیعلامت max_اندازه)
{
      إذا (adr == صف.size())
	    إذا (!max_اندازه || (صف.size() < max_اندازه))
		  صف.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: assigning to queue<vector>[" << adr << "] is"
		          " outside bound (" << max_اندازه << "). " << value
		       << " was not added." << پایان_سطر;
      وإلا
	    مجموعه_کلمه(adr, value);
}

فراغ vvp_صف_vec4::مجموعه_کلمه(بیعلامت adr, ثابت vvp_vector4_t&value)
{
      إذا (adr < صف.size())
	    صف[adr] = value;
      وإلا
	    cerr << get_fileline()
	         << "Warning: assigning to queue<vector>[" << adr << "] is outside "
	            "of size (" << صف.size() << "). " << value
	         << " was not added." << پایان_سطر;
}

فراغ vvp_صف_vec4::get_کلمه(بیعلامت adr, vvp_vector4_t&value)
{
      إذا (adr >= صف.size())
	    value = vvp_vector4_t(صف[0].size());
      وإلا
	    value = صف[adr];
}

فراغ vvp_صف_vec4::insert(بیعلامت idx, ثابت vvp_vector4_t&value, بیعلامت max_اندازه)
{
	// Inserting past the end of the queue
      إذا (idx > صف.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<vector[" << value.size()
	         << "]>[" << idx << "] is outside of size (" << صف.size()
	         << "). " << value << " was not added." << پایان_سطر;
	// Inserting at the end
      وإلا إذا (idx == صف.size())
	    إذا (!max_اندازه || (صف.size() < max_اندازه))
		  صف.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: inserting to queue<vector[" << value.size()
		       << "]>[" << idx << "] is outside bound (" << max_اندازه
		       << "). " << value << " was not added." << پایان_سطر;
      وإلا  {
	    إذا (max_اندازه && (صف.size() == max_اندازه)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << صف.back() << " from already full bounded queue<vector["
		       << value.size() << "]> [" << max_اندازه << "]." << پایان_سطر;
		  صف.pop_back();
	    }
	    صف.insert(صف.begin()+idx, value);
      }
}

فراغ vvp_صف_vec4::push_back(ثابت vvp_vector4_t&value, بیعلامت max_اندازه)
{
      إذا (!max_اندازه || (صف.size() < max_اندازه))
	    صف.push_back(value);
      وإلا
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<vector["
	         << value.size() << "]> [" << max_اندازه << "]." << پایان_سطر;
}

فراغ vvp_صف_vec4::push_front(ثابت vvp_vector4_t&value, بیعلامت max_اندازه)
{
      إذا (max_اندازه && (صف.size() == max_اندازه)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << صف.back() << " from already full bounded queue<vector["
	         << value.size() << "]> [" << max_اندازه << "]." << پایان_سطر;
	    صف.pop_back();
      }
      صف.push_front(value);
}

فراغ vvp_صف_vec4::erase(بیعلامت idx)
{
      أكد(صف.size() > idx);
      صف.erase(صف.begin()+idx);
}

فراغ vvp_صف_vec4::erase_دم(بیعلامت idx)
{
      أكد(صف.size() >= idx);
      إذا (صف.size() > idx)
	    صف.resize(idx);
}

فراغ vvp_صف_vec4::معکوس_elems(فراغ)
{
      std::reverse(صف.begin(), صف.انتها());
}

فراغ vvp_صف_vec4::مرتب_سازی_elems(منطقي ascending)
{
      إذا (ascending) {
	    std::sort(صف.begin(), صف.انتها(), vec4_lt_unsigned);
      } وإلا {
	    std::sort(صف.begin(), صف.انتها(),
		      [](ثابت vvp_vector4_t&a, ثابت vvp_vector4_t&b) {
			    أرجع vec4_lt_unsigned(b, a);
		      });
      }
}

فراغ vvp_صف_vec4::shuffle_elems(فراغ)
{
      std::shuffle(صف.begin(), صف.انتها(), shuffle_rng());
}
