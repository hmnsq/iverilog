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

vvp_fun_bufif::vvp_fun_bufif(ਤਾਰਕਿਕ en_invert, ਤਾਰਕਿਕ ਬਾਹਰਲਾ_invert,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ str0, ਬਿਨਾਨਿਸ਼ਾਨੀ str1)
: pol_(en_invert? 1 : 0), inv_(ਬਾਹਰਲਾ_invert? 1 : 0)
{
      drive0_ = str0;
      drive1_ = str1;
      ਗਿਣਤੀ_functors_bufif += 1;
}

ਖਾਲੀ vvp_fun_bufif::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                              vvp_context_t)
{
      ਚੋਣ (ptr.ਪੋਰਟ()) {
	  ਹਾਲਤ 0:
	    ਦੋਆਧਾਰੀ_ਅੰਕ_2 = inv_? ~ਦੋਆਧਾਰੀ_ਅੰਕ : ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;
	  ਹਾਲਤ 1:
	    en_ = pol_? ~ਦੋਆਧਾਰੀ_ਅੰਕ : ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;
	  ਮੂਲ:
	    ਵਾਪਸ;
      }

      vvp_vector8_t out (ਦੋਆਧਾਰੀ_ਅੰਕ.size());

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < ਦੋਆਧਾਰੀ_ਅੰਕ.size() ;  idx += 1) {
	    vvp_bit4_t b_en  = en_.value(idx);
	    vvp_bit4_t b_ਦੋਆਧਾਰੀ_ਅੰਕ = ਦੋਆਧਾਰੀ_ਅੰਕ_2.value(idx);

	    ਚੋਣ (b_en) {
		ਹਾਲਤ BIT4_0:
		  out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, vvp_scalar_t(BIT4_Z,drive0_,drive1_));
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_1:
		  ਜੇ (bit4_is_xz(b_ਦੋਆਧਾਰੀ_ਅੰਕ))
			out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, vvp_scalar_t(BIT4_X,drive0_,drive1_));
		  ਨਹੀਂਤਾਂ
			out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, vvp_scalar_t(b_ਦੋਆਧਾਰੀ_ਅੰਕ,drive0_,drive1_));
		  ਰੋਕੋ;

		ਮੂਲ:
		  ਚੋਣ (b_ਦੋਆਧਾਰੀ_ਅੰਕ) {
		      ਹਾਲਤ BIT4_0:
			out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, vvp_scalar_t(BIT4_X,drive0_,0));
			ਰੋਕੋ;
		      ਹਾਲਤ BIT4_1:
			out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, vvp_scalar_t(BIT4_X,0,drive1_));
			ਰੋਕੋ;
		      ਮੂਲ:
			out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, vvp_scalar_t(BIT4_X,drive0_,drive1_));
			ਰੋਕੋ;
		  }
		  ਰੋਕੋ;
	    }
      }

      ptr.ptr()->send_vec8(out);
}

ਖਾਲੀ vvp_fun_bufif::recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			         ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid, ctx);
}

ਖਾਲੀ vvp_fun_bufif::recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ real,
                              vvp_context_t ctx)
{
      recv_vec4(ptr, double_ਮੰਜ਼ਿਲ_vector4_LSB(real), ctx);
}
