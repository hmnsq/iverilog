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

vvp_fun_boolean_::vvp_fun_boolean_(अचिह्नित wid)
{
      net_ = 0;
      प्रति (अचिह्नित idx = 0 ;  idx < 4 ;  idx += 1)
	    निवेशः_2[idx] = vvp_vector4_t(wid, BIT4_Z);
}

vvp_fun_boolean_::~vvp_fun_boolean_()
{
}

शून्य vvp_fun_boolean_::recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                                 vvp_context_t)
{
      अचिह्नित पत्तनम् = ptr.पत्तनम्();
      यदि (निवेशः_2[पत्तनम्] .eeq( bit )) प्रत्यागम;

      निवेशः_2[पत्तनम्] = bit;
      यदि (net_ == 0) {
	    net_ = ptr.ptr();
	    कालसारणी_functor(एतत्);
      }
}

शून्य vvp_fun_boolean_::recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
				    अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t)
{
      अचिह्नित पत्तनम् = ptr.पत्तनम्();

      प्रतिज्ञा(आधारः + bit.size() <= vwid);

	// Set the part for the input. If nothing changes, then break.
      तार्किक flag = निवेशः_2[पत्तनम्] .समुच्चयः_vec(आधारः, bit);
      यदि (flag == असत्य)
	    प्रत्यागम;

      यदि (net_ == 0) {
	    net_ = ptr.ptr();
	    कालसारणी_functor(एतत्);
      }
}

शून्य vvp_fun_boolean_::recv_real(vvp_net_ptr_t ptr, द्विगुण real,
                                 vvp_context_t ctx)
{
      recv_vec4(ptr, double_गन्तव्यम्_vector4_LSB(real), ctx);
}

vvp_fun_and::vvp_fun_and(अचिह्नित wid, तार्किक invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      गणनम्_functors_logic += 1;
}

vvp_fun_and::~vvp_fun_and()
{
}

शून्य vvp_fun_and::सततम्_सततम्()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t परिणाम (निवेशः_2[0]);

      प्रति (अचिह्नित pdx = 1 ; pdx < 4 ; pdx += 1)
	    परिणाम &= निवेशः_2[pdx];

      यदि (invert_)
	    परिणाम.invert();

      ptr->send_vec4(परिणाम, 0);
}

vvp_fun_equiv::vvp_fun_equiv()
: vvp_fun_boolean_(1)
{
      गणनम्_functors_logic += 1;
}

vvp_fun_equiv::~vvp_fun_equiv()
{
}

शून्य vvp_fun_equiv::सततम्_सततम्()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      प्रतिज्ञा(निवेशः_2[0].size() == 1);
      प्रतिज्ञा(निवेशः_2[1].size() == 1);

      vvp_bit4_t bit = ~(निवेशः_2[0].value(0) ^ निवेशः_2[1].value(0));
      vvp_vector4_t परिणाम (1, bit);

      ptr->send_vec4(परिणाम, 0);
}

vvp_fun_impl::vvp_fun_impl()
: vvp_fun_boolean_(1)
{
      गणनम्_functors_logic += 1;
}

vvp_fun_impl::~vvp_fun_impl()
{
}

शून्य vvp_fun_impl::सततम्_सततम्()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      प्रतिज्ञा(निवेशः_2[0].size() == 1);
      प्रतिज्ञा(निवेशः_2[1].size() == 1);

      vvp_bit4_t bit = ~निवेशः_2[0].value(0) | निवेशः_2[1].value(0);
      vvp_vector4_t परिणाम (1, bit);

      ptr->send_vec4(परिणाम, 0);
}

vvp_fun_buf_not_::vvp_fun_buf_not_(अचिह्नित wid)
: निवेशः_2(wid, BIT4_Z)
{
      net_ = 0;
}

vvp_fun_buf_not_::~vvp_fun_buf_not_()
{
}

शून्य vvp_fun_buf_not_::recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                                 vvp_context_t)
{
      यदि (ptr.पत्तनम्() != 0) प्रत्यागम;

      यदि (निवेशः_2 .eeq( bit )) प्रत्यागम;

      निवेशः_2 = bit;
      यदि (net_ == 0) {
	    net_ = ptr.ptr();
	    कालसारणी_functor(एतत्);
      }
}

शून्य vvp_fun_buf_not_::recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                                    अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t)
{
      यदि (ptr.पत्तनम्() != 0) प्रत्यागम;

      प्रतिज्ञा(आधारः + bit.size() <= vwid);

	// Set the input part. If nothing changes, then break.
      तार्किक flag = निवेशः_2.समुच्चयः_vec(आधारः, bit);
      यदि (flag == असत्य) प्रत्यागम;

      यदि (net_ == 0) {
	    net_ = ptr.ptr();
	    कालसारणी_functor(एतत्);
      }
}

शून्य vvp_fun_buf_not_::recv_real(vvp_net_ptr_t ptr, द्विगुण real,
                                 vvp_context_t ctx)
{
      यदि (ptr.पत्तनम्() != 0) प्रत्यागम;

      recv_vec4(ptr, double_गन्तव्यम्_vector4_LSB(real), ctx);
}

vvp_fun_buf::vvp_fun_buf(अचिह्नित wid)
: vvp_fun_buf_not_(wid)
{
      गणनम्_functors_logic += 1;
}

vvp_fun_buf::~vvp_fun_buf()
{
}

/*
 * The buf functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the result.
 */
शून्य vvp_fun_buf::सततम्_सततम्()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t tmp (निवेशः_2);
      tmp.change_z2x();
      ptr->send_vec4(tmp, 0);
}

vvp_fun_bufz::vvp_fun_bufz()
{
      गणनम्_functors_logic += 1;
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
      यदि (ptr.पत्तनम्() != 0)
	    प्रत्यागम;

      ptr.ptr()->send_vec4(bit, 0);
}

शून्य vvp_fun_bufz::recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                                अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t)
{
      यदि (ptr.पत्तनम्() != 0)
	    प्रत्यागम;

      ptr.ptr()->send_vec4_pv(bit, आधारः, vwid, 0);
}

शून्य vvp_fun_bufz::recv_real(vvp_net_ptr_t ptr, द्विगुण bit,
                             vvp_context_t)
{
      यदि (ptr.पत्तनम्() != 0)
	    प्रत्यागम;

      ptr.ptr()->send_real(bit, 0);
}

शून्य vvp_fun_buft::recv_vec8(vvp_net_ptr_t ptr, ध्रुव vvp_vector8_t&bit)
{
      यदि (ptr.पत्तनम्() != 0)
	    प्रत्यागम;

      ptr.ptr()->send_vec8(bit);
}

vvp_fun_muxr::vvp_fun_muxr()
: a_(0.0), b_(0.0)
{
      net_ = 0;
      गणनम्_functors_logic += 1;
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
      यदि (ptr.पत्तनम्() != 2)
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
	    कालसारणी_functor(एतत्);
      }
}

शून्य vvp_fun_muxr::recv_real(vvp_net_ptr_t ptr, द्विगुण bit,
                             vvp_context_t)
{
      विकल्प (ptr.पत्तनम्()) {
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
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.पत्तनम्());
	    प्रतिज्ञा(0);
      }

      यदि (net_ == 0) {
	    net_ = ptr.ptr();
	    कालसारणी_functor(एतत्);
      }
}

शून्य vvp_fun_muxr::सततम्_सततम्()
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

vvp_fun_muxz::vvp_fun_muxz(अचिह्नित wid)
: a_(wid, BIT4_Z), b_(wid, BIT4_Z)
{
      net_ = 0;
      गणनम्_functors_logic += 1;
      select_ = SEL_BOTH;
      has_सततम् = असत्य;
}

vvp_fun_muxz::~vvp_fun_muxz()
{
}

शून्य vvp_fun_muxz::recv_vec4(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
                             vvp_context_t)
{
      विकल्प (ptr.पत्तनम्()) {
	  पक्ष 0:
	    यदि (a_ .eeq(bit) && has_सततम्) प्रत्यागम;
	    a_ = bit;
	    यदि (select_ == SEL_PORT1) प्रत्यागम; // The other port is selected.
	    भङ्ग;
	  पक्ष 1:
	    यदि (b_ .eeq(bit) && has_सततम्) प्रत्यागम;
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
		  यदि (select_ == SEL_BOTH && has_सततम्) प्रत्यागम;
		  select_ = SEL_BOTH;
	    }
	    भङ्ग;
	  स्वभाव:
	    प्रत्यागम;
      }

      यदि (net_ == 0) {
	    net_ = ptr.ptr();
	    कालसारणी_functor(एतत्);
      }
}

शून्य vvp_fun_muxz::recv_vec4_pv(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit,
				अचिह्नित आधारः, अचिह्नित vwid, vvp_context_t)
{
      प्रतिज्ञा(आधारः + bit.size() <= vwid);
      तार्किक flag;

      विकल्प (ptr.पत्तनम्()) {
	  पक्ष 0:
	    flag = a_.समुच्चयः_vec(आधारः, bit);
	    यदि (flag == असत्य && has_सततम्) प्रत्यागम;
	    यदि (select_ == SEL_PORT1) प्रत्यागम; // The other port is selected.
	    भङ्ग;
	  पक्ष 1:
	    flag = b_.समुच्चयः_vec(आधारः, bit);
	    यदि (flag == असत्य && has_सततम्) प्रत्यागम;
	    यदि (select_ == SEL_PORT0) प्रत्यागम; // The other port is selected.
	    भङ्ग;
	  पक्ष 2:
	    प्रतिज्ञा((आधारः == 0) && (bit.size() == 1));
	    recv_vec4(ptr, bit, 0);
	  स्वभाव:
	    प्रत्यागम;
      }
      यदि (net_ == 0) {
	    net_ = ptr.ptr();
	    कालसारणी_functor(एतत्);
      }
}

शून्य vvp_fun_muxz::सततम्_सततम्()
{
      has_सततम् = सत्य;
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
		    अचिह्नित min_आकार = a_.size();
		    अचिह्नित max_आकार = min_आकार;
		    यदि (b_.size() < min_आकार)
			  min_आकार = b_.size();
		    यदि (b_.size() > max_आकार)
			  max_आकार = b_.size();

		    vvp_vector4_t res (max_आकार);

		    प्रति (अचिह्नित idx = 0 ;  idx < min_आकार ;  idx += 1) {
			  यदि (a_.value(idx) == b_.value(idx))
				res.समुच्चयः_bit(idx, a_.value(idx));
			  अन्यथा
				res.समुच्चयः_bit(idx, BIT4_X);
		    }

		    प्रति (अचिह्नित idx = min_आकार ;  idx < max_आकार ;  idx += 1)
			  res.समुच्चयः_bit(idx, BIT4_X);

		    ptr->send_vec4(res, 0);
	      }
	    भङ्ग;
      }
}

vvp_fun_not::vvp_fun_not(अचिह्नित wid)
: vvp_fun_buf_not_(wid)
{
      गणनम्_functors_logic += 1;
}

vvp_fun_not::~vvp_fun_not()
{
}

/*
 * The not functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the inverted result.
 */
शून्य vvp_fun_not::सततम्_सततम्()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t परिणाम (निवेशः_2, सत्य /* invert */);
      ptr->send_vec4(परिणाम, 0);
}

vvp_fun_or::vvp_fun_or(अचिह्नित wid, तार्किक invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      गणनम्_functors_logic += 1;
}

vvp_fun_or::~vvp_fun_or()
{
}

शून्य vvp_fun_or::सततम्_सततम्()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t परिणाम (निवेशः_2[0]);

      प्रति (अचिह्नित pdx = 1 ; pdx < 4 ; pdx += 1)
	    परिणाम |= निवेशः_2[pdx];

      यदि (invert_)
	    परिणाम.invert();

      ptr->send_vec4(परिणाम, 0);
}

vvp_fun_xor::vvp_fun_xor(अचिह्नित wid, तार्किक invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      गणनम्_functors_logic += 1;
}

vvp_fun_xor::~vvp_fun_xor()
{
}

शून्य vvp_fun_xor::सततम्_सततम्()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t परिणाम (निवेशः_2[0]);

      प्रति (अचिह्नित pdx = 1 ; pdx < 4 ; pdx += 1)
	    परिणाम ^= निवेशः_2[pdx];

      यदि (invert_)
	    परिणाम.invert();

      ptr->send_vec4(परिणाम, 0);
}

/*
 * The parser calls this function to create a logic functor. I allocate a
 * functor, and map the name to the vvp_ipoint_t address for the
 * functor. Also resolve the inputs to the functor.
 */

शून्य compile_functor(वर्ण*label, वर्ण*type, अचिह्नित width,
		     अचिह्नित ostr0, अचिह्नित ostr1,
		     अचिह्नित argc, संरचना symb_s*argv)
{
      vvp_net_fun_t* obj = 0;
      तार्किक strength_aware = असत्य;

      यदि (वाक्यं_तुलय(type, "OR") == 0) {
	    obj = नूतन vvp_fun_or(width, असत्य);

      } अन्यथा यदि (वाक्यं_तुलय(type, "AND") == 0) {
	    obj = नूतन vvp_fun_and(width, असत्य);

      } अन्यथा यदि (वाक्यं_तुलय(type, "BUF") == 0) {
	    obj = नूतन vvp_fun_buf(width);

      } अन्यथा यदि (वाक्यं_तुलय(type, "BUFIF0") == 0) {
	    obj = नूतन vvp_fun_bufif(सत्य,असत्य, ostr0, ostr1);
	    strength_aware = सत्य;

      } अन्यथा यदि (वाक्यं_तुलय(type, "BUFIF1") == 0) {
	    obj = नूतन vvp_fun_bufif(असत्य,असत्य, ostr0, ostr1);
	    strength_aware = सत्य;

      } अन्यथा यदि (वाक्यं_तुलय(type, "EQUIV") == 0) {
	    obj = नूतन vvp_fun_equiv();

      } अन्यथा यदि (वाक्यं_तुलय(type, "IMPL") == 0) {
	    obj = नूतन vvp_fun_impl();

      } अन्यथा यदि (वाक्यं_तुलय(type, "NAND") == 0) {
	    obj = नूतन vvp_fun_and(width, सत्य);

      } अन्यथा यदि (वाक्यं_तुलय(type, "NOR") == 0) {
	    obj = नूतन vvp_fun_or(width, सत्य);

      } अन्यथा यदि (वाक्यं_तुलय(type, "NOTIF0") == 0) {
	    obj = नूतन vvp_fun_bufif(सत्य,सत्य, ostr0, ostr1);
	    strength_aware = सत्य;

      } अन्यथा यदि (वाक्यं_तुलय(type, "NOTIF1") == 0) {
	    obj = नूतन vvp_fun_bufif(असत्य,सत्य, ostr0, ostr1);
	    strength_aware = सत्य;

      } अन्यथा यदि (वाक्यं_तुलय(type, "BUFT") == 0) {
	    obj = नूतन vvp_fun_buft();

      } अन्यथा यदि (वाक्यं_तुलय(type, "BUFZ") == 0) {
	    obj = नूतन vvp_fun_bufz();

      } अन्यथा यदि (वाक्यं_तुलय(type, "MUXR") == 0) {
	    obj = नूतन vvp_fun_muxr;

      } अन्यथा यदि (वाक्यं_तुलय(type, "MUXZ") == 0) {
	    obj = नूतन vvp_fun_muxz(width);

      } अन्यथा यदि (वाक्यं_तुलय(type, "CMOS") == 0) {
	    obj = नूतन vvp_fun_cmos();

      } अन्यथा यदि (वाक्यं_तुलय(type, "NMOS") == 0) {
	    obj = नूतन vvp_fun_pmos(सत्य);

      } अन्यथा यदि (वाक्यं_तुलय(type, "PMOS") == 0) {
	    obj = नूतन vvp_fun_pmos(असत्य);

      } अन्यथा यदि (वाक्यं_तुलय(type, "RCMOS") == 0) {
	    obj = नूतन vvp_fun_rcmos();

      } अन्यथा यदि (वाक्यं_तुलय(type, "RNMOS") == 0) {
	    obj = नूतन vvp_fun_rpmos(सत्य);

      } अन्यथा यदि (वाक्यं_तुलय(type, "RPMOS") == 0) {
	    obj = नूतन vvp_fun_rpmos(असत्य);

      } अन्यथा यदि (वाक्यं_तुलय(type, "NOT") == 0) {
	    obj = नूतन vvp_fun_not(width);

      } अन्यथा यदि (वाक्यं_तुलय(type, "XNOR") == 0) {
	    obj = नूतन vvp_fun_xor(width, सत्य);

      } अन्यथा यदि (वाक्यं_तुलय(type, "XOR") == 0) {
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
	    define_functor_चिह्नम्(label, net);
	    मुक्त(label);
	    प्रत्यागम;
      }

      vvp_net_t*net_drv = नूतन vvp_net_t;
      vvp_net_fun_t*obj_drv = नूतन vvp_fun_drive(ostr0, ostr1);
      net_drv->fun = obj_drv;

	/* Point the gate to the drive node. */
      net->link(vvp_net_ptr_t(net_drv, 0));

      define_functor_चिह्नम्(label, net_drv);
      मुक्त(label);
}
