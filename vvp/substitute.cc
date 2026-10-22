/*
 * Copyright (c) 2016-2025 Stephen Williams (steve@icarus.com)
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
# include  "vvp_net.h"
# include  <cstdlib>
# include  <iostream>
# include  <cassert>


वर्ग vvp_fun_substitute : सार्वजनिक vvp_net_fun_t {

    सार्वजनिक:
      vvp_fun_substitute(अचिन्हित wid, अचिन्हित soff, अचिन्हित swid);
      ~vvp_fun_substitute() override;

      शून्य recv_vec4(vvp_net_ptr_t port, अचर vvp_vector4_t&bit, vvp_context_t) override;

      शून्य recv_vec4_pv(vvp_net_ptr_t ptr, अचर vvp_vector4_t&bit,
			अचिन्हित base, अचिन्हित vwid, vvp_context_t ctx) override;

    गुप्त:
      अचिन्हित wid_;
      अचिन्हित soff_;
      अचिन्हित swid_;

      vvp_vector4_t val_;
};

vvp_fun_substitute::vvp_fun_substitute(अचिन्हित wid, अचिन्हित soff, अचिन्हित swid)
: wid_(wid), soff_(soff), swid_(swid), val_(wid)
{
      लागि (अचिन्हित idx = 0 ; idx < val_.size() ; idx += 1)
	    val_.set_bit(idx, BIT4_Z);
}

vvp_fun_substitute::~vvp_fun_substitute()
{
}

शून्य vvp_fun_substitute::recv_vec4(vvp_net_ptr_t port, अचर vvp_vector4_t&bit,
				   vvp_context_t)
{
      अचिन्हित pdx = port.port();
      प्रतिज्ञा(pdx <= 1);

      यदि (pdx == 0) {
	    प्रतिज्ञा(bit.size() == wid_);

	    लागि (अचिन्हित idx = 0 ; idx < wid_ ; idx += 1) {
		  यदि (idx >= soff_ && idx < (soff_+swid_))
			जारी;

		  val_.set_bit(idx, bit.value(idx));
	    }

      } अन्यथा {
	    प्रतिज्ञा(bit.size() == swid_);

	    लागि (अचिन्हित idx = 0 ; idx < swid_ ; idx += 1)
		  val_.set_bit(idx+soff_, bit.value(idx));
      }

      port.ptr()->send_vec4(val_, 0);
}

शून्य vvp_fun_substitute::recv_vec4_pv(vvp_net_ptr_t ptr, अचर vvp_vector4_t&bit,
				      अचिन्हित base, अचिन्हित vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

शून्य compile_substitute(वर्ण*label, अचिन्हित width,
			अचिन्हित soff, अचिन्हित swidth,
			अचिन्हित argc, संरचना symb_s*argv)
{
      vvp_fun_substitute*fun = नूतन vvp_fun_substitute(width, soff, swidth);

      vvp_net_t*net = नूतन vvp_net_t;
      net->fun = fun;

      define_functor_चिह्न(label, net);
      मुक्त(label);

      inputs_connect(net, argc, argv);
      मुक्त(argv);
}
