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


स्थिर ध्रुव __vpiBit*bit_from_handle(ध्रुव __vpiHandle*ref)
{
      सचे (ref == रिक्त) पटिनिवत्त रिक्त;
      ध्रुव __vpiBit::as_bit_t*ptr = dynamic_cast<ध्रुव __vpiBit::as_bit_t*> (ref);
      सचे (ptr == रिक्त) पटिनिवत्त रिक्त;
      पटिनिवत्त reinterpret_cast<ध्रुव संरचना __vpiBit*>(ref);
}

स्थिर __vpiBit*bit_from_handle(__vpiHandle*ref)
{
      सचे (ref == रिक्त) पटिनिवत्त रिक्त;
      ध्रुव __vpiBit::as_bit_t*ptr = dynamic_cast<__vpiBit::as_bit_t*> (ref);
      सचे (ptr == रिक्त) पटिनिवत्त रिक्त;
      पटिनिवत्त reinterpret_cast<संरचना __vpiBit*>(ref);
}

स्थिर पूर्णाङ्क bit_get_type(ध्रुव __vpiBit*rfp)
{
      प्रतिज्ञा(rfp);

      ध्रुव संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      विकल्प (parent->get_type_code()) {
	पक्ष vpiNet:
	    पटिनिवत्त vpiNetBit;
	पक्ष vpiReg:
	    पटिनिवत्त vpiRegBit;
      }
      प्रतिज्ञा(0);
      पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क bit_get(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiBit*rfp = bit_from_handle(ref);
      प्रतिज्ञा(rfp);

      संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      विकल्प (code) {
	पक्ष vpiArray:  // A bit is not an array
	    पटिनिवत्त 0;

	पक्ष vpiAutomatic:
	    पटिनिवत्त vpi_get(vpiAutomatic, parent);

	पक्ष vpiIndex:
	    {
		  s_vpi_मानम् vp;
		  vp.format = vpiIntVal;
		  vpi_get_मानम्(rfp->index, &vp);
		  पटिनिवत्त vp.value.integer;
	    }

	पक्ष vpi_पङ्क्तिः_No:
	    पटिनिवत्त vpi_get(vpi_पङ्क्तिः_No, parent);

	पक्ष vpiScalar:  // A bit is a scalar
	    पटिनिवत्त 1;

	पक्ष vpiSigned:  // A bit is unsigned
	    पटिनिवत्त 0;

	पक्ष vpiSize:  // A bit has a width of 1
	    पटिनिवत्त 1;

	पक्ष vpiVector:  // A bit is not a vector
	    पटिनिवत्त 0;

	स्वभाव:
	    fprintf(stderr, "VPI error: unknown bit_get property %d.\n",
	            code);
	    पटिनिवत्त vpiUndefined;
      }
}

स्थिर वर्ण* bit_get_str(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiBit*rfp = bit_from_handle(ref);
      प्रतिज्ञा(rfp);

      सचे (code == vpiFile) {  // Not implemented for now!
            पटिनिवत्त simple_set_rbuf_str(file_names[0]);
      }

      सचे ((code != vpiName) && (code != vpiFullName)) पटिनिवत्त रिक्त;

      संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      वर्ण *nm;
      ध्रुव वर्ण *ixs;
      nm = strdup(vpi_get_str(vpiName, parent));
      s_vpi_मानम् vp;
      vp.format = vpiDecStrVal;
      vpi_get_मानम्(rfp->index, &vp);
      ixs = vp.value.str;

      वर्ण *rbuf = generic_get_str(code, vpip_scope(parent), nm, ixs);
      मुक्त(nm);
      पटिनिवत्त rbuf;
}

स्थिर vpiHandle bit_get_handle(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiBit*rfp = bit_from_handle(ref);
      प्रतिज्ञा(rfp);

      संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      विकल्प (code) {
	पक्ष vpiIndex:
	    पटिनिवत्त rfp->index;

	पक्ष vpiParent:
	    पटिनिवत्त parent;

	पक्ष vpiScope:
	    पटिनिवत्त vpi_handle(vpiScope, parent);

	पक्ष vpiModule:
	    पटिनिवत्त vpi_handle(vpiModule, parent);
      }

    पटिनिवत्त 0;
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

स्थिर शून्य bit_get_मानम्(vpiHandle ref, s_vpi_मानम्*vp)
{
      संरचना __vpiBit*rfp = bit_from_handle(ref);
      प्रतिज्ञा(rfp);

      संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      parent->get_bit_मानम्(rfp, vp);
}

स्थिर vpiHandle bit_put_मानम्(vpiHandle ref, s_vpi_मानम्*vp, पूर्णाङ्क flags)
{
      संरचना __vpiBit*rfp = bit_from_handle(ref);
      प्रतिज्ञा(rfp);

      संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      पटिनिवत्त parent->put_bit_मानम्(rfp, vp, flags);
}


पूर्णाङ्क __vpiBit::get_index(शून्य) ध्रुव
{
      s_vpi_मानम् vp;
      vp.format = vpiIntVal;
      vpi_get_मानम्(index, &vp);
      पटिनिवत्त vp.value.integer;
}

पूर्णाङ्क __vpiBit::as_bit_t::get_type_code(शून्य) ध्रुव
{ पटिनिवत्त bit_get_type(bit_from_handle(एतत्)); }

पूर्णाङ्क __vpiBit::as_bit_t::vpi_get(पूर्णाङ्क code)
{ पटिनिवत्त bit_get(code, एतत्); }

वर्ण* __vpiBit::as_bit_t::vpi_get_str(पूर्णाङ्क code)
{ पटिनिवत्त bit_get_str(code, एतत्); }

शून्य __vpiBit::as_bit_t::vpi_get_मानम्(p_vpi_मानम् val)
{ bit_get_मानम्(एतत्, val); }

vpiHandle __vpiBit::as_bit_t::vpi_put_मानम्(p_vpi_मानम् val, पूर्णाङ्क flags)
{ पटिनिवत्त bit_put_मानम्(एतत्, val, flags); }

vpiHandle __vpiBit::as_bit_t::vpi_handle(पूर्णाङ्क code)
{ पटिनिवत्त bit_get_handle(code, एतत्); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
