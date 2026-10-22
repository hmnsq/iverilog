#اگر_غیر_تعریف_شدہ IVL_sfunc_H
#تعریف_کرو IVL_sfunc_H
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

class sfunc_core : عوامی vvp_wide_fun_core, محفوظ vvp_gen_event_s {

    عوامی:
      sfunc_core(vvp_net_t*ptr, vpiHandle sys, غیرعلامتی argc, vpiHandle*argv);
      ~sfunc_core() override;

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;

    نجی:
      خالی recv_vec4_from_inputs(غیرعلامتی پورٹ) override;
      خالی recv_real_from_inputs(غیرعلامتی پورٹ) override;

      خالی تسلسل_تسلسل() override;


    نجی:
      vpiHandle sys_;
      غیرعلامتی  argc_;
      vpiHandle*argv_;
};

#اگر_ختم /* IVL_sfunc_H */
