#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_sfunc_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_sfunc_H
/*
 * Copyright (c) 2006-2025 Stephen Williams (steve@icarus.com)
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

# include  "config.h"

class sfunc_core : ಸಾರ್ವಜನಿಕ vvp_wide_fun_core, ಸಂರಕ್ಷಿತ vvp_gen_event_s {

    ಸಾರ್ವಜನಿಕ:
      sfunc_core(vvp_net_t*ptr, vpiHandle sys, ಚಿಹ್ನೆರಹಿತ argc, vpiHandle*argv);
      ~sfunc_core() override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t context) override;

    ಖಾಸಗಿ:
      ಶೂನ್ಯ recv_vec4_from_inputs(ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್) override;
      ಶೂನ್ಯ recv_real_from_inputs(ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್) override;

      ಶೂನ್ಯ ಸತತ_ಸತತ() override;


    ಖಾಸಗಿ:
      vpiHandle sys_;
      ಚಿಹ್ನೆರಹಿತ  argc_;
      vpiHandle*argv_;
};

#ಆದರೆ_ಅಂತ್ಯ /* IVL_sfunc_H */
