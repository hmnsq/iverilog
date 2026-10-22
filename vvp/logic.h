#నిర్వచితం_కాకపోతే IVL_logic_H
#నిర్వచించు IVL_logic_H
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
class vvp_fun_boolean_ : బహిరంగ vvp_net_fun_t, రక్షిత vvp_gen_event_s {

    బహిరంగ:
      explicit vvp_fun_boolean_(గుర్తులేని wid);
      ~vvp_fun_boolean_() override;

      శూన్య recv_vec4(vvp_net_ptr_t p, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;
      శూన్య recv_vec4_pv(vvp_net_ptr_t p, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t) override;
      శూన్య recv_real(vvp_net_ptr_t p, ద్విగుణ real,
                     vvp_context_t) override;

    రక్షిత:
      vvp_vector4_t ఆదానం_2[4];
      vvp_net_t*net_;
};

class vvp_fun_and  : బహిరంగ vvp_fun_boolean_ {

    బహిరంగ:
      explicit vvp_fun_and(గుర్తులేని wid, bool invert);
      ~vvp_fun_and() override;

    అంతరంగ:
      శూన్య నిరంతరం_నిరంతరం() override;
      bool invert_;
};

class vvp_fun_equiv : బహిరంగ vvp_fun_boolean_ {

    బహిరంగ:
      explicit vvp_fun_equiv();
      ~vvp_fun_equiv() override;

    అంతరంగ:
      శూన్య నిరంతరం_నిరంతరం() override;
};

class vvp_fun_impl : బహిరంగ vvp_fun_boolean_ {

    బహిరంగ:
      explicit vvp_fun_impl();
      ~vvp_fun_impl() override;

    అంతరంగ:
      శూన్య నిరంతరం_నిరంతరం() override;
};

/*
 * vvp_fun_buf_not_ is just a common hook for holding operands.
 */
class vvp_fun_buf_not_ : బహిరంగ vvp_net_fun_t, రక్షిత vvp_gen_event_s {

    బహిరంగ:
      explicit vvp_fun_buf_not_(గుర్తులేని wid);
      ~vvp_fun_buf_not_() override;

      శూన్య recv_vec4(vvp_net_ptr_t p, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;
      శూన్య recv_vec4_pv(vvp_net_ptr_t p, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t) override;
      శూన్య recv_real(vvp_net_ptr_t p, ద్విగుణ real,
                     vvp_context_t ctx) override;

    రక్షిత:
      vvp_vector4_t ఆదానం_2;
      vvp_net_t*net_;
};

/*
 * The buffer functor is a very primitive functor that takes the input
 * from port-0 (and only port-0) and retransmits it as a vvp_vector4_t.
 * The retransmitted vector has all Z values changed to X, just like
 * the buf(Q,D) gate in Verilog.
 */
class vvp_fun_buf: బహిరంగ vvp_fun_buf_not_ {

    బహిరంగ:
      explicit vvp_fun_buf(గుర్తులేని wid);
      మాయా ~vvp_fun_buf() override;

    అంతరంగ:
      శూన్య నిరంతరం_నిరంతరం() override;
};

/*
 * The vvp_fun_bufz is like the vvp_fun_buf, but it does not change
 * Z values to X -- it passes Z values unchanged.
 */
class vvp_fun_bufz: బహిరంగ vvp_net_fun_t {

    బహిరంగ:
      explicit vvp_fun_bufz();
      మాయా ~vvp_fun_bufz() override;

      శూన్య recv_vec4(vvp_net_ptr_t p, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;
      శూన్య recv_vec4_pv(vvp_net_ptr_t p, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t) override;
	//void recv_vec8(vvp_net_ptr_t port, const vvp_vector8_t&bit);
      శూన్య recv_real(vvp_net_ptr_t p, ద్విగుణ బిట్టు,
                     vvp_context_t) override;

    అంతరంగ:
};

/*
 * The vp_fun_buft is like the vvp_fun_bufz, but is completely
 * transparent to strengths.
 */
class vvp_fun_buft: బహిరంగ vvp_fun_bufz {

    బహిరంగ:
      శూన్య recv_vec8(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector8_t&బిట్టు) override;
    అంతరంగ:
};

గణన sel_type {SEL_PORT0, SEL_PORT1, SEL_BOTH};

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
class vvp_fun_muxz : బహిరంగ vvp_net_fun_t, అంతరంగ vvp_gen_event_s {

    బహిరంగ:
      explicit vvp_fun_muxz(గుర్తులేని వెడల్పు);
      మాయా ~vvp_fun_muxz() override;

      శూన్య recv_vec4(vvp_net_ptr_t p, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;
      శూన్య recv_vec4_pv(vvp_net_ptr_t p, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t) override;

    అంతరంగ:
      శూన్య నిరంతరం_నిరంతరం() override;

    అంతరంగ:
      vvp_vector4_t a_;
      vvp_vector4_t b_;
      vvp_net_t*net_;
      sel_type select_;
      bool has_నిరంతరం;
};

class vvp_fun_muxr : బహిరంగ vvp_net_fun_t, అంతరంగ vvp_gen_event_s {

    బహిరంగ:
      explicit vvp_fun_muxr();
      మాయా ~vvp_fun_muxr() override;

      శూన్య recv_vec4(vvp_net_ptr_t p, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;
      శూన్య recv_real(vvp_net_ptr_t p, ద్విగుణ బిట్టు,
                     vvp_context_t) override;

    అంతరంగ:
      శూన్య నిరంతరం_నిరంతరం() override;

    అంతరంగ:
      ద్విగుణ a_;
      ద్విగుణ b_;
      vvp_net_t*net_;
      sel_type select_;
};

class vvp_fun_not: బహిరంగ vvp_fun_buf_not_ {

    బహిరంగ:
      explicit vvp_fun_not(గుర్తులేని wid);
      మాయా ~vvp_fun_not() override;

    అంతరంగ:
      శూన్య నిరంతరం_నిరంతరం() override;
};

class vvp_fun_or  : బహిరంగ vvp_fun_boolean_ {

    బహిరంగ:
      explicit vvp_fun_or(గుర్తులేని wid, bool invert);
      ~vvp_fun_or() override;

    అంతరంగ:
      శూన్య నిరంతరం_నిరంతరం() override;
      bool invert_;
};

class vvp_fun_xor  : బహిరంగ vvp_fun_boolean_ {

    బహిరంగ:
      explicit vvp_fun_xor(గుర్తులేని wid, bool invert);
      ~vvp_fun_xor() override;

    అంతరంగ:
      శూన్య నిరంతరం_నిరంతరం() override;
      bool invert_;
};

#అయితే_ముగింపు /* IVL_logic_H */
