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
class vvp_arith_  : सार्वजनिक vvp_net_fun_t {

    सार्वजनिक:
      explicit vvp_arith_(unsigned wid);

      शून्य recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;

    रक्षित:
      शून्य dispatch_operand_(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit);

    रक्षित:
      unsigned wid_;

      vvp_vector4_t op_a_;
      vvp_vector4_t op_b_;
	// Precalculated X result for propagation.
      vvp_vector4_t x_val_;
};

class vvp_arith_abs : सार्वजनिक vvp_net_fun_t {
    सार्वजनिक:
      explicit vvp_arith_abs();
      ~vvp_arith_abs() override;

      शून्य recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;
      शून्य recv_real(vvp_net_ptr_t ptr, द्विगुण bit,
                     vvp_context_t) override;

      शून्य recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;

    गुप्त:
};

class vvp_arith_cast_int : सार्वजनिक vvp_net_fun_t {
    सार्वजनिक:
      explicit vvp_arith_cast_int(unsigned wid);
      ~vvp_arith_cast_int() override;

      शून्य recv_real(vvp_net_ptr_t ptr, द्विगुण bit,
                     vvp_context_t) override;

    गुप्त:
      unsigned wid_;
};

class vvp_arith_cast_real : सार्वजनिक vvp_net_fun_t {
    सार्वजनिक:
      explicit vvp_arith_cast_real(bool signed_flag);
      ~vvp_arith_cast_real() override;

      शून्य recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;

      शून्य recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;

    गुप्त:
      bool signed_;
};

class vvp_arith_cast_vec2 : सार्वजनिक vvp_net_fun_t {
    सार्वजनिक:
      explicit vvp_arith_cast_vec2(unsigned wid);
      ~vvp_arith_cast_vec2() override;

      शून्य recv_real(vvp_net_ptr_t ptr, द्विगुण bit,
                     vvp_context_t) override;
      शून्य recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;

      शून्य recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;

    गुप्त:
      unsigned wid_;
};

class vvp_arith_div : सार्वजनिक vvp_arith_ {

    सार्वजनिक:
      explicit vvp_arith_div(unsigned wid, bool signed_flag);
      ~vvp_arith_div() override;
      शून्य recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;
    गुप्त:
      शून्य wide4_(vvp_net_ptr_t ptr);
      bool signed_flag_;
};

class vvp_arith_mod : सार्वजनिक vvp_arith_ {

    सार्वजनिक:
      explicit vvp_arith_mod(unsigned wid, bool signed_flag);
      ~vvp_arith_mod() override;
      शून्य recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;
    गुप्त:
      शून्य wide_(vvp_net_ptr_t ptr);
      bool signed_flag_;
};

/* vvp_cmp_* objects...
 * the vvp_cmp_* objects all are special vvp_arith_ objects in that
 * their widths are only for their inputs. The output widths are all
 * exactly 1 bit.
 */

class vvp_cmp_eeq  : सार्वजनिक vvp_arith_ {

    सार्वजनिक:
      explicit vvp_cmp_eeq(unsigned wid);
      शून्य recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_nee  : सार्वजनिक vvp_arith_ {

    सार्वजनिक:
      explicit vvp_cmp_nee(unsigned wid);
      शून्य recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_eq  : सार्वजनिक vvp_arith_ {

    सार्वजनिक:
      explicit vvp_cmp_eq(unsigned wid);
      शून्य recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_eqx  : सार्वजनिक vvp_arith_ {

    सार्वजनिक:
      explicit vvp_cmp_eqx(unsigned wid);
      शून्य recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_eqz  : सार्वजनिक vvp_arith_ {

    सार्वजनिक:
      explicit vvp_cmp_eqz(unsigned wid);
      शून्य recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_ne  : सार्वजनिक vvp_arith_ {

    सार्वजनिक:
      explicit vvp_cmp_ne(unsigned wid);
      शून्य recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_weq  : सार्वजनिक vvp_arith_ {

    सार्वजनिक:
      explicit vvp_cmp_weq(unsigned wid);
      शून्य recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_wne  : सार्वजनिक vvp_arith_ {

    सार्वजनिक:
      explicit vvp_cmp_wne(unsigned wid);
      शून्य recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;

};


/*
 * This base class implements both GT and GE comparisons. The derived
 * GT and GE call the recv_vec4_base_ method with a different
 * out_if_equal argument that reflects the different expectations.
 */
class vvp_cmp_gtge_base_ : सार्वजनिक vvp_arith_ {

    सार्वजनिक:
      explicit vvp_cmp_gtge_base_(unsigned wid, bool signed_flag);

    रक्षित:
      शून्य recv_vec4_base_(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
			   vvp_bit4_t out_if_equal);
    गुप्त:
      bool signed_flag_;
};

class vvp_cmp_ge  : सार्वजनिक vvp_cmp_gtge_base_ {

    सार्वजनिक:
      explicit vvp_cmp_ge(unsigned wid, bool signed_flag);

      शून्य recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_gt  : सार्वजनिक vvp_cmp_gtge_base_ {

    सार्वजनिक:
      explicit vvp_cmp_gt(unsigned wid, bool signed_flag);

      शून्य recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;
};

/*
 * NOTE: The inputs to the vvp_arith_mult are not necessarily the same
 * width as the output. This is different from the typical vvp_arith_
 * object. Perhaps that means this isn't quite a vvp_arith_ object?
 */
class vvp_arith_mult  : सार्वजनिक vvp_arith_ {

    सार्वजनिक:
      explicit vvp_arith_mult(unsigned wid);
      ~vvp_arith_mult() override;
      शून्य recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;
    गुप्त:
      शून्य wide_(vvp_net_ptr_t ptr);
};

class vvp_arith_pow  : सार्वजनिक vvp_arith_ {

    सार्वजनिक:
      explicit vvp_arith_pow(unsigned wid, bool signed_flag);
      ~vvp_arith_pow() override;
      शून्य recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;
    गुप्त:
      bool signed_flag_;
};

class vvp_arith_sub  : सार्वजनिक vvp_arith_ {

    सार्वजनिक:
      explicit vvp_arith_sub(unsigned wid);
      ~vvp_arith_sub() override;
      आभास शून्य recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                             vvp_context_t) override;

};

class vvp_arith_sum  : सार्वजनिक vvp_arith_ {

    सार्वजनिक:
      explicit vvp_arith_sum(unsigned wid);
      ~vvp_arith_sum() override;
      आभास शून्य recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                             vvp_context_t) override;

};

class vvp_shiftl  : सार्वजनिक vvp_arith_ {

    सार्वजनिक:
      explicit vvp_shiftl(unsigned wid);
      ~vvp_shiftl() override;
      आभास शून्य recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                             vvp_context_t) override;
};

class vvp_shiftr  : सार्वजनिक vvp_arith_ {

    सार्वजनिक:
      explicit vvp_shiftr(unsigned wid, bool signed_flag);
      ~vvp_shiftr() override;
      आभास शून्य recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                             vvp_context_t) override;

    गुप्त:
      bool signed_flag_;
};

/*
 * Base class for real valued expressions. These are similar to the
 * vector expression classes, but the inputs are collected from the
 * recv_real method.
 */
class vvp_arith_real_  : सार्वजनिक vvp_net_fun_t {

    सार्वजनिक:
      explicit vvp_arith_real_();

    रक्षित:
      शून्य dispatch_operand_(vvp_net_ptr_t ptr, द्विगुण bit);

    रक्षित:
      द्विगुण op_a_;
      द्विगुण op_b_;
};


class vvp_arith_sum_real : सार्वजनिक vvp_arith_real_ {

    सार्वजनिक:
      explicit vvp_arith_sum_real();
      ~vvp_arith_sum_real() override;
      शून्य recv_real(vvp_net_ptr_t ptr, द्विगुण bit,
                     vvp_context_t) override;
};

class vvp_arith_div_real : सार्वजनिक vvp_arith_real_ {

    सार्वजनिक:
      explicit vvp_arith_div_real();
      ~vvp_arith_div_real() override;
      शून्य recv_real(vvp_net_ptr_t ptr, द्विगुण bit,
                     vvp_context_t) override;
};

class vvp_arith_mod_real : सार्वजनिक vvp_arith_real_ {

    सार्वजनिक:
      explicit vvp_arith_mod_real();
      ~vvp_arith_mod_real() override;
      शून्य recv_real(vvp_net_ptr_t ptr, द्विगुण bit,
                     vvp_context_t) override;
};

class vvp_arith_mult_real : सार्वजनिक vvp_arith_real_ {

    सार्वजनिक:
      explicit vvp_arith_mult_real();
      ~vvp_arith_mult_real() override;
      शून्य recv_real(vvp_net_ptr_t ptr, द्विगुण bit,
                     vvp_context_t) override;
};

class vvp_arith_pow_real : सार्वजनिक vvp_arith_real_ {

    सार्वजनिक:
      explicit vvp_arith_pow_real();
      ~vvp_arith_pow_real() override;
      शून्य recv_real(vvp_net_ptr_t ptr, द्विगुण bit,
                     vvp_context_t) override;
};

class vvp_arith_sub_real : सार्वजनिक vvp_arith_real_ {

    सार्वजनिक:
      explicit vvp_arith_sub_real();
      ~vvp_arith_sub_real() override;
      शून्य recv_real(vvp_net_ptr_t ptr, द्विगुण bit,
                     vvp_context_t) override;
};

class vvp_cmp_eq_real  : सार्वजनिक vvp_arith_real_ {

    सार्वजनिक:
      explicit vvp_cmp_eq_real();
      शून्य recv_real(vvp_net_ptr_t ptr, ध्रुव द्विगुण bit,
                     vvp_context_t) override;
};

class vvp_cmp_ne_real  : सार्वजनिक vvp_arith_real_ {

    सार्वजनिक:
      explicit vvp_cmp_ne_real();
      शून्य recv_real(vvp_net_ptr_t ptr, ध्रुव द्विगुण bit,
                     vvp_context_t) override;
};

class vvp_cmp_ge_real  : सार्वजनिक vvp_arith_real_ {

    सार्वजनिक:
      explicit vvp_cmp_ge_real();
      शून्य recv_real(vvp_net_ptr_t ptr, ध्रुव द्विगुण bit,
                     vvp_context_t) override;
};

class vvp_cmp_gt_real  : सार्वजनिक vvp_arith_real_ {

    सार्वजनिक:
      explicit vvp_cmp_gt_real();
      शून्य recv_real(vvp_net_ptr_t ptr, ध्रुव द्विगुण bit,
                     vvp_context_t) override;
};

#endif /* IVL_arith_H */
