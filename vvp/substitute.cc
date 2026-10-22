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
      vvp_fun_substitute(بنانشان wid, بنانشان soff, بنانشان swid);
      ~vvp_fun_substitute() override;

      فراغ recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ, vvp_context_t) override;

      فراغ recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ٻٽو_انگ,
			بنانشان بنياد, بنانشان vwid, vvp_context_t ctx) override;

    خاص:
      بنانشان wid_;
      بنانشان soff_;
      بنانشان swid_;

      vvp_vector4_t val_;
};

vvp_fun_substitute::vvp_fun_substitute(بنانشان wid, بنانشان soff, بنانشان swid)
: wid_(wid), soff_(soff), swid_(swid), val_(wid)
{
      لكل (بنانشان idx = 0 ; idx < val_.size() ; idx += 1)
	    val_.سيٽ_ٻٽو_انگ(idx, BIT4_Z);
}

vvp_fun_substitute::~vvp_fun_substitute()
{
}

فراغ vvp_fun_substitute::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
				   vvp_context_t)
{
      بنانشان pdx = پورٽ.پورٽ();
      أكد(pdx <= 1);

      إذا (pdx == 0) {
	    أكد(ٻٽو_انگ.size() == wid_);

	    لكل (بنانشان idx = 0 ; idx < wid_ ; idx += 1) {
		  إذا (idx >= soff_ && idx < (soff_+swid_))
			استمر;

		  val_.سيٽ_ٻٽو_انگ(idx, ٻٽو_انگ.value(idx));
	    }

      } وإلا {
	    أكد(ٻٽو_انگ.size() == swid_);

	    لكل (بنانشان idx = 0 ; idx < swid_ ; idx += 1)
		  val_.سيٽ_ٻٽو_انگ(idx+soff_, ٻٽو_انگ.value(idx));
      }

      پورٽ.ptr()->send_vec4(val_, 0);
}

فراغ vvp_fun_substitute::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ٻٽو_انگ,
				      بنانشان بنياد, بنانشان vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ٻٽو_انگ, بنياد, vwid, ctx);
}

فراغ compile_substitute(حرف*label, بنانشان width,
			بنانشان soff, بنانشان swidth,
			بنانشان argc, بنية symb_s*argv)
{
      vvp_fun_substitute*fun = جديد vvp_fun_substitute(width, soff, swidth);

      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_نشان(label, net);
      حرر(label);

      inputs_connect(net, argc, argv);
      حرر(argv);
}
