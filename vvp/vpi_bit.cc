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


ساكن ثابت __vpiBit*بیت_from_handle(ثابت __vpiHandle*ref)
{
      إذا (ref == عدم) أرجع عدم;
      ثابت __vpiBit::as_بیت_t*ptr = dynamic_cast<ثابت __vpiBit::as_بیت_t*> (ref);
      إذا (ptr == عدم) أرجع عدم;
      أرجع reinterpret_cast<ثابت بنية __vpiBit*>(ref);
}

ساكن __vpiBit*بیت_from_handle(__vpiHandle*ref)
{
      إذا (ref == عدم) أرجع عدم;
      ثابت __vpiBit::as_بیت_t*ptr = dynamic_cast<__vpiBit::as_بیت_t*> (ref);
      إذا (ptr == عدم) أرجع عدم;
      أرجع reinterpret_cast<بنية __vpiBit*>(ref);
}

ساكن صحيح بیت_get_type(ثابت __vpiBit*rfp)
{
      أكد(rfp);

      ثابت بنية __vpiSignal*والد = rfp->get_والد();
      أكد(والد);

      بدل (والد->get_type_کد()) {
	حالة vpiNet:
	    أرجع vpi_Net_بیت;
	حالة vpiReg:
	    أرجع vpi_Reg_بیت;
      }
      أكد(0);
      أرجع 0;
}

ساكن صحيح بیت_get(صحيح code, vpiHandle ref)
{
      بنية __vpiBit*rfp = بیت_from_handle(ref);
      أكد(rfp);

      بنية __vpiSignal*والد = rfp->get_والد();
      أكد(والد);

      بدل (code) {
	حالة vpiArray:  // A bit is not an array
	    أرجع 0;

	حالة vpiAutomatic:
	    أرجع vpi_get(vpiAutomatic, والد);

	حالة vpi_شاخص_2:
	    {
		  s_vpi_مقدار vp;
		  vp.format = vpiIntVal;
		  vpi_get_مقدار(rfp->شاخص, &vp);
		  أرجع vp.value.integer;
	    }

	حالة vpi_سطر_No:
	    أرجع vpi_get(vpi_سطر_No, والد);

	حالة vpiScalar:  // A bit is a scalar
	    أرجع 1;

	حالة vpiSigned:  // A bit is unsigned
	    أرجع 0;

	حالة vpi_اندازه:  // A bit has a width of 1
	    أرجع 1;

	حالة vpi_وکتور:  // A bit is not a vector
	    أرجع 0;

	افتراضي:
	    fprintf(stderr, "VPI error: unknown bit_get property %d.\n",
	            code);
	    أرجع vpiUndefined;
      }
}

ساكن حرف* بیت_get_str(صحيح code, vpiHandle ref)
{
      بنية __vpiBit*rfp = بیت_from_handle(ref);
      أكد(rfp);

      إذا (code == vpiFile) {  // Not implemented for now!
            أرجع simple_مجموعه_rbuf_str(file_نامها[0]);
      }

      إذا ((code != vpiName) && (code != vpi_پر_Name)) أرجع عدم;

      بنية __vpiSignal*والد = rfp->get_والد();
      أكد(والد);

      حرف *nm;
      ثابت حرف *ixs;
      nm = strdup(vpi_get_str(vpiName, والد));
      s_vpi_مقدار vp;
      vp.format = vpiDecStrVal;
      vpi_get_مقدار(rfp->شاخص, &vp);
      ixs = vp.value.str;

      حرف *rbuf = generic_get_str(code, vpip_scope(والد), nm, ixs);
      حرر(nm);
      أرجع rbuf;
}

ساكن vpiHandle بیت_get_handle(صحيح code, vpiHandle ref)
{
      بنية __vpiBit*rfp = بیت_from_handle(ref);
      أكد(rfp);

      بنية __vpiSignal*والد = rfp->get_والد();
      أكد(والد);

      بدل (code) {
	حالة vpi_شاخص_2:
	    أرجع rfp->شاخص;

	حالة vpi_والد:
	    أرجع والد;

	حالة vpiScope:
	    أرجع vpi_handle(vpiScope, والد);

	حالة vpiModule:
	    أرجع vpi_handle(vpiModule, والد);
      }

    أرجع 0;
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

ساكن فراغ بیت_get_مقدار(vpiHandle ref, s_vpi_مقدار*vp)
{
      بنية __vpiBit*rfp = بیت_from_handle(ref);
      أكد(rfp);

      بنية __vpiSignal*والد = rfp->get_والد();
      أكد(والد);

      والد->get_بیت_مقدار(rfp, vp);
}

ساكن vpiHandle بیت_put_مقدار(vpiHandle ref, s_vpi_مقدار*vp, صحيح flags)
{
      بنية __vpiBit*rfp = بیت_from_handle(ref);
      أكد(rfp);

      بنية __vpiSignal*والد = rfp->get_والد();
      أكد(والد);

      أرجع والد->put_بیت_مقدار(rfp, vp, flags);
}


صحيح __vpiBit::get_شاخص(فراغ) ثابت
{
      s_vpi_مقدار vp;
      vp.format = vpiIntVal;
      vpi_get_مقدار(شاخص, &vp);
      أرجع vp.value.integer;
}

صحيح __vpiBit::as_بیت_t::get_type_کد(فراغ) ثابت
{ أرجع بیت_get_type(بیت_from_handle(هذا)); }

صحيح __vpiBit::as_بیت_t::vpi_get(صحيح code)
{ أرجع بیت_get(code, هذا); }

حرف* __vpiBit::as_بیت_t::vpi_get_str(صحيح code)
{ أرجع بیت_get_str(code, هذا); }

فراغ __vpiBit::as_بیت_t::vpi_get_مقدار(p_vpi_مقدار val)
{ بیت_get_مقدار(هذا, val); }

vpiHandle __vpiBit::as_بیت_t::vpi_put_مقدار(p_vpi_مقدار val, صحيح flags)
{ أرجع بیت_put_مقدار(هذا, val, flags); }

vpiHandle __vpiBit::as_بیت_t::vpi_handle(صحيح code)
{ أرجع بیت_get_handle(code, هذا); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
