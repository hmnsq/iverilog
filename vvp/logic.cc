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
      க்கு (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    உள்ளீடு_2[idx] = vvp_vector4_t(wid, BIT4_Z);
}

vvp_fun_boolean_::~vvp_fun_boolean_()
{
}

வெறுமை vvp_fun_boolean_::recv_vec4(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit,
                                 vvp_context_t)
{
      unsigned port = ptr.port();
      எனில் (உள்ளீடு_2[port] .eeq( bit )) திரும்பு;

      உள்ளீடு_2[port] = bit;
      எனில் (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(இது);
      }
}

வெறுமை vvp_fun_boolean_::recv_vec4_pv(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit,
				    unsigned base, unsigned vwid, vvp_context_t)
{
      unsigned port = ptr.port();

      ப்ரதிஜ்ஞா(base + bit.size() <= vwid);

	// Set the part for the input. If nothing changes, then break.
      தருக்க flag = உள்ளீடு_2[port] .set_vec(base, bit);
      எனில் (flag == பொய்)
	    திரும்பு;

      எனில் (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(இது);
      }
}

வெறுமை vvp_fun_boolean_::recv_real(vvp_net_ptr_t ptr, இரட்டை real,
                                 vvp_context_t ctx)
{
      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_and::vvp_fun_and(unsigned wid, தருக்க invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      மொத்தம்_functors_logic += 1;
}

vvp_fun_and::~vvp_fun_and()
{
}

வெறுமை vvp_fun_and::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (உள்ளீடு_2[0]);

      க்கு (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result &= உள்ளீடு_2[pdx];

      எனில் (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_equiv::vvp_fun_equiv()
: vvp_fun_boolean_(1)
{
      மொத்தம்_functors_logic += 1;
}

vvp_fun_equiv::~vvp_fun_equiv()
{
}

வெறுமை vvp_fun_equiv::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      ப்ரதிஜ்ஞா(உள்ளீடு_2[0].size() == 1);
      ப்ரதிஜ்ஞா(உள்ளீடு_2[1].size() == 1);

      vvp_bit4_t bit = ~(உள்ளீடு_2[0].value(0) ^ உள்ளீடு_2[1].value(0));
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_impl::vvp_fun_impl()
: vvp_fun_boolean_(1)
{
      மொத்தம்_functors_logic += 1;
}

vvp_fun_impl::~vvp_fun_impl()
{
}

வெறுமை vvp_fun_impl::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      ப்ரதிஜ்ஞா(உள்ளீடு_2[0].size() == 1);
      ப்ரதிஜ்ஞா(உள்ளீடு_2[1].size() == 1);

      vvp_bit4_t bit = ~உள்ளீடு_2[0].value(0) | உள்ளீடு_2[1].value(0);
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_buf_not_::vvp_fun_buf_not_(unsigned wid)
: உள்ளீடு_2(wid, BIT4_Z)
{
      net_ = 0;
}

vvp_fun_buf_not_::~vvp_fun_buf_not_()
{
}

வெறுமை vvp_fun_buf_not_::recv_vec4(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit,
                                 vvp_context_t)
{
      எனில் (ptr.port() != 0) திரும்பு;

      எனில் (உள்ளீடு_2 .eeq( bit )) திரும்பு;

      உள்ளீடு_2 = bit;
      எனில் (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(இது);
      }
}

வெறுமை vvp_fun_buf_not_::recv_vec4_pv(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit,
                                    unsigned base, unsigned vwid, vvp_context_t)
{
      எனில் (ptr.port() != 0) திரும்பு;

      ப்ரதிஜ்ஞா(base + bit.size() <= vwid);

	// Set the input part. If nothing changes, then break.
      தருக்க flag = உள்ளீடு_2.set_vec(base, bit);
      எனில் (flag == பொய்) திரும்பு;

      எனில் (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(இது);
      }
}

வெறுமை vvp_fun_buf_not_::recv_real(vvp_net_ptr_t ptr, இரட்டை real,
                                 vvp_context_t ctx)
{
      எனில் (ptr.port() != 0) திரும்பு;

      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_buf::vvp_fun_buf(unsigned wid)
: vvp_fun_buf_not_(wid)
{
      மொத்தம்_functors_logic += 1;
}

vvp_fun_buf::~vvp_fun_buf()
{
}

/*
 * The buf functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the result.
 */
வெறுமை vvp_fun_buf::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t tmp (உள்ளீடு_2);
      tmp.change_z2x();
      ptr->send_vec4(tmp, 0);
}

vvp_fun_bufz::vvp_fun_bufz()
{
      மொத்தம்_functors_logic += 1;
}

vvp_fun_bufz::~vvp_fun_bufz()
{
}

/*
 * The bufz is similar to the buf device, except that it does not
 * bother translating z bits to x.
 */
வெறுமை vvp_fun_bufz::recv_vec4(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit,
                             vvp_context_t)
{
      எனில் (ptr.port() != 0)
	    திரும்பு;

      ptr.ptr()->send_vec4(bit, 0);
}

வெறுமை vvp_fun_bufz::recv_vec4_pv(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit,
                                unsigned base, unsigned vwid, vvp_context_t)
{
      எனில் (ptr.port() != 0)
	    திரும்பு;

      ptr.ptr()->send_vec4_pv(bit, base, vwid, 0);
}

வெறுமை vvp_fun_bufz::recv_real(vvp_net_ptr_t ptr, இரட்டை bit,
                             vvp_context_t)
{
      எனில் (ptr.port() != 0)
	    திரும்பு;

      ptr.ptr()->send_real(bit, 0);
}

வெறுமை vvp_fun_buft::recv_vec8(vvp_net_ptr_t ptr, மாறிலி vvp_vector8_t&bit)
{
      எனில் (ptr.port() != 0)
	    திரும்பு;

      ptr.ptr()->send_vec8(bit);
}

vvp_fun_muxr::vvp_fun_muxr()
: a_(0.0), b_(0.0)
{
      net_ = 0;
      மொத்தம்_functors_logic += 1;
      select_ = SEL_BOTH;
}

vvp_fun_muxr::~vvp_fun_muxr()
{
}

வெறுமை vvp_fun_muxr::recv_vec4(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit,
                             vvp_context_t)
{
	/* The real valued mux can only take in the select as a
	   vector4_t. The muxed data is real. */
      எனில் (ptr.port() != 2)
	    திரும்பு;

      ப்ரதிஜ்ஞா(bit.size() == 1);

      தேர்வு (bit.value(0)) {
	  நிலை BIT4_0:
	    எனில் (select_ == SEL_PORT0) திரும்பு;
	    select_ = SEL_PORT0;
	    நிறுத்து;
	  நிலை BIT4_1:
	    எனில் (select_ == SEL_PORT1) திரும்பு;
	    select_ = SEL_PORT1;
	    நிறுத்து;
	  இயல்பு:
	    எனில் (select_ == SEL_BOTH) திரும்பு;
	    select_ = SEL_BOTH;
      }

      எனில் (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(இது);
      }
}

வெறுமை vvp_fun_muxr::recv_real(vvp_net_ptr_t ptr, இரட்டை bit,
                             vvp_context_t)
{
      தேர்வு (ptr.port()) {
	  நிலை 0:
	    எனில் (a_ == bit) திரும்பு;
	    a_ = bit;
	    எனில் (select_ == SEL_PORT1) திரும்பு; // The other port is selected.
	    நிறுத்து;

	  நிலை 1:
	    எனில் (b_ == bit) திரும்பு;
	    b_ = bit;
	    எனில் (select_ == SEL_PORT0) திரும்பு; // The other port is selected.
	    நிறுத்து;

	  இயல்பு:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    ப்ரதிஜ்ஞா(0);
      }

      எனில் (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(இது);
      }
}

வெறுமை vvp_fun_muxr::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      தேர்வு (select_) {
	  நிலை SEL_PORT0:
	    ptr->send_real(a_, 0);
	    நிறுத்து;
	  நிலை SEL_PORT1:
	    ptr->send_real(b_, 0);
	    நிறுத்து;
	  இயல்பு:
	    எனில் (a_ == b_) {
		  ptr->send_real(a_, 0);
	    } இல்லையெனில் {
		  ptr->send_real(0.0, 0); // Should this be NaN?
	    }
	    நிறுத்து;
      }
}

vvp_fun_muxz::vvp_fun_muxz(unsigned wid)
: a_(wid, BIT4_Z), b_(wid, BIT4_Z)
{
      net_ = 0;
      மொத்தம்_functors_logic += 1;
      select_ = SEL_BOTH;
      has_run_ = பொய்;
}

vvp_fun_muxz::~vvp_fun_muxz()
{
}

வெறுமை vvp_fun_muxz::recv_vec4(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit,
                             vvp_context_t)
{
      தேர்வு (ptr.port()) {
	  நிலை 0:
	    எனில் (a_ .eeq(bit) && has_run_) திரும்பு;
	    a_ = bit;
	    எனில் (select_ == SEL_PORT1) திரும்பு; // The other port is selected.
	    நிறுத்து;
	  நிலை 1:
	    எனில் (b_ .eeq(bit) && has_run_) திரும்பு;
	    b_ = bit;
	    எனில் (select_ == SEL_PORT0) திரும்பு; // The other port is selected.
	    நிறுத்து;
	  நிலை 2:
	    ப்ரதிஜ்ஞா(bit.size() == 1);
	    தேர்வு (bit.value(0)) {
		நிலை BIT4_0:
		  எனில் (select_ == SEL_PORT0) திரும்பு;
		  select_ = SEL_PORT0;
		  நிறுத்து;
		நிலை BIT4_1:
		  எனில் (select_ == SEL_PORT1) திரும்பு;
		  select_ = SEL_PORT1;
		  நிறுத்து;
		இயல்பு:
		  எனில் (select_ == SEL_BOTH && has_run_) திரும்பு;
		  select_ = SEL_BOTH;
	    }
	    நிறுத்து;
	  இயல்பு:
	    திரும்பு;
      }

      எனில் (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(இது);
      }
}

வெறுமை vvp_fun_muxz::recv_vec4_pv(vvp_net_ptr_t ptr, மாறிலி vvp_vector4_t&bit,
				unsigned base, unsigned vwid, vvp_context_t)
{
      ப்ரதிஜ்ஞா(base + bit.size() <= vwid);
      தருக்க flag;

      தேர்வு (ptr.port()) {
	  நிலை 0:
	    flag = a_.set_vec(base, bit);
	    எனில் (flag == பொய் && has_run_) திரும்பு;
	    எனில் (select_ == SEL_PORT1) திரும்பு; // The other port is selected.
	    நிறுத்து;
	  நிலை 1:
	    flag = b_.set_vec(base, bit);
	    எனில் (flag == பொய் && has_run_) திரும்பு;
	    எனில் (select_ == SEL_PORT0) திரும்பு; // The other port is selected.
	    நிறுத்து;
	  நிலை 2:
	    ப்ரதிஜ்ஞா((base == 0) && (bit.size() == 1));
	    recv_vec4(ptr, bit, 0);
	  இயல்பு:
	    திரும்பு;
      }
      எனில் (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(இது);
      }
}

வெறுமை vvp_fun_muxz::run_run()
{
      has_run_ = உண்மை;
      vvp_net_t*ptr = net_;
      net_ = 0;

      தேர்வு (select_) {
	  நிலை SEL_PORT0:
	    ptr->send_vec4(a_, 0);
	    நிறுத்து;
	  நிலை SEL_PORT1:
	    ptr->send_vec4(b_, 0);
	    நிறுத்து;
	  இயல்பு:
	      {
		    unsigned min_size = a_.size();
		    unsigned max_size = min_size;
		    எனில் (b_.size() < min_size)
			  min_size = b_.size();
		    எனில் (b_.size() > max_size)
			  max_size = b_.size();

		    vvp_vector4_t res (max_size);

		    க்கு (unsigned idx = 0 ;  idx < min_size ;  idx += 1) {
			  எனில் (a_.value(idx) == b_.value(idx))
				res.set_bit(idx, a_.value(idx));
			  இல்லையெனில்
				res.set_bit(idx, BIT4_X);
		    }

		    க்கு (unsigned idx = min_size ;  idx < max_size ;  idx += 1)
			  res.set_bit(idx, BIT4_X);

		    ptr->send_vec4(res, 0);
	      }
	    நிறுத்து;
      }
}

vvp_fun_not::vvp_fun_not(unsigned wid)
: vvp_fun_buf_not_(wid)
{
      மொத்தம்_functors_logic += 1;
}

vvp_fun_not::~vvp_fun_not()
{
}

/*
 * The not functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the inverted result.
 */
வெறுமை vvp_fun_not::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (உள்ளீடு_2, உண்மை /* invert */);
      ptr->send_vec4(result, 0);
}

vvp_fun_or::vvp_fun_or(unsigned wid, தருக்க invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      மொத்தம்_functors_logic += 1;
}

vvp_fun_or::~vvp_fun_or()
{
}

வெறுமை vvp_fun_or::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (உள்ளீடு_2[0]);

      க்கு (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result |= உள்ளீடு_2[pdx];

      எனில் (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_xor::vvp_fun_xor(unsigned wid, தருக்க invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      மொத்தம்_functors_logic += 1;
}

vvp_fun_xor::~vvp_fun_xor()
{
}

வெறுமை vvp_fun_xor::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (உள்ளீடு_2[0]);

      க்கு (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result ^= உள்ளீடு_2[pdx];

      எனில் (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

/*
 * The parser calls this function to create a logic functor. I allocate a
 * functor, and map the name to the vvp_ipoint_t address for the
 * functor. Also resolve the inputs to the functor.
 */

வெறுமை compile_functor(எழுத்து*label, எழுத்து*type, unsigned width,
		     unsigned ostr0, unsigned ostr1,
		     unsigned argc, கட்டமைப்பு symb_s*argv)
{
      vvp_net_fun_t* obj = 0;
      தருக்க strength_aware = பொய்;

      எனில் (strcmp(type, "OR") == 0) {
	    obj = புதிய vvp_fun_or(width, பொய்);

      } இல்லையெனில் எனில் (strcmp(type, "AND") == 0) {
	    obj = புதிய vvp_fun_and(width, பொய்);

      } இல்லையெனில் எனில் (strcmp(type, "BUF") == 0) {
	    obj = புதிய vvp_fun_buf(width);

      } இல்லையெனில் எனில் (strcmp(type, "BUFIF0") == 0) {
	    obj = புதிய vvp_fun_bufif(உண்மை,பொய், ostr0, ostr1);
	    strength_aware = உண்மை;

      } இல்லையெனில் எனில் (strcmp(type, "BUFIF1") == 0) {
	    obj = புதிய vvp_fun_bufif(பொய்,பொய், ostr0, ostr1);
	    strength_aware = உண்மை;

      } இல்லையெனில் எனில் (strcmp(type, "EQUIV") == 0) {
	    obj = புதிய vvp_fun_equiv();

      } இல்லையெனில் எனில் (strcmp(type, "IMPL") == 0) {
	    obj = புதிய vvp_fun_impl();

      } இல்லையெனில் எனில் (strcmp(type, "NAND") == 0) {
	    obj = புதிய vvp_fun_and(width, உண்மை);

      } இல்லையெனில் எனில் (strcmp(type, "NOR") == 0) {
	    obj = புதிய vvp_fun_or(width, உண்மை);

      } இல்லையெனில் எனில் (strcmp(type, "NOTIF0") == 0) {
	    obj = புதிய vvp_fun_bufif(உண்மை,உண்மை, ostr0, ostr1);
	    strength_aware = உண்மை;

      } இல்லையெனில் எனில் (strcmp(type, "NOTIF1") == 0) {
	    obj = புதிய vvp_fun_bufif(பொய்,உண்மை, ostr0, ostr1);
	    strength_aware = உண்மை;

      } இல்லையெனில் எனில் (strcmp(type, "BUFT") == 0) {
	    obj = புதிய vvp_fun_buft();

      } இல்லையெனில் எனில் (strcmp(type, "BUFZ") == 0) {
	    obj = புதிய vvp_fun_bufz();

      } இல்லையெனில் எனில் (strcmp(type, "MUXR") == 0) {
	    obj = புதிய vvp_fun_muxr;

      } இல்லையெனில் எனில் (strcmp(type, "MUXZ") == 0) {
	    obj = புதிய vvp_fun_muxz(width);

      } இல்லையெனில் எனில் (strcmp(type, "CMOS") == 0) {
	    obj = புதிய vvp_fun_cmos();

      } இல்லையெனில் எனில் (strcmp(type, "NMOS") == 0) {
	    obj = புதிய vvp_fun_pmos(உண்மை);

      } இல்லையெனில் எனில் (strcmp(type, "PMOS") == 0) {
	    obj = புதிய vvp_fun_pmos(பொய்);

      } இல்லையெனில் எனில் (strcmp(type, "RCMOS") == 0) {
	    obj = புதிய vvp_fun_rcmos();

      } இல்லையெனில் எனில் (strcmp(type, "RNMOS") == 0) {
	    obj = புதிய vvp_fun_rpmos(உண்மை);

      } இல்லையெனில் எனில் (strcmp(type, "RPMOS") == 0) {
	    obj = புதிய vvp_fun_rpmos(பொய்);

      } இல்லையெனில் எனில் (strcmp(type, "NOT") == 0) {
	    obj = புதிய vvp_fun_not(width);

      } இல்லையெனில் எனில் (strcmp(type, "XNOR") == 0) {
	    obj = புதிய vvp_fun_xor(width, உண்மை);

      } இல்லையெனில் எனில் (strcmp(type, "XOR") == 0) {
	    obj = புதிய vvp_fun_xor(width, பொய்);

      } இல்லையெனில் {
	    yyerror("invalid functor type.");
	    விடுவி(type);
	    விடுவி(argv);
	    விடுவி(label);
	    திரும்பு;
      }

      விடுவி(type);

      ப்ரதிஜ்ஞா(argc <= 4);
      vvp_net_t*net = புதிய vvp_net_t;
      net->fun = obj;

      inputs_connect(net, argc, argv);
      விடுவி(argv);

	/* If both the strengths are the default strong drive, then
	   there is no need for a specialized driver. Attach the label
	   to this node and we are finished. */
      எனில் (strength_aware || (ostr0 == 6 && ostr1 == 6)) {
	    define_functor_குறியீடு(label, net);
	    விடுவி(label);
	    திரும்பு;
      }

      vvp_net_t*net_drv = புதிய vvp_net_t;
      vvp_net_fun_t*obj_drv = புதிய vvp_fun_drive(ostr0, ostr1);
      net_drv->fun = obj_drv;

	/* Point the gate to the drive node. */
      net->link(vvp_net_ptr_t(net_drv, 0));

      define_functor_குறியீடு(label, net_drv);
      விடுவி(label);
}
