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

ఉపయోగించి నామాకాశ std;

vvp_arith_::vvp_arith_(గుర్తులేని wid)
: wid_(wid), op_a_(wid), op_b_(wid), x_val_(wid)
{
      కొరకు (గుర్తులేని idx = 0 ;  idx < wid ;  idx += 1) {
	    op_a_ .సమితి_బిట్టు(idx, BIT4_Z);
	    op_b_ .సమితి_బిట్టు(idx, BIT4_Z);
	    x_val_.సమితి_బిట్టు(idx, BIT4_X);
      }
}

శూన్య vvp_arith_::dispatch_నిర్వాహ్యం(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు)
{
      గుర్తులేని పోర్టు = ptr.పోర్టు();
      ఎంపిక (పోర్టు) {
	  స్థితి 0:
	    op_a_ = బిట్టు;
	    ఆపు;
	  స్థితి 1:
	    op_b_ = బిట్టు;
	    ఆపు;
	  అప్రమేయ:
	    fprintf(stderr, "Unsupported port type %u.\n", పోర్టు);
	    ప్రతిజ్ఞా(0);
      }
}

శూన్య vvp_arith_::recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
			      గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, బిట్టు, పునాది, vwid, ctx);
}

vvp_arith_abs::vvp_arith_abs()
{
}

vvp_arith_abs::~vvp_arith_abs()
{
}

శూన్య vvp_arith_abs::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                              vvp_context_t)
{
      vvp_vector4_t out (బిట్టు.size(), BIT4_0);;

      vvp_bit4_t cmp = compare_gtge_signed(బిట్టు, out, BIT4_1);
      ఎంపిక (cmp) {
	  స్థితి BIT4_1: // bit >= 0
	    out = బిట్టు;
	    ఆపు;
	  స్థితి BIT4_0: //  bit < 0
	    out = ~బిట్టు;
	    out += 1;
	    ఆపు;
	  అప్రమేయ: // There's an X.
	    out = vvp_vector4_t(బిట్టు.size(), BIT4_X);
	    ఆపు;
      }

      ptr.ptr()->send_vec4(out, 0);
}

శూన్య vvp_arith_abs::recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
				 గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, బిట్టు, పునాది, vwid, ctx);
}

శూన్య vvp_arith_abs::recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                              vvp_context_t)
{
      ద్విగుణ out = fabs(బిట్టు);
      ptr.ptr()->send_real(out, 0);
}

vvp_arith_cast_int::vvp_arith_cast_int(గుర్తులేని wid)
: wid_(wid)
{
}

vvp_arith_cast_int::~vvp_arith_cast_int()
{
}

శూన్య vvp_arith_cast_int::recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                                   vvp_context_t)
{
      ptr.ptr()->send_vec4(vvp_vector4_t(wid_, బిట్టు), 0);
}

vvp_arith_cast_real::vvp_arith_cast_real(తార్కిక signed_flag)
: signed_(signed_flag)
{
}

vvp_arith_cast_real::~vvp_arith_cast_real()
{
}

శూన్య vvp_arith_cast_real::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                    vvp_context_t)
{
      ద్విగుణ val;
      vector4_గమ్యం_విలువ(బిట్టు, val, signed_);
      ptr.ptr()->send_real(val, 0);
}

శూన్య vvp_arith_cast_real::recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
				       గుర్తులేని పునాది, గుర్తులేని vwid,
				       vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, బిట్టు, పునాది, vwid, ctx);
}

vvp_arith_cast_vec2::vvp_arith_cast_vec2(గుర్తులేని wid)
: wid_(wid)
{
}

vvp_arith_cast_vec2::~vvp_arith_cast_vec2()
{
}

శూన్య vvp_arith_cast_vec2::recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                                   vvp_context_t)
{
      ptr.ptr()->send_vec4(vvp_vector4_t(wid_, బిట్టు), 0);
}

శూన్య vvp_arith_cast_vec2::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                    vvp_context_t)
{
      vvp_vector2_t tmp = vvp_vector2_t(బిట్టు);
      ptr.ptr()->send_vec4(vector2_గమ్యం_vector4(tmp,wid_), 0);
}

శూన్య vvp_arith_cast_vec2::recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
				       గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, బిట్టు, పునాది, vwid, ctx);
}

// Division

vvp_arith_div::vvp_arith_div(గుర్తులేని wid, తార్కిక signed_flag)
: vvp_arith_(wid), signed_flag_(signed_flag)
{
}

vvp_arith_div::~vvp_arith_div()
{
}

శూన్య vvp_arith_div::wide4_(vvp_net_ptr_t ptr)
{
      vvp_vector2_t a2 (op_a_, నిజం);
      ఒకవేళ (a2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    తిరిగి;
      }

      vvp_vector2_t b2 (op_b_, నిజం);
      ఒకవేళ (b2.is_NaN() || b2.is_zero()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    తిరిగి;
      }

      తార్కిక negate = అబద్ధం;
      ఒకవేళ (signed_flag_) {
	    ఒకవేళ (a2.value(a2.size()-1)) {
		  a2 = -a2;
		  negate = నిజం;
            }
	    ఒకవేళ (b2.value(b2.size()-1)) {
		  b2 = -b2;
		  negate = !negate;
            }
      }
      vvp_vector2_t res = a2 / b2;
      ఒకవేళ (negate) res = -res;
      ptr.ptr()->send_vec4(vector2_గమ్యం_vector4(res, wid_), 0);
}

శూన్య vvp_arith_div::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                              vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      ఒకవేళ (wid_ > 8 * పరిమాణం(గుర్తులేని దీర్ఘ)) {
	    wide4_(ptr);
	    తిరిగి ;
      }

      గుర్తులేని దీర్ఘ a;
      ఒకవేళ (! vector4_గమ్యం_విలువ(op_a_, a)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    తిరిగి;
      }

      గుర్తులేని దీర్ఘ b;
      ఒకవేళ (! vector4_గమ్యం_విలువ(op_b_, b)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    తిరిగి;
      }

      తార్కిక negate = అబద్ధం;
	/* If we are doing signed divide, then take the sign out of
	   the operands for now, and remember to put the sign back
	   later. */
      ఒకవేళ (signed_flag_) {
	    గుర్తులేని దీర్ఘ sign_ముసుగు = 0;
	    ఒకవేళ (op_a_.size() != 8 * పరిమాణం(గుర్తులేని దీర్ఘ)) {
		  sign_ముసుగు = -1UL << op_a_.size();
	    }
	    ఒకవేళ (op_a_.value(op_a_.size()-1)) {
		  a = (-a) & ~sign_ముసుగు;
		  negate = !negate;
	    }

	    sign_ముసుగు = 0;
	    ఒకవేళ (op_b_.size() != 8 * పరిమాణం(గుర్తులేని దీర్ఘ)) {
		  sign_ముసుగు = -1UL << op_b_.size();
	    }
	    ఒకవేళ (op_b_.value(op_b_.size()-1)) {
		  b = (-b) & ~sign_ముసుగు;
		  negate = ! negate;
	    }
      }

      ఒకవేళ (b == 0) {
	    vvp_vector4_t xval (wid_);
	    కొరకు (గుర్తులేని idx = 0 ;  idx < wid_ ;  idx += 1)
		  xval.సమితి_బిట్టు(idx, BIT4_X);

	    ptr.ptr()->send_vec4(xval, 0);
	    తిరిగి;
      }

      గుర్తులేని దీర్ఘ val = a / b;
      ఒకవేళ (negate)
	    val = -val;

      ప్రతిజ్ఞా(wid_ <= 8*పరిమాణం(val));

      vvp_vector4_t vval (wid_);
      కొరకు (గుర్తులేని idx = 0 ;  idx < wid_ ;  idx += 1) {
	    ఒకవేళ (val & 1)
		  vval.సమితి_బిట్టు(idx, BIT4_1);
	    లేకపోతే
		  vval.సమితి_బిట్టు(idx, BIT4_0);

	    val >>= 1;
      }

      ptr.ptr()->send_vec4(vval, 0);
}


vvp_arith_mod::vvp_arith_mod(గుర్తులేని wid, తార్కిక sf)
: vvp_arith_(wid), signed_flag_(sf)
{
}

vvp_arith_mod::~vvp_arith_mod()
{
}

శూన్య vvp_arith_mod::wide_(vvp_net_ptr_t ptr)
{
      vvp_vector2_t a2 (op_a_, నిజం);
      ఒకవేళ (a2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    తిరిగి;
      }

      vvp_vector2_t b2 (op_b_, నిజం);
      ఒకవేళ (b2.is_NaN() || b2.is_zero()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    తిరిగి;
      }

      తార్కిక negate = అబద్ధం;
      ఒకవేళ (signed_flag_) {
	    ఒకవేళ (a2.value(a2.size()-1)) {
		  a2 = -a2;
		  negate = నిజం;
            }
	    ఒకవేళ (b2.value(b2.size()-1)) {
		  b2 = -b2;
            }
      }
      vvp_vector2_t res = a2 % b2;
      ఒకవేళ (negate) res = -res;
      ptr.ptr()->send_vec4(vector2_గమ్యం_vector4(res, res.size()), 0);
}

శూన్య vvp_arith_mod::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                              vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      ఒకవేళ (wid_ > 8 * పరిమాణం(గుర్తులేని దీర్ఘ)) {
	    wide_(ptr);
	    తిరిగి ;
      }

      గుర్తులేని దీర్ఘ a;
      ఒకవేళ (! vector4_గమ్యం_విలువ(op_a_, a)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    తిరిగి;
      }

      గుర్తులేని దీర్ఘ b;
      ఒకవేళ (! vector4_గమ్యం_విలువ(op_b_, b)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    తిరిగి;
      }

      తార్కిక negate = అబద్ధం;
	/* If we are doing signed divide, then take the sign out of
	   the operands for now, and remember to put the sign back
	   later. */
      ఒకవేళ (signed_flag_) {
	    గుర్తులేని దీర్ఘ sign_ముసుగు = 0;
	    ఒకవేళ (op_a_.size() != 8 * పరిమాణం(గుర్తులేని దీర్ఘ)) {
		  sign_ముసుగు = -1UL << op_a_.size();
	    }
	    ఒకవేళ (op_a_.value(op_a_.size()-1)) {
		  a = (-a) & ~sign_ముసుగు;
		  negate = !negate;
	    }

	    sign_ముసుగు = 0;
	    ఒకవేళ (op_b_.size() != 8 * పరిమాణం(గుర్తులేని దీర్ఘ)) {
		  sign_ముసుగు = -1UL << op_b_.size();
	    }
	    ఒకవేళ (op_b_.value(op_b_.size()-1)) {
		  b = (-b) & ~sign_ముసుగు;
	    }
      }

      ఒకవేళ (b == 0) {
	    vvp_vector4_t xval (wid_);
	    కొరకు (గుర్తులేని idx = 0 ;  idx < wid_ ;  idx += 1)
		  xval.సమితి_బిట్టు(idx, BIT4_X);

	    ptr.ptr()->send_vec4(xval, 0);
	    తిరిగి;
      }

      గుర్తులేని దీర్ఘ val = a % b;
      ఒకవేళ (negate)
	    val = -val;

      ప్రతిజ్ఞా(wid_ <= 8*పరిమాణం(val));

      vvp_vector4_t vval (wid_);
      కొరకు (గుర్తులేని idx = 0 ;  idx < wid_ ;  idx += 1) {
	    ఒకవేళ (val & 1)
		  vval.సమితి_బిట్టు(idx, BIT4_1);
	    లేకపోతే
		  vval.సమితి_బిట్టు(idx, BIT4_0);

	    val >>= 1;
      }

      ptr.ptr()->send_vec4(vval, 0);
}


// Multiplication

vvp_arith_mult::vvp_arith_mult(గుర్తులేని wid)
: vvp_arith_(wid)
{
}

vvp_arith_mult::~vvp_arith_mult()
{
}

శూన్య vvp_arith_mult::wide_(vvp_net_ptr_t ptr)
{
      vvp_vector2_t a2 (op_a_, నిజం);
      vvp_vector2_t b2 (op_b_, నిజం);

      ఒకవేళ (a2.is_NaN() || b2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    తిరిగి;
      }

      vvp_vector2_t ఫలితం = a2 * b2;

      vvp_vector4_t res4 = vector2_గమ్యం_vector4(ఫలితం, wid_);
      ptr.ptr()->send_vec4(res4, 0);
}

శూన్య vvp_arith_mult::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                               vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      ఒకవేళ (wid_ > 8 * పరిమాణం(int64_t)) {
	    wide_(ptr);
	    తిరిగి ;
      }

      int64_t a;
      ఒకవేళ (! vector4_గమ్యం_విలువ(op_a_, a, అబద్ధం, నిజం)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    తిరిగి;
      }

      int64_t b;
      ఒకవేళ (! vector4_గమ్యం_విలువ(op_b_, b, అబద్ధం, నిజం)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    తిరిగి;
      }

      int64_t val = a * b;
      ప్రతిజ్ఞా(wid_ <= 8*పరిమాణం(val));

      vvp_vector4_t vval (wid_);
      కొరకు (గుర్తులేని idx = 0 ;  idx < wid_ ;  idx += 1) {
	    ఒకవేళ (val & 1)
		  vval.సమితి_బిట్టు(idx, BIT4_1);
	    లేకపోతే
		  vval.సమితి_బిట్టు(idx, BIT4_0);

	    val >>= 1;
      }

      ptr.ptr()->send_vec4(vval, 0);
}


// Power

vvp_arith_pow::vvp_arith_pow(గుర్తులేని wid, తార్కిక signed_flag)
: vvp_arith_(wid), signed_flag_(signed_flag)
{
}

vvp_arith_pow::~vvp_arith_pow()
{
}

శూన్య vvp_arith_pow::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                              vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      vvp_vector2_t a2 (op_a_, నిజం);
      vvp_vector2_t b2 (op_b_, నిజం);

        // If we have an X or Z in the arguments return X.
      ఒకవేళ (a2.is_NaN() || b2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    తిరిగి;
      }

	// Is the exponent negative? If so, table 5-6 in IEEE1364-2005
	// defines what value is returned.
      ఒకవేళ (signed_flag_ && b2.value(b2.size()-1)) {
	    పూర్ణాంకం a_val;
	    ద్విగుణ r_val = 0.0;
	    ఒకవేళ (vector2_గమ్యం_విలువ(a2, a_val, నిజం)) {
		  ఒకవేళ (a_val == 0) {
			ptr.ptr()->send_vec4(x_val_, 0);
			తిరిగి;
		  }
		  ఒకవేళ (a_val == 1) {
			r_val = 1.0;
		  }
		  ఒకవేళ (a_val == -1) {
			r_val = b2.value(0) ? -1.0 : 1.0;
		  }
	    }
	    ptr.ptr()->send_vec4(vvp_vector4_t(wid_, r_val), 0);
	    తిరిగి;
      }

      ptr.ptr()->send_vec4(vector2_గమ్యం_vector4(ఘాతం(a2, b2), wid_), 0);
}


// Addition

vvp_arith_కూడిక::vvp_arith_కూడిక(గుర్తులేని wid)
: vvp_arith_(wid)
{
}

vvp_arith_కూడిక::~vvp_arith_కూడిక()
{
}

శూన్య vvp_arith_కూడిక::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                              vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      vvp_net_t*net = ptr.ptr();

      vvp_vector4_t value (wid_);

	/* Pad input vectors with this value to widen to the desired
	   output width. */
      స్థిరరాశి vvp_bit4_t pad = BIT4_0;

      vvp_bit4_t వాహకం = BIT4_0;
      కొరకు (గుర్తులేని idx = 0 ;  idx < wid_ ;  idx += 1) {
	    vvp_bit4_t a = (idx >= op_a_.size())? pad : op_a_.value(idx);
	    vvp_bit4_t b = (idx >= op_b_.size())? pad : op_b_.value(idx);
	    vvp_bit4_t cur = కూడు_with_వాహకం(a, b, వాహకం);

	    ఒకవేళ (cur == BIT4_X) {
		  net->send_vec4(x_val_, 0);
		  తిరిగి;
	    }

	    value.సమితి_బిట్టు(idx, cur);
      }

      net->send_vec4(value, 0);
}

vvp_arith_sub::vvp_arith_sub(గుర్తులేని wid)
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
శూన్య vvp_arith_sub::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                              vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      vvp_net_t*net = ptr.ptr();

      vvp_vector4_t value (wid_);

	/* Pad input vectors with this value to widen to the desired
	   output width. */
      స్థిరరాశి vvp_bit4_t pad = BIT4_1;

      vvp_bit4_t వాహకం = BIT4_1;
      కొరకు (గుర్తులేని idx = 0 ;  idx < wid_ ;  idx += 1) {
	    vvp_bit4_t a = (idx >= op_a_.size())? pad : op_a_.value(idx);
	    vvp_bit4_t b = (idx >= op_b_.size())? pad : ~op_b_.value(idx);
	    vvp_bit4_t cur = కూడు_with_వాహకం(a, b, వాహకం);

	    ఒకవేళ (cur == BIT4_X) {
		  net->send_vec4(x_val_, 0);
		  తిరిగి;
	    }

	    value.సమితి_బిట్టు(idx, cur);
      }

      net->send_vec4(value, 0);
}

vvp_cmp_eeq::vvp_cmp_eeq(గుర్తులేని wid)
: vvp_arith_(wid)
{
}

శూన్య vvp_cmp_eeq::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                            vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      vvp_vector4_t eeq (1);
      eeq.సమితి_బిట్టు(0, BIT4_1);

      ప్రతిజ్ఞా(op_a_.size() == op_b_.size());
      కొరకు (గుర్తులేని idx = 0 ;  idx < op_a_.size() ;  idx += 1)
	    ఒకవేళ (op_a_.value(idx) != op_b_.value(idx)) {
		  eeq.సమితి_బిట్టు(0, BIT4_0);
		  ఆపు;
	    }


      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}

vvp_cmp_nee::vvp_cmp_nee(గుర్తులేని wid)
: vvp_arith_(wid)
{
}

శూన్య vvp_cmp_nee::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                            vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      vvp_vector4_t eeq (1);
      eeq.సమితి_బిట్టు(0, BIT4_0);

      ప్రతిజ్ఞా(op_a_.size() == op_b_.size());
      కొరకు (గుర్తులేని idx = 0 ;  idx < op_a_.size() ;  idx += 1)
	    ఒకవేళ (op_a_.value(idx) != op_b_.value(idx)) {
		  eeq.సమితి_బిట్టు(0, BIT4_1);
		  ఆపు;
	    }


      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}

vvp_cmp_eq::vvp_cmp_eq(గుర్తులేని wid)
: vvp_arith_(wid)
{
}

/*
 * Compare Vector a and Vector b. If in any bit position the a and b
 * bits are known and different, then the result is 0. Otherwise, if
 * there are X/Z bits anywhere in A or B, the result is X. Finally,
 * the result is 1.
 */
శూన్య vvp_cmp_eq::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                           vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      ఒకవేళ (op_a_.size() != op_b_.size()) {
	    cerr << "COMPARISON size mismatch. "
		 << "a=" << op_a_ << ", b=" << op_b_ << పంక్తి_ముగింపు;
	    ప్రతిజ్ఞా(0);
      }

      vvp_vector4_t res (1);
      res.సమితి_బిట్టు(0, BIT4_1);

      కొరకు (గుర్తులేని idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    ఒకవేళ (a == BIT4_X)
		  res.సమితి_బిట్టు(0, BIT4_X);
	    లేకపోతే ఒకవేళ (a == BIT4_Z)
		  res.సమితి_బిట్టు(0, BIT4_X);
	    లేకపోతే ఒకవేళ (b == BIT4_X)
		  res.సమితి_బిట్టు(0, BIT4_X);
	    లేకపోతే ఒకవేళ (b == BIT4_Z)
		  res.సమితి_బిట్టు(0, BIT4_X);
            లేకపోతే ఒకవేళ (a != b) {
		  res.సమితి_బిట్టు(0, BIT4_0);
		  ఆపు;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}

vvp_cmp_eqx::vvp_cmp_eqx(గుర్తులేని wid)
: vvp_arith_(wid)
{
}

/*
 * Compare Vector a and Vector b. If in any bit position the a and b
 * bits are known and different, then the result is 0. Otherwise, if
 * there are X/Z bits anywhere in A or B, the result is X. Finally,
 * the result is 1.
 */
శూన్య vvp_cmp_eqx::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                           vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      ఒకవేళ (op_a_.size() != op_b_.size()) {
	    cerr << "COMPARISON size mismatch. "
		 << "a=" << op_a_ << ", b=" << op_b_ << పంక్తి_ముగింపు;
	    ప్రతిజ్ఞా(0);
      }

      vvp_vector4_t res (1);
      res.సమితి_బిట్టు(0, BIT4_1);

      కొరకు (గుర్తులేని idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    ఒకవేళ (b == BIT4_X)
		  కొనసాగు;
	    ఒకవేళ (b == BIT4_Z)
		  కొనసాగు;
	    ఒకవేళ (a != b) {
		  res.సమితి_బిట్టు(0, BIT4_0);
		  ఆపు;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}

vvp_cmp_eqz::vvp_cmp_eqz(గుర్తులేని wid)
: vvp_arith_(wid)
{
}

శూన్య vvp_cmp_eqz::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                           vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      ఒకవేళ (op_a_.size() != op_b_.size()) {
	    cerr << "COMPARISON size mismatch. "
		 << "a=" << op_a_ << ", b=" << op_b_ << పంక్తి_ముగింపు;
	    ప్రతిజ్ఞా(0);
      }

      vvp_vector4_t res (1);
      res.సమితి_బిట్టు(0, BIT4_1);

      కొరకు (గుర్తులేని idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    ఒకవేళ (b == BIT4_Z)
		  కొనసాగు;
	    ఒకవేళ (a != b) {
		  res.సమితి_బిట్టు(0, BIT4_0);
		  ఆపు;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}

vvp_cmp_ne::vvp_cmp_ne(గుర్తులేని wid)
: vvp_arith_(wid)
{
}

/*
 * Compare Vector a and Vector b. If in any bit position the a and b
 * bits are known and different, then the result is 1. Otherwise, if
 * there are X/Z bits anywhere in A or B, the result is X. Finally,
 * the result is 0.
 */
శూన్య vvp_cmp_ne::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                           vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      ఒకవేళ (op_a_.size() != op_b_.size()) {
	    cerr << "internal error: vvp_cmp_ne: op_a_=" << op_a_
		 << ", op_b_=" << op_b_ << పంక్తి_ముగింపు;
	    ప్రతిజ్ఞా(op_a_.size() == op_b_.size());
      }

      vvp_vector4_t res (1);
      res.సమితి_బిట్టు(0, BIT4_0);

      కొరకు (గుర్తులేని idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    ఒకవేళ (a == BIT4_X)
		  res.సమితి_బిట్టు(0, BIT4_X);
	    లేకపోతే ఒకవేళ (a == BIT4_Z)
		  res.సమితి_బిట్టు(0, BIT4_X);
	    లేకపోతే ఒకవేళ (b == BIT4_X)
		  res.సమితి_బిట్టు(0, BIT4_X);
	    లేకపోతే ఒకవేళ (b == BIT4_Z)
		  res.సమితి_బిట్టు(0, BIT4_X);
            లేకపోతే ఒకవేళ (a != b) {
		  res.సమితి_బిట్టు(0, BIT4_1);
		  ఆపు;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}


vvp_cmp_gtge_పునాది::vvp_cmp_gtge_పునాది(గుర్తులేని wid, తార్కిక flag)
: vvp_arith_(wid), signed_flag_(flag)
{
}


శూన్య vvp_cmp_gtge_పునాది::recv_vec4_పునాది(vvp_net_ptr_t ptr,
					 స్థిరరాశి vvp_vector4_t&బిట్టు,
					 vvp_bit4_t బహిర్గతం_if_equal)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      vvp_bit4_t out = signed_flag_
	    ? compare_gtge_signed(op_a_, op_b_, బహిర్గతం_if_equal)
	    : compare_gtge(op_a_, op_b_, బహిర్గతం_if_equal);
      vvp_vector4_t val (1);
      val.సమితి_బిట్టు(0, out);
      ptr.ptr()->send_vec4(val, 0);

      తిరిగి;
}


vvp_cmp_ge::vvp_cmp_ge(గుర్తులేని wid, తార్కిక flag)
: vvp_cmp_gtge_పునాది(wid, flag)
{
}

శూన్య vvp_cmp_ge::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                           vvp_context_t)
{
      recv_vec4_పునాది(ptr, బిట్టు, BIT4_1);
}

vvp_cmp_gt::vvp_cmp_gt(గుర్తులేని wid, తార్కిక flag)
: vvp_cmp_gtge_పునాది(wid, flag)
{
}

శూన్య vvp_cmp_gt::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                           vvp_context_t)
{
      recv_vec4_పునాది(ptr, బిట్టు, BIT4_0);
}

vvp_cmp_weq::vvp_cmp_weq(గుర్తులేని wid)
: vvp_arith_(wid)
{
}

శూన్య vvp_cmp_weq::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                            vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      vvp_vector4_t eeq (1);
      eeq.సమితి_బిట్టు(0, BIT4_1);

      ప్రతిజ్ఞా(op_a_.size() == op_b_.size());
      కొరకు (గుర్తులేని idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);
	    ఒకవేళ (b == BIT4_X)
		  కొనసాగు;
	    లేకపోతే ఒకవేళ (b == BIT4_Z)
		  కొనసాగు;
	    లేకపోతే ఒకవేళ (a == BIT4_X)
		  eeq.సమితి_బిట్టు(0, BIT4_X);
	    లేకపోతే ఒకవేళ (a == BIT4_Z)
		  eeq.సమితి_బిట్టు(0, BIT4_X);
            లేకపోతే ఒకవేళ (a != b) {
		  eeq.సమితి_బిట్టు(0, BIT4_0);
		  ఆపు;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}

vvp_cmp_wne::vvp_cmp_wne(గుర్తులేని wid)
: vvp_arith_(wid)
{
}

శూన్య vvp_cmp_wne::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                            vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      vvp_vector4_t eeq (1);
      eeq.సమితి_బిట్టు(0, BIT4_0);

      ప్రతిజ్ఞా(op_a_.size() == op_b_.size());
      కొరకు (గుర్తులేని idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);
	    ఒకవేళ (b == BIT4_X)
		  కొనసాగు;
	    లేకపోతే ఒకవేళ (b == BIT4_Z)
		  కొనసాగు;
	    లేకపోతే ఒకవేళ (a == BIT4_X)
		  eeq.సమితి_బిట్టు(0, BIT4_X);
	    లేకపోతే ఒకవేళ (a == BIT4_Z)
		  eeq.సమితి_బిట్టు(0, BIT4_X);
            లేకపోతే ఒకవేళ (a != b) {
		  eeq.సమితి_బిట్టు(0, BIT4_1);
		  ఆపు;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}


vvp_shiftl::vvp_shiftl(గుర్తులేని wid)
: vvp_arith_(wid)
{
}

vvp_shiftl::~vvp_shiftl()
{
}

శూన్య vvp_shiftl::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                           vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      vvp_vector4_t out (op_a_.size());

      తార్కిక అధికప్రవాహం_flag;
      గుర్తులేని దీర్ఘ జరుపు = 0;
      ఒకవేళ (! vector4_గమ్యం_విలువ(op_b_, అధికప్రవాహం_flag, జరుపు)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    తిరిగి;
      }

      ఒకవేళ (అధికప్రవాహం_flag || జరుపు > out.size())
	    జరుపు = out.size();

      కొరకు (గుర్తులేని idx = 0 ;  idx < జరుపు ;  idx += 1)
	    out.సమితి_బిట్టు(idx, BIT4_0);

      కొరకు (గుర్తులేని idx = జరుపు ;  idx < out.size() ;  idx += 1)
	    out.సమితి_బిట్టు(idx, op_a_.value(idx-జరుపు));

      ptr.ptr()->send_vec4(out, 0);
}

vvp_shiftr::vvp_shiftr(గుర్తులేని wid, తార్కిక signed_flag)
: vvp_arith_(wid), signed_flag_(signed_flag)
{
}

vvp_shiftr::~vvp_shiftr()
{
}

శూన్య vvp_shiftr::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                           vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      vvp_vector4_t out (op_a_.size());

      తార్కిక అధికప్రవాహం_flag;
      గుర్తులేని దీర్ఘ జరుపు = 0;
      ఒకవేళ (! vector4_గమ్యం_విలువ(op_b_, అధికప్రవాహం_flag, జరుపు)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    తిరిగి;
      }

      ఒకవేళ (అధికప్రవాహం_flag || జరుపు > out.size())
	    జరుపు = out.size();

      కొరకు (గుర్తులేని idx = జరుపు ;  idx < out.size() ;  idx += 1)
	    out.సమితి_బిట్టు(idx-జరుపు, op_a_.value(idx));

      vvp_bit4_t pad = BIT4_0;
      ఒకవేళ (signed_flag_ && op_a_.size() > 0)
	    pad = op_a_.value(op_a_.size()-1);

      కొరకు (గుర్తులేని idx = 0 ;  idx < జరుపు ;  idx += 1)
	    out.సమితి_బిట్టు(idx+out.size()-జరుపు, pad);

      ptr.ptr()->send_vec4(out, 0);
}


vvp_arith_real_::vvp_arith_real_()
{
      op_a_ = 0.0;
      op_b_ = 0.0;
}

శూన్య vvp_arith_real_::dispatch_నిర్వాహ్యం(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు)
{
      ఎంపిక (ptr.పోర్టు()) {
	  స్థితి 0:
	    op_a_ = బిట్టు;
	    ఆపు;
	  స్థితి 1:
	    op_b_ = బిట్టు;
	    ఆపు;
	  అప్రమేయ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.పోర్టు());
	    ప్రతిజ్ఞా(0);
      }
}


/* Real multiplication. */
vvp_arith_mult_real::vvp_arith_mult_real()
{
}

vvp_arith_mult_real::~vvp_arith_mult_real()
{
}

శూన్య vvp_arith_mult_real::recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                                    vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      ద్విగుణ val = op_a_ * op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real power. */
vvp_arith_pow_real::vvp_arith_pow_real()
{
}

vvp_arith_pow_real::~vvp_arith_pow_real()
{
}

శూన్య vvp_arith_pow_real::recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                                   vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      ద్విగుణ val = ఘాతం(op_a_, op_b_);
      ptr.ptr()->send_real(val, 0);
}

/* Real division. */
vvp_arith_div_real::vvp_arith_div_real()
{
}

vvp_arith_div_real::~vvp_arith_div_real()
{
}

శూన్య vvp_arith_div_real::recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                                   vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      ద్విగుణ val = op_a_ / op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real modulus. */
vvp_arith_mod_real::vvp_arith_mod_real()
{
}

vvp_arith_mod_real::~vvp_arith_mod_real()
{
}

శూన్య vvp_arith_mod_real::recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                                   vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      ద్విగుణ val = fmod(op_a_, op_b_);
      ptr.ptr()->send_real(val, 0);
}

/* Real summation. */
vvp_arith_కూడిక_real::vvp_arith_కూడిక_real()
{
}

vvp_arith_కూడిక_real::~vvp_arith_కూడిక_real()
{
}

శూన్య vvp_arith_కూడిక_real::recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                                   vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      ద్విగుణ val = op_a_ + op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real subtraction. */
vvp_arith_sub_real::vvp_arith_sub_real()
{
}

vvp_arith_sub_real::~vvp_arith_sub_real()
{
}

శూన్య vvp_arith_sub_real::recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                                   vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      ద్విగుణ val = op_a_ - op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real compare equal. */
vvp_cmp_eq_real::vvp_cmp_eq_real()
{
}

శూన్య vvp_cmp_eq_real::recv_real(vvp_net_ptr_t ptr, స్థిరరాశి ద్విగుణ బిట్టు,
                                vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      vvp_vector4_t res (1);
      ఒకవేళ (op_a_ == op_b_) res.సమితి_బిట్టు(0, BIT4_1);
      లేకపోతే res.సమితి_బిట్టు(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}

/* Real compare not equal. */
vvp_cmp_ne_real::vvp_cmp_ne_real()
{
}

శూన్య vvp_cmp_ne_real::recv_real(vvp_net_ptr_t ptr, స్థిరరాశి ద్విగుణ బిట్టు,
                                vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      vvp_vector4_t res (1);
      ఒకవేళ (op_a_ != op_b_) res.సమితి_బిట్టు(0, BIT4_1);
      లేకపోతే res.సమితి_బిట్టు(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}

/* Real compare greater than or equal. */
vvp_cmp_ge_real::vvp_cmp_ge_real()
{
}

శూన్య vvp_cmp_ge_real::recv_real(vvp_net_ptr_t ptr, స్థిరరాశి ద్విగుణ బిట్టు,
                                vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      vvp_vector4_t res (1);
      ఒకవేళ (op_a_ >= op_b_) res.సమితి_బిట్టు(0, BIT4_1);
      లేకపోతే res.సమితి_బిట్టు(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}

/* Real compare greater than. */
vvp_cmp_gt_real::vvp_cmp_gt_real()
{
}

శూన్య vvp_cmp_gt_real::recv_real(vvp_net_ptr_t ptr, స్థిరరాశి ద్విగుణ బిట్టు,
                                vvp_context_t)
{
      dispatch_నిర్వాహ్యం(ptr, బిట్టు);

      vvp_vector4_t res (1);
      ఒకవేళ (op_a_ > op_b_) res.సమితి_బిట్టు(0, BIT4_1);
      లేకపోతే res.సమితి_బిట్టు(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}
