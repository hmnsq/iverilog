/*
 * Copyright (c) 2016 Martin Whitaker (icarus@martin-whitaker.me.uk)
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
# include  "latch.h"
# include  <climits>
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>
# include  <iostream>

/* We need to ensure an initial output value is propagated. This is
   achieved by scheduling an initial value to be sent to port 3. Any
   value received on port 3 will propagate an initial value of 'bx. */

vvp_latch::vvp_latch(గుర్తులేని width)
: en_(BIT4_X), d_(width, BIT4_X)
{
}

vvp_latch::~vvp_latch()
{
}

శూన్య vvp_latch::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                          vvp_context_t)
{
      vvp_bit4_t tmp;

      ఎంపిక (పోర్టు.పోర్టు()) {

	  స్థితి 0: // D
	    d_ = బిట్టు;
	    ఒకవేళ (en_ == BIT4_1)
		  కాలపట్టిక_propagate_సదిశ(పోర్టు.ptr(), 0, d_);
	    ఆపు;

	  స్థితి 1: // EN
	    ప్రతిజ్ఞా(బిట్టు.size() == 1);
	    tmp = en_;
	    en_ = బిట్టు.value(0);
	    ఒకవేళ (en_ == BIT4_1 && tmp != BIT4_1)
		  కాలపట్టిక_propagate_సదిశ(పోర్టు.ptr(), 0, d_);
	    ఆపు;

	  స్థితి 2:
	    ప్రతిజ్ఞా(0);
	    ఆపు;

	  స్థితి 3:
	    పోర్టు.ptr()->send_vec4(vvp_vector4_t(d_.size(), BIT4_X), 0);
	    ఆపు;
      }
}

శూన్య vvp_latch::recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
			     గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, బిట్టు, పునాది, vwid, ctx);
}

శూన్య compile_latch(అక్షరం*label, గుర్తులేని width,
		   నిర్మాణం symb_s arg_d,
		   నిర్మాణం symb_s arg_e)
{
      vvp_net_t*ptr = కొత్త vvp_net_t;
      vvp_latch*fun = కొత్త vvp_latch(width);

      ptr->fun = fun;
      define_functor_చిహ్నం(label, ptr);
      విడుదల(label);
      ఆదానం_connect(ptr, 0, arg_d.పాఠ్యం);
      ఆదానం_connect(ptr, 1, arg_e.పాఠ్యం);

      vvp_vector4_t init_val = vvp_vector4_t(1, BIT4_1);
      కాలపట్టిక_init_సదిశ(vvp_net_ptr_t(ptr,3), init_val);
}
