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

unsigned long गणनम्_net_arrays = 0;
unsigned long गणनम्_net_array_शब्दाः = 0;
unsigned long गणनम्_var_arrays = 0;
unsigned long गणनम्_var_array_शब्दाः = 0;
unsigned long गणनम्_real_arrays = 0;
unsigned long गणनम्_real_array_शब्दाः = 0;

स्थिर चिह्नम्_map_s<संरचना __vpiArray>* array_table =0;

वग्ग vvp_fun_arrayport;
स्थिर शून्य array_attach_port(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_find(ध्रुव वर्ण*label)
{
      सचे (array_table == 0)
	    पटिनिवत्त 0;

      vvp_array_t v = array_table->sym_get_मानम्(label);
      पटिनिवत्त v;
}

संरचना __vpiArrayVthrA : सार्वजनिक __vpiHandle {
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { पटिनिवत्त vpiMemoryWord; }
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण* vpi_get_str(पूर्णाङ्क code) override;
      शून्य vpi_get_मानम्(p_vpi_मानम् val) override;
      vpiHandle vpi_put_मानम्(p_vpi_मानम् val, पूर्णाङ्क flags) override;
      vpiHandle vpi_handle(पूर्णाङ्क code) override;

      संरचना __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle address_handle;
	// If wid==0, then address is the address into the array.
      unsigned address;

      unsigned get_address() ध्रुव
      {
	    सचे (address_handle) {
		  s_vpi_मानम् vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpiVectorVal;
		  address_handle->vpi_get_मानम्(&vp);
		  पूर्णाङ्क शब्दाः = (address_handle->vpi_get(vpiSize)-1)/32 + 1;
		  अत्थाय(पूर्णाङ्क idx = 0; idx < शब्दाः; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			सचे (vp.value.vector[idx].bval != 0) पटिनिवत्त UINT_MAX;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  address_handle->vpi_get_मानम्(&vp);
		  पटिनिवत्त vp.value.integer;
	    }

	    पटिनिवत्त address;
      }
};


संरचना __vpiArrayVthrAPV : सार्वजनिक __vpiHandle {
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override { पटिनिवत्त vpiPartSelect; }
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण* vpi_get_str(पूर्णाङ्क code) override;
      शून्य vpi_get_मानम्(p_vpi_मानम् val) override;
      vpiHandle vpi_handle(पूर्णाङ्क code) override;

      संरचना __vpiArray*array;
      unsigned word_sel;
      unsigned part_bit;
      unsigned part_wid;
};

तार्किक is_net_array(vpiHandle obj)
{
      संरचना __vpiArray*rfp = dynamic_cast<__vpiArray*> (obj);
      प्रतिज्ञा(rfp);

      सचे (rfp->nets != 0) पटिनिवत्त सच्च;
      पटिनिवत्त मिच्छा;
}

// This function return true if the underlying array words are real.
स्थिर तार्किक vpi_array_is_real(ध्रुव vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      सचे (arr->vals4 != 0)  // A bit based variable/register array.
	    पटिनिवत्त मिच्छा;

      सचे (dynamic_cast<vvp_darray_real*> (arr->vals))
	    पटिनिवत्त सच्च;

      सचे (arr->vals != 0)
	    पटिनिवत्त मिच्छा;

	// This must be a net array so look at element 0 to find the type.
      प्रतिज्ञा(arr->nets != 0);
      प्रतिज्ञा(arr->get_size() > 0);
      ध्रुव संरचना __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      सचे (rsig) {
	    पटिनिवत्त सच्च;
      }

      पटिनिवत्त मिच्छा;
}

स्थिर तार्किक vpi_array_is_string(ध्रुव vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      सचे (arr->vals4 != 0)  // A bit based variable/register array.
	    पटिनिवत्त मिच्छा;

      सचे (dynamic_cast<vvp_darray_string*> (arr->vals))
	    पटिनिवत्त सच्च;

      पटिनिवत्त मिच्छा;
}

पूर्णाङ्क __vpiArray::get_word_size() ध्रुव
{
      unsigned width;

      प्रतिज्ञा(get_size() > 0);
	/* For a net array we need to get the width from the first element. */
      सचे (nets) {
	    प्रतिज्ञा(vals4 == 0 && vals == 0);
	    ध्रुव संरचना __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    प्रतिज्ञा(vsig);
	    width = vpip_size(vsig);
	/* For a variable array we can get the width from vals_width. */
      } अञ्ञथा {
	    प्रतिज्ञा(vals4 || vals);
	    width = vals_width;
      }

      पटिनिवत्त width;
}

वर्ण*__vpiArray::get_word_str(संरचना __vpiArrayWord*word, पूर्णाङ्क code)
{
      unsigned index = word->get_index();

      सचे (code == vpiFile) {  // Not implemented for now!
	    पटिनिवत्त simple_set_rbuf_str(file_names[0]);
      }

      वर्ण sidx [64];
      snprintf(sidx, 63, "%d", (पूर्णाङ्क)index + first_addr.get_मानम्());
      पटिनिवत्त generic_get_str(code, get_scope(), name, sidx);
}

शून्य __vpiArray::get_word_मानम्(संरचना __vpiArrayWord*word, p_vpi_मानम् vp)
{
      unsigned index = word->get_index();

    // Determine the appropriate format (The Verilog PLI Handbook 5.2.10)
      सचे(vp->format == vpiObjTypeVal) {
          सचे(vpi_array_is_real(एतत्))
              vp->format = vpiRealVal;
          अञ्ञथा सचे(vpi_array_is_string(एतत्))
              vp->format = vpiStringVal;
          अञ्ञथा
              vp->format = vpiIntVal;
      }

      सचे(vals4) {
          vpip_vec4_get_मानम्(vals4->get_word(index),
                              vals4->width(), signed_flag, vp);
      } अञ्ञथा सचे(vals) {
          विकल्प(vp->format) {
            पक्ष vpiBinStrVal:
            पक्ष vpiOctStrVal:
            पक्ष vpiDecStrVal:
            पक्ष vpiHexStrVal:
            पक्ष vpiScalarVal:
            पक्ष vpiIntVal:
            पक्ष vpiVectorVal:
            {
                vvp_vector4_t v;
                vals->get_word(index, v);
                vpip_vec4_get_मानम्(v, vals_width, signed_flag, vp);
            }
            भङ्ग;

            पक्ष vpiRealVal:
            {
                द्विगुण d;
                vals->get_word(index, d);
                vpip_real_get_मानम्(d, vp);
            }
            भङ्ग;

            पक्ष vpiStringVal:
            {
                string s;
                vals->get_word(index, s);
                vpip_string_get_मानम्(s, vp);
            }
            भङ्ग;

            स्वभाव:
                fprintf(stderr, "vpi sorry: format is not implemented\n");
                प्रतिज्ञा(मिच्छा);
          }
      }
}

शून्य __vpiArray::put_word_मानम्(संरचना __vpiArrayWord*word, p_vpi_मानम् vp, पूर्णाङ्क)
{
      unsigned index = word->get_index();
      vvp_vector4_t val = vec4_from_vpi_मानम्(vp, vals_width);
      set_word(index, 0, val);
}

vpiHandle __vpiArray::get_iter_index(संरचना __vpiArrayIterator*, पूर्णाङ्क idx)
{
      सचे (nets) पटिनिवत्त nets[idx];

      प्रतिज्ञा(vals4 || vals);

      सचे (vals_शब्दाः == 0) make_vals_शब्दाः();

      पटिनिवत्त &(vals_शब्दाः[idx].as_word);
}

पूर्णाङ्क __vpiArray::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpi_पङ्क्तिः_No:
	    पटिनिवत्त 0; // Not implemented for now!

	  पक्ष vpiSize:
	    पटिनिवत्त get_size();

	  पक्ष vpiAutomatic:
	    पटिनिवत्त scope->is_automatic()? 1 : 0;

	  स्वभाव:
	    पटिनिवत्त 0;
      }
}

वर्ण* __vpiArray::vpi_get_str(पूर्णाङ्क code)
{
      सचे (code == vpiFile) {  // Not implemented for now!
            पटिनिवत्त simple_set_rbuf_str(file_names[0]);
      }

      पटिनिवत्त generic_get_str(code, scope, name, रिक्त);
}

vpiHandle __vpiArray::vpi_handle(पूर्णाङ्क code)
{
      विकल्प (code) {
          पक्ष vpi_वामः_Range:
            सचे (swap_addr) पटिनिवत्त &last_addr;
            अञ्ञथा पटिनिवत्त &first_addr;

          पक्ष vpi_दक्षिणः_Range:
            सचे (swap_addr) पटिनिवत्त &first_addr;
            अञ्ञथा पटिनिवत्त &last_addr;

	  पक्ष vpiScope:
	    पटिनिवत्त scope;

	  पक्ष vpiModule:
	    पटिनिवत्त vpip_module(scope);
      }

      पटिनिवत्त 0;
}

/*
* VPI code passes indices that are not yet converted to canonical
* form, so this index function does it here.
*/
vpiHandle __vpiArray::vpi_index(पूर्णाङ्क index)
{
      index -= first_addr.get_मानम्();
      सचे (index >= (long) get_size())
	    पटिनिवत्त 0;
      सचे (index < 0)
	    पटिनिवत्त 0;

      सचे (nets != 0) {
	    पटिनिवत्त nets[index];
      }

      सचे (vals_शब्दाः == 0)
	    make_vals_शब्दाः();

      पटिनिवत्त &(vals_शब्दाः[index].as_word);
}

पूर्णाङ्क __vpiArrayWord::as_word_t::vpi_get(पूर्णाङ्क code)
{
      संरचना __vpiArrayWord*obj = array_var_word_from_handle(एतत्);
      प्रतिज्ञा(obj);
      संरचना __vpiArrayBase*my_parent = obj->get_parent();
      t_vpi_मानम् val;

      विकल्प (code) {
	  पक्ष vpi_पङ्क्तिः_No:
	    पटिनिवत्त 0; // Not implemented for now!

	  पक्ष vpiSize:
	    पटिनिवत्त my_parent->get_word_size();

	  पक्ष vpi_वामः_Range:
            val.format = vpiIntVal;
	    my_parent->get_वामः_range()->vpi_get_मानम्(&val);
            प्रतिज्ञा(val.format == vpiIntVal);
            पटिनिवत्त val.value.integer;

	  पक्ष vpi_दक्षिणः_Range:
            val.format = vpiIntVal;
	    my_parent->get_दक्षिणः_range()->vpi_get_मानम्(&val);
            प्रतिज्ञा(val.format == vpiIntVal);
            पटिनिवत्त val.value.integer;

	  पक्ष vpiIndex:
	    {
		  पूर्णाङ्क base_offset = 0;
		  संरचना __vpiArray*base = dynamic_cast<__vpiArray*> (my_parent);
		  सचे (base) {
			val.format = vpiIntVal;
			base->first_addr.vpi_get_मानम्(&val);
			base_offset += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_index.vpi_get_मानम्(&val);
		  प्रतिज्ञा(val.format == vpiIntVal);
		  पटिनिवत्त val.value.integer + base_offset;
	    }

	  पक्ष vpiAutomatic:
	    पटिनिवत्त my_parent->get_scope()->is_automatic()? 1 : 0;

#सचे defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  पक्ष _vpiFromThr:
	    पटिनिवत्त _vpiNoThr;
#endif

	  स्वभाव:
	    पटिनिवत्त 0;
      }
}


पूर्णाङ्क __vpiArrayVthrA::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpi_पङ्क्तिः_No:
	    पटिनिवत्त 0; // Not implemented for now!

	  पक्ष vpiSize:
	    पटिनिवत्त array->get_word_size();

	  पक्ष vpi_वामः_Range:
	    पटिनिवत्त array->msb.get_मानम्();

	  पक्ष vpi_दक्षिणः_Range:
	    पटिनिवत्त array->lsb.get_मानम्();

	  पक्ष vpiIndex:
	    पटिनिवत्त (पूर्णाङ्क)get_address() + array->first_addr.get_मानम्();

	  पक्ष vpiAutomatic:
	    पटिनिवत्त array->get_scope()->is_automatic() ? 1 : 0;

#सचे defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  पक्ष _vpiFromThr:
	    पटिनिवत्त _vpi_at_A;
#endif

	  // If address_handle is not zero we definitely have a
	  // variable.
	  पक्ष vpiConstantSelect:
	    पटिनिवत्त address_handle == 0;

	  स्वभाव:
	    पटिनिवत्त 0;
      }
}

वर्ण* __vpiArrayVthrA::vpi_get_str(पूर्णाङ्क code)
{
      सचे (code == vpiFile) {  // Not implemented for now!
            पटिनिवत्त simple_set_rbuf_str(file_names[0]);
      }

      वर्ण sidx [64];
      snprintf(sidx, 63, "%d", (पूर्णाङ्क)get_address() + array->first_addr.get_मानम्());
      पटिनिवत्त generic_get_str(code, array->get_scope(), array->name, sidx);
}

शून्य __vpiArrayVthrA::vpi_get_मानम्(p_vpi_मानम् vp)
{
      प्रतिज्ञा(array);

      unsigned index = get_address();
      सचे (vpi_array_is_real(array)) {
	    द्विगुण tmp = array->get_word_r(index);
	    vpip_real_get_मानम्(tmp, vp);
      } अञ्ञथा सचे (vpi_array_is_string(array)) {
	    string tmp = array->get_word_str(index);
	    vpip_string_get_मानम्(tmp, vp);
      } अञ्ञथा {
	    vvp_vector4_t tmp = array->get_word(index);
	    unsigned width = array->get_word_size();
	    vpip_vec4_get_मानम्(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_मानम्(p_vpi_मानम् vp, पूर्णाङ्क)
{
      unsigned index = get_address();

      प्रतिज्ञा(array);
      प्रतिज्ञा(index < array->get_size());

      सचे (vpi_array_is_real(array)) {
	    द्विगुण val = real_from_vpi_मानम्(vp);
	    array->set_word(index, val);
      } अञ्ञथा {
	    unsigned width = array->get_word_size();
	    vvp_vector4_t val = vec4_from_vpi_मानम्(vp, width);
	    array->set_word(index, 0, val);
      }

      पटिनिवत्त एतत्;
}

vpiHandle __vpiArrayVthrA::vpi_handle(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiIndex:
	    भङ्ग;  // Not implemented!

	  पक्ष vpi_वामः_Range:
	    पटिनिवत्त &array->msb;

	  पक्ष vpi_दक्षिणः_Range:
	    पटिनिवत्त &array->lsb;

	  पक्ष vpiParent:
	  पक्ष vpiArray:
	    पटिनिवत्त array;

	  पक्ष vpiScope:
	    पटिनिवत्त array->get_scope();

	  पक्ष vpiModule:
	    पटिनिवत्त vpip_module(array->get_scope());
      }

      पटिनिवत्त 0;
}


पूर्णाङ्क __vpiArrayVthrAPV::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpi_पङ्क्तिः_No:
	    पटिनिवत्त 0; // Not implemented for now!

	  पक्ष vpiSize:
	    पटिनिवत्त part_wid;

	  पक्ष vpi_वामः_Range:
	    पटिनिवत्त part_bit + part_wid - 1;

	  पक्ष vpi_दक्षिणः_Range:
	    पटिनिवत्त part_bit;

	  पक्ष vpiAutomatic:
	    पटिनिवत्त array->get_scope()->is_automatic() ? 1 : 0;

#सचे defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  पक्ष _vpiFromThr:
	    पटिनिवत्त _vpi_at_APV;
#endif

	  पक्ष vpiConstantSelect:
	    पटिनिवत्त 1;

	  स्वभाव:
	    पटिनिवत्त 0;
      }
}

वर्ण* __vpiArrayVthrAPV::vpi_get_str(पूर्णाङ्क code)
{
      सचे (code == vpiFile) {  // Not implemented for now!
            पटिनिवत्त simple_set_rbuf_str(file_names[0]);
      }

      वर्ण sidx [64];
      snprintf(sidx, 63, "%u", word_sel + array->first_addr.get_मानम्());
      पटिनिवत्त generic_get_str(code, array->get_scope(), array->name, sidx);
}

शून्य __vpiArrayVthrAPV::vpi_get_मानम्(p_vpi_मानम् vp)
{
      प्रतिज्ञा(array);

      unsigned index = word_sel;
      सचे (vpi_array_is_real(array)) {
	    द्विगुण tmp = array->get_word_r(index);
	    vpip_real_get_मानम्(tmp, vp);
      } अञ्ञथा {
	    vvp_vector4_t tmp = array->get_word(index);
	    tmp = tmp.subvalue(part_bit, part_wid);
	    vpip_vec4_get_मानम्(tmp, part_wid, मिच्छा, vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(पूर्णाङ्क code)
{
      विकल्प (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  पक्ष vpiParent:
	    पटिनिवत्त 0;

            // Not part of the Verilog standard. We use this internally.
	  पक्ष vpiArray:
	    पटिनिवत्त array;

	  पक्ष vpiScope:
	    पटिनिवत्त array->get_scope();

	  पक्ष vpiModule:
	    पटिनिवत्त vpip_module(array->get_scope());
      }

      पटिनिवत्त 0;
}

शून्य __vpiArray::set_word(unsigned address, unsigned part_off, ध्रुव vvp_vector4_t&val)
{
      सचे (address >= get_size())
	    पटिनिवत्त;

      सचे (vals4) {
	    प्रतिज्ञा(nets == 0);
	    सचे (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp = vals4->get_word(address);
		  सचे ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			प्रतिज्ञा(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals4->set_word(address, tmp);
	    } अञ्ञथा {
		  vals4->set_word(address, val);
	    }
	    word_change(address);
	    पटिनिवत्त;
      }

      सचे (vals) {
	    प्रतिज्ञा(nets == 0);
	    सचे (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp;
		  vals->get_word(address, tmp);
		  सचे ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			प्रतिज्ञा(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals->set_word(address, tmp);
	    } अञ्ञथा {
		  vals->set_word(address, val);
	    }
	    word_change(address);
	    पटिनिवत्त;
      }

      प्रतिज्ञा(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle word = nets[address];
      ध्रुव संरचना __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      प्रतिज्ञा(vsig);

      vsig->ग्रन्थिः->send_vec4_pv(val, part_off, vpip_size(vsig), 0);
      word_change(address);
}

शून्य __vpiArray::set_word(unsigned address, द्विगुण val)
{
      प्रतिज्ञा(vals != 0);
      प्रतिज्ञा(nets == 0);

      सचे (address >= vals->get_size())
	    पटिनिवत्त;

      vals->set_word(address, val);
      word_change(address);
}

शून्य __vpiArray::set_word(unsigned address, ध्रुव string&val)
{
      प्रतिज्ञा(vals != 0);
      प्रतिज्ञा(nets == 0);

      सचे (address >= vals->get_size())
	    पटिनिवत्त;

      vals->set_word(address, val);
      word_change(address);
}

शून्य __vpiArray::set_word(unsigned address, ध्रुव vvp_object_t&val)
{
      प्रतिज्ञा(vals != 0);
      प्रतिज्ञा(nets == 0);

      सचे (address >= vals->get_size())
	    पटिनिवत्त;

      vals->set_word(address, val);
      word_change(address);
}

vvp_vector4_t __vpiArray::get_word(unsigned address)
{
      सचे (vals4) {
	    प्रतिज्ञा(nets == 0);
	    प्रतिज्ञा(vals == 0);
	    पटिनिवत्त vals4->get_word(address);
      }

      सचे (vals) {
	    प्रतिज्ञा(nets == 0);
	    प्रतिज्ञा(vals4== 0);
	    सचे (address >= vals->get_size())
		  पटिनिवत्त vvp_vector4_t(vals_width, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_word(address, val);
	    पटिनिवत्त val;
      }

      प्रतिज्ञा(vals4 == 0);
      प्रतिज्ञा(vals == 0);
      प्रतिज्ञा(nets != 0);

      सचे (address >= get_size()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    प्रतिज्ञा(get_size() > 0);
	    vpiHandle word = nets[0];
	    प्रतिज्ञा(word);
	    संरचना __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
	    प्रतिज्ञा(vsig);
	    vvp_signal_मानम्*sig = dynamic_cast<vvp_signal_मानम्*> (vsig->ग्रन्थिः->fil);
	    प्रतिज्ञा(sig);
	    पटिनिवत्त vvp_vector4_t(sig->मानम्_size(), BIT4_X);
      }

      vpiHandle word = nets[address];
      संरचना __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      प्रतिज्ञा(vsig);
      vvp_signal_मानम्*sig = dynamic_cast<vvp_signal_मानम्*> (vsig->ग्रन्थिः->fil);
      प्रतिज्ञा(sig);

      vvp_vector4_t val;
      sig->vec4_मानम्(val);
      पटिनिवत्त val;
}

द्विगुण __vpiArray::get_word_r(unsigned address)
{
      सचे (vals) {
	    प्रतिज्ञा(vals4 == 0);
	    प्रतिज्ञा(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    सचे (address >= vals->get_size())
		  पटिनिवत्त 0.0;

	    द्विगुण val;
	    vals->get_word(address, val);
	    पटिनिवत्त val;
      }

      सचे (address >= get_size())
	    पटिनिवत्त 0.0;

      प्रतिज्ञा(nets);
      vpiHandle word = nets[address];
      संरचना __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(word);
      प्रतिज्ञा(vsig);
      vvp_signal_मानम्*sig = dynamic_cast<vvp_signal_मानम्*> (vsig->net->fil);
      प्रतिज्ञा(sig);

      द्विगुण val = sig->real_मानम्();
      पटिनिवत्त val;
}

शून्य __vpiArray::get_word_obj(unsigned address, vvp_object_t&val)
{
      सचे (vals) {
	    प्रतिज्ञा(vals4 == 0);
	    प्रतिज्ञा(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    सचे (address >= vals->get_size()) {
		  val = vvp_object_t();
		  पटिनिवत्त;
	    }

	    vals->get_word(address, val);
	    पटिनिवत्त;
      }

      प्रतिज्ञा(nets);
	// Arrays of string nets not implemented!
      प्रतिज्ञा(0);
      पटिनिवत्त;
}

string __vpiArray::get_word_str(unsigned address)
{
      सचे (vals) {
	    प्रतिज्ञा(vals4 == 0);
	    प्रतिज्ञा(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    सचे (address >= vals->get_size())
		  पटिनिवत्त "";

	    string val;
	    vals->get_word(address, val);
	    पटिनिवत्त val;
      }

      प्रतिज्ञा(nets);
	// Arrays of string nets not implemented!
      प्रतिज्ञा(0);
      पटिनिवत्त "";
}

vpiHandle vpip_make_array(ध्रुव वर्ण*label, ध्रुव वर्ण*name,
                          पूर्णाङ्क first_addr, पूर्णाङ्क last_addr,
                          तार्किक signed_flag)
{
      संरचना __vpiArray*obj = नूतन __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      सचे (last_addr >= first_addr) {
	    obj->swap_addr = मिच्छा;
      } अञ्ञथा {
	    obj->swap_addr = सच्च;
	    पूर्णाङ्क tmp = last_addr;
	    last_addr = first_addr;
	    first_addr = tmp;
      }
      प्रतिज्ञा(last_addr >= first_addr);
      unsigned array_गणनम् = last_addr+1-first_addr;

	// For now, treat all arrays as memories. This is not quite
	// correct, as arrays are arrays with memories a special case.
      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_गणनम् = array_गणनम्;

      obj->first_addr.set_मानम्(first_addr);
      obj->last_addr .set_मानम्(last_addr);

	// Start off now knowing if we are nets or variables.
      obj->nets = 0;
      obj->vals4 = 0;
      obj->vals  = 0;
      obj->vals_width = 0;
      obj->vals_शब्दाः = 0;

	// Initialize (clear) the read-ports list.
      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

	/* Add this symbol to the array_symbols table for later lookup. */
      सचे (!array_table)
	    array_table = नूतन चिह्नम्_map_s<संरचना __vpiArray>;

      प्रतिज्ञा(!array_find(label));
      array_table->sym_set_मानम्(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_चिह्नम्(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_to_current_scope(obj);

      पटिनिवत्त obj;
}

शून्य __vpiArray::alias_word(unsigned long addr, vpiHandle word, पूर्णाङ्क msb_, पूर्णाङ्क lsb_)
{
      प्रतिज्ञा(msb.get_मानम्() == msb_);
      प्रतिज्ञा(lsb.get_मानम्() == lsb_);
      प्रतिज्ञा(addr < get_size());
      प्रतिज्ञा(nets);
      nets[addr] = word;
}

शून्य __vpiArray::attach_word(unsigned addr, vpiHandle word)
{
      प्रतिज्ञा(addr < get_size());
      प्रतिज्ञा(nets);
      nets[addr] = word;

      सचे (संरचना __vpiSignal*sig = dynamic_cast<__vpiSignal*>(word)) {
	    vvp_net_t*net = sig->ग्रन्थिः;
	    प्रतिज्ञा(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    प्रतिज्ञा(fun);
	    fun->attach_as_word(एतत्, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = एतत्;
	    sig->id.index = नूतन __vpiDecConst(addr + first_addr.get_मानम्());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    पटिनिवत्त;
      }

      सचे (संरचना __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(word)) {
	    vvp_net_t*net = sig->net;
	    प्रतिज्ञा(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    प्रतिज्ञा(fun);
	    fun->attach_as_word(एतत्, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = एतत्;
	    sig->id.index = नूतन __vpiDecConst(addr + first_addr.get_मानम्());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = सच्च;
	    पटिनिवत्त;
      }
}

शून्य compile_var_array(वर्ण*label, वर्ण*name, पूर्णाङ्क last, पूर्णाङ्क first,
		   पूर्णाङ्क msb, पूर्णाङ्क lsb, वर्ण signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last,
                                      signed_flag != 0);

      संरचना __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_width = labs(msb-lsb) + 1;
      सचे (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = नूतन vvp_vector4array_aa(arr->vals_width,
						 arr->get_size());
      } अञ्ञथा {
            arr->vals4 = नूतन vvp_vector4array_sa(arr->vals_width,
						 arr->get_size());
      }
      arr->msb.set_मानम्(msb);
      arr->lsb.set_मानम्(lsb);

      गणनम्_var_arrays += 1;
      गणनम्_var_array_शब्दाः += arr->get_size();

      मुक्त(label);
      delete[] name;
}

शून्य compile_var2_array(वर्ण*label, वर्ण*name, पूर्णाङ्क last, पूर्णाङ्क first,
		   पूर्णाङ्क msb, पूर्णाङ्क lsb, तार्किक signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, signed_flag);

      संरचना __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.set_मानम्(msb);
      arr->lsb.set_मानम्(lsb);
      arr->vals_width = labs(msb-lsb) + 1;

      प्रतिज्ञा(! arr->nets);
      सचे (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = नूतन vvp_darray_atom<int8_t>(arr->get_size());
      } अञ्ञथा सचे (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = नूतन vvp_darray_atom<uint8_t>(arr->get_size());
      } अञ्ञथा सचे (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = नूतन vvp_darray_atom<int16_t>(arr->get_size());
      } अञ्ञथा सचे (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = नूतन vvp_darray_atom<uint16_t>(arr->get_size());
      } अञ्ञथा सचे (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = नूतन vvp_darray_atom<int32_t>(arr->get_size());
      } अञ्ञथा सचे (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = नूतन vvp_darray_atom<uint32_t>(arr->get_size());
      } अञ्ञथा सचे (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = नूतन vvp_darray_atom<int64_t>(arr->get_size());
      } अञ्ञथा सचे (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = नूतन vvp_darray_atom<uint64_t>(arr->get_size());
      } अञ्ञथा {
	    arr->vals = नूतन vvp_darray_vec2(arr->get_size(), arr->vals_width);
      }
      गणनम्_var_arrays += 1;
      गणनम्_var_array_शब्दाः += arr->get_size();

      मुक्त(label);
      delete[] name;
}

शून्य compile_real_array(वर्ण*label, वर्ण*name, पूर्णाङ्क last, पूर्णाङ्क first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, सच्च);

      संरचना __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = नूतन vvp_darray_real(arr->get_size());
      arr->vals_width = 1;

      गणनम्_real_arrays += 1;
      गणनम्_real_array_शब्दाः += arr->get_size();

      मुक्त(label);
      delete[] name;
}

शून्य compile_string_array(वर्ण*label, वर्ण*name, पूर्णाङ्क last, पूर्णाङ्क first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, सच्च);

      संरचना __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = नूतन vvp_darray_string(arr->get_size());
      arr->vals_width = 1;

      गणनम्_real_arrays += 1;
      गणनम्_real_array_शब्दाः += arr->get_size();

      मुक्त(label);
      delete[] name;
}

शून्य compile_object_array(वर्ण*label, वर्ण*name, पूर्णाङ्क last, पूर्णाङ्क first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, सच्च);

      संरचना __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = नूतन vvp_darray_object(arr->get_size());
      arr->vals_width = 1;

      गणनम्_real_arrays += 1;
      गणनम्_real_array_शब्दाः += arr->get_size();

      मुक्त(label);
      delete[] name;
}

शून्य compile_net_array(वर्ण*label, वर्ण*name, पूर्णाङ्क last, पूर्णाङ्क first)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, first, last, मिच्छा);

      संरचना __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(calloc(arr->get_size(), परिमाण(vpiHandle)));

      गणनम्_net_arrays += 1;
      गणनम्_net_array_शब्दाः += arr->get_size();

      मुक्त(label);
      delete[] name;
}

वग्ग vvp_fun_arrayport  : सार्वजनिक vvp_net_fun_t {

    सार्वजनिक:
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport() override;

      आभास शून्य check_word_change(unsigned long addr) = 0;

    रक्षित:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      unsigned long addr_;

      friend शून्य array_attach_port(vvp_array_t, vvp_fun_arrayport*);
      friend शून्य __vpiArray::word_change(unsigned long);
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

वग्ग vvp_fun_arrayport_sa  : सार्वजनिक vvp_fun_arrayport {

    सार्वजनिक:
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_sa() override;

      शून्य check_word_change(unsigned long addr) override;

      शून्य recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;

    गुप्त:
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

शून्य vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                                     vvp_context_t)
{
      तार्किक addr_valid_flag;

      विकल्प (port.port()) {

	  पक्ष 0: // Address input
	    addr_valid_flag = vector4_to_मानम्(bit, addr_);
	    सचे (! addr_valid_flag)
		  addr_ = arr_->get_size();
	    सचे (vpi_array_is_real(arr_))
		  port.ptr()->send_real(arr_->get_word_r(addr_), 0);
	    अञ्ञथा
		  port.ptr()->send_vec4(arr_->get_word(addr_), 0);

	    भङ्ग;

	  स्वभाव:
	    fprintf(stdout, "XXXX write ports not implemented.\n");
	    प्रतिज्ञा(0);
      }
}

शून्य vvp_fun_arrayport_sa::check_word_change(unsigned long addr)
{
      सचे (addr != addr_) पटिनिवत्त;

      सचे (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr_), 0);
      } अञ्ञथा {
	    net_->send_vec4(arr_->get_word(addr_), 0);
      }
}

वग्ग vvp_fun_arrayport_aa  : सार्वजनिक vvp_fun_arrayport, सार्वजनिक automatic_hooks_s {

    सार्वजनिक:
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_aa() override;

      शून्य alloc_instance(vvp_context_t context) override;
      शून्य reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      शून्य free_instance(vvp_context_t context) override;
#endif

      शून्य check_word_change(unsigned long addr) override;

      शून्य recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t context) override;

    गुप्त:
      शून्य check_word_change_(unsigned long addr, vvp_context_t context);

      __vpiScope*context_scope_;
      unsigned context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(एतत्, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(एतत्, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

शून्य vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      unsigned long*addr = नूतन unsigned long;
      vvp_set_context_item(context, context_idx_, addr);

      *addr = addr_;
}

शून्य vvp_fun_arrayport_aa::reset_instance(vvp_context_t context)
{
      unsigned long*addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));

      *addr = addr_;
}

#ifdef CHECK_WITH_VALGRIND
शून्य vvp_fun_arrayport_aa::free_instance(vvp_context_t context)
{
      unsigned long*addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));
      delete addr;
}
#endif

शून्य vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
                                     vvp_context_t context)
{
      सचे (context) {
            unsigned long*addr = static_cast<unsigned long*>
                  (vvp_get_context_item(context, context_idx_));

            तार्किक addr_valid_flag;

            विकल्प (port.port()) {

                पक्ष 0: // Address input
                  addr_valid_flag = vector4_to_मानम्(bit, *addr);
                  सचे (! addr_valid_flag) *addr = arr_->get_size();
                  सचे (vpi_array_is_real(arr_)) {
			port.ptr()->send_real(arr_->get_word_r(*addr),
					      context);
                  } अञ्ञथा {
			port.ptr()->send_vec4(arr_->get_word(*addr),
					      context);
                  }
                  भङ्ग;

                स्वभाव:
                  fprintf(stdout, "XXXX write ports not implemented.\n");
                  प्रतिज्ञा(0);
            }
      } अञ्ञथा {
            context = context_scope_->live_contexts;
            याव (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

शून्य vvp_fun_arrayport_aa::check_word_change_(unsigned long addr,
                                              vvp_context_t context)
{
      ध्रुव unsigned long*port_addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));

      सचे (addr != *port_addr)
	    पटिनिवत्त;

      सचे (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr), context);
      } अञ्ञथा {
	    net_->send_vec4(arr_->get_word(addr), context);
      }
}

शून्य vvp_fun_arrayport_aa::check_word_change(unsigned long addr)
{
      सचे (arr_->get_scope()->is_automatic()) {
            प्रतिज्ञा(vthread_get_wt_context());
            check_word_change_(addr, vthread_get_wt_context());
      } अञ्ञथा {
            vvp_context_t context = context_scope_->live_contexts;
            याव (context) {
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
      सचे (!array->get_scope()->is_automatic() &&
          (array->vals4 || array->vals)) {
              /* propagate initial values for variable arrays */
            सचे (!vpi_array_is_real(array)) {
                  vvp_bit4_t init;
                  सचे (array->vals4)
                      init = BIT4_X;
                  अञ्ञथा
                      init = BIT4_0;
                  vvp_vector4_t tmp(array->vals_width, init);
                  schedule_init_propagate(fun->net_, tmp);
            } अञ्ञथा {
                  schedule_init_propagate(fun->net_, 0.0);
            }
      }
}

वग्ग array_word_मानम्_callback : सार्वजनिक मानम्_callback {
    सार्वजनिक:
      अन्तःस्थ explicit array_word_मानम्_callback(p_cb_data data, long addr)
      : मानम्_callback(data), word_addr(addr)
      { }

    सार्वजनिक:
      long word_addr;
};

शून्य __vpiArray::word_change(unsigned long addr)
{
      अत्थाय (vvp_fun_arrayport*cur = ports_; cur; cur = cur->next_)
	    cur->check_word_change(addr);

	// Run callbacks attached to the array itself.
      संरचना __vpiCallback *next = vpi_callbacks;
      संरचना __vpiCallback *prev = 0;

      याव (next) {
	    array_word_मानम्_callback*cur = dynamic_cast<array_word_मानम्_callback*>(next);
	    next = cur->next;

	      // Skip callbacks that are not for me. -1 is for every element.
	    सचे (cur->word_addr != (long)addr && cur->word_addr != -1) {
		  prev = cur;
		  अनुवर्त;
	    }

	      // For whole array callbacks we need to set the index.
	    सचे (cur->word_addr == -1) {
		  cur->cb_data.index = (PLI_INT32) ((पूर्णाङ्क)addr +
						    first_addr.get_मानम्());
	    }

	    सचे (cur->cb_data.cb_rtn != 0) {
		  सचे (cur->test_मानम्_callback_ready()) {
			सचे (cur->cb_data.value) {
			      सचे (vpi_array_is_real(एतत्)) {
				    द्विगुण val = 0.0;
				    सचे (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_real_get_मानम्(val, cur->cb_data.value);
			      } अञ्ञथा सचे (vals4) {
				    vpip_vec4_get_मानम्(vals4->get_word(addr),
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } अञ्ञथा सचे (dynamic_cast<vvp_darray_atom<int8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int64_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint64_t>*>(vals)
				      || dynamic_cast<vvp_darray_vec2*>(vals)) {
				    vvp_vector4_t val;
				    सचे (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_vec4_get_मानम्(val,
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } अञ्ञथा {
			            प्रतिज्ञा(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } अञ्ञथा सचे (prev == 0) {

		  vpi_callbacks = next;
		  cur->next = 0;
		  delete cur;

	    } अञ्ञथा {
		  प्रतिज्ञा(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}

वग्ग array_resolv_list_t : सार्वजनिक resolv_list_s {

    सार्वजनिक:
      explicit array_resolv_list_t(वर्ण*lab) : resolv_list_s(lab) {
	    array = 0;
      }

      vvp_array_t*array;
      तार्किक resolve(तार्किक mes) override;

    गुप्त:
};

तार्किक array_resolv_list_t::resolve(तार्किक mes)
{
      *array = array_find(label());
      सचे (*array == 0) {
	    प्रतिज्ञा(!mes);
	    पटिनिवत्त मिच्छा;
      }
      पटिनिवत्त सच्च;
}

वग्ग array_port_resolv_list_t : सार्वजनिक resolv_list_s {

    सार्वजनिक:
      explicit array_port_resolv_list_t(वर्ण* lab, तार्किक use_addr__,
                                        long addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      तार्किक use_addr;
      long addr;
      तार्किक resolve(तार्किक mes) override;

    गुप्त:
};

array_port_resolv_list_t::array_port_resolv_list_t(वर्ण *lab, तार्किक use_addr__,
                                                   long addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      सचे (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      अञ्ञथा
            context_scope = 0;
      ptr = नूतन vvp_net_t;
}

तार्किक array_port_resolv_list_t::resolve(तार्किक mes)
{
      vvp_array_t mem = array_find(label());
      सचे (mem == 0) {
	    प्रतिज्ञा(mem || !mes);
	    पटिनिवत्त मिच्छा;
      }

      vvp_fun_arrayport*fun;
      सचे (use_addr)
            सचे (context_scope)
                  fun = नूतन vvp_fun_arrayport_aa(context_scope, mem, ptr, addr);
            अञ्ञथा
                  fun = नूतन vvp_fun_arrayport_sa(mem, ptr, addr);
      अञ्ञथा
            सचे (context_scope)
                  fun = नूतन vvp_fun_arrayport_aa(context_scope, mem, ptr);
            अञ्ञथा
                  fun = नूतन vvp_fun_arrayport_sa(mem, ptr);
      ptr->fun = fun;

      array_attach_port(mem, fun);

      पटिनिवत्त सच्च;
}

वग्ग array_word_part_callback : सार्वजनिक array_word_मानम्_callback {
    सार्वजनिक:
      explicit array_word_part_callback(p_cb_data data, long addr);
      ~array_word_part_callback() override;

      array_word_part_callback(ध्रुव array_word_part_callback&) = delete;
      array_word_part_callback& operator=(ध्रुव array_word_part_callback&) = delete;

      तार्किक test_मानम्_callback_ready(शून्य) override;

    गुप्त:
      वर्ण*मानम्_bits;
};

array_word_part_callback::array_word_part_callback(p_cb_data data, long addr)
: array_word_मानम्_callback(data, addr)
{
	// Get the initial value of the part, to use as a reference.
      संरचना __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj);
      s_vpi_मानम् tmp_मानम्;
      tmp_मानम्.format = vpiBinStrVal;
      apvword->vpi_get_मानम्(&tmp_मानम्);

      मानम्_bits = नूतन वर्ण[apvword->part_wid+1];

      memcpy(मानम्_bits, tmp_मानम्.value.str, apvword->part_wid);
      मानम्_bits[apvword->part_wid] = 0;
}

array_word_part_callback::~array_word_part_callback()
{
      delete[]मानम्_bits;
}

तार्किक array_word_part_callback::test_मानम्_callback_ready(शून्य)
{
      संरचना __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_data.obj);
      प्रतिज्ञा(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_मानम् tmp_मानम्;
      tmp_मानम्.format = vpiBinStrVal;
      apvword->vpi_get_मानम्(&tmp_मानम्);

      सचे (memcmp(मानम्_bits, tmp_मानम्.value.str, apvword->part_wid) == 0)
	    पटिनिवत्त मिच्छा;

      memcpy(मानम्_bits, tmp_मानम्.value.str, apvword->part_wid);
      पटिनिवत्त सच्च;

}

मानम्_callback*vpip_array_word_change(p_cb_data data)
{
      संरचना __vpiArray*parent = 0;
      array_word_मानम्_callback*cbh = 0;
      सचे (ध्रुव संरचना __vpiArrayWord*word = array_var_word_from_handle(data->obj)) {
	    parent = static_cast<__vpiArray*>(word->get_parent());
	    unsigned addr = word->get_index();
	    cbh = नूतन array_word_मानम्_callback(data, addr);

      } अञ्ञथा सचे (संरचना __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(data->obj)) {
	    parent = tword->array;
	    cbh = नूतन array_word_मानम्_callback(data, tword->address);

      } अञ्ञथा सचे (संरचना __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj)) {
	    parent = apvword->array;
	    cbh = नूतन array_word_part_callback(data, apvword->word_sel);
      }

      प्रतिज्ञा(cbh);
      प्रतिज्ञा(parent);
      cbh->next = parent->vpi_callbacks;
      parent->vpi_callbacks = cbh;

      पटिनिवत्त cbh;
}

मानम्_callback* vpip_array_change(p_cb_data data)
{
      array_word_मानम्_callback*cbh = नूतन array_word_मानम्_callback(data, -1);
      प्रतिज्ञा(data->obj);

      संरचना __vpiArray*arr = dynamic_cast<__vpiArray*>(data->obj);
      cbh->next = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      पटिनिवत्त cbh;
}

शून्य compile_array_port(वर्ण*label, वर्ण*array, वर्ण*addr)
{
      array_port_resolv_list_t*resolv_mem
	    = नूतन array_port_resolv_list_t(array, मिच्छा, 0);

      define_functor_चिह्नम्(label, resolv_mem->ptr);
      मुक्त(label);
	// Connect the port-0 input as the address.
      निवेशः_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

शून्य compile_array_port(वर्ण*label, वर्ण*array, long addr)
{
      array_port_resolv_list_t*resolv_mem
	    = नूतन array_port_resolv_list_t(array, सच्च, addr);

      define_functor_चिह्नम्(label, resolv_mem->ptr);
      मुक्त(label);

      resolv_submit(resolv_mem);
}

शून्य compile_array_alias(वर्ण*label, वर्ण*name, वर्ण*src)
{
      vvp_array_t mem = array_find(src);
      प्रतिज्ञा(mem);

      संरचना __vpiArray*obj = नूतन __vpiArray;

      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_गणनम् = mem->array_गणनम्;
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
      obj->vals_शब्दाः = mem->vals_शब्दाः;

      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

      प्रतिज्ञा(array_table);
      प्रतिज्ञा(!array_find(label));
      array_table->sym_set_मानम्(label, obj);

      compile_vpi_चिह्नम्(label, obj);
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
vpiHandle vpip_make_vthr_A(वर्ण*label, unsigned addr)
{
      संरचना __vpiArrayVthrA*obj = नूतन __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = नूतन array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      obj->address = addr;

      पटिनिवत्त obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(वर्ण*label, वर्ण*चिह्नम्)
{
      संरचना __vpiArrayVthrA*obj = नूतन __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = नूतन array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      compile_vpi_lookup(&obj->address_handle, चिह्नम्);
      obj->address = 0;

      पटिनिवत्त obj;
}

vpiHandle vpip_make_vthr_A(वर्ण*label, vpiHandle handle)
{
      संरचना __vpiArrayVthrA*obj = नूतन __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = नूतन array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = handle;
      obj->address = 0;

      पटिनिवत्त obj;
}

vpiHandle vpip_make_vthr_APV(वर्ण*label, unsigned index, unsigned bit, unsigned wid)
{
      संरचना __vpiArrayVthrAPV*obj = नूतन __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = नूतन array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->word_sel = index;
      obj->part_bit = bit;
      obj->part_wid = wid;

      पटिनिवत्त obj;
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
      सचे (arr->vals_शब्दाः) delete [] (arr->vals_शब्दाः-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals;

      सचे (arr->nets) {
	    अत्थाय (unsigned idx = 0; idx < arr->get_size(); idx += 1) {
		  सचे (संरचना __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.index);
		    /* These should only be the real words. */
		  } अञ्ञथा {
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

      याव (arr->vpi_callbacks) {
	    संरचना __vpiCallback*tmp = arr->vpi_callbacks->next;
	    delete arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      delete arr;
}

शून्य A_delete(vpiHandle item)
{
      संरचना __vpiArrayVthrA*obj = static_cast<संरचना __vpiArrayVthrA*>(item);
      सचे (obj->address_handle) {
	    विकल्प (obj->address_handle->get_type_code()) {
		पक्ष vpiMemoryWord:
		  सचे (vpi_get(_vpiFromThr, obj->address_handle) == _vpi_at_A) {
			A_delete(obj->address_handle);
		  }
		  भङ्ग;
		पक्ष vpiPartSelect:
		  प्रतिज्ञा(vpi_get(_vpiFromThr, obj->address_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->address_handle);
		  भङ्ग;
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
