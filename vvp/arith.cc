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

استفاده نطاق std;

vvp_arith_::vvp_arith_(بیعلامت wid)
: wid_(wid), op_a_(wid), op_b_(wid), x_val_(wid)
{
      لكل (بیعلامت idx = 0 ;  idx < wid ;  idx += 1) {
	    op_a_ .مجموعه_بیت(idx, BIT4_Z);
	    op_b_ .مجموعه_بیت(idx, BIT4_Z);
	    x_val_.مجموعه_بیت(idx, BIT4_X);
      }
}

فراغ vvp_arith_::dispatch_عملوند(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت)
{
      بیعلامت پورت = ptr.پورت();
      بدل (پورت) {
	  حالة 0:
	    op_a_ = بیت;
	    اكسر;
	  حالة 1:
	    op_b_ = بیت;
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", پورت);
	    أكد(0);
      }
}

فراغ vvp_arith_::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
			      بیعلامت پایه, بیعلامت vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, بیت, پایه, vwid, ctx);
}

vvp_arith_abs::vvp_arith_abs()
{
}

vvp_arith_abs::~vvp_arith_abs()
{
}

فراغ vvp_arith_abs::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                              vvp_context_t)
{
      vvp_vector4_t out (بیت.size(), BIT4_0);;

      vvp_bit4_t cmp = compare_gtge_signed(بیت, out, BIT4_1);
      بدل (cmp) {
	  حالة BIT4_1: // bit >= 0
	    out = بیت;
	    اكسر;
	  حالة BIT4_0: //  bit < 0
	    out = ~بیت;
	    out += 1;
	    اكسر;
	  افتراضي: // There's an X.
	    out = vvp_vector4_t(بیت.size(), BIT4_X);
	    اكسر;
      }

      ptr.ptr()->send_vec4(out, 0);
}

فراغ vvp_arith_abs::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
				 بیعلامت پایه, بیعلامت vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, بیت, پایه, vwid, ctx);
}

فراغ vvp_arith_abs::recv_real(vvp_net_ptr_t ptr, مزدوج بیت,
                              vvp_context_t)
{
      مزدوج out = fabs(بیت);
      ptr.ptr()->send_real(out, 0);
}

vvp_arith_cast_int::vvp_arith_cast_int(بیعلامت wid)
: wid_(wid)
{
}

vvp_arith_cast_int::~vvp_arith_cast_int()
{
}

فراغ vvp_arith_cast_int::recv_real(vvp_net_ptr_t ptr, مزدوج بیت,
                                   vvp_context_t)
{
      ptr.ptr()->send_vec4(vvp_vector4_t(wid_, بیت), 0);
}

vvp_arith_cast_real::vvp_arith_cast_real(منطقي signed_flag)
: signed_(signed_flag)
{
}

vvp_arith_cast_real::~vvp_arith_cast_real()
{
}

فراغ vvp_arith_cast_real::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                                    vvp_context_t)
{
      مزدوج val;
      vector4_مقصد_مقدار(بیت, val, signed_);
      ptr.ptr()->send_real(val, 0);
}

فراغ vvp_arith_cast_real::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
				       بیعلامت پایه, بیعلامت vwid,
				       vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, بیت, پایه, vwid, ctx);
}

vvp_arith_cast_vec2::vvp_arith_cast_vec2(بیعلامت wid)
: wid_(wid)
{
}

vvp_arith_cast_vec2::~vvp_arith_cast_vec2()
{
}

فراغ vvp_arith_cast_vec2::recv_real(vvp_net_ptr_t ptr, مزدوج بیت,
                                   vvp_context_t)
{
      ptr.ptr()->send_vec4(vvp_vector4_t(wid_, بیت), 0);
}

فراغ vvp_arith_cast_vec2::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                                    vvp_context_t)
{
      vvp_vector2_t tmp = vvp_vector2_t(بیت);
      ptr.ptr()->send_vec4(vector2_مقصد_vector4(tmp,wid_), 0);
}

فراغ vvp_arith_cast_vec2::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
				       بیعلامت پایه, بیعلامت vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, بیت, پایه, vwid, ctx);
}

// Division

vvp_arith_div::vvp_arith_div(بیعلامت wid, منطقي signed_flag)
: vvp_arith_(wid), signed_flag_(signed_flag)
{
}

vvp_arith_div::~vvp_arith_div()
{
}

فراغ vvp_arith_div::wide4_(vvp_net_ptr_t ptr)
{
      vvp_vector2_t a2 (op_a_, درست);
      إذا (a2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    أرجع;
      }

      vvp_vector2_t b2 (op_b_, درست);
      إذا (b2.is_NaN() || b2.is_zero()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    أرجع;
      }

      منطقي negate = خطأ;
      إذا (signed_flag_) {
	    إذا (a2.value(a2.size()-1)) {
		  a2 = -a2;
		  negate = درست;
            }
	    إذا (b2.value(b2.size()-1)) {
		  b2 = -b2;
		  negate = !negate;
            }
      }
      vvp_vector2_t res = a2 / b2;
      إذا (negate) res = -res;
      ptr.ptr()->send_vec4(vector2_مقصد_vector4(res, wid_), 0);
}

فراغ vvp_arith_div::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                              vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      إذا (wid_ > 8 * حجم(بیعلامت دراز)) {
	    wide4_(ptr);
	    أرجع ;
      }

      بیعلامت دراز a;
      إذا (! vector4_مقصد_مقدار(op_a_, a)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    أرجع;
      }

      بیعلامت دراز b;
      إذا (! vector4_مقصد_مقدار(op_b_, b)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    أرجع;
      }

      منطقي negate = خطأ;
	/* If we are doing signed divide, then take the sign out of
	   the operands for now, and remember to put the sign back
	   later. */
      إذا (signed_flag_) {
	    بیعلامت دراز sign_ماسک = 0;
	    إذا (op_a_.size() != 8 * حجم(بیعلامت دراز)) {
		  sign_ماسک = -1UL << op_a_.size();
	    }
	    إذا (op_a_.value(op_a_.size()-1)) {
		  a = (-a) & ~sign_ماسک;
		  negate = !negate;
	    }

	    sign_ماسک = 0;
	    إذا (op_b_.size() != 8 * حجم(بیعلامت دراز)) {
		  sign_ماسک = -1UL << op_b_.size();
	    }
	    إذا (op_b_.value(op_b_.size()-1)) {
		  b = (-b) & ~sign_ماسک;
		  negate = ! negate;
	    }
      }

      إذا (b == 0) {
	    vvp_vector4_t xval (wid_);
	    لكل (بیعلامت idx = 0 ;  idx < wid_ ;  idx += 1)
		  xval.مجموعه_بیت(idx, BIT4_X);

	    ptr.ptr()->send_vec4(xval, 0);
	    أرجع;
      }

      بیعلامت دراز val = a / b;
      إذا (negate)
	    val = -val;

      أكد(wid_ <= 8*حجم(val));

      vvp_vector4_t vval (wid_);
      لكل (بیعلامت idx = 0 ;  idx < wid_ ;  idx += 1) {
	    إذا (val & 1)
		  vval.مجموعه_بیت(idx, BIT4_1);
	    وإلا
		  vval.مجموعه_بیت(idx, BIT4_0);

	    val >>= 1;
      }

      ptr.ptr()->send_vec4(vval, 0);
}


vvp_arith_mod::vvp_arith_mod(بیعلامت wid, منطقي sf)
: vvp_arith_(wid), signed_flag_(sf)
{
}

vvp_arith_mod::~vvp_arith_mod()
{
}

فراغ vvp_arith_mod::wide_(vvp_net_ptr_t ptr)
{
      vvp_vector2_t a2 (op_a_, درست);
      إذا (a2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    أرجع;
      }

      vvp_vector2_t b2 (op_b_, درست);
      إذا (b2.is_NaN() || b2.is_zero()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    أرجع;
      }

      منطقي negate = خطأ;
      إذا (signed_flag_) {
	    إذا (a2.value(a2.size()-1)) {
		  a2 = -a2;
		  negate = درست;
            }
	    إذا (b2.value(b2.size()-1)) {
		  b2 = -b2;
            }
      }
      vvp_vector2_t res = a2 % b2;
      إذا (negate) res = -res;
      ptr.ptr()->send_vec4(vector2_مقصد_vector4(res, res.size()), 0);
}

فراغ vvp_arith_mod::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                              vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      إذا (wid_ > 8 * حجم(بیعلامت دراز)) {
	    wide_(ptr);
	    أرجع ;
      }

      بیعلامت دراز a;
      إذا (! vector4_مقصد_مقدار(op_a_, a)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    أرجع;
      }

      بیعلامت دراز b;
      إذا (! vector4_مقصد_مقدار(op_b_, b)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    أرجع;
      }

      منطقي negate = خطأ;
	/* If we are doing signed divide, then take the sign out of
	   the operands for now, and remember to put the sign back
	   later. */
      إذا (signed_flag_) {
	    بیعلامت دراز sign_ماسک = 0;
	    إذا (op_a_.size() != 8 * حجم(بیعلامت دراز)) {
		  sign_ماسک = -1UL << op_a_.size();
	    }
	    إذا (op_a_.value(op_a_.size()-1)) {
		  a = (-a) & ~sign_ماسک;
		  negate = !negate;
	    }

	    sign_ماسک = 0;
	    إذا (op_b_.size() != 8 * حجم(بیعلامت دراز)) {
		  sign_ماسک = -1UL << op_b_.size();
	    }
	    إذا (op_b_.value(op_b_.size()-1)) {
		  b = (-b) & ~sign_ماسک;
	    }
      }

      إذا (b == 0) {
	    vvp_vector4_t xval (wid_);
	    لكل (بیعلامت idx = 0 ;  idx < wid_ ;  idx += 1)
		  xval.مجموعه_بیت(idx, BIT4_X);

	    ptr.ptr()->send_vec4(xval, 0);
	    أرجع;
      }

      بیعلامت دراز val = a % b;
      إذا (negate)
	    val = -val;

      أكد(wid_ <= 8*حجم(val));

      vvp_vector4_t vval (wid_);
      لكل (بیعلامت idx = 0 ;  idx < wid_ ;  idx += 1) {
	    إذا (val & 1)
		  vval.مجموعه_بیت(idx, BIT4_1);
	    وإلا
		  vval.مجموعه_بیت(idx, BIT4_0);

	    val >>= 1;
      }

      ptr.ptr()->send_vec4(vval, 0);
}


// Multiplication

vvp_arith_mult::vvp_arith_mult(بیعلامت wid)
: vvp_arith_(wid)
{
}

vvp_arith_mult::~vvp_arith_mult()
{
}

فراغ vvp_arith_mult::wide_(vvp_net_ptr_t ptr)
{
      vvp_vector2_t a2 (op_a_, درست);
      vvp_vector2_t b2 (op_b_, درست);

      إذا (a2.is_NaN() || b2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    أرجع;
      }

      vvp_vector2_t نتیجه = a2 * b2;

      vvp_vector4_t res4 = vector2_مقصد_vector4(نتیجه, wid_);
      ptr.ptr()->send_vec4(res4, 0);
}

فراغ vvp_arith_mult::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                               vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      إذا (wid_ > 8 * حجم(int64_t)) {
	    wide_(ptr);
	    أرجع ;
      }

      int64_t a;
      إذا (! vector4_مقصد_مقدار(op_a_, a, خطأ, درست)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    أرجع;
      }

      int64_t b;
      إذا (! vector4_مقصد_مقدار(op_b_, b, خطأ, درست)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    أرجع;
      }

      int64_t val = a * b;
      أكد(wid_ <= 8*حجم(val));

      vvp_vector4_t vval (wid_);
      لكل (بیعلامت idx = 0 ;  idx < wid_ ;  idx += 1) {
	    إذا (val & 1)
		  vval.مجموعه_بیت(idx, BIT4_1);
	    وإلا
		  vval.مجموعه_بیت(idx, BIT4_0);

	    val >>= 1;
      }

      ptr.ptr()->send_vec4(vval, 0);
}


// Power

vvp_arith_pow::vvp_arith_pow(بیعلامت wid, منطقي signed_flag)
: vvp_arith_(wid), signed_flag_(signed_flag)
{
}

vvp_arith_pow::~vvp_arith_pow()
{
}

فراغ vvp_arith_pow::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                              vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      vvp_vector2_t a2 (op_a_, درست);
      vvp_vector2_t b2 (op_b_, درست);

        // If we have an X or Z in the arguments return X.
      إذا (a2.is_NaN() || b2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    أرجع;
      }

	// Is the exponent negative? If so, table 5-6 in IEEE1364-2005
	// defines what value is returned.
      إذا (signed_flag_ && b2.value(b2.size()-1)) {
	    صحيح a_val;
	    مزدوج r_val = 0.0;
	    إذا (vector2_مقصد_مقدار(a2, a_val, درست)) {
		  إذا (a_val == 0) {
			ptr.ptr()->send_vec4(x_val_, 0);
			أرجع;
		  }
		  إذا (a_val == 1) {
			r_val = 1.0;
		  }
		  إذا (a_val == -1) {
			r_val = b2.value(0) ? -1.0 : 1.0;
		  }
	    }
	    ptr.ptr()->send_vec4(vvp_vector4_t(wid_, r_val), 0);
	    أرجع;
      }

      ptr.ptr()->send_vec4(vector2_مقصد_vector4(توان(a2, b2), wid_), 0);
}


// Addition

vvp_arith_جمع::vvp_arith_جمع(بیعلامت wid)
: vvp_arith_(wid)
{
}

vvp_arith_جمع::~vvp_arith_جمع()
{
}

فراغ vvp_arith_جمع::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                              vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      vvp_net_t*net = ptr.ptr();

      vvp_vector4_t value (wid_);

	/* Pad input vectors with this value to widen to the desired
	   output width. */
      ثابت vvp_bit4_t pad = BIT4_0;

      vvp_bit4_t انتقال = BIT4_0;
      لكل (بیعلامت idx = 0 ;  idx < wid_ ;  idx += 1) {
	    vvp_bit4_t a = (idx >= op_a_.size())? pad : op_a_.value(idx);
	    vvp_bit4_t b = (idx >= op_b_.size())? pad : op_b_.value(idx);
	    vvp_bit4_t cur = جمع_کن_with_انتقال(a, b, انتقال);

	    إذا (cur == BIT4_X) {
		  net->send_vec4(x_val_, 0);
		  أرجع;
	    }

	    value.مجموعه_بیت(idx, cur);
      }

      net->send_vec4(value, 0);
}

vvp_arith_sub::vvp_arith_sub(بیعلامت wid)
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
فراغ vvp_arith_sub::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                              vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      vvp_net_t*net = ptr.ptr();

      vvp_vector4_t value (wid_);

	/* Pad input vectors with this value to widen to the desired
	   output width. */
      ثابت vvp_bit4_t pad = BIT4_1;

      vvp_bit4_t انتقال = BIT4_1;
      لكل (بیعلامت idx = 0 ;  idx < wid_ ;  idx += 1) {
	    vvp_bit4_t a = (idx >= op_a_.size())? pad : op_a_.value(idx);
	    vvp_bit4_t b = (idx >= op_b_.size())? pad : ~op_b_.value(idx);
	    vvp_bit4_t cur = جمع_کن_with_انتقال(a, b, انتقال);

	    إذا (cur == BIT4_X) {
		  net->send_vec4(x_val_, 0);
		  أرجع;
	    }

	    value.مجموعه_بیت(idx, cur);
      }

      net->send_vec4(value, 0);
}

vvp_cmp_eeq::vvp_cmp_eeq(بیعلامت wid)
: vvp_arith_(wid)
{
}

فراغ vvp_cmp_eeq::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                            vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      vvp_vector4_t eeq (1);
      eeq.مجموعه_بیت(0, BIT4_1);

      أكد(op_a_.size() == op_b_.size());
      لكل (بیعلامت idx = 0 ;  idx < op_a_.size() ;  idx += 1)
	    إذا (op_a_.value(idx) != op_b_.value(idx)) {
		  eeq.مجموعه_بیت(0, BIT4_0);
		  اكسر;
	    }


      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}

vvp_cmp_nee::vvp_cmp_nee(بیعلامت wid)
: vvp_arith_(wid)
{
}

فراغ vvp_cmp_nee::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                            vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      vvp_vector4_t eeq (1);
      eeq.مجموعه_بیت(0, BIT4_0);

      أكد(op_a_.size() == op_b_.size());
      لكل (بیعلامت idx = 0 ;  idx < op_a_.size() ;  idx += 1)
	    إذا (op_a_.value(idx) != op_b_.value(idx)) {
		  eeq.مجموعه_بیت(0, BIT4_1);
		  اكسر;
	    }


      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}

vvp_cmp_eq::vvp_cmp_eq(بیعلامت wid)
: vvp_arith_(wid)
{
}

/*
 * Compare Vector a and Vector b. If in any bit position the a and b
 * bits are known and different, then the result is 0. Otherwise, if
 * there are X/Z bits anywhere in A or B, the result is X. Finally,
 * the result is 1.
 */
فراغ vvp_cmp_eq::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                           vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      إذا (op_a_.size() != op_b_.size()) {
	    cerr << "COMPARISON size mismatch. "
		 << "a=" << op_a_ << ", b=" << op_b_ << پایان_سطر;
	    أكد(0);
      }

      vvp_vector4_t res (1);
      res.مجموعه_بیت(0, BIT4_1);

      لكل (بیعلامت idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    إذا (a == BIT4_X)
		  res.مجموعه_بیت(0, BIT4_X);
	    وإلا إذا (a == BIT4_Z)
		  res.مجموعه_بیت(0, BIT4_X);
	    وإلا إذا (b == BIT4_X)
		  res.مجموعه_بیت(0, BIT4_X);
	    وإلا إذا (b == BIT4_Z)
		  res.مجموعه_بیت(0, BIT4_X);
            وإلا إذا (a != b) {
		  res.مجموعه_بیت(0, BIT4_0);
		  اكسر;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}

vvp_cmp_eqx::vvp_cmp_eqx(بیعلامت wid)
: vvp_arith_(wid)
{
}

/*
 * Compare Vector a and Vector b. If in any bit position the a and b
 * bits are known and different, then the result is 0. Otherwise, if
 * there are X/Z bits anywhere in A or B, the result is X. Finally,
 * the result is 1.
 */
فراغ vvp_cmp_eqx::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                           vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      إذا (op_a_.size() != op_b_.size()) {
	    cerr << "COMPARISON size mismatch. "
		 << "a=" << op_a_ << ", b=" << op_b_ << پایان_سطر;
	    أكد(0);
      }

      vvp_vector4_t res (1);
      res.مجموعه_بیت(0, BIT4_1);

      لكل (بیعلامت idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    إذا (b == BIT4_X)
		  استمر;
	    إذا (b == BIT4_Z)
		  استمر;
	    إذا (a != b) {
		  res.مجموعه_بیت(0, BIT4_0);
		  اكسر;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}

vvp_cmp_eqz::vvp_cmp_eqz(بیعلامت wid)
: vvp_arith_(wid)
{
}

فراغ vvp_cmp_eqz::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                           vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      إذا (op_a_.size() != op_b_.size()) {
	    cerr << "COMPARISON size mismatch. "
		 << "a=" << op_a_ << ", b=" << op_b_ << پایان_سطر;
	    أكد(0);
      }

      vvp_vector4_t res (1);
      res.مجموعه_بیت(0, BIT4_1);

      لكل (بیعلامت idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    إذا (b == BIT4_Z)
		  استمر;
	    إذا (a != b) {
		  res.مجموعه_بیت(0, BIT4_0);
		  اكسر;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}

vvp_cmp_ne::vvp_cmp_ne(بیعلامت wid)
: vvp_arith_(wid)
{
}

/*
 * Compare Vector a and Vector b. If in any bit position the a and b
 * bits are known and different, then the result is 1. Otherwise, if
 * there are X/Z bits anywhere in A or B, the result is X. Finally,
 * the result is 0.
 */
فراغ vvp_cmp_ne::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                           vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      إذا (op_a_.size() != op_b_.size()) {
	    cerr << "internal error: vvp_cmp_ne: op_a_=" << op_a_
		 << ", op_b_=" << op_b_ << پایان_سطر;
	    أكد(op_a_.size() == op_b_.size());
      }

      vvp_vector4_t res (1);
      res.مجموعه_بیت(0, BIT4_0);

      لكل (بیعلامت idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    إذا (a == BIT4_X)
		  res.مجموعه_بیت(0, BIT4_X);
	    وإلا إذا (a == BIT4_Z)
		  res.مجموعه_بیت(0, BIT4_X);
	    وإلا إذا (b == BIT4_X)
		  res.مجموعه_بیت(0, BIT4_X);
	    وإلا إذا (b == BIT4_Z)
		  res.مجموعه_بیت(0, BIT4_X);
            وإلا إذا (a != b) {
		  res.مجموعه_بیت(0, BIT4_1);
		  اكسر;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}


vvp_cmp_gtge_پایه::vvp_cmp_gtge_پایه(بیعلامت wid, منطقي flag)
: vvp_arith_(wid), signed_flag_(flag)
{
}


فراغ vvp_cmp_gtge_پایه::recv_vec4_پایه(vvp_net_ptr_t ptr,
					 ثابت vvp_vector4_t&بیت,
					 vvp_bit4_t بیرون_if_equal)
{
      dispatch_عملوند(ptr, بیت);

      vvp_bit4_t out = signed_flag_
	    ? compare_gtge_signed(op_a_, op_b_, بیرون_if_equal)
	    : compare_gtge(op_a_, op_b_, بیرون_if_equal);
      vvp_vector4_t val (1);
      val.مجموعه_بیت(0, out);
      ptr.ptr()->send_vec4(val, 0);

      أرجع;
}


vvp_cmp_ge::vvp_cmp_ge(بیعلامت wid, منطقي flag)
: vvp_cmp_gtge_پایه(wid, flag)
{
}

فراغ vvp_cmp_ge::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                           vvp_context_t)
{
      recv_vec4_پایه(ptr, بیت, BIT4_1);
}

vvp_cmp_gt::vvp_cmp_gt(بیعلامت wid, منطقي flag)
: vvp_cmp_gtge_پایه(wid, flag)
{
}

فراغ vvp_cmp_gt::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                           vvp_context_t)
{
      recv_vec4_پایه(ptr, بیت, BIT4_0);
}

vvp_cmp_weq::vvp_cmp_weq(بیعلامت wid)
: vvp_arith_(wid)
{
}

فراغ vvp_cmp_weq::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                            vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      vvp_vector4_t eeq (1);
      eeq.مجموعه_بیت(0, BIT4_1);

      أكد(op_a_.size() == op_b_.size());
      لكل (بیعلامت idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);
	    إذا (b == BIT4_X)
		  استمر;
	    وإلا إذا (b == BIT4_Z)
		  استمر;
	    وإلا إذا (a == BIT4_X)
		  eeq.مجموعه_بیت(0, BIT4_X);
	    وإلا إذا (a == BIT4_Z)
		  eeq.مجموعه_بیت(0, BIT4_X);
            وإلا إذا (a != b) {
		  eeq.مجموعه_بیت(0, BIT4_0);
		  اكسر;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}

vvp_cmp_wne::vvp_cmp_wne(بیعلامت wid)
: vvp_arith_(wid)
{
}

فراغ vvp_cmp_wne::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                            vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      vvp_vector4_t eeq (1);
      eeq.مجموعه_بیت(0, BIT4_0);

      أكد(op_a_.size() == op_b_.size());
      لكل (بیعلامت idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);
	    إذا (b == BIT4_X)
		  استمر;
	    وإلا إذا (b == BIT4_Z)
		  استمر;
	    وإلا إذا (a == BIT4_X)
		  eeq.مجموعه_بیت(0, BIT4_X);
	    وإلا إذا (a == BIT4_Z)
		  eeq.مجموعه_بیت(0, BIT4_X);
            وإلا إذا (a != b) {
		  eeq.مجموعه_بیت(0, BIT4_1);
		  اكسر;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}


vvp_shiftl::vvp_shiftl(بیعلامت wid)
: vvp_arith_(wid)
{
}

vvp_shiftl::~vvp_shiftl()
{
}

فراغ vvp_shiftl::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                           vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      vvp_vector4_t out (op_a_.size());

      منطقي سرریز_flag;
      بیعلامت دراز لغزش = 0;
      إذا (! vector4_مقصد_مقدار(op_b_, سرریز_flag, لغزش)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    أرجع;
      }

      إذا (سرریز_flag || لغزش > out.size())
	    لغزش = out.size();

      لكل (بیعلامت idx = 0 ;  idx < لغزش ;  idx += 1)
	    out.مجموعه_بیت(idx, BIT4_0);

      لكل (بیعلامت idx = لغزش ;  idx < out.size() ;  idx += 1)
	    out.مجموعه_بیت(idx, op_a_.value(idx-لغزش));

      ptr.ptr()->send_vec4(out, 0);
}

vvp_shiftr::vvp_shiftr(بیعلامت wid, منطقي signed_flag)
: vvp_arith_(wid), signed_flag_(signed_flag)
{
}

vvp_shiftr::~vvp_shiftr()
{
}

فراغ vvp_shiftr::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                           vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      vvp_vector4_t out (op_a_.size());

      منطقي سرریز_flag;
      بیعلامت دراز لغزش = 0;
      إذا (! vector4_مقصد_مقدار(op_b_, سرریز_flag, لغزش)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    أرجع;
      }

      إذا (سرریز_flag || لغزش > out.size())
	    لغزش = out.size();

      لكل (بیعلامت idx = لغزش ;  idx < out.size() ;  idx += 1)
	    out.مجموعه_بیت(idx-لغزش, op_a_.value(idx));

      vvp_bit4_t pad = BIT4_0;
      إذا (signed_flag_ && op_a_.size() > 0)
	    pad = op_a_.value(op_a_.size()-1);

      لكل (بیعلامت idx = 0 ;  idx < لغزش ;  idx += 1)
	    out.مجموعه_بیت(idx+out.size()-لغزش, pad);

      ptr.ptr()->send_vec4(out, 0);
}


vvp_arith_real_::vvp_arith_real_()
{
      op_a_ = 0.0;
      op_b_ = 0.0;
}

فراغ vvp_arith_real_::dispatch_عملوند(vvp_net_ptr_t ptr, مزدوج بیت)
{
      بدل (ptr.پورت()) {
	  حالة 0:
	    op_a_ = بیت;
	    اكسر;
	  حالة 1:
	    op_b_ = بیت;
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.پورت());
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

فراغ vvp_arith_mult_real::recv_real(vvp_net_ptr_t ptr, مزدوج بیت,
                                    vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      مزدوج val = op_a_ * op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real power. */
vvp_arith_pow_real::vvp_arith_pow_real()
{
}

vvp_arith_pow_real::~vvp_arith_pow_real()
{
}

فراغ vvp_arith_pow_real::recv_real(vvp_net_ptr_t ptr, مزدوج بیت,
                                   vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      مزدوج val = توان(op_a_, op_b_);
      ptr.ptr()->send_real(val, 0);
}

/* Real division. */
vvp_arith_div_real::vvp_arith_div_real()
{
}

vvp_arith_div_real::~vvp_arith_div_real()
{
}

فراغ vvp_arith_div_real::recv_real(vvp_net_ptr_t ptr, مزدوج بیت,
                                   vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      مزدوج val = op_a_ / op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real modulus. */
vvp_arith_mod_real::vvp_arith_mod_real()
{
}

vvp_arith_mod_real::~vvp_arith_mod_real()
{
}

فراغ vvp_arith_mod_real::recv_real(vvp_net_ptr_t ptr, مزدوج بیت,
                                   vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      مزدوج val = fmod(op_a_, op_b_);
      ptr.ptr()->send_real(val, 0);
}

/* Real summation. */
vvp_arith_جمع_real::vvp_arith_جمع_real()
{
}

vvp_arith_جمع_real::~vvp_arith_جمع_real()
{
}

فراغ vvp_arith_جمع_real::recv_real(vvp_net_ptr_t ptr, مزدوج بیت,
                                   vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      مزدوج val = op_a_ + op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real subtraction. */
vvp_arith_sub_real::vvp_arith_sub_real()
{
}

vvp_arith_sub_real::~vvp_arith_sub_real()
{
}

فراغ vvp_arith_sub_real::recv_real(vvp_net_ptr_t ptr, مزدوج بیت,
                                   vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      مزدوج val = op_a_ - op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real compare equal. */
vvp_cmp_eq_real::vvp_cmp_eq_real()
{
}

فراغ vvp_cmp_eq_real::recv_real(vvp_net_ptr_t ptr, ثابت مزدوج بیت,
                                vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      vvp_vector4_t res (1);
      إذا (op_a_ == op_b_) res.مجموعه_بیت(0, BIT4_1);
      وإلا res.مجموعه_بیت(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}

/* Real compare not equal. */
vvp_cmp_ne_real::vvp_cmp_ne_real()
{
}

فراغ vvp_cmp_ne_real::recv_real(vvp_net_ptr_t ptr, ثابت مزدوج بیت,
                                vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      vvp_vector4_t res (1);
      إذا (op_a_ != op_b_) res.مجموعه_بیت(0, BIT4_1);
      وإلا res.مجموعه_بیت(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}

/* Real compare greater than or equal. */
vvp_cmp_ge_real::vvp_cmp_ge_real()
{
}

فراغ vvp_cmp_ge_real::recv_real(vvp_net_ptr_t ptr, ثابت مزدوج بیت,
                                vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      vvp_vector4_t res (1);
      إذا (op_a_ >= op_b_) res.مجموعه_بیت(0, BIT4_1);
      وإلا res.مجموعه_بیت(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}

/* Real compare greater than. */
vvp_cmp_gt_real::vvp_cmp_gt_real()
{
}

فراغ vvp_cmp_gt_real::recv_real(vvp_net_ptr_t ptr, ثابت مزدوج بیت,
                                vvp_context_t)
{
      dispatch_عملوند(ptr, بیت);

      vvp_vector4_t res (1);
      إذا (op_a_ > op_b_) res.مجموعه_بیت(0, BIT4_1);
      وإلا res.مجموعه_بیت(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}
