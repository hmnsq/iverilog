#اگر_غیر_تعریف_شدہ IVL_logic_H
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
class vvp_fun_boolean_ : عوامی vvp_net_fun_t, محفوظ vvp_gen_event_s {

    عوامی:
      explicit vvp_fun_boolean_(غیرعلامتی wid);
      ~vvp_fun_boolean_() override;

      خالی recv_vec4(vvp_net_ptr_t p, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;
      خالی recv_vec4_pv(vvp_net_ptr_t p, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t) override;
      خالی recv_real(vvp_net_ptr_t p, دوہرا real,
                     vvp_context_t) override;

    محفوظ:
      vvp_vector4_t داخلہ_2[4];
      vvp_net_t*net_;
};

class vvp_fun_and  : عوامی vvp_fun_boolean_ {

    عوامی:
      explicit vvp_fun_and(غیرعلامتی wid, bool invert);
      ~vvp_fun_and() override;

    نجی:
      خالی تسلسل_تسلسل() override;
      bool invert_;
};

class vvp_fun_equiv : عوامی vvp_fun_boolean_ {

    عوامی:
      explicit vvp_fun_equiv();
      ~vvp_fun_equiv() override;

    نجی:
      خالی تسلسل_تسلسل() override;
};

class vvp_fun_impl : عوامی vvp_fun_boolean_ {

    عوامی:
      explicit vvp_fun_impl();
      ~vvp_fun_impl() override;

    نجی:
      خالی تسلسل_تسلسل() override;
};

/*
 * vvp_fun_buf_not_ is just a common hook for holding operands.
 */
class vvp_fun_buf_not_ : عوامی vvp_net_fun_t, محفوظ vvp_gen_event_s {

    عوامی:
      explicit vvp_fun_buf_not_(غیرعلامتی wid);
      ~vvp_fun_buf_not_() override;

      خالی recv_vec4(vvp_net_ptr_t p, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;
      خالی recv_vec4_pv(vvp_net_ptr_t p, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t) override;
      خالی recv_real(vvp_net_ptr_t p, دوہرا real,
                     vvp_context_t ctx) override;

    محفوظ:
      vvp_vector4_t داخلہ_2;
      vvp_net_t*net_;
};

/*
 * The buffer functor is a very primitive functor that takes the input
 * from port-0 (and only port-0) and retransmits it as a vvp_vector4_t.
 * The retransmitted vector has all Z values changed to X, just like
 * the buf(Q,D) gate in Verilog.
 */
class vvp_fun_buf: عوامی vvp_fun_buf_not_ {

    عوامی:
      explicit vvp_fun_buf(غیرعلامتی wid);
      مجازی ~vvp_fun_buf() override;

    نجی:
      خالی تسلسل_تسلسل() override;
};

/*
 * The vvp_fun_bufz is like the vvp_fun_buf, but it does not change
 * Z values to X -- it passes Z values unchanged.
 */
class vvp_fun_bufz: عوامی vvp_net_fun_t {

    عوامی:
      explicit vvp_fun_bufz();
      مجازی ~vvp_fun_bufz() override;

      خالی recv_vec4(vvp_net_ptr_t p, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;
      خالی recv_vec4_pv(vvp_net_ptr_t p, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t) override;
	//void recv_vec8(vvp_net_ptr_t port, const vvp_vector8_t&bit);
      خالی recv_real(vvp_net_ptr_t p, دوہرا ثنائی_ہندسہ,
                     vvp_context_t) override;

    نجی:
};

/*
 * The vp_fun_buft is like the vvp_fun_bufz, but is completely
 * transparent to strengths.
 */
class vvp_fun_buft: عوامی vvp_fun_bufz {

    عوامی:
      خالی recv_vec8(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ) override;
    نجی:
};

شمار sel_type {SEL_PORT0, SEL_PORT1, SEL_BOTH};

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
class vvp_fun_muxz : عوامی vvp_net_fun_t, نجی vvp_gen_event_s {

    عوامی:
      explicit vvp_fun_muxz(غیرعلامتی چوڑائی);
      مجازی ~vvp_fun_muxz() override;

      خالی recv_vec4(vvp_net_ptr_t p, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;
      خالی recv_vec4_pv(vvp_net_ptr_t p, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t) override;

    نجی:
      خالی تسلسل_تسلسل() override;

    نجی:
      vvp_vector4_t a_;
      vvp_vector4_t b_;
      vvp_net_t*net_;
      sel_type select_;
      bool has_تسلسل;
};

class vvp_fun_muxr : عوامی vvp_net_fun_t, نجی vvp_gen_event_s {

    عوامی:
      explicit vvp_fun_muxr();
      مجازی ~vvp_fun_muxr() override;

      خالی recv_vec4(vvp_net_ptr_t p, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;
      خالی recv_real(vvp_net_ptr_t p, دوہرا ثنائی_ہندسہ,
                     vvp_context_t) override;

    نجی:
      خالی تسلسل_تسلسل() override;

    نجی:
      دوہرا a_;
      دوہرا b_;
      vvp_net_t*net_;
      sel_type select_;
};

class vvp_fun_not: عوامی vvp_fun_buf_not_ {

    عوامی:
      explicit vvp_fun_not(غیرعلامتی wid);
      مجازی ~vvp_fun_not() override;

    نجی:
      خالی تسلسل_تسلسل() override;
};

class vvp_fun_or  : عوامی vvp_fun_boolean_ {

    عوامی:
      explicit vvp_fun_or(غیرعلامتی wid, bool invert);
      ~vvp_fun_or() override;

    نجی:
      خالی تسلسل_تسلسل() override;
      bool invert_;
};

class vvp_fun_xor  : عوامی vvp_fun_boolean_ {

    عوامی:
      explicit vvp_fun_xor(غیرعلامتی wid, bool invert);
      ~vvp_fun_xor() override;

    نجی:
      خالی تسلسل_تسلسل() override;
      bool invert_;
};

#اگر_ختم /* IVL_logic_H */
