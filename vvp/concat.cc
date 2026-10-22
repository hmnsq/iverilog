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

استعمال نطاق std;

vvp_fun_concat::vvp_fun_concat(بنانشان w0, بنانشان w1,
			       بنانشان w2, بنانشان w3)
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

فراغ vvp_fun_concat::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                               vvp_context_t context)
{
      recv_vec4_pv(پورٽ, ٻٽو_انگ, 0, ٻٽو_انگ.size(), context);
}

فراغ vvp_fun_concat::recv_vec4_pv(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                                  بنانشان بنياد, بنانشان vwid, vvp_context_t)
{
      بنانشان pdx = پورٽ.پورٽ();

      إذا (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << سٽ_پڄاڻي;
	    أكد(0);
      }

      بنانشان off = بنياد;
      لكل (بنانشان idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      إذا (!val_.سيٽ_vec(off, ٻٽو_انگ))
	    أرجع;

      إذا (net_)
	    أرجع;

      net_ = پورٽ.ptr();
      وقت_جدول_functor(هذا);
}

فراغ vvp_fun_concat::لاڳيتو_لاڳيتو()
{
      vvp_net_t *ptr = net_;
      net_ = خالي_اشارو;
      ptr->send_vec4(val_, 0);
}

فراغ compile_concat(حرف*label, بنانشان w0, بنانشان w1,
		    بنانشان w2, بنانشان w3,
		    بنانشان argc, بنية symb_s*argv)
{
      vvp_fun_concat*fun = جديد vvp_fun_concat(w0, w1, w2, w3);

      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_نشان(label, net);
      حرر(label);

      inputs_connect(net, argc, argv);
      حرر(argv);
}


vvp_fun_concat8::vvp_fun_concat8(بنانشان w0, بنانشان w1,
			       بنانشان w2, بنانشان w3)
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

فراغ vvp_fun_concat8::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
				vvp_context_t)
{
      vvp_vector8_t bit8 (ٻٽو_انگ, 6, 6);
      recv_vec8_pv(پورٽ, bit8, 0, bit8.size());
}

فراغ vvp_fun_concat8::recv_vec4_pv(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
				   بنانشان بنياد, بنانشان vwid, vvp_context_t)
{
      vvp_vector8_t bit8 (ٻٽو_انگ, 6, 6);
      recv_vec8_pv(پورٽ, bit8, بنياد, vwid);
}

فراغ vvp_fun_concat8::recv_vec8(vvp_net_ptr_t پورٽ, ثابت vvp_vector8_t&ٻٽو_انگ)
{
      recv_vec8_pv(پورٽ, ٻٽو_انگ, 0, ٻٽو_انگ.size());
}

فراغ vvp_fun_concat8::recv_vec8_pv(vvp_net_ptr_t پورٽ, ثابت vvp_vector8_t&ٻٽو_انگ,
				   بنانشان بنياد, بنانشان vwid)
{
      بنانشان pdx = پورٽ.پورٽ();

      إذا (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << سٽ_پڄاڻي;
	    أكد(0);
      }

      بنانشان off = بنياد;
      لكل (بنانشان idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      val_.سيٽ_vec(off, ٻٽو_انگ);

      إذا (net_)
	    أرجع;

      net_ = پورٽ.ptr();
      وقت_جدول_functor(هذا);
}

فراغ vvp_fun_concat8::لاڳيتو_لاڳيتو()
{
      vvp_net_t *ptr = net_;
      net_ = خالي_اشارو;
      ptr->send_vec8(val_);
}

فراغ compile_concat8(حرف*label, بنانشان w0, بنانشان w1,
		     بنانشان w2, بنانشان w3,
		     بنانشان argc, بنية symb_s*argv)
{
      vvp_fun_concat8*fun = جديد vvp_fun_concat8(w0, w1, w2, w3);

      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_نشان(label, net);
      حرر(label);

      inputs_connect(net, argc, argv);
      حرر(argv);
}

vvp_fun_repeat::vvp_fun_repeat(بنانشان width, بنانشان repeat)
: wid_(width), rep_(repeat)
{
}

vvp_fun_repeat::~vvp_fun_repeat()
{
}

فراغ vvp_fun_repeat::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                               vvp_context_t)
{
      أكد(ٻٽو_انگ.size() == wid_/rep_);

      vvp_vector4_t val (wid_);

      لكل (بنانشان rdx = 0 ;  rdx < rep_ ;  rdx += 1) {
	    بنانشان off = rdx * ٻٽو_انگ.size();

	    val.سيٽ_vec(off, ٻٽو_انگ);
      }

      پورٽ.ptr()->send_vec4(val, 0);
}

فراغ vvp_fun_repeat::recv_vec4_pv(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t &ٻٽو_انگ,
			          بنانشان بنياد, بنانشان vwid,
				  vvp_context_t context)
{
      recv_vec4_pv_(پورٽ, ٻٽو_انگ, بنياد, vwid, context);
}

فراغ compile_repeat(حرف*label, ڊگهو width, ڊگهو repeat, بنية symb_s arg)
{
      vvp_fun_repeat*fun = جديد vvp_fun_repeat(width, repeat);

      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_نشان(label, net);
      حرر(label);

      داخلا_connect(net, 0, arg.متن);
}
