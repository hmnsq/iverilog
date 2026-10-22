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


static const __vpiBit*bit_from_handle(const __vpiHandle*ref)
{
      만약 (ref == NULL) 반환 NULL;
      const __vpiBit::as_bit_t*ptr = dynamic_cast<const __vpiBit::as_bit_t*> (ref);
      만약 (ptr == NULL) 반환 NULL;
      반환 reinterpret_cast<const struct __vpiBit*>(ref);
}

static __vpiBit*bit_from_handle(__vpiHandle*ref)
{
      만약 (ref == NULL) 반환 NULL;
      const __vpiBit::as_bit_t*ptr = dynamic_cast<__vpiBit::as_bit_t*> (ref);
      만약 (ptr == NULL) 반환 NULL;
      반환 reinterpret_cast<struct __vpiBit*>(ref);
}

static 정수 bit_get_type(const __vpiBit*rfp)
{
      assert(rfp);

      const struct __vpiSignal*parent = rfp->get_parent();
      assert(parent);

      switch (parent->get_type_code()) {
	case vpiNet:
	    반환 vpiNetBit;
	case vpiReg:
	    반환 vpiRegBit;
      }
      assert(0);
      반환 0;
}

static 정수 bit_get(정수 code, vpiHandle ref)
{
      struct __vpiBit*rfp = bit_from_handle(ref);
      assert(rfp);

      struct __vpiSignal*parent = rfp->get_parent();
      assert(parent);

      switch (code) {
	case vpiArray:  // A bit is not an array
	    반환 0;

	case vpiAutomatic:
	    반환 vpi_get(vpiAutomatic, parent);

	case vpiIndex:
	    {
		  s_vpi_value vp;
		  vp.format = vpiIntVal;
		  vpi_get_value(rfp->index, &vp);
		  반환 vp.value.integer;
	    }

	case vpiLineNo:
	    반환 vpi_get(vpiLineNo, parent);

	case vpiScalar:  // A bit is a scalar
	    반환 1;

	case vpiSigned:  // A bit is unsigned
	    반환 0;

	case vpiSize:  // A bit has a width of 1
	    반환 1;

	case vpiVector:  // A bit is not a vector
	    반환 0;

	default:
	    fprintf(stderr, "VPI error: unknown bit_get property %d.\n",
	            code);
	    반환 vpiUndefined;
      }
}

static char* bit_get_str(정수 code, vpiHandle ref)
{
      struct __vpiBit*rfp = bit_from_handle(ref);
      assert(rfp);

      만약 (code == vpiFile) {  // Not implemented for now!
            반환 simple_set_rbuf_str(file_names[0]);
      }

      만약 ((code != vpiName) && (code != vpiFullName)) 반환 NULL;

      struct __vpiSignal*parent = rfp->get_parent();
      assert(parent);

      char *nm;
      const char *ixs;
      nm = strdup(vpi_get_str(vpiName, parent));
      s_vpi_value vp;
      vp.format = vpiDecStrVal;
      vpi_get_value(rfp->index, &vp);
      ixs = vp.value.str;

      char *rbuf = generic_get_str(code, vpip_scope(parent), nm, ixs);
      free(nm);
      반환 rbuf;
}

static vpiHandle bit_get_handle(정수 code, vpiHandle ref)
{
      struct __vpiBit*rfp = bit_from_handle(ref);
      assert(rfp);

      struct __vpiSignal*parent = rfp->get_parent();
      assert(parent);

      switch (code) {
	case vpiIndex:
	    반환 rfp->index;

	case vpiParent:
	    반환 parent;

	case vpiScope:
	    반환 vpi_handle(vpiScope, parent);

	case vpiModule:
	    반환 vpi_handle(vpiModule, parent);
      }

    반환 0;
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

static void bit_get_value(vpiHandle ref, s_vpi_value*vp)
{
      struct __vpiBit*rfp = bit_from_handle(ref);
      assert(rfp);

      struct __vpiSignal*parent = rfp->get_parent();
      assert(parent);

      parent->get_bit_value(rfp, vp);
}

static vpiHandle bit_put_value(vpiHandle ref, s_vpi_value*vp, 정수 flags)
{
      struct __vpiBit*rfp = bit_from_handle(ref);
      assert(rfp);

      struct __vpiSignal*parent = rfp->get_parent();
      assert(parent);

      반환 parent->put_bit_value(rfp, vp, flags);
}


정수 __vpiBit::get_index(void) const
{
      s_vpi_value vp;
      vp.format = vpiIntVal;
      vpi_get_value(index, &vp);
      반환 vp.value.integer;
}

정수 __vpiBit::as_bit_t::get_type_code(void) const
{ 반환 bit_get_type(bit_from_handle(this)); }

정수 __vpiBit::as_bit_t::vpi_get(정수 code)
{ 반환 bit_get(code, this); }

char* __vpiBit::as_bit_t::vpi_get_str(정수 code)
{ 반환 bit_get_str(code, this); }

void __vpiBit::as_bit_t::vpi_get_value(p_vpi_value val)
{ bit_get_value(this, val); }

vpiHandle __vpiBit::as_bit_t::vpi_put_value(p_vpi_value val, 정수 flags)
{ 반환 bit_put_value(this, val, flags); }

vpiHandle __vpiBit::as_bit_t::vpi_handle(정수 code)
{ 반환 bit_get_handle(code, this); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
