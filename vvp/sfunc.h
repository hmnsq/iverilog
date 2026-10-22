#ifndef IVL_sfunc_H
#define IVL_sfunc_H
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

class sfunc_core : பொது vvp_wide_fun_core, பாதுகாக்கப்பட்ட vvp_gen_event_s {

    பொது:
      sfunc_core(vvp_net_t*ptr, vpiHandle sys, unsigned argc, vpiHandle*argv);
      ~sfunc_core() override;

      வெறுமை recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                     vvp_context_t context) override;

    தனிப்பட்ட:
      வெறுமை recv_vec4_from_inputs(unsigned port) override;
      வெறுமை recv_real_from_inputs(unsigned port) override;

      வெறுமை run_run() override;


    தனிப்பட்ட:
      vpiHandle sys_;
      unsigned  argc_;
      vpiHandle*argv_;
};

#endif /* IVL_sfunc_H */
