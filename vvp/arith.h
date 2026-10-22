#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_arith_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_arith_H
/*
 * Copyright (c) 2001-2025 Stephen Williams (steve@icarus.com)
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

# include  "vvp_net.h"

/*
 * Base class for arithmetic functors.
 * The wid constructor is used to size the output. This includes
 * precalculating an X value. Most arithmetic nodes can handle
 * whatever width comes in, given the knowledge of the output width.
 *
 * The width is also used to make initial values for the op_a_ and
 * op_b_ operands. Most arithmetic operators expect the widths of the
 * inputs to match, and since only one input at a time changes, the
 * other will need to be initialized to X.
 */
class vvp_arith_  : ਜਨਤਕ vvp_net_fun_t {

    ਜਨਤਕ:
      explicit vvp_arith_(ਬਿਨਾਨਿਸ਼ਾਨੀ wid);

      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx) override;

    ਸੁਰੱਖਿਅਤ:
      ਖਾਲੀ dispatch_ਸੰਕਾਰਜ(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ);

    ਸੁਰੱਖਿਅਤ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid_;

      vvp_vector4_t op_a_;
      vvp_vector4_t op_b_;
	// Precalculated X result for propagation.
      vvp_vector4_t x_val_;
};

class vvp_arith_abs : ਜਨਤਕ vvp_net_fun_t {
    ਜਨਤਕ:
      explicit vvp_arith_abs();
      ~vvp_arith_abs() override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
      ਖਾਲੀ recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx) override;

    ਨਿਜੀ:
};

class vvp_arith_cast_int : ਜਨਤਕ vvp_net_fun_t {
    ਜਨਤਕ:
      explicit vvp_arith_cast_int(ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ~vvp_arith_cast_int() override;

      ਖਾਲੀ recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

    ਨਿਜੀ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid_;
};

class vvp_arith_cast_real : ਜਨਤਕ vvp_net_fun_t {
    ਜਨਤਕ:
      explicit vvp_arith_cast_real(bool signed_flag);
      ~vvp_arith_cast_real() override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx) override;

    ਨਿਜੀ:
      bool signed_;
};

class vvp_arith_cast_vec2 : ਜਨਤਕ vvp_net_fun_t {
    ਜਨਤਕ:
      explicit vvp_arith_cast_vec2(ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ~vvp_arith_cast_vec2() override;

      ਖਾਲੀ recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx) override;

    ਨਿਜੀ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid_;
};

class vvp_arith_div : ਜਨਤਕ vvp_arith_ {

    ਜਨਤਕ:
      explicit vvp_arith_div(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, bool signed_flag);
      ~vvp_arith_div() override;
      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
    ਨਿਜੀ:
      ਖਾਲੀ wide4_(vvp_net_ptr_t ptr);
      bool signed_flag_;
};

class vvp_arith_mod : ਜਨਤਕ vvp_arith_ {

    ਜਨਤਕ:
      explicit vvp_arith_mod(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, bool signed_flag);
      ~vvp_arith_mod() override;
      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
    ਨਿਜੀ:
      ਖਾਲੀ wide_(vvp_net_ptr_t ptr);
      bool signed_flag_;
};

/* vvp_cmp_* objects...
 * the vvp_cmp_* objects all are special vvp_arith_ objects in that
 * their widths are only for their inputs. The output widths are all
 * exactly 1 bit.
 */

class vvp_cmp_eeq  : ਜਨਤਕ vvp_arith_ {

    ਜਨਤਕ:
      explicit vvp_cmp_eeq(ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

};

class vvp_cmp_nee  : ਜਨਤਕ vvp_arith_ {

    ਜਨਤਕ:
      explicit vvp_cmp_nee(ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

};

class vvp_cmp_eq  : ਜਨਤਕ vvp_arith_ {

    ਜਨਤਕ:
      explicit vvp_cmp_eq(ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

};

class vvp_cmp_eqx  : ਜਨਤਕ vvp_arith_ {

    ਜਨਤਕ:
      explicit vvp_cmp_eqx(ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

};

class vvp_cmp_eqz  : ਜਨਤਕ vvp_arith_ {

    ਜਨਤਕ:
      explicit vvp_cmp_eqz(ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

};

class vvp_cmp_ne  : ਜਨਤਕ vvp_arith_ {

    ਜਨਤਕ:
      explicit vvp_cmp_ne(ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

};

class vvp_cmp_weq  : ਜਨਤਕ vvp_arith_ {

    ਜਨਤਕ:
      explicit vvp_cmp_weq(ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

};

class vvp_cmp_wne  : ਜਨਤਕ vvp_arith_ {

    ਜਨਤਕ:
      explicit vvp_cmp_wne(ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

};


/*
 * This base class implements both GT and GE comparisons. The derived
 * GT and GE call the recv_vec4_base_ method with a different
 * out_if_equal argument that reflects the different expectations.
 */
class vvp_cmp_gtge_ਅਧਾਰ : ਜਨਤਕ vvp_arith_ {

    ਜਨਤਕ:
      explicit vvp_cmp_gtge_ਅਧਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, bool signed_flag);

    ਸੁਰੱਖਿਅਤ:
      ਖਾਲੀ recv_vec4_ਅਧਾਰ(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			   vvp_bit4_t ਬਾਹਰਲਾ_if_equal);
    ਨਿਜੀ:
      bool signed_flag_;
};

class vvp_cmp_ge  : ਜਨਤਕ vvp_cmp_gtge_ਅਧਾਰ {

    ਜਨਤਕ:
      explicit vvp_cmp_ge(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, bool signed_flag);

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

};

class vvp_cmp_gt  : ਜਨਤਕ vvp_cmp_gtge_ਅਧਾਰ {

    ਜਨਤਕ:
      explicit vvp_cmp_gt(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, bool signed_flag);

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
};

/*
 * NOTE: The inputs to the vvp_arith_mult are not necessarily the same
 * width as the output. This is different from the typical vvp_arith_
 * object. Perhaps that means this isn't quite a vvp_arith_ object?
 */
class vvp_arith_mult  : ਜਨਤਕ vvp_arith_ {

    ਜਨਤਕ:
      explicit vvp_arith_mult(ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ~vvp_arith_mult() override;
      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
    ਨਿਜੀ:
      ਖਾਲੀ wide_(vvp_net_ptr_t ptr);
};

class vvp_arith_pow  : ਜਨਤਕ vvp_arith_ {

    ਜਨਤਕ:
      explicit vvp_arith_pow(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, bool signed_flag);
      ~vvp_arith_pow() override;
      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
    ਨਿਜੀ:
      bool signed_flag_;
};

class vvp_arith_sub  : ਜਨਤਕ vvp_arith_ {

    ਜਨਤਕ:
      explicit vvp_arith_sub(ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ~vvp_arith_sub() override;
      ਆਭਾਸੀ ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                             vvp_context_t) override;

};

class vvp_arith_ਜੋੜ  : ਜਨਤਕ vvp_arith_ {

    ਜਨਤਕ:
      explicit vvp_arith_ਜੋੜ(ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ~vvp_arith_ਜੋੜ() override;
      ਆਭਾਸੀ ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                             vvp_context_t) override;

};

class vvp_shiftl  : ਜਨਤਕ vvp_arith_ {

    ਜਨਤਕ:
      explicit vvp_shiftl(ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ~vvp_shiftl() override;
      ਆਭਾਸੀ ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                             vvp_context_t) override;
};

class vvp_shiftr  : ਜਨਤਕ vvp_arith_ {

    ਜਨਤਕ:
      explicit vvp_shiftr(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, bool signed_flag);
      ~vvp_shiftr() override;
      ਆਭਾਸੀ ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                             vvp_context_t) override;

    ਨਿਜੀ:
      bool signed_flag_;
};

/*
 * Base class for real valued expressions. These are similar to the
 * vector expression classes, but the inputs are collected from the
 * recv_real method.
 */
class vvp_arith_real_  : ਜਨਤਕ vvp_net_fun_t {

    ਜਨਤਕ:
      explicit vvp_arith_real_();

    ਸੁਰੱਖਿਅਤ:
      ਖਾਲੀ dispatch_ਸੰਕਾਰਜ(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ);

    ਸੁਰੱਖਿਅਤ:
      ਦੁੱਗਣਾ op_a_;
      ਦੁੱਗਣਾ op_b_;
};


class vvp_arith_ਜੋੜ_real : ਜਨਤਕ vvp_arith_real_ {

    ਜਨਤਕ:
      explicit vvp_arith_ਜੋੜ_real();
      ~vvp_arith_ਜੋੜ_real() override;
      ਖਾਲੀ recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
};

class vvp_arith_div_real : ਜਨਤਕ vvp_arith_real_ {

    ਜਨਤਕ:
      explicit vvp_arith_div_real();
      ~vvp_arith_div_real() override;
      ਖਾਲੀ recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
};

class vvp_arith_mod_real : ਜਨਤਕ vvp_arith_real_ {

    ਜਨਤਕ:
      explicit vvp_arith_mod_real();
      ~vvp_arith_mod_real() override;
      ਖਾਲੀ recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
};

class vvp_arith_mult_real : ਜਨਤਕ vvp_arith_real_ {

    ਜਨਤਕ:
      explicit vvp_arith_mult_real();
      ~vvp_arith_mult_real() override;
      ਖਾਲੀ recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
};

class vvp_arith_pow_real : ਜਨਤਕ vvp_arith_real_ {

    ਜਨਤਕ:
      explicit vvp_arith_pow_real();
      ~vvp_arith_pow_real() override;
      ਖਾਲੀ recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
};

class vvp_arith_sub_real : ਜਨਤਕ vvp_arith_real_ {

    ਜਨਤਕ:
      explicit vvp_arith_sub_real();
      ~vvp_arith_sub_real() override;
      ਖਾਲੀ recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
};

class vvp_cmp_eq_real  : ਜਨਤਕ vvp_arith_real_ {

    ਜਨਤਕ:
      explicit vvp_cmp_eq_real();
      ਖਾਲੀ recv_real(vvp_net_ptr_t ptr, ਅਚਲ ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
};

class vvp_cmp_ne_real  : ਜਨਤਕ vvp_arith_real_ {

    ਜਨਤਕ:
      explicit vvp_cmp_ne_real();
      ਖਾਲੀ recv_real(vvp_net_ptr_t ptr, ਅਚਲ ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
};

class vvp_cmp_ge_real  : ਜਨਤਕ vvp_arith_real_ {

    ਜਨਤਕ:
      explicit vvp_cmp_ge_real();
      ਖਾਲੀ recv_real(vvp_net_ptr_t ptr, ਅਚਲ ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
};

class vvp_cmp_gt_real  : ਜਨਤਕ vvp_arith_real_ {

    ਜਨਤਕ:
      explicit vvp_cmp_gt_real();
      ਖਾਲੀ recv_real(vvp_net_ptr_t ptr, ਅਚਲ ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
};

#ਜੇ_ਅੰਤ /* IVL_arith_H */
