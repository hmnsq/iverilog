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

unsigned long عدد_net_arrays = 0;
unsigned long عدد_net_array_كلمات = 0;
unsigned long عدد_var_arrays = 0;
unsigned long عدد_var_array_كلمات = 0;
unsigned long عدد_real_arrays = 0;
unsigned long عدد_real_array_كلمات = 0;

ساكن رمز_map_s<بنية __vpiArray>* array_table =0;

صنف vvp_fun_arrayport;
ساكن فراغ array_attach_port(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_find(ثابت حرف*label)
{
      إذا (array_table == 0)
	    أرجع 0;

      vvp_array_t v = array_table->sym_get_قيمة(label);
      أرجع v;
}

بنية __vpiArrayVthrA : عام __vpiHandle {
      صحيح get_type_code(فراغ) ثابت override { أرجع vpiMemoryWord; }
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      فراغ vpi_get_قيمة(p_vpi_قيمة val) override;
      vpiHandle vpi_put_قيمة(p_vpi_قيمة val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح code) override;

      بنية __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle address_handle;
	// If wid==0, then address is the address into the array.
      unsigned address;

      unsigned get_address() ثابت
      {
	    إذا (address_handle) {
		  s_vpi_قيمة vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpiVectorVal;
		  address_handle->vpi_get_قيمة(&vp);
		  صحيح كلمات = (address_handle->vpi_get(vpiSize)-1)/32 + 1;
		  لكل(صحيح idx = 0; idx < كلمات; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			إذا (vp.value.vector[idx].bval != 0) أرجع UINT_MAX;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  address_handle->vpi_get_قيمة(&vp);
		  أرجع vp.value.integer;
	    }

	    أرجع address;
      }
};


بنية __vpiArrayVthrAPV : عام __vpiHandle {
      صحيح get_type_code(فراغ) ثابت override { أرجع vpiPartSelect; }
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      فراغ vpi_get_قيمة(p_vpi_قيمة val) override;
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

      إذا (rfp->nets != 0) أرجع true;
      أرجع خطأ;
}

// This function return true if the underlying array words are real.
ساكن منطقي vpi_array_is_real(ثابت vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      إذا (arr->vals4 != 0)  // A bit based variable/register array.
	    أرجع خطأ;

      إذا (dynamic_cast<vvp_darray_real*> (arr->vals))
	    أرجع true;

      إذا (arr->vals != 0)
	    أرجع خطأ;

	// This must be a net array so look at element 0 to find the type.
      أكد(arr->nets != 0);
      أكد(arr->get_size() > 0);
      ثابت بنية __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      إذا (rsig) {
	    أرجع true;
      }

      أرجع خطأ;
}

ساكن منطقي vpi_array_is_string(ثابت vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      إذا (arr->vals4 != 0)  // A bit based variable/register array.
	    أرجع خطأ;

      إذا (dynamic_cast<vvp_darray_string*> (arr->vals))
	    أرجع true;

      أرجع خطأ;
}

صحيح __vpiArray::get_word_size() ثابت
{
      unsigned width;

      أكد(get_size() > 0);
	/* For a net array we need to get the width from the first element. */
      إذا (nets) {
	    أكد(vals4 == 0 && vals == 0);
	    ثابت بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    أكد(vsig);
	    width = vpip_size(vsig);
	/* For a variable array we can get the width from vals_width. */
      } وإلا {
	    أكد(vals4 || vals);
	    width = vals_width;
      }

      أرجع width;
}

حرف*__vpiArray::get_word_str(بنية __vpiArrayWord*word, صحيح code)
{
      unsigned index = word->get_index();

      إذا (code == vpiFile) {  // Not implemented for now!
	    أرجع simple_set_rbuf_str(file_names[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%d", (صحيح)index + first_addr.get_قيمة());
      أرجع generic_get_str(code, get_scope(), name, sidx);
}

فراغ __vpiArray::get_word_قيمة(بنية __vpiArrayWord*word, p_vpi_قيمة vp)
{
      unsigned index = word->get_index();

    // Determine the appropriate format (The Verilog PLI Handbook 5.2.10)
      إذا(vp->format == vpiObjTypeVal) {
          إذا(vpi_array_is_real(هذا))
              vp->format = vpiRealVal;
          وإلا إذا(vpi_array_is_string(هذا))
              vp->format = vpiStringVal;
          وإلا
              vp->format = vpiIntVal;
      }

      إذا(vals4) {
          vpip_vec4_get_قيمة(vals4->get_word(index),
                              vals4->width(), signed_flag, vp);
      } وإلا إذا(vals) {
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
                vpip_vec4_get_قيمة(v, vals_width, signed_flag, vp);
            }
            اكسر;

            حالة vpiRealVal:
            {
                مزدوج d;
                vals->get_word(index, d);
                vpip_real_get_قيمة(d, vp);
            }
            اكسر;

            حالة vpiStringVal:
            {
                string s;
                vals->get_word(index, s);
                vpip_string_get_قيمة(s, vp);
            }
            اكسر;

            افتراضي:
                fprintf(stderr, "vpi sorry: format is not implemented\n");
                أكد(خطأ);
          }
      }
}

فراغ __vpiArray::put_word_قيمة(بنية __vpiArrayWord*word, p_vpi_قيمة vp, صحيح)
{
      unsigned index = word->get_index();
      vvp_vector4_t val = vec4_from_vpi_قيمة(vp, vals_width);
      set_word(index, 0, val);
}

vpiHandle __vpiArray::get_iter_index(بنية __vpiArrayIterator*, صحيح idx)
{
      إذا (nets) أرجع nets[idx];

      أكد(vals4 || vals);

      إذا (vals_كلمات == 0) make_vals_كلمات();

      أرجع &(vals_كلمات[idx].as_word);
}

صحيح __vpiArray::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_سطر_No:
	    أرجع 0; // Not implemented for now!

	  حالة vpiSize:
	    أرجع get_size();

	  حالة vpiAutomatic:
	    أرجع scope->is_automatic()? 1 : 0;

	  افتراضي:
	    أرجع 0;
      }
}

حرف* __vpiArray::vpi_get_str(صحيح code)
{
      إذا (code == vpiFile) {  // Not implemented for now!
            أرجع simple_set_rbuf_str(file_names[0]);
      }

      أرجع generic_get_str(code, scope, name, عدم);
}

vpiHandle __vpiArray::vpi_handle(صحيح code)
{
      بدل (code) {
          حالة vpi_يسار_Range:
            إذا (swap_addr) أرجع &last_addr;
            وإلا أرجع &first_addr;

          حالة vpi_يمين_Range:
            إذا (swap_addr) أرجع &first_addr;
            وإلا أرجع &last_addr;

	  حالة vpiScope:
	    أرجع scope;

	  حالة vpiModule:
	    أرجع vpip_module(scope);
      }

      أرجع 0;
}

/*
* VPI code passes indices that are not yet converted to canonical
* form, so this index function does it here.
*/
vpiHandle __vpiArray::vpi_index(صحيح index)
{
      index -= first_addr.get_قيمة();
      إذا (index >= (long) get_size())
	    أرجع 0;
      إذا (index < 0)
	    أرجع 0;

      إذا (nets != 0) {
	    أرجع nets[index];
      }

      إذا (vals_كلمات == 0)
	    make_vals_كلمات();

      أرجع &(vals_كلمات[index].as_word);
}

صحيح __vpiArrayWord::as_word_t::vpi_get(صحيح code)
{
      بنية __vpiArrayWord*obj = array_var_word_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_parent = obj->get_parent();
      t_vpi_قيمة val;

      بدل (code) {
	  حالة vpi_سطر_No:
	    أرجع 0; // Not implemented for now!

	  حالة vpiSize:
	    أرجع my_parent->get_word_size();

	  حالة vpi_يسار_Range:
            val.format = vpiIntVal;
	    my_parent->get_يسار_range()->vpi_get_قيمة(&val);
            أكد(val.format == vpiIntVal);
            أرجع val.value.integer;

	  حالة vpi_يمين_Range:
            val.format = vpiIntVal;
	    my_parent->get_يمين_range()->vpi_get_قيمة(&val);
            أكد(val.format == vpiIntVal);
            أرجع val.value.integer;

	  حالة vpiIndex:
	    {
		  صحيح base_offset = 0;
		  بنية __vpiArray*base = dynamic_cast<__vpiArray*> (my_parent);
		  إذا (base) {
			val.format = vpiIntVal;
			base->first_addr.vpi_get_قيمة(&val);
			base_offset += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_index.vpi_get_قيمة(&val);
		  أكد(val.format == vpiIntVal);
		  أرجع val.value.integer + base_offset;
	    }

	  حالة vpiAutomatic:
	    أرجع my_parent->get_scope()->is_automatic()? 1 : 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    أرجع _vpiNoThr;
#endif

	  افتراضي:
	    أرجع 0;
      }
}


صحيح __vpiArrayVthrA::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_سطر_No:
	    أرجع 0; // Not implemented for now!

	  حالة vpiSize:
	    أرجع array->get_word_size();

	  حالة vpi_يسار_Range:
	    أرجع array->msb.get_قيمة();

	  حالة vpi_يمين_Range:
	    أرجع array->lsb.get_قيمة();

	  حالة vpiIndex:
	    أرجع (صحيح)get_address() + array->first_addr.get_قيمة();

	  حالة vpiAutomatic:
	    أرجع array->get_scope()->is_automatic() ? 1 : 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    أرجع _vpi_at_A;
#endif

	  // If address_handle is not zero we definitely have a
	  // variable.
	  حالة vpiConstantSelect:
	    أرجع address_handle == 0;

	  افتراضي:
	    أرجع 0;
      }
}

حرف* __vpiArrayVthrA::vpi_get_str(صحيح code)
{
      إذا (code == vpiFile) {  // Not implemented for now!
            أرجع simple_set_rbuf_str(file_names[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%d", (صحيح)get_address() + array->first_addr.get_قيمة());
      أرجع generic_get_str(code, array->get_scope(), array->name, sidx);
}

فراغ __vpiArrayVthrA::vpi_get_قيمة(p_vpi_قيمة vp)
{
      أكد(array);

      unsigned index = get_address();
      إذا (vpi_array_is_real(array)) {
	    مزدوج tmp = array->get_word_r(index);
	    vpip_real_get_قيمة(tmp, vp);
      } وإلا إذا (vpi_array_is_string(array)) {
	    string tmp = array->get_word_str(index);
	    vpip_string_get_قيمة(tmp, vp);
      } وإلا {
	    vvp_vector4_t tmp = array->get_word(index);
	    unsigned width = array->get_word_size();
	    vpip_vec4_get_قيمة(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_قيمة(p_vpi_قيمة vp, صحيح)
{
      unsigned index = get_address();

      أكد(array);
      أكد(index < array->get_size());

      إذا (vpi_array_is_real(array)) {
	    مزدوج val = real_from_vpi_قيمة(vp);
	    array->set_word(index, val);
      } وإلا {
	    unsigned width = array->get_word_size();
	    vvp_vector4_t val = vec4_from_vpi_قيمة(vp, width);
	    array->set_word(index, 0, val);
      }

      أرجع هذا;
}

vpiHandle __vpiArrayVthrA::vpi_handle(صحيح code)
{
      بدل (code) {
	  حالة vpiIndex:
	    اكسر;  // Not implemented!

	  حالة vpi_يسار_Range:
	    أرجع &array->msb;

	  حالة vpi_يمين_Range:
	    أرجع &array->lsb;

	  حالة vpiParent:
	  حالة vpiArray:
	    أرجع array;

	  حالة vpiScope:
	    أرجع array->get_scope();

	  حالة vpiModule:
	    أرجع vpip_module(array->get_scope());
      }

      أرجع 0;
}


صحيح __vpiArrayVthrAPV::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_سطر_No:
	    أرجع 0; // Not implemented for now!

	  حالة vpiSize:
	    أرجع part_wid;

	  حالة vpi_يسار_Range:
	    أرجع part_bit + part_wid - 1;

	  حالة vpi_يمين_Range:
	    أرجع part_bit;

	  حالة vpiAutomatic:
	    أرجع array->get_scope()->is_automatic() ? 1 : 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    أرجع _vpi_at_APV;
#endif

	  حالة vpiConstantSelect:
	    أرجع 1;

	  افتراضي:
	    أرجع 0;
      }
}

حرف* __vpiArrayVthrAPV::vpi_get_str(صحيح code)
{
      إذا (code == vpiFile) {  // Not implemented for now!
            أرجع simple_set_rbuf_str(file_names[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%u", word_sel + array->first_addr.get_قيمة());
      أرجع generic_get_str(code, array->get_scope(), array->name, sidx);
}

فراغ __vpiArrayVthrAPV::vpi_get_قيمة(p_vpi_قيمة vp)
{
      أكد(array);

      unsigned index = word_sel;
      إذا (vpi_array_is_real(array)) {
	    مزدوج tmp = array->get_word_r(index);
	    vpip_real_get_قيمة(tmp, vp);
      } وإلا {
	    vvp_vector4_t tmp = array->get_word(index);
	    tmp = tmp.subvalue(part_bit, part_wid);
	    vpip_vec4_get_قيمة(tmp, part_wid, خطأ, vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(صحيح code)
{
      بدل (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  حالة vpiParent:
	    أرجع 0;

            // Not part of the Verilog standard. We use this internally.
	  حالة vpiArray:
	    أرجع array;

	  حالة vpiScope:
	    أرجع array->get_scope();

	  حالة vpiModule:
	    أرجع vpip_module(array->get_scope());
      }

      أرجع 0;
}

فراغ __vpiArray::set_word(unsigned address, unsigned part_off, ثابت vvp_vector4_t&val)
{
      إذا (address >= get_size())
	    أرجع;

      إذا (vals4) {
	    أكد(nets == 0);
	    إذا (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp = vals4->get_word(address);
		  إذا ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			أكد(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals4->set_word(address, tmp);
	    } وإلا {
		  vals4->set_word(address, val);
	    }
	    word_change(address);
	    أرجع;
      }

      إذا (vals) {
	    أكد(nets == 0);
	    إذا (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp;
		  vals->get_word(address, tmp);
		  إذا ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			أكد(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals->set_word(address, tmp);
	    } وإلا {
		  vals->set_word(address, val);
	    }
	    word_change(address);
	    أرجع;
      }

      أكد(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle word = nets[address];
      ثابت بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      أكد(vsig);

      vsig->عقدة->send_vec4_pv(val, part_off, vpip_size(vsig), 0);
      word_change(address);
}

فراغ __vpiArray::set_word(unsigned address, مزدوج val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      إذا (address >= vals->get_size())
	    أرجع;

      vals->set_word(address, val);
      word_change(address);
}

فراغ __vpiArray::set_word(unsigned address, ثابت string&val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      إذا (address >= vals->get_size())
	    أرجع;

      vals->set_word(address, val);
      word_change(address);
}

فراغ __vpiArray::set_word(unsigned address, ثابت vvp_object_t&val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      إذا (address >= vals->get_size())
	    أرجع;

      vals->set_word(address, val);
      word_change(address);
}

vvp_vector4_t __vpiArray::get_word(unsigned address)
{
      إذا (vals4) {
	    أكد(nets == 0);
	    أكد(vals == 0);
	    أرجع vals4->get_word(address);
      }

      إذا (vals) {
	    أكد(nets == 0);
	    أكد(vals4== 0);
	    إذا (address >= vals->get_size())
		  أرجع vvp_vector4_t(vals_width, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_word(address, val);
	    أرجع val;
      }

      أكد(vals4 == 0);
      أكد(vals == 0);
      أكد(nets != 0);

      إذا (address >= get_size()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    أكد(get_size() > 0);
	    vpiHandle word = nets[0];
	    أكد(word);
	    بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
	    أكد(vsig);
	    vvp_signal_قيمة*sig = dynamic_cast<vvp_signal_قيمة*> (vsig->عقدة->fil);
	    أكد(sig);
	    أرجع vvp_vector4_t(sig->قيمة_size(), BIT4_X);
      }

      vpiHandle word = nets[address];
      بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      أكد(vsig);
      vvp_signal_قيمة*sig = dynamic_cast<vvp_signal_قيمة*> (vsig->عقدة->fil);
      أكد(sig);

      vvp_vector4_t val;
      sig->vec4_قيمة(val);
      أرجع val;
}

مزدوج __vpiArray::get_word_r(unsigned address)
{
      إذا (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    إذا (address >= vals->get_size())
		  أرجع 0.0;

	    مزدوج val;
	    vals->get_word(address, val);
	    أرجع val;
      }

      إذا (address >= get_size())
	    أرجع 0.0;

      أكد(nets);
      vpiHandle word = nets[address];
      بنية __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(word);
      أكد(vsig);
      vvp_signal_قيمة*sig = dynamic_cast<vvp_signal_قيمة*> (vsig->net->fil);
      أكد(sig);

      مزدوج val = sig->real_قيمة();
      أرجع val;
}

فراغ __vpiArray::get_word_obj(unsigned address, vvp_object_t&val)
{
      إذا (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    إذا (address >= vals->get_size()) {
		  val = vvp_object_t();
		  أرجع;
	    }

	    vals->get_word(address, val);
	    أرجع;
      }

      أكد(nets);
	// Arrays of string nets not implemented!
      أكد(0);
      أرجع;
}

string __vpiArray::get_word_str(unsigned address)
{
      إذا (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    إذا (address >= vals->get_size())
		  أرجع "";

	    string val;
	    vals->get_word(address, val);
	    أرجع val;
      }

      أكد(nets);
	// Arrays of string nets not implemented!
      أكد(0);
      أرجع "";
}

vpiHandle vpip_make_array(ثابت حرف*label, ثابت حرف*name,
                          صحيح first_addr, صحيح last_addr,
                          منطقي signed_flag)
{
      بنية __vpiArray*obj = جديد __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      إذا (last_addr >= first_addr) {
	    obj->swap_addr = خطأ;
      } وإلا {
	    obj->swap_addr = true;
	    صحيح tmp = last_addr;
	    last_addr = first_addr;
	    first_addr = tmp;
      }
      أكد(last_addr >= first_addr);
      unsigned array_عدد = last_addr+1-first_addr;

	// For now, treat all arrays as memories. This is not quite
	// correct, as arrays are arrays with memories a special case.
      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_عدد = array_عدد;

      obj->first_addr.set_قيمة(first_addr);
      obj->last_addr .set_قيمة(last_addr);

	// Start off now knowing if we are nets or variables.
      obj->nets = 0;
      obj->vals4 = 0;
      obj->vals  = 0;
      obj->vals_width = 0;
      obj->vals_كلمات = 0;

	// Initialize (clear) the read-ports list.
      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

	/* Add this symbol to the array_symbols table for later lookup. */
      إذا (!array_table)
	    array_table = جديد رمز_map_s<بنية __vpiArray>;

      أكد(!array_find(label));
      array_table->sym_set_قيمة(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_رمز(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_to_current_scope(obj);

      أرجع obj;
}

فراغ __vpiArray::alias_word(unsigned long addr, vpiHandle word, صحيح msb_, صحيح lsb_)
{
      أكد(msb.get_قيمة() == msb_);
      أكد(lsb.get_قيمة() == lsb_);
      أكد(addr < get_size());
      أكد(nets);
      nets[addr] = word;
}

فراغ __vpiArray::attach_word(unsigned addr, vpiHandle word)
{
      أكد(addr < get_size());
      أكد(nets);
      nets[addr] = word;

      إذا (بنية __vpiSignal*sig = dynamic_cast<__vpiSignal*>(word)) {
	    vvp_net_t*net = sig->عقدة;
	    أكد(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    أكد(fun);
	    fun->attach_as_word(هذا, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = هذا;
	    sig->id.index = جديد __vpiDecConst(addr + first_addr.get_قيمة());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    أرجع;
      }

      إذا (بنية __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(word)) {
	    vvp_net_t*net = sig->net;
	    أكد(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    أكد(fun);
	    fun->attach_as_word(هذا, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = هذا;
	    sig->id.index = جديد __vpiDecConst(addr + first_addr.get_قيمة());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = true;
	    أرجع;
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
      إذا (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = جديد vvp_vector4array_aa(arr->vals_width,
						 arr->get_size());
      } وإلا {
            arr->vals4 = جديد vvp_vector4array_sa(arr->vals_width,
						 arr->get_size());
      }
      arr->msb.set_قيمة(msb);
      arr->lsb.set_قيمة(lsb);

      عدد_var_arrays += 1;
      عدد_var_array_كلمات += arr->get_size();

      حرر(label);
      delete[] name;
}

فراغ compile_var2_array(حرف*label, حرف*name, صحيح last, صحيح first,
		   صحيح msb, صحيح lsb, منطقي signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, signed_flag);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.set_قيمة(msb);
      arr->lsb.set_قيمة(lsb);
      arr->vals_width = labs(msb-lsb) + 1;

      أكد(! arr->nets);
      إذا (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = جديد vvp_darray_atom<int8_t>(arr->get_size());
      } وإلا إذا (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = جديد vvp_darray_atom<uint8_t>(arr->get_size());
      } وإلا إذا (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = جديد vvp_darray_atom<int16_t>(arr->get_size());
      } وإلا إذا (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = جديد vvp_darray_atom<uint16_t>(arr->get_size());
      } وإلا إذا (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = جديد vvp_darray_atom<int32_t>(arr->get_size());
      } وإلا إذا (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = جديد vvp_darray_atom<uint32_t>(arr->get_size());
      } وإلا إذا (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = جديد vvp_darray_atom<int64_t>(arr->get_size());
      } وإلا إذا (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = جديد vvp_darray_atom<uint64_t>(arr->get_size());
      } وإلا {
	    arr->vals = جديد vvp_darray_vec2(arr->get_size(), arr->vals_width);
      }
      عدد_var_arrays += 1;
      عدد_var_array_كلمات += arr->get_size();

      حرر(label);
      delete[] name;
}

فراغ compile_real_array(حرف*label, حرف*name, صحيح last, صحيح first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, true);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = جديد vvp_darray_real(arr->get_size());
      arr->vals_width = 1;

      عدد_real_arrays += 1;
      عدد_real_array_كلمات += arr->get_size();

      حرر(label);
      delete[] name;
}

فراغ compile_string_array(حرف*label, حرف*name, صحيح last, صحيح first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, true);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = جديد vvp_darray_string(arr->get_size());
      arr->vals_width = 1;

      عدد_real_arrays += 1;
      عدد_real_array_كلمات += arr->get_size();

      حرر(label);
      delete[] name;
}

فراغ compile_object_array(حرف*label, حرف*name, صحيح last, صحيح first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, true);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = جديد vvp_darray_object(arr->get_size());
      arr->vals_width = 1;

      عدد_real_arrays += 1;
      عدد_real_array_كلمات += arr->get_size();

      حرر(label);
      delete[] name;
}

فراغ compile_net_array(حرف*label, حرف*name, صحيح last, صحيح first)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, first, last, خطأ);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(calloc(arr->get_size(), حجم(vpiHandle)));

      عدد_net_arrays += 1;
      عدد_net_array_كلمات += arr->get_size();

      حرر(label);
      delete[] name;
}

صنف vvp_fun_arrayport  : عام vvp_net_fun_t {

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

صنف vvp_fun_arrayport_sa  : عام vvp_fun_arrayport {

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
	    addr_valid_flag = vector4_to_قيمة(bit, addr_);
	    إذا (! addr_valid_flag)
		  addr_ = arr_->get_size();
	    إذا (vpi_array_is_real(arr_))
		  port.ptr()->send_real(arr_->get_word_r(addr_), 0);
	    وإلا
		  port.ptr()->send_vec4(arr_->get_word(addr_), 0);

	    اكسر;

	  افتراضي:
	    fprintf(stdout, "XXXX write ports not implemented.\n");
	    أكد(0);
      }
}

فراغ vvp_fun_arrayport_sa::check_word_change(unsigned long addr)
{
      إذا (addr != addr_) أرجع;

      إذا (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr_), 0);
      } وإلا {
	    net_->send_vec4(arr_->get_word(addr_), 0);
      }
}

صنف vvp_fun_arrayport_aa  : عام vvp_fun_arrayport, عام automatic_hooks_s {

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
      إذا (context) {
            unsigned long*addr = static_cast<unsigned long*>
                  (vvp_get_context_item(context, context_idx_));

            منطقي addr_valid_flag;

            بدل (port.port()) {

                حالة 0: // Address input
                  addr_valid_flag = vector4_to_قيمة(bit, *addr);
                  إذا (! addr_valid_flag) *addr = arr_->get_size();
                  إذا (vpi_array_is_real(arr_)) {
			port.ptr()->send_real(arr_->get_word_r(*addr),
					      context);
                  } وإلا {
			port.ptr()->send_vec4(arr_->get_word(*addr),
					      context);
                  }
                  اكسر;

                افتراضي:
                  fprintf(stdout, "XXXX write ports not implemented.\n");
                  أكد(0);
            }
      } وإلا {
            context = context_scope_->live_contexts;
            طالما (context) {
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

      إذا (addr != *port_addr)
	    أرجع;

      إذا (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr), context);
      } وإلا {
	    net_->send_vec4(arr_->get_word(addr), context);
      }
}

فراغ vvp_fun_arrayport_aa::check_word_change(unsigned long addr)
{
      إذا (arr_->get_scope()->is_automatic()) {
            أكد(vthread_get_wt_context());
            check_word_change_(addr, vthread_get_wt_context());
      } وإلا {
            vvp_context_t context = context_scope_->live_contexts;
            طالما (context) {
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
      إذا (!array->get_scope()->is_automatic() &&
          (array->vals4 || array->vals)) {
              /* propagate initial values for variable arrays */
            إذا (!vpi_array_is_real(array)) {
                  vvp_bit4_t init;
                  إذا (array->vals4)
                      init = BIT4_X;
                  وإلا
                      init = BIT4_0;
                  vvp_vector4_t tmp(array->vals_width, init);
                  schedule_init_propagate(fun->net_, tmp);
            } وإلا {
                  schedule_init_propagate(fun->net_, 0.0);
            }
      }
}

صنف array_word_قيمة_callback : عام قيمة_callback {
    عام:
      مضمن explicit array_word_قيمة_callback(p_cb_data data, long addr)
      : قيمة_callback(data), word_addr(addr)
      { }

    عام:
      long word_addr;
};

فراغ __vpiArray::word_change(unsigned long addr)
{
      لكل (vvp_fun_arrayport*cur = ports_; cur; cur = cur->next_)
	    cur->check_word_change(addr);

	// Run callbacks attached to the array itself.
      بنية __vpiCallback *next = vpi_callbacks;
      بنية __vpiCallback *prev = 0;

      طالما (next) {
	    array_word_قيمة_callback*cur = dynamic_cast<array_word_قيمة_callback*>(next);
	    next = cur->next;

	      // Skip callbacks that are not for me. -1 is for every element.
	    إذا (cur->word_addr != (long)addr && cur->word_addr != -1) {
		  prev = cur;
		  استمر;
	    }

	      // For whole array callbacks we need to set the index.
	    إذا (cur->word_addr == -1) {
		  cur->cb_data.index = (PLI_INT32) ((صحيح)addr +
						    first_addr.get_قيمة());
	    }

	    إذا (cur->cb_data.cb_rtn != 0) {
		  إذا (cur->test_قيمة_callback_ready()) {
			إذا (cur->cb_data.value) {
			      إذا (vpi_array_is_real(هذا)) {
				    مزدوج val = 0.0;
				    إذا (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_real_get_قيمة(val, cur->cb_data.value);
			      } وإلا إذا (vals4) {
				    vpip_vec4_get_قيمة(vals4->get_word(addr),
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } وإلا إذا (dynamic_cast<vvp_darray_atom<int8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int64_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint64_t>*>(vals)
				      || dynamic_cast<vvp_darray_vec2*>(vals)) {
				    vvp_vector4_t val;
				    إذا (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_vec4_get_قيمة(val,
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } وإلا {
			            أكد(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } وإلا إذا (prev == 0) {

		  vpi_callbacks = next;
		  cur->next = 0;
		  delete cur;

	    } وإلا {
		  أكد(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}

صنف array_resolv_list_t : عام resolv_list_s {

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
      إذا (*array == 0) {
	    أكد(!mes);
	    أرجع خطأ;
      }
      أرجع true;
}

صنف array_port_resolv_list_t : عام resolv_list_s {

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
      إذا (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      وإلا
            context_scope = 0;
      ptr = جديد vvp_net_t;
}

منطقي array_port_resolv_list_t::resolve(منطقي mes)
{
      vvp_array_t mem = array_find(label());
      إذا (mem == 0) {
	    أكد(mem || !mes);
	    أرجع خطأ;
      }

      vvp_fun_arrayport*fun;
      إذا (use_addr)
            إذا (context_scope)
                  fun = جديد vvp_fun_arrayport_aa(context_scope, mem, ptr, addr);
            وإلا
                  fun = جديد vvp_fun_arrayport_sa(mem, ptr, addr);
      وإلا
            إذا (context_scope)
                  fun = جديد vvp_fun_arrayport_aa(context_scope, mem, ptr);
            وإلا
                  fun = جديد vvp_fun_arrayport_sa(mem, ptr);
      ptr->fun = fun;

      array_attach_port(mem, fun);

      أرجع true;
}

صنف array_word_part_callback : عام array_word_قيمة_callback {
    عام:
      explicit array_word_part_callback(p_cb_data data, long addr);
      ~array_word_part_callback() override;

      array_word_part_callback(ثابت array_word_part_callback&) = delete;
      array_word_part_callback& operator=(ثابت array_word_part_callback&) = delete;

      منطقي test_قيمة_callback_ready(فراغ) override;

    خاص:
      حرف*قيمة_bits;
};

array_word_part_callback::array_word_part_callback(p_cb_data data, long addr)
: array_word_قيمة_callback(data, addr)
{
	// Get the initial value of the part, to use as a reference.
      بنية __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj);
      s_vpi_قيمة tmp_قيمة;
      tmp_قيمة.format = vpiBinStrVal;
      apvword->vpi_get_قيمة(&tmp_قيمة);

      قيمة_bits = جديد حرف[apvword->part_wid+1];

      memcpy(قيمة_bits, tmp_قيمة.value.str, apvword->part_wid);
      قيمة_bits[apvword->part_wid] = 0;
}

array_word_part_callback::~array_word_part_callback()
{
      delete[]قيمة_bits;
}

منطقي array_word_part_callback::test_قيمة_callback_ready(فراغ)
{
      بنية __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_data.obj);
      أكد(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_قيمة tmp_قيمة;
      tmp_قيمة.format = vpiBinStrVal;
      apvword->vpi_get_قيمة(&tmp_قيمة);

      إذا (memcmp(قيمة_bits, tmp_قيمة.value.str, apvword->part_wid) == 0)
	    أرجع خطأ;

      memcpy(قيمة_bits, tmp_قيمة.value.str, apvword->part_wid);
      أرجع true;

}

قيمة_callback*vpip_array_word_change(p_cb_data data)
{
      بنية __vpiArray*parent = 0;
      array_word_قيمة_callback*cbh = 0;
      إذا (ثابت بنية __vpiArrayWord*word = array_var_word_from_handle(data->obj)) {
	    parent = static_cast<__vpiArray*>(word->get_parent());
	    unsigned addr = word->get_index();
	    cbh = جديد array_word_قيمة_callback(data, addr);

      } وإلا إذا (بنية __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(data->obj)) {
	    parent = tword->array;
	    cbh = جديد array_word_قيمة_callback(data, tword->address);

      } وإلا إذا (بنية __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj)) {
	    parent = apvword->array;
	    cbh = جديد array_word_part_callback(data, apvword->word_sel);
      }

      أكد(cbh);
      أكد(parent);
      cbh->next = parent->vpi_callbacks;
      parent->vpi_callbacks = cbh;

      أرجع cbh;
}

قيمة_callback* vpip_array_change(p_cb_data data)
{
      array_word_قيمة_callback*cbh = جديد array_word_قيمة_callback(data, -1);
      أكد(data->obj);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(data->obj);
      cbh->next = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      أرجع cbh;
}

فراغ compile_array_port(حرف*label, حرف*array, حرف*addr)
{
      array_port_resolv_list_t*resolv_mem
	    = جديد array_port_resolv_list_t(array, خطأ, 0);

      define_functor_رمز(label, resolv_mem->ptr);
      حرر(label);
	// Connect the port-0 input as the address.
      مدخل_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

فراغ compile_array_port(حرف*label, حرف*array, long addr)
{
      array_port_resolv_list_t*resolv_mem
	    = جديد array_port_resolv_list_t(array, true, addr);

      define_functor_رمز(label, resolv_mem->ptr);
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
      obj->array_عدد = mem->array_عدد;
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
      obj->vals_كلمات = mem->vals_كلمات;

      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

      أكد(array_table);
      أكد(!array_find(label));
      array_table->sym_set_قيمة(label, obj);

      compile_vpi_رمز(label, obj);
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

      أرجع obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(حرف*label, حرف*رمز)
{
      بنية __vpiArrayVthrA*obj = جديد __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = جديد array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      compile_vpi_lookup(&obj->address_handle, رمز);
      obj->address = 0;

      أرجع obj;
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

      أرجع obj;
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

      أرجع obj;
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
      إذا (arr->vals_كلمات) delete [] (arr->vals_كلمات-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals;

      إذا (arr->nets) {
	    لكل (unsigned idx = 0; idx < arr->get_size(); idx += 1) {
		  إذا (بنية __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.index);
		    /* These should only be the real words. */
		  } وإلا {
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

      طالما (arr->vpi_callbacks) {
	    بنية __vpiCallback*tmp = arr->vpi_callbacks->next;
	    delete arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      delete arr;
}

فراغ A_delete(vpiHandle item)
{
      بنية __vpiArrayVthrA*obj = static_cast<بنية __vpiArrayVthrA*>(item);
      إذا (obj->address_handle) {
	    بدل (obj->address_handle->get_type_code()) {
		حالة vpiMemoryWord:
		  إذا (vpi_get(_vpiFromThr, obj->address_handle) == _vpi_at_A) {
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
