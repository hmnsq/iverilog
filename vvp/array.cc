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
#ضمن  "vvp_cleanup.h"
#endif
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <iostream>
# include  "compile.h"
# include  <cassert>
# include  "ivl_alloc.h"

using نطاق std;

unsigned long count_net_arrays = 0;
unsigned long count_net_array_words = 0;
unsigned long count_var_arrays = 0;
unsigned long count_var_array_words = 0;
unsigned long count_real_arrays = 0;
unsigned long count_real_array_words = 0;

ساكن symbol_map_s<بنية __vpiArray>* array_table =0;

מחלקה vvp_fun_arrayport;
ساكن فراغ array_attach_port(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_find(ثابت حرف*label)
{
      אם (array_table == 0)
	    החזר 0;

      vvp_array_t v = array_table->sym_get_value(label);
      החזר v;
}

بنية __vpiArrayVthrA : عام __vpiHandle {
      صحيح get_type_code(فراغ) ثابت override { החזר vpiMemoryWord; }
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      فراغ vpi_get_value(p_vpi_value val) override;
      vpiHandle vpi_put_value(p_vpi_value val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح code) override;

      بنية __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle address_handle;
	// If wid==0, then address is the address into the array.
      unsigned address;

      unsigned get_address() ثابت
      {
	    אם (address_handle) {
		  s_vpi_value vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpiVectorVal;
		  address_handle->vpi_get_value(&vp);
		  صحيح words = (address_handle->vpi_get(vpiSize)-1)/32 + 1;
		  עבור(صحيح idx = 0; idx < words; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			אם (vp.value.vector[idx].bval != 0) החזר UINT_MAX;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  address_handle->vpi_get_value(&vp);
		  החזר vp.value.integer;
	    }

	    החזר address;
      }
};


بنية __vpiArrayVthrAPV : عام __vpiHandle {
      صحيح get_type_code(فراغ) ثابت override { החזר vpiPartSelect; }
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      فراغ vpi_get_value(p_vpi_value val) override;
      vpiHandle vpi_handle(صحيح code) override;

      بنية __vpiArray*array;
      unsigned word_sel;
      unsigned part_bit;
      unsigned part_wid;
};

منطقي is_net_array(vpiHandle obj)
{
      بنية __vpiArray*rfp = dynamic_cast<__vpiArray*> (obj);
      أكد(rfp);

      אם (rfp->nets != 0) החזר אמת;
      החזר שקר;
}

// This function return true if the underlying array words are real.
ساكن منطقي vpi_array_is_real(ثابت vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      אם (arr->vals4 != 0)  // A bit based variable/register array.
	    החזר שקר;

      אם (dynamic_cast<vvp_darray_real*> (arr->vals))
	    החזר אמת;

      אם (arr->vals != 0)
	    החזר שקר;

	// This must be a net array so look at element 0 to find the type.
      أكد(arr->nets != 0);
      أكد(arr->get_size() > 0);
      ثابت بنية __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      אם (rsig) {
	    החזר אמת;
      }

      החזר שקר;
}

ساكن منطقي vpi_array_is_string(ثابت vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      אם (arr->vals4 != 0)  // A bit based variable/register array.
	    החזר שקר;

      אם (dynamic_cast<vvp_darray_string*> (arr->vals))
	    החזר אמת;

      החזר שקר;
}

صحيح __vpiArray::get_word_size() ثابت
{
      unsigned width;

      أكد(get_size() > 0);
	/* For a net array we need to get the width from the first element. */
      אם (nets) {
	    أكد(vals4 == 0 && vals == 0);
	    ثابت بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    أكد(vsig);
	    width = vpip_size(vsig);
	/* For a variable array we can get the width from vals_width. */
      } אחרת {
	    أكد(vals4 || vals);
	    width = vals_width;
      }

      החזר width;
}

حرف*__vpiArray::get_word_str(بنية __vpiArrayWord*word, صحيح code)
{
      unsigned index = word->get_index();

      אם (code == vpiFile) {  // Not implemented for now!
	    החזר simple_set_rbuf_str(file_names[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%d", (صحيح)index + first_addr.get_value());
      החזר generic_get_str(code, get_scope(), name, sidx);
}

فراغ __vpiArray::get_word_value(بنية __vpiArrayWord*word, p_vpi_value vp)
{
      unsigned index = word->get_index();

    // Determine the appropriate format (The Verilog PLI Handbook 5.2.10)
      אם(vp->format == vpiObjTypeVal) {
          אם(vpi_array_is_real(هذا))
              vp->format = vpiRealVal;
          אחרת אם(vpi_array_is_string(هذا))
              vp->format = vpiStringVal;
          אחרת
              vp->format = vpiIntVal;
      }

      אם(vals4) {
          vpip_vec4_get_value(vals4->get_word(index),
                              vals4->width(), signed_flag, vp);
      } אחרת אם(vals) {
          بدل(vp->format) {
            حالة vpiBinStrVal:
            حالة vpiOctStrVal:
            حالة vpiDecStrVal:
            حالة vpiHexStrVal:
            حالة vpiScalarVal:
            حالة vpiIntVal:
            حالة vpiVectorVal:
            {
                vvp_vector4_t v;
                vals->get_word(index, v);
                vpip_vec4_get_value(v, vals_width, signed_flag, vp);
            }
            اكسر;

            حالة vpiRealVal:
            {
                مزدوج d;
                vals->get_word(index, d);
                vpip_real_get_value(d, vp);
            }
            اكسر;

            حالة vpiStringVal:
            {
                string s;
                vals->get_word(index, s);
                vpip_string_get_value(s, vp);
            }
            اكسر;

            افتراضي:
                fprintf(stderr, "vpi sorry: format is not implemented\n");
                أكد(שקר);
          }
      }
}

فراغ __vpiArray::put_word_value(بنية __vpiArrayWord*word, p_vpi_value vp, صحيح)
{
      unsigned index = word->get_index();
      vvp_vector4_t val = vec4_from_vpi_value(vp, vals_width);
      set_word(index, 0, val);
}

vpiHandle __vpiArray::get_iter_index(بنية __vpiArrayIterator*, صحيح idx)
{
      אם (nets) החזר nets[idx];

      أكد(vals4 || vals);

      אם (vals_words == 0) make_vals_words();

      החזר &(vals_words[idx].as_word);
}

صحيح __vpiArray::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiLineNo:
	    החזר 0; // Not implemented for now!

	  حالة vpiSize:
	    החזר get_size();

	  حالة vpiAutomatic:
	    החזר scope->is_automatic()? 1 : 0;

	  افتراضي:
	    החזר 0;
      }
}

حرف* __vpiArray::vpi_get_str(صحيح code)
{
      אם (code == vpiFile) {  // Not implemented for now!
            החזר simple_set_rbuf_str(file_names[0]);
      }

      החזר generic_get_str(code, scope, name, عدم);
}

vpiHandle __vpiArray::vpi_handle(صحيح code)
{
      بدل (code) {
          حالة vpiLeftRange:
            אם (swap_addr) החזר &last_addr;
            אחרת החזר &first_addr;

          حالة vpiRightRange:
            אם (swap_addr) החזר &first_addr;
            אחרת החזר &last_addr;

	  حالة vpiScope:
	    החזר scope;

	  حالة vpiModule:
	    החזר vpip_module(scope);
      }

      החזר 0;
}

/*
* VPI code passes indices that are not yet converted to canonical
* form, so this index function does it here.
*/
vpiHandle __vpiArray::vpi_index(صحيح index)
{
      index -= first_addr.get_value();
      אם (index >= (long) get_size())
	    החזר 0;
      אם (index < 0)
	    החזר 0;

      אם (nets != 0) {
	    החזר nets[index];
      }

      אם (vals_words == 0)
	    make_vals_words();

      החזר &(vals_words[index].as_word);
}

صحيح __vpiArrayWord::as_word_t::vpi_get(صحيح code)
{
      بنية __vpiArrayWord*obj = array_var_word_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_parent = obj->get_parent();
      t_vpi_value val;

      بدل (code) {
	  حالة vpiLineNo:
	    החזר 0; // Not implemented for now!

	  حالة vpiSize:
	    החזר my_parent->get_word_size();

	  حالة vpiLeftRange:
            val.format = vpiIntVal;
	    my_parent->get_left_range()->vpi_get_value(&val);
            أكد(val.format == vpiIntVal);
            החזר val.value.integer;

	  حالة vpiRightRange:
            val.format = vpiIntVal;
	    my_parent->get_right_range()->vpi_get_value(&val);
            أكد(val.format == vpiIntVal);
            החזר val.value.integer;

	  حالة vpiIndex:
	    {
		  صحيح base_offset = 0;
		  بنية __vpiArray*base = dynamic_cast<__vpiArray*> (my_parent);
		  אם (base) {
			val.format = vpiIntVal;
			base->first_addr.vpi_get_value(&val);
			base_offset += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_index.vpi_get_value(&val);
		  أكد(val.format == vpiIntVal);
		  החזר val.value.integer + base_offset;
	    }

	  حالة vpiAutomatic:
	    החזר my_parent->get_scope()->is_automatic()? 1 : 0;

#אם defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    החזר _vpiNoThr;
#endif

	  افتراضي:
	    החזר 0;
      }
}


صحيح __vpiArrayVthrA::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiLineNo:
	    החזר 0; // Not implemented for now!

	  حالة vpiSize:
	    החזר array->get_word_size();

	  حالة vpiLeftRange:
	    החזר array->msb.get_value();

	  حالة vpiRightRange:
	    החזר array->lsb.get_value();

	  حالة vpiIndex:
	    החזר (صحيح)get_address() + array->first_addr.get_value();

	  حالة vpiAutomatic:
	    החזר array->get_scope()->is_automatic() ? 1 : 0;

#אם defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    החזר _vpi_at_A;
#endif

	  // If address_handle is not zero we definitely have a
	  // variable.
	  حالة vpiConstantSelect:
	    החזר address_handle == 0;

	  افتراضي:
	    החזר 0;
      }
}

حرف* __vpiArrayVthrA::vpi_get_str(صحيح code)
{
      אם (code == vpiFile) {  // Not implemented for now!
            החזר simple_set_rbuf_str(file_names[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%d", (صحيح)get_address() + array->first_addr.get_value());
      החזר generic_get_str(code, array->get_scope(), array->name, sidx);
}

فراغ __vpiArrayVthrA::vpi_get_value(p_vpi_value vp)
{
      أكد(array);

      unsigned index = get_address();
      אם (vpi_array_is_real(array)) {
	    مزدوج tmp = array->get_word_r(index);
	    vpip_real_get_value(tmp, vp);
      } אחרת אם (vpi_array_is_string(array)) {
	    string tmp = array->get_word_str(index);
	    vpip_string_get_value(tmp, vp);
      } אחרת {
	    vvp_vector4_t tmp = array->get_word(index);
	    unsigned width = array->get_word_size();
	    vpip_vec4_get_value(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_value(p_vpi_value vp, صحيح)
{
      unsigned index = get_address();

      أكد(array);
      أكد(index < array->get_size());

      אם (vpi_array_is_real(array)) {
	    مزدوج val = real_from_vpi_value(vp);
	    array->set_word(index, val);
      } אחרת {
	    unsigned width = array->get_word_size();
	    vvp_vector4_t val = vec4_from_vpi_value(vp, width);
	    array->set_word(index, 0, val);
      }

      החזר هذا;
}

vpiHandle __vpiArrayVthrA::vpi_handle(صحيح code)
{
      بدل (code) {
	  حالة vpiIndex:
	    اكسر;  // Not implemented!

	  حالة vpiLeftRange:
	    החזר &array->msb;

	  حالة vpiRightRange:
	    החזר &array->lsb;

	  حالة vpiParent:
	  حالة vpiArray:
	    החזר array;

	  حالة vpiScope:
	    החזר array->get_scope();

	  حالة vpiModule:
	    החזר vpip_module(array->get_scope());
      }

      החזר 0;
}


صحيح __vpiArrayVthrAPV::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiLineNo:
	    החזר 0; // Not implemented for now!

	  حالة vpiSize:
	    החזר part_wid;

	  حالة vpiLeftRange:
	    החזר part_bit + part_wid - 1;

	  حالة vpiRightRange:
	    החזר part_bit;

	  حالة vpiAutomatic:
	    החזר array->get_scope()->is_automatic() ? 1 : 0;

#אם defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    החזר _vpi_at_APV;
#endif

	  حالة vpiConstantSelect:
	    החזר 1;

	  افتراضي:
	    החזר 0;
      }
}

حرف* __vpiArrayVthrAPV::vpi_get_str(صحيح code)
{
      אם (code == vpiFile) {  // Not implemented for now!
            החזר simple_set_rbuf_str(file_names[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%u", word_sel + array->first_addr.get_value());
      החזר generic_get_str(code, array->get_scope(), array->name, sidx);
}

فراغ __vpiArrayVthrAPV::vpi_get_value(p_vpi_value vp)
{
      أكد(array);

      unsigned index = word_sel;
      אם (vpi_array_is_real(array)) {
	    مزدوج tmp = array->get_word_r(index);
	    vpip_real_get_value(tmp, vp);
      } אחרת {
	    vvp_vector4_t tmp = array->get_word(index);
	    tmp = tmp.subvalue(part_bit, part_wid);
	    vpip_vec4_get_value(tmp, part_wid, שקר, vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(صحيح code)
{
      بدل (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  حالة vpiParent:
	    החזר 0;

            // Not part of the Verilog standard. We use this internally.
	  حالة vpiArray:
	    החזר array;

	  حالة vpiScope:
	    החזר array->get_scope();

	  حالة vpiModule:
	    החזר vpip_module(array->get_scope());
      }

      החזר 0;
}

فراغ __vpiArray::set_word(unsigned address, unsigned part_off, ثابت vvp_vector4_t&val)
{
      אם (address >= get_size())
	    החזר;

      אם (vals4) {
	    أكد(nets == 0);
	    אם (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp = vals4->get_word(address);
		  אם ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			أكد(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals4->set_word(address, tmp);
	    } אחרת {
		  vals4->set_word(address, val);
	    }
	    word_change(address);
	    החזר;
      }

      אם (vals) {
	    أكد(nets == 0);
	    אם (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp;
		  vals->get_word(address, tmp);
		  אם ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			أكد(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals->set_word(address, tmp);
	    } אחרת {
		  vals->set_word(address, val);
	    }
	    word_change(address);
	    החזר;
      }

      أكد(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle word = nets[address];
      ثابت بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      أكد(vsig);

      vsig->node->send_vec4_pv(val, part_off, vpip_size(vsig), 0);
      word_change(address);
}

فراغ __vpiArray::set_word(unsigned address, مزدوج val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      אם (address >= vals->get_size())
	    החזר;

      vals->set_word(address, val);
      word_change(address);
}

فراغ __vpiArray::set_word(unsigned address, ثابت string&val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      אם (address >= vals->get_size())
	    החזר;

      vals->set_word(address, val);
      word_change(address);
}

فراغ __vpiArray::set_word(unsigned address, ثابت vvp_object_t&val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      אם (address >= vals->get_size())
	    החזר;

      vals->set_word(address, val);
      word_change(address);
}

vvp_vector4_t __vpiArray::get_word(unsigned address)
{
      אם (vals4) {
	    أكد(nets == 0);
	    أكد(vals == 0);
	    החזר vals4->get_word(address);
      }

      אם (vals) {
	    أكد(nets == 0);
	    أكد(vals4== 0);
	    אם (address >= vals->get_size())
		  החזר vvp_vector4_t(vals_width, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_word(address, val);
	    החזר val;
      }

      أكد(vals4 == 0);
      أكد(vals == 0);
      أكد(nets != 0);

      אם (address >= get_size()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    أكد(get_size() > 0);
	    vpiHandle word = nets[0];
	    أكد(word);
	    بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
	    أكد(vsig);
	    vvp_signal_value*sig = dynamic_cast<vvp_signal_value*> (vsig->node->fil);
	    أكد(sig);
	    החזר vvp_vector4_t(sig->value_size(), BIT4_X);
      }

      vpiHandle word = nets[address];
      بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      أكد(vsig);
      vvp_signal_value*sig = dynamic_cast<vvp_signal_value*> (vsig->node->fil);
      أكد(sig);

      vvp_vector4_t val;
      sig->vec4_value(val);
      החזר val;
}

مزدوج __vpiArray::get_word_r(unsigned address)
{
      אם (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    אם (address >= vals->get_size())
		  החזר 0.0;

	    مزدوج val;
	    vals->get_word(address, val);
	    החזר val;
      }

      אם (address >= get_size())
	    החזר 0.0;

      أكد(nets);
      vpiHandle word = nets[address];
      بنية __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(word);
      أكد(vsig);
      vvp_signal_value*sig = dynamic_cast<vvp_signal_value*> (vsig->net->fil);
      أكد(sig);

      مزدوج val = sig->real_value();
      החזר val;
}

فراغ __vpiArray::get_word_obj(unsigned address, vvp_object_t&val)
{
      אם (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    אם (address >= vals->get_size()) {
		  val = vvp_object_t();
		  החזר;
	    }

	    vals->get_word(address, val);
	    החזר;
      }

      أكد(nets);
	// Arrays of string nets not implemented!
      أكد(0);
      החזר;
}

string __vpiArray::get_word_str(unsigned address)
{
      אם (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    אם (address >= vals->get_size())
		  החזר "";

	    string val;
	    vals->get_word(address, val);
	    החזר val;
      }

      أكد(nets);
	// Arrays of string nets not implemented!
      أكد(0);
      החזר "";
}

vpiHandle vpip_make_array(ثابت حرف*label, ثابت حرف*name,
                          صحيح first_addr, صحيح last_addr,
                          منطقي signed_flag)
{
      بنية __vpiArray*obj = جديد __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      אם (last_addr >= first_addr) {
	    obj->swap_addr = שקר;
      } אחרת {
	    obj->swap_addr = אמת;
	    صحيح tmp = last_addr;
	    last_addr = first_addr;
	    first_addr = tmp;
      }
      أكد(last_addr >= first_addr);
      unsigned array_count = last_addr+1-first_addr;

	// For now, treat all arrays as memories. This is not quite
	// correct, as arrays are arrays with memories a special case.
      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_count = array_count;

      obj->first_addr.set_value(first_addr);
      obj->last_addr .set_value(last_addr);

	// Start off now knowing if we are nets or variables.
      obj->nets = 0;
      obj->vals4 = 0;
      obj->vals  = 0;
      obj->vals_width = 0;
      obj->vals_words = 0;

	// Initialize (clear) the read-ports list.
      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

	/* Add this symbol to the array_symbols table for later lookup. */
      אם (!array_table)
	    array_table = جديد symbol_map_s<بنية __vpiArray>;

      أكد(!array_find(label));
      array_table->sym_set_value(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_symbol(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_to_current_scope(obj);

      החזר obj;
}

فراغ __vpiArray::alias_word(unsigned long addr, vpiHandle word, صحيح msb_, صحيح lsb_)
{
      أكد(msb.get_value() == msb_);
      أكد(lsb.get_value() == lsb_);
      أكد(addr < get_size());
      أكد(nets);
      nets[addr] = word;
}

فراغ __vpiArray::attach_word(unsigned addr, vpiHandle word)
{
      أكد(addr < get_size());
      أكد(nets);
      nets[addr] = word;

      אם (بنية __vpiSignal*sig = dynamic_cast<__vpiSignal*>(word)) {
	    vvp_net_t*net = sig->node;
	    أكد(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    أكد(fun);
	    fun->attach_as_word(هذا, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = هذا;
	    sig->id.index = جديد __vpiDecConst(addr + first_addr.get_value());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    החזר;
      }

      אם (بنية __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(word)) {
	    vvp_net_t*net = sig->net;
	    أكد(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    أكد(fun);
	    fun->attach_as_word(هذا, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = هذا;
	    sig->id.index = جديد __vpiDecConst(addr + first_addr.get_value());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = אמת;
	    החזר;
      }
}

فراغ compile_var_array(حرف*label, حرف*name, صحيح last, صحيح first,
		   صحيح msb, صحيح lsb, حرف signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last,
                                      signed_flag != 0);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_width = labs(msb-lsb) + 1;
      אם (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = جديد vvp_vector4array_aa(arr->vals_width,
						 arr->get_size());
      } אחרת {
            arr->vals4 = جديد vvp_vector4array_sa(arr->vals_width,
						 arr->get_size());
      }
      arr->msb.set_value(msb);
      arr->lsb.set_value(lsb);

      count_var_arrays += 1;
      count_var_array_words += arr->get_size();

      حرر(label);
      delete[] name;
}

فراغ compile_var2_array(حرف*label, حرف*name, صحيح last, صحيح first,
		   صحيح msb, صحيح lsb, منطقي signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, signed_flag);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.set_value(msb);
      arr->lsb.set_value(lsb);
      arr->vals_width = labs(msb-lsb) + 1;

      أكد(! arr->nets);
      אם (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = جديد vvp_darray_atom<int8_t>(arr->get_size());
      } אחרת אם (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = جديد vvp_darray_atom<uint8_t>(arr->get_size());
      } אחרת אם (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = جديد vvp_darray_atom<int16_t>(arr->get_size());
      } אחרת אם (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = جديد vvp_darray_atom<uint16_t>(arr->get_size());
      } אחרת אם (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = جديد vvp_darray_atom<int32_t>(arr->get_size());
      } אחרת אם (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = جديد vvp_darray_atom<uint32_t>(arr->get_size());
      } אחרת אם (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = جديد vvp_darray_atom<int64_t>(arr->get_size());
      } אחרת אם (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = جديد vvp_darray_atom<uint64_t>(arr->get_size());
      } אחרת {
	    arr->vals = جديد vvp_darray_vec2(arr->get_size(), arr->vals_width);
      }
      count_var_arrays += 1;
      count_var_array_words += arr->get_size();

      حرر(label);
      delete[] name;
}

فراغ compile_real_array(حرف*label, حرف*name, صحيح last, صحيح first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, אמת);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = جديد vvp_darray_real(arr->get_size());
      arr->vals_width = 1;

      count_real_arrays += 1;
      count_real_array_words += arr->get_size();

      حرر(label);
      delete[] name;
}

فراغ compile_string_array(حرف*label, حرف*name, صحيح last, صحيح first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, אמת);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = جديد vvp_darray_string(arr->get_size());
      arr->vals_width = 1;

      count_real_arrays += 1;
      count_real_array_words += arr->get_size();

      حرر(label);
      delete[] name;
}

فراغ compile_object_array(حرف*label, حرف*name, صحيح last, صحيح first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, אמת);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = جديد vvp_darray_object(arr->get_size());
      arr->vals_width = 1;

      count_real_arrays += 1;
      count_real_array_words += arr->get_size();

      حرر(label);
      delete[] name;
}

فراغ compile_net_array(حرف*label, حرف*name, صحيح last, صحيح first)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, first, last, שקר);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(calloc(arr->get_size(), حجم(vpiHandle)));

      count_net_arrays += 1;
      count_net_array_words += arr->get_size();

      حرر(label);
      delete[] name;
}

מחלקה vvp_fun_arrayport  : عام vvp_net_fun_t {

    عام:
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport() override;

      virtual فراغ check_word_change(unsigned long addr) = 0;

    محمي:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      unsigned long addr_;

      friend فراغ array_attach_port(vvp_array_t, vvp_fun_arrayport*);
      friend فراغ __vpiArray::word_change(unsigned long);
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

מחלקה vvp_fun_arrayport_sa  : عام vvp_fun_arrayport {

    عام:
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_sa() override;

      فراغ check_word_change(unsigned long addr) override;

      فراغ recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t) override;

    خاص:
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

فراغ vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                     vvp_context_t)
{
      منطقي addr_valid_flag;

      بدل (port.port()) {

	  حالة 0: // Address input
	    addr_valid_flag = vector4_to_value(bit, addr_);
	    אם (! addr_valid_flag)
		  addr_ = arr_->get_size();
	    אם (vpi_array_is_real(arr_))
		  port.ptr()->send_real(arr_->get_word_r(addr_), 0);
	    אחרת
		  port.ptr()->send_vec4(arr_->get_word(addr_), 0);

	    اكسر;

	  افتراضي:
	    fprintf(stdout, "XXXX write ports not implemented.\n");
	    أكد(0);
      }
}

فراغ vvp_fun_arrayport_sa::check_word_change(unsigned long addr)
{
      אם (addr != addr_) החזר;

      אם (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr_), 0);
      } אחרת {
	    net_->send_vec4(arr_->get_word(addr_), 0);
      }
}

מחלקה vvp_fun_arrayport_aa  : عام vvp_fun_arrayport, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#endif

      فراغ check_word_change(unsigned long addr) override;

      فراغ recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                     vvp_context_t context) override;

    خاص:
      فراغ check_word_change_(unsigned long addr, vvp_context_t context);

      __vpiScope*context_scope_;
      unsigned context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(هذا, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(هذا, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

فراغ vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      unsigned long*addr = جديد unsigned long;
      vvp_set_context_item(context, context_idx_, addr);

      *addr = addr_;
}

فراغ vvp_fun_arrayport_aa::reset_instance(vvp_context_t context)
{
      unsigned long*addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));

      *addr = addr_;
}

#ifdef CHECK_WITH_VALGRIND
فراغ vvp_fun_arrayport_aa::free_instance(vvp_context_t context)
{
      unsigned long*addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));
      delete addr;
}
#endif

فراغ vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
                                     vvp_context_t context)
{
      אם (context) {
            unsigned long*addr = static_cast<unsigned long*>
                  (vvp_get_context_item(context, context_idx_));

            منطقي addr_valid_flag;

            بدل (port.port()) {

                حالة 0: // Address input
                  addr_valid_flag = vector4_to_value(bit, *addr);
                  אם (! addr_valid_flag) *addr = arr_->get_size();
                  אם (vpi_array_is_real(arr_)) {
			port.ptr()->send_real(arr_->get_word_r(*addr),
					      context);
                  } אחרת {
			port.ptr()->send_vec4(arr_->get_word(*addr),
					      context);
                  }
                  اكسر;

                افتراضي:
                  fprintf(stdout, "XXXX write ports not implemented.\n");
                  أكد(0);
            }
      } אחרת {
            context = context_scope_->live_contexts;
            כלעוד (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

فراغ vvp_fun_arrayport_aa::check_word_change_(unsigned long addr,
                                              vvp_context_t context)
{
      ثابت unsigned long*port_addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));

      אם (addr != *port_addr)
	    החזר;

      אם (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr), context);
      } אחרת {
	    net_->send_vec4(arr_->get_word(addr), context);
      }
}

فراغ vvp_fun_arrayport_aa::check_word_change(unsigned long addr)
{
      אם (arr_->get_scope()->is_automatic()) {
            أكد(vthread_get_wt_context());
            check_word_change_(addr, vthread_get_wt_context());
      } אחרת {
            vvp_context_t context = context_scope_->live_contexts;
            כלעוד (context) {
                  check_word_change_(addr, context);
                  context = vvp_get_next_context(context);
            }
      }
}

ساكن فراغ array_attach_port(vvp_array_t array, vvp_fun_arrayport*fun)
{
      أكد(fun->next_ == 0);
      fun->next_ = array->ports_;
      array->ports_ = fun;
      אם (!array->get_scope()->is_automatic() &&
          (array->vals4 || array->vals)) {
              /* propagate initial values for variable arrays */
            אם (!vpi_array_is_real(array)) {
                  vvp_bit4_t init;
                  אם (array->vals4)
                      init = BIT4_X;
                  אחרת
                      init = BIT4_0;
                  vvp_vector4_t tmp(array->vals_width, init);
                  schedule_init_propagate(fun->net_, tmp);
            } אחרת {
                  schedule_init_propagate(fun->net_, 0.0);
            }
      }
}

מחלקה array_word_value_callback : عام value_callback {
    عام:
      مضمن explicit array_word_value_callback(p_cb_data data, long addr)
      : value_callback(data), word_addr(addr)
      { }

    عام:
      long word_addr;
};

فراغ __vpiArray::word_change(unsigned long addr)
{
      עבור (vvp_fun_arrayport*cur = ports_; cur; cur = cur->next_)
	    cur->check_word_change(addr);

	// Run callbacks attached to the array itself.
      بنية __vpiCallback *next = vpi_callbacks;
      بنية __vpiCallback *prev = 0;

      כלעוד (next) {
	    array_word_value_callback*cur = dynamic_cast<array_word_value_callback*>(next);
	    next = cur->next;

	      // Skip callbacks that are not for me. -1 is for every element.
	    אם (cur->word_addr != (long)addr && cur->word_addr != -1) {
		  prev = cur;
		  استمر;
	    }

	      // For whole array callbacks we need to set the index.
	    אם (cur->word_addr == -1) {
		  cur->cb_data.index = (PLI_INT32) ((صحيح)addr +
						    first_addr.get_value());
	    }

	    אם (cur->cb_data.cb_rtn != 0) {
		  אם (cur->test_value_callback_ready()) {
			אם (cur->cb_data.value) {
			      אם (vpi_array_is_real(هذا)) {
				    مزدوج val = 0.0;
				    אם (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_real_get_value(val, cur->cb_data.value);
			      } אחרת אם (vals4) {
				    vpip_vec4_get_value(vals4->get_word(addr),
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } אחרת אם (dynamic_cast<vvp_darray_atom<int8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int64_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint64_t>*>(vals)
				      || dynamic_cast<vvp_darray_vec2*>(vals)) {
				    vvp_vector4_t val;
				    אם (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_vec4_get_value(val,
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } אחרת {
			            أكد(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } אחרת אם (prev == 0) {

		  vpi_callbacks = next;
		  cur->next = 0;
		  delete cur;

	    } אחרת {
		  أكد(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}

מחלקה array_resolv_list_t : عام resolv_list_s {

    عام:
      explicit array_resolv_list_t(حرف*lab) : resolv_list_s(lab) {
	    array = 0;
      }

      vvp_array_t*array;
      منطقي resolve(منطقي mes) override;

    خاص:
};

منطقي array_resolv_list_t::resolve(منطقي mes)
{
      *array = array_find(label());
      אם (*array == 0) {
	    أكد(!mes);
	    החזר שקר;
      }
      החזר אמת;
}

מחלקה array_port_resolv_list_t : عام resolv_list_s {

    عام:
      explicit array_port_resolv_list_t(حرف* lab, منطقي use_addr__,
                                        long addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      منطقي use_addr;
      long addr;
      منطقي resolve(منطقي mes) override;

    خاص:
};

array_port_resolv_list_t::array_port_resolv_list_t(حرف *lab, منطقي use_addr__,
                                                   long addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      אם (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      אחרת
            context_scope = 0;
      ptr = جديد vvp_net_t;
}

منطقي array_port_resolv_list_t::resolve(منطقي mes)
{
      vvp_array_t mem = array_find(label());
      אם (mem == 0) {
	    أكد(mem || !mes);
	    החזר שקר;
      }

      vvp_fun_arrayport*fun;
      אם (use_addr)
            אם (context_scope)
                  fun = جديد vvp_fun_arrayport_aa(context_scope, mem, ptr, addr);
            אחרת
                  fun = جديد vvp_fun_arrayport_sa(mem, ptr, addr);
      אחרת
            אם (context_scope)
                  fun = جديد vvp_fun_arrayport_aa(context_scope, mem, ptr);
            אחרת
                  fun = جديد vvp_fun_arrayport_sa(mem, ptr);
      ptr->fun = fun;

      array_attach_port(mem, fun);

      החזר אמת;
}

מחלקה array_word_part_callback : عام array_word_value_callback {
    عام:
      explicit array_word_part_callback(p_cb_data data, long addr);
      ~array_word_part_callback() override;

      array_word_part_callback(ثابت array_word_part_callback&) = delete;
      array_word_part_callback& operator=(ثابت array_word_part_callback&) = delete;

      منطقي test_value_callback_ready(فراغ) override;

    خاص:
      حرف*value_bits_;
};

array_word_part_callback::array_word_part_callback(p_cb_data data, long addr)
: array_word_value_callback(data, addr)
{
	// Get the initial value of the part, to use as a reference.
      بنية __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj);
      s_vpi_value tmp_value;
      tmp_value.format = vpiBinStrVal;
      apvword->vpi_get_value(&tmp_value);

      value_bits_ = جديد حرف[apvword->part_wid+1];

      memcpy(value_bits_, tmp_value.value.str, apvword->part_wid);
      value_bits_[apvword->part_wid] = 0;
}

array_word_part_callback::~array_word_part_callback()
{
      delete[]value_bits_;
}

منطقي array_word_part_callback::test_value_callback_ready(فراغ)
{
      بنية __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_data.obj);
      أكد(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_value tmp_value;
      tmp_value.format = vpiBinStrVal;
      apvword->vpi_get_value(&tmp_value);

      אם (memcmp(value_bits_, tmp_value.value.str, apvword->part_wid) == 0)
	    החזר שקר;

      memcpy(value_bits_, tmp_value.value.str, apvword->part_wid);
      החזר אמת;

}

value_callback*vpip_array_word_change(p_cb_data data)
{
      بنية __vpiArray*parent = 0;
      array_word_value_callback*cbh = 0;
      אם (ثابت بنية __vpiArrayWord*word = array_var_word_from_handle(data->obj)) {
	    parent = static_cast<__vpiArray*>(word->get_parent());
	    unsigned addr = word->get_index();
	    cbh = جديد array_word_value_callback(data, addr);

      } אחרת אם (بنية __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(data->obj)) {
	    parent = tword->array;
	    cbh = جديد array_word_value_callback(data, tword->address);

      } אחרת אם (بنية __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj)) {
	    parent = apvword->array;
	    cbh = جديد array_word_part_callback(data, apvword->word_sel);
      }

      أكد(cbh);
      أكد(parent);
      cbh->next = parent->vpi_callbacks;
      parent->vpi_callbacks = cbh;

      החזר cbh;
}

value_callback* vpip_array_change(p_cb_data data)
{
      array_word_value_callback*cbh = جديد array_word_value_callback(data, -1);
      أكد(data->obj);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(data->obj);
      cbh->next = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      החזר cbh;
}

فراغ compile_array_port(حرف*label, حرف*array, حرف*addr)
{
      array_port_resolv_list_t*resolv_mem
	    = جديد array_port_resolv_list_t(array, שקר, 0);

      define_functor_symbol(label, resolv_mem->ptr);
      حرر(label);
	// Connect the port-0 input as the address.
      input_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

فراغ compile_array_port(حرف*label, حرف*array, long addr)
{
      array_port_resolv_list_t*resolv_mem
	    = جديد array_port_resolv_list_t(array, אמת, addr);

      define_functor_symbol(label, resolv_mem->ptr);
      حرر(label);

      resolv_submit(resolv_mem);
}

فراغ compile_array_alias(حرف*label, حرف*name, حرف*src)
{
      vvp_array_t mem = array_find(src);
      أكد(mem);

      بنية __vpiArray*obj = جديد __vpiArray;

      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_count = mem->array_count;
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
      obj->vals_words = mem->vals_words;

      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

      أكد(array_table);
      أكد(!array_find(label));
      array_table->sym_set_value(label, obj);

      compile_vpi_symbol(label, obj);
      vpip_attach_to_current_scope(obj);

      حرر(label);
      حرر(name);
      حرر(src);
}

/*
 * &A<label,addr>
 * This represents a VPI handle for an addressed array. This comes
 * from expressions like "label[addr]" where "label" is the array and
 * "addr" is the canonical address of the desired word.
 */
vpiHandle vpip_make_vthr_A(حرف*label, unsigned addr)
{
      بنية __vpiArrayVthrA*obj = جديد __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = جديد array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      obj->address = addr;

      החזר obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(حرف*label, حرف*symbol)
{
      بنية __vpiArrayVthrA*obj = جديد __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = جديد array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      compile_vpi_lookup(&obj->address_handle, symbol);
      obj->address = 0;

      החזר obj;
}

vpiHandle vpip_make_vthr_A(حرف*label, vpiHandle handle)
{
      بنية __vpiArrayVthrA*obj = جديد __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = جديد array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = handle;
      obj->address = 0;

      החזר obj;
}

vpiHandle vpip_make_vthr_APV(حرف*label, unsigned index, unsigned bit, unsigned wid)
{
      بنية __vpiArrayVthrAPV*obj = جديد __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = جديد array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->word_sel = index;
      obj->part_bit = bit;
      obj->part_wid = wid;

      החזר obj;
}

فراغ compile_array_cleanup(فراغ)
{
      delete array_table;
      array_table = 0;
}

#ifdef CHECK_WITH_VALGRIND
فراغ memory_delete(vpiHandle item)
{
      بنية __vpiArray*arr = static_cast<بنية __vpiArray*>(item);
      אם (arr->vals_words) delete [] (arr->vals_words-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals;

      אם (arr->nets) {
	    עבור (unsigned idx = 0; idx < arr->get_size(); idx += 1) {
		  אם (بنية __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.index);
		    /* These should only be the real words. */
		  } אחרת {
			أكد(arr->nets[idx]->get_type_code() ==
			       vpiRealVar);
			بنية __vpiRealVar *sigr = static_cast<بنية __vpiRealVar *>
			                            (arr->nets[idx]);
			constant_delete(sigr->id.index);
// Why are only the real words still here?
			delete arr->nets[idx];
		  }
	    }
	    حرر(arr->nets);
      }

      כלעוד (arr->vpi_callbacks) {
	    بنية __vpiCallback*tmp = arr->vpi_callbacks->next;
	    delete arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      delete arr;
}

فراغ A_delete(vpiHandle item)
{
      بنية __vpiArrayVthrA*obj = static_cast<بنية __vpiArrayVthrA*>(item);
      אם (obj->address_handle) {
	    بدل (obj->address_handle->get_type_code()) {
		حالة vpiMemoryWord:
		  אם (vpi_get(_vpiFromThr, obj->address_handle) == _vpi_at_A) {
			A_delete(obj->address_handle);
		  }
		  اكسر;
		حالة vpiPartSelect:
		  أكد(vpi_get(_vpiFromThr, obj->address_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->address_handle);
		  اكسر;
	    }
      }

      delete obj;
}

فراغ APV_delete(vpiHandle item)
{
      بنية __vpiArrayVthrAPV*obj = static_cast<بنية __vpiArrayVthrAPV*>(item);
      delete obj;
}
#endif
