#यदि_अपरिभाषितम् IVL_sfunc_H
#परिभाषय IVL_sfunc_H
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

class sfunc_core : सार्वजनिक vvp_wide_fun_core, रक्षित vvp_gen_event_s {

    सार्वजनिक:
      sfunc_core(vvp_net_t*ptr, vpiHandle sys, अचिह्नित argc, vpiHandle*argv);
      ~sfunc_core() override;

      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t context) override;

    गुप्त:
      शून्य recv_vec4_from_inputs(अचिह्नित पत्तनम्) override;
      शून्य recv_real_from_inputs(अचिह्नित पत्तनम्) override;

      शून्य सततम्_सततम्() override;


    गुप्त:
      vpiHandle sys_;
      अचिह्नित  argc_;
      vpiHandle*argv_;
};

#यदि_अन्तः /* IVL_sfunc_H */
