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
      لكل (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    input_[idx] = vvp_vector4_t(wid, BIT4_Z);
}

vvp_fun_boolean_::~vvp_fun_boolean_()
{
}

فراغ vvp_fun_boolean_::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                                 vvp_context_t)
{
      unsigned port = ptr.port();
      ܐܢ (input_[port] .eeq( bit )) ܦܢܐ;

      input_[port] = bit;
      ܐܢ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(هذا);
      }
}

فراغ vvp_fun_boolean_::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
				    unsigned base, unsigned vwid, vvp_context_t)
{
      unsigned port = ptr.port();

      أكد(base + bit.size() <= vwid);

	// Set the part for the input. If nothing changes, then break.
      منطقي flag = input_[port] .set_vec(base, bit);
      ܐܢ (flag == ܕܓܠܐ)
	    ܦܢܐ;

      ܐܢ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(هذا);
      }
}

فراغ vvp_fun_boolean_::recv_real(vvp_net_ptr_t ptr, مزدوج real,
                                 vvp_context_t ctx)
{
      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_and::vvp_fun_and(unsigned wid, منطقي invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      count_functors_logic += 1;
}

vvp_fun_and::~vvp_fun_and()
{
}

فراغ vvp_fun_and::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (input_[0]);

      لكل (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result &= input_[pdx];

      ܐܢ (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_equiv::vvp_fun_equiv()
: vvp_fun_boolean_(1)
{
      count_functors_logic += 1;
}

vvp_fun_equiv::~vvp_fun_equiv()
{
}

فراغ vvp_fun_equiv::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      أكد(input_[0].size() == 1);
      أكد(input_[1].size() == 1);

      vvp_bit4_t bit = ~(input_[0].value(0) ^ input_[1].value(0));
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_impl::vvp_fun_impl()
: vvp_fun_boolean_(1)
{
      count_functors_logic += 1;
}

vvp_fun_impl::~vvp_fun_impl()
{
}

فراغ vvp_fun_impl::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      أكد(input_[0].size() == 1);
      أكد(input_[1].size() == 1);

      vvp_bit4_t bit = ~input_[0].value(0) | input_[1].value(0);
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_buf_not_::vvp_fun_buf_not_(unsigned wid)
: input_(wid, BIT4_Z)
{
      net_ = 0;
}

vvp_fun_buf_not_::~vvp_fun_buf_not_()
{
}

فراغ vvp_fun_buf_not_::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                                 vvp_context_t)
{
      ܐܢ (ptr.port() != 0) ܦܢܐ;

      ܐܢ (input_ .eeq( bit )) ܦܢܐ;

      input_ = bit;
      ܐܢ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(هذا);
      }
}

فراغ vvp_fun_buf_not_::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                                    unsigned base, unsigned vwid, vvp_context_t)
{
      ܐܢ (ptr.port() != 0) ܦܢܐ;

      أكد(base + bit.size() <= vwid);

	// Set the input part. If nothing changes, then break.
      منطقي flag = input_.set_vec(base, bit);
      ܐܢ (flag == ܕܓܠܐ) ܦܢܐ;

      ܐܢ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(هذا);
      }
}

فراغ vvp_fun_buf_not_::recv_real(vvp_net_ptr_t ptr, مزدوج real,
                                 vvp_context_t ctx)
{
      ܐܢ (ptr.port() != 0) ܦܢܐ;

      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_buf::vvp_fun_buf(unsigned wid)
: vvp_fun_buf_not_(wid)
{
      count_functors_logic += 1;
}

vvp_fun_buf::~vvp_fun_buf()
{
}

/*
 * The buf functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the result.
 */
فراغ vvp_fun_buf::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t tmp (input_);
      tmp.change_z2x();
      ptr->send_vec4(tmp, 0);
}

vvp_fun_bufz::vvp_fun_bufz()
{
      count_functors_logic += 1;
}

vvp_fun_bufz::~vvp_fun_bufz()
{
}

/*
 * The bufz is similar to the buf device, except that it does not
 * bother translating z bits to x.
 */
فراغ vvp_fun_bufz::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                             vvp_context_t)
{
      ܐܢ (ptr.port() != 0)
	    ܦܢܐ;

      ptr.ptr()->send_vec4(bit, 0);
}

فراغ vvp_fun_bufz::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                                unsigned base, unsigned vwid, vvp_context_t)
{
      ܐܢ (ptr.port() != 0)
	    ܦܢܐ;

      ptr.ptr()->send_vec4_pv(bit, base, vwid, 0);
}

فراغ vvp_fun_bufz::recv_real(vvp_net_ptr_t ptr, مزدوج bit,
                             vvp_context_t)
{
      ܐܢ (ptr.port() != 0)
	    ܦܢܐ;

      ptr.ptr()->send_real(bit, 0);
}

فراغ vvp_fun_buft::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&bit)
{
      ܐܢ (ptr.port() != 0)
	    ܦܢܐ;

      ptr.ptr()->send_vec8(bit);
}

vvp_fun_muxr::vvp_fun_muxr()
: a_(0.0), b_(0.0)
{
      net_ = 0;
      count_functors_logic += 1;
      select_ = SEL_BOTH;
}

vvp_fun_muxr::~vvp_fun_muxr()
{
}

فراغ vvp_fun_muxr::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                             vvp_context_t)
{
	/* The real valued mux can only take in the select as a
	   vector4_t. The muxed data is real. */
      ܐܢ (ptr.port() != 2)
	    ܦܢܐ;

      أكد(bit.size() == 1);

      بدل (bit.value(0)) {
	  حالة BIT4_0:
	    ܐܢ (select_ == SEL_PORT0) ܦܢܐ;
	    select_ = SEL_PORT0;
	    اكسر;
	  حالة BIT4_1:
	    ܐܢ (select_ == SEL_PORT1) ܦܢܐ;
	    select_ = SEL_PORT1;
	    اكسر;
	  افتراضي:
	    ܐܢ (select_ == SEL_BOTH) ܦܢܐ;
	    select_ = SEL_BOTH;
      }

      ܐܢ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(هذا);
      }
}

فراغ vvp_fun_muxr::recv_real(vvp_net_ptr_t ptr, مزدوج bit,
                             vvp_context_t)
{
      بدل (ptr.port()) {
	  حالة 0:
	    ܐܢ (a_ == bit) ܦܢܐ;
	    a_ = bit;
	    ܐܢ (select_ == SEL_PORT1) ܦܢܐ; // The other port is selected.
	    اكسر;

	  حالة 1:
	    ܐܢ (b_ == bit) ܦܢܐ;
	    b_ = bit;
	    ܐܢ (select_ == SEL_PORT0) ܦܢܐ; // The other port is selected.
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    أكد(0);
      }

      ܐܢ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(هذا);
      }
}

فراغ vvp_fun_muxr::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      بدل (select_) {
	  حالة SEL_PORT0:
	    ptr->send_real(a_, 0);
	    اكسر;
	  حالة SEL_PORT1:
	    ptr->send_real(b_, 0);
	    اكسر;
	  افتراضي:
	    ܐܢ (a_ == b_) {
		  ptr->send_real(a_, 0);
	    } ܐܚܪܢܐ {
		  ptr->send_real(0.0, 0); // Should this be NaN?
	    }
	    اكسر;
      }
}

vvp_fun_muxz::vvp_fun_muxz(unsigned wid)
: a_(wid, BIT4_Z), b_(wid, BIT4_Z)
{
      net_ = 0;
      count_functors_logic += 1;
      select_ = SEL_BOTH;
      has_run_ = ܕܓܠܐ;
}

vvp_fun_muxz::~vvp_fun_muxz()
{
}

فراغ vvp_fun_muxz::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                             vvp_context_t)
{
      بدل (ptr.port()) {
	  حالة 0:
	    ܐܢ (a_ .eeq(bit) && has_run_) ܦܢܐ;
	    a_ = bit;
	    ܐܢ (select_ == SEL_PORT1) ܦܢܐ; // The other port is selected.
	    اكسر;
	  حالة 1:
	    ܐܢ (b_ .eeq(bit) && has_run_) ܦܢܐ;
	    b_ = bit;
	    ܐܢ (select_ == SEL_PORT0) ܦܢܐ; // The other port is selected.
	    اكسر;
	  حالة 2:
	    أكد(bit.size() == 1);
	    بدل (bit.value(0)) {
		حالة BIT4_0:
		  ܐܢ (select_ == SEL_PORT0) ܦܢܐ;
		  select_ = SEL_PORT0;
		  اكسر;
		حالة BIT4_1:
		  ܐܢ (select_ == SEL_PORT1) ܦܢܐ;
		  select_ = SEL_PORT1;
		  اكسر;
		افتراضي:
		  ܐܢ (select_ == SEL_BOTH && has_run_) ܦܢܐ;
		  select_ = SEL_BOTH;
	    }
	    اكسر;
	  افتراضي:
	    ܦܢܐ;
      }

      ܐܢ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(هذا);
      }
}

فراغ vvp_fun_muxz::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
				unsigned base, unsigned vwid, vvp_context_t)
{
      أكد(base + bit.size() <= vwid);
      منطقي flag;

      بدل (ptr.port()) {
	  حالة 0:
	    flag = a_.set_vec(base, bit);
	    ܐܢ (flag == ܕܓܠܐ && has_run_) ܦܢܐ;
	    ܐܢ (select_ == SEL_PORT1) ܦܢܐ; // The other port is selected.
	    اكسر;
	  حالة 1:
	    flag = b_.set_vec(base, bit);
	    ܐܢ (flag == ܕܓܠܐ && has_run_) ܦܢܐ;
	    ܐܢ (select_ == SEL_PORT0) ܦܢܐ; // The other port is selected.
	    اكسر;
	  حالة 2:
	    أكد((base == 0) && (bit.size() == 1));
	    recv_vec4(ptr, bit, 0);
	  افتراضي:
	    ܦܢܐ;
      }
      ܐܢ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(هذا);
      }
}

فراغ vvp_fun_muxz::run_run()
{
      has_run_ = ܫܪܪܐ;
      vvp_net_t*ptr = net_;
      net_ = 0;

      بدل (select_) {
	  حالة SEL_PORT0:
	    ptr->send_vec4(a_, 0);
	    اكسر;
	  حالة SEL_PORT1:
	    ptr->send_vec4(b_, 0);
	    اكسر;
	  افتراضي:
	      {
		    unsigned min_size = a_.size();
		    unsigned max_size = min_size;
		    ܐܢ (b_.size() < min_size)
			  min_size = b_.size();
		    ܐܢ (b_.size() > max_size)
			  max_size = b_.size();

		    vvp_vector4_t res (max_size);

		    لكل (unsigned idx = 0 ;  idx < min_size ;  idx += 1) {
			  ܐܢ (a_.value(idx) == b_.value(idx))
				res.set_bit(idx, a_.value(idx));
			  ܐܚܪܢܐ
				res.set_bit(idx, BIT4_X);
		    }

		    لكل (unsigned idx = min_size ;  idx < max_size ;  idx += 1)
			  res.set_bit(idx, BIT4_X);

		    ptr->send_vec4(res, 0);
	      }
	    اكسر;
      }
}

vvp_fun_not::vvp_fun_not(unsigned wid)
: vvp_fun_buf_not_(wid)
{
      count_functors_logic += 1;
}

vvp_fun_not::~vvp_fun_not()
{
}

/*
 * The not functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the inverted result.
 */
فراغ vvp_fun_not::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (input_, ܫܪܪܐ /* invert */);
      ptr->send_vec4(result, 0);
}

vvp_fun_or::vvp_fun_or(unsigned wid, منطقي invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      count_functors_logic += 1;
}

vvp_fun_or::~vvp_fun_or()
{
}

فراغ vvp_fun_or::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (input_[0]);

      لكل (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result |= input_[pdx];

      ܐܢ (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_xor::vvp_fun_xor(unsigned wid, منطقي invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      count_functors_logic += 1;
}

vvp_fun_xor::~vvp_fun_xor()
{
}

فراغ vvp_fun_xor::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (input_[0]);

      لكل (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result ^= input_[pdx];

      ܐܢ (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

/*
 * The parser calls this function to create a logic functor. I allocate a
 * functor, and map the name to the vvp_ipoint_t address for the
 * functor. Also resolve the inputs to the functor.
 */

فراغ compile_functor(حرف*label, حرف*type, unsigned width,
		     unsigned ostr0, unsigned ostr1,
		     unsigned argc, بنية symb_s*argv)
{
      vvp_net_fun_t* obj = 0;
      منطقي strength_aware = ܕܓܠܐ;

      ܐܢ (strcmp(type, "OR") == 0) {
	    obj = جديد vvp_fun_or(width, ܕܓܠܐ);

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "AND") == 0) {
	    obj = جديد vvp_fun_and(width, ܕܓܠܐ);

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "BUF") == 0) {
	    obj = جديد vvp_fun_buf(width);

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "BUFIF0") == 0) {
	    obj = جديد vvp_fun_bufif(ܫܪܪܐ,ܕܓܠܐ, ostr0, ostr1);
	    strength_aware = ܫܪܪܐ;

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "BUFIF1") == 0) {
	    obj = جديد vvp_fun_bufif(ܕܓܠܐ,ܕܓܠܐ, ostr0, ostr1);
	    strength_aware = ܫܪܪܐ;

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "EQUIV") == 0) {
	    obj = جديد vvp_fun_equiv();

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "IMPL") == 0) {
	    obj = جديد vvp_fun_impl();

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "NAND") == 0) {
	    obj = جديد vvp_fun_and(width, ܫܪܪܐ);

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "NOR") == 0) {
	    obj = جديد vvp_fun_or(width, ܫܪܪܐ);

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "NOTIF0") == 0) {
	    obj = جديد vvp_fun_bufif(ܫܪܪܐ,ܫܪܪܐ, ostr0, ostr1);
	    strength_aware = ܫܪܪܐ;

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "NOTIF1") == 0) {
	    obj = جديد vvp_fun_bufif(ܕܓܠܐ,ܫܪܪܐ, ostr0, ostr1);
	    strength_aware = ܫܪܪܐ;

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "BUFT") == 0) {
	    obj = جديد vvp_fun_buft();

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "BUFZ") == 0) {
	    obj = جديد vvp_fun_bufz();

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "MUXR") == 0) {
	    obj = جديد vvp_fun_muxr;

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "MUXZ") == 0) {
	    obj = جديد vvp_fun_muxz(width);

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "CMOS") == 0) {
	    obj = جديد vvp_fun_cmos();

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "NMOS") == 0) {
	    obj = جديد vvp_fun_pmos(ܫܪܪܐ);

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "PMOS") == 0) {
	    obj = جديد vvp_fun_pmos(ܕܓܠܐ);

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "RCMOS") == 0) {
	    obj = جديد vvp_fun_rcmos();

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "RNMOS") == 0) {
	    obj = جديد vvp_fun_rpmos(ܫܪܪܐ);

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "RPMOS") == 0) {
	    obj = جديد vvp_fun_rpmos(ܕܓܠܐ);

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "NOT") == 0) {
	    obj = جديد vvp_fun_not(width);

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "XNOR") == 0) {
	    obj = جديد vvp_fun_xor(width, ܫܪܪܐ);

      } ܐܚܪܢܐ ܐܢ (strcmp(type, "XOR") == 0) {
	    obj = جديد vvp_fun_xor(width, ܕܓܠܐ);

      } ܐܚܪܢܐ {
	    yyerror("invalid functor type.");
	    حرر(type);
	    حرر(argv);
	    حرر(label);
	    ܦܢܐ;
      }

      حرر(type);

      أكد(argc <= 4);
      vvp_net_t*net = جديد vvp_net_t;
      net->fun = obj;

      inputs_connect(net, argc, argv);
      حرر(argv);

	/* If both the strengths are the default strong drive, then
	   there is no need for a specialized driver. Attach the label
	   to this node and we are finished. */
      ܐܢ (strength_aware || (ostr0 == 6 && ostr1 == 6)) {
	    define_functor_symbol(label, net);
	    حرر(label);
	    ܦܢܐ;
      }

      vvp_net_t*net_drv = جديد vvp_net_t;
      vvp_net_fun_t*obj_drv = جديد vvp_fun_drive(ostr0, ostr1);
      net_drv->fun = obj_drv;

	/* Point the gate to the drive node. */
      net->link(vvp_net_ptr_t(net_drv, 0));

      define_functor_symbol(label, net_drv);
      حرر(label);
}
