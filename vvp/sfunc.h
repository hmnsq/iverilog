#جيڪڏهن_وصف_نه_ٿيل IVL_sfunc_H
#وصف_ڪريو IVL_sfunc_H
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

class sfunc_core : عام vvp_wide_fun_core, محمي vvp_gen_event_s {

    عام:
      sfunc_core(vvp_net_t*ptr, vpiHandle sys, بنانشان argc, vpiHandle*argv);
      ~sfunc_core() override;

      فراغ recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                     vvp_context_t context) override;

    خاص:
      فراغ recv_vec4_from_inputs(بنانشان پورٽ) override;
      فراغ recv_real_from_inputs(بنانشان پورٽ) override;

      فراغ لاڳيتو_لاڳيتو() override;


    خاص:
      vpiHandle sys_;
      بنانشان  argc_;
      vpiHandle*argv_;
};

#جيڪڏهن_پورو /* IVL_sfunc_H */
