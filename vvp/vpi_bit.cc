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


ایستا ثابت __vpiBit*bit_from_handle(ثابت __vpiHandle*ref)
{
      اگر (ref == پوچ) بازگشت پوچ;
      ثابت __vpiBit::as_bit_t*ptr = dynamic_cast<ثابت __vpiBit::as_bit_t*> (ref);
      اگر (ptr == پوچ) بازگشت پوچ;
      بازگشت reinterpret_cast<ثابت ساختار __vpiBit*>(ref);
}

ایستا __vpiBit*bit_from_handle(__vpiHandle*ref)
{
      اگر (ref == پوچ) بازگشت پوچ;
      ثابت __vpiBit::as_bit_t*ptr = dynamic_cast<__vpiBit::as_bit_t*> (ref);
      اگر (ptr == پوچ) بازگشت پوچ;
      بازگشت reinterpret_cast<ساختار __vpiBit*>(ref);
}

ایستا صحیح bit_get_type(ثابت __vpiBit*rfp)
{
      أكد(rfp);

      ثابت ساختار __vpiSignal*parent = rfp->get_parent();
      أكد(parent);

      انتخاب (parent->get_type_code()) {
	حالت vpiNet:
	    بازگشت vpiNetBit;
	حالت vpiReg:
	    بازگشت vpiRegBit;
      }
      أكد(0);
      بازگشت 0;
}

ایستا صحیح bit_get(صحیح code, vpiHandle ref)
{
      ساختار __vpiBit*rfp = bit_from_handle(ref);
      أكد(rfp);

      ساختار __vpiSignal*parent = rfp->get_parent();
      أكد(parent);

      انتخاب (code) {
	حالت vpiArray:  // A bit is not an array
	    بازگشت 0;

	حالت vpiAutomatic:
	    بازگشت vpi_get(vpiAutomatic, parent);

	حالت vpiIndex:
	    {
		  s_vpi_مقدار vp;
		  vp.format = vpiIntVal;
		  vpi_get_مقدار(rfp->index, &vp);
		  بازگشت vp.value.integer;
	    }

	حالت vpi_سطر_No:
	    بازگشت vpi_get(vpi_سطر_No, parent);

	حالت vpiScalar:  // A bit is a scalar
	    بازگشت 1;

	حالت vpiSigned:  // A bit is unsigned
	    بازگشت 0;

	حالت vpiSize:  // A bit has a width of 1
	    بازگشت 1;

	حالت vpiVector:  // A bit is not a vector
	    بازگشت 0;

	پیشفرض:
	    fprintf(stderr, "VPI error: unknown bit_get property %d.\n",
	            code);
	    بازگشت vpiUndefined;
      }
}

ایستا نویسه* bit_get_str(صحیح code, vpiHandle ref)
{
      ساختار __vpiBit*rfp = bit_from_handle(ref);
      أكد(rfp);

      اگر (code == vpiFile) {  // Not implemented for now!
            بازگشت simple_set_rbuf_str(file_names[0]);
      }

      اگر ((code != vpiName) && (code != vpiFullName)) بازگشت پوچ;

      ساختار __vpiSignal*parent = rfp->get_parent();
      أكد(parent);

      نویسه *nm;
      ثابت نویسه *ixs;
      nm = strdup(vpi_get_str(vpiName, parent));
      s_vpi_مقدار vp;
      vp.format = vpiDecStrVal;
      vpi_get_مقدار(rfp->index, &vp);
      ixs = vp.value.str;

      نویسه *rbuf = generic_get_str(code, vpip_scope(parent), nm, ixs);
      آزاد(nm);
      بازگشت rbuf;
}

ایستا vpiHandle bit_get_handle(صحیح code, vpiHandle ref)
{
      ساختار __vpiBit*rfp = bit_from_handle(ref);
      أكد(rfp);

      ساختار __vpiSignal*parent = rfp->get_parent();
      أكد(parent);

      انتخاب (code) {
	حالت vpiIndex:
	    بازگشت rfp->index;

	حالت vpiParent:
	    بازگشت parent;

	حالت vpiScope:
	    بازگشت vpi_handle(vpiScope, parent);

	حالت vpiModule:
	    بازگشت vpi_handle(vpiModule, parent);
      }

    بازگشت 0;
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

ایستا تهی bit_get_مقدار(vpiHandle ref, s_vpi_مقدار*vp)
{
      ساختار __vpiBit*rfp = bit_from_handle(ref);
      أكد(rfp);

      ساختار __vpiSignal*parent = rfp->get_parent();
      أكد(parent);

      parent->get_bit_مقدار(rfp, vp);
}

ایستا vpiHandle bit_put_مقدار(vpiHandle ref, s_vpi_مقدار*vp, صحیح flags)
{
      ساختار __vpiBit*rfp = bit_from_handle(ref);
      أكد(rfp);

      ساختار __vpiSignal*parent = rfp->get_parent();
      أكد(parent);

      بازگشت parent->put_bit_مقدار(rfp, vp, flags);
}


صحیح __vpiBit::get_index(تهی) ثابت
{
      s_vpi_مقدار vp;
      vp.format = vpiIntVal;
      vpi_get_مقدار(index, &vp);
      بازگشت vp.value.integer;
}

صحیح __vpiBit::as_bit_t::get_type_code(تهی) ثابت
{ بازگشت bit_get_type(bit_from_handle(این)); }

صحیح __vpiBit::as_bit_t::vpi_get(صحیح code)
{ بازگشت bit_get(code, این); }

نویسه* __vpiBit::as_bit_t::vpi_get_str(صحیح code)
{ بازگشت bit_get_str(code, این); }

تهی __vpiBit::as_bit_t::vpi_get_مقدار(p_vpi_مقدار val)
{ bit_get_مقدار(این, val); }

vpiHandle __vpiBit::as_bit_t::vpi_put_مقدار(p_vpi_مقدار val, صحیح flags)
{ بازگشت bit_put_مقدار(این, val, flags); }

vpiHandle __vpiBit::as_bit_t::vpi_handle(صحیح code)
{ بازگشت bit_get_handle(code, این); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
