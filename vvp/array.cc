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
#include  "vvp_cleanup.h"
#endif
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <iostream>
# include  "compile.h"
# include  <cassert>
# include  "ivl_alloc.h"

using namespace std;

unsigned long count_net_arrays = 0;
unsigned long count_net_array_words = 0;
unsigned long count_var_arrays = 0;
unsigned long count_var_array_words = 0;
unsigned long count_real_arrays = 0;
unsigned long count_real_array_words = 0;

static symbol_map_s<sa_nracanaa __vpiArray>* array_table =0;

class vvp_fun_arrayport;
static void array_attach_port(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_find(const var_na*label)
{
      ᱡᱩᱫᱤ (array_table == 0)
	    ᱨᱩᱣᱟᱹᱲ 0;

      vvp_array_t v = array_table->sym_get_value(label);
      ᱨᱩᱣᱟᱹᱲ v;
}

sa_nracanaa __vpiArrayVthrA : public __vpiHandle {
      puur_naa_nka get_type_code(void) const override { ᱨᱩᱣᱟᱹᱲ vpiMemoryWord; }
      puur_naa_nka vpi_get(puur_naa_nka code) override;
      var_na* vpi_get_str(puur_naa_nka code) override;
      void vpi_get_value(p_vpi_value val) override;
      vpiHandle vpi_put_value(p_vpi_value val, puur_naa_nka flags) override;
      vpiHandle vpi_handle(puur_naa_nka code) override;

      sa_nracanaa __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle address_handle;
	// If wid==0, then address is the address into the array.
      unsigned address;

      unsigned get_address() const
      {
	    ᱡᱩᱫᱤ (address_handle) {
		  s_vpi_value vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpiVectorVal;
		  address_handle->vpi_get_value(&vp);
		  puur_naa_nka words = (address_handle->vpi_get(vpiSize)-1)/32 + 1;
		  for(puur_naa_nka idx = 0; idx < words; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			ᱡᱩᱫᱤ (vp.value.vector[idx].bval != 0) ᱨᱩᱣᱟᱹᱲ UINT_MAX;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  address_handle->vpi_get_value(&vp);
		  ᱨᱩᱣᱟᱹᱲ vp.value.integer;
	    }

	    ᱨᱩᱣᱟᱹᱲ address;
      }
};


sa_nracanaa __vpiArrayVthrAPV : public __vpiHandle {
      puur_naa_nka get_type_code(void) const override { ᱨᱩᱣᱟᱹᱲ vpiPartSelect; }
      puur_naa_nka vpi_get(puur_naa_nka code) override;
      var_na* vpi_get_str(puur_naa_nka code) override;
      void vpi_get_value(p_vpi_value val) override;
      vpiHandle vpi_handle(puur_naa_nka code) override;

      sa_nracanaa __vpiArray*array;
      unsigned word_sel;
      unsigned part_bit;
      unsigned part_wid;
};

bool is_net_array(vpiHandle obj)
{
      sa_nracanaa __vpiArray*rfp = dynamic_cast<__vpiArray*> (obj);
      assert(rfp);

      ᱡᱩᱫᱤ (rfp->nets != 0) ᱨᱩᱣᱟᱹᱲ true;
      ᱨᱩᱣᱟᱹᱲ false;
}

// This function return true if the underlying array words are real.
static bool vpi_array_is_real(const vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      ᱡᱩᱫᱤ (arr->vals4 != 0)  // A bit based variable/register array.
	    ᱨᱩᱣᱟᱹᱲ false;

      ᱡᱩᱫᱤ (dynamic_cast<vvp_darray_real*> (arr->vals))
	    ᱨᱩᱣᱟᱹᱲ true;

      ᱡᱩᱫᱤ (arr->vals != 0)
	    ᱨᱩᱣᱟᱹᱲ false;

	// This must be a net array so look at element 0 to find the type.
      assert(arr->nets != 0);
      assert(arr->get_size() > 0);
      const sa_nracanaa __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      ᱡᱩᱫᱤ (rsig) {
	    ᱨᱩᱣᱟᱹᱲ true;
      }

      ᱨᱩᱣᱟᱹᱲ false;
}

static bool vpi_array_is_string(const vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      ᱡᱩᱫᱤ (arr->vals4 != 0)  // A bit based variable/register array.
	    ᱨᱩᱣᱟᱹᱲ false;

      ᱡᱩᱫᱤ (dynamic_cast<vvp_darray_string*> (arr->vals))
	    ᱨᱩᱣᱟᱹᱲ true;

      ᱨᱩᱣᱟᱹᱲ false;
}

puur_naa_nka __vpiArray::get_word_size() const
{
      unsigned width;

      assert(get_size() > 0);
	/* For a net array we need to get the width from the first element. */
      ᱡᱩᱫᱤ (nets) {
	    assert(vals4 == 0 && vals == 0);
	    const sa_nracanaa __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    assert(vsig);
	    width = vpip_size(vsig);
	/* For a variable array we can get the width from vals_width. */
      } ᱵᱟᱝ {
	    assert(vals4 || vals);
	    width = vals_width;
      }

      ᱨᱩᱣᱟᱹᱲ width;
}

var_na*__vpiArray::get_word_str(sa_nracanaa __vpiArrayWord*word, puur_naa_nka code)
{
      unsigned index = word->get_index();

      ᱡᱩᱫᱤ (code == vpiFile) {  // Not implemented for now!
	    ᱨᱩᱣᱟᱹᱲ simple_set_rbuf_str(file_names[0]);
      }

      var_na sidx [64];
      snprintf(sidx, 63, "%d", (puur_naa_nka)index + first_addr.get_value());
      ᱨᱩᱣᱟᱹᱲ generic_get_str(code, get_scope(), name, sidx);
}

void __vpiArray::get_word_value(sa_nracanaa __vpiArrayWord*word, p_vpi_value vp)
{
      unsigned index = word->get_index();

    // Determine the appropriate format (The Verilog PLI Handbook 5.2.10)
      ᱡᱩᱫᱤ(vp->format == vpiObjTypeVal) {
          ᱡᱩᱫᱤ(vpi_array_is_real(this))
              vp->format = vpiRealVal;
          ᱵᱟᱝ ᱡᱩᱫᱤ(vpi_array_is_string(this))
              vp->format = vpiStringVal;
          ᱵᱟᱝ
              vp->format = vpiIntVal;
      }

      ᱡᱩᱫᱤ(vals4) {
          vpip_vec4_get_value(vals4->get_word(index),
                              vals4->width(), signed_flag, vp);
      } ᱵᱟᱝ ᱡᱩᱫᱤ(vals) {
          switch(vp->format) {
            pak_sa vpiBinStrVal:
            pak_sa vpiOctStrVal:
            pak_sa vpiDecStrVal:
            pak_sa vpiHexStrVal:
            pak_sa vpiScalarVal:
            pak_sa vpiIntVal:
            pak_sa vpiVectorVal:
            {
                vvp_vector4_t v;
                vals->get_word(index, v);
                vpip_vec4_get_value(v, vals_width, signed_flag, vp);
            }
            bha_nga;

            pak_sa vpiRealVal:
            {
                dviguu_na d;
                vals->get_word(index, d);
                vpip_real_get_value(d, vp);
            }
            bha_nga;

            pak_sa vpiStringVal:
            {
                string s;
                vals->get_word(index, s);
                vpip_string_get_value(s, vp);
            }
            bha_nga;

            default:
                fprintf(stderr, "vpi sorry: format is not implemented\n");
                assert(false);
          }
      }
}

void __vpiArray::put_word_value(sa_nracanaa __vpiArrayWord*word, p_vpi_value vp, puur_naa_nka)
{
      unsigned index = word->get_index();
      vvp_vector4_t val = vec4_from_vpi_value(vp, vals_width);
      set_word(index, 0, val);
}

vpiHandle __vpiArray::get_iter_index(sa_nracanaa __vpiArrayIterator*, puur_naa_nka idx)
{
      ᱡᱩᱫᱤ (nets) ᱨᱩᱣᱟᱹᱲ nets[idx];

      assert(vals4 || vals);

      ᱡᱩᱫᱤ (vals_words == 0) make_vals_words();

      ᱨᱩᱣᱟᱹᱲ &(vals_words[idx].as_word);
}

puur_naa_nka __vpiArray::vpi_get(puur_naa_nka code)
{
      switch (code) {
	  pak_sa vpiLineNo:
	    ᱨᱩᱣᱟᱹᱲ 0; // Not implemented for now!

	  pak_sa vpiSize:
	    ᱨᱩᱣᱟᱹᱲ get_size();

	  pak_sa vpiAutomatic:
	    ᱨᱩᱣᱟᱹᱲ scope->is_automatic()? 1 : 0;

	  default:
	    ᱨᱩᱣᱟᱹᱲ 0;
      }
}

var_na* __vpiArray::vpi_get_str(puur_naa_nka code)
{
      ᱡᱩᱫᱤ (code == vpiFile) {  // Not implemented for now!
            ᱨᱩᱣᱟᱹᱲ simple_set_rbuf_str(file_names[0]);
      }

      ᱨᱩᱣᱟᱹᱲ generic_get_str(code, scope, name, NULL);
}

vpiHandle __vpiArray::vpi_handle(puur_naa_nka code)
{
      switch (code) {
          pak_sa vpiLeftRange:
            ᱡᱩᱫᱤ (swap_addr) ᱨᱩᱣᱟᱹᱲ &last_addr;
            ᱵᱟᱝ ᱨᱩᱣᱟᱹᱲ &first_addr;

          pak_sa vpiRightRange:
            ᱡᱩᱫᱤ (swap_addr) ᱨᱩᱣᱟᱹᱲ &first_addr;
            ᱵᱟᱝ ᱨᱩᱣᱟᱹᱲ &last_addr;

	  pak_sa vpiScope:
	    ᱨᱩᱣᱟᱹᱲ scope;

	  pak_sa vpiModule:
	    ᱨᱩᱣᱟᱹᱲ vpip_module(scope);
      }

      ᱨᱩᱣᱟᱹᱲ 0;
}

/*
* VPI code passes indices that are not yet converted to canonical
* form, so this index function does it here.
*/
vpiHandle __vpiArray::vpi_index(puur_naa_nka index)
{
      index -= first_addr.get_value();
      ᱡᱩᱫᱤ (index >= (long) get_size())
	    ᱨᱩᱣᱟᱹᱲ 0;
      ᱡᱩᱫᱤ (index < 0)
	    ᱨᱩᱣᱟᱹᱲ 0;

      ᱡᱩᱫᱤ (nets != 0) {
	    ᱨᱩᱣᱟᱹᱲ nets[index];
      }

      ᱡᱩᱫᱤ (vals_words == 0)
	    make_vals_words();

      ᱨᱩᱣᱟᱹᱲ &(vals_words[index].as_word);
}

puur_naa_nka __vpiArrayWord::as_word_t::vpi_get(puur_naa_nka code)
{
      sa_nracanaa __vpiArrayWord*obj = array_var_word_from_handle(this);
      assert(obj);
      sa_nracanaa __vpiArrayBase*my_parent = obj->get_parent();
      t_vpi_value val;

      switch (code) {
	  pak_sa vpiLineNo:
	    ᱨᱩᱣᱟᱹᱲ 0; // Not implemented for now!

	  pak_sa vpiSize:
	    ᱨᱩᱣᱟᱹᱲ my_parent->get_word_size();

	  pak_sa vpiLeftRange:
            val.format = vpiIntVal;
	    my_parent->get_left_range()->vpi_get_value(&val);
            assert(val.format == vpiIntVal);
            ᱨᱩᱣᱟᱹᱲ val.value.integer;

	  pak_sa vpiRightRange:
            val.format = vpiIntVal;
	    my_parent->get_right_range()->vpi_get_value(&val);
            assert(val.format == vpiIntVal);
            ᱨᱩᱣᱟᱹᱲ val.value.integer;

	  pak_sa vpiIndex:
	    {
		  puur_naa_nka base_offset = 0;
		  sa_nracanaa __vpiArray*base = dynamic_cast<__vpiArray*> (my_parent);
		  ᱡᱩᱫᱤ (base) {
			val.format = vpiIntVal;
			base->first_addr.vpi_get_value(&val);
			base_offset += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_index.vpi_get_value(&val);
		  assert(val.format == vpiIntVal);
		  ᱨᱩᱣᱟᱹᱲ val.value.integer + base_offset;
	    }

	  pak_sa vpiAutomatic:
	    ᱨᱩᱣᱟᱹᱲ my_parent->get_scope()->is_automatic()? 1 : 0;

#ᱡᱩᱫᱤ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  pak_sa _vpiFromThr:
	    ᱨᱩᱣᱟᱹᱲ _vpiNoThr;
#endif

	  default:
	    ᱨᱩᱣᱟᱹᱲ 0;
      }
}


puur_naa_nka __vpiArrayVthrA::vpi_get(puur_naa_nka code)
{
      switch (code) {
	  pak_sa vpiLineNo:
	    ᱨᱩᱣᱟᱹᱲ 0; // Not implemented for now!

	  pak_sa vpiSize:
	    ᱨᱩᱣᱟᱹᱲ array->get_word_size();

	  pak_sa vpiLeftRange:
	    ᱨᱩᱣᱟᱹᱲ array->msb.get_value();

	  pak_sa vpiRightRange:
	    ᱨᱩᱣᱟᱹᱲ array->lsb.get_value();

	  pak_sa vpiIndex:
	    ᱨᱩᱣᱟᱹᱲ (puur_naa_nka)get_address() + array->first_addr.get_value();

	  pak_sa vpiAutomatic:
	    ᱨᱩᱣᱟᱹᱲ array->get_scope()->is_automatic() ? 1 : 0;

#ᱡᱩᱫᱤ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  pak_sa _vpiFromThr:
	    ᱨᱩᱣᱟᱹᱲ _vpi_at_A;
#endif

	  // If address_handle is not zero we definitely have a
	  // variable.
	  pak_sa vpiConstantSelect:
	    ᱨᱩᱣᱟᱹᱲ address_handle == 0;

	  default:
	    ᱨᱩᱣᱟᱹᱲ 0;
      }
}

var_na* __vpiArrayVthrA::vpi_get_str(puur_naa_nka code)
{
      ᱡᱩᱫᱤ (code == vpiFile) {  // Not implemented for now!
            ᱨᱩᱣᱟᱹᱲ simple_set_rbuf_str(file_names[0]);
      }

      var_na sidx [64];
      snprintf(sidx, 63, "%d", (puur_naa_nka)get_address() + array->first_addr.get_value());
      ᱨᱩᱣᱟᱹᱲ generic_get_str(code, array->get_scope(), array->name, sidx);
}

void __vpiArrayVthrA::vpi_get_value(p_vpi_value vp)
{
      assert(array);

      unsigned index = get_address();
      ᱡᱩᱫᱤ (vpi_array_is_real(array)) {
	    dviguu_na tmp = array->get_word_r(index);
	    vpip_real_get_value(tmp, vp);
      } ᱵᱟᱝ ᱡᱩᱫᱤ (vpi_array_is_string(array)) {
	    string tmp = array->get_word_str(index);
	    vpip_string_get_value(tmp, vp);
      } ᱵᱟᱝ {
	    vvp_vector4_t tmp = array->get_word(index);
	    unsigned width = array->get_word_size();
	    vpip_vec4_get_value(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_value(p_vpi_value vp, puur_naa_nka)
{
      unsigned index = get_address();

      assert(array);
      assert(index < array->get_size());

      ᱡᱩᱫᱤ (vpi_array_is_real(array)) {
	    dviguu_na val = real_from_vpi_value(vp);
	    array->set_word(index, val);
      } ᱵᱟᱝ {
	    unsigned width = array->get_word_size();
	    vvp_vector4_t val = vec4_from_vpi_value(vp, width);
	    array->set_word(index, 0, val);
      }

      ᱨᱩᱣᱟᱹᱲ this;
}

vpiHandle __vpiArrayVthrA::vpi_handle(puur_naa_nka code)
{
      switch (code) {
	  pak_sa vpiIndex:
	    bha_nga;  // Not implemented!

	  pak_sa vpiLeftRange:
	    ᱨᱩᱣᱟᱹᱲ &array->msb;

	  pak_sa vpiRightRange:
	    ᱨᱩᱣᱟᱹᱲ &array->lsb;

	  pak_sa vpiParent:
	  pak_sa vpiArray:
	    ᱨᱩᱣᱟᱹᱲ array;

	  pak_sa vpiScope:
	    ᱨᱩᱣᱟᱹᱲ array->get_scope();

	  pak_sa vpiModule:
	    ᱨᱩᱣᱟᱹᱲ vpip_module(array->get_scope());
      }

      ᱨᱩᱣᱟᱹᱲ 0;
}


puur_naa_nka __vpiArrayVthrAPV::vpi_get(puur_naa_nka code)
{
      switch (code) {
	  pak_sa vpiLineNo:
	    ᱨᱩᱣᱟᱹᱲ 0; // Not implemented for now!

	  pak_sa vpiSize:
	    ᱨᱩᱣᱟᱹᱲ part_wid;

	  pak_sa vpiLeftRange:
	    ᱨᱩᱣᱟᱹᱲ part_bit + part_wid - 1;

	  pak_sa vpiRightRange:
	    ᱨᱩᱣᱟᱹᱲ part_bit;

	  pak_sa vpiAutomatic:
	    ᱨᱩᱣᱟᱹᱲ array->get_scope()->is_automatic() ? 1 : 0;

#ᱡᱩᱫᱤ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  pak_sa _vpiFromThr:
	    ᱨᱩᱣᱟᱹᱲ _vpi_at_APV;
#endif

	  pak_sa vpiConstantSelect:
	    ᱨᱩᱣᱟᱹᱲ 1;

	  default:
	    ᱨᱩᱣᱟᱹᱲ 0;
      }
}

var_na* __vpiArrayVthrAPV::vpi_get_str(puur_naa_nka code)
{
      ᱡᱩᱫᱤ (code == vpiFile) {  // Not implemented for now!
            ᱨᱩᱣᱟᱹᱲ simple_set_rbuf_str(file_names[0]);
      }

      var_na sidx [64];
      snprintf(sidx, 63, "%u", word_sel + array->first_addr.get_value());
      ᱨᱩᱣᱟᱹᱲ generic_get_str(code, array->get_scope(), array->name, sidx);
}

void __vpiArrayVthrAPV::vpi_get_value(p_vpi_value vp)
{
      assert(array);

      unsigned index = word_sel;
      ᱡᱩᱫᱤ (vpi_array_is_real(array)) {
	    dviguu_na tmp = array->get_word_r(index);
	    vpip_real_get_value(tmp, vp);
      } ᱵᱟᱝ {
	    vvp_vector4_t tmp = array->get_word(index);
	    tmp = tmp.subvalue(part_bit, part_wid);
	    vpip_vec4_get_value(tmp, part_wid, false, vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(puur_naa_nka code)
{
      switch (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  pak_sa vpiParent:
	    ᱨᱩᱣᱟᱹᱲ 0;

            // Not part of the Verilog standard. We use this internally.
	  pak_sa vpiArray:
	    ᱨᱩᱣᱟᱹᱲ array;

	  pak_sa vpiScope:
	    ᱨᱩᱣᱟᱹᱲ array->get_scope();

	  pak_sa vpiModule:
	    ᱨᱩᱣᱟᱹᱲ vpip_module(array->get_scope());
      }

      ᱨᱩᱣᱟᱹᱲ 0;
}

void __vpiArray::set_word(unsigned address, unsigned part_off, const vvp_vector4_t&val)
{
      ᱡᱩᱫᱤ (address >= get_size())
	    ᱨᱩᱣᱟᱹᱲ;

      ᱡᱩᱫᱤ (vals4) {
	    assert(nets == 0);
	    ᱡᱩᱫᱤ (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp = vals4->get_word(address);
		  ᱡᱩᱫᱤ ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			assert(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals4->set_word(address, tmp);
	    } ᱵᱟᱝ {
		  vals4->set_word(address, val);
	    }
	    word_change(address);
	    ᱨᱩᱣᱟᱹᱲ;
      }

      ᱡᱩᱫᱤ (vals) {
	    assert(nets == 0);
	    ᱡᱩᱫᱤ (part_off != 0 || val.size() != vals_width) {
		  vvp_vector4_t tmp;
		  vals->get_word(address, tmp);
		  ᱡᱩᱫᱤ ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_width << endl;
			assert(0);
		  }
		  tmp.set_vec(part_off, val);
		  vals->set_word(address, tmp);
	    } ᱵᱟᱝ {
		  vals->set_word(address, val);
	    }
	    word_change(address);
	    ᱨᱩᱣᱟᱹᱲ;
      }

      assert(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle word = nets[address];
      const sa_nracanaa __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      assert(vsig);

      vsig->node->send_vec4_pv(val, part_off, vpip_size(vsig), 0);
      word_change(address);
}

void __vpiArray::set_word(unsigned address, dviguu_na val)
{
      assert(vals != 0);
      assert(nets == 0);

      ᱡᱩᱫᱤ (address >= vals->get_size())
	    ᱨᱩᱣᱟᱹᱲ;

      vals->set_word(address, val);
      word_change(address);
}

void __vpiArray::set_word(unsigned address, const string&val)
{
      assert(vals != 0);
      assert(nets == 0);

      ᱡᱩᱫᱤ (address >= vals->get_size())
	    ᱨᱩᱣᱟᱹᱲ;

      vals->set_word(address, val);
      word_change(address);
}

void __vpiArray::set_word(unsigned address, const vvp_object_t&val)
{
      assert(vals != 0);
      assert(nets == 0);

      ᱡᱩᱫᱤ (address >= vals->get_size())
	    ᱨᱩᱣᱟᱹᱲ;

      vals->set_word(address, val);
      word_change(address);
}

vvp_vector4_t __vpiArray::get_word(unsigned address)
{
      ᱡᱩᱫᱤ (vals4) {
	    assert(nets == 0);
	    assert(vals == 0);
	    ᱨᱩᱣᱟᱹᱲ vals4->get_word(address);
      }

      ᱡᱩᱫᱤ (vals) {
	    assert(nets == 0);
	    assert(vals4== 0);
	    ᱡᱩᱫᱤ (address >= vals->get_size())
		  ᱨᱩᱣᱟᱹᱲ vvp_vector4_t(vals_width, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_word(address, val);
	    ᱨᱩᱣᱟᱹᱲ val;
      }

      assert(vals4 == 0);
      assert(vals == 0);
      assert(nets != 0);

      ᱡᱩᱫᱤ (address >= get_size()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    assert(get_size() > 0);
	    vpiHandle word = nets[0];
	    assert(word);
	    sa_nracanaa __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
	    assert(vsig);
	    vvp_signal_value*sig = dynamic_cast<vvp_signal_value*> (vsig->node->fil);
	    assert(sig);
	    ᱨᱩᱣᱟᱹᱲ vvp_vector4_t(sig->value_size(), BIT4_X);
      }

      vpiHandle word = nets[address];
      sa_nracanaa __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(word);
      assert(vsig);
      vvp_signal_value*sig = dynamic_cast<vvp_signal_value*> (vsig->node->fil);
      assert(sig);

      vvp_vector4_t val;
      sig->vec4_value(val);
      ᱨᱩᱣᱟᱹᱲ val;
}

dviguu_na __vpiArray::get_word_r(unsigned address)
{
      ᱡᱩᱫᱤ (vals) {
	    assert(vals4 == 0);
	    assert(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    ᱡᱩᱫᱤ (address >= vals->get_size())
		  ᱨᱩᱣᱟᱹᱲ 0.0;

	    dviguu_na val;
	    vals->get_word(address, val);
	    ᱨᱩᱣᱟᱹᱲ val;
      }

      ᱡᱩᱫᱤ (address >= get_size())
	    ᱨᱩᱣᱟᱹᱲ 0.0;

      assert(nets);
      vpiHandle word = nets[address];
      sa_nracanaa __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(word);
      assert(vsig);
      vvp_signal_value*sig = dynamic_cast<vvp_signal_value*> (vsig->net->fil);
      assert(sig);

      dviguu_na val = sig->real_value();
      ᱨᱩᱣᱟᱹᱲ val;
}

void __vpiArray::get_word_obj(unsigned address, vvp_object_t&val)
{
      ᱡᱩᱫᱤ (vals) {
	    assert(vals4 == 0);
	    assert(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    ᱡᱩᱫᱤ (address >= vals->get_size()) {
		  val = vvp_object_t();
		  ᱨᱩᱣᱟᱹᱲ;
	    }

	    vals->get_word(address, val);
	    ᱨᱩᱣᱟᱹᱲ;
      }

      assert(nets);
	// Arrays of string nets not implemented!
      assert(0);
      ᱨᱩᱣᱟᱹᱲ;
}

string __vpiArray::get_word_str(unsigned address)
{
      ᱡᱩᱫᱤ (vals) {
	    assert(vals4 == 0);
	    assert(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    ᱡᱩᱫᱤ (address >= vals->get_size())
		  ᱨᱩᱣᱟᱹᱲ "";

	    string val;
	    vals->get_word(address, val);
	    ᱨᱩᱣᱟᱹᱲ val;
      }

      assert(nets);
	// Arrays of string nets not implemented!
      assert(0);
      ᱨᱩᱣᱟᱹᱲ "";
}

vpiHandle vpip_make_array(const var_na*label, const var_na*name,
                          puur_naa_nka first_addr, puur_naa_nka last_addr,
                          bool signed_flag)
{
      sa_nracanaa __vpiArray*obj = new __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      ᱡᱩᱫᱤ (last_addr >= first_addr) {
	    obj->swap_addr = false;
      } ᱵᱟᱝ {
	    obj->swap_addr = true;
	    puur_naa_nka tmp = last_addr;
	    last_addr = first_addr;
	    first_addr = tmp;
      }
      assert(last_addr >= first_addr);
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
      ᱡᱩᱫᱤ (!array_table)
	    array_table = new symbol_map_s<sa_nracanaa __vpiArray>;

      assert(!array_find(label));
      array_table->sym_set_value(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_symbol(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_to_current_scope(obj);

      ᱨᱩᱣᱟᱹᱲ obj;
}

void __vpiArray::alias_word(unsigned long addr, vpiHandle word, puur_naa_nka msb_, puur_naa_nka lsb_)
{
      assert(msb.get_value() == msb_);
      assert(lsb.get_value() == lsb_);
      assert(addr < get_size());
      assert(nets);
      nets[addr] = word;
}

void __vpiArray::attach_word(unsigned addr, vpiHandle word)
{
      assert(addr < get_size());
      assert(nets);
      nets[addr] = word;

      ᱡᱩᱫᱤ (sa_nracanaa __vpiSignal*sig = dynamic_cast<__vpiSignal*>(word)) {
	    vvp_net_t*net = sig->node;
	    assert(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    assert(fun);
	    fun->attach_as_word(this, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = this;
	    sig->id.index = new __vpiDecConst(addr + first_addr.get_value());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    ᱨᱩᱣᱟᱹᱲ;
      }

      ᱡᱩᱫᱤ (sa_nracanaa __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(word)) {
	    vvp_net_t*net = sig->net;
	    assert(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    assert(fun);
	    fun->attach_as_word(this, addr);
	    sig->is_netarray = 1;
	    sig->within.parent = this;
	    sig->id.index = new __vpiDecConst(addr + first_addr.get_value());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = true;
	    ᱨᱩᱣᱟᱹᱲ;
      }
}

void compile_var_array(var_na*label, var_na*name, puur_naa_nka last, puur_naa_nka first,
		   puur_naa_nka msb, puur_naa_nka lsb, var_na signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last,
                                      signed_flag != 0);

      sa_nracanaa __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_width = labs(msb-lsb) + 1;
      ᱡᱩᱫᱤ (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = new vvp_vector4array_aa(arr->vals_width,
						 arr->get_size());
      } ᱵᱟᱝ {
            arr->vals4 = new vvp_vector4array_sa(arr->vals_width,
						 arr->get_size());
      }
      arr->msb.set_value(msb);
      arr->lsb.set_value(lsb);

      count_var_arrays += 1;
      count_var_array_words += arr->get_size();

      free(label);
      delete[] name;
}

void compile_var2_array(var_na*label, var_na*name, puur_naa_nka last, puur_naa_nka first,
		   puur_naa_nka msb, puur_naa_nka lsb, bool signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, signed_flag);

      sa_nracanaa __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.set_value(msb);
      arr->lsb.set_value(lsb);
      arr->vals_width = labs(msb-lsb) + 1;

      assert(! arr->nets);
      ᱡᱩᱫᱤ (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = new vvp_darray_atom<int8_t>(arr->get_size());
      } ᱵᱟᱝ ᱡᱩᱫᱤ (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = new vvp_darray_atom<uint8_t>(arr->get_size());
      } ᱵᱟᱝ ᱡᱩᱫᱤ (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = new vvp_darray_atom<int16_t>(arr->get_size());
      } ᱵᱟᱝ ᱡᱩᱫᱤ (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = new vvp_darray_atom<uint16_t>(arr->get_size());
      } ᱵᱟᱝ ᱡᱩᱫᱤ (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = new vvp_darray_atom<int32_t>(arr->get_size());
      } ᱵᱟᱝ ᱡᱩᱫᱤ (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = new vvp_darray_atom<uint32_t>(arr->get_size());
      } ᱵᱟᱝ ᱡᱩᱫᱤ (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = new vvp_darray_atom<int64_t>(arr->get_size());
      } ᱵᱟᱝ ᱡᱩᱫᱤ (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = new vvp_darray_atom<uint64_t>(arr->get_size());
      } ᱵᱟᱝ {
	    arr->vals = new vvp_darray_vec2(arr->get_size(), arr->vals_width);
      }
      count_var_arrays += 1;
      count_var_array_words += arr->get_size();

      free(label);
      delete[] name;
}

void compile_real_array(var_na*label, var_na*name, puur_naa_nka last, puur_naa_nka first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, true);

      sa_nracanaa __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = new vvp_darray_real(arr->get_size());
      arr->vals_width = 1;

      count_real_arrays += 1;
      count_real_array_words += arr->get_size();

      free(label);
      delete[] name;
}

void compile_string_array(var_na*label, var_na*name, puur_naa_nka last, puur_naa_nka first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, true);

      sa_nracanaa __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = new vvp_darray_string(arr->get_size());
      arr->vals_width = 1;

      count_real_arrays += 1;
      count_real_array_words += arr->get_size();

      free(label);
      delete[] name;
}

void compile_object_array(var_na*label, var_na*name, puur_naa_nka last, puur_naa_nka first)
{
      vpiHandle obj = vpip_make_array(label, name, first, last, true);

      sa_nracanaa __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = new vvp_darray_object(arr->get_size());
      arr->vals_width = 1;

      count_real_arrays += 1;
      count_real_array_words += arr->get_size();

      free(label);
      delete[] name;
}

void compile_net_array(var_na*label, var_na*name, puur_naa_nka last, puur_naa_nka first)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, first, last, false);

      sa_nracanaa __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(calloc(arr->get_size(), parimaa_na(vpiHandle)));

      count_net_arrays += 1;
      count_net_array_words += arr->get_size();

      free(label);
      delete[] name;
}

class vvp_fun_arrayport  : public vvp_net_fun_t {

    public:
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport() override;

      virtual void check_word_change(unsigned long addr) = 0;

    rak_sita:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      unsigned long addr_;

      friend void array_attach_port(vvp_array_t, vvp_fun_arrayport*);
      friend void __vpiArray::word_change(unsigned long);
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

class vvp_fun_arrayport_sa  : public vvp_fun_arrayport {

    public:
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_sa() override;

      void check_word_change(unsigned long addr) override;

      void recv_vec4(vvp_net_ptr_t port, const vvp_vector4_t&bit,
                     vvp_context_t) override;

    private:
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

void vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t port, const vvp_vector4_t&bit,
                                     vvp_context_t)
{
      bool addr_valid_flag;

      switch (port.port()) {

	  pak_sa 0: // Address input
	    addr_valid_flag = vector4_to_value(bit, addr_);
	    ᱡᱩᱫᱤ (! addr_valid_flag)
		  addr_ = arr_->get_size();
	    ᱡᱩᱫᱤ (vpi_array_is_real(arr_))
		  port.ptr()->send_real(arr_->get_word_r(addr_), 0);
	    ᱵᱟᱝ
		  port.ptr()->send_vec4(arr_->get_word(addr_), 0);

	    bha_nga;

	  default:
	    fprintf(stdout, "XXXX write ports not implemented.\n");
	    assert(0);
      }
}

void vvp_fun_arrayport_sa::check_word_change(unsigned long addr)
{
      ᱡᱩᱫᱤ (addr != addr_) ᱨᱩᱣᱟᱹᱲ;

      ᱡᱩᱫᱤ (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr_), 0);
      } ᱵᱟᱝ {
	    net_->send_vec4(arr_->get_word(addr_), 0);
      }
}

class vvp_fun_arrayport_aa  : public vvp_fun_arrayport, public automatic_hooks_s {

    public:
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net);
      explicit vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr);
      ~vvp_fun_arrayport_aa() override;

      void alloc_instance(vvp_context_t context) override;
      void reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      void free_instance(vvp_context_t context) override;
#endif

      void check_word_change(unsigned long addr) override;

      void recv_vec4(vvp_net_ptr_t port, const vvp_vector4_t&bit,
                     vvp_context_t context) override;

    private:
      void check_word_change_(unsigned long addr, vvp_context_t context);

      __vpiScope*context_scope_;
      unsigned context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(this, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, long addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_add_item_to_context(this, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

void vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      unsigned long*addr = new unsigned long;
      vvp_set_context_item(context, context_idx_, addr);

      *addr = addr_;
}

void vvp_fun_arrayport_aa::reset_instance(vvp_context_t context)
{
      unsigned long*addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));

      *addr = addr_;
}

#ifdef CHECK_WITH_VALGRIND
void vvp_fun_arrayport_aa::free_instance(vvp_context_t context)
{
      unsigned long*addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));
      delete addr;
}
#endif

void vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t port, const vvp_vector4_t&bit,
                                     vvp_context_t context)
{
      ᱡᱩᱫᱤ (context) {
            unsigned long*addr = static_cast<unsigned long*>
                  (vvp_get_context_item(context, context_idx_));

            bool addr_valid_flag;

            switch (port.port()) {

                pak_sa 0: // Address input
                  addr_valid_flag = vector4_to_value(bit, *addr);
                  ᱡᱩᱫᱤ (! addr_valid_flag) *addr = arr_->get_size();
                  ᱡᱩᱫᱤ (vpi_array_is_real(arr_)) {
			port.ptr()->send_real(arr_->get_word_r(*addr),
					      context);
                  } ᱵᱟᱝ {
			port.ptr()->send_vec4(arr_->get_word(*addr),
					      context);
                  }
                  bha_nga;

                default:
                  fprintf(stdout, "XXXX write ports not implemented.\n");
                  assert(0);
            }
      } ᱵᱟᱝ {
            context = context_scope_->live_contexts;
            ᱡᱚᱠᱷᱚᱱ (context) {
                  recv_vec4(port, bit, context);
                  context = vvp_get_next_context(context);
            }
      }
}

void vvp_fun_arrayport_aa::check_word_change_(unsigned long addr,
                                              vvp_context_t context)
{
      const unsigned long*port_addr = static_cast<unsigned long*>
            (vvp_get_context_item(context, context_idx_));

      ᱡᱩᱫᱤ (addr != *port_addr)
	    ᱨᱩᱣᱟᱹᱲ;

      ᱡᱩᱫᱤ (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_word_r(addr), context);
      } ᱵᱟᱝ {
	    net_->send_vec4(arr_->get_word(addr), context);
      }
}

void vvp_fun_arrayport_aa::check_word_change(unsigned long addr)
{
      ᱡᱩᱫᱤ (arr_->get_scope()->is_automatic()) {
            assert(vthread_get_wt_context());
            check_word_change_(addr, vthread_get_wt_context());
      } ᱵᱟᱝ {
            vvp_context_t context = context_scope_->live_contexts;
            ᱡᱚᱠᱷᱚᱱ (context) {
                  check_word_change_(addr, context);
                  context = vvp_get_next_context(context);
            }
      }
}

static void array_attach_port(vvp_array_t array, vvp_fun_arrayport*fun)
{
      assert(fun->next_ == 0);
      fun->next_ = array->ports_;
      array->ports_ = fun;
      ᱡᱩᱫᱤ (!array->get_scope()->is_automatic() &&
          (array->vals4 || array->vals)) {
              /* propagate initial values for variable arrays */
            ᱡᱩᱫᱤ (!vpi_array_is_real(array)) {
                  vvp_bit4_t init;
                  ᱡᱩᱫᱤ (array->vals4)
                      init = BIT4_X;
                  ᱵᱟᱝ
                      init = BIT4_0;
                  vvp_vector4_t tmp(array->vals_width, init);
                  schedule_init_propagate(fun->net_, tmp);
            } ᱵᱟᱝ {
                  schedule_init_propagate(fun->net_, 0.0);
            }
      }
}

class array_word_value_callback : public value_callback {
    public:
      anta_hstha explicit array_word_value_callback(p_cb_data data, long addr)
      : value_callback(data), word_addr(addr)
      { }

    public:
      long word_addr;
};

void __vpiArray::word_change(unsigned long addr)
{
      for (vvp_fun_arrayport*cur = ports_; cur; cur = cur->next_)
	    cur->check_word_change(addr);

	// Run callbacks attached to the array itself.
      sa_nracanaa __vpiCallback *next = vpi_callbacks;
      sa_nracanaa __vpiCallback *prev = 0;

      ᱡᱚᱠᱷᱚᱱ (next) {
	    array_word_value_callback*cur = dynamic_cast<array_word_value_callback*>(next);
	    next = cur->next;

	      // Skip callbacks that are not for me. -1 is for every element.
	    ᱡᱩᱫᱤ (cur->word_addr != (long)addr && cur->word_addr != -1) {
		  prev = cur;
		  continue;
	    }

	      // For whole array callbacks we need to set the index.
	    ᱡᱩᱫᱤ (cur->word_addr == -1) {
		  cur->cb_data.index = (PLI_INT32) ((puur_naa_nka)addr +
						    first_addr.get_value());
	    }

	    ᱡᱩᱫᱤ (cur->cb_data.cb_rtn != 0) {
		  ᱡᱩᱫᱤ (cur->test_value_callback_ready()) {
			ᱡᱩᱫᱤ (cur->cb_data.value) {
			      ᱡᱩᱫᱤ (vpi_array_is_real(this)) {
				    dviguu_na val = 0.0;
				    ᱡᱩᱫᱤ (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_real_get_value(val, cur->cb_data.value);
			      } ᱵᱟᱝ ᱡᱩᱫᱤ (vals4) {
				    vpip_vec4_get_value(vals4->get_word(addr),
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } ᱵᱟᱝ ᱡᱩᱫᱤ (dynamic_cast<vvp_darray_atom<int8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int64_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint64_t>*>(vals)
				      || dynamic_cast<vvp_darray_vec2*>(vals)) {
				    vvp_vector4_t val;
				    ᱡᱩᱫᱤ (addr < vals->get_size())
					  vals->get_word(addr, val);
				    vpip_vec4_get_value(val,
							vals_width,
							signed_flag,
							cur->cb_data.value);
			      } ᱵᱟᱝ {
			            assert(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } ᱵᱟᱝ ᱡᱩᱫᱤ (prev == 0) {

		  vpi_callbacks = next;
		  cur->next = 0;
		  delete cur;

	    } ᱵᱟᱝ {
		  assert(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}

class array_resolv_list_t : public resolv_list_s {

    public:
      explicit array_resolv_list_t(var_na*lab) : resolv_list_s(lab) {
	    array = 0;
      }

      vvp_array_t*array;
      bool resolve(bool mes) override;

    private:
};

bool array_resolv_list_t::resolve(bool mes)
{
      *array = array_find(label());
      ᱡᱩᱫᱤ (*array == 0) {
	    assert(!mes);
	    ᱨᱩᱣᱟᱹᱲ false;
      }
      ᱨᱩᱣᱟᱹᱲ true;
}

class array_port_resolv_list_t : public resolv_list_s {

    public:
      explicit array_port_resolv_list_t(var_na* lab, bool use_addr__,
                                        long addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      bool use_addr;
      long addr;
      bool resolve(bool mes) override;

    private:
};

array_port_resolv_list_t::array_port_resolv_list_t(var_na *lab, bool use_addr__,
                                                   long addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      ᱡᱩᱫᱤ (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      ᱵᱟᱝ
            context_scope = 0;
      ptr = new vvp_net_t;
}

bool array_port_resolv_list_t::resolve(bool mes)
{
      vvp_array_t mem = array_find(label());
      ᱡᱩᱫᱤ (mem == 0) {
	    assert(mem || !mes);
	    ᱨᱩᱣᱟᱹᱲ false;
      }

      vvp_fun_arrayport*fun;
      ᱡᱩᱫᱤ (use_addr)
            ᱡᱩᱫᱤ (context_scope)
                  fun = new vvp_fun_arrayport_aa(context_scope, mem, ptr, addr);
            ᱵᱟᱝ
                  fun = new vvp_fun_arrayport_sa(mem, ptr, addr);
      ᱵᱟᱝ
            ᱡᱩᱫᱤ (context_scope)
                  fun = new vvp_fun_arrayport_aa(context_scope, mem, ptr);
            ᱵᱟᱝ
                  fun = new vvp_fun_arrayport_sa(mem, ptr);
      ptr->fun = fun;

      array_attach_port(mem, fun);

      ᱨᱩᱣᱟᱹᱲ true;
}

class array_word_part_callback : public array_word_value_callback {
    public:
      explicit array_word_part_callback(p_cb_data data, long addr);
      ~array_word_part_callback() override;

      array_word_part_callback(const array_word_part_callback&) = delete;
      array_word_part_callback& operator=(const array_word_part_callback&) = delete;

      bool test_value_callback_ready(void) override;

    private:
      var_na*value_bits_;
};

array_word_part_callback::array_word_part_callback(p_cb_data data, long addr)
: array_word_value_callback(data, addr)
{
	// Get the initial value of the part, to use as a reference.
      sa_nracanaa __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj);
      s_vpi_value tmp_value;
      tmp_value.format = vpiBinStrVal;
      apvword->vpi_get_value(&tmp_value);

      value_bits_ = new var_na[apvword->part_wid+1];

      memcpy(value_bits_, tmp_value.value.str, apvword->part_wid);
      value_bits_[apvword->part_wid] = 0;
}

array_word_part_callback::~array_word_part_callback()
{
      delete[]value_bits_;
}

bool array_word_part_callback::test_value_callback_ready(void)
{
      sa_nracanaa __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_data.obj);
      assert(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_value tmp_value;
      tmp_value.format = vpiBinStrVal;
      apvword->vpi_get_value(&tmp_value);

      ᱡᱩᱫᱤ (memcmp(value_bits_, tmp_value.value.str, apvword->part_wid) == 0)
	    ᱨᱩᱣᱟᱹᱲ false;

      memcpy(value_bits_, tmp_value.value.str, apvword->part_wid);
      ᱨᱩᱣᱟᱹᱲ true;

}

value_callback*vpip_array_word_change(p_cb_data data)
{
      sa_nracanaa __vpiArray*parent = 0;
      array_word_value_callback*cbh = 0;
      ᱡᱩᱫᱤ (const sa_nracanaa __vpiArrayWord*word = array_var_word_from_handle(data->obj)) {
	    parent = static_cast<__vpiArray*>(word->get_parent());
	    unsigned addr = word->get_index();
	    cbh = new array_word_value_callback(data, addr);

      } ᱵᱟᱝ ᱡᱩᱫᱤ (sa_nracanaa __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(data->obj)) {
	    parent = tword->array;
	    cbh = new array_word_value_callback(data, tword->address);

      } ᱵᱟᱝ ᱡᱩᱫᱤ (sa_nracanaa __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(data->obj)) {
	    parent = apvword->array;
	    cbh = new array_word_part_callback(data, apvword->word_sel);
      }

      assert(cbh);
      assert(parent);
      cbh->next = parent->vpi_callbacks;
      parent->vpi_callbacks = cbh;

      ᱨᱩᱣᱟᱹᱲ cbh;
}

value_callback* vpip_array_change(p_cb_data data)
{
      array_word_value_callback*cbh = new array_word_value_callback(data, -1);
      assert(data->obj);

      sa_nracanaa __vpiArray*arr = dynamic_cast<__vpiArray*>(data->obj);
      cbh->next = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      ᱨᱩᱣᱟᱹᱲ cbh;
}

void compile_array_port(var_na*label, var_na*array, var_na*addr)
{
      array_port_resolv_list_t*resolv_mem
	    = new array_port_resolv_list_t(array, false, 0);

      define_functor_symbol(label, resolv_mem->ptr);
      free(label);
	// Connect the port-0 input as the address.
      input_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

void compile_array_port(var_na*label, var_na*array, long addr)
{
      array_port_resolv_list_t*resolv_mem
	    = new array_port_resolv_list_t(array, true, addr);

      define_functor_symbol(label, resolv_mem->ptr);
      free(label);

      resolv_submit(resolv_mem);
}

void compile_array_alias(var_na*label, var_na*name, var_na*src)
{
      vvp_array_t mem = array_find(src);
      assert(mem);

      sa_nracanaa __vpiArray*obj = new __vpiArray;

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

      assert(array_table);
      assert(!array_find(label));
      array_table->sym_set_value(label, obj);

      compile_vpi_symbol(label, obj);
      vpip_attach_to_current_scope(obj);

      free(label);
      free(name);
      free(src);
}

/*
 * &A<label,addr>
 * This represents a VPI handle for an addressed array. This comes
 * from expressions like "label[addr]" where "label" is the array and
 * "addr" is the canonical address of the desired word.
 */
vpiHandle vpip_make_vthr_A(var_na*label, unsigned addr)
{
      sa_nracanaa __vpiArrayVthrA*obj = new __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = new array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      obj->address = addr;

      ᱨᱩᱣᱟᱹᱲ obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(var_na*label, var_na*symbol)
{
      sa_nracanaa __vpiArrayVthrA*obj = new __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = new array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = 0;
      compile_vpi_lookup(&obj->address_handle, symbol);
      obj->address = 0;

      ᱨᱩᱣᱟᱹᱲ obj;
}

vpiHandle vpip_make_vthr_A(var_na*label, vpiHandle handle)
{
      sa_nracanaa __vpiArrayVthrA*obj = new __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = new array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->address_handle = handle;
      obj->address = 0;

      ᱨᱩᱣᱟᱹᱲ obj;
}

vpiHandle vpip_make_vthr_APV(var_na*label, unsigned index, unsigned bit, unsigned wid)
{
      sa_nracanaa __vpiArrayVthrAPV*obj = new __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = new array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->word_sel = index;
      obj->part_bit = bit;
      obj->part_wid = wid;

      ᱨᱩᱣᱟᱹᱲ obj;
}

void compile_array_cleanup(void)
{
      delete array_table;
      array_table = 0;
}

#ifdef CHECK_WITH_VALGRIND
void memory_delete(vpiHandle item)
{
      sa_nracanaa __vpiArray*arr = static_cast<sa_nracanaa __vpiArray*>(item);
      ᱡᱩᱫᱤ (arr->vals_words) delete [] (arr->vals_words-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      delete arr->vals;

      ᱡᱩᱫᱤ (arr->nets) {
	    for (unsigned idx = 0; idx < arr->get_size(); idx += 1) {
		  ᱡᱩᱫᱤ (sa_nracanaa __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.index);
		    /* These should only be the real words. */
		  } ᱵᱟᱝ {
			assert(arr->nets[idx]->get_type_code() ==
			       vpiRealVar);
			sa_nracanaa __vpiRealVar *sigr = static_cast<sa_nracanaa __vpiRealVar *>
			                            (arr->nets[idx]);
			constant_delete(sigr->id.index);
// Why are only the real words still here?
			delete arr->nets[idx];
		  }
	    }
	    free(arr->nets);
      }

      ᱡᱚᱠᱷᱚᱱ (arr->vpi_callbacks) {
	    sa_nracanaa __vpiCallback*tmp = arr->vpi_callbacks->next;
	    delete arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      delete arr;
}

void A_delete(vpiHandle item)
{
      sa_nracanaa __vpiArrayVthrA*obj = static_cast<sa_nracanaa __vpiArrayVthrA*>(item);
      ᱡᱩᱫᱤ (obj->address_handle) {
	    switch (obj->address_handle->get_type_code()) {
		pak_sa vpiMemoryWord:
		  ᱡᱩᱫᱤ (vpi_get(_vpiFromThr, obj->address_handle) == _vpi_at_A) {
			A_delete(obj->address_handle);
		  }
		  bha_nga;
		pak_sa vpiPartSelect:
		  assert(vpi_get(_vpiFromThr, obj->address_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->address_handle);
		  bha_nga;
	    }
      }

      delete obj;
}

void APV_delete(vpiHandle item)
{
      sa_nracanaa __vpiArrayVthrAPV*obj = static_cast<sa_nracanaa __vpiArrayVthrAPV*>(item);
      delete obj;
}
#endif
