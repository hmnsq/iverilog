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

using نطاق std;

vvp_fun_concat::vvp_fun_concat(unsigned w0, unsigned w1,
			       unsigned w2, unsigned w3)
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

فراغ vvp_fun_concat::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                               vvp_context_t context)
{
      recv_vec4_pv(port, bit, 0, bit.size(), context);
}

فراغ vvp_fun_concat::recv_vec4_pv(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                  unsigned base, unsigned vwid, vvp_context_t)
{
      unsigned pdx = port.port();

      ܐܢ (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << endl;
	    أكد(0);
      }

      unsigned off = base;
      لكل (unsigned idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      ܐܢ (!val_.set_vec(off, bit))
	    ܦܢܐ;

      ܐܢ (net_)
	    ܦܢܐ;

      net_ = port.ptr();
      schedule_functor(هذا);
}

فراغ vvp_fun_concat::run_run()
{
      vvp_net_t *ptr = net_;
      net_ = nullptr;
      ptr->send_vec4(val_, 0);
}

فراغ compile_concat(حرف*label, unsigned w0, unsigned w1,
		    unsigned w2, unsigned w3,
		    unsigned argc, بنية symb_s*argv)
{
      vvp_fun_concat*fun = جديد vvp_fun_concat(w0, w1, w2, w3);

      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_symbol(label, net);
      حرر(label);

      inputs_connect(net, argc, argv);
      حرر(argv);
}


vvp_fun_concat8::vvp_fun_concat8(unsigned w0, unsigned w1,
			       unsigned w2, unsigned w3)
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

فراغ vvp_fun_concat8::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
				vvp_context_t)
{
      vvp_vector8_t bit8 (bit, 6, 6);
      recv_vec8_pv(port, bit8, 0, bit8.size());
}

فراغ vvp_fun_concat8::recv_vec4_pv(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector8_t bit8 (bit, 6, 6);
      recv_vec8_pv(port, bit8, base, vwid);
}

فراغ vvp_fun_concat8::recv_vec8(vvp_net_ptr_t port, ثابت vvp_vector8_t&bit)
{
      recv_vec8_pv(port, bit, 0, bit.size());
}

فراغ vvp_fun_concat8::recv_vec8_pv(vvp_net_ptr_t port, ثابت vvp_vector8_t&bit,
				   unsigned base, unsigned vwid)
{
      unsigned pdx = port.port();

      ܐܢ (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << endl;
	    أكد(0);
      }

      unsigned off = base;
      لكل (unsigned idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      val_.set_vec(off, bit);

      ܐܢ (net_)
	    ܦܢܐ;

      net_ = port.ptr();
      schedule_functor(هذا);
}

فراغ vvp_fun_concat8::run_run()
{
      vvp_net_t *ptr = net_;
      net_ = nullptr;
      ptr->send_vec8(val_);
}

فراغ compile_concat8(حرف*label, unsigned w0, unsigned w1,
		     unsigned w2, unsigned w3,
		     unsigned argc, بنية symb_s*argv)
{
      vvp_fun_concat8*fun = جديد vvp_fun_concat8(w0, w1, w2, w3);

      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_symbol(label, net);
      حرر(label);

      inputs_connect(net, argc, argv);
      حرر(argv);
}

vvp_fun_repeat::vvp_fun_repeat(unsigned width, unsigned repeat)
: wid_(width), rep_(repeat)
{
}

vvp_fun_repeat::~vvp_fun_repeat()
{
}

فراغ vvp_fun_repeat::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                               vvp_context_t)
{
      أكد(bit.size() == wid_/rep_);

      vvp_vector4_t val (wid_);

      لكل (unsigned rdx = 0 ;  rdx < rep_ ;  rdx += 1) {
	    unsigned off = rdx * bit.size();

	    val.set_vec(off, bit);
      }

      port.ptr()->send_vec4(val, 0);
}

فراغ vvp_fun_repeat::recv_vec4_pv(vvp_net_ptr_t port, ثابت vvp_vector4_t &bit,
			          unsigned base, unsigned vwid,
				  vvp_context_t context)
{
      recv_vec4_pv_(port, bit, base, vwid, context);
}

فراغ compile_repeat(حرف*label, long width, long repeat, بنية symb_s arg)
{
      vvp_fun_repeat*fun = جديد vvp_fun_repeat(width, repeat);

      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_symbol(label, net);
      حرر(label);

      input_connect(net, 0, arg.text);
}
