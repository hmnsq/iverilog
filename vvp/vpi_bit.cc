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


अटल स्थिर __vpiBit*bit_from_handle(स्थिर __vpiHandle*ref)
{
      अगर (ref == शून्य) वापस शून्य;
      स्थिर __vpiBit::as_bit_t*ptr = dynamic_cast<स्थिर __vpiBit::as_bit_t*> (ref);
      अगर (ptr == शून्य) वापस शून्य;
      वापस reinterpret_cast<स्थिर काष्ठा __vpiBit*>(ref);
}

अटल __vpiBit*bit_from_handle(__vpiHandle*ref)
{
      अगर (ref == शून्य) वापस शून्य;
      स्थिर __vpiBit::as_bit_t*ptr = dynamic_cast<__vpiBit::as_bit_t*> (ref);
      अगर (ptr == शून्य) वापस शून्य;
      वापस reinterpret_cast<काष्ठा __vpiBit*>(ref);
}

अटल पूर्णांक bit_get_type(स्थिर __vpiBit*rfp)
{
      निश्चित(rfp);

      स्थिर काष्ठा __vpiSignal*parent = rfp->get_parent();
      निश्चित(parent);

      चयन (parent->get_type_code()) {
	हाल vpiNet:
	    वापस vpiNetBit;
	हाल vpiReg:
	    वापस vpiRegBit;
      }
      निश्चित(0);
      वापस 0;
}

अटल पूर्णांक bit_get(पूर्णांक code, vpiHandle ref)
{
      काष्ठा __vpiBit*rfp = bit_from_handle(ref);
      निश्चित(rfp);

      काष्ठा __vpiSignal*parent = rfp->get_parent();
      निश्चित(parent);

      चयन (code) {
	हाल vpiArray:  // A bit is not an array
	    वापस 0;

	हाल vpiAutomatic:
	    वापस vpi_get(vpiAutomatic, parent);

	हाल vpi_सूचकांक_2:
	    {
		  s_vpi_मूल्य vp;
		  vp.format = vpiIntVal;
		  vpi_get_मूल्य(rfp->सूचकांक, &vp);
		  वापस vp.value.integer;
	    }

	हाल vpi_पंक्ति_No:
	    वापस vpi_get(vpi_पंक्ति_No, parent);

	हाल vpiScalar:  // A bit is a scalar
	    वापस 1;

	हाल vpiSigned:  // A bit is unsigned
	    वापस 0;

	हाल vpi_आकार:  // A bit has a width of 1
	    वापस 1;

	हाल vpiVector:  // A bit is not a vector
	    वापस 0;

	शेष:
	    ख_लिखो(मानक_त्रुटि, "VPI error: unknown bit_get property %d.\n",
	            code);
	    वापस vpiUndefined;
      }
}

अटल अक्षर* bit_get_str(पूर्णांक code, vpiHandle ref)
{
      काष्ठा __vpiBit*rfp = bit_from_handle(ref);
      निश्चित(rfp);

      अगर (code == vpiFile) {  // Not implemented for now!
            वापस simple_set_rbuf_str(file_names[0]);
      }

      अगर ((code != vpiName) && (code != vpiFullName)) वापस शून्य;

      काष्ठा __vpiSignal*parent = rfp->get_parent();
      निश्चित(parent);

      अक्षर *nm;
      स्थिर अक्षर *ixs;
      nm = strdup(vpi_get_str(vpiName, parent));
      s_vpi_मूल्य vp;
      vp.format = vpiDecStrVal;
      vpi_get_मूल्य(rfp->सूचकांक, &vp);
      ixs = vp.value.str;

      अक्षर *rbuf = generic_get_str(code, vpip_scope(parent), nm, ixs);
      मुक्त(nm);
      वापस rbuf;
}

अटल vpiHandle bit_get_handle(पूर्णांक code, vpiHandle ref)
{
      काष्ठा __vpiBit*rfp = bit_from_handle(ref);
      निश्चित(rfp);

      काष्ठा __vpiSignal*parent = rfp->get_parent();
      निश्चित(parent);

      चयन (code) {
	हाल vpi_सूचकांक_2:
	    वापस rfp->सूचकांक;

	हाल vpiParent:
	    वापस parent;

	हाल vpiScope:
	    वापस vpi_handle(vpiScope, parent);

	हाल vpiModule:
	    वापस vpi_handle(vpiModule, parent);
      }

    वापस 0;
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

अटल व्योम bit_get_मूल्य(vpiHandle ref, s_vpi_मूल्य*vp)
{
      काष्ठा __vpiBit*rfp = bit_from_handle(ref);
      निश्चित(rfp);

      काष्ठा __vpiSignal*parent = rfp->get_parent();
      निश्चित(parent);

      parent->get_bit_मूल्य(rfp, vp);
}

अटल vpiHandle bit_put_मूल्य(vpiHandle ref, s_vpi_मूल्य*vp, पूर्णांक flags)
{
      काष्ठा __vpiBit*rfp = bit_from_handle(ref);
      निश्चित(rfp);

      काष्ठा __vpiSignal*parent = rfp->get_parent();
      निश्चित(parent);

      वापस parent->put_bit_मूल्य(rfp, vp, flags);
}


पूर्णांक __vpiBit::get_सूचकांक(व्योम) स्थिर
{
      s_vpi_मूल्य vp;
      vp.format = vpiIntVal;
      vpi_get_मूल्य(सूचकांक, &vp);
      वापस vp.value.integer;
}

पूर्णांक __vpiBit::as_bit_t::get_type_code(व्योम) स्थिर
{ वापस bit_get_type(bit_from_handle(यह)); }

पूर्णांक __vpiBit::as_bit_t::vpi_get(पूर्णांक code)
{ वापस bit_get(code, यह); }

अक्षर* __vpiBit::as_bit_t::vpi_get_str(पूर्णांक code)
{ वापस bit_get_str(code, यह); }

व्योम __vpiBit::as_bit_t::vpi_get_मूल्य(p_vpi_मूल्य val)
{ bit_get_मूल्य(यह, val); }

vpiHandle __vpiBit::as_bit_t::vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags)
{ वापस bit_put_मूल्य(यह, val, flags); }

vpiHandle __vpiBit::as_bit_t::vpi_handle(पूर्णांक code)
{ वापस bit_get_handle(code, यह); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
