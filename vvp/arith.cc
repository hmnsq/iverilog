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

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

vvp_arith_::vvp_arith_(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: wid_(wid), op_a_(wid), op_b_(wid), x_val_(wid)
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid ;  idx += 1) {
	    op_a_ .ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_Z);
	    op_b_ .ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_Z);
	    x_val_.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_X);
      }
}

ਖਾਲੀ vvp_arith_::dispatch_ਸੰਕਾਰਜ(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ = ptr.ਪੋਰਟ();
      ਚੋਣ (ਪੋਰਟ) {
	  ਹਾਲਤ 0:
	    op_a_ = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;
	  ਹਾਲਤ 1:
	    op_b_ = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;
	  ਮੂਲ:
	    fprintf(stderr, "Unsupported port type %u.\n", ਪੋਰਟ);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }
}

ਖਾਲੀ vvp_arith_::recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid, ctx);
}

vvp_arith_abs::vvp_arith_abs()
{
}

vvp_arith_abs::~vvp_arith_abs()
{
}

ਖਾਲੀ vvp_arith_abs::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                              vvp_context_t)
{
      vvp_vector4_t out (ਦੋਆਧਾਰੀ_ਅੰਕ.size(), BIT4_0);;

      vvp_bit4_t cmp = compare_gtge_signed(ਦੋਆਧਾਰੀ_ਅੰਕ, out, BIT4_1);
      ਚੋਣ (cmp) {
	  ਹਾਲਤ BIT4_1: // bit >= 0
	    out = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_0: //  bit < 0
	    out = ~ਦੋਆਧਾਰੀ_ਅੰਕ;
	    out += 1;
	    ਰੋਕੋ;
	  ਮੂਲ: // There's an X.
	    out = vvp_vector4_t(ਦੋਆਧਾਰੀ_ਅੰਕ.size(), BIT4_X);
	    ਰੋਕੋ;
      }

      ptr.ptr()->send_vec4(out, 0);
}

ਖਾਲੀ vvp_arith_abs::recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid, ctx);
}

ਖਾਲੀ vvp_arith_abs::recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                              vvp_context_t)
{
      ਦੁੱਗਣਾ out = fabs(ਦੋਆਧਾਰੀ_ਅੰਕ);
      ptr.ptr()->send_real(out, 0);
}

vvp_arith_cast_int::vvp_arith_cast_int(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: wid_(wid)
{
}

vvp_arith_cast_int::~vvp_arith_cast_int()
{
}

ਖਾਲੀ vvp_arith_cast_int::recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                                   vvp_context_t)
{
      ptr.ptr()->send_vec4(vvp_vector4_t(wid_, ਦੋਆਧਾਰੀ_ਅੰਕ), 0);
}

vvp_arith_cast_real::vvp_arith_cast_real(ਤਾਰਕਿਕ signed_flag)
: signed_(signed_flag)
{
}

vvp_arith_cast_real::~vvp_arith_cast_real()
{
}

ਖਾਲੀ vvp_arith_cast_real::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                    vvp_context_t)
{
      ਦੁੱਗਣਾ val;
      vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਦੋਆਧਾਰੀ_ਅੰਕ, val, signed_);
      ptr.ptr()->send_real(val, 0);
}

ਖਾਲੀ vvp_arith_cast_real::recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid,
				       vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid, ctx);
}

vvp_arith_cast_vec2::vvp_arith_cast_vec2(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: wid_(wid)
{
}

vvp_arith_cast_vec2::~vvp_arith_cast_vec2()
{
}

ਖਾਲੀ vvp_arith_cast_vec2::recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                                   vvp_context_t)
{
      ptr.ptr()->send_vec4(vvp_vector4_t(wid_, ਦੋਆਧਾਰੀ_ਅੰਕ), 0);
}

ਖਾਲੀ vvp_arith_cast_vec2::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                    vvp_context_t)
{
      vvp_vector2_t tmp = vvp_vector2_t(ਦੋਆਧਾਰੀ_ਅੰਕ);
      ptr.ptr()->send_vec4(vector2_ਮੰਜ਼ਿਲ_vector4(tmp,wid_), 0);
}

ਖਾਲੀ vvp_arith_cast_vec2::recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				       ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid, ctx);
}

// Division

vvp_arith_div::vvp_arith_div(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਤਾਰਕਿਕ signed_flag)
: vvp_arith_(wid), signed_flag_(signed_flag)
{
}

vvp_arith_div::~vvp_arith_div()
{
}

ਖਾਲੀ vvp_arith_div::wide4_(vvp_net_ptr_t ptr)
{
      vvp_vector2_t a2 (op_a_, ਸੱਚ);
      ਜੇ (a2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ਵਾਪਸ;
      }

      vvp_vector2_t b2 (op_b_, ਸੱਚ);
      ਜੇ (b2.is_NaN() || b2.is_zero()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ਵਾਪਸ;
      }

      ਤਾਰਕਿਕ negate = ਝੂਠ;
      ਜੇ (signed_flag_) {
	    ਜੇ (a2.value(a2.size()-1)) {
		  a2 = -a2;
		  negate = ਸੱਚ;
            }
	    ਜੇ (b2.value(b2.size()-1)) {
		  b2 = -b2;
		  negate = !negate;
            }
      }
      vvp_vector2_t res = a2 / b2;
      ਜੇ (negate) res = -res;
      ptr.ptr()->send_vec4(vector2_ਮੰਜ਼ਿਲ_vector4(res, wid_), 0);
}

ਖਾਲੀ vvp_arith_div::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                              vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      ਜੇ (wid_ > 8 * ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)) {
	    wide4_(ptr);
	    ਵਾਪਸ ;
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ a;
      ਜੇ (! vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(op_a_, a)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ਵਾਪਸ;
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ b;
      ਜੇ (! vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(op_b_, b)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ਵਾਪਸ;
      }

      ਤਾਰਕਿਕ negate = ਝੂਠ;
	/* If we are doing signed divide, then take the sign out of
	   the operands for now, and remember to put the sign back
	   later. */
      ਜੇ (signed_flag_) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sign_ਨਕਾਬ = 0;
	    ਜੇ (op_a_.size() != 8 * ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)) {
		  sign_ਨਕਾਬ = -1UL << op_a_.size();
	    }
	    ਜੇ (op_a_.value(op_a_.size()-1)) {
		  a = (-a) & ~sign_ਨਕਾਬ;
		  negate = !negate;
	    }

	    sign_ਨਕਾਬ = 0;
	    ਜੇ (op_b_.size() != 8 * ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)) {
		  sign_ਨਕਾਬ = -1UL << op_b_.size();
	    }
	    ਜੇ (op_b_.value(op_b_.size()-1)) {
		  b = (-b) & ~sign_ਨਕਾਬ;
		  negate = ! negate;
	    }
      }

      ਜੇ (b == 0) {
	    vvp_vector4_t xval (wid_);
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid_ ;  idx += 1)
		  xval.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_X);

	    ptr.ptr()->send_vec4(xval, 0);
	    ਵਾਪਸ;
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ val = a / b;
      ਜੇ (negate)
	    val = -val;

      ਪ੍ਰਤਿਜ੍ਞਾ(wid_ <= 8*ਆਕਾਰ(val));

      vvp_vector4_t vval (wid_);
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid_ ;  idx += 1) {
	    ਜੇ (val & 1)
		  vval.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_1);
	    ਨਹੀਂਤਾਂ
		  vval.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_0);

	    val >>= 1;
      }

      ptr.ptr()->send_vec4(vval, 0);
}


vvp_arith_mod::vvp_arith_mod(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਤਾਰਕਿਕ sf)
: vvp_arith_(wid), signed_flag_(sf)
{
}

vvp_arith_mod::~vvp_arith_mod()
{
}

ਖਾਲੀ vvp_arith_mod::wide_(vvp_net_ptr_t ptr)
{
      vvp_vector2_t a2 (op_a_, ਸੱਚ);
      ਜੇ (a2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ਵਾਪਸ;
      }

      vvp_vector2_t b2 (op_b_, ਸੱਚ);
      ਜੇ (b2.is_NaN() || b2.is_zero()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ਵਾਪਸ;
      }

      ਤਾਰਕਿਕ negate = ਝੂਠ;
      ਜੇ (signed_flag_) {
	    ਜੇ (a2.value(a2.size()-1)) {
		  a2 = -a2;
		  negate = ਸੱਚ;
            }
	    ਜੇ (b2.value(b2.size()-1)) {
		  b2 = -b2;
            }
      }
      vvp_vector2_t res = a2 % b2;
      ਜੇ (negate) res = -res;
      ptr.ptr()->send_vec4(vector2_ਮੰਜ਼ਿਲ_vector4(res, res.size()), 0);
}

ਖਾਲੀ vvp_arith_mod::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                              vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      ਜੇ (wid_ > 8 * ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)) {
	    wide_(ptr);
	    ਵਾਪਸ ;
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ a;
      ਜੇ (! vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(op_a_, a)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ਵਾਪਸ;
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ b;
      ਜੇ (! vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(op_b_, b)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ਵਾਪਸ;
      }

      ਤਾਰਕਿਕ negate = ਝੂਠ;
	/* If we are doing signed divide, then take the sign out of
	   the operands for now, and remember to put the sign back
	   later. */
      ਜੇ (signed_flag_) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sign_ਨਕਾਬ = 0;
	    ਜੇ (op_a_.size() != 8 * ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)) {
		  sign_ਨਕਾਬ = -1UL << op_a_.size();
	    }
	    ਜੇ (op_a_.value(op_a_.size()-1)) {
		  a = (-a) & ~sign_ਨਕਾਬ;
		  negate = !negate;
	    }

	    sign_ਨਕਾਬ = 0;
	    ਜੇ (op_b_.size() != 8 * ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)) {
		  sign_ਨਕਾਬ = -1UL << op_b_.size();
	    }
	    ਜੇ (op_b_.value(op_b_.size()-1)) {
		  b = (-b) & ~sign_ਨਕਾਬ;
	    }
      }

      ਜੇ (b == 0) {
	    vvp_vector4_t xval (wid_);
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid_ ;  idx += 1)
		  xval.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_X);

	    ptr.ptr()->send_vec4(xval, 0);
	    ਵਾਪਸ;
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ val = a % b;
      ਜੇ (negate)
	    val = -val;

      ਪ੍ਰਤਿਜ੍ਞਾ(wid_ <= 8*ਆਕਾਰ(val));

      vvp_vector4_t vval (wid_);
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid_ ;  idx += 1) {
	    ਜੇ (val & 1)
		  vval.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_1);
	    ਨਹੀਂਤਾਂ
		  vval.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_0);

	    val >>= 1;
      }

      ptr.ptr()->send_vec4(vval, 0);
}


// Multiplication

vvp_arith_mult::vvp_arith_mult(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: vvp_arith_(wid)
{
}

vvp_arith_mult::~vvp_arith_mult()
{
}

ਖਾਲੀ vvp_arith_mult::wide_(vvp_net_ptr_t ptr)
{
      vvp_vector2_t a2 (op_a_, ਸੱਚ);
      vvp_vector2_t b2 (op_b_, ਸੱਚ);

      ਜੇ (a2.is_NaN() || b2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ਵਾਪਸ;
      }

      vvp_vector2_t ਨਤੀਜਾ = a2 * b2;

      vvp_vector4_t res4 = vector2_ਮੰਜ਼ਿਲ_vector4(ਨਤੀਜਾ, wid_);
      ptr.ptr()->send_vec4(res4, 0);
}

ਖਾਲੀ vvp_arith_mult::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                               vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      ਜੇ (wid_ > 8 * ਆਕਾਰ(int64_t)) {
	    wide_(ptr);
	    ਵਾਪਸ ;
      }

      int64_t a;
      ਜੇ (! vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(op_a_, a, ਝੂਠ, ਸੱਚ)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ਵਾਪਸ;
      }

      int64_t b;
      ਜੇ (! vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(op_b_, b, ਝੂਠ, ਸੱਚ)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ਵਾਪਸ;
      }

      int64_t val = a * b;
      ਪ੍ਰਤਿਜ੍ਞਾ(wid_ <= 8*ਆਕਾਰ(val));

      vvp_vector4_t vval (wid_);
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid_ ;  idx += 1) {
	    ਜੇ (val & 1)
		  vval.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_1);
	    ਨਹੀਂਤਾਂ
		  vval.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_0);

	    val >>= 1;
      }

      ptr.ptr()->send_vec4(vval, 0);
}


// Power

vvp_arith_pow::vvp_arith_pow(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਤਾਰਕਿਕ signed_flag)
: vvp_arith_(wid), signed_flag_(signed_flag)
{
}

vvp_arith_pow::~vvp_arith_pow()
{
}

ਖਾਲੀ vvp_arith_pow::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                              vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      vvp_vector2_t a2 (op_a_, ਸੱਚ);
      vvp_vector2_t b2 (op_b_, ਸੱਚ);

        // If we have an X or Z in the arguments return X.
      ਜੇ (a2.is_NaN() || b2.is_NaN()) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ਵਾਪਸ;
      }

	// Is the exponent negative? If so, table 5-6 in IEEE1364-2005
	// defines what value is returned.
      ਜੇ (signed_flag_ && b2.value(b2.size()-1)) {
	    ਪੂਰਨ a_val;
	    ਦੁੱਗਣਾ r_val = 0.0;
	    ਜੇ (vector2_ਮੰਜ਼ਿਲ_ਮੁੱਲ(a2, a_val, ਸੱਚ)) {
		  ਜੇ (a_val == 0) {
			ptr.ptr()->send_vec4(x_val_, 0);
			ਵਾਪਸ;
		  }
		  ਜੇ (a_val == 1) {
			r_val = 1.0;
		  }
		  ਜੇ (a_val == -1) {
			r_val = b2.value(0) ? -1.0 : 1.0;
		  }
	    }
	    ptr.ptr()->send_vec4(vvp_vector4_t(wid_, r_val), 0);
	    ਵਾਪਸ;
      }

      ptr.ptr()->send_vec4(vector2_ਮੰਜ਼ਿਲ_vector4(ਘਾਤ(a2, b2), wid_), 0);
}


// Addition

vvp_arith_ਜੋੜ::vvp_arith_ਜੋੜ(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: vvp_arith_(wid)
{
}

vvp_arith_ਜੋੜ::~vvp_arith_ਜੋੜ()
{
}

ਖਾਲੀ vvp_arith_ਜੋੜ::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                              vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      vvp_net_t*net = ptr.ptr();

      vvp_vector4_t value (wid_);

	/* Pad input vectors with this value to widen to the desired
	   output width. */
      ਅਚਲ vvp_bit4_t pad = BIT4_0;

      vvp_bit4_t ਹਾਸਲ = BIT4_0;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid_ ;  idx += 1) {
	    vvp_bit4_t a = (idx >= op_a_.size())? pad : op_a_.value(idx);
	    vvp_bit4_t b = (idx >= op_b_.size())? pad : op_b_.value(idx);
	    vvp_bit4_t cur = ਜੋੜੋ_with_ਹਾਸਲ(a, b, ਹਾਸਲ);

	    ਜੇ (cur == BIT4_X) {
		  net->send_vec4(x_val_, 0);
		  ਵਾਪਸ;
	    }

	    value.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, cur);
      }

      net->send_vec4(value, 0);
}

vvp_arith_sub::vvp_arith_sub(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
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
ਖਾਲੀ vvp_arith_sub::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                              vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      vvp_net_t*net = ptr.ptr();

      vvp_vector4_t value (wid_);

	/* Pad input vectors with this value to widen to the desired
	   output width. */
      ਅਚਲ vvp_bit4_t pad = BIT4_1;

      vvp_bit4_t ਹਾਸਲ = BIT4_1;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < wid_ ;  idx += 1) {
	    vvp_bit4_t a = (idx >= op_a_.size())? pad : op_a_.value(idx);
	    vvp_bit4_t b = (idx >= op_b_.size())? pad : ~op_b_.value(idx);
	    vvp_bit4_t cur = ਜੋੜੋ_with_ਹਾਸਲ(a, b, ਹਾਸਲ);

	    ਜੇ (cur == BIT4_X) {
		  net->send_vec4(x_val_, 0);
		  ਵਾਪਸ;
	    }

	    value.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, cur);
      }

      net->send_vec4(value, 0);
}

vvp_cmp_eeq::vvp_cmp_eeq(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: vvp_arith_(wid)
{
}

ਖਾਲੀ vvp_cmp_eeq::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                            vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      vvp_vector4_t eeq (1);
      eeq.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_1);

      ਪ੍ਰਤਿਜ੍ਞਾ(op_a_.size() == op_b_.size());
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < op_a_.size() ;  idx += 1)
	    ਜੇ (op_a_.value(idx) != op_b_.value(idx)) {
		  eeq.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_0);
		  ਰੋਕੋ;
	    }


      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}

vvp_cmp_nee::vvp_cmp_nee(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: vvp_arith_(wid)
{
}

ਖਾਲੀ vvp_cmp_nee::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                            vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      vvp_vector4_t eeq (1);
      eeq.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_0);

      ਪ੍ਰਤਿਜ੍ਞਾ(op_a_.size() == op_b_.size());
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < op_a_.size() ;  idx += 1)
	    ਜੇ (op_a_.value(idx) != op_b_.value(idx)) {
		  eeq.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_1);
		  ਰੋਕੋ;
	    }


      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}

vvp_cmp_eq::vvp_cmp_eq(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: vvp_arith_(wid)
{
}

/*
 * Compare Vector a and Vector b. If in any bit position the a and b
 * bits are known and different, then the result is 0. Otherwise, if
 * there are X/Z bits anywhere in A or B, the result is X. Finally,
 * the result is 1.
 */
ਖਾਲੀ vvp_cmp_eq::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                           vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      ਜੇ (op_a_.size() != op_b_.size()) {
	    cerr << "COMPARISON size mismatch. "
		 << "a=" << op_a_ << ", b=" << op_b_ << ਸਤਰ_ਅੰਤ;
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }

      vvp_vector4_t res (1);
      res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_1);

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    ਜੇ (a == BIT4_X)
		  res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_X);
	    ਨਹੀਂਤਾਂ ਜੇ (a == BIT4_Z)
		  res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_X);
	    ਨਹੀਂਤਾਂ ਜੇ (b == BIT4_X)
		  res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_X);
	    ਨਹੀਂਤਾਂ ਜੇ (b == BIT4_Z)
		  res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_X);
            ਨਹੀਂਤਾਂ ਜੇ (a != b) {
		  res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_0);
		  ਰੋਕੋ;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}

vvp_cmp_eqx::vvp_cmp_eqx(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: vvp_arith_(wid)
{
}

/*
 * Compare Vector a and Vector b. If in any bit position the a and b
 * bits are known and different, then the result is 0. Otherwise, if
 * there are X/Z bits anywhere in A or B, the result is X. Finally,
 * the result is 1.
 */
ਖਾਲੀ vvp_cmp_eqx::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                           vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      ਜੇ (op_a_.size() != op_b_.size()) {
	    cerr << "COMPARISON size mismatch. "
		 << "a=" << op_a_ << ", b=" << op_b_ << ਸਤਰ_ਅੰਤ;
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }

      vvp_vector4_t res (1);
      res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_1);

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    ਜੇ (b == BIT4_X)
		  ਜਾਰੀ;
	    ਜੇ (b == BIT4_Z)
		  ਜਾਰੀ;
	    ਜੇ (a != b) {
		  res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_0);
		  ਰੋਕੋ;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}

vvp_cmp_eqz::vvp_cmp_eqz(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: vvp_arith_(wid)
{
}

ਖਾਲੀ vvp_cmp_eqz::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                           vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      ਜੇ (op_a_.size() != op_b_.size()) {
	    cerr << "COMPARISON size mismatch. "
		 << "a=" << op_a_ << ", b=" << op_b_ << ਸਤਰ_ਅੰਤ;
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }

      vvp_vector4_t res (1);
      res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_1);

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    ਜੇ (b == BIT4_Z)
		  ਜਾਰੀ;
	    ਜੇ (a != b) {
		  res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_0);
		  ਰੋਕੋ;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}

vvp_cmp_ne::vvp_cmp_ne(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: vvp_arith_(wid)
{
}

/*
 * Compare Vector a and Vector b. If in any bit position the a and b
 * bits are known and different, then the result is 1. Otherwise, if
 * there are X/Z bits anywhere in A or B, the result is X. Finally,
 * the result is 0.
 */
ਖਾਲੀ vvp_cmp_ne::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                           vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      ਜੇ (op_a_.size() != op_b_.size()) {
	    cerr << "internal error: vvp_cmp_ne: op_a_=" << op_a_
		 << ", op_b_=" << op_b_ << ਸਤਰ_ਅੰਤ;
	    ਪ੍ਰਤਿਜ੍ਞਾ(op_a_.size() == op_b_.size());
      }

      vvp_vector4_t res (1);
      res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_0);

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);

	    ਜੇ (a == BIT4_X)
		  res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_X);
	    ਨਹੀਂਤਾਂ ਜੇ (a == BIT4_Z)
		  res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_X);
	    ਨਹੀਂਤਾਂ ਜੇ (b == BIT4_X)
		  res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_X);
	    ਨਹੀਂਤਾਂ ਜੇ (b == BIT4_Z)
		  res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_X);
            ਨਹੀਂਤਾਂ ਜੇ (a != b) {
		  res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_1);
		  ਰੋਕੋ;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(res, 0);
}


vvp_cmp_gtge_ਅਧਾਰ::vvp_cmp_gtge_ਅਧਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਤਾਰਕਿਕ flag)
: vvp_arith_(wid), signed_flag_(flag)
{
}


ਖਾਲੀ vvp_cmp_gtge_ਅਧਾਰ::recv_vec4_ਅਧਾਰ(vvp_net_ptr_t ptr,
					 ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
					 vvp_bit4_t ਬਾਹਰਲਾ_if_equal)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      vvp_bit4_t out = signed_flag_
	    ? compare_gtge_signed(op_a_, op_b_, ਬਾਹਰਲਾ_if_equal)
	    : compare_gtge(op_a_, op_b_, ਬਾਹਰਲਾ_if_equal);
      vvp_vector4_t val (1);
      val.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, out);
      ptr.ptr()->send_vec4(val, 0);

      ਵਾਪਸ;
}


vvp_cmp_ge::vvp_cmp_ge(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਤਾਰਕਿਕ flag)
: vvp_cmp_gtge_ਅਧਾਰ(wid, flag)
{
}

ਖਾਲੀ vvp_cmp_ge::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                           vvp_context_t)
{
      recv_vec4_ਅਧਾਰ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ, BIT4_1);
}

vvp_cmp_gt::vvp_cmp_gt(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਤਾਰਕਿਕ flag)
: vvp_cmp_gtge_ਅਧਾਰ(wid, flag)
{
}

ਖਾਲੀ vvp_cmp_gt::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                           vvp_context_t)
{
      recv_vec4_ਅਧਾਰ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ, BIT4_0);
}

vvp_cmp_weq::vvp_cmp_weq(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: vvp_arith_(wid)
{
}

ਖਾਲੀ vvp_cmp_weq::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                            vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      vvp_vector4_t eeq (1);
      eeq.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_1);

      ਪ੍ਰਤਿਜ੍ਞਾ(op_a_.size() == op_b_.size());
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);
	    ਜੇ (b == BIT4_X)
		  ਜਾਰੀ;
	    ਨਹੀਂਤਾਂ ਜੇ (b == BIT4_Z)
		  ਜਾਰੀ;
	    ਨਹੀਂਤਾਂ ਜੇ (a == BIT4_X)
		  eeq.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_X);
	    ਨਹੀਂਤਾਂ ਜੇ (a == BIT4_Z)
		  eeq.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_X);
            ਨਹੀਂਤਾਂ ਜੇ (a != b) {
		  eeq.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_0);
		  ਰੋਕੋ;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}

vvp_cmp_wne::vvp_cmp_wne(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: vvp_arith_(wid)
{
}

ਖਾਲੀ vvp_cmp_wne::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                            vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      vvp_vector4_t eeq (1);
      eeq.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_0);

      ਪ੍ਰਤਿਜ੍ਞਾ(op_a_.size() == op_b_.size());
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < op_a_.size() ;  idx += 1) {
	    vvp_bit4_t a = op_a_.value(idx);
	    vvp_bit4_t b = op_b_.value(idx);
	    ਜੇ (b == BIT4_X)
		  ਜਾਰੀ;
	    ਨਹੀਂਤਾਂ ਜੇ (b == BIT4_Z)
		  ਜਾਰੀ;
	    ਨਹੀਂਤਾਂ ਜੇ (a == BIT4_X)
		  eeq.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_X);
	    ਨਹੀਂਤਾਂ ਜੇ (a == BIT4_Z)
		  eeq.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_X);
            ਨਹੀਂਤਾਂ ਜੇ (a != b) {
		  eeq.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_1);
		  ਰੋਕੋ;
	    }
      }

      vvp_net_t*net = ptr.ptr();
      net->send_vec4(eeq, 0);
}


vvp_shiftl::vvp_shiftl(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: vvp_arith_(wid)
{
}

vvp_shiftl::~vvp_shiftl()
{
}

ਖਾਲੀ vvp_shiftl::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                           vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      vvp_vector4_t out (op_a_.size());

      ਤਾਰਕਿਕ ਉਛਾਲ_flag;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਖਿਸਕਾਓ = 0;
      ਜੇ (! vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(op_b_, ਉਛਾਲ_flag, ਖਿਸਕਾਓ)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ਵਾਪਸ;
      }

      ਜੇ (ਉਛਾਲ_flag || ਖਿਸਕਾਓ > out.size())
	    ਖਿਸਕਾਓ = out.size();

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਖਿਸਕਾਓ ;  idx += 1)
	    out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_0);

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = ਖਿਸਕਾਓ ;  idx < out.size() ;  idx += 1)
	    out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, op_a_.value(idx-ਖਿਸਕਾਓ));

      ptr.ptr()->send_vec4(out, 0);
}

vvp_shiftr::vvp_shiftr(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਤਾਰਕਿਕ signed_flag)
: vvp_arith_(wid), signed_flag_(signed_flag)
{
}

vvp_shiftr::~vvp_shiftr()
{
}

ਖਾਲੀ vvp_shiftr::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                           vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      vvp_vector4_t out (op_a_.size());

      ਤਾਰਕਿਕ ਉਛਾਲ_flag;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਖਿਸਕਾਓ = 0;
      ਜੇ (! vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(op_b_, ਉਛਾਲ_flag, ਖਿਸਕਾਓ)) {
	    ptr.ptr()->send_vec4(x_val_, 0);
	    ਵਾਪਸ;
      }

      ਜੇ (ਉਛਾਲ_flag || ਖਿਸਕਾਓ > out.size())
	    ਖਿਸਕਾਓ = out.size();

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = ਖਿਸਕਾਓ ;  idx < out.size() ;  idx += 1)
	    out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx-ਖਿਸਕਾਓ, op_a_.value(idx));

      vvp_bit4_t pad = BIT4_0;
      ਜੇ (signed_flag_ && op_a_.size() > 0)
	    pad = op_a_.value(op_a_.size()-1);

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਖਿਸਕਾਓ ;  idx += 1)
	    out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx+out.size()-ਖਿਸਕਾਓ, pad);

      ptr.ptr()->send_vec4(out, 0);
}


vvp_arith_real_::vvp_arith_real_()
{
      op_a_ = 0.0;
      op_b_ = 0.0;
}

ਖਾਲੀ vvp_arith_real_::dispatch_ਸੰਕਾਰਜ(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਚੋਣ (ptr.ਪੋਰਟ()) {
	  ਹਾਲਤ 0:
	    op_a_ = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;
	  ਹਾਲਤ 1:
	    op_b_ = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;
	  ਮੂਲ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.ਪੋਰਟ());
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }
}


/* Real multiplication. */
vvp_arith_mult_real::vvp_arith_mult_real()
{
}

vvp_arith_mult_real::~vvp_arith_mult_real()
{
}

ਖਾਲੀ vvp_arith_mult_real::recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                                    vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      ਦੁੱਗਣਾ val = op_a_ * op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real power. */
vvp_arith_pow_real::vvp_arith_pow_real()
{
}

vvp_arith_pow_real::~vvp_arith_pow_real()
{
}

ਖਾਲੀ vvp_arith_pow_real::recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                                   vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      ਦੁੱਗਣਾ val = ਘਾਤ(op_a_, op_b_);
      ptr.ptr()->send_real(val, 0);
}

/* Real division. */
vvp_arith_div_real::vvp_arith_div_real()
{
}

vvp_arith_div_real::~vvp_arith_div_real()
{
}

ਖਾਲੀ vvp_arith_div_real::recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                                   vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      ਦੁੱਗਣਾ val = op_a_ / op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real modulus. */
vvp_arith_mod_real::vvp_arith_mod_real()
{
}

vvp_arith_mod_real::~vvp_arith_mod_real()
{
}

ਖਾਲੀ vvp_arith_mod_real::recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                                   vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      ਦੁੱਗਣਾ val = fmod(op_a_, op_b_);
      ptr.ptr()->send_real(val, 0);
}

/* Real summation. */
vvp_arith_ਜੋੜ_real::vvp_arith_ਜੋੜ_real()
{
}

vvp_arith_ਜੋੜ_real::~vvp_arith_ਜੋੜ_real()
{
}

ਖਾਲੀ vvp_arith_ਜੋੜ_real::recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                                   vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      ਦੁੱਗਣਾ val = op_a_ + op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real subtraction. */
vvp_arith_sub_real::vvp_arith_sub_real()
{
}

vvp_arith_sub_real::~vvp_arith_sub_real()
{
}

ਖਾਲੀ vvp_arith_sub_real::recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                                   vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      ਦੁੱਗਣਾ val = op_a_ - op_b_;
      ptr.ptr()->send_real(val, 0);
}

/* Real compare equal. */
vvp_cmp_eq_real::vvp_cmp_eq_real()
{
}

ਖਾਲੀ vvp_cmp_eq_real::recv_real(vvp_net_ptr_t ptr, ਅਚਲ ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                                vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      vvp_vector4_t res (1);
      ਜੇ (op_a_ == op_b_) res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_1);
      ਨਹੀਂਤਾਂ res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}

/* Real compare not equal. */
vvp_cmp_ne_real::vvp_cmp_ne_real()
{
}

ਖਾਲੀ vvp_cmp_ne_real::recv_real(vvp_net_ptr_t ptr, ਅਚਲ ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                                vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      vvp_vector4_t res (1);
      ਜੇ (op_a_ != op_b_) res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_1);
      ਨਹੀਂਤਾਂ res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}

/* Real compare greater than or equal. */
vvp_cmp_ge_real::vvp_cmp_ge_real()
{
}

ਖਾਲੀ vvp_cmp_ge_real::recv_real(vvp_net_ptr_t ptr, ਅਚਲ ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                                vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      vvp_vector4_t res (1);
      ਜੇ (op_a_ >= op_b_) res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_1);
      ਨਹੀਂਤਾਂ res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}

/* Real compare greater than. */
vvp_cmp_gt_real::vvp_cmp_gt_real()
{
}

ਖਾਲੀ vvp_cmp_gt_real::recv_real(vvp_net_ptr_t ptr, ਅਚਲ ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                                vvp_context_t)
{
      dispatch_ਸੰਕਾਰਜ(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ);

      vvp_vector4_t res (1);
      ਜੇ (op_a_ > op_b_) res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_1);
      ਨਹੀਂਤਾਂ res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, BIT4_0);

      ptr.ptr()->send_vec4(res, 0);
}
