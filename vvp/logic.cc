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

vvp_fun_boolean_::vvp_fun_boolean_(अचिन्हित wid)
{
      net_ = 0;
      साठी (अचिन्हित idx = 0 ;  idx < 4 ;  idx += 1)
	    आदान_2[idx] = vvp_vector4_t(wid, BIT4_Z);
}

vvp_fun_boolean_::~vvp_fun_boolean_()
{
}

शून्य vvp_fun_boolean_::recv_vec4(vvp_net_ptr_t ptr, स्थिरांक vvp_vector4_t&bit,
                                 vvp_context_t)
{
      अचिन्हित port = ptr.port();
      जर (आदान_2[port] .eeq( bit )) परत;

      आदान_2[port] = bit;
      जर (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(हे);
      }
}

शून्य vvp_fun_boolean_::recv_vec4_pv(vvp_net_ptr_t ptr, स्थिरांक vvp_vector4_t&bit,
				    अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      अचिन्हित port = ptr.port();

      प्रतिज्ञा(base + bit.size() <= vwid);

	// Set the part for the input. If nothing changes, then break.
      तार्किक flag = आदान_2[port] .set_vec(base, bit);
      जर (flag == असत्य)
	    परत;

      जर (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(हे);
      }
}

शून्य vvp_fun_boolean_::recv_real(vvp_net_ptr_t ptr, द्विगुण real,
                                 vvp_context_t ctx)
{
      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_and::vvp_fun_and(अचिन्हित wid, तार्किक invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      मोजणी_functors_logic += 1;
}

vvp_fun_and::~vvp_fun_and()
{
}

शून्य vvp_fun_and::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (आदान_2[0]);

      साठी (अचिन्हित pdx = 1 ; pdx < 4 ; pdx += 1)
	    result &= आदान_2[pdx];

      जर (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_equiv::vvp_fun_equiv()
: vvp_fun_boolean_(1)
{
      मोजणी_functors_logic += 1;
}

vvp_fun_equiv::~vvp_fun_equiv()
{
}

शून्य vvp_fun_equiv::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      प्रतिज्ञा(आदान_2[0].size() == 1);
      प्रतिज्ञा(आदान_2[1].size() == 1);

      vvp_bit4_t bit = ~(आदान_2[0].value(0) ^ आदान_2[1].value(0));
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_impl::vvp_fun_impl()
: vvp_fun_boolean_(1)
{
      मोजणी_functors_logic += 1;
}

vvp_fun_impl::~vvp_fun_impl()
{
}

शून्य vvp_fun_impl::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      प्रतिज्ञा(आदान_2[0].size() == 1);
      प्रतिज्ञा(आदान_2[1].size() == 1);

      vvp_bit4_t bit = ~आदान_2[0].value(0) | आदान_2[1].value(0);
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_buf_not_::vvp_fun_buf_not_(अचिन्हित wid)
: आदान_2(wid, BIT4_Z)
{
      net_ = 0;
}

vvp_fun_buf_not_::~vvp_fun_buf_not_()
{
}

शून्य vvp_fun_buf_not_::recv_vec4(vvp_net_ptr_t ptr, स्थिरांक vvp_vector4_t&bit,
                                 vvp_context_t)
{
      जर (ptr.port() != 0) परत;

      जर (आदान_2 .eeq( bit )) परत;

      आदान_2 = bit;
      जर (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(हे);
      }
}

शून्य vvp_fun_buf_not_::recv_vec4_pv(vvp_net_ptr_t ptr, स्थिरांक vvp_vector4_t&bit,
                                    अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      जर (ptr.port() != 0) परत;

      प्रतिज्ञा(base + bit.size() <= vwid);

	// Set the input part. If nothing changes, then break.
      तार्किक flag = आदान_2.set_vec(base, bit);
      जर (flag == असत्य) परत;

      जर (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(हे);
      }
}

शून्य vvp_fun_buf_not_::recv_real(vvp_net_ptr_t ptr, द्विगुण real,
                                 vvp_context_t ctx)
{
      जर (ptr.port() != 0) परत;

      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_buf::vvp_fun_buf(अचिन्हित wid)
: vvp_fun_buf_not_(wid)
{
      मोजणी_functors_logic += 1;
}

vvp_fun_buf::~vvp_fun_buf()
{
}

/*
 * The buf functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the result.
 */
शून्य vvp_fun_buf::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t tmp (आदान_2);
      tmp.change_z2x();
      ptr->send_vec4(tmp, 0);
}

vvp_fun_bufz::vvp_fun_bufz()
{
      मोजणी_functors_logic += 1;
}

vvp_fun_bufz::~vvp_fun_bufz()
{
}

/*
 * The bufz is similar to the buf device, except that it does not
 * bother translating z bits to x.
 */
शून्य vvp_fun_bufz::recv_vec4(vvp_net_ptr_t ptr, स्थिरांक vvp_vector4_t&bit,
                             vvp_context_t)
{
      जर (ptr.port() != 0)
	    परत;

      ptr.ptr()->send_vec4(bit, 0);
}

शून्य vvp_fun_bufz::recv_vec4_pv(vvp_net_ptr_t ptr, स्थिरांक vvp_vector4_t&bit,
                                अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      जर (ptr.port() != 0)
	    परत;

      ptr.ptr()->send_vec4_pv(bit, base, vwid, 0);
}

शून्य vvp_fun_bufz::recv_real(vvp_net_ptr_t ptr, द्विगुण bit,
                             vvp_context_t)
{
      जर (ptr.port() != 0)
	    परत;

      ptr.ptr()->send_real(bit, 0);
}

शून्य vvp_fun_buft::recv_vec8(vvp_net_ptr_t ptr, स्थिरांक vvp_vector8_t&bit)
{
      जर (ptr.port() != 0)
	    परत;

      ptr.ptr()->send_vec8(bit);
}

vvp_fun_muxr::vvp_fun_muxr()
: a_(0.0), b_(0.0)
{
      net_ = 0;
      मोजणी_functors_logic += 1;
      select_ = SEL_BOTH;
}

vvp_fun_muxr::~vvp_fun_muxr()
{
}

शून्य vvp_fun_muxr::recv_vec4(vvp_net_ptr_t ptr, स्थिरांक vvp_vector4_t&bit,
                             vvp_context_t)
{
	/* The real valued mux can only take in the select as a
	   vector4_t. The muxed data is real. */
      जर (ptr.port() != 2)
	    परत;

      प्रतिज्ञा(bit.size() == 1);

      निवड (bit.value(0)) {
	  स्थिती BIT4_0:
	    जर (select_ == SEL_PORT0) परत;
	    select_ = SEL_PORT0;
	    थांबा;
	  स्थिती BIT4_1:
	    जर (select_ == SEL_PORT1) परत;
	    select_ = SEL_PORT1;
	    थांबा;
	  पूर्वनिर्धारित:
	    जर (select_ == SEL_BOTH) परत;
	    select_ = SEL_BOTH;
      }

      जर (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(हे);
      }
}

शून्य vvp_fun_muxr::recv_real(vvp_net_ptr_t ptr, द्विगुण bit,
                             vvp_context_t)
{
      निवड (ptr.port()) {
	  स्थिती 0:
	    जर (a_ == bit) परत;
	    a_ = bit;
	    जर (select_ == SEL_PORT1) परत; // The other port is selected.
	    थांबा;

	  स्थिती 1:
	    जर (b_ == bit) परत;
	    b_ = bit;
	    जर (select_ == SEL_PORT0) परत; // The other port is selected.
	    थांबा;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "Unsupported port type %u.\n", ptr.port());
	    प्रतिज्ञा(0);
      }

      जर (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(हे);
      }
}

शून्य vvp_fun_muxr::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      निवड (select_) {
	  स्थिती SEL_PORT0:
	    ptr->send_real(a_, 0);
	    थांबा;
	  स्थिती SEL_PORT1:
	    ptr->send_real(b_, 0);
	    थांबा;
	  पूर्वनिर्धारित:
	    जर (a_ == b_) {
		  ptr->send_real(a_, 0);
	    } अन्यथा {
		  ptr->send_real(0.0, 0); // Should this be NaN?
	    }
	    थांबा;
      }
}

vvp_fun_muxz::vvp_fun_muxz(अचिन्हित wid)
: a_(wid, BIT4_Z), b_(wid, BIT4_Z)
{
      net_ = 0;
      मोजणी_functors_logic += 1;
      select_ = SEL_BOTH;
      has_run_ = असत्य;
}

vvp_fun_muxz::~vvp_fun_muxz()
{
}

शून्य vvp_fun_muxz::recv_vec4(vvp_net_ptr_t ptr, स्थिरांक vvp_vector4_t&bit,
                             vvp_context_t)
{
      निवड (ptr.port()) {
	  स्थिती 0:
	    जर (a_ .eeq(bit) && has_run_) परत;
	    a_ = bit;
	    जर (select_ == SEL_PORT1) परत; // The other port is selected.
	    थांबा;
	  स्थिती 1:
	    जर (b_ .eeq(bit) && has_run_) परत;
	    b_ = bit;
	    जर (select_ == SEL_PORT0) परत; // The other port is selected.
	    थांबा;
	  स्थिती 2:
	    प्रतिज्ञा(bit.size() == 1);
	    निवड (bit.value(0)) {
		स्थिती BIT4_0:
		  जर (select_ == SEL_PORT0) परत;
		  select_ = SEL_PORT0;
		  थांबा;
		स्थिती BIT4_1:
		  जर (select_ == SEL_PORT1) परत;
		  select_ = SEL_PORT1;
		  थांबा;
		पूर्वनिर्धारित:
		  जर (select_ == SEL_BOTH && has_run_) परत;
		  select_ = SEL_BOTH;
	    }
	    थांबा;
	  पूर्वनिर्धारित:
	    परत;
      }

      जर (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(हे);
      }
}

शून्य vvp_fun_muxz::recv_vec4_pv(vvp_net_ptr_t ptr, स्थिरांक vvp_vector4_t&bit,
				अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      प्रतिज्ञा(base + bit.size() <= vwid);
      तार्किक flag;

      निवड (ptr.port()) {
	  स्थिती 0:
	    flag = a_.set_vec(base, bit);
	    जर (flag == असत्य && has_run_) परत;
	    जर (select_ == SEL_PORT1) परत; // The other port is selected.
	    थांबा;
	  स्थिती 1:
	    flag = b_.set_vec(base, bit);
	    जर (flag == असत्य && has_run_) परत;
	    जर (select_ == SEL_PORT0) परत; // The other port is selected.
	    थांबा;
	  स्थिती 2:
	    प्रतिज्ञा((base == 0) && (bit.size() == 1));
	    recv_vec4(ptr, bit, 0);
	  पूर्वनिर्धारित:
	    परत;
      }
      जर (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(हे);
      }
}

शून्य vvp_fun_muxz::run_run()
{
      has_run_ = सत्य;
      vvp_net_t*ptr = net_;
      net_ = 0;

      निवड (select_) {
	  स्थिती SEL_PORT0:
	    ptr->send_vec4(a_, 0);
	    थांबा;
	  स्थिती SEL_PORT1:
	    ptr->send_vec4(b_, 0);
	    थांबा;
	  पूर्वनिर्धारित:
	      {
		    अचिन्हित min_size = a_.size();
		    अचिन्हित max_size = min_size;
		    जर (b_.size() < min_size)
			  min_size = b_.size();
		    जर (b_.size() > max_size)
			  max_size = b_.size();

		    vvp_vector4_t res (max_size);

		    साठी (अचिन्हित idx = 0 ;  idx < min_size ;  idx += 1) {
			  जर (a_.value(idx) == b_.value(idx))
				res.set_bit(idx, a_.value(idx));
			  अन्यथा
				res.set_bit(idx, BIT4_X);
		    }

		    साठी (अचिन्हित idx = min_size ;  idx < max_size ;  idx += 1)
			  res.set_bit(idx, BIT4_X);

		    ptr->send_vec4(res, 0);
	      }
	    थांबा;
      }
}

vvp_fun_not::vvp_fun_not(अचिन्हित wid)
: vvp_fun_buf_not_(wid)
{
      मोजणी_functors_logic += 1;
}

vvp_fun_not::~vvp_fun_not()
{
}

/*
 * The not functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the inverted result.
 */
शून्य vvp_fun_not::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (आदान_2, सत्य /* invert */);
      ptr->send_vec4(result, 0);
}

vvp_fun_or::vvp_fun_or(अचिन्हित wid, तार्किक invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      मोजणी_functors_logic += 1;
}

vvp_fun_or::~vvp_fun_or()
{
}

शून्य vvp_fun_or::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (आदान_2[0]);

      साठी (अचिन्हित pdx = 1 ; pdx < 4 ; pdx += 1)
	    result |= आदान_2[pdx];

      जर (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_xor::vvp_fun_xor(अचिन्हित wid, तार्किक invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      मोजणी_functors_logic += 1;
}

vvp_fun_xor::~vvp_fun_xor()
{
}

शून्य vvp_fun_xor::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (आदान_2[0]);

      साठी (अचिन्हित pdx = 1 ; pdx < 4 ; pdx += 1)
	    result ^= आदान_2[pdx];

      जर (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

/*
 * The parser calls this function to create a logic functor. I allocate a
 * functor, and map the name to the vvp_ipoint_t address for the
 * functor. Also resolve the inputs to the functor.
 */

शून्य compile_functor(अक्षर*label, अक्षर*type, अचिन्हित width,
		     अचिन्हित ostr0, अचिन्हित ostr1,
		     अचिन्हित argc, संरचना symb_s*argv)
{
      vvp_net_fun_t* obj = 0;
      तार्किक strength_aware = असत्य;

      जर (म_भेद(type, "OR") == 0) {
	    obj = नवीन vvp_fun_or(width, असत्य);

      } अन्यथा जर (म_भेद(type, "AND") == 0) {
	    obj = नवीन vvp_fun_and(width, असत्य);

      } अन्यथा जर (म_भेद(type, "BUF") == 0) {
	    obj = नवीन vvp_fun_buf(width);

      } अन्यथा जर (म_भेद(type, "BUFIF0") == 0) {
	    obj = नवीन vvp_fun_bufif(सत्य,असत्य, ostr0, ostr1);
	    strength_aware = सत्य;

      } अन्यथा जर (म_भेद(type, "BUFIF1") == 0) {
	    obj = नवीन vvp_fun_bufif(असत्य,असत्य, ostr0, ostr1);
	    strength_aware = सत्य;

      } अन्यथा जर (म_भेद(type, "EQUIV") == 0) {
	    obj = नवीन vvp_fun_equiv();

      } अन्यथा जर (म_भेद(type, "IMPL") == 0) {
	    obj = नवीन vvp_fun_impl();

      } अन्यथा जर (म_भेद(type, "NAND") == 0) {
	    obj = नवीन vvp_fun_and(width, सत्य);

      } अन्यथा जर (म_भेद(type, "NOR") == 0) {
	    obj = नवीन vvp_fun_or(width, सत्य);

      } अन्यथा जर (म_भेद(type, "NOTIF0") == 0) {
	    obj = नवीन vvp_fun_bufif(सत्य,सत्य, ostr0, ostr1);
	    strength_aware = सत्य;

      } अन्यथा जर (म_भेद(type, "NOTIF1") == 0) {
	    obj = नवीन vvp_fun_bufif(असत्य,सत्य, ostr0, ostr1);
	    strength_aware = सत्य;

      } अन्यथा जर (म_भेद(type, "BUFT") == 0) {
	    obj = नवीन vvp_fun_buft();

      } अन्यथा जर (म_भेद(type, "BUFZ") == 0) {
	    obj = नवीन vvp_fun_bufz();

      } अन्यथा जर (म_भेद(type, "MUXR") == 0) {
	    obj = नवीन vvp_fun_muxr;

      } अन्यथा जर (म_भेद(type, "MUXZ") == 0) {
	    obj = नवीन vvp_fun_muxz(width);

      } अन्यथा जर (म_भेद(type, "CMOS") == 0) {
	    obj = नवीन vvp_fun_cmos();

      } अन्यथा जर (म_भेद(type, "NMOS") == 0) {
	    obj = नवीन vvp_fun_pmos(सत्य);

      } अन्यथा जर (म_भेद(type, "PMOS") == 0) {
	    obj = नवीन vvp_fun_pmos(असत्य);

      } अन्यथा जर (म_भेद(type, "RCMOS") == 0) {
	    obj = नवीन vvp_fun_rcmos();

      } अन्यथा जर (म_भेद(type, "RNMOS") == 0) {
	    obj = नवीन vvp_fun_rpmos(सत्य);

      } अन्यथा जर (म_भेद(type, "RPMOS") == 0) {
	    obj = नवीन vvp_fun_rpmos(असत्य);

      } अन्यथा जर (म_भेद(type, "NOT") == 0) {
	    obj = नवीन vvp_fun_not(width);

      } अन्यथा जर (म_भेद(type, "XNOR") == 0) {
	    obj = नवीन vvp_fun_xor(width, सत्य);

      } अन्यथा जर (म_भेद(type, "XOR") == 0) {
	    obj = नवीन vvp_fun_xor(width, असत्य);

      } अन्यथा {
	    yyerror("invalid functor type.");
	    मुक्त(type);
	    मुक्त(argv);
	    मुक्त(label);
	    परत;
      }

      मुक्त(type);

      प्रतिज्ञा(argc <= 4);
      vvp_net_t*net = नवीन vvp_net_t;
      net->fun = obj;

      inputs_connect(net, argc, argv);
      मुक्त(argv);

	/* If both the strengths are the default strong drive, then
	   there is no need for a specialized driver. Attach the label
	   to this node and we are finished. */
      जर (strength_aware || (ostr0 == 6 && ostr1 == 6)) {
	    define_functor_चिन्ह(label, net);
	    मुक्त(label);
	    परत;
      }

      vvp_net_t*net_drv = नवीन vvp_net_t;
      vvp_net_fun_t*obj_drv = नवीन vvp_fun_drive(ostr0, ostr1);
      net_drv->fun = obj_drv;

	/* Point the gate to the drive node. */
      net->link(vvp_net_ptr_t(net_drv, 0));

      define_functor_चिन्ह(label, net_drv);
      मुक्त(label);
}
