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


স্থির ধ্রুৱ __vpiBit*bit_from_handle(ধ্রুৱ __vpiHandle*ref)
{
      যদি (ref == রিক্ত) ঘূৰাই রিক্ত;
      ধ্রুৱ __vpiBit::as_bit_t*ptr = dynamic_cast<ধ্রুৱ __vpiBit::as_bit_t*> (ref);
      যদি (ptr == রিক্ত) ঘূৰাই রিক্ত;
      ঘূৰাই reinterpret_cast<ধ্রুৱ সংরচনা __vpiBit*>(ref);
}

স্থির __vpiBit*bit_from_handle(__vpiHandle*ref)
{
      যদি (ref == রিক্ত) ঘূৰাই রিক্ত;
      ধ্রুৱ __vpiBit::as_bit_t*ptr = dynamic_cast<__vpiBit::as_bit_t*> (ref);
      যদি (ptr == রিক্ত) ঘূৰাই রিক্ত;
      ঘূৰাই reinterpret_cast<সংরচনা __vpiBit*>(ref);
}

স্থির পূর্ণাঙ্ক bit_get_type(ধ্রুৱ __vpiBit*rfp)
{
      প্রতিজ্ঞা(rfp);

      ধ্রুৱ সংরচনা __vpiSignal*parent = rfp->get_parent();
      প্রতিজ্ঞা(parent);

      ৱিকল্প (parent->get_type_code()) {
	পক্ষ vpiNet:
	    ঘূৰাই vpiNetBit;
	পক্ষ vpiReg:
	    ঘূৰাই vpiRegBit;
      }
      প্রতিজ্ঞা(0);
      ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক bit_get(পূর্ণাঙ্ক code, vpiHandle ref)
{
      সংরচনা __vpiBit*rfp = bit_from_handle(ref);
      প্রতিজ্ঞা(rfp);

      সংরচনা __vpiSignal*parent = rfp->get_parent();
      প্রতিজ্ঞা(parent);

      ৱিকল্প (code) {
	পক্ষ vpiArray:  // A bit is not an array
	    ঘূৰাই 0;

	পক্ষ vpiAutomatic:
	    ঘূৰাই vpi_get(vpiAutomatic, parent);

	পক্ষ vpiIndex:
	    {
		  s_vpi_মান vp;
		  vp.format = vpiIntVal;
		  vpi_get_মান(rfp->index, &vp);
		  ঘূৰাই vp.value.integer;
	    }

	পক্ষ vpi_সাৰি_No:
	    ঘূৰাই vpi_get(vpi_সাৰি_No, parent);

	পক্ষ vpiScalar:  // A bit is a scalar
	    ঘূৰাই 1;

	পক্ষ vpiSigned:  // A bit is unsigned
	    ঘূৰাই 0;

	পক্ষ vpiSize:  // A bit has a width of 1
	    ঘূৰাই 1;

	পক্ষ vpiVector:  // A bit is not a vector
	    ঘূৰাই 0;

	স্ৱভাৱ:
	    fprintf(stderr, "VPI error: unknown bit_get property %d.\n",
	            code);
	    ঘূৰাই vpiUndefined;
      }
}

স্থির ৱর্ণ* bit_get_str(পূর্ণাঙ্ক code, vpiHandle ref)
{
      সংরচনা __vpiBit*rfp = bit_from_handle(ref);
      প্রতিজ্ঞা(rfp);

      যদি (code == vpiFile) {  // Not implemented for now!
            ঘূৰাই simple_set_rbuf_str(file_names[0]);
      }

      যদি ((code != vpiName) && (code != vpiFullName)) ঘূৰাই রিক্ত;

      সংরচনা __vpiSignal*parent = rfp->get_parent();
      প্রতিজ্ঞা(parent);

      ৱর্ণ *nm;
      ধ্রুৱ ৱর্ণ *ixs;
      nm = strdup(vpi_get_str(vpiName, parent));
      s_vpi_মান vp;
      vp.format = vpiDecStrVal;
      vpi_get_মান(rfp->index, &vp);
      ixs = vp.value.str;

      ৱর্ণ *rbuf = generic_get_str(code, vpip_scope(parent), nm, ixs);
      মুক্ত(nm);
      ঘূৰাই rbuf;
}

স্থির vpiHandle bit_get_handle(পূর্ণাঙ্ক code, vpiHandle ref)
{
      সংরচনা __vpiBit*rfp = bit_from_handle(ref);
      প্রতিজ্ঞা(rfp);

      সংরচনা __vpiSignal*parent = rfp->get_parent();
      প্রতিজ্ঞা(parent);

      ৱিকল্প (code) {
	পক্ষ vpiIndex:
	    ঘূৰাই rfp->index;

	পক্ষ vpiParent:
	    ঘূৰাই parent;

	পক্ষ vpiScope:
	    ঘূৰাই vpi_handle(vpiScope, parent);

	পক্ষ vpiModule:
	    ঘূৰাই vpi_handle(vpiModule, parent);
      }

    ঘূৰাই 0;
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

স্থির শূন্য bit_get_মান(vpiHandle ref, s_vpi_মান*vp)
{
      সংরচনা __vpiBit*rfp = bit_from_handle(ref);
      প্রতিজ্ঞা(rfp);

      সংরচনা __vpiSignal*parent = rfp->get_parent();
      প্রতিজ্ঞা(parent);

      parent->get_bit_মান(rfp, vp);
}

স্থির vpiHandle bit_put_মান(vpiHandle ref, s_vpi_মান*vp, পূর্ণাঙ্ক flags)
{
      সংরচনা __vpiBit*rfp = bit_from_handle(ref);
      প্রতিজ্ঞা(rfp);

      সংরচনা __vpiSignal*parent = rfp->get_parent();
      প্রতিজ্ঞা(parent);

      ঘূৰাই parent->put_bit_মান(rfp, vp, flags);
}


পূর্ণাঙ্ক __vpiBit::get_index(শূন্য) ধ্রুৱ
{
      s_vpi_মান vp;
      vp.format = vpiIntVal;
      vpi_get_মান(index, &vp);
      ঘূৰাই vp.value.integer;
}

পূর্ণাঙ্ক __vpiBit::as_bit_t::get_type_code(শূন্য) ধ্রুৱ
{ ঘূৰাই bit_get_type(bit_from_handle(এতত্)); }

পূর্ণাঙ্ক __vpiBit::as_bit_t::vpi_get(পূর্ণাঙ্ক code)
{ ঘূৰাই bit_get(code, এতত্); }

ৱর্ণ* __vpiBit::as_bit_t::vpi_get_str(পূর্ণাঙ্ক code)
{ ঘূৰাই bit_get_str(code, এতত্); }

শূন্য __vpiBit::as_bit_t::vpi_get_মান(p_vpi_মান val)
{ bit_get_মান(এতত্, val); }

vpiHandle __vpiBit::as_bit_t::vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক flags)
{ ঘূৰাই bit_put_মান(এতত্, val, flags); }

vpiHandle __vpiBit::as_bit_t::vpi_handle(পূর্ণাঙ্ক code)
{ ঘূৰাই bit_get_handle(code, এতত্); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
