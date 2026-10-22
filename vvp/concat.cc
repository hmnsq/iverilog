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

using নামাকাশ std;

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

শূন্য vvp_fun_concat::recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                               vvp_context_t context)
{
      recv_vec4_pv(port, bit, 0, bit.size(), context);
}

শূন্য vvp_fun_concat::recv_vec4_pv(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                                  unsigned base, unsigned vwid, vvp_context_t)
{
      unsigned pdx = port.port();

      যদি (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << endl;
	    প্রতিজ্ঞা(0);
      }

      unsigned off = base;
      বাবে (unsigned idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      যদি (!val_.set_vec(off, bit))
	    ঘূৰাই;

      যদি (net_)
	    ঘূৰাই;

      net_ = port.ptr();
      schedule_functor(এতত্);
}

শূন্য vvp_fun_concat::run_run()
{
      vvp_net_t *ptr = net_;
      net_ = nullptr;
      ptr->send_vec4(val_, 0);
}

শূন্য compile_concat(ৱর্ণ*label, unsigned w0, unsigned w1,
		    unsigned w2, unsigned w3,
		    unsigned argc, সংরচনা symb_s*argv)
{
      vvp_fun_concat*fun = নূতন vvp_fun_concat(w0, w1, w2, w3);

      vvp_net_t*net = নূতন vvp_net_t;
      net->fun = fun;

      define_functor_প্ৰতীক(label, net);
      মুক্ত(label);

      inputs_connect(net, argc, argv);
      মুক্ত(argv);
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

শূন্য vvp_fun_concat8::recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
				vvp_context_t)
{
      vvp_vector8_t bit8 (bit, 6, 6);
      recv_vec8_pv(port, bit8, 0, bit8.size());
}

শূন্য vvp_fun_concat8::recv_vec4_pv(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector8_t bit8 (bit, 6, 6);
      recv_vec8_pv(port, bit8, base, vwid);
}

শূন্য vvp_fun_concat8::recv_vec8(vvp_net_ptr_t port, ধ্রুৱ vvp_vector8_t&bit)
{
      recv_vec8_pv(port, bit, 0, bit.size());
}

শূন্য vvp_fun_concat8::recv_vec8_pv(vvp_net_ptr_t port, ধ্রুৱ vvp_vector8_t&bit,
				   unsigned base, unsigned vwid)
{
      unsigned pdx = port.port();

      যদি (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << endl;
	    প্রতিজ্ঞা(0);
      }

      unsigned off = base;
      বাবে (unsigned idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      val_.set_vec(off, bit);

      যদি (net_)
	    ঘূৰাই;

      net_ = port.ptr();
      schedule_functor(এতত্);
}

শূন্য vvp_fun_concat8::run_run()
{
      vvp_net_t *ptr = net_;
      net_ = nullptr;
      ptr->send_vec8(val_);
}

শূন্য compile_concat8(ৱর্ণ*label, unsigned w0, unsigned w1,
		     unsigned w2, unsigned w3,
		     unsigned argc, সংরচনা symb_s*argv)
{
      vvp_fun_concat8*fun = নূতন vvp_fun_concat8(w0, w1, w2, w3);

      vvp_net_t*net = নূতন vvp_net_t;
      net->fun = fun;

      define_functor_প্ৰতীক(label, net);
      মুক্ত(label);

      inputs_connect(net, argc, argv);
      মুক্ত(argv);
}

vvp_fun_repeat::vvp_fun_repeat(unsigned width, unsigned repeat)
: wid_(width), rep_(repeat)
{
}

vvp_fun_repeat::~vvp_fun_repeat()
{
}

শূন্য vvp_fun_repeat::recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                               vvp_context_t)
{
      প্রতিজ্ঞা(bit.size() == wid_/rep_);

      vvp_vector4_t val (wid_);

      বাবে (unsigned rdx = 0 ;  rdx < rep_ ;  rdx += 1) {
	    unsigned off = rdx * bit.size();

	    val.set_vec(off, bit);
      }

      port.ptr()->send_vec4(val, 0);
}

শূন্য vvp_fun_repeat::recv_vec4_pv(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t &bit,
			          unsigned base, unsigned vwid,
				  vvp_context_t context)
{
      recv_vec4_pv_(port, bit, base, vwid, context);
}

শূন্য compile_repeat(ৱর্ণ*label, long width, long repeat, সংরচনা symb_s arg)
{
      vvp_fun_repeat*fun = নূতন vvp_fun_repeat(width, repeat);

      vvp_net_t*net = নূতন vvp_net_t;
      net->fun = fun;

      define_functor_প্ৰতীক(label, net);
      মুক্ত(label);

      প্ৰবেশ_connect(net, 0, arg.text);
}
