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
      प्रति (अचिन्हित idx = 0 ;  idx < 4 ;  idx += 1)
	    निवेश_2[idx] = vvp_vector4_t(wid, BIT4_Z);
}

vvp_fun_boolean_::~vvp_fun_boolean_()
{
}

शून्य vvp_fun_boolean_::recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                                 vvp_context_t)
{
      अचिन्हित port = ptr.port();
      यदि (निवेश_2[port] .eeq( bit )) प्रत्यागम;

      निवेश_2[port] = bit;
      यदि (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(एतत्);
      }
}

शून्य vvp_fun_boolean_::recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
				    अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      अचिन्हित port = ptr.port();

      प्रतिज्ञा(base + bit.size() <= vwid);

	// Set the part for the input. If nothing changes, then break.
      तार्किक flag = निवेश_2[port] .set_vec(base, bit);
      यदि (flag == असत्य)
	    प्रत्यागम;

      यदि (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(एतत्);
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
      गिनती_functors_logic += 1;
}

vvp_fun_and::~vvp_fun_and()
{
}

शून्य vvp_fun_and::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (निवेश_2[0]);

      प्रति (अचिन्हित pdx = 1 ; pdx < 4 ; pdx += 1)
	    result &= निवेश_2[pdx];

      यदि (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_equiv::vvp_fun_equiv()
: vvp_fun_boolean_(1)
{
      गिनती_functors_logic += 1;
}

vvp_fun_equiv::~vvp_fun_equiv()
{
}

शून्य vvp_fun_equiv::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      प्रतिज्ञा(निवेश_2[0].size() == 1);
      प्रतिज्ञा(निवेश_2[1].size() == 1);

      vvp_bit4_t bit = ~(निवेश_2[0].value(0) ^ निवेश_2[1].value(0));
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_impl::vvp_fun_impl()
: vvp_fun_boolean_(1)
{
      गिनती_functors_logic += 1;
}

vvp_fun_impl::~vvp_fun_impl()
{
}

शून्य vvp_fun_impl::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      प्रतिज्ञा(निवेश_2[0].size() == 1);
      प्रतिज्ञा(निवेश_2[1].size() == 1);

      vvp_bit4_t bit = ~निवेश_2[0].value(0) | निवेश_2[1].value(0);
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_buf_not_::vvp_fun_buf_not_(अचिन्हित wid)
: निवेश_2(wid, BIT4_Z)
{
      net_ = 0;
}

vvp_fun_buf_not_::~vvp_fun_buf_not_()
{
}

शून्य vvp_fun_buf_not_::recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                                 vvp_context_t)
{
      यदि (ptr.port() != 0) प्रत्यागम;

      यदि (निवेश_2 .eeq( bit )) प्रत्यागम;

      निवेश_2 = bit;
      यदि (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(एतत्);
      }
}

शून्य vvp_fun_buf_not_::recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                                    अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      यदि (ptr.port() != 0) प्रत्यागम;

      प्रतिज्ञा(base + bit.size() <= vwid);

	// Set the input part. If nothing changes, then break.
      तार्किक flag = निवेश_2.set_vec(base, bit);
      यदि (flag == असत्य) प्रत्यागम;

      यदि (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(एतत्);
      }
}

शून्य vvp_fun_buf_not_::recv_real(vvp_net_ptr_t ptr, द्विगुण real,
                                 vvp_context_t ctx)
{
      यदि (ptr.port() != 0) प्रत्यागम;

      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_buf::vvp_fun_buf(अचिन्हित wid)
: vvp_fun_buf_not_(wid)
{
      गिनती_functors_logic += 1;
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

      vvp_vector4_t tmp (निवेश_2);
      tmp.change_z2x();
      ptr->send_vec4(tmp, 0);
}

vvp_fun_bufz::vvp_fun_bufz()
{
      गिनती_functors_logic += 1;
}

vvp_fun_bufz::~vvp_fun_bufz()
{
}

/*
 * The bufz is similar to the buf device, except that it does not
 * bother translating z bits to x.
 */
शून्य vvp_fun_bufz::recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                             vvp_context_t)
{
      यदि (ptr.port() != 0)
	    प्रत्यागम;

      ptr.ptr()->send_vec4(bit, 0);
}

शून्य vvp_fun_bufz::recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                                अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      यदि (ptr.port() != 0)
	    प्रत्यागम;

      ptr.ptr()->send_vec4_pv(bit, base, vwid, 0);
}

शून्य vvp_fun_bufz::recv_real(vvp_net_ptr_t ptr, द्विगुण bit,
                             vvp_context_t)
{
      यदि (ptr.port() != 0)
	    प्रत्यागम;

      ptr.ptr()->send_real(bit, 0);
}

शून्य vvp_fun_buft::recv_vec8(vvp_net_ptr_t ptr, ध्रुव vvp_vector8_t&bit)
{
      यदि (ptr.port() != 0)
	    प्रत्यागम;

      ptr.ptr()->send_vec8(bit);
}

vvp_fun_muxr::vvp_fun_muxr()
: a_(0.0), b_(0.0)
{
      net_ = 0;
      गिनती_functors_logic += 1;
      select_ = SEL_BOTH;
}

vvp_fun_muxr::~vvp_fun_muxr()
{
}

शून्य vvp_fun_muxr::recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                             vvp_context_t)
{
	/* The real valued mux can only take in the select as a
	   vector4_t. The muxed data is real. */
      यदि (ptr.port() != 2)
	    प्रत्यागम;

      प्रतिज्ञा(bit.size() == 1);

      विकल्प (bit.value(0)) {
	  पक्ष BIT4_0:
	    यदि (select_ == SEL_PORT0) प्रत्यागम;
	    select_ = SEL_PORT0;
	    भङ्ग;
	  पक्ष BIT4_1:
	    यदि (select_ == SEL_PORT1) प्रत्यागम;
	    select_ = SEL_PORT1;
	    भङ्ग;
	  स्वभाव:
	    यदि (select_ == SEL_BOTH) प्रत्यागम;
	    select_ = SEL_BOTH;
      }

      यदि (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(एतत्);
      }
}

शून्य vvp_fun_muxr::recv_real(vvp_net_ptr_t ptr, द्विगुण bit,
                             vvp_context_t)
{
      विकल्प (ptr.port()) {
	  पक्ष 0:
	    यदि (a_ == bit) प्रत्यागम;
	    a_ = bit;
	    यदि (select_ == SEL_PORT1) प्रत्यागम; // The other port is selected.
	    भङ्ग;

	  पक्ष 1:
	    यदि (b_ == bit) प्रत्यागम;
	    b_ = bit;
	    यदि (select_ == SEL_PORT0) प्रत्यागम; // The other port is selected.
	    भङ्ग;

	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "Unsupported port type %u.\n", ptr.port());
	    प्रतिज्ञा(0);
      }

      यदि (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(एतत्);
      }
}

शून्य vvp_fun_muxr::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      विकल्प (select_) {
	  पक्ष SEL_PORT0:
	    ptr->send_real(a_, 0);
	    भङ्ग;
	  पक्ष SEL_PORT1:
	    ptr->send_real(b_, 0);
	    भङ्ग;
	  स्वभाव:
	    यदि (a_ == b_) {
		  ptr->send_real(a_, 0);
	    } अन्यथा {
		  ptr->send_real(0.0, 0); // Should this be NaN?
	    }
	    भङ्ग;
      }
}

vvp_fun_muxz::vvp_fun_muxz(अचिन्हित wid)
: a_(wid, BIT4_Z), b_(wid, BIT4_Z)
{
      net_ = 0;
      गिनती_functors_logic += 1;
      select_ = SEL_BOTH;
      has_run_ = असत्य;
}

vvp_fun_muxz::~vvp_fun_muxz()
{
}

शून्य vvp_fun_muxz::recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                             vvp_context_t)
{
      विकल्प (ptr.port()) {
	  पक्ष 0:
	    यदि (a_ .eeq(bit) && has_run_) प्रत्यागम;
	    a_ = bit;
	    यदि (select_ == SEL_PORT1) प्रत्यागम; // The other port is selected.
	    भङ्ग;
	  पक्ष 1:
	    यदि (b_ .eeq(bit) && has_run_) प्रत्यागम;
	    b_ = bit;
	    यदि (select_ == SEL_PORT0) प्रत्यागम; // The other port is selected.
	    भङ्ग;
	  पक्ष 2:
	    प्रतिज्ञा(bit.size() == 1);
	    विकल्प (bit.value(0)) {
		पक्ष BIT4_0:
		  यदि (select_ == SEL_PORT0) प्रत्यागम;
		  select_ = SEL_PORT0;
		  भङ्ग;
		पक्ष BIT4_1:
		  यदि (select_ == SEL_PORT1) प्रत्यागम;
		  select_ = SEL_PORT1;
		  भङ्ग;
		स्वभाव:
		  यदि (select_ == SEL_BOTH && has_run_) प्रत्यागम;
		  select_ = SEL_BOTH;
	    }
	    भङ्ग;
	  स्वभाव:
	    प्रत्यागम;
      }

      यदि (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(एतत्);
      }
}

शून्य vvp_fun_muxz::recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
				अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      प्रतिज्ञा(base + bit.size() <= vwid);
      तार्किक flag;

      विकल्प (ptr.port()) {
	  पक्ष 0:
	    flag = a_.set_vec(base, bit);
	    यदि (flag == असत्य && has_run_) प्रत्यागम;
	    यदि (select_ == SEL_PORT1) प्रत्यागम; // The other port is selected.
	    भङ्ग;
	  पक्ष 1:
	    flag = b_.set_vec(base, bit);
	    यदि (flag == असत्य && has_run_) प्रत्यागम;
	    यदि (select_ == SEL_PORT0) प्रत्यागम; // The other port is selected.
	    भङ्ग;
	  पक्ष 2:
	    प्रतिज्ञा((base == 0) && (bit.size() == 1));
	    recv_vec4(ptr, bit, 0);
	  स्वभाव:
	    प्रत्यागम;
      }
      यदि (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(एतत्);
      }
}

शून्य vvp_fun_muxz::run_run()
{
      has_run_ = सत्य;
      vvp_net_t*ptr = net_;
      net_ = 0;

      विकल्प (select_) {
	  पक्ष SEL_PORT0:
	    ptr->send_vec4(a_, 0);
	    भङ्ग;
	  पक्ष SEL_PORT1:
	    ptr->send_vec4(b_, 0);
	    भङ्ग;
	  स्वभाव:
	      {
		    अचिन्हित min_size = a_.size();
		    अचिन्हित max_size = min_size;
		    यदि (b_.size() < min_size)
			  min_size = b_.size();
		    यदि (b_.size() > max_size)
			  max_size = b_.size();

		    vvp_vector4_t res (max_size);

		    प्रति (अचिन्हित idx = 0 ;  idx < min_size ;  idx += 1) {
			  यदि (a_.value(idx) == b_.value(idx))
				res.set_bit(idx, a_.value(idx));
			  अन्यथा
				res.set_bit(idx, BIT4_X);
		    }

		    प्रति (अचिन्हित idx = min_size ;  idx < max_size ;  idx += 1)
			  res.set_bit(idx, BIT4_X);

		    ptr->send_vec4(res, 0);
	      }
	    भङ्ग;
      }
}

vvp_fun_not::vvp_fun_not(अचिन्हित wid)
: vvp_fun_buf_not_(wid)
{
      गिनती_functors_logic += 1;
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

      vvp_vector4_t result (निवेश_2, सत्य /* invert */);
      ptr->send_vec4(result, 0);
}

vvp_fun_or::vvp_fun_or(अचिन्हित wid, तार्किक invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      गिनती_functors_logic += 1;
}

vvp_fun_or::~vvp_fun_or()
{
}

शून्य vvp_fun_or::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (निवेश_2[0]);

      प्रति (अचिन्हित pdx = 1 ; pdx < 4 ; pdx += 1)
	    result |= निवेश_2[pdx];

      यदि (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_xor::vvp_fun_xor(अचिन्हित wid, तार्किक invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      गिनती_functors_logic += 1;
}

vvp_fun_xor::~vvp_fun_xor()
{
}

शून्य vvp_fun_xor::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (निवेश_2[0]);

      प्रति (अचिन्हित pdx = 1 ; pdx < 4 ; pdx += 1)
	    result ^= निवेश_2[pdx];

      यदि (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

/*
 * The parser calls this function to create a logic functor. I allocate a
 * functor, and map the name to the vvp_ipoint_t address for the
 * functor. Also resolve the inputs to the functor.
 */

शून्य compile_functor(वर्ण*label, वर्ण*type, अचिन्हित width,
		     अचिन्हित ostr0, अचिन्हित ostr1,
		     अचिन्हित argc, संरचना symb_s*argv)
{
      vvp_net_fun_t* obj = 0;
      तार्किक strength_aware = असत्य;

      यदि (म_भेद(type, "OR") == 0) {
	    obj = नूतन vvp_fun_or(width, असत्य);

      } अन्यथा यदि (म_भेद(type, "AND") == 0) {
	    obj = नूतन vvp_fun_and(width, असत्य);

      } अन्यथा यदि (म_भेद(type, "BUF") == 0) {
	    obj = नूतन vvp_fun_buf(width);

      } अन्यथा यदि (म_भेद(type, "BUFIF0") == 0) {
	    obj = नूतन vvp_fun_bufif(सत्य,असत्य, ostr0, ostr1);
	    strength_aware = सत्य;

      } अन्यथा यदि (म_भेद(type, "BUFIF1") == 0) {
	    obj = नूतन vvp_fun_bufif(असत्य,असत्य, ostr0, ostr1);
	    strength_aware = सत्य;

      } अन्यथा यदि (म_भेद(type, "EQUIV") == 0) {
	    obj = नूतन vvp_fun_equiv();

      } अन्यथा यदि (म_भेद(type, "IMPL") == 0) {
	    obj = नूतन vvp_fun_impl();

      } अन्यथा यदि (म_भेद(type, "NAND") == 0) {
	    obj = नूतन vvp_fun_and(width, सत्य);

      } अन्यथा यदि (म_भेद(type, "NOR") == 0) {
	    obj = नूतन vvp_fun_or(width, सत्य);

      } अन्यथा यदि (म_भेद(type, "NOTIF0") == 0) {
	    obj = नूतन vvp_fun_bufif(सत्य,सत्य, ostr0, ostr1);
	    strength_aware = सत्य;

      } अन्यथा यदि (म_भेद(type, "NOTIF1") == 0) {
	    obj = नूतन vvp_fun_bufif(असत्य,सत्य, ostr0, ostr1);
	    strength_aware = सत्य;

      } अन्यथा यदि (म_भेद(type, "BUFT") == 0) {
	    obj = नूतन vvp_fun_buft();

      } अन्यथा यदि (म_भेद(type, "BUFZ") == 0) {
	    obj = नूतन vvp_fun_bufz();

      } अन्यथा यदि (म_भेद(type, "MUXR") == 0) {
	    obj = नूतन vvp_fun_muxr;

      } अन्यथा यदि (म_भेद(type, "MUXZ") == 0) {
	    obj = नूतन vvp_fun_muxz(width);

      } अन्यथा यदि (म_भेद(type, "CMOS") == 0) {
	    obj = नूतन vvp_fun_cmos();

      } अन्यथा यदि (म_भेद(type, "NMOS") == 0) {
	    obj = नूतन vvp_fun_pmos(सत्य);

      } अन्यथा यदि (म_भेद(type, "PMOS") == 0) {
	    obj = नूतन vvp_fun_pmos(असत्य);

      } अन्यथा यदि (म_भेद(type, "RCMOS") == 0) {
	    obj = नूतन vvp_fun_rcmos();

      } अन्यथा यदि (म_भेद(type, "RNMOS") == 0) {
	    obj = नूतन vvp_fun_rpmos(सत्य);

      } अन्यथा यदि (म_भेद(type, "RPMOS") == 0) {
	    obj = नूतन vvp_fun_rpmos(असत्य);

      } अन्यथा यदि (म_भेद(type, "NOT") == 0) {
	    obj = नूतन vvp_fun_not(width);

      } अन्यथा यदि (म_भेद(type, "XNOR") == 0) {
	    obj = नूतन vvp_fun_xor(width, सत्य);

      } अन्यथा यदि (म_भेद(type, "XOR") == 0) {
	    obj = नूतन vvp_fun_xor(width, असत्य);

      } अन्यथा {
	    yyerror("invalid functor type.");
	    मुक्त(type);
	    मुक्त(argv);
	    मुक्त(label);
	    प्रत्यागम;
      }

      मुक्त(type);

      प्रतिज्ञा(argc <= 4);
      vvp_net_t*net = नूतन vvp_net_t;
      net->fun = obj;

      inputs_connect(net, argc, argv);
      मुक्त(argv);

	/* If both the strengths are the default strong drive, then
	   there is no need for a specialized driver. Attach the label
	   to this node and we are finished. */
      यदि (strength_aware || (ostr0 == 6 && ostr1 == 6)) {
	    define_functor_प्रतीक(label, net);
	    मुक्त(label);
	    प्रत्यागम;
      }

      vvp_net_t*net_drv = नूतन vvp_net_t;
      vvp_net_fun_t*obj_drv = नूतन vvp_fun_drive(ostr0, ostr1);
      net_drv->fun = obj_drv;

	/* Point the gate to the drive node. */
      net->link(vvp_net_ptr_t(net_drv, 0));

      define_functor_प्रतीक(label, net_drv);
      मुक्त(label);
}
