#اگر_غیر_تعریف_شدہ IVL_arith_H
#تعریف_کرو IVL_arith_H
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
class vvp_arith_  : عوامی vvp_net_fun_t {

    عوامی:
      explicit vvp_arith_(غیرعلامتی wid);

      خالی recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t ctx) override;

    محفوظ:
      خالی dispatch_عملیہ(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ);

    محفوظ:
      غیرعلامتی wid_;

      vvp_vector4_t op_a_;
      vvp_vector4_t op_b_;
	// Precalculated X result for propagation.
      vvp_vector4_t x_val_;
};

class vvp_arith_abs : عوامی vvp_net_fun_t {
    عوامی:
      explicit vvp_arith_abs();
      ~vvp_arith_abs() override;

      خالی recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;
      خالی recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                     vvp_context_t) override;

      خالی recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t ctx) override;

    نجی:
};

class vvp_arith_cast_int : عوامی vvp_net_fun_t {
    عوامی:
      explicit vvp_arith_cast_int(غیرعلامتی wid);
      ~vvp_arith_cast_int() override;

      خالی recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                     vvp_context_t) override;

    نجی:
      غیرعلامتی wid_;
};

class vvp_arith_cast_real : عوامی vvp_net_fun_t {
    عوامی:
      explicit vvp_arith_cast_real(bool signed_flag);
      ~vvp_arith_cast_real() override;

      خالی recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;

      خالی recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t ctx) override;

    نجی:
      bool signed_;
};

class vvp_arith_cast_vec2 : عوامی vvp_net_fun_t {
    عوامی:
      explicit vvp_arith_cast_vec2(غیرعلامتی wid);
      ~vvp_arith_cast_vec2() override;

      خالی recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                     vvp_context_t) override;
      خالی recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;

      خالی recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t ctx) override;

    نجی:
      غیرعلامتی wid_;
};

class vvp_arith_div : عوامی vvp_arith_ {

    عوامی:
      explicit vvp_arith_div(غیرعلامتی wid, bool signed_flag);
      ~vvp_arith_div() override;
      خالی recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;
    نجی:
      خالی wide4_(vvp_net_ptr_t ptr);
      bool signed_flag_;
};

class vvp_arith_mod : عوامی vvp_arith_ {

    عوامی:
      explicit vvp_arith_mod(غیرعلامتی wid, bool signed_flag);
      ~vvp_arith_mod() override;
      خالی recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;
    نجی:
      خالی wide_(vvp_net_ptr_t ptr);
      bool signed_flag_;
};

/* vvp_cmp_* objects...
 * the vvp_cmp_* objects all are special vvp_arith_ objects in that
 * their widths are only for their inputs. The output widths are all
 * exactly 1 bit.
 */

class vvp_cmp_eeq  : عوامی vvp_arith_ {

    عوامی:
      explicit vvp_cmp_eeq(غیرعلامتی wid);
      خالی recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;

};

class vvp_cmp_nee  : عوامی vvp_arith_ {

    عوامی:
      explicit vvp_cmp_nee(غیرعلامتی wid);
      خالی recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;

};

class vvp_cmp_eq  : عوامی vvp_arith_ {

    عوامی:
      explicit vvp_cmp_eq(غیرعلامتی wid);
      خالی recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;

};

class vvp_cmp_eqx  : عوامی vvp_arith_ {

    عوامی:
      explicit vvp_cmp_eqx(غیرعلامتی wid);
      خالی recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;

};

class vvp_cmp_eqz  : عوامی vvp_arith_ {

    عوامی:
      explicit vvp_cmp_eqz(غیرعلامتی wid);
      خالی recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;

};

class vvp_cmp_ne  : عوامی vvp_arith_ {

    عوامی:
      explicit vvp_cmp_ne(غیرعلامتی wid);
      خالی recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;

};

class vvp_cmp_weq  : عوامی vvp_arith_ {

    عوامی:
      explicit vvp_cmp_weq(غیرعلامتی wid);
      خالی recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;

};

class vvp_cmp_wne  : عوامی vvp_arith_ {

    عوامی:
      explicit vvp_cmp_wne(غیرعلامتی wid);
      خالی recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;

};


/*
 * This base class implements both GT and GE comparisons. The derived
 * GT and GE call the recv_vec4_base_ method with a different
 * out_if_equal argument that reflects the different expectations.
 */
class vvp_cmp_gtge_بنیاد : عوامی vvp_arith_ {

    عوامی:
      explicit vvp_cmp_gtge_بنیاد(غیرعلامتی wid, bool signed_flag);

    محفوظ:
      خالی recv_vec4_بنیاد(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			   vvp_bit4_t برآمد_if_equal);
    نجی:
      bool signed_flag_;
};

class vvp_cmp_ge  : عوامی vvp_cmp_gtge_بنیاد {

    عوامی:
      explicit vvp_cmp_ge(غیرعلامتی wid, bool signed_flag);

      خالی recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;

};

class vvp_cmp_gt  : عوامی vvp_cmp_gtge_بنیاد {

    عوامی:
      explicit vvp_cmp_gt(غیرعلامتی wid, bool signed_flag);

      خالی recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;
};

/*
 * NOTE: The inputs to the vvp_arith_mult are not necessarily the same
 * width as the output. This is different from the typical vvp_arith_
 * object. Perhaps that means this isn't quite a vvp_arith_ object?
 */
class vvp_arith_mult  : عوامی vvp_arith_ {

    عوامی:
      explicit vvp_arith_mult(غیرعلامتی wid);
      ~vvp_arith_mult() override;
      خالی recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;
    نجی:
      خالی wide_(vvp_net_ptr_t ptr);
};

class vvp_arith_pow  : عوامی vvp_arith_ {

    عوامی:
      explicit vvp_arith_pow(غیرعلامتی wid, bool signed_flag);
      ~vvp_arith_pow() override;
      خالی recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;
    نجی:
      bool signed_flag_;
};

class vvp_arith_sub  : عوامی vvp_arith_ {

    عوامی:
      explicit vvp_arith_sub(غیرعلامتی wid);
      ~vvp_arith_sub() override;
      مجازی خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                             vvp_context_t) override;

};

class vvp_arith_مجموع  : عوامی vvp_arith_ {

    عوامی:
      explicit vvp_arith_مجموع(غیرعلامتی wid);
      ~vvp_arith_مجموع() override;
      مجازی خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                             vvp_context_t) override;

};

class vvp_shiftl  : عوامی vvp_arith_ {

    عوامی:
      explicit vvp_shiftl(غیرعلامتی wid);
      ~vvp_shiftl() override;
      مجازی خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                             vvp_context_t) override;
};

class vvp_shiftr  : عوامی vvp_arith_ {

    عوامی:
      explicit vvp_shiftr(غیرعلامتی wid, bool signed_flag);
      ~vvp_shiftr() override;
      مجازی خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                             vvp_context_t) override;

    نجی:
      bool signed_flag_;
};

/*
 * Base class for real valued expressions. These are similar to the
 * vector expression classes, but the inputs are collected from the
 * recv_real method.
 */
class vvp_arith_real_  : عوامی vvp_net_fun_t {

    عوامی:
      explicit vvp_arith_real_();

    محفوظ:
      خالی dispatch_عملیہ(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ);

    محفوظ:
      دوہرا op_a_;
      دوہرا op_b_;
};


class vvp_arith_مجموع_real : عوامی vvp_arith_real_ {

    عوامی:
      explicit vvp_arith_مجموع_real();
      ~vvp_arith_مجموع_real() override;
      خالی recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                     vvp_context_t) override;
};

class vvp_arith_div_real : عوامی vvp_arith_real_ {

    عوامی:
      explicit vvp_arith_div_real();
      ~vvp_arith_div_real() override;
      خالی recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                     vvp_context_t) override;
};

class vvp_arith_mod_real : عوامی vvp_arith_real_ {

    عوامی:
      explicit vvp_arith_mod_real();
      ~vvp_arith_mod_real() override;
      خالی recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                     vvp_context_t) override;
};

class vvp_arith_mult_real : عوامی vvp_arith_real_ {

    عوامی:
      explicit vvp_arith_mult_real();
      ~vvp_arith_mult_real() override;
      خالی recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                     vvp_context_t) override;
};

class vvp_arith_pow_real : عوامی vvp_arith_real_ {

    عوامی:
      explicit vvp_arith_pow_real();
      ~vvp_arith_pow_real() override;
      خالی recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                     vvp_context_t) override;
};

class vvp_arith_sub_real : عوامی vvp_arith_real_ {

    عوامی:
      explicit vvp_arith_sub_real();
      ~vvp_arith_sub_real() override;
      خالی recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                     vvp_context_t) override;
};

class vvp_cmp_eq_real  : عوامی vvp_arith_real_ {

    عوامی:
      explicit vvp_cmp_eq_real();
      خالی recv_real(vvp_net_ptr_t ptr, ثابت دوہرا ثنائی_ہندسہ,
                     vvp_context_t) override;
};

class vvp_cmp_ne_real  : عوامی vvp_arith_real_ {

    عوامی:
      explicit vvp_cmp_ne_real();
      خالی recv_real(vvp_net_ptr_t ptr, ثابت دوہرا ثنائی_ہندسہ,
                     vvp_context_t) override;
};

class vvp_cmp_ge_real  : عوامی vvp_arith_real_ {

    عوامی:
      explicit vvp_cmp_ge_real();
      خالی recv_real(vvp_net_ptr_t ptr, ثابت دوہرا ثنائی_ہندسہ,
                     vvp_context_t) override;
};

class vvp_cmp_gt_real  : عوامی vvp_arith_real_ {

    عوامی:
      explicit vvp_cmp_gt_real();
      خالی recv_real(vvp_net_ptr_t ptr, ثابت دوہرا ثنائی_ہندسہ,
                     vvp_context_t) override;
};

#اگر_ختم /* IVL_arith_H */
