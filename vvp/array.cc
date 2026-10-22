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
#अगर_घोषित CHECK_WITH_VALGRIND
#समावेश  "vvp_cleanup.h"
#पूर्ण_अगर
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <iostream>
# include  "compile.h"
# include  <cassert>
# include  "ivl_alloc.h"

using namespace std;

अचिन्हित दीर्घ गिनती_net_arrays = 0;
अचिन्हित दीर्घ गिनती_net_array_शब्दगण = 0;
अचिन्हित दीर्घ गिनती_var_arrays = 0;
अचिन्हित दीर्घ गिनती_var_array_शब्दगण = 0;
अचिन्हित दीर्घ गिनती_real_arrays = 0;
अचिन्हित दीर्घ गिनती_real_array_शब्दगण = 0;

अटल प्रतीक_map_s<काष्ठा __vpiArray>* array_table =0;

श्रेणी vvp_fun_arrayport;
अटल व्योम array_attach_port(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_find(स्थिर अक्षर*label)
{
      अगर (array_table == 0)
	    वापस 0;

      vvp_array_t v = array_table->sym_get_मूल्य(label);
      वापस v;
}

काष्ठा __vpiArrayVthrA : खुला __vpiHandle {
      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiMemoryWord; }
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर* vpi_get_str(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;
      vpiHandle vpi_put_मूल्य(p_vpi_मूल्य val, पूर्णांक flags) override;
      vpiHandle vpi_handle(पूर्णांक code) override;

      काष्ठा __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle address_handle;
	// If wid==0, then address is the address into the array.
      अचिन्हित address;

      अचिन्हित get_address() स्थिर
      {
	    अगर (address_handle) {
		  s_vpi_मूल्य vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpiVectorVal;
		  address_handle->vpi_get_मूल्य(&vp);
		  पूर्णांक शब्दगण = (address_handle->vpi_get(vpi_आकार)-1)/32 + 1;
		  क्रम(पूर्णांक idx = 0; idx < शब्दगण; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			अगर (vp.value.vector[idx].bval != 0) वापस अच_पूर्णांक_उच्च;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  address_handle->vpi_get_मूल्य(&vp);
		  वापस vp.value.integer;
	    }

	    वापस address;
      }
};


काष्ठा __vpiArrayVthrAPV : खुला __vpiHandle {
      पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiPartSelect; }
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर* vpi_get_str(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;
      vpiHandle vpi_handle(पूर्णांक code) override;

      काष्ठा __vpiArray*array;
      अचिन्हित word_sel;
      अचिन्हित part_bit;
      अचिन्हित part_wid;
};

bool is_net_array(vpiHandle obj)
{
      काष्ठा __vpiArray*rfp = dynamic_cast<__vpiArray*> (obj);
      निश्चित(rfp);

      अगर (rfp->nets != 0) वापस true;
      वापस false;
}

// This function return true if the underlying array words are real.
अटल bool vpi_array_is_real(स्थिर vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      अगर (arr->vals4 != 0)  // A bit based variable/register array.
	    वापस false;

      अगर (dynamic_cast<vvp_darray_real*> (arr->vals))
	    वापस true;

      अगर (arr->vals != 0)
	    वापस false;

	// This must be a net array so look at element 0 to find the type.
      निश्चित(arr->nets != 0);
      निश्चित(arr->get_आकार() > 0);
      स्थिर काष्ठा __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      अगर (rsig) {
	    वापस true;
      }

      वापस false;
}

अटल bool vpi_array_is_string(स्थिर vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      अगर (arr->vals4 != 0)  // A bit based variable/register array.
	    वापस false;

      अगर (dynamic_cast<vvp_darray_string*> (arr->vals))
	    वापस true;

      वापस false;
}

पूर्णांक __vpiArray::get_word_आकार() स्थिर
{
      अचिन्हित width;

      निश्चित(get_आकार() > 0);
	/* For a net array we need to get the width from the first element. */
      अगर (nets) {
	    निश्चित(vals4 == 0 && vals == 0);
	    स्थिर काष्ठा __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    निश्चित(vsig);
	    width = vpip_आकार(vsig);
	/* For a variable array we can get the width from vals_width. */
      } अन्यथा {
	    निश्चित(vals4 || vals);
	    width = vals_width;
      }

      वापस width;
}

अक्षर*__vpiArray::get_word_str(काष्ठा __vpiArrayWord*word, पूर्णांक code)
{
      अचिन्हित सूचकांक = word->get_सूचकांक();

      अगर (code == vpiFile) {  // Not implemented for now!
	    वापस simple_set_rbuf_str(file_names[0]);
      }

      अक्षर sidx [64];
      snprintf(sidx, 63, "%d", (पूर्णांक)सूचकांक + first_addr.get_मूल्य());
      वापस generic_get_str(code, get_scope(), name, sidx);
}

व्योम __vpiArray::get_word_मूल्य(काष्ठा __vpiArrayWord*word, p_vpi_मूल्य vp)
{
      अचिन्हित सूचकांक = word->get_सूचकांक();

    // Determine the appropriate format (The Verilog PLI Handbook 5.2.10)
      अगर(vp->format == vpiObjTypeVal) {
          अगर(vpi_array_is_real(यह))
              vp->format = vpiRealVal;
          अन्यथा अगर(vpi_array_is_string(यह))
              vp->format = vpiStringVal;
          अन्यथा
              vp->format = vpiIntVal;
      }

      अगर(vals4) {
          vpip_vec4_get_मूल्य(vals4->get_word(सूचकांक),
                              vals4->width(), signed_flag, vp);
      } अन्यथा अगर(vals) {
          चयन(vp->format) {
            हाल vpiBinStrVal:
            हाल vpiOctStrVal:
            हाल vpiDecStrVal:
            हाल vpiHexStrVal:
            हाल vpiScalarVal:
            हाल vpiIntVal:
            हाल vpiVectorVal:
            {
                vvp_vector4_t v;
                vals->get_word(सूचकांक, v);
                vpip_vec4_get_मूल्य(v, vals_width, signed_flag, vp);
            }
            अवरोध;

            हाल vpiRealVal:
            {
                द्विगुन d;
                vals->get_word(सूचकांक, d);
                vpip_real_get_मूल्य(d, vp);
            }
            अवरोध;

            हाल vpiStringVal:
            {
                string s;
                vals->get_word(सूचकांक, s);
                vpip_string_get_मूल्य(s, vp);
            }
            अवरोध;

            शेष:
                ख_लिखो(मानक_त्रुटि, "vpi sorry: format is not implemented\n");
                निश्चित(false);
          }
      }
}

व्योम __vpiArray::put_word_मूल्य(काष्ठा __vpiArrayWord*word, p_vpi_मूल्य vp, पूर्णांक)
{
      अचिन्हित सूचकांक = word->get_सूचकांक();
      vvp_vector4_t val = vec4_from_vpi_मूल्य(vp, vals_width);
      set_word(सूचकांक, 0, val);
}

vpiHandle __vpiArray::get_iter_सूचकांक(काष्ठा __vpiArrayIterator*, पूर्णांक idx)
{
      अगर (nets) वापस nets[idx];

      निश्चित(vals4 || vals);

      अगर (vals_शब्दगण == 0) make_vals_शब्दगण();

      वापस &(vals_शब्दगण[idx].as_word);
}

पूर्णांक __vpiArray::vpi_get(पूर्णांक code)
{
      चयन (code) {
	  हाल vpi_पंक्ति_No:
	    वापस 0; // Not implemented for now!

	  हाल vpi_आकार:
	    वापस get_आकार();

	  हाल vpiAutomatic:
	    वापस scope->is_automatic()? 1 : 0;

	  शेष:
	    वापस 0;
      }
}

अक्षर* __vpiArray::vpi_get_str(पूर्णांक code)
{
      अगर (code == vpiFile) {  // Not implemented for now!
            वापस simple_set_rbuf_str(file_names[0]);
      }

      वापस generic_get_str(code, scope, name, शून्य);
}

vpiHandle __vpiArray::vpi_handle(पूर्णांक code)
{
      चयन (code) {
          हाल vpi_बायाँ_Range:
            अगर (swap_addr) वापस &last_addr;
            अन्यथा वापस &first_addr;

          हाल vpi_दायाँ_Range:
            अगर (swap_addr) वापस &first_addr;
            अन्यथा वापस &last_addr;

	  हाल vpiScope:
	    वापस scope;

	  हाल vpiModule:
	    वापस vpip_module(scope);
      }

      वापस 0;
}

/*
* VPI code passes indices that are not yet converted to canonical
* form, so this index function does it here.
*/
vpiHandle __vpiArray::vpi_सूचकांक(पूर्णांक सूचकांक)
{
      सूचकांक -= first_addr.get_मूल्य();
      अगर (सूचकांक >= (दीर्घ) get_आकार())
	    वापस 0;
      अगर (सूचकांक < 0)
	    वापस 0;

      अगर (nets != 0) {
	    वापस nets[सूचकांक];
      }

      अगर (vals_शब्दगण == 0)
	    make_vals_शब्दगण();

      वापस &(vals_शब्दगण[सूचकांक].as_word);
}

पूर्णांक __vpiArrayWord::as_word_t::vpi_get(पूर्णांक code)
{
      काष्ठा __vpiArrayWord*obj = array_var_word_from_handle(यह);
      निश्चित(obj);
      काष्ठा __vpiArrayBase*my_parent = obj->get_parent();
      t_vpi_मूल्य val;

      चयन (code) {
	  हाल vpi_पंक्ति_No:
	    वापस 0; // Not implemented for now!

	  हाल vpi_आकार:
	    वापस my_parent->get_word_आकार();

	  हाल vpi_बायाँ_Range:
            val.format = vpiIntVal;
	    my_parent->get_बायाँ_range()->vpi_get_मूल्य(&val);
            निश्चित(val.format == vpiIntVal);
            वापस val.value.integer;

	  हाल vpi_दायाँ_Range:
            val.format = vpiIntVal;
	    my_parent->get_दायाँ_range()->vpi_get_मूल्य(&val);
            निश्चित(val.format == vpiIntVal);
            वापस val.value.integer;

	  हाल vpi_सूचकांक_2:
	    {
		  पूर्णांक base_offset = 0;
		  काष्ठा __vpiArray*base = dynamic_cast<__vpiArray*> (my_parent);
		  अगर (base) {
			val.format = vpiIntVal;
			base->first_addr.vpi_get_मूल्य(&val);
			base_offset += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_सूचकांक.vpi_get_मूल्य(&val);
		  निश्चित(val.format == vpiIntVal);
		  वापस val.value.integer + base_offset;
	    }

	  हाल vpiAutomatic:
	    वापस my_parent->get_scope()->is_automatic()? 1 : 0;

#अगर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  हाल _vpiFromThr:
	    वापस _vpiNoThr;
#पूर्ण_अगर

	  शेष:
	    वापस 0;
      }
}


पूर्णांक __vpiArrayVthrA::vpi_get(पूर्णांक code)
{
      चयन (code) {
	  हाल vpi_पंक्ति_No:
	    वापस 0; // Not implemented for now!

	  हाल vpi_आकार:
	    वापस array->get_word_आकार();

	  हाल vpi_बायाँ_Range:
	    वापस array->msb.get_मूल्य();

	  हाल vpi_दायाँ_Range:
	    वापस array->lsb.get_मूल्य();

	  हाल vpi_सूचकांक_2:
	    वापस (पूर्णांक)get_address() + array->first_addr.get_मूल्य();

	  हाल vpiAutomatic:
	    वापस array->get_scope()->is_automatic() ? 1 : 0;

#अगर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  हाल _vpiFromThr:
	    वापस _vpi_at_A;
#पूर्ण_अगर

	  // If address_handle is not zero we definitely have a
	  // variable.
	  हाल vpiConstantSelect:
	    वापस address_handle == 0;

	  शेष:
	    वापस 0;
      }
}

अक्षर* __vpiArrayVthrA::vpi_get_str(पूर्णांक code)
{
      अगर (code == vpiFile) {  // Not implemented for now!
            वापस simple_set_rbuf_str(file_names[0]);
      }

      अक्षर sidx [64];
      snprintf(sidx, 63, "%d", (पूर्णांक)get_address() + array->first_addr.get_मूल्य());
      वापस generic_get_str(code, array->get_scope(), array->name, sidx);
}

व्योम __vpiArrayVthrA::vpi_get_मूल्य(p_vpi_मूल्य vp)
{
      निश्चित(array);

      अचिन्हित सूचकांक = get_address();
      अगर (vpi_array_is_real(array)) {
	    द्विगुन tmp = array->get_word_r(सूचकांक);
	    vpip_real_get_मूल्य(tmp, vp);
      } अन्यथा अगर (vpi_array_is_string(array)) {
	    string tmp = array->get_word_str(सूचकांक);
	    vpip_string_get_मूल्य(tmp, vp);
      } अन्यथा {
	    vvp_vector4_t tmp = array->get_word(सूचकांक);
	    अचिन्हित width = array->get_word_आकार();
	    vpip_vec4_get_मूल्य(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_मूल्य(p_vpi_मूल्य vp, पूर्णांक)
{
      अचिन्हित सूचकांक = get_address();

      निश्चित(array);
      निश्चित(सूचकांक < array->get_आकार());

      अगर (vpi_array_is_real(array)) {
	    द्विगुन val = real_from_vpi_मूल्य(vp);
	    array->set_word(सूचकांक, val);
      } अन्यथा {
	    अचिन्हित width = array->get_word_आकार();
	    vvp_vector4_t val = vec4_from_vpi_मूल्य(vp, width);
	    array->set_word(सूचकांक, 0, val);
      }

      वापस यह;
}

vpiHandle __vpiArrayVthrA::vpi_handle(पूर्णांक code)
{
      चयन (code) {
	  हाल vpi_सूचकांक_2:
	    अवरोध;  // Not implemented!

	  हाल vpi_बायाँ_Range:
	    वापस &array->msb;

	  हाल vpi_दायाँ_Range:
	    वापस &array->lsb;

	  हाल vpiParent:
	  हाल vpiArray:
	    वापस array;

	  हाल vpiScope:
	    वापस array->get_scope();

	  हाल vpiModule:
	    वापस vpip_module(array->get_scope());
      }

      वापस 0;
}


पूर्णांक __vpiArrayVthrAPV::vpi_get(पूर्णांक code)
{
      चयन (code) {
	  हाल vpi_पंक्ति_No:
	    वापस 0; // Not implemented for now!

	  हाल vpi_आकार:
	    वापस part_wid;

	  हाल vpi_बायाँ_Range:
	    वापस part_bit + part_wid - 1;

	  हाल vpi_दायाँ_Range:
	    वापस part_bit;

	  हाल vpiAutomatic:
	    वापस array->get_scope()->is_automatic() ? 1 : 0;

#अगर defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  हाल _vpiFromThr:
	    वापस _vpi_at_APV;
#पूर्ण_अगर

	  हाल vpiConstantSelect:
	    वापस 1;

	  शेष:
	    वापस 0;
      }
}

अक्षर* __vpiArrayVthrAPV::vpi_get_str(पूर्णांक code)
{
      अगर (code == vpiFile) {  // Not implemented for now!
            वापस simple_set_rbuf_str(file_names[0]);
      }

      अक्षर sidx [64];
      snprintf(sidx, 63, "%u", word_sel + array->first_addr.get_मूल्य());
      वापस generic_get_str(code, array->get_scope(), array->name, sidx);
}

व्योम __vpiArrayVthrAPV::vpi_get_मूल्य(p_vpi_मूल्य vp)
{
      निश्चित(array);

      अचिन्हित सूचकांक = word_sel;
      अगर (vpi_array_is_real(array)) {
	    द्विगुन tmp = array->get_word_r(सूचकांक);
	    vpip_real_get_मूल्य(tmp, vp);
      } अन्यथा {
	    vvp_vector4_t tmp = array->get_word(सूचकांक);
	    tmp = tmp.subvalue(part_bit, part_wid);
	    vpip_vec4_get_मूल्य(tmp, part_wid, false, vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(पूर्णांक code)
{
      चयन (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  हाल vpiParent:
	    वापस 0;

            // Not part of the Verilog standard. We use this internally.
	  हाल vpiArray:
	    वापस array;

	  हाल vpiScope:
	    वापस array->get_scope();

	  हाल vpiModule:
	    वापस vpip_module(array->get_scope());
      }

      वापस 0;
}

व्योम __vpiArray::set_word(अचिन्हित address, अचिन्हित part_off, स्थिर vvp_vector4_t&val)
{
      अगर (address >= get_आकार())
	    वापस;

      अगर (vals4) {
	    निश्चित(nets == 0);
	    अगर (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp = vals4->get_word(address);
		  अगर ((part_off + val.size()) > tmp.size()) {
			अ_त्रुटि << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << प_पूर्ण;
			निश्चित(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals4->set_word(address, tmp);
	    } अन्यथा {
		  vals4->set_word(address, val);
	    }
	    word_change(address);
	    वापस;
      }

      अगर (vals) {
	    निश्चित(nets == 0);
	    अगर (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp;
		  vals->get_word(address, tmp);
		  अगर ((part_off + val.size()) > tmp.size()) {
			अ_त्रुटि << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << प_पूर्ण;
			निश्चित(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals->set_word(address, tmp);
	    } अन्यथा {
		  vals->set_word(address, val);
	    }
	    word_change(address);
	    वापस;
      }

      निश्चित(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle word = nets[address];
      स्थिर काष्ठा __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      निश्चित(vsig);

      vsig->गाँठ->send_vec4_pv(val, part_off, vpip_आकार(vsig), 0);
      word_change(address);
}

व्योम __vpiArray::set_word(अचिन्हित address, द्विगुन val)
{
      निश्चित(vals != 0);
      निश्चित(nets == 0);

      अगर (address >= vals->get_आकार())
	    वापस;

      vals->set_word(address, val);
      word_change(address);
}

व्योम __vpiArray::set_word(अचिन्हित address, स्थिर string&val)
{
      निश्चित(vals != 0);
      निश्चित(nets == 0);

      अगर (address >= vals->get_आकार())
	    वापस;

      vals->set_word(address, val);
      word_change(address);
}

व्योम __vpiArray::set_word(अचिन्हित address, स्थिर vvp_object_t&val)
{
      निश्चित(vals != 0);
      निश्चित(nets == 0);

      अगर (address >= vals->get_आकार())
	    वापस;

      vals->set_word(address, val);
      word_change(address);
}

vvp_vector4_t __vpiArray::get_word(अचिन्हित address)
{
      अगर (vals4) {
	    निश्चित(nets == 0);
	    निश्चित(vals == 0);
	    वापस vals4->get_word(address);
      }

      अगर (vals) {
	    निश्चित(nets == 0);
	    निश्चित(vals4== 0);
	    अगर (address >= vals->get_आकार())
		  वापस vvp_vector4_t(vals_width, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_word(address, val);
	    वापस val;
      }

      निश्चित(vals4 == 0);
      निश्चित(vals == 0);
      निश्चित(nets != 0);

      अगर (address >= get_आकार()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    निश्चित(get_आकार() > 0);
	    vpiHandle word = nets[0];
	    निश्चित(word);
	    काष्ठा __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
	    निश्चित(vsig);
	    vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*> (vsig->गाँठ->fil);
	    निश्चित(sig);
	    वापस vvp_vector4_t(sig->मूल्य_आकार(), BIT4_X);
      }

      vpiHandle word = nets[address];
      काष्ठा __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      निश्चित(vsig);
      vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*> (vsig->गाँठ->fil);
      निश्चित(sig);

      vvp_vector4_t val;
      sig->vec4_मूल्य(val);
      वापस val;
}

द्विगुन __vpiArray::get_word_r(अचिन्हित address)
{
      अगर (vals) {
	    निश्चित(vals4 == 0);
	    निश्चित(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    अगर (address >= vals->get_आकार())
		  वापस 0.0;

	    द्विगुन val;
	    vals->get_word(address, val);
	    वापस val;
      }

      अगर (address >= get_आकार())
	    वापस 0.0;

      निश्चित(nets);
      vpiHandle word = nets[address];
      काष्ठा __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(word);
      निश्चित(vsig);
      vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*> (vsig->net->fil);
      निश्चित(sig);

      द्विगुन val = sig->real_मूल्य();
      वापस val;
}

व्योम __vpiArray::get_word_obj(अचिन्हित address, vvp_object_t&val)
{
      अगर (vals) {
	    निश्चित(vals4 == 0);
	    निश्चित(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    अगर (address >= vals->get_आकार()) {
		  val = vvp_object_t();
		  वापस;
	    }

	    vals->get_word(address, val);
	    वापस;
      }

      निश्चित(nets);
	// Arrays of string nets not implemented!
      निश्चित(0);
      वापस;
}

string __vpiArray::get_word_str(अचिन्हित address)
{
      अगर (vals) {
	    निश्चित(vals4 == 0);
	    निश्चित(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    अगर (address >= vals->get_आकार())
		  वापस "";

	    string val;
	    vals->get_word(address, val);
	    वापस val;
      }

      निश्चित(nets);
	// Arrays of string nets not implemented!
      निश्चित(0);
      वापस "";
}

vpiHandle vpip_make_array(स्थिर अक्षर*label, स्थिर अक्षर*name,
                          पूर्णांक first_addr, पूर्णांक last_addr,
                          bool signed_flag)
{
      काष्ठा __vpiArray*obj = नया __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      अगर (last_addr >= first_addr) {
	    obj->swap_addr = false;
      } अन्यथा {
	    obj->swap_addr = true;
	    पूर्णांक tmp = last_addr;
	    last_addr = first_addr;
	    first_addr = tmp;
      }
      निश्चित(last_addr >= first_addr);
      अचिन्हित array_गिनती = last_addr+1-first_addr;

	// For now, treat all arrays as memories. This is not quite
	// correct, as arrays are arrays with memories a special case.
      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_गिनती = array_गिनती;

      obj->first_addr.set_मूल्य(first_addr);
      obj->last_addr .set_मूल्य(last_addr);

	// Start off now knowing if we are nets or variables.
      obj->nets = 0;
      obj->vals4 = 0;
      obj->vals  = 0;
      obj->vals_width = 0;
      obj->vals_शब्दगण = 0;

	// Initialize (clear) the read-ports list.
      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

	/* Add this symbol to the array_symbols table for later lookup. */
      अगर (!array_table)
	    array_table = नया प्रतीक_map_s<काष्ठा __vpiArray>;

      निश्चित(!array_find(label));
      array_table->sym_set_मूल्य(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_प्रतीक(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_to_current_scope(obj);

      वापस obj;
}

व्योम __vpiArray::alias_word(अचिन्हित दीर्घ addr, vpiHandle word, पूर्णांक msb_, पूर्णांक lsb_)
{
      निश्चित(msb.get_मूल्य() == msb_);
      निश्चित(lsb.get_मूल्य() == lsb_);
      निश्चित(addr < get_आकार());
      निश्चित(nets);
      nets[addr] = word;
}

व्योम __vpiArray::attach_word(अचिन्हित addr, vpiHandle word)
{
      निश्चित(addr < get_आकार());
      निश्चित(nets);
      nets[addr] = word;

      अगर (काष्ठा __vpiSignal*sig = dynamic_cast<__vpiSignal*>(word)) {
	    vvp_net_t*net = sig->गाँठ;
	    निश्चित(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    निश्चित(fun);
	    fun->attach_as_word(यह, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = यह;
	    sig->id.सूचकांक = नया __vpiDecConst(addr + first_addr.get_मूल्य());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    वापस;
      }

      अगर (काष्ठा __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(word)) {
	    vvp_net_t*net = sig->net;
	    निश्चित(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    निश्चित(fun);
	    fun->attach_as_word(यह, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = यह;
	    sig->id.सूचकांक = नया __vpiDecConst(addr + first_addr.get_मूल्य());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = true;
	    वापस;
      }
}

व्योम compile_var_array(अक्षर*label, अक्षर*name, पूर्णांक last, पूर्णांक first,
		   पूर्णांक msb, पूर्णांक lsb, अक्षर signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last,
                                      signed_flag != 0);

      काष्ठा __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_width = द_असल(msb-lsb) + 1;
      अगर (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = नया vvp_vector4array_aa(arr->vals_width,
						 arr->get_आकार());
      } अन्यथा {
            arr->vals4 = नया vvp_vector4array_sa(arr->vals_width,
						 arr->get_आकार());
      }
      arr->msb.set_मूल्य(msb);
      arr->lsb.set_मूल्य(lsb);

      गिनती_var_arrays += 1;
      गिनती_var_array_शब्दगण += arr->get_आकार();

      मुक्त(label);
      मिटाओ[] name;
}

व्योम compile_var2_array(अक्षर*label, अक्षर*name, पूर्णांक last, पूर्णांक first,
		   पूर्णांक msb, पूर्णांक lsb, bool signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, signed_flag);

      काष्ठा __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.set_मूल्य(msb);
      arr->lsb.set_मूल्य(lsb);
      arr->vals_width = द_असल(msb-lsb) + 1;

      निश्चित(! arr->nets);
      अगर (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = नया vvp_darray_atom<int8_t>(arr->get_आकार());
      } अन्यथा अगर (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = नया vvp_darray_atom<uint8_t>(arr->get_आकार());
      } अन्यथा अगर (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = नया vvp_darray_atom<int16_t>(arr->get_आकार());
      } अन्यथा अगर (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = नया vvp_darray_atom<uint16_t>(arr->get_आकार());
      } अन्यथा अगर (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = नया vvp_darray_atom<int32_t>(arr->get_आकार());
      } अन्यथा अगर (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = नया vvp_darray_atom<uint32_t>(arr->get_आकार());
      } अन्यथा अगर (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = नया vvp_darray_atom<int64_t>(arr->get_आकार());
      } अन्यथा अगर (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = नया vvp_darray_atom<uint64_t>(arr->get_आकार());
      } अन्यथा {
	    arr->vals = नया vvp_darray_vec2(arr->get_आकार(), arr->vals_width);
      }
      गिनती_var_arrays += 1;
      गिनती_var_array_शब्दगण += arr->get_आकार();

      मुक्त(label);
      मिटाओ[] name;
}

व्योम compile_real_array(अक्षर*label, अक्षर*name, पूर्णांक last, पूर्णांक first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, true);

      काष्ठा __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = नया vvp_darray_real(arr->get_आकार());
      arr->vals_width = 1;

      गिनती_real_arrays += 1;
      गिनती_real_array_शब्दगण += arr->get_आकार();

      मुक्त(label);
      मिटाओ[] name;
}

व्योम compile_string_array(अक्षर*label, अक्षर*name, पूर्णांक last, पूर्णांक first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, true);

      काष्ठा __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = नया vvp_darray_string(arr->get_आकार());
      arr->vals_width = 1;

      गिनती_real_arrays += 1;
      गिनती_real_array_शब्दगण += arr->get_आकार();

      मुक्त(label);
      मिटाओ[] name;
}

व्योम compile_object_array(अक्षर*label, अक्षर*name, पूर्णांक last, पूर्णांक first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, true);

      काष्ठा __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = नया vvp_darray_object(arr->get_आकार());
      arr->vals_width = 1;

      गिनती_real_arrays += 1;
      गिनती_real_array_शब्दगण += arr->get_आकार();

      मुक्त(label);
      मिटाओ[] name;
}

व्योम compile_net_array(अक्षर*label, अक्षर*name, पूर्णांक last, पूर्णांक first)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, first, last, false);

      काष्ठा __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(सुस्मृति(arr->get_आकार(), माप(vpiHandle)));

      गिनती_net_arrays += 1;
      गिनती_net_array_शब्दगण += arr->get_आकार();

      मुक्त(label);
      मिटाओ[] name;
}

श्रेणी vvp_fun_arrayport  : खुला vvp_net_fun_t {

    खुला:
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, दीर्घ addr);
      ~vvp_fun_arrayport() override;

      भव व्योम check_word_change(अचिन्हित दीर्घ addr) = 0;

    रक्षित:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      अचिन्हित दीर्घ addr_;

      मित्र व्योम array_attach_port(vvp_array_t, vvp_fun_arrayport*);
      मित्र व्योम __vpiArray::word_change(अचिन्हित दीर्घ);
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

श्रेणी vvp_fun_arrayport_sa  : खुला vvp_fun_arrayport {

    खुला:
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, दीर्घ addr);
      ~vvp_fun_arrayport_sa() override;

      व्योम check_word_change(अचिन्हित दीर्घ addr) override;

      व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;

    निजी:
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

व्योम vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                                     vvp_context_t)
{
      bool addr_valid_flag;

      चयन (port.port()) {

	  हाल 0: // Address input
	    addr_valid_flag = vector4_to_मूल्य(bit, addr_);
	    अगर (! addr_valid_flag)
		  addr_ = arr_->get_आकार();
	    अगर (vpi_array_is_real(arr_))
		  port.ptr()->send_real(arr_->get_word_r(addr_), 0);
	    अन्यथा
		  port.ptr()->send_vec4(arr_->get_word(addr_), 0);

	    अवरोध;

	  शेष:
	    ख_लिखो(मानक_निकास, "XXXX write ports not implemented.\n");
	    निश्चित(0);
      }
}

व्योम vvp_fun_arrayport_sa::check_word_change(अचिन्हित दीर्घ addr)
{
      अगर (addr != addr_) वापस;

      अगर (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr_), 0);
      } अन्यथा {
	    net_->send_vec4(arr_->get_word(addr_), 0);
      }
}

श्रेणी vvp_fun_arrayport_aa  : खुला vvp_fun_arrayport, खुला automatic_hooks_s {

    खुला:
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, दीर्घ addr);
      ~vvp_fun_arrayport_aa() override;

      व्योम alloc_instance(vvp_context_t context) override;
      व्योम reset_instance(vvp_context_t context) override;
#अगर_घोषित CHECK_WITH_VALGRIND
      व्योम free_instance(vvp_context_t context) override;
#पूर्ण_अगर

      व्योम check_word_change(अचिन्हित दीर्घ addr) override;

      व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                     vvp_context_t context) override;

    निजी:
      व्योम check_word_change_(अचिन्हित दीर्घ addr, vvp_context_t context);

      __vpiScope*context_scope_;
      अचिन्हित context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(यह, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, दीर्घ addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(यह, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

व्योम vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      अचिन्हित दीर्घ*addr = नया अचिन्हित दीर्घ;
      vvp_set_context_item(context, context_idx_, addr);

      *addr = addr_;
}

व्योम vvp_fun_arrayport_aa::reset_instance(vvp_context_t context)
{
      अचिन्हित दीर्घ*addr = static_cast<अचिन्हित दीर्घ*>
            (vvp_get_context_item(context, context_idx_));

      *addr = addr_;
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम vvp_fun_arrayport_aa::free_instance(vvp_context_t context)
{
      अचिन्हित दीर्घ*addr = static_cast<अचिन्हित दीर्घ*>
            (vvp_get_context_item(context, context_idx_));
      मिटाओ addr;
}
#पूर्ण_अगर

व्योम vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                                     vvp_context_t context)
{
      अगर (context) {
            अचिन्हित दीर्घ*addr = static_cast<अचिन्हित दीर्घ*>
                  (vvp_get_context_item(context, context_idx_));

            bool addr_valid_flag;

            चयन (port.port()) {

                हाल 0: // Address input
                  addr_valid_flag = vector4_to_मूल्य(bit, *addr);
                  अगर (! addr_valid_flag) *addr = arr_->get_आकार();
                  अगर (vpi_array_is_real(arr_)) {
			port.ptr()->send_real(arr_->get_word_r(*addr),
					      context);
                  } अन्यथा {
			port.ptr()->send_vec4(arr_->get_word(*addr),
					      context);
                  }
                  अवरोध;

                शेष:
                  ख_लिखो(मानक_निकास, "XXXX write ports not implemented.\n");
                  निश्चित(0);
            }
      } अन्यथा {
            context = context_scope_->live_contexts;
            जबतक (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_अग्र_context(context);
            }
      }
}

व्योम vvp_fun_arrayport_aa::check_word_change_(अचिन्हित दीर्घ addr,
                                              vvp_context_t context)
{
      स्थिर अचिन्हित दीर्घ*port_addr = static_cast<अचिन्हित दीर्घ*>
            (vvp_get_context_item(context, context_idx_));

      अगर (addr != *port_addr)
	    वापस;

      अगर (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr), context);
      } अन्यथा {
	    net_->send_vec4(arr_->get_word(addr), context);
      }
}

व्योम vvp_fun_arrayport_aa::check_word_change(अचिन्हित दीर्घ addr)
{
      अगर (arr_->get_scope()->is_automatic()) {
            निश्चित(vthread_get_wt_context());
            check_word_change_(addr, vthread_get_wt_context());
      } अन्यथा {
            vvp_context_t context = context_scope_->live_contexts;
            जबतक (context) {
                  check_word_change_(addr, context);
                  context = vvp_get_अग्र_context(context);
            }
      }
}

अटल व्योम array_attach_port(vvp_array_t array, vvp_fun_arrayport*fun)
{
      निश्चित(fun->अग्र == 0);
      fun->अग्र = array->ports_;
      array->ports_ = fun;
      अगर (!array->get_scope()->is_automatic() &&
          (array->vals4 || array->vals)) {
              /* propagate initial values for variable arrays */
            अगर (!vpi_array_is_real(array)) {
                  vvp_bit4_t init;
                  अगर (array->vals4)
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

श्रेणी array_word_मूल्य_callback : खुला मूल्य_callback {
    खुला:
      अंतरभूत explicit array_word_मूल्य_callback(p_cb_तथ्य तथ्य, दीर्घ addr)
      : मूल्य_callback(तथ्य), word_addr(addr)
      { }

    खुला:
      दीर्घ word_addr;
};

व्योम __vpiArray::word_change(अचिन्हित दीर्घ addr)
{
      क्रम (vvp_fun_arrayport*cur = ports_; cur; cur = cur->अग्र)
	    cur->check_word_change(addr);

	// Run callbacks attached to the array itself.
      काष्ठा __vpiCallback *अग्र_2 = vpi_callbacks;
      काष्ठा __vpiCallback *prev = 0;

      जबतक (अग्र_2) {
	    array_word_मूल्य_callback*cur = dynamic_cast<array_word_मूल्य_callback*>(अग्र_2);
	    अग्र_2 = cur->अग्र_2;

	      // Skip callbacks that are not for me. -1 is for every element.
	    अगर (cur->word_addr != (दीर्घ)addr && cur->word_addr != -1) {
		  prev = cur;
		  जारी;
	    }

	      // For whole array callbacks we need to set the index.
	    अगर (cur->word_addr == -1) {
		  cur->cb_तथ्य.सूचकांक = (PLI_INT32) ((पूर्णांक)addr +
						    first_addr.get_मूल्य());
	    }

	    अगर (cur->cb_तथ्य.cb_rtn != 0) {
		  अगर (cur->test_मूल्य_callback_ready()) {
			अगर (cur->cb_तथ्य.value) {
			      अगर (vpi_array_is_real(यह)) {
				    द्विगुन val = 0.0;
				    अगर (addr < vals->get_आकार())
					  vals->get_word(addr, val);
				    vpip_real_get_मूल्य(val, cur->cb_तथ्य.value);
			      } अन्यथा अगर (vals4) {
				    vpip_vec4_get_मूल्य(vals4->get_word(addr),
							vals_width,
							signed_flag,
							cur->cb_तथ्य.value);
			      } अन्यथा अगर (dynamic_cast<vvp_darray_atom<int8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int64_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint64_t>*>(vals)
				      || dynamic_cast<vvp_darray_vec2*>(vals)) {
				    vvp_vector4_t val;
				    अगर (addr < vals->get_आकार())
					  vals->get_word(addr, val);
				    vpip_vec4_get_मूल्य(val,
							vals_width,
							signed_flag,
							cur->cb_तथ्य.value);
			      } अन्यथा {
			            निश्चित(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } अन्यथा अगर (prev == 0) {

		  vpi_callbacks = अग्र_2;
		  cur->अग्र_2 = 0;
		  मिटाओ cur;

	    } अन्यथा {
		  निश्चित(prev->अग्र_2 == cur);
		  prev->अग्र_2 = अग्र_2;
		  cur->अग्र_2 = 0;
		  मिटाओ cur;
	    }
      }
}

श्रेणी array_resolv_list_t : खुला resolv_list_s {

    खुला:
      explicit array_resolv_list_t(अक्षर*lab) : resolv_list_s(lab) {
	    array = 0;
      }

      vvp_array_t*array;
      bool resolve(bool mes) override;

    निजी:
};

bool array_resolv_list_t::resolve(bool mes)
{
      *array = array_find(label());
      अगर (*array == 0) {
	    निश्चित(!mes);
	    वापस false;
      }
      वापस true;
}

श्रेणी array_port_resolv_list_t : खुला resolv_list_s {

    खुला:
      explicit array_port_resolv_list_t(अक्षर* lab, bool use_addr__,
                                        दीर्घ addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      bool use_addr;
      दीर्घ addr;
      bool resolve(bool mes) override;

    निजी:
};

array_port_resolv_list_t::array_port_resolv_list_t(अक्षर *lab, bool use_addr__,
                                                   दीर्घ addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      अगर (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      अन्यथा
            context_scope = 0;
      ptr = नया vvp_net_t;
}

bool array_port_resolv_list_t::resolve(bool mes)
{
      vvp_array_t mem = array_find(label());
      अगर (mem == 0) {
	    निश्चित(mem || !mes);
	    वापस false;
      }

      vvp_fun_arrayport*fun;
      अगर (use_addr)
            अगर (context_scope)
                  fun = नया vvp_fun_arrayport_aa(context_scope, mem, ptr, addr);
            अन्यथा
                  fun = नया vvp_fun_arrayport_sa(mem, ptr, addr);
      अन्यथा
            अगर (context_scope)
                  fun = नया vvp_fun_arrayport_aa(context_scope, mem, ptr);
            अन्यथा
                  fun = नया vvp_fun_arrayport_sa(mem, ptr);
      ptr->fun = fun;

      array_attach_port(mem, fun);

      वापस true;
}

श्रेणी array_word_part_callback : खुला array_word_मूल्य_callback {
    खुला:
      explicit array_word_part_callback(p_cb_तथ्य तथ्य, दीर्घ addr);
      ~array_word_part_callback() override;

      array_word_part_callback(स्थिर array_word_part_callback&) = मिटाओ;
      array_word_part_callback& चालक=(स्थिर array_word_part_callback&) = मिटाओ;

      bool test_मूल्य_callback_ready(व्योम) override;

    निजी:
      अक्षर*मूल्य_bits;
};

array_word_part_callback::array_word_part_callback(p_cb_तथ्य तथ्य, दीर्घ addr)
: array_word_मूल्य_callback(तथ्य, addr)
{
	// Get the initial value of the part, to use as a reference.
      काष्ठा __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(तथ्य->obj);
      s_vpi_मूल्य tmp_मूल्य;
      tmp_मूल्य.format = vpiBinStrVal;
      apvword->vpi_get_मूल्य(&tmp_मूल्य);

      मूल्य_bits = नया अक्षर[apvword->part_wid+1];

      स_नकल(मूल्य_bits, tmp_मूल्य.value.str, apvword->part_wid);
      मूल्य_bits[apvword->part_wid] = 0;
}

array_word_part_callback::~array_word_part_callback()
{
      मिटाओ[]मूल्य_bits;
}

bool array_word_part_callback::test_मूल्य_callback_ready(व्योम)
{
      काष्ठा __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_तथ्य.obj);
      निश्चित(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_मूल्य tmp_मूल्य;
      tmp_मूल्य.format = vpiBinStrVal;
      apvword->vpi_get_मूल्य(&tmp_मूल्य);

      अगर (स_भेद(मूल्य_bits, tmp_मूल्य.value.str, apvword->part_wid) == 0)
	    वापस false;

      स_नकल(मूल्य_bits, tmp_मूल्य.value.str, apvword->part_wid);
      वापस true;

}

मूल्य_callback*vpip_array_word_change(p_cb_तथ्य तथ्य)
{
      काष्ठा __vpiArray*parent = 0;
      array_word_मूल्य_callback*cbh = 0;
      अगर (स्थिर काष्ठा __vpiArrayWord*word = array_var_word_from_handle(तथ्य->obj)) {
	    parent = static_cast<__vpiArray*>(word->get_parent());
	    अचिन्हित addr = word->get_सूचकांक();
	    cbh = नया array_word_मूल्य_callback(तथ्य, addr);

      } अन्यथा अगर (काष्ठा __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(तथ्य->obj)) {
	    parent = tword->array;
	    cbh = नया array_word_मूल्य_callback(तथ्य, tword->address);

      } अन्यथा अगर (काष्ठा __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(तथ्य->obj)) {
	    parent = apvword->array;
	    cbh = नया array_word_part_callback(तथ्य, apvword->word_sel);
      }

      निश्चित(cbh);
      निश्चित(parent);
      cbh->अग्र_2 = parent->vpi_callbacks;
      parent->vpi_callbacks = cbh;

      वापस cbh;
}

मूल्य_callback* vpip_array_change(p_cb_तथ्य तथ्य)
{
      array_word_मूल्य_callback*cbh = नया array_word_मूल्य_callback(तथ्य, -1);
      निश्चित(तथ्य->obj);

      काष्ठा __vpiArray*arr = dynamic_cast<__vpiArray*>(तथ्य->obj);
      cbh->अग्र_2 = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      वापस cbh;
}

व्योम compile_array_port(अक्षर*label, अक्षर*array, अक्षर*addr)
{
      array_port_resolv_list_t*resolv_mem
	    = नया array_port_resolv_list_t(array, false, 0);

      define_functor_प्रतीक(label, resolv_mem->ptr);
      मुक्त(label);
	// Connect the port-0 input as the address.
      निवेश_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

व्योम compile_array_port(अक्षर*label, अक्षर*array, दीर्घ addr)
{
      array_port_resolv_list_t*resolv_mem
	    = नया array_port_resolv_list_t(array, true, addr);

      define_functor_प्रतीक(label, resolv_mem->ptr);
      मुक्त(label);

      resolv_submit(resolv_mem);
}

व्योम compile_array_alias(अक्षर*label, अक्षर*name, अक्षर*src)
{
      vvp_array_t mem = array_find(src);
      निश्चित(mem);

      काष्ठा __vpiArray*obj = नया __vpiArray;

      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_गिनती = mem->array_गिनती;
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
      obj->vals_शब्दगण = mem->vals_शब्दगण;

      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

      निश्चित(array_table);
      निश्चित(!array_find(label));
      array_table->sym_set_मूल्य(label, obj);

      compile_vpi_प्रतीक(label, obj);
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
      काष्ठा __vpiArrayVthrA*obj = नया __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = नया array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      obj->address = addr;

      वापस obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(अक्षर*label, अक्षर*प्रतीक)
{
      काष्ठा __vpiArrayVthrA*obj = नया __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = नया array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      compile_vpi_lookup(&obj->address_handle, प्रतीक);
      obj->address = 0;

      वापस obj;
}

vpiHandle vpip_make_vthr_A(अक्षर*label, vpiHandle handle)
{
      काष्ठा __vpiArrayVthrA*obj = नया __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = नया array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = handle;
      obj->address = 0;

      वापस obj;
}

vpiHandle vpip_make_vthr_APV(अक्षर*label, अचिन्हित सूचकांक, अचिन्हित bit, अचिन्हित wid)
{
      काष्ठा __vpiArrayVthrAPV*obj = नया __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = नया array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->word_sel = सूचकांक;
      obj->part_bit = bit;
      obj->part_wid = wid;

      वापस obj;
}

व्योम compile_array_cleanup(व्योम)
{
      मिटाओ array_table;
      array_table = 0;
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम memory_delete(vpiHandle item)
{
      काष्ठा __vpiArray*arr = static_cast<काष्ठा __vpiArray*>(item);
      अगर (arr->vals_शब्दगण) मिटाओ [] (arr->vals_शब्दगण-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      मिटाओ arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      मिटाओ arr->vals;

      अगर (arr->nets) {
	    क्रम (अचिन्हित idx = 0; idx < arr->get_आकार(); idx += 1) {
		  अगर (काष्ठा __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.सूचकांक);
		    /* These should only be the real words. */
		  } अन्यथा {
			निश्चित(arr->nets[idx]->get_type_code() ==
			       vpiRealVar);
			काष्ठा __vpiRealVar *sigr = static_cast<काष्ठा __vpiRealVar *>
			                            (arr->nets[idx]);
			constant_delete(sigr->id.सूचकांक);
// Why are only the real words still here?
			मिटाओ arr->nets[idx];
		  }
	    }
	    मुक्त(arr->nets);
      }

      जबतक (arr->vpi_callbacks) {
	    काष्ठा __vpiCallback*tmp = arr->vpi_callbacks->अग्र_2;
	    मिटाओ arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      मिटाओ arr;
}

व्योम A_delete(vpiHandle item)
{
      काष्ठा __vpiArrayVthrA*obj = static_cast<काष्ठा __vpiArrayVthrA*>(item);
      अगर (obj->address_handle) {
	    चयन (obj->address_handle->get_type_code()) {
		हाल vpiMemoryWord:
		  अगर (vpi_get(_vpiFromThr, obj->address_handle) == _vpi_at_A) {
			A_delete(obj->address_handle);
		  }
		  अवरोध;
		हाल vpiPartSelect:
		  निश्चित(vpi_get(_vpiFromThr, obj->address_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->address_handle);
		  अवरोध;
	    }
      }

      मिटाओ obj;
}

व्योम APV_delete(vpiHandle item)
{
      काष्ठा __vpiArrayVthrAPV*obj = static_cast<काष्ठा __vpiArrayVthrAPV*>(item);
      मिटाओ obj;
}
#पूर्ण_अगर
