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

vvp_fun_boolean_::vvp_fun_boolean_(unsigned wid)
{
      net_ = 0;
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    ಒಳಹರಿವು_2[idx] = vvp_vector4_t(wid, BIT4_Z);
}

vvp_fun_boolean_::~vvp_fun_boolean_()
{
}

ಶೂನ್ಯ vvp_fun_boolean_::recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                                 vvp_context_t)
{
      unsigned port = ptr.port();
      ಯದಿ (ಒಳಹರಿವು_2[port] .eeq( bit )) ಪ್ರತ್ಯಾಗಮ;

      ಒಳಹರಿವು_2[port] = bit;
      ಯದಿ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ಏತತ್);
      }
}

ಶೂನ್ಯ vvp_fun_boolean_::recv_vec4_pv(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
				    unsigned base, unsigned vwid, vvp_context_t)
{
      unsigned port = ptr.port();

      ಪ್ರತಿಜ್ಞಾ(base + bit.size() <= vwid);

	// Set the part for the input. If nothing changes, then break.
      ತಾರ್ಕಿಕ flag = ಒಳಹರಿವು_2[port] .set_vec(base, bit);
      ಯದಿ (flag == ಅಸತ್ಯ)
	    ಪ್ರತ್ಯಾಗಮ;

      ಯದಿ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ಏತತ್);
      }
}

ಶೂನ್ಯ vvp_fun_boolean_::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ real,
                                 vvp_context_t ctx)
{
      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_and::vvp_fun_and(unsigned wid, ತಾರ್ಕಿಕ invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      ಎಣಿಕೆ_functors_logic += 1;
}

vvp_fun_and::~vvp_fun_and()
{
}

ಶೂನ್ಯ vvp_fun_and::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (ಒಳಹರಿವು_2[0]);

      ಪ್ರತಿ (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result &= ಒಳಹರಿವು_2[pdx];

      ಯದಿ (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_equiv::vvp_fun_equiv()
: vvp_fun_boolean_(1)
{
      ಎಣಿಕೆ_functors_logic += 1;
}

vvp_fun_equiv::~vvp_fun_equiv()
{
}

ಶೂನ್ಯ vvp_fun_equiv::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      ಪ್ರತಿಜ್ಞಾ(ಒಳಹರಿವು_2[0].size() == 1);
      ಪ್ರತಿಜ್ಞಾ(ಒಳಹರಿವು_2[1].size() == 1);

      vvp_bit4_t bit = ~(ಒಳಹರಿವು_2[0].value(0) ^ ಒಳಹರಿವು_2[1].value(0));
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_impl::vvp_fun_impl()
: vvp_fun_boolean_(1)
{
      ಎಣಿಕೆ_functors_logic += 1;
}

vvp_fun_impl::~vvp_fun_impl()
{
}

ಶೂನ್ಯ vvp_fun_impl::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      ಪ್ರತಿಜ್ಞಾ(ಒಳಹರಿವು_2[0].size() == 1);
      ಪ್ರತಿಜ್ಞಾ(ಒಳಹರಿವು_2[1].size() == 1);

      vvp_bit4_t bit = ~ಒಳಹರಿವು_2[0].value(0) | ಒಳಹರಿವು_2[1].value(0);
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_buf_not_::vvp_fun_buf_not_(unsigned wid)
: ಒಳಹರಿವು_2(wid, BIT4_Z)
{
      net_ = 0;
}

vvp_fun_buf_not_::~vvp_fun_buf_not_()
{
}

ಶೂನ್ಯ vvp_fun_buf_not_::recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                                 vvp_context_t)
{
      ಯದಿ (ptr.port() != 0) ಪ್ರತ್ಯಾಗಮ;

      ಯದಿ (ಒಳಹರಿವು_2 .eeq( bit )) ಪ್ರತ್ಯಾಗಮ;

      ಒಳಹರಿವು_2 = bit;
      ಯದಿ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ಏತತ್);
      }
}

ಶೂನ್ಯ vvp_fun_buf_not_::recv_vec4_pv(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                                    unsigned base, unsigned vwid, vvp_context_t)
{
      ಯದಿ (ptr.port() != 0) ಪ್ರತ್ಯಾಗಮ;

      ಪ್ರತಿಜ್ಞಾ(base + bit.size() <= vwid);

	// Set the input part. If nothing changes, then break.
      ತಾರ್ಕಿಕ flag = ಒಳಹರಿವು_2.set_vec(base, bit);
      ಯದಿ (flag == ಅಸತ್ಯ) ಪ್ರತ್ಯಾಗಮ;

      ಯದಿ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ಏತತ್);
      }
}

ಶೂನ್ಯ vvp_fun_buf_not_::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ real,
                                 vvp_context_t ctx)
{
      ಯದಿ (ptr.port() != 0) ಪ್ರತ್ಯಾಗಮ;

      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_buf::vvp_fun_buf(unsigned wid)
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
ಶೂನ್ಯ vvp_fun_buf::run_run()
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
ಶೂನ್ಯ vvp_fun_bufz::recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                             vvp_context_t)
{
      ಯದಿ (ptr.port() != 0)
	    ಪ್ರತ್ಯಾಗಮ;

      ptr.ptr()->send_vec4(bit, 0);
}

ಶೂನ್ಯ vvp_fun_bufz::recv_vec4_pv(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                                unsigned base, unsigned vwid, vvp_context_t)
{
      ಯದಿ (ptr.port() != 0)
	    ಪ್ರತ್ಯಾಗಮ;

      ptr.ptr()->send_vec4_pv(bit, base, vwid, 0);
}

ಶೂನ್ಯ vvp_fun_bufz::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ bit,
                             vvp_context_t)
{
      ಯದಿ (ptr.port() != 0)
	    ಪ್ರತ್ಯಾಗಮ;

      ptr.ptr()->send_real(bit, 0);
}

ಶೂನ್ಯ vvp_fun_buft::recv_vec8(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector8_t&bit)
{
      ಯದಿ (ptr.port() != 0)
	    ಪ್ರತ್ಯಾಗಮ;

      ptr.ptr()->send_vec8(bit);
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

ಶೂನ್ಯ vvp_fun_muxr::recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                             vvp_context_t)
{
	/* The real valued mux can only take in the select as a
	   vector4_t. The muxed data is real. */
      ಯದಿ (ptr.port() != 2)
	    ಪ್ರತ್ಯಾಗಮ;

      ಪ್ರತಿಜ್ಞಾ(bit.size() == 1);

      ವಿಕಲ್ಪ (bit.value(0)) {
	  ಪಕ್ಷ BIT4_0:
	    ಯದಿ (select_ == SEL_PORT0) ಪ್ರತ್ಯಾಗಮ;
	    select_ = SEL_PORT0;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ BIT4_1:
	    ಯದಿ (select_ == SEL_PORT1) ಪ್ರತ್ಯಾಗಮ;
	    select_ = SEL_PORT1;
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    ಯದಿ (select_ == SEL_BOTH) ಪ್ರತ್ಯಾಗಮ;
	    select_ = SEL_BOTH;
      }

      ಯದಿ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ಏತತ್);
      }
}

ಶೂನ್ಯ vvp_fun_muxr::recv_real(vvp_net_ptr_t ptr, ದ್ವಿಗುಣ bit,
                             vvp_context_t)
{
      ವಿಕಲ್ಪ (ptr.port()) {
	  ಪಕ್ಷ 0:
	    ಯದಿ (a_ == bit) ಪ್ರತ್ಯಾಗಮ;
	    a_ = bit;
	    ಯದಿ (select_ == SEL_PORT1) ಪ್ರತ್ಯಾಗಮ; // The other port is selected.
	    ಭಙ್ಗ;

	  ಪಕ್ಷ 1:
	    ಯದಿ (b_ == bit) ಪ್ರತ್ಯಾಗಮ;
	    b_ = bit;
	    ಯದಿ (select_ == SEL_PORT0) ಪ್ರತ್ಯಾಗಮ; // The other port is selected.
	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    ಪ್ರತಿಜ್ಞಾ(0);
      }

      ಯದಿ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ಏತತ್);
      }
}

ಶೂನ್ಯ vvp_fun_muxr::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      ವಿಕಲ್ಪ (select_) {
	  ಪಕ್ಷ SEL_PORT0:
	    ptr->send_real(a_, 0);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ SEL_PORT1:
	    ptr->send_real(b_, 0);
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    ಯದಿ (a_ == b_) {
		  ptr->send_real(a_, 0);
	    } ಅನ್ಯಥಾ {
		  ptr->send_real(0.0, 0); // Should this be NaN?
	    }
	    ಭಙ್ಗ;
      }
}

vvp_fun_muxz::vvp_fun_muxz(unsigned wid)
: a_(wid, BIT4_Z), b_(wid, BIT4_Z)
{
      net_ = 0;
      ಎಣಿಕೆ_functors_logic += 1;
      select_ = SEL_BOTH;
      has_run_ = ಅಸತ್ಯ;
}

vvp_fun_muxz::~vvp_fun_muxz()
{
}

ಶೂನ್ಯ vvp_fun_muxz::recv_vec4(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
                             vvp_context_t)
{
      ವಿಕಲ್ಪ (ptr.port()) {
	  ಪಕ್ಷ 0:
	    ಯದಿ (a_ .eeq(bit) && has_run_) ಪ್ರತ್ಯಾಗಮ;
	    a_ = bit;
	    ಯದಿ (select_ == SEL_PORT1) ಪ್ರತ್ಯಾಗಮ; // The other port is selected.
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 1:
	    ಯದಿ (b_ .eeq(bit) && has_run_) ಪ್ರತ್ಯಾಗಮ;
	    b_ = bit;
	    ಯದಿ (select_ == SEL_PORT0) ಪ್ರತ್ಯಾಗಮ; // The other port is selected.
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 2:
	    ಪ್ರತಿಜ್ಞಾ(bit.size() == 1);
	    ವಿಕಲ್ಪ (bit.value(0)) {
		ಪಕ್ಷ BIT4_0:
		  ಯದಿ (select_ == SEL_PORT0) ಪ್ರತ್ಯಾಗಮ;
		  select_ = SEL_PORT0;
		  ಭಙ್ಗ;
		ಪಕ್ಷ BIT4_1:
		  ಯದಿ (select_ == SEL_PORT1) ಪ್ರತ್ಯಾಗಮ;
		  select_ = SEL_PORT1;
		  ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		  ಯದಿ (select_ == SEL_BOTH && has_run_) ಪ್ರತ್ಯಾಗಮ;
		  select_ = SEL_BOTH;
	    }
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ;
      }

      ಯದಿ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ಏತತ್);
      }
}

ಶೂನ್ಯ vvp_fun_muxz::recv_vec4_pv(vvp_net_ptr_t ptr, ಧ್ರುವ vvp_vector4_t&bit,
				unsigned base, unsigned vwid, vvp_context_t)
{
      ಪ್ರತಿಜ್ಞಾ(base + bit.size() <= vwid);
      ತಾರ್ಕಿಕ flag;

      ವಿಕಲ್ಪ (ptr.port()) {
	  ಪಕ್ಷ 0:
	    flag = a_.set_vec(base, bit);
	    ಯದಿ (flag == ಅಸತ್ಯ && has_run_) ಪ್ರತ್ಯಾಗಮ;
	    ಯದಿ (select_ == SEL_PORT1) ಪ್ರತ್ಯಾಗಮ; // The other port is selected.
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 1:
	    flag = b_.set_vec(base, bit);
	    ಯದಿ (flag == ಅಸತ್ಯ && has_run_) ಪ್ರತ್ಯಾಗಮ;
	    ಯದಿ (select_ == SEL_PORT0) ಪ್ರತ್ಯಾಗಮ; // The other port is selected.
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 2:
	    ಪ್ರತಿಜ್ಞಾ((base == 0) && (bit.size() == 1));
	    recv_vec4(ptr, bit, 0);
	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ;
      }
      ಯದಿ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ಏತತ್);
      }
}

ಶೂನ್ಯ vvp_fun_muxz::run_run()
{
      has_run_ = ಸತ್ಯ;
      vvp_net_t*ptr = net_;
      net_ = 0;

      ವಿಕಲ್ಪ (select_) {
	  ಪಕ್ಷ SEL_PORT0:
	    ptr->send_vec4(a_, 0);
	    ಭಙ್ಗ;
	  ಪಕ್ಷ SEL_PORT1:
	    ptr->send_vec4(b_, 0);
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	      {
		    unsigned min_size = a_.size();
		    unsigned max_size = min_size;
		    ಯದಿ (b_.size() < min_size)
			  min_size = b_.size();
		    ಯದಿ (b_.size() > max_size)
			  max_size = b_.size();

		    vvp_vector4_t res (max_size);

		    ಪ್ರತಿ (unsigned idx = 0 ;  idx < min_size ;  idx += 1) {
			  ಯದಿ (a_.value(idx) == b_.value(idx))
				res.set_bit(idx, a_.value(idx));
			  ಅನ್ಯಥಾ
				res.set_bit(idx, BIT4_X);
		    }

		    ಪ್ರತಿ (unsigned idx = min_size ;  idx < max_size ;  idx += 1)
			  res.set_bit(idx, BIT4_X);

		    ptr->send_vec4(res, 0);
	      }
	    ಭಙ್ಗ;
      }
}

vvp_fun_not::vvp_fun_not(unsigned wid)
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
ಶೂನ್ಯ vvp_fun_not::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (ಒಳಹರಿವು_2, ಸತ್ಯ /* invert */);
      ptr->send_vec4(result, 0);
}

vvp_fun_or::vvp_fun_or(unsigned wid, ತಾರ್ಕಿಕ invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      ಎಣಿಕೆ_functors_logic += 1;
}

vvp_fun_or::~vvp_fun_or()
{
}

ಶೂನ್ಯ vvp_fun_or::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (ಒಳಹರಿವು_2[0]);

      ಪ್ರತಿ (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result |= ಒಳಹರಿವು_2[pdx];

      ಯದಿ (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_xor::vvp_fun_xor(unsigned wid, ತಾರ್ಕಿಕ invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      ಎಣಿಕೆ_functors_logic += 1;
}

vvp_fun_xor::~vvp_fun_xor()
{
}

ಶೂನ್ಯ vvp_fun_xor::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (ಒಳಹರಿವು_2[0]);

      ಪ್ರತಿ (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result ^= ಒಳಹರಿವು_2[pdx];

      ಯದಿ (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

/*
 * The parser calls this function to create a logic functor. I allocate a
 * functor, and map the name to the vvp_ipoint_t address for the
 * functor. Also resolve the inputs to the functor.
 */

ಶೂನ್ಯ compile_functor(ವರ್ಣ*label, ವರ್ಣ*type, unsigned width,
		     unsigned ostr0, unsigned ostr1,
		     unsigned argc, ಸಂರಚನಾ symb_s*argv)
{
      vvp_net_fun_t* obj = 0;
      ತಾರ್ಕಿಕ strength_aware = ಅಸತ್ಯ;

      ಯದಿ (strcmp(type, "OR") == 0) {
	    obj = ನೂತನ vvp_fun_or(width, ಅಸತ್ಯ);

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "AND") == 0) {
	    obj = ನೂತನ vvp_fun_and(width, ಅಸತ್ಯ);

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "BUF") == 0) {
	    obj = ನೂತನ vvp_fun_buf(width);

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "BUFIF0") == 0) {
	    obj = ನೂತನ vvp_fun_bufif(ಸತ್ಯ,ಅಸತ್ಯ, ostr0, ostr1);
	    strength_aware = ಸತ್ಯ;

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "BUFIF1") == 0) {
	    obj = ನೂತನ vvp_fun_bufif(ಅಸತ್ಯ,ಅಸತ್ಯ, ostr0, ostr1);
	    strength_aware = ಸತ್ಯ;

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "EQUIV") == 0) {
	    obj = ನೂತನ vvp_fun_equiv();

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "IMPL") == 0) {
	    obj = ನೂತನ vvp_fun_impl();

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "NAND") == 0) {
	    obj = ನೂತನ vvp_fun_and(width, ಸತ್ಯ);

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "NOR") == 0) {
	    obj = ನೂತನ vvp_fun_or(width, ಸತ್ಯ);

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "NOTIF0") == 0) {
	    obj = ನೂತನ vvp_fun_bufif(ಸತ್ಯ,ಸತ್ಯ, ostr0, ostr1);
	    strength_aware = ಸತ್ಯ;

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "NOTIF1") == 0) {
	    obj = ನೂತನ vvp_fun_bufif(ಅಸತ್ಯ,ಸತ್ಯ, ostr0, ostr1);
	    strength_aware = ಸತ್ಯ;

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "BUFT") == 0) {
	    obj = ನೂತನ vvp_fun_buft();

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "BUFZ") == 0) {
	    obj = ನೂತನ vvp_fun_bufz();

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "MUXR") == 0) {
	    obj = ನೂತನ vvp_fun_muxr;

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "MUXZ") == 0) {
	    obj = ನೂತನ vvp_fun_muxz(width);

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "CMOS") == 0) {
	    obj = ನೂತನ vvp_fun_cmos();

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "NMOS") == 0) {
	    obj = ನೂತನ vvp_fun_pmos(ಸತ್ಯ);

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "PMOS") == 0) {
	    obj = ನೂತನ vvp_fun_pmos(ಅಸತ್ಯ);

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "RCMOS") == 0) {
	    obj = ನೂತನ vvp_fun_rcmos();

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "RNMOS") == 0) {
	    obj = ನೂತನ vvp_fun_rpmos(ಸತ್ಯ);

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "RPMOS") == 0) {
	    obj = ನೂತನ vvp_fun_rpmos(ಅಸತ್ಯ);

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "NOT") == 0) {
	    obj = ನೂತನ vvp_fun_not(width);

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "XNOR") == 0) {
	    obj = ನೂತನ vvp_fun_xor(width, ಸತ್ಯ);

      } ಅನ್ಯಥಾ ಯದಿ (strcmp(type, "XOR") == 0) {
	    obj = ನೂತನ vvp_fun_xor(width, ಅಸತ್ಯ);

      } ಅನ್ಯಥಾ {
	    yyerror("invalid functor type.");
	    ಮುಕ್ತ(type);
	    ಮುಕ್ತ(argv);
	    ಮುಕ್ತ(label);
	    ಪ್ರತ್ಯಾಗಮ;
      }

      ಮುಕ್ತ(type);

      ಪ್ರತಿಜ್ಞಾ(argc <= 4);
      vvp_net_t*net = ನೂತನ vvp_net_t;
      net->fun = obj;

      inputs_connect(net, argc, argv);
      ಮುಕ್ತ(argv);

	/* If both the strengths are the default strong drive, then
	   there is no need for a specialized driver. Attach the label
	   to this node and we are finished. */
      ಯದಿ (strength_aware || (ostr0 == 6 && ostr1 == 6)) {
	    define_functor_ಚಿಹ್ನೆ(label, net);
	    ಮುಕ್ತ(label);
	    ಪ್ರತ್ಯಾಗಮ;
      }

      vvp_net_t*net_drv = ನೂತನ vvp_net_t;
      vvp_net_fun_t*obj_drv = ನೂತನ vvp_fun_drive(ostr0, ostr1);
      net_drv->fun = obj_drv;

	/* Point the gate to the drive node. */
      net->link(vvp_net_ptr_t(net_drv, 0));

      define_functor_ಚಿಹ್ನೆ(label, net_drv);
      ಮುಕ್ತ(label);
}
