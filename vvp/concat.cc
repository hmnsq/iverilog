/*
 * Copyright (c) 2004-2021 Stephen Williams (steve@icarus.com)
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
# include  "concat.h"
# include  <cstdlib>
# include  <iostream>
# include  <cassert>

ఉపయోగించి నామాకాశ std;

vvp_fun_concat::vvp_fun_concat(గుర్తులేని w0, గుర్తులేని w1,
			       గుర్తులేని w2, గుర్తులేని w3)
: val_(w0+w1+w2+w3, BIT4_Z)
{
      wid_[0] = w0;
      wid_[1] = w1;
      wid_[2] = w2;
      wid_[3] = w3;
}

vvp_fun_concat::~vvp_fun_concat()
{
}

శూన్య vvp_fun_concat::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                               vvp_context_t context)
{
      recv_vec4_pv(పోర్టు, బిట్టు, 0, బిట్టు.size(), context);
}

శూన్య vvp_fun_concat::recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                  గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t)
{
      గుర్తులేని pdx = పోర్టు.పోర్టు();

      ఒకవేళ (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << పంక్తి_ముగింపు;
	    ప్రతిజ్ఞా(0);
      }

      గుర్తులేని off = పునాది;
      కొరకు (గుర్తులేని idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      ఒకవేళ (!val_.సమితి_vec(off, బిట్టు))
	    తిరిగి;

      ఒకవేళ (net_)
	    తిరిగి;

      net_ = పోర్టు.ptr();
      కాలపట్టిక_functor(ఇది);
}

శూన్య vvp_fun_concat::నిరంతరం_నిరంతరం()
{
      vvp_net_t *ptr = net_;
      net_ = శూన్యసూచిక;
      ptr->send_vec4(val_, 0);
}

శూన్య compile_concat(అక్షరం*label, గుర్తులేని w0, గుర్తులేని w1,
		    గుర్తులేని w2, గుర్తులేని w3,
		    గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      vvp_fun_concat*fun = కొత్త vvp_fun_concat(w0, w1, w2, w3);

      vvp_net_t*net = కొత్త vvp_net_t;
      net->fun = fun;

      define_functor_చిహ్నం(label, net);
      విడుదల(label);

      inputs_connect(net, argc, argv);
      విడుదల(argv);
}


vvp_fun_concat8::vvp_fun_concat8(గుర్తులేని w0, గుర్తులేని w1,
			       గుర్తులేని w2, గుర్తులేని w3)
: val_(w0+w1+w2+w3)
{
      wid_[0] = w0;
      wid_[1] = w1;
      wid_[2] = w2;
      wid_[3] = w3;
}

vvp_fun_concat8::~vvp_fun_concat8()
{
}

శూన్య vvp_fun_concat8::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
				vvp_context_t)
{
      vvp_vector8_t bit8 (బిట్టు, 6, 6);
      recv_vec8_pv(పోర్టు, bit8, 0, bit8.size());
}

శూన్య vvp_fun_concat8::recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
				   గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t)
{
      vvp_vector8_t bit8 (బిట్టు, 6, 6);
      recv_vec8_pv(పోర్టు, bit8, పునాది, vwid);
}

శూన్య vvp_fun_concat8::recv_vec8(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector8_t&బిట్టు)
{
      recv_vec8_pv(పోర్టు, బిట్టు, 0, బిట్టు.size());
}

శూన్య vvp_fun_concat8::recv_vec8_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector8_t&బిట్టు,
				   గుర్తులేని పునాది, గుర్తులేని vwid)
{
      గుర్తులేని pdx = పోర్టు.పోర్టు();

      ఒకవేళ (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << పంక్తి_ముగింపు;
	    ప్రతిజ్ఞా(0);
      }

      గుర్తులేని off = పునాది;
      కొరకు (గుర్తులేని idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      val_.సమితి_vec(off, బిట్టు);

      ఒకవేళ (net_)
	    తిరిగి;

      net_ = పోర్టు.ptr();
      కాలపట్టిక_functor(ఇది);
}

శూన్య vvp_fun_concat8::నిరంతరం_నిరంతరం()
{
      vvp_net_t *ptr = net_;
      net_ = శూన్యసూచిక;
      ptr->send_vec8(val_);
}

శూన్య compile_concat8(అక్షరం*label, గుర్తులేని w0, గుర్తులేని w1,
		     గుర్తులేని w2, గుర్తులేని w3,
		     గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      vvp_fun_concat8*fun = కొత్త vvp_fun_concat8(w0, w1, w2, w3);

      vvp_net_t*net = కొత్త vvp_net_t;
      net->fun = fun;

      define_functor_చిహ్నం(label, net);
      విడుదల(label);

      inputs_connect(net, argc, argv);
      విడుదల(argv);
}

vvp_fun_repeat::vvp_fun_repeat(గుర్తులేని width, గుర్తులేని repeat)
: wid_(width), rep_(repeat)
{
}

vvp_fun_repeat::~vvp_fun_repeat()
{
}

శూన్య vvp_fun_repeat::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                               vvp_context_t)
{
      ప్రతిజ్ఞా(బిట్టు.size() == wid_/rep_);

      vvp_vector4_t val (wid_);

      కొరకు (గుర్తులేని rdx = 0 ;  rdx < rep_ ;  rdx += 1) {
	    గుర్తులేని off = rdx * బిట్టు.size();

	    val.సమితి_vec(off, బిట్టు);
      }

      పోర్టు.ptr()->send_vec4(val, 0);
}

శూన్య vvp_fun_repeat::recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t &బిట్టు,
			          గుర్తులేని పునాది, గుర్తులేని vwid,
				  vvp_context_t context)
{
      recv_vec4_pv_(పోర్టు, బిట్టు, పునాది, vwid, context);
}

శూన్య compile_repeat(అక్షరం*label, దీర్ఘ width, దీర్ఘ repeat, నిర్మాణం symb_s arg)
{
      vvp_fun_repeat*fun = కొత్త vvp_fun_repeat(width, repeat);

      vvp_net_t*net = కొత్త vvp_net_t;
      net->fun = fun;

      define_functor_చిహ్నం(label, net);
      విడుదల(label);

      ఆదానం_connect(net, 0, arg.పాఠ్యం);
}
