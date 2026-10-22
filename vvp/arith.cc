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

استعمال نامخلا std;

vvp_arith_::vvp_arith_(غیرعلامتی wid)
: wid_(wid), op_a_(wid), op_b_(wid), x_val_(wid)
{
      برائے (غیرعلامتی idx = 0 ;  idx < wid ;  idx += 1) {
	    op_a_ .سیٹ_ثنائی_ہندسہ(idx, BIT4_Z);
	    op_b_ .سیٹ_ثنائی_ہندسہ(idx, BIT4_Z);
	    x_val_.سیٹ_ثنائی_ہندسہ(idx, BIT4_X);
      }
}

خالی vvp_arith_::dispatch_عملیہ(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ)
{
      غیرعلامتی پورٹ = ptr.پورٹ();
      انتخاب (پورٹ) {
	  حالت 0:
	    op_a_ = ثنائی_ہندسہ;
	    توڑو;
	  حالت 1:
	    op_b_ = ثنائی_ہندسہ;
	    توڑو;
	  طےشدہ:
	    fprintf(stderr, "Unsupported port type %u.\n", پورٹ);
	    أكد(0);
      }
}

خالی vvp_arith_::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			      غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ثنائی_ہندسہ, بنیاد, vwid, ctx);
}

vvp_arith_abs::vvp_arith_abs()
{
}

vvp_arith_abs::~vvp_arith_abs()
{
}

خالی vvp_arith_abs::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                              vvp_context_t)
{
      vvp_vector4_t out (ثنائی_ہندسہ.size(), BIT4_0);;

      vvp_bit4_t cmp = compare_gtge_signed(ثنائی_ہندسہ, out, BIT4_1);
      انتخاب (cmp) {
	  حالت BIT4_1: // bit >= 0
	    out = ثنائی_ہندسہ;
	    توڑو;
	  حالت BIT4_0: //  bit < 0
	    out = ~ثنائی_ہندسہ;
	    out += 1;
	    توڑو;
	  طےشدہ: // There's an X.
	    out = vvp_vector4_t(ثنائی_ہندسہ.size(), BIT4_X);
	    توڑو;
      }

      ptr.ptr()->send_vec4(out, 0);
}

خالی vvp_arith_abs::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				 غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ثنائی_ہندسہ, بنیاد, vwid, ctx);
}

خالی vvp_arith_abs::recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                              vvp_context_t)
{
      دوہرا out = fabs(ثنائی_ہندسہ);
      ptr.ptr()->send_real(out, 0);
}

vvp_arith_cast_int::vvp_arith_cast_int(غیرعلامتی wid)
: wid_(wid)
{
}

vvp_arith_cast_int::~vvp_arith_cast_int()
{
}

خالی vvp_arith_cast_int::recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                                   vvp_context_t)
{
      ptr.ptr()->send_vec4(vvp_vector4_t(wid_, ثنائی_ہندسہ), 0);
}

vvp_arith_cast_real::vvp_arith_cast_real(منطقی signed_flag)
: signed_(signed_flag)
{
}

vvp_arith_cast_real::~vvp_arith_cast_real()
{
}

خالی vvp_arith_cast_real::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                    vvp_context_t)
{
      دوہرا val;
      vector4_مقصد_قدر(ثنائی_ہندسہ, val, signed_);
      ptr.ptr()->send_real(val, 0);
}

خالی vvp_arith_cast_real::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				       غیرعلامتی بنیاد, غیرعلامتی vwid,
				       vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ثنائی_ہندسہ, بنیاد, vwid, ctx);
}

vvp_arith_cast_vec2::vvp_arith_cast_vec2(غیرعلامتی wid)
: wid_(wid)
{
}

vvp_arith_cast_vec2::~vvp_arith_cast_vec2()
{
}

خالی vvp_arith_cast_vec2::recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                                   vvp_context_t)
{
      ptr.ptr()->send_vec4(vvp_vector4_t(wid_, ثنائی_ہندسہ), 0);
}

خالی vvp_arith_cast_vec2::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                    vvp_context_t)
{
      vvp_vector2_t tmp = vvp_vector2_t(ثنائی_ہندسہ);
      ptr.ptr()->send_vec4(vector2_مقصد_vector4(tmp,wid_), 0);
}

خالی vvp_arith_cast_vec2::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				       غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ثنائی_ہندسہ, بنیاد, vwid, ctx);
}

// Division

vvp_arith_div::vvp_arith_div(غیرعلامتی wid, منطقی signed_flag)
: vvp_arith_(wid), signed_flag_(signed_flag)
{
}

vvp_arith_div::~vvp_arith_div()
{
}

خالی vvp_arith_div::wide4_(vvp_net_ptr_t ptr)
{
      vvp_vector2_t a2 (op_a_, سچ);
      اگر (a2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    واپس;
      }

      vvp_vector2_t b2 (op_b_, سچ);
      اگر (b2.is_NaN() || b2.is_zero()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    واپس;
      }

      منطقی negate = جھوٹ;
      اگر (signed_flag_) {
	    اگر (a2.value(a2.size()-1)) {
		  a2 = -a2;
		  negate = سچ;
            }
	    اگر (b2.value(b2.size()-1)) {
		  b2 = -b2;
		  negate = !negate;
            }
      }
      vvp_vector2_t res = a2 / b2;
      اگر (negate) res = -res;
      ptr.ptr()->send_vec4(vector2_مقصد_vector4(res, wid_), 0);
}

خالی vvp_arith_div::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                              vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      اگر (wid_ > 8 * حجم(غیرعلامتی طویل)) {
	    wide4_(ptr);
	    واپس ;
      }

      غیرعلامتی طویل a;
      اگر (! vector4_مقصد_قدر(op_a_, a)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    واپس;
      }

      غیرعلامتی طویل b;
      اگر (! vector4_مقصد_قدر(op_b_, b)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    واپس;
      }

      منطقی negate = جھوٹ;
	/* If we are doing signed divide, then take the sign out of
	   the operands for now, and remember to put the sign back
	   later. */
      اگر (signed_flag_) {
	    غیرعلامتی طویل sign_نقاب = 0;
	    اگر (op_a_.size() != 8 * حجم(غیرعلامتی طویل)) {
		  sign_نقاب = -1UL << op_a_.size();
	    }
	    اگر (op_a_.value(op_a_.size()-1)) {
		  a = (-a) & ~sign_نقاب;
		  negate = !negate;
	    }

	    sign_نقاب = 0;
	    اگر (op_b_.size() != 8 * حجم(غیرعلامتی طویل)) {
		  sign_نقاب = -1UL << op_b_.size();
	    }
	    اگر (op_b_.value(op_b_.size()-1)) {
		  b = (-b) & ~sign_نقاب;
		  negate = ! negate;
	    }
      }

      اگر (b == 0) {
	    vvp_vector4_t xval (wid_);
	    برائے (غیرعلامتی idx = 0 ;  idx < wid_ ;  idx += 1)
		  xval.سیٹ_ثنائی_ہندسہ(idx, BIT4_X);

	    ptr.ptr()->send_vec4(xval, 0);
	    واپس;
      }

      غیرعلامتی طویل val = a / b;
      اگر (negate)
	    val = -val;

      أكد(wid_ <= 8*حجم(val));

      vvp_vector4_t vval (wid_);
      برائے (غیرعلامتی idx = 0 ;  idx < wid_ ;  idx += 1) {
	    اگر (val & 1)
		  vval.سیٹ_ثنائی_ہندسہ(idx, BIT4_1);
	    ورنہ
		  vval.سیٹ_ثنائی_ہندسہ(idx, BIT4_0);

	    val >>= 1;
      }

      ptr.ptr()->send_vec4(vval, 0);
}


vvp_arith_mod::vvp_arith_mod(غیرعلامتی wid, منطقی sf)
: vvp_arith_(wid), signed_flag_(sf)
{
}

vvp_arith_mod::~vvp_arith_mod()
{
}

خالی vvp_arith_mod::wide_(vvp_net_ptr_t ptr)
{
      vvp_vector2_t a2 (op_a_, سچ);
      اگر (a2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    واپس;
      }

      vvp_vector2_t b2 (op_b_, سچ);
      اگر (b2.is_NaN() || b2.is_zero()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    واپس;
      }

      منطقی negate = جھوٹ;
      اگر (signed_flag_) {
	    اگر (a2.value(a2.size()-1)) {
		  a2 = -a2;
		  negate = سچ;
            }
	    اگر (b2.value(b2.size()-1)) {
		  b2 = -b2;
            }
      }
      vvp_vector2_t res = a2 % b2;
      اگر (negate) res = -res;
      ptr.ptr()->send_vec4(vector2_مقصد_vector4(res, res.size()), 0);
}

خالی vvp_arith_mod::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                              vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      اگر (wid_ > 8 * حجم(غیرعلامتی طویل)) {
	    wide_(ptr);
	    واپس ;
      }

      غیرعلامتی طویل a;
      اگر (! vector4_مقصد_قدر(op_a_, a)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    واپس;
      }

      غیرعلامتی طویل b;
      اگر (! vector4_مقصد_قدر(op_b_, b)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    واپس;
      }

      منطقی negate = جھوٹ;
	/* If we are doing signed divide, then take the sign out of
	   the operands for now, and remember to put the sign back
	   later. */
      اگر (signed_flag_) {
	    غیرعلامتی طویل sign_نقاب = 0;
	    اگر (op_a_.size() != 8 * حجم(غیرعلامتی طویل)) {
		  sign_نقاب = -1UL << op_a_.size();
	    }
	    اگر (op_a_.value(op_a_.size()-1)) {
		  a = (-a) & ~sign_نقاب;
		  negate = !negate;
	    }

	    sign_نقاب = 0;
	    اگر (op_b_.size() != 8 * حجم(غیرعلامتی طویل)) {
		  sign_نقاب = -1UL << op_b_.size();
	    }
	    اگر (op_b_.value(op_b_.size()-1)) {
		  b = (-b) & ~sign_نقاب;
	    }
      }

      اگر (b == 0) {
	    vvp_vector4_t xval (wid_);
	    برائے (غیرعلامتی idx = 0 ;  idx < wid_ ;  idx += 1)
		  xval.سیٹ_ثنائی_ہندسہ(idx, BIT4_X);

	    ptr.ptr()->send_vec4(xval, 0);
	    واپس;
      }

      غیرعلامتی طویل val = a % b;
      اگر (negate)
	    val = -val;

      أكد(wid_ <= 8*حجم(val));

      vvp_vector4_t vval (wid_);
      برائے (غیرعلامتی idx = 0 ;  idx < wid_ ;  idx += 1) {
	    اگر (val & 1)
		  vval.سیٹ_ثنائی_ہندسہ(idx, BIT4_1);
	    ورنہ
		  vval.سیٹ_ثنائی_ہندسہ(idx, BIT4_0);

	    val >>= 1;
      }

      ptr.ptr()->send_vec4(vval, 0);
}


// Multiplication

vvp_arith_mult::vvp_arith_mult(غیرعلامتی wid)
: vvp_arith_(wid)
{
}

vvp_arith_mult::~vvp_arith_mult()
{
}

خالی vvp_arith_mult::wide_(vvp_net_ptr_t ptr)
{
      vvp_vector2_t a2 (op_a_, سچ);
      vvp_vector2_t b2 (op_b_, سچ);

      اگر (a2.is_NaN() || b2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    واپس;
      }

      vvp_vector2_t نتیجہ = a2 * b2;

      vvp_vector4_t res4 = vector2_مقصد_vector4(نتیجہ, wid_);
      ptr.ptr()->send_vec4(res4, 0);
}

خالی vvp_arith_mult::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                               vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      اگر (wid_ > 8 * حجم(int64_t)) {
	    wide_(ptr);
	    واپس ;
      }

      int64_t a;
      اگر (! vector4_مقصد_قدر(op_a_, a, جھوٹ, سچ)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    واپس;
      }

      int64_t b;
      اگر (! vector4_مقصد_قدر(op_b_, b, جھوٹ, سچ)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    واپس;
      }

      int64_t val = a * b;
      أكد(wid_ <= 8*حجم(val));

      vvp_vector4_t vval (wid_);
      برائے (غیرعلامتی idx = 0 ;  idx < wid_ ;  idx += 1) {
	    اگر (val & 1)
		  vval.سیٹ_ثنائی_ہندسہ(idx, BIT4_1);
	    ورنہ
		  vval.سیٹ_ثنائی_ہندسہ(idx, BIT4_0);

	    val >>= 1;
      }

      ptr.ptr()->send_vec4(vval, 0);
}


// Power

vvp_arith_pow::vvp_arith_pow(غیرعلامتی wid, منطقی signed_flag)
: vvp_arith_(wid), signed_flag_(signed_flag)
{
}

vvp_arith_pow::~vvp_arith_pow()
{
}

خالی vvp_arith_pow::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                              vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      vvp_vector2_t a2 (op_a_, سچ);
      vvp_vector2_t b2 (op_b_, سچ);

        // If we have an X or Z in the arguments return X.
      اگر (a2.is_NaN() || b2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    واپس;
      }

	// Is the exponent negative? If so, table 5-6 in IEEE1364-2005
	// defines what value is returned.
      اگر (signed_flag_ && b2.value(b2.size()-1)) {
	    صحیح a_val;
	    دوہرا r_val = 0.0;
	    اگر (vector2_مقصد_قدر(a2, a_val, سچ)) {
		  اگر (a_val == 0) {
			ptr.ptr()->send_vec4(x_val_, 0);
			واپس;
		  }
		  اگر (a_val == 1) {
			r_val = 1.0;
		  }
		  اگر (a_val == -1) {
			r_val = b2.value(0) ? -1.0 : 1.0;
		  }
	    }
	    ptr.ptr()->send_vec4(vvp_vector4_t(wid_, r_val), 0);
	    واپس;
      }

      ptr.ptr()->send_vec4(vector2_مقصد_vector4(قوت(a2, b2), wid_), 0);
}


// Addition

vvp_arith_مجموع::vvp_arith_مجموع(غیرعلامتی wid)
: vvp_arith_(wid)
{
}

vvp_arith_مجموع::~vvp_arith_مجموع()
{
}

خالی vvp_arith_مجموع::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                              vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      vvp_net_t*net = ptr.ptr();

      vvp_vector4_t value (wid_);

	/* Pad input vectors with this value to widen to the desired
	   output width. */
      ثابت vvp_bit4_t pad = BIT4_0;

      vvp_bit4_t حاصل = BIT4_0;
      برائے (غیرعلامتی idx = 0 ;  idx < wid_ ;  idx += 1) {
	    vvp_bit4_t a = (idx >= op_a_.size())? pad : op_a_.value(idx);
	    vvp_bit4_t b = (idx >= op_b_.size())? pad : op_b_.value(idx);
	    vvp_bit4_t cur = جوڑو_with_حاصل(a, b, حاصل);

	    اگر (cur == BIT4_X) {
		  net->send_vec4(x_val_, 0);
		  واپس;
	    }

	    value.سیٹ_ثنائی_ہندسہ(idx, cur);
      }

      net->send_vec4(value, 0);
}

vvp_arith_sub::vvp_arith_sub(غیرعلامتی wid)
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
خالی vvp_arith_sub::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                              vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      vvp_net_t*net = ptr.ptr();

      vvp_vector4_t value (wid_);

	/* Pad input vectors with this value to widen to the desired
	   output width. */
      ثابت vvp_bit4_t pad = BIT4_1;

      vvp_bit4_t حاصل = BIT4_1;
      برائے (غیرعلامتی idx = 0 ;  idx < wid_ ;  idx += 1) {
	    vvp_bit4_t a = (idx >= op_a_.size())? pad : op_a_.value(idx);
	    vvp_bit4_t b = (idx >= op_b_.size())? pad : ~op_b_.value(idx);
	    vvp_bit4_t cur = جوڑو_with_حاصل(a, b, حاصل);

	    اگر (cur == BIT4_X) {
		  net->send_vec4(x_val_, 0);
		  واپس;
	    }

	    value.سیٹ_ثنائی_ہندسہ(idx, cur);
      }

      net->send_vec4(value, 0);
}

vvp_cmp_eeq::vvp_cmp_eeq(غیرعلامتی wid)
: vvp_arith_(wid)
{
}

خالی vvp_cmp_eeq::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                            vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      vvp_vector4_t eeq (1);
      eeq.سیٹ_ثنائی_ہندسہ(0, BIT4_1);

      أكد(op_a_.size() == op_b_.size());
      برائے (غیرعلامتی idx = 0 ;  idx < op_a_.size() ;  idx += 1)
	    اگر (op_a_.value(idx) != op_b_.value(idx)) {
		  eeq.سیٹ_ثنائی_ہندسہ(0, BIT4_0);
		  توڑو;
	    }


      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}

vvp_cmp_nee::vvp_cmp_nee(غیرعلامتی wid)
: vvp_arith_(wid)
{
}

خالی vvp_cmp_nee::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                            vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      vvp_vector4_t eeq (1);
      eeq.سیٹ_ثنائی_ہندسہ(0, BIT4_0);

      أكد(op_a_.size() == op_b_.size());
      برائے (غیرعلامتی idx = 0 ;  idx < op_a_.size() ;  idx += 1)
	    اگر (op_a_.value(idx) != op_b_.value(idx)) {
		  eeq.سیٹ_ثنائی_ہندسہ(0, BIT4_1);
		  توڑو;
	    }


      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}

vvp_cmp_eq::vvp_cmp_eq(غیرعلامتی wid)
: vvp_arith_(wid)
{
}

/*
 * Compare Vector a and Vector b. If in any bit position the a and b
 * bits are known and different, then the result is 0. Otherwise, if
 * there are X/Z bits anywhere in A or B, the result is X. Finally,
 * the result is 1.
 */
خالی vvp_cmp_eq::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                           vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      اگر (op_a_.size() != op_b_.size()) {
	    cerr << "COMPARISON size mismatch. "
		 << "a=" << op_a_ << ", b=" << op_b_ << سطر_ختم;
	    أكد(0);
      }

      vvp_vector4_t res (1);
      res.سیٹ_ثنائی_ہندسہ(0, BIT4_1);

      برائے (غیرعلامتی idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    اگر (a == BIT4_X)
		  res.سیٹ_ثنائی_ہندسہ(0, BIT4_X);
	    ورنہ اگر (a == BIT4_Z)
		  res.سیٹ_ثنائی_ہندسہ(0, BIT4_X);
	    ورنہ اگر (b == BIT4_X)
		  res.سیٹ_ثنائی_ہندسہ(0, BIT4_X);
	    ورنہ اگر (b == BIT4_Z)
		  res.سیٹ_ثنائی_ہندسہ(0, BIT4_X);
            ورنہ اگر (a != b) {
		  res.سیٹ_ثنائی_ہندسہ(0, BIT4_0);
		  توڑو;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}

vvp_cmp_eqx::vvp_cmp_eqx(غیرعلامتی wid)
: vvp_arith_(wid)
{
}

/*
 * Compare Vector a and Vector b. If in any bit position the a and b
 * bits are known and different, then the result is 0. Otherwise, if
 * there are X/Z bits anywhere in A or B, the result is X. Finally,
 * the result is 1.
 */
خالی vvp_cmp_eqx::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                           vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      اگر (op_a_.size() != op_b_.size()) {
	    cerr << "COMPARISON size mismatch. "
		 << "a=" << op_a_ << ", b=" << op_b_ << سطر_ختم;
	    أكد(0);
      }

      vvp_vector4_t res (1);
      res.سیٹ_ثنائی_ہندسہ(0, BIT4_1);

      برائے (غیرعلامتی idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    اگر (b == BIT4_X)
		  جاری;
	    اگر (b == BIT4_Z)
		  جاری;
	    اگر (a != b) {
		  res.سیٹ_ثنائی_ہندسہ(0, BIT4_0);
		  توڑو;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}

vvp_cmp_eqz::vvp_cmp_eqz(غیرعلامتی wid)
: vvp_arith_(wid)
{
}

خالی vvp_cmp_eqz::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                           vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      اگر (op_a_.size() != op_b_.size()) {
	    cerr << "COMPARISON size mismatch. "
		 << "a=" << op_a_ << ", b=" << op_b_ << سطر_ختم;
	    أكد(0);
      }

      vvp_vector4_t res (1);
      res.سیٹ_ثنائی_ہندسہ(0, BIT4_1);

      برائے (غیرعلامتی idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    اگر (b == BIT4_Z)
		  جاری;
	    اگر (a != b) {
		  res.سیٹ_ثنائی_ہندسہ(0, BIT4_0);
		  توڑو;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}

vvp_cmp_ne::vvp_cmp_ne(غیرعلامتی wid)
: vvp_arith_(wid)
{
}

/*
 * Compare Vector a and Vector b. If in any bit position the a and b
 * bits are known and different, then the result is 1. Otherwise, if
 * there are X/Z bits anywhere in A or B, the result is X. Finally,
 * the result is 0.
 */
خالی vvp_cmp_ne::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                           vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      اگر (op_a_.size() != op_b_.size()) {
	    cerr << "internal error: vvp_cmp_ne: op_a_=" << op_a_
		 << ", op_b_=" << op_b_ << سطر_ختم;
	    أكد(op_a_.size() == op_b_.size());
      }

      vvp_vector4_t res (1);
      res.سیٹ_ثنائی_ہندسہ(0, BIT4_0);

      برائے (غیرعلامتی idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    اگر (a == BIT4_X)
		  res.سیٹ_ثنائی_ہندسہ(0, BIT4_X);
	    ورنہ اگر (a == BIT4_Z)
		  res.سیٹ_ثنائی_ہندسہ(0, BIT4_X);
	    ورنہ اگر (b == BIT4_X)
		  res.سیٹ_ثنائی_ہندسہ(0, BIT4_X);
	    ورنہ اگر (b == BIT4_Z)
		  res.سیٹ_ثنائی_ہندسہ(0, BIT4_X);
            ورنہ اگر (a != b) {
		  res.سیٹ_ثنائی_ہندسہ(0, BIT4_1);
		  توڑو;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}


vvp_cmp_gtge_بنیاد::vvp_cmp_gtge_بنیاد(غیرعلامتی wid, منطقی flag)
: vvp_arith_(wid), signed_flag_(flag)
{
}


خالی vvp_cmp_gtge_بنیاد::recv_vec4_بنیاد(vvp_net_ptr_t ptr,
					 ثابت vvp_vector4_t&ثنائی_ہندسہ,
					 vvp_bit4_t برآمد_if_equal)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      vvp_bit4_t out = signed_flag_
	    ? compare_gtge_signed(op_a_, op_b_, برآمد_if_equal)
	    : compare_gtge(op_a_, op_b_, برآمد_if_equal);
      vvp_vector4_t val (1);
      val.سیٹ_ثنائی_ہندسہ(0, out);
      ptr.ptr()->send_vec4(val, 0);

      واپس;
}


vvp_cmp_ge::vvp_cmp_ge(غیرعلامتی wid, منطقی flag)
: vvp_cmp_gtge_بنیاد(wid, flag)
{
}

خالی vvp_cmp_ge::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                           vvp_context_t)
{
      recv_vec4_بنیاد(ptr, ثنائی_ہندسہ, BIT4_1);
}

vvp_cmp_gt::vvp_cmp_gt(غیرعلامتی wid, منطقی flag)
: vvp_cmp_gtge_بنیاد(wid, flag)
{
}

خالی vvp_cmp_gt::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                           vvp_context_t)
{
      recv_vec4_بنیاد(ptr, ثنائی_ہندسہ, BIT4_0);
}

vvp_cmp_weq::vvp_cmp_weq(غیرعلامتی wid)
: vvp_arith_(wid)
{
}

خالی vvp_cmp_weq::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                            vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      vvp_vector4_t eeq (1);
      eeq.سیٹ_ثنائی_ہندسہ(0, BIT4_1);

      أكد(op_a_.size() == op_b_.size());
      برائے (غیرعلامتی idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);
	    اگر (b == BIT4_X)
		  جاری;
	    ورنہ اگر (b == BIT4_Z)
		  جاری;
	    ورنہ اگر (a == BIT4_X)
		  eeq.سیٹ_ثنائی_ہندسہ(0, BIT4_X);
	    ورنہ اگر (a == BIT4_Z)
		  eeq.سیٹ_ثنائی_ہندسہ(0, BIT4_X);
            ورنہ اگر (a != b) {
		  eeq.سیٹ_ثنائی_ہندسہ(0, BIT4_0);
		  توڑو;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}

vvp_cmp_wne::vvp_cmp_wne(غیرعلامتی wid)
: vvp_arith_(wid)
{
}

خالی vvp_cmp_wne::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                            vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      vvp_vector4_t eeq (1);
      eeq.سیٹ_ثنائی_ہندسہ(0, BIT4_0);

      أكد(op_a_.size() == op_b_.size());
      برائے (غیرعلامتی idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);
	    اگر (b == BIT4_X)
		  جاری;
	    ورنہ اگر (b == BIT4_Z)
		  جاری;
	    ورنہ اگر (a == BIT4_X)
		  eeq.سیٹ_ثنائی_ہندسہ(0, BIT4_X);
	    ورنہ اگر (a == BIT4_Z)
		  eeq.سیٹ_ثنائی_ہندسہ(0, BIT4_X);
            ورنہ اگر (a != b) {
		  eeq.سیٹ_ثنائی_ہندسہ(0, BIT4_1);
		  توڑو;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}


vvp_shiftl::vvp_shiftl(غیرعلامتی wid)
: vvp_arith_(wid)
{
}

vvp_shiftl::~vvp_shiftl()
{
}

خالی vvp_shiftl::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                           vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      vvp_vector4_t out (op_a_.size());

      منطقی حد_سے_تجاوز_flag;
      غیرعلامتی طویل سرکاؤ = 0;
      اگر (! vector4_مقصد_قدر(op_b_, حد_سے_تجاوز_flag, سرکاؤ)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    واپس;
      }

      اگر (حد_سے_تجاوز_flag || سرکاؤ > out.size())
	    سرکاؤ = out.size();

      برائے (غیرعلامتی idx = 0 ;  idx < سرکاؤ ;  idx += 1)
	    out.سیٹ_ثنائی_ہندسہ(idx, BIT4_0);

      برائے (غیرعلامتی idx = سرکاؤ ;  idx < out.size() ;  idx += 1)
	    out.سیٹ_ثنائی_ہندسہ(idx, op_a_.value(idx-سرکاؤ));

      ptr.ptr()->send_vec4(out, 0);
}

vvp_shiftr::vvp_shiftr(غیرعلامتی wid, منطقی signed_flag)
: vvp_arith_(wid), signed_flag_(signed_flag)
{
}

vvp_shiftr::~vvp_shiftr()
{
}

خالی vvp_shiftr::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                           vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      vvp_vector4_t out (op_a_.size());

      منطقی حد_سے_تجاوز_flag;
      غیرعلامتی طویل سرکاؤ = 0;
      اگر (! vector4_مقصد_قدر(op_b_, حد_سے_تجاوز_flag, سرکاؤ)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    واپس;
      }

      اگر (حد_سے_تجاوز_flag || سرکاؤ > out.size())
	    سرکاؤ = out.size();

      برائے (غیرعلامتی idx = سرکاؤ ;  idx < out.size() ;  idx += 1)
	    out.سیٹ_ثنائی_ہندسہ(idx-سرکاؤ, op_a_.value(idx));

      vvp_bit4_t pad = BIT4_0;
      اگر (signed_flag_ && op_a_.size() > 0)
	    pad = op_a_.value(op_a_.size()-1);

      برائے (غیرعلامتی idx = 0 ;  idx < سرکاؤ ;  idx += 1)
	    out.سیٹ_ثنائی_ہندسہ(idx+out.size()-سرکاؤ, pad);

      ptr.ptr()->send_vec4(out, 0);
}


vvp_arith_real_::vvp_arith_real_()
{
      op_a_ = 0.0;
      op_b_ = 0.0;
}

خالی vvp_arith_real_::dispatch_عملیہ(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ)
{
      انتخاب (ptr.پورٹ()) {
	  حالت 0:
	    op_a_ = ثنائی_ہندسہ;
	    توڑو;
	  حالت 1:
	    op_b_ = ثنائی_ہندسہ;
	    توڑو;
	  طےشدہ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.پورٹ());
	    أكد(0);
      }
}


/* Real multiplication. */
vvp_arith_mult_real::vvp_arith_mult_real()
{
}

vvp_arith_mult_real::~vvp_arith_mult_real()
{
}

خالی vvp_arith_mult_real::recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                                    vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      دوہرا val = op_a_ * op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real power. */
vvp_arith_pow_real::vvp_arith_pow_real()
{
}

vvp_arith_pow_real::~vvp_arith_pow_real()
{
}

خالی vvp_arith_pow_real::recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                                   vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      دوہرا val = قوت(op_a_, op_b_);
      ptr.ptr()->send_real(val, 0);
}

/* Real division. */
vvp_arith_div_real::vvp_arith_div_real()
{
}

vvp_arith_div_real::~vvp_arith_div_real()
{
}

خالی vvp_arith_div_real::recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                                   vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      دوہرا val = op_a_ / op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real modulus. */
vvp_arith_mod_real::vvp_arith_mod_real()
{
}

vvp_arith_mod_real::~vvp_arith_mod_real()
{
}

خالی vvp_arith_mod_real::recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                                   vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      دوہرا val = fmod(op_a_, op_b_);
      ptr.ptr()->send_real(val, 0);
}

/* Real summation. */
vvp_arith_مجموع_real::vvp_arith_مجموع_real()
{
}

vvp_arith_مجموع_real::~vvp_arith_مجموع_real()
{
}

خالی vvp_arith_مجموع_real::recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                                   vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      دوہرا val = op_a_ + op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real subtraction. */
vvp_arith_sub_real::vvp_arith_sub_real()
{
}

vvp_arith_sub_real::~vvp_arith_sub_real()
{
}

خالی vvp_arith_sub_real::recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                                   vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      دوہرا val = op_a_ - op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real compare equal. */
vvp_cmp_eq_real::vvp_cmp_eq_real()
{
}

خالی vvp_cmp_eq_real::recv_real(vvp_net_ptr_t ptr, ثابت دوہرا ثنائی_ہندسہ,
                                vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      vvp_vector4_t res (1);
      اگر (op_a_ == op_b_) res.سیٹ_ثنائی_ہندسہ(0, BIT4_1);
      ورنہ res.سیٹ_ثنائی_ہندسہ(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}

/* Real compare not equal. */
vvp_cmp_ne_real::vvp_cmp_ne_real()
{
}

خالی vvp_cmp_ne_real::recv_real(vvp_net_ptr_t ptr, ثابت دوہرا ثنائی_ہندسہ,
                                vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      vvp_vector4_t res (1);
      اگر (op_a_ != op_b_) res.سیٹ_ثنائی_ہندسہ(0, BIT4_1);
      ورنہ res.سیٹ_ثنائی_ہندسہ(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}

/* Real compare greater than or equal. */
vvp_cmp_ge_real::vvp_cmp_ge_real()
{
}

خالی vvp_cmp_ge_real::recv_real(vvp_net_ptr_t ptr, ثابت دوہرا ثنائی_ہندسہ,
                                vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      vvp_vector4_t res (1);
      اگر (op_a_ >= op_b_) res.سیٹ_ثنائی_ہندسہ(0, BIT4_1);
      ورنہ res.سیٹ_ثنائی_ہندسہ(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}

/* Real compare greater than. */
vvp_cmp_gt_real::vvp_cmp_gt_real()
{
}

خالی vvp_cmp_gt_real::recv_real(vvp_net_ptr_t ptr, ثابت دوہرا ثنائی_ہندسہ,
                                vvp_context_t)
{
      dispatch_عملیہ(ptr, ثنائی_ہندسہ);

      vvp_vector4_t res (1);
      اگر (op_a_ > op_b_) res.سیٹ_ثنائی_ہندسہ(0, BIT4_1);
      ورنہ res.سیٹ_ثنائی_ہندسہ(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}
