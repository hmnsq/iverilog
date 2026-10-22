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
      প্রতি (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    প্রবেশ_2[idx] = vvp_vector4_t(wid, BIT4_Z);
}

vvp_fun_boolean_::~vvp_fun_boolean_()
{
}

শূন্য vvp_fun_boolean_::recv_vec4(vvp_net_ptr_t ptr, ধ্রুব vvp_vector4_t&bit,
                                 vvp_context_t)
{
      unsigned port = ptr.port();
      যদি (প্রবেশ_2[port] .eeq( bit )) প্রত্যাগম;

      প্রবেশ_2[port] = bit;
      যদি (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(এতত্);
      }
}

শূন্য vvp_fun_boolean_::recv_vec4_pv(vvp_net_ptr_t ptr, ধ্রুব vvp_vector4_t&bit,
				    unsigned base, unsigned vwid, vvp_context_t)
{
      unsigned port = ptr.port();

      প্রতিজ্ঞা(base + bit.size() <= vwid);

	// Set the part for the input. If nothing changes, then break.
      তার্কিক flag = প্রবেশ_2[port] .set_vec(base, bit);
      যদি (flag == অসত্য)
	    প্রত্যাগম;

      যদি (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(এতত্);
      }
}

শূন্য vvp_fun_boolean_::recv_real(vvp_net_ptr_t ptr, দ্বিগুণ real,
                                 vvp_context_t ctx)
{
      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_and::vvp_fun_and(unsigned wid, তার্কিক invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      গুনতি_functors_logic += 1;
}

vvp_fun_and::~vvp_fun_and()
{
}

শূন্য vvp_fun_and::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (প্রবেশ_2[0]);

      প্রতি (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result &= প্রবেশ_2[pdx];

      যদি (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_equiv::vvp_fun_equiv()
: vvp_fun_boolean_(1)
{
      গুনতি_functors_logic += 1;
}

vvp_fun_equiv::~vvp_fun_equiv()
{
}

শূন্য vvp_fun_equiv::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      প্রতিজ্ঞা(প্রবেশ_2[0].size() == 1);
      প্রতিজ্ঞা(প্রবেশ_2[1].size() == 1);

      vvp_bit4_t bit = ~(প্রবেশ_2[0].value(0) ^ প্রবেশ_2[1].value(0));
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_impl::vvp_fun_impl()
: vvp_fun_boolean_(1)
{
      গুনতি_functors_logic += 1;
}

vvp_fun_impl::~vvp_fun_impl()
{
}

শূন্য vvp_fun_impl::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      প্রতিজ্ঞা(প্রবেশ_2[0].size() == 1);
      প্রতিজ্ঞা(প্রবেশ_2[1].size() == 1);

      vvp_bit4_t bit = ~প্রবেশ_2[0].value(0) | প্রবেশ_2[1].value(0);
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_buf_not_::vvp_fun_buf_not_(unsigned wid)
: প্রবেশ_2(wid, BIT4_Z)
{
      net_ = 0;
}

vvp_fun_buf_not_::~vvp_fun_buf_not_()
{
}

শূন্য vvp_fun_buf_not_::recv_vec4(vvp_net_ptr_t ptr, ধ্রুব vvp_vector4_t&bit,
                                 vvp_context_t)
{
      যদি (ptr.port() != 0) প্রত্যাগম;

      যদি (প্রবেশ_2 .eeq( bit )) প্রত্যাগম;

      প্রবেশ_2 = bit;
      যদি (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(এতত্);
      }
}

শূন্য vvp_fun_buf_not_::recv_vec4_pv(vvp_net_ptr_t ptr, ধ্রুব vvp_vector4_t&bit,
                                    unsigned base, unsigned vwid, vvp_context_t)
{
      যদি (ptr.port() != 0) প্রত্যাগম;

      প্রতিজ্ঞা(base + bit.size() <= vwid);

	// Set the input part. If nothing changes, then break.
      তার্কিক flag = প্রবেশ_2.set_vec(base, bit);
      যদি (flag == অসত্য) প্রত্যাগম;

      যদি (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(এতত্);
      }
}

শূন্য vvp_fun_buf_not_::recv_real(vvp_net_ptr_t ptr, দ্বিগুণ real,
                                 vvp_context_t ctx)
{
      যদি (ptr.port() != 0) প্রত্যাগম;

      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_buf::vvp_fun_buf(unsigned wid)
: vvp_fun_buf_not_(wid)
{
      গুনতি_functors_logic += 1;
}

vvp_fun_buf::~vvp_fun_buf()
{
}

/*
 * The buf functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the result.
 */
শূন্য vvp_fun_buf::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t tmp (প্রবেশ_2);
      tmp.change_z2x();
      ptr->send_vec4(tmp, 0);
}

vvp_fun_bufz::vvp_fun_bufz()
{
      গুনতি_functors_logic += 1;
}

vvp_fun_bufz::~vvp_fun_bufz()
{
}

/*
 * The bufz is similar to the buf device, except that it does not
 * bother translating z bits to x.
 */
শূন্য vvp_fun_bufz::recv_vec4(vvp_net_ptr_t ptr, ধ্রুব vvp_vector4_t&bit,
                             vvp_context_t)
{
      যদি (ptr.port() != 0)
	    প্রত্যাগম;

      ptr.ptr()->send_vec4(bit, 0);
}

শূন্য vvp_fun_bufz::recv_vec4_pv(vvp_net_ptr_t ptr, ধ্রুব vvp_vector4_t&bit,
                                unsigned base, unsigned vwid, vvp_context_t)
{
      যদি (ptr.port() != 0)
	    প্রত্যাগম;

      ptr.ptr()->send_vec4_pv(bit, base, vwid, 0);
}

শূন্য vvp_fun_bufz::recv_real(vvp_net_ptr_t ptr, দ্বিগুণ bit,
                             vvp_context_t)
{
      যদি (ptr.port() != 0)
	    প্রত্যাগম;

      ptr.ptr()->send_real(bit, 0);
}

শূন্য vvp_fun_buft::recv_vec8(vvp_net_ptr_t ptr, ধ্রুব vvp_vector8_t&bit)
{
      যদি (ptr.port() != 0)
	    প্রত্যাগম;

      ptr.ptr()->send_vec8(bit);
}

vvp_fun_muxr::vvp_fun_muxr()
: a_(0.0), b_(0.0)
{
      net_ = 0;
      গুনতি_functors_logic += 1;
      select_ = SEL_BOTH;
}

vvp_fun_muxr::~vvp_fun_muxr()
{
}

শূন্য vvp_fun_muxr::recv_vec4(vvp_net_ptr_t ptr, ধ্রুব vvp_vector4_t&bit,
                             vvp_context_t)
{
	/* The real valued mux can only take in the select as a
	   vector4_t. The muxed data is real. */
      যদি (ptr.port() != 2)
	    প্রত্যাগম;

      প্রতিজ্ঞা(bit.size() == 1);

      বিকল্প (bit.value(0)) {
	  পক্ষ BIT4_0:
	    যদি (select_ == SEL_PORT0) প্রত্যাগম;
	    select_ = SEL_PORT0;
	    ভঙ্গ;
	  পক্ষ BIT4_1:
	    যদি (select_ == SEL_PORT1) প্রত্যাগম;
	    select_ = SEL_PORT1;
	    ভঙ্গ;
	  স্বভাব:
	    যদি (select_ == SEL_BOTH) প্রত্যাগম;
	    select_ = SEL_BOTH;
      }

      যদি (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(এতত্);
      }
}

শূন্য vvp_fun_muxr::recv_real(vvp_net_ptr_t ptr, দ্বিগুণ bit,
                             vvp_context_t)
{
      বিকল্প (ptr.port()) {
	  পক্ষ 0:
	    যদি (a_ == bit) প্রত্যাগম;
	    a_ = bit;
	    যদি (select_ == SEL_PORT1) প্রত্যাগম; // The other port is selected.
	    ভঙ্গ;

	  পক্ষ 1:
	    যদি (b_ == bit) প্রত্যাগম;
	    b_ = bit;
	    যদি (select_ == SEL_PORT0) প্রত্যাগম; // The other port is selected.
	    ভঙ্গ;

	  স্বভাব:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    প্রতিজ্ঞা(0);
      }

      যদি (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(এতত্);
      }
}

শূন্য vvp_fun_muxr::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      বিকল্প (select_) {
	  পক্ষ SEL_PORT0:
	    ptr->send_real(a_, 0);
	    ভঙ্গ;
	  পক্ষ SEL_PORT1:
	    ptr->send_real(b_, 0);
	    ভঙ্গ;
	  স্বভাব:
	    যদি (a_ == b_) {
		  ptr->send_real(a_, 0);
	    } অন্যথা {
		  ptr->send_real(0.0, 0); // Should this be NaN?
	    }
	    ভঙ্গ;
      }
}

vvp_fun_muxz::vvp_fun_muxz(unsigned wid)
: a_(wid, BIT4_Z), b_(wid, BIT4_Z)
{
      net_ = 0;
      গুনতি_functors_logic += 1;
      select_ = SEL_BOTH;
      has_run_ = অসত্য;
}

vvp_fun_muxz::~vvp_fun_muxz()
{
}

শূন্য vvp_fun_muxz::recv_vec4(vvp_net_ptr_t ptr, ধ্রুব vvp_vector4_t&bit,
                             vvp_context_t)
{
      বিকল্প (ptr.port()) {
	  পক্ষ 0:
	    যদি (a_ .eeq(bit) && has_run_) প্রত্যাগম;
	    a_ = bit;
	    যদি (select_ == SEL_PORT1) প্রত্যাগম; // The other port is selected.
	    ভঙ্গ;
	  পক্ষ 1:
	    যদি (b_ .eeq(bit) && has_run_) প্রত্যাগম;
	    b_ = bit;
	    যদি (select_ == SEL_PORT0) প্রত্যাগম; // The other port is selected.
	    ভঙ্গ;
	  পক্ষ 2:
	    প্রতিজ্ঞা(bit.size() == 1);
	    বিকল্প (bit.value(0)) {
		পক্ষ BIT4_0:
		  যদি (select_ == SEL_PORT0) প্রত্যাগম;
		  select_ = SEL_PORT0;
		  ভঙ্গ;
		পক্ষ BIT4_1:
		  যদি (select_ == SEL_PORT1) প্রত্যাগম;
		  select_ = SEL_PORT1;
		  ভঙ্গ;
		স্বভাব:
		  যদি (select_ == SEL_BOTH && has_run_) প্রত্যাগম;
		  select_ = SEL_BOTH;
	    }
	    ভঙ্গ;
	  স্বভাব:
	    প্রত্যাগম;
      }

      যদি (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(এতত্);
      }
}

শূন্য vvp_fun_muxz::recv_vec4_pv(vvp_net_ptr_t ptr, ধ্রুব vvp_vector4_t&bit,
				unsigned base, unsigned vwid, vvp_context_t)
{
      প্রতিজ্ঞা(base + bit.size() <= vwid);
      তার্কিক flag;

      বিকল্প (ptr.port()) {
	  পক্ষ 0:
	    flag = a_.set_vec(base, bit);
	    যদি (flag == অসত্য && has_run_) প্রত্যাগম;
	    যদি (select_ == SEL_PORT1) প্রত্যাগম; // The other port is selected.
	    ভঙ্গ;
	  পক্ষ 1:
	    flag = b_.set_vec(base, bit);
	    যদি (flag == অসত্য && has_run_) প্রত্যাগম;
	    যদি (select_ == SEL_PORT0) প্রত্যাগম; // The other port is selected.
	    ভঙ্গ;
	  পক্ষ 2:
	    প্রতিজ্ঞা((base == 0) && (bit.size() == 1));
	    recv_vec4(ptr, bit, 0);
	  স্বভাব:
	    প্রত্যাগম;
      }
      যদি (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(এতত্);
      }
}

শূন্য vvp_fun_muxz::run_run()
{
      has_run_ = সত্য;
      vvp_net_t*ptr = net_;
      net_ = 0;

      বিকল্প (select_) {
	  পক্ষ SEL_PORT0:
	    ptr->send_vec4(a_, 0);
	    ভঙ্গ;
	  পক্ষ SEL_PORT1:
	    ptr->send_vec4(b_, 0);
	    ভঙ্গ;
	  স্বভাব:
	      {
		    unsigned min_size = a_.size();
		    unsigned max_size = min_size;
		    যদি (b_.size() < min_size)
			  min_size = b_.size();
		    যদি (b_.size() > max_size)
			  max_size = b_.size();

		    vvp_vector4_t res (max_size);

		    প্রতি (unsigned idx = 0 ;  idx < min_size ;  idx += 1) {
			  যদি (a_.value(idx) == b_.value(idx))
				res.set_bit(idx, a_.value(idx));
			  অন্যথা
				res.set_bit(idx, BIT4_X);
		    }

		    প্রতি (unsigned idx = min_size ;  idx < max_size ;  idx += 1)
			  res.set_bit(idx, BIT4_X);

		    ptr->send_vec4(res, 0);
	      }
	    ভঙ্গ;
      }
}

vvp_fun_not::vvp_fun_not(unsigned wid)
: vvp_fun_buf_not_(wid)
{
      গুনতি_functors_logic += 1;
}

vvp_fun_not::~vvp_fun_not()
{
}

/*
 * The not functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the inverted result.
 */
শূন্য vvp_fun_not::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (প্রবেশ_2, সত্য /* invert */);
      ptr->send_vec4(result, 0);
}

vvp_fun_or::vvp_fun_or(unsigned wid, তার্কিক invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      গুনতি_functors_logic += 1;
}

vvp_fun_or::~vvp_fun_or()
{
}

শূন্য vvp_fun_or::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (প্রবেশ_2[0]);

      প্রতি (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result |= প্রবেশ_2[pdx];

      যদি (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_xor::vvp_fun_xor(unsigned wid, তার্কিক invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      গুনতি_functors_logic += 1;
}

vvp_fun_xor::~vvp_fun_xor()
{
}

শূন্য vvp_fun_xor::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (প্রবেশ_2[0]);

      প্রতি (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result ^= প্রবেশ_2[pdx];

      যদি (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

/*
 * The parser calls this function to create a logic functor. I allocate a
 * functor, and map the name to the vvp_ipoint_t address for the
 * functor. Also resolve the inputs to the functor.
 */

শূন্য compile_functor(বর্ণ*label, বর্ণ*type, unsigned width,
		     unsigned ostr0, unsigned ostr1,
		     unsigned argc, সংরচনা symb_s*argv)
{
      vvp_net_fun_t* obj = 0;
      তার্কিক strength_aware = অসত্য;

      যদি (strcmp(type, "OR") == 0) {
	    obj = নূতন vvp_fun_or(width, অসত্য);

      } অন্যথা যদি (strcmp(type, "AND") == 0) {
	    obj = নূতন vvp_fun_and(width, অসত্য);

      } অন্যথা যদি (strcmp(type, "BUF") == 0) {
	    obj = নূতন vvp_fun_buf(width);

      } অন্যথা যদি (strcmp(type, "BUFIF0") == 0) {
	    obj = নূতন vvp_fun_bufif(সত্য,অসত্য, ostr0, ostr1);
	    strength_aware = সত্য;

      } অন্যথা যদি (strcmp(type, "BUFIF1") == 0) {
	    obj = নূতন vvp_fun_bufif(অসত্য,অসত্য, ostr0, ostr1);
	    strength_aware = সত্য;

      } অন্যথা যদি (strcmp(type, "EQUIV") == 0) {
	    obj = নূতন vvp_fun_equiv();

      } অন্যথা যদি (strcmp(type, "IMPL") == 0) {
	    obj = নূতন vvp_fun_impl();

      } অন্যথা যদি (strcmp(type, "NAND") == 0) {
	    obj = নূতন vvp_fun_and(width, সত্য);

      } অন্যথা যদি (strcmp(type, "NOR") == 0) {
	    obj = নূতন vvp_fun_or(width, সত্য);

      } অন্যথা যদি (strcmp(type, "NOTIF0") == 0) {
	    obj = নূতন vvp_fun_bufif(সত্য,সত্য, ostr0, ostr1);
	    strength_aware = সত্য;

      } অন্যথা যদি (strcmp(type, "NOTIF1") == 0) {
	    obj = নূতন vvp_fun_bufif(অসত্য,সত্য, ostr0, ostr1);
	    strength_aware = সত্য;

      } অন্যথা যদি (strcmp(type, "BUFT") == 0) {
	    obj = নূতন vvp_fun_buft();

      } অন্যথা যদি (strcmp(type, "BUFZ") == 0) {
	    obj = নূতন vvp_fun_bufz();

      } অন্যথা যদি (strcmp(type, "MUXR") == 0) {
	    obj = নূতন vvp_fun_muxr;

      } অন্যথা যদি (strcmp(type, "MUXZ") == 0) {
	    obj = নূতন vvp_fun_muxz(width);

      } অন্যথা যদি (strcmp(type, "CMOS") == 0) {
	    obj = নূতন vvp_fun_cmos();

      } অন্যথা যদি (strcmp(type, "NMOS") == 0) {
	    obj = নূতন vvp_fun_pmos(সত্য);

      } অন্যথা যদি (strcmp(type, "PMOS") == 0) {
	    obj = নূতন vvp_fun_pmos(অসত্য);

      } অন্যথা যদি (strcmp(type, "RCMOS") == 0) {
	    obj = নূতন vvp_fun_rcmos();

      } অন্যথা যদি (strcmp(type, "RNMOS") == 0) {
	    obj = নূতন vvp_fun_rpmos(সত্য);

      } অন্যথা যদি (strcmp(type, "RPMOS") == 0) {
	    obj = নূতন vvp_fun_rpmos(অসত্য);

      } অন্যথা যদি (strcmp(type, "NOT") == 0) {
	    obj = নূতন vvp_fun_not(width);

      } অন্যথা যদি (strcmp(type, "XNOR") == 0) {
	    obj = নূতন vvp_fun_xor(width, সত্য);

      } অন্যথা যদি (strcmp(type, "XOR") == 0) {
	    obj = নূতন vvp_fun_xor(width, অসত্য);

      } অন্যথা {
	    yyerror("invalid functor type.");
	    মুক্ত(type);
	    মুক্ত(argv);
	    মুক্ত(label);
	    প্রত্যাগম;
      }

      মুক্ত(type);

      প্রতিজ্ঞা(argc <= 4);
      vvp_net_t*net = নূতন vvp_net_t;
      net->fun = obj;

      inputs_connect(net, argc, argv);
      মুক্ত(argv);

	/* If both the strengths are the default strong drive, then
	   there is no need for a specialized driver. Attach the label
	   to this node and we are finished. */
      যদি (strength_aware || (ostr0 == 6 && ostr1 == 6)) {
	    define_functor_প্রতীক(label, net);
	    মুক্ত(label);
	    প্রত্যাগম;
      }

      vvp_net_t*net_drv = নূতন vvp_net_t;
      vvp_net_fun_t*obj_drv = নূতন vvp_fun_drive(ostr0, ostr1);
      net_drv->fun = obj_drv;

	/* Point the gate to the drive node. */
      net->link(vvp_net_ptr_t(net_drv, 0));

      define_functor_প্রতীক(label, net_drv);
      মুক্ত(label);
}
