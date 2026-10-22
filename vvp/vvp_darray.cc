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

کارول نطاق std;

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

فراغ vvp_darray::سرچپه_elems(فراغ)
{
      cerr << "XXXX reverse_elems() not implemented for "
           << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_darray::ترتیبول_elems(منطقي /*ascending*/)
{
      cerr << "XXXX sort_elems() not implemented for "
           << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_darray::shuffle_elems(فراغ)
{
      cerr << "XXXX shuffle_elems() not implemented for "
           << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_darray::سېټ_کلمه(بینښه, ثابت vvp_vector4_t&)
{
      cerr << "XXXX set_word(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_darray::سېټ_کلمه(بینښه, مزدوج)
{
      cerr << "XXXX set_word(double) not implemented for " << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_darray::سېټ_کلمه(بینښه, ثابت string&)
{
      cerr << "XXXX set_word(string) not implemented for " << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_darray::سېټ_کلمه(بینښه, ثابت vvp_object_t&)
{
      cerr << "XXXX set_word(vvp_object_t) not implemented for " << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_darray::get_کلمه(بینښه, vvp_vector4_t&)
{
      cerr << "XXXX get_word(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_darray::get_کلمه(بینښه, مزدوج&)
{
      cerr << "XXXX get_word(double) not implemented for " << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_darray::get_کلمه(بینښه, string&)
{
      cerr << "XXXX get_word(string) not implemented for " << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_darray::get_کلمه(بینښه, vvp_object_t&)
{
      cerr << "XXXX get_word(vvp_object_t) not implemented for " << typeid(*هذا).name() << کرښه_پای;
}

vvp_vector4_t vvp_darray::get_bitstream(منطقي)
{
      cerr << "XXXX get_bitstream() not implemented for " << typeid(*هذا).name() << کرښه_پای;
      أرجع vvp_vector4_t();
}

کينډۍ <صنف TYPE> vvp_darray_atom<TYPE>::~vvp_darray_atom()
{
}

کينډۍ <صنف TYPE> size_t vvp_darray_atom<TYPE>::get_اندازه() ثابت
{
      أرجع array_.size();
}

کينډۍ <صنف TYPE> فراغ vvp_darray_atom<TYPE>::سرچپه_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

کينډۍ <صنف TYPE> فراغ vvp_darray_atom<TYPE>::ترتیبول_elems(منطقي ascending)
{
      إذا (ascending) std::sort(array_.begin(), array_.پای_ټکی());
      وإلا std::sort(array_.begin(), array_.پای_ټکی(), std::greater<TYPE>());
}

کينډۍ <صنف TYPE> فراغ vvp_darray_atom<TYPE>::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.پای_ټکی(), shuffle_rng());
}

کينډۍ <صنف TYPE> فراغ vvp_darray_atom<TYPE>::سېټ_کلمه(بینښه adr, ثابت vvp_vector4_t&value)
{
      إذا (adr >= array_.size())
	    أرجع;
      TYPE tmp;
      vector4_منزل_ارزښت(value, tmp, ریښتیا, خطأ);
      array_[adr] = tmp;
}

کينډۍ <صنف TYPE> فراغ vvp_darray_atom<TYPE>::get_کلمه(بینښه adr, vvp_vector4_t&value)
{
      إذا (adr >= array_.size()) {
	    value = vvp_vector4_t(8*حجم(TYPE), BIT4_X);
	    أرجع;
      }

      TYPE کلمه = array_[adr];
      vvp_vector4_t tmp (8*حجم(TYPE), BIT4_0);
      لكل (بینښه idx = 0 ; idx < tmp.size() ; idx += 1) {
	    إذا (کلمه&1) tmp.سېټ_بټ(idx, BIT4_1);
	    کلمه >>= 1;
      }
      value = tmp;
}

کينډۍ <صنف TYPE> فراغ vvp_darray_atom<TYPE>::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_atom<TYPE>*that = dynamic_cast<ثابت vvp_darray_atom<TYPE>*>(obj);
      أكد(that);

      بینښه عدد_توکي = min(array_.size(), that->array_.size());
      لكل (بینښه idx = 0 ; idx < عدد_توکي ; idx += 1)
	    array_[idx] = that->array_[idx];
}

کينډۍ <صنف TYPE> vvp_object* vvp_darray_atom<TYPE>::duplicate(فراغ) ثابت
{
      vvp_darray_atom<TYPE>*that = جديد vvp_darray_atom<TYPE>(array_.size());
      لكل (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      أرجع that;
}

کينډۍ <صنف TYPE> vvp_vector4_t vvp_darray_atom<TYPE>::get_bitstream(منطقي)
{
      ثابت بینښه کلمه_wid = حجم(TYPE) * 8;

      vvp_vector4_t vec(array_.size() * کلمه_wid, BIT4_0);

      بینښه adx = 0;
      بینښه vdx = vec.size();
      طالما (vdx > 0) {
            TYPE کلمه = array_[adx++];
            vdx -= کلمه_wid;
            لكل (بینښه bdx = 0; bdx < کلمه_wid; bdx += 1) {
                  إذا (کلمه & 1)
                        vec.سېټ_بټ(vdx+bdx, BIT4_1);
                  کلمه >>= 1;
            }
      }

      أرجع vec;
}

کينډۍ صنف vvp_darray_atom<uint8_t>;
کينډۍ صنف vvp_darray_atom<uint16_t>;
کينډۍ صنف vvp_darray_atom<uint32_t>;
کينډۍ صنف vvp_darray_atom<uint64_t>;
کينډۍ صنف vvp_darray_atom<int8_t>;
کينډۍ صنف vvp_darray_atom<int16_t>;
کينډۍ صنف vvp_darray_atom<int32_t>;
کينډۍ صنف vvp_darray_atom<int64_t>;

vvp_darray_vec4::~vvp_darray_vec4()
{
}

size_t vvp_darray_vec4::get_اندازه(فراغ) ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_vec4::سېټ_کلمه(بینښه adr, ثابت vvp_vector4_t&value)
{
      إذا (adr >= array_.size()) أرجع;
      أكد(value.size() == کلمه_wid_2);
      array_[adr] = value;
}

فراغ vvp_darray_vec4::get_کلمه(بینښه adr, vvp_vector4_t&value)
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

      بینښه عدد_توکي = min(array_.size(), that->array_.size());
      لكل (بینښه idx = 0 ; idx < عدد_توکي ; idx += 1)
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

      بینښه adx = 0;
      بینښه vdx = vec.size();
      طالما (vdx > 0) {
            vdx -= کلمه_wid_2;
            لكل (بینښه bdx = 0; bdx < کلمه_wid_2; bdx += 1) {
                  vvp_bit4_t بټ = array_[adx].value(bdx);
                  إذا (as_vec4 || (بټ == BIT4_1))
                        vec.سېټ_بټ(vdx+bdx, بټ);
            }
            adx++;
      }

      أرجع vec;
}

فراغ vvp_darray_vec4::سرچپه_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_vec4::ترتیبول_elems(منطقي ascending)
{
      إذا (ascending) {
	    std::sort(array_.begin(), array_.پای_ټکی(), vec4_lt_unsigned);
      } وإلا {
	    std::sort(array_.begin(), array_.پای_ټکی(),
		      [](ثابت vvp_vector4_t&a, ثابت vvp_vector4_t&b) {
			    أرجع vec4_lt_unsigned(b, a);
		      });
      }
}

فراغ vvp_darray_vec4::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.پای_ټکی(), shuffle_rng());
}

فراغ vvp_darray_vec2::سرچپه_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_vec2::ترتیبول_elems(منطقي ascending)
{
      إذا (ascending) {
	    std::sort(array_.begin(), array_.پای_ټکی());
      } وإلا {
	    std::sort(array_.begin(), array_.پای_ټکی(),
		      [](ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b) {
			    أرجع b < a;
		      });
      }
}

فراغ vvp_darray_vec2::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.پای_ټکی(), shuffle_rng());
}

vvp_darray_vec2::~vvp_darray_vec2()
{
}

size_t vvp_darray_vec2::get_اندازه(فراغ) ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_vec2::سېټ_کلمه(بینښه adr, ثابت vvp_vector4_t&value)
{
      إذا (adr >= array_.size()) أرجع;
      أكد(value.size() == کلمه_wid_2);
      array_[adr] = value;
}

فراغ vvp_darray_vec2::get_کلمه(بینښه adr, vvp_vector4_t&value)
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
      لكل (بینښه idx = 0; idx < کلمه_wid_2; idx += 1) {
	    value.سېټ_بټ(idx, array_[adr].value4(idx));
      }
}

فراغ vvp_darray_vec2::shallow_copy(ثابت vvp_object*obj)
{
      ثابت vvp_darray_vec2*that = dynamic_cast<ثابت vvp_darray_vec2*>(obj);
      أكد(that);

      بینښه عدد_توکي = min(array_.size(), that->array_.size());
      لكل (بینښه idx = 0 ; idx < عدد_توکي ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_vector4_t vvp_darray_vec2::get_bitstream(منطقي)
{
      vvp_vector4_t vec(array_.size() * کلمه_wid_2, BIT4_0);

      بینښه adx = 0;
      بینښه vdx = vec.size();
      طالما (vdx > 0) {
            vdx -= کلمه_wid_2;
            لكل (بینښه bdx = 0; bdx < کلمه_wid_2; bdx += 1) {
                  إذا (array_[adx].value(bdx))
                        vec.سېټ_بټ(vdx+bdx, BIT4_1);
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

فراغ vvp_darray_object::سېټ_کلمه(بینښه adr, ثابت vvp_object_t&value)
{
      إذا (adr >= array_.size())
	    أرجع;
      array_[adr] = value;
}

فراغ vvp_darray_object::get_کلمه(بینښه adr, vvp_object_t&value)
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

      بینښه عدد_توکي = min(array_.size(), that->array_.size());
      لكل (بینښه idx = 0 ; idx < عدد_توکي ; idx += 1)
	    array_[idx] = that->array_[idx];
}

فراغ vvp_darray_object::سرچپه_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_object::ترتیبول_elems(منطقي ascending)
{
      إذا (ascending) {
	    std::sort(array_.begin(), array_.پای_ټکی(),
		      [](ثابت vvp_object_t&a, ثابت vvp_object_t&b) {
			    أرجع a.ترتیبول_ترتیب_کلی() < b.ترتیبول_ترتیب_کلی();
		      });
      } وإلا {
	    std::sort(array_.begin(), array_.پای_ټکی(),
		      [](ثابت vvp_object_t&a, ثابت vvp_object_t&b) {
			    أرجع a.ترتیبول_ترتیب_کلی() > b.ترتیبول_ترتیب_کلی();
		      });
      }
}

فراغ vvp_darray_object::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.پای_ټکی(), shuffle_rng());
}

vvp_darray_real::~vvp_darray_real()
{
}

size_t vvp_darray_real::get_اندازه() ثابت
{
      أرجع array_.size();
}

فراغ vvp_darray_real::سېټ_کلمه(بینښه adr, مزدوج value)
{
      إذا (adr >= array_.size())
	    أرجع;
      array_[adr] = value;
}

فراغ vvp_darray_real::get_کلمه(بینښه adr, مزدوج&value)
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

      بینښه عدد_توکي = min(array_.size(), that->array_.size());
      لكل (بینښه idx = 0 ; idx < عدد_توکي ; idx += 1)
	    array_[idx] = that->array_[idx];
}

فراغ vvp_darray_real::سرچپه_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_real::ترتیبول_elems(منطقي ascending)
{
      إذا (ascending) std::sort(array_.begin(), array_.پای_ټکی());
      وإلا std::sort(array_.begin(), array_.پای_ټکی(), std::greater<مزدوج>());
}

فراغ vvp_darray_real::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.پای_ټکی(), shuffle_rng());
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
      ثابت بینښه کلمه_wid = حجم(مزدوج) * 8;
      أكد(کلمه_wid == 64);

      vvp_vector4_t vec(array_.size() * کلمه_wid, BIT4_0);

      بینښه adx = 0;
      بینښه vdx = vec.size();
      طالما (vdx > 0) {
            اتحاد {
                مزدوج   value;
                uint64_t بټونه;
            } کلمه;
            کلمه.value = array_[adx++];
            vdx -= کلمه_wid;
            لكل (بینښه bdx = 0; bdx < کلمه_wid; bdx += 1) {
                  إذا (کلمه.بټونه & 1)
                        vec.سېټ_بټ(vdx+bdx, BIT4_1);
                  کلمه.بټونه >>= 1;
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

فراغ vvp_darray_string::سېټ_کلمه(بینښه adr, ثابت string&value)
{
      إذا (adr >= array_.size())
	    أرجع;
      array_[adr] = value;
}

فراغ vvp_darray_string::get_کلمه(بینښه adr, string&value)
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

      بینښه عدد_توکي = min(array_.size(), that->array_.size());
      لكل (بینښه idx = 0 ; idx < عدد_توکي ; idx += 1)
	    array_[idx] = that->array_[idx];
}

فراغ vvp_darray_string::سرچپه_elems(فراغ)
{
      size_t n = array_.size();
      لكل (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

فراغ vvp_darray_string::ترتیبول_elems(منطقي ascending)
{
      إذا (ascending) std::sort(array_.begin(), array_.پای_ټکی());
      وإلا std::sort(array_.begin(), array_.پای_ټکی(), std::greater<string>());
}

فراغ vvp_darray_string::shuffle_elems(فراغ)
{
      std::shuffle(array_.begin(), array_.پای_ټکی(), shuffle_rng());
}

vvp_object* vvp_darray_string::duplicate(فراغ) ثابت
{
      vvp_darray_string*that = جديد vvp_darray_string(array_.size());

      لكل (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      أرجع that;
}

vvp_کتار::~vvp_کتار()
{
}

فراغ vvp_کتار::copy_elems(vvp_object_t, بینښه)
{
      cerr << "Sorry: copy_elems() not implemented for " << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_کتار::سېټ_کلمه_max(بینښه, ثابت vvp_vector4_t&, بینښه)
{
      cerr << "XXXX set_word_max(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_کتار::insert(بینښه, ثابت vvp_vector4_t&, بینښه)
{
      cerr << "XXXX insert(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_کتار::push_back(ثابت vvp_vector4_t&, بینښه)
{
      cerr << "XXXX push_back(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_کتار::push_front(ثابت vvp_vector4_t&, بینښه)
{
      cerr << "XXXX push_front(vvp_vector4_t) not implemented for " << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_کتار::سېټ_کلمه_max(بینښه, مزدوج, بینښه)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_کتار::insert(بینښه, مزدوج, بینښه)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_کتار::push_back(مزدوج, بینښه)
{
      cerr << "XXXX push_back(double) not implemented for " << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_کتار::push_front(مزدوج, بینښه)
{
      cerr << "XXXX push_front(double) not implemented for " << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_کتار::سېټ_کلمه_max(بینښه, ثابت string&, بینښه)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_کتار::insert(بینښه, ثابت string&, بینښه)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_کتار::push_back(ثابت string&, بینښه)
{
      cerr << "XXXX push_back(string) not implemented for " << typeid(*هذا).name() << کرښه_پای;
}

فراغ vvp_کتار::push_front(ثابت string&, بینښه)
{
      cerr << "XXXX push_front(string) not implemented for " << typeid(*هذا).name() << کرښه_پای;
}

/*
 * Helper functions used while copying multiple elements into a queue.
 */
ساكن فراغ print_copy_is_too_big(size_t src_اندازه, بینښه max_اندازه, ثابت string&qtype)
{
      cerr << get_fileline()
           << "Warning: queue<" << qtype << "> is bounded to have at most "
           << max_اندازه << " elements, source has " << src_اندازه << " elements." << کرښه_پای;
}

ساكن فراغ print_copy_is_too_big(مزدوج&, size_t src_اندازه, بینښه max_اندازه)
{
      print_copy_is_too_big(src_اندازه, max_اندازه, "real");
}

ساكن فراغ print_copy_is_too_big(string&, size_t src_اندازه, بینښه max_اندازه)
{
      print_copy_is_too_big(src_اندازه, max_اندازه, "string");
}

ساكن فراغ print_copy_is_too_big(vvp_vector4_t&, size_t src_اندازه, بینښه max_اندازه)
{
      print_copy_is_too_big(src_اندازه, max_اندازه, "vector");
}

کينډۍ <ډول_نوم ELEM, صنف QTYPE, صنف SRC_TYPE>
ساكن فراغ copy_elements(QTYPE*کتار, SRC_TYPE*src, بینښه max_اندازه)
{
      size_t src_اندازه = src->get_اندازه();
      إذا ((max_اندازه != 0) && (src_اندازه > max_اندازه)) {
	    ELEM tmp;
	    print_copy_is_too_big(tmp, src_اندازه, max_اندازه);
      }
      بینښه copy_اندازه = ((src_اندازه < max_اندازه) ||
                            (max_اندازه == 0)) ? src_اندازه : max_اندازه;
      إذا (copy_اندازه < کتار->get_اندازه())
	    کتار->erase_لکۍ(copy_اندازه);
      لكل (بینښه idx=0; idx < copy_اندازه; ++idx) {
	    ELEM value;
	    src->get_کلمه(idx, value);
	    کتار->سېټ_کلمه_max(idx, value, max_اندازه);
      }
}

فراغ vvp_کتار_real::copy_elems(vvp_object_t src, بینښه max_اندازه)
{
      إذا (vvp_کتار*src_کتار = src.peek<vvp_کتار>())
	    copy_elements<مزدوج, vvp_کتار_real, vvp_کتار>(هذا, src_کتار, max_اندازه);
      وإلا إذا (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<مزدوج, vvp_کتار_real, vvp_darray>(هذا, src_darray, max_اندازه);
      وإلا
	    cerr << get_fileline() << "Sorry: cannot copy object to real queue." << کرښه_پای;
}

فراغ vvp_کتار_real::سېټ_کلمه_max(بینښه adr, مزدوج value, بینښه max_اندازه)
{
      إذا (adr == کتار.size())
	    إذا (!max_اندازه || (کتار.size() < max_اندازه))
		  کتار.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: assigning to queue<real>[" << adr << "] is"
		          " outside bound (" << max_اندازه << "). " << value
		       << " was not added." << کرښه_پای;
      وإلا
	    سېټ_کلمه(adr, value);
}

فراغ vvp_کتار_real::سېټ_کلمه(بینښه adr, مزدوج value)
{
      إذا (adr < کتار.size())
	    کتار[adr] = value;
      وإلا
	    cerr << get_fileline()
	         << "Warning: assigning to queue<real>[" << adr << "] is outside "
	            "of size (" << کتار.size() << "). " << value
	         << " was not added." << کرښه_پای;
}

فراغ vvp_کتار_real::get_کلمه(بینښه adr, مزدوج&value)
{
      إذا (adr >= کتار.size())
	    value = 0.0;
      وإلا
	    value = کتار[adr];
}

فراغ vvp_کتار_real::insert(بینښه idx, مزدوج value, بینښه max_اندازه)
{
	// Inserting past the end of the queue
      إذا (idx > کتار.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<real>[" << idx << "] is "
	            "outside of size (" << کتار.size() << "). " << value
	         << " was not added." << کرښه_پای;
	// Inserting at the end
      وإلا إذا (idx == کتار.size())
	    إذا (!max_اندازه || (کتار.size() < max_اندازه))
		  کتار.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: inserting to queue<real>[" << idx << "] is"
		          " outside bound (" << max_اندازه << "). " << value
		       << " was not added." << کرښه_پای;
      وإلا  {
	    إذا (max_اندازه && (کتار.size() == max_اندازه)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << کتار.back() << " from already full bounded queue<real> ["
		       << max_اندازه << "]." << کرښه_پای;
		  کتار.pop_back();
	    }
	    کتار.insert(کتار.begin()+idx, value);
      }
}

فراغ vvp_کتار_real::push_back(مزدوج value, بینښه max_اندازه)
{
      إذا (!max_اندازه || (کتار.size() < max_اندازه))
	    کتار.push_back(value);
      وإلا
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<real> ["
	         << max_اندازه << "]." << کرښه_پای;
}

فراغ vvp_کتار_real::push_front(مزدوج value, بینښه max_اندازه)
{
      إذا (max_اندازه && (کتار.size() == max_اندازه)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << کتار.back() << " from already full bounded queue<real> ["
	         << max_اندازه << "]." << کرښه_پای;
	    کتار.pop_back();
      }
      کتار.push_front(value);
}

فراغ vvp_کتار_real::erase(بینښه idx)
{
      أكد(کتار.size() > idx);
      کتار.erase(کتار.begin()+idx);
}

فراغ vvp_کتار_real::erase_لکۍ(بینښه idx)
{
      أكد(کتار.size() >= idx);
      إذا (کتار.size() > idx)
	    کتار.resize(idx);
}

فراغ vvp_کتار_string::copy_elems(vvp_object_t src, بینښه max_اندازه)
{
      إذا (vvp_کتار*src_کتار = src.peek<vvp_کتار>())
	    copy_elements<string, vvp_کتار_string, vvp_کتار>(هذا, src_کتار, max_اندازه);
      وإلا إذا (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<string, vvp_کتار_string, vvp_darray>(هذا, src_darray, max_اندازه);
      وإلا
	    cerr << get_fileline() << "Sorry: cannot copy object to string queue." << کرښه_پای;
}

فراغ vvp_کتار_string::سېټ_کلمه_max(بینښه adr, ثابت string&value, بینښه max_اندازه)
{
      إذا (adr == کتار.size())
	    إذا (!max_اندازه || (کتار.size() < max_اندازه))
		  کتار.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: assigning to queue<string>[" << adr << "] is"
		          " outside bound (" << max_اندازه << "). \"" << value
		       << "\" was not added." << کرښه_پای;
      وإلا
	    سېټ_کلمه(adr, value);
}

فراغ vvp_کتار_string::سېټ_کلمه(بینښه adr, ثابت string&value)
{
      إذا (adr < کتار.size())
	    کتار[adr] = value;
      وإلا
	    cerr << get_fileline()
	         << "Warning: assigning to queue<string>[" << adr << "] is outside "
	            "of size (" << کتار.size() << "). \"" << value
	         << "\" was not added." << کرښه_پای;
}

فراغ vvp_کتار_string::get_کلمه(بینښه adr, string&value)
{
      إذا (adr >= کتار.size())
	    value = "";
      وإلا
	    value = کتار[adr];
}

فراغ vvp_کتار_string::insert(بینښه idx, ثابت string&value, بینښه max_اندازه)
{
	// Inserting past the end of the queue
      إذا (idx > کتار.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<string>[" << idx << "] is "
	            "outside of size (" << کتار.size() << "). \"" << value
	         << "\" was not added." << کرښه_پای;
	// Inserting at the end
      وإلا إذا (idx == کتار.size())
	    إذا (!max_اندازه || (کتار.size() < max_اندازه))
		  کتار.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: inserting to queue<string>[" << idx << "] is"
		          " outside bound (" << max_اندازه << "). \"" << value
		       << "\" was not added." << کرښه_پای;
      وإلا  {
	    إذا (max_اندازه && (کتار.size() == max_اندازه)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", \"" << value << "\") removed \""
		       << کتار.back() << "\" from already full bounded queue<string> ["
		       << max_اندازه << "]." << کرښه_پای;
		  کتار.pop_back();
	    }
	    کتار.insert(کتار.begin()+idx, value);
      }
}

فراغ vvp_کتار_string::push_back(ثابت string&value, بینښه max_اندازه)
{
      إذا (!max_اندازه || (کتار.size() < max_اندازه))
	    کتار.push_back(value);
      وإلا
	    cerr << get_fileline()
	         << "Warning: push_back(\"" << value
	         << "\") skipped for already full bounded queue<string> ["
	         << max_اندازه << "]." << کرښه_پای;
}

فراغ vvp_کتار_string::push_front(ثابت string&value, بینښه max_اندازه)
{
      إذا (max_اندازه && (کتار.size() == max_اندازه)) {
	    cerr << get_fileline()
	         << "Warning: push_front(\"" << value << "\") removed \""
	         << کتار.back() << "\" from already full bounded queue<string> ["
	         << max_اندازه << "]." << کرښه_پای;
	    کتار.pop_back();
      }
      کتار.push_front(value);
}

فراغ vvp_کتار_string::erase(بینښه idx)
{
      أكد(کتار.size() > idx);
      کتار.erase(کتار.begin()+idx);
}

فراغ vvp_کتار_string::erase_لکۍ(بینښه idx)
{
      أكد(کتار.size() >= idx);
      إذا (کتار.size() > idx)
	    کتار.resize(idx);
}

فراغ vvp_کتار_real::سرچپه_elems(فراغ)
{
      std::reverse(کتار.begin(), کتار.پای_ټکی());
}

فراغ vvp_کتار_string::سرچپه_elems(فراغ)
{
      std::reverse(کتار.begin(), کتار.پای_ټکی());
}

فراغ vvp_کتار_real::ترتیبول_elems(منطقي ascending)
{
      إذا (ascending) std::sort(کتار.begin(), کتار.پای_ټکی());
      وإلا std::sort(کتار.begin(), کتار.پای_ټکی(), std::greater<مزدوج>());
}

فراغ vvp_کتار_real::shuffle_elems(فراغ)
{
      std::shuffle(کتار.begin(), کتار.پای_ټکی(), shuffle_rng());
}

فراغ vvp_کتار_string::ترتیبول_elems(منطقي ascending)
{
      إذا (ascending) std::sort(کتار.begin(), کتار.پای_ټکی());
      وإلا std::sort(کتار.begin(), کتار.پای_ټکی(), std::greater<string>());
}

فراغ vvp_کتار_string::shuffle_elems(فراغ)
{
      std::shuffle(کتار.begin(), کتار.پای_ټکی(), shuffle_rng());
}

فراغ vvp_کتار_vec4::copy_elems(vvp_object_t src, بینښه max_اندازه)
{
      إذا (vvp_کتار*src_کتار = src.peek<vvp_کتار>())
	    copy_elements<vvp_vector4_t, vvp_کتار_vec4, vvp_کتار>(هذا, src_کتار, max_اندازه);
      وإلا إذا (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<vvp_vector4_t, vvp_کتار_vec4, vvp_darray>(هذا, src_darray, max_اندازه);
      وإلا
	    cerr << get_fileline() << "Sorry: cannot copy object to vector queue." << کرښه_پای;
}

فراغ vvp_کتار_vec4::سېټ_کلمه_max(بینښه adr, ثابت vvp_vector4_t&value, بینښه max_اندازه)
{
      إذا (adr == کتار.size())
	    إذا (!max_اندازه || (کتار.size() < max_اندازه))
		  کتار.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: assigning to queue<vector>[" << adr << "] is"
		          " outside bound (" << max_اندازه << "). " << value
		       << " was not added." << کرښه_پای;
      وإلا
	    سېټ_کلمه(adr, value);
}

فراغ vvp_کتار_vec4::سېټ_کلمه(بینښه adr, ثابت vvp_vector4_t&value)
{
      إذا (adr < کتار.size())
	    کتار[adr] = value;
      وإلا
	    cerr << get_fileline()
	         << "Warning: assigning to queue<vector>[" << adr << "] is outside "
	            "of size (" << کتار.size() << "). " << value
	         << " was not added." << کرښه_پای;
}

فراغ vvp_کتار_vec4::get_کلمه(بینښه adr, vvp_vector4_t&value)
{
      إذا (adr >= کتار.size())
	    value = vvp_vector4_t(کتار[0].size());
      وإلا
	    value = کتار[adr];
}

فراغ vvp_کتار_vec4::insert(بینښه idx, ثابت vvp_vector4_t&value, بینښه max_اندازه)
{
	// Inserting past the end of the queue
      إذا (idx > کتار.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<vector[" << value.size()
	         << "]>[" << idx << "] is outside of size (" << کتار.size()
	         << "). " << value << " was not added." << کرښه_پای;
	// Inserting at the end
      وإلا إذا (idx == کتار.size())
	    إذا (!max_اندازه || (کتار.size() < max_اندازه))
		  کتار.push_back(value);
	    وإلا
		  cerr << get_fileline()
		       << "Warning: inserting to queue<vector[" << value.size()
		       << "]>[" << idx << "] is outside bound (" << max_اندازه
		       << "). " << value << " was not added." << کرښه_پای;
      وإلا  {
	    إذا (max_اندازه && (کتار.size() == max_اندازه)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << کتار.back() << " from already full bounded queue<vector["
		       << value.size() << "]> [" << max_اندازه << "]." << کرښه_پای;
		  کتار.pop_back();
	    }
	    کتار.insert(کتار.begin()+idx, value);
      }
}

فراغ vvp_کتار_vec4::push_back(ثابت vvp_vector4_t&value, بینښه max_اندازه)
{
      إذا (!max_اندازه || (کتار.size() < max_اندازه))
	    کتار.push_back(value);
      وإلا
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<vector["
	         << value.size() << "]> [" << max_اندازه << "]." << کرښه_پای;
}

فراغ vvp_کتار_vec4::push_front(ثابت vvp_vector4_t&value, بینښه max_اندازه)
{
      إذا (max_اندازه && (کتار.size() == max_اندازه)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << کتار.back() << " from already full bounded queue<vector["
	         << value.size() << "]> [" << max_اندازه << "]." << کرښه_پای;
	    کتار.pop_back();
      }
      کتار.push_front(value);
}

فراغ vvp_کتار_vec4::erase(بینښه idx)
{
      أكد(کتار.size() > idx);
      کتار.erase(کتار.begin()+idx);
}

فراغ vvp_کتار_vec4::erase_لکۍ(بینښه idx)
{
      أكد(کتار.size() >= idx);
      إذا (کتار.size() > idx)
	    کتار.resize(idx);
}

فراغ vvp_کتار_vec4::سرچپه_elems(فراغ)
{
      std::reverse(کتار.begin(), کتار.پای_ټکی());
}

فراغ vvp_کتار_vec4::ترتیبول_elems(منطقي ascending)
{
      إذا (ascending) {
	    std::sort(کتار.begin(), کتار.پای_ټکی(), vec4_lt_unsigned);
      } وإلا {
	    std::sort(کتار.begin(), کتار.پای_ټکی(),
		      [](ثابت vvp_vector4_t&a, ثابت vvp_vector4_t&b) {
			    أرجع vec4_lt_unsigned(b, a);
		      });
      }
}

فراغ vvp_کتار_vec4::shuffle_elems(فراغ)
{
      std::shuffle(کتار.begin(), کتار.پای_ټکی(), shuffle_rng());
}
