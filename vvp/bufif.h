#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_bufif_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_bufif_H
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

# include  "vvp_net.h"

/*
 * The vvp_fun_bufif functor implements the logic of bufif0/1 and
 * notif0/1 gates. Input 0 is the value to be buffered, and input 1 is
 * the enable. The gate processes vectors as bit slices handled by an
 * array of gates.
 *
 * The output from the gate is a vvp_vector8_t. The gate adds
 * strengths to the buffered value, and sends H/L in response to
 * unknown enable bits.
 */
class vvp_fun_bufif  : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      vvp_fun_bufif(bool en_invert, bool ಹೊರಹೋಗು_invert,
		    ಚಿಹ್ನೆರಹಿತ str0, ಚಿಹ್ನೆರಹಿತ str1);

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t) override;
      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t ctx) override;
      ಶೂನ್ಯ recv_real(vvp_net_ptr_t ಪೋರ್ಟ್, ದ್ವಿಗುಣ real,
                     vvp_context_t ctx) override;

    ಖಾಸಗಿ:
      vvp_vector4_t ಬಿಟ್_2;
      vvp_vector4_t en_;
      ಚಿಹ್ನೆರಹಿತ pol_ : 1;
      ಚಿಹ್ನೆರಹಿತ inv_ : 1;
      ಚಿಹ್ನೆರಹಿತ drive0_ : 8;
      ಚಿಹ್ನೆರಹಿತ drive1_ : 8;
};

#ಆದರೆ_ಅಂತ್ಯ /* IVL_bufif_H */
