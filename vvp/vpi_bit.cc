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


ساكن ثابت __vpiBit*bit_from_handle(ثابت __vpiHandle*ref)
{
      إذا (ref == عدم) إرجاع عدم;
      ثابت __vpiBit::as_bit_t*ptr = dynamic_cast<ثابت __vpiBit::as_bit_t*> (ref);
      إذا (ptr == عدم) إرجاع عدم;
      إرجاع reinterpret_cast<ثابت بنية __vpiBit*>(ref);
}

ساكن __vpiBit*bit_from_handle(__vpiHandle*ref)
{
      إذا (ref == عدم) إرجاع عدم;
      ثابت __vpiBit::as_bit_t*ptr = dynamic_cast<__vpiBit::as_bit_t*> (ref);
      إذا (ptr == عدم) إرجاع عدم;
      إرجاع reinterpret_cast<بنية __vpiBit*>(ref);
}

ساكن صحيح bit_get_type(ثابت __vpiBit*rfp)
{
      أكد(rfp);

      ثابت بنية __vpiSignal*parent = rfp->get_parent();
      أكد(parent);

      بدل (parent->get_type_code()) {
	حالة vpiNet:
	    إرجاع vpiNetBit;
	حالة vpiReg:
	    إرجاع vpiRegBit;
      }
      أكد(0);
      إرجاع 0;
}

ساكن صحيح bit_get(صحيح code, vpiHandle ref)
{
      بنية __vpiBit*rfp = bit_from_handle(ref);
      أكد(rfp);

      بنية __vpiSignal*parent = rfp->get_parent();
      أكد(parent);

      بدل (code) {
	حالة vpiArray:  // A bit is not an array
	    إرجاع 0;

	حالة vpiAutomatic:
	    إرجاع vpi_get(vpiAutomatic, parent);

	حالة vpiIndex:
	    {
		  s_vpi_قيمة vp;
		  vp.format = vpiIntVal;
		  vpi_get_قيمة(rfp->index, &vp);
		  إرجاع vp.value.integer;
	    }

	حالة vpi_سطر_No:
	    إرجاع vpi_get(vpi_سطر_No, parent);

	حالة vpiScalar:  // A bit is a scalar
	    إرجاع 1;

	حالة vpiSigned:  // A bit is unsigned
	    إرجاع 0;

	حالة vpiSize:  // A bit has a width of 1
	    إرجاع 1;

	حالة vpiVector:  // A bit is not a vector
	    إرجاع 0;

	افتراضي:
	    fprintf(stderr, "VPI error: unknown bit_get property %d.\n",
	            code);
	    إرجاع vpiUndefined;
      }
}

ساكن حرف* bit_get_str(صحيح code, vpiHandle ref)
{
      بنية __vpiBit*rfp = bit_from_handle(ref);
      أكد(rfp);

      إذا (code == vpiFile) {  // Not implemented for now!
            إرجاع simple_set_rbuf_str(file_names[0]);
      }

      إذا ((code != vpiName) && (code != vpiFullName)) إرجاع عدم;

      بنية __vpiSignal*parent = rfp->get_parent();
      أكد(parent);

      حرف *nm;
      ثابت حرف *ixs;
      nm = strdup(vpi_get_str(vpiName, parent));
      s_vpi_قيمة vp;
      vp.format = vpiDecStrVal;
      vpi_get_قيمة(rfp->index, &vp);
      ixs = vp.value.str;

      حرف *rbuf = generic_get_str(code, vpip_scope(parent), nm, ixs);
      حرر(nm);
      إرجاع rbuf;
}

ساكن vpiHandle bit_get_handle(صحيح code, vpiHandle ref)
{
      بنية __vpiBit*rfp = bit_from_handle(ref);
      أكد(rfp);

      بنية __vpiSignal*parent = rfp->get_parent();
      أكد(parent);

      بدل (code) {
	حالة vpiIndex:
	    إرجاع rfp->index;

	حالة vpiParent:
	    إرجاع parent;

	حالة vpiScope:
	    إرجاع vpi_handle(vpiScope, parent);

	حالة vpiModule:
	    إرجاع vpi_handle(vpiModule, parent);
      }

    إرجاع 0;
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

ساكن فراغ bit_get_قيمة(vpiHandle ref, s_vpi_قيمة*vp)
{
      بنية __vpiBit*rfp = bit_from_handle(ref);
      أكد(rfp);

      بنية __vpiSignal*parent = rfp->get_parent();
      أكد(parent);

      parent->get_bit_قيمة(rfp, vp);
}

ساكن vpiHandle bit_put_قيمة(vpiHandle ref, s_vpi_قيمة*vp, صحيح flags)
{
      بنية __vpiBit*rfp = bit_from_handle(ref);
      أكد(rfp);

      بنية __vpiSignal*parent = rfp->get_parent();
      أكد(parent);

      إرجاع parent->put_bit_قيمة(rfp, vp, flags);
}


صحيح __vpiBit::get_index(فراغ) ثابت
{
      s_vpi_قيمة vp;
      vp.format = vpiIntVal;
      vpi_get_قيمة(index, &vp);
      إرجاع vp.value.integer;
}

صحيح __vpiBit::as_bit_t::get_type_code(فراغ) ثابت
{ إرجاع bit_get_type(bit_from_handle(هذا)); }

صحيح __vpiBit::as_bit_t::vpi_get(صحيح code)
{ إرجاع bit_get(code, هذا); }

حرف* __vpiBit::as_bit_t::vpi_get_str(صحيح code)
{ إرجاع bit_get_str(code, هذا); }

فراغ __vpiBit::as_bit_t::vpi_get_قيمة(p_vpi_قيمة val)
{ bit_get_قيمة(هذا, val); }

vpiHandle __vpiBit::as_bit_t::vpi_put_قيمة(p_vpi_قيمة val, صحيح flags)
{ إرجاع bit_put_قيمة(هذا, val, flags); }

vpiHandle __vpiBit::as_bit_t::vpi_handle(صحيح code)
{ إرجاع bit_get_handle(code, هذا); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
