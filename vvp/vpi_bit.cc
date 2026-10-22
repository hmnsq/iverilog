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


स्थिर अचर __vpiBit*bit_from_handle(अचर __vpiHandle*ref)
{
      यदि (ref == रिक्त) फर्काऊ रिक्त;
      अचर __vpiBit::as_bit_t*ptr = dynamic_cast<अचर __vpiBit::as_bit_t*> (ref);
      यदि (ptr == रिक्त) फर्काऊ रिक्त;
      फर्काऊ reinterpret_cast<अचर संरचना __vpiBit*>(ref);
}

स्थिर __vpiBit*bit_from_handle(__vpiHandle*ref)
{
      यदि (ref == रिक्त) फर्काऊ रिक्त;
      अचर __vpiBit::as_bit_t*ptr = dynamic_cast<__vpiBit::as_bit_t*> (ref);
      यदि (ptr == रिक्त) फर्काऊ रिक्त;
      फर्काऊ reinterpret_cast<संरचना __vpiBit*>(ref);
}

स्थिर पूर्णाङ्क bit_get_type(अचर __vpiBit*rfp)
{
      प्रतिज्ञा(rfp);

      अचर संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      विकल्प (parent->get_type_code()) {
	पक्ष vpiNet:
	    फर्काऊ vpiNetBit;
	पक्ष vpiReg:
	    फर्काऊ vpiRegBit;
      }
      प्रतिज्ञा(0);
      फर्काऊ 0;
}

स्थिर पूर्णाङ्क bit_get(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiBit*rfp = bit_from_handle(ref);
      प्रतिज्ञा(rfp);

      संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      विकल्प (code) {
	पक्ष vpiArray:  // A bit is not an array
	    फर्काऊ 0;

	पक्ष vpiAutomatic:
	    फर्काऊ vpi_get(vpiAutomatic, parent);

	पक्ष vpiIndex:
	    {
		  s_vpi_मान vp;
		  vp.format = vpiIntVal;
		  vpi_get_मान(rfp->index, &vp);
		  फर्काऊ vp.value.integer;
	    }

	पक्ष vpi_पङ्क्ति_No:
	    फर्काऊ vpi_get(vpi_पङ्क्ति_No, parent);

	पक्ष vpiScalar:  // A bit is a scalar
	    फर्काऊ 1;

	पक्ष vpiSigned:  // A bit is unsigned
	    फर्काऊ 0;

	पक्ष vpiSize:  // A bit has a width of 1
	    फर्काऊ 1;

	पक्ष vpiVector:  // A bit is not a vector
	    फर्काऊ 0;

	स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "VPI error: unknown bit_get property %d.\n",
	            code);
	    फर्काऊ vpiUndefined;
      }
}

स्थिर वर्ण* bit_get_str(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiBit*rfp = bit_from_handle(ref);
      प्रतिज्ञा(rfp);

      यदि (code == vpiFile) {  // Not implemented for now!
            फर्काऊ simple_set_rbuf_str(file_names[0]);
      }

      यदि ((code != vpiName) && (code != vpiFullName)) फर्काऊ रिक्त;

      संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      वर्ण *nm;
      अचर वर्ण *ixs;
      nm = strdup(vpi_get_str(vpiName, parent));
      s_vpi_मान vp;
      vp.format = vpiDecStrVal;
      vpi_get_मान(rfp->index, &vp);
      ixs = vp.value.str;

      वर्ण *rbuf = generic_get_str(code, vpip_scope(parent), nm, ixs);
      मुक्त(nm);
      फर्काऊ rbuf;
}

स्थिर vpiHandle bit_get_handle(पूर्णाङ्क code, vpiHandle ref)
{
      संरचना __vpiBit*rfp = bit_from_handle(ref);
      प्रतिज्ञा(rfp);

      संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      विकल्प (code) {
	पक्ष vpiIndex:
	    फर्काऊ rfp->index;

	पक्ष vpiParent:
	    फर्काऊ parent;

	पक्ष vpiScope:
	    फर्काऊ vpi_handle(vpiScope, parent);

	पक्ष vpiModule:
	    फर्काऊ vpi_handle(vpiModule, parent);
      }

    फर्काऊ 0;
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

स्थिर शून्य bit_get_मान(vpiHandle ref, s_vpi_मान*vp)
{
      संरचना __vpiBit*rfp = bit_from_handle(ref);
      प्रतिज्ञा(rfp);

      संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      parent->get_bit_मान(rfp, vp);
}

स्थिर vpiHandle bit_put_मान(vpiHandle ref, s_vpi_मान*vp, पूर्णाङ्क flags)
{
      संरचना __vpiBit*rfp = bit_from_handle(ref);
      प्रतिज्ञा(rfp);

      संरचना __vpiSignal*parent = rfp->get_parent();
      प्रतिज्ञा(parent);

      फर्काऊ parent->put_bit_मान(rfp, vp, flags);
}


पूर्णाङ्क __vpiBit::get_index(शून्य) अचर
{
      s_vpi_मान vp;
      vp.format = vpiIntVal;
      vpi_get_मान(index, &vp);
      फर्काऊ vp.value.integer;
}

पूर्णाङ्क __vpiBit::as_bit_t::get_type_code(शून्य) अचर
{ फर्काऊ bit_get_type(bit_from_handle(एतत्)); }

पूर्णाङ्क __vpiBit::as_bit_t::vpi_get(पूर्णाङ्क code)
{ फर्काऊ bit_get(code, एतत्); }

वर्ण* __vpiBit::as_bit_t::vpi_get_str(पूर्णाङ्क code)
{ फर्काऊ bit_get_str(code, एतत्); }

शून्य __vpiBit::as_bit_t::vpi_get_मान(p_vpi_मान val)
{ bit_get_मान(एतत्, val); }

vpiHandle __vpiBit::as_bit_t::vpi_put_मान(p_vpi_मान val, पूर्णाङ्क flags)
{ फर्काऊ bit_put_मान(एतत्, val, flags); }

vpiHandle __vpiBit::as_bit_t::vpi_handle(पूर्णाङ्क code)
{ फर्काऊ bit_get_handle(code, एतत्); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
