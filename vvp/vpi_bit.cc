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


ساكن ثابت __vpiBit*ثنائی_ہندسہ_from_handle(ثابت __vpiHandle*ref)
{
      إذا (ref == عدم) أرجع عدم;
      ثابت __vpiBit::as_ثنائی_ہندسہ_t*ptr = dynamic_cast<ثابت __vpiBit::as_ثنائی_ہندسہ_t*> (ref);
      إذا (ptr == عدم) أرجع عدم;
      أرجع reinterpret_cast<ثابت بنية __vpiBit*>(ref);
}

ساكن __vpiBit*ثنائی_ہندسہ_from_handle(__vpiHandle*ref)
{
      إذا (ref == عدم) أرجع عدم;
      ثابت __vpiBit::as_ثنائی_ہندسہ_t*ptr = dynamic_cast<__vpiBit::as_ثنائی_ہندسہ_t*> (ref);
      إذا (ptr == عدم) أرجع عدم;
      أرجع reinterpret_cast<بنية __vpiBit*>(ref);
}

ساكن صحيح ثنائی_ہندسہ_get_type(ثابت __vpiBit*rfp)
{
      أكد(rfp);

      ثابت بنية __vpiSignal*ماپا = rfp->get_ماپا();
      أكد(ماپا);

      بدل (ماپا->get_type_کوڈ()) {
	حالة vpiNet:
	    أرجع vpi_Net_ثنائی_ہندسہ;
	حالة vpiReg:
	    أرجع vpi_Reg_ثنائی_ہندسہ;
      }
      أكد(0);
      أرجع 0;
}

ساكن صحيح ثنائی_ہندسہ_get(صحيح code, vpiHandle ref)
{
      بنية __vpiBit*rfp = ثنائی_ہندسہ_from_handle(ref);
      أكد(rfp);

      بنية __vpiSignal*ماپا = rfp->get_ماپا();
      أكد(ماپا);

      بدل (code) {
	حالة vpiArray:  // A bit is not an array
	    أرجع 0;

	حالة vpiAutomatic:
	    أرجع vpi_get(vpiAutomatic, ماپا);

	حالة vpi_اشاریہ_2:
	    {
		  s_vpi_قدر vp;
		  vp.format = vpiIntVal;
		  vpi_get_قدر(rfp->اشاریہ, &vp);
		  أرجع vp.value.integer;
	    }

	حالة vpi_سطر_No:
	    أرجع vpi_get(vpi_سطر_No, ماپا);

	حالة vpiScalar:  // A bit is a scalar
	    أرجع 1;

	حالة vpiSigned:  // A bit is unsigned
	    أرجع 0;

	حالة vpi_ناپ:  // A bit has a width of 1
	    أرجع 1;

	حالة vpi_سمتیہ:  // A bit is not a vector
	    أرجع 0;

	افتراضي:
	    fprintf(stderr, "VPI error: unknown bit_get property %d.\n",
	            code);
	    أرجع vpiUndefined;
      }
}

ساكن حرف* ثنائی_ہندسہ_get_str(صحيح code, vpiHandle ref)
{
      بنية __vpiBit*rfp = ثنائی_ہندسہ_from_handle(ref);
      أكد(rfp);

      إذا (code == vpiFile) {  // Not implemented for now!
            أرجع simple_سیٹ_rbuf_str(file_ناں[0]);
      }

      إذا ((code != vpiName) && (code != vpi_بھریا_Name)) أرجع عدم;

      بنية __vpiSignal*ماپا = rfp->get_ماپا();
      أكد(ماپا);

      حرف *nm;
      ثابت حرف *ixs;
      nm = strdup(vpi_get_str(vpiName, ماپا));
      s_vpi_قدر vp;
      vp.format = vpiDecStrVal;
      vpi_get_قدر(rfp->اشاریہ, &vp);
      ixs = vp.value.str;

      حرف *rbuf = generic_get_str(code, vpip_scope(ماپا), nm, ixs);
      حرر(nm);
      أرجع rbuf;
}

ساكن vpiHandle ثنائی_ہندسہ_get_handle(صحيح code, vpiHandle ref)
{
      بنية __vpiBit*rfp = ثنائی_ہندسہ_from_handle(ref);
      أكد(rfp);

      بنية __vpiSignal*ماپا = rfp->get_ماپا();
      أكد(ماپا);

      بدل (code) {
	حالة vpi_اشاریہ_2:
	    أرجع rfp->اشاریہ;

	حالة vpi_ماپا:
	    أرجع ماپا;

	حالة vpiScope:
	    أرجع vpi_handle(vpiScope, ماپا);

	حالة vpiModule:
	    أرجع vpi_handle(vpiModule, ماپا);
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

ساكن فراغ ثنائی_ہندسہ_get_قدر(vpiHandle ref, s_vpi_قدر*vp)
{
      بنية __vpiBit*rfp = ثنائی_ہندسہ_from_handle(ref);
      أكد(rfp);

      بنية __vpiSignal*ماپا = rfp->get_ماپا();
      أكد(ماپا);

      ماپا->get_ثنائی_ہندسہ_قدر(rfp, vp);
}

ساكن vpiHandle ثنائی_ہندسہ_put_قدر(vpiHandle ref, s_vpi_قدر*vp, صحيح flags)
{
      بنية __vpiBit*rfp = ثنائی_ہندسہ_from_handle(ref);
      أكد(rfp);

      بنية __vpiSignal*ماپا = rfp->get_ماپا();
      أكد(ماپا);

      أرجع ماپا->put_ثنائی_ہندسہ_قدر(rfp, vp, flags);
}


صحيح __vpiBit::get_اشاریہ(فراغ) ثابت
{
      s_vpi_قدر vp;
      vp.format = vpiIntVal;
      vpi_get_قدر(اشاریہ, &vp);
      أرجع vp.value.integer;
}

صحيح __vpiBit::as_ثنائی_ہندسہ_t::get_type_کوڈ(فراغ) ثابت
{ أرجع ثنائی_ہندسہ_get_type(ثنائی_ہندسہ_from_handle(هذا)); }

صحيح __vpiBit::as_ثنائی_ہندسہ_t::vpi_get(صحيح code)
{ أرجع ثنائی_ہندسہ_get(code, هذا); }

حرف* __vpiBit::as_ثنائی_ہندسہ_t::vpi_get_str(صحيح code)
{ أرجع ثنائی_ہندسہ_get_str(code, هذا); }

فراغ __vpiBit::as_ثنائی_ہندسہ_t::vpi_get_قدر(p_vpi_قدر val)
{ ثنائی_ہندسہ_get_قدر(هذا, val); }

vpiHandle __vpiBit::as_ثنائی_ہندسہ_t::vpi_put_قدر(p_vpi_قدر val, صحيح flags)
{ أرجع ثنائی_ہندسہ_put_قدر(هذا, val, flags); }

vpiHandle __vpiBit::as_ثنائی_ہندسہ_t::vpi_handle(صحيح code)
{ أرجع ثنائی_ہندسہ_get_handle(code, هذا); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
