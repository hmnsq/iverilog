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
class vvp_arith_  : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_arith_(unsigned wid);

      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;

    ರಕ್ಷಿತ:
      ಶೂನ್ಯ dispatch_operand_(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit);

    ರಕ್ಷಿತ:
      unsigned wid_;

      vvp_vector4_t op_a_;
      vvp_vector4_t op_b_;
	// Precalculated X result for propagation.
      vvp_vector4_t x_val_;
};

class vvp_arith_abs : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {
    ಸಾರ್ವಜನಿಕ:
      explicit vvp_arith_abs();
      ~vvp_arith_abs() override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t) override;
      ಶೂನ್ಯ recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ bit,
                     vvp_context_t) override;

      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;

    ಗುಪ್ತ:
};

class vvp_arith_cast_int : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {
    ಸಾರ್ವಜನಿಕ:
      explicit vvp_arith_cast_int(unsigned wid);
      ~vvp_arith_cast_int() override;

      ಶೂನ್ಯ recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ bit,
                     vvp_context_t) override;

    ಗುಪ್ತ:
      unsigned wid_;
};

class vvp_arith_cast_real : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {
    ಸಾರ್ವಜನಿಕ:
      explicit vvp_arith_cast_real(bool signed_flag);
      ~vvp_arith_cast_real() override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t) override;

      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;

    ಗುಪ್ತ:
      bool signed_;
};

class vvp_arith_cast_vec2 : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {
    ಸಾರ್ವಜನಿಕ:
      explicit vvp_arith_cast_vec2(unsigned wid);
      ~vvp_arith_cast_vec2() override;

      ಶೂನ್ಯ recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ bit,
                     vvp_context_t) override;
      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t) override;

      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;

    ಗುಪ್ತ:
      unsigned wid_;
};

class vvp_arith_div : ಸಾರ್ವಜನಿಕ vvp_arith_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_arith_div(unsigned wid, bool signed_flag);
      ~vvp_arith_div() override;
      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t) override;
    ಗುಪ್ತ:
      ಶೂನ್ಯ wide4_(vvp_net_ptr_t ptr);
      bool signed_flag_;
};

class vvp_arith_mod : ಸಾರ್ವಜನಿಕ vvp_arith_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_arith_mod(unsigned wid, bool signed_flag);
      ~vvp_arith_mod() override;
      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t) override;
    ಗುಪ್ತ:
      ಶೂನ್ಯ wide_(vvp_net_ptr_t ptr);
      bool signed_flag_;
};

/* vvp_cmp_* objects...
 * the vvp_cmp_* objects all are special vvp_arith_ objects in that
 * their widths are only for their inputs. The output widths are all
 * exactly 1 bit.
 */

class vvp_cmp_eeq  : ಸಾರ್ವಜನಿಕ vvp_arith_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_cmp_eeq(unsigned wid);
      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_nee  : ಸಾರ್ವಜನಿಕ vvp_arith_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_cmp_nee(unsigned wid);
      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_eq  : ಸಾರ್ವಜನಿಕ vvp_arith_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_cmp_eq(unsigned wid);
      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_eqx  : ಸಾರ್ವಜನಿಕ vvp_arith_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_cmp_eqx(unsigned wid);
      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_eqz  : ಸಾರ್ವಜನಿಕ vvp_arith_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_cmp_eqz(unsigned wid);
      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_ne  : ಸಾರ್ವಜನಿಕ vvp_arith_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_cmp_ne(unsigned wid);
      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_weq  : ಸಾರ್ವಜನಿಕ vvp_arith_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_cmp_weq(unsigned wid);
      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_wne  : ಸಾರ್ವಜನಿಕ vvp_arith_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_cmp_wne(unsigned wid);
      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t) override;

};


/*
 * This base class implements both GT and GE comparisons. The derived
 * GT and GE call the recv_vec4_base_ method with a different
 * out_if_equal argument that reflects the different expectations.
 */
class vvp_cmp_gtge_base_ : ಸಾರ್ವಜನಿಕ vvp_arith_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_cmp_gtge_base_(unsigned wid, bool signed_flag);

    ರಕ್ಷಿತ:
      ಶೂನ್ಯ recv_vec4_base_(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
			   vvp_bit4_t out_if_equal);
    ಗುಪ್ತ:
      bool signed_flag_;
};

class vvp_cmp_ge  : ಸಾರ್ವಜನಿಕ vvp_cmp_gtge_base_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_cmp_ge(unsigned wid, bool signed_flag);

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_gt  : ಸಾರ್ವಜನಿಕ vvp_cmp_gtge_base_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_cmp_gt(unsigned wid, bool signed_flag);

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t) override;
};

/*
 * NOTE: The inputs to the vvp_arith_mult are not necessarily the same
 * width as the output. This is different from the typical vvp_arith_
 * object. Perhaps that means this isn't quite a vvp_arith_ object?
 */
class vvp_arith_mult  : ಸಾರ್ವಜನಿಕ vvp_arith_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_arith_mult(unsigned wid);
      ~vvp_arith_mult() override;
      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t) override;
    ಗುಪ್ತ:
      ಶೂನ್ಯ wide_(vvp_net_ptr_t ptr);
};

class vvp_arith_pow  : ಸಾರ್ವಜನಿಕ vvp_arith_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_arith_pow(unsigned wid, bool signed_flag);
      ~vvp_arith_pow() override;
      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t) override;
    ಗುಪ್ತ:
      bool signed_flag_;
};

class vvp_arith_sub  : ಸಾರ್ವಜನಿಕ vvp_arith_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_arith_sub(unsigned wid);
      ~vvp_arith_sub() override;
      ಆಭಾಸ ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                             vvp_context_t) override;

};

class vvp_arith_sum  : ಸಾರ್ವಜನಿಕ vvp_arith_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_arith_sum(unsigned wid);
      ~vvp_arith_sum() override;
      ಆಭಾಸ ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                             vvp_context_t) override;

};

class vvp_shiftl  : ಸಾರ್ವಜನಿಕ vvp_arith_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_shiftl(unsigned wid);
      ~vvp_shiftl() override;
      ಆಭಾಸ ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                             vvp_context_t) override;
};

class vvp_shiftr  : ಸಾರ್ವಜನಿಕ vvp_arith_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_shiftr(unsigned wid, bool signed_flag);
      ~vvp_shiftr() override;
      ಆಭಾಸ ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                             vvp_context_t) override;

    ಗುಪ್ತ:
      bool signed_flag_;
};

/*
 * Base class for real valued expressions. These are similar to the
 * vector expression classes, but the inputs are collected from the
 * recv_real method.
 */
class vvp_arith_real_  : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_arith_real_();

    ರಕ್ಷಿತ:
      ಶೂನ್ಯ dispatch_operand_(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ bit);

    ರಕ್ಷಿತ:
      ದ್ವಿಗುಣ op_a_;
      ದ್ವಿಗುಣ op_b_;
};


class vvp_arith_sum_real : ಸಾರ್ವಜನಿಕ vvp_arith_real_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_arith_sum_real();
      ~vvp_arith_sum_real() override;
      ಶೂನ್ಯ recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ bit,
                     vvp_context_t) override;
};

class vvp_arith_div_real : ಸಾರ್ವಜನಿಕ vvp_arith_real_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_arith_div_real();
      ~vvp_arith_div_real() override;
      ಶೂನ್ಯ recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ bit,
                     vvp_context_t) override;
};

class vvp_arith_mod_real : ಸಾರ್ವಜನಿಕ vvp_arith_real_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_arith_mod_real();
      ~vvp_arith_mod_real() override;
      ಶೂನ್ಯ recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ bit,
                     vvp_context_t) override;
};

class vvp_arith_mult_real : ಸಾರ್ವಜನಿಕ vvp_arith_real_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_arith_mult_real();
      ~vvp_arith_mult_real() override;
      ಶೂನ್ಯ recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ bit,
                     vvp_context_t) override;
};

class vvp_arith_pow_real : ಸಾರ್ವಜನಿಕ vvp_arith_real_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_arith_pow_real();
      ~vvp_arith_pow_real() override;
      ಶೂನ್ಯ recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ bit,
                     vvp_context_t) override;
};

class vvp_arith_sub_real : ಸಾರ್ವಜನಿಕ vvp_arith_real_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_arith_sub_real();
      ~vvp_arith_sub_real() override;
      ಶೂನ್ಯ recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ bit,
                     vvp_context_t) override;
};

class vvp_cmp_eq_real  : ಸಾರ್ವಜನಿಕ vvp_arith_real_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_cmp_eq_real();
      ಶೂನ್ಯ recv_real(vvp_net_ptr_t ptr, ಧ್ರುವ ದ್ವಿಗುಣ bit,
                     vvp_context_t) override;
};

class vvp_cmp_ne_real  : ಸಾರ್ವಜನಿಕ vvp_arith_real_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_cmp_ne_real();
      ಶೂನ್ಯ recv_real(vvp_net_ptr_t ptr, ಧ್ರುವ ದ್ವಿಗುಣ bit,
                     vvp_context_t) override;
};

class vvp_cmp_ge_real  : ಸಾರ್ವಜನಿಕ vvp_arith_real_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_cmp_ge_real();
      ಶೂನ್ಯ recv_real(vvp_net_ptr_t ptr, ಧ್ರುವ ದ್ವಿಗುಣ bit,
                     vvp_context_t) override;
};

class vvp_cmp_gt_real  : ಸಾರ್ವಜನಿಕ vvp_arith_real_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_cmp_gt_real();
      ಶೂನ್ಯ recv_real(vvp_net_ptr_t ptr, ಧ್ರುವ ದ್ವಿಗುಣ bit,
                     vvp_context_t) override;
};

#endif /* IVL_arith_H */
