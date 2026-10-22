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


ಸ್ಥಿರ ಧ್ರುವ __vpiBit*bit_from_handle(ಧ್ರುವ __vpiHandle*ref)
{
      ಯದಿ (ref == ರಿಕ್ತ) ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
      ಧ್ರುವ __vpiBit::as_bit_t*ptr = dynamic_cast<ಧ್ರುವ __vpiBit::as_bit_t*> (ref);
      ಯದಿ (ptr == ರಿಕ್ತ) ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
      ಪ್ರತ್ಯಾಗಮ reinterpret_cast<ಧ್ರುವ ಸಂರಚನಾ __vpiBit*>(ref);
}

ಸ್ಥಿರ __vpiBit*bit_from_handle(__vpiHandle*ref)
{
      ಯದಿ (ref == ರಿಕ್ತ) ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
      ಧ್ರುವ __vpiBit::as_bit_t*ptr = dynamic_cast<__vpiBit::as_bit_t*> (ref);
      ಯದಿ (ptr == ರಿಕ್ತ) ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
      ಪ್ರತ್ಯಾಗಮ reinterpret_cast<ಸಂರಚನಾ __vpiBit*>(ref);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ bit_get_type(ಧ್ರುವ __vpiBit*rfp)
{
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಧ್ರುವ ಸಂರಚನಾ __vpiSignal*parent = rfp->get_parent();
      ಪ್ರತಿಜ್ಞಾ(parent);

      ವಿಕಲ್ಪ (parent->get_type_code()) {
	ಪಕ್ಷ vpiNet:
	    ಪ್ರತ್ಯಾಗಮ vpiNetBit;
	ಪಕ್ಷ vpiReg:
	    ಪ್ರತ್ಯಾಗಮ vpiRegBit;
      }
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ bit_get(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiBit*rfp = bit_from_handle(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಸಂರಚನಾ __vpiSignal*parent = rfp->get_parent();
      ಪ್ರತಿಜ್ಞಾ(parent);

      ವಿಕಲ್ಪ (code) {
	ಪಕ್ಷ vpiArray:  // A bit is not an array
	    ಪ್ರತ್ಯಾಗಮ 0;

	ಪಕ್ಷ vpiAutomatic:
	    ಪ್ರತ್ಯಾಗಮ vpi_get(vpiAutomatic, parent);

	ಪಕ್ಷ vpiIndex:
	    {
		  s_vpi_ಮೌಲ್ಯ vp;
		  vp.format = vpiIntVal;
		  vpi_get_ಮೌಲ್ಯ(rfp->index, &vp);
		  ಪ್ರತ್ಯಾಗಮ vp.value.integer;
	    }

	ಪಕ್ಷ vpi_ಸಾಲು_No:
	    ಪ್ರತ್ಯಾಗಮ vpi_get(vpi_ಸಾಲು_No, parent);

	ಪಕ್ಷ vpiScalar:  // A bit is a scalar
	    ಪ್ರತ್ಯಾಗಮ 1;

	ಪಕ್ಷ vpiSigned:  // A bit is unsigned
	    ಪ್ರತ್ಯಾಗಮ 0;

	ಪಕ್ಷ vpiSize:  // A bit has a width of 1
	    ಪ್ರತ್ಯಾಗಮ 1;

	ಪಕ್ಷ vpiVector:  // A bit is not a vector
	    ಪ್ರತ್ಯಾಗಮ 0;

	ಸ್ವಭಾವ:
	    fprintf(stderr, "VPI error: unknown bit_get property %d.\n",
	            code);
	    ಪ್ರತ್ಯಾಗಮ vpiUndefined;
      }
}

ಸ್ಥಿರ ವರ್ಣ* bit_get_str(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiBit*rfp = bit_from_handle(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಯದಿ (code == vpiFile) {  // Not implemented for now!
            ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(file_names[0]);
      }

      ಯದಿ ((code != vpiName) && (code != vpiFullName)) ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

      ಸಂರಚನಾ __vpiSignal*parent = rfp->get_parent();
      ಪ್ರತಿಜ್ಞಾ(parent);

      ವರ್ಣ *nm;
      ಧ್ರುವ ವರ್ಣ *ixs;
      nm = strdup(vpi_get_str(vpiName, parent));
      s_vpi_ಮೌಲ್ಯ vp;
      vp.format = vpiDecStrVal;
      vpi_get_ಮೌಲ್ಯ(rfp->index, &vp);
      ixs = vp.value.str;

      ವರ್ಣ *rbuf = generic_get_str(code, vpip_scope(parent), nm, ixs);
      ಮುಕ್ತ(nm);
      ಪ್ರತ್ಯಾಗಮ rbuf;
}

ಸ್ಥಿರ vpiHandle bit_get_handle(ಪೂರ್ಣಾಙ್ಕ code, vpiHandle ref)
{
      ಸಂರಚನಾ __vpiBit*rfp = bit_from_handle(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಸಂರಚನಾ __vpiSignal*parent = rfp->get_parent();
      ಪ್ರತಿಜ್ಞಾ(parent);

      ವಿಕಲ್ಪ (code) {
	ಪಕ್ಷ vpiIndex:
	    ಪ್ರತ್ಯಾಗಮ rfp->index;

	ಪಕ್ಷ vpiParent:
	    ಪ್ರತ್ಯಾಗಮ parent;

	ಪಕ್ಷ vpiScope:
	    ಪ್ರತ್ಯಾಗಮ vpi_handle(vpiScope, parent);

	ಪಕ್ಷ vpiModule:
	    ಪ್ರತ್ಯಾಗಮ vpi_handle(vpiModule, parent);
      }

    ಪ್ರತ್ಯಾಗಮ 0;
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

ಸ್ಥಿರ ಶೂನ್ಯ bit_get_ಮೌಲ್ಯ(vpiHandle ref, s_vpi_ಮೌಲ್ಯ*vp)
{
      ಸಂರಚನಾ __vpiBit*rfp = bit_from_handle(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಸಂರಚನಾ __vpiSignal*parent = rfp->get_parent();
      ಪ್ರತಿಜ್ಞಾ(parent);

      parent->get_bit_ಮೌಲ್ಯ(rfp, vp);
}

ಸ್ಥಿರ vpiHandle bit_put_ಮೌಲ್ಯ(vpiHandle ref, s_vpi_ಮೌಲ್ಯ*vp, ಪೂರ್ಣಾಙ್ಕ flags)
{
      ಸಂರಚನಾ __vpiBit*rfp = bit_from_handle(ref);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಸಂರಚನಾ __vpiSignal*parent = rfp->get_parent();
      ಪ್ರತಿಜ್ಞಾ(parent);

      ಪ್ರತ್ಯಾಗಮ parent->put_bit_ಮೌಲ್ಯ(rfp, vp, flags);
}


ಪೂರ್ಣಾಙ್ಕ __vpiBit::get_index(ಶೂನ್ಯ) ಧ್ರುವ
{
      s_vpi_ಮೌಲ್ಯ vp;
      vp.format = vpiIntVal;
      vpi_get_ಮೌಲ್ಯ(index, &vp);
      ಪ್ರತ್ಯಾಗಮ vp.value.integer;
}

ಪೂರ್ಣಾಙ್ಕ __vpiBit::as_bit_t::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ bit_get_type(bit_from_handle(ಏತತ್)); }

ಪೂರ್ಣಾಙ್ಕ __vpiBit::as_bit_t::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ bit_get(code, ಏತತ್); }

ವರ್ಣ* __vpiBit::as_bit_t::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ bit_get_str(code, ಏತತ್); }

ಶೂನ್ಯ __vpiBit::as_bit_t::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{ bit_get_ಮೌಲ್ಯ(ಏತತ್, val); }

vpiHandle __vpiBit::as_bit_t::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags)
{ ಪ್ರತ್ಯಾಗಮ bit_put_ಮೌಲ್ಯ(ಏತತ್, val, flags); }

vpiHandle __vpiBit::as_bit_t::vpi_handle(ಪೂರ್ಣಾಙ್ಕ code)
{ ಪ್ರತ್ಯಾಗಮ bit_get_handle(code, ಏತತ್); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
