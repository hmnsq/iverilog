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

vvp_fun_boolean_::vvp_fun_boolean_(ಚಿಹ್ನೆರಹಿತ wid)
{
      net_ = 0;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < 4 ;  idx += 1)
	    ಒಳಹರಿವು_2[idx] = vvp_vector4_t(wid, BIT4_Z);
}

vvp_fun_boolean_::~vvp_fun_boolean_()
{
}

ಶೂನ್ಯ vvp_fun_boolean_::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                 vvp_context_t)
{
      ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್ = ptr.ಪೋರ್ಟ್();
      ಆದರೆ (ಒಳಹರಿವು_2[ಪೋರ್ಟ್] .eeq( ಬಿಟ್ )) ಹಿಂತಿರುಗಿ;

      ಒಳಹರಿವು_2[ಪೋರ್ಟ್] = ಬಿಟ್;
      ಆದರೆ (net_ == 0) {
	    net_ = ptr.ptr();
	    ವೇಳಾಪಟ್ಟಿ_functor(ಇದು);
      }
}

ಶೂನ್ಯ vvp_fun_boolean_::recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				    ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t)
{
      ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್ = ptr.ಪೋರ್ಟ್();

      ಪ್ರತಿಜ್ಞಾ(ತಳ + ಬಿಟ್.size() <= vwid);

	// Set the part for the input. If nothing changes, then break.
      ತಾರ್ಕಿಕ flag = ಒಳಹರಿವು_2[ಪೋರ್ಟ್] .ಗಣ_vec(ತಳ, ಬಿಟ್);
      ಆದರೆ (flag == ಸುಳ್ಳು)
	    ಹಿಂತಿರುಗಿ;

      ಆದರೆ (net_ == 0) {
	    net_ = ptr.ptr();
	    ವೇಳಾಪಟ್ಟಿ_functor(ಇದು);
      }
}

ಶೂನ್ಯ vvp_fun_boolean_::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ real,
                                 vvp_context_t ctx)
{
      recv_vec4(ptr, double_ಗಮ್ಯ_vector4_LSB(real), ctx);
}

vvp_fun_and::vvp_fun_and(ಚಿಹ್ನೆರಹಿತ wid, ತಾರ್ಕಿಕ invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      ಎಣಿಕೆ_functors_logic += 1;
}

vvp_fun_and::~vvp_fun_and()
{
}

ಶೂನ್ಯ vvp_fun_and::ಸತತ_ಸತತ()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t ಫಲಿತಾಂಶ (ಒಳಹರಿವು_2[0]);

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ pdx = 1 ; pdx < 4 ; pdx += 1)
	    ಫಲಿತಾಂಶ &= ಒಳಹರಿವು_2[pdx];

      ಆದರೆ (invert_)
	    ಫಲಿತಾಂಶ.invert();

      ptr->send_vec4(ಫಲಿತಾಂಶ, 0);
}

vvp_fun_equiv::vvp_fun_equiv()
: vvp_fun_boolean_(1)
{
      ಎಣಿಕೆ_functors_logic += 1;
}

vvp_fun_equiv::~vvp_fun_equiv()
{
}

ಶೂನ್ಯ vvp_fun_equiv::ಸತತ_ಸತತ()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      ಪ್ರತಿಜ್ಞಾ(ಒಳಹರಿವು_2[0].size() == 1);
      ಪ್ರತಿಜ್ಞಾ(ಒಳಹರಿವು_2[1].size() == 1);

      vvp_bit4_t ಬಿಟ್ = ~(ಒಳಹರಿವು_2[0].value(0) ^ ಒಳಹರಿವು_2[1].value(0));
      vvp_vector4_t ಫಲಿತಾಂಶ (1, ಬಿಟ್);

      ptr->send_vec4(ಫಲಿತಾಂಶ, 0);
}

vvp_fun_impl::vvp_fun_impl()
: vvp_fun_boolean_(1)
{
      ಎಣಿಕೆ_functors_logic += 1;
}

vvp_fun_impl::~vvp_fun_impl()
{
}

ಶೂನ್ಯ vvp_fun_impl::ಸತತ_ಸತತ()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      ಪ್ರತಿಜ್ಞಾ(ಒಳಹರಿವು_2[0].size() == 1);
      ಪ್ರತಿಜ್ಞಾ(ಒಳಹರಿವು_2[1].size() == 1);

      vvp_bit4_t ಬಿಟ್ = ~ಒಳಹರಿವು_2[0].value(0) | ಒಳಹರಿವು_2[1].value(0);
      vvp_vector4_t ಫಲಿತಾಂಶ (1, ಬಿಟ್);

      ptr->send_vec4(ಫಲಿತಾಂಶ, 0);
}

vvp_fun_buf_not_::vvp_fun_buf_not_(ಚಿಹ್ನೆರಹಿತ wid)
: ಒಳಹರಿವು_2(wid, BIT4_Z)
{
      net_ = 0;
}

vvp_fun_buf_not_::~vvp_fun_buf_not_()
{
}

ಶೂನ್ಯ vvp_fun_buf_not_::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                 vvp_context_t)
{
      ಆದರೆ (ptr.ಪೋರ್ಟ್() != 0) ಹಿಂತಿರುಗಿ;

      ಆದರೆ (ಒಳಹರಿವು_2 .eeq( ಬಿಟ್ )) ಹಿಂತಿರುಗಿ;

      ಒಳಹರಿವು_2 = ಬಿಟ್;
      ಆದರೆ (net_ == 0) {
	    net_ = ptr.ptr();
	    ವೇಳಾಪಟ್ಟಿ_functor(ಇದು);
      }
}

ಶೂನ್ಯ vvp_fun_buf_not_::recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                    ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t)
{
      ಆದರೆ (ptr.ಪೋರ್ಟ್() != 0) ಹಿಂತಿರುಗಿ;

      ಪ್ರತಿಜ್ಞಾ(ತಳ + ಬಿಟ್.size() <= vwid);

	// Set the input part. If nothing changes, then break.
      ತಾರ್ಕಿಕ flag = ಒಳಹರಿವು_2.ಗಣ_vec(ತಳ, ಬಿಟ್);
      ಆದರೆ (flag == ಸುಳ್ಳು) ಹಿಂತಿರುಗಿ;

      ಆದರೆ (net_ == 0) {
	    net_ = ptr.ptr();
	    ವೇಳಾಪಟ್ಟಿ_functor(ಇದು);
      }
}

ಶೂನ್ಯ vvp_fun_buf_not_::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ real,
                                 vvp_context_t ctx)
{
      ಆದರೆ (ptr.ಪೋರ್ಟ್() != 0) ಹಿಂತಿರುಗಿ;

      recv_vec4(ptr, double_ಗಮ್ಯ_vector4_LSB(real), ctx);
}

vvp_fun_buf::vvp_fun_buf(ಚಿಹ್ನೆರಹಿತ wid)
: vvp_fun_buf_not_(wid)
{
      ಎಣಿಕೆ_functors_logic += 1;
}

vvp_fun_buf::~vvp_fun_buf()
{
}

/*
 * The buf functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the result.
 */
ಶೂನ್ಯ vvp_fun_buf::ಸತತ_ಸತತ()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t tmp (ಒಳಹರಿವು_2);
      tmp.change_z2x();
      ptr->send_vec4(tmp, 0);
}

vvp_fun_bufz::vvp_fun_bufz()
{
      ಎಣಿಕೆ_functors_logic += 1;
}

vvp_fun_bufz::~vvp_fun_bufz()
{
}

/*
 * The bufz is similar to the buf device, except that it does not
 * bother translating z bits to x.
 */
ಶೂನ್ಯ vvp_fun_bufz::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                             vvp_context_t)
{
      ಆದರೆ (ptr.ಪೋರ್ಟ್() != 0)
	    ಹಿಂತಿರುಗಿ;

      ptr.ptr()->send_vec4(ಬಿಟ್, 0);
}

ಶೂನ್ಯ vvp_fun_bufz::recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t)
{
      ಆದರೆ (ptr.ಪೋರ್ಟ್() != 0)
	    ಹಿಂತಿರುಗಿ;

      ptr.ptr()->send_vec4_pv(ಬಿಟ್, ತಳ, vwid, 0);
}

ಶೂನ್ಯ vvp_fun_bufz::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ ಬಿಟ್,
                             vvp_context_t)
{
      ಆದರೆ (ptr.ಪೋರ್ಟ್() != 0)
	    ಹಿಂತಿರುಗಿ;

      ptr.ptr()->send_real(ಬಿಟ್, 0);
}

ಶೂನ್ಯ vvp_fun_buft::recv_vec8(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್)
{
      ಆದರೆ (ptr.ಪೋರ್ಟ್() != 0)
	    ಹಿಂತಿರುಗಿ;

      ptr.ptr()->send_vec8(ಬಿಟ್);
}

vvp_fun_muxr::vvp_fun_muxr()
: a_(0.0), b_(0.0)
{
      net_ = 0;
      ಎಣಿಕೆ_functors_logic += 1;
      select_ = SEL_BOTH;
}

vvp_fun_muxr::~vvp_fun_muxr()
{
}

ಶೂನ್ಯ vvp_fun_muxr::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                             vvp_context_t)
{
	/* The real valued mux can only take in the select as a
	   vector4_t. The muxed data is real. */
      ಆದರೆ (ptr.ಪೋರ್ಟ್() != 2)
	    ಹಿಂತಿರುಗಿ;

      ಪ್ರತಿಜ್ಞಾ(ಬಿಟ್.size() == 1);

      ಆಯ್ಕೆ (ಬಿಟ್.value(0)) {
	  ಸ್ಥಿತಿ BIT4_0:
	    ಆದರೆ (select_ == SEL_PORT0) ಹಿಂತಿರುಗಿ;
	    select_ = SEL_PORT0;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_1:
	    ಆದರೆ (select_ == SEL_PORT1) ಹಿಂತಿರುಗಿ;
	    select_ = SEL_PORT1;
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಆದರೆ (select_ == SEL_BOTH) ಹಿಂತಿರುಗಿ;
	    select_ = SEL_BOTH;
      }

      ಆದರೆ (net_ == 0) {
	    net_ = ptr.ptr();
	    ವೇಳಾಪಟ್ಟಿ_functor(ಇದು);
      }
}

ಶೂನ್ಯ vvp_fun_muxr::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ ಬಿಟ್,
                             vvp_context_t)
{
      ಆಯ್ಕೆ (ptr.ಪೋರ್ಟ್()) {
	  ಸ್ಥಿತಿ 0:
	    ಆದರೆ (a_ == ಬಿಟ್) ಹಿಂತಿರುಗಿ;
	    a_ = ಬಿಟ್;
	    ಆದರೆ (select_ == SEL_PORT1) ಹಿಂತಿರುಗಿ; // The other port is selected.
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ 1:
	    ಆದರೆ (b_ == ಬಿಟ್) ಹಿಂತಿರುಗಿ;
	    b_ = ಬಿಟ್;
	    ಆದರೆ (select_ == SEL_PORT0) ಹಿಂತಿರುಗಿ; // The other port is selected.
	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.ಪೋರ್ಟ್());
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      ಆದರೆ (net_ == 0) {
	    net_ = ptr.ptr();
	    ವೇಳಾಪಟ್ಟಿ_functor(ಇದು);
      }
}

ಶೂನ್ಯ vvp_fun_muxr::ಸತತ_ಸತತ()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      ಆಯ್ಕೆ (select_) {
	  ಸ್ಥಿತಿ SEL_PORT0:
	    ptr->send_real(a_, 0);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ SEL_PORT1:
	    ptr->send_real(b_, 0);
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಆದರೆ (a_ == b_) {
		  ptr->send_real(a_, 0);
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ptr->send_real(0.0, 0); // Should this be NaN?
	    }
	    ನಿಲ್ಲಿಸು;
      }
}

vvp_fun_muxz::vvp_fun_muxz(ಚಿಹ್ನೆರಹಿತ wid)
: a_(wid, BIT4_Z), b_(wid, BIT4_Z)
{
      net_ = 0;
      ಎಣಿಕೆ_functors_logic += 1;
      select_ = SEL_BOTH;
      has_ಸತತ = ಸುಳ್ಳು;
}

vvp_fun_muxz::~vvp_fun_muxz()
{
}

ಶೂನ್ಯ vvp_fun_muxz::recv_vec4(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                             vvp_context_t)
{
      ಆಯ್ಕೆ (ptr.ಪೋರ್ಟ್()) {
	  ಸ್ಥಿತಿ 0:
	    ಆದರೆ (a_ .eeq(ಬಿಟ್) && has_ಸತತ) ಹಿಂತಿರುಗಿ;
	    a_ = ಬಿಟ್;
	    ಆದರೆ (select_ == SEL_PORT1) ಹಿಂತಿರುಗಿ; // The other port is selected.
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 1:
	    ಆದರೆ (b_ .eeq(ಬಿಟ್) && has_ಸತತ) ಹಿಂತಿರುಗಿ;
	    b_ = ಬಿಟ್;
	    ಆದರೆ (select_ == SEL_PORT0) ಹಿಂತಿರುಗಿ; // The other port is selected.
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 2:
	    ಪ್ರತಿಜ್ಞಾ(ಬಿಟ್.size() == 1);
	    ಆಯ್ಕೆ (ಬಿಟ್.value(0)) {
		ಸ್ಥಿತಿ BIT4_0:
		  ಆದರೆ (select_ == SEL_PORT0) ಹಿಂತಿರುಗಿ;
		  select_ = SEL_PORT0;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ BIT4_1:
		  ಆದರೆ (select_ == SEL_PORT1) ಹಿಂತಿರುಗಿ;
		  select_ = SEL_PORT1;
		  ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		  ಆದರೆ (select_ == SEL_BOTH && has_ಸತತ) ಹಿಂತಿರುಗಿ;
		  select_ = SEL_BOTH;
	    }
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ;
      }

      ಆದರೆ (net_ == 0) {
	    net_ = ptr.ptr();
	    ವೇಳಾಪಟ್ಟಿ_functor(ಇದು);
      }
}

ಶೂನ್ಯ vvp_fun_muxz::recv_vec4_pv(vvp_net_ptr_t ptr, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t)
{
      ಪ್ರತಿಜ್ಞಾ(ತಳ + ಬಿಟ್.size() <= vwid);
      ತಾರ್ಕಿಕ flag;

      ಆಯ್ಕೆ (ptr.ಪೋರ್ಟ್()) {
	  ಸ್ಥಿತಿ 0:
	    flag = a_.ಗಣ_vec(ತಳ, ಬಿಟ್);
	    ಆದರೆ (flag == ಸುಳ್ಳು && has_ಸತತ) ಹಿಂತಿರುಗಿ;
	    ಆದರೆ (select_ == SEL_PORT1) ಹಿಂತಿರುಗಿ; // The other port is selected.
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 1:
	    flag = b_.ಗಣ_vec(ತಳ, ಬಿಟ್);
	    ಆದರೆ (flag == ಸುಳ್ಳು && has_ಸತತ) ಹಿಂತಿರುಗಿ;
	    ಆದರೆ (select_ == SEL_PORT0) ಹಿಂತಿರುಗಿ; // The other port is selected.
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 2:
	    ಪ್ರತಿಜ್ಞಾ((ತಳ == 0) && (ಬಿಟ್.size() == 1));
	    recv_vec4(ptr, ಬಿಟ್, 0);
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ;
      }
      ಆದರೆ (net_ == 0) {
	    net_ = ptr.ptr();
	    ವೇಳಾಪಟ್ಟಿ_functor(ಇದು);
      }
}

ಶೂನ್ಯ vvp_fun_muxz::ಸತತ_ಸತತ()
{
      has_ಸತತ = ನಿಜ;
      vvp_net_t*ptr = net_;
      net_ = 0;

      ಆಯ್ಕೆ (select_) {
	  ಸ್ಥಿತಿ SEL_PORT0:
	    ptr->send_vec4(a_, 0);
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ SEL_PORT1:
	    ptr->send_vec4(b_, 0);
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	      {
		    ಚಿಹ್ನೆರಹಿತ min_ಗಾತ್ರ = a_.size();
		    ಚಿಹ್ನೆರಹಿತ max_ಗಾತ್ರ = min_ಗಾತ್ರ;
		    ಆದರೆ (b_.size() < min_ಗಾತ್ರ)
			  min_ಗಾತ್ರ = b_.size();
		    ಆದರೆ (b_.size() > max_ಗಾತ್ರ)
			  max_ಗಾತ್ರ = b_.size();

		    vvp_vector4_t res (max_ಗಾತ್ರ);

		    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < min_ಗಾತ್ರ ;  idx += 1) {
			  ಆದರೆ (a_.value(idx) == b_.value(idx))
				res.ಗಣ_ಬಿಟ್(idx, a_.value(idx));
			  ಇಲ್ಲದಿದ್ದರೆ
				res.ಗಣ_ಬಿಟ್(idx, BIT4_X);
		    }

		    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = min_ಗಾತ್ರ ;  idx < max_ಗಾತ್ರ ;  idx += 1)
			  res.ಗಣ_ಬಿಟ್(idx, BIT4_X);

		    ptr->send_vec4(res, 0);
	      }
	    ನಿಲ್ಲಿಸು;
      }
}

vvp_fun_not::vvp_fun_not(ಚಿಹ್ನೆರಹಿತ wid)
: vvp_fun_buf_not_(wid)
{
      ಎಣಿಕೆ_functors_logic += 1;
}

vvp_fun_not::~vvp_fun_not()
{
}

/*
 * The not functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the inverted result.
 */
ಶೂನ್ಯ vvp_fun_not::ಸತತ_ಸತತ()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t ಫಲಿತಾಂಶ (ಒಳಹರಿವು_2, ನಿಜ /* invert */);
      ptr->send_vec4(ಫಲಿತಾಂಶ, 0);
}

vvp_fun_or::vvp_fun_or(ಚಿಹ್ನೆರಹಿತ wid, ತಾರ್ಕಿಕ invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      ಎಣಿಕೆ_functors_logic += 1;
}

vvp_fun_or::~vvp_fun_or()
{
}

ಶೂನ್ಯ vvp_fun_or::ಸತತ_ಸತತ()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t ಫಲಿತಾಂಶ (ಒಳಹರಿವು_2[0]);

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ pdx = 1 ; pdx < 4 ; pdx += 1)
	    ಫಲಿತಾಂಶ |= ಒಳಹರಿವು_2[pdx];

      ಆದರೆ (invert_)
	    ಫಲಿತಾಂಶ.invert();

      ptr->send_vec4(ಫಲಿತಾಂಶ, 0);
}

vvp_fun_xor::vvp_fun_xor(ಚಿಹ್ನೆರಹಿತ wid, ತಾರ್ಕಿಕ invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      ಎಣಿಕೆ_functors_logic += 1;
}

vvp_fun_xor::~vvp_fun_xor()
{
}

ಶೂನ್ಯ vvp_fun_xor::ಸತತ_ಸತತ()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t ಫಲಿತಾಂಶ (ಒಳಹರಿವು_2[0]);

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ pdx = 1 ; pdx < 4 ; pdx += 1)
	    ಫಲಿತಾಂಶ ^= ಒಳಹರಿವು_2[pdx];

      ಆದರೆ (invert_)
	    ಫಲಿತಾಂಶ.invert();

      ptr->send_vec4(ಫಲಿತಾಂಶ, 0);
}

/*
 * The parser calls this function to create a logic functor. I allocate a
 * functor, and map the name to the vvp_ipoint_t address for the
 * functor. Also resolve the inputs to the functor.
 */

ಶೂನ್ಯ compile_functor(ಅಕ್ಷರ*label, ಅಕ್ಷರ*type, ಚಿಹ್ನೆರಹಿತ width,
		     ಚಿಹ್ನೆರಹಿತ ostr0, ಚಿಹ್ನೆರಹಿತ ostr1,
		     ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      vvp_net_fun_t* obj = 0;
      ತಾರ್ಕಿಕ strength_aware = ಸುಳ್ಳು;

      ಆದರೆ (strcmp(type, "OR") == 0) {
	    obj = ಹೊಸ vvp_fun_or(width, ಸುಳ್ಳು);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "AND") == 0) {
	    obj = ಹೊಸ vvp_fun_and(width, ಸುಳ್ಳು);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "BUF") == 0) {
	    obj = ಹೊಸ vvp_fun_buf(width);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "BUFIF0") == 0) {
	    obj = ಹೊಸ vvp_fun_bufif(ನಿಜ,ಸುಳ್ಳು, ostr0, ostr1);
	    strength_aware = ನಿಜ;

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "BUFIF1") == 0) {
	    obj = ಹೊಸ vvp_fun_bufif(ಸುಳ್ಳು,ಸುಳ್ಳು, ostr0, ostr1);
	    strength_aware = ನಿಜ;

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "EQUIV") == 0) {
	    obj = ಹೊಸ vvp_fun_equiv();

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "IMPL") == 0) {
	    obj = ಹೊಸ vvp_fun_impl();

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "NAND") == 0) {
	    obj = ಹೊಸ vvp_fun_and(width, ನಿಜ);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "NOR") == 0) {
	    obj = ಹೊಸ vvp_fun_or(width, ನಿಜ);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "NOTIF0") == 0) {
	    obj = ಹೊಸ vvp_fun_bufif(ನಿಜ,ನಿಜ, ostr0, ostr1);
	    strength_aware = ನಿಜ;

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "NOTIF1") == 0) {
	    obj = ಹೊಸ vvp_fun_bufif(ಸುಳ್ಳು,ನಿಜ, ostr0, ostr1);
	    strength_aware = ನಿಜ;

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "BUFT") == 0) {
	    obj = ಹೊಸ vvp_fun_buft();

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "BUFZ") == 0) {
	    obj = ಹೊಸ vvp_fun_bufz();

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "MUXR") == 0) {
	    obj = ಹೊಸ vvp_fun_muxr;

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "MUXZ") == 0) {
	    obj = ಹೊಸ vvp_fun_muxz(width);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "CMOS") == 0) {
	    obj = ಹೊಸ vvp_fun_cmos();

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "NMOS") == 0) {
	    obj = ಹೊಸ vvp_fun_pmos(ನಿಜ);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "PMOS") == 0) {
	    obj = ಹೊಸ vvp_fun_pmos(ಸುಳ್ಳು);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "RCMOS") == 0) {
	    obj = ಹೊಸ vvp_fun_rcmos();

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "RNMOS") == 0) {
	    obj = ಹೊಸ vvp_fun_rpmos(ನಿಜ);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "RPMOS") == 0) {
	    obj = ಹೊಸ vvp_fun_rpmos(ಸುಳ್ಳು);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "NOT") == 0) {
	    obj = ಹೊಸ vvp_fun_not(width);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "XNOR") == 0) {
	    obj = ಹೊಸ vvp_fun_xor(width, ನಿಜ);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strcmp(type, "XOR") == 0) {
	    obj = ಹೊಸ vvp_fun_xor(width, ಸುಳ್ಳು);

      } ಇಲ್ಲದಿದ್ದರೆ {
	    yyerror("invalid functor type.");
	    ಬಿಡುಗಡೆ(type);
	    ಬಿಡುಗಡೆ(argv);
	    ಬಿಡುಗಡೆ(label);
	    ಹಿಂತಿರುಗಿ;
      }

      ಬಿಡುಗಡೆ(type);

      ಪ್ರತಿಜ್ಞಾ(argc <= 4);
      vvp_net_t*net = ಹೊಸ vvp_net_t;
      net->fun = obj;

      inputs_connect(net, argc, argv);
      ಬಿಡುಗಡೆ(argv);

	/* If both the strengths are the default strong drive, then
	   there is no need for a specialized driver. Attach the label
	   to this node and we are finished. */
      ಆದರೆ (strength_aware || (ostr0 == 6 && ostr1 == 6)) {
	    define_functor_ಚಿಹ್ನೆ(label, net);
	    ಬಿಡುಗಡೆ(label);
	    ಹಿಂತಿರುಗಿ;
      }

      vvp_net_t*net_drv = ಹೊಸ vvp_net_t;
      vvp_net_fun_t*obj_drv = ಹೊಸ vvp_fun_drive(ostr0, ostr1);
      net_drv->fun = obj_drv;

	/* Point the gate to the drive node. */
      net->link(vvp_net_ptr_t(net_drv, 0));

      define_functor_ಚಿಹ್ನೆ(label, net_drv);
      ಬಿಡುಗಡೆ(label);
}
