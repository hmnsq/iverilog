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

vvp_dff::vvp_dff(ಚಿಹ್ನೆರಹಿತ width, ತಾರ್ಕಿಕ negedge)
: clk_(BIT4_X), ena_(BIT4_X), asc_(BIT4_Z), d_(width, BIT4_X)
{
      clk_ಸಕ್ರಿಯ = negedge ? BIT4_0 : BIT4_1;
}

vvp_dff::~vvp_dff()
{
}

vvp_dff_aclr::vvp_dff_aclr(ಚಿಹ್ನೆರಹಿತ width, ತಾರ್ಕಿಕ negedge)
: vvp_dff(width, negedge)
{
}

vvp_dff_aset::vvp_dff_aset(ಚಿಹ್ನೆರಹಿತ width, ತಾರ್ಕಿಕ negedge)
: vvp_dff(width, negedge)
{
}

vvp_dff_asc::vvp_dff_asc(ಚಿಹ್ನೆರಹಿತ width, ತಾರ್ಕಿಕ negedge, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*asc_ಮೌಲ್ಯ)
: vvp_dff(width, negedge), asc_ಮೌಲ್ಯ_2(c4string_ಗಮ್ಯ_vector4(asc_ಮೌಲ್ಯ))
{
}

ಶೂನ್ಯ vvp_dff::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                        vvp_context_t)
{
      vvp_bit4_t tmp;

      ಆಯ್ಕೆ (ಪೋರ್ಟ್.ಪೋರ್ಟ್()) {

	  ಸ್ಥಿತಿ 0: // D
	    d_ = ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ 1: // CLK
	    ಪ್ರತಿಜ್ಞಾ(ಬಿಟ್.size() == 1);
	    ಆದರೆ (asc_ != BIT4_0)
		  ನಿಲ್ಲಿಸು;
	    ಆದರೆ (ena_ != BIT4_1)
		  ನಿಲ್ಲಿಸು;
	    tmp = clk_;
	    clk_ = ಬಿಟ್.value(0);
	    ಆದರೆ (clk_ == clk_ಸಕ್ರಿಯ && tmp != clk_ಸಕ್ರಿಯ)
		  ವೇಳಾಪಟ್ಟಿ_propagate_ಸದಿಶ(ಪೋರ್ಟ್.ptr(), 0, d_);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ 2: // CE
	    ಪ್ರತಿಜ್ಞಾ(ಬಿಟ್.size() == 1);
	    ena_ = ಬಿಟ್.value(0);
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ 3: // asynch SET/CLR
	    ಪ್ರತಿಜ್ಞಾ(ಬಿಟ್.size() == 1);
	    tmp = asc_;
	    asc_ = ಬಿಟ್.value(0);
	    ಆದರೆ (asc_ == BIT4_1 && tmp != BIT4_1)
		  recv_async(ಪೋರ್ಟ್);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tmp == BIT4_Z)
		  ಪೋರ್ಟ್.ptr()->send_vec4(vvp_vector4_t(d_.size(), BIT4_X), 0);
	    ನಿಲ್ಲಿಸು;
      }
}

ಶೂನ್ಯ vvp_dff::recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			   ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ಬಿಟ್, ತಳ, vwid, ctx);
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

ಶೂನ್ಯ vvp_dff_aclr::recv_async(vvp_net_ptr_t ಪೋರ್ಟ್)
{
      ವೇಳಾಪಟ್ಟಿ_propagate_ಸದಿಶ(ಪೋರ್ಟ್.ptr(), 0, vvp_vector4_t(d_.size(), BIT4_0));
}

ಶೂನ್ಯ vvp_dff_aset::recv_async(vvp_net_ptr_t ಪೋರ್ಟ್)
{
      ವೇಳಾಪಟ್ಟಿ_propagate_ಸದಿಶ(ಪೋರ್ಟ್.ptr(), 0, vvp_vector4_t(d_.size(), BIT4_1));
}

ಶೂನ್ಯ vvp_dff_asc::recv_async(vvp_net_ptr_t ಪೋರ್ಟ್)
{
      ವೇಳಾಪಟ್ಟಿ_propagate_ಸದಿಶ(ಪೋರ್ಟ್.ptr(), 0, asc_ಮೌಲ್ಯ_2);
}

ಶೂನ್ಯ compile_dff(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ width, ತಾರ್ಕಿಕ negedge,
		 ರಚನೆ symb_s arg_d,
		 ರಚನೆ symb_s arg_c,
		 ರಚನೆ symb_s arg_e)
{
      vvp_net_t*ptr = ಹೊಸ vvp_net_t;
      vvp_dff*fun = ಹೊಸ vvp_dff(width, negedge);

      ptr->fun = fun;
      define_functor_ಚಿಹ್ನೆ(label, ptr);
      ಬಿಡುಗಡೆ(label);
      ಒಳಹರಿವು_connect(ptr, 0, arg_d.ಪಠ್ಯ);
      ಒಳಹರಿವು_connect(ptr, 1, arg_c.ಪಠ್ಯ);
      ಒಳಹರಿವು_connect(ptr, 2, arg_e.ಪಠ್ಯ);

      vvp_vector4_t init_val = vvp_vector4_t(1, BIT4_0);
      ವೇಳಾಪಟ್ಟಿ_init_ಸದಿಶ(vvp_net_ptr_t(ptr,3), init_val);
}

ಶೂನ್ಯ compile_dff_aclr(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ width, ತಾರ್ಕಿಕ negedge,
		      ರಚನೆ symb_s arg_d,
		      ರಚನೆ symb_s arg_c,
		      ರಚನೆ symb_s arg_e,
		      ರಚನೆ symb_s arg_a)
{
      vvp_net_t*ptr = ಹೊಸ vvp_net_t;
      vvp_dff*fun = ಹೊಸ vvp_dff_aclr(width, negedge);

      ptr->fun = fun;
      define_functor_ಚಿಹ್ನೆ(label, ptr);
      ಬಿಡುಗಡೆ(label);
      ಒಳಹರಿವು_connect(ptr, 0, arg_d.ಪಠ್ಯ);
      ಒಳಹರಿವು_connect(ptr, 1, arg_c.ಪಠ್ಯ);
      ಒಳಹರಿವು_connect(ptr, 2, arg_e.ಪಠ್ಯ);
      ಒಳಹರಿವು_connect(ptr, 3, arg_a.ಪಠ್ಯ);
}

ಶೂನ್ಯ compile_dff_aset(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ width, ತಾರ್ಕಿಕ negedge,
		      ರಚನೆ symb_s arg_d,
		      ರಚನೆ symb_s arg_c,
		      ರಚನೆ symb_s arg_e,
		      ರಚನೆ symb_s arg_a,
		      ಅಕ್ಷರ*asc_ಮೌಲ್ಯ)
{
      vvp_net_t*ptr = ಹೊಸ vvp_net_t;
      vvp_dff*fun;
      ಆದರೆ (asc_ಮೌಲ್ಯ) {
	    ಪ್ರತಿಜ್ಞಾ(c4string_test(asc_ಮೌಲ್ಯ));
	    fun = ಹೊಸ vvp_dff_asc(width, negedge, asc_ಮೌಲ್ಯ);
	    ಬಿಡುಗಡೆ(asc_ಮೌಲ್ಯ);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    fun = ಹೊಸ vvp_dff_aset(width, negedge);
      }

      ptr->fun = fun;
      define_functor_ಚಿಹ್ನೆ(label, ptr);
      ಬಿಡುಗಡೆ(label);
      ಒಳಹರಿವು_connect(ptr, 0, arg_d.ಪಠ್ಯ);
      ಒಳಹರಿವು_connect(ptr, 1, arg_c.ಪಠ್ಯ);
      ಒಳಹರಿವು_connect(ptr, 2, arg_e.ಪಠ್ಯ);
      ಒಳಹರಿವು_connect(ptr, 3, arg_a.ಪಠ್ಯ);
}
