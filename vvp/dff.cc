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

vvp_dff::vvp_dff(بینښه width, منطقي negedge)
: clk_(BIT4_X), ena_(BIT4_X), asc_(BIT4_Z), d_(width, BIT4_X)
{
      clk_فعال = negedge ? BIT4_0 : BIT4_1;
}

vvp_dff::~vvp_dff()
{
}

vvp_dff_aclr::vvp_dff_aclr(بینښه width, منطقي negedge)
: vvp_dff(width, negedge)
{
}

vvp_dff_aset::vvp_dff_aset(بینښه width, منطقي negedge)
: vvp_dff(width, negedge)
{
}

vvp_dff_asc::vvp_dff_asc(بینښه width, منطقي negedge, ثابت حرف*asc_ارزښت)
: vvp_dff(width, negedge), asc_ارزښت_2(c4string_منزل_vector4(asc_ارزښت))
{
}

فراغ vvp_dff::recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                        vvp_context_t)
{
      vvp_bit4_t tmp;

      بدل (پورټ.پورټ()) {

	  حالة 0: // D
	    d_ = بټ;
	    اكسر;

	  حالة 1: // CLK
	    أكد(بټ.size() == 1);
	    إذا (asc_ != BIT4_0)
		  اكسر;
	    إذا (ena_ != BIT4_1)
		  اكسر;
	    tmp = clk_;
	    clk_ = بټ.value(0);
	    إذا (clk_ == clk_فعال && tmp != clk_فعال)
		  مهالوېش_propagate_ویکټور(پورټ.ptr(), 0, d_);
	    اكسر;

	  حالة 2: // CE
	    أكد(بټ.size() == 1);
	    ena_ = بټ.value(0);
	    اكسر;

	  حالة 3: // asynch SET/CLR
	    أكد(بټ.size() == 1);
	    tmp = asc_;
	    asc_ = بټ.value(0);
	    إذا (asc_ == BIT4_1 && tmp != BIT4_1)
		  recv_async(پورټ);
	    وإلا إذا (tmp == BIT4_Z)
		  پورټ.ptr()->send_vec4(vvp_vector4_t(d_.size(), BIT4_X), 0);
	    اكسر;
      }
}

فراغ vvp_dff::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بټ,
			   بینښه بنسټ, بینښه vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, بټ, بنسټ, vwid, ctx);
}

/*
 * The recv_async functions respond to the asynchronous
 * set/clear input by propagating the desired output.
 *
 * NOTE: Don't touch the d_ value, because that tracks the D input,
 * which may be needed when the device is clocked afterwards.
 */
فراغ vvp_dff::recv_async(vvp_net_ptr_t)
{
	// The base dff does not have an asynchronous set/clr input.
      أكد(0);
}

فراغ vvp_dff_aclr::recv_async(vvp_net_ptr_t پورټ)
{
      مهالوېش_propagate_ویکټور(پورټ.ptr(), 0, vvp_vector4_t(d_.size(), BIT4_0));
}

فراغ vvp_dff_aset::recv_async(vvp_net_ptr_t پورټ)
{
      مهالوېش_propagate_ویکټور(پورټ.ptr(), 0, vvp_vector4_t(d_.size(), BIT4_1));
}

فراغ vvp_dff_asc::recv_async(vvp_net_ptr_t پورټ)
{
      مهالوېش_propagate_ویکټور(پورټ.ptr(), 0, asc_ارزښت_2);
}

فراغ compile_dff(حرف*label, بینښه width, منطقي negedge,
		 بنية symb_s arg_d,
		 بنية symb_s arg_c,
		 بنية symb_s arg_e)
{
      vvp_net_t*ptr = جديد vvp_net_t;
      vvp_dff*fun = جديد vvp_dff(width, negedge);

      ptr->fun = fun;
      define_functor_نښه(label, ptr);
      حرر(label);
      ننوت_connect(ptr, 0, arg_d.متن);
      ننوت_connect(ptr, 1, arg_c.متن);
      ننوت_connect(ptr, 2, arg_e.متن);

      vvp_vector4_t init_val = vvp_vector4_t(1, BIT4_0);
      مهالوېش_init_ویکټور(vvp_net_ptr_t(ptr,3), init_val);
}

فراغ compile_dff_aclr(حرف*label, بینښه width, منطقي negedge,
		      بنية symb_s arg_d,
		      بنية symb_s arg_c,
		      بنية symb_s arg_e,
		      بنية symb_s arg_a)
{
      vvp_net_t*ptr = جديد vvp_net_t;
      vvp_dff*fun = جديد vvp_dff_aclr(width, negedge);

      ptr->fun = fun;
      define_functor_نښه(label, ptr);
      حرر(label);
      ننوت_connect(ptr, 0, arg_d.متن);
      ننوت_connect(ptr, 1, arg_c.متن);
      ننوت_connect(ptr, 2, arg_e.متن);
      ننوت_connect(ptr, 3, arg_a.متن);
}

فراغ compile_dff_aset(حرف*label, بینښه width, منطقي negedge,
		      بنية symb_s arg_d,
		      بنية symb_s arg_c,
		      بنية symb_s arg_e,
		      بنية symb_s arg_a,
		      حرف*asc_ارزښت)
{
      vvp_net_t*ptr = جديد vvp_net_t;
      vvp_dff*fun;
      إذا (asc_ارزښت) {
	    أكد(c4string_test(asc_ارزښت));
	    fun = جديد vvp_dff_asc(width, negedge, asc_ارزښت);
	    حرر(asc_ارزښت);
      } وإلا {
	    fun = جديد vvp_dff_aset(width, negedge);
      }

      ptr->fun = fun;
      define_functor_نښه(label, ptr);
      حرر(label);
      ننوت_connect(ptr, 0, arg_d.متن);
      ننوت_connect(ptr, 1, arg_c.متن);
      ننوت_connect(ptr, 2, arg_e.متن);
      ننوت_connect(ptr, 3, arg_a.متن);
}
