#ifndef IVL_arith_H
#define IVL_arith_H
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
class vvp_arith_  : જાહેર vvp_net_fun_t {

    જાહેર:
      explicit vvp_arith_(unsigned wid);

      શૂન્ય recv_vec4_pv(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;

    સુરક્ષિત:
      શૂન્ય dispatch_operand_(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit);

    સુરક્ષિત:
      unsigned wid_;

      vvp_vector4_t op_a_;
      vvp_vector4_t op_b_;
	// Precalculated X result for propagation.
      vvp_vector4_t x_val_;
};

class vvp_arith_abs : જાહેર vvp_net_fun_t {
    જાહેર:
      explicit vvp_arith_abs();
      ~vvp_arith_abs() override;

      શૂન્ય recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                     vvp_context_t) override;
      શૂન્ય recv_real(vvp_net_ptr_t ptr, દ્વિગુણ bit,
                     vvp_context_t) override;

      શૂન્ય recv_vec4_pv(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;

    ખાનગી:
};

class vvp_arith_cast_int : જાહેર vvp_net_fun_t {
    જાહેર:
      explicit vvp_arith_cast_int(unsigned wid);
      ~vvp_arith_cast_int() override;

      શૂન્ય recv_real(vvp_net_ptr_t ptr, દ્વિગુણ bit,
                     vvp_context_t) override;

    ખાનગી:
      unsigned wid_;
};

class vvp_arith_cast_real : જાહેર vvp_net_fun_t {
    જાહેર:
      explicit vvp_arith_cast_real(bool signed_flag);
      ~vvp_arith_cast_real() override;

      શૂન્ય recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                     vvp_context_t) override;

      શૂન્ય recv_vec4_pv(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;

    ખાનગી:
      bool signed_;
};

class vvp_arith_cast_vec2 : જાહેર vvp_net_fun_t {
    જાહેર:
      explicit vvp_arith_cast_vec2(unsigned wid);
      ~vvp_arith_cast_vec2() override;

      શૂન્ય recv_real(vvp_net_ptr_t ptr, દ્વિગુણ bit,
                     vvp_context_t) override;
      શૂન્ય recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                     vvp_context_t) override;

      શૂન્ય recv_vec4_pv(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;

    ખાનગી:
      unsigned wid_;
};

class vvp_arith_div : જાહેર vvp_arith_ {

    જાહેર:
      explicit vvp_arith_div(unsigned wid, bool signed_flag);
      ~vvp_arith_div() override;
      શૂન્ય recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                     vvp_context_t) override;
    ખાનગી:
      શૂન્ય wide4_(vvp_net_ptr_t ptr);
      bool signed_flag_;
};

class vvp_arith_mod : જાહેર vvp_arith_ {

    જાહેર:
      explicit vvp_arith_mod(unsigned wid, bool signed_flag);
      ~vvp_arith_mod() override;
      શૂન્ય recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                     vvp_context_t) override;
    ખાનગી:
      શૂન્ય wide_(vvp_net_ptr_t ptr);
      bool signed_flag_;
};

/* vvp_cmp_* objects...
 * the vvp_cmp_* objects all are special vvp_arith_ objects in that
 * their widths are only for their inputs. The output widths are all
 * exactly 1 bit.
 */

class vvp_cmp_eeq  : જાહેર vvp_arith_ {

    જાહેર:
      explicit vvp_cmp_eeq(unsigned wid);
      શૂન્ય recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_nee  : જાહેર vvp_arith_ {

    જાહેર:
      explicit vvp_cmp_nee(unsigned wid);
      શૂન્ય recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_eq  : જાહેર vvp_arith_ {

    જાહેર:
      explicit vvp_cmp_eq(unsigned wid);
      શૂન્ય recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_eqx  : જાહેર vvp_arith_ {

    જાહેર:
      explicit vvp_cmp_eqx(unsigned wid);
      શૂન્ય recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_eqz  : જાહેર vvp_arith_ {

    જાહેર:
      explicit vvp_cmp_eqz(unsigned wid);
      શૂન્ય recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_ne  : જાહેર vvp_arith_ {

    જાહેર:
      explicit vvp_cmp_ne(unsigned wid);
      શૂન્ય recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_weq  : જાહેર vvp_arith_ {

    જાહેર:
      explicit vvp_cmp_weq(unsigned wid);
      શૂન્ય recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_wne  : જાહેર vvp_arith_ {

    જાહેર:
      explicit vvp_cmp_wne(unsigned wid);
      શૂન્ય recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                     vvp_context_t) override;

};


/*
 * This base class implements both GT and GE comparisons. The derived
 * GT and GE call the recv_vec4_base_ method with a different
 * out_if_equal argument that reflects the different expectations.
 */
class vvp_cmp_gtge_base_ : જાહેર vvp_arith_ {

    જાહેર:
      explicit vvp_cmp_gtge_base_(unsigned wid, bool signed_flag);

    સુરક્ષિત:
      શૂન્ય recv_vec4_base_(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
			   vvp_bit4_t out_if_equal);
    ખાનગી:
      bool signed_flag_;
};

class vvp_cmp_ge  : જાહેર vvp_cmp_gtge_base_ {

    જાહેર:
      explicit vvp_cmp_ge(unsigned wid, bool signed_flag);

      શૂન્ય recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_gt  : જાહેર vvp_cmp_gtge_base_ {

    જાહેર:
      explicit vvp_cmp_gt(unsigned wid, bool signed_flag);

      શૂન્ય recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                     vvp_context_t) override;
};

/*
 * NOTE: The inputs to the vvp_arith_mult are not necessarily the same
 * width as the output. This is different from the typical vvp_arith_
 * object. Perhaps that means this isn't quite a vvp_arith_ object?
 */
class vvp_arith_mult  : જાહેર vvp_arith_ {

    જાહેર:
      explicit vvp_arith_mult(unsigned wid);
      ~vvp_arith_mult() override;
      શૂન્ય recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                     vvp_context_t) override;
    ખાનગી:
      શૂન્ય wide_(vvp_net_ptr_t ptr);
};

class vvp_arith_pow  : જાહેર vvp_arith_ {

    જાહેર:
      explicit vvp_arith_pow(unsigned wid, bool signed_flag);
      ~vvp_arith_pow() override;
      શૂન્ય recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                     vvp_context_t) override;
    ખાનગી:
      bool signed_flag_;
};

class vvp_arith_sub  : જાહેર vvp_arith_ {

    જાહેર:
      explicit vvp_arith_sub(unsigned wid);
      ~vvp_arith_sub() override;
      આભાસી શૂન્ય recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                             vvp_context_t) override;

};

class vvp_arith_sum  : જાહેર vvp_arith_ {

    જાહેર:
      explicit vvp_arith_sum(unsigned wid);
      ~vvp_arith_sum() override;
      આભાસી શૂન્ય recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                             vvp_context_t) override;

};

class vvp_shiftl  : જાહેર vvp_arith_ {

    જાહેર:
      explicit vvp_shiftl(unsigned wid);
      ~vvp_shiftl() override;
      આભાસી શૂન્ય recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                             vvp_context_t) override;
};

class vvp_shiftr  : જાહેર vvp_arith_ {

    જાહેર:
      explicit vvp_shiftr(unsigned wid, bool signed_flag);
      ~vvp_shiftr() override;
      આભાસી શૂન્ય recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                             vvp_context_t) override;

    ખાનગી:
      bool signed_flag_;
};

/*
 * Base class for real valued expressions. These are similar to the
 * vector expression classes, but the inputs are collected from the
 * recv_real method.
 */
class vvp_arith_real_  : જાહેર vvp_net_fun_t {

    જાહેર:
      explicit vvp_arith_real_();

    સુરક્ષિત:
      શૂન્ય dispatch_operand_(vvp_net_ptr_t ptr, દ્વિગુણ bit);

    સુરક્ષિત:
      દ્વિગુણ op_a_;
      દ્વિગુણ op_b_;
};


class vvp_arith_sum_real : જાહેર vvp_arith_real_ {

    જાહેર:
      explicit vvp_arith_sum_real();
      ~vvp_arith_sum_real() override;
      શૂન્ય recv_real(vvp_net_ptr_t ptr, દ્વિગુણ bit,
                     vvp_context_t) override;
};

class vvp_arith_div_real : જાહેર vvp_arith_real_ {

    જાહેર:
      explicit vvp_arith_div_real();
      ~vvp_arith_div_real() override;
      શૂન્ય recv_real(vvp_net_ptr_t ptr, દ્વિગુણ bit,
                     vvp_context_t) override;
};

class vvp_arith_mod_real : જાહેર vvp_arith_real_ {

    જાહેર:
      explicit vvp_arith_mod_real();
      ~vvp_arith_mod_real() override;
      શૂન્ય recv_real(vvp_net_ptr_t ptr, દ્વિગુણ bit,
                     vvp_context_t) override;
};

class vvp_arith_mult_real : જાહેર vvp_arith_real_ {

    જાહેર:
      explicit vvp_arith_mult_real();
      ~vvp_arith_mult_real() override;
      શૂન્ય recv_real(vvp_net_ptr_t ptr, દ્વિગુણ bit,
                     vvp_context_t) override;
};

class vvp_arith_pow_real : જાહેર vvp_arith_real_ {

    જાહેર:
      explicit vvp_arith_pow_real();
      ~vvp_arith_pow_real() override;
      શૂન્ય recv_real(vvp_net_ptr_t ptr, દ્વિગુણ bit,
                     vvp_context_t) override;
};

class vvp_arith_sub_real : જાહેર vvp_arith_real_ {

    જાહેર:
      explicit vvp_arith_sub_real();
      ~vvp_arith_sub_real() override;
      શૂન્ય recv_real(vvp_net_ptr_t ptr, દ્વિગુણ bit,
                     vvp_context_t) override;
};

class vvp_cmp_eq_real  : જાહેર vvp_arith_real_ {

    જાહેર:
      explicit vvp_cmp_eq_real();
      શૂન્ય recv_real(vvp_net_ptr_t ptr, અચલ દ્વિગુણ bit,
                     vvp_context_t) override;
};

class vvp_cmp_ne_real  : જાહેર vvp_arith_real_ {

    જાહેર:
      explicit vvp_cmp_ne_real();
      શૂન્ય recv_real(vvp_net_ptr_t ptr, અચલ દ્વિગુણ bit,
                     vvp_context_t) override;
};

class vvp_cmp_ge_real  : જાહેર vvp_arith_real_ {

    જાહેર:
      explicit vvp_cmp_ge_real();
      શૂન્ય recv_real(vvp_net_ptr_t ptr, અચલ દ્વિગુણ bit,
                     vvp_context_t) override;
};

class vvp_cmp_gt_real  : જાહેર vvp_arith_real_ {

    જાહેર:
      explicit vvp_cmp_gt_real();
      શૂન્ય recv_real(vvp_net_ptr_t ptr, અચલ દ્વિગુણ bit,
                     vvp_context_t) override;
};

#endif /* IVL_arith_H */
