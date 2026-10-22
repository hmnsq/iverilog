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


ସ୍ଥିର ଅଚଳ __vpiBit*bit_from_handle(ଅଚଳ __vpiHandle*ref)
{
      ଯଦି (ref == ଖାଲି) ଫେରନ୍ତୁ ଖାଲି;
      ଅଚଳ __vpiBit::as_bit_t*ptr = dynamic_cast<ଅଚଳ __vpiBit::as_bit_t*> (ref);
      ଯଦି (ptr == ଖାଲି) ଫେରନ୍ତୁ ଖାଲି;
      ଫେରନ୍ତୁ reinterpret_cast<ଅଚଳ ସଂରଚନା __vpiBit*>(ref);
}

ସ୍ଥିର __vpiBit*bit_from_handle(__vpiHandle*ref)
{
      ଯଦି (ref == ଖାଲି) ଫେରନ୍ତୁ ଖାଲି;
      ଅଚଳ __vpiBit::as_bit_t*ptr = dynamic_cast<__vpiBit::as_bit_t*> (ref);
      ଯଦି (ptr == ଖାଲି) ଫେରନ୍ତୁ ଖାଲି;
      ଫେରନ୍ତୁ reinterpret_cast<ସଂରଚନା __vpiBit*>(ref);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା bit_get_type(ଅଚଳ __vpiBit*rfp)
{
      ପ୍ରତିଜ୍ଞା(rfp);

      ଅଚଳ ସଂରଚନା __vpiSignal*parent = rfp->get_parent();
      ପ୍ରତିଜ୍ଞା(parent);

      ବାଛ (parent->get_type_code()) {
	ସ୍ଥିତି vpiNet:
	    ଫେରନ୍ତୁ vpiNetBit;
	ସ୍ଥିତି vpiReg:
	    ଫେରନ୍ତୁ vpiRegBit;
      }
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା bit_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ସଂରଚନା __vpiBit*rfp = bit_from_handle(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ସଂରଚନା __vpiSignal*parent = rfp->get_parent();
      ପ୍ରତିଜ୍ଞା(parent);

      ବାଛ (code) {
	ସ୍ଥିତି vpiArray:  // A bit is not an array
	    ଫେରନ୍ତୁ 0;

	ସ୍ଥିତି vpiAutomatic:
	    ଫେରନ୍ତୁ vpi_get(vpiAutomatic, parent);

	ସ୍ଥିତି vpiIndex:
	    {
		  s_vpi_ମୂଲ୍ୟ vp;
		  vp.format = vpiIntVal;
		  vpi_get_ମୂଲ୍ୟ(rfp->index, &vp);
		  ଫେରନ୍ତୁ vp.value.integer;
	    }

	ସ୍ଥିତି vpi_ଧାଡ଼ି_No:
	    ଫେରନ୍ତୁ vpi_get(vpi_ଧାଡ଼ି_No, parent);

	ସ୍ଥିତି vpiScalar:  // A bit is a scalar
	    ଫେରନ୍ତୁ 1;

	ସ୍ଥିତି vpiSigned:  // A bit is unsigned
	    ଫେରନ୍ତୁ 0;

	ସ୍ଥିତି vpiSize:  // A bit has a width of 1
	    ଫେରନ୍ତୁ 1;

	ସ୍ଥିତି vpiVector:  // A bit is not a vector
	    ଫେରନ୍ତୁ 0;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "VPI error: unknown bit_get property %d.\n",
	            code);
	    ଫେରନ୍ତୁ vpiUndefined;
      }
}

ସ୍ଥିର ଅକ୍ଷର* bit_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ସଂରଚନା __vpiBit*rfp = bit_from_handle(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ଯଦି (code == vpiFile) {  // Not implemented for now!
            ଫେରନ୍ତୁ simple_set_rbuf_str(file_names[0]);
      }

      ଯଦି ((code != vpiName) && (code != vpiFullName)) ଫେରନ୍ତୁ ଖାଲି;

      ସଂରଚନା __vpiSignal*parent = rfp->get_parent();
      ପ୍ରତିଜ୍ଞା(parent);

      ଅକ୍ଷର *nm;
      ଅଚଳ ଅକ୍ଷର *ixs;
      nm = strdup(vpi_get_str(vpiName, parent));
      s_vpi_ମୂଲ୍ୟ vp;
      vp.format = vpiDecStrVal;
      vpi_get_ମୂଲ୍ୟ(rfp->index, &vp);
      ixs = vp.value.str;

      ଅକ୍ଷର *rbuf = generic_get_str(code, vpip_scope(parent), nm, ixs);
      ମୁକ୍ତ(nm);
      ଫେରନ୍ତୁ rbuf;
}

ସ୍ଥିର vpiHandle bit_get_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, vpiHandle ref)
{
      ସଂରଚନା __vpiBit*rfp = bit_from_handle(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ସଂରଚନା __vpiSignal*parent = rfp->get_parent();
      ପ୍ରତିଜ୍ଞା(parent);

      ବାଛ (code) {
	ସ୍ଥିତି vpiIndex:
	    ଫେରନ୍ତୁ rfp->index;

	ସ୍ଥିତି vpiParent:
	    ଫେରନ୍ତୁ parent;

	ସ୍ଥିତି vpiScope:
	    ଫେରନ୍ତୁ vpi_handle(vpiScope, parent);

	ସ୍ଥିତି vpiModule:
	    ଫେରନ୍ତୁ vpi_handle(vpiModule, parent);
      }

    ଫେରନ୍ତୁ 0;
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

ସ୍ଥିର ଶୂନ୍ୟ bit_get_ମୂଲ୍ୟ(vpiHandle ref, s_vpi_ମୂଲ୍ୟ*vp)
{
      ସଂରଚନା __vpiBit*rfp = bit_from_handle(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ସଂରଚନା __vpiSignal*parent = rfp->get_parent();
      ପ୍ରତିଜ୍ଞା(parent);

      parent->get_bit_ମୂଲ୍ୟ(rfp, vp);
}

ସ୍ଥିର vpiHandle bit_put_ମୂଲ୍ୟ(vpiHandle ref, s_vpi_ମୂଲ୍ୟ*vp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
      ସଂରଚନା __vpiBit*rfp = bit_from_handle(ref);
      ପ୍ରତିଜ୍ଞା(rfp);

      ସଂରଚନା __vpiSignal*parent = rfp->get_parent();
      ପ୍ରତିଜ୍ଞା(parent);

      ଫେରନ୍ତୁ parent->put_bit_ମୂଲ୍ୟ(rfp, vp, flags);
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiBit::get_index(ଶୂନ୍ୟ) ଅଚଳ
{
      s_vpi_ମୂଲ୍ୟ vp;
      vp.format = vpiIntVal;
      vpi_get_ମୂଲ୍ୟ(index, &vp);
      ଫେରନ୍ତୁ vp.value.integer;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiBit::as_bit_t::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ bit_get_type(bit_from_handle(ଏହି)); }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiBit::as_bit_t::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ bit_get(code, ଏହି); }

ଅକ୍ଷର* __vpiBit::as_bit_t::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ bit_get_str(code, ଏହି); }

ଶୂନ୍ୟ __vpiBit::as_bit_t::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val)
{ bit_get_ମୂଲ୍ୟ(ଏହି, val); }

vpiHandle __vpiBit::as_bit_t::vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{ ଫେରନ୍ତୁ bit_put_ମୂଲ୍ୟ(ଏହି, val, flags); }

vpiHandle __vpiBit::as_bit_t::vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{ ଫେରନ୍ତୁ bit_get_handle(code, ଏହି); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
