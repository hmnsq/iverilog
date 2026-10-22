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

vvp_fun_pmos_::vvp_fun_pmos_(ਤਾਰਕਿਕ enable_invert, ਤਾਰਕਿਕ resistive)
{
      inv_en_ = enable_invert;
      resistive_ = resistive;
}


ਖਾਲੀ vvp_fun_pmos_::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                              vvp_context_t)
{
	/* Data input is processed through eh recv_vec8 method,
	   because the strength must be preserved. */
      ਜੇ (ptr.ਪੋਰਟ() == 0) {
	    vvp_vector8_t tmp = vvp_vector8_t(ਦੋਆਧਾਰੀ_ਅੰਕ,6,6);
	    recv_vec8(ptr, tmp);
	    ਵਾਪਸ;
      }

      ਜੇ (ptr.ਪੋਰਟ() != 1)
	    ਵਾਪਸ;

      en_ = inv_en_? ~ਦੋਆਧਾਰੀ_ਅੰਕ : ਦੋਆਧਾਰੀ_ਅੰਕ;
      generate_output_(ptr);
}

ਖਾਲੀ vvp_fun_pmos_::recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			         ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid, ctx);
}

ਖਾਲੀ vvp_fun_pmos_::recv_vec8_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			         ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid)
{
      recv_vec8_pv_(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid);
}

ਖਾਲੀ vvp_fun_pmos_::recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ real,
                              vvp_context_t ctx)
{
      recv_vec4(ptr, double_ਮੰਜ਼ਿਲ_vector4_LSB(real), ctx);
}

ਖਾਲੀ vvp_fun_pmos_::generate_output_(vvp_net_ptr_t ptr)
{
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ*strength_map = vvp_switch_strength_map[resistive_];

      vvp_vector8_t out (ਦੋਆਧਾਰੀ_ਅੰਕ_2.size());

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_bit4_t   b_en  = en_.value(idx);
	    vvp_scalar_t b_ਦੋਆਧਾਰੀ_ਅੰਕ = ਦੋਆਧਾਰੀ_ਅੰਕ_2.value(idx);

            b_ਦੋਆਧਾਰੀ_ਅੰਕ = vvp_scalar_t(b_ਦੋਆਧਾਰੀ_ਅੰਕ.value(),
                                 strength_map[b_ਦੋਆਧਾਰੀ_ਅੰਕ.strength0()],
                                 strength_map[b_ਦੋਆਧਾਰੀ_ਅੰਕ.strength1()]);

	    ਚੋਣ (b_en) {
		ਹਾਲਤ BIT4_0:
		  out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, b_ਦੋਆਧਾਰੀ_ਅੰਕ);
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_1:
		  out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, vvp_scalar_t(BIT4_Z,0,0));
		  ਰੋਕੋ;
		ਮੂਲ:
		  ਚੋਣ (b_ਦੋਆਧਾਰੀ_ਅੰਕ.value()) {
		      ਹਾਲਤ BIT4_0:
			b_ਦੋਆਧਾਰੀ_ਅੰਕ = vvp_scalar_t(BIT4_X,b_ਦੋਆਧਾਰੀ_ਅੰਕ.strength0(),0);
			ਰੋਕੋ;
		      ਹਾਲਤ BIT4_1:
			b_ਦੋਆਧਾਰੀ_ਅੰਕ = vvp_scalar_t(BIT4_X,0,b_ਦੋਆਧਾਰੀ_ਅੰਕ.strength1());
			ਰੋਕੋ;
		      ਮੂਲ:
			ਰੋਕੋ;
		  }
		  out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, b_ਦੋਆਧਾਰੀ_ਅੰਕ);
		  ਰੋਕੋ;
	    }
      }

      ਜੇ (out.size() > 0)
	    ptr.ptr()->send_vec8(out);
}


vvp_fun_pmos::vvp_fun_pmos(ਤਾਰਕਿਕ enable_invert)
: vvp_fun_pmos_(enable_invert, ਝੂਠ)
{
}

ਖਾਲੀ vvp_fun_pmos::recv_vec8(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਜੇ (ptr.ਪੋਰਟ() == 1) {
	    recv_vec4(ptr, reduce4(ਦੋਆਧਾਰੀ_ਅੰਕ), 0);
	    ਵਾਪਸ;
      }

      ਜੇ (ptr.ਪੋਰਟ() != 0)
	    ਵਾਪਸ;

      ਦੋਆਧਾਰੀ_ਅੰਕ_2 = ਦੋਆਧਾਰੀ_ਅੰਕ;
      generate_output_(ptr);
}

vvp_fun_rpmos::vvp_fun_rpmos(ਤਾਰਕਿਕ enable_invert)
: vvp_fun_pmos_(enable_invert, ਸੱਚ)
{
}

ਖਾਲੀ vvp_fun_rpmos::recv_vec8(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਜੇ (ptr.ਪੋਰਟ() == 1) {
	    recv_vec4(ptr, reduce4(ਦੋਆਧਾਰੀ_ਅੰਕ), 0);
	    ਵਾਪਸ;
      }

      ਜੇ (ptr.ਪੋਰਟ() != 0)
	    ਵਾਪਸ;

      ਦੋਆਧਾਰੀ_ਅੰਕ_2 = ਦੋਆਧਾਰੀ_ਅੰਕ;
      generate_output_(ptr);
}


/*
 * CMOS primitive.
 */

vvp_fun_cmos_::vvp_fun_cmos_(ਤਾਰਕਿਕ resistive)
{
      resistive_ = resistive;
}

ਖਾਲੀ vvp_fun_cmos_::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t &ਦੋਆਧਾਰੀ_ਅੰਕ,
                              vvp_context_t)
{
	/* Data input is processed through the recv_vec8 method,
	   because the strength must be preserved. */
      ਜੇ (ptr.ਪੋਰਟ() == 0) {
	    vvp_vector8_t tmp = vvp_vector8_t(ਦੋਆਧਾਰੀ_ਅੰਕ,6,6);
	    recv_vec8(ptr, tmp);
	    ਵਾਪਸ;
      }

      ਜੇ (ptr.ਪੋਰਟ() != 1 && ptr.ਪੋਰਟ() != 2)
	    ਵਾਪਸ;

      ਜੇ (ptr.ਪੋਰਟ() == 1)
	    n_en_ = ਦੋਆਧਾਰੀ_ਅੰਕ;
      ਨਹੀਂਤਾਂ
	    p_en_ = ਦੋਆਧਾਰੀ_ਅੰਕ;
      generate_output_(ptr);
}

ਖਾਲੀ vvp_fun_cmos_::recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			         ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid, ctx);
}

ਖਾਲੀ vvp_fun_cmos_::recv_vec8_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			         ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid)
{
      recv_vec8_pv_(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid);
}

ਖਾਲੀ vvp_fun_cmos_::recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ real,
                              vvp_context_t ctx)
{
      recv_vec4(ptr, double_ਮੰਜ਼ਿਲ_vector4_LSB(real), ctx);
}

ਖਾਲੀ vvp_fun_cmos_::generate_output_(vvp_net_ptr_t ptr)
{
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ*strength_map = vvp_switch_strength_map[resistive_];

      vvp_vector8_t out (ਦੋਆਧਾਰੀ_ਅੰਕ_2.size());

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_bit4_t   b_n_en  = n_en_.value(idx);
	    vvp_bit4_t   b_p_en  = p_en_.value(idx);
	    vvp_scalar_t b_ਦੋਆਧਾਰੀ_ਅੰਕ = ਦੋਆਧਾਰੀ_ਅੰਕ_2.value(idx);

            b_ਦੋਆਧਾਰੀ_ਅੰਕ = vvp_scalar_t(b_ਦੋਆਧਾਰੀ_ਅੰਕ.value(),
                                 strength_map[b_ਦੋਆਧਾਰੀ_ਅੰਕ.strength0()],
                                 strength_map[b_ਦੋਆਧਾਰੀ_ਅੰਕ.strength1()]);

	    ਜੇ (b_n_en == BIT4_1 || b_p_en == BIT4_0) {
		  out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, b_ਦੋਆਧਾਰੀ_ਅੰਕ);
	    } ਨਹੀਂਤਾਂ ਜੇ (b_n_en == BIT4_0 && b_p_en == BIT4_1) {
		  out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, vvp_scalar_t(BIT4_Z,0,0));
	    } ਨਹੀਂਤਾਂ {
		  ਚੋਣ (b_ਦੋਆਧਾਰੀ_ਅੰਕ.value()) {
		      ਹਾਲਤ BIT4_0:
			b_ਦੋਆਧਾਰੀ_ਅੰਕ = vvp_scalar_t(BIT4_X,b_ਦੋਆਧਾਰੀ_ਅੰਕ.strength0(),0);
			ਰੋਕੋ;
		      ਹਾਲਤ BIT4_1:
			b_ਦੋਆਧਾਰੀ_ਅੰਕ = vvp_scalar_t(BIT4_X,0,b_ਦੋਆਧਾਰੀ_ਅੰਕ.strength1());
			ਰੋਕੋ;
		      ਮੂਲ:
			ਰੋਕੋ;
		  }
		  out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, b_ਦੋਆਧਾਰੀ_ਅੰਕ);
	    }
      }

      ਜੇ (out.size() > 0)
	    ptr.ptr()->send_vec8(out);
}

vvp_fun_cmos::vvp_fun_cmos()
: vvp_fun_cmos_(ਝੂਠ)
{
}

ਖਾਲੀ vvp_fun_cmos::recv_vec8(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਜੇ (ptr.ਪੋਰਟ() == 1 || ptr.ਪੋਰਟ() == 2) {
	    recv_vec4(ptr, reduce4(ਦੋਆਧਾਰੀ_ਅੰਕ), 0);
	    ਵਾਪਸ;
      }

      ਜੇ (ptr.ਪੋਰਟ() != 0)
	    ਵਾਪਸ;

      ਦੋਆਧਾਰੀ_ਅੰਕ_2 = ਦੋਆਧਾਰੀ_ਅੰਕ;
      generate_output_(ptr);
}

vvp_fun_rcmos::vvp_fun_rcmos()
: vvp_fun_cmos_(ਸੱਚ)
{
}

ਖਾਲੀ vvp_fun_rcmos::recv_vec8(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਜੇ (ptr.ਪੋਰਟ() == 1) {
	    recv_vec4(ptr, reduce4(ਦੋਆਧਾਰੀ_ਅੰਕ), 0);
	    ਵਾਪਸ;
      }

      ਜੇ (ptr.ਪੋਰਟ() != 0)
	    ਵਾਪਸ;

      ਦੋਆਧਾਰੀ_ਅੰਕ_2 = ਦੋਆਧਾਰੀ_ਅੰਕ;
      generate_output_(ptr);
}
