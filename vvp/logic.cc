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
      क्रम (अचिन्हित idx = 0 ;  idx < 4 ;  idx += 1)
	    निवेश_2[idx] = vvp_vector4_t(wid, BIT4_Z);
}

vvp_fun_boolean_::~vvp_fun_boolean_()
{
}

व्योम vvp_fun_boolean_::recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                                 vvp_context_t)
{
      अचिन्हित port = ptr.port();
      अगर (निवेश_2[port] .eeq( bit )) वापस;

      निवेश_2[port] = bit;
      अगर (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(यह);
      }
}

व्योम vvp_fun_boolean_::recv_vec4_pv(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
				    अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      अचिन्हित port = ptr.port();

      निश्चित(base + bit.size() <= vwid);

	// Set the part for the input. If nothing changes, then break.
      bool flag = निवेश_2[port] .set_vec(base, bit);
      अगर (flag == false)
	    वापस;

      अगर (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(यह);
      }
}

व्योम vvp_fun_boolean_::recv_real(vvp_net_ptr_t ptr, द्विगुन real,
                                 vvp_context_t ctx)
{
      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_and::vvp_fun_and(अचिन्हित wid, bool invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      गिनती_functors_logic += 1;
}

vvp_fun_and::~vvp_fun_and()
{
}

व्योम vvp_fun_and::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t परिणाम (निवेश_2[0]);

      क्रम (अचिन्हित pdx = 1 ; pdx < 4 ; pdx += 1)
	    परिणाम &= निवेश_2[pdx];

      अगर (invert_)
	    परिणाम.invert();

      ptr->send_vec4(परिणाम, 0);
}

vvp_fun_equiv::vvp_fun_equiv()
: vvp_fun_boolean_(1)
{
      गिनती_functors_logic += 1;
}

vvp_fun_equiv::~vvp_fun_equiv()
{
}

व्योम vvp_fun_equiv::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      निश्चित(निवेश_2[0].size() == 1);
      निश्चित(निवेश_2[1].size() == 1);

      vvp_bit4_t bit = ~(निवेश_2[0].value(0) ^ निवेश_2[1].value(0));
      vvp_vector4_t परिणाम (1, bit);

      ptr->send_vec4(परिणाम, 0);
}

vvp_fun_impl::vvp_fun_impl()
: vvp_fun_boolean_(1)
{
      गिनती_functors_logic += 1;
}

vvp_fun_impl::~vvp_fun_impl()
{
}

व्योम vvp_fun_impl::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      निश्चित(निवेश_2[0].size() == 1);
      निश्चित(निवेश_2[1].size() == 1);

      vvp_bit4_t bit = ~निवेश_2[0].value(0) | निवेश_2[1].value(0);
      vvp_vector4_t परिणाम (1, bit);

      ptr->send_vec4(परिणाम, 0);
}

vvp_fun_buf_not_::vvp_fun_buf_not_(अचिन्हित wid)
: निवेश_2(wid, BIT4_Z)
{
      net_ = 0;
}

vvp_fun_buf_not_::~vvp_fun_buf_not_()
{
}

व्योम vvp_fun_buf_not_::recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                                 vvp_context_t)
{
      अगर (ptr.port() != 0) वापस;

      अगर (निवेश_2 .eeq( bit )) वापस;

      निवेश_2 = bit;
      अगर (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(यह);
      }
}

व्योम vvp_fun_buf_not_::recv_vec4_pv(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                                    अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      अगर (ptr.port() != 0) वापस;

      निश्चित(base + bit.size() <= vwid);

	// Set the input part. If nothing changes, then break.
      bool flag = निवेश_2.set_vec(base, bit);
      अगर (flag == false) वापस;

      अगर (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(यह);
      }
}

व्योम vvp_fun_buf_not_::recv_real(vvp_net_ptr_t ptr, द्विगुन real,
                                 vvp_context_t ctx)
{
      अगर (ptr.port() != 0) वापस;

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
व्योम vvp_fun_buf::run_run()
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
व्योम vvp_fun_bufz::recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                             vvp_context_t)
{
      अगर (ptr.port() != 0)
	    वापस;

      ptr.ptr()->send_vec4(bit, 0);
}

व्योम vvp_fun_bufz::recv_vec4_pv(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                                अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      अगर (ptr.port() != 0)
	    वापस;

      ptr.ptr()->send_vec4_pv(bit, base, vwid, 0);
}

व्योम vvp_fun_bufz::recv_real(vvp_net_ptr_t ptr, द्विगुन bit,
                             vvp_context_t)
{
      अगर (ptr.port() != 0)
	    वापस;

      ptr.ptr()->send_real(bit, 0);
}

व्योम vvp_fun_buft::recv_vec8(vvp_net_ptr_t ptr, स्थिर vvp_vector8_t&bit)
{
      अगर (ptr.port() != 0)
	    वापस;

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

व्योम vvp_fun_muxr::recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                             vvp_context_t)
{
	/* The real valued mux can only take in the select as a
	   vector4_t. The muxed data is real. */
      अगर (ptr.port() != 2)
	    वापस;

      निश्चित(bit.size() == 1);

      चयन (bit.value(0)) {
	  हाल BIT4_0:
	    अगर (select_ == SEL_PORT0) वापस;
	    select_ = SEL_PORT0;
	    अवरोध;
	  हाल BIT4_1:
	    अगर (select_ == SEL_PORT1) वापस;
	    select_ = SEL_PORT1;
	    अवरोध;
	  शेष:
	    अगर (select_ == SEL_BOTH) वापस;
	    select_ = SEL_BOTH;
      }

      अगर (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(यह);
      }
}

व्योम vvp_fun_muxr::recv_real(vvp_net_ptr_t ptr, द्विगुन bit,
                             vvp_context_t)
{
      चयन (ptr.port()) {
	  हाल 0:
	    अगर (a_ == bit) वापस;
	    a_ = bit;
	    अगर (select_ == SEL_PORT1) वापस; // The other port is selected.
	    अवरोध;

	  हाल 1:
	    अगर (b_ == bit) वापस;
	    b_ = bit;
	    अगर (select_ == SEL_PORT0) वापस; // The other port is selected.
	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "Unsupported port type %u.\n", ptr.port());
	    निश्चित(0);
      }

      अगर (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(यह);
      }
}

व्योम vvp_fun_muxr::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      चयन (select_) {
	  हाल SEL_PORT0:
	    ptr->send_real(a_, 0);
	    अवरोध;
	  हाल SEL_PORT1:
	    ptr->send_real(b_, 0);
	    अवरोध;
	  शेष:
	    अगर (a_ == b_) {
		  ptr->send_real(a_, 0);
	    } अन्यथा {
		  ptr->send_real(0.0, 0); // Should this be NaN?
	    }
	    अवरोध;
      }
}

vvp_fun_muxz::vvp_fun_muxz(अचिन्हित wid)
: a_(wid, BIT4_Z), b_(wid, BIT4_Z)
{
      net_ = 0;
      गिनती_functors_logic += 1;
      select_ = SEL_BOTH;
      has_run_ = false;
}

vvp_fun_muxz::~vvp_fun_muxz()
{
}

व्योम vvp_fun_muxz::recv_vec4(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
                             vvp_context_t)
{
      चयन (ptr.port()) {
	  हाल 0:
	    अगर (a_ .eeq(bit) && has_run_) वापस;
	    a_ = bit;
	    अगर (select_ == SEL_PORT1) वापस; // The other port is selected.
	    अवरोध;
	  हाल 1:
	    अगर (b_ .eeq(bit) && has_run_) वापस;
	    b_ = bit;
	    अगर (select_ == SEL_PORT0) वापस; // The other port is selected.
	    अवरोध;
	  हाल 2:
	    निश्चित(bit.size() == 1);
	    चयन (bit.value(0)) {
		हाल BIT4_0:
		  अगर (select_ == SEL_PORT0) वापस;
		  select_ = SEL_PORT0;
		  अवरोध;
		हाल BIT4_1:
		  अगर (select_ == SEL_PORT1) वापस;
		  select_ = SEL_PORT1;
		  अवरोध;
		शेष:
		  अगर (select_ == SEL_BOTH && has_run_) वापस;
		  select_ = SEL_BOTH;
	    }
	    अवरोध;
	  शेष:
	    वापस;
      }

      अगर (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(यह);
      }
}

व्योम vvp_fun_muxz::recv_vec4_pv(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit,
				अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      निश्चित(base + bit.size() <= vwid);
      bool flag;

      चयन (ptr.port()) {
	  हाल 0:
	    flag = a_.set_vec(base, bit);
	    अगर (flag == false && has_run_) वापस;
	    अगर (select_ == SEL_PORT1) वापस; // The other port is selected.
	    अवरोध;
	  हाल 1:
	    flag = b_.set_vec(base, bit);
	    अगर (flag == false && has_run_) वापस;
	    अगर (select_ == SEL_PORT0) वापस; // The other port is selected.
	    अवरोध;
	  हाल 2:
	    निश्चित((base == 0) && (bit.size() == 1));
	    recv_vec4(ptr, bit, 0);
	  शेष:
	    वापस;
      }
      अगर (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(यह);
      }
}

व्योम vvp_fun_muxz::run_run()
{
      has_run_ = true;
      vvp_net_t*ptr = net_;
      net_ = 0;

      चयन (select_) {
	  हाल SEL_PORT0:
	    ptr->send_vec4(a_, 0);
	    अवरोध;
	  हाल SEL_PORT1:
	    ptr->send_vec4(b_, 0);
	    अवरोध;
	  शेष:
	      {
		    अचिन्हित min_आकार = a_.size();
		    अचिन्हित max_आकार = min_आकार;
		    अगर (b_.size() < min_आकार)
			  min_आकार = b_.size();
		    अगर (b_.size() > max_आकार)
			  max_आकार = b_.size();

		    vvp_vector4_t res (max_आकार);

		    क्रम (अचिन्हित idx = 0 ;  idx < min_आकार ;  idx += 1) {
			  अगर (a_.value(idx) == b_.value(idx))
				res.set_bit(idx, a_.value(idx));
			  अन्यथा
				res.set_bit(idx, BIT4_X);
		    }

		    क्रम (अचिन्हित idx = min_आकार ;  idx < max_आकार ;  idx += 1)
			  res.set_bit(idx, BIT4_X);

		    ptr->send_vec4(res, 0);
	      }
	    अवरोध;
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
व्योम vvp_fun_not::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t परिणाम (निवेश_2, true /* invert */);
      ptr->send_vec4(परिणाम, 0);
}

vvp_fun_or::vvp_fun_or(अचिन्हित wid, bool invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      गिनती_functors_logic += 1;
}

vvp_fun_or::~vvp_fun_or()
{
}

व्योम vvp_fun_or::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t परिणाम (निवेश_2[0]);

      क्रम (अचिन्हित pdx = 1 ; pdx < 4 ; pdx += 1)
	    परिणाम |= निवेश_2[pdx];

      अगर (invert_)
	    परिणाम.invert();

      ptr->send_vec4(परिणाम, 0);
}

vvp_fun_xor::vvp_fun_xor(अचिन्हित wid, bool invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      गिनती_functors_logic += 1;
}

vvp_fun_xor::~vvp_fun_xor()
{
}

व्योम vvp_fun_xor::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t परिणाम (निवेश_2[0]);

      क्रम (अचिन्हित pdx = 1 ; pdx < 4 ; pdx += 1)
	    परिणाम ^= निवेश_2[pdx];

      अगर (invert_)
	    परिणाम.invert();

      ptr->send_vec4(परिणाम, 0);
}

/*
 * The parser calls this function to create a logic functor. I allocate a
 * functor, and map the name to the vvp_ipoint_t address for the
 * functor. Also resolve the inputs to the functor.
 */

व्योम compile_functor(अक्षर*label, अक्षर*type, अचिन्हित width,
		     अचिन्हित ostr0, अचिन्हित ostr1,
		     अचिन्हित argc, काष्ठा symb_s*argv)
{
      vvp_net_fun_t* obj = 0;
      bool strength_aware = false;

      अगर (म_भेद(type, "OR") == 0) {
	    obj = नया vvp_fun_or(width, false);

      } अन्यथा अगर (म_भेद(type, "AND") == 0) {
	    obj = नया vvp_fun_and(width, false);

      } अन्यथा अगर (म_भेद(type, "BUF") == 0) {
	    obj = नया vvp_fun_buf(width);

      } अन्यथा अगर (म_भेद(type, "BUFIF0") == 0) {
	    obj = नया vvp_fun_bufif(true,false, ostr0, ostr1);
	    strength_aware = true;

      } अन्यथा अगर (म_भेद(type, "BUFIF1") == 0) {
	    obj = नया vvp_fun_bufif(false,false, ostr0, ostr1);
	    strength_aware = true;

      } अन्यथा अगर (म_भेद(type, "EQUIV") == 0) {
	    obj = नया vvp_fun_equiv();

      } अन्यथा अगर (म_भेद(type, "IMPL") == 0) {
	    obj = नया vvp_fun_impl();

      } अन्यथा अगर (म_भेद(type, "NAND") == 0) {
	    obj = नया vvp_fun_and(width, true);

      } अन्यथा अगर (म_भेद(type, "NOR") == 0) {
	    obj = नया vvp_fun_or(width, true);

      } अन्यथा अगर (म_भेद(type, "NOTIF0") == 0) {
	    obj = नया vvp_fun_bufif(true,true, ostr0, ostr1);
	    strength_aware = true;

      } अन्यथा अगर (म_भेद(type, "NOTIF1") == 0) {
	    obj = नया vvp_fun_bufif(false,true, ostr0, ostr1);
	    strength_aware = true;

      } अन्यथा अगर (म_भेद(type, "BUFT") == 0) {
	    obj = नया vvp_fun_buft();

      } अन्यथा अगर (म_भेद(type, "BUFZ") == 0) {
	    obj = नया vvp_fun_bufz();

      } अन्यथा अगर (म_भेद(type, "MUXR") == 0) {
	    obj = नया vvp_fun_muxr;

      } अन्यथा अगर (म_भेद(type, "MUXZ") == 0) {
	    obj = नया vvp_fun_muxz(width);

      } अन्यथा अगर (म_भेद(type, "CMOS") == 0) {
	    obj = नया vvp_fun_cmos();

      } अन्यथा अगर (म_भेद(type, "NMOS") == 0) {
	    obj = नया vvp_fun_pmos(true);

      } अन्यथा अगर (म_भेद(type, "PMOS") == 0) {
	    obj = नया vvp_fun_pmos(false);

      } अन्यथा अगर (म_भेद(type, "RCMOS") == 0) {
	    obj = नया vvp_fun_rcmos();

      } अन्यथा अगर (म_भेद(type, "RNMOS") == 0) {
	    obj = नया vvp_fun_rpmos(true);

      } अन्यथा अगर (म_भेद(type, "RPMOS") == 0) {
	    obj = नया vvp_fun_rpmos(false);

      } अन्यथा अगर (म_भेद(type, "NOT") == 0) {
	    obj = नया vvp_fun_not(width);

      } अन्यथा अगर (म_भेद(type, "XNOR") == 0) {
	    obj = नया vvp_fun_xor(width, true);

      } अन्यथा अगर (म_भेद(type, "XOR") == 0) {
	    obj = नया vvp_fun_xor(width, false);

      } अन्यथा {
	    yyerror("invalid functor type.");
	    मुक्त(type);
	    मुक्त(argv);
	    मुक्त(label);
	    वापस;
      }

      मुक्त(type);

      निश्चित(argc <= 4);
      vvp_net_t*net = नया vvp_net_t;
      net->fun = obj;

      inputs_connect(net, argc, argv);
      मुक्त(argv);

	/* If both the strengths are the default strong drive, then
	   there is no need for a specialized driver. Attach the label
	   to this node and we are finished. */
      अगर (strength_aware || (ostr0 == 6 && ostr1 == 6)) {
	    define_functor_प्रतीक(label, net);
	    मुक्त(label);
	    वापस;
      }

      vvp_net_t*net_drv = नया vvp_net_t;
      vvp_net_fun_t*obj_drv = नया vvp_fun_drive(ostr0, ostr1);
      net_drv->fun = obj_drv;

	/* Point the gate to the drive node. */
      net->link(vvp_net_ptr_t(net_drv, 0));

      define_functor_प्रतीक(label, net_drv);
      मुक्त(label);
}
