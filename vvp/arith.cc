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

ಬಳಸಿ ನಾಮಾಕಾಶ std;

vvp_arith_::vvp_arith_(ಚಿಹ್ನೆರಹಿತ wid)
: wid_(wid), op_a_(wid), op_b_(wid), x_val_(wid)
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid ;  idx += 1) {
	    op_a_ .ಗಣ_ಬಿಟ್(idx, BIT4_Z);
	    op_b_ .ಗಣ_ಬಿಟ್(idx, BIT4_Z);
	    x_val_.ಗಣ_ಬಿಟ್(idx, BIT4_X);
      }
}

ಶೂನ್ಯ vvp_arith_::dispatch_ಕಾರ್ಯಾಂಶ(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್)
{
      ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್ = ptr.ಪೋರ್ಟ್();
      ಆಯ್ಕೆ (ಪೋರ್ಟ್) {
	  ಸ್ಥಿತಿ 0:
	    op_a_ = ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 1:
	    op_b_ = ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Unsupported port type %u.\n", ಪೋರ್ಟ್);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }
}

ಶೂನ್ಯ vvp_arith_::recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			      ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ಬಿಟ್, ತಳ, vwid, ctx);
}

vvp_arith_abs::vvp_arith_abs()
{
}

vvp_arith_abs::~vvp_arith_abs()
{
}

ಶೂನ್ಯ vvp_arith_abs::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                              vvp_context_t)
{
      vvp_vector4_t out (ಬಿಟ್.size(), BIT4_0);;

      vvp_bit4_t cmp = compare_gtge_signed(ಬಿಟ್, out, BIT4_1);
      ಆಯ್ಕೆ (cmp) {
	  ಸ್ಥಿತಿ BIT4_1: // bit >= 0
	    out = ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_0: //  bit < 0
	    out = ~ಬಿಟ್;
	    out += 1;
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ: // There's an X.
	    out = vvp_vector4_t(ಬಿಟ್.size(), BIT4_X);
	    ನಿಲ್ಲಿಸು;
      }

      ptr.ptr()->send_vec4(out, 0);
}

ಶೂನ್ಯ vvp_arith_abs::recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				 ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ಬಿಟ್, ತಳ, vwid, ctx);
}

ಶೂನ್ಯ vvp_arith_abs::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ ಬಿಟ್,
                              vvp_context_t)
{
      ದ್ವಿಗುಣ out = fabs(ಬಿಟ್);
      ptr.ptr()->send_real(out, 0);
}

vvp_arith_cast_int::vvp_arith_cast_int(ಚಿಹ್ನೆರಹಿತ wid)
: wid_(wid)
{
}

vvp_arith_cast_int::~vvp_arith_cast_int()
{
}

ಶೂನ್ಯ vvp_arith_cast_int::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ ಬಿಟ್,
                                   vvp_context_t)
{
      ptr.ptr()->send_vec4(vvp_vector4_t(wid_, ಬಿಟ್), 0);
}

vvp_arith_cast_real::vvp_arith_cast_real(ತಾರ್ಕಿಕ signed_flag)
: signed_(signed_flag)
{
}

vvp_arith_cast_real::~vvp_arith_cast_real()
{
}

ಶೂನ್ಯ vvp_arith_cast_real::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                    vvp_context_t)
{
      ದ್ವಿಗುಣ val;
      vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಬಿಟ್, val, signed_);
      ptr.ptr()->send_real(val, 0);
}

ಶೂನ್ಯ vvp_arith_cast_real::recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				       ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid,
				       vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ಬಿಟ್, ತಳ, vwid, ctx);
}

vvp_arith_cast_vec2::vvp_arith_cast_vec2(ಚಿಹ್ನೆರಹಿತ wid)
: wid_(wid)
{
}

vvp_arith_cast_vec2::~vvp_arith_cast_vec2()
{
}

ಶೂನ್ಯ vvp_arith_cast_vec2::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ ಬಿಟ್,
                                   vvp_context_t)
{
      ptr.ptr()->send_vec4(vvp_vector4_t(wid_, ಬಿಟ್), 0);
}

ಶೂನ್ಯ vvp_arith_cast_vec2::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                    vvp_context_t)
{
      vvp_vector2_t tmp = vvp_vector2_t(ಬಿಟ್);
      ptr.ptr()->send_vec4(vector2_ಗಮ್ಯ_vector4(tmp,wid_), 0);
}

ಶೂನ್ಯ vvp_arith_cast_vec2::recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				       ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ಬಿಟ್, ತಳ, vwid, ctx);
}

// Division

vvp_arith_div::vvp_arith_div(ಚಿಹ್ನೆರಹಿತ wid, ತಾರ್ಕಿಕ signed_flag)
: vvp_arith_(wid), signed_flag_(signed_flag)
{
}

vvp_arith_div::~vvp_arith_div()
{
}

ಶೂನ್ಯ vvp_arith_div::wide4_(vvp_net_ptr_t ptr)
{
      vvp_vector2_t a2 (op_a_, ನಿಜ);
      ಆದರೆ (a2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_vector2_t b2 (op_b_, ನಿಜ);
      ಆದರೆ (b2.is_NaN() || b2.is_zero()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ಹಿಂತಿರುಗಿ;
      }

      ತಾರ್ಕಿಕ negate = ಸುಳ್ಳು;
      ಆದರೆ (signed_flag_) {
	    ಆದರೆ (a2.value(a2.size()-1)) {
		  a2 = -a2;
		  negate = ನಿಜ;
            }
	    ಆದರೆ (b2.value(b2.size()-1)) {
		  b2 = -b2;
		  negate = !negate;
            }
      }
      vvp_vector2_t res = a2 / b2;
      ಆದರೆ (negate) res = -res;
      ptr.ptr()->send_vec4(vector2_ಗಮ್ಯ_vector4(res, wid_), 0);
}

ಶೂನ್ಯ vvp_arith_div::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                              vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      ಆದರೆ (wid_ > 8 * ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)) {
	    wide4_(ptr);
	    ಹಿಂತಿರುಗಿ ;
      }

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ a;
      ಆದರೆ (! vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(op_a_, a)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ಹಿಂತಿರುಗಿ;
      }

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ b;
      ಆದರೆ (! vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(op_b_, b)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ಹಿಂತಿರುಗಿ;
      }

      ತಾರ್ಕಿಕ negate = ಸುಳ್ಳು;
	/* If we are doing signed divide, then take the sign out of
	   the operands for now, and remember to put the sign back
	   later. */
      ಆದರೆ (signed_flag_) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sign_ಮುಖವಾಡ = 0;
	    ಆದರೆ (op_a_.size() != 8 * ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)) {
		  sign_ಮುಖವಾಡ = -1UL << op_a_.size();
	    }
	    ಆದರೆ (op_a_.value(op_a_.size()-1)) {
		  a = (-a) & ~sign_ಮುಖವಾಡ;
		  negate = !negate;
	    }

	    sign_ಮುಖವಾಡ = 0;
	    ಆದರೆ (op_b_.size() != 8 * ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)) {
		  sign_ಮುಖವಾಡ = -1UL << op_b_.size();
	    }
	    ಆದರೆ (op_b_.value(op_b_.size()-1)) {
		  b = (-b) & ~sign_ಮುಖವಾಡ;
		  negate = ! negate;
	    }
      }

      ಆದರೆ (b == 0) {
	    vvp_vector4_t xval (wid_);
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid_ ;  idx += 1)
		  xval.ಗಣ_ಬಿಟ್(idx, BIT4_X);

	    ptr.ptr()->send_vec4(xval, 0);
	    ಹಿಂತಿರುಗಿ;
      }

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ val = a / b;
      ಆದರೆ (negate)
	    val = -val;

      ಪ್ರತಿಜ್ಞಾ(wid_ <= 8*ಗಾತ್ರ(val));

      vvp_vector4_t vval (wid_);
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid_ ;  idx += 1) {
	    ಆದರೆ (val & 1)
		  vval.ಗಣ_ಬಿಟ್(idx, BIT4_1);
	    ಇಲ್ಲದಿದ್ದರೆ
		  vval.ಗಣ_ಬಿಟ್(idx, BIT4_0);

	    val >>= 1;
      }

      ptr.ptr()->send_vec4(vval, 0);
}


vvp_arith_mod::vvp_arith_mod(ಚಿಹ್ನೆರಹಿತ wid, ತಾರ್ಕಿಕ sf)
: vvp_arith_(wid), signed_flag_(sf)
{
}

vvp_arith_mod::~vvp_arith_mod()
{
}

ಶೂನ್ಯ vvp_arith_mod::wide_(vvp_net_ptr_t ptr)
{
      vvp_vector2_t a2 (op_a_, ನಿಜ);
      ಆದರೆ (a2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_vector2_t b2 (op_b_, ನಿಜ);
      ಆದರೆ (b2.is_NaN() || b2.is_zero()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ಹಿಂತಿರುಗಿ;
      }

      ತಾರ್ಕಿಕ negate = ಸುಳ್ಳು;
      ಆದರೆ (signed_flag_) {
	    ಆದರೆ (a2.value(a2.size()-1)) {
		  a2 = -a2;
		  negate = ನಿಜ;
            }
	    ಆದರೆ (b2.value(b2.size()-1)) {
		  b2 = -b2;
            }
      }
      vvp_vector2_t res = a2 % b2;
      ಆದರೆ (negate) res = -res;
      ptr.ptr()->send_vec4(vector2_ಗಮ್ಯ_vector4(res, res.size()), 0);
}

ಶೂನ್ಯ vvp_arith_mod::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                              vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      ಆದರೆ (wid_ > 8 * ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)) {
	    wide_(ptr);
	    ಹಿಂತಿರುಗಿ ;
      }

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ a;
      ಆದರೆ (! vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(op_a_, a)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ಹಿಂತಿರುಗಿ;
      }

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ b;
      ಆದರೆ (! vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(op_b_, b)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ಹಿಂತಿರುಗಿ;
      }

      ತಾರ್ಕಿಕ negate = ಸುಳ್ಳು;
	/* If we are doing signed divide, then take the sign out of
	   the operands for now, and remember to put the sign back
	   later. */
      ಆದರೆ (signed_flag_) {
	    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sign_ಮುಖವಾಡ = 0;
	    ಆದರೆ (op_a_.size() != 8 * ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)) {
		  sign_ಮುಖವಾಡ = -1UL << op_a_.size();
	    }
	    ಆದರೆ (op_a_.value(op_a_.size()-1)) {
		  a = (-a) & ~sign_ಮುಖವಾಡ;
		  negate = !negate;
	    }

	    sign_ಮುಖವಾಡ = 0;
	    ಆದರೆ (op_b_.size() != 8 * ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)) {
		  sign_ಮುಖವಾಡ = -1UL << op_b_.size();
	    }
	    ಆದರೆ (op_b_.value(op_b_.size()-1)) {
		  b = (-b) & ~sign_ಮುಖವಾಡ;
	    }
      }

      ಆದರೆ (b == 0) {
	    vvp_vector4_t xval (wid_);
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid_ ;  idx += 1)
		  xval.ಗಣ_ಬಿಟ್(idx, BIT4_X);

	    ptr.ptr()->send_vec4(xval, 0);
	    ಹಿಂತಿರುಗಿ;
      }

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ val = a % b;
      ಆದರೆ (negate)
	    val = -val;

      ಪ್ರತಿಜ್ಞಾ(wid_ <= 8*ಗಾತ್ರ(val));

      vvp_vector4_t vval (wid_);
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid_ ;  idx += 1) {
	    ಆದರೆ (val & 1)
		  vval.ಗಣ_ಬಿಟ್(idx, BIT4_1);
	    ಇಲ್ಲದಿದ್ದರೆ
		  vval.ಗಣ_ಬಿಟ್(idx, BIT4_0);

	    val >>= 1;
      }

      ptr.ptr()->send_vec4(vval, 0);
}


// Multiplication

vvp_arith_mult::vvp_arith_mult(ಚಿಹ್ನೆರಹಿತ wid)
: vvp_arith_(wid)
{
}

vvp_arith_mult::~vvp_arith_mult()
{
}

ಶೂನ್ಯ vvp_arith_mult::wide_(vvp_net_ptr_t ptr)
{
      vvp_vector2_t a2 (op_a_, ನಿಜ);
      vvp_vector2_t b2 (op_b_, ನಿಜ);

      ಆದರೆ (a2.is_NaN() || b2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_vector2_t ಫಲಿತಾಂಶ = a2 * b2;

      vvp_vector4_t res4 = vector2_ಗಮ್ಯ_vector4(ಫಲಿತಾಂಶ, wid_);
      ptr.ptr()->send_vec4(res4, 0);
}

ಶೂನ್ಯ vvp_arith_mult::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                               vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      ಆದರೆ (wid_ > 8 * ಗಾತ್ರ(int64_t)) {
	    wide_(ptr);
	    ಹಿಂತಿರುಗಿ ;
      }

      int64_t a;
      ಆದರೆ (! vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(op_a_, a, ಸುಳ್ಳು, ನಿಜ)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ಹಿಂತಿರುಗಿ;
      }

      int64_t b;
      ಆದರೆ (! vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(op_b_, b, ಸುಳ್ಳು, ನಿಜ)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ಹಿಂತಿರುಗಿ;
      }

      int64_t val = a * b;
      ಪ್ರತಿಜ್ಞಾ(wid_ <= 8*ಗಾತ್ರ(val));

      vvp_vector4_t vval (wid_);
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid_ ;  idx += 1) {
	    ಆದರೆ (val & 1)
		  vval.ಗಣ_ಬಿಟ್(idx, BIT4_1);
	    ಇಲ್ಲದಿದ್ದರೆ
		  vval.ಗಣ_ಬಿಟ್(idx, BIT4_0);

	    val >>= 1;
      }

      ptr.ptr()->send_vec4(vval, 0);
}


// Power

vvp_arith_pow::vvp_arith_pow(ಚಿಹ್ನೆರಹಿತ wid, ತಾರ್ಕಿಕ signed_flag)
: vvp_arith_(wid), signed_flag_(signed_flag)
{
}

vvp_arith_pow::~vvp_arith_pow()
{
}

ಶೂನ್ಯ vvp_arith_pow::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                              vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      vvp_vector2_t a2 (op_a_, ನಿಜ);
      vvp_vector2_t b2 (op_b_, ನಿಜ);

        // If we have an X or Z in the arguments return X.
      ಆದರೆ (a2.is_NaN() || b2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ಹಿಂತಿರುಗಿ;
      }

	// Is the exponent negative? If so, table 5-6 in IEEE1364-2005
	// defines what value is returned.
      ಆದರೆ (signed_flag_ && b2.value(b2.size()-1)) {
	    ಪೂರ್ಣಾಂಕ a_val;
	    ದ್ವಿಗುಣ r_val = 0.0;
	    ಆದರೆ (vector2_ಗಮ್ಯ_ಮೌಲ್ಯ(a2, a_val, ನಿಜ)) {
		  ಆದರೆ (a_val == 0) {
			ptr.ptr()->send_vec4(x_val_, 0);
			ಹಿಂತಿರುಗಿ;
		  }
		  ಆದರೆ (a_val == 1) {
			r_val = 1.0;
		  }
		  ಆದರೆ (a_val == -1) {
			r_val = b2.value(0) ? -1.0 : 1.0;
		  }
	    }
	    ptr.ptr()->send_vec4(vvp_vector4_t(wid_, r_val), 0);
	    ಹಿಂತಿರುಗಿ;
      }

      ptr.ptr()->send_vec4(vector2_ಗಮ್ಯ_vector4(ಘಾತ(a2, b2), wid_), 0);
}


// Addition

vvp_arith_ಮೊತ್ತ::vvp_arith_ಮೊತ್ತ(ಚಿಹ್ನೆರಹಿತ wid)
: vvp_arith_(wid)
{
}

vvp_arith_ಮೊತ್ತ::~vvp_arith_ಮೊತ್ತ()
{
}

ಶೂನ್ಯ vvp_arith_ಮೊತ್ತ::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                              vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      vvp_net_t*net = ptr.ptr();

      vvp_vector4_t value (wid_);

	/* Pad input vectors with this value to widen to the desired
	   output width. */
      ಸ್ಥಿರಾಂಕ vvp_bit4_t pad = BIT4_0;

      vvp_bit4_t ವಾಹಕ = BIT4_0;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid_ ;  idx += 1) {
	    vvp_bit4_t a = (idx >= op_a_.size())? pad : op_a_.value(idx);
	    vvp_bit4_t b = (idx >= op_b_.size())? pad : op_b_.value(idx);
	    vvp_bit4_t cur = ಕೂಡಿಸು_with_ವಾಹಕ(a, b, ವಾಹಕ);

	    ಆದರೆ (cur == BIT4_X) {
		  net->send_vec4(x_val_, 0);
		  ಹಿಂತಿರುಗಿ;
	    }

	    value.ಗಣ_ಬಿಟ್(idx, cur);
      }

      net->send_vec4(value, 0);
}

vvp_arith_sub::vvp_arith_sub(ಚಿಹ್ನೆರಹಿತ wid)
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
ಶೂನ್ಯ vvp_arith_sub::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                              vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      vvp_net_t*net = ptr.ptr();

      vvp_vector4_t value (wid_);

	/* Pad input vectors with this value to widen to the desired
	   output width. */
      ಸ್ಥಿರಾಂಕ vvp_bit4_t pad = BIT4_1;

      vvp_bit4_t ವಾಹಕ = BIT4_1;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < wid_ ;  idx += 1) {
	    vvp_bit4_t a = (idx >= op_a_.size())? pad : op_a_.value(idx);
	    vvp_bit4_t b = (idx >= op_b_.size())? pad : ~op_b_.value(idx);
	    vvp_bit4_t cur = ಕೂಡಿಸು_with_ವಾಹಕ(a, b, ವಾಹಕ);

	    ಆದರೆ (cur == BIT4_X) {
		  net->send_vec4(x_val_, 0);
		  ಹಿಂತಿರುಗಿ;
	    }

	    value.ಗಣ_ಬಿಟ್(idx, cur);
      }

      net->send_vec4(value, 0);
}

vvp_cmp_eeq::vvp_cmp_eeq(ಚಿಹ್ನೆರಹಿತ wid)
: vvp_arith_(wid)
{
}

ಶೂನ್ಯ vvp_cmp_eeq::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                            vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      vvp_vector4_t eeq (1);
      eeq.ಗಣ_ಬಿಟ್(0, BIT4_1);

      ಪ್ರತಿಜ್ಞಾ(op_a_.size() == op_b_.size());
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < op_a_.size() ;  idx += 1)
	    ಆದರೆ (op_a_.value(idx) != op_b_.value(idx)) {
		  eeq.ಗಣ_ಬಿಟ್(0, BIT4_0);
		  ನಿಲ್ಲಿಸು;
	    }


      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}

vvp_cmp_nee::vvp_cmp_nee(ಚಿಹ್ನೆರಹಿತ wid)
: vvp_arith_(wid)
{
}

ಶೂನ್ಯ vvp_cmp_nee::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                            vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      vvp_vector4_t eeq (1);
      eeq.ಗಣ_ಬಿಟ್(0, BIT4_0);

      ಪ್ರತಿಜ್ಞಾ(op_a_.size() == op_b_.size());
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < op_a_.size() ;  idx += 1)
	    ಆದರೆ (op_a_.value(idx) != op_b_.value(idx)) {
		  eeq.ಗಣ_ಬಿಟ್(0, BIT4_1);
		  ನಿಲ್ಲಿಸು;
	    }


      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}

vvp_cmp_eq::vvp_cmp_eq(ಚಿಹ್ನೆರಹಿತ wid)
: vvp_arith_(wid)
{
}

/*
 * Compare Vector a and Vector b. If in any bit position the a and b
 * bits are known and different, then the result is 0. Otherwise, if
 * there are X/Z bits anywhere in A or B, the result is X. Finally,
 * the result is 1.
 */
ಶೂನ್ಯ vvp_cmp_eq::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                           vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      ಆದರೆ (op_a_.size() != op_b_.size()) {
	    cerr << "COMPARISON size mismatch. "
		 << "a=" << op_a_ << ", b=" << op_b_ << ಸಾಲು_ಅಂತ್ಯ;
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      vvp_vector4_t res (1);
      res.ಗಣ_ಬಿಟ್(0, BIT4_1);

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    ಆದರೆ (a == BIT4_X)
		  res.ಗಣ_ಬಿಟ್(0, BIT4_X);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (a == BIT4_Z)
		  res.ಗಣ_ಬಿಟ್(0, BIT4_X);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (b == BIT4_X)
		  res.ಗಣ_ಬಿಟ್(0, BIT4_X);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (b == BIT4_Z)
		  res.ಗಣ_ಬಿಟ್(0, BIT4_X);
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (a != b) {
		  res.ಗಣ_ಬಿಟ್(0, BIT4_0);
		  ನಿಲ್ಲಿಸು;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}

vvp_cmp_eqx::vvp_cmp_eqx(ಚಿಹ್ನೆರಹಿತ wid)
: vvp_arith_(wid)
{
}

/*
 * Compare Vector a and Vector b. If in any bit position the a and b
 * bits are known and different, then the result is 0. Otherwise, if
 * there are X/Z bits anywhere in A or B, the result is X. Finally,
 * the result is 1.
 */
ಶೂನ್ಯ vvp_cmp_eqx::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                           vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      ಆದರೆ (op_a_.size() != op_b_.size()) {
	    cerr << "COMPARISON size mismatch. "
		 << "a=" << op_a_ << ", b=" << op_b_ << ಸಾಲು_ಅಂತ್ಯ;
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      vvp_vector4_t res (1);
      res.ಗಣ_ಬಿಟ್(0, BIT4_1);

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    ಆದರೆ (b == BIT4_X)
		  ಮುಂದುವರಿ;
	    ಆದರೆ (b == BIT4_Z)
		  ಮುಂದುವರಿ;
	    ಆದರೆ (a != b) {
		  res.ಗಣ_ಬಿಟ್(0, BIT4_0);
		  ನಿಲ್ಲಿಸು;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}

vvp_cmp_eqz::vvp_cmp_eqz(ಚಿಹ್ನೆರಹಿತ wid)
: vvp_arith_(wid)
{
}

ಶೂನ್ಯ vvp_cmp_eqz::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                           vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      ಆದರೆ (op_a_.size() != op_b_.size()) {
	    cerr << "COMPARISON size mismatch. "
		 << "a=" << op_a_ << ", b=" << op_b_ << ಸಾಲು_ಅಂತ್ಯ;
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      vvp_vector4_t res (1);
      res.ಗಣ_ಬಿಟ್(0, BIT4_1);

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    ಆದರೆ (b == BIT4_Z)
		  ಮುಂದುವರಿ;
	    ಆದರೆ (a != b) {
		  res.ಗಣ_ಬಿಟ್(0, BIT4_0);
		  ನಿಲ್ಲಿಸು;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}

vvp_cmp_ne::vvp_cmp_ne(ಚಿಹ್ನೆರಹಿತ wid)
: vvp_arith_(wid)
{
}

/*
 * Compare Vector a and Vector b. If in any bit position the a and b
 * bits are known and different, then the result is 1. Otherwise, if
 * there are X/Z bits anywhere in A or B, the result is X. Finally,
 * the result is 0.
 */
ಶೂನ್ಯ vvp_cmp_ne::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                           vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      ಆದರೆ (op_a_.size() != op_b_.size()) {
	    cerr << "internal error: vvp_cmp_ne: op_a_=" << op_a_
		 << ", op_b_=" << op_b_ << ಸಾಲು_ಅಂತ್ಯ;
	    ಪ್ರತಿಜ್ಞಾ(op_a_.size() == op_b_.size());
      }

      vvp_vector4_t res (1);
      res.ಗಣ_ಬಿಟ್(0, BIT4_0);

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    ಆದರೆ (a == BIT4_X)
		  res.ಗಣ_ಬಿಟ್(0, BIT4_X);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (a == BIT4_Z)
		  res.ಗಣ_ಬಿಟ್(0, BIT4_X);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (b == BIT4_X)
		  res.ಗಣ_ಬಿಟ್(0, BIT4_X);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (b == BIT4_Z)
		  res.ಗಣ_ಬಿಟ್(0, BIT4_X);
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (a != b) {
		  res.ಗಣ_ಬಿಟ್(0, BIT4_1);
		  ನಿಲ್ಲಿಸು;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}


vvp_cmp_gtge_ತಳ::vvp_cmp_gtge_ತಳ(ಚಿಹ್ನೆರಹಿತ wid, ತಾರ್ಕಿಕ flag)
: vvp_arith_(wid), signed_flag_(flag)
{
}


ಶೂನ್ಯ vvp_cmp_gtge_ತಳ::recv_vec4_ತಳ(vvp_net_ptr_t ptr,
					 ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
					 vvp_bit4_t ಹೊರಹೋಗು_if_equal)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      vvp_bit4_t out = signed_flag_
	    ? compare_gtge_signed(op_a_, op_b_, ಹೊರಹೋಗು_if_equal)
	    : compare_gtge(op_a_, op_b_, ಹೊರಹೋಗು_if_equal);
      vvp_vector4_t val (1);
      val.ಗಣ_ಬಿಟ್(0, out);
      ptr.ptr()->send_vec4(val, 0);

      ಹಿಂತಿರುಗಿ;
}


vvp_cmp_ge::vvp_cmp_ge(ಚಿಹ್ನೆರಹಿತ wid, ತಾರ್ಕಿಕ flag)
: vvp_cmp_gtge_ತಳ(wid, flag)
{
}

ಶೂನ್ಯ vvp_cmp_ge::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                           vvp_context_t)
{
      recv_vec4_ತಳ(ptr, ಬಿಟ್, BIT4_1);
}

vvp_cmp_gt::vvp_cmp_gt(ಚಿಹ್ನೆರಹಿತ wid, ತಾರ್ಕಿಕ flag)
: vvp_cmp_gtge_ತಳ(wid, flag)
{
}

ಶೂನ್ಯ vvp_cmp_gt::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                           vvp_context_t)
{
      recv_vec4_ತಳ(ptr, ಬಿಟ್, BIT4_0);
}

vvp_cmp_weq::vvp_cmp_weq(ಚಿಹ್ನೆರಹಿತ wid)
: vvp_arith_(wid)
{
}

ಶೂನ್ಯ vvp_cmp_weq::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                            vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      vvp_vector4_t eeq (1);
      eeq.ಗಣ_ಬಿಟ್(0, BIT4_1);

      ಪ್ರತಿಜ್ಞಾ(op_a_.size() == op_b_.size());
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);
	    ಆದರೆ (b == BIT4_X)
		  ಮುಂದುವರಿ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (b == BIT4_Z)
		  ಮುಂದುವರಿ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (a == BIT4_X)
		  eeq.ಗಣ_ಬಿಟ್(0, BIT4_X);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (a == BIT4_Z)
		  eeq.ಗಣ_ಬಿಟ್(0, BIT4_X);
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (a != b) {
		  eeq.ಗಣ_ಬಿಟ್(0, BIT4_0);
		  ನಿಲ್ಲಿಸು;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}

vvp_cmp_wne::vvp_cmp_wne(ಚಿಹ್ನೆರಹಿತ wid)
: vvp_arith_(wid)
{
}

ಶೂನ್ಯ vvp_cmp_wne::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                            vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      vvp_vector4_t eeq (1);
      eeq.ಗಣ_ಬಿಟ್(0, BIT4_0);

      ಪ್ರತಿಜ್ಞಾ(op_a_.size() == op_b_.size());
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);
	    ಆದರೆ (b == BIT4_X)
		  ಮುಂದುವರಿ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (b == BIT4_Z)
		  ಮುಂದುವರಿ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (a == BIT4_X)
		  eeq.ಗಣ_ಬಿಟ್(0, BIT4_X);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (a == BIT4_Z)
		  eeq.ಗಣ_ಬಿಟ್(0, BIT4_X);
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (a != b) {
		  eeq.ಗಣ_ಬಿಟ್(0, BIT4_1);
		  ನಿಲ್ಲಿಸು;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}


vvp_shiftl::vvp_shiftl(ಚಿಹ್ನೆರಹಿತ wid)
: vvp_arith_(wid)
{
}

vvp_shiftl::~vvp_shiftl()
{
}

ಶೂನ್ಯ vvp_shiftl::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                           vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      vvp_vector4_t out (op_a_.size());

      ತಾರ್ಕಿಕ ಅತಿಪ್ರವಾಹ_flag;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಸರಿಸು = 0;
      ಆದರೆ (! vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(op_b_, ಅತಿಪ್ರವಾಹ_flag, ಸರಿಸು)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ಹಿಂತಿರುಗಿ;
      }

      ಆದರೆ (ಅತಿಪ್ರವಾಹ_flag || ಸರಿಸು > out.size())
	    ಸರಿಸು = out.size();

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಸರಿಸು ;  idx += 1)
	    out.ಗಣ_ಬಿಟ್(idx, BIT4_0);

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = ಸರಿಸು ;  idx < out.size() ;  idx += 1)
	    out.ಗಣ_ಬಿಟ್(idx, op_a_.value(idx-ಸರಿಸು));

      ptr.ptr()->send_vec4(out, 0);
}

vvp_shiftr::vvp_shiftr(ಚಿಹ್ನೆರಹಿತ wid, ತಾರ್ಕಿಕ signed_flag)
: vvp_arith_(wid), signed_flag_(signed_flag)
{
}

vvp_shiftr::~vvp_shiftr()
{
}

ಶೂನ್ಯ vvp_shiftr::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                           vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      vvp_vector4_t out (op_a_.size());

      ತಾರ್ಕಿಕ ಅತಿಪ್ರವಾಹ_flag;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಸರಿಸು = 0;
      ಆದರೆ (! vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(op_b_, ಅತಿಪ್ರವಾಹ_flag, ಸರಿಸು)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ಹಿಂತಿರುಗಿ;
      }

      ಆದರೆ (ಅತಿಪ್ರವಾಹ_flag || ಸರಿಸು > out.size())
	    ಸರಿಸು = out.size();

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = ಸರಿಸು ;  idx < out.size() ;  idx += 1)
	    out.ಗಣ_ಬಿಟ್(idx-ಸರಿಸು, op_a_.value(idx));

      vvp_bit4_t pad = BIT4_0;
      ಆದರೆ (signed_flag_ && op_a_.size() > 0)
	    pad = op_a_.value(op_a_.size()-1);

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಸರಿಸು ;  idx += 1)
	    out.ಗಣ_ಬಿಟ್(idx+out.size()-ಸರಿಸು, pad);

      ptr.ptr()->send_vec4(out, 0);
}


vvp_arith_real_::vvp_arith_real_()
{
      op_a_ = 0.0;
      op_b_ = 0.0;
}

ಶೂನ್ಯ vvp_arith_real_::dispatch_ಕಾರ್ಯಾಂಶ(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ ಬಿಟ್)
{
      ಆಯ್ಕೆ (ptr.ಪೋರ್ಟ್()) {
	  ಸ್ಥಿತಿ 0:
	    op_a_ = ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 1:
	    op_b_ = ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.ಪೋರ್ಟ್());
	    ಪ್ರತಿಜ್ಞಾ(0);
      }
}


/* Real multiplication. */
vvp_arith_mult_real::vvp_arith_mult_real()
{
}

vvp_arith_mult_real::~vvp_arith_mult_real()
{
}

ಶೂನ್ಯ vvp_arith_mult_real::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ ಬಿಟ್,
                                    vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      ದ್ವಿಗುಣ val = op_a_ * op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real power. */
vvp_arith_pow_real::vvp_arith_pow_real()
{
}

vvp_arith_pow_real::~vvp_arith_pow_real()
{
}

ಶೂನ್ಯ vvp_arith_pow_real::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ ಬಿಟ್,
                                   vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      ದ್ವಿಗುಣ val = ಘಾತ(op_a_, op_b_);
      ptr.ptr()->send_real(val, 0);
}

/* Real division. */
vvp_arith_div_real::vvp_arith_div_real()
{
}

vvp_arith_div_real::~vvp_arith_div_real()
{
}

ಶೂನ್ಯ vvp_arith_div_real::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ ಬಿಟ್,
                                   vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      ದ್ವಿಗುಣ val = op_a_ / op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real modulus. */
vvp_arith_mod_real::vvp_arith_mod_real()
{
}

vvp_arith_mod_real::~vvp_arith_mod_real()
{
}

ಶೂನ್ಯ vvp_arith_mod_real::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ ಬಿಟ್,
                                   vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      ದ್ವಿಗುಣ val = fmod(op_a_, op_b_);
      ptr.ptr()->send_real(val, 0);
}

/* Real summation. */
vvp_arith_ಮೊತ್ತ_real::vvp_arith_ಮೊತ್ತ_real()
{
}

vvp_arith_ಮೊತ್ತ_real::~vvp_arith_ಮೊತ್ತ_real()
{
}

ಶೂನ್ಯ vvp_arith_ಮೊತ್ತ_real::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ ಬಿಟ್,
                                   vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      ದ್ವಿಗುಣ val = op_a_ + op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real subtraction. */
vvp_arith_sub_real::vvp_arith_sub_real()
{
}

vvp_arith_sub_real::~vvp_arith_sub_real()
{
}

ಶೂನ್ಯ vvp_arith_sub_real::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ ಬಿಟ್,
                                   vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      ದ್ವಿಗುಣ val = op_a_ - op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real compare equal. */
vvp_cmp_eq_real::vvp_cmp_eq_real()
{
}

ಶೂನ್ಯ vvp_cmp_eq_real::recv_real(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ ಬಿಟ್,
                                vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      vvp_vector4_t res (1);
      ಆದರೆ (op_a_ == op_b_) res.ಗಣ_ಬಿಟ್(0, BIT4_1);
      ಇಲ್ಲದಿದ್ದರೆ res.ಗಣ_ಬಿಟ್(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}

/* Real compare not equal. */
vvp_cmp_ne_real::vvp_cmp_ne_real()
{
}

ಶೂನ್ಯ vvp_cmp_ne_real::recv_real(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ ಬಿಟ್,
                                vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      vvp_vector4_t res (1);
      ಆದರೆ (op_a_ != op_b_) res.ಗಣ_ಬಿಟ್(0, BIT4_1);
      ಇಲ್ಲದಿದ್ದರೆ res.ಗಣ_ಬಿಟ್(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}

/* Real compare greater than or equal. */
vvp_cmp_ge_real::vvp_cmp_ge_real()
{
}

ಶೂನ್ಯ vvp_cmp_ge_real::recv_real(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ ಬಿಟ್,
                                vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      vvp_vector4_t res (1);
      ಆದರೆ (op_a_ >= op_b_) res.ಗಣ_ಬಿಟ್(0, BIT4_1);
      ಇಲ್ಲದಿದ್ದರೆ res.ಗಣ_ಬಿಟ್(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}

/* Real compare greater than. */
vvp_cmp_gt_real::vvp_cmp_gt_real()
{
}

ಶೂನ್ಯ vvp_cmp_gt_real::recv_real(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ ಬಿಟ್,
                                vvp_context_t)
{
      dispatch_ಕಾರ್ಯಾಂಶ(ptr, ಬಿಟ್);

      vvp_vector4_t res (1);
      ಆದರೆ (op_a_ > op_b_) res.ಗಣ_ಬಿಟ್(0, BIT4_1);
      ಇಲ್ಲದಿದ್ದರೆ res.ಗಣ_ಬಿಟ್(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}
