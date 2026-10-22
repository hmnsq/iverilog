#अगर_अघोषित IVL_logic_H
#घोषणा IVL_logic_H
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
class vvp_fun_boolean_ : खुला vvp_net_fun_t, रक्षित vvp_gen_event_s {

    खुला:
      explicit vvp_fun_boolean_(अचिन्हित wid);
      ~vvp_fun_boolean_() override;

      व्योम recv_vec4(vvp_net_ptr_t p, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;
      व्योम recv_vec4_pv(vvp_net_ptr_t p, स्थिर vvp_vector4_t&bit,
			अचिन्हित base, अचिन्हित vwid, vvp_context_t) override;
      व्योम recv_real(vvp_net_ptr_t p, द्विगुन real,
                     vvp_context_t) override;

    रक्षित:
      vvp_vector4_t निवेश_2[4];
      vvp_net_t*net_;
};

class vvp_fun_and  : खुला vvp_fun_boolean_ {

    खुला:
      explicit vvp_fun_and(अचिन्हित wid, bool invert);
      ~vvp_fun_and() override;

    निजी:
      व्योम run_run() override;
      bool invert_;
};

class vvp_fun_equiv : खुला vvp_fun_boolean_ {

    खुला:
      explicit vvp_fun_equiv();
      ~vvp_fun_equiv() override;

    निजी:
      व्योम run_run() override;
};

class vvp_fun_impl : खुला vvp_fun_boolean_ {

    खुला:
      explicit vvp_fun_impl();
      ~vvp_fun_impl() override;

    निजी:
      व्योम run_run() override;
};

/*
 * vvp_fun_buf_not_ is just a common hook for holding operands.
 */
class vvp_fun_buf_not_ : खुला vvp_net_fun_t, रक्षित vvp_gen_event_s {

    खुला:
      explicit vvp_fun_buf_not_(अचिन्हित wid);
      ~vvp_fun_buf_not_() override;

      व्योम recv_vec4(vvp_net_ptr_t p, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;
      व्योम recv_vec4_pv(vvp_net_ptr_t p, स्थिर vvp_vector4_t&bit,
			अचिन्हित base, अचिन्हित vwid, vvp_context_t) override;
      व्योम recv_real(vvp_net_ptr_t p, द्विगुन real,
                     vvp_context_t ctx) override;

    रक्षित:
      vvp_vector4_t निवेश_2;
      vvp_net_t*net_;
};

/*
 * The buffer functor is a very primitive functor that takes the input
 * from port-0 (and only port-0) and retransmits it as a vvp_vector4_t.
 * The retransmitted vector has all Z values changed to X, just like
 * the buf(Q,D) gate in Verilog.
 */
class vvp_fun_buf: खुला vvp_fun_buf_not_ {

    खुला:
      explicit vvp_fun_buf(अचिन्हित wid);
      भव ~vvp_fun_buf() override;

    निजी:
      व्योम run_run() override;
};

/*
 * The vvp_fun_bufz is like the vvp_fun_buf, but it does not change
 * Z values to X -- it passes Z values unchanged.
 */
class vvp_fun_bufz: खुला vvp_net_fun_t {

    खुला:
      explicit vvp_fun_bufz();
      भव ~vvp_fun_bufz() override;

      व्योम recv_vec4(vvp_net_ptr_t p, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;
      व्योम recv_vec4_pv(vvp_net_ptr_t p, स्थिर vvp_vector4_t&bit,
			अचिन्हित base, अचिन्हित vwid, vvp_context_t) override;
	//void recv_vec8(vvp_net_ptr_t port, const vvp_vector8_t&bit);
      व्योम recv_real(vvp_net_ptr_t p, द्विगुन bit,
                     vvp_context_t) override;

    निजी:
};

/*
 * The vp_fun_buft is like the vvp_fun_bufz, but is completely
 * transparent to strengths.
 */
class vvp_fun_buft: खुला vvp_fun_bufz {

    खुला:
      व्योम recv_vec8(vvp_net_ptr_t port, स्थिर vvp_vector8_t&bit) override;
    निजी:
};

क्रमागत sel_type {SEL_PORT0, SEL_PORT1, SEL_BOTH};

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
class vvp_fun_muxz : खुला vvp_net_fun_t, निजी vvp_gen_event_s {

    खुला:
      explicit vvp_fun_muxz(अचिन्हित width);
      भव ~vvp_fun_muxz() override;

      व्योम recv_vec4(vvp_net_ptr_t p, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;
      व्योम recv_vec4_pv(vvp_net_ptr_t p, स्थिर vvp_vector4_t&bit,
			अचिन्हित base, अचिन्हित vwid, vvp_context_t) override;

    निजी:
      व्योम run_run() override;

    निजी:
      vvp_vector4_t a_;
      vvp_vector4_t b_;
      vvp_net_t*net_;
      sel_type select_;
      bool has_run_;
};

class vvp_fun_muxr : खुला vvp_net_fun_t, निजी vvp_gen_event_s {

    खुला:
      explicit vvp_fun_muxr();
      भव ~vvp_fun_muxr() override;

      व्योम recv_vec4(vvp_net_ptr_t p, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;
      व्योम recv_real(vvp_net_ptr_t p, द्विगुन bit,
                     vvp_context_t) override;

    निजी:
      व्योम run_run() override;

    निजी:
      द्विगुन a_;
      द्विगुन b_;
      vvp_net_t*net_;
      sel_type select_;
};

class vvp_fun_not: खुला vvp_fun_buf_not_ {

    खुला:
      explicit vvp_fun_not(अचिन्हित wid);
      भव ~vvp_fun_not() override;

    निजी:
      व्योम run_run() override;
};

class vvp_fun_or  : खुला vvp_fun_boolean_ {

    खुला:
      explicit vvp_fun_or(अचिन्हित wid, bool invert);
      ~vvp_fun_or() override;

    निजी:
      व्योम run_run() override;
      bool invert_;
};

class vvp_fun_xor  : खुला vvp_fun_boolean_ {

    खुला:
      explicit vvp_fun_xor(अचिन्हित wid, bool invert);
      ~vvp_fun_xor() override;

    निजी:
      व्योम run_run() override;
      bool invert_;
};

#पूर्ण_अगर /* IVL_logic_H */
