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


స్థిర స్థిరరాశి __vpiBit*బిట్టు_from_handle(స్థిరరాశి __vpiHandle*ref)
{
      ఒకవేళ (ref == ఖాళీ) తిరిగి ఖాళీ;
      స్థిరరాశి __vpiBit::as_బిట్టు_t*ptr = dynamic_cast<స్థిరరాశి __vpiBit::as_బిట్టు_t*> (ref);
      ఒకవేళ (ptr == ఖాళీ) తిరిగి ఖాళీ;
      తిరిగి reinterpret_cast<స్థిరరాశి నిర్మాణం __vpiBit*>(ref);
}

స్థిర __vpiBit*బిట్టు_from_handle(__vpiHandle*ref)
{
      ఒకవేళ (ref == ఖాళీ) తిరిగి ఖాళీ;
      స్థిరరాశి __vpiBit::as_బిట్టు_t*ptr = dynamic_cast<__vpiBit::as_బిట్టు_t*> (ref);
      ఒకవేళ (ptr == ఖాళీ) తిరిగి ఖాళీ;
      తిరిగి reinterpret_cast<నిర్మాణం __vpiBit*>(ref);
}

స్థిర పూర్ణాంకం బిట్టు_get_type(స్థిరరాశి __vpiBit*rfp)
{
      ప్రతిజ్ఞా(rfp);

      స్థిరరాశి నిర్మాణం __vpiSignal*మాతృ = rfp->get_మాతృ();
      ప్రతిజ్ఞా(మాతృ);

      ఎంపిక (మాతృ->get_type_సంకేతం()) {
	స్థితి vpiNet:
	    తిరిగి vpi_Net_బిట్టు;
	స్థితి vpiReg:
	    తిరిగి vpi_Reg_బిట్టు;
      }
      ప్రతిజ్ఞా(0);
      తిరిగి 0;
}

స్థిర పూర్ణాంకం బిట్టు_get(పూర్ణాంకం code, vpiHandle ref)
{
      నిర్మాణం __vpiBit*rfp = బిట్టు_from_handle(ref);
      ప్రతిజ్ఞా(rfp);

      నిర్మాణం __vpiSignal*మాతృ = rfp->get_మాతృ();
      ప్రతిజ్ఞా(మాతృ);

      ఎంపిక (code) {
	స్థితి vpiArray:  // A bit is not an array
	    తిరిగి 0;

	స్థితి vpiAutomatic:
	    తిరిగి vpi_get(vpiAutomatic, మాతృ);

	స్థితి vpi_సూచీ_2:
	    {
		  s_vpi_విలువ vp;
		  vp.format = vpiIntVal;
		  vpi_get_విలువ(rfp->సూచీ, &vp);
		  తిరిగి vp.value.integer;
	    }

	స్థితి vpi_పంక్తి_No:
	    తిరిగి vpi_get(vpi_పంక్తి_No, మాతృ);

	స్థితి vpiScalar:  // A bit is a scalar
	    తిరిగి 1;

	స్థితి vpiSigned:  // A bit is unsigned
	    తిరిగి 0;

	స్థితి vpi_కొలత:  // A bit has a width of 1
	    తిరిగి 1;

	స్థితి vpi_సదిశ:  // A bit is not a vector
	    తిరిగి 0;

	అప్రమేయ:
	    fprintf(stderr, "VPI error: unknown bit_get property %d.\n",
	            code);
	    తిరిగి vpiUndefined;
      }
}

స్థిర అక్షరం* బిట్టు_get_str(పూర్ణాంకం code, vpiHandle ref)
{
      నిర్మాణం __vpiBit*rfp = బిట్టు_from_handle(ref);
      ప్రతిజ్ఞా(rfp);

      ఒకవేళ (code == vpiFile) {  // Not implemented for now!
            తిరిగి simple_సమితి_rbuf_str(file_పేర్లు[0]);
      }

      ఒకవేళ ((code != vpiName) && (code != vpi_నిండిన_Name)) తిరిగి ఖాళీ;

      నిర్మాణం __vpiSignal*మాతృ = rfp->get_మాతృ();
      ప్రతిజ్ఞా(మాతృ);

      అక్షరం *nm;
      స్థిరరాశి అక్షరం *ixs;
      nm = strdup(vpi_get_str(vpiName, మాతృ));
      s_vpi_విలువ vp;
      vp.format = vpiDecStrVal;
      vpi_get_విలువ(rfp->సూచీ, &vp);
      ixs = vp.value.str;

      అక్షరం *rbuf = generic_get_str(code, vpip_scope(మాతృ), nm, ixs);
      విడుదల(nm);
      తిరిగి rbuf;
}

స్థిర vpiHandle బిట్టు_get_handle(పూర్ణాంకం code, vpiHandle ref)
{
      నిర్మాణం __vpiBit*rfp = బిట్టు_from_handle(ref);
      ప్రతిజ్ఞా(rfp);

      నిర్మాణం __vpiSignal*మాతృ = rfp->get_మాతృ();
      ప్రతిజ్ఞా(మాతృ);

      ఎంపిక (code) {
	స్థితి vpi_సూచీ_2:
	    తిరిగి rfp->సూచీ;

	స్థితి vpi_మాతృ:
	    తిరిగి మాతృ;

	స్థితి vpiScope:
	    తిరిగి vpi_handle(vpiScope, మాతృ);

	స్థితి vpiModule:
	    తిరిగి vpi_handle(vpiModule, మాతృ);
      }

    తిరిగి 0;
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

స్థిర శూన్య బిట్టు_get_విలువ(vpiHandle ref, s_vpi_విలువ*vp)
{
      నిర్మాణం __vpiBit*rfp = బిట్టు_from_handle(ref);
      ప్రతిజ్ఞా(rfp);

      నిర్మాణం __vpiSignal*మాతృ = rfp->get_మాతృ();
      ప్రతిజ్ఞా(మాతృ);

      మాతృ->get_బిట్టు_విలువ(rfp, vp);
}

స్థిర vpiHandle బిట్టు_put_విలువ(vpiHandle ref, s_vpi_విలువ*vp, పూర్ణాంకం flags)
{
      నిర్మాణం __vpiBit*rfp = బిట్టు_from_handle(ref);
      ప్రతిజ్ఞా(rfp);

      నిర్మాణం __vpiSignal*మాతృ = rfp->get_మాతృ();
      ప్రతిజ్ఞా(మాతృ);

      తిరిగి మాతృ->put_బిట్టు_విలువ(rfp, vp, flags);
}


పూర్ణాంకం __vpiBit::get_సూచీ(శూన్య) స్థిరరాశి
{
      s_vpi_విలువ vp;
      vp.format = vpiIntVal;
      vpi_get_విలువ(సూచీ, &vp);
      తిరిగి vp.value.integer;
}

పూర్ణాంకం __vpiBit::as_బిట్టు_t::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి బిట్టు_get_type(బిట్టు_from_handle(ఇది)); }

పూర్ణాంకం __vpiBit::as_బిట్టు_t::vpi_get(పూర్ణాంకం code)
{ తిరిగి బిట్టు_get(code, ఇది); }

అక్షరం* __vpiBit::as_బిట్టు_t::vpi_get_str(పూర్ణాంకం code)
{ తిరిగి బిట్టు_get_str(code, ఇది); }

శూన్య __vpiBit::as_బిట్టు_t::vpi_get_విలువ(p_vpi_విలువ val)
{ బిట్టు_get_విలువ(ఇది, val); }

vpiHandle __vpiBit::as_బిట్టు_t::vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags)
{ తిరిగి బిట్టు_put_విలువ(ఇది, val, flags); }

vpiHandle __vpiBit::as_బిట్టు_t::vpi_handle(పూర్ణాంకం code)
{ తిరిగి బిట్టు_get_handle(code, ఇది); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
