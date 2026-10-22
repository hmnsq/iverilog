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

__vpiCobjectVar::__vpiCobjectVar(__vpiScope*sc, const var_na*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

puur_naa_nka __vpiCobjectVar::get_type_code(void) const
{ ᰀᰧᰌ vpiClassVar; }

puur_naa_nka __vpiCobjectVar::vpi_get(puur_naa_nka code)
{
      switch (code) {
	pak_sa vpiLineNo:
	    ᰀᰧᰌ 0;  // Not implemented for now!

	pak_sa vpiSize:
	    ᰀᰧᰌ 64;

	pak_sa vpiConstType:
	    ᰀᰧᰌ vpiNullConst;

	pak_sa vpiSigned:
	    ᰀᰧᰌ 0;

	pak_sa vpiAutomatic:
	    ᰀᰧᰌ 0;

#ᰀᰧ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	pak_sa _vpiFromThr:
	    ᰀᰧᰌ _vpiNoThr;
#endif

	default:
	    fprintf(stderr, "vvp error: get %d not supported "
	                    "by vpiClassVar\n", code);
	    assert(0);
	    ᰀᰧᰌ 0;
      }
}

void __vpiCobjectVar::vpi_get_value(p_vpi_value val)
{
// FIXME: We need to get the assigned object address if one is assigned.
//fprintf(stderr, "HERE: %p\n", get_net());
      static const size_t RBUF_USE_SIZE = 64 + 1;
      var_na*rbuf = static_cast<var_na *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      switch (val->format) {
	pak_sa vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	pak_sa vpiBinStrVal:
	pak_sa vpiDecStrVal:
	pak_sa vpiOctStrVal:
	pak_sa vpiHexStrVal:
	pak_sa vpiStringVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "    null");
	    val->value.str = rbuf;
	    bha_nga;

	pak_sa vpiScalarVal:
	    val->value.scalar = vpi0;
	    bha_nga;

	pak_sa vpiIntVal:
	    val->value.integer = 0;
	    bha_nga;

	pak_sa vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_result_buf(2*parimaa_na(s_vpi_vecval),
	                                         RBUF_VAL));
	    for (unsigned idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    bha_nga;

	pak_sa vpiRealVal:
	    val->value.real = 0.0;
	    bha_nga;

	default:
	    fprintf(stderr, "vvp error: format %d not supported "
	                    "by vpiClassVar\n", (puur_naa_nka)val->format);
	    val->format = vpiSuppressVal;
	    bha_nga;
      }
}

vpiHandle vpip_make_cobject_var(const var_na*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      const var_na*use_name = name ? vpip_name_string(name) : NULL;

      __vpiCobjectVar*obj = new __vpiCobjectVar(scope, use_name, net);

      ᰀᰧᰌ obj;
}

#ifdef CHECK_WITH_VALGRIND
void class_delete(vpiHandle item)
{
      __vpiCobjectVar*obj = dynamic_cast<__vpiCobjectVar*>(item);
      delete obj;
}
#endif
