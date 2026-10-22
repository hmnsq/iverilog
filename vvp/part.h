#यदि_अपरिभाषितम् IVL_part_H
#परिभाषय IVL_part_H
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
class vvp_fun_part  : सार्वजनिक vvp_net_fun_t {

    सार्वजनिक:
      vvp_fun_part(अचिह्नित आधारः, अचिह्नित wid);
      ~vvp_fun_part() override;

      अचिह्नित get_आधारः() ध्रुव { प्रत्यागम आधारः_2; }
      अचिह्नित get_wid() ध्रुव { प्रत्यागम wid_; }

    रक्षित:
      अचिह्नित आधारः_2;
      अचिह्नित wid_;
};

/*
 * Statically allocated vvp_fun_part.
 */
class vvp_fun_part_sa  : सार्वजनिक vvp_fun_part, सार्वजनिक vvp_gen_event_s {

    सार्वजनिक:
      vvp_fun_part_sa(अचिह्नित आधारः, अचिह्नित wid);
      ~vvp_fun_part_sa() override;

    सार्वजनिक:
      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;

      शून्य recv_vec4_pv(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
			अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t) override;

    गुप्त:
      शून्य सततम्_सततम्() override;

    गुप्त:
      vvp_vector4_t val_;
      vvp_net_t*net_;
};

/*
 * Automatically allocated vvp_fun_part.
 */
class vvp_fun_part_aa  : सार्वजनिक vvp_fun_part, सार्वजनिक automatic_hooks_s {

    सार्वजनिक:
      vvp_fun_part_aa(अचिह्नित आधारः, अचिह्नित wid);
      ~vvp_fun_part_aa() override;

    सार्वजनिक:
      शून्य alloc_instance(vvp_context_t context) override;
      शून्य reset_instance(vvp_context_t context) override;
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
      शून्य free_instance(vvp_context_t context) override;
#यदि_अन्तः

      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t context) override;

      शून्य recv_vec4_pv(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
			अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t context) override;

    गुप्त:
      __vpiScope*context_scope_;
      अचिह्नित context_idx_;
};

/* vvp_fun_part_pv
 * This node takes a vector input and turns it into the part select of
 * a wider output network. It used the recv_vec4_pv methods of the
 * destination nodes to propagate the part select. It can be used in
 * both statically and automatically allocated scopes, as it has no
 * dynamic state.
 */
class vvp_fun_part_pv  : सार्वजनिक vvp_net_fun_t {

    सार्वजनिक:
      vvp_fun_part_pv(अचिह्नित आधारः, अचिह्नित wid, अचिह्नित vec_wid);
      ~vvp_fun_part_pv() override;

    सार्वजनिक:
      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t context) override;

      शून्य recv_vec4_pv(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                        अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t) override;

      शून्य recv_vec8(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector8_t&bit) override;

    गुप्त:
      अचिह्नित आधारः_2;
      अचिह्नित wid_;
      अचिह्नित vwid_;
};

/*
 * This part select is more flexible in that it takes the vector to
 * part in port 0, and the base of the part in port 1. The width of
 * the part to take out is fixed.
 */
class vvp_fun_part_var  : सार्वजनिक vvp_net_fun_t {

    सार्वजनिक:
      explicit vvp_fun_part_var(अचिह्नित wid, bool is_signed);
      ~vvp_fun_part_var() override;

    रक्षित:
      bool recv_vec4_(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                      पूर्णाङ्क&आधारः, vvp_vector4_t&source,
                      vvp_vector4_t&ref);

      अचिह्नित wid_;
      bool is_signed_;
};

/*
 * Statically allocated vvp_fun_part_var.
 */
class vvp_fun_part_var_sa  : सार्वजनिक vvp_fun_part_var {

    सार्वजनिक:
      explicit vvp_fun_part_var_sa(अचिह्नित wid, bool is_signed);
      ~vvp_fun_part_var_sa() override;

    सार्वजनिक:
      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;

      शून्य recv_vec4_pv(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
			अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t) override;

    गुप्त:
      पूर्णाङ्क आधारः_2;
      vvp_vector4_t source_;
	// Save the last output, for detecting change.
      vvp_vector4_t ref_;
};

/*
 * Automatically allocated vvp_fun_part_var.
 */
class vvp_fun_part_var_aa  : सार्वजनिक vvp_fun_part_var, सार्वजनिक automatic_hooks_s {

    सार्वजनिक:
      explicit vvp_fun_part_var_aa(अचिह्नित wid, bool is_signed);
      ~vvp_fun_part_var_aa() override;

    सार्वजनिक:
      शून्य alloc_instance(vvp_context_t context) override;
      शून्य reset_instance(vvp_context_t context) override;
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
      शून्य free_instance(vvp_context_t context) override;
#यदि_अन्तः

      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t context) override;

      शून्य recv_vec4_pv(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
			अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t context) override;

    गुप्त:
      __vpiScope*context_scope_;
      अचिह्नित context_idx_;
};

#यदि_अन्तः /* IVL_part_H */
