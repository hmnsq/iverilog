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

vvp_fun_boolean_::vvp_fun_boolean_(غیرعلامتی wid)
{
      net_ = 0;
      برائے (غیرعلامتی idx = 0 ;  idx < 4 ;  idx += 1)
	    داخلہ_2[idx] = vvp_vector4_t(wid, BIT4_Z);
}

vvp_fun_boolean_::~vvp_fun_boolean_()
{
}

خالی vvp_fun_boolean_::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                 vvp_context_t)
{
      غیرعلامتی پورٹ = ptr.پورٹ();
      اگر (داخلہ_2[پورٹ] .eeq( ثنائی_ہندسہ )) واپس;

      داخلہ_2[پورٹ] = ثنائی_ہندسہ;
      اگر (net_ == 0) {
	    net_ = ptr.ptr();
	    جدول_وقت_functor(یہ);
      }
}

خالی vvp_fun_boolean_::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				    غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t)
{
      غیرعلامتی پورٹ = ptr.پورٹ();

      أكد(بنیاد + ثنائی_ہندسہ.size() <= vwid);

	// Set the part for the input. If nothing changes, then break.
      منطقی flag = داخلہ_2[پورٹ] .سیٹ_vec(بنیاد, ثنائی_ہندسہ);
      اگر (flag == جھوٹ)
	    واپس;

      اگر (net_ == 0) {
	    net_ = ptr.ptr();
	    جدول_وقت_functor(یہ);
      }
}

خالی vvp_fun_boolean_::recv_real(vvp_net_ptr_t ptr, دوہرا real,
                                 vvp_context_t ctx)
{
      recv_vec4(ptr, double_مقصد_vector4_LSB(real), ctx);
}

vvp_fun_and::vvp_fun_and(غیرعلامتی wid, منطقی invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      گنتی_functors_logic += 1;
}

vvp_fun_and::~vvp_fun_and()
{
}

خالی vvp_fun_and::تسلسل_تسلسل()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t نتیجہ (داخلہ_2[0]);

      برائے (غیرعلامتی pdx = 1 ; pdx < 4 ; pdx += 1)
	    نتیجہ &= داخلہ_2[pdx];

      اگر (invert_)
	    نتیجہ.invert();

      ptr->send_vec4(نتیجہ, 0);
}

vvp_fun_equiv::vvp_fun_equiv()
: vvp_fun_boolean_(1)
{
      گنتی_functors_logic += 1;
}

vvp_fun_equiv::~vvp_fun_equiv()
{
}

خالی vvp_fun_equiv::تسلسل_تسلسل()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      أكد(داخلہ_2[0].size() == 1);
      أكد(داخلہ_2[1].size() == 1);

      vvp_bit4_t ثنائی_ہندسہ = ~(داخلہ_2[0].value(0) ^ داخلہ_2[1].value(0));
      vvp_vector4_t نتیجہ (1, ثنائی_ہندسہ);

      ptr->send_vec4(نتیجہ, 0);
}

vvp_fun_impl::vvp_fun_impl()
: vvp_fun_boolean_(1)
{
      گنتی_functors_logic += 1;
}

vvp_fun_impl::~vvp_fun_impl()
{
}

خالی vvp_fun_impl::تسلسل_تسلسل()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      أكد(داخلہ_2[0].size() == 1);
      أكد(داخلہ_2[1].size() == 1);

      vvp_bit4_t ثنائی_ہندسہ = ~داخلہ_2[0].value(0) | داخلہ_2[1].value(0);
      vvp_vector4_t نتیجہ (1, ثنائی_ہندسہ);

      ptr->send_vec4(نتیجہ, 0);
}

vvp_fun_buf_not_::vvp_fun_buf_not_(غیرعلامتی wid)
: داخلہ_2(wid, BIT4_Z)
{
      net_ = 0;
}

vvp_fun_buf_not_::~vvp_fun_buf_not_()
{
}

خالی vvp_fun_buf_not_::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                 vvp_context_t)
{
      اگر (ptr.پورٹ() != 0) واپس;

      اگر (داخلہ_2 .eeq( ثنائی_ہندسہ )) واپس;

      داخلہ_2 = ثنائی_ہندسہ;
      اگر (net_ == 0) {
	    net_ = ptr.ptr();
	    جدول_وقت_functor(یہ);
      }
}

خالی vvp_fun_buf_not_::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                    غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t)
{
      اگر (ptr.پورٹ() != 0) واپس;

      أكد(بنیاد + ثنائی_ہندسہ.size() <= vwid);

	// Set the input part. If nothing changes, then break.
      منطقی flag = داخلہ_2.سیٹ_vec(بنیاد, ثنائی_ہندسہ);
      اگر (flag == جھوٹ) واپس;

      اگر (net_ == 0) {
	    net_ = ptr.ptr();
	    جدول_وقت_functor(یہ);
      }
}

خالی vvp_fun_buf_not_::recv_real(vvp_net_ptr_t ptr, دوہرا real,
                                 vvp_context_t ctx)
{
      اگر (ptr.پورٹ() != 0) واپس;

      recv_vec4(ptr, double_مقصد_vector4_LSB(real), ctx);
}

vvp_fun_buf::vvp_fun_buf(غیرعلامتی wid)
: vvp_fun_buf_not_(wid)
{
      گنتی_functors_logic += 1;
}

vvp_fun_buf::~vvp_fun_buf()
{
}

/*
 * The buf functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the result.
 */
خالی vvp_fun_buf::تسلسل_تسلسل()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t tmp (داخلہ_2);
      tmp.change_z2x();
      ptr->send_vec4(tmp, 0);
}

vvp_fun_bufz::vvp_fun_bufz()
{
      گنتی_functors_logic += 1;
}

vvp_fun_bufz::~vvp_fun_bufz()
{
}

/*
 * The bufz is similar to the buf device, except that it does not
 * bother translating z bits to x.
 */
خالی vvp_fun_bufz::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                             vvp_context_t)
{
      اگر (ptr.پورٹ() != 0)
	    واپس;

      ptr.ptr()->send_vec4(ثنائی_ہندسہ, 0);
}

خالی vvp_fun_bufz::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t)
{
      اگر (ptr.پورٹ() != 0)
	    واپس;

      ptr.ptr()->send_vec4_pv(ثنائی_ہندسہ, بنیاد, vwid, 0);
}

خالی vvp_fun_bufz::recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                             vvp_context_t)
{
      اگر (ptr.پورٹ() != 0)
	    واپس;

      ptr.ptr()->send_real(ثنائی_ہندسہ, 0);
}

خالی vvp_fun_buft::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      اگر (ptr.پورٹ() != 0)
	    واپس;

      ptr.ptr()->send_vec8(ثنائی_ہندسہ);
}

vvp_fun_muxr::vvp_fun_muxr()
: a_(0.0), b_(0.0)
{
      net_ = 0;
      گنتی_functors_logic += 1;
      select_ = SEL_BOTH;
}

vvp_fun_muxr::~vvp_fun_muxr()
{
}

خالی vvp_fun_muxr::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                             vvp_context_t)
{
	/* The real valued mux can only take in the select as a
	   vector4_t. The muxed data is real. */
      اگر (ptr.پورٹ() != 2)
	    واپس;

      أكد(ثنائی_ہندسہ.size() == 1);

      انتخاب (ثنائی_ہندسہ.value(0)) {
	  حالت BIT4_0:
	    اگر (select_ == SEL_PORT0) واپس;
	    select_ = SEL_PORT0;
	    توڑو;
	  حالت BIT4_1:
	    اگر (select_ == SEL_PORT1) واپس;
	    select_ = SEL_PORT1;
	    توڑو;
	  طےشدہ:
	    اگر (select_ == SEL_BOTH) واپس;
	    select_ = SEL_BOTH;
      }

      اگر (net_ == 0) {
	    net_ = ptr.ptr();
	    جدول_وقت_functor(یہ);
      }
}

خالی vvp_fun_muxr::recv_real(vvp_net_ptr_t ptr, دوہرا ثنائی_ہندسہ,
                             vvp_context_t)
{
      انتخاب (ptr.پورٹ()) {
	  حالت 0:
	    اگر (a_ == ثنائی_ہندسہ) واپس;
	    a_ = ثنائی_ہندسہ;
	    اگر (select_ == SEL_PORT1) واپس; // The other port is selected.
	    توڑو;

	  حالت 1:
	    اگر (b_ == ثنائی_ہندسہ) واپس;
	    b_ = ثنائی_ہندسہ;
	    اگر (select_ == SEL_PORT0) واپس; // The other port is selected.
	    توڑو;

	  طےشدہ:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.پورٹ());
	    أكد(0);
      }

      اگر (net_ == 0) {
	    net_ = ptr.ptr();
	    جدول_وقت_functor(یہ);
      }
}

خالی vvp_fun_muxr::تسلسل_تسلسل()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      انتخاب (select_) {
	  حالت SEL_PORT0:
	    ptr->send_real(a_, 0);
	    توڑو;
	  حالت SEL_PORT1:
	    ptr->send_real(b_, 0);
	    توڑو;
	  طےشدہ:
	    اگر (a_ == b_) {
		  ptr->send_real(a_, 0);
	    } ورنہ {
		  ptr->send_real(0.0, 0); // Should this be NaN?
	    }
	    توڑو;
      }
}

vvp_fun_muxz::vvp_fun_muxz(غیرعلامتی wid)
: a_(wid, BIT4_Z), b_(wid, BIT4_Z)
{
      net_ = 0;
      گنتی_functors_logic += 1;
      select_ = SEL_BOTH;
      has_تسلسل = جھوٹ;
}

vvp_fun_muxz::~vvp_fun_muxz()
{
}

خالی vvp_fun_muxz::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                             vvp_context_t)
{
      انتخاب (ptr.پورٹ()) {
	  حالت 0:
	    اگر (a_ .eeq(ثنائی_ہندسہ) && has_تسلسل) واپس;
	    a_ = ثنائی_ہندسہ;
	    اگر (select_ == SEL_PORT1) واپس; // The other port is selected.
	    توڑو;
	  حالت 1:
	    اگر (b_ .eeq(ثنائی_ہندسہ) && has_تسلسل) واپس;
	    b_ = ثنائی_ہندسہ;
	    اگر (select_ == SEL_PORT0) واپس; // The other port is selected.
	    توڑو;
	  حالت 2:
	    أكد(ثنائی_ہندسہ.size() == 1);
	    انتخاب (ثنائی_ہندسہ.value(0)) {
		حالت BIT4_0:
		  اگر (select_ == SEL_PORT0) واپس;
		  select_ = SEL_PORT0;
		  توڑو;
		حالت BIT4_1:
		  اگر (select_ == SEL_PORT1) واپس;
		  select_ = SEL_PORT1;
		  توڑو;
		طےشدہ:
		  اگر (select_ == SEL_BOTH && has_تسلسل) واپس;
		  select_ = SEL_BOTH;
	    }
	    توڑو;
	  طےشدہ:
	    واپس;
      }

      اگر (net_ == 0) {
	    net_ = ptr.ptr();
	    جدول_وقت_functor(یہ);
      }
}

خالی vvp_fun_muxz::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t)
{
      أكد(بنیاد + ثنائی_ہندسہ.size() <= vwid);
      منطقی flag;

      انتخاب (ptr.پورٹ()) {
	  حالت 0:
	    flag = a_.سیٹ_vec(بنیاد, ثنائی_ہندسہ);
	    اگر (flag == جھوٹ && has_تسلسل) واپس;
	    اگر (select_ == SEL_PORT1) واپس; // The other port is selected.
	    توڑو;
	  حالت 1:
	    flag = b_.سیٹ_vec(بنیاد, ثنائی_ہندسہ);
	    اگر (flag == جھوٹ && has_تسلسل) واپس;
	    اگر (select_ == SEL_PORT0) واپس; // The other port is selected.
	    توڑو;
	  حالت 2:
	    أكد((بنیاد == 0) && (ثنائی_ہندسہ.size() == 1));
	    recv_vec4(ptr, ثنائی_ہندسہ, 0);
	  طےشدہ:
	    واپس;
      }
      اگر (net_ == 0) {
	    net_ = ptr.ptr();
	    جدول_وقت_functor(یہ);
      }
}

خالی vvp_fun_muxz::تسلسل_تسلسل()
{
      has_تسلسل = سچ;
      vvp_net_t*ptr = net_;
      net_ = 0;

      انتخاب (select_) {
	  حالت SEL_PORT0:
	    ptr->send_vec4(a_, 0);
	    توڑو;
	  حالت SEL_PORT1:
	    ptr->send_vec4(b_, 0);
	    توڑو;
	  طےشدہ:
	      {
		    غیرعلامتی min_سائز = a_.size();
		    غیرعلامتی max_سائز = min_سائز;
		    اگر (b_.size() < min_سائز)
			  min_سائز = b_.size();
		    اگر (b_.size() > max_سائز)
			  max_سائز = b_.size();

		    vvp_vector4_t res (max_سائز);

		    برائے (غیرعلامتی idx = 0 ;  idx < min_سائز ;  idx += 1) {
			  اگر (a_.value(idx) == b_.value(idx))
				res.سیٹ_ثنائی_ہندسہ(idx, a_.value(idx));
			  ورنہ
				res.سیٹ_ثنائی_ہندسہ(idx, BIT4_X);
		    }

		    برائے (غیرعلامتی idx = min_سائز ;  idx < max_سائز ;  idx += 1)
			  res.سیٹ_ثنائی_ہندسہ(idx, BIT4_X);

		    ptr->send_vec4(res, 0);
	      }
	    توڑو;
      }
}

vvp_fun_not::vvp_fun_not(غیرعلامتی wid)
: vvp_fun_buf_not_(wid)
{
      گنتی_functors_logic += 1;
}

vvp_fun_not::~vvp_fun_not()
{
}

/*
 * The not functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the inverted result.
 */
خالی vvp_fun_not::تسلسل_تسلسل()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t نتیجہ (داخلہ_2, سچ /* invert */);
      ptr->send_vec4(نتیجہ, 0);
}

vvp_fun_or::vvp_fun_or(غیرعلامتی wid, منطقی invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      گنتی_functors_logic += 1;
}

vvp_fun_or::~vvp_fun_or()
{
}

خالی vvp_fun_or::تسلسل_تسلسل()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t نتیجہ (داخلہ_2[0]);

      برائے (غیرعلامتی pdx = 1 ; pdx < 4 ; pdx += 1)
	    نتیجہ |= داخلہ_2[pdx];

      اگر (invert_)
	    نتیجہ.invert();

      ptr->send_vec4(نتیجہ, 0);
}

vvp_fun_xor::vvp_fun_xor(غیرعلامتی wid, منطقی invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      گنتی_functors_logic += 1;
}

vvp_fun_xor::~vvp_fun_xor()
{
}

خالی vvp_fun_xor::تسلسل_تسلسل()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t نتیجہ (داخلہ_2[0]);

      برائے (غیرعلامتی pdx = 1 ; pdx < 4 ; pdx += 1)
	    نتیجہ ^= داخلہ_2[pdx];

      اگر (invert_)
	    نتیجہ.invert();

      ptr->send_vec4(نتیجہ, 0);
}

/*
 * The parser calls this function to create a logic functor. I allocate a
 * functor, and map the name to the vvp_ipoint_t address for the
 * functor. Also resolve the inputs to the functor.
 */

خالی compile_functor(حرف*label, حرف*type, غیرعلامتی width,
		     غیرعلامتی ostr0, غیرعلامتی ostr1,
		     غیرعلامتی argc, ساخت symb_s*argv)
{
      vvp_net_fun_t* obj = 0;
      منطقی strength_aware = جھوٹ;

      اگر (strcmp(type, "OR") == 0) {
	    obj = نیا vvp_fun_or(width, جھوٹ);

      } ورنہ اگر (strcmp(type, "AND") == 0) {
	    obj = نیا vvp_fun_and(width, جھوٹ);

      } ورنہ اگر (strcmp(type, "BUF") == 0) {
	    obj = نیا vvp_fun_buf(width);

      } ورنہ اگر (strcmp(type, "BUFIF0") == 0) {
	    obj = نیا vvp_fun_bufif(سچ,جھوٹ, ostr0, ostr1);
	    strength_aware = سچ;

      } ورنہ اگر (strcmp(type, "BUFIF1") == 0) {
	    obj = نیا vvp_fun_bufif(جھوٹ,جھوٹ, ostr0, ostr1);
	    strength_aware = سچ;

      } ورنہ اگر (strcmp(type, "EQUIV") == 0) {
	    obj = نیا vvp_fun_equiv();

      } ورنہ اگر (strcmp(type, "IMPL") == 0) {
	    obj = نیا vvp_fun_impl();

      } ورنہ اگر (strcmp(type, "NAND") == 0) {
	    obj = نیا vvp_fun_and(width, سچ);

      } ورنہ اگر (strcmp(type, "NOR") == 0) {
	    obj = نیا vvp_fun_or(width, سچ);

      } ورنہ اگر (strcmp(type, "NOTIF0") == 0) {
	    obj = نیا vvp_fun_bufif(سچ,سچ, ostr0, ostr1);
	    strength_aware = سچ;

      } ورنہ اگر (strcmp(type, "NOTIF1") == 0) {
	    obj = نیا vvp_fun_bufif(جھوٹ,سچ, ostr0, ostr1);
	    strength_aware = سچ;

      } ورنہ اگر (strcmp(type, "BUFT") == 0) {
	    obj = نیا vvp_fun_buft();

      } ورنہ اگر (strcmp(type, "BUFZ") == 0) {
	    obj = نیا vvp_fun_bufz();

      } ورنہ اگر (strcmp(type, "MUXR") == 0) {
	    obj = نیا vvp_fun_muxr;

      } ورنہ اگر (strcmp(type, "MUXZ") == 0) {
	    obj = نیا vvp_fun_muxz(width);

      } ورنہ اگر (strcmp(type, "CMOS") == 0) {
	    obj = نیا vvp_fun_cmos();

      } ورنہ اگر (strcmp(type, "NMOS") == 0) {
	    obj = نیا vvp_fun_pmos(سچ);

      } ورنہ اگر (strcmp(type, "PMOS") == 0) {
	    obj = نیا vvp_fun_pmos(جھوٹ);

      } ورنہ اگر (strcmp(type, "RCMOS") == 0) {
	    obj = نیا vvp_fun_rcmos();

      } ورنہ اگر (strcmp(type, "RNMOS") == 0) {
	    obj = نیا vvp_fun_rpmos(سچ);

      } ورنہ اگر (strcmp(type, "RPMOS") == 0) {
	    obj = نیا vvp_fun_rpmos(جھوٹ);

      } ورنہ اگر (strcmp(type, "NOT") == 0) {
	    obj = نیا vvp_fun_not(width);

      } ورنہ اگر (strcmp(type, "XNOR") == 0) {
	    obj = نیا vvp_fun_xor(width, سچ);

      } ورنہ اگر (strcmp(type, "XOR") == 0) {
	    obj = نیا vvp_fun_xor(width, جھوٹ);

      } ورنہ {
	    yyerror("invalid functor type.");
	    آزاد(type);
	    آزاد(argv);
	    آزاد(label);
	    واپس;
      }

      آزاد(type);

      أكد(argc <= 4);
      vvp_net_t*net = نیا vvp_net_t;
      net->fun = obj;

      inputs_connect(net, argc, argv);
      آزاد(argv);

	/* If both the strengths are the default strong drive, then
	   there is no need for a specialized driver. Attach the label
	   to this node and we are finished. */
      اگر (strength_aware || (ostr0 == 6 && ostr1 == 6)) {
	    define_functor_علامت(label, net);
	    آزاد(label);
	    واپس;
      }

      vvp_net_t*net_drv = نیا vvp_net_t;
      vvp_net_fun_t*obj_drv = نیا vvp_fun_drive(ostr0, ostr1);
      net_drv->fun = obj_drv;

	/* Point the gate to the drive node. */
      net->link(vvp_net_ptr_t(net_drv, 0));

      define_functor_علامت(label, net_drv);
      آزاد(label);
}
