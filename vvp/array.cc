/*
 * Copyright (c) 2007-2026 Stephen Williams (steve@icarus.com)
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

# include  "array_common.h"
# include  "array.h"
# include  "symbols.h"
# include  "schedule.h"
# include  "vpi_priv.h"
# include  "vvp_net_sig.h"
# include  "vvp_darray.h"
# include  "config.h"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
#ਅਨ੍ਤਰ੍ਭਾਵ  "vvp_cleanup.h"
#ਜੇ_ਅੰਤ
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <iostream>
# include  "compile.h"
# include  <cassert>
# include  "ivl_alloc.h"

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_net_arrays = 0;
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_net_array_ਸ਼ਬਦ = 0;
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_var_arrays = 0;
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_var_array_ਸ਼ਬਦ = 0;
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_real_arrays = 0;
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਗਿਣਤੀ_real_array_ਸ਼ਬਦ = 0;

ਸਥਿਰ ਚਿੰਨ੍ਹ_map_s<ਬਣਤਰ __vpiArray>* array_ਸਾਰਣੀ =0;

ਵਰਗ vvp_fun_arrayport;
ਸਥਿਰ ਖਾਲੀ array_attach_ਪੋਰਟ(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_ਲੱਭੋ(ਅਚਲ ਅੱਖਰ*label)
{
      ਜੇ (array_ਸਾਰਣੀ == 0)
	    ਵਾਪਸ 0;

      vvp_array_t v = array_ਸਾਰਣੀ->sym_get_ਮੁੱਲ(label);
      ਵਾਪਸ v;
}

ਬਣਤਰ __vpiArrayVthrA : ਜਨਤਕ __vpiHandle {
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpi_ਸਮ੍ਰਿਤੀ_ਸ਼ਬਦ; }
      ਪੂਰਨ vpi_get(ਪੂਰਨ code) override;
      ਅੱਖਰ* vpi_get_str(ਪੂਰਨ code) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;
      vpiHandle vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ val, ਪੂਰਨ flags) override;
      vpiHandle vpi_handle(ਪੂਰਨ code) override;

      ਬਣਤਰ __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle ਪਤਾ_handle;
	// If wid==0, then address is the address into the array.
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪਤਾ;

      ਬਿਨਾਨਿਸ਼ਾਨੀ get_ਪਤਾ() ਅਚਲ
      {
	    ਜੇ (ਪਤਾ_handle) {
		  s_vpi_ਮੁੱਲ vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpi_ਸਦਿਸ਼_Val;
		  ਪਤਾ_handle->vpi_get_ਮੁੱਲ(&vp);
		  ਪੂਰਨ ਸ਼ਬਦ = (ਪਤਾ_handle->vpi_get(vpi_ਮਾਪ)-1)/32 + 1;
		  ਲਈ(ਪੂਰਨ idx = 0; idx < ਸ਼ਬਦ; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			ਜੇ (vp.value.vector[idx].bval != 0) ਵਾਪਸ UINT_MAX;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  ਪਤਾ_handle->vpi_get_ਮੁੱਲ(&vp);
		  ਵਾਪਸ vp.value.integer;
	    }

	    ਵਾਪਸ ਪਤਾ;
      }
};


ਬਣਤਰ __vpiArrayVthrAPV : ਜਨਤਕ __vpiHandle {
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpiPartSelect; }
      ਪੂਰਨ vpi_get(ਪੂਰਨ code) override;
      ਅੱਖਰ* vpi_get_str(ਪੂਰਨ code) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;
      vpiHandle vpi_handle(ਪੂਰਨ code) override;

      ਬਣਤਰ __vpiArray*array;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸ਼ਬਦ_sel;
      ਬਿਨਾਨਿਸ਼ਾਨੀ part_ਦੋਆਧਾਰੀ_ਅੰਕ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ part_wid;
};

ਤਾਰਕਿਕ is_net_array(vpiHandle obj)
{
      ਬਣਤਰ __vpiArray*rfp = dynamic_cast<__vpiArray*> (obj);
      ਪ੍ਰਤਿਜ੍ਞਾ(rfp);

      ਜੇ (rfp->nets != 0) ਵਾਪਸ ਸੱਚ;
      ਵਾਪਸ ਝੂਠ;
}

// This function return true if the underlying array words are real.
ਸਥਿਰ ਤਾਰਕਿਕ vpi_array_is_real(ਅਚਲ vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      ਜੇ (arr->vals4 != 0)  // A bit based variable/register array.
	    ਵਾਪਸ ਝੂਠ;

      ਜੇ (dynamic_cast<vvp_darray_real*> (arr->vals))
	    ਵਾਪਸ ਸੱਚ;

      ਜੇ (arr->vals != 0)
	    ਵਾਪਸ ਝੂਠ;

	// This must be a net array so look at element 0 to find the type.
      ਪ੍ਰਤਿਜ੍ਞਾ(arr->nets != 0);
      ਪ੍ਰਤਿਜ੍ਞਾ(arr->get_ਮਾਪ() > 0);
      ਅਚਲ ਬਣਤਰ __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      ਜੇ (rsig) {
	    ਵਾਪਸ ਸੱਚ;
      }

      ਵਾਪਸ ਝੂਠ;
}

ਸਥਿਰ ਤਾਰਕਿਕ vpi_array_is_string(ਅਚਲ vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      ਜੇ (arr->vals4 != 0)  // A bit based variable/register array.
	    ਵਾਪਸ ਝੂਠ;

      ਜੇ (dynamic_cast<vvp_darray_string*> (arr->vals))
	    ਵਾਪਸ ਸੱਚ;

      ਵਾਪਸ ਝੂਠ;
}

ਪੂਰਨ __vpiArray::get_ਸ਼ਬਦ_ਮਾਪ() ਅਚਲ
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ width;

      ਪ੍ਰਤਿਜ੍ਞਾ(get_ਮਾਪ() > 0);
	/* For a net array we need to get the width from the first element. */
      ਜੇ (nets) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(vals4 == 0 && vals == 0);
	    ਅਚਲ ਬਣਤਰ __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    ਪ੍ਰਤਿਜ੍ਞਾ(vsig);
	    width = vpip_ਮਾਪ(vsig);
	/* For a variable array we can get the width from vals_width. */
      } ਨਹੀਂਤਾਂ {
	    ਪ੍ਰਤਿਜ੍ਞਾ(vals4 || vals);
	    width = vals_ਚੌੜਾਈ;
      }

      ਵਾਪਸ width;
}

ਅੱਖਰ*__vpiArray::get_ਸ਼ਬਦ_str(ਬਣਤਰ __vpiArrayWord*ਸ਼ਬਦ_2, ਪੂਰਨ code)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ = ਸ਼ਬਦ_2->get_ਸੂਚਕਾਂਕ();

      ਜੇ (code == vpiFile) {  // Not implemented for now!
	    ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(file_ਨਾਂ[0]);
      }

      ਅੱਖਰ sidx [64];
      snprintf(sidx, 63, "%d", (ਪੂਰਨ)ਸੂਚਕਾਂਕ + ਪਹਿਲਾ_addr.get_ਮੁੱਲ());
      ਵਾਪਸ generic_get_str(code, get_scope(), name, sidx);
}

ਖਾਲੀ __vpiArray::get_ਸ਼ਬਦ_ਮੁੱਲ(ਬਣਤਰ __vpiArrayWord*ਸ਼ਬਦ_2, p_vpi_ਮੁੱਲ vp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ = ਸ਼ਬਦ_2->get_ਸੂਚਕਾਂਕ();

    // Determine the appropriate format (The Verilog PLI Handbook 5.2.10)
      ਜੇ(vp->format == vpiObjTypeVal) {
          ਜੇ(vpi_array_is_real(ਇਹ))
              vp->format = vpiRealVal;
          ਨਹੀਂਤਾਂ ਜੇ(vpi_array_is_string(ਇਹ))
              vp->format = vpiStringVal;
          ਨਹੀਂਤਾਂ
              vp->format = vpiIntVal;
      }

      ਜੇ(vals4) {
          vpip_vec4_get_ਮੁੱਲ(vals4->get_ਸ਼ਬਦ(ਸੂਚਕਾਂਕ),
                              vals4->width(), signed_flag, vp);
      } ਨਹੀਂਤਾਂ ਜੇ(vals) {
          ਚੋਣ(vp->format) {
            ਹਾਲਤ vpiBinStrVal:
            ਹਾਲਤ vpiOctStrVal:
            ਹਾਲਤ vpiDecStrVal:
            ਹਾਲਤ vpiHexStrVal:
            ਹਾਲਤ vpiScalarVal:
            ਹਾਲਤ vpiIntVal:
            ਹਾਲਤ vpi_ਸਦਿਸ਼_Val:
            {
                vvp_vector4_t v;
                vals->get_ਸ਼ਬਦ(ਸੂਚਕਾਂਕ, v);
                vpip_vec4_get_ਮੁੱਲ(v, vals_ਚੌੜਾਈ, signed_flag, vp);
            }
            ਰੋਕੋ;

            ਹਾਲਤ vpiRealVal:
            {
                ਦੁੱਗਣਾ d;
                vals->get_ਸ਼ਬਦ(ਸੂਚਕਾਂਕ, d);
                vpip_real_get_ਮੁੱਲ(d, vp);
            }
            ਰੋਕੋ;

            ਹਾਲਤ vpiStringVal:
            {
                string s;
                vals->get_ਸ਼ਬਦ(ਸੂਚਕਾਂਕ, s);
                vpip_string_get_ਮੁੱਲ(s, vp);
            }
            ਰੋਕੋ;

            ਮੂਲ:
                fprintf(stderr, "vpi sorry: format is not implemented\n");
                ਪ੍ਰਤਿਜ੍ਞਾ(ਝੂਠ);
          }
      }
}

ਖਾਲੀ __vpiArray::put_ਸ਼ਬਦ_ਮੁੱਲ(ਬਣਤਰ __vpiArrayWord*ਸ਼ਬਦ_2, p_vpi_ਮੁੱਲ vp, ਪੂਰਨ)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ = ਸ਼ਬਦ_2->get_ਸੂਚਕਾਂਕ();
      vvp_vector4_t val = vec4_from_vpi_ਮੁੱਲ(vp, vals_ਚੌੜਾਈ);
      ਸੈੱਟ_ਸ਼ਬਦ(ਸੂਚਕਾਂਕ, 0, val);
}

vpiHandle __vpiArray::get_iter_ਸੂਚਕਾਂਕ(ਬਣਤਰ __vpiArrayIterator*, ਪੂਰਨ idx)
{
      ਜੇ (nets) ਵਾਪਸ nets[idx];

      ਪ੍ਰਤਿਜ੍ਞਾ(vals4 || vals);

      ਜੇ (vals_ਸ਼ਬਦ == 0) make_vals_ਸ਼ਬਦ();

      ਵਾਪਸ &(vals_ਸ਼ਬਦ[idx].as_ਸ਼ਬਦ);
}

ਪੂਰਨ __vpiArray::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_ਸਤਰ_No:
	    ਵਾਪਸ 0; // Not implemented for now!

	  ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ get_ਮਾਪ();

	  ਹਾਲਤ vpiAutomatic:
	    ਵਾਪਸ scope->is_automatic()? 1 : 0;

	  ਮੂਲ:
	    ਵਾਪਸ 0;
      }
}

ਅੱਖਰ* __vpiArray::vpi_get_str(ਪੂਰਨ code)
{
      ਜੇ (code == vpiFile) {  // Not implemented for now!
            ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(file_ਨਾਂ[0]);
      }

      ਵਾਪਸ generic_get_str(code, scope, name, ਨਿਰਾ);
}

vpiHandle __vpiArray::vpi_handle(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
          ਹਾਲਤ vpi_ਖੱਬਾ_Range:
            ਜੇ (ਅਦਲਾ_ਬਦਲੀ_addr) ਵਾਪਸ &ਆਖਰੀ_addr;
            ਨਹੀਂਤਾਂ ਵਾਪਸ &ਪਹਿਲਾ_addr;

          ਹਾਲਤ vpi_ਸੱਜਾ_Range:
            ਜੇ (ਅਦਲਾ_ਬਦਲੀ_addr) ਵਾਪਸ &ਪਹਿਲਾ_addr;
            ਨਹੀਂਤਾਂ ਵਾਪਸ &ਆਖਰੀ_addr;

	  ਹਾਲਤ vpiScope:
	    ਵਾਪਸ scope;

	  ਹਾਲਤ vpiModule:
	    ਵਾਪਸ vpip_module(scope);
      }

      ਵਾਪਸ 0;
}

/*
* VPI code passes indices that are not yet converted to canonical
* form, so this index function does it here.
*/
vpiHandle __vpiArray::vpi_ਸੂਚਕਾਂਕ(ਪੂਰਨ ਸੂਚਕਾਂਕ)
{
      ਸੂਚਕਾਂਕ -= ਪਹਿਲਾ_addr.get_ਮੁੱਲ();
      ਜੇ (ਸੂਚਕਾਂਕ >= (ਲੰਮਾ) get_ਮਾਪ())
	    ਵਾਪਸ 0;
      ਜੇ (ਸੂਚਕਾਂਕ < 0)
	    ਵਾਪਸ 0;

      ਜੇ (nets != 0) {
	    ਵਾਪਸ nets[ਸੂਚਕਾਂਕ];
      }

      ਜੇ (vals_ਸ਼ਬਦ == 0)
	    make_vals_ਸ਼ਬਦ();

      ਵਾਪਸ &(vals_ਸ਼ਬਦ[ਸੂਚਕਾਂਕ].as_ਸ਼ਬਦ);
}

ਪੂਰਨ __vpiArrayWord::as_ਸ਼ਬਦ_t::vpi_get(ਪੂਰਨ code)
{
      ਬਣਤਰ __vpiArrayWord*obj = array_var_ਸ਼ਬਦ_from_handle(ਇਹ);
      ਪ੍ਰਤਿਜ੍ਞਾ(obj);
      ਬਣਤਰ __vpiArrayBase*my_ਮਾਪਾ = obj->get_ਮਾਪਾ();
      t_vpi_ਮੁੱਲ val;

      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_ਸਤਰ_No:
	    ਵਾਪਸ 0; // Not implemented for now!

	  ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ my_ਮਾਪਾ->get_ਸ਼ਬਦ_ਮਾਪ();

	  ਹਾਲਤ vpi_ਖੱਬਾ_Range:
            val.format = vpiIntVal;
	    my_ਮਾਪਾ->get_ਖੱਬਾ_range()->vpi_get_ਮੁੱਲ(&val);
            ਪ੍ਰਤਿਜ੍ਞਾ(val.format == vpiIntVal);
            ਵਾਪਸ val.value.integer;

	  ਹਾਲਤ vpi_ਸੱਜਾ_Range:
            val.format = vpiIntVal;
	    my_ਮਾਪਾ->get_ਸੱਜਾ_range()->vpi_get_ਮੁੱਲ(&val);
            ਪ੍ਰਤਿਜ੍ਞਾ(val.format == vpiIntVal);
            ਵਾਪਸ val.value.integer;

	  ਹਾਲਤ vpi_ਸੂਚਕਾਂਕ_2:
	    {
		  ਪੂਰਨ ਅਧਾਰ_ਵਿੱਥ = 0;
		  ਬਣਤਰ __vpiArray*ਅਧਾਰ = dynamic_cast<__vpiArray*> (my_ਮਾਪਾ);
		  ਜੇ (ਅਧਾਰ) {
			val.format = vpiIntVal;
			ਅਧਾਰ->ਪਹਿਲਾ_addr.vpi_get_ਮੁੱਲ(&val);
			ਅਧਾਰ_ਵਿੱਥ += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_ਸੂਚਕਾਂਕ.vpi_get_ਮੁੱਲ(&val);
		  ਪ੍ਰਤਿਜ੍ਞਾ(val.format == vpiIntVal);
		  ਵਾਪਸ val.value.integer + ਅਧਾਰ_ਵਿੱਥ;
	    }

	  ਹਾਲਤ vpiAutomatic:
	    ਵਾਪਸ my_ਮਾਪਾ->get_scope()->is_automatic()? 1 : 0;

#ਜੇ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ਹਾਲਤ _vpiFromThr:
	    ਵਾਪਸ _vpiNoThr;
#ਜੇ_ਅੰਤ

	  ਮੂਲ:
	    ਵਾਪਸ 0;
      }
}


ਪੂਰਨ __vpiArrayVthrA::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_ਸਤਰ_No:
	    ਵਾਪਸ 0; // Not implemented for now!

	  ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ array->get_ਸ਼ਬਦ_ਮਾਪ();

	  ਹਾਲਤ vpi_ਖੱਬਾ_Range:
	    ਵਾਪਸ array->msb.get_ਮੁੱਲ();

	  ਹਾਲਤ vpi_ਸੱਜਾ_Range:
	    ਵਾਪਸ array->lsb.get_ਮੁੱਲ();

	  ਹਾਲਤ vpi_ਸੂਚਕਾਂਕ_2:
	    ਵਾਪਸ (ਪੂਰਨ)get_ਪਤਾ() + array->ਪਹਿਲਾ_addr.get_ਮੁੱਲ();

	  ਹਾਲਤ vpiAutomatic:
	    ਵਾਪਸ array->get_scope()->is_automatic() ? 1 : 0;

#ਜੇ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ਹਾਲਤ _vpiFromThr:
	    ਵਾਪਸ _vpi_at_A;
#ਜੇ_ਅੰਤ

	  // If address_handle is not zero we definitely have a
	  // variable.
	  ਹਾਲਤ vpiConstantSelect:
	    ਵਾਪਸ ਪਤਾ_handle == 0;

	  ਮੂਲ:
	    ਵਾਪਸ 0;
      }
}

ਅੱਖਰ* __vpiArrayVthrA::vpi_get_str(ਪੂਰਨ code)
{
      ਜੇ (code == vpiFile) {  // Not implemented for now!
            ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(file_ਨਾਂ[0]);
      }

      ਅੱਖਰ sidx [64];
      snprintf(sidx, 63, "%d", (ਪੂਰਨ)get_ਪਤਾ() + array->ਪਹਿਲਾ_addr.get_ਮੁੱਲ());
      ਵਾਪਸ generic_get_str(code, array->get_scope(), array->name, sidx);
}

ਖਾਲੀ __vpiArrayVthrA::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ vp)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(array);

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ = get_ਪਤਾ();
      ਜੇ (vpi_array_is_real(array)) {
	    ਦੁੱਗਣਾ tmp = array->get_ਸ਼ਬਦ_r(ਸੂਚਕਾਂਕ);
	    vpip_real_get_ਮੁੱਲ(tmp, vp);
      } ਨਹੀਂਤਾਂ ਜੇ (vpi_array_is_string(array)) {
	    string tmp = array->get_ਸ਼ਬਦ_str(ਸੂਚਕਾਂਕ);
	    vpip_string_get_ਮੁੱਲ(tmp, vp);
      } ਨਹੀਂਤਾਂ {
	    vvp_vector4_t tmp = array->get_ਸ਼ਬਦ(ਸੂਚਕਾਂਕ);
	    ਬਿਨਾਨਿਸ਼ਾਨੀ width = array->get_ਸ਼ਬਦ_ਮਾਪ();
	    vpip_vec4_get_ਮੁੱਲ(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ vp, ਪੂਰਨ)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ = get_ਪਤਾ();

      ਪ੍ਰਤਿਜ੍ਞਾ(array);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਸੂਚਕਾਂਕ < array->get_ਮਾਪ());

      ਜੇ (vpi_array_is_real(array)) {
	    ਦੁੱਗਣਾ val = real_from_vpi_ਮੁੱਲ(vp);
	    array->ਸੈੱਟ_ਸ਼ਬਦ(ਸੂਚਕਾਂਕ, val);
      } ਨਹੀਂਤਾਂ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ width = array->get_ਸ਼ਬਦ_ਮਾਪ();
	    vvp_vector4_t val = vec4_from_vpi_ਮੁੱਲ(vp, width);
	    array->ਸੈੱਟ_ਸ਼ਬਦ(ਸੂਚਕਾਂਕ, 0, val);
      }

      ਵਾਪਸ ਇਹ;
}

vpiHandle __vpiArrayVthrA::vpi_handle(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_ਸੂਚਕਾਂਕ_2:
	    ਰੋਕੋ;  // Not implemented!

	  ਹਾਲਤ vpi_ਖੱਬਾ_Range:
	    ਵਾਪਸ &array->msb;

	  ਹਾਲਤ vpi_ਸੱਜਾ_Range:
	    ਵਾਪਸ &array->lsb;

	  ਹਾਲਤ vpi_ਮਾਪਾ:
	  ਹਾਲਤ vpiArray:
	    ਵਾਪਸ array;

	  ਹਾਲਤ vpiScope:
	    ਵਾਪਸ array->get_scope();

	  ਹਾਲਤ vpiModule:
	    ਵਾਪਸ vpip_module(array->get_scope());
      }

      ਵਾਪਸ 0;
}


ਪੂਰਨ __vpiArrayVthrAPV::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_ਸਤਰ_No:
	    ਵਾਪਸ 0; // Not implemented for now!

	  ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ part_wid;

	  ਹਾਲਤ vpi_ਖੱਬਾ_Range:
	    ਵਾਪਸ part_ਦੋਆਧਾਰੀ_ਅੰਕ + part_wid - 1;

	  ਹਾਲਤ vpi_ਸੱਜਾ_Range:
	    ਵਾਪਸ part_ਦੋਆਧਾਰੀ_ਅੰਕ;

	  ਹਾਲਤ vpiAutomatic:
	    ਵਾਪਸ array->get_scope()->is_automatic() ? 1 : 0;

#ਜੇ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ਹਾਲਤ _vpiFromThr:
	    ਵਾਪਸ _vpi_at_APV;
#ਜੇ_ਅੰਤ

	  ਹਾਲਤ vpiConstantSelect:
	    ਵਾਪਸ 1;

	  ਮੂਲ:
	    ਵਾਪਸ 0;
      }
}

ਅੱਖਰ* __vpiArrayVthrAPV::vpi_get_str(ਪੂਰਨ code)
{
      ਜੇ (code == vpiFile) {  // Not implemented for now!
            ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(file_ਨਾਂ[0]);
      }

      ਅੱਖਰ sidx [64];
      snprintf(sidx, 63, "%u", ਸ਼ਬਦ_sel + array->ਪਹਿਲਾ_addr.get_ਮੁੱਲ());
      ਵਾਪਸ generic_get_str(code, array->get_scope(), array->name, sidx);
}

ਖਾਲੀ __vpiArrayVthrAPV::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ vp)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(array);

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ = ਸ਼ਬਦ_sel;
      ਜੇ (vpi_array_is_real(array)) {
	    ਦੁੱਗਣਾ tmp = array->get_ਸ਼ਬਦ_r(ਸੂਚਕਾਂਕ);
	    vpip_real_get_ਮੁੱਲ(tmp, vp);
      } ਨਹੀਂਤਾਂ {
	    vvp_vector4_t tmp = array->get_ਸ਼ਬਦ(ਸੂਚਕਾਂਕ);
	    tmp = tmp.subvalue(part_ਦੋਆਧਾਰੀ_ਅੰਕ, part_wid);
	    vpip_vec4_get_ਮੁੱਲ(tmp, part_wid, ਝੂਠ, vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  ਹਾਲਤ vpi_ਮਾਪਾ:
	    ਵਾਪਸ 0;

            // Not part of the Verilog standard. We use this internally.
	  ਹਾਲਤ vpiArray:
	    ਵਾਪਸ array;

	  ਹਾਲਤ vpiScope:
	    ਵਾਪਸ array->get_scope();

	  ਹਾਲਤ vpiModule:
	    ਵਾਪਸ vpip_module(array->get_scope());
      }

      ਵਾਪਸ 0;
}

ਖਾਲੀ __vpiArray::ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪਤਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ part_off, ਅਚਲ vvp_vector4_t&val)
{
      ਜੇ (ਪਤਾ >= get_ਮਾਪ())
	    ਵਾਪਸ;

      ਜੇ (vals4) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(nets == 0);
	    ਜੇ (part_off != 0 || val.size() != vals_ਚੌੜਾਈ) {
		  vvp_vector4_t tmp = vals4->get_ਸ਼ਬਦ(ਪਤਾ);
		  ਜੇ ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_ਚੌੜਾਈ << ਸਤਰ_ਅੰਤ;
			ਪ੍ਰਤਿਜ੍ਞਾ(0);
		  }
		  tmp.ਸੈੱਟ_vec(part_off, val);
		  vals4->ਸੈੱਟ_ਸ਼ਬਦ(ਪਤਾ, tmp);
	    } ਨਹੀਂਤਾਂ {
		  vals4->ਸੈੱਟ_ਸ਼ਬਦ(ਪਤਾ, val);
	    }
	    ਸ਼ਬਦ_change(ਪਤਾ);
	    ਵਾਪਸ;
      }

      ਜੇ (vals) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(nets == 0);
	    ਜੇ (part_off != 0 || val.size() != vals_ਚੌੜਾਈ) {
		  vvp_vector4_t tmp;
		  vals->get_ਸ਼ਬਦ(ਪਤਾ, tmp);
		  ਜੇ ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_ਚੌੜਾਈ << ਸਤਰ_ਅੰਤ;
			ਪ੍ਰਤਿਜ੍ਞਾ(0);
		  }
		  tmp.ਸੈੱਟ_vec(part_off, val);
		  vals->ਸੈੱਟ_ਸ਼ਬਦ(ਪਤਾ, tmp);
	    } ਨਹੀਂਤਾਂ {
		  vals->ਸੈੱਟ_ਸ਼ਬਦ(ਪਤਾ, val);
	    }
	    ਸ਼ਬਦ_change(ਪਤਾ);
	    ਵਾਪਸ;
      }

      ਪ੍ਰਤਿਜ੍ਞਾ(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle ਸ਼ਬਦ_2 = nets[ਪਤਾ];
      ਅਚਲ ਬਣਤਰ __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(ਸ਼ਬਦ_2);
      ਪ੍ਰਤਿਜ੍ਞਾ(vsig);

      vsig->ਗੰਢ->send_vec4_pv(val, part_off, vpip_ਮਾਪ(vsig), 0);
      ਸ਼ਬਦ_change(ਪਤਾ);
}

ਖਾਲੀ __vpiArray::ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪਤਾ, ਦੁੱਗਣਾ val)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(vals != 0);
      ਪ੍ਰਤਿਜ੍ਞਾ(nets == 0);

      ਜੇ (ਪਤਾ >= vals->get_ਮਾਪ())
	    ਵਾਪਸ;

      vals->ਸੈੱਟ_ਸ਼ਬਦ(ਪਤਾ, val);
      ਸ਼ਬਦ_change(ਪਤਾ);
}

ਖਾਲੀ __vpiArray::ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪਤਾ, ਅਚਲ string&val)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(vals != 0);
      ਪ੍ਰਤਿਜ੍ਞਾ(nets == 0);

      ਜੇ (ਪਤਾ >= vals->get_ਮਾਪ())
	    ਵਾਪਸ;

      vals->ਸੈੱਟ_ਸ਼ਬਦ(ਪਤਾ, val);
      ਸ਼ਬਦ_change(ਪਤਾ);
}

ਖਾਲੀ __vpiArray::ਸੈੱਟ_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪਤਾ, ਅਚਲ vvp_object_t&val)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(vals != 0);
      ਪ੍ਰਤਿਜ੍ਞਾ(nets == 0);

      ਜੇ (ਪਤਾ >= vals->get_ਮਾਪ())
	    ਵਾਪਸ;

      vals->ਸੈੱਟ_ਸ਼ਬਦ(ਪਤਾ, val);
      ਸ਼ਬਦ_change(ਪਤਾ);
}

vvp_vector4_t __vpiArray::get_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪਤਾ)
{
      ਜੇ (vals4) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(nets == 0);
	    ਪ੍ਰਤਿਜ੍ਞਾ(vals == 0);
	    ਵਾਪਸ vals4->get_ਸ਼ਬਦ(ਪਤਾ);
      }

      ਜੇ (vals) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(nets == 0);
	    ਪ੍ਰਤਿਜ੍ਞਾ(vals4== 0);
	    ਜੇ (ਪਤਾ >= vals->get_ਮਾਪ())
		  ਵਾਪਸ vvp_vector4_t(vals_ਚੌੜਾਈ, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_ਸ਼ਬਦ(ਪਤਾ, val);
	    ਵਾਪਸ val;
      }

      ਪ੍ਰਤਿਜ੍ਞਾ(vals4 == 0);
      ਪ੍ਰਤਿਜ੍ਞਾ(vals == 0);
      ਪ੍ਰਤਿਜ੍ਞਾ(nets != 0);

      ਜੇ (ਪਤਾ >= get_ਮਾਪ()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    ਪ੍ਰਤਿਜ੍ਞਾ(get_ਮਾਪ() > 0);
	    vpiHandle ਸ਼ਬਦ_2 = nets[0];
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਸ਼ਬਦ_2);
	    ਬਣਤਰ __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(ਸ਼ਬਦ_2);
	    ਪ੍ਰਤਿਜ੍ਞਾ(vsig);
	    vvp_signal_ਮੁੱਲ*sig = dynamic_cast<vvp_signal_ਮੁੱਲ*> (vsig->ਗੰਢ->fil);
	    ਪ੍ਰਤਿਜ੍ਞਾ(sig);
	    ਵਾਪਸ vvp_vector4_t(sig->ਮੁੱਲ_ਮਾਪ(), BIT4_X);
      }

      vpiHandle ਸ਼ਬਦ_2 = nets[ਪਤਾ];
      ਬਣਤਰ __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(ਸ਼ਬਦ_2);
      ਪ੍ਰਤਿਜ੍ਞਾ(vsig);
      vvp_signal_ਮੁੱਲ*sig = dynamic_cast<vvp_signal_ਮੁੱਲ*> (vsig->ਗੰਢ->fil);
      ਪ੍ਰਤਿਜ੍ਞਾ(sig);

      vvp_vector4_t val;
      sig->vec4_ਮੁੱਲ(val);
      ਵਾਪਸ val;
}

ਦੁੱਗਣਾ __vpiArray::get_ਸ਼ਬਦ_r(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪਤਾ)
{
      ਜੇ (vals) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(vals4 == 0);
	    ਪ੍ਰਤਿਜ੍ਞਾ(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    ਜੇ (ਪਤਾ >= vals->get_ਮਾਪ())
		  ਵਾਪਸ 0.0;

	    ਦੁੱਗਣਾ val;
	    vals->get_ਸ਼ਬਦ(ਪਤਾ, val);
	    ਵਾਪਸ val;
      }

      ਜੇ (ਪਤਾ >= get_ਮਾਪ())
	    ਵਾਪਸ 0.0;

      ਪ੍ਰਤਿਜ੍ਞਾ(nets);
      vpiHandle ਸ਼ਬਦ_2 = nets[ਪਤਾ];
      ਬਣਤਰ __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(ਸ਼ਬਦ_2);
      ਪ੍ਰਤਿਜ੍ਞਾ(vsig);
      vvp_signal_ਮੁੱਲ*sig = dynamic_cast<vvp_signal_ਮੁੱਲ*> (vsig->net->fil);
      ਪ੍ਰਤਿਜ੍ਞਾ(sig);

      ਦੁੱਗਣਾ val = sig->real_ਮੁੱਲ();
      ਵਾਪਸ val;
}

ਖਾਲੀ __vpiArray::get_ਸ਼ਬਦ_obj(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪਤਾ, vvp_object_t&val)
{
      ਜੇ (vals) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(vals4 == 0);
	    ਪ੍ਰਤਿਜ੍ਞਾ(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    ਜੇ (ਪਤਾ >= vals->get_ਮਾਪ()) {
		  val = vvp_object_t();
		  ਵਾਪਸ;
	    }

	    vals->get_ਸ਼ਬਦ(ਪਤਾ, val);
	    ਵਾਪਸ;
      }

      ਪ੍ਰਤਿਜ੍ਞਾ(nets);
	// Arrays of string nets not implemented!
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ;
}

string __vpiArray::get_ਸ਼ਬਦ_str(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪਤਾ)
{
      ਜੇ (vals) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(vals4 == 0);
	    ਪ੍ਰਤਿਜ੍ਞਾ(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    ਜੇ (ਪਤਾ >= vals->get_ਮਾਪ())
		  ਵਾਪਸ "";

	    string val;
	    vals->get_ਸ਼ਬਦ(ਪਤਾ, val);
	    ਵਾਪਸ val;
      }

      ਪ੍ਰਤਿਜ੍ਞਾ(nets);
	// Arrays of string nets not implemented!
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
      ਵਾਪਸ "";
}

vpiHandle vpip_make_array(ਅਚਲ ਅੱਖਰ*label, ਅਚਲ ਅੱਖਰ*name,
                          ਪੂਰਨ ਪਹਿਲਾ_addr, ਪੂਰਨ ਆਖਰੀ_addr,
                          ਤਾਰਕਿਕ signed_flag)
{
      ਬਣਤਰ __vpiArray*obj = ਨਵਾਂ __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      ਜੇ (ਆਖਰੀ_addr >= ਪਹਿਲਾ_addr) {
	    obj->ਅਦਲਾ_ਬਦਲੀ_addr = ਝੂਠ;
      } ਨਹੀਂਤਾਂ {
	    obj->ਅਦਲਾ_ਬਦਲੀ_addr = ਸੱਚ;
	    ਪੂਰਨ tmp = ਆਖਰੀ_addr;
	    ਆਖਰੀ_addr = ਪਹਿਲਾ_addr;
	    ਪਹਿਲਾ_addr = tmp;
      }
      ਪ੍ਰਤਿਜ੍ਞਾ(ਆਖਰੀ_addr >= ਪਹਿਲਾ_addr);
      ਬਿਨਾਨਿਸ਼ਾਨੀ array_ਗਿਣਤੀ = ਆਖਰੀ_addr+1-ਪਹਿਲਾ_addr;

	// For now, treat all arrays as memories. This is not quite
	// correct, as arrays are arrays with memories a special case.
      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_ਗਿਣਤੀ = array_ਗਿਣਤੀ;

      obj->ਪਹਿਲਾ_addr.ਸੈੱਟ_ਮੁੱਲ(ਪਹਿਲਾ_addr);
      obj->ਆਖਰੀ_addr .ਸੈੱਟ_ਮੁੱਲ(ਆਖਰੀ_addr);

	// Start off now knowing if we are nets or variables.
      obj->nets = 0;
      obj->vals4 = 0;
      obj->vals  = 0;
      obj->vals_ਚੌੜਾਈ = 0;
      obj->vals_ਸ਼ਬਦ = 0;

	// Initialize (clear) the read-ports list.
      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

	/* Add this symbol to the array_symbols table for later lookup. */
      ਜੇ (!array_ਸਾਰਣੀ)
	    array_ਸਾਰਣੀ = ਨਵਾਂ ਚਿੰਨ੍ਹ_map_s<ਬਣਤਰ __vpiArray>;

      ਪ੍ਰਤਿਜ੍ਞਾ(!array_ਲੱਭੋ(label));
      array_ਸਾਰਣੀ->sym_ਸੈੱਟ_ਮੁੱਲ(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_ਚਿੰਨ੍ਹ(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_ਮੰਜ਼ਿਲ_current_scope(obj);

      ਵਾਪਸ obj;
}

ਖਾਲੀ __vpiArray::alias_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr, vpiHandle ਸ਼ਬਦ_2, ਪੂਰਨ msb_, ਪੂਰਨ lsb_)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(msb.get_ਮੁੱਲ() == msb_);
      ਪ੍ਰਤਿਜ੍ਞਾ(lsb.get_ਮੁੱਲ() == lsb_);
      ਪ੍ਰਤਿਜ੍ਞਾ(addr < get_ਮਾਪ());
      ਪ੍ਰਤਿਜ੍ਞਾ(nets);
      nets[addr] = ਸ਼ਬਦ_2;
}

ਖਾਲੀ __vpiArray::attach_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ addr, vpiHandle ਸ਼ਬਦ_2)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(addr < get_ਮਾਪ());
      ਪ੍ਰਤਿਜ੍ਞਾ(nets);
      nets[addr] = ਸ਼ਬਦ_2;

      ਜੇ (ਬਣਤਰ __vpiSignal*sig = dynamic_cast<__vpiSignal*>(ਸ਼ਬਦ_2)) {
	    vvp_net_t*net = sig->ਗੰਢ;
	    ਪ੍ਰਤਿਜ੍ਞਾ(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    ਪ੍ਰਤਿਜ੍ਞਾ(fun);
	    fun->attach_as_ਸ਼ਬਦ(ਇਹ, addr);
	    sig->is_netarray = 1;
	    sig->within.ਮਾਪਾ = ਇਹ;
	    sig->id.ਸੂਚਕਾਂਕ = ਨਵਾਂ __vpiDecConst(addr + ਪਹਿਲਾ_addr.get_ਮੁੱਲ());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    ਵਾਪਸ;
      }

      ਜੇ (ਬਣਤਰ __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(ਸ਼ਬਦ_2)) {
	    vvp_net_t*net = sig->net;
	    ਪ੍ਰਤਿਜ੍ਞਾ(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    ਪ੍ਰਤਿਜ੍ਞਾ(fun);
	    fun->attach_as_ਸ਼ਬਦ(ਇਹ, addr);
	    sig->is_netarray = 1;
	    sig->within.ਮਾਪਾ = ਇਹ;
	    sig->id.ਸੂਚਕਾਂਕ = ਨਵਾਂ __vpiDecConst(addr + ਪਹਿਲਾ_addr.get_ਮੁੱਲ());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = ਸੱਚ;
	    ਵਾਪਸ;
      }
}

ਖਾਲੀ compile_var_array(ਅੱਖਰ*label, ਅੱਖਰ*name, ਪੂਰਨ ਆਖਰੀ, ਪੂਰਨ ਪਹਿਲਾ,
		   ਪੂਰਨ msb, ਪੂਰਨ lsb, ਅੱਖਰ signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, ਪਹਿਲਾ, ਆਖਰੀ,
                                      signed_flag != 0);

      ਬਣਤਰ __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_ਚੌੜਾਈ = labs(msb-lsb) + 1;
      ਜੇ (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = ਨਵਾਂ vvp_vector4array_aa(arr->vals_ਚੌੜਾਈ,
						 arr->get_ਮਾਪ());
      } ਨਹੀਂਤਾਂ {
            arr->vals4 = ਨਵਾਂ vvp_vector4array_sa(arr->vals_ਚੌੜਾਈ,
						 arr->get_ਮਾਪ());
      }
      arr->msb.ਸੈੱਟ_ਮੁੱਲ(msb);
      arr->lsb.ਸੈੱਟ_ਮੁੱਲ(lsb);

      ਗਿਣਤੀ_var_arrays += 1;
      ਗਿਣਤੀ_var_array_ਸ਼ਬਦ += arr->get_ਮਾਪ();

      ਮੁਕਤ(label);
      ਮਿਟਾਓ[] name;
}

ਖਾਲੀ compile_var2_array(ਅੱਖਰ*label, ਅੱਖਰ*name, ਪੂਰਨ ਆਖਰੀ, ਪੂਰਨ ਪਹਿਲਾ,
		   ਪੂਰਨ msb, ਪੂਰਨ lsb, ਤਾਰਕਿਕ signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, ਪਹਿਲਾ, ਆਖਰੀ, signed_flag);

      ਬਣਤਰ __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.ਸੈੱਟ_ਮੁੱਲ(msb);
      arr->lsb.ਸੈੱਟ_ਮੁੱਲ(lsb);
      arr->vals_ਚੌੜਾਈ = labs(msb-lsb) + 1;

      ਪ੍ਰਤਿਜ੍ਞਾ(! arr->nets);
      ਜੇ (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = ਨਵਾਂ vvp_darray_atom<int8_t>(arr->get_ਮਾਪ());
      } ਨਹੀਂਤਾਂ ਜੇ (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = ਨਵਾਂ vvp_darray_atom<uint8_t>(arr->get_ਮਾਪ());
      } ਨਹੀਂਤਾਂ ਜੇ (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = ਨਵਾਂ vvp_darray_atom<int16_t>(arr->get_ਮਾਪ());
      } ਨਹੀਂਤਾਂ ਜੇ (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = ਨਵਾਂ vvp_darray_atom<uint16_t>(arr->get_ਮਾਪ());
      } ਨਹੀਂਤਾਂ ਜੇ (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = ਨਵਾਂ vvp_darray_atom<int32_t>(arr->get_ਮਾਪ());
      } ਨਹੀਂਤਾਂ ਜੇ (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = ਨਵਾਂ vvp_darray_atom<uint32_t>(arr->get_ਮਾਪ());
      } ਨਹੀਂਤਾਂ ਜੇ (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = ਨਵਾਂ vvp_darray_atom<int64_t>(arr->get_ਮਾਪ());
      } ਨਹੀਂਤਾਂ ਜੇ (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = ਨਵਾਂ vvp_darray_atom<uint64_t>(arr->get_ਮਾਪ());
      } ਨਹੀਂਤਾਂ {
	    arr->vals = ਨਵਾਂ vvp_darray_vec2(arr->get_ਮਾਪ(), arr->vals_ਚੌੜਾਈ);
      }
      ਗਿਣਤੀ_var_arrays += 1;
      ਗਿਣਤੀ_var_array_ਸ਼ਬਦ += arr->get_ਮਾਪ();

      ਮੁਕਤ(label);
      ਮਿਟਾਓ[] name;
}

ਖਾਲੀ compile_real_array(ਅੱਖਰ*label, ਅੱਖਰ*name, ਪੂਰਨ ਆਖਰੀ, ਪੂਰਨ ਪਹਿਲਾ)
{
      vpiHandle obj = vpip_make_array(label, name, ਪਹਿਲਾ, ਆਖਰੀ, ਸੱਚ);

      ਬਣਤਰ __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = ਨਵਾਂ vvp_darray_real(arr->get_ਮਾਪ());
      arr->vals_ਚੌੜਾਈ = 1;

      ਗਿਣਤੀ_real_arrays += 1;
      ਗਿਣਤੀ_real_array_ਸ਼ਬਦ += arr->get_ਮਾਪ();

      ਮੁਕਤ(label);
      ਮਿਟਾਓ[] name;
}

ਖਾਲੀ compile_string_array(ਅੱਖਰ*label, ਅੱਖਰ*name, ਪੂਰਨ ਆਖਰੀ, ਪੂਰਨ ਪਹਿਲਾ)
{
      vpiHandle obj = vpip_make_array(label, name, ਪਹਿਲਾ, ਆਖਰੀ, ਸੱਚ);

      ਬਣਤਰ __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = ਨਵਾਂ vvp_darray_string(arr->get_ਮਾਪ());
      arr->vals_ਚੌੜਾਈ = 1;

      ਗਿਣਤੀ_real_arrays += 1;
      ਗਿਣਤੀ_real_array_ਸ਼ਬਦ += arr->get_ਮਾਪ();

      ਮੁਕਤ(label);
      ਮਿਟਾਓ[] name;
}

ਖਾਲੀ compile_object_array(ਅੱਖਰ*label, ਅੱਖਰ*name, ਪੂਰਨ ਆਖਰੀ, ਪੂਰਨ ਪਹਿਲਾ)
{
      vpiHandle obj = vpip_make_array(label, name, ਪਹਿਲਾ, ਆਖਰੀ, ਸੱਚ);

      ਬਣਤਰ __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = ਨਵਾਂ vvp_darray_object(arr->get_ਮਾਪ());
      arr->vals_ਚੌੜਾਈ = 1;

      ਗਿਣਤੀ_real_arrays += 1;
      ਗਿਣਤੀ_real_array_ਸ਼ਬਦ += arr->get_ਮਾਪ();

      ਮੁਕਤ(label);
      ਮਿਟਾਓ[] name;
}

ਖਾਲੀ compile_net_array(ਅੱਖਰ*label, ਅੱਖਰ*name, ਪੂਰਨ ਆਖਰੀ, ਪੂਰਨ ਪਹਿਲਾ)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, ਪਹਿਲਾ, ਆਖਰੀ, ਝੂਠ);

      ਬਣਤਰ __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(arr->get_ਮਾਪ(), ਆਕਾਰ(vpiHandle)));

      ਗਿਣਤੀ_net_arrays += 1;
      ਗਿਣਤੀ_net_array_ਸ਼ਬਦ += arr->get_ਮਾਪ();

      ਮੁਕਤ(label);
      ਮਿਟਾਓ[] name;
}

ਵਰਗ vvp_fun_arrayport  : ਜਨਤਕ vvp_net_fun_t {

    ਜਨਤਕ:
      ਸਪਸ਼ਟ vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      ਸਪਸ਼ਟ vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, ਲੰਮਾ addr);
      ~vvp_fun_arrayport() override;

      ਆਭਾਸੀ ਖਾਲੀ check_ਸ਼ਬਦ_change(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr) = 0;

    ਸੁਰੱਖਿਅਤ:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr_;

      ਮਿੱਤਰ ਖਾਲੀ array_attach_ਪੋਰਟ(vvp_array_t, vvp_fun_arrayport*);
      ਮਿੱਤਰ ਖਾਲੀ __vpiArray::ਸ਼ਬਦ_change(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ);
      vvp_fun_arrayport*ਅਗਲਾ;
};

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net)
: arr_(mem), net_(net), addr_(0)
{
      ਅਗਲਾ = 0;
}

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, ਲੰਮਾ addr)
: arr_(mem), net_(net), addr_(addr)
{
      ਅਗਲਾ = 0;
}

vvp_fun_arrayport::~vvp_fun_arrayport()
{
}

ਵਰਗ vvp_fun_arrayport_sa  : ਜਨਤਕ vvp_fun_arrayport {

    ਜਨਤਕ:
      ਸਪਸ਼ਟ vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      ਸਪਸ਼ਟ vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, ਲੰਮਾ addr);
      ~vvp_fun_arrayport_sa() override;

      ਖਾਲੀ check_ਸ਼ਬਦ_change(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr) override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

    ਨਿਜੀ:
};

vvp_fun_arrayport_sa::vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net)
{
}

vvp_fun_arrayport_sa::vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, ਲੰਮਾ addr)
: vvp_fun_arrayport(mem, net, addr)
{
}

vvp_fun_arrayport_sa::~vvp_fun_arrayport_sa()
{
}

ਖਾਲੀ vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                     vvp_context_t)
{
      ਤਾਰਕਿਕ addr_valid_flag;

      ਚੋਣ (ਪੋਰਟ.ਪੋਰਟ()) {

	  ਹਾਲਤ 0: // Address input
	    addr_valid_flag = vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਦੋਆਧਾਰੀ_ਅੰਕ, addr_);
	    ਜੇ (! addr_valid_flag)
		  addr_ = arr_->get_ਮਾਪ();
	    ਜੇ (vpi_array_is_real(arr_))
		  ਪੋਰਟ.ptr()->send_real(arr_->get_ਸ਼ਬਦ_r(addr_), 0);
	    ਨਹੀਂਤਾਂ
		  ਪੋਰਟ.ptr()->send_vec4(arr_->get_ਸ਼ਬਦ(addr_), 0);

	    ਰੋਕੋ;

	  ਮੂਲ:
	    fprintf(stdout, "XXXX write ports not implemented.\n");
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }
}

ਖਾਲੀ vvp_fun_arrayport_sa::check_ਸ਼ਬਦ_change(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr)
{
      ਜੇ (addr != addr_) ਵਾਪਸ;

      ਜੇ (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_ਸ਼ਬਦ_r(addr_), 0);
      } ਨਹੀਂਤਾਂ {
	    net_->send_vec4(arr_->get_ਸ਼ਬਦ(addr_), 0);
      }
}

ਵਰਗ vvp_fun_arrayport_aa  : ਜਨਤਕ vvp_fun_arrayport, ਜਨਤਕ automatic_hooks_s {

    ਜਨਤਕ:
      ਸਪਸ਼ਟ vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net);
      ਸਪਸ਼ਟ vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, ਲੰਮਾ addr);
      ~vvp_fun_arrayport_aa() override;

      ਖਾਲੀ alloc_instance(vvp_context_t context) override;
      ਖਾਲੀ reset_instance(vvp_context_t context) override;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਖਾਲੀ free_instance(vvp_context_t context) override;
#ਜੇ_ਅੰਤ

      ਖਾਲੀ check_ਸ਼ਬਦ_change(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr) override;

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t context) override;

    ਨਿਜੀ:
      ਖਾਲੀ check_ਸ਼ਬਦ_change_2(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr, vvp_context_t context);

      __vpiScope*context_scope_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_context(ਇਹ, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, ਲੰਮਾ addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_ਜੋੜੋ_ਵਸਤੂ_ਮੰਜ਼ਿਲ_context(ਇਹ, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

ਖਾਲੀ vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*addr = ਨਵਾਂ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ;
      vvp_ਸੈੱਟ_context_ਵਸਤੂ(context, context_idx_, addr);

      *addr = addr_;
}

ਖਾਲੀ vvp_fun_arrayport_aa::reset_instance(vvp_context_t context)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*addr = static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));

      *addr = addr_;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ vvp_fun_arrayport_aa::free_instance(vvp_context_t context)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*addr = static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));
      ਮਿਟਾਓ addr;
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                     vvp_context_t context)
{
      ਜੇ (context) {
            ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*addr = static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*>
                  (vvp_get_context_ਵਸਤੂ(context, context_idx_));

            ਤਾਰਕਿਕ addr_valid_flag;

            ਚੋਣ (ਪੋਰਟ.ਪੋਰਟ()) {

                ਹਾਲਤ 0: // Address input
                  addr_valid_flag = vector4_ਮੰਜ਼ਿਲ_ਮੁੱਲ(ਦੋਆਧਾਰੀ_ਅੰਕ, *addr);
                  ਜੇ (! addr_valid_flag) *addr = arr_->get_ਮਾਪ();
                  ਜੇ (vpi_array_is_real(arr_)) {
			ਪੋਰਟ.ptr()->send_real(arr_->get_ਸ਼ਬਦ_r(*addr),
					      context);
                  } ਨਹੀਂਤਾਂ {
			ਪੋਰਟ.ptr()->send_vec4(arr_->get_ਸ਼ਬਦ(*addr),
					      context);
                  }
                  ਰੋਕੋ;

                ਮੂਲ:
                  fprintf(stdout, "XXXX write ports not implemented.\n");
                  ਪ੍ਰਤਿਜ੍ਞਾ(0);
            }
      } ਨਹੀਂਤਾਂ {
            context = context_scope_->ਜੀਵਤ_contexts;
            ਜਦੋਂ (context) {
                  recv_vec4(ਪੋਰਟ, ਦੋਆਧਾਰੀ_ਅੰਕ, context);
                  context = vvp_get_ਅਗਲਾ_context(context);
            }
      }
}

ਖਾਲੀ vvp_fun_arrayport_aa::check_ਸ਼ਬਦ_change_2(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr,
                                              vvp_context_t context)
{
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*ਪੋਰਟ_addr = static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ*>
            (vvp_get_context_ਵਸਤੂ(context, context_idx_));

      ਜੇ (addr != *ਪੋਰਟ_addr)
	    ਵਾਪਸ;

      ਜੇ (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_ਸ਼ਬਦ_r(addr), context);
      } ਨਹੀਂਤਾਂ {
	    net_->send_vec4(arr_->get_ਸ਼ਬਦ(addr), context);
      }
}

ਖਾਲੀ vvp_fun_arrayport_aa::check_ਸ਼ਬਦ_change(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr)
{
      ਜੇ (arr_->get_scope()->is_automatic()) {
            ਪ੍ਰਤਿਜ੍ਞਾ(vthread_get_wt_context());
            check_ਸ਼ਬਦ_change_2(addr, vthread_get_wt_context());
      } ਨਹੀਂਤਾਂ {
            vvp_context_t context = context_scope_->ਜੀਵਤ_contexts;
            ਜਦੋਂ (context) {
                  check_ਸ਼ਬਦ_change_2(addr, context);
                  context = vvp_get_ਅਗਲਾ_context(context);
            }
      }
}

ਸਥਿਰ ਖਾਲੀ array_attach_ਪੋਰਟ(vvp_array_t array, vvp_fun_arrayport*fun)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(fun->ਅਗਲਾ == 0);
      fun->ਅਗਲਾ = array->ports_;
      array->ports_ = fun;
      ਜੇ (!array->get_scope()->is_automatic() &&
          (array->vals4 || array->vals)) {
              /* propagate initial values for variable arrays */
            ਜੇ (!vpi_array_is_real(array)) {
                  vvp_bit4_t init;
                  ਜੇ (array->vals4)
                      init = BIT4_X;
                  ਨਹੀਂਤਾਂ
                      init = BIT4_0;
                  vvp_vector4_t tmp(array->vals_ਚੌੜਾਈ, init);
                  ਸਮਾਂ_ਸਾਰਣੀ_init_propagate(fun->net_, tmp);
            } ਨਹੀਂਤਾਂ {
                  ਸਮਾਂ_ਸਾਰਣੀ_init_propagate(fun->net_, 0.0);
            }
      }
}

ਵਰਗ array_ਸ਼ਬਦ_ਮੁੱਲ_callback : ਜਨਤਕ ਮੁੱਲ_callback {
    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ ਸਪਸ਼ਟ array_ਸ਼ਬਦ_ਮੁੱਲ_callback(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ, ਲੰਮਾ addr)
      : ਮੁੱਲ_callback(ਅੰਕੜੇ), ਸ਼ਬਦ_addr(addr)
      { }

    ਜਨਤਕ:
      ਲੰਮਾ ਸ਼ਬਦ_addr;
};

ਖਾਲੀ __vpiArray::ਸ਼ਬਦ_change(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ addr)
{
      ਲਈ (vvp_fun_arrayport*cur = ports_; cur; cur = cur->ਅਗਲਾ)
	    cur->check_ਸ਼ਬਦ_change(addr);

	// Run callbacks attached to the array itself.
      ਬਣਤਰ __vpiCallback *ਅਗਲਾ_2 = vpi_callbacks;
      ਬਣਤਰ __vpiCallback *prev = 0;

      ਜਦੋਂ (ਅਗਲਾ_2) {
	    array_ਸ਼ਬਦ_ਮੁੱਲ_callback*cur = dynamic_cast<array_ਸ਼ਬਦ_ਮੁੱਲ_callback*>(ਅਗਲਾ_2);
	    ਅਗਲਾ_2 = cur->ਅਗਲਾ_2;

	      // Skip callbacks that are not for me. -1 is for every element.
	    ਜੇ (cur->ਸ਼ਬਦ_addr != (ਲੰਮਾ)addr && cur->ਸ਼ਬਦ_addr != -1) {
		  prev = cur;
		  ਜਾਰੀ;
	    }

	      // For whole array callbacks we need to set the index.
	    ਜੇ (cur->ਸ਼ਬਦ_addr == -1) {
		  cur->cb_ਅੰਕੜੇ.ਸੂਚਕਾਂਕ = (PLI_INT32) ((ਪੂਰਨ)addr +
						    ਪਹਿਲਾ_addr.get_ਮੁੱਲ());
	    }

	    ਜੇ (cur->cb_ਅੰਕੜੇ.cb_rtn != 0) {
		  ਜੇ (cur->test_ਮੁੱਲ_callback_ready()) {
			ਜੇ (cur->cb_ਅੰਕੜੇ.value) {
			      ਜੇ (vpi_array_is_real(ਇਹ)) {
				    ਦੁੱਗਣਾ val = 0.0;
				    ਜੇ (addr < vals->get_ਮਾਪ())
					  vals->get_ਸ਼ਬਦ(addr, val);
				    vpip_real_get_ਮੁੱਲ(val, cur->cb_ਅੰਕੜੇ.value);
			      } ਨਹੀਂਤਾਂ ਜੇ (vals4) {
				    vpip_vec4_get_ਮੁੱਲ(vals4->get_ਸ਼ਬਦ(addr),
							vals_ਚੌੜਾਈ,
							signed_flag,
							cur->cb_ਅੰਕੜੇ.value);
			      } ਨਹੀਂਤਾਂ ਜੇ (dynamic_cast<vvp_darray_atom<int8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int64_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint64_t>*>(vals)
				      || dynamic_cast<vvp_darray_vec2*>(vals)) {
				    vvp_vector4_t val;
				    ਜੇ (addr < vals->get_ਮਾਪ())
					  vals->get_ਸ਼ਬਦ(addr, val);
				    vpip_vec4_get_ਮੁੱਲ(val,
							vals_ਚੌੜਾਈ,
							signed_flag,
							cur->cb_ਅੰਕੜੇ.value);
			      } ਨਹੀਂਤਾਂ {
			            ਪ੍ਰਤਿਜ੍ਞਾ(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } ਨਹੀਂਤਾਂ ਜੇ (prev == 0) {

		  vpi_callbacks = ਅਗਲਾ_2;
		  cur->ਅਗਲਾ_2 = 0;
		  ਮਿਟਾਓ cur;

	    } ਨਹੀਂਤਾਂ {
		  ਪ੍ਰਤਿਜ੍ਞਾ(prev->ਅਗਲਾ_2 == cur);
		  prev->ਅਗਲਾ_2 = ਅਗਲਾ_2;
		  cur->ਅਗਲਾ_2 = 0;
		  ਮਿਟਾਓ cur;
	    }
      }
}

ਵਰਗ array_resolv_list_t : ਜਨਤਕ resolv_list_s {

    ਜਨਤਕ:
      ਸਪਸ਼ਟ array_resolv_list_t(ਅੱਖਰ*lab) : resolv_list_s(lab) {
	    array = 0;
      }

      vvp_array_t*array;
      ਤਾਰਕਿਕ resolve(ਤਾਰਕਿਕ mes) override;

    ਨਿਜੀ:
};

ਤਾਰਕਿਕ array_resolv_list_t::resolve(ਤਾਰਕਿਕ mes)
{
      *array = array_ਲੱਭੋ(label());
      ਜੇ (*array == 0) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(!mes);
	    ਵਾਪਸ ਝੂਠ;
      }
      ਵਾਪਸ ਸੱਚ;
}

ਵਰਗ array_ਪੋਰਟ_resolv_list_t : ਜਨਤਕ resolv_list_s {

    ਜਨਤਕ:
      ਸਪਸ਼ਟ array_ਪੋਰਟ_resolv_list_t(ਅੱਖਰ* lab, ਤਾਰਕਿਕ use_addr__,
                                        ਲੰਮਾ addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      ਤਾਰਕਿਕ use_addr;
      ਲੰਮਾ addr;
      ਤਾਰਕਿਕ resolve(ਤਾਰਕਿਕ mes) override;

    ਨਿਜੀ:
};

array_ਪੋਰਟ_resolv_list_t::array_ਪੋਰਟ_resolv_list_t(ਅੱਖਰ *lab, ਤਾਰਕਿਕ use_addr__,
                                                   ਲੰਮਾ addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      ਜੇ (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      ਨਹੀਂਤਾਂ
            context_scope = 0;
      ptr = ਨਵਾਂ vvp_net_t;
}

ਤਾਰਕਿਕ array_ਪੋਰਟ_resolv_list_t::resolve(ਤਾਰਕਿਕ mes)
{
      vvp_array_t mem = array_ਲੱਭੋ(label());
      ਜੇ (mem == 0) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(mem || !mes);
	    ਵਾਪਸ ਝੂਠ;
      }

      vvp_fun_arrayport*fun;
      ਜੇ (use_addr)
            ਜੇ (context_scope)
                  fun = ਨਵਾਂ vvp_fun_arrayport_aa(context_scope, mem, ptr, addr);
            ਨਹੀਂਤਾਂ
                  fun = ਨਵਾਂ vvp_fun_arrayport_sa(mem, ptr, addr);
      ਨਹੀਂਤਾਂ
            ਜੇ (context_scope)
                  fun = ਨਵਾਂ vvp_fun_arrayport_aa(context_scope, mem, ptr);
            ਨਹੀਂਤਾਂ
                  fun = ਨਵਾਂ vvp_fun_arrayport_sa(mem, ptr);
      ptr->fun = fun;

      array_attach_ਪੋਰਟ(mem, fun);

      ਵਾਪਸ ਸੱਚ;
}

ਵਰਗ array_ਸ਼ਬਦ_part_callback : ਜਨਤਕ array_ਸ਼ਬਦ_ਮੁੱਲ_callback {
    ਜਨਤਕ:
      ਸਪਸ਼ਟ array_ਸ਼ਬਦ_part_callback(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ, ਲੰਮਾ addr);
      ~array_ਸ਼ਬਦ_part_callback() override;

      array_ਸ਼ਬਦ_part_callback(ਅਚਲ array_ਸ਼ਬਦ_part_callback&) = ਮਿਟਾਓ;
      array_ਸ਼ਬਦ_part_callback& ਸੰਚਾਲਕ=(ਅਚਲ array_ਸ਼ਬਦ_part_callback&) = ਮਿਟਾਓ;

      ਤਾਰਕਿਕ test_ਮੁੱਲ_callback_ready(ਖਾਲੀ) override;

    ਨਿਜੀ:
      ਅੱਖਰ*ਮੁੱਲ_ਬਿੱਟ;
};

array_ਸ਼ਬਦ_part_callback::array_ਸ਼ਬਦ_part_callback(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ, ਲੰਮਾ addr)
: array_ਸ਼ਬਦ_ਮੁੱਲ_callback(ਅੰਕੜੇ, addr)
{
	// Get the initial value of the part, to use as a reference.
      ਬਣਤਰ __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(ਅੰਕੜੇ->obj);
      s_vpi_ਮੁੱਲ tmp_ਮੁੱਲ;
      tmp_ਮੁੱਲ.format = vpiBinStrVal;
      apvword->vpi_get_ਮੁੱਲ(&tmp_ਮੁੱਲ);

      ਮੁੱਲ_ਬਿੱਟ = ਨਵਾਂ ਅੱਖਰ[apvword->part_wid+1];

      ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਮੁੱਲ_ਬਿੱਟ, tmp_ਮੁੱਲ.value.str, apvword->part_wid);
      ਮੁੱਲ_ਬਿੱਟ[apvword->part_wid] = 0;
}

array_ਸ਼ਬਦ_part_callback::~array_ਸ਼ਬਦ_part_callback()
{
      ਮਿਟਾਓ[]ਮੁੱਲ_ਬਿੱਟ;
}

ਤਾਰਕਿਕ array_ਸ਼ਬਦ_part_callback::test_ਮੁੱਲ_callback_ready(ਖਾਲੀ)
{
      ਬਣਤਰ __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_ਅੰਕੜੇ.obj);
      ਪ੍ਰਤਿਜ੍ਞਾ(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_ਮੁੱਲ tmp_ਮੁੱਲ;
      tmp_ਮੁੱਲ.format = vpiBinStrVal;
      apvword->vpi_get_ਮੁੱਲ(&tmp_ਮੁੱਲ);

      ਜੇ (memcmp(ਮੁੱਲ_ਬਿੱਟ, tmp_ਮੁੱਲ.value.str, apvword->part_wid) == 0)
	    ਵਾਪਸ ਝੂਠ;

      ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਮੁੱਲ_ਬਿੱਟ, tmp_ਮੁੱਲ.value.str, apvword->part_wid);
      ਵਾਪਸ ਸੱਚ;

}

ਮੁੱਲ_callback*vpip_array_ਸ਼ਬਦ_change(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ)
{
      ਬਣਤਰ __vpiArray*ਮਾਪਾ = 0;
      array_ਸ਼ਬਦ_ਮੁੱਲ_callback*cbh = 0;
      ਜੇ (ਅਚਲ ਬਣਤਰ __vpiArrayWord*ਸ਼ਬਦ_2 = array_var_ਸ਼ਬਦ_from_handle(ਅੰਕੜੇ->obj)) {
	    ਮਾਪਾ = static_cast<__vpiArray*>(ਸ਼ਬਦ_2->get_ਮਾਪਾ());
	    ਬਿਨਾਨਿਸ਼ਾਨੀ addr = ਸ਼ਬਦ_2->get_ਸੂਚਕਾਂਕ();
	    cbh = ਨਵਾਂ array_ਸ਼ਬਦ_ਮੁੱਲ_callback(ਅੰਕੜੇ, addr);

      } ਨਹੀਂਤਾਂ ਜੇ (ਬਣਤਰ __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(ਅੰਕੜੇ->obj)) {
	    ਮਾਪਾ = tword->array;
	    cbh = ਨਵਾਂ array_ਸ਼ਬਦ_ਮੁੱਲ_callback(ਅੰਕੜੇ, tword->ਪਤਾ);

      } ਨਹੀਂਤਾਂ ਜੇ (ਬਣਤਰ __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(ਅੰਕੜੇ->obj)) {
	    ਮਾਪਾ = apvword->array;
	    cbh = ਨਵਾਂ array_ਸ਼ਬਦ_part_callback(ਅੰਕੜੇ, apvword->ਸ਼ਬਦ_sel);
      }

      ਪ੍ਰਤਿਜ੍ਞਾ(cbh);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਮਾਪਾ);
      cbh->ਅਗਲਾ_2 = ਮਾਪਾ->vpi_callbacks;
      ਮਾਪਾ->vpi_callbacks = cbh;

      ਵਾਪਸ cbh;
}

ਮੁੱਲ_callback* vpip_array_change(p_cb_ਅੰਕੜੇ ਅੰਕੜੇ)
{
      array_ਸ਼ਬਦ_ਮੁੱਲ_callback*cbh = ਨਵਾਂ array_ਸ਼ਬਦ_ਮੁੱਲ_callback(ਅੰਕੜੇ, -1);
      ਪ੍ਰਤਿਜ੍ਞਾ(ਅੰਕੜੇ->obj);

      ਬਣਤਰ __vpiArray*arr = dynamic_cast<__vpiArray*>(ਅੰਕੜੇ->obj);
      cbh->ਅਗਲਾ_2 = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      ਵਾਪਸ cbh;
}

ਖਾਲੀ compile_array_ਪੋਰਟ(ਅੱਖਰ*label, ਅੱਖਰ*array, ਅੱਖਰ*addr)
{
      array_ਪੋਰਟ_resolv_list_t*resolv_mem
	    = ਨਵਾਂ array_ਪੋਰਟ_resolv_list_t(array, ਝੂਠ, 0);

      define_functor_ਚਿੰਨ੍ਹ(label, resolv_mem->ptr);
      ਮੁਕਤ(label);
	// Connect the port-0 input as the address.
      ਨਿਵੇਸ਼_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

ਖਾਲੀ compile_array_ਪੋਰਟ(ਅੱਖਰ*label, ਅੱਖਰ*array, ਲੰਮਾ addr)
{
      array_ਪੋਰਟ_resolv_list_t*resolv_mem
	    = ਨਵਾਂ array_ਪੋਰਟ_resolv_list_t(array, ਸੱਚ, addr);

      define_functor_ਚਿੰਨ੍ਹ(label, resolv_mem->ptr);
      ਮੁਕਤ(label);

      resolv_submit(resolv_mem);
}

ਖਾਲੀ compile_array_alias(ਅੱਖਰ*label, ਅੱਖਰ*name, ਅੱਖਰ*src)
{
      vvp_array_t mem = array_ਲੱਭੋ(src);
      ਪ੍ਰਤਿਜ੍ਞਾ(mem);

      ਬਣਤਰ __vpiArray*obj = ਨਵਾਂ __vpiArray;

      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_ਗਿਣਤੀ = mem->array_ਗਿਣਤੀ;
      obj->signed_flag = mem->signed_flag;

	// Need to set an accurate range of addresses.
      obj->ਪਹਿਲਾ_addr = mem->ਪਹਿਲਾ_addr;
      obj->ਆਖਰੀ_addr  = mem->ਆਖਰੀ_addr;
      obj->ਅਦਲਾ_ਬਦਲੀ_addr = mem->ਅਦਲਾ_ਬਦਲੀ_addr;

      obj->msb = mem->msb;
      obj->lsb = mem->lsb;

	// Share the words with the source array.
      obj->nets = mem->nets;
      obj->vals4 = mem->vals4;
      obj->vals  = mem->vals;
      obj->vals_ਚੌੜਾਈ = mem->vals_ਚੌੜਾਈ;
      obj->vals_ਸ਼ਬਦ = mem->vals_ਸ਼ਬਦ;

      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

      ਪ੍ਰਤਿਜ੍ਞਾ(array_ਸਾਰਣੀ);
      ਪ੍ਰਤਿਜ੍ਞਾ(!array_ਲੱਭੋ(label));
      array_ਸਾਰਣੀ->sym_ਸੈੱਟ_ਮੁੱਲ(label, obj);

      compile_vpi_ਚਿੰਨ੍ਹ(label, obj);
      vpip_attach_ਮੰਜ਼ਿਲ_current_scope(obj);

      ਮੁਕਤ(label);
      ਮੁਕਤ(name);
      ਮੁਕਤ(src);
}

/*
 * &A<label,addr>
 * This represents a VPI handle for an addressed array. This comes
 * from expressions like "label[addr]" where "label" is the array and
 * "addr" is the canonical address of the desired word.
 */
vpiHandle vpip_make_vthr_A(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ addr)
{
      ਬਣਤਰ __vpiArrayVthrA*obj = ਨਵਾਂ __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = ਨਵਾਂ array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->ਪਤਾ_handle = 0;
      obj->ਪਤਾ = addr;

      ਵਾਪਸ obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(ਅੱਖਰ*label, ਅੱਖਰ*ਚਿੰਨ੍ਹ)
{
      ਬਣਤਰ __vpiArrayVthrA*obj = ਨਵਾਂ __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = ਨਵਾਂ array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->ਪਤਾ_handle = 0;
      compile_vpi_lookup(&obj->ਪਤਾ_handle, ਚਿੰਨ੍ਹ);
      obj->ਪਤਾ = 0;

      ਵਾਪਸ obj;
}

vpiHandle vpip_make_vthr_A(ਅੱਖਰ*label, vpiHandle handle)
{
      ਬਣਤਰ __vpiArrayVthrA*obj = ਨਵਾਂ __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = ਨਵਾਂ array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->ਪਤਾ_handle = handle;
      obj->ਪਤਾ = 0;

      ਵਾਪਸ obj;
}

vpiHandle vpip_make_vthr_APV(ਅੱਖਰ*label, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid)
{
      ਬਣਤਰ __vpiArrayVthrAPV*obj = ਨਵਾਂ __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = ਨਵਾਂ array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->ਸ਼ਬਦ_sel = ਸੂਚਕਾਂਕ;
      obj->part_ਦੋਆਧਾਰੀ_ਅੰਕ = ਦੋਆਧਾਰੀ_ਅੰਕ;
      obj->part_wid = wid;

      ਵਾਪਸ obj;
}

ਖਾਲੀ compile_array_cleanup(ਖਾਲੀ)
{
      ਮਿਟਾਓ array_ਸਾਰਣੀ;
      array_ਸਾਰਣੀ = 0;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ ਸਮ੍ਰਿਤੀ_delete(vpiHandle ਵਸਤੂ)
{
      ਬਣਤਰ __vpiArray*arr = static_cast<ਬਣਤਰ __vpiArray*>(ਵਸਤੂ);
      ਜੇ (arr->vals_ਸ਼ਬਦ) ਮਿਟਾਓ [] (arr->vals_ਸ਼ਬਦ-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      ਮਿਟਾਓ arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      ਮਿਟਾਓ arr->vals;

      ਜੇ (arr->nets) {
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < arr->get_ਮਾਪ(); idx += 1) {
		  ਜੇ (ਬਣਤਰ __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.ਸੂਚਕਾਂਕ);
		    /* These should only be the real words. */
		  } ਨਹੀਂਤਾਂ {
			ਪ੍ਰਤਿਜ੍ਞਾ(arr->nets[idx]->get_type_ਕੋਡ() ==
			       vpiRealVar);
			ਬਣਤਰ __vpiRealVar *sigr = static_cast<ਬਣਤਰ __vpiRealVar *>
			                            (arr->nets[idx]);
			constant_delete(sigr->id.ਸੂਚਕਾਂਕ);
// Why are only the real words still here?
			ਮਿਟਾਓ arr->nets[idx];
		  }
	    }
	    ਮੁਕਤ(arr->nets);
      }

      ਜਦੋਂ (arr->vpi_callbacks) {
	    ਬਣਤਰ __vpiCallback*tmp = arr->vpi_callbacks->ਅਗਲਾ_2;
	    ਮਿਟਾਓ arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      ਮਿਟਾਓ arr;
}

ਖਾਲੀ A_delete(vpiHandle ਵਸਤੂ)
{
      ਬਣਤਰ __vpiArrayVthrA*obj = static_cast<ਬਣਤਰ __vpiArrayVthrA*>(ਵਸਤੂ);
      ਜੇ (obj->ਪਤਾ_handle) {
	    ਚੋਣ (obj->ਪਤਾ_handle->get_type_ਕੋਡ()) {
		ਹਾਲਤ vpi_ਸਮ੍ਰਿਤੀ_ਸ਼ਬਦ:
		  ਜੇ (vpi_get(_vpiFromThr, obj->ਪਤਾ_handle) == _vpi_at_A) {
			A_delete(obj->ਪਤਾ_handle);
		  }
		  ਰੋਕੋ;
		ਹਾਲਤ vpiPartSelect:
		  ਪ੍ਰਤਿਜ੍ਞਾ(vpi_get(_vpiFromThr, obj->ਪਤਾ_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->ਪਤਾ_handle);
		  ਰੋਕੋ;
	    }
      }

      ਮਿਟਾਓ obj;
}

ਖਾਲੀ APV_delete(vpiHandle ਵਸਤੂ)
{
      ਬਣਤਰ __vpiArrayVthrAPV*obj = static_cast<ਬਣਤਰ __vpiArrayVthrAPV*>(ਵਸਤੂ);
      ਮਿਟਾਓ obj;
}
#ਜੇ_ਅੰਤ
