/*
 * Copyright (c) 2001-2025 Stephen Williams (steve@icarus.com)
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

# include  "bufif.h"
# include  "schedule.h"
# include  "statistics.h"
# include  <iostream>
# include  <cassert>

vvp_fun_bufif::vvp_fun_bufif(منطقي en_invert, منطقي باہرلا_invert,
			     بنانشانی str0, بنانشانی str1)
: pol_(en_invert? 1 : 0), inv_(باہرلا_invert? 1 : 0)
{
      drive0_ = str0;
      drive1_ = str1;
      گنتی_functors_bufif += 1;
}

فراغ vvp_fun_bufif::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                              vvp_context_t)
{
      بدل (ptr.پورٹ()) {
	  حالة 0:
	    ثنائی_ہندسہ_2 = inv_? ~ثنائی_ہندسہ : ثنائی_ہندسہ;
	    اكسر;
	  حالة 1:
	    en_ = pol_? ~ثنائی_ہندسہ : ثنائی_ہندسہ;
	    اكسر;
	  افتراضي:
	    أرجع;
      }

      vvp_vector8_t out (ثنائی_ہندسہ.size());

      لكل (بنانشانی idx = 0 ;  idx < ثنائی_ہندسہ.size() ;  idx += 1) {
	    vvp_bit4_t b_en  = en_.value(idx);
	    vvp_bit4_t b_ثنائی_ہندسہ = ثنائی_ہندسہ_2.value(idx);

	    بدل (b_en) {
		حالة BIT4_0:
		  out.سیٹ_ثنائی_ہندسہ(idx, vvp_scalar_t(BIT4_Z,drive0_,drive1_));
		  اكسر;
		حالة BIT4_1:
		  إذا (bit4_is_xz(b_ثنائی_ہندسہ))
			out.سیٹ_ثنائی_ہندسہ(idx, vvp_scalar_t(BIT4_X,drive0_,drive1_));
		  وإلا
			out.سیٹ_ثنائی_ہندسہ(idx, vvp_scalar_t(b_ثنائی_ہندسہ,drive0_,drive1_));
		  اكسر;

		افتراضي:
		  بدل (b_ثنائی_ہندسہ) {
		      حالة BIT4_0:
			out.سیٹ_ثنائی_ہندسہ(idx, vvp_scalar_t(BIT4_X,drive0_,0));
			اكسر;
		      حالة BIT4_1:
			out.سیٹ_ثنائی_ہندسہ(idx, vvp_scalar_t(BIT4_X,0,drive1_));
			اكسر;
		      افتراضي:
			out.سیٹ_ثنائی_ہندسہ(idx, vvp_scalar_t(BIT4_X,drive0_,drive1_));
			اكسر;
		  }
		  اكسر;
	    }
      }

      ptr.ptr()->send_vec8(out);
}

فراغ vvp_fun_bufif::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			         بنانشانی ادھار, بنانشانی vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ثنائی_ہندسہ, ادھار, vwid, ctx);
}

فراغ vvp_fun_bufif::recv_real(vvp_net_ptr_t ptr, مزدوج real,
                              vvp_context_t ctx)
{
      recv_vec4(ptr, double_منزل_vector4_LSB(real), ctx);
}
