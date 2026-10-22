#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_logic_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_logic_H
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
class vvp_fun_boolean_ : ਜਨਤਕ vvp_net_fun_t, ਸੁਰੱਖਿਅਤ vvp_gen_event_s {

    ਜਨਤਕ:
      explicit vvp_fun_boolean_(ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ~vvp_fun_boolean_() override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t p, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t p, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t) override;
      ਖਾਲੀ recv_real(vvp_net_ptr_t p, ਦੁੱਗਣਾ real,
                     vvp_context_t) override;

    ਸੁਰੱਖਿਅਤ:
      vvp_vector4_t ਨਿਵੇਸ਼_2[4];
      vvp_net_t*net_;
};

class vvp_fun_and  : ਜਨਤਕ vvp_fun_boolean_ {

    ਜਨਤਕ:
      explicit vvp_fun_and(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, bool invert);
      ~vvp_fun_and() override;

    ਨਿਜੀ:
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() override;
      bool invert_;
};

class vvp_fun_equiv : ਜਨਤਕ vvp_fun_boolean_ {

    ਜਨਤਕ:
      explicit vvp_fun_equiv();
      ~vvp_fun_equiv() override;

    ਨਿਜੀ:
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() override;
};

class vvp_fun_impl : ਜਨਤਕ vvp_fun_boolean_ {

    ਜਨਤਕ:
      explicit vvp_fun_impl();
      ~vvp_fun_impl() override;

    ਨਿਜੀ:
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() override;
};

/*
 * vvp_fun_buf_not_ is just a common hook for holding operands.
 */
class vvp_fun_buf_not_ : ਜਨਤਕ vvp_net_fun_t, ਸੁਰੱਖਿਅਤ vvp_gen_event_s {

    ਜਨਤਕ:
      explicit vvp_fun_buf_not_(ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ~vvp_fun_buf_not_() override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t p, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t p, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t) override;
      ਖਾਲੀ recv_real(vvp_net_ptr_t p, ਦੁੱਗਣਾ real,
                     vvp_context_t ctx) override;

    ਸੁਰੱਖਿਅਤ:
      vvp_vector4_t ਨਿਵੇਸ਼_2;
      vvp_net_t*net_;
};

/*
 * The buffer functor is a very primitive functor that takes the input
 * from port-0 (and only port-0) and retransmits it as a vvp_vector4_t.
 * The retransmitted vector has all Z values changed to X, just like
 * the buf(Q,D) gate in Verilog.
 */
class vvp_fun_buf: ਜਨਤਕ vvp_fun_buf_not_ {

    ਜਨਤਕ:
      explicit vvp_fun_buf(ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ਆਭਾਸੀ ~vvp_fun_buf() override;

    ਨਿਜੀ:
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() override;
};

/*
 * The vvp_fun_bufz is like the vvp_fun_buf, but it does not change
 * Z values to X -- it passes Z values unchanged.
 */
class vvp_fun_bufz: ਜਨਤਕ vvp_net_fun_t {

    ਜਨਤਕ:
      explicit vvp_fun_bufz();
      ਆਭਾਸੀ ~vvp_fun_bufz() override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t p, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t p, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t) override;
	//void recv_vec8(vvp_net_ptr_t port, const vvp_vector8_t&bit);
      ਖਾਲੀ recv_real(vvp_net_ptr_t p, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

    ਨਿਜੀ:
};

/*
 * The vp_fun_buft is like the vvp_fun_bufz, but is completely
 * transparent to strengths.
 */
class vvp_fun_buft: ਜਨਤਕ vvp_fun_bufz {

    ਜਨਤਕ:
      ਖਾਲੀ recv_vec8(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ) override;
    ਨਿਜੀ:
};

ਗਿਣਤੀ sel_type {SEL_PORT0, SEL_PORT1, SEL_BOTH};

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
class vvp_fun_muxz : ਜਨਤਕ vvp_net_fun_t, ਨਿਜੀ vvp_gen_event_s {

    ਜਨਤਕ:
      explicit vvp_fun_muxz(ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ);
      ਆਭਾਸੀ ~vvp_fun_muxz() override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t p, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t p, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t) override;

    ਨਿਜੀ:
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() override;

    ਨਿਜੀ:
      vvp_vector4_t a_;
      vvp_vector4_t b_;
      vvp_net_t*net_;
      sel_type select_;
      bool has_ਲੜੀਵਾਰ;
};

class vvp_fun_muxr : ਜਨਤਕ vvp_net_fun_t, ਨਿਜੀ vvp_gen_event_s {

    ਜਨਤਕ:
      explicit vvp_fun_muxr();
      ਆਭਾਸੀ ~vvp_fun_muxr() override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t p, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
      ਖਾਲੀ recv_real(vvp_net_ptr_t p, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

    ਨਿਜੀ:
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() override;

    ਨਿਜੀ:
      ਦੁੱਗਣਾ a_;
      ਦੁੱਗਣਾ b_;
      vvp_net_t*net_;
      sel_type select_;
};

class vvp_fun_not: ਜਨਤਕ vvp_fun_buf_not_ {

    ਜਨਤਕ:
      explicit vvp_fun_not(ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ਆਭਾਸੀ ~vvp_fun_not() override;

    ਨਿਜੀ:
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() override;
};

class vvp_fun_or  : ਜਨਤਕ vvp_fun_boolean_ {

    ਜਨਤਕ:
      explicit vvp_fun_or(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, bool invert);
      ~vvp_fun_or() override;

    ਨਿਜੀ:
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() override;
      bool invert_;
};

class vvp_fun_xor  : ਜਨਤਕ vvp_fun_boolean_ {

    ਜਨਤਕ:
      explicit vvp_fun_xor(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, bool invert);
      ~vvp_fun_xor() override;

    ਨਿਜੀ:
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() override;
      bool invert_;
};

#ਜੇ_ਅੰਤ /* IVL_logic_H */
