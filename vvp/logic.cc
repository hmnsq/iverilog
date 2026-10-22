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
      برای (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    درون_داد_2[idx] = vvp_vector4_t(wid, BIT4_Z);
}

vvp_fun_boolean_::~vvp_fun_boolean_()
{
}

تهی vvp_fun_boolean_::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                                 vvp_context_t)
{
      unsigned port = ptr.port();
      اگر (درون_داد_2[port] .eeq( bit )) بازگشت;

      درون_داد_2[port] = bit;
      اگر (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(این);
      }
}

تهی vvp_fun_boolean_::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
				    unsigned base, unsigned vwid, vvp_context_t)
{
      unsigned port = ptr.port();

      أكد(base + bit.size() <= vwid);

	// Set the part for the input. If nothing changes, then break.
      منطقی flag = درون_داد_2[port] .set_vec(base, bit);
      اگر (flag == نادرست)
	    بازگشت;

      اگر (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(این);
      }
}

تهی vvp_fun_boolean_::recv_real(vvp_net_ptr_t ptr, دوگانه real,
                                 vvp_context_t ctx)
{
      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_and::vvp_fun_and(unsigned wid, منطقی invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      شمار_functors_logic += 1;
}

vvp_fun_and::~vvp_fun_and()
{
}

تهی vvp_fun_and::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (درون_داد_2[0]);

      برای (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result &= درون_داد_2[pdx];

      اگر (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_equiv::vvp_fun_equiv()
: vvp_fun_boolean_(1)
{
      شمار_functors_logic += 1;
}

vvp_fun_equiv::~vvp_fun_equiv()
{
}

تهی vvp_fun_equiv::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      أكد(درون_داد_2[0].size() == 1);
      أكد(درون_داد_2[1].size() == 1);

      vvp_bit4_t bit = ~(درون_داد_2[0].value(0) ^ درون_داد_2[1].value(0));
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_impl::vvp_fun_impl()
: vvp_fun_boolean_(1)
{
      شمار_functors_logic += 1;
}

vvp_fun_impl::~vvp_fun_impl()
{
}

تهی vvp_fun_impl::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      أكد(درون_داد_2[0].size() == 1);
      أكد(درون_داد_2[1].size() == 1);

      vvp_bit4_t bit = ~درون_داد_2[0].value(0) | درون_داد_2[1].value(0);
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_buf_not_::vvp_fun_buf_not_(unsigned wid)
: درون_داد_2(wid, BIT4_Z)
{
      net_ = 0;
}

vvp_fun_buf_not_::~vvp_fun_buf_not_()
{
}

تهی vvp_fun_buf_not_::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                                 vvp_context_t)
{
      اگر (ptr.port() != 0) بازگشت;

      اگر (درون_داد_2 .eeq( bit )) بازگشت;

      درون_داد_2 = bit;
      اگر (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(این);
      }
}

تهی vvp_fun_buf_not_::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                                    unsigned base, unsigned vwid, vvp_context_t)
{
      اگر (ptr.port() != 0) بازگشت;

      أكد(base + bit.size() <= vwid);

	// Set the input part. If nothing changes, then break.
      منطقی flag = درون_داد_2.set_vec(base, bit);
      اگر (flag == نادرست) بازگشت;

      اگر (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(این);
      }
}

تهی vvp_fun_buf_not_::recv_real(vvp_net_ptr_t ptr, دوگانه real,
                                 vvp_context_t ctx)
{
      اگر (ptr.port() != 0) بازگشت;

      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_buf::vvp_fun_buf(unsigned wid)
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
تهی vvp_fun_buf::run_run()
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
تهی vvp_fun_bufz::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                             vvp_context_t)
{
      اگر (ptr.port() != 0)
	    بازگشت;

      ptr.ptr()->send_vec4(bit, 0);
}

تهی vvp_fun_bufz::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                                unsigned base, unsigned vwid, vvp_context_t)
{
      اگر (ptr.port() != 0)
	    بازگشت;

      ptr.ptr()->send_vec4_pv(bit, base, vwid, 0);
}

تهی vvp_fun_bufz::recv_real(vvp_net_ptr_t ptr, دوگانه bit,
                             vvp_context_t)
{
      اگر (ptr.port() != 0)
	    بازگشت;

      ptr.ptr()->send_real(bit, 0);
}

تهی vvp_fun_buft::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&bit)
{
      اگر (ptr.port() != 0)
	    بازگشت;

      ptr.ptr()->send_vec8(bit);
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

تهی vvp_fun_muxr::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                             vvp_context_t)
{
	/* The real valued mux can only take in the select as a
	   vector4_t. The muxed data is real. */
      اگر (ptr.port() != 2)
	    بازگشت;

      أكد(bit.size() == 1);

      انتخاب (bit.value(0)) {
	  حالت BIT4_0:
	    اگر (select_ == SEL_PORT0) بازگشت;
	    select_ = SEL_PORT0;
	    بشکن;
	  حالت BIT4_1:
	    اگر (select_ == SEL_PORT1) بازگشت;
	    select_ = SEL_PORT1;
	    بشکن;
	  پیشفرض:
	    اگر (select_ == SEL_BOTH) بازگشت;
	    select_ = SEL_BOTH;
      }

      اگر (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(این);
      }
}

تهی vvp_fun_muxr::recv_real(vvp_net_ptr_t ptr, دوگانه bit,
                             vvp_context_t)
{
      انتخاب (ptr.port()) {
	  حالت 0:
	    اگر (a_ == bit) بازگشت;
	    a_ = bit;
	    اگر (select_ == SEL_PORT1) بازگشت; // The other port is selected.
	    بشکن;

	  حالت 1:
	    اگر (b_ == bit) بازگشت;
	    b_ = bit;
	    اگر (select_ == SEL_PORT0) بازگشت; // The other port is selected.
	    بشکن;

	  پیشفرض:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    أكد(0);
      }

      اگر (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(این);
      }
}

تهی vvp_fun_muxr::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      انتخاب (select_) {
	  حالت SEL_PORT0:
	    ptr->send_real(a_, 0);
	    بشکن;
	  حالت SEL_PORT1:
	    ptr->send_real(b_, 0);
	    بشکن;
	  پیشفرض:
	    اگر (a_ == b_) {
		  ptr->send_real(a_, 0);
	    } وگرنه {
		  ptr->send_real(0.0, 0); // Should this be NaN?
	    }
	    بشکن;
      }
}

vvp_fun_muxz::vvp_fun_muxz(unsigned wid)
: a_(wid, BIT4_Z), b_(wid, BIT4_Z)
{
      net_ = 0;
      شمار_functors_logic += 1;
      select_ = SEL_BOTH;
      has_run_ = نادرست;
}

vvp_fun_muxz::~vvp_fun_muxz()
{
}

تهی vvp_fun_muxz::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
                             vvp_context_t)
{
      انتخاب (ptr.port()) {
	  حالت 0:
	    اگر (a_ .eeq(bit) && has_run_) بازگشت;
	    a_ = bit;
	    اگر (select_ == SEL_PORT1) بازگشت; // The other port is selected.
	    بشکن;
	  حالت 1:
	    اگر (b_ .eeq(bit) && has_run_) بازگشت;
	    b_ = bit;
	    اگر (select_ == SEL_PORT0) بازگشت; // The other port is selected.
	    بشکن;
	  حالت 2:
	    أكد(bit.size() == 1);
	    انتخاب (bit.value(0)) {
		حالت BIT4_0:
		  اگر (select_ == SEL_PORT0) بازگشت;
		  select_ = SEL_PORT0;
		  بشکن;
		حالت BIT4_1:
		  اگر (select_ == SEL_PORT1) بازگشت;
		  select_ = SEL_PORT1;
		  بشکن;
		پیشفرض:
		  اگر (select_ == SEL_BOTH && has_run_) بازگشت;
		  select_ = SEL_BOTH;
	    }
	    بشکن;
	  پیشفرض:
	    بازگشت;
      }

      اگر (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(این);
      }
}

تهی vvp_fun_muxz::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&bit,
				unsigned base, unsigned vwid, vvp_context_t)
{
      أكد(base + bit.size() <= vwid);
      منطقی flag;

      انتخاب (ptr.port()) {
	  حالت 0:
	    flag = a_.set_vec(base, bit);
	    اگر (flag == نادرست && has_run_) بازگشت;
	    اگر (select_ == SEL_PORT1) بازگشت; // The other port is selected.
	    بشکن;
	  حالت 1:
	    flag = b_.set_vec(base, bit);
	    اگر (flag == نادرست && has_run_) بازگشت;
	    اگر (select_ == SEL_PORT0) بازگشت; // The other port is selected.
	    بشکن;
	  حالت 2:
	    أكد((base == 0) && (bit.size() == 1));
	    recv_vec4(ptr, bit, 0);
	  پیشفرض:
	    بازگشت;
      }
      اگر (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(این);
      }
}

تهی vvp_fun_muxz::run_run()
{
      has_run_ = درست;
      vvp_net_t*ptr = net_;
      net_ = 0;

      انتخاب (select_) {
	  حالت SEL_PORT0:
	    ptr->send_vec4(a_, 0);
	    بشکن;
	  حالت SEL_PORT1:
	    ptr->send_vec4(b_, 0);
	    بشکن;
	  پیشفرض:
	      {
		    unsigned min_size = a_.size();
		    unsigned max_size = min_size;
		    اگر (b_.size() < min_size)
			  min_size = b_.size();
		    اگر (b_.size() > max_size)
			  max_size = b_.size();

		    vvp_vector4_t res (max_size);

		    برای (unsigned idx = 0 ;  idx < min_size ;  idx += 1) {
			  اگر (a_.value(idx) == b_.value(idx))
				res.set_bit(idx, a_.value(idx));
			  وگرنه
				res.set_bit(idx, BIT4_X);
		    }

		    برای (unsigned idx = min_size ;  idx < max_size ;  idx += 1)
			  res.set_bit(idx, BIT4_X);

		    ptr->send_vec4(res, 0);
	      }
	    بشکن;
      }
}

vvp_fun_not::vvp_fun_not(unsigned wid)
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
تهی vvp_fun_not::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (درون_داد_2, درست /* invert */);
      ptr->send_vec4(result, 0);
}

vvp_fun_or::vvp_fun_or(unsigned wid, منطقی invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      شمار_functors_logic += 1;
}

vvp_fun_or::~vvp_fun_or()
{
}

تهی vvp_fun_or::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (درون_داد_2[0]);

      برای (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result |= درون_داد_2[pdx];

      اگر (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_xor::vvp_fun_xor(unsigned wid, منطقی invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      شمار_functors_logic += 1;
}

vvp_fun_xor::~vvp_fun_xor()
{
}

تهی vvp_fun_xor::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (درون_داد_2[0]);

      برای (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result ^= درون_داد_2[pdx];

      اگر (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

/*
 * The parser calls this function to create a logic functor. I allocate a
 * functor, and map the name to the vvp_ipoint_t address for the
 * functor. Also resolve the inputs to the functor.
 */

تهی compile_functor(نویسه*label, نویسه*type, unsigned width,
		     unsigned ostr0, unsigned ostr1,
		     unsigned argc, ساختار symb_s*argv)
{
      vvp_net_fun_t* obj = 0;
      منطقی strength_aware = نادرست;

      اگر (strcmp(type, "OR") == 0) {
	    obj = جدید vvp_fun_or(width, نادرست);

      } وگرنه اگر (strcmp(type, "AND") == 0) {
	    obj = جدید vvp_fun_and(width, نادرست);

      } وگرنه اگر (strcmp(type, "BUF") == 0) {
	    obj = جدید vvp_fun_buf(width);

      } وگرنه اگر (strcmp(type, "BUFIF0") == 0) {
	    obj = جدید vvp_fun_bufif(درست,نادرست, ostr0, ostr1);
	    strength_aware = درست;

      } وگرنه اگر (strcmp(type, "BUFIF1") == 0) {
	    obj = جدید vvp_fun_bufif(نادرست,نادرست, ostr0, ostr1);
	    strength_aware = درست;

      } وگرنه اگر (strcmp(type, "EQUIV") == 0) {
	    obj = جدید vvp_fun_equiv();

      } وگرنه اگر (strcmp(type, "IMPL") == 0) {
	    obj = جدید vvp_fun_impl();

      } وگرنه اگر (strcmp(type, "NAND") == 0) {
	    obj = جدید vvp_fun_and(width, درست);

      } وگرنه اگر (strcmp(type, "NOR") == 0) {
	    obj = جدید vvp_fun_or(width, درست);

      } وگرنه اگر (strcmp(type, "NOTIF0") == 0) {
	    obj = جدید vvp_fun_bufif(درست,درست, ostr0, ostr1);
	    strength_aware = درست;

      } وگرنه اگر (strcmp(type, "NOTIF1") == 0) {
	    obj = جدید vvp_fun_bufif(نادرست,درست, ostr0, ostr1);
	    strength_aware = درست;

      } وگرنه اگر (strcmp(type, "BUFT") == 0) {
	    obj = جدید vvp_fun_buft();

      } وگرنه اگر (strcmp(type, "BUFZ") == 0) {
	    obj = جدید vvp_fun_bufz();

      } وگرنه اگر (strcmp(type, "MUXR") == 0) {
	    obj = جدید vvp_fun_muxr;

      } وگرنه اگر (strcmp(type, "MUXZ") == 0) {
	    obj = جدید vvp_fun_muxz(width);

      } وگرنه اگر (strcmp(type, "CMOS") == 0) {
	    obj = جدید vvp_fun_cmos();

      } وگرنه اگر (strcmp(type, "NMOS") == 0) {
	    obj = جدید vvp_fun_pmos(درست);

      } وگرنه اگر (strcmp(type, "PMOS") == 0) {
	    obj = جدید vvp_fun_pmos(نادرست);

      } وگرنه اگر (strcmp(type, "RCMOS") == 0) {
	    obj = جدید vvp_fun_rcmos();

      } وگرنه اگر (strcmp(type, "RNMOS") == 0) {
	    obj = جدید vvp_fun_rpmos(درست);

      } وگرنه اگر (strcmp(type, "RPMOS") == 0) {
	    obj = جدید vvp_fun_rpmos(نادرست);

      } وگرنه اگر (strcmp(type, "NOT") == 0) {
	    obj = جدید vvp_fun_not(width);

      } وگرنه اگر (strcmp(type, "XNOR") == 0) {
	    obj = جدید vvp_fun_xor(width, درست);

      } وگرنه اگر (strcmp(type, "XOR") == 0) {
	    obj = جدید vvp_fun_xor(width, نادرست);

      } وگرنه {
	    yyerror("invalid functor type.");
	    آزاد(type);
	    آزاد(argv);
	    آزاد(label);
	    بازگشت;
      }

      آزاد(type);

      أكد(argc <= 4);
      vvp_net_t*net = جدید vvp_net_t;
      net->fun = obj;

      inputs_connect(net, argc, argv);
      آزاد(argv);

	/* If both the strengths are the default strong drive, then
	   there is no need for a specialized driver. Attach the label
	   to this node and we are finished. */
      اگر (strength_aware || (ostr0 == 6 && ostr1 == 6)) {
	    define_functor_نماد(label, net);
	    آزاد(label);
	    بازگشت;
      }

      vvp_net_t*net_drv = جدید vvp_net_t;
      vvp_net_fun_t*obj_drv = جدید vvp_fun_drive(ostr0, ostr1);
      net_drv->fun = obj_drv;

	/* Point the gate to the drive node. */
      net->link(vvp_net_ptr_t(net_drv, 0));

      define_functor_نماد(label, net_drv);
      آزاد(label);
}
