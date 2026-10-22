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
#شامل  "vvp_cleanup.h"
#endif
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <iostream>
# include  "compile.h"
# include  <cassert>
# include  "ivl_alloc.h"

using فضاینام std;

unsigned long شمار_net_arrays = 0;
unsigned long شمار_net_array_واژگان = 0;
unsigned long شمار_var_arrays = 0;
unsigned long شمار_var_array_واژگان = 0;
unsigned long شمار_real_arrays = 0;
unsigned long شمار_real_array_واژگان = 0;

ایستا نماد_map_s<ساختار __vpiArray>* array_table =0;

رده vvp_fun_arrayport;
ایستا تهی array_attach_port(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_find(ثابت نویسه*label)
{
      اگر (array_table == 0)
	    بازگشت 0;

      vvp_array_t v = array_table->sym_get_مقدار(label);
      بازگشت v;
}

ساختار __vpiArrayVthrA : عمومی __vpiHandle {
      صحیح get_type_code(تهی) ثابت override { بازگشت vpiMemoryWord; }
      صحیح vpi_get(صحیح code) override;
      نویسه* vpi_get_str(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحیح flags) override;
      vpiHandle vpi_handle(صحیح code) override;

      ساختار __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle address_handle;
	// If wid==0, then address is the address into the array.
      unsigned address;

      unsigned get_address() ثابت
      {
	    اگر (address_handle) {
		  s_vpi_مقدار vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpiVectorVal;
		  address_handle->vpi_get_مقدار(&vp);
		  صحیح واژگان = (address_handle->vpi_get(vpiSize)-1)/32 + 1;
		  برای(صحیح idx = 0; idx < واژگان; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			اگر (vp.value.vector[idx].bval != 0) بازگشت UINT_MAX;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  address_handle->vpi_get_مقدار(&vp);
		  بازگشت vp.value.integer;
	    }

	    بازگشت address;
      }
};


ساختار __vpiArrayVthrAPV : عمومی __vpiHandle {
      صحیح get_type_code(تهی) ثابت override { بازگشت vpiPartSelect; }
      صحیح vpi_get(صحیح code) override;
      نویسه* vpi_get_str(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;
      vpiHandle vpi_handle(صحیح code) override;

      ساختار __vpiArray*array;
      unsigned word_sel;
      unsigned part_bit;
      unsigned part_wid;
};

منطقی is_net_array(vpiHandle obj)
{
      ساختار __vpiArray*rfp = dynamic_cast<__vpiArray*> (obj);
      أكد(rfp);

      اگر (rfp->nets != 0) بازگشت درست;
      بازگشت نادرست;
}

// This function return true if the underlying array words are real.
ایستا منطقی vpi_array_is_real(ثابت vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      اگر (arr->vals4 != 0)  // A bit based variable/register array.
	    بازگشت نادرست;

      اگر (dynamic_cast<vvp_darray_real*> (arr->vals))
	    بازگشت درست;

      اگر (arr->vals != 0)
	    بازگشت نادرست;

	// This must be a net array so look at element 0 to find the type.
      أكد(arr->nets != 0);
      أكد(arr->get_size() > 0);
      ثابت ساختار __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      اگر (rsig) {
	    بازگشت درست;
      }

      بازگشت نادرست;
}

ایستا منطقی vpi_array_is_string(ثابت vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      اگر (arr->vals4 != 0)  // A bit based variable/register array.
	    بازگشت نادرست;

      اگر (dynamic_cast<vvp_darray_string*> (arr->vals))
	    بازگشت درست;

      بازگشت نادرست;
}

صحیح __vpiArray::get_word_size() ثابت
{
      unsigned width;

      أكد(get_size() > 0);
	/* For a net array we need to get the width from the first element. */
      اگر (nets) {
	    أكد(vals4 == 0 && vals == 0);
	    ثابت ساختار __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    أكد(vsig);
	    width = vpip_size(vsig);
	/* For a variable array we can get the width from vals_width. */
      } وگرنه {
	    أكد(vals4 || vals);
	    width = vals_width;
      }

      بازگشت width;
}

نویسه*__vpiArray::get_word_str(ساختار __vpiArrayWord*word, صحیح code)
{
      unsigned index = word->get_index();

      اگر (code == vpiFile) {  // Not implemented for now!
	    بازگشت simple_set_rbuf_str(file_names[0]);
      }

      نویسه sidx [64];
      snprintf(sidx, 63, "%d", (صحیح)index + first_addr.get_مقدار());
      بازگشت generic_get_str(code, get_scope(), name, sidx);
}

تهی __vpiArray::get_word_مقدار(ساختار __vpiArrayWord*word, p_vpi_مقدار vp)
{
      unsigned index = word->get_index();

    // Determine the appropriate format (The Verilog PLI Handbook 5.2.10)
      اگر(vp->format == vpiObjTypeVal) {
          اگر(vpi_array_is_real(این))
              vp->format = vpiRealVal;
          وگرنه اگر(vpi_array_is_string(این))
              vp->format = vpiStringVal;
          وگرنه
              vp->format = vpiIntVal;
      }

      اگر(vals4) {
          vpip_vec4_get_مقدار(vals4->get_word(index),
                              vals4->width(), signed_flag, vp);
      } وگرنه اگر(vals) {
          انتخاب(vp->format) {
            حالت vpiBinStrVal:
            حالت vpiOctStrVal:
            حالت vpiDecStrVal:
            حالت vpiHexStrVal:
            حالت vpiScalarVal:
            حالت vpiIntVal:
            حالت vpiVectorVal:
            {
                vvp_vector4_t v;
                vals->get_word(index, v);
                vpip_vec4_get_مقدار(v, vals_width, signed_flag, vp);
            }
            بشکن;

            حالت vpiRealVal:
            {
                دوگانه d;
                vals->get_word(index, d);
                vpip_real_get_مقدار(d, vp);
            }
            بشکن;

            حالت vpiStringVal:
            {
                string s;
                vals->get_word(index, s);
                vpip_string_get_مقدار(s, vp);
            }
            بشکن;

            پیشفرض:
                fprintf(stderr, "vpi sorry: format is not implemented\n");
                أكد(نادرست);
          }
      }
}

تهی __vpiArray::put_word_مقدار(ساختار __vpiArrayWord*word, p_vpi_مقدار vp, صحیح)
{
      unsigned index = word->get_index();
      vvp_vector4_t val = vec4_from_vpi_مقدار(vp, vals_width);
      set_word(index, 0, val);
}

vpiHandle __vpiArray::get_iter_index(ساختار __vpiArrayIterator*, صحیح idx)
{
      اگر (nets) بازگشت nets[idx];

      أكد(vals4 || vals);

      اگر (vals_واژگان == 0) make_vals_واژگان();

      بازگشت &(vals_واژگان[idx].as_word);
}

صحیح __vpiArray::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpi_سطر_No:
	    بازگشت 0; // Not implemented for now!

	  حالت vpiSize:
	    بازگشت get_size();

	  حالت vpiAutomatic:
	    بازگشت scope->is_automatic()? 1 : 0;

	  پیشفرض:
	    بازگشت 0;
      }
}

نویسه* __vpiArray::vpi_get_str(صحیح code)
{
      اگر (code == vpiFile) {  // Not implemented for now!
            بازگشت simple_set_rbuf_str(file_names[0]);
      }

      بازگشت generic_get_str(code, scope, name, پوچ);
}

vpiHandle __vpiArray::vpi_handle(صحیح code)
{
      انتخاب (code) {
          حالت vpi_چپ_Range:
            اگر (swap_addr) بازگشت &last_addr;
            وگرنه بازگشت &first_addr;

          حالت vpi_راست_Range:
            اگر (swap_addr) بازگشت &first_addr;
            وگرنه بازگشت &last_addr;

	  حالت vpiScope:
	    بازگشت scope;

	  حالت vpiModule:
	    بازگشت vpip_module(scope);
      }

      بازگشت 0;
}

/*
* VPI code passes indices that are not yet converted to canonical
* form, so this index function does it here.
*/
vpiHandle __vpiArray::vpi_index(صحیح index)
{
      index -= first_addr.get_مقدار();
      اگر (index >= (long) get_size())
	    بازگشت 0;
      اگر (index < 0)
	    بازگشت 0;

      اگر (nets != 0) {
	    بازگشت nets[index];
      }

      اگر (vals_واژگان == 0)
	    make_vals_واژگان();

      بازگشت &(vals_واژگان[index].as_word);
}

صحیح __vpiArrayWord::as_word_t::vpi_get(صحیح code)
{
      ساختار __vpiArrayWord*obj = array_var_word_from_handle(این);
      أكد(obj);
      ساختار __vpiArrayBase*my_parent = obj->get_parent();
      t_vpi_مقدار val;

      انتخاب (code) {
	  حالت vpi_سطر_No:
	    بازگشت 0; // Not implemented for now!

	  حالت vpiSize:
	    بازگشت my_parent->get_word_size();

	  حالت vpi_چپ_Range:
            val.format = vpiIntVal;
	    my_parent->get_چپ_range()->vpi_get_مقدار(&val);
            أكد(val.format == vpiIntVal);
            بازگشت val.value.integer;

	  حالت vpi_راست_Range:
            val.format = vpiIntVal;
	    my_parent->get_راست_range()->vpi_get_مقدار(&val);
            أكد(val.format == vpiIntVal);
            بازگشت val.value.integer;

	  حالت vpiIndex:
	    {
		  صحیح base_offset = 0;
		  ساختار __vpiArray*base = dynamic_cast<__vpiArray*> (my_parent);
		  اگر (base) {
			val.format = vpiIntVal;
			base->first_addr.vpi_get_مقدار(&val);
			base_offset += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_index.vpi_get_مقدار(&val);
		  أكد(val.format == vpiIntVal);
		  بازگشت val.value.integer + base_offset;
	    }

	  حالت vpiAutomatic:
	    بازگشت my_parent->get_scope()->is_automatic()? 1 : 0;

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالت _vpiFromThr:
	    بازگشت _vpiNoThr;
#endif

	  پیشفرض:
	    بازگشت 0;
      }
}


صحیح __vpiArrayVthrA::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpi_سطر_No:
	    بازگشت 0; // Not implemented for now!

	  حالت vpiSize:
	    بازگشت array->get_word_size();

	  حالت vpi_چپ_Range:
	    بازگشت array->msb.get_مقدار();

	  حالت vpi_راست_Range:
	    بازگشت array->lsb.get_مقدار();

	  حالت vpiIndex:
	    بازگشت (صحیح)get_address() + array->first_addr.get_مقدار();

	  حالت vpiAutomatic:
	    بازگشت array->get_scope()->is_automatic() ? 1 : 0;

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالت _vpiFromThr:
	    بازگشت _vpi_at_A;
#endif

	  // If address_handle is not zero we definitely have a
	  // variable.
	  حالت vpiConstantSelect:
	    بازگشت address_handle == 0;

	  پیشفرض:
	    بازگشت 0;
      }
}

نویسه* __vpiArrayVthrA::vpi_get_str(صحیح code)
{
      اگر (code == vpiFile) {  // Not implemented for now!
            بازگشت simple_set_rbuf_str(file_names[0]);
      }

      نویسه sidx [64];
      snprintf(sidx, 63, "%d", (صحیح)get_address() + array->first_addr.get_مقدار());
      بازگشت generic_get_str(code, array->get_scope(), array->name, sidx);
}

تهی __vpiArrayVthrA::vpi_get_مقدار(p_vpi_مقدار vp)
{
      أكد(array);

      unsigned index = get_address();
      اگر (vpi_array_is_real(array)) {
	    دوگانه tmp = array->get_word_r(index);
	    vpip_real_get_مقدار(tmp, vp);
      } وگرنه اگر (vpi_array_is_string(array)) {
	    string tmp = array->get_word_str(index);
	    vpip_string_get_مقدار(tmp, vp);
      } وگرنه {
	    vvp_vector4_t tmp = array->get_word(index);
	    unsigned width = array->get_word_size();
	    vpip_vec4_get_مقدار(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_مقدار(p_vpi_مقدار vp, صحیح)
{
      unsigned index = get_address();

      أكد(array);
      أكد(index < array->get_size());

      اگر (vpi_array_is_real(array)) {
	    دوگانه val = real_from_vpi_مقدار(vp);
	    array->set_word(index, val);
      } وگرنه {
	    unsigned width = array->get_word_size();
	    vvp_vector4_t val = vec4_from_vpi_مقدار(vp, width);
	    array->set_word(index, 0, val);
      }

      بازگشت این;
}

vpiHandle __vpiArrayVthrA::vpi_handle(صحیح code)
{
      انتخاب (code) {
	  حالت vpiIndex:
	    بشکن;  // Not implemented!

	  حالت vpi_چپ_Range:
	    بازگشت &array->msb;

	  حالت vpi_راست_Range:
	    بازگشت &array->lsb;

	  حالت vpiParent:
	  حالت vpiArray:
	    بازگشت array;

	  حالت vpiScope:
	    بازگشت array->get_scope();

	  حالت vpiModule:
	    بازگشت vpip_module(array->get_scope());
      }

      بازگشت 0;
}


صحیح __vpiArrayVthrAPV::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpi_سطر_No:
	    بازگشت 0; // Not implemented for now!

	  حالت vpiSize:
	    بازگشت part_wid;

	  حالت vpi_چپ_Range:
	    بازگشت part_bit + part_wid - 1;

	  حالت vpi_راست_Range:
	    بازگشت part_bit;

	  حالت vpiAutomatic:
	    بازگشت array->get_scope()->is_automatic() ? 1 : 0;

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالت _vpiFromThr:
	    بازگشت _vpi_at_APV;
#endif

	  حالت vpiConstantSelect:
	    بازگشت 1;

	  پیشفرض:
	    بازگشت 0;
      }
}

نویسه* __vpiArrayVthrAPV::vpi_get_str(صحیح code)
{
      اگر (code == vpiFile) {  // Not implemented for now!
            بازگشت simple_set_rbuf_str(file_names[0]);
      }

      نویسه sidx [64];
      snprintf(sidx, 63, "%u", word_sel + array->first_addr.get_مقدار());
      بازگشت generic_get_str(code, array->get_scope(), array->name, sidx);
}

تهی __vpiArrayVthrAPV::vpi_get_مقدار(p_vpi_مقدار vp)
{
      أكد(array);

      unsigned index = word_sel;
      اگر (vpi_array_is_real(array)) {
	    دوگانه tmp = array->get_word_r(index);
	    vpip_real_get_مقدار(tmp, vp);
      } وگرنه {
	    vvp_vector4_t tmp = array->get_word(index);
	    tmp = tmp.subvalue(part_bit, part_wid);
	    vpip_vec4_get_مقدار(tmp, part_wid, نادرست, vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(صحیح code)
{
      انتخاب (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  حالت vpiParent:
	    بازگشت 0;

            // Not part of the Verilog standard. We use this internally.
	  حالت vpiArray:
	    بازگشت array;

	  حالت vpiScope:
	    بازگشت array->get_scope();

	  حالت vpiModule:
	    بازگشت vpip_module(array->get_scope());
      }

      بازگشت 0;
}

تهی __vpiArray::set_word(unsigned address, unsigned part_off, ثابت vvp_vector4_t&val)
{
      اگر (address >= get_size())
	    بازگشت;

      اگر (vals4) {
	    أكد(nets == 0);
	    اگر (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp = vals4->get_word(address);
		  اگر ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			أكد(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals4->set_word(address, tmp);
	    } وگرنه {
		  vals4->set_word(address, val);
	    }
	    word_change(address);
	    بازگشت;
      }

      اگر (vals) {
	    أكد(nets == 0);
	    اگر (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp;
		  vals->get_word(address, tmp);
		  اگر ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			أكد(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals->set_word(address, tmp);
	    } وگرنه {
		  vals->set_word(address, val);
	    }
	    word_change(address);
	    بازگشت;
      }

      أكد(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle word = nets[address];
      ثابت ساختار __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      أكد(vsig);

      vsig->گره->send_vec4_pv(val, part_off, vpip_size(vsig), 0);
      word_change(address);
}

تهی __vpiArray::set_word(unsigned address, دوگانه val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      اگر (address >= vals->get_size())
	    بازگشت;

      vals->set_word(address, val);
      word_change(address);
}

تهی __vpiArray::set_word(unsigned address, ثابت string&val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      اگر (address >= vals->get_size())
	    بازگشت;

      vals->set_word(address, val);
      word_change(address);
}

تهی __vpiArray::set_word(unsigned address, ثابت vvp_object_t&val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      اگر (address >= vals->get_size())
	    بازگشت;

      vals->set_word(address, val);
      word_change(address);
}

vvp_vector4_t __vpiArray::get_word(unsigned address)
{
      اگر (vals4) {
	    أكد(nets == 0);
	    أكد(vals == 0);
	    بازگشت vals4->get_word(address);
      }

      اگر (vals) {
	    أكد(nets == 0);
	    أكد(vals4== 0);
	    اگر (address >= vals->get_size())
		  بازگشت vvp_vector4_t(vals_width, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_word(address, val);
	    بازگشت val;
      }

      أكد(vals4 == 0);
      أكد(vals == 0);
      أكد(nets != 0);

      اگر (address >= get_size()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    أكد(get_size() > 0);
	    vpiHandle word = nets[0];
	    أكد(word);
	    ساختار __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
	    أكد(vsig);
	    vvp_signal_مقدار*sig = dynamic_cast<vvp_signal_مقدار*> (vsig->گره->fil);
	    أكد(sig);
	    بازگشت vvp_vector4_t(sig->مقدار_size(), BIT4_X);
      }

      vpiHandle word = nets[address];
      ساختار __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      أكد(vsig);
      vvp_signal_مقدار*sig = dynamic_cast<vvp_signal_مقدار*> (vsig->گره->fil);
      أكد(sig);

      vvp_vector4_t val;
      sig->vec4_مقدار(val);
      بازگشت val;
}

دوگانه __vpiArray::get_word_r(unsigned address)
{
      اگر (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    اگر (address >= vals->get_size())
		  بازگشت 0.0;

	    دوگانه val;
	    vals->get_word(address, val);
	    بازگشت val;
      }

      اگر (address >= get_size())
	    بازگشت 0.0;

      أكد(nets);
      vpiHandle word = nets[address];
      ساختار __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(word);
      أكد(vsig);
      vvp_signal_مقدار*sig = dynamic_cast<vvp_signal_مقدار*> (vsig->net->fil);
      أكد(sig);

      دوگانه val = sig->real_مقدار();
      بازگشت val;
}

تهی __vpiArray::get_word_obj(unsigned address, vvp_object_t&val)
{
      اگر (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    اگر (address >= vals->get_size()) {
		  val = vvp_object_t();
		  بازگشت;
	    }

	    vals->get_word(address, val);
	    بازگشت;
      }

      أكد(nets);
	// Arrays of string nets not implemented!
      أكد(0);
      بازگشت;
}

string __vpiArray::get_word_str(unsigned address)
{
      اگر (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    اگر (address >= vals->get_size())
		  بازگشت "";

	    string val;
	    vals->get_word(address, val);
	    بازگشت val;
      }

      أكد(nets);
	// Arrays of string nets not implemented!
      أكد(0);
      بازگشت "";
}

vpiHandle vpip_make_array(ثابت نویسه*label, ثابت نویسه*name,
                          صحیح first_addr, صحیح last_addr,
                          منطقی signed_flag)
{
      ساختار __vpiArray*obj = جدید __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      اگر (last_addr >= first_addr) {
	    obj->swap_addr = نادرست;
      } وگرنه {
	    obj->swap_addr = درست;
	    صحیح tmp = last_addr;
	    last_addr = first_addr;
	    first_addr = tmp;
      }
      أكد(last_addr >= first_addr);
      unsigned array_شمار = last_addr+1-first_addr;

	// For now, treat all arrays as memories. This is not quite
	// correct, as arrays are arrays with memories a special case.
      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_شمار = array_شمار;

      obj->first_addr.set_مقدار(first_addr);
      obj->last_addr .set_مقدار(last_addr);

	// Start off now knowing if we are nets or variables.
      obj->nets = 0;
      obj->vals4 = 0;
      obj->vals  = 0;
      obj->vals_width = 0;
      obj->vals_واژگان = 0;

	// Initialize (clear) the read-ports list.
      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

	/* Add this symbol to the array_symbols table for later lookup. */
      اگر (!array_table)
	    array_table = جدید نماد_map_s<ساختار __vpiArray>;

      أكد(!array_find(label));
      array_table->sym_set_مقدار(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_نماد(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_to_current_scope(obj);

      بازگشت obj;
}

تهی __vpiArray::alias_word(unsigned long addr, vpiHandle word, صحیح msb_, صحیح lsb_)
{
      أكد(msb.get_مقدار() == msb_);
      أكد(lsb.get_مقدار() == lsb_);
      أكد(addr < get_size());
      أكد(nets);
      nets[addr] = word;
}

تهی __vpiArray::attach_word(unsigned addr, vpiHandle word)
{
      أكد(addr < get_size());
      أكد(nets);
      nets[addr] = word;

      اگر (ساختار __vpiSignal*sig = dynamic_cast<__vpiSignal*>(word)) {
	    vvp_net_t*net = sig->گره;
	    أكد(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    أكد(fun);
	    fun->attach_as_word(این, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = این;
	    sig->id.index = جدید __vpiDecConst(addr + first_addr.get_مقدار());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    بازگشت;
      }

      اگر (ساختار __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(word)) {
	    vvp_net_t*net = sig->net;
	    أكد(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    أكد(fun);
	    fun->attach_as_word(این, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = این;
	    sig->id.index = جدید __vpiDecConst(addr + first_addr.get_مقدار());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = درست;
	    بازگشت;
      }
}

تهی compile_var_array(نویسه*label, نویسه*name, صحیح last, صحیح first,
		   صحیح msb, صحیح lsb, نویسه signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last,
                                      signed_flag != 0);

      ساختار __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_width = labs(msb-lsb) + 1;
      اگر (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = جدید vvp_vector4array_aa(arr->vals_width,
						 arr->get_size());
      } وگرنه {
            arr->vals4 = جدید vvp_vector4array_sa(arr->vals_width,
						 arr->get_size());
      }
      arr->msb.set_مقدار(msb);
      arr->lsb.set_مقدار(lsb);

      شمار_var_arrays += 1;
      شمار_var_array_واژگان += arr->get_size();

      آزاد(label);
      delete[] name;
}

تهی compile_var2_array(نویسه*label, نویسه*name, صحیح last, صحیح first,
		   صحیح msb, صحیح lsb, منطقی signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, signed_flag);

      ساختار __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.set_مقدار(msb);
      arr->lsb.set_مقدار(lsb);
      arr->vals_width = labs(msb-lsb) + 1;

      أكد(! arr->nets);
      اگر (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = جدید vvp_darray_atom<int8_t>(arr->get_size());
      } وگرنه اگر (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = جدید vvp_darray_atom<uint8_t>(arr->get_size());
      } وگرنه اگر (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = جدید vvp_darray_atom<int16_t>(arr->get_size());
      } وگرنه اگر (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = جدید vvp_darray_atom<uint16_t>(arr->get_size());
      } وگرنه اگر (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = جدید vvp_darray_atom<int32_t>(arr->get_size());
      } وگرنه اگر (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = جدید vvp_darray_atom<uint32_t>(arr->get_size());
      } وگرنه اگر (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = جدید vvp_darray_atom<int64_t>(arr->get_size());
      } وگرنه اگر (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = جدید vvp_darray_atom<uint64_t>(arr->get_size());
      } وگرنه {
	    arr->vals = جدید vvp_darray_vec2(arr->get_size(), arr->vals_width);
      }
      شمار_var_arrays += 1;
      شمار_var_array_واژگان += arr->get_size();

      آزاد(label);
      delete[] name;
}

تهی compile_real_array(نویسه*label, نویسه*name, صحیح last, صحیح first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, درست);

      ساختار __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = جدید vvp_darray_real(arr->get_size());
      arr->vals_width = 1;

      شمار_real_arrays += 1;
      شمار_real_array_واژگان += arr->get_size();

      آزاد(label);
      delete[] name;
}

تهی compile_string_array(نویسه*label, نویسه*name, صحیح last, صحیح first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, درست);

      ساختار __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = جدید vvp_darray_string(arr->get_size());
      arr->vals_width = 1;

      شمار_real_arrays += 1;
      شمار_real_array_واژگان += arr->get_size();

      آزاد(label);
      delete[] name;
}

تهی compile_object_array(نویسه*label, نویسه*name, صحیح last, صحیح first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, درست);

      ساختار __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = جدید vvp_darray_object(arr->get_size());
      arr->vals_width = 1;

      شمار_real_arrays += 1;
      شمار_real_array_واژگان += arr->get_size();

      آزاد(label);
      delete[] name;
}

تهی compile_net_array(نویسه*label, نویسه*name, صحیح last, صحیح first)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, first, last, نادرست);

      ساختار __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(calloc(arr->get_size(), اندازه(vpiHandle)));

      شمار_net_arrays += 1;
      شمار_net_array_واژگان += arr->get_size();

      آزاد(label);
      delete[] name;
}

رده vvp_fun_arrayport  : عمومی vvp_net_fun_t {

    عمومی:
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport() override;

      مجازی تهی check_word_change(unsigned long addr) = 0;

    محافظتشده:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      unsigned long addr_;

      friend تهی array_attach_port(vvp_array_t, vvp_fun_arrayport*);
      friend تهی __vpiArray::word_change(unsigned long);
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

رده vvp_fun_arrayport_sa  : عمومی vvp_fun_arrayport {

    عمومی:
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_sa() override;

      تهی check_word_change(unsigned long addr) override;

      تهی recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;

    خصوصی:
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

تهی vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                     vvp_context_t)
{
      منطقی addr_valid_flag;

      انتخاب (port.port()) {

	  حالت 0: // Address input
	    addr_valid_flag = vector4_to_مقدار(bit, addr_);
	    اگر (! addr_valid_flag)
		  addr_ = arr_->get_size();
	    اگر (vpi_array_is_real(arr_))
		  port.ptr()->send_real(arr_->get_word_r(addr_), 0);
	    وگرنه
		  port.ptr()->send_vec4(arr_->get_word(addr_), 0);

	    بشکن;

	  پیشفرض:
	    fprintf(stdout, "XXXX write ports not implemented.\n");
	    أكد(0);
      }
}

تهی vvp_fun_arrayport_sa::check_word_change(unsigned long addr)
{
      اگر (addr != addr_) بازگشت;

      اگر (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr_), 0);
      } وگرنه {
	    net_->send_vec4(arr_->get_word(addr_), 0);
      }
}

رده vvp_fun_arrayport_aa  : عمومی vvp_fun_arrayport, عمومی automatic_hooks_s {

    عمومی:
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_aa() override;

      تهی alloc_instance(vvp_context_t context) override;
      تهی reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      تهی free_instance(vvp_context_t context) override;
#endif

      تهی check_word_change(unsigned long addr) override;

      تهی recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t context) override;

    خصوصی:
      تهی check_word_change_(unsigned long addr, vvp_context_t context);

      __vpiScope*context_scope_;
      unsigned context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(این, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(این, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

تهی vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      unsigned long*addr = جدید unsigned long;
      vvp_set_context_item(context, context_idx_, addr);

      *addr = addr_;
}

تهی vvp_fun_arrayport_aa::reset_instance(vvp_context_t context)
{
      unsigned long*addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));

      *addr = addr_;
}

#ifdef CHECK_WITH_VALGRIND
تهی vvp_fun_arrayport_aa::free_instance(vvp_context_t context)
{
      unsigned long*addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));
      delete addr;
}
#endif

تهی vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                     vvp_context_t context)
{
      اگر (context) {
            unsigned long*addr = static_cast<unsigned long*>
                  (vvp_get_context_item(context, context_idx_));

            منطقی addr_valid_flag;

            انتخاب (port.port()) {

                حالت 0: // Address input
                  addr_valid_flag = vector4_to_مقدار(bit, *addr);
                  اگر (! addr_valid_flag) *addr = arr_->get_size();
                  اگر (vpi_array_is_real(arr_)) {
			port.ptr()->send_real(arr_->get_word_r(*addr),
					      context);
                  } وگرنه {
			port.ptr()->send_vec4(arr_->get_word(*addr),
					      context);
                  }
                  بشکن;

                پیشفرض:
                  fprintf(stdout, "XXXX write ports not implemented.\n");
                  أكد(0);
            }
      } وگرنه {
            context = context_scope_->live_contexts;
            وقتی (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

تهی vvp_fun_arrayport_aa::check_word_change_(unsigned long addr,
                                              vvp_context_t context)
{
      ثابت unsigned long*port_addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));

      اگر (addr != *port_addr)
	    بازگشت;

      اگر (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr), context);
      } وگرنه {
	    net_->send_vec4(arr_->get_word(addr), context);
      }
}

تهی vvp_fun_arrayport_aa::check_word_change(unsigned long addr)
{
      اگر (arr_->get_scope()->is_automatic()) {
            أكد(vthread_get_wt_context());
            check_word_change_(addr, vthread_get_wt_context());
      } وگرنه {
            vvp_context_t context = context_scope_->live_contexts;
            وقتی (context) {
                  check_word_change_(addr, context);
                  context = vvp_get_next_context(context);
            }
      }
}

ایستا تهی array_attach_port(vvp_array_t array, vvp_fun_arrayport*fun)
{
      أكد(fun->next_ == 0);
      fun->next_ = array->ports_;
      array->ports_ = fun;
      اگر (!array->get_scope()->is_automatic() &&
          (array->vals4 || array->vals)) {
              /* propagate initial values for variable arrays */
            اگر (!vpi_array_is_real(array)) {
                  vvp_bit4_t init;
                  اگر (array->vals4)
                      init = BIT4_X;
                  وگرنه
                      init = BIT4_0;
                  vvp_vector4_t tmp(array->vals_width, init);
                  schedule_init_propagate(fun->net_, tmp);
            } وگرنه {
                  schedule_init_propagate(fun->net_, 0.0);
            }
      }
}

رده array_word_مقدار_callback : عمومی مقدار_callback {
    عمومی:
      درونخطی explicit array_word_مقدار_callback(p_cb_data data, long addr)
      : مقدار_callback(data), word_addr(addr)
      { }

    عمومی:
      long word_addr;
};

تهی __vpiArray::word_change(unsigned long addr)
{
      برای (vvp_fun_arrayport*cur = ports_; cur; cur = cur->next_)
	    cur->check_word_change(addr);

	// Run callbacks attached to the array itself.
      ساختار __vpiCallback *next = vpi_callbacks;
      ساختار __vpiCallback *prev = 0;

      وقتی (next) {
	    array_word_مقدار_callback*cur = dynamic_cast<array_word_مقدار_callback*>(next);
	    next = cur->next;

	      // Skip callbacks that are not for me. -1 is for every element.
	    اگر (cur->word_addr != (long)addr && cur->word_addr != -1) {
		  prev = cur;
		  ادامه;
	    }

	      // For whole array callbacks we need to set the index.
	    اگر (cur->word_addr == -1) {
		  cur->cb_data.index = (PLI_INT32) ((صحیح)addr +
						    first_addr.get_مقدار());
	    }

	    اگر (cur->cb_data.cb_rtn != 0) {
		  اگر (cur->test_مقدار_callback_ready()) {
			اگر (cur->cb_data.value) {
			      اگر (vpi_array_is_real(این)) {
				    دوگانه val = 0.0;
				    اگر (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_real_get_مقدار(val, cur->cb_data.value);
			      } وگرنه اگر (vals4) {
				    vpip_vec4_get_مقدار(vals4->get_word(addr),
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } وگرنه اگر (dynamic_cast<vvp_darray_atom<int8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int64_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint64_t>*>(vals)
				      || dynamic_cast<vvp_darray_vec2*>(vals)) {
				    vvp_vector4_t val;
				    اگر (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_vec4_get_مقدار(val,
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } وگرنه {
			            أكد(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } وگرنه اگر (prev == 0) {

		  vpi_callbacks = next;
		  cur->next = 0;
		  delete cur;

	    } وگرنه {
		  أكد(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}

رده array_resolv_list_t : عمومی resolv_list_s {

    عمومی:
      explicit array_resolv_list_t(نویسه*lab) : resolv_list_s(lab) {
	    array = 0;
      }

      vvp_array_t*array;
      منطقی resolve(منطقی mes) override;

    خصوصی:
};

منطقی array_resolv_list_t::resolve(منطقی mes)
{
      *array = array_find(label());
      اگر (*array == 0) {
	    أكد(!mes);
	    بازگشت نادرست;
      }
      بازگشت درست;
}

رده array_port_resolv_list_t : عمومی resolv_list_s {

    عمومی:
      explicit array_port_resolv_list_t(نویسه* lab, منطقی use_addr__,
                                        long addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      منطقی use_addr;
      long addr;
      منطقی resolve(منطقی mes) override;

    خصوصی:
};

array_port_resolv_list_t::array_port_resolv_list_t(نویسه *lab, منطقی use_addr__,
                                                   long addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      اگر (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      وگرنه
            context_scope = 0;
      ptr = جدید vvp_net_t;
}

منطقی array_port_resolv_list_t::resolve(منطقی mes)
{
      vvp_array_t mem = array_find(label());
      اگر (mem == 0) {
	    أكد(mem || !mes);
	    بازگشت نادرست;
      }

      vvp_fun_arrayport*fun;
      اگر (use_addr)
            اگر (context_scope)
                  fun = جدید vvp_fun_arrayport_aa(context_scope, mem, ptr, addr);
            وگرنه
                  fun = جدید vvp_fun_arrayport_sa(mem, ptr, addr);
      وگرنه
            اگر (context_scope)
                  fun = جدید vvp_fun_arrayport_aa(context_scope, mem, ptr);
            وگرنه
                  fun = جدید vvp_fun_arrayport_sa(mem, ptr);
      ptr->fun = fun;

      array_attach_port(mem, fun);

      بازگشت درست;
}

رده array_word_part_callback : عمومی array_word_مقدار_callback {
    عمومی:
      explicit array_word_part_callback(p_cb_data data, long addr);
      ~array_word_part_callback() override;

      array_word_part_callback(ثابت array_word_part_callback&) = delete;
      array_word_part_callback& operator=(ثابت array_word_part_callback&) = delete;

      منطقی test_مقدار_callback_ready(تهی) override;

    خصوصی:
      نویسه*مقدار_bits;
};

array_word_part_callback::array_word_part_callback(p_cb_data data, long addr)
: array_word_مقدار_callback(data, addr)
{
	// Get the initial value of the part, to use as a reference.
      ساختار __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj);
      s_vpi_مقدار tmp_مقدار;
      tmp_مقدار.format = vpiBinStrVal;
      apvword->vpi_get_مقدار(&tmp_مقدار);

      مقدار_bits = جدید نویسه[apvword->part_wid+1];

      memcpy(مقدار_bits, tmp_مقدار.value.str, apvword->part_wid);
      مقدار_bits[apvword->part_wid] = 0;
}

array_word_part_callback::~array_word_part_callback()
{
      delete[]مقدار_bits;
}

منطقی array_word_part_callback::test_مقدار_callback_ready(تهی)
{
      ساختار __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_data.obj);
      أكد(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_مقدار tmp_مقدار;
      tmp_مقدار.format = vpiBinStrVal;
      apvword->vpi_get_مقدار(&tmp_مقدار);

      اگر (memcmp(مقدار_bits, tmp_مقدار.value.str, apvword->part_wid) == 0)
	    بازگشت نادرست;

      memcpy(مقدار_bits, tmp_مقدار.value.str, apvword->part_wid);
      بازگشت درست;

}

مقدار_callback*vpip_array_word_change(p_cb_data data)
{
      ساختار __vpiArray*parent = 0;
      array_word_مقدار_callback*cbh = 0;
      اگر (ثابت ساختار __vpiArrayWord*word = array_var_word_from_handle(data->obj)) {
	    parent = static_cast<__vpiArray*>(word->get_parent());
	    unsigned addr = word->get_index();
	    cbh = جدید array_word_مقدار_callback(data, addr);

      } وگرنه اگر (ساختار __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(data->obj)) {
	    parent = tword->array;
	    cbh = جدید array_word_مقدار_callback(data, tword->address);

      } وگرنه اگر (ساختار __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj)) {
	    parent = apvword->array;
	    cbh = جدید array_word_part_callback(data, apvword->word_sel);
      }

      أكد(cbh);
      أكد(parent);
      cbh->next = parent->vpi_callbacks;
      parent->vpi_callbacks = cbh;

      بازگشت cbh;
}

مقدار_callback* vpip_array_change(p_cb_data data)
{
      array_word_مقدار_callback*cbh = جدید array_word_مقدار_callback(data, -1);
      أكد(data->obj);

      ساختار __vpiArray*arr = dynamic_cast<__vpiArray*>(data->obj);
      cbh->next = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      بازگشت cbh;
}

تهی compile_array_port(نویسه*label, نویسه*array, نویسه*addr)
{
      array_port_resolv_list_t*resolv_mem
	    = جدید array_port_resolv_list_t(array, نادرست, 0);

      define_functor_نماد(label, resolv_mem->ptr);
      آزاد(label);
	// Connect the port-0 input as the address.
      درون_داد_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

تهی compile_array_port(نویسه*label, نویسه*array, long addr)
{
      array_port_resolv_list_t*resolv_mem
	    = جدید array_port_resolv_list_t(array, درست, addr);

      define_functor_نماد(label, resolv_mem->ptr);
      آزاد(label);

      resolv_submit(resolv_mem);
}

تهی compile_array_alias(نویسه*label, نویسه*name, نویسه*src)
{
      vvp_array_t mem = array_find(src);
      أكد(mem);

      ساختار __vpiArray*obj = جدید __vpiArray;

      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_شمار = mem->array_شمار;
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
      obj->vals_واژگان = mem->vals_واژگان;

      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

      أكد(array_table);
      أكد(!array_find(label));
      array_table->sym_set_مقدار(label, obj);

      compile_vpi_نماد(label, obj);
      vpip_attach_to_current_scope(obj);

      آزاد(label);
      آزاد(name);
      آزاد(src);
}

/*
 * &A<label,addr>
 * This represents a VPI handle for an addressed array. This comes
 * from expressions like "label[addr]" where "label" is the array and
 * "addr" is the canonical address of the desired word.
 */
vpiHandle vpip_make_vthr_A(نویسه*label, unsigned addr)
{
      ساختار __vpiArrayVthrA*obj = جدید __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = جدید array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      obj->address = addr;

      بازگشت obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(نویسه*label, نویسه*نماد)
{
      ساختار __vpiArrayVthrA*obj = جدید __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = جدید array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      compile_vpi_lookup(&obj->address_handle, نماد);
      obj->address = 0;

      بازگشت obj;
}

vpiHandle vpip_make_vthr_A(نویسه*label, vpiHandle handle)
{
      ساختار __vpiArrayVthrA*obj = جدید __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = جدید array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = handle;
      obj->address = 0;

      بازگشت obj;
}

vpiHandle vpip_make_vthr_APV(نویسه*label, unsigned index, unsigned bit, unsigned wid)
{
      ساختار __vpiArrayVthrAPV*obj = جدید __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = جدید array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->word_sel = index;
      obj->part_bit = bit;
      obj->part_wid = wid;

      بازگشت obj;
}

تهی compile_array_cleanup(تهی)
{
      delete array_table;
      array_table = 0;
}

#ifdef CHECK_WITH_VALGRIND
تهی memory_delete(vpiHandle item)
{
      ساختار __vpiArray*arr = static_cast<ساختار __vpiArray*>(item);
      اگر (arr->vals_واژگان) delete [] (arr->vals_واژگان-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals;

      اگر (arr->nets) {
	    برای (unsigned idx = 0; idx < arr->get_size(); idx += 1) {
		  اگر (ساختار __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.index);
		    /* These should only be the real words. */
		  } وگرنه {
			أكد(arr->nets[idx]->get_type_code() ==
			       vpiRealVar);
			ساختار __vpiRealVar *sigr = static_cast<ساختار __vpiRealVar *>
			                            (arr->nets[idx]);
			constant_delete(sigr->id.index);
// Why are only the real words still here?
			delete arr->nets[idx];
		  }
	    }
	    آزاد(arr->nets);
      }

      وقتی (arr->vpi_callbacks) {
	    ساختار __vpiCallback*tmp = arr->vpi_callbacks->next;
	    delete arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      delete arr;
}

تهی A_delete(vpiHandle item)
{
      ساختار __vpiArrayVthrA*obj = static_cast<ساختار __vpiArrayVthrA*>(item);
      اگر (obj->address_handle) {
	    انتخاب (obj->address_handle->get_type_code()) {
		حالت vpiMemoryWord:
		  اگر (vpi_get(_vpiFromThr, obj->address_handle) == _vpi_at_A) {
			A_delete(obj->address_handle);
		  }
		  بشکن;
		حالت vpiPartSelect:
		  أكد(vpi_get(_vpiFromThr, obj->address_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->address_handle);
		  بشکن;
	    }
      }

      delete obj;
}

تهی APV_delete(vpiHandle item)
{
      ساختار __vpiArrayVthrAPV*obj = static_cast<ساختار __vpiArrayVthrAPV*>(item);
      delete obj;
}
#endif
