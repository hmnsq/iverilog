/*
 * Copyright (c) 2001-2026 Stephen Williams (steve@icarus.com)
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

# include  "arith.h"
# include  "schedule.h"
# include  <climits>
# include  <iostream>
# include  <cassert>
# include  <cstdlib>
# include  <cmath>

using ନାମାକାଶ std;

vvp_arith_::vvp_arith_(unsigned wid)
: wid_(wid), op_a_(wid), op_b_(wid), x_val_(wid)
{
      ପାଇଁ (unsigned idx = 0 ;  idx < wid ;  idx += 1) {
	    op_a_ .set_bit(idx, BIT4_Z);
	    op_b_ .set_bit(idx, BIT4_Z);
	    x_val_.set_bit(idx, BIT4_X);
      }
}

ଶୂନ୍ୟ vvp_arith_::dispatch_operand_(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit)
{
      unsigned port = ptr.port();
      ବାଛ (port) {
	  ସ୍ଥିତି 0:
	    op_a_ = bit;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 1:
	    op_b_ = bit;
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Unsupported port type %u.\n", port);
	    ପ୍ରତିଜ୍ଞା(0);
      }
}

ଶୂନ୍ୟ vvp_arith_::recv_vec4_pv(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
			      unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

vvp_arith_abs::vvp_arith_abs()
{
}

vvp_arith_abs::~vvp_arith_abs()
{
}

ଶୂନ୍ୟ vvp_arith_abs::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                              vvp_context_t)
{
      vvp_vector4_t out (bit.size(), BIT4_0);;

      vvp_bit4_t cmp = compare_gtge_signed(bit, out, BIT4_1);
      ବାଛ (cmp) {
	  ସ୍ଥିତି BIT4_1: // bit >= 0
	    out = bit;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_0: //  bit < 0
	    out = ~bit;
	    out += 1;
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ: // There's an X.
	    out = vvp_vector4_t(bit.size(), BIT4_X);
	    ଭାଙ୍ଗ;
      }

      ptr.ptr()->send_vec4(out, 0);
}

ଶୂନ୍ୟ vvp_arith_abs::recv_vec4_pv(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
				 unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

ଶୂନ୍ୟ vvp_arith_abs::recv_real(vvp_net_ptr_t ptr, ଦ୍ୱିଗୁଣ bit,
                              vvp_context_t)
{
      ଦ୍ୱିଗୁଣ out = fabs(bit);
      ptr.ptr()->send_real(out, 0);
}

vvp_arith_cast_int::vvp_arith_cast_int(unsigned wid)
: wid_(wid)
{
}

vvp_arith_cast_int::~vvp_arith_cast_int()
{
}

ଶୂନ୍ୟ vvp_arith_cast_int::recv_real(vvp_net_ptr_t ptr, ଦ୍ୱିଗୁଣ bit,
                                   vvp_context_t)
{
      ptr.ptr()->send_vec4(vvp_vector4_t(wid_, bit), 0);
}

vvp_arith_cast_real::vvp_arith_cast_real(ତାର୍କିକ signed_flag)
: signed_(signed_flag)
{
}

vvp_arith_cast_real::~vvp_arith_cast_real()
{
}

ଶୂନ୍ୟ vvp_arith_cast_real::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                                    vvp_context_t)
{
      ଦ୍ୱିଗୁଣ val;
      vector4_to_ମୂଲ୍ୟ(bit, val, signed_);
      ptr.ptr()->send_real(val, 0);
}

ଶୂନ୍ୟ vvp_arith_cast_real::recv_vec4_pv(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
				       unsigned base, unsigned vwid,
				       vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

vvp_arith_cast_vec2::vvp_arith_cast_vec2(unsigned wid)
: wid_(wid)
{
}

vvp_arith_cast_vec2::~vvp_arith_cast_vec2()
{
}

ଶୂନ୍ୟ vvp_arith_cast_vec2::recv_real(vvp_net_ptr_t ptr, ଦ୍ୱିଗୁଣ bit,
                                   vvp_context_t)
{
      ptr.ptr()->send_vec4(vvp_vector4_t(wid_, bit), 0);
}

ଶୂନ୍ୟ vvp_arith_cast_vec2::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                                    vvp_context_t)
{
      vvp_vector2_t tmp = vvp_vector2_t(bit);
      ptr.ptr()->send_vec4(vector2_to_vector4(tmp,wid_), 0);
}

ଶୂନ୍ୟ vvp_arith_cast_vec2::recv_vec4_pv(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
				       unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

// Division

vvp_arith_div::vvp_arith_div(unsigned wid, ତାର୍କିକ signed_flag)
: vvp_arith_(wid), signed_flag_(signed_flag)
{
}

vvp_arith_div::~vvp_arith_div()
{
}

ଶୂନ୍ୟ vvp_arith_div::wide4_(vvp_net_ptr_t ptr)
{
      vvp_vector2_t a2 (op_a_, ସତ୍ୟ);
      ଯଦି (a2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ଫେରନ୍ତୁ;
      }

      vvp_vector2_t b2 (op_b_, ସତ୍ୟ);
      ଯଦି (b2.is_NaN() || b2.is_zero()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ଫେରନ୍ତୁ;
      }

      ତାର୍କିକ negate = ମିଥ୍ୟା;
      ଯଦି (signed_flag_) {
	    ଯଦି (a2.value(a2.size()-1)) {
		  a2 = -a2;
		  negate = ସତ୍ୟ;
            }
	    ଯଦି (b2.value(b2.size()-1)) {
		  b2 = -b2;
		  negate = !negate;
            }
      }
      vvp_vector2_t res = a2 / b2;
      ଯଦି (negate) res = -res;
      ptr.ptr()->send_vec4(vector2_to_vector4(res, wid_), 0);
}

ଶୂନ୍ୟ vvp_arith_div::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                              vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      ଯଦି (wid_ > 8 * ଆକାର(unsigned long)) {
	    wide4_(ptr);
	    ଫେରନ୍ତୁ ;
      }

      unsigned long a;
      ଯଦି (! vector4_to_ମୂଲ୍ୟ(op_a_, a)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ଫେରନ୍ତୁ;
      }

      unsigned long b;
      ଯଦି (! vector4_to_ମୂଲ୍ୟ(op_b_, b)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ଫେରନ୍ତୁ;
      }

      ତାର୍କିକ negate = ମିଥ୍ୟା;
	/* If we are doing signed divide, then take the sign out of
	   the operands for now, and remember to put the sign back
	   later. */
      ଯଦି (signed_flag_) {
	    unsigned long sign_mask = 0;
	    ଯଦି (op_a_.size() != 8 * ଆକାର(unsigned long)) {
		  sign_mask = -1UL << op_a_.size();
	    }
	    ଯଦି (op_a_.value(op_a_.size()-1)) {
		  a = (-a) & ~sign_mask;
		  negate = !negate;
	    }

	    sign_mask = 0;
	    ଯଦି (op_b_.size() != 8 * ଆକାର(unsigned long)) {
		  sign_mask = -1UL << op_b_.size();
	    }
	    ଯଦି (op_b_.value(op_b_.size()-1)) {
		  b = (-b) & ~sign_mask;
		  negate = ! negate;
	    }
      }

      ଯଦି (b == 0) {
	    vvp_vector4_t xval (wid_);
	    ପାଇଁ (unsigned idx = 0 ;  idx < wid_ ;  idx += 1)
		  xval.set_bit(idx, BIT4_X);

	    ptr.ptr()->send_vec4(xval, 0);
	    ଫେରନ୍ତୁ;
      }

      unsigned long val = a / b;
      ଯଦି (negate)
	    val = -val;

      ପ୍ରତିଜ୍ଞା(wid_ <= 8*ଆକାର(val));

      vvp_vector4_t vval (wid_);
      ପାଇଁ (unsigned idx = 0 ;  idx < wid_ ;  idx += 1) {
	    ଯଦି (val & 1)
		  vval.set_bit(idx, BIT4_1);
	    ଅନ୍ୟଥା
		  vval.set_bit(idx, BIT4_0);

	    val >>= 1;
      }

      ptr.ptr()->send_vec4(vval, 0);
}


vvp_arith_mod::vvp_arith_mod(unsigned wid, ତାର୍କିକ sf)
: vvp_arith_(wid), signed_flag_(sf)
{
}

vvp_arith_mod::~vvp_arith_mod()
{
}

ଶୂନ୍ୟ vvp_arith_mod::wide_(vvp_net_ptr_t ptr)
{
      vvp_vector2_t a2 (op_a_, ସତ୍ୟ);
      ଯଦି (a2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ଫେରନ୍ତୁ;
      }

      vvp_vector2_t b2 (op_b_, ସତ୍ୟ);
      ଯଦି (b2.is_NaN() || b2.is_zero()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ଫେରନ୍ତୁ;
      }

      ତାର୍କିକ negate = ମିଥ୍ୟା;
      ଯଦି (signed_flag_) {
	    ଯଦି (a2.value(a2.size()-1)) {
		  a2 = -a2;
		  negate = ସତ୍ୟ;
            }
	    ଯଦି (b2.value(b2.size()-1)) {
		  b2 = -b2;
            }
      }
      vvp_vector2_t res = a2 % b2;
      ଯଦି (negate) res = -res;
      ptr.ptr()->send_vec4(vector2_to_vector4(res, res.size()), 0);
}

ଶୂନ୍ୟ vvp_arith_mod::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                              vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      ଯଦି (wid_ > 8 * ଆକାର(unsigned long)) {
	    wide_(ptr);
	    ଫେରନ୍ତୁ ;
      }

      unsigned long a;
      ଯଦି (! vector4_to_ମୂଲ୍ୟ(op_a_, a)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ଫେରନ୍ତୁ;
      }

      unsigned long b;
      ଯଦି (! vector4_to_ମୂଲ୍ୟ(op_b_, b)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ଫେରନ୍ତୁ;
      }

      ତାର୍କିକ negate = ମିଥ୍ୟା;
	/* If we are doing signed divide, then take the sign out of
	   the operands for now, and remember to put the sign back
	   later. */
      ଯଦି (signed_flag_) {
	    unsigned long sign_mask = 0;
	    ଯଦି (op_a_.size() != 8 * ଆକାର(unsigned long)) {
		  sign_mask = -1UL << op_a_.size();
	    }
	    ଯଦି (op_a_.value(op_a_.size()-1)) {
		  a = (-a) & ~sign_mask;
		  negate = !negate;
	    }

	    sign_mask = 0;
	    ଯଦି (op_b_.size() != 8 * ଆକାର(unsigned long)) {
		  sign_mask = -1UL << op_b_.size();
	    }
	    ଯଦି (op_b_.value(op_b_.size()-1)) {
		  b = (-b) & ~sign_mask;
	    }
      }

      ଯଦି (b == 0) {
	    vvp_vector4_t xval (wid_);
	    ପାଇଁ (unsigned idx = 0 ;  idx < wid_ ;  idx += 1)
		  xval.set_bit(idx, BIT4_X);

	    ptr.ptr()->send_vec4(xval, 0);
	    ଫେରନ୍ତୁ;
      }

      unsigned long val = a % b;
      ଯଦି (negate)
	    val = -val;

      ପ୍ରତିଜ୍ଞା(wid_ <= 8*ଆକାର(val));

      vvp_vector4_t vval (wid_);
      ପାଇଁ (unsigned idx = 0 ;  idx < wid_ ;  idx += 1) {
	    ଯଦି (val & 1)
		  vval.set_bit(idx, BIT4_1);
	    ଅନ୍ୟଥା
		  vval.set_bit(idx, BIT4_0);

	    val >>= 1;
      }

      ptr.ptr()->send_vec4(vval, 0);
}


// Multiplication

vvp_arith_mult::vvp_arith_mult(unsigned wid)
: vvp_arith_(wid)
{
}

vvp_arith_mult::~vvp_arith_mult()
{
}

ଶୂନ୍ୟ vvp_arith_mult::wide_(vvp_net_ptr_t ptr)
{
      vvp_vector2_t a2 (op_a_, ସତ୍ୟ);
      vvp_vector2_t b2 (op_b_, ସତ୍ୟ);

      ଯଦି (a2.is_NaN() || b2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ଫେରନ୍ତୁ;
      }

      vvp_vector2_t result = a2 * b2;

      vvp_vector4_t res4 = vector2_to_vector4(result, wid_);
      ptr.ptr()->send_vec4(res4, 0);
}

ଶୂନ୍ୟ vvp_arith_mult::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                               vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      ଯଦି (wid_ > 8 * ଆକାର(int64_t)) {
	    wide_(ptr);
	    ଫେରନ୍ତୁ ;
      }

      int64_t a;
      ଯଦି (! vector4_to_ମୂଲ୍ୟ(op_a_, a, ମିଥ୍ୟା, ସତ୍ୟ)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ଫେରନ୍ତୁ;
      }

      int64_t b;
      ଯଦି (! vector4_to_ମୂଲ୍ୟ(op_b_, b, ମିଥ୍ୟା, ସତ୍ୟ)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ଫେରନ୍ତୁ;
      }

      int64_t val = a * b;
      ପ୍ରତିଜ୍ଞା(wid_ <= 8*ଆକାର(val));

      vvp_vector4_t vval (wid_);
      ପାଇଁ (unsigned idx = 0 ;  idx < wid_ ;  idx += 1) {
	    ଯଦି (val & 1)
		  vval.set_bit(idx, BIT4_1);
	    ଅନ୍ୟଥା
		  vval.set_bit(idx, BIT4_0);

	    val >>= 1;
      }

      ptr.ptr()->send_vec4(vval, 0);
}


// Power

vvp_arith_pow::vvp_arith_pow(unsigned wid, ତାର୍କିକ signed_flag)
: vvp_arith_(wid), signed_flag_(signed_flag)
{
}

vvp_arith_pow::~vvp_arith_pow()
{
}

ଶୂନ୍ୟ vvp_arith_pow::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                              vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      vvp_vector2_t a2 (op_a_, ସତ୍ୟ);
      vvp_vector2_t b2 (op_b_, ସତ୍ୟ);

        // If we have an X or Z in the arguments return X.
      ଯଦି (a2.is_NaN() || b2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ଫେରନ୍ତୁ;
      }

	// Is the exponent negative? If so, table 5-6 in IEEE1364-2005
	// defines what value is returned.
      ଯଦି (signed_flag_ && b2.value(b2.size()-1)) {
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_val;
	    ଦ୍ୱିଗୁଣ r_val = 0.0;
	    ଯଦି (vector2_to_ମୂଲ୍ୟ(a2, a_val, ସତ୍ୟ)) {
		  ଯଦି (a_val == 0) {
			ptr.ptr()->send_vec4(x_val_, 0);
			ଫେରନ୍ତୁ;
		  }
		  ଯଦି (a_val == 1) {
			r_val = 1.0;
		  }
		  ଯଦି (a_val == -1) {
			r_val = b2.value(0) ? -1.0 : 1.0;
		  }
	    }
	    ptr.ptr()->send_vec4(vvp_vector4_t(wid_, r_val), 0);
	    ଫେରନ୍ତୁ;
      }

      ptr.ptr()->send_vec4(vector2_to_vector4(pow(a2, b2), wid_), 0);
}


// Addition

vvp_arith_sum::vvp_arith_sum(unsigned wid)
: vvp_arith_(wid)
{
}

vvp_arith_sum::~vvp_arith_sum()
{
}

ଶୂନ୍ୟ vvp_arith_sum::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                              vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      vvp_net_t*net = ptr.ptr();

      vvp_vector4_t value (wid_);

	/* Pad input vectors with this value to widen to the desired
	   output width. */
      ଅଚଳ vvp_bit4_t pad = BIT4_0;

      vvp_bit4_t carry = BIT4_0;
      ପାଇଁ (unsigned idx = 0 ;  idx < wid_ ;  idx += 1) {
	    vvp_bit4_t a = (idx >= op_a_.size())? pad : op_a_.value(idx);
	    vvp_bit4_t b = (idx >= op_b_.size())? pad : op_b_.value(idx);
	    vvp_bit4_t cur = add_with_carry(a, b, carry);

	    ଯଦି (cur == BIT4_X) {
		  net->send_vec4(x_val_, 0);
		  ଫେରନ୍ତୁ;
	    }

	    value.set_bit(idx, cur);
      }

      net->send_vec4(value, 0);
}

vvp_arith_sub::vvp_arith_sub(unsigned wid)
: vvp_arith_(wid)
{
}

vvp_arith_sub::~vvp_arith_sub()
{
}

/*
 * Subtraction works by adding the 2s complement of the B input from
 * the A input. The 2s complement is the 1s complement plus one, so we
 * further reduce the operation to adding in the inverted value and
 * adding a correction.
 */
ଶୂନ୍ୟ vvp_arith_sub::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                              vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      vvp_net_t*net = ptr.ptr();

      vvp_vector4_t value (wid_);

	/* Pad input vectors with this value to widen to the desired
	   output width. */
      ଅଚଳ vvp_bit4_t pad = BIT4_1;

      vvp_bit4_t carry = BIT4_1;
      ପାଇଁ (unsigned idx = 0 ;  idx < wid_ ;  idx += 1) {
	    vvp_bit4_t a = (idx >= op_a_.size())? pad : op_a_.value(idx);
	    vvp_bit4_t b = (idx >= op_b_.size())? pad : ~op_b_.value(idx);
	    vvp_bit4_t cur = add_with_carry(a, b, carry);

	    ଯଦି (cur == BIT4_X) {
		  net->send_vec4(x_val_, 0);
		  ଫେରନ୍ତୁ;
	    }

	    value.set_bit(idx, cur);
      }

      net->send_vec4(value, 0);
}

vvp_cmp_eeq::vvp_cmp_eeq(unsigned wid)
: vvp_arith_(wid)
{
}

ଶୂନ୍ୟ vvp_cmp_eeq::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                            vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      vvp_vector4_t eeq (1);
      eeq.set_bit(0, BIT4_1);

      ପ୍ରତିଜ୍ଞା(op_a_.size() == op_b_.size());
      ପାଇଁ (unsigned idx = 0 ;  idx < op_a_.size() ;  idx += 1)
	    ଯଦି (op_a_.value(idx) != op_b_.value(idx)) {
		  eeq.set_bit(0, BIT4_0);
		  ଭାଙ୍ଗ;
	    }


      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}

vvp_cmp_nee::vvp_cmp_nee(unsigned wid)
: vvp_arith_(wid)
{
}

ଶୂନ୍ୟ vvp_cmp_nee::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                            vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      vvp_vector4_t eeq (1);
      eeq.set_bit(0, BIT4_0);

      ପ୍ରତିଜ୍ଞା(op_a_.size() == op_b_.size());
      ପାଇଁ (unsigned idx = 0 ;  idx < op_a_.size() ;  idx += 1)
	    ଯଦି (op_a_.value(idx) != op_b_.value(idx)) {
		  eeq.set_bit(0, BIT4_1);
		  ଭାଙ୍ଗ;
	    }


      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}

vvp_cmp_eq::vvp_cmp_eq(unsigned wid)
: vvp_arith_(wid)
{
}

/*
 * Compare Vector a and Vector b. If in any bit position the a and b
 * bits are known and different, then the result is 0. Otherwise, if
 * there are X/Z bits anywhere in A or B, the result is X. Finally,
 * the result is 1.
 */
ଶୂନ୍ୟ vvp_cmp_eq::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                           vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      ଯଦି (op_a_.size() != op_b_.size()) {
	    cerr << "COMPARISON size mismatch. "
		 << "a=" << op_a_ << ", b=" << op_b_ << endl;
	    ପ୍ରତିଜ୍ଞା(0);
      }

      vvp_vector4_t res (1);
      res.set_bit(0, BIT4_1);

      ପାଇଁ (unsigned idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    ଯଦି (a == BIT4_X)
		  res.set_bit(0, BIT4_X);
	    ଅନ୍ୟଥା ଯଦି (a == BIT4_Z)
		  res.set_bit(0, BIT4_X);
	    ଅନ୍ୟଥା ଯଦି (b == BIT4_X)
		  res.set_bit(0, BIT4_X);
	    ଅନ୍ୟଥା ଯଦି (b == BIT4_Z)
		  res.set_bit(0, BIT4_X);
            ଅନ୍ୟଥା ଯଦି (a != b) {
		  res.set_bit(0, BIT4_0);
		  ଭାଙ୍ଗ;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}

vvp_cmp_eqx::vvp_cmp_eqx(unsigned wid)
: vvp_arith_(wid)
{
}

/*
 * Compare Vector a and Vector b. If in any bit position the a and b
 * bits are known and different, then the result is 0. Otherwise, if
 * there are X/Z bits anywhere in A or B, the result is X. Finally,
 * the result is 1.
 */
ଶୂନ୍ୟ vvp_cmp_eqx::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                           vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      ଯଦି (op_a_.size() != op_b_.size()) {
	    cerr << "COMPARISON size mismatch. "
		 << "a=" << op_a_ << ", b=" << op_b_ << endl;
	    ପ୍ରତିଜ୍ଞା(0);
      }

      vvp_vector4_t res (1);
      res.set_bit(0, BIT4_1);

      ପାଇଁ (unsigned idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    ଯଦି (b == BIT4_X)
		  ଜାରି;
	    ଯଦି (b == BIT4_Z)
		  ଜାରି;
	    ଯଦି (a != b) {
		  res.set_bit(0, BIT4_0);
		  ଭାଙ୍ଗ;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}

vvp_cmp_eqz::vvp_cmp_eqz(unsigned wid)
: vvp_arith_(wid)
{
}

ଶୂନ୍ୟ vvp_cmp_eqz::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                           vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      ଯଦି (op_a_.size() != op_b_.size()) {
	    cerr << "COMPARISON size mismatch. "
		 << "a=" << op_a_ << ", b=" << op_b_ << endl;
	    ପ୍ରତିଜ୍ଞା(0);
      }

      vvp_vector4_t res (1);
      res.set_bit(0, BIT4_1);

      ପାଇଁ (unsigned idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    ଯଦି (b == BIT4_Z)
		  ଜାରି;
	    ଯଦି (a != b) {
		  res.set_bit(0, BIT4_0);
		  ଭାଙ୍ଗ;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}

vvp_cmp_ne::vvp_cmp_ne(unsigned wid)
: vvp_arith_(wid)
{
}

/*
 * Compare Vector a and Vector b. If in any bit position the a and b
 * bits are known and different, then the result is 1. Otherwise, if
 * there are X/Z bits anywhere in A or B, the result is X. Finally,
 * the result is 0.
 */
ଶୂନ୍ୟ vvp_cmp_ne::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                           vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      ଯଦି (op_a_.size() != op_b_.size()) {
	    cerr << "internal error: vvp_cmp_ne: op_a_=" << op_a_
		 << ", op_b_=" << op_b_ << endl;
	    ପ୍ରତିଜ୍ଞା(op_a_.size() == op_b_.size());
      }

      vvp_vector4_t res (1);
      res.set_bit(0, BIT4_0);

      ପାଇଁ (unsigned idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    ଯଦି (a == BIT4_X)
		  res.set_bit(0, BIT4_X);
	    ଅନ୍ୟଥା ଯଦି (a == BIT4_Z)
		  res.set_bit(0, BIT4_X);
	    ଅନ୍ୟଥା ଯଦି (b == BIT4_X)
		  res.set_bit(0, BIT4_X);
	    ଅନ୍ୟଥା ଯଦି (b == BIT4_Z)
		  res.set_bit(0, BIT4_X);
            ଅନ୍ୟଥା ଯଦି (a != b) {
		  res.set_bit(0, BIT4_1);
		  ଭାଙ୍ଗ;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}


vvp_cmp_gtge_base_::vvp_cmp_gtge_base_(unsigned wid, ତାର୍କିକ flag)
: vvp_arith_(wid), signed_flag_(flag)
{
}


ଶୂନ୍ୟ vvp_cmp_gtge_base_::recv_vec4_base_(vvp_net_ptr_t ptr,
					 ଅଚଳ vvp_vector4_t&bit,
					 vvp_bit4_t out_if_equal)
{
      dispatch_operand_(ptr, bit);

      vvp_bit4_t out = signed_flag_
	    ? compare_gtge_signed(op_a_, op_b_, out_if_equal)
	    : compare_gtge(op_a_, op_b_, out_if_equal);
      vvp_vector4_t val (1);
      val.set_bit(0, out);
      ptr.ptr()->send_vec4(val, 0);

      ଫେରନ୍ତୁ;
}


vvp_cmp_ge::vvp_cmp_ge(unsigned wid, ତାର୍କିକ flag)
: vvp_cmp_gtge_base_(wid, flag)
{
}

ଶୂନ୍ୟ vvp_cmp_ge::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                           vvp_context_t)
{
      recv_vec4_base_(ptr, bit, BIT4_1);
}

vvp_cmp_gt::vvp_cmp_gt(unsigned wid, ତାର୍କିକ flag)
: vvp_cmp_gtge_base_(wid, flag)
{
}

ଶୂନ୍ୟ vvp_cmp_gt::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                           vvp_context_t)
{
      recv_vec4_base_(ptr, bit, BIT4_0);
}

vvp_cmp_weq::vvp_cmp_weq(unsigned wid)
: vvp_arith_(wid)
{
}

ଶୂନ୍ୟ vvp_cmp_weq::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                            vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      vvp_vector4_t eeq (1);
      eeq.set_bit(0, BIT4_1);

      ପ୍ରତିଜ୍ଞା(op_a_.size() == op_b_.size());
      ପାଇଁ (unsigned idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);
	    ଯଦି (b == BIT4_X)
		  ଜାରି;
	    ଅନ୍ୟଥା ଯଦି (b == BIT4_Z)
		  ଜାରି;
	    ଅନ୍ୟଥା ଯଦି (a == BIT4_X)
		  eeq.set_bit(0, BIT4_X);
	    ଅନ୍ୟଥା ଯଦି (a == BIT4_Z)
		  eeq.set_bit(0, BIT4_X);
            ଅନ୍ୟଥା ଯଦି (a != b) {
		  eeq.set_bit(0, BIT4_0);
		  ଭାଙ୍ଗ;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}

vvp_cmp_wne::vvp_cmp_wne(unsigned wid)
: vvp_arith_(wid)
{
}

ଶୂନ୍ୟ vvp_cmp_wne::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                            vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      vvp_vector4_t eeq (1);
      eeq.set_bit(0, BIT4_0);

      ପ୍ରତିଜ୍ଞା(op_a_.size() == op_b_.size());
      ପାଇଁ (unsigned idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);
	    ଯଦି (b == BIT4_X)
		  ଜାରି;
	    ଅନ୍ୟଥା ଯଦି (b == BIT4_Z)
		  ଜାରି;
	    ଅନ୍ୟଥା ଯଦି (a == BIT4_X)
		  eeq.set_bit(0, BIT4_X);
	    ଅନ୍ୟଥା ଯଦି (a == BIT4_Z)
		  eeq.set_bit(0, BIT4_X);
            ଅନ୍ୟଥା ଯଦି (a != b) {
		  eeq.set_bit(0, BIT4_1);
		  ଭାଙ୍ଗ;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}


vvp_shiftl::vvp_shiftl(unsigned wid)
: vvp_arith_(wid)
{
}

vvp_shiftl::~vvp_shiftl()
{
}

ଶୂନ୍ୟ vvp_shiftl::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                           vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      vvp_vector4_t out (op_a_.size());

      ତାର୍କିକ overflow_flag;
      unsigned long shift = 0;
      ଯଦି (! vector4_to_ମୂଲ୍ୟ(op_b_, overflow_flag, shift)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ଫେରନ୍ତୁ;
      }

      ଯଦି (overflow_flag || shift > out.size())
	    shift = out.size();

      ପାଇଁ (unsigned idx = 0 ;  idx < shift ;  idx += 1)
	    out.set_bit(idx, BIT4_0);

      ପାଇଁ (unsigned idx = shift ;  idx < out.size() ;  idx += 1)
	    out.set_bit(idx, op_a_.value(idx-shift));

      ptr.ptr()->send_vec4(out, 0);
}

vvp_shiftr::vvp_shiftr(unsigned wid, ତାର୍କିକ signed_flag)
: vvp_arith_(wid), signed_flag_(signed_flag)
{
}

vvp_shiftr::~vvp_shiftr()
{
}

ଶୂନ୍ୟ vvp_shiftr::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                           vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      vvp_vector4_t out (op_a_.size());

      ତାର୍କିକ overflow_flag;
      unsigned long shift = 0;
      ଯଦି (! vector4_to_ମୂଲ୍ୟ(op_b_, overflow_flag, shift)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ଫେରନ୍ତୁ;
      }

      ଯଦି (overflow_flag || shift > out.size())
	    shift = out.size();

      ପାଇଁ (unsigned idx = shift ;  idx < out.size() ;  idx += 1)
	    out.set_bit(idx-shift, op_a_.value(idx));

      vvp_bit4_t pad = BIT4_0;
      ଯଦି (signed_flag_ && op_a_.size() > 0)
	    pad = op_a_.value(op_a_.size()-1);

      ପାଇଁ (unsigned idx = 0 ;  idx < shift ;  idx += 1)
	    out.set_bit(idx+out.size()-shift, pad);

      ptr.ptr()->send_vec4(out, 0);
}


vvp_arith_real_::vvp_arith_real_()
{
      op_a_ = 0.0;
      op_b_ = 0.0;
}

ଶୂନ୍ୟ vvp_arith_real_::dispatch_operand_(vvp_net_ptr_t ptr, ଦ୍ୱିଗୁଣ bit)
{
      ବାଛ (ptr.port()) {
	  ସ୍ଥିତି 0:
	    op_a_ = bit;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 1:
	    op_b_ = bit;
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    ପ୍ରତିଜ୍ଞା(0);
      }
}


/* Real multiplication. */
vvp_arith_mult_real::vvp_arith_mult_real()
{
}

vvp_arith_mult_real::~vvp_arith_mult_real()
{
}

ଶୂନ୍ୟ vvp_arith_mult_real::recv_real(vvp_net_ptr_t ptr, ଦ୍ୱିଗୁଣ bit,
                                    vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      ଦ୍ୱିଗୁଣ val = op_a_ * op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real power. */
vvp_arith_pow_real::vvp_arith_pow_real()
{
}

vvp_arith_pow_real::~vvp_arith_pow_real()
{
}

ଶୂନ୍ୟ vvp_arith_pow_real::recv_real(vvp_net_ptr_t ptr, ଦ୍ୱିଗୁଣ bit,
                                   vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      ଦ୍ୱିଗୁଣ val = pow(op_a_, op_b_);
      ptr.ptr()->send_real(val, 0);
}

/* Real division. */
vvp_arith_div_real::vvp_arith_div_real()
{
}

vvp_arith_div_real::~vvp_arith_div_real()
{
}

ଶୂନ୍ୟ vvp_arith_div_real::recv_real(vvp_net_ptr_t ptr, ଦ୍ୱିଗୁଣ bit,
                                   vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      ଦ୍ୱିଗୁଣ val = op_a_ / op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real modulus. */
vvp_arith_mod_real::vvp_arith_mod_real()
{
}

vvp_arith_mod_real::~vvp_arith_mod_real()
{
}

ଶୂନ୍ୟ vvp_arith_mod_real::recv_real(vvp_net_ptr_t ptr, ଦ୍ୱିଗୁଣ bit,
                                   vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      ଦ୍ୱିଗୁଣ val = fmod(op_a_, op_b_);
      ptr.ptr()->send_real(val, 0);
}

/* Real summation. */
vvp_arith_sum_real::vvp_arith_sum_real()
{
}

vvp_arith_sum_real::~vvp_arith_sum_real()
{
}

ଶୂନ୍ୟ vvp_arith_sum_real::recv_real(vvp_net_ptr_t ptr, ଦ୍ୱିଗୁଣ bit,
                                   vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      ଦ୍ୱିଗୁଣ val = op_a_ + op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real subtraction. */
vvp_arith_sub_real::vvp_arith_sub_real()
{
}

vvp_arith_sub_real::~vvp_arith_sub_real()
{
}

ଶୂନ୍ୟ vvp_arith_sub_real::recv_real(vvp_net_ptr_t ptr, ଦ୍ୱିଗୁଣ bit,
                                   vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      ଦ୍ୱିଗୁଣ val = op_a_ - op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real compare equal. */
vvp_cmp_eq_real::vvp_cmp_eq_real()
{
}

ଶୂନ୍ୟ vvp_cmp_eq_real::recv_real(vvp_net_ptr_t ptr, ଅଚଳ ଦ୍ୱିଗୁଣ bit,
                                vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      vvp_vector4_t res (1);
      ଯଦି (op_a_ == op_b_) res.set_bit(0, BIT4_1);
      ଅନ୍ୟଥା res.set_bit(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}

/* Real compare not equal. */
vvp_cmp_ne_real::vvp_cmp_ne_real()
{
}

ଶୂନ୍ୟ vvp_cmp_ne_real::recv_real(vvp_net_ptr_t ptr, ଅଚଳ ଦ୍ୱିଗୁଣ bit,
                                vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      vvp_vector4_t res (1);
      ଯଦି (op_a_ != op_b_) res.set_bit(0, BIT4_1);
      ଅନ୍ୟଥା res.set_bit(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}

/* Real compare greater than or equal. */
vvp_cmp_ge_real::vvp_cmp_ge_real()
{
}

ଶୂନ୍ୟ vvp_cmp_ge_real::recv_real(vvp_net_ptr_t ptr, ଅଚଳ ଦ୍ୱିଗୁଣ bit,
                                vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      vvp_vector4_t res (1);
      ଯଦି (op_a_ >= op_b_) res.set_bit(0, BIT4_1);
      ଅନ୍ୟଥା res.set_bit(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}

/* Real compare greater than. */
vvp_cmp_gt_real::vvp_cmp_gt_real()
{
}

ଶୂନ୍ୟ vvp_cmp_gt_real::recv_real(vvp_net_ptr_t ptr, ଅଚଳ ଦ୍ୱିଗୁଣ bit,
                                vvp_context_t)
{
      dispatch_operand_(ptr, bit);

      vvp_vector4_t res (1);
      ଯଦି (op_a_ > op_b_) res.set_bit(0, BIT4_1);
      ଅନ୍ୟଥା res.set_bit(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}
