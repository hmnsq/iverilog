#ifndef IVL_vvp_net_H
#define IVL_vvp_net_H
/*
 * Copyright (c) 2004-2026 Stephen Williams (steve@icarus.com)
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

# include  "config.h"
# include  "vpi_user.h"
# include  "vvp_vpi_callback.h"
# include  "permaheap.h"
# include  "vvp_object.h"
# include  <cstddef>
# include  <cstdlib>
# include  <cstring>
# include  <string>
# include  <new>
# include  <cassert>

#ifdef HAVE_IOSFWD
# include  <iosfwd>
#وإلا
# include  <iostream>
#endif

# include  "ivl_alloc.h"

/* Data types */
class  vvp_scalar_t;

class vvp_vector2_t;
class vvp_vector4_t;
class vvp_vector8_t;

/* Basic netlist types. */
class  vvp_net_t;
class  vvp_net_fun_t;
class  vvp_net_fil_t;

/* Core net function types. */
class  vvp_fun_drive;
class  vvp_fun_part;

class  vvp_delay_t;

/*
 * Storage for items declared in automatically allocated scopes (i.e. automatic
 * tasks and functions). The first two slots in each context are reserved for
 * linking to other contexts. The function that adds items to a context knows
 * this, and allocates context indices accordingly.
 */
تعريفنوع فراغ**vvp_context_t;

تعريفنوع فراغ*vvp_context_item_t;

مضمن vvp_context_t vvp_allocate_context(unsigned nitem)
{
      أرجع (vvp_context_t)malloc((2 + nitem) * حجم(فراغ*));
}

مضمن vvp_context_t vvp_get_next_context(vvp_context_t context)
{
      أرجع (vvp_context_t)context[0];
}

مضمن فراغ vvp_set_next_context(vvp_context_t context, vvp_context_t next)
{
      context[0] = next;
}

مضمن vvp_context_t vvp_get_stacked_context(vvp_context_t context)
{
      أرجع (vvp_context_t)context[1];
}

مضمن فراغ vvp_set_stacked_context(vvp_context_t context, vvp_context_t دۆۋە)
{
      context[1] = دۆۋە;
}

مضمن vvp_context_item_t vvp_get_context_item(vvp_context_t context,
                                               unsigned item_idx)
{
      أرجع (vvp_context_item_t)context[item_idx];
}

مضمن فراغ vvp_set_context_item(vvp_context_t context, unsigned item_idx,
                                 vvp_context_item_t item)
{
      context[item_idx] = item;
}

/*
 * An "automatic" functor is one which may be associated with an automatically
 * allocated scope item. This provides the infrastructure needed to allocate
 * the state information for individual instances of the item.
 */
بنية automatic_hooks_s {

      automatic_hooks_s() {}
      virtual ~automatic_hooks_s() {}

      virtual فراغ alloc_instance(vvp_context_t context) = 0;
      virtual فراغ reset_instance(vvp_context_t context) = 0;
#ifdef CHECK_WITH_VALGRIND
      virtual فراغ free_instance(vvp_context_t context) = 0;
#endif
};

/*
 * This is the set of Verilog 4-value bit values. Scalars have this
 * value along with strength, vectors are a collection of these
 * values. The enumeration has fixed numeric values that can be
 * expressed in 2 real bits, so that some of the internal classes can
 * pack them tightly.
 *
 * WARNING: Many things rely on this encoding for the BIT4_* enumeration
 * values, so accept that these values are cast in stone.
 */
تعداد vvp_bit4_t {
      BIT4_0 = 0,
      BIT4_1 = 1,
      BIT4_X = 3,
      BIT4_Z = 2
};

  /* Return an ASCII character that represents the vvp_bit4_t value. */
مضمن حرف vvp_bit4_to_ascii(vvp_bit4_t a) { أرجع "01zx"[a]; }

خارجي vvp_bit4_t add_with_carry(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c);

خارجي vvp_bit4_t scalar_to_bit4(PLI_INT32 scalar);

  /* Return TRUE if the bit is BIT4_X or BIT4_Z. The fast
     implementation here relies on the encoding of vvp_bit4_t values. */
مضمن bool bit4_is_xz(vvp_bit4_t a) { أرجع a >= 2; }

  /* This function converts BIT4_Z to BIT4_X, but passes other values
     unchanged. This fast implementation relies of the encoding of the
     vvp_bit4_t values. In particular, the BIT4_X==3 and BIT4_Z==2 */
مضمن vvp_bit4_t bit4_z2x(vvp_bit4_t a)
{ أرجع (vvp_bit4_t) ( (صحيح)a | ((صحيح)a >> 1) ); }

  /* Some common boolean operators. These implement the Verilog rules
     for 4-value bit operations. The fast implementations here rely
     on the encoding of vvp_bit4_t values. */

  // ~BIT4_0 --> BIT4_1
  // ~BIT4_1 --> BIT4_0
  // ~BIT4_X --> BIT4_X
  // ~BIT4_Z --> BIT4_X
مضمن vvp_bit4_t operator ~ (vvp_bit4_t a)
{ أرجع bit4_z2x((vvp_bit4_t) (((صحيح)a) ^ 1)); }

مضمن vvp_bit4_t operator | (vvp_bit4_t a, vvp_bit4_t b)
{
      إذا (a==BIT4_1 || b==BIT4_1)
	    أرجع BIT4_1;
      أرجع bit4_z2x( (vvp_bit4_t) ((صحيح)a | (صحيح)b) );
}

مضمن vvp_bit4_t operator & (vvp_bit4_t a, vvp_bit4_t b)
{
      إذا (a==BIT4_0 || b==BIT4_0)
	    أرجع BIT4_0;
      أرجع bit4_z2x( (vvp_bit4_t) ((صحيح)a | (صحيح)b) );
}


خارجي vvp_bit4_t operator ^ (vvp_bit4_t a, vvp_bit4_t b);
خارجي std::ostream& operator<< (std::ostream&o, vvp_bit4_t a);

  /* Return >0, ==0 or <0 if the from-to transition represents a
     posedge, no edge, or negedge. */
خارجي صحيح edge(vvp_bit4_t from, vvp_bit4_t to);

  /* Support for $countdrivers. */
مضمن فراغ update_driver_counts(vvp_bit4_t bit, unsigned counts[3])
{
      بدل (bit) {
          حالة BIT4_0:
            counts[0] += 1;
            اكسر;
          حالة BIT4_1:
            counts[1] += 1;
            اكسر;
          حالة BIT4_X:
            counts[2] += 1;
            اكسر;
          افتراضي:
            اكسر;
      }
}

/*
 * Some of the instructions do wide addition to arrays of long. They
 * use this add_with_carry function to help.
 */
ساكن مضمن unsigned long add_with_carry(unsigned long a, unsigned long b,
					   unsigned long&carry)
{
      unsigned long tmp = b + carry;
      unsigned long sum = a + tmp;
      carry = 0;
      إذا (tmp < b)
	    carry = 1;
      إذا (sum < tmp)
	    carry = 1;
      إذا (sum < a)
	    carry = 1;
      أرجع sum;
}

خارجي unsigned long multiply_with_carry(unsigned long a, unsigned long b,
					 unsigned long&carry);

/*
 * This class represents scalar values collected into vectors. The
 * vector values can be accessed individually, or treated as a
 * unit. in any case, the elements of the vector are addressed from
 * zero(LSB) to size-1(MSB).
 *
 * No strength values are stored here, if strengths are needed, use a
 * collection of vvp_scalar_t objects instead.
 */
class vvp_vector4_t {

      friend vvp_vector4_t operator ~(ثابت vvp_vector4_t&that);
      friend class vvp_vector4array_t;
      friend class vvp_vector4array_sa;
      friend class vvp_vector4array_aa;

    عام:
      ساكن ثابت vvp_vector4_t nil;

    عام:
      explicit vvp_vector4_t(unsigned size =0, vvp_bit4_t bits =BIT4_X);

      explicit vvp_vector4_t(unsigned size, مزدوج val);

	// Construct a vector4 from the subvalue of another
	// vector4. The width of the result is 'wid', and the bits are
	// pulled from 'that' to implement the Verilog part select
	// semantics. This means that part select beyond 'that'
	// returns X bits.
      explicit vvp_vector4_t(ثابت vvp_vector4_t&that,
			     unsigned adr, unsigned wid);

      vvp_vector4_t(ثابت vvp_vector4_t&that);
      vvp_vector4_t(ثابت vvp_vector4_t&that, bool invert_flag);
      vvp_vector4_t& operator= (ثابت vvp_vector4_t&that);

      ~vvp_vector4_t();

      مضمن unsigned size() ثابت { أرجع size_; }
      فراغ resize(unsigned new_size, vvp_bit4_t pad_bit = BIT4_X);

	// Get the bit at the specified address
      vvp_bit4_t قىممەت(unsigned idx) ثابت;
	// Get the vector4 subvector starting at the address
      vvp_vector4_t subvalue(unsigned idx, unsigned size) ثابت;
	// Get the 2-value bits for the subvector. This returns a new
	// array of longs, or a nil pointer if an XZ bit was detected
	// in the array.
      unsigned long*subarray(unsigned idx, unsigned size, bool xz_to_0 =false) ثابت;
      فراغ setarray(unsigned idx, unsigned size, ثابت unsigned long*val);

	// Set a 4-value bit or subvector into the vector. Return true
	// if any bits of the vector change as a result of this operation.
      فراغ set_bit(unsigned idx, vvp_bit4_t val);
      bool set_vec(unsigned idx, ثابت vvp_vector4_t&that);

        // Get the bits from another vector, but keep my size.
      فراغ copy_bits(ثابت vvp_vector4_t&that);

	// Move bits within this vector.
      فراغ mov(unsigned dst, unsigned src, unsigned cnt);

	// Add that to this in the Verilog way.
      فراغ add(ثابت vvp_vector4_t&that);

	// Subtract that from this in the Verilog way.
      فراغ sub(ثابت vvp_vector4_t&that);

	// Multiply this by that in the Verilog way.
      فراغ mul(ثابت vvp_vector4_t&that);

	// Test that the vectors are exactly equal
      bool eeq(ثابت vvp_vector4_t&that) ثابت;

	// Test that the vectors are equal, with xz comparing as equal.
      bool eq_xz(ثابت vvp_vector4_t&that) ثابت;

	// Return true if there is an X or Z anywhere in the vector.
      bool has_xz() ثابت;

	// Change all Z bits to X bits.
      فراغ change_z2x();

	// Set all bits to the specified value.
      فراغ fill_bits(vvp_bit4_t bit);

	// Change all bits to X bits.
      فراغ set_to_x() { fill_bits(BIT4_X); }

	// Display the value into the buf as a string.
      حرف*as_string(حرف*buf, size_t buf_len) ثابت;

      فراغ invert();
      vvp_vector4_t& operator &= (ثابت vvp_vector4_t&that);
      vvp_vector4_t& operator |= (ثابت vvp_vector4_t&that);
      vvp_vector4_t& operator ^= (ثابت vvp_vector4_t&that);
      vvp_vector4_t& operator += (int64_t);

      vvp_bit4_t reduce_or() ثابت;
      vvp_bit4_t reduce_and() ثابت;
      vvp_bit4_t reduce_xor() ثابت;

    خاص:
	// Number of vvp_bit4_t bits that can be shoved into a word.
      تعداد { BITS_PER_WORD = 8*حجم(unsigned long) };
	// The double value constructor requires that WORD_0_BBITS
	// and WORD_1_BBITS have the same value!
#إذا SIZEOF_UNSIGNED_LONG == 8
      تعداد { WORD_0_ABITS = 0x0000000000000000UL,
	     WORD_0_BBITS = 0x0000000000000000UL };
      تعداد { WORD_1_ABITS = 0xFFFFFFFFFFFFFFFFUL,
	     WORD_1_BBITS = 0x0000000000000000UL };
      تعداد { WORD_X_ABITS = 0xFFFFFFFFFFFFFFFFUL,
	     WORD_X_BBITS = 0xFFFFFFFFFFFFFFFFUL };
      تعداد { WORD_Z_ABITS = 0x0000000000000000UL,
             WORD_Z_BBITS = 0xFFFFFFFFFFFFFFFFUL };
#وإلاإذا SIZEOF_UNSIGNED_LONG == 4
      تعداد { WORD_0_ABITS = 0x00000000UL, WORD_0_BBITS = 0x00000000UL };
      تعداد { WORD_1_ABITS = 0xFFFFFFFFUL, WORD_1_BBITS = 0x00000000UL };
      تعداد { WORD_X_ABITS = 0xFFFFFFFFUL, WORD_X_BBITS = 0xFFFFFFFFUL };
      تعداد { WORD_Z_ABITS = 0x00000000UL, WORD_Z_BBITS = 0xFFFFFFFFUL };
#وإلا
#error "WORD_X_xBITS not defined for this architecture?"
#endif

	// Initialize and operator= use this private method to copy
	// the data from that object into this object.
      فراغ copy_from_(ثابت vvp_vector4_t&that);
      فراغ copy_from_big_(ثابت vvp_vector4_t&that);
      فراغ copy_inverted_from_(ثابت vvp_vector4_t&that);

      فراغ allocate_سۆزلەر(unsigned long inita, unsigned long initb);

	// Values in the vvp_vector4_t are stored split across two
	// arrays. For each bit in the vector, there is an abit and a
	// bbit. the encoding of a vvp_vector4_t is:
	//
	//         abit bbit
	//         ---- ----
	// BIT4_0    0    0   (Note that for BIT4_0 and BIT4_1, the bbit
	// BIT4_1    1    0    value is 0. This makes detecting XZ fast.)
	// BIT4_X    1    1
	// BIT4_Z    0    1

      unsigned size_;
      اتحاد {
	    unsigned long abits_val_;
	    unsigned long*abits_ptr_;
      };
      اتحاد {
	    unsigned long bbits_val_;
	    unsigned long*bbits_ptr_;
      };
};

مضمن vvp_vector4_t::vvp_vector4_t(ثابت vvp_vector4_t&that)
{
      copy_from_(that);
}

مضمن vvp_vector4_t::vvp_vector4_t(ثابت vvp_vector4_t&that, bool invert_flag)
{
      إذا (invert_flag)
	    copy_inverted_from_(that);
      وإلا
	    copy_from_(that);
}

مضمن vvp_vector4_t::vvp_vector4_t(unsigned size__, vvp_bit4_t val)
: size_(size__)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      ساكن ثابت unsigned long init_atable[4] = {
	    WORD_0_ABITS,
	    WORD_1_ABITS,
	    WORD_Z_ABITS,
	    WORD_X_ABITS };
      ساكن ثابت unsigned long init_btable[4] = {
	    WORD_0_BBITS,
	    WORD_1_BBITS,
	    WORD_Z_BBITS,
	    WORD_X_BBITS };

      allocate_سۆزلەر(init_atable[val], init_btable[val]);
}

مضمن vvp_vector4_t::~vvp_vector4_t()
{
      إذا (size_ > BITS_PER_WORD) {
	    delete[] abits_ptr_;
	      // bbits_ptr_ actually points half-way into a
	      // double-length array started at abits_ptr_
      }
}

مضمن vvp_vector4_t& vvp_vector4_t::operator= (ثابت vvp_vector4_t&that)
{
      إذا (this == &that)
	    أرجع *this;

      إذا (size_ > BITS_PER_WORD)
	    delete[] abits_ptr_;

      copy_from_(that);

      أرجع *this;
}

مضمن فراغ vvp_vector4_t::copy_from_(ثابت vvp_vector4_t&that)
{
      size_ = that.size_;
      إذا (size_ <= BITS_PER_WORD) {
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;
      } وإلا {
	    copy_from_big_(that);
      }
}

مضمن vvp_bit4_t vvp_vector4_t::قىممەت(unsigned idx) ثابت
{
      إذا (idx >= size_)
	    أرجع BIT4_X;

      unsigned off;

      unsigned long abits, bbits;
      إذا (size_ > BITS_PER_WORD) {
	    unsigned wdx = idx / BITS_PER_WORD;
	    off = idx % BITS_PER_WORD;
	    abits = abits_ptr_[wdx];
	    bbits = bbits_ptr_[wdx];
      } وإلا {
	    off = idx;
	    abits = abits_val_;
	    bbits = bbits_val_;
      }

      abits >>= off;
      bbits >>= off;
      صحيح tmp = ((bbits&1) << 1) + (abits&1);
	// This cast works since b==1,a==1 is X and b==1,a==0 is Z.
      أرجع (vvp_bit4_t)tmp;
}

مضمن vvp_vector4_t vvp_vector4_t::subvalue(unsigned adr, unsigned wid) ثابت
{
      أرجع vvp_vector4_t(*this, adr, wid);
}

مضمن فراغ vvp_vector4_t::set_bit(unsigned idx, vvp_bit4_t val)
{
      أكد(idx < size_);

      unsigned long off = idx % BITS_PER_WORD;
      unsigned long mask = 1UL << off;

      إذا (size_ > BITS_PER_WORD) {
	    unsigned wdx = idx / BITS_PER_WORD;
	    بدل (val) {
		حالة BIT4_0:
		  abits_ptr_[wdx] &= ~mask;
		  bbits_ptr_[wdx] &= ~mask;
		  اكسر;
		حالة BIT4_1:
		  abits_ptr_[wdx] |=  mask;
		  bbits_ptr_[wdx] &= ~mask;
		  اكسر;
		حالة BIT4_X:
		  abits_ptr_[wdx] |=  mask;
		  bbits_ptr_[wdx] |=  mask;
		  اكسر;
		حالة BIT4_Z:
		  abits_ptr_[wdx] &= ~mask;
		  bbits_ptr_[wdx] |=  mask;
		  اكسر;
	    }
      } وإلا {
	    بدل (val) {
		حالة BIT4_0:
		  abits_val_ &= ~mask;
		  bbits_val_ &= ~mask;
		  اكسر;
		حالة BIT4_1:
		  abits_val_ |=  mask;
		  bbits_val_ &= ~mask;
		  اكسر;
		حالة BIT4_X:
		  abits_val_ |=  mask;
		  bbits_val_ |=  mask;
		  اكسر;
		حالة BIT4_Z:
		  abits_val_ &= ~mask;
		  bbits_val_ |=  mask;
		  اكسر;
	    }
      }
}

مضمن vvp_vector4_t operator ~ (ثابت vvp_vector4_t&that)
{
      vvp_vector4_t res (that, true);
      أرجع res;
}

خارجي std::ostream& operator << (std::ostream&, ثابت vvp_vector4_t&);

خارجي vvp_bit4_t compare_gtge(ثابت vvp_vector4_t&a,
			       ثابت vvp_vector4_t&b,
			       vvp_bit4_t val_if_equal);
خارجي vvp_bit4_t compare_gtge_signed(ثابت vvp_vector4_t&a,
				      ثابت vvp_vector4_t&b,
				      vvp_bit4_t val_if_equal);
template <class T> خارجي T coerce_to_width(ثابت T&that, unsigned width);

/*
 * These functions extract the value of the vector as a native type,
 * if possible, and return true to indicate success. If the vector has
 * any X or Z bits, the resulting value will have 0 bits in their
 * place (this follows the rules of Verilog conversions from vector4
 * to real and integers) and the return value becomes false to
 * indicate an error.
 *
 * The "is_arithmetic" flag true will cause a result to be entirely 0
 * if any bits are X/Z. That is normally what you want if this value
 * is in the midst of an arithmetic expression. If is_arithmetic=false
 * then the X/Z bits will be replaced with 0 bits, and the return
 * value will be "false", but the other bits will be transferred. This
 * is what you want if you are doing "vpi_get_value", for example.
 */
template <class T> خارجي bool vector4_to_قىممەت(ثابت vvp_vector4_t&a, T&val,
						bool is_signed,
						bool is_arithmetic =true);

template <class T> خارجي bool vector4_to_قىممەت(ثابت vvp_vector4_t&a,
                                                bool&overflow_flag, T&val);

template <class T> مضمن bool vector4_to_قىممەت(ثابت vvp_vector4_t&a, T&val)
{
      bool overflow_flag;
      أرجع vector4_to_قىممەت(a, overflow_flag, val);
}

خارجي bool vector4_to_قىممەت(ثابت vvp_vector4_t&a, مزدوج&val, bool is_signed);

خارجي bool vector2_to_قىممەت(ثابت vvp_vector2_t&a, int32_t&val, bool is_signed);

خارجي vvp_vector4_t vector4_from_text(ثابت حرف*bits, unsigned wid);

مضمن vvp_vector4_t double_to_vector4_LSB(مزدوج real)
{
      // Convert the double to a bit vector and then use the LSB.
      // You can use very large real values and break the 64-bit
      // size, but that's really horrible code so don't do that!
      vvp_vector4_t bit = vvp_vector4_t(64, real);
      bit.resize(1);
      أرجع bit;
}

/*
 * vvp_vector4array_t
 */
class vvp_vector4array_t {

    عام:
      vvp_vector4array_t(unsigned width, unsigned سۆزلەر);
      virtual ~vvp_vector4array_t();

      unsigned width() ثابت { أرجع width_; }
      unsigned سۆزلەر() ثابت { أرجع سۆزلەر_3; }

      virtual vvp_vector4_t get_word(unsigned idx) ثابت = 0;
      virtual فراغ set_word(unsigned idx, ثابت vvp_vector4_t&that) = 0;

    محمي:
      بنية v4cell {
	    اتحاد {
		  unsigned long abits_val_;
		  unsigned long*abits_ptr_;
	    };
	    اتحاد {
		  unsigned long bbits_val_;
		  unsigned long*bbits_ptr_;
	    };
      };

      vvp_vector4_t get_word_(v4cell*cell) ثابت;
      فراغ set_word_(v4cell*cell, ثابت vvp_vector4_t&that);

      unsigned width_;
      unsigned سۆزلەر_3;

    خاص: // Not implemented
      vvp_vector4array_t(ثابت vvp_vector4array_t&);
      vvp_vector4array_t& operator = (ثابت vvp_vector4array_t&);
};

/*
 * Statically allocated vvp_vector4array_t
 */
class vvp_vector4array_sa : عام vvp_vector4array_t {

    عام:
      vvp_vector4array_sa(unsigned width, unsigned سۆزلەر);
      ~vvp_vector4array_sa() override;

      vvp_vector4_t get_word(unsigned idx) ثابت override;
      فراغ set_word(unsigned idx, ثابت vvp_vector4_t&that) override;

    خاص:
      v4cell* array_;
};

/*
 * Automatically allocated vvp_vector4array_t
 */
class vvp_vector4array_aa : عام vvp_vector4array_t, عام automatic_hooks_s {

    عام:
      vvp_vector4array_aa(unsigned width, unsigned سۆزلەر);
      ~vvp_vector4array_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#endif

      vvp_vector4_t get_word(unsigned idx) ثابت override;
      فراغ set_word(unsigned idx, ثابت vvp_vector4_t&that) override;

    خاص:
      unsigned context_idx_;
};

/* vvp_vector2_t
 */
class vvp_vector2_t {

      friend vvp_vector2_t operator - (ثابت vvp_vector2_t&);
      friend vvp_vector2_t operator + (ثابت vvp_vector2_t&,
				       ثابت vvp_vector2_t&);
      friend vvp_vector2_t operator * (ثابت vvp_vector2_t&,
				       ثابت vvp_vector2_t&);
      friend bool operator >  (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
      friend bool operator >= (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
      friend bool operator <  (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
      friend bool operator <= (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
      friend bool operator == (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);

    عام:
      vvp_vector2_t();
      vvp_vector2_t(ثابت vvp_vector2_t&);
      vvp_vector2_t(ثابت vvp_vector2_t&, unsigned newsize);
      vvp_vector2_t(ثابت vvp_vector2_t&, unsigned base, unsigned wid);
	// Make a vvp_vector2_t from a vvp_vector4_t. If enable_NaN
	// is true and there are X or Z bits, then the result becomes
	// a NaN value. If enable_NaN is false then X or Z bits are
	// converted to 0 as per the standard Verilog rules.
      explicit vvp_vector2_t(ثابت vvp_vector4_t&that, bool allow_NaN = false);
	// Make from a 64-bit value and a specified width.
      vvp_vector2_t(uint64_t val, unsigned wid);
	// Make with the width, and filled with 1 or 0 bits.
      تعداد fill_t {FILL0, FILL1};
      vvp_vector2_t(fill_t fill, unsigned wid);
      ~vvp_vector2_t();

      vvp_vector2_t&operator += (ثابت vvp_vector2_t&that);
      vvp_vector2_t&operator -= (ثابت vvp_vector2_t&that);
      vvp_vector2_t&operator <<= (unsigned shift);
      vvp_vector2_t&operator >>= (unsigned shift);
      vvp_vector2_t&operator = (ثابت vvp_vector2_t&);

	// Assign a vec4 to a vec2, using Verilog rules for converting
	// XZ values to 0.
      vvp_vector2_t&operator = (ثابت vvp_vector4_t&);

      bool is_NaN() ثابت;
      bool is_zero() ثابت;
      unsigned size() ثابت;
      صحيح قىممەت(unsigned idx) ثابت;
      vvp_bit4_t value4(unsigned idx) ثابت;
	// Get the vector2 subvector starting at the address
      vvp_vector2_t subvalue(unsigned idx, unsigned size) ثابت;
      فراغ set_bit(unsigned idx, صحيح bit);
      فراغ set_vec(unsigned idx, ثابت vvp_vector2_t&that);
	// Make the size just big enough to hold the first 1 bit.
      فراغ trim();
	// Trim off extra 1 bit since this is representing a negative value.
	// Always keep at least 32 bits.
      فراغ trim_neg();

    خاص:
      تعداد { BITS_PER_WORD = 8 * حجم(unsigned long) };
      unsigned long*vec_;
      unsigned wid_;

    خاص:
      فراغ copy_from_that_(ثابت vvp_vector2_t&that);
      فراغ copy_from_that_(ثابت vvp_vector4_t&that);
};

خارجي bool operator >  (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
خارجي bool operator >= (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
خارجي bool operator <  (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
خارجي bool operator <= (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
خارجي bool operator == (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
خارجي vvp_vector2_t operator - (ثابت vvp_vector2_t&);
خارجي vvp_vector2_t operator + (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
خارجي vvp_vector2_t operator * (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
خارجي vvp_vector2_t operator / (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
خارجي vvp_vector2_t operator % (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);

vvp_vector2_t pow(ثابت vvp_vector2_t&, vvp_vector2_t&);
خارجي vvp_vector4_t vector2_to_vector4(ثابت vvp_vector2_t&, unsigned wid);

/* A c4string is of the form C4<...> where ... are bits. */
خارجي bool c4string_test(ثابت حرف*str);
خارجي vvp_vector4_t c4string_to_vector4(ثابت حرف*str);

/* A crstring is of the form Cr<...> where ... defines are real. */
خارجي bool crstring_test(ثابت حرف*str);
خارجي مزدوج crstring_to_double(ثابت حرف*str);

خارجي std::ostream& operator<< (std::ostream&, ثابت vvp_vector2_t&);

مضمن vvp_vector2_t::vvp_vector2_t(ثابت vvp_vector2_t&that)
{
      copy_from_that_(that);
}

مضمن vvp_vector2_t::vvp_vector2_t(ثابت vvp_vector4_t&that, bool enable_NaN)
{
      إذا (enable_NaN && that.has_xz()) {
	    vec_ = 0;
	    wid_ = 0;
	    أرجع;
      }
      copy_from_that_(that);
}

مضمن vvp_vector2_t::~vvp_vector2_t()
{
      delete[] vec_;
}

/* Inline some of the vector2_t methods. */
مضمن unsigned vvp_vector2_t::size() ثابت
{
      أرجع wid_;
}

مضمن vvp_bit4_t vvp_vector2_t::value4(unsigned idx) ثابت
{
      إذا (قىممەت(idx))
	    أرجع BIT4_1;
      وإلا
	    أرجع BIT4_0;
}

مضمن vvp_vector2_t vvp_vector2_t::subvalue(unsigned base, unsigned wid) ثابت
{
      أرجع vvp_vector2_t(*this, base, wid);
}

/*
 * This class represents a scalar value with strength. These are
 * heavier than the simple vvp_bit4_t, but more information is
 * carried by that weight.
 *
 * The strength values are as defined here:
 *   HiZ    - 0
 *   Small  - 1
 *   Medium - 2
 *   Weak   - 3
 *   Large  - 4
 *   Pull   - 5
 *   Strong - 6
 *   Supply - 7
 *
 * There are two strengths for a value: strength0 and strength1. If
 * the value is Z, then strength0 is the strength of the 0-value, and
 * strength of the 1-value. If the value is 0 or 1, then the strengths
 * are the range for that value.
 */
class vvp_scalar_t {

      friend vvp_scalar_t fully_featured_resolv_(vvp_scalar_t a, vvp_scalar_t b);

    عام:
	// Make a HiZ value.
      explicit vvp_scalar_t();

	// Make an unambiguous value.
      explicit vvp_scalar_t(vvp_bit4_t val, unsigned str0, unsigned str1);

	// Get the vvp_bit4_t version of the value
      vvp_bit4_t قىممەت() ثابت;
      unsigned strength0() ثابت;
      unsigned strength1() ثابت;

      bool eeq(vvp_scalar_t that) ثابت { أرجع قىممەت_2 == that.قىممەت_2; }
      bool is_hiz() ثابت { أرجع (قىممەت_2 & 0x77) == 0; }

    خاص:
	// This class and the vvp_vector8_t class are closely related,
	// so allow vvp_vector8_t access to the raw encoding so that
	// it can do compact vectoring of vvp_scalar_t objects.
      friend class vvp_vector8_t;
      explicit vvp_scalar_t(unsigned حرف val) : قىممەت_2(val) { }
      unsigned حرف raw() ثابت { أرجع قىممەت_2; }

    خاص:
      unsigned حرف قىممەت_2;
};

مضمن vvp_scalar_t::vvp_scalar_t()
{
      قىممەت_2 = 0;
}

مضمن vvp_scalar_t::vvp_scalar_t(vvp_bit4_t val, unsigned str0, unsigned str1)
{
      أكد(str0 <= 7);
      أكد(str1 <= 7);

      إذا (str0 == 0 && str1 == 0) {
	    قىممەت_2 = 0x00;
      } وإلا بدل (val) {
	  حالة BIT4_0:
	    قىممەت_2 = str0 | (str0<<4);
	    اكسر;
	  حالة BIT4_1:
	    قىممەت_2 = str1 | (str1<<4) | 0x88;
	    اكسر;
	  حالة BIT4_X:
	    قىممەت_2 = str0 | (str1<<4) | 0x80;
	    اكسر;
	  حالة BIT4_Z:
	    قىممەت_2 = 0x00;
	    اكسر;
      }
}

مضمن vvp_bit4_t vvp_scalar_t::قىممەت() ثابت
{
      إذا ((قىممەت_2 & 0x77) == 0) {
	    أرجع BIT4_Z;
      } وإلا بدل (قىممەت_2 & 0x88) {
	  حالة 0x00:
	    أرجع BIT4_0;
	  حالة 0x88:
	    أرجع BIT4_1;
	  افتراضي:
	    أرجع BIT4_X;
      }
}


مضمن vvp_scalar_t resolve(vvp_scalar_t a, vvp_scalar_t b)
{
      خارجي vvp_scalar_t fully_featured_resolv_(vvp_scalar_t, vvp_scalar_t);

	// If the value is HiZ, resolution is simply a matter of
	// returning the *other* value.
      إذا (a.is_hiz())
	    أرجع b;
      إذا (b.is_hiz())
	    أرجع a;
	// If the values are the identical, then resolution is simply
	// returning *either* value.
      إذا (a .eeq( b ))
	    أرجع a;

      أرجع fully_featured_resolv_(a,b);
}

خارجي std::ostream& operator<< (std::ostream&, vvp_scalar_t);

/*
 * This class is a way to carry vectors of strength modeled
 * values. The 8 in the name is the number of possible distinct values
 * a well defined bit may have. When you add in ambiguous values, the
 * number of distinct values span the vvp_scalar_t.
 *
 * A vvp_vector8_t object can be created from a vvp_vector4_t and a
 * strength value. The vvp_vector8_t bits have the values of the input
 * vector, all with the strength specified. If no strength is
 * specified, then the conversion from bit4 to a scalar will use the
 * Verilog convention default of strong (6).
 */
class vvp_vector8_t {

      friend vvp_vector8_t part_expand(ثابت vvp_vector8_t&, unsigned, unsigned);

    عام:
      explicit vvp_vector8_t(unsigned size =0);
	// Make a vvp_vector8_t from a vector4 and a specified
	// strength.
      explicit vvp_vector8_t(ثابت vvp_vector4_t&that,
			     unsigned str0,
			     unsigned str1);
      explicit vvp_vector8_t(ثابت vvp_vector2_t&that,
			     unsigned str0,
			     unsigned str1);

      ~vvp_vector8_t();

      ساكن ثابت vvp_vector8_t nil;

    عام:

      unsigned size() ثابت { أرجع size_; }
      vvp_scalar_t قىممەت(unsigned idx) ثابت;
      vvp_vector8_t subvalue(unsigned adr, unsigned width) ثابت;
      فراغ set_bit(unsigned idx, vvp_scalar_t val);
      فراغ set_vec(unsigned idx, ثابت vvp_vector8_t&that);

	// Test that the vectors are exactly equal
      bool eeq(ثابت vvp_vector8_t&that) ثابت;

      vvp_vector8_t(ثابت vvp_vector8_t&that);
      vvp_vector8_t& operator= (ثابت vvp_vector8_t&that);

    خاص:
      unsigned size_;
      اتحاد {
	    unsigned حرف*ptr_;
	    unsigned حرف val_[حجم(فراغ*)];
      };
};

  /* Resolve uses the default Verilog resolver algorithm to resolve
     two drive vectors to a single output. */
مضمن vvp_vector8_t resolve(ثابت vvp_vector8_t&a, ثابت vvp_vector8_t&b)
{
      أكد(a.size() == b.size());
      vvp_vector8_t out (a.size());

      لكل (unsigned idx = 0 ;  idx < out.size() ;  idx += 1) {
	    out.set_bit(idx, resolve(a.قىممەت(idx), b.قىممەت(idx)));
      }

      أرجع out;
}

  /* This lookup tabke implements the strength reduction implied by
     Verilog standard switch devices. The major dimension selects
     between non-resistive and resistive devices. */
خارجي unsigned vvp_switch_strength_map[2][8];

  /* The reduce4 function converts a vector8 to a vector4, losing
     strength information in the process. */
خارجي vvp_vector4_t reduce4(ثابت vvp_vector8_t&that);
خارجي vvp_vector8_t part_expand(ثابت vvp_vector8_t&a, unsigned wid, unsigned off);

  /* A c8string is of the form C8<...> where ... are bits. */
خارجي bool c8string_test(ثابت حرف*str);
خارجي vvp_vector8_t c8string_to_vector8(ثابت حرف*str);

  /* Print a vector8 value to a stream. */
خارجي std::ostream& operator<< (std::ostream&, ثابت vvp_vector8_t&);

مضمن vvp_vector8_t::vvp_vector8_t(unsigned size__)
: size_(size__)
{
      إذا (size_ <= حجم(val_)) {
	    memset(val_, 0, حجم(val_));
      } وإلا {
	    ptr_ = new unsigned حرف[size_];
	    memset(ptr_, 0, size_);
      }
}

مضمن vvp_vector8_t::~vvp_vector8_t()
{
      إذا (size_ > حجم(val_))
	    delete[]ptr_;
}

مضمن vvp_scalar_t vvp_vector8_t::قىممەت(unsigned idx) ثابت
{
      أكد(idx < size_);
      إذا (size_ <= حجم(val_))
	    أرجع vvp_scalar_t(val_[idx]);
      وإلا
	    أرجع vvp_scalar_t(ptr_[idx]);
}

مضمن فراغ vvp_vector8_t::set_bit(unsigned idx, vvp_scalar_t val)
{
      أكد(idx < size_);
      إذا (size_ <= حجم(val_))
	    val_[idx] = val.raw();
      وإلا
	    ptr_[idx] = val.raw();
}

  // Exactly-equal for vvp_vector8_t is common and should be as tight
  // as possible.
مضمن bool vvp_vector8_t::eeq(ثابت vvp_vector8_t&that) ثابت
{
      إذا (size_ != that.size_)
	    أرجع false;
      إذا (size_ == 0)
	    أرجع true;

      إذا (size_ <= حجم(val_))
	      // This is equivalent to memcmp(val_, that.val_, sizeof val_)==0
	    أرجع ptr_ == that.ptr_;
      وإلا
	    أرجع memcmp(ptr_, that.ptr_, size_) == 0;
}

/*
 * This class implements a pointer that points to an item within a
 * target. The ptr() method returns a pointer to the vvp_net_t, and
 * the port() method returns a 0-3 value that selects the input within
 * the vvp_net_t. Use this pointer to point only to the inputs of
 * vvp_net_t objects. To point to vvp_net_t objects as a whole, use
 * vvp_net_t* pointers.
 *
 * Alert! Ugly details. Protective clothing recommended!
 * The vvp_net_ptr_t encodes the bits of a C pointer, and two bits of
 * port identifier into an unsigned long. This works only if vvp_net_t*
 * values are always aligned on 4-byte boundaries.
 */
template <class T> class vvp_sub_pointer_t {

    عام:
      vvp_sub_pointer_t() : bits_(0) { }

      vvp_sub_pointer_t(T*ptr__, unsigned port__)
      {
	    bits_ = reinterpret_cast<uintptr_t> (ptr__);
	    أكد( (bits_  &  UINTPTR_C(3)) == 0 );
	    أكد( (port__ & ~UINTPTR_C(3)) == 0 );
	    bits_ |= port__;
      }

      ~vvp_sub_pointer_t() = افتراضي;

      T* ptr()
      { أرجع reinterpret_cast<T*> (bits_ & ~UINTPTR_C(3)); }

      ثابت T* ptr() ثابت
      { أرجع reinterpret_cast<ثابت T*> (bits_ & ~UINTPTR_C(3)); }

      unsigned  port() ثابت { أرجع bits_ & UINTPTR_C(3); }

      bool nil() ثابت { أرجع bits_ == 0; }

      bool operator == (vvp_sub_pointer_t that) ثابت { أرجع bits_ == that.bits_; }
      bool operator != (vvp_sub_pointer_t that) ثابت { أرجع bits_ != that.bits_; }

    خاص:
      uintptr_t bits_;
};

تعريفنوع vvp_sub_pointer_t<vvp_net_t> vvp_net_ptr_t;
template <class T> std::ostream& operator << (std::ostream&out, vvp_sub_pointer_t<T> val)
{ out << val.ptr() << "[" << val.port() << "]"; أرجع out; }

/*
 * This is the basic unit of netlist connectivity. It is a fan-in of
 * up to 4 inputs, and output pointer, and a pointer to the node's
 * functionality.
 *
 * A net output that is non-nil points to the input of one of its
 * destination nets. If there are multiple destinations, then the
 * referenced input port points to the next input. For example:
 *
 *   +--+--+--+--+---+
 *   |  |  |  |  | . |  Output from this vvp_net_t points to...
 *   +--+--+--+--+-|-+
 *                 |
 *                /
 *               /
 *              /
 *             |
 *             v
 *   +--+--+--+--+---+
 *   |  |  |  |  | . |  ... the fourth input of this vvp_net_t, and...
 *   +--+--+--+--+-|-+
 *             |   |
 *            /    .
 *           /     .
 *          |      .
 *          v
 *   +--+--+--+--+---+
 *   |  |  |  |  | . |  ... the third input of this vvp_net_t.
 *   +--+--+--+--+-|-+
 *
 * Thus, the fan-in of a vvp_net_t node is limited to 4 inputs, but
 * the fan-out is unlimited.
 *
 * The vvp_send_*() functions take as input a vvp_net_ptr_t and follow
 * all the fan-out chain, delivering the specified value. The send_*()
 * methods of the vvp_net_t class are similar, but they follow the
 * output, possibly filtered, from the vvp_net_t.
 */
class vvp_net_t {
    عام:
      vvp_net_t();

#ifdef CHECK_WITH_VALGRIND
      vvp_net_t *pool;
#endif
      vvp_net_ptr_t port[4];
      vvp_net_fun_t*fun;
      vvp_net_fil_t*fil;

    عام:
	// Connect the port to the output from this net.
      فراغ link(vvp_net_ptr_t port);
	// Disconnect the port from the output of this net.
      فراغ unlink(vvp_net_ptr_t port);

    عام: // Methods to propagate output from this node.
      فراغ send_vec4(ثابت vvp_vector4_t&val, vvp_context_t context);
      فراغ send_vec8(ثابت vvp_vector8_t&val);
      فراغ send_real(مزدوج val, vvp_context_t context);
      فراغ send_string(ثابت std::string&val, vvp_context_t context);
      فراغ send_object(vvp_object_t val, vvp_context_t context);

      فراغ send_vec4_pv(ثابت vvp_vector4_t&val,
			unsigned base, unsigned vwid, vvp_context_t context);
      فراغ send_vec8_pv(ثابت vvp_vector8_t&val,
			unsigned base, unsigned vwid);


    عام: // Methods to arrange for the output of this net to be forced.

	// The intent is that all efforts at force are directed to
	// operate only on the vvp_net_t whose output is to be
	// forced. These methods then communicate the force to the
	// attached filter to set up the actual force.
      فراغ force_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&mask);
      فراغ force_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&mask);
      فراغ force_real(مزدوج val, ثابت vvp_vector2_t&mask);

    عام: // Method to support $countdrivers
      فراغ سانى_drivers(unsigned idx, unsigned counts[4]);

    // This needs to be public so that SDF interconnects can be inserted
    عام:
      vvp_net_ptr_t out_;

    عام: // Need a better new for these objects.
      ساكن فراغ* operator new(std::size_t size);
      ساكن فراغ operator delete(فراغ*); // not implemented
    خاص: // not implemented
      ساكن فراغ* operator new[](std::size_t size);
#إذا defined(__GNUC__)
      ساكن فراغ operator delete[](فراغ*);
#endif
};

/*
 * Instances of this class represent the functionality of a
 * node. vvp_net_t objects hold pointers to the vvp_net_fun_t
 * associated with it. Objects of this type take inputs that arrive at
 * a port and perform some sort of action in response.
 *
 * Whenever a bit is delivered to a vvp_net_t object, the associated
 * vvp_net_fun_t::recv_*() method is invoked with the port pointer and
 * the bit value. The port pointer is used to figure out which exact
 * input receives the bit.
 *
 * In this context, a "bit" is the thing that arrives at a single
 * input. The bit may be a single data bit, a bit vector, various
 * sorts of numbers or aggregate objects.
 *
 * recv_vec4 is the most common way for a datum to arrive at a
 * port. The value is a vvp_vector4_t.
 *
 * Most nodes do not care about the specific strengths of bits, so the
 * default behavior for recv_vec8 and recv_vec8_pv is to reduce the
 * operand to a vvp_vector4_t and pass it on to the recv_vec4 or
 * recv_vec4_pv method.
 *
 * The recv_vec4, recv_vec4_pv, and recv_real methods are also
 * passed a context pointer. When the received bit has propagated
 * from a statically allocated node, this will be a null pointer.
 * When the received bit has propagated from an automatically
 * allocated node, this will be a pointer to the context that
 * contains the instance of that bit that has just been modified.
 * When the received bit was from a procedural assignment or from
 * a VPI set_value() operation, this will be a pointer to the
 * writable context associated with the currently running thread.
 */
class vvp_net_fun_t {

    عام:
      vvp_net_fun_t();
      virtual ~vvp_net_fun_t();

      virtual فراغ recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                             vvp_context_t context);
      virtual فراغ recv_vec8(vvp_net_ptr_t port, ثابت vvp_vector8_t&bit);
      virtual فراغ recv_real(vvp_net_ptr_t port, مزدوج bit,
                             vvp_context_t context);
      virtual فراغ recv_string(vvp_net_ptr_t port, ثابت std::string&bit,
			       vvp_context_t context);
      virtual فراغ recv_object(vvp_net_ptr_t port, vvp_object_t bit,
			       vvp_context_t context);

	// Part select variants of above
      virtual فراغ recv_vec4_pv(vvp_net_ptr_t p, ثابت vvp_vector4_t&bit,
				unsigned base, unsigned vwid, vvp_context_t context);
      virtual فراغ recv_vec8_pv(vvp_net_ptr_t p, ثابت vvp_vector8_t&bit,
				unsigned base, unsigned vwid);

	// This method is called when the net is forced or
	// released. This is very rarely needed; island ports use it
	// to know that the net is being forced and that it needs to
	// do something about it.
      virtual فراغ force_flag(bool run_now);

   محمي:
      فراغ recv_vec4_pv_(vvp_net_ptr_t p, ثابت vvp_vector4_t&bit,
			 unsigned base, unsigned vwid, vvp_context_t context);
      فراغ recv_vec8_pv_(vvp_net_ptr_t p, ثابت vvp_vector8_t&bit,
			 unsigned base, unsigned vwid);

    عام: // These objects are only permallocated.
      ساكن فراغ* operator new(std::size_t size) { أرجع heap_.alloc(size); }
      ساكن فراغ operator delete(فراغ*); // not implemented

      ساكن std::size_t heap_total() { أرجع heap_.heap_total(); }

    محمي:
      ساكن permaheap heap_;

    خاص: // not implemented
      vvp_net_fun_t(ثابت vvp_net_fun_t&);
      vvp_net_fun_t& operator= (ثابت vvp_net_fun_t&);
      ساكن فراغ* operator new[](std::size_t size);
      ساكن فراغ operator delete[](فراغ*);
};

/*
 * A vvp_net_fil_t is a filter object that filters an output from a
 * vvp_net_t. The send_*() methods of the vvp_net_t object invoke the
 * filter of the output being transmitted. The filter function will
 * decide if this value is to be propagated, and how, and return a
 * prop_t enumeration to reflect the choice.
 *
 * The filter object also provides an implementation hooks for
 * force/release.
 */
class vvp_net_fil_t  : عام vvp_vpi_callback {

    عام:
      vvp_net_fil_t();
      virtual ~vvp_net_fil_t() override;

    عام:
      تعداد prop_t { STOP=0, PROP, REPL };


	// These filter methods are used by the vvp_net_t::send_*()
	// methods to test the output (from the functor) bit value
	// against any force filters. If none of the bits are forced,
	// then the method returns PROP and the caller propagates the
	// bit value. If bits were changed by the force mask, then the
	// method returns REPL and the caller should propagate the rep
	// value instead. If the function returns STOP, then all the
	// output bits are filtered by the force mask and there is
	// nothing to propagate.
      virtual prop_t filter_vec4(ثابت vvp_vector4_t&bit, vvp_vector4_t&rep,
				 unsigned base, unsigned vwid);
      virtual prop_t filter_vec8(ثابت vvp_vector8_t&val, vvp_vector8_t&rep,
				 unsigned base, unsigned vwid);
      virtual prop_t filter_real(مزدوج&val);
      virtual prop_t filter_object(vvp_object_t&val);
      virtual prop_t filter_string(ثابت std::string&val);

      virtual فراغ release(vvp_net_ptr_t ptr, bool net_flag) =0;
      virtual فراغ release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) =0;

	// The %force/link instruction needs a place to write the
	// source node of the force, so that subsequent %force and
	// %release instructions can undo the link as needed. */
      فراغ force_link(vvp_net_t*dst, vvp_net_t*src);
      فراغ force_unlink(فراغ);

      virtual unsigned filter_size() ثابت =0;

    عام:
	// Support for force methods. These are called by the
	// vvp_net_t::force_* methods to set the force value and mask
	// for the filter.
      virtual فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&mask) =0;
      virtual فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&mask) =0;
      virtual فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&mask) =0;

    عام: // These objects are only permallocated.
      ساكن فراغ* operator new(std::size_t size) { أرجع heap_.alloc(size); }
      ساكن فراغ operator delete(فراغ*); // not implemented

      ساكن size_t heap_total() { أرجع heap_.heap_total(); }

    خاص:
      ساكن permaheap heap_;

    خاص: // not implemented
      vvp_net_fil_t(ثابت vvp_net_fil_t&);
      vvp_net_fil_t& operator= (ثابت vvp_net_fil_t&);
      ساكن فراغ* operator new[](std::size_t size);
      ساكن فراغ operator delete[](فراغ*);

    محمي:
	// Set bits of the filter force mask
      فراغ force_mask(ثابت vvp_vector2_t&mask);
	// Release the force on the bits set in the mask.
      فراغ release_mask(ثابت vvp_vector2_t&mask);
	// Test bits of the filter force mask;
      bool test_force_mask(unsigned bit) ثابت;
      bool test_force_mask_is_zero() ثابت;

	// This template method is used by derived classes to process
	// the val through the force mask. The force value is the
	// currently forced value, and the buf is a value that this
	// method will use to hold a filtered value, if needed. This
	// method returns the replacement value into the "rep"
	// argument and returns a code indicating whether any changes
	// were made.
      template <class T> prop_t filter_mask_(ثابت T&val, ثابت T&force, T&rep, unsigned addr);
	// This template method is similar to the above, but works for
	// native types that are not so expensive to edit in place.
      template <class T> prop_t filter_mask_(T&val, T force);

	// These templates are similar to filter_mask_, but are
	// idempotent. Then do not trigger callbacks or otherwise
	// cause any local changes. These methods are used to test
	// arbitrary values against the force mask.
      template <class T> prop_t filter_كىرگۈزۈش_mask(ثابت T&val, ثابت T&force, T&rep) ثابت;

    خاص:
	// Mask of forced bits
      vvp_vector2_t force_mask_;
	// True if the next filter must propagate. Need this to allow
	// the forced value to get through.
      bool force_propagate_;
	// force link back.
      class vvp_net_t*force_link_;
};

/* **** Some core net functions **** */

/*
 * The vvp_fun_force class objects are net functors that use their input
 * to force the associated filter. They do not actually  have an
 * output, they instead drive the force_* methods of the net filter.
 *
 * This functor is also special in that we know a priori that only
 * port-0 is used, so we can use ports 1-3 for local storage. See the
 * implementation of vvp_filter_wire_base::force_link in
 * vvp_net_sig.cc for details.
 */
class vvp_fun_force : عام vvp_net_fun_t {

    عام:
      vvp_fun_force();
      ~vvp_fun_force() override;

      فراغ recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
		     vvp_context_t context) override;
      فراغ recv_real(vvp_net_ptr_t port, مزدوج bit, vvp_context_t) override;
};

/* vvp_fun_drive
 * This node function takes an input vvp_vector4_t as input, and
 * repeats that value as a vvp_vector8_t with all the bits given the
 * strength of the drive. This is the way vvp_scalar8_t objects are
 * created. Input 0 is the value to be driven (vvp_vector4_t) and
 * inputs 1 and two are the strength0 and strength1 values to use. The
 * strengths may be taken as constant values, or adjusted as longs
 * from the network.
 *
 * This functor only propagates vvp_vector8_t values.
 */
class vvp_fun_drive  : عام vvp_net_fun_t {

    عام:
      explicit vvp_fun_drive(unsigned str0 =6, unsigned str1 =6);
      ~vvp_fun_drive() override;

      فراغ recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t context) override;

      فراغ recv_vec4_pv(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;
    خاص:
      unsigned حرف drive0_;
      unsigned حرف drive1_;
};

/*
 * EXTEND functors expand an input vector to the desired output
 * width. The extend_signed functor sign extends the input. If the
 * input is already wider than the desired output, then it is passed
 * unmodified.
 */
class vvp_fun_extend_signed  : عام vvp_net_fun_t {

    عام:
      explicit vvp_fun_extend_signed(unsigned wid);
      ~vvp_fun_extend_signed() override;

      فراغ recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t context) override;

      فراغ recv_vec4_pv(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;
    خاص:
      unsigned width_;
};

/*
 * Wide Functors:
 * Wide functors represent special devices that may have more than 4
 * input ports. These devices need a set of N/4 actual functors to
 * catch the inputs, and use another to deliver the output.
 *
 *  vvp_wide_fun_t --+--> vvp_wide_fun_core --> ...
 *                   |
 *  vvp_wide_fun_t --+
 *                   |
 *  vvp_wide_fun_t --+
 *
 * There are enough input functors to take all the functor inputs, 4
 * per functor. These inputs deliver the changed input value to the
 * wide_fun_core, which carries the infrastructure for the thread. The
 * wide_fun_core is also a functor whose output is connected to the rest
 * of the netlist. This is where the result is delivered back to the
 * netlist.
 *
 * The vvp_wide_fun_core keeps a store of the inputs from all the
 * input functors, and makes them available to the derived class that
 * does the processing.
 */

class vvp_wide_fun_core : عام vvp_net_fun_t {

    عام:
      vvp_wide_fun_core(vvp_net_t*net, unsigned nports);
      virtual ~vvp_wide_fun_core() override;
	// These objects are not perm allocated.
      ساكن فراغ* operator new(std::size_t size) { أرجع ::new حرف[size]; }
      ساكن فراغ operator delete(فراغ* ptr) { ::delete[](static_cast<حرف*>(ptr)); }

    محمي:
      فراغ propagate_vec4(ثابت vvp_vector4_t&bit, vvp_time64_t delay =0);
      فراغ propagate_real(مزدوج bit, vvp_time64_t delay =0);
      unsigned port_سانى() ثابت;

      vvp_vector4_t& قىممەت(unsigned);
      مزدوج قىممەت_r(unsigned);

    خاص:
	// the derived class implements this to receive an indication
	// that one of the port input values changed.
      virtual فراغ recv_vec4_from_inputs(unsigned port) =0;
      virtual فراغ recv_real_from_inputs(unsigned port);

      friend class vvp_wide_fun_t;
      فراغ dispatch_vec4_from_كىرگۈزۈش(unsigned port, ثابت vvp_vector4_t&bit);
      فراغ dispatch_real_from_كىرگۈزۈش(unsigned port, مزدوج bit);

    خاص:
	// Back-point to the vvp_net_t that points to me.
      vvp_net_t*ptr_;
	// Structure to track the input values from the input functors.
      unsigned nports_;
      vvp_vector4_t*port_قىممەتلەر;
      مزدوج*port_rvalues_;
};

/*
 * The job of the input functor is only to monitor inputs to the
 * function and pass them to the ufunc_core object. Each functor takes
 * up to 4 inputs, with the base the port number for the first
 * function input that this functor represents.
 */
class vvp_wide_fun_t : عام vvp_net_fun_t {

    عام:
      vvp_wide_fun_t(vvp_wide_fun_core*c, unsigned base);
      ~vvp_wide_fun_t() override;

      فراغ recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t context) override;
      فراغ recv_real(vvp_net_ptr_t port, مزدوج bit,
                     vvp_context_t context) override;

      فراغ recv_vec4_pv(vvp_net_ptr_t p, ثابت vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t context) override;

    خاص:
      vvp_wide_fun_core*core_;
      unsigned port_base_;
};


مضمن فراغ vvp_send_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&val, vvp_context_t context)
{
      طالما (class vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    إذا (cur->fun)
		  cur->fun->recv_vec4(ptr, val, context);

	    ptr = next_val;
      }
}

خارجي فراغ vvp_send_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&val);
خارجي فراغ vvp_send_real(vvp_net_ptr_t ptr, مزدوج val,
                          vvp_context_t context);

مضمن فراغ vvp_send_string(vvp_net_ptr_t ptr, ثابت std::string&val, vvp_context_t context)
{
      طالما (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    إذا (cur->fun)
		  cur->fun->recv_string(ptr, val, context);

	    ptr = next_val;
      }
}

مضمن فراغ vvp_send_object(vvp_net_ptr_t ptr, vvp_object_t val, vvp_context_t context)
{
      طالما (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    إذا (cur->fun)
		  cur->fun->recv_object(ptr, val, context);

	    ptr = next_val;
      }
}

/*
 * Part-vector versions of above functions. This function uses the
 * corresponding recv_vec4_pv method in the vvp_net_fun_t functor to
 * deliver parts of a vector.
 *
 * The ptr is the destination input port to write to.
 *
 * <val> is the vector to be written.
 *
 * The <base> is where in the receiver the bit vector is to be
 * written. This address is given in canonical units; 0 is the LSB, 1
 * is the next bit, and so on.
 *
 * The <vwid> is the width of the destination vector that this part is
 * part of. This is used by intermediate nodes, i.e. resolvers, to
 * know how wide to pad with Z, if it needs to transform the part to a
 * mirror of the destination vector.
 */
مضمن فراغ vvp_send_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&val,
			     unsigned base, unsigned vwid,
			     vvp_context_t context)
{
      طالما (class vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    إذا (cur->fun)
		  cur->fun->recv_vec4_pv(ptr, val, base, vwid, context);

	    ptr = next_val;
      }
}

مضمن فراغ vvp_send_vec8_pv(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&val,
			     unsigned base, unsigned vwid)
{
      طالما (class vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    إذا (cur->fun)
		  cur->fun->recv_vec8_pv(ptr, val, base, vwid);

	    ptr = next_val;
      }
}

مضمن فراغ vvp_net_t::send_vec4(ثابت vvp_vector4_t&val, vvp_context_t context)
{
      إذا (fil == 0) {
	    vvp_send_vec4(out_, val, context);
	    أرجع;
      }

      vvp_vector4_t rep;
      بدل (fil->filter_vec4(val, rep, 0, val.size())) {
	  حالة vvp_net_fil_t::STOP:
	    اكسر;
	  حالة vvp_net_fil_t::PROP:
	    vvp_send_vec4(out_, val, context);
	    اكسر;
	  حالة vvp_net_fil_t::REPL:
	    vvp_send_vec4(out_, rep, context);
	    اكسر;
      }
}

مضمن فراغ vvp_net_t::send_vec4_pv(ثابت vvp_vector4_t&val,
				    unsigned base, unsigned vwid,
				    vvp_context_t context)
{
      إذا (fil == 0) {
	    vvp_send_vec4_pv(out_, val, base, vwid, context);
	    أرجع;
      }

      vvp_vector4_t rep;
      بدل (fil->filter_vec4(val, rep, base, vwid)) {
	  حالة vvp_net_fil_t::STOP:
	    اكسر;
	  حالة vvp_net_fil_t::PROP:
	    vvp_send_vec4_pv(out_, val, base, vwid, context);
	    اكسر;
	  حالة vvp_net_fil_t::REPL:
	    vvp_send_vec4_pv(out_, rep, base, vwid, context);
	    اكسر;
      }
}

مضمن فراغ vvp_net_t::send_vec8(ثابت vvp_vector8_t&val)
{
      إذا (fil == 0) {
	    vvp_send_vec8(out_, val);
	    أرجع;
      }

      vvp_vector8_t rep;
      بدل (fil->filter_vec8(val, rep, 0, val.size())) {
	  حالة vvp_net_fil_t::STOP:
	    اكسر;
	  حالة vvp_net_fil_t::PROP:
	    vvp_send_vec8(out_, val);
	    اكسر;
	  حالة vvp_net_fil_t::REPL:
	    vvp_send_vec8(out_, rep);
	    اكسر;
      }
}

مضمن فراغ vvp_net_t::send_vec8_pv(ثابت vvp_vector8_t&val,
				    unsigned base, unsigned vwid)
{
      إذا (fil == 0) {
	    vvp_send_vec8_pv(out_, val, base, vwid);
	    أرجع;
      }

      vvp_vector8_t rep;
      بدل (fil->filter_vec8(val, rep, base, vwid)) {
	  حالة vvp_net_fil_t::STOP:
	    اكسر;
	  حالة vvp_net_fil_t::PROP:
	    vvp_send_vec8_pv(out_, val, base, vwid);
	    اكسر;
	  حالة vvp_net_fil_t::REPL:
	    vvp_send_vec8_pv(out_, rep, base, vwid);
	    اكسر;
      }
}

مضمن فراغ vvp_net_t::send_real(مزدوج val, vvp_context_t context)
{
      إذا (fil && ! fil->filter_real(val))
	    أرجع;

      vvp_send_real(out_, val, context);
}


مضمن فراغ vvp_net_t::send_string(ثابت std::string&val, vvp_context_t context)
{
      إذا (fil && !fil->filter_string(val))
	    أرجع;

      vvp_send_string(out_, val, context);
}


مضمن فراغ vvp_net_t::send_object(vvp_object_t val, vvp_context_t context)
{
      إذا (fil && ! fil->filter_object(val))
	    أرجع;

      vvp_send_object(out_, val, context);
}


مضمن bool vvp_net_fil_t::test_force_mask(unsigned bit) ثابت
{
      إذا (bit >= force_mask_.size())
	    أرجع false;
      إذا (force_mask_.قىممەت(bit))
	    أرجع true;
      وإلا
	    أرجع false;
}

مضمن bool vvp_net_fil_t::test_force_mask_is_zero(فراغ) ثابت
{
      إذا (force_mask_.size() == 0)
	    أرجع true;
      إذا (force_mask_.is_zero())
	    أرجع true;
      أرجع false;
}

/*
 * Undefine the ivl_alloc.h definitions so they don't leak out of this file.
 */
#undef malloc
#undef realloc
#undef calloc
#undef __ivl_alloc_H

#endif /* IVL_vvp_net_H */
