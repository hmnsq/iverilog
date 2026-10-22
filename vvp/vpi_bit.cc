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


સ્થિર અચલ __vpiBit*bit_from_handle(અચલ __vpiHandle*ref)
{
      જો (ref == ખાલી) પાછા ખાલી;
      અચલ __vpiBit::as_bit_t*ptr = dynamic_cast<અચલ __vpiBit::as_bit_t*> (ref);
      જો (ptr == ખાલી) પાછા ખાલી;
      પાછા reinterpret_cast<અચલ સંરચના __vpiBit*>(ref);
}

સ્થિર __vpiBit*bit_from_handle(__vpiHandle*ref)
{
      જો (ref == ખાલી) પાછા ખાલી;
      અચલ __vpiBit::as_bit_t*ptr = dynamic_cast<__vpiBit::as_bit_t*> (ref);
      જો (ptr == ખાલી) પાછા ખાલી;
      પાછા reinterpret_cast<સંરચના __vpiBit*>(ref);
}

સ્થિર પૂર્ણાંક bit_get_type(અચલ __vpiBit*rfp)
{
      પ્રતિજ્ઞા(rfp);

      અચલ સંરચના __vpiSignal*parent = rfp->get_parent();
      પ્રતિજ્ઞા(parent);

      પસંદગી (parent->get_type_code()) {
	સ્થિતિ vpiNet:
	    પાછા vpiNetBit;
	સ્થિતિ vpiReg:
	    પાછા vpiRegBit;
      }
      પ્રતિજ્ઞા(0);
      પાછા 0;
}

સ્થિર પૂર્ણાંક bit_get(પૂર્ણાંક code, vpiHandle ref)
{
      સંરચના __vpiBit*rfp = bit_from_handle(ref);
      પ્રતિજ્ઞા(rfp);

      સંરચના __vpiSignal*parent = rfp->get_parent();
      પ્રતિજ્ઞા(parent);

      પસંદગી (code) {
	સ્થિતિ vpiArray:  // A bit is not an array
	    પાછા 0;

	સ્થિતિ vpiAutomatic:
	    પાછા vpi_get(vpiAutomatic, parent);

	સ્થિતિ vpiIndex:
	    {
		  s_vpi_કિંમત vp;
		  vp.format = vpiIntVal;
		  vpi_get_કિંમત(rfp->index, &vp);
		  પાછા vp.value.integer;
	    }

	સ્થિતિ vpi_લીટી_No:
	    પાછા vpi_get(vpi_લીટી_No, parent);

	સ્થિતિ vpiScalar:  // A bit is a scalar
	    પાછા 1;

	સ્થિતિ vpiSigned:  // A bit is unsigned
	    પાછા 0;

	સ્થિતિ vpiSize:  // A bit has a width of 1
	    પાછા 1;

	સ્થિતિ vpiVector:  // A bit is not a vector
	    પાછા 0;

	મૂળભૂત:
	    fprintf(stderr, "VPI error: unknown bit_get property %d.\n",
	            code);
	    પાછા vpiUndefined;
      }
}

સ્થિર અક્ષર* bit_get_str(પૂર્ણાંક code, vpiHandle ref)
{
      સંરચના __vpiBit*rfp = bit_from_handle(ref);
      પ્રતિજ્ઞા(rfp);

      જો (code == vpiFile) {  // Not implemented for now!
            પાછા simple_set_rbuf_str(file_names[0]);
      }

      જો ((code != vpiName) && (code != vpiFullName)) પાછા ખાલી;

      સંરચના __vpiSignal*parent = rfp->get_parent();
      પ્રતિજ્ઞા(parent);

      અક્ષર *nm;
      અચલ અક્ષર *ixs;
      nm = strdup(vpi_get_str(vpiName, parent));
      s_vpi_કિંમત vp;
      vp.format = vpiDecStrVal;
      vpi_get_કિંમત(rfp->index, &vp);
      ixs = vp.value.str;

      અક્ષર *rbuf = generic_get_str(code, vpip_scope(parent), nm, ixs);
      મુક્ત(nm);
      પાછા rbuf;
}

સ્થિર vpiHandle bit_get_handle(પૂર્ણાંક code, vpiHandle ref)
{
      સંરચના __vpiBit*rfp = bit_from_handle(ref);
      પ્રતિજ્ઞા(rfp);

      સંરચના __vpiSignal*parent = rfp->get_parent();
      પ્રતિજ્ઞા(parent);

      પસંદગી (code) {
	સ્થિતિ vpiIndex:
	    પાછા rfp->index;

	સ્થિતિ vpiParent:
	    પાછા parent;

	સ્થિતિ vpiScope:
	    પાછા vpi_handle(vpiScope, parent);

	સ્થિતિ vpiModule:
	    પાછા vpi_handle(vpiModule, parent);
      }

    પાછા 0;
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

સ્થિર શૂન્ય bit_get_કિંમત(vpiHandle ref, s_vpi_કિંમત*vp)
{
      સંરચના __vpiBit*rfp = bit_from_handle(ref);
      પ્રતિજ્ઞા(rfp);

      સંરચના __vpiSignal*parent = rfp->get_parent();
      પ્રતિજ્ઞા(parent);

      parent->get_bit_કિંમત(rfp, vp);
}

સ્થિર vpiHandle bit_put_કિંમત(vpiHandle ref, s_vpi_કિંમત*vp, પૂર્ણાંક flags)
{
      સંરચના __vpiBit*rfp = bit_from_handle(ref);
      પ્રતિજ્ઞા(rfp);

      સંરચના __vpiSignal*parent = rfp->get_parent();
      પ્રતિજ્ઞા(parent);

      પાછા parent->put_bit_કિંમત(rfp, vp, flags);
}


પૂર્ણાંક __vpiBit::get_index(શૂન્ય) અચલ
{
      s_vpi_કિંમત vp;
      vp.format = vpiIntVal;
      vpi_get_કિંમત(index, &vp);
      પાછા vp.value.integer;
}

પૂર્ણાંક __vpiBit::as_bit_t::get_type_code(શૂન્ય) અચલ
{ પાછા bit_get_type(bit_from_handle(આ)); }

પૂર્ણાંક __vpiBit::as_bit_t::vpi_get(પૂર્ણાંક code)
{ પાછા bit_get(code, આ); }

અક્ષર* __vpiBit::as_bit_t::vpi_get_str(પૂર્ણાંક code)
{ પાછા bit_get_str(code, આ); }

શૂન્ય __vpiBit::as_bit_t::vpi_get_કિંમત(p_vpi_કિંમત val)
{ bit_get_કિંમત(આ, val); }

vpiHandle __vpiBit::as_bit_t::vpi_put_કિંમત(p_vpi_કિંમત val, પૂર્ણાંક flags)
{ પાછા bit_put_કિંમત(આ, val, flags); }

vpiHandle __vpiBit::as_bit_t::vpi_handle(પૂર્ણાંક code)
{ પાછા bit_get_handle(code, આ); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
