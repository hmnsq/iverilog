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
      vvp_fun_substitute(بنانشانی wid, بنانشانی soff, بنانشانی swid);
      ~vvp_fun_substitute() override;

      فراغ recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ, vvp_context_t) override;

      فراغ recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			بنانشانی ادھار, بنانشانی vwid, vvp_context_t ctx) override;

    خاص:
      بنانشانی wid_;
      بنانشانی soff_;
      بنانشانی swid_;

      vvp_vector4_t val_;
};

vvp_fun_substitute::vvp_fun_substitute(بنانشانی wid, بنانشانی soff, بنانشانی swid)
: wid_(wid), soff_(soff), swid_(swid), val_(wid)
{
      لكل (بنانشانی idx = 0 ; idx < val_.size() ; idx += 1)
	    val_.سیٹ_ثنائی_ہندسہ(idx, BIT4_Z);
}

vvp_fun_substitute::~vvp_fun_substitute()
{
}

فراغ vvp_fun_substitute::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				   vvp_context_t)
{
      بنانشانی pdx = پورٹ.پورٹ();
      أكد(pdx <= 1);

      إذا (pdx == 0) {
	    أكد(ثنائی_ہندسہ.size() == wid_);

	    لكل (بنانشانی idx = 0 ; idx < wid_ ; idx += 1) {
		  إذا (idx >= soff_ && idx < (soff_+swid_))
			استمر;

		  val_.سیٹ_ثنائی_ہندسہ(idx, ثنائی_ہندسہ.value(idx));
	    }

      } وإلا {
	    أكد(ثنائی_ہندسہ.size() == swid_);

	    لكل (بنانشانی idx = 0 ; idx < swid_ ; idx += 1)
		  val_.سیٹ_ثنائی_ہندسہ(idx+soff_, ثنائی_ہندسہ.value(idx));
      }

      پورٹ.ptr()->send_vec4(val_, 0);
}

فراغ vvp_fun_substitute::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				      بنانشانی ادھار, بنانشانی vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ثنائی_ہندسہ, ادھار, vwid, ctx);
}

فراغ compile_substitute(حرف*label, بنانشانی width,
			بنانشانی soff, بنانشانی swidth,
			بنانشانی argc, بنية symb_s*argv)
{
      vvp_fun_substitute*fun = جديد vvp_fun_substitute(width, soff, swidth);

      vvp_net_t*net = جديد vvp_net_t;
      net->fun = fun;

      define_functor_علامت(label, net);
      حرر(label);

      inputs_connect(net, argc, argv);
      حرر(argv);
}
