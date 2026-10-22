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
      വേണ്ടി (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    നിവേശം_2[idx] = vvp_vector4_t(wid, BIT4_Z);
}

vvp_fun_boolean_::~vvp_fun_boolean_()
{
}

ശൂന്യം vvp_fun_boolean_::recv_vec4(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
                                 vvp_context_t)
{
      unsigned port = ptr.port();
      എങ്കിൽ (നിവേശം_2[port] .eeq( bit )) തിരികെ;

      നിവേശം_2[port] = bit;
      എങ്കിൽ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ഇത്);
      }
}

ശൂന്യം vvp_fun_boolean_::recv_vec4_pv(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
				    unsigned base, unsigned vwid, vvp_context_t)
{
      unsigned port = ptr.port();

      പ്രതിജ്ഞാ(base + bit.size() <= vwid);

	// Set the part for the input. If nothing changes, then break.
      തർക്കം flag = നിവേശം_2[port] .set_vec(base, bit);
      എങ്കിൽ (flag == അസത്യം)
	    തിരികെ;

      എങ്കിൽ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ഇത്);
      }
}

ശൂന്യം vvp_fun_boolean_::recv_real(vvp_net_ptr_t ptr, ഇരട്ട real,
                                 vvp_context_t ctx)
{
      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_and::vvp_fun_and(unsigned wid, തർക്കം invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      എണ്ണം_functors_logic += 1;
}

vvp_fun_and::~vvp_fun_and()
{
}

ശൂന്യം vvp_fun_and::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (നിവേശം_2[0]);

      വേണ്ടി (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result &= നിവേശം_2[pdx];

      എങ്കിൽ (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_equiv::vvp_fun_equiv()
: vvp_fun_boolean_(1)
{
      എണ്ണം_functors_logic += 1;
}

vvp_fun_equiv::~vvp_fun_equiv()
{
}

ശൂന്യം vvp_fun_equiv::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      പ്രതിജ്ഞാ(നിവേശം_2[0].size() == 1);
      പ്രതിജ്ഞാ(നിവേശം_2[1].size() == 1);

      vvp_bit4_t bit = ~(നിവേശം_2[0].value(0) ^ നിവേശം_2[1].value(0));
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_impl::vvp_fun_impl()
: vvp_fun_boolean_(1)
{
      എണ്ണം_functors_logic += 1;
}

vvp_fun_impl::~vvp_fun_impl()
{
}

ശൂന്യം vvp_fun_impl::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      പ്രതിജ്ഞാ(നിവേശം_2[0].size() == 1);
      പ്രതിജ്ഞാ(നിവേശം_2[1].size() == 1);

      vvp_bit4_t bit = ~നിവേശം_2[0].value(0) | നിവേശം_2[1].value(0);
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_buf_not_::vvp_fun_buf_not_(unsigned wid)
: നിവേശം_2(wid, BIT4_Z)
{
      net_ = 0;
}

vvp_fun_buf_not_::~vvp_fun_buf_not_()
{
}

ശൂന്യം vvp_fun_buf_not_::recv_vec4(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
                                 vvp_context_t)
{
      എങ്കിൽ (ptr.port() != 0) തിരികെ;

      എങ്കിൽ (നിവേശം_2 .eeq( bit )) തിരികെ;

      നിവേശം_2 = bit;
      എങ്കിൽ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ഇത്);
      }
}

ശൂന്യം vvp_fun_buf_not_::recv_vec4_pv(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
                                    unsigned base, unsigned vwid, vvp_context_t)
{
      എങ്കിൽ (ptr.port() != 0) തിരികെ;

      പ്രതിജ്ഞാ(base + bit.size() <= vwid);

	// Set the input part. If nothing changes, then break.
      തർക്കം flag = നിവേശം_2.set_vec(base, bit);
      എങ്കിൽ (flag == അസത്യം) തിരികെ;

      എങ്കിൽ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ഇത്);
      }
}

ശൂന്യം vvp_fun_buf_not_::recv_real(vvp_net_ptr_t ptr, ഇരട്ട real,
                                 vvp_context_t ctx)
{
      എങ്കിൽ (ptr.port() != 0) തിരികെ;

      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_buf::vvp_fun_buf(unsigned wid)
: vvp_fun_buf_not_(wid)
{
      എണ്ണം_functors_logic += 1;
}

vvp_fun_buf::~vvp_fun_buf()
{
}

/*
 * The buf functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the result.
 */
ശൂന്യം vvp_fun_buf::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t tmp (നിവേശം_2);
      tmp.change_z2x();
      ptr->send_vec4(tmp, 0);
}

vvp_fun_bufz::vvp_fun_bufz()
{
      എണ്ണം_functors_logic += 1;
}

vvp_fun_bufz::~vvp_fun_bufz()
{
}

/*
 * The bufz is similar to the buf device, except that it does not
 * bother translating z bits to x.
 */
ശൂന്യം vvp_fun_bufz::recv_vec4(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
                             vvp_context_t)
{
      എങ്കിൽ (ptr.port() != 0)
	    തിരികെ;

      ptr.ptr()->send_vec4(bit, 0);
}

ശൂന്യം vvp_fun_bufz::recv_vec4_pv(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
                                unsigned base, unsigned vwid, vvp_context_t)
{
      എങ്കിൽ (ptr.port() != 0)
	    തിരികെ;

      ptr.ptr()->send_vec4_pv(bit, base, vwid, 0);
}

ശൂന്യം vvp_fun_bufz::recv_real(vvp_net_ptr_t ptr, ഇരട്ട bit,
                             vvp_context_t)
{
      എങ്കിൽ (ptr.port() != 0)
	    തിരികെ;

      ptr.ptr()->send_real(bit, 0);
}

ശൂന്യം vvp_fun_buft::recv_vec8(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector8_t&bit)
{
      എങ്കിൽ (ptr.port() != 0)
	    തിരികെ;

      ptr.ptr()->send_vec8(bit);
}

vvp_fun_muxr::vvp_fun_muxr()
: a_(0.0), b_(0.0)
{
      net_ = 0;
      എണ്ണം_functors_logic += 1;
      select_ = SEL_BOTH;
}

vvp_fun_muxr::~vvp_fun_muxr()
{
}

ശൂന്യം vvp_fun_muxr::recv_vec4(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
                             vvp_context_t)
{
	/* The real valued mux can only take in the select as a
	   vector4_t. The muxed data is real. */
      എങ്കിൽ (ptr.port() != 2)
	    തിരികെ;

      പ്രതിജ്ഞാ(bit.size() == 1);

      തിരഞ്ഞെടുപ്പ് (bit.value(0)) {
	  അവസ്ഥ BIT4_0:
	    എങ്കിൽ (select_ == SEL_PORT0) തിരികെ;
	    select_ = SEL_PORT0;
	    നിർത്തുക;
	  അവസ്ഥ BIT4_1:
	    എങ്കിൽ (select_ == SEL_PORT1) തിരികെ;
	    select_ = SEL_PORT1;
	    നിർത്തുക;
	  സ്ഥിരസ്ഥിതി:
	    എങ്കിൽ (select_ == SEL_BOTH) തിരികെ;
	    select_ = SEL_BOTH;
      }

      എങ്കിൽ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ഇത്);
      }
}

ശൂന്യം vvp_fun_muxr::recv_real(vvp_net_ptr_t ptr, ഇരട്ട bit,
                             vvp_context_t)
{
      തിരഞ്ഞെടുപ്പ് (ptr.port()) {
	  അവസ്ഥ 0:
	    എങ്കിൽ (a_ == bit) തിരികെ;
	    a_ = bit;
	    എങ്കിൽ (select_ == SEL_PORT1) തിരികെ; // The other port is selected.
	    നിർത്തുക;

	  അവസ്ഥ 1:
	    എങ്കിൽ (b_ == bit) തിരികെ;
	    b_ = bit;
	    എങ്കിൽ (select_ == SEL_PORT0) തിരികെ; // The other port is selected.
	    നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    പ്രതിജ്ഞാ(0);
      }

      എങ്കിൽ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ഇത്);
      }
}

ശൂന്യം vvp_fun_muxr::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      തിരഞ്ഞെടുപ്പ് (select_) {
	  അവസ്ഥ SEL_PORT0:
	    ptr->send_real(a_, 0);
	    നിർത്തുക;
	  അവസ്ഥ SEL_PORT1:
	    ptr->send_real(b_, 0);
	    നിർത്തുക;
	  സ്ഥിരസ്ഥിതി:
	    എങ്കിൽ (a_ == b_) {
		  ptr->send_real(a_, 0);
	    } അല്ലെങ്കിൽ {
		  ptr->send_real(0.0, 0); // Should this be NaN?
	    }
	    നിർത്തുക;
      }
}

vvp_fun_muxz::vvp_fun_muxz(unsigned wid)
: a_(wid, BIT4_Z), b_(wid, BIT4_Z)
{
      net_ = 0;
      എണ്ണം_functors_logic += 1;
      select_ = SEL_BOTH;
      has_run_ = അസത്യം;
}

vvp_fun_muxz::~vvp_fun_muxz()
{
}

ശൂന്യം vvp_fun_muxz::recv_vec4(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
                             vvp_context_t)
{
      തിരഞ്ഞെടുപ്പ് (ptr.port()) {
	  അവസ്ഥ 0:
	    എങ്കിൽ (a_ .eeq(bit) && has_run_) തിരികെ;
	    a_ = bit;
	    എങ്കിൽ (select_ == SEL_PORT1) തിരികെ; // The other port is selected.
	    നിർത്തുക;
	  അവസ്ഥ 1:
	    എങ്കിൽ (b_ .eeq(bit) && has_run_) തിരികെ;
	    b_ = bit;
	    എങ്കിൽ (select_ == SEL_PORT0) തിരികെ; // The other port is selected.
	    നിർത്തുക;
	  അവസ്ഥ 2:
	    പ്രതിജ്ഞാ(bit.size() == 1);
	    തിരഞ്ഞെടുപ്പ് (bit.value(0)) {
		അവസ്ഥ BIT4_0:
		  എങ്കിൽ (select_ == SEL_PORT0) തിരികെ;
		  select_ = SEL_PORT0;
		  നിർത്തുക;
		അവസ്ഥ BIT4_1:
		  എങ്കിൽ (select_ == SEL_PORT1) തിരികെ;
		  select_ = SEL_PORT1;
		  നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		  എങ്കിൽ (select_ == SEL_BOTH && has_run_) തിരികെ;
		  select_ = SEL_BOTH;
	    }
	    നിർത്തുക;
	  സ്ഥിരസ്ഥിതി:
	    തിരികെ;
      }

      എങ്കിൽ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ഇത്);
      }
}

ശൂന്യം vvp_fun_muxz::recv_vec4_pv(vvp_net_ptr_t ptr, സ്ഥിരം vvp_vector4_t&bit,
				unsigned base, unsigned vwid, vvp_context_t)
{
      പ്രതിജ്ഞാ(base + bit.size() <= vwid);
      തർക്കം flag;

      തിരഞ്ഞെടുപ്പ് (ptr.port()) {
	  അവസ്ഥ 0:
	    flag = a_.set_vec(base, bit);
	    എങ്കിൽ (flag == അസത്യം && has_run_) തിരികെ;
	    എങ്കിൽ (select_ == SEL_PORT1) തിരികെ; // The other port is selected.
	    നിർത്തുക;
	  അവസ്ഥ 1:
	    flag = b_.set_vec(base, bit);
	    എങ്കിൽ (flag == അസത്യം && has_run_) തിരികെ;
	    എങ്കിൽ (select_ == SEL_PORT0) തിരികെ; // The other port is selected.
	    നിർത്തുക;
	  അവസ്ഥ 2:
	    പ്രതിജ്ഞാ((base == 0) && (bit.size() == 1));
	    recv_vec4(ptr, bit, 0);
	  സ്ഥിരസ്ഥിതി:
	    തിരികെ;
      }
      എങ്കിൽ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(ഇത്);
      }
}

ശൂന്യം vvp_fun_muxz::run_run()
{
      has_run_ = സത്യം;
      vvp_net_t*ptr = net_;
      net_ = 0;

      തിരഞ്ഞെടുപ്പ് (select_) {
	  അവസ്ഥ SEL_PORT0:
	    ptr->send_vec4(a_, 0);
	    നിർത്തുക;
	  അവസ്ഥ SEL_PORT1:
	    ptr->send_vec4(b_, 0);
	    നിർത്തുക;
	  സ്ഥിരസ്ഥിതി:
	      {
		    unsigned min_size = a_.size();
		    unsigned max_size = min_size;
		    എങ്കിൽ (b_.size() < min_size)
			  min_size = b_.size();
		    എങ്കിൽ (b_.size() > max_size)
			  max_size = b_.size();

		    vvp_vector4_t res (max_size);

		    വേണ്ടി (unsigned idx = 0 ;  idx < min_size ;  idx += 1) {
			  എങ്കിൽ (a_.value(idx) == b_.value(idx))
				res.set_bit(idx, a_.value(idx));
			  അല്ലെങ്കിൽ
				res.set_bit(idx, BIT4_X);
		    }

		    വേണ്ടി (unsigned idx = min_size ;  idx < max_size ;  idx += 1)
			  res.set_bit(idx, BIT4_X);

		    ptr->send_vec4(res, 0);
	      }
	    നിർത്തുക;
      }
}

vvp_fun_not::vvp_fun_not(unsigned wid)
: vvp_fun_buf_not_(wid)
{
      എണ്ണം_functors_logic += 1;
}

vvp_fun_not::~vvp_fun_not()
{
}

/*
 * The not functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the inverted result.
 */
ശൂന്യം vvp_fun_not::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (നിവേശം_2, സത്യം /* invert */);
      ptr->send_vec4(result, 0);
}

vvp_fun_or::vvp_fun_or(unsigned wid, തർക്കം invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      എണ്ണം_functors_logic += 1;
}

vvp_fun_or::~vvp_fun_or()
{
}

ശൂന്യം vvp_fun_or::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (നിവേശം_2[0]);

      വേണ്ടി (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result |= നിവേശം_2[pdx];

      എങ്കിൽ (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_xor::vvp_fun_xor(unsigned wid, തർക്കം invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      എണ്ണം_functors_logic += 1;
}

vvp_fun_xor::~vvp_fun_xor()
{
}

ശൂന്യം vvp_fun_xor::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (നിവേശം_2[0]);

      വേണ്ടി (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result ^= നിവേശം_2[pdx];

      എങ്കിൽ (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

/*
 * The parser calls this function to create a logic functor. I allocate a
 * functor, and map the name to the vvp_ipoint_t address for the
 * functor. Also resolve the inputs to the functor.
 */

ശൂന്യം compile_functor(അക്ഷരം*label, അക്ഷരം*type, unsigned width,
		     unsigned ostr0, unsigned ostr1,
		     unsigned argc, ഘടന symb_s*argv)
{
      vvp_net_fun_t* obj = 0;
      തർക്കം strength_aware = അസത്യം;

      എങ്കിൽ (strcmp(type, "OR") == 0) {
	    obj = പുതിയ vvp_fun_or(width, അസത്യം);

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "AND") == 0) {
	    obj = പുതിയ vvp_fun_and(width, അസത്യം);

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "BUF") == 0) {
	    obj = പുതിയ vvp_fun_buf(width);

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "BUFIF0") == 0) {
	    obj = പുതിയ vvp_fun_bufif(സത്യം,അസത്യം, ostr0, ostr1);
	    strength_aware = സത്യം;

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "BUFIF1") == 0) {
	    obj = പുതിയ vvp_fun_bufif(അസത്യം,അസത്യം, ostr0, ostr1);
	    strength_aware = സത്യം;

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "EQUIV") == 0) {
	    obj = പുതിയ vvp_fun_equiv();

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "IMPL") == 0) {
	    obj = പുതിയ vvp_fun_impl();

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "NAND") == 0) {
	    obj = പുതിയ vvp_fun_and(width, സത്യം);

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "NOR") == 0) {
	    obj = പുതിയ vvp_fun_or(width, സത്യം);

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "NOTIF0") == 0) {
	    obj = പുതിയ vvp_fun_bufif(സത്യം,സത്യം, ostr0, ostr1);
	    strength_aware = സത്യം;

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "NOTIF1") == 0) {
	    obj = പുതിയ vvp_fun_bufif(അസത്യം,സത്യം, ostr0, ostr1);
	    strength_aware = സത്യം;

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "BUFT") == 0) {
	    obj = പുതിയ vvp_fun_buft();

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "BUFZ") == 0) {
	    obj = പുതിയ vvp_fun_bufz();

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "MUXR") == 0) {
	    obj = പുതിയ vvp_fun_muxr;

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "MUXZ") == 0) {
	    obj = പുതിയ vvp_fun_muxz(width);

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "CMOS") == 0) {
	    obj = പുതിയ vvp_fun_cmos();

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "NMOS") == 0) {
	    obj = പുതിയ vvp_fun_pmos(സത്യം);

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "PMOS") == 0) {
	    obj = പുതിയ vvp_fun_pmos(അസത്യം);

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "RCMOS") == 0) {
	    obj = പുതിയ vvp_fun_rcmos();

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "RNMOS") == 0) {
	    obj = പുതിയ vvp_fun_rpmos(സത്യം);

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "RPMOS") == 0) {
	    obj = പുതിയ vvp_fun_rpmos(അസത്യം);

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "NOT") == 0) {
	    obj = പുതിയ vvp_fun_not(width);

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "XNOR") == 0) {
	    obj = പുതിയ vvp_fun_xor(width, സത്യം);

      } അല്ലെങ്കിൽ എങ്കിൽ (strcmp(type, "XOR") == 0) {
	    obj = പുതിയ vvp_fun_xor(width, അസത്യം);

      } അല്ലെങ്കിൽ {
	    yyerror("invalid functor type.");
	    സ്വതന്ത്രം(type);
	    സ്വതന്ത്രം(argv);
	    സ്വതന്ത്രം(label);
	    തിരികെ;
      }

      സ്വതന്ത്രം(type);

      പ്രതിജ്ഞാ(argc <= 4);
      vvp_net_t*net = പുതിയ vvp_net_t;
      net->fun = obj;

      inputs_connect(net, argc, argv);
      സ്വതന്ത്രം(argv);

	/* If both the strengths are the default strong drive, then
	   there is no need for a specialized driver. Attach the label
	   to this node and we are finished. */
      എങ്കിൽ (strength_aware || (ostr0 == 6 && ostr1 == 6)) {
	    define_functor_ചിഹ്നം(label, net);
	    സ്വതന്ത്രം(label);
	    തിരികെ;
      }

      vvp_net_t*net_drv = പുതിയ vvp_net_t;
      vvp_net_fun_t*obj_drv = പുതിയ vvp_fun_drive(ostr0, ostr1);
      net_drv->fun = obj_drv;

	/* Point the gate to the drive node. */
      net->link(vvp_net_ptr_t(net_drv, 0));

      define_functor_ചിഹ്നം(label, net_drv);
      സ്വതന്ത്രം(label);
}
