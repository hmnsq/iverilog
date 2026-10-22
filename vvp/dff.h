#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_dff_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_dff_H
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
class vvp_dff : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_dff(ಚಿಹ್ನೆರಹಿತ ಅಗಲ, bool negedge);
      ~vvp_dff() override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t) override;

      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t ctx) override;

    ಖಾಸಗಿ:
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ recv_async(vvp_net_ptr_t ಪೋರ್ಟ್);

      vvp_bit4_t clk_ಸಕ್ರಿಯ : 8;
      vvp_bit4_t clk_        : 8;
      vvp_bit4_t ena_        : 8;
      vvp_bit4_t asc_        : 8;

    ಸಂರಕ್ಷಿತ:
      vvp_vector4_t d_;
};

/*
 * This variant implements an asynchronous clear to all zeros.
 */
class vvp_dff_aclr : ಸಾರ್ವಜನಿಕ vvp_dff {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_dff_aclr(ಚಿಹ್ನೆರಹಿತ ಅಗಲ, bool negedge);

    ಖಾಸಗಿ:
      ಶೂನ್ಯ recv_async(vvp_net_ptr_t ಪೋರ್ಟ್) override;
};

/*
 * This variant implements an asynchronous set to all ones.
 */
class vvp_dff_aset : ಸಾರ್ವಜನಿಕ vvp_dff {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_dff_aset(ಚಿಹ್ನೆರಹಿತ ಅಗಲ, bool negedge);

    ಖಾಸಗಿ:
      ಶೂನ್ಯ recv_async(vvp_net_ptr_t ಪೋರ್ಟ್) override;
};

/*
 * This variant implements an asynchronous set to a specified constant
 * vector value.
 */
class vvp_dff_asc : ಸಾರ್ವಜನಿಕ vvp_dff {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_dff_asc(ಚಿಹ್ನೆರಹಿತ ಅಗಲ, bool negedge, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*asc_ಮೌಲ್ಯ);

    ಖಾಸಗಿ:
      ಶೂನ್ಯ recv_async(vvp_net_ptr_t ಪೋರ್ಟ್) override;

      vvp_vector4_t asc_ಮೌಲ್ಯ_2;
};

#ಆದರೆ_ಅಂತ್ಯ /* IVL_dff_H */
