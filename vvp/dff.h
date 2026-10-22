#यदि_अपरिभाषितम् IVL_dff_H
#परिभाषय IVL_dff_H
/*
 * Copyright (c) 2005-2025 Stephen Williams (steve@icarus.com)
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
 * The vvp_dff implements an arbitrary width D-type FF. The clock,
 * clock-enable, and asynchronous set/clear inputs are single bits.
 * Both positive and negative edge triggered flip-flops are supported.
 * An output is propagated on the chosen edge of the clock input, or
 * on the rising edge of the asynchronous set/clear input. Ports are:
 *
 *   port-0:  D input
 *   port-1:  Clock input
 *   port-2:  Clock Enable input
 *   port-3:  Asynchronous Set/Clear input.
 *
 * The base vvp_dff does not implement an asynchronous set/clear.
 */
class vvp_dff : सार्वजनिक vvp_net_fun_t {

    सार्वजनिक:
      explicit vvp_dff(अचिह्नित विस्तारः, bool negedge);
      ~vvp_dff() override;

      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;

      शून्य recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
			अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t ctx) override;

    गुप्त:
      आभास शून्य recv_async(vvp_net_ptr_t पत्तनम्);

      vvp_bit4_t clk_सक्रिय : 8;
      vvp_bit4_t clk_        : 8;
      vvp_bit4_t ena_        : 8;
      vvp_bit4_t asc_        : 8;

    रक्षित:
      vvp_vector4_t d_;
};

/*
 * This variant implements an asynchronous clear to all zeros.
 */
class vvp_dff_aclr : सार्वजनिक vvp_dff {

    सार्वजनिक:
      explicit vvp_dff_aclr(अचिह्नित विस्तारः, bool negedge);

    गुप्त:
      शून्य recv_async(vvp_net_ptr_t पत्तनम्) override;
};

/*
 * This variant implements an asynchronous set to all ones.
 */
class vvp_dff_aset : सार्वजनिक vvp_dff {

    सार्वजनिक:
      explicit vvp_dff_aset(अचिह्नित विस्तारः, bool negedge);

    गुप्त:
      शून्य recv_async(vvp_net_ptr_t पत्तनम्) override;
};

/*
 * This variant implements an asynchronous set to a specified constant
 * vector value.
 */
class vvp_dff_asc : सार्वजनिक vvp_dff {

    सार्वजनिक:
      explicit vvp_dff_asc(अचिह्नित विस्तारः, bool negedge, ध्रुव वर्ण*asc_मानम्);

    गुप्त:
      शून्य recv_async(vvp_net_ptr_t पत्तनम्) override;

      vvp_vector4_t asc_मानम्_2;
};

#यदि_अन्तः /* IVL_dff_H */
