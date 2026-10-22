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
#অন্তর্ভাব  "vvp_cleanup.h"
#endif
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <iostream>
# include  "compile.h"
# include  <cassert>
# include  "ivl_alloc.h"

using নামাকাশ std;

unsigned long গুনতি_net_arrays = 0;
unsigned long গুনতি_net_array_শব্দগুলি = 0;
unsigned long গুনতি_var_arrays = 0;
unsigned long গুনতি_var_array_শব্দগুলি = 0;
unsigned long গুনতি_real_arrays = 0;
unsigned long গুনতি_real_array_শব্দগুলি = 0;

স্থির প্রতীক_map_s<সংরচনা __vpiArray>* array_table =0;

বর্গ vvp_fun_arrayport;
স্থির শূন্য array_attach_port(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_find(ধ্রুব বর্ণ*label)
{
      যদি (array_table == 0)
	    প্রত্যাগম 0;

      vvp_array_t v = array_table->sym_get_মান(label);
      প্রত্যাগম v;
}

সংরচনা __vpiArrayVthrA : সার্বজনিক __vpiHandle {
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override { প্রত্যাগম vpiMemoryWord; }
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      বর্ণ* vpi_get_str(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণাঙ্ক flags) override;
      vpiHandle vpi_handle(পূর্ণাঙ্ক code) override;

      সংরচনা __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle address_handle;
	// If wid==0, then address is the address into the array.
      unsigned address;

      unsigned get_address() ধ্রুব
      {
	    যদি (address_handle) {
		  s_vpi_মান vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpiVectorVal;
		  address_handle->vpi_get_মান(&vp);
		  পূর্ণাঙ্ক শব্দগুলি = (address_handle->vpi_get(vpiSize)-1)/32 + 1;
		  প্রতি(পূর্ণাঙ্ক idx = 0; idx < শব্দগুলি; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			যদি (vp.value.vector[idx].bval != 0) প্রত্যাগম UINT_MAX;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  address_handle->vpi_get_মান(&vp);
		  প্রত্যাগম vp.value.integer;
	    }

	    প্রত্যাগম address;
      }
};


সংরচনা __vpiArrayVthrAPV : সার্বজনিক __vpiHandle {
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুব override { প্রত্যাগম vpiPartSelect; }
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      বর্ণ* vpi_get_str(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;
      vpiHandle vpi_handle(পূর্ণাঙ্ক code) override;

      সংরচনা __vpiArray*array;
      unsigned word_sel;
      unsigned part_bit;
      unsigned part_wid;
};

তার্কিক is_net_array(vpiHandle obj)
{
      সংরচনা __vpiArray*rfp = dynamic_cast<__vpiArray*> (obj);
      প্রতিজ্ঞা(rfp);

      যদি (rfp->nets != 0) প্রত্যাগম সত্য;
      প্রত্যাগম অসত্য;
}

// This function return true if the underlying array words are real.
স্থির তার্কিক vpi_array_is_real(ধ্রুব vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      যদি (arr->vals4 != 0)  // A bit based variable/register array.
	    প্রত্যাগম অসত্য;

      যদি (dynamic_cast<vvp_darray_real*> (arr->vals))
	    প্রত্যাগম সত্য;

      যদি (arr->vals != 0)
	    প্রত্যাগম অসত্য;

	// This must be a net array so look at element 0 to find the type.
      প্রতিজ্ঞা(arr->nets != 0);
      প্রতিজ্ঞা(arr->get_size() > 0);
      ধ্রুব সংরচনা __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      যদি (rsig) {
	    প্রত্যাগম সত্য;
      }

      প্রত্যাগম অসত্য;
}

স্থির তার্কিক vpi_array_is_string(ধ্রুব vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      যদি (arr->vals4 != 0)  // A bit based variable/register array.
	    প্রত্যাগম অসত্য;

      যদি (dynamic_cast<vvp_darray_string*> (arr->vals))
	    প্রত্যাগম সত্য;

      প্রত্যাগম অসত্য;
}

পূর্ণাঙ্ক __vpiArray::get_word_size() ধ্রুব
{
      unsigned width;

      প্রতিজ্ঞা(get_size() > 0);
	/* For a net array we need to get the width from the first element. */
      যদি (nets) {
	    প্রতিজ্ঞা(vals4 == 0 && vals == 0);
	    ধ্রুব সংরচনা __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    প্রতিজ্ঞা(vsig);
	    width = vpip_size(vsig);
	/* For a variable array we can get the width from vals_width. */
      } অন্যথা {
	    প্রতিজ্ঞা(vals4 || vals);
	    width = vals_width;
      }

      প্রত্যাগম width;
}

বর্ণ*__vpiArray::get_word_str(সংরচনা __vpiArrayWord*word, পূর্ণাঙ্ক code)
{
      unsigned index = word->get_index();

      যদি (code == vpiFile) {  // Not implemented for now!
	    প্রত্যাগম simple_set_rbuf_str(file_names[0]);
      }

      বর্ণ sidx [64];
      snprintf(sidx, 63, "%d", (পূর্ণাঙ্ক)index + first_addr.get_মান());
      প্রত্যাগম generic_get_str(code, get_scope(), name, sidx);
}

শূন্য __vpiArray::get_word_মান(সংরচনা __vpiArrayWord*word, p_vpi_মান vp)
{
      unsigned index = word->get_index();

    // Determine the appropriate format (The Verilog PLI Handbook 5.2.10)
      যদি(vp->format == vpiObjTypeVal) {
          যদি(vpi_array_is_real(এতত্))
              vp->format = vpiRealVal;
          অন্যথা যদি(vpi_array_is_string(এতত্))
              vp->format = vpiStringVal;
          অন্যথা
              vp->format = vpiIntVal;
      }

      যদি(vals4) {
          vpip_vec4_get_মান(vals4->get_word(index),
                              vals4->width(), signed_flag, vp);
      } অন্যথা যদি(vals) {
          বিকল্প(vp->format) {
            পক্ষ vpiBinStrVal:
            পক্ষ vpiOctStrVal:
            পক্ষ vpiDecStrVal:
            পক্ষ vpiHexStrVal:
            পক্ষ vpiScalarVal:
            পক্ষ vpiIntVal:
            পক্ষ vpiVectorVal:
            {
                vvp_vector4_t v;
                vals->get_word(index, v);
                vpip_vec4_get_মান(v, vals_width, signed_flag, vp);
            }
            ভঙ্গ;

            পক্ষ vpiRealVal:
            {
                দ্বিগুণ d;
                vals->get_word(index, d);
                vpip_real_get_মান(d, vp);
            }
            ভঙ্গ;

            পক্ষ vpiStringVal:
            {
                string s;
                vals->get_word(index, s);
                vpip_string_get_মান(s, vp);
            }
            ভঙ্গ;

            স্বভাব:
                fprintf(stderr, "vpi sorry: format is not implemented\n");
                প্রতিজ্ঞা(অসত্য);
          }
      }
}

শূন্য __vpiArray::put_word_মান(সংরচনা __vpiArrayWord*word, p_vpi_মান vp, পূর্ণাঙ্ক)
{
      unsigned index = word->get_index();
      vvp_vector4_t val = vec4_from_vpi_মান(vp, vals_width);
      set_word(index, 0, val);
}

vpiHandle __vpiArray::get_iter_index(সংরচনা __vpiArrayIterator*, পূর্ণাঙ্ক idx)
{
      যদি (nets) প্রত্যাগম nets[idx];

      প্রতিজ্ঞা(vals4 || vals);

      যদি (vals_শব্দগুলি == 0) make_vals_শব্দগুলি();

      প্রত্যাগম &(vals_শব্দগুলি[idx].as_word);
}

পূর্ণাঙ্ক __vpiArray::vpi_get(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
	  পক্ষ vpi_সারি_No:
	    প্রত্যাগম 0; // Not implemented for now!

	  পক্ষ vpiSize:
	    প্রত্যাগম get_size();

	  পক্ষ vpiAutomatic:
	    প্রত্যাগম scope->is_automatic()? 1 : 0;

	  স্বভাব:
	    প্রত্যাগম 0;
      }
}

বর্ণ* __vpiArray::vpi_get_str(পূর্ণাঙ্ক code)
{
      যদি (code == vpiFile) {  // Not implemented for now!
            প্রত্যাগম simple_set_rbuf_str(file_names[0]);
      }

      প্রত্যাগম generic_get_str(code, scope, name, রিক্ত);
}

vpiHandle __vpiArray::vpi_handle(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
          পক্ষ vpi_বাম_Range:
            যদি (swap_addr) প্রত্যাগম &last_addr;
            অন্যথা প্রত্যাগম &first_addr;

          পক্ষ vpi_ডান_Range:
            যদি (swap_addr) প্রত্যাগম &first_addr;
            অন্যথা প্রত্যাগম &last_addr;

	  পক্ষ vpiScope:
	    প্রত্যাগম scope;

	  পক্ষ vpiModule:
	    প্রত্যাগম vpip_module(scope);
      }

      প্রত্যাগম 0;
}

/*
* VPI code passes indices that are not yet converted to canonical
* form, so this index function does it here.
*/
vpiHandle __vpiArray::vpi_index(পূর্ণাঙ্ক index)
{
      index -= first_addr.get_মান();
      যদি (index >= (long) get_size())
	    প্রত্যাগম 0;
      যদি (index < 0)
	    প্রত্যাগম 0;

      যদি (nets != 0) {
	    প্রত্যাগম nets[index];
      }

      যদি (vals_শব্দগুলি == 0)
	    make_vals_শব্দগুলি();

      প্রত্যাগম &(vals_শব্দগুলি[index].as_word);
}

পূর্ণাঙ্ক __vpiArrayWord::as_word_t::vpi_get(পূর্ণাঙ্ক code)
{
      সংরচনা __vpiArrayWord*obj = array_var_word_from_handle(এতত্);
      প্রতিজ্ঞা(obj);
      সংরচনা __vpiArrayBase*my_parent = obj->get_parent();
      t_vpi_মান val;

      বিকল্প (code) {
	  পক্ষ vpi_সারি_No:
	    প্রত্যাগম 0; // Not implemented for now!

	  পক্ষ vpiSize:
	    প্রত্যাগম my_parent->get_word_size();

	  পক্ষ vpi_বাম_Range:
            val.format = vpiIntVal;
	    my_parent->get_বাম_range()->vpi_get_মান(&val);
            প্রতিজ্ঞা(val.format == vpiIntVal);
            প্রত্যাগম val.value.integer;

	  পক্ষ vpi_ডান_Range:
            val.format = vpiIntVal;
	    my_parent->get_ডান_range()->vpi_get_মান(&val);
            প্রতিজ্ঞা(val.format == vpiIntVal);
            প্রত্যাগম val.value.integer;

	  পক্ষ vpiIndex:
	    {
		  পূর্ণাঙ্ক base_offset = 0;
		  সংরচনা __vpiArray*base = dynamic_cast<__vpiArray*> (my_parent);
		  যদি (base) {
			val.format = vpiIntVal;
			base->first_addr.vpi_get_মান(&val);
			base_offset += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_index.vpi_get_মান(&val);
		  প্রতিজ্ঞা(val.format == vpiIntVal);
		  প্রত্যাগম val.value.integer + base_offset;
	    }

	  পক্ষ vpiAutomatic:
	    প্রত্যাগম my_parent->get_scope()->is_automatic()? 1 : 0;

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  পক্ষ _vpiFromThr:
	    প্রত্যাগম _vpiNoThr;
#endif

	  স্বভাব:
	    প্রত্যাগম 0;
      }
}


পূর্ণাঙ্ক __vpiArrayVthrA::vpi_get(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
	  পক্ষ vpi_সারি_No:
	    প্রত্যাগম 0; // Not implemented for now!

	  পক্ষ vpiSize:
	    প্রত্যাগম array->get_word_size();

	  পক্ষ vpi_বাম_Range:
	    প্রত্যাগম array->msb.get_মান();

	  পক্ষ vpi_ডান_Range:
	    প্রত্যাগম array->lsb.get_মান();

	  পক্ষ vpiIndex:
	    প্রত্যাগম (পূর্ণাঙ্ক)get_address() + array->first_addr.get_মান();

	  পক্ষ vpiAutomatic:
	    প্রত্যাগম array->get_scope()->is_automatic() ? 1 : 0;

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  পক্ষ _vpiFromThr:
	    প্রত্যাগম _vpi_at_A;
#endif

	  // If address_handle is not zero we definitely have a
	  // variable.
	  পক্ষ vpiConstantSelect:
	    প্রত্যাগম address_handle == 0;

	  স্বভাব:
	    প্রত্যাগম 0;
      }
}

বর্ণ* __vpiArrayVthrA::vpi_get_str(পূর্ণাঙ্ক code)
{
      যদি (code == vpiFile) {  // Not implemented for now!
            প্রত্যাগম simple_set_rbuf_str(file_names[0]);
      }

      বর্ণ sidx [64];
      snprintf(sidx, 63, "%d", (পূর্ণাঙ্ক)get_address() + array->first_addr.get_মান());
      প্রত্যাগম generic_get_str(code, array->get_scope(), array->name, sidx);
}

শূন্য __vpiArrayVthrA::vpi_get_মান(p_vpi_মান vp)
{
      প্রতিজ্ঞা(array);

      unsigned index = get_address();
      যদি (vpi_array_is_real(array)) {
	    দ্বিগুণ tmp = array->get_word_r(index);
	    vpip_real_get_মান(tmp, vp);
      } অন্যথা যদি (vpi_array_is_string(array)) {
	    string tmp = array->get_word_str(index);
	    vpip_string_get_মান(tmp, vp);
      } অন্যথা {
	    vvp_vector4_t tmp = array->get_word(index);
	    unsigned width = array->get_word_size();
	    vpip_vec4_get_মান(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_মান(p_vpi_মান vp, পূর্ণাঙ্ক)
{
      unsigned index = get_address();

      প্রতিজ্ঞা(array);
      প্রতিজ্ঞা(index < array->get_size());

      যদি (vpi_array_is_real(array)) {
	    দ্বিগুণ val = real_from_vpi_মান(vp);
	    array->set_word(index, val);
      } অন্যথা {
	    unsigned width = array->get_word_size();
	    vvp_vector4_t val = vec4_from_vpi_মান(vp, width);
	    array->set_word(index, 0, val);
      }

      প্রত্যাগম এতত্;
}

vpiHandle __vpiArrayVthrA::vpi_handle(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
	  পক্ষ vpiIndex:
	    ভঙ্গ;  // Not implemented!

	  পক্ষ vpi_বাম_Range:
	    প্রত্যাগম &array->msb;

	  পক্ষ vpi_ডান_Range:
	    প্রত্যাগম &array->lsb;

	  পক্ষ vpiParent:
	  পক্ষ vpiArray:
	    প্রত্যাগম array;

	  পক্ষ vpiScope:
	    প্রত্যাগম array->get_scope();

	  পক্ষ vpiModule:
	    প্রত্যাগম vpip_module(array->get_scope());
      }

      প্রত্যাগম 0;
}


পূর্ণাঙ্ক __vpiArrayVthrAPV::vpi_get(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
	  পক্ষ vpi_সারি_No:
	    প্রত্যাগম 0; // Not implemented for now!

	  পক্ষ vpiSize:
	    প্রত্যাগম part_wid;

	  পক্ষ vpi_বাম_Range:
	    প্রত্যাগম part_bit + part_wid - 1;

	  পক্ষ vpi_ডান_Range:
	    প্রত্যাগম part_bit;

	  পক্ষ vpiAutomatic:
	    প্রত্যাগম array->get_scope()->is_automatic() ? 1 : 0;

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  পক্ষ _vpiFromThr:
	    প্রত্যাগম _vpi_at_APV;
#endif

	  পক্ষ vpiConstantSelect:
	    প্রত্যাগম 1;

	  স্বভাব:
	    প্রত্যাগম 0;
      }
}

বর্ণ* __vpiArrayVthrAPV::vpi_get_str(পূর্ণাঙ্ক code)
{
      যদি (code == vpiFile) {  // Not implemented for now!
            প্রত্যাগম simple_set_rbuf_str(file_names[0]);
      }

      বর্ণ sidx [64];
      snprintf(sidx, 63, "%u", word_sel + array->first_addr.get_মান());
      প্রত্যাগম generic_get_str(code, array->get_scope(), array->name, sidx);
}

শূন্য __vpiArrayVthrAPV::vpi_get_মান(p_vpi_মান vp)
{
      প্রতিজ্ঞা(array);

      unsigned index = word_sel;
      যদি (vpi_array_is_real(array)) {
	    দ্বিগুণ tmp = array->get_word_r(index);
	    vpip_real_get_মান(tmp, vp);
      } অন্যথা {
	    vvp_vector4_t tmp = array->get_word(index);
	    tmp = tmp.subvalue(part_bit, part_wid);
	    vpip_vec4_get_মান(tmp, part_wid, অসত্য, vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  পক্ষ vpiParent:
	    প্রত্যাগম 0;

            // Not part of the Verilog standard. We use this internally.
	  পক্ষ vpiArray:
	    প্রত্যাগম array;

	  পক্ষ vpiScope:
	    প্রত্যাগম array->get_scope();

	  পক্ষ vpiModule:
	    প্রত্যাগম vpip_module(array->get_scope());
      }

      প্রত্যাগম 0;
}

শূন্য __vpiArray::set_word(unsigned address, unsigned part_off, ধ্রুব vvp_vector4_t&val)
{
      যদি (address >= get_size())
	    প্রত্যাগম;

      যদি (vals4) {
	    প্রতিজ্ঞা(nets == 0);
	    যদি (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp = vals4->get_word(address);
		  যদি ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			প্রতিজ্ঞা(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals4->set_word(address, tmp);
	    } অন্যথা {
		  vals4->set_word(address, val);
	    }
	    word_change(address);
	    প্রত্যাগম;
      }

      যদি (vals) {
	    প্রতিজ্ঞা(nets == 0);
	    যদি (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp;
		  vals->get_word(address, tmp);
		  যদি ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			প্রতিজ্ঞা(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals->set_word(address, tmp);
	    } অন্যথা {
		  vals->set_word(address, val);
	    }
	    word_change(address);
	    প্রত্যাগম;
      }

      প্রতিজ্ঞা(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle word = nets[address];
      ধ্রুব সংরচনা __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      প্রতিজ্ঞা(vsig);

      vsig->গ্রন্থি->send_vec4_pv(val, part_off, vpip_size(vsig), 0);
      word_change(address);
}

শূন্য __vpiArray::set_word(unsigned address, দ্বিগুণ val)
{
      প্রতিজ্ঞা(vals != 0);
      প্রতিজ্ঞা(nets == 0);

      যদি (address >= vals->get_size())
	    প্রত্যাগম;

      vals->set_word(address, val);
      word_change(address);
}

শূন্য __vpiArray::set_word(unsigned address, ধ্রুব string&val)
{
      প্রতিজ্ঞা(vals != 0);
      প্রতিজ্ঞা(nets == 0);

      যদি (address >= vals->get_size())
	    প্রত্যাগম;

      vals->set_word(address, val);
      word_change(address);
}

শূন্য __vpiArray::set_word(unsigned address, ধ্রুব vvp_object_t&val)
{
      প্রতিজ্ঞা(vals != 0);
      প্রতিজ্ঞা(nets == 0);

      যদি (address >= vals->get_size())
	    প্রত্যাগম;

      vals->set_word(address, val);
      word_change(address);
}

vvp_vector4_t __vpiArray::get_word(unsigned address)
{
      যদি (vals4) {
	    প্রতিজ্ঞা(nets == 0);
	    প্রতিজ্ঞা(vals == 0);
	    প্রত্যাগম vals4->get_word(address);
      }

      যদি (vals) {
	    প্রতিজ্ঞা(nets == 0);
	    প্রতিজ্ঞা(vals4== 0);
	    যদি (address >= vals->get_size())
		  প্রত্যাগম vvp_vector4_t(vals_width, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_word(address, val);
	    প্রত্যাগম val;
      }

      প্রতিজ্ঞা(vals4 == 0);
      প্রতিজ্ঞা(vals == 0);
      প্রতিজ্ঞা(nets != 0);

      যদি (address >= get_size()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    প্রতিজ্ঞা(get_size() > 0);
	    vpiHandle word = nets[0];
	    প্রতিজ্ঞা(word);
	    সংরচনা __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
	    প্রতিজ্ঞা(vsig);
	    vvp_signal_মান*sig = dynamic_cast<vvp_signal_মান*> (vsig->গ্রন্থি->fil);
	    প্রতিজ্ঞা(sig);
	    প্রত্যাগম vvp_vector4_t(sig->মান_size(), BIT4_X);
      }

      vpiHandle word = nets[address];
      সংরচনা __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      প্রতিজ্ঞা(vsig);
      vvp_signal_মান*sig = dynamic_cast<vvp_signal_মান*> (vsig->গ্রন্থি->fil);
      প্রতিজ্ঞা(sig);

      vvp_vector4_t val;
      sig->vec4_মান(val);
      প্রত্যাগম val;
}

দ্বিগুণ __vpiArray::get_word_r(unsigned address)
{
      যদি (vals) {
	    প্রতিজ্ঞা(vals4 == 0);
	    প্রতিজ্ঞা(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    যদি (address >= vals->get_size())
		  প্রত্যাগম 0.0;

	    দ্বিগুণ val;
	    vals->get_word(address, val);
	    প্রত্যাগম val;
      }

      যদি (address >= get_size())
	    প্রত্যাগম 0.0;

      প্রতিজ্ঞা(nets);
      vpiHandle word = nets[address];
      সংরচনা __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(word);
      প্রতিজ্ঞা(vsig);
      vvp_signal_মান*sig = dynamic_cast<vvp_signal_মান*> (vsig->net->fil);
      প্রতিজ্ঞা(sig);

      দ্বিগুণ val = sig->real_মান();
      প্রত্যাগম val;
}

শূন্য __vpiArray::get_word_obj(unsigned address, vvp_object_t&val)
{
      যদি (vals) {
	    প্রতিজ্ঞা(vals4 == 0);
	    প্রতিজ্ঞা(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    যদি (address >= vals->get_size()) {
		  val = vvp_object_t();
		  প্রত্যাগম;
	    }

	    vals->get_word(address, val);
	    প্রত্যাগম;
      }

      প্রতিজ্ঞা(nets);
	// Arrays of string nets not implemented!
      প্রতিজ্ঞা(0);
      প্রত্যাগম;
}

string __vpiArray::get_word_str(unsigned address)
{
      যদি (vals) {
	    প্রতিজ্ঞা(vals4 == 0);
	    প্রতিজ্ঞা(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    যদি (address >= vals->get_size())
		  প্রত্যাগম "";

	    string val;
	    vals->get_word(address, val);
	    প্রত্যাগম val;
      }

      প্রতিজ্ঞা(nets);
	// Arrays of string nets not implemented!
      প্রতিজ্ঞা(0);
      প্রত্যাগম "";
}

vpiHandle vpip_make_array(ধ্রুব বর্ণ*label, ধ্রুব বর্ণ*name,
                          পূর্ণাঙ্ক first_addr, পূর্ণাঙ্ক last_addr,
                          তার্কিক signed_flag)
{
      সংরচনা __vpiArray*obj = নূতন __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      যদি (last_addr >= first_addr) {
	    obj->swap_addr = অসত্য;
      } অন্যথা {
	    obj->swap_addr = সত্য;
	    পূর্ণাঙ্ক tmp = last_addr;
	    last_addr = first_addr;
	    first_addr = tmp;
      }
      প্রতিজ্ঞা(last_addr >= first_addr);
      unsigned array_গুনতি = last_addr+1-first_addr;

	// For now, treat all arrays as memories. This is not quite
	// correct, as arrays are arrays with memories a special case.
      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_গুনতি = array_গুনতি;

      obj->first_addr.set_মান(first_addr);
      obj->last_addr .set_মান(last_addr);

	// Start off now knowing if we are nets or variables.
      obj->nets = 0;
      obj->vals4 = 0;
      obj->vals  = 0;
      obj->vals_width = 0;
      obj->vals_শব্দগুলি = 0;

	// Initialize (clear) the read-ports list.
      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

	/* Add this symbol to the array_symbols table for later lookup. */
      যদি (!array_table)
	    array_table = নূতন প্রতীক_map_s<সংরচনা __vpiArray>;

      প্রতিজ্ঞা(!array_find(label));
      array_table->sym_set_মান(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_প্রতীক(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_to_current_scope(obj);

      প্রত্যাগম obj;
}

শূন্য __vpiArray::alias_word(unsigned long addr, vpiHandle word, পূর্ণাঙ্ক msb_, পূর্ণাঙ্ক lsb_)
{
      প্রতিজ্ঞা(msb.get_মান() == msb_);
      প্রতিজ্ঞা(lsb.get_মান() == lsb_);
      প্রতিজ্ঞা(addr < get_size());
      প্রতিজ্ঞা(nets);
      nets[addr] = word;
}

শূন্য __vpiArray::attach_word(unsigned addr, vpiHandle word)
{
      প্রতিজ্ঞা(addr < get_size());
      প্রতিজ্ঞা(nets);
      nets[addr] = word;

      যদি (সংরচনা __vpiSignal*sig = dynamic_cast<__vpiSignal*>(word)) {
	    vvp_net_t*net = sig->গ্রন্থি;
	    প্রতিজ্ঞা(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    প্রতিজ্ঞা(fun);
	    fun->attach_as_word(এতত্, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = এতত্;
	    sig->id.index = নূতন __vpiDecConst(addr + first_addr.get_মান());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    প্রত্যাগম;
      }

      যদি (সংরচনা __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(word)) {
	    vvp_net_t*net = sig->net;
	    প্রতিজ্ঞা(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    প্রতিজ্ঞা(fun);
	    fun->attach_as_word(এতত্, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = এতত্;
	    sig->id.index = নূতন __vpiDecConst(addr + first_addr.get_মান());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = সত্য;
	    প্রত্যাগম;
      }
}

শূন্য compile_var_array(বর্ণ*label, বর্ণ*name, পূর্ণাঙ্ক last, পূর্ণাঙ্ক first,
		   পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb, বর্ণ signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last,
                                      signed_flag != 0);

      সংরচনা __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_width = labs(msb-lsb) + 1;
      যদি (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = নূতন vvp_vector4array_aa(arr->vals_width,
						 arr->get_size());
      } অন্যথা {
            arr->vals4 = নূতন vvp_vector4array_sa(arr->vals_width,
						 arr->get_size());
      }
      arr->msb.set_মান(msb);
      arr->lsb.set_মান(lsb);

      গুনতি_var_arrays += 1;
      গুনতি_var_array_শব্দগুলি += arr->get_size();

      মুক্ত(label);
      delete[] name;
}

শূন্য compile_var2_array(বর্ণ*label, বর্ণ*name, পূর্ণাঙ্ক last, পূর্ণাঙ্ক first,
		   পূর্ণাঙ্ক msb, পূর্ণাঙ্ক lsb, তার্কিক signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, signed_flag);

      সংরচনা __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.set_মান(msb);
      arr->lsb.set_মান(lsb);
      arr->vals_width = labs(msb-lsb) + 1;

      প্রতিজ্ঞা(! arr->nets);
      যদি (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = নূতন vvp_darray_atom<int8_t>(arr->get_size());
      } অন্যথা যদি (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = নূতন vvp_darray_atom<uint8_t>(arr->get_size());
      } অন্যথা যদি (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = নূতন vvp_darray_atom<int16_t>(arr->get_size());
      } অন্যথা যদি (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = নূতন vvp_darray_atom<uint16_t>(arr->get_size());
      } অন্যথা যদি (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = নূতন vvp_darray_atom<int32_t>(arr->get_size());
      } অন্যথা যদি (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = নূতন vvp_darray_atom<uint32_t>(arr->get_size());
      } অন্যথা যদি (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = নূতন vvp_darray_atom<int64_t>(arr->get_size());
      } অন্যথা যদি (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = নূতন vvp_darray_atom<uint64_t>(arr->get_size());
      } অন্যথা {
	    arr->vals = নূতন vvp_darray_vec2(arr->get_size(), arr->vals_width);
      }
      গুনতি_var_arrays += 1;
      গুনতি_var_array_শব্দগুলি += arr->get_size();

      মুক্ত(label);
      delete[] name;
}

শূন্য compile_real_array(বর্ণ*label, বর্ণ*name, পূর্ণাঙ্ক last, পূর্ণাঙ্ক first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, সত্য);

      সংরচনা __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = নূতন vvp_darray_real(arr->get_size());
      arr->vals_width = 1;

      গুনতি_real_arrays += 1;
      গুনতি_real_array_শব্দগুলি += arr->get_size();

      মুক্ত(label);
      delete[] name;
}

শূন্য compile_string_array(বর্ণ*label, বর্ণ*name, পূর্ণাঙ্ক last, পূর্ণাঙ্ক first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, সত্য);

      সংরচনা __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = নূতন vvp_darray_string(arr->get_size());
      arr->vals_width = 1;

      গুনতি_real_arrays += 1;
      গুনতি_real_array_শব্দগুলি += arr->get_size();

      মুক্ত(label);
      delete[] name;
}

শূন্য compile_object_array(বর্ণ*label, বর্ণ*name, পূর্ণাঙ্ক last, পূর্ণাঙ্ক first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, সত্য);

      সংরচনা __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = নূতন vvp_darray_object(arr->get_size());
      arr->vals_width = 1;

      গুনতি_real_arrays += 1;
      গুনতি_real_array_শব্দগুলি += arr->get_size();

      মুক্ত(label);
      delete[] name;
}

শূন্য compile_net_array(বর্ণ*label, বর্ণ*name, পূর্ণাঙ্ক last, পূর্ণাঙ্ক first)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, first, last, অসত্য);

      সংরচনা __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(calloc(arr->get_size(), পরিমাণ(vpiHandle)));

      গুনতি_net_arrays += 1;
      গুনতি_net_array_শব্দগুলি += arr->get_size();

      মুক্ত(label);
      delete[] name;
}

বর্গ vvp_fun_arrayport  : সার্বজনিক vvp_net_fun_t {

    সার্বজনিক:
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport() override;

      আভাস শূন্য check_word_change(unsigned long addr) = 0;

    রক্ষিত:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      unsigned long addr_;

      friend শূন্য array_attach_port(vvp_array_t, vvp_fun_arrayport*);
      friend শূন্য __vpiArray::word_change(unsigned long);
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

বর্গ vvp_fun_arrayport_sa  : সার্বজনিক vvp_fun_arrayport {

    সার্বজনিক:
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_sa() override;

      শূন্য check_word_change(unsigned long addr) override;

      শূন্য recv_vec4(vvp_net_ptr_t port, ধ্রুব vvp_vector4_t&bit,
                     vvp_context_t) override;

    গুপ্ত:
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

শূন্য vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t port, ধ্রুব vvp_vector4_t&bit,
                                     vvp_context_t)
{
      তার্কিক addr_valid_flag;

      বিকল্প (port.port()) {

	  পক্ষ 0: // Address input
	    addr_valid_flag = vector4_to_মান(bit, addr_);
	    যদি (! addr_valid_flag)
		  addr_ = arr_->get_size();
	    যদি (vpi_array_is_real(arr_))
		  port.ptr()->send_real(arr_->get_word_r(addr_), 0);
	    অন্যথা
		  port.ptr()->send_vec4(arr_->get_word(addr_), 0);

	    ভঙ্গ;

	  স্বভাব:
	    fprintf(stdout, "XXXX write ports not implemented.\n");
	    প্রতিজ্ঞা(0);
      }
}

শূন্য vvp_fun_arrayport_sa::check_word_change(unsigned long addr)
{
      যদি (addr != addr_) প্রত্যাগম;

      যদি (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr_), 0);
      } অন্যথা {
	    net_->send_vec4(arr_->get_word(addr_), 0);
      }
}

বর্গ vvp_fun_arrayport_aa  : সার্বজনিক vvp_fun_arrayport, সার্বজনিক automatic_hooks_s {

    সার্বজনিক:
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_aa() override;

      শূন্য alloc_instance(vvp_context_t context) override;
      শূন্য reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      শূন্য free_instance(vvp_context_t context) override;
#endif

      শূন্য check_word_change(unsigned long addr) override;

      শূন্য recv_vec4(vvp_net_ptr_t port, ধ্রুব vvp_vector4_t&bit,
                     vvp_context_t context) override;

    গুপ্ত:
      শূন্য check_word_change_(unsigned long addr, vvp_context_t context);

      __vpiScope*context_scope_;
      unsigned context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(এতত্, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(এতত্, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

শূন্য vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      unsigned long*addr = নূতন unsigned long;
      vvp_set_context_item(context, context_idx_, addr);

      *addr = addr_;
}

শূন্য vvp_fun_arrayport_aa::reset_instance(vvp_context_t context)
{
      unsigned long*addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));

      *addr = addr_;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য vvp_fun_arrayport_aa::free_instance(vvp_context_t context)
{
      unsigned long*addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));
      delete addr;
}
#endif

শূন্য vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t port, ধ্রুব vvp_vector4_t&bit,
                                     vvp_context_t context)
{
      যদি (context) {
            unsigned long*addr = static_cast<unsigned long*>
                  (vvp_get_context_item(context, context_idx_));

            তার্কিক addr_valid_flag;

            বিকল্প (port.port()) {

                পক্ষ 0: // Address input
                  addr_valid_flag = vector4_to_মান(bit, *addr);
                  যদি (! addr_valid_flag) *addr = arr_->get_size();
                  যদি (vpi_array_is_real(arr_)) {
			port.ptr()->send_real(arr_->get_word_r(*addr),
					      context);
                  } অন্যথা {
			port.ptr()->send_vec4(arr_->get_word(*addr),
					      context);
                  }
                  ভঙ্গ;

                স্বভাব:
                  fprintf(stdout, "XXXX write ports not implemented.\n");
                  প্রতিজ্ঞা(0);
            }
      } অন্যথা {
            context = context_scope_->live_contexts;
            যাবত্ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

শূন্য vvp_fun_arrayport_aa::check_word_change_(unsigned long addr,
                                              vvp_context_t context)
{
      ধ্রুব unsigned long*port_addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));

      যদি (addr != *port_addr)
	    প্রত্যাগম;

      যদি (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr), context);
      } অন্যথা {
	    net_->send_vec4(arr_->get_word(addr), context);
      }
}

শূন্য vvp_fun_arrayport_aa::check_word_change(unsigned long addr)
{
      যদি (arr_->get_scope()->is_automatic()) {
            প্রতিজ্ঞা(vthread_get_wt_context());
            check_word_change_(addr, vthread_get_wt_context());
      } অন্যথা {
            vvp_context_t context = context_scope_->live_contexts;
            যাবত্ (context) {
                  check_word_change_(addr, context);
                  context = vvp_get_next_context(context);
            }
      }
}

স্থির শূন্য array_attach_port(vvp_array_t array, vvp_fun_arrayport*fun)
{
      প্রতিজ্ঞা(fun->next_ == 0);
      fun->next_ = array->ports_;
      array->ports_ = fun;
      যদি (!array->get_scope()->is_automatic() &&
          (array->vals4 || array->vals)) {
              /* propagate initial values for variable arrays */
            যদি (!vpi_array_is_real(array)) {
                  vvp_bit4_t init;
                  যদি (array->vals4)
                      init = BIT4_X;
                  অন্যথা
                      init = BIT4_0;
                  vvp_vector4_t tmp(array->vals_width, init);
                  schedule_init_propagate(fun->net_, tmp);
            } অন্যথা {
                  schedule_init_propagate(fun->net_, 0.0);
            }
      }
}

বর্গ array_word_মান_callback : সার্বজনিক মান_callback {
    সার্বজনিক:
      অন্তঃস্থ explicit array_word_মান_callback(p_cb_data data, long addr)
      : মান_callback(data), word_addr(addr)
      { }

    সার্বজনিক:
      long word_addr;
};

শূন্য __vpiArray::word_change(unsigned long addr)
{
      প্রতি (vvp_fun_arrayport*cur = ports_; cur; cur = cur->next_)
	    cur->check_word_change(addr);

	// Run callbacks attached to the array itself.
      সংরচনা __vpiCallback *next = vpi_callbacks;
      সংরচনা __vpiCallback *prev = 0;

      যাবত্ (next) {
	    array_word_মান_callback*cur = dynamic_cast<array_word_মান_callback*>(next);
	    next = cur->next;

	      // Skip callbacks that are not for me. -1 is for every element.
	    যদি (cur->word_addr != (long)addr && cur->word_addr != -1) {
		  prev = cur;
		  অনুবর্ত;
	    }

	      // For whole array callbacks we need to set the index.
	    যদি (cur->word_addr == -1) {
		  cur->cb_data.index = (PLI_INT32) ((পূর্ণাঙ্ক)addr +
						    first_addr.get_মান());
	    }

	    যদি (cur->cb_data.cb_rtn != 0) {
		  যদি (cur->test_মান_callback_ready()) {
			যদি (cur->cb_data.value) {
			      যদি (vpi_array_is_real(এতত্)) {
				    দ্বিগুণ val = 0.0;
				    যদি (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_real_get_মান(val, cur->cb_data.value);
			      } অন্যথা যদি (vals4) {
				    vpip_vec4_get_মান(vals4->get_word(addr),
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } অন্যথা যদি (dynamic_cast<vvp_darray_atom<int8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int64_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint64_t>*>(vals)
				      || dynamic_cast<vvp_darray_vec2*>(vals)) {
				    vvp_vector4_t val;
				    যদি (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_vec4_get_মান(val,
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } অন্যথা {
			            প্রতিজ্ঞা(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } অন্যথা যদি (prev == 0) {

		  vpi_callbacks = next;
		  cur->next = 0;
		  delete cur;

	    } অন্যথা {
		  প্রতিজ্ঞা(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}

বর্গ array_resolv_list_t : সার্বজনিক resolv_list_s {

    সার্বজনিক:
      explicit array_resolv_list_t(বর্ণ*lab) : resolv_list_s(lab) {
	    array = 0;
      }

      vvp_array_t*array;
      তার্কিক resolve(তার্কিক mes) override;

    গুপ্ত:
};

তার্কিক array_resolv_list_t::resolve(তার্কিক mes)
{
      *array = array_find(label());
      যদি (*array == 0) {
	    প্রতিজ্ঞা(!mes);
	    প্রত্যাগম অসত্য;
      }
      প্রত্যাগম সত্য;
}

বর্গ array_port_resolv_list_t : সার্বজনিক resolv_list_s {

    সার্বজনিক:
      explicit array_port_resolv_list_t(বর্ণ* lab, তার্কিক use_addr__,
                                        long addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      তার্কিক use_addr;
      long addr;
      তার্কিক resolve(তার্কিক mes) override;

    গুপ্ত:
};

array_port_resolv_list_t::array_port_resolv_list_t(বর্ণ *lab, তার্কিক use_addr__,
                                                   long addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      যদি (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      অন্যথা
            context_scope = 0;
      ptr = নূতন vvp_net_t;
}

তার্কিক array_port_resolv_list_t::resolve(তার্কিক mes)
{
      vvp_array_t mem = array_find(label());
      যদি (mem == 0) {
	    প্রতিজ্ঞা(mem || !mes);
	    প্রত্যাগম অসত্য;
      }

      vvp_fun_arrayport*fun;
      যদি (use_addr)
            যদি (context_scope)
                  fun = নূতন vvp_fun_arrayport_aa(context_scope, mem, ptr, addr);
            অন্যথা
                  fun = নূতন vvp_fun_arrayport_sa(mem, ptr, addr);
      অন্যথা
            যদি (context_scope)
                  fun = নূতন vvp_fun_arrayport_aa(context_scope, mem, ptr);
            অন্যথা
                  fun = নূতন vvp_fun_arrayport_sa(mem, ptr);
      ptr->fun = fun;

      array_attach_port(mem, fun);

      প্রত্যাগম সত্য;
}

বর্গ array_word_part_callback : সার্বজনিক array_word_মান_callback {
    সার্বজনিক:
      explicit array_word_part_callback(p_cb_data data, long addr);
      ~array_word_part_callback() override;

      array_word_part_callback(ধ্রুব array_word_part_callback&) = delete;
      array_word_part_callback& operator=(ধ্রুব array_word_part_callback&) = delete;

      তার্কিক test_মান_callback_ready(শূন্য) override;

    গুপ্ত:
      বর্ণ*মান_bits;
};

array_word_part_callback::array_word_part_callback(p_cb_data data, long addr)
: array_word_মান_callback(data, addr)
{
	// Get the initial value of the part, to use as a reference.
      সংরচনা __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj);
      s_vpi_মান tmp_মান;
      tmp_মান.format = vpiBinStrVal;
      apvword->vpi_get_মান(&tmp_মান);

      মান_bits = নূতন বর্ণ[apvword->part_wid+1];

      memcpy(মান_bits, tmp_মান.value.str, apvword->part_wid);
      মান_bits[apvword->part_wid] = 0;
}

array_word_part_callback::~array_word_part_callback()
{
      delete[]মান_bits;
}

তার্কিক array_word_part_callback::test_মান_callback_ready(শূন্য)
{
      সংরচনা __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_data.obj);
      প্রতিজ্ঞা(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_মান tmp_মান;
      tmp_মান.format = vpiBinStrVal;
      apvword->vpi_get_মান(&tmp_মান);

      যদি (memcmp(মান_bits, tmp_মান.value.str, apvword->part_wid) == 0)
	    প্রত্যাগম অসত্য;

      memcpy(মান_bits, tmp_মান.value.str, apvword->part_wid);
      প্রত্যাগম সত্য;

}

মান_callback*vpip_array_word_change(p_cb_data data)
{
      সংরচনা __vpiArray*parent = 0;
      array_word_মান_callback*cbh = 0;
      যদি (ধ্রুব সংরচনা __vpiArrayWord*word = array_var_word_from_handle(data->obj)) {
	    parent = static_cast<__vpiArray*>(word->get_parent());
	    unsigned addr = word->get_index();
	    cbh = নূতন array_word_মান_callback(data, addr);

      } অন্যথা যদি (সংরচনা __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(data->obj)) {
	    parent = tword->array;
	    cbh = নূতন array_word_মান_callback(data, tword->address);

      } অন্যথা যদি (সংরচনা __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj)) {
	    parent = apvword->array;
	    cbh = নূতন array_word_part_callback(data, apvword->word_sel);
      }

      প্রতিজ্ঞা(cbh);
      প্রতিজ্ঞা(parent);
      cbh->next = parent->vpi_callbacks;
      parent->vpi_callbacks = cbh;

      প্রত্যাগম cbh;
}

মান_callback* vpip_array_change(p_cb_data data)
{
      array_word_মান_callback*cbh = নূতন array_word_মান_callback(data, -1);
      প্রতিজ্ঞা(data->obj);

      সংরচনা __vpiArray*arr = dynamic_cast<__vpiArray*>(data->obj);
      cbh->next = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      প্রত্যাগম cbh;
}

শূন্য compile_array_port(বর্ণ*label, বর্ণ*array, বর্ণ*addr)
{
      array_port_resolv_list_t*resolv_mem
	    = নূতন array_port_resolv_list_t(array, অসত্য, 0);

      define_functor_প্রতীক(label, resolv_mem->ptr);
      মুক্ত(label);
	// Connect the port-0 input as the address.
      প্রবেশ_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

শূন্য compile_array_port(বর্ণ*label, বর্ণ*array, long addr)
{
      array_port_resolv_list_t*resolv_mem
	    = নূতন array_port_resolv_list_t(array, সত্য, addr);

      define_functor_প্রতীক(label, resolv_mem->ptr);
      মুক্ত(label);

      resolv_submit(resolv_mem);
}

শূন্য compile_array_alias(বর্ণ*label, বর্ণ*name, বর্ণ*src)
{
      vvp_array_t mem = array_find(src);
      প্রতিজ্ঞা(mem);

      সংরচনা __vpiArray*obj = নূতন __vpiArray;

      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_গুনতি = mem->array_গুনতি;
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
      obj->vals_শব্দগুলি = mem->vals_শব্দগুলি;

      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

      প্রতিজ্ঞা(array_table);
      প্রতিজ্ঞা(!array_find(label));
      array_table->sym_set_মান(label, obj);

      compile_vpi_প্রতীক(label, obj);
      vpip_attach_to_current_scope(obj);

      মুক্ত(label);
      মুক্ত(name);
      মুক্ত(src);
}

/*
 * &A<label,addr>
 * This represents a VPI handle for an addressed array. This comes
 * from expressions like "label[addr]" where "label" is the array and
 * "addr" is the canonical address of the desired word.
 */
vpiHandle vpip_make_vthr_A(বর্ণ*label, unsigned addr)
{
      সংরচনা __vpiArrayVthrA*obj = নূতন __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = নূতন array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      obj->address = addr;

      প্রত্যাগম obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(বর্ণ*label, বর্ণ*প্রতীক)
{
      সংরচনা __vpiArrayVthrA*obj = নূতন __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = নূতন array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      compile_vpi_lookup(&obj->address_handle, প্রতীক);
      obj->address = 0;

      প্রত্যাগম obj;
}

vpiHandle vpip_make_vthr_A(বর্ণ*label, vpiHandle handle)
{
      সংরচনা __vpiArrayVthrA*obj = নূতন __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = নূতন array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = handle;
      obj->address = 0;

      প্রত্যাগম obj;
}

vpiHandle vpip_make_vthr_APV(বর্ণ*label, unsigned index, unsigned bit, unsigned wid)
{
      সংরচনা __vpiArrayVthrAPV*obj = নূতন __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = নূতন array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->word_sel = index;
      obj->part_bit = bit;
      obj->part_wid = wid;

      প্রত্যাগম obj;
}

শূন্য compile_array_cleanup(শূন্য)
{
      delete array_table;
      array_table = 0;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য memory_delete(vpiHandle item)
{
      সংরচনা __vpiArray*arr = static_cast<সংরচনা __vpiArray*>(item);
      যদি (arr->vals_শব্দগুলি) delete [] (arr->vals_শব্দগুলি-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals;

      যদি (arr->nets) {
	    প্রতি (unsigned idx = 0; idx < arr->get_size(); idx += 1) {
		  যদি (সংরচনা __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.index);
		    /* These should only be the real words. */
		  } অন্যথা {
			প্রতিজ্ঞা(arr->nets[idx]->get_type_code() ==
			       vpiRealVar);
			সংরচনা __vpiRealVar *sigr = static_cast<সংরচনা __vpiRealVar *>
			                            (arr->nets[idx]);
			constant_delete(sigr->id.index);
// Why are only the real words still here?
			delete arr->nets[idx];
		  }
	    }
	    মুক্ত(arr->nets);
      }

      যাবত্ (arr->vpi_callbacks) {
	    সংরচনা __vpiCallback*tmp = arr->vpi_callbacks->next;
	    delete arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      delete arr;
}

শূন্য A_delete(vpiHandle item)
{
      সংরচনা __vpiArrayVthrA*obj = static_cast<সংরচনা __vpiArrayVthrA*>(item);
      যদি (obj->address_handle) {
	    বিকল্প (obj->address_handle->get_type_code()) {
		পক্ষ vpiMemoryWord:
		  যদি (vpi_get(_vpiFromThr, obj->address_handle) == _vpi_at_A) {
			A_delete(obj->address_handle);
		  }
		  ভঙ্গ;
		পক্ষ vpiPartSelect:
		  প্রতিজ্ঞা(vpi_get(_vpiFromThr, obj->address_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->address_handle);
		  ভঙ্গ;
	    }
      }

      delete obj;
}

শূন্য APV_delete(vpiHandle item)
{
      সংরচনা __vpiArrayVthrAPV*obj = static_cast<সংরচনা __vpiArrayVthrAPV*>(item);
      delete obj;
}
#endif
