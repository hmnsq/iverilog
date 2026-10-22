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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
#ಅನ್ತರ್ಭಾವ  "vvp_cleanup.h"
#ಆದರೆ_ಅಂತ್ಯ
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <iostream>
# include  "compile.h"
# include  <cassert>
# include  "ivl_alloc.h"

ಬಳಸಿ ನಾಮಾಕಾಶ std;

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_net_arrays = 0;
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_net_array_ಪದಗಳು = 0;
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_var_arrays = 0;
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_var_array_ಪದಗಳು = 0;
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_real_arrays = 0;
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಎಣಿಕೆ_real_array_ಪದಗಳು = 0;

ಸ್ಥಿರ ಚಿಹ್ನೆ_map_s<ರಚನೆ __vpiArray>* array_ಕೋಷ್ಟಕ =0;

ವರ್ಗ vvp_fun_arrayport;
ಸ್ಥಿರ ಶೂನ್ಯ array_attach_ಪೋರ್ಟ್(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*label)
{
      ಆದರೆ (array_ಕೋಷ್ಟಕ == 0)
	    ಹಿಂತಿರುಗಿ 0;

      vvp_array_t v = array_ಕೋಷ್ಟಕ->sym_get_ಮೌಲ್ಯ(label);
      ಹಿಂತಿರುಗಿ v;
}

ರಚನೆ __vpiArrayVthrA : ಸಾರ್ವಜನಿಕ __vpiHandle {
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpi_ಸ್ಮರಣೆ_ಶಬ್ದ; }
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ code) override;
      ಅಕ್ಷರ* vpi_get_str(ಪೂರ್ಣಾಂಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಂಕ flags) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ code) override;

      ರಚನೆ __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle ವಿಳಾಸ_handle;
	// If wid==0, then address is the address into the array.
      ಚಿಹ್ನೆರಹಿತ ವಿಳಾಸ;

      ಚಿಹ್ನೆರಹಿತ get_ವಿಳಾಸ() ಸ್ಥಿರಾಂಕ
      {
	    ಆದರೆ (ವಿಳಾಸ_handle) {
		  s_vpi_ಮೌಲ್ಯ vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpi_ಸದಿಶ_Val;
		  ವಿಳಾಸ_handle->vpi_get_ಮೌಲ್ಯ(&vp);
		  ಪೂರ್ಣಾಂಕ ಪದಗಳು = (ವಿಳಾಸ_handle->vpi_get(vpi_ಗಾತ್ರ)-1)/32 + 1;
		  ಗಾಗಿ(ಪೂರ್ಣಾಂಕ idx = 0; idx < ಪದಗಳು; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			ಆದರೆ (vp.value.vector[idx].bval != 0) ಹಿಂತಿರುಗಿ UINT_MAX;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  ವಿಳಾಸ_handle->vpi_get_ಮೌಲ್ಯ(&vp);
		  ಹಿಂತಿರುಗಿ vp.value.integer;
	    }

	    ಹಿಂತಿರುಗಿ ವಿಳಾಸ;
      }
};


ರಚನೆ __vpiArrayVthrAPV : ಸಾರ್ವಜನಿಕ __vpiHandle {
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpiPartSelect; }
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ code) override;
      ಅಕ್ಷರ* vpi_get_str(ಪೂರ್ಣಾಂಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ code) override;

      ರಚನೆ __vpiArray*array;
      ಚಿಹ್ನೆರಹಿತ ಶಬ್ದ_sel;
      ಚಿಹ್ನೆರಹಿತ part_ಬಿಟ್;
      ಚಿಹ್ನೆರಹಿತ part_wid;
};

ತಾರ್ಕಿಕ is_net_array(vpiHandle obj)
{
      ರಚನೆ __vpiArray*rfp = dynamic_cast<__vpiArray*> (obj);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಆದರೆ (rfp->nets != 0) ಹಿಂತಿರುಗಿ ನಿಜ;
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

// This function return true if the underlying array words are real.
ಸ್ಥಿರ ತಾರ್ಕಿಕ vpi_array_is_real(ಸ್ಥಿರಾಂಕ vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      ಆದರೆ (arr->vals4 != 0)  // A bit based variable/register array.
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

      ಆದರೆ (dynamic_cast<vvp_darray_real*> (arr->vals))
	    ಹಿಂತಿರುಗಿ ನಿಜ;

      ಆದರೆ (arr->vals != 0)
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	// This must be a net array so look at element 0 to find the type.
      ಪ್ರತಿಜ್ಞಾ(arr->nets != 0);
      ಪ್ರತಿಜ್ಞಾ(arr->get_ಗಾತ್ರ() > 0);
      ಸ್ಥಿರಾಂಕ ರಚನೆ __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      ಆದರೆ (rsig) {
	    ಹಿಂತಿರುಗಿ ನಿಜ;
      }

      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ vpi_array_is_string(ಸ್ಥಿರಾಂಕ vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      ಆದರೆ (arr->vals4 != 0)  // A bit based variable/register array.
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

      ಆದರೆ (dynamic_cast<vvp_darray_string*> (arr->vals))
	    ಹಿಂತಿರುಗಿ ನಿಜ;

      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಪೂರ್ಣಾಂಕ __vpiArray::get_ಶಬ್ದ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ
{
      ಚಿಹ್ನೆರಹಿತ width;

      ಪ್ರತಿಜ್ಞಾ(get_ಗಾತ್ರ() > 0);
	/* For a net array we need to get the width from the first element. */
      ಆದರೆ (nets) {
	    ಪ್ರತಿಜ್ಞಾ(vals4 == 0 && vals == 0);
	    ಸ್ಥಿರಾಂಕ ರಚನೆ __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    ಪ್ರತಿಜ್ಞಾ(vsig);
	    width = vpip_ಗಾತ್ರ(vsig);
	/* For a variable array we can get the width from vals_width. */
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಪ್ರತಿಜ್ಞಾ(vals4 || vals);
	    width = vals_ಅಗಲ;
      }

      ಹಿಂತಿರುಗಿ width;
}

ಅಕ್ಷರ*__vpiArray::get_ಶಬ್ದ_str(ರಚನೆ __vpiArrayWord*ಶಬ್ದ, ಪೂರ್ಣಾಂಕ code)
{
      ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ = ಶಬ್ದ->get_ಸೂಚ್ಯಂಕ();

      ಆದರೆ (code == vpiFile) {  // Not implemented for now!
	    ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str(file_ಹೆಸರುಗಳು[0]);
      }

      ಅಕ್ಷರ sidx [64];
      snprintf(sidx, 63, "%d", (ಪೂರ್ಣಾಂಕ)ಸೂಚ್ಯಂಕ + ಮೊದಲ_addr.get_ಮೌಲ್ಯ());
      ಹಿಂತಿರುಗಿ generic_get_str(code, get_scope(), name, sidx);
}

ಶೂನ್ಯ __vpiArray::get_ಶಬ್ದ_ಮೌಲ್ಯ(ರಚನೆ __vpiArrayWord*ಶಬ್ದ, p_vpi_ಮೌಲ್ಯ vp)
{
      ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ = ಶಬ್ದ->get_ಸೂಚ್ಯಂಕ();

    // Determine the appropriate format (The Verilog PLI Handbook 5.2.10)
      ಆದರೆ(vp->format == vpiObjTypeVal) {
          ಆದರೆ(vpi_array_is_real(ಇದು))
              vp->format = vpiRealVal;
          ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(vpi_array_is_string(ಇದು))
              vp->format = vpiStringVal;
          ಇಲ್ಲದಿದ್ದರೆ
              vp->format = vpiIntVal;
      }

      ಆದರೆ(vals4) {
          vpip_vec4_get_ಮೌಲ್ಯ(vals4->get_ಶಬ್ದ(ಸೂಚ್ಯಂಕ),
                              vals4->width(), signed_flag, vp);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(vals) {
          ಆಯ್ಕೆ(vp->format) {
            ಸ್ಥಿತಿ vpiBinStrVal:
            ಸ್ಥಿತಿ vpiOctStrVal:
            ಸ್ಥಿತಿ vpiDecStrVal:
            ಸ್ಥಿತಿ vpiHexStrVal:
            ಸ್ಥಿತಿ vpiScalarVal:
            ಸ್ಥಿತಿ vpiIntVal:
            ಸ್ಥಿತಿ vpi_ಸದಿಶ_Val:
            {
                vvp_vector4_t v;
                vals->get_ಶಬ್ದ(ಸೂಚ್ಯಂಕ, v);
                vpip_vec4_get_ಮೌಲ್ಯ(v, vals_ಅಗಲ, signed_flag, vp);
            }
            ನಿಲ್ಲಿಸು;

            ಸ್ಥಿತಿ vpiRealVal:
            {
                ದ್ವಿಗುಣ d;
                vals->get_ಶಬ್ದ(ಸೂಚ್ಯಂಕ, d);
                vpip_real_get_ಮೌಲ್ಯ(d, vp);
            }
            ನಿಲ್ಲಿಸು;

            ಸ್ಥಿತಿ vpiStringVal:
            {
                string s;
                vals->get_ಶಬ್ದ(ಸೂಚ್ಯಂಕ, s);
                vpip_string_get_ಮೌಲ್ಯ(s, vp);
            }
            ನಿಲ್ಲಿಸು;

            ಪೂರ್ವನಿಯೋಜಿತ:
                fprintf(stderr, "vpi sorry: format is not implemented\n");
                ಪ್ರತಿಜ್ಞಾ(ಸುಳ್ಳು);
          }
      }
}

ಶೂನ್ಯ __vpiArray::put_ಶಬ್ದ_ಮೌಲ್ಯ(ರಚನೆ __vpiArrayWord*ಶಬ್ದ, p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಂಕ)
{
      ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ = ಶಬ್ದ->get_ಸೂಚ್ಯಂಕ();
      vvp_vector4_t val = vec4_from_vpi_ಮೌಲ್ಯ(vp, vals_ಅಗಲ);
      ಗಣ_ಶಬ್ದ(ಸೂಚ್ಯಂಕ, 0, val);
}

vpiHandle __vpiArray::get_iter_ಸೂಚ್ಯಂಕ(ರಚನೆ __vpiArrayIterator*, ಪೂರ್ಣಾಂಕ idx)
{
      ಆದರೆ (nets) ಹಿಂತಿರುಗಿ nets[idx];

      ಪ್ರತಿಜ್ಞಾ(vals4 || vals);

      ಆದರೆ (vals_ಪದಗಳು == 0) make_vals_ಪದಗಳು();

      ಹಿಂತಿರುಗಿ &(vals_ಪದಗಳು[idx].as_ಶಬ್ದ);
}

ಪೂರ್ಣಾಂಕ __vpiArray::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_ಸಾಲು_No:
	    ಹಿಂತಿರುಗಿ 0; // Not implemented for now!

	  ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ get_ಗಾತ್ರ();

	  ಸ್ಥಿತಿ vpiAutomatic:
	    ಹಿಂತಿರುಗಿ scope->is_automatic()? 1 : 0;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ 0;
      }
}

ಅಕ್ಷರ* __vpiArray::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{
      ಆದರೆ (code == vpiFile) {  // Not implemented for now!
            ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str(file_ಹೆಸರುಗಳು[0]);
      }

      ಹಿಂತಿರುಗಿ generic_get_str(code, scope, name, ಖಾಲಿ);
}

vpiHandle __vpiArray::vpi_handle(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
          ಸ್ಥಿತಿ vpi_ಎಡ_Range:
            ಆದರೆ (ಅದಲುಬದಲು_addr) ಹಿಂತಿರುಗಿ &ಕೊನೆಯ_addr;
            ಇಲ್ಲದಿದ್ದರೆ ಹಿಂತಿರುಗಿ &ಮೊದಲ_addr;

          ಸ್ಥಿತಿ vpi_ಬಲ_Range:
            ಆದರೆ (ಅದಲುಬದಲು_addr) ಹಿಂತಿರುಗಿ &ಮೊದಲ_addr;
            ಇಲ್ಲದಿದ್ದರೆ ಹಿಂತಿರುಗಿ &ಕೊನೆಯ_addr;

	  ಸ್ಥಿತಿ vpiScope:
	    ಹಿಂತಿರುಗಿ scope;

	  ಸ್ಥಿತಿ vpiModule:
	    ಹಿಂತಿರುಗಿ vpip_module(scope);
      }

      ಹಿಂತಿರುಗಿ 0;
}

/*
* VPI code passes indices that are not yet converted to canonical
* form, so this index function does it here.
*/
vpiHandle __vpiArray::vpi_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ)
{
      ಸೂಚ್ಯಂಕ -= ಮೊದಲ_addr.get_ಮೌಲ್ಯ();
      ಆದರೆ (ಸೂಚ್ಯಂಕ >= (ದೀರ್ಘ) get_ಗಾತ್ರ())
	    ಹಿಂತಿರುಗಿ 0;
      ಆದರೆ (ಸೂಚ್ಯಂಕ < 0)
	    ಹಿಂತಿರುಗಿ 0;

      ಆದರೆ (nets != 0) {
	    ಹಿಂತಿರುಗಿ nets[ಸೂಚ್ಯಂಕ];
      }

      ಆದರೆ (vals_ಪದಗಳು == 0)
	    make_vals_ಪದಗಳು();

      ಹಿಂತಿರುಗಿ &(vals_ಪದಗಳು[ಸೂಚ್ಯಂಕ].as_ಶಬ್ದ);
}

ಪೂರ್ಣಾಂಕ __vpiArrayWord::as_ಶಬ್ದ_t::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ರಚನೆ __vpiArrayWord*obj = array_var_ಶಬ್ದ_from_handle(ಇದು);
      ಪ್ರತಿಜ್ಞಾ(obj);
      ರಚನೆ __vpiArrayBase*my_ಪೋಷಕ = obj->get_ಪೋಷಕ();
      t_vpi_ಮೌಲ್ಯ val;

      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_ಸಾಲು_No:
	    ಹಿಂತಿರುಗಿ 0; // Not implemented for now!

	  ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ my_ಪೋಷಕ->get_ಶಬ್ದ_ಗಾತ್ರ();

	  ಸ್ಥಿತಿ vpi_ಎಡ_Range:
            val.format = vpiIntVal;
	    my_ಪೋಷಕ->get_ಎಡ_range()->vpi_get_ಮೌಲ್ಯ(&val);
            ಪ್ರತಿಜ್ಞಾ(val.format == vpiIntVal);
            ಹಿಂತಿರುಗಿ val.value.integer;

	  ಸ್ಥಿತಿ vpi_ಬಲ_Range:
            val.format = vpiIntVal;
	    my_ಪೋಷಕ->get_ಬಲ_range()->vpi_get_ಮೌಲ್ಯ(&val);
            ಪ್ರತಿಜ್ಞಾ(val.format == vpiIntVal);
            ಹಿಂತಿರುಗಿ val.value.integer;

	  ಸ್ಥಿತಿ vpi_ಸೂಚ್ಯಂಕ_2:
	    {
		  ಪೂರ್ಣಾಂಕ ತಳ_ಸ್ಥಾನಾಂತರ = 0;
		  ರಚನೆ __vpiArray*ತಳ = dynamic_cast<__vpiArray*> (my_ಪೋಷಕ);
		  ಆದರೆ (ತಳ) {
			val.format = vpiIntVal;
			ತಳ->ಮೊದಲ_addr.vpi_get_ಮೌಲ್ಯ(&val);
			ತಳ_ಸ್ಥಾನಾಂತರ += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_ಸೂಚ್ಯಂಕ.vpi_get_ಮೌಲ್ಯ(&val);
		  ಪ್ರತಿಜ್ಞಾ(val.format == vpiIntVal);
		  ಹಿಂತಿರುಗಿ val.value.integer + ತಳ_ಸ್ಥಾನಾಂತರ;
	    }

	  ಸ್ಥಿತಿ vpiAutomatic:
	    ಹಿಂತಿರುಗಿ my_ಪೋಷಕ->get_scope()->is_automatic()? 1 : 0;

#ಆದರೆ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ಸ್ಥಿತಿ _vpiFromThr:
	    ಹಿಂತಿರುಗಿ _vpiNoThr;
#ಆದರೆ_ಅಂತ್ಯ

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ 0;
      }
}


ಪೂರ್ಣಾಂಕ __vpiArrayVthrA::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_ಸಾಲು_No:
	    ಹಿಂತಿರುಗಿ 0; // Not implemented for now!

	  ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ array->get_ಶಬ್ದ_ಗಾತ್ರ();

	  ಸ್ಥಿತಿ vpi_ಎಡ_Range:
	    ಹಿಂತಿರುಗಿ array->msb.get_ಮೌಲ್ಯ();

	  ಸ್ಥಿತಿ vpi_ಬಲ_Range:
	    ಹಿಂತಿರುಗಿ array->lsb.get_ಮೌಲ್ಯ();

	  ಸ್ಥಿತಿ vpi_ಸೂಚ್ಯಂಕ_2:
	    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)get_ವಿಳಾಸ() + array->ಮೊದಲ_addr.get_ಮೌಲ್ಯ();

	  ಸ್ಥಿತಿ vpiAutomatic:
	    ಹಿಂತಿರುಗಿ array->get_scope()->is_automatic() ? 1 : 0;

#ಆದರೆ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ಸ್ಥಿತಿ _vpiFromThr:
	    ಹಿಂತಿರುಗಿ _vpi_at_A;
#ಆದರೆ_ಅಂತ್ಯ

	  // If address_handle is not zero we definitely have a
	  // variable.
	  ಸ್ಥಿತಿ vpiConstantSelect:
	    ಹಿಂತಿರುಗಿ ವಿಳಾಸ_handle == 0;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ 0;
      }
}

ಅಕ್ಷರ* __vpiArrayVthrA::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{
      ಆದರೆ (code == vpiFile) {  // Not implemented for now!
            ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str(file_ಹೆಸರುಗಳು[0]);
      }

      ಅಕ್ಷರ sidx [64];
      snprintf(sidx, 63, "%d", (ಪೂರ್ಣಾಂಕ)get_ವಿಳಾಸ() + array->ಮೊದಲ_addr.get_ಮೌಲ್ಯ());
      ಹಿಂತಿರುಗಿ generic_get_str(code, array->get_scope(), array->name, sidx);
}

ಶೂನ್ಯ __vpiArrayVthrA::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp)
{
      ಪ್ರತಿಜ್ಞಾ(array);

      ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ = get_ವಿಳಾಸ();
      ಆದರೆ (vpi_array_is_real(array)) {
	    ದ್ವಿಗುಣ tmp = array->get_ಶಬ್ದ_r(ಸೂಚ್ಯಂಕ);
	    vpip_real_get_ಮೌಲ್ಯ(tmp, vp);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vpi_array_is_string(array)) {
	    string tmp = array->get_ಶಬ್ದ_str(ಸೂಚ್ಯಂಕ);
	    vpip_string_get_ಮೌಲ್ಯ(tmp, vp);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_vector4_t tmp = array->get_ಶಬ್ದ(ಸೂಚ್ಯಂಕ);
	    ಚಿಹ್ನೆರಹಿತ width = array->get_ಶಬ್ದ_ಗಾತ್ರ();
	    vpip_vec4_get_ಮೌಲ್ಯ(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಂಕ)
{
      ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ = get_ವಿಳಾಸ();

      ಪ್ರತಿಜ್ಞಾ(array);
      ಪ್ರತಿಜ್ಞಾ(ಸೂಚ್ಯಂಕ < array->get_ಗಾತ್ರ());

      ಆದರೆ (vpi_array_is_real(array)) {
	    ದ್ವಿಗುಣ val = real_from_vpi_ಮೌಲ್ಯ(vp);
	    array->ಗಣ_ಶಬ್ದ(ಸೂಚ್ಯಂಕ, val);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಚಿಹ್ನೆರಹಿತ width = array->get_ಶಬ್ದ_ಗಾತ್ರ();
	    vvp_vector4_t val = vec4_from_vpi_ಮೌಲ್ಯ(vp, width);
	    array->ಗಣ_ಶಬ್ದ(ಸೂಚ್ಯಂಕ, 0, val);
      }

      ಹಿಂತಿರುಗಿ ಇದು;
}

vpiHandle __vpiArrayVthrA::vpi_handle(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_ಸೂಚ್ಯಂಕ_2:
	    ನಿಲ್ಲಿಸು;  // Not implemented!

	  ಸ್ಥಿತಿ vpi_ಎಡ_Range:
	    ಹಿಂತಿರುಗಿ &array->msb;

	  ಸ್ಥಿತಿ vpi_ಬಲ_Range:
	    ಹಿಂತಿರುಗಿ &array->lsb;

	  ಸ್ಥಿತಿ vpi_ಪೋಷಕ:
	  ಸ್ಥಿತಿ vpiArray:
	    ಹಿಂತಿರುಗಿ array;

	  ಸ್ಥಿತಿ vpiScope:
	    ಹಿಂತಿರುಗಿ array->get_scope();

	  ಸ್ಥಿತಿ vpiModule:
	    ಹಿಂತಿರುಗಿ vpip_module(array->get_scope());
      }

      ಹಿಂತಿರುಗಿ 0;
}


ಪೂರ್ಣಾಂಕ __vpiArrayVthrAPV::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_ಸಾಲು_No:
	    ಹಿಂತಿರುಗಿ 0; // Not implemented for now!

	  ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ part_wid;

	  ಸ್ಥಿತಿ vpi_ಎಡ_Range:
	    ಹಿಂತಿರುಗಿ part_ಬಿಟ್ + part_wid - 1;

	  ಸ್ಥಿತಿ vpi_ಬಲ_Range:
	    ಹಿಂತಿರುಗಿ part_ಬಿಟ್;

	  ಸ್ಥಿತಿ vpiAutomatic:
	    ಹಿಂತಿರುಗಿ array->get_scope()->is_automatic() ? 1 : 0;

#ಆದರೆ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ಸ್ಥಿತಿ _vpiFromThr:
	    ಹಿಂತಿರುಗಿ _vpi_at_APV;
#ಆದರೆ_ಅಂತ್ಯ

	  ಸ್ಥಿತಿ vpiConstantSelect:
	    ಹಿಂತಿರುಗಿ 1;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ 0;
      }
}

ಅಕ್ಷರ* __vpiArrayVthrAPV::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{
      ಆದರೆ (code == vpiFile) {  // Not implemented for now!
            ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str(file_ಹೆಸರುಗಳು[0]);
      }

      ಅಕ್ಷರ sidx [64];
      snprintf(sidx, 63, "%u", ಶಬ್ದ_sel + array->ಮೊದಲ_addr.get_ಮೌಲ್ಯ());
      ಹಿಂತಿರುಗಿ generic_get_str(code, array->get_scope(), array->name, sidx);
}

ಶೂನ್ಯ __vpiArrayVthrAPV::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp)
{
      ಪ್ರತಿಜ್ಞಾ(array);

      ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ = ಶಬ್ದ_sel;
      ಆದರೆ (vpi_array_is_real(array)) {
	    ದ್ವಿಗುಣ tmp = array->get_ಶಬ್ದ_r(ಸೂಚ್ಯಂಕ);
	    vpip_real_get_ಮೌಲ್ಯ(tmp, vp);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    vvp_vector4_t tmp = array->get_ಶಬ್ದ(ಸೂಚ್ಯಂಕ);
	    tmp = tmp.subvalue(part_ಬಿಟ್, part_wid);
	    vpip_vec4_get_ಮೌಲ್ಯ(tmp, part_wid, ಸುಳ್ಳು, vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  ಸ್ಥಿತಿ vpi_ಪೋಷಕ:
	    ಹಿಂತಿರುಗಿ 0;

            // Not part of the Verilog standard. We use this internally.
	  ಸ್ಥಿತಿ vpiArray:
	    ಹಿಂತಿರುಗಿ array;

	  ಸ್ಥಿತಿ vpiScope:
	    ಹಿಂತಿರುಗಿ array->get_scope();

	  ಸ್ಥಿತಿ vpiModule:
	    ಹಿಂತಿರುಗಿ vpip_module(array->get_scope());
      }

      ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ __vpiArray::ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ ವಿಳಾಸ, ಚಿಹ್ನೆರಹಿತ part_off, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val)
{
      ಆದರೆ (ವಿಳಾಸ >= get_ಗಾತ್ರ())
	    ಹಿಂತಿರುಗಿ;

      ಆದರೆ (vals4) {
	    ಪ್ರತಿಜ್ಞಾ(nets == 0);
	    ಆದರೆ (part_off != 0 || val.size() != vals_ಅಗಲ) {
		  vvp_vector4_t tmp = vals4->get_ಶಬ್ದ(ವಿಳಾಸ);
		  ಆದರೆ ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_ಅಗಲ << ಸಾಲು_ಅಂತ್ಯ;
			ಪ್ರತಿಜ್ಞಾ(0);
		  }
		  tmp.ಗಣ_vec(part_off, val);
		  vals4->ಗಣ_ಶಬ್ದ(ವಿಳಾಸ, tmp);
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  vals4->ಗಣ_ಶಬ್ದ(ವಿಳಾಸ, val);
	    }
	    ಶಬ್ದ_change(ವಿಳಾಸ);
	    ಹಿಂತಿರುಗಿ;
      }

      ಆದರೆ (vals) {
	    ಪ್ರತಿಜ್ಞಾ(nets == 0);
	    ಆದರೆ (part_off != 0 || val.size() != vals_ಅಗಲ) {
		  vvp_vector4_t tmp;
		  vals->get_ಶಬ್ದ(ವಿಳಾಸ, tmp);
		  ಆದರೆ ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_ಅಗಲ << ಸಾಲು_ಅಂತ್ಯ;
			ಪ್ರತಿಜ್ಞಾ(0);
		  }
		  tmp.ಗಣ_vec(part_off, val);
		  vals->ಗಣ_ಶಬ್ದ(ವಿಳಾಸ, tmp);
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  vals->ಗಣ_ಶಬ್ದ(ವಿಳಾಸ, val);
	    }
	    ಶಬ್ದ_change(ವಿಳಾಸ);
	    ಹಿಂತಿರುಗಿ;
      }

      ಪ್ರತಿಜ್ಞಾ(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle ಶಬ್ದ = nets[ವಿಳಾಸ];
      ಸ್ಥಿರಾಂಕ ರಚನೆ __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(ಶಬ್ದ);
      ಪ್ರತಿಜ್ಞಾ(vsig);

      vsig->ಗಂಟು->send_vec4_pv(val, part_off, vpip_ಗಾತ್ರ(vsig), 0);
      ಶಬ್ದ_change(ವಿಳಾಸ);
}

ಶೂನ್ಯ __vpiArray::ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ ವಿಳಾಸ, ದ್ವಿಗುಣ val)
{
      ಪ್ರತಿಜ್ಞಾ(vals != 0);
      ಪ್ರತಿಜ್ಞಾ(nets == 0);

      ಆದರೆ (ವಿಳಾಸ >= vals->get_ಗಾತ್ರ())
	    ಹಿಂತಿರುಗಿ;

      vals->ಗಣ_ಶಬ್ದ(ವಿಳಾಸ, val);
      ಶಬ್ದ_change(ವಿಳಾಸ);
}

ಶೂನ್ಯ __vpiArray::ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ ವಿಳಾಸ, ಸ್ಥಿರಾಂಕ string&val)
{
      ಪ್ರತಿಜ್ಞಾ(vals != 0);
      ಪ್ರತಿಜ್ಞಾ(nets == 0);

      ಆದರೆ (ವಿಳಾಸ >= vals->get_ಗಾತ್ರ())
	    ಹಿಂತಿರುಗಿ;

      vals->ಗಣ_ಶಬ್ದ(ವಿಳಾಸ, val);
      ಶಬ್ದ_change(ವಿಳಾಸ);
}

ಶೂನ್ಯ __vpiArray::ಗಣ_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ ವಿಳಾಸ, ಸ್ಥಿರಾಂಕ vvp_object_t&val)
{
      ಪ್ರತಿಜ್ಞಾ(vals != 0);
      ಪ್ರತಿಜ್ಞಾ(nets == 0);

      ಆದರೆ (ವಿಳಾಸ >= vals->get_ಗಾತ್ರ())
	    ಹಿಂತಿರುಗಿ;

      vals->ಗಣ_ಶಬ್ದ(ವಿಳಾಸ, val);
      ಶಬ್ದ_change(ವಿಳಾಸ);
}

vvp_vector4_t __vpiArray::get_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ ವಿಳಾಸ)
{
      ಆದರೆ (vals4) {
	    ಪ್ರತಿಜ್ಞಾ(nets == 0);
	    ಪ್ರತಿಜ್ಞಾ(vals == 0);
	    ಹಿಂತಿರುಗಿ vals4->get_ಶಬ್ದ(ವಿಳಾಸ);
      }

      ಆದರೆ (vals) {
	    ಪ್ರತಿಜ್ಞಾ(nets == 0);
	    ಪ್ರತಿಜ್ಞಾ(vals4== 0);
	    ಆದರೆ (ವಿಳಾಸ >= vals->get_ಗಾತ್ರ())
		  ಹಿಂತಿರುಗಿ vvp_vector4_t(vals_ಅಗಲ, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_ಶಬ್ದ(ವಿಳಾಸ, val);
	    ಹಿಂತಿರುಗಿ val;
      }

      ಪ್ರತಿಜ್ಞಾ(vals4 == 0);
      ಪ್ರತಿಜ್ಞಾ(vals == 0);
      ಪ್ರತಿಜ್ಞಾ(nets != 0);

      ಆದರೆ (ವಿಳಾಸ >= get_ಗಾತ್ರ()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    ಪ್ರತಿಜ್ಞಾ(get_ಗಾತ್ರ() > 0);
	    vpiHandle ಶಬ್ದ = nets[0];
	    ಪ್ರತಿಜ್ಞಾ(ಶಬ್ದ);
	    ರಚನೆ __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(ಶಬ್ದ);
	    ಪ್ರತಿಜ್ಞಾ(vsig);
	    vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*> (vsig->ಗಂಟು->fil);
	    ಪ್ರತಿಜ್ಞಾ(sig);
	    ಹಿಂತಿರುಗಿ vvp_vector4_t(sig->ಮೌಲ್ಯ_ಗಾತ್ರ(), BIT4_X);
      }

      vpiHandle ಶಬ್ದ = nets[ವಿಳಾಸ];
      ರಚನೆ __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(ಶಬ್ದ);
      ಪ್ರತಿಜ್ಞಾ(vsig);
      vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*> (vsig->ಗಂಟು->fil);
      ಪ್ರತಿಜ್ಞಾ(sig);

      vvp_vector4_t val;
      sig->vec4_ಮೌಲ್ಯ(val);
      ಹಿಂತಿರುಗಿ val;
}

ದ್ವಿಗುಣ __vpiArray::get_ಶಬ್ದ_r(ಚಿಹ್ನೆರಹಿತ ವಿಳಾಸ)
{
      ಆದರೆ (vals) {
	    ಪ್ರತಿಜ್ಞಾ(vals4 == 0);
	    ಪ್ರತಿಜ್ಞಾ(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    ಆದರೆ (ವಿಳಾಸ >= vals->get_ಗಾತ್ರ())
		  ಹಿಂತಿರುಗಿ 0.0;

	    ದ್ವಿಗುಣ val;
	    vals->get_ಶಬ್ದ(ವಿಳಾಸ, val);
	    ಹಿಂತಿರುಗಿ val;
      }

      ಆದರೆ (ವಿಳಾಸ >= get_ಗಾತ್ರ())
	    ಹಿಂತಿರುಗಿ 0.0;

      ಪ್ರತಿಜ್ಞಾ(nets);
      vpiHandle ಶಬ್ದ = nets[ವಿಳಾಸ];
      ರಚನೆ __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(ಶಬ್ದ);
      ಪ್ರತಿಜ್ಞಾ(vsig);
      vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*> (vsig->net->fil);
      ಪ್ರತಿಜ್ಞಾ(sig);

      ದ್ವಿಗುಣ val = sig->real_ಮೌಲ್ಯ();
      ಹಿಂತಿರುಗಿ val;
}

ಶೂನ್ಯ __vpiArray::get_ಶಬ್ದ_obj(ಚಿಹ್ನೆರಹಿತ ವಿಳಾಸ, vvp_object_t&val)
{
      ಆದರೆ (vals) {
	    ಪ್ರತಿಜ್ಞಾ(vals4 == 0);
	    ಪ್ರತಿಜ್ಞಾ(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    ಆದರೆ (ವಿಳಾಸ >= vals->get_ಗಾತ್ರ()) {
		  val = vvp_object_t();
		  ಹಿಂತಿರುಗಿ;
	    }

	    vals->get_ಶಬ್ದ(ವಿಳಾಸ, val);
	    ಹಿಂತಿರುಗಿ;
      }

      ಪ್ರತಿಜ್ಞಾ(nets);
	// Arrays of string nets not implemented!
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ;
}

string __vpiArray::get_ಶಬ್ದ_str(ಚಿಹ್ನೆರಹಿತ ವಿಳಾಸ)
{
      ಆದರೆ (vals) {
	    ಪ್ರತಿಜ್ಞಾ(vals4 == 0);
	    ಪ್ರತಿಜ್ಞಾ(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    ಆದರೆ (ವಿಳಾಸ >= vals->get_ಗಾತ್ರ())
		  ಹಿಂತಿರುಗಿ "";

	    string val;
	    vals->get_ಶಬ್ದ(ವಿಳಾಸ, val);
	    ಹಿಂತಿರುಗಿ val;
      }

      ಪ್ರತಿಜ್ಞಾ(nets);
	// Arrays of string nets not implemented!
      ಪ್ರತಿಜ್ಞಾ(0);
      ಹಿಂತಿರುಗಿ "";
}

vpiHandle vpip_make_array(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*label, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name,
                          ಪೂರ್ಣಾಂಕ ಮೊದಲ_addr, ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_addr,
                          ತಾರ್ಕಿಕ signed_flag)
{
      ರಚನೆ __vpiArray*obj = ಹೊಸ __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      ಆದರೆ (ಕೊನೆಯ_addr >= ಮೊದಲ_addr) {
	    obj->ಅದಲುಬದಲು_addr = ಸುಳ್ಳು;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    obj->ಅದಲುಬದಲು_addr = ನಿಜ;
	    ಪೂರ್ಣಾಂಕ tmp = ಕೊನೆಯ_addr;
	    ಕೊನೆಯ_addr = ಮೊದಲ_addr;
	    ಮೊದಲ_addr = tmp;
      }
      ಪ್ರತಿಜ್ಞಾ(ಕೊನೆಯ_addr >= ಮೊದಲ_addr);
      ಚಿಹ್ನೆರಹಿತ array_ಎಣಿಕೆ = ಕೊನೆಯ_addr+1-ಮೊದಲ_addr;

	// For now, treat all arrays as memories. This is not quite
	// correct, as arrays are arrays with memories a special case.
      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_ಎಣಿಕೆ = array_ಎಣಿಕೆ;

      obj->ಮೊದಲ_addr.ಗಣ_ಮೌಲ್ಯ(ಮೊದಲ_addr);
      obj->ಕೊನೆಯ_addr .ಗಣ_ಮೌಲ್ಯ(ಕೊನೆಯ_addr);

	// Start off now knowing if we are nets or variables.
      obj->nets = 0;
      obj->vals4 = 0;
      obj->vals  = 0;
      obj->vals_ಅಗಲ = 0;
      obj->vals_ಪದಗಳು = 0;

	// Initialize (clear) the read-ports list.
      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

	/* Add this symbol to the array_symbols table for later lookup. */
      ಆದರೆ (!array_ಕೋಷ್ಟಕ)
	    array_ಕೋಷ್ಟಕ = ಹೊಸ ಚಿಹ್ನೆ_map_s<ರಚನೆ __vpiArray>;

      ಪ್ರತಿಜ್ಞಾ(!array_ಕಂಡುಹಿಡಿ(label));
      array_ಕೋಷ್ಟಕ->sym_ಗಣ_ಮೌಲ್ಯ(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_ಚಿಹ್ನೆ(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_ಗಮ್ಯ_current_scope(obj);

      ಹಿಂತಿರುಗಿ obj;
}

ಶೂನ್ಯ __vpiArray::alias_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr, vpiHandle ಶಬ್ದ, ಪೂರ್ಣಾಂಕ msb_, ಪೂರ್ಣಾಂಕ lsb_)
{
      ಪ್ರತಿಜ್ಞಾ(msb.get_ಮೌಲ್ಯ() == msb_);
      ಪ್ರತಿಜ್ಞಾ(lsb.get_ಮೌಲ್ಯ() == lsb_);
      ಪ್ರತಿಜ್ಞಾ(addr < get_ಗಾತ್ರ());
      ಪ್ರತಿಜ್ಞಾ(nets);
      nets[addr] = ಶಬ್ದ;
}

ಶೂನ್ಯ __vpiArray::attach_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ addr, vpiHandle ಶಬ್ದ)
{
      ಪ್ರತಿಜ್ಞಾ(addr < get_ಗಾತ್ರ());
      ಪ್ರತಿಜ್ಞಾ(nets);
      nets[addr] = ಶಬ್ದ;

      ಆದರೆ (ರಚನೆ __vpiSignal*sig = dynamic_cast<__vpiSignal*>(ಶಬ್ದ)) {
	    vvp_net_t*net = sig->ಗಂಟು;
	    ಪ್ರತಿಜ್ಞಾ(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    ಪ್ರತಿಜ್ಞಾ(fun);
	    fun->attach_as_ಶಬ್ದ(ಇದು, addr);
	    sig->is_netarray = 1;
	    sig->within.ಪೋಷಕ = ಇದು;
	    sig->id.ಸೂಚ್ಯಂಕ = ಹೊಸ __vpiDecConst(addr + ಮೊದಲ_addr.get_ಮೌಲ್ಯ());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    ಹಿಂತಿರುಗಿ;
      }

      ಆದರೆ (ರಚನೆ __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(ಶಬ್ದ)) {
	    vvp_net_t*net = sig->net;
	    ಪ್ರತಿಜ್ಞಾ(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    ಪ್ರತಿಜ್ಞಾ(fun);
	    fun->attach_as_ಶಬ್ದ(ಇದು, addr);
	    sig->is_netarray = 1;
	    sig->within.ಪೋಷಕ = ಇದು;
	    sig->id.ಸೂಚ್ಯಂಕ = ಹೊಸ __vpiDecConst(addr + ಮೊದಲ_addr.get_ಮೌಲ್ಯ());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = ನಿಜ;
	    ಹಿಂತಿರುಗಿ;
      }
}

ಶೂನ್ಯ compile_var_array(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ ಕೊನೆಯ, ಪೂರ್ಣಾಂಕ ಮೊದಲ,
		   ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb, ಅಕ್ಷರ signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, ಮೊದಲ, ಕೊನೆಯ,
                                      signed_flag != 0);

      ರಚನೆ __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_ಅಗಲ = labs(msb-lsb) + 1;
      ಆದರೆ (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = ಹೊಸ vvp_vector4array_aa(arr->vals_ಅಗಲ,
						 arr->get_ಗಾತ್ರ());
      } ಇಲ್ಲದಿದ್ದರೆ {
            arr->vals4 = ಹೊಸ vvp_vector4array_sa(arr->vals_ಅಗಲ,
						 arr->get_ಗಾತ್ರ());
      }
      arr->msb.ಗಣ_ಮೌಲ್ಯ(msb);
      arr->lsb.ಗಣ_ಮೌಲ್ಯ(lsb);

      ಎಣಿಕೆ_var_arrays += 1;
      ಎಣಿಕೆ_var_array_ಪದಗಳು += arr->get_ಗಾತ್ರ();

      ಬಿಡುಗಡೆ(label);
      ಅಳಿಸು[] name;
}

ಶೂನ್ಯ compile_var2_array(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ ಕೊನೆಯ, ಪೂರ್ಣಾಂಕ ಮೊದಲ,
		   ಪೂರ್ಣಾಂಕ msb, ಪೂರ್ಣಾಂಕ lsb, ತಾರ್ಕಿಕ signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, ಮೊದಲ, ಕೊನೆಯ, signed_flag);

      ರಚನೆ __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.ಗಣ_ಮೌಲ್ಯ(msb);
      arr->lsb.ಗಣ_ಮೌಲ್ಯ(lsb);
      arr->vals_ಅಗಲ = labs(msb-lsb) + 1;

      ಪ್ರತಿಜ್ಞಾ(! arr->nets);
      ಆದರೆ (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = ಹೊಸ vvp_darray_atom<int8_t>(arr->get_ಗಾತ್ರ());
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = ಹೊಸ vvp_darray_atom<uint8_t>(arr->get_ಗಾತ್ರ());
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = ಹೊಸ vvp_darray_atom<int16_t>(arr->get_ಗಾತ್ರ());
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = ಹೊಸ vvp_darray_atom<uint16_t>(arr->get_ಗಾತ್ರ());
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = ಹೊಸ vvp_darray_atom<int32_t>(arr->get_ಗಾತ್ರ());
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = ಹೊಸ vvp_darray_atom<uint32_t>(arr->get_ಗಾತ್ರ());
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = ಹೊಸ vvp_darray_atom<int64_t>(arr->get_ಗಾತ್ರ());
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = ಹೊಸ vvp_darray_atom<uint64_t>(arr->get_ಗಾತ್ರ());
      } ಇಲ್ಲದಿದ್ದರೆ {
	    arr->vals = ಹೊಸ vvp_darray_vec2(arr->get_ಗಾತ್ರ(), arr->vals_ಅಗಲ);
      }
      ಎಣಿಕೆ_var_arrays += 1;
      ಎಣಿಕೆ_var_array_ಪದಗಳು += arr->get_ಗಾತ್ರ();

      ಬಿಡುಗಡೆ(label);
      ಅಳಿಸು[] name;
}

ಶೂನ್ಯ compile_real_array(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ ಕೊನೆಯ, ಪೂರ್ಣಾಂಕ ಮೊದಲ)
{
      vpiHandle obj = vpip_make_array(label, name, ಮೊದಲ, ಕೊನೆಯ, ನಿಜ);

      ರಚನೆ __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = ಹೊಸ vvp_darray_real(arr->get_ಗಾತ್ರ());
      arr->vals_ಅಗಲ = 1;

      ಎಣಿಕೆ_real_arrays += 1;
      ಎಣಿಕೆ_real_array_ಪದಗಳು += arr->get_ಗಾತ್ರ();

      ಬಿಡುಗಡೆ(label);
      ಅಳಿಸು[] name;
}

ಶೂನ್ಯ compile_string_array(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ ಕೊನೆಯ, ಪೂರ್ಣಾಂಕ ಮೊದಲ)
{
      vpiHandle obj = vpip_make_array(label, name, ಮೊದಲ, ಕೊನೆಯ, ನಿಜ);

      ರಚನೆ __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = ಹೊಸ vvp_darray_string(arr->get_ಗಾತ್ರ());
      arr->vals_ಅಗಲ = 1;

      ಎಣಿಕೆ_real_arrays += 1;
      ಎಣಿಕೆ_real_array_ಪದಗಳು += arr->get_ಗಾತ್ರ();

      ಬಿಡುಗಡೆ(label);
      ಅಳಿಸು[] name;
}

ಶೂನ್ಯ compile_object_array(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ ಕೊನೆಯ, ಪೂರ್ಣಾಂಕ ಮೊದಲ)
{
      vpiHandle obj = vpip_make_array(label, name, ಮೊದಲ, ಕೊನೆಯ, ನಿಜ);

      ರಚನೆ __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = ಹೊಸ vvp_darray_object(arr->get_ಗಾತ್ರ());
      arr->vals_ಅಗಲ = 1;

      ಎಣಿಕೆ_real_arrays += 1;
      ಎಣಿಕೆ_real_array_ಪದಗಳು += arr->get_ಗಾತ್ರ();

      ಬಿಡುಗಡೆ(label);
      ಅಳಿಸು[] name;
}

ಶೂನ್ಯ compile_net_array(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, ಪೂರ್ಣಾಂಕ ಕೊನೆಯ, ಪೂರ್ಣಾಂಕ ಮೊದಲ)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, ಮೊದಲ, ಕೊನೆಯ, ಸುಳ್ಳು);

      ರಚನೆ __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು(arr->get_ಗಾತ್ರ(), ಗಾತ್ರ(vpiHandle)));

      ಎಣಿಕೆ_net_arrays += 1;
      ಎಣಿಕೆ_net_array_ಪದಗಳು += arr->get_ಗಾತ್ರ();

      ಬಿಡುಗಡೆ(label);
      ಅಳಿಸು[] name;
}

ವರ್ಗ vvp_fun_arrayport  : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      ಸ್ಪಷ್ಟ vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      ಸ್ಪಷ್ಟ vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, ದೀರ್ಘ addr);
      ~vvp_fun_arrayport() override;

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ check_ಶಬ್ದ_change(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr) = 0;

    ಸಂರಕ್ಷಿತ:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr_;

      ಮಿತ್ರ ಶೂನ್ಯ array_attach_ಪೋರ್ಟ್(vvp_array_t, vvp_fun_arrayport*);
      ಮಿತ್ರ ಶೂನ್ಯ __vpiArray::ಶಬ್ದ_change(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ);
      vvp_fun_arrayport*ಮುಂದಿನ;
};

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net)
: arr_(mem), net_(net), addr_(0)
{
      ಮುಂದಿನ = 0;
}

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, ದೀರ್ಘ addr)
: arr_(mem), net_(net), addr_(addr)
{
      ಮುಂದಿನ = 0;
}

vvp_fun_arrayport::~vvp_fun_arrayport()
{
}

ವರ್ಗ vvp_fun_arrayport_sa  : ಸಾರ್ವಜನಿಕ vvp_fun_arrayport {

    ಸಾರ್ವಜನಿಕ:
      ಸ್ಪಷ್ಟ vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      ಸ್ಪಷ್ಟ vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, ದೀರ್ಘ addr);
      ~vvp_fun_arrayport_sa() override;

      ಶೂನ್ಯ check_ಶಬ್ದ_change(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr) override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t) override;

    ಖಾಸಗಿ:
};

vvp_fun_arrayport_sa::vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net)
{
}

vvp_fun_arrayport_sa::vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, ದೀರ್ಘ addr)
: vvp_fun_arrayport(mem, net, addr)
{
}

vvp_fun_arrayport_sa::~vvp_fun_arrayport_sa()
{
}

ಶೂನ್ಯ vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                     vvp_context_t)
{
      ತಾರ್ಕಿಕ addr_valid_flag;

      ಆಯ್ಕೆ (ಪೋರ್ಟ್.ಪೋರ್ಟ್()) {

	  ಸ್ಥಿತಿ 0: // Address input
	    addr_valid_flag = vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಬಿಟ್, addr_);
	    ಆದರೆ (! addr_valid_flag)
		  addr_ = arr_->get_ಗಾತ್ರ();
	    ಆದರೆ (vpi_array_is_real(arr_))
		  ಪೋರ್ಟ್.ptr()->send_real(arr_->get_ಶಬ್ದ_r(addr_), 0);
	    ಇಲ್ಲದಿದ್ದರೆ
		  ಪೋರ್ಟ್.ptr()->send_vec4(arr_->get_ಶಬ್ದ(addr_), 0);

	    ನಿಲ್ಲಿಸು;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stdout, "XXXX write ports not implemented.\n");
	    ಪ್ರತಿಜ್ಞಾ(0);
      }
}

ಶೂನ್ಯ vvp_fun_arrayport_sa::check_ಶಬ್ದ_change(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr)
{
      ಆದರೆ (addr != addr_) ಹಿಂತಿರುಗಿ;

      ಆದರೆ (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_ಶಬ್ದ_r(addr_), 0);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    net_->send_vec4(arr_->get_ಶಬ್ದ(addr_), 0);
      }
}

ವರ್ಗ vvp_fun_arrayport_aa  : ಸಾರ್ವಜನಿಕ vvp_fun_arrayport, ಸಾರ್ವಜನಿಕ automatic_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      ಸ್ಪಷ್ಟ vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net);
      ಸ್ಪಷ್ಟ vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, ದೀರ್ಘ addr);
      ~vvp_fun_arrayport_aa() override;

      ಶೂನ್ಯ alloc_instance(vvp_context_t context) override;
      ಶೂನ್ಯ reset_instance(vvp_context_t context) override;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      ಶೂನ್ಯ free_instance(vvp_context_t context) override;
#ಆದರೆ_ಅಂತ್ಯ

      ಶೂನ್ಯ check_ಶಬ್ದ_change(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr) override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t context) override;

    ಖಾಸಗಿ:
      ಶೂನ್ಯ check_ಶಬ್ದ_change_2(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr, vvp_context_t context);

      __vpiScope*context_scope_;
      ಚಿಹ್ನೆರಹಿತ context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_ಕೂಡಿಸು_ಅಂಶ_ಗಮ್ಯ_context(ಇದು, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, ದೀರ್ಘ addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_ಕೂಡಿಸು_ಅಂಶ_ಗಮ್ಯ_context(ಇದು, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

ಶೂನ್ಯ vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*addr = ಹೊಸ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ;
      vvp_ಗಣ_context_ಅಂಶ(context, context_idx_, addr);

      *addr = addr_;
}

ಶೂನ್ಯ vvp_fun_arrayport_aa::reset_instance(vvp_context_t context)
{
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*addr = static_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));

      *addr = addr_;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_fun_arrayport_aa::free_instance(vvp_context_t context)
{
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*addr = static_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));
      ಅಳಿಸು addr;
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                     vvp_context_t context)
{
      ಆದರೆ (context) {
            ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*addr = static_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*>
                  (vvp_get_context_ಅಂಶ(context, context_idx_));

            ತಾರ್ಕಿಕ addr_valid_flag;

            ಆಯ್ಕೆ (ಪೋರ್ಟ್.ಪೋರ್ಟ್()) {

                ಸ್ಥಿತಿ 0: // Address input
                  addr_valid_flag = vector4_ಗಮ್ಯ_ಮೌಲ್ಯ(ಬಿಟ್, *addr);
                  ಆದರೆ (! addr_valid_flag) *addr = arr_->get_ಗಾತ್ರ();
                  ಆದರೆ (vpi_array_is_real(arr_)) {
			ಪೋರ್ಟ್.ptr()->send_real(arr_->get_ಶಬ್ದ_r(*addr),
					      context);
                  } ಇಲ್ಲದಿದ್ದರೆ {
			ಪೋರ್ಟ್.ptr()->send_vec4(arr_->get_ಶಬ್ದ(*addr),
					      context);
                  }
                  ನಿಲ್ಲಿಸು;

                ಪೂರ್ವನಿಯೋಜಿತ:
                  fprintf(stdout, "XXXX write ports not implemented.\n");
                  ಪ್ರತಿಜ್ಞಾ(0);
            }
      } ಇಲ್ಲದಿದ್ದರೆ {
            context = context_scope_->ಜೀವಂತ_contexts;
            ಆಗ (context) {
                  recv_vec4(ಪೋರ್ಟ್, ಬಿಟ್, context);
                  context = vvp_get_ಮುಂದಿನ_context(context);
            }
      }
}

ಶೂನ್ಯ vvp_fun_arrayport_aa::check_ಶಬ್ದ_change_2(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr,
                                              vvp_context_t context)
{
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*ಪೋರ್ಟ್_addr = static_cast<ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ*>
            (vvp_get_context_ಅಂಶ(context, context_idx_));

      ಆದರೆ (addr != *ಪೋರ್ಟ್_addr)
	    ಹಿಂತಿರುಗಿ;

      ಆದರೆ (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_ಶಬ್ದ_r(addr), context);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    net_->send_vec4(arr_->get_ಶಬ್ದ(addr), context);
      }
}

ಶೂನ್ಯ vvp_fun_arrayport_aa::check_ಶಬ್ದ_change(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr)
{
      ಆದರೆ (arr_->get_scope()->is_automatic()) {
            ಪ್ರತಿಜ್ಞಾ(vthread_get_wt_context());
            check_ಶಬ್ದ_change_2(addr, vthread_get_wt_context());
      } ಇಲ್ಲದಿದ್ದರೆ {
            vvp_context_t context = context_scope_->ಜೀವಂತ_contexts;
            ಆಗ (context) {
                  check_ಶಬ್ದ_change_2(addr, context);
                  context = vvp_get_ಮುಂದಿನ_context(context);
            }
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ array_attach_ಪೋರ್ಟ್(vvp_array_t array, vvp_fun_arrayport*fun)
{
      ಪ್ರತಿಜ್ಞಾ(fun->ಮುಂದಿನ == 0);
      fun->ಮುಂದಿನ = array->ports_;
      array->ports_ = fun;
      ಆದರೆ (!array->get_scope()->is_automatic() &&
          (array->vals4 || array->vals)) {
              /* propagate initial values for variable arrays */
            ಆದರೆ (!vpi_array_is_real(array)) {
                  vvp_bit4_t init;
                  ಆದರೆ (array->vals4)
                      init = BIT4_X;
                  ಇಲ್ಲದಿದ್ದರೆ
                      init = BIT4_0;
                  vvp_vector4_t tmp(array->vals_ಅಗಲ, init);
                  ವೇಳಾಪಟ್ಟಿ_init_propagate(fun->net_, tmp);
            } ಇಲ್ಲದಿದ್ದರೆ {
                  ವೇಳಾಪಟ್ಟಿ_init_propagate(fun->net_, 0.0);
            }
      }
}

ವರ್ಗ array_ಶಬ್ದ_ಮೌಲ್ಯ_callback : ಸಾರ್ವಜನಿಕ ಮೌಲ್ಯ_callback {
    ಸಾರ್ವಜನಿಕ:
      ಸಾಲೊಳಗೆ ಸ್ಪಷ್ಟ array_ಶಬ್ದ_ಮೌಲ್ಯ_callback(p_cb_ದತ್ತಾಂಶ ದತ್ತಾಂಶ, ದೀರ್ಘ addr)
      : ಮೌಲ್ಯ_callback(ದತ್ತಾಂಶ), ಶಬ್ದ_addr(addr)
      { }

    ಸಾರ್ವಜನಿಕ:
      ದೀರ್ಘ ಶಬ್ದ_addr;
};

ಶೂನ್ಯ __vpiArray::ಶಬ್ದ_change(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ addr)
{
      ಗಾಗಿ (vvp_fun_arrayport*cur = ports_; cur; cur = cur->ಮುಂದಿನ)
	    cur->check_ಶಬ್ದ_change(addr);

	// Run callbacks attached to the array itself.
      ರಚನೆ __vpiCallback *ಮುಂದಿನ_2 = vpi_callbacks;
      ರಚನೆ __vpiCallback *prev = 0;

      ಆಗ (ಮುಂದಿನ_2) {
	    array_ಶಬ್ದ_ಮೌಲ್ಯ_callback*cur = dynamic_cast<array_ಶಬ್ದ_ಮೌಲ್ಯ_callback*>(ಮುಂದಿನ_2);
	    ಮುಂದಿನ_2 = cur->ಮುಂದಿನ_2;

	      // Skip callbacks that are not for me. -1 is for every element.
	    ಆದರೆ (cur->ಶಬ್ದ_addr != (ದೀರ್ಘ)addr && cur->ಶಬ್ದ_addr != -1) {
		  prev = cur;
		  ಮುಂದುವರಿ;
	    }

	      // For whole array callbacks we need to set the index.
	    ಆದರೆ (cur->ಶಬ್ದ_addr == -1) {
		  cur->cb_ದತ್ತಾಂಶ.ಸೂಚ್ಯಂಕ = (PLI_INT32) ((ಪೂರ್ಣಾಂಕ)addr +
						    ಮೊದಲ_addr.get_ಮೌಲ್ಯ());
	    }

	    ಆದರೆ (cur->cb_ದತ್ತಾಂಶ.cb_rtn != 0) {
		  ಆದರೆ (cur->test_ಮೌಲ್ಯ_callback_ready()) {
			ಆದರೆ (cur->cb_ದತ್ತಾಂಶ.value) {
			      ಆದರೆ (vpi_array_is_real(ಇದು)) {
				    ದ್ವಿಗುಣ val = 0.0;
				    ಆದರೆ (addr < vals->get_ಗಾತ್ರ())
					  vals->get_ಶಬ್ದ(addr, val);
				    vpip_real_get_ಮೌಲ್ಯ(val, cur->cb_ದತ್ತಾಂಶ.value);
			      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vals4) {
				    vpip_vec4_get_ಮೌಲ್ಯ(vals4->get_ಶಬ್ದ(addr),
							vals_ಅಗಲ,
							signed_flag,
							cur->cb_ದತ್ತಾಂಶ.value);
			      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dynamic_cast<vvp_darray_atom<int8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int64_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint64_t>*>(vals)
				      || dynamic_cast<vvp_darray_vec2*>(vals)) {
				    vvp_vector4_t val;
				    ಆದರೆ (addr < vals->get_ಗಾತ್ರ())
					  vals->get_ಶಬ್ದ(addr, val);
				    vpip_vec4_get_ಮೌಲ್ಯ(val,
							vals_ಅಗಲ,
							signed_flag,
							cur->cb_ದತ್ತಾಂಶ.value);
			      } ಇಲ್ಲದಿದ್ದರೆ {
			            ಪ್ರತಿಜ್ಞಾ(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (prev == 0) {

		  vpi_callbacks = ಮುಂದಿನ_2;
		  cur->ಮುಂದಿನ_2 = 0;
		  ಅಳಿಸು cur;

	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ಪ್ರತಿಜ್ಞಾ(prev->ಮುಂದಿನ_2 == cur);
		  prev->ಮುಂದಿನ_2 = ಮುಂದಿನ_2;
		  cur->ಮುಂದಿನ_2 = 0;
		  ಅಳಿಸು cur;
	    }
      }
}

ವರ್ಗ array_resolv_list_t : ಸಾರ್ವಜನಿಕ resolv_list_s {

    ಸಾರ್ವಜನಿಕ:
      ಸ್ಪಷ್ಟ array_resolv_list_t(ಅಕ್ಷರ*lab) : resolv_list_s(lab) {
	    array = 0;
      }

      vvp_array_t*array;
      ತಾರ್ಕಿಕ resolve(ತಾರ್ಕಿಕ mes) override;

    ಖಾಸಗಿ:
};

ತಾರ್ಕಿಕ array_resolv_list_t::resolve(ತಾರ್ಕಿಕ mes)
{
      *array = array_ಕಂಡುಹಿಡಿ(label());
      ಆದರೆ (*array == 0) {
	    ಪ್ರತಿಜ್ಞಾ(!mes);
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ವರ್ಗ array_ಪೋರ್ಟ್_resolv_list_t : ಸಾರ್ವಜನಿಕ resolv_list_s {

    ಸಾರ್ವಜನಿಕ:
      ಸ್ಪಷ್ಟ array_ಪೋರ್ಟ್_resolv_list_t(ಅಕ್ಷರ* lab, ತಾರ್ಕಿಕ use_addr__,
                                        ದೀರ್ಘ addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      ತಾರ್ಕಿಕ use_addr;
      ದೀರ್ಘ addr;
      ತಾರ್ಕಿಕ resolve(ತಾರ್ಕಿಕ mes) override;

    ಖಾಸಗಿ:
};

array_ಪೋರ್ಟ್_resolv_list_t::array_ಪೋರ್ಟ್_resolv_list_t(ಅಕ್ಷರ *lab, ತಾರ್ಕಿಕ use_addr__,
                                                   ದೀರ್ಘ addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      ಆದರೆ (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      ಇಲ್ಲದಿದ್ದರೆ
            context_scope = 0;
      ptr = ಹೊಸ vvp_net_t;
}

ತಾರ್ಕಿಕ array_ಪೋರ್ಟ್_resolv_list_t::resolve(ತಾರ್ಕಿಕ mes)
{
      vvp_array_t mem = array_ಕಂಡುಹಿಡಿ(label());
      ಆದರೆ (mem == 0) {
	    ಪ್ರತಿಜ್ಞಾ(mem || !mes);
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      }

      vvp_fun_arrayport*fun;
      ಆದರೆ (use_addr)
            ಆದರೆ (context_scope)
                  fun = ಹೊಸ vvp_fun_arrayport_aa(context_scope, mem, ptr, addr);
            ಇಲ್ಲದಿದ್ದರೆ
                  fun = ಹೊಸ vvp_fun_arrayport_sa(mem, ptr, addr);
      ಇಲ್ಲದಿದ್ದರೆ
            ಆದರೆ (context_scope)
                  fun = ಹೊಸ vvp_fun_arrayport_aa(context_scope, mem, ptr);
            ಇಲ್ಲದಿದ್ದರೆ
                  fun = ಹೊಸ vvp_fun_arrayport_sa(mem, ptr);
      ptr->fun = fun;

      array_attach_ಪೋರ್ಟ್(mem, fun);

      ಹಿಂತಿರುಗಿ ನಿಜ;
}

ವರ್ಗ array_ಶಬ್ದ_part_callback : ಸಾರ್ವಜನಿಕ array_ಶಬ್ದ_ಮೌಲ್ಯ_callback {
    ಸಾರ್ವಜನಿಕ:
      ಸ್ಪಷ್ಟ array_ಶಬ್ದ_part_callback(p_cb_ದತ್ತಾಂಶ ದತ್ತಾಂಶ, ದೀರ್ಘ addr);
      ~array_ಶಬ್ದ_part_callback() override;

      array_ಶಬ್ದ_part_callback(ಸ್ಥಿರಾಂಕ array_ಶಬ್ದ_part_callback&) = ಅಳಿಸು;
      array_ಶಬ್ದ_part_callback& ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ array_ಶಬ್ದ_part_callback&) = ಅಳಿಸು;

      ತಾರ್ಕಿಕ test_ಮೌಲ್ಯ_callback_ready(ಶೂನ್ಯ) override;

    ಖಾಸಗಿ:
      ಅಕ್ಷರ*ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು;
};

array_ಶಬ್ದ_part_callback::array_ಶಬ್ದ_part_callback(p_cb_ದತ್ತಾಂಶ ದತ್ತಾಂಶ, ದೀರ್ಘ addr)
: array_ಶಬ್ದ_ಮೌಲ್ಯ_callback(ದತ್ತಾಂಶ, addr)
{
	// Get the initial value of the part, to use as a reference.
      ರಚನೆ __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(ದತ್ತಾಂಶ->obj);
      s_vpi_ಮೌಲ್ಯ tmp_ಮೌಲ್ಯ;
      tmp_ಮೌಲ್ಯ.format = vpiBinStrVal;
      apvword->vpi_get_ಮೌಲ್ಯ(&tmp_ಮೌಲ್ಯ);

      ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು = ಹೊಸ ಅಕ್ಷರ[apvword->part_wid+1];

      ಸ್ಮರಣೆ_ನಕಲು(ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು, tmp_ಮೌಲ್ಯ.value.str, apvword->part_wid);
      ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು[apvword->part_wid] = 0;
}

array_ಶಬ್ದ_part_callback::~array_ಶಬ್ದ_part_callback()
{
      ಅಳಿಸು[]ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು;
}

ತಾರ್ಕಿಕ array_ಶಬ್ದ_part_callback::test_ಮೌಲ್ಯ_callback_ready(ಶೂನ್ಯ)
{
      ರಚನೆ __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_ದತ್ತಾಂಶ.obj);
      ಪ್ರತಿಜ್ಞಾ(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_ಮೌಲ್ಯ tmp_ಮೌಲ್ಯ;
      tmp_ಮೌಲ್ಯ.format = vpiBinStrVal;
      apvword->vpi_get_ಮೌಲ್ಯ(&tmp_ಮೌಲ್ಯ);

      ಆದರೆ (memcmp(ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು, tmp_ಮೌಲ್ಯ.value.str, apvword->part_wid) == 0)
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

      ಸ್ಮರಣೆ_ನಕಲು(ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು, tmp_ಮೌಲ್ಯ.value.str, apvword->part_wid);
      ಹಿಂತಿರುಗಿ ನಿಜ;

}

ಮೌಲ್ಯ_callback*vpip_array_ಶಬ್ದ_change(p_cb_ದತ್ತಾಂಶ ದತ್ತಾಂಶ)
{
      ರಚನೆ __vpiArray*ಪೋಷಕ = 0;
      array_ಶಬ್ದ_ಮೌಲ್ಯ_callback*cbh = 0;
      ಆದರೆ (ಸ್ಥಿರಾಂಕ ರಚನೆ __vpiArrayWord*ಶಬ್ದ = array_var_ಶಬ್ದ_from_handle(ದತ್ತಾಂಶ->obj)) {
	    ಪೋಷಕ = static_cast<__vpiArray*>(ಶಬ್ದ->get_ಪೋಷಕ());
	    ಚಿಹ್ನೆರಹಿತ addr = ಶಬ್ದ->get_ಸೂಚ್ಯಂಕ();
	    cbh = ಹೊಸ array_ಶಬ್ದ_ಮೌಲ್ಯ_callback(ದತ್ತಾಂಶ, addr);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ರಚನೆ __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(ದತ್ತಾಂಶ->obj)) {
	    ಪೋಷಕ = tword->array;
	    cbh = ಹೊಸ array_ಶಬ್ದ_ಮೌಲ್ಯ_callback(ದತ್ತಾಂಶ, tword->ವಿಳಾಸ);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ರಚನೆ __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(ದತ್ತಾಂಶ->obj)) {
	    ಪೋಷಕ = apvword->array;
	    cbh = ಹೊಸ array_ಶಬ್ದ_part_callback(ದತ್ತಾಂಶ, apvword->ಶಬ್ದ_sel);
      }

      ಪ್ರತಿಜ್ಞಾ(cbh);
      ಪ್ರತಿಜ್ಞಾ(ಪೋಷಕ);
      cbh->ಮುಂದಿನ_2 = ಪೋಷಕ->vpi_callbacks;
      ಪೋಷಕ->vpi_callbacks = cbh;

      ಹಿಂತಿರುಗಿ cbh;
}

ಮೌಲ್ಯ_callback* vpip_array_change(p_cb_ದತ್ತಾಂಶ ದತ್ತಾಂಶ)
{
      array_ಶಬ್ದ_ಮೌಲ್ಯ_callback*cbh = ಹೊಸ array_ಶಬ್ದ_ಮೌಲ್ಯ_callback(ದತ್ತಾಂಶ, -1);
      ಪ್ರತಿಜ್ಞಾ(ದತ್ತಾಂಶ->obj);

      ರಚನೆ __vpiArray*arr = dynamic_cast<__vpiArray*>(ದತ್ತಾಂಶ->obj);
      cbh->ಮುಂದಿನ_2 = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      ಹಿಂತಿರುಗಿ cbh;
}

ಶೂನ್ಯ compile_array_ಪೋರ್ಟ್(ಅಕ್ಷರ*label, ಅಕ್ಷರ*array, ಅಕ್ಷರ*addr)
{
      array_ಪೋರ್ಟ್_resolv_list_t*resolv_mem
	    = ಹೊಸ array_ಪೋರ್ಟ್_resolv_list_t(array, ಸುಳ್ಳು, 0);

      define_functor_ಚಿಹ್ನೆ(label, resolv_mem->ptr);
      ಬಿಡುಗಡೆ(label);
	// Connect the port-0 input as the address.
      ಒಳಹರಿವು_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

ಶೂನ್ಯ compile_array_ಪೋರ್ಟ್(ಅಕ್ಷರ*label, ಅಕ್ಷರ*array, ದೀರ್ಘ addr)
{
      array_ಪೋರ್ಟ್_resolv_list_t*resolv_mem
	    = ಹೊಸ array_ಪೋರ್ಟ್_resolv_list_t(array, ನಿಜ, addr);

      define_functor_ಚಿಹ್ನೆ(label, resolv_mem->ptr);
      ಬಿಡುಗಡೆ(label);

      resolv_submit(resolv_mem);
}

ಶೂನ್ಯ compile_array_alias(ಅಕ್ಷರ*label, ಅಕ್ಷರ*name, ಅಕ್ಷರ*src)
{
      vvp_array_t mem = array_ಕಂಡುಹಿಡಿ(src);
      ಪ್ರತಿಜ್ಞಾ(mem);

      ರಚನೆ __vpiArray*obj = ಹೊಸ __vpiArray;

      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_ಎಣಿಕೆ = mem->array_ಎಣಿಕೆ;
      obj->signed_flag = mem->signed_flag;

	// Need to set an accurate range of addresses.
      obj->ಮೊದಲ_addr = mem->ಮೊದಲ_addr;
      obj->ಕೊನೆಯ_addr  = mem->ಕೊನೆಯ_addr;
      obj->ಅದಲುಬದಲು_addr = mem->ಅದಲುಬದಲು_addr;

      obj->msb = mem->msb;
      obj->lsb = mem->lsb;

	// Share the words with the source array.
      obj->nets = mem->nets;
      obj->vals4 = mem->vals4;
      obj->vals  = mem->vals;
      obj->vals_ಅಗಲ = mem->vals_ಅಗಲ;
      obj->vals_ಪದಗಳು = mem->vals_ಪದಗಳು;

      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

      ಪ್ರತಿಜ್ಞಾ(array_ಕೋಷ್ಟಕ);
      ಪ್ರತಿಜ್ಞಾ(!array_ಕಂಡುಹಿಡಿ(label));
      array_ಕೋಷ್ಟಕ->sym_ಗಣ_ಮೌಲ್ಯ(label, obj);

      compile_vpi_ಚಿಹ್ನೆ(label, obj);
      vpip_attach_ಗಮ್ಯ_current_scope(obj);

      ಬಿಡುಗಡೆ(label);
      ಬಿಡುಗಡೆ(name);
      ಬಿಡುಗಡೆ(src);
}

/*
 * &A<label,addr>
 * This represents a VPI handle for an addressed array. This comes
 * from expressions like "label[addr]" where "label" is the array and
 * "addr" is the canonical address of the desired word.
 */
vpiHandle vpip_make_vthr_A(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ addr)
{
      ರಚನೆ __vpiArrayVthrA*obj = ಹೊಸ __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = ಹೊಸ array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->ವಿಳಾಸ_handle = 0;
      obj->ವಿಳಾಸ = addr;

      ಹಿಂತಿರುಗಿ obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(ಅಕ್ಷರ*label, ಅಕ್ಷರ*ಚಿಹ್ನೆ)
{
      ರಚನೆ __vpiArrayVthrA*obj = ಹೊಸ __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = ಹೊಸ array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->ವಿಳಾಸ_handle = 0;
      compile_vpi_lookup(&obj->ವಿಳಾಸ_handle, ಚಿಹ್ನೆ);
      obj->ವಿಳಾಸ = 0;

      ಹಿಂತಿರುಗಿ obj;
}

vpiHandle vpip_make_vthr_A(ಅಕ್ಷರ*label, vpiHandle handle)
{
      ರಚನೆ __vpiArrayVthrA*obj = ಹೊಸ __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = ಹೊಸ array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->ವಿಳಾಸ_handle = handle;
      obj->ವಿಳಾಸ = 0;

      ಹಿಂತಿರುಗಿ obj;
}

vpiHandle vpip_make_vthr_APV(ಅಕ್ಷರ*label, ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ, ಚಿಹ್ನೆರಹಿತ ಬಿಟ್, ಚಿಹ್ನೆರಹಿತ wid)
{
      ರಚನೆ __vpiArrayVthrAPV*obj = ಹೊಸ __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = ಹೊಸ array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->ಶಬ್ದ_sel = ಸೂಚ್ಯಂಕ;
      obj->part_ಬಿಟ್ = ಬಿಟ್;
      obj->part_wid = wid;

      ಹಿಂತಿರುಗಿ obj;
}

ಶೂನ್ಯ compile_array_cleanup(ಶೂನ್ಯ)
{
      ಅಳಿಸು array_ಕೋಷ್ಟಕ;
      array_ಕೋಷ್ಟಕ = 0;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ ಸ್ಮರಣೆ_delete(vpiHandle ಅಂಶ)
{
      ರಚನೆ __vpiArray*arr = static_cast<ರಚನೆ __vpiArray*>(ಅಂಶ);
      ಆದರೆ (arr->vals_ಪದಗಳು) ಅಳಿಸು [] (arr->vals_ಪದಗಳು-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      ಅಳಿಸು arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      ಅಳಿಸು arr->vals;

      ಆದರೆ (arr->nets) {
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < arr->get_ಗಾತ್ರ(); idx += 1) {
		  ಆದರೆ (ರಚನೆ __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.ಸೂಚ್ಯಂಕ);
		    /* These should only be the real words. */
		  } ಇಲ್ಲದಿದ್ದರೆ {
			ಪ್ರತಿಜ್ಞಾ(arr->nets[idx]->get_type_ಸಂಕೇತ() ==
			       vpiRealVar);
			ರಚನೆ __vpiRealVar *sigr = static_cast<ರಚನೆ __vpiRealVar *>
			                            (arr->nets[idx]);
			constant_delete(sigr->id.ಸೂಚ್ಯಂಕ);
// Why are only the real words still here?
			ಅಳಿಸು arr->nets[idx];
		  }
	    }
	    ಬಿಡುಗಡೆ(arr->nets);
      }

      ಆಗ (arr->vpi_callbacks) {
	    ರಚನೆ __vpiCallback*tmp = arr->vpi_callbacks->ಮುಂದಿನ_2;
	    ಅಳಿಸು arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      ಅಳಿಸು arr;
}

ಶೂನ್ಯ A_delete(vpiHandle ಅಂಶ)
{
      ರಚನೆ __vpiArrayVthrA*obj = static_cast<ರಚನೆ __vpiArrayVthrA*>(ಅಂಶ);
      ಆದರೆ (obj->ವಿಳಾಸ_handle) {
	    ಆಯ್ಕೆ (obj->ವಿಳಾಸ_handle->get_type_ಸಂಕೇತ()) {
		ಸ್ಥಿತಿ vpi_ಸ್ಮರಣೆ_ಶಬ್ದ:
		  ಆದರೆ (vpi_get(_vpiFromThr, obj->ವಿಳಾಸ_handle) == _vpi_at_A) {
			A_delete(obj->ವಿಳಾಸ_handle);
		  }
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ vpiPartSelect:
		  ಪ್ರತಿಜ್ಞಾ(vpi_get(_vpiFromThr, obj->ವಿಳಾಸ_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->ವಿಳಾಸ_handle);
		  ನಿಲ್ಲಿಸು;
	    }
      }

      ಅಳಿಸು obj;
}

ಶೂನ್ಯ APV_delete(vpiHandle ಅಂಶ)
{
      ರಚನೆ __vpiArrayVthrAPV*obj = static_cast<ರಚನೆ __vpiArrayVthrAPV*>(ಅಂಶ);
      ಅಳಿಸು obj;
}
#ಆದರೆ_ಅಂತ್ಯ
