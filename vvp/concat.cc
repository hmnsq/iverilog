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

using नामाकाश std;

vvp_fun_concat::vvp_fun_concat(अचिन्हित w0, अचिन्हित w1,
			       अचिन्हित w2, अचिन्हित w3)
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

शून्य vvp_fun_concat::recv_vec4(vvp_net_ptr_t port, स्थिरांक vvp_vector4_t&bit,
                               vvp_context_t context)
{
      recv_vec4_pv(port, bit, 0, bit.size(), context);
}

शून्य vvp_fun_concat::recv_vec4_pv(vvp_net_ptr_t port, स्थिरांक vvp_vector4_t&bit,
                                  अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      अचिन्हित pdx = port.port();

      जर (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << endl;
	    प्रतिज्ञा(0);
      }

      अचिन्हित off = base;
      साठी (अचिन्हित idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      जर (!val_.set_vec(off, bit))
	    परत;

      जर (net_)
	    परत;

      net_ = port.ptr();
      schedule_functor(हे);
}

शून्य vvp_fun_concat::run_run()
{
      vvp_net_t *ptr = net_;
      net_ = nullptr;
      ptr->send_vec4(val_, 0);
}

शून्य compile_concat(अक्षर*label, अचिन्हित w0, अचिन्हित w1,
		    अचिन्हित w2, अचिन्हित w3,
		    अचिन्हित argc, संरचना symb_s*argv)
{
      vvp_fun_concat*fun = नवीन vvp_fun_concat(w0, w1, w2, w3);

      vvp_net_t*net = नवीन vvp_net_t;
      net->fun = fun;

      define_functor_चिन्ह(label, net);
      मुक्त(label);

      inputs_connect(net, argc, argv);
      मुक्त(argv);
}


vvp_fun_concat8::vvp_fun_concat8(अचिन्हित w0, अचिन्हित w1,
			       अचिन्हित w2, अचिन्हित w3)
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

शून्य vvp_fun_concat8::recv_vec4(vvp_net_ptr_t port, स्थिरांक vvp_vector4_t&bit,
				vvp_context_t)
{
      vvp_vector8_t bit8 (bit, 6, 6);
      recv_vec8_pv(port, bit8, 0, bit8.size());
}

शून्य vvp_fun_concat8::recv_vec4_pv(vvp_net_ptr_t port, स्थिरांक vvp_vector4_t&bit,
				   अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      vvp_vector8_t bit8 (bit, 6, 6);
      recv_vec8_pv(port, bit8, base, vwid);
}

शून्य vvp_fun_concat8::recv_vec8(vvp_net_ptr_t port, स्थिरांक vvp_vector8_t&bit)
{
      recv_vec8_pv(port, bit, 0, bit.size());
}

शून्य vvp_fun_concat8::recv_vec8_pv(vvp_net_ptr_t port, स्थिरांक vvp_vector8_t&bit,
				   अचिन्हित base, अचिन्हित vwid)
{
      अचिन्हित pdx = port.port();

      जर (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << endl;
	    प्रतिज्ञा(0);
      }

      अचिन्हित off = base;
      साठी (अचिन्हित idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      val_.set_vec(off, bit);

      जर (net_)
	    परत;

      net_ = port.ptr();
      schedule_functor(हे);
}

शून्य vvp_fun_concat8::run_run()
{
      vvp_net_t *ptr = net_;
      net_ = nullptr;
      ptr->send_vec8(val_);
}

शून्य compile_concat8(अक्षर*label, अचिन्हित w0, अचिन्हित w1,
		     अचिन्हित w2, अचिन्हित w3,
		     अचिन्हित argc, संरचना symb_s*argv)
{
      vvp_fun_concat8*fun = नवीन vvp_fun_concat8(w0, w1, w2, w3);

      vvp_net_t*net = नवीन vvp_net_t;
      net->fun = fun;

      define_functor_चिन्ह(label, net);
      मुक्त(label);

      inputs_connect(net, argc, argv);
      मुक्त(argv);
}

vvp_fun_repeat::vvp_fun_repeat(अचिन्हित width, अचिन्हित repeat)
: wid_(width), rep_(repeat)
{
}

vvp_fun_repeat::~vvp_fun_repeat()
{
}

शून्य vvp_fun_repeat::recv_vec4(vvp_net_ptr_t port, स्थिरांक vvp_vector4_t&bit,
                               vvp_context_t)
{
      प्रतिज्ञा(bit.size() == wid_/rep_);

      vvp_vector4_t val (wid_);

      साठी (अचिन्हित rdx = 0 ;  rdx < rep_ ;  rdx += 1) {
	    अचिन्हित off = rdx * bit.size();

	    val.set_vec(off, bit);
      }

      port.ptr()->send_vec4(val, 0);
}

शून्य vvp_fun_repeat::recv_vec4_pv(vvp_net_ptr_t port, स्थिरांक vvp_vector4_t &bit,
			          अचिन्हित base, अचिन्हित vwid,
				  vvp_context_t context)
{
      recv_vec4_pv_(port, bit, base, vwid, context);
}

शून्य compile_repeat(अक्षर*label, दीर्घ width, दीर्घ repeat, संरचना symb_s arg)
{
      vvp_fun_repeat*fun = नवीन vvp_fun_repeat(width, repeat);

      vvp_net_t*net = नवीन vvp_net_t;
      net->fun = fun;

      define_functor_चिन्ह(label, net);
      मुक्त(label);

      आदान_connect(net, 0, arg.text);
}
