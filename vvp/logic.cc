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
	    مدخل_2[idx] = vvp_vector4_t(wid, BIT4_Z);
}

vvp_fun_boolean_::~vvp_fun_boolean_()
{
}

فراغ vvp_fun_boolean_::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                                 vvp_context_t)
{
      unsigned port = ptr.port();
      إذا (مدخل_2[port] .eeq( bit )) أرجع;

      مدخل_2[port] = bit;
      إذا (net_ == 0) {
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
      منطقي flag = مدخل_2[port] .set_vec(base, bit);
      إذا (flag == خطأ)
	    أرجع;

      إذا (net_ == 0) {
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
      عدد_functors_logic += 1;
}

vvp_fun_and::~vvp_fun_and()
{
}

فراغ vvp_fun_and::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (مدخل_2[0]);

      لكل (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result &= مدخل_2[pdx];

      إذا (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_equiv::vvp_fun_equiv()
: vvp_fun_boolean_(1)
{
      عدد_functors_logic += 1;
}

vvp_fun_equiv::~vvp_fun_equiv()
{
}

فراغ vvp_fun_equiv::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      أكد(مدخل_2[0].size() == 1);
      أكد(مدخل_2[1].size() == 1);

      vvp_bit4_t bit = ~(مدخل_2[0].value(0) ^ مدخل_2[1].value(0));
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_impl::vvp_fun_impl()
: vvp_fun_boolean_(1)
{
      عدد_functors_logic += 1;
}

vvp_fun_impl::~vvp_fun_impl()
{
}

فراغ vvp_fun_impl::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      أكد(مدخل_2[0].size() == 1);
      أكد(مدخل_2[1].size() == 1);

      vvp_bit4_t bit = ~مدخل_2[0].value(0) | مدخل_2[1].value(0);
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_buf_not_::vvp_fun_buf_not_(unsigned wid)
: مدخل_2(wid, BIT4_Z)
{
      net_ = 0;
}

vvp_fun_buf_not_::~vvp_fun_buf_not_()
{
}

فراغ vvp_fun_buf_not_::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                                 vvp_context_t)
{
      إذا (ptr.port() != 0) أرجع;

      إذا (مدخل_2 .eeq( bit )) أرجع;

      مدخل_2 = bit;
      إذا (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(هذا);
      }
}

فراغ vvp_fun_buf_not_::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                                    unsigned base, unsigned vwid, vvp_context_t)
{
      إذا (ptr.port() != 0) أرجع;

      أكد(base + bit.size() <= vwid);

	// Set the input part. If nothing changes, then break.
      منطقي flag = مدخل_2.set_vec(base, bit);
      إذا (flag == خطأ) أرجع;

      إذا (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(هذا);
      }
}

فراغ vvp_fun_buf_not_::recv_real(vvp_net_ptr_t ptr, مزدوج real,
                                 vvp_context_t ctx)
{
      إذا (ptr.port() != 0) أرجع;

      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_buf::vvp_fun_buf(unsigned wid)
: vvp_fun_buf_not_(wid)
{
      عدد_functors_logic += 1;
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

      vvp_vector4_t tmp (مدخل_2);
      tmp.change_z2x();
      ptr->send_vec4(tmp, 0);
}

vvp_fun_bufz::vvp_fun_bufz()
{
      عدد_functors_logic += 1;
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
      إذا (ptr.port() != 0)
	    أرجع;

      ptr.ptr()->send_vec4(bit, 0);
}

فراغ vvp_fun_bufz::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                                unsigned base, unsigned vwid, vvp_context_t)
{
      إذا (ptr.port() != 0)
	    أرجع;

      ptr.ptr()->send_vec4_pv(bit, base, vwid, 0);
}

فراغ vvp_fun_bufz::recv_real(vvp_net_ptr_t ptr, مزدوج bit,
                             vvp_context_t)
{
      إذا (ptr.port() != 0)
	    أرجع;

      ptr.ptr()->send_real(bit, 0);
}

فراغ vvp_fun_buft::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&bit)
{
      إذا (ptr.port() != 0)
	    أرجع;

      ptr.ptr()->send_vec8(bit);
}

vvp_fun_muxr::vvp_fun_muxr()
: a_(0.0), b_(0.0)
{
      net_ = 0;
      عدد_functors_logic += 1;
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
      إذا (ptr.port() != 2)
	    أرجع;

      أكد(bit.size() == 1);

      بدل (bit.value(0)) {
	  حالة BIT4_0:
	    إذا (select_ == SEL_PORT0) أرجع;
	    select_ = SEL_PORT0;
	    اكسر;
	  حالة BIT4_1:
	    إذا (select_ == SEL_PORT1) أرجع;
	    select_ = SEL_PORT1;
	    اكسر;
	  افتراضي:
	    إذا (select_ == SEL_BOTH) أرجع;
	    select_ = SEL_BOTH;
      }

      إذا (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(هذا);
      }
}

فراغ vvp_fun_muxr::recv_real(vvp_net_ptr_t ptr, مزدوج bit,
                             vvp_context_t)
{
      بدل (ptr.port()) {
	  حالة 0:
	    إذا (a_ == bit) أرجع;
	    a_ = bit;
	    إذا (select_ == SEL_PORT1) أرجع; // The other port is selected.
	    اكسر;

	  حالة 1:
	    إذا (b_ == bit) أرجع;
	    b_ = bit;
	    إذا (select_ == SEL_PORT0) أرجع; // The other port is selected.
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    أكد(0);
      }

      إذا (net_ == 0) {
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
	    إذا (a_ == b_) {
		  ptr->send_real(a_, 0);
	    } وإلا {
		  ptr->send_real(0.0, 0); // Should this be NaN?
	    }
	    اكسر;
      }
}

vvp_fun_muxz::vvp_fun_muxz(unsigned wid)
: a_(wid, BIT4_Z), b_(wid, BIT4_Z)
{
      net_ = 0;
      عدد_functors_logic += 1;
      select_ = SEL_BOTH;
      has_run_ = خطأ;
}

vvp_fun_muxz::~vvp_fun_muxz()
{
}

فراغ vvp_fun_muxz::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                             vvp_context_t)
{
      بدل (ptr.port()) {
	  حالة 0:
	    إذا (a_ .eeq(bit) && has_run_) أرجع;
	    a_ = bit;
	    إذا (select_ == SEL_PORT1) أرجع; // The other port is selected.
	    اكسر;
	  حالة 1:
	    إذا (b_ .eeq(bit) && has_run_) أرجع;
	    b_ = bit;
	    إذا (select_ == SEL_PORT0) أرجع; // The other port is selected.
	    اكسر;
	  حالة 2:
	    أكد(bit.size() == 1);
	    بدل (bit.value(0)) {
		حالة BIT4_0:
		  إذا (select_ == SEL_PORT0) أرجع;
		  select_ = SEL_PORT0;
		  اكسر;
		حالة BIT4_1:
		  إذا (select_ == SEL_PORT1) أرجع;
		  select_ = SEL_PORT1;
		  اكسر;
		افتراضي:
		  إذا (select_ == SEL_BOTH && has_run_) أرجع;
		  select_ = SEL_BOTH;
	    }
	    اكسر;
	  افتراضي:
	    أرجع;
      }

      إذا (net_ == 0) {
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
	    إذا (flag == خطأ && has_run_) أرجع;
	    إذا (select_ == SEL_PORT1) أرجع; // The other port is selected.
	    اكسر;
	  حالة 1:
	    flag = b_.set_vec(base, bit);
	    إذا (flag == خطأ && has_run_) أرجع;
	    إذا (select_ == SEL_PORT0) أرجع; // The other port is selected.
	    اكسر;
	  حالة 2:
	    أكد((base == 0) && (bit.size() == 1));
	    recv_vec4(ptr, bit, 0);
	  افتراضي:
	    أرجع;
      }
      إذا (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(هذا);
      }
}

فراغ vvp_fun_muxz::run_run()
{
      has_run_ = true;
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
		    إذا (b_.size() < min_size)
			  min_size = b_.size();
		    إذا (b_.size() > max_size)
			  max_size = b_.size();

		    vvp_vector4_t res (max_size);

		    لكل (unsigned idx = 0 ;  idx < min_size ;  idx += 1) {
			  إذا (a_.value(idx) == b_.value(idx))
				res.set_bit(idx, a_.value(idx));
			  وإلا
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
      عدد_functors_logic += 1;
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

      vvp_vector4_t result (مدخل_2, true /* invert */);
      ptr->send_vec4(result, 0);
}

vvp_fun_or::vvp_fun_or(unsigned wid, منطقي invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      عدد_functors_logic += 1;
}

vvp_fun_or::~vvp_fun_or()
{
}

فراغ vvp_fun_or::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (مدخل_2[0]);

      لكل (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result |= مدخل_2[pdx];

      إذا (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_xor::vvp_fun_xor(unsigned wid, منطقي invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      عدد_functors_logic += 1;
}

vvp_fun_xor::~vvp_fun_xor()
{
}

فراغ vvp_fun_xor::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (مدخل_2[0]);

      لكل (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result ^= مدخل_2[pdx];

      إذا (invert_)
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
      منطقي strength_aware = خطأ;

      إذا (strcmp(type, "OR") == 0) {
	    obj = جديد vvp_fun_or(width, خطأ);

      } وإلا إذا (strcmp(type, "AND") == 0) {
	    obj = جديد vvp_fun_and(width, خطأ);

      } وإلا إذا (strcmp(type, "BUF") == 0) {
	    obj = جديد vvp_fun_buf(width);

      } وإلا إذا (strcmp(type, "BUFIF0") == 0) {
	    obj = جديد vvp_fun_bufif(true,خطأ, ostr0, ostr1);
	    strength_aware = true;

      } وإلا إذا (strcmp(type, "BUFIF1") == 0) {
	    obj = جديد vvp_fun_bufif(خطأ,خطأ, ostr0, ostr1);
	    strength_aware = true;

      } وإلا إذا (strcmp(type, "EQUIV") == 0) {
	    obj = جديد vvp_fun_equiv();

      } وإلا إذا (strcmp(type, "IMPL") == 0) {
	    obj = جديد vvp_fun_impl();

      } وإلا إذا (strcmp(type, "NAND") == 0) {
	    obj = جديد vvp_fun_and(width, true);

      } وإلا إذا (strcmp(type, "NOR") == 0) {
	    obj = جديد vvp_fun_or(width, true);

      } وإلا إذا (strcmp(type, "NOTIF0") == 0) {
	    obj = جديد vvp_fun_bufif(true,true, ostr0, ostr1);
	    strength_aware = true;

      } وإلا إذا (strcmp(type, "NOTIF1") == 0) {
	    obj = جديد vvp_fun_bufif(خطأ,true, ostr0, ostr1);
	    strength_aware = true;

      } وإلا إذا (strcmp(type, "BUFT") == 0) {
	    obj = جديد vvp_fun_buft();

      } وإلا إذا (strcmp(type, "BUFZ") == 0) {
	    obj = جديد vvp_fun_bufz();

      } وإلا إذا (strcmp(type, "MUXR") == 0) {
	    obj = جديد vvp_fun_muxr;

      } وإلا إذا (strcmp(type, "MUXZ") == 0) {
	    obj = جديد vvp_fun_muxz(width);

      } وإلا إذا (strcmp(type, "CMOS") == 0) {
	    obj = جديد vvp_fun_cmos();

      } وإلا إذا (strcmp(type, "NMOS") == 0) {
	    obj = جديد vvp_fun_pmos(true);

      } وإلا إذا (strcmp(type, "PMOS") == 0) {
	    obj = جديد vvp_fun_pmos(خطأ);

      } وإلا إذا (strcmp(type, "RCMOS") == 0) {
	    obj = جديد vvp_fun_rcmos();

      } وإلا إذا (strcmp(type, "RNMOS") == 0) {
	    obj = جديد vvp_fun_rpmos(true);

      } وإلا إذا (strcmp(type, "RPMOS") == 0) {
	    obj = جديد vvp_fun_rpmos(خطأ);

      } وإلا إذا (strcmp(type, "NOT") == 0) {
	    obj = جديد vvp_fun_not(width);

      } وإلا إذا (strcmp(type, "XNOR") == 0) {
	    obj = جديد vvp_fun_xor(width, true);

      } وإلا إذا (strcmp(type, "XOR") == 0) {
	    obj = جديد vvp_fun_xor(width, خطأ);

      } وإلا {
	    yyerror("invalid functor type.");
	    حرر(type);
	    حرر(argv);
	    حرر(label);
	    أرجع;
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
      إذا (strength_aware || (ostr0 == 6 && ostr1 == 6)) {
	    define_functor_رمز(label, net);
	    حرر(label);
	    أرجع;
      }

      vvp_net_t*net_drv = جديد vvp_net_t;
      vvp_net_fun_t*obj_drv = جديد vvp_fun_drive(ostr0, ostr1);
      net_drv->fun = obj_drv;

	/* Point the gate to the drive node. */
      net->link(vvp_net_ptr_t(net_drv, 0));

      define_functor_رمز(label, net_drv);
      حرر(label);
}
