#اگر_غیر_تعریف_شدہ IVL_bufif_H
#تعریف_کرو IVL_bufif_H
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
class vvp_fun_bufif  : عوامی vvp_net_fun_t {

    عوامی:
      vvp_fun_bufif(bool en_invert, bool برآمد_invert,
		    غیرعلامتی str0, غیرعلامتی str1);

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;
      خالی recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t ctx) override;
      خالی recv_real(vvp_net_ptr_t پورٹ, دوہرا real,
                     vvp_context_t ctx) override;

    نجی:
      vvp_vector4_t ثنائی_ہندسہ_2;
      vvp_vector4_t en_;
      غیرعلامتی pol_ : 1;
      غیرعلامتی inv_ : 1;
      غیرعلامتی drive0_ : 8;
      غیرعلامتی drive1_ : 8;
};

#اگر_ختم /* IVL_bufif_H */
