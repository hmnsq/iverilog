/*
 * Copyright (c) 2012-2025 Picture Elements, Inc.
 *    Stephen Williams (steve@icarus.com)
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

# include  "compile.h"
# include  "vpi_priv.h"
# include  "config.h"
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif

__vpiCobjectVar::__vpiCobjectVar(__vpiScope*sc, ধ্রুব বর্ণ*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

পূর্ণাঙ্ক __vpiCobjectVar::get_type_code(শূন্য) ধ্রুব
{ প্রত্যাগম vpiClassVar; }

পূর্ণাঙ্ক __vpiCobjectVar::vpi_get(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
	পক্ষ vpi_সারি_No:
	    প্রত্যাগম 0;  // Not implemented for now!

	পক্ষ vpiSize:
	    প্রত্যাগম 64;

	পক্ষ vpiConstType:
	    প্রত্যাগম vpiNullConst;

	পক্ষ vpiSigned:
	    প্রত্যাগম 0;

	পক্ষ vpiAutomatic:
	    প্রত্যাগম 0;

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	পক্ষ _vpiFromThr:
	    প্রত্যাগম _vpiNoThr;
#endif

	স্বভাব:
	    fprintf(stderr, "vvp error: get %d not supported "
	                    "by vpiClassVar\n", code);
	    প্রতিজ্ঞা(0);
	    প্রত্যাগম 0;
      }
}

শূন্য __vpiCobjectVar::vpi_get_মান(p_vpi_মান val)
{
// FIXME: We need to get the assigned object address if one is assigned.
//fprintf(stderr, "HERE: %p\n", get_net());
      স্থির ধ্রুব size_t RBUF_USE_SIZE = 64 + 1;
      বর্ণ*rbuf = static_cast<বর্ণ *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      বিকল্প (val->format) {
	পক্ষ vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	পক্ষ vpiBinStrVal:
	পক্ষ vpiDecStrVal:
	পক্ষ vpiOctStrVal:
	পক্ষ vpiHexStrVal:
	পক্ষ vpiStringVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "    null");
	    val->value.str = rbuf;
	    ভঙ্গ;

	পক্ষ vpiScalarVal:
	    val->value.scalar = vpi0;
	    ভঙ্গ;

	পক্ষ vpiIntVal:
	    val->value.integer = 0;
	    ভঙ্গ;

	পক্ষ vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_result_buf(2*পরিমাণ(s_vpi_vecval),
	                                         RBUF_VAL));
	    প্রতি (unsigned idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    ভঙ্গ;

	পক্ষ vpiRealVal:
	    val->value.real = 0.0;
	    ভঙ্গ;

	স্বভাব:
	    fprintf(stderr, "vvp error: format %d not supported "
	                    "by vpiClassVar\n", (পূর্ণাঙ্ক)val->format);
	    val->format = vpiSuppressVal;
	    ভঙ্গ;
      }
}

vpiHandle vpip_make_cobject_var(ধ্রুব বর্ণ*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ধ্রুব বর্ণ*use_name = name ? vpip_name_string(name) : রিক্ত;

      __vpiCobjectVar*obj = নূতন __vpiCobjectVar(scope, use_name, net);

      প্রত্যাগম obj;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য class_delete(vpiHandle item)
{
      __vpiCobjectVar*obj = dynamic_cast<__vpiCobjectVar*>(item);
      delete obj;
}
#endif
