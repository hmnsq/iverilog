#که_نه_تعریف_شوی IVL_vvp_net_H
#تعریف_کړه IVL_vvp_net_H
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

#که_تعریف_شوی HAVE_IOSFWD
# include  <iosfwd>
#وإلا
# include  <iostream>
#د_که_پای

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

تعريفنوع فراغ*vvp_context_توکی_t;

مضمن vvp_context_t vvp_allocate_context(بینښه nitem)
{
      أرجع (vvp_context_t)حافظه_ورکړه((2 + nitem) * حجم(فراغ*));
}

مضمن vvp_context_t vvp_get_بل_context(vvp_context_t context)
{
      أرجع (vvp_context_t)context[0];
}

مضمن فراغ vvp_سېټ_بل_context(vvp_context_t context, vvp_context_t بل_2)
{
      context[0] = بل_2;
}

مضمن vvp_context_t vvp_get_stacked_context(vvp_context_t context)
{
      أرجع (vvp_context_t)context[1];
}

مضمن فراغ vvp_سېټ_stacked_context(vvp_context_t context, vvp_context_t ډېری)
{
      context[1] = ډېری;
}

مضمن vvp_context_توکی_t vvp_get_context_توکی(vvp_context_t context,
                                               بینښه توکی_idx)
{
      أرجع (vvp_context_توکی_t)context[توکی_idx];
}

مضمن فراغ vvp_سېټ_context_توکی(vvp_context_t context, بینښه توکی_idx,
                                 vvp_context_توکی_t توکی)
{
      context[توکی_idx] = توکی;
}

/*
 * An "automatic" functor is one which may be associated with an automatically
 * allocated scope item. This provides the infrastructure needed to allocate
 * the state information for individual instances of the item.
 */
بنية automatic_hooks_s {

      automatic_hooks_s() {}
      مجازي ~automatic_hooks_s() {}

      مجازي فراغ alloc_instance(vvp_context_t context) = 0;
      مجازي فراغ reset_instance(vvp_context_t context) = 0;
#که_تعریف_شوی CHECK_WITH_VALGRIND
      مجازي فراغ free_instance(vvp_context_t context) = 0;
#د_که_پای
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
مضمن حرف vvp_bit4_منزل_ascii(vvp_bit4_t a) { أرجع "01zx"[a]; }

خارجي vvp_bit4_t جمع_کړه_with_لېږد(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c);

خارجي vvp_bit4_t scalar_منزل_bit4(PLI_INT32 scalar);

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
مضمن vvp_bit4_t چلوونکی ~ (vvp_bit4_t a)
{ أرجع bit4_z2x((vvp_bit4_t) (((صحيح)a) ^ 1)); }

مضمن vvp_bit4_t چلوونکی | (vvp_bit4_t a, vvp_bit4_t b)
{
      إذا (a==BIT4_1 || b==BIT4_1)
	    أرجع BIT4_1;
      أرجع bit4_z2x( (vvp_bit4_t) ((صحيح)a | (صحيح)b) );
}

مضمن vvp_bit4_t چلوونکی & (vvp_bit4_t a, vvp_bit4_t b)
{
      إذا (a==BIT4_0 || b==BIT4_0)
	    أرجع BIT4_0;
      أرجع bit4_z2x( (vvp_bit4_t) ((صحيح)a | (صحيح)b) );
}


خارجي vvp_bit4_t چلوونکی ^ (vvp_bit4_t a, vvp_bit4_t b);
خارجي std::ostream& چلوونکی<< (std::ostream&o, vvp_bit4_t a);

  /* Return >0, ==0 or <0 if the from-to transition represents a
     posedge, no edge, or negedge. */
خارجي صحيح څنډه(vvp_bit4_t from, vvp_bit4_t منزل);

  /* Support for $countdrivers. */
مضمن فراغ update_driver_counts(vvp_bit4_t بټ, بینښه counts[3])
{
      بدل (بټ) {
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
ساكن مضمن بینښه اوږد جمع_کړه_with_لېږد(بینښه اوږد a, بینښه اوږد b,
					   بینښه اوږد&لېږد)
{
      بینښه اوږد tmp = b + لېږد;
      بینښه اوږد مجموعه = a + tmp;
      لېږد = 0;
      إذا (tmp < b)
	    لېږد = 1;
      إذا (مجموعه < tmp)
	    لېږد = 1;
      إذا (مجموعه < a)
	    لېږد = 1;
      أرجع مجموعه;
}

خارجي بینښه اوږد multiply_with_لېږد(بینښه اوږد a, بینښه اوږد b,
					 بینښه اوږد&لېږد);

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

      ملګری vvp_vector4_t چلوونکی ~(ثابت vvp_vector4_t&that);
      ملګری class vvp_vector4array_t;
      ملګری class vvp_vector4array_sa;
      ملګری class vvp_vector4array_aa;

    عام:
      ساكن ثابت vvp_vector4_t nil;

    عام:
      explicit vvp_vector4_t(بینښه اندازه =0, vvp_bit4_t بټونه =BIT4_X);

      explicit vvp_vector4_t(بینښه اندازه, مزدوج val);

	// Construct a vector4 from the subvalue of another
	// vector4. The width of the result is 'wid', and the bits are
	// pulled from 'that' to implement the Verilog part select
	// semantics. This means that part select beyond 'that'
	// returns X bits.
      explicit vvp_vector4_t(ثابت vvp_vector4_t&that,
			     بینښه adr, بینښه wid);

      vvp_vector4_t(ثابت vvp_vector4_t&that);
      vvp_vector4_t(ثابت vvp_vector4_t&that, bool invert_flag);
      vvp_vector4_t& چلوونکی= (ثابت vvp_vector4_t&that);

      ~vvp_vector4_t();

      مضمن بینښه اندازه() ثابت { أرجع اندازه_2; }
      فراغ resize(بینښه new_اندازه, vvp_bit4_t pad_بټ = BIT4_X);

	// Get the bit at the specified address
      vvp_bit4_t ارزښت(بینښه idx) ثابت;
	// Get the vector4 subvector starting at the address
      vvp_vector4_t subvalue(بینښه idx, بینښه اندازه) ثابت;
	// Get the 2-value bits for the subvector. This returns a new
	// array of longs, or a nil pointer if an XZ bit was detected
	// in the array.
      بینښه اوږد*subarray(بینښه idx, بینښه اندازه, bool xz_منزل_0 =false) ثابت;
      فراغ setarray(بینښه idx, بینښه اندازه, ثابت بینښه اوږد*val);

	// Set a 4-value bit or subvector into the vector. Return true
	// if any bits of the vector change as a result of this operation.
      فراغ سېټ_بټ(بینښه idx, vvp_bit4_t val);
      bool سېټ_vec(بینښه idx, ثابت vvp_vector4_t&that);

        // Get the bits from another vector, but keep my size.
      فراغ copy_بټونه(ثابت vvp_vector4_t&that);

	// Move bits within this vector.
      فراغ mov(بینښه dst, بینښه src, بینښه cnt);

	// Add that to this in the Verilog way.
      فراغ جمع_کړه(ثابت vvp_vector4_t&that);

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
      فراغ fill_بټونه(vvp_bit4_t بټ);

	// Change all bits to X bits.
      فراغ سېټ_منزل_x() { fill_بټونه(BIT4_X); }

	// Display the value into the buf as a string.
      حرف*as_string(حرف*buf, size_t buf_len) ثابت;

      فراغ invert();
      vvp_vector4_t& چلوونکی &= (ثابت vvp_vector4_t&that);
      vvp_vector4_t& چلوونکی |= (ثابت vvp_vector4_t&that);
      vvp_vector4_t& چلوونکی ^= (ثابت vvp_vector4_t&that);
      vvp_vector4_t& چلوونکی += (int64_t);

      vvp_bit4_t reduce_or() ثابت;
      vvp_bit4_t reduce_and() ثابت;
      vvp_bit4_t reduce_xor() ثابت;

    خاص:
	// Number of vvp_bit4_t bits that can be shoved into a word.
      تعداد { بټونه_فی_کلمه = 8*حجم(بینښه اوږد) };
	// The double value constructor requires that WORD_0_BBITS
	// and WORD_1_BBITS have the same value!
#إذا SIZEOF_UNSIGNED_LONG == 8
      تعداد { کلمه_0_ABITS = 0x0000000000000000UL,
	     کلمه_0_BBITS = 0x0000000000000000UL };
      تعداد { کلمه_1_ABITS = 0xFFFFFFFFFFFFFFFFUL,
	     کلمه_1_BBITS = 0x0000000000000000UL };
      تعداد { کلمه_X_ABITS = 0xFFFFFFFFFFFFFFFFUL,
	     کلمه_X_BBITS = 0xFFFFFFFFFFFFFFFFUL };
      تعداد { کلمه_Z_ABITS = 0x0000000000000000UL,
             کلمه_Z_BBITS = 0xFFFFFFFFFFFFFFFFUL };
#وإلاإذا SIZEOF_UNSIGNED_LONG == 4
      تعداد { کلمه_0_ABITS = 0x00000000UL, کلمه_0_BBITS = 0x00000000UL };
      تعداد { کلمه_1_ABITS = 0xFFFFFFFFUL, کلمه_1_BBITS = 0x00000000UL };
      تعداد { کلمه_X_ABITS = 0xFFFFFFFFUL, کلمه_X_BBITS = 0xFFFFFFFFUL };
      تعداد { کلمه_Z_ABITS = 0x00000000UL, کلمه_Z_BBITS = 0xFFFFFFFFUL };
#وإلا
#تېروتنه "WORD_X_xBITS not defined for this architecture?"
#د_که_پای

	// Initialize and operator= use this private method to copy
	// the data from that object into this object.
      فراغ copy_from_(ثابت vvp_vector4_t&that);
      فراغ copy_from_big_(ثابت vvp_vector4_t&that);
      فراغ copy_inverted_from_(ثابت vvp_vector4_t&that);

      فراغ allocate_ټکي(بینښه اوږد inita, بینښه اوږد initb);

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

      بینښه اندازه_2;
      اتحاد {
	    بینښه اوږد abits_val_;
	    بینښه اوږد*abits_ptr_;
      };
      اتحاد {
	    بینښه اوږد bbits_val_;
	    بینښه اوږد*bbits_ptr_;
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

مضمن vvp_vector4_t::vvp_vector4_t(بینښه اندازه_3, vvp_bit4_t val)
: اندازه_2(اندازه_3)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      ساكن ثابت بینښه اوږد init_atable[4] = {
	    کلمه_0_ABITS,
	    کلمه_1_ABITS,
	    کلمه_Z_ABITS,
	    کلمه_X_ABITS };
      ساكن ثابت بینښه اوږد init_btable[4] = {
	    کلمه_0_BBITS,
	    کلمه_1_BBITS,
	    کلمه_Z_BBITS,
	    کلمه_X_BBITS };

      allocate_ټکي(init_atable[val], init_btable[val]);
}

مضمن vvp_vector4_t::~vvp_vector4_t()
{
      إذا (اندازه_2 > بټونه_فی_کلمه) {
	    delete[] abits_ptr_;
	      // bbits_ptr_ actually points half-way into a
	      // double-length array started at abits_ptr_
      }
}

مضمن vvp_vector4_t& vvp_vector4_t::چلوونکی= (ثابت vvp_vector4_t&that)
{
      إذا (this == &that)
	    أرجع *this;

      إذا (اندازه_2 > بټونه_فی_کلمه)
	    delete[] abits_ptr_;

      copy_from_(that);

      أرجع *this;
}

مضمن فراغ vvp_vector4_t::copy_from_(ثابت vvp_vector4_t&that)
{
      اندازه_2 = that.اندازه_2;
      إذا (اندازه_2 <= بټونه_فی_کلمه) {
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;
      } وإلا {
	    copy_from_big_(that);
      }
}

مضمن vvp_bit4_t vvp_vector4_t::ارزښت(بینښه idx) ثابت
{
      إذا (idx >= اندازه_2)
	    أرجع BIT4_X;

      بینښه off;

      بینښه اوږد abits, bbits;
      إذا (اندازه_2 > بټونه_فی_کلمه) {
	    بینښه wdx = idx / بټونه_فی_کلمه;
	    off = idx % بټونه_فی_کلمه;
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

مضمن vvp_vector4_t vvp_vector4_t::subvalue(بینښه adr, بینښه wid) ثابت
{
      أرجع vvp_vector4_t(*this, adr, wid);
}

مضمن فراغ vvp_vector4_t::سېټ_بټ(بینښه idx, vvp_bit4_t val)
{
      أكد(idx < اندازه_2);

      بینښه اوږد off = idx % بټونه_فی_کلمه;
      بینښه اوږد ماسک = 1UL << off;

      إذا (اندازه_2 > بټونه_فی_کلمه) {
	    بینښه wdx = idx / بټونه_فی_کلمه;
	    بدل (val) {
		حالة BIT4_0:
		  abits_ptr_[wdx] &= ~ماسک;
		  bbits_ptr_[wdx] &= ~ماسک;
		  اكسر;
		حالة BIT4_1:
		  abits_ptr_[wdx] |=  ماسک;
		  bbits_ptr_[wdx] &= ~ماسک;
		  اكسر;
		حالة BIT4_X:
		  abits_ptr_[wdx] |=  ماسک;
		  bbits_ptr_[wdx] |=  ماسک;
		  اكسر;
		حالة BIT4_Z:
		  abits_ptr_[wdx] &= ~ماسک;
		  bbits_ptr_[wdx] |=  ماسک;
		  اكسر;
	    }
      } وإلا {
	    بدل (val) {
		حالة BIT4_0:
		  abits_val_ &= ~ماسک;
		  bbits_val_ &= ~ماسک;
		  اكسر;
		حالة BIT4_1:
		  abits_val_ |=  ماسک;
		  bbits_val_ &= ~ماسک;
		  اكسر;
		حالة BIT4_X:
		  abits_val_ |=  ماسک;
		  bbits_val_ |=  ماسک;
		  اكسر;
		حالة BIT4_Z:
		  abits_val_ &= ~ماسک;
		  bbits_val_ |=  ماسک;
		  اكسر;
	    }
      }
}

مضمن vvp_vector4_t چلوونکی ~ (ثابت vvp_vector4_t&that)
{
      vvp_vector4_t res (that, true);
      أرجع res;
}

خارجي std::ostream& چلوونکی << (std::ostream&, ثابت vvp_vector4_t&);

خارجي vvp_bit4_t compare_gtge(ثابت vvp_vector4_t&a,
			       ثابت vvp_vector4_t&b,
			       vvp_bit4_t val_if_equal);
خارجي vvp_bit4_t compare_gtge_signed(ثابت vvp_vector4_t&a,
				      ثابت vvp_vector4_t&b,
				      vvp_bit4_t val_if_equal);
کينډۍ <class T> خارجي T coerce_منزل_پلنوالی(ثابت T&that, بینښه پلنوالی);

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
کينډۍ <class T> خارجي bool vector4_منزل_ارزښت(ثابت vvp_vector4_t&a, T&val,
						bool is_signed,
						bool is_arithmetic =true);

کينډۍ <class T> خارجي bool vector4_منزل_ارزښت(ثابت vvp_vector4_t&a,
                                                bool&ډکېدنه_flag, T&val);

کينډۍ <class T> مضمن bool vector4_منزل_ارزښت(ثابت vvp_vector4_t&a, T&val)
{
      bool ډکېدنه_flag;
      أرجع vector4_منزل_ارزښت(a, ډکېدنه_flag, val);
}

خارجي bool vector4_منزل_ارزښت(ثابت vvp_vector4_t&a, مزدوج&val, bool is_signed);

خارجي bool vector2_منزل_ارزښت(ثابت vvp_vector2_t&a, int32_t&val, bool is_signed);

خارجي vvp_vector4_t vector4_from_متن(ثابت حرف*بټونه, بینښه wid);

مضمن vvp_vector4_t double_منزل_vector4_LSB(مزدوج real)
{
      // Convert the double to a bit vector and then use the LSB.
      // You can use very large real values and break the 64-bit
      // size, but that's really horrible code so don't do that!
      vvp_vector4_t بټ = vvp_vector4_t(64, real);
      بټ.resize(1);
      أرجع بټ;
}

/*
 * vvp_vector4array_t
 */
class vvp_vector4array_t {

    عام:
      vvp_vector4array_t(بینښه پلنوالی, بینښه ټکي);
      مجازي ~vvp_vector4array_t();

      بینښه پلنوالی() ثابت { أرجع پلنوالی_2; }
      بینښه ټکي() ثابت { أرجع ټکي_3; }

      مجازي vvp_vector4_t get_کلمه(بینښه idx) ثابت = 0;
      مجازي فراغ سېټ_کلمه(بینښه idx, ثابت vvp_vector4_t&that) = 0;

    محمي:
      بنية v4cell {
	    اتحاد {
		  بینښه اوږد abits_val_;
		  بینښه اوږد*abits_ptr_;
	    };
	    اتحاد {
		  بینښه اوږد bbits_val_;
		  بینښه اوږد*bbits_ptr_;
	    };
      };

      vvp_vector4_t get_کلمه_2(v4cell*cell) ثابت;
      فراغ سېټ_کلمه_2(v4cell*cell, ثابت vvp_vector4_t&that);

      بینښه پلنوالی_2;
      بینښه ټکي_3;

    خاص: // Not implemented
      vvp_vector4array_t(ثابت vvp_vector4array_t&);
      vvp_vector4array_t& چلوونکی = (ثابت vvp_vector4array_t&);
};

/*
 * Statically allocated vvp_vector4array_t
 */
class vvp_vector4array_sa : عام vvp_vector4array_t {

    عام:
      vvp_vector4array_sa(بینښه پلنوالی, بینښه ټکي);
      ~vvp_vector4array_sa() override;

      vvp_vector4_t get_کلمه(بینښه idx) ثابت override;
      فراغ سېټ_کلمه(بینښه idx, ثابت vvp_vector4_t&that) override;

    خاص:
      v4cell* array_;
};

/*
 * Automatically allocated vvp_vector4array_t
 */
class vvp_vector4array_aa : عام vvp_vector4array_t, عام automatic_hooks_s {

    عام:
      vvp_vector4array_aa(بینښه پلنوالی, بینښه ټکي);
      ~vvp_vector4array_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#که_تعریف_شوی CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#د_که_پای

      vvp_vector4_t get_کلمه(بینښه idx) ثابت override;
      فراغ سېټ_کلمه(بینښه idx, ثابت vvp_vector4_t&that) override;

    خاص:
      بینښه context_idx_;
};

/* vvp_vector2_t
 */
class vvp_vector2_t {

      ملګری vvp_vector2_t چلوونکی - (ثابت vvp_vector2_t&);
      ملګری vvp_vector2_t چلوونکی + (ثابت vvp_vector2_t&,
				       ثابت vvp_vector2_t&);
      ملګری vvp_vector2_t چلوونکی * (ثابت vvp_vector2_t&,
				       ثابت vvp_vector2_t&);
      ملګری bool چلوونکی >  (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
      ملګری bool چلوونکی >= (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
      ملګری bool چلوونکی <  (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
      ملګری bool چلوونکی <= (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
      ملګری bool چلوونکی == (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);

    عام:
      vvp_vector2_t();
      vvp_vector2_t(ثابت vvp_vector2_t&);
      vvp_vector2_t(ثابت vvp_vector2_t&, بینښه newsize);
      vvp_vector2_t(ثابت vvp_vector2_t&, بینښه بنسټ, بینښه wid);
	// Make a vvp_vector2_t from a vvp_vector4_t. If enable_NaN
	// is true and there are X or Z bits, then the result becomes
	// a NaN value. If enable_NaN is false then X or Z bits are
	// converted to 0 as per the standard Verilog rules.
      explicit vvp_vector2_t(ثابت vvp_vector4_t&that, bool allow_NaN = false);
	// Make from a 64-bit value and a specified width.
      vvp_vector2_t(uint64_t val, بینښه wid);
	// Make with the width, and filled with 1 or 0 bits.
      تعداد fill_t {FILL0, FILL1};
      vvp_vector2_t(fill_t fill, بینښه wid);
      ~vvp_vector2_t();

      vvp_vector2_t&چلوونکی += (ثابت vvp_vector2_t&that);
      vvp_vector2_t&چلوونکی -= (ثابت vvp_vector2_t&that);
      vvp_vector2_t&چلوونکی <<= (بینښه ښويدنه);
      vvp_vector2_t&چلوونکی >>= (بینښه ښويدنه);
      vvp_vector2_t&چلوونکی = (ثابت vvp_vector2_t&);

	// Assign a vec4 to a vec2, using Verilog rules for converting
	// XZ values to 0.
      vvp_vector2_t&چلوونکی = (ثابت vvp_vector4_t&);

      bool is_NaN() ثابت;
      bool is_zero() ثابت;
      بینښه اندازه() ثابت;
      صحيح ارزښت(بینښه idx) ثابت;
      vvp_bit4_t value4(بینښه idx) ثابت;
	// Get the vector2 subvector starting at the address
      vvp_vector2_t subvalue(بینښه idx, بینښه اندازه) ثابت;
      فراغ سېټ_بټ(بینښه idx, صحيح بټ);
      فراغ سېټ_vec(بینښه idx, ثابت vvp_vector2_t&that);
	// Make the size just big enough to hold the first 1 bit.
      فراغ trim();
	// Trim off extra 1 bit since this is representing a negative value.
	// Always keep at least 32 bits.
      فراغ trim_neg();

    خاص:
      تعداد { بټونه_فی_کلمه = 8 * حجم(بینښه اوږد) };
      بینښه اوږد*vec_;
      بینښه wid_;

    خاص:
      فراغ copy_from_that_(ثابت vvp_vector2_t&that);
      فراغ copy_from_that_(ثابت vvp_vector4_t&that);
};

خارجي bool چلوونکی >  (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
خارجي bool چلوونکی >= (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
خارجي bool چلوونکی <  (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
خارجي bool چلوونکی <= (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
خارجي bool چلوونکی == (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
خارجي vvp_vector2_t چلوونکی - (ثابت vvp_vector2_t&);
خارجي vvp_vector2_t چلوونکی + (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
خارجي vvp_vector2_t چلوونکی * (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
خارجي vvp_vector2_t چلوونکی / (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);
خارجي vvp_vector2_t چلوونکی % (ثابت vvp_vector2_t&, ثابت vvp_vector2_t&);

vvp_vector2_t توان(ثابت vvp_vector2_t&, vvp_vector2_t&);
خارجي vvp_vector4_t vector2_منزل_vector4(ثابت vvp_vector2_t&, بینښه wid);

/* A c4string is of the form C4<...> where ... are bits. */
خارجي bool c4string_test(ثابت حرف*str);
خارجي vvp_vector4_t c4string_منزل_vector4(ثابت حرف*str);

/* A crstring is of the form Cr<...> where ... defines are real. */
خارجي bool crstring_test(ثابت حرف*str);
خارجي مزدوج crstring_منزل_double(ثابت حرف*str);

خارجي std::ostream& چلوونکی<< (std::ostream&, ثابت vvp_vector2_t&);

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
مضمن بینښه vvp_vector2_t::اندازه() ثابت
{
      أرجع wid_;
}

مضمن vvp_bit4_t vvp_vector2_t::value4(بینښه idx) ثابت
{
      إذا (ارزښت(idx))
	    أرجع BIT4_1;
      وإلا
	    أرجع BIT4_0;
}

مضمن vvp_vector2_t vvp_vector2_t::subvalue(بینښه بنسټ, بینښه wid) ثابت
{
      أرجع vvp_vector2_t(*this, بنسټ, wid);
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

      ملګری vvp_scalar_t fully_featured_resolv_(vvp_scalar_t a, vvp_scalar_t b);

    عام:
	// Make a HiZ value.
      explicit vvp_scalar_t();

	// Make an unambiguous value.
      explicit vvp_scalar_t(vvp_bit4_t val, بینښه str0, بینښه str1);

	// Get the vvp_bit4_t version of the value
      vvp_bit4_t ارزښت() ثابت;
      بینښه strength0() ثابت;
      بینښه strength1() ثابت;

      bool eeq(vvp_scalar_t that) ثابت { أرجع ارزښت_2 == that.ارزښت_2; }
      bool is_hiz() ثابت { أرجع (ارزښت_2 & 0x77) == 0; }

    خاص:
	// This class and the vvp_vector8_t class are closely related,
	// so allow vvp_vector8_t access to the raw encoding so that
	// it can do compact vectoring of vvp_scalar_t objects.
      ملګری class vvp_vector8_t;
      explicit vvp_scalar_t(بینښه حرف val) : ارزښت_2(val) { }
      بینښه حرف raw() ثابت { أرجع ارزښت_2; }

    خاص:
      بینښه حرف ارزښت_2;
};

مضمن vvp_scalar_t::vvp_scalar_t()
{
      ارزښت_2 = 0;
}

مضمن vvp_scalar_t::vvp_scalar_t(vvp_bit4_t val, بینښه str0, بینښه str1)
{
      أكد(str0 <= 7);
      أكد(str1 <= 7);

      إذا (str0 == 0 && str1 == 0) {
	    ارزښت_2 = 0x00;
      } وإلا بدل (val) {
	  حالة BIT4_0:
	    ارزښت_2 = str0 | (str0<<4);
	    اكسر;
	  حالة BIT4_1:
	    ارزښت_2 = str1 | (str1<<4) | 0x88;
	    اكسر;
	  حالة BIT4_X:
	    ارزښت_2 = str0 | (str1<<4) | 0x80;
	    اكسر;
	  حالة BIT4_Z:
	    ارزښت_2 = 0x00;
	    اكسر;
      }
}

مضمن vvp_bit4_t vvp_scalar_t::ارزښت() ثابت
{
      إذا ((ارزښت_2 & 0x77) == 0) {
	    أرجع BIT4_Z;
      } وإلا بدل (ارزښت_2 & 0x88) {
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

خارجي std::ostream& چلوونکی<< (std::ostream&, vvp_scalar_t);

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

      ملګری vvp_vector8_t part_expand(ثابت vvp_vector8_t&, بینښه, بینښه);

    عام:
      explicit vvp_vector8_t(بینښه اندازه =0);
	// Make a vvp_vector8_t from a vector4 and a specified
	// strength.
      explicit vvp_vector8_t(ثابت vvp_vector4_t&that,
			     بینښه str0,
			     بینښه str1);
      explicit vvp_vector8_t(ثابت vvp_vector2_t&that,
			     بینښه str0,
			     بینښه str1);

      ~vvp_vector8_t();

      ساكن ثابت vvp_vector8_t nil;

    عام:

      بینښه اندازه() ثابت { أرجع اندازه_2; }
      vvp_scalar_t ارزښت(بینښه idx) ثابت;
      vvp_vector8_t subvalue(بینښه adr, بینښه پلنوالی) ثابت;
      فراغ سېټ_بټ(بینښه idx, vvp_scalar_t val);
      فراغ سېټ_vec(بینښه idx, ثابت vvp_vector8_t&that);

	// Test that the vectors are exactly equal
      bool eeq(ثابت vvp_vector8_t&that) ثابت;

      vvp_vector8_t(ثابت vvp_vector8_t&that);
      vvp_vector8_t& چلوونکی= (ثابت vvp_vector8_t&that);

    خاص:
      بینښه اندازه_2;
      اتحاد {
	    بینښه حرف*ptr_;
	    بینښه حرف val_[حجم(فراغ*)];
      };
};

  /* Resolve uses the default Verilog resolver algorithm to resolve
     two drive vectors to a single output. */
مضمن vvp_vector8_t resolve(ثابت vvp_vector8_t&a, ثابت vvp_vector8_t&b)
{
      أكد(a.اندازه() == b.اندازه());
      vvp_vector8_t بهر (a.اندازه());

      لكل (بینښه idx = 0 ;  idx < بهر.اندازه() ;  idx += 1) {
	    بهر.سېټ_بټ(idx, resolve(a.ارزښت(idx), b.ارزښت(idx)));
      }

      أرجع بهر;
}

  /* This lookup tabke implements the strength reduction implied by
     Verilog standard switch devices. The major dimension selects
     between non-resistive and resistive devices. */
خارجي بینښه vvp_switch_strength_map[2][8];

  /* The reduce4 function converts a vector8 to a vector4, losing
     strength information in the process. */
خارجي vvp_vector4_t reduce4(ثابت vvp_vector8_t&that);
خارجي vvp_vector8_t part_expand(ثابت vvp_vector8_t&a, بینښه wid, بینښه off);

  /* A c8string is of the form C8<...> where ... are bits. */
خارجي bool c8string_test(ثابت حرف*str);
خارجي vvp_vector8_t c8string_منزل_vector8(ثابت حرف*str);

  /* Print a vector8 value to a stream. */
خارجي std::ostream& چلوونکی<< (std::ostream&, ثابت vvp_vector8_t&);

مضمن vvp_vector8_t::vvp_vector8_t(بینښه اندازه_3)
: اندازه_2(اندازه_3)
{
      إذا (اندازه_2 <= حجم(val_)) {
	    memset(val_, 0, حجم(val_));
      } وإلا {
	    ptr_ = new بینښه حرف[اندازه_2];
	    memset(ptr_, 0, اندازه_2);
      }
}

مضمن vvp_vector8_t::~vvp_vector8_t()
{
      إذا (اندازه_2 > حجم(val_))
	    delete[]ptr_;
}

مضمن vvp_scalar_t vvp_vector8_t::ارزښت(بینښه idx) ثابت
{
      أكد(idx < اندازه_2);
      إذا (اندازه_2 <= حجم(val_))
	    أرجع vvp_scalar_t(val_[idx]);
      وإلا
	    أرجع vvp_scalar_t(ptr_[idx]);
}

مضمن فراغ vvp_vector8_t::سېټ_بټ(بینښه idx, vvp_scalar_t val)
{
      أكد(idx < اندازه_2);
      إذا (اندازه_2 <= حجم(val_))
	    val_[idx] = val.raw();
      وإلا
	    ptr_[idx] = val.raw();
}

  // Exactly-equal for vvp_vector8_t is common and should be as tight
  // as possible.
مضمن bool vvp_vector8_t::eeq(ثابت vvp_vector8_t&that) ثابت
{
      إذا (اندازه_2 != that.اندازه_2)
	    أرجع false;
      إذا (اندازه_2 == 0)
	    أرجع true;

      إذا (اندازه_2 <= حجم(val_))
	      // This is equivalent to memcmp(val_, that.val_, sizeof val_)==0
	    أرجع ptr_ == that.ptr_;
      وإلا
	    أرجع memcmp(ptr_, that.ptr_, اندازه_2) == 0;
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
کينډۍ <class T> class vvp_sub_اشاره_ګر_t {

    عام:
      vvp_sub_اشاره_ګر_t() : بټونه_2(0) { }

      vvp_sub_اشاره_ګر_t(T*ptr__, بینښه پورټ_2)
      {
	    بټونه_2 = reinterpret_cast<uintptr_t> (ptr__);
	    أكد( (بټونه_2  &  UINTPTR_C(3)) == 0 );
	    أكد( (پورټ_2 & ~UINTPTR_C(3)) == 0 );
	    بټونه_2 |= پورټ_2;
      }

      ~vvp_sub_اشاره_ګر_t() = افتراضي;

      T* ptr()
      { أرجع reinterpret_cast<T*> (بټونه_2 & ~UINTPTR_C(3)); }

      ثابت T* ptr() ثابت
      { أرجع reinterpret_cast<ثابت T*> (بټونه_2 & ~UINTPTR_C(3)); }

      بینښه  پورټ() ثابت { أرجع بټونه_2 & UINTPTR_C(3); }

      bool nil() ثابت { أرجع بټونه_2 == 0; }

      bool چلوونکی == (vvp_sub_اشاره_ګر_t that) ثابت { أرجع بټونه_2 == that.بټونه_2; }
      bool چلوونکی != (vvp_sub_اشاره_ګر_t that) ثابت { أرجع بټونه_2 != that.بټونه_2; }

    خاص:
      uintptr_t بټونه_2;
};

تعريفنوع vvp_sub_اشاره_ګر_t<vvp_net_t> vvp_net_ptr_t;
کينډۍ <class T> std::ostream& چلوونکی << (std::ostream&بهر, vvp_sub_اشاره_ګر_t<T> val)
{ بهر << val.ptr() << "[" << val.پورټ() << "]"; أرجع بهر; }

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

#که_تعریف_شوی CHECK_WITH_VALGRIND
      vvp_net_t *pool;
#د_که_پای
      vvp_net_ptr_t پورټ[4];
      vvp_net_fun_t*fun;
      vvp_net_fil_t*fil;

    عام:
	// Connect the port to the output from this net.
      فراغ link(vvp_net_ptr_t پورټ);
	// Disconnect the port from the output of this net.
      فراغ unlink(vvp_net_ptr_t پورټ);

    عام: // Methods to propagate output from this node.
      فراغ send_vec4(ثابت vvp_vector4_t&val, vvp_context_t context);
      فراغ send_vec8(ثابت vvp_vector8_t&val);
      فراغ send_real(مزدوج val, vvp_context_t context);
      فراغ send_string(ثابت std::string&val, vvp_context_t context);
      فراغ send_object(vvp_object_t val, vvp_context_t context);

      فراغ send_vec4_pv(ثابت vvp_vector4_t&val,
			بینښه بنسټ, بینښه vwid, vvp_context_t context);
      فراغ send_vec8_pv(ثابت vvp_vector8_t&val,
			بینښه بنسټ, بینښه vwid);


    عام: // Methods to arrange for the output of this net to be forced.

	// The intent is that all efforts at force are directed to
	// operate only on the vvp_net_t whose output is to be
	// forced. These methods then communicate the force to the
	// attached filter to set up the actual force.
      فراغ force_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&ماسک);
      فراغ force_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&ماسک);
      فراغ force_real(مزدوج val, ثابت vvp_vector2_t&ماسک);

    عام: // Method to support $countdrivers
      فراغ شمېر_drivers(بینښه idx, بینښه counts[4]);

    // This needs to be public so that SDF interconnects can be inserted
    عام:
      vvp_net_ptr_t بهر_2;

    عام: // Need a better new for these objects.
      ساكن فراغ* چلوونکی new(std::size_t اندازه);
      ساكن فراغ چلوونکی delete(فراغ*); // not implemented
    خاص: // not implemented
      ساكن فراغ* چلوونکی new[](std::size_t اندازه);
#إذا defined(__GNUC__)
      ساكن فراغ چلوونکی delete[](فراغ*);
#د_که_پای
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
      مجازي ~vvp_net_fun_t();

      مجازي فراغ recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                             vvp_context_t context);
      مجازي فراغ recv_vec8(vvp_net_ptr_t پورټ, ثابت vvp_vector8_t&بټ);
      مجازي فراغ recv_real(vvp_net_ptr_t پورټ, مزدوج بټ,
                             vvp_context_t context);
      مجازي فراغ recv_string(vvp_net_ptr_t پورټ, ثابت std::string&بټ,
			       vvp_context_t context);
      مجازي فراغ recv_object(vvp_net_ptr_t پورټ, vvp_object_t بټ,
			       vvp_context_t context);

	// Part select variants of above
      مجازي فراغ recv_vec4_pv(vvp_net_ptr_t p, ثابت vvp_vector4_t&بټ,
				بینښه بنسټ, بینښه vwid, vvp_context_t context);
      مجازي فراغ recv_vec8_pv(vvp_net_ptr_t p, ثابت vvp_vector8_t&بټ,
				بینښه بنسټ, بینښه vwid);

	// This method is called when the net is forced or
	// released. This is very rarely needed; island ports use it
	// to know that the net is being forced and that it needs to
	// do something about it.
      مجازي فراغ force_flag(bool پرلپسې_now);

   محمي:
      فراغ recv_vec4_pv_(vvp_net_ptr_t p, ثابت vvp_vector4_t&بټ,
			 بینښه بنسټ, بینښه vwid, vvp_context_t context);
      فراغ recv_vec8_pv_(vvp_net_ptr_t p, ثابت vvp_vector8_t&بټ,
			 بینښه بنسټ, بینښه vwid);

    عام: // These objects are only permallocated.
      ساكن فراغ* چلوونکی new(std::size_t اندازه) { أرجع ډېرۍ.alloc(اندازه); }
      ساكن فراغ چلوونکی delete(فراغ*); // not implemented

      ساكن std::size_t ډېرۍ_ټول() { أرجع ډېرۍ.ډېرۍ_ټول(); }

    محمي:
      ساكن permaheap ډېرۍ;

    خاص: // not implemented
      vvp_net_fun_t(ثابت vvp_net_fun_t&);
      vvp_net_fun_t& چلوونکی= (ثابت vvp_net_fun_t&);
      ساكن فراغ* چلوونکی new[](std::size_t اندازه);
      ساكن فراغ چلوونکی delete[](فراغ*);
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
      مجازي ~vvp_net_fil_t() override;

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
      مجازي prop_t filter_vec4(ثابت vvp_vector4_t&بټ, vvp_vector4_t&rep,
				 بینښه بنسټ, بینښه vwid);
      مجازي prop_t filter_vec8(ثابت vvp_vector8_t&val, vvp_vector8_t&rep,
				 بینښه بنسټ, بینښه vwid);
      مجازي prop_t filter_real(مزدوج&val);
      مجازي prop_t filter_object(vvp_object_t&val);
      مجازي prop_t filter_string(ثابت std::string&val);

      مجازي فراغ release(vvp_net_ptr_t ptr, bool net_flag) =0;
      مجازي فراغ release_pv(vvp_net_ptr_t ptr, بینښه بنسټ, بینښه wid, bool net_flag) =0;

	// The %force/link instruction needs a place to write the
	// source node of the force, so that subsequent %force and
	// %release instructions can undo the link as needed. */
      فراغ force_link(vvp_net_t*dst, vvp_net_t*src);
      فراغ force_unlink(فراغ);

      مجازي بینښه filter_اندازه() ثابت =0;

    عام:
	// Support for force methods. These are called by the
	// vvp_net_t::force_* methods to set the force value and mask
	// for the filter.
      مجازي فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&ماسک) =0;
      مجازي فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&ماسک) =0;
      مجازي فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&ماسک) =0;

    عام: // These objects are only permallocated.
      ساكن فراغ* چلوونکی new(std::size_t اندازه) { أرجع ډېرۍ.alloc(اندازه); }
      ساكن فراغ چلوونکی delete(فراغ*); // not implemented

      ساكن size_t ډېرۍ_ټول() { أرجع ډېرۍ.ډېرۍ_ټول(); }

    خاص:
      ساكن permaheap ډېرۍ;

    خاص: // not implemented
      vvp_net_fil_t(ثابت vvp_net_fil_t&);
      vvp_net_fil_t& چلوونکی= (ثابت vvp_net_fil_t&);
      ساكن فراغ* چلوونکی new[](std::size_t اندازه);
      ساكن فراغ چلوونکی delete[](فراغ*);

    محمي:
	// Set bits of the filter force mask
      فراغ force_ماسک(ثابت vvp_vector2_t&ماسک);
	// Release the force on the bits set in the mask.
      فراغ release_ماسک(ثابت vvp_vector2_t&ماسک);
	// Test bits of the filter force mask;
      bool test_force_ماسک(بینښه بټ) ثابت;
      bool test_force_ماسک_is_zero() ثابت;

	// This template method is used by derived classes to process
	// the val through the force mask. The force value is the
	// currently forced value, and the buf is a value that this
	// method will use to hold a filtered value, if needed. This
	// method returns the replacement value into the "rep"
	// argument and returns a code indicating whether any changes
	// were made.
      کينډۍ <class T> prop_t filter_ماسک(ثابت T&val, ثابت T&force, T&rep, بینښه addr);
	// This template method is similar to the above, but works for
	// native types that are not so expensive to edit in place.
      کينډۍ <class T> prop_t filter_ماسک(T&val, T force);

	// These templates are similar to filter_mask_, but are
	// idempotent. Then do not trigger callbacks or otherwise
	// cause any local changes. These methods are used to test
	// arbitrary values against the force mask.
      کينډۍ <class T> prop_t filter_ننوت_ماسک(ثابت T&val, ثابت T&force, T&rep) ثابت;

    خاص:
	// Mask of forced bits
      vvp_vector2_t force_ماسک_2;
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

      فراغ recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
		     vvp_context_t context) override;
      فراغ recv_real(vvp_net_ptr_t پورټ, مزدوج بټ, vvp_context_t) override;
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
      explicit vvp_fun_drive(بینښه str0 =6, بینښه str1 =6);
      ~vvp_fun_drive() override;

      فراغ recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                     vvp_context_t context) override;

      فراغ recv_vec4_pv(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
			بینښه بنسټ, بینښه vwid, vvp_context_t) override;
    خاص:
      بینښه حرف drive0_;
      بینښه حرف drive1_;
};

/*
 * EXTEND functors expand an input vector to the desired output
 * width. The extend_signed functor sign extends the input. If the
 * input is already wider than the desired output, then it is passed
 * unmodified.
 */
class vvp_fun_extend_signed  : عام vvp_net_fun_t {

    عام:
      explicit vvp_fun_extend_signed(بینښه wid);
      ~vvp_fun_extend_signed() override;

      فراغ recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                     vvp_context_t context) override;

      فراغ recv_vec4_pv(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
			بینښه بنسټ, بینښه vwid, vvp_context_t) override;
    خاص:
      بینښه پلنوالی_2;
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
      vvp_wide_fun_core(vvp_net_t*net, بینښه nports);
      مجازي ~vvp_wide_fun_core() override;
	// These objects are not perm allocated.
      ساكن فراغ* چلوونکی new(std::size_t اندازه) { أرجع ::new حرف[اندازه]; }
      ساكن فراغ چلوونکی delete(فراغ* ptr) { ::delete[](static_cast<حرف*>(ptr)); }

    محمي:
      فراغ propagate_vec4(ثابت vvp_vector4_t&بټ, vvp_time64_t delay =0);
      فراغ propagate_real(مزدوج بټ, vvp_time64_t delay =0);
      بینښه پورټ_شمېر() ثابت;

      vvp_vector4_t& ارزښت(بینښه);
      مزدوج ارزښت_r(بینښه);

    خاص:
	// the derived class implements this to receive an indication
	// that one of the port input values changed.
      مجازي فراغ recv_vec4_from_inputs(بینښه پورټ) =0;
      مجازي فراغ recv_real_from_inputs(بینښه پورټ);

      ملګری class vvp_wide_fun_t;
      فراغ dispatch_vec4_from_ننوت(بینښه پورټ, ثابت vvp_vector4_t&بټ);
      فراغ dispatch_real_from_ننوت(بینښه پورټ, مزدوج بټ);

    خاص:
	// Back-point to the vvp_net_t that points to me.
      vvp_net_t*ptr_;
	// Structure to track the input values from the input functors.
      بینښه nports_;
      vvp_vector4_t*پورټ_ارزښتونه;
      مزدوج*پورټ_rvalues;
};

/*
 * The job of the input functor is only to monitor inputs to the
 * function and pass them to the ufunc_core object. Each functor takes
 * up to 4 inputs, with the base the port number for the first
 * function input that this functor represents.
 */
class vvp_wide_fun_t : عام vvp_net_fun_t {

    عام:
      vvp_wide_fun_t(vvp_wide_fun_core*c, بینښه بنسټ);
      ~vvp_wide_fun_t() override;

      فراغ recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                     vvp_context_t context) override;
      فراغ recv_real(vvp_net_ptr_t پورټ, مزدوج بټ,
                     vvp_context_t context) override;

      فراغ recv_vec4_pv(vvp_net_ptr_t p, ثابت vvp_vector4_t&بټ,
			بینښه بنسټ, بینښه vwid, vvp_context_t context) override;

    خاص:
      vvp_wide_fun_core*core_;
      بینښه پورټ_بنسټ_2;
};


مضمن فراغ vvp_send_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&val, vvp_context_t context)
{
      طالما (class vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t بل_val = cur->پورټ[ptr.پورټ()];

	    إذا (cur->fun)
		  cur->fun->recv_vec4(ptr, val, context);

	    ptr = بل_val;
      }
}

خارجي فراغ vvp_send_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&val);
خارجي فراغ vvp_send_real(vvp_net_ptr_t ptr, مزدوج val,
                          vvp_context_t context);

مضمن فراغ vvp_send_string(vvp_net_ptr_t ptr, ثابت std::string&val, vvp_context_t context)
{
      طالما (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t بل_val = cur->پورټ[ptr.پورټ()];

	    إذا (cur->fun)
		  cur->fun->recv_string(ptr, val, context);

	    ptr = بل_val;
      }
}

مضمن فراغ vvp_send_object(vvp_net_ptr_t ptr, vvp_object_t val, vvp_context_t context)
{
      طالما (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t بل_val = cur->پورټ[ptr.پورټ()];

	    إذا (cur->fun)
		  cur->fun->recv_object(ptr, val, context);

	    ptr = بل_val;
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
			     بینښه بنسټ, بینښه vwid,
			     vvp_context_t context)
{
      طالما (class vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t بل_val = cur->پورټ[ptr.پورټ()];

	    إذا (cur->fun)
		  cur->fun->recv_vec4_pv(ptr, val, بنسټ, vwid, context);

	    ptr = بل_val;
      }
}

مضمن فراغ vvp_send_vec8_pv(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&val,
			     بینښه بنسټ, بینښه vwid)
{
      طالما (class vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t بل_val = cur->پورټ[ptr.پورټ()];

	    إذا (cur->fun)
		  cur->fun->recv_vec8_pv(ptr, val, بنسټ, vwid);

	    ptr = بل_val;
      }
}

مضمن فراغ vvp_net_t::send_vec4(ثابت vvp_vector4_t&val, vvp_context_t context)
{
      إذا (fil == 0) {
	    vvp_send_vec4(بهر_2, val, context);
	    أرجع;
      }

      vvp_vector4_t rep;
      بدل (fil->filter_vec4(val, rep, 0, val.اندازه())) {
	  حالة vvp_net_fil_t::STOP:
	    اكسر;
	  حالة vvp_net_fil_t::PROP:
	    vvp_send_vec4(بهر_2, val, context);
	    اكسر;
	  حالة vvp_net_fil_t::REPL:
	    vvp_send_vec4(بهر_2, rep, context);
	    اكسر;
      }
}

مضمن فراغ vvp_net_t::send_vec4_pv(ثابت vvp_vector4_t&val,
				    بینښه بنسټ, بینښه vwid,
				    vvp_context_t context)
{
      إذا (fil == 0) {
	    vvp_send_vec4_pv(بهر_2, val, بنسټ, vwid, context);
	    أرجع;
      }

      vvp_vector4_t rep;
      بدل (fil->filter_vec4(val, rep, بنسټ, vwid)) {
	  حالة vvp_net_fil_t::STOP:
	    اكسر;
	  حالة vvp_net_fil_t::PROP:
	    vvp_send_vec4_pv(بهر_2, val, بنسټ, vwid, context);
	    اكسر;
	  حالة vvp_net_fil_t::REPL:
	    vvp_send_vec4_pv(بهر_2, rep, بنسټ, vwid, context);
	    اكسر;
      }
}

مضمن فراغ vvp_net_t::send_vec8(ثابت vvp_vector8_t&val)
{
      إذا (fil == 0) {
	    vvp_send_vec8(بهر_2, val);
	    أرجع;
      }

      vvp_vector8_t rep;
      بدل (fil->filter_vec8(val, rep, 0, val.اندازه())) {
	  حالة vvp_net_fil_t::STOP:
	    اكسر;
	  حالة vvp_net_fil_t::PROP:
	    vvp_send_vec8(بهر_2, val);
	    اكسر;
	  حالة vvp_net_fil_t::REPL:
	    vvp_send_vec8(بهر_2, rep);
	    اكسر;
      }
}

مضمن فراغ vvp_net_t::send_vec8_pv(ثابت vvp_vector8_t&val,
				    بینښه بنسټ, بینښه vwid)
{
      إذا (fil == 0) {
	    vvp_send_vec8_pv(بهر_2, val, بنسټ, vwid);
	    أرجع;
      }

      vvp_vector8_t rep;
      بدل (fil->filter_vec8(val, rep, بنسټ, vwid)) {
	  حالة vvp_net_fil_t::STOP:
	    اكسر;
	  حالة vvp_net_fil_t::PROP:
	    vvp_send_vec8_pv(بهر_2, val, بنسټ, vwid);
	    اكسر;
	  حالة vvp_net_fil_t::REPL:
	    vvp_send_vec8_pv(بهر_2, rep, بنسټ, vwid);
	    اكسر;
      }
}

مضمن فراغ vvp_net_t::send_real(مزدوج val, vvp_context_t context)
{
      إذا (fil && ! fil->filter_real(val))
	    أرجع;

      vvp_send_real(بهر_2, val, context);
}


مضمن فراغ vvp_net_t::send_string(ثابت std::string&val, vvp_context_t context)
{
      إذا (fil && !fil->filter_string(val))
	    أرجع;

      vvp_send_string(بهر_2, val, context);
}


مضمن فراغ vvp_net_t::send_object(vvp_object_t val, vvp_context_t context)
{
      إذا (fil && ! fil->filter_object(val))
	    أرجع;

      vvp_send_object(بهر_2, val, context);
}


مضمن bool vvp_net_fil_t::test_force_ماسک(بینښه بټ) ثابت
{
      إذا (بټ >= force_ماسک_2.اندازه())
	    أرجع false;
      إذا (force_ماسک_2.ارزښت(بټ))
	    أرجع true;
      وإلا
	    أرجع false;
}

مضمن bool vvp_net_fil_t::test_force_ماسک_is_zero(فراغ) ثابت
{
      إذا (force_ماسک_2.اندازه() == 0)
	    أرجع true;
      إذا (force_ماسک_2.is_zero())
	    أرجع true;
      أرجع false;
}

/*
 * Undefine the ivl_alloc.h definitions so they don't leak out of this file.
 */
#تعریف_لغوه حافظه_ورکړه
#تعریف_لغوه realloc
#تعریف_لغوه حافظه_صفر_ورکړه
#تعریف_لغوه __ivl_alloc_H

#د_که_پای /* IVL_vvp_net_H */
