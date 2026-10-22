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
#ଅନ୍ୟଥା
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ**vvp_context_t;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ*vvp_context_item_t;

ଧାଡ଼ିଭିତରେ vvp_context_t vvp_allocate_context(unsigned nitem)
{
      ଫେରନ୍ତୁ (vvp_context_t)malloc((2 + nitem) * ଆକାର(ଶୂନ୍ୟ*));
}

ଧାଡ଼ିଭିତରେ vvp_context_t vvp_get_next_context(vvp_context_t context)
{
      ଫେରନ୍ତୁ (vvp_context_t)context[0];
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ vvp_set_next_context(vvp_context_t context, vvp_context_t next)
{
      context[0] = next;
}

ଧାଡ଼ିଭିତରେ vvp_context_t vvp_get_stacked_context(vvp_context_t context)
{
      ଫେରନ୍ତୁ (vvp_context_t)context[1];
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ vvp_set_stacked_context(vvp_context_t context, vvp_context_t ଗଦା)
{
      context[1] = ଗଦା;
}

ଧାଡ଼ିଭିତରେ vvp_context_item_t vvp_get_context_item(vvp_context_t context,
                                               unsigned item_idx)
{
      ଫେରନ୍ତୁ (vvp_context_item_t)context[item_idx];
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ vvp_set_context_item(vvp_context_t context, unsigned item_idx,
                                 vvp_context_item_t item)
{
      context[item_idx] = item;
}

/*
 * An "automatic" functor is one which may be associated with an automatically
 * allocated scope item. This provides the infrastructure needed to allocate
 * the state information for individual instances of the item.
 */
ସଂରଚନା automatic_hooks_s {

      automatic_hooks_s() {}
      ଆଭାସୀ ~automatic_hooks_s() {}

      ଆଭାସୀ ଶୂନ୍ୟ alloc_instance(vvp_context_t context) = 0;
      ଆଭାସୀ ଶୂନ୍ୟ reset_instance(vvp_context_t context) = 0;
#ifdef CHECK_WITH_VALGRIND
      ଆଭାସୀ ଶୂନ୍ୟ free_instance(vvp_context_t context) = 0;
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
ଗଣନା vvp_bit4_t {
      BIT4_0 = 0,
      BIT4_1 = 1,
      BIT4_X = 3,
      BIT4_Z = 2
};

  /* Return an ASCII character that represents the vvp_bit4_t value. */
ଧାଡ଼ିଭିତରେ ଅକ୍ଷର vvp_bit4_to_ascii(vvp_bit4_t a) { ଫେରନ୍ତୁ "01zx"[a]; }

ବାହ୍ୟ vvp_bit4_t add_with_carry(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c);

ବାହ୍ୟ vvp_bit4_t scalar_to_bit4(PLI_INT32 scalar);

  /* Return TRUE if the bit is BIT4_X or BIT4_Z. The fast
     implementation here relies on the encoding of vvp_bit4_t values. */
ଧାଡ଼ିଭିତରେ bool bit4_is_xz(vvp_bit4_t a) { ଫେରନ୍ତୁ a >= 2; }

  /* This function converts BIT4_Z to BIT4_X, but passes other values
     unchanged. This fast implementation relies of the encoding of the
     vvp_bit4_t values. In particular, the BIT4_X==3 and BIT4_Z==2 */
ଧାଡ଼ିଭିତରେ vvp_bit4_t bit4_z2x(vvp_bit4_t a)
{ ଫେରନ୍ତୁ (vvp_bit4_t) ( (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)a | ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)a >> 1) ); }

  /* Some common boolean operators. These implement the Verilog rules
     for 4-value bit operations. The fast implementations here rely
     on the encoding of vvp_bit4_t values. */

  // ~BIT4_0 --> BIT4_1
  // ~BIT4_1 --> BIT4_0
  // ~BIT4_X --> BIT4_X
  // ~BIT4_Z --> BIT4_X
ଧାଡ଼ିଭିତରେ vvp_bit4_t operator ~ (vvp_bit4_t a)
{ ଫେରନ୍ତୁ bit4_z2x((vvp_bit4_t) (((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)a) ^ 1)); }

ଧାଡ଼ିଭିତରେ vvp_bit4_t operator | (vvp_bit4_t a, vvp_bit4_t b)
{
      ଯଦି (a==BIT4_1 || b==BIT4_1)
	    ଫେରନ୍ତୁ BIT4_1;
      ଫେରନ୍ତୁ bit4_z2x( (vvp_bit4_t) ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)a | (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)b) );
}

ଧାଡ଼ିଭିତରେ vvp_bit4_t operator & (vvp_bit4_t a, vvp_bit4_t b)
{
      ଯଦି (a==BIT4_0 || b==BIT4_0)
	    ଫେରନ୍ତୁ BIT4_0;
      ଫେରନ୍ତୁ bit4_z2x( (vvp_bit4_t) ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)a | (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)b) );
}


ବାହ୍ୟ vvp_bit4_t operator ^ (vvp_bit4_t a, vvp_bit4_t b);
ବାହ୍ୟ std::ostream& operator<< (std::ostream&o, vvp_bit4_t a);

  /* Return >0, ==0 or <0 if the from-to transition represents a
     posedge, no edge, or negedge. */
ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା edge(vvp_bit4_t from, vvp_bit4_t to);

  /* Support for $countdrivers. */
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ update_driver_counts(vvp_bit4_t bit, unsigned counts[3])
{
      ବାଛ (bit) {
          ସ୍ଥିତି BIT4_0:
            counts[0] += 1;
            ଭାଙ୍ଗ;
          ସ୍ଥିତି BIT4_1:
            counts[1] += 1;
            ଭାଙ୍ଗ;
          ସ୍ଥିତି BIT4_X:
            counts[2] += 1;
            ଭାଙ୍ଗ;
          ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            ଭାଙ୍ଗ;
      }
}

/*
 * Some of the instructions do wide addition to arrays of long. They
 * use this add_with_carry function to help.
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ unsigned long add_with_carry(unsigned long a, unsigned long b,
					   unsigned long&carry)
{
      unsigned long tmp = b + carry;
      unsigned long sum = a + tmp;
      carry = 0;
      ଯଦି (tmp < b)
	    carry = 1;
      ଯଦି (sum < tmp)
	    carry = 1;
      ଯଦି (sum < a)
	    carry = 1;
      ଫେରନ୍ତୁ sum;
}

ବାହ୍ୟ unsigned long multiply_with_carry(unsigned long a, unsigned long b,
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

      friend vvp_vector4_t operator ~(ଅଚଳ vvp_vector4_t&that);
      friend class vvp_vector4array_t;
      friend class vvp_vector4array_sa;
      friend class vvp_vector4array_aa;

    ସର୍ବସାଧାରଣ:
      ସ୍ଥିର ଅଚଳ vvp_vector4_t nil;

    ସର୍ବସାଧାରଣ:
      explicit vvp_vector4_t(unsigned size =0, vvp_bit4_t bits =BIT4_X);

      explicit vvp_vector4_t(unsigned size, ଦ୍ୱିଗୁଣ val);

	// Construct a vector4 from the subvalue of another
	// vector4. The width of the result is 'wid', and the bits are
	// pulled from 'that' to implement the Verilog part select
	// semantics. This means that part select beyond 'that'
	// returns X bits.
      explicit vvp_vector4_t(ଅଚଳ vvp_vector4_t&that,
			     unsigned adr, unsigned wid);

      vvp_vector4_t(ଅଚଳ vvp_vector4_t&that);
      vvp_vector4_t(ଅଚଳ vvp_vector4_t&that, bool invert_flag);
      vvp_vector4_t& operator= (ଅଚଳ vvp_vector4_t&that);

      ~vvp_vector4_t();

      ଧାଡ଼ିଭିତରେ unsigned size() ଅଚଳ { ଫେରନ୍ତୁ size_; }
      ଶୂନ୍ୟ resize(unsigned new_size, vvp_bit4_t pad_bit = BIT4_X);

	// Get the bit at the specified address
      vvp_bit4_t ମୂଲ୍ୟ(unsigned idx) ଅଚଳ;
	// Get the vector4 subvector starting at the address
      vvp_vector4_t subvalue(unsigned idx, unsigned size) ଅଚଳ;
	// Get the 2-value bits for the subvector. This returns a new
	// array of longs, or a nil pointer if an XZ bit was detected
	// in the array.
      unsigned long*subarray(unsigned idx, unsigned size, bool xz_to_0 =false) ଅଚଳ;
      ଶୂନ୍ୟ setarray(unsigned idx, unsigned size, ଅଚଳ unsigned long*val);

	// Set a 4-value bit or subvector into the vector. Return true
	// if any bits of the vector change as a result of this operation.
      ଶୂନ୍ୟ set_bit(unsigned idx, vvp_bit4_t val);
      bool set_vec(unsigned idx, ଅଚଳ vvp_vector4_t&that);

        // Get the bits from another vector, but keep my size.
      ଶୂନ୍ୟ copy_bits(ଅଚଳ vvp_vector4_t&that);

	// Move bits within this vector.
      ଶୂନ୍ୟ mov(unsigned dst, unsigned src, unsigned cnt);

	// Add that to this in the Verilog way.
      ଶୂନ୍ୟ add(ଅଚଳ vvp_vector4_t&that);

	// Subtract that from this in the Verilog way.
      ଶୂନ୍ୟ sub(ଅଚଳ vvp_vector4_t&that);

	// Multiply this by that in the Verilog way.
      ଶୂନ୍ୟ mul(ଅଚଳ vvp_vector4_t&that);

	// Test that the vectors are exactly equal
      bool eeq(ଅଚଳ vvp_vector4_t&that) ଅଚଳ;

	// Test that the vectors are equal, with xz comparing as equal.
      bool eq_xz(ଅଚଳ vvp_vector4_t&that) ଅଚଳ;

	// Return true if there is an X or Z anywhere in the vector.
      bool has_xz() ଅଚଳ;

	// Change all Z bits to X bits.
      ଶୂନ୍ୟ change_z2x();

	// Set all bits to the specified value.
      ଶୂନ୍ୟ fill_bits(vvp_bit4_t bit);

	// Change all bits to X bits.
      ଶୂନ୍ୟ set_to_x() { fill_bits(BIT4_X); }

	// Display the value into the buf as a string.
      ଅକ୍ଷର*as_string(ଅକ୍ଷର*buf, size_t buf_len) ଅଚଳ;

      ଶୂନ୍ୟ invert();
      vvp_vector4_t& operator &= (ଅଚଳ vvp_vector4_t&that);
      vvp_vector4_t& operator |= (ଅଚଳ vvp_vector4_t&that);
      vvp_vector4_t& operator ^= (ଅଚଳ vvp_vector4_t&that);
      vvp_vector4_t& operator += (int64_t);

      vvp_bit4_t reduce_or() ଅଚଳ;
      vvp_bit4_t reduce_and() ଅଚଳ;
      vvp_bit4_t reduce_xor() ଅଚଳ;

    ବ୍ୟକ୍ତିଗତ:
	// Number of vvp_bit4_t bits that can be shoved into a word.
      ଗଣନା { BITS_PER_WORD = 8*ଆକାର(unsigned long) };
	// The double value constructor requires that WORD_0_BBITS
	// and WORD_1_BBITS have the same value!
#ଯଦି SIZEOF_UNSIGNED_LONG == 8
      ଗଣନା { WORD_0_ABITS = 0x0000000000000000UL,
	     WORD_0_BBITS = 0x0000000000000000UL };
      ଗଣନା { WORD_1_ABITS = 0xFFFFFFFFFFFFFFFFUL,
	     WORD_1_BBITS = 0x0000000000000000UL };
      ଗଣନା { WORD_X_ABITS = 0xFFFFFFFFFFFFFFFFUL,
	     WORD_X_BBITS = 0xFFFFFFFFFFFFFFFFUL };
      ଗଣନା { WORD_Z_ABITS = 0x0000000000000000UL,
             WORD_Z_BBITS = 0xFFFFFFFFFFFFFFFFUL };
#ନଚେତଯଦି SIZEOF_UNSIGNED_LONG == 4
      ଗଣନା { WORD_0_ABITS = 0x00000000UL, WORD_0_BBITS = 0x00000000UL };
      ଗଣନା { WORD_1_ABITS = 0xFFFFFFFFUL, WORD_1_BBITS = 0x00000000UL };
      ଗଣନା { WORD_X_ABITS = 0xFFFFFFFFUL, WORD_X_BBITS = 0xFFFFFFFFUL };
      ଗଣନା { WORD_Z_ABITS = 0x00000000UL, WORD_Z_BBITS = 0xFFFFFFFFUL };
#ଅନ୍ୟଥା
#error "WORD_X_xBITS not defined for this architecture?"
#endif

	// Initialize and operator= use this private method to copy
	// the data from that object into this object.
      ଶୂନ୍ୟ copy_from_(ଅଚଳ vvp_vector4_t&that);
      ଶୂନ୍ୟ copy_from_big_(ଅଚଳ vvp_vector4_t&that);
      ଶୂନ୍ୟ copy_inverted_from_(ଅଚଳ vvp_vector4_t&that);

      ଶୂନ୍ୟ allocate_ଶବ୍ଦଗୁଡ଼ିକ(unsigned long inita, unsigned long initb);

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
      ସଂଘ {
	    unsigned long abits_val_;
	    unsigned long*abits_ptr_;
      };
      ସଂଘ {
	    unsigned long bbits_val_;
	    unsigned long*bbits_ptr_;
      };
};

ଧାଡ଼ିଭିତରେ vvp_vector4_t::vvp_vector4_t(ଅଚଳ vvp_vector4_t&that)
{
      copy_from_(that);
}

ଧାଡ଼ିଭିତରେ vvp_vector4_t::vvp_vector4_t(ଅଚଳ vvp_vector4_t&that, bool invert_flag)
{
      ଯଦି (invert_flag)
	    copy_inverted_from_(that);
      ଅନ୍ୟଥା
	    copy_from_(that);
}

ଧାଡ଼ିଭିତରେ vvp_vector4_t::vvp_vector4_t(unsigned size__, vvp_bit4_t val)
: size_(size__)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      ସ୍ଥିର ଅଚଳ unsigned long init_atable[4] = {
	    WORD_0_ABITS,
	    WORD_1_ABITS,
	    WORD_Z_ABITS,
	    WORD_X_ABITS };
      ସ୍ଥିର ଅଚଳ unsigned long init_btable[4] = {
	    WORD_0_BBITS,
	    WORD_1_BBITS,
	    WORD_Z_BBITS,
	    WORD_X_BBITS };

      allocate_ଶବ୍ଦଗୁଡ଼ିକ(init_atable[val], init_btable[val]);
}

ଧାଡ଼ିଭିତରେ vvp_vector4_t::~vvp_vector4_t()
{
      ଯଦି (size_ > BITS_PER_WORD) {
	    delete[] abits_ptr_;
	      // bbits_ptr_ actually points half-way into a
	      // double-length array started at abits_ptr_
      }
}

ଧାଡ଼ିଭିତରେ vvp_vector4_t& vvp_vector4_t::operator= (ଅଚଳ vvp_vector4_t&that)
{
      ଯଦି (this == &that)
	    ଫେରନ୍ତୁ *this;

      ଯଦି (size_ > BITS_PER_WORD)
	    delete[] abits_ptr_;

      copy_from_(that);

      ଫେରନ୍ତୁ *this;
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ vvp_vector4_t::copy_from_(ଅଚଳ vvp_vector4_t&that)
{
      size_ = that.size_;
      ଯଦି (size_ <= BITS_PER_WORD) {
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;
      } ଅନ୍ୟଥା {
	    copy_from_big_(that);
      }
}

ଧାଡ଼ିଭିତରେ vvp_bit4_t vvp_vector4_t::ମୂଲ୍ୟ(unsigned idx) ଅଚଳ
{
      ଯଦି (idx >= size_)
	    ଫେରନ୍ତୁ BIT4_X;

      unsigned off;

      unsigned long abits, bbits;
      ଯଦି (size_ > BITS_PER_WORD) {
	    unsigned wdx = idx / BITS_PER_WORD;
	    off = idx % BITS_PER_WORD;
	    abits = abits_ptr_[wdx];
	    bbits = bbits_ptr_[wdx];
      } ଅନ୍ୟଥା {
	    off = idx;
	    abits = abits_val_;
	    bbits = bbits_val_;
      }

      abits >>= off;
      bbits >>= off;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା tmp = ((bbits&1) << 1) + (abits&1);
	// This cast works since b==1,a==1 is X and b==1,a==0 is Z.
      ଫେରନ୍ତୁ (vvp_bit4_t)tmp;
}

ଧାଡ଼ିଭିତରେ vvp_vector4_t vvp_vector4_t::subvalue(unsigned adr, unsigned wid) ଅଚଳ
{
      ଫେରନ୍ତୁ vvp_vector4_t(*this, adr, wid);
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ vvp_vector4_t::set_bit(unsigned idx, vvp_bit4_t val)
{
      ପ୍ରତିଜ୍ଞା(idx < size_);

      unsigned long off = idx % BITS_PER_WORD;
      unsigned long mask = 1UL << off;

      ଯଦି (size_ > BITS_PER_WORD) {
	    unsigned wdx = idx / BITS_PER_WORD;
	    ବାଛ (val) {
		ସ୍ଥିତି BIT4_0:
		  abits_ptr_[wdx] &= ~mask;
		  bbits_ptr_[wdx] &= ~mask;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_1:
		  abits_ptr_[wdx] |=  mask;
		  bbits_ptr_[wdx] &= ~mask;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_X:
		  abits_ptr_[wdx] |=  mask;
		  bbits_ptr_[wdx] |=  mask;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_Z:
		  abits_ptr_[wdx] &= ~mask;
		  bbits_ptr_[wdx] |=  mask;
		  ଭାଙ୍ଗ;
	    }
      } ଅନ୍ୟଥା {
	    ବାଛ (val) {
		ସ୍ଥିତି BIT4_0:
		  abits_val_ &= ~mask;
		  bbits_val_ &= ~mask;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_1:
		  abits_val_ |=  mask;
		  bbits_val_ &= ~mask;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_X:
		  abits_val_ |=  mask;
		  bbits_val_ |=  mask;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_Z:
		  abits_val_ &= ~mask;
		  bbits_val_ |=  mask;
		  ଭାଙ୍ଗ;
	    }
      }
}

ଧାଡ଼ିଭିତରେ vvp_vector4_t operator ~ (ଅଚଳ vvp_vector4_t&that)
{
      vvp_vector4_t res (that, true);
      ଫେରନ୍ତୁ res;
}

ବାହ୍ୟ std::ostream& operator << (std::ostream&, ଅଚଳ vvp_vector4_t&);

ବାହ୍ୟ vvp_bit4_t compare_gtge(ଅଚଳ vvp_vector4_t&a,
			       ଅଚଳ vvp_vector4_t&b,
			       vvp_bit4_t val_if_equal);
ବାହ୍ୟ vvp_bit4_t compare_gtge_signed(ଅଚଳ vvp_vector4_t&a,
				      ଅଚଳ vvp_vector4_t&b,
				      vvp_bit4_t val_if_equal);
template <class T> ବାହ୍ୟ T coerce_to_width(ଅଚଳ T&that, unsigned width);

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
template <class T> ବାହ୍ୟ bool vector4_to_ମୂଲ୍ୟ(ଅଚଳ vvp_vector4_t&a, T&val,
						bool is_signed,
						bool is_arithmetic =true);

template <class T> ବାହ୍ୟ bool vector4_to_ମୂଲ୍ୟ(ଅଚଳ vvp_vector4_t&a,
                                                bool&overflow_flag, T&val);

template <class T> ଧାଡ଼ିଭିତରେ bool vector4_to_ମୂଲ୍ୟ(ଅଚଳ vvp_vector4_t&a, T&val)
{
      bool overflow_flag;
      ଫେରନ୍ତୁ vector4_to_ମୂଲ୍ୟ(a, overflow_flag, val);
}

ବାହ୍ୟ bool vector4_to_ମୂଲ୍ୟ(ଅଚଳ vvp_vector4_t&a, ଦ୍ୱିଗୁଣ&val, bool is_signed);

ବାହ୍ୟ bool vector2_to_ମୂଲ୍ୟ(ଅଚଳ vvp_vector2_t&a, int32_t&val, bool is_signed);

ବାହ୍ୟ vvp_vector4_t vector4_from_text(ଅଚଳ ଅକ୍ଷର*bits, unsigned wid);

ଧାଡ଼ିଭିତରେ vvp_vector4_t double_to_vector4_LSB(ଦ୍ୱିଗୁଣ real)
{
      // Convert the double to a bit vector and then use the LSB.
      // You can use very large real values and break the 64-bit
      // size, but that's really horrible code so don't do that!
      vvp_vector4_t bit = vvp_vector4_t(64, real);
      bit.resize(1);
      ଫେରନ୍ତୁ bit;
}

/*
 * vvp_vector4array_t
 */
class vvp_vector4array_t {

    ସର୍ବସାଧାରଣ:
      vvp_vector4array_t(unsigned width, unsigned ଶବ୍ଦଗୁଡ଼ିକ);
      ଆଭାସୀ ~vvp_vector4array_t();

      unsigned width() ଅଚଳ { ଫେରନ୍ତୁ width_; }
      unsigned ଶବ୍ଦଗୁଡ଼ିକ() ଅଚଳ { ଫେରନ୍ତୁ ଶବ୍ଦଗୁଡ଼ିକ_3; }

      ଆଭାସୀ vvp_vector4_t get_word(unsigned idx) ଅଚଳ = 0;
      ଆଭାସୀ ଶୂନ୍ୟ set_word(unsigned idx, ଅଚଳ vvp_vector4_t&that) = 0;

    ସୁରକ୍ଷିତ:
      ସଂରଚନା v4cell {
	    ସଂଘ {
		  unsigned long abits_val_;
		  unsigned long*abits_ptr_;
	    };
	    ସଂଘ {
		  unsigned long bbits_val_;
		  unsigned long*bbits_ptr_;
	    };
      };

      vvp_vector4_t get_word_(v4cell*cell) ଅଚଳ;
      ଶୂନ୍ୟ set_word_(v4cell*cell, ଅଚଳ vvp_vector4_t&that);

      unsigned width_;
      unsigned ଶବ୍ଦଗୁଡ଼ିକ_3;

    ବ୍ୟକ୍ତିଗତ: // Not implemented
      vvp_vector4array_t(ଅଚଳ vvp_vector4array_t&);
      vvp_vector4array_t& operator = (ଅଚଳ vvp_vector4array_t&);
};

/*
 * Statically allocated vvp_vector4array_t
 */
class vvp_vector4array_sa : ସର୍ବସାଧାରଣ vvp_vector4array_t {

    ସର୍ବସାଧାରଣ:
      vvp_vector4array_sa(unsigned width, unsigned ଶବ୍ଦଗୁଡ଼ିକ);
      ~vvp_vector4array_sa() override;

      vvp_vector4_t get_word(unsigned idx) ଅଚଳ override;
      ଶୂନ୍ୟ set_word(unsigned idx, ଅଚଳ vvp_vector4_t&that) override;

    ବ୍ୟକ୍ତିଗତ:
      v4cell* array_;
};

/*
 * Automatically allocated vvp_vector4array_t
 */
class vvp_vector4array_aa : ସର୍ବସାଧାରଣ vvp_vector4array_t, ସର୍ବସାଧାରଣ automatic_hooks_s {

    ସର୍ବସାଧାରଣ:
      vvp_vector4array_aa(unsigned width, unsigned ଶବ୍ଦଗୁଡ଼ିକ);
      ~vvp_vector4array_aa() override;

      ଶୂନ୍ୟ alloc_instance(vvp_context_t context) override;
      ଶୂନ୍ୟ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ଶୂନ୍ୟ free_instance(vvp_context_t context) override;
#endif

      vvp_vector4_t get_word(unsigned idx) ଅଚଳ override;
      ଶୂନ୍ୟ set_word(unsigned idx, ଅଚଳ vvp_vector4_t&that) override;

    ବ୍ୟକ୍ତିଗତ:
      unsigned context_idx_;
};

/* vvp_vector2_t
 */
class vvp_vector2_t {

      friend vvp_vector2_t operator - (ଅଚଳ vvp_vector2_t&);
      friend vvp_vector2_t operator + (ଅଚଳ vvp_vector2_t&,
				       ଅଚଳ vvp_vector2_t&);
      friend vvp_vector2_t operator * (ଅଚଳ vvp_vector2_t&,
				       ଅଚଳ vvp_vector2_t&);
      friend bool operator >  (ଅଚଳ vvp_vector2_t&, ଅଚଳ vvp_vector2_t&);
      friend bool operator >= (ଅଚଳ vvp_vector2_t&, ଅଚଳ vvp_vector2_t&);
      friend bool operator <  (ଅଚଳ vvp_vector2_t&, ଅଚଳ vvp_vector2_t&);
      friend bool operator <= (ଅଚଳ vvp_vector2_t&, ଅଚଳ vvp_vector2_t&);
      friend bool operator == (ଅଚଳ vvp_vector2_t&, ଅଚଳ vvp_vector2_t&);

    ସର୍ବସାଧାରଣ:
      vvp_vector2_t();
      vvp_vector2_t(ଅଚଳ vvp_vector2_t&);
      vvp_vector2_t(ଅଚଳ vvp_vector2_t&, unsigned newsize);
      vvp_vector2_t(ଅଚଳ vvp_vector2_t&, unsigned base, unsigned wid);
	// Make a vvp_vector2_t from a vvp_vector4_t. If enable_NaN
	// is true and there are X or Z bits, then the result becomes
	// a NaN value. If enable_NaN is false then X or Z bits are
	// converted to 0 as per the standard Verilog rules.
      explicit vvp_vector2_t(ଅଚଳ vvp_vector4_t&that, bool allow_NaN = false);
	// Make from a 64-bit value and a specified width.
      vvp_vector2_t(uint64_t val, unsigned wid);
	// Make with the width, and filled with 1 or 0 bits.
      ଗଣନା fill_t {FILL0, FILL1};
      vvp_vector2_t(fill_t fill, unsigned wid);
      ~vvp_vector2_t();

      vvp_vector2_t&operator += (ଅଚଳ vvp_vector2_t&that);
      vvp_vector2_t&operator -= (ଅଚଳ vvp_vector2_t&that);
      vvp_vector2_t&operator <<= (unsigned shift);
      vvp_vector2_t&operator >>= (unsigned shift);
      vvp_vector2_t&operator = (ଅଚଳ vvp_vector2_t&);

	// Assign a vec4 to a vec2, using Verilog rules for converting
	// XZ values to 0.
      vvp_vector2_t&operator = (ଅଚଳ vvp_vector4_t&);

      bool is_NaN() ଅଚଳ;
      bool is_zero() ଅଚଳ;
      unsigned size() ଅଚଳ;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ(unsigned idx) ଅଚଳ;
      vvp_bit4_t value4(unsigned idx) ଅଚଳ;
	// Get the vector2 subvector starting at the address
      vvp_vector2_t subvalue(unsigned idx, unsigned size) ଅଚଳ;
      ଶୂନ୍ୟ set_bit(unsigned idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bit);
      ଶୂନ୍ୟ set_vec(unsigned idx, ଅଚଳ vvp_vector2_t&that);
	// Make the size just big enough to hold the first 1 bit.
      ଶୂନ୍ୟ trim();
	// Trim off extra 1 bit since this is representing a negative value.
	// Always keep at least 32 bits.
      ଶୂନ୍ୟ trim_neg();

    ବ୍ୟକ୍ତିଗତ:
      ଗଣନା { BITS_PER_WORD = 8 * ଆକାର(unsigned long) };
      unsigned long*vec_;
      unsigned wid_;

    ବ୍ୟକ୍ତିଗତ:
      ଶୂନ୍ୟ copy_from_that_(ଅଚଳ vvp_vector2_t&that);
      ଶୂନ୍ୟ copy_from_that_(ଅଚଳ vvp_vector4_t&that);
};

ବାହ୍ୟ bool operator >  (ଅଚଳ vvp_vector2_t&, ଅଚଳ vvp_vector2_t&);
ବାହ୍ୟ bool operator >= (ଅଚଳ vvp_vector2_t&, ଅଚଳ vvp_vector2_t&);
ବାହ୍ୟ bool operator <  (ଅଚଳ vvp_vector2_t&, ଅଚଳ vvp_vector2_t&);
ବାହ୍ୟ bool operator <= (ଅଚଳ vvp_vector2_t&, ଅଚଳ vvp_vector2_t&);
ବାହ୍ୟ bool operator == (ଅଚଳ vvp_vector2_t&, ଅଚଳ vvp_vector2_t&);
ବାହ୍ୟ vvp_vector2_t operator - (ଅଚଳ vvp_vector2_t&);
ବାହ୍ୟ vvp_vector2_t operator + (ଅଚଳ vvp_vector2_t&, ଅଚଳ vvp_vector2_t&);
ବାହ୍ୟ vvp_vector2_t operator * (ଅଚଳ vvp_vector2_t&, ଅଚଳ vvp_vector2_t&);
ବାହ୍ୟ vvp_vector2_t operator / (ଅଚଳ vvp_vector2_t&, ଅଚଳ vvp_vector2_t&);
ବାହ୍ୟ vvp_vector2_t operator % (ଅଚଳ vvp_vector2_t&, ଅଚଳ vvp_vector2_t&);

vvp_vector2_t pow(ଅଚଳ vvp_vector2_t&, vvp_vector2_t&);
ବାହ୍ୟ vvp_vector4_t vector2_to_vector4(ଅଚଳ vvp_vector2_t&, unsigned wid);

/* A c4string is of the form C4<...> where ... are bits. */
ବାହ୍ୟ bool c4string_test(ଅଚଳ ଅକ୍ଷର*str);
ବାହ୍ୟ vvp_vector4_t c4string_to_vector4(ଅଚଳ ଅକ୍ଷର*str);

/* A crstring is of the form Cr<...> where ... defines are real. */
ବାହ୍ୟ bool crstring_test(ଅଚଳ ଅକ୍ଷର*str);
ବାହ୍ୟ ଦ୍ୱିଗୁଣ crstring_to_double(ଅଚଳ ଅକ୍ଷର*str);

ବାହ୍ୟ std::ostream& operator<< (std::ostream&, ଅଚଳ vvp_vector2_t&);

ଧାଡ଼ିଭିତରେ vvp_vector2_t::vvp_vector2_t(ଅଚଳ vvp_vector2_t&that)
{
      copy_from_that_(that);
}

ଧାଡ଼ିଭିତରେ vvp_vector2_t::vvp_vector2_t(ଅଚଳ vvp_vector4_t&that, bool enable_NaN)
{
      ଯଦି (enable_NaN && that.has_xz()) {
	    vec_ = 0;
	    wid_ = 0;
	    ଫେରନ୍ତୁ;
      }
      copy_from_that_(that);
}

ଧାଡ଼ିଭିତରେ vvp_vector2_t::~vvp_vector2_t()
{
      delete[] vec_;
}

/* Inline some of the vector2_t methods. */
ଧାଡ଼ିଭିତରେ unsigned vvp_vector2_t::size() ଅଚଳ
{
      ଫେରନ୍ତୁ wid_;
}

ଧାଡ଼ିଭିତରେ vvp_bit4_t vvp_vector2_t::value4(unsigned idx) ଅଚଳ
{
      ଯଦି (ମୂଲ୍ୟ(idx))
	    ଫେରନ୍ତୁ BIT4_1;
      ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ BIT4_0;
}

ଧାଡ଼ିଭିତରେ vvp_vector2_t vvp_vector2_t::subvalue(unsigned base, unsigned wid) ଅଚଳ
{
      ଫେରନ୍ତୁ vvp_vector2_t(*this, base, wid);
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

    ସର୍ବସାଧାରଣ:
	// Make a HiZ value.
      explicit vvp_scalar_t();

	// Make an unambiguous value.
      explicit vvp_scalar_t(vvp_bit4_t val, unsigned str0, unsigned str1);

	// Get the vvp_bit4_t version of the value
      vvp_bit4_t ମୂଲ୍ୟ() ଅଚଳ;
      unsigned strength0() ଅଚଳ;
      unsigned strength1() ଅଚଳ;

      bool eeq(vvp_scalar_t that) ଅଚଳ { ଫେରନ୍ତୁ ମୂଲ୍ୟ_2 == that.ମୂଲ୍ୟ_2; }
      bool is_hiz() ଅଚଳ { ଫେରନ୍ତୁ (ମୂଲ୍ୟ_2 & 0x77) == 0; }

    ବ୍ୟକ୍ତିଗତ:
	// This class and the vvp_vector8_t class are closely related,
	// so allow vvp_vector8_t access to the raw encoding so that
	// it can do compact vectoring of vvp_scalar_t objects.
      friend class vvp_vector8_t;
      explicit vvp_scalar_t(unsigned ଅକ୍ଷର val) : ମୂଲ୍ୟ_2(val) { }
      unsigned ଅକ୍ଷର raw() ଅଚଳ { ଫେରନ୍ତୁ ମୂଲ୍ୟ_2; }

    ବ୍ୟକ୍ତିଗତ:
      unsigned ଅକ୍ଷର ମୂଲ୍ୟ_2;
};

ଧାଡ଼ିଭିତରେ vvp_scalar_t::vvp_scalar_t()
{
      ମୂଲ୍ୟ_2 = 0;
}

ଧାଡ଼ିଭିତରେ vvp_scalar_t::vvp_scalar_t(vvp_bit4_t val, unsigned str0, unsigned str1)
{
      ପ୍ରତିଜ୍ଞା(str0 <= 7);
      ପ୍ରତିଜ୍ଞା(str1 <= 7);

      ଯଦି (str0 == 0 && str1 == 0) {
	    ମୂଲ୍ୟ_2 = 0x00;
      } ଅନ୍ୟଥା ବାଛ (val) {
	  ସ୍ଥିତି BIT4_0:
	    ମୂଲ୍ୟ_2 = str0 | (str0<<4);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_1:
	    ମୂଲ୍ୟ_2 = str1 | (str1<<4) | 0x88;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_X:
	    ମୂଲ୍ୟ_2 = str0 | (str1<<4) | 0x80;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_Z:
	    ମୂଲ୍ୟ_2 = 0x00;
	    ଭାଙ୍ଗ;
      }
}

ଧାଡ଼ିଭିତରେ vvp_bit4_t vvp_scalar_t::ମୂଲ୍ୟ() ଅଚଳ
{
      ଯଦି ((ମୂଲ୍ୟ_2 & 0x77) == 0) {
	    ଫେରନ୍ତୁ BIT4_Z;
      } ଅନ୍ୟଥା ବାଛ (ମୂଲ୍ୟ_2 & 0x88) {
	  ସ୍ଥିତି 0x00:
	    ଫେରନ୍ତୁ BIT4_0;
	  ସ୍ଥିତି 0x88:
	    ଫେରନ୍ତୁ BIT4_1;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ BIT4_X;
      }
}


ଧାଡ଼ିଭିତରେ vvp_scalar_t resolve(vvp_scalar_t a, vvp_scalar_t b)
{
      ବାହ୍ୟ vvp_scalar_t fully_featured_resolv_(vvp_scalar_t, vvp_scalar_t);

	// If the value is HiZ, resolution is simply a matter of
	// returning the *other* value.
      ଯଦି (a.is_hiz())
	    ଫେରନ୍ତୁ b;
      ଯଦି (b.is_hiz())
	    ଫେରନ୍ତୁ a;
	// If the values are the identical, then resolution is simply
	// returning *either* value.
      ଯଦି (a .eeq( b ))
	    ଫେରନ୍ତୁ a;

      ଫେରନ୍ତୁ fully_featured_resolv_(a,b);
}

ବାହ୍ୟ std::ostream& operator<< (std::ostream&, vvp_scalar_t);

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

      friend vvp_vector8_t part_expand(ଅଚଳ vvp_vector8_t&, unsigned, unsigned);

    ସର୍ବସାଧାରଣ:
      explicit vvp_vector8_t(unsigned size =0);
	// Make a vvp_vector8_t from a vector4 and a specified
	// strength.
      explicit vvp_vector8_t(ଅଚଳ vvp_vector4_t&that,
			     unsigned str0,
			     unsigned str1);
      explicit vvp_vector8_t(ଅଚଳ vvp_vector2_t&that,
			     unsigned str0,
			     unsigned str1);

      ~vvp_vector8_t();

      ସ୍ଥିର ଅଚଳ vvp_vector8_t nil;

    ସର୍ବସାଧାରଣ:

      unsigned size() ଅଚଳ { ଫେରନ୍ତୁ size_; }
      vvp_scalar_t ମୂଲ୍ୟ(unsigned idx) ଅଚଳ;
      vvp_vector8_t subvalue(unsigned adr, unsigned width) ଅଚଳ;
      ଶୂନ୍ୟ set_bit(unsigned idx, vvp_scalar_t val);
      ଶୂନ୍ୟ set_vec(unsigned idx, ଅଚଳ vvp_vector8_t&that);

	// Test that the vectors are exactly equal
      bool eeq(ଅଚଳ vvp_vector8_t&that) ଅଚଳ;

      vvp_vector8_t(ଅଚଳ vvp_vector8_t&that);
      vvp_vector8_t& operator= (ଅଚଳ vvp_vector8_t&that);

    ବ୍ୟକ୍ତିଗତ:
      unsigned size_;
      ସଂଘ {
	    unsigned ଅକ୍ଷର*ptr_;
	    unsigned ଅକ୍ଷର val_[ଆକାର(ଶୂନ୍ୟ*)];
      };
};

  /* Resolve uses the default Verilog resolver algorithm to resolve
     two drive vectors to a single output. */
ଧାଡ଼ିଭିତରେ vvp_vector8_t resolve(ଅଚଳ vvp_vector8_t&a, ଅଚଳ vvp_vector8_t&b)
{
      ପ୍ରତିଜ୍ଞା(a.size() == b.size());
      vvp_vector8_t out (a.size());

      ପାଇଁ (unsigned idx = 0 ;  idx < out.size() ;  idx += 1) {
	    out.set_bit(idx, resolve(a.ମୂଲ୍ୟ(idx), b.ମୂଲ୍ୟ(idx)));
      }

      ଫେରନ୍ତୁ out;
}

  /* This lookup tabke implements the strength reduction implied by
     Verilog standard switch devices. The major dimension selects
     between non-resistive and resistive devices. */
ବାହ୍ୟ unsigned vvp_switch_strength_map[2][8];

  /* The reduce4 function converts a vector8 to a vector4, losing
     strength information in the process. */
ବାହ୍ୟ vvp_vector4_t reduce4(ଅଚଳ vvp_vector8_t&that);
ବାହ୍ୟ vvp_vector8_t part_expand(ଅଚଳ vvp_vector8_t&a, unsigned wid, unsigned off);

  /* A c8string is of the form C8<...> where ... are bits. */
ବାହ୍ୟ bool c8string_test(ଅଚଳ ଅକ୍ଷର*str);
ବାହ୍ୟ vvp_vector8_t c8string_to_vector8(ଅଚଳ ଅକ୍ଷର*str);

  /* Print a vector8 value to a stream. */
ବାହ୍ୟ std::ostream& operator<< (std::ostream&, ଅଚଳ vvp_vector8_t&);

ଧାଡ଼ିଭିତରେ vvp_vector8_t::vvp_vector8_t(unsigned size__)
: size_(size__)
{
      ଯଦି (size_ <= ଆକାର(val_)) {
	    memset(val_, 0, ଆକାର(val_));
      } ଅନ୍ୟଥା {
	    ptr_ = new unsigned ଅକ୍ଷର[size_];
	    memset(ptr_, 0, size_);
      }
}

ଧାଡ଼ିଭିତରେ vvp_vector8_t::~vvp_vector8_t()
{
      ଯଦି (size_ > ଆକାର(val_))
	    delete[]ptr_;
}

ଧାଡ଼ିଭିତରେ vvp_scalar_t vvp_vector8_t::ମୂଲ୍ୟ(unsigned idx) ଅଚଳ
{
      ପ୍ରତିଜ୍ଞା(idx < size_);
      ଯଦି (size_ <= ଆକାର(val_))
	    ଫେରନ୍ତୁ vvp_scalar_t(val_[idx]);
      ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ vvp_scalar_t(ptr_[idx]);
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ vvp_vector8_t::set_bit(unsigned idx, vvp_scalar_t val)
{
      ପ୍ରତିଜ୍ଞା(idx < size_);
      ଯଦି (size_ <= ଆକାର(val_))
	    val_[idx] = val.raw();
      ଅନ୍ୟଥା
	    ptr_[idx] = val.raw();
}

  // Exactly-equal for vvp_vector8_t is common and should be as tight
  // as possible.
ଧାଡ଼ିଭିତରେ bool vvp_vector8_t::eeq(ଅଚଳ vvp_vector8_t&that) ଅଚଳ
{
      ଯଦି (size_ != that.size_)
	    ଫେରନ୍ତୁ false;
      ଯଦି (size_ == 0)
	    ଫେରନ୍ତୁ true;

      ଯଦି (size_ <= ଆକାର(val_))
	      // This is equivalent to memcmp(val_, that.val_, sizeof val_)==0
	    ଫେରନ୍ତୁ ptr_ == that.ptr_;
      ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ memcmp(ptr_, that.ptr_, size_) == 0;
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

    ସର୍ବସାଧାରଣ:
      vvp_sub_pointer_t() : bits_(0) { }

      vvp_sub_pointer_t(T*ptr__, unsigned port__)
      {
	    bits_ = reinterpret_cast<uintptr_t> (ptr__);
	    ପ୍ରତିଜ୍ଞା( (bits_  &  UINTPTR_C(3)) == 0 );
	    ପ୍ରତିଜ୍ଞା( (port__ & ~UINTPTR_C(3)) == 0 );
	    bits_ |= port__;
      }

      ~vvp_sub_pointer_t() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

      T* ptr()
      { ଫେରନ୍ତୁ reinterpret_cast<T*> (bits_ & ~UINTPTR_C(3)); }

      ଅଚଳ T* ptr() ଅଚଳ
      { ଫେରନ୍ତୁ reinterpret_cast<ଅଚଳ T*> (bits_ & ~UINTPTR_C(3)); }

      unsigned  port() ଅଚଳ { ଫେରନ୍ତୁ bits_ & UINTPTR_C(3); }

      bool nil() ଅଚଳ { ଫେରନ୍ତୁ bits_ == 0; }

      bool operator == (vvp_sub_pointer_t that) ଅଚଳ { ଫେରନ୍ତୁ bits_ == that.bits_; }
      bool operator != (vvp_sub_pointer_t that) ଅଚଳ { ଫେରନ୍ତୁ bits_ != that.bits_; }

    ବ୍ୟକ୍ତିଗତ:
      uintptr_t bits_;
};

ପ୍ରକାରବ୍ୟାଖ୍ୟା vvp_sub_pointer_t<vvp_net_t> vvp_net_ptr_t;
template <class T> std::ostream& operator << (std::ostream&out, vvp_sub_pointer_t<T> val)
{ out << val.ptr() << "[" << val.port() << "]"; ଫେରନ୍ତୁ out; }

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
    ସର୍ବସାଧାରଣ:
      vvp_net_t();

#ifdef CHECK_WITH_VALGRIND
      vvp_net_t *pool;
#endif
      vvp_net_ptr_t port[4];
      vvp_net_fun_t*fun;
      vvp_net_fil_t*fil;

    ସର୍ବସାଧାରଣ:
	// Connect the port to the output from this net.
      ଶୂନ୍ୟ link(vvp_net_ptr_t port);
	// Disconnect the port from the output of this net.
      ଶୂନ୍ୟ unlink(vvp_net_ptr_t port);

    ସର୍ବସାଧାରଣ: // Methods to propagate output from this node.
      ଶୂନ୍ୟ send_vec4(ଅଚଳ vvp_vector4_t&val, vvp_context_t context);
      ଶୂନ୍ୟ send_vec8(ଅଚଳ vvp_vector8_t&val);
      ଶୂନ୍ୟ send_real(ଦ୍ୱିଗୁଣ val, vvp_context_t context);
      ଶୂନ୍ୟ send_string(ଅଚଳ std::string&val, vvp_context_t context);
      ଶୂନ୍ୟ send_object(vvp_object_t val, vvp_context_t context);

      ଶୂନ୍ୟ send_vec4_pv(ଅଚଳ vvp_vector4_t&val,
			unsigned base, unsigned vwid, vvp_context_t context);
      ଶୂନ୍ୟ send_vec8_pv(ଅଚଳ vvp_vector8_t&val,
			unsigned base, unsigned vwid);


    ସର୍ବସାଧାରଣ: // Methods to arrange for the output of this net to be forced.

	// The intent is that all efforts at force are directed to
	// operate only on the vvp_net_t whose output is to be
	// forced. These methods then communicate the force to the
	// attached filter to set up the actual force.
      ଶୂନ୍ୟ force_vec4(ଅଚଳ vvp_vector4_t&val, ଅଚଳ vvp_vector2_t&mask);
      ଶୂନ୍ୟ force_vec8(ଅଚଳ vvp_vector8_t&val, ଅଚଳ vvp_vector2_t&mask);
      ଶୂନ୍ୟ force_real(ଦ୍ୱିଗୁଣ val, ଅଚଳ vvp_vector2_t&mask);

    ସର୍ବସାଧାରଣ: // Method to support $countdrivers
      ଶୂନ୍ୟ ଗଣନା_drivers(unsigned idx, unsigned counts[4]);

    // This needs to be public so that SDF interconnects can be inserted
    ସର୍ବସାଧାରଣ:
      vvp_net_ptr_t out_;

    ସର୍ବସାଧାରଣ: // Need a better new for these objects.
      ସ୍ଥିର ଶୂନ୍ୟ* operator new(std::size_t size);
      ସ୍ଥିର ଶୂନ୍ୟ operator delete(ଶୂନ୍ୟ*); // not implemented
    ବ୍ୟକ୍ତିଗତ: // not implemented
      ସ୍ଥିର ଶୂନ୍ୟ* operator new[](std::size_t size);
#ଯଦି defined(__GNUC__)
      ସ୍ଥିର ଶୂନ୍ୟ operator delete[](ଶୂନ୍ୟ*);
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

    ସର୍ବସାଧାରଣ:
      vvp_net_fun_t();
      ଆଭାସୀ ~vvp_net_fun_t();

      ଆଭାସୀ ଶୂନ୍ୟ recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                             vvp_context_t context);
      ଆଭାସୀ ଶୂନ୍ୟ recv_vec8(vvp_net_ptr_t port, ଅଚଳ vvp_vector8_t&bit);
      ଆଭାସୀ ଶୂନ୍ୟ recv_real(vvp_net_ptr_t port, ଦ୍ୱିଗୁଣ bit,
                             vvp_context_t context);
      ଆଭାସୀ ଶୂନ୍ୟ recv_string(vvp_net_ptr_t port, ଅଚଳ std::string&bit,
			       vvp_context_t context);
      ଆଭାସୀ ଶୂନ୍ୟ recv_object(vvp_net_ptr_t port, vvp_object_t bit,
			       vvp_context_t context);

	// Part select variants of above
      ଆଭାସୀ ଶୂନ୍ୟ recv_vec4_pv(vvp_net_ptr_t p, ଅଚଳ vvp_vector4_t&bit,
				unsigned base, unsigned vwid, vvp_context_t context);
      ଆଭାସୀ ଶୂନ୍ୟ recv_vec8_pv(vvp_net_ptr_t p, ଅଚଳ vvp_vector8_t&bit,
				unsigned base, unsigned vwid);

	// This method is called when the net is forced or
	// released. This is very rarely needed; island ports use it
	// to know that the net is being forced and that it needs to
	// do something about it.
      ଆଭାସୀ ଶୂନ୍ୟ force_flag(bool run_now);

   ସୁରକ୍ଷିତ:
      ଶୂନ୍ୟ recv_vec4_pv_(vvp_net_ptr_t p, ଅଚଳ vvp_vector4_t&bit,
			 unsigned base, unsigned vwid, vvp_context_t context);
      ଶୂନ୍ୟ recv_vec8_pv_(vvp_net_ptr_t p, ଅଚଳ vvp_vector8_t&bit,
			 unsigned base, unsigned vwid);

    ସର୍ବସାଧାରଣ: // These objects are only permallocated.
      ସ୍ଥିର ଶୂନ୍ୟ* operator new(std::size_t size) { ଫେରନ୍ତୁ heap_.alloc(size); }
      ସ୍ଥିର ଶୂନ୍ୟ operator delete(ଶୂନ୍ୟ*); // not implemented

      ସ୍ଥିର std::size_t heap_total() { ଫେରନ୍ତୁ heap_.heap_total(); }

    ସୁରକ୍ଷିତ:
      ସ୍ଥିର permaheap heap_;

    ବ୍ୟକ୍ତିଗତ: // not implemented
      vvp_net_fun_t(ଅଚଳ vvp_net_fun_t&);
      vvp_net_fun_t& operator= (ଅଚଳ vvp_net_fun_t&);
      ସ୍ଥିର ଶୂନ୍ୟ* operator new[](std::size_t size);
      ସ୍ଥିର ଶୂନ୍ୟ operator delete[](ଶୂନ୍ୟ*);
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
class vvp_net_fil_t  : ସର୍ବସାଧାରଣ vvp_vpi_callback {

    ସର୍ବସାଧାରଣ:
      vvp_net_fil_t();
      ଆଭାସୀ ~vvp_net_fil_t() override;

    ସର୍ବସାଧାରଣ:
      ଗଣନା prop_t { STOP=0, PROP, REPL };


	// These filter methods are used by the vvp_net_t::send_*()
	// methods to test the output (from the functor) bit value
	// against any force filters. If none of the bits are forced,
	// then the method returns PROP and the caller propagates the
	// bit value. If bits were changed by the force mask, then the
	// method returns REPL and the caller should propagate the rep
	// value instead. If the function returns STOP, then all the
	// output bits are filtered by the force mask and there is
	// nothing to propagate.
      ଆଭାସୀ prop_t filter_vec4(ଅଚଳ vvp_vector4_t&bit, vvp_vector4_t&rep,
				 unsigned base, unsigned vwid);
      ଆଭାସୀ prop_t filter_vec8(ଅଚଳ vvp_vector8_t&val, vvp_vector8_t&rep,
				 unsigned base, unsigned vwid);
      ଆଭାସୀ prop_t filter_real(ଦ୍ୱିଗୁଣ&val);
      ଆଭାସୀ prop_t filter_object(vvp_object_t&val);
      ଆଭାସୀ prop_t filter_string(ଅଚଳ std::string&val);

      ଆଭାସୀ ଶୂନ୍ୟ release(vvp_net_ptr_t ptr, bool net_flag) =0;
      ଆଭାସୀ ଶୂନ୍ୟ release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) =0;

	// The %force/link instruction needs a place to write the
	// source node of the force, so that subsequent %force and
	// %release instructions can undo the link as needed. */
      ଶୂନ୍ୟ force_link(vvp_net_t*dst, vvp_net_t*src);
      ଶୂନ୍ୟ force_unlink(ଶୂନ୍ୟ);

      ଆଭାସୀ unsigned filter_size() ଅଚଳ =0;

    ସର୍ବସାଧାରଣ:
	// Support for force methods. These are called by the
	// vvp_net_t::force_* methods to set the force value and mask
	// for the filter.
      ଆଭାସୀ ଶୂନ୍ୟ force_fil_vec4(ଅଚଳ vvp_vector4_t&val, ଅଚଳ vvp_vector2_t&mask) =0;
      ଆଭାସୀ ଶୂନ୍ୟ force_fil_vec8(ଅଚଳ vvp_vector8_t&val, ଅଚଳ vvp_vector2_t&mask) =0;
      ଆଭାସୀ ଶୂନ୍ୟ force_fil_real(ଦ୍ୱିଗୁଣ val, ଅଚଳ vvp_vector2_t&mask) =0;

    ସର୍ବସାଧାରଣ: // These objects are only permallocated.
      ସ୍ଥିର ଶୂନ୍ୟ* operator new(std::size_t size) { ଫେରନ୍ତୁ heap_.alloc(size); }
      ସ୍ଥିର ଶୂନ୍ୟ operator delete(ଶୂନ୍ୟ*); // not implemented

      ସ୍ଥିର size_t heap_total() { ଫେରନ୍ତୁ heap_.heap_total(); }

    ବ୍ୟକ୍ତିଗତ:
      ସ୍ଥିର permaheap heap_;

    ବ୍ୟକ୍ତିଗତ: // not implemented
      vvp_net_fil_t(ଅଚଳ vvp_net_fil_t&);
      vvp_net_fil_t& operator= (ଅଚଳ vvp_net_fil_t&);
      ସ୍ଥିର ଶୂନ୍ୟ* operator new[](std::size_t size);
      ସ୍ଥିର ଶୂନ୍ୟ operator delete[](ଶୂନ୍ୟ*);

    ସୁରକ୍ଷିତ:
	// Set bits of the filter force mask
      ଶୂନ୍ୟ force_mask(ଅଚଳ vvp_vector2_t&mask);
	// Release the force on the bits set in the mask.
      ଶୂନ୍ୟ release_mask(ଅଚଳ vvp_vector2_t&mask);
	// Test bits of the filter force mask;
      bool test_force_mask(unsigned bit) ଅଚଳ;
      bool test_force_mask_is_zero() ଅଚଳ;

	// This template method is used by derived classes to process
	// the val through the force mask. The force value is the
	// currently forced value, and the buf is a value that this
	// method will use to hold a filtered value, if needed. This
	// method returns the replacement value into the "rep"
	// argument and returns a code indicating whether any changes
	// were made.
      template <class T> prop_t filter_mask_(ଅଚଳ T&val, ଅଚଳ T&force, T&rep, unsigned addr);
	// This template method is similar to the above, but works for
	// native types that are not so expensive to edit in place.
      template <class T> prop_t filter_mask_(T&val, T force);

	// These templates are similar to filter_mask_, but are
	// idempotent. Then do not trigger callbacks or otherwise
	// cause any local changes. These methods are used to test
	// arbitrary values against the force mask.
      template <class T> prop_t filter_ନିବେଶ_mask(ଅଚଳ T&val, ଅଚଳ T&force, T&rep) ଅଚଳ;

    ବ୍ୟକ୍ତିଗତ:
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
class vvp_fun_force : ସର୍ବସାଧାରଣ vvp_net_fun_t {

    ସର୍ବସାଧାରଣ:
      vvp_fun_force();
      ~vvp_fun_force() override;

      ଶୂନ୍ୟ recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
		     vvp_context_t context) override;
      ଶୂନ୍ୟ recv_real(vvp_net_ptr_t port, ଦ୍ୱିଗୁଣ bit, vvp_context_t) override;
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
class vvp_fun_drive  : ସର୍ବସାଧାରଣ vvp_net_fun_t {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_drive(unsigned str0 =6, unsigned str1 =6);
      ~vvp_fun_drive() override;

      ଶୂନ୍ୟ recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                     vvp_context_t context) override;

      ଶୂନ୍ୟ recv_vec4_pv(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;
    ବ୍ୟକ୍ତିଗତ:
      unsigned ଅକ୍ଷର drive0_;
      unsigned ଅକ୍ଷର drive1_;
};

/*
 * EXTEND functors expand an input vector to the desired output
 * width. The extend_signed functor sign extends the input. If the
 * input is already wider than the desired output, then it is passed
 * unmodified.
 */
class vvp_fun_extend_signed  : ସର୍ବସାଧାରଣ vvp_net_fun_t {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_extend_signed(unsigned wid);
      ~vvp_fun_extend_signed() override;

      ଶୂନ୍ୟ recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                     vvp_context_t context) override;

      ଶୂନ୍ୟ recv_vec4_pv(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;
    ବ୍ୟକ୍ତିଗତ:
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

class vvp_wide_fun_core : ସର୍ବସାଧାରଣ vvp_net_fun_t {

    ସର୍ବସାଧାରଣ:
      vvp_wide_fun_core(vvp_net_t*net, unsigned nports);
      ଆଭାସୀ ~vvp_wide_fun_core() override;
	// These objects are not perm allocated.
      ସ୍ଥିର ଶୂନ୍ୟ* operator new(std::size_t size) { ଫେରନ୍ତୁ ::new ଅକ୍ଷର[size]; }
      ସ୍ଥିର ଶୂନ୍ୟ operator delete(ଶୂନ୍ୟ* ptr) { ::delete[](static_cast<ଅକ୍ଷର*>(ptr)); }

    ସୁରକ୍ଷିତ:
      ଶୂନ୍ୟ propagate_vec4(ଅଚଳ vvp_vector4_t&bit, vvp_time64_t delay =0);
      ଶୂନ୍ୟ propagate_real(ଦ୍ୱିଗୁଣ bit, vvp_time64_t delay =0);
      unsigned port_ଗଣନା() ଅଚଳ;

      vvp_vector4_t& ମୂଲ୍ୟ(unsigned);
      ଦ୍ୱିଗୁଣ ମୂଲ୍ୟ_r(unsigned);

    ବ୍ୟକ୍ତିଗତ:
	// the derived class implements this to receive an indication
	// that one of the port input values changed.
      ଆଭାସୀ ଶୂନ୍ୟ recv_vec4_from_inputs(unsigned port) =0;
      ଆଭାସୀ ଶୂନ୍ୟ recv_real_from_inputs(unsigned port);

      friend class vvp_wide_fun_t;
      ଶୂନ୍ୟ dispatch_vec4_from_ନିବେଶ(unsigned port, ଅଚଳ vvp_vector4_t&bit);
      ଶୂନ୍ୟ dispatch_real_from_ନିବେଶ(unsigned port, ଦ୍ୱିଗୁଣ bit);

    ବ୍ୟକ୍ତିଗତ:
	// Back-point to the vvp_net_t that points to me.
      vvp_net_t*ptr_;
	// Structure to track the input values from the input functors.
      unsigned nports_;
      vvp_vector4_t*port_ମୂଲ୍ୟଗୁଡ଼ିକ;
      ଦ୍ୱିଗୁଣ*port_rvalues_;
};

/*
 * The job of the input functor is only to monitor inputs to the
 * function and pass them to the ufunc_core object. Each functor takes
 * up to 4 inputs, with the base the port number for the first
 * function input that this functor represents.
 */
class vvp_wide_fun_t : ସର୍ବସାଧାରଣ vvp_net_fun_t {

    ସର୍ବସାଧାରଣ:
      vvp_wide_fun_t(vvp_wide_fun_core*c, unsigned base);
      ~vvp_wide_fun_t() override;

      ଶୂନ୍ୟ recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                     vvp_context_t context) override;
      ଶୂନ୍ୟ recv_real(vvp_net_ptr_t port, ଦ୍ୱିଗୁଣ bit,
                     vvp_context_t context) override;

      ଶୂନ୍ୟ recv_vec4_pv(vvp_net_ptr_t p, ଅଚଳ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t context) override;

    ବ୍ୟକ୍ତିଗତ:
      vvp_wide_fun_core*core_;
      unsigned port_base_;
};


ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ vvp_send_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&val, vvp_context_t context)
{
      ଯେତେବେଳେ (class vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    ଯଦି (cur->fun)
		  cur->fun->recv_vec4(ptr, val, context);

	    ptr = next_val;
      }
}

ବାହ୍ୟ ଶୂନ୍ୟ vvp_send_vec8(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector8_t&val);
ବାହ୍ୟ ଶୂନ୍ୟ vvp_send_real(vvp_net_ptr_t ptr, ଦ୍ୱିଗୁଣ val,
                          vvp_context_t context);

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ vvp_send_string(vvp_net_ptr_t ptr, ଅଚଳ std::string&val, vvp_context_t context)
{
      ଯେତେବେଳେ (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    ଯଦି (cur->fun)
		  cur->fun->recv_string(ptr, val, context);

	    ptr = next_val;
      }
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ vvp_send_object(vvp_net_ptr_t ptr, vvp_object_t val, vvp_context_t context)
{
      ଯେତେବେଳେ (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    ଯଦି (cur->fun)
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
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ vvp_send_vec4_pv(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&val,
			     unsigned base, unsigned vwid,
			     vvp_context_t context)
{
      ଯେତେବେଳେ (class vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    ଯଦି (cur->fun)
		  cur->fun->recv_vec4_pv(ptr, val, base, vwid, context);

	    ptr = next_val;
      }
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ vvp_send_vec8_pv(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector8_t&val,
			     unsigned base, unsigned vwid)
{
      ଯେତେବେଳେ (class vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t next_val = cur->port[ptr.port()];

	    ଯଦି (cur->fun)
		  cur->fun->recv_vec8_pv(ptr, val, base, vwid);

	    ptr = next_val;
      }
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ vvp_net_t::send_vec4(ଅଚଳ vvp_vector4_t&val, vvp_context_t context)
{
      ଯଦି (fil == 0) {
	    vvp_send_vec4(out_, val, context);
	    ଫେରନ୍ତୁ;
      }

      vvp_vector4_t rep;
      ବାଛ (fil->filter_vec4(val, rep, 0, val.size())) {
	  ସ୍ଥିତି vvp_net_fil_t::STOP:
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vvp_net_fil_t::PROP:
	    vvp_send_vec4(out_, val, context);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vvp_net_fil_t::REPL:
	    vvp_send_vec4(out_, rep, context);
	    ଭାଙ୍ଗ;
      }
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ vvp_net_t::send_vec4_pv(ଅଚଳ vvp_vector4_t&val,
				    unsigned base, unsigned vwid,
				    vvp_context_t context)
{
      ଯଦି (fil == 0) {
	    vvp_send_vec4_pv(out_, val, base, vwid, context);
	    ଫେରନ୍ତୁ;
      }

      vvp_vector4_t rep;
      ବାଛ (fil->filter_vec4(val, rep, base, vwid)) {
	  ସ୍ଥିତି vvp_net_fil_t::STOP:
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vvp_net_fil_t::PROP:
	    vvp_send_vec4_pv(out_, val, base, vwid, context);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vvp_net_fil_t::REPL:
	    vvp_send_vec4_pv(out_, rep, base, vwid, context);
	    ଭାଙ୍ଗ;
      }
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ vvp_net_t::send_vec8(ଅଚଳ vvp_vector8_t&val)
{
      ଯଦି (fil == 0) {
	    vvp_send_vec8(out_, val);
	    ଫେରନ୍ତୁ;
      }

      vvp_vector8_t rep;
      ବାଛ (fil->filter_vec8(val, rep, 0, val.size())) {
	  ସ୍ଥିତି vvp_net_fil_t::STOP:
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vvp_net_fil_t::PROP:
	    vvp_send_vec8(out_, val);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vvp_net_fil_t::REPL:
	    vvp_send_vec8(out_, rep);
	    ଭାଙ୍ଗ;
      }
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ vvp_net_t::send_vec8_pv(ଅଚଳ vvp_vector8_t&val,
				    unsigned base, unsigned vwid)
{
      ଯଦି (fil == 0) {
	    vvp_send_vec8_pv(out_, val, base, vwid);
	    ଫେରନ୍ତୁ;
      }

      vvp_vector8_t rep;
      ବାଛ (fil->filter_vec8(val, rep, base, vwid)) {
	  ସ୍ଥିତି vvp_net_fil_t::STOP:
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vvp_net_fil_t::PROP:
	    vvp_send_vec8_pv(out_, val, base, vwid);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vvp_net_fil_t::REPL:
	    vvp_send_vec8_pv(out_, rep, base, vwid);
	    ଭାଙ୍ଗ;
      }
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ vvp_net_t::send_real(ଦ୍ୱିଗୁଣ val, vvp_context_t context)
{
      ଯଦି (fil && ! fil->filter_real(val))
	    ଫେରନ୍ତୁ;

      vvp_send_real(out_, val, context);
}


ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ vvp_net_t::send_string(ଅଚଳ std::string&val, vvp_context_t context)
{
      ଯଦି (fil && !fil->filter_string(val))
	    ଫେରନ୍ତୁ;

      vvp_send_string(out_, val, context);
}


ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ vvp_net_t::send_object(vvp_object_t val, vvp_context_t context)
{
      ଯଦି (fil && ! fil->filter_object(val))
	    ଫେରନ୍ତୁ;

      vvp_send_object(out_, val, context);
}


ଧାଡ଼ିଭିତରେ bool vvp_net_fil_t::test_force_mask(unsigned bit) ଅଚଳ
{
      ଯଦି (bit >= force_mask_.size())
	    ଫେରନ୍ତୁ false;
      ଯଦି (force_mask_.ମୂଲ୍ୟ(bit))
	    ଫେରନ୍ତୁ true;
      ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ false;
}

ଧାଡ଼ିଭିତରେ bool vvp_net_fil_t::test_force_mask_is_zero(ଶୂନ୍ୟ) ଅଚଳ
{
      ଯଦି (force_mask_.size() == 0)
	    ଫେରନ୍ତୁ true;
      ଯଦି (force_mask_.is_zero())
	    ଫେରନ୍ତୁ true;
      ଫେରନ୍ତୁ false;
}

/*
 * Undefine the ivl_alloc.h definitions so they don't leak out of this file.
 */
#undef malloc
#undef realloc
#undef calloc
#undef __ivl_alloc_H

#endif /* IVL_vvp_net_H */
