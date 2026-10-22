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


فراغ vvp_fun_pmos_::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                              vvp_context_t)
{
	/* Data input is processed through eh recv_vec8 method,
	   because the strength must be preserved. */
      إذا (ptr.port() == 0) {
	    vvp_vector8_t tmp = vvp_vector8_t(bit,6,6);
	    recv_vec8(ptr, tmp);
	    أرجع;
      }

      إذا (ptr.port() != 1)
	    أرجع;

      en_ = inv_en_? ~bit : bit;
      generate_output_(ptr);
}

فراغ vvp_fun_pmos_::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
			         unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

فراغ vvp_fun_pmos_::recv_vec8_pv(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&bit,
			         unsigned base, unsigned vwid)
{
      recv_vec8_pv_(ptr, bit, base, vwid);
}

فراغ vvp_fun_pmos_::recv_real(vvp_net_ptr_t ptr, مزدوج real,
                              vvp_context_t ctx)
{
      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

فراغ vvp_fun_pmos_::generate_output_(vvp_net_ptr_t ptr)
{
      ثابت unsigned*strength_map = vvp_switch_strength_map[resistive_];

      vvp_vector8_t out (bit_.size());

      لكل (unsigned idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_bit4_t   b_en  = en_.value(idx);
	    vvp_scalar_t b_bit = bit_.value(idx);

            b_bit = vvp_scalar_t(b_bit.value(),
                                 strength_map[b_bit.strength0()],
                                 strength_map[b_bit.strength1()]);

	    بدل (b_en) {
		حالة BIT4_0:
		  out.set_bit(idx, b_bit);
		  اكسر;
		حالة BIT4_1:
		  out.set_bit(idx, vvp_scalar_t(BIT4_Z,0,0));
		  اكسر;
		افتراضي:
		  بدل (b_bit.value()) {
		      حالة BIT4_0:
			b_bit = vvp_scalar_t(BIT4_X,b_bit.strength0(),0);
			اكسر;
		      حالة BIT4_1:
			b_bit = vvp_scalar_t(BIT4_X,0,b_bit.strength1());
			اكسر;
		      افتراضي:
			اكسر;
		  }
		  out.set_bit(idx, b_bit);
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

فراغ vvp_fun_pmos::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&bit)
{
      إذا (ptr.port() == 1) {
	    recv_vec4(ptr, reduce4(bit), 0);
	    أرجع;
      }

      إذا (ptr.port() != 0)
	    أرجع;

      bit_ = bit;
      generate_output_(ptr);
}

vvp_fun_rpmos::vvp_fun_rpmos(منطقي enable_invert)
: vvp_fun_pmos_(enable_invert, true)
{
}

فراغ vvp_fun_rpmos::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&bit)
{
      إذا (ptr.port() == 1) {
	    recv_vec4(ptr, reduce4(bit), 0);
	    أرجع;
      }

      إذا (ptr.port() != 0)
	    أرجع;

      bit_ = bit;
      generate_output_(ptr);
}


/*
 * CMOS primitive.
 */

vvp_fun_cmos_::vvp_fun_cmos_(منطقي resistive)
{
      resistive_ = resistive;
}

فراغ vvp_fun_cmos_::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t &bit,
                              vvp_context_t)
{
	/* Data input is processed through the recv_vec8 method,
	   because the strength must be preserved. */
      إذا (ptr.port() == 0) {
	    vvp_vector8_t tmp = vvp_vector8_t(bit,6,6);
	    recv_vec8(ptr, tmp);
	    أرجع;
      }

      إذا (ptr.port() != 1 && ptr.port() != 2)
	    أرجع;

      إذا (ptr.port() == 1)
	    n_en_ = bit;
      وإلا
	    p_en_ = bit;
      generate_output_(ptr);
}

فراغ vvp_fun_cmos_::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
			         unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

فراغ vvp_fun_cmos_::recv_vec8_pv(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&bit,
			         unsigned base, unsigned vwid)
{
      recv_vec8_pv_(ptr, bit, base, vwid);
}

فراغ vvp_fun_cmos_::recv_real(vvp_net_ptr_t ptr, مزدوج real,
                              vvp_context_t ctx)
{
      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

فراغ vvp_fun_cmos_::generate_output_(vvp_net_ptr_t ptr)
{
      ثابت unsigned*strength_map = vvp_switch_strength_map[resistive_];

      vvp_vector8_t out (bit_.size());

      لكل (unsigned idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_bit4_t   b_n_en  = n_en_.value(idx);
	    vvp_bit4_t   b_p_en  = p_en_.value(idx);
	    vvp_scalar_t b_bit = bit_.value(idx);

            b_bit = vvp_scalar_t(b_bit.value(),
                                 strength_map[b_bit.strength0()],
                                 strength_map[b_bit.strength1()]);

	    إذا (b_n_en == BIT4_1 || b_p_en == BIT4_0) {
		  out.set_bit(idx, b_bit);
	    } وإلا إذا (b_n_en == BIT4_0 && b_p_en == BIT4_1) {
		  out.set_bit(idx, vvp_scalar_t(BIT4_Z,0,0));
	    } وإلا {
		  بدل (b_bit.value()) {
		      حالة BIT4_0:
			b_bit = vvp_scalar_t(BIT4_X,b_bit.strength0(),0);
			اكسر;
		      حالة BIT4_1:
			b_bit = vvp_scalar_t(BIT4_X,0,b_bit.strength1());
			اكسر;
		      افتراضي:
			اكسر;
		  }
		  out.set_bit(idx, b_bit);
	    }
      }

      إذا (out.size() > 0)
	    ptr.ptr()->send_vec8(out);
}

vvp_fun_cmos::vvp_fun_cmos()
: vvp_fun_cmos_(خطأ)
{
}

فراغ vvp_fun_cmos::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&bit)
{
      إذا (ptr.port() == 1 || ptr.port() == 2) {
	    recv_vec4(ptr, reduce4(bit), 0);
	    أرجع;
      }

      إذا (ptr.port() != 0)
	    أرجع;

      bit_ = bit;
      generate_output_(ptr);
}

vvp_fun_rcmos::vvp_fun_rcmos()
: vvp_fun_cmos_(true)
{
}

فراغ vvp_fun_rcmos::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&bit)
{
      إذا (ptr.port() == 1) {
	    recv_vec4(ptr, reduce4(bit), 0);
	    أرجع;
      }

      إذا (ptr.port() != 0)
	    أرجع;

      bit_ = bit;
      generate_output_(ptr);
}
