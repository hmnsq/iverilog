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

vvp_latch::vvp_latch(ਬਿਨਾਨਿਸ਼ਾਨੀ width)
: en_(BIT4_X), d_(width, BIT4_X)
{
}

vvp_latch::~vvp_latch()
{
}

ਖਾਲੀ vvp_latch::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                          vvp_context_t)
{
      vvp_bit4_t tmp;

      ਚੋਣ (ਪੋਰਟ.ਪੋਰਟ()) {

	  ਹਾਲਤ 0: // D
	    d_ = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਜੇ (en_ == BIT4_1)
		  ਸਮਾਂ_ਸਾਰਣੀ_propagate_ਸਦਿਸ਼(ਪੋਰਟ.ptr(), 0, d_);
	    ਰੋਕੋ;

	  ਹਾਲਤ 1: // EN
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਦੋਆਧਾਰੀ_ਅੰਕ.size() == 1);
	    tmp = en_;
	    en_ = ਦੋਆਧਾਰੀ_ਅੰਕ.value(0);
	    ਜੇ (en_ == BIT4_1 && tmp != BIT4_1)
		  ਸਮਾਂ_ਸਾਰਣੀ_propagate_ਸਦਿਸ਼(ਪੋਰਟ.ptr(), 0, d_);
	    ਰੋਕੋ;

	  ਹਾਲਤ 2:
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਰੋਕੋ;

	  ਹਾਲਤ 3:
	    ਪੋਰਟ.ptr()->send_vec4(vvp_vector4_t(d_.size(), BIT4_X), 0);
	    ਰੋਕੋ;
      }
}

ਖਾਲੀ vvp_latch::recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid, ctx);
}

ਖਾਲੀ compile_latch(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ width,
		   ਬਣਤਰ symb_s arg_d,
		   ਬਣਤਰ symb_s arg_e)
{
      vvp_net_t*ptr = ਨਵਾਂ vvp_net_t;
      vvp_latch*fun = ਨਵਾਂ vvp_latch(width);

      ptr->fun = fun;
      define_functor_ਚਿੰਨ੍ਹ(label, ptr);
      ਮੁਕਤ(label);
      ਨਿਵੇਸ਼_connect(ptr, 0, arg_d.ਪਾਠ);
      ਨਿਵੇਸ਼_connect(ptr, 1, arg_e.ਪਾਠ);

      vvp_vector4_t init_val = vvp_vector4_t(1, BIT4_1);
      ਸਮਾਂ_ਸਾਰਣੀ_init_ਸਦਿਸ਼(vvp_net_ptr_t(ptr,3), init_val);
}
