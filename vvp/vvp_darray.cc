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

ఉపయోగించి నామాకాశ std;

నామాకాశ {

స్థిర std::mt19937& shuffle_rng(శూన్య)
{
      స్థిర std::mt19937 rng(std::random_device{}());
      తిరిగి rng;
}

స్థిర తార్కిక vec4_lt_unsigned(స్థిరరాశి vvp_vector4_t&a, స్థిరరాశి vvp_vector4_t&b)
{
      ఒకవేళ (a.eeq(b)) తిరిగి అబద్ధం;
      vvp_bit4_t gt = compare_gtge(b, a, BIT4_1);
      ఒకవేళ (gt == BIT4_X) తిరిగి అబద్ధం;
      తిరిగి gt == BIT4_1;
}

} // namespace

vvp_darray::~vvp_darray()
{
}

శూన్య vvp_darray::తిరగేయి_elems(శూన్య)
{
      cerr << "XXXX reverse_elems() not implemented for "
           << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_darray::క్రమీకరించు_elems(తార్కిక /*ascending*/)
{
      cerr << "XXXX sort_elems() not implemented for "
           << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_darray::shuffle_elems(శూన్య)
{
      cerr << "XXXX shuffle_elems() not implemented for "
           << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_darray::సమితి_మాట(గుర్తులేని, స్థిరరాశి vvp_vector4_t&)
{
      cerr << "XXXX set_word(vvp_vector4_t) not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_darray::సమితి_మాట(గుర్తులేని, ద్విగుణ)
{
      cerr << "XXXX set_word(double) not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_darray::సమితి_మాట(గుర్తులేని, స్థిరరాశి string&)
{
      cerr << "XXXX set_word(string) not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_darray::సమితి_మాట(గుర్తులేని, స్థిరరాశి vvp_object_t&)
{
      cerr << "XXXX set_word(vvp_object_t) not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_darray::get_మాట(గుర్తులేని, vvp_vector4_t&)
{
      cerr << "XXXX get_word(vvp_vector4_t) not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_darray::get_మాట(గుర్తులేని, ద్విగుణ&)
{
      cerr << "XXXX get_word(double) not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_darray::get_మాట(గుర్తులేని, string&)
{
      cerr << "XXXX get_word(string) not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_darray::get_మాట(గుర్తులేని, vvp_object_t&)
{
      cerr << "XXXX get_word(vvp_object_t) not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

vvp_vector4_t vvp_darray::get_bitstream(తార్కిక)
{
      cerr << "XXXX get_bitstream() not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
      తిరిగి vvp_vector4_t();
}

మూస <తరగతి TYPE> vvp_darray_atom<TYPE>::~vvp_darray_atom()
{
}

మూస <తరగతి TYPE> size_t vvp_darray_atom<TYPE>::get_కొలత() స్థిరరాశి
{
      తిరిగి array_.size();
}

మూస <తరగతి TYPE> శూన్య vvp_darray_atom<TYPE>::తిరగేయి_elems(శూన్య)
{
      size_t n = array_.size();
      కొరకు (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

మూస <తరగతి TYPE> శూన్య vvp_darray_atom<TYPE>::క్రమీకరించు_elems(తార్కిక ascending)
{
      ఒకవేళ (ascending) std::sort(array_.begin(), array_.చివర());
      లేకపోతే std::sort(array_.begin(), array_.చివర(), std::greater<TYPE>());
}

మూస <తరగతి TYPE> శూన్య vvp_darray_atom<TYPE>::shuffle_elems(శూన్య)
{
      std::shuffle(array_.begin(), array_.చివర(), shuffle_rng());
}

మూస <తరగతి TYPE> శూన్య vvp_darray_atom<TYPE>::సమితి_మాట(గుర్తులేని adr, స్థిరరాశి vvp_vector4_t&value)
{
      ఒకవేళ (adr >= array_.size())
	    తిరిగి;
      TYPE tmp;
      vector4_గమ్యం_విలువ(value, tmp, నిజం, అబద్ధం);
      array_[adr] = tmp;
}

మూస <తరగతి TYPE> శూన్య vvp_darray_atom<TYPE>::get_మాట(గుర్తులేని adr, vvp_vector4_t&value)
{
      ఒకవేళ (adr >= array_.size()) {
	    value = vvp_vector4_t(8*పరిమాణం(TYPE), BIT4_X);
	    తిరిగి;
      }

      TYPE మాట = array_[adr];
      vvp_vector4_t tmp (8*పరిమాణం(TYPE), BIT4_0);
      కొరకు (గుర్తులేని idx = 0 ; idx < tmp.size() ; idx += 1) {
	    ఒకవేళ (మాట&1) tmp.సమితి_బిట్టు(idx, BIT4_1);
	    మాట >>= 1;
      }
      value = tmp;
}

మూస <తరగతి TYPE> శూన్య vvp_darray_atom<TYPE>::shallow_copy(స్థిరరాశి vvp_object*obj)
{
      స్థిరరాశి vvp_darray_atom<TYPE>*that = dynamic_cast<స్థిరరాశి vvp_darray_atom<TYPE>*>(obj);
      ప్రతిజ్ఞా(that);

      గుర్తులేని అంకె_వస్తువులు = min(array_.size(), that->array_.size());
      కొరకు (గుర్తులేని idx = 0 ; idx < అంకె_వస్తువులు ; idx += 1)
	    array_[idx] = that->array_[idx];
}

మూస <తరగతి TYPE> vvp_object* vvp_darray_atom<TYPE>::duplicate(శూన్య) స్థిరరాశి
{
      vvp_darray_atom<TYPE>*that = కొత్త vvp_darray_atom<TYPE>(array_.size());
      కొరకు (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      తిరిగి that;
}

మూస <తరగతి TYPE> vvp_vector4_t vvp_darray_atom<TYPE>::get_bitstream(తార్కిక)
{
      స్థిరరాశి గుర్తులేని మాట_wid = పరిమాణం(TYPE) * 8;

      vvp_vector4_t vec(array_.size() * మాట_wid, BIT4_0);

      గుర్తులేని adx = 0;
      గుర్తులేని vdx = vec.size();
      అయితే (vdx > 0) {
            TYPE మాట = array_[adx++];
            vdx -= మాట_wid;
            కొరకు (గుర్తులేని bdx = 0; bdx < మాట_wid; bdx += 1) {
                  ఒకవేళ (మాట & 1)
                        vec.సమితి_బిట్టు(vdx+bdx, BIT4_1);
                  మాట >>= 1;
            }
      }

      తిరిగి vec;
}

మూస తరగతి vvp_darray_atom<uint8_t>;
మూస తరగతి vvp_darray_atom<uint16_t>;
మూస తరగతి vvp_darray_atom<uint32_t>;
మూస తరగతి vvp_darray_atom<uint64_t>;
మూస తరగతి vvp_darray_atom<int8_t>;
మూస తరగతి vvp_darray_atom<int16_t>;
మూస తరగతి vvp_darray_atom<int32_t>;
మూస తరగతి vvp_darray_atom<int64_t>;

vvp_darray_vec4::~vvp_darray_vec4()
{
}

size_t vvp_darray_vec4::get_కొలత(శూన్య) స్థిరరాశి
{
      తిరిగి array_.size();
}

శూన్య vvp_darray_vec4::సమితి_మాట(గుర్తులేని adr, స్థిరరాశి vvp_vector4_t&value)
{
      ఒకవేళ (adr >= array_.size()) తిరిగి;
      ప్రతిజ్ఞా(value.size() == మాట_wid_2);
      array_[adr] = value;
}

శూన్య vvp_darray_vec4::get_మాట(గుర్తులేని adr, vvp_vector4_t&value)
{
	/*
	 * Return an undefined value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      ఒకవేళ ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(మాట_wid_2, BIT4_X);
	    తిరిగి;
      }
      value = array_[adr];
      ప్రతిజ్ఞా(value.size() == మాట_wid_2);
}

శూన్య vvp_darray_vec4::shallow_copy(స్థిరరాశి vvp_object*obj)
{
      స్థిరరాశి vvp_darray_vec4*that = dynamic_cast<స్థిరరాశి vvp_darray_vec4*>(obj);
      ప్రతిజ్ఞా(that);

      గుర్తులేని అంకె_వస్తువులు = min(array_.size(), that->array_.size());
      కొరకు (గుర్తులేని idx = 0 ; idx < అంకె_వస్తువులు ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_object* vvp_darray_vec4::duplicate(శూన్య) స్థిరరాశి
{
      vvp_darray_vec4*that = కొత్త vvp_darray_vec4(array_.size(), మాట_wid_2);

      కొరకు (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      తిరిగి that;
}

vvp_vector4_t vvp_darray_vec4::get_bitstream(తార్కిక as_vec4)
{
      vvp_vector4_t vec(array_.size() * మాట_wid_2, BIT4_0);

      గుర్తులేని adx = 0;
      గుర్తులేని vdx = vec.size();
      అయితే (vdx > 0) {
            vdx -= మాట_wid_2;
            కొరకు (గుర్తులేని bdx = 0; bdx < మాట_wid_2; bdx += 1) {
                  vvp_bit4_t బిట్టు = array_[adx].value(bdx);
                  ఒకవేళ (as_vec4 || (బిట్టు == BIT4_1))
                        vec.సమితి_బిట్టు(vdx+bdx, బిట్టు);
            }
            adx++;
      }

      తిరిగి vec;
}

శూన్య vvp_darray_vec4::తిరగేయి_elems(శూన్య)
{
      size_t n = array_.size();
      కొరకు (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

శూన్య vvp_darray_vec4::క్రమీకరించు_elems(తార్కిక ascending)
{
      ఒకవేళ (ascending) {
	    std::sort(array_.begin(), array_.చివర(), vec4_lt_unsigned);
      } లేకపోతే {
	    std::sort(array_.begin(), array_.చివర(),
		      [](స్థిరరాశి vvp_vector4_t&a, స్థిరరాశి vvp_vector4_t&b) {
			    తిరిగి vec4_lt_unsigned(b, a);
		      });
      }
}

శూన్య vvp_darray_vec4::shuffle_elems(శూన్య)
{
      std::shuffle(array_.begin(), array_.చివర(), shuffle_rng());
}

శూన్య vvp_darray_vec2::తిరగేయి_elems(శూన్య)
{
      size_t n = array_.size();
      కొరకు (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

శూన్య vvp_darray_vec2::క్రమీకరించు_elems(తార్కిక ascending)
{
      ఒకవేళ (ascending) {
	    std::sort(array_.begin(), array_.చివర());
      } లేకపోతే {
	    std::sort(array_.begin(), array_.చివర(),
		      [](స్థిరరాశి vvp_vector2_t&a, స్థిరరాశి vvp_vector2_t&b) {
			    తిరిగి b < a;
		      });
      }
}

శూన్య vvp_darray_vec2::shuffle_elems(శూన్య)
{
      std::shuffle(array_.begin(), array_.చివర(), shuffle_rng());
}

vvp_darray_vec2::~vvp_darray_vec2()
{
}

size_t vvp_darray_vec2::get_కొలత(శూన్య) స్థిరరాశి
{
      తిరిగి array_.size();
}

శూన్య vvp_darray_vec2::సమితి_మాట(గుర్తులేని adr, స్థిరరాశి vvp_vector4_t&value)
{
      ఒకవేళ (adr >= array_.size()) తిరిగి;
      ప్రతిజ్ఞా(value.size() == మాట_wid_2);
      array_[adr] = value;
}

శూన్య vvp_darray_vec2::get_మాట(గుర్తులేని adr, vvp_vector4_t&value)
{
	/*
	 * Return a zero value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      ఒకవేళ ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(మాట_wid_2, BIT4_0);
	    తిరిగి;
      }
      ప్రతిజ్ఞా(array_[adr].size() == మాట_wid_2);
      value.resize(మాట_wid_2);
      కొరకు (గుర్తులేని idx = 0; idx < మాట_wid_2; idx += 1) {
	    value.సమితి_బిట్టు(idx, array_[adr].value4(idx));
      }
}

శూన్య vvp_darray_vec2::shallow_copy(స్థిరరాశి vvp_object*obj)
{
      స్థిరరాశి vvp_darray_vec2*that = dynamic_cast<స్థిరరాశి vvp_darray_vec2*>(obj);
      ప్రతిజ్ఞా(that);

      గుర్తులేని అంకె_వస్తువులు = min(array_.size(), that->array_.size());
      కొరకు (గుర్తులేని idx = 0 ; idx < అంకె_వస్తువులు ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_vector4_t vvp_darray_vec2::get_bitstream(తార్కిక)
{
      vvp_vector4_t vec(array_.size() * మాట_wid_2, BIT4_0);

      గుర్తులేని adx = 0;
      గుర్తులేని vdx = vec.size();
      అయితే (vdx > 0) {
            vdx -= మాట_wid_2;
            కొరకు (గుర్తులేని bdx = 0; bdx < మాట_wid_2; bdx += 1) {
                  ఒకవేళ (array_[adx].value(bdx))
                        vec.సమితి_బిట్టు(vdx+bdx, BIT4_1);
            }
            adx++;
      }

      తిరిగి vec;
}

vvp_darray_object::~vvp_darray_object()
{
}

size_t vvp_darray_object::get_కొలత() స్థిరరాశి
{
      తిరిగి array_.size();
}

శూన్య vvp_darray_object::సమితి_మాట(గుర్తులేని adr, స్థిరరాశి vvp_object_t&value)
{
      ఒకవేళ (adr >= array_.size())
	    తిరిగి;
      array_[adr] = value;
}

శూన్య vvp_darray_object::get_మాట(గుర్తులేని adr, vvp_object_t&value)
{
      ఒకవేళ (adr >= array_.size()) {
	    value = vvp_object_t();
	    తిరిగి;
      }

      value = array_[adr];
}

శూన్య vvp_darray_object::shallow_copy(స్థిరరాశి vvp_object*obj)
{
      స్థిరరాశి vvp_darray_object*that = dynamic_cast<స్థిరరాశి vvp_darray_object*>(obj);
      ప్రతిజ్ఞా(that);

      గుర్తులేని అంకె_వస్తువులు = min(array_.size(), that->array_.size());
      కొరకు (గుర్తులేని idx = 0 ; idx < అంకె_వస్తువులు ; idx += 1)
	    array_[idx] = that->array_[idx];
}

శూన్య vvp_darray_object::తిరగేయి_elems(శూన్య)
{
      size_t n = array_.size();
      కొరకు (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

శూన్య vvp_darray_object::క్రమీకరించు_elems(తార్కిక ascending)
{
      ఒకవేళ (ascending) {
	    std::sort(array_.begin(), array_.చివర(),
		      [](స్థిరరాశి vvp_object_t&a, స్థిరరాశి vvp_object_t&b) {
			    తిరిగి a.క్రమీకరించు_క్రమం_కీలకం() < b.క్రమీకరించు_క్రమం_కీలకం();
		      });
      } లేకపోతే {
	    std::sort(array_.begin(), array_.చివర(),
		      [](స్థిరరాశి vvp_object_t&a, స్థిరరాశి vvp_object_t&b) {
			    తిరిగి a.క్రమీకరించు_క్రమం_కీలకం() > b.క్రమీకరించు_క్రమం_కీలకం();
		      });
      }
}

శూన్య vvp_darray_object::shuffle_elems(శూన్య)
{
      std::shuffle(array_.begin(), array_.చివర(), shuffle_rng());
}

vvp_darray_real::~vvp_darray_real()
{
}

size_t vvp_darray_real::get_కొలత() స్థిరరాశి
{
      తిరిగి array_.size();
}

శూన్య vvp_darray_real::సమితి_మాట(గుర్తులేని adr, ద్విగుణ value)
{
      ఒకవేళ (adr >= array_.size())
	    తిరిగి;
      array_[adr] = value;
}

శూన్య vvp_darray_real::get_మాట(గుర్తులేని adr, ద్విగుణ&value)
{
      ఒకవేళ (adr >= array_.size()) {
	    value = 0.0;
	    తిరిగి;
      }

      value = array_[adr];
}

శూన్య vvp_darray_real::shallow_copy(స్థిరరాశి vvp_object*obj)
{
      స్థిరరాశి vvp_darray_real*that = dynamic_cast<స్థిరరాశి vvp_darray_real*>(obj);
      ప్రతిజ్ఞా(that);

      గుర్తులేని అంకె_వస్తువులు = min(array_.size(), that->array_.size());
      కొరకు (గుర్తులేని idx = 0 ; idx < అంకె_వస్తువులు ; idx += 1)
	    array_[idx] = that->array_[idx];
}

శూన్య vvp_darray_real::తిరగేయి_elems(శూన్య)
{
      size_t n = array_.size();
      కొరకు (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

శూన్య vvp_darray_real::క్రమీకరించు_elems(తార్కిక ascending)
{
      ఒకవేళ (ascending) std::sort(array_.begin(), array_.చివర());
      లేకపోతే std::sort(array_.begin(), array_.చివర(), std::greater<ద్విగుణ>());
}

శూన్య vvp_darray_real::shuffle_elems(శూన్య)
{
      std::shuffle(array_.begin(), array_.చివర(), shuffle_rng());
}

vvp_object* vvp_darray_real::duplicate(శూన్య) స్థిరరాశి
{
      vvp_darray_real*that = కొత్త vvp_darray_real(array_.size());

      కొరకు (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      తిరిగి that;
}

vvp_vector4_t vvp_darray_real::get_bitstream(తార్కిక)
{
      స్థిరరాశి గుర్తులేని మాట_wid = పరిమాణం(ద్విగుణ) * 8;
      ప్రతిజ్ఞా(మాట_wid == 64);

      vvp_vector4_t vec(array_.size() * మాట_wid, BIT4_0);

      గుర్తులేని adx = 0;
      గుర్తులేని vdx = vec.size();
      అయితే (vdx > 0) {
            సంఘం {
                ద్విగుణ   value;
                uint64_t బిట్లు;
            } మాట;
            మాట.value = array_[adx++];
            vdx -= మాట_wid;
            కొరకు (గుర్తులేని bdx = 0; bdx < మాట_wid; bdx += 1) {
                  ఒకవేళ (మాట.బిట్లు & 1)
                        vec.సమితి_బిట్టు(vdx+bdx, BIT4_1);
                  మాట.బిట్లు >>= 1;
            }
      }

      తిరిగి vec;
}

vvp_darray_string::~vvp_darray_string()
{
}

size_t vvp_darray_string::get_కొలత() స్థిరరాశి
{
      తిరిగి array_.size();
}

శూన్య vvp_darray_string::సమితి_మాట(గుర్తులేని adr, స్థిరరాశి string&value)
{
      ఒకవేళ (adr >= array_.size())
	    తిరిగి;
      array_[adr] = value;
}

శూన్య vvp_darray_string::get_మాట(గుర్తులేని adr, string&value)
{
      ఒకవేళ (adr >= array_.size()) {
	    value = "";
	    తిరిగి;
      }

      value = array_[adr];
}

శూన్య vvp_darray_string::shallow_copy(స్థిరరాశి vvp_object*obj)
{
      స్థిరరాశి vvp_darray_string*that = dynamic_cast<స్థిరరాశి vvp_darray_string*>(obj);
      ప్రతిజ్ఞా(that);

      గుర్తులేని అంకె_వస్తువులు = min(array_.size(), that->array_.size());
      కొరకు (గుర్తులేని idx = 0 ; idx < అంకె_వస్తువులు ; idx += 1)
	    array_[idx] = that->array_[idx];
}

శూన్య vvp_darray_string::తిరగేయి_elems(శూన్య)
{
      size_t n = array_.size();
      కొరకు (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

శూన్య vvp_darray_string::క్రమీకరించు_elems(తార్కిక ascending)
{
      ఒకవేళ (ascending) std::sort(array_.begin(), array_.చివర());
      లేకపోతే std::sort(array_.begin(), array_.చివర(), std::greater<string>());
}

శూన్య vvp_darray_string::shuffle_elems(శూన్య)
{
      std::shuffle(array_.begin(), array_.చివర(), shuffle_rng());
}

vvp_object* vvp_darray_string::duplicate(శూన్య) స్థిరరాశి
{
      vvp_darray_string*that = కొత్త vvp_darray_string(array_.size());

      కొరకు (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      తిరిగి that;
}

vvp_వరుస::~vvp_వరుస()
{
}

శూన్య vvp_వరుస::copy_elems(vvp_object_t, గుర్తులేని)
{
      cerr << "Sorry: copy_elems() not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_వరుస::సమితి_మాట_max(గుర్తులేని, స్థిరరాశి vvp_vector4_t&, గుర్తులేని)
{
      cerr << "XXXX set_word_max(vvp_vector4_t) not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_వరుస::insert(గుర్తులేని, స్థిరరాశి vvp_vector4_t&, గుర్తులేని)
{
      cerr << "XXXX insert(vvp_vector4_t) not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_వరుస::push_back(స్థిరరాశి vvp_vector4_t&, గుర్తులేని)
{
      cerr << "XXXX push_back(vvp_vector4_t) not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_వరుస::push_front(స్థిరరాశి vvp_vector4_t&, గుర్తులేని)
{
      cerr << "XXXX push_front(vvp_vector4_t) not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_వరుస::సమితి_మాట_max(గుర్తులేని, ద్విగుణ, గుర్తులేని)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_వరుస::insert(గుర్తులేని, ద్విగుణ, గుర్తులేని)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_వరుస::push_back(ద్విగుణ, గుర్తులేని)
{
      cerr << "XXXX push_back(double) not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_వరుస::push_front(ద్విగుణ, గుర్తులేని)
{
      cerr << "XXXX push_front(double) not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_వరుస::సమితి_మాట_max(గుర్తులేని, స్థిరరాశి string&, గుర్తులేని)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_వరుస::insert(గుర్తులేని, స్థిరరాశి string&, గుర్తులేని)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_వరుస::push_back(స్థిరరాశి string&, గుర్తులేని)
{
      cerr << "XXXX push_back(string) not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

శూన్య vvp_వరుస::push_front(స్థిరరాశి string&, గుర్తులేని)
{
      cerr << "XXXX push_front(string) not implemented for " << typeid(*ఇది).name() << పంక్తి_ముగింపు;
}

/*
 * Helper functions used while copying multiple elements into a queue.
 */
స్థిర శూన్య print_copy_is_too_big(size_t src_కొలత, గుర్తులేని max_కొలత, స్థిరరాశి string&qtype)
{
      cerr << get_fileline()
           << "Warning: queue<" << qtype << "> is bounded to have at most "
           << max_కొలత << " elements, source has " << src_కొలత << " elements." << పంక్తి_ముగింపు;
}

స్థిర శూన్య print_copy_is_too_big(ద్విగుణ&, size_t src_కొలత, గుర్తులేని max_కొలత)
{
      print_copy_is_too_big(src_కొలత, max_కొలత, "real");
}

స్థిర శూన్య print_copy_is_too_big(string&, size_t src_కొలత, గుర్తులేని max_కొలత)
{
      print_copy_is_too_big(src_కొలత, max_కొలత, "string");
}

స్థిర శూన్య print_copy_is_too_big(vvp_vector4_t&, size_t src_కొలత, గుర్తులేని max_కొలత)
{
      print_copy_is_too_big(src_కొలత, max_కొలత, "vector");
}

మూస <రకం_పేరు ELEM, తరగతి QTYPE, తరగతి SRC_TYPE>
స్థిర శూన్య copy_elements(QTYPE*వరుస, SRC_TYPE*src, గుర్తులేని max_కొలత)
{
      size_t src_కొలత = src->get_కొలత();
      ఒకవేళ ((max_కొలత != 0) && (src_కొలత > max_కొలత)) {
	    ELEM tmp;
	    print_copy_is_too_big(tmp, src_కొలత, max_కొలత);
      }
      గుర్తులేని copy_కొలత = ((src_కొలత < max_కొలత) ||
                            (max_కొలత == 0)) ? src_కొలత : max_కొలత;
      ఒకవేళ (copy_కొలత < వరుస->get_కొలత())
	    వరుస->erase_తోక(copy_కొలత);
      కొరకు (గుర్తులేని idx=0; idx < copy_కొలత; ++idx) {
	    ELEM value;
	    src->get_మాట(idx, value);
	    వరుస->సమితి_మాట_max(idx, value, max_కొలత);
      }
}

శూన్య vvp_వరుస_real::copy_elems(vvp_object_t src, గుర్తులేని max_కొలత)
{
      ఒకవేళ (vvp_వరుస*src_వరుస = src.peek<vvp_వరుస>())
	    copy_elements<ద్విగుణ, vvp_వరుస_real, vvp_వరుస>(ఇది, src_వరుస, max_కొలత);
      లేకపోతే ఒకవేళ (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<ద్విగుణ, vvp_వరుస_real, vvp_darray>(ఇది, src_darray, max_కొలత);
      లేకపోతే
	    cerr << get_fileline() << "Sorry: cannot copy object to real queue." << పంక్తి_ముగింపు;
}

శూన్య vvp_వరుస_real::సమితి_మాట_max(గుర్తులేని adr, ద్విగుణ value, గుర్తులేని max_కొలత)
{
      ఒకవేళ (adr == వరుస.size())
	    ఒకవేళ (!max_కొలత || (వరుస.size() < max_కొలత))
		  వరుస.push_back(value);
	    లేకపోతే
		  cerr << get_fileline()
		       << "Warning: assigning to queue<real>[" << adr << "] is"
		          " outside bound (" << max_కొలత << "). " << value
		       << " was not added." << పంక్తి_ముగింపు;
      లేకపోతే
	    సమితి_మాట(adr, value);
}

శూన్య vvp_వరుస_real::సమితి_మాట(గుర్తులేని adr, ద్విగుణ value)
{
      ఒకవేళ (adr < వరుస.size())
	    వరుస[adr] = value;
      లేకపోతే
	    cerr << get_fileline()
	         << "Warning: assigning to queue<real>[" << adr << "] is outside "
	            "of size (" << వరుస.size() << "). " << value
	         << " was not added." << పంక్తి_ముగింపు;
}

శూన్య vvp_వరుస_real::get_మాట(గుర్తులేని adr, ద్విగుణ&value)
{
      ఒకవేళ (adr >= వరుస.size())
	    value = 0.0;
      లేకపోతే
	    value = వరుస[adr];
}

శూన్య vvp_వరుస_real::insert(గుర్తులేని idx, ద్విగుణ value, గుర్తులేని max_కొలత)
{
	// Inserting past the end of the queue
      ఒకవేళ (idx > వరుస.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<real>[" << idx << "] is "
	            "outside of size (" << వరుస.size() << "). " << value
	         << " was not added." << పంక్తి_ముగింపు;
	// Inserting at the end
      లేకపోతే ఒకవేళ (idx == వరుస.size())
	    ఒకవేళ (!max_కొలత || (వరుస.size() < max_కొలత))
		  వరుస.push_back(value);
	    లేకపోతే
		  cerr << get_fileline()
		       << "Warning: inserting to queue<real>[" << idx << "] is"
		          " outside bound (" << max_కొలత << "). " << value
		       << " was not added." << పంక్తి_ముగింపు;
      లేకపోతే  {
	    ఒకవేళ (max_కొలత && (వరుస.size() == max_కొలత)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << వరుస.back() << " from already full bounded queue<real> ["
		       << max_కొలత << "]." << పంక్తి_ముగింపు;
		  వరుస.pop_back();
	    }
	    వరుస.insert(వరుస.begin()+idx, value);
      }
}

శూన్య vvp_వరుస_real::push_back(ద్విగుణ value, గుర్తులేని max_కొలత)
{
      ఒకవేళ (!max_కొలత || (వరుస.size() < max_కొలత))
	    వరుస.push_back(value);
      లేకపోతే
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<real> ["
	         << max_కొలత << "]." << పంక్తి_ముగింపు;
}

శూన్య vvp_వరుస_real::push_front(ద్విగుణ value, గుర్తులేని max_కొలత)
{
      ఒకవేళ (max_కొలత && (వరుస.size() == max_కొలత)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << వరుస.back() << " from already full bounded queue<real> ["
	         << max_కొలత << "]." << పంక్తి_ముగింపు;
	    వరుస.pop_back();
      }
      వరుస.push_front(value);
}

శూన్య vvp_వరుస_real::erase(గుర్తులేని idx)
{
      ప్రతిజ్ఞా(వరుస.size() > idx);
      వరుస.erase(వరుస.begin()+idx);
}

శూన్య vvp_వరుస_real::erase_తోక(గుర్తులేని idx)
{
      ప్రతిజ్ఞా(వరుస.size() >= idx);
      ఒకవేళ (వరుస.size() > idx)
	    వరుస.resize(idx);
}

శూన్య vvp_వరుస_string::copy_elems(vvp_object_t src, గుర్తులేని max_కొలత)
{
      ఒకవేళ (vvp_వరుస*src_వరుస = src.peek<vvp_వరుస>())
	    copy_elements<string, vvp_వరుస_string, vvp_వరుస>(ఇది, src_వరుస, max_కొలత);
      లేకపోతే ఒకవేళ (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<string, vvp_వరుస_string, vvp_darray>(ఇది, src_darray, max_కొలత);
      లేకపోతే
	    cerr << get_fileline() << "Sorry: cannot copy object to string queue." << పంక్తి_ముగింపు;
}

శూన్య vvp_వరుస_string::సమితి_మాట_max(గుర్తులేని adr, స్థిరరాశి string&value, గుర్తులేని max_కొలత)
{
      ఒకవేళ (adr == వరుస.size())
	    ఒకవేళ (!max_కొలత || (వరుస.size() < max_కొలత))
		  వరుస.push_back(value);
	    లేకపోతే
		  cerr << get_fileline()
		       << "Warning: assigning to queue<string>[" << adr << "] is"
		          " outside bound (" << max_కొలత << "). \"" << value
		       << "\" was not added." << పంక్తి_ముగింపు;
      లేకపోతే
	    సమితి_మాట(adr, value);
}

శూన్య vvp_వరుస_string::సమితి_మాట(గుర్తులేని adr, స్థిరరాశి string&value)
{
      ఒకవేళ (adr < వరుస.size())
	    వరుస[adr] = value;
      లేకపోతే
	    cerr << get_fileline()
	         << "Warning: assigning to queue<string>[" << adr << "] is outside "
	            "of size (" << వరుస.size() << "). \"" << value
	         << "\" was not added." << పంక్తి_ముగింపు;
}

శూన్య vvp_వరుస_string::get_మాట(గుర్తులేని adr, string&value)
{
      ఒకవేళ (adr >= వరుస.size())
	    value = "";
      లేకపోతే
	    value = వరుస[adr];
}

శూన్య vvp_వరుస_string::insert(గుర్తులేని idx, స్థిరరాశి string&value, గుర్తులేని max_కొలత)
{
	// Inserting past the end of the queue
      ఒకవేళ (idx > వరుస.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<string>[" << idx << "] is "
	            "outside of size (" << వరుస.size() << "). \"" << value
	         << "\" was not added." << పంక్తి_ముగింపు;
	// Inserting at the end
      లేకపోతే ఒకవేళ (idx == వరుస.size())
	    ఒకవేళ (!max_కొలత || (వరుస.size() < max_కొలత))
		  వరుస.push_back(value);
	    లేకపోతే
		  cerr << get_fileline()
		       << "Warning: inserting to queue<string>[" << idx << "] is"
		          " outside bound (" << max_కొలత << "). \"" << value
		       << "\" was not added." << పంక్తి_ముగింపు;
      లేకపోతే  {
	    ఒకవేళ (max_కొలత && (వరుస.size() == max_కొలత)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", \"" << value << "\") removed \""
		       << వరుస.back() << "\" from already full bounded queue<string> ["
		       << max_కొలత << "]." << పంక్తి_ముగింపు;
		  వరుస.pop_back();
	    }
	    వరుస.insert(వరుస.begin()+idx, value);
      }
}

శూన్య vvp_వరుస_string::push_back(స్థిరరాశి string&value, గుర్తులేని max_కొలత)
{
      ఒకవేళ (!max_కొలత || (వరుస.size() < max_కొలత))
	    వరుస.push_back(value);
      లేకపోతే
	    cerr << get_fileline()
	         << "Warning: push_back(\"" << value
	         << "\") skipped for already full bounded queue<string> ["
	         << max_కొలత << "]." << పంక్తి_ముగింపు;
}

శూన్య vvp_వరుస_string::push_front(స్థిరరాశి string&value, గుర్తులేని max_కొలత)
{
      ఒకవేళ (max_కొలత && (వరుస.size() == max_కొలత)) {
	    cerr << get_fileline()
	         << "Warning: push_front(\"" << value << "\") removed \""
	         << వరుస.back() << "\" from already full bounded queue<string> ["
	         << max_కొలత << "]." << పంక్తి_ముగింపు;
	    వరుస.pop_back();
      }
      వరుస.push_front(value);
}

శూన్య vvp_వరుస_string::erase(గుర్తులేని idx)
{
      ప్రతిజ్ఞా(వరుస.size() > idx);
      వరుస.erase(వరుస.begin()+idx);
}

శూన్య vvp_వరుస_string::erase_తోక(గుర్తులేని idx)
{
      ప్రతిజ్ఞా(వరుస.size() >= idx);
      ఒకవేళ (వరుస.size() > idx)
	    వరుస.resize(idx);
}

శూన్య vvp_వరుస_real::తిరగేయి_elems(శూన్య)
{
      std::reverse(వరుస.begin(), వరుస.చివర());
}

శూన్య vvp_వరుస_string::తిరగేయి_elems(శూన్య)
{
      std::reverse(వరుస.begin(), వరుస.చివర());
}

శూన్య vvp_వరుస_real::క్రమీకరించు_elems(తార్కిక ascending)
{
      ఒకవేళ (ascending) std::sort(వరుస.begin(), వరుస.చివర());
      లేకపోతే std::sort(వరుస.begin(), వరుస.చివర(), std::greater<ద్విగుణ>());
}

శూన్య vvp_వరుస_real::shuffle_elems(శూన్య)
{
      std::shuffle(వరుస.begin(), వరుస.చివర(), shuffle_rng());
}

శూన్య vvp_వరుస_string::క్రమీకరించు_elems(తార్కిక ascending)
{
      ఒకవేళ (ascending) std::sort(వరుస.begin(), వరుస.చివర());
      లేకపోతే std::sort(వరుస.begin(), వరుస.చివర(), std::greater<string>());
}

శూన్య vvp_వరుస_string::shuffle_elems(శూన్య)
{
      std::shuffle(వరుస.begin(), వరుస.చివర(), shuffle_rng());
}

శూన్య vvp_వరుస_vec4::copy_elems(vvp_object_t src, గుర్తులేని max_కొలత)
{
      ఒకవేళ (vvp_వరుస*src_వరుస = src.peek<vvp_వరుస>())
	    copy_elements<vvp_vector4_t, vvp_వరుస_vec4, vvp_వరుస>(ఇది, src_వరుస, max_కొలత);
      లేకపోతే ఒకవేళ (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<vvp_vector4_t, vvp_వరుస_vec4, vvp_darray>(ఇది, src_darray, max_కొలత);
      లేకపోతే
	    cerr << get_fileline() << "Sorry: cannot copy object to vector queue." << పంక్తి_ముగింపు;
}

శూన్య vvp_వరుస_vec4::సమితి_మాట_max(గుర్తులేని adr, స్థిరరాశి vvp_vector4_t&value, గుర్తులేని max_కొలత)
{
      ఒకవేళ (adr == వరుస.size())
	    ఒకవేళ (!max_కొలత || (వరుస.size() < max_కొలత))
		  వరుస.push_back(value);
	    లేకపోతే
		  cerr << get_fileline()
		       << "Warning: assigning to queue<vector>[" << adr << "] is"
		          " outside bound (" << max_కొలత << "). " << value
		       << " was not added." << పంక్తి_ముగింపు;
      లేకపోతే
	    సమితి_మాట(adr, value);
}

శూన్య vvp_వరుస_vec4::సమితి_మాట(గుర్తులేని adr, స్థిరరాశి vvp_vector4_t&value)
{
      ఒకవేళ (adr < వరుస.size())
	    వరుస[adr] = value;
      లేకపోతే
	    cerr << get_fileline()
	         << "Warning: assigning to queue<vector>[" << adr << "] is outside "
	            "of size (" << వరుస.size() << "). " << value
	         << " was not added." << పంక్తి_ముగింపు;
}

శూన్య vvp_వరుస_vec4::get_మాట(గుర్తులేని adr, vvp_vector4_t&value)
{
      ఒకవేళ (adr >= వరుస.size())
	    value = vvp_vector4_t(వరుస[0].size());
      లేకపోతే
	    value = వరుస[adr];
}

శూన్య vvp_వరుస_vec4::insert(గుర్తులేని idx, స్థిరరాశి vvp_vector4_t&value, గుర్తులేని max_కొలత)
{
	// Inserting past the end of the queue
      ఒకవేళ (idx > వరుస.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<vector[" << value.size()
	         << "]>[" << idx << "] is outside of size (" << వరుస.size()
	         << "). " << value << " was not added." << పంక్తి_ముగింపు;
	// Inserting at the end
      లేకపోతే ఒకవేళ (idx == వరుస.size())
	    ఒకవేళ (!max_కొలత || (వరుస.size() < max_కొలత))
		  వరుస.push_back(value);
	    లేకపోతే
		  cerr << get_fileline()
		       << "Warning: inserting to queue<vector[" << value.size()
		       << "]>[" << idx << "] is outside bound (" << max_కొలత
		       << "). " << value << " was not added." << పంక్తి_ముగింపు;
      లేకపోతే  {
	    ఒకవేళ (max_కొలత && (వరుస.size() == max_కొలత)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << వరుస.back() << " from already full bounded queue<vector["
		       << value.size() << "]> [" << max_కొలత << "]." << పంక్తి_ముగింపు;
		  వరుస.pop_back();
	    }
	    వరుస.insert(వరుస.begin()+idx, value);
      }
}

శూన్య vvp_వరుస_vec4::push_back(స్థిరరాశి vvp_vector4_t&value, గుర్తులేని max_కొలత)
{
      ఒకవేళ (!max_కొలత || (వరుస.size() < max_కొలత))
	    వరుస.push_back(value);
      లేకపోతే
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<vector["
	         << value.size() << "]> [" << max_కొలత << "]." << పంక్తి_ముగింపు;
}

శూన్య vvp_వరుస_vec4::push_front(స్థిరరాశి vvp_vector4_t&value, గుర్తులేని max_కొలత)
{
      ఒకవేళ (max_కొలత && (వరుస.size() == max_కొలత)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << వరుస.back() << " from already full bounded queue<vector["
	         << value.size() << "]> [" << max_కొలత << "]." << పంక్తి_ముగింపు;
	    వరుస.pop_back();
      }
      వరుస.push_front(value);
}

శూన్య vvp_వరుస_vec4::erase(గుర్తులేని idx)
{
      ప్రతిజ్ఞా(వరుస.size() > idx);
      వరుస.erase(వరుస.begin()+idx);
}

శూన్య vvp_వరుస_vec4::erase_తోక(గుర్తులేని idx)
{
      ప్రతిజ్ఞా(వరుస.size() >= idx);
      ఒకవేళ (వరుస.size() > idx)
	    వరుస.resize(idx);
}

శూన్య vvp_వరుస_vec4::తిరగేయి_elems(శూన్య)
{
      std::reverse(వరుస.begin(), వరుస.చివర());
}

శూన్య vvp_వరుస_vec4::క్రమీకరించు_elems(తార్కిక ascending)
{
      ఒకవేళ (ascending) {
	    std::sort(వరుస.begin(), వరుస.చివర(), vec4_lt_unsigned);
      } లేకపోతే {
	    std::sort(వరుస.begin(), వరుస.చివర(),
		      [](స్థిరరాశి vvp_vector4_t&a, స్థిరరాశి vvp_vector4_t&b) {
			    తిరిగి vec4_lt_unsigned(b, a);
		      });
      }
}

శూన్య vvp_వరుస_vec4::shuffle_elems(శూన్య)
{
      std::shuffle(వరుస.begin(), వరుస.చివర(), shuffle_rng());
}
