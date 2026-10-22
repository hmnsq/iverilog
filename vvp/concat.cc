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

उपयुज्य नामाकाश std;

vvp_fun_concat::vvp_fun_concat(अचिह्नित w0, अचिह्नित w1,
			       अचिह्नित w2, अचिह्नित w3)
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

शून्य vvp_fun_concat::recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                               vvp_context_t context)
{
      recv_vec4_pv(पत्तनम्, bit, 0, bit.size(), context);
}

शून्य vvp_fun_concat::recv_vec4_pv(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                                  अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t)
{
      अचिह्नित pdx = पत्तनम्.पत्तनम्();

      यदि (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << पङ्क्त्यन्तः;
	    प्रतिज्ञा(0);
      }

      अचिह्नित off = आधारः;
      प्रति (अचिह्नित idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      यदि (!val_.समुच्चयः_vec(off, bit))
	    प्रत्यागम;

      यदि (net_)
	    प्रत्यागम;

      net_ = पत्तनम्.ptr();
      कालसारणी_functor(एतत्);
}

शून्य vvp_fun_concat::सततम्_सततम्()
{
      vvp_net_t *ptr = net_;
      net_ = शून्यसूचकः;
      ptr->send_vec4(val_, 0);
}

शून्य compile_concat(वर्ण*label, अचिह्नित w0, अचिह्नित w1,
		    अचिह्नित w2, अचिह्नित w3,
		    अचिह्नित argc, संरचना symb_s*argv)
{
      vvp_fun_concat*fun = नूतन vvp_fun_concat(w0, w1, w2, w3);

      vvp_net_t*net = नूतन vvp_net_t;
      net->fun = fun;

      define_functor_चिह्नम्(label, net);
      मुक्त(label);

      inputs_connect(net, argc, argv);
      मुक्त(argv);
}


vvp_fun_concat8::vvp_fun_concat8(अचिह्नित w0, अचिह्नित w1,
			       अचिह्नित w2, अचिह्नित w3)
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

शून्य vvp_fun_concat8::recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
				vvp_context_t)
{
      vvp_vector8_t bit8 (bit, 6, 6);
      recv_vec8_pv(पत्तनम्, bit8, 0, bit8.size());
}

शून्य vvp_fun_concat8::recv_vec4_pv(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
				   अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t)
{
      vvp_vector8_t bit8 (bit, 6, 6);
      recv_vec8_pv(पत्तनम्, bit8, आधारः, vwid);
}

शून्य vvp_fun_concat8::recv_vec8(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector8_t&bit)
{
      recv_vec8_pv(पत्तनम्, bit, 0, bit.size());
}

शून्य vvp_fun_concat8::recv_vec8_pv(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector8_t&bit,
				   अचिह्नित आधारः, अचिह्नित vwid)
{
      अचिह्नित pdx = पत्तनम्.पत्तनम्();

      यदि (vwid != wid_[pdx]) {
	    cerr << "internal error: port " << pdx
		 << " expects wid=" << wid_[pdx]
		 << ", got wid=" << vwid << पङ्क्त्यन्तः;
	    प्रतिज्ञा(0);
      }

      अचिह्नित off = आधारः;
      प्रति (अचिह्नित idx = 0 ;  idx < pdx ;  idx += 1)
	    off += wid_[idx];

      val_.समुच्चयः_vec(off, bit);

      यदि (net_)
	    प्रत्यागम;

      net_ = पत्तनम्.ptr();
      कालसारणी_functor(एतत्);
}

शून्य vvp_fun_concat8::सततम्_सततम्()
{
      vvp_net_t *ptr = net_;
      net_ = शून्यसूचकः;
      ptr->send_vec8(val_);
}

शून्य compile_concat8(वर्ण*label, अचिह्नित w0, अचिह्नित w1,
		     अचिह्नित w2, अचिह्नित w3,
		     अचिह्नित argc, संरचना symb_s*argv)
{
      vvp_fun_concat8*fun = नूतन vvp_fun_concat8(w0, w1, w2, w3);

      vvp_net_t*net = नूतन vvp_net_t;
      net->fun = fun;

      define_functor_चिह्नम्(label, net);
      मुक्त(label);

      inputs_connect(net, argc, argv);
      मुक्त(argv);
}

vvp_fun_repeat::vvp_fun_repeat(अचिह्नित width, अचिह्नित repeat)
: wid_(width), rep_(repeat)
{
}

vvp_fun_repeat::~vvp_fun_repeat()
{
}

शून्य vvp_fun_repeat::recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                               vvp_context_t)
{
      प्रतिज्ञा(bit.size() == wid_/rep_);

      vvp_vector4_t val (wid_);

      प्रति (अचिह्नित rdx = 0 ;  rdx < rep_ ;  rdx += 1) {
	    अचिह्नित off = rdx * bit.size();

	    val.समुच्चयः_vec(off, bit);
      }

      पत्तनम्.ptr()->send_vec4(val, 0);
}

शून्य vvp_fun_repeat::recv_vec4_pv(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t &bit,
			          अचिह्नित आधारः, अचिह्नित vwid,
				  vvp_context_t context)
{
      recv_vec4_pv_(पत्तनम्, bit, आधारः, vwid, context);
}

शून्य compile_repeat(वर्ण*label, दीर्घ width, दीर्घ repeat, संरचना symb_s arg)
{
      vvp_fun_repeat*fun = नूतन vvp_fun_repeat(width, repeat);

      vvp_net_t*net = नूतन vvp_net_t;
      net->fun = fun;

      define_functor_चिह्नम्(label, net);
      मुक्त(label);

      निवेशः_connect(net, 0, arg.text);
}
