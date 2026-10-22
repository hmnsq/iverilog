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

# include  "npmos.h"

vvp_fun_pmos_::vvp_fun_pmos_(منطقي enable_invert, منطقي resistive)
{
      inv_en_ = enable_invert;
      resistive_ = resistive;
}


فراغ vvp_fun_pmos_::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                              vvp_context_t)
{
	/* Data input is processed through eh recv_vec8 method,
	   because the strength must be preserved. */
      إذا (ptr.پورت() == 0) {
	    vvp_vector8_t tmp = vvp_vector8_t(بیت,6,6);
	    recv_vec8(ptr, tmp);
	    أرجع;
      }

      إذا (ptr.پورت() != 1)
	    أرجع;

      en_ = inv_en_? ~بیت : بیت;
      generate_output_(ptr);
}

فراغ vvp_fun_pmos_::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
			         بیعلامت پایه, بیعلامت vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, بیت, پایه, vwid, ctx);
}

فراغ vvp_fun_pmos_::recv_vec8_pv(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&بیت,
			         بیعلامت پایه, بیعلامت vwid)
{
      recv_vec8_pv_(ptr, بیت, پایه, vwid);
}

فراغ vvp_fun_pmos_::recv_real(vvp_net_ptr_t ptr, مزدوج real,
                              vvp_context_t ctx)
{
      recv_vec4(ptr, double_مقصد_vector4_LSB(real), ctx);
}

فراغ vvp_fun_pmos_::generate_output_(vvp_net_ptr_t ptr)
{
      ثابت بیعلامت*strength_map = vvp_switch_strength_map[resistive_];

      vvp_vector8_t out (بیت_2.size());

      لكل (بیعلامت idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_bit4_t   b_en  = en_.value(idx);
	    vvp_scalar_t b_بیت = بیت_2.value(idx);

            b_بیت = vvp_scalar_t(b_بیت.value(),
                                 strength_map[b_بیت.strength0()],
                                 strength_map[b_بیت.strength1()]);

	    بدل (b_en) {
		حالة BIT4_0:
		  out.مجموعه_بیت(idx, b_بیت);
		  اكسر;
		حالة BIT4_1:
		  out.مجموعه_بیت(idx, vvp_scalar_t(BIT4_Z,0,0));
		  اكسر;
		افتراضي:
		  بدل (b_بیت.value()) {
		      حالة BIT4_0:
			b_بیت = vvp_scalar_t(BIT4_X,b_بیت.strength0(),0);
			اكسر;
		      حالة BIT4_1:
			b_بیت = vvp_scalar_t(BIT4_X,0,b_بیت.strength1());
			اكسر;
		      افتراضي:
			اكسر;
		  }
		  out.مجموعه_بیت(idx, b_بیت);
		  اكسر;
	    }
      }

      إذا (out.size() > 0)
	    ptr.ptr()->send_vec8(out);
}


vvp_fun_pmos::vvp_fun_pmos(منطقي enable_invert)
: vvp_fun_pmos_(enable_invert, خطأ)
{
}

فراغ vvp_fun_pmos::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&بیت)
{
      إذا (ptr.پورت() == 1) {
	    recv_vec4(ptr, reduce4(بیت), 0);
	    أرجع;
      }

      إذا (ptr.پورت() != 0)
	    أرجع;

      بیت_2 = بیت;
      generate_output_(ptr);
}

vvp_fun_rpmos::vvp_fun_rpmos(منطقي enable_invert)
: vvp_fun_pmos_(enable_invert, درست)
{
}

فراغ vvp_fun_rpmos::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&بیت)
{
      إذا (ptr.پورت() == 1) {
	    recv_vec4(ptr, reduce4(بیت), 0);
	    أرجع;
      }

      إذا (ptr.پورت() != 0)
	    أرجع;

      بیت_2 = بیت;
      generate_output_(ptr);
}


/*
 * CMOS primitive.
 */

vvp_fun_cmos_::vvp_fun_cmos_(منطقي resistive)
{
      resistive_ = resistive;
}

فراغ vvp_fun_cmos_::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t &بیت,
                              vvp_context_t)
{
	/* Data input is processed through the recv_vec8 method,
	   because the strength must be preserved. */
      إذا (ptr.پورت() == 0) {
	    vvp_vector8_t tmp = vvp_vector8_t(بیت,6,6);
	    recv_vec8(ptr, tmp);
	    أرجع;
      }

      إذا (ptr.پورت() != 1 && ptr.پورت() != 2)
	    أرجع;

      إذا (ptr.پورت() == 1)
	    n_en_ = بیت;
      وإلا
	    p_en_ = بیت;
      generate_output_(ptr);
}

فراغ vvp_fun_cmos_::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
			         بیعلامت پایه, بیعلامت vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, بیت, پایه, vwid, ctx);
}

فراغ vvp_fun_cmos_::recv_vec8_pv(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&بیت,
			         بیعلامت پایه, بیعلامت vwid)
{
      recv_vec8_pv_(ptr, بیت, پایه, vwid);
}

فراغ vvp_fun_cmos_::recv_real(vvp_net_ptr_t ptr, مزدوج real,
                              vvp_context_t ctx)
{
      recv_vec4(ptr, double_مقصد_vector4_LSB(real), ctx);
}

فراغ vvp_fun_cmos_::generate_output_(vvp_net_ptr_t ptr)
{
      ثابت بیعلامت*strength_map = vvp_switch_strength_map[resistive_];

      vvp_vector8_t out (بیت_2.size());

      لكل (بیعلامت idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_bit4_t   b_n_en  = n_en_.value(idx);
	    vvp_bit4_t   b_p_en  = p_en_.value(idx);
	    vvp_scalar_t b_بیت = بیت_2.value(idx);

            b_بیت = vvp_scalar_t(b_بیت.value(),
                                 strength_map[b_بیت.strength0()],
                                 strength_map[b_بیت.strength1()]);

	    إذا (b_n_en == BIT4_1 || b_p_en == BIT4_0) {
		  out.مجموعه_بیت(idx, b_بیت);
	    } وإلا إذا (b_n_en == BIT4_0 && b_p_en == BIT4_1) {
		  out.مجموعه_بیت(idx, vvp_scalar_t(BIT4_Z,0,0));
	    } وإلا {
		  بدل (b_بیت.value()) {
		      حالة BIT4_0:
			b_بیت = vvp_scalar_t(BIT4_X,b_بیت.strength0(),0);
			اكسر;
		      حالة BIT4_1:
			b_بیت = vvp_scalar_t(BIT4_X,0,b_بیت.strength1());
			اكسر;
		      افتراضي:
			اكسر;
		  }
		  out.مجموعه_بیت(idx, b_بیت);
	    }
      }

      إذا (out.size() > 0)
	    ptr.ptr()->send_vec8(out);
}

vvp_fun_cmos::vvp_fun_cmos()
: vvp_fun_cmos_(خطأ)
{
}

فراغ vvp_fun_cmos::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&بیت)
{
      إذا (ptr.پورت() == 1 || ptr.پورت() == 2) {
	    recv_vec4(ptr, reduce4(بیت), 0);
	    أرجع;
      }

      إذا (ptr.پورت() != 0)
	    أرجع;

      بیت_2 = بیت;
      generate_output_(ptr);
}

vvp_fun_rcmos::vvp_fun_rcmos()
: vvp_fun_cmos_(درست)
{
}

فراغ vvp_fun_rcmos::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&بیت)
{
      إذا (ptr.پورت() == 1) {
	    recv_vec4(ptr, reduce4(بیت), 0);
	    أرجع;
      }

      إذا (ptr.پورت() != 0)
	    أرجع;

      بیت_2 = بیت;
      generate_output_(ptr);
}
