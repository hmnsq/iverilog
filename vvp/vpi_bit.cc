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
      ຖ້າ (ref == NULL) ສົ່ງຄືນ NULL;
      const __vpiBit::as_bit_t*ptr = dynamic_cast<const __vpiBit::as_bit_t*> (ref);
      ຖ້າ (ptr == NULL) ສົ່ງຄືນ NULL;
      ສົ່ງຄືນ reinterpret_cast<const sa_nracanaa __vpiBit*>(ref);
}

static __vpiBit*bit_from_handle(__vpiHandle*ref)
{
      ຖ້າ (ref == NULL) ສົ່ງຄືນ NULL;
      const __vpiBit::as_bit_t*ptr = dynamic_cast<__vpiBit::as_bit_t*> (ref);
      ຖ້າ (ptr == NULL) ສົ່ງຄືນ NULL;
      ສົ່ງຄືນ reinterpret_cast<sa_nracanaa __vpiBit*>(ref);
}

static ຈຳນວນເຕັມ bit_get_type(const __vpiBit*rfp)
{
      assert(rfp);

      const sa_nracanaa __vpiSignal*parent = rfp->get_parent();
      assert(parent);

      switch (parent->get_type_code()) {
	pak_sa vpiNet:
	    ສົ່ງຄືນ vpiNetBit;
	pak_sa vpiReg:
	    ສົ່ງຄືນ vpiRegBit;
      }
      assert(0);
      ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ bit_get(ຈຳນວນເຕັມ code, vpiHandle ref)
{
      sa_nracanaa __vpiBit*rfp = bit_from_handle(ref);
      assert(rfp);

      sa_nracanaa __vpiSignal*parent = rfp->get_parent();
      assert(parent);

      switch (code) {
	pak_sa vpiArray:  // A bit is not an array
	    ສົ່ງຄືນ 0;

	pak_sa vpiAutomatic:
	    ສົ່ງຄືນ vpi_get(vpiAutomatic, parent);

	pak_sa vpiIndex:
	    {
		  s_vpi_value vp;
		  vp.format = vpiIntVal;
		  vpi_get_value(rfp->index, &vp);
		  ສົ່ງຄືນ vp.value.integer;
	    }

	pak_sa vpiLineNo:
	    ສົ່ງຄືນ vpi_get(vpiLineNo, parent);

	pak_sa vpiScalar:  // A bit is a scalar
	    ສົ່ງຄືນ 1;

	pak_sa vpiSigned:  // A bit is unsigned
	    ສົ່ງຄືນ 0;

	pak_sa vpiSize:  // A bit has a width of 1
	    ສົ່ງຄືນ 1;

	pak_sa vpiVector:  // A bit is not a vector
	    ສົ່ງຄືນ 0;

	default:
	    fprintf(stderr, "VPI error: unknown bit_get property %d.\n",
	            code);
	    ສົ່ງຄືນ vpiUndefined;
      }
}

static var_na* bit_get_str(ຈຳນວນເຕັມ code, vpiHandle ref)
{
      sa_nracanaa __vpiBit*rfp = bit_from_handle(ref);
      assert(rfp);

      ຖ້າ (code == vpiFile) {  // Not implemented for now!
            ສົ່ງຄືນ simple_set_rbuf_str(file_names[0]);
      }

      ຖ້າ ((code != vpiName) && (code != vpiFullName)) ສົ່ງຄືນ NULL;

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
      ສົ່ງຄືນ rbuf;
}

static vpiHandle bit_get_handle(ຈຳນວນເຕັມ code, vpiHandle ref)
{
      sa_nracanaa __vpiBit*rfp = bit_from_handle(ref);
      assert(rfp);

      sa_nracanaa __vpiSignal*parent = rfp->get_parent();
      assert(parent);

      switch (code) {
	pak_sa vpiIndex:
	    ສົ່ງຄືນ rfp->index;

	pak_sa vpiParent:
	    ສົ່ງຄືນ parent;

	pak_sa vpiScope:
	    ສົ່ງຄືນ vpi_handle(vpiScope, parent);

	pak_sa vpiModule:
	    ສົ່ງຄືນ vpi_handle(vpiModule, parent);
      }

    ສົ່ງຄືນ 0;
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

static vpiHandle bit_put_value(vpiHandle ref, s_vpi_value*vp, ຈຳນວນເຕັມ flags)
{
      sa_nracanaa __vpiBit*rfp = bit_from_handle(ref);
      assert(rfp);

      sa_nracanaa __vpiSignal*parent = rfp->get_parent();
      assert(parent);

      ສົ່ງຄືນ parent->put_bit_value(rfp, vp, flags);
}


ຈຳນວນເຕັມ __vpiBit::get_index(void) const
{
      s_vpi_value vp;
      vp.format = vpiIntVal;
      vpi_get_value(index, &vp);
      ສົ່ງຄືນ vp.value.integer;
}

ຈຳນວນເຕັມ __vpiBit::as_bit_t::get_type_code(void) const
{ ສົ່ງຄືນ bit_get_type(bit_from_handle(this)); }

ຈຳນວນເຕັມ __vpiBit::as_bit_t::vpi_get(ຈຳນວນເຕັມ code)
{ ສົ່ງຄືນ bit_get(code, this); }

var_na* __vpiBit::as_bit_t::vpi_get_str(ຈຳນວນເຕັມ code)
{ ສົ່ງຄືນ bit_get_str(code, this); }

void __vpiBit::as_bit_t::vpi_get_value(p_vpi_value val)
{ bit_get_value(this, val); }

vpiHandle __vpiBit::as_bit_t::vpi_put_value(p_vpi_value val, ຈຳນວນເຕັມ flags)
{ ສົ່ງຄືນ bit_put_value(this, val, flags); }

vpiHandle __vpiBit::as_bit_t::vpi_handle(ຈຳນວນເຕັມ code)
{ ສົ່ງຄືນ bit_get_handle(code, this); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
