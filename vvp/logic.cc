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
      עבור (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    input_[idx] = vvp_vector4_t(wid, BIT4_Z);
}

vvp_fun_boolean_::~vvp_fun_boolean_()
{
}

فراغ vvp_fun_boolean_::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                                 vvp_context_t)
{
      unsigned port = ptr.port();
      אם (input_[port] .eeq( bit )) החזר;

      input_[port] = bit;
      אם (net_ == 0) {
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
      אם (flag == שקר)
	    החזר;

      אם (net_ == 0) {
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

      עבור (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result &= input_[pdx];

      אם (invert_)
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
      אם (ptr.port() != 0) החזר;

      אם (input_ .eeq( bit )) החזר;

      input_ = bit;
      אם (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(هذا);
      }
}

فراغ vvp_fun_buf_not_::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                                    unsigned base, unsigned vwid, vvp_context_t)
{
      אם (ptr.port() != 0) החזר;

      أكد(base + bit.size() <= vwid);

	// Set the input part. If nothing changes, then break.
      منطقي flag = input_.set_vec(base, bit);
      אם (flag == שקר) החזר;

      אם (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(هذا);
      }
}

فراغ vvp_fun_buf_not_::recv_real(vvp_net_ptr_t ptr, مزدوج real,
                                 vvp_context_t ctx)
{
      אם (ptr.port() != 0) החזר;

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
      אם (ptr.port() != 0)
	    החזר;

      ptr.ptr()->send_vec4(bit, 0);
}

فراغ vvp_fun_bufz::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                                unsigned base, unsigned vwid, vvp_context_t)
{
      אם (ptr.port() != 0)
	    החזר;

      ptr.ptr()->send_vec4_pv(bit, base, vwid, 0);
}

فراغ vvp_fun_bufz::recv_real(vvp_net_ptr_t ptr, مزدوج bit,
                             vvp_context_t)
{
      אם (ptr.port() != 0)
	    החזר;

      ptr.ptr()->send_real(bit, 0);
}

فراغ vvp_fun_buft::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&bit)
{
      אם (ptr.port() != 0)
	    החזר;

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
      אם (ptr.port() != 2)
	    החזר;

      أكد(bit.size() == 1);

      بدل (bit.value(0)) {
	  حالة BIT4_0:
	    אם (select_ == SEL_PORT0) החזר;
	    select_ = SEL_PORT0;
	    اكسر;
	  حالة BIT4_1:
	    אם (select_ == SEL_PORT1) החזר;
	    select_ = SEL_PORT1;
	    اكسر;
	  افتراضي:
	    אם (select_ == SEL_BOTH) החזר;
	    select_ = SEL_BOTH;
      }

      אם (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(هذا);
      }
}

فراغ vvp_fun_muxr::recv_real(vvp_net_ptr_t ptr, مزدوج bit,
                             vvp_context_t)
{
      بدل (ptr.port()) {
	  حالة 0:
	    אם (a_ == bit) החזר;
	    a_ = bit;
	    אם (select_ == SEL_PORT1) החזר; // The other port is selected.
	    اكسر;

	  حالة 1:
	    אם (b_ == bit) החזר;
	    b_ = bit;
	    אם (select_ == SEL_PORT0) החזר; // The other port is selected.
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    أكد(0);
      }

      אם (net_ == 0) {
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
	    אם (a_ == b_) {
		  ptr->send_real(a_, 0);
	    } אחרת {
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
      has_run_ = שקר;
}

vvp_fun_muxz::~vvp_fun_muxz()
{
}

فراغ vvp_fun_muxz::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                             vvp_context_t)
{
      بدل (ptr.port()) {
	  حالة 0:
	    אם (a_ .eeq(bit) && has_run_) החזר;
	    a_ = bit;
	    אם (select_ == SEL_PORT1) החזר; // The other port is selected.
	    اكسر;
	  حالة 1:
	    אם (b_ .eeq(bit) && has_run_) החזר;
	    b_ = bit;
	    אם (select_ == SEL_PORT0) החזר; // The other port is selected.
	    اكسر;
	  حالة 2:
	    أكد(bit.size() == 1);
	    بدل (bit.value(0)) {
		حالة BIT4_0:
		  אם (select_ == SEL_PORT0) החזר;
		  select_ = SEL_PORT0;
		  اكسر;
		حالة BIT4_1:
		  אם (select_ == SEL_PORT1) החזר;
		  select_ = SEL_PORT1;
		  اكسر;
		افتراضي:
		  אם (select_ == SEL_BOTH && has_run_) החזר;
		  select_ = SEL_BOTH;
	    }
	    اكسر;
	  افتراضي:
	    החזר;
      }

      אם (net_ == 0) {
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
	    אם (flag == שקר && has_run_) החזר;
	    אם (select_ == SEL_PORT1) החזר; // The other port is selected.
	    اكسر;
	  حالة 1:
	    flag = b_.set_vec(base, bit);
	    אם (flag == שקר && has_run_) החזר;
	    אם (select_ == SEL_PORT0) החזר; // The other port is selected.
	    اكسر;
	  حالة 2:
	    أكد((base == 0) && (bit.size() == 1));
	    recv_vec4(ptr, bit, 0);
	  افتراضي:
	    החזר;
      }
      אם (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(هذا);
      }
}

فراغ vvp_fun_muxz::run_run()
{
      has_run_ = אמת;
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
		    אם (b_.size() < min_size)
			  min_size = b_.size();
		    אם (b_.size() > max_size)
			  max_size = b_.size();

		    vvp_vector4_t res (max_size);

		    עבור (unsigned idx = 0 ;  idx < min_size ;  idx += 1) {
			  אם (a_.value(idx) == b_.value(idx))
				res.set_bit(idx, a_.value(idx));
			  אחרת
				res.set_bit(idx, BIT4_X);
		    }

		    עבור (unsigned idx = min_size ;  idx < max_size ;  idx += 1)
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

      vvp_vector4_t result (input_, אמת /* invert */);
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

      עבור (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result |= input_[pdx];

      אם (invert_)
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

      עבור (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result ^= input_[pdx];

      אם (invert_)
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
      منطقي strength_aware = שקר;

      אם (strcmp(type, "OR") == 0) {
	    obj = جديد vvp_fun_or(width, שקר);

      } אחרת אם (strcmp(type, "AND") == 0) {
	    obj = جديد vvp_fun_and(width, שקר);

      } אחרת אם (strcmp(type, "BUF") == 0) {
	    obj = جديد vvp_fun_buf(width);

      } אחרת אם (strcmp(type, "BUFIF0") == 0) {
	    obj = جديد vvp_fun_bufif(אמת,שקר, ostr0, ostr1);
	    strength_aware = אמת;

      } אחרת אם (strcmp(type, "BUFIF1") == 0) {
	    obj = جديد vvp_fun_bufif(שקר,שקר, ostr0, ostr1);
	    strength_aware = אמת;

      } אחרת אם (strcmp(type, "EQUIV") == 0) {
	    obj = جديد vvp_fun_equiv();

      } אחרת אם (strcmp(type, "IMPL") == 0) {
	    obj = جديد vvp_fun_impl();

      } אחרת אם (strcmp(type, "NAND") == 0) {
	    obj = جديد vvp_fun_and(width, אמת);

      } אחרת אם (strcmp(type, "NOR") == 0) {
	    obj = جديد vvp_fun_or(width, אמת);

      } אחרת אם (strcmp(type, "NOTIF0") == 0) {
	    obj = جديد vvp_fun_bufif(אמת,אמת, ostr0, ostr1);
	    strength_aware = אמת;

      } אחרת אם (strcmp(type, "NOTIF1") == 0) {
	    obj = جديد vvp_fun_bufif(שקר,אמת, ostr0, ostr1);
	    strength_aware = אמת;

      } אחרת אם (strcmp(type, "BUFT") == 0) {
	    obj = جديد vvp_fun_buft();

      } אחרת אם (strcmp(type, "BUFZ") == 0) {
	    obj = جديد vvp_fun_bufz();

      } אחרת אם (strcmp(type, "MUXR") == 0) {
	    obj = جديد vvp_fun_muxr;

      } אחרת אם (strcmp(type, "MUXZ") == 0) {
	    obj = جديد vvp_fun_muxz(width);

      } אחרת אם (strcmp(type, "CMOS") == 0) {
	    obj = جديد vvp_fun_cmos();

      } אחרת אם (strcmp(type, "NMOS") == 0) {
	    obj = جديد vvp_fun_pmos(אמת);

      } אחרת אם (strcmp(type, "PMOS") == 0) {
	    obj = جديد vvp_fun_pmos(שקר);

      } אחרת אם (strcmp(type, "RCMOS") == 0) {
	    obj = جديد vvp_fun_rcmos();

      } אחרת אם (strcmp(type, "RNMOS") == 0) {
	    obj = جديد vvp_fun_rpmos(אמת);

      } אחרת אם (strcmp(type, "RPMOS") == 0) {
	    obj = جديد vvp_fun_rpmos(שקר);

      } אחרת אם (strcmp(type, "NOT") == 0) {
	    obj = جديد vvp_fun_not(width);

      } אחרת אם (strcmp(type, "XNOR") == 0) {
	    obj = جديد vvp_fun_xor(width, אמת);

      } אחרת אם (strcmp(type, "XOR") == 0) {
	    obj = جديد vvp_fun_xor(width, שקר);

      } אחרת {
	    yyerror("invalid functor type.");
	    حرر(type);
	    حرر(argv);
	    حرر(label);
	    החזר;
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
      אם (strength_aware || (ostr0 == 6 && ostr1 == 6)) {
	    define_functor_symbol(label, net);
	    حرر(label);
	    החזר;
      }

      vvp_net_t*net_drv = جديد vvp_net_t;
      vvp_net_fun_t*obj_drv = جديد vvp_fun_drive(ostr0, ostr1);
      net_drv->fun = obj_drv;

	/* Point the gate to the drive node. */
      net->link(vvp_net_ptr_t(net_drv, 0));

      define_functor_symbol(label, net_drv);
      حرر(label);
}
