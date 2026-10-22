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


מחלקה vvp_fun_substitute : عام vvp_net_fun_t {

    عام:
      vvp_fun_substitute(unsigned wid, unsigned soff, unsigned swid);
      ~vvp_fun_substitute() override;

      فراغ recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit, vvp_context_t) override;

      فراغ recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t ctx) override;

    خاص:
      unsigned wid_;
      unsigned soff_;
      unsigned swid_;

      vvp_vector4_t val_;
};

vvp_fun_substitute::vvp_fun_substitute(unsigned wid, unsigned soff, unsigned swid)
: wid_(wid), soff_(soff), swid_(swid), val_(wid)
{
      עבור (unsigned idx = 0 ; idx < val_.size() ; idx += 1)
	    val_.set_bit(idx, BIT4_Z);
}

vvp_fun_substitute::~vvp_fun_substitute()
{
}

فراغ vvp_fun_substitute::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
				   vvp_context_t)
{
      unsigned pdx = port.port();
      أكد(pdx <= 1);

      אם (pdx == 0) {
	    أكد(bit.size() == wid_);

	    עבור (unsigned idx = 0 ; idx < wid_ ; idx += 1) {
		  אם (idx >= soff_ && idx < (soff_+swid_))
			استمر;

		  val_.set_bit(idx, bit.value(idx));
	    }

      } אחרת {
	    أكد(bit.size() == swid_);

	    עבור (unsigned idx = 0 ; idx < swid_ ; idx += 1)
		  val_.set_bit(idx+soff_, bit.value(idx));
      }

      port.ptr()->send_vec4(val_, 0);
}

فراغ vvp_fun_substitute::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
				      unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

فراغ compile_substitute(حرف*label, unsigned width,
			unsigned soff, unsigned swidth,
			unsigned argc, بنية symb_s*argv)
{
      vvp_fun_substitute*fun = جديد vvp_fun_substitute(width, soff, swidth);

      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_symbol(label, net);
      حرر(label);

      inputs_connect(net, argc, argv);
      حرر(argv);
}
