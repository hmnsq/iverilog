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


صنف vvp_fun_substitute : عام vvp_net_fun_t {

    عام:
      vvp_fun_substitute(بیعلامت wid, بیعلامت soff, بیعلامت swid);
      ~vvp_fun_substitute() override;

      فراغ recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت, vvp_context_t) override;

      فراغ recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
			بیعلامت پایه, بیعلامت vwid, vvp_context_t ctx) override;

    خاص:
      بیعلامت wid_;
      بیعلامت soff_;
      بیعلامت swid_;

      vvp_vector4_t val_;
};

vvp_fun_substitute::vvp_fun_substitute(بیعلامت wid, بیعلامت soff, بیعلامت swid)
: wid_(wid), soff_(soff), swid_(swid), val_(wid)
{
      لكل (بیعلامت idx = 0 ; idx < val_.size() ; idx += 1)
	    val_.مجموعه_بیت(idx, BIT4_Z);
}

vvp_fun_substitute::~vvp_fun_substitute()
{
}

فراغ vvp_fun_substitute::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
				   vvp_context_t)
{
      بیعلامت pdx = پورت.پورت();
      أكد(pdx <= 1);

      إذا (pdx == 0) {
	    أكد(بیت.size() == wid_);

	    لكل (بیعلامت idx = 0 ; idx < wid_ ; idx += 1) {
		  إذا (idx >= soff_ && idx < (soff_+swid_))
			استمر;

		  val_.مجموعه_بیت(idx, بیت.value(idx));
	    }

      } وإلا {
	    أكد(بیت.size() == swid_);

	    لكل (بیعلامت idx = 0 ; idx < swid_ ; idx += 1)
		  val_.مجموعه_بیت(idx+soff_, بیت.value(idx));
      }

      پورت.ptr()->send_vec4(val_, 0);
}

فراغ vvp_fun_substitute::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
				      بیعلامت پایه, بیعلامت vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, بیت, پایه, vwid, ctx);
}

فراغ compile_substitute(حرف*label, بیعلامت width,
			بیعلامت soff, بیعلامت swidth,
			بیعلامت argc, بنية symb_s*argv)
{
      vvp_fun_substitute*fun = جديد vvp_fun_substitute(width, soff, swidth);

      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_نماد(label, net);
      حرر(label);

      inputs_connect(net, argc, argv);
      حرر(argv);
}
