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
#ଅନ୍ତର୍ଭାଵ  "vvp_cleanup.h"
#endif
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <iostream>
# include  "compile.h"
# include  <cassert>
# include  "ivl_alloc.h"

using ନାମାକାଶ std;

unsigned long ଗଣନା_net_arrays = 0;
unsigned long ଗଣନା_net_array_ଶବ୍ଦଗୁଡ଼ିକ = 0;
unsigned long ଗଣନା_var_arrays = 0;
unsigned long ଗଣନା_var_array_ଶବ୍ଦଗୁଡ଼ିକ = 0;
unsigned long ଗଣନା_real_arrays = 0;
unsigned long ଗଣନା_real_array_ଶବ୍ଦଗୁଡ଼ିକ = 0;

ସ୍ଥିର ପ୍ରତୀକ_map_s<ସଂରଚନା __vpiArray>* array_table =0;

ଶ୍ରେଣୀ vvp_fun_arrayport;
ସ୍ଥିର ଶୂନ୍ୟ array_attach_port(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_find(ଅଚଳ ଅକ୍ଷର*label)
{
      ଯଦି (array_table == 0)
	    ଫେରନ୍ତୁ 0;

      vvp_array_t v = array_table->sym_get_ମୂଲ୍ୟ(label);
      ଫେରନ୍ତୁ v;
}

ସଂରଚନା __vpiArrayVthrA : ସର୍ବସାଧାରଣ __vpiHandle {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiMemoryWord; }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଅକ୍ଷର* vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;
      vpiHandle vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) override;
      vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;

      ସଂରଚନା __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle address_handle;
	// If wid==0, then address is the address into the array.
      unsigned address;

      unsigned get_address() ଅଚଳ
      {
	    ଯଦି (address_handle) {
		  s_vpi_ମୂଲ୍ୟ vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpiVectorVal;
		  address_handle->vpi_get_ମୂଲ୍ୟ(&vp);
		  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଶବ୍ଦଗୁଡ଼ିକ = (address_handle->vpi_get(vpiSize)-1)/32 + 1;
		  ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0; idx < ଶବ୍ଦଗୁଡ଼ିକ; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			ଯଦି (vp.value.vector[idx].bval != 0) ଫେରନ୍ତୁ UINT_MAX;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  address_handle->vpi_get_ମୂଲ୍ୟ(&vp);
		  ଫେରନ୍ତୁ vp.value.integer;
	    }

	    ଫେରନ୍ତୁ address;
      }
};


ସଂରଚନା __vpiArrayVthrAPV : ସର୍ବସାଧାରଣ __vpiHandle {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiPartSelect; }
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଅକ୍ଷର* vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;
      vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;

      ସଂରଚନା __vpiArray*array;
      unsigned word_sel;
      unsigned part_bit;
      unsigned part_wid;
};

ତାର୍କିକ is_net_array(vpiHandle obj)
{
      ସଂରଚନା __vpiArray*rfp = dynamic_cast<__vpiArray*> (obj);
      ପ୍ରତିଜ୍ଞା(rfp);

      ଯଦି (rfp->nets != 0) ଫେରନ୍ତୁ ସତ୍ୟ;
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

// This function return true if the underlying array words are real.
ସ୍ଥିର ତାର୍କିକ vpi_array_is_real(ଅଚଳ vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      ଯଦି (arr->vals4 != 0)  // A bit based variable/register array.
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;

      ଯଦି (dynamic_cast<vvp_darray_real*> (arr->vals))
	    ଫେରନ୍ତୁ ସତ୍ୟ;

      ଯଦି (arr->vals != 0)
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;

	// This must be a net array so look at element 0 to find the type.
      ପ୍ରତିଜ୍ଞା(arr->nets != 0);
      ପ୍ରତିଜ୍ଞା(arr->get_size() > 0);
      ଅଚଳ ସଂରଚନା __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      ଯଦି (rsig) {
	    ଫେରନ୍ତୁ ସତ୍ୟ;
      }

      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ସ୍ଥିର ତାର୍କିକ vpi_array_is_string(ଅଚଳ vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      ଯଦି (arr->vals4 != 0)  // A bit based variable/register array.
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;

      ଯଦି (dynamic_cast<vvp_darray_string*> (arr->vals))
	    ଫେରନ୍ତୁ ସତ୍ୟ;

      ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiArray::get_word_size() ଅଚଳ
{
      unsigned width;

      ପ୍ରତିଜ୍ଞା(get_size() > 0);
	/* For a net array we need to get the width from the first element. */
      ଯଦି (nets) {
	    ପ୍ରତିଜ୍ଞା(vals4 == 0 && vals == 0);
	    ଅଚଳ ସଂରଚନା __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    ପ୍ରତିଜ୍ଞା(vsig);
	    width = vpip_size(vsig);
	/* For a variable array we can get the width from vals_width. */
      } ଅନ୍ୟଥା {
	    ପ୍ରତିଜ୍ଞା(vals4 || vals);
	    width = vals_width;
      }

      ଫେରନ୍ତୁ width;
}

ଅକ୍ଷର*__vpiArray::get_word_str(ସଂରଚନା __vpiArrayWord*word, ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      unsigned index = word->get_index();

      ଯଦି (code == vpiFile) {  // Not implemented for now!
	    ଫେରନ୍ତୁ simple_set_rbuf_str(file_names[0]);
      }

      ଅକ୍ଷର sidx [64];
      snprintf(sidx, 63, "%d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)index + first_addr.get_ମୂଲ୍ୟ());
      ଫେରନ୍ତୁ generic_get_str(code, get_scope(), name, sidx);
}

ଶୂନ୍ୟ __vpiArray::get_word_ମୂଲ୍ୟ(ସଂରଚନା __vpiArrayWord*word, p_vpi_ମୂଲ୍ୟ vp)
{
      unsigned index = word->get_index();

    // Determine the appropriate format (The Verilog PLI Handbook 5.2.10)
      ଯଦି(vp->format == vpiObjTypeVal) {
          ଯଦି(vpi_array_is_real(ଏହି))
              vp->format = vpiRealVal;
          ଅନ୍ୟଥା ଯଦି(vpi_array_is_string(ଏହି))
              vp->format = vpiStringVal;
          ଅନ୍ୟଥା
              vp->format = vpiIntVal;
      }

      ଯଦି(vals4) {
          vpip_vec4_get_ମୂଲ୍ୟ(vals4->get_word(index),
                              vals4->width(), signed_flag, vp);
      } ଅନ୍ୟଥା ଯଦି(vals) {
          ବାଛ(vp->format) {
            ସ୍ଥିତି vpiBinStrVal:
            ସ୍ଥିତି vpiOctStrVal:
            ସ୍ଥିତି vpiDecStrVal:
            ସ୍ଥିତି vpiHexStrVal:
            ସ୍ଥିତି vpiScalarVal:
            ସ୍ଥିତି vpiIntVal:
            ସ୍ଥିତି vpiVectorVal:
            {
                vvp_vector4_t v;
                vals->get_word(index, v);
                vpip_vec4_get_ମୂଲ୍ୟ(v, vals_width, signed_flag, vp);
            }
            ଭାଙ୍ଗ;

            ସ୍ଥିତି vpiRealVal:
            {
                ଦ୍ୱିଗୁଣ d;
                vals->get_word(index, d);
                vpip_real_get_ମୂଲ୍ୟ(d, vp);
            }
            ଭାଙ୍ଗ;

            ସ୍ଥିତି vpiStringVal:
            {
                string s;
                vals->get_word(index, s);
                vpip_string_get_ମୂଲ୍ୟ(s, vp);
            }
            ଭାଙ୍ଗ;

            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                fprintf(stderr, "vpi sorry: format is not implemented\n");
                ପ୍ରତିଜ୍ଞା(ମିଥ୍ୟା);
          }
      }
}

ଶୂନ୍ୟ __vpiArray::put_word_ମୂଲ୍ୟ(ସଂରଚନା __vpiArrayWord*word, p_vpi_ମୂଲ୍ୟ vp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{
      unsigned index = word->get_index();
      vvp_vector4_t val = vec4_from_vpi_ମୂଲ୍ୟ(vp, vals_width);
      set_word(index, 0, val);
}

vpiHandle __vpiArray::get_iter_index(ସଂରଚନା __vpiArrayIterator*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
      ଯଦି (nets) ଫେରନ୍ତୁ nets[idx];

      ପ୍ରତିଜ୍ଞା(vals4 || vals);

      ଯଦି (vals_ଶବ୍ଦଗୁଡ଼ିକ == 0) make_vals_ଶବ୍ଦଗୁଡ଼ିକ();

      ଫେରନ୍ତୁ &(vals_ଶବ୍ଦଗୁଡ଼ିକ[idx].as_word);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiArray::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpi_ଧାଡ଼ି_No:
	    ଫେରନ୍ତୁ 0; // Not implemented for now!

	  ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ get_size();

	  ସ୍ଥିତି vpiAutomatic:
	    ଫେରନ୍ତୁ scope->is_automatic()? 1 : 0;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ 0;
      }
}

ଅକ୍ଷର* __vpiArray::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ଯଦି (code == vpiFile) {  // Not implemented for now!
            ଫେରନ୍ତୁ simple_set_rbuf_str(file_names[0]);
      }

      ଫେରନ୍ତୁ generic_get_str(code, scope, name, ଖାଲି);
}

vpiHandle __vpiArray::vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
          ସ୍ଥିତି vpi_ବାମ_Range:
            ଯଦି (swap_addr) ଫେରନ୍ତୁ &last_addr;
            ଅନ୍ୟଥା ଫେରନ୍ତୁ &first_addr;

          ସ୍ଥିତି vpi_ଡାହାଣ_Range:
            ଯଦି (swap_addr) ଫେରନ୍ତୁ &first_addr;
            ଅନ୍ୟଥା ଫେରନ୍ତୁ &last_addr;

	  ସ୍ଥିତି vpiScope:
	    ଫେରନ୍ତୁ scope;

	  ସ୍ଥିତି vpiModule:
	    ଫେରନ୍ତୁ vpip_module(scope);
      }

      ଫେରନ୍ତୁ 0;
}

/*
* VPI code passes indices that are not yet converted to canonical
* form, so this index function does it here.
*/
vpiHandle __vpiArray::vpi_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index)
{
      index -= first_addr.get_ମୂଲ୍ୟ();
      ଯଦି (index >= (long) get_size())
	    ଫେରନ୍ତୁ 0;
      ଯଦି (index < 0)
	    ଫେରନ୍ତୁ 0;

      ଯଦି (nets != 0) {
	    ଫେରନ୍ତୁ nets[index];
      }

      ଯଦି (vals_ଶବ୍ଦଗୁଡ଼ିକ == 0)
	    make_vals_ଶବ୍ଦଗୁଡ଼ିକ();

      ଫେରନ୍ତୁ &(vals_ଶବ୍ଦଗୁଡ଼ିକ[index].as_word);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiArrayWord::as_word_t::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ସଂରଚନା __vpiArrayWord*obj = array_var_word_from_handle(ଏହି);
      ପ୍ରତିଜ୍ଞା(obj);
      ସଂରଚନା __vpiArrayBase*my_parent = obj->get_parent();
      t_vpi_ମୂଲ୍ୟ val;

      ବାଛ (code) {
	  ସ୍ଥିତି vpi_ଧାଡ଼ି_No:
	    ଫେରନ୍ତୁ 0; // Not implemented for now!

	  ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ my_parent->get_word_size();

	  ସ୍ଥିତି vpi_ବାମ_Range:
            val.format = vpiIntVal;
	    my_parent->get_ବାମ_range()->vpi_get_ମୂଲ୍ୟ(&val);
            ପ୍ରତିଜ୍ଞା(val.format == vpiIntVal);
            ଫେରନ୍ତୁ val.value.integer;

	  ସ୍ଥିତି vpi_ଡାହାଣ_Range:
            val.format = vpiIntVal;
	    my_parent->get_ଡାହାଣ_range()->vpi_get_ମୂଲ୍ୟ(&val);
            ପ୍ରତିଜ୍ଞା(val.format == vpiIntVal);
            ଫେରନ୍ତୁ val.value.integer;

	  ସ୍ଥିତି vpiIndex:
	    {
		  ପୂର୍ଣ୍ଣସଂଖ୍ୟା base_offset = 0;
		  ସଂରଚନା __vpiArray*base = dynamic_cast<__vpiArray*> (my_parent);
		  ଯଦି (base) {
			val.format = vpiIntVal;
			base->first_addr.vpi_get_ମୂଲ୍ୟ(&val);
			base_offset += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_index.vpi_get_ମୂଲ୍ୟ(&val);
		  ପ୍ରତିଜ୍ଞା(val.format == vpiIntVal);
		  ଫେରନ୍ତୁ val.value.integer + base_offset;
	    }

	  ସ୍ଥିତି vpiAutomatic:
	    ଫେରନ୍ତୁ my_parent->get_scope()->is_automatic()? 1 : 0;

#ଯଦି defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ସ୍ଥିତି _vpiFromThr:
	    ଫେରନ୍ତୁ _vpiNoThr;
#endif

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ 0;
      }
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiArrayVthrA::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpi_ଧାଡ଼ି_No:
	    ଫେରନ୍ତୁ 0; // Not implemented for now!

	  ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ array->get_word_size();

	  ସ୍ଥିତି vpi_ବାମ_Range:
	    ଫେରନ୍ତୁ array->msb.get_ମୂଲ୍ୟ();

	  ସ୍ଥିତି vpi_ଡାହାଣ_Range:
	    ଫେରନ୍ତୁ array->lsb.get_ମୂଲ୍ୟ();

	  ସ୍ଥିତି vpiIndex:
	    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)get_address() + array->first_addr.get_ମୂଲ୍ୟ();

	  ସ୍ଥିତି vpiAutomatic:
	    ଫେରନ୍ତୁ array->get_scope()->is_automatic() ? 1 : 0;

#ଯଦି defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ସ୍ଥିତି _vpiFromThr:
	    ଫେରନ୍ତୁ _vpi_at_A;
#endif

	  // If address_handle is not zero we definitely have a
	  // variable.
	  ସ୍ଥିତି vpiConstantSelect:
	    ଫେରନ୍ତୁ address_handle == 0;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ 0;
      }
}

ଅକ୍ଷର* __vpiArrayVthrA::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ଯଦି (code == vpiFile) {  // Not implemented for now!
            ଫେରନ୍ତୁ simple_set_rbuf_str(file_names[0]);
      }

      ଅକ୍ଷର sidx [64];
      snprintf(sidx, 63, "%d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)get_address() + array->first_addr.get_ମୂଲ୍ୟ());
      ଫେରନ୍ତୁ generic_get_str(code, array->get_scope(), array->name, sidx);
}

ଶୂନ୍ୟ __vpiArrayVthrA::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ vp)
{
      ପ୍ରତିଜ୍ଞା(array);

      unsigned index = get_address();
      ଯଦି (vpi_array_is_real(array)) {
	    ଦ୍ୱିଗୁଣ tmp = array->get_word_r(index);
	    vpip_real_get_ମୂଲ୍ୟ(tmp, vp);
      } ଅନ୍ୟଥା ଯଦି (vpi_array_is_string(array)) {
	    string tmp = array->get_word_str(index);
	    vpip_string_get_ମୂଲ୍ୟ(tmp, vp);
      } ଅନ୍ୟଥା {
	    vvp_vector4_t tmp = array->get_word(index);
	    unsigned width = array->get_word_size();
	    vpip_vec4_get_ମୂଲ୍ୟ(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ vp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{
      unsigned index = get_address();

      ପ୍ରତିଜ୍ଞା(array);
      ପ୍ରତିଜ୍ଞା(index < array->get_size());

      ଯଦି (vpi_array_is_real(array)) {
	    ଦ୍ୱିଗୁଣ val = real_from_vpi_ମୂଲ୍ୟ(vp);
	    array->set_word(index, val);
      } ଅନ୍ୟଥା {
	    unsigned width = array->get_word_size();
	    vvp_vector4_t val = vec4_from_vpi_ମୂଲ୍ୟ(vp, width);
	    array->set_word(index, 0, val);
      }

      ଫେରନ୍ତୁ ଏହି;
}

vpiHandle __vpiArrayVthrA::vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiIndex:
	    ଭାଙ୍ଗ;  // Not implemented!

	  ସ୍ଥିତି vpi_ବାମ_Range:
	    ଫେରନ୍ତୁ &array->msb;

	  ସ୍ଥିତି vpi_ଡାହାଣ_Range:
	    ଫେରନ୍ତୁ &array->lsb;

	  ସ୍ଥିତି vpiParent:
	  ସ୍ଥିତି vpiArray:
	    ଫେରନ୍ତୁ array;

	  ସ୍ଥିତି vpiScope:
	    ଫେରନ୍ତୁ array->get_scope();

	  ସ୍ଥିତି vpiModule:
	    ଫେରନ୍ତୁ vpip_module(array->get_scope());
      }

      ଫେରନ୍ତୁ 0;
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiArrayVthrAPV::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpi_ଧାଡ଼ି_No:
	    ଫେରନ୍ତୁ 0; // Not implemented for now!

	  ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ part_wid;

	  ସ୍ଥିତି vpi_ବାମ_Range:
	    ଫେରନ୍ତୁ part_bit + part_wid - 1;

	  ସ୍ଥିତି vpi_ଡାହାଣ_Range:
	    ଫେରନ୍ତୁ part_bit;

	  ସ୍ଥିତି vpiAutomatic:
	    ଫେରନ୍ତୁ array->get_scope()->is_automatic() ? 1 : 0;

#ଯଦି defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ସ୍ଥିତି _vpiFromThr:
	    ଫେରନ୍ତୁ _vpi_at_APV;
#endif

	  ସ୍ଥିତି vpiConstantSelect:
	    ଫେରନ୍ତୁ 1;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ 0;
      }
}

ଅକ୍ଷର* __vpiArrayVthrAPV::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ଯଦି (code == vpiFile) {  // Not implemented for now!
            ଫେରନ୍ତୁ simple_set_rbuf_str(file_names[0]);
      }

      ଅକ୍ଷର sidx [64];
      snprintf(sidx, 63, "%u", word_sel + array->first_addr.get_ମୂଲ୍ୟ());
      ଫେରନ୍ତୁ generic_get_str(code, array->get_scope(), array->name, sidx);
}

ଶୂନ୍ୟ __vpiArrayVthrAPV::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ vp)
{
      ପ୍ରତିଜ୍ଞା(array);

      unsigned index = word_sel;
      ଯଦି (vpi_array_is_real(array)) {
	    ଦ୍ୱିଗୁଣ tmp = array->get_word_r(index);
	    vpip_real_get_ମୂଲ୍ୟ(tmp, vp);
      } ଅନ୍ୟଥା {
	    vvp_vector4_t tmp = array->get_word(index);
	    tmp = tmp.subvalue(part_bit, part_wid);
	    vpip_vec4_get_ମୂଲ୍ୟ(tmp, part_wid, ମିଥ୍ୟା, vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  ସ୍ଥିତି vpiParent:
	    ଫେରନ୍ତୁ 0;

            // Not part of the Verilog standard. We use this internally.
	  ସ୍ଥିତି vpiArray:
	    ଫେରନ୍ତୁ array;

	  ସ୍ଥିତି vpiScope:
	    ଫେରନ୍ତୁ array->get_scope();

	  ସ୍ଥିତି vpiModule:
	    ଫେରନ୍ତୁ vpip_module(array->get_scope());
      }

      ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ __vpiArray::set_word(unsigned address, unsigned part_off, ଅଚଳ vvp_vector4_t&val)
{
      ଯଦି (address >= get_size())
	    ଫେରନ୍ତୁ;

      ଯଦି (vals4) {
	    ପ୍ରତିଜ୍ଞା(nets == 0);
	    ଯଦି (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp = vals4->get_word(address);
		  ଯଦି ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			ପ୍ରତିଜ୍ଞା(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals4->set_word(address, tmp);
	    } ଅନ୍ୟଥା {
		  vals4->set_word(address, val);
	    }
	    word_change(address);
	    ଫେରନ୍ତୁ;
      }

      ଯଦି (vals) {
	    ପ୍ରତିଜ୍ଞା(nets == 0);
	    ଯଦି (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp;
		  vals->get_word(address, tmp);
		  ଯଦି ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			ପ୍ରତିଜ୍ଞା(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals->set_word(address, tmp);
	    } ଅନ୍ୟଥା {
		  vals->set_word(address, val);
	    }
	    word_change(address);
	    ଫେରନ୍ତୁ;
      }

      ପ୍ରତିଜ୍ଞା(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle word = nets[address];
      ଅଚଳ ସଂରଚନା __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      ପ୍ରତିଜ୍ଞା(vsig);

      vsig->ଗଣ୍ଠି->send_vec4_pv(val, part_off, vpip_size(vsig), 0);
      word_change(address);
}

ଶୂନ୍ୟ __vpiArray::set_word(unsigned address, ଦ୍ୱିଗୁଣ val)
{
      ପ୍ରତିଜ୍ଞା(vals != 0);
      ପ୍ରତିଜ୍ଞା(nets == 0);

      ଯଦି (address >= vals->get_size())
	    ଫେରନ୍ତୁ;

      vals->set_word(address, val);
      word_change(address);
}

ଶୂନ୍ୟ __vpiArray::set_word(unsigned address, ଅଚଳ string&val)
{
      ପ୍ରତିଜ୍ଞା(vals != 0);
      ପ୍ରତିଜ୍ଞା(nets == 0);

      ଯଦି (address >= vals->get_size())
	    ଫେରନ୍ତୁ;

      vals->set_word(address, val);
      word_change(address);
}

ଶୂନ୍ୟ __vpiArray::set_word(unsigned address, ଅଚଳ vvp_object_t&val)
{
      ପ୍ରତିଜ୍ଞା(vals != 0);
      ପ୍ରତିଜ୍ଞା(nets == 0);

      ଯଦି (address >= vals->get_size())
	    ଫେରନ୍ତୁ;

      vals->set_word(address, val);
      word_change(address);
}

vvp_vector4_t __vpiArray::get_word(unsigned address)
{
      ଯଦି (vals4) {
	    ପ୍ରତିଜ୍ଞା(nets == 0);
	    ପ୍ରତିଜ୍ଞା(vals == 0);
	    ଫେରନ୍ତୁ vals4->get_word(address);
      }

      ଯଦି (vals) {
	    ପ୍ରତିଜ୍ଞା(nets == 0);
	    ପ୍ରତିଜ୍ଞା(vals4== 0);
	    ଯଦି (address >= vals->get_size())
		  ଫେରନ୍ତୁ vvp_vector4_t(vals_width, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_word(address, val);
	    ଫେରନ୍ତୁ val;
      }

      ପ୍ରତିଜ୍ଞା(vals4 == 0);
      ପ୍ରତିଜ୍ଞା(vals == 0);
      ପ୍ରତିଜ୍ଞା(nets != 0);

      ଯଦି (address >= get_size()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    ପ୍ରତିଜ୍ଞା(get_size() > 0);
	    vpiHandle word = nets[0];
	    ପ୍ରତିଜ୍ଞା(word);
	    ସଂରଚନା __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
	    ପ୍ରତିଜ୍ଞା(vsig);
	    vvp_signal_ମୂଲ୍ୟ*sig = dynamic_cast<vvp_signal_ମୂଲ୍ୟ*> (vsig->ଗଣ୍ଠି->fil);
	    ପ୍ରତିଜ୍ଞା(sig);
	    ଫେରନ୍ତୁ vvp_vector4_t(sig->ମୂଲ୍ୟ_size(), BIT4_X);
      }

      vpiHandle word = nets[address];
      ସଂରଚନା __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      ପ୍ରତିଜ୍ଞା(vsig);
      vvp_signal_ମୂଲ୍ୟ*sig = dynamic_cast<vvp_signal_ମୂଲ୍ୟ*> (vsig->ଗଣ୍ଠି->fil);
      ପ୍ରତିଜ୍ଞା(sig);

      vvp_vector4_t val;
      sig->vec4_ମୂଲ୍ୟ(val);
      ଫେରନ୍ତୁ val;
}

ଦ୍ୱିଗୁଣ __vpiArray::get_word_r(unsigned address)
{
      ଯଦି (vals) {
	    ପ୍ରତିଜ୍ଞା(vals4 == 0);
	    ପ୍ରତିଜ୍ଞା(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    ଯଦି (address >= vals->get_size())
		  ଫେରନ୍ତୁ 0.0;

	    ଦ୍ୱିଗୁଣ val;
	    vals->get_word(address, val);
	    ଫେରନ୍ତୁ val;
      }

      ଯଦି (address >= get_size())
	    ଫେରନ୍ତୁ 0.0;

      ପ୍ରତିଜ୍ଞା(nets);
      vpiHandle word = nets[address];
      ସଂରଚନା __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(word);
      ପ୍ରତିଜ୍ଞା(vsig);
      vvp_signal_ମୂଲ୍ୟ*sig = dynamic_cast<vvp_signal_ମୂଲ୍ୟ*> (vsig->net->fil);
      ପ୍ରତିଜ୍ଞା(sig);

      ଦ୍ୱିଗୁଣ val = sig->real_ମୂଲ୍ୟ();
      ଫେରନ୍ତୁ val;
}

ଶୂନ୍ୟ __vpiArray::get_word_obj(unsigned address, vvp_object_t&val)
{
      ଯଦି (vals) {
	    ପ୍ରତିଜ୍ଞା(vals4 == 0);
	    ପ୍ରତିଜ୍ଞା(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    ଯଦି (address >= vals->get_size()) {
		  val = vvp_object_t();
		  ଫେରନ୍ତୁ;
	    }

	    vals->get_word(address, val);
	    ଫେରନ୍ତୁ;
      }

      ପ୍ରତିଜ୍ଞା(nets);
	// Arrays of string nets not implemented!
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ;
}

string __vpiArray::get_word_str(unsigned address)
{
      ଯଦି (vals) {
	    ପ୍ରତିଜ୍ଞା(vals4 == 0);
	    ପ୍ରତିଜ୍ଞା(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    ଯଦି (address >= vals->get_size())
		  ଫେରନ୍ତୁ "";

	    string val;
	    vals->get_word(address, val);
	    ଫେରନ୍ତୁ val;
      }

      ପ୍ରତିଜ୍ଞା(nets);
	// Arrays of string nets not implemented!
      ପ୍ରତିଜ୍ଞା(0);
      ଫେରନ୍ତୁ "";
}

vpiHandle vpip_make_array(ଅଚଳ ଅକ୍ଷର*label, ଅଚଳ ଅକ୍ଷର*name,
                          ପୂର୍ଣ୍ଣସଂଖ୍ୟା first_addr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_addr,
                          ତାର୍କିକ signed_flag)
{
      ସଂରଚନା __vpiArray*obj = ନୂତନ __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      ଯଦି (last_addr >= first_addr) {
	    obj->swap_addr = ମିଥ୍ୟା;
      } ଅନ୍ୟଥା {
	    obj->swap_addr = ସତ୍ୟ;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tmp = last_addr;
	    last_addr = first_addr;
	    first_addr = tmp;
      }
      ପ୍ରତିଜ୍ଞା(last_addr >= first_addr);
      unsigned array_ଗଣନା = last_addr+1-first_addr;

	// For now, treat all arrays as memories. This is not quite
	// correct, as arrays are arrays with memories a special case.
      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_ଗଣନା = array_ଗଣନା;

      obj->first_addr.set_ମୂଲ୍ୟ(first_addr);
      obj->last_addr .set_ମୂଲ୍ୟ(last_addr);

	// Start off now knowing if we are nets or variables.
      obj->nets = 0;
      obj->vals4 = 0;
      obj->vals  = 0;
      obj->vals_width = 0;
      obj->vals_ଶବ୍ଦଗୁଡ଼ିକ = 0;

	// Initialize (clear) the read-ports list.
      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

	/* Add this symbol to the array_symbols table for later lookup. */
      ଯଦି (!array_table)
	    array_table = ନୂତନ ପ୍ରତୀକ_map_s<ସଂରଚନା __vpiArray>;

      ପ୍ରତିଜ୍ଞା(!array_find(label));
      array_table->sym_set_ମୂଲ୍ୟ(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_ପ୍ରତୀକ(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_to_current_scope(obj);

      ଫେରନ୍ତୁ obj;
}

ଶୂନ୍ୟ __vpiArray::alias_word(unsigned long addr, vpiHandle word, ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb_, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb_)
{
      ପ୍ରତିଜ୍ଞା(msb.get_ମୂଲ୍ୟ() == msb_);
      ପ୍ରତିଜ୍ଞା(lsb.get_ମୂଲ୍ୟ() == lsb_);
      ପ୍ରତିଜ୍ଞା(addr < get_size());
      ପ୍ରତିଜ୍ଞା(nets);
      nets[addr] = word;
}

ଶୂନ୍ୟ __vpiArray::attach_word(unsigned addr, vpiHandle word)
{
      ପ୍ରତିଜ୍ଞା(addr < get_size());
      ପ୍ରତିଜ୍ଞା(nets);
      nets[addr] = word;

      ଯଦି (ସଂରଚନା __vpiSignal*sig = dynamic_cast<__vpiSignal*>(word)) {
	    vvp_net_t*net = sig->ଗଣ୍ଠି;
	    ପ୍ରତିଜ୍ଞା(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    ପ୍ରତିଜ୍ଞା(fun);
	    fun->attach_as_word(ଏହି, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = ଏହି;
	    sig->id.index = ନୂତନ __vpiDecConst(addr + first_addr.get_ମୂଲ୍ୟ());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    ଫେରନ୍ତୁ;
      }

      ଯଦି (ସଂରଚନା __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(word)) {
	    vvp_net_t*net = sig->net;
	    ପ୍ରତିଜ୍ଞା(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    ପ୍ରତିଜ୍ଞା(fun);
	    fun->attach_as_word(ଏହି, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = ଏହି;
	    sig->id.index = ନୂତନ __vpiDecConst(addr + first_addr.get_ମୂଲ୍ୟ());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = ସତ୍ୟ;
	    ଫେରନ୍ତୁ;
      }
}

ଶୂନ୍ୟ compile_var_array(ଅକ୍ଷର*label, ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା last, ପୂର୍ଣ୍ଣସଂଖ୍ୟା first,
		   ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb, ଅକ୍ଷର signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last,
                                      signed_flag != 0);

      ସଂରଚନା __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_width = labs(msb-lsb) + 1;
      ଯଦି (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = ନୂତନ vvp_vector4array_aa(arr->vals_width,
						 arr->get_size());
      } ଅନ୍ୟଥା {
            arr->vals4 = ନୂତନ vvp_vector4array_sa(arr->vals_width,
						 arr->get_size());
      }
      arr->msb.set_ମୂଲ୍ୟ(msb);
      arr->lsb.set_ମୂଲ୍ୟ(lsb);

      ଗଣନା_var_arrays += 1;
      ଗଣନା_var_array_ଶବ୍ଦଗୁଡ଼ିକ += arr->get_size();

      ମୁକ୍ତ(label);
      delete[] name;
}

ଶୂନ୍ୟ compile_var2_array(ଅକ୍ଷର*label, ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା last, ପୂର୍ଣ୍ଣସଂଖ୍ୟା first,
		   ପୂର୍ଣ୍ଣସଂଖ୍ୟା msb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsb, ତାର୍କିକ signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, signed_flag);

      ସଂରଚନା __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.set_ମୂଲ୍ୟ(msb);
      arr->lsb.set_ମୂଲ୍ୟ(lsb);
      arr->vals_width = labs(msb-lsb) + 1;

      ପ୍ରତିଜ୍ଞା(! arr->nets);
      ଯଦି (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = ନୂତନ vvp_darray_atom<int8_t>(arr->get_size());
      } ଅନ୍ୟଥା ଯଦି (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = ନୂତନ vvp_darray_atom<uint8_t>(arr->get_size());
      } ଅନ୍ୟଥା ଯଦି (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = ନୂତନ vvp_darray_atom<int16_t>(arr->get_size());
      } ଅନ୍ୟଥା ଯଦି (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = ନୂତନ vvp_darray_atom<uint16_t>(arr->get_size());
      } ଅନ୍ୟଥା ଯଦି (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = ନୂତନ vvp_darray_atom<int32_t>(arr->get_size());
      } ଅନ୍ୟଥା ଯଦି (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = ନୂତନ vvp_darray_atom<uint32_t>(arr->get_size());
      } ଅନ୍ୟଥା ଯଦି (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = ନୂତନ vvp_darray_atom<int64_t>(arr->get_size());
      } ଅନ୍ୟଥା ଯଦି (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = ନୂତନ vvp_darray_atom<uint64_t>(arr->get_size());
      } ଅନ୍ୟଥା {
	    arr->vals = ନୂତନ vvp_darray_vec2(arr->get_size(), arr->vals_width);
      }
      ଗଣନା_var_arrays += 1;
      ଗଣନା_var_array_ଶବ୍ଦଗୁଡ଼ିକ += arr->get_size();

      ମୁକ୍ତ(label);
      delete[] name;
}

ଶୂନ୍ୟ compile_real_array(ଅକ୍ଷର*label, ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା last, ପୂର୍ଣ୍ଣସଂଖ୍ୟା first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, ସତ୍ୟ);

      ସଂରଚନା __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = ନୂତନ vvp_darray_real(arr->get_size());
      arr->vals_width = 1;

      ଗଣନା_real_arrays += 1;
      ଗଣନା_real_array_ଶବ୍ଦଗୁଡ଼ିକ += arr->get_size();

      ମୁକ୍ତ(label);
      delete[] name;
}

ଶୂନ୍ୟ compile_string_array(ଅକ୍ଷର*label, ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା last, ପୂର୍ଣ୍ଣସଂଖ୍ୟା first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, ସତ୍ୟ);

      ସଂରଚନା __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = ନୂତନ vvp_darray_string(arr->get_size());
      arr->vals_width = 1;

      ଗଣନା_real_arrays += 1;
      ଗଣନା_real_array_ଶବ୍ଦଗୁଡ଼ିକ += arr->get_size();

      ମୁକ୍ତ(label);
      delete[] name;
}

ଶୂନ୍ୟ compile_object_array(ଅକ୍ଷର*label, ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା last, ପୂର୍ଣ୍ଣସଂଖ୍ୟା first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, ସତ୍ୟ);

      ସଂରଚନା __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = ନୂତନ vvp_darray_object(arr->get_size());
      arr->vals_width = 1;

      ଗଣନା_real_arrays += 1;
      ଗଣନା_real_array_ଶବ୍ଦଗୁଡ଼ିକ += arr->get_size();

      ମୁକ୍ତ(label);
      delete[] name;
}

ଶୂନ୍ୟ compile_net_array(ଅକ୍ଷର*label, ଅକ୍ଷର*name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା last, ପୂର୍ଣ୍ଣସଂଖ୍ୟା first)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, first, last, ମିଥ୍ୟା);

      ସଂରଚନା __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(calloc(arr->get_size(), ଆକାର(vpiHandle)));

      ଗଣନା_net_arrays += 1;
      ଗଣନା_net_array_ଶବ୍ଦଗୁଡ଼ିକ += arr->get_size();

      ମୁକ୍ତ(label);
      delete[] name;
}

ଶ୍ରେଣୀ vvp_fun_arrayport  : ସର୍ବସାଧାରଣ vvp_net_fun_t {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport() override;

      ଆଭାସୀ ଶୂନ୍ୟ check_word_change(unsigned long addr) = 0;

    ସୁରକ୍ଷିତ:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      unsigned long addr_;

      friend ଶୂନ୍ୟ array_attach_port(vvp_array_t, vvp_fun_arrayport*);
      friend ଶୂନ୍ୟ __vpiArray::word_change(unsigned long);
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

ଶ୍ରେଣୀ vvp_fun_arrayport_sa  : ସର୍ବସାଧାରଣ vvp_fun_arrayport {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_sa() override;

      ଶୂନ୍ୟ check_word_change(unsigned long addr) override;

      ଶୂନ୍ୟ recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                     vvp_context_t) override;

    ବ୍ୟକ୍ତିଗତ:
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

ଶୂନ୍ୟ vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                                     vvp_context_t)
{
      ତାର୍କିକ addr_valid_flag;

      ବାଛ (port.port()) {

	  ସ୍ଥିତି 0: // Address input
	    addr_valid_flag = vector4_to_ମୂଲ୍ୟ(bit, addr_);
	    ଯଦି (! addr_valid_flag)
		  addr_ = arr_->get_size();
	    ଯଦି (vpi_array_is_real(arr_))
		  port.ptr()->send_real(arr_->get_word_r(addr_), 0);
	    ଅନ୍ୟଥା
		  port.ptr()->send_vec4(arr_->get_word(addr_), 0);

	    ଭାଙ୍ଗ;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stdout, "XXXX write ports not implemented.\n");
	    ପ୍ରତିଜ୍ଞା(0);
      }
}

ଶୂନ୍ୟ vvp_fun_arrayport_sa::check_word_change(unsigned long addr)
{
      ଯଦି (addr != addr_) ଫେରନ୍ତୁ;

      ଯଦି (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr_), 0);
      } ଅନ୍ୟଥା {
	    net_->send_vec4(arr_->get_word(addr_), 0);
      }
}

ଶ୍ରେଣୀ vvp_fun_arrayport_aa  : ସର୍ବସାଧାରଣ vvp_fun_arrayport, ସର୍ବସାଧାରଣ automatic_hooks_s {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_aa() override;

      ଶୂନ୍ୟ alloc_instance(vvp_context_t context) override;
      ଶୂନ୍ୟ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ଶୂନ୍ୟ free_instance(vvp_context_t context) override;
#endif

      ଶୂନ୍ୟ check_word_change(unsigned long addr) override;

      ଶୂନ୍ୟ recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                     vvp_context_t context) override;

    ବ୍ୟକ୍ତିଗତ:
      ଶୂନ୍ୟ check_word_change_(unsigned long addr, vvp_context_t context);

      __vpiScope*context_scope_;
      unsigned context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(ଏହି, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(ଏହି, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

ଶୂନ୍ୟ vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      unsigned long*addr = ନୂତନ unsigned long;
      vvp_set_context_item(context, context_idx_, addr);

      *addr = addr_;
}

ଶୂନ୍ୟ vvp_fun_arrayport_aa::reset_instance(vvp_context_t context)
{
      unsigned long*addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));

      *addr = addr_;
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ vvp_fun_arrayport_aa::free_instance(vvp_context_t context)
{
      unsigned long*addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));
      delete addr;
}
#endif

ଶୂନ୍ୟ vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                                     vvp_context_t context)
{
      ଯଦି (context) {
            unsigned long*addr = static_cast<unsigned long*>
                  (vvp_get_context_item(context, context_idx_));

            ତାର୍କିକ addr_valid_flag;

            ବାଛ (port.port()) {

                ସ୍ଥିତି 0: // Address input
                  addr_valid_flag = vector4_to_ମୂଲ୍ୟ(bit, *addr);
                  ଯଦି (! addr_valid_flag) *addr = arr_->get_size();
                  ଯଦି (vpi_array_is_real(arr_)) {
			port.ptr()->send_real(arr_->get_word_r(*addr),
					      context);
                  } ଅନ୍ୟଥା {
			port.ptr()->send_vec4(arr_->get_word(*addr),
					      context);
                  }
                  ଭାଙ୍ଗ;

                ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                  fprintf(stdout, "XXXX write ports not implemented.\n");
                  ପ୍ରତିଜ୍ଞା(0);
            }
      } ଅନ୍ୟଥା {
            context = context_scope_->live_contexts;
            ଯେତେବେଳେ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

ଶୂନ୍ୟ vvp_fun_arrayport_aa::check_word_change_(unsigned long addr,
                                              vvp_context_t context)
{
      ଅଚଳ unsigned long*port_addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));

      ଯଦି (addr != *port_addr)
	    ଫେରନ୍ତୁ;

      ଯଦି (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr), context);
      } ଅନ୍ୟଥା {
	    net_->send_vec4(arr_->get_word(addr), context);
      }
}

ଶୂନ୍ୟ vvp_fun_arrayport_aa::check_word_change(unsigned long addr)
{
      ଯଦି (arr_->get_scope()->is_automatic()) {
            ପ୍ରତିଜ୍ଞା(vthread_get_wt_context());
            check_word_change_(addr, vthread_get_wt_context());
      } ଅନ୍ୟଥା {
            vvp_context_t context = context_scope_->live_contexts;
            ଯେତେବେଳେ (context) {
                  check_word_change_(addr, context);
                  context = vvp_get_next_context(context);
            }
      }
}

ସ୍ଥିର ଶୂନ୍ୟ array_attach_port(vvp_array_t array, vvp_fun_arrayport*fun)
{
      ପ୍ରତିଜ୍ଞା(fun->next_ == 0);
      fun->next_ = array->ports_;
      array->ports_ = fun;
      ଯଦି (!array->get_scope()->is_automatic() &&
          (array->vals4 || array->vals)) {
              /* propagate initial values for variable arrays */
            ଯଦି (!vpi_array_is_real(array)) {
                  vvp_bit4_t init;
                  ଯଦି (array->vals4)
                      init = BIT4_X;
                  ଅନ୍ୟଥା
                      init = BIT4_0;
                  vvp_vector4_t tmp(array->vals_width, init);
                  schedule_init_propagate(fun->net_, tmp);
            } ଅନ୍ୟଥା {
                  schedule_init_propagate(fun->net_, 0.0);
            }
      }
}

ଶ୍ରେଣୀ array_word_ମୂଲ୍ୟ_callback : ସର୍ବସାଧାରଣ ମୂଲ୍ୟ_callback {
    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ explicit array_word_ମୂଲ୍ୟ_callback(p_cb_data data, long addr)
      : ମୂଲ୍ୟ_callback(data), word_addr(addr)
      { }

    ସର୍ବସାଧାରଣ:
      long word_addr;
};

ଶୂନ୍ୟ __vpiArray::word_change(unsigned long addr)
{
      ପାଇଁ (vvp_fun_arrayport*cur = ports_; cur; cur = cur->next_)
	    cur->check_word_change(addr);

	// Run callbacks attached to the array itself.
      ସଂରଚନା __vpiCallback *next = vpi_callbacks;
      ସଂରଚନା __vpiCallback *prev = 0;

      ଯେତେବେଳେ (next) {
	    array_word_ମୂଲ୍ୟ_callback*cur = dynamic_cast<array_word_ମୂଲ୍ୟ_callback*>(next);
	    next = cur->next;

	      // Skip callbacks that are not for me. -1 is for every element.
	    ଯଦି (cur->word_addr != (long)addr && cur->word_addr != -1) {
		  prev = cur;
		  ଜାରି;
	    }

	      // For whole array callbacks we need to set the index.
	    ଯଦି (cur->word_addr == -1) {
		  cur->cb_data.index = (PLI_INT32) ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)addr +
						    first_addr.get_ମୂଲ୍ୟ());
	    }

	    ଯଦି (cur->cb_data.cb_rtn != 0) {
		  ଯଦି (cur->test_ମୂଲ୍ୟ_callback_ready()) {
			ଯଦି (cur->cb_data.value) {
			      ଯଦି (vpi_array_is_real(ଏହି)) {
				    ଦ୍ୱିଗୁଣ val = 0.0;
				    ଯଦି (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_real_get_ମୂଲ୍ୟ(val, cur->cb_data.value);
			      } ଅନ୍ୟଥା ଯଦି (vals4) {
				    vpip_vec4_get_ମୂଲ୍ୟ(vals4->get_word(addr),
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } ଅନ୍ୟଥା ଯଦି (dynamic_cast<vvp_darray_atom<int8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int64_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint64_t>*>(vals)
				      || dynamic_cast<vvp_darray_vec2*>(vals)) {
				    vvp_vector4_t val;
				    ଯଦି (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_vec4_get_ମୂଲ୍ୟ(val,
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } ଅନ୍ୟଥା {
			            ପ୍ରତିଜ୍ଞା(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } ଅନ୍ୟଥା ଯଦି (prev == 0) {

		  vpi_callbacks = next;
		  cur->next = 0;
		  delete cur;

	    } ଅନ୍ୟଥା {
		  ପ୍ରତିଜ୍ଞା(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}

ଶ୍ରେଣୀ array_resolv_list_t : ସର୍ବସାଧାରଣ resolv_list_s {

    ସର୍ବସାଧାରଣ:
      explicit array_resolv_list_t(ଅକ୍ଷର*lab) : resolv_list_s(lab) {
	    array = 0;
      }

      vvp_array_t*array;
      ତାର୍କିକ resolve(ତାର୍କିକ mes) override;

    ବ୍ୟକ୍ତିଗତ:
};

ତାର୍କିକ array_resolv_list_t::resolve(ତାର୍କିକ mes)
{
      *array = array_find(label());
      ଯଦି (*array == 0) {
	    ପ୍ରତିଜ୍ଞା(!mes);
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ଶ୍ରେଣୀ array_port_resolv_list_t : ସର୍ବସାଧାରଣ resolv_list_s {

    ସର୍ବସାଧାରଣ:
      explicit array_port_resolv_list_t(ଅକ୍ଷର* lab, ତାର୍କିକ use_addr__,
                                        long addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      ତାର୍କିକ use_addr;
      long addr;
      ତାର୍କିକ resolve(ତାର୍କିକ mes) override;

    ବ୍ୟକ୍ତିଗତ:
};

array_port_resolv_list_t::array_port_resolv_list_t(ଅକ୍ଷର *lab, ତାର୍କିକ use_addr__,
                                                   long addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      ଯଦି (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      ଅନ୍ୟଥା
            context_scope = 0;
      ptr = ନୂତନ vvp_net_t;
}

ତାର୍କିକ array_port_resolv_list_t::resolve(ତାର୍କିକ mes)
{
      vvp_array_t mem = array_find(label());
      ଯଦି (mem == 0) {
	    ପ୍ରତିଜ୍ଞା(mem || !mes);
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      }

      vvp_fun_arrayport*fun;
      ଯଦି (use_addr)
            ଯଦି (context_scope)
                  fun = ନୂତନ vvp_fun_arrayport_aa(context_scope, mem, ptr, addr);
            ଅନ୍ୟଥା
                  fun = ନୂତନ vvp_fun_arrayport_sa(mem, ptr, addr);
      ଅନ୍ୟଥା
            ଯଦି (context_scope)
                  fun = ନୂତନ vvp_fun_arrayport_aa(context_scope, mem, ptr);
            ଅନ୍ୟଥା
                  fun = ନୂତନ vvp_fun_arrayport_sa(mem, ptr);
      ptr->fun = fun;

      array_attach_port(mem, fun);

      ଫେରନ୍ତୁ ସତ୍ୟ;
}

ଶ୍ରେଣୀ array_word_part_callback : ସର୍ବସାଧାରଣ array_word_ମୂଲ୍ୟ_callback {
    ସର୍ବସାଧାରଣ:
      explicit array_word_part_callback(p_cb_data data, long addr);
      ~array_word_part_callback() override;

      array_word_part_callback(ଅଚଳ array_word_part_callback&) = delete;
      array_word_part_callback& operator=(ଅଚଳ array_word_part_callback&) = delete;

      ତାର୍କିକ test_ମୂଲ୍ୟ_callback_ready(ଶୂନ୍ୟ) override;

    ବ୍ୟକ୍ତିଗତ:
      ଅକ୍ଷର*ମୂଲ୍ୟ_bits;
};

array_word_part_callback::array_word_part_callback(p_cb_data data, long addr)
: array_word_ମୂଲ୍ୟ_callback(data, addr)
{
	// Get the initial value of the part, to use as a reference.
      ସଂରଚନା __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj);
      s_vpi_ମୂଲ୍ୟ tmp_ମୂଲ୍ୟ;
      tmp_ମୂଲ୍ୟ.format = vpiBinStrVal;
      apvword->vpi_get_ମୂଲ୍ୟ(&tmp_ମୂଲ୍ୟ);

      ମୂଲ୍ୟ_bits = ନୂତନ ଅକ୍ଷର[apvword->part_wid+1];

      memcpy(ମୂଲ୍ୟ_bits, tmp_ମୂଲ୍ୟ.value.str, apvword->part_wid);
      ମୂଲ୍ୟ_bits[apvword->part_wid] = 0;
}

array_word_part_callback::~array_word_part_callback()
{
      delete[]ମୂଲ୍ୟ_bits;
}

ତାର୍କିକ array_word_part_callback::test_ମୂଲ୍ୟ_callback_ready(ଶୂନ୍ୟ)
{
      ସଂରଚନା __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_data.obj);
      ପ୍ରତିଜ୍ଞା(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_ମୂଲ୍ୟ tmp_ମୂଲ୍ୟ;
      tmp_ମୂଲ୍ୟ.format = vpiBinStrVal;
      apvword->vpi_get_ମୂଲ୍ୟ(&tmp_ମୂଲ୍ୟ);

      ଯଦି (memcmp(ମୂଲ୍ୟ_bits, tmp_ମୂଲ୍ୟ.value.str, apvword->part_wid) == 0)
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;

      memcpy(ମୂଲ୍ୟ_bits, tmp_ମୂଲ୍ୟ.value.str, apvword->part_wid);
      ଫେରନ୍ତୁ ସତ୍ୟ;

}

ମୂଲ୍ୟ_callback*vpip_array_word_change(p_cb_data data)
{
      ସଂରଚନା __vpiArray*parent = 0;
      array_word_ମୂଲ୍ୟ_callback*cbh = 0;
      ଯଦି (ଅଚଳ ସଂରଚନା __vpiArrayWord*word = array_var_word_from_handle(data->obj)) {
	    parent = static_cast<__vpiArray*>(word->get_parent());
	    unsigned addr = word->get_index();
	    cbh = ନୂତନ array_word_ମୂଲ୍ୟ_callback(data, addr);

      } ଅନ୍ୟଥା ଯଦି (ସଂରଚନା __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(data->obj)) {
	    parent = tword->array;
	    cbh = ନୂତନ array_word_ମୂଲ୍ୟ_callback(data, tword->address);

      } ଅନ୍ୟଥା ଯଦି (ସଂରଚନା __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj)) {
	    parent = apvword->array;
	    cbh = ନୂତନ array_word_part_callback(data, apvword->word_sel);
      }

      ପ୍ରତିଜ୍ଞା(cbh);
      ପ୍ରତିଜ୍ଞା(parent);
      cbh->next = parent->vpi_callbacks;
      parent->vpi_callbacks = cbh;

      ଫେରନ୍ତୁ cbh;
}

ମୂଲ୍ୟ_callback* vpip_array_change(p_cb_data data)
{
      array_word_ମୂଲ୍ୟ_callback*cbh = ନୂତନ array_word_ମୂଲ୍ୟ_callback(data, -1);
      ପ୍ରତିଜ୍ଞା(data->obj);

      ସଂରଚନା __vpiArray*arr = dynamic_cast<__vpiArray*>(data->obj);
      cbh->next = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      ଫେରନ୍ତୁ cbh;
}

ଶୂନ୍ୟ compile_array_port(ଅକ୍ଷର*label, ଅକ୍ଷର*array, ଅକ୍ଷର*addr)
{
      array_port_resolv_list_t*resolv_mem
	    = ନୂତନ array_port_resolv_list_t(array, ମିଥ୍ୟା, 0);

      define_functor_ପ୍ରତୀକ(label, resolv_mem->ptr);
      ମୁକ୍ତ(label);
	// Connect the port-0 input as the address.
      ନିବେଶ_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

ଶୂନ୍ୟ compile_array_port(ଅକ୍ଷର*label, ଅକ୍ଷର*array, long addr)
{
      array_port_resolv_list_t*resolv_mem
	    = ନୂତନ array_port_resolv_list_t(array, ସତ୍ୟ, addr);

      define_functor_ପ୍ରତୀକ(label, resolv_mem->ptr);
      ମୁକ୍ତ(label);

      resolv_submit(resolv_mem);
}

ଶୂନ୍ୟ compile_array_alias(ଅକ୍ଷର*label, ଅକ୍ଷର*name, ଅକ୍ଷର*src)
{
      vvp_array_t mem = array_find(src);
      ପ୍ରତିଜ୍ଞା(mem);

      ସଂରଚନା __vpiArray*obj = ନୂତନ __vpiArray;

      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_ଗଣନା = mem->array_ଗଣନା;
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
      obj->vals_ଶବ୍ଦଗୁଡ଼ିକ = mem->vals_ଶବ୍ଦଗୁଡ଼ିକ;

      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

      ପ୍ରତିଜ୍ଞା(array_table);
      ପ୍ରତିଜ୍ଞା(!array_find(label));
      array_table->sym_set_ମୂଲ୍ୟ(label, obj);

      compile_vpi_ପ୍ରତୀକ(label, obj);
      vpip_attach_to_current_scope(obj);

      ମୁକ୍ତ(label);
      ମୁକ୍ତ(name);
      ମୁକ୍ତ(src);
}

/*
 * &A<label,addr>
 * This represents a VPI handle for an addressed array. This comes
 * from expressions like "label[addr]" where "label" is the array and
 * "addr" is the canonical address of the desired word.
 */
vpiHandle vpip_make_vthr_A(ଅକ୍ଷର*label, unsigned addr)
{
      ସଂରଚନା __vpiArrayVthrA*obj = ନୂତନ __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = ନୂତନ array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      obj->address = addr;

      ଫେରନ୍ତୁ obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(ଅକ୍ଷର*label, ଅକ୍ଷର*ପ୍ରତୀକ)
{
      ସଂରଚନା __vpiArrayVthrA*obj = ନୂତନ __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = ନୂତନ array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      compile_vpi_lookup(&obj->address_handle, ପ୍ରତୀକ);
      obj->address = 0;

      ଫେରନ୍ତୁ obj;
}

vpiHandle vpip_make_vthr_A(ଅକ୍ଷର*label, vpiHandle handle)
{
      ସଂରଚନା __vpiArrayVthrA*obj = ନୂତନ __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = ନୂତନ array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = handle;
      obj->address = 0;

      ଫେରନ୍ତୁ obj;
}

vpiHandle vpip_make_vthr_APV(ଅକ୍ଷର*label, unsigned index, unsigned bit, unsigned wid)
{
      ସଂରଚନା __vpiArrayVthrAPV*obj = ନୂତନ __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = ନୂତନ array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->word_sel = index;
      obj->part_bit = bit;
      obj->part_wid = wid;

      ଫେରନ୍ତୁ obj;
}

ଶୂନ୍ୟ compile_array_cleanup(ଶୂନ୍ୟ)
{
      delete array_table;
      array_table = 0;
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ memory_delete(vpiHandle item)
{
      ସଂରଚନା __vpiArray*arr = static_cast<ସଂରଚନା __vpiArray*>(item);
      ଯଦି (arr->vals_ଶବ୍ଦଗୁଡ଼ିକ) delete [] (arr->vals_ଶବ୍ଦଗୁଡ଼ିକ-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals;

      ଯଦି (arr->nets) {
	    ପାଇଁ (unsigned idx = 0; idx < arr->get_size(); idx += 1) {
		  ଯଦି (ସଂରଚନା __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.index);
		    /* These should only be the real words. */
		  } ଅନ୍ୟଥା {
			ପ୍ରତିଜ୍ଞା(arr->nets[idx]->get_type_code() ==
			       vpiRealVar);
			ସଂରଚନା __vpiRealVar *sigr = static_cast<ସଂରଚନା __vpiRealVar *>
			                            (arr->nets[idx]);
			constant_delete(sigr->id.index);
// Why are only the real words still here?
			delete arr->nets[idx];
		  }
	    }
	    ମୁକ୍ତ(arr->nets);
      }

      ଯେତେବେଳେ (arr->vpi_callbacks) {
	    ସଂରଚନା __vpiCallback*tmp = arr->vpi_callbacks->next;
	    delete arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      delete arr;
}

ଶୂନ୍ୟ A_delete(vpiHandle item)
{
      ସଂରଚନା __vpiArrayVthrA*obj = static_cast<ସଂରଚନା __vpiArrayVthrA*>(item);
      ଯଦି (obj->address_handle) {
	    ବାଛ (obj->address_handle->get_type_code()) {
		ସ୍ଥିତି vpiMemoryWord:
		  ଯଦି (vpi_get(_vpiFromThr, obj->address_handle) == _vpi_at_A) {
			A_delete(obj->address_handle);
		  }
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି vpiPartSelect:
		  ପ୍ରତିଜ୍ଞା(vpi_get(_vpiFromThr, obj->address_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->address_handle);
		  ଭାଙ୍ଗ;
	    }
      }

      delete obj;
}

ଶୂନ୍ୟ APV_delete(vpiHandle item)
{
      ସଂରଚନା __vpiArrayVthrAPV*obj = static_cast<ସଂରଚନା __vpiArrayVthrAPV*>(item);
      delete obj;
}
#endif
