/*
 * Copyright (c) 2005-2016 Stephen Williams <steve@icarus.com>
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

# include  "vvp_net.h"
# include  "compile.h"
# include  <cstring>
# include  <iostream>
# include  <cassert>

vvp_fun_extend_signed::vvp_fun_extend_signed(ಚಿಹ್ನೆರಹಿತ wid)
: ಅಗಲ_2(wid)
{
}

vvp_fun_extend_signed::~vvp_fun_extend_signed()
{
}

ಶೂನ್ಯ vvp_fun_extend_signed::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                      vvp_context_t)
{
      ಆದರೆ (ಬಿಟ್.size() >= ಅಗಲ_2) {
	    ಪೋರ್ಟ್.ptr()->send_vec4(ಬಿಟ್, 0);
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_vector4_t res (ಅಗಲ_2);

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಬಿಟ್.size() ;  idx += 1)
	    res.ಗಣ_ಬಿಟ್(idx, ಬಿಟ್.value(idx));

      vvp_bit4_t pad = ಬಿಟ್.size() > 0? ಬಿಟ್.value(ಬಿಟ್.size()-1) : BIT4_0;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = ಬಿಟ್.size() ;  idx < res.size() ;  idx += 1)
	    res.ಗಣ_ಬಿಟ್(idx, pad);

      ಪೋರ್ಟ್.ptr()->send_vec4(res, 0);
}

ಶೂನ್ಯ vvp_fun_extend_signed::recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
					 ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ಬಿಟ್, ತಳ, vwid, ctx);
}
