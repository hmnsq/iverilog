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
#నిర్వచితమైతే CHECK_WITH_VALGRIND
#అన్తర్భావ  "vvp_cleanup.h"
#అయితే_ముగింపు
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <iostream>
# include  "compile.h"
# include  <cassert>
# include  "ivl_alloc.h"

ఉపయోగించి నామాకాశ std;

గుర్తులేని దీర్ఘ లెక్క_net_arrays = 0;
గుర్తులేని దీర్ఘ లెక్క_net_array_పదాలు = 0;
గుర్తులేని దీర్ఘ లెక్క_var_arrays = 0;
గుర్తులేని దీర్ఘ లెక్క_var_array_పదాలు = 0;
గుర్తులేని దీర్ఘ లెక్క_real_arrays = 0;
గుర్తులేని దీర్ఘ లెక్క_real_array_పదాలు = 0;

స్థిర చిహ్నం_map_s<నిర్మాణం __vpiArray>* array_పట్టిక =0;

తరగతి vvp_fun_arrayport;
స్థిర శూన్య array_attach_పోర్టు(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_కనుగొను(స్థిరరాశి అక్షరం*label)
{
      ఒకవేళ (array_పట్టిక == 0)
	    తిరిగి 0;

      vvp_array_t v = array_పట్టిక->sym_get_విలువ(label);
      తిరిగి v;
}

నిర్మాణం __vpiArrayVthrA : బహిరంగ __vpiHandle {
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpi_జ్ఞాపకం_మాట; }
      పూర్ణాంకం vpi_get(పూర్ణాంకం code) override;
      అక్షరం* vpi_get_str(పూర్ణాంకం code) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;
      vpiHandle vpi_put_విలువ(p_vpi_విలువ val, పూర్ణాంకం flags) override;
      vpiHandle vpi_handle(పూర్ణాంకం code) override;

      నిర్మాణం __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle చిరునామా_handle;
	// If wid==0, then address is the address into the array.
      గుర్తులేని చిరునామా;

      గుర్తులేని get_చిరునామా() స్థిరరాశి
      {
	    ఒకవేళ (చిరునామా_handle) {
		  s_vpi_విలువ vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpi_సదిశ_Val;
		  చిరునామా_handle->vpi_get_విలువ(&vp);
		  పూర్ణాంకం పదాలు = (చిరునామా_handle->vpi_get(vpi_కొలత)-1)/32 + 1;
		  కొరకు(పూర్ణాంకం idx = 0; idx < పదాలు; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			ఒకవేళ (vp.value.vector[idx].bval != 0) తిరిగి UINT_MAX;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  చిరునామా_handle->vpi_get_విలువ(&vp);
		  తిరిగి vp.value.integer;
	    }

	    తిరిగి చిరునామా;
      }
};


నిర్మాణం __vpiArrayVthrAPV : బహిరంగ __vpiHandle {
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpiPartSelect; }
      పూర్ణాంకం vpi_get(పూర్ణాంకం code) override;
      అక్షరం* vpi_get_str(పూర్ణాంకం code) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;
      vpiHandle vpi_handle(పూర్ణాంకం code) override;

      నిర్మాణం __vpiArray*array;
      గుర్తులేని మాట_sel;
      గుర్తులేని part_బిట్టు;
      గుర్తులేని part_wid;
};

తార్కిక is_net_array(vpiHandle obj)
{
      నిర్మాణం __vpiArray*rfp = dynamic_cast<__vpiArray*> (obj);
      ప్రతిజ్ఞా(rfp);

      ఒకవేళ (rfp->nets != 0) తిరిగి నిజం;
      తిరిగి అబద్ధం;
}

// This function return true if the underlying array words are real.
స్థిర తార్కిక vpi_array_is_real(స్థిరరాశి vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      ఒకవేళ (arr->vals4 != 0)  // A bit based variable/register array.
	    తిరిగి అబద్ధం;

      ఒకవేళ (dynamic_cast<vvp_darray_real*> (arr->vals))
	    తిరిగి నిజం;

      ఒకవేళ (arr->vals != 0)
	    తిరిగి అబద్ధం;

	// This must be a net array so look at element 0 to find the type.
      ప్రతిజ్ఞా(arr->nets != 0);
      ప్రతిజ్ఞా(arr->get_కొలత() > 0);
      స్థిరరాశి నిర్మాణం __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      ఒకవేళ (rsig) {
	    తిరిగి నిజం;
      }

      తిరిగి అబద్ధం;
}

స్థిర తార్కిక vpi_array_is_string(స్థిరరాశి vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      ఒకవేళ (arr->vals4 != 0)  // A bit based variable/register array.
	    తిరిగి అబద్ధం;

      ఒకవేళ (dynamic_cast<vvp_darray_string*> (arr->vals))
	    తిరిగి నిజం;

      తిరిగి అబద్ధం;
}

పూర్ణాంకం __vpiArray::get_మాట_కొలత() స్థిరరాశి
{
      గుర్తులేని width;

      ప్రతిజ్ఞా(get_కొలత() > 0);
	/* For a net array we need to get the width from the first element. */
      ఒకవేళ (nets) {
	    ప్రతిజ్ఞా(vals4 == 0 && vals == 0);
	    స్థిరరాశి నిర్మాణం __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    ప్రతిజ్ఞా(vsig);
	    width = vpip_కొలత(vsig);
	/* For a variable array we can get the width from vals_width. */
      } లేకపోతే {
	    ప్రతిజ్ఞా(vals4 || vals);
	    width = vals_వెడల్పు;
      }

      తిరిగి width;
}

అక్షరం*__vpiArray::get_మాట_str(నిర్మాణం __vpiArrayWord*మాట, పూర్ణాంకం code)
{
      గుర్తులేని సూచీ = మాట->get_సూచీ();

      ఒకవేళ (code == vpiFile) {  // Not implemented for now!
	    తిరిగి simple_సమితి_rbuf_str(file_పేర్లు[0]);
      }

      అక్షరం sidx [64];
      snprintf(sidx, 63, "%d", (పూర్ణాంకం)సూచీ + మొదటి_addr.get_విలువ());
      తిరిగి generic_get_str(code, get_scope(), name, sidx);
}

శూన్య __vpiArray::get_మాట_విలువ(నిర్మాణం __vpiArrayWord*మాట, p_vpi_విలువ vp)
{
      గుర్తులేని సూచీ = మాట->get_సూచీ();

    // Determine the appropriate format (The Verilog PLI Handbook 5.2.10)
      ఒకవేళ(vp->format == vpiObjTypeVal) {
          ఒకవేళ(vpi_array_is_real(ఇది))
              vp->format = vpiRealVal;
          లేకపోతే ఒకవేళ(vpi_array_is_string(ఇది))
              vp->format = vpiStringVal;
          లేకపోతే
              vp->format = vpiIntVal;
      }

      ఒకవేళ(vals4) {
          vpip_vec4_get_విలువ(vals4->get_మాట(సూచీ),
                              vals4->width(), signed_flag, vp);
      } లేకపోతే ఒకవేళ(vals) {
          ఎంపిక(vp->format) {
            స్థితి vpiBinStrVal:
            స్థితి vpiOctStrVal:
            స్థితి vpiDecStrVal:
            స్థితి vpiHexStrVal:
            స్థితి vpiScalarVal:
            స్థితి vpiIntVal:
            స్థితి vpi_సదిశ_Val:
            {
                vvp_vector4_t v;
                vals->get_మాట(సూచీ, v);
                vpip_vec4_get_విలువ(v, vals_వెడల్పు, signed_flag, vp);
            }
            ఆపు;

            స్థితి vpiRealVal:
            {
                ద్విగుణ d;
                vals->get_మాట(సూచీ, d);
                vpip_real_get_విలువ(d, vp);
            }
            ఆపు;

            స్థితి vpiStringVal:
            {
                string s;
                vals->get_మాట(సూచీ, s);
                vpip_string_get_విలువ(s, vp);
            }
            ఆపు;

            అప్రమేయ:
                fprintf(stderr, "vpi sorry: format is not implemented\n");
                ప్రతిజ్ఞా(అబద్ధం);
          }
      }
}

శూన్య __vpiArray::put_మాట_విలువ(నిర్మాణం __vpiArrayWord*మాట, p_vpi_విలువ vp, పూర్ణాంకం)
{
      గుర్తులేని సూచీ = మాట->get_సూచీ();
      vvp_vector4_t val = vec4_from_vpi_విలువ(vp, vals_వెడల్పు);
      సమితి_మాట(సూచీ, 0, val);
}

vpiHandle __vpiArray::get_iter_సూచీ(నిర్మాణం __vpiArrayIterator*, పూర్ణాంకం idx)
{
      ఒకవేళ (nets) తిరిగి nets[idx];

      ప్రతిజ్ఞా(vals4 || vals);

      ఒకవేళ (vals_పదాలు == 0) make_vals_పదాలు();

      తిరిగి &(vals_పదాలు[idx].as_మాట);
}

పూర్ణాంకం __vpiArray::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpi_పంక్తి_No:
	    తిరిగి 0; // Not implemented for now!

	  స్థితి vpi_కొలత:
	    తిరిగి get_కొలత();

	  స్థితి vpiAutomatic:
	    తిరిగి scope->is_automatic()? 1 : 0;

	  అప్రమేయ:
	    తిరిగి 0;
      }
}

అక్షరం* __vpiArray::vpi_get_str(పూర్ణాంకం code)
{
      ఒకవేళ (code == vpiFile) {  // Not implemented for now!
            తిరిగి simple_సమితి_rbuf_str(file_పేర్లు[0]);
      }

      తిరిగి generic_get_str(code, scope, name, ఖాళీ);
}

vpiHandle __vpiArray::vpi_handle(పూర్ణాంకం code)
{
      ఎంపిక (code) {
          స్థితి vpi_ఎడమ_Range:
            ఒకవేళ (మార్పిడి_addr) తిరిగి &చివరి_addr;
            లేకపోతే తిరిగి &మొదటి_addr;

          స్థితి vpi_కుడి_Range:
            ఒకవేళ (మార్పిడి_addr) తిరిగి &మొదటి_addr;
            లేకపోతే తిరిగి &చివరి_addr;

	  స్థితి vpiScope:
	    తిరిగి scope;

	  స్థితి vpiModule:
	    తిరిగి vpip_module(scope);
      }

      తిరిగి 0;
}

/*
* VPI code passes indices that are not yet converted to canonical
* form, so this index function does it here.
*/
vpiHandle __vpiArray::vpi_సూచీ(పూర్ణాంకం సూచీ)
{
      సూచీ -= మొదటి_addr.get_విలువ();
      ఒకవేళ (సూచీ >= (దీర్ఘ) get_కొలత())
	    తిరిగి 0;
      ఒకవేళ (సూచీ < 0)
	    తిరిగి 0;

      ఒకవేళ (nets != 0) {
	    తిరిగి nets[సూచీ];
      }

      ఒకవేళ (vals_పదాలు == 0)
	    make_vals_పదాలు();

      తిరిగి &(vals_పదాలు[సూచీ].as_మాట);
}

పూర్ణాంకం __vpiArrayWord::as_మాట_t::vpi_get(పూర్ణాంకం code)
{
      నిర్మాణం __vpiArrayWord*obj = array_var_మాట_from_handle(ఇది);
      ప్రతిజ్ఞా(obj);
      నిర్మాణం __vpiArrayBase*my_మాతృ = obj->get_మాతృ();
      t_vpi_విలువ val;

      ఎంపిక (code) {
	  స్థితి vpi_పంక్తి_No:
	    తిరిగి 0; // Not implemented for now!

	  స్థితి vpi_కొలత:
	    తిరిగి my_మాతృ->get_మాట_కొలత();

	  స్థితి vpi_ఎడమ_Range:
            val.format = vpiIntVal;
	    my_మాతృ->get_ఎడమ_range()->vpi_get_విలువ(&val);
            ప్రతిజ్ఞా(val.format == vpiIntVal);
            తిరిగి val.value.integer;

	  స్థితి vpi_కుడి_Range:
            val.format = vpiIntVal;
	    my_మాతృ->get_కుడి_range()->vpi_get_విలువ(&val);
            ప్రతిజ్ఞా(val.format == vpiIntVal);
            తిరిగి val.value.integer;

	  స్థితి vpi_సూచీ_2:
	    {
		  పూర్ణాంకం పునాది_స్థానభ్రంశం = 0;
		  నిర్మాణం __vpiArray*పునాది = dynamic_cast<__vpiArray*> (my_మాతృ);
		  ఒకవేళ (పునాది) {
			val.format = vpiIntVal;
			పునాది->మొదటి_addr.vpi_get_విలువ(&val);
			పునాది_స్థానభ్రంశం += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_సూచీ.vpi_get_విలువ(&val);
		  ప్రతిజ్ఞా(val.format == vpiIntVal);
		  తిరిగి val.value.integer + పునాది_స్థానభ్రంశం;
	    }

	  స్థితి vpiAutomatic:
	    తిరిగి my_మాతృ->get_scope()->is_automatic()? 1 : 0;

#ఒకవేళ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  స్థితి _vpiFromThr:
	    తిరిగి _vpiNoThr;
#అయితే_ముగింపు

	  అప్రమేయ:
	    తిరిగి 0;
      }
}


పూర్ణాంకం __vpiArrayVthrA::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpi_పంక్తి_No:
	    తిరిగి 0; // Not implemented for now!

	  స్థితి vpi_కొలత:
	    తిరిగి array->get_మాట_కొలత();

	  స్థితి vpi_ఎడమ_Range:
	    తిరిగి array->msb.get_విలువ();

	  స్థితి vpi_కుడి_Range:
	    తిరిగి array->lsb.get_విలువ();

	  స్థితి vpi_సూచీ_2:
	    తిరిగి (పూర్ణాంకం)get_చిరునామా() + array->మొదటి_addr.get_విలువ();

	  స్థితి vpiAutomatic:
	    తిరిగి array->get_scope()->is_automatic() ? 1 : 0;

#ఒకవేళ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  స్థితి _vpiFromThr:
	    తిరిగి _vpi_at_A;
#అయితే_ముగింపు

	  // If address_handle is not zero we definitely have a
	  // variable.
	  స్థితి vpiConstantSelect:
	    తిరిగి చిరునామా_handle == 0;

	  అప్రమేయ:
	    తిరిగి 0;
      }
}

అక్షరం* __vpiArrayVthrA::vpi_get_str(పూర్ణాంకం code)
{
      ఒకవేళ (code == vpiFile) {  // Not implemented for now!
            తిరిగి simple_సమితి_rbuf_str(file_పేర్లు[0]);
      }

      అక్షరం sidx [64];
      snprintf(sidx, 63, "%d", (పూర్ణాంకం)get_చిరునామా() + array->మొదటి_addr.get_విలువ());
      తిరిగి generic_get_str(code, array->get_scope(), array->name, sidx);
}

శూన్య __vpiArrayVthrA::vpi_get_విలువ(p_vpi_విలువ vp)
{
      ప్రతిజ్ఞా(array);

      గుర్తులేని సూచీ = get_చిరునామా();
      ఒకవేళ (vpi_array_is_real(array)) {
	    ద్విగుణ tmp = array->get_మాట_r(సూచీ);
	    vpip_real_get_విలువ(tmp, vp);
      } లేకపోతే ఒకవేళ (vpi_array_is_string(array)) {
	    string tmp = array->get_మాట_str(సూచీ);
	    vpip_string_get_విలువ(tmp, vp);
      } లేకపోతే {
	    vvp_vector4_t tmp = array->get_మాట(సూచీ);
	    గుర్తులేని width = array->get_మాట_కొలత();
	    vpip_vec4_get_విలువ(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_విలువ(p_vpi_విలువ vp, పూర్ణాంకం)
{
      గుర్తులేని సూచీ = get_చిరునామా();

      ప్రతిజ్ఞా(array);
      ప్రతిజ్ఞా(సూచీ < array->get_కొలత());

      ఒకవేళ (vpi_array_is_real(array)) {
	    ద్విగుణ val = real_from_vpi_విలువ(vp);
	    array->సమితి_మాట(సూచీ, val);
      } లేకపోతే {
	    గుర్తులేని width = array->get_మాట_కొలత();
	    vvp_vector4_t val = vec4_from_vpi_విలువ(vp, width);
	    array->సమితి_మాట(సూచీ, 0, val);
      }

      తిరిగి ఇది;
}

vpiHandle __vpiArrayVthrA::vpi_handle(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpi_సూచీ_2:
	    ఆపు;  // Not implemented!

	  స్థితి vpi_ఎడమ_Range:
	    తిరిగి &array->msb;

	  స్థితి vpi_కుడి_Range:
	    తిరిగి &array->lsb;

	  స్థితి vpi_మాతృ:
	  స్థితి vpiArray:
	    తిరిగి array;

	  స్థితి vpiScope:
	    తిరిగి array->get_scope();

	  స్థితి vpiModule:
	    తిరిగి vpip_module(array->get_scope());
      }

      తిరిగి 0;
}


పూర్ణాంకం __vpiArrayVthrAPV::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpi_పంక్తి_No:
	    తిరిగి 0; // Not implemented for now!

	  స్థితి vpi_కొలత:
	    తిరిగి part_wid;

	  స్థితి vpi_ఎడమ_Range:
	    తిరిగి part_బిట్టు + part_wid - 1;

	  స్థితి vpi_కుడి_Range:
	    తిరిగి part_బిట్టు;

	  స్థితి vpiAutomatic:
	    తిరిగి array->get_scope()->is_automatic() ? 1 : 0;

#ఒకవేళ defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  స్థితి _vpiFromThr:
	    తిరిగి _vpi_at_APV;
#అయితే_ముగింపు

	  స్థితి vpiConstantSelect:
	    తిరిగి 1;

	  అప్రమేయ:
	    తిరిగి 0;
      }
}

అక్షరం* __vpiArrayVthrAPV::vpi_get_str(పూర్ణాంకం code)
{
      ఒకవేళ (code == vpiFile) {  // Not implemented for now!
            తిరిగి simple_సమితి_rbuf_str(file_పేర్లు[0]);
      }

      అక్షరం sidx [64];
      snprintf(sidx, 63, "%u", మాట_sel + array->మొదటి_addr.get_విలువ());
      తిరిగి generic_get_str(code, array->get_scope(), array->name, sidx);
}

శూన్య __vpiArrayVthrAPV::vpi_get_విలువ(p_vpi_విలువ vp)
{
      ప్రతిజ్ఞా(array);

      గుర్తులేని సూచీ = మాట_sel;
      ఒకవేళ (vpi_array_is_real(array)) {
	    ద్విగుణ tmp = array->get_మాట_r(సూచీ);
	    vpip_real_get_విలువ(tmp, vp);
      } లేకపోతే {
	    vvp_vector4_t tmp = array->get_మాట(సూచీ);
	    tmp = tmp.subvalue(part_బిట్టు, part_wid);
	    vpip_vec4_get_విలువ(tmp, part_wid, అబద్ధం, vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(పూర్ణాంకం code)
{
      ఎంపిక (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  స్థితి vpi_మాతృ:
	    తిరిగి 0;

            // Not part of the Verilog standard. We use this internally.
	  స్థితి vpiArray:
	    తిరిగి array;

	  స్థితి vpiScope:
	    తిరిగి array->get_scope();

	  స్థితి vpiModule:
	    తిరిగి vpip_module(array->get_scope());
      }

      తిరిగి 0;
}

శూన్య __vpiArray::సమితి_మాట(గుర్తులేని చిరునామా, గుర్తులేని part_off, స్థిరరాశి vvp_vector4_t&val)
{
      ఒకవేళ (చిరునామా >= get_కొలత())
	    తిరిగి;

      ఒకవేళ (vals4) {
	    ప్రతిజ్ఞా(nets == 0);
	    ఒకవేళ (part_off != 0 || val.size() != vals_వెడల్పు) {
		  vvp_vector4_t tmp = vals4->get_మాట(చిరునామా);
		  ఒకవేళ ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_వెడల్పు << పంక్తి_ముగింపు;
			ప్రతిజ్ఞా(0);
		  }
		  tmp.సమితి_vec(part_off, val);
		  vals4->సమితి_మాట(చిరునామా, tmp);
	    } లేకపోతే {
		  vals4->సమితి_మాట(చిరునామా, val);
	    }
	    మాట_change(చిరునామా);
	    తిరిగి;
      }

      ఒకవేళ (vals) {
	    ప్రతిజ్ఞా(nets == 0);
	    ఒకవేళ (part_off != 0 || val.size() != vals_వెడల్పు) {
		  vvp_vector4_t tmp;
		  vals->get_మాట(చిరునామా, tmp);
		  ఒకవేళ ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_వెడల్పు << పంక్తి_ముగింపు;
			ప్రతిజ్ఞా(0);
		  }
		  tmp.సమితి_vec(part_off, val);
		  vals->సమితి_మాట(చిరునామా, tmp);
	    } లేకపోతే {
		  vals->సమితి_మాట(చిరునామా, val);
	    }
	    మాట_change(చిరునామా);
	    తిరిగి;
      }

      ప్రతిజ్ఞా(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle మాట = nets[చిరునామా];
      స్థిరరాశి నిర్మాణం __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(మాట);
      ప్రతిజ్ఞా(vsig);

      vsig->కణుపు->send_vec4_pv(val, part_off, vpip_కొలత(vsig), 0);
      మాట_change(చిరునామా);
}

శూన్య __vpiArray::సమితి_మాట(గుర్తులేని చిరునామా, ద్విగుణ val)
{
      ప్రతిజ్ఞా(vals != 0);
      ప్రతిజ్ఞా(nets == 0);

      ఒకవేళ (చిరునామా >= vals->get_కొలత())
	    తిరిగి;

      vals->సమితి_మాట(చిరునామా, val);
      మాట_change(చిరునామా);
}

శూన్య __vpiArray::సమితి_మాట(గుర్తులేని చిరునామా, స్థిరరాశి string&val)
{
      ప్రతిజ్ఞా(vals != 0);
      ప్రతిజ్ఞా(nets == 0);

      ఒకవేళ (చిరునామా >= vals->get_కొలత())
	    తిరిగి;

      vals->సమితి_మాట(చిరునామా, val);
      మాట_change(చిరునామా);
}

శూన్య __vpiArray::సమితి_మాట(గుర్తులేని చిరునామా, స్థిరరాశి vvp_object_t&val)
{
      ప్రతిజ్ఞా(vals != 0);
      ప్రతిజ్ఞా(nets == 0);

      ఒకవేళ (చిరునామా >= vals->get_కొలత())
	    తిరిగి;

      vals->సమితి_మాట(చిరునామా, val);
      మాట_change(చిరునామా);
}

vvp_vector4_t __vpiArray::get_మాట(గుర్తులేని చిరునామా)
{
      ఒకవేళ (vals4) {
	    ప్రతిజ్ఞా(nets == 0);
	    ప్రతిజ్ఞా(vals == 0);
	    తిరిగి vals4->get_మాట(చిరునామా);
      }

      ఒకవేళ (vals) {
	    ప్రతిజ్ఞా(nets == 0);
	    ప్రతిజ్ఞా(vals4== 0);
	    ఒకవేళ (చిరునామా >= vals->get_కొలత())
		  తిరిగి vvp_vector4_t(vals_వెడల్పు, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_మాట(చిరునామా, val);
	    తిరిగి val;
      }

      ప్రతిజ్ఞా(vals4 == 0);
      ప్రతిజ్ఞా(vals == 0);
      ప్రతిజ్ఞా(nets != 0);

      ఒకవేళ (చిరునామా >= get_కొలత()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    ప్రతిజ్ఞా(get_కొలత() > 0);
	    vpiHandle మాట = nets[0];
	    ప్రతిజ్ఞా(మాట);
	    నిర్మాణం __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(మాట);
	    ప్రతిజ్ఞా(vsig);
	    vvp_signal_విలువ*sig = dynamic_cast<vvp_signal_విలువ*> (vsig->కణుపు->fil);
	    ప్రతిజ్ఞా(sig);
	    తిరిగి vvp_vector4_t(sig->విలువ_కొలత(), BIT4_X);
      }

      vpiHandle మాట = nets[చిరునామా];
      నిర్మాణం __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(మాట);
      ప్రతిజ్ఞా(vsig);
      vvp_signal_విలువ*sig = dynamic_cast<vvp_signal_విలువ*> (vsig->కణుపు->fil);
      ప్రతిజ్ఞా(sig);

      vvp_vector4_t val;
      sig->vec4_విలువ(val);
      తిరిగి val;
}

ద్విగుణ __vpiArray::get_మాట_r(గుర్తులేని చిరునామా)
{
      ఒకవేళ (vals) {
	    ప్రతిజ్ఞా(vals4 == 0);
	    ప్రతిజ్ఞా(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    ఒకవేళ (చిరునామా >= vals->get_కొలత())
		  తిరిగి 0.0;

	    ద్విగుణ val;
	    vals->get_మాట(చిరునామా, val);
	    తిరిగి val;
      }

      ఒకవేళ (చిరునామా >= get_కొలత())
	    తిరిగి 0.0;

      ప్రతిజ్ఞా(nets);
      vpiHandle మాట = nets[చిరునామా];
      నిర్మాణం __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(మాట);
      ప్రతిజ్ఞా(vsig);
      vvp_signal_విలువ*sig = dynamic_cast<vvp_signal_విలువ*> (vsig->net->fil);
      ప్రతిజ్ఞా(sig);

      ద్విగుణ val = sig->real_విలువ();
      తిరిగి val;
}

శూన్య __vpiArray::get_మాట_obj(గుర్తులేని చిరునామా, vvp_object_t&val)
{
      ఒకవేళ (vals) {
	    ప్రతిజ్ఞా(vals4 == 0);
	    ప్రతిజ్ఞా(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    ఒకవేళ (చిరునామా >= vals->get_కొలత()) {
		  val = vvp_object_t();
		  తిరిగి;
	    }

	    vals->get_మాట(చిరునామా, val);
	    తిరిగి;
      }

      ప్రతిజ్ఞా(nets);
	// Arrays of string nets not implemented!
      ప్రతిజ్ఞా(0);
      తిరిగి;
}

string __vpiArray::get_మాట_str(గుర్తులేని చిరునామా)
{
      ఒకవేళ (vals) {
	    ప్రతిజ్ఞా(vals4 == 0);
	    ప్రతిజ్ఞా(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    ఒకవేళ (చిరునామా >= vals->get_కొలత())
		  తిరిగి "";

	    string val;
	    vals->get_మాట(చిరునామా, val);
	    తిరిగి val;
      }

      ప్రతిజ్ఞా(nets);
	// Arrays of string nets not implemented!
      ప్రతిజ్ఞా(0);
      తిరిగి "";
}

vpiHandle vpip_make_array(స్థిరరాశి అక్షరం*label, స్థిరరాశి అక్షరం*name,
                          పూర్ణాంకం మొదటి_addr, పూర్ణాంకం చివరి_addr,
                          తార్కిక signed_flag)
{
      నిర్మాణం __vpiArray*obj = కొత్త __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      ఒకవేళ (చివరి_addr >= మొదటి_addr) {
	    obj->మార్పిడి_addr = అబద్ధం;
      } లేకపోతే {
	    obj->మార్పిడి_addr = నిజం;
	    పూర్ణాంకం tmp = చివరి_addr;
	    చివరి_addr = మొదటి_addr;
	    మొదటి_addr = tmp;
      }
      ప్రతిజ్ఞా(చివరి_addr >= మొదటి_addr);
      గుర్తులేని array_లెక్క = చివరి_addr+1-మొదటి_addr;

	// For now, treat all arrays as memories. This is not quite
	// correct, as arrays are arrays with memories a special case.
      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_లెక్క = array_లెక్క;

      obj->మొదటి_addr.సమితి_విలువ(మొదటి_addr);
      obj->చివరి_addr .సమితి_విలువ(చివరి_addr);

	// Start off now knowing if we are nets or variables.
      obj->nets = 0;
      obj->vals4 = 0;
      obj->vals  = 0;
      obj->vals_వెడల్పు = 0;
      obj->vals_పదాలు = 0;

	// Initialize (clear) the read-ports list.
      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

	/* Add this symbol to the array_symbols table for later lookup. */
      ఒకవేళ (!array_పట్టిక)
	    array_పట్టిక = కొత్త చిహ్నం_map_s<నిర్మాణం __vpiArray>;

      ప్రతిజ్ఞా(!array_కనుగొను(label));
      array_పట్టిక->sym_సమితి_విలువ(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_చిహ్నం(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_గమ్యం_current_scope(obj);

      తిరిగి obj;
}

శూన్య __vpiArray::alias_మాట(గుర్తులేని దీర్ఘ addr, vpiHandle మాట, పూర్ణాంకం msb_, పూర్ణాంకం lsb_)
{
      ప్రతిజ్ఞా(msb.get_విలువ() == msb_);
      ప్రతిజ్ఞా(lsb.get_విలువ() == lsb_);
      ప్రతిజ్ఞా(addr < get_కొలత());
      ప్రతిజ్ఞా(nets);
      nets[addr] = మాట;
}

శూన్య __vpiArray::attach_మాట(గుర్తులేని addr, vpiHandle మాట)
{
      ప్రతిజ్ఞా(addr < get_కొలత());
      ప్రతిజ్ఞా(nets);
      nets[addr] = మాట;

      ఒకవేళ (నిర్మాణం __vpiSignal*sig = dynamic_cast<__vpiSignal*>(మాట)) {
	    vvp_net_t*net = sig->కణుపు;
	    ప్రతిజ్ఞా(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    ప్రతిజ్ఞా(fun);
	    fun->attach_as_మాట(ఇది, addr);
	    sig->is_netarray = 1;
	    sig->within.మాతృ = ఇది;
	    sig->id.సూచీ = కొత్త __vpiDecConst(addr + మొదటి_addr.get_విలువ());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    తిరిగి;
      }

      ఒకవేళ (నిర్మాణం __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(మాట)) {
	    vvp_net_t*net = sig->net;
	    ప్రతిజ్ఞా(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    ప్రతిజ్ఞా(fun);
	    fun->attach_as_మాట(ఇది, addr);
	    sig->is_netarray = 1;
	    sig->within.మాతృ = ఇది;
	    sig->id.సూచీ = కొత్త __vpiDecConst(addr + మొదటి_addr.get_విలువ());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = నిజం;
	    తిరిగి;
      }
}

శూన్య compile_var_array(అక్షరం*label, అక్షరం*name, పూర్ణాంకం చివరి, పూర్ణాంకం మొదటి,
		   పూర్ణాంకం msb, పూర్ణాంకం lsb, అక్షరం signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, మొదటి, చివరి,
                                      signed_flag != 0);

      నిర్మాణం __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_వెడల్పు = labs(msb-lsb) + 1;
      ఒకవేళ (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = కొత్త vvp_vector4array_aa(arr->vals_వెడల్పు,
						 arr->get_కొలత());
      } లేకపోతే {
            arr->vals4 = కొత్త vvp_vector4array_sa(arr->vals_వెడల్పు,
						 arr->get_కొలత());
      }
      arr->msb.సమితి_విలువ(msb);
      arr->lsb.సమితి_విలువ(lsb);

      లెక్క_var_arrays += 1;
      లెక్క_var_array_పదాలు += arr->get_కొలత();

      విడుదల(label);
      తొలగించు[] name;
}

శూన్య compile_var2_array(అక్షరం*label, అక్షరం*name, పూర్ణాంకం చివరి, పూర్ణాంకం మొదటి,
		   పూర్ణాంకం msb, పూర్ణాంకం lsb, తార్కిక signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, మొదటి, చివరి, signed_flag);

      నిర్మాణం __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.సమితి_విలువ(msb);
      arr->lsb.సమితి_విలువ(lsb);
      arr->vals_వెడల్పు = labs(msb-lsb) + 1;

      ప్రతిజ్ఞా(! arr->nets);
      ఒకవేళ (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = కొత్త vvp_darray_atom<int8_t>(arr->get_కొలత());
      } లేకపోతే ఒకవేళ (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = కొత్త vvp_darray_atom<uint8_t>(arr->get_కొలత());
      } లేకపోతే ఒకవేళ (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = కొత్త vvp_darray_atom<int16_t>(arr->get_కొలత());
      } లేకపోతే ఒకవేళ (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = కొత్త vvp_darray_atom<uint16_t>(arr->get_కొలత());
      } లేకపోతే ఒకవేళ (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = కొత్త vvp_darray_atom<int32_t>(arr->get_కొలత());
      } లేకపోతే ఒకవేళ (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = కొత్త vvp_darray_atom<uint32_t>(arr->get_కొలత());
      } లేకపోతే ఒకవేళ (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = కొత్త vvp_darray_atom<int64_t>(arr->get_కొలత());
      } లేకపోతే ఒకవేళ (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = కొత్త vvp_darray_atom<uint64_t>(arr->get_కొలత());
      } లేకపోతే {
	    arr->vals = కొత్త vvp_darray_vec2(arr->get_కొలత(), arr->vals_వెడల్పు);
      }
      లెక్క_var_arrays += 1;
      లెక్క_var_array_పదాలు += arr->get_కొలత();

      విడుదల(label);
      తొలగించు[] name;
}

శూన్య compile_real_array(అక్షరం*label, అక్షరం*name, పూర్ణాంకం చివరి, పూర్ణాంకం మొదటి)
{
      vpiHandle obj = vpip_make_array(label, name, మొదటి, చివరి, నిజం);

      నిర్మాణం __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = కొత్త vvp_darray_real(arr->get_కొలత());
      arr->vals_వెడల్పు = 1;

      లెక్క_real_arrays += 1;
      లెక్క_real_array_పదాలు += arr->get_కొలత();

      విడుదల(label);
      తొలగించు[] name;
}

శూన్య compile_string_array(అక్షరం*label, అక్షరం*name, పూర్ణాంకం చివరి, పూర్ణాంకం మొదటి)
{
      vpiHandle obj = vpip_make_array(label, name, మొదటి, చివరి, నిజం);

      నిర్మాణం __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = కొత్త vvp_darray_string(arr->get_కొలత());
      arr->vals_వెడల్పు = 1;

      లెక్క_real_arrays += 1;
      లెక్క_real_array_పదాలు += arr->get_కొలత();

      విడుదల(label);
      తొలగించు[] name;
}

శూన్య compile_object_array(అక్షరం*label, అక్షరం*name, పూర్ణాంకం చివరి, పూర్ణాంకం మొదటి)
{
      vpiHandle obj = vpip_make_array(label, name, మొదటి, చివరి, నిజం);

      నిర్మాణం __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = కొత్త vvp_darray_object(arr->get_కొలత());
      arr->vals_వెడల్పు = 1;

      లెక్క_real_arrays += 1;
      లెక్క_real_array_పదాలు += arr->get_కొలత();

      విడుదల(label);
      తొలగించు[] name;
}

శూన్య compile_net_array(అక్షరం*label, అక్షరం*name, పూర్ణాంకం చివరి, పూర్ణాంకం మొదటి)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, మొదటి, చివరి, అబద్ధం);

      నిర్మాణం __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(సున్నా_జ్ఞాపకం_ఇవ్వు(arr->get_కొలత(), పరిమాణం(vpiHandle)));

      లెక్క_net_arrays += 1;
      లెక్క_net_array_పదాలు += arr->get_కొలత();

      విడుదల(label);
      తొలగించు[] name;
}

తరగతి vvp_fun_arrayport  : బహిరంగ vvp_net_fun_t {

    బహిరంగ:
      స్పష్టమైన vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      స్పష్టమైన vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, దీర్ఘ addr);
      ~vvp_fun_arrayport() override;

      మాయా శూన్య check_మాట_change(గుర్తులేని దీర్ఘ addr) = 0;

    రక్షిత:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      గుర్తులేని దీర్ఘ addr_;

      మిత్ర శూన్య array_attach_పోర్టు(vvp_array_t, vvp_fun_arrayport*);
      మిత్ర శూన్య __vpiArray::మాట_change(గుర్తులేని దీర్ఘ);
      vvp_fun_arrayport*తదుపరి;
};

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net)
: arr_(mem), net_(net), addr_(0)
{
      తదుపరి = 0;
}

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, దీర్ఘ addr)
: arr_(mem), net_(net), addr_(addr)
{
      తదుపరి = 0;
}

vvp_fun_arrayport::~vvp_fun_arrayport()
{
}

తరగతి vvp_fun_arrayport_sa  : బహిరంగ vvp_fun_arrayport {

    బహిరంగ:
      స్పష్టమైన vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      స్పష్టమైన vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, దీర్ఘ addr);
      ~vvp_fun_arrayport_sa() override;

      శూన్య check_మాట_change(గుర్తులేని దీర్ఘ addr) override;

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;

    అంతరంగ:
};

vvp_fun_arrayport_sa::vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net)
{
}

vvp_fun_arrayport_sa::vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, దీర్ఘ addr)
: vvp_fun_arrayport(mem, net, addr)
{
}

vvp_fun_arrayport_sa::~vvp_fun_arrayport_sa()
{
}

శూన్య vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                     vvp_context_t)
{
      తార్కిక addr_valid_flag;

      ఎంపిక (పోర్టు.పోర్టు()) {

	  స్థితి 0: // Address input
	    addr_valid_flag = vector4_గమ్యం_విలువ(బిట్టు, addr_);
	    ఒకవేళ (! addr_valid_flag)
		  addr_ = arr_->get_కొలత();
	    ఒకవేళ (vpi_array_is_real(arr_))
		  పోర్టు.ptr()->send_real(arr_->get_మాట_r(addr_), 0);
	    లేకపోతే
		  పోర్టు.ptr()->send_vec4(arr_->get_మాట(addr_), 0);

	    ఆపు;

	  అప్రమేయ:
	    fprintf(stdout, "XXXX write ports not implemented.\n");
	    ప్రతిజ్ఞా(0);
      }
}

శూన్య vvp_fun_arrayport_sa::check_మాట_change(గుర్తులేని దీర్ఘ addr)
{
      ఒకవేళ (addr != addr_) తిరిగి;

      ఒకవేళ (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_మాట_r(addr_), 0);
      } లేకపోతే {
	    net_->send_vec4(arr_->get_మాట(addr_), 0);
      }
}

తరగతి vvp_fun_arrayport_aa  : బహిరంగ vvp_fun_arrayport, బహిరంగ automatic_hooks_s {

    బహిరంగ:
      స్పష్టమైన vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net);
      స్పష్టమైన vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, దీర్ఘ addr);
      ~vvp_fun_arrayport_aa() override;

      శూన్య alloc_instance(vvp_context_t context) override;
      శూన్య reset_instance(vvp_context_t context) override;
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      శూన్య free_instance(vvp_context_t context) override;
#అయితే_ముగింపు

      శూన్య check_మాట_change(గుర్తులేని దీర్ఘ addr) override;

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t context) override;

    అంతరంగ:
      శూన్య check_మాట_change_2(గుర్తులేని దీర్ఘ addr, vvp_context_t context);

      __vpiScope*context_scope_;
      గుర్తులేని context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_కూడు_అంశం_గమ్యం_context(ఇది, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, దీర్ఘ addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_కూడు_అంశం_గమ్యం_context(ఇది, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

శూన్య vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      గుర్తులేని దీర్ఘ*addr = కొత్త గుర్తులేని దీర్ఘ;
      vvp_సమితి_context_అంశం(context, context_idx_, addr);

      *addr = addr_;
}

శూన్య vvp_fun_arrayport_aa::reset_instance(vvp_context_t context)
{
      గుర్తులేని దీర్ఘ*addr = static_cast<గుర్తులేని దీర్ఘ*>
            (vvp_get_context_అంశం(context, context_idx_));

      *addr = addr_;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య vvp_fun_arrayport_aa::free_instance(vvp_context_t context)
{
      గుర్తులేని దీర్ఘ*addr = static_cast<గుర్తులేని దీర్ఘ*>
            (vvp_get_context_అంశం(context, context_idx_));
      తొలగించు addr;
}
#అయితే_ముగింపు

శూన్య vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                     vvp_context_t context)
{
      ఒకవేళ (context) {
            గుర్తులేని దీర్ఘ*addr = static_cast<గుర్తులేని దీర్ఘ*>
                  (vvp_get_context_అంశం(context, context_idx_));

            తార్కిక addr_valid_flag;

            ఎంపిక (పోర్టు.పోర్టు()) {

                స్థితి 0: // Address input
                  addr_valid_flag = vector4_గమ్యం_విలువ(బిట్టు, *addr);
                  ఒకవేళ (! addr_valid_flag) *addr = arr_->get_కొలత();
                  ఒకవేళ (vpi_array_is_real(arr_)) {
			పోర్టు.ptr()->send_real(arr_->get_మాట_r(*addr),
					      context);
                  } లేకపోతే {
			పోర్టు.ptr()->send_vec4(arr_->get_మాట(*addr),
					      context);
                  }
                  ఆపు;

                అప్రమేయ:
                  fprintf(stdout, "XXXX write ports not implemented.\n");
                  ప్రతిజ్ఞా(0);
            }
      } లేకపోతే {
            context = context_scope_->సజీవ_contexts;
            అయితే (context) {
                  recv_vec4(పోర్టు, బిట్టు, context);
                  context = vvp_get_తదుపరి_context(context);
            }
      }
}

శూన్య vvp_fun_arrayport_aa::check_మాట_change_2(గుర్తులేని దీర్ఘ addr,
                                              vvp_context_t context)
{
      స్థిరరాశి గుర్తులేని దీర్ఘ*పోర్టు_addr = static_cast<గుర్తులేని దీర్ఘ*>
            (vvp_get_context_అంశం(context, context_idx_));

      ఒకవేళ (addr != *పోర్టు_addr)
	    తిరిగి;

      ఒకవేళ (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_మాట_r(addr), context);
      } లేకపోతే {
	    net_->send_vec4(arr_->get_మాట(addr), context);
      }
}

శూన్య vvp_fun_arrayport_aa::check_మాట_change(గుర్తులేని దీర్ఘ addr)
{
      ఒకవేళ (arr_->get_scope()->is_automatic()) {
            ప్రతిజ్ఞా(vthread_get_wt_context());
            check_మాట_change_2(addr, vthread_get_wt_context());
      } లేకపోతే {
            vvp_context_t context = context_scope_->సజీవ_contexts;
            అయితే (context) {
                  check_మాట_change_2(addr, context);
                  context = vvp_get_తదుపరి_context(context);
            }
      }
}

స్థిర శూన్య array_attach_పోర్టు(vvp_array_t array, vvp_fun_arrayport*fun)
{
      ప్రతిజ్ఞా(fun->తదుపరి == 0);
      fun->తదుపరి = array->ports_;
      array->ports_ = fun;
      ఒకవేళ (!array->get_scope()->is_automatic() &&
          (array->vals4 || array->vals)) {
              /* propagate initial values for variable arrays */
            ఒకవేళ (!vpi_array_is_real(array)) {
                  vvp_bit4_t init;
                  ఒకవేళ (array->vals4)
                      init = BIT4_X;
                  లేకపోతే
                      init = BIT4_0;
                  vvp_vector4_t tmp(array->vals_వెడల్పు, init);
                  కాలపట్టిక_init_propagate(fun->net_, tmp);
            } లేకపోతే {
                  కాలపట్టిక_init_propagate(fun->net_, 0.0);
            }
      }
}

తరగతి array_మాట_విలువ_callback : బహిరంగ విలువ_callback {
    బహిరంగ:
      అంతఃవరుస స్పష్టమైన array_మాట_విలువ_callback(p_cb_దత్తాంశం దత్తాంశం, దీర్ఘ addr)
      : విలువ_callback(దత్తాంశం), మాట_addr(addr)
      { }

    బహిరంగ:
      దీర్ఘ మాట_addr;
};

శూన్య __vpiArray::మాట_change(గుర్తులేని దీర్ఘ addr)
{
      కొరకు (vvp_fun_arrayport*cur = ports_; cur; cur = cur->తదుపరి)
	    cur->check_మాట_change(addr);

	// Run callbacks attached to the array itself.
      నిర్మాణం __vpiCallback *తదుపరి_2 = vpi_callbacks;
      నిర్మాణం __vpiCallback *prev = 0;

      అయితే (తదుపరి_2) {
	    array_మాట_విలువ_callback*cur = dynamic_cast<array_మాట_విలువ_callback*>(తదుపరి_2);
	    తదుపరి_2 = cur->తదుపరి_2;

	      // Skip callbacks that are not for me. -1 is for every element.
	    ఒకవేళ (cur->మాట_addr != (దీర్ఘ)addr && cur->మాట_addr != -1) {
		  prev = cur;
		  కొనసాగు;
	    }

	      // For whole array callbacks we need to set the index.
	    ఒకవేళ (cur->మాట_addr == -1) {
		  cur->cb_దత్తాంశం.సూచీ = (PLI_INT32) ((పూర్ణాంకం)addr +
						    మొదటి_addr.get_విలువ());
	    }

	    ఒకవేళ (cur->cb_దత్తాంశం.cb_rtn != 0) {
		  ఒకవేళ (cur->test_విలువ_callback_ready()) {
			ఒకవేళ (cur->cb_దత్తాంశం.value) {
			      ఒకవేళ (vpi_array_is_real(ఇది)) {
				    ద్విగుణ val = 0.0;
				    ఒకవేళ (addr < vals->get_కొలత())
					  vals->get_మాట(addr, val);
				    vpip_real_get_విలువ(val, cur->cb_దత్తాంశం.value);
			      } లేకపోతే ఒకవేళ (vals4) {
				    vpip_vec4_get_విలువ(vals4->get_మాట(addr),
							vals_వెడల్పు,
							signed_flag,
							cur->cb_దత్తాంశం.value);
			      } లేకపోతే ఒకవేళ (dynamic_cast<vvp_darray_atom<int8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int64_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint64_t>*>(vals)
				      || dynamic_cast<vvp_darray_vec2*>(vals)) {
				    vvp_vector4_t val;
				    ఒకవేళ (addr < vals->get_కొలత())
					  vals->get_మాట(addr, val);
				    vpip_vec4_get_విలువ(val,
							vals_వెడల్పు,
							signed_flag,
							cur->cb_దత్తాంశం.value);
			      } లేకపోతే {
			            ప్రతిజ్ఞా(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } లేకపోతే ఒకవేళ (prev == 0) {

		  vpi_callbacks = తదుపరి_2;
		  cur->తదుపరి_2 = 0;
		  తొలగించు cur;

	    } లేకపోతే {
		  ప్రతిజ్ఞా(prev->తదుపరి_2 == cur);
		  prev->తదుపరి_2 = తదుపరి_2;
		  cur->తదుపరి_2 = 0;
		  తొలగించు cur;
	    }
      }
}

తరగతి array_resolv_list_t : బహిరంగ resolv_list_s {

    బహిరంగ:
      స్పష్టమైన array_resolv_list_t(అక్షరం*lab) : resolv_list_s(lab) {
	    array = 0;
      }

      vvp_array_t*array;
      తార్కిక resolve(తార్కిక mes) override;

    అంతరంగ:
};

తార్కిక array_resolv_list_t::resolve(తార్కిక mes)
{
      *array = array_కనుగొను(label());
      ఒకవేళ (*array == 0) {
	    ప్రతిజ్ఞా(!mes);
	    తిరిగి అబద్ధం;
      }
      తిరిగి నిజం;
}

తరగతి array_పోర్టు_resolv_list_t : బహిరంగ resolv_list_s {

    బహిరంగ:
      స్పష్టమైన array_పోర్టు_resolv_list_t(అక్షరం* lab, తార్కిక use_addr__,
                                        దీర్ఘ addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      తార్కిక use_addr;
      దీర్ఘ addr;
      తార్కిక resolve(తార్కిక mes) override;

    అంతరంగ:
};

array_పోర్టు_resolv_list_t::array_పోర్టు_resolv_list_t(అక్షరం *lab, తార్కిక use_addr__,
                                                   దీర్ఘ addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      ఒకవేళ (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      లేకపోతే
            context_scope = 0;
      ptr = కొత్త vvp_net_t;
}

తార్కిక array_పోర్టు_resolv_list_t::resolve(తార్కిక mes)
{
      vvp_array_t mem = array_కనుగొను(label());
      ఒకవేళ (mem == 0) {
	    ప్రతిజ్ఞా(mem || !mes);
	    తిరిగి అబద్ధం;
      }

      vvp_fun_arrayport*fun;
      ఒకవేళ (use_addr)
            ఒకవేళ (context_scope)
                  fun = కొత్త vvp_fun_arrayport_aa(context_scope, mem, ptr, addr);
            లేకపోతే
                  fun = కొత్త vvp_fun_arrayport_sa(mem, ptr, addr);
      లేకపోతే
            ఒకవేళ (context_scope)
                  fun = కొత్త vvp_fun_arrayport_aa(context_scope, mem, ptr);
            లేకపోతే
                  fun = కొత్త vvp_fun_arrayport_sa(mem, ptr);
      ptr->fun = fun;

      array_attach_పోర్టు(mem, fun);

      తిరిగి నిజం;
}

తరగతి array_మాట_part_callback : బహిరంగ array_మాట_విలువ_callback {
    బహిరంగ:
      స్పష్టమైన array_మాట_part_callback(p_cb_దత్తాంశం దత్తాంశం, దీర్ఘ addr);
      ~array_మాట_part_callback() override;

      array_మాట_part_callback(స్థిరరాశి array_మాట_part_callback&) = తొలగించు;
      array_మాట_part_callback& క్రియాకారకం=(స్థిరరాశి array_మాట_part_callback&) = తొలగించు;

      తార్కిక test_విలువ_callback_ready(శూన్య) override;

    అంతరంగ:
      అక్షరం*విలువ_బిట్లు;
};

array_మాట_part_callback::array_మాట_part_callback(p_cb_దత్తాంశం దత్తాంశం, దీర్ఘ addr)
: array_మాట_విలువ_callback(దత్తాంశం, addr)
{
	// Get the initial value of the part, to use as a reference.
      నిర్మాణం __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(దత్తాంశం->obj);
      s_vpi_విలువ tmp_విలువ;
      tmp_విలువ.format = vpiBinStrVal;
      apvword->vpi_get_విలువ(&tmp_విలువ);

      విలువ_బిట్లు = కొత్త అక్షరం[apvword->part_wid+1];

      జ్ఞాపకం_నకలు(విలువ_బిట్లు, tmp_విలువ.value.str, apvword->part_wid);
      విలువ_బిట్లు[apvword->part_wid] = 0;
}

array_మాట_part_callback::~array_మాట_part_callback()
{
      తొలగించు[]విలువ_బిట్లు;
}

తార్కిక array_మాట_part_callback::test_విలువ_callback_ready(శూన్య)
{
      నిర్మాణం __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_దత్తాంశం.obj);
      ప్రతిజ్ఞా(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_విలువ tmp_విలువ;
      tmp_విలువ.format = vpiBinStrVal;
      apvword->vpi_get_విలువ(&tmp_విలువ);

      ఒకవేళ (memcmp(విలువ_బిట్లు, tmp_విలువ.value.str, apvword->part_wid) == 0)
	    తిరిగి అబద్ధం;

      జ్ఞాపకం_నకలు(విలువ_బిట్లు, tmp_విలువ.value.str, apvword->part_wid);
      తిరిగి నిజం;

}

విలువ_callback*vpip_array_మాట_change(p_cb_దత్తాంశం దత్తాంశం)
{
      నిర్మాణం __vpiArray*మాతృ = 0;
      array_మాట_విలువ_callback*cbh = 0;
      ఒకవేళ (స్థిరరాశి నిర్మాణం __vpiArrayWord*మాట = array_var_మాట_from_handle(దత్తాంశం->obj)) {
	    మాతృ = static_cast<__vpiArray*>(మాట->get_మాతృ());
	    గుర్తులేని addr = మాట->get_సూచీ();
	    cbh = కొత్త array_మాట_విలువ_callback(దత్తాంశం, addr);

      } లేకపోతే ఒకవేళ (నిర్మాణం __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(దత్తాంశం->obj)) {
	    మాతృ = tword->array;
	    cbh = కొత్త array_మాట_విలువ_callback(దత్తాంశం, tword->చిరునామా);

      } లేకపోతే ఒకవేళ (నిర్మాణం __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(దత్తాంశం->obj)) {
	    మాతృ = apvword->array;
	    cbh = కొత్త array_మాట_part_callback(దత్తాంశం, apvword->మాట_sel);
      }

      ప్రతిజ్ఞా(cbh);
      ప్రతిజ్ఞా(మాతృ);
      cbh->తదుపరి_2 = మాతృ->vpi_callbacks;
      మాతృ->vpi_callbacks = cbh;

      తిరిగి cbh;
}

విలువ_callback* vpip_array_change(p_cb_దత్తాంశం దత్తాంశం)
{
      array_మాట_విలువ_callback*cbh = కొత్త array_మాట_విలువ_callback(దత్తాంశం, -1);
      ప్రతిజ్ఞా(దత్తాంశం->obj);

      నిర్మాణం __vpiArray*arr = dynamic_cast<__vpiArray*>(దత్తాంశం->obj);
      cbh->తదుపరి_2 = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      తిరిగి cbh;
}

శూన్య compile_array_పోర్టు(అక్షరం*label, అక్షరం*array, అక్షరం*addr)
{
      array_పోర్టు_resolv_list_t*resolv_mem
	    = కొత్త array_పోర్టు_resolv_list_t(array, అబద్ధం, 0);

      define_functor_చిహ్నం(label, resolv_mem->ptr);
      విడుదల(label);
	// Connect the port-0 input as the address.
      ఆదానం_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

శూన్య compile_array_పోర్టు(అక్షరం*label, అక్షరం*array, దీర్ఘ addr)
{
      array_పోర్టు_resolv_list_t*resolv_mem
	    = కొత్త array_పోర్టు_resolv_list_t(array, నిజం, addr);

      define_functor_చిహ్నం(label, resolv_mem->ptr);
      విడుదల(label);

      resolv_submit(resolv_mem);
}

శూన్య compile_array_alias(అక్షరం*label, అక్షరం*name, అక్షరం*src)
{
      vvp_array_t mem = array_కనుగొను(src);
      ప్రతిజ్ఞా(mem);

      నిర్మాణం __vpiArray*obj = కొత్త __vpiArray;

      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_లెక్క = mem->array_లెక్క;
      obj->signed_flag = mem->signed_flag;

	// Need to set an accurate range of addresses.
      obj->మొదటి_addr = mem->మొదటి_addr;
      obj->చివరి_addr  = mem->చివరి_addr;
      obj->మార్పిడి_addr = mem->మార్పిడి_addr;

      obj->msb = mem->msb;
      obj->lsb = mem->lsb;

	// Share the words with the source array.
      obj->nets = mem->nets;
      obj->vals4 = mem->vals4;
      obj->vals  = mem->vals;
      obj->vals_వెడల్పు = mem->vals_వెడల్పు;
      obj->vals_పదాలు = mem->vals_పదాలు;

      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

      ప్రతిజ్ఞా(array_పట్టిక);
      ప్రతిజ్ఞా(!array_కనుగొను(label));
      array_పట్టిక->sym_సమితి_విలువ(label, obj);

      compile_vpi_చిహ్నం(label, obj);
      vpip_attach_గమ్యం_current_scope(obj);

      విడుదల(label);
      విడుదల(name);
      విడుదల(src);
}

/*
 * &A<label,addr>
 * This represents a VPI handle for an addressed array. This comes
 * from expressions like "label[addr]" where "label" is the array and
 * "addr" is the canonical address of the desired word.
 */
vpiHandle vpip_make_vthr_A(అక్షరం*label, గుర్తులేని addr)
{
      నిర్మాణం __vpiArrayVthrA*obj = కొత్త __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = కొత్త array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->చిరునామా_handle = 0;
      obj->చిరునామా = addr;

      తిరిగి obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(అక్షరం*label, అక్షరం*చిహ్నం)
{
      నిర్మాణం __vpiArrayVthrA*obj = కొత్త __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = కొత్త array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->చిరునామా_handle = 0;
      compile_vpi_lookup(&obj->చిరునామా_handle, చిహ్నం);
      obj->చిరునామా = 0;

      తిరిగి obj;
}

vpiHandle vpip_make_vthr_A(అక్షరం*label, vpiHandle handle)
{
      నిర్మాణం __vpiArrayVthrA*obj = కొత్త __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = కొత్త array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->చిరునామా_handle = handle;
      obj->చిరునామా = 0;

      తిరిగి obj;
}

vpiHandle vpip_make_vthr_APV(అక్షరం*label, గుర్తులేని సూచీ, గుర్తులేని బిట్టు, గుర్తులేని wid)
{
      నిర్మాణం __vpiArrayVthrAPV*obj = కొత్త __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = కొత్త array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->మాట_sel = సూచీ;
      obj->part_బిట్టు = బిట్టు;
      obj->part_wid = wid;

      తిరిగి obj;
}

శూన్య compile_array_cleanup(శూన్య)
{
      తొలగించు array_పట్టిక;
      array_పట్టిక = 0;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య జ్ఞాపకం_delete(vpiHandle అంశం)
{
      నిర్మాణం __vpiArray*arr = static_cast<నిర్మాణం __vpiArray*>(అంశం);
      ఒకవేళ (arr->vals_పదాలు) తొలగించు [] (arr->vals_పదాలు-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      తొలగించు arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      తొలగించు arr->vals;

      ఒకవేళ (arr->nets) {
	    కొరకు (గుర్తులేని idx = 0; idx < arr->get_కొలత(); idx += 1) {
		  ఒకవేళ (నిర్మాణం __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.సూచీ);
		    /* These should only be the real words. */
		  } లేకపోతే {
			ప్రతిజ్ఞా(arr->nets[idx]->get_type_సంకేతం() ==
			       vpiRealVar);
			నిర్మాణం __vpiRealVar *sigr = static_cast<నిర్మాణం __vpiRealVar *>
			                            (arr->nets[idx]);
			constant_delete(sigr->id.సూచీ);
// Why are only the real words still here?
			తొలగించు arr->nets[idx];
		  }
	    }
	    విడుదల(arr->nets);
      }

      అయితే (arr->vpi_callbacks) {
	    నిర్మాణం __vpiCallback*tmp = arr->vpi_callbacks->తదుపరి_2;
	    తొలగించు arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      తొలగించు arr;
}

శూన్య A_delete(vpiHandle అంశం)
{
      నిర్మాణం __vpiArrayVthrA*obj = static_cast<నిర్మాణం __vpiArrayVthrA*>(అంశం);
      ఒకవేళ (obj->చిరునామా_handle) {
	    ఎంపిక (obj->చిరునామా_handle->get_type_సంకేతం()) {
		స్థితి vpi_జ్ఞాపకం_మాట:
		  ఒకవేళ (vpi_get(_vpiFromThr, obj->చిరునామా_handle) == _vpi_at_A) {
			A_delete(obj->చిరునామా_handle);
		  }
		  ఆపు;
		స్థితి vpiPartSelect:
		  ప్రతిజ్ఞా(vpi_get(_vpiFromThr, obj->చిరునామా_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->చిరునామా_handle);
		  ఆపు;
	    }
      }

      తొలగించు obj;
}

శూన్య APV_delete(vpiHandle అంశం)
{
      నిర్మాణం __vpiArrayVthrAPV*obj = static_cast<నిర్మాణం __vpiArrayVthrAPV*>(అంశం);
      తొలగించు obj;
}
#అయితే_ముగింపు
