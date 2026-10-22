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

vvp_fun_bufif::vvp_fun_bufif(തർക്കം en_invert, തർക്കം out_invert,
			     unsigned str0, unsigned str1)
: pol_(en_invert? 1 : 0), inv_(out_invert? 1 : 0)
{
      drive0_ = str0;
      drive1_ = str1;
      എണ്ണം_functors_bufif += 1;
}

ശൂന്യം vvp_fun_bufif::recv_vec4(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
                              vvp_context_t)
{
      തിരഞ്ഞെടുപ്പ് (ptr.port()) {
	  അവസ്ഥ 0:
	    bit_ = inv_? ~bit : bit;
	    നിർത്തുക;
	  അവസ്ഥ 1:
	    en_ = pol_? ~bit : bit;
	    നിർത്തുക;
	  സ്ഥിരസ്ഥിതി:
	    തിരികെ;
      }

      vvp_vector8_t out (bit.size());

      വേണ്ടി (unsigned idx = 0 ;  idx < bit.size() ;  idx += 1) {
	    vvp_bit4_t b_en  = en_.value(idx);
	    vvp_bit4_t b_bit = bit_.value(idx);

	    തിരഞ്ഞെടുപ്പ് (b_en) {
		അവസ്ഥ BIT4_0:
		  out.set_bit(idx, vvp_scalar_t(BIT4_Z,drive0_,drive1_));
		  നിർത്തുക;
		അവസ്ഥ BIT4_1:
		  എങ്കിൽ (bit4_is_xz(b_bit))
			out.set_bit(idx, vvp_scalar_t(BIT4_X,drive0_,drive1_));
		  അല്ലെങ്കിൽ
			out.set_bit(idx, vvp_scalar_t(b_bit,drive0_,drive1_));
		  നിർത്തുക;

		സ്ഥിരസ്ഥിതി:
		  തിരഞ്ഞെടുപ്പ് (b_bit) {
		      അവസ്ഥ BIT4_0:
			out.set_bit(idx, vvp_scalar_t(BIT4_X,drive0_,0));
			നിർത്തുക;
		      അവസ്ഥ BIT4_1:
			out.set_bit(idx, vvp_scalar_t(BIT4_X,0,drive1_));
			നിർത്തുക;
		      സ്ഥിരസ്ഥിതി:
			out.set_bit(idx, vvp_scalar_t(BIT4_X,drive0_,drive1_));
			നിർത്തുക;
		  }
		  നിർത്തുക;
	    }
      }

      ptr.ptr()->send_vec8(out);
}

ശൂന്യം vvp_fun_bufif::recv_vec4_pv(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
			         unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

ശൂന്യം vvp_fun_bufif::recv_real(vvp_net_ptr_t ptr, ഇരട്ട real,
                              vvp_context_t ctx)
{
      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}
