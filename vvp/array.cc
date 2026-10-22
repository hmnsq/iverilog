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
#ifdef CHECK_WITH_VALGRIND
#ಅನ್ತರ್ಭಾವ  "vvp_cleanup.h"
#endif
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <iostream>
# include  "compile.h"
# include  <cassert>
# include  "ivl_alloc.h"

using ನಾಮಾಕಾಶ std;

unsigned long ಎಣಿಕೆ_net_arrays = 0;
unsigned long ಎಣಿಕೆ_net_array_ಪದಗಳು = 0;
unsigned long ಎಣಿಕೆ_var_arrays = 0;
unsigned long ಎಣಿಕೆ_var_array_ಪದಗಳು = 0;
unsigned long ಎಣಿಕೆ_real_arrays = 0;
unsigned long ಎಣಿಕೆ_real_array_ಪದಗಳು = 0;

ಸ್ಥಿರ ಚಿಹ್ನೆ_map_s<ಸಂರಚನಾ __vpiArray>* array_table =0;

ವರ್ಗ vvp_fun_arrayport;
ಸ್ಥಿರ ಶೂನ್ಯ array_attach_port(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_find(ಧ್ರುವ ವರ್ಣ*label)
{
      ಯದಿ (array_table == 0)
	    ಪ್ರತ್ಯಾಗಮ 0;

      vvp_array_t v = array_table->sym_get_ಮೌಲ್ಯ(label);
      ಪ್ರತ್ಯಾಗಮ v;
}

ಸಂರಚನಾ __vpiArrayVthrA : ಸಾರ್ವಜನಿಕ __vpiHandle {
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiMemoryWord; }
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ವರ್ಣ* vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val, ಪೂರ್ಣಾಙ್ಕ flags) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code) override;

      ಸಂರಚನಾ __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle address_handle;
	// If wid==0, then address is the address into the array.
      unsigned address;

      unsigned get_address() ಧ್ರುವ
      {
	    ಯದಿ (address_handle) {
		  s_vpi_ಮೌಲ್ಯ vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpiVectorVal;
		  address_handle->vpi_get_ಮೌಲ್ಯ(&vp);
		  ಪೂರ್ಣಾಙ್ಕ ಪದಗಳು = (address_handle->vpi_get(vpiSize)-1)/32 + 1;
		  ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ idx = 0; idx < ಪದಗಳು; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			ಯದಿ (vp.value.vector[idx].bval != 0) ಪ್ರತ್ಯಾಗಮ UINT_MAX;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  address_handle->vpi_get_ಮೌಲ್ಯ(&vp);
		  ಪ್ರತ್ಯಾಗಮ vp.value.integer;
	    }

	    ಪ್ರತ್ಯಾಗಮ address;
      }
};


ಸಂರಚನಾ __vpiArrayVthrAPV : ಸಾರ್ವಜನಿಕ __vpiHandle {
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiPartSelect; }
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ವರ್ಣ* vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code) override;

      ಸಂರಚನಾ __vpiArray*array;
      unsigned word_sel;
      unsigned part_bit;
      unsigned part_wid;
};

ತಾರ್ಕಿಕ is_net_array(vpiHandle obj)
{
      ಸಂರಚನಾ __vpiArray*rfp = dynamic_cast<__vpiArray*> (obj);
      ಪ್ರತಿಜ್ಞಾ(rfp);

      ಯದಿ (rfp->nets != 0) ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
      ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

// This function return true if the underlying array words are real.
ಸ್ಥಿರ ತಾರ್ಕಿಕ vpi_array_is_real(ಧ್ರುವ vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      ಯದಿ (arr->vals4 != 0)  // A bit based variable/register array.
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

      ಯದಿ (dynamic_cast<vvp_darray_real*> (arr->vals))
	    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;

      ಯದಿ (arr->vals != 0)
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	// This must be a net array so look at element 0 to find the type.
      ಪ್ರತಿಜ್ಞಾ(arr->nets != 0);
      ಪ್ರತಿಜ್ಞಾ(arr->get_size() > 0);
      ಧ್ರುವ ಸಂರಚನಾ __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      ಯದಿ (rsig) {
	    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
      }

      ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ vpi_array_is_string(ಧ್ರುವ vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      ಯದಿ (arr->vals4 != 0)  // A bit based variable/register array.
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

      ಯದಿ (dynamic_cast<vvp_darray_string*> (arr->vals))
	    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;

      ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಪೂರ್ಣಾಙ್ಕ __vpiArray::get_word_size() ಧ್ರುವ
{
      unsigned width;

      ಪ್ರತಿಜ್ಞಾ(get_size() > 0);
	/* For a net array we need to get the width from the first element. */
      ಯದಿ (nets) {
	    ಪ್ರತಿಜ್ಞಾ(vals4 == 0 && vals == 0);
	    ಧ್ರುವ ಸಂರಚನಾ __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    ಪ್ರತಿಜ್ಞಾ(vsig);
	    width = vpip_size(vsig);
	/* For a variable array we can get the width from vals_width. */
      } ಅನ್ಯಥಾ {
	    ಪ್ರತಿಜ್ಞಾ(vals4 || vals);
	    width = vals_width;
      }

      ಪ್ರತ್ಯಾಗಮ width;
}

ವರ್ಣ*__vpiArray::get_word_str(ಸಂರಚನಾ __vpiArrayWord*word, ಪೂರ್ಣಾಙ್ಕ code)
{
      unsigned index = word->get_index();

      ಯದಿ (code == vpiFile) {  // Not implemented for now!
	    ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(file_names[0]);
      }

      ವರ್ಣ sidx [64];
      snprintf(sidx, 63, "%d", (ಪೂರ್ಣಾಙ್ಕ)index + first_addr.get_ಮೌಲ್ಯ());
      ಪ್ರತ್ಯಾಗಮ generic_get_str(code, get_scope(), name, sidx);
}

ಶೂನ್ಯ __vpiArray::get_word_ಮೌಲ್ಯ(ಸಂರಚನಾ __vpiArrayWord*word, p_vpi_ಮೌಲ್ಯ vp)
{
      unsigned index = word->get_index();

    // Determine the appropriate format (The Verilog PLI Handbook 5.2.10)
      ಯದಿ(vp->format == vpiObjTypeVal) {
          ಯದಿ(vpi_array_is_real(ಏತತ್))
              vp->format = vpiRealVal;
          ಅನ್ಯಥಾ ಯದಿ(vpi_array_is_string(ಏತತ್))
              vp->format = vpiStringVal;
          ಅನ್ಯಥಾ
              vp->format = vpiIntVal;
      }

      ಯದಿ(vals4) {
          vpip_vec4_get_ಮೌಲ್ಯ(vals4->get_word(index),
                              vals4->width(), signed_flag, vp);
      } ಅನ್ಯಥಾ ಯದಿ(vals) {
          ವಿಕಲ್ಪ(vp->format) {
            ಪಕ್ಷ vpiBinStrVal:
            ಪಕ್ಷ vpiOctStrVal:
            ಪಕ್ಷ vpiDecStrVal:
            ಪಕ್ಷ vpiHexStrVal:
            ಪಕ್ಷ vpiScalarVal:
            ಪಕ್ಷ vpiIntVal:
            ಪಕ್ಷ vpiVectorVal:
            {
                vvp_vector4_t v;
                vals->get_word(index, v);
                vpip_vec4_get_ಮೌಲ್ಯ(v, vals_width, signed_flag, vp);
            }
            ಭಙ್ಗ;

            ಪಕ್ಷ vpiRealVal:
            {
                ದ್ವಿಗುಣ d;
                vals->get_word(index, d);
                vpip_real_get_ಮೌಲ್ಯ(d, vp);
            }
            ಭಙ್ಗ;

            ಪಕ್ಷ vpiStringVal:
            {
                string s;
                vals->get_word(index, s);
                vpip_string_get_ಮೌಲ್ಯ(s, vp);
            }
            ಭಙ್ಗ;

            ಸ್ವಭಾವ:
                fprintf(stderr, "vpi sorry: format is not implemented\n");
                ಪ್ರತಿಜ್ಞಾ(ಅಸತ್ಯ);
          }
      }
}

ಶೂನ್ಯ __vpiArray::put_word_ಮೌಲ್ಯ(ಸಂರಚನಾ __vpiArrayWord*word, p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಙ್ಕ)
{
      unsigned index = word->get_index();
      vvp_vector4_t val = vec4_from_vpi_ಮೌಲ್ಯ(vp, vals_width);
      set_word(index, 0, val);
}

vpiHandle __vpiArray::get_iter_index(ಸಂರಚನಾ __vpiArrayIterator*, ಪೂರ್ಣಾಙ್ಕ idx)
{
      ಯದಿ (nets) ಪ್ರತ್ಯಾಗಮ nets[idx];

      ಪ್ರತಿಜ್ಞಾ(vals4 || vals);

      ಯದಿ (vals_ಪದಗಳು == 0) make_vals_ಪದಗಳು();

      ಪ್ರತ್ಯಾಗಮ &(vals_ಪದಗಳು[idx].as_word);
}

ಪೂರ್ಣಾಙ್ಕ __vpiArray::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpi_ಸಾಲು_No:
	    ಪ್ರತ್ಯಾಗಮ 0; // Not implemented for now!

	  ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ get_size();

	  ಪಕ್ಷ vpiAutomatic:
	    ಪ್ರತ್ಯಾಗಮ scope->is_automatic()? 1 : 0;

	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}

ವರ್ಣ* __vpiArray::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{
      ಯದಿ (code == vpiFile) {  // Not implemented for now!
            ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(file_names[0]);
      }

      ಪ್ರತ್ಯಾಗಮ generic_get_str(code, scope, name, ರಿಕ್ತ);
}

vpiHandle __vpiArray::vpi_handle(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
          ಪಕ್ಷ vpi_ಎಡ_Range:
            ಯದಿ (swap_addr) ಪ್ರತ್ಯಾಗಮ &last_addr;
            ಅನ್ಯಥಾ ಪ್ರತ್ಯಾಗಮ &first_addr;

          ಪಕ್ಷ vpi_ಬಲ_Range:
            ಯದಿ (swap_addr) ಪ್ರತ್ಯಾಗಮ &first_addr;
            ಅನ್ಯಥಾ ಪ್ರತ್ಯಾಗಮ &last_addr;

	  ಪಕ್ಷ vpiScope:
	    ಪ್ರತ್ಯಾಗಮ scope;

	  ಪಕ್ಷ vpiModule:
	    ಪ್ರತ್ಯಾಗಮ vpip_module(scope);
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

/*
* VPI code passes indices that are not yet converted to canonical
* form, so this index function does it here.
*/
vpiHandle __vpiArray::vpi_index(ಪೂರ್ಣಾಙ್ಕ index)
{
      index -= first_addr.get_ಮೌಲ್ಯ();
      ಯದಿ (index >= (long) get_size())
	    ಪ್ರತ್ಯಾಗಮ 0;
      ಯದಿ (index < 0)
	    ಪ್ರತ್ಯಾಗಮ 0;

      ಯದಿ (nets != 0) {
	    ಪ್ರತ್ಯಾಗಮ nets[index];
      }

      ಯದಿ (vals_ಪದಗಳು == 0)
	    make_vals_ಪದಗಳು();

      ಪ್ರತ್ಯಾಗಮ &(vals_ಪದಗಳು[index].as_word);
}

ಪೂರ್ಣಾಙ್ಕ __vpiArrayWord::as_word_t::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ಸಂರಚನಾ __vpiArrayWord*obj = array_var_word_from_handle(ಏತತ್);
      ಪ್ರತಿಜ್ಞಾ(obj);
      ಸಂರಚನಾ __vpiArrayBase*my_parent = obj->get_parent();
      t_vpi_ಮೌಲ್ಯ val;

      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpi_ಸಾಲು_No:
	    ಪ್ರತ್ಯಾಗಮ 0; // Not implemented for now!

	  ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ my_parent->get_word_size();

	  ಪಕ್ಷ vpi_ಎಡ_Range:
            val.format = vpiIntVal;
	    my_parent->get_ಎಡ_range()->vpi_get_ಮೌಲ್ಯ(&val);
            ಪ್ರತಿಜ್ಞಾ(val.format == vpiIntVal);
            ಪ್ರತ್ಯಾಗಮ val.value.integer;

	  ಪಕ್ಷ vpi_ಬಲ_Range:
            val.format = vpiIntVal;
	    my_parent->get_ಬಲ_range()->vpi_get_ಮೌಲ್ಯ(&val);
            ಪ್ರತಿಜ್ಞಾ(val.format == vpiIntVal);
            ಪ್ರತ್ಯಾಗಮ val.value.integer;

	  ಪಕ್ಷ vpiIndex:
	    {
		  ಪೂರ್ಣಾಙ್ಕ base_offset = 0;
		  ಸಂರಚನಾ __vpiArray*base = dynamic_cast<__vpiArray*> (my_parent);
		  ಯದಿ (base) {
			val.format = vpiIntVal;
			base->first_addr.vpi_get_ಮೌಲ್ಯ(&val);
			base_offset += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_index.vpi_get_ಮೌಲ್ಯ(&val);
		  ಪ್ರತಿಜ್ಞಾ(val.format == vpiIntVal);
		  ಪ್ರತ್ಯಾಗಮ val.value.integer + base_offset;
	    }

	  ಪಕ್ಷ vpiAutomatic:
	    ಪ್ರತ್ಯಾಗಮ my_parent->get_scope()->is_automatic()? 1 : 0;

#ಯದಿ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ಪಕ್ಷ _vpiFromThr:
	    ಪ್ರತ್ಯಾಗಮ _vpiNoThr;
#endif

	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}


ಪೂರ್ಣಾಙ್ಕ __vpiArrayVthrA::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpi_ಸಾಲು_No:
	    ಪ್ರತ್ಯಾಗಮ 0; // Not implemented for now!

	  ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ array->get_word_size();

	  ಪಕ್ಷ vpi_ಎಡ_Range:
	    ಪ್ರತ್ಯಾಗಮ array->msb.get_ಮೌಲ್ಯ();

	  ಪಕ್ಷ vpi_ಬಲ_Range:
	    ಪ್ರತ್ಯಾಗಮ array->lsb.get_ಮೌಲ್ಯ();

	  ಪಕ್ಷ vpiIndex:
	    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)get_address() + array->first_addr.get_ಮೌಲ್ಯ();

	  ಪಕ್ಷ vpiAutomatic:
	    ಪ್ರತ್ಯಾಗಮ array->get_scope()->is_automatic() ? 1 : 0;

#ಯದಿ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ಪಕ್ಷ _vpiFromThr:
	    ಪ್ರತ್ಯಾಗಮ _vpi_at_A;
#endif

	  // If address_handle is not zero we definitely have a
	  // variable.
	  ಪಕ್ಷ vpiConstantSelect:
	    ಪ್ರತ್ಯಾಗಮ address_handle == 0;

	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}

ವರ್ಣ* __vpiArrayVthrA::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{
      ಯದಿ (code == vpiFile) {  // Not implemented for now!
            ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(file_names[0]);
      }

      ವರ್ಣ sidx [64];
      snprintf(sidx, 63, "%d", (ಪೂರ್ಣಾಙ್ಕ)get_address() + array->first_addr.get_ಮೌಲ್ಯ());
      ಪ್ರತ್ಯಾಗಮ generic_get_str(code, array->get_scope(), array->name, sidx);
}

ಶೂನ್ಯ __vpiArrayVthrA::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp)
{
      ಪ್ರತಿಜ್ಞಾ(array);

      unsigned index = get_address();
      ಯದಿ (vpi_array_is_real(array)) {
	    ದ್ವಿಗುಣ tmp = array->get_word_r(index);
	    vpip_real_get_ಮೌಲ್ಯ(tmp, vp);
      } ಅನ್ಯಥಾ ಯದಿ (vpi_array_is_string(array)) {
	    string tmp = array->get_word_str(index);
	    vpip_string_get_ಮೌಲ್ಯ(tmp, vp);
      } ಅನ್ಯಥಾ {
	    vvp_vector4_t tmp = array->get_word(index);
	    unsigned width = array->get_word_size();
	    vpip_vec4_get_ಮೌಲ್ಯ(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಙ್ಕ)
{
      unsigned index = get_address();

      ಪ್ರತಿಜ್ಞಾ(array);
      ಪ್ರತಿಜ್ಞಾ(index < array->get_size());

      ಯದಿ (vpi_array_is_real(array)) {
	    ದ್ವಿಗುಣ val = real_from_vpi_ಮೌಲ್ಯ(vp);
	    array->set_word(index, val);
      } ಅನ್ಯಥಾ {
	    unsigned width = array->get_word_size();
	    vvp_vector4_t val = vec4_from_vpi_ಮೌಲ್ಯ(vp, width);
	    array->set_word(index, 0, val);
      }

      ಪ್ರತ್ಯಾಗಮ ಏತತ್;
}

vpiHandle __vpiArrayVthrA::vpi_handle(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiIndex:
	    ಭಙ್ಗ;  // Not implemented!

	  ಪಕ್ಷ vpi_ಎಡ_Range:
	    ಪ್ರತ್ಯಾಗಮ &array->msb;

	  ಪಕ್ಷ vpi_ಬಲ_Range:
	    ಪ್ರತ್ಯಾಗಮ &array->lsb;

	  ಪಕ್ಷ vpiParent:
	  ಪಕ್ಷ vpiArray:
	    ಪ್ರತ್ಯಾಗಮ array;

	  ಪಕ್ಷ vpiScope:
	    ಪ್ರತ್ಯಾಗಮ array->get_scope();

	  ಪಕ್ಷ vpiModule:
	    ಪ್ರತ್ಯಾಗಮ vpip_module(array->get_scope());
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}


ಪೂರ್ಣಾಙ್ಕ __vpiArrayVthrAPV::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpi_ಸಾಲು_No:
	    ಪ್ರತ್ಯಾಗಮ 0; // Not implemented for now!

	  ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ part_wid;

	  ಪಕ್ಷ vpi_ಎಡ_Range:
	    ಪ್ರತ್ಯಾಗಮ part_bit + part_wid - 1;

	  ಪಕ್ಷ vpi_ಬಲ_Range:
	    ಪ್ರತ್ಯಾಗಮ part_bit;

	  ಪಕ್ಷ vpiAutomatic:
	    ಪ್ರತ್ಯಾಗಮ array->get_scope()->is_automatic() ? 1 : 0;

#ಯದಿ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ಪಕ್ಷ _vpiFromThr:
	    ಪ್ರತ್ಯಾಗಮ _vpi_at_APV;
#endif

	  ಪಕ್ಷ vpiConstantSelect:
	    ಪ್ರತ್ಯಾಗಮ 1;

	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}

ವರ್ಣ* __vpiArrayVthrAPV::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{
      ಯದಿ (code == vpiFile) {  // Not implemented for now!
            ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(file_names[0]);
      }

      ವರ್ಣ sidx [64];
      snprintf(sidx, 63, "%u", word_sel + array->first_addr.get_ಮೌಲ್ಯ());
      ಪ್ರತ್ಯಾಗಮ generic_get_str(code, array->get_scope(), array->name, sidx);
}

ಶೂನ್ಯ __vpiArrayVthrAPV::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp)
{
      ಪ್ರತಿಜ್ಞಾ(array);

      unsigned index = word_sel;
      ಯದಿ (vpi_array_is_real(array)) {
	    ದ್ವಿಗುಣ tmp = array->get_word_r(index);
	    vpip_real_get_ಮೌಲ್ಯ(tmp, vp);
      } ಅನ್ಯಥಾ {
	    vvp_vector4_t tmp = array->get_word(index);
	    tmp = tmp.subvalue(part_bit, part_wid);
	    vpip_vec4_get_ಮೌಲ್ಯ(tmp, part_wid, ಅಸತ್ಯ, vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  ಪಕ್ಷ vpiParent:
	    ಪ್ರತ್ಯಾಗಮ 0;

            // Not part of the Verilog standard. We use this internally.
	  ಪಕ್ಷ vpiArray:
	    ಪ್ರತ್ಯಾಗಮ array;

	  ಪಕ್ಷ vpiScope:
	    ಪ್ರತ್ಯಾಗಮ array->get_scope();

	  ಪಕ್ಷ vpiModule:
	    ಪ್ರತ್ಯಾಗಮ vpip_module(array->get_scope());
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ __vpiArray::set_word(unsigned address, unsigned part_off, ಧ್ರುವ vvp_vector4_t&val)
{
      ಯದಿ (address >= get_size())
	    ಪ್ರತ್ಯಾಗಮ;

      ಯದಿ (vals4) {
	    ಪ್ರತಿಜ್ಞಾ(nets == 0);
	    ಯದಿ (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp = vals4->get_word(address);
		  ಯದಿ ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			ಪ್ರತಿಜ್ಞಾ(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals4->set_word(address, tmp);
	    } ಅನ್ಯಥಾ {
		  vals4->set_word(address, val);
	    }
	    word_change(address);
	    ಪ್ರತ್ಯಾಗಮ;
      }

      ಯದಿ (vals) {
	    ಪ್ರತಿಜ್ಞಾ(nets == 0);
	    ಯದಿ (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp;
		  vals->get_word(address, tmp);
		  ಯದಿ ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			ಪ್ರತಿಜ್ಞಾ(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals->set_word(address, tmp);
	    } ಅನ್ಯಥಾ {
		  vals->set_word(address, val);
	    }
	    word_change(address);
	    ಪ್ರತ್ಯಾಗಮ;
      }

      ಪ್ರತಿಜ್ಞಾ(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle word = nets[address];
      ಧ್ರುವ ಸಂರಚನಾ __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      ಪ್ರತಿಜ್ಞಾ(vsig);

      vsig->ಗಂಟು->send_vec4_pv(val, part_off, vpip_size(vsig), 0);
      word_change(address);
}

ಶೂನ್ಯ __vpiArray::set_word(unsigned address, ದ್ವಿಗುಣ val)
{
      ಪ್ರತಿಜ್ಞಾ(vals != 0);
      ಪ್ರತಿಜ್ಞಾ(nets == 0);

      ಯದಿ (address >= vals->get_size())
	    ಪ್ರತ್ಯಾಗಮ;

      vals->set_word(address, val);
      word_change(address);
}

ಶೂನ್ಯ __vpiArray::set_word(unsigned address, ಧ್ರುವ string&val)
{
      ಪ್ರತಿಜ್ಞಾ(vals != 0);
      ಪ್ರತಿಜ್ಞಾ(nets == 0);

      ಯದಿ (address >= vals->get_size())
	    ಪ್ರತ್ಯಾಗಮ;

      vals->set_word(address, val);
      word_change(address);
}

ಶೂನ್ಯ __vpiArray::set_word(unsigned address, ಧ್ರುವ vvp_object_t&val)
{
      ಪ್ರತಿಜ್ಞಾ(vals != 0);
      ಪ್ರತಿಜ್ಞಾ(nets == 0);

      ಯದಿ (address >= vals->get_size())
	    ಪ್ರತ್ಯಾಗಮ;

      vals->set_word(address, val);
      word_change(address);
}

vvp_vector4_t __vpiArray::get_word(unsigned address)
{
      ಯದಿ (vals4) {
	    ಪ್ರತಿಜ್ಞಾ(nets == 0);
	    ಪ್ರತಿಜ್ಞಾ(vals == 0);
	    ಪ್ರತ್ಯಾಗಮ vals4->get_word(address);
      }

      ಯದಿ (vals) {
	    ಪ್ರತಿಜ್ಞಾ(nets == 0);
	    ಪ್ರತಿಜ್ಞಾ(vals4== 0);
	    ಯದಿ (address >= vals->get_size())
		  ಪ್ರತ್ಯಾಗಮ vvp_vector4_t(vals_width, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_word(address, val);
	    ಪ್ರತ್ಯಾಗಮ val;
      }

      ಪ್ರತಿಜ್ಞಾ(vals4 == 0);
      ಪ್ರತಿಜ್ಞಾ(vals == 0);
      ಪ್ರತಿಜ್ಞಾ(nets != 0);

      ಯದಿ (address >= get_size()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    ಪ್ರತಿಜ್ಞಾ(get_size() > 0);
	    vpiHandle word = nets[0];
	    ಪ್ರತಿಜ್ಞಾ(word);
	    ಸಂರಚನಾ __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
	    ಪ್ರತಿಜ್ಞಾ(vsig);
	    vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*> (vsig->ಗಂಟು->fil);
	    ಪ್ರತಿಜ್ಞಾ(sig);
	    ಪ್ರತ್ಯಾಗಮ vvp_vector4_t(sig->ಮೌಲ್ಯ_size(), BIT4_X);
      }

      vpiHandle word = nets[address];
      ಸಂರಚನಾ __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      ಪ್ರತಿಜ್ಞಾ(vsig);
      vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*> (vsig->ಗಂಟು->fil);
      ಪ್ರತಿಜ್ಞಾ(sig);

      vvp_vector4_t val;
      sig->vec4_ಮೌಲ್ಯ(val);
      ಪ್ರತ್ಯಾಗಮ val;
}

ದ್ವಿಗುಣ __vpiArray::get_word_r(unsigned address)
{
      ಯದಿ (vals) {
	    ಪ್ರತಿಜ್ಞಾ(vals4 == 0);
	    ಪ್ರತಿಜ್ಞಾ(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    ಯದಿ (address >= vals->get_size())
		  ಪ್ರತ್ಯಾಗಮ 0.0;

	    ದ್ವಿಗುಣ val;
	    vals->get_word(address, val);
	    ಪ್ರತ್ಯಾಗಮ val;
      }

      ಯದಿ (address >= get_size())
	    ಪ್ರತ್ಯಾಗಮ 0.0;

      ಪ್ರತಿಜ್ಞಾ(nets);
      vpiHandle word = nets[address];
      ಸಂರಚನಾ __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(word);
      ಪ್ರತಿಜ್ಞಾ(vsig);
      vvp_signal_ಮೌಲ್ಯ*sig = dynamic_cast<vvp_signal_ಮೌಲ್ಯ*> (vsig->net->fil);
      ಪ್ರತಿಜ್ಞಾ(sig);

      ದ್ವಿಗುಣ val = sig->real_ಮೌಲ್ಯ();
      ಪ್ರತ್ಯಾಗಮ val;
}

ಶೂನ್ಯ __vpiArray::get_word_obj(unsigned address, vvp_object_t&val)
{
      ಯದಿ (vals) {
	    ಪ್ರತಿಜ್ಞಾ(vals4 == 0);
	    ಪ್ರತಿಜ್ಞಾ(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    ಯದಿ (address >= vals->get_size()) {
		  val = vvp_object_t();
		  ಪ್ರತ್ಯಾಗಮ;
	    }

	    vals->get_word(address, val);
	    ಪ್ರತ್ಯಾಗಮ;
      }

      ಪ್ರತಿಜ್ಞಾ(nets);
	// Arrays of string nets not implemented!
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ;
}

string __vpiArray::get_word_str(unsigned address)
{
      ಯದಿ (vals) {
	    ಪ್ರತಿಜ್ಞಾ(vals4 == 0);
	    ಪ್ರತಿಜ್ಞಾ(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    ಯದಿ (address >= vals->get_size())
		  ಪ್ರತ್ಯಾಗಮ "";

	    string val;
	    vals->get_word(address, val);
	    ಪ್ರತ್ಯಾಗಮ val;
      }

      ಪ್ರತಿಜ್ಞಾ(nets);
	// Arrays of string nets not implemented!
      ಪ್ರತಿಜ್ಞಾ(0);
      ಪ್ರತ್ಯಾಗಮ "";
}

vpiHandle vpip_make_array(ಧ್ರುವ ವರ್ಣ*label, ಧ್ರುವ ವರ್ಣ*name,
                          ಪೂರ್ಣಾಙ್ಕ first_addr, ಪೂರ್ಣಾಙ್ಕ last_addr,
                          ತಾರ್ಕಿಕ signed_flag)
{
      ಸಂರಚನಾ __vpiArray*obj = ನೂತನ __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      ಯದಿ (last_addr >= first_addr) {
	    obj->swap_addr = ಅಸತ್ಯ;
      } ಅನ್ಯಥಾ {
	    obj->swap_addr = ಸತ್ಯ;
	    ಪೂರ್ಣಾಙ್ಕ tmp = last_addr;
	    last_addr = first_addr;
	    first_addr = tmp;
      }
      ಪ್ರತಿಜ್ಞಾ(last_addr >= first_addr);
      unsigned array_ಎಣಿಕೆ = last_addr+1-first_addr;

	// For now, treat all arrays as memories. This is not quite
	// correct, as arrays are arrays with memories a special case.
      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_ಎಣಿಕೆ = array_ಎಣಿಕೆ;

      obj->first_addr.set_ಮೌಲ್ಯ(first_addr);
      obj->last_addr .set_ಮೌಲ್ಯ(last_addr);

	// Start off now knowing if we are nets or variables.
      obj->nets = 0;
      obj->vals4 = 0;
      obj->vals  = 0;
      obj->vals_width = 0;
      obj->vals_ಪದಗಳು = 0;

	// Initialize (clear) the read-ports list.
      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

	/* Add this symbol to the array_symbols table for later lookup. */
      ಯದಿ (!array_table)
	    array_table = ನೂತನ ಚಿಹ್ನೆ_map_s<ಸಂರಚನಾ __vpiArray>;

      ಪ್ರತಿಜ್ಞಾ(!array_find(label));
      array_table->sym_set_ಮೌಲ್ಯ(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_ಚಿಹ್ನೆ(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_to_current_scope(obj);

      ಪ್ರತ್ಯಾಗಮ obj;
}

ಶೂನ್ಯ __vpiArray::alias_word(unsigned long addr, vpiHandle word, ಪೂರ್ಣಾಙ್ಕ msb_, ಪೂರ್ಣಾಙ್ಕ lsb_)
{
      ಪ್ರತಿಜ್ಞಾ(msb.get_ಮೌಲ್ಯ() == msb_);
      ಪ್ರತಿಜ್ಞಾ(lsb.get_ಮೌಲ್ಯ() == lsb_);
      ಪ್ರತಿಜ್ಞಾ(addr < get_size());
      ಪ್ರತಿಜ್ಞಾ(nets);
      nets[addr] = word;
}

ಶೂನ್ಯ __vpiArray::attach_word(unsigned addr, vpiHandle word)
{
      ಪ್ರತಿಜ್ಞಾ(addr < get_size());
      ಪ್ರತಿಜ್ಞಾ(nets);
      nets[addr] = word;

      ಯದಿ (ಸಂರಚನಾ __vpiSignal*sig = dynamic_cast<__vpiSignal*>(word)) {
	    vvp_net_t*net = sig->ಗಂಟು;
	    ಪ್ರತಿಜ್ಞಾ(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    ಪ್ರತಿಜ್ಞಾ(fun);
	    fun->attach_as_word(ಏತತ್, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = ಏತತ್;
	    sig->id.index = ನೂತನ __vpiDecConst(addr + first_addr.get_ಮೌಲ್ಯ());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    ಪ್ರತ್ಯಾಗಮ;
      }

      ಯದಿ (ಸಂರಚನಾ __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(word)) {
	    vvp_net_t*net = sig->net;
	    ಪ್ರತಿಜ್ಞಾ(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    ಪ್ರತಿಜ್ಞಾ(fun);
	    fun->attach_as_word(ಏತತ್, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = ಏತತ್;
	    sig->id.index = ನೂತನ __vpiDecConst(addr + first_addr.get_ಮೌಲ್ಯ());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = ಸತ್ಯ;
	    ಪ್ರತ್ಯಾಗಮ;
      }
}

ಶೂನ್ಯ compile_var_array(ವರ್ಣ*label, ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ last, ಪೂರ್ಣಾಙ್ಕ first,
		   ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb, ವರ್ಣ signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last,
                                      signed_flag != 0);

      ಸಂರಚನಾ __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_width = labs(msb-lsb) + 1;
      ಯದಿ (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = ನೂತನ vvp_vector4array_aa(arr->vals_width,
						 arr->get_size());
      } ಅನ್ಯಥಾ {
            arr->vals4 = ನೂತನ vvp_vector4array_sa(arr->vals_width,
						 arr->get_size());
      }
      arr->msb.set_ಮೌಲ್ಯ(msb);
      arr->lsb.set_ಮೌಲ್ಯ(lsb);

      ಎಣಿಕೆ_var_arrays += 1;
      ಎಣಿಕೆ_var_array_ಪದಗಳು += arr->get_size();

      ಮುಕ್ತ(label);
      delete[] name;
}

ಶೂನ್ಯ compile_var2_array(ವರ್ಣ*label, ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ last, ಪೂರ್ಣಾಙ್ಕ first,
		   ಪೂರ್ಣಾಙ್ಕ msb, ಪೂರ್ಣಾಙ್ಕ lsb, ತಾರ್ಕಿಕ signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, signed_flag);

      ಸಂರಚನಾ __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.set_ಮೌಲ್ಯ(msb);
      arr->lsb.set_ಮೌಲ್ಯ(lsb);
      arr->vals_width = labs(msb-lsb) + 1;

      ಪ್ರತಿಜ್ಞಾ(! arr->nets);
      ಯದಿ (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = ನೂತನ vvp_darray_atom<int8_t>(arr->get_size());
      } ಅನ್ಯಥಾ ಯದಿ (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = ನೂತನ vvp_darray_atom<uint8_t>(arr->get_size());
      } ಅನ್ಯಥಾ ಯದಿ (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = ನೂತನ vvp_darray_atom<int16_t>(arr->get_size());
      } ಅನ್ಯಥಾ ಯದಿ (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = ನೂತನ vvp_darray_atom<uint16_t>(arr->get_size());
      } ಅನ್ಯಥಾ ಯದಿ (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = ನೂತನ vvp_darray_atom<int32_t>(arr->get_size());
      } ಅನ್ಯಥಾ ಯದಿ (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = ನೂತನ vvp_darray_atom<uint32_t>(arr->get_size());
      } ಅನ್ಯಥಾ ಯದಿ (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = ನೂತನ vvp_darray_atom<int64_t>(arr->get_size());
      } ಅನ್ಯಥಾ ಯದಿ (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = ನೂತನ vvp_darray_atom<uint64_t>(arr->get_size());
      } ಅನ್ಯಥಾ {
	    arr->vals = ನೂತನ vvp_darray_vec2(arr->get_size(), arr->vals_width);
      }
      ಎಣಿಕೆ_var_arrays += 1;
      ಎಣಿಕೆ_var_array_ಪದಗಳು += arr->get_size();

      ಮುಕ್ತ(label);
      delete[] name;
}

ಶೂನ್ಯ compile_real_array(ವರ್ಣ*label, ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ last, ಪೂರ್ಣಾಙ್ಕ first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, ಸತ್ಯ);

      ಸಂರಚನಾ __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = ನೂತನ vvp_darray_real(arr->get_size());
      arr->vals_width = 1;

      ಎಣಿಕೆ_real_arrays += 1;
      ಎಣಿಕೆ_real_array_ಪದಗಳು += arr->get_size();

      ಮುಕ್ತ(label);
      delete[] name;
}

ಶೂನ್ಯ compile_string_array(ವರ್ಣ*label, ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ last, ಪೂರ್ಣಾಙ್ಕ first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, ಸತ್ಯ);

      ಸಂರಚನಾ __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = ನೂತನ vvp_darray_string(arr->get_size());
      arr->vals_width = 1;

      ಎಣಿಕೆ_real_arrays += 1;
      ಎಣಿಕೆ_real_array_ಪದಗಳು += arr->get_size();

      ಮುಕ್ತ(label);
      delete[] name;
}

ಶೂನ್ಯ compile_object_array(ವರ್ಣ*label, ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ last, ಪೂರ್ಣಾಙ್ಕ first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, ಸತ್ಯ);

      ಸಂರಚನಾ __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = ನೂತನ vvp_darray_object(arr->get_size());
      arr->vals_width = 1;

      ಎಣಿಕೆ_real_arrays += 1;
      ಎಣಿಕೆ_real_array_ಪದಗಳು += arr->get_size();

      ಮುಕ್ತ(label);
      delete[] name;
}

ಶೂನ್ಯ compile_net_array(ವರ್ಣ*label, ವರ್ಣ*name, ಪೂರ್ಣಾಙ್ಕ last, ಪೂರ್ಣಾಙ್ಕ first)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, first, last, ಅಸತ್ಯ);

      ಸಂರಚನಾ __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(calloc(arr->get_size(), ಪರಿಮಾಣ(vpiHandle)));

      ಎಣಿಕೆ_net_arrays += 1;
      ಎಣಿಕೆ_net_array_ಪದಗಳು += arr->get_size();

      ಮುಕ್ತ(label);
      delete[] name;
}

ವರ್ಗ vvp_fun_arrayport  : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport() override;

      ಆಭಾಸ ಶೂನ್ಯ check_word_change(unsigned long addr) = 0;

    ರಕ್ಷಿತ:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      unsigned long addr_;

      friend ಶೂನ್ಯ array_attach_port(vvp_array_t, vvp_fun_arrayport*);
      friend ಶೂನ್ಯ __vpiArray::word_change(unsigned long);
      vvp_fun_arrayport*next_;
};

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net)
: arr_(mem), net_(net), addr_(0)
{
      next_ = 0;
}

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, long addr)
: arr_(mem), net_(net), addr_(addr)
{
      next_ = 0;
}

vvp_fun_arrayport::~vvp_fun_arrayport()
{
}

ವರ್ಗ vvp_fun_arrayport_sa  : ಸಾರ್ವಜನಿಕ vvp_fun_arrayport {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_sa() override;

      ಶೂನ್ಯ check_word_change(unsigned long addr) override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t) override;

    ಗುಪ್ತ:
};

vvp_fun_arrayport_sa::vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net)
{
}

vvp_fun_arrayport_sa::vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, long addr)
: vvp_fun_arrayport(mem, net, addr)
{
}

vvp_fun_arrayport_sa::~vvp_fun_arrayport_sa()
{
}

ಶೂನ್ಯ vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                                     vvp_context_t)
{
      ತಾರ್ಕಿಕ addr_valid_flag;

      ವಿಕಲ್ಪ (port.port()) {

	  ಪಕ್ಷ 0: // Address input
	    addr_valid_flag = vector4_to_ಮೌಲ್ಯ(bit, addr_);
	    ಯದಿ (! addr_valid_flag)
		  addr_ = arr_->get_size();
	    ಯದಿ (vpi_array_is_real(arr_))
		  port.ptr()->send_real(arr_->get_word_r(addr_), 0);
	    ಅನ್ಯಥಾ
		  port.ptr()->send_vec4(arr_->get_word(addr_), 0);

	    ಭಙ್ಗ;

	  ಸ್ವಭಾವ:
	    fprintf(stdout, "XXXX write ports not implemented.\n");
	    ಪ್ರತಿಜ್ಞಾ(0);
      }
}

ಶೂನ್ಯ vvp_fun_arrayport_sa::check_word_change(unsigned long addr)
{
      ಯದಿ (addr != addr_) ಪ್ರತ್ಯಾಗಮ;

      ಯದಿ (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr_), 0);
      } ಅನ್ಯಥಾ {
	    net_->send_vec4(arr_->get_word(addr_), 0);
      }
}

ವರ್ಗ vvp_fun_arrayport_aa  : ಸಾರ್ವಜನಿಕ vvp_fun_arrayport, ಸಾರ್ವಜನಿಕ automatic_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_aa() override;

      ಶೂನ್ಯ alloc_instance(vvp_context_t context) override;
      ಶೂನ್ಯ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ಶೂನ್ಯ free_instance(vvp_context_t context) override;
#endif

      ಶೂನ್ಯ check_word_change(unsigned long addr) override;

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                     vvp_context_t context) override;

    ಗುಪ್ತ:
      ಶೂನ್ಯ check_word_change_(unsigned long addr, vvp_context_t context);

      __vpiScope*context_scope_;
      unsigned context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(ಏತತ್, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(ಏತತ್, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

ಶೂನ್ಯ vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      unsigned long*addr = ನೂತನ unsigned long;
      vvp_set_context_item(context, context_idx_, addr);

      *addr = addr_;
}

ಶೂನ್ಯ vvp_fun_arrayport_aa::reset_instance(vvp_context_t context)
{
      unsigned long*addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));

      *addr = addr_;
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ vvp_fun_arrayport_aa::free_instance(vvp_context_t context)
{
      unsigned long*addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));
      delete addr;
}
#endif

ಶೂನ್ಯ vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
                                     vvp_context_t context)
{
      ಯದಿ (context) {
            unsigned long*addr = static_cast<unsigned long*>
                  (vvp_get_context_item(context, context_idx_));

            ತಾರ್ಕಿಕ addr_valid_flag;

            ವಿಕಲ್ಪ (port.port()) {

                ಪಕ್ಷ 0: // Address input
                  addr_valid_flag = vector4_to_ಮೌಲ್ಯ(bit, *addr);
                  ಯದಿ (! addr_valid_flag) *addr = arr_->get_size();
                  ಯದಿ (vpi_array_is_real(arr_)) {
			port.ptr()->send_real(arr_->get_word_r(*addr),
					      context);
                  } ಅನ್ಯಥಾ {
			port.ptr()->send_vec4(arr_->get_word(*addr),
					      context);
                  }
                  ಭಙ್ಗ;

                ಸ್ವಭಾವ:
                  fprintf(stdout, "XXXX write ports not implemented.\n");
                  ಪ್ರತಿಜ್ಞಾ(0);
            }
      } ಅನ್ಯಥಾ {
            context = context_scope_->live_contexts;
            ಯಾವತ್ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

ಶೂನ್ಯ vvp_fun_arrayport_aa::check_word_change_(unsigned long addr,
                                              vvp_context_t context)
{
      ಧ್ರುವ unsigned long*port_addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));

      ಯದಿ (addr != *port_addr)
	    ಪ್ರತ್ಯಾಗಮ;

      ಯದಿ (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr), context);
      } ಅನ್ಯಥಾ {
	    net_->send_vec4(arr_->get_word(addr), context);
      }
}

ಶೂನ್ಯ vvp_fun_arrayport_aa::check_word_change(unsigned long addr)
{
      ಯದಿ (arr_->get_scope()->is_automatic()) {
            ಪ್ರತಿಜ್ಞಾ(vthread_get_wt_context());
            check_word_change_(addr, vthread_get_wt_context());
      } ಅನ್ಯಥಾ {
            vvp_context_t context = context_scope_->live_contexts;
            ಯಾವತ್ (context) {
                  check_word_change_(addr, context);
                  context = vvp_get_next_context(context);
            }
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ array_attach_port(vvp_array_t array, vvp_fun_arrayport*fun)
{
      ಪ್ರತಿಜ್ಞಾ(fun->next_ == 0);
      fun->next_ = array->ports_;
      array->ports_ = fun;
      ಯದಿ (!array->get_scope()->is_automatic() &&
          (array->vals4 || array->vals)) {
              /* propagate initial values for variable arrays */
            ಯದಿ (!vpi_array_is_real(array)) {
                  vvp_bit4_t init;
                  ಯದಿ (array->vals4)
                      init = BIT4_X;
                  ಅನ್ಯಥಾ
                      init = BIT4_0;
                  vvp_vector4_t tmp(array->vals_width, init);
                  schedule_init_propagate(fun->net_, tmp);
            } ಅನ್ಯಥಾ {
                  schedule_init_propagate(fun->net_, 0.0);
            }
      }
}

ವರ್ಗ array_word_ಮೌಲ್ಯ_callback : ಸಾರ್ವಜನಿಕ ಮೌಲ್ಯ_callback {
    ಸಾರ್ವಜನಿಕ:
      ಅನ್ತಃಸ್ಥ explicit array_word_ಮೌಲ್ಯ_callback(p_cb_data data, long addr)
      : ಮೌಲ್ಯ_callback(data), word_addr(addr)
      { }

    ಸಾರ್ವಜನಿಕ:
      long word_addr;
};

ಶೂನ್ಯ __vpiArray::word_change(unsigned long addr)
{
      ಪ್ರತಿ (vvp_fun_arrayport*cur = ports_; cur; cur = cur->next_)
	    cur->check_word_change(addr);

	// Run callbacks attached to the array itself.
      ಸಂರಚನಾ __vpiCallback *next = vpi_callbacks;
      ಸಂರಚನಾ __vpiCallback *prev = 0;

      ಯಾವತ್ (next) {
	    array_word_ಮೌಲ್ಯ_callback*cur = dynamic_cast<array_word_ಮೌಲ್ಯ_callback*>(next);
	    next = cur->next;

	      // Skip callbacks that are not for me. -1 is for every element.
	    ಯದಿ (cur->word_addr != (long)addr && cur->word_addr != -1) {
		  prev = cur;
		  ಅನುವರ್ತ;
	    }

	      // For whole array callbacks we need to set the index.
	    ಯದಿ (cur->word_addr == -1) {
		  cur->cb_data.index = (PLI_INT32) ((ಪೂರ್ಣಾಙ್ಕ)addr +
						    first_addr.get_ಮೌಲ್ಯ());
	    }

	    ಯದಿ (cur->cb_data.cb_rtn != 0) {
		  ಯದಿ (cur->test_ಮೌಲ್ಯ_callback_ready()) {
			ಯದಿ (cur->cb_data.value) {
			      ಯದಿ (vpi_array_is_real(ಏತತ್)) {
				    ದ್ವಿಗುಣ val = 0.0;
				    ಯದಿ (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_real_get_ಮೌಲ್ಯ(val, cur->cb_data.value);
			      } ಅನ್ಯಥಾ ಯದಿ (vals4) {
				    vpip_vec4_get_ಮೌಲ್ಯ(vals4->get_word(addr),
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } ಅನ್ಯಥಾ ಯದಿ (dynamic_cast<vvp_darray_atom<int8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int64_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint64_t>*>(vals)
				      || dynamic_cast<vvp_darray_vec2*>(vals)) {
				    vvp_vector4_t val;
				    ಯದಿ (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_vec4_get_ಮೌಲ್ಯ(val,
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } ಅನ್ಯಥಾ {
			            ಪ್ರತಿಜ್ಞಾ(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } ಅನ್ಯಥಾ ಯದಿ (prev == 0) {

		  vpi_callbacks = next;
		  cur->next = 0;
		  delete cur;

	    } ಅನ್ಯಥಾ {
		  ಪ್ರತಿಜ್ಞಾ(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}

ವರ್ಗ array_resolv_list_t : ಸಾರ್ವಜನಿಕ resolv_list_s {

    ಸಾರ್ವಜನಿಕ:
      explicit array_resolv_list_t(ವರ್ಣ*lab) : resolv_list_s(lab) {
	    array = 0;
      }

      vvp_array_t*array;
      ತಾರ್ಕಿಕ resolve(ತಾರ್ಕಿಕ mes) override;

    ಗುಪ್ತ:
};

ತಾರ್ಕಿಕ array_resolv_list_t::resolve(ತಾರ್ಕಿಕ mes)
{
      *array = array_find(label());
      ಯದಿ (*array == 0) {
	    ಪ್ರತಿಜ್ಞಾ(!mes);
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      }
      ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ವರ್ಗ array_port_resolv_list_t : ಸಾರ್ವಜನಿಕ resolv_list_s {

    ಸಾರ್ವಜನಿಕ:
      explicit array_port_resolv_list_t(ವರ್ಣ* lab, ತಾರ್ಕಿಕ use_addr__,
                                        long addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      ತಾರ್ಕಿಕ use_addr;
      long addr;
      ತಾರ್ಕಿಕ resolve(ತಾರ್ಕಿಕ mes) override;

    ಗುಪ್ತ:
};

array_port_resolv_list_t::array_port_resolv_list_t(ವರ್ಣ *lab, ತಾರ್ಕಿಕ use_addr__,
                                                   long addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      ಯದಿ (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      ಅನ್ಯಥಾ
            context_scope = 0;
      ptr = ನೂತನ vvp_net_t;
}

ತಾರ್ಕಿಕ array_port_resolv_list_t::resolve(ತಾರ್ಕಿಕ mes)
{
      vvp_array_t mem = array_find(label());
      ಯದಿ (mem == 0) {
	    ಪ್ರತಿಜ್ಞಾ(mem || !mes);
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      }

      vvp_fun_arrayport*fun;
      ಯದಿ (use_addr)
            ಯದಿ (context_scope)
                  fun = ನೂತನ vvp_fun_arrayport_aa(context_scope, mem, ptr, addr);
            ಅನ್ಯಥಾ
                  fun = ನೂತನ vvp_fun_arrayport_sa(mem, ptr, addr);
      ಅನ್ಯಥಾ
            ಯದಿ (context_scope)
                  fun = ನೂತನ vvp_fun_arrayport_aa(context_scope, mem, ptr);
            ಅನ್ಯಥಾ
                  fun = ನೂತನ vvp_fun_arrayport_sa(mem, ptr);
      ptr->fun = fun;

      array_attach_port(mem, fun);

      ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ವರ್ಗ array_word_part_callback : ಸಾರ್ವಜನಿಕ array_word_ಮೌಲ್ಯ_callback {
    ಸಾರ್ವಜನಿಕ:
      explicit array_word_part_callback(p_cb_data data, long addr);
      ~array_word_part_callback() override;

      array_word_part_callback(ಧ್ರುವ array_word_part_callback&) = delete;
      array_word_part_callback& operator=(ಧ್ರುವ array_word_part_callback&) = delete;

      ತಾರ್ಕಿಕ test_ಮೌಲ್ಯ_callback_ready(ಶೂನ್ಯ) override;

    ಗುಪ್ತ:
      ವರ್ಣ*ಮೌಲ್ಯ_bits;
};

array_word_part_callback::array_word_part_callback(p_cb_data data, long addr)
: array_word_ಮೌಲ್ಯ_callback(data, addr)
{
	// Get the initial value of the part, to use as a reference.
      ಸಂರಚನಾ __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj);
      s_vpi_ಮೌಲ್ಯ tmp_ಮೌಲ್ಯ;
      tmp_ಮೌಲ್ಯ.format = vpiBinStrVal;
      apvword->vpi_get_ಮೌಲ್ಯ(&tmp_ಮೌಲ್ಯ);

      ಮೌಲ್ಯ_bits = ನೂತನ ವರ್ಣ[apvword->part_wid+1];

      memcpy(ಮೌಲ್ಯ_bits, tmp_ಮೌಲ್ಯ.value.str, apvword->part_wid);
      ಮೌಲ್ಯ_bits[apvword->part_wid] = 0;
}

array_word_part_callback::~array_word_part_callback()
{
      delete[]ಮೌಲ್ಯ_bits;
}

ತಾರ್ಕಿಕ array_word_part_callback::test_ಮೌಲ್ಯ_callback_ready(ಶೂನ್ಯ)
{
      ಸಂರಚನಾ __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_data.obj);
      ಪ್ರತಿಜ್ಞಾ(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_ಮೌಲ್ಯ tmp_ಮೌಲ್ಯ;
      tmp_ಮೌಲ್ಯ.format = vpiBinStrVal;
      apvword->vpi_get_ಮೌಲ್ಯ(&tmp_ಮೌಲ್ಯ);

      ಯದಿ (memcmp(ಮೌಲ್ಯ_bits, tmp_ಮೌಲ್ಯ.value.str, apvword->part_wid) == 0)
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

      memcpy(ಮೌಲ್ಯ_bits, tmp_ಮೌಲ್ಯ.value.str, apvword->part_wid);
      ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;

}

ಮೌಲ್ಯ_callback*vpip_array_word_change(p_cb_data data)
{
      ಸಂರಚನಾ __vpiArray*parent = 0;
      array_word_ಮೌಲ್ಯ_callback*cbh = 0;
      ಯದಿ (ಧ್ರುವ ಸಂರಚನಾ __vpiArrayWord*word = array_var_word_from_handle(data->obj)) {
	    parent = static_cast<__vpiArray*>(word->get_parent());
	    unsigned addr = word->get_index();
	    cbh = ನೂತನ array_word_ಮೌಲ್ಯ_callback(data, addr);

      } ಅನ್ಯಥಾ ಯದಿ (ಸಂರಚನಾ __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(data->obj)) {
	    parent = tword->array;
	    cbh = ನೂತನ array_word_ಮೌಲ್ಯ_callback(data, tword->address);

      } ಅನ್ಯಥಾ ಯದಿ (ಸಂರಚನಾ __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj)) {
	    parent = apvword->array;
	    cbh = ನೂತನ array_word_part_callback(data, apvword->word_sel);
      }

      ಪ್ರತಿಜ್ಞಾ(cbh);
      ಪ್ರತಿಜ್ಞಾ(parent);
      cbh->next = parent->vpi_callbacks;
      parent->vpi_callbacks = cbh;

      ಪ್ರತ್ಯಾಗಮ cbh;
}

ಮೌಲ್ಯ_callback* vpip_array_change(p_cb_data data)
{
      array_word_ಮೌಲ್ಯ_callback*cbh = ನೂತನ array_word_ಮೌಲ್ಯ_callback(data, -1);
      ಪ್ರತಿಜ್ಞಾ(data->obj);

      ಸಂರಚನಾ __vpiArray*arr = dynamic_cast<__vpiArray*>(data->obj);
      cbh->next = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      ಪ್ರತ್ಯಾಗಮ cbh;
}

ಶೂನ್ಯ compile_array_port(ವರ್ಣ*label, ವರ್ಣ*array, ವರ್ಣ*addr)
{
      array_port_resolv_list_t*resolv_mem
	    = ನೂತನ array_port_resolv_list_t(array, ಅಸತ್ಯ, 0);

      define_functor_ಚಿಹ್ನೆ(label, resolv_mem->ptr);
      ಮುಕ್ತ(label);
	// Connect the port-0 input as the address.
      ಒಳಹರಿವು_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

ಶೂನ್ಯ compile_array_port(ವರ್ಣ*label, ವರ್ಣ*array, long addr)
{
      array_port_resolv_list_t*resolv_mem
	    = ನೂತನ array_port_resolv_list_t(array, ಸತ್ಯ, addr);

      define_functor_ಚಿಹ್ನೆ(label, resolv_mem->ptr);
      ಮುಕ್ತ(label);

      resolv_submit(resolv_mem);
}

ಶೂನ್ಯ compile_array_alias(ವರ್ಣ*label, ವರ್ಣ*name, ವರ್ಣ*src)
{
      vvp_array_t mem = array_find(src);
      ಪ್ರತಿಜ್ಞಾ(mem);

      ಸಂರಚನಾ __vpiArray*obj = ನೂತನ __vpiArray;

      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_ಎಣಿಕೆ = mem->array_ಎಣಿಕೆ;
      obj->signed_flag = mem->signed_flag;

	// Need to set an accurate range of addresses.
      obj->first_addr = mem->first_addr;
      obj->last_addr  = mem->last_addr;
      obj->swap_addr = mem->swap_addr;

      obj->msb = mem->msb;
      obj->lsb = mem->lsb;

	// Share the words with the source array.
      obj->nets = mem->nets;
      obj->vals4 = mem->vals4;
      obj->vals  = mem->vals;
      obj->vals_width = mem->vals_width;
      obj->vals_ಪದಗಳು = mem->vals_ಪದಗಳು;

      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

      ಪ್ರತಿಜ್ಞಾ(array_table);
      ಪ್ರತಿಜ್ಞಾ(!array_find(label));
      array_table->sym_set_ಮೌಲ್ಯ(label, obj);

      compile_vpi_ಚಿಹ್ನೆ(label, obj);
      vpip_attach_to_current_scope(obj);

      ಮುಕ್ತ(label);
      ಮುಕ್ತ(name);
      ಮುಕ್ತ(src);
}

/*
 * &A<label,addr>
 * This represents a VPI handle for an addressed array. This comes
 * from expressions like "label[addr]" where "label" is the array and
 * "addr" is the canonical address of the desired word.
 */
vpiHandle vpip_make_vthr_A(ವರ್ಣ*label, unsigned addr)
{
      ಸಂರಚನಾ __vpiArrayVthrA*obj = ನೂತನ __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = ನೂತನ array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      obj->address = addr;

      ಪ್ರತ್ಯಾಗಮ obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(ವರ್ಣ*label, ವರ್ಣ*ಚಿಹ್ನೆ)
{
      ಸಂರಚನಾ __vpiArrayVthrA*obj = ನೂತನ __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = ನೂತನ array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      compile_vpi_lookup(&obj->address_handle, ಚಿಹ್ನೆ);
      obj->address = 0;

      ಪ್ರತ್ಯಾಗಮ obj;
}

vpiHandle vpip_make_vthr_A(ವರ್ಣ*label, vpiHandle handle)
{
      ಸಂರಚನಾ __vpiArrayVthrA*obj = ನೂತನ __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = ನೂತನ array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = handle;
      obj->address = 0;

      ಪ್ರತ್ಯಾಗಮ obj;
}

vpiHandle vpip_make_vthr_APV(ವರ್ಣ*label, unsigned index, unsigned bit, unsigned wid)
{
      ಸಂರಚನಾ __vpiArrayVthrAPV*obj = ನೂತನ __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = ನೂತನ array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->word_sel = index;
      obj->part_bit = bit;
      obj->part_wid = wid;

      ಪ್ರತ್ಯಾಗಮ obj;
}

ಶೂನ್ಯ compile_array_cleanup(ಶೂನ್ಯ)
{
      delete array_table;
      array_table = 0;
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ memory_delete(vpiHandle item)
{
      ಸಂರಚನಾ __vpiArray*arr = static_cast<ಸಂರಚನಾ __vpiArray*>(item);
      ಯದಿ (arr->vals_ಪದಗಳು) delete [] (arr->vals_ಪದಗಳು-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals;

      ಯದಿ (arr->nets) {
	    ಪ್ರತಿ (unsigned idx = 0; idx < arr->get_size(); idx += 1) {
		  ಯದಿ (ಸಂರಚನಾ __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.index);
		    /* These should only be the real words. */
		  } ಅನ್ಯಥಾ {
			ಪ್ರತಿಜ್ಞಾ(arr->nets[idx]->get_type_code() ==
			       vpiRealVar);
			ಸಂರಚನಾ __vpiRealVar *sigr = static_cast<ಸಂರಚನಾ __vpiRealVar *>
			                            (arr->nets[idx]);
			constant_delete(sigr->id.index);
// Why are only the real words still here?
			delete arr->nets[idx];
		  }
	    }
	    ಮುಕ್ತ(arr->nets);
      }

      ಯಾವತ್ (arr->vpi_callbacks) {
	    ಸಂರಚನಾ __vpiCallback*tmp = arr->vpi_callbacks->next;
	    delete arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      delete arr;
}

ಶೂನ್ಯ A_delete(vpiHandle item)
{
      ಸಂರಚನಾ __vpiArrayVthrA*obj = static_cast<ಸಂರಚನಾ __vpiArrayVthrA*>(item);
      ಯದಿ (obj->address_handle) {
	    ವಿಕಲ್ಪ (obj->address_handle->get_type_code()) {
		ಪಕ್ಷ vpiMemoryWord:
		  ಯದಿ (vpi_get(_vpiFromThr, obj->address_handle) == _vpi_at_A) {
			A_delete(obj->address_handle);
		  }
		  ಭಙ್ಗ;
		ಪಕ್ಷ vpiPartSelect:
		  ಪ್ರತಿಜ್ಞಾ(vpi_get(_vpiFromThr, obj->address_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->address_handle);
		  ಭಙ್ಗ;
	    }
      }

      delete obj;
}

ಶೂನ್ಯ APV_delete(vpiHandle item)
{
      ಸಂರಚನಾ __vpiArrayVthrAPV*obj = static_cast<ಸಂರಚನಾ __vpiArrayVthrAPV*>(item);
      delete obj;
}
#endif
