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

ಬಳಸಿ ನಾಮಾಕಾಶ std;

vvp_fun_concat::vvp_fun_concat(ಚಿಹ್ನೆರಹಿತ w0, ಚಿಹ್ನೆರಹಿತ w1,
			       ಚಿಹ್ನೆರಹಿತ w2, ಚಿಹ್ನೆರಹಿತ w3)
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

ಶೂನ್ಯ vvp_fun_concat::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                               vvp_context_t context)
{
      recv_vec4_pv(ಪೋರ್ಟ್, ಬಿಟ್, 0, ಬಿಟ್.size(), context);
}

ಶೂನ್ಯ vvp_fun_concat::recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                  ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t)
{
      ಚಿಹ್ನೆರಹಿತ pdx = ಪೋರ್ಟ್.ಪೋರ್ಟ್();

      ಆದರೆ (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << ಸಾಲು_ಅಂತ್ಯ;
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      ಚಿಹ್ನೆರಹಿತ off = ತಳ;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      ಆದರೆ (!val_.ಗಣ_vec(off, ಬಿಟ್))
	    ಹಿಂತಿರುಗಿ;

      ಆದರೆ (net_)
	    ಹಿಂತಿರುಗಿ;

      net_ = ಪೋರ್ಟ್.ptr();
      ವೇಳಾಪಟ್ಟಿ_functor(ಇದು);
}

ಶೂನ್ಯ vvp_fun_concat::ಸತತ_ಸತತ()
{
      vvp_net_t *ptr = net_;
      net_ = ಶೂನ್ಯಸೂಚಕ;
      ptr->send_vec4(val_, 0);
}

ಶೂನ್ಯ compile_concat(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ w0, ಚಿಹ್ನೆರಹಿತ w1,
		    ಚಿಹ್ನೆರಹಿತ w2, ಚಿಹ್ನೆರಹಿತ w3,
		    ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      vvp_fun_concat*fun = ಹೊಸ vvp_fun_concat(w0, w1, w2, w3);

      vvp_net_t*net = ಹೊಸ vvp_net_t;
      net->fun = fun;

      define_functor_ಚಿಹ್ನೆ(label, net);
      ಬಿಡುಗಡೆ(label);

      inputs_connect(net, argc, argv);
      ಬಿಡುಗಡೆ(argv);
}


vvp_fun_concat8::vvp_fun_concat8(ಚಿಹ್ನೆರಹಿತ w0, ಚಿಹ್ನೆರಹಿತ w1,
			       ಚಿಹ್ನೆರಹಿತ w2, ಚಿಹ್ನೆರಹಿತ w3)
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

ಶೂನ್ಯ vvp_fun_concat8::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				vvp_context_t)
{
      vvp_vector8_t bit8 (ಬಿಟ್, 6, 6);
      recv_vec8_pv(ಪೋರ್ಟ್, bit8, 0, bit8.size());
}

ಶೂನ್ಯ vvp_fun_concat8::recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				   ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t)
{
      vvp_vector8_t bit8 (ಬಿಟ್, 6, 6);
      recv_vec8_pv(ಪೋರ್ಟ್, bit8, ತಳ, vwid);
}

ಶೂನ್ಯ vvp_fun_concat8::recv_vec8(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್)
{
      recv_vec8_pv(ಪೋರ್ಟ್, ಬಿಟ್, 0, ಬಿಟ್.size());
}

ಶೂನ್ಯ vvp_fun_concat8::recv_vec8_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್,
				   ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid)
{
      ಚಿಹ್ನೆರಹಿತ pdx = ಪೋರ್ಟ್.ಪೋರ್ಟ್();

      ಆದರೆ (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << ಸಾಲು_ಅಂತ್ಯ;
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      ಚಿಹ್ನೆರಹಿತ off = ತಳ;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      val_.ಗಣ_vec(off, ಬಿಟ್);

      ಆದರೆ (net_)
	    ಹಿಂತಿರುಗಿ;

      net_ = ಪೋರ್ಟ್.ptr();
      ವೇಳಾಪಟ್ಟಿ_functor(ಇದು);
}

ಶೂನ್ಯ vvp_fun_concat8::ಸತತ_ಸತತ()
{
      vvp_net_t *ptr = net_;
      net_ = ಶೂನ್ಯಸೂಚಕ;
      ptr->send_vec8(val_);
}

ಶೂನ್ಯ compile_concat8(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ w0, ಚಿಹ್ನೆರಹಿತ w1,
		     ಚಿಹ್ನೆರಹಿತ w2, ಚಿಹ್ನೆರಹಿತ w3,
		     ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      vvp_fun_concat8*fun = ಹೊಸ vvp_fun_concat8(w0, w1, w2, w3);

      vvp_net_t*net = ಹೊಸ vvp_net_t;
      net->fun = fun;

      define_functor_ಚಿಹ್ನೆ(label, net);
      ಬಿಡುಗಡೆ(label);

      inputs_connect(net, argc, argv);
      ಬಿಡುಗಡೆ(argv);
}

vvp_fun_repeat::vvp_fun_repeat(ಚಿಹ್ನೆರಹಿತ width, ಚಿಹ್ನೆರಹಿತ repeat)
: wid_(width), rep_(repeat)
{
}

vvp_fun_repeat::~vvp_fun_repeat()
{
}

ಶೂನ್ಯ vvp_fun_repeat::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                               vvp_context_t)
{
      ಪ್ರತಿಜ್ಞಾ(ಬಿಟ್.size() == wid_/rep_);

      vvp_vector4_t val (wid_);

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ rdx = 0 ;  rdx < rep_ ;  rdx += 1) {
	    ಚಿಹ್ನೆರಹಿತ off = rdx * ಬಿಟ್.size();

	    val.ಗಣ_vec(off, ಬಿಟ್);
      }

      ಪೋರ್ಟ್.ptr()->send_vec4(val, 0);
}

ಶೂನ್ಯ vvp_fun_repeat::recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t &ಬಿಟ್,
			          ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid,
				  vvp_context_t context)
{
      recv_vec4_pv_(ಪೋರ್ಟ್, ಬಿಟ್, ತಳ, vwid, context);
}

ಶೂನ್ಯ compile_repeat(ಅಕ್ಷರ*label, ದೀರ್ಘ width, ದೀರ್ಘ repeat, ರಚನೆ symb_s arg)
{
      vvp_fun_repeat*fun = ಹೊಸ vvp_fun_repeat(width, repeat);

      vvp_net_t*net = ಹೊಸ vvp_net_t;
      net->fun = fun;

      define_functor_ಚಿಹ್ನೆ(label, net);
      ಬಿಡುಗಡೆ(label);

      ಒಳಹರಿವು_connect(net, 0, arg.ಪಠ್ಯ);
}
