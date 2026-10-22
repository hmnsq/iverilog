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

کارول نطاق std;

vvp_fun_concat::vvp_fun_concat(بینښه w0, بینښه w1,
			       بینښه w2, بینښه w3)
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

فراغ vvp_fun_concat::recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                               vvp_context_t context)
{
      recv_vec4_pv(پورټ, بټ, 0, بټ.size(), context);
}

فراغ vvp_fun_concat::recv_vec4_pv(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                                  بینښه بنسټ, بینښه vwid, vvp_context_t)
{
      بینښه pdx = پورټ.پورټ();

      إذا (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << کرښه_پای;
	    أكد(0);
      }

      بینښه off = بنسټ;
      لكل (بینښه idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      إذا (!val_.سېټ_vec(off, بټ))
	    أرجع;

      إذا (net_)
	    أرجع;

      net_ = پورټ.ptr();
      مهالوېش_functor(هذا);
}

فراغ vvp_fun_concat::پرلپسې_پرلپسې()
{
      vvp_net_t *ptr = net_;
      net_ = تش_اشاره;
      ptr->send_vec4(val_, 0);
}

فراغ compile_concat(حرف*label, بینښه w0, بینښه w1,
		    بینښه w2, بینښه w3,
		    بینښه argc, بنية symb_s*argv)
{
      vvp_fun_concat*fun = جديد vvp_fun_concat(w0, w1, w2, w3);

      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_نښه(label, net);
      حرر(label);

      inputs_connect(net, argc, argv);
      حرر(argv);
}


vvp_fun_concat8::vvp_fun_concat8(بینښه w0, بینښه w1,
			       بینښه w2, بینښه w3)
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

فراغ vvp_fun_concat8::recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
				vvp_context_t)
{
      vvp_vector8_t bit8 (بټ, 6, 6);
      recv_vec8_pv(پورټ, bit8, 0, bit8.size());
}

فراغ vvp_fun_concat8::recv_vec4_pv(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
				   بینښه بنسټ, بینښه vwid, vvp_context_t)
{
      vvp_vector8_t bit8 (بټ, 6, 6);
      recv_vec8_pv(پورټ, bit8, بنسټ, vwid);
}

فراغ vvp_fun_concat8::recv_vec8(vvp_net_ptr_t پورټ, ثابت vvp_vector8_t&بټ)
{
      recv_vec8_pv(پورټ, بټ, 0, بټ.size());
}

فراغ vvp_fun_concat8::recv_vec8_pv(vvp_net_ptr_t پورټ, ثابت vvp_vector8_t&بټ,
				   بینښه بنسټ, بینښه vwid)
{
      بینښه pdx = پورټ.پورټ();

      إذا (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << کرښه_پای;
	    أكد(0);
      }

      بینښه off = بنسټ;
      لكل (بینښه idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      val_.سېټ_vec(off, بټ);

      إذا (net_)
	    أرجع;

      net_ = پورټ.ptr();
      مهالوېش_functor(هذا);
}

فراغ vvp_fun_concat8::پرلپسې_پرلپسې()
{
      vvp_net_t *ptr = net_;
      net_ = تش_اشاره;
      ptr->send_vec8(val_);
}

فراغ compile_concat8(حرف*label, بینښه w0, بینښه w1,
		     بینښه w2, بینښه w3,
		     بینښه argc, بنية symb_s*argv)
{
      vvp_fun_concat8*fun = جديد vvp_fun_concat8(w0, w1, w2, w3);

      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_نښه(label, net);
      حرر(label);

      inputs_connect(net, argc, argv);
      حرر(argv);
}

vvp_fun_repeat::vvp_fun_repeat(بینښه width, بینښه repeat)
: wid_(width), rep_(repeat)
{
}

vvp_fun_repeat::~vvp_fun_repeat()
{
}

فراغ vvp_fun_repeat::recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                               vvp_context_t)
{
      أكد(بټ.size() == wid_/rep_);

      vvp_vector4_t val (wid_);

      لكل (بینښه rdx = 0 ;  rdx < rep_ ;  rdx += 1) {
	    بینښه off = rdx * بټ.size();

	    val.سېټ_vec(off, بټ);
      }

      پورټ.ptr()->send_vec4(val, 0);
}

فراغ vvp_fun_repeat::recv_vec4_pv(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t &بټ,
			          بینښه بنسټ, بینښه vwid,
				  vvp_context_t context)
{
      recv_vec4_pv_(پورټ, بټ, بنسټ, vwid, context);
}

فراغ compile_repeat(حرف*label, اوږد width, اوږد repeat, بنية symb_s arg)
{
      vvp_fun_repeat*fun = جديد vvp_fun_repeat(width, repeat);

      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_نښه(label, net);
      حرر(label);

      ننوت_connect(net, 0, arg.متن);
}
