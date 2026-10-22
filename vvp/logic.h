#جے_بنا_تعریف IVL_logic_H
#تعریف_کرو IVL_logic_H
/*
 * Copyright (c) 2000-2025 Stephen Williams (steve@icarus.com)
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
# include  "schedule.h"
# include  <cstddef>

/*
 * vvp_fun_boolean_ is just a common hook for holding operands.
 */
class vvp_fun_boolean_ : عام vvp_net_fun_t, محمي vvp_gen_event_s {

    عام:
      explicit vvp_fun_boolean_(بنانشانی wid);
      ~vvp_fun_boolean_() override;

      فراغ recv_vec4(vvp_net_ptr_t p, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;
      فراغ recv_vec4_pv(vvp_net_ptr_t p, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			بنانشانی ادھار, بنانشانی vwid, vvp_context_t) override;
      فراغ recv_real(vvp_net_ptr_t p, مزدوج real,
                     vvp_context_t) override;

    محمي:
      vvp_vector4_t داخلہ_2[4];
      vvp_net_t*net_;
};

class vvp_fun_and  : عام vvp_fun_boolean_ {

    عام:
      explicit vvp_fun_and(بنانشانی wid, bool invert);
      ~vvp_fun_and() override;

    خاص:
      فراغ لگاتار_لگاتار() override;
      bool invert_;
};

class vvp_fun_equiv : عام vvp_fun_boolean_ {

    عام:
      explicit vvp_fun_equiv();
      ~vvp_fun_equiv() override;

    خاص:
      فراغ لگاتار_لگاتار() override;
};

class vvp_fun_impl : عام vvp_fun_boolean_ {

    عام:
      explicit vvp_fun_impl();
      ~vvp_fun_impl() override;

    خاص:
      فراغ لگاتار_لگاتار() override;
};

/*
 * vvp_fun_buf_not_ is just a common hook for holding operands.
 */
class vvp_fun_buf_not_ : عام vvp_net_fun_t, محمي vvp_gen_event_s {

    عام:
      explicit vvp_fun_buf_not_(بنانشانی wid);
      ~vvp_fun_buf_not_() override;

      فراغ recv_vec4(vvp_net_ptr_t p, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;
      فراغ recv_vec4_pv(vvp_net_ptr_t p, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			بنانشانی ادھار, بنانشانی vwid, vvp_context_t) override;
      فراغ recv_real(vvp_net_ptr_t p, مزدوج real,
                     vvp_context_t ctx) override;

    محمي:
      vvp_vector4_t داخلہ_2;
      vvp_net_t*net_;
};

/*
 * The buffer functor is a very primitive functor that takes the input
 * from port-0 (and only port-0) and retransmits it as a vvp_vector4_t.
 * The retransmitted vector has all Z values changed to X, just like
 * the buf(Q,D) gate in Verilog.
 */
class vvp_fun_buf: عام vvp_fun_buf_not_ {

    عام:
      explicit vvp_fun_buf(بنانشانی wid);
      مجازی ~vvp_fun_buf() override;

    خاص:
      فراغ لگاتار_لگاتار() override;
};

/*
 * The vvp_fun_bufz is like the vvp_fun_buf, but it does not change
 * Z values to X -- it passes Z values unchanged.
 */
class vvp_fun_bufz: عام vvp_net_fun_t {

    عام:
      explicit vvp_fun_bufz();
      مجازی ~vvp_fun_bufz() override;

      فراغ recv_vec4(vvp_net_ptr_t p, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;
      فراغ recv_vec4_pv(vvp_net_ptr_t p, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			بنانشانی ادھار, بنانشانی vwid, vvp_context_t) override;
	//void recv_vec8(vvp_net_ptr_t port, const vvp_vector8_t&bit);
      فراغ recv_real(vvp_net_ptr_t p, مزدوج ثنائی_ہندسہ,
                     vvp_context_t) override;

    خاص:
};

/*
 * The vp_fun_buft is like the vvp_fun_bufz, but is completely
 * transparent to strengths.
 */
class vvp_fun_buft: عام vvp_fun_bufz {

    عام:
      فراغ recv_vec8(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ) override;
    خاص:
};

تعداد sel_type {SEL_PORT0, SEL_PORT1, SEL_BOTH};

/*
 * The muxz functor is an A-B mux device, with the data inputs on
 * ports 0 and 1. port 2 is the select input.
 *
 * The select input must be 1 bit wide. If it is 0, then the port-0
 * vector is passed out. If select is 1, then port-1 is passed
 * out. Otherwise, a vector is passed out that reflects x?: behavior
 * in Verilog. The width of the blended output is the width of the largest
 * input (port-0 or port-1) to enter the device. The narrow vector is
 * padded with X values.
 */
class vvp_fun_muxz : عام vvp_net_fun_t, خاص vvp_gen_event_s {

    عام:
      explicit vvp_fun_muxz(بنانشانی چوڑائی);
      مجازی ~vvp_fun_muxz() override;

      فراغ recv_vec4(vvp_net_ptr_t p, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;
      فراغ recv_vec4_pv(vvp_net_ptr_t p, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			بنانشانی ادھار, بنانشانی vwid, vvp_context_t) override;

    خاص:
      فراغ لگاتار_لگاتار() override;

    خاص:
      vvp_vector4_t a_;
      vvp_vector4_t b_;
      vvp_net_t*net_;
      sel_type select_;
      bool has_لگاتار;
};

class vvp_fun_muxr : عام vvp_net_fun_t, خاص vvp_gen_event_s {

    عام:
      explicit vvp_fun_muxr();
      مجازی ~vvp_fun_muxr() override;

      فراغ recv_vec4(vvp_net_ptr_t p, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;
      فراغ recv_real(vvp_net_ptr_t p, مزدوج ثنائی_ہندسہ,
                     vvp_context_t) override;

    خاص:
      فراغ لگاتار_لگاتار() override;

    خاص:
      مزدوج a_;
      مزدوج b_;
      vvp_net_t*net_;
      sel_type select_;
};

class vvp_fun_not: عام vvp_fun_buf_not_ {

    عام:
      explicit vvp_fun_not(بنانشانی wid);
      مجازی ~vvp_fun_not() override;

    خاص:
      فراغ لگاتار_لگاتار() override;
};

class vvp_fun_or  : عام vvp_fun_boolean_ {

    عام:
      explicit vvp_fun_or(بنانشانی wid, bool invert);
      ~vvp_fun_or() override;

    خاص:
      فراغ لگاتار_لگاتار() override;
      bool invert_;
};

class vvp_fun_xor  : عام vvp_fun_boolean_ {

    عام:
      explicit vvp_fun_xor(بنانشانی wid, bool invert);
      ~vvp_fun_xor() override;

    خاص:
      فراغ لگاتار_لگاتار() override;
      bool invert_;
};

#جے_انت /* IVL_logic_H */
