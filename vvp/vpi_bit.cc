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


സ്ഥിതം സ്ഥിരം __vpiBit*bit_from_handle(സ്ഥിരം __vpiHandle*ref)
{
      എങ്കിൽ (ref == ശൂന്യ) തിരികെ ശൂന്യ;
      സ്ഥിരം __vpiBit::as_bit_t*ptr = dynamic_cast<സ്ഥിരം __vpiBit::as_bit_t*> (ref);
      എങ്കിൽ (ptr == ശൂന്യ) തിരികെ ശൂന്യ;
      തിരികെ reinterpret_cast<സ്ഥിരം ഘടന __vpiBit*>(ref);
}

സ്ഥിതം __vpiBit*bit_from_handle(__vpiHandle*ref)
{
      എങ്കിൽ (ref == ശൂന്യ) തിരികെ ശൂന്യ;
      സ്ഥിരം __vpiBit::as_bit_t*ptr = dynamic_cast<__vpiBit::as_bit_t*> (ref);
      എങ്കിൽ (ptr == ശൂന്യ) തിരികെ ശൂന്യ;
      തിരികെ reinterpret_cast<ഘടന __vpiBit*>(ref);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ bit_get_type(സ്ഥിരം __vpiBit*rfp)
{
      പ്രതിജ്ഞാ(rfp);

      സ്ഥിരം ഘടന __vpiSignal*parent = rfp->get_parent();
      പ്രതിജ്ഞാ(parent);

      തിരഞ്ഞെടുപ്പ് (parent->get_type_code()) {
	അവസ്ഥ vpiNet:
	    തിരികെ vpiNetBit;
	അവസ്ഥ vpiReg:
	    തിരികെ vpiRegBit;
      }
      പ്രതിജ്ഞാ(0);
      തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ bit_get(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      ഘടന __vpiBit*rfp = bit_from_handle(ref);
      പ്രതിജ്ഞാ(rfp);

      ഘടന __vpiSignal*parent = rfp->get_parent();
      പ്രതിജ്ഞാ(parent);

      തിരഞ്ഞെടുപ്പ് (code) {
	അവസ്ഥ vpiArray:  // A bit is not an array
	    തിരികെ 0;

	അവസ്ഥ vpiAutomatic:
	    തിരികെ vpi_get(vpiAutomatic, parent);

	അവസ്ഥ vpiIndex:
	    {
		  s_vpi_മൂല്യം vp;
		  vp.format = vpiIntVal;
		  vpi_get_മൂല്യം(rfp->index, &vp);
		  തിരികെ vp.value.integer;
	    }

	അവസ്ഥ vpi_വരി_No:
	    തിരികെ vpi_get(vpi_വരി_No, parent);

	അവസ്ഥ vpiScalar:  // A bit is a scalar
	    തിരികെ 1;

	അവസ്ഥ vpiSigned:  // A bit is unsigned
	    തിരികെ 0;

	അവസ്ഥ vpiSize:  // A bit has a width of 1
	    തിരികെ 1;

	അവസ്ഥ vpiVector:  // A bit is not a vector
	    തിരികെ 0;

	സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "VPI error: unknown bit_get property %d.\n",
	            code);
	    തിരികെ vpiUndefined;
      }
}

സ്ഥിതം അക്ഷരം* bit_get_str(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      ഘടന __vpiBit*rfp = bit_from_handle(ref);
      പ്രതിജ്ഞാ(rfp);

      എങ്കിൽ (code == vpiFile) {  // Not implemented for now!
            തിരികെ simple_set_rbuf_str(file_names[0]);
      }

      എങ്കിൽ ((code != vpiName) && (code != vpiFullName)) തിരികെ ശൂന്യ;

      ഘടന __vpiSignal*parent = rfp->get_parent();
      പ്രതിജ്ഞാ(parent);

      അക്ഷരം *nm;
      സ്ഥിരം അക്ഷരം *ixs;
      nm = strdup(vpi_get_str(vpiName, parent));
      s_vpi_മൂല്യം vp;
      vp.format = vpiDecStrVal;
      vpi_get_മൂല്യം(rfp->index, &vp);
      ixs = vp.value.str;

      അക്ഷരം *rbuf = generic_get_str(code, vpip_scope(parent), nm, ixs);
      സ്വതന്ത്രം(nm);
      തിരികെ rbuf;
}

സ്ഥിതം vpiHandle bit_get_handle(പൂർണ്ണസംഖ്യ code, vpiHandle ref)
{
      ഘടന __vpiBit*rfp = bit_from_handle(ref);
      പ്രതിജ്ഞാ(rfp);

      ഘടന __vpiSignal*parent = rfp->get_parent();
      പ്രതിജ്ഞാ(parent);

      തിരഞ്ഞെടുപ്പ് (code) {
	അവസ്ഥ vpiIndex:
	    തിരികെ rfp->index;

	അവസ്ഥ vpiParent:
	    തിരികെ parent;

	അവസ്ഥ vpiScope:
	    തിരികെ vpi_handle(vpiScope, parent);

	അവസ്ഥ vpiModule:
	    തിരികെ vpi_handle(vpiModule, parent);
      }

    തിരികെ 0;
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

സ്ഥിതം ശൂന്യം bit_get_മൂല്യം(vpiHandle ref, s_vpi_മൂല്യം*vp)
{
      ഘടന __vpiBit*rfp = bit_from_handle(ref);
      പ്രതിജ്ഞാ(rfp);

      ഘടന __vpiSignal*parent = rfp->get_parent();
      പ്രതിജ്ഞാ(parent);

      parent->get_bit_മൂല്യം(rfp, vp);
}

സ്ഥിതം vpiHandle bit_put_മൂല്യം(vpiHandle ref, s_vpi_മൂല്യം*vp, പൂർണ്ണസംഖ്യ flags)
{
      ഘടന __vpiBit*rfp = bit_from_handle(ref);
      പ്രതിജ്ഞാ(rfp);

      ഘടന __vpiSignal*parent = rfp->get_parent();
      പ്രതിജ്ഞാ(parent);

      തിരികെ parent->put_bit_മൂല്യം(rfp, vp, flags);
}


പൂർണ്ണസംഖ്യ __vpiBit::get_index(ശൂന്യം) സ്ഥിരം
{
      s_vpi_മൂല്യം vp;
      vp.format = vpiIntVal;
      vpi_get_മൂല്യം(index, &vp);
      തിരികെ vp.value.integer;
}

പൂർണ്ണസംഖ്യ __vpiBit::as_bit_t::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ bit_get_type(bit_from_handle(ഇത്)); }

പൂർണ്ണസംഖ്യ __vpiBit::as_bit_t::vpi_get(പൂർണ്ണസംഖ്യ code)
{ തിരികെ bit_get(code, ഇത്); }

അക്ഷരം* __vpiBit::as_bit_t::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{ തിരികെ bit_get_str(code, ഇത്); }

ശൂന്യം __vpiBit::as_bit_t::vpi_get_മൂല്യം(p_vpi_മൂല്യം val)
{ bit_get_മൂല്യം(ഇത്, val); }

vpiHandle __vpiBit::as_bit_t::vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags)
{ തിരികെ bit_put_മൂല്യം(ഇത്, val, flags); }

vpiHandle __vpiBit::as_bit_t::vpi_handle(പൂർണ്ണസംഖ്യ code)
{ തിരികെ bit_get_handle(code, ഇത്); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
