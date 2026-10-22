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

__vpiCobjectVar::__vpiCobjectVar(__vpiScope*sc, ধ্রুবক অক্ষর*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

পূর্ণসংখ্যা __vpiCobjectVar::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiClassVar; }

পূর্ণসংখ্যা __vpiCobjectVar::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	ক্ষেত্র vpi_সারি_No:
	    ফেরত 0;  // Not implemented for now!

	ক্ষেত্র vpi_আকার:
	    ফেরত 64;

	ক্ষেত্র vpiConstType:
	    ফেরত vpiNullConst;

	ক্ষেত্র vpiSigned:
	    ফেরত 0;

	ক্ষেত্র vpiAutomatic:
	    ফেরত 0;

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	ক্ষেত্র _vpiFromThr:
	    ফেরত _vpiNoThr;
#endif

	পূর্বনির্ধারিত:
	    fprintf(stderr, "vvp error: get %d not supported "
	                    "by vpiClassVar\n", code);
	    প্রতিজ্ঞা(0);
	    ফেরত 0;
      }
}

শূন্য __vpiCobjectVar::vpi_get_মান(p_vpi_মান val)
{
// FIXME: We need to get the assigned object address if one is assigned.
//fprintf(stderr, "HERE: %p\n", get_net());
      স্থির ধ্রুবক size_t RBUF_USE_আকার = 64 + 1;
      অক্ষর*rbuf = static_cast<অক্ষর *>(need_ফলাফল_buf(RBUF_USE_আকার, RBUF_VAL));

      নির্বাচন (val->format) {
	ক্ষেত্র vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	ক্ষেত্র vpiBinStrVal:
	ক্ষেত্র vpiDecStrVal:
	ক্ষেত্র vpiOctStrVal:
	ক্ষেত্র vpiHexStrVal:
	ক্ষেত্র vpiStringVal:
	    snprintf(rbuf, RBUF_USE_আকার, "    null");
	    val->value.str = rbuf;
	    বিরতি;

	ক্ষেত্র vpiScalarVal:
	    val->value.scalar = vpi0;
	    বিরতি;

	ক্ষেত্র vpiIntVal:
	    val->value.integer = 0;
	    বিরতি;

	ক্ষেত্র vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_ফলাফল_buf(2*আকার(s_vpi_vecval),
	                                         RBUF_VAL));
	    জন্য (unsigned idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    বিরতি;

	ক্ষেত্র vpiRealVal:
	    val->value.real = 0.0;
	    বিরতি;

	পূর্বনির্ধারিত:
	    fprintf(stderr, "vvp error: format %d not supported "
	                    "by vpiClassVar\n", (পূর্ণসংখ্যা)val->format);
	    val->format = vpiSuppressVal;
	    বিরতি;
      }
}

vpiHandle vpip_make_cobject_var(ধ্রুবক অক্ষর*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ধ্রুবক অক্ষর*use_name = name ? vpip_name_string(name) : ফাঁকা;

      __vpiCobjectVar*obj = নতুন __vpiCobjectVar(scope, use_name, net);

      ফেরত obj;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য class_delete(vpiHandle item)
{
      __vpiCobjectVar*obj = dynamic_cast<__vpiCobjectVar*>(item);
      delete obj;
}
#endif
