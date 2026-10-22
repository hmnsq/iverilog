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


ਸਥਿਰ ਅਚਲ __vpiBit*ਦੋਆਧਾਰੀ_ਅੰਕ_from_handle(ਅਚਲ __vpiHandle*ref)
{
      ਜੇ (ref == ਨਿਰਾ) ਵਾਪਸ ਨਿਰਾ;
      ਅਚਲ __vpiBit::as_ਦੋਆਧਾਰੀ_ਅੰਕ_t*ptr = dynamic_cast<ਅਚਲ __vpiBit::as_ਦੋਆਧਾਰੀ_ਅੰਕ_t*> (ref);
      ਜੇ (ptr == ਨਿਰਾ) ਵਾਪਸ ਨਿਰਾ;
      ਵਾਪਸ reinterpret_cast<ਅਚਲ ਬਣਤਰ __vpiBit*>(ref);
}

ਸਥਿਰ __vpiBit*ਦੋਆਧਾਰੀ_ਅੰਕ_from_handle(__vpiHandle*ref)
{
      ਜੇ (ref == ਨਿਰਾ) ਵਾਪਸ ਨਿਰਾ;
      ਅਚਲ __vpiBit::as_ਦੋਆਧਾਰੀ_ਅੰਕ_t*ptr = dynamic_cast<__vpiBit::as_ਦੋਆਧਾਰੀ_ਅੰਕ_t*> (ref);
      ਜੇ (ptr == ਨਿਰਾ) ਵਾਪਸ ਨਿਰਾ;
      ਵਾਪਸ reinterpret_cast<ਬਣਤਰ __vpiBit*>(ref);
}

ਸਥਿਰ ਪੂਰਨ ਦੋਆਧਾਰੀ_ਅੰਕ_get_type(ਅਚਲ __vpiBit*rfp)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਅਚਲ ਬਣਤਰ __vpiSignal*ਮਾਪਾ = rfp->get_ਮਾਪਾ();
      ਪ੍ਰਤਿਜ੍ਞਾ(ਮਾਪਾ);

      ਚੋਣ (ਮਾਪਾ->get_type_ਕੋਡ()) {
	ਹਾਲਤ vpiNet:
	    ਵਾਪਸ vpi_Net_ਦੋਆਧਾਰੀ_ਅੰਕ;
	ਹਾਲਤ vpiReg:
	    ਵਾਪਸ vpi_Reg_ਦੋਆਧਾਰੀ_ਅੰਕ;
      }
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ ਦੋਆਧਾਰੀ_ਅੰਕ_get(ਪੂਰਨ code, vpiHandle ref)
{
      ਬਣਤਰ __vpiBit*rfp = ਦੋਆਧਾਰੀ_ਅੰਕ_from_handle(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਬਣਤਰ __vpiSignal*ਮਾਪਾ = rfp->get_ਮਾਪਾ();
      ਪ੍ਰਤਿਜ੍ਞਾ(ਮਾਪਾ);

      ਚੋਣ (code) {
	ਹਾਲਤ vpiArray:  // A bit is not an array
	    ਵਾਪਸ 0;

	ਹਾਲਤ vpiAutomatic:
	    ਵਾਪਸ vpi_get(vpiAutomatic, ਮਾਪਾ);

	ਹਾਲਤ vpi_ਸੂਚਕਾਂਕ_2:
	    {
		  s_vpi_ਮੁੱਲ vp;
		  vp.format = vpiIntVal;
		  vpi_get_ਮੁੱਲ(rfp->ਸੂਚਕਾਂਕ, &vp);
		  ਵਾਪਸ vp.value.integer;
	    }

	ਹਾਲਤ vpi_ਸਤਰ_No:
	    ਵਾਪਸ vpi_get(vpi_ਸਤਰ_No, ਮਾਪਾ);

	ਹਾਲਤ vpiScalar:  // A bit is a scalar
	    ਵਾਪਸ 1;

	ਹਾਲਤ vpiSigned:  // A bit is unsigned
	    ਵਾਪਸ 0;

	ਹਾਲਤ vpi_ਮਾਪ:  // A bit has a width of 1
	    ਵਾਪਸ 1;

	ਹਾਲਤ vpi_ਸਦਿਸ਼:  // A bit is not a vector
	    ਵਾਪਸ 0;

	ਮੂਲ:
	    fprintf(stderr, "VPI error: unknown bit_get property %d.\n",
	            code);
	    ਵਾਪਸ vpiUndefined;
      }
}

ਸਥਿਰ ਅੱਖਰ* ਦੋਆਧਾਰੀ_ਅੰਕ_get_str(ਪੂਰਨ code, vpiHandle ref)
{
      ਬਣਤਰ __vpiBit*rfp = ਦੋਆਧਾਰੀ_ਅੰਕ_from_handle(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਜੇ (code == vpiFile) {  // Not implemented for now!
            ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(file_ਨਾਂ[0]);
      }

      ਜੇ ((code != vpiName) && (code != vpi_ਭਰਿਆ_Name)) ਵਾਪਸ ਨਿਰਾ;

      ਬਣਤਰ __vpiSignal*ਮਾਪਾ = rfp->get_ਮਾਪਾ();
      ਪ੍ਰਤਿਜ੍ਞਾ(ਮਾਪਾ);

      ਅੱਖਰ *nm;
      ਅਚਲ ਅੱਖਰ *ixs;
      nm = strdup(vpi_get_str(vpiName, ਮਾਪਾ));
      s_vpi_ਮੁੱਲ vp;
      vp.format = vpiDecStrVal;
      vpi_get_ਮੁੱਲ(rfp->ਸੂਚਕਾਂਕ, &vp);
      ixs = vp.value.str;

      ਅੱਖਰ *rbuf = generic_get_str(code, vpip_scope(ਮਾਪਾ), nm, ixs);
      ਮੁਕਤ(nm);
      ਵਾਪਸ rbuf;
}

ਸਥਿਰ vpiHandle ਦੋਆਧਾਰੀ_ਅੰਕ_get_handle(ਪੂਰਨ code, vpiHandle ref)
{
      ਬਣਤਰ __vpiBit*rfp = ਦੋਆਧਾਰੀ_ਅੰਕ_from_handle(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਬਣਤਰ __vpiSignal*ਮਾਪਾ = rfp->get_ਮਾਪਾ();
      ਪ੍ਰਤਿਜ੍ਞਾ(ਮਾਪਾ);

      ਚੋਣ (code) {
	ਹਾਲਤ vpi_ਸੂਚਕਾਂਕ_2:
	    ਵਾਪਸ rfp->ਸੂਚਕਾਂਕ;

	ਹਾਲਤ vpi_ਮਾਪਾ:
	    ਵਾਪਸ ਮਾਪਾ;

	ਹਾਲਤ vpiScope:
	    ਵਾਪਸ vpi_handle(vpiScope, ਮਾਪਾ);

	ਹਾਲਤ vpiModule:
	    ਵਾਪਸ vpi_handle(vpiModule, ਮਾਪਾ);
      }

    ਵਾਪਸ 0;
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

ਸਥਿਰ ਖਾਲੀ ਦੋਆਧਾਰੀ_ਅੰਕ_get_ਮੁੱਲ(vpiHandle ref, s_vpi_ਮੁੱਲ*vp)
{
      ਬਣਤਰ __vpiBit*rfp = ਦੋਆਧਾਰੀ_ਅੰਕ_from_handle(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਬਣਤਰ __vpiSignal*ਮਾਪਾ = rfp->get_ਮਾਪਾ();
      ਪ੍ਰਤਿਜ੍ਞਾ(ਮਾਪਾ);

      ਮਾਪਾ->get_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮੁੱਲ(rfp, vp);
}

ਸਥਿਰ vpiHandle ਦੋਆਧਾਰੀ_ਅੰਕ_put_ਮੁੱਲ(vpiHandle ref, s_vpi_ਮੁੱਲ*vp, ਪੂਰਨ flags)
{
      ਬਣਤਰ __vpiBit*rfp = ਦੋਆਧਾਰੀ_ਅੰਕ_from_handle(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਬਣਤਰ __vpiSignal*ਮਾਪਾ = rfp->get_ਮਾਪਾ();
      ਪ੍ਰਤਿਜ੍ਞਾ(ਮਾਪਾ);

      ਵਾਪਸ ਮਾਪਾ->put_ਦੋਆਧਾਰੀ_ਅੰਕ_ਮੁੱਲ(rfp, vp, flags);
}


ਪੂਰਨ __vpiBit::get_ਸੂਚਕਾਂਕ(ਖਾਲੀ) ਅਚਲ
{
      s_vpi_ਮੁੱਲ vp;
      vp.format = vpiIntVal;
      vpi_get_ਮੁੱਲ(ਸੂਚਕਾਂਕ, &vp);
      ਵਾਪਸ vp.value.integer;
}

ਪੂਰਨ __vpiBit::as_ਦੋਆਧਾਰੀ_ਅੰਕ_t::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_get_type(ਦੋਆਧਾਰੀ_ਅੰਕ_from_handle(ਇਹ)); }

ਪੂਰਨ __vpiBit::as_ਦੋਆਧਾਰੀ_ਅੰਕ_t::vpi_get(ਪੂਰਨ code)
{ ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_get(code, ਇਹ); }

ਅੱਖਰ* __vpiBit::as_ਦੋਆਧਾਰੀ_ਅੰਕ_t::vpi_get_str(ਪੂਰਨ code)
{ ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_get_str(code, ਇਹ); }

ਖਾਲੀ __vpiBit::as_ਦੋਆਧਾਰੀ_ਅੰਕ_t::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val)
{ ਦੋਆਧਾਰੀ_ਅੰਕ_get_ਮੁੱਲ(ਇਹ, val); }

vpiHandle __vpiBit::as_ਦੋਆਧਾਰੀ_ਅੰਕ_t::vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags)
{ ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_put_ਮੁੱਲ(ਇਹ, val, flags); }

vpiHandle __vpiBit::as_ਦੋਆਧਾਰੀ_ਅੰਕ_t::vpi_handle(ਪੂਰਨ code)
{ ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_get_handle(code, ਇਹ); }

// FIXME: How are delayed put values handled?
//vpiHandle __vpiBit::as_bit_t::vpi_iterate(int code) // FIXME: Is this needed?
//{ return bit_iterate(code, this); }

//vpiHandle __vpiBit::as_bit_t::vpi_index(int idx) // FIXME: Is this needed?
//{ return bit_index(idx, this); }
