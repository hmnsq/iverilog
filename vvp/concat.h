#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_concat_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_concat_H
/*
 * Copyright (c) 2004-2025 Stephen Williams (steve@icarus.com)
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

# include "vvp_net.h"

/* vvp_fun_concat
 * This node function creates vectors (vvp_vector4_t) from the
 * concatenation of the inputs. The inputs (4) may be vector or
 * vector8 objects, but they are reduced to vector4 values and
 * strength information lost.
 *
 * The expected widths of the input vectors must be given up front so
 * that the positions in the output vector (and also the size of the
 * output vector) can be worked out. The input vectors must match the
 * expected width.
 */
class vvp_fun_concat  : ಸಾರ್ವಜನಿಕ vvp_net_fun_t, ಸಂರಕ್ಷಿತ vvp_gen_event_s  {

    ಸಾರ್ವಜನಿಕ:
      vvp_fun_concat(ಚಿಹ್ನೆರಹಿತ w0, ಚಿಹ್ನೆರಹಿತ w1,
		     ಚಿಹ್ನೆರಹಿತ w2, ಚಿಹ್ನೆರಹಿತ w3);
      ~vvp_fun_concat() override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t context) final;

      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t) final;
    ಖಾಸಗಿ:
      ಶೂನ್ಯ ಸತತ_ಸತತ() final;

      ಚಿಹ್ನೆರಹಿತ wid_[4];
      vvp_vector4_t val_;
      vvp_net_t *net_ = nullptr;
};

class vvp_fun_concat8  : ಸಾರ್ವಜನಿಕ vvp_net_fun_t, ಸಂರಕ್ಷಿತ vvp_gen_event_s {

    ಸಾರ್ವಜನಿಕ:
      vvp_fun_concat8(ಚಿಹ್ನೆರಹಿತ w0, ಚಿಹ್ನೆರಹಿತ w1,
		     ಚಿಹ್ನೆರಹಿತ w2, ಚಿಹ್ನೆರಹಿತ w3);
      ~vvp_fun_concat8() override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t context) final;
      ಶೂನ್ಯ recv_vec8(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್) final;

      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t) final;
      ಶೂನ್ಯ recv_vec8_pv(vvp_net_ptr_t p, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid) final;

    ಖಾಸಗಿ:
      ಶೂನ್ಯ ಸತತ_ಸತತ() final;

      ಚಿಹ್ನೆರಹಿತ wid_[4];
      vvp_vector8_t val_;
      vvp_net_t *net_ = nullptr;
};

/* vvp_fun_repeat
 * This node function create vectors by repeating the input. The width
 * is the width of the output vector, and the repeat is the number of
 * times to repeat the input. The width of the input vector is
 * implicit from these values.
 */
class vvp_fun_repeat  : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      vvp_fun_repeat(ಚಿಹ್ನೆರಹಿತ ಅಗಲ, ಚಿಹ್ನೆರಹಿತ repeat);
      ~vvp_fun_repeat() override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t context) override;
      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ತಳ, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ vwid,
			vvp_context_t context) final;

    ಖಾಸಗಿ:
      ಚಿಹ್ನೆರಹಿತ wid_;
      ಚಿಹ್ನೆರಹಿತ rep_;
};

#ಆದರೆ_ಅಂತ್ಯ
