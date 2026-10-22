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

vvp_dff::vvp_dff(unsigned width, തർക്കം negedge)
: clk_(BIT4_X), ena_(BIT4_X), asc_(BIT4_Z), d_(width, BIT4_X)
{
      clk_active_ = negedge ? BIT4_0 : BIT4_1;
}

vvp_dff::~vvp_dff()
{
}

vvp_dff_aclr::vvp_dff_aclr(unsigned width, തർക്കം negedge)
: vvp_dff(width, negedge)
{
}

vvp_dff_aset::vvp_dff_aset(unsigned width, തർക്കം negedge)
: vvp_dff(width, negedge)
{
}

vvp_dff_asc::vvp_dff_asc(unsigned width, തർക്കം negedge, സ്ഥിരം അക്ഷരം*asc_മൂല്യം)
: vvp_dff(width, negedge), asc_മൂല്യം_2(c4string_to_vector4(asc_മൂല്യം))
{
}

ശൂന്യം vvp_dff::recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                        vvp_context_t)
{
      vvp_bit4_t tmp;

      തിരഞ്ഞെടുപ്പ് (port.port()) {

	  അവസ്ഥ 0: // D
	    d_ = bit;
	    നിർത്തുക;

	  അവസ്ഥ 1: // CLK
	    പ്രതിജ്ഞാ(bit.size() == 1);
	    എങ്കിൽ (asc_ != BIT4_0)
		  നിർത്തുക;
	    എങ്കിൽ (ena_ != BIT4_1)
		  നിർത്തുക;
	    tmp = clk_;
	    clk_ = bit.value(0);
	    എങ്കിൽ (clk_ == clk_active_ && tmp != clk_active_)
		  schedule_propagate_vector(port.ptr(), 0, d_);
	    നിർത്തുക;

	  അവസ്ഥ 2: // CE
	    പ്രതിജ്ഞാ(bit.size() == 1);
	    ena_ = bit.value(0);
	    നിർത്തുക;

	  അവസ്ഥ 3: // asynch SET/CLR
	    പ്രതിജ്ഞാ(bit.size() == 1);
	    tmp = asc_;
	    asc_ = bit.value(0);
	    എങ്കിൽ (asc_ == BIT4_1 && tmp != BIT4_1)
		  recv_async(port);
	    അല്ലെങ്കിൽ എങ്കിൽ (tmp == BIT4_Z)
		  port.ptr()->send_vec4(vvp_vector4_t(d_.size(), BIT4_X), 0);
	    നിർത്തുക;
      }
}

ശൂന്യം vvp_dff::recv_vec4_pv(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
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
ശൂന്യം vvp_dff::recv_async(vvp_net_ptr_t)
{
	// The base dff does not have an asynchronous set/clr input.
      പ്രതിജ്ഞാ(0);
}

ശൂന്യം vvp_dff_aclr::recv_async(vvp_net_ptr_t port)
{
      schedule_propagate_vector(port.ptr(), 0, vvp_vector4_t(d_.size(), BIT4_0));
}

ശൂന്യം vvp_dff_aset::recv_async(vvp_net_ptr_t port)
{
      schedule_propagate_vector(port.ptr(), 0, vvp_vector4_t(d_.size(), BIT4_1));
}

ശൂന്യം vvp_dff_asc::recv_async(vvp_net_ptr_t port)
{
      schedule_propagate_vector(port.ptr(), 0, asc_മൂല്യം_2);
}

ശൂന്യം compile_dff(അക്ഷരം*label, unsigned width, തർക്കം negedge,
		 ഘടന symb_s arg_d,
		 ഘടന symb_s arg_c,
		 ഘടന symb_s arg_e)
{
      vvp_net_t*ptr = പുതിയ vvp_net_t;
      vvp_dff*fun = പുതിയ vvp_dff(width, negedge);

      ptr->fun = fun;
      define_functor_ചിഹ്നം(label, ptr);
      സ്വതന്ത്രം(label);
      നിവേശം_connect(ptr, 0, arg_d.text);
      നിവേശം_connect(ptr, 1, arg_c.text);
      നിവേശം_connect(ptr, 2, arg_e.text);

      vvp_vector4_t init_val = vvp_vector4_t(1, BIT4_0);
      schedule_init_vector(vvp_net_ptr_t(ptr,3), init_val);
}

ശൂന്യം compile_dff_aclr(അക്ഷരം*label, unsigned width, തർക്കം negedge,
		      ഘടന symb_s arg_d,
		      ഘടന symb_s arg_c,
		      ഘടന symb_s arg_e,
		      ഘടന symb_s arg_a)
{
      vvp_net_t*ptr = പുതിയ vvp_net_t;
      vvp_dff*fun = പുതിയ vvp_dff_aclr(width, negedge);

      ptr->fun = fun;
      define_functor_ചിഹ്നം(label, ptr);
      സ്വതന്ത്രം(label);
      നിവേശം_connect(ptr, 0, arg_d.text);
      നിവേശം_connect(ptr, 1, arg_c.text);
      നിവേശം_connect(ptr, 2, arg_e.text);
      നിവേശം_connect(ptr, 3, arg_a.text);
}

ശൂന്യം compile_dff_aset(അക്ഷരം*label, unsigned width, തർക്കം negedge,
		      ഘടന symb_s arg_d,
		      ഘടന symb_s arg_c,
		      ഘടന symb_s arg_e,
		      ഘടന symb_s arg_a,
		      അക്ഷരം*asc_മൂല്യം)
{
      vvp_net_t*ptr = പുതിയ vvp_net_t;
      vvp_dff*fun;
      എങ്കിൽ (asc_മൂല്യം) {
	    പ്രതിജ്ഞാ(c4string_test(asc_മൂല്യം));
	    fun = പുതിയ vvp_dff_asc(width, negedge, asc_മൂല്യം);
	    സ്വതന്ത്രം(asc_മൂല്യം);
      } അല്ലെങ്കിൽ {
	    fun = പുതിയ vvp_dff_aset(width, negedge);
      }

      ptr->fun = fun;
      define_functor_ചിഹ്നം(label, ptr);
      സ്വതന്ത്രം(label);
      നിവേശം_connect(ptr, 0, arg_d.text);
      നിവേശം_connect(ptr, 1, arg_c.text);
      നിവേശം_connect(ptr, 2, arg_e.text);
      നിവേശം_connect(ptr, 3, arg_a.text);
}
