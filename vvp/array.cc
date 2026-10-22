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
#அந்தர்பாவ  "vvp_cleanup.h"
#endif
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <iostream>
# include  "compile.h"
# include  <cassert>
# include  "ivl_alloc.h"

using நாமாகாஶ std;

unsigned long மொத்தம்_net_arrays = 0;
unsigned long மொத்தம்_net_array_சொற்கள் = 0;
unsigned long மொத்தம்_var_arrays = 0;
unsigned long மொத்தம்_var_array_சொற்கள் = 0;
unsigned long மொத்தம்_real_arrays = 0;
unsigned long மொத்தம்_real_array_சொற்கள் = 0;

நிலையான குறியீடு_map_s<கட்டமைப்பு __vpiArray>* array_table =0;

வகுப்பு vvp_fun_arrayport;
நிலையான வெறுமை array_attach_port(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_find(மாறிலி எழுத்து*label)
{
      எனில் (array_table == 0)
	    திரும்பு 0;

      vvp_array_t v = array_table->sym_get_மதிப்பு(label);
      திரும்பு v;
}

கட்டமைப்பு __vpiArrayVthrA : பொது __vpiHandle {
      முழு get_type_code(வெறுமை) மாறிலி override { திரும்பு vpiMemoryWord; }
      முழு vpi_get(முழு code) override;
      எழுத்து* vpi_get_str(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;
      vpiHandle vpi_put_மதிப்பு(p_vpi_மதிப்பு val, முழு flags) override;
      vpiHandle vpi_handle(முழு code) override;

      கட்டமைப்பு __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle address_handle;
	// If wid==0, then address is the address into the array.
      unsigned address;

      unsigned get_address() மாறிலி
      {
	    எனில் (address_handle) {
		  s_vpi_மதிப்பு vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpiVectorVal;
		  address_handle->vpi_get_மதிப்பு(&vp);
		  முழு சொற்கள் = (address_handle->vpi_get(vpiSize)-1)/32 + 1;
		  க்கு(முழு idx = 0; idx < சொற்கள்; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			எனில் (vp.value.vector[idx].bval != 0) திரும்பு UINT_MAX;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  address_handle->vpi_get_மதிப்பு(&vp);
		  திரும்பு vp.value.integer;
	    }

	    திரும்பு address;
      }
};


கட்டமைப்பு __vpiArrayVthrAPV : பொது __vpiHandle {
      முழு get_type_code(வெறுமை) மாறிலி override { திரும்பு vpiPartSelect; }
      முழு vpi_get(முழு code) override;
      எழுத்து* vpi_get_str(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;
      vpiHandle vpi_handle(முழு code) override;

      கட்டமைப்பு __vpiArray*array;
      unsigned word_sel;
      unsigned part_bit;
      unsigned part_wid;
};

தருக்க is_net_array(vpiHandle obj)
{
      கட்டமைப்பு __vpiArray*rfp = dynamic_cast<__vpiArray*> (obj);
      ப்ரதிஜ்ஞா(rfp);

      எனில் (rfp->nets != 0) திரும்பு உண்மை;
      திரும்பு பொய்;
}

// This function return true if the underlying array words are real.
நிலையான தருக்க vpi_array_is_real(மாறிலி vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      எனில் (arr->vals4 != 0)  // A bit based variable/register array.
	    திரும்பு பொய்;

      எனில் (dynamic_cast<vvp_darray_real*> (arr->vals))
	    திரும்பு உண்மை;

      எனில் (arr->vals != 0)
	    திரும்பு பொய்;

	// This must be a net array so look at element 0 to find the type.
      ப்ரதிஜ்ஞா(arr->nets != 0);
      ப்ரதிஜ்ஞா(arr->get_size() > 0);
      மாறிலி கட்டமைப்பு __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      எனில் (rsig) {
	    திரும்பு உண்மை;
      }

      திரும்பு பொய்;
}

நிலையான தருக்க vpi_array_is_string(மாறிலி vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      எனில் (arr->vals4 != 0)  // A bit based variable/register array.
	    திரும்பு பொய்;

      எனில் (dynamic_cast<vvp_darray_string*> (arr->vals))
	    திரும்பு உண்மை;

      திரும்பு பொய்;
}

முழு __vpiArray::get_word_size() மாறிலி
{
      unsigned width;

      ப்ரதிஜ்ஞா(get_size() > 0);
	/* For a net array we need to get the width from the first element. */
      எனில் (nets) {
	    ப்ரதிஜ்ஞா(vals4 == 0 && vals == 0);
	    மாறிலி கட்டமைப்பு __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    ப்ரதிஜ்ஞா(vsig);
	    width = vpip_size(vsig);
	/* For a variable array we can get the width from vals_width. */
      } இல்லையெனில் {
	    ப்ரதிஜ்ஞா(vals4 || vals);
	    width = vals_width;
      }

      திரும்பு width;
}

எழுத்து*__vpiArray::get_word_str(கட்டமைப்பு __vpiArrayWord*word, முழு code)
{
      unsigned index = word->get_index();

      எனில் (code == vpiFile) {  // Not implemented for now!
	    திரும்பு simple_set_rbuf_str(file_names[0]);
      }

      எழுத்து sidx [64];
      snprintf(sidx, 63, "%d", (முழு)index + first_addr.get_மதிப்பு());
      திரும்பு generic_get_str(code, get_scope(), name, sidx);
}

வெறுமை __vpiArray::get_word_மதிப்பு(கட்டமைப்பு __vpiArrayWord*word, p_vpi_மதிப்பு vp)
{
      unsigned index = word->get_index();

    // Determine the appropriate format (The Verilog PLI Handbook 5.2.10)
      எனில்(vp->format == vpiObjTypeVal) {
          எனில்(vpi_array_is_real(இது))
              vp->format = vpiRealVal;
          இல்லையெனில் எனில்(vpi_array_is_string(இது))
              vp->format = vpiStringVal;
          இல்லையெனில்
              vp->format = vpiIntVal;
      }

      எனில்(vals4) {
          vpip_vec4_get_மதிப்பு(vals4->get_word(index),
                              vals4->width(), signed_flag, vp);
      } இல்லையெனில் எனில்(vals) {
          தேர்வு(vp->format) {
            நிலை vpiBinStrVal:
            நிலை vpiOctStrVal:
            நிலை vpiDecStrVal:
            நிலை vpiHexStrVal:
            நிலை vpiScalarVal:
            நிலை vpiIntVal:
            நிலை vpiVectorVal:
            {
                vvp_vector4_t v;
                vals->get_word(index, v);
                vpip_vec4_get_மதிப்பு(v, vals_width, signed_flag, vp);
            }
            நிறுத்து;

            நிலை vpiRealVal:
            {
                இரட்டை d;
                vals->get_word(index, d);
                vpip_real_get_மதிப்பு(d, vp);
            }
            நிறுத்து;

            நிலை vpiStringVal:
            {
                string s;
                vals->get_word(index, s);
                vpip_string_get_மதிப்பு(s, vp);
            }
            நிறுத்து;

            இயல்பு:
                fprintf(stderr, "vpi sorry: format is not implemented\n");
                ப்ரதிஜ்ஞா(பொய்);
          }
      }
}

வெறுமை __vpiArray::put_word_மதிப்பு(கட்டமைப்பு __vpiArrayWord*word, p_vpi_மதிப்பு vp, முழு)
{
      unsigned index = word->get_index();
      vvp_vector4_t val = vec4_from_vpi_மதிப்பு(vp, vals_width);
      set_word(index, 0, val);
}

vpiHandle __vpiArray::get_iter_index(கட்டமைப்பு __vpiArrayIterator*, முழு idx)
{
      எனில் (nets) திரும்பு nets[idx];

      ப்ரதிஜ்ஞா(vals4 || vals);

      எனில் (vals_சொற்கள் == 0) make_vals_சொற்கள்();

      திரும்பு &(vals_சொற்கள்[idx].as_word);
}

முழு __vpiArray::vpi_get(முழு code)
{
      தேர்வு (code) {
	  நிலை vpi_வரி_No:
	    திரும்பு 0; // Not implemented for now!

	  நிலை vpiSize:
	    திரும்பு get_size();

	  நிலை vpiAutomatic:
	    திரும்பு scope->is_automatic()? 1 : 0;

	  இயல்பு:
	    திரும்பு 0;
      }
}

எழுத்து* __vpiArray::vpi_get_str(முழு code)
{
      எனில் (code == vpiFile) {  // Not implemented for now!
            திரும்பு simple_set_rbuf_str(file_names[0]);
      }

      திரும்பு generic_get_str(code, scope, name, வெற்று);
}

vpiHandle __vpiArray::vpi_handle(முழு code)
{
      தேர்வு (code) {
          நிலை vpi_இடது_Range:
            எனில் (swap_addr) திரும்பு &last_addr;
            இல்லையெனில் திரும்பு &first_addr;

          நிலை vpi_வலது_Range:
            எனில் (swap_addr) திரும்பு &first_addr;
            இல்லையெனில் திரும்பு &last_addr;

	  நிலை vpiScope:
	    திரும்பு scope;

	  நிலை vpiModule:
	    திரும்பு vpip_module(scope);
      }

      திரும்பு 0;
}

/*
* VPI code passes indices that are not yet converted to canonical
* form, so this index function does it here.
*/
vpiHandle __vpiArray::vpi_index(முழு index)
{
      index -= first_addr.get_மதிப்பு();
      எனில் (index >= (long) get_size())
	    திரும்பு 0;
      எனில் (index < 0)
	    திரும்பு 0;

      எனில் (nets != 0) {
	    திரும்பு nets[index];
      }

      எனில் (vals_சொற்கள் == 0)
	    make_vals_சொற்கள்();

      திரும்பு &(vals_சொற்கள்[index].as_word);
}

முழு __vpiArrayWord::as_word_t::vpi_get(முழு code)
{
      கட்டமைப்பு __vpiArrayWord*obj = array_var_word_from_handle(இது);
      ப்ரதிஜ்ஞா(obj);
      கட்டமைப்பு __vpiArrayBase*my_parent = obj->get_parent();
      t_vpi_மதிப்பு val;

      தேர்வு (code) {
	  நிலை vpi_வரி_No:
	    திரும்பு 0; // Not implemented for now!

	  நிலை vpiSize:
	    திரும்பு my_parent->get_word_size();

	  நிலை vpi_இடது_Range:
            val.format = vpiIntVal;
	    my_parent->get_இடது_range()->vpi_get_மதிப்பு(&val);
            ப்ரதிஜ்ஞா(val.format == vpiIntVal);
            திரும்பு val.value.integer;

	  நிலை vpi_வலது_Range:
            val.format = vpiIntVal;
	    my_parent->get_வலது_range()->vpi_get_மதிப்பு(&val);
            ப்ரதிஜ்ஞா(val.format == vpiIntVal);
            திரும்பு val.value.integer;

	  நிலை vpiIndex:
	    {
		  முழு base_offset = 0;
		  கட்டமைப்பு __vpiArray*base = dynamic_cast<__vpiArray*> (my_parent);
		  எனில் (base) {
			val.format = vpiIntVal;
			base->first_addr.vpi_get_மதிப்பு(&val);
			base_offset += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_index.vpi_get_மதிப்பு(&val);
		  ப்ரதிஜ்ஞா(val.format == vpiIntVal);
		  திரும்பு val.value.integer + base_offset;
	    }

	  நிலை vpiAutomatic:
	    திரும்பு my_parent->get_scope()->is_automatic()? 1 : 0;

#எனில் defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  நிலை _vpiFromThr:
	    திரும்பு _vpiNoThr;
#endif

	  இயல்பு:
	    திரும்பு 0;
      }
}


முழு __vpiArrayVthrA::vpi_get(முழு code)
{
      தேர்வு (code) {
	  நிலை vpi_வரி_No:
	    திரும்பு 0; // Not implemented for now!

	  நிலை vpiSize:
	    திரும்பு array->get_word_size();

	  நிலை vpi_இடது_Range:
	    திரும்பு array->msb.get_மதிப்பு();

	  நிலை vpi_வலது_Range:
	    திரும்பு array->lsb.get_மதிப்பு();

	  நிலை vpiIndex:
	    திரும்பு (முழு)get_address() + array->first_addr.get_மதிப்பு();

	  நிலை vpiAutomatic:
	    திரும்பு array->get_scope()->is_automatic() ? 1 : 0;

#எனில் defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  நிலை _vpiFromThr:
	    திரும்பு _vpi_at_A;
#endif

	  // If address_handle is not zero we definitely have a
	  // variable.
	  நிலை vpiConstantSelect:
	    திரும்பு address_handle == 0;

	  இயல்பு:
	    திரும்பு 0;
      }
}

எழுத்து* __vpiArrayVthrA::vpi_get_str(முழு code)
{
      எனில் (code == vpiFile) {  // Not implemented for now!
            திரும்பு simple_set_rbuf_str(file_names[0]);
      }

      எழுத்து sidx [64];
      snprintf(sidx, 63, "%d", (முழு)get_address() + array->first_addr.get_மதிப்பு());
      திரும்பு generic_get_str(code, array->get_scope(), array->name, sidx);
}

வெறுமை __vpiArrayVthrA::vpi_get_மதிப்பு(p_vpi_மதிப்பு vp)
{
      ப்ரதிஜ்ஞா(array);

      unsigned index = get_address();
      எனில் (vpi_array_is_real(array)) {
	    இரட்டை tmp = array->get_word_r(index);
	    vpip_real_get_மதிப்பு(tmp, vp);
      } இல்லையெனில் எனில் (vpi_array_is_string(array)) {
	    string tmp = array->get_word_str(index);
	    vpip_string_get_மதிப்பு(tmp, vp);
      } இல்லையெனில் {
	    vvp_vector4_t tmp = array->get_word(index);
	    unsigned width = array->get_word_size();
	    vpip_vec4_get_மதிப்பு(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_மதிப்பு(p_vpi_மதிப்பு vp, முழு)
{
      unsigned index = get_address();

      ப்ரதிஜ்ஞா(array);
      ப்ரதிஜ்ஞா(index < array->get_size());

      எனில் (vpi_array_is_real(array)) {
	    இரட்டை val = real_from_vpi_மதிப்பு(vp);
	    array->set_word(index, val);
      } இல்லையெனில் {
	    unsigned width = array->get_word_size();
	    vvp_vector4_t val = vec4_from_vpi_மதிப்பு(vp, width);
	    array->set_word(index, 0, val);
      }

      திரும்பு இது;
}

vpiHandle __vpiArrayVthrA::vpi_handle(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiIndex:
	    நிறுத்து;  // Not implemented!

	  நிலை vpi_இடது_Range:
	    திரும்பு &array->msb;

	  நிலை vpi_வலது_Range:
	    திரும்பு &array->lsb;

	  நிலை vpiParent:
	  நிலை vpiArray:
	    திரும்பு array;

	  நிலை vpiScope:
	    திரும்பு array->get_scope();

	  நிலை vpiModule:
	    திரும்பு vpip_module(array->get_scope());
      }

      திரும்பு 0;
}


முழு __vpiArrayVthrAPV::vpi_get(முழு code)
{
      தேர்வு (code) {
	  நிலை vpi_வரி_No:
	    திரும்பு 0; // Not implemented for now!

	  நிலை vpiSize:
	    திரும்பு part_wid;

	  நிலை vpi_இடது_Range:
	    திரும்பு part_bit + part_wid - 1;

	  நிலை vpi_வலது_Range:
	    திரும்பு part_bit;

	  நிலை vpiAutomatic:
	    திரும்பு array->get_scope()->is_automatic() ? 1 : 0;

#எனில் defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  நிலை _vpiFromThr:
	    திரும்பு _vpi_at_APV;
#endif

	  நிலை vpiConstantSelect:
	    திரும்பு 1;

	  இயல்பு:
	    திரும்பு 0;
      }
}

எழுத்து* __vpiArrayVthrAPV::vpi_get_str(முழு code)
{
      எனில் (code == vpiFile) {  // Not implemented for now!
            திரும்பு simple_set_rbuf_str(file_names[0]);
      }

      எழுத்து sidx [64];
      snprintf(sidx, 63, "%u", word_sel + array->first_addr.get_மதிப்பு());
      திரும்பு generic_get_str(code, array->get_scope(), array->name, sidx);
}

வெறுமை __vpiArrayVthrAPV::vpi_get_மதிப்பு(p_vpi_மதிப்பு vp)
{
      ப்ரதிஜ்ஞா(array);

      unsigned index = word_sel;
      எனில் (vpi_array_is_real(array)) {
	    இரட்டை tmp = array->get_word_r(index);
	    vpip_real_get_மதிப்பு(tmp, vp);
      } இல்லையெனில் {
	    vvp_vector4_t tmp = array->get_word(index);
	    tmp = tmp.subvalue(part_bit, part_wid);
	    vpip_vec4_get_மதிப்பு(tmp, part_wid, பொய், vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(முழு code)
{
      தேர்வு (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  நிலை vpiParent:
	    திரும்பு 0;

            // Not part of the Verilog standard. We use this internally.
	  நிலை vpiArray:
	    திரும்பு array;

	  நிலை vpiScope:
	    திரும்பு array->get_scope();

	  நிலை vpiModule:
	    திரும்பு vpip_module(array->get_scope());
      }

      திரும்பு 0;
}

வெறுமை __vpiArray::set_word(unsigned address, unsigned part_off, மாறிலி vvp_vector4_t&val)
{
      எனில் (address >= get_size())
	    திரும்பு;

      எனில் (vals4) {
	    ப்ரதிஜ்ஞா(nets == 0);
	    எனில் (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp = vals4->get_word(address);
		  எனில் ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			ப்ரதிஜ்ஞா(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals4->set_word(address, tmp);
	    } இல்லையெனில் {
		  vals4->set_word(address, val);
	    }
	    word_change(address);
	    திரும்பு;
      }

      எனில் (vals) {
	    ப்ரதிஜ்ஞா(nets == 0);
	    எனில் (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp;
		  vals->get_word(address, tmp);
		  எனில் ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			ப்ரதிஜ்ஞா(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals->set_word(address, tmp);
	    } இல்லையெனில் {
		  vals->set_word(address, val);
	    }
	    word_change(address);
	    திரும்பு;
      }

      ப்ரதிஜ்ஞா(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle word = nets[address];
      மாறிலி கட்டமைப்பு __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      ப்ரதிஜ்ஞா(vsig);

      vsig->முனை->send_vec4_pv(val, part_off, vpip_size(vsig), 0);
      word_change(address);
}

வெறுமை __vpiArray::set_word(unsigned address, இரட்டை val)
{
      ப்ரதிஜ்ஞா(vals != 0);
      ப்ரதிஜ்ஞா(nets == 0);

      எனில் (address >= vals->get_size())
	    திரும்பு;

      vals->set_word(address, val);
      word_change(address);
}

வெறுமை __vpiArray::set_word(unsigned address, மாறிலி string&val)
{
      ப்ரதிஜ்ஞா(vals != 0);
      ப்ரதிஜ்ஞா(nets == 0);

      எனில் (address >= vals->get_size())
	    திரும்பு;

      vals->set_word(address, val);
      word_change(address);
}

வெறுமை __vpiArray::set_word(unsigned address, மாறிலி vvp_object_t&val)
{
      ப்ரதிஜ்ஞா(vals != 0);
      ப்ரதிஜ்ஞா(nets == 0);

      எனில் (address >= vals->get_size())
	    திரும்பு;

      vals->set_word(address, val);
      word_change(address);
}

vvp_vector4_t __vpiArray::get_word(unsigned address)
{
      எனில் (vals4) {
	    ப்ரதிஜ்ஞா(nets == 0);
	    ப்ரதிஜ்ஞா(vals == 0);
	    திரும்பு vals4->get_word(address);
      }

      எனில் (vals) {
	    ப்ரதிஜ்ஞா(nets == 0);
	    ப்ரதிஜ்ஞா(vals4== 0);
	    எனில் (address >= vals->get_size())
		  திரும்பு vvp_vector4_t(vals_width, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_word(address, val);
	    திரும்பு val;
      }

      ப்ரதிஜ்ஞா(vals4 == 0);
      ப்ரதிஜ்ஞா(vals == 0);
      ப்ரதிஜ்ஞா(nets != 0);

      எனில் (address >= get_size()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    ப்ரதிஜ்ஞா(get_size() > 0);
	    vpiHandle word = nets[0];
	    ப்ரதிஜ்ஞா(word);
	    கட்டமைப்பு __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
	    ப்ரதிஜ்ஞா(vsig);
	    vvp_signal_மதிப்பு*sig = dynamic_cast<vvp_signal_மதிப்பு*> (vsig->முனை->fil);
	    ப்ரதிஜ்ஞா(sig);
	    திரும்பு vvp_vector4_t(sig->மதிப்பு_size(), BIT4_X);
      }

      vpiHandle word = nets[address];
      கட்டமைப்பு __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      ப்ரதிஜ்ஞா(vsig);
      vvp_signal_மதிப்பு*sig = dynamic_cast<vvp_signal_மதிப்பு*> (vsig->முனை->fil);
      ப்ரதிஜ்ஞா(sig);

      vvp_vector4_t val;
      sig->vec4_மதிப்பு(val);
      திரும்பு val;
}

இரட்டை __vpiArray::get_word_r(unsigned address)
{
      எனில் (vals) {
	    ப்ரதிஜ்ஞா(vals4 == 0);
	    ப்ரதிஜ்ஞா(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    எனில் (address >= vals->get_size())
		  திரும்பு 0.0;

	    இரட்டை val;
	    vals->get_word(address, val);
	    திரும்பு val;
      }

      எனில் (address >= get_size())
	    திரும்பு 0.0;

      ப்ரதிஜ்ஞா(nets);
      vpiHandle word = nets[address];
      கட்டமைப்பு __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(word);
      ப்ரதிஜ்ஞா(vsig);
      vvp_signal_மதிப்பு*sig = dynamic_cast<vvp_signal_மதிப்பு*> (vsig->net->fil);
      ப்ரதிஜ்ஞா(sig);

      இரட்டை val = sig->real_மதிப்பு();
      திரும்பு val;
}

வெறுமை __vpiArray::get_word_obj(unsigned address, vvp_object_t&val)
{
      எனில் (vals) {
	    ப்ரதிஜ்ஞா(vals4 == 0);
	    ப்ரதிஜ்ஞா(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    எனில் (address >= vals->get_size()) {
		  val = vvp_object_t();
		  திரும்பு;
	    }

	    vals->get_word(address, val);
	    திரும்பு;
      }

      ப்ரதிஜ்ஞா(nets);
	// Arrays of string nets not implemented!
      ப்ரதிஜ்ஞா(0);
      திரும்பு;
}

string __vpiArray::get_word_str(unsigned address)
{
      எனில் (vals) {
	    ப்ரதிஜ்ஞா(vals4 == 0);
	    ப்ரதிஜ்ஞா(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    எனில் (address >= vals->get_size())
		  திரும்பு "";

	    string val;
	    vals->get_word(address, val);
	    திரும்பு val;
      }

      ப்ரதிஜ்ஞா(nets);
	// Arrays of string nets not implemented!
      ப்ரதிஜ்ஞா(0);
      திரும்பு "";
}

vpiHandle vpip_make_array(மாறிலி எழுத்து*label, மாறிலி எழுத்து*name,
                          முழு first_addr, முழு last_addr,
                          தருக்க signed_flag)
{
      கட்டமைப்பு __vpiArray*obj = புதிய __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      எனில் (last_addr >= first_addr) {
	    obj->swap_addr = பொய்;
      } இல்லையெனில் {
	    obj->swap_addr = உண்மை;
	    முழு tmp = last_addr;
	    last_addr = first_addr;
	    first_addr = tmp;
      }
      ப்ரதிஜ்ஞா(last_addr >= first_addr);
      unsigned array_மொத்தம் = last_addr+1-first_addr;

	// For now, treat all arrays as memories. This is not quite
	// correct, as arrays are arrays with memories a special case.
      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_மொத்தம் = array_மொத்தம்;

      obj->first_addr.set_மதிப்பு(first_addr);
      obj->last_addr .set_மதிப்பு(last_addr);

	// Start off now knowing if we are nets or variables.
      obj->nets = 0;
      obj->vals4 = 0;
      obj->vals  = 0;
      obj->vals_width = 0;
      obj->vals_சொற்கள் = 0;

	// Initialize (clear) the read-ports list.
      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

	/* Add this symbol to the array_symbols table for later lookup. */
      எனில் (!array_table)
	    array_table = புதிய குறியீடு_map_s<கட்டமைப்பு __vpiArray>;

      ப்ரதிஜ்ஞா(!array_find(label));
      array_table->sym_set_மதிப்பு(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_குறியீடு(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_to_current_scope(obj);

      திரும்பு obj;
}

வெறுமை __vpiArray::alias_word(unsigned long addr, vpiHandle word, முழு msb_, முழு lsb_)
{
      ப்ரதிஜ்ஞா(msb.get_மதிப்பு() == msb_);
      ப்ரதிஜ்ஞா(lsb.get_மதிப்பு() == lsb_);
      ப்ரதிஜ்ஞா(addr < get_size());
      ப்ரதிஜ்ஞா(nets);
      nets[addr] = word;
}

வெறுமை __vpiArray::attach_word(unsigned addr, vpiHandle word)
{
      ப்ரதிஜ்ஞா(addr < get_size());
      ப்ரதிஜ்ஞா(nets);
      nets[addr] = word;

      எனில் (கட்டமைப்பு __vpiSignal*sig = dynamic_cast<__vpiSignal*>(word)) {
	    vvp_net_t*net = sig->முனை;
	    ப்ரதிஜ்ஞா(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    ப்ரதிஜ்ஞா(fun);
	    fun->attach_as_word(இது, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = இது;
	    sig->id.index = புதிய __vpiDecConst(addr + first_addr.get_மதிப்பு());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    திரும்பு;
      }

      எனில் (கட்டமைப்பு __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(word)) {
	    vvp_net_t*net = sig->net;
	    ப்ரதிஜ்ஞா(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    ப்ரதிஜ்ஞா(fun);
	    fun->attach_as_word(இது, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = இது;
	    sig->id.index = புதிய __vpiDecConst(addr + first_addr.get_மதிப்பு());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = உண்மை;
	    திரும்பு;
      }
}

வெறுமை compile_var_array(எழுத்து*label, எழுத்து*name, முழு last, முழு first,
		   முழு msb, முழு lsb, எழுத்து signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last,
                                      signed_flag != 0);

      கட்டமைப்பு __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_width = labs(msb-lsb) + 1;
      எனில் (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = புதிய vvp_vector4array_aa(arr->vals_width,
						 arr->get_size());
      } இல்லையெனில் {
            arr->vals4 = புதிய vvp_vector4array_sa(arr->vals_width,
						 arr->get_size());
      }
      arr->msb.set_மதிப்பு(msb);
      arr->lsb.set_மதிப்பு(lsb);

      மொத்தம்_var_arrays += 1;
      மொத்தம்_var_array_சொற்கள் += arr->get_size();

      விடுவி(label);
      delete[] name;
}

வெறுமை compile_var2_array(எழுத்து*label, எழுத்து*name, முழு last, முழு first,
		   முழு msb, முழு lsb, தருக்க signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, signed_flag);

      கட்டமைப்பு __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.set_மதிப்பு(msb);
      arr->lsb.set_மதிப்பு(lsb);
      arr->vals_width = labs(msb-lsb) + 1;

      ப்ரதிஜ்ஞா(! arr->nets);
      எனில் (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = புதிய vvp_darray_atom<int8_t>(arr->get_size());
      } இல்லையெனில் எனில் (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = புதிய vvp_darray_atom<uint8_t>(arr->get_size());
      } இல்லையெனில் எனில் (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = புதிய vvp_darray_atom<int16_t>(arr->get_size());
      } இல்லையெனில் எனில் (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = புதிய vvp_darray_atom<uint16_t>(arr->get_size());
      } இல்லையெனில் எனில் (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = புதிய vvp_darray_atom<int32_t>(arr->get_size());
      } இல்லையெனில் எனில் (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = புதிய vvp_darray_atom<uint32_t>(arr->get_size());
      } இல்லையெனில் எனில் (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = புதிய vvp_darray_atom<int64_t>(arr->get_size());
      } இல்லையெனில் எனில் (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = புதிய vvp_darray_atom<uint64_t>(arr->get_size());
      } இல்லையெனில் {
	    arr->vals = புதிய vvp_darray_vec2(arr->get_size(), arr->vals_width);
      }
      மொத்தம்_var_arrays += 1;
      மொத்தம்_var_array_சொற்கள் += arr->get_size();

      விடுவி(label);
      delete[] name;
}

வெறுமை compile_real_array(எழுத்து*label, எழுத்து*name, முழு last, முழு first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, உண்மை);

      கட்டமைப்பு __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = புதிய vvp_darray_real(arr->get_size());
      arr->vals_width = 1;

      மொத்தம்_real_arrays += 1;
      மொத்தம்_real_array_சொற்கள் += arr->get_size();

      விடுவி(label);
      delete[] name;
}

வெறுமை compile_string_array(எழுத்து*label, எழுத்து*name, முழு last, முழு first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, உண்மை);

      கட்டமைப்பு __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = புதிய vvp_darray_string(arr->get_size());
      arr->vals_width = 1;

      மொத்தம்_real_arrays += 1;
      மொத்தம்_real_array_சொற்கள் += arr->get_size();

      விடுவி(label);
      delete[] name;
}

வெறுமை compile_object_array(எழுத்து*label, எழுத்து*name, முழு last, முழு first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, உண்மை);

      கட்டமைப்பு __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = புதிய vvp_darray_object(arr->get_size());
      arr->vals_width = 1;

      மொத்தம்_real_arrays += 1;
      மொத்தம்_real_array_சொற்கள் += arr->get_size();

      விடுவி(label);
      delete[] name;
}

வெறுமை compile_net_array(எழுத்து*label, எழுத்து*name, முழு last, முழு first)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, first, last, பொய்);

      கட்டமைப்பு __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(calloc(arr->get_size(), அளவு(vpiHandle)));

      மொத்தம்_net_arrays += 1;
      மொத்தம்_net_array_சொற்கள் += arr->get_size();

      விடுவி(label);
      delete[] name;
}

வகுப்பு vvp_fun_arrayport  : பொது vvp_net_fun_t {

    பொது:
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport() override;

      மெய்நிகர் வெறுமை check_word_change(unsigned long addr) = 0;

    பாதுகாக்கப்பட்ட:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      unsigned long addr_;

      friend வெறுமை array_attach_port(vvp_array_t, vvp_fun_arrayport*);
      friend வெறுமை __vpiArray::word_change(unsigned long);
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

வகுப்பு vvp_fun_arrayport_sa  : பொது vvp_fun_arrayport {

    பொது:
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_sa() override;

      வெறுமை check_word_change(unsigned long addr) override;

      வெறுமை recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                     vvp_context_t) override;

    தனிப்பட்ட:
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

வெறுமை vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                                     vvp_context_t)
{
      தருக்க addr_valid_flag;

      தேர்வு (port.port()) {

	  நிலை 0: // Address input
	    addr_valid_flag = vector4_to_மதிப்பு(bit, addr_);
	    எனில் (! addr_valid_flag)
		  addr_ = arr_->get_size();
	    எனில் (vpi_array_is_real(arr_))
		  port.ptr()->send_real(arr_->get_word_r(addr_), 0);
	    இல்லையெனில்
		  port.ptr()->send_vec4(arr_->get_word(addr_), 0);

	    நிறுத்து;

	  இயல்பு:
	    fprintf(stdout, "XXXX write ports not implemented.\n");
	    ப்ரதிஜ்ஞா(0);
      }
}

வெறுமை vvp_fun_arrayport_sa::check_word_change(unsigned long addr)
{
      எனில் (addr != addr_) திரும்பு;

      எனில் (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr_), 0);
      } இல்லையெனில் {
	    net_->send_vec4(arr_->get_word(addr_), 0);
      }
}

வகுப்பு vvp_fun_arrayport_aa  : பொது vvp_fun_arrayport, பொது automatic_hooks_s {

    பொது:
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_aa() override;

      வெறுமை alloc_instance(vvp_context_t context) override;
      வெறுமை reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      வெறுமை free_instance(vvp_context_t context) override;
#endif

      வெறுமை check_word_change(unsigned long addr) override;

      வெறுமை recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                     vvp_context_t context) override;

    தனிப்பட்ட:
      வெறுமை check_word_change_(unsigned long addr, vvp_context_t context);

      __vpiScope*context_scope_;
      unsigned context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(இது, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(இது, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

வெறுமை vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      unsigned long*addr = புதிய unsigned long;
      vvp_set_context_item(context, context_idx_, addr);

      *addr = addr_;
}

வெறுமை vvp_fun_arrayport_aa::reset_instance(vvp_context_t context)
{
      unsigned long*addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));

      *addr = addr_;
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை vvp_fun_arrayport_aa::free_instance(vvp_context_t context)
{
      unsigned long*addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));
      delete addr;
}
#endif

வெறுமை vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
                                     vvp_context_t context)
{
      எனில் (context) {
            unsigned long*addr = static_cast<unsigned long*>
                  (vvp_get_context_item(context, context_idx_));

            தருக்க addr_valid_flag;

            தேர்வு (port.port()) {

                நிலை 0: // Address input
                  addr_valid_flag = vector4_to_மதிப்பு(bit, *addr);
                  எனில் (! addr_valid_flag) *addr = arr_->get_size();
                  எனில் (vpi_array_is_real(arr_)) {
			port.ptr()->send_real(arr_->get_word_r(*addr),
					      context);
                  } இல்லையெனில் {
			port.ptr()->send_vec4(arr_->get_word(*addr),
					      context);
                  }
                  நிறுத்து;

                இயல்பு:
                  fprintf(stdout, "XXXX write ports not implemented.\n");
                  ப்ரதிஜ்ஞா(0);
            }
      } இல்லையெனில் {
            context = context_scope_->live_contexts;
            போது (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

வெறுமை vvp_fun_arrayport_aa::check_word_change_(unsigned long addr,
                                              vvp_context_t context)
{
      மாறிலி unsigned long*port_addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));

      எனில் (addr != *port_addr)
	    திரும்பு;

      எனில் (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr), context);
      } இல்லையெனில் {
	    net_->send_vec4(arr_->get_word(addr), context);
      }
}

வெறுமை vvp_fun_arrayport_aa::check_word_change(unsigned long addr)
{
      எனில் (arr_->get_scope()->is_automatic()) {
            ப்ரதிஜ்ஞா(vthread_get_wt_context());
            check_word_change_(addr, vthread_get_wt_context());
      } இல்லையெனில் {
            vvp_context_t context = context_scope_->live_contexts;
            போது (context) {
                  check_word_change_(addr, context);
                  context = vvp_get_next_context(context);
            }
      }
}

நிலையான வெறுமை array_attach_port(vvp_array_t array, vvp_fun_arrayport*fun)
{
      ப்ரதிஜ்ஞா(fun->next_ == 0);
      fun->next_ = array->ports_;
      array->ports_ = fun;
      எனில் (!array->get_scope()->is_automatic() &&
          (array->vals4 || array->vals)) {
              /* propagate initial values for variable arrays */
            எனில் (!vpi_array_is_real(array)) {
                  vvp_bit4_t init;
                  எனில் (array->vals4)
                      init = BIT4_X;
                  இல்லையெனில்
                      init = BIT4_0;
                  vvp_vector4_t tmp(array->vals_width, init);
                  schedule_init_propagate(fun->net_, tmp);
            } இல்லையெனில் {
                  schedule_init_propagate(fun->net_, 0.0);
            }
      }
}

வகுப்பு array_word_மதிப்பு_callback : பொது மதிப்பு_callback {
    பொது:
      உள்வரி explicit array_word_மதிப்பு_callback(p_cb_data data, long addr)
      : மதிப்பு_callback(data), word_addr(addr)
      { }

    பொது:
      long word_addr;
};

வெறுமை __vpiArray::word_change(unsigned long addr)
{
      க்கு (vvp_fun_arrayport*cur = ports_; cur; cur = cur->next_)
	    cur->check_word_change(addr);

	// Run callbacks attached to the array itself.
      கட்டமைப்பு __vpiCallback *next = vpi_callbacks;
      கட்டமைப்பு __vpiCallback *prev = 0;

      போது (next) {
	    array_word_மதிப்பு_callback*cur = dynamic_cast<array_word_மதிப்பு_callback*>(next);
	    next = cur->next;

	      // Skip callbacks that are not for me. -1 is for every element.
	    எனில் (cur->word_addr != (long)addr && cur->word_addr != -1) {
		  prev = cur;
		  தொடர்;
	    }

	      // For whole array callbacks we need to set the index.
	    எனில் (cur->word_addr == -1) {
		  cur->cb_data.index = (PLI_INT32) ((முழு)addr +
						    first_addr.get_மதிப்பு());
	    }

	    எனில் (cur->cb_data.cb_rtn != 0) {
		  எனில் (cur->test_மதிப்பு_callback_ready()) {
			எனில் (cur->cb_data.value) {
			      எனில் (vpi_array_is_real(இது)) {
				    இரட்டை val = 0.0;
				    எனில் (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_real_get_மதிப்பு(val, cur->cb_data.value);
			      } இல்லையெனில் எனில் (vals4) {
				    vpip_vec4_get_மதிப்பு(vals4->get_word(addr),
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } இல்லையெனில் எனில் (dynamic_cast<vvp_darray_atom<int8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int64_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint64_t>*>(vals)
				      || dynamic_cast<vvp_darray_vec2*>(vals)) {
				    vvp_vector4_t val;
				    எனில் (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_vec4_get_மதிப்பு(val,
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } இல்லையெனில் {
			            ப்ரதிஜ்ஞா(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } இல்லையெனில் எனில் (prev == 0) {

		  vpi_callbacks = next;
		  cur->next = 0;
		  delete cur;

	    } இல்லையெனில் {
		  ப்ரதிஜ்ஞா(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}

வகுப்பு array_resolv_list_t : பொது resolv_list_s {

    பொது:
      explicit array_resolv_list_t(எழுத்து*lab) : resolv_list_s(lab) {
	    array = 0;
      }

      vvp_array_t*array;
      தருக்க resolve(தருக்க mes) override;

    தனிப்பட்ட:
};

தருக்க array_resolv_list_t::resolve(தருக்க mes)
{
      *array = array_find(label());
      எனில் (*array == 0) {
	    ப்ரதிஜ்ஞா(!mes);
	    திரும்பு பொய்;
      }
      திரும்பு உண்மை;
}

வகுப்பு array_port_resolv_list_t : பொது resolv_list_s {

    பொது:
      explicit array_port_resolv_list_t(எழுத்து* lab, தருக்க use_addr__,
                                        long addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      தருக்க use_addr;
      long addr;
      தருக்க resolve(தருக்க mes) override;

    தனிப்பட்ட:
};

array_port_resolv_list_t::array_port_resolv_list_t(எழுத்து *lab, தருக்க use_addr__,
                                                   long addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      எனில் (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      இல்லையெனில்
            context_scope = 0;
      ptr = புதிய vvp_net_t;
}

தருக்க array_port_resolv_list_t::resolve(தருக்க mes)
{
      vvp_array_t mem = array_find(label());
      எனில் (mem == 0) {
	    ப்ரதிஜ்ஞா(mem || !mes);
	    திரும்பு பொய்;
      }

      vvp_fun_arrayport*fun;
      எனில் (use_addr)
            எனில் (context_scope)
                  fun = புதிய vvp_fun_arrayport_aa(context_scope, mem, ptr, addr);
            இல்லையெனில்
                  fun = புதிய vvp_fun_arrayport_sa(mem, ptr, addr);
      இல்லையெனில்
            எனில் (context_scope)
                  fun = புதிய vvp_fun_arrayport_aa(context_scope, mem, ptr);
            இல்லையெனில்
                  fun = புதிய vvp_fun_arrayport_sa(mem, ptr);
      ptr->fun = fun;

      array_attach_port(mem, fun);

      திரும்பு உண்மை;
}

வகுப்பு array_word_part_callback : பொது array_word_மதிப்பு_callback {
    பொது:
      explicit array_word_part_callback(p_cb_data data, long addr);
      ~array_word_part_callback() override;

      array_word_part_callback(மாறிலி array_word_part_callback&) = delete;
      array_word_part_callback& operator=(மாறிலி array_word_part_callback&) = delete;

      தருக்க test_மதிப்பு_callback_ready(வெறுமை) override;

    தனிப்பட்ட:
      எழுத்து*மதிப்பு_bits;
};

array_word_part_callback::array_word_part_callback(p_cb_data data, long addr)
: array_word_மதிப்பு_callback(data, addr)
{
	// Get the initial value of the part, to use as a reference.
      கட்டமைப்பு __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj);
      s_vpi_மதிப்பு tmp_மதிப்பு;
      tmp_மதிப்பு.format = vpiBinStrVal;
      apvword->vpi_get_மதிப்பு(&tmp_மதிப்பு);

      மதிப்பு_bits = புதிய எழுத்து[apvword->part_wid+1];

      memcpy(மதிப்பு_bits, tmp_மதிப்பு.value.str, apvword->part_wid);
      மதிப்பு_bits[apvword->part_wid] = 0;
}

array_word_part_callback::~array_word_part_callback()
{
      delete[]மதிப்பு_bits;
}

தருக்க array_word_part_callback::test_மதிப்பு_callback_ready(வெறுமை)
{
      கட்டமைப்பு __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_data.obj);
      ப்ரதிஜ்ஞா(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_மதிப்பு tmp_மதிப்பு;
      tmp_மதிப்பு.format = vpiBinStrVal;
      apvword->vpi_get_மதிப்பு(&tmp_மதிப்பு);

      எனில் (memcmp(மதிப்பு_bits, tmp_மதிப்பு.value.str, apvword->part_wid) == 0)
	    திரும்பு பொய்;

      memcpy(மதிப்பு_bits, tmp_மதிப்பு.value.str, apvword->part_wid);
      திரும்பு உண்மை;

}

மதிப்பு_callback*vpip_array_word_change(p_cb_data data)
{
      கட்டமைப்பு __vpiArray*parent = 0;
      array_word_மதிப்பு_callback*cbh = 0;
      எனில் (மாறிலி கட்டமைப்பு __vpiArrayWord*word = array_var_word_from_handle(data->obj)) {
	    parent = static_cast<__vpiArray*>(word->get_parent());
	    unsigned addr = word->get_index();
	    cbh = புதிய array_word_மதிப்பு_callback(data, addr);

      } இல்லையெனில் எனில் (கட்டமைப்பு __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(data->obj)) {
	    parent = tword->array;
	    cbh = புதிய array_word_மதிப்பு_callback(data, tword->address);

      } இல்லையெனில் எனில் (கட்டமைப்பு __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj)) {
	    parent = apvword->array;
	    cbh = புதிய array_word_part_callback(data, apvword->word_sel);
      }

      ப்ரதிஜ்ஞா(cbh);
      ப்ரதிஜ்ஞா(parent);
      cbh->next = parent->vpi_callbacks;
      parent->vpi_callbacks = cbh;

      திரும்பு cbh;
}

மதிப்பு_callback* vpip_array_change(p_cb_data data)
{
      array_word_மதிப்பு_callback*cbh = புதிய array_word_மதிப்பு_callback(data, -1);
      ப்ரதிஜ்ஞா(data->obj);

      கட்டமைப்பு __vpiArray*arr = dynamic_cast<__vpiArray*>(data->obj);
      cbh->next = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      திரும்பு cbh;
}

வெறுமை compile_array_port(எழுத்து*label, எழுத்து*array, எழுத்து*addr)
{
      array_port_resolv_list_t*resolv_mem
	    = புதிய array_port_resolv_list_t(array, பொய், 0);

      define_functor_குறியீடு(label, resolv_mem->ptr);
      விடுவி(label);
	// Connect the port-0 input as the address.
      உள்ளீடு_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

வெறுமை compile_array_port(எழுத்து*label, எழுத்து*array, long addr)
{
      array_port_resolv_list_t*resolv_mem
	    = புதிய array_port_resolv_list_t(array, உண்மை, addr);

      define_functor_குறியீடு(label, resolv_mem->ptr);
      விடுவி(label);

      resolv_submit(resolv_mem);
}

வெறுமை compile_array_alias(எழுத்து*label, எழுத்து*name, எழுத்து*src)
{
      vvp_array_t mem = array_find(src);
      ப்ரதிஜ்ஞா(mem);

      கட்டமைப்பு __vpiArray*obj = புதிய __vpiArray;

      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_மொத்தம் = mem->array_மொத்தம்;
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
      obj->vals_சொற்கள் = mem->vals_சொற்கள்;

      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

      ப்ரதிஜ்ஞா(array_table);
      ப்ரதிஜ்ஞா(!array_find(label));
      array_table->sym_set_மதிப்பு(label, obj);

      compile_vpi_குறியீடு(label, obj);
      vpip_attach_to_current_scope(obj);

      விடுவி(label);
      விடுவி(name);
      விடுவி(src);
}

/*
 * &A<label,addr>
 * This represents a VPI handle for an addressed array. This comes
 * from expressions like "label[addr]" where "label" is the array and
 * "addr" is the canonical address of the desired word.
 */
vpiHandle vpip_make_vthr_A(எழுத்து*label, unsigned addr)
{
      கட்டமைப்பு __vpiArrayVthrA*obj = புதிய __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = புதிய array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      obj->address = addr;

      திரும்பு obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(எழுத்து*label, எழுத்து*குறியீடு)
{
      கட்டமைப்பு __vpiArrayVthrA*obj = புதிய __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = புதிய array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      compile_vpi_lookup(&obj->address_handle, குறியீடு);
      obj->address = 0;

      திரும்பு obj;
}

vpiHandle vpip_make_vthr_A(எழுத்து*label, vpiHandle handle)
{
      கட்டமைப்பு __vpiArrayVthrA*obj = புதிய __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = புதிய array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = handle;
      obj->address = 0;

      திரும்பு obj;
}

vpiHandle vpip_make_vthr_APV(எழுத்து*label, unsigned index, unsigned bit, unsigned wid)
{
      கட்டமைப்பு __vpiArrayVthrAPV*obj = புதிய __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = புதிய array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->word_sel = index;
      obj->part_bit = bit;
      obj->part_wid = wid;

      திரும்பு obj;
}

வெறுமை compile_array_cleanup(வெறுமை)
{
      delete array_table;
      array_table = 0;
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை memory_delete(vpiHandle item)
{
      கட்டமைப்பு __vpiArray*arr = static_cast<கட்டமைப்பு __vpiArray*>(item);
      எனில் (arr->vals_சொற்கள்) delete [] (arr->vals_சொற்கள்-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals;

      எனில் (arr->nets) {
	    க்கு (unsigned idx = 0; idx < arr->get_size(); idx += 1) {
		  எனில் (கட்டமைப்பு __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.index);
		    /* These should only be the real words. */
		  } இல்லையெனில் {
			ப்ரதிஜ்ஞா(arr->nets[idx]->get_type_code() ==
			       vpiRealVar);
			கட்டமைப்பு __vpiRealVar *sigr = static_cast<கட்டமைப்பு __vpiRealVar *>
			                            (arr->nets[idx]);
			constant_delete(sigr->id.index);
// Why are only the real words still here?
			delete arr->nets[idx];
		  }
	    }
	    விடுவி(arr->nets);
      }

      போது (arr->vpi_callbacks) {
	    கட்டமைப்பு __vpiCallback*tmp = arr->vpi_callbacks->next;
	    delete arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      delete arr;
}

வெறுமை A_delete(vpiHandle item)
{
      கட்டமைப்பு __vpiArrayVthrA*obj = static_cast<கட்டமைப்பு __vpiArrayVthrA*>(item);
      எனில் (obj->address_handle) {
	    தேர்வு (obj->address_handle->get_type_code()) {
		நிலை vpiMemoryWord:
		  எனில் (vpi_get(_vpiFromThr, obj->address_handle) == _vpi_at_A) {
			A_delete(obj->address_handle);
		  }
		  நிறுத்து;
		நிலை vpiPartSelect:
		  ப்ரதிஜ்ஞா(vpi_get(_vpiFromThr, obj->address_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->address_handle);
		  நிறுத்து;
	    }
      }

      delete obj;
}

வெறுமை APV_delete(vpiHandle item)
{
      கட்டமைப்பு __vpiArrayVthrAPV*obj = static_cast<கட்டமைப்பு __vpiArrayVthrAPV*>(item);
      delete obj;
}
#endif
