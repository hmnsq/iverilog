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

ورتوں نطاق std;

vvp_fun_concat::vvp_fun_concat(بنانشانی w0, بنانشانی w1,
			       بنانشانی w2, بنانشانی w3)
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

فراغ vvp_fun_concat::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                               vvp_context_t context)
{
      recv_vec4_pv(پورٹ, ثنائی_ہندسہ, 0, ثنائی_ہندسہ.size(), context);
}

فراغ vvp_fun_concat::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                  بنانشانی ادھار, بنانشانی vwid, vvp_context_t)
{
      بنانشانی pdx = پورٹ.پورٹ();

      إذا (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << سطر_انت;
	    أكد(0);
      }

      بنانشانی off = ادھار;
      لكل (بنانشانی idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      إذا (!val_.سیٹ_vec(off, ثنائی_ہندسہ))
	    أرجع;

      إذا (net_)
	    أرجع;

      net_ = پورٹ.ptr();
      ویلا_سارنی_functor(هذا);
}

فراغ vvp_fun_concat::لگاتار_لگاتار()
{
      vvp_net_t *ptr = net_;
      net_ = خالی_سوچک;
      ptr->send_vec4(val_, 0);
}

فراغ compile_concat(حرف*label, بنانشانی w0, بنانشانی w1,
		    بنانشانی w2, بنانشانی w3,
		    بنانشانی argc, بنية symb_s*argv)
{
      vvp_fun_concat*fun = جديد vvp_fun_concat(w0, w1, w2, w3);

      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_علامت(label, net);
      حرر(label);

      inputs_connect(net, argc, argv);
      حرر(argv);
}


vvp_fun_concat8::vvp_fun_concat8(بنانشانی w0, بنانشانی w1,
			       بنانشانی w2, بنانشانی w3)
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

فراغ vvp_fun_concat8::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				vvp_context_t)
{
      vvp_vector8_t bit8 (ثنائی_ہندسہ, 6, 6);
      recv_vec8_pv(پورٹ, bit8, 0, bit8.size());
}

فراغ vvp_fun_concat8::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				   بنانشانی ادھار, بنانشانی vwid, vvp_context_t)
{
      vvp_vector8_t bit8 (ثنائی_ہندسہ, 6, 6);
      recv_vec8_pv(پورٹ, bit8, ادھار, vwid);
}

فراغ vvp_fun_concat8::recv_vec8(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      recv_vec8_pv(پورٹ, ثنائی_ہندسہ, 0, ثنائی_ہندسہ.size());
}

فراغ vvp_fun_concat8::recv_vec8_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ,
				   بنانشانی ادھار, بنانشانی vwid)
{
      بنانشانی pdx = پورٹ.پورٹ();

      إذا (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << سطر_انت;
	    أكد(0);
      }

      بنانشانی off = ادھار;
      لكل (بنانشانی idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      val_.سیٹ_vec(off, ثنائی_ہندسہ);

      إذا (net_)
	    أرجع;

      net_ = پورٹ.ptr();
      ویلا_سارنی_functor(هذا);
}

فراغ vvp_fun_concat8::لگاتار_لگاتار()
{
      vvp_net_t *ptr = net_;
      net_ = خالی_سوچک;
      ptr->send_vec8(val_);
}

فراغ compile_concat8(حرف*label, بنانشانی w0, بنانشانی w1,
		     بنانشانی w2, بنانشانی w3,
		     بنانشانی argc, بنية symb_s*argv)
{
      vvp_fun_concat8*fun = جديد vvp_fun_concat8(w0, w1, w2, w3);

      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_علامت(label, net);
      حرر(label);

      inputs_connect(net, argc, argv);
      حرر(argv);
}

vvp_fun_repeat::vvp_fun_repeat(بنانشانی width, بنانشانی repeat)
: wid_(width), rep_(repeat)
{
}

vvp_fun_repeat::~vvp_fun_repeat()
{
}

فراغ vvp_fun_repeat::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                               vvp_context_t)
{
      أكد(ثنائی_ہندسہ.size() == wid_/rep_);

      vvp_vector4_t val (wid_);

      لكل (بنانشانی rdx = 0 ;  rdx < rep_ ;  rdx += 1) {
	    بنانشانی off = rdx * ثنائی_ہندسہ.size();

	    val.سیٹ_vec(off, ثنائی_ہندسہ);
      }

      پورٹ.ptr()->send_vec4(val, 0);
}

فراغ vvp_fun_repeat::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t &ثنائی_ہندسہ,
			          بنانشانی ادھار, بنانشانی vwid,
				  vvp_context_t context)
{
      recv_vec4_pv_(پورٹ, ثنائی_ہندسہ, ادھار, vwid, context);
}

فراغ compile_repeat(حرف*label, لما width, لما repeat, بنية symb_s arg)
{
      vvp_fun_repeat*fun = جديد vvp_fun_repeat(width, repeat);

      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_علامت(label, net);
      حرر(label);

      داخلہ_connect(net, 0, arg.متن);
}
