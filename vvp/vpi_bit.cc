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


স্থির ধ্রুবক __vpiBit*bit_from_handle(ধ্রুবক __vpiHandle*ref)
{
      যদি (ref == ফাঁকা) ফেরত ফাঁকা;
      ধ্রুবক __vpiBit::as_bit_t*ptr = dynamic_cast<ধ্রুবক __vpiBit::as_bit_t*> (ref);
      যদি (ptr == ফাঁকা) ফেরত ফাঁকা;
      ফেরত reinterpret_cast<ধ্রুবক কাঠামো __vpiBit*>(ref);
}

স্থির __vpiBit*bit_from_handle(__vpiHandle*ref)
{
      যদি (ref == ফাঁকা) ফেরত ফাঁকা;
      ধ্রুবক __vpiBit::as_bit_t*ptr = dynamic_cast<__vpiBit::as_bit_t*> (ref);
      যদি (ptr == ফাঁকা) ফেরত ফাঁকা;
      ফেরত reinterpret_cast<কাঠামো __vpiBit*>(ref);
}

স্থির পূর্ণসংখ্যা bit_get_type(ধ্রুবক __vpiBit*rfp)
{
      প্রতিজ্ঞা(rfp);

      ধ্রুবক কাঠামো __vpiSignal*parent = rfp->get_parent();
      প্রতিজ্ঞা(parent);

      নির্বাচন (parent->get_type_code()) {
	ক্ষেত্র vpiNet:
	    ফেরত vpiNetBit;
	ক্ষেত্র vpiReg:
	    ফেরত vpiRegBit;
      }
      প্রতিজ্ঞা(0);
      ফেরত 0;
}

স্থির পূর্ণসংখ্যা bit_get(পূর্ণসংখ্যা code, vpiHandle ref)
{
      কাঠামো __vpiBit*rfp = bit_from_handle(ref);
      প্রতিজ্ঞা(rfp);

      কাঠামো __vpiSignal*parent = rfp->get_parent();
      প্রতিজ্ঞা(parent);

      নির্বাচন (code) {
	ক্ষেত্র vpiArray:  // A bit is not an array
	    ফেরত 0;

	ক্ষেত্র vpiAutomatic:
	    ফেরত vpi_get(vpiAutomatic, parent);

	ক্ষেত্র vpi_সূচক_2:
	    {
		  s_vpi_মান vp;
		  vp.format = vpiIntVal;
		  vpi_get_মান(rfp->সূচক, &vp);
		  ফেরত vp.value.integer;
	    }

	ক্ষেত্র vpi_সারি_No:
	    ফেরত vpi_get(vpi_সারি_No, parent);

	ক্ষেত্র vpiScalar:  // A bit is a scalar
	    ফেরত 1;

	ক্ষেত্র vpiSigned:  // A bit is unsigned
	    ফেরত 0;

	ক্ষেত্র vpi_আকার:  // A bit has a width of 1
	    ফেরত 1;

	ক্ষেত্র vpiVector:  // A bit is not a vector
	    ফেরত 0;

	পূর্বনির্ধারিত:
	    fprintf(stderr, "VPI error: unknown bit_get property %d.\n",
	            code);
	    ফেরত vpiUndefined;
      }
}

স্থির অক্ষর* bit_get_str(পূর্ণসংখ্যা code, vpiHandle ref)
{
      কাঠামো __vpiBit*rfp = bit_from_handle(ref);
      প্রতিজ্ঞা(rfp);

      যদি (code == vpiFile) {  // Not implemented for now!
            ফেরত simple_set_rbuf_str(file_names[0]);
      }

      যদি ((code != vpiName) && (code != vpiFullName)) ফেরত ফাঁকা;

      কাঠামো __vpiSignal*parent = rfp->get_parent();
      প্রতিজ্ঞা(parent);

      অক্ষর *nm;
      ধ্রুবক অক্ষর *ixs;
      nm = strdup(vpi_get_str(vpiName, parent));
      s_vpi_মান vp;
      vp.format = vpiDecStrVal;
      vpi_get_মান(rfp->সূচক, &vp);
      ixs = vp.value.str;

      অক্ষর *rbuf = generic_get_str(code, vpip_scope(parent), nm, ixs);
      মুক্ত(nm);
      ফেরত rbuf;
}

স্থির vpiHandle bit_get_handle(পূর্ণসংখ্যা code, vpiHandle ref)
{
      কাঠামো __vpiBit*rfp = bit_from_handle(ref);
      প্রতিজ্ঞা(rfp);

      কাঠামো __vpiSignal*parent = rfp->get_parent();
      প্রতিজ্ঞা(parent);

      নির্বাচন (code) {
	ক্ষেত্র vpi_সূচক_2:
	    ফেরত rfp->সূচক;

	ক্ষেত্র vpiParent:
	    ফেরত parent;

	ক্ষেত্র vpiScope:
	    ফেরত vpi_handle(vpiScope, parent);

	ক্ষেত্র vpiModule:
	    ফেরত vpi_handle(vpiModule, parent);
      }

    ফেরত 0;
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
      কাঠামো __vpiBit*rfp = bit_from_handle(ref);
      প্রতিজ্ঞা(rfp);

      কাঠামো __vpiSignal*parent = rfp->get_parent();
      প্রতিজ্ঞা(parent);

      parent->get_bit_মান(rfp, vp);
}

স্থির vpiHandle bit_put_মান(vpiHandle ref, s_vpi_মান*vp, পূর্ণসংখ্যা flags)
{
      কাঠামো __vpiBit*rfp = bit_from_handle(ref);
      প্রতিজ্ঞা(rfp);

      কাঠামো __vpiSignal*parent = rfp->get_parent();
      প্রতিজ্ঞা(parent);

      ফেরত parent->put_bit_মান(rfp, vp, flags);
}


পূর্ণসংখ্যা __vpiBit::get_সূচক(শূন্য) ধ্রুবক
{
      s_vpi_মান vp;
      vp.format = vpiIntVal;
      vpi_get_মান(সূচক, &vp);
      ফেরত vp.value.integer;
}

পূর্ণসংখ্যা __vpiBit::as_bit_t::get_type_code(শূন্য) ধ্রুবক
{ ফেরত bit_get_type(bit_from_handle(এই)); }

পূর্ণসংখ্যা __vpiBit::as_bit_t::vpi_get(পূর্ণসংখ্যা code)
{ ফেরত bit_get(code, এই); }

অক্ষর* __vpiBit::as_bit_t::vpi_get_str(পূর্ণসংখ্যা code)
{ ফেরত bit_get_str(code, এই); }

শূন্য __vpiBit::as_bit_t::vpi_get_মান(p_vpi_মান val)
{ bit_get_মান(এই, val); }

vpiHandle __vpiBit::as_bit_t::vpi_put_মান(p_vpi_মান val, পূর্ণসংখ্যা flags)
{ ফেরত bit_put_মান(এই, val, flags); }

vpiHandle __vpiBit::as_bit_t::vpi_handle(পূর্ণসংখ্যা code)
{ ফেরত bit_get_handle(code, এই); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
