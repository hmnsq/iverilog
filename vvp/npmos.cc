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

vvp_fun_pmos_::vvp_fun_pmos_(తార్కిక enable_invert, తార్కిక resistive)
{
      inv_en_ = enable_invert;
      resistive_ = resistive;
}


శూన్య vvp_fun_pmos_::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                              vvp_context_t)
{
	/* Data input is processed through eh recv_vec8 method,
	   because the strength must be preserved. */
      ఒకవేళ (ptr.పోర్టు() == 0) {
	    vvp_vector8_t tmp = vvp_vector8_t(బిట్టు,6,6);
	    recv_vec8(ptr, tmp);
	    తిరిగి;
      }

      ఒకవేళ (ptr.పోర్టు() != 1)
	    తిరిగి;

      en_ = inv_en_? ~బిట్టు : బిట్టు;
      generate_output_(ptr);
}

శూన్య vvp_fun_pmos_::recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
			         గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, బిట్టు, పునాది, vwid, ctx);
}

శూన్య vvp_fun_pmos_::recv_vec8_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector8_t&బిట్టు,
			         గుర్తులేని పునాది, గుర్తులేని vwid)
{
      recv_vec8_pv_(ptr, బిట్టు, పునాది, vwid);
}

శూన్య vvp_fun_pmos_::recv_real(vvp_net_ptr_t ptr, ద్విగుణ real,
                              vvp_context_t ctx)
{
      recv_vec4(ptr, double_గమ్యం_vector4_LSB(real), ctx);
}

శూన్య vvp_fun_pmos_::generate_output_(vvp_net_ptr_t ptr)
{
      స్థిరరాశి గుర్తులేని*strength_map = vvp_switch_strength_map[resistive_];

      vvp_vector8_t out (బిట్టు_2.size());

      కొరకు (గుర్తులేని idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_bit4_t   b_en  = en_.value(idx);
	    vvp_scalar_t b_బిట్టు = బిట్టు_2.value(idx);

            b_బిట్టు = vvp_scalar_t(b_బిట్టు.value(),
                                 strength_map[b_బిట్టు.strength0()],
                                 strength_map[b_బిట్టు.strength1()]);

	    ఎంపిక (b_en) {
		స్థితి BIT4_0:
		  out.సమితి_బిట్టు(idx, b_బిట్టు);
		  ఆపు;
		స్థితి BIT4_1:
		  out.సమితి_బిట్టు(idx, vvp_scalar_t(BIT4_Z,0,0));
		  ఆపు;
		అప్రమేయ:
		  ఎంపిక (b_బిట్టు.value()) {
		      స్థితి BIT4_0:
			b_బిట్టు = vvp_scalar_t(BIT4_X,b_బిట్టు.strength0(),0);
			ఆపు;
		      స్థితి BIT4_1:
			b_బిట్టు = vvp_scalar_t(BIT4_X,0,b_బిట్టు.strength1());
			ఆపు;
		      అప్రమేయ:
			ఆపు;
		  }
		  out.సమితి_బిట్టు(idx, b_బిట్టు);
		  ఆపు;
	    }
      }

      ఒకవేళ (out.size() > 0)
	    ptr.ptr()->send_vec8(out);
}


vvp_fun_pmos::vvp_fun_pmos(తార్కిక enable_invert)
: vvp_fun_pmos_(enable_invert, అబద్ధం)
{
}

శూన్య vvp_fun_pmos::recv_vec8(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector8_t&బిట్టు)
{
      ఒకవేళ (ptr.పోర్టు() == 1) {
	    recv_vec4(ptr, reduce4(బిట్టు), 0);
	    తిరిగి;
      }

      ఒకవేళ (ptr.పోర్టు() != 0)
	    తిరిగి;

      బిట్టు_2 = బిట్టు;
      generate_output_(ptr);
}

vvp_fun_rpmos::vvp_fun_rpmos(తార్కిక enable_invert)
: vvp_fun_pmos_(enable_invert, నిజం)
{
}

శూన్య vvp_fun_rpmos::recv_vec8(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector8_t&బిట్టు)
{
      ఒకవేళ (ptr.పోర్టు() == 1) {
	    recv_vec4(ptr, reduce4(బిట్టు), 0);
	    తిరిగి;
      }

      ఒకవేళ (ptr.పోర్టు() != 0)
	    తిరిగి;

      బిట్టు_2 = బిట్టు;
      generate_output_(ptr);
}


/*
 * CMOS primitive.
 */

vvp_fun_cmos_::vvp_fun_cmos_(తార్కిక resistive)
{
      resistive_ = resistive;
}

శూన్య vvp_fun_cmos_::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t &బిట్టు,
                              vvp_context_t)
{
	/* Data input is processed through the recv_vec8 method,
	   because the strength must be preserved. */
      ఒకవేళ (ptr.పోర్టు() == 0) {
	    vvp_vector8_t tmp = vvp_vector8_t(బిట్టు,6,6);
	    recv_vec8(ptr, tmp);
	    తిరిగి;
      }

      ఒకవేళ (ptr.పోర్టు() != 1 && ptr.పోర్టు() != 2)
	    తిరిగి;

      ఒకవేళ (ptr.పోర్టు() == 1)
	    n_en_ = బిట్టు;
      లేకపోతే
	    p_en_ = బిట్టు;
      generate_output_(ptr);
}

శూన్య vvp_fun_cmos_::recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
			         గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, బిట్టు, పునాది, vwid, ctx);
}

శూన్య vvp_fun_cmos_::recv_vec8_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector8_t&బిట్టు,
			         గుర్తులేని పునాది, గుర్తులేని vwid)
{
      recv_vec8_pv_(ptr, బిట్టు, పునాది, vwid);
}

శూన్య vvp_fun_cmos_::recv_real(vvp_net_ptr_t ptr, ద్విగుణ real,
                              vvp_context_t ctx)
{
      recv_vec4(ptr, double_గమ్యం_vector4_LSB(real), ctx);
}

శూన్య vvp_fun_cmos_::generate_output_(vvp_net_ptr_t ptr)
{
      స్థిరరాశి గుర్తులేని*strength_map = vvp_switch_strength_map[resistive_];

      vvp_vector8_t out (బిట్టు_2.size());

      కొరకు (గుర్తులేని idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_bit4_t   b_n_en  = n_en_.value(idx);
	    vvp_bit4_t   b_p_en  = p_en_.value(idx);
	    vvp_scalar_t b_బిట్టు = బిట్టు_2.value(idx);

            b_బిట్టు = vvp_scalar_t(b_బిట్టు.value(),
                                 strength_map[b_బిట్టు.strength0()],
                                 strength_map[b_బిట్టు.strength1()]);

	    ఒకవేళ (b_n_en == BIT4_1 || b_p_en == BIT4_0) {
		  out.సమితి_బిట్టు(idx, b_బిట్టు);
	    } లేకపోతే ఒకవేళ (b_n_en == BIT4_0 && b_p_en == BIT4_1) {
		  out.సమితి_బిట్టు(idx, vvp_scalar_t(BIT4_Z,0,0));
	    } లేకపోతే {
		  ఎంపిక (b_బిట్టు.value()) {
		      స్థితి BIT4_0:
			b_బిట్టు = vvp_scalar_t(BIT4_X,b_బిట్టు.strength0(),0);
			ఆపు;
		      స్థితి BIT4_1:
			b_బిట్టు = vvp_scalar_t(BIT4_X,0,b_బిట్టు.strength1());
			ఆపు;
		      అప్రమేయ:
			ఆపు;
		  }
		  out.సమితి_బిట్టు(idx, b_బిట్టు);
	    }
      }

      ఒకవేళ (out.size() > 0)
	    ptr.ptr()->send_vec8(out);
}

vvp_fun_cmos::vvp_fun_cmos()
: vvp_fun_cmos_(అబద్ధం)
{
}

శూన్య vvp_fun_cmos::recv_vec8(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector8_t&బిట్టు)
{
      ఒకవేళ (ptr.పోర్టు() == 1 || ptr.పోర్టు() == 2) {
	    recv_vec4(ptr, reduce4(బిట్టు), 0);
	    తిరిగి;
      }

      ఒకవేళ (ptr.పోర్టు() != 0)
	    తిరిగి;

      బిట్టు_2 = బిట్టు;
      generate_output_(ptr);
}

vvp_fun_rcmos::vvp_fun_rcmos()
: vvp_fun_cmos_(నిజం)
{
}

శూన్య vvp_fun_rcmos::recv_vec8(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector8_t&బిట్టు)
{
      ఒకవేళ (ptr.పోర్టు() == 1) {
	    recv_vec4(ptr, reduce4(బిట్టు), 0);
	    తిరిగి;
      }

      ఒకవేళ (ptr.పోర్టు() != 0)
	    తిరిగి;

      బిట్టు_2 = బిట్టు;
      generate_output_(ptr);
}
