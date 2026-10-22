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

vvp_latch::vvp_latch(unsigned width)
: en_(BIT4_X), d_(width, BIT4_X)
{
}

vvp_latch::~vvp_latch()
{
}

শূন্য vvp_latch::recv_vec4(vvp_net_ptr_t port, ধ্রুৱ vvp_vector4_t&bit,
                          vvp_context_t)
{
      vvp_bit4_t tmp;

      ৱিকল্প (port.port()) {

	  পক্ষ 0: // D
	    d_ = bit;
	    যদি (en_ == BIT4_1)
		  schedule_propagate_vector(port.ptr(), 0, d_);
	    ভঙ্গ;

	  পক্ষ 1: // EN
	    প্রতিজ্ঞা(bit.size() == 1);
	    tmp = en_;
	    en_ = bit.value(0);
	    যদি (en_ == BIT4_1 && tmp != BIT4_1)
		  schedule_propagate_vector(port.ptr(), 0, d_);
	    ভঙ্গ;

	  পক্ষ 2:
	    প্রতিজ্ঞা(0);
	    ভঙ্গ;

	  পক্ষ 3:
	    port.ptr()->send_vec4(vvp_vector4_t(d_.size(), BIT4_X), 0);
	    ভঙ্গ;
      }
}

শূন্য vvp_latch::recv_vec4_pv(vvp_net_ptr_t ptr, ধ্রুৱ vvp_vector4_t&bit,
			     unsigned base, unsigned vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, bit, base, vwid, ctx);
}

শূন্য compile_latch(ৱর্ণ*label, unsigned width,
		   সংরচনা symb_s arg_d,
		   সংরচনা symb_s arg_e)
{
      vvp_net_t*ptr = নূতন vvp_net_t;
      vvp_latch*fun = নূতন vvp_latch(width);

      ptr->fun = fun;
      define_functor_প্ৰতীক(label, ptr);
      মুক্ত(label);
      প্ৰবেশ_connect(ptr, 0, arg_d.text);
      প্ৰবেশ_connect(ptr, 1, arg_e.text);

      vvp_vector4_t init_val = vvp_vector4_t(1, BIT4_1);
      schedule_init_vector(vvp_net_ptr_t(ptr,3), init_val);
}
