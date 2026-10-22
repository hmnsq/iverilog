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

vvp_fun_bufif::vvp_fun_bufif(తార్కిక en_invert, తార్కిక బహిర్గతం_invert,
			     గుర్తులేని str0, గుర్తులేని str1)
: pol_(en_invert? 1 : 0), inv_(బహిర్గతం_invert? 1 : 0)
{
      drive0_ = str0;
      drive1_ = str1;
      లెక్క_functors_bufif += 1;
}

శూన్య vvp_fun_bufif::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                              vvp_context_t)
{
      ఎంపిక (ptr.పోర్టు()) {
	  స్థితి 0:
	    బిట్టు_2 = inv_? ~బిట్టు : బిట్టు;
	    ఆపు;
	  స్థితి 1:
	    en_ = pol_? ~బిట్టు : బిట్టు;
	    ఆపు;
	  అప్రమేయ:
	    తిరిగి;
      }

      vvp_vector8_t out (బిట్టు.size());

      కొరకు (గుర్తులేని idx = 0 ;  idx < బిట్టు.size() ;  idx += 1) {
	    vvp_bit4_t b_en  = en_.value(idx);
	    vvp_bit4_t b_బిట్టు = బిట్టు_2.value(idx);

	    ఎంపిక (b_en) {
		స్థితి BIT4_0:
		  out.సమితి_బిట్టు(idx, vvp_scalar_t(BIT4_Z,drive0_,drive1_));
		  ఆపు;
		స్థితి BIT4_1:
		  ఒకవేళ (bit4_is_xz(b_బిట్టు))
			out.సమితి_బిట్టు(idx, vvp_scalar_t(BIT4_X,drive0_,drive1_));
		  లేకపోతే
			out.సమితి_బిట్టు(idx, vvp_scalar_t(b_బిట్టు,drive0_,drive1_));
		  ఆపు;

		అప్రమేయ:
		  ఎంపిక (b_బిట్టు) {
		      స్థితి BIT4_0:
			out.సమితి_బిట్టు(idx, vvp_scalar_t(BIT4_X,drive0_,0));
			ఆపు;
		      స్థితి BIT4_1:
			out.సమితి_బిట్టు(idx, vvp_scalar_t(BIT4_X,0,drive1_));
			ఆపు;
		      అప్రమేయ:
			out.సమితి_బిట్టు(idx, vvp_scalar_t(BIT4_X,drive0_,drive1_));
			ఆపు;
		  }
		  ఆపు;
	    }
      }

      ptr.ptr()->send_vec8(out);
}

శూన్య vvp_fun_bufif::recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
			         గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, బిట్టు, పునాది, vwid, ctx);
}

శూన్య vvp_fun_bufif::recv_real(vvp_net_ptr_t ptr, ద్విగుణ real,
                              vvp_context_t ctx)
{
      recv_vec4(ptr, double_గమ్యం_vector4_LSB(real), ctx);
}
