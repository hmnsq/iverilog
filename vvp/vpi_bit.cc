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


ساکن ثابت __vpiBit*ثنائی_ہندسہ_from_handle(ثابت __vpiHandle*ref)
{
      اگر (ref == تہی) واپس تہی;
      ثابت __vpiBit::as_ثنائی_ہندسہ_t*ptr = dynamic_cast<ثابت __vpiBit::as_ثنائی_ہندسہ_t*> (ref);
      اگر (ptr == تہی) واپس تہی;
      واپس reinterpret_cast<ثابت ساخت __vpiBit*>(ref);
}

ساکن __vpiBit*ثنائی_ہندسہ_from_handle(__vpiHandle*ref)
{
      اگر (ref == تہی) واپس تہی;
      ثابت __vpiBit::as_ثنائی_ہندسہ_t*ptr = dynamic_cast<__vpiBit::as_ثنائی_ہندسہ_t*> (ref);
      اگر (ptr == تہی) واپس تہی;
      واپس reinterpret_cast<ساخت __vpiBit*>(ref);
}

ساکن صحیح ثنائی_ہندسہ_get_type(ثابت __vpiBit*rfp)
{
      أكد(rfp);

      ثابت ساخت __vpiSignal*والد = rfp->get_والد();
      أكد(والد);

      انتخاب (والد->get_type_رمز()) {
	حالت vpiNet:
	    واپس vpi_Net_ثنائی_ہندسہ;
	حالت vpiReg:
	    واپس vpi_Reg_ثنائی_ہندسہ;
      }
      أكد(0);
      واپس 0;
}

ساکن صحیح ثنائی_ہندسہ_get(صحیح code, vpiHandle ref)
{
      ساخت __vpiBit*rfp = ثنائی_ہندسہ_from_handle(ref);
      أكد(rfp);

      ساخت __vpiSignal*والد = rfp->get_والد();
      أكد(والد);

      انتخاب (code) {
	حالت vpiArray:  // A bit is not an array
	    واپس 0;

	حالت vpiAutomatic:
	    واپس vpi_get(vpiAutomatic, والد);

	حالت vpi_اشاریہ_2:
	    {
		  s_vpi_قدر vp;
		  vp.format = vpiIntVal;
		  vpi_get_قدر(rfp->اشاریہ, &vp);
		  واپس vp.value.integer;
	    }

	حالت vpi_سطر_No:
	    واپس vpi_get(vpi_سطر_No, والد);

	حالت vpiScalar:  // A bit is a scalar
	    واپس 1;

	حالت vpiSigned:  // A bit is unsigned
	    واپس 0;

	حالت vpi_سائز:  // A bit has a width of 1
	    واپس 1;

	حالت vpi_سمتیہ:  // A bit is not a vector
	    واپس 0;

	طےشدہ:
	    fprintf(stderr, "VPI error: unknown bit_get property %d.\n",
	            code);
	    واپس vpiUndefined;
      }
}

ساکن حرف* ثنائی_ہندسہ_get_str(صحیح code, vpiHandle ref)
{
      ساخت __vpiBit*rfp = ثنائی_ہندسہ_from_handle(ref);
      أكد(rfp);

      اگر (code == vpiFile) {  // Not implemented for now!
            واپس simple_سیٹ_rbuf_str(file_نام[0]);
      }

      اگر ((code != vpiName) && (code != vpi_بھرا_Name)) واپس تہی;

      ساخت __vpiSignal*والد = rfp->get_والد();
      أكد(والد);

      حرف *nm;
      ثابت حرف *ixs;
      nm = strdup(vpi_get_str(vpiName, والد));
      s_vpi_قدر vp;
      vp.format = vpiDecStrVal;
      vpi_get_قدر(rfp->اشاریہ, &vp);
      ixs = vp.value.str;

      حرف *rbuf = generic_get_str(code, vpip_scope(والد), nm, ixs);
      آزاد(nm);
      واپس rbuf;
}

ساکن vpiHandle ثنائی_ہندسہ_get_handle(صحیح code, vpiHandle ref)
{
      ساخت __vpiBit*rfp = ثنائی_ہندسہ_from_handle(ref);
      أكد(rfp);

      ساخت __vpiSignal*والد = rfp->get_والد();
      أكد(والد);

      انتخاب (code) {
	حالت vpi_اشاریہ_2:
	    واپس rfp->اشاریہ;

	حالت vpi_والد:
	    واپس والد;

	حالت vpiScope:
	    واپس vpi_handle(vpiScope, والد);

	حالت vpiModule:
	    واپس vpi_handle(vpiModule, والد);
      }

    واپس 0;
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

ساکن خالی ثنائی_ہندسہ_get_قدر(vpiHandle ref, s_vpi_قدر*vp)
{
      ساخت __vpiBit*rfp = ثنائی_ہندسہ_from_handle(ref);
      أكد(rfp);

      ساخت __vpiSignal*والد = rfp->get_والد();
      أكد(والد);

      والد->get_ثنائی_ہندسہ_قدر(rfp, vp);
}

ساکن vpiHandle ثنائی_ہندسہ_put_قدر(vpiHandle ref, s_vpi_قدر*vp, صحیح flags)
{
      ساخت __vpiBit*rfp = ثنائی_ہندسہ_from_handle(ref);
      أكد(rfp);

      ساخت __vpiSignal*والد = rfp->get_والد();
      أكد(والد);

      واپس والد->put_ثنائی_ہندسہ_قدر(rfp, vp, flags);
}


صحیح __vpiBit::get_اشاریہ(خالی) ثابت
{
      s_vpi_قدر vp;
      vp.format = vpiIntVal;
      vpi_get_قدر(اشاریہ, &vp);
      واپس vp.value.integer;
}

صحیح __vpiBit::as_ثنائی_ہندسہ_t::get_type_رمز(خالی) ثابت
{ واپس ثنائی_ہندسہ_get_type(ثنائی_ہندسہ_from_handle(یہ)); }

صحیح __vpiBit::as_ثنائی_ہندسہ_t::vpi_get(صحیح code)
{ واپس ثنائی_ہندسہ_get(code, یہ); }

حرف* __vpiBit::as_ثنائی_ہندسہ_t::vpi_get_str(صحیح code)
{ واپس ثنائی_ہندسہ_get_str(code, یہ); }

خالی __vpiBit::as_ثنائی_ہندسہ_t::vpi_get_قدر(p_vpi_قدر val)
{ ثنائی_ہندسہ_get_قدر(یہ, val); }

vpiHandle __vpiBit::as_ثنائی_ہندسہ_t::vpi_put_قدر(p_vpi_قدر val, صحیح flags)
{ واپس ثنائی_ہندسہ_put_قدر(یہ, val, flags); }

vpiHandle __vpiBit::as_ثنائی_ہندسہ_t::vpi_handle(صحیح code)
{ واپس ثنائی_ہندسہ_get_handle(code, یہ); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
