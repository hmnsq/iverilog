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

vvp_dff::vvp_dff(ਬਿਨਾਨਿਸ਼ਾਨੀ width, ਤਾਰਕਿਕ negedge)
: clk_(BIT4_X), ena_(BIT4_X), asc_(BIT4_Z), d_(width, BIT4_X)
{
      clk_ਸਰਗਰਮ = negedge ? BIT4_0 : BIT4_1;
}

vvp_dff::~vvp_dff()
{
}

vvp_dff_aclr::vvp_dff_aclr(ਬਿਨਾਨਿਸ਼ਾਨੀ width, ਤਾਰਕਿਕ negedge)
: vvp_dff(width, negedge)
{
}

vvp_dff_aset::vvp_dff_aset(ਬਿਨਾਨਿਸ਼ਾਨੀ width, ਤਾਰਕਿਕ negedge)
: vvp_dff(width, negedge)
{
}

vvp_dff_asc::vvp_dff_asc(ਬਿਨਾਨਿਸ਼ਾਨੀ width, ਤਾਰਕਿਕ negedge, ਅਚਲ ਅੱਖਰ*asc_ਮੁੱਲ)
: vvp_dff(width, negedge), asc_ਮੁੱਲ_2(c4string_ਮੰਜ਼ਿਲ_vector4(asc_ਮੁੱਲ))
{
}

ਖਾਲੀ vvp_dff::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                        vvp_context_t)
{
      vvp_bit4_t tmp;

      ਚੋਣ (ਪੋਰਟ.ਪੋਰਟ()) {

	  ਹਾਲਤ 0: // D
	    d_ = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;

	  ਹਾਲਤ 1: // CLK
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਦੋਆਧਾਰੀ_ਅੰਕ.size() == 1);
	    ਜੇ (asc_ != BIT4_0)
		  ਰੋਕੋ;
	    ਜੇ (ena_ != BIT4_1)
		  ਰੋਕੋ;
	    tmp = clk_;
	    clk_ = ਦੋਆਧਾਰੀ_ਅੰਕ.value(0);
	    ਜੇ (clk_ == clk_ਸਰਗਰਮ && tmp != clk_ਸਰਗਰਮ)
		  ਸਮਾਂ_ਸਾਰਣੀ_propagate_ਸਦਿਸ਼(ਪੋਰਟ.ptr(), 0, d_);
	    ਰੋਕੋ;

	  ਹਾਲਤ 2: // CE
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਦੋਆਧਾਰੀ_ਅੰਕ.size() == 1);
	    ena_ = ਦੋਆਧਾਰੀ_ਅੰਕ.value(0);
	    ਰੋਕੋ;

	  ਹਾਲਤ 3: // asynch SET/CLR
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਦੋਆਧਾਰੀ_ਅੰਕ.size() == 1);
	    tmp = asc_;
	    asc_ = ਦੋਆਧਾਰੀ_ਅੰਕ.value(0);
	    ਜੇ (asc_ == BIT4_1 && tmp != BIT4_1)
		  recv_async(ਪੋਰਟ);
	    ਨਹੀਂਤਾਂ ਜੇ (tmp == BIT4_Z)
		  ਪੋਰਟ.ptr()->send_vec4(vvp_vector4_t(d_.size(), BIT4_X), 0);
	    ਰੋਕੋ;
      }
}

ਖਾਲੀ vvp_dff::recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid, ctx);
}

/*
 * The recv_async functions respond to the asynchronous
 * set/clear input by propagating the desired output.
 *
 * NOTE: Don't touch the d_ value, because that tracks the D input,
 * which may be needed when the device is clocked afterwards.
 */
ਖਾਲੀ vvp_dff::recv_async(vvp_net_ptr_t)
{
	// The base dff does not have an asynchronous set/clr input.
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}

ਖਾਲੀ vvp_dff_aclr::recv_async(vvp_net_ptr_t ਪੋਰਟ)
{
      ਸਮਾਂ_ਸਾਰਣੀ_propagate_ਸਦਿਸ਼(ਪੋਰਟ.ptr(), 0, vvp_vector4_t(d_.size(), BIT4_0));
}

ਖਾਲੀ vvp_dff_aset::recv_async(vvp_net_ptr_t ਪੋਰਟ)
{
      ਸਮਾਂ_ਸਾਰਣੀ_propagate_ਸਦਿਸ਼(ਪੋਰਟ.ptr(), 0, vvp_vector4_t(d_.size(), BIT4_1));
}

ਖਾਲੀ vvp_dff_asc::recv_async(vvp_net_ptr_t ਪੋਰਟ)
{
      ਸਮਾਂ_ਸਾਰਣੀ_propagate_ਸਦਿਸ਼(ਪੋਰਟ.ptr(), 0, asc_ਮੁੱਲ_2);
}

ਖਾਲੀ compile_dff(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ width, ਤਾਰਕਿਕ negedge,
		 ਬਣਤਰ symb_s arg_d,
		 ਬਣਤਰ symb_s arg_c,
		 ਬਣਤਰ symb_s arg_e)
{
      vvp_net_t*ptr = ਨਵਾਂ vvp_net_t;
      vvp_dff*fun = ਨਵਾਂ vvp_dff(width, negedge);

      ptr->fun = fun;
      define_functor_ਚਿੰਨ੍ਹ(label, ptr);
      ਮੁਕਤ(label);
      ਨਿਵੇਸ਼_connect(ptr, 0, arg_d.ਪਾਠ);
      ਨਿਵੇਸ਼_connect(ptr, 1, arg_c.ਪਾਠ);
      ਨਿਵੇਸ਼_connect(ptr, 2, arg_e.ਪਾਠ);

      vvp_vector4_t init_val = vvp_vector4_t(1, BIT4_0);
      ਸਮਾਂ_ਸਾਰਣੀ_init_ਸਦਿਸ਼(vvp_net_ptr_t(ptr,3), init_val);
}

ਖਾਲੀ compile_dff_aclr(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ width, ਤਾਰਕਿਕ negedge,
		      ਬਣਤਰ symb_s arg_d,
		      ਬਣਤਰ symb_s arg_c,
		      ਬਣਤਰ symb_s arg_e,
		      ਬਣਤਰ symb_s arg_a)
{
      vvp_net_t*ptr = ਨਵਾਂ vvp_net_t;
      vvp_dff*fun = ਨਵਾਂ vvp_dff_aclr(width, negedge);

      ptr->fun = fun;
      define_functor_ਚਿੰਨ੍ਹ(label, ptr);
      ਮੁਕਤ(label);
      ਨਿਵੇਸ਼_connect(ptr, 0, arg_d.ਪਾਠ);
      ਨਿਵੇਸ਼_connect(ptr, 1, arg_c.ਪਾਠ);
      ਨਿਵੇਸ਼_connect(ptr, 2, arg_e.ਪਾਠ);
      ਨਿਵੇਸ਼_connect(ptr, 3, arg_a.ਪਾਠ);
}

ਖਾਲੀ compile_dff_aset(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ width, ਤਾਰਕਿਕ negedge,
		      ਬਣਤਰ symb_s arg_d,
		      ਬਣਤਰ symb_s arg_c,
		      ਬਣਤਰ symb_s arg_e,
		      ਬਣਤਰ symb_s arg_a,
		      ਅੱਖਰ*asc_ਮੁੱਲ)
{
      vvp_net_t*ptr = ਨਵਾਂ vvp_net_t;
      vvp_dff*fun;
      ਜੇ (asc_ਮੁੱਲ) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(c4string_test(asc_ਮੁੱਲ));
	    fun = ਨਵਾਂ vvp_dff_asc(width, negedge, asc_ਮੁੱਲ);
	    ਮੁਕਤ(asc_ਮੁੱਲ);
      } ਨਹੀਂਤਾਂ {
	    fun = ਨਵਾਂ vvp_dff_aset(width, negedge);
      }

      ptr->fun = fun;
      define_functor_ਚਿੰਨ੍ਹ(label, ptr);
      ਮੁਕਤ(label);
      ਨਿਵੇਸ਼_connect(ptr, 0, arg_d.ਪਾਠ);
      ਨਿਵੇਸ਼_connect(ptr, 1, arg_c.ਪਾਠ);
      ਨਿਵੇਸ਼_connect(ptr, 2, arg_e.ਪਾਠ);
      ਨਿਵੇਸ਼_connect(ptr, 3, arg_a.ਪਾਠ);
}
