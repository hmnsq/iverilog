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
#അന്തര്ഭാവ  "vvp_cleanup.h"
#endif
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <iostream>
# include  "compile.h"
# include  <cassert>
# include  "ivl_alloc.h"

using നാമാകാശ std;

unsigned long എണ്ണം_net_arrays = 0;
unsigned long എണ്ണം_net_array_വാക്കുകൾ = 0;
unsigned long എണ്ണം_var_arrays = 0;
unsigned long എണ്ണം_var_array_വാക്കുകൾ = 0;
unsigned long എണ്ണം_real_arrays = 0;
unsigned long എണ്ണം_real_array_വാക്കുകൾ = 0;

സ്ഥിതം ചിഹ്നം_map_s<ഘടന __vpiArray>* array_table =0;

വർഗ്ഗം vvp_fun_arrayport;
സ്ഥിതം ശൂന്യം array_attach_port(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_find(സ്ഥിരം അക്ഷരം*label)
{
      എങ്കിൽ (array_table == 0)
	    തിരികെ 0;

      vvp_array_t v = array_table->sym_get_മൂല്യം(label);
      തിരികെ v;
}

ഘടന __vpiArrayVthrA : പൊതു __vpiHandle {
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiMemoryWord; }
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      അക്ഷരം* vpi_get_str(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;
      vpiHandle vpi_put_മൂല്യം(p_vpi_മൂല്യം val, പൂർണ്ണസംഖ്യ flags) override;
      vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code) override;

      ഘടന __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle address_handle;
	// If wid==0, then address is the address into the array.
      unsigned address;

      unsigned get_address() സ്ഥിരം
      {
	    എങ്കിൽ (address_handle) {
		  s_vpi_മൂല്യം vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpiVectorVal;
		  address_handle->vpi_get_മൂല്യം(&vp);
		  പൂർണ്ണസംഖ്യ വാക്കുകൾ = (address_handle->vpi_get(vpiSize)-1)/32 + 1;
		  വേണ്ടി(പൂർണ്ണസംഖ്യ idx = 0; idx < വാക്കുകൾ; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			എങ്കിൽ (vp.value.vector[idx].bval != 0) തിരികെ UINT_MAX;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  address_handle->vpi_get_മൂല്യം(&vp);
		  തിരികെ vp.value.integer;
	    }

	    തിരികെ address;
      }
};


ഘടന __vpiArrayVthrAPV : പൊതു __vpiHandle {
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiPartSelect; }
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      അക്ഷരം* vpi_get_str(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;
      vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code) override;

      ഘടന __vpiArray*array;
      unsigned word_sel;
      unsigned part_bit;
      unsigned part_wid;
};

തർക്കം is_net_array(vpiHandle obj)
{
      ഘടന __vpiArray*rfp = dynamic_cast<__vpiArray*> (obj);
      പ്രതിജ്ഞാ(rfp);

      എങ്കിൽ (rfp->nets != 0) തിരികെ സത്യം;
      തിരികെ അസത്യം;
}

// This function return true if the underlying array words are real.
സ്ഥിതം തർക്കം vpi_array_is_real(സ്ഥിരം vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      എങ്കിൽ (arr->vals4 != 0)  // A bit based variable/register array.
	    തിരികെ അസത്യം;

      എങ്കിൽ (dynamic_cast<vvp_darray_real*> (arr->vals))
	    തിരികെ സത്യം;

      എങ്കിൽ (arr->vals != 0)
	    തിരികെ അസത്യം;

	// This must be a net array so look at element 0 to find the type.
      പ്രതിജ്ഞാ(arr->nets != 0);
      പ്രതിജ്ഞാ(arr->get_size() > 0);
      സ്ഥിരം ഘടന __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      എങ്കിൽ (rsig) {
	    തിരികെ സത്യം;
      }

      തിരികെ അസത്യം;
}

സ്ഥിതം തർക്കം vpi_array_is_string(സ്ഥിരം vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      എങ്കിൽ (arr->vals4 != 0)  // A bit based variable/register array.
	    തിരികെ അസത്യം;

      എങ്കിൽ (dynamic_cast<vvp_darray_string*> (arr->vals))
	    തിരികെ സത്യം;

      തിരികെ അസത്യം;
}

പൂർണ്ണസംഖ്യ __vpiArray::get_word_size() സ്ഥിരം
{
      unsigned width;

      പ്രതിജ്ഞാ(get_size() > 0);
	/* For a net array we need to get the width from the first element. */
      എങ്കിൽ (nets) {
	    പ്രതിജ്ഞാ(vals4 == 0 && vals == 0);
	    സ്ഥിരം ഘടന __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    പ്രതിജ്ഞാ(vsig);
	    width = vpip_size(vsig);
	/* For a variable array we can get the width from vals_width. */
      } അല്ലെങ്കിൽ {
	    പ്രതിജ്ഞാ(vals4 || vals);
	    width = vals_width;
      }

      തിരികെ width;
}

അക്ഷരം*__vpiArray::get_word_str(ഘടന __vpiArrayWord*word, പൂർണ്ണസംഖ്യ code)
{
      unsigned index = word->get_index();

      എങ്കിൽ (code == vpiFile) {  // Not implemented for now!
	    തിരികെ simple_set_rbuf_str(file_names[0]);
      }

      അക്ഷരം sidx [64];
      snprintf(sidx, 63, "%d", (പൂർണ്ണസംഖ്യ)index + first_addr.get_മൂല്യം());
      തിരികെ generic_get_str(code, get_scope(), name, sidx);
}

ശൂന്യം __vpiArray::get_word_മൂല്യം(ഘടന __vpiArrayWord*word, p_vpi_മൂല്യം vp)
{
      unsigned index = word->get_index();

    // Determine the appropriate format (The Verilog PLI Handbook 5.2.10)
      എങ്കിൽ(vp->format == vpiObjTypeVal) {
          എങ്കിൽ(vpi_array_is_real(ഇത്))
              vp->format = vpiRealVal;
          അല്ലെങ്കിൽ എങ്കിൽ(vpi_array_is_string(ഇത്))
              vp->format = vpiStringVal;
          അല്ലെങ്കിൽ
              vp->format = vpiIntVal;
      }

      എങ്കിൽ(vals4) {
          vpip_vec4_get_മൂല്യം(vals4->get_word(index),
                              vals4->width(), signed_flag, vp);
      } അല്ലെങ്കിൽ എങ്കിൽ(vals) {
          തിരഞ്ഞെടുപ്പ്(vp->format) {
            അവസ്ഥ vpiBinStrVal:
            അവസ്ഥ vpiOctStrVal:
            അവസ്ഥ vpiDecStrVal:
            അവസ്ഥ vpiHexStrVal:
            അവസ്ഥ vpiScalarVal:
            അവസ്ഥ vpiIntVal:
            അവസ്ഥ vpiVectorVal:
            {
                vvp_vector4_t v;
                vals->get_word(index, v);
                vpip_vec4_get_മൂല്യം(v, vals_width, signed_flag, vp);
            }
            നിർത്തുക;

            അവസ്ഥ vpiRealVal:
            {
                ഇരട്ട d;
                vals->get_word(index, d);
                vpip_real_get_മൂല്യം(d, vp);
            }
            നിർത്തുക;

            അവസ്ഥ vpiStringVal:
            {
                string s;
                vals->get_word(index, s);
                vpip_string_get_മൂല്യം(s, vp);
            }
            നിർത്തുക;

            സ്ഥിരസ്ഥിതി:
                fprintf(stderr, "vpi sorry: format is not implemented\n");
                പ്രതിജ്ഞാ(അസത്യം);
          }
      }
}

ശൂന്യം __vpiArray::put_word_മൂല്യം(ഘടന __vpiArrayWord*word, p_vpi_മൂല്യം vp, പൂർണ്ണസംഖ്യ)
{
      unsigned index = word->get_index();
      vvp_vector4_t val = vec4_from_vpi_മൂല്യം(vp, vals_width);
      set_word(index, 0, val);
}

vpiHandle __vpiArray::get_iter_index(ഘടന __vpiArrayIterator*, പൂർണ്ണസംഖ്യ idx)
{
      എങ്കിൽ (nets) തിരികെ nets[idx];

      പ്രതിജ്ഞാ(vals4 || vals);

      എങ്കിൽ (vals_വാക്കുകൾ == 0) make_vals_വാക്കുകൾ();

      തിരികെ &(vals_വാക്കുകൾ[idx].as_word);
}

പൂർണ്ണസംഖ്യ __vpiArray::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpi_വരി_No:
	    തിരികെ 0; // Not implemented for now!

	  അവസ്ഥ vpiSize:
	    തിരികെ get_size();

	  അവസ്ഥ vpiAutomatic:
	    തിരികെ scope->is_automatic()? 1 : 0;

	  സ്ഥിരസ്ഥിതി:
	    തിരികെ 0;
      }
}

അക്ഷരം* __vpiArray::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{
      എങ്കിൽ (code == vpiFile) {  // Not implemented for now!
            തിരികെ simple_set_rbuf_str(file_names[0]);
      }

      തിരികെ generic_get_str(code, scope, name, ശൂന്യ);
}

vpiHandle __vpiArray::vpi_handle(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
          അവസ്ഥ vpi_ഇടത്_Range:
            എങ്കിൽ (swap_addr) തിരികെ &last_addr;
            അല്ലെങ്കിൽ തിരികെ &first_addr;

          അവസ്ഥ vpi_വലത്_Range:
            എങ്കിൽ (swap_addr) തിരികെ &first_addr;
            അല്ലെങ്കിൽ തിരികെ &last_addr;

	  അവസ്ഥ vpiScope:
	    തിരികെ scope;

	  അവസ്ഥ vpiModule:
	    തിരികെ vpip_module(scope);
      }

      തിരികെ 0;
}

/*
* VPI code passes indices that are not yet converted to canonical
* form, so this index function does it here.
*/
vpiHandle __vpiArray::vpi_index(പൂർണ്ണസംഖ്യ index)
{
      index -= first_addr.get_മൂല്യം();
      എങ്കിൽ (index >= (long) get_size())
	    തിരികെ 0;
      എങ്കിൽ (index < 0)
	    തിരികെ 0;

      എങ്കിൽ (nets != 0) {
	    തിരികെ nets[index];
      }

      എങ്കിൽ (vals_വാക്കുകൾ == 0)
	    make_vals_വാക്കുകൾ();

      തിരികെ &(vals_വാക്കുകൾ[index].as_word);
}

പൂർണ്ണസംഖ്യ __vpiArrayWord::as_word_t::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      ഘടന __vpiArrayWord*obj = array_var_word_from_handle(ഇത്);
      പ്രതിജ്ഞാ(obj);
      ഘടന __vpiArrayBase*my_parent = obj->get_parent();
      t_vpi_മൂല്യം val;

      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpi_വരി_No:
	    തിരികെ 0; // Not implemented for now!

	  അവസ്ഥ vpiSize:
	    തിരികെ my_parent->get_word_size();

	  അവസ്ഥ vpi_ഇടത്_Range:
            val.format = vpiIntVal;
	    my_parent->get_ഇടത്_range()->vpi_get_മൂല്യം(&val);
            പ്രതിജ്ഞാ(val.format == vpiIntVal);
            തിരികെ val.value.integer;

	  അവസ്ഥ vpi_വലത്_Range:
            val.format = vpiIntVal;
	    my_parent->get_വലത്_range()->vpi_get_മൂല്യം(&val);
            പ്രതിജ്ഞാ(val.format == vpiIntVal);
            തിരികെ val.value.integer;

	  അവസ്ഥ vpiIndex:
	    {
		  പൂർണ്ണസംഖ്യ base_offset = 0;
		  ഘടന __vpiArray*base = dynamic_cast<__vpiArray*> (my_parent);
		  എങ്കിൽ (base) {
			val.format = vpiIntVal;
			base->first_addr.vpi_get_മൂല്യം(&val);
			base_offset += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_index.vpi_get_മൂല്യം(&val);
		  പ്രതിജ്ഞാ(val.format == vpiIntVal);
		  തിരികെ val.value.integer + base_offset;
	    }

	  അവസ്ഥ vpiAutomatic:
	    തിരികെ my_parent->get_scope()->is_automatic()? 1 : 0;

#എങ്കിൽ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  അവസ്ഥ _vpiFromThr:
	    തിരികെ _vpiNoThr;
#endif

	  സ്ഥിരസ്ഥിതി:
	    തിരികെ 0;
      }
}


പൂർണ്ണസംഖ്യ __vpiArrayVthrA::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpi_വരി_No:
	    തിരികെ 0; // Not implemented for now!

	  അവസ്ഥ vpiSize:
	    തിരികെ array->get_word_size();

	  അവസ്ഥ vpi_ഇടത്_Range:
	    തിരികെ array->msb.get_മൂല്യം();

	  അവസ്ഥ vpi_വലത്_Range:
	    തിരികെ array->lsb.get_മൂല്യം();

	  അവസ്ഥ vpiIndex:
	    തിരികെ (പൂർണ്ണസംഖ്യ)get_address() + array->first_addr.get_മൂല്യം();

	  അവസ്ഥ vpiAutomatic:
	    തിരികെ array->get_scope()->is_automatic() ? 1 : 0;

#എങ്കിൽ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  അവസ്ഥ _vpiFromThr:
	    തിരികെ _vpi_at_A;
#endif

	  // If address_handle is not zero we definitely have a
	  // variable.
	  അവസ്ഥ vpiConstantSelect:
	    തിരികെ address_handle == 0;

	  സ്ഥിരസ്ഥിതി:
	    തിരികെ 0;
      }
}

അക്ഷരം* __vpiArrayVthrA::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{
      എങ്കിൽ (code == vpiFile) {  // Not implemented for now!
            തിരികെ simple_set_rbuf_str(file_names[0]);
      }

      അക്ഷരം sidx [64];
      snprintf(sidx, 63, "%d", (പൂർണ്ണസംഖ്യ)get_address() + array->first_addr.get_മൂല്യം());
      തിരികെ generic_get_str(code, array->get_scope(), array->name, sidx);
}

ശൂന്യം __vpiArrayVthrA::vpi_get_മൂല്യം(p_vpi_മൂല്യം vp)
{
      പ്രതിജ്ഞാ(array);

      unsigned index = get_address();
      എങ്കിൽ (vpi_array_is_real(array)) {
	    ഇരട്ട tmp = array->get_word_r(index);
	    vpip_real_get_മൂല്യം(tmp, vp);
      } അല്ലെങ്കിൽ എങ്കിൽ (vpi_array_is_string(array)) {
	    string tmp = array->get_word_str(index);
	    vpip_string_get_മൂല്യം(tmp, vp);
      } അല്ലെങ്കിൽ {
	    vvp_vector4_t tmp = array->get_word(index);
	    unsigned width = array->get_word_size();
	    vpip_vec4_get_മൂല്യം(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_മൂല്യം(p_vpi_മൂല്യം vp, പൂർണ്ണസംഖ്യ)
{
      unsigned index = get_address();

      പ്രതിജ്ഞാ(array);
      പ്രതിജ്ഞാ(index < array->get_size());

      എങ്കിൽ (vpi_array_is_real(array)) {
	    ഇരട്ട val = real_from_vpi_മൂല്യം(vp);
	    array->set_word(index, val);
      } അല്ലെങ്കിൽ {
	    unsigned width = array->get_word_size();
	    vvp_vector4_t val = vec4_from_vpi_മൂല്യം(vp, width);
	    array->set_word(index, 0, val);
      }

      തിരികെ ഇത്;
}

vpiHandle __vpiArrayVthrA::vpi_handle(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiIndex:
	    നിർത്തുക;  // Not implemented!

	  അവസ്ഥ vpi_ഇടത്_Range:
	    തിരികെ &array->msb;

	  അവസ്ഥ vpi_വലത്_Range:
	    തിരികെ &array->lsb;

	  അവസ്ഥ vpiParent:
	  അവസ്ഥ vpiArray:
	    തിരികെ array;

	  അവസ്ഥ vpiScope:
	    തിരികെ array->get_scope();

	  അവസ്ഥ vpiModule:
	    തിരികെ vpip_module(array->get_scope());
      }

      തിരികെ 0;
}


പൂർണ്ണസംഖ്യ __vpiArrayVthrAPV::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpi_വരി_No:
	    തിരികെ 0; // Not implemented for now!

	  അവസ്ഥ vpiSize:
	    തിരികെ part_wid;

	  അവസ്ഥ vpi_ഇടത്_Range:
	    തിരികെ part_bit + part_wid - 1;

	  അവസ്ഥ vpi_വലത്_Range:
	    തിരികെ part_bit;

	  അവസ്ഥ vpiAutomatic:
	    തിരികെ array->get_scope()->is_automatic() ? 1 : 0;

#എങ്കിൽ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  അവസ്ഥ _vpiFromThr:
	    തിരികെ _vpi_at_APV;
#endif

	  അവസ്ഥ vpiConstantSelect:
	    തിരികെ 1;

	  സ്ഥിരസ്ഥിതി:
	    തിരികെ 0;
      }
}

അക്ഷരം* __vpiArrayVthrAPV::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{
      എങ്കിൽ (code == vpiFile) {  // Not implemented for now!
            തിരികെ simple_set_rbuf_str(file_names[0]);
      }

      അക്ഷരം sidx [64];
      snprintf(sidx, 63, "%u", word_sel + array->first_addr.get_മൂല്യം());
      തിരികെ generic_get_str(code, array->get_scope(), array->name, sidx);
}

ശൂന്യം __vpiArrayVthrAPV::vpi_get_മൂല്യം(p_vpi_മൂല്യം vp)
{
      പ്രതിജ്ഞാ(array);

      unsigned index = word_sel;
      എങ്കിൽ (vpi_array_is_real(array)) {
	    ഇരട്ട tmp = array->get_word_r(index);
	    vpip_real_get_മൂല്യം(tmp, vp);
      } അല്ലെങ്കിൽ {
	    vvp_vector4_t tmp = array->get_word(index);
	    tmp = tmp.subvalue(part_bit, part_wid);
	    vpip_vec4_get_മൂല്യം(tmp, part_wid, അസത്യം, vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  അവസ്ഥ vpiParent:
	    തിരികെ 0;

            // Not part of the Verilog standard. We use this internally.
	  അവസ്ഥ vpiArray:
	    തിരികെ array;

	  അവസ്ഥ vpiScope:
	    തിരികെ array->get_scope();

	  അവസ്ഥ vpiModule:
	    തിരികെ vpip_module(array->get_scope());
      }

      തിരികെ 0;
}

ശൂന്യം __vpiArray::set_word(unsigned address, unsigned part_off, സ്ഥിരം vvp_vector4_t&val)
{
      എങ്കിൽ (address >= get_size())
	    തിരികെ;

      എങ്കിൽ (vals4) {
	    പ്രതിജ്ഞാ(nets == 0);
	    എങ്കിൽ (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp = vals4->get_word(address);
		  എങ്കിൽ ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			പ്രതിജ്ഞാ(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals4->set_word(address, tmp);
	    } അല്ലെങ്കിൽ {
		  vals4->set_word(address, val);
	    }
	    word_change(address);
	    തിരികെ;
      }

      എങ്കിൽ (vals) {
	    പ്രതിജ്ഞാ(nets == 0);
	    എങ്കിൽ (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp;
		  vals->get_word(address, tmp);
		  എങ്കിൽ ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			പ്രതിജ്ഞാ(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals->set_word(address, tmp);
	    } അല്ലെങ്കിൽ {
		  vals->set_word(address, val);
	    }
	    word_change(address);
	    തിരികെ;
      }

      പ്രതിജ്ഞാ(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle word = nets[address];
      സ്ഥിരം ഘടന __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      പ്രതിജ്ഞാ(vsig);

      vsig->കണ്ണി->send_vec4_pv(val, part_off, vpip_size(vsig), 0);
      word_change(address);
}

ശൂന്യം __vpiArray::set_word(unsigned address, ഇരട്ട val)
{
      പ്രതിജ്ഞാ(vals != 0);
      പ്രതിജ്ഞാ(nets == 0);

      എങ്കിൽ (address >= vals->get_size())
	    തിരികെ;

      vals->set_word(address, val);
      word_change(address);
}

ശൂന്യം __vpiArray::set_word(unsigned address, സ്ഥിരം string&val)
{
      പ്രതിജ്ഞാ(vals != 0);
      പ്രതിജ്ഞാ(nets == 0);

      എങ്കിൽ (address >= vals->get_size())
	    തിരികെ;

      vals->set_word(address, val);
      word_change(address);
}

ശൂന്യം __vpiArray::set_word(unsigned address, സ്ഥിരം vvp_object_t&val)
{
      പ്രതിജ്ഞാ(vals != 0);
      പ്രതിജ്ഞാ(nets == 0);

      എങ്കിൽ (address >= vals->get_size())
	    തിരികെ;

      vals->set_word(address, val);
      word_change(address);
}

vvp_vector4_t __vpiArray::get_word(unsigned address)
{
      എങ്കിൽ (vals4) {
	    പ്രതിജ്ഞാ(nets == 0);
	    പ്രതിജ്ഞാ(vals == 0);
	    തിരികെ vals4->get_word(address);
      }

      എങ്കിൽ (vals) {
	    പ്രതിജ്ഞാ(nets == 0);
	    പ്രതിജ്ഞാ(vals4== 0);
	    എങ്കിൽ (address >= vals->get_size())
		  തിരികെ vvp_vector4_t(vals_width, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_word(address, val);
	    തിരികെ val;
      }

      പ്രതിജ്ഞാ(vals4 == 0);
      പ്രതിജ്ഞാ(vals == 0);
      പ്രതിജ്ഞാ(nets != 0);

      എങ്കിൽ (address >= get_size()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    പ്രതിജ്ഞാ(get_size() > 0);
	    vpiHandle word = nets[0];
	    പ്രതിജ്ഞാ(word);
	    ഘടന __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
	    പ്രതിജ്ഞാ(vsig);
	    vvp_signal_മൂല്യം*sig = dynamic_cast<vvp_signal_മൂല്യം*> (vsig->കണ്ണി->fil);
	    പ്രതിജ്ഞാ(sig);
	    തിരികെ vvp_vector4_t(sig->മൂല്യം_size(), BIT4_X);
      }

      vpiHandle word = nets[address];
      ഘടന __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      പ്രതിജ്ഞാ(vsig);
      vvp_signal_മൂല്യം*sig = dynamic_cast<vvp_signal_മൂല്യം*> (vsig->കണ്ണി->fil);
      പ്രതിജ്ഞാ(sig);

      vvp_vector4_t val;
      sig->vec4_മൂല്യം(val);
      തിരികെ val;
}

ഇരട്ട __vpiArray::get_word_r(unsigned address)
{
      എങ്കിൽ (vals) {
	    പ്രതിജ്ഞാ(vals4 == 0);
	    പ്രതിജ്ഞാ(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    എങ്കിൽ (address >= vals->get_size())
		  തിരികെ 0.0;

	    ഇരട്ട val;
	    vals->get_word(address, val);
	    തിരികെ val;
      }

      എങ്കിൽ (address >= get_size())
	    തിരികെ 0.0;

      പ്രതിജ്ഞാ(nets);
      vpiHandle word = nets[address];
      ഘടന __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(word);
      പ്രതിജ്ഞാ(vsig);
      vvp_signal_മൂല്യം*sig = dynamic_cast<vvp_signal_മൂല്യം*> (vsig->net->fil);
      പ്രതിജ്ഞാ(sig);

      ഇരട്ട val = sig->real_മൂല്യം();
      തിരികെ val;
}

ശൂന്യം __vpiArray::get_word_obj(unsigned address, vvp_object_t&val)
{
      എങ്കിൽ (vals) {
	    പ്രതിജ്ഞാ(vals4 == 0);
	    പ്രതിജ്ഞാ(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    എങ്കിൽ (address >= vals->get_size()) {
		  val = vvp_object_t();
		  തിരികെ;
	    }

	    vals->get_word(address, val);
	    തിരികെ;
      }

      പ്രതിജ്ഞാ(nets);
	// Arrays of string nets not implemented!
      പ്രതിജ്ഞാ(0);
      തിരികെ;
}

string __vpiArray::get_word_str(unsigned address)
{
      എങ്കിൽ (vals) {
	    പ്രതിജ്ഞാ(vals4 == 0);
	    പ്രതിജ്ഞാ(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    എങ്കിൽ (address >= vals->get_size())
		  തിരികെ "";

	    string val;
	    vals->get_word(address, val);
	    തിരികെ val;
      }

      പ്രതിജ്ഞാ(nets);
	// Arrays of string nets not implemented!
      പ്രതിജ്ഞാ(0);
      തിരികെ "";
}

vpiHandle vpip_make_array(സ്ഥിരം അക്ഷരം*label, സ്ഥിരം അക്ഷരം*name,
                          പൂർണ്ണസംഖ്യ first_addr, പൂർണ്ണസംഖ്യ last_addr,
                          തർക്കം signed_flag)
{
      ഘടന __vpiArray*obj = പുതിയ __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      എങ്കിൽ (last_addr >= first_addr) {
	    obj->swap_addr = അസത്യം;
      } അല്ലെങ്കിൽ {
	    obj->swap_addr = സത്യം;
	    പൂർണ്ണസംഖ്യ tmp = last_addr;
	    last_addr = first_addr;
	    first_addr = tmp;
      }
      പ്രതിജ്ഞാ(last_addr >= first_addr);
      unsigned array_എണ്ണം = last_addr+1-first_addr;

	// For now, treat all arrays as memories. This is not quite
	// correct, as arrays are arrays with memories a special case.
      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_എണ്ണം = array_എണ്ണം;

      obj->first_addr.set_മൂല്യം(first_addr);
      obj->last_addr .set_മൂല്യം(last_addr);

	// Start off now knowing if we are nets or variables.
      obj->nets = 0;
      obj->vals4 = 0;
      obj->vals  = 0;
      obj->vals_width = 0;
      obj->vals_വാക്കുകൾ = 0;

	// Initialize (clear) the read-ports list.
      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

	/* Add this symbol to the array_symbols table for later lookup. */
      എങ്കിൽ (!array_table)
	    array_table = പുതിയ ചിഹ്നം_map_s<ഘടന __vpiArray>;

      പ്രതിജ്ഞാ(!array_find(label));
      array_table->sym_set_മൂല്യം(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_ചിഹ്നം(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_to_current_scope(obj);

      തിരികെ obj;
}

ശൂന്യം __vpiArray::alias_word(unsigned long addr, vpiHandle word, പൂർണ്ണസംഖ്യ msb_, പൂർണ്ണസംഖ്യ lsb_)
{
      പ്രതിജ്ഞാ(msb.get_മൂല്യം() == msb_);
      പ്രതിജ്ഞാ(lsb.get_മൂല്യം() == lsb_);
      പ്രതിജ്ഞാ(addr < get_size());
      പ്രതിജ്ഞാ(nets);
      nets[addr] = word;
}

ശൂന്യം __vpiArray::attach_word(unsigned addr, vpiHandle word)
{
      പ്രതിജ്ഞാ(addr < get_size());
      പ്രതിജ്ഞാ(nets);
      nets[addr] = word;

      എങ്കിൽ (ഘടന __vpiSignal*sig = dynamic_cast<__vpiSignal*>(word)) {
	    vvp_net_t*net = sig->കണ്ണി;
	    പ്രതിജ്ഞാ(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    പ്രതിജ്ഞാ(fun);
	    fun->attach_as_word(ഇത്, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = ഇത്;
	    sig->id.index = പുതിയ __vpiDecConst(addr + first_addr.get_മൂല്യം());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    തിരികെ;
      }

      എങ്കിൽ (ഘടന __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(word)) {
	    vvp_net_t*net = sig->net;
	    പ്രതിജ്ഞാ(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    പ്രതിജ്ഞാ(fun);
	    fun->attach_as_word(ഇത്, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = ഇത്;
	    sig->id.index = പുതിയ __vpiDecConst(addr + first_addr.get_മൂല്യം());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = സത്യം;
	    തിരികെ;
      }
}

ശൂന്യം compile_var_array(അക്ഷരം*label, അക്ഷരം*name, പൂർണ്ണസംഖ്യ last, പൂർണ്ണസംഖ്യ first,
		   പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb, അക്ഷരം signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last,
                                      signed_flag != 0);

      ഘടന __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_width = labs(msb-lsb) + 1;
      എങ്കിൽ (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = പുതിയ vvp_vector4array_aa(arr->vals_width,
						 arr->get_size());
      } അല്ലെങ്കിൽ {
            arr->vals4 = പുതിയ vvp_vector4array_sa(arr->vals_width,
						 arr->get_size());
      }
      arr->msb.set_മൂല്യം(msb);
      arr->lsb.set_മൂല്യം(lsb);

      എണ്ണം_var_arrays += 1;
      എണ്ണം_var_array_വാക്കുകൾ += arr->get_size();

      സ്വതന്ത്രം(label);
      delete[] name;
}

ശൂന്യം compile_var2_array(അക്ഷരം*label, അക്ഷരം*name, പൂർണ്ണസംഖ്യ last, പൂർണ്ണസംഖ്യ first,
		   പൂർണ്ണസംഖ്യ msb, പൂർണ്ണസംഖ്യ lsb, തർക്കം signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, signed_flag);

      ഘടന __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.set_മൂല്യം(msb);
      arr->lsb.set_മൂല്യം(lsb);
      arr->vals_width = labs(msb-lsb) + 1;

      പ്രതിജ്ഞാ(! arr->nets);
      എങ്കിൽ (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = പുതിയ vvp_darray_atom<int8_t>(arr->get_size());
      } അല്ലെങ്കിൽ എങ്കിൽ (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = പുതിയ vvp_darray_atom<uint8_t>(arr->get_size());
      } അല്ലെങ്കിൽ എങ്കിൽ (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = പുതിയ vvp_darray_atom<int16_t>(arr->get_size());
      } അല്ലെങ്കിൽ എങ്കിൽ (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = പുതിയ vvp_darray_atom<uint16_t>(arr->get_size());
      } അല്ലെങ്കിൽ എങ്കിൽ (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = പുതിയ vvp_darray_atom<int32_t>(arr->get_size());
      } അല്ലെങ്കിൽ എങ്കിൽ (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = പുതിയ vvp_darray_atom<uint32_t>(arr->get_size());
      } അല്ലെങ്കിൽ എങ്കിൽ (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = പുതിയ vvp_darray_atom<int64_t>(arr->get_size());
      } അല്ലെങ്കിൽ എങ്കിൽ (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = പുതിയ vvp_darray_atom<uint64_t>(arr->get_size());
      } അല്ലെങ്കിൽ {
	    arr->vals = പുതിയ vvp_darray_vec2(arr->get_size(), arr->vals_width);
      }
      എണ്ണം_var_arrays += 1;
      എണ്ണം_var_array_വാക്കുകൾ += arr->get_size();

      സ്വതന്ത്രം(label);
      delete[] name;
}

ശൂന്യം compile_real_array(അക്ഷരം*label, അക്ഷരം*name, പൂർണ്ണസംഖ്യ last, പൂർണ്ണസംഖ്യ first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, സത്യം);

      ഘടന __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = പുതിയ vvp_darray_real(arr->get_size());
      arr->vals_width = 1;

      എണ്ണം_real_arrays += 1;
      എണ്ണം_real_array_വാക്കുകൾ += arr->get_size();

      സ്വതന്ത്രം(label);
      delete[] name;
}

ശൂന്യം compile_string_array(അക്ഷരം*label, അക്ഷരം*name, പൂർണ്ണസംഖ്യ last, പൂർണ്ണസംഖ്യ first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, സത്യം);

      ഘടന __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = പുതിയ vvp_darray_string(arr->get_size());
      arr->vals_width = 1;

      എണ്ണം_real_arrays += 1;
      എണ്ണം_real_array_വാക്കുകൾ += arr->get_size();

      സ്വതന്ത്രം(label);
      delete[] name;
}

ശൂന്യം compile_object_array(അക്ഷരം*label, അക്ഷരം*name, പൂർണ്ണസംഖ്യ last, പൂർണ്ണസംഖ്യ first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, സത്യം);

      ഘടന __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = പുതിയ vvp_darray_object(arr->get_size());
      arr->vals_width = 1;

      എണ്ണം_real_arrays += 1;
      എണ്ണം_real_array_വാക്കുകൾ += arr->get_size();

      സ്വതന്ത്രം(label);
      delete[] name;
}

ശൂന്യം compile_net_array(അക്ഷരം*label, അക്ഷരം*name, പൂർണ്ണസംഖ്യ last, പൂർണ്ണസംഖ്യ first)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, first, last, അസത്യം);

      ഘടന __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(calloc(arr->get_size(), വലുപ്പം(vpiHandle)));

      എണ്ണം_net_arrays += 1;
      എണ്ണം_net_array_വാക്കുകൾ += arr->get_size();

      സ്വതന്ത്രം(label);
      delete[] name;
}

വർഗ്ഗം vvp_fun_arrayport  : പൊതു vvp_net_fun_t {

    പൊതു:
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport() override;

      മായാ ശൂന്യം check_word_change(unsigned long addr) = 0;

    സംരക്ഷിത:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      unsigned long addr_;

      friend ശൂന്യം array_attach_port(vvp_array_t, vvp_fun_arrayport*);
      friend ശൂന്യം __vpiArray::word_change(unsigned long);
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

വർഗ്ഗം vvp_fun_arrayport_sa  : പൊതു vvp_fun_arrayport {

    പൊതു:
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_sa() override;

      ശൂന്യം check_word_change(unsigned long addr) override;

      ശൂന്യം recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                     vvp_context_t) override;

    സ്വകാര്യ:
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

ശൂന്യം vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                                     vvp_context_t)
{
      തർക്കം addr_valid_flag;

      തിരഞ്ഞെടുപ്പ് (port.port()) {

	  അവസ്ഥ 0: // Address input
	    addr_valid_flag = vector4_to_മൂല്യം(bit, addr_);
	    എങ്കിൽ (! addr_valid_flag)
		  addr_ = arr_->get_size();
	    എങ്കിൽ (vpi_array_is_real(arr_))
		  port.ptr()->send_real(arr_->get_word_r(addr_), 0);
	    അല്ലെങ്കിൽ
		  port.ptr()->send_vec4(arr_->get_word(addr_), 0);

	    നിർത്തുക;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stdout, "XXXX write ports not implemented.\n");
	    പ്രതിജ്ഞാ(0);
      }
}

ശൂന്യം vvp_fun_arrayport_sa::check_word_change(unsigned long addr)
{
      എങ്കിൽ (addr != addr_) തിരികെ;

      എങ്കിൽ (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr_), 0);
      } അല്ലെങ്കിൽ {
	    net_->send_vec4(arr_->get_word(addr_), 0);
      }
}

വർഗ്ഗം vvp_fun_arrayport_aa  : പൊതു vvp_fun_arrayport, പൊതു automatic_hooks_s {

    പൊതു:
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_aa() override;

      ശൂന്യം alloc_instance(vvp_context_t context) override;
      ശൂന്യം reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ശൂന്യം free_instance(vvp_context_t context) override;
#endif

      ശൂന്യം check_word_change(unsigned long addr) override;

      ശൂന്യം recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                     vvp_context_t context) override;

    സ്വകാര്യ:
      ശൂന്യം check_word_change_(unsigned long addr, vvp_context_t context);

      __vpiScope*context_scope_;
      unsigned context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(ഇത്, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(ഇത്, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

ശൂന്യം vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      unsigned long*addr = പുതിയ unsigned long;
      vvp_set_context_item(context, context_idx_, addr);

      *addr = addr_;
}

ശൂന്യം vvp_fun_arrayport_aa::reset_instance(vvp_context_t context)
{
      unsigned long*addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));

      *addr = addr_;
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം vvp_fun_arrayport_aa::free_instance(vvp_context_t context)
{
      unsigned long*addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));
      delete addr;
}
#endif

ശൂന്യം vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                                     vvp_context_t context)
{
      എങ്കിൽ (context) {
            unsigned long*addr = static_cast<unsigned long*>
                  (vvp_get_context_item(context, context_idx_));

            തർക്കം addr_valid_flag;

            തിരഞ്ഞെടുപ്പ് (port.port()) {

                അവസ്ഥ 0: // Address input
                  addr_valid_flag = vector4_to_മൂല്യം(bit, *addr);
                  എങ്കിൽ (! addr_valid_flag) *addr = arr_->get_size();
                  എങ്കിൽ (vpi_array_is_real(arr_)) {
			port.ptr()->send_real(arr_->get_word_r(*addr),
					      context);
                  } അല്ലെങ്കിൽ {
			port.ptr()->send_vec4(arr_->get_word(*addr),
					      context);
                  }
                  നിർത്തുക;

                സ്ഥിരസ്ഥിതി:
                  fprintf(stdout, "XXXX write ports not implemented.\n");
                  പ്രതിജ്ഞാ(0);
            }
      } അല്ലെങ്കിൽ {
            context = context_scope_->live_contexts;
            ആകുമ്പോൾ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

ശൂന്യം vvp_fun_arrayport_aa::check_word_change_(unsigned long addr,
                                              vvp_context_t context)
{
      സ്ഥിരം unsigned long*port_addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));

      എങ്കിൽ (addr != *port_addr)
	    തിരികെ;

      എങ്കിൽ (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr), context);
      } അല്ലെങ്കിൽ {
	    net_->send_vec4(arr_->get_word(addr), context);
      }
}

ശൂന്യം vvp_fun_arrayport_aa::check_word_change(unsigned long addr)
{
      എങ്കിൽ (arr_->get_scope()->is_automatic()) {
            പ്രതിജ്ഞാ(vthread_get_wt_context());
            check_word_change_(addr, vthread_get_wt_context());
      } അല്ലെങ്കിൽ {
            vvp_context_t context = context_scope_->live_contexts;
            ആകുമ്പോൾ (context) {
                  check_word_change_(addr, context);
                  context = vvp_get_next_context(context);
            }
      }
}

സ്ഥിതം ശൂന്യം array_attach_port(vvp_array_t array, vvp_fun_arrayport*fun)
{
      പ്രതിജ്ഞാ(fun->next_ == 0);
      fun->next_ = array->ports_;
      array->ports_ = fun;
      എങ്കിൽ (!array->get_scope()->is_automatic() &&
          (array->vals4 || array->vals)) {
              /* propagate initial values for variable arrays */
            എങ്കിൽ (!vpi_array_is_real(array)) {
                  vvp_bit4_t init;
                  എങ്കിൽ (array->vals4)
                      init = BIT4_X;
                  അല്ലെങ്കിൽ
                      init = BIT4_0;
                  vvp_vector4_t tmp(array->vals_width, init);
                  schedule_init_propagate(fun->net_, tmp);
            } അല്ലെങ്കിൽ {
                  schedule_init_propagate(fun->net_, 0.0);
            }
      }
}

വർഗ്ഗം array_word_മൂല്യം_callback : പൊതു മൂല്യം_callback {
    പൊതു:
      വരിയിൽ explicit array_word_മൂല്യം_callback(p_cb_data data, long addr)
      : മൂല്യം_callback(data), word_addr(addr)
      { }

    പൊതു:
      long word_addr;
};

ശൂന്യം __vpiArray::word_change(unsigned long addr)
{
      വേണ്ടി (vvp_fun_arrayport*cur = ports_; cur; cur = cur->next_)
	    cur->check_word_change(addr);

	// Run callbacks attached to the array itself.
      ഘടന __vpiCallback *next = vpi_callbacks;
      ഘടന __vpiCallback *prev = 0;

      ആകുമ്പോൾ (next) {
	    array_word_മൂല്യം_callback*cur = dynamic_cast<array_word_മൂല്യം_callback*>(next);
	    next = cur->next;

	      // Skip callbacks that are not for me. -1 is for every element.
	    എങ്കിൽ (cur->word_addr != (long)addr && cur->word_addr != -1) {
		  prev = cur;
		  തുടരുക;
	    }

	      // For whole array callbacks we need to set the index.
	    എങ്കിൽ (cur->word_addr == -1) {
		  cur->cb_data.index = (PLI_INT32) ((പൂർണ്ണസംഖ്യ)addr +
						    first_addr.get_മൂല്യം());
	    }

	    എങ്കിൽ (cur->cb_data.cb_rtn != 0) {
		  എങ്കിൽ (cur->test_മൂല്യം_callback_ready()) {
			എങ്കിൽ (cur->cb_data.value) {
			      എങ്കിൽ (vpi_array_is_real(ഇത്)) {
				    ഇരട്ട val = 0.0;
				    എങ്കിൽ (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_real_get_മൂല്യം(val, cur->cb_data.value);
			      } അല്ലെങ്കിൽ എങ്കിൽ (vals4) {
				    vpip_vec4_get_മൂല്യം(vals4->get_word(addr),
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } അല്ലെങ്കിൽ എങ്കിൽ (dynamic_cast<vvp_darray_atom<int8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int64_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint64_t>*>(vals)
				      || dynamic_cast<vvp_darray_vec2*>(vals)) {
				    vvp_vector4_t val;
				    എങ്കിൽ (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_vec4_get_മൂല്യം(val,
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } അല്ലെങ്കിൽ {
			            പ്രതിജ്ഞാ(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } അല്ലെങ്കിൽ എങ്കിൽ (prev == 0) {

		  vpi_callbacks = next;
		  cur->next = 0;
		  delete cur;

	    } അല്ലെങ്കിൽ {
		  പ്രതിജ്ഞാ(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}

വർഗ്ഗം array_resolv_list_t : പൊതു resolv_list_s {

    പൊതു:
      explicit array_resolv_list_t(അക്ഷരം*lab) : resolv_list_s(lab) {
	    array = 0;
      }

      vvp_array_t*array;
      തർക്കം resolve(തർക്കം mes) override;

    സ്വകാര്യ:
};

തർക്കം array_resolv_list_t::resolve(തർക്കം mes)
{
      *array = array_find(label());
      എങ്കിൽ (*array == 0) {
	    പ്രതിജ്ഞാ(!mes);
	    തിരികെ അസത്യം;
      }
      തിരികെ സത്യം;
}

വർഗ്ഗം array_port_resolv_list_t : പൊതു resolv_list_s {

    പൊതു:
      explicit array_port_resolv_list_t(അക്ഷരം* lab, തർക്കം use_addr__,
                                        long addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      തർക്കം use_addr;
      long addr;
      തർക്കം resolve(തർക്കം mes) override;

    സ്വകാര്യ:
};

array_port_resolv_list_t::array_port_resolv_list_t(അക്ഷരം *lab, തർക്കം use_addr__,
                                                   long addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      എങ്കിൽ (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      അല്ലെങ്കിൽ
            context_scope = 0;
      ptr = പുതിയ vvp_net_t;
}

തർക്കം array_port_resolv_list_t::resolve(തർക്കം mes)
{
      vvp_array_t mem = array_find(label());
      എങ്കിൽ (mem == 0) {
	    പ്രതിജ്ഞാ(mem || !mes);
	    തിരികെ അസത്യം;
      }

      vvp_fun_arrayport*fun;
      എങ്കിൽ (use_addr)
            എങ്കിൽ (context_scope)
                  fun = പുതിയ vvp_fun_arrayport_aa(context_scope, mem, ptr, addr);
            അല്ലെങ്കിൽ
                  fun = പുതിയ vvp_fun_arrayport_sa(mem, ptr, addr);
      അല്ലെങ്കിൽ
            എങ്കിൽ (context_scope)
                  fun = പുതിയ vvp_fun_arrayport_aa(context_scope, mem, ptr);
            അല്ലെങ്കിൽ
                  fun = പുതിയ vvp_fun_arrayport_sa(mem, ptr);
      ptr->fun = fun;

      array_attach_port(mem, fun);

      തിരികെ സത്യം;
}

വർഗ്ഗം array_word_part_callback : പൊതു array_word_മൂല്യം_callback {
    പൊതു:
      explicit array_word_part_callback(p_cb_data data, long addr);
      ~array_word_part_callback() override;

      array_word_part_callback(സ്ഥിരം array_word_part_callback&) = delete;
      array_word_part_callback& operator=(സ്ഥിരം array_word_part_callback&) = delete;

      തർക്കം test_മൂല്യം_callback_ready(ശൂന്യം) override;

    സ്വകാര്യ:
      അക്ഷരം*മൂല്യം_bits;
};

array_word_part_callback::array_word_part_callback(p_cb_data data, long addr)
: array_word_മൂല്യം_callback(data, addr)
{
	// Get the initial value of the part, to use as a reference.
      ഘടന __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj);
      s_vpi_മൂല്യം tmp_മൂല്യം;
      tmp_മൂല്യം.format = vpiBinStrVal;
      apvword->vpi_get_മൂല്യം(&tmp_മൂല്യം);

      മൂല്യം_bits = പുതിയ അക്ഷരം[apvword->part_wid+1];

      memcpy(മൂല്യം_bits, tmp_മൂല്യം.value.str, apvword->part_wid);
      മൂല്യം_bits[apvword->part_wid] = 0;
}

array_word_part_callback::~array_word_part_callback()
{
      delete[]മൂല്യം_bits;
}

തർക്കം array_word_part_callback::test_മൂല്യം_callback_ready(ശൂന്യം)
{
      ഘടന __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_data.obj);
      പ്രതിജ്ഞാ(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_മൂല്യം tmp_മൂല്യം;
      tmp_മൂല്യം.format = vpiBinStrVal;
      apvword->vpi_get_മൂല്യം(&tmp_മൂല്യം);

      എങ്കിൽ (memcmp(മൂല്യം_bits, tmp_മൂല്യം.value.str, apvword->part_wid) == 0)
	    തിരികെ അസത്യം;

      memcpy(മൂല്യം_bits, tmp_മൂല്യം.value.str, apvword->part_wid);
      തിരികെ സത്യം;

}

മൂല്യം_callback*vpip_array_word_change(p_cb_data data)
{
      ഘടന __vpiArray*parent = 0;
      array_word_മൂല്യം_callback*cbh = 0;
      എങ്കിൽ (സ്ഥിരം ഘടന __vpiArrayWord*word = array_var_word_from_handle(data->obj)) {
	    parent = static_cast<__vpiArray*>(word->get_parent());
	    unsigned addr = word->get_index();
	    cbh = പുതിയ array_word_മൂല്യം_callback(data, addr);

      } അല്ലെങ്കിൽ എങ്കിൽ (ഘടന __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(data->obj)) {
	    parent = tword->array;
	    cbh = പുതിയ array_word_മൂല്യം_callback(data, tword->address);

      } അല്ലെങ്കിൽ എങ്കിൽ (ഘടന __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj)) {
	    parent = apvword->array;
	    cbh = പുതിയ array_word_part_callback(data, apvword->word_sel);
      }

      പ്രതിജ്ഞാ(cbh);
      പ്രതിജ്ഞാ(parent);
      cbh->next = parent->vpi_callbacks;
      parent->vpi_callbacks = cbh;

      തിരികെ cbh;
}

മൂല്യം_callback* vpip_array_change(p_cb_data data)
{
      array_word_മൂല്യം_callback*cbh = പുതിയ array_word_മൂല്യം_callback(data, -1);
      പ്രതിജ്ഞാ(data->obj);

      ഘടന __vpiArray*arr = dynamic_cast<__vpiArray*>(data->obj);
      cbh->next = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      തിരികെ cbh;
}

ശൂന്യം compile_array_port(അക്ഷരം*label, അക്ഷരം*array, അക്ഷരം*addr)
{
      array_port_resolv_list_t*resolv_mem
	    = പുതിയ array_port_resolv_list_t(array, അസത്യം, 0);

      define_functor_ചിഹ്നം(label, resolv_mem->ptr);
      സ്വതന്ത്രം(label);
	// Connect the port-0 input as the address.
      നിവേശം_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

ശൂന്യം compile_array_port(അക്ഷരം*label, അക്ഷരം*array, long addr)
{
      array_port_resolv_list_t*resolv_mem
	    = പുതിയ array_port_resolv_list_t(array, സത്യം, addr);

      define_functor_ചിഹ്നം(label, resolv_mem->ptr);
      സ്വതന്ത്രം(label);

      resolv_submit(resolv_mem);
}

ശൂന്യം compile_array_alias(അക്ഷരം*label, അക്ഷരം*name, അക്ഷരം*src)
{
      vvp_array_t mem = array_find(src);
      പ്രതിജ്ഞാ(mem);

      ഘടന __vpiArray*obj = പുതിയ __vpiArray;

      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_എണ്ണം = mem->array_എണ്ണം;
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
      obj->vals_വാക്കുകൾ = mem->vals_വാക്കുകൾ;

      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

      പ്രതിജ്ഞാ(array_table);
      പ്രതിജ്ഞാ(!array_find(label));
      array_table->sym_set_മൂല്യം(label, obj);

      compile_vpi_ചിഹ്നം(label, obj);
      vpip_attach_to_current_scope(obj);

      സ്വതന്ത്രം(label);
      സ്വതന്ത്രം(name);
      സ്വതന്ത്രം(src);
}

/*
 * &A<label,addr>
 * This represents a VPI handle for an addressed array. This comes
 * from expressions like "label[addr]" where "label" is the array and
 * "addr" is the canonical address of the desired word.
 */
vpiHandle vpip_make_vthr_A(അക്ഷരം*label, unsigned addr)
{
      ഘടന __vpiArrayVthrA*obj = പുതിയ __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = പുതിയ array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      obj->address = addr;

      തിരികെ obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(അക്ഷരം*label, അക്ഷരം*ചിഹ്നം)
{
      ഘടന __vpiArrayVthrA*obj = പുതിയ __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = പുതിയ array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      compile_vpi_lookup(&obj->address_handle, ചിഹ്നം);
      obj->address = 0;

      തിരികെ obj;
}

vpiHandle vpip_make_vthr_A(അക്ഷരം*label, vpiHandle handle)
{
      ഘടന __vpiArrayVthrA*obj = പുതിയ __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = പുതിയ array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = handle;
      obj->address = 0;

      തിരികെ obj;
}

vpiHandle vpip_make_vthr_APV(അക്ഷരം*label, unsigned index, unsigned bit, unsigned wid)
{
      ഘടന __vpiArrayVthrAPV*obj = പുതിയ __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = പുതിയ array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->word_sel = index;
      obj->part_bit = bit;
      obj->part_wid = wid;

      തിരികെ obj;
}

ശൂന്യം compile_array_cleanup(ശൂന്യം)
{
      delete array_table;
      array_table = 0;
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം memory_delete(vpiHandle item)
{
      ഘടന __vpiArray*arr = static_cast<ഘടന __vpiArray*>(item);
      എങ്കിൽ (arr->vals_വാക്കുകൾ) delete [] (arr->vals_വാക്കുകൾ-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals;

      എങ്കിൽ (arr->nets) {
	    വേണ്ടി (unsigned idx = 0; idx < arr->get_size(); idx += 1) {
		  എങ്കിൽ (ഘടന __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.index);
		    /* These should only be the real words. */
		  } അല്ലെങ്കിൽ {
			പ്രതിജ്ഞാ(arr->nets[idx]->get_type_code() ==
			       vpiRealVar);
			ഘടന __vpiRealVar *sigr = static_cast<ഘടന __vpiRealVar *>
			                            (arr->nets[idx]);
			constant_delete(sigr->id.index);
// Why are only the real words still here?
			delete arr->nets[idx];
		  }
	    }
	    സ്വതന്ത്രം(arr->nets);
      }

      ആകുമ്പോൾ (arr->vpi_callbacks) {
	    ഘടന __vpiCallback*tmp = arr->vpi_callbacks->next;
	    delete arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      delete arr;
}

ശൂന്യം A_delete(vpiHandle item)
{
      ഘടന __vpiArrayVthrA*obj = static_cast<ഘടന __vpiArrayVthrA*>(item);
      എങ്കിൽ (obj->address_handle) {
	    തിരഞ്ഞെടുപ്പ് (obj->address_handle->get_type_code()) {
		അവസ്ഥ vpiMemoryWord:
		  എങ്കിൽ (vpi_get(_vpiFromThr, obj->address_handle) == _vpi_at_A) {
			A_delete(obj->address_handle);
		  }
		  നിർത്തുക;
		അവസ്ഥ vpiPartSelect:
		  പ്രതിജ്ഞാ(vpi_get(_vpiFromThr, obj->address_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->address_handle);
		  നിർത്തുക;
	    }
      }

      delete obj;
}

ശൂന്യം APV_delete(vpiHandle item)
{
      ഘടന __vpiArrayVthrAPV*obj = static_cast<ഘടന __vpiArrayVthrAPV*>(item);
      delete obj;
}
#endif
