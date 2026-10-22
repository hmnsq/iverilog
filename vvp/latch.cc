/*
 * Copyright (c) 2016 Martin Whitaker (icarus@martin-whitaker.me.uk)
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
# include  "schedule.h"
# include  "latch.h"
# include  <climits>
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>
# include  <iostream>

/* We need to ensure an initial output value is propagated. This is
   achieved by scheduling an initial value to be sent to port 3. Any
   value received on port 3 will propagate an initial value of 'bx. */

vvp_latch::vvp_latch(ಚಿಹ್ನೆರಹಿತ width)
: en_(BIT4_X), d_(width, BIT4_X)
{
}

vvp_latch::~vvp_latch()
{
}

ಶೂನ್ಯ vvp_latch::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                          vvp_context_t)
{
      vvp_bit4_t tmp;

      ಆಯ್ಕೆ (ಪೋರ್ಟ್.ಪೋರ್ಟ್()) {

	  ಸ್ಥಿತಿ 0: // D
	    d_ = ಬಿಟ್;
	    ಆದರೆ (en_ == BIT4_1)
		  ವೇಳಾಪಟ್ಟಿ_propagate_ಸದಿಶ(ಪೋರ್ಟ್.ptr(), 0, d_);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ 1: // EN
	    ಪ್ರತಿಜ್ಞಾ(ಬಿಟ್.size() == 1);
	    tmp = en_;
	    en_ = ಬಿಟ್.value(0);
	    ಆದರೆ (en_ == BIT4_1 && tmp != BIT4_1)
		  ವೇಳಾಪಟ್ಟಿ_propagate_ಸದಿಶ(ಪೋರ್ಟ್.ptr(), 0, d_);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ 2:
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ 3:
	    ಪೋರ್ಟ್.ptr()->send_vec4(vvp_vector4_t(d_.size(), BIT4_X), 0);
	    ನಿಲ್ಲಿಸು;
      }
}

ಶೂನ್ಯ vvp_latch::recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			     ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ಬಿಟ್, ತಳ, vwid, ctx);
}

ಶೂನ್ಯ compile_latch(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ width,
		   ರಚನೆ symb_s arg_d,
		   ರಚನೆ symb_s arg_e)
{
      vvp_net_t*ptr = ಹೊಸ vvp_net_t;
      vvp_latch*fun = ಹೊಸ vvp_latch(width);

      ptr->fun = fun;
      define_functor_ಚಿಹ್ನೆ(label, ptr);
      ಬಿಡುಗಡೆ(label);
      ಒಳಹರಿವು_connect(ptr, 0, arg_d.ಪಠ್ಯ);
      ಒಳಹರಿವು_connect(ptr, 1, arg_e.ಪಠ್ಯ);

      vvp_vector4_t init_val = vvp_vector4_t(1, BIT4_1);
      ವೇಳಾಪಟ್ಟಿ_init_ಸದಿಶ(vvp_net_ptr_t(ptr,3), init_val);
}
