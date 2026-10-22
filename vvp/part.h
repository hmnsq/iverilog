#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_part_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_part_H
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
class vvp_fun_part  : ਜਨਤਕ vvp_net_fun_t {

    ਜਨਤਕ:
      vvp_fun_part(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ~vvp_fun_part() override;

      ਬਿਨਾਨਿਸ਼ਾਨੀ get_ਅਧਾਰ() ਅਚਲ { ਵਾਪਸ ਅਧਾਰ_2; }
      ਬਿਨਾਨਿਸ਼ਾਨੀ get_wid() ਅਚਲ { ਵਾਪਸ wid_; }

    ਸੁਰੱਖਿਅਤ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ_2;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid_;
};

/*
 * Statically allocated vvp_fun_part.
 */
class vvp_fun_part_sa  : ਜਨਤਕ vvp_fun_part, ਜਨਤਕ vvp_gen_event_s {

    ਜਨਤਕ:
      vvp_fun_part_sa(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ~vvp_fun_part_sa() override;

    ਜਨਤਕ:
      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t) override;

    ਨਿਜੀ:
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() override;

    ਨਿਜੀ:
      vvp_vector4_t val_;
      vvp_net_t*net_;
};

/*
 * Automatically allocated vvp_fun_part.
 */
class vvp_fun_part_aa  : ਜਨਤਕ vvp_fun_part, ਜਨਤਕ automatic_hooks_s {

    ਜਨਤਕ:
      vvp_fun_part_aa(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid);
      ~vvp_fun_part_aa() override;

    ਜਨਤਕ:
      ਖਾਲੀ alloc_instance(vvp_context_t context) override;
      ਖਾਲੀ reset_instance(vvp_context_t context) override;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਖਾਲੀ free_instance(vvp_context_t context) override;
#ਜੇ_ਅੰਤ

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t context) override;

      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t context) override;

    ਨਿਜੀ:
      __vpiScope*context_scope_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ context_idx_;
};

/* vvp_fun_part_pv
 * This node takes a vector input and turns it into the part select of
 * a wider output network. It used the recv_vec4_pv methods of the
 * destination nodes to propagate the part select. It can be used in
 * both statically and automatically allocated scopes, as it has no
 * dynamic state.
 */
class vvp_fun_part_pv  : ਜਨਤਕ vvp_net_fun_t {

    ਜਨਤਕ:
      vvp_fun_part_pv(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਬਿਨਾਨਿਸ਼ਾਨੀ vec_wid);
      ~vvp_fun_part_pv() override;

    ਜਨਤਕ:
      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t context) override;

      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                        ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t) override;

      ਖਾਲੀ recv_vec8(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ) override;

    ਨਿਜੀ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ_2;
      ਬਿਨਾਨਿਸ਼ਾਨੀ wid_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ vwid_;
};

/*
 * This part select is more flexible in that it takes the vector to
 * part in port 0, and the base of the part in port 1. The width of
 * the part to take out is fixed.
 */
class vvp_fun_part_var  : ਜਨਤਕ vvp_net_fun_t {

    ਜਨਤਕ:
      explicit vvp_fun_part_var(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, bool is_signed);
      ~vvp_fun_part_var() override;

    ਸੁਰੱਖਿਅਤ:
      bool recv_vec4_(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                      ਪੂਰਨ&ਅਧਾਰ, vvp_vector4_t&ਸਰੋਤ,
                      vvp_vector4_t&ref);

      ਬਿਨਾਨਿਸ਼ਾਨੀ wid_;
      bool is_signed_;
};

/*
 * Statically allocated vvp_fun_part_var.
 */
class vvp_fun_part_var_sa  : ਜਨਤਕ vvp_fun_part_var {

    ਜਨਤਕ:
      explicit vvp_fun_part_var_sa(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, bool is_signed);
      ~vvp_fun_part_var_sa() override;

    ਜਨਤਕ:
      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t) override;

    ਨਿਜੀ:
      ਪੂਰਨ ਅਧਾਰ_2;
      vvp_vector4_t ਸਰੋਤ_2;
	// Save the last output, for detecting change.
      vvp_vector4_t ref_;
};

/*
 * Automatically allocated vvp_fun_part_var.
 */
class vvp_fun_part_var_aa  : ਜਨਤਕ vvp_fun_part_var, ਜਨਤਕ automatic_hooks_s {

    ਜਨਤਕ:
      explicit vvp_fun_part_var_aa(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, bool is_signed);
      ~vvp_fun_part_var_aa() override;

    ਜਨਤਕ:
      ਖਾਲੀ alloc_instance(vvp_context_t context) override;
      ਖਾਲੀ reset_instance(vvp_context_t context) override;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਖਾਲੀ free_instance(vvp_context_t context) override;
#ਜੇ_ਅੰਤ

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t context) override;

      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t context) override;

    ਨਿਜੀ:
      __vpiScope*context_scope_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ context_idx_;
};

#ਜੇ_ਅੰਤ /* IVL_part_H */
