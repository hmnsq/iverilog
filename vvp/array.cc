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
#અન્તર્ભાવ  "vvp_cleanup.h"
#endif
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <iostream>
# include  "compile.h"
# include  <cassert>
# include  "ivl_alloc.h"

using નામાકાશ std;

unsigned long ગણતરી_net_arrays = 0;
unsigned long ગણતરી_net_array_શબ્દો = 0;
unsigned long ગણતરી_var_arrays = 0;
unsigned long ગણતરી_var_array_શબ્દો = 0;
unsigned long ગણતરી_real_arrays = 0;
unsigned long ગણતરી_real_array_શબ્દો = 0;

સ્થિર પ્રતીક_map_s<સંરચના __vpiArray>* array_table =0;

વર્ગ vvp_fun_arrayport;
સ્થિર શૂન્ય array_attach_port(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_find(અચલ અક્ષર*label)
{
      જો (array_table == 0)
	    પાછા 0;

      vvp_array_t v = array_table->sym_get_કિંમત(label);
      પાછા v;
}

સંરચના __vpiArrayVthrA : જાહેર __vpiHandle {
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override { પાછા vpiMemoryWord; }
      પૂર્ણાંક vpi_get(પૂર્ણાંક code) override;
      અક્ષર* vpi_get_str(પૂર્ણાંક code) override;
      શૂન્ય vpi_get_કિંમત(p_vpi_કિંમત val) override;
      vpiHandle vpi_put_કિંમત(p_vpi_કિંમત val, પૂર્ણાંક flags) override;
      vpiHandle vpi_handle(પૂર્ણાંક code) override;

      સંરચના __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle address_handle;
	// If wid==0, then address is the address into the array.
      unsigned address;

      unsigned get_address() અચલ
      {
	    જો (address_handle) {
		  s_vpi_કિંમત vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpiVectorVal;
		  address_handle->vpi_get_કિંમત(&vp);
		  પૂર્ણાંક શબ્દો = (address_handle->vpi_get(vpiSize)-1)/32 + 1;
		  માટે(પૂર્ણાંક idx = 0; idx < શબ્દો; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			જો (vp.value.vector[idx].bval != 0) પાછા UINT_MAX;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  address_handle->vpi_get_કિંમત(&vp);
		  પાછા vp.value.integer;
	    }

	    પાછા address;
      }
};


સંરચના __vpiArrayVthrAPV : જાહેર __vpiHandle {
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override { પાછા vpiPartSelect; }
      પૂર્ણાંક vpi_get(પૂર્ણાંક code) override;
      અક્ષર* vpi_get_str(પૂર્ણાંક code) override;
      શૂન્ય vpi_get_કિંમત(p_vpi_કિંમત val) override;
      vpiHandle vpi_handle(પૂર્ણાંક code) override;

      સંરચના __vpiArray*array;
      unsigned word_sel;
      unsigned part_bit;
      unsigned part_wid;
};

તાર્કિક is_net_array(vpiHandle obj)
{
      સંરચના __vpiArray*rfp = dynamic_cast<__vpiArray*> (obj);
      પ્રતિજ્ઞા(rfp);

      જો (rfp->nets != 0) પાછા સાચું;
      પાછા ખોટું;
}

// This function return true if the underlying array words are real.
સ્થિર તાર્કિક vpi_array_is_real(અચલ vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      જો (arr->vals4 != 0)  // A bit based variable/register array.
	    પાછા ખોટું;

      જો (dynamic_cast<vvp_darray_real*> (arr->vals))
	    પાછા સાચું;

      જો (arr->vals != 0)
	    પાછા ખોટું;

	// This must be a net array so look at element 0 to find the type.
      પ્રતિજ્ઞા(arr->nets != 0);
      પ્રતિજ્ઞા(arr->get_size() > 0);
      અચલ સંરચના __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      જો (rsig) {
	    પાછા સાચું;
      }

      પાછા ખોટું;
}

સ્થિર તાર્કિક vpi_array_is_string(અચલ vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      જો (arr->vals4 != 0)  // A bit based variable/register array.
	    પાછા ખોટું;

      જો (dynamic_cast<vvp_darray_string*> (arr->vals))
	    પાછા સાચું;

      પાછા ખોટું;
}

પૂર્ણાંક __vpiArray::get_word_size() અચલ
{
      unsigned width;

      પ્રતિજ્ઞા(get_size() > 0);
	/* For a net array we need to get the width from the first element. */
      જો (nets) {
	    પ્રતિજ્ઞા(vals4 == 0 && vals == 0);
	    અચલ સંરચના __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    પ્રતિજ્ઞા(vsig);
	    width = vpip_size(vsig);
	/* For a variable array we can get the width from vals_width. */
      } નહીંતર {
	    પ્રતિજ્ઞા(vals4 || vals);
	    width = vals_width;
      }

      પાછા width;
}

અક્ષર*__vpiArray::get_word_str(સંરચના __vpiArrayWord*word, પૂર્ણાંક code)
{
      unsigned index = word->get_index();

      જો (code == vpiFile) {  // Not implemented for now!
	    પાછા simple_set_rbuf_str(file_names[0]);
      }

      અક્ષર sidx [64];
      snprintf(sidx, 63, "%d", (પૂર્ણાંક)index + first_addr.get_કિંમત());
      પાછા generic_get_str(code, get_scope(), name, sidx);
}

શૂન્ય __vpiArray::get_word_કિંમત(સંરચના __vpiArrayWord*word, p_vpi_કિંમત vp)
{
      unsigned index = word->get_index();

    // Determine the appropriate format (The Verilog PLI Handbook 5.2.10)
      જો(vp->format == vpiObjTypeVal) {
          જો(vpi_array_is_real(આ))
              vp->format = vpiRealVal;
          નહીંતર જો(vpi_array_is_string(આ))
              vp->format = vpiStringVal;
          નહીંતર
              vp->format = vpiIntVal;
      }

      જો(vals4) {
          vpip_vec4_get_કિંમત(vals4->get_word(index),
                              vals4->width(), signed_flag, vp);
      } નહીંતર જો(vals) {
          પસંદગી(vp->format) {
            સ્થિતિ vpiBinStrVal:
            સ્થિતિ vpiOctStrVal:
            સ્થિતિ vpiDecStrVal:
            સ્થિતિ vpiHexStrVal:
            સ્થિતિ vpiScalarVal:
            સ્થિતિ vpiIntVal:
            સ્થિતિ vpiVectorVal:
            {
                vvp_vector4_t v;
                vals->get_word(index, v);
                vpip_vec4_get_કિંમત(v, vals_width, signed_flag, vp);
            }
            તોડો;

            સ્થિતિ vpiRealVal:
            {
                દ્વિગુણ d;
                vals->get_word(index, d);
                vpip_real_get_કિંમત(d, vp);
            }
            તોડો;

            સ્થિતિ vpiStringVal:
            {
                string s;
                vals->get_word(index, s);
                vpip_string_get_કિંમત(s, vp);
            }
            તોડો;

            મૂળભૂત:
                fprintf(stderr, "vpi sorry: format is not implemented\n");
                પ્રતિજ્ઞા(ખોટું);
          }
      }
}

શૂન્ય __vpiArray::put_word_કિંમત(સંરચના __vpiArrayWord*word, p_vpi_કિંમત vp, પૂર્ણાંક)
{
      unsigned index = word->get_index();
      vvp_vector4_t val = vec4_from_vpi_કિંમત(vp, vals_width);
      set_word(index, 0, val);
}

vpiHandle __vpiArray::get_iter_index(સંરચના __vpiArrayIterator*, પૂર્ણાંક idx)
{
      જો (nets) પાછા nets[idx];

      પ્રતિજ્ઞા(vals4 || vals);

      જો (vals_શબ્દો == 0) make_vals_શબ્દો();

      પાછા &(vals_શબ્દો[idx].as_word);
}

પૂર્ણાંક __vpiArray::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpi_લીટી_No:
	    પાછા 0; // Not implemented for now!

	  સ્થિતિ vpiSize:
	    પાછા get_size();

	  સ્થિતિ vpiAutomatic:
	    પાછા scope->is_automatic()? 1 : 0;

	  મૂળભૂત:
	    પાછા 0;
      }
}

અક્ષર* __vpiArray::vpi_get_str(પૂર્ણાંક code)
{
      જો (code == vpiFile) {  // Not implemented for now!
            પાછા simple_set_rbuf_str(file_names[0]);
      }

      પાછા generic_get_str(code, scope, name, ખાલી);
}

vpiHandle __vpiArray::vpi_handle(પૂર્ણાંક code)
{
      પસંદગી (code) {
          સ્થિતિ vpi_ડાબું_Range:
            જો (swap_addr) પાછા &last_addr;
            નહીંતર પાછા &first_addr;

          સ્થિતિ vpi_જમણું_Range:
            જો (swap_addr) પાછા &first_addr;
            નહીંતર પાછા &last_addr;

	  સ્થિતિ vpiScope:
	    પાછા scope;

	  સ્થિતિ vpiModule:
	    પાછા vpip_module(scope);
      }

      પાછા 0;
}

/*
* VPI code passes indices that are not yet converted to canonical
* form, so this index function does it here.
*/
vpiHandle __vpiArray::vpi_index(પૂર્ણાંક index)
{
      index -= first_addr.get_કિંમત();
      જો (index >= (long) get_size())
	    પાછા 0;
      જો (index < 0)
	    પાછા 0;

      જો (nets != 0) {
	    પાછા nets[index];
      }

      જો (vals_શબ્દો == 0)
	    make_vals_શબ્દો();

      પાછા &(vals_શબ્દો[index].as_word);
}

પૂર્ણાંક __vpiArrayWord::as_word_t::vpi_get(પૂર્ણાંક code)
{
      સંરચના __vpiArrayWord*obj = array_var_word_from_handle(આ);
      પ્રતિજ્ઞા(obj);
      સંરચના __vpiArrayBase*my_parent = obj->get_parent();
      t_vpi_કિંમત val;

      પસંદગી (code) {
	  સ્થિતિ vpi_લીટી_No:
	    પાછા 0; // Not implemented for now!

	  સ્થિતિ vpiSize:
	    પાછા my_parent->get_word_size();

	  સ્થિતિ vpi_ડાબું_Range:
            val.format = vpiIntVal;
	    my_parent->get_ડાબું_range()->vpi_get_કિંમત(&val);
            પ્રતિજ્ઞા(val.format == vpiIntVal);
            પાછા val.value.integer;

	  સ્થિતિ vpi_જમણું_Range:
            val.format = vpiIntVal;
	    my_parent->get_જમણું_range()->vpi_get_કિંમત(&val);
            પ્રતિજ્ઞા(val.format == vpiIntVal);
            પાછા val.value.integer;

	  સ્થિતિ vpiIndex:
	    {
		  પૂર્ણાંક base_offset = 0;
		  સંરચના __vpiArray*base = dynamic_cast<__vpiArray*> (my_parent);
		  જો (base) {
			val.format = vpiIntVal;
			base->first_addr.vpi_get_કિંમત(&val);
			base_offset += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_index.vpi_get_કિંમત(&val);
		  પ્રતિજ્ઞા(val.format == vpiIntVal);
		  પાછા val.value.integer + base_offset;
	    }

	  સ્થિતિ vpiAutomatic:
	    પાછા my_parent->get_scope()->is_automatic()? 1 : 0;

#જો defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  સ્થિતિ _vpiFromThr:
	    પાછા _vpiNoThr;
#endif

	  મૂળભૂત:
	    પાછા 0;
      }
}


પૂર્ણાંક __vpiArrayVthrA::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpi_લીટી_No:
	    પાછા 0; // Not implemented for now!

	  સ્થિતિ vpiSize:
	    પાછા array->get_word_size();

	  સ્થિતિ vpi_ડાબું_Range:
	    પાછા array->msb.get_કિંમત();

	  સ્થિતિ vpi_જમણું_Range:
	    પાછા array->lsb.get_કિંમત();

	  સ્થિતિ vpiIndex:
	    પાછા (પૂર્ણાંક)get_address() + array->first_addr.get_કિંમત();

	  સ્થિતિ vpiAutomatic:
	    પાછા array->get_scope()->is_automatic() ? 1 : 0;

#જો defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  સ્થિતિ _vpiFromThr:
	    પાછા _vpi_at_A;
#endif

	  // If address_handle is not zero we definitely have a
	  // variable.
	  સ્થિતિ vpiConstantSelect:
	    પાછા address_handle == 0;

	  મૂળભૂત:
	    પાછા 0;
      }
}

અક્ષર* __vpiArrayVthrA::vpi_get_str(પૂર્ણાંક code)
{
      જો (code == vpiFile) {  // Not implemented for now!
            પાછા simple_set_rbuf_str(file_names[0]);
      }

      અક્ષર sidx [64];
      snprintf(sidx, 63, "%d", (પૂર્ણાંક)get_address() + array->first_addr.get_કિંમત());
      પાછા generic_get_str(code, array->get_scope(), array->name, sidx);
}

શૂન્ય __vpiArrayVthrA::vpi_get_કિંમત(p_vpi_કિંમત vp)
{
      પ્રતિજ્ઞા(array);

      unsigned index = get_address();
      જો (vpi_array_is_real(array)) {
	    દ્વિગુણ tmp = array->get_word_r(index);
	    vpip_real_get_કિંમત(tmp, vp);
      } નહીંતર જો (vpi_array_is_string(array)) {
	    string tmp = array->get_word_str(index);
	    vpip_string_get_કિંમત(tmp, vp);
      } નહીંતર {
	    vvp_vector4_t tmp = array->get_word(index);
	    unsigned width = array->get_word_size();
	    vpip_vec4_get_કિંમત(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_કિંમત(p_vpi_કિંમત vp, પૂર્ણાંક)
{
      unsigned index = get_address();

      પ્રતિજ્ઞા(array);
      પ્રતિજ્ઞા(index < array->get_size());

      જો (vpi_array_is_real(array)) {
	    દ્વિગુણ val = real_from_vpi_કિંમત(vp);
	    array->set_word(index, val);
      } નહીંતર {
	    unsigned width = array->get_word_size();
	    vvp_vector4_t val = vec4_from_vpi_કિંમત(vp, width);
	    array->set_word(index, 0, val);
      }

      પાછા આ;
}

vpiHandle __vpiArrayVthrA::vpi_handle(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiIndex:
	    તોડો;  // Not implemented!

	  સ્થિતિ vpi_ડાબું_Range:
	    પાછા &array->msb;

	  સ્થિતિ vpi_જમણું_Range:
	    પાછા &array->lsb;

	  સ્થિતિ vpiParent:
	  સ્થિતિ vpiArray:
	    પાછા array;

	  સ્થિતિ vpiScope:
	    પાછા array->get_scope();

	  સ્થિતિ vpiModule:
	    પાછા vpip_module(array->get_scope());
      }

      પાછા 0;
}


પૂર્ણાંક __vpiArrayVthrAPV::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpi_લીટી_No:
	    પાછા 0; // Not implemented for now!

	  સ્થિતિ vpiSize:
	    પાછા part_wid;

	  સ્થિતિ vpi_ડાબું_Range:
	    પાછા part_bit + part_wid - 1;

	  સ્થિતિ vpi_જમણું_Range:
	    પાછા part_bit;

	  સ્થિતિ vpiAutomatic:
	    પાછા array->get_scope()->is_automatic() ? 1 : 0;

#જો defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  સ્થિતિ _vpiFromThr:
	    પાછા _vpi_at_APV;
#endif

	  સ્થિતિ vpiConstantSelect:
	    પાછા 1;

	  મૂળભૂત:
	    પાછા 0;
      }
}

અક્ષર* __vpiArrayVthrAPV::vpi_get_str(પૂર્ણાંક code)
{
      જો (code == vpiFile) {  // Not implemented for now!
            પાછા simple_set_rbuf_str(file_names[0]);
      }

      અક્ષર sidx [64];
      snprintf(sidx, 63, "%u", word_sel + array->first_addr.get_કિંમત());
      પાછા generic_get_str(code, array->get_scope(), array->name, sidx);
}

શૂન્ય __vpiArrayVthrAPV::vpi_get_કિંમત(p_vpi_કિંમત vp)
{
      પ્રતિજ્ઞા(array);

      unsigned index = word_sel;
      જો (vpi_array_is_real(array)) {
	    દ્વિગુણ tmp = array->get_word_r(index);
	    vpip_real_get_કિંમત(tmp, vp);
      } નહીંતર {
	    vvp_vector4_t tmp = array->get_word(index);
	    tmp = tmp.subvalue(part_bit, part_wid);
	    vpip_vec4_get_કિંમત(tmp, part_wid, ખોટું, vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(પૂર્ણાંક code)
{
      પસંદગી (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  સ્થિતિ vpiParent:
	    પાછા 0;

            // Not part of the Verilog standard. We use this internally.
	  સ્થિતિ vpiArray:
	    પાછા array;

	  સ્થિતિ vpiScope:
	    પાછા array->get_scope();

	  સ્થિતિ vpiModule:
	    પાછા vpip_module(array->get_scope());
      }

      પાછા 0;
}

શૂન્ય __vpiArray::set_word(unsigned address, unsigned part_off, અચલ vvp_vector4_t&val)
{
      જો (address >= get_size())
	    પાછા;

      જો (vals4) {
	    પ્રતિજ્ઞા(nets == 0);
	    જો (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp = vals4->get_word(address);
		  જો ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			પ્રતિજ્ઞા(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals4->set_word(address, tmp);
	    } નહીંતર {
		  vals4->set_word(address, val);
	    }
	    word_change(address);
	    પાછા;
      }

      જો (vals) {
	    પ્રતિજ્ઞા(nets == 0);
	    જો (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp;
		  vals->get_word(address, tmp);
		  જો ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			પ્રતિજ્ઞા(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals->set_word(address, tmp);
	    } નહીંતર {
		  vals->set_word(address, val);
	    }
	    word_change(address);
	    પાછા;
      }

      પ્રતિજ્ઞા(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle word = nets[address];
      અચલ સંરચના __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      પ્રતિજ્ઞા(vsig);

      vsig->ગાંઠ->send_vec4_pv(val, part_off, vpip_size(vsig), 0);
      word_change(address);
}

શૂન્ય __vpiArray::set_word(unsigned address, દ્વિગુણ val)
{
      પ્રતિજ્ઞા(vals != 0);
      પ્રતિજ્ઞા(nets == 0);

      જો (address >= vals->get_size())
	    પાછા;

      vals->set_word(address, val);
      word_change(address);
}

શૂન્ય __vpiArray::set_word(unsigned address, અચલ string&val)
{
      પ્રતિજ્ઞા(vals != 0);
      પ્રતિજ્ઞા(nets == 0);

      જો (address >= vals->get_size())
	    પાછા;

      vals->set_word(address, val);
      word_change(address);
}

શૂન્ય __vpiArray::set_word(unsigned address, અચલ vvp_object_t&val)
{
      પ્રતિજ્ઞા(vals != 0);
      પ્રતિજ્ઞા(nets == 0);

      જો (address >= vals->get_size())
	    પાછા;

      vals->set_word(address, val);
      word_change(address);
}

vvp_vector4_t __vpiArray::get_word(unsigned address)
{
      જો (vals4) {
	    પ્રતિજ્ઞા(nets == 0);
	    પ્રતિજ્ઞા(vals == 0);
	    પાછા vals4->get_word(address);
      }

      જો (vals) {
	    પ્રતિજ્ઞા(nets == 0);
	    પ્રતિજ્ઞા(vals4== 0);
	    જો (address >= vals->get_size())
		  પાછા vvp_vector4_t(vals_width, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_word(address, val);
	    પાછા val;
      }

      પ્રતિજ્ઞા(vals4 == 0);
      પ્રતિજ્ઞા(vals == 0);
      પ્રતિજ્ઞા(nets != 0);

      જો (address >= get_size()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    પ્રતિજ્ઞા(get_size() > 0);
	    vpiHandle word = nets[0];
	    પ્રતિજ્ઞા(word);
	    સંરચના __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
	    પ્રતિજ્ઞા(vsig);
	    vvp_signal_કિંમત*sig = dynamic_cast<vvp_signal_કિંમત*> (vsig->ગાંઠ->fil);
	    પ્રતિજ્ઞા(sig);
	    પાછા vvp_vector4_t(sig->કિંમત_size(), BIT4_X);
      }

      vpiHandle word = nets[address];
      સંરચના __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      પ્રતિજ્ઞા(vsig);
      vvp_signal_કિંમત*sig = dynamic_cast<vvp_signal_કિંમત*> (vsig->ગાંઠ->fil);
      પ્રતિજ્ઞા(sig);

      vvp_vector4_t val;
      sig->vec4_કિંમત(val);
      પાછા val;
}

દ્વિગુણ __vpiArray::get_word_r(unsigned address)
{
      જો (vals) {
	    પ્રતિજ્ઞા(vals4 == 0);
	    પ્રતિજ્ઞા(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    જો (address >= vals->get_size())
		  પાછા 0.0;

	    દ્વિગુણ val;
	    vals->get_word(address, val);
	    પાછા val;
      }

      જો (address >= get_size())
	    પાછા 0.0;

      પ્રતિજ્ઞા(nets);
      vpiHandle word = nets[address];
      સંરચના __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(word);
      પ્રતિજ્ઞા(vsig);
      vvp_signal_કિંમત*sig = dynamic_cast<vvp_signal_કિંમત*> (vsig->net->fil);
      પ્રતિજ્ઞા(sig);

      દ્વિગુણ val = sig->real_કિંમત();
      પાછા val;
}

શૂન્ય __vpiArray::get_word_obj(unsigned address, vvp_object_t&val)
{
      જો (vals) {
	    પ્રતિજ્ઞા(vals4 == 0);
	    પ્રતિજ્ઞા(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    જો (address >= vals->get_size()) {
		  val = vvp_object_t();
		  પાછા;
	    }

	    vals->get_word(address, val);
	    પાછા;
      }

      પ્રતિજ્ઞા(nets);
	// Arrays of string nets not implemented!
      પ્રતિજ્ઞા(0);
      પાછા;
}

string __vpiArray::get_word_str(unsigned address)
{
      જો (vals) {
	    પ્રતિજ્ઞા(vals4 == 0);
	    પ્રતિજ્ઞા(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    જો (address >= vals->get_size())
		  પાછા "";

	    string val;
	    vals->get_word(address, val);
	    પાછા val;
      }

      પ્રતિજ્ઞા(nets);
	// Arrays of string nets not implemented!
      પ્રતિજ્ઞા(0);
      પાછા "";
}

vpiHandle vpip_make_array(અચલ અક્ષર*label, અચલ અક્ષર*name,
                          પૂર્ણાંક first_addr, પૂર્ણાંક last_addr,
                          તાર્કિક signed_flag)
{
      સંરચના __vpiArray*obj = નવું __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      જો (last_addr >= first_addr) {
	    obj->swap_addr = ખોટું;
      } નહીંતર {
	    obj->swap_addr = સાચું;
	    પૂર્ણાંક tmp = last_addr;
	    last_addr = first_addr;
	    first_addr = tmp;
      }
      પ્રતિજ્ઞા(last_addr >= first_addr);
      unsigned array_ગણતરી = last_addr+1-first_addr;

	// For now, treat all arrays as memories. This is not quite
	// correct, as arrays are arrays with memories a special case.
      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_ગણતરી = array_ગણતરી;

      obj->first_addr.set_કિંમત(first_addr);
      obj->last_addr .set_કિંમત(last_addr);

	// Start off now knowing if we are nets or variables.
      obj->nets = 0;
      obj->vals4 = 0;
      obj->vals  = 0;
      obj->vals_width = 0;
      obj->vals_શબ્દો = 0;

	// Initialize (clear) the read-ports list.
      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

	/* Add this symbol to the array_symbols table for later lookup. */
      જો (!array_table)
	    array_table = નવું પ્રતીક_map_s<સંરચના __vpiArray>;

      પ્રતિજ્ઞા(!array_find(label));
      array_table->sym_set_કિંમત(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_પ્રતીક(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_to_current_scope(obj);

      પાછા obj;
}

શૂન્ય __vpiArray::alias_word(unsigned long addr, vpiHandle word, પૂર્ણાંક msb_, પૂર્ણાંક lsb_)
{
      પ્રતિજ્ઞા(msb.get_કિંમત() == msb_);
      પ્રતિજ્ઞા(lsb.get_કિંમત() == lsb_);
      પ્રતિજ્ઞા(addr < get_size());
      પ્રતિજ્ઞા(nets);
      nets[addr] = word;
}

શૂન્ય __vpiArray::attach_word(unsigned addr, vpiHandle word)
{
      પ્રતિજ્ઞા(addr < get_size());
      પ્રતિજ્ઞા(nets);
      nets[addr] = word;

      જો (સંરચના __vpiSignal*sig = dynamic_cast<__vpiSignal*>(word)) {
	    vvp_net_t*net = sig->ગાંઠ;
	    પ્રતિજ્ઞા(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    પ્રતિજ્ઞા(fun);
	    fun->attach_as_word(આ, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = આ;
	    sig->id.index = નવું __vpiDecConst(addr + first_addr.get_કિંમત());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    પાછા;
      }

      જો (સંરચના __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(word)) {
	    vvp_net_t*net = sig->net;
	    પ્રતિજ્ઞા(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    પ્રતિજ્ઞા(fun);
	    fun->attach_as_word(આ, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = આ;
	    sig->id.index = નવું __vpiDecConst(addr + first_addr.get_કિંમત());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = સાચું;
	    પાછા;
      }
}

શૂન્ય compile_var_array(અક્ષર*label, અક્ષર*name, પૂર્ણાંક last, પૂર્ણાંક first,
		   પૂર્ણાંક msb, પૂર્ણાંક lsb, અક્ષર signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last,
                                      signed_flag != 0);

      સંરચના __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_width = labs(msb-lsb) + 1;
      જો (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = નવું vvp_vector4array_aa(arr->vals_width,
						 arr->get_size());
      } નહીંતર {
            arr->vals4 = નવું vvp_vector4array_sa(arr->vals_width,
						 arr->get_size());
      }
      arr->msb.set_કિંમત(msb);
      arr->lsb.set_કિંમત(lsb);

      ગણતરી_var_arrays += 1;
      ગણતરી_var_array_શબ્દો += arr->get_size();

      મુક્ત(label);
      delete[] name;
}

શૂન્ય compile_var2_array(અક્ષર*label, અક્ષર*name, પૂર્ણાંક last, પૂર્ણાંક first,
		   પૂર્ણાંક msb, પૂર્ણાંક lsb, તાર્કિક signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, signed_flag);

      સંરચના __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.set_કિંમત(msb);
      arr->lsb.set_કિંમત(lsb);
      arr->vals_width = labs(msb-lsb) + 1;

      પ્રતિજ્ઞા(! arr->nets);
      જો (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = નવું vvp_darray_atom<int8_t>(arr->get_size());
      } નહીંતર જો (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = નવું vvp_darray_atom<uint8_t>(arr->get_size());
      } નહીંતર જો (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = નવું vvp_darray_atom<int16_t>(arr->get_size());
      } નહીંતર જો (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = નવું vvp_darray_atom<uint16_t>(arr->get_size());
      } નહીંતર જો (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = નવું vvp_darray_atom<int32_t>(arr->get_size());
      } નહીંતર જો (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = નવું vvp_darray_atom<uint32_t>(arr->get_size());
      } નહીંતર જો (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = નવું vvp_darray_atom<int64_t>(arr->get_size());
      } નહીંતર જો (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = નવું vvp_darray_atom<uint64_t>(arr->get_size());
      } નહીંતર {
	    arr->vals = નવું vvp_darray_vec2(arr->get_size(), arr->vals_width);
      }
      ગણતરી_var_arrays += 1;
      ગણતરી_var_array_શબ્દો += arr->get_size();

      મુક્ત(label);
      delete[] name;
}

શૂન્ય compile_real_array(અક્ષર*label, અક્ષર*name, પૂર્ણાંક last, પૂર્ણાંક first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, સાચું);

      સંરચના __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = નવું vvp_darray_real(arr->get_size());
      arr->vals_width = 1;

      ગણતરી_real_arrays += 1;
      ગણતરી_real_array_શબ્દો += arr->get_size();

      મુક્ત(label);
      delete[] name;
}

શૂન્ય compile_string_array(અક્ષર*label, અક્ષર*name, પૂર્ણાંક last, પૂર્ણાંક first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, સાચું);

      સંરચના __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = નવું vvp_darray_string(arr->get_size());
      arr->vals_width = 1;

      ગણતરી_real_arrays += 1;
      ગણતરી_real_array_શબ્દો += arr->get_size();

      મુક્ત(label);
      delete[] name;
}

શૂન્ય compile_object_array(અક્ષર*label, અક્ષર*name, પૂર્ણાંક last, પૂર્ણાંક first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, સાચું);

      સંરચના __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = નવું vvp_darray_object(arr->get_size());
      arr->vals_width = 1;

      ગણતરી_real_arrays += 1;
      ગણતરી_real_array_શબ્દો += arr->get_size();

      મુક્ત(label);
      delete[] name;
}

શૂન્ય compile_net_array(અક્ષર*label, અક્ષર*name, પૂર્ણાંક last, પૂર્ણાંક first)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, first, last, ખોટું);

      સંરચના __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(calloc(arr->get_size(), કદ(vpiHandle)));

      ગણતરી_net_arrays += 1;
      ગણતરી_net_array_શબ્દો += arr->get_size();

      મુક્ત(label);
      delete[] name;
}

વર્ગ vvp_fun_arrayport  : જાહેર vvp_net_fun_t {

    જાહેર:
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport() override;

      આભાસી શૂન્ય check_word_change(unsigned long addr) = 0;

    સુરક્ષિત:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      unsigned long addr_;

      friend શૂન્ય array_attach_port(vvp_array_t, vvp_fun_arrayport*);
      friend શૂન્ય __vpiArray::word_change(unsigned long);
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

વર્ગ vvp_fun_arrayport_sa  : જાહેર vvp_fun_arrayport {

    જાહેર:
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_sa() override;

      શૂન્ય check_word_change(unsigned long addr) override;

      શૂન્ય recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                     vvp_context_t) override;

    ખાનગી:
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

શૂન્ય vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                                     vvp_context_t)
{
      તાર્કિક addr_valid_flag;

      પસંદગી (port.port()) {

	  સ્થિતિ 0: // Address input
	    addr_valid_flag = vector4_to_કિંમત(bit, addr_);
	    જો (! addr_valid_flag)
		  addr_ = arr_->get_size();
	    જો (vpi_array_is_real(arr_))
		  port.ptr()->send_real(arr_->get_word_r(addr_), 0);
	    નહીંતર
		  port.ptr()->send_vec4(arr_->get_word(addr_), 0);

	    તોડો;

	  મૂળભૂત:
	    fprintf(stdout, "XXXX write ports not implemented.\n");
	    પ્રતિજ્ઞા(0);
      }
}

શૂન્ય vvp_fun_arrayport_sa::check_word_change(unsigned long addr)
{
      જો (addr != addr_) પાછા;

      જો (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr_), 0);
      } નહીંતર {
	    net_->send_vec4(arr_->get_word(addr_), 0);
      }
}

વર્ગ vvp_fun_arrayport_aa  : જાહેર vvp_fun_arrayport, જાહેર automatic_hooks_s {

    જાહેર:
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_aa() override;

      શૂન્ય alloc_instance(vvp_context_t context) override;
      શૂન્ય reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      શૂન્ય free_instance(vvp_context_t context) override;
#endif

      શૂન્ય check_word_change(unsigned long addr) override;

      શૂન્ય recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                     vvp_context_t context) override;

    ખાનગી:
      શૂન્ય check_word_change_(unsigned long addr, vvp_context_t context);

      __vpiScope*context_scope_;
      unsigned context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(આ, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(આ, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

શૂન્ય vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      unsigned long*addr = નવું unsigned long;
      vvp_set_context_item(context, context_idx_, addr);

      *addr = addr_;
}

શૂન્ય vvp_fun_arrayport_aa::reset_instance(vvp_context_t context)
{
      unsigned long*addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));

      *addr = addr_;
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય vvp_fun_arrayport_aa::free_instance(vvp_context_t context)
{
      unsigned long*addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));
      delete addr;
}
#endif

શૂન્ય vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                                     vvp_context_t context)
{
      જો (context) {
            unsigned long*addr = static_cast<unsigned long*>
                  (vvp_get_context_item(context, context_idx_));

            તાર્કિક addr_valid_flag;

            પસંદગી (port.port()) {

                સ્થિતિ 0: // Address input
                  addr_valid_flag = vector4_to_કિંમત(bit, *addr);
                  જો (! addr_valid_flag) *addr = arr_->get_size();
                  જો (vpi_array_is_real(arr_)) {
			port.ptr()->send_real(arr_->get_word_r(*addr),
					      context);
                  } નહીંતર {
			port.ptr()->send_vec4(arr_->get_word(*addr),
					      context);
                  }
                  તોડો;

                મૂળભૂત:
                  fprintf(stdout, "XXXX write ports not implemented.\n");
                  પ્રતિજ્ઞા(0);
            }
      } નહીંતર {
            context = context_scope_->live_contexts;
            જ્યારે (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

શૂન્ય vvp_fun_arrayport_aa::check_word_change_(unsigned long addr,
                                              vvp_context_t context)
{
      અચલ unsigned long*port_addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));

      જો (addr != *port_addr)
	    પાછા;

      જો (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr), context);
      } નહીંતર {
	    net_->send_vec4(arr_->get_word(addr), context);
      }
}

શૂન્ય vvp_fun_arrayport_aa::check_word_change(unsigned long addr)
{
      જો (arr_->get_scope()->is_automatic()) {
            પ્રતિજ્ઞા(vthread_get_wt_context());
            check_word_change_(addr, vthread_get_wt_context());
      } નહીંતર {
            vvp_context_t context = context_scope_->live_contexts;
            જ્યારે (context) {
                  check_word_change_(addr, context);
                  context = vvp_get_next_context(context);
            }
      }
}

સ્થિર શૂન્ય array_attach_port(vvp_array_t array, vvp_fun_arrayport*fun)
{
      પ્રતિજ્ઞા(fun->next_ == 0);
      fun->next_ = array->ports_;
      array->ports_ = fun;
      જો (!array->get_scope()->is_automatic() &&
          (array->vals4 || array->vals)) {
              /* propagate initial values for variable arrays */
            જો (!vpi_array_is_real(array)) {
                  vvp_bit4_t init;
                  જો (array->vals4)
                      init = BIT4_X;
                  નહીંતર
                      init = BIT4_0;
                  vvp_vector4_t tmp(array->vals_width, init);
                  schedule_init_propagate(fun->net_, tmp);
            } નહીંતર {
                  schedule_init_propagate(fun->net_, 0.0);
            }
      }
}

વર્ગ array_word_કિંમત_callback : જાહેર કિંમત_callback {
    જાહેર:
      અંતઃરેખ explicit array_word_કિંમત_callback(p_cb_data data, long addr)
      : કિંમત_callback(data), word_addr(addr)
      { }

    જાહેર:
      long word_addr;
};

શૂન્ય __vpiArray::word_change(unsigned long addr)
{
      માટે (vvp_fun_arrayport*cur = ports_; cur; cur = cur->next_)
	    cur->check_word_change(addr);

	// Run callbacks attached to the array itself.
      સંરચના __vpiCallback *next = vpi_callbacks;
      સંરચના __vpiCallback *prev = 0;

      જ્યારે (next) {
	    array_word_કિંમત_callback*cur = dynamic_cast<array_word_કિંમત_callback*>(next);
	    next = cur->next;

	      // Skip callbacks that are not for me. -1 is for every element.
	    જો (cur->word_addr != (long)addr && cur->word_addr != -1) {
		  prev = cur;
		  ચાલુ;
	    }

	      // For whole array callbacks we need to set the index.
	    જો (cur->word_addr == -1) {
		  cur->cb_data.index = (PLI_INT32) ((પૂર્ણાંક)addr +
						    first_addr.get_કિંમત());
	    }

	    જો (cur->cb_data.cb_rtn != 0) {
		  જો (cur->test_કિંમત_callback_ready()) {
			જો (cur->cb_data.value) {
			      જો (vpi_array_is_real(આ)) {
				    દ્વિગુણ val = 0.0;
				    જો (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_real_get_કિંમત(val, cur->cb_data.value);
			      } નહીંતર જો (vals4) {
				    vpip_vec4_get_કિંમત(vals4->get_word(addr),
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } નહીંતર જો (dynamic_cast<vvp_darray_atom<int8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int64_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint64_t>*>(vals)
				      || dynamic_cast<vvp_darray_vec2*>(vals)) {
				    vvp_vector4_t val;
				    જો (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_vec4_get_કિંમત(val,
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } નહીંતર {
			            પ્રતિજ્ઞા(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } નહીંતર જો (prev == 0) {

		  vpi_callbacks = next;
		  cur->next = 0;
		  delete cur;

	    } નહીંતર {
		  પ્રતિજ્ઞા(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}

વર્ગ array_resolv_list_t : જાહેર resolv_list_s {

    જાહેર:
      explicit array_resolv_list_t(અક્ષર*lab) : resolv_list_s(lab) {
	    array = 0;
      }

      vvp_array_t*array;
      તાર્કિક resolve(તાર્કિક mes) override;

    ખાનગી:
};

તાર્કિક array_resolv_list_t::resolve(તાર્કિક mes)
{
      *array = array_find(label());
      જો (*array == 0) {
	    પ્રતિજ્ઞા(!mes);
	    પાછા ખોટું;
      }
      પાછા સાચું;
}

વર્ગ array_port_resolv_list_t : જાહેર resolv_list_s {

    જાહેર:
      explicit array_port_resolv_list_t(અક્ષર* lab, તાર્કિક use_addr__,
                                        long addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      તાર્કિક use_addr;
      long addr;
      તાર્કિક resolve(તાર્કિક mes) override;

    ખાનગી:
};

array_port_resolv_list_t::array_port_resolv_list_t(અક્ષર *lab, તાર્કિક use_addr__,
                                                   long addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      જો (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      નહીંતર
            context_scope = 0;
      ptr = નવું vvp_net_t;
}

તાર્કિક array_port_resolv_list_t::resolve(તાર્કિક mes)
{
      vvp_array_t mem = array_find(label());
      જો (mem == 0) {
	    પ્રતિજ્ઞા(mem || !mes);
	    પાછા ખોટું;
      }

      vvp_fun_arrayport*fun;
      જો (use_addr)
            જો (context_scope)
                  fun = નવું vvp_fun_arrayport_aa(context_scope, mem, ptr, addr);
            નહીંતર
                  fun = નવું vvp_fun_arrayport_sa(mem, ptr, addr);
      નહીંતર
            જો (context_scope)
                  fun = નવું vvp_fun_arrayport_aa(context_scope, mem, ptr);
            નહીંતર
                  fun = નવું vvp_fun_arrayport_sa(mem, ptr);
      ptr->fun = fun;

      array_attach_port(mem, fun);

      પાછા સાચું;
}

વર્ગ array_word_part_callback : જાહેર array_word_કિંમત_callback {
    જાહેર:
      explicit array_word_part_callback(p_cb_data data, long addr);
      ~array_word_part_callback() override;

      array_word_part_callback(અચલ array_word_part_callback&) = delete;
      array_word_part_callback& operator=(અચલ array_word_part_callback&) = delete;

      તાર્કિક test_કિંમત_callback_ready(શૂન્ય) override;

    ખાનગી:
      અક્ષર*કિંમત_bits;
};

array_word_part_callback::array_word_part_callback(p_cb_data data, long addr)
: array_word_કિંમત_callback(data, addr)
{
	// Get the initial value of the part, to use as a reference.
      સંરચના __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj);
      s_vpi_કિંમત tmp_કિંમત;
      tmp_કિંમત.format = vpiBinStrVal;
      apvword->vpi_get_કિંમત(&tmp_કિંમત);

      કિંમત_bits = નવું અક્ષર[apvword->part_wid+1];

      memcpy(કિંમત_bits, tmp_કિંમત.value.str, apvword->part_wid);
      કિંમત_bits[apvword->part_wid] = 0;
}

array_word_part_callback::~array_word_part_callback()
{
      delete[]કિંમત_bits;
}

તાર્કિક array_word_part_callback::test_કિંમત_callback_ready(શૂન્ય)
{
      સંરચના __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_data.obj);
      પ્રતિજ્ઞા(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_કિંમત tmp_કિંમત;
      tmp_કિંમત.format = vpiBinStrVal;
      apvword->vpi_get_કિંમત(&tmp_કિંમત);

      જો (memcmp(કિંમત_bits, tmp_કિંમત.value.str, apvword->part_wid) == 0)
	    પાછા ખોટું;

      memcpy(કિંમત_bits, tmp_કિંમત.value.str, apvword->part_wid);
      પાછા સાચું;

}

કિંમત_callback*vpip_array_word_change(p_cb_data data)
{
      સંરચના __vpiArray*parent = 0;
      array_word_કિંમત_callback*cbh = 0;
      જો (અચલ સંરચના __vpiArrayWord*word = array_var_word_from_handle(data->obj)) {
	    parent = static_cast<__vpiArray*>(word->get_parent());
	    unsigned addr = word->get_index();
	    cbh = નવું array_word_કિંમત_callback(data, addr);

      } નહીંતર જો (સંરચના __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(data->obj)) {
	    parent = tword->array;
	    cbh = નવું array_word_કિંમત_callback(data, tword->address);

      } નહીંતર જો (સંરચના __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj)) {
	    parent = apvword->array;
	    cbh = નવું array_word_part_callback(data, apvword->word_sel);
      }

      પ્રતિજ્ઞા(cbh);
      પ્રતિજ્ઞા(parent);
      cbh->next = parent->vpi_callbacks;
      parent->vpi_callbacks = cbh;

      પાછા cbh;
}

કિંમત_callback* vpip_array_change(p_cb_data data)
{
      array_word_કિંમત_callback*cbh = નવું array_word_કિંમત_callback(data, -1);
      પ્રતિજ્ઞા(data->obj);

      સંરચના __vpiArray*arr = dynamic_cast<__vpiArray*>(data->obj);
      cbh->next = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      પાછા cbh;
}

શૂન્ય compile_array_port(અક્ષર*label, અક્ષર*array, અક્ષર*addr)
{
      array_port_resolv_list_t*resolv_mem
	    = નવું array_port_resolv_list_t(array, ખોટું, 0);

      define_functor_પ્રતીક(label, resolv_mem->ptr);
      મુક્ત(label);
	// Connect the port-0 input as the address.
      આગત_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

શૂન્ય compile_array_port(અક્ષર*label, અક્ષર*array, long addr)
{
      array_port_resolv_list_t*resolv_mem
	    = નવું array_port_resolv_list_t(array, સાચું, addr);

      define_functor_પ્રતીક(label, resolv_mem->ptr);
      મુક્ત(label);

      resolv_submit(resolv_mem);
}

શૂન્ય compile_array_alias(અક્ષર*label, અક્ષર*name, અક્ષર*src)
{
      vvp_array_t mem = array_find(src);
      પ્રતિજ્ઞા(mem);

      સંરચના __vpiArray*obj = નવું __vpiArray;

      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_ગણતરી = mem->array_ગણતરી;
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
      obj->vals_શબ્દો = mem->vals_શબ્દો;

      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

      પ્રતિજ્ઞા(array_table);
      પ્રતિજ્ઞા(!array_find(label));
      array_table->sym_set_કિંમત(label, obj);

      compile_vpi_પ્રતીક(label, obj);
      vpip_attach_to_current_scope(obj);

      મુક્ત(label);
      મુક્ત(name);
      મુક્ત(src);
}

/*
 * &A<label,addr>
 * This represents a VPI handle for an addressed array. This comes
 * from expressions like "label[addr]" where "label" is the array and
 * "addr" is the canonical address of the desired word.
 */
vpiHandle vpip_make_vthr_A(અક્ષર*label, unsigned addr)
{
      સંરચના __vpiArrayVthrA*obj = નવું __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = નવું array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      obj->address = addr;

      પાછા obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(અક્ષર*label, અક્ષર*પ્રતીક)
{
      સંરચના __vpiArrayVthrA*obj = નવું __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = નવું array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      compile_vpi_lookup(&obj->address_handle, પ્રતીક);
      obj->address = 0;

      પાછા obj;
}

vpiHandle vpip_make_vthr_A(અક્ષર*label, vpiHandle handle)
{
      સંરચના __vpiArrayVthrA*obj = નવું __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = નવું array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = handle;
      obj->address = 0;

      પાછા obj;
}

vpiHandle vpip_make_vthr_APV(અક્ષર*label, unsigned index, unsigned bit, unsigned wid)
{
      સંરચના __vpiArrayVthrAPV*obj = નવું __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = નવું array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->word_sel = index;
      obj->part_bit = bit;
      obj->part_wid = wid;

      પાછા obj;
}

શૂન્ય compile_array_cleanup(શૂન્ય)
{
      delete array_table;
      array_table = 0;
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય memory_delete(vpiHandle item)
{
      સંરચના __vpiArray*arr = static_cast<સંરચના __vpiArray*>(item);
      જો (arr->vals_શબ્દો) delete [] (arr->vals_શબ્દો-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals;

      જો (arr->nets) {
	    માટે (unsigned idx = 0; idx < arr->get_size(); idx += 1) {
		  જો (સંરચના __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.index);
		    /* These should only be the real words. */
		  } નહીંતર {
			પ્રતિજ્ઞા(arr->nets[idx]->get_type_code() ==
			       vpiRealVar);
			સંરચના __vpiRealVar *sigr = static_cast<સંરચના __vpiRealVar *>
			                            (arr->nets[idx]);
			constant_delete(sigr->id.index);
// Why are only the real words still here?
			delete arr->nets[idx];
		  }
	    }
	    મુક્ત(arr->nets);
      }

      જ્યારે (arr->vpi_callbacks) {
	    સંરચના __vpiCallback*tmp = arr->vpi_callbacks->next;
	    delete arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      delete arr;
}

શૂન્ય A_delete(vpiHandle item)
{
      સંરચના __vpiArrayVthrA*obj = static_cast<સંરચના __vpiArrayVthrA*>(item);
      જો (obj->address_handle) {
	    પસંદગી (obj->address_handle->get_type_code()) {
		સ્થિતિ vpiMemoryWord:
		  જો (vpi_get(_vpiFromThr, obj->address_handle) == _vpi_at_A) {
			A_delete(obj->address_handle);
		  }
		  તોડો;
		સ્થિતિ vpiPartSelect:
		  પ્રતિજ્ઞા(vpi_get(_vpiFromThr, obj->address_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->address_handle);
		  તોડો;
	    }
      }

      delete obj;
}

શૂન્ય APV_delete(vpiHandle item)
{
      સંરચના __vpiArrayVthrAPV*obj = static_cast<સંરચના __vpiArrayVthrAPV*>(item);
      delete obj;
}
#endif
