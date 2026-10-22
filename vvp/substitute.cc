/*
 * Copyright (c) 2016-2025 Stephen Williams (steve@icarus.com)
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

# include  "compile.h"
# include  "vvp_net.h"
# include  <cstdlib>
# include  <iostream>
# include  <cassert>


తరగతి vvp_fun_substitute : బహిరంగ vvp_net_fun_t {

    బహిరంగ:
      vvp_fun_substitute(గుర్తులేని wid, గుర్తులేని soff, గుర్తులేని swid);
      ~vvp_fun_substitute() override;

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు, vvp_context_t) override;

      శూన్య recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t ctx) override;

    అంతరంగ:
      గుర్తులేని wid_;
      గుర్తులేని soff_;
      గుర్తులేని swid_;

      vvp_vector4_t val_;
};

vvp_fun_substitute::vvp_fun_substitute(గుర్తులేని wid, గుర్తులేని soff, గుర్తులేని swid)
: wid_(wid), soff_(soff), swid_(swid), val_(wid)
{
      కొరకు (గుర్తులేని idx = 0 ; idx < val_.size() ; idx += 1)
	    val_.సమితి_బిట్టు(idx, BIT4_Z);
}

vvp_fun_substitute::~vvp_fun_substitute()
{
}

శూన్య vvp_fun_substitute::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
				   vvp_context_t)
{
      గుర్తులేని pdx = పోర్టు.పోర్టు();
      ప్రతిజ్ఞా(pdx <= 1);

      ఒకవేళ (pdx == 0) {
	    ప్రతిజ్ఞా(బిట్టు.size() == wid_);

	    కొరకు (గుర్తులేని idx = 0 ; idx < wid_ ; idx += 1) {
		  ఒకవేళ (idx >= soff_ && idx < (soff_+swid_))
			కొనసాగు;

		  val_.సమితి_బిట్టు(idx, బిట్టు.value(idx));
	    }

      } లేకపోతే {
	    ప్రతిజ్ఞా(బిట్టు.size() == swid_);

	    కొరకు (గుర్తులేని idx = 0 ; idx < swid_ ; idx += 1)
		  val_.సమితి_బిట్టు(idx+soff_, బిట్టు.value(idx));
      }

      పోర్టు.ptr()->send_vec4(val_, 0);
}

శూన్య vvp_fun_substitute::recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
				      గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, బిట్టు, పునాది, vwid, ctx);
}

శూన్య compile_substitute(అక్షరం*label, గుర్తులేని width,
			గుర్తులేని soff, గుర్తులేని swidth,
			గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      vvp_fun_substitute*fun = కొత్త vvp_fun_substitute(width, soff, swidth);

      vvp_net_t*net = కొత్త vvp_net_t;
      net->fun = fun;

      define_functor_చిహ్నం(label, net);
      విడుదల(label);

      inputs_connect(net, argc, argv);
      విడుదల(argv);
}
