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
      ପାଇଁ (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    ନିବେଶ_2[idx] = vvp_vector4_t(wid, BIT4_Z);
}

vvp_fun_boolean_::~vvp_fun_boolean_()
{
}

ଶୂନ୍ୟ vvp_fun_boolean_::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                                 vvp_context_t)
{
      unsigned port = ptr.port();
      ଯଦି (ନିବେଶ_2[port] .eeq( bit )) ଫେରନ୍ତୁ;

      ନିବେଶ_2[port] = bit;
      ଯଦି (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ଏହି);
      }
}

ଶୂନ୍ୟ vvp_fun_boolean_::recv_vec4_pv(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
				    unsigned base, unsigned vwid, vvp_context_t)
{
      unsigned port = ptr.port();

      ପ୍ରତିଜ୍ଞା(base + bit.size() <= vwid);

	// Set the part for the input. If nothing changes, then break.
      ତାର୍କିକ flag = ନିବେଶ_2[port] .set_vec(base, bit);
      ଯଦି (flag == ମିଥ୍ୟା)
	    ଫେରନ୍ତୁ;

      ଯଦି (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ଏହି);
      }
}

ଶୂନ୍ୟ vvp_fun_boolean_::recv_real(vvp_net_ptr_t ptr, ଦ୍ୱିଗୁଣ real,
                                 vvp_context_t ctx)
{
      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_and::vvp_fun_and(unsigned wid, ତାର୍କିକ invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      ଗଣନା_functors_logic += 1;
}

vvp_fun_and::~vvp_fun_and()
{
}

ଶୂନ୍ୟ vvp_fun_and::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (ନିବେଶ_2[0]);

      ପାଇଁ (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result &= ନିବେଶ_2[pdx];

      ଯଦି (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_equiv::vvp_fun_equiv()
: vvp_fun_boolean_(1)
{
      ଗଣନା_functors_logic += 1;
}

vvp_fun_equiv::~vvp_fun_equiv()
{
}

ଶୂନ୍ୟ vvp_fun_equiv::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      ପ୍ରତିଜ୍ଞା(ନିବେଶ_2[0].size() == 1);
      ପ୍ରତିଜ୍ଞା(ନିବେଶ_2[1].size() == 1);

      vvp_bit4_t bit = ~(ନିବେଶ_2[0].value(0) ^ ନିବେଶ_2[1].value(0));
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_impl::vvp_fun_impl()
: vvp_fun_boolean_(1)
{
      ଗଣନା_functors_logic += 1;
}

vvp_fun_impl::~vvp_fun_impl()
{
}

ଶୂନ୍ୟ vvp_fun_impl::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      ପ୍ରତିଜ୍ଞା(ନିବେଶ_2[0].size() == 1);
      ପ୍ରତିଜ୍ଞା(ନିବେଶ_2[1].size() == 1);

      vvp_bit4_t bit = ~ନିବେଶ_2[0].value(0) | ନିବେଶ_2[1].value(0);
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_buf_not_::vvp_fun_buf_not_(unsigned wid)
: ନିବେଶ_2(wid, BIT4_Z)
{
      net_ = 0;
}

vvp_fun_buf_not_::~vvp_fun_buf_not_()
{
}

ଶୂନ୍ୟ vvp_fun_buf_not_::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                                 vvp_context_t)
{
      ଯଦି (ptr.port() != 0) ଫେରନ୍ତୁ;

      ଯଦି (ନିବେଶ_2 .eeq( bit )) ଫେରନ୍ତୁ;

      ନିବେଶ_2 = bit;
      ଯଦି (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ଏହି);
      }
}

ଶୂନ୍ୟ vvp_fun_buf_not_::recv_vec4_pv(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                                    unsigned base, unsigned vwid, vvp_context_t)
{
      ଯଦି (ptr.port() != 0) ଫେରନ୍ତୁ;

      ପ୍ରତିଜ୍ଞା(base + bit.size() <= vwid);

	// Set the input part. If nothing changes, then break.
      ତାର୍କିକ flag = ନିବେଶ_2.set_vec(base, bit);
      ଯଦି (flag == ମିଥ୍ୟା) ଫେରନ୍ତୁ;

      ଯଦି (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ଏହି);
      }
}

ଶୂନ୍ୟ vvp_fun_buf_not_::recv_real(vvp_net_ptr_t ptr, ଦ୍ୱିଗୁଣ real,
                                 vvp_context_t ctx)
{
      ଯଦି (ptr.port() != 0) ଫେରନ୍ତୁ;

      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_buf::vvp_fun_buf(unsigned wid)
: vvp_fun_buf_not_(wid)
{
      ଗଣନା_functors_logic += 1;
}

vvp_fun_buf::~vvp_fun_buf()
{
}

/*
 * The buf functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the result.
 */
ଶୂନ୍ୟ vvp_fun_buf::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t tmp (ନିବେଶ_2);
      tmp.change_z2x();
      ptr->send_vec4(tmp, 0);
}

vvp_fun_bufz::vvp_fun_bufz()
{
      ଗଣନା_functors_logic += 1;
}

vvp_fun_bufz::~vvp_fun_bufz()
{
}

/*
 * The bufz is similar to the buf device, except that it does not
 * bother translating z bits to x.
 */
ଶୂନ୍ୟ vvp_fun_bufz::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                             vvp_context_t)
{
      ଯଦି (ptr.port() != 0)
	    ଫେରନ୍ତୁ;

      ptr.ptr()->send_vec4(bit, 0);
}

ଶୂନ୍ୟ vvp_fun_bufz::recv_vec4_pv(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                                unsigned base, unsigned vwid, vvp_context_t)
{
      ଯଦି (ptr.port() != 0)
	    ଫେରନ୍ତୁ;

      ptr.ptr()->send_vec4_pv(bit, base, vwid, 0);
}

ଶୂନ୍ୟ vvp_fun_bufz::recv_real(vvp_net_ptr_t ptr, ଦ୍ୱିଗୁଣ bit,
                             vvp_context_t)
{
      ଯଦି (ptr.port() != 0)
	    ଫେରନ୍ତୁ;

      ptr.ptr()->send_real(bit, 0);
}

ଶୂନ୍ୟ vvp_fun_buft::recv_vec8(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector8_t&bit)
{
      ଯଦି (ptr.port() != 0)
	    ଫେରନ୍ତୁ;

      ptr.ptr()->send_vec8(bit);
}

vvp_fun_muxr::vvp_fun_muxr()
: a_(0.0), b_(0.0)
{
      net_ = 0;
      ଗଣନା_functors_logic += 1;
      select_ = SEL_BOTH;
}

vvp_fun_muxr::~vvp_fun_muxr()
{
}

ଶୂନ୍ୟ vvp_fun_muxr::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                             vvp_context_t)
{
	/* The real valued mux can only take in the select as a
	   vector4_t. The muxed data is real. */
      ଯଦି (ptr.port() != 2)
	    ଫେରନ୍ତୁ;

      ପ୍ରତିଜ୍ଞା(bit.size() == 1);

      ବାଛ (bit.value(0)) {
	  ସ୍ଥିତି BIT4_0:
	    ଯଦି (select_ == SEL_PORT0) ଫେରନ୍ତୁ;
	    select_ = SEL_PORT0;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_1:
	    ଯଦି (select_ == SEL_PORT1) ଫେରନ୍ତୁ;
	    select_ = SEL_PORT1;
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଯଦି (select_ == SEL_BOTH) ଫେରନ୍ତୁ;
	    select_ = SEL_BOTH;
      }

      ଯଦି (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ଏହି);
      }
}

ଶୂନ୍ୟ vvp_fun_muxr::recv_real(vvp_net_ptr_t ptr, ଦ୍ୱିଗୁଣ bit,
                             vvp_context_t)
{
      ବାଛ (ptr.port()) {
	  ସ୍ଥିତି 0:
	    ଯଦି (a_ == bit) ଫେରନ୍ତୁ;
	    a_ = bit;
	    ଯଦି (select_ == SEL_PORT1) ଫେରନ୍ତୁ; // The other port is selected.
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି 1:
	    ଯଦି (b_ == bit) ଫେରନ୍ତୁ;
	    b_ = bit;
	    ଯଦି (select_ == SEL_PORT0) ଫେରନ୍ତୁ; // The other port is selected.
	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    ପ୍ରତିଜ୍ଞା(0);
      }

      ଯଦି (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ଏହି);
      }
}

ଶୂନ୍ୟ vvp_fun_muxr::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      ବାଛ (select_) {
	  ସ୍ଥିତି SEL_PORT0:
	    ptr->send_real(a_, 0);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି SEL_PORT1:
	    ptr->send_real(b_, 0);
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଯଦି (a_ == b_) {
		  ptr->send_real(a_, 0);
	    } ଅନ୍ୟଥା {
		  ptr->send_real(0.0, 0); // Should this be NaN?
	    }
	    ଭାଙ୍ଗ;
      }
}

vvp_fun_muxz::vvp_fun_muxz(unsigned wid)
: a_(wid, BIT4_Z), b_(wid, BIT4_Z)
{
      net_ = 0;
      ଗଣନା_functors_logic += 1;
      select_ = SEL_BOTH;
      has_run_ = ମିଥ୍ୟା;
}

vvp_fun_muxz::~vvp_fun_muxz()
{
}

ଶୂନ୍ୟ vvp_fun_muxz::recv_vec4(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
                             vvp_context_t)
{
      ବାଛ (ptr.port()) {
	  ସ୍ଥିତି 0:
	    ଯଦି (a_ .eeq(bit) && has_run_) ଫେରନ୍ତୁ;
	    a_ = bit;
	    ଯଦି (select_ == SEL_PORT1) ଫେରନ୍ତୁ; // The other port is selected.
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 1:
	    ଯଦି (b_ .eeq(bit) && has_run_) ଫେରନ୍ତୁ;
	    b_ = bit;
	    ଯଦି (select_ == SEL_PORT0) ଫେରନ୍ତୁ; // The other port is selected.
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 2:
	    ପ୍ରତିଜ୍ଞା(bit.size() == 1);
	    ବାଛ (bit.value(0)) {
		ସ୍ଥିତି BIT4_0:
		  ଯଦି (select_ == SEL_PORT0) ଫେରନ୍ତୁ;
		  select_ = SEL_PORT0;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି BIT4_1:
		  ଯଦି (select_ == SEL_PORT1) ଫେରନ୍ତୁ;
		  select_ = SEL_PORT1;
		  ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		  ଯଦି (select_ == SEL_BOTH && has_run_) ଫେରନ୍ତୁ;
		  select_ = SEL_BOTH;
	    }
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ;
      }

      ଯଦି (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ଏହି);
      }
}

ଶୂନ୍ୟ vvp_fun_muxz::recv_vec4_pv(vvp_net_ptr_t ptr, ଅଚଳ vvp_vector4_t&bit,
				unsigned base, unsigned vwid, vvp_context_t)
{
      ପ୍ରତିଜ୍ଞା(base + bit.size() <= vwid);
      ତାର୍କିକ flag;

      ବାଛ (ptr.port()) {
	  ସ୍ଥିତି 0:
	    flag = a_.set_vec(base, bit);
	    ଯଦି (flag == ମିଥ୍ୟା && has_run_) ଫେରନ୍ତୁ;
	    ଯଦି (select_ == SEL_PORT1) ଫେରନ୍ତୁ; // The other port is selected.
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 1:
	    flag = b_.set_vec(base, bit);
	    ଯଦି (flag == ମିଥ୍ୟା && has_run_) ଫେରନ୍ତୁ;
	    ଯଦି (select_ == SEL_PORT0) ଫେରନ୍ତୁ; // The other port is selected.
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 2:
	    ପ୍ରତିଜ୍ଞା((base == 0) && (bit.size() == 1));
	    recv_vec4(ptr, bit, 0);
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ;
      }
      ଯଦି (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ଏହି);
      }
}

ଶୂନ୍ୟ vvp_fun_muxz::run_run()
{
      has_run_ = ସତ୍ୟ;
      vvp_net_t*ptr = net_;
      net_ = 0;

      ବାଛ (select_) {
	  ସ୍ଥିତି SEL_PORT0:
	    ptr->send_vec4(a_, 0);
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି SEL_PORT1:
	    ptr->send_vec4(b_, 0);
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	      {
		    unsigned min_size = a_.size();
		    unsigned max_size = min_size;
		    ଯଦି (b_.size() < min_size)
			  min_size = b_.size();
		    ଯଦି (b_.size() > max_size)
			  max_size = b_.size();

		    vvp_vector4_t res (max_size);

		    ପାଇଁ (unsigned idx = 0 ;  idx < min_size ;  idx += 1) {
			  ଯଦି (a_.value(idx) == b_.value(idx))
				res.set_bit(idx, a_.value(idx));
			  ଅନ୍ୟଥା
				res.set_bit(idx, BIT4_X);
		    }

		    ପାଇଁ (unsigned idx = min_size ;  idx < max_size ;  idx += 1)
			  res.set_bit(idx, BIT4_X);

		    ptr->send_vec4(res, 0);
	      }
	    ଭାଙ୍ଗ;
      }
}

vvp_fun_not::vvp_fun_not(unsigned wid)
: vvp_fun_buf_not_(wid)
{
      ଗଣନା_functors_logic += 1;
}

vvp_fun_not::~vvp_fun_not()
{
}

/*
 * The not functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the inverted result.
 */
ଶୂନ୍ୟ vvp_fun_not::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (ନିବେଶ_2, ସତ୍ୟ /* invert */);
      ptr->send_vec4(result, 0);
}

vvp_fun_or::vvp_fun_or(unsigned wid, ତାର୍କିକ invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      ଗଣନା_functors_logic += 1;
}

vvp_fun_or::~vvp_fun_or()
{
}

ଶୂନ୍ୟ vvp_fun_or::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (ନିବେଶ_2[0]);

      ପାଇଁ (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result |= ନିବେଶ_2[pdx];

      ଯଦି (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_xor::vvp_fun_xor(unsigned wid, ତାର୍କିକ invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      ଗଣନା_functors_logic += 1;
}

vvp_fun_xor::~vvp_fun_xor()
{
}

ଶୂନ୍ୟ vvp_fun_xor::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (ନିବେଶ_2[0]);

      ପାଇଁ (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result ^= ନିବେଶ_2[pdx];

      ଯଦି (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

/*
 * The parser calls this function to create a logic functor. I allocate a
 * functor, and map the name to the vvp_ipoint_t address for the
 * functor. Also resolve the inputs to the functor.
 */

ଶୂନ୍ୟ compile_functor(ଅକ୍ଷର*label, ଅକ୍ଷର*type, unsigned width,
		     unsigned ostr0, unsigned ostr1,
		     unsigned argc, ସଂରଚନା symb_s*argv)
{
      vvp_net_fun_t* obj = 0;
      ତାର୍କିକ strength_aware = ମିଥ୍ୟା;

      ଯଦି (strcmp(type, "OR") == 0) {
	    obj = ନୂତନ vvp_fun_or(width, ମିଥ୍ୟା);

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "AND") == 0) {
	    obj = ନୂତନ vvp_fun_and(width, ମିଥ୍ୟା);

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "BUF") == 0) {
	    obj = ନୂତନ vvp_fun_buf(width);

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "BUFIF0") == 0) {
	    obj = ନୂତନ vvp_fun_bufif(ସତ୍ୟ,ମିଥ୍ୟା, ostr0, ostr1);
	    strength_aware = ସତ୍ୟ;

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "BUFIF1") == 0) {
	    obj = ନୂତନ vvp_fun_bufif(ମିଥ୍ୟା,ମିଥ୍ୟା, ostr0, ostr1);
	    strength_aware = ସତ୍ୟ;

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "EQUIV") == 0) {
	    obj = ନୂତନ vvp_fun_equiv();

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "IMPL") == 0) {
	    obj = ନୂତନ vvp_fun_impl();

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "NAND") == 0) {
	    obj = ନୂତନ vvp_fun_and(width, ସତ୍ୟ);

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "NOR") == 0) {
	    obj = ନୂତନ vvp_fun_or(width, ସତ୍ୟ);

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "NOTIF0") == 0) {
	    obj = ନୂତନ vvp_fun_bufif(ସତ୍ୟ,ସତ୍ୟ, ostr0, ostr1);
	    strength_aware = ସତ୍ୟ;

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "NOTIF1") == 0) {
	    obj = ନୂତନ vvp_fun_bufif(ମିଥ୍ୟା,ସତ୍ୟ, ostr0, ostr1);
	    strength_aware = ସତ୍ୟ;

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "BUFT") == 0) {
	    obj = ନୂତନ vvp_fun_buft();

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "BUFZ") == 0) {
	    obj = ନୂତନ vvp_fun_bufz();

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "MUXR") == 0) {
	    obj = ନୂତନ vvp_fun_muxr;

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "MUXZ") == 0) {
	    obj = ନୂତନ vvp_fun_muxz(width);

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "CMOS") == 0) {
	    obj = ନୂତନ vvp_fun_cmos();

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "NMOS") == 0) {
	    obj = ନୂତନ vvp_fun_pmos(ସତ୍ୟ);

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "PMOS") == 0) {
	    obj = ନୂତନ vvp_fun_pmos(ମିଥ୍ୟା);

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "RCMOS") == 0) {
	    obj = ନୂତନ vvp_fun_rcmos();

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "RNMOS") == 0) {
	    obj = ନୂତନ vvp_fun_rpmos(ସତ୍ୟ);

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "RPMOS") == 0) {
	    obj = ନୂତନ vvp_fun_rpmos(ମିଥ୍ୟା);

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "NOT") == 0) {
	    obj = ନୂତନ vvp_fun_not(width);

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "XNOR") == 0) {
	    obj = ନୂତନ vvp_fun_xor(width, ସତ୍ୟ);

      } ଅନ୍ୟଥା ଯଦି (strcmp(type, "XOR") == 0) {
	    obj = ନୂତନ vvp_fun_xor(width, ମିଥ୍ୟା);

      } ଅନ୍ୟଥା {
	    yyerror("invalid functor type.");
	    ମୁକ୍ତ(type);
	    ମୁକ୍ତ(argv);
	    ମୁକ୍ତ(label);
	    ଫେରନ୍ତୁ;
      }

      ମୁକ୍ତ(type);

      ପ୍ରତିଜ୍ଞା(argc <= 4);
      vvp_net_t*net = ନୂତନ vvp_net_t;
      net->fun = obj;

      inputs_connect(net, argc, argv);
      ମୁକ୍ତ(argv);

	/* If both the strengths are the default strong drive, then
	   there is no need for a specialized driver. Attach the label
	   to this node and we are finished. */
      ଯଦି (strength_aware || (ostr0 == 6 && ostr1 == 6)) {
	    define_functor_ପ୍ରତୀକ(label, net);
	    ମୁକ୍ତ(label);
	    ଫେରନ୍ତୁ;
      }

      vvp_net_t*net_drv = ନୂତନ vvp_net_t;
      vvp_net_fun_t*obj_drv = ନୂତନ vvp_fun_drive(ostr0, ostr1);
      net_drv->fun = obj_drv;

	/* Point the gate to the drive node. */
      net->link(vvp_net_ptr_t(net_drv, 0));

      define_functor_ପ୍ରତୀକ(label, net_drv);
      ମୁକ୍ତ(label);
}
