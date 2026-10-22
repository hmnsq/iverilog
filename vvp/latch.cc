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

vvp_latch::vvp_latch(غیرعلامتی width)
: en_(BIT4_X), d_(width, BIT4_X)
{
}

vvp_latch::~vvp_latch()
{
}

خالی vvp_latch::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                          vvp_context_t)
{
      vvp_bit4_t tmp;

      انتخاب (پورٹ.پورٹ()) {

	  حالت 0: // D
	    d_ = ثنائی_ہندسہ;
	    اگر (en_ == BIT4_1)
		  جدول_وقت_propagate_سمتیہ(پورٹ.ptr(), 0, d_);
	    توڑو;

	  حالت 1: // EN
	    أكد(ثنائی_ہندسہ.size() == 1);
	    tmp = en_;
	    en_ = ثنائی_ہندسہ.value(0);
	    اگر (en_ == BIT4_1 && tmp != BIT4_1)
		  جدول_وقت_propagate_سمتیہ(پورٹ.ptr(), 0, d_);
	    توڑو;

	  حالت 2:
	    أكد(0);
	    توڑو;

	  حالت 3:
	    پورٹ.ptr()->send_vec4(vvp_vector4_t(d_.size(), BIT4_X), 0);
	    توڑو;
      }
}

خالی vvp_latch::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			     غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ثنائی_ہندسہ, بنیاد, vwid, ctx);
}

خالی compile_latch(حرف*label, غیرعلامتی width,
		   ساخت symb_s arg_d,
		   ساخت symb_s arg_e)
{
      vvp_net_t*ptr = نیا vvp_net_t;
      vvp_latch*fun = نیا vvp_latch(width);

      ptr->fun = fun;
      define_functor_علامت(label, ptr);
      آزاد(label);
      داخلہ_connect(ptr, 0, arg_d.متن);
      داخلہ_connect(ptr, 1, arg_e.متن);

      vvp_vector4_t init_val = vvp_vector4_t(1, BIT4_1);
      جدول_وقت_init_سمتیہ(vvp_net_ptr_t(ptr,3), init_val);
}
