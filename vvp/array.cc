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
#अन्तर्भाव  "vvp_cleanup.h"
#endif
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <iostream>
# include  "compile.h"
# include  <cassert>
# include  "ivl_alloc.h"

using नामाकाश std;

अचिन्हित दीर्घ मोजणी_net_arrays = 0;
अचिन्हित दीर्घ मोजणी_net_array_शब्द = 0;
अचिन्हित दीर्घ मोजणी_var_arrays = 0;
अचिन्हित दीर्घ मोजणी_var_array_शब्द = 0;
अचिन्हित दीर्घ मोजणी_real_arrays = 0;
अचिन्हित दीर्घ मोजणी_real_array_शब्द = 0;

स्थिर चिन्ह_map_s<संरचना __vpiArray>* array_table =0;

वर्ग vvp_fun_arrayport;
स्थिर शून्य array_attach_port(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_find(स्थिरांक अक्षर*label)
{
      जर (array_table == 0)
	    परत 0;

      vvp_array_t v = array_table->sym_get_मूल्य(label);
      परत v;
}

संरचना __vpiArrayVthrA : सार्वजनिक __vpiHandle {
      पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiMemoryWord; }
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर* vpi_get_str(पूर्णांक code) override;
      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;
      vpiHandle vpi_handle(पूर्णांक code) override;

      संरचना __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle address_handle;
	// If wid==0, then address is the address into the array.
      अचिन्हित address;

      अचिन्हित get_address() स्थिरांक
      {
	    जर (address_handle) {
		  s_vpi_मूल्य vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpiVectorVal;
		  address_handle->vpi_get_मूल्य(&vp);
		  पूर्णांक शब्द = (address_handle->vpi_get(vpiSize)-1)/32 + 1;
		  साठी(पूर्णांक idx = 0; idx < शब्द; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			जर (vp.value.vector[idx].bval != 0) परत अच_पूर्णांक_उच्च;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  address_handle->vpi_get_मूल्य(&vp);
		  परत vp.value.integer;
	    }

	    परत address;
      }
};


संरचना __vpiArrayVthrAPV : सार्वजनिक __vpiHandle {
      पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiPartSelect; }
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर* vpi_get_str(पूर्णांक code) override;
      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;
      vpiHandle vpi_handle(पूर्णांक code) override;

      संरचना __vpiArray*array;
      अचिन्हित word_sel;
      अचिन्हित part_bit;
      अचिन्हित part_wid;
};

तार्किक is_net_array(vpiHandle obj)
{
      संरचना __vpiArray*rfp = dynamic_cast<__vpiArray*> (obj);
      प्रतिज्ञा(rfp);

      जर (rfp->nets != 0) परत सत्य;
      परत असत्य;
}

// This function return true if the underlying array words are real.
स्थिर तार्किक vpi_array_is_real(स्थिरांक vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      जर (arr->vals4 != 0)  // A bit based variable/register array.
	    परत असत्य;

      जर (dynamic_cast<vvp_darray_real*> (arr->vals))
	    परत सत्य;

      जर (arr->vals != 0)
	    परत असत्य;

	// This must be a net array so look at element 0 to find the type.
      प्रतिज्ञा(arr->nets != 0);
      प्रतिज्ञा(arr->get_size() > 0);
      स्थिरांक संरचना __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      जर (rsig) {
	    परत सत्य;
      }

      परत असत्य;
}

स्थिर तार्किक vpi_array_is_string(स्थिरांक vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      जर (arr->vals4 != 0)  // A bit based variable/register array.
	    परत असत्य;

      जर (dynamic_cast<vvp_darray_string*> (arr->vals))
	    परत सत्य;

      परत असत्य;
}

पूर्णांक __vpiArray::get_word_size() स्थिरांक
{
      अचिन्हित width;

      प्रतिज्ञा(get_size() > 0);
	/* For a net array we need to get the width from the first element. */
      जर (nets) {
	    प्रतिज्ञा(vals4 == 0 && vals == 0);
	    स्थिरांक संरचना __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    प्रतिज्ञा(vsig);
	    width = vpip_size(vsig);
	/* For a variable array we can get the width from vals_width. */
      } अन्यथा {
	    प्रतिज्ञा(vals4 || vals);
	    width = vals_width;
      }

      परत width;
}

अक्षर*__vpiArray::get_word_str(संरचना __vpiArrayWord*word, पूर्णांक code)
{
      अचिन्हित index = word->get_index();

      जर (code == vpiFile) {  // Not implemented for now!
	    परत simple_set_rbuf_str(file_names[0]);
      }

      अक्षर sidx [64];
      snprintf(sidx, 63, "%d", (पूर्णांक)index + first_addr.get_मूल्य());
      परत generic_get_str(code, get_scope(), name, sidx);
}

शून्य __vpiArray::get_word_मूल्य(संरचना __vpiArrayWord*word, p_vpi_मूल्य vp)
{
      अचिन्हित index = word->get_index();

    // Determine the appropriate format (The Verilog PLI Handbook 5.2.10)
      जर(vp->format == vpiObjTypeVal) {
          जर(vpi_array_is_real(हे))
              vp->format = vpiRealVal;
          अन्यथा जर(vpi_array_is_string(हे))
              vp->format = vpiStringVal;
          अन्यथा
              vp->format = vpiIntVal;
      }

      जर(vals4) {
          vpip_vec4_get_मूल्य(vals4->get_word(index),
                              vals4->width(), signed_flag, vp);
      } अन्यथा जर(vals) {
          निवड(vp->format) {
            स्थिती vpiBinStrVal:
            स्थिती vpiOctStrVal:
            स्थिती vpiDecStrVal:
            स्थिती vpiHexStrVal:
            स्थिती vpiScalarVal:
            स्थिती vpiIntVal:
            स्थिती vpiVectorVal:
            {
                vvp_vector4_t v;
                vals->get_word(index, v);
                vpip_vec4_get_मूल्य(v, vals_width, signed_flag, vp);
            }
            थांबा;

            स्थिती vpiRealVal:
            {
                द्विगुण d;
                vals->get_word(index, d);
                vpip_real_get_मूल्य(d, vp);
            }
            थांबा;

            स्थिती vpiStringVal:
            {
                string s;
                vals->get_word(index, s);
                vpip_string_get_मूल्य(s, vp);
            }
            थांबा;

            पूर्वनिर्धारित:
                ख_लिखो(मानक_त्रुटि, "vpi sorry: format is not implemented\n");
                प्रतिज्ञा(असत्य);
          }
      }
}

शून्य __vpiArray::put_word_मूल्य(संरचना __vpiArrayWord*word, p_vpi_मूल्य vp, पूर्णांक)
{
      अचिन्हित index = word->get_index();
      vvp_vector4_t val = vec4_from_vpi_मूल्य(vp, vals_width);
      set_word(index, 0, val);
}

vpiHandle __vpiArray::get_iter_index(संरचना __vpiArrayIterator*, पूर्णांक idx)
{
      जर (nets) परत nets[idx];

      प्रतिज्ञा(vals4 || vals);

      जर (vals_शब्द == 0) make_vals_शब्द();

      परत &(vals_शब्द[idx].as_word);
}

पूर्णांक __vpiArray::vpi_get(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpi_ओळ_No:
	    परत 0; // Not implemented for now!

	  स्थिती vpiSize:
	    परत get_size();

	  स्थिती vpiAutomatic:
	    परत scope->is_automatic()? 1 : 0;

	  पूर्वनिर्धारित:
	    परत 0;
      }
}

अक्षर* __vpiArray::vpi_get_str(पूर्णांक code)
{
      जर (code == vpiFile) {  // Not implemented for now!
            परत simple_set_rbuf_str(file_names[0]);
      }

      परत generic_get_str(code, scope, name, रिक्त);
}

vpiHandle __vpiArray::vpi_handle(पूर्णांक code)
{
      निवड (code) {
          स्थिती vpi_डावा_Range:
            जर (swap_addr) परत &last_addr;
            अन्यथा परत &first_addr;

          स्थिती vpi_उजवा_Range:
            जर (swap_addr) परत &first_addr;
            अन्यथा परत &last_addr;

	  स्थिती vpiScope:
	    परत scope;

	  स्थिती vpiModule:
	    परत vpip_module(scope);
      }

      परत 0;
}

/*
* VPI code passes indices that are not yet converted to canonical
* form, so this index function does it here.
*/
vpiHandle __vpiArray::vpi_index(पूर्णांक index)
{
      index -= first_addr.get_मूल्य();
      जर (index >= (दीर्घ) get_size())
	    परत 0;
      जर (index < 0)
	    परत 0;

      जर (nets != 0) {
	    परत nets[index];
      }

      जर (vals_शब्द == 0)
	    make_vals_शब्द();

      परत &(vals_शब्द[index].as_word);
}

पूर्णांक __vpiArrayWord::as_word_t::vpi_get(पूर्णांक code)
{
      संरचना __vpiArrayWord*obj = array_var_word_from_handle(हे);
      प्रतिज्ञा(obj);
      संरचना __vpiArrayBase*my_parent = obj->get_parent();
      t_vpi_मूल्य val;

      निवड (code) {
	  स्थिती vpi_ओळ_No:
	    परत 0; // Not implemented for now!

	  स्थिती vpiSize:
	    परत my_parent->get_word_size();

	  स्थिती vpi_डावा_Range:
            val.format = vpiIntVal;
	    my_parent->get_डावा_range()->vpi_get_मूल्य(&val);
            प्रतिज्ञा(val.format == vpiIntVal);
            परत val.value.integer;

	  स्थिती vpi_उजवा_Range:
            val.format = vpiIntVal;
	    my_parent->get_उजवा_range()->vpi_get_मूल्य(&val);
            प्रतिज्ञा(val.format == vpiIntVal);
            परत val.value.integer;

	  स्थिती vpiIndex:
	    {
		  पूर्णांक base_offset = 0;
		  संरचना __vpiArray*base = dynamic_cast<__vpiArray*> (my_parent);
		  जर (base) {
			val.format = vpiIntVal;
			base->first_addr.vpi_get_मूल्य(&val);
			base_offset += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_index.vpi_get_मूल्य(&val);
		  प्रतिज्ञा(val.format == vpiIntVal);
		  परत val.value.integer + base_offset;
	    }

	  स्थिती vpiAutomatic:
	    परत my_parent->get_scope()->is_automatic()? 1 : 0;

#जर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  स्थिती _vpiFromThr:
	    परत _vpiNoThr;
#endif

	  पूर्वनिर्धारित:
	    परत 0;
      }
}


पूर्णांक __vpiArrayVthrA::vpi_get(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpi_ओळ_No:
	    परत 0; // Not implemented for now!

	  स्थिती vpiSize:
	    परत array->get_word_size();

	  स्थिती vpi_डावा_Range:
	    परत array->msb.get_मूल्य();

	  स्थिती vpi_उजवा_Range:
	    परत array->lsb.get_मूल्य();

	  स्थिती vpiIndex:
	    परत (पूर्णांक)get_address() + array->first_addr.get_मूल्य();

	  स्थिती vpiAutomatic:
	    परत array->get_scope()->is_automatic() ? 1 : 0;

#जर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  स्थिती _vpiFromThr:
	    परत _vpi_at_A;
#endif

	  // If address_handle is not zero we definitely have a
	  // variable.
	  स्थिती vpiConstantSelect:
	    परत address_handle == 0;

	  पूर्वनिर्धारित:
	    परत 0;
      }
}

अक्षर* __vpiArrayVthrA::vpi_get_str(पूर्णांक code)
{
      जर (code == vpiFile) {  // Not implemented for now!
            परत simple_set_rbuf_str(file_names[0]);
      }

      अक्षर sidx [64];
      snprintf(sidx, 63, "%d", (पूर्णांक)get_address() + array->first_addr.get_मूल्य());
      परत generic_get_str(code, array->get_scope(), array->name, sidx);
}

शून्य __vpiArrayVthrA::vpi_get_मूल्य(p_vpi_मूल्य vp)
{
      प्रतिज्ञा(array);

      अचिन्हित index = get_address();
      जर (vpi_array_is_real(array)) {
	    द्विगुण tmp = array->get_word_r(index);
	    vpip_real_get_मूल्य(tmp, vp);
      } अन्यथा जर (vpi_array_is_string(array)) {
	    string tmp = array->get_word_str(index);
	    vpip_string_get_मूल्य(tmp, vp);
      } अन्यथा {
	    vvp_vector4_t tmp = array->get_word(index);
	    अचिन्हित width = array->get_word_size();
	    vpip_vec4_get_मूल्य(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_मूल्य(p_vpi_मूल्य vp, पूर्णांक)
{
      अचिन्हित index = get_address();

      प्रतिज्ञा(array);
      प्रतिज्ञा(index < array->get_size());

      जर (vpi_array_is_real(array)) {
	    द्विगुण val = real_from_vpi_मूल्य(vp);
	    array->set_word(index, val);
      } अन्यथा {
	    अचिन्हित width = array->get_word_size();
	    vvp_vector4_t val = vec4_from_vpi_मूल्य(vp, width);
	    array->set_word(index, 0, val);
      }

      परत हे;
}

vpiHandle __vpiArrayVthrA::vpi_handle(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpiIndex:
	    थांबा;  // Not implemented!

	  स्थिती vpi_डावा_Range:
	    परत &array->msb;

	  स्थिती vpi_उजवा_Range:
	    परत &array->lsb;

	  स्थिती vpiParent:
	  स्थिती vpiArray:
	    परत array;

	  स्थिती vpiScope:
	    परत array->get_scope();

	  स्थिती vpiModule:
	    परत vpip_module(array->get_scope());
      }

      परत 0;
}


पूर्णांक __vpiArrayVthrAPV::vpi_get(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpi_ओळ_No:
	    परत 0; // Not implemented for now!

	  स्थिती vpiSize:
	    परत part_wid;

	  स्थिती vpi_डावा_Range:
	    परत part_bit + part_wid - 1;

	  स्थिती vpi_उजवा_Range:
	    परत part_bit;

	  स्थिती vpiAutomatic:
	    परत array->get_scope()->is_automatic() ? 1 : 0;

#जर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  स्थिती _vpiFromThr:
	    परत _vpi_at_APV;
#endif

	  स्थिती vpiConstantSelect:
	    परत 1;

	  पूर्वनिर्धारित:
	    परत 0;
      }
}

अक्षर* __vpiArrayVthrAPV::vpi_get_str(पूर्णांक code)
{
      जर (code == vpiFile) {  // Not implemented for now!
            परत simple_set_rbuf_str(file_names[0]);
      }

      अक्षर sidx [64];
      snprintf(sidx, 63, "%u", word_sel + array->first_addr.get_मूल्य());
      परत generic_get_str(code, array->get_scope(), array->name, sidx);
}

शून्य __vpiArrayVthrAPV::vpi_get_मूल्य(p_vpi_मूल्य vp)
{
      प्रतिज्ञा(array);

      अचिन्हित index = word_sel;
      जर (vpi_array_is_real(array)) {
	    द्विगुण tmp = array->get_word_r(index);
	    vpip_real_get_मूल्य(tmp, vp);
      } अन्यथा {
	    vvp_vector4_t tmp = array->get_word(index);
	    tmp = tmp.subvalue(part_bit, part_wid);
	    vpip_vec4_get_मूल्य(tmp, part_wid, असत्य, vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(पूर्णांक code)
{
      निवड (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  स्थिती vpiParent:
	    परत 0;

            // Not part of the Verilog standard. We use this internally.
	  स्थिती vpiArray:
	    परत array;

	  स्थिती vpiScope:
	    परत array->get_scope();

	  स्थिती vpiModule:
	    परत vpip_module(array->get_scope());
      }

      परत 0;
}

शून्य __vpiArray::set_word(अचिन्हित address, अचिन्हित part_off, स्थिरांक vvp_vector4_t&val)
{
      जर (address >= get_size())
	    परत;

      जर (vals4) {
	    प्रतिज्ञा(nets == 0);
	    जर (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp = vals4->get_word(address);
		  जर ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			प्रतिज्ञा(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals4->set_word(address, tmp);
	    } अन्यथा {
		  vals4->set_word(address, val);
	    }
	    word_change(address);
	    परत;
      }

      जर (vals) {
	    प्रतिज्ञा(nets == 0);
	    जर (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp;
		  vals->get_word(address, tmp);
		  जर ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			प्रतिज्ञा(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals->set_word(address, tmp);
	    } अन्यथा {
		  vals->set_word(address, val);
	    }
	    word_change(address);
	    परत;
      }

      प्रतिज्ञा(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle word = nets[address];
      स्थिरांक संरचना __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      प्रतिज्ञा(vsig);

      vsig->गाठ->send_vec4_pv(val, part_off, vpip_size(vsig), 0);
      word_change(address);
}

शून्य __vpiArray::set_word(अचिन्हित address, द्विगुण val)
{
      प्रतिज्ञा(vals != 0);
      प्रतिज्ञा(nets == 0);

      जर (address >= vals->get_size())
	    परत;

      vals->set_word(address, val);
      word_change(address);
}

शून्य __vpiArray::set_word(अचिन्हित address, स्थिरांक string&val)
{
      प्रतिज्ञा(vals != 0);
      प्रतिज्ञा(nets == 0);

      जर (address >= vals->get_size())
	    परत;

      vals->set_word(address, val);
      word_change(address);
}

शून्य __vpiArray::set_word(अचिन्हित address, स्थिरांक vvp_object_t&val)
{
      प्रतिज्ञा(vals != 0);
      प्रतिज्ञा(nets == 0);

      जर (address >= vals->get_size())
	    परत;

      vals->set_word(address, val);
      word_change(address);
}

vvp_vector4_t __vpiArray::get_word(अचिन्हित address)
{
      जर (vals4) {
	    प्रतिज्ञा(nets == 0);
	    प्रतिज्ञा(vals == 0);
	    परत vals4->get_word(address);
      }

      जर (vals) {
	    प्रतिज्ञा(nets == 0);
	    प्रतिज्ञा(vals4== 0);
	    जर (address >= vals->get_size())
		  परत vvp_vector4_t(vals_width, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_word(address, val);
	    परत val;
      }

      प्रतिज्ञा(vals4 == 0);
      प्रतिज्ञा(vals == 0);
      प्रतिज्ञा(nets != 0);

      जर (address >= get_size()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    प्रतिज्ञा(get_size() > 0);
	    vpiHandle word = nets[0];
	    प्रतिज्ञा(word);
	    संरचना __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
	    प्रतिज्ञा(vsig);
	    vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*> (vsig->गाठ->fil);
	    प्रतिज्ञा(sig);
	    परत vvp_vector4_t(sig->मूल्य_size(), BIT4_X);
      }

      vpiHandle word = nets[address];
      संरचना __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      प्रतिज्ञा(vsig);
      vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*> (vsig->गाठ->fil);
      प्रतिज्ञा(sig);

      vvp_vector4_t val;
      sig->vec4_मूल्य(val);
      परत val;
}

द्विगुण __vpiArray::get_word_r(अचिन्हित address)
{
      जर (vals) {
	    प्रतिज्ञा(vals4 == 0);
	    प्रतिज्ञा(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    जर (address >= vals->get_size())
		  परत 0.0;

	    द्विगुण val;
	    vals->get_word(address, val);
	    परत val;
      }

      जर (address >= get_size())
	    परत 0.0;

      प्रतिज्ञा(nets);
      vpiHandle word = nets[address];
      संरचना __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(word);
      प्रतिज्ञा(vsig);
      vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*> (vsig->net->fil);
      प्रतिज्ञा(sig);

      द्विगुण val = sig->real_मूल्य();
      परत val;
}

शून्य __vpiArray::get_word_obj(अचिन्हित address, vvp_object_t&val)
{
      जर (vals) {
	    प्रतिज्ञा(vals4 == 0);
	    प्रतिज्ञा(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    जर (address >= vals->get_size()) {
		  val = vvp_object_t();
		  परत;
	    }

	    vals->get_word(address, val);
	    परत;
      }

      प्रतिज्ञा(nets);
	// Arrays of string nets not implemented!
      प्रतिज्ञा(0);
      परत;
}

string __vpiArray::get_word_str(अचिन्हित address)
{
      जर (vals) {
	    प्रतिज्ञा(vals4 == 0);
	    प्रतिज्ञा(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    जर (address >= vals->get_size())
		  परत "";

	    string val;
	    vals->get_word(address, val);
	    परत val;
      }

      प्रतिज्ञा(nets);
	// Arrays of string nets not implemented!
      प्रतिज्ञा(0);
      परत "";
}

vpiHandle vpip_make_array(स्थिरांक अक्षर*label, स्थिरांक अक्षर*name,
                          पूर्णांक first_addr, पूर्णांक last_addr,
                          तार्किक signed_flag)
{
      संरचना __vpiArray*obj = नवीन __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      जर (last_addr >= first_addr) {
	    obj->swap_addr = असत्य;
      } अन्यथा {
	    obj->swap_addr = सत्य;
	    पूर्णांक tmp = last_addr;
	    last_addr = first_addr;
	    first_addr = tmp;
      }
      प्रतिज्ञा(last_addr >= first_addr);
      अचिन्हित array_मोजणी = last_addr+1-first_addr;

	// For now, treat all arrays as memories. This is not quite
	// correct, as arrays are arrays with memories a special case.
      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_मोजणी = array_मोजणी;

      obj->first_addr.set_मूल्य(first_addr);
      obj->last_addr .set_मूल्य(last_addr);

	// Start off now knowing if we are nets or variables.
      obj->nets = 0;
      obj->vals4 = 0;
      obj->vals  = 0;
      obj->vals_width = 0;
      obj->vals_शब्द = 0;

	// Initialize (clear) the read-ports list.
      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

	/* Add this symbol to the array_symbols table for later lookup. */
      जर (!array_table)
	    array_table = नवीन चिन्ह_map_s<संरचना __vpiArray>;

      प्रतिज्ञा(!array_find(label));
      array_table->sym_set_मूल्य(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_चिन्ह(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_to_current_scope(obj);

      परत obj;
}

शून्य __vpiArray::alias_word(अचिन्हित दीर्घ addr, vpiHandle word, पूर्णांक msb_, पूर्णांक lsb_)
{
      प्रतिज्ञा(msb.get_मूल्य() == msb_);
      प्रतिज्ञा(lsb.get_मूल्य() == lsb_);
      प्रतिज्ञा(addr < get_size());
      प्रतिज्ञा(nets);
      nets[addr] = word;
}

शून्य __vpiArray::attach_word(अचिन्हित addr, vpiHandle word)
{
      प्रतिज्ञा(addr < get_size());
      प्रतिज्ञा(nets);
      nets[addr] = word;

      जर (संरचना __vpiSignal*sig = dynamic_cast<__vpiSignal*>(word)) {
	    vvp_net_t*net = sig->गाठ;
	    प्रतिज्ञा(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    प्रतिज्ञा(fun);
	    fun->attach_as_word(हे, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = हे;
	    sig->id.index = नवीन __vpiDecConst(addr + first_addr.get_मूल्य());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    परत;
      }

      जर (संरचना __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(word)) {
	    vvp_net_t*net = sig->net;
	    प्रतिज्ञा(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    प्रतिज्ञा(fun);
	    fun->attach_as_word(हे, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = हे;
	    sig->id.index = नवीन __vpiDecConst(addr + first_addr.get_मूल्य());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = सत्य;
	    परत;
      }
}

शून्य compile_var_array(अक्षर*label, अक्षर*name, पूर्णांक last, पूर्णांक first,
		   पूर्णांक msb, पूर्णांक lsb, अक्षर signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last,
                                      signed_flag != 0);

      संरचना __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_width = द_असल(msb-lsb) + 1;
      जर (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = नवीन vvp_vector4array_aa(arr->vals_width,
						 arr->get_size());
      } अन्यथा {
            arr->vals4 = नवीन vvp_vector4array_sa(arr->vals_width,
						 arr->get_size());
      }
      arr->msb.set_मूल्य(msb);
      arr->lsb.set_मूल्य(lsb);

      मोजणी_var_arrays += 1;
      मोजणी_var_array_शब्द += arr->get_size();

      मुक्त(label);
      delete[] name;
}

शून्य compile_var2_array(अक्षर*label, अक्षर*name, पूर्णांक last, पूर्णांक first,
		   पूर्णांक msb, पूर्णांक lsb, तार्किक signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, signed_flag);

      संरचना __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.set_मूल्य(msb);
      arr->lsb.set_मूल्य(lsb);
      arr->vals_width = द_असल(msb-lsb) + 1;

      प्रतिज्ञा(! arr->nets);
      जर (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = नवीन vvp_darray_atom<int8_t>(arr->get_size());
      } अन्यथा जर (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = नवीन vvp_darray_atom<uint8_t>(arr->get_size());
      } अन्यथा जर (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = नवीन vvp_darray_atom<int16_t>(arr->get_size());
      } अन्यथा जर (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = नवीन vvp_darray_atom<uint16_t>(arr->get_size());
      } अन्यथा जर (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = नवीन vvp_darray_atom<int32_t>(arr->get_size());
      } अन्यथा जर (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = नवीन vvp_darray_atom<uint32_t>(arr->get_size());
      } अन्यथा जर (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = नवीन vvp_darray_atom<int64_t>(arr->get_size());
      } अन्यथा जर (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = नवीन vvp_darray_atom<uint64_t>(arr->get_size());
      } अन्यथा {
	    arr->vals = नवीन vvp_darray_vec2(arr->get_size(), arr->vals_width);
      }
      मोजणी_var_arrays += 1;
      मोजणी_var_array_शब्द += arr->get_size();

      मुक्त(label);
      delete[] name;
}

शून्य compile_real_array(अक्षर*label, अक्षर*name, पूर्णांक last, पूर्णांक first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, सत्य);

      संरचना __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = नवीन vvp_darray_real(arr->get_size());
      arr->vals_width = 1;

      मोजणी_real_arrays += 1;
      मोजणी_real_array_शब्द += arr->get_size();

      मुक्त(label);
      delete[] name;
}

शून्य compile_string_array(अक्षर*label, अक्षर*name, पूर्णांक last, पूर्णांक first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, सत्य);

      संरचना __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = नवीन vvp_darray_string(arr->get_size());
      arr->vals_width = 1;

      मोजणी_real_arrays += 1;
      मोजणी_real_array_शब्द += arr->get_size();

      मुक्त(label);
      delete[] name;
}

शून्य compile_object_array(अक्षर*label, अक्षर*name, पूर्णांक last, पूर्णांक first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, सत्य);

      संरचना __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = नवीन vvp_darray_object(arr->get_size());
      arr->vals_width = 1;

      मोजणी_real_arrays += 1;
      मोजणी_real_array_शब्द += arr->get_size();

      मुक्त(label);
      delete[] name;
}

शून्य compile_net_array(अक्षर*label, अक्षर*name, पूर्णांक last, पूर्णांक first)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, first, last, असत्य);

      संरचना __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(सुस्मृति(arr->get_size(), आकार(vpiHandle)));

      मोजणी_net_arrays += 1;
      मोजणी_net_array_शब्द += arr->get_size();

      मुक्त(label);
      delete[] name;
}

वर्ग vvp_fun_arrayport  : सार्वजनिक vvp_net_fun_t {

    सार्वजनिक:
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, दीर्घ addr);
      ~vvp_fun_arrayport() override;

      आभासी शून्य check_word_change(अचिन्हित दीर्घ addr) = 0;

    संरक्षित:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      अचिन्हित दीर्घ addr_;

      मित्र शून्य array_attach_port(vvp_array_t, vvp_fun_arrayport*);
      मित्र शून्य __vpiArray::word_change(अचिन्हित दीर्घ);
      vvp_fun_arrayport*next_;
};

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net)
: arr_(mem), net_(net), addr_(0)
{
      next_ = 0;
}

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, दीर्घ addr)
: arr_(mem), net_(net), addr_(addr)
{
      next_ = 0;
}

vvp_fun_arrayport::~vvp_fun_arrayport()
{
}

वर्ग vvp_fun_arrayport_sa  : सार्वजनिक vvp_fun_arrayport {

    सार्वजनिक:
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, दीर्घ addr);
      ~vvp_fun_arrayport_sa() override;

      शून्य check_word_change(अचिन्हित दीर्घ addr) override;

      शून्य recv_vec4(vvp_net_ptr_t port, स्थिरांक vvp_vector4_t&bit,
                     vvp_context_t) override;

    खाजगी:
};

vvp_fun_arrayport_sa::vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net)
{
}

vvp_fun_arrayport_sa::vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, दीर्घ addr)
: vvp_fun_arrayport(mem, net, addr)
{
}

vvp_fun_arrayport_sa::~vvp_fun_arrayport_sa()
{
}

शून्य vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t port, स्थिरांक vvp_vector4_t&bit,
                                     vvp_context_t)
{
      तार्किक addr_valid_flag;

      निवड (port.port()) {

	  स्थिती 0: // Address input
	    addr_valid_flag = vector4_to_मूल्य(bit, addr_);
	    जर (! addr_valid_flag)
		  addr_ = arr_->get_size();
	    जर (vpi_array_is_real(arr_))
		  port.ptr()->send_real(arr_->get_word_r(addr_), 0);
	    अन्यथा
		  port.ptr()->send_vec4(arr_->get_word(addr_), 0);

	    थांबा;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_निकास, "XXXX write ports not implemented.\n");
	    प्रतिज्ञा(0);
      }
}

शून्य vvp_fun_arrayport_sa::check_word_change(अचिन्हित दीर्घ addr)
{
      जर (addr != addr_) परत;

      जर (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr_), 0);
      } अन्यथा {
	    net_->send_vec4(arr_->get_word(addr_), 0);
      }
}

वर्ग vvp_fun_arrayport_aa  : सार्वजनिक vvp_fun_arrayport, सार्वजनिक automatic_hooks_s {

    सार्वजनिक:
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, दीर्घ addr);
      ~vvp_fun_arrayport_aa() override;

      शून्य alloc_instance(vvp_context_t context) override;
      शून्य reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      शून्य free_instance(vvp_context_t context) override;
#endif

      शून्य check_word_change(अचिन्हित दीर्घ addr) override;

      शून्य recv_vec4(vvp_net_ptr_t port, स्थिरांक vvp_vector4_t&bit,
                     vvp_context_t context) override;

    खाजगी:
      शून्य check_word_change_(अचिन्हित दीर्घ addr, vvp_context_t context);

      __vpiScope*context_scope_;
      अचिन्हित context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(हे, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, दीर्घ addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(हे, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

शून्य vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      अचिन्हित दीर्घ*addr = नवीन अचिन्हित दीर्घ;
      vvp_set_context_item(context, context_idx_, addr);

      *addr = addr_;
}

शून्य vvp_fun_arrayport_aa::reset_instance(vvp_context_t context)
{
      अचिन्हित दीर्घ*addr = static_cast<अचिन्हित दीर्घ*>
            (vvp_get_context_item(context, context_idx_));

      *addr = addr_;
}

#ifdef CHECK_WITH_VALGRIND
शून्य vvp_fun_arrayport_aa::free_instance(vvp_context_t context)
{
      अचिन्हित दीर्घ*addr = static_cast<अचिन्हित दीर्घ*>
            (vvp_get_context_item(context, context_idx_));
      delete addr;
}
#endif

शून्य vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t port, स्थिरांक vvp_vector4_t&bit,
                                     vvp_context_t context)
{
      जर (context) {
            अचिन्हित दीर्घ*addr = static_cast<अचिन्हित दीर्घ*>
                  (vvp_get_context_item(context, context_idx_));

            तार्किक addr_valid_flag;

            निवड (port.port()) {

                स्थिती 0: // Address input
                  addr_valid_flag = vector4_to_मूल्य(bit, *addr);
                  जर (! addr_valid_flag) *addr = arr_->get_size();
                  जर (vpi_array_is_real(arr_)) {
			port.ptr()->send_real(arr_->get_word_r(*addr),
					      context);
                  } अन्यथा {
			port.ptr()->send_vec4(arr_->get_word(*addr),
					      context);
                  }
                  थांबा;

                पूर्वनिर्धारित:
                  ख_लिखो(मानक_निकास, "XXXX write ports not implemented.\n");
                  प्रतिज्ञा(0);
            }
      } अन्यथा {
            context = context_scope_->live_contexts;
            पर्यंत (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

शून्य vvp_fun_arrayport_aa::check_word_change_(अचिन्हित दीर्घ addr,
                                              vvp_context_t context)
{
      स्थिरांक अचिन्हित दीर्घ*port_addr = static_cast<अचिन्हित दीर्घ*>
            (vvp_get_context_item(context, context_idx_));

      जर (addr != *port_addr)
	    परत;

      जर (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr), context);
      } अन्यथा {
	    net_->send_vec4(arr_->get_word(addr), context);
      }
}

शून्य vvp_fun_arrayport_aa::check_word_change(अचिन्हित दीर्घ addr)
{
      जर (arr_->get_scope()->is_automatic()) {
            प्रतिज्ञा(vthread_get_wt_context());
            check_word_change_(addr, vthread_get_wt_context());
      } अन्यथा {
            vvp_context_t context = context_scope_->live_contexts;
            पर्यंत (context) {
                  check_word_change_(addr, context);
                  context = vvp_get_next_context(context);
            }
      }
}

स्थिर शून्य array_attach_port(vvp_array_t array, vvp_fun_arrayport*fun)
{
      प्रतिज्ञा(fun->next_ == 0);
      fun->next_ = array->ports_;
      array->ports_ = fun;
      जर (!array->get_scope()->is_automatic() &&
          (array->vals4 || array->vals)) {
              /* propagate initial values for variable arrays */
            जर (!vpi_array_is_real(array)) {
                  vvp_bit4_t init;
                  जर (array->vals4)
                      init = BIT4_X;
                  अन्यथा
                      init = BIT4_0;
                  vvp_vector4_t tmp(array->vals_width, init);
                  schedule_init_propagate(fun->net_, tmp);
            } अन्यथा {
                  schedule_init_propagate(fun->net_, 0.0);
            }
      }
}

वर्ग array_word_मूल्य_callback : सार्वजनिक मूल्य_callback {
    सार्वजनिक:
      ओळीत explicit array_word_मूल्य_callback(p_cb_data data, दीर्घ addr)
      : मूल्य_callback(data), word_addr(addr)
      { }

    सार्वजनिक:
      दीर्घ word_addr;
};

शून्य __vpiArray::word_change(अचिन्हित दीर्घ addr)
{
      साठी (vvp_fun_arrayport*cur = ports_; cur; cur = cur->next_)
	    cur->check_word_change(addr);

	// Run callbacks attached to the array itself.
      संरचना __vpiCallback *next = vpi_callbacks;
      संरचना __vpiCallback *prev = 0;

      पर्यंत (next) {
	    array_word_मूल्य_callback*cur = dynamic_cast<array_word_मूल्य_callback*>(next);
	    next = cur->next;

	      // Skip callbacks that are not for me. -1 is for every element.
	    जर (cur->word_addr != (दीर्घ)addr && cur->word_addr != -1) {
		  prev = cur;
		  सुरूठेवा;
	    }

	      // For whole array callbacks we need to set the index.
	    जर (cur->word_addr == -1) {
		  cur->cb_data.index = (PLI_INT32) ((पूर्णांक)addr +
						    first_addr.get_मूल्य());
	    }

	    जर (cur->cb_data.cb_rtn != 0) {
		  जर (cur->test_मूल्य_callback_ready()) {
			जर (cur->cb_data.value) {
			      जर (vpi_array_is_real(हे)) {
				    द्विगुण val = 0.0;
				    जर (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_real_get_मूल्य(val, cur->cb_data.value);
			      } अन्यथा जर (vals4) {
				    vpip_vec4_get_मूल्य(vals4->get_word(addr),
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } अन्यथा जर (dynamic_cast<vvp_darray_atom<int8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int64_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint64_t>*>(vals)
				      || dynamic_cast<vvp_darray_vec2*>(vals)) {
				    vvp_vector4_t val;
				    जर (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_vec4_get_मूल्य(val,
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } अन्यथा {
			            प्रतिज्ञा(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } अन्यथा जर (prev == 0) {

		  vpi_callbacks = next;
		  cur->next = 0;
		  delete cur;

	    } अन्यथा {
		  प्रतिज्ञा(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}

वर्ग array_resolv_list_t : सार्वजनिक resolv_list_s {

    सार्वजनिक:
      explicit array_resolv_list_t(अक्षर*lab) : resolv_list_s(lab) {
	    array = 0;
      }

      vvp_array_t*array;
      तार्किक resolve(तार्किक mes) override;

    खाजगी:
};

तार्किक array_resolv_list_t::resolve(तार्किक mes)
{
      *array = array_find(label());
      जर (*array == 0) {
	    प्रतिज्ञा(!mes);
	    परत असत्य;
      }
      परत सत्य;
}

वर्ग array_port_resolv_list_t : सार्वजनिक resolv_list_s {

    सार्वजनिक:
      explicit array_port_resolv_list_t(अक्षर* lab, तार्किक use_addr__,
                                        दीर्घ addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      तार्किक use_addr;
      दीर्घ addr;
      तार्किक resolve(तार्किक mes) override;

    खाजगी:
};

array_port_resolv_list_t::array_port_resolv_list_t(अक्षर *lab, तार्किक use_addr__,
                                                   दीर्घ addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      जर (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      अन्यथा
            context_scope = 0;
      ptr = नवीन vvp_net_t;
}

तार्किक array_port_resolv_list_t::resolve(तार्किक mes)
{
      vvp_array_t mem = array_find(label());
      जर (mem == 0) {
	    प्रतिज्ञा(mem || !mes);
	    परत असत्य;
      }

      vvp_fun_arrayport*fun;
      जर (use_addr)
            जर (context_scope)
                  fun = नवीन vvp_fun_arrayport_aa(context_scope, mem, ptr, addr);
            अन्यथा
                  fun = नवीन vvp_fun_arrayport_sa(mem, ptr, addr);
      अन्यथा
            जर (context_scope)
                  fun = नवीन vvp_fun_arrayport_aa(context_scope, mem, ptr);
            अन्यथा
                  fun = नवीन vvp_fun_arrayport_sa(mem, ptr);
      ptr->fun = fun;

      array_attach_port(mem, fun);

      परत सत्य;
}

वर्ग array_word_part_callback : सार्वजनिक array_word_मूल्य_callback {
    सार्वजनिक:
      explicit array_word_part_callback(p_cb_data data, दीर्घ addr);
      ~array_word_part_callback() override;

      array_word_part_callback(स्थिरांक array_word_part_callback&) = delete;
      array_word_part_callback& चालक=(स्थिरांक array_word_part_callback&) = delete;

      तार्किक test_मूल्य_callback_ready(शून्य) override;

    खाजगी:
      अक्षर*मूल्य_bits;
};

array_word_part_callback::array_word_part_callback(p_cb_data data, दीर्घ addr)
: array_word_मूल्य_callback(data, addr)
{
	// Get the initial value of the part, to use as a reference.
      संरचना __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj);
      s_vpi_मूल्य tmp_मूल्य;
      tmp_मूल्य.format = vpiBinStrVal;
      apvword->vpi_get_मूल्य(&tmp_मूल्य);

      मूल्य_bits = नवीन अक्षर[apvword->part_wid+1];

      स_नकल(मूल्य_bits, tmp_मूल्य.value.str, apvword->part_wid);
      मूल्य_bits[apvword->part_wid] = 0;
}

array_word_part_callback::~array_word_part_callback()
{
      delete[]मूल्य_bits;
}

तार्किक array_word_part_callback::test_मूल्य_callback_ready(शून्य)
{
      संरचना __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_data.obj);
      प्रतिज्ञा(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_मूल्य tmp_मूल्य;
      tmp_मूल्य.format = vpiBinStrVal;
      apvword->vpi_get_मूल्य(&tmp_मूल्य);

      जर (स_भेद(मूल्य_bits, tmp_मूल्य.value.str, apvword->part_wid) == 0)
	    परत असत्य;

      स_नकल(मूल्य_bits, tmp_मूल्य.value.str, apvword->part_wid);
      परत सत्य;

}

मूल्य_callback*vpip_array_word_change(p_cb_data data)
{
      संरचना __vpiArray*parent = 0;
      array_word_मूल्य_callback*cbh = 0;
      जर (स्थिरांक संरचना __vpiArrayWord*word = array_var_word_from_handle(data->obj)) {
	    parent = static_cast<__vpiArray*>(word->get_parent());
	    अचिन्हित addr = word->get_index();
	    cbh = नवीन array_word_मूल्य_callback(data, addr);

      } अन्यथा जर (संरचना __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(data->obj)) {
	    parent = tword->array;
	    cbh = नवीन array_word_मूल्य_callback(data, tword->address);

      } अन्यथा जर (संरचना __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj)) {
	    parent = apvword->array;
	    cbh = नवीन array_word_part_callback(data, apvword->word_sel);
      }

      प्रतिज्ञा(cbh);
      प्रतिज्ञा(parent);
      cbh->next = parent->vpi_callbacks;
      parent->vpi_callbacks = cbh;

      परत cbh;
}

मूल्य_callback* vpip_array_change(p_cb_data data)
{
      array_word_मूल्य_callback*cbh = नवीन array_word_मूल्य_callback(data, -1);
      प्रतिज्ञा(data->obj);

      संरचना __vpiArray*arr = dynamic_cast<__vpiArray*>(data->obj);
      cbh->next = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      परत cbh;
}

शून्य compile_array_port(अक्षर*label, अक्षर*array, अक्षर*addr)
{
      array_port_resolv_list_t*resolv_mem
	    = नवीन array_port_resolv_list_t(array, असत्य, 0);

      define_functor_चिन्ह(label, resolv_mem->ptr);
      मुक्त(label);
	// Connect the port-0 input as the address.
      आदान_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

शून्य compile_array_port(अक्षर*label, अक्षर*array, दीर्घ addr)
{
      array_port_resolv_list_t*resolv_mem
	    = नवीन array_port_resolv_list_t(array, सत्य, addr);

      define_functor_चिन्ह(label, resolv_mem->ptr);
      मुक्त(label);

      resolv_submit(resolv_mem);
}

शून्य compile_array_alias(अक्षर*label, अक्षर*name, अक्षर*src)
{
      vvp_array_t mem = array_find(src);
      प्रतिज्ञा(mem);

      संरचना __vpiArray*obj = नवीन __vpiArray;

      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_मोजणी = mem->array_मोजणी;
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
      obj->vals_शब्द = mem->vals_शब्द;

      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

      प्रतिज्ञा(array_table);
      प्रतिज्ञा(!array_find(label));
      array_table->sym_set_मूल्य(label, obj);

      compile_vpi_चिन्ह(label, obj);
      vpip_attach_to_current_scope(obj);

      मुक्त(label);
      मुक्त(name);
      मुक्त(src);
}

/*
 * &A<label,addr>
 * This represents a VPI handle for an addressed array. This comes
 * from expressions like "label[addr]" where "label" is the array and
 * "addr" is the canonical address of the desired word.
 */
vpiHandle vpip_make_vthr_A(अक्षर*label, अचिन्हित addr)
{
      संरचना __vpiArrayVthrA*obj = नवीन __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = नवीन array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      obj->address = addr;

      परत obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(अक्षर*label, अक्षर*चिन्ह)
{
      संरचना __vpiArrayVthrA*obj = नवीन __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = नवीन array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      compile_vpi_lookup(&obj->address_handle, चिन्ह);
      obj->address = 0;

      परत obj;
}

vpiHandle vpip_make_vthr_A(अक्षर*label, vpiHandle handle)
{
      संरचना __vpiArrayVthrA*obj = नवीन __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = नवीन array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = handle;
      obj->address = 0;

      परत obj;
}

vpiHandle vpip_make_vthr_APV(अक्षर*label, अचिन्हित index, अचिन्हित bit, अचिन्हित wid)
{
      संरचना __vpiArrayVthrAPV*obj = नवीन __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = नवीन array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->word_sel = index;
      obj->part_bit = bit;
      obj->part_wid = wid;

      परत obj;
}

शून्य compile_array_cleanup(शून्य)
{
      delete array_table;
      array_table = 0;
}

#ifdef CHECK_WITH_VALGRIND
शून्य memory_delete(vpiHandle item)
{
      संरचना __vpiArray*arr = static_cast<संरचना __vpiArray*>(item);
      जर (arr->vals_शब्द) delete [] (arr->vals_शब्द-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals;

      जर (arr->nets) {
	    साठी (अचिन्हित idx = 0; idx < arr->get_size(); idx += 1) {
		  जर (संरचना __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.index);
		    /* These should only be the real words. */
		  } अन्यथा {
			प्रतिज्ञा(arr->nets[idx]->get_type_code() ==
			       vpiRealVar);
			संरचना __vpiRealVar *sigr = static_cast<संरचना __vpiRealVar *>
			                            (arr->nets[idx]);
			constant_delete(sigr->id.index);
// Why are only the real words still here?
			delete arr->nets[idx];
		  }
	    }
	    मुक्त(arr->nets);
      }

      पर्यंत (arr->vpi_callbacks) {
	    संरचना __vpiCallback*tmp = arr->vpi_callbacks->next;
	    delete arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      delete arr;
}

शून्य A_delete(vpiHandle item)
{
      संरचना __vpiArrayVthrA*obj = static_cast<संरचना __vpiArrayVthrA*>(item);
      जर (obj->address_handle) {
	    निवड (obj->address_handle->get_type_code()) {
		स्थिती vpiMemoryWord:
		  जर (vpi_get(_vpiFromThr, obj->address_handle) == _vpi_at_A) {
			A_delete(obj->address_handle);
		  }
		  थांबा;
		स्थिती vpiPartSelect:
		  प्रतिज्ञा(vpi_get(_vpiFromThr, obj->address_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->address_handle);
		  थांबा;
	    }
      }

      delete obj;
}

शून्य APV_delete(vpiHandle item)
{
      संरचना __vpiArrayVthrAPV*obj = static_cast<संरचना __vpiArrayVthrAPV*>(item);
      delete obj;
}
#endif
