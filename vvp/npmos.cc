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

vvp_fun_pmos_::vvp_fun_pmos_(منطقی enable_invert, منطقی resistive)
{
      inv_en_ = enable_invert;
      resistive_ = resistive;
}


خالی vvp_fun_pmos_::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                              vvp_context_t)
{
	/* Data input is processed through eh recv_vec8 method,
	   because the strength must be preserved. */
      اگر (ptr.پورٹ() == 0) {
	    vvp_vector8_t tmp = vvp_vector8_t(ثنائی_ہندسہ,6,6);
	    recv_vec8(ptr, tmp);
	    واپس;
      }

      اگر (ptr.پورٹ() != 1)
	    واپس;

      en_ = inv_en_? ~ثنائی_ہندسہ : ثنائی_ہندسہ;
      generate_output_(ptr);
}

خالی vvp_fun_pmos_::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			         غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ثنائی_ہندسہ, بنیاد, vwid, ctx);
}

خالی vvp_fun_pmos_::recv_vec8_pv(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ثنائی_ہندسہ,
			         غیرعلامتی بنیاد, غیرعلامتی vwid)
{
      recv_vec8_pv_(ptr, ثنائی_ہندسہ, بنیاد, vwid);
}

خالی vvp_fun_pmos_::recv_real(vvp_net_ptr_t ptr, دوہرا real,
                              vvp_context_t ctx)
{
      recv_vec4(ptr, double_مقصد_vector4_LSB(real), ctx);
}

خالی vvp_fun_pmos_::generate_output_(vvp_net_ptr_t ptr)
{
      ثابت غیرعلامتی*strength_map = vvp_switch_strength_map[resistive_];

      vvp_vector8_t out (ثنائی_ہندسہ_2.size());

      برائے (غیرعلامتی idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_bit4_t   b_en  = en_.value(idx);
	    vvp_scalar_t b_ثنائی_ہندسہ = ثنائی_ہندسہ_2.value(idx);

            b_ثنائی_ہندسہ = vvp_scalar_t(b_ثنائی_ہندسہ.value(),
                                 strength_map[b_ثنائی_ہندسہ.strength0()],
                                 strength_map[b_ثنائی_ہندسہ.strength1()]);

	    انتخاب (b_en) {
		حالت BIT4_0:
		  out.سیٹ_ثنائی_ہندسہ(idx, b_ثنائی_ہندسہ);
		  توڑو;
		حالت BIT4_1:
		  out.سیٹ_ثنائی_ہندسہ(idx, vvp_scalar_t(BIT4_Z,0,0));
		  توڑو;
		طےشدہ:
		  انتخاب (b_ثنائی_ہندسہ.value()) {
		      حالت BIT4_0:
			b_ثنائی_ہندسہ = vvp_scalar_t(BIT4_X,b_ثنائی_ہندسہ.strength0(),0);
			توڑو;
		      حالت BIT4_1:
			b_ثنائی_ہندسہ = vvp_scalar_t(BIT4_X,0,b_ثنائی_ہندسہ.strength1());
			توڑو;
		      طےشدہ:
			توڑو;
		  }
		  out.سیٹ_ثنائی_ہندسہ(idx, b_ثنائی_ہندسہ);
		  توڑو;
	    }
      }

      اگر (out.size() > 0)
	    ptr.ptr()->send_vec8(out);
}


vvp_fun_pmos::vvp_fun_pmos(منطقی enable_invert)
: vvp_fun_pmos_(enable_invert, جھوٹ)
{
}

خالی vvp_fun_pmos::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      اگر (ptr.پورٹ() == 1) {
	    recv_vec4(ptr, reduce4(ثنائی_ہندسہ), 0);
	    واپس;
      }

      اگر (ptr.پورٹ() != 0)
	    واپس;

      ثنائی_ہندسہ_2 = ثنائی_ہندسہ;
      generate_output_(ptr);
}

vvp_fun_rpmos::vvp_fun_rpmos(منطقی enable_invert)
: vvp_fun_pmos_(enable_invert, سچ)
{
}

خالی vvp_fun_rpmos::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      اگر (ptr.پورٹ() == 1) {
	    recv_vec4(ptr, reduce4(ثنائی_ہندسہ), 0);
	    واپس;
      }

      اگر (ptr.پورٹ() != 0)
	    واپس;

      ثنائی_ہندسہ_2 = ثنائی_ہندسہ;
      generate_output_(ptr);
}


/*
 * CMOS primitive.
 */

vvp_fun_cmos_::vvp_fun_cmos_(منطقی resistive)
{
      resistive_ = resistive;
}

خالی vvp_fun_cmos_::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t &ثنائی_ہندسہ,
                              vvp_context_t)
{
	/* Data input is processed through the recv_vec8 method,
	   because the strength must be preserved. */
      اگر (ptr.پورٹ() == 0) {
	    vvp_vector8_t tmp = vvp_vector8_t(ثنائی_ہندسہ,6,6);
	    recv_vec8(ptr, tmp);
	    واپس;
      }

      اگر (ptr.پورٹ() != 1 && ptr.پورٹ() != 2)
	    واپس;

      اگر (ptr.پورٹ() == 1)
	    n_en_ = ثنائی_ہندسہ;
      ورنہ
	    p_en_ = ثنائی_ہندسہ;
      generate_output_(ptr);
}

خالی vvp_fun_cmos_::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			         غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ثنائی_ہندسہ, بنیاد, vwid, ctx);
}

خالی vvp_fun_cmos_::recv_vec8_pv(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ثنائی_ہندسہ,
			         غیرعلامتی بنیاد, غیرعلامتی vwid)
{
      recv_vec8_pv_(ptr, ثنائی_ہندسہ, بنیاد, vwid);
}

خالی vvp_fun_cmos_::recv_real(vvp_net_ptr_t ptr, دوہرا real,
                              vvp_context_t ctx)
{
      recv_vec4(ptr, double_مقصد_vector4_LSB(real), ctx);
}

خالی vvp_fun_cmos_::generate_output_(vvp_net_ptr_t ptr)
{
      ثابت غیرعلامتی*strength_map = vvp_switch_strength_map[resistive_];

      vvp_vector8_t out (ثنائی_ہندسہ_2.size());

      برائے (غیرعلامتی idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_bit4_t   b_n_en  = n_en_.value(idx);
	    vvp_bit4_t   b_p_en  = p_en_.value(idx);
	    vvp_scalar_t b_ثنائی_ہندسہ = ثنائی_ہندسہ_2.value(idx);

            b_ثنائی_ہندسہ = vvp_scalar_t(b_ثنائی_ہندسہ.value(),
                                 strength_map[b_ثنائی_ہندسہ.strength0()],
                                 strength_map[b_ثنائی_ہندسہ.strength1()]);

	    اگر (b_n_en == BIT4_1 || b_p_en == BIT4_0) {
		  out.سیٹ_ثنائی_ہندسہ(idx, b_ثنائی_ہندسہ);
	    } ورنہ اگر (b_n_en == BIT4_0 && b_p_en == BIT4_1) {
		  out.سیٹ_ثنائی_ہندسہ(idx, vvp_scalar_t(BIT4_Z,0,0));
	    } ورنہ {
		  انتخاب (b_ثنائی_ہندسہ.value()) {
		      حالت BIT4_0:
			b_ثنائی_ہندسہ = vvp_scalar_t(BIT4_X,b_ثنائی_ہندسہ.strength0(),0);
			توڑو;
		      حالت BIT4_1:
			b_ثنائی_ہندسہ = vvp_scalar_t(BIT4_X,0,b_ثنائی_ہندسہ.strength1());
			توڑو;
		      طےشدہ:
			توڑو;
		  }
		  out.سیٹ_ثنائی_ہندسہ(idx, b_ثنائی_ہندسہ);
	    }
      }

      اگر (out.size() > 0)
	    ptr.ptr()->send_vec8(out);
}

vvp_fun_cmos::vvp_fun_cmos()
: vvp_fun_cmos_(جھوٹ)
{
}

خالی vvp_fun_cmos::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      اگر (ptr.پورٹ() == 1 || ptr.پورٹ() == 2) {
	    recv_vec4(ptr, reduce4(ثنائی_ہندسہ), 0);
	    واپس;
      }

      اگر (ptr.پورٹ() != 0)
	    واپس;

      ثنائی_ہندسہ_2 = ثنائی_ہندسہ;
      generate_output_(ptr);
}

vvp_fun_rcmos::vvp_fun_rcmos()
: vvp_fun_cmos_(سچ)
{
}

خالی vvp_fun_rcmos::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      اگر (ptr.پورٹ() == 1) {
	    recv_vec4(ptr, reduce4(ثنائی_ہندسہ), 0);
	    واپس;
      }

      اگر (ptr.پورٹ() != 0)
	    واپس;

      ثنائی_ہندسہ_2 = ثنائی_ہندسہ;
      generate_output_(ptr);
}
