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

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

vvp_fun_concat::vvp_fun_concat(ਬਿਨਾਨਿਸ਼ਾਨੀ w0, ਬਿਨਾਨਿਸ਼ਾਨੀ w1,
			       ਬਿਨਾਨਿਸ਼ਾਨੀ w2, ਬਿਨਾਨਿਸ਼ਾਨੀ w3)
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

ਖਾਲੀ vvp_fun_concat::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                               vvp_context_t context)
{
      recv_vec4_pv(ਪੋਰਟ, ਦੋਆਧਾਰੀ_ਅੰਕ, 0, ਦੋਆਧਾਰੀ_ਅੰਕ.size(), context);
}

ਖਾਲੀ vvp_fun_concat::recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ pdx = ਪੋਰਟ.ਪੋਰਟ();

      ਜੇ (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << ਸਤਰ_ਅੰਤ;
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ off = ਅਧਾਰ;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      ਜੇ (!val_.ਸੈੱਟ_vec(off, ਦੋਆਧਾਰੀ_ਅੰਕ))
	    ਵਾਪਸ;

      ਜੇ (net_)
	    ਵਾਪਸ;

      net_ = ਪੋਰਟ.ptr();
      ਸਮਾਂ_ਸਾਰਣੀ_functor(ਇਹ);
}

ਖਾਲੀ vvp_fun_concat::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      vvp_net_t *ptr = net_;
      net_ = ਖਾਲੀ_ਸੂਚਕ;
      ptr->send_vec4(val_, 0);
}

ਖਾਲੀ compile_concat(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ w0, ਬਿਨਾਨਿਸ਼ਾਨੀ w1,
		    ਬਿਨਾਨਿਸ਼ਾਨੀ w2, ਬਿਨਾਨਿਸ਼ਾਨੀ w3,
		    ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      vvp_fun_concat*fun = ਨਵਾਂ vvp_fun_concat(w0, w1, w2, w3);

      vvp_net_t*net = ਨਵਾਂ vvp_net_t;
      net->fun = fun;

      define_functor_ਚਿੰਨ੍ਹ(label, net);
      ਮੁਕਤ(label);

      inputs_connect(net, argc, argv);
      ਮੁਕਤ(argv);
}


vvp_fun_concat8::vvp_fun_concat8(ਬਿਨਾਨਿਸ਼ਾਨੀ w0, ਬਿਨਾਨਿਸ਼ਾਨੀ w1,
			       ਬਿਨਾਨਿਸ਼ਾਨੀ w2, ਬਿਨਾਨਿਸ਼ਾਨੀ w3)
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

ਖਾਲੀ vvp_fun_concat8::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				vvp_context_t)
{
      vvp_vector8_t bit8 (ਦੋਆਧਾਰੀ_ਅੰਕ, 6, 6);
      recv_vec8_pv(ਪੋਰਟ, bit8, 0, bit8.size());
}

ਖਾਲੀ vvp_fun_concat8::recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				   ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t)
{
      vvp_vector8_t bit8 (ਦੋਆਧਾਰੀ_ਅੰਕ, 6, 6);
      recv_vec8_pv(ਪੋਰਟ, bit8, ਅਧਾਰ, vwid);
}

ਖਾਲੀ vvp_fun_concat8::recv_vec8(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      recv_vec8_pv(ਪੋਰਟ, ਦੋਆਧਾਰੀ_ਅੰਕ, 0, ਦੋਆਧਾਰੀ_ਅੰਕ.size());
}

ਖਾਲੀ vvp_fun_concat8::recv_vec8_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				   ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ pdx = ਪੋਰਟ.ਪੋਰਟ();

      ਜੇ (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << ਸਤਰ_ਅੰਤ;
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }

      ਬਿਨਾਨਿਸ਼ਾਨੀ off = ਅਧਾਰ;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      val_.ਸੈੱਟ_vec(off, ਦੋਆਧਾਰੀ_ਅੰਕ);

      ਜੇ (net_)
	    ਵਾਪਸ;

      net_ = ਪੋਰਟ.ptr();
      ਸਮਾਂ_ਸਾਰਣੀ_functor(ਇਹ);
}

ਖਾਲੀ vvp_fun_concat8::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      vvp_net_t *ptr = net_;
      net_ = ਖਾਲੀ_ਸੂਚਕ;
      ptr->send_vec8(val_);
}

ਖਾਲੀ compile_concat8(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ w0, ਬਿਨਾਨਿਸ਼ਾਨੀ w1,
		     ਬਿਨਾਨਿਸ਼ਾਨੀ w2, ਬਿਨਾਨਿਸ਼ਾਨੀ w3,
		     ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      vvp_fun_concat8*fun = ਨਵਾਂ vvp_fun_concat8(w0, w1, w2, w3);

      vvp_net_t*net = ਨਵਾਂ vvp_net_t;
      net->fun = fun;

      define_functor_ਚਿੰਨ੍ਹ(label, net);
      ਮੁਕਤ(label);

      inputs_connect(net, argc, argv);
      ਮੁਕਤ(argv);
}

vvp_fun_repeat::vvp_fun_repeat(ਬਿਨਾਨਿਸ਼ਾਨੀ width, ਬਿਨਾਨਿਸ਼ਾਨੀ repeat)
: wid_(width), rep_(repeat)
{
}

vvp_fun_repeat::~vvp_fun_repeat()
{
}

ਖਾਲੀ vvp_fun_repeat::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                               vvp_context_t)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਦੋਆਧਾਰੀ_ਅੰਕ.size() == wid_/rep_);

      vvp_vector4_t val (wid_);

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ rdx = 0 ;  rdx < rep_ ;  rdx += 1) {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ off = rdx * ਦੋਆਧਾਰੀ_ਅੰਕ.size();

	    val.ਸੈੱਟ_vec(off, ਦੋਆਧਾਰੀ_ਅੰਕ);
      }

      ਪੋਰਟ.ptr()->send_vec4(val, 0);
}

ਖਾਲੀ vvp_fun_repeat::recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t &ਦੋਆਧਾਰੀ_ਅੰਕ,
			          ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid,
				  vvp_context_t context)
{
      recv_vec4_pv_(ਪੋਰਟ, ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid, context);
}

ਖਾਲੀ compile_repeat(ਅੱਖਰ*label, ਲੰਮਾ width, ਲੰਮਾ repeat, ਬਣਤਰ symb_s arg)
{
      vvp_fun_repeat*fun = ਨਵਾਂ vvp_fun_repeat(width, repeat);

      vvp_net_t*net = ਨਵਾਂ vvp_net_t;
      net->fun = fun;

      define_functor_ਚਿੰਨ੍ਹ(label, net);
      ਮੁਕਤ(label);

      ਨਿਵੇਸ਼_connect(net, 0, arg.ਪਾਠ);
}
