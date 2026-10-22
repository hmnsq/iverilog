/*
 * Copyright (c) 2001-2025 Stephen Williams (steve@icarus.com)
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

# include  "bufif.h"
# include  "schedule.h"
# include  "statistics.h"
# include  <iostream>
# include  <cassert>

vvp_fun_bufif::vvp_fun_bufif(ತಾರ್ಕಿಕ en_invert, ತಾರ್ಕಿಕ ಹೊರಹೋಗು_invert,
			     ಚಿಹ್ನೆರಹಿತ str0, ಚಿಹ್ನೆರಹಿತ str1)
: pol_(en_invert? 1 : 0), inv_(ಹೊರಹೋಗು_invert? 1 : 0)
{
      drive0_ = str0;
      drive1_ = str1;
      ಎಣಿಕೆ_functors_bufif += 1;
}

ಶೂನ್ಯ vvp_fun_bufif::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                              vvp_context_t)
{
      ಆಯ್ಕೆ (ptr.ಪೋರ್ಟ್()) {
	  ಸ್ಥಿತಿ 0:
	    ಬಿಟ್_2 = inv_? ~ಬಿಟ್ : ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 1:
	    en_ = pol_? ~ಬಿಟ್ : ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_vector8_t out (ಬಿಟ್.size());

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < ಬಿಟ್.size() ;  idx += 1) {
	    vvp_bit4_t b_en  = en_.value(idx);
	    vvp_bit4_t b_ಬಿಟ್ = ಬಿಟ್_2.value(idx);

	    ಆಯ್ಕೆ (b_en) {
		ಸ್ಥಿತಿ BIT4_0:
		  out.ಗಣ_ಬಿಟ್(idx, vvp_scalar_t(BIT4_Z,drive0_,drive1_));
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_1:
		  ಆದರೆ (bit4_is_xz(b_ಬಿಟ್))
			out.ಗಣ_ಬಿಟ್(idx, vvp_scalar_t(BIT4_X,drive0_,drive1_));
		  ಇಲ್ಲದಿದ್ದರೆ
			out.ಗಣ_ಬಿಟ್(idx, vvp_scalar_t(b_ಬಿಟ್,drive0_,drive1_));
		  ನಿಲ್ಲಿಸು;

		ಪೂರ್ವನಿಯೋಜಿತ:
		  ಆಯ್ಕೆ (b_ಬಿಟ್) {
		      ಸ್ಥಿತಿ BIT4_0:
			out.ಗಣ_ಬಿಟ್(idx, vvp_scalar_t(BIT4_X,drive0_,0));
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ BIT4_1:
			out.ಗಣ_ಬಿಟ್(idx, vvp_scalar_t(BIT4_X,0,drive1_));
			ನಿಲ್ಲಿಸು;
		      ಪೂರ್ವನಿಯೋಜಿತ:
			out.ಗಣ_ಬಿಟ್(idx, vvp_scalar_t(BIT4_X,drive0_,drive1_));
			ನಿಲ್ಲಿಸು;
		  }
		  ನಿಲ್ಲಿಸು;
	    }
      }

      ptr.ptr()->send_vec8(out);
}

ಶೂನ್ಯ vvp_fun_bufif::recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			         ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ಬಿಟ್, ತಳ, vwid, ctx);
}

ಶೂನ್ಯ vvp_fun_bufif::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ real,
                              vvp_context_t ctx)
{
      recv_vec4(ptr, double_ಗಮ್ಯ_vector4_LSB(real), ctx);
}
