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

vvp_latch::vvp_latch(بیعلامت width)
: en_(BIT4_X), d_(width, BIT4_X)
{
}

vvp_latch::~vvp_latch()
{
}

فراغ vvp_latch::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                          vvp_context_t)
{
      vvp_bit4_t tmp;

      بدل (پورت.پورت()) {

	  حالة 0: // D
	    d_ = بیت;
	    إذا (en_ == BIT4_1)
		  زمانبندی_propagate_وکتور(پورت.ptr(), 0, d_);
	    اكسر;

	  حالة 1: // EN
	    أكد(بیت.size() == 1);
	    tmp = en_;
	    en_ = بیت.value(0);
	    إذا (en_ == BIT4_1 && tmp != BIT4_1)
		  زمانبندی_propagate_وکتور(پورت.ptr(), 0, d_);
	    اكسر;

	  حالة 2:
	    أكد(0);
	    اكسر;

	  حالة 3:
	    پورت.ptr()->send_vec4(vvp_vector4_t(d_.size(), BIT4_X), 0);
	    اكسر;
      }
}

فراغ vvp_latch::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
			     بیعلامت پایه, بیعلامت vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, بیت, پایه, vwid, ctx);
}

فراغ compile_latch(حرف*label, بیعلامت width,
		   بنية symb_s arg_d,
		   بنية symb_s arg_e)
{
      vvp_net_t*ptr = جديد vvp_net_t;
      vvp_latch*fun = جديد vvp_latch(width);

      ptr->fun = fun;
      define_functor_نماد(label, ptr);
      حرر(label);
      درون_داد_connect(ptr, 0, arg_d.متن);
      درون_داد_connect(ptr, 1, arg_e.متن);

      vvp_vector4_t init_val = vvp_vector4_t(1, BIT4_1);
      زمانبندی_init_وکتور(vvp_net_ptr_t(ptr,3), init_val);
}
