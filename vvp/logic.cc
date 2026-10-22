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

vvp_fun_boolean_::vvp_fun_boolean_(بیعلامت wid)
{
      net_ = 0;
      لكل (بیعلامت idx = 0 ;  idx < 4 ;  idx += 1)
	    درون_داد_2[idx] = vvp_vector4_t(wid, BIT4_Z);
}

vvp_fun_boolean_::~vvp_fun_boolean_()
{
}

فراغ vvp_fun_boolean_::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                                 vvp_context_t)
{
      بیعلامت پورت = ptr.پورت();
      إذا (درون_داد_2[پورت] .eeq( بیت )) أرجع;

      درون_داد_2[پورت] = بیت;
      إذا (net_ == 0) {
	    net_ = ptr.ptr();
	    زمانبندی_functor(هذا);
      }
}

فراغ vvp_fun_boolean_::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
				    بیعلامت پایه, بیعلامت vwid, vvp_context_t)
{
      بیعلامت پورت = ptr.پورت();

      أكد(پایه + بیت.size() <= vwid);

	// Set the part for the input. If nothing changes, then break.
      منطقي flag = درون_داد_2[پورت] .مجموعه_vec(پایه, بیت);
      إذا (flag == خطأ)
	    أرجع;

      إذا (net_ == 0) {
	    net_ = ptr.ptr();
	    زمانبندی_functor(هذا);
      }
}

فراغ vvp_fun_boolean_::recv_real(vvp_net_ptr_t ptr, مزدوج real,
                                 vvp_context_t ctx)
{
      recv_vec4(ptr, double_مقصد_vector4_LSB(real), ctx);
}

vvp_fun_and::vvp_fun_and(بیعلامت wid, منطقي invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      شمار_functors_logic += 1;
}

vvp_fun_and::~vvp_fun_and()
{
}

فراغ vvp_fun_and::پیاپی_پیاپی()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t نتیجه (درون_داد_2[0]);

      لكل (بیعلامت pdx = 1 ; pdx < 4 ; pdx += 1)
	    نتیجه &= درون_داد_2[pdx];

      إذا (invert_)
	    نتیجه.invert();

      ptr->send_vec4(نتیجه, 0);
}

vvp_fun_equiv::vvp_fun_equiv()
: vvp_fun_boolean_(1)
{
      شمار_functors_logic += 1;
}

vvp_fun_equiv::~vvp_fun_equiv()
{
}

فراغ vvp_fun_equiv::پیاپی_پیاپی()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      أكد(درون_داد_2[0].size() == 1);
      أكد(درون_داد_2[1].size() == 1);

      vvp_bit4_t بیت = ~(درون_داد_2[0].value(0) ^ درون_داد_2[1].value(0));
      vvp_vector4_t نتیجه (1, بیت);

      ptr->send_vec4(نتیجه, 0);
}

vvp_fun_impl::vvp_fun_impl()
: vvp_fun_boolean_(1)
{
      شمار_functors_logic += 1;
}

vvp_fun_impl::~vvp_fun_impl()
{
}

فراغ vvp_fun_impl::پیاپی_پیاپی()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      أكد(درون_داد_2[0].size() == 1);
      أكد(درون_داد_2[1].size() == 1);

      vvp_bit4_t بیت = ~درون_داد_2[0].value(0) | درون_داد_2[1].value(0);
      vvp_vector4_t نتیجه (1, بیت);

      ptr->send_vec4(نتیجه, 0);
}

vvp_fun_buf_not_::vvp_fun_buf_not_(بیعلامت wid)
: درون_داد_2(wid, BIT4_Z)
{
      net_ = 0;
}

vvp_fun_buf_not_::~vvp_fun_buf_not_()
{
}

فراغ vvp_fun_buf_not_::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                                 vvp_context_t)
{
      إذا (ptr.پورت() != 0) أرجع;

      إذا (درون_داد_2 .eeq( بیت )) أرجع;

      درون_داد_2 = بیت;
      إذا (net_ == 0) {
	    net_ = ptr.ptr();
	    زمانبندی_functor(هذا);
      }
}

فراغ vvp_fun_buf_not_::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                                    بیعلامت پایه, بیعلامت vwid, vvp_context_t)
{
      إذا (ptr.پورت() != 0) أرجع;

      أكد(پایه + بیت.size() <= vwid);

	// Set the input part. If nothing changes, then break.
      منطقي flag = درون_داد_2.مجموعه_vec(پایه, بیت);
      إذا (flag == خطأ) أرجع;

      إذا (net_ == 0) {
	    net_ = ptr.ptr();
	    زمانبندی_functor(هذا);
      }
}

فراغ vvp_fun_buf_not_::recv_real(vvp_net_ptr_t ptr, مزدوج real,
                                 vvp_context_t ctx)
{
      إذا (ptr.پورت() != 0) أرجع;

      recv_vec4(ptr, double_مقصد_vector4_LSB(real), ctx);
}

vvp_fun_buf::vvp_fun_buf(بیعلامت wid)
: vvp_fun_buf_not_(wid)
{
      شمار_functors_logic += 1;
}

vvp_fun_buf::~vvp_fun_buf()
{
}

/*
 * The buf functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the result.
 */
فراغ vvp_fun_buf::پیاپی_پیاپی()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t tmp (درون_داد_2);
      tmp.change_z2x();
      ptr->send_vec4(tmp, 0);
}

vvp_fun_bufz::vvp_fun_bufz()
{
      شمار_functors_logic += 1;
}

vvp_fun_bufz::~vvp_fun_bufz()
{
}

/*
 * The bufz is similar to the buf device, except that it does not
 * bother translating z bits to x.
 */
فراغ vvp_fun_bufz::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                             vvp_context_t)
{
      إذا (ptr.پورت() != 0)
	    أرجع;

      ptr.ptr()->send_vec4(بیت, 0);
}

فراغ vvp_fun_bufz::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                                بیعلامت پایه, بیعلامت vwid, vvp_context_t)
{
      إذا (ptr.پورت() != 0)
	    أرجع;

      ptr.ptr()->send_vec4_pv(بیت, پایه, vwid, 0);
}

فراغ vvp_fun_bufz::recv_real(vvp_net_ptr_t ptr, مزدوج بیت,
                             vvp_context_t)
{
      إذا (ptr.پورت() != 0)
	    أرجع;

      ptr.ptr()->send_real(بیت, 0);
}

فراغ vvp_fun_buft::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&بیت)
{
      إذا (ptr.پورت() != 0)
	    أرجع;

      ptr.ptr()->send_vec8(بیت);
}

vvp_fun_muxr::vvp_fun_muxr()
: a_(0.0), b_(0.0)
{
      net_ = 0;
      شمار_functors_logic += 1;
      select_ = SEL_BOTH;
}

vvp_fun_muxr::~vvp_fun_muxr()
{
}

فراغ vvp_fun_muxr::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                             vvp_context_t)
{
	/* The real valued mux can only take in the select as a
	   vector4_t. The muxed data is real. */
      إذا (ptr.پورت() != 2)
	    أرجع;

      أكد(بیت.size() == 1);

      بدل (بیت.value(0)) {
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
	    زمانبندی_functor(هذا);
      }
}

فراغ vvp_fun_muxr::recv_real(vvp_net_ptr_t ptr, مزدوج بیت,
                             vvp_context_t)
{
      بدل (ptr.پورت()) {
	  حالة 0:
	    إذا (a_ == بیت) أرجع;
	    a_ = بیت;
	    إذا (select_ == SEL_PORT1) أرجع; // The other port is selected.
	    اكسر;

	  حالة 1:
	    إذا (b_ == بیت) أرجع;
	    b_ = بیت;
	    إذا (select_ == SEL_PORT0) أرجع; // The other port is selected.
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.پورت());
	    أكد(0);
      }

      إذا (net_ == 0) {
	    net_ = ptr.ptr();
	    زمانبندی_functor(هذا);
      }
}

فراغ vvp_fun_muxr::پیاپی_پیاپی()
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

vvp_fun_muxz::vvp_fun_muxz(بیعلامت wid)
: a_(wid, BIT4_Z), b_(wid, BIT4_Z)
{
      net_ = 0;
      شمار_functors_logic += 1;
      select_ = SEL_BOTH;
      has_پیاپی = خطأ;
}

vvp_fun_muxz::~vvp_fun_muxz()
{
}

فراغ vvp_fun_muxz::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
                             vvp_context_t)
{
      بدل (ptr.پورت()) {
	  حالة 0:
	    إذا (a_ .eeq(بیت) && has_پیاپی) أرجع;
	    a_ = بیت;
	    إذا (select_ == SEL_PORT1) أرجع; // The other port is selected.
	    اكسر;
	  حالة 1:
	    إذا (b_ .eeq(بیت) && has_پیاپی) أرجع;
	    b_ = بیت;
	    إذا (select_ == SEL_PORT0) أرجع; // The other port is selected.
	    اكسر;
	  حالة 2:
	    أكد(بیت.size() == 1);
	    بدل (بیت.value(0)) {
		حالة BIT4_0:
		  إذا (select_ == SEL_PORT0) أرجع;
		  select_ = SEL_PORT0;
		  اكسر;
		حالة BIT4_1:
		  إذا (select_ == SEL_PORT1) أرجع;
		  select_ = SEL_PORT1;
		  اكسر;
		افتراضي:
		  إذا (select_ == SEL_BOTH && has_پیاپی) أرجع;
		  select_ = SEL_BOTH;
	    }
	    اكسر;
	  افتراضي:
	    أرجع;
      }

      إذا (net_ == 0) {
	    net_ = ptr.ptr();
	    زمانبندی_functor(هذا);
      }
}

فراغ vvp_fun_muxz::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بیت,
				بیعلامت پایه, بیعلامت vwid, vvp_context_t)
{
      أكد(پایه + بیت.size() <= vwid);
      منطقي flag;

      بدل (ptr.پورت()) {
	  حالة 0:
	    flag = a_.مجموعه_vec(پایه, بیت);
	    إذا (flag == خطأ && has_پیاپی) أرجع;
	    إذا (select_ == SEL_PORT1) أرجع; // The other port is selected.
	    اكسر;
	  حالة 1:
	    flag = b_.مجموعه_vec(پایه, بیت);
	    إذا (flag == خطأ && has_پیاپی) أرجع;
	    إذا (select_ == SEL_PORT0) أرجع; // The other port is selected.
	    اكسر;
	  حالة 2:
	    أكد((پایه == 0) && (بیت.size() == 1));
	    recv_vec4(ptr, بیت, 0);
	  افتراضي:
	    أرجع;
      }
      إذا (net_ == 0) {
	    net_ = ptr.ptr();
	    زمانبندی_functor(هذا);
      }
}

فراغ vvp_fun_muxz::پیاپی_پیاپی()
{
      has_پیاپی = درست;
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
		    بیعلامت min_اندازه = a_.size();
		    بیعلامت max_اندازه = min_اندازه;
		    إذا (b_.size() < min_اندازه)
			  min_اندازه = b_.size();
		    إذا (b_.size() > max_اندازه)
			  max_اندازه = b_.size();

		    vvp_vector4_t res (max_اندازه);

		    لكل (بیعلامت idx = 0 ;  idx < min_اندازه ;  idx += 1) {
			  إذا (a_.value(idx) == b_.value(idx))
				res.مجموعه_بیت(idx, a_.value(idx));
			  وإلا
				res.مجموعه_بیت(idx, BIT4_X);
		    }

		    لكل (بیعلامت idx = min_اندازه ;  idx < max_اندازه ;  idx += 1)
			  res.مجموعه_بیت(idx, BIT4_X);

		    ptr->send_vec4(res, 0);
	      }
	    اكسر;
      }
}

vvp_fun_not::vvp_fun_not(بیعلامت wid)
: vvp_fun_buf_not_(wid)
{
      شمار_functors_logic += 1;
}

vvp_fun_not::~vvp_fun_not()
{
}

/*
 * The not functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the inverted result.
 */
فراغ vvp_fun_not::پیاپی_پیاپی()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t نتیجه (درون_داد_2, درست /* invert */);
      ptr->send_vec4(نتیجه, 0);
}

vvp_fun_or::vvp_fun_or(بیعلامت wid, منطقي invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      شمار_functors_logic += 1;
}

vvp_fun_or::~vvp_fun_or()
{
}

فراغ vvp_fun_or::پیاپی_پیاپی()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t نتیجه (درون_داد_2[0]);

      لكل (بیعلامت pdx = 1 ; pdx < 4 ; pdx += 1)
	    نتیجه |= درون_داد_2[pdx];

      إذا (invert_)
	    نتیجه.invert();

      ptr->send_vec4(نتیجه, 0);
}

vvp_fun_xor::vvp_fun_xor(بیعلامت wid, منطقي invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      شمار_functors_logic += 1;
}

vvp_fun_xor::~vvp_fun_xor()
{
}

فراغ vvp_fun_xor::پیاپی_پیاپی()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t نتیجه (درون_داد_2[0]);

      لكل (بیعلامت pdx = 1 ; pdx < 4 ; pdx += 1)
	    نتیجه ^= درون_داد_2[pdx];

      إذا (invert_)
	    نتیجه.invert();

      ptr->send_vec4(نتیجه, 0);
}

/*
 * The parser calls this function to create a logic functor. I allocate a
 * functor, and map the name to the vvp_ipoint_t address for the
 * functor. Also resolve the inputs to the functor.
 */

فراغ compile_functor(حرف*label, حرف*type, بیعلامت width,
		     بیعلامت ostr0, بیعلامت ostr1,
		     بیعلامت argc, بنية symb_s*argv)
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
	    obj = جديد vvp_fun_bufif(درست,خطأ, ostr0, ostr1);
	    strength_aware = درست;

      } وإلا إذا (strcmp(type, "BUFIF1") == 0) {
	    obj = جديد vvp_fun_bufif(خطأ,خطأ, ostr0, ostr1);
	    strength_aware = درست;

      } وإلا إذا (strcmp(type, "EQUIV") == 0) {
	    obj = جديد vvp_fun_equiv();

      } وإلا إذا (strcmp(type, "IMPL") == 0) {
	    obj = جديد vvp_fun_impl();

      } وإلا إذا (strcmp(type, "NAND") == 0) {
	    obj = جديد vvp_fun_and(width, درست);

      } وإلا إذا (strcmp(type, "NOR") == 0) {
	    obj = جديد vvp_fun_or(width, درست);

      } وإلا إذا (strcmp(type, "NOTIF0") == 0) {
	    obj = جديد vvp_fun_bufif(درست,درست, ostr0, ostr1);
	    strength_aware = درست;

      } وإلا إذا (strcmp(type, "NOTIF1") == 0) {
	    obj = جديد vvp_fun_bufif(خطأ,درست, ostr0, ostr1);
	    strength_aware = درست;

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
	    obj = جديد vvp_fun_pmos(درست);

      } وإلا إذا (strcmp(type, "PMOS") == 0) {
	    obj = جديد vvp_fun_pmos(خطأ);

      } وإلا إذا (strcmp(type, "RCMOS") == 0) {
	    obj = جديد vvp_fun_rcmos();

      } وإلا إذا (strcmp(type, "RNMOS") == 0) {
	    obj = جديد vvp_fun_rpmos(درست);

      } وإلا إذا (strcmp(type, "RPMOS") == 0) {
	    obj = جديد vvp_fun_rpmos(خطأ);

      } وإلا إذا (strcmp(type, "NOT") == 0) {
	    obj = جديد vvp_fun_not(width);

      } وإلا إذا (strcmp(type, "XNOR") == 0) {
	    obj = جديد vvp_fun_xor(width, درست);

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
	    define_functor_نماد(label, net);
	    حرر(label);
	    أرجع;
      }

      vvp_net_t*net_drv = جديد vvp_net_t;
      vvp_net_fun_t*obj_drv = جديد vvp_fun_drive(ostr0, ostr1);
      net_drv->fun = obj_drv;

	/* Point the gate to the drive node. */
      net->link(vvp_net_ptr_t(net_drv, 0));

      define_functor_نماد(label, net_drv);
      حرر(label);
}
