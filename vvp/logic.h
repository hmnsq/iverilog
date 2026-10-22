#ifndef IVL_logic_H
#define IVL_logic_H
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
class vvp_fun_boolean_ : പൊതു vvp_net_fun_t, സംരക്ഷിത vvp_gen_event_s {

    പൊതു:
      explicit vvp_fun_boolean_(unsigned wid);
      ~vvp_fun_boolean_() override;

      ശൂന്യം recv_vec4(vvp_net_ptr_t p, സ്ഥിരം vvp_vector4_t&bit,
                     vvp_context_t) override;
      ശൂന്യം recv_vec4_pv(vvp_net_ptr_t p, സ്ഥിരം vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;
      ശൂന്യം recv_real(vvp_net_ptr_t p, ഇരട്ട real,
                     vvp_context_t) override;

    സംരക്ഷിത:
      vvp_vector4_t നിവേശം_2[4];
      vvp_net_t*net_;
};

class vvp_fun_and  : പൊതു vvp_fun_boolean_ {

    പൊതു:
      explicit vvp_fun_and(unsigned wid, bool invert);
      ~vvp_fun_and() override;

    സ്വകാര്യ:
      ശൂന്യം run_run() override;
      bool invert_;
};

class vvp_fun_equiv : പൊതു vvp_fun_boolean_ {

    പൊതു:
      explicit vvp_fun_equiv();
      ~vvp_fun_equiv() override;

    സ്വകാര്യ:
      ശൂന്യം run_run() override;
};

class vvp_fun_impl : പൊതു vvp_fun_boolean_ {

    പൊതു:
      explicit vvp_fun_impl();
      ~vvp_fun_impl() override;

    സ്വകാര്യ:
      ശൂന്യം run_run() override;
};

/*
 * vvp_fun_buf_not_ is just a common hook for holding operands.
 */
class vvp_fun_buf_not_ : പൊതു vvp_net_fun_t, സംരക്ഷിത vvp_gen_event_s {

    പൊതു:
      explicit vvp_fun_buf_not_(unsigned wid);
      ~vvp_fun_buf_not_() override;

      ശൂന്യം recv_vec4(vvp_net_ptr_t p, സ്ഥിരം vvp_vector4_t&bit,
                     vvp_context_t) override;
      ശൂന്യം recv_vec4_pv(vvp_net_ptr_t p, സ്ഥിരം vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;
      ശൂന്യം recv_real(vvp_net_ptr_t p, ഇരട്ട real,
                     vvp_context_t ctx) override;

    സംരക്ഷിത:
      vvp_vector4_t നിവേശം_2;
      vvp_net_t*net_;
};

/*
 * The buffer functor is a very primitive functor that takes the input
 * from port-0 (and only port-0) and retransmits it as a vvp_vector4_t.
 * The retransmitted vector has all Z values changed to X, just like
 * the buf(Q,D) gate in Verilog.
 */
class vvp_fun_buf: പൊതു vvp_fun_buf_not_ {

    പൊതു:
      explicit vvp_fun_buf(unsigned wid);
      മായാ ~vvp_fun_buf() override;

    സ്വകാര്യ:
      ശൂന്യം run_run() override;
};

/*
 * The vvp_fun_bufz is like the vvp_fun_buf, but it does not change
 * Z values to X -- it passes Z values unchanged.
 */
class vvp_fun_bufz: പൊതു vvp_net_fun_t {

    പൊതു:
      explicit vvp_fun_bufz();
      മായാ ~vvp_fun_bufz() override;

      ശൂന്യം recv_vec4(vvp_net_ptr_t p, സ്ഥിരം vvp_vector4_t&bit,
                     vvp_context_t) override;
      ശൂന്യം recv_vec4_pv(vvp_net_ptr_t p, സ്ഥിരം vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;
	//void recv_vec8(vvp_net_ptr_t port, const vvp_vector8_t&bit);
      ശൂന്യം recv_real(vvp_net_ptr_t p, ഇരട്ട bit,
                     vvp_context_t) override;

    സ്വകാര്യ:
};

/*
 * The vp_fun_buft is like the vvp_fun_bufz, but is completely
 * transparent to strengths.
 */
class vvp_fun_buft: പൊതു vvp_fun_bufz {

    പൊതു:
      ശൂന്യം recv_vec8(vvp_net_ptr_t port, സ്ഥിരം vvp_vector8_t&bit) override;
    സ്വകാര്യ:
};

എണ്ണൽ sel_type {SEL_PORT0, SEL_PORT1, SEL_BOTH};

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
class vvp_fun_muxz : പൊതു vvp_net_fun_t, സ്വകാര്യ vvp_gen_event_s {

    പൊതു:
      explicit vvp_fun_muxz(unsigned width);
      മായാ ~vvp_fun_muxz() override;

      ശൂന്യം recv_vec4(vvp_net_ptr_t p, സ്ഥിരം vvp_vector4_t&bit,
                     vvp_context_t) override;
      ശൂന്യം recv_vec4_pv(vvp_net_ptr_t p, സ്ഥിരം vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;

    സ്വകാര്യ:
      ശൂന്യം run_run() override;

    സ്വകാര്യ:
      vvp_vector4_t a_;
      vvp_vector4_t b_;
      vvp_net_t*net_;
      sel_type select_;
      bool has_run_;
};

class vvp_fun_muxr : പൊതു vvp_net_fun_t, സ്വകാര്യ vvp_gen_event_s {

    പൊതു:
      explicit vvp_fun_muxr();
      മായാ ~vvp_fun_muxr() override;

      ശൂന്യം recv_vec4(vvp_net_ptr_t p, സ്ഥിരം vvp_vector4_t&bit,
                     vvp_context_t) override;
      ശൂന്യം recv_real(vvp_net_ptr_t p, ഇരട്ട bit,
                     vvp_context_t) override;

    സ്വകാര്യ:
      ശൂന്യം run_run() override;

    സ്വകാര്യ:
      ഇരട്ട a_;
      ഇരട്ട b_;
      vvp_net_t*net_;
      sel_type select_;
};

class vvp_fun_not: പൊതു vvp_fun_buf_not_ {

    പൊതു:
      explicit vvp_fun_not(unsigned wid);
      മായാ ~vvp_fun_not() override;

    സ്വകാര്യ:
      ശൂന്യം run_run() override;
};

class vvp_fun_or  : പൊതു vvp_fun_boolean_ {

    പൊതു:
      explicit vvp_fun_or(unsigned wid, bool invert);
      ~vvp_fun_or() override;

    സ്വകാര്യ:
      ശൂന്യം run_run() override;
      bool invert_;
};

class vvp_fun_xor  : പൊതു vvp_fun_boolean_ {

    പൊതു:
      explicit vvp_fun_xor(unsigned wid, bool invert);
      ~vvp_fun_xor() override;

    സ്വകാര്യ:
      ശൂന്യം run_run() override;
      bool invert_;
};

#endif /* IVL_logic_H */
