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

স্থির প্রতীক_map_s<কাঠামো __vpiArray>* array_table =0;

শ্রেণী vvp_fun_arrayport;
স্থির শূন্য array_attach_port(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_find(ধ্রুবক অক্ষর*label)
{
      যদি (array_table == 0)
	    ফেরত 0;

      vvp_array_t v = array_table->sym_get_মান(label);
      ফেরত v;
}

কাঠামো __vpiArrayVthrA : সর্বজনীন __vpiHandle {
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiMemoryWord; }
      পূর্ণসংখ্যা vpi_get(পূর্ণসংখ্যা code) override;
      অক্ষর* vpi_get_str(পূর্ণসংখ্যা code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;
      vpiHandle vpi_put_মান(p_vpi_মান val, পূর্ণসংখ্যা flags) override;
      vpiHandle vpi_handle(পূর্ণসংখ্যা code) override;

      কাঠামো __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle address_handle;
	// If wid==0, then address is the address into the array.
      unsigned address;

      unsigned get_address() ধ্রুবক
      {
	    যদি (address_handle) {
		  s_vpi_মান vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpiVectorVal;
		  address_handle->vpi_get_মান(&vp);
		  পূর্ণসংখ্যা শব্দগুলি = (address_handle->vpi_get(vpi_আকার)-1)/32 + 1;
		  জন্য(পূর্ণসংখ্যা idx = 0; idx < শব্দগুলি; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			যদি (vp.value.vector[idx].bval != 0) ফেরত UINT_MAX;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  address_handle->vpi_get_মান(&vp);
		  ফেরত vp.value.integer;
	    }

	    ফেরত address;
      }
};


কাঠামো __vpiArrayVthrAPV : সর্বজনীন __vpiHandle {
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiPartSelect; }
      পূর্ণসংখ্যা vpi_get(পূর্ণসংখ্যা code) override;
      অক্ষর* vpi_get_str(পূর্ণসংখ্যা code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;
      vpiHandle vpi_handle(পূর্ণসংখ্যা code) override;

      কাঠামো __vpiArray*array;
      unsigned word_sel;
      unsigned part_bit;
      unsigned part_wid;
};

যৌক্তিক is_net_array(vpiHandle obj)
{
      কাঠামো __vpiArray*rfp = dynamic_cast<__vpiArray*> (obj);
      প্রতিজ্ঞা(rfp);

      যদি (rfp->nets != 0) ফেরত সত্য;
      ফেরত মিথ্যা;
}

// This function return true if the underlying array words are real.
স্থির যৌক্তিক vpi_array_is_real(ধ্রুবক vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      যদি (arr->vals4 != 0)  // A bit based variable/register array.
	    ফেরত মিথ্যা;

      যদি (dynamic_cast<vvp_darray_real*> (arr->vals))
	    ফেরত সত্য;

      যদি (arr->vals != 0)
	    ফেরত মিথ্যা;

	// This must be a net array so look at element 0 to find the type.
      প্রতিজ্ঞা(arr->nets != 0);
      প্রতিজ্ঞা(arr->get_আকার() > 0);
      ধ্রুবক কাঠামো __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      যদি (rsig) {
	    ফেরত সত্য;
      }

      ফেরত মিথ্যা;
}

স্থির যৌক্তিক vpi_array_is_string(ধ্রুবক vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      যদি (arr->vals4 != 0)  // A bit based variable/register array.
	    ফেরত মিথ্যা;

      যদি (dynamic_cast<vvp_darray_string*> (arr->vals))
	    ফেরত সত্য;

      ফেরত মিথ্যা;
}

পূর্ণসংখ্যা __vpiArray::get_word_আকার() ধ্রুবক
{
      unsigned width;

      প্রতিজ্ঞা(get_আকার() > 0);
	/* For a net array we need to get the width from the first element. */
      যদি (nets) {
	    প্রতিজ্ঞা(vals4 == 0 && vals == 0);
	    ধ্রুবক কাঠামো __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    প্রতিজ্ঞা(vsig);
	    width = vpip_আকার(vsig);
	/* For a variable array we can get the width from vals_width. */
      } অন্যথা {
	    প্রতিজ্ঞা(vals4 || vals);
	    width = vals_width;
      }

      ফেরত width;
}

অক্ষর*__vpiArray::get_word_str(কাঠামো __vpiArrayWord*word, পূর্ণসংখ্যা code)
{
      unsigned সূচক = word->get_সূচক();

      যদি (code == vpiFile) {  // Not implemented for now!
	    ফেরত simple_set_rbuf_str(file_names[0]);
      }

      অক্ষর sidx [64];
      snprintf(sidx, 63, "%d", (পূর্ণসংখ্যা)সূচক + first_addr.get_মান());
      ফেরত generic_get_str(code, get_scope(), name, sidx);
}

শূন্য __vpiArray::get_word_মান(কাঠামো __vpiArrayWord*word, p_vpi_মান vp)
{
      unsigned সূচক = word->get_সূচক();

    // Determine the appropriate format (The Verilog PLI Handbook 5.2.10)
      যদি(vp->format == vpiObjTypeVal) {
          যদি(vpi_array_is_real(এই))
              vp->format = vpiRealVal;
          অন্যথা যদি(vpi_array_is_string(এই))
              vp->format = vpiStringVal;
          অন্যথা
              vp->format = vpiIntVal;
      }

      যদি(vals4) {
          vpip_vec4_get_মান(vals4->get_word(সূচক),
                              vals4->width(), signed_flag, vp);
      } অন্যথা যদি(vals) {
          নির্বাচন(vp->format) {
            ক্ষেত্র vpiBinStrVal:
            ক্ষেত্র vpiOctStrVal:
            ক্ষেত্র vpiDecStrVal:
            ক্ষেত্র vpiHexStrVal:
            ক্ষেত্র vpiScalarVal:
            ক্ষেত্র vpiIntVal:
            ক্ষেত্র vpiVectorVal:
            {
                vvp_vector4_t v;
                vals->get_word(সূচক, v);
                vpip_vec4_get_মান(v, vals_width, signed_flag, vp);
            }
            বিরতি;

            ক্ষেত্র vpiRealVal:
            {
                দ্বিগুণ d;
                vals->get_word(সূচক, d);
                vpip_real_get_মান(d, vp);
            }
            বিরতি;

            ক্ষেত্র vpiStringVal:
            {
                string s;
                vals->get_word(সূচক, s);
                vpip_string_get_মান(s, vp);
            }
            বিরতি;

            পূর্বনির্ধারিত:
                fprintf(stderr, "vpi sorry: format is not implemented\n");
                প্রতিজ্ঞা(মিথ্যা);
          }
      }
}

শূন্য __vpiArray::put_word_মান(কাঠামো __vpiArrayWord*word, p_vpi_মান vp, পূর্ণসংখ্যা)
{
      unsigned সূচক = word->get_সূচক();
      vvp_vector4_t val = vec4_from_vpi_মান(vp, vals_width);
      set_word(সূচক, 0, val);
}

vpiHandle __vpiArray::get_iter_সূচক(কাঠামো __vpiArrayIterator*, পূর্ণসংখ্যা idx)
{
      যদি (nets) ফেরত nets[idx];

      প্রতিজ্ঞা(vals4 || vals);

      যদি (vals_শব্দগুলি == 0) make_vals_শব্দগুলি();

      ফেরত &(vals_শব্দগুলি[idx].as_word);
}

পূর্ণসংখ্যা __vpiArray::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpi_সারি_No:
	    ফেরত 0; // Not implemented for now!

	  ক্ষেত্র vpi_আকার:
	    ফেরত get_আকার();

	  ক্ষেত্র vpiAutomatic:
	    ফেরত scope->is_automatic()? 1 : 0;

	  পূর্বনির্ধারিত:
	    ফেরত 0;
      }
}

অক্ষর* __vpiArray::vpi_get_str(পূর্ণসংখ্যা code)
{
      যদি (code == vpiFile) {  // Not implemented for now!
            ফেরত simple_set_rbuf_str(file_names[0]);
      }

      ফেরত generic_get_str(code, scope, name, ফাঁকা);
}

vpiHandle __vpiArray::vpi_handle(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
          ক্ষেত্র vpi_বাম_Range:
            যদি (swap_addr) ফেরত &last_addr;
            অন্যথা ফেরত &first_addr;

          ক্ষেত্র vpi_ডান_Range:
            যদি (swap_addr) ফেরত &first_addr;
            অন্যথা ফেরত &last_addr;

	  ক্ষেত্র vpiScope:
	    ফেরত scope;

	  ক্ষেত্র vpiModule:
	    ফেরত vpip_module(scope);
      }

      ফেরত 0;
}

/*
* VPI code passes indices that are not yet converted to canonical
* form, so this index function does it here.
*/
vpiHandle __vpiArray::vpi_সূচক(পূর্ণসংখ্যা সূচক)
{
      সূচক -= first_addr.get_মান();
      যদি (সূচক >= (long) get_আকার())
	    ফেরত 0;
      যদি (সূচক < 0)
	    ফেরত 0;

      যদি (nets != 0) {
	    ফেরত nets[সূচক];
      }

      যদি (vals_শব্দগুলি == 0)
	    make_vals_শব্দগুলি();

      ফেরত &(vals_শব্দগুলি[সূচক].as_word);
}

পূর্ণসংখ্যা __vpiArrayWord::as_word_t::vpi_get(পূর্ণসংখ্যা code)
{
      কাঠামো __vpiArrayWord*obj = array_var_word_from_handle(এই);
      প্রতিজ্ঞা(obj);
      কাঠামো __vpiArrayBase*my_parent = obj->get_parent();
      t_vpi_মান val;

      নির্বাচন (code) {
	  ক্ষেত্র vpi_সারি_No:
	    ফেরত 0; // Not implemented for now!

	  ক্ষেত্র vpi_আকার:
	    ফেরত my_parent->get_word_আকার();

	  ক্ষেত্র vpi_বাম_Range:
            val.format = vpiIntVal;
	    my_parent->get_বাম_range()->vpi_get_মান(&val);
            প্রতিজ্ঞা(val.format == vpiIntVal);
            ফেরত val.value.integer;

	  ক্ষেত্র vpi_ডান_Range:
            val.format = vpiIntVal;
	    my_parent->get_ডান_range()->vpi_get_মান(&val);
            প্রতিজ্ঞা(val.format == vpiIntVal);
            ফেরত val.value.integer;

	  ক্ষেত্র vpi_সূচক_2:
	    {
		  পূর্ণসংখ্যা base_offset = 0;
		  কাঠামো __vpiArray*base = dynamic_cast<__vpiArray*> (my_parent);
		  যদি (base) {
			val.format = vpiIntVal;
			base->first_addr.vpi_get_মান(&val);
			base_offset += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_সূচক.vpi_get_মান(&val);
		  প্রতিজ্ঞা(val.format == vpiIntVal);
		  ফেরত val.value.integer + base_offset;
	    }

	  ক্ষেত্র vpiAutomatic:
	    ফেরত my_parent->get_scope()->is_automatic()? 1 : 0;

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ক্ষেত্র _vpiFromThr:
	    ফেরত _vpiNoThr;
#endif

	  পূর্বনির্ধারিত:
	    ফেরত 0;
      }
}


পূর্ণসংখ্যা __vpiArrayVthrA::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpi_সারি_No:
	    ফেরত 0; // Not implemented for now!

	  ক্ষেত্র vpi_আকার:
	    ফেরত array->get_word_আকার();

	  ক্ষেত্র vpi_বাম_Range:
	    ফেরত array->msb.get_মান();

	  ক্ষেত্র vpi_ডান_Range:
	    ফেরত array->lsb.get_মান();

	  ক্ষেত্র vpi_সূচক_2:
	    ফেরত (পূর্ণসংখ্যা)get_address() + array->first_addr.get_মান();

	  ক্ষেত্র vpiAutomatic:
	    ফেরত array->get_scope()->is_automatic() ? 1 : 0;

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ক্ষেত্র _vpiFromThr:
	    ফেরত _vpi_at_A;
#endif

	  // If address_handle is not zero we definitely have a
	  // variable.
	  ক্ষেত্র vpiConstantSelect:
	    ফেরত address_handle == 0;

	  পূর্বনির্ধারিত:
	    ফেরত 0;
      }
}

অক্ষর* __vpiArrayVthrA::vpi_get_str(পূর্ণসংখ্যা code)
{
      যদি (code == vpiFile) {  // Not implemented for now!
            ফেরত simple_set_rbuf_str(file_names[0]);
      }

      অক্ষর sidx [64];
      snprintf(sidx, 63, "%d", (পূর্ণসংখ্যা)get_address() + array->first_addr.get_মান());
      ফেরত generic_get_str(code, array->get_scope(), array->name, sidx);
}

শূন্য __vpiArrayVthrA::vpi_get_মান(p_vpi_মান vp)
{
      প্রতিজ্ঞা(array);

      unsigned সূচক = get_address();
      যদি (vpi_array_is_real(array)) {
	    দ্বিগুণ tmp = array->get_word_r(সূচক);
	    vpip_real_get_মান(tmp, vp);
      } অন্যথা যদি (vpi_array_is_string(array)) {
	    string tmp = array->get_word_str(সূচক);
	    vpip_string_get_মান(tmp, vp);
      } অন্যথা {
	    vvp_vector4_t tmp = array->get_word(সূচক);
	    unsigned width = array->get_word_আকার();
	    vpip_vec4_get_মান(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_মান(p_vpi_মান vp, পূর্ণসংখ্যা)
{
      unsigned সূচক = get_address();

      প্রতিজ্ঞা(array);
      প্রতিজ্ঞা(সূচক < array->get_আকার());

      যদি (vpi_array_is_real(array)) {
	    দ্বিগুণ val = real_from_vpi_মান(vp);
	    array->set_word(সূচক, val);
      } অন্যথা {
	    unsigned width = array->get_word_আকার();
	    vvp_vector4_t val = vec4_from_vpi_মান(vp, width);
	    array->set_word(সূচক, 0, val);
      }

      ফেরত এই;
}

vpiHandle __vpiArrayVthrA::vpi_handle(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpi_সূচক_2:
	    বিরতি;  // Not implemented!

	  ক্ষেত্র vpi_বাম_Range:
	    ফেরত &array->msb;

	  ক্ষেত্র vpi_ডান_Range:
	    ফেরত &array->lsb;

	  ক্ষেত্র vpiParent:
	  ক্ষেত্র vpiArray:
	    ফেরত array;

	  ক্ষেত্র vpiScope:
	    ফেরত array->get_scope();

	  ক্ষেত্র vpiModule:
	    ফেরত vpip_module(array->get_scope());
      }

      ফেরত 0;
}


পূর্ণসংখ্যা __vpiArrayVthrAPV::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpi_সারি_No:
	    ফেরত 0; // Not implemented for now!

	  ক্ষেত্র vpi_আকার:
	    ফেরত part_wid;

	  ক্ষেত্র vpi_বাম_Range:
	    ফেরত part_bit + part_wid - 1;

	  ক্ষেত্র vpi_ডান_Range:
	    ফেরত part_bit;

	  ক্ষেত্র vpiAutomatic:
	    ফেরত array->get_scope()->is_automatic() ? 1 : 0;

#যদি defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  ক্ষেত্র _vpiFromThr:
	    ফেরত _vpi_at_APV;
#endif

	  ক্ষেত্র vpiConstantSelect:
	    ফেরত 1;

	  পূর্বনির্ধারিত:
	    ফেরত 0;
      }
}

অক্ষর* __vpiArrayVthrAPV::vpi_get_str(পূর্ণসংখ্যা code)
{
      যদি (code == vpiFile) {  // Not implemented for now!
            ফেরত simple_set_rbuf_str(file_names[0]);
      }

      অক্ষর sidx [64];
      snprintf(sidx, 63, "%u", word_sel + array->first_addr.get_মান());
      ফেরত generic_get_str(code, array->get_scope(), array->name, sidx);
}

শূন্য __vpiArrayVthrAPV::vpi_get_মান(p_vpi_মান vp)
{
      প্রতিজ্ঞা(array);

      unsigned সূচক = word_sel;
      যদি (vpi_array_is_real(array)) {
	    দ্বিগুণ tmp = array->get_word_r(সূচক);
	    vpip_real_get_মান(tmp, vp);
      } অন্যথা {
	    vvp_vector4_t tmp = array->get_word(সূচক);
	    tmp = tmp.subvalue(part_bit, part_wid);
	    vpip_vec4_get_মান(tmp, part_wid, মিথ্যা, vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  ক্ষেত্র vpiParent:
	    ফেরত 0;

            // Not part of the Verilog standard. We use this internally.
	  ক্ষেত্র vpiArray:
	    ফেরত array;

	  ক্ষেত্র vpiScope:
	    ফেরত array->get_scope();

	  ক্ষেত্র vpiModule:
	    ফেরত vpip_module(array->get_scope());
      }

      ফেরত 0;
}

শূন্য __vpiArray::set_word(unsigned address, unsigned part_off, ধ্রুবক vvp_vector4_t&val)
{
      যদি (address >= get_আকার())
	    ফেরত;

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
	    ফেরত;
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
	    ফেরত;
      }

      প্রতিজ্ঞা(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle word = nets[address];
      ধ্রুবক কাঠামো __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      প্রতিজ্ঞা(vsig);

      vsig->গ্রন্থি->send_vec4_pv(val, part_off, vpip_আকার(vsig), 0);
      word_change(address);
}

শূন্য __vpiArray::set_word(unsigned address, দ্বিগুণ val)
{
      প্রতিজ্ঞা(vals != 0);
      প্রতিজ্ঞা(nets == 0);

      যদি (address >= vals->get_আকার())
	    ফেরত;

      vals->set_word(address, val);
      word_change(address);
}

শূন্য __vpiArray::set_word(unsigned address, ধ্রুবক string&val)
{
      প্রতিজ্ঞা(vals != 0);
      প্রতিজ্ঞা(nets == 0);

      যদি (address >= vals->get_আকার())
	    ফেরত;

      vals->set_word(address, val);
      word_change(address);
}

শূন্য __vpiArray::set_word(unsigned address, ধ্রুবক vvp_object_t&val)
{
      প্রতিজ্ঞা(vals != 0);
      প্রতিজ্ঞা(nets == 0);

      যদি (address >= vals->get_আকার())
	    ফেরত;

      vals->set_word(address, val);
      word_change(address);
}

vvp_vector4_t __vpiArray::get_word(unsigned address)
{
      যদি (vals4) {
	    প্রতিজ্ঞা(nets == 0);
	    প্রতিজ্ঞা(vals == 0);
	    ফেরত vals4->get_word(address);
      }

      যদি (vals) {
	    প্রতিজ্ঞা(nets == 0);
	    প্রতিজ্ঞা(vals4== 0);
	    যদি (address >= vals->get_আকার())
		  ফেরত vvp_vector4_t(vals_width, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_word(address, val);
	    ফেরত val;
      }

      প্রতিজ্ঞা(vals4 == 0);
      প্রতিজ্ঞা(vals == 0);
      প্রতিজ্ঞা(nets != 0);

      যদি (address >= get_আকার()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    প্রতিজ্ঞা(get_আকার() > 0);
	    vpiHandle word = nets[0];
	    প্রতিজ্ঞা(word);
	    কাঠামো __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
	    প্রতিজ্ঞা(vsig);
	    vvp_signal_মান*sig = dynamic_cast<vvp_signal_মান*> (vsig->গ্রন্থি->fil);
	    প্রতিজ্ঞা(sig);
	    ফেরত vvp_vector4_t(sig->মান_আকার(), BIT4_X);
      }

      vpiHandle word = nets[address];
      কাঠামো __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      প্রতিজ্ঞা(vsig);
      vvp_signal_মান*sig = dynamic_cast<vvp_signal_মান*> (vsig->গ্রন্থি->fil);
      প্রতিজ্ঞা(sig);

      vvp_vector4_t val;
      sig->vec4_মান(val);
      ফেরত val;
}

দ্বিগুণ __vpiArray::get_word_r(unsigned address)
{
      যদি (vals) {
	    প্রতিজ্ঞা(vals4 == 0);
	    প্রতিজ্ঞা(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    যদি (address >= vals->get_আকার())
		  ফেরত 0.0;

	    দ্বিগুণ val;
	    vals->get_word(address, val);
	    ফেরত val;
      }

      যদি (address >= get_আকার())
	    ফেরত 0.0;

      প্রতিজ্ঞা(nets);
      vpiHandle word = nets[address];
      কাঠামো __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(word);
      প্রতিজ্ঞা(vsig);
      vvp_signal_মান*sig = dynamic_cast<vvp_signal_মান*> (vsig->net->fil);
      প্রতিজ্ঞা(sig);

      দ্বিগুণ val = sig->real_মান();
      ফেরত val;
}

শূন্য __vpiArray::get_word_obj(unsigned address, vvp_object_t&val)
{
      যদি (vals) {
	    প্রতিজ্ঞা(vals4 == 0);
	    প্রতিজ্ঞা(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    যদি (address >= vals->get_আকার()) {
		  val = vvp_object_t();
		  ফেরত;
	    }

	    vals->get_word(address, val);
	    ফেরত;
      }

      প্রতিজ্ঞা(nets);
	// Arrays of string nets not implemented!
      প্রতিজ্ঞা(0);
      ফেরত;
}

string __vpiArray::get_word_str(unsigned address)
{
      যদি (vals) {
	    প্রতিজ্ঞা(vals4 == 0);
	    প্রতিজ্ঞা(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    যদি (address >= vals->get_আকার())
		  ফেরত "";

	    string val;
	    vals->get_word(address, val);
	    ফেরত val;
      }

      প্রতিজ্ঞা(nets);
	// Arrays of string nets not implemented!
      প্রতিজ্ঞা(0);
      ফেরত "";
}

vpiHandle vpip_make_array(ধ্রুবক অক্ষর*label, ধ্রুবক অক্ষর*name,
                          পূর্ণসংখ্যা first_addr, পূর্ণসংখ্যা last_addr,
                          যৌক্তিক signed_flag)
{
      কাঠামো __vpiArray*obj = নতুন __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      যদি (last_addr >= first_addr) {
	    obj->swap_addr = মিথ্যা;
      } অন্যথা {
	    obj->swap_addr = সত্য;
	    পূর্ণসংখ্যা tmp = last_addr;
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
	    array_table = নতুন প্রতীক_map_s<কাঠামো __vpiArray>;

      প্রতিজ্ঞা(!array_find(label));
      array_table->sym_set_মান(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_প্রতীক(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_to_current_scope(obj);

      ফেরত obj;
}

শূন্য __vpiArray::alias_word(unsigned long addr, vpiHandle word, পূর্ণসংখ্যা msb_, পূর্ণসংখ্যা lsb_)
{
      প্রতিজ্ঞা(msb.get_মান() == msb_);
      প্রতিজ্ঞা(lsb.get_মান() == lsb_);
      প্রতিজ্ঞা(addr < get_আকার());
      প্রতিজ্ঞা(nets);
      nets[addr] = word;
}

শূন্য __vpiArray::attach_word(unsigned addr, vpiHandle word)
{
      প্রতিজ্ঞা(addr < get_আকার());
      প্রতিজ্ঞা(nets);
      nets[addr] = word;

      যদি (কাঠামো __vpiSignal*sig = dynamic_cast<__vpiSignal*>(word)) {
	    vvp_net_t*net = sig->গ্রন্থি;
	    প্রতিজ্ঞা(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    প্রতিজ্ঞা(fun);
	    fun->attach_as_word(এই, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = এই;
	    sig->id.সূচক = নতুন __vpiDecConst(addr + first_addr.get_মান());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    ফেরত;
      }

      যদি (কাঠামো __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(word)) {
	    vvp_net_t*net = sig->net;
	    প্রতিজ্ঞা(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    প্রতিজ্ঞা(fun);
	    fun->attach_as_word(এই, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = এই;
	    sig->id.সূচক = নতুন __vpiDecConst(addr + first_addr.get_মান());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = সত্য;
	    ফেরত;
      }
}

শূন্য compile_var_array(অক্ষর*label, অক্ষর*name, পূর্ণসংখ্যা last, পূর্ণসংখ্যা first,
		   পূর্ণসংখ্যা msb, পূর্ণসংখ্যা lsb, অক্ষর signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last,
                                      signed_flag != 0);

      কাঠামো __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_width = labs(msb-lsb) + 1;
      যদি (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = নতুন vvp_vector4array_aa(arr->vals_width,
						 arr->get_আকার());
      } অন্যথা {
            arr->vals4 = নতুন vvp_vector4array_sa(arr->vals_width,
						 arr->get_আকার());
      }
      arr->msb.set_মান(msb);
      arr->lsb.set_মান(lsb);

      গুনতি_var_arrays += 1;
      গুনতি_var_array_শব্দগুলি += arr->get_আকার();

      মুক্ত(label);
      delete[] name;
}

শূন্য compile_var2_array(অক্ষর*label, অক্ষর*name, পূর্ণসংখ্যা last, পূর্ণসংখ্যা first,
		   পূর্ণসংখ্যা msb, পূর্ণসংখ্যা lsb, যৌক্তিক signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, signed_flag);

      কাঠামো __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.set_মান(msb);
      arr->lsb.set_মান(lsb);
      arr->vals_width = labs(msb-lsb) + 1;

      প্রতিজ্ঞা(! arr->nets);
      যদি (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = নতুন vvp_darray_atom<int8_t>(arr->get_আকার());
      } অন্যথা যদি (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = নতুন vvp_darray_atom<uint8_t>(arr->get_আকার());
      } অন্যথা যদি (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = নতুন vvp_darray_atom<int16_t>(arr->get_আকার());
      } অন্যথা যদি (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = নতুন vvp_darray_atom<uint16_t>(arr->get_আকার());
      } অন্যথা যদি (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = নতুন vvp_darray_atom<int32_t>(arr->get_আকার());
      } অন্যথা যদি (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = নতুন vvp_darray_atom<uint32_t>(arr->get_আকার());
      } অন্যথা যদি (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = নতুন vvp_darray_atom<int64_t>(arr->get_আকার());
      } অন্যথা যদি (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = নতুন vvp_darray_atom<uint64_t>(arr->get_আকার());
      } অন্যথা {
	    arr->vals = নতুন vvp_darray_vec2(arr->get_আকার(), arr->vals_width);
      }
      গুনতি_var_arrays += 1;
      গুনতি_var_array_শব্দগুলি += arr->get_আকার();

      মুক্ত(label);
      delete[] name;
}

শূন্য compile_real_array(অক্ষর*label, অক্ষর*name, পূর্ণসংখ্যা last, পূর্ণসংখ্যা first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, সত্য);

      কাঠামো __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = নতুন vvp_darray_real(arr->get_আকার());
      arr->vals_width = 1;

      গুনতি_real_arrays += 1;
      গুনতি_real_array_শব্দগুলি += arr->get_আকার();

      মুক্ত(label);
      delete[] name;
}

শূন্য compile_string_array(অক্ষর*label, অক্ষর*name, পূর্ণসংখ্যা last, পূর্ণসংখ্যা first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, সত্য);

      কাঠামো __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = নতুন vvp_darray_string(arr->get_আকার());
      arr->vals_width = 1;

      গুনতি_real_arrays += 1;
      গুনতি_real_array_শব্দগুলি += arr->get_আকার();

      মুক্ত(label);
      delete[] name;
}

শূন্য compile_object_array(অক্ষর*label, অক্ষর*name, পূর্ণসংখ্যা last, পূর্ণসংখ্যা first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, সত্য);

      কাঠামো __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = নতুন vvp_darray_object(arr->get_আকার());
      arr->vals_width = 1;

      গুনতি_real_arrays += 1;
      গুনতি_real_array_শব্দগুলি += arr->get_আকার();

      মুক্ত(label);
      delete[] name;
}

শূন্য compile_net_array(অক্ষর*label, অক্ষর*name, পূর্ণসংখ্যা last, পূর্ণসংখ্যা first)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, first, last, মিথ্যা);

      কাঠামো __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(calloc(arr->get_আকার(), আকার(vpiHandle)));

      গুনতি_net_arrays += 1;
      গুনতি_net_array_শব্দগুলি += arr->get_আকার();

      মুক্ত(label);
      delete[] name;
}

শ্রেণী vvp_fun_arrayport  : সর্বজনীন vvp_net_fun_t {

    সর্বজনীন:
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport() override;

      আভাসী শূন্য check_word_change(unsigned long addr) = 0;

    সুরক্ষিত:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      unsigned long addr_;

      friend শূন্য array_attach_port(vvp_array_t, vvp_fun_arrayport*);
      friend শূন্য __vpiArray::word_change(unsigned long);
      vvp_fun_arrayport*পরবর্তী;
};

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net)
: arr_(mem), net_(net), addr_(0)
{
      পরবর্তী = 0;
}

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, long addr)
: arr_(mem), net_(net), addr_(addr)
{
      পরবর্তী = 0;
}

vvp_fun_arrayport::~vvp_fun_arrayport()
{
}

শ্রেণী vvp_fun_arrayport_sa  : সর্বজনীন vvp_fun_arrayport {

    সর্বজনীন:
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_sa() override;

      শূন্য check_word_change(unsigned long addr) override;

      শূন্য recv_vec4(vvp_net_ptr_t port, ধ্রুবক vvp_vector4_t&bit,
                     vvp_context_t) override;

    ব্যক্তিগত:
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

শূন্য vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t port, ধ্রুবক vvp_vector4_t&bit,
                                     vvp_context_t)
{
      যৌক্তিক addr_valid_flag;

      নির্বাচন (port.port()) {

	  ক্ষেত্র 0: // Address input
	    addr_valid_flag = vector4_to_মান(bit, addr_);
	    যদি (! addr_valid_flag)
		  addr_ = arr_->get_আকার();
	    যদি (vpi_array_is_real(arr_))
		  port.ptr()->send_real(arr_->get_word_r(addr_), 0);
	    অন্যথা
		  port.ptr()->send_vec4(arr_->get_word(addr_), 0);

	    বিরতি;

	  পূর্বনির্ধারিত:
	    fprintf(stdout, "XXXX write ports not implemented.\n");
	    প্রতিজ্ঞা(0);
      }
}

শূন্য vvp_fun_arrayport_sa::check_word_change(unsigned long addr)
{
      যদি (addr != addr_) ফেরত;

      যদি (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr_), 0);
      } অন্যথা {
	    net_->send_vec4(arr_->get_word(addr_), 0);
      }
}

শ্রেণী vvp_fun_arrayport_aa  : সর্বজনীন vvp_fun_arrayport, সর্বজনীন automatic_hooks_s {

    সর্বজনীন:
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_aa() override;

      শূন্য alloc_instance(vvp_context_t context) override;
      শূন্য reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      শূন্য free_instance(vvp_context_t context) override;
#endif

      শূন্য check_word_change(unsigned long addr) override;

      শূন্য recv_vec4(vvp_net_ptr_t port, ধ্রুবক vvp_vector4_t&bit,
                     vvp_context_t context) override;

    ব্যক্তিগত:
      শূন্য check_word_change_(unsigned long addr, vvp_context_t context);

      __vpiScope*context_scope_;
      unsigned context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(এই, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(এই, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

শূন্য vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      unsigned long*addr = নতুন unsigned long;
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

শূন্য vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t port, ধ্রুবক vvp_vector4_t&bit,
                                     vvp_context_t context)
{
      যদি (context) {
            unsigned long*addr = static_cast<unsigned long*>
                  (vvp_get_context_item(context, context_idx_));

            যৌক্তিক addr_valid_flag;

            নির্বাচন (port.port()) {

                ক্ষেত্র 0: // Address input
                  addr_valid_flag = vector4_to_মান(bit, *addr);
                  যদি (! addr_valid_flag) *addr = arr_->get_আকার();
                  যদি (vpi_array_is_real(arr_)) {
			port.ptr()->send_real(arr_->get_word_r(*addr),
					      context);
                  } অন্যথা {
			port.ptr()->send_vec4(arr_->get_word(*addr),
					      context);
                  }
                  বিরতি;

                পূর্বনির্ধারিত:
                  fprintf(stdout, "XXXX write ports not implemented.\n");
                  প্রতিজ্ঞা(0);
            }
      } অন্যথা {
            context = context_scope_->live_contexts;
            যখন (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_পরবর্তী_context(context);
            }
      }
}

শূন্য vvp_fun_arrayport_aa::check_word_change_(unsigned long addr,
                                              vvp_context_t context)
{
      ধ্রুবক unsigned long*port_addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));

      যদি (addr != *port_addr)
	    ফেরত;

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
            যখন (context) {
                  check_word_change_(addr, context);
                  context = vvp_get_পরবর্তী_context(context);
            }
      }
}

স্থির শূন্য array_attach_port(vvp_array_t array, vvp_fun_arrayport*fun)
{
      প্রতিজ্ঞা(fun->পরবর্তী == 0);
      fun->পরবর্তী = array->ports_;
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

শ্রেণী array_word_মান_callback : সর্বজনীন মান_callback {
    সর্বজনীন:
      অন্তঃরেখ explicit array_word_মান_callback(p_cb_তথ্য তথ্য, long addr)
      : মান_callback(তথ্য), word_addr(addr)
      { }

    সর্বজনীন:
      long word_addr;
};

শূন্য __vpiArray::word_change(unsigned long addr)
{
      জন্য (vvp_fun_arrayport*cur = ports_; cur; cur = cur->পরবর্তী)
	    cur->check_word_change(addr);

	// Run callbacks attached to the array itself.
      কাঠামো __vpiCallback *পরবর্তী_2 = vpi_callbacks;
      কাঠামো __vpiCallback *prev = 0;

      যখন (পরবর্তী_2) {
	    array_word_মান_callback*cur = dynamic_cast<array_word_মান_callback*>(পরবর্তী_2);
	    পরবর্তী_2 = cur->পরবর্তী_2;

	      // Skip callbacks that are not for me. -1 is for every element.
	    যদি (cur->word_addr != (long)addr && cur->word_addr != -1) {
		  prev = cur;
		  চালু;
	    }

	      // For whole array callbacks we need to set the index.
	    যদি (cur->word_addr == -1) {
		  cur->cb_তথ্য.সূচক = (PLI_INT32) ((পূর্ণসংখ্যা)addr +
						    first_addr.get_মান());
	    }

	    যদি (cur->cb_তথ্য.cb_rtn != 0) {
		  যদি (cur->test_মান_callback_ready()) {
			যদি (cur->cb_তথ্য.value) {
			      যদি (vpi_array_is_real(এই)) {
				    দ্বিগুণ val = 0.0;
				    যদি (addr < vals->get_আকার())
					  vals->get_word(addr, val);
				    vpip_real_get_মান(val, cur->cb_তথ্য.value);
			      } অন্যথা যদি (vals4) {
				    vpip_vec4_get_মান(vals4->get_word(addr),
							vals_width,
							signed_flag,
							cur->cb_তথ্য.value);
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
				    যদি (addr < vals->get_আকার())
					  vals->get_word(addr, val);
				    vpip_vec4_get_মান(val,
							vals_width,
							signed_flag,
							cur->cb_তথ্য.value);
			      } অন্যথা {
			            প্রতিজ্ঞা(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } অন্যথা যদি (prev == 0) {

		  vpi_callbacks = পরবর্তী_2;
		  cur->পরবর্তী_2 = 0;
		  delete cur;

	    } অন্যথা {
		  প্রতিজ্ঞা(prev->পরবর্তী_2 == cur);
		  prev->পরবর্তী_2 = পরবর্তী_2;
		  cur->পরবর্তী_2 = 0;
		  delete cur;
	    }
      }
}

শ্রেণী array_resolv_list_t : সর্বজনীন resolv_list_s {

    সর্বজনীন:
      explicit array_resolv_list_t(অক্ষর*lab) : resolv_list_s(lab) {
	    array = 0;
      }

      vvp_array_t*array;
      যৌক্তিক resolve(যৌক্তিক mes) override;

    ব্যক্তিগত:
};

যৌক্তিক array_resolv_list_t::resolve(যৌক্তিক mes)
{
      *array = array_find(label());
      যদি (*array == 0) {
	    প্রতিজ্ঞা(!mes);
	    ফেরত মিথ্যা;
      }
      ফেরত সত্য;
}

শ্রেণী array_port_resolv_list_t : সর্বজনীন resolv_list_s {

    সর্বজনীন:
      explicit array_port_resolv_list_t(অক্ষর* lab, যৌক্তিক use_addr__,
                                        long addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      যৌক্তিক use_addr;
      long addr;
      যৌক্তিক resolve(যৌক্তিক mes) override;

    ব্যক্তিগত:
};

array_port_resolv_list_t::array_port_resolv_list_t(অক্ষর *lab, যৌক্তিক use_addr__,
                                                   long addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      যদি (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      অন্যথা
            context_scope = 0;
      ptr = নতুন vvp_net_t;
}

যৌক্তিক array_port_resolv_list_t::resolve(যৌক্তিক mes)
{
      vvp_array_t mem = array_find(label());
      যদি (mem == 0) {
	    প্রতিজ্ঞা(mem || !mes);
	    ফেরত মিথ্যা;
      }

      vvp_fun_arrayport*fun;
      যদি (use_addr)
            যদি (context_scope)
                  fun = নতুন vvp_fun_arrayport_aa(context_scope, mem, ptr, addr);
            অন্যথা
                  fun = নতুন vvp_fun_arrayport_sa(mem, ptr, addr);
      অন্যথা
            যদি (context_scope)
                  fun = নতুন vvp_fun_arrayport_aa(context_scope, mem, ptr);
            অন্যথা
                  fun = নতুন vvp_fun_arrayport_sa(mem, ptr);
      ptr->fun = fun;

      array_attach_port(mem, fun);

      ফেরত সত্য;
}

শ্রেণী array_word_part_callback : সর্বজনীন array_word_মান_callback {
    সর্বজনীন:
      explicit array_word_part_callback(p_cb_তথ্য তথ্য, long addr);
      ~array_word_part_callback() override;

      array_word_part_callback(ধ্রুবক array_word_part_callback&) = delete;
      array_word_part_callback& operator=(ধ্রুবক array_word_part_callback&) = delete;

      যৌক্তিক test_মান_callback_ready(শূন্য) override;

    ব্যক্তিগত:
      অক্ষর*মান_bits;
};

array_word_part_callback::array_word_part_callback(p_cb_তথ্য তথ্য, long addr)
: array_word_মান_callback(তথ্য, addr)
{
	// Get the initial value of the part, to use as a reference.
      কাঠামো __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(তথ্য->obj);
      s_vpi_মান tmp_মান;
      tmp_মান.format = vpiBinStrVal;
      apvword->vpi_get_মান(&tmp_মান);

      মান_bits = নতুন অক্ষর[apvword->part_wid+1];

      memcpy(মান_bits, tmp_মান.value.str, apvword->part_wid);
      মান_bits[apvword->part_wid] = 0;
}

array_word_part_callback::~array_word_part_callback()
{
      delete[]মান_bits;
}

যৌক্তিক array_word_part_callback::test_মান_callback_ready(শূন্য)
{
      কাঠামো __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_তথ্য.obj);
      প্রতিজ্ঞা(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_মান tmp_মান;
      tmp_মান.format = vpiBinStrVal;
      apvword->vpi_get_মান(&tmp_মান);

      যদি (memcmp(মান_bits, tmp_মান.value.str, apvword->part_wid) == 0)
	    ফেরত মিথ্যা;

      memcpy(মান_bits, tmp_মান.value.str, apvword->part_wid);
      ফেরত সত্য;

}

মান_callback*vpip_array_word_change(p_cb_তথ্য তথ্য)
{
      কাঠামো __vpiArray*parent = 0;
      array_word_মান_callback*cbh = 0;
      যদি (ধ্রুবক কাঠামো __vpiArrayWord*word = array_var_word_from_handle(তথ্য->obj)) {
	    parent = static_cast<__vpiArray*>(word->get_parent());
	    unsigned addr = word->get_সূচক();
	    cbh = নতুন array_word_মান_callback(তথ্য, addr);

      } অন্যথা যদি (কাঠামো __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(তথ্য->obj)) {
	    parent = tword->array;
	    cbh = নতুন array_word_মান_callback(তথ্য, tword->address);

      } অন্যথা যদি (কাঠামো __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(তথ্য->obj)) {
	    parent = apvword->array;
	    cbh = নতুন array_word_part_callback(তথ্য, apvword->word_sel);
      }

      প্রতিজ্ঞা(cbh);
      প্রতিজ্ঞা(parent);
      cbh->পরবর্তী_2 = parent->vpi_callbacks;
      parent->vpi_callbacks = cbh;

      ফেরত cbh;
}

মান_callback* vpip_array_change(p_cb_তথ্য তথ্য)
{
      array_word_মান_callback*cbh = নতুন array_word_মান_callback(তথ্য, -1);
      প্রতিজ্ঞা(তথ্য->obj);

      কাঠামো __vpiArray*arr = dynamic_cast<__vpiArray*>(তথ্য->obj);
      cbh->পরবর্তী_2 = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      ফেরত cbh;
}

শূন্য compile_array_port(অক্ষর*label, অক্ষর*array, অক্ষর*addr)
{
      array_port_resolv_list_t*resolv_mem
	    = নতুন array_port_resolv_list_t(array, মিথ্যা, 0);

      define_functor_প্রতীক(label, resolv_mem->ptr);
      মুক্ত(label);
	// Connect the port-0 input as the address.
      প্রবেশ_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

শূন্য compile_array_port(অক্ষর*label, অক্ষর*array, long addr)
{
      array_port_resolv_list_t*resolv_mem
	    = নতুন array_port_resolv_list_t(array, সত্য, addr);

      define_functor_প্রতীক(label, resolv_mem->ptr);
      মুক্ত(label);

      resolv_submit(resolv_mem);
}

শূন্য compile_array_alias(অক্ষর*label, অক্ষর*name, অক্ষর*src)
{
      vvp_array_t mem = array_find(src);
      প্রতিজ্ঞা(mem);

      কাঠামো __vpiArray*obj = নতুন __vpiArray;

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
vpiHandle vpip_make_vthr_A(অক্ষর*label, unsigned addr)
{
      কাঠামো __vpiArrayVthrA*obj = নতুন __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = নতুন array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      obj->address = addr;

      ফেরত obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(অক্ষর*label, অক্ষর*প্রতীক)
{
      কাঠামো __vpiArrayVthrA*obj = নতুন __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = নতুন array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      compile_vpi_lookup(&obj->address_handle, প্রতীক);
      obj->address = 0;

      ফেরত obj;
}

vpiHandle vpip_make_vthr_A(অক্ষর*label, vpiHandle handle)
{
      কাঠামো __vpiArrayVthrA*obj = নতুন __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = নতুন array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = handle;
      obj->address = 0;

      ফেরত obj;
}

vpiHandle vpip_make_vthr_APV(অক্ষর*label, unsigned সূচক, unsigned bit, unsigned wid)
{
      কাঠামো __vpiArrayVthrAPV*obj = নতুন __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = নতুন array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->word_sel = সূচক;
      obj->part_bit = bit;
      obj->part_wid = wid;

      ফেরত obj;
}

শূন্য compile_array_cleanup(শূন্য)
{
      delete array_table;
      array_table = 0;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য memory_delete(vpiHandle item)
{
      কাঠামো __vpiArray*arr = static_cast<কাঠামো __vpiArray*>(item);
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
	    জন্য (unsigned idx = 0; idx < arr->get_আকার(); idx += 1) {
		  যদি (কাঠামো __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.সূচক);
		    /* These should only be the real words. */
		  } অন্যথা {
			প্রতিজ্ঞা(arr->nets[idx]->get_type_code() ==
			       vpiRealVar);
			কাঠামো __vpiRealVar *sigr = static_cast<কাঠামো __vpiRealVar *>
			                            (arr->nets[idx]);
			constant_delete(sigr->id.সূচক);
// Why are only the real words still here?
			delete arr->nets[idx];
		  }
	    }
	    মুক্ত(arr->nets);
      }

      যখন (arr->vpi_callbacks) {
	    কাঠামো __vpiCallback*tmp = arr->vpi_callbacks->পরবর্তী_2;
	    delete arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      delete arr;
}

শূন্য A_delete(vpiHandle item)
{
      কাঠামো __vpiArrayVthrA*obj = static_cast<কাঠামো __vpiArrayVthrA*>(item);
      যদি (obj->address_handle) {
	    নির্বাচন (obj->address_handle->get_type_code()) {
		ক্ষেত্র vpiMemoryWord:
		  যদি (vpi_get(_vpiFromThr, obj->address_handle) == _vpi_at_A) {
			A_delete(obj->address_handle);
		  }
		  বিরতি;
		ক্ষেত্র vpiPartSelect:
		  প্রতিজ্ঞা(vpi_get(_vpiFromThr, obj->address_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->address_handle);
		  বিরতি;
	    }
      }

      delete obj;
}

শূন্য APV_delete(vpiHandle item)
{
      কাঠামো __vpiArrayVthrAPV*obj = static_cast<কাঠামো __vpiArrayVthrAPV*>(item);
      delete obj;
}
#endif
