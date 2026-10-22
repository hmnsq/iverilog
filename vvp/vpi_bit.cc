/*
 * Copyright (C) 2020-2026 Cary R. (cygcary@yahoo.com)
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

/*
 * vpiNetBit and vpiRegBit are handled here along with some help from their
 * parent singal or net.
 */

# include "compile.h"
# include "vpi_priv.h"
# include <cassert>


ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ __vpiBit*ಬಿಟ್_from_handle(ಸ್ಥಿರಾಂಕ __vpiHandle*ref)
{
      ಆದರೆ (ref == ಖಾಲಿ) ಹಿಂತಿರುಗಿ ಖಾಲಿ;
      ಸ್ಥಿರಾಂಕ __vpiBit::as_ಬಿಟ್_t*ptr = dynamic_cast<ಸ್ಥಿರಾಂಕ __vpiBit::as_ಬಿಟ್_t*> (ref);
      ಆದರೆ (ptr == ಖಾಲಿ) ಹಿಂತಿರುಗಿ ಖಾಲಿ;
      ಹಿಂತಿರುಗಿ reinterpret_cast<ಸ್ಥಿರಾಂಕ ರಚನೆ __vpiBit*>(ref);
}

ಸ್ಥಿರ __vpiBit*ಬಿಟ್_from_handle(__vpiHandle*ref)
{
      ಆದರೆ (ref == ಖಾಲಿ) ಹಿಂತಿರುಗಿ ಖಾಲಿ;
      ಸ್ಥಿರಾಂಕ __vpiBit::as_ಬಿಟ್_t*ptr = dynamic_cast<__vpiBit::as_ಬಿಟ್_t*> (ref);
      ಆದರೆ (ptr == ಖಾಲಿ) ಹಿಂತಿರುಗಿ ಖಾಲಿ;
      ಹಿಂತಿರುಗಿ reinterpret_cast<ರಚನೆ __vpiBit*>(ref);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಬಿಟ್_get_type(ಸ್ಥಿರಾಂಕ __vpiBit*rfp)
{
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಸ್ಥಿರಾಂಕ ರಚನೆ __vpiSignal*ಪೋಷಕ = rfp->get_ಪೋಷಕ();
      ಪ್ರತಿಜ್ಞಾ(ಪೋಷಕ);

      ಆಯ್ಕೆ (ಪೋಷಕ->get_type_ಸಂಕೇತ()) {
	ಸ್ಥಿತಿ vpiNet:
	    ಹಿಂತಿರುಗಿ vpi_Net_ಬಿಟ್;
	ಸ್ಥಿತಿ vpiReg:
	    ಹಿಂತಿರುಗಿ vpi_Reg_ಬಿಟ್;
      }
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಬಿಟ್_get(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ರಚನೆ __vpiBit*rfp = ಬಿಟ್_from_handle(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ರಚನೆ __vpiSignal*ಪೋಷಕ = rfp->get_ಪೋಷಕ();
      ಪ್ರತಿಜ್ಞಾ(ಪೋಷಕ);

      ಆಯ್ಕೆ (code) {
	ಸ್ಥಿತಿ vpiArray:  // A bit is not an array
	    ಹಿಂತಿರುಗಿ 0;

	ಸ್ಥಿತಿ vpiAutomatic:
	    ಹಿಂತಿರುಗಿ vpi_get(vpiAutomatic, ಪೋಷಕ);

	ಸ್ಥಿತಿ vpi_ಸೂಚ್ಯಂಕ_2:
	    {
		  s_vpi_ಮೌಲ್ಯ vp;
		  vp.format = vpiIntVal;
		  vpi_get_ಮೌಲ್ಯ(rfp->ಸೂಚ್ಯಂಕ, &vp);
		  ಹಿಂತಿರುಗಿ vp.value.integer;
	    }

	ಸ್ಥಿತಿ vpi_ಸಾಲು_No:
	    ಹಿಂತಿರುಗಿ vpi_get(vpi_ಸಾಲು_No, ಪೋಷಕ);

	ಸ್ಥಿತಿ vpiScalar:  // A bit is a scalar
	    ಹಿಂತಿರುಗಿ 1;

	ಸ್ಥಿತಿ vpiSigned:  // A bit is unsigned
	    ಹಿಂತಿರುಗಿ 0;

	ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:  // A bit has a width of 1
	    ಹಿಂತಿರುಗಿ 1;

	ಸ್ಥಿತಿ vpi_ಸದಿಶ:  // A bit is not a vector
	    ಹಿಂತಿರುಗಿ 0;

	ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "VPI error: unknown bit_get property %d.\n",
	            code);
	    ಹಿಂತಿರುಗಿ vpiUndefined;
      }
}

ಸ್ಥಿರ ಅಕ್ಷರ* ಬಿಟ್_get_str(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ರಚನೆ __vpiBit*rfp = ಬಿಟ್_from_handle(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಆದರೆ (code == vpiFile) {  // Not implemented for now!
            ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str(file_ಹೆಸರುಗಳು[0]);
      }

      ಆದರೆ ((code != vpiName) && (code != vpi_ತುಂಬಿದ_Name)) ಹಿಂತಿರುಗಿ ಖಾಲಿ;

      ರಚನೆ __vpiSignal*ಪೋಷಕ = rfp->get_ಪೋಷಕ();
      ಪ್ರತಿಜ್ಞಾ(ಪೋಷಕ);

      ಅಕ್ಷರ *nm;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ixs;
      nm = strdup(vpi_get_str(vpiName, ಪೋಷಕ));
      s_vpi_ಮೌಲ್ಯ vp;
      vp.format = vpiDecStrVal;
      vpi_get_ಮೌಲ್ಯ(rfp->ಸೂಚ್ಯಂಕ, &vp);
      ixs = vp.value.str;

      ಅಕ್ಷರ *rbuf = generic_get_str(code, vpip_scope(ಪೋಷಕ), nm, ixs);
      ಬಿಡುಗಡೆ(nm);
      ಹಿಂತಿರುಗಿ rbuf;
}

ಸ್ಥಿರ vpiHandle ಬಿಟ್_get_handle(ಪೂರ್ಣಾಂಕ code, vpiHandle ref)
{
      ರಚನೆ __vpiBit*rfp = ಬಿಟ್_from_handle(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ರಚನೆ __vpiSignal*ಪೋಷಕ = rfp->get_ಪೋಷಕ();
      ಪ್ರತಿಜ್ಞಾ(ಪೋಷಕ);

      ಆಯ್ಕೆ (code) {
	ಸ್ಥಿತಿ vpi_ಸೂಚ್ಯಂಕ_2:
	    ಹಿಂತಿರುಗಿ rfp->ಸೂಚ್ಯಂಕ;

	ಸ್ಥಿತಿ vpi_ಪೋಷಕ:
	    ಹಿಂತಿರುಗಿ ಪೋಷಕ;

	ಸ್ಥಿತಿ vpiScope:
	    ಹಿಂತಿರುಗಿ vpi_handle(vpiScope, ಪೋಷಕ);

	ಸ್ಥಿತಿ vpiModule:
	    ಹಿಂತಿರುಗಿ vpi_handle(vpiModule, ಪೋಷಕ);
      }

    ಹಿಂತಿರುಗಿ 0;
}

/*
static vpiHandle bit_iterate(int code, vpiHandle ref)
{
      struct __vpiBit*rfp = bit_from_handle(ref);
      assert(rfp);
      (void) code;

      return 0;
}

static vpiHandle bit_index(int idx, vpiHandle ref)
{
      struct __vpiBit*rfp = bit_from_handle(ref);
      assert(rfp);
      (void) idx;

      return 0;
}
*/

ಸ್ಥಿರ ಶೂನ್ಯ ಬಿಟ್_get_ಮೌಲ್ಯ(vpiHandle ref, s_vpi_ಮೌಲ್ಯ*vp)
{
      ರಚನೆ __vpiBit*rfp = ಬಿಟ್_from_handle(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ರಚನೆ __vpiSignal*ಪೋಷಕ = rfp->get_ಪೋಷಕ();
      ಪ್ರತಿಜ್ಞಾ(ಪೋಷಕ);

      ಪೋಷಕ->get_ಬಿಟ್_ಮೌಲ್ಯ(rfp, vp);
}

ಸ್ಥಿರ vpiHandle ಬಿಟ್_put_ಮೌಲ್ಯ(vpiHandle ref, s_vpi_ಮೌಲ್ಯ*vp, ಪೂರ್ಣಾಂಕ flags)
{
      ರಚನೆ __vpiBit*rfp = ಬಿಟ್_from_handle(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ರಚನೆ __vpiSignal*ಪೋಷಕ = rfp->get_ಪೋಷಕ();
      ಪ್ರತಿಜ್ಞಾ(ಪೋಷಕ);

      ಹಿಂತಿರುಗಿ ಪೋಷಕ->put_ಬಿಟ್_ಮೌಲ್ಯ(rfp, vp, flags);
}


ಪೂರ್ಣಾಂಕ __vpiBit::get_ಸೂಚ್ಯಂಕ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{
      s_vpi_ಮೌಲ್ಯ vp;
      vp.format = vpiIntVal;
      vpi_get_ಮೌಲ್ಯ(ಸೂಚ್ಯಂಕ, &vp);
      ಹಿಂತಿರುಗಿ vp.value.integer;
}

ಪೂರ್ಣಾಂಕ __vpiBit::as_ಬಿಟ್_t::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ ಬಿಟ್_get_type(ಬಿಟ್_from_handle(ಇದು)); }

ಪೂರ್ಣಾಂಕ __vpiBit::as_ಬಿಟ್_t::vpi_get(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ ಬಿಟ್_get(code, ಇದು); }

ಅಕ್ಷರ* __vpiBit::as_ಬಿಟ್_t::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ ಬಿಟ್_get_str(code, ಇದು); }

ಶೂನ್ಯ __vpiBit::as_ಬಿಟ್_t::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ ಬಿಟ್_get_ಮೌಲ್ಯ(ಇದು, val); }

vpiHandle __vpiBit::as_ಬಿಟ್_t::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags)
{ ಹಿಂತಿರುಗಿ ಬಿಟ್_put_ಮೌಲ್ಯ(ಇದು, val, flags); }

vpiHandle __vpiBit::as_ಬಿಟ್_t::vpi_handle(ಪೂರ್ಣಾಂಕ code)
{ ಹಿಂತಿರುಗಿ ಬಿಟ್_get_handle(code, ಇದು); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
