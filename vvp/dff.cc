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

vvp_dff::vvp_dff(unsigned width, तार्किक negedge)
: clk_(BIT4_X), ena_(BIT4_X), asc_(BIT4_Z), d_(width, BIT4_X)
{
      clk_active_ = negedge ? BIT4_0 : BIT4_1;
}

vvp_dff::~vvp_dff()
{
}

vvp_dff_aclr::vvp_dff_aclr(unsigned width, तार्किक negedge)
: vvp_dff(width, negedge)
{
}

vvp_dff_aset::vvp_dff_aset(unsigned width, तार्किक negedge)
: vvp_dff(width, negedge)
{
}

vvp_dff_asc::vvp_dff_asc(unsigned width, तार्किक negedge, ध्रुव वर्ण*asc_मानम्)
: vvp_dff(width, negedge), asc_मानम्_2(c4string_to_vector4(asc_मानम्))
{
}

शून्य vvp_dff::recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                        vvp_context_t)
{
      vvp_bit4_t tmp;

      विकल्प (port.port()) {

	  पक्ष 0: // D
	    d_ = bit;
	    भङ्ग;

	  पक्ष 1: // CLK
	    प्रतिज्ञा(bit.size() == 1);
	    सचे (asc_ != BIT4_0)
		  भङ्ग;
	    सचे (ena_ != BIT4_1)
		  भङ्ग;
	    tmp = clk_;
	    clk_ = bit.value(0);
	    सचे (clk_ == clk_active_ && tmp != clk_active_)
		  schedule_propagate_vector(port.ptr(), 0, d_);
	    भङ्ग;

	  पक्ष 2: // CE
	    प्रतिज्ञा(bit.size() == 1);
	    ena_ = bit.value(0);
	    भङ्ग;

	  पक्ष 3: // asynch SET/CLR
	    प्रतिज्ञा(bit.size() == 1);
	    tmp = asc_;
	    asc_ = bit.value(0);
	    सचे (asc_ == BIT4_1 && tmp != BIT4_1)
		  recv_async(port);
	    अञ्ञथा सचे (tmp == BIT4_Z)
		  port.ptr()->send_vec4(vvp_vector4_t(d_.size(), BIT4_X), 0);
	    भङ्ग;
      }
}

शून्य vvp_dff::recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
			   unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

/*
 * The recv_async functions respond to the asynchronous
 * set/clear input by propagating the desired output.
 *
 * NOTE: Don't touch the d_ value, because that tracks the D input,
 * which may be needed when the device is clocked afterwards.
 */
शून्य vvp_dff::recv_async(vvp_net_ptr_t)
{
	// The base dff does not have an asynchronous set/clr input.
      प्रतिज्ञा(0);
}

शून्य vvp_dff_aclr::recv_async(vvp_net_ptr_t port)
{
      schedule_propagate_vector(port.ptr(), 0, vvp_vector4_t(d_.size(), BIT4_0));
}

शून्य vvp_dff_aset::recv_async(vvp_net_ptr_t port)
{
      schedule_propagate_vector(port.ptr(), 0, vvp_vector4_t(d_.size(), BIT4_1));
}

शून्य vvp_dff_asc::recv_async(vvp_net_ptr_t port)
{
      schedule_propagate_vector(port.ptr(), 0, asc_मानम्_2);
}

शून्य compile_dff(वर्ण*label, unsigned width, तार्किक negedge,
		 संरचना symb_s arg_d,
		 संरचना symb_s arg_c,
		 संरचना symb_s arg_e)
{
      vvp_net_t*ptr = नूतन vvp_net_t;
      vvp_dff*fun = नूतन vvp_dff(width, negedge);

      ptr->fun = fun;
      define_functor_चिह्नम्(label, ptr);
      मुक्त(label);
      निवेशः_connect(ptr, 0, arg_d.text);
      निवेशः_connect(ptr, 1, arg_c.text);
      निवेशः_connect(ptr, 2, arg_e.text);

      vvp_vector4_t init_val = vvp_vector4_t(1, BIT4_0);
      schedule_init_vector(vvp_net_ptr_t(ptr,3), init_val);
}

शून्य compile_dff_aclr(वर्ण*label, unsigned width, तार्किक negedge,
		      संरचना symb_s arg_d,
		      संरचना symb_s arg_c,
		      संरचना symb_s arg_e,
		      संरचना symb_s arg_a)
{
      vvp_net_t*ptr = नूतन vvp_net_t;
      vvp_dff*fun = नूतन vvp_dff_aclr(width, negedge);

      ptr->fun = fun;
      define_functor_चिह्नम्(label, ptr);
      मुक्त(label);
      निवेशः_connect(ptr, 0, arg_d.text);
      निवेशः_connect(ptr, 1, arg_c.text);
      निवेशः_connect(ptr, 2, arg_e.text);
      निवेशः_connect(ptr, 3, arg_a.text);
}

शून्य compile_dff_aset(वर्ण*label, unsigned width, तार्किक negedge,
		      संरचना symb_s arg_d,
		      संरचना symb_s arg_c,
		      संरचना symb_s arg_e,
		      संरचना symb_s arg_a,
		      वर्ण*asc_मानम्)
{
      vvp_net_t*ptr = नूतन vvp_net_t;
      vvp_dff*fun;
      सचे (asc_मानम्) {
	    प्रतिज्ञा(c4string_test(asc_मानम्));
	    fun = नूतन vvp_dff_asc(width, negedge, asc_मानम्);
	    मुक्त(asc_मानम्);
      } अञ्ञथा {
	    fun = नूतन vvp_dff_aset(width, negedge);
      }

      ptr->fun = fun;
      define_functor_चिह्नम्(label, ptr);
      मुक्त(label);
      निवेशः_connect(ptr, 0, arg_d.text);
      निवेशः_connect(ptr, 1, arg_c.text);
      निवेशः_connect(ptr, 2, arg_e.text);
      निवेशः_connect(ptr, 3, arg_a.text);
}
