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


स्थिर स्थिरांक __vpiBit*bit_from_handle(स्थिरांक __vpiHandle*ref)
{
      जर (ref == रिक्त) परत रिक्त;
      स्थिरांक __vpiBit::as_bit_t*ptr = dynamic_cast<स्थिरांक __vpiBit::as_bit_t*> (ref);
      जर (ptr == रिक्त) परत रिक्त;
      परत reinterpret_cast<स्थिरांक संरचना __vpiBit*>(ref);
}

स्थिर __vpiBit*bit_from_handle(__vpiHandle*ref)
{
      जर (ref == रिक्त) परत रिक्त;
      स्थिरांक __vpiBit::as_bit_t*ptr = dynamic_cast<__vpiBit::as_bit_t*> (ref);
      जर (ptr == रिक्त) परत रिक्त;
      परत reinterpret_cast<संरचना __vpiBit*>(ref);
}

स्थिर पूर्णांक bit_get_type(स्थिरांक __vpiBit*rfp)
{
      प्रतिज्ञा(rfp);

      स्थिरांक संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      निवड (parent->get_type_code()) {
	स्थिती vpiNet:
	    परत vpiNetBit;
	स्थिती vpiReg:
	    परत vpiRegBit;
      }
      प्रतिज्ञा(0);
      परत 0;
}

स्थिर पूर्णांक bit_get(पूर्णांक code, vpiHandle ref)
{
      संरचना __vpiBit*rfp = bit_from_handle(ref);
      प्रतिज्ञा(rfp);

      संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      निवड (code) {
	स्थिती vpiArray:  // A bit is not an array
	    परत 0;

	स्थिती vpiAutomatic:
	    परत vpi_get(vpiAutomatic, parent);

	स्थिती vpiIndex:
	    {
		  s_vpi_मूल्य vp;
		  vp.format = vpiIntVal;
		  vpi_get_मूल्य(rfp->index, &vp);
		  परत vp.value.integer;
	    }

	स्थिती vpi_ओळ_No:
	    परत vpi_get(vpi_ओळ_No, parent);

	स्थिती vpiScalar:  // A bit is a scalar
	    परत 1;

	स्थिती vpiSigned:  // A bit is unsigned
	    परत 0;

	स्थिती vpiSize:  // A bit has a width of 1
	    परत 1;

	स्थिती vpiVector:  // A bit is not a vector
	    परत 0;

	पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "VPI error: unknown bit_get property %d.\n",
	            code);
	    परत vpiUndefined;
      }
}

स्थिर अक्षर* bit_get_str(पूर्णांक code, vpiHandle ref)
{
      संरचना __vpiBit*rfp = bit_from_handle(ref);
      प्रतिज्ञा(rfp);

      जर (code == vpiFile) {  // Not implemented for now!
            परत simple_set_rbuf_str(file_names[0]);
      }

      जर ((code != vpiName) && (code != vpiFullName)) परत रिक्त;

      संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      अक्षर *nm;
      स्थिरांक अक्षर *ixs;
      nm = strdup(vpi_get_str(vpiName, parent));
      s_vpi_मूल्य vp;
      vp.format = vpiDecStrVal;
      vpi_get_मूल्य(rfp->index, &vp);
      ixs = vp.value.str;

      अक्षर *rbuf = generic_get_str(code, vpip_scope(parent), nm, ixs);
      मुक्त(nm);
      परत rbuf;
}

स्थिर vpiHandle bit_get_handle(पूर्णांक code, vpiHandle ref)
{
      संरचना __vpiBit*rfp = bit_from_handle(ref);
      प्रतिज्ञा(rfp);

      संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      निवड (code) {
	स्थिती vpiIndex:
	    परत rfp->index;

	स्थिती vpiParent:
	    परत parent;

	स्थिती vpiScope:
	    परत vpi_handle(vpiScope, parent);

	स्थिती vpiModule:
	    परत vpi_handle(vpiModule, parent);
      }

    परत 0;
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

स्थिर शून्य bit_get_मूल्य(vpiHandle ref, s_vpi_मूल्य*vp)
{
      संरचना __vpiBit*rfp = bit_from_handle(ref);
      प्रतिज्ञा(rfp);

      संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      parent->get_bit_मूल्य(rfp, vp);
}

स्थिर vpiHandle bit_put_मूल्य(vpiHandle ref, s_vpi_मूल्य*vp, पूर्णांक flags)
{
      संरचना __vpiBit*rfp = bit_from_handle(ref);
      प्रतिज्ञा(rfp);

      संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      परत parent->put_bit_मूल्य(rfp, vp, flags);
}


पूर्णांक __vpiBit::get_index(शून्य) स्थिरांक
{
      s_vpi_मूल्य vp;
      vp.format = vpiIntVal;
      vpi_get_मूल्य(index, &vp);
      परत vp.value.integer;
}

पूर्णांक __vpiBit::as_bit_t::get_type_code(शून्य) स्थिरांक
{ परत bit_get_type(bit_from_handle(हे)); }

पूर्णांक __vpiBit::as_bit_t::vpi_get(पूर्णांक code)
{ परत bit_get(code, हे); }

अक्षर* __vpiBit::as_bit_t::vpi_get_str(पूर्णांक code)
{ परत bit_get_str(code, हे); }

शून्य __vpiBit::as_bit_t::vpi_get_मूल्य(p_vpi_मूल्य val)
{ bit_get_मूल्य(हे, val); }

vpiHandle __vpiBit::as_bit_t::vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags)
{ परत bit_put_मूल्य(हे, val, flags); }

vpiHandle __vpiBit::as_bit_t::vpi_handle(पूर्णांक code)
{ परत bit_get_handle(code, हे); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
