#अगर_अघोषित IVL_arith_H
#घोषणा IVL_arith_H
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
class vvp_arith_  : खुला vvp_net_fun_t {

    खुला:
      explicit vvp_arith_(अचिन्हित wid);

      व्योम recv_vec4_pv(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
			अचिन्हित base, अचिन्हित vwid, vvp_context_t ctx) override;

    रक्षित:
      व्योम dispatch_operand_(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit);

    रक्षित:
      अचिन्हित wid_;

      vvp_vector4_t op_a_;
      vvp_vector4_t op_b_;
	// Precalculated X result for propagation.
      vvp_vector4_t x_val_;
};

class vvp_arith_abs : खुला vvp_net_fun_t {
    खुला:
      explicit vvp_arith_abs();
      ~vvp_arith_abs() override;

      व्योम recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;
      व्योम recv_real(vvp_net_ptr_t ptr, द्विगुन bit,
                     vvp_context_t) override;

      व्योम recv_vec4_pv(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
			अचिन्हित base, अचिन्हित vwid, vvp_context_t ctx) override;

    निजी:
};

class vvp_arith_cast_int : खुला vvp_net_fun_t {
    खुला:
      explicit vvp_arith_cast_int(अचिन्हित wid);
      ~vvp_arith_cast_int() override;

      व्योम recv_real(vvp_net_ptr_t ptr, द्विगुन bit,
                     vvp_context_t) override;

    निजी:
      अचिन्हित wid_;
};

class vvp_arith_cast_real : खुला vvp_net_fun_t {
    खुला:
      explicit vvp_arith_cast_real(bool signed_flag);
      ~vvp_arith_cast_real() override;

      व्योम recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;

      व्योम recv_vec4_pv(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
			अचिन्हित base, अचिन्हित vwid, vvp_context_t ctx) override;

    निजी:
      bool signed_;
};

class vvp_arith_cast_vec2 : खुला vvp_net_fun_t {
    खुला:
      explicit vvp_arith_cast_vec2(अचिन्हित wid);
      ~vvp_arith_cast_vec2() override;

      व्योम recv_real(vvp_net_ptr_t ptr, द्विगुन bit,
                     vvp_context_t) override;
      व्योम recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;

      व्योम recv_vec4_pv(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
			अचिन्हित base, अचिन्हित vwid, vvp_context_t ctx) override;

    निजी:
      अचिन्हित wid_;
};

class vvp_arith_div : खुला vvp_arith_ {

    खुला:
      explicit vvp_arith_div(अचिन्हित wid, bool signed_flag);
      ~vvp_arith_div() override;
      व्योम recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;
    निजी:
      व्योम wide4_(vvp_net_ptr_t ptr);
      bool signed_flag_;
};

class vvp_arith_mod : खुला vvp_arith_ {

    खुला:
      explicit vvp_arith_mod(अचिन्हित wid, bool signed_flag);
      ~vvp_arith_mod() override;
      व्योम recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;
    निजी:
      व्योम wide_(vvp_net_ptr_t ptr);
      bool signed_flag_;
};

/* vvp_cmp_* objects...
 * the vvp_cmp_* objects all are special vvp_arith_ objects in that
 * their widths are only for their inputs. The output widths are all
 * exactly 1 bit.
 */

class vvp_cmp_eeq  : खुला vvp_arith_ {

    खुला:
      explicit vvp_cmp_eeq(अचिन्हित wid);
      व्योम recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_nee  : खुला vvp_arith_ {

    खुला:
      explicit vvp_cmp_nee(अचिन्हित wid);
      व्योम recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_eq  : खुला vvp_arith_ {

    खुला:
      explicit vvp_cmp_eq(अचिन्हित wid);
      व्योम recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_eqx  : खुला vvp_arith_ {

    खुला:
      explicit vvp_cmp_eqx(अचिन्हित wid);
      व्योम recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_eqz  : खुला vvp_arith_ {

    खुला:
      explicit vvp_cmp_eqz(अचिन्हित wid);
      व्योम recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_ne  : खुला vvp_arith_ {

    खुला:
      explicit vvp_cmp_ne(अचिन्हित wid);
      व्योम recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_weq  : खुला vvp_arith_ {

    खुला:
      explicit vvp_cmp_weq(अचिन्हित wid);
      व्योम recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_wne  : खुला vvp_arith_ {

    खुला:
      explicit vvp_cmp_wne(अचिन्हित wid);
      व्योम recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;

};


/*
 * This base class implements both GT and GE comparisons. The derived
 * GT and GE call the recv_vec4_base_ method with a different
 * out_if_equal argument that reflects the different expectations.
 */
class vvp_cmp_gtge_base_ : खुला vvp_arith_ {

    खुला:
      explicit vvp_cmp_gtge_base_(अचिन्हित wid, bool signed_flag);

    रक्षित:
      व्योम recv_vec4_base_(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
			   vvp_bit4_t out_if_equal);
    निजी:
      bool signed_flag_;
};

class vvp_cmp_ge  : खुला vvp_cmp_gtge_base_ {

    खुला:
      explicit vvp_cmp_ge(अचिन्हित wid, bool signed_flag);

      व्योम recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;

};

class vvp_cmp_gt  : खुला vvp_cmp_gtge_base_ {

    खुला:
      explicit vvp_cmp_gt(अचिन्हित wid, bool signed_flag);

      व्योम recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;
};

/*
 * NOTE: The inputs to the vvp_arith_mult are not necessarily the same
 * width as the output. This is different from the typical vvp_arith_
 * object. Perhaps that means this isn't quite a vvp_arith_ object?
 */
class vvp_arith_mult  : खुला vvp_arith_ {

    खुला:
      explicit vvp_arith_mult(अचिन्हित wid);
      ~vvp_arith_mult() override;
      व्योम recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;
    निजी:
      व्योम wide_(vvp_net_ptr_t ptr);
};

class vvp_arith_pow  : खुला vvp_arith_ {

    खुला:
      explicit vvp_arith_pow(अचिन्हित wid, bool signed_flag);
      ~vvp_arith_pow() override;
      व्योम recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;
    निजी:
      bool signed_flag_;
};

class vvp_arith_sub  : खुला vvp_arith_ {

    खुला:
      explicit vvp_arith_sub(अचिन्हित wid);
      ~vvp_arith_sub() override;
      भव व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                             vvp_context_t) override;

};

class vvp_arith_योग  : खुला vvp_arith_ {

    खुला:
      explicit vvp_arith_योग(अचिन्हित wid);
      ~vvp_arith_योग() override;
      भव व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                             vvp_context_t) override;

};

class vvp_shiftl  : खुला vvp_arith_ {

    खुला:
      explicit vvp_shiftl(अचिन्हित wid);
      ~vvp_shiftl() override;
      भव व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                             vvp_context_t) override;
};

class vvp_shiftr  : खुला vvp_arith_ {

    खुला:
      explicit vvp_shiftr(अचिन्हित wid, bool signed_flag);
      ~vvp_shiftr() override;
      भव व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                             vvp_context_t) override;

    निजी:
      bool signed_flag_;
};

/*
 * Base class for real valued expressions. These are similar to the
 * vector expression classes, but the inputs are collected from the
 * recv_real method.
 */
class vvp_arith_real_  : खुला vvp_net_fun_t {

    खुला:
      explicit vvp_arith_real_();

    रक्षित:
      व्योम dispatch_operand_(vvp_net_ptr_t ptr, द्विगुन bit);

    रक्षित:
      द्विगुन op_a_;
      द्विगुन op_b_;
};


class vvp_arith_योग_real : खुला vvp_arith_real_ {

    खुला:
      explicit vvp_arith_योग_real();
      ~vvp_arith_योग_real() override;
      व्योम recv_real(vvp_net_ptr_t ptr, द्विगुन bit,
                     vvp_context_t) override;
};

class vvp_arith_div_real : खुला vvp_arith_real_ {

    खुला:
      explicit vvp_arith_div_real();
      ~vvp_arith_div_real() override;
      व्योम recv_real(vvp_net_ptr_t ptr, द्विगुन bit,
                     vvp_context_t) override;
};

class vvp_arith_mod_real : खुला vvp_arith_real_ {

    खुला:
      explicit vvp_arith_mod_real();
      ~vvp_arith_mod_real() override;
      व्योम recv_real(vvp_net_ptr_t ptr, द्विगुन bit,
                     vvp_context_t) override;
};

class vvp_arith_mult_real : खुला vvp_arith_real_ {

    खुला:
      explicit vvp_arith_mult_real();
      ~vvp_arith_mult_real() override;
      व्योम recv_real(vvp_net_ptr_t ptr, द्विगुन bit,
                     vvp_context_t) override;
};

class vvp_arith_pow_real : खुला vvp_arith_real_ {

    खुला:
      explicit vvp_arith_pow_real();
      ~vvp_arith_pow_real() override;
      व्योम recv_real(vvp_net_ptr_t ptr, द्विगुन bit,
                     vvp_context_t) override;
};

class vvp_arith_sub_real : खुला vvp_arith_real_ {

    खुला:
      explicit vvp_arith_sub_real();
      ~vvp_arith_sub_real() override;
      व्योम recv_real(vvp_net_ptr_t ptr, द्विगुन bit,
                     vvp_context_t) override;
};

class vvp_cmp_eq_real  : खुला vvp_arith_real_ {

    खुला:
      explicit vvp_cmp_eq_real();
      व्योम recv_real(vvp_net_ptr_t ptr, स्थिर द्विगुन bit,
                     vvp_context_t) override;
};

class vvp_cmp_ne_real  : खुला vvp_arith_real_ {

    खुला:
      explicit vvp_cmp_ne_real();
      व्योम recv_real(vvp_net_ptr_t ptr, स्थिर द्विगुन bit,
                     vvp_context_t) override;
};

class vvp_cmp_ge_real  : खुला vvp_arith_real_ {

    खुला:
      explicit vvp_cmp_ge_real();
      व्योम recv_real(vvp_net_ptr_t ptr, स्थिर द्विगुन bit,
                     vvp_context_t) override;
};

class vvp_cmp_gt_real  : खुला vvp_arith_real_ {

    खुला:
      explicit vvp_cmp_gt_real();
      व्योम recv_real(vvp_net_ptr_t ptr, स्थिर द्विगुन bit,
                     vvp_context_t) override;
};

#पूर्ण_अगर /* IVL_arith_H */
