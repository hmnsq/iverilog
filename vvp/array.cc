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
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
#अन्तर्भाव  "vvp_cleanup.h"
#यदि_अन्तः
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <iostream>
# include  "compile.h"
# include  <cassert>
# include  "ivl_alloc.h"

उपयुज्य नामाकाश std;

अचिह्नित दीर्घ गणनम्_net_arrays = 0;
अचिह्नित दीर्घ गणनम्_net_array_शब्दाः = 0;
अचिह्नित दीर्घ गणनम्_var_arrays = 0;
अचिह्नित दीर्घ गणनम्_var_array_शब्दाः = 0;
अचिह्नित दीर्घ गणनम्_real_arrays = 0;
अचिह्नित दीर्घ गणनम्_real_array_शब्दाः = 0;

स्थिर चिह्नम्_map_s<संरचना __vpiArray>* array_सारणी =0;

वर्ग vvp_fun_arrayport;
स्थिर शून्य array_attach_पत्तनम्(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_find(ध्रुव वर्ण*label)
{
      यदि (array_सारणी == 0)
	    प्रत्यागम 0;

      vvp_array_t v = array_सारणी->sym_get_मानम्(label);
      प्रत्यागम v;
}

संरचना __vpiArrayVthrA : सार्वजनिक __vpiHandle {
      पूर्णाङ्क get_type_संहिता(शून्य) ध्रुव override { प्रत्यागम vpi_स्मृतिः_Word; }
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण* vpi_get_str(पूर्णाङ्क code) override;
      शून्य vpi_get_मानम्(p_vpi_मानम् val) override;
      vpiHandle vpi_put_मानम्(p_vpi_मानम् val, पूर्णाङ्क flags) override;
      vpiHandle vpi_handle(पूर्णाङ्क code) override;

      संरचना __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle स्थानाङ्कः_handle;
	// If wid==0, then address is the address into the array.
      अचिह्नित स्थानाङ्कः;

      अचिह्नित get_स्थानाङ्कः() ध्रुव
      {
	    यदि (स्थानाङ्कः_handle) {
		  s_vpi_मानम् vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpi_सदिशम्_Val;
		  स्थानाङ्कः_handle->vpi_get_मानम्(&vp);
		  पूर्णाङ्क शब्दाः = (स्थानाङ्कः_handle->vpi_get(vpi_आकार)-1)/32 + 1;
		  प्रति(पूर्णाङ्क idx = 0; idx < शब्दाः; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			यदि (vp.value.vector[idx].bval != 0) प्रत्यागम UINT_MAX;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  स्थानाङ्कः_handle->vpi_get_मानम्(&vp);
		  प्रत्यागम vp.value.integer;
	    }

	    प्रत्यागम स्थानाङ्कः;
      }
};


संरचना __vpiArrayVthrAPV : सार्वजनिक __vpiHandle {
      पूर्णाङ्क get_type_संहिता(शून्य) ध्रुव override { प्रत्यागम vpiPartSelect; }
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण* vpi_get_str(पूर्णाङ्क code) override;
      शून्य vpi_get_मानम्(p_vpi_मानम् val) override;
      vpiHandle vpi_handle(पूर्णाङ्क code) override;

      संरचना __vpiArray*array;
      अचिह्नित word_sel;
      अचिह्नित part_bit;
      अचिह्नित part_wid;
};

तार्किक is_net_array(vpiHandle obj)
{
      संरचना __vpiArray*rfp = dynamic_cast<__vpiArray*> (obj);
      प्रतिज्ञा(rfp);

      यदि (rfp->nets != 0) प्रत्यागम सत्य;
      प्रत्यागम असत्य;
}

// This function return true if the underlying array words are real.
स्थिर तार्किक vpi_array_is_real(ध्रुव vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      यदि (arr->vals4 != 0)  // A bit based variable/register array.
	    प्रत्यागम असत्य;

      यदि (dynamic_cast<vvp_darray_real*> (arr->vals))
	    प्रत्यागम सत्य;

      यदि (arr->vals != 0)
	    प्रत्यागम असत्य;

	// This must be a net array so look at element 0 to find the type.
      प्रतिज्ञा(arr->nets != 0);
      प्रतिज्ञा(arr->get_आकार() > 0);
      ध्रुव संरचना __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      यदि (rsig) {
	    प्रत्यागम सत्य;
      }

      प्रत्यागम असत्य;
}

स्थिर तार्किक vpi_array_is_string(ध्रुव vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      यदि (arr->vals4 != 0)  // A bit based variable/register array.
	    प्रत्यागम असत्य;

      यदि (dynamic_cast<vvp_darray_string*> (arr->vals))
	    प्रत्यागम सत्य;

      प्रत्यागम असत्य;
}

पूर्णाङ्क __vpiArray::get_word_आकार() ध्रुव
{
      अचिह्नित width;

      प्रतिज्ञा(get_आकार() > 0);
	/* For a net array we need to get the width from the first element. */
      यदि (nets) {
	    प्रतिज्ञा(vals4 == 0 && vals == 0);
	    ध्रुव संरचना __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    प्रतिज्ञा(vsig);
	    width = vpip_आकार(vsig);
	/* For a variable array we can get the width from vals_width. */
      } अन्यथा {
	    प्रतिज्ञा(vals4 || vals);
	    width = vals_विस्तारः;
      }

      प्रत्यागम width;
}

वर्ण*__vpiArray::get_word_str(संरचना __vpiArrayWord*word, पूर्णाङ्क code)
{
      अचिह्नित सूचकांक = word->get_सूचकांक();

      यदि (code == vpiFile) {  // Not implemented for now!
	    प्रत्यागम simple_समुच्चयः_rbuf_str(file_नामानि[0]);
      }

      वर्ण sidx [64];
      snprintf(sidx, 63, "%d", (पूर्णाङ्क)सूचकांक + first_addr.get_मानम्());
      प्रत्यागम generic_get_str(code, get_scope(), name, sidx);
}

शून्य __vpiArray::get_word_मानम्(संरचना __vpiArrayWord*word, p_vpi_मानम् vp)
{
      अचिह्नित सूचकांक = word->get_सूचकांक();

    // Determine the appropriate format (The Verilog PLI Handbook 5.2.10)
      यदि(vp->format == vpiObjTypeVal) {
          यदि(vpi_array_is_real(एतत्))
              vp->format = vpiRealVal;
          अन्यथा यदि(vpi_array_is_string(एतत्))
              vp->format = vpiStringVal;
          अन्यथा
              vp->format = vpiIntVal;
      }

      यदि(vals4) {
          vpip_vec4_get_मानम्(vals4->get_word(सूचकांक),
                              vals4->width(), signed_flag, vp);
      } अन्यथा यदि(vals) {
          विकल्प(vp->format) {
            पक्ष vpiBinStrVal:
            पक्ष vpiOctStrVal:
            पक्ष vpiDecStrVal:
            पक्ष vpiHexStrVal:
            पक्ष vpiScalarVal:
            पक्ष vpiIntVal:
            पक्ष vpi_सदिशम्_Val:
            {
                vvp_vector4_t v;
                vals->get_word(सूचकांक, v);
                vpip_vec4_get_मानम्(v, vals_विस्तारः, signed_flag, vp);
            }
            भङ्ग;

            पक्ष vpiRealVal:
            {
                द्विगुण d;
                vals->get_word(सूचकांक, d);
                vpip_real_get_मानम्(d, vp);
            }
            भङ्ग;

            पक्ष vpiStringVal:
            {
                string s;
                vals->get_word(सूचकांक, s);
                vpip_string_get_मानम्(s, vp);
            }
            भङ्ग;

            स्वभाव:
                fprintf(stderr, "vpi sorry: format is not implemented\n");
                प्रतिज्ञा(असत्य);
          }
      }
}

शून्य __vpiArray::put_word_मानम्(संरचना __vpiArrayWord*word, p_vpi_मानम् vp, पूर्णाङ्क)
{
      अचिह्नित सूचकांक = word->get_सूचकांक();
      vvp_vector4_t val = vec4_from_vpi_मानम्(vp, vals_विस्तारः);
      समुच्चयः_word(सूचकांक, 0, val);
}

vpiHandle __vpiArray::get_iter_सूचकांक(संरचना __vpiArrayIterator*, पूर्णाङ्क idx)
{
      यदि (nets) प्रत्यागम nets[idx];

      प्रतिज्ञा(vals4 || vals);

      यदि (vals_शब्दाः == 0) make_vals_शब्दाः();

      प्रत्यागम &(vals_शब्दाः[idx].as_word);
}

पूर्णाङ्क __vpiArray::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpi_पङ्क्तिः_No:
	    प्रत्यागम 0; // Not implemented for now!

	  पक्ष vpi_आकार:
	    प्रत्यागम get_आकार();

	  पक्ष vpiAutomatic:
	    प्रत्यागम scope->is_automatic()? 1 : 0;

	  स्वभाव:
	    प्रत्यागम 0;
      }
}

वर्ण* __vpiArray::vpi_get_str(पूर्णाङ्क code)
{
      यदि (code == vpiFile) {  // Not implemented for now!
            प्रत्यागम simple_समुच्चयः_rbuf_str(file_नामानि[0]);
      }

      प्रत्यागम generic_get_str(code, scope, name, रिक्त);
}

vpiHandle __vpiArray::vpi_handle(पूर्णाङ्क code)
{
      विकल्प (code) {
          पक्ष vpi_वामः_Range:
            यदि (swap_addr) प्रत्यागम &चरमः_addr;
            अन्यथा प्रत्यागम &first_addr;

          पक्ष vpi_दक्षिणः_Range:
            यदि (swap_addr) प्रत्यागम &first_addr;
            अन्यथा प्रत्यागम &चरमः_addr;

	  पक्ष vpiScope:
	    प्रत्यागम scope;

	  पक्ष vpiModule:
	    प्रत्यागम vpip_module(scope);
      }

      प्रत्यागम 0;
}

/*
* VPI code passes indices that are not yet converted to canonical
* form, so this index function does it here.
*/
vpiHandle __vpiArray::vpi_सूचकांक(पूर्णाङ्क सूचकांक)
{
      सूचकांक -= first_addr.get_मानम्();
      यदि (सूचकांक >= (दीर्घ) get_आकार())
	    प्रत्यागम 0;
      यदि (सूचकांक < 0)
	    प्रत्यागम 0;

      यदि (nets != 0) {
	    प्रत्यागम nets[सूचकांक];
      }

      यदि (vals_शब्दाः == 0)
	    make_vals_शब्दाः();

      प्रत्यागम &(vals_शब्दाः[सूचकांक].as_word);
}

पूर्णाङ्क __vpiArrayWord::as_word_t::vpi_get(पूर्णाङ्क code)
{
      संरचना __vpiArrayWord*obj = array_var_word_from_handle(एतत्);
      प्रतिज्ञा(obj);
      संरचना __vpiArrayBase*my_parent = obj->get_parent();
      t_vpi_मानम् val;

      विकल्प (code) {
	  पक्ष vpi_पङ्क्तिः_No:
	    प्रत्यागम 0; // Not implemented for now!

	  पक्ष vpi_आकार:
	    प्रत्यागम my_parent->get_word_आकार();

	  पक्ष vpi_वामः_Range:
            val.format = vpiIntVal;
	    my_parent->get_वामः_range()->vpi_get_मानम्(&val);
            प्रतिज्ञा(val.format == vpiIntVal);
            प्रत्यागम val.value.integer;

	  पक्ष vpi_दक्षिणः_Range:
            val.format = vpiIntVal;
	    my_parent->get_दक्षिणः_range()->vpi_get_मानम्(&val);
            प्रतिज्ञा(val.format == vpiIntVal);
            प्रत्यागम val.value.integer;

	  पक्ष vpi_सूचकांक_2:
	    {
		  पूर्णाङ्क आधारः_विस्थापनम् = 0;
		  संरचना __vpiArray*आधारः = dynamic_cast<__vpiArray*> (my_parent);
		  यदि (आधारः) {
			val.format = vpiIntVal;
			आधारः->first_addr.vpi_get_मानम्(&val);
			आधारः_विस्थापनम् += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_सूचकांक.vpi_get_मानम्(&val);
		  प्रतिज्ञा(val.format == vpiIntVal);
		  प्रत्यागम val.value.integer + आधारः_विस्थापनम्;
	    }

	  पक्ष vpiAutomatic:
	    प्रत्यागम my_parent->get_scope()->is_automatic()? 1 : 0;

#यदि defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  पक्ष _vpiFromThr:
	    प्रत्यागम _vpiNoThr;
#यदि_अन्तः

	  स्वभाव:
	    प्रत्यागम 0;
      }
}


पूर्णाङ्क __vpiArrayVthrA::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpi_पङ्क्तिः_No:
	    प्रत्यागम 0; // Not implemented for now!

	  पक्ष vpi_आकार:
	    प्रत्यागम array->get_word_आकार();

	  पक्ष vpi_वामः_Range:
	    प्रत्यागम array->msb.get_मानम्();

	  पक्ष vpi_दक्षिणः_Range:
	    प्रत्यागम array->lsb.get_मानम्();

	  पक्ष vpi_सूचकांक_2:
	    प्रत्यागम (पूर्णाङ्क)get_स्थानाङ्कः() + array->first_addr.get_मानम्();

	  पक्ष vpiAutomatic:
	    प्रत्यागम array->get_scope()->is_automatic() ? 1 : 0;

#यदि defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  पक्ष _vpiFromThr:
	    प्रत्यागम _vpi_at_A;
#यदि_अन्तः

	  // If address_handle is not zero we definitely have a
	  // variable.
	  पक्ष vpiConstantSelect:
	    प्रत्यागम स्थानाङ्कः_handle == 0;

	  स्वभाव:
	    प्रत्यागम 0;
      }
}

वर्ण* __vpiArrayVthrA::vpi_get_str(पूर्णाङ्क code)
{
      यदि (code == vpiFile) {  // Not implemented for now!
            प्रत्यागम simple_समुच्चयः_rbuf_str(file_नामानि[0]);
      }

      वर्ण sidx [64];
      snprintf(sidx, 63, "%d", (पूर्णाङ्क)get_स्थानाङ्कः() + array->first_addr.get_मानम्());
      प्रत्यागम generic_get_str(code, array->get_scope(), array->name, sidx);
}

शून्य __vpiArrayVthrA::vpi_get_मानम्(p_vpi_मानम् vp)
{
      प्रतिज्ञा(array);

      अचिह्नित सूचकांक = get_स्थानाङ्कः();
      यदि (vpi_array_is_real(array)) {
	    द्विगुण tmp = array->get_word_r(सूचकांक);
	    vpip_real_get_मानम्(tmp, vp);
      } अन्यथा यदि (vpi_array_is_string(array)) {
	    string tmp = array->get_word_str(सूचकांक);
	    vpip_string_get_मानम्(tmp, vp);
      } अन्यथा {
	    vvp_vector4_t tmp = array->get_word(सूचकांक);
	    अचिह्नित width = array->get_word_आकार();
	    vpip_vec4_get_मानम्(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_मानम्(p_vpi_मानम् vp, पूर्णाङ्क)
{
      अचिह्नित सूचकांक = get_स्थानाङ्कः();

      प्रतिज्ञा(array);
      प्रतिज्ञा(सूचकांक < array->get_आकार());

      यदि (vpi_array_is_real(array)) {
	    द्विगुण val = real_from_vpi_मानम्(vp);
	    array->समुच्चयः_word(सूचकांक, val);
      } अन्यथा {
	    अचिह्नित width = array->get_word_आकार();
	    vvp_vector4_t val = vec4_from_vpi_मानम्(vp, width);
	    array->समुच्चयः_word(सूचकांक, 0, val);
      }

      प्रत्यागम एतत्;
}

vpiHandle __vpiArrayVthrA::vpi_handle(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpi_सूचकांक_2:
	    भङ्ग;  // Not implemented!

	  पक्ष vpi_वामः_Range:
	    प्रत्यागम &array->msb;

	  पक्ष vpi_दक्षिणः_Range:
	    प्रत्यागम &array->lsb;

	  पक्ष vpiParent:
	  पक्ष vpiArray:
	    प्रत्यागम array;

	  पक्ष vpiScope:
	    प्रत्यागम array->get_scope();

	  पक्ष vpiModule:
	    प्रत्यागम vpip_module(array->get_scope());
      }

      प्रत्यागम 0;
}


पूर्णाङ्क __vpiArrayVthrAPV::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpi_पङ्क्तिः_No:
	    प्रत्यागम 0; // Not implemented for now!

	  पक्ष vpi_आकार:
	    प्रत्यागम part_wid;

	  पक्ष vpi_वामः_Range:
	    प्रत्यागम part_bit + part_wid - 1;

	  पक्ष vpi_दक्षिणः_Range:
	    प्रत्यागम part_bit;

	  पक्ष vpiAutomatic:
	    प्रत्यागम array->get_scope()->is_automatic() ? 1 : 0;

#यदि defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  पक्ष _vpiFromThr:
	    प्रत्यागम _vpi_at_APV;
#यदि_अन्तः

	  पक्ष vpiConstantSelect:
	    प्रत्यागम 1;

	  स्वभाव:
	    प्रत्यागम 0;
      }
}

वर्ण* __vpiArrayVthrAPV::vpi_get_str(पूर्णाङ्क code)
{
      यदि (code == vpiFile) {  // Not implemented for now!
            प्रत्यागम simple_समुच्चयः_rbuf_str(file_नामानि[0]);
      }

      वर्ण sidx [64];
      snprintf(sidx, 63, "%u", word_sel + array->first_addr.get_मानम्());
      प्रत्यागम generic_get_str(code, array->get_scope(), array->name, sidx);
}

शून्य __vpiArrayVthrAPV::vpi_get_मानम्(p_vpi_मानम् vp)
{
      प्रतिज्ञा(array);

      अचिह्नित सूचकांक = word_sel;
      यदि (vpi_array_is_real(array)) {
	    द्विगुण tmp = array->get_word_r(सूचकांक);
	    vpip_real_get_मानम्(tmp, vp);
      } अन्यथा {
	    vvp_vector4_t tmp = array->get_word(सूचकांक);
	    tmp = tmp.subvalue(part_bit, part_wid);
	    vpip_vec4_get_मानम्(tmp, part_wid, असत्य, vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(पूर्णाङ्क code)
{
      विकल्प (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  पक्ष vpiParent:
	    प्रत्यागम 0;

            // Not part of the Verilog standard. We use this internally.
	  पक्ष vpiArray:
	    प्रत्यागम array;

	  पक्ष vpiScope:
	    प्रत्यागम array->get_scope();

	  पक्ष vpiModule:
	    प्रत्यागम vpip_module(array->get_scope());
      }

      प्रत्यागम 0;
}

शून्य __vpiArray::समुच्चयः_word(अचिह्नित स्थानाङ्कः, अचिह्नित part_off, ध्रुव vvp_vector4_t&val)
{
      यदि (स्थानाङ्कः >= get_आकार())
	    प्रत्यागम;

      यदि (vals4) {
	    प्रतिज्ञा(nets == 0);
	    यदि (part_off != 0 || val.size() != vals_विस्तारः) {
		  vvp_vector4_t tmp = vals4->get_word(स्थानाङ्कः);
		  यदि ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_विस्तारः << पङ्क्त्यन्तः;
			प्रतिज्ञा(0);
		  }
		  tmp.समुच्चयः_vec(part_off, val);
		  vals4->समुच्चयः_word(स्थानाङ्कः, tmp);
	    } अन्यथा {
		  vals4->समुच्चयः_word(स्थानाङ्कः, val);
	    }
	    word_change(स्थानाङ्कः);
	    प्रत्यागम;
      }

      यदि (vals) {
	    प्रतिज्ञा(nets == 0);
	    यदि (part_off != 0 || val.size() != vals_विस्तारः) {
		  vvp_vector4_t tmp;
		  vals->get_word(स्थानाङ्कः, tmp);
		  यदि ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_विस्तारः << पङ्क्त्यन्तः;
			प्रतिज्ञा(0);
		  }
		  tmp.समुच्चयः_vec(part_off, val);
		  vals->समुच्चयः_word(स्थानाङ्कः, tmp);
	    } अन्यथा {
		  vals->समुच्चयः_word(स्थानाङ्कः, val);
	    }
	    word_change(स्थानाङ्कः);
	    प्रत्यागम;
      }

      प्रतिज्ञा(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle word = nets[स्थानाङ्कः];
      ध्रुव संरचना __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      प्रतिज्ञा(vsig);

      vsig->ग्रन्थिः->send_vec4_pv(val, part_off, vpip_आकार(vsig), 0);
      word_change(स्थानाङ्कः);
}

शून्य __vpiArray::समुच्चयः_word(अचिह्नित स्थानाङ्कः, द्विगुण val)
{
      प्रतिज्ञा(vals != 0);
      प्रतिज्ञा(nets == 0);

      यदि (स्थानाङ्कः >= vals->get_आकार())
	    प्रत्यागम;

      vals->समुच्चयः_word(स्थानाङ्कः, val);
      word_change(स्थानाङ्कः);
}

शून्य __vpiArray::समुच्चयः_word(अचिह्नित स्थानाङ्कः, ध्रुव string&val)
{
      प्रतिज्ञा(vals != 0);
      प्रतिज्ञा(nets == 0);

      यदि (स्थानाङ्कः >= vals->get_आकार())
	    प्रत्यागम;

      vals->समुच्चयः_word(स्थानाङ्कः, val);
      word_change(स्थानाङ्कः);
}

शून्य __vpiArray::समुच्चयः_word(अचिह्नित स्थानाङ्कः, ध्रुव vvp_object_t&val)
{
      प्रतिज्ञा(vals != 0);
      प्रतिज्ञा(nets == 0);

      यदि (स्थानाङ्कः >= vals->get_आकार())
	    प्रत्यागम;

      vals->समुच्चयः_word(स्थानाङ्कः, val);
      word_change(स्थानाङ्कः);
}

vvp_vector4_t __vpiArray::get_word(अचिह्नित स्थानाङ्कः)
{
      यदि (vals4) {
	    प्रतिज्ञा(nets == 0);
	    प्रतिज्ञा(vals == 0);
	    प्रत्यागम vals4->get_word(स्थानाङ्कः);
      }

      यदि (vals) {
	    प्रतिज्ञा(nets == 0);
	    प्रतिज्ञा(vals4== 0);
	    यदि (स्थानाङ्कः >= vals->get_आकार())
		  प्रत्यागम vvp_vector4_t(vals_विस्तारः, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_word(स्थानाङ्कः, val);
	    प्रत्यागम val;
      }

      प्रतिज्ञा(vals4 == 0);
      प्रतिज्ञा(vals == 0);
      प्रतिज्ञा(nets != 0);

      यदि (स्थानाङ्कः >= get_आकार()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    प्रतिज्ञा(get_आकार() > 0);
	    vpiHandle word = nets[0];
	    प्रतिज्ञा(word);
	    संरचना __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
	    प्रतिज्ञा(vsig);
	    vvp_signal_मानम्*sig = dynamic_cast<vvp_signal_मानम्*> (vsig->ग्रन्थिः->fil);
	    प्रतिज्ञा(sig);
	    प्रत्यागम vvp_vector4_t(sig->मानम्_आकार(), BIT4_X);
      }

      vpiHandle word = nets[स्थानाङ्कः];
      संरचना __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      प्रतिज्ञा(vsig);
      vvp_signal_मानम्*sig = dynamic_cast<vvp_signal_मानम्*> (vsig->ग्रन्थिः->fil);
      प्रतिज्ञा(sig);

      vvp_vector4_t val;
      sig->vec4_मानम्(val);
      प्रत्यागम val;
}

द्विगुण __vpiArray::get_word_r(अचिह्नित स्थानाङ्कः)
{
      यदि (vals) {
	    प्रतिज्ञा(vals4 == 0);
	    प्रतिज्ञा(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    यदि (स्थानाङ्कः >= vals->get_आकार())
		  प्रत्यागम 0.0;

	    द्विगुण val;
	    vals->get_word(स्थानाङ्कः, val);
	    प्रत्यागम val;
      }

      यदि (स्थानाङ्कः >= get_आकार())
	    प्रत्यागम 0.0;

      प्रतिज्ञा(nets);
      vpiHandle word = nets[स्थानाङ्कः];
      संरचना __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(word);
      प्रतिज्ञा(vsig);
      vvp_signal_मानम्*sig = dynamic_cast<vvp_signal_मानम्*> (vsig->net->fil);
      प्रतिज्ञा(sig);

      द्विगुण val = sig->real_मानम्();
      प्रत्यागम val;
}

शून्य __vpiArray::get_word_obj(अचिह्नित स्थानाङ्कः, vvp_object_t&val)
{
      यदि (vals) {
	    प्रतिज्ञा(vals4 == 0);
	    प्रतिज्ञा(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    यदि (स्थानाङ्कः >= vals->get_आकार()) {
		  val = vvp_object_t();
		  प्रत्यागम;
	    }

	    vals->get_word(स्थानाङ्कः, val);
	    प्रत्यागम;
      }

      प्रतिज्ञा(nets);
	// Arrays of string nets not implemented!
      प्रतिज्ञा(0);
      प्रत्यागम;
}

string __vpiArray::get_word_str(अचिह्नित स्थानाङ्कः)
{
      यदि (vals) {
	    प्रतिज्ञा(vals4 == 0);
	    प्रतिज्ञा(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    यदि (स्थानाङ्कः >= vals->get_आकार())
		  प्रत्यागम "";

	    string val;
	    vals->get_word(स्थानाङ्कः, val);
	    प्रत्यागम val;
      }

      प्रतिज्ञा(nets);
	// Arrays of string nets not implemented!
      प्रतिज्ञा(0);
      प्रत्यागम "";
}

vpiHandle vpip_make_array(ध्रुव वर्ण*label, ध्रुव वर्ण*name,
                          पूर्णाङ्क first_addr, पूर्णाङ्क चरमः_addr,
                          तार्किक signed_flag)
{
      संरचना __vpiArray*obj = नूतन __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      यदि (चरमः_addr >= first_addr) {
	    obj->swap_addr = असत्य;
      } अन्यथा {
	    obj->swap_addr = सत्य;
	    पूर्णाङ्क tmp = चरमः_addr;
	    चरमः_addr = first_addr;
	    first_addr = tmp;
      }
      प्रतिज्ञा(चरमः_addr >= first_addr);
      अचिह्नित array_गणनम् = चरमः_addr+1-first_addr;

	// For now, treat all arrays as memories. This is not quite
	// correct, as arrays are arrays with memories a special case.
      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_गणनम् = array_गणनम्;

      obj->first_addr.समुच्चयः_मानम्(first_addr);
      obj->चरमः_addr .समुच्चयः_मानम्(चरमः_addr);

	// Start off now knowing if we are nets or variables.
      obj->nets = 0;
      obj->vals4 = 0;
      obj->vals  = 0;
      obj->vals_विस्तारः = 0;
      obj->vals_शब्दाः = 0;

	// Initialize (clear) the read-ports list.
      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

	/* Add this symbol to the array_symbols table for later lookup. */
      यदि (!array_सारणी)
	    array_सारणी = नूतन चिह्नम्_map_s<संरचना __vpiArray>;

      प्रतिज्ञा(!array_find(label));
      array_सारणी->sym_समुच्चयः_मानम्(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_चिह्नम्(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_गन्तव्यम्_current_scope(obj);

      प्रत्यागम obj;
}

शून्य __vpiArray::alias_word(अचिह्नित दीर्घ addr, vpiHandle word, पूर्णाङ्क msb_, पूर्णाङ्क lsb_)
{
      प्रतिज्ञा(msb.get_मानम्() == msb_);
      प्रतिज्ञा(lsb.get_मानम्() == lsb_);
      प्रतिज्ञा(addr < get_आकार());
      प्रतिज्ञा(nets);
      nets[addr] = word;
}

शून्य __vpiArray::attach_word(अचिह्नित addr, vpiHandle word)
{
      प्रतिज्ञा(addr < get_आकार());
      प्रतिज्ञा(nets);
      nets[addr] = word;

      यदि (संरचना __vpiSignal*sig = dynamic_cast<__vpiSignal*>(word)) {
	    vvp_net_t*net = sig->ग्रन्थिः;
	    प्रतिज्ञा(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    प्रतिज्ञा(fun);
	    fun->attach_as_word(एतत्, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = एतत्;
	    sig->id.सूचकांक = नूतन __vpiDecConst(addr + first_addr.get_मानम्());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    प्रत्यागम;
      }

      यदि (संरचना __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(word)) {
	    vvp_net_t*net = sig->net;
	    प्रतिज्ञा(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    प्रतिज्ञा(fun);
	    fun->attach_as_word(एतत्, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = एतत्;
	    sig->id.सूचकांक = नूतन __vpiDecConst(addr + first_addr.get_मानम्());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = सत्य;
	    प्रत्यागम;
      }
}

शून्य compile_var_array(वर्ण*label, वर्ण*name, पूर्णाङ्क चरमः, पूर्णाङ्क first,
		   पूर्णाङ्क msb, पूर्णाङ्क lsb, वर्ण signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, चरमः,
                                      signed_flag != 0);

      संरचना __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_विस्तारः = labs(msb-lsb) + 1;
      यदि (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = नूतन vvp_vector4array_aa(arr->vals_विस्तारः,
						 arr->get_आकार());
      } अन्यथा {
            arr->vals4 = नूतन vvp_vector4array_sa(arr->vals_विस्तारः,
						 arr->get_आकार());
      }
      arr->msb.समुच्चयः_मानम्(msb);
      arr->lsb.समुच्चयः_मानम्(lsb);

      गणनम्_var_arrays += 1;
      गणनम्_var_array_शब्दाः += arr->get_आकार();

      मुक्त(label);
      विलोपय[] name;
}

शून्य compile_var2_array(वर्ण*label, वर्ण*name, पूर्णाङ्क चरमः, पूर्णाङ्क first,
		   पूर्णाङ्क msb, पूर्णाङ्क lsb, तार्किक signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, चरमः, signed_flag);

      संरचना __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.समुच्चयः_मानम्(msb);
      arr->lsb.समुच्चयः_मानम्(lsb);
      arr->vals_विस्तारः = labs(msb-lsb) + 1;

      प्रतिज्ञा(! arr->nets);
      यदि (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = नूतन vvp_darray_atom<int8_t>(arr->get_आकार());
      } अन्यथा यदि (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = नूतन vvp_darray_atom<uint8_t>(arr->get_आकार());
      } अन्यथा यदि (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = नूतन vvp_darray_atom<int16_t>(arr->get_आकार());
      } अन्यथा यदि (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = नूतन vvp_darray_atom<uint16_t>(arr->get_आकार());
      } अन्यथा यदि (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = नूतन vvp_darray_atom<int32_t>(arr->get_आकार());
      } अन्यथा यदि (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = नूतन vvp_darray_atom<uint32_t>(arr->get_आकार());
      } अन्यथा यदि (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = नूतन vvp_darray_atom<int64_t>(arr->get_आकार());
      } अन्यथा यदि (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = नूतन vvp_darray_atom<uint64_t>(arr->get_आकार());
      } अन्यथा {
	    arr->vals = नूतन vvp_darray_vec2(arr->get_आकार(), arr->vals_विस्तारः);
      }
      गणनम्_var_arrays += 1;
      गणनम्_var_array_शब्दाः += arr->get_आकार();

      मुक्त(label);
      विलोपय[] name;
}

शून्य compile_real_array(वर्ण*label, वर्ण*name, पूर्णाङ्क चरमः, पूर्णाङ्क first)
{
      vpiHandle obj = vpip_make_array(label, name, first, चरमः, सत्य);

      संरचना __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = नूतन vvp_darray_real(arr->get_आकार());
      arr->vals_विस्तारः = 1;

      गणनम्_real_arrays += 1;
      गणनम्_real_array_शब्दाः += arr->get_आकार();

      मुक्त(label);
      विलोपय[] name;
}

शून्य compile_string_array(वर्ण*label, वर्ण*name, पूर्णाङ्क चरमः, पूर्णाङ्क first)
{
      vpiHandle obj = vpip_make_array(label, name, first, चरमः, सत्य);

      संरचना __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = नूतन vvp_darray_string(arr->get_आकार());
      arr->vals_विस्तारः = 1;

      गणनम्_real_arrays += 1;
      गणनम्_real_array_शब्दाः += arr->get_आकार();

      मुक्त(label);
      विलोपय[] name;
}

शून्य compile_object_array(वर्ण*label, वर्ण*name, पूर्णाङ्क चरमः, पूर्णाङ्क first)
{
      vpiHandle obj = vpip_make_array(label, name, first, चरमः, सत्य);

      संरचना __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = नूतन vvp_darray_object(arr->get_आकार());
      arr->vals_विस्तारः = 1;

      गणनम्_real_arrays += 1;
      गणनम्_real_array_शब्दाः += arr->get_आकार();

      मुक्त(label);
      विलोपय[] name;
}

शून्य compile_net_array(वर्ण*label, वर्ण*name, पूर्णाङ्क चरमः, पूर्णाङ्क first)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, first, चरमः, असत्य);

      संरचना __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(शून्यस्मृतिं_देहि(arr->get_आकार(), परिमाण(vpiHandle)));

      गणनम्_net_arrays += 1;
      गणनम्_net_array_शब्दाः += arr->get_आकार();

      मुक्त(label);
      विलोपय[] name;
}

वर्ग vvp_fun_arrayport  : सार्वजनिक vvp_net_fun_t {

    सार्वजनिक:
      स्पष्ट vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      स्पष्ट vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, दीर्घ addr);
      ~vvp_fun_arrayport() override;

      आभास शून्य check_word_change(अचिह्नित दीर्घ addr) = 0;

    रक्षित:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      अचिह्नित दीर्घ addr_;

      मित्रम् शून्य array_attach_पत्तनम्(vvp_array_t, vvp_fun_arrayport*);
      मित्रम् शून्य __vpiArray::word_change(अचिह्नित दीर्घ);
      vvp_fun_arrayport*अग्र;
};

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net)
: arr_(mem), net_(net), addr_(0)
{
      अग्र = 0;
}

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, दीर्घ addr)
: arr_(mem), net_(net), addr_(addr)
{
      अग्र = 0;
}

vvp_fun_arrayport::~vvp_fun_arrayport()
{
}

वर्ग vvp_fun_arrayport_sa  : सार्वजनिक vvp_fun_arrayport {

    सार्वजनिक:
      स्पष्ट vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      स्पष्ट vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, दीर्घ addr);
      ~vvp_fun_arrayport_sa() override;

      शून्य check_word_change(अचिह्नित दीर्घ addr) override;

      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t) override;

    गुप्त:
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

शून्य vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                                     vvp_context_t)
{
      तार्किक addr_valid_flag;

      विकल्प (पत्तनम्.पत्तनम्()) {

	  पक्ष 0: // Address input
	    addr_valid_flag = vector4_गन्तव्यम्_मानम्(bit, addr_);
	    यदि (! addr_valid_flag)
		  addr_ = arr_->get_आकार();
	    यदि (vpi_array_is_real(arr_))
		  पत्तनम्.ptr()->send_real(arr_->get_word_r(addr_), 0);
	    अन्यथा
		  पत्तनम्.ptr()->send_vec4(arr_->get_word(addr_), 0);

	    भङ्ग;

	  स्वभाव:
	    fprintf(stdout, "XXXX write ports not implemented.\n");
	    प्रतिज्ञा(0);
      }
}

शून्य vvp_fun_arrayport_sa::check_word_change(अचिह्नित दीर्घ addr)
{
      यदि (addr != addr_) प्रत्यागम;

      यदि (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr_), 0);
      } अन्यथा {
	    net_->send_vec4(arr_->get_word(addr_), 0);
      }
}

वर्ग vvp_fun_arrayport_aa  : सार्वजनिक vvp_fun_arrayport, सार्वजनिक automatic_hooks_s {

    सार्वजनिक:
      स्पष्ट vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net);
      स्पष्ट vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, दीर्घ addr);
      ~vvp_fun_arrayport_aa() override;

      शून्य alloc_instance(vvp_context_t context) override;
      शून्य reset_instance(vvp_context_t context) override;
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
      शून्य free_instance(vvp_context_t context) override;
#यदि_अन्तः

      शून्य check_word_change(अचिह्नित दीर्घ addr) override;

      शून्य recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                     vvp_context_t context) override;

    गुप्त:
      शून्य check_word_change_(अचिह्नित दीर्घ addr, vvp_context_t context);

      __vpiScope*context_scope_;
      अचिह्नित context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_सङ्कलय_item_गन्तव्यम्_context(एतत्, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, दीर्घ addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_सङ्कलय_item_गन्तव्यम्_context(एतत्, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

शून्य vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      अचिह्नित दीर्घ*addr = नूतन अचिह्नित दीर्घ;
      vvp_समुच्चयः_context_item(context, context_idx_, addr);

      *addr = addr_;
}

शून्य vvp_fun_arrayport_aa::reset_instance(vvp_context_t context)
{
      अचिह्नित दीर्घ*addr = static_cast<अचिह्नित दीर्घ*>
            (vvp_get_context_item(context, context_idx_));

      *addr = addr_;
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य vvp_fun_arrayport_aa::free_instance(vvp_context_t context)
{
      अचिह्नित दीर्घ*addr = static_cast<अचिह्नित दीर्घ*>
            (vvp_get_context_item(context, context_idx_));
      विलोपय addr;
}
#यदि_अन्तः

शून्य vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t पत्तनम्, ध्रुव vvp_vector4_t&bit,
                                     vvp_context_t context)
{
      यदि (context) {
            अचिह्नित दीर्घ*addr = static_cast<अचिह्नित दीर्घ*>
                  (vvp_get_context_item(context, context_idx_));

            तार्किक addr_valid_flag;

            विकल्प (पत्तनम्.पत्तनम्()) {

                पक्ष 0: // Address input
                  addr_valid_flag = vector4_गन्तव्यम्_मानम्(bit, *addr);
                  यदि (! addr_valid_flag) *addr = arr_->get_आकार();
                  यदि (vpi_array_is_real(arr_)) {
			पत्तनम्.ptr()->send_real(arr_->get_word_r(*addr),
					      context);
                  } अन्यथा {
			पत्तनम्.ptr()->send_vec4(arr_->get_word(*addr),
					      context);
                  }
                  भङ्ग;

                स्वभाव:
                  fprintf(stdout, "XXXX write ports not implemented.\n");
                  प्रतिज्ञा(0);
            }
      } अन्यथा {
            context = context_scope_->जीवितम्_contexts;
            यावत् (context) {
                  recv_vec4(पत्तनम्, bit, context);
                  context = vvp_get_अग्र_context(context);
            }
      }
}

शून्य vvp_fun_arrayport_aa::check_word_change_(अचिह्नित दीर्घ addr,
                                              vvp_context_t context)
{
      ध्रुव अचिह्नित दीर्घ*पत्तनम्_addr = static_cast<अचिह्नित दीर्घ*>
            (vvp_get_context_item(context, context_idx_));

      यदि (addr != *पत्तनम्_addr)
	    प्रत्यागम;

      यदि (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr), context);
      } अन्यथा {
	    net_->send_vec4(arr_->get_word(addr), context);
      }
}

शून्य vvp_fun_arrayport_aa::check_word_change(अचिह्नित दीर्घ addr)
{
      यदि (arr_->get_scope()->is_automatic()) {
            प्रतिज्ञा(vthread_get_wt_context());
            check_word_change_(addr, vthread_get_wt_context());
      } अन्यथा {
            vvp_context_t context = context_scope_->जीवितम्_contexts;
            यावत् (context) {
                  check_word_change_(addr, context);
                  context = vvp_get_अग्र_context(context);
            }
      }
}

स्थिर शून्य array_attach_पत्तनम्(vvp_array_t array, vvp_fun_arrayport*fun)
{
      प्रतिज्ञा(fun->अग्र == 0);
      fun->अग्र = array->ports_;
      array->ports_ = fun;
      यदि (!array->get_scope()->is_automatic() &&
          (array->vals4 || array->vals)) {
              /* propagate initial values for variable arrays */
            यदि (!vpi_array_is_real(array)) {
                  vvp_bit4_t init;
                  यदि (array->vals4)
                      init = BIT4_X;
                  अन्यथा
                      init = BIT4_0;
                  vvp_vector4_t tmp(array->vals_विस्तारः, init);
                  कालसारणी_init_propagate(fun->net_, tmp);
            } अन्यथा {
                  कालसारणी_init_propagate(fun->net_, 0.0);
            }
      }
}

वर्ग array_word_मानम्_callback : सार्वजनिक मानम्_callback {
    सार्वजनिक:
      अन्तःस्थ स्पष्ट array_word_मानम्_callback(p_cb_दत्त दत्त, दीर्घ addr)
      : मानम्_callback(दत्त), word_addr(addr)
      { }

    सार्वजनिक:
      दीर्घ word_addr;
};

शून्य __vpiArray::word_change(अचिह्नित दीर्घ addr)
{
      प्रति (vvp_fun_arrayport*cur = ports_; cur; cur = cur->अग्र)
	    cur->check_word_change(addr);

	// Run callbacks attached to the array itself.
      संरचना __vpiCallback *अग्र_2 = vpi_callbacks;
      संरचना __vpiCallback *prev = 0;

      यावत् (अग्र_2) {
	    array_word_मानम्_callback*cur = dynamic_cast<array_word_मानम्_callback*>(अग्र_2);
	    अग्र_2 = cur->अग्र_2;

	      // Skip callbacks that are not for me. -1 is for every element.
	    यदि (cur->word_addr != (दीर्घ)addr && cur->word_addr != -1) {
		  prev = cur;
		  अनुवर्त;
	    }

	      // For whole array callbacks we need to set the index.
	    यदि (cur->word_addr == -1) {
		  cur->cb_दत्त.सूचकांक = (PLI_INT32) ((पूर्णाङ्क)addr +
						    first_addr.get_मानम्());
	    }

	    यदि (cur->cb_दत्त.cb_rtn != 0) {
		  यदि (cur->test_मानम्_callback_ready()) {
			यदि (cur->cb_दत्त.value) {
			      यदि (vpi_array_is_real(एतत्)) {
				    द्विगुण val = 0.0;
				    यदि (addr < vals->get_आकार())
					  vals->get_word(addr, val);
				    vpip_real_get_मानम्(val, cur->cb_दत्त.value);
			      } अन्यथा यदि (vals4) {
				    vpip_vec4_get_मानम्(vals4->get_word(addr),
							vals_विस्तारः,
							signed_flag,
							cur->cb_दत्त.value);
			      } अन्यथा यदि (dynamic_cast<vvp_darray_atom<int8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int64_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint64_t>*>(vals)
				      || dynamic_cast<vvp_darray_vec2*>(vals)) {
				    vvp_vector4_t val;
				    यदि (addr < vals->get_आकार())
					  vals->get_word(addr, val);
				    vpip_vec4_get_मानम्(val,
							vals_विस्तारः,
							signed_flag,
							cur->cb_दत्त.value);
			      } अन्यथा {
			            प्रतिज्ञा(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } अन्यथा यदि (prev == 0) {

		  vpi_callbacks = अग्र_2;
		  cur->अग्र_2 = 0;
		  विलोपय cur;

	    } अन्यथा {
		  प्रतिज्ञा(prev->अग्र_2 == cur);
		  prev->अग्र_2 = अग्र_2;
		  cur->अग्र_2 = 0;
		  विलोपय cur;
	    }
      }
}

वर्ग array_resolv_list_t : सार्वजनिक resolv_list_s {

    सार्वजनिक:
      स्पष्ट array_resolv_list_t(वर्ण*lab) : resolv_list_s(lab) {
	    array = 0;
      }

      vvp_array_t*array;
      तार्किक resolve(तार्किक mes) override;

    गुप्त:
};

तार्किक array_resolv_list_t::resolve(तार्किक mes)
{
      *array = array_find(label());
      यदि (*array == 0) {
	    प्रतिज्ञा(!mes);
	    प्रत्यागम असत्य;
      }
      प्रत्यागम सत्य;
}

वर्ग array_पत्तनम्_resolv_list_t : सार्वजनिक resolv_list_s {

    सार्वजनिक:
      स्पष्ट array_पत्तनम्_resolv_list_t(वर्ण* lab, तार्किक use_addr__,
                                        दीर्घ addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      तार्किक use_addr;
      दीर्घ addr;
      तार्किक resolve(तार्किक mes) override;

    गुप्त:
};

array_पत्तनम्_resolv_list_t::array_पत्तनम्_resolv_list_t(वर्ण *lab, तार्किक use_addr__,
                                                   दीर्घ addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      यदि (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      अन्यथा
            context_scope = 0;
      ptr = नूतन vvp_net_t;
}

तार्किक array_पत्तनम्_resolv_list_t::resolve(तार्किक mes)
{
      vvp_array_t mem = array_find(label());
      यदि (mem == 0) {
	    प्रतिज्ञा(mem || !mes);
	    प्रत्यागम असत्य;
      }

      vvp_fun_arrayport*fun;
      यदि (use_addr)
            यदि (context_scope)
                  fun = नूतन vvp_fun_arrayport_aa(context_scope, mem, ptr, addr);
            अन्यथा
                  fun = नूतन vvp_fun_arrayport_sa(mem, ptr, addr);
      अन्यथा
            यदि (context_scope)
                  fun = नूतन vvp_fun_arrayport_aa(context_scope, mem, ptr);
            अन्यथा
                  fun = नूतन vvp_fun_arrayport_sa(mem, ptr);
      ptr->fun = fun;

      array_attach_पत्तनम्(mem, fun);

      प्रत्यागम सत्य;
}

वर्ग array_word_part_callback : सार्वजनिक array_word_मानम्_callback {
    सार्वजनिक:
      स्पष्ट array_word_part_callback(p_cb_दत्त दत्त, दीर्घ addr);
      ~array_word_part_callback() override;

      array_word_part_callback(ध्रुव array_word_part_callback&) = विलोपय;
      array_word_part_callback& संक्रियकः=(ध्रुव array_word_part_callback&) = विलोपय;

      तार्किक test_मानम्_callback_ready(शून्य) override;

    गुप्त:
      वर्ण*मानम्_द्वयङ्काः;
};

array_word_part_callback::array_word_part_callback(p_cb_दत्त दत्त, दीर्घ addr)
: array_word_मानम्_callback(दत्त, addr)
{
	// Get the initial value of the part, to use as a reference.
      संरचना __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(दत्त->obj);
      s_vpi_मानम् tmp_मानम्;
      tmp_मानम्.format = vpiBinStrVal;
      apvword->vpi_get_मानम्(&tmp_मानम्);

      मानम्_द्वयङ्काः = नूतन वर्ण[apvword->part_wid+1];

      स्मृतिं_प्रतिलिख(मानम्_द्वयङ्काः, tmp_मानम्.value.str, apvword->part_wid);
      मानम्_द्वयङ्काः[apvword->part_wid] = 0;
}

array_word_part_callback::~array_word_part_callback()
{
      विलोपय[]मानम्_द्वयङ्काः;
}

तार्किक array_word_part_callback::test_मानम्_callback_ready(शून्य)
{
      संरचना __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_दत्त.obj);
      प्रतिज्ञा(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_मानम् tmp_मानम्;
      tmp_मानम्.format = vpiBinStrVal;
      apvword->vpi_get_मानम्(&tmp_मानम्);

      यदि (memcmp(मानम्_द्वयङ्काः, tmp_मानम्.value.str, apvword->part_wid) == 0)
	    प्रत्यागम असत्य;

      स्मृतिं_प्रतिलिख(मानम्_द्वयङ्काः, tmp_मानम्.value.str, apvword->part_wid);
      प्रत्यागम सत्य;

}

मानम्_callback*vpip_array_word_change(p_cb_दत्त दत्त)
{
      संरचना __vpiArray*parent = 0;
      array_word_मानम्_callback*cbh = 0;
      यदि (ध्रुव संरचना __vpiArrayWord*word = array_var_word_from_handle(दत्त->obj)) {
	    parent = static_cast<__vpiArray*>(word->get_parent());
	    अचिह्नित addr = word->get_सूचकांक();
	    cbh = नूतन array_word_मानम्_callback(दत्त, addr);

      } अन्यथा यदि (संरचना __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(दत्त->obj)) {
	    parent = tword->array;
	    cbh = नूतन array_word_मानम्_callback(दत्त, tword->स्थानाङ्कः);

      } अन्यथा यदि (संरचना __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(दत्त->obj)) {
	    parent = apvword->array;
	    cbh = नूतन array_word_part_callback(दत्त, apvword->word_sel);
      }

      प्रतिज्ञा(cbh);
      प्रतिज्ञा(parent);
      cbh->अग्र_2 = parent->vpi_callbacks;
      parent->vpi_callbacks = cbh;

      प्रत्यागम cbh;
}

मानम्_callback* vpip_array_change(p_cb_दत्त दत्त)
{
      array_word_मानम्_callback*cbh = नूतन array_word_मानम्_callback(दत्त, -1);
      प्रतिज्ञा(दत्त->obj);

      संरचना __vpiArray*arr = dynamic_cast<__vpiArray*>(दत्त->obj);
      cbh->अग्र_2 = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      प्रत्यागम cbh;
}

शून्य compile_array_पत्तनम्(वर्ण*label, वर्ण*array, वर्ण*addr)
{
      array_पत्तनम्_resolv_list_t*resolv_mem
	    = नूतन array_पत्तनम्_resolv_list_t(array, असत्य, 0);

      define_functor_चिह्नम्(label, resolv_mem->ptr);
      मुक्त(label);
	// Connect the port-0 input as the address.
      निवेशः_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

शून्य compile_array_पत्तनम्(वर्ण*label, वर्ण*array, दीर्घ addr)
{
      array_पत्तनम्_resolv_list_t*resolv_mem
	    = नूतन array_पत्तनम्_resolv_list_t(array, सत्य, addr);

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
      obj->चरमः_addr  = mem->चरमः_addr;
      obj->swap_addr = mem->swap_addr;

      obj->msb = mem->msb;
      obj->lsb = mem->lsb;

	// Share the words with the source array.
      obj->nets = mem->nets;
      obj->vals4 = mem->vals4;
      obj->vals  = mem->vals;
      obj->vals_विस्तारः = mem->vals_विस्तारः;
      obj->vals_शब्दाः = mem->vals_शब्दाः;

      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

      प्रतिज्ञा(array_सारणी);
      प्रतिज्ञा(!array_find(label));
      array_सारणी->sym_समुच्चयः_मानम्(label, obj);

      compile_vpi_चिह्नम्(label, obj);
      vpip_attach_गन्तव्यम्_current_scope(obj);

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
vpiHandle vpip_make_vthr_A(वर्ण*label, अचिह्नित addr)
{
      संरचना __vpiArrayVthrA*obj = नूतन __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = नूतन array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->स्थानाङ्कः_handle = 0;
      obj->स्थानाङ्कः = addr;

      प्रत्यागम obj;
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

      obj->स्थानाङ्कः_handle = 0;
      compile_vpi_lookup(&obj->स्थानाङ्कः_handle, चिह्नम्);
      obj->स्थानाङ्कः = 0;

      प्रत्यागम obj;
}

vpiHandle vpip_make_vthr_A(वर्ण*label, vpiHandle handle)
{
      संरचना __vpiArrayVthrA*obj = नूतन __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = नूतन array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->स्थानाङ्कः_handle = handle;
      obj->स्थानाङ्कः = 0;

      प्रत्यागम obj;
}

vpiHandle vpip_make_vthr_APV(वर्ण*label, अचिह्नित सूचकांक, अचिह्नित bit, अचिह्नित wid)
{
      संरचना __vpiArrayVthrAPV*obj = नूतन __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = नूतन array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->word_sel = सूचकांक;
      obj->part_bit = bit;
      obj->part_wid = wid;

      प्रत्यागम obj;
}

शून्य compile_array_cleanup(शून्य)
{
      विलोपय array_सारणी;
      array_सारणी = 0;
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य स्मृतिः_delete(vpiHandle item)
{
      संरचना __vpiArray*arr = static_cast<संरचना __vpiArray*>(item);
      यदि (arr->vals_शब्दाः) विलोपय [] (arr->vals_शब्दाः-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      विलोपय arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      विलोपय arr->vals;

      यदि (arr->nets) {
	    प्रति (अचिह्नित idx = 0; idx < arr->get_आकार(); idx += 1) {
		  यदि (संरचना __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.सूचकांक);
		    /* These should only be the real words. */
		  } अन्यथा {
			प्रतिज्ञा(arr->nets[idx]->get_type_संहिता() ==
			       vpiRealVar);
			संरचना __vpiRealVar *sigr = static_cast<संरचना __vpiRealVar *>
			                            (arr->nets[idx]);
			constant_delete(sigr->id.सूचकांक);
// Why are only the real words still here?
			विलोपय arr->nets[idx];
		  }
	    }
	    मुक्त(arr->nets);
      }

      यावत् (arr->vpi_callbacks) {
	    संरचना __vpiCallback*tmp = arr->vpi_callbacks->अग्र_2;
	    विलोपय arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      विलोपय arr;
}

शून्य A_delete(vpiHandle item)
{
      संरचना __vpiArrayVthrA*obj = static_cast<संरचना __vpiArrayVthrA*>(item);
      यदि (obj->स्थानाङ्कः_handle) {
	    विकल्प (obj->स्थानाङ्कः_handle->get_type_संहिता()) {
		पक्ष vpi_स्मृतिः_Word:
		  यदि (vpi_get(_vpiFromThr, obj->स्थानाङ्कः_handle) == _vpi_at_A) {
			A_delete(obj->स्थानाङ्कः_handle);
		  }
		  भङ्ग;
		पक्ष vpiPartSelect:
		  प्रतिज्ञा(vpi_get(_vpiFromThr, obj->स्थानाङ्कः_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->स्थानाङ्कः_handle);
		  भङ्ग;
	    }
      }

      विलोपय obj;
}

शून्य APV_delete(vpiHandle item)
{
      संरचना __vpiArrayVthrAPV*obj = static_cast<संरचना __vpiArrayVthrAPV*>(item);
      विलोपय obj;
}
#यदि_अन्तः
