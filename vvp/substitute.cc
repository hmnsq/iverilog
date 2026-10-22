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


श्रेणी vvp_fun_substitute : खुला vvp_net_fun_t {

    खुला:
      vvp_fun_substitute(अचिन्हित wid, अचिन्हित soff, अचिन्हित swid);
      ~vvp_fun_substitute() override;

      व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit, vvp_context_t) override;

      व्योम recv_vec4_pv(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
			अचिन्हित base, अचिन्हित vwid, vvp_context_t ctx) override;

    निजी:
      अचिन्हित wid_;
      अचिन्हित soff_;
      अचिन्हित swid_;

      vvp_vector4_t val_;
};

vvp_fun_substitute::vvp_fun_substitute(अचिन्हित wid, अचिन्हित soff, अचिन्हित swid)
: wid_(wid), soff_(soff), swid_(swid), val_(wid)
{
      क्रम (अचिन्हित idx = 0 ; idx < val_.size() ; idx += 1)
	    val_.set_bit(idx, BIT4_Z);
}

vvp_fun_substitute::~vvp_fun_substitute()
{
}

व्योम vvp_fun_substitute::recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
				   vvp_context_t)
{
      अचिन्हित pdx = port.port();
      निश्चित(pdx <= 1);

      अगर (pdx == 0) {
	    निश्चित(bit.size() == wid_);

	    क्रम (अचिन्हित idx = 0 ; idx < wid_ ; idx += 1) {
		  अगर (idx >= soff_ && idx < (soff_+swid_))
			जारी;

		  val_.set_bit(idx, bit.value(idx));
	    }

      } अन्यथा {
	    निश्चित(bit.size() == swid_);

	    क्रम (अचिन्हित idx = 0 ; idx < swid_ ; idx += 1)
		  val_.set_bit(idx+soff_, bit.value(idx));
      }

      port.ptr()->send_vec4(val_, 0);
}

व्योम vvp_fun_substitute::recv_vec4_pv(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
				      अचिन्हित base, अचिन्हित vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

व्योम compile_substitute(अक्षर*label, अचिन्हित width,
			अचिन्हित soff, अचिन्हित swidth,
			अचिन्हित argc, काष्ठा symb_s*argv)
{
      vvp_fun_substitute*fun = नया vvp_fun_substitute(width, soff, swidth);

      vvp_net_t*net = नया vvp_net_t;
      net->fun = fun;

      define_functor_प्रतीक(label, net);
      मुक्त(label);

      inputs_connect(net, argc, argv);
      मुक्त(argv);
}
