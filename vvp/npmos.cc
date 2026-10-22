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

vvp_fun_pmos_::vvp_fun_pmos_(தருக்க enable_invert, தருக்க resistive)
{
      inv_en_ = enable_invert;
      resistive_ = resistive;
}


வெறுமை vvp_fun_pmos_::recv_vec4(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit,
                              vvp_context_t)
{
	/* Data input is processed through eh recv_vec8 method,
	   because the strength must be preserved. */
      எனில் (ptr.port() == 0) {
	    vvp_vector8_t tmp = vvp_vector8_t(bit,6,6);
	    recv_vec8(ptr, tmp);
	    திரும்பு;
      }

      எனில் (ptr.port() != 1)
	    திரும்பு;

      en_ = inv_en_? ~bit : bit;
      generate_output_(ptr);
}

வெறுமை vvp_fun_pmos_::recv_vec4_pv(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit,
			         unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

வெறுமை vvp_fun_pmos_::recv_vec8_pv(vvp_net_ptr_t ptr, மாறிலி vvp_vector8_t&bit,
			         unsigned base, unsigned vwid)
{
      recv_vec8_pv_(ptr, bit, base, vwid);
}

வெறுமை vvp_fun_pmos_::recv_real(vvp_net_ptr_t ptr, இரட்டை real,
                              vvp_context_t ctx)
{
      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

வெறுமை vvp_fun_pmos_::generate_output_(vvp_net_ptr_t ptr)
{
      மாறிலி unsigned*strength_map = vvp_switch_strength_map[resistive_];

      vvp_vector8_t out (bit_.size());

      க்கு (unsigned idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_bit4_t   b_en  = en_.value(idx);
	    vvp_scalar_t b_bit = bit_.value(idx);

            b_bit = vvp_scalar_t(b_bit.value(),
                                 strength_map[b_bit.strength0()],
                                 strength_map[b_bit.strength1()]);

	    தேர்வு (b_en) {
		நிலை BIT4_0:
		  out.set_bit(idx, b_bit);
		  நிறுத்து;
		நிலை BIT4_1:
		  out.set_bit(idx, vvp_scalar_t(BIT4_Z,0,0));
		  நிறுத்து;
		இயல்பு:
		  தேர்வு (b_bit.value()) {
		      நிலை BIT4_0:
			b_bit = vvp_scalar_t(BIT4_X,b_bit.strength0(),0);
			நிறுத்து;
		      நிலை BIT4_1:
			b_bit = vvp_scalar_t(BIT4_X,0,b_bit.strength1());
			நிறுத்து;
		      இயல்பு:
			நிறுத்து;
		  }
		  out.set_bit(idx, b_bit);
		  நிறுத்து;
	    }
      }

      எனில் (out.size() > 0)
	    ptr.ptr()->send_vec8(out);
}


vvp_fun_pmos::vvp_fun_pmos(தருக்க enable_invert)
: vvp_fun_pmos_(enable_invert, பொய்)
{
}

வெறுமை vvp_fun_pmos::recv_vec8(vvp_net_ptr_t ptr, மாறிலி vvp_vector8_t&bit)
{
      எனில் (ptr.port() == 1) {
	    recv_vec4(ptr, reduce4(bit), 0);
	    திரும்பு;
      }

      எனில் (ptr.port() != 0)
	    திரும்பு;

      bit_ = bit;
      generate_output_(ptr);
}

vvp_fun_rpmos::vvp_fun_rpmos(தருக்க enable_invert)
: vvp_fun_pmos_(enable_invert, உண்மை)
{
}

வெறுமை vvp_fun_rpmos::recv_vec8(vvp_net_ptr_t ptr, மாறிலி vvp_vector8_t&bit)
{
      எனில் (ptr.port() == 1) {
	    recv_vec4(ptr, reduce4(bit), 0);
	    திரும்பு;
      }

      எனில் (ptr.port() != 0)
	    திரும்பு;

      bit_ = bit;
      generate_output_(ptr);
}


/*
 * CMOS primitive.
 */

vvp_fun_cmos_::vvp_fun_cmos_(தருக்க resistive)
{
      resistive_ = resistive;
}

வெறுமை vvp_fun_cmos_::recv_vec4(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t &bit,
                              vvp_context_t)
{
	/* Data input is processed through the recv_vec8 method,
	   because the strength must be preserved. */
      எனில் (ptr.port() == 0) {
	    vvp_vector8_t tmp = vvp_vector8_t(bit,6,6);
	    recv_vec8(ptr, tmp);
	    திரும்பு;
      }

      எனில் (ptr.port() != 1 && ptr.port() != 2)
	    திரும்பு;

      எனில் (ptr.port() == 1)
	    n_en_ = bit;
      இல்லையெனில்
	    p_en_ = bit;
      generate_output_(ptr);
}

வெறுமை vvp_fun_cmos_::recv_vec4_pv(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit,
			         unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

வெறுமை vvp_fun_cmos_::recv_vec8_pv(vvp_net_ptr_t ptr, மாறிலி vvp_vector8_t&bit,
			         unsigned base, unsigned vwid)
{
      recv_vec8_pv_(ptr, bit, base, vwid);
}

வெறுமை vvp_fun_cmos_::recv_real(vvp_net_ptr_t ptr, இரட்டை real,
                              vvp_context_t ctx)
{
      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

வெறுமை vvp_fun_cmos_::generate_output_(vvp_net_ptr_t ptr)
{
      மாறிலி unsigned*strength_map = vvp_switch_strength_map[resistive_];

      vvp_vector8_t out (bit_.size());

      க்கு (unsigned idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_bit4_t   b_n_en  = n_en_.value(idx);
	    vvp_bit4_t   b_p_en  = p_en_.value(idx);
	    vvp_scalar_t b_bit = bit_.value(idx);

            b_bit = vvp_scalar_t(b_bit.value(),
                                 strength_map[b_bit.strength0()],
                                 strength_map[b_bit.strength1()]);

	    எனில் (b_n_en == BIT4_1 || b_p_en == BIT4_0) {
		  out.set_bit(idx, b_bit);
	    } இல்லையெனில் எனில் (b_n_en == BIT4_0 && b_p_en == BIT4_1) {
		  out.set_bit(idx, vvp_scalar_t(BIT4_Z,0,0));
	    } இல்லையெனில் {
		  தேர்வு (b_bit.value()) {
		      நிலை BIT4_0:
			b_bit = vvp_scalar_t(BIT4_X,b_bit.strength0(),0);
			நிறுத்து;
		      நிலை BIT4_1:
			b_bit = vvp_scalar_t(BIT4_X,0,b_bit.strength1());
			நிறுத்து;
		      இயல்பு:
			நிறுத்து;
		  }
		  out.set_bit(idx, b_bit);
	    }
      }

      எனில் (out.size() > 0)
	    ptr.ptr()->send_vec8(out);
}

vvp_fun_cmos::vvp_fun_cmos()
: vvp_fun_cmos_(பொய்)
{
}

வெறுமை vvp_fun_cmos::recv_vec8(vvp_net_ptr_t ptr, மாறிலி vvp_vector8_t&bit)
{
      எனில் (ptr.port() == 1 || ptr.port() == 2) {
	    recv_vec4(ptr, reduce4(bit), 0);
	    திரும்பு;
      }

      எனில் (ptr.port() != 0)
	    திரும்பு;

      bit_ = bit;
      generate_output_(ptr);
}

vvp_fun_rcmos::vvp_fun_rcmos()
: vvp_fun_cmos_(உண்மை)
{
}

வெறுமை vvp_fun_rcmos::recv_vec8(vvp_net_ptr_t ptr, மாறிலி vvp_vector8_t&bit)
{
      எனில் (ptr.port() == 1) {
	    recv_vec4(ptr, reduce4(bit), 0);
	    திரும்பு;
      }

      எனில் (ptr.port() != 0)
	    திரும்பு;

      bit_ = bit;
      generate_output_(ptr);
}
