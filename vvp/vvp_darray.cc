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

using નામાકાશ std;

નામાકાશ {

સ્થિર std::mt19937& shuffle_rng(શૂન્ય)
{
      સ્થિર std::mt19937 rng(std::random_device{}());
      પાછા rng;
}

સ્થિર તાર્કિક vec4_lt_unsigned(અચલ vvp_vector4_t&a, અચલ vvp_vector4_t&b)
{
      જો (a.eeq(b)) પાછા ખોટું;
      vvp_bit4_t gt = compare_gtge(b, a, BIT4_1);
      જો (gt == BIT4_X) પાછા ખોટું;
      પાછા gt == BIT4_1;
}

} // namespace

vvp_darray::~vvp_darray()
{
}

શૂન્ય vvp_darray::reverse_elems(શૂન્ય)
{
      cerr << "XXXX reverse_elems() not implemented for "
           << typeid(*આ).name() << endl;
}

શૂન્ય vvp_darray::sort_elems(તાર્કિક /*ascending*/)
{
      cerr << "XXXX sort_elems() not implemented for "
           << typeid(*આ).name() << endl;
}

શૂન્ય vvp_darray::shuffle_elems(શૂન્ય)
{
      cerr << "XXXX shuffle_elems() not implemented for "
           << typeid(*આ).name() << endl;
}

શૂન્ય vvp_darray::set_word(unsigned, અચલ vvp_vector4_t&)
{
      cerr << "XXXX set_word(vvp_vector4_t) not implemented for " << typeid(*આ).name() << endl;
}

શૂન્ય vvp_darray::set_word(unsigned, દ્વિગુણ)
{
      cerr << "XXXX set_word(double) not implemented for " << typeid(*આ).name() << endl;
}

શૂન્ય vvp_darray::set_word(unsigned, અચલ string&)
{
      cerr << "XXXX set_word(string) not implemented for " << typeid(*આ).name() << endl;
}

શૂન્ય vvp_darray::set_word(unsigned, અચલ vvp_object_t&)
{
      cerr << "XXXX set_word(vvp_object_t) not implemented for " << typeid(*આ).name() << endl;
}

શૂન્ય vvp_darray::get_word(unsigned, vvp_vector4_t&)
{
      cerr << "XXXX get_word(vvp_vector4_t) not implemented for " << typeid(*આ).name() << endl;
}

શૂન્ય vvp_darray::get_word(unsigned, દ્વિગુણ&)
{
      cerr << "XXXX get_word(double) not implemented for " << typeid(*આ).name() << endl;
}

શૂન્ય vvp_darray::get_word(unsigned, string&)
{
      cerr << "XXXX get_word(string) not implemented for " << typeid(*આ).name() << endl;
}

શૂન્ય vvp_darray::get_word(unsigned, vvp_object_t&)
{
      cerr << "XXXX get_word(vvp_object_t) not implemented for " << typeid(*આ).name() << endl;
}

vvp_vector4_t vvp_darray::get_bitstream(તાર્કિક)
{
      cerr << "XXXX get_bitstream() not implemented for " << typeid(*આ).name() << endl;
      પાછા vvp_vector4_t();
}

template <વર્ગ TYPE> vvp_darray_atom<TYPE>::~vvp_darray_atom()
{
}

template <વર્ગ TYPE> size_t vvp_darray_atom<TYPE>::get_size() અચલ
{
      પાછા array_.size();
}

template <વર્ગ TYPE> શૂન્ય vvp_darray_atom<TYPE>::reverse_elems(શૂન્ય)
{
      size_t n = array_.size();
      માટે (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

template <વર્ગ TYPE> શૂન્ય vvp_darray_atom<TYPE>::sort_elems(તાર્કિક ascending)
{
      જો (ascending) std::sort(array_.begin(), array_.end());
      નહીંતર std::sort(array_.begin(), array_.end(), std::greater<TYPE>());
}

template <વર્ગ TYPE> શૂન્ય vvp_darray_atom<TYPE>::shuffle_elems(શૂન્ય)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

template <વર્ગ TYPE> શૂન્ય vvp_darray_atom<TYPE>::set_word(unsigned adr, અચલ vvp_vector4_t&value)
{
      જો (adr >= array_.size())
	    પાછા;
      TYPE tmp;
      vector4_to_કિંમત(value, tmp, સાચું, ખોટું);
      array_[adr] = tmp;
}

template <વર્ગ TYPE> શૂન્ય vvp_darray_atom<TYPE>::get_word(unsigned adr, vvp_vector4_t&value)
{
      જો (adr >= array_.size()) {
	    value = vvp_vector4_t(8*કદ(TYPE), BIT4_X);
	    પાછા;
      }

      TYPE word = array_[adr];
      vvp_vector4_t tmp (8*કદ(TYPE), BIT4_0);
      માટે (unsigned idx = 0 ; idx < tmp.size() ; idx += 1) {
	    જો (word&1) tmp.set_bit(idx, BIT4_1);
	    word >>= 1;
      }
      value = tmp;
}

template <વર્ગ TYPE> શૂન્ય vvp_darray_atom<TYPE>::shallow_copy(અચલ vvp_object*obj)
{
      અચલ vvp_darray_atom<TYPE>*that = dynamic_cast<અચલ vvp_darray_atom<TYPE>*>(obj);
      પ્રતિજ્ઞા(that);

      unsigned અંક_વસ્તુઓ = min(array_.size(), that->array_.size());
      માટે (unsigned idx = 0 ; idx < અંક_વસ્તુઓ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

template <વર્ગ TYPE> vvp_object* vvp_darray_atom<TYPE>::duplicate(શૂન્ય) અચલ
{
      vvp_darray_atom<TYPE>*that = નવું vvp_darray_atom<TYPE>(array_.size());
      માટે (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      પાછા that;
}

template <વર્ગ TYPE> vvp_vector4_t vvp_darray_atom<TYPE>::get_bitstream(તાર્કિક)
{
      અચલ unsigned word_wid = કદ(TYPE) * 8;

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      જ્યારે (vdx > 0) {
            TYPE word = array_[adx++];
            vdx -= word_wid;
            માટે (unsigned bdx = 0; bdx < word_wid; bdx += 1) {
                  જો (word & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word >>= 1;
            }
      }

      પાછા vec;
}

template વર્ગ vvp_darray_atom<uint8_t>;
template વર્ગ vvp_darray_atom<uint16_t>;
template વર્ગ vvp_darray_atom<uint32_t>;
template વર્ગ vvp_darray_atom<uint64_t>;
template વર્ગ vvp_darray_atom<int8_t>;
template વર્ગ vvp_darray_atom<int16_t>;
template વર્ગ vvp_darray_atom<int32_t>;
template વર્ગ vvp_darray_atom<int64_t>;

vvp_darray_vec4::~vvp_darray_vec4()
{
}

size_t vvp_darray_vec4::get_size(શૂન્ય) અચલ
{
      પાછા array_.size();
}

શૂન્ય vvp_darray_vec4::set_word(unsigned adr, અચલ vvp_vector4_t&value)
{
      જો (adr >= array_.size()) પાછા;
      પ્રતિજ્ઞા(value.size() == word_wid_);
      array_[adr] = value;
}

શૂન્ય vvp_darray_vec4::get_word(unsigned adr, vvp_vector4_t&value)
{
	/*
	 * Return an undefined value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      જો ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_X);
	    પાછા;
      }
      value = array_[adr];
      પ્રતિજ્ઞા(value.size() == word_wid_);
}

શૂન્ય vvp_darray_vec4::shallow_copy(અચલ vvp_object*obj)
{
      અચલ vvp_darray_vec4*that = dynamic_cast<અચલ vvp_darray_vec4*>(obj);
      પ્રતિજ્ઞા(that);

      unsigned અંક_વસ્તુઓ = min(array_.size(), that->array_.size());
      માટે (unsigned idx = 0 ; idx < અંક_વસ્તુઓ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_object* vvp_darray_vec4::duplicate(શૂન્ય) અચલ
{
      vvp_darray_vec4*that = નવું vvp_darray_vec4(array_.size(), word_wid_);

      માટે (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      પાછા that;
}

vvp_vector4_t vvp_darray_vec4::get_bitstream(તાર્કિક as_vec4)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      જ્યારે (vdx > 0) {
            vdx -= word_wid_;
            માટે (unsigned bdx = 0; bdx < word_wid_; bdx += 1) {
                  vvp_bit4_t bit = array_[adx].value(bdx);
                  જો (as_vec4 || (bit == BIT4_1))
                        vec.set_bit(vdx+bdx, bit);
            }
            adx++;
      }

      પાછા vec;
}

શૂન્ય vvp_darray_vec4::reverse_elems(શૂન્ય)
{
      size_t n = array_.size();
      માટે (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

શૂન્ય vvp_darray_vec4::sort_elems(તાર્કિક ascending)
{
      જો (ascending) {
	    std::sort(array_.begin(), array_.end(), vec4_lt_unsigned);
      } નહીંતર {
	    std::sort(array_.begin(), array_.end(),
		      [](અચલ vvp_vector4_t&a, અચલ vvp_vector4_t&b) {
			    પાછા vec4_lt_unsigned(b, a);
		      });
      }
}

શૂન્ય vvp_darray_vec4::shuffle_elems(શૂન્ય)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

શૂન્ય vvp_darray_vec2::reverse_elems(શૂન્ય)
{
      size_t n = array_.size();
      માટે (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

શૂન્ય vvp_darray_vec2::sort_elems(તાર્કિક ascending)
{
      જો (ascending) {
	    std::sort(array_.begin(), array_.end());
      } નહીંતર {
	    std::sort(array_.begin(), array_.end(),
		      [](અચલ vvp_vector2_t&a, અચલ vvp_vector2_t&b) {
			    પાછા b < a;
		      });
      }
}

શૂન્ય vvp_darray_vec2::shuffle_elems(શૂન્ય)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_darray_vec2::~vvp_darray_vec2()
{
}

size_t vvp_darray_vec2::get_size(શૂન્ય) અચલ
{
      પાછા array_.size();
}

શૂન્ય vvp_darray_vec2::set_word(unsigned adr, અચલ vvp_vector4_t&value)
{
      જો (adr >= array_.size()) પાછા;
      પ્રતિજ્ઞા(value.size() == word_wid_);
      array_[adr] = value;
}

શૂન્ય vvp_darray_vec2::get_word(unsigned adr, vvp_vector4_t&value)
{
	/*
	 * Return a zero value for an out of range address or if the
	 * value has not been written yet (has a size of zero).
	 */
      જો ((adr >= array_.size()) || (array_[adr].size() == 0)) {
	    value = vvp_vector4_t(word_wid_, BIT4_0);
	    પાછા;
      }
      પ્રતિજ્ઞા(array_[adr].size() == word_wid_);
      value.resize(word_wid_);
      માટે (unsigned idx = 0; idx < word_wid_; idx += 1) {
	    value.set_bit(idx, array_[adr].value4(idx));
      }
}

શૂન્ય vvp_darray_vec2::shallow_copy(અચલ vvp_object*obj)
{
      અચલ vvp_darray_vec2*that = dynamic_cast<અચલ vvp_darray_vec2*>(obj);
      પ્રતિજ્ઞા(that);

      unsigned અંક_વસ્તુઓ = min(array_.size(), that->array_.size());
      માટે (unsigned idx = 0 ; idx < અંક_વસ્તુઓ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

vvp_vector4_t vvp_darray_vec2::get_bitstream(તાર્કિક)
{
      vvp_vector4_t vec(array_.size() * word_wid_, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      જ્યારે (vdx > 0) {
            vdx -= word_wid_;
            માટે (unsigned bdx = 0; bdx < word_wid_; bdx += 1) {
                  જો (array_[adx].value(bdx))
                        vec.set_bit(vdx+bdx, BIT4_1);
            }
            adx++;
      }

      પાછા vec;
}

vvp_darray_object::~vvp_darray_object()
{
}

size_t vvp_darray_object::get_size() અચલ
{
      પાછા array_.size();
}

શૂન્ય vvp_darray_object::set_word(unsigned adr, અચલ vvp_object_t&value)
{
      જો (adr >= array_.size())
	    પાછા;
      array_[adr] = value;
}

શૂન્ય vvp_darray_object::get_word(unsigned adr, vvp_object_t&value)
{
      જો (adr >= array_.size()) {
	    value = vvp_object_t();
	    પાછા;
      }

      value = array_[adr];
}

શૂન્ય vvp_darray_object::shallow_copy(અચલ vvp_object*obj)
{
      અચલ vvp_darray_object*that = dynamic_cast<અચલ vvp_darray_object*>(obj);
      પ્રતિજ્ઞા(that);

      unsigned અંક_વસ્તુઓ = min(array_.size(), that->array_.size());
      માટે (unsigned idx = 0 ; idx < અંક_વસ્તુઓ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

શૂન્ય vvp_darray_object::reverse_elems(શૂન્ય)
{
      size_t n = array_.size();
      માટે (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

શૂન્ય vvp_darray_object::sort_elems(તાર્કિક ascending)
{
      જો (ascending) {
	    std::sort(array_.begin(), array_.end(),
		      [](અચલ vvp_object_t&a, અચલ vvp_object_t&b) {
			    પાછા a.sort_order_ચાવી() < b.sort_order_ચાવી();
		      });
      } નહીંતર {
	    std::sort(array_.begin(), array_.end(),
		      [](અચલ vvp_object_t&a, અચલ vvp_object_t&b) {
			    પાછા a.sort_order_ચાવી() > b.sort_order_ચાવી();
		      });
      }
}

શૂન્ય vvp_darray_object::shuffle_elems(શૂન્ય)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_darray_real::~vvp_darray_real()
{
}

size_t vvp_darray_real::get_size() અચલ
{
      પાછા array_.size();
}

શૂન્ય vvp_darray_real::set_word(unsigned adr, દ્વિગુણ value)
{
      જો (adr >= array_.size())
	    પાછા;
      array_[adr] = value;
}

શૂન્ય vvp_darray_real::get_word(unsigned adr, દ્વિગુણ&value)
{
      જો (adr >= array_.size()) {
	    value = 0.0;
	    પાછા;
      }

      value = array_[adr];
}

શૂન્ય vvp_darray_real::shallow_copy(અચલ vvp_object*obj)
{
      અચલ vvp_darray_real*that = dynamic_cast<અચલ vvp_darray_real*>(obj);
      પ્રતિજ્ઞા(that);

      unsigned અંક_વસ્તુઓ = min(array_.size(), that->array_.size());
      માટે (unsigned idx = 0 ; idx < અંક_વસ્તુઓ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

શૂન્ય vvp_darray_real::reverse_elems(શૂન્ય)
{
      size_t n = array_.size();
      માટે (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

શૂન્ય vvp_darray_real::sort_elems(તાર્કિક ascending)
{
      જો (ascending) std::sort(array_.begin(), array_.end());
      નહીંતર std::sort(array_.begin(), array_.end(), std::greater<દ્વિગુણ>());
}

શૂન્ય vvp_darray_real::shuffle_elems(શૂન્ય)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_real::duplicate(શૂન્ય) અચલ
{
      vvp_darray_real*that = નવું vvp_darray_real(array_.size());

      માટે (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      પાછા that;
}

vvp_vector4_t vvp_darray_real::get_bitstream(તાર્કિક)
{
      અચલ unsigned word_wid = કદ(દ્વિગુણ) * 8;
      પ્રતિજ્ઞા(word_wid == 64);

      vvp_vector4_t vec(array_.size() * word_wid, BIT4_0);

      unsigned adx = 0;
      unsigned vdx = vec.size();
      જ્યારે (vdx > 0) {
            સંઘ {
                દ્વિગુણ   value;
                uint64_t bits;
            } word;
            word.value = array_[adx++];
            vdx -= word_wid;
            માટે (unsigned bdx = 0; bdx < word_wid; bdx += 1) {
                  જો (word.bits & 1)
                        vec.set_bit(vdx+bdx, BIT4_1);
                  word.bits >>= 1;
            }
      }

      પાછા vec;
}

vvp_darray_string::~vvp_darray_string()
{
}

size_t vvp_darray_string::get_size() અચલ
{
      પાછા array_.size();
}

શૂન્ય vvp_darray_string::set_word(unsigned adr, અચલ string&value)
{
      જો (adr >= array_.size())
	    પાછા;
      array_[adr] = value;
}

શૂન્ય vvp_darray_string::get_word(unsigned adr, string&value)
{
      જો (adr >= array_.size()) {
	    value = "";
	    પાછા;
      }

      value = array_[adr];
}

શૂન્ય vvp_darray_string::shallow_copy(અચલ vvp_object*obj)
{
      અચલ vvp_darray_string*that = dynamic_cast<અચલ vvp_darray_string*>(obj);
      પ્રતિજ્ઞા(that);

      unsigned અંક_વસ્તુઓ = min(array_.size(), that->array_.size());
      માટે (unsigned idx = 0 ; idx < અંક_વસ્તુઓ ; idx += 1)
	    array_[idx] = that->array_[idx];
}

શૂન્ય vvp_darray_string::reverse_elems(શૂન્ય)
{
      size_t n = array_.size();
      માટે (size_t idx = 0 ; idx < n/2 ; idx += 1) {
	    std::swap(array_[idx], array_[n-1-idx]);
      }
}

શૂન્ય vvp_darray_string::sort_elems(તાર્કિક ascending)
{
      જો (ascending) std::sort(array_.begin(), array_.end());
      નહીંતર std::sort(array_.begin(), array_.end(), std::greater<string>());
}

શૂન્ય vvp_darray_string::shuffle_elems(શૂન્ય)
{
      std::shuffle(array_.begin(), array_.end(), shuffle_rng());
}

vvp_object* vvp_darray_string::duplicate(શૂન્ય) અચલ
{
      vvp_darray_string*that = નવું vvp_darray_string(array_.size());

      માટે (size_t idx = 0 ; idx < array_.size() ; idx += 1)
	    that->array_[idx] = array_[idx];

      પાછા that;
}

vvp_queue::~vvp_queue()
{
}

શૂન્ય vvp_queue::copy_elems(vvp_object_t, unsigned)
{
      cerr << "Sorry: copy_elems() not implemented for " << typeid(*આ).name() << endl;
}

શૂન્ય vvp_queue::set_word_max(unsigned, અચલ vvp_vector4_t&, unsigned)
{
      cerr << "XXXX set_word_max(vvp_vector4_t) not implemented for " << typeid(*આ).name() << endl;
}

શૂન્ય vvp_queue::insert(unsigned, અચલ vvp_vector4_t&, unsigned)
{
      cerr << "XXXX insert(vvp_vector4_t) not implemented for " << typeid(*આ).name() << endl;
}

શૂન્ય vvp_queue::push_back(અચલ vvp_vector4_t&, unsigned)
{
      cerr << "XXXX push_back(vvp_vector4_t) not implemented for " << typeid(*આ).name() << endl;
}

શૂન્ય vvp_queue::push_front(અચલ vvp_vector4_t&, unsigned)
{
      cerr << "XXXX push_front(vvp_vector4_t) not implemented for " << typeid(*આ).name() << endl;
}

શૂન્ય vvp_queue::set_word_max(unsigned, દ્વિગુણ, unsigned)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*આ).name() << endl;
}

શૂન્ય vvp_queue::insert(unsigned, દ્વિગુણ, unsigned)
{
      cerr << "XXXX set_word_max(double) not implemented for " << typeid(*આ).name() << endl;
}

શૂન્ય vvp_queue::push_back(દ્વિગુણ, unsigned)
{
      cerr << "XXXX push_back(double) not implemented for " << typeid(*આ).name() << endl;
}

શૂન્ય vvp_queue::push_front(દ્વિગુણ, unsigned)
{
      cerr << "XXXX push_front(double) not implemented for " << typeid(*આ).name() << endl;
}

શૂન્ય vvp_queue::set_word_max(unsigned, અચલ string&, unsigned)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*આ).name() << endl;
}

શૂન્ય vvp_queue::insert(unsigned, અચલ string&, unsigned)
{
      cerr << "XXXX set_word_max(string) not implemented for " << typeid(*આ).name() << endl;
}

શૂન્ય vvp_queue::push_back(અચલ string&, unsigned)
{
      cerr << "XXXX push_back(string) not implemented for " << typeid(*આ).name() << endl;
}

શૂન્ય vvp_queue::push_front(અચલ string&, unsigned)
{
      cerr << "XXXX push_front(string) not implemented for " << typeid(*આ).name() << endl;
}

/*
 * Helper functions used while copying multiple elements into a queue.
 */
સ્થિર શૂન્ય print_copy_is_too_big(size_t src_size, unsigned max_size, અચલ string&qtype)
{
      cerr << get_fileline()
           << "Warning: queue<" << qtype << "> is bounded to have at most "
           << max_size << " elements, source has " << src_size << " elements." << endl;
}

સ્થિર શૂન્ય print_copy_is_too_big(દ્વિગુણ&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "real");
}

સ્થિર શૂન્ય print_copy_is_too_big(string&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "string");
}

સ્થિર શૂન્ય print_copy_is_too_big(vvp_vector4_t&, size_t src_size, unsigned max_size)
{
      print_copy_is_too_big(src_size, max_size, "vector");
}

template <typename ELEM, વર્ગ QTYPE, વર્ગ SRC_TYPE>
સ્થિર શૂન્ય copy_elements(QTYPE*queue, SRC_TYPE*src, unsigned max_size)
{
      size_t src_size = src->get_size();
      જો ((max_size != 0) && (src_size > max_size)) {
	    ELEM tmp;
	    print_copy_is_too_big(tmp, src_size, max_size);
      }
      unsigned copy_size = ((src_size < max_size) ||
                            (max_size == 0)) ? src_size : max_size;
      જો (copy_size < queue->get_size())
	    queue->erase_tail(copy_size);
      માટે (unsigned idx=0; idx < copy_size; ++idx) {
	    ELEM value;
	    src->get_word(idx, value);
	    queue->set_word_max(idx, value, max_size);
      }
}

શૂન્ય vvp_queue_real::copy_elems(vvp_object_t src, unsigned max_size)
{
      જો (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<દ્વિગુણ, vvp_queue_real, vvp_queue>(આ, src_queue, max_size);
      નહીંતર જો (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<દ્વિગુણ, vvp_queue_real, vvp_darray>(આ, src_darray, max_size);
      નહીંતર
	    cerr << get_fileline() << "Sorry: cannot copy object to real queue." << endl;
}

શૂન્ય vvp_queue_real::set_word_max(unsigned adr, દ્વિગુણ value, unsigned max_size)
{
      જો (adr == queue.size())
	    જો (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    નહીંતર
		  cerr << get_fileline()
		       << "Warning: assigning to queue<real>[" << adr << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      નહીંતર
	    set_word(adr, value);
}

શૂન્ય vvp_queue_real::set_word(unsigned adr, દ્વિગુણ value)
{
      જો (adr < queue.size())
	    queue[adr] = value;
      નહીંતર
	    cerr << get_fileline()
	         << "Warning: assigning to queue<real>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
}

શૂન્ય vvp_queue_real::get_word(unsigned adr, દ્વિગુણ&value)
{
      જો (adr >= queue.size())
	    value = 0.0;
      નહીંતર
	    value = queue[adr];
}

શૂન્ય vvp_queue_real::insert(unsigned idx, દ્વિગુણ value, unsigned max_size)
{
	// Inserting past the end of the queue
      જો (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<real>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
	// Inserting at the end
      નહીંતર જો (idx == queue.size())
	    જો (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    નહીંતર
		  cerr << get_fileline()
		       << "Warning: inserting to queue<real>[" << idx << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      નહીંતર  {
	    જો (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<real> ["
		       << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

શૂન્ય vvp_queue_real::push_back(દ્વિગુણ value, unsigned max_size)
{
      જો (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      નહીંતર
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<real> ["
	         << max_size << "]." << endl;
}

શૂન્ય vvp_queue_real::push_front(દ્વિગુણ value, unsigned max_size)
{
      જો (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<real> ["
	         << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

શૂન્ય vvp_queue_real::erase(unsigned idx)
{
      પ્રતિજ્ઞા(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

શૂન્ય vvp_queue_real::erase_tail(unsigned idx)
{
      પ્રતિજ્ઞા(queue.size() >= idx);
      જો (queue.size() > idx)
	    queue.resize(idx);
}

શૂન્ય vvp_queue_string::copy_elems(vvp_object_t src, unsigned max_size)
{
      જો (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<string, vvp_queue_string, vvp_queue>(આ, src_queue, max_size);
      નહીંતર જો (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<string, vvp_queue_string, vvp_darray>(આ, src_darray, max_size);
      નહીંતર
	    cerr << get_fileline() << "Sorry: cannot copy object to string queue." << endl;
}

શૂન્ય vvp_queue_string::set_word_max(unsigned adr, અચલ string&value, unsigned max_size)
{
      જો (adr == queue.size())
	    જો (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    નહીંતર
		  cerr << get_fileline()
		       << "Warning: assigning to queue<string>[" << adr << "] is"
		          " outside bound (" << max_size << "). \"" << value
		       << "\" was not added." << endl;
      નહીંતર
	    set_word(adr, value);
}

શૂન્ય vvp_queue_string::set_word(unsigned adr, અચલ string&value)
{
      જો (adr < queue.size())
	    queue[adr] = value;
      નહીંતર
	    cerr << get_fileline()
	         << "Warning: assigning to queue<string>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << endl;
}

શૂન્ય vvp_queue_string::get_word(unsigned adr, string&value)
{
      જો (adr >= queue.size())
	    value = "";
      નહીંતર
	    value = queue[adr];
}

શૂન્ય vvp_queue_string::insert(unsigned idx, અચલ string&value, unsigned max_size)
{
	// Inserting past the end of the queue
      જો (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<string>[" << idx << "] is "
	            "outside of size (" << queue.size() << "). \"" << value
	         << "\" was not added." << endl;
	// Inserting at the end
      નહીંતર જો (idx == queue.size())
	    જો (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    નહીંતર
		  cerr << get_fileline()
		       << "Warning: inserting to queue<string>[" << idx << "] is"
		          " outside bound (" << max_size << "). \"" << value
		       << "\" was not added." << endl;
      નહીંતર  {
	    જો (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", \"" << value << "\") removed \""
		       << queue.back() << "\" from already full bounded queue<string> ["
		       << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

શૂન્ય vvp_queue_string::push_back(અચલ string&value, unsigned max_size)
{
      જો (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      નહીંતર
	    cerr << get_fileline()
	         << "Warning: push_back(\"" << value
	         << "\") skipped for already full bounded queue<string> ["
	         << max_size << "]." << endl;
}

શૂન્ય vvp_queue_string::push_front(અચલ string&value, unsigned max_size)
{
      જો (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(\"" << value << "\") removed \""
	         << queue.back() << "\" from already full bounded queue<string> ["
	         << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

શૂન્ય vvp_queue_string::erase(unsigned idx)
{
      પ્રતિજ્ઞા(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

શૂન્ય vvp_queue_string::erase_tail(unsigned idx)
{
      પ્રતિજ્ઞા(queue.size() >= idx);
      જો (queue.size() > idx)
	    queue.resize(idx);
}

શૂન્ય vvp_queue_real::reverse_elems(શૂન્ય)
{
      std::reverse(queue.begin(), queue.end());
}

શૂન્ય vvp_queue_string::reverse_elems(શૂન્ય)
{
      std::reverse(queue.begin(), queue.end());
}

શૂન્ય vvp_queue_real::sort_elems(તાર્કિક ascending)
{
      જો (ascending) std::sort(queue.begin(), queue.end());
      નહીંતર std::sort(queue.begin(), queue.end(), std::greater<દ્વિગુણ>());
}

શૂન્ય vvp_queue_real::shuffle_elems(શૂન્ય)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

શૂન્ય vvp_queue_string::sort_elems(તાર્કિક ascending)
{
      જો (ascending) std::sort(queue.begin(), queue.end());
      નહીંતર std::sort(queue.begin(), queue.end(), std::greater<string>());
}

શૂન્ય vvp_queue_string::shuffle_elems(શૂન્ય)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}

શૂન્ય vvp_queue_vec4::copy_elems(vvp_object_t src, unsigned max_size)
{
      જો (vvp_queue*src_queue = src.peek<vvp_queue>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_queue>(આ, src_queue, max_size);
      નહીંતર જો (vvp_darray*src_darray = src.peek<vvp_darray>())
	    copy_elements<vvp_vector4_t, vvp_queue_vec4, vvp_darray>(આ, src_darray, max_size);
      નહીંતર
	    cerr << get_fileline() << "Sorry: cannot copy object to vector queue." << endl;
}

શૂન્ય vvp_queue_vec4::set_word_max(unsigned adr, અચલ vvp_vector4_t&value, unsigned max_size)
{
      જો (adr == queue.size())
	    જો (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    નહીંતર
		  cerr << get_fileline()
		       << "Warning: assigning to queue<vector>[" << adr << "] is"
		          " outside bound (" << max_size << "). " << value
		       << " was not added." << endl;
      નહીંતર
	    set_word(adr, value);
}

શૂન્ય vvp_queue_vec4::set_word(unsigned adr, અચલ vvp_vector4_t&value)
{
      જો (adr < queue.size())
	    queue[adr] = value;
      નહીંતર
	    cerr << get_fileline()
	         << "Warning: assigning to queue<vector>[" << adr << "] is outside "
	            "of size (" << queue.size() << "). " << value
	         << " was not added." << endl;
}

શૂન્ય vvp_queue_vec4::get_word(unsigned adr, vvp_vector4_t&value)
{
      જો (adr >= queue.size())
	    value = vvp_vector4_t(queue[0].size());
      નહીંતર
	    value = queue[adr];
}

શૂન્ય vvp_queue_vec4::insert(unsigned idx, અચલ vvp_vector4_t&value, unsigned max_size)
{
	// Inserting past the end of the queue
      જો (idx > queue.size())
	    cerr << get_fileline()
	         << "Warning: inserting to queue<vector[" << value.size()
	         << "]>[" << idx << "] is outside of size (" << queue.size()
	         << "). " << value << " was not added." << endl;
	// Inserting at the end
      નહીંતર જો (idx == queue.size())
	    જો (!max_size || (queue.size() < max_size))
		  queue.push_back(value);
	    નહીંતર
		  cerr << get_fileline()
		       << "Warning: inserting to queue<vector[" << value.size()
		       << "]>[" << idx << "] is outside bound (" << max_size
		       << "). " << value << " was not added." << endl;
      નહીંતર  {
	    જો (max_size && (queue.size() == max_size)) {
		  cerr << get_fileline()
		       << "Warning: insert("<< idx << ", " << value << ") removed "
		       << queue.back() << " from already full bounded queue<vector["
		       << value.size() << "]> [" << max_size << "]." << endl;
		  queue.pop_back();
	    }
	    queue.insert(queue.begin()+idx, value);
      }
}

શૂન્ય vvp_queue_vec4::push_back(અચલ vvp_vector4_t&value, unsigned max_size)
{
      જો (!max_size || (queue.size() < max_size))
	    queue.push_back(value);
      નહીંતર
	    cerr << get_fileline()
	         << "Warning: push_back(" << value
	         << ") skipped for already full bounded queue<vector["
	         << value.size() << "]> [" << max_size << "]." << endl;
}

શૂન્ય vvp_queue_vec4::push_front(અચલ vvp_vector4_t&value, unsigned max_size)
{
      જો (max_size && (queue.size() == max_size)) {
	    cerr << get_fileline()
	         << "Warning: push_front(" << value << ") removed "
	         << queue.back() << " from already full bounded queue<vector["
	         << value.size() << "]> [" << max_size << "]." << endl;
	    queue.pop_back();
      }
      queue.push_front(value);
}

શૂન્ય vvp_queue_vec4::erase(unsigned idx)
{
      પ્રતિજ્ઞા(queue.size() > idx);
      queue.erase(queue.begin()+idx);
}

શૂન્ય vvp_queue_vec4::erase_tail(unsigned idx)
{
      પ્રતિજ્ઞા(queue.size() >= idx);
      જો (queue.size() > idx)
	    queue.resize(idx);
}

શૂન્ય vvp_queue_vec4::reverse_elems(શૂન્ય)
{
      std::reverse(queue.begin(), queue.end());
}

શૂન્ય vvp_queue_vec4::sort_elems(તાર્કિક ascending)
{
      જો (ascending) {
	    std::sort(queue.begin(), queue.end(), vec4_lt_unsigned);
      } નહીંતર {
	    std::sort(queue.begin(), queue.end(),
		      [](અચલ vvp_vector4_t&a, અચલ vvp_vector4_t&b) {
			    પાછા vec4_lt_unsigned(b, a);
		      });
      }
}

શૂન્ય vvp_queue_vec4::shuffle_elems(શૂન્ય)
{
      std::shuffle(queue.begin(), queue.end(), shuffle_rng());
}
