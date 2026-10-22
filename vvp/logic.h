#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_logic_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_logic_H
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
class vvp_fun_boolean_ : ಸಾರ್ವಜನಿಕ vvp_net_fun_t, ಸಂರಕ್ಷಿತ vvp_gen_event_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_boolean_(ಚಿಹ್ನೆರಹಿತ wid);
      ~vvp_fun_boolean_() override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t p, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t) override;
      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t p, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t) override;
      ಶೂನ್ಯ recv_real(vvp_net_ptr_t p, ದ್ವಿಗುಣ real,
                     vvp_context_t) override;

    ಸಂರಕ್ಷಿತ:
      vvp_vector4_t ಒಳಹರಿವು_2[4];
      vvp_net_t*net_;
};

class vvp_fun_and  : ಸಾರ್ವಜನಿಕ vvp_fun_boolean_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_and(ಚಿಹ್ನೆರಹಿತ wid, bool invert);
      ~vvp_fun_and() override;

    ಖಾಸಗಿ:
      ಶೂನ್ಯ ಸತತ_ಸತತ() override;
      bool invert_;
};

class vvp_fun_equiv : ಸಾರ್ವಜನಿಕ vvp_fun_boolean_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_equiv();
      ~vvp_fun_equiv() override;

    ಖಾಸಗಿ:
      ಶೂನ್ಯ ಸತತ_ಸತತ() override;
};

class vvp_fun_impl : ಸಾರ್ವಜನಿಕ vvp_fun_boolean_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_impl();
      ~vvp_fun_impl() override;

    ಖಾಸಗಿ:
      ಶೂನ್ಯ ಸತತ_ಸತತ() override;
};

/*
 * vvp_fun_buf_not_ is just a common hook for holding operands.
 */
class vvp_fun_buf_not_ : ಸಾರ್ವಜನಿಕ vvp_net_fun_t, ಸಂರಕ್ಷಿತ vvp_gen_event_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_buf_not_(ಚಿಹ್ನೆರಹಿತ wid);
      ~vvp_fun_buf_not_() override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t p, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t) override;
      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t p, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t) override;
      ಶೂನ್ಯ recv_real(vvp_net_ptr_t p, ದ್ವಿಗುಣ real,
                     vvp_context_t ctx) override;

    ಸಂರಕ್ಷಿತ:
      vvp_vector4_t ಒಳಹರಿವು_2;
      vvp_net_t*net_;
};

/*
 * The buffer functor is a very primitive functor that takes the input
 * from port-0 (and only port-0) and retransmits it as a vvp_vector4_t.
 * The retransmitted vector has all Z values changed to X, just like
 * the buf(Q,D) gate in Verilog.
 */
class vvp_fun_buf: ಸಾರ್ವಜನಿಕ vvp_fun_buf_not_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_buf(ಚಿಹ್ನೆರಹಿತ wid);
      ವಾಸ್ತವಿಕ ~vvp_fun_buf() override;

    ಖಾಸಗಿ:
      ಶೂನ್ಯ ಸತತ_ಸತತ() override;
};

/*
 * The vvp_fun_bufz is like the vvp_fun_buf, but it does not change
 * Z values to X -- it passes Z values unchanged.
 */
class vvp_fun_bufz: ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_bufz();
      ವಾಸ್ತವಿಕ ~vvp_fun_bufz() override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t p, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t) override;
      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t p, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t) override;
	//void recv_vec8(vvp_net_ptr_t port, const vvp_vector8_t&bit);
      ಶೂನ್ಯ recv_real(vvp_net_ptr_t p, ದ್ವಿಗುಣ ಬಿಟ್,
                     vvp_context_t) override;

    ಖಾಸಗಿ:
};

/*
 * The vp_fun_buft is like the vvp_fun_bufz, but is completely
 * transparent to strengths.
 */
class vvp_fun_buft: ಸಾರ್ವಜನಿಕ vvp_fun_bufz {

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ recv_vec8(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್) override;
    ಖಾಸಗಿ:
};

ಎಣಿಕೆ sel_type {SEL_PORT0, SEL_PORT1, SEL_BOTH};

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
class vvp_fun_muxz : ಸಾರ್ವಜನಿಕ vvp_net_fun_t, ಖಾಸಗಿ vvp_gen_event_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_muxz(ಚಿಹ್ನೆರಹಿತ ಅಗಲ);
      ವಾಸ್ತವಿಕ ~vvp_fun_muxz() override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t p, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t) override;
      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t p, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t) override;

    ಖಾಸಗಿ:
      ಶೂನ್ಯ ಸತತ_ಸತತ() override;

    ಖಾಸಗಿ:
      vvp_vector4_t a_;
      vvp_vector4_t b_;
      vvp_net_t*net_;
      sel_type select_;
      bool has_ಸತತ;
};

class vvp_fun_muxr : ಸಾರ್ವಜನಿಕ vvp_net_fun_t, ಖಾಸಗಿ vvp_gen_event_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_muxr();
      ವಾಸ್ತವಿಕ ~vvp_fun_muxr() override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t p, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t) override;
      ಶೂನ್ಯ recv_real(vvp_net_ptr_t p, ದ್ವಿಗುಣ ಬಿಟ್,
                     vvp_context_t) override;

    ಖಾಸಗಿ:
      ಶೂನ್ಯ ಸತತ_ಸತತ() override;

    ಖಾಸಗಿ:
      ದ್ವಿಗುಣ a_;
      ದ್ವಿಗುಣ b_;
      vvp_net_t*net_;
      sel_type select_;
};

class vvp_fun_not: ಸಾರ್ವಜನಿಕ vvp_fun_buf_not_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_not(ಚಿಹ್ನೆರಹಿತ wid);
      ವಾಸ್ತವಿಕ ~vvp_fun_not() override;

    ಖಾಸಗಿ:
      ಶೂನ್ಯ ಸತತ_ಸತತ() override;
};

class vvp_fun_or  : ಸಾರ್ವಜನಿಕ vvp_fun_boolean_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_or(ಚಿಹ್ನೆರಹಿತ wid, bool invert);
      ~vvp_fun_or() override;

    ಖಾಸಗಿ:
      ಶೂನ್ಯ ಸತತ_ಸತತ() override;
      bool invert_;
};

class vvp_fun_xor  : ಸಾರ್ವಜನಿಕ vvp_fun_boolean_ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_xor(ಚಿಹ್ನೆರಹಿತ wid, bool invert);
      ~vvp_fun_xor() override;

    ಖಾಸಗಿ:
      ಶೂನ್ಯ ಸತತ_ಸತತ() override;
      bool invert_;
};

#ಆದರೆ_ಅಂತ್ಯ /* IVL_logic_H */
