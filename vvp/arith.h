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
class vvp_arith_  : عام vvp_net_fun_t {

    عام:
      explicit vvp_arith_(unsigned wid);

      فراغ recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;

    محمي:
      فراغ dispatch_operand_(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit);

    محمي:
      unsigned wid_;

      vvp_vector4_t op_a_;
      vvp_vector4_t op_b_;
	// Precalculated X result for propagation.
      vvp_vector4_t x_val_;
};

class vvp_arith_abs : عام vvp_net_fun_t {
    عام:
      explicit vvp_arith_abs();
      ~vvp_arith_abs() override;

      فراغ recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;
      فراغ recv_real(vvp_net_ptr_t ptr, مزدوج bit,
                     vvp_context_t) override;

      فراغ recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;

    خاص:
};

class vvp_arith_cast_int : عام vvp_net_fun_t {
    عام:
      explicit vvp_arith_cast_int(unsigned wid);
      ~vvp_arith_cast_int() override;

      فراغ recv_real(vvp_net_ptr_t ptr, مزدوج bit,
                     vvp_context_t) override;

    خاص:
      unsigned wid_;
};

class vvp_arith_cast_real : عام vvp_net_fun_t {
    عام:
      explicit vvp_arith_cast_real(bool signed_flag);
      ~vvp_arith_cast_real() override;

      فراغ recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;

      فراغ recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;

    خاص:
      bool signed_;
};

class vvp_arith_cast_vec2 : عام vvp_net_fun_t {
    عام:
      explicit vvp_arith_cast_vec2(unsigned wid);
      ~vvp_arith_cast_vec2() override;

      فراغ recv_real(vvp_net_ptr_t ptr, مزدوج bit,
                     vvp_context_t) override;
      فراغ recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;

      فراغ recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;

    خاص:
      unsigned wid_;
};

class vvp_arith_div : عام vvp_arith_ {

    عام:
      explicit vvp_arith_div(unsigned wid, bool signed_flag);
      ~vvp_arith_div() override;
      فراغ recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;
    خاص:
      فراغ wide4_(vvp_net_ptr_t ptr);
      bool signed_flag_;
};

class vvp_arith_mod : عام vvp_arith_ {

    عام:
      explicit vvp_arith_mod(unsigned wid, bool signed_flag);
      ~vvp_arith_mod() override;
      فراغ recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;
    خاص:
      فراغ wide_(vvp_net_ptr_t ptr);
      bool signed_flag_;
};

/* vvp_cmp_* objects...
 * the vvp_cmp_* objects all are special vvp_arith_ objects in that
 * their widths are only for their inputs. The output widths are all
 * exactly 1 bit.
 */

class vvp_cmp_eeq  : عام vvp_arith_ {

    عام:
      explicit vvp_cmp_eeq(unsigned wid);
      فراغ recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_nee  : عام vvp_arith_ {

    عام:
      explicit vvp_cmp_nee(unsigned wid);
      فراغ recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_eq  : عام vvp_arith_ {

    عام:
      explicit vvp_cmp_eq(unsigned wid);
      فراغ recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_eqx  : عام vvp_arith_ {

    عام:
      explicit vvp_cmp_eqx(unsigned wid);
      فراغ recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_eqz  : عام vvp_arith_ {

    عام:
      explicit vvp_cmp_eqz(unsigned wid);
      فراغ recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_ne  : عام vvp_arith_ {

    عام:
      explicit vvp_cmp_ne(unsigned wid);
      فراغ recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_weq  : عام vvp_arith_ {

    عام:
      explicit vvp_cmp_weq(unsigned wid);
      فراغ recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_wne  : عام vvp_arith_ {

    عام:
      explicit vvp_cmp_wne(unsigned wid);
      فراغ recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;

};


/*
 * This base class implements both GT and GE comparisons. The derived
 * GT and GE call the recv_vec4_base_ method with a different
 * out_if_equal argument that reflects the different expectations.
 */
class vvp_cmp_gtge_base_ : عام vvp_arith_ {

    عام:
      explicit vvp_cmp_gtge_base_(unsigned wid, bool signed_flag);

    محمي:
      فراغ recv_vec4_base_(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
			   vvp_bit4_t out_if_equal);
    خاص:
      bool signed_flag_;
};

class vvp_cmp_ge  : عام vvp_cmp_gtge_base_ {

    عام:
      explicit vvp_cmp_ge(unsigned wid, bool signed_flag);

      فراغ recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_gt  : عام vvp_cmp_gtge_base_ {

    عام:
      explicit vvp_cmp_gt(unsigned wid, bool signed_flag);

      فراغ recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;
};

/*
 * NOTE: The inputs to the vvp_arith_mult are not necessarily the same
 * width as the output. This is different from the typical vvp_arith_
 * object. Perhaps that means this isn't quite a vvp_arith_ object?
 */
class vvp_arith_mult  : عام vvp_arith_ {

    عام:
      explicit vvp_arith_mult(unsigned wid);
      ~vvp_arith_mult() override;
      فراغ recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;
    خاص:
      فراغ wide_(vvp_net_ptr_t ptr);
};

class vvp_arith_pow  : عام vvp_arith_ {

    عام:
      explicit vvp_arith_pow(unsigned wid, bool signed_flag);
      ~vvp_arith_pow() override;
      فراغ recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;
    خاص:
      bool signed_flag_;
};

class vvp_arith_sub  : عام vvp_arith_ {

    عام:
      explicit vvp_arith_sub(unsigned wid);
      ~vvp_arith_sub() override;
      virtual فراغ recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                             vvp_context_t) override;

};

class vvp_arith_sum  : عام vvp_arith_ {

    عام:
      explicit vvp_arith_sum(unsigned wid);
      ~vvp_arith_sum() override;
      virtual فراغ recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                             vvp_context_t) override;

};

class vvp_shiftl  : عام vvp_arith_ {

    عام:
      explicit vvp_shiftl(unsigned wid);
      ~vvp_shiftl() override;
      virtual فراغ recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                             vvp_context_t) override;
};

class vvp_shiftr  : عام vvp_arith_ {

    عام:
      explicit vvp_shiftr(unsigned wid, bool signed_flag);
      ~vvp_shiftr() override;
      virtual فراغ recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                             vvp_context_t) override;

    خاص:
      bool signed_flag_;
};

/*
 * Base class for real valued expressions. These are similar to the
 * vector expression classes, but the inputs are collected from the
 * recv_real method.
 */
class vvp_arith_real_  : عام vvp_net_fun_t {

    عام:
      explicit vvp_arith_real_();

    محمي:
      فراغ dispatch_operand_(vvp_net_ptr_t ptr, مزدوج bit);

    محمي:
      مزدوج op_a_;
      مزدوج op_b_;
};


class vvp_arith_sum_real : عام vvp_arith_real_ {

    عام:
      explicit vvp_arith_sum_real();
      ~vvp_arith_sum_real() override;
      فراغ recv_real(vvp_net_ptr_t ptr, مزدوج bit,
                     vvp_context_t) override;
};

class vvp_arith_div_real : عام vvp_arith_real_ {

    عام:
      explicit vvp_arith_div_real();
      ~vvp_arith_div_real() override;
      فراغ recv_real(vvp_net_ptr_t ptr, مزدوج bit,
                     vvp_context_t) override;
};

class vvp_arith_mod_real : عام vvp_arith_real_ {

    عام:
      explicit vvp_arith_mod_real();
      ~vvp_arith_mod_real() override;
      فراغ recv_real(vvp_net_ptr_t ptr, مزدوج bit,
                     vvp_context_t) override;
};

class vvp_arith_mult_real : عام vvp_arith_real_ {

    عام:
      explicit vvp_arith_mult_real();
      ~vvp_arith_mult_real() override;
      فراغ recv_real(vvp_net_ptr_t ptr, مزدوج bit,
                     vvp_context_t) override;
};

class vvp_arith_pow_real : عام vvp_arith_real_ {

    عام:
      explicit vvp_arith_pow_real();
      ~vvp_arith_pow_real() override;
      فراغ recv_real(vvp_net_ptr_t ptr, مزدوج bit,
                     vvp_context_t) override;
};

class vvp_arith_sub_real : عام vvp_arith_real_ {

    عام:
      explicit vvp_arith_sub_real();
      ~vvp_arith_sub_real() override;
      فراغ recv_real(vvp_net_ptr_t ptr, مزدوج bit,
                     vvp_context_t) override;
};

class vvp_cmp_eq_real  : عام vvp_arith_real_ {

    عام:
      explicit vvp_cmp_eq_real();
      فراغ recv_real(vvp_net_ptr_t ptr, ثابت مزدوج bit,
                     vvp_context_t) override;
};

class vvp_cmp_ne_real  : عام vvp_arith_real_ {

    عام:
      explicit vvp_cmp_ne_real();
      فراغ recv_real(vvp_net_ptr_t ptr, ثابت مزدوج bit,
                     vvp_context_t) override;
};

class vvp_cmp_ge_real  : عام vvp_arith_real_ {

    عام:
      explicit vvp_cmp_ge_real();
      فراغ recv_real(vvp_net_ptr_t ptr, ثابت مزدوج bit,
                     vvp_context_t) override;
};

class vvp_cmp_gt_real  : عام vvp_arith_real_ {

    عام:
      explicit vvp_cmp_gt_real();
      فراغ recv_real(vvp_net_ptr_t ptr, ثابت مزدوج bit,
                     vvp_context_t) override;
};

#endif /* IVL_arith_H */
