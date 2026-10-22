#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_vvp_net_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_vvp_net_H
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_IOSFWD
# include  <iosfwd>
#ਨਹੀਂਤਾਂ
# include  <iostream>
#ਜੇ_ਅੰਤ

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ**vvp_context_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ*vvp_context_ਵਸਤੂ_t;

ਸਤਰਅੰਦਰ vvp_context_t vvp_allocate_context(ਬਿਨਾਨਿਸ਼ਾਨੀ nitem)
{
      ਵਾਪਸ (vvp_context_t)ਸਮ੍ਰਿਤੀ_ਦਿਓ((2 + nitem) * ਆਕਾਰ(ਖਾਲੀ*));
}

ਸਤਰਅੰਦਰ vvp_context_t vvp_get_ਅਗਲਾ_context(vvp_context_t context)
{
      ਵਾਪਸ (vvp_context_t)context[0];
}

ਸਤਰਅੰਦਰ ਖਾਲੀ vvp_ਸੈੱਟ_ਅਗਲਾ_context(vvp_context_t context, vvp_context_t ਅਗਲਾ_2)
{
      context[0] = ਅਗਲਾ_2;
}

ਸਤਰਅੰਦਰ vvp_context_t vvp_get_stacked_context(vvp_context_t context)
{
      ਵਾਪਸ (vvp_context_t)context[1];
}

ਸਤਰਅੰਦਰ ਖਾਲੀ vvp_ਸੈੱਟ_stacked_context(vvp_context_t context, vvp_context_t ਢੇਰ_2)
{
      context[1] = ਢੇਰ_2;
}

ਸਤਰਅੰਦਰ vvp_context_ਵਸਤੂ_t vvp_get_context_ਵਸਤੂ(vvp_context_t context,
                                               ਬਿਨਾਨਿਸ਼ਾਨੀ ਵਸਤੂ_idx)
{
      ਵਾਪਸ (vvp_context_ਵਸਤੂ_t)context[ਵਸਤੂ_idx];
}

ਸਤਰਅੰਦਰ ਖਾਲੀ vvp_ਸੈੱਟ_context_ਵਸਤੂ(vvp_context_t context, ਬਿਨਾਨਿਸ਼ਾਨੀ ਵਸਤੂ_idx,
                                 vvp_context_ਵਸਤੂ_t ਵਸਤੂ)
{
      context[ਵਸਤੂ_idx] = ਵਸਤੂ;
}

/*
 * An "automatic" functor is one which may be associated with an automatically
 * allocated scope item. This provides the infrastructure needed to allocate
 * the state information for individual instances of the item.
 */
ਬਣਤਰ automatic_hooks_s {

      automatic_hooks_s() {}
      ਆਭਾਸੀ ~automatic_hooks_s() {}

      ਆਭਾਸੀ ਖਾਲੀ alloc_instance(vvp_context_t context) = 0;
      ਆਭਾਸੀ ਖਾਲੀ reset_instance(vvp_context_t context) = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਆਭਾਸੀ ਖਾਲੀ free_instance(vvp_context_t context) = 0;
#ਜੇ_ਅੰਤ
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
ਗਿਣਤੀ vvp_bit4_t {
      BIT4_0 = 0,
      BIT4_1 = 1,
      BIT4_X = 3,
      BIT4_Z = 2
};

  /* Return an ASCII character that represents the vvp_bit4_t value. */
ਸਤਰਅੰਦਰ ਅੱਖਰ vvp_bit4_ਮੰਜ਼ਿਲ_ascii(vvp_bit4_t a) { ਵਾਪਸ "01zx"[a]; }

ਬਾਹਰੀ vvp_bit4_t ਜੋੜੋ_with_ਹਾਸਲ(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c);

ਬਾਹਰੀ vvp_bit4_t scalar_ਮੰਜ਼ਿਲ_bit4(PLI_INT32 scalar);

  /* Return TRUE if the bit is BIT4_X or BIT4_Z. The fast
     implementation here relies on the encoding of vvp_bit4_t values. */
ਸਤਰਅੰਦਰ bool bit4_is_xz(vvp_bit4_t a) { ਵਾਪਸ a >= 2; }

  /* This function converts BIT4_Z to BIT4_X, but passes other values
     unchanged. This fast implementation relies of the encoding of the
     vvp_bit4_t values. In particular, the BIT4_X==3 and BIT4_Z==2 */
ਸਤਰਅੰਦਰ vvp_bit4_t bit4_z2x(vvp_bit4_t a)
{ ਵਾਪਸ (vvp_bit4_t) ( (ਪੂਰਨ)a | ((ਪੂਰਨ)a >> 1) ); }

  /* Some common boolean operators. These implement the Verilog rules
     for 4-value bit operations. The fast implementations here rely
     on the encoding of vvp_bit4_t values. */

  // ~BIT4_0 --> BIT4_1
  // ~BIT4_1 --> BIT4_0
  // ~BIT4_X --> BIT4_X
  // ~BIT4_Z --> BIT4_X
ਸਤਰਅੰਦਰ vvp_bit4_t ਸੰਚਾਲਕ ~ (vvp_bit4_t a)
{ ਵਾਪਸ bit4_z2x((vvp_bit4_t) (((ਪੂਰਨ)a) ^ 1)); }

ਸਤਰਅੰਦਰ vvp_bit4_t ਸੰਚਾਲਕ | (vvp_bit4_t a, vvp_bit4_t b)
{
      ਜੇ (a==BIT4_1 || b==BIT4_1)
	    ਵਾਪਸ BIT4_1;
      ਵਾਪਸ bit4_z2x( (vvp_bit4_t) ((ਪੂਰਨ)a | (ਪੂਰਨ)b) );
}

ਸਤਰਅੰਦਰ vvp_bit4_t ਸੰਚਾਲਕ & (vvp_bit4_t a, vvp_bit4_t b)
{
      ਜੇ (a==BIT4_0 || b==BIT4_0)
	    ਵਾਪਸ BIT4_0;
      ਵਾਪਸ bit4_z2x( (vvp_bit4_t) ((ਪੂਰਨ)a | (ਪੂਰਨ)b) );
}


ਬਾਹਰੀ vvp_bit4_t ਸੰਚਾਲਕ ^ (vvp_bit4_t a, vvp_bit4_t b);
ਬਾਹਰੀ std::ostream& ਸੰਚਾਲਕ<< (std::ostream&o, vvp_bit4_t a);

  /* Return >0, ==0 or <0 if the from-to transition represents a
     posedge, no edge, or negedge. */
ਬਾਹਰੀ ਪੂਰਨ ਕਿਨਾਰਾ(vvp_bit4_t from, vvp_bit4_t ਮੰਜ਼ਿਲ);

  /* Support for $countdrivers. */
ਸਤਰਅੰਦਰ ਖਾਲੀ update_driver_counts(vvp_bit4_t ਦੋਆਧਾਰੀ_ਅੰਕ, ਬਿਨਾਨਿਸ਼ਾਨੀ counts[3])
{
      ਚੋਣ (ਦੋਆਧਾਰੀ_ਅੰਕ) {
          ਹਾਲਤ BIT4_0:
            counts[0] += 1;
            ਰੋਕੋ;
          ਹਾਲਤ BIT4_1:
            counts[1] += 1;
            ਰੋਕੋ;
          ਹਾਲਤ BIT4_X:
            counts[2] += 1;
            ਰੋਕੋ;
          ਮੂਲ:
            ਰੋਕੋ;
      }
}

/*
 * Some of the instructions do wide addition to arrays of long. They
 * use this add_with_carry function to help.
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਜੋੜੋ_with_ਹਾਸਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ a, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ b,
					   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ&ਹਾਸਲ)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp = b + ਹਾਸਲ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਜੋੜ = a + tmp;
      ਹਾਸਲ = 0;
      ਜੇ (tmp < b)
	    ਹਾਸਲ = 1;
      ਜੇ (ਜੋੜ < tmp)
	    ਹਾਸਲ = 1;
      ਜੇ (ਜੋੜ < a)
	    ਹਾਸਲ = 1;
      ਵਾਪਸ ਜੋੜ;
}

ਬਾਹਰੀ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ multiply_with_ਹਾਸਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ a, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ b,
					 ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ&ਹਾਸਲ);

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

      ਮਿੱਤਰ vvp_vector4_t ਸੰਚਾਲਕ ~(ਅਚਲ vvp_vector4_t&that);
      ਮਿੱਤਰ class vvp_vector4array_t;
      ਮਿੱਤਰ class vvp_vector4array_sa;
      ਮਿੱਤਰ class vvp_vector4array_aa;

    ਜਨਤਕ:
      ਸਥਿਰ ਅਚਲ vvp_vector4_t nil;

    ਜਨਤਕ:
      explicit vvp_vector4_t(ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ =0, vvp_bit4_t ਬਿੱਟ =BIT4_X);

      explicit vvp_vector4_t(ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ, ਦੁੱਗਣਾ val);

	// Construct a vector4 from the subvalue of another
	// vector4. The width of the result is 'wid', and the bits are
	// pulled from 'that' to implement the Verilog part select
	// semantics. This means that part select beyond 'that'
	// returns X bits.
      explicit vvp_vector4_t(ਅਚਲ vvp_vector4_t&that,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਬਿਨਾਨਿਸ਼ਾਨੀ wid);

      vvp_vector4_t(ਅਚਲ vvp_vector4_t&that);
      vvp_vector4_t(ਅਚਲ vvp_vector4_t&that, bool invert_flag);
      vvp_vector4_t& ਸੰਚਾਲਕ= (ਅਚਲ vvp_vector4_t&that);

      ~vvp_vector4_t();

      ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ() ਅਚਲ { ਵਾਪਸ ਮਾਪ_2; }
      ਖਾਲੀ resize(ਬਿਨਾਨਿਸ਼ਾਨੀ new_ਮਾਪ, vvp_bit4_t pad_ਦੋਆਧਾਰੀ_ਅੰਕ = BIT4_X);

	// Get the bit at the specified address
      vvp_bit4_t ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ;
	// Get the vector4 subvector starting at the address
      vvp_vector4_t subvalue(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ) ਅਚਲ;
	// Get the 2-value bits for the subvector. This returns a new
	// array of longs, or a nil pointer if an XZ bit was detected
	// in the array.
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*subarray(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ, bool xz_ਮੰਜ਼ਿਲ_0 =false) ਅਚਲ;
      ਖਾਲੀ setarray(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*val);

	// Set a 4-value bit or subvector into the vector. Return true
	// if any bits of the vector change as a result of this operation.
      ਖਾਲੀ ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, vvp_bit4_t val);
      bool ਸੈੱਟ_vec(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਅਚਲ vvp_vector4_t&that);

        // Get the bits from another vector, but keep my size.
      ਖਾਲੀ copy_ਬਿੱਟ(ਅਚਲ vvp_vector4_t&that);

	// Move bits within this vector.
      ਖਾਲੀ mov(ਬਿਨਾਨਿਸ਼ਾਨੀ dst, ਬਿਨਾਨਿਸ਼ਾਨੀ src, ਬਿਨਾਨਿਸ਼ਾਨੀ cnt);

	// Add that to this in the Verilog way.
      ਖਾਲੀ ਜੋੜੋ(ਅਚਲ vvp_vector4_t&that);

	// Subtract that from this in the Verilog way.
      ਖਾਲੀ sub(ਅਚਲ vvp_vector4_t&that);

	// Multiply this by that in the Verilog way.
      ਖਾਲੀ mul(ਅਚਲ vvp_vector4_t&that);

	// Test that the vectors are exactly equal
      bool eeq(ਅਚਲ vvp_vector4_t&that) ਅਚਲ;

	// Test that the vectors are equal, with xz comparing as equal.
      bool eq_xz(ਅਚਲ vvp_vector4_t&that) ਅਚਲ;

	// Return true if there is an X or Z anywhere in the vector.
      bool has_xz() ਅਚਲ;

	// Change all Z bits to X bits.
      ਖਾਲੀ change_z2x();

	// Set all bits to the specified value.
      ਖਾਲੀ fill_ਬਿੱਟ(vvp_bit4_t ਦੋਆਧਾਰੀ_ਅੰਕ);

	// Change all bits to X bits.
      ਖਾਲੀ ਸੈੱਟ_ਮੰਜ਼ਿਲ_x() { fill_ਬਿੱਟ(BIT4_X); }

	// Display the value into the buf as a string.
      ਅੱਖਰ*as_string(ਅੱਖਰ*buf, size_t buf_len) ਅਚਲ;

      ਖਾਲੀ invert();
      vvp_vector4_t& ਸੰਚਾਲਕ &= (ਅਚਲ vvp_vector4_t&that);
      vvp_vector4_t& ਸੰਚਾਲਕ |= (ਅਚਲ vvp_vector4_t&that);
      vvp_vector4_t& ਸੰਚਾਲਕ ^= (ਅਚਲ vvp_vector4_t&that);
      vvp_vector4_t& ਸੰਚਾਲਕ += (int64_t);

      vvp_bit4_t reduce_or() ਅਚਲ;
      vvp_bit4_t reduce_and() ਅਚਲ;
      vvp_bit4_t reduce_xor() ਅਚਲ;

    ਨਿਜੀ:
	// Number of vvp_bit4_t bits that can be shoved into a word.
      ਗਿਣਤੀ { ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ = 8*ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) };
	// The double value constructor requires that WORD_0_BBITS
	// and WORD_1_BBITS have the same value!
#ਜੇ SIZEOF_UNSIGNED_LONG == 8
      ਗਿਣਤੀ { ਸ਼ਬਦ_0_ABITS = 0x0000000000000000UL,
	     ਸ਼ਬਦ_0_BBITS = 0x0000000000000000UL };
      ਗਿਣਤੀ { ਸ਼ਬਦ_1_ABITS = 0xFFFFFFFFFFFFFFFFUL,
	     ਸ਼ਬਦ_1_BBITS = 0x0000000000000000UL };
      ਗਿਣਤੀ { ਸ਼ਬਦ_X_ABITS = 0xFFFFFFFFFFFFFFFFUL,
	     ਸ਼ਬਦ_X_BBITS = 0xFFFFFFFFFFFFFFFFUL };
      ਗਿਣਤੀ { ਸ਼ਬਦ_Z_ABITS = 0x0000000000000000UL,
             ਸ਼ਬਦ_Z_BBITS = 0xFFFFFFFFFFFFFFFFUL };
#ਜਾਂਜੇ SIZEOF_UNSIGNED_LONG == 4
      ਗਿਣਤੀ { ਸ਼ਬਦ_0_ABITS = 0x00000000UL, ਸ਼ਬਦ_0_BBITS = 0x00000000UL };
      ਗਿਣਤੀ { ਸ਼ਬਦ_1_ABITS = 0xFFFFFFFFUL, ਸ਼ਬਦ_1_BBITS = 0x00000000UL };
      ਗਿਣਤੀ { ਸ਼ਬਦ_X_ABITS = 0xFFFFFFFFUL, ਸ਼ਬਦ_X_BBITS = 0xFFFFFFFFUL };
      ਗਿਣਤੀ { ਸ਼ਬਦ_Z_ABITS = 0x00000000UL, ਸ਼ਬਦ_Z_BBITS = 0xFFFFFFFFUL };
#ਨਹੀਂਤਾਂ
#ਗਲਤੀ "WORD_X_xBITS not defined for this architecture?"
#ਜੇ_ਅੰਤ

	// Initialize and operator= use this private method to copy
	// the data from that object into this object.
      ਖਾਲੀ copy_from_(ਅਚਲ vvp_vector4_t&that);
      ਖਾਲੀ copy_from_big_(ਅਚਲ vvp_vector4_t&that);
      ਖਾਲੀ copy_inverted_from_(ਅਚਲ vvp_vector4_t&that);

      ਖਾਲੀ allocate_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ inita, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ initb);

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

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ_2;
      ਸੰਘ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ abits_val_;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*abits_ptr_;
      };
      ਸੰਘ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ bbits_val_;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*bbits_ptr_;
      };
};

ਸਤਰਅੰਦਰ vvp_vector4_t::vvp_vector4_t(ਅਚਲ vvp_vector4_t&that)
{
      copy_from_(that);
}

ਸਤਰਅੰਦਰ vvp_vector4_t::vvp_vector4_t(ਅਚਲ vvp_vector4_t&that, bool invert_flag)
{
      ਜੇ (invert_flag)
	    copy_inverted_from_(that);
      ਨਹੀਂਤਾਂ
	    copy_from_(that);
}

ਸਤਰਅੰਦਰ vvp_vector4_t::vvp_vector4_t(ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ_3, vvp_bit4_t val)
: ਮਾਪ_2(ਮਾਪ_3)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ init_atable[4] = {
	    ਸ਼ਬਦ_0_ABITS,
	    ਸ਼ਬਦ_1_ABITS,
	    ਸ਼ਬਦ_Z_ABITS,
	    ਸ਼ਬਦ_X_ABITS };
      ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ init_btable[4] = {
	    ਸ਼ਬਦ_0_BBITS,
	    ਸ਼ਬਦ_1_BBITS,
	    ਸ਼ਬਦ_Z_BBITS,
	    ਸ਼ਬਦ_X_BBITS };

      allocate_ਸ਼ਬਦ(init_atable[val], init_btable[val]);
}

ਸਤਰਅੰਦਰ vvp_vector4_t::~vvp_vector4_t()
{
      ਜੇ (ਮਾਪ_2 > ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    delete[] abits_ptr_;
	      // bbits_ptr_ actually points half-way into a
	      // double-length array started at abits_ptr_
      }
}

ਸਤਰਅੰਦਰ vvp_vector4_t& vvp_vector4_t::ਸੰਚਾਲਕ= (ਅਚਲ vvp_vector4_t&that)
{
      ਜੇ (this == &that)
	    ਵਾਪਸ *this;

      ਜੇ (ਮਾਪ_2 > ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ)
	    delete[] abits_ptr_;

      copy_from_(that);

      ਵਾਪਸ *this;
}

ਸਤਰਅੰਦਰ ਖਾਲੀ vvp_vector4_t::copy_from_(ਅਚਲ vvp_vector4_t&that)
{
      ਮਾਪ_2 = that.ਮਾਪ_2;
      ਜੇ (ਮਾਪ_2 <= ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;
      } ਨਹੀਂਤਾਂ {
	    copy_from_big_(that);
      }
}

ਸਤਰਅੰਦਰ vvp_bit4_t vvp_vector4_t::ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ
{
      ਜੇ (idx >= ਮਾਪ_2)
	    ਵਾਪਸ BIT4_X;

      ਬਿਨਾਨਿਸ਼ਾਨੀ off;

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ abits, bbits;
      ਜੇ (ਮਾਪ_2 > ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ wdx = idx / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    off = idx % ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    abits = abits_ptr_[wdx];
	    bbits = bbits_ptr_[wdx];
      } ਨਹੀਂਤਾਂ {
	    off = idx;
	    abits = abits_val_;
	    bbits = bbits_val_;
      }

      abits >>= off;
      bbits >>= off;
      ਪੂਰਨ tmp = ((bbits&1) << 1) + (abits&1);
	// This cast works since b==1,a==1 is X and b==1,a==0 is Z.
      ਵਾਪਸ (vvp_bit4_t)tmp;
}

ਸਤਰਅੰਦਰ vvp_vector4_t vvp_vector4_t::subvalue(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਬਿਨਾਨਿਸ਼ਾਨੀ wid) ਅਚਲ
{
      ਵਾਪਸ vvp_vector4_t(*this, adr, wid);
}

ਸਤਰਅੰਦਰ ਖਾਲੀ vvp_vector4_t::ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, vvp_bit4_t val)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(idx < ਮਾਪ_2);

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ off = idx % ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = 1UL << off;

      ਜੇ (ਮਾਪ_2 > ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ wdx = idx / ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ;
	    ਚੋਣ (val) {
		ਹਾਲਤ BIT4_0:
		  abits_ptr_[wdx] &= ~ਨਕਾਬ;
		  bbits_ptr_[wdx] &= ~ਨਕਾਬ;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_1:
		  abits_ptr_[wdx] |=  ਨਕਾਬ;
		  bbits_ptr_[wdx] &= ~ਨਕਾਬ;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_X:
		  abits_ptr_[wdx] |=  ਨਕਾਬ;
		  bbits_ptr_[wdx] |=  ਨਕਾਬ;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_Z:
		  abits_ptr_[wdx] &= ~ਨਕਾਬ;
		  bbits_ptr_[wdx] |=  ਨਕਾਬ;
		  ਰੋਕੋ;
	    }
      } ਨਹੀਂਤਾਂ {
	    ਚੋਣ (val) {
		ਹਾਲਤ BIT4_0:
		  abits_val_ &= ~ਨਕਾਬ;
		  bbits_val_ &= ~ਨਕਾਬ;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_1:
		  abits_val_ |=  ਨਕਾਬ;
		  bbits_val_ &= ~ਨਕਾਬ;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_X:
		  abits_val_ |=  ਨਕਾਬ;
		  bbits_val_ |=  ਨਕਾਬ;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_Z:
		  abits_val_ &= ~ਨਕਾਬ;
		  bbits_val_ |=  ਨਕਾਬ;
		  ਰੋਕੋ;
	    }
      }
}

ਸਤਰਅੰਦਰ vvp_vector4_t ਸੰਚਾਲਕ ~ (ਅਚਲ vvp_vector4_t&that)
{
      vvp_vector4_t res (that, true);
      ਵਾਪਸ res;
}

ਬਾਹਰੀ std::ostream& ਸੰਚਾਲਕ << (std::ostream&, ਅਚਲ vvp_vector4_t&);

ਬਾਹਰੀ vvp_bit4_t compare_gtge(ਅਚਲ vvp_vector4_t&a,
			       ਅਚਲ vvp_vector4_t&b,
			       vvp_bit4_t val_if_equal);
ਬਾਹਰੀ vvp_bit4_t compare_gtge_signed(ਅਚਲ vvp_vector4_t&a,
				      ਅਚਲ vvp_vector4_t&b,
				      vvp_bit4_t val_if_equal);
ਸਾਂਚਾ <class T> ਬਾਹਰੀ T coerce_ਮੰਜ਼ਿਲ_ਚੌੜਾਈ(ਅਚਲ T&that, ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ);

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
ਸਾਂਚਾ <class T> ਬਾਹਰੀ bool vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਅਚਲ vvp_vector4_t&a, T&val,
						bool is_signed,
						bool is_arithmetic =true);

ਸਾਂਚਾ <class T> ਬਾਹਰੀ bool vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਅਚਲ vvp_vector4_t&a,
                                                bool&ਉਛਾਲ_flag, T&val);

ਸਾਂਚਾ <class T> ਸਤਰਅੰਦਰ bool vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਅਚਲ vvp_vector4_t&a, T&val)
{
      bool ਉਛਾਲ_flag;
      ਵਾਪਸ vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(a, ਉਛਾਲ_flag, val);
}

ਬਾਹਰੀ bool vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਅਚਲ vvp_vector4_t&a, ਦੁੱਗਣਾ&val, bool is_signed);

ਬਾਹਰੀ bool vector2_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਅਚਲ vvp_vector2_t&a, int32_t&val, bool is_signed);

ਬਾਹਰੀ vvp_vector4_t vector4_from_ਪਾਠ(ਅਚਲ ਅੱਖਰ*ਬਿੱਟ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid);

ਸਤਰਅੰਦਰ vvp_vector4_t double_ਮੰਜ਼ਿਲ_vector4_LSB(ਦੁੱਗਣਾ real)
{
      // Convert the double to a bit vector and then use the LSB.
      // You can use very large real values and break the 64-bit
      // size, but that's really horrible code so don't do that!
      vvp_vector4_t ਦੋਆਧਾਰੀ_ਅੰਕ = vvp_vector4_t(64, real);
      ਦੋਆਧਾਰੀ_ਅੰਕ.resize(1);
      ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ;
}

/*
 * vvp_vector4array_t
 */
class vvp_vector4array_t {

    ਜਨਤਕ:
      vvp_vector4array_t(ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ);
      ਆਭਾਸੀ ~vvp_vector4array_t();

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ() ਅਚਲ { ਵਾਪਸ ਚੌੜਾਈ_2; }
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ() ਅਚਲ { ਵਾਪਸ ਸ਼ਬਦ_4; }

      ਆਭਾਸੀ vvp_vector4_t get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ = 0;
      ਆਭਾਸੀ ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਅਚਲ vvp_vector4_t&that) = 0;

    ਸੁਰੱਖਿਅਤ:
      ਬਣਤਰ v4cell {
	    ਸੰਘ {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ abits_val_;
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*abits_ptr_;
	    };
	    ਸੰਘ {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ bbits_val_;
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*bbits_ptr_;
	    };
      };

      vvp_vector4_t get_ਸ਼ਬਦ_2(v4cell*cell) ਅਚਲ;
      ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ_2(v4cell*cell, ਅਚਲ vvp_vector4_t&that);

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ_2;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ_4;

    ਨਿਜੀ: // Not implemented
      vvp_vector4array_t(ਅਚਲ vvp_vector4array_t&);
      vvp_vector4array_t& ਸੰਚਾਲਕ = (ਅਚਲ vvp_vector4array_t&);
};

/*
 * Statically allocated vvp_vector4array_t
 */
class vvp_vector4array_sa : ਜਨਤਕ vvp_vector4array_t {

    ਜਨਤਕ:
      vvp_vector4array_sa(ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ);
      ~vvp_vector4array_sa() override;

      vvp_vector4_t get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ override;
      ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਅਚਲ vvp_vector4_t&that) override;

    ਨਿਜੀ:
      v4cell* array_;
};

/*
 * Automatically allocated vvp_vector4array_t
 */
class vvp_vector4array_aa : ਜਨਤਕ vvp_vector4array_t, ਜਨਤਕ automatic_hooks_s {

    ਜਨਤਕ:
      vvp_vector4array_aa(ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ);
      ~vvp_vector4array_aa() override;

      ਖਾਲੀ alloc_instance(vvp_context_t context) override;
      ਖਾਲੀ reset_instance(vvp_context_t context) override;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਖਾਲੀ free_instance(vvp_context_t context) override;
#ਜੇ_ਅੰਤ

      vvp_vector4_t get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ override;
      ਖਾਲੀ ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਅਚਲ vvp_vector4_t&that) override;

    ਨਿਜੀ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ context_idx_;
};

/* vvp_vector2_t
 */
class vvp_vector2_t {

      ਮਿੱਤਰ vvp_vector2_t ਸੰਚਾਲਕ - (ਅਚਲ vvp_vector2_t&);
      ਮਿੱਤਰ vvp_vector2_t ਸੰਚਾਲਕ + (ਅਚਲ vvp_vector2_t&,
				       ਅਚਲ vvp_vector2_t&);
      ਮਿੱਤਰ vvp_vector2_t ਸੰਚਾਲਕ * (ਅਚਲ vvp_vector2_t&,
				       ਅਚਲ vvp_vector2_t&);
      ਮਿੱਤਰ bool ਸੰਚਾਲਕ >  (ਅਚਲ vvp_vector2_t&, ਅਚਲ vvp_vector2_t&);
      ਮਿੱਤਰ bool ਸੰਚਾਲਕ >= (ਅਚਲ vvp_vector2_t&, ਅਚਲ vvp_vector2_t&);
      ਮਿੱਤਰ bool ਸੰਚਾਲਕ <  (ਅਚਲ vvp_vector2_t&, ਅਚਲ vvp_vector2_t&);
      ਮਿੱਤਰ bool ਸੰਚਾਲਕ <= (ਅਚਲ vvp_vector2_t&, ਅਚਲ vvp_vector2_t&);
      ਮਿੱਤਰ bool ਸੰਚਾਲਕ == (ਅਚਲ vvp_vector2_t&, ਅਚਲ vvp_vector2_t&);

    ਜਨਤਕ:
      vvp_vector2_t();
      vvp_vector2_t(ਅਚਲ vvp_vector2_t&);
      vvp_vector2_t(ਅਚਲ vvp_vector2_t&, ਬਿਨਾਨਿਸ਼ਾਨੀ newsize);
      vvp_vector2_t(ਅਚਲ vvp_vector2_t&, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
	// Make a vvp_vector2_t from a vvp_vector4_t. If enable_NaN
	// is true and there are X or Z bits, then the result becomes
	// a NaN value. If enable_NaN is false then X or Z bits are
	// converted to 0 as per the standard Verilog rules.
      explicit vvp_vector2_t(ਅਚਲ vvp_vector4_t&that, bool allow_NaN = false);
	// Make from a 64-bit value and a specified width.
      vvp_vector2_t(uint64_t val, ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
	// Make with the width, and filled with 1 or 0 bits.
      ਗਿਣਤੀ fill_t {FILL0, FILL1};
      vvp_vector2_t(fill_t fill, ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ~vvp_vector2_t();

      vvp_vector2_t&ਸੰਚਾਲਕ += (ਅਚਲ vvp_vector2_t&that);
      vvp_vector2_t&ਸੰਚਾਲਕ -= (ਅਚਲ vvp_vector2_t&that);
      vvp_vector2_t&ਸੰਚਾਲਕ <<= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਖਿਸਕਾਓ);
      vvp_vector2_t&ਸੰਚਾਲਕ >>= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਖਿਸਕਾਓ);
      vvp_vector2_t&ਸੰਚਾਲਕ = (ਅਚਲ vvp_vector2_t&);

	// Assign a vec4 to a vec2, using Verilog rules for converting
	// XZ values to 0.
      vvp_vector2_t&ਸੰਚਾਲਕ = (ਅਚਲ vvp_vector4_t&);

      bool is_NaN() ਅਚਲ;
      bool is_zero() ਅਚਲ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ() ਅਚਲ;
      ਪੂਰਨ ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ;
      vvp_bit4_t value4(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ;
	// Get the vector2 subvector starting at the address
      vvp_vector2_t subvalue(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ) ਅਚਲ;
      ਖਾਲੀ ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਪੂਰਨ ਦੋਆਧਾਰੀ_ਅੰਕ);
      ਖਾਲੀ ਸੈੱਟ_vec(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਅਚਲ vvp_vector2_t&that);
	// Make the size just big enough to hold the first 1 bit.
      ਖਾਲੀ trim();
	// Trim off extra 1 bit since this is representing a negative value.
	// Always keep at least 32 bits.
      ਖਾਲੀ trim_neg();

    ਨਿਜੀ:
      ਗਿਣਤੀ { ਬਿੱਟ_ਪ੍ਰਤੀ_ਸ਼ਬਦ = 8 * ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) };
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*vec_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid_;

    ਨਿਜੀ:
      ਖਾਲੀ copy_from_that_(ਅਚਲ vvp_vector2_t&that);
      ਖਾਲੀ copy_from_that_(ਅਚਲ vvp_vector4_t&that);
};

ਬਾਹਰੀ bool ਸੰਚਾਲਕ >  (ਅਚਲ vvp_vector2_t&, ਅਚਲ vvp_vector2_t&);
ਬਾਹਰੀ bool ਸੰਚਾਲਕ >= (ਅਚਲ vvp_vector2_t&, ਅਚਲ vvp_vector2_t&);
ਬਾਹਰੀ bool ਸੰਚਾਲਕ <  (ਅਚਲ vvp_vector2_t&, ਅਚਲ vvp_vector2_t&);
ਬਾਹਰੀ bool ਸੰਚਾਲਕ <= (ਅਚਲ vvp_vector2_t&, ਅਚਲ vvp_vector2_t&);
ਬਾਹਰੀ bool ਸੰਚਾਲਕ == (ਅਚਲ vvp_vector2_t&, ਅਚਲ vvp_vector2_t&);
ਬਾਹਰੀ vvp_vector2_t ਸੰਚਾਲਕ - (ਅਚਲ vvp_vector2_t&);
ਬਾਹਰੀ vvp_vector2_t ਸੰਚਾਲਕ + (ਅਚਲ vvp_vector2_t&, ਅਚਲ vvp_vector2_t&);
ਬਾਹਰੀ vvp_vector2_t ਸੰਚਾਲਕ * (ਅਚਲ vvp_vector2_t&, ਅਚਲ vvp_vector2_t&);
ਬਾਹਰੀ vvp_vector2_t ਸੰਚਾਲਕ / (ਅਚਲ vvp_vector2_t&, ਅਚਲ vvp_vector2_t&);
ਬਾਹਰੀ vvp_vector2_t ਸੰਚਾਲਕ % (ਅਚਲ vvp_vector2_t&, ਅਚਲ vvp_vector2_t&);

vvp_vector2_t ਘਾਤ(ਅਚਲ vvp_vector2_t&, vvp_vector2_t&);
ਬਾਹਰੀ vvp_vector4_t vector2_ਮੰਜ਼ਿਲ_vector4(ਅਚਲ vvp_vector2_t&, ਬਿਨਾਨਿਸ਼ਾਨੀ wid);

/* A c4string is of the form C4<...> where ... are bits. */
ਬਾਹਰੀ bool c4string_test(ਅਚਲ ਅੱਖਰ*str);
ਬਾਹਰੀ vvp_vector4_t c4string_ਮੰਜ਼ਿਲ_vector4(ਅਚਲ ਅੱਖਰ*str);

/* A crstring is of the form Cr<...> where ... defines are real. */
ਬਾਹਰੀ bool crstring_test(ਅਚਲ ਅੱਖਰ*str);
ਬਾਹਰੀ ਦੁੱਗਣਾ crstring_ਮੰਜ਼ਿਲ_double(ਅਚਲ ਅੱਖਰ*str);

ਬਾਹਰੀ std::ostream& ਸੰਚਾਲਕ<< (std::ostream&, ਅਚਲ vvp_vector2_t&);

ਸਤਰਅੰਦਰ vvp_vector2_t::vvp_vector2_t(ਅਚਲ vvp_vector2_t&that)
{
      copy_from_that_(that);
}

ਸਤਰਅੰਦਰ vvp_vector2_t::vvp_vector2_t(ਅਚਲ vvp_vector4_t&that, bool enable_NaN)
{
      ਜੇ (enable_NaN && that.has_xz()) {
	    vec_ = 0;
	    wid_ = 0;
	    ਵਾਪਸ;
      }
      copy_from_that_(that);
}

ਸਤਰਅੰਦਰ vvp_vector2_t::~vvp_vector2_t()
{
      delete[] vec_;
}

/* Inline some of the vector2_t methods. */
ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ vvp_vector2_t::ਮਾਪ() ਅਚਲ
{
      ਵਾਪਸ wid_;
}

ਸਤਰਅੰਦਰ vvp_bit4_t vvp_vector2_t::value4(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ
{
      ਜੇ (ਮੁੱਲ(idx))
	    ਵਾਪਸ BIT4_1;
      ਨਹੀਂਤਾਂ
	    ਵਾਪਸ BIT4_0;
}

ਸਤਰਅੰਦਰ vvp_vector2_t vvp_vector2_t::subvalue(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid) ਅਚਲ
{
      ਵਾਪਸ vvp_vector2_t(*this, ਅਧਾਰ, wid);
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

      ਮਿੱਤਰ vvp_scalar_t fully_featured_resolv_(vvp_scalar_t a, vvp_scalar_t b);

    ਜਨਤਕ:
	// Make a HiZ value.
      explicit vvp_scalar_t();

	// Make an unambiguous value.
      explicit vvp_scalar_t(vvp_bit4_t val, ਬਿਨਾਨਿਸ਼ਾਨੀ str0, ਬਿਨਾਨਿਸ਼ਾਨੀ str1);

	// Get the vvp_bit4_t version of the value
      vvp_bit4_t ਮੁੱਲ() ਅਚਲ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ strength0() ਅਚਲ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ strength1() ਅਚਲ;

      bool eeq(vvp_scalar_t that) ਅਚਲ { ਵਾਪਸ ਮੁੱਲ_2 == that.ਮੁੱਲ_2; }
      bool is_hiz() ਅਚਲ { ਵਾਪਸ (ਮੁੱਲ_2 & 0x77) == 0; }

    ਨਿਜੀ:
	// This class and the vvp_vector8_t class are closely related,
	// so allow vvp_vector8_t access to the raw encoding so that
	// it can do compact vectoring of vvp_scalar_t objects.
      ਮਿੱਤਰ class vvp_vector8_t;
      explicit vvp_scalar_t(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ val) : ਮੁੱਲ_2(val) { }
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ raw() ਅਚਲ { ਵਾਪਸ ਮੁੱਲ_2; }

    ਨਿਜੀ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਮੁੱਲ_2;
};

ਸਤਰਅੰਦਰ vvp_scalar_t::vvp_scalar_t()
{
      ਮੁੱਲ_2 = 0;
}

ਸਤਰਅੰਦਰ vvp_scalar_t::vvp_scalar_t(vvp_bit4_t val, ਬਿਨਾਨਿਸ਼ਾਨੀ str0, ਬਿਨਾਨਿਸ਼ਾਨੀ str1)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(str0 <= 7);
      ਪ੍ਰਤਿਜ੍ਞਾ(str1 <= 7);

      ਜੇ (str0 == 0 && str1 == 0) {
	    ਮੁੱਲ_2 = 0x00;
      } ਨਹੀਂਤਾਂ ਚੋਣ (val) {
	  ਹਾਲਤ BIT4_0:
	    ਮੁੱਲ_2 = str0 | (str0<<4);
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_1:
	    ਮੁੱਲ_2 = str1 | (str1<<4) | 0x88;
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_X:
	    ਮੁੱਲ_2 = str0 | (str1<<4) | 0x80;
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_Z:
	    ਮੁੱਲ_2 = 0x00;
	    ਰੋਕੋ;
      }
}

ਸਤਰਅੰਦਰ vvp_bit4_t vvp_scalar_t::ਮੁੱਲ() ਅਚਲ
{
      ਜੇ ((ਮੁੱਲ_2 & 0x77) == 0) {
	    ਵਾਪਸ BIT4_Z;
      } ਨਹੀਂਤਾਂ ਚੋਣ (ਮੁੱਲ_2 & 0x88) {
	  ਹਾਲਤ 0x00:
	    ਵਾਪਸ BIT4_0;
	  ਹਾਲਤ 0x88:
	    ਵਾਪਸ BIT4_1;
	  ਮੂਲ:
	    ਵਾਪਸ BIT4_X;
      }
}


ਸਤਰਅੰਦਰ vvp_scalar_t resolve(vvp_scalar_t a, vvp_scalar_t b)
{
      ਬਾਹਰੀ vvp_scalar_t fully_featured_resolv_(vvp_scalar_t, vvp_scalar_t);

	// If the value is HiZ, resolution is simply a matter of
	// returning the *other* value.
      ਜੇ (a.is_hiz())
	    ਵਾਪਸ b;
      ਜੇ (b.is_hiz())
	    ਵਾਪਸ a;
	// If the values are the identical, then resolution is simply
	// returning *either* value.
      ਜੇ (a .eeq( b ))
	    ਵਾਪਸ a;

      ਵਾਪਸ fully_featured_resolv_(a,b);
}

ਬਾਹਰੀ std::ostream& ਸੰਚਾਲਕ<< (std::ostream&, vvp_scalar_t);

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

      ਮਿੱਤਰ vvp_vector8_t part_expand(ਅਚਲ vvp_vector8_t&, ਬਿਨਾਨਿਸ਼ਾਨੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ);

    ਜਨਤਕ:
      explicit vvp_vector8_t(ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ =0);
	// Make a vvp_vector8_t from a vector4 and a specified
	// strength.
      explicit vvp_vector8_t(ਅਚਲ vvp_vector4_t&that,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ str0,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ str1);
      explicit vvp_vector8_t(ਅਚਲ vvp_vector2_t&that,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ str0,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ str1);

      ~vvp_vector8_t();

      ਸਥਿਰ ਅਚਲ vvp_vector8_t nil;

    ਜਨਤਕ:

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ() ਅਚਲ { ਵਾਪਸ ਮਾਪ_2; }
      vvp_scalar_t ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ;
      vvp_vector8_t subvalue(ਬਿਨਾਨਿਸ਼ਾਨੀ adr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ) ਅਚਲ;
      ਖਾਲੀ ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, vvp_scalar_t val);
      ਖਾਲੀ ਸੈੱਟ_vec(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਅਚਲ vvp_vector8_t&that);

	// Test that the vectors are exactly equal
      bool eeq(ਅਚਲ vvp_vector8_t&that) ਅਚਲ;

      vvp_vector8_t(ਅਚਲ vvp_vector8_t&that);
      vvp_vector8_t& ਸੰਚਾਲਕ= (ਅਚਲ vvp_vector8_t&that);

    ਨਿਜੀ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ_2;
      ਸੰਘ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ*ptr_;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ val_[ਆਕਾਰ(ਖਾਲੀ*)];
      };
};

  /* Resolve uses the default Verilog resolver algorithm to resolve
     two drive vectors to a single output. */
ਸਤਰਅੰਦਰ vvp_vector8_t resolve(ਅਚਲ vvp_vector8_t&a, ਅਚਲ vvp_vector8_t&b)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(a.ਮਾਪ() == b.ਮਾਪ());
      vvp_vector8_t ਬਾਹਰਲਾ (a.ਮਾਪ());

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਬਾਹਰਲਾ.ਮਾਪ() ;  idx += 1) {
	    ਬਾਹਰਲਾ.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, resolve(a.ਮੁੱਲ(idx), b.ਮੁੱਲ(idx)));
      }

      ਵਾਪਸ ਬਾਹਰਲਾ;
}

  /* This lookup tabke implements the strength reduction implied by
     Verilog standard switch devices. The major dimension selects
     between non-resistive and resistive devices. */
ਬਾਹਰੀ ਬਿਨਾਨਿਸ਼ਾਨੀ vvp_switch_strength_map[2][8];

  /* The reduce4 function converts a vector8 to a vector4, losing
     strength information in the process. */
ਬਾਹਰੀ vvp_vector4_t reduce4(ਅਚਲ vvp_vector8_t&that);
ਬਾਹਰੀ vvp_vector8_t part_expand(ਅਚਲ vvp_vector8_t&a, ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਬਿਨਾਨਿਸ਼ਾਨੀ off);

  /* A c8string is of the form C8<...> where ... are bits. */
ਬਾਹਰੀ bool c8string_test(ਅਚਲ ਅੱਖਰ*str);
ਬਾਹਰੀ vvp_vector8_t c8string_ਮੰਜ਼ਿਲ_vector8(ਅਚਲ ਅੱਖਰ*str);

  /* Print a vector8 value to a stream. */
ਬਾਹਰੀ std::ostream& ਸੰਚਾਲਕ<< (std::ostream&, ਅਚਲ vvp_vector8_t&);

ਸਤਰਅੰਦਰ vvp_vector8_t::vvp_vector8_t(ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ_3)
: ਮਾਪ_2(ਮਾਪ_3)
{
      ਜੇ (ਮਾਪ_2 <= ਆਕਾਰ(val_)) {
	    memset(val_, 0, ਆਕਾਰ(val_));
      } ਨਹੀਂਤਾਂ {
	    ptr_ = new ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ[ਮਾਪ_2];
	    memset(ptr_, 0, ਮਾਪ_2);
      }
}

ਸਤਰਅੰਦਰ vvp_vector8_t::~vvp_vector8_t()
{
      ਜੇ (ਮਾਪ_2 > ਆਕਾਰ(val_))
	    delete[]ptr_;
}

ਸਤਰਅੰਦਰ vvp_scalar_t vvp_vector8_t::ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ
{
      ਪ੍ਰਤਿਜ੍ਞਾ(idx < ਮਾਪ_2);
      ਜੇ (ਮਾਪ_2 <= ਆਕਾਰ(val_))
	    ਵਾਪਸ vvp_scalar_t(val_[idx]);
      ਨਹੀਂਤਾਂ
	    ਵਾਪਸ vvp_scalar_t(ptr_[idx]);
}

ਸਤਰਅੰਦਰ ਖਾਲੀ vvp_vector8_t::ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, vvp_scalar_t val)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(idx < ਮਾਪ_2);
      ਜੇ (ਮਾਪ_2 <= ਆਕਾਰ(val_))
	    val_[idx] = val.raw();
      ਨਹੀਂਤਾਂ
	    ptr_[idx] = val.raw();
}

  // Exactly-equal for vvp_vector8_t is common and should be as tight
  // as possible.
ਸਤਰਅੰਦਰ bool vvp_vector8_t::eeq(ਅਚਲ vvp_vector8_t&that) ਅਚਲ
{
      ਜੇ (ਮਾਪ_2 != that.ਮਾਪ_2)
	    ਵਾਪਸ false;
      ਜੇ (ਮਾਪ_2 == 0)
	    ਵਾਪਸ true;

      ਜੇ (ਮਾਪ_2 <= ਆਕਾਰ(val_))
	      // This is equivalent to memcmp(val_, that.val_, sizeof val_)==0
	    ਵਾਪਸ ptr_ == that.ptr_;
      ਨਹੀਂਤਾਂ
	    ਵਾਪਸ memcmp(ptr_, that.ptr_, ਮਾਪ_2) == 0;
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
ਸਾਂਚਾ <class T> class vvp_sub_ਸੂਚਕ_t {

    ਜਨਤਕ:
      vvp_sub_ਸੂਚਕ_t() : ਬਿੱਟ_2(0) { }

      vvp_sub_ਸੂਚਕ_t(T*ptr__, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ_2)
      {
	    ਬਿੱਟ_2 = reinterpret_cast<uintptr_t> (ptr__);
	    ਪ੍ਰਤਿਜ੍ਞਾ( (ਬਿੱਟ_2  &  UINTPTR_C(3)) == 0 );
	    ਪ੍ਰਤਿਜ੍ਞਾ( (ਪੋਰਟ_2 & ~UINTPTR_C(3)) == 0 );
	    ਬਿੱਟ_2 |= ਪੋਰਟ_2;
      }

      ~vvp_sub_ਸੂਚਕ_t() = ਮੂਲ;

      T* ptr()
      { ਵਾਪਸ reinterpret_cast<T*> (ਬਿੱਟ_2 & ~UINTPTR_C(3)); }

      ਅਚਲ T* ptr() ਅਚਲ
      { ਵਾਪਸ reinterpret_cast<ਅਚਲ T*> (ਬਿੱਟ_2 & ~UINTPTR_C(3)); }

      ਬਿਨਾਨਿਸ਼ਾਨੀ  ਪੋਰਟ() ਅਚਲ { ਵਾਪਸ ਬਿੱਟ_2 & UINTPTR_C(3); }

      bool nil() ਅਚਲ { ਵਾਪਸ ਬਿੱਟ_2 == 0; }

      bool ਸੰਚਾਲਕ == (vvp_sub_ਸੂਚਕ_t that) ਅਚਲ { ਵਾਪਸ ਬਿੱਟ_2 == that.ਬਿੱਟ_2; }
      bool ਸੰਚਾਲਕ != (vvp_sub_ਸੂਚਕ_t that) ਅਚਲ { ਵਾਪਸ ਬਿੱਟ_2 != that.ਬਿੱਟ_2; }

    ਨਿਜੀ:
      uintptr_t ਬਿੱਟ_2;
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ vvp_sub_ਸੂਚਕ_t<vvp_net_t> vvp_net_ptr_t;
ਸਾਂਚਾ <class T> std::ostream& ਸੰਚਾਲਕ << (std::ostream&ਬਾਹਰਲਾ, vvp_sub_ਸੂਚਕ_t<T> val)
{ ਬਾਹਰਲਾ << val.ptr() << "[" << val.ਪੋਰਟ() << "]"; ਵਾਪਸ ਬਾਹਰਲਾ; }

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
    ਜਨਤਕ:
      vvp_net_t();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      vvp_net_t *pool;
#ਜੇ_ਅੰਤ
      vvp_net_ptr_t ਪੋਰਟ[4];
      vvp_net_fun_t*fun;
      vvp_net_fil_t*fil;

    ਜਨਤਕ:
	// Connect the port to the output from this net.
      ਖਾਲੀ link(vvp_net_ptr_t ਪੋਰਟ);
	// Disconnect the port from the output of this net.
      ਖਾਲੀ unlink(vvp_net_ptr_t ਪੋਰਟ);

    ਜਨਤਕ: // Methods to propagate output from this node.
      ਖਾਲੀ send_vec4(ਅਚਲ vvp_vector4_t&val, vvp_context_t context);
      ਖਾਲੀ send_vec8(ਅਚਲ vvp_vector8_t&val);
      ਖਾਲੀ send_real(ਦੁੱਗਣਾ val, vvp_context_t context);
      ਖਾਲੀ send_string(ਅਚਲ std::string&val, vvp_context_t context);
      ਖਾਲੀ send_object(vvp_object_t val, vvp_context_t context);

      ਖਾਲੀ send_vec4_pv(ਅਚਲ vvp_vector4_t&val,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t context);
      ਖਾਲੀ send_vec8_pv(ਅਚਲ vvp_vector8_t&val,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid);


    ਜਨਤਕ: // Methods to arrange for the output of this net to be forced.

	// The intent is that all efforts at force are directed to
	// operate only on the vvp_net_t whose output is to be
	// forced. These methods then communicate the force to the
	// attached filter to set up the actual force.
      ਖਾਲੀ force_vec4(ਅਚਲ vvp_vector4_t&val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ);
      ਖਾਲੀ force_vec8(ਅਚਲ vvp_vector8_t&val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ);
      ਖਾਲੀ force_real(ਦੁੱਗਣਾ val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ);

    ਜਨਤਕ: // Method to support $countdrivers
      ਖਾਲੀ ਗਿਣਤੀ_drivers(ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਬਿਨਾਨਿਸ਼ਾਨੀ counts[4]);

    // This needs to be public so that SDF interconnects can be inserted
    ਜਨਤਕ:
      vvp_net_ptr_t ਬਾਹਰਲਾ_2;

    ਜਨਤਕ: // Need a better new for these objects.
      ਸਥਿਰ ਖਾਲੀ* ਸੰਚਾਲਕ new(std::size_t ਮਾਪ);
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ delete(ਖਾਲੀ*); // not implemented
    ਨਿਜੀ: // not implemented
      ਸਥਿਰ ਖਾਲੀ* ਸੰਚਾਲਕ new[](std::size_t ਮਾਪ);
#ਜੇ defined(__GNUC__)
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ delete[](ਖਾਲੀ*);
#ਜੇ_ਅੰਤ
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

    ਜਨਤਕ:
      vvp_net_fun_t();
      ਆਭਾਸੀ ~vvp_net_fun_t();

      ਆਭਾਸੀ ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                             vvp_context_t context);
      ਆਭਾਸੀ ਖਾਲੀ recv_vec8(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ);
      ਆਭਾਸੀ ਖਾਲੀ recv_real(vvp_net_ptr_t ਪੋਰਟ, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                             vvp_context_t context);
      ਆਭਾਸੀ ਖਾਲੀ recv_string(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ std::string&ਦੋਆਧਾਰੀ_ਅੰਕ,
			       vvp_context_t context);
      ਆਭਾਸੀ ਖਾਲੀ recv_object(vvp_net_ptr_t ਪੋਰਟ, vvp_object_t ਦੋਆਧਾਰੀ_ਅੰਕ,
			       vvp_context_t context);

	// Part select variants of above
      ਆਭਾਸੀ ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t p, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t context);
      ਆਭਾਸੀ ਖਾਲੀ recv_vec8_pv(vvp_net_ptr_t p, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid);

	// This method is called when the net is forced or
	// released. This is very rarely needed; island ports use it
	// to know that the net is being forced and that it needs to
	// do something about it.
      ਆਭਾਸੀ ਖਾਲੀ force_flag(bool ਲੜੀਵਾਰ_now);

   ਸੁਰੱਖਿਅਤ:
      ਖਾਲੀ recv_vec4_pv_(vvp_net_ptr_t p, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t context);
      ਖਾਲੀ recv_vec8_pv_(vvp_net_ptr_t p, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid);

    ਜਨਤਕ: // These objects are only permallocated.
      ਸਥਿਰ ਖਾਲੀ* ਸੰਚਾਲਕ new(std::size_t ਮਾਪ) { ਵਾਪਸ ਢੇਰ.alloc(ਮਾਪ); }
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ delete(ਖਾਲੀ*); // not implemented

      ਸਥਿਰ std::size_t ਢੇਰ_ਕੁੱਲ() { ਵਾਪਸ ਢੇਰ.ਢੇਰ_ਕੁੱਲ(); }

    ਸੁਰੱਖਿਅਤ:
      ਸਥਿਰ permaheap ਢੇਰ;

    ਨਿਜੀ: // not implemented
      vvp_net_fun_t(ਅਚਲ vvp_net_fun_t&);
      vvp_net_fun_t& ਸੰਚਾਲਕ= (ਅਚਲ vvp_net_fun_t&);
      ਸਥਿਰ ਖਾਲੀ* ਸੰਚਾਲਕ new[](std::size_t ਮਾਪ);
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ delete[](ਖਾਲੀ*);
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
class vvp_net_fil_t  : ਜਨਤਕ vvp_vpi_callback {

    ਜਨਤਕ:
      vvp_net_fil_t();
      ਆਭਾਸੀ ~vvp_net_fil_t() override;

    ਜਨਤਕ:
      ਗਿਣਤੀ prop_t { STOP=0, PROP, REPL };


	// These filter methods are used by the vvp_net_t::send_*()
	// methods to test the output (from the functor) bit value
	// against any force filters. If none of the bits are forced,
	// then the method returns PROP and the caller propagates the
	// bit value. If bits were changed by the force mask, then the
	// method returns REPL and the caller should propagate the rep
	// value instead. If the function returns STOP, then all the
	// output bits are filtered by the force mask and there is
	// nothing to propagate.
      ਆਭਾਸੀ prop_t filter_vec4(ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ, vvp_vector4_t&rep,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid);
      ਆਭਾਸੀ prop_t filter_vec8(ਅਚਲ vvp_vector8_t&val, vvp_vector8_t&rep,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid);
      ਆਭਾਸੀ prop_t filter_real(ਦੁੱਗਣਾ&val);
      ਆਭਾਸੀ prop_t filter_object(vvp_object_t&val);
      ਆਭਾਸੀ prop_t filter_string(ਅਚਲ std::string&val);

      ਆਭਾਸੀ ਖਾਲੀ release(vvp_net_ptr_t ptr, bool net_flag) =0;
      ਆਭਾਸੀ ਖਾਲੀ release_pv(vvp_net_ptr_t ptr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid, bool net_flag) =0;

	// The %force/link instruction needs a place to write the
	// source node of the force, so that subsequent %force and
	// %release instructions can undo the link as needed. */
      ਖਾਲੀ force_link(vvp_net_t*dst, vvp_net_t*src);
      ਖਾਲੀ force_unlink(ਖਾਲੀ);

      ਆਭਾਸੀ ਬਿਨਾਨਿਸ਼ਾਨੀ filter_ਮਾਪ() ਅਚਲ =0;

    ਜਨਤਕ:
	// Support for force methods. These are called by the
	// vvp_net_t::force_* methods to set the force value and mask
	// for the filter.
      ਆਭਾਸੀ ਖਾਲੀ force_fil_vec4(ਅਚਲ vvp_vector4_t&val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ) =0;
      ਆਭਾਸੀ ਖਾਲੀ force_fil_vec8(ਅਚਲ vvp_vector8_t&val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ) =0;
      ਆਭਾਸੀ ਖਾਲੀ force_fil_real(ਦੁੱਗਣਾ val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ) =0;

    ਜਨਤਕ: // These objects are only permallocated.
      ਸਥਿਰ ਖਾਲੀ* ਸੰਚਾਲਕ new(std::size_t ਮਾਪ) { ਵਾਪਸ ਢੇਰ.alloc(ਮਾਪ); }
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ delete(ਖਾਲੀ*); // not implemented

      ਸਥਿਰ size_t ਢੇਰ_ਕੁੱਲ() { ਵਾਪਸ ਢੇਰ.ਢੇਰ_ਕੁੱਲ(); }

    ਨਿਜੀ:
      ਸਥਿਰ permaheap ਢੇਰ;

    ਨਿਜੀ: // not implemented
      vvp_net_fil_t(ਅਚਲ vvp_net_fil_t&);
      vvp_net_fil_t& ਸੰਚਾਲਕ= (ਅਚਲ vvp_net_fil_t&);
      ਸਥਿਰ ਖਾਲੀ* ਸੰਚਾਲਕ new[](std::size_t ਮਾਪ);
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ delete[](ਖਾਲੀ*);

    ਸੁਰੱਖਿਅਤ:
	// Set bits of the filter force mask
      ਖਾਲੀ force_ਨਕਾਬ(ਅਚਲ vvp_vector2_t&ਨਕਾਬ);
	// Release the force on the bits set in the mask.
      ਖਾਲੀ release_ਨਕਾਬ(ਅਚਲ vvp_vector2_t&ਨਕਾਬ);
	// Test bits of the filter force mask;
      bool test_force_ਨਕਾਬ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ;
      bool test_force_ਨਕਾਬ_is_zero() ਅਚਲ;

	// This template method is used by derived classes to process
	// the val through the force mask. The force value is the
	// currently forced value, and the buf is a value that this
	// method will use to hold a filtered value, if needed. This
	// method returns the replacement value into the "rep"
	// argument and returns a code indicating whether any changes
	// were made.
      ਸਾਂਚਾ <class T> prop_t filter_ਨਕਾਬ(ਅਚਲ T&val, ਅਚਲ T&force, T&rep, ਬਿਨਾਨਿਸ਼ਾਨੀ addr);
	// This template method is similar to the above, but works for
	// native types that are not so expensive to edit in place.
      ਸਾਂਚਾ <class T> prop_t filter_ਨਕਾਬ(T&val, T force);

	// These templates are similar to filter_mask_, but are
	// idempotent. Then do not trigger callbacks or otherwise
	// cause any local changes. These methods are used to test
	// arbitrary values against the force mask.
      ਸਾਂਚਾ <class T> prop_t filter_ਨਿਵੇਸ਼_ਨਕਾਬ(ਅਚਲ T&val, ਅਚਲ T&force, T&rep) ਅਚਲ;

    ਨਿਜੀ:
	// Mask of forced bits
      vvp_vector2_t force_ਨਕਾਬ_2;
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
class vvp_fun_force : ਜਨਤਕ vvp_net_fun_t {

    ਜਨਤਕ:
      vvp_fun_force();
      ~vvp_fun_force() override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
		     vvp_context_t context) override;
      ਖਾਲੀ recv_real(vvp_net_ptr_t ਪੋਰਟ, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ, vvp_context_t) override;
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
class vvp_fun_drive  : ਜਨਤਕ vvp_net_fun_t {

    ਜਨਤਕ:
      explicit vvp_fun_drive(ਬਿਨਾਨਿਸ਼ਾਨੀ str0 =6, ਬਿਨਾਨਿਸ਼ਾਨੀ str1 =6);
      ~vvp_fun_drive() override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t context) override;

      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t) override;
    ਨਿਜੀ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ drive0_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ drive1_;
};

/*
 * EXTEND functors expand an input vector to the desired output
 * width. The extend_signed functor sign extends the input. If the
 * input is already wider than the desired output, then it is passed
 * unmodified.
 */
class vvp_fun_extend_signed  : ਜਨਤਕ vvp_net_fun_t {

    ਜਨਤਕ:
      explicit vvp_fun_extend_signed(ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ~vvp_fun_extend_signed() override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t context) override;

      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t) override;
    ਨਿਜੀ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ_2;
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

class vvp_wide_fun_core : ਜਨਤਕ vvp_net_fun_t {

    ਜਨਤਕ:
      vvp_wide_fun_core(vvp_net_t*net, ਬਿਨਾਨਿਸ਼ਾਨੀ nports);
      ਆਭਾਸੀ ~vvp_wide_fun_core() override;
	// These objects are not perm allocated.
      ਸਥਿਰ ਖਾਲੀ* ਸੰਚਾਲਕ new(std::size_t ਮਾਪ) { ਵਾਪਸ ::new ਅੱਖਰ[ਮਾਪ]; }
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ delete(ਖਾਲੀ* ptr) { ::delete[](static_cast<ਅੱਖਰ*>(ptr)); }

    ਸੁਰੱਖਿਅਤ:
      ਖਾਲੀ propagate_vec4(ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ, vvp_time64_t delay =0);
      ਖਾਲੀ propagate_real(ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ, vvp_time64_t delay =0);
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ_ਗਿਣਤੀ() ਅਚਲ;

      vvp_vector4_t& ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ);
      ਦੁੱਗਣਾ ਮੁੱਲ_r(ਬਿਨਾਨਿਸ਼ਾਨੀ);

    ਨਿਜੀ:
	// the derived class implements this to receive an indication
	// that one of the port input values changed.
      ਆਭਾਸੀ ਖਾਲੀ recv_vec4_from_inputs(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ) =0;
      ਆਭਾਸੀ ਖਾਲੀ recv_real_from_inputs(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ);

      ਮਿੱਤਰ class vvp_wide_fun_t;
      ਖਾਲੀ dispatch_vec4_from_ਨਿਵੇਸ਼(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ);
      ਖਾਲੀ dispatch_real_from_ਨਿਵੇਸ਼(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ);

    ਨਿਜੀ:
	// Back-point to the vvp_net_t that points to me.
      vvp_net_t*ptr_;
	// Structure to track the input values from the input functors.
      ਬਿਨਾਨਿਸ਼ਾਨੀ nports_;
      vvp_vector4_t*ਪੋਰਟ_ਮੁੱਲਾਂ;
      ਦੁੱਗਣਾ*ਪੋਰਟ_rvalues;
};

/*
 * The job of the input functor is only to monitor inputs to the
 * function and pass them to the ufunc_core object. Each functor takes
 * up to 4 inputs, with the base the port number for the first
 * function input that this functor represents.
 */
class vvp_wide_fun_t : ਜਨਤਕ vvp_net_fun_t {

    ਜਨਤਕ:
      vvp_wide_fun_t(vvp_wide_fun_core*c, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ);
      ~vvp_wide_fun_t() override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t context) override;
      ਖਾਲੀ recv_real(vvp_net_ptr_t ਪੋਰਟ, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t context) override;

      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t p, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t context) override;

    ਨਿਜੀ:
      vvp_wide_fun_core*core_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ_ਅਧਾਰ_2;
};


ਸਤਰਅੰਦਰ ਖਾਲੀ vvp_send_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&val, vvp_context_t context)
{
      ਜਦੋਂ (class vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t ਅਗਲਾ_val = cur->ਪੋਰਟ[ptr.ਪੋਰਟ()];

	    ਜੇ (cur->fun)
		  cur->fun->recv_vec4(ptr, val, context);

	    ptr = ਅਗਲਾ_val;
      }
}

ਬਾਹਰੀ ਖਾਲੀ vvp_send_vec8(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector8_t&val);
ਬਾਹਰੀ ਖਾਲੀ vvp_send_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ val,
                          vvp_context_t context);

ਸਤਰਅੰਦਰ ਖਾਲੀ vvp_send_string(vvp_net_ptr_t ptr, ਅਚਲ std::string&val, vvp_context_t context)
{
      ਜਦੋਂ (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t ਅਗਲਾ_val = cur->ਪੋਰਟ[ptr.ਪੋਰਟ()];

	    ਜੇ (cur->fun)
		  cur->fun->recv_string(ptr, val, context);

	    ptr = ਅਗਲਾ_val;
      }
}

ਸਤਰਅੰਦਰ ਖਾਲੀ vvp_send_object(vvp_net_ptr_t ptr, vvp_object_t val, vvp_context_t context)
{
      ਜਦੋਂ (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t ਅਗਲਾ_val = cur->ਪੋਰਟ[ptr.ਪੋਰਟ()];

	    ਜੇ (cur->fun)
		  cur->fun->recv_object(ptr, val, context);

	    ptr = ਅਗਲਾ_val;
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
ਸਤਰਅੰਦਰ ਖਾਲੀ vvp_send_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&val,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid,
			     vvp_context_t context)
{
      ਜਦੋਂ (class vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t ਅਗਲਾ_val = cur->ਪੋਰਟ[ptr.ਪੋਰਟ()];

	    ਜੇ (cur->fun)
		  cur->fun->recv_vec4_pv(ptr, val, ਅਧਾਰ, vwid, context);

	    ptr = ਅਗਲਾ_val;
      }
}

ਸਤਰਅੰਦਰ ਖਾਲੀ vvp_send_vec8_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector8_t&val,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid)
{
      ਜਦੋਂ (class vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t ਅਗਲਾ_val = cur->ਪੋਰਟ[ptr.ਪੋਰਟ()];

	    ਜੇ (cur->fun)
		  cur->fun->recv_vec8_pv(ptr, val, ਅਧਾਰ, vwid);

	    ptr = ਅਗਲਾ_val;
      }
}

ਸਤਰਅੰਦਰ ਖਾਲੀ vvp_net_t::send_vec4(ਅਚਲ vvp_vector4_t&val, vvp_context_t context)
{
      ਜੇ (fil == 0) {
	    vvp_send_vec4(ਬਾਹਰਲਾ_2, val, context);
	    ਵਾਪਸ;
      }

      vvp_vector4_t rep;
      ਚੋਣ (fil->filter_vec4(val, rep, 0, val.ਮਾਪ())) {
	  ਹਾਲਤ vvp_net_fil_t::STOP:
	    ਰੋਕੋ;
	  ਹਾਲਤ vvp_net_fil_t::PROP:
	    vvp_send_vec4(ਬਾਹਰਲਾ_2, val, context);
	    ਰੋਕੋ;
	  ਹਾਲਤ vvp_net_fil_t::REPL:
	    vvp_send_vec4(ਬਾਹਰਲਾ_2, rep, context);
	    ਰੋਕੋ;
      }
}

ਸਤਰਅੰਦਰ ਖਾਲੀ vvp_net_t::send_vec4_pv(ਅਚਲ vvp_vector4_t&val,
				    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid,
				    vvp_context_t context)
{
      ਜੇ (fil == 0) {
	    vvp_send_vec4_pv(ਬਾਹਰਲਾ_2, val, ਅਧਾਰ, vwid, context);
	    ਵਾਪਸ;
      }

      vvp_vector4_t rep;
      ਚੋਣ (fil->filter_vec4(val, rep, ਅਧਾਰ, vwid)) {
	  ਹਾਲਤ vvp_net_fil_t::STOP:
	    ਰੋਕੋ;
	  ਹਾਲਤ vvp_net_fil_t::PROP:
	    vvp_send_vec4_pv(ਬਾਹਰਲਾ_2, val, ਅਧਾਰ, vwid, context);
	    ਰੋਕੋ;
	  ਹਾਲਤ vvp_net_fil_t::REPL:
	    vvp_send_vec4_pv(ਬਾਹਰਲਾ_2, rep, ਅਧਾਰ, vwid, context);
	    ਰੋਕੋ;
      }
}

ਸਤਰਅੰਦਰ ਖਾਲੀ vvp_net_t::send_vec8(ਅਚਲ vvp_vector8_t&val)
{
      ਜੇ (fil == 0) {
	    vvp_send_vec8(ਬਾਹਰਲਾ_2, val);
	    ਵਾਪਸ;
      }

      vvp_vector8_t rep;
      ਚੋਣ (fil->filter_vec8(val, rep, 0, val.ਮਾਪ())) {
	  ਹਾਲਤ vvp_net_fil_t::STOP:
	    ਰੋਕੋ;
	  ਹਾਲਤ vvp_net_fil_t::PROP:
	    vvp_send_vec8(ਬਾਹਰਲਾ_2, val);
	    ਰੋਕੋ;
	  ਹਾਲਤ vvp_net_fil_t::REPL:
	    vvp_send_vec8(ਬਾਹਰਲਾ_2, rep);
	    ਰੋਕੋ;
      }
}

ਸਤਰਅੰਦਰ ਖਾਲੀ vvp_net_t::send_vec8_pv(ਅਚਲ vvp_vector8_t&val,
				    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid)
{
      ਜੇ (fil == 0) {
	    vvp_send_vec8_pv(ਬਾਹਰਲਾ_2, val, ਅਧਾਰ, vwid);
	    ਵਾਪਸ;
      }

      vvp_vector8_t rep;
      ਚੋਣ (fil->filter_vec8(val, rep, ਅਧਾਰ, vwid)) {
	  ਹਾਲਤ vvp_net_fil_t::STOP:
	    ਰੋਕੋ;
	  ਹਾਲਤ vvp_net_fil_t::PROP:
	    vvp_send_vec8_pv(ਬਾਹਰਲਾ_2, val, ਅਧਾਰ, vwid);
	    ਰੋਕੋ;
	  ਹਾਲਤ vvp_net_fil_t::REPL:
	    vvp_send_vec8_pv(ਬਾਹਰਲਾ_2, rep, ਅਧਾਰ, vwid);
	    ਰੋਕੋ;
      }
}

ਸਤਰਅੰਦਰ ਖਾਲੀ vvp_net_t::send_real(ਦੁੱਗਣਾ val, vvp_context_t context)
{
      ਜੇ (fil && ! fil->filter_real(val))
	    ਵਾਪਸ;

      vvp_send_real(ਬਾਹਰਲਾ_2, val, context);
}


ਸਤਰਅੰਦਰ ਖਾਲੀ vvp_net_t::send_string(ਅਚਲ std::string&val, vvp_context_t context)
{
      ਜੇ (fil && !fil->filter_string(val))
	    ਵਾਪਸ;

      vvp_send_string(ਬਾਹਰਲਾ_2, val, context);
}


ਸਤਰਅੰਦਰ ਖਾਲੀ vvp_net_t::send_object(vvp_object_t val, vvp_context_t context)
{
      ਜੇ (fil && ! fil->filter_object(val))
	    ਵਾਪਸ;

      vvp_send_object(ਬਾਹਰਲਾ_2, val, context);
}


ਸਤਰਅੰਦਰ bool vvp_net_fil_t::test_force_ਨਕਾਬ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ
{
      ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ >= force_ਨਕਾਬ_2.ਮਾਪ())
	    ਵਾਪਸ false;
      ਜੇ (force_ਨਕਾਬ_2.ਮੁੱਲ(ਦੋਆਧਾਰੀ_ਅੰਕ))
	    ਵਾਪਸ true;
      ਨਹੀਂਤਾਂ
	    ਵਾਪਸ false;
}

ਸਤਰਅੰਦਰ bool vvp_net_fil_t::test_force_ਨਕਾਬ_is_zero(ਖਾਲੀ) ਅਚਲ
{
      ਜੇ (force_ਨਕਾਬ_2.ਮਾਪ() == 0)
	    ਵਾਪਸ true;
      ਜੇ (force_ਨਕਾਬ_2.is_zero())
	    ਵਾਪਸ true;
      ਵਾਪਸ false;
}

/*
 * Undefine the ivl_alloc.h definitions so they don't leak out of this file.
 */
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ਸਮ੍ਰਿਤੀ_ਦਿਓ
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ realloc
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ __ivl_alloc_H

#ਜੇ_ਅੰਤ /* IVL_vvp_net_H */
