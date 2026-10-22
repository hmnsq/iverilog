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

__vpiCobjectVar::__vpiCobjectVar(__vpiScope*sc, ଅଚଳ ଅକ୍ଷର*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiCobjectVar::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiClassVar; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiCobjectVar::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	ସ୍ଥିତି vpi_ଧାଡ଼ି_No:
	    ଫେରନ୍ତୁ 0;  // Not implemented for now!

	ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ 64;

	ସ୍ଥିତି vpiConstType:
	    ଫେରନ୍ତୁ vpiNullConst;

	ସ୍ଥିତି vpiSigned:
	    ଫେରନ୍ତୁ 0;

	ସ୍ଥିତି vpiAutomatic:
	    ଫେରନ୍ତୁ 0;

#ଯଦି defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	ସ୍ଥିତି _vpiFromThr:
	    ଫେରନ୍ତୁ _vpiNoThr;
#endif

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "vvp error: get %d not supported "
	                    "by vpiClassVar\n", code);
	    ପ୍ରତିଜ୍ଞା(0);
	    ଫେରନ୍ତୁ 0;
      }
}

ଶୂନ୍ୟ __vpiCobjectVar::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val)
{
// FIXME: We need to get the assigned object address if one is assigned.
//fprintf(stderr, "HERE: %p\n", get_net());
      ସ୍ଥିର ଅଚଳ size_t RBUF_USE_SIZE = 64 + 1;
      ଅକ୍ଷର*rbuf = static_cast<ଅକ୍ଷର *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      ବାଛ (val->format) {
	ସ୍ଥିତି vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	ସ୍ଥିତି vpiBinStrVal:
	ସ୍ଥିତି vpiDecStrVal:
	ସ୍ଥିତି vpiOctStrVal:
	ସ୍ଥିତି vpiHexStrVal:
	ସ୍ଥିତି vpiStringVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "    null");
	    val->value.str = rbuf;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି vpiScalarVal:
	    val->value.scalar = vpi0;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି vpiIntVal:
	    val->value.integer = 0;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_result_buf(2*ଆକାର(s_vpi_vecval),
	                                         RBUF_VAL));
	    ପାଇଁ (unsigned idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି vpiRealVal:
	    val->value.real = 0.0;
	    ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "vvp error: format %d not supported "
	                    "by vpiClassVar\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)val->format);
	    val->format = vpiSuppressVal;
	    ଭାଙ୍ଗ;
      }
}

vpiHandle vpip_make_cobject_var(ଅଚଳ ଅକ୍ଷର*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ଅଚଳ ଅକ୍ଷର*use_name = name ? vpip_name_string(name) : ଖାଲି;

      __vpiCobjectVar*obj = ନୂତନ __vpiCobjectVar(scope, use_name, net);

      ଫେରନ୍ତୁ obj;
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ class_delete(vpiHandle item)
{
      __vpiCobjectVar*obj = dynamic_cast<__vpiCobjectVar*>(item);
      delete obj;
}
#endif
