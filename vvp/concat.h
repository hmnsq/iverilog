#جے_بنا_تعریف IVL_concat_H
#تعریف_کرو IVL_concat_H
/*
 * Copyright (c) 2004-2025 Stephen Williams (steve@icarus.com)
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

# include "vvp_net.h"

/* vvp_fun_concat
 * This node function creates vectors (vvp_vector4_t) from the
 * concatenation of the inputs. The inputs (4) may be vector or
 * vector8 objects, but they are reduced to vector4 values and
 * strength information lost.
 *
 * The expected widths of the input vectors must be given up front so
 * that the positions in the output vector (and also the size of the
 * output vector) can be worked out. The input vectors must match the
 * expected width.
 */
class vvp_fun_concat  : عام vvp_net_fun_t, محمي vvp_gen_event_s  {

    عام:
      vvp_fun_concat(بنانشانی w0, بنانشانی w1,
		     بنانشانی w2, بنانشانی w3);
      ~vvp_fun_concat() override;

      فراغ recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) final;

      فراغ recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			بنانشانی ادھار, بنانشانی vwid, vvp_context_t) final;
    خاص:
      فراغ لگاتار_لگاتار() final;

      بنانشانی wid_[4];
      vvp_vector4_t val_;
      vvp_net_t *net_ = nullptr;
};

class vvp_fun_concat8  : عام vvp_net_fun_t, محمي vvp_gen_event_s {

    عام:
      vvp_fun_concat8(بنانشانی w0, بنانشانی w1,
		     بنانشانی w2, بنانشانی w3);
      ~vvp_fun_concat8() override;

      فراغ recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) final;
      فراغ recv_vec8(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ) final;

      فراغ recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			بنانشانی ادھار, بنانشانی vwid, vvp_context_t) final;
      فراغ recv_vec8_pv(vvp_net_ptr_t p, ثابت vvp_vector8_t&ثنائی_ہندسہ,
			بنانشانی ادھار, بنانشانی vwid) final;

    خاص:
      فراغ لگاتار_لگاتار() final;

      بنانشانی wid_[4];
      vvp_vector8_t val_;
      vvp_net_t *net_ = nullptr;
};

/* vvp_fun_repeat
 * This node function create vectors by repeating the input. The width
 * is the width of the output vector, and the repeat is the number of
 * times to repeat the input. The width of the input vector is
 * implicit from these values.
 */
class vvp_fun_repeat  : عام vvp_net_fun_t {

    عام:
      vvp_fun_repeat(بنانشانی چوڑائی, بنانشانی repeat);
      ~vvp_fun_repeat() override;

      فراغ recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;
      فراغ recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			بنانشانی صحيح ادھار, بنانشانی صحيح vwid,
			vvp_context_t context) final;

    خاص:
      بنانشانی wid_;
      بنانشانی rep_;
};

#جے_انت
