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
      ᰀᰧ (ref == NULL) ᰀᰧᰌ NULL;
      const __vpiBit::as_bit_t*ptr = dynamic_cast<const __vpiBit::as_bit_t*> (ref);
      ᰀᰧ (ptr == NULL) ᰀᰧᰌ NULL;
      ᰀᰧᰌ reinterpret_cast<const sa_nracanaa __vpiBit*>(ref);
}

static __vpiBit*bit_from_handle(__vpiHandle*ref)
{
      ᰀᰧ (ref == NULL) ᰀᰧᰌ NULL;
      const __vpiBit::as_bit_t*ptr = dynamic_cast<__vpiBit::as_bit_t*> (ref);
      ᰀᰧ (ptr == NULL) ᰀᰧᰌ NULL;
      ᰀᰧᰌ reinterpret_cast<sa_nracanaa __vpiBit*>(ref);
}

static puur_naa_nka bit_get_type(const __vpiBit*rfp)
{
      assert(rfp);

      const sa_nracanaa __vpiSignal*parent = rfp->get_parent();
      assert(parent);

      switch (parent->get_type_code()) {
	pak_sa vpiNet:
	    ᰀᰧᰌ vpiNetBit;
	pak_sa vpiReg:
	    ᰀᰧᰌ vpiRegBit;
      }
      assert(0);
      ᰀᰧᰌ 0;
}

static puur_naa_nka bit_get(puur_naa_nka code, vpiHandle ref)
{
      sa_nracanaa __vpiBit*rfp = bit_from_handle(ref);
      assert(rfp);

      sa_nracanaa __vpiSignal*parent = rfp->get_parent();
      assert(parent);

      switch (code) {
	pak_sa vpiArray:  // A bit is not an array
	    ᰀᰧᰌ 0;

	pak_sa vpiAutomatic:
	    ᰀᰧᰌ vpi_get(vpiAutomatic, parent);

	pak_sa vpiIndex:
	    {
		  s_vpi_value vp;
		  vp.format = vpiIntVal;
		  vpi_get_value(rfp->index, &vp);
		  ᰀᰧᰌ vp.value.integer;
	    }

	pak_sa vpiLineNo:
	    ᰀᰧᰌ vpi_get(vpiLineNo, parent);

	pak_sa vpiScalar:  // A bit is a scalar
	    ᰀᰧᰌ 1;

	pak_sa vpiSigned:  // A bit is unsigned
	    ᰀᰧᰌ 0;

	pak_sa vpiSize:  // A bit has a width of 1
	    ᰀᰧᰌ 1;

	pak_sa vpiVector:  // A bit is not a vector
	    ᰀᰧᰌ 0;

	default:
	    fprintf(stderr, "VPI error: unknown bit_get property %d.\n",
	            code);
	    ᰀᰧᰌ vpiUndefined;
      }
}

static var_na* bit_get_str(puur_naa_nka code, vpiHandle ref)
{
      sa_nracanaa __vpiBit*rfp = bit_from_handle(ref);
      assert(rfp);

      ᰀᰧ (code == vpiFile) {  // Not implemented for now!
            ᰀᰧᰌ simple_set_rbuf_str(file_names[0]);
      }

      ᰀᰧ ((code != vpiName) && (code != vpiFullName)) ᰀᰧᰌ NULL;

      sa_nracanaa __vpiSignal*parent = rfp->get_parent();
      assert(parent);

      var_na *nm;
      const var_na *ixs;
      nm = strdup(vpi_get_str(vpiName, parent));
      s_vpi_value vp;
      vp.format = vpiDecStrVal;
      vpi_get_value(rfp->index, &vp);
      ixs = vp.value.str;

      var_na *rbuf = generic_get_str(code, vpip_scope(parent), nm, ixs);
      free(nm);
      ᰀᰧᰌ rbuf;
}

static vpiHandle bit_get_handle(puur_naa_nka code, vpiHandle ref)
{
      sa_nracanaa __vpiBit*rfp = bit_from_handle(ref);
      assert(rfp);

      sa_nracanaa __vpiSignal*parent = rfp->get_parent();
      assert(parent);

      switch (code) {
	pak_sa vpiIndex:
	    ᰀᰧᰌ rfp->index;

	pak_sa vpiParent:
	    ᰀᰧᰌ parent;

	pak_sa vpiScope:
	    ᰀᰧᰌ vpi_handle(vpiScope, parent);

	pak_sa vpiModule:
	    ᰀᰧᰌ vpi_handle(vpiModule, parent);
      }

    ᰀᰧᰌ 0;
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
      sa_nracanaa __vpiBit*rfp = bit_from_handle(ref);
      assert(rfp);

      sa_nracanaa __vpiSignal*parent = rfp->get_parent();
      assert(parent);

      parent->get_bit_value(rfp, vp);
}

static vpiHandle bit_put_value(vpiHandle ref, s_vpi_value*vp, puur_naa_nka flags)
{
      sa_nracanaa __vpiBit*rfp = bit_from_handle(ref);
      assert(rfp);

      sa_nracanaa __vpiSignal*parent = rfp->get_parent();
      assert(parent);

      ᰀᰧᰌ parent->put_bit_value(rfp, vp, flags);
}


puur_naa_nka __vpiBit::get_index(void) const
{
      s_vpi_value vp;
      vp.format = vpiIntVal;
      vpi_get_value(index, &vp);
      ᰀᰧᰌ vp.value.integer;
}

puur_naa_nka __vpiBit::as_bit_t::get_type_code(void) const
{ ᰀᰧᰌ bit_get_type(bit_from_handle(this)); }

puur_naa_nka __vpiBit::as_bit_t::vpi_get(puur_naa_nka code)
{ ᰀᰧᰌ bit_get(code, this); }

var_na* __vpiBit::as_bit_t::vpi_get_str(puur_naa_nka code)
{ ᰀᰧᰌ bit_get_str(code, this); }

void __vpiBit::as_bit_t::vpi_get_value(p_vpi_value val)
{ bit_get_value(this, val); }

vpiHandle __vpiBit::as_bit_t::vpi_put_value(p_vpi_value val, puur_naa_nka flags)
{ ᰀᰧᰌ bit_put_value(this, val, flags); }

vpiHandle __vpiBit::as_bit_t::vpi_handle(puur_naa_nka code)
{ ᰀᰧᰌ bit_get_handle(code, this); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
