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

__vpiCobjectVar::__vpiCobjectVar(__vpiScope*sc, ಧ್ರುವ ವರ್ಣ*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

ಪೂರ್ಣಾಙ್ಕ __vpiCobjectVar::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiClassVar; }

ಪೂರ್ಣಾಙ್ಕ __vpiCobjectVar::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	ಪಕ್ಷ vpi_ಸಾಲು_No:
	    ಪ್ರತ್ಯಾಗಮ 0;  // Not implemented for now!

	ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ 64;

	ಪಕ್ಷ vpiConstType:
	    ಪ್ರತ್ಯಾಗಮ vpiNullConst;

	ಪಕ್ಷ vpiSigned:
	    ಪ್ರತ್ಯಾಗಮ 0;

	ಪಕ್ಷ vpiAutomatic:
	    ಪ್ರತ್ಯಾಗಮ 0;

#ಯದಿ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	ಪಕ್ಷ _vpiFromThr:
	    ಪ್ರತ್ಯಾಗಮ _vpiNoThr;
#endif

	ಸ್ವಭಾವ:
	    fprintf(stderr, "vvp error: get %d not supported "
	                    "by vpiClassVar\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}

ಶೂನ್ಯ __vpiCobjectVar::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{
// FIXME: We need to get the assigned object address if one is assigned.
//fprintf(stderr, "HERE: %p\n", get_net());
      ಸ್ಥಿರ ಧ್ರುವ size_t RBUF_USE_SIZE = 64 + 1;
      ವರ್ಣ*rbuf = static_cast<ವರ್ಣ *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

      ವಿಕಲ್ಪ (val->format) {
	ಪಕ್ಷ vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	ಪಕ್ಷ vpiBinStrVal:
	ಪಕ್ಷ vpiDecStrVal:
	ಪಕ್ಷ vpiOctStrVal:
	ಪಕ್ಷ vpiHexStrVal:
	ಪಕ್ಷ vpiStringVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "    null");
	    val->value.str = rbuf;
	    ಭಙ್ಗ;

	ಪಕ್ಷ vpiScalarVal:
	    val->value.scalar = vpi0;
	    ಭಙ್ಗ;

	ಪಕ್ಷ vpiIntVal:
	    val->value.integer = 0;
	    ಭಙ್ಗ;

	ಪಕ್ಷ vpiVectorVal:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_result_buf(2*ಪರಿಮಾಣ(s_vpi_vecval),
	                                         RBUF_VAL));
	    ಪ್ರತಿ (unsigned idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ vpiRealVal:
	    val->value.real = 0.0;
	    ಭಙ್ಗ;

	ಸ್ವಭಾವ:
	    fprintf(stderr, "vvp error: format %d not supported "
	                    "by vpiClassVar\n", (ಪೂರ್ಣಾಙ್ಕ)val->format);
	    val->format = vpiSuppressVal;
	    ಭಙ್ಗ;
      }
}

vpiHandle vpip_make_cobject_var(ಧ್ರುವ ವರ್ಣ*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ಧ್ರುವ ವರ್ಣ*use_name = name ? vpip_name_string(name) : ರಿಕ್ತ;

      __vpiCobjectVar*obj = ನೂತನ __vpiCobjectVar(scope, use_name, net);

      ಪ್ರತ್ಯಾಗಮ obj;
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ class_delete(vpiHandle item)
{
      __vpiCobjectVar*obj = dynamic_cast<__vpiCobjectVar*>(item);
      delete obj;
}
#endif
