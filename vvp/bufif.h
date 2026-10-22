#यदि_अपरिभाषितम् IVL_bufif_H
#परिभाषय IVL_bufif_H
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
class vvp_fun_bufif  : सार्वजनिक vvp_net_fun_t {

    सार्वजनिक:
      vvp_fun_bufif(bool en_invert, bool बहिर्गमनम्_invert,
		    अचिह्नित str0, अचिह्नित str1);

      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;
      शून्य recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
			अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t ctx) override;
      शून्य recv_real(vvp_net_ptr_t पत्तनम्, द्विगुण real,
                     vvp_context_t ctx) override;

    गुप्त:
      vvp_vector4_t bit_;
      vvp_vector4_t en_;
      अचिह्नित pol_ : 1;
      अचिह्नित inv_ : 1;
      अचिह्नित drive0_ : 8;
      अचिह्नित drive1_ : 8;
};

#यदि_अन्तः /* IVL_bufif_H */
