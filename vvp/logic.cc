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

vvp_fun_boolean_::vvp_fun_boolean_(గుర్తులేని wid)
{
      net_ = 0;
      కొరకు (గుర్తులేని idx = 0 ;  idx < 4 ;  idx += 1)
	    ఆదానం_2[idx] = vvp_vector4_t(wid, BIT4_Z);
}

vvp_fun_boolean_::~vvp_fun_boolean_()
{
}

శూన్య vvp_fun_boolean_::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                 vvp_context_t)
{
      గుర్తులేని పోర్టు = ptr.పోర్టు();
      ఒకవేళ (ఆదానం_2[పోర్టు] .eeq( బిట్టు )) తిరిగి;

      ఆదానం_2[పోర్టు] = బిట్టు;
      ఒకవేళ (net_ == 0) {
	    net_ = ptr.ptr();
	    కాలపట్టిక_functor(ఇది);
      }
}

శూన్య vvp_fun_boolean_::recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
				    గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t)
{
      గుర్తులేని పోర్టు = ptr.పోర్టు();

      ప్రతిజ్ఞా(పునాది + బిట్టు.size() <= vwid);

	// Set the part for the input. If nothing changes, then break.
      తార్కిక flag = ఆదానం_2[పోర్టు] .సమితి_vec(పునాది, బిట్టు);
      ఒకవేళ (flag == అబద్ధం)
	    తిరిగి;

      ఒకవేళ (net_ == 0) {
	    net_ = ptr.ptr();
	    కాలపట్టిక_functor(ఇది);
      }
}

శూన్య vvp_fun_boolean_::recv_real(vvp_net_ptr_t ptr, ద్విగుణ real,
                                 vvp_context_t ctx)
{
      recv_vec4(ptr, double_గమ్యం_vector4_LSB(real), ctx);
}

vvp_fun_and::vvp_fun_and(గుర్తులేని wid, తార్కిక invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      లెక్క_functors_logic += 1;
}

vvp_fun_and::~vvp_fun_and()
{
}

శూన్య vvp_fun_and::నిరంతరం_నిరంతరం()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t ఫలితం (ఆదానం_2[0]);

      కొరకు (గుర్తులేని pdx = 1 ; pdx < 4 ; pdx += 1)
	    ఫలితం &= ఆదానం_2[pdx];

      ఒకవేళ (invert_)
	    ఫలితం.invert();

      ptr->send_vec4(ఫలితం, 0);
}

vvp_fun_equiv::vvp_fun_equiv()
: vvp_fun_boolean_(1)
{
      లెక్క_functors_logic += 1;
}

vvp_fun_equiv::~vvp_fun_equiv()
{
}

శూన్య vvp_fun_equiv::నిరంతరం_నిరంతరం()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      ప్రతిజ్ఞా(ఆదానం_2[0].size() == 1);
      ప్రతిజ్ఞా(ఆదానం_2[1].size() == 1);

      vvp_bit4_t బిట్టు = ~(ఆదానం_2[0].value(0) ^ ఆదానం_2[1].value(0));
      vvp_vector4_t ఫలితం (1, బిట్టు);

      ptr->send_vec4(ఫలితం, 0);
}

vvp_fun_impl::vvp_fun_impl()
: vvp_fun_boolean_(1)
{
      లెక్క_functors_logic += 1;
}

vvp_fun_impl::~vvp_fun_impl()
{
}

శూన్య vvp_fun_impl::నిరంతరం_నిరంతరం()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      ప్రతిజ్ఞా(ఆదానం_2[0].size() == 1);
      ప్రతిజ్ఞా(ఆదానం_2[1].size() == 1);

      vvp_bit4_t బిట్టు = ~ఆదానం_2[0].value(0) | ఆదానం_2[1].value(0);
      vvp_vector4_t ఫలితం (1, బిట్టు);

      ptr->send_vec4(ఫలితం, 0);
}

vvp_fun_buf_not_::vvp_fun_buf_not_(గుర్తులేని wid)
: ఆదానం_2(wid, BIT4_Z)
{
      net_ = 0;
}

vvp_fun_buf_not_::~vvp_fun_buf_not_()
{
}

శూన్య vvp_fun_buf_not_::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                 vvp_context_t)
{
      ఒకవేళ (ptr.పోర్టు() != 0) తిరిగి;

      ఒకవేళ (ఆదానం_2 .eeq( బిట్టు )) తిరిగి;

      ఆదానం_2 = బిట్టు;
      ఒకవేళ (net_ == 0) {
	    net_ = ptr.ptr();
	    కాలపట్టిక_functor(ఇది);
      }
}

శూన్య vvp_fun_buf_not_::recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                    గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t)
{
      ఒకవేళ (ptr.పోర్టు() != 0) తిరిగి;

      ప్రతిజ్ఞా(పునాది + బిట్టు.size() <= vwid);

	// Set the input part. If nothing changes, then break.
      తార్కిక flag = ఆదానం_2.సమితి_vec(పునాది, బిట్టు);
      ఒకవేళ (flag == అబద్ధం) తిరిగి;

      ఒకవేళ (net_ == 0) {
	    net_ = ptr.ptr();
	    కాలపట్టిక_functor(ఇది);
      }
}

శూన్య vvp_fun_buf_not_::recv_real(vvp_net_ptr_t ptr, ద్విగుణ real,
                                 vvp_context_t ctx)
{
      ఒకవేళ (ptr.పోర్టు() != 0) తిరిగి;

      recv_vec4(ptr, double_గమ్యం_vector4_LSB(real), ctx);
}

vvp_fun_buf::vvp_fun_buf(గుర్తులేని wid)
: vvp_fun_buf_not_(wid)
{
      లెక్క_functors_logic += 1;
}

vvp_fun_buf::~vvp_fun_buf()
{
}

/*
 * The buf functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the result.
 */
శూన్య vvp_fun_buf::నిరంతరం_నిరంతరం()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t tmp (ఆదానం_2);
      tmp.change_z2x();
      ptr->send_vec4(tmp, 0);
}

vvp_fun_bufz::vvp_fun_bufz()
{
      లెక్క_functors_logic += 1;
}

vvp_fun_bufz::~vvp_fun_bufz()
{
}

/*
 * The bufz is similar to the buf device, except that it does not
 * bother translating z bits to x.
 */
శూన్య vvp_fun_bufz::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                             vvp_context_t)
{
      ఒకవేళ (ptr.పోర్టు() != 0)
	    తిరిగి;

      ptr.ptr()->send_vec4(బిట్టు, 0);
}

శూన్య vvp_fun_bufz::recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t)
{
      ఒకవేళ (ptr.పోర్టు() != 0)
	    తిరిగి;

      ptr.ptr()->send_vec4_pv(బిట్టు, పునాది, vwid, 0);
}

శూన్య vvp_fun_bufz::recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                             vvp_context_t)
{
      ఒకవేళ (ptr.పోర్టు() != 0)
	    తిరిగి;

      ptr.ptr()->send_real(బిట్టు, 0);
}

శూన్య vvp_fun_buft::recv_vec8(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector8_t&బిట్టు)
{
      ఒకవేళ (ptr.పోర్టు() != 0)
	    తిరిగి;

      ptr.ptr()->send_vec8(బిట్టు);
}

vvp_fun_muxr::vvp_fun_muxr()
: a_(0.0), b_(0.0)
{
      net_ = 0;
      లెక్క_functors_logic += 1;
      select_ = SEL_BOTH;
}

vvp_fun_muxr::~vvp_fun_muxr()
{
}

శూన్య vvp_fun_muxr::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                             vvp_context_t)
{
	/* The real valued mux can only take in the select as a
	   vector4_t. The muxed data is real. */
      ఒకవేళ (ptr.పోర్టు() != 2)
	    తిరిగి;

      ప్రతిజ్ఞా(బిట్టు.size() == 1);

      ఎంపిక (బిట్టు.value(0)) {
	  స్థితి BIT4_0:
	    ఒకవేళ (select_ == SEL_PORT0) తిరిగి;
	    select_ = SEL_PORT0;
	    ఆపు;
	  స్థితి BIT4_1:
	    ఒకవేళ (select_ == SEL_PORT1) తిరిగి;
	    select_ = SEL_PORT1;
	    ఆపు;
	  అప్రమేయ:
	    ఒకవేళ (select_ == SEL_BOTH) తిరిగి;
	    select_ = SEL_BOTH;
      }

      ఒకవేళ (net_ == 0) {
	    net_ = ptr.ptr();
	    కాలపట్టిక_functor(ఇది);
      }
}

శూన్య vvp_fun_muxr::recv_real(vvp_net_ptr_t ptr, ద్విగుణ బిట్టు,
                             vvp_context_t)
{
      ఎంపిక (ptr.పోర్టు()) {
	  స్థితి 0:
	    ఒకవేళ (a_ == బిట్టు) తిరిగి;
	    a_ = బిట్టు;
	    ఒకవేళ (select_ == SEL_PORT1) తిరిగి; // The other port is selected.
	    ఆపు;

	  స్థితి 1:
	    ఒకవేళ (b_ == బిట్టు) తిరిగి;
	    b_ = బిట్టు;
	    ఒకవేళ (select_ == SEL_PORT0) తిరిగి; // The other port is selected.
	    ఆపు;

	  అప్రమేయ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.పోర్టు());
	    ప్రతిజ్ఞా(0);
      }

      ఒకవేళ (net_ == 0) {
	    net_ = ptr.ptr();
	    కాలపట్టిక_functor(ఇది);
      }
}

శూన్య vvp_fun_muxr::నిరంతరం_నిరంతరం()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      ఎంపిక (select_) {
	  స్థితి SEL_PORT0:
	    ptr->send_real(a_, 0);
	    ఆపు;
	  స్థితి SEL_PORT1:
	    ptr->send_real(b_, 0);
	    ఆపు;
	  అప్రమేయ:
	    ఒకవేళ (a_ == b_) {
		  ptr->send_real(a_, 0);
	    } లేకపోతే {
		  ptr->send_real(0.0, 0); // Should this be NaN?
	    }
	    ఆపు;
      }
}

vvp_fun_muxz::vvp_fun_muxz(గుర్తులేని wid)
: a_(wid, BIT4_Z), b_(wid, BIT4_Z)
{
      net_ = 0;
      లెక్క_functors_logic += 1;
      select_ = SEL_BOTH;
      has_నిరంతరం = అబద్ధం;
}

vvp_fun_muxz::~vvp_fun_muxz()
{
}

శూన్య vvp_fun_muxz::recv_vec4(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
                             vvp_context_t)
{
      ఎంపిక (ptr.పోర్టు()) {
	  స్థితి 0:
	    ఒకవేళ (a_ .eeq(బిట్టు) && has_నిరంతరం) తిరిగి;
	    a_ = బిట్టు;
	    ఒకవేళ (select_ == SEL_PORT1) తిరిగి; // The other port is selected.
	    ఆపు;
	  స్థితి 1:
	    ఒకవేళ (b_ .eeq(బిట్టు) && has_నిరంతరం) తిరిగి;
	    b_ = బిట్టు;
	    ఒకవేళ (select_ == SEL_PORT0) తిరిగి; // The other port is selected.
	    ఆపు;
	  స్థితి 2:
	    ప్రతిజ్ఞా(బిట్టు.size() == 1);
	    ఎంపిక (బిట్టు.value(0)) {
		స్థితి BIT4_0:
		  ఒకవేళ (select_ == SEL_PORT0) తిరిగి;
		  select_ = SEL_PORT0;
		  ఆపు;
		స్థితి BIT4_1:
		  ఒకవేళ (select_ == SEL_PORT1) తిరిగి;
		  select_ = SEL_PORT1;
		  ఆపు;
		అప్రమేయ:
		  ఒకవేళ (select_ == SEL_BOTH && has_నిరంతరం) తిరిగి;
		  select_ = SEL_BOTH;
	    }
	    ఆపు;
	  అప్రమేయ:
	    తిరిగి;
      }

      ఒకవేళ (net_ == 0) {
	    net_ = ptr.ptr();
	    కాలపట్టిక_functor(ఇది);
      }
}

శూన్య vvp_fun_muxz::recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
				గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t)
{
      ప్రతిజ్ఞా(పునాది + బిట్టు.size() <= vwid);
      తార్కిక flag;

      ఎంపిక (ptr.పోర్టు()) {
	  స్థితి 0:
	    flag = a_.సమితి_vec(పునాది, బిట్టు);
	    ఒకవేళ (flag == అబద్ధం && has_నిరంతరం) తిరిగి;
	    ఒకవేళ (select_ == SEL_PORT1) తిరిగి; // The other port is selected.
	    ఆపు;
	  స్థితి 1:
	    flag = b_.సమితి_vec(పునాది, బిట్టు);
	    ఒకవేళ (flag == అబద్ధం && has_నిరంతరం) తిరిగి;
	    ఒకవేళ (select_ == SEL_PORT0) తిరిగి; // The other port is selected.
	    ఆపు;
	  స్థితి 2:
	    ప్రతిజ్ఞా((పునాది == 0) && (బిట్టు.size() == 1));
	    recv_vec4(ptr, బిట్టు, 0);
	  అప్రమేయ:
	    తిరిగి;
      }
      ఒకవేళ (net_ == 0) {
	    net_ = ptr.ptr();
	    కాలపట్టిక_functor(ఇది);
      }
}

శూన్య vvp_fun_muxz::నిరంతరం_నిరంతరం()
{
      has_నిరంతరం = నిజం;
      vvp_net_t*ptr = net_;
      net_ = 0;

      ఎంపిక (select_) {
	  స్థితి SEL_PORT0:
	    ptr->send_vec4(a_, 0);
	    ఆపు;
	  స్థితి SEL_PORT1:
	    ptr->send_vec4(b_, 0);
	    ఆపు;
	  అప్రమేయ:
	      {
		    గుర్తులేని min_కొలత = a_.size();
		    గుర్తులేని max_కొలత = min_కొలత;
		    ఒకవేళ (b_.size() < min_కొలత)
			  min_కొలత = b_.size();
		    ఒకవేళ (b_.size() > max_కొలత)
			  max_కొలత = b_.size();

		    vvp_vector4_t res (max_కొలత);

		    కొరకు (గుర్తులేని idx = 0 ;  idx < min_కొలత ;  idx += 1) {
			  ఒకవేళ (a_.value(idx) == b_.value(idx))
				res.సమితి_బిట్టు(idx, a_.value(idx));
			  లేకపోతే
				res.సమితి_బిట్టు(idx, BIT4_X);
		    }

		    కొరకు (గుర్తులేని idx = min_కొలత ;  idx < max_కొలత ;  idx += 1)
			  res.సమితి_బిట్టు(idx, BIT4_X);

		    ptr->send_vec4(res, 0);
	      }
	    ఆపు;
      }
}

vvp_fun_not::vvp_fun_not(గుర్తులేని wid)
: vvp_fun_buf_not_(wid)
{
      లెక్క_functors_logic += 1;
}

vvp_fun_not::~vvp_fun_not()
{
}

/*
 * The not functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the inverted result.
 */
శూన్య vvp_fun_not::నిరంతరం_నిరంతరం()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t ఫలితం (ఆదానం_2, నిజం /* invert */);
      ptr->send_vec4(ఫలితం, 0);
}

vvp_fun_or::vvp_fun_or(గుర్తులేని wid, తార్కిక invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      లెక్క_functors_logic += 1;
}

vvp_fun_or::~vvp_fun_or()
{
}

శూన్య vvp_fun_or::నిరంతరం_నిరంతరం()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t ఫలితం (ఆదానం_2[0]);

      కొరకు (గుర్తులేని pdx = 1 ; pdx < 4 ; pdx += 1)
	    ఫలితం |= ఆదానం_2[pdx];

      ఒకవేళ (invert_)
	    ఫలితం.invert();

      ptr->send_vec4(ఫలితం, 0);
}

vvp_fun_xor::vvp_fun_xor(గుర్తులేని wid, తార్కిక invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      లెక్క_functors_logic += 1;
}

vvp_fun_xor::~vvp_fun_xor()
{
}

శూన్య vvp_fun_xor::నిరంతరం_నిరంతరం()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t ఫలితం (ఆదానం_2[0]);

      కొరకు (గుర్తులేని pdx = 1 ; pdx < 4 ; pdx += 1)
	    ఫలితం ^= ఆదానం_2[pdx];

      ఒకవేళ (invert_)
	    ఫలితం.invert();

      ptr->send_vec4(ఫలితం, 0);
}

/*
 * The parser calls this function to create a logic functor. I allocate a
 * functor, and map the name to the vvp_ipoint_t address for the
 * functor. Also resolve the inputs to the functor.
 */

శూన్య compile_functor(అక్షరం*label, అక్షరం*type, గుర్తులేని width,
		     గుర్తులేని ostr0, గుర్తులేని ostr1,
		     గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      vvp_net_fun_t* obj = 0;
      తార్కిక strength_aware = అబద్ధం;

      ఒకవేళ (strcmp(type, "OR") == 0) {
	    obj = కొత్త vvp_fun_or(width, అబద్ధం);

      } లేకపోతే ఒకవేళ (strcmp(type, "AND") == 0) {
	    obj = కొత్త vvp_fun_and(width, అబద్ధం);

      } లేకపోతే ఒకవేళ (strcmp(type, "BUF") == 0) {
	    obj = కొత్త vvp_fun_buf(width);

      } లేకపోతే ఒకవేళ (strcmp(type, "BUFIF0") == 0) {
	    obj = కొత్త vvp_fun_bufif(నిజం,అబద్ధం, ostr0, ostr1);
	    strength_aware = నిజం;

      } లేకపోతే ఒకవేళ (strcmp(type, "BUFIF1") == 0) {
	    obj = కొత్త vvp_fun_bufif(అబద్ధం,అబద్ధం, ostr0, ostr1);
	    strength_aware = నిజం;

      } లేకపోతే ఒకవేళ (strcmp(type, "EQUIV") == 0) {
	    obj = కొత్త vvp_fun_equiv();

      } లేకపోతే ఒకవేళ (strcmp(type, "IMPL") == 0) {
	    obj = కొత్త vvp_fun_impl();

      } లేకపోతే ఒకవేళ (strcmp(type, "NAND") == 0) {
	    obj = కొత్త vvp_fun_and(width, నిజం);

      } లేకపోతే ఒకవేళ (strcmp(type, "NOR") == 0) {
	    obj = కొత్త vvp_fun_or(width, నిజం);

      } లేకపోతే ఒకవేళ (strcmp(type, "NOTIF0") == 0) {
	    obj = కొత్త vvp_fun_bufif(నిజం,నిజం, ostr0, ostr1);
	    strength_aware = నిజం;

      } లేకపోతే ఒకవేళ (strcmp(type, "NOTIF1") == 0) {
	    obj = కొత్త vvp_fun_bufif(అబద్ధం,నిజం, ostr0, ostr1);
	    strength_aware = నిజం;

      } లేకపోతే ఒకవేళ (strcmp(type, "BUFT") == 0) {
	    obj = కొత్త vvp_fun_buft();

      } లేకపోతే ఒకవేళ (strcmp(type, "BUFZ") == 0) {
	    obj = కొత్త vvp_fun_bufz();

      } లేకపోతే ఒకవేళ (strcmp(type, "MUXR") == 0) {
	    obj = కొత్త vvp_fun_muxr;

      } లేకపోతే ఒకవేళ (strcmp(type, "MUXZ") == 0) {
	    obj = కొత్త vvp_fun_muxz(width);

      } లేకపోతే ఒకవేళ (strcmp(type, "CMOS") == 0) {
	    obj = కొత్త vvp_fun_cmos();

      } లేకపోతే ఒకవేళ (strcmp(type, "NMOS") == 0) {
	    obj = కొత్త vvp_fun_pmos(నిజం);

      } లేకపోతే ఒకవేళ (strcmp(type, "PMOS") == 0) {
	    obj = కొత్త vvp_fun_pmos(అబద్ధం);

      } లేకపోతే ఒకవేళ (strcmp(type, "RCMOS") == 0) {
	    obj = కొత్త vvp_fun_rcmos();

      } లేకపోతే ఒకవేళ (strcmp(type, "RNMOS") == 0) {
	    obj = కొత్త vvp_fun_rpmos(నిజం);

      } లేకపోతే ఒకవేళ (strcmp(type, "RPMOS") == 0) {
	    obj = కొత్త vvp_fun_rpmos(అబద్ధం);

      } లేకపోతే ఒకవేళ (strcmp(type, "NOT") == 0) {
	    obj = కొత్త vvp_fun_not(width);

      } లేకపోతే ఒకవేళ (strcmp(type, "XNOR") == 0) {
	    obj = కొత్త vvp_fun_xor(width, నిజం);

      } లేకపోతే ఒకవేళ (strcmp(type, "XOR") == 0) {
	    obj = కొత్త vvp_fun_xor(width, అబద్ధం);

      } లేకపోతే {
	    yyerror("invalid functor type.");
	    విడుదల(type);
	    విడుదల(argv);
	    విడుదల(label);
	    తిరిగి;
      }

      విడుదల(type);

      ప్రతిజ్ఞా(argc <= 4);
      vvp_net_t*net = కొత్త vvp_net_t;
      net->fun = obj;

      inputs_connect(net, argc, argv);
      విడుదల(argv);

	/* If both the strengths are the default strong drive, then
	   there is no need for a specialized driver. Attach the label
	   to this node and we are finished. */
      ఒకవేళ (strength_aware || (ostr0 == 6 && ostr1 == 6)) {
	    define_functor_చిహ్నం(label, net);
	    విడుదల(label);
	    తిరిగి;
      }

      vvp_net_t*net_drv = కొత్త vvp_net_t;
      vvp_net_fun_t*obj_drv = కొత్త vvp_fun_drive(ostr0, ostr1);
      net_drv->fun = obj_drv;

	/* Point the gate to the drive node. */
      net->link(vvp_net_ptr_t(net_drv, 0));

      define_functor_చిహ్నం(label, net_drv);
      విడుదల(label);
}
