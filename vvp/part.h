#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_part_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_part_H
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

# include  "schedule.h"
# include  "config.h"

/* vvp_fun_part
 * This node takes a part select of the input vector. Input 0 is the
 * vector to be selected from, and input 1 is the location where the
 * select starts. Input 2, which is typically constant, is the width
 * of the result.
 */
class vvp_fun_part  : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      vvp_fun_part(ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid);
      ~vvp_fun_part() override;

      ಚಿಹ್ನೆರಹಿತ get_ತಳ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ತಳ_2; }
      ಚಿಹ್ನೆರಹಿತ get_wid() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ wid_; }

    ಸಂರಕ್ಷಿತ:
      ಚಿಹ್ನೆರಹಿತ ತಳ_2;
      ಚಿಹ್ನೆರಹಿತ wid_;
};

/*
 * Statically allocated vvp_fun_part.
 */
class vvp_fun_part_sa  : ಸಾರ್ವಜನಿಕ vvp_fun_part, ಸಾರ್ವಜನಿಕ vvp_gen_event_s {

    ಸಾರ್ವಜನಿಕ:
      vvp_fun_part_sa(ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid);
      ~vvp_fun_part_sa() override;

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t) override;

      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t) override;

    ಖಾಸಗಿ:
      ಶೂನ್ಯ ಸತತ_ಸತತ() override;

    ಖಾಸಗಿ:
      vvp_vector4_t val_;
      vvp_net_t*net_;
};

/*
 * Automatically allocated vvp_fun_part.
 */
class vvp_fun_part_aa  : ಸಾರ್ವಜನಿಕ vvp_fun_part, ಸಾರ್ವಜನಿಕ automatic_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      vvp_fun_part_aa(ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid);
      ~vvp_fun_part_aa() override;

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ alloc_instance(vvp_context_t context) override;
      ಶೂನ್ಯ reset_instance(vvp_context_t context) override;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      ಶೂನ್ಯ free_instance(vvp_context_t context) override;
#ಆದರೆ_ಅಂತ್ಯ

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t context) override;

      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t context) override;

    ಖಾಸಗಿ:
      __vpiScope*context_scope_;
      ಚಿಹ್ನೆರಹಿತ context_idx_;
};

/* vvp_fun_part_pv
 * This node takes a vector input and turns it into the part select of
 * a wider output network. It used the recv_vec4_pv methods of the
 * destination nodes to propagate the part select. It can be used in
 * both statically and automatically allocated scopes, as it has no
 * dynamic state.
 */
class vvp_fun_part_pv  : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      vvp_fun_part_pv(ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid, ಚಿಹ್ನೆರಹಿತ vec_wid);
      ~vvp_fun_part_pv() override;

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t context) override;

      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                        ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t) override;

      ಶೂನ್ಯ recv_vec8(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್) override;

    ಖಾಸಗಿ:
      ಚಿಹ್ನೆರಹಿತ ತಳ_2;
      ಚಿಹ್ನೆರಹಿತ wid_;
      ಚಿಹ್ನೆರಹಿತ vwid_;
};

/*
 * This part select is more flexible in that it takes the vector to
 * part in port 0, and the base of the part in port 1. The width of
 * the part to take out is fixed.
 */
class vvp_fun_part_var  : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_part_var(ಚಿಹ್ನೆರಹಿತ wid, bool is_signed);
      ~vvp_fun_part_var() override;

    ಸಂರಕ್ಷಿತ:
      bool recv_vec4_(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                      ಪೂರ್ಣಾಂಕ&ತಳ, vvp_vector4_t&ಮೂಲ,
                      vvp_vector4_t&ref);

      ಚಿಹ್ನೆರಹಿತ wid_;
      bool is_signed_;
};

/*
 * Statically allocated vvp_fun_part_var.
 */
class vvp_fun_part_var_sa  : ಸಾರ್ವಜನಿಕ vvp_fun_part_var {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_part_var_sa(ಚಿಹ್ನೆರಹಿತ wid, bool is_signed);
      ~vvp_fun_part_var_sa() override;

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t) override;

      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t) override;

    ಖಾಸಗಿ:
      ಪೂರ್ಣಾಂಕ ತಳ_2;
      vvp_vector4_t ಮೂಲ_2;
	// Save the last output, for detecting change.
      vvp_vector4_t ref_;
};

/*
 * Automatically allocated vvp_fun_part_var.
 */
class vvp_fun_part_var_aa  : ಸಾರ್ವಜನಿಕ vvp_fun_part_var, ಸಾರ್ವಜನಿಕ automatic_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_part_var_aa(ಚಿಹ್ನೆರಹಿತ wid, bool is_signed);
      ~vvp_fun_part_var_aa() override;

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ alloc_instance(vvp_context_t context) override;
      ಶೂನ್ಯ reset_instance(vvp_context_t context) override;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      ಶೂನ್ಯ free_instance(vvp_context_t context) override;
#ಆದರೆ_ಅಂತ್ಯ

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t context) override;

      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t context) override;

    ಖಾಸಗಿ:
      __vpiScope*context_scope_;
      ಚಿಹ್ನೆರಹಿತ context_idx_;
};

#ಆದರೆ_ಅಂತ್ಯ /* IVL_part_H */
