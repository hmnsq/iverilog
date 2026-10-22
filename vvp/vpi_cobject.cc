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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ಆದರೆ_ಅಂತ್ಯ

__vpiCobjectVar::__vpiCobjectVar(__vpiScope*sc, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

ಪೂರ್ಣಾಂಕ __vpiCobjectVar::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiClassVar; }

ಪೂರ್ಣಾಂಕ __vpiCobjectVar::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	ಸ್ಥಿತಿ vpi_ಸಾಲು_No:
	    ಹಿಂತಿರುಗಿ 0;  // Not implemented for now!

	ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ 64;

	ಸ್ಥಿತಿ vpiConstType:
	    ಹಿಂತಿರುಗಿ vpiNullConst;

	ಸ್ಥಿತಿ vpiSigned:
	    ಹಿಂತಿರುಗಿ 0;

	ಸ್ಥಿತಿ vpiAutomatic:
	    ಹಿಂತಿರುಗಿ 0;

#ಆದರೆ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	ಸ್ಥಿತಿ _vpiFromThr:
	    ಹಿಂತಿರುಗಿ _vpiNoThr;
#ಆದರೆ_ಅಂತ್ಯ

	ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vvp error: get %d not supported "
	                    "by vpiClassVar\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಹಿಂತಿರುಗಿ 0;
      }
}

ಶೂನ್ಯ __vpiCobjectVar::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{
// FIXME: We need to get the assigned object address if one is assigned.
//fprintf(stderr, "HERE: %p\n", get_net());
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ size_t RBUF_USE_ಗಾತ್ರ = 64 + 1;
      ಅಕ್ಷರ*rbuf = static_cast<ಅಕ್ಷರ *>(need_ಫಲಿತಾಂಶ_buf(RBUF_USE_ಗಾತ್ರ, RBUF_VAL));

      ಆಯ್ಕೆ (val->format) {
	ಸ್ಥಿತಿ vpiObjTypeVal:
	    val->format = vpiStringVal;
	    // fall through
	ಸ್ಥಿತಿ vpiBinStrVal:
	ಸ್ಥಿತಿ vpiDecStrVal:
	ಸ್ಥಿತಿ vpiOctStrVal:
	ಸ್ಥಿತಿ vpiHexStrVal:
	ಸ್ಥಿತಿ vpiStringVal:
	    snprintf(rbuf, RBUF_USE_ಗಾತ್ರ, "    null");
	    val->value.str = rbuf;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ vpiScalarVal:
	    val->value.scalar = vpi0;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ vpiIntVal:
	    val->value.integer = 0;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ vpi_ಸದಿಶ_Val:
	    val->value.vector = static_cast<p_vpi_vecval>
	                        (need_ಫಲಿತಾಂಶ_buf(2*ಗಾತ್ರ(s_vpi_vecval),
	                                         RBUF_VAL));
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < 2; idx += 1) {
		  val->value.vector[idx].aval = 0;
		  val->value.vector[idx].bval = 0;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ vpiRealVal:
	    val->value.real = 0.0;
	    ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vvp error: format %d not supported "
	                    "by vpiClassVar\n", (ಪೂರ್ಣಾಂಕ)val->format);
	    val->format = vpiSuppressVal;
	    ನಿಲ್ಲಿಸು;
      }
}

vpiHandle vpip_make_cobject_var(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*use_name = name ? vpip_name_string(name) : ಖಾಲಿ;

      __vpiCobjectVar*obj = ಹೊಸ __vpiCobjectVar(scope, use_name, net);

      ಹಿಂತಿರುಗಿ obj;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ class_delete(vpiHandle ಅಂಶ)
{
      __vpiCobjectVar*obj = dynamic_cast<__vpiCobjectVar*>(ಅಂಶ);
      ಅಳಿಸು obj;
}
#ಆದರೆ_ಅಂತ್ಯ
