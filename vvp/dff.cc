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

# include  "compile.h"
# include  "schedule.h"
# include  "dff.h"
# include  <climits>
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>
# include  <iostream>

/* We need to ensure an initial output value is propagated. This is
   achieved by setting asc_ to BIT4_Z to flag that we haven't yet
   propagated an output value. This will also disable clocked output.
   For flip-flops without an asynchronous set/clear, we schedule an
   initial value of BIT4_0 to be sent to port 3. For flip-flops with
   an asynchronous set/clear, we rely on the network propagating an
   initial value to port 3. The first value received on port 3 will
   either propagate the set/clear value (if the received value is
   BIT4_1) or will propagate an initial value of 'bx. From then on
   the flip-flop operates normally. */

vvp_dff::vvp_dff(గుర్తులేని width, తార్కిక negedge)
: clk_(BIT4_X), ena_(BIT4_X), asc_(BIT4_Z), d_(width, BIT4_X)
{
      clk_చురుకైన = negedge ? BIT4_0 : BIT4_1;
}

vvp_dff::~vvp_dff()
{
}

vvp_dff_aclr::vvp_dff_aclr(గుర్తులేని width, తార్కిక negedge)
: vvp_dff(width, negedge)
{
}

vvp_dff_aset::vvp_dff_aset(గుర్తులేని width, తార్కిక negedge)
: vvp_dff(width, negedge)
{
}

vvp_dff_asc::vvp_dff_asc(గుర్తులేని width, తార్కిక negedge, స్థిరరాశి అక్షరం*asc_విలువ)
: vvp_dff(width, negedge), asc_విలువ_2(c4string_గమ్యం_vector4(asc_విలువ))
{
}

శూన్య vvp_dff::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                        vvp_context_t)
{
      vvp_bit4_t tmp;

      ఎంపిక (పోర్టు.పోర్టు()) {

	  స్థితి 0: // D
	    d_ = బిట్టు;
	    ఆపు;

	  స్థితి 1: // CLK
	    ప్రతిజ్ఞా(బిట్టు.size() == 1);
	    ఒకవేళ (asc_ != BIT4_0)
		  ఆపు;
	    ఒకవేళ (ena_ != BIT4_1)
		  ఆపు;
	    tmp = clk_;
	    clk_ = బిట్టు.value(0);
	    ఒకవేళ (clk_ == clk_చురుకైన && tmp != clk_చురుకైన)
		  కాలపట్టిక_propagate_సదిశ(పోర్టు.ptr(), 0, d_);
	    ఆపు;

	  స్థితి 2: // CE
	    ప్రతిజ్ఞా(బిట్టు.size() == 1);
	    ena_ = బిట్టు.value(0);
	    ఆపు;

	  స్థితి 3: // asynch SET/CLR
	    ప్రతిజ్ఞా(బిట్టు.size() == 1);
	    tmp = asc_;
	    asc_ = బిట్టు.value(0);
	    ఒకవేళ (asc_ == BIT4_1 && tmp != BIT4_1)
		  recv_async(పోర్టు);
	    లేకపోతే ఒకవేళ (tmp == BIT4_Z)
		  పోర్టు.ptr()->send_vec4(vvp_vector4_t(d_.size(), BIT4_X), 0);
	    ఆపు;
      }
}

శూన్య vvp_dff::recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
			   గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, బిట్టు, పునాది, vwid, ctx);
}

/*
 * The recv_async functions respond to the asynchronous
 * set/clear input by propagating the desired output.
 *
 * NOTE: Don't touch the d_ value, because that tracks the D input,
 * which may be needed when the device is clocked afterwards.
 */
శూన్య vvp_dff::recv_async(vvp_net_ptr_t)
{
	// The base dff does not have an asynchronous set/clr input.
      ప్రతిజ్ఞా(0);
}

శూన్య vvp_dff_aclr::recv_async(vvp_net_ptr_t పోర్టు)
{
      కాలపట్టిక_propagate_సదిశ(పోర్టు.ptr(), 0, vvp_vector4_t(d_.size(), BIT4_0));
}

శూన్య vvp_dff_aset::recv_async(vvp_net_ptr_t పోర్టు)
{
      కాలపట్టిక_propagate_సదిశ(పోర్టు.ptr(), 0, vvp_vector4_t(d_.size(), BIT4_1));
}

శూన్య vvp_dff_asc::recv_async(vvp_net_ptr_t పోర్టు)
{
      కాలపట్టిక_propagate_సదిశ(పోర్టు.ptr(), 0, asc_విలువ_2);
}

శూన్య compile_dff(అక్షరం*label, గుర్తులేని width, తార్కిక negedge,
		 నిర్మాణం symb_s arg_d,
		 నిర్మాణం symb_s arg_c,
		 నిర్మాణం symb_s arg_e)
{
      vvp_net_t*ptr = కొత్త vvp_net_t;
      vvp_dff*fun = కొత్త vvp_dff(width, negedge);

      ptr->fun = fun;
      define_functor_చిహ్నం(label, ptr);
      విడుదల(label);
      ఆదానం_connect(ptr, 0, arg_d.పాఠ్యం);
      ఆదానం_connect(ptr, 1, arg_c.పాఠ్యం);
      ఆదానం_connect(ptr, 2, arg_e.పాఠ్యం);

      vvp_vector4_t init_val = vvp_vector4_t(1, BIT4_0);
      కాలపట్టిక_init_సదిశ(vvp_net_ptr_t(ptr,3), init_val);
}

శూన్య compile_dff_aclr(అక్షరం*label, గుర్తులేని width, తార్కిక negedge,
		      నిర్మాణం symb_s arg_d,
		      నిర్మాణం symb_s arg_c,
		      నిర్మాణం symb_s arg_e,
		      నిర్మాణం symb_s arg_a)
{
      vvp_net_t*ptr = కొత్త vvp_net_t;
      vvp_dff*fun = కొత్త vvp_dff_aclr(width, negedge);

      ptr->fun = fun;
      define_functor_చిహ్నం(label, ptr);
      విడుదల(label);
      ఆదానం_connect(ptr, 0, arg_d.పాఠ్యం);
      ఆదానం_connect(ptr, 1, arg_c.పాఠ్యం);
      ఆదానం_connect(ptr, 2, arg_e.పాఠ్యం);
      ఆదానం_connect(ptr, 3, arg_a.పాఠ్యం);
}

శూన్య compile_dff_aset(అక్షరం*label, గుర్తులేని width, తార్కిక negedge,
		      నిర్మాణం symb_s arg_d,
		      నిర్మాణం symb_s arg_c,
		      నిర్మాణం symb_s arg_e,
		      నిర్మాణం symb_s arg_a,
		      అక్షరం*asc_విలువ)
{
      vvp_net_t*ptr = కొత్త vvp_net_t;
      vvp_dff*fun;
      ఒకవేళ (asc_విలువ) {
	    ప్రతిజ్ఞా(c4string_test(asc_విలువ));
	    fun = కొత్త vvp_dff_asc(width, negedge, asc_విలువ);
	    విడుదల(asc_విలువ);
      } లేకపోతే {
	    fun = కొత్త vvp_dff_aset(width, negedge);
      }

      ptr->fun = fun;
      define_functor_చిహ్నం(label, ptr);
      విడుదల(label);
      ఆదానం_connect(ptr, 0, arg_d.పాఠ్యం);
      ఆదానం_connect(ptr, 1, arg_c.పాఠ్యం);
      ఆదానం_connect(ptr, 2, arg_e.పాఠ్యం);
      ఆదానం_connect(ptr, 3, arg_a.పాఠ్యం);
}
