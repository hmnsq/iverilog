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


ವರ್ಗ vvp_fun_substitute : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      vvp_fun_substitute(ಚಿಹ್ನೆರಹಿತ wid, ಚಿಹ್ನೆರಹಿತ soff, ಚಿಹ್ನೆರಹಿತ swid);
      ~vvp_fun_substitute() override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್, vvp_context_t) override;

      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t ctx) override;

    ಖಾಸಗಿ:
      ಚಿಹ್ನೆರಹಿತ wid_;
      ಚಿಹ್ನೆರಹಿತ soff_;
      ಚಿಹ್ನೆರಹಿತ swid_;

      vvp_vector4_t val_;
};

vvp_fun_substitute::vvp_fun_substitute(ಚಿಹ್ನೆರಹಿತ wid, ಚಿಹ್ನೆರಹಿತ soff, ಚಿಹ್ನೆರಹಿತ swid)
: wid_(wid), soff_(soff), swid_(swid), val_(wid)
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < val_.size() ; idx += 1)
	    val_.ಗಣ_ಬಿಟ್(idx, BIT4_Z);
}

vvp_fun_substitute::~vvp_fun_substitute()
{
}

ಶೂನ್ಯ vvp_fun_substitute::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				   vvp_context_t)
{
      ಚಿಹ್ನೆರಹಿತ pdx = ಪೋರ್ಟ್.ಪೋರ್ಟ್();
      ಪ್ರತಿಜ್ಞಾ(pdx <= 1);

      ಆದರೆ (pdx == 0) {
	    ಪ್ರತಿಜ್ಞಾ(ಬಿಟ್.size() == wid_);

	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < wid_ ; idx += 1) {
		  ಆದರೆ (idx >= soff_ && idx < (soff_+swid_))
			ಮುಂದುವರಿ;

		  val_.ಗಣ_ಬಿಟ್(idx, ಬಿಟ್.value(idx));
	    }

      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಪ್ರತಿಜ್ಞಾ(ಬಿಟ್.size() == swid_);

	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < swid_ ; idx += 1)
		  val_.ಗಣ_ಬಿಟ್(idx+soff_, ಬಿಟ್.value(idx));
      }

      ಪೋರ್ಟ್.ptr()->send_vec4(val_, 0);
}

ಶೂನ್ಯ vvp_fun_substitute::recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				      ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ಬಿಟ್, ತಳ, vwid, ctx);
}

ಶೂನ್ಯ compile_substitute(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ width,
			ಚಿಹ್ನೆರಹಿತ soff, ಚಿಹ್ನೆರಹಿತ swidth,
			ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      vvp_fun_substitute*fun = ಹೊಸ vvp_fun_substitute(width, soff, swidth);

      vvp_net_t*net = ಹೊಸ vvp_net_t;
      net->fun = fun;

      define_functor_ಚಿಹ್ನೆ(label, net);
      ಬಿಡುಗಡೆ(label);

      inputs_connect(net, argc, argv);
      ಬಿಡುಗಡೆ(argv);
}
