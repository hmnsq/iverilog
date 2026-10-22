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

vvp_fun_boolean_::vvp_fun_boolean_(بنانشانی wid)
{
      net_ = 0;
      لكل (بنانشانی idx = 0 ;  idx < 4 ;  idx += 1)
	    داخلہ_2[idx] = vvp_vector4_t(wid, BIT4_Z);
}

vvp_fun_boolean_::~vvp_fun_boolean_()
{
}

فراغ vvp_fun_boolean_::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                 vvp_context_t)
{
      بنانشانی پورٹ = ptr.پورٹ();
      إذا (داخلہ_2[پورٹ] .eeq( ثنائی_ہندسہ )) أرجع;

      داخلہ_2[پورٹ] = ثنائی_ہندسہ;
      إذا (net_ == 0) {
	    net_ = ptr.ptr();
	    ویلا_سارنی_functor(هذا);
      }
}

فراغ vvp_fun_boolean_::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				    بنانشانی ادھار, بنانشانی vwid, vvp_context_t)
{
      بنانشانی پورٹ = ptr.پورٹ();

      أكد(ادھار + ثنائی_ہندسہ.size() <= vwid);

	// Set the part for the input. If nothing changes, then break.
      منطقي flag = داخلہ_2[پورٹ] .سیٹ_vec(ادھار, ثنائی_ہندسہ);
      إذا (flag == خطأ)
	    أرجع;

      إذا (net_ == 0) {
	    net_ = ptr.ptr();
	    ویلا_سارنی_functor(هذا);
      }
}

فراغ vvp_fun_boolean_::recv_real(vvp_net_ptr_t ptr, مزدوج real,
                                 vvp_context_t ctx)
{
      recv_vec4(ptr, double_منزل_vector4_LSB(real), ctx);
}

vvp_fun_and::vvp_fun_and(بنانشانی wid, منطقي invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      گنتی_functors_logic += 1;
}

vvp_fun_and::~vvp_fun_and()
{
}

فراغ vvp_fun_and::لگاتار_لگاتار()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t نتیجہ (داخلہ_2[0]);

      لكل (بنانشانی pdx = 1 ; pdx < 4 ; pdx += 1)
	    نتیجہ &= داخلہ_2[pdx];

      إذا (invert_)
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

فراغ vvp_fun_equiv::لگاتار_لگاتار()
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

فراغ vvp_fun_impl::لگاتار_لگاتار()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      أكد(داخلہ_2[0].size() == 1);
      أكد(داخلہ_2[1].size() == 1);

      vvp_bit4_t ثنائی_ہندسہ = ~داخلہ_2[0].value(0) | داخلہ_2[1].value(0);
      vvp_vector4_t نتیجہ (1, ثنائی_ہندسہ);

      ptr->send_vec4(نتیجہ, 0);
}

vvp_fun_buf_not_::vvp_fun_buf_not_(بنانشانی wid)
: داخلہ_2(wid, BIT4_Z)
{
      net_ = 0;
}

vvp_fun_buf_not_::~vvp_fun_buf_not_()
{
}

فراغ vvp_fun_buf_not_::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                 vvp_context_t)
{
      إذا (ptr.پورٹ() != 0) أرجع;

      إذا (داخلہ_2 .eeq( ثنائی_ہندسہ )) أرجع;

      داخلہ_2 = ثنائی_ہندسہ;
      إذا (net_ == 0) {
	    net_ = ptr.ptr();
	    ویلا_سارنی_functor(هذا);
      }
}

فراغ vvp_fun_buf_not_::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                    بنانشانی ادھار, بنانشانی vwid, vvp_context_t)
{
      إذا (ptr.پورٹ() != 0) أرجع;

      أكد(ادھار + ثنائی_ہندسہ.size() <= vwid);

	// Set the input part. If nothing changes, then break.
      منطقي flag = داخلہ_2.سیٹ_vec(ادھار, ثنائی_ہندسہ);
      إذا (flag == خطأ) أرجع;

      إذا (net_ == 0) {
	    net_ = ptr.ptr();
	    ویلا_سارنی_functor(هذا);
      }
}

فراغ vvp_fun_buf_not_::recv_real(vvp_net_ptr_t ptr, مزدوج real,
                                 vvp_context_t ctx)
{
      إذا (ptr.پورٹ() != 0) أرجع;

      recv_vec4(ptr, double_منزل_vector4_LSB(real), ctx);
}

vvp_fun_buf::vvp_fun_buf(بنانشانی wid)
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
فراغ vvp_fun_buf::لگاتار_لگاتار()
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
فراغ vvp_fun_bufz::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                             vvp_context_t)
{
      إذا (ptr.پورٹ() != 0)
	    أرجع;

      ptr.ptr()->send_vec4(ثنائی_ہندسہ, 0);
}

فراغ vvp_fun_bufz::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                بنانشانی ادھار, بنانشانی vwid, vvp_context_t)
{
      إذا (ptr.پورٹ() != 0)
	    أرجع;

      ptr.ptr()->send_vec4_pv(ثنائی_ہندسہ, ادھار, vwid, 0);
}

فراغ vvp_fun_bufz::recv_real(vvp_net_ptr_t ptr, مزدوج ثنائی_ہندسہ,
                             vvp_context_t)
{
      إذا (ptr.پورٹ() != 0)
	    أرجع;

      ptr.ptr()->send_real(ثنائی_ہندسہ, 0);
}

فراغ vvp_fun_buft::recv_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      إذا (ptr.پورٹ() != 0)
	    أرجع;

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

فراغ vvp_fun_muxr::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                             vvp_context_t)
{
	/* The real valued mux can only take in the select as a
	   vector4_t. The muxed data is real. */
      إذا (ptr.پورٹ() != 2)
	    أرجع;

      أكد(ثنائی_ہندسہ.size() == 1);

      بدل (ثنائی_ہندسہ.value(0)) {
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
	    ویلا_سارنی_functor(هذا);
      }
}

فراغ vvp_fun_muxr::recv_real(vvp_net_ptr_t ptr, مزدوج ثنائی_ہندسہ,
                             vvp_context_t)
{
      بدل (ptr.پورٹ()) {
	  حالة 0:
	    إذا (a_ == ثنائی_ہندسہ) أرجع;
	    a_ = ثنائی_ہندسہ;
	    إذا (select_ == SEL_PORT1) أرجع; // The other port is selected.
	    اكسر;

	  حالة 1:
	    إذا (b_ == ثنائی_ہندسہ) أرجع;
	    b_ = ثنائی_ہندسہ;
	    إذا (select_ == SEL_PORT0) أرجع; // The other port is selected.
	    اكسر;

	  افتراضي:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.پورٹ());
	    أكد(0);
      }

      إذا (net_ == 0) {
	    net_ = ptr.ptr();
	    ویلا_سارنی_functor(هذا);
      }
}

فراغ vvp_fun_muxr::لگاتار_لگاتار()
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

vvp_fun_muxz::vvp_fun_muxz(بنانشانی wid)
: a_(wid, BIT4_Z), b_(wid, BIT4_Z)
{
      net_ = 0;
      گنتی_functors_logic += 1;
      select_ = SEL_BOTH;
      has_لگاتار = خطأ;
}

vvp_fun_muxz::~vvp_fun_muxz()
{
}

فراغ vvp_fun_muxz::recv_vec4(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                             vvp_context_t)
{
      بدل (ptr.پورٹ()) {
	  حالة 0:
	    إذا (a_ .eeq(ثنائی_ہندسہ) && has_لگاتار) أرجع;
	    a_ = ثنائی_ہندسہ;
	    إذا (select_ == SEL_PORT1) أرجع; // The other port is selected.
	    اكسر;
	  حالة 1:
	    إذا (b_ .eeq(ثنائی_ہندسہ) && has_لگاتار) أرجع;
	    b_ = ثنائی_ہندسہ;
	    إذا (select_ == SEL_PORT0) أرجع; // The other port is selected.
	    اكسر;
	  حالة 2:
	    أكد(ثنائی_ہندسہ.size() == 1);
	    بدل (ثنائی_ہندسہ.value(0)) {
		حالة BIT4_0:
		  إذا (select_ == SEL_PORT0) أرجع;
		  select_ = SEL_PORT0;
		  اكسر;
		حالة BIT4_1:
		  إذا (select_ == SEL_PORT1) أرجع;
		  select_ = SEL_PORT1;
		  اكسر;
		افتراضي:
		  إذا (select_ == SEL_BOTH && has_لگاتار) أرجع;
		  select_ = SEL_BOTH;
	    }
	    اكسر;
	  افتراضي:
	    أرجع;
      }

      إذا (net_ == 0) {
	    net_ = ptr.ptr();
	    ویلا_سارنی_functor(هذا);
      }
}

فراغ vvp_fun_muxz::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				بنانشانی ادھار, بنانشانی vwid, vvp_context_t)
{
      أكد(ادھار + ثنائی_ہندسہ.size() <= vwid);
      منطقي flag;

      بدل (ptr.پورٹ()) {
	  حالة 0:
	    flag = a_.سیٹ_vec(ادھار, ثنائی_ہندسہ);
	    إذا (flag == خطأ && has_لگاتار) أرجع;
	    إذا (select_ == SEL_PORT1) أرجع; // The other port is selected.
	    اكسر;
	  حالة 1:
	    flag = b_.سیٹ_vec(ادھار, ثنائی_ہندسہ);
	    إذا (flag == خطأ && has_لگاتار) أرجع;
	    إذا (select_ == SEL_PORT0) أرجع; // The other port is selected.
	    اكسر;
	  حالة 2:
	    أكد((ادھار == 0) && (ثنائی_ہندسہ.size() == 1));
	    recv_vec4(ptr, ثنائی_ہندسہ, 0);
	  افتراضي:
	    أرجع;
      }
      إذا (net_ == 0) {
	    net_ = ptr.ptr();
	    ویلا_سارنی_functor(هذا);
      }
}

فراغ vvp_fun_muxz::لگاتار_لگاتار()
{
      has_لگاتار = سچ;
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
		    بنانشانی min_ناپ = a_.size();
		    بنانشانی max_ناپ = min_ناپ;
		    إذا (b_.size() < min_ناپ)
			  min_ناپ = b_.size();
		    إذا (b_.size() > max_ناپ)
			  max_ناپ = b_.size();

		    vvp_vector4_t res (max_ناپ);

		    لكل (بنانشانی idx = 0 ;  idx < min_ناپ ;  idx += 1) {
			  إذا (a_.value(idx) == b_.value(idx))
				res.سیٹ_ثنائی_ہندسہ(idx, a_.value(idx));
			  وإلا
				res.سیٹ_ثنائی_ہندسہ(idx, BIT4_X);
		    }

		    لكل (بنانشانی idx = min_ناپ ;  idx < max_ناپ ;  idx += 1)
			  res.سیٹ_ثنائی_ہندسہ(idx, BIT4_X);

		    ptr->send_vec4(res, 0);
	      }
	    اكسر;
      }
}

vvp_fun_not::vvp_fun_not(بنانشانی wid)
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
فراغ vvp_fun_not::لگاتار_لگاتار()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t نتیجہ (داخلہ_2, سچ /* invert */);
      ptr->send_vec4(نتیجہ, 0);
}

vvp_fun_or::vvp_fun_or(بنانشانی wid, منطقي invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      گنتی_functors_logic += 1;
}

vvp_fun_or::~vvp_fun_or()
{
}

فراغ vvp_fun_or::لگاتار_لگاتار()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t نتیجہ (داخلہ_2[0]);

      لكل (بنانشانی pdx = 1 ; pdx < 4 ; pdx += 1)
	    نتیجہ |= داخلہ_2[pdx];

      إذا (invert_)
	    نتیجہ.invert();

      ptr->send_vec4(نتیجہ, 0);
}

vvp_fun_xor::vvp_fun_xor(بنانشانی wid, منطقي invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      گنتی_functors_logic += 1;
}

vvp_fun_xor::~vvp_fun_xor()
{
}

فراغ vvp_fun_xor::لگاتار_لگاتار()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t نتیجہ (داخلہ_2[0]);

      لكل (بنانشانی pdx = 1 ; pdx < 4 ; pdx += 1)
	    نتیجہ ^= داخلہ_2[pdx];

      إذا (invert_)
	    نتیجہ.invert();

      ptr->send_vec4(نتیجہ, 0);
}

/*
 * The parser calls this function to create a logic functor. I allocate a
 * functor, and map the name to the vvp_ipoint_t address for the
 * functor. Also resolve the inputs to the functor.
 */

فراغ compile_functor(حرف*label, حرف*type, بنانشانی width,
		     بنانشانی ostr0, بنانشانی ostr1,
		     بنانشانی argc, بنية symb_s*argv)
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
	    obj = جديد vvp_fun_bufif(سچ,خطأ, ostr0, ostr1);
	    strength_aware = سچ;

      } وإلا إذا (strcmp(type, "BUFIF1") == 0) {
	    obj = جديد vvp_fun_bufif(خطأ,خطأ, ostr0, ostr1);
	    strength_aware = سچ;

      } وإلا إذا (strcmp(type, "EQUIV") == 0) {
	    obj = جديد vvp_fun_equiv();

      } وإلا إذا (strcmp(type, "IMPL") == 0) {
	    obj = جديد vvp_fun_impl();

      } وإلا إذا (strcmp(type, "NAND") == 0) {
	    obj = جديد vvp_fun_and(width, سچ);

      } وإلا إذا (strcmp(type, "NOR") == 0) {
	    obj = جديد vvp_fun_or(width, سچ);

      } وإلا إذا (strcmp(type, "NOTIF0") == 0) {
	    obj = جديد vvp_fun_bufif(سچ,سچ, ostr0, ostr1);
	    strength_aware = سچ;

      } وإلا إذا (strcmp(type, "NOTIF1") == 0) {
	    obj = جديد vvp_fun_bufif(خطأ,سچ, ostr0, ostr1);
	    strength_aware = سچ;

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
	    obj = جديد vvp_fun_pmos(سچ);

      } وإلا إذا (strcmp(type, "PMOS") == 0) {
	    obj = جديد vvp_fun_pmos(خطأ);

      } وإلا إذا (strcmp(type, "RCMOS") == 0) {
	    obj = جديد vvp_fun_rcmos();

      } وإلا إذا (strcmp(type, "RNMOS") == 0) {
	    obj = جديد vvp_fun_rpmos(سچ);

      } وإلا إذا (strcmp(type, "RPMOS") == 0) {
	    obj = جديد vvp_fun_rpmos(خطأ);

      } وإلا إذا (strcmp(type, "NOT") == 0) {
	    obj = جديد vvp_fun_not(width);

      } وإلا إذا (strcmp(type, "XNOR") == 0) {
	    obj = جديد vvp_fun_xor(width, سچ);

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
	    define_functor_علامت(label, net);
	    حرر(label);
	    أرجع;
      }

      vvp_net_t*net_drv = جديد vvp_net_t;
      vvp_net_fun_t*obj_drv = جديد vvp_fun_drive(ostr0, ostr1);
      net_drv->fun = obj_drv;

	/* Point the gate to the drive node. */
      net->link(vvp_net_ptr_t(net_drv, 0));

      define_functor_علامت(label, net_drv);
      حرر(label);
}
