#నిర్వచితం_కాకపోతే IVL_vvp_net_H
#నిర్వచించు IVL_vvp_net_H
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

#నిర్వచితమైతే HAVE_IOSFWD
# include  <iosfwd>
#లేకపోతే
# include  <iostream>
#అయితే_ముగింపు

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
రకనిర్వచనం శూన్య**vvp_context_t;

రకనిర్వచనం శూన్య*vvp_context_అంశం_t;

అంతఃవరుస vvp_context_t vvp_allocate_context(గుర్తులేని nitem)
{
      తిరిగి (vvp_context_t)జ్ఞాపకం_ఇవ్వు((2 + nitem) * పరిమాణం(శూన్య*));
}

అంతఃవరుస vvp_context_t vvp_get_తదుపరి_context(vvp_context_t context)
{
      తిరిగి (vvp_context_t)context[0];
}

అంతఃవరుస శూన్య vvp_సమితి_తదుపరి_context(vvp_context_t context, vvp_context_t తదుపరి_2)
{
      context[0] = తదుపరి_2;
}

అంతఃవరుస vvp_context_t vvp_get_stacked_context(vvp_context_t context)
{
      తిరిగి (vvp_context_t)context[1];
}

అంతఃవరుస శూన్య vvp_సమితి_stacked_context(vvp_context_t context, vvp_context_t దొంతర)
{
      context[1] = దొంతర;
}

అంతఃవరుస vvp_context_అంశం_t vvp_get_context_అంశం(vvp_context_t context,
                                               గుర్తులేని అంశం_idx)
{
      తిరిగి (vvp_context_అంశం_t)context[అంశం_idx];
}

అంతఃవరుస శూన్య vvp_సమితి_context_అంశం(vvp_context_t context, గుర్తులేని అంశం_idx,
                                 vvp_context_అంశం_t అంశం)
{
      context[అంశం_idx] = అంశం;
}

/*
 * An "automatic" functor is one which may be associated with an automatically
 * allocated scope item. This provides the infrastructure needed to allocate
 * the state information for individual instances of the item.
 */
నిర్మాణం automatic_hooks_s {

      automatic_hooks_s() {}
      మాయా ~automatic_hooks_s() {}

      మాయా శూన్య alloc_instance(vvp_context_t context) = 0;
      మాయా శూన్య reset_instance(vvp_context_t context) = 0;
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      మాయా శూన్య free_instance(vvp_context_t context) = 0;
#అయితే_ముగింపు
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
గణన vvp_bit4_t {
      BIT4_0 = 0,
      BIT4_1 = 1,
      BIT4_X = 3,
      BIT4_Z = 2
};

  /* Return an ASCII character that represents the vvp_bit4_t value. */
అంతఃవరుస అక్షరం vvp_bit4_గమ్యం_ascii(vvp_bit4_t a) { తిరిగి "01zx"[a]; }

బాహ్య vvp_bit4_t కూడు_with_వాహకం(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c);

బాహ్య vvp_bit4_t scalar_గమ్యం_bit4(PLI_INT32 scalar);

  /* Return TRUE if the bit is BIT4_X or BIT4_Z. The fast
     implementation here relies on the encoding of vvp_bit4_t values. */
అంతఃవరుస bool bit4_is_xz(vvp_bit4_t a) { తిరిగి a >= 2; }

  /* This function converts BIT4_Z to BIT4_X, but passes other values
     unchanged. This fast implementation relies of the encoding of the
     vvp_bit4_t values. In particular, the BIT4_X==3 and BIT4_Z==2 */
అంతఃవరుస vvp_bit4_t bit4_z2x(vvp_bit4_t a)
{ తిరిగి (vvp_bit4_t) ( (పూర్ణాంకం)a | ((పూర్ణాంకం)a >> 1) ); }

  /* Some common boolean operators. These implement the Verilog rules
     for 4-value bit operations. The fast implementations here rely
     on the encoding of vvp_bit4_t values. */

  // ~BIT4_0 --> BIT4_1
  // ~BIT4_1 --> BIT4_0
  // ~BIT4_X --> BIT4_X
  // ~BIT4_Z --> BIT4_X
అంతఃవరుస vvp_bit4_t క్రియాకారకం ~ (vvp_bit4_t a)
{ తిరిగి bit4_z2x((vvp_bit4_t) (((పూర్ణాంకం)a) ^ 1)); }

అంతఃవరుస vvp_bit4_t క్రియాకారకం | (vvp_bit4_t a, vvp_bit4_t b)
{
      ఒకవేళ (a==BIT4_1 || b==BIT4_1)
	    తిరిగి BIT4_1;
      తిరిగి bit4_z2x( (vvp_bit4_t) ((పూర్ణాంకం)a | (పూర్ణాంకం)b) );
}

అంతఃవరుస vvp_bit4_t క్రియాకారకం & (vvp_bit4_t a, vvp_bit4_t b)
{
      ఒకవేళ (a==BIT4_0 || b==BIT4_0)
	    తిరిగి BIT4_0;
      తిరిగి bit4_z2x( (vvp_bit4_t) ((పూర్ణాంకం)a | (పూర్ణాంకం)b) );
}


బాహ్య vvp_bit4_t క్రియాకారకం ^ (vvp_bit4_t a, vvp_bit4_t b);
బాహ్య std::ostream& క్రియాకారకం<< (std::ostream&o, vvp_bit4_t a);

  /* Return >0, ==0 or <0 if the from-to transition represents a
     posedge, no edge, or negedge. */
బాహ్య పూర్ణాంకం అంచు(vvp_bit4_t from, vvp_bit4_t గమ్యం);

  /* Support for $countdrivers. */
అంతఃవరుస శూన్య update_driver_counts(vvp_bit4_t బిట్టు, గుర్తులేని counts[3])
{
      ఎంపిక (బిట్టు) {
          స్థితి BIT4_0:
            counts[0] += 1;
            ఆపు;
          స్థితి BIT4_1:
            counts[1] += 1;
            ఆపు;
          స్థితి BIT4_X:
            counts[2] += 1;
            ఆపు;
          అప్రమేయ:
            ఆపు;
      }
}

/*
 * Some of the instructions do wide addition to arrays of long. They
 * use this add_with_carry function to help.
 */
స్థిర అంతఃవరుస గుర్తులేని దీర్ఘ కూడు_with_వాహకం(గుర్తులేని దీర్ఘ a, గుర్తులేని దీర్ఘ b,
					   గుర్తులేని దీర్ఘ&వాహకం)
{
      గుర్తులేని దీర్ఘ tmp = b + వాహకం;
      గుర్తులేని దీర్ఘ కూడిక = a + tmp;
      వాహకం = 0;
      ఒకవేళ (tmp < b)
	    వాహకం = 1;
      ఒకవేళ (కూడిక < tmp)
	    వాహకం = 1;
      ఒకవేళ (కూడిక < a)
	    వాహకం = 1;
      తిరిగి కూడిక;
}

బాహ్య గుర్తులేని దీర్ఘ multiply_with_వాహకం(గుర్తులేని దీర్ఘ a, గుర్తులేని దీర్ఘ b,
					 గుర్తులేని దీర్ఘ&వాహకం);

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

      మిత్ర vvp_vector4_t క్రియాకారకం ~(స్థిరరాశి vvp_vector4_t&that);
      మిత్ర class vvp_vector4array_t;
      మిత్ర class vvp_vector4array_sa;
      మిత్ర class vvp_vector4array_aa;

    బహిరంగ:
      స్థిర స్థిరరాశి vvp_vector4_t nil;

    బహిరంగ:
      explicit vvp_vector4_t(గుర్తులేని కొలత =0, vvp_bit4_t బిట్లు =BIT4_X);

      explicit vvp_vector4_t(గుర్తులేని కొలత, ద్విగుణ val);

	// Construct a vector4 from the subvalue of another
	// vector4. The width of the result is 'wid', and the bits are
	// pulled from 'that' to implement the Verilog part select
	// semantics. This means that part select beyond 'that'
	// returns X bits.
      explicit vvp_vector4_t(స్థిరరాశి vvp_vector4_t&that,
			     గుర్తులేని adr, గుర్తులేని wid);

      vvp_vector4_t(స్థిరరాశి vvp_vector4_t&that);
      vvp_vector4_t(స్థిరరాశి vvp_vector4_t&that, bool invert_flag);
      vvp_vector4_t& క్రియాకారకం= (స్థిరరాశి vvp_vector4_t&that);

      ~vvp_vector4_t();

      అంతఃవరుస గుర్తులేని కొలత() స్థిరరాశి { తిరిగి కొలత_2; }
      శూన్య resize(గుర్తులేని new_కొలత, vvp_bit4_t pad_బిట్టు = BIT4_X);

	// Get the bit at the specified address
      vvp_bit4_t విలువ(గుర్తులేని idx) స్థిరరాశి;
	// Get the vector4 subvector starting at the address
      vvp_vector4_t subvalue(గుర్తులేని idx, గుర్తులేని కొలత) స్థిరరాశి;
	// Get the 2-value bits for the subvector. This returns a new
	// array of longs, or a nil pointer if an XZ bit was detected
	// in the array.
      గుర్తులేని దీర్ఘ*subarray(గుర్తులేని idx, గుర్తులేని కొలత, bool xz_గమ్యం_0 =false) స్థిరరాశి;
      శూన్య setarray(గుర్తులేని idx, గుర్తులేని కొలత, స్థిరరాశి గుర్తులేని దీర్ఘ*val);

	// Set a 4-value bit or subvector into the vector. Return true
	// if any bits of the vector change as a result of this operation.
      శూన్య సమితి_బిట్టు(గుర్తులేని idx, vvp_bit4_t val);
      bool సమితి_vec(గుర్తులేని idx, స్థిరరాశి vvp_vector4_t&that);

        // Get the bits from another vector, but keep my size.
      శూన్య copy_బిట్లు(స్థిరరాశి vvp_vector4_t&that);

	// Move bits within this vector.
      శూన్య mov(గుర్తులేని dst, గుర్తులేని src, గుర్తులేని cnt);

	// Add that to this in the Verilog way.
      శూన్య కూడు(స్థిరరాశి vvp_vector4_t&that);

	// Subtract that from this in the Verilog way.
      శూన్య sub(స్థిరరాశి vvp_vector4_t&that);

	// Multiply this by that in the Verilog way.
      శూన్య mul(స్థిరరాశి vvp_vector4_t&that);

	// Test that the vectors are exactly equal
      bool eeq(స్థిరరాశి vvp_vector4_t&that) స్థిరరాశి;

	// Test that the vectors are equal, with xz comparing as equal.
      bool eq_xz(స్థిరరాశి vvp_vector4_t&that) స్థిరరాశి;

	// Return true if there is an X or Z anywhere in the vector.
      bool has_xz() స్థిరరాశి;

	// Change all Z bits to X bits.
      శూన్య change_z2x();

	// Set all bits to the specified value.
      శూన్య fill_బిట్లు(vvp_bit4_t బిట్టు);

	// Change all bits to X bits.
      శూన్య సమితి_గమ్యం_x() { fill_బిట్లు(BIT4_X); }

	// Display the value into the buf as a string.
      అక్షరం*as_string(అక్షరం*buf, size_t buf_len) స్థిరరాశి;

      శూన్య invert();
      vvp_vector4_t& క్రియాకారకం &= (స్థిరరాశి vvp_vector4_t&that);
      vvp_vector4_t& క్రియాకారకం |= (స్థిరరాశి vvp_vector4_t&that);
      vvp_vector4_t& క్రియాకారకం ^= (స్థిరరాశి vvp_vector4_t&that);
      vvp_vector4_t& క్రియాకారకం += (int64_t);

      vvp_bit4_t reduce_or() స్థిరరాశి;
      vvp_bit4_t reduce_and() స్థిరరాశి;
      vvp_bit4_t reduce_xor() స్థిరరాశి;

    అంతరంగ:
	// Number of vvp_bit4_t bits that can be shoved into a word.
      గణన { బిట్లు_చొప్పున_మాట = 8*పరిమాణం(గుర్తులేని దీర్ఘ) };
	// The double value constructor requires that WORD_0_BBITS
	// and WORD_1_BBITS have the same value!
#ఒకవేళ SIZEOF_UNSIGNED_LONG == 8
      గణన { మాట_0_ABITS = 0x0000000000000000UL,
	     మాట_0_BBITS = 0x0000000000000000UL };
      గణన { మాట_1_ABITS = 0xFFFFFFFFFFFFFFFFUL,
	     మాట_1_BBITS = 0x0000000000000000UL };
      గణన { మాట_X_ABITS = 0xFFFFFFFFFFFFFFFFUL,
	     మాట_X_BBITS = 0xFFFFFFFFFFFFFFFFUL };
      గణన { మాట_Z_ABITS = 0x0000000000000000UL,
             మాట_Z_BBITS = 0xFFFFFFFFFFFFFFFFUL };
#లేదావేళ SIZEOF_UNSIGNED_LONG == 4
      గణన { మాట_0_ABITS = 0x00000000UL, మాట_0_BBITS = 0x00000000UL };
      గణన { మాట_1_ABITS = 0xFFFFFFFFUL, మాట_1_BBITS = 0x00000000UL };
      గణన { మాట_X_ABITS = 0xFFFFFFFFUL, మాట_X_BBITS = 0xFFFFFFFFUL };
      గణన { మాట_Z_ABITS = 0x00000000UL, మాట_Z_BBITS = 0xFFFFFFFFUL };
#లేకపోతే
#దోషం "WORD_X_xBITS not defined for this architecture?"
#అయితే_ముగింపు

	// Initialize and operator= use this private method to copy
	// the data from that object into this object.
      శూన్య copy_from_(స్థిరరాశి vvp_vector4_t&that);
      శూన్య copy_from_big_(స్థిరరాశి vvp_vector4_t&that);
      శూన్య copy_inverted_from_(స్థిరరాశి vvp_vector4_t&that);

      శూన్య allocate_పదాలు(గుర్తులేని దీర్ఘ inita, గుర్తులేని దీర్ఘ initb);

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

      గుర్తులేని కొలత_2;
      సంఘం {
	    గుర్తులేని దీర్ఘ abits_val_;
	    గుర్తులేని దీర్ఘ*abits_ptr_;
      };
      సంఘం {
	    గుర్తులేని దీర్ఘ bbits_val_;
	    గుర్తులేని దీర్ఘ*bbits_ptr_;
      };
};

అంతఃవరుస vvp_vector4_t::vvp_vector4_t(స్థిరరాశి vvp_vector4_t&that)
{
      copy_from_(that);
}

అంతఃవరుస vvp_vector4_t::vvp_vector4_t(స్థిరరాశి vvp_vector4_t&that, bool invert_flag)
{
      ఒకవేళ (invert_flag)
	    copy_inverted_from_(that);
      లేకపోతే
	    copy_from_(that);
}

అంతఃవరుస vvp_vector4_t::vvp_vector4_t(గుర్తులేని కొలత_3, vvp_bit4_t val)
: కొలత_2(కొలత_3)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      స్థిర స్థిరరాశి గుర్తులేని దీర్ఘ init_atable[4] = {
	    మాట_0_ABITS,
	    మాట_1_ABITS,
	    మాట_Z_ABITS,
	    మాట_X_ABITS };
      స్థిర స్థిరరాశి గుర్తులేని దీర్ఘ init_btable[4] = {
	    మాట_0_BBITS,
	    మాట_1_BBITS,
	    మాట_Z_BBITS,
	    మాట_X_BBITS };

      allocate_పదాలు(init_atable[val], init_btable[val]);
}

అంతఃవరుస vvp_vector4_t::~vvp_vector4_t()
{
      ఒకవేళ (కొలత_2 > బిట్లు_చొప్పున_మాట) {
	    delete[] abits_ptr_;
	      // bbits_ptr_ actually points half-way into a
	      // double-length array started at abits_ptr_
      }
}

అంతఃవరుస vvp_vector4_t& vvp_vector4_t::క్రియాకారకం= (స్థిరరాశి vvp_vector4_t&that)
{
      ఒకవేళ (this == &that)
	    తిరిగి *this;

      ఒకవేళ (కొలత_2 > బిట్లు_చొప్పున_మాట)
	    delete[] abits_ptr_;

      copy_from_(that);

      తిరిగి *this;
}

అంతఃవరుస శూన్య vvp_vector4_t::copy_from_(స్థిరరాశి vvp_vector4_t&that)
{
      కొలత_2 = that.కొలత_2;
      ఒకవేళ (కొలత_2 <= బిట్లు_చొప్పున_మాట) {
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;
      } లేకపోతే {
	    copy_from_big_(that);
      }
}

అంతఃవరుస vvp_bit4_t vvp_vector4_t::విలువ(గుర్తులేని idx) స్థిరరాశి
{
      ఒకవేళ (idx >= కొలత_2)
	    తిరిగి BIT4_X;

      గుర్తులేని off;

      గుర్తులేని దీర్ఘ abits, bbits;
      ఒకవేళ (కొలత_2 > బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని wdx = idx / బిట్లు_చొప్పున_మాట;
	    off = idx % బిట్లు_చొప్పున_మాట;
	    abits = abits_ptr_[wdx];
	    bbits = bbits_ptr_[wdx];
      } లేకపోతే {
	    off = idx;
	    abits = abits_val_;
	    bbits = bbits_val_;
      }

      abits >>= off;
      bbits >>= off;
      పూర్ణాంకం tmp = ((bbits&1) << 1) + (abits&1);
	// This cast works since b==1,a==1 is X and b==1,a==0 is Z.
      తిరిగి (vvp_bit4_t)tmp;
}

అంతఃవరుస vvp_vector4_t vvp_vector4_t::subvalue(గుర్తులేని adr, గుర్తులేని wid) స్థిరరాశి
{
      తిరిగి vvp_vector4_t(*this, adr, wid);
}

అంతఃవరుస శూన్య vvp_vector4_t::సమితి_బిట్టు(గుర్తులేని idx, vvp_bit4_t val)
{
      ప్రతిజ్ఞా(idx < కొలత_2);

      గుర్తులేని దీర్ఘ off = idx % బిట్లు_చొప్పున_మాట;
      గుర్తులేని దీర్ఘ ముసుగు = 1UL << off;

      ఒకవేళ (కొలత_2 > బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని wdx = idx / బిట్లు_చొప్పున_మాట;
	    ఎంపిక (val) {
		స్థితి BIT4_0:
		  abits_ptr_[wdx] &= ~ముసుగు;
		  bbits_ptr_[wdx] &= ~ముసుగు;
		  ఆపు;
		స్థితి BIT4_1:
		  abits_ptr_[wdx] |=  ముసుగు;
		  bbits_ptr_[wdx] &= ~ముసుగు;
		  ఆపు;
		స్థితి BIT4_X:
		  abits_ptr_[wdx] |=  ముసుగు;
		  bbits_ptr_[wdx] |=  ముసుగు;
		  ఆపు;
		స్థితి BIT4_Z:
		  abits_ptr_[wdx] &= ~ముసుగు;
		  bbits_ptr_[wdx] |=  ముసుగు;
		  ఆపు;
	    }
      } లేకపోతే {
	    ఎంపిక (val) {
		స్థితి BIT4_0:
		  abits_val_ &= ~ముసుగు;
		  bbits_val_ &= ~ముసుగు;
		  ఆపు;
		స్థితి BIT4_1:
		  abits_val_ |=  ముసుగు;
		  bbits_val_ &= ~ముసుగు;
		  ఆపు;
		స్థితి BIT4_X:
		  abits_val_ |=  ముసుగు;
		  bbits_val_ |=  ముసుగు;
		  ఆపు;
		స్థితి BIT4_Z:
		  abits_val_ &= ~ముసుగు;
		  bbits_val_ |=  ముసుగు;
		  ఆపు;
	    }
      }
}

అంతఃవరుస vvp_vector4_t క్రియాకారకం ~ (స్థిరరాశి vvp_vector4_t&that)
{
      vvp_vector4_t res (that, true);
      తిరిగి res;
}

బాహ్య std::ostream& క్రియాకారకం << (std::ostream&, స్థిరరాశి vvp_vector4_t&);

బాహ్య vvp_bit4_t compare_gtge(స్థిరరాశి vvp_vector4_t&a,
			       స్థిరరాశి vvp_vector4_t&b,
			       vvp_bit4_t val_if_equal);
బాహ్య vvp_bit4_t compare_gtge_signed(స్థిరరాశి vvp_vector4_t&a,
				      స్థిరరాశి vvp_vector4_t&b,
				      vvp_bit4_t val_if_equal);
మూస <class T> బాహ్య T coerce_గమ్యం_వెడల్పు(స్థిరరాశి T&that, గుర్తులేని వెడల్పు);

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
మూస <class T> బాహ్య bool vector4_గమ్యం_విలువ(స్థిరరాశి vvp_vector4_t&a, T&val,
						bool is_signed,
						bool is_arithmetic =true);

మూస <class T> బాహ్య bool vector4_గమ్యం_విలువ(స్థిరరాశి vvp_vector4_t&a,
                                                bool&అధికప్రవాహం_flag, T&val);

మూస <class T> అంతఃవరుస bool vector4_గమ్యం_విలువ(స్థిరరాశి vvp_vector4_t&a, T&val)
{
      bool అధికప్రవాహం_flag;
      తిరిగి vector4_గమ్యం_విలువ(a, అధికప్రవాహం_flag, val);
}

బాహ్య bool vector4_గమ్యం_విలువ(స్థిరరాశి vvp_vector4_t&a, ద్విగుణ&val, bool is_signed);

బాహ్య bool vector2_గమ్యం_విలువ(స్థిరరాశి vvp_vector2_t&a, int32_t&val, bool is_signed);

బాహ్య vvp_vector4_t vector4_from_పాఠ్యం(స్థిరరాశి అక్షరం*బిట్లు, గుర్తులేని wid);

అంతఃవరుస vvp_vector4_t double_గమ్యం_vector4_LSB(ద్విగుణ real)
{
      // Convert the double to a bit vector and then use the LSB.
      // You can use very large real values and break the 64-bit
      // size, but that's really horrible code so don't do that!
      vvp_vector4_t బిట్టు = vvp_vector4_t(64, real);
      బిట్టు.resize(1);
      తిరిగి బిట్టు;
}

/*
 * vvp_vector4array_t
 */
class vvp_vector4array_t {

    బహిరంగ:
      vvp_vector4array_t(గుర్తులేని వెడల్పు, గుర్తులేని పదాలు);
      మాయా ~vvp_vector4array_t();

      గుర్తులేని వెడల్పు() స్థిరరాశి { తిరిగి వెడల్పు_2; }
      గుర్తులేని పదాలు() స్థిరరాశి { తిరిగి పదాలు_3; }

      మాయా vvp_vector4_t get_మాట(గుర్తులేని idx) స్థిరరాశి = 0;
      మాయా శూన్య సమితి_మాట(గుర్తులేని idx, స్థిరరాశి vvp_vector4_t&that) = 0;

    రక్షిత:
      నిర్మాణం v4cell {
	    సంఘం {
		  గుర్తులేని దీర్ఘ abits_val_;
		  గుర్తులేని దీర్ఘ*abits_ptr_;
	    };
	    సంఘం {
		  గుర్తులేని దీర్ఘ bbits_val_;
		  గుర్తులేని దీర్ఘ*bbits_ptr_;
	    };
      };

      vvp_vector4_t get_మాట_2(v4cell*cell) స్థిరరాశి;
      శూన్య సమితి_మాట_2(v4cell*cell, స్థిరరాశి vvp_vector4_t&that);

      గుర్తులేని వెడల్పు_2;
      గుర్తులేని పదాలు_3;

    అంతరంగ: // Not implemented
      vvp_vector4array_t(స్థిరరాశి vvp_vector4array_t&);
      vvp_vector4array_t& క్రియాకారకం = (స్థిరరాశి vvp_vector4array_t&);
};

/*
 * Statically allocated vvp_vector4array_t
 */
class vvp_vector4array_sa : బహిరంగ vvp_vector4array_t {

    బహిరంగ:
      vvp_vector4array_sa(గుర్తులేని వెడల్పు, గుర్తులేని పదాలు);
      ~vvp_vector4array_sa() override;

      vvp_vector4_t get_మాట(గుర్తులేని idx) స్థిరరాశి override;
      శూన్య సమితి_మాట(గుర్తులేని idx, స్థిరరాశి vvp_vector4_t&that) override;

    అంతరంగ:
      v4cell* array_;
};

/*
 * Automatically allocated vvp_vector4array_t
 */
class vvp_vector4array_aa : బహిరంగ vvp_vector4array_t, బహిరంగ automatic_hooks_s {

    బహిరంగ:
      vvp_vector4array_aa(గుర్తులేని వెడల్పు, గుర్తులేని పదాలు);
      ~vvp_vector4array_aa() override;

      శూన్య alloc_instance(vvp_context_t context) override;
      శూన్య reset_instance(vvp_context_t context) override;
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      శూన్య free_instance(vvp_context_t context) override;
#అయితే_ముగింపు

      vvp_vector4_t get_మాట(గుర్తులేని idx) స్థిరరాశి override;
      శూన్య సమితి_మాట(గుర్తులేని idx, స్థిరరాశి vvp_vector4_t&that) override;

    అంతరంగ:
      గుర్తులేని context_idx_;
};

/* vvp_vector2_t
 */
class vvp_vector2_t {

      మిత్ర vvp_vector2_t క్రియాకారకం - (స్థిరరాశి vvp_vector2_t&);
      మిత్ర vvp_vector2_t క్రియాకారకం + (స్థిరరాశి vvp_vector2_t&,
				       స్థిరరాశి vvp_vector2_t&);
      మిత్ర vvp_vector2_t క్రియాకారకం * (స్థిరరాశి vvp_vector2_t&,
				       స్థిరరాశి vvp_vector2_t&);
      మిత్ర bool క్రియాకారకం >  (స్థిరరాశి vvp_vector2_t&, స్థిరరాశి vvp_vector2_t&);
      మిత్ర bool క్రియాకారకం >= (స్థిరరాశి vvp_vector2_t&, స్థిరరాశి vvp_vector2_t&);
      మిత్ర bool క్రియాకారకం <  (స్థిరరాశి vvp_vector2_t&, స్థిరరాశి vvp_vector2_t&);
      మిత్ర bool క్రియాకారకం <= (స్థిరరాశి vvp_vector2_t&, స్థిరరాశి vvp_vector2_t&);
      మిత్ర bool క్రియాకారకం == (స్థిరరాశి vvp_vector2_t&, స్థిరరాశి vvp_vector2_t&);

    బహిరంగ:
      vvp_vector2_t();
      vvp_vector2_t(స్థిరరాశి vvp_vector2_t&);
      vvp_vector2_t(స్థిరరాశి vvp_vector2_t&, గుర్తులేని newsize);
      vvp_vector2_t(స్థిరరాశి vvp_vector2_t&, గుర్తులేని పునాది, గుర్తులేని wid);
	// Make a vvp_vector2_t from a vvp_vector4_t. If enable_NaN
	// is true and there are X or Z bits, then the result becomes
	// a NaN value. If enable_NaN is false then X or Z bits are
	// converted to 0 as per the standard Verilog rules.
      explicit vvp_vector2_t(స్థిరరాశి vvp_vector4_t&that, bool allow_NaN = false);
	// Make from a 64-bit value and a specified width.
      vvp_vector2_t(uint64_t val, గుర్తులేని wid);
	// Make with the width, and filled with 1 or 0 bits.
      గణన fill_t {FILL0, FILL1};
      vvp_vector2_t(fill_t fill, గుర్తులేని wid);
      ~vvp_vector2_t();

      vvp_vector2_t&క్రియాకారకం += (స్థిరరాశి vvp_vector2_t&that);
      vvp_vector2_t&క్రియాకారకం -= (స్థిరరాశి vvp_vector2_t&that);
      vvp_vector2_t&క్రియాకారకం <<= (గుర్తులేని జరుపు);
      vvp_vector2_t&క్రియాకారకం >>= (గుర్తులేని జరుపు);
      vvp_vector2_t&క్రియాకారకం = (స్థిరరాశి vvp_vector2_t&);

	// Assign a vec4 to a vec2, using Verilog rules for converting
	// XZ values to 0.
      vvp_vector2_t&క్రియాకారకం = (స్థిరరాశి vvp_vector4_t&);

      bool is_NaN() స్థిరరాశి;
      bool is_zero() స్థిరరాశి;
      గుర్తులేని కొలత() స్థిరరాశి;
      పూర్ణాంకం విలువ(గుర్తులేని idx) స్థిరరాశి;
      vvp_bit4_t value4(గుర్తులేని idx) స్థిరరాశి;
	// Get the vector2 subvector starting at the address
      vvp_vector2_t subvalue(గుర్తులేని idx, గుర్తులేని కొలత) స్థిరరాశి;
      శూన్య సమితి_బిట్టు(గుర్తులేని idx, పూర్ణాంకం బిట్టు);
      శూన్య సమితి_vec(గుర్తులేని idx, స్థిరరాశి vvp_vector2_t&that);
	// Make the size just big enough to hold the first 1 bit.
      శూన్య trim();
	// Trim off extra 1 bit since this is representing a negative value.
	// Always keep at least 32 bits.
      శూన్య trim_neg();

    అంతరంగ:
      గణన { బిట్లు_చొప్పున_మాట = 8 * పరిమాణం(గుర్తులేని దీర్ఘ) };
      గుర్తులేని దీర్ఘ*vec_;
      గుర్తులేని wid_;

    అంతరంగ:
      శూన్య copy_from_that_(స్థిరరాశి vvp_vector2_t&that);
      శూన్య copy_from_that_(స్థిరరాశి vvp_vector4_t&that);
};

బాహ్య bool క్రియాకారకం >  (స్థిరరాశి vvp_vector2_t&, స్థిరరాశి vvp_vector2_t&);
బాహ్య bool క్రియాకారకం >= (స్థిరరాశి vvp_vector2_t&, స్థిరరాశి vvp_vector2_t&);
బాహ్య bool క్రియాకారకం <  (స్థిరరాశి vvp_vector2_t&, స్థిరరాశి vvp_vector2_t&);
బాహ్య bool క్రియాకారకం <= (స్థిరరాశి vvp_vector2_t&, స్థిరరాశి vvp_vector2_t&);
బాహ్య bool క్రియాకారకం == (స్థిరరాశి vvp_vector2_t&, స్థిరరాశి vvp_vector2_t&);
బాహ్య vvp_vector2_t క్రియాకారకం - (స్థిరరాశి vvp_vector2_t&);
బాహ్య vvp_vector2_t క్రియాకారకం + (స్థిరరాశి vvp_vector2_t&, స్థిరరాశి vvp_vector2_t&);
బాహ్య vvp_vector2_t క్రియాకారకం * (స్థిరరాశి vvp_vector2_t&, స్థిరరాశి vvp_vector2_t&);
బాహ్య vvp_vector2_t క్రియాకారకం / (స్థిరరాశి vvp_vector2_t&, స్థిరరాశి vvp_vector2_t&);
బాహ్య vvp_vector2_t క్రియాకారకం % (స్థిరరాశి vvp_vector2_t&, స్థిరరాశి vvp_vector2_t&);

vvp_vector2_t ఘాతం(స్థిరరాశి vvp_vector2_t&, vvp_vector2_t&);
బాహ్య vvp_vector4_t vector2_గమ్యం_vector4(స్థిరరాశి vvp_vector2_t&, గుర్తులేని wid);

/* A c4string is of the form C4<...> where ... are bits. */
బాహ్య bool c4string_test(స్థిరరాశి అక్షరం*str);
బాహ్య vvp_vector4_t c4string_గమ్యం_vector4(స్థిరరాశి అక్షరం*str);

/* A crstring is of the form Cr<...> where ... defines are real. */
బాహ్య bool crstring_test(స్థిరరాశి అక్షరం*str);
బాహ్య ద్విగుణ crstring_గమ్యం_double(స్థిరరాశి అక్షరం*str);

బాహ్య std::ostream& క్రియాకారకం<< (std::ostream&, స్థిరరాశి vvp_vector2_t&);

అంతఃవరుస vvp_vector2_t::vvp_vector2_t(స్థిరరాశి vvp_vector2_t&that)
{
      copy_from_that_(that);
}

అంతఃవరుస vvp_vector2_t::vvp_vector2_t(స్థిరరాశి vvp_vector4_t&that, bool enable_NaN)
{
      ఒకవేళ (enable_NaN && that.has_xz()) {
	    vec_ = 0;
	    wid_ = 0;
	    తిరిగి;
      }
      copy_from_that_(that);
}

అంతఃవరుస vvp_vector2_t::~vvp_vector2_t()
{
      delete[] vec_;
}

/* Inline some of the vector2_t methods. */
అంతఃవరుస గుర్తులేని vvp_vector2_t::కొలత() స్థిరరాశి
{
      తిరిగి wid_;
}

అంతఃవరుస vvp_bit4_t vvp_vector2_t::value4(గుర్తులేని idx) స్థిరరాశి
{
      ఒకవేళ (విలువ(idx))
	    తిరిగి BIT4_1;
      లేకపోతే
	    తిరిగి BIT4_0;
}

అంతఃవరుస vvp_vector2_t vvp_vector2_t::subvalue(గుర్తులేని పునాది, గుర్తులేని wid) స్థిరరాశి
{
      తిరిగి vvp_vector2_t(*this, పునాది, wid);
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

      మిత్ర vvp_scalar_t fully_featured_resolv_(vvp_scalar_t a, vvp_scalar_t b);

    బహిరంగ:
	// Make a HiZ value.
      explicit vvp_scalar_t();

	// Make an unambiguous value.
      explicit vvp_scalar_t(vvp_bit4_t val, గుర్తులేని str0, గుర్తులేని str1);

	// Get the vvp_bit4_t version of the value
      vvp_bit4_t విలువ() స్థిరరాశి;
      గుర్తులేని strength0() స్థిరరాశి;
      గుర్తులేని strength1() స్థిరరాశి;

      bool eeq(vvp_scalar_t that) స్థిరరాశి { తిరిగి విలువ_2 == that.విలువ_2; }
      bool is_hiz() స్థిరరాశి { తిరిగి (విలువ_2 & 0x77) == 0; }

    అంతరంగ:
	// This class and the vvp_vector8_t class are closely related,
	// so allow vvp_vector8_t access to the raw encoding so that
	// it can do compact vectoring of vvp_scalar_t objects.
      మిత్ర class vvp_vector8_t;
      explicit vvp_scalar_t(గుర్తులేని అక్షరం val) : విలువ_2(val) { }
      గుర్తులేని అక్షరం raw() స్థిరరాశి { తిరిగి విలువ_2; }

    అంతరంగ:
      గుర్తులేని అక్షరం విలువ_2;
};

అంతఃవరుస vvp_scalar_t::vvp_scalar_t()
{
      విలువ_2 = 0;
}

అంతఃవరుస vvp_scalar_t::vvp_scalar_t(vvp_bit4_t val, గుర్తులేని str0, గుర్తులేని str1)
{
      ప్రతిజ్ఞా(str0 <= 7);
      ప్రతిజ్ఞా(str1 <= 7);

      ఒకవేళ (str0 == 0 && str1 == 0) {
	    విలువ_2 = 0x00;
      } లేకపోతే ఎంపిక (val) {
	  స్థితి BIT4_0:
	    విలువ_2 = str0 | (str0<<4);
	    ఆపు;
	  స్థితి BIT4_1:
	    విలువ_2 = str1 | (str1<<4) | 0x88;
	    ఆపు;
	  స్థితి BIT4_X:
	    విలువ_2 = str0 | (str1<<4) | 0x80;
	    ఆపు;
	  స్థితి BIT4_Z:
	    విలువ_2 = 0x00;
	    ఆపు;
      }
}

అంతఃవరుస vvp_bit4_t vvp_scalar_t::విలువ() స్థిరరాశి
{
      ఒకవేళ ((విలువ_2 & 0x77) == 0) {
	    తిరిగి BIT4_Z;
      } లేకపోతే ఎంపిక (విలువ_2 & 0x88) {
	  స్థితి 0x00:
	    తిరిగి BIT4_0;
	  స్థితి 0x88:
	    తిరిగి BIT4_1;
	  అప్రమేయ:
	    తిరిగి BIT4_X;
      }
}


అంతఃవరుస vvp_scalar_t resolve(vvp_scalar_t a, vvp_scalar_t b)
{
      బాహ్య vvp_scalar_t fully_featured_resolv_(vvp_scalar_t, vvp_scalar_t);

	// If the value is HiZ, resolution is simply a matter of
	// returning the *other* value.
      ఒకవేళ (a.is_hiz())
	    తిరిగి b;
      ఒకవేళ (b.is_hiz())
	    తిరిగి a;
	// If the values are the identical, then resolution is simply
	// returning *either* value.
      ఒకవేళ (a .eeq( b ))
	    తిరిగి a;

      తిరిగి fully_featured_resolv_(a,b);
}

బాహ్య std::ostream& క్రియాకారకం<< (std::ostream&, vvp_scalar_t);

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

      మిత్ర vvp_vector8_t part_expand(స్థిరరాశి vvp_vector8_t&, గుర్తులేని, గుర్తులేని);

    బహిరంగ:
      explicit vvp_vector8_t(గుర్తులేని కొలత =0);
	// Make a vvp_vector8_t from a vector4 and a specified
	// strength.
      explicit vvp_vector8_t(స్థిరరాశి vvp_vector4_t&that,
			     గుర్తులేని str0,
			     గుర్తులేని str1);
      explicit vvp_vector8_t(స్థిరరాశి vvp_vector2_t&that,
			     గుర్తులేని str0,
			     గుర్తులేని str1);

      ~vvp_vector8_t();

      స్థిర స్థిరరాశి vvp_vector8_t nil;

    బహిరంగ:

      గుర్తులేని కొలత() స్థిరరాశి { తిరిగి కొలత_2; }
      vvp_scalar_t విలువ(గుర్తులేని idx) స్థిరరాశి;
      vvp_vector8_t subvalue(గుర్తులేని adr, గుర్తులేని వెడల్పు) స్థిరరాశి;
      శూన్య సమితి_బిట్టు(గుర్తులేని idx, vvp_scalar_t val);
      శూన్య సమితి_vec(గుర్తులేని idx, స్థిరరాశి vvp_vector8_t&that);

	// Test that the vectors are exactly equal
      bool eeq(స్థిరరాశి vvp_vector8_t&that) స్థిరరాశి;

      vvp_vector8_t(స్థిరరాశి vvp_vector8_t&that);
      vvp_vector8_t& క్రియాకారకం= (స్థిరరాశి vvp_vector8_t&that);

    అంతరంగ:
      గుర్తులేని కొలత_2;
      సంఘం {
	    గుర్తులేని అక్షరం*ptr_;
	    గుర్తులేని అక్షరం val_[పరిమాణం(శూన్య*)];
      };
};

  /* Resolve uses the default Verilog resolver algorithm to resolve
     two drive vectors to a single output. */
అంతఃవరుస vvp_vector8_t resolve(స్థిరరాశి vvp_vector8_t&a, స్థిరరాశి vvp_vector8_t&b)
{
      ప్రతిజ్ఞా(a.కొలత() == b.కొలత());
      vvp_vector8_t బహిర్గతం (a.కొలత());

      కొరకు (గుర్తులేని idx = 0 ;  idx < బహిర్గతం.కొలత() ;  idx += 1) {
	    బహిర్గతం.సమితి_బిట్టు(idx, resolve(a.విలువ(idx), b.విలువ(idx)));
      }

      తిరిగి బహిర్గతం;
}

  /* This lookup tabke implements the strength reduction implied by
     Verilog standard switch devices. The major dimension selects
     between non-resistive and resistive devices. */
బాహ్య గుర్తులేని vvp_switch_strength_map[2][8];

  /* The reduce4 function converts a vector8 to a vector4, losing
     strength information in the process. */
బాహ్య vvp_vector4_t reduce4(స్థిరరాశి vvp_vector8_t&that);
బాహ్య vvp_vector8_t part_expand(స్థిరరాశి vvp_vector8_t&a, గుర్తులేని wid, గుర్తులేని off);

  /* A c8string is of the form C8<...> where ... are bits. */
బాహ్య bool c8string_test(స్థిరరాశి అక్షరం*str);
బాహ్య vvp_vector8_t c8string_గమ్యం_vector8(స్థిరరాశి అక్షరం*str);

  /* Print a vector8 value to a stream. */
బాహ్య std::ostream& క్రియాకారకం<< (std::ostream&, స్థిరరాశి vvp_vector8_t&);

అంతఃవరుస vvp_vector8_t::vvp_vector8_t(గుర్తులేని కొలత_3)
: కొలత_2(కొలత_3)
{
      ఒకవేళ (కొలత_2 <= పరిమాణం(val_)) {
	    memset(val_, 0, పరిమాణం(val_));
      } లేకపోతే {
	    ptr_ = new గుర్తులేని అక్షరం[కొలత_2];
	    memset(ptr_, 0, కొలత_2);
      }
}

అంతఃవరుస vvp_vector8_t::~vvp_vector8_t()
{
      ఒకవేళ (కొలత_2 > పరిమాణం(val_))
	    delete[]ptr_;
}

అంతఃవరుస vvp_scalar_t vvp_vector8_t::విలువ(గుర్తులేని idx) స్థిరరాశి
{
      ప్రతిజ్ఞా(idx < కొలత_2);
      ఒకవేళ (కొలత_2 <= పరిమాణం(val_))
	    తిరిగి vvp_scalar_t(val_[idx]);
      లేకపోతే
	    తిరిగి vvp_scalar_t(ptr_[idx]);
}

అంతఃవరుస శూన్య vvp_vector8_t::సమితి_బిట్టు(గుర్తులేని idx, vvp_scalar_t val)
{
      ప్రతిజ్ఞా(idx < కొలత_2);
      ఒకవేళ (కొలత_2 <= పరిమాణం(val_))
	    val_[idx] = val.raw();
      లేకపోతే
	    ptr_[idx] = val.raw();
}

  // Exactly-equal for vvp_vector8_t is common and should be as tight
  // as possible.
అంతఃవరుస bool vvp_vector8_t::eeq(స్థిరరాశి vvp_vector8_t&that) స్థిరరాశి
{
      ఒకవేళ (కొలత_2 != that.కొలత_2)
	    తిరిగి false;
      ఒకవేళ (కొలత_2 == 0)
	    తిరిగి true;

      ఒకవేళ (కొలత_2 <= పరిమాణం(val_))
	      // This is equivalent to memcmp(val_, that.val_, sizeof val_)==0
	    తిరిగి ptr_ == that.ptr_;
      లేకపోతే
	    తిరిగి memcmp(ptr_, that.ptr_, కొలత_2) == 0;
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
మూస <class T> class vvp_sub_సూచిక_t {

    బహిరంగ:
      vvp_sub_సూచిక_t() : బిట్లు_2(0) { }

      vvp_sub_సూచిక_t(T*ptr__, గుర్తులేని పోర్టు_2)
      {
	    బిట్లు_2 = reinterpret_cast<uintptr_t> (ptr__);
	    ప్రతిజ్ఞా( (బిట్లు_2  &  UINTPTR_C(3)) == 0 );
	    ప్రతిజ్ఞా( (పోర్టు_2 & ~UINTPTR_C(3)) == 0 );
	    బిట్లు_2 |= పోర్టు_2;
      }

      ~vvp_sub_సూచిక_t() = అప్రమేయ;

      T* ptr()
      { తిరిగి reinterpret_cast<T*> (బిట్లు_2 & ~UINTPTR_C(3)); }

      స్థిరరాశి T* ptr() స్థిరరాశి
      { తిరిగి reinterpret_cast<స్థిరరాశి T*> (బిట్లు_2 & ~UINTPTR_C(3)); }

      గుర్తులేని  పోర్టు() స్థిరరాశి { తిరిగి బిట్లు_2 & UINTPTR_C(3); }

      bool nil() స్థిరరాశి { తిరిగి బిట్లు_2 == 0; }

      bool క్రియాకారకం == (vvp_sub_సూచిక_t that) స్థిరరాశి { తిరిగి బిట్లు_2 == that.బిట్లు_2; }
      bool క్రియాకారకం != (vvp_sub_సూచిక_t that) స్థిరరాశి { తిరిగి బిట్లు_2 != that.బిట్లు_2; }

    అంతరంగ:
      uintptr_t బిట్లు_2;
};

రకనిర్వచనం vvp_sub_సూచిక_t<vvp_net_t> vvp_net_ptr_t;
మూస <class T> std::ostream& క్రియాకారకం << (std::ostream&బహిర్గతం, vvp_sub_సూచిక_t<T> val)
{ బహిర్గతం << val.ptr() << "[" << val.పోర్టు() << "]"; తిరిగి బహిర్గతం; }

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
    బహిరంగ:
      vvp_net_t();

#నిర్వచితమైతే CHECK_WITH_VALGRIND
      vvp_net_t *pool;
#అయితే_ముగింపు
      vvp_net_ptr_t పోర్టు[4];
      vvp_net_fun_t*fun;
      vvp_net_fil_t*fil;

    బహిరంగ:
	// Connect the port to the output from this net.
      శూన్య link(vvp_net_ptr_t పోర్టు);
	// Disconnect the port from the output of this net.
      శూన్య unlink(vvp_net_ptr_t పోర్టు);

    బహిరంగ: // Methods to propagate output from this node.
      శూన్య send_vec4(స్థిరరాశి vvp_vector4_t&val, vvp_context_t context);
      శూన్య send_vec8(స్థిరరాశి vvp_vector8_t&val);
      శూన్య send_real(ద్విగుణ val, vvp_context_t context);
      శూన్య send_string(స్థిరరాశి std::string&val, vvp_context_t context);
      శూన్య send_object(vvp_object_t val, vvp_context_t context);

      శూన్య send_vec4_pv(స్థిరరాశి vvp_vector4_t&val,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t context);
      శూన్య send_vec8_pv(స్థిరరాశి vvp_vector8_t&val,
			గుర్తులేని పునాది, గుర్తులేని vwid);


    బహిరంగ: // Methods to arrange for the output of this net to be forced.

	// The intent is that all efforts at force are directed to
	// operate only on the vvp_net_t whose output is to be
	// forced. These methods then communicate the force to the
	// attached filter to set up the actual force.
      శూన్య force_vec4(స్థిరరాశి vvp_vector4_t&val, స్థిరరాశి vvp_vector2_t&ముసుగు);
      శూన్య force_vec8(స్థిరరాశి vvp_vector8_t&val, స్థిరరాశి vvp_vector2_t&ముసుగు);
      శూన్య force_real(ద్విగుణ val, స్థిరరాశి vvp_vector2_t&ముసుగు);

    బహిరంగ: // Method to support $countdrivers
      శూన్య లెక్క_drivers(గుర్తులేని idx, గుర్తులేని counts[4]);

    // This needs to be public so that SDF interconnects can be inserted
    బహిరంగ:
      vvp_net_ptr_t బహిర్గతం_2;

    బహిరంగ: // Need a better new for these objects.
      స్థిర శూన్య* క్రియాకారకం new(std::size_t కొలత);
      స్థిర శూన్య క్రియాకారకం delete(శూన్య*); // not implemented
    అంతరంగ: // not implemented
      స్థిర శూన్య* క్రియాకారకం new[](std::size_t కొలత);
#ఒకవేళ defined(__GNUC__)
      స్థిర శూన్య క్రియాకారకం delete[](శూన్య*);
#అయితే_ముగింపు
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

    బహిరంగ:
      vvp_net_fun_t();
      మాయా ~vvp_net_fun_t();

      మాయా శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                             vvp_context_t context);
      మాయా శూన్య recv_vec8(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector8_t&బిట్టు);
      మాయా శూన్య recv_real(vvp_net_ptr_t పోర్టు, ద్విగుణ బిట్టు,
                             vvp_context_t context);
      మాయా శూన్య recv_string(vvp_net_ptr_t పోర్టు, స్థిరరాశి std::string&బిట్టు,
			       vvp_context_t context);
      మాయా శూన్య recv_object(vvp_net_ptr_t పోర్టు, vvp_object_t బిట్టు,
			       vvp_context_t context);

	// Part select variants of above
      మాయా శూన్య recv_vec4_pv(vvp_net_ptr_t p, స్థిరరాశి vvp_vector4_t&బిట్టు,
				గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t context);
      మాయా శూన్య recv_vec8_pv(vvp_net_ptr_t p, స్థిరరాశి vvp_vector8_t&బిట్టు,
				గుర్తులేని పునాది, గుర్తులేని vwid);

	// This method is called when the net is forced or
	// released. This is very rarely needed; island ports use it
	// to know that the net is being forced and that it needs to
	// do something about it.
      మాయా శూన్య force_flag(bool నిరంతరం_now);

   రక్షిత:
      శూన్య recv_vec4_pv_(vvp_net_ptr_t p, స్థిరరాశి vvp_vector4_t&బిట్టు,
			 గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t context);
      శూన్య recv_vec8_pv_(vvp_net_ptr_t p, స్థిరరాశి vvp_vector8_t&బిట్టు,
			 గుర్తులేని పునాది, గుర్తులేని vwid);

    బహిరంగ: // These objects are only permallocated.
      స్థిర శూన్య* క్రియాకారకం new(std::size_t కొలత) { తిరిగి కుప్ప.alloc(కొలత); }
      స్థిర శూన్య క్రియాకారకం delete(శూన్య*); // not implemented

      స్థిర std::size_t కుప్ప_మొత్తం() { తిరిగి కుప్ప.కుప్ప_మొత్తం(); }

    రక్షిత:
      స్థిర permaheap కుప్ప;

    అంతరంగ: // not implemented
      vvp_net_fun_t(స్థిరరాశి vvp_net_fun_t&);
      vvp_net_fun_t& క్రియాకారకం= (స్థిరరాశి vvp_net_fun_t&);
      స్థిర శూన్య* క్రియాకారకం new[](std::size_t కొలత);
      స్థిర శూన్య క్రియాకారకం delete[](శూన్య*);
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
class vvp_net_fil_t  : బహిరంగ vvp_vpi_callback {

    బహిరంగ:
      vvp_net_fil_t();
      మాయా ~vvp_net_fil_t() override;

    బహిరంగ:
      గణన prop_t { STOP=0, PROP, REPL };


	// These filter methods are used by the vvp_net_t::send_*()
	// methods to test the output (from the functor) bit value
	// against any force filters. If none of the bits are forced,
	// then the method returns PROP and the caller propagates the
	// bit value. If bits were changed by the force mask, then the
	// method returns REPL and the caller should propagate the rep
	// value instead. If the function returns STOP, then all the
	// output bits are filtered by the force mask and there is
	// nothing to propagate.
      మాయా prop_t filter_vec4(స్థిరరాశి vvp_vector4_t&బిట్టు, vvp_vector4_t&rep,
				 గుర్తులేని పునాది, గుర్తులేని vwid);
      మాయా prop_t filter_vec8(స్థిరరాశి vvp_vector8_t&val, vvp_vector8_t&rep,
				 గుర్తులేని పునాది, గుర్తులేని vwid);
      మాయా prop_t filter_real(ద్విగుణ&val);
      మాయా prop_t filter_object(vvp_object_t&val);
      మాయా prop_t filter_string(స్థిరరాశి std::string&val);

      మాయా శూన్య release(vvp_net_ptr_t ptr, bool net_flag) =0;
      మాయా శూన్య release_pv(vvp_net_ptr_t ptr, గుర్తులేని పునాది, గుర్తులేని wid, bool net_flag) =0;

	// The %force/link instruction needs a place to write the
	// source node of the force, so that subsequent %force and
	// %release instructions can undo the link as needed. */
      శూన్య force_link(vvp_net_t*dst, vvp_net_t*src);
      శూన్య force_unlink(శూన్య);

      మాయా గుర్తులేని filter_కొలత() స్థిరరాశి =0;

    బహిరంగ:
	// Support for force methods. These are called by the
	// vvp_net_t::force_* methods to set the force value and mask
	// for the filter.
      మాయా శూన్య force_fil_vec4(స్థిరరాశి vvp_vector4_t&val, స్థిరరాశి vvp_vector2_t&ముసుగు) =0;
      మాయా శూన్య force_fil_vec8(స్థిరరాశి vvp_vector8_t&val, స్థిరరాశి vvp_vector2_t&ముసుగు) =0;
      మాయా శూన్య force_fil_real(ద్విగుణ val, స్థిరరాశి vvp_vector2_t&ముసుగు) =0;

    బహిరంగ: // These objects are only permallocated.
      స్థిర శూన్య* క్రియాకారకం new(std::size_t కొలత) { తిరిగి కుప్ప.alloc(కొలత); }
      స్థిర శూన్య క్రియాకారకం delete(శూన్య*); // not implemented

      స్థిర size_t కుప్ప_మొత్తం() { తిరిగి కుప్ప.కుప్ప_మొత్తం(); }

    అంతరంగ:
      స్థిర permaheap కుప్ప;

    అంతరంగ: // not implemented
      vvp_net_fil_t(స్థిరరాశి vvp_net_fil_t&);
      vvp_net_fil_t& క్రియాకారకం= (స్థిరరాశి vvp_net_fil_t&);
      స్థిర శూన్య* క్రియాకారకం new[](std::size_t కొలత);
      స్థిర శూన్య క్రియాకారకం delete[](శూన్య*);

    రక్షిత:
	// Set bits of the filter force mask
      శూన్య force_ముసుగు(స్థిరరాశి vvp_vector2_t&ముసుగు);
	// Release the force on the bits set in the mask.
      శూన్య release_ముసుగు(స్థిరరాశి vvp_vector2_t&ముసుగు);
	// Test bits of the filter force mask;
      bool test_force_ముసుగు(గుర్తులేని బిట్టు) స్థిరరాశి;
      bool test_force_ముసుగు_is_zero() స్థిరరాశి;

	// This template method is used by derived classes to process
	// the val through the force mask. The force value is the
	// currently forced value, and the buf is a value that this
	// method will use to hold a filtered value, if needed. This
	// method returns the replacement value into the "rep"
	// argument and returns a code indicating whether any changes
	// were made.
      మూస <class T> prop_t filter_ముసుగు(స్థిరరాశి T&val, స్థిరరాశి T&force, T&rep, గుర్తులేని addr);
	// This template method is similar to the above, but works for
	// native types that are not so expensive to edit in place.
      మూస <class T> prop_t filter_ముసుగు(T&val, T force);

	// These templates are similar to filter_mask_, but are
	// idempotent. Then do not trigger callbacks or otherwise
	// cause any local changes. These methods are used to test
	// arbitrary values against the force mask.
      మూస <class T> prop_t filter_ఆదానం_ముసుగు(స్థిరరాశి T&val, స్థిరరాశి T&force, T&rep) స్థిరరాశి;

    అంతరంగ:
	// Mask of forced bits
      vvp_vector2_t force_ముసుగు_2;
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
class vvp_fun_force : బహిరంగ vvp_net_fun_t {

    బహిరంగ:
      vvp_fun_force();
      ~vvp_fun_force() override;

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
		     vvp_context_t context) override;
      శూన్య recv_real(vvp_net_ptr_t పోర్టు, ద్విగుణ బిట్టు, vvp_context_t) override;
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
class vvp_fun_drive  : బహిరంగ vvp_net_fun_t {

    బహిరంగ:
      explicit vvp_fun_drive(గుర్తులేని str0 =6, గుర్తులేని str1 =6);
      ~vvp_fun_drive() override;

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t context) override;

      శూన్య recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t) override;
    అంతరంగ:
      గుర్తులేని అక్షరం drive0_;
      గుర్తులేని అక్షరం drive1_;
};

/*
 * EXTEND functors expand an input vector to the desired output
 * width. The extend_signed functor sign extends the input. If the
 * input is already wider than the desired output, then it is passed
 * unmodified.
 */
class vvp_fun_extend_signed  : బహిరంగ vvp_net_fun_t {

    బహిరంగ:
      explicit vvp_fun_extend_signed(గుర్తులేని wid);
      ~vvp_fun_extend_signed() override;

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t context) override;

      శూన్య recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t) override;
    అంతరంగ:
      గుర్తులేని వెడల్పు_2;
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

class vvp_wide_fun_core : బహిరంగ vvp_net_fun_t {

    బహిరంగ:
      vvp_wide_fun_core(vvp_net_t*net, గుర్తులేని nports);
      మాయా ~vvp_wide_fun_core() override;
	// These objects are not perm allocated.
      స్థిర శూన్య* క్రియాకారకం new(std::size_t కొలత) { తిరిగి ::new అక్షరం[కొలత]; }
      స్థిర శూన్య క్రియాకారకం delete(శూన్య* ptr) { ::delete[](static_cast<అక్షరం*>(ptr)); }

    రక్షిత:
      శూన్య propagate_vec4(స్థిరరాశి vvp_vector4_t&బిట్టు, vvp_time64_t delay =0);
      శూన్య propagate_real(ద్విగుణ బిట్టు, vvp_time64_t delay =0);
      గుర్తులేని పోర్టు_లెక్క() స్థిరరాశి;

      vvp_vector4_t& విలువ(గుర్తులేని);
      ద్విగుణ విలువ_r(గుర్తులేని);

    అంతరంగ:
	// the derived class implements this to receive an indication
	// that one of the port input values changed.
      మాయా శూన్య recv_vec4_from_inputs(గుర్తులేని పోర్టు) =0;
      మాయా శూన్య recv_real_from_inputs(గుర్తులేని పోర్టు);

      మిత్ర class vvp_wide_fun_t;
      శూన్య dispatch_vec4_from_ఆదానం(గుర్తులేని పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు);
      శూన్య dispatch_real_from_ఆదానం(గుర్తులేని పోర్టు, ద్విగుణ బిట్టు);

    అంతరంగ:
	// Back-point to the vvp_net_t that points to me.
      vvp_net_t*ptr_;
	// Structure to track the input values from the input functors.
      గుర్తులేని nports_;
      vvp_vector4_t*పోర్టు_విలువలు;
      ద్విగుణ*పోర్టు_rvalues;
};

/*
 * The job of the input functor is only to monitor inputs to the
 * function and pass them to the ufunc_core object. Each functor takes
 * up to 4 inputs, with the base the port number for the first
 * function input that this functor represents.
 */
class vvp_wide_fun_t : బహిరంగ vvp_net_fun_t {

    బహిరంగ:
      vvp_wide_fun_t(vvp_wide_fun_core*c, గుర్తులేని పునాది);
      ~vvp_wide_fun_t() override;

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t context) override;
      శూన్య recv_real(vvp_net_ptr_t పోర్టు, ద్విగుణ బిట్టు,
                     vvp_context_t context) override;

      శూన్య recv_vec4_pv(vvp_net_ptr_t p, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t context) override;

    అంతరంగ:
      vvp_wide_fun_core*core_;
      గుర్తులేని పోర్టు_పునాది_2;
};


అంతఃవరుస శూన్య vvp_send_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&val, vvp_context_t context)
{
      అయితే (class vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t తదుపరి_val = cur->పోర్టు[ptr.పోర్టు()];

	    ఒకవేళ (cur->fun)
		  cur->fun->recv_vec4(ptr, val, context);

	    ptr = తదుపరి_val;
      }
}

బాహ్య శూన్య vvp_send_vec8(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector8_t&val);
బాహ్య శూన్య vvp_send_real(vvp_net_ptr_t ptr, ద్విగుణ val,
                          vvp_context_t context);

అంతఃవరుస శూన్య vvp_send_string(vvp_net_ptr_t ptr, స్థిరరాశి std::string&val, vvp_context_t context)
{
      అయితే (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t తదుపరి_val = cur->పోర్టు[ptr.పోర్టు()];

	    ఒకవేళ (cur->fun)
		  cur->fun->recv_string(ptr, val, context);

	    ptr = తదుపరి_val;
      }
}

అంతఃవరుస శూన్య vvp_send_object(vvp_net_ptr_t ptr, vvp_object_t val, vvp_context_t context)
{
      అయితే (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t తదుపరి_val = cur->పోర్టు[ptr.పోర్టు()];

	    ఒకవేళ (cur->fun)
		  cur->fun->recv_object(ptr, val, context);

	    ptr = తదుపరి_val;
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
అంతఃవరుస శూన్య vvp_send_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&val,
			     గుర్తులేని పునాది, గుర్తులేని vwid,
			     vvp_context_t context)
{
      అయితే (class vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t తదుపరి_val = cur->పోర్టు[ptr.పోర్టు()];

	    ఒకవేళ (cur->fun)
		  cur->fun->recv_vec4_pv(ptr, val, పునాది, vwid, context);

	    ptr = తదుపరి_val;
      }
}

అంతఃవరుస శూన్య vvp_send_vec8_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector8_t&val,
			     గుర్తులేని పునాది, గుర్తులేని vwid)
{
      అయితే (class vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t తదుపరి_val = cur->పోర్టు[ptr.పోర్టు()];

	    ఒకవేళ (cur->fun)
		  cur->fun->recv_vec8_pv(ptr, val, పునాది, vwid);

	    ptr = తదుపరి_val;
      }
}

అంతఃవరుస శూన్య vvp_net_t::send_vec4(స్థిరరాశి vvp_vector4_t&val, vvp_context_t context)
{
      ఒకవేళ (fil == 0) {
	    vvp_send_vec4(బహిర్గతం_2, val, context);
	    తిరిగి;
      }

      vvp_vector4_t rep;
      ఎంపిక (fil->filter_vec4(val, rep, 0, val.కొలత())) {
	  స్థితి vvp_net_fil_t::STOP:
	    ఆపు;
	  స్థితి vvp_net_fil_t::PROP:
	    vvp_send_vec4(బహిర్గతం_2, val, context);
	    ఆపు;
	  స్థితి vvp_net_fil_t::REPL:
	    vvp_send_vec4(బహిర్గతం_2, rep, context);
	    ఆపు;
      }
}

అంతఃవరుస శూన్య vvp_net_t::send_vec4_pv(స్థిరరాశి vvp_vector4_t&val,
				    గుర్తులేని పునాది, గుర్తులేని vwid,
				    vvp_context_t context)
{
      ఒకవేళ (fil == 0) {
	    vvp_send_vec4_pv(బహిర్గతం_2, val, పునాది, vwid, context);
	    తిరిగి;
      }

      vvp_vector4_t rep;
      ఎంపిక (fil->filter_vec4(val, rep, పునాది, vwid)) {
	  స్థితి vvp_net_fil_t::STOP:
	    ఆపు;
	  స్థితి vvp_net_fil_t::PROP:
	    vvp_send_vec4_pv(బహిర్గతం_2, val, పునాది, vwid, context);
	    ఆపు;
	  స్థితి vvp_net_fil_t::REPL:
	    vvp_send_vec4_pv(బహిర్గతం_2, rep, పునాది, vwid, context);
	    ఆపు;
      }
}

అంతఃవరుస శూన్య vvp_net_t::send_vec8(స్థిరరాశి vvp_vector8_t&val)
{
      ఒకవేళ (fil == 0) {
	    vvp_send_vec8(బహిర్గతం_2, val);
	    తిరిగి;
      }

      vvp_vector8_t rep;
      ఎంపిక (fil->filter_vec8(val, rep, 0, val.కొలత())) {
	  స్థితి vvp_net_fil_t::STOP:
	    ఆపు;
	  స్థితి vvp_net_fil_t::PROP:
	    vvp_send_vec8(బహిర్గతం_2, val);
	    ఆపు;
	  స్థితి vvp_net_fil_t::REPL:
	    vvp_send_vec8(బహిర్గతం_2, rep);
	    ఆపు;
      }
}

అంతఃవరుస శూన్య vvp_net_t::send_vec8_pv(స్థిరరాశి vvp_vector8_t&val,
				    గుర్తులేని పునాది, గుర్తులేని vwid)
{
      ఒకవేళ (fil == 0) {
	    vvp_send_vec8_pv(బహిర్గతం_2, val, పునాది, vwid);
	    తిరిగి;
      }

      vvp_vector8_t rep;
      ఎంపిక (fil->filter_vec8(val, rep, పునాది, vwid)) {
	  స్థితి vvp_net_fil_t::STOP:
	    ఆపు;
	  స్థితి vvp_net_fil_t::PROP:
	    vvp_send_vec8_pv(బహిర్గతం_2, val, పునాది, vwid);
	    ఆపు;
	  స్థితి vvp_net_fil_t::REPL:
	    vvp_send_vec8_pv(బహిర్గతం_2, rep, పునాది, vwid);
	    ఆపు;
      }
}

అంతఃవరుస శూన్య vvp_net_t::send_real(ద్విగుణ val, vvp_context_t context)
{
      ఒకవేళ (fil && ! fil->filter_real(val))
	    తిరిగి;

      vvp_send_real(బహిర్గతం_2, val, context);
}


అంతఃవరుస శూన్య vvp_net_t::send_string(స్థిరరాశి std::string&val, vvp_context_t context)
{
      ఒకవేళ (fil && !fil->filter_string(val))
	    తిరిగి;

      vvp_send_string(బహిర్గతం_2, val, context);
}


అంతఃవరుస శూన్య vvp_net_t::send_object(vvp_object_t val, vvp_context_t context)
{
      ఒకవేళ (fil && ! fil->filter_object(val))
	    తిరిగి;

      vvp_send_object(బహిర్గతం_2, val, context);
}


అంతఃవరుస bool vvp_net_fil_t::test_force_ముసుగు(గుర్తులేని బిట్టు) స్థిరరాశి
{
      ఒకవేళ (బిట్టు >= force_ముసుగు_2.కొలత())
	    తిరిగి false;
      ఒకవేళ (force_ముసుగు_2.విలువ(బిట్టు))
	    తిరిగి true;
      లేకపోతే
	    తిరిగి false;
}

అంతఃవరుస bool vvp_net_fil_t::test_force_ముసుగు_is_zero(శూన్య) స్థిరరాశి
{
      ఒకవేళ (force_ముసుగు_2.కొలత() == 0)
	    తిరిగి true;
      ఒకవేళ (force_ముసుగు_2.is_zero())
	    తిరిగి true;
      తిరిగి false;
}

/*
 * Undefine the ivl_alloc.h definitions so they don't leak out of this file.
 */
#నిర్వచనం_రద్దు జ్ఞాపకం_ఇవ్వు
#నిర్వచనం_రద్దు realloc
#నిర్వచనం_రద్దు సున్నా_జ్ఞాపకం_ఇవ్వు
#నిర్వచనం_రద్దు __ivl_alloc_H

#అయితే_ముగింపు /* IVL_vvp_net_H */
