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

استفاده نطاق std;

vvp_fun_concat::vvp_fun_concat(بیعلامت w0, بیعلامت w1,
			       بیعلامت w2, بیعلامت w3)
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

فراغ vvp_fun_concat::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                               vvp_context_t context)
{
      recv_vec4_pv(پورت, بیت, 0, بیت.size(), context);
}

فراغ vvp_fun_concat::recv_vec4_pv(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                                  بیعلامت پایه, بیعلامت vwid, vvp_context_t)
{
      بیعلامت pdx = پورت.پورت();

      إذا (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << پایان_سطر;
	    أكد(0);
      }

      بیعلامت off = پایه;
      لكل (بیعلامت idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      إذا (!val_.مجموعه_vec(off, بیت))
	    أرجع;

      إذا (net_)
	    أرجع;

      net_ = پورت.ptr();
      زمانبندی_functor(هذا);
}

فراغ vvp_fun_concat::پیاپی_پیاپی()
{
      vvp_net_t *ptr = net_;
      net_ = اشاره_تهی;
      ptr->send_vec4(val_, 0);
}

فراغ compile_concat(حرف*label, بیعلامت w0, بیعلامت w1,
		    بیعلامت w2, بیعلامت w3,
		    بیعلامت argc, بنية symb_s*argv)
{
      vvp_fun_concat*fun = جديد vvp_fun_concat(w0, w1, w2, w3);

      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_نماد(label, net);
      حرر(label);

      inputs_connect(net, argc, argv);
      حرر(argv);
}


vvp_fun_concat8::vvp_fun_concat8(بیعلامت w0, بیعلامت w1,
			       بیعلامت w2, بیعلامت w3)
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

فراغ vvp_fun_concat8::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
				vvp_context_t)
{
      vvp_vector8_t bit8 (بیت, 6, 6);
      recv_vec8_pv(پورت, bit8, 0, bit8.size());
}

فراغ vvp_fun_concat8::recv_vec4_pv(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
				   بیعلامت پایه, بیعلامت vwid, vvp_context_t)
{
      vvp_vector8_t bit8 (بیت, 6, 6);
      recv_vec8_pv(پورت, bit8, پایه, vwid);
}

فراغ vvp_fun_concat8::recv_vec8(vvp_net_ptr_t پورت, ثابت vvp_vector8_t&بیت)
{
      recv_vec8_pv(پورت, بیت, 0, بیت.size());
}

فراغ vvp_fun_concat8::recv_vec8_pv(vvp_net_ptr_t پورت, ثابت vvp_vector8_t&بیت,
				   بیعلامت پایه, بیعلامت vwid)
{
      بیعلامت pdx = پورت.پورت();

      إذا (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << پایان_سطر;
	    أكد(0);
      }

      بیعلامت off = پایه;
      لكل (بیعلامت idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      val_.مجموعه_vec(off, بیت);

      إذا (net_)
	    أرجع;

      net_ = پورت.ptr();
      زمانبندی_functor(هذا);
}

فراغ vvp_fun_concat8::پیاپی_پیاپی()
{
      vvp_net_t *ptr = net_;
      net_ = اشاره_تهی;
      ptr->send_vec8(val_);
}

فراغ compile_concat8(حرف*label, بیعلامت w0, بیعلامت w1,
		     بیعلامت w2, بیعلامت w3,
		     بیعلامت argc, بنية symb_s*argv)
{
      vvp_fun_concat8*fun = جديد vvp_fun_concat8(w0, w1, w2, w3);

      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_نماد(label, net);
      حرر(label);

      inputs_connect(net, argc, argv);
      حرر(argv);
}

vvp_fun_repeat::vvp_fun_repeat(بیعلامت width, بیعلامت repeat)
: wid_(width), rep_(repeat)
{
}

vvp_fun_repeat::~vvp_fun_repeat()
{
}

فراغ vvp_fun_repeat::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                               vvp_context_t)
{
      أكد(بیت.size() == wid_/rep_);

      vvp_vector4_t val (wid_);

      لكل (بیعلامت rdx = 0 ;  rdx < rep_ ;  rdx += 1) {
	    بیعلامت off = rdx * بیت.size();

	    val.مجموعه_vec(off, بیت);
      }

      پورت.ptr()->send_vec4(val, 0);
}

فراغ vvp_fun_repeat::recv_vec4_pv(vvp_net_ptr_t پورت, ثابت vvp_vector4_t &بیت,
			          بیعلامت پایه, بیعلامت vwid,
				  vvp_context_t context)
{
      recv_vec4_pv_(پورت, بیت, پایه, vwid, context);
}

فراغ compile_repeat(حرف*label, دراز width, دراز repeat, بنية symb_s arg)
{
      vvp_fun_repeat*fun = جديد vvp_fun_repeat(width, repeat);

      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_نماد(label, net);
      حرر(label);

      درون_داد_connect(net, 0, arg.متن);
}
