/*
 * Copyright (c) 2001-2026 Stephen Williams (steve@icarus.com)
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

# include  "logic.h"
# include  "compile.h"
# include  "bufif.h"
# include  "npmos.h"
# include  "schedule.h"
# include  "delay.h"
# include  "statistics.h"
# include  <iostream>
# include  <cstring>
# include  <cassert>
# include  <cstdlib>

vvp_fun_boolean_::vvp_fun_boolean_(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      net_ = 0;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < 4 ;  idx += 1)
	    ਨਿਵੇਸ਼_2[idx] = vvp_vector4_t(wid, BIT4_Z);
}

vvp_fun_boolean_::~vvp_fun_boolean_()
{
}

ਖਾਲੀ vvp_fun_boolean_::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                 vvp_context_t)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ = ptr.ਪੋਰਟ();
      ਜੇ (ਨਿਵੇਸ਼_2[ਪੋਰਟ] .eeq( ਦੋਆਧਾਰੀ_ਅੰਕ )) ਵਾਪਸ;

      ਨਿਵੇਸ਼_2[ਪੋਰਟ] = ਦੋਆਧਾਰੀ_ਅੰਕ;
      ਜੇ (net_ == 0) {
	    net_ = ptr.ptr();
	    ਸਮਾਂ_ਸਾਰਣੀ_functor(ਇਹ);
      }
}

ਖਾਲੀ vvp_fun_boolean_::recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ = ptr.ਪੋਰਟ();

      ਪ੍ਰਤਿਜ੍ਞਾ(ਅਧਾਰ + ਦੋਆਧਾਰੀ_ਅੰਕ.size() <= vwid);

	// Set the part for the input. If nothing changes, then break.
      ਤਾਰਕਿਕ flag = ਨਿਵੇਸ਼_2[ਪੋਰਟ] .ਸੈੱਟ_vec(ਅਧਾਰ, ਦੋਆਧਾਰੀ_ਅੰਕ);
      ਜੇ (flag == ਝੂਠ)
	    ਵਾਪਸ;

      ਜੇ (net_ == 0) {
	    net_ = ptr.ptr();
	    ਸਮਾਂ_ਸਾਰਣੀ_functor(ਇਹ);
      }
}

ਖਾਲੀ vvp_fun_boolean_::recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ real,
                                 vvp_context_t ctx)
{
      recv_vec4(ptr, double_ਮੰਜ਼ਿਲ_vector4_LSB(real), ctx);
}

vvp_fun_and::vvp_fun_and(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਤਾਰਕਿਕ invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      ਗਿਣਤੀ_functors_logic += 1;
}

vvp_fun_and::~vvp_fun_and()
{
}

ਖਾਲੀ vvp_fun_and::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t ਨਤੀਜਾ (ਨਿਵੇਸ਼_2[0]);

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ pdx = 1 ; pdx < 4 ; pdx += 1)
	    ਨਤੀਜਾ &= ਨਿਵੇਸ਼_2[pdx];

      ਜੇ (invert_)
	    ਨਤੀਜਾ.invert();

      ptr->send_vec4(ਨਤੀਜਾ, 0);
}

vvp_fun_equiv::vvp_fun_equiv()
: vvp_fun_boolean_(1)
{
      ਗਿਣਤੀ_functors_logic += 1;
}

vvp_fun_equiv::~vvp_fun_equiv()
{
}

ਖਾਲੀ vvp_fun_equiv::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਵੇਸ਼_2[0].size() == 1);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਵੇਸ਼_2[1].size() == 1);

      vvp_bit4_t ਦੋਆਧਾਰੀ_ਅੰਕ = ~(ਨਿਵੇਸ਼_2[0].value(0) ^ ਨਿਵੇਸ਼_2[1].value(0));
      vvp_vector4_t ਨਤੀਜਾ (1, ਦੋਆਧਾਰੀ_ਅੰਕ);

      ptr->send_vec4(ਨਤੀਜਾ, 0);
}

vvp_fun_impl::vvp_fun_impl()
: vvp_fun_boolean_(1)
{
      ਗਿਣਤੀ_functors_logic += 1;
}

vvp_fun_impl::~vvp_fun_impl()
{
}

ਖਾਲੀ vvp_fun_impl::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਵੇਸ਼_2[0].size() == 1);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਵੇਸ਼_2[1].size() == 1);

      vvp_bit4_t ਦੋਆਧਾਰੀ_ਅੰਕ = ~ਨਿਵੇਸ਼_2[0].value(0) | ਨਿਵੇਸ਼_2[1].value(0);
      vvp_vector4_t ਨਤੀਜਾ (1, ਦੋਆਧਾਰੀ_ਅੰਕ);

      ptr->send_vec4(ਨਤੀਜਾ, 0);
}

vvp_fun_buf_not_::vvp_fun_buf_not_(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: ਨਿਵੇਸ਼_2(wid, BIT4_Z)
{
      net_ = 0;
}

vvp_fun_buf_not_::~vvp_fun_buf_not_()
{
}

ਖਾਲੀ vvp_fun_buf_not_::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                 vvp_context_t)
{
      ਜੇ (ptr.ਪੋਰਟ() != 0) ਵਾਪਸ;

      ਜੇ (ਨਿਵੇਸ਼_2 .eeq( ਦੋਆਧਾਰੀ_ਅੰਕ )) ਵਾਪਸ;

      ਨਿਵੇਸ਼_2 = ਦੋਆਧਾਰੀ_ਅੰਕ;
      ਜੇ (net_ == 0) {
	    net_ = ptr.ptr();
	    ਸਮਾਂ_ਸਾਰਣੀ_functor(ਇਹ);
      }
}

ਖਾਲੀ vvp_fun_buf_not_::recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t)
{
      ਜੇ (ptr.ਪੋਰਟ() != 0) ਵਾਪਸ;

      ਪ੍ਰਤਿਜ੍ਞਾ(ਅਧਾਰ + ਦੋਆਧਾਰੀ_ਅੰਕ.size() <= vwid);

	// Set the input part. If nothing changes, then break.
      ਤਾਰਕਿਕ flag = ਨਿਵੇਸ਼_2.ਸੈੱਟ_vec(ਅਧਾਰ, ਦੋਆਧਾਰੀ_ਅੰਕ);
      ਜੇ (flag == ਝੂਠ) ਵਾਪਸ;

      ਜੇ (net_ == 0) {
	    net_ = ptr.ptr();
	    ਸਮਾਂ_ਸਾਰਣੀ_functor(ਇਹ);
      }
}

ਖਾਲੀ vvp_fun_buf_not_::recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ real,
                                 vvp_context_t ctx)
{
      ਜੇ (ptr.ਪੋਰਟ() != 0) ਵਾਪਸ;

      recv_vec4(ptr, double_ਮੰਜ਼ਿਲ_vector4_LSB(real), ctx);
}

vvp_fun_buf::vvp_fun_buf(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: vvp_fun_buf_not_(wid)
{
      ਗਿਣਤੀ_functors_logic += 1;
}

vvp_fun_buf::~vvp_fun_buf()
{
}

/*
 * The buf functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the result.
 */
ਖਾਲੀ vvp_fun_buf::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t tmp (ਨਿਵੇਸ਼_2);
      tmp.change_z2x();
      ptr->send_vec4(tmp, 0);
}

vvp_fun_bufz::vvp_fun_bufz()
{
      ਗਿਣਤੀ_functors_logic += 1;
}

vvp_fun_bufz::~vvp_fun_bufz()
{
}

/*
 * The bufz is similar to the buf device, except that it does not
 * bother translating z bits to x.
 */
ਖਾਲੀ vvp_fun_bufz::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                             vvp_context_t)
{
      ਜੇ (ptr.ਪੋਰਟ() != 0)
	    ਵਾਪਸ;

      ptr.ptr()->send_vec4(ਦੋਆਧਾਰੀ_ਅੰਕ, 0);
}

ਖਾਲੀ vvp_fun_bufz::recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t)
{
      ਜੇ (ptr.ਪੋਰਟ() != 0)
	    ਵਾਪਸ;

      ptr.ptr()->send_vec4_pv(ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਧਾਰ, vwid, 0);
}

ਖਾਲੀ vvp_fun_bufz::recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                             vvp_context_t)
{
      ਜੇ (ptr.ਪੋਰਟ() != 0)
	    ਵਾਪਸ;

      ptr.ptr()->send_real(ਦੋਆਧਾਰੀ_ਅੰਕ, 0);
}

ਖਾਲੀ vvp_fun_buft::recv_vec8(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਜੇ (ptr.ਪੋਰਟ() != 0)
	    ਵਾਪਸ;

      ptr.ptr()->send_vec8(ਦੋਆਧਾਰੀ_ਅੰਕ);
}

vvp_fun_muxr::vvp_fun_muxr()
: a_(0.0), b_(0.0)
{
      net_ = 0;
      ਗਿਣਤੀ_functors_logic += 1;
      select_ = SEL_BOTH;
}

vvp_fun_muxr::~vvp_fun_muxr()
{
}

ਖਾਲੀ vvp_fun_muxr::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                             vvp_context_t)
{
	/* The real valued mux can only take in the select as a
	   vector4_t. The muxed data is real. */
      ਜੇ (ptr.ਪੋਰਟ() != 2)
	    ਵਾਪਸ;

      ਪ੍ਰਤਿਜ੍ਞਾ(ਦੋਆਧਾਰੀ_ਅੰਕ.size() == 1);

      ਚੋਣ (ਦੋਆਧਾਰੀ_ਅੰਕ.value(0)) {
	  ਹਾਲਤ BIT4_0:
	    ਜੇ (select_ == SEL_PORT0) ਵਾਪਸ;
	    select_ = SEL_PORT0;
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_1:
	    ਜੇ (select_ == SEL_PORT1) ਵਾਪਸ;
	    select_ = SEL_PORT1;
	    ਰੋਕੋ;
	  ਮੂਲ:
	    ਜੇ (select_ == SEL_BOTH) ਵਾਪਸ;
	    select_ = SEL_BOTH;
      }

      ਜੇ (net_ == 0) {
	    net_ = ptr.ptr();
	    ਸਮਾਂ_ਸਾਰਣੀ_functor(ਇਹ);
      }
}

ਖਾਲੀ vvp_fun_muxr::recv_real(vvp_net_ptr_t ptr, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                             vvp_context_t)
{
      ਚੋਣ (ptr.ਪੋਰਟ()) {
	  ਹਾਲਤ 0:
	    ਜੇ (a_ == ਦੋਆਧਾਰੀ_ਅੰਕ) ਵਾਪਸ;
	    a_ = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਜੇ (select_ == SEL_PORT1) ਵਾਪਸ; // The other port is selected.
	    ਰੋਕੋ;

	  ਹਾਲਤ 1:
	    ਜੇ (b_ == ਦੋਆਧਾਰੀ_ਅੰਕ) ਵਾਪਸ;
	    b_ = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਜੇ (select_ == SEL_PORT0) ਵਾਪਸ; // The other port is selected.
	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.ਪੋਰਟ());
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }

      ਜੇ (net_ == 0) {
	    net_ = ptr.ptr();
	    ਸਮਾਂ_ਸਾਰਣੀ_functor(ਇਹ);
      }
}

ਖਾਲੀ vvp_fun_muxr::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      ਚੋਣ (select_) {
	  ਹਾਲਤ SEL_PORT0:
	    ptr->send_real(a_, 0);
	    ਰੋਕੋ;
	  ਹਾਲਤ SEL_PORT1:
	    ptr->send_real(b_, 0);
	    ਰੋਕੋ;
	  ਮੂਲ:
	    ਜੇ (a_ == b_) {
		  ptr->send_real(a_, 0);
	    } ਨਹੀਂਤਾਂ {
		  ptr->send_real(0.0, 0); // Should this be NaN?
	    }
	    ਰੋਕੋ;
      }
}

vvp_fun_muxz::vvp_fun_muxz(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: a_(wid, BIT4_Z), b_(wid, BIT4_Z)
{
      net_ = 0;
      ਗਿਣਤੀ_functors_logic += 1;
      select_ = SEL_BOTH;
      has_ਲੜੀਵਾਰ = ਝੂਠ;
}

vvp_fun_muxz::~vvp_fun_muxz()
{
}

ਖਾਲੀ vvp_fun_muxz::recv_vec4(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                             vvp_context_t)
{
      ਚੋਣ (ptr.ਪੋਰਟ()) {
	  ਹਾਲਤ 0:
	    ਜੇ (a_ .eeq(ਦੋਆਧਾਰੀ_ਅੰਕ) && has_ਲੜੀਵਾਰ) ਵਾਪਸ;
	    a_ = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਜੇ (select_ == SEL_PORT1) ਵਾਪਸ; // The other port is selected.
	    ਰੋਕੋ;
	  ਹਾਲਤ 1:
	    ਜੇ (b_ .eeq(ਦੋਆਧਾਰੀ_ਅੰਕ) && has_ਲੜੀਵਾਰ) ਵਾਪਸ;
	    b_ = ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਜੇ (select_ == SEL_PORT0) ਵਾਪਸ; // The other port is selected.
	    ਰੋਕੋ;
	  ਹਾਲਤ 2:
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਦੋਆਧਾਰੀ_ਅੰਕ.size() == 1);
	    ਚੋਣ (ਦੋਆਧਾਰੀ_ਅੰਕ.value(0)) {
		ਹਾਲਤ BIT4_0:
		  ਜੇ (select_ == SEL_PORT0) ਵਾਪਸ;
		  select_ = SEL_PORT0;
		  ਰੋਕੋ;
		ਹਾਲਤ BIT4_1:
		  ਜੇ (select_ == SEL_PORT1) ਵਾਪਸ;
		  select_ = SEL_PORT1;
		  ਰੋਕੋ;
		ਮੂਲ:
		  ਜੇ (select_ == SEL_BOTH && has_ਲੜੀਵਾਰ) ਵਾਪਸ;
		  select_ = SEL_BOTH;
	    }
	    ਰੋਕੋ;
	  ਮੂਲ:
	    ਵਾਪਸ;
      }

      ਜੇ (net_ == 0) {
	    net_ = ptr.ptr();
	    ਸਮਾਂ_ਸਾਰਣੀ_functor(ਇਹ);
      }
}

ਖਾਲੀ vvp_fun_muxz::recv_vec4_pv(vvp_net_ptr_t ptr, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(ਅਧਾਰ + ਦੋਆਧਾਰੀ_ਅੰਕ.size() <= vwid);
      ਤਾਰਕਿਕ flag;

      ਚੋਣ (ptr.ਪੋਰਟ()) {
	  ਹਾਲਤ 0:
	    flag = a_.ਸੈੱਟ_vec(ਅਧਾਰ, ਦੋਆਧਾਰੀ_ਅੰਕ);
	    ਜੇ (flag == ਝੂਠ && has_ਲੜੀਵਾਰ) ਵਾਪਸ;
	    ਜੇ (select_ == SEL_PORT1) ਵਾਪਸ; // The other port is selected.
	    ਰੋਕੋ;
	  ਹਾਲਤ 1:
	    flag = b_.ਸੈੱਟ_vec(ਅਧਾਰ, ਦੋਆਧਾਰੀ_ਅੰਕ);
	    ਜੇ (flag == ਝੂਠ && has_ਲੜੀਵਾਰ) ਵਾਪਸ;
	    ਜੇ (select_ == SEL_PORT0) ਵਾਪਸ; // The other port is selected.
	    ਰੋਕੋ;
	  ਹਾਲਤ 2:
	    ਪ੍ਰਤਿਜ੍ਞਾ((ਅਧਾਰ == 0) && (ਦੋਆਧਾਰੀ_ਅੰਕ.size() == 1));
	    recv_vec4(ptr, ਦੋਆਧਾਰੀ_ਅੰਕ, 0);
	  ਮੂਲ:
	    ਵਾਪਸ;
      }
      ਜੇ (net_ == 0) {
	    net_ = ptr.ptr();
	    ਸਮਾਂ_ਸਾਰਣੀ_functor(ਇਹ);
      }
}

ਖਾਲੀ vvp_fun_muxz::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      has_ਲੜੀਵਾਰ = ਸੱਚ;
      vvp_net_t*ptr = net_;
      net_ = 0;

      ਚੋਣ (select_) {
	  ਹਾਲਤ SEL_PORT0:
	    ptr->send_vec4(a_, 0);
	    ਰੋਕੋ;
	  ਹਾਲਤ SEL_PORT1:
	    ptr->send_vec4(b_, 0);
	    ਰੋਕੋ;
	  ਮੂਲ:
	      {
		    ਬਿਨਾਨਿਸ਼ਾਨੀ min_ਮਾਪ = a_.size();
		    ਬਿਨਾਨਿਸ਼ਾਨੀ max_ਮਾਪ = min_ਮਾਪ;
		    ਜੇ (b_.size() < min_ਮਾਪ)
			  min_ਮਾਪ = b_.size();
		    ਜੇ (b_.size() > max_ਮਾਪ)
			  max_ਮਾਪ = b_.size();

		    vvp_vector4_t res (max_ਮਾਪ);

		    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < min_ਮਾਪ ;  idx += 1) {
			  ਜੇ (a_.value(idx) == b_.value(idx))
				res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, a_.value(idx));
			  ਨਹੀਂਤਾਂ
				res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_X);
		    }

		    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = min_ਮਾਪ ;  idx < max_ਮਾਪ ;  idx += 1)
			  res.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, BIT4_X);

		    ptr->send_vec4(res, 0);
	      }
	    ਰੋਕੋ;
      }
}

vvp_fun_not::vvp_fun_not(ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
: vvp_fun_buf_not_(wid)
{
      ਗਿਣਤੀ_functors_logic += 1;
}

vvp_fun_not::~vvp_fun_not()
{
}

/*
 * The not functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the inverted result.
 */
ਖਾਲੀ vvp_fun_not::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t ਨਤੀਜਾ (ਨਿਵੇਸ਼_2, ਸੱਚ /* invert */);
      ptr->send_vec4(ਨਤੀਜਾ, 0);
}

vvp_fun_or::vvp_fun_or(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਤਾਰਕਿਕ invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      ਗਿਣਤੀ_functors_logic += 1;
}

vvp_fun_or::~vvp_fun_or()
{
}

ਖਾਲੀ vvp_fun_or::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t ਨਤੀਜਾ (ਨਿਵੇਸ਼_2[0]);

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ pdx = 1 ; pdx < 4 ; pdx += 1)
	    ਨਤੀਜਾ |= ਨਿਵੇਸ਼_2[pdx];

      ਜੇ (invert_)
	    ਨਤੀਜਾ.invert();

      ptr->send_vec4(ਨਤੀਜਾ, 0);
}

vvp_fun_xor::vvp_fun_xor(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਤਾਰਕਿਕ invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      ਗਿਣਤੀ_functors_logic += 1;
}

vvp_fun_xor::~vvp_fun_xor()
{
}

ਖਾਲੀ vvp_fun_xor::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t ਨਤੀਜਾ (ਨਿਵੇਸ਼_2[0]);

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ pdx = 1 ; pdx < 4 ; pdx += 1)
	    ਨਤੀਜਾ ^= ਨਿਵੇਸ਼_2[pdx];

      ਜੇ (invert_)
	    ਨਤੀਜਾ.invert();

      ptr->send_vec4(ਨਤੀਜਾ, 0);
}

/*
 * The parser calls this function to create a logic functor. I allocate a
 * functor, and map the name to the vvp_ipoint_t address for the
 * functor. Also resolve the inputs to the functor.
 */

ਖਾਲੀ compile_functor(ਅੱਖਰ*label, ਅੱਖਰ*type, ਬਿਨਾਨਿਸ਼ਾਨੀ width,
		     ਬਿਨਾਨਿਸ਼ਾਨੀ ostr0, ਬਿਨਾਨਿਸ਼ਾਨੀ ostr1,
		     ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      vvp_net_fun_t* obj = 0;
      ਤਾਰਕਿਕ strength_aware = ਝੂਠ;

      ਜੇ (strcmp(type, "OR") == 0) {
	    obj = ਨਵਾਂ vvp_fun_or(width, ਝੂਠ);

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "AND") == 0) {
	    obj = ਨਵਾਂ vvp_fun_and(width, ਝੂਠ);

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "BUF") == 0) {
	    obj = ਨਵਾਂ vvp_fun_buf(width);

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "BUFIF0") == 0) {
	    obj = ਨਵਾਂ vvp_fun_bufif(ਸੱਚ,ਝੂਠ, ostr0, ostr1);
	    strength_aware = ਸੱਚ;

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "BUFIF1") == 0) {
	    obj = ਨਵਾਂ vvp_fun_bufif(ਝੂਠ,ਝੂਠ, ostr0, ostr1);
	    strength_aware = ਸੱਚ;

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "EQUIV") == 0) {
	    obj = ਨਵਾਂ vvp_fun_equiv();

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "IMPL") == 0) {
	    obj = ਨਵਾਂ vvp_fun_impl();

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "NAND") == 0) {
	    obj = ਨਵਾਂ vvp_fun_and(width, ਸੱਚ);

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "NOR") == 0) {
	    obj = ਨਵਾਂ vvp_fun_or(width, ਸੱਚ);

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "NOTIF0") == 0) {
	    obj = ਨਵਾਂ vvp_fun_bufif(ਸੱਚ,ਸੱਚ, ostr0, ostr1);
	    strength_aware = ਸੱਚ;

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "NOTIF1") == 0) {
	    obj = ਨਵਾਂ vvp_fun_bufif(ਝੂਠ,ਸੱਚ, ostr0, ostr1);
	    strength_aware = ਸੱਚ;

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "BUFT") == 0) {
	    obj = ਨਵਾਂ vvp_fun_buft();

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "BUFZ") == 0) {
	    obj = ਨਵਾਂ vvp_fun_bufz();

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "MUXR") == 0) {
	    obj = ਨਵਾਂ vvp_fun_muxr;

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "MUXZ") == 0) {
	    obj = ਨਵਾਂ vvp_fun_muxz(width);

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "CMOS") == 0) {
	    obj = ਨਵਾਂ vvp_fun_cmos();

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "NMOS") == 0) {
	    obj = ਨਵਾਂ vvp_fun_pmos(ਸੱਚ);

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "PMOS") == 0) {
	    obj = ਨਵਾਂ vvp_fun_pmos(ਝੂਠ);

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "RCMOS") == 0) {
	    obj = ਨਵਾਂ vvp_fun_rcmos();

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "RNMOS") == 0) {
	    obj = ਨਵਾਂ vvp_fun_rpmos(ਸੱਚ);

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "RPMOS") == 0) {
	    obj = ਨਵਾਂ vvp_fun_rpmos(ਝੂਠ);

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "NOT") == 0) {
	    obj = ਨਵਾਂ vvp_fun_not(width);

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "XNOR") == 0) {
	    obj = ਨਵਾਂ vvp_fun_xor(width, ਸੱਚ);

      } ਨਹੀਂਤਾਂ ਜੇ (strcmp(type, "XOR") == 0) {
	    obj = ਨਵਾਂ vvp_fun_xor(width, ਝੂਠ);

      } ਨਹੀਂਤਾਂ {
	    yyerror("invalid functor type.");
	    ਮੁਕਤ(type);
	    ਮੁਕਤ(argv);
	    ਮੁਕਤ(label);
	    ਵਾਪਸ;
      }

      ਮੁਕਤ(type);

      ਪ੍ਰਤਿਜ੍ਞਾ(argc <= 4);
      vvp_net_t*net = ਨਵਾਂ vvp_net_t;
      net->fun = obj;

      inputs_connect(net, argc, argv);
      ਮੁਕਤ(argv);

	/* If both the strengths are the default strong drive, then
	   there is no need for a specialized driver. Attach the label
	   to this node and we are finished. */
      ਜੇ (strength_aware || (ostr0 == 6 && ostr1 == 6)) {
	    define_functor_ਚਿੰਨ੍ਹ(label, net);
	    ਮੁਕਤ(label);
	    ਵਾਪਸ;
      }

      vvp_net_t*net_drv = ਨਵਾਂ vvp_net_t;
      vvp_net_fun_t*obj_drv = ਨਵਾਂ vvp_fun_drive(ostr0, ostr1);
      net_drv->fun = obj_drv;

	/* Point the gate to the drive node. */
      net->link(vvp_net_ptr_t(net_drv, 0));

      define_functor_ਚਿੰਨ੍ਹ(label, net_drv);
      ਮੁਕਤ(label);
}
