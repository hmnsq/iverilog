#جے_بنا_تعریف IVL_part_H
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
class vvp_fun_part  : عام vvp_net_fun_t {

    عام:
      vvp_fun_part(بنانشانی ادھار, بنانشانی wid);
      ~vvp_fun_part() override;

      بنانشانی get_ادھار() ثابت { أرجع ادھار_2; }
      بنانشانی get_wid() ثابت { أرجع wid_; }

    محمي:
      بنانشانی ادھار_2;
      بنانشانی wid_;
};

/*
 * Statically allocated vvp_fun_part.
 */
class vvp_fun_part_sa  : عام vvp_fun_part, عام vvp_gen_event_s {

    عام:
      vvp_fun_part_sa(بنانشانی ادھار, بنانشانی wid);
      ~vvp_fun_part_sa() override;

    عام:
      فراغ recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;

      فراغ recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			بنانشانی ادھار, بنانشانی vwid, vvp_context_t) override;

    خاص:
      فراغ لگاتار_لگاتار() override;

    خاص:
      vvp_vector4_t val_;
      vvp_net_t*net_;
};

/*
 * Automatically allocated vvp_fun_part.
 */
class vvp_fun_part_aa  : عام vvp_fun_part, عام automatic_hooks_s {

    عام:
      vvp_fun_part_aa(بنانشانی ادھار, بنانشانی wid);
      ~vvp_fun_part_aa() override;

    عام:
      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#جے_انت

      فراغ recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;

      فراغ recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			بنانشانی ادھار, بنانشانی vwid, vvp_context_t context) override;

    خاص:
      __vpiScope*context_scope_;
      بنانشانی context_idx_;
};

/* vvp_fun_part_pv
 * This node takes a vector input and turns it into the part select of
 * a wider output network. It used the recv_vec4_pv methods of the
 * destination nodes to propagate the part select. It can be used in
 * both statically and automatically allocated scopes, as it has no
 * dynamic state.
 */
class vvp_fun_part_pv  : عام vvp_net_fun_t {

    عام:
      vvp_fun_part_pv(بنانشانی ادھار, بنانشانی wid, بنانشانی vec_wid);
      ~vvp_fun_part_pv() override;

    عام:
      فراغ recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;

      فراغ recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                        بنانشانی ادھار, بنانشانی vwid, vvp_context_t) override;

      فراغ recv_vec8(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ) override;

    خاص:
      بنانشانی ادھار_2;
      بنانشانی wid_;
      بنانشانی vwid_;
};

/*
 * This part select is more flexible in that it takes the vector to
 * part in port 0, and the base of the part in port 1. The width of
 * the part to take out is fixed.
 */
class vvp_fun_part_var  : عام vvp_net_fun_t {

    عام:
      explicit vvp_fun_part_var(بنانشانی wid, bool is_signed);
      ~vvp_fun_part_var() override;

    محمي:
      bool recv_vec4_(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                      صحيح&ادھار, vvp_vector4_t&سروت,
                      vvp_vector4_t&ref);

      بنانشانی wid_;
      bool is_signed_;
};

/*
 * Statically allocated vvp_fun_part_var.
 */
class vvp_fun_part_var_sa  : عام vvp_fun_part_var {

    عام:
      explicit vvp_fun_part_var_sa(بنانشانی wid, bool is_signed);
      ~vvp_fun_part_var_sa() override;

    عام:
      فراغ recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;

      فراغ recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			بنانشانی ادھار, بنانشانی vwid, vvp_context_t) override;

    خاص:
      صحيح ادھار_2;
      vvp_vector4_t سروت_2;
	// Save the last output, for detecting change.
      vvp_vector4_t ref_;
};

/*
 * Automatically allocated vvp_fun_part_var.
 */
class vvp_fun_part_var_aa  : عام vvp_fun_part_var, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_part_var_aa(بنانشانی wid, bool is_signed);
      ~vvp_fun_part_var_aa() override;

    عام:
      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#جے_انت

      فراغ recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;

      فراغ recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			بنانشانی ادھار, بنانشانی vwid, vvp_context_t context) override;

    خاص:
      __vpiScope*context_scope_;
      بنانشانی context_idx_;
};

#جے_انت /* IVL_part_H */
