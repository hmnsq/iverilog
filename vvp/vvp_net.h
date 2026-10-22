#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_vvp_net_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_vvp_net_H
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_IOSFWD
# include  <iosfwd>
#ಇಲ್ಲದಿದ್ದರೆ
# include  <iostream>
#ಆದರೆ_ಅಂತ್ಯ

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ**vvp_context_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ*vvp_context_ಅಂಶ_t;

ಸಾಲೊಳಗೆ vvp_context_t vvp_allocate_context(ಚಿಹ್ನೆರಹಿತ nitem)
{
      ಹಿಂತಿರುಗಿ (vvp_context_t)ಸ್ಮರಣೆ_ಕೊಡು((2 + nitem) * ಗಾತ್ರ(ಶೂನ್ಯ*));
}

ಸಾಲೊಳಗೆ vvp_context_t vvp_get_ಮುಂದಿನ_context(vvp_context_t context)
{
      ಹಿಂತಿರುಗಿ (vvp_context_t)context[0];
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ vvp_ಗಣ_ಮುಂದಿನ_context(vvp_context_t context, vvp_context_t ಮುಂದಿನ_2)
{
      context[0] = ಮುಂದಿನ_2;
}

ಸಾಲೊಳಗೆ vvp_context_t vvp_get_stacked_context(vvp_context_t context)
{
      ಹಿಂತಿರುಗಿ (vvp_context_t)context[1];
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ vvp_ಗಣ_stacked_context(vvp_context_t context, vvp_context_t ರಾಶಿ_2)
{
      context[1] = ರಾಶಿ_2;
}

ಸಾಲೊಳಗೆ vvp_context_ಅಂಶ_t vvp_get_context_ಅಂಶ(vvp_context_t context,
                                               ಚಿಹ್ನೆರಹಿತ ಅಂಶ_idx)
{
      ಹಿಂತಿರುಗಿ (vvp_context_ಅಂಶ_t)context[ಅಂಶ_idx];
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ vvp_ಗಣ_context_ಅಂಶ(vvp_context_t context, ಚಿಹ್ನೆರಹಿತ ಅಂಶ_idx,
                                 vvp_context_ಅಂಶ_t ಅಂಶ)
{
      context[ಅಂಶ_idx] = ಅಂಶ;
}

/*
 * An "automatic" functor is one which may be associated with an automatically
 * allocated scope item. This provides the infrastructure needed to allocate
 * the state information for individual instances of the item.
 */
ರಚನೆ automatic_hooks_s {

      automatic_hooks_s() {}
      ವಾಸ್ತವಿಕ ~automatic_hooks_s() {}

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ alloc_instance(vvp_context_t context) = 0;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ reset_instance(vvp_context_t context) = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ free_instance(vvp_context_t context) = 0;
#ಆದರೆ_ಅಂತ್ಯ
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
ಎಣಿಕೆ vvp_bit4_t {
      BIT4_0 = 0,
      BIT4_1 = 1,
      BIT4_X = 3,
      BIT4_Z = 2
};

  /* Return an ASCII character that represents the vvp_bit4_t value. */
ಸಾಲೊಳಗೆ ಅಕ್ಷರ vvp_bit4_ಗಮ್ಯ_ascii(vvp_bit4_t a) { ಹಿಂತಿರುಗಿ "01zx"[a]; }

ಬಾಹ್ಯ vvp_bit4_t ಕೂಡಿಸು_with_ವಾಹಕ(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c);

ಬಾಹ್ಯ vvp_bit4_t scalar_ಗಮ್ಯ_bit4(PLI_INT32 scalar);

  /* Return TRUE if the bit is BIT4_X or BIT4_Z. The fast
     implementation here relies on the encoding of vvp_bit4_t values. */
ಸಾಲೊಳಗೆ bool bit4_is_xz(vvp_bit4_t a) { ಹಿಂತಿರುಗಿ a >= 2; }

  /* This function converts BIT4_Z to BIT4_X, but passes other values
     unchanged. This fast implementation relies of the encoding of the
     vvp_bit4_t values. In particular, the BIT4_X==3 and BIT4_Z==2 */
ಸಾಲೊಳಗೆ vvp_bit4_t bit4_z2x(vvp_bit4_t a)
{ ಹಿಂತಿರುಗಿ (vvp_bit4_t) ( (ಪೂರ್ಣಾಂಕ)a | ((ಪೂರ್ಣಾಂಕ)a >> 1) ); }

  /* Some common boolean operators. These implement the Verilog rules
     for 4-value bit operations. The fast implementations here rely
     on the encoding of vvp_bit4_t values. */

  // ~BIT4_0 --> BIT4_1
  // ~BIT4_1 --> BIT4_0
  // ~BIT4_X --> BIT4_X
  // ~BIT4_Z --> BIT4_X
ಸಾಲೊಳಗೆ vvp_bit4_t ಕ್ರಿಯಾಕಾರಕ ~ (vvp_bit4_t a)
{ ಹಿಂತಿರುಗಿ bit4_z2x((vvp_bit4_t) (((ಪೂರ್ಣಾಂಕ)a) ^ 1)); }

ಸಾಲೊಳಗೆ vvp_bit4_t ಕ್ರಿಯಾಕಾರಕ | (vvp_bit4_t a, vvp_bit4_t b)
{
      ಆದರೆ (a==BIT4_1 || b==BIT4_1)
	    ಹಿಂತಿರುಗಿ BIT4_1;
      ಹಿಂತಿರುಗಿ bit4_z2x( (vvp_bit4_t) ((ಪೂರ್ಣಾಂಕ)a | (ಪೂರ್ಣಾಂಕ)b) );
}

ಸಾಲೊಳಗೆ vvp_bit4_t ಕ್ರಿಯಾಕಾರಕ & (vvp_bit4_t a, vvp_bit4_t b)
{
      ಆದರೆ (a==BIT4_0 || b==BIT4_0)
	    ಹಿಂತಿರುಗಿ BIT4_0;
      ಹಿಂತಿರುಗಿ bit4_z2x( (vvp_bit4_t) ((ಪೂರ್ಣಾಂಕ)a | (ಪೂರ್ಣಾಂಕ)b) );
}


ಬಾಹ್ಯ vvp_bit4_t ಕ್ರಿಯಾಕಾರಕ ^ (vvp_bit4_t a, vvp_bit4_t b);
ಬಾಹ್ಯ std::ostream& ಕ್ರಿಯಾಕಾರಕ<< (std::ostream&o, vvp_bit4_t a);

  /* Return >0, ==0 or <0 if the from-to transition represents a
     posedge, no edge, or negedge. */
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ ಅಂಚು(vvp_bit4_t from, vvp_bit4_t ಗಮ್ಯ);

  /* Support for $countdrivers. */
ಸಾಲೊಳಗೆ ಶೂನ್ಯ update_driver_counts(vvp_bit4_t ಬಿಟ್, ಚಿಹ್ನೆರಹಿತ counts[3])
{
      ಆಯ್ಕೆ (ಬಿಟ್) {
          ಸ್ಥಿತಿ BIT4_0:
            counts[0] += 1;
            ನಿಲ್ಲಿಸು;
          ಸ್ಥಿತಿ BIT4_1:
            counts[1] += 1;
            ನಿಲ್ಲಿಸು;
          ಸ್ಥಿತಿ BIT4_X:
            counts[2] += 1;
            ನಿಲ್ಲಿಸು;
          ಪೂರ್ವನಿಯೋಜಿತ:
            ನಿಲ್ಲಿಸು;
      }
}

/*
 * Some of the instructions do wide addition to arrays of long. They
 * use this add_with_carry function to help.
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಕೂಡಿಸು_with_ವಾಹಕ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ a, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ b,
					   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ&ವಾಹಕ)
{
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp = b + ವಾಹಕ;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮೊತ್ತ = a + tmp;
      ವಾಹಕ = 0;
      ಆದರೆ (tmp < b)
	    ವಾಹಕ = 1;
      ಆದರೆ (ಮೊತ್ತ < tmp)
	    ವಾಹಕ = 1;
      ಆದರೆ (ಮೊತ್ತ < a)
	    ವಾಹಕ = 1;
      ಹಿಂತಿರುಗಿ ಮೊತ್ತ;
}

ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ multiply_with_ವಾಹಕ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ a, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ b,
					 ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ&ವಾಹಕ);

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

      ಮಿತ್ರ vvp_vector4_t ಕ್ರಿಯಾಕಾರಕ ~(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that);
      ಮಿತ್ರ class vvp_vector4array_t;
      ಮಿತ್ರ class vvp_vector4array_sa;
      ಮಿತ್ರ class vvp_vector4array_aa;

    ಸಾರ್ವಜನಿಕ:
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ vvp_vector4_t nil;

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_vector4_t(ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2 =0, vvp_bit4_t ಬಿಟ್ಟುಗಳು =BIT4_X);

      explicit vvp_vector4_t(ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2, ದ್ವಿಗುಣ val);

	// Construct a vector4 from the subvalue of another
	// vector4. The width of the result is 'wid', and the bits are
	// pulled from 'that' to implement the Verilog part select
	// semantics. This means that part select beyond 'that'
	// returns X bits.
      explicit vvp_vector4_t(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that,
			     ಚಿಹ್ನೆರಹಿತ adr, ಚಿಹ್ನೆರಹಿತ wid);

      vvp_vector4_t(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that);
      vvp_vector4_t(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that, bool invert_flag);
      vvp_vector4_t& ಕ್ರಿಯಾಕಾರಕ= (ಸ್ಥಿರಾಂಕ vvp_vector4_t&that);

      ~vvp_vector4_t();

      ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಗಾತ್ರ_3; }
      ಶೂನ್ಯ resize(ಚಿಹ್ನೆರಹಿತ new_ಗಾತ್ರ, vvp_bit4_t pad_ಬಿಟ್ = BIT4_X);

	// Get the bit at the specified address
      vvp_bit4_t ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ;
	// Get the vector4 subvector starting at the address
      vvp_vector4_t subvalue(ಚಿಹ್ನೆರಹಿತ idx, ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2) ಸ್ಥಿರಾಂಕ;
	// Get the 2-value bits for the subvector. This returns a new
	// array of longs, or a nil pointer if an XZ bit was detected
	// in the array.
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*subarray(ಚಿಹ್ನೆರಹಿತ idx, ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2, bool xz_ಗಮ್ಯ_0 =false) ಸ್ಥಿರಾಂಕ;
      ಶೂನ್ಯ setarray(ಚಿಹ್ನೆರಹಿತ idx, ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*val);

	// Set a 4-value bit or subvector into the vector. Return true
	// if any bits of the vector change as a result of this operation.
      ಶೂನ್ಯ ಗಣ_ಬಿಟ್(ಚಿಹ್ನೆರಹಿತ idx, vvp_bit4_t val);
      bool ಗಣ_vec(ಚಿಹ್ನೆರಹಿತ idx, ಸ್ಥಿರಾಂಕ vvp_vector4_t&that);

        // Get the bits from another vector, but keep my size.
      ಶೂನ್ಯ copy_ಬಿಟ್ಟುಗಳು(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that);

	// Move bits within this vector.
      ಶೂನ್ಯ mov(ಚಿಹ್ನೆರಹಿತ dst, ಚಿಹ್ನೆರಹಿತ src, ಚಿಹ್ನೆರಹಿತ cnt);

	// Add that to this in the Verilog way.
      ಶೂನ್ಯ ಕೂಡಿಸು(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that);

	// Subtract that from this in the Verilog way.
      ಶೂನ್ಯ sub(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that);

	// Multiply this by that in the Verilog way.
      ಶೂನ್ಯ mul(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that);

	// Test that the vectors are exactly equal
      bool eeq(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that) ಸ್ಥಿರಾಂಕ;

	// Test that the vectors are equal, with xz comparing as equal.
      bool eq_xz(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that) ಸ್ಥಿರಾಂಕ;

	// Return true if there is an X or Z anywhere in the vector.
      bool has_xz() ಸ್ಥಿರಾಂಕ;

	// Change all Z bits to X bits.
      ಶೂನ್ಯ change_z2x();

	// Set all bits to the specified value.
      ಶೂನ್ಯ fill_ಬಿಟ್ಟುಗಳು(vvp_bit4_t ಬಿಟ್);

	// Change all bits to X bits.
      ಶೂನ್ಯ ಗಣ_ಗಮ್ಯ_x() { fill_ಬಿಟ್ಟುಗಳು(BIT4_X); }

	// Display the value into the buf as a string.
      ಅಕ್ಷರ*as_string(ಅಕ್ಷರ*buf, size_t buf_len) ಸ್ಥಿರಾಂಕ;

      ಶೂನ್ಯ invert();
      vvp_vector4_t& ಕ್ರಿಯಾಕಾರಕ &= (ಸ್ಥಿರಾಂಕ vvp_vector4_t&that);
      vvp_vector4_t& ಕ್ರಿಯಾಕಾರಕ |= (ಸ್ಥಿರಾಂಕ vvp_vector4_t&that);
      vvp_vector4_t& ಕ್ರಿಯಾಕಾರಕ ^= (ಸ್ಥಿರಾಂಕ vvp_vector4_t&that);
      vvp_vector4_t& ಕ್ರಿಯಾಕಾರಕ += (int64_t);

      vvp_bit4_t reduce_or() ಸ್ಥಿರಾಂಕ;
      vvp_bit4_t reduce_and() ಸ್ಥಿರಾಂಕ;
      vvp_bit4_t reduce_xor() ಸ್ಥಿರಾಂಕ;

    ಖಾಸಗಿ:
	// Number of vvp_bit4_t bits that can be shoved into a word.
      ಎಣಿಕೆ { ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ = 8*ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) };
	// The double value constructor requires that WORD_0_BBITS
	// and WORD_1_BBITS have the same value!
#ಆದರೆ SIZEOF_UNSIGNED_LONG == 8
      ಎಣಿಕೆ { ಶಬ್ದ_0_ABITS = 0x0000000000000000UL,
	     ಶಬ್ದ_0_BBITS = 0x0000000000000000UL };
      ಎಣಿಕೆ { ಶಬ್ದ_1_ABITS = 0xFFFFFFFFFFFFFFFFUL,
	     ಶಬ್ದ_1_BBITS = 0x0000000000000000UL };
      ಎಣಿಕೆ { ಶಬ್ದ_X_ABITS = 0xFFFFFFFFFFFFFFFFUL,
	     ಶಬ್ದ_X_BBITS = 0xFFFFFFFFFFFFFFFFUL };
      ಎಣಿಕೆ { ಶಬ್ದ_Z_ABITS = 0x0000000000000000UL,
             ಶಬ್ದ_Z_BBITS = 0xFFFFFFFFFFFFFFFFUL };
#ಅಥವಾಆದರೆ SIZEOF_UNSIGNED_LONG == 4
      ಎಣಿಕೆ { ಶಬ್ದ_0_ABITS = 0x00000000UL, ಶಬ್ದ_0_BBITS = 0x00000000UL };
      ಎಣಿಕೆ { ಶಬ್ದ_1_ABITS = 0xFFFFFFFFUL, ಶಬ್ದ_1_BBITS = 0x00000000UL };
      ಎಣಿಕೆ { ಶಬ್ದ_X_ABITS = 0xFFFFFFFFUL, ಶಬ್ದ_X_BBITS = 0xFFFFFFFFUL };
      ಎಣಿಕೆ { ಶಬ್ದ_Z_ABITS = 0x00000000UL, ಶಬ್ದ_Z_BBITS = 0xFFFFFFFFUL };
#ಇಲ್ಲದಿದ್ದರೆ
#ದೋಷ "WORD_X_xBITS not defined for this architecture?"
#ಆದರೆ_ಅಂತ್ಯ

	// Initialize and operator= use this private method to copy
	// the data from that object into this object.
      ಶೂನ್ಯ copy_from_(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that);
      ಶೂನ್ಯ copy_from_big_(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that);
      ಶೂನ್ಯ copy_inverted_from_(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that);

      ಶೂನ್ಯ allocate_ಪದಗಳು(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ inita, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ initb);

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

      ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_3;
      ಒಕ್ಕೂಟ {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ abits_val_;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*abits_ptr_;
      };
      ಒಕ್ಕೂಟ {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ bbits_val_;
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*bbits_ptr_;
      };
};

ಸಾಲೊಳಗೆ vvp_vector4_t::vvp_vector4_t(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that)
{
      copy_from_(that);
}

ಸಾಲೊಳಗೆ vvp_vector4_t::vvp_vector4_t(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that, bool invert_flag)
{
      ಆದರೆ (invert_flag)
	    copy_inverted_from_(that);
      ಇಲ್ಲದಿದ್ದರೆ
	    copy_from_(that);
}

ಸಾಲೊಳಗೆ vvp_vector4_t::vvp_vector4_t(ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_4, vvp_bit4_t val)
: ಗಾತ್ರ_3(ಗಾತ್ರ_4)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ init_atable[4] = {
	    ಶಬ್ದ_0_ABITS,
	    ಶಬ್ದ_1_ABITS,
	    ಶಬ್ದ_Z_ABITS,
	    ಶಬ್ದ_X_ABITS };
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ init_btable[4] = {
	    ಶಬ್ದ_0_BBITS,
	    ಶಬ್ದ_1_BBITS,
	    ಶಬ್ದ_Z_BBITS,
	    ಶಬ್ದ_X_BBITS };

      allocate_ಪದಗಳು(init_atable[val], init_btable[val]);
}

ಸಾಲೊಳಗೆ vvp_vector4_t::~vvp_vector4_t()
{
      ಆದರೆ (ಗಾತ್ರ_3 > ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    delete[] abits_ptr_;
	      // bbits_ptr_ actually points half-way into a
	      // double-length array started at abits_ptr_
      }
}

ಸಾಲೊಳಗೆ vvp_vector4_t& vvp_vector4_t::ಕ್ರಿಯಾಕಾರಕ= (ಸ್ಥಿರಾಂಕ vvp_vector4_t&that)
{
      ಆದರೆ (this == &that)
	    ಹಿಂತಿರುಗಿ *this;

      ಆದರೆ (ಗಾತ್ರ_3 > ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ)
	    delete[] abits_ptr_;

      copy_from_(that);

      ಹಿಂತಿರುಗಿ *this;
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ vvp_vector4_t::copy_from_(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that)
{
      ಗಾತ್ರ_3 = that.ಗಾತ್ರ_3;
      ಆದರೆ (ಗಾತ್ರ_3 <= ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    copy_from_big_(that);
      }
}

ಸಾಲೊಳಗೆ vvp_bit4_t vvp_vector4_t::ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ
{
      ಆದರೆ (idx >= ಗಾತ್ರ_3)
	    ಹಿಂತಿರುಗಿ BIT4_X;

      ಚಿಹ್ನೆರಹಿತ off;

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ abits, bbits;
      ಆದರೆ (ಗಾತ್ರ_3 > ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ wdx = idx / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    off = idx % ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    abits = abits_ptr_[wdx];
	    bbits = bbits_ptr_[wdx];
      } ಇಲ್ಲದಿದ್ದರೆ {
	    off = idx;
	    abits = abits_val_;
	    bbits = bbits_val_;
      }

      abits >>= off;
      bbits >>= off;
      ಪೂರ್ಣಾಂಕ tmp = ((bbits&1) << 1) + (abits&1);
	// This cast works since b==1,a==1 is X and b==1,a==0 is Z.
      ಹಿಂತಿರುಗಿ (vvp_bit4_t)tmp;
}

ಸಾಲೊಳಗೆ vvp_vector4_t vvp_vector4_t::subvalue(ಚಿಹ್ನೆರಹಿತ adr, ಚಿಹ್ನೆರಹಿತ wid) ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ vvp_vector4_t(*this, adr, wid);
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ vvp_vector4_t::ಗಣ_ಬಿಟ್(ಚಿಹ್ನೆರಹಿತ idx, vvp_bit4_t val)
{
      ಪ್ರತಿಜ್ಞಾ(idx < ಗಾತ್ರ_3);

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ off = idx % ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = 1UL << off;

      ಆದರೆ (ಗಾತ್ರ_3 > ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ) {
	    ಚಿಹ್ನೆರಹಿತ wdx = idx / ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ;
	    ಆಯ್ಕೆ (val) {
		ಸ್ಥಿತಿ BIT4_0:
		  abits_ptr_[wdx] &= ~ಮುಖವಾಡ;
		  bbits_ptr_[wdx] &= ~ಮುಖವಾಡ;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_1:
		  abits_ptr_[wdx] |=  ಮುಖವಾಡ;
		  bbits_ptr_[wdx] &= ~ಮುಖವಾಡ;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_X:
		  abits_ptr_[wdx] |=  ಮುಖವಾಡ;
		  bbits_ptr_[wdx] |=  ಮುಖವಾಡ;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_Z:
		  abits_ptr_[wdx] &= ~ಮುಖವಾಡ;
		  bbits_ptr_[wdx] |=  ಮುಖವಾಡ;
		  ನಿಲ್ಲಿಸು;
	    }
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಆಯ್ಕೆ (val) {
		ಸ್ಥಿತಿ BIT4_0:
		  abits_val_ &= ~ಮುಖವಾಡ;
		  bbits_val_ &= ~ಮುಖವಾಡ;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_1:
		  abits_val_ |=  ಮುಖವಾಡ;
		  bbits_val_ &= ~ಮುಖವಾಡ;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_X:
		  abits_val_ |=  ಮುಖವಾಡ;
		  bbits_val_ |=  ಮುಖವಾಡ;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_Z:
		  abits_val_ &= ~ಮುಖವಾಡ;
		  bbits_val_ |=  ಮುಖವಾಡ;
		  ನಿಲ್ಲಿಸು;
	    }
      }
}

ಸಾಲೊಳಗೆ vvp_vector4_t ಕ್ರಿಯಾಕಾರಕ ~ (ಸ್ಥಿರಾಂಕ vvp_vector4_t&that)
{
      vvp_vector4_t res (that, true);
      ಹಿಂತಿರುಗಿ res;
}

ಬಾಹ್ಯ std::ostream& ಕ್ರಿಯಾಕಾರಕ << (std::ostream&, ಸ್ಥಿರಾಂಕ vvp_vector4_t&);

ಬಾಹ್ಯ vvp_bit4_t compare_gtge(ಸ್ಥಿರಾಂಕ vvp_vector4_t&a,
			       ಸ್ಥಿರಾಂಕ vvp_vector4_t&b,
			       vvp_bit4_t val_if_equal);
ಬಾಹ್ಯ vvp_bit4_t compare_gtge_signed(ಸ್ಥಿರಾಂಕ vvp_vector4_t&a,
				      ಸ್ಥಿರಾಂಕ vvp_vector4_t&b,
				      vvp_bit4_t val_if_equal);
ಮಾದರಿ <class T> ಬಾಹ್ಯ T coerce_ಗಮ್ಯ_ಅಗಲ(ಸ್ಥಿರಾಂಕ T&that, ಚಿಹ್ನೆರಹಿತ ಅಗಲ);

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
ಮಾದರಿ <class T> ಬಾಹ್ಯ bool vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&a, T&val,
						bool is_signed,
						bool is_arithmetic =true);

ಮಾದರಿ <class T> ಬಾಹ್ಯ bool vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&a,
                                                bool&ಅತಿಪ್ರವಾಹ_flag, T&val);

ಮಾದರಿ <class T> ಸಾಲೊಳಗೆ bool vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&a, T&val)
{
      bool ಅತಿಪ್ರವಾಹ_flag;
      ಹಿಂತಿರುಗಿ vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(a, ಅತಿಪ್ರವಾಹ_flag, val);
}

ಬಾಹ್ಯ bool vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector4_t&a, ದ್ವಿಗುಣ&val, bool is_signed);

ಬಾಹ್ಯ bool vector2_ಗಮ್ಯ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ vvp_vector2_t&a, int32_t&val, bool is_signed);

ಬಾಹ್ಯ vvp_vector4_t vector4_from_ಪಠ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಬಿಟ್ಟುಗಳು, ಚಿಹ್ನೆರಹಿತ wid);

ಸಾಲೊಳಗೆ vvp_vector4_t double_ಗಮ್ಯ_vector4_LSB(ದ್ವಿಗುಣ real)
{
      // Convert the double to a bit vector and then use the LSB.
      // You can use very large real values and break the 64-bit
      // size, but that's really horrible code so don't do that!
      vvp_vector4_t ಬಿಟ್ = vvp_vector4_t(64, real);
      ಬಿಟ್.resize(1);
      ಹಿಂತಿರುಗಿ ಬಿಟ್;
}

/*
 * vvp_vector4array_t
 */
class vvp_vector4array_t {

    ಸಾರ್ವಜನಿಕ:
      vvp_vector4array_t(ಚಿಹ್ನೆರಹಿತ ಅಗಲ, ಚಿಹ್ನೆರಹಿತ ಪದಗಳು);
      ವಾಸ್ತವಿಕ ~vvp_vector4array_t();

      ಚಿಹ್ನೆರಹಿತ ಅಗಲ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಅಗಲ_2; }
      ಚಿಹ್ನೆರಹಿತ ಪದಗಳು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಪದಗಳು_3; }

      ವಾಸ್ತವಿಕ vvp_vector4_t get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ = 0;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ idx, ಸ್ಥಿರಾಂಕ vvp_vector4_t&that) = 0;

    ಸಂರಕ್ಷಿತ:
      ರಚನೆ v4cell {
	    ಒಕ್ಕೂಟ {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ abits_val_;
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*abits_ptr_;
	    };
	    ಒಕ್ಕೂಟ {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ bbits_val_;
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*bbits_ptr_;
	    };
      };

      vvp_vector4_t get_ಶಬ್ದ_2(v4cell*cell) ಸ್ಥಿರಾಂಕ;
      ಶೂನ್ಯ ಗಣ_ಶಬ್ದ_2(v4cell*cell, ಸ್ಥಿರಾಂಕ vvp_vector4_t&that);

      ಚಿಹ್ನೆರಹಿತ ಅಗಲ_2;
      ಚಿಹ್ನೆರಹಿತ ಪದಗಳು_3;

    ಖಾಸಗಿ: // Not implemented
      vvp_vector4array_t(ಸ್ಥಿರಾಂಕ vvp_vector4array_t&);
      vvp_vector4array_t& ಕ್ರಿಯಾಕಾರಕ = (ಸ್ಥಿರಾಂಕ vvp_vector4array_t&);
};

/*
 * Statically allocated vvp_vector4array_t
 */
class vvp_vector4array_sa : ಸಾರ್ವಜನಿಕ vvp_vector4array_t {

    ಸಾರ್ವಜನಿಕ:
      vvp_vector4array_sa(ಚಿಹ್ನೆರಹಿತ ಅಗಲ, ಚಿಹ್ನೆರಹಿತ ಪದಗಳು);
      ~vvp_vector4array_sa() override;

      vvp_vector4_t get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ override;
      ಶೂನ್ಯ ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ idx, ಸ್ಥಿರಾಂಕ vvp_vector4_t&that) override;

    ಖಾಸಗಿ:
      v4cell* array_;
};

/*
 * Automatically allocated vvp_vector4array_t
 */
class vvp_vector4array_aa : ಸಾರ್ವಜನಿಕ vvp_vector4array_t, ಸಾರ್ವಜನಿಕ automatic_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      vvp_vector4array_aa(ಚಿಹ್ನೆರಹಿತ ಅಗಲ, ಚಿಹ್ನೆರಹಿತ ಪದಗಳು);
      ~vvp_vector4array_aa() override;

      ಶೂನ್ಯ alloc_instance(vvp_context_t context) override;
      ಶೂನ್ಯ reset_instance(vvp_context_t context) override;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      ಶೂನ್ಯ free_instance(vvp_context_t context) override;
#ಆದರೆ_ಅಂತ್ಯ

      vvp_vector4_t get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ override;
      ಶೂನ್ಯ ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ idx, ಸ್ಥಿರಾಂಕ vvp_vector4_t&that) override;

    ಖಾಸಗಿ:
      ಚಿಹ್ನೆರಹಿತ context_idx_;
};

/* vvp_vector2_t
 */
class vvp_vector2_t {

      ಮಿತ್ರ vvp_vector2_t ಕ್ರಿಯಾಕಾರಕ - (ಸ್ಥಿರಾಂಕ vvp_vector2_t&);
      ಮಿತ್ರ vvp_vector2_t ಕ್ರಿಯಾಕಾರಕ + (ಸ್ಥಿರಾಂಕ vvp_vector2_t&,
				       ಸ್ಥಿರಾಂಕ vvp_vector2_t&);
      ಮಿತ್ರ vvp_vector2_t ಕ್ರಿಯಾಕಾರಕ * (ಸ್ಥಿರಾಂಕ vvp_vector2_t&,
				       ಸ್ಥಿರಾಂಕ vvp_vector2_t&);
      ಮಿತ್ರ bool ಕ್ರಿಯಾಕಾರಕ >  (ಸ್ಥಿರಾಂಕ vvp_vector2_t&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&);
      ಮಿತ್ರ bool ಕ್ರಿಯಾಕಾರಕ >= (ಸ್ಥಿರಾಂಕ vvp_vector2_t&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&);
      ಮಿತ್ರ bool ಕ್ರಿಯಾಕಾರಕ <  (ಸ್ಥಿರಾಂಕ vvp_vector2_t&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&);
      ಮಿತ್ರ bool ಕ್ರಿಯಾಕಾರಕ <= (ಸ್ಥಿರಾಂಕ vvp_vector2_t&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&);
      ಮಿತ್ರ bool ಕ್ರಿಯಾಕಾರಕ == (ಸ್ಥಿರಾಂಕ vvp_vector2_t&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&);

    ಸಾರ್ವಜನಿಕ:
      vvp_vector2_t();
      vvp_vector2_t(ಸ್ಥಿರಾಂಕ vvp_vector2_t&);
      vvp_vector2_t(ಸ್ಥಿರಾಂಕ vvp_vector2_t&, ಚಿಹ್ನೆರಹಿತ newsize);
      vvp_vector2_t(ಸ್ಥಿರಾಂಕ vvp_vector2_t&, ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid);
	// Make a vvp_vector2_t from a vvp_vector4_t. If enable_NaN
	// is true and there are X or Z bits, then the result becomes
	// a NaN value. If enable_NaN is false then X or Z bits are
	// converted to 0 as per the standard Verilog rules.
      explicit vvp_vector2_t(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that, bool allow_NaN = false);
	// Make from a 64-bit value and a specified width.
      vvp_vector2_t(uint64_t val, ಚಿಹ್ನೆರಹಿತ wid);
	// Make with the width, and filled with 1 or 0 bits.
      ಎಣಿಕೆ fill_t {FILL0, FILL1};
      vvp_vector2_t(fill_t fill, ಚಿಹ್ನೆರಹಿತ wid);
      ~vvp_vector2_t();

      vvp_vector2_t&ಕ್ರಿಯಾಕಾರಕ += (ಸ್ಥಿರಾಂಕ vvp_vector2_t&that);
      vvp_vector2_t&ಕ್ರಿಯಾಕಾರಕ -= (ಸ್ಥಿರಾಂಕ vvp_vector2_t&that);
      vvp_vector2_t&ಕ್ರಿಯಾಕಾರಕ <<= (ಚಿಹ್ನೆರಹಿತ ಸರಿಸು);
      vvp_vector2_t&ಕ್ರಿಯಾಕಾರಕ >>= (ಚಿಹ್ನೆರಹಿತ ಸರಿಸು);
      vvp_vector2_t&ಕ್ರಿಯಾಕಾರಕ = (ಸ್ಥಿರಾಂಕ vvp_vector2_t&);

	// Assign a vec4 to a vec2, using Verilog rules for converting
	// XZ values to 0.
      vvp_vector2_t&ಕ್ರಿಯಾಕಾರಕ = (ಸ್ಥಿರಾಂಕ vvp_vector4_t&);

      bool is_NaN() ಸ್ಥಿರಾಂಕ;
      bool is_zero() ಸ್ಥಿರಾಂಕ;
      ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ;
      ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ;
      vvp_bit4_t value4(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ;
	// Get the vector2 subvector starting at the address
      vvp_vector2_t subvalue(ಚಿಹ್ನೆರಹಿತ idx, ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2) ಸ್ಥಿರಾಂಕ;
      ಶೂನ್ಯ ಗಣ_ಬಿಟ್(ಚಿಹ್ನೆರಹಿತ idx, ಪೂರ್ಣಾಂಕ ಬಿಟ್);
      ಶೂನ್ಯ ಗಣ_vec(ಚಿಹ್ನೆರಹಿತ idx, ಸ್ಥಿರಾಂಕ vvp_vector2_t&that);
	// Make the size just big enough to hold the first 1 bit.
      ಶೂನ್ಯ trim();
	// Trim off extra 1 bit since this is representing a negative value.
	// Always keep at least 32 bits.
      ಶೂನ್ಯ trim_neg();

    ಖಾಸಗಿ:
      ಎಣಿಕೆ { ಬಿಟ್ಟುಗಳು_ಪ್ರತಿ_ಶಬ್ದ = 8 * ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) };
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*vec_;
      ಚಿಹ್ನೆರಹಿತ wid_;

    ಖಾಸಗಿ:
      ಶೂನ್ಯ copy_from_that_(ಸ್ಥಿರಾಂಕ vvp_vector2_t&that);
      ಶೂನ್ಯ copy_from_that_(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that);
};

ಬಾಹ್ಯ bool ಕ್ರಿಯಾಕಾರಕ >  (ಸ್ಥಿರಾಂಕ vvp_vector2_t&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&);
ಬಾಹ್ಯ bool ಕ್ರಿಯಾಕಾರಕ >= (ಸ್ಥಿರಾಂಕ vvp_vector2_t&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&);
ಬಾಹ್ಯ bool ಕ್ರಿಯಾಕಾರಕ <  (ಸ್ಥಿರಾಂಕ vvp_vector2_t&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&);
ಬಾಹ್ಯ bool ಕ್ರಿಯಾಕಾರಕ <= (ಸ್ಥಿರಾಂಕ vvp_vector2_t&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&);
ಬಾಹ್ಯ bool ಕ್ರಿಯಾಕಾರಕ == (ಸ್ಥಿರಾಂಕ vvp_vector2_t&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&);
ಬಾಹ್ಯ vvp_vector2_t ಕ್ರಿಯಾಕಾರಕ - (ಸ್ಥಿರಾಂಕ vvp_vector2_t&);
ಬಾಹ್ಯ vvp_vector2_t ಕ್ರಿಯಾಕಾರಕ + (ಸ್ಥಿರಾಂಕ vvp_vector2_t&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&);
ಬಾಹ್ಯ vvp_vector2_t ಕ್ರಿಯಾಕಾರಕ * (ಸ್ಥಿರಾಂಕ vvp_vector2_t&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&);
ಬಾಹ್ಯ vvp_vector2_t ಕ್ರಿಯಾಕಾರಕ / (ಸ್ಥಿರಾಂಕ vvp_vector2_t&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&);
ಬಾಹ್ಯ vvp_vector2_t ಕ್ರಿಯಾಕಾರಕ % (ಸ್ಥಿರಾಂಕ vvp_vector2_t&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&);

vvp_vector2_t ಘಾತ(ಸ್ಥಿರಾಂಕ vvp_vector2_t&, vvp_vector2_t&);
ಬಾಹ್ಯ vvp_vector4_t vector2_ಗಮ್ಯ_vector4(ಸ್ಥಿರಾಂಕ vvp_vector2_t&, ಚಿಹ್ನೆರಹಿತ wid);

/* A c4string is of the form C4<...> where ... are bits. */
ಬಾಹ್ಯ bool c4string_test(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str);
ಬಾಹ್ಯ vvp_vector4_t c4string_ಗಮ್ಯ_vector4(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str);

/* A crstring is of the form Cr<...> where ... defines are real. */
ಬಾಹ್ಯ bool crstring_test(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str);
ಬಾಹ್ಯ ದ್ವಿಗುಣ crstring_ಗಮ್ಯ_double(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str);

ಬಾಹ್ಯ std::ostream& ಕ್ರಿಯಾಕಾರಕ<< (std::ostream&, ಸ್ಥಿರಾಂಕ vvp_vector2_t&);

ಸಾಲೊಳಗೆ vvp_vector2_t::vvp_vector2_t(ಸ್ಥಿರಾಂಕ vvp_vector2_t&that)
{
      copy_from_that_(that);
}

ಸಾಲೊಳಗೆ vvp_vector2_t::vvp_vector2_t(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that, bool enable_NaN)
{
      ಆದರೆ (enable_NaN && that.has_xz()) {
	    vec_ = 0;
	    wid_ = 0;
	    ಹಿಂತಿರುಗಿ;
      }
      copy_from_that_(that);
}

ಸಾಲೊಳಗೆ vvp_vector2_t::~vvp_vector2_t()
{
      delete[] vec_;
}

/* Inline some of the vector2_t methods. */
ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ vvp_vector2_t::ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ wid_;
}

ಸಾಲೊಳಗೆ vvp_bit4_t vvp_vector2_t::value4(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ
{
      ಆದರೆ (ಮೌಲ್ಯ(idx))
	    ಹಿಂತಿರುಗಿ BIT4_1;
      ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ BIT4_0;
}

ಸಾಲೊಳಗೆ vvp_vector2_t vvp_vector2_t::subvalue(ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid) ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ vvp_vector2_t(*this, ತಳ, wid);
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

      ಮಿತ್ರ vvp_scalar_t fully_featured_resolv_(vvp_scalar_t a, vvp_scalar_t b);

    ಸಾರ್ವಜನಿಕ:
	// Make a HiZ value.
      explicit vvp_scalar_t();

	// Make an unambiguous value.
      explicit vvp_scalar_t(vvp_bit4_t val, ಚಿಹ್ನೆರಹಿತ str0, ಚಿಹ್ನೆರಹಿತ str1);

	// Get the vvp_bit4_t version of the value
      vvp_bit4_t ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ;
      ಚಿಹ್ನೆರಹಿತ strength0() ಸ್ಥಿರಾಂಕ;
      ಚಿಹ್ನೆರಹಿತ strength1() ಸ್ಥಿರಾಂಕ;

      bool eeq(vvp_scalar_t that) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ_2 == that.ಮೌಲ್ಯ_2; }
      bool is_hiz() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ (ಮೌಲ್ಯ_2 & 0x77) == 0; }

    ಖಾಸಗಿ:
	// This class and the vvp_vector8_t class are closely related,
	// so allow vvp_vector8_t access to the raw encoding so that
	// it can do compact vectoring of vvp_scalar_t objects.
      ಮಿತ್ರ class vvp_vector8_t;
      explicit vvp_scalar_t(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ val) : ಮೌಲ್ಯ_2(val) { }
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ raw() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ_2; }

    ಖಾಸಗಿ:
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಮೌಲ್ಯ_2;
};

ಸಾಲೊಳಗೆ vvp_scalar_t::vvp_scalar_t()
{
      ಮೌಲ್ಯ_2 = 0;
}

ಸಾಲೊಳಗೆ vvp_scalar_t::vvp_scalar_t(vvp_bit4_t val, ಚಿಹ್ನೆರಹಿತ str0, ಚಿಹ್ನೆರಹಿತ str1)
{
      ಪ್ರತಿಜ್ಞಾ(str0 <= 7);
      ಪ್ರತಿಜ್ಞಾ(str1 <= 7);

      ಆದರೆ (str0 == 0 && str1 == 0) {
	    ಮೌಲ್ಯ_2 = 0x00;
      } ಇಲ್ಲದಿದ್ದರೆ ಆಯ್ಕೆ (val) {
	  ಸ್ಥಿತಿ BIT4_0:
	    ಮೌಲ್ಯ_2 = str0 | (str0<<4);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_1:
	    ಮೌಲ್ಯ_2 = str1 | (str1<<4) | 0x88;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_X:
	    ಮೌಲ್ಯ_2 = str0 | (str1<<4) | 0x80;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_Z:
	    ಮೌಲ್ಯ_2 = 0x00;
	    ನಿಲ್ಲಿಸು;
      }
}

ಸಾಲೊಳಗೆ vvp_bit4_t vvp_scalar_t::ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ
{
      ಆದರೆ ((ಮೌಲ್ಯ_2 & 0x77) == 0) {
	    ಹಿಂತಿರುಗಿ BIT4_Z;
      } ಇಲ್ಲದಿದ್ದರೆ ಆಯ್ಕೆ (ಮೌಲ್ಯ_2 & 0x88) {
	  ಸ್ಥಿತಿ 0x00:
	    ಹಿಂತಿರುಗಿ BIT4_0;
	  ಸ್ಥಿತಿ 0x88:
	    ಹಿಂತಿರುಗಿ BIT4_1;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ BIT4_X;
      }
}


ಸಾಲೊಳಗೆ vvp_scalar_t resolve(vvp_scalar_t a, vvp_scalar_t b)
{
      ಬಾಹ್ಯ vvp_scalar_t fully_featured_resolv_(vvp_scalar_t, vvp_scalar_t);

	// If the value is HiZ, resolution is simply a matter of
	// returning the *other* value.
      ಆದರೆ (a.is_hiz())
	    ಹಿಂತಿರುಗಿ b;
      ಆದರೆ (b.is_hiz())
	    ಹಿಂತಿರುಗಿ a;
	// If the values are the identical, then resolution is simply
	// returning *either* value.
      ಆದರೆ (a .eeq( b ))
	    ಹಿಂತಿರುಗಿ a;

      ಹಿಂತಿರುಗಿ fully_featured_resolv_(a,b);
}

ಬಾಹ್ಯ std::ostream& ಕ್ರಿಯಾಕಾರಕ<< (std::ostream&, vvp_scalar_t);

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

      ಮಿತ್ರ vvp_vector8_t part_expand(ಸ್ಥಿರಾಂಕ vvp_vector8_t&, ಚಿಹ್ನೆರಹಿತ, ಚಿಹ್ನೆರಹಿತ);

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_vector8_t(ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2 =0);
	// Make a vvp_vector8_t from a vector4 and a specified
	// strength.
      explicit vvp_vector8_t(ಸ್ಥಿರಾಂಕ vvp_vector4_t&that,
			     ಚಿಹ್ನೆರಹಿತ str0,
			     ಚಿಹ್ನೆರಹಿತ str1);
      explicit vvp_vector8_t(ಸ್ಥಿರಾಂಕ vvp_vector2_t&that,
			     ಚಿಹ್ನೆರಹಿತ str0,
			     ಚಿಹ್ನೆರಹಿತ str1);

      ~vvp_vector8_t();

      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ vvp_vector8_t nil;

    ಸಾರ್ವಜನಿಕ:

      ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಗಾತ್ರ_3; }
      vvp_scalar_t ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ;
      vvp_vector8_t subvalue(ಚಿಹ್ನೆರಹಿತ adr, ಚಿಹ್ನೆರಹಿತ ಅಗಲ) ಸ್ಥಿರಾಂಕ;
      ಶೂನ್ಯ ಗಣ_ಬಿಟ್(ಚಿಹ್ನೆರಹಿತ idx, vvp_scalar_t val);
      ಶೂನ್ಯ ಗಣ_vec(ಚಿಹ್ನೆರಹಿತ idx, ಸ್ಥಿರಾಂಕ vvp_vector8_t&that);

	// Test that the vectors are exactly equal
      bool eeq(ಸ್ಥಿರಾಂಕ vvp_vector8_t&that) ಸ್ಥಿರಾಂಕ;

      vvp_vector8_t(ಸ್ಥಿರಾಂಕ vvp_vector8_t&that);
      vvp_vector8_t& ಕ್ರಿಯಾಕಾರಕ= (ಸ್ಥಿರಾಂಕ vvp_vector8_t&that);

    ಖಾಸಗಿ:
      ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_3;
      ಒಕ್ಕೂಟ {
	    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ*ptr_;
	    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ val_[ಗಾತ್ರ(ಶೂನ್ಯ*)];
      };
};

  /* Resolve uses the default Verilog resolver algorithm to resolve
     two drive vectors to a single output. */
ಸಾಲೊಳಗೆ vvp_vector8_t resolve(ಸ್ಥಿರಾಂಕ vvp_vector8_t&a, ಸ್ಥಿರಾಂಕ vvp_vector8_t&b)
{
      ಪ್ರತಿಜ್ಞಾ(a.ಗಾತ್ರ_2() == b.ಗಾತ್ರ_2());
      vvp_vector8_t ಹೊರಹೋಗು (a.ಗಾತ್ರ_2());

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಹೊರಹೋಗು.ಗಾತ್ರ_2() ;  idx += 1) {
	    ಹೊರಹೋಗು.ಗಣ_ಬಿಟ್(idx, resolve(a.ಮೌಲ್ಯ(idx), b.ಮೌಲ್ಯ(idx)));
      }

      ಹಿಂತಿರುಗಿ ಹೊರಹೋಗು;
}

  /* This lookup tabke implements the strength reduction implied by
     Verilog standard switch devices. The major dimension selects
     between non-resistive and resistive devices. */
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ vvp_switch_strength_map[2][8];

  /* The reduce4 function converts a vector8 to a vector4, losing
     strength information in the process. */
ಬಾಹ್ಯ vvp_vector4_t reduce4(ಸ್ಥಿರಾಂಕ vvp_vector8_t&that);
ಬಾಹ್ಯ vvp_vector8_t part_expand(ಸ್ಥಿರಾಂಕ vvp_vector8_t&a, ಚಿಹ್ನೆರಹಿತ wid, ಚಿಹ್ನೆರಹಿತ off);

  /* A c8string is of the form C8<...> where ... are bits. */
ಬಾಹ್ಯ bool c8string_test(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str);
ಬಾಹ್ಯ vvp_vector8_t c8string_ಗಮ್ಯ_vector8(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str);

  /* Print a vector8 value to a stream. */
ಬಾಹ್ಯ std::ostream& ಕ್ರಿಯಾಕಾರಕ<< (std::ostream&, ಸ್ಥಿರಾಂಕ vvp_vector8_t&);

ಸಾಲೊಳಗೆ vvp_vector8_t::vvp_vector8_t(ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_4)
: ಗಾತ್ರ_3(ಗಾತ್ರ_4)
{
      ಆದರೆ (ಗಾತ್ರ_3 <= ಗಾತ್ರ(val_)) {
	    memset(val_, 0, ಗಾತ್ರ(val_));
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ptr_ = new ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ[ಗಾತ್ರ_3];
	    memset(ptr_, 0, ಗಾತ್ರ_3);
      }
}

ಸಾಲೊಳಗೆ vvp_vector8_t::~vvp_vector8_t()
{
      ಆದರೆ (ಗಾತ್ರ_3 > ಗಾತ್ರ(val_))
	    delete[]ptr_;
}

ಸಾಲೊಳಗೆ vvp_scalar_t vvp_vector8_t::ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ
{
      ಪ್ರತಿಜ್ಞಾ(idx < ಗಾತ್ರ_3);
      ಆದರೆ (ಗಾತ್ರ_3 <= ಗಾತ್ರ(val_))
	    ಹಿಂತಿರುಗಿ vvp_scalar_t(val_[idx]);
      ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ vvp_scalar_t(ptr_[idx]);
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ vvp_vector8_t::ಗಣ_ಬಿಟ್(ಚಿಹ್ನೆರಹಿತ idx, vvp_scalar_t val)
{
      ಪ್ರತಿಜ್ಞಾ(idx < ಗಾತ್ರ_3);
      ಆದರೆ (ಗಾತ್ರ_3 <= ಗಾತ್ರ(val_))
	    val_[idx] = val.raw();
      ಇಲ್ಲದಿದ್ದರೆ
	    ptr_[idx] = val.raw();
}

  // Exactly-equal for vvp_vector8_t is common and should be as tight
  // as possible.
ಸಾಲೊಳಗೆ bool vvp_vector8_t::eeq(ಸ್ಥಿರಾಂಕ vvp_vector8_t&that) ಸ್ಥಿರಾಂಕ
{
      ಆದರೆ (ಗಾತ್ರ_3 != that.ಗಾತ್ರ_3)
	    ಹಿಂತಿರುಗಿ false;
      ಆದರೆ (ಗಾತ್ರ_3 == 0)
	    ಹಿಂತಿರುಗಿ true;

      ಆದರೆ (ಗಾತ್ರ_3 <= ಗಾತ್ರ(val_))
	      // This is equivalent to memcmp(val_, that.val_, sizeof val_)==0
	    ಹಿಂತಿರುಗಿ ptr_ == that.ptr_;
      ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ memcmp(ptr_, that.ptr_, ಗಾತ್ರ_3) == 0;
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
ಮಾದರಿ <class T> class vvp_sub_ಸೂಚಕ_t {

    ಸಾರ್ವಜನಿಕ:
      vvp_sub_ಸೂಚಕ_t() : ಬಿಟ್ಟುಗಳು_2(0) { }

      vvp_sub_ಸೂಚಕ_t(T*ptr__, ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್_2)
      {
	    ಬಿಟ್ಟುಗಳು_2 = reinterpret_cast<uintptr_t> (ptr__);
	    ಪ್ರತಿಜ್ಞಾ( (ಬಿಟ್ಟುಗಳು_2  &  UINTPTR_C(3)) == 0 );
	    ಪ್ರತಿಜ್ಞಾ( (ಪೋರ್ಟ್_2 & ~UINTPTR_C(3)) == 0 );
	    ಬಿಟ್ಟುಗಳು_2 |= ಪೋರ್ಟ್_2;
      }

      ~vvp_sub_ಸೂಚಕ_t() = ಪೂರ್ವನಿಯೋಜಿತ;

      T* ptr()
      { ಹಿಂತಿರುಗಿ reinterpret_cast<T*> (ಬಿಟ್ಟುಗಳು_2 & ~UINTPTR_C(3)); }

      ಸ್ಥಿರಾಂಕ T* ptr() ಸ್ಥಿರಾಂಕ
      { ಹಿಂತಿರುಗಿ reinterpret_cast<ಸ್ಥಿರಾಂಕ T*> (ಬಿಟ್ಟುಗಳು_2 & ~UINTPTR_C(3)); }

      ಚಿಹ್ನೆರಹಿತ  ಪೋರ್ಟ್() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಬಿಟ್ಟುಗಳು_2 & UINTPTR_C(3); }

      bool nil() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಬಿಟ್ಟುಗಳು_2 == 0; }

      bool ಕ್ರಿಯಾಕಾರಕ == (vvp_sub_ಸೂಚಕ_t that) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಬಿಟ್ಟುಗಳು_2 == that.ಬಿಟ್ಟುಗಳು_2; }
      bool ಕ್ರಿಯಾಕಾರಕ != (vvp_sub_ಸೂಚಕ_t that) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಬಿಟ್ಟುಗಳು_2 != that.ಬಿಟ್ಟುಗಳು_2; }

    ಖಾಸಗಿ:
      uintptr_t ಬಿಟ್ಟುಗಳು_2;
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ vvp_sub_ಸೂಚಕ_t<vvp_net_t> vvp_net_ptr_t;
ಮಾದರಿ <class T> std::ostream& ಕ್ರಿಯಾಕಾರಕ << (std::ostream&ಹೊರಹೋಗು, vvp_sub_ಸೂಚಕ_t<T> val)
{ ಹೊರಹೋಗು << val.ptr() << "[" << val.ಪೋರ್ಟ್() << "]"; ಹಿಂತಿರುಗಿ ಹೊರಹೋಗು; }

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
    ಸಾರ್ವಜನಿಕ:
      vvp_net_t();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      vvp_net_t *pool;
#ಆದರೆ_ಅಂತ್ಯ
      vvp_net_ptr_t ಪೋರ್ಟ್[4];
      vvp_net_fun_t*fun;
      vvp_net_fil_t*fil;

    ಸಾರ್ವಜನಿಕ:
	// Connect the port to the output from this net.
      ಶೂನ್ಯ link(vvp_net_ptr_t ಪೋರ್ಟ್);
	// Disconnect the port from the output of this net.
      ಶೂನ್ಯ unlink(vvp_net_ptr_t ಪೋರ್ಟ್);

    ಸಾರ್ವಜನಿಕ: // Methods to propagate output from this node.
      ಶೂನ್ಯ send_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&val, vvp_context_t context);
      ಶೂನ್ಯ send_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&val);
      ಶೂನ್ಯ send_real(ದ್ವಿಗುಣ val, vvp_context_t context);
      ಶೂನ್ಯ send_string(ಸ್ಥಿರಾಂಕ std::string&val, vvp_context_t context);
      ಶೂನ್ಯ send_object(vvp_object_t val, vvp_context_t context);

      ಶೂನ್ಯ send_vec4_pv(ಸ್ಥಿರಾಂಕ vvp_vector4_t&val,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t context);
      ಶೂನ್ಯ send_vec8_pv(ಸ್ಥಿರಾಂಕ vvp_vector8_t&val,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid);


    ಸಾರ್ವಜನಿಕ: // Methods to arrange for the output of this net to be forced.

	// The intent is that all efforts at force are directed to
	// operate only on the vvp_net_t whose output is to be
	// forced. These methods then communicate the force to the
	// attached filter to set up the actual force.
      ಶೂನ್ಯ force_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ);
      ಶೂನ್ಯ force_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ);
      ಶೂನ್ಯ force_real(ದ್ವಿಗುಣ val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ);

    ಸಾರ್ವಜನಿಕ: // Method to support $countdrivers
      ಶೂನ್ಯ ಎಣಿಕೆ_drivers(ಚಿಹ್ನೆರಹಿತ idx, ಚಿಹ್ನೆರಹಿತ counts[4]);

    // This needs to be public so that SDF interconnects can be inserted
    ಸಾರ್ವಜನಿಕ:
      vvp_net_ptr_t ಹೊರಹೋಗು_2;

    ಸಾರ್ವಜನಿಕ: // Need a better new for these objects.
      ಸ್ಥಿರ ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ new(std::size_t ಗಾತ್ರ_2);
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ delete(ಶೂನ್ಯ*); // not implemented
    ಖಾಸಗಿ: // not implemented
      ಸ್ಥಿರ ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ new[](std::size_t ಗಾತ್ರ_2);
#ಆದರೆ defined(__GNUC__)
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ delete[](ಶೂನ್ಯ*);
#ಆದರೆ_ಅಂತ್ಯ
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

    ಸಾರ್ವಜನಿಕ:
      vvp_net_fun_t();
      ವಾಸ್ತವಿಕ ~vvp_net_fun_t();

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                             vvp_context_t context);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ recv_vec8(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ recv_real(vvp_net_ptr_t ಪೋರ್ಟ್, ದ್ವಿಗುಣ ಬಿಟ್,
                             vvp_context_t context);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ recv_string(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ std::string&ಬಿಟ್,
			       vvp_context_t context);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ recv_object(vvp_net_ptr_t ಪೋರ್ಟ್, vvp_object_t ಬಿಟ್,
			       vvp_context_t context);

	// Part select variants of above
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t p, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t context);
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ recv_vec8_pv(vvp_net_ptr_t p, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್,
				ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid);

	// This method is called when the net is forced or
	// released. This is very rarely needed; island ports use it
	// to know that the net is being forced and that it needs to
	// do something about it.
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ force_flag(bool ಸತತ_now);

   ಸಂರಕ್ಷಿತ:
      ಶೂನ್ಯ recv_vec4_pv_(vvp_net_ptr_t p, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			 ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t context);
      ಶೂನ್ಯ recv_vec8_pv_(vvp_net_ptr_t p, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್,
			 ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid);

    ಸಾರ್ವಜನಿಕ: // These objects are only permallocated.
      ಸ್ಥಿರ ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ new(std::size_t ಗಾತ್ರ_2) { ಹಿಂತಿರುಗಿ ರಾಶಿ.alloc(ಗಾತ್ರ_2); }
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ delete(ಶೂನ್ಯ*); // not implemented

      ಸ್ಥಿರ std::size_t ರಾಶಿ_ಒಟ್ಟು() { ಹಿಂತಿರುಗಿ ರಾಶಿ.ರಾಶಿ_ಒಟ್ಟು(); }

    ಸಂರಕ್ಷಿತ:
      ಸ್ಥಿರ permaheap ರಾಶಿ;

    ಖಾಸಗಿ: // not implemented
      vvp_net_fun_t(ಸ್ಥಿರಾಂಕ vvp_net_fun_t&);
      vvp_net_fun_t& ಕ್ರಿಯಾಕಾರಕ= (ಸ್ಥಿರಾಂಕ vvp_net_fun_t&);
      ಸ್ಥಿರ ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ new[](std::size_t ಗಾತ್ರ_2);
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ delete[](ಶೂನ್ಯ*);
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
class vvp_net_fil_t  : ಸಾರ್ವಜನಿಕ vvp_vpi_callback {

    ಸಾರ್ವಜನಿಕ:
      vvp_net_fil_t();
      ವಾಸ್ತವಿಕ ~vvp_net_fil_t() override;

    ಸಾರ್ವಜನಿಕ:
      ಎಣಿಕೆ prop_t { STOP=0, PROP, REPL };


	// These filter methods are used by the vvp_net_t::send_*()
	// methods to test the output (from the functor) bit value
	// against any force filters. If none of the bits are forced,
	// then the method returns PROP and the caller propagates the
	// bit value. If bits were changed by the force mask, then the
	// method returns REPL and the caller should propagate the rep
	// value instead. If the function returns STOP, then all the
	// output bits are filtered by the force mask and there is
	// nothing to propagate.
      ವಾಸ್ತವಿಕ prop_t filter_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್, vvp_vector4_t&rep,
				 ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid);
      ವಾಸ್ತವಿಕ prop_t filter_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&val, vvp_vector8_t&rep,
				 ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid);
      ವಾಸ್ತವಿಕ prop_t filter_real(ದ್ವಿಗುಣ&val);
      ವಾಸ್ತವಿಕ prop_t filter_object(vvp_object_t&val);
      ವಾಸ್ತವಿಕ prop_t filter_string(ಸ್ಥಿರಾಂಕ std::string&val);

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ release(vvp_net_ptr_t ptr, bool net_flag) =0;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ release_pv(vvp_net_ptr_t ptr, ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid, bool net_flag) =0;

	// The %force/link instruction needs a place to write the
	// source node of the force, so that subsequent %force and
	// %release instructions can undo the link as needed. */
      ಶೂನ್ಯ force_link(vvp_net_t*dst, vvp_net_t*src);
      ಶೂನ್ಯ force_unlink(ಶೂನ್ಯ);

      ವಾಸ್ತವಿಕ ಚಿಹ್ನೆರಹಿತ filter_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ =0;

    ಸಾರ್ವಜನಿಕ:
	// Support for force methods. These are called by the
	// vvp_net_t::force_* methods to set the force value and mask
	// for the filter.
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ force_fil_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ) =0;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ force_fil_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ) =0;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ force_fil_real(ದ್ವಿಗುಣ val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ) =0;

    ಸಾರ್ವಜನಿಕ: // These objects are only permallocated.
      ಸ್ಥಿರ ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ new(std::size_t ಗಾತ್ರ_2) { ಹಿಂತಿರುಗಿ ರಾಶಿ.alloc(ಗಾತ್ರ_2); }
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ delete(ಶೂನ್ಯ*); // not implemented

      ಸ್ಥಿರ size_t ರಾಶಿ_ಒಟ್ಟು() { ಹಿಂತಿರುಗಿ ರಾಶಿ.ರಾಶಿ_ಒಟ್ಟು(); }

    ಖಾಸಗಿ:
      ಸ್ಥಿರ permaheap ರಾಶಿ;

    ಖಾಸಗಿ: // not implemented
      vvp_net_fil_t(ಸ್ಥಿರಾಂಕ vvp_net_fil_t&);
      vvp_net_fil_t& ಕ್ರಿಯಾಕಾರಕ= (ಸ್ಥಿರಾಂಕ vvp_net_fil_t&);
      ಸ್ಥಿರ ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ new[](std::size_t ಗಾತ್ರ_2);
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ delete[](ಶೂನ್ಯ*);

    ಸಂರಕ್ಷಿತ:
	// Set bits of the filter force mask
      ಶೂನ್ಯ force_ಮುಖವಾಡ(ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ);
	// Release the force on the bits set in the mask.
      ಶೂನ್ಯ release_ಮುಖವಾಡ(ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ);
	// Test bits of the filter force mask;
      bool test_force_ಮುಖವಾಡ(ಚಿಹ್ನೆರಹಿತ ಬಿಟ್) ಸ್ಥಿರಾಂಕ;
      bool test_force_ಮುಖವಾಡ_is_zero() ಸ್ಥಿರಾಂಕ;

	// This template method is used by derived classes to process
	// the val through the force mask. The force value is the
	// currently forced value, and the buf is a value that this
	// method will use to hold a filtered value, if needed. This
	// method returns the replacement value into the "rep"
	// argument and returns a code indicating whether any changes
	// were made.
      ಮಾದರಿ <class T> prop_t filter_ಮುಖವಾಡ(ಸ್ಥಿರಾಂಕ T&val, ಸ್ಥಿರಾಂಕ T&force, T&rep, ಚಿಹ್ನೆರಹಿತ addr);
	// This template method is similar to the above, but works for
	// native types that are not so expensive to edit in place.
      ಮಾದರಿ <class T> prop_t filter_ಮುಖವಾಡ(T&val, T force);

	// These templates are similar to filter_mask_, but are
	// idempotent. Then do not trigger callbacks or otherwise
	// cause any local changes. These methods are used to test
	// arbitrary values against the force mask.
      ಮಾದರಿ <class T> prop_t filter_ಒಳಹರಿವು_ಮುಖವಾಡ(ಸ್ಥಿರಾಂಕ T&val, ಸ್ಥಿರಾಂಕ T&force, T&rep) ಸ್ಥಿರಾಂಕ;

    ಖಾಸಗಿ:
	// Mask of forced bits
      vvp_vector2_t force_ಮುಖವಾಡ_2;
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
class vvp_fun_force : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      vvp_fun_force();
      ~vvp_fun_force() override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
		     vvp_context_t context) override;
      ಶೂನ್ಯ recv_real(vvp_net_ptr_t ಪೋರ್ಟ್, ದ್ವಿಗುಣ ಬಿಟ್, vvp_context_t) override;
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
class vvp_fun_drive  : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_drive(ಚಿಹ್ನೆರಹಿತ str0 =6, ಚಿಹ್ನೆರಹಿತ str1 =6);
      ~vvp_fun_drive() override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t context) override;

      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t) override;
    ಖಾಸಗಿ:
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ drive0_;
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ drive1_;
};

/*
 * EXTEND functors expand an input vector to the desired output
 * width. The extend_signed functor sign extends the input. If the
 * input is already wider than the desired output, then it is passed
 * unmodified.
 */
class vvp_fun_extend_signed  : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_extend_signed(ಚಿಹ್ನೆರಹಿತ wid);
      ~vvp_fun_extend_signed() override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t context) override;

      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t) override;
    ಖಾಸಗಿ:
      ಚಿಹ್ನೆರಹಿತ ಅಗಲ_2;
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

class vvp_wide_fun_core : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      vvp_wide_fun_core(vvp_net_t*net, ಚಿಹ್ನೆರಹಿತ nports);
      ವಾಸ್ತವಿಕ ~vvp_wide_fun_core() override;
	// These objects are not perm allocated.
      ಸ್ಥಿರ ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ new(std::size_t ಗಾತ್ರ_2) { ಹಿಂತಿರುಗಿ ::new ಅಕ್ಷರ[ಗಾತ್ರ_2]; }
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ delete(ಶೂನ್ಯ* ptr) { ::delete[](static_cast<ಅಕ್ಷರ*>(ptr)); }

    ಸಂರಕ್ಷಿತ:
      ಶೂನ್ಯ propagate_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್, vvp_time64_t delay =0);
      ಶೂನ್ಯ propagate_real(ದ್ವಿಗುಣ ಬಿಟ್, vvp_time64_t delay =0);
      ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್_ಎಣಿಕೆ() ಸ್ಥಿರಾಂಕ;

      vvp_vector4_t& ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ);
      ದ್ವಿಗುಣ ಮೌಲ್ಯ_r(ಚಿಹ್ನೆರಹಿತ);

    ಖಾಸಗಿ:
	// the derived class implements this to receive an indication
	// that one of the port input values changed.
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ recv_vec4_from_inputs(ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್) =0;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ recv_real_from_inputs(ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್);

      ಮಿತ್ರ class vvp_wide_fun_t;
      ಶೂನ್ಯ dispatch_vec4_from_ಒಳಹರಿವು(ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್);
      ಶೂನ್ಯ dispatch_real_from_ಒಳಹರಿವು(ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್, ದ್ವಿಗುಣ ಬಿಟ್);

    ಖಾಸಗಿ:
	// Back-point to the vvp_net_t that points to me.
      vvp_net_t*ptr_;
	// Structure to track the input values from the input functors.
      ಚಿಹ್ನೆರಹಿತ nports_;
      vvp_vector4_t*ಪೋರ್ಟ್_ಮೌಲ್ಯಗಳು;
      ದ್ವಿಗುಣ*ಪೋರ್ಟ್_rvalues;
};

/*
 * The job of the input functor is only to monitor inputs to the
 * function and pass them to the ufunc_core object. Each functor takes
 * up to 4 inputs, with the base the port number for the first
 * function input that this functor represents.
 */
class vvp_wide_fun_t : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      vvp_wide_fun_t(vvp_wide_fun_core*c, ಚಿಹ್ನೆರಹಿತ ತಳ);
      ~vvp_wide_fun_t() override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t context) override;
      ಶೂನ್ಯ recv_real(vvp_net_ptr_t ಪೋರ್ಟ್, ದ್ವಿಗುಣ ಬಿಟ್,
                     vvp_context_t context) override;

      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t p, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t context) override;

    ಖಾಸಗಿ:
      vvp_wide_fun_core*core_;
      ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್_ತಳ_2;
};


ಸಾಲೊಳಗೆ ಶೂನ್ಯ vvp_send_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val, vvp_context_t context)
{
      ಆಗ (class vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t ಮುಂದಿನ_val = cur->ಪೋರ್ಟ್[ptr.ಪೋರ್ಟ್()];

	    ಆದರೆ (cur->fun)
		  cur->fun->recv_vec4(ptr, val, context);

	    ptr = ಮುಂದಿನ_val;
      }
}

ಬಾಹ್ಯ ಶೂನ್ಯ vvp_send_vec8(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector8_t&val);
ಬಾಹ್ಯ ಶೂನ್ಯ vvp_send_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ val,
                          vvp_context_t context);

ಸಾಲೊಳಗೆ ಶೂನ್ಯ vvp_send_string(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ std::string&val, vvp_context_t context)
{
      ಆಗ (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t ಮುಂದಿನ_val = cur->ಪೋರ್ಟ್[ptr.ಪೋರ್ಟ್()];

	    ಆದರೆ (cur->fun)
		  cur->fun->recv_string(ptr, val, context);

	    ptr = ಮುಂದಿನ_val;
      }
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ vvp_send_object(vvp_net_ptr_t ptr, vvp_object_t val, vvp_context_t context)
{
      ಆಗ (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t ಮುಂದಿನ_val = cur->ಪೋರ್ಟ್[ptr.ಪೋರ್ಟ್()];

	    ಆದರೆ (cur->fun)
		  cur->fun->recv_object(ptr, val, context);

	    ptr = ಮುಂದಿನ_val;
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
ಸಾಲೊಳಗೆ ಶೂನ್ಯ vvp_send_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val,
			     ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid,
			     vvp_context_t context)
{
      ಆಗ (class vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t ಮುಂದಿನ_val = cur->ಪೋರ್ಟ್[ptr.ಪೋರ್ಟ್()];

	    ಆದರೆ (cur->fun)
		  cur->fun->recv_vec4_pv(ptr, val, ತಳ, vwid, context);

	    ptr = ಮುಂದಿನ_val;
      }
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ vvp_send_vec8_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector8_t&val,
			     ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid)
{
      ಆಗ (class vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t ಮುಂದಿನ_val = cur->ಪೋರ್ಟ್[ptr.ಪೋರ್ಟ್()];

	    ಆದರೆ (cur->fun)
		  cur->fun->recv_vec8_pv(ptr, val, ತಳ, vwid);

	    ptr = ಮುಂದಿನ_val;
      }
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ vvp_net_t::send_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&val, vvp_context_t context)
{
      ಆದರೆ (fil == 0) {
	    vvp_send_vec4(ಹೊರಹೋಗು_2, val, context);
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_vector4_t rep;
      ಆಯ್ಕೆ (fil->filter_vec4(val, rep, 0, val.ಗಾತ್ರ_2())) {
	  ಸ್ಥಿತಿ vvp_net_fil_t::STOP:
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vvp_net_fil_t::PROP:
	    vvp_send_vec4(ಹೊರಹೋಗು_2, val, context);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vvp_net_fil_t::REPL:
	    vvp_send_vec4(ಹೊರಹೋಗು_2, rep, context);
	    ನಿಲ್ಲಿಸು;
      }
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ vvp_net_t::send_vec4_pv(ಸ್ಥಿರಾಂಕ vvp_vector4_t&val,
				    ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid,
				    vvp_context_t context)
{
      ಆದರೆ (fil == 0) {
	    vvp_send_vec4_pv(ಹೊರಹೋಗು_2, val, ತಳ, vwid, context);
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_vector4_t rep;
      ಆಯ್ಕೆ (fil->filter_vec4(val, rep, ತಳ, vwid)) {
	  ಸ್ಥಿತಿ vvp_net_fil_t::STOP:
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vvp_net_fil_t::PROP:
	    vvp_send_vec4_pv(ಹೊರಹೋಗು_2, val, ತಳ, vwid, context);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vvp_net_fil_t::REPL:
	    vvp_send_vec4_pv(ಹೊರಹೋಗು_2, rep, ತಳ, vwid, context);
	    ನಿಲ್ಲಿಸು;
      }
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ vvp_net_t::send_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&val)
{
      ಆದರೆ (fil == 0) {
	    vvp_send_vec8(ಹೊರಹೋಗು_2, val);
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_vector8_t rep;
      ಆಯ್ಕೆ (fil->filter_vec8(val, rep, 0, val.ಗಾತ್ರ_2())) {
	  ಸ್ಥಿತಿ vvp_net_fil_t::STOP:
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vvp_net_fil_t::PROP:
	    vvp_send_vec8(ಹೊರಹೋಗು_2, val);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vvp_net_fil_t::REPL:
	    vvp_send_vec8(ಹೊರಹೋಗು_2, rep);
	    ನಿಲ್ಲಿಸು;
      }
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ vvp_net_t::send_vec8_pv(ಸ್ಥಿರಾಂಕ vvp_vector8_t&val,
				    ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid)
{
      ಆದರೆ (fil == 0) {
	    vvp_send_vec8_pv(ಹೊರಹೋಗು_2, val, ತಳ, vwid);
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_vector8_t rep;
      ಆಯ್ಕೆ (fil->filter_vec8(val, rep, ತಳ, vwid)) {
	  ಸ್ಥಿತಿ vvp_net_fil_t::STOP:
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vvp_net_fil_t::PROP:
	    vvp_send_vec8_pv(ಹೊರಹೋಗು_2, val, ತಳ, vwid);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vvp_net_fil_t::REPL:
	    vvp_send_vec8_pv(ಹೊರಹೋಗು_2, rep, ತಳ, vwid);
	    ನಿಲ್ಲಿಸು;
      }
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ vvp_net_t::send_real(ದ್ವಿಗುಣ val, vvp_context_t context)
{
      ಆದರೆ (fil && ! fil->filter_real(val))
	    ಹಿಂತಿರುಗಿ;

      vvp_send_real(ಹೊರಹೋಗು_2, val, context);
}


ಸಾಲೊಳಗೆ ಶೂನ್ಯ vvp_net_t::send_string(ಸ್ಥಿರಾಂಕ std::string&val, vvp_context_t context)
{
      ಆದರೆ (fil && !fil->filter_string(val))
	    ಹಿಂತಿರುಗಿ;

      vvp_send_string(ಹೊರಹೋಗು_2, val, context);
}


ಸಾಲೊಳಗೆ ಶೂನ್ಯ vvp_net_t::send_object(vvp_object_t val, vvp_context_t context)
{
      ಆದರೆ (fil && ! fil->filter_object(val))
	    ಹಿಂತಿರುಗಿ;

      vvp_send_object(ಹೊರಹೋಗು_2, val, context);
}


ಸಾಲೊಳಗೆ bool vvp_net_fil_t::test_force_ಮುಖವಾಡ(ಚಿಹ್ನೆರಹಿತ ಬಿಟ್) ಸ್ಥಿರಾಂಕ
{
      ಆದರೆ (ಬಿಟ್ >= force_ಮುಖವಾಡ_2.ಗಾತ್ರ_2())
	    ಹಿಂತಿರುಗಿ false;
      ಆದರೆ (force_ಮುಖವಾಡ_2.ಮೌಲ್ಯ(ಬಿಟ್))
	    ಹಿಂತಿರುಗಿ true;
      ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ false;
}

ಸಾಲೊಳಗೆ bool vvp_net_fil_t::test_force_ಮುಖವಾಡ_is_zero(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{
      ಆದರೆ (force_ಮುಖವಾಡ_2.ಗಾತ್ರ_2() == 0)
	    ಹಿಂತಿರುಗಿ true;
      ಆದರೆ (force_ಮುಖವಾಡ_2.is_zero())
	    ಹಿಂತಿರುಗಿ true;
      ಹಿಂತಿರುಗಿ false;
}

/*
 * Undefine the ivl_alloc.h definitions so they don't leak out of this file.
 */
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ಸ್ಮರಣೆ_ಕೊಡು
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು realloc
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು __ivl_alloc_H

#ಆದರೆ_ಅಂತ್ಯ /* IVL_vvp_net_H */
