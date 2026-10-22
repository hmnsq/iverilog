#اگر_غیر_تعریف_شدہ IVL_part_H
#تعریف_کرو IVL_part_H
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
class vvp_fun_part  : عوامی vvp_net_fun_t {

    عوامی:
      vvp_fun_part(غیرعلامتی بنیاد, غیرعلامتی wid);
      ~vvp_fun_part() override;

      غیرعلامتی get_بنیاد() ثابت { واپس بنیاد_2; }
      غیرعلامتی get_wid() ثابت { واپس wid_; }

    محفوظ:
      غیرعلامتی بنیاد_2;
      غیرعلامتی wid_;
};

/*
 * Statically allocated vvp_fun_part.
 */
class vvp_fun_part_sa  : عوامی vvp_fun_part, عوامی vvp_gen_event_s {

    عوامی:
      vvp_fun_part_sa(غیرعلامتی بنیاد, غیرعلامتی wid);
      ~vvp_fun_part_sa() override;

    عوامی:
      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;

      خالی recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t) override;

    نجی:
      خالی تسلسل_تسلسل() override;

    نجی:
      vvp_vector4_t val_;
      vvp_net_t*net_;
};

/*
 * Automatically allocated vvp_fun_part.
 */
class vvp_fun_part_aa  : عوامی vvp_fun_part, عوامی automatic_hooks_s {

    عوامی:
      vvp_fun_part_aa(غیرعلامتی بنیاد, غیرعلامتی wid);
      ~vvp_fun_part_aa() override;

    عوامی:
      خالی alloc_instance(vvp_context_t context) override;
      خالی reset_instance(vvp_context_t context) override;
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      خالی free_instance(vvp_context_t context) override;
#اگر_ختم

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;

      خالی recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t context) override;

    نجی:
      __vpiScope*context_scope_;
      غیرعلامتی context_idx_;
};

/* vvp_fun_part_pv
 * This node takes a vector input and turns it into the part select of
 * a wider output network. It used the recv_vec4_pv methods of the
 * destination nodes to propagate the part select. It can be used in
 * both statically and automatically allocated scopes, as it has no
 * dynamic state.
 */
class vvp_fun_part_pv  : عوامی vvp_net_fun_t {

    عوامی:
      vvp_fun_part_pv(غیرعلامتی بنیاد, غیرعلامتی wid, غیرعلامتی vec_wid);
      ~vvp_fun_part_pv() override;

    عوامی:
      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;

      خالی recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                        غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t) override;

      خالی recv_vec8(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ) override;

    نجی:
      غیرعلامتی بنیاد_2;
      غیرعلامتی wid_;
      غیرعلامتی vwid_;
};

/*
 * This part select is more flexible in that it takes the vector to
 * part in port 0, and the base of the part in port 1. The width of
 * the part to take out is fixed.
 */
class vvp_fun_part_var  : عوامی vvp_net_fun_t {

    عوامی:
      explicit vvp_fun_part_var(غیرعلامتی wid, bool is_signed);
      ~vvp_fun_part_var() override;

    محفوظ:
      bool recv_vec4_(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                      صحیح&بنیاد, vvp_vector4_t&ماخذ,
                      vvp_vector4_t&ref);

      غیرعلامتی wid_;
      bool is_signed_;
};

/*
 * Statically allocated vvp_fun_part_var.
 */
class vvp_fun_part_var_sa  : عوامی vvp_fun_part_var {

    عوامی:
      explicit vvp_fun_part_var_sa(غیرعلامتی wid, bool is_signed);
      ~vvp_fun_part_var_sa() override;

    عوامی:
      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;

      خالی recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t) override;

    نجی:
      صحیح بنیاد_2;
      vvp_vector4_t ماخذ_2;
	// Save the last output, for detecting change.
      vvp_vector4_t ref_;
};

/*
 * Automatically allocated vvp_fun_part_var.
 */
class vvp_fun_part_var_aa  : عوامی vvp_fun_part_var, عوامی automatic_hooks_s {

    عوامی:
      explicit vvp_fun_part_var_aa(غیرعلامتی wid, bool is_signed);
      ~vvp_fun_part_var_aa() override;

    عوامی:
      خالی alloc_instance(vvp_context_t context) override;
      خالی reset_instance(vvp_context_t context) override;
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      خالی free_instance(vvp_context_t context) override;
#اگر_ختم

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;

      خالی recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t context) override;

    نجی:
      __vpiScope*context_scope_;
      غیرعلامتی context_idx_;
};

#اگر_ختم /* IVL_part_H */
