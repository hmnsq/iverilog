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

vvp_dff::vvp_dff(unsigned width, ತಾರ್ಕಿಕ negedge)
: clk_(BIT4_X), ena_(BIT4_X), asc_(BIT4_Z), d_(width, BIT4_X)
{
      clk_active_ = negedge ? BIT4_0 : BIT4_1;
}

vvp_dff::~vvp_dff()
{
}

vvp_dff_aclr::vvp_dff_aclr(unsigned width, ತಾರ್ಕಿಕ negedge)
: vvp_dff(width, negedge)
{
}

vvp_dff_aset::vvp_dff_aset(unsigned width, ತಾರ್ಕಿಕ negedge)
: vvp_dff(width, negedge)
{
}

vvp_dff_asc::vvp_dff_asc(unsigned width, ತಾರ್ಕಿಕ negedge, ಧ್ರುವ ವರ್ಣ*asc_ಮೌಲ್ಯ)
: vvp_dff(width, negedge), asc_ಮೌಲ್ಯ_2(c4string_to_vector4(asc_ಮೌಲ್ಯ))
{
}

ಶೂನ್ಯ vvp_dff::recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                        vvp_context_t)
{
      vvp_bit4_t tmp;

      ವಿಕಲ್ಪ (port.port()) {

	  ಪಕ್ಷ 0: // D
	    d_ = bit;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ 1: // CLK
	    ಪ್ರತಿಜ್ಞಾ(bit.size() == 1);
	    ಯದಿ (asc_ != BIT4_0)
		  ಭಙ್ಗ;
	    ಯದಿ (ena_ != BIT4_1)
		  ಭಙ್ಗ;
	    tmp = clk_;
	    clk_ = bit.value(0);
	    ಯದಿ (clk_ == clk_active_ && tmp != clk_active_)
		  schedule_propagate_vector(port.ptr(), 0, d_);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ 2: // CE
	    ಪ್ರತಿಜ್ಞಾ(bit.size() == 1);
	    ena_ = bit.value(0);
	    ಭಙ್ಗ;

	  ಪಕ್ಷ 3: // asynch SET/CLR
	    ಪ್ರತಿಜ್ಞಾ(bit.size() == 1);
	    tmp = asc_;
	    asc_ = bit.value(0);
	    ಯದಿ (asc_ == BIT4_1 && tmp != BIT4_1)
		  recv_async(port);
	    ಅನ್ಯಥಾ ಯದಿ (tmp == BIT4_Z)
		  port.ptr()->send_vec4(vvp_vector4_t(d_.size(), BIT4_X), 0);
	    ಭಙ್ಗ;
      }
}

ಶೂನ್ಯ vvp_dff::recv_vec4_pv(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
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
ಶೂನ್ಯ vvp_dff::recv_async(vvp_net_ptr_t)
{
	// The base dff does not have an asynchronous set/clr input.
      ಪ್ರತಿಜ್ಞಾ(0);
}

ಶೂನ್ಯ vvp_dff_aclr::recv_async(vvp_net_ptr_t port)
{
      schedule_propagate_vector(port.ptr(), 0, vvp_vector4_t(d_.size(), BIT4_0));
}

ಶೂನ್ಯ vvp_dff_aset::recv_async(vvp_net_ptr_t port)
{
      schedule_propagate_vector(port.ptr(), 0, vvp_vector4_t(d_.size(), BIT4_1));
}

ಶೂನ್ಯ vvp_dff_asc::recv_async(vvp_net_ptr_t port)
{
      schedule_propagate_vector(port.ptr(), 0, asc_ಮೌಲ್ಯ_2);
}

ಶೂನ್ಯ compile_dff(ವರ್ಣ*label, unsigned width, ತಾರ್ಕಿಕ negedge,
		 ಸಂರಚನಾ symb_s arg_d,
		 ಸಂರಚನಾ symb_s arg_c,
		 ಸಂರಚನಾ symb_s arg_e)
{
      vvp_net_t*ptr = ನೂತನ vvp_net_t;
      vvp_dff*fun = ನೂತನ vvp_dff(width, negedge);

      ptr->fun = fun;
      define_functor_ಚಿಹ್ನೆ(label, ptr);
      ಮುಕ್ತ(label);
      ಒಳಹರಿವು_connect(ptr, 0, arg_d.text);
      ಒಳಹರಿವು_connect(ptr, 1, arg_c.text);
      ಒಳಹರಿವು_connect(ptr, 2, arg_e.text);

      vvp_vector4_t init_val = vvp_vector4_t(1, BIT4_0);
      schedule_init_vector(vvp_net_ptr_t(ptr,3), init_val);
}

ಶೂನ್ಯ compile_dff_aclr(ವರ್ಣ*label, unsigned width, ತಾರ್ಕಿಕ negedge,
		      ಸಂರಚನಾ symb_s arg_d,
		      ಸಂರಚನಾ symb_s arg_c,
		      ಸಂರಚನಾ symb_s arg_e,
		      ಸಂರಚನಾ symb_s arg_a)
{
      vvp_net_t*ptr = ನೂತನ vvp_net_t;
      vvp_dff*fun = ನೂತನ vvp_dff_aclr(width, negedge);

      ptr->fun = fun;
      define_functor_ಚಿಹ್ನೆ(label, ptr);
      ಮುಕ್ತ(label);
      ಒಳಹರಿವು_connect(ptr, 0, arg_d.text);
      ಒಳಹರಿವು_connect(ptr, 1, arg_c.text);
      ಒಳಹರಿವು_connect(ptr, 2, arg_e.text);
      ಒಳಹರಿವು_connect(ptr, 3, arg_a.text);
}

ಶೂನ್ಯ compile_dff_aset(ವರ್ಣ*label, unsigned width, ತಾರ್ಕಿಕ negedge,
		      ಸಂರಚನಾ symb_s arg_d,
		      ಸಂರಚನಾ symb_s arg_c,
		      ಸಂರಚನಾ symb_s arg_e,
		      ಸಂರಚನಾ symb_s arg_a,
		      ವರ್ಣ*asc_ಮೌಲ್ಯ)
{
      vvp_net_t*ptr = ನೂತನ vvp_net_t;
      vvp_dff*fun;
      ಯದಿ (asc_ಮೌಲ್ಯ) {
	    ಪ್ರತಿಜ್ಞಾ(c4string_test(asc_ಮೌಲ್ಯ));
	    fun = ನೂತನ vvp_dff_asc(width, negedge, asc_ಮೌಲ್ಯ);
	    ಮುಕ್ತ(asc_ಮೌಲ್ಯ);
      } ಅನ್ಯಥಾ {
	    fun = ನೂತನ vvp_dff_aset(width, negedge);
      }

      ptr->fun = fun;
      define_functor_ಚಿಹ್ನೆ(label, ptr);
      ಮುಕ್ತ(label);
      ಒಳಹರಿವು_connect(ptr, 0, arg_d.text);
      ಒಳಹರಿವು_connect(ptr, 1, arg_c.text);
      ಒಳಹರಿವು_connect(ptr, 2, arg_e.text);
      ಒಳಹರಿವು_connect(ptr, 3, arg_a.text);
}
