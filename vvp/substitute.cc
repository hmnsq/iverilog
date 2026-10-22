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


جماعت vvp_fun_substitute : عوامی vvp_net_fun_t {

    عوامی:
      vvp_fun_substitute(غیرعلامتی wid, غیرعلامتی soff, غیرعلامتی swid);
      ~vvp_fun_substitute() override;

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ, vvp_context_t) override;

      خالی recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t ctx) override;

    نجی:
      غیرعلامتی wid_;
      غیرعلامتی soff_;
      غیرعلامتی swid_;

      vvp_vector4_t val_;
};

vvp_fun_substitute::vvp_fun_substitute(غیرعلامتی wid, غیرعلامتی soff, غیرعلامتی swid)
: wid_(wid), soff_(soff), swid_(swid), val_(wid)
{
      برائے (غیرعلامتی idx = 0 ; idx < val_.size() ; idx += 1)
	    val_.سیٹ_ثنائی_ہندسہ(idx, BIT4_Z);
}

vvp_fun_substitute::~vvp_fun_substitute()
{
}

خالی vvp_fun_substitute::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				   vvp_context_t)
{
      غیرعلامتی pdx = پورٹ.پورٹ();
      أكد(pdx <= 1);

      اگر (pdx == 0) {
	    أكد(ثنائی_ہندسہ.size() == wid_);

	    برائے (غیرعلامتی idx = 0 ; idx < wid_ ; idx += 1) {
		  اگر (idx >= soff_ && idx < (soff_+swid_))
			جاری;

		  val_.سیٹ_ثنائی_ہندسہ(idx, ثنائی_ہندسہ.value(idx));
	    }

      } ورنہ {
	    أكد(ثنائی_ہندسہ.size() == swid_);

	    برائے (غیرعلامتی idx = 0 ; idx < swid_ ; idx += 1)
		  val_.سیٹ_ثنائی_ہندسہ(idx+soff_, ثنائی_ہندسہ.value(idx));
      }

      پورٹ.ptr()->send_vec4(val_, 0);
}

خالی vvp_fun_substitute::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				      غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ثنائی_ہندسہ, بنیاد, vwid, ctx);
}

خالی compile_substitute(حرف*label, غیرعلامتی width,
			غیرعلامتی soff, غیرعلامتی swidth,
			غیرعلامتی argc, ساخت symb_s*argv)
{
      vvp_fun_substitute*fun = نیا vvp_fun_substitute(width, soff, swidth);

      vvp_net_t*net = نیا vvp_net_t;
      net->fun = fun;

      define_functor_علامت(label, net);
      آزاد(label);

      inputs_connect(net, argc, argv);
      آزاد(argv);
}
