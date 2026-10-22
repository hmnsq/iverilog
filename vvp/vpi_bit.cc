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
      यदि (ref == रिक्त) प्रत्यागम रिक्त;
      ध्रुव __vpiBit::as_bit_t*ptr = dynamic_cast<ध्रुव __vpiBit::as_bit_t*> (ref);
      यदि (ptr == रिक्त) प्रत्यागम रिक्त;
      प्रत्यागम reinterpret_cast<ध्रुव संरचना __vpiBit*>(ref);
}

स्थिर __vpiBit*bit_from_handle(__vpiHandle*ref)
{
      यदि (ref == रिक्त) प्रत्यागम रिक्त;
      ध्रुव __vpiBit::as_bit_t*ptr = dynamic_cast<__vpiBit::as_bit_t*> (ref);
      यदि (ptr == रिक्त) प्रत्यागम रिक्त;
      प्रत्यागम reinterpret_cast<संरचना __vpiBit*>(ref);
}

स्थिर पूर्णाङ्क bit_get_type(ध्रुव __vpiBit*rfp)
{
      प्रतिज्ञा(rfp);

      ध्रुव संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      विकल्प (parent->get_type_संहिता()) {
	पक्ष vpiNet:
	    प्रत्यागम vpiNetBit;
	पक्ष vpiReg:
	    प्रत्यागम vpiRegBit;
      }
      प्रतिज्ञा(0);
      प्रत्यागम 0;
}

स्थिर पूर्णाङ्क bit_get(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiBit*rfp = bit_from_handle(ref);
      प्रतिज्ञा(rfp);

      संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      विकल्प (code) {
	पक्ष vpiArray:  // A bit is not an array
	    प्रत्यागम 0;

	पक्ष vpiAutomatic:
	    प्रत्यागम vpi_get(vpiAutomatic, parent);

	पक्ष vpi_सूचकांक_2:
	    {
		  s_vpi_मानम् vp;
		  vp.format = vpiIntVal;
		  vpi_get_मानम्(rfp->सूचकांक, &vp);
		  प्रत्यागम vp.value.integer;
	    }

	पक्ष vpi_पङ्क्तिः_No:
	    प्रत्यागम vpi_get(vpi_पङ्क्तिः_No, parent);

	पक्ष vpiScalar:  // A bit is a scalar
	    प्रत्यागम 1;

	पक्ष vpiSigned:  // A bit is unsigned
	    प्रत्यागम 0;

	पक्ष vpi_आकार:  // A bit has a width of 1
	    प्रत्यागम 1;

	पक्ष vpi_सदिशम्:  // A bit is not a vector
	    प्रत्यागम 0;

	स्वभाव:
	    fprintf(stderr, "VPI error: unknown bit_get property %d.\n",
	            code);
	    प्रत्यागम vpiUndefined;
      }
}

स्थिर वर्ण* bit_get_str(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiBit*rfp = bit_from_handle(ref);
      प्रतिज्ञा(rfp);

      यदि (code == vpiFile) {  // Not implemented for now!
            प्रत्यागम simple_समुच्चयः_rbuf_str(file_नामानि[0]);
      }

      यदि ((code != vpiName) && (code != vpi_पूर्णम्_Name)) प्रत्यागम रिक्त;

      संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      वर्ण *nm;
      ध्रुव वर्ण *ixs;
      nm = strdup(vpi_get_str(vpiName, parent));
      s_vpi_मानम् vp;
      vp.format = vpiDecStrVal;
      vpi_get_मानम्(rfp->सूचकांक, &vp);
      ixs = vp.value.str;

      वर्ण *rbuf = generic_get_str(code, vpip_scope(parent), nm, ixs);
      मुक्त(nm);
      प्रत्यागम rbuf;
}

स्थिर vpiHandle bit_get_handle(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiBit*rfp = bit_from_handle(ref);
      प्रतिज्ञा(rfp);

      संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      विकल्प (code) {
	पक्ष vpi_सूचकांक_2:
	    प्रत्यागम rfp->सूचकांक;

	पक्ष vpiParent:
	    प्रत्यागम parent;

	पक्ष vpiScope:
	    प्रत्यागम vpi_handle(vpiScope, parent);

	पक्ष vpiModule:
	    प्रत्यागम vpi_handle(vpiModule, parent);
      }

    प्रत्यागम 0;
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

      प्रत्यागम parent->put_bit_मानम्(rfp, vp, flags);
}


पूर्णाङ्क __vpiBit::get_सूचकांक(शून्य) ध्रुव
{
      s_vpi_मानम् vp;
      vp.format = vpiIntVal;
      vpi_get_मानम्(सूचकांक, &vp);
      प्रत्यागम vp.value.integer;
}

पूर्णाङ्क __vpiBit::as_bit_t::get_type_संहिता(शून्य) ध्रुव
{ प्रत्यागम bit_get_type(bit_from_handle(एतत्)); }

पूर्णाङ्क __vpiBit::as_bit_t::vpi_get(पूर्णाङ्क code)
{ प्रत्यागम bit_get(code, एतत्); }

वर्ण* __vpiBit::as_bit_t::vpi_get_str(पूर्णाङ्क code)
{ प्रत्यागम bit_get_str(code, एतत्); }

शून्य __vpiBit::as_bit_t::vpi_get_मानम्(p_vpi_मानम् val)
{ bit_get_मानम्(एतत्, val); }

vpiHandle __vpiBit::as_bit_t::vpi_put_मानम्(p_vpi_मानम् val, पूर्णाङ्क flags)
{ प्रत्यागम bit_put_मानम्(एतत्, val, flags); }

vpiHandle __vpiBit::as_bit_t::vpi_handle(पूर्णाङ्क code)
{ प्रत्यागम bit_get_handle(code, एतत्); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
