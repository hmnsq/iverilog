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

vvp_fun_pmos_::vvp_fun_pmos_(ತಾರ್ಕಿಕ enable_invert, ತಾರ್ಕಿಕ resistive)
{
      inv_en_ = enable_invert;
      resistive_ = resistive;
}


ಶೂನ್ಯ vvp_fun_pmos_::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                              vvp_context_t)
{
	/* Data input is processed through eh recv_vec8 method,
	   because the strength must be preserved. */
      ಆದರೆ (ptr.ಪೋರ್ಟ್() == 0) {
	    vvp_vector8_t tmp = vvp_vector8_t(ಬಿಟ್,6,6);
	    recv_vec8(ptr, tmp);
	    ಹಿಂತಿರುಗಿ;
      }

      ಆದರೆ (ptr.ಪೋರ್ಟ್() != 1)
	    ಹಿಂತಿರುಗಿ;

      en_ = inv_en_? ~ಬಿಟ್ : ಬಿಟ್;
      generate_output_(ptr);
}

ಶೂನ್ಯ vvp_fun_pmos_::recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			         ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ಬಿಟ್, ತಳ, vwid, ctx);
}

ಶೂನ್ಯ vvp_fun_pmos_::recv_vec8_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್,
			         ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid)
{
      recv_vec8_pv_(ptr, ಬಿಟ್, ತಳ, vwid);
}

ಶೂನ್ಯ vvp_fun_pmos_::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ real,
                              vvp_context_t ctx)
{
      recv_vec4(ptr, double_ಗಮ್ಯ_vector4_LSB(real), ctx);
}

ಶೂನ್ಯ vvp_fun_pmos_::generate_output_(vvp_net_ptr_t ptr)
{
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ*strength_map = vvp_switch_strength_map[resistive_];

      vvp_vector8_t out (ಬಿಟ್_2.size());

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_bit4_t   b_en  = en_.value(idx);
	    vvp_scalar_t b_ಬಿಟ್ = ಬಿಟ್_2.value(idx);

            b_ಬಿಟ್ = vvp_scalar_t(b_ಬಿಟ್.value(),
                                 strength_map[b_ಬಿಟ್.strength0()],
                                 strength_map[b_ಬಿಟ್.strength1()]);

	    ಆಯ್ಕೆ (b_en) {
		ಸ್ಥಿತಿ BIT4_0:
		  out.ಗಣ_ಬಿಟ್(idx, b_ಬಿಟ್);
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_1:
		  out.ಗಣ_ಬಿಟ್(idx, vvp_scalar_t(BIT4_Z,0,0));
		  ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		  ಆಯ್ಕೆ (b_ಬಿಟ್.value()) {
		      ಸ್ಥಿತಿ BIT4_0:
			b_ಬಿಟ್ = vvp_scalar_t(BIT4_X,b_ಬಿಟ್.strength0(),0);
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ BIT4_1:
			b_ಬಿಟ್ = vvp_scalar_t(BIT4_X,0,b_ಬಿಟ್.strength1());
			ನಿಲ್ಲಿಸು;
		      ಪೂರ್ವನಿಯೋಜಿತ:
			ನಿಲ್ಲಿಸು;
		  }
		  out.ಗಣ_ಬಿಟ್(idx, b_ಬಿಟ್);
		  ನಿಲ್ಲಿಸು;
	    }
      }

      ಆದರೆ (out.size() > 0)
	    ptr.ptr()->send_vec8(out);
}


vvp_fun_pmos::vvp_fun_pmos(ತಾರ್ಕಿಕ enable_invert)
: vvp_fun_pmos_(enable_invert, ಸುಳ್ಳು)
{
}

ಶೂನ್ಯ vvp_fun_pmos::recv_vec8(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್)
{
      ಆದರೆ (ptr.ಪೋರ್ಟ್() == 1) {
	    recv_vec4(ptr, reduce4(ಬಿಟ್), 0);
	    ಹಿಂತಿರುಗಿ;
      }

      ಆದರೆ (ptr.ಪೋರ್ಟ್() != 0)
	    ಹಿಂತಿರುಗಿ;

      ಬಿಟ್_2 = ಬಿಟ್;
      generate_output_(ptr);
}

vvp_fun_rpmos::vvp_fun_rpmos(ತಾರ್ಕಿಕ enable_invert)
: vvp_fun_pmos_(enable_invert, ನಿಜ)
{
}

ಶೂನ್ಯ vvp_fun_rpmos::recv_vec8(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್)
{
      ಆದರೆ (ptr.ಪೋರ್ಟ್() == 1) {
	    recv_vec4(ptr, reduce4(ಬಿಟ್), 0);
	    ಹಿಂತಿರುಗಿ;
      }

      ಆದರೆ (ptr.ಪೋರ್ಟ್() != 0)
	    ಹಿಂತಿರುಗಿ;

      ಬಿಟ್_2 = ಬಿಟ್;
      generate_output_(ptr);
}


/*
 * CMOS primitive.
 */

vvp_fun_cmos_::vvp_fun_cmos_(ತಾರ್ಕಿಕ resistive)
{
      resistive_ = resistive;
}

ಶೂನ್ಯ vvp_fun_cmos_::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t &ಬಿಟ್,
                              vvp_context_t)
{
	/* Data input is processed through the recv_vec8 method,
	   because the strength must be preserved. */
      ಆದರೆ (ptr.ಪೋರ್ಟ್() == 0) {
	    vvp_vector8_t tmp = vvp_vector8_t(ಬಿಟ್,6,6);
	    recv_vec8(ptr, tmp);
	    ಹಿಂತಿರುಗಿ;
      }

      ಆದರೆ (ptr.ಪೋರ್ಟ್() != 1 && ptr.ಪೋರ್ಟ್() != 2)
	    ಹಿಂತಿರುಗಿ;

      ಆದರೆ (ptr.ಪೋರ್ಟ್() == 1)
	    n_en_ = ಬಿಟ್;
      ಇಲ್ಲದಿದ್ದರೆ
	    p_en_ = ಬಿಟ್;
      generate_output_(ptr);
}

ಶೂನ್ಯ vvp_fun_cmos_::recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			         ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ಬಿಟ್, ತಳ, vwid, ctx);
}

ಶೂನ್ಯ vvp_fun_cmos_::recv_vec8_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್,
			         ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid)
{
      recv_vec8_pv_(ptr, ಬಿಟ್, ತಳ, vwid);
}

ಶೂನ್ಯ vvp_fun_cmos_::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ real,
                              vvp_context_t ctx)
{
      recv_vec4(ptr, double_ಗಮ್ಯ_vector4_LSB(real), ctx);
}

ಶೂನ್ಯ vvp_fun_cmos_::generate_output_(vvp_net_ptr_t ptr)
{
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ*strength_map = vvp_switch_strength_map[resistive_];

      vvp_vector8_t out (ಬಿಟ್_2.size());

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_bit4_t   b_n_en  = n_en_.value(idx);
	    vvp_bit4_t   b_p_en  = p_en_.value(idx);
	    vvp_scalar_t b_ಬಿಟ್ = ಬಿಟ್_2.value(idx);

            b_ಬಿಟ್ = vvp_scalar_t(b_ಬಿಟ್.value(),
                                 strength_map[b_ಬಿಟ್.strength0()],
                                 strength_map[b_ಬಿಟ್.strength1()]);

	    ಆದರೆ (b_n_en == BIT4_1 || b_p_en == BIT4_0) {
		  out.ಗಣ_ಬಿಟ್(idx, b_ಬಿಟ್);
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (b_n_en == BIT4_0 && b_p_en == BIT4_1) {
		  out.ಗಣ_ಬಿಟ್(idx, vvp_scalar_t(BIT4_Z,0,0));
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ಆಯ್ಕೆ (b_ಬಿಟ್.value()) {
		      ಸ್ಥಿತಿ BIT4_0:
			b_ಬಿಟ್ = vvp_scalar_t(BIT4_X,b_ಬಿಟ್.strength0(),0);
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ BIT4_1:
			b_ಬಿಟ್ = vvp_scalar_t(BIT4_X,0,b_ಬಿಟ್.strength1());
			ನಿಲ್ಲಿಸು;
		      ಪೂರ್ವನಿಯೋಜಿತ:
			ನಿಲ್ಲಿಸು;
		  }
		  out.ಗಣ_ಬಿಟ್(idx, b_ಬಿಟ್);
	    }
      }

      ಆದರೆ (out.size() > 0)
	    ptr.ptr()->send_vec8(out);
}

vvp_fun_cmos::vvp_fun_cmos()
: vvp_fun_cmos_(ಸುಳ್ಳು)
{
}

ಶೂನ್ಯ vvp_fun_cmos::recv_vec8(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್)
{
      ಆದರೆ (ptr.ಪೋರ್ಟ್() == 1 || ptr.ಪೋರ್ಟ್() == 2) {
	    recv_vec4(ptr, reduce4(ಬಿಟ್), 0);
	    ಹಿಂತಿರುಗಿ;
      }

      ಆದರೆ (ptr.ಪೋರ್ಟ್() != 0)
	    ಹಿಂತಿರುಗಿ;

      ಬಿಟ್_2 = ಬಿಟ್;
      generate_output_(ptr);
}

vvp_fun_rcmos::vvp_fun_rcmos()
: vvp_fun_cmos_(ನಿಜ)
{
}

ಶೂನ್ಯ vvp_fun_rcmos::recv_vec8(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್)
{
      ಆದರೆ (ptr.ಪೋರ್ಟ್() == 1) {
	    recv_vec4(ptr, reduce4(ಬಿಟ್), 0);
	    ಹಿಂತಿರುಗಿ;
      }

      ಆದರೆ (ptr.ಪೋರ್ಟ್() != 0)
	    ಹಿಂತಿರುಗಿ;

      ಬಿಟ್_2 = ಬಿಟ್;
      generate_output_(ptr);
}
