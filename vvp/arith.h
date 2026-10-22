#నిర్వచితం_కాకపోతే IVL_arith_H
#నిర్వచించు IVL_arith_H
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
class vvp_arith_  : బహిరంగ vvp_net_fun_t {

    బహిరంగ:
      explicit vvp_arith_(గుర్తులేని wid);

      శూన్య recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t ctx) override;

    రక్షిత:
      శూన్య dispatch_నిర్వాహ్యం(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు);

    రక్షిత:
      గుర్తులేని wid_;

      vvp_vector4_t op_a_;
      vvp_vector4_t op_b_;
	// Precalculated X result for propagation.
      vvp_vector4_t x_val_;
};

class vvp_arith_abs : బహిరంగ vvp_net_fun_t {
    బహిరంగ:
      explicit vvp_arith_abs();
      ~vvp_arith_abs() override;

      శూన్య recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;
      శూన్య recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                     vvp_context_t) override;

      శూన్య recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t ctx) override;

    అంతరంగ:
};

class vvp_arith_cast_int : బహిరంగ vvp_net_fun_t {
    బహిరంగ:
      explicit vvp_arith_cast_int(గుర్తులేని wid);
      ~vvp_arith_cast_int() override;

      శూన్య recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                     vvp_context_t) override;

    అంతరంగ:
      గుర్తులేని wid_;
};

class vvp_arith_cast_real : బహిరంగ vvp_net_fun_t {
    బహిరంగ:
      explicit vvp_arith_cast_real(bool signed_flag);
      ~vvp_arith_cast_real() override;

      శూన్య recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;

      శూన్య recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t ctx) override;

    అంతరంగ:
      bool signed_;
};

class vvp_arith_cast_vec2 : బహిరంగ vvp_net_fun_t {
    బహిరంగ:
      explicit vvp_arith_cast_vec2(గుర్తులేని wid);
      ~vvp_arith_cast_vec2() override;

      శూన్య recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                     vvp_context_t) override;
      శూన్య recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;

      శూన్య recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t ctx) override;

    అంతరంగ:
      గుర్తులేని wid_;
};

class vvp_arith_div : బహిరంగ vvp_arith_ {

    బహిరంగ:
      explicit vvp_arith_div(గుర్తులేని wid, bool signed_flag);
      ~vvp_arith_div() override;
      శూన్య recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;
    అంతరంగ:
      శూన్య wide4_(vvp_net_ptr_t ptr);
      bool signed_flag_;
};

class vvp_arith_mod : బహిరంగ vvp_arith_ {

    బహిరంగ:
      explicit vvp_arith_mod(గుర్తులేని wid, bool signed_flag);
      ~vvp_arith_mod() override;
      శూన్య recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;
    అంతరంగ:
      శూన్య wide_(vvp_net_ptr_t ptr);
      bool signed_flag_;
};

/* vvp_cmp_* objects...
 * the vvp_cmp_* objects all are special vvp_arith_ objects in that
 * their widths are only for their inputs. The output widths are all
 * exactly 1 bit.
 */

class vvp_cmp_eeq  : బహిరంగ vvp_arith_ {

    బహిరంగ:
      explicit vvp_cmp_eeq(గుర్తులేని wid);
      శూన్య recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;

};

class vvp_cmp_nee  : బహిరంగ vvp_arith_ {

    బహిరంగ:
      explicit vvp_cmp_nee(గుర్తులేని wid);
      శూన్య recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;

};

class vvp_cmp_eq  : బహిరంగ vvp_arith_ {

    బహిరంగ:
      explicit vvp_cmp_eq(గుర్తులేని wid);
      శూన్య recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;

};

class vvp_cmp_eqx  : బహిరంగ vvp_arith_ {

    బహిరంగ:
      explicit vvp_cmp_eqx(గుర్తులేని wid);
      శూన్య recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;

};

class vvp_cmp_eqz  : బహిరంగ vvp_arith_ {

    బహిరంగ:
      explicit vvp_cmp_eqz(గుర్తులేని wid);
      శూన్య recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;

};

class vvp_cmp_ne  : బహిరంగ vvp_arith_ {

    బహిరంగ:
      explicit vvp_cmp_ne(గుర్తులేని wid);
      శూన్య recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;

};

class vvp_cmp_weq  : బహిరంగ vvp_arith_ {

    బహిరంగ:
      explicit vvp_cmp_weq(గుర్తులేని wid);
      శూన్య recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;

};

class vvp_cmp_wne  : బహిరంగ vvp_arith_ {

    బహిరంగ:
      explicit vvp_cmp_wne(గుర్తులేని wid);
      శూన్య recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;

};


/*
 * This base class implements both GT and GE comparisons. The derived
 * GT and GE call the recv_vec4_base_ method with a different
 * out_if_equal argument that reflects the different expectations.
 */
class vvp_cmp_gtge_పునాది : బహిరంగ vvp_arith_ {

    బహిరంగ:
      explicit vvp_cmp_gtge_పునాది(గుర్తులేని wid, bool signed_flag);

    రక్షిత:
      శూన్య recv_vec4_పునాది(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
			   vvp_bit4_t బహిర్గతం_if_equal);
    అంతరంగ:
      bool signed_flag_;
};

class vvp_cmp_ge  : బహిరంగ vvp_cmp_gtge_పునాది {

    బహిరంగ:
      explicit vvp_cmp_ge(గుర్తులేని wid, bool signed_flag);

      శూన్య recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;

};

class vvp_cmp_gt  : బహిరంగ vvp_cmp_gtge_పునాది {

    బహిరంగ:
      explicit vvp_cmp_gt(గుర్తులేని wid, bool signed_flag);

      శూన్య recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;
};

/*
 * NOTE: The inputs to the vvp_arith_mult are not necessarily the same
 * width as the output. This is different from the typical vvp_arith_
 * object. Perhaps that means this isn't quite a vvp_arith_ object?
 */
class vvp_arith_mult  : బహిరంగ vvp_arith_ {

    బహిరంగ:
      explicit vvp_arith_mult(గుర్తులేని wid);
      ~vvp_arith_mult() override;
      శూన్య recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;
    అంతరంగ:
      శూన్య wide_(vvp_net_ptr_t ptr);
};

class vvp_arith_pow  : బహిరంగ vvp_arith_ {

    బహిరంగ:
      explicit vvp_arith_pow(గుర్తులేని wid, bool signed_flag);
      ~vvp_arith_pow() override;
      శూన్య recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;
    అంతరంగ:
      bool signed_flag_;
};

class vvp_arith_sub  : బహిరంగ vvp_arith_ {

    బహిరంగ:
      explicit vvp_arith_sub(గుర్తులేని wid);
      ~vvp_arith_sub() override;
      మాయా శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                             vvp_context_t) override;

};

class vvp_arith_కూడిక  : బహిరంగ vvp_arith_ {

    బహిరంగ:
      explicit vvp_arith_కూడిక(గుర్తులేని wid);
      ~vvp_arith_కూడిక() override;
      మాయా శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                             vvp_context_t) override;

};

class vvp_shiftl  : బహిరంగ vvp_arith_ {

    బహిరంగ:
      explicit vvp_shiftl(గుర్తులేని wid);
      ~vvp_shiftl() override;
      మాయా శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                             vvp_context_t) override;
};

class vvp_shiftr  : బహిరంగ vvp_arith_ {

    బహిరంగ:
      explicit vvp_shiftr(గుర్తులేని wid, bool signed_flag);
      ~vvp_shiftr() override;
      మాయా శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                             vvp_context_t) override;

    అంతరంగ:
      bool signed_flag_;
};

/*
 * Base class for real valued expressions. These are similar to the
 * vector expression classes, but the inputs are collected from the
 * recv_real method.
 */
class vvp_arith_real_  : బహిరంగ vvp_net_fun_t {

    బహిరంగ:
      explicit vvp_arith_real_();

    రక్షిత:
      శూన్య dispatch_నిర్వాహ్యం(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు);

    రక్షిత:
      ద్విగుణ op_a_;
      ద్విగుణ op_b_;
};


class vvp_arith_కూడిక_real : బహిరంగ vvp_arith_real_ {

    బహిరంగ:
      explicit vvp_arith_కూడిక_real();
      ~vvp_arith_కూడిక_real() override;
      శూన్య recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                     vvp_context_t) override;
};

class vvp_arith_div_real : బహిరంగ vvp_arith_real_ {

    బహిరంగ:
      explicit vvp_arith_div_real();
      ~vvp_arith_div_real() override;
      శూన్య recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                     vvp_context_t) override;
};

class vvp_arith_mod_real : బహిరంగ vvp_arith_real_ {

    బహిరంగ:
      explicit vvp_arith_mod_real();
      ~vvp_arith_mod_real() override;
      శూన్య recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                     vvp_context_t) override;
};

class vvp_arith_mult_real : బహిరంగ vvp_arith_real_ {

    బహిరంగ:
      explicit vvp_arith_mult_real();
      ~vvp_arith_mult_real() override;
      శూన్య recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                     vvp_context_t) override;
};

class vvp_arith_pow_real : బహిరంగ vvp_arith_real_ {

    బహిరంగ:
      explicit vvp_arith_pow_real();
      ~vvp_arith_pow_real() override;
      శూన్య recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                     vvp_context_t) override;
};

class vvp_arith_sub_real : బహిరంగ vvp_arith_real_ {

    బహిరంగ:
      explicit vvp_arith_sub_real();
      ~vvp_arith_sub_real() override;
      శూన్య recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                     vvp_context_t) override;
};

class vvp_cmp_eq_real  : బహిరంగ vvp_arith_real_ {

    బహిరంగ:
      explicit vvp_cmp_eq_real();
      శూన్య recv_real(vvp_net_ptr_t ptr, స్థిరరాశి ద్విగుణ బిట్టు,
                     vvp_context_t) override;
};

class vvp_cmp_ne_real  : బహిరంగ vvp_arith_real_ {

    బహిరంగ:
      explicit vvp_cmp_ne_real();
      శూన్య recv_real(vvp_net_ptr_t ptr, స్థిరరాశి ద్విగుణ బిట్టు,
                     vvp_context_t) override;
};

class vvp_cmp_ge_real  : బహిరంగ vvp_arith_real_ {

    బహిరంగ:
      explicit vvp_cmp_ge_real();
      శూన్య recv_real(vvp_net_ptr_t ptr, స్థిరరాశి ద్విగుణ బిట్టు,
                     vvp_context_t) override;
};

class vvp_cmp_gt_real  : బహిరంగ vvp_arith_real_ {

    బహిరంగ:
      explicit vvp_cmp_gt_real();
      శూన్య recv_real(vvp_net_ptr_t ptr, స్థిరరాశి ద్విగుణ బిట్టు,
                     vvp_context_t) override;
};

#అయితే_ముగింపు /* IVL_arith_H */
