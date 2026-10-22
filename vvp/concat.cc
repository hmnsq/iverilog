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

استعمال نامخلا std;

vvp_fun_concat::vvp_fun_concat(غیرعلامتی w0, غیرعلامتی w1,
			       غیرعلامتی w2, غیرعلامتی w3)
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

خالی vvp_fun_concat::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                               vvp_context_t context)
{
      recv_vec4_pv(پورٹ, ثنائی_ہندسہ, 0, ثنائی_ہندسہ.size(), context);
}

خالی vvp_fun_concat::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                  غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t)
{
      غیرعلامتی pdx = پورٹ.پورٹ();

      اگر (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << سطر_ختم;
	    أكد(0);
      }

      غیرعلامتی off = بنیاد;
      برائے (غیرعلامتی idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      اگر (!val_.سیٹ_vec(off, ثنائی_ہندسہ))
	    واپس;

      اگر (net_)
	    واپس;

      net_ = پورٹ.ptr();
      جدول_وقت_functor(یہ);
}

خالی vvp_fun_concat::تسلسل_تسلسل()
{
      vvp_net_t *ptr = net_;
      net_ = خالی_اشاریہ;
      ptr->send_vec4(val_, 0);
}

خالی compile_concat(حرف*label, غیرعلامتی w0, غیرعلامتی w1,
		    غیرعلامتی w2, غیرعلامتی w3,
		    غیرعلامتی argc, ساخت symb_s*argv)
{
      vvp_fun_concat*fun = نیا vvp_fun_concat(w0, w1, w2, w3);

      vvp_net_t*net = نیا vvp_net_t;
      net->fun = fun;

      define_functor_علامت(label, net);
      آزاد(label);

      inputs_connect(net, argc, argv);
      آزاد(argv);
}


vvp_fun_concat8::vvp_fun_concat8(غیرعلامتی w0, غیرعلامتی w1,
			       غیرعلامتی w2, غیرعلامتی w3)
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

خالی vvp_fun_concat8::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				vvp_context_t)
{
      vvp_vector8_t bit8 (ثنائی_ہندسہ, 6, 6);
      recv_vec8_pv(پورٹ, bit8, 0, bit8.size());
}

خالی vvp_fun_concat8::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				   غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t)
{
      vvp_vector8_t bit8 (ثنائی_ہندسہ, 6, 6);
      recv_vec8_pv(پورٹ, bit8, بنیاد, vwid);
}

خالی vvp_fun_concat8::recv_vec8(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      recv_vec8_pv(پورٹ, ثنائی_ہندسہ, 0, ثنائی_ہندسہ.size());
}

خالی vvp_fun_concat8::recv_vec8_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ,
				   غیرعلامتی بنیاد, غیرعلامتی vwid)
{
      غیرعلامتی pdx = پورٹ.پورٹ();

      اگر (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << سطر_ختم;
	    أكد(0);
      }

      غیرعلامتی off = بنیاد;
      برائے (غیرعلامتی idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      val_.سیٹ_vec(off, ثنائی_ہندسہ);

      اگر (net_)
	    واپس;

      net_ = پورٹ.ptr();
      جدول_وقت_functor(یہ);
}

خالی vvp_fun_concat8::تسلسل_تسلسل()
{
      vvp_net_t *ptr = net_;
      net_ = خالی_اشاریہ;
      ptr->send_vec8(val_);
}

خالی compile_concat8(حرف*label, غیرعلامتی w0, غیرعلامتی w1,
		     غیرعلامتی w2, غیرعلامتی w3,
		     غیرعلامتی argc, ساخت symb_s*argv)
{
      vvp_fun_concat8*fun = نیا vvp_fun_concat8(w0, w1, w2, w3);

      vvp_net_t*net = نیا vvp_net_t;
      net->fun = fun;

      define_functor_علامت(label, net);
      آزاد(label);

      inputs_connect(net, argc, argv);
      آزاد(argv);
}

vvp_fun_repeat::vvp_fun_repeat(غیرعلامتی width, غیرعلامتی repeat)
: wid_(width), rep_(repeat)
{
}

vvp_fun_repeat::~vvp_fun_repeat()
{
}

خالی vvp_fun_repeat::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                               vvp_context_t)
{
      أكد(ثنائی_ہندسہ.size() == wid_/rep_);

      vvp_vector4_t val (wid_);

      برائے (غیرعلامتی rdx = 0 ;  rdx < rep_ ;  rdx += 1) {
	    غیرعلامتی off = rdx * ثنائی_ہندسہ.size();

	    val.سیٹ_vec(off, ثنائی_ہندسہ);
      }

      پورٹ.ptr()->send_vec4(val, 0);
}

خالی vvp_fun_repeat::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t &ثنائی_ہندسہ,
			          غیرعلامتی بنیاد, غیرعلامتی vwid,
				  vvp_context_t context)
{
      recv_vec4_pv_(پورٹ, ثنائی_ہندسہ, بنیاد, vwid, context);
}

خالی compile_repeat(حرف*label, طویل width, طویل repeat, ساخت symb_s arg)
{
      vvp_fun_repeat*fun = نیا vvp_fun_repeat(width, repeat);

      vvp_net_t*net = نیا vvp_net_t;
      net->fun = fun;

      define_functor_علامت(label, net);
      آزاد(label);

      داخلہ_connect(net, 0, arg.متن);
}
