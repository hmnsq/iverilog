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


நிலையான மாறிலி __vpiBit*bit_from_handle(மாறிலி __vpiHandle*ref)
{
      எனில் (ref == வெற்று) திரும்பு வெற்று;
      மாறிலி __vpiBit::as_bit_t*ptr = dynamic_cast<மாறிலி __vpiBit::as_bit_t*> (ref);
      எனில் (ptr == வெற்று) திரும்பு வெற்று;
      திரும்பு reinterpret_cast<மாறிலி கட்டமைப்பு __vpiBit*>(ref);
}

நிலையான __vpiBit*bit_from_handle(__vpiHandle*ref)
{
      எனில் (ref == வெற்று) திரும்பு வெற்று;
      மாறிலி __vpiBit::as_bit_t*ptr = dynamic_cast<__vpiBit::as_bit_t*> (ref);
      எனில் (ptr == வெற்று) திரும்பு வெற்று;
      திரும்பு reinterpret_cast<கட்டமைப்பு __vpiBit*>(ref);
}

நிலையான முழு bit_get_type(மாறிலி __vpiBit*rfp)
{
      ப்ரதிஜ்ஞா(rfp);

      மாறிலி கட்டமைப்பு __vpiSignal*parent = rfp->get_parent();
      ப்ரதிஜ்ஞா(parent);

      தேர்வு (parent->get_type_code()) {
	நிலை vpiNet:
	    திரும்பு vpiNetBit;
	நிலை vpiReg:
	    திரும்பு vpiRegBit;
      }
      ப்ரதிஜ்ஞா(0);
      திரும்பு 0;
}

நிலையான முழு bit_get(முழு code, vpiHandle ref)
{
      கட்டமைப்பு __vpiBit*rfp = bit_from_handle(ref);
      ப்ரதிஜ்ஞா(rfp);

      கட்டமைப்பு __vpiSignal*parent = rfp->get_parent();
      ப்ரதிஜ்ஞா(parent);

      தேர்வு (code) {
	நிலை vpiArray:  // A bit is not an array
	    திரும்பு 0;

	நிலை vpiAutomatic:
	    திரும்பு vpi_get(vpiAutomatic, parent);

	நிலை vpiIndex:
	    {
		  s_vpi_மதிப்பு vp;
		  vp.format = vpiIntVal;
		  vpi_get_மதிப்பு(rfp->index, &vp);
		  திரும்பு vp.value.integer;
	    }

	நிலை vpi_வரி_No:
	    திரும்பு vpi_get(vpi_வரி_No, parent);

	நிலை vpiScalar:  // A bit is a scalar
	    திரும்பு 1;

	நிலை vpiSigned:  // A bit is unsigned
	    திரும்பு 0;

	நிலை vpiSize:  // A bit has a width of 1
	    திரும்பு 1;

	நிலை vpiVector:  // A bit is not a vector
	    திரும்பு 0;

	இயல்பு:
	    fprintf(stderr, "VPI error: unknown bit_get property %d.\n",
	            code);
	    திரும்பு vpiUndefined;
      }
}

நிலையான எழுத்து* bit_get_str(முழு code, vpiHandle ref)
{
      கட்டமைப்பு __vpiBit*rfp = bit_from_handle(ref);
      ப்ரதிஜ்ஞா(rfp);

      எனில் (code == vpiFile) {  // Not implemented for now!
            திரும்பு simple_set_rbuf_str(file_names[0]);
      }

      எனில் ((code != vpiName) && (code != vpiFullName)) திரும்பு வெற்று;

      கட்டமைப்பு __vpiSignal*parent = rfp->get_parent();
      ப்ரதிஜ்ஞா(parent);

      எழுத்து *nm;
      மாறிலி எழுத்து *ixs;
      nm = strdup(vpi_get_str(vpiName, parent));
      s_vpi_மதிப்பு vp;
      vp.format = vpiDecStrVal;
      vpi_get_மதிப்பு(rfp->index, &vp);
      ixs = vp.value.str;

      எழுத்து *rbuf = generic_get_str(code, vpip_scope(parent), nm, ixs);
      விடுவி(nm);
      திரும்பு rbuf;
}

நிலையான vpiHandle bit_get_handle(முழு code, vpiHandle ref)
{
      கட்டமைப்பு __vpiBit*rfp = bit_from_handle(ref);
      ப்ரதிஜ்ஞா(rfp);

      கட்டமைப்பு __vpiSignal*parent = rfp->get_parent();
      ப்ரதிஜ்ஞா(parent);

      தேர்வு (code) {
	நிலை vpiIndex:
	    திரும்பு rfp->index;

	நிலை vpiParent:
	    திரும்பு parent;

	நிலை vpiScope:
	    திரும்பு vpi_handle(vpiScope, parent);

	நிலை vpiModule:
	    திரும்பு vpi_handle(vpiModule, parent);
      }

    திரும்பு 0;
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

நிலையான வெறுமை bit_get_மதிப்பு(vpiHandle ref, s_vpi_மதிப்பு*vp)
{
      கட்டமைப்பு __vpiBit*rfp = bit_from_handle(ref);
      ப்ரதிஜ்ஞா(rfp);

      கட்டமைப்பு __vpiSignal*parent = rfp->get_parent();
      ப்ரதிஜ்ஞா(parent);

      parent->get_bit_மதிப்பு(rfp, vp);
}

நிலையான vpiHandle bit_put_மதிப்பு(vpiHandle ref, s_vpi_மதிப்பு*vp, முழு flags)
{
      கட்டமைப்பு __vpiBit*rfp = bit_from_handle(ref);
      ப்ரதிஜ்ஞா(rfp);

      கட்டமைப்பு __vpiSignal*parent = rfp->get_parent();
      ப்ரதிஜ்ஞா(parent);

      திரும்பு parent->put_bit_மதிப்பு(rfp, vp, flags);
}


முழு __vpiBit::get_index(வெறுமை) மாறிலி
{
      s_vpi_மதிப்பு vp;
      vp.format = vpiIntVal;
      vpi_get_மதிப்பு(index, &vp);
      திரும்பு vp.value.integer;
}

முழு __vpiBit::as_bit_t::get_type_code(வெறுமை) மாறிலி
{ திரும்பு bit_get_type(bit_from_handle(இது)); }

முழு __vpiBit::as_bit_t::vpi_get(முழு code)
{ திரும்பு bit_get(code, இது); }

எழுத்து* __vpiBit::as_bit_t::vpi_get_str(முழு code)
{ திரும்பு bit_get_str(code, இது); }

வெறுமை __vpiBit::as_bit_t::vpi_get_மதிப்பு(p_vpi_மதிப்பு val)
{ bit_get_மதிப்பு(இது, val); }

vpiHandle __vpiBit::as_bit_t::vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு flags)
{ திரும்பு bit_put_மதிப்பு(இது, val, flags); }

vpiHandle __vpiBit::as_bit_t::vpi_handle(முழு code)
{ திரும்பு bit_get_handle(code, இது); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
