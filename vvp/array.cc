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
#که_تعریف_شوی CHECK_WITH_VALGRIND
#ضمن  "vvp_cleanup.h"
#د_که_پای
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <iostream>
# include  "compile.h"
# include  <cassert>
# include  "ivl_alloc.h"

کارول نطاق std;

بینښه اوږد شمېر_net_arrays = 0;
بینښه اوږد شمېر_net_array_ټکي = 0;
بینښه اوږد شمېر_var_arrays = 0;
بینښه اوږد شمېر_var_array_ټکي = 0;
بینښه اوږد شمېر_real_arrays = 0;
بینښه اوږد شمېر_real_array_ټکي = 0;

ساكن نښه_map_s<بنية __vpiArray>* array_جدول =0;

صنف vvp_fun_arrayport;
ساكن فراغ array_attach_پورټ(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_پیدا_کړه(ثابت حرف*label)
{
      إذا (array_جدول == 0)
	    أرجع 0;

      vvp_array_t v = array_جدول->sym_get_ارزښت(label);
      أرجع v;
}

بنية __vpiArrayVthrA : عام __vpiHandle {
      صحيح get_type_کوډ(فراغ) ثابت override { أرجع vpi_حافظه_کلمه; }
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      فراغ vpi_get_ارزښت(p_vpi_ارزښت val) override;
      vpiHandle vpi_put_ارزښت(p_vpi_ارزښت val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح code) override;

      بنية __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle پته_handle;
	// If wid==0, then address is the address into the array.
      بینښه پته;

      بینښه get_پته() ثابت
      {
	    إذا (پته_handle) {
		  s_vpi_ارزښت vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpi_ویکټور_Val;
		  پته_handle->vpi_get_ارزښت(&vp);
		  صحيح ټکي = (پته_handle->vpi_get(vpi_اندازه)-1)/32 + 1;
		  لكل(صحيح idx = 0; idx < ټکي; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			إذا (vp.value.vector[idx].bval != 0) أرجع UINT_MAX;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  پته_handle->vpi_get_ارزښت(&vp);
		  أرجع vp.value.integer;
	    }

	    أرجع پته;
      }
};


بنية __vpiArrayVthrAPV : عام __vpiHandle {
      صحيح get_type_کوډ(فراغ) ثابت override { أرجع vpiPartSelect; }
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      فراغ vpi_get_ارزښت(p_vpi_ارزښت val) override;
      vpiHandle vpi_handle(صحيح code) override;

      بنية __vpiArray*array;
      بینښه کلمه_sel;
      بینښه part_بټ;
      بینښه part_wid;
};

منطقي is_net_array(vpiHandle obj)
{
      بنية __vpiArray*rfp = dynamic_cast<__vpiArray*> (obj);
      أكد(rfp);

      إذا (rfp->nets != 0) أرجع ریښتیا;
      أرجع خطأ;
}

// This function return true if the underlying array words are real.
ساكن منطقي vpi_array_is_real(ثابت vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      إذا (arr->vals4 != 0)  // A bit based variable/register array.
	    أرجع خطأ;

      إذا (dynamic_cast<vvp_darray_real*> (arr->vals))
	    أرجع ریښتیا;

      إذا (arr->vals != 0)
	    أرجع خطأ;

	// This must be a net array so look at element 0 to find the type.
      أكد(arr->nets != 0);
      أكد(arr->get_اندازه() > 0);
      ثابت بنية __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      إذا (rsig) {
	    أرجع ریښتیا;
      }

      أرجع خطأ;
}

ساكن منطقي vpi_array_is_string(ثابت vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      إذا (arr->vals4 != 0)  // A bit based variable/register array.
	    أرجع خطأ;

      إذا (dynamic_cast<vvp_darray_string*> (arr->vals))
	    أرجع ریښتیا;

      أرجع خطأ;
}

صحيح __vpiArray::get_کلمه_اندازه() ثابت
{
      بینښه width;

      أكد(get_اندازه() > 0);
	/* For a net array we need to get the width from the first element. */
      إذا (nets) {
	    أكد(vals4 == 0 && vals == 0);
	    ثابت بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    أكد(vsig);
	    width = vpip_اندازه(vsig);
	/* For a variable array we can get the width from vals_width. */
      } وإلا {
	    أكد(vals4 || vals);
	    width = vals_پلنوالی;
      }

      أرجع width;
}

حرف*__vpiArray::get_کلمه_str(بنية __vpiArrayWord*کلمه, صحيح code)
{
      بینښه شاخص = کلمه->get_شاخص();

      إذا (code == vpiFile) {  // Not implemented for now!
	    أرجع simple_سېټ_rbuf_str(file_نومونه[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%d", (صحيح)شاخص + لومړی_addr.get_ارزښت());
      أرجع generic_get_str(code, get_scope(), name, sidx);
}

فراغ __vpiArray::get_کلمه_ارزښت(بنية __vpiArrayWord*کلمه, p_vpi_ارزښت vp)
{
      بینښه شاخص = کلمه->get_شاخص();

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
          vpip_vec4_get_ارزښت(vals4->get_کلمه(شاخص),
                              vals4->width(), signed_flag, vp);
      } وإلا إذا(vals) {
          بدل(vp->format) {
            حالة vpiBinStrVal:
            حالة vpiOctStrVal:
            حالة vpiDecStrVal:
            حالة vpiHexStrVal:
            حالة vpiScalarVal:
            حالة vpiIntVal:
            حالة vpi_ویکټور_Val:
            {
                vvp_vector4_t v;
                vals->get_کلمه(شاخص, v);
                vpip_vec4_get_ارزښت(v, vals_پلنوالی, signed_flag, vp);
            }
            اكسر;

            حالة vpiRealVal:
            {
                مزدوج d;
                vals->get_کلمه(شاخص, d);
                vpip_real_get_ارزښت(d, vp);
            }
            اكسر;

            حالة vpiStringVal:
            {
                string s;
                vals->get_کلمه(شاخص, s);
                vpip_string_get_ارزښت(s, vp);
            }
            اكسر;

            افتراضي:
                fprintf(stderr, "vpi sorry: format is not implemented\n");
                أكد(خطأ);
          }
      }
}

فراغ __vpiArray::put_کلمه_ارزښت(بنية __vpiArrayWord*کلمه, p_vpi_ارزښت vp, صحيح)
{
      بینښه شاخص = کلمه->get_شاخص();
      vvp_vector4_t val = vec4_from_vpi_ارزښت(vp, vals_پلنوالی);
      سېټ_کلمه(شاخص, 0, val);
}

vpiHandle __vpiArray::get_iter_شاخص(بنية __vpiArrayIterator*, صحيح idx)
{
      إذا (nets) أرجع nets[idx];

      أكد(vals4 || vals);

      إذا (vals_ټکي == 0) make_vals_ټکي();

      أرجع &(vals_ټکي[idx].as_کلمه);
}

صحيح __vpiArray::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_کرښه_No:
	    أرجع 0; // Not implemented for now!

	  حالة vpi_اندازه:
	    أرجع get_اندازه();

	  حالة vpiAutomatic:
	    أرجع scope->is_automatic()? 1 : 0;

	  افتراضي:
	    أرجع 0;
      }
}

حرف* __vpiArray::vpi_get_str(صحيح code)
{
      إذا (code == vpiFile) {  // Not implemented for now!
            أرجع simple_سېټ_rbuf_str(file_نومونه[0]);
      }

      أرجع generic_get_str(code, scope, name, عدم);
}

vpiHandle __vpiArray::vpi_handle(صحيح code)
{
      بدل (code) {
          حالة vpi_کیڼ_Range:
            إذا (بدلون_addr) أرجع &وروستی_addr;
            وإلا أرجع &لومړی_addr;

          حالة vpi_ښی_Range:
            إذا (بدلون_addr) أرجع &لومړی_addr;
            وإلا أرجع &وروستی_addr;

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
vpiHandle __vpiArray::vpi_شاخص(صحيح شاخص)
{
      شاخص -= لومړی_addr.get_ارزښت();
      إذا (شاخص >= (اوږد) get_اندازه())
	    أرجع 0;
      إذا (شاخص < 0)
	    أرجع 0;

      إذا (nets != 0) {
	    أرجع nets[شاخص];
      }

      إذا (vals_ټکي == 0)
	    make_vals_ټکي();

      أرجع &(vals_ټکي[شاخص].as_کلمه);
}

صحيح __vpiArrayWord::as_کلمه_t::vpi_get(صحيح code)
{
      بنية __vpiArrayWord*obj = array_var_کلمه_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_پلار = obj->get_پلار();
      t_vpi_ارزښت val;

      بدل (code) {
	  حالة vpi_کرښه_No:
	    أرجع 0; // Not implemented for now!

	  حالة vpi_اندازه:
	    أرجع my_پلار->get_کلمه_اندازه();

	  حالة vpi_کیڼ_Range:
            val.format = vpiIntVal;
	    my_پلار->get_کیڼ_range()->vpi_get_ارزښت(&val);
            أكد(val.format == vpiIntVal);
            أرجع val.value.integer;

	  حالة vpi_ښی_Range:
            val.format = vpiIntVal;
	    my_پلار->get_ښی_range()->vpi_get_ارزښت(&val);
            أكد(val.format == vpiIntVal);
            أرجع val.value.integer;

	  حالة vpi_شاخص_2:
	    {
		  صحيح بنسټ_دننه_واټن = 0;
		  بنية __vpiArray*بنسټ = dynamic_cast<__vpiArray*> (my_پلار);
		  إذا (بنسټ) {
			val.format = vpiIntVal;
			بنسټ->لومړی_addr.vpi_get_ارزښت(&val);
			بنسټ_دننه_واټن += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_شاخص.vpi_get_ارزښت(&val);
		  أكد(val.format == vpiIntVal);
		  أرجع val.value.integer + بنسټ_دننه_واټن;
	    }

	  حالة vpiAutomatic:
	    أرجع my_پلار->get_scope()->is_automatic()? 1 : 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    أرجع _vpiNoThr;
#د_که_پای

	  افتراضي:
	    أرجع 0;
      }
}


صحيح __vpiArrayVthrA::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_کرښه_No:
	    أرجع 0; // Not implemented for now!

	  حالة vpi_اندازه:
	    أرجع array->get_کلمه_اندازه();

	  حالة vpi_کیڼ_Range:
	    أرجع array->msb.get_ارزښت();

	  حالة vpi_ښی_Range:
	    أرجع array->lsb.get_ارزښت();

	  حالة vpi_شاخص_2:
	    أرجع (صحيح)get_پته() + array->لومړی_addr.get_ارزښت();

	  حالة vpiAutomatic:
	    أرجع array->get_scope()->is_automatic() ? 1 : 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    أرجع _vpi_at_A;
#د_که_پای

	  // If address_handle is not zero we definitely have a
	  // variable.
	  حالة vpiConstantSelect:
	    أرجع پته_handle == 0;

	  افتراضي:
	    أرجع 0;
      }
}

حرف* __vpiArrayVthrA::vpi_get_str(صحيح code)
{
      إذا (code == vpiFile) {  // Not implemented for now!
            أرجع simple_سېټ_rbuf_str(file_نومونه[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%d", (صحيح)get_پته() + array->لومړی_addr.get_ارزښت());
      أرجع generic_get_str(code, array->get_scope(), array->name, sidx);
}

فراغ __vpiArrayVthrA::vpi_get_ارزښت(p_vpi_ارزښت vp)
{
      أكد(array);

      بینښه شاخص = get_پته();
      إذا (vpi_array_is_real(array)) {
	    مزدوج tmp = array->get_کلمه_r(شاخص);
	    vpip_real_get_ارزښت(tmp, vp);
      } وإلا إذا (vpi_array_is_string(array)) {
	    string tmp = array->get_کلمه_str(شاخص);
	    vpip_string_get_ارزښت(tmp, vp);
      } وإلا {
	    vvp_vector4_t tmp = array->get_کلمه(شاخص);
	    بینښه width = array->get_کلمه_اندازه();
	    vpip_vec4_get_ارزښت(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_ارزښت(p_vpi_ارزښت vp, صحيح)
{
      بینښه شاخص = get_پته();

      أكد(array);
      أكد(شاخص < array->get_اندازه());

      إذا (vpi_array_is_real(array)) {
	    مزدوج val = real_from_vpi_ارزښت(vp);
	    array->سېټ_کلمه(شاخص, val);
      } وإلا {
	    بینښه width = array->get_کلمه_اندازه();
	    vvp_vector4_t val = vec4_from_vpi_ارزښت(vp, width);
	    array->سېټ_کلمه(شاخص, 0, val);
      }

      أرجع هذا;
}

vpiHandle __vpiArrayVthrA::vpi_handle(صحيح code)
{
      بدل (code) {
	  حالة vpi_شاخص_2:
	    اكسر;  // Not implemented!

	  حالة vpi_کیڼ_Range:
	    أرجع &array->msb;

	  حالة vpi_ښی_Range:
	    أرجع &array->lsb;

	  حالة vpi_پلار:
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
	  حالة vpi_کرښه_No:
	    أرجع 0; // Not implemented for now!

	  حالة vpi_اندازه:
	    أرجع part_wid;

	  حالة vpi_کیڼ_Range:
	    أرجع part_بټ + part_wid - 1;

	  حالة vpi_ښی_Range:
	    أرجع part_بټ;

	  حالة vpiAutomatic:
	    أرجع array->get_scope()->is_automatic() ? 1 : 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    أرجع _vpi_at_APV;
#د_که_پای

	  حالة vpiConstantSelect:
	    أرجع 1;

	  افتراضي:
	    أرجع 0;
      }
}

حرف* __vpiArrayVthrAPV::vpi_get_str(صحيح code)
{
      إذا (code == vpiFile) {  // Not implemented for now!
            أرجع simple_سېټ_rbuf_str(file_نومونه[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%u", کلمه_sel + array->لومړی_addr.get_ارزښت());
      أرجع generic_get_str(code, array->get_scope(), array->name, sidx);
}

فراغ __vpiArrayVthrAPV::vpi_get_ارزښت(p_vpi_ارزښت vp)
{
      أكد(array);

      بینښه شاخص = کلمه_sel;
      إذا (vpi_array_is_real(array)) {
	    مزدوج tmp = array->get_کلمه_r(شاخص);
	    vpip_real_get_ارزښت(tmp, vp);
      } وإلا {
	    vvp_vector4_t tmp = array->get_کلمه(شاخص);
	    tmp = tmp.subvalue(part_بټ, part_wid);
	    vpip_vec4_get_ارزښت(tmp, part_wid, خطأ, vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(صحيح code)
{
      بدل (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  حالة vpi_پلار:
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

فراغ __vpiArray::سېټ_کلمه(بینښه پته, بینښه part_off, ثابت vvp_vector4_t&val)
{
      إذا (پته >= get_اندازه())
	    أرجع;

      إذا (vals4) {
	    أكد(nets == 0);
	    إذا (part_off != 0 || val.size() != vals_پلنوالی) {
		  vvp_vector4_t tmp = vals4->get_کلمه(پته);
		  إذا ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_پلنوالی << کرښه_پای;
			أكد(0);
		  }
		  tmp.سېټ_vec(part_off, val);
		  vals4->سېټ_کلمه(پته, tmp);
	    } وإلا {
		  vals4->سېټ_کلمه(پته, val);
	    }
	    کلمه_change(پته);
	    أرجع;
      }

      إذا (vals) {
	    أكد(nets == 0);
	    إذا (part_off != 0 || val.size() != vals_پلنوالی) {
		  vvp_vector4_t tmp;
		  vals->get_کلمه(پته, tmp);
		  إذا ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_پلنوالی << کرښه_پای;
			أكد(0);
		  }
		  tmp.سېټ_vec(part_off, val);
		  vals->سېټ_کلمه(پته, tmp);
	    } وإلا {
		  vals->سېټ_کلمه(پته, val);
	    }
	    کلمه_change(پته);
	    أرجع;
      }

      أكد(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle کلمه = nets[پته];
      ثابت بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(کلمه);
      أكد(vsig);

      vsig->غوټه->send_vec4_pv(val, part_off, vpip_اندازه(vsig), 0);
      کلمه_change(پته);
}

فراغ __vpiArray::سېټ_کلمه(بینښه پته, مزدوج val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      إذا (پته >= vals->get_اندازه())
	    أرجع;

      vals->سېټ_کلمه(پته, val);
      کلمه_change(پته);
}

فراغ __vpiArray::سېټ_کلمه(بینښه پته, ثابت string&val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      إذا (پته >= vals->get_اندازه())
	    أرجع;

      vals->سېټ_کلمه(پته, val);
      کلمه_change(پته);
}

فراغ __vpiArray::سېټ_کلمه(بینښه پته, ثابت vvp_object_t&val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      إذا (پته >= vals->get_اندازه())
	    أرجع;

      vals->سېټ_کلمه(پته, val);
      کلمه_change(پته);
}

vvp_vector4_t __vpiArray::get_کلمه(بینښه پته)
{
      إذا (vals4) {
	    أكد(nets == 0);
	    أكد(vals == 0);
	    أرجع vals4->get_کلمه(پته);
      }

      إذا (vals) {
	    أكد(nets == 0);
	    أكد(vals4== 0);
	    إذا (پته >= vals->get_اندازه())
		  أرجع vvp_vector4_t(vals_پلنوالی, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_کلمه(پته, val);
	    أرجع val;
      }

      أكد(vals4 == 0);
      أكد(vals == 0);
      أكد(nets != 0);

      إذا (پته >= get_اندازه()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    أكد(get_اندازه() > 0);
	    vpiHandle کلمه = nets[0];
	    أكد(کلمه);
	    بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(کلمه);
	    أكد(vsig);
	    vvp_signal_ارزښت*sig = dynamic_cast<vvp_signal_ارزښت*> (vsig->غوټه->fil);
	    أكد(sig);
	    أرجع vvp_vector4_t(sig->ارزښت_اندازه(), BIT4_X);
      }

      vpiHandle کلمه = nets[پته];
      بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(کلمه);
      أكد(vsig);
      vvp_signal_ارزښت*sig = dynamic_cast<vvp_signal_ارزښت*> (vsig->غوټه->fil);
      أكد(sig);

      vvp_vector4_t val;
      sig->vec4_ارزښت(val);
      أرجع val;
}

مزدوج __vpiArray::get_کلمه_r(بینښه پته)
{
      إذا (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    إذا (پته >= vals->get_اندازه())
		  أرجع 0.0;

	    مزدوج val;
	    vals->get_کلمه(پته, val);
	    أرجع val;
      }

      إذا (پته >= get_اندازه())
	    أرجع 0.0;

      أكد(nets);
      vpiHandle کلمه = nets[پته];
      بنية __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(کلمه);
      أكد(vsig);
      vvp_signal_ارزښت*sig = dynamic_cast<vvp_signal_ارزښت*> (vsig->net->fil);
      أكد(sig);

      مزدوج val = sig->real_ارزښت();
      أرجع val;
}

فراغ __vpiArray::get_کلمه_obj(بینښه پته, vvp_object_t&val)
{
      إذا (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    إذا (پته >= vals->get_اندازه()) {
		  val = vvp_object_t();
		  أرجع;
	    }

	    vals->get_کلمه(پته, val);
	    أرجع;
      }

      أكد(nets);
	// Arrays of string nets not implemented!
      أكد(0);
      أرجع;
}

string __vpiArray::get_کلمه_str(بینښه پته)
{
      إذا (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    إذا (پته >= vals->get_اندازه())
		  أرجع "";

	    string val;
	    vals->get_کلمه(پته, val);
	    أرجع val;
      }

      أكد(nets);
	// Arrays of string nets not implemented!
      أكد(0);
      أرجع "";
}

vpiHandle vpip_make_array(ثابت حرف*label, ثابت حرف*name,
                          صحيح لومړی_addr, صحيح وروستی_addr,
                          منطقي signed_flag)
{
      بنية __vpiArray*obj = جديد __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      إذا (وروستی_addr >= لومړی_addr) {
	    obj->بدلون_addr = خطأ;
      } وإلا {
	    obj->بدلون_addr = ریښتیا;
	    صحيح tmp = وروستی_addr;
	    وروستی_addr = لومړی_addr;
	    لومړی_addr = tmp;
      }
      أكد(وروستی_addr >= لومړی_addr);
      بینښه array_شمېر = وروستی_addr+1-لومړی_addr;

	// For now, treat all arrays as memories. This is not quite
	// correct, as arrays are arrays with memories a special case.
      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_شمېر = array_شمېر;

      obj->لومړی_addr.سېټ_ارزښت(لومړی_addr);
      obj->وروستی_addr .سېټ_ارزښت(وروستی_addr);

	// Start off now knowing if we are nets or variables.
      obj->nets = 0;
      obj->vals4 = 0;
      obj->vals  = 0;
      obj->vals_پلنوالی = 0;
      obj->vals_ټکي = 0;

	// Initialize (clear) the read-ports list.
      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

	/* Add this symbol to the array_symbols table for later lookup. */
      إذا (!array_جدول)
	    array_جدول = جديد نښه_map_s<بنية __vpiArray>;

      أكد(!array_پیدا_کړه(label));
      array_جدول->sym_سېټ_ارزښت(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_نښه(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_منزل_current_scope(obj);

      أرجع obj;
}

فراغ __vpiArray::alias_کلمه(بینښه اوږد addr, vpiHandle کلمه, صحيح msb_, صحيح lsb_)
{
      أكد(msb.get_ارزښت() == msb_);
      أكد(lsb.get_ارزښت() == lsb_);
      أكد(addr < get_اندازه());
      أكد(nets);
      nets[addr] = کلمه;
}

فراغ __vpiArray::attach_کلمه(بینښه addr, vpiHandle کلمه)
{
      أكد(addr < get_اندازه());
      أكد(nets);
      nets[addr] = کلمه;

      إذا (بنية __vpiSignal*sig = dynamic_cast<__vpiSignal*>(کلمه)) {
	    vvp_net_t*net = sig->غوټه;
	    أكد(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    أكد(fun);
	    fun->attach_as_کلمه(هذا, addr);
	    sig->is_netarray = 1;
	    sig->within.پلار = هذا;
	    sig->id.شاخص = جديد __vpiDecConst(addr + لومړی_addr.get_ارزښت());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    أرجع;
      }

      إذا (بنية __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(کلمه)) {
	    vvp_net_t*net = sig->net;
	    أكد(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    أكد(fun);
	    fun->attach_as_کلمه(هذا, addr);
	    sig->is_netarray = 1;
	    sig->within.پلار = هذا;
	    sig->id.شاخص = جديد __vpiDecConst(addr + لومړی_addr.get_ارزښت());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = ریښتیا;
	    أرجع;
      }
}

فراغ compile_var_array(حرف*label, حرف*name, صحيح وروستی, صحيح لومړی,
		   صحيح msb, صحيح lsb, حرف signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, لومړی, وروستی,
                                      signed_flag != 0);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_پلنوالی = labs(msb-lsb) + 1;
      إذا (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = جديد vvp_vector4array_aa(arr->vals_پلنوالی,
						 arr->get_اندازه());
      } وإلا {
            arr->vals4 = جديد vvp_vector4array_sa(arr->vals_پلنوالی,
						 arr->get_اندازه());
      }
      arr->msb.سېټ_ارزښت(msb);
      arr->lsb.سېټ_ارزښت(lsb);

      شمېر_var_arrays += 1;
      شمېر_var_array_ټکي += arr->get_اندازه();

      حرر(label);
      ړنګول[] name;
}

فراغ compile_var2_array(حرف*label, حرف*name, صحيح وروستی, صحيح لومړی,
		   صحيح msb, صحيح lsb, منطقي signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, لومړی, وروستی, signed_flag);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.سېټ_ارزښت(msb);
      arr->lsb.سېټ_ارزښت(lsb);
      arr->vals_پلنوالی = labs(msb-lsb) + 1;

      أكد(! arr->nets);
      إذا (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = جديد vvp_darray_atom<int8_t>(arr->get_اندازه());
      } وإلا إذا (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = جديد vvp_darray_atom<uint8_t>(arr->get_اندازه());
      } وإلا إذا (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = جديد vvp_darray_atom<int16_t>(arr->get_اندازه());
      } وإلا إذا (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = جديد vvp_darray_atom<uint16_t>(arr->get_اندازه());
      } وإلا إذا (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = جديد vvp_darray_atom<int32_t>(arr->get_اندازه());
      } وإلا إذا (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = جديد vvp_darray_atom<uint32_t>(arr->get_اندازه());
      } وإلا إذا (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = جديد vvp_darray_atom<int64_t>(arr->get_اندازه());
      } وإلا إذا (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = جديد vvp_darray_atom<uint64_t>(arr->get_اندازه());
      } وإلا {
	    arr->vals = جديد vvp_darray_vec2(arr->get_اندازه(), arr->vals_پلنوالی);
      }
      شمېر_var_arrays += 1;
      شمېر_var_array_ټکي += arr->get_اندازه();

      حرر(label);
      ړنګول[] name;
}

فراغ compile_real_array(حرف*label, حرف*name, صحيح وروستی, صحيح لومړی)
{
      vpiHandle obj = vpip_make_array(label, name, لومړی, وروستی, ریښتیا);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = جديد vvp_darray_real(arr->get_اندازه());
      arr->vals_پلنوالی = 1;

      شمېر_real_arrays += 1;
      شمېر_real_array_ټکي += arr->get_اندازه();

      حرر(label);
      ړنګول[] name;
}

فراغ compile_string_array(حرف*label, حرف*name, صحيح وروستی, صحيح لومړی)
{
      vpiHandle obj = vpip_make_array(label, name, لومړی, وروستی, ریښتیا);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = جديد vvp_darray_string(arr->get_اندازه());
      arr->vals_پلنوالی = 1;

      شمېر_real_arrays += 1;
      شمېر_real_array_ټکي += arr->get_اندازه();

      حرر(label);
      ړنګول[] name;
}

فراغ compile_object_array(حرف*label, حرف*name, صحيح وروستی, صحيح لومړی)
{
      vpiHandle obj = vpip_make_array(label, name, لومړی, وروستی, ریښتیا);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = جديد vvp_darray_object(arr->get_اندازه());
      arr->vals_پلنوالی = 1;

      شمېر_real_arrays += 1;
      شمېر_real_array_ټکي += arr->get_اندازه();

      حرر(label);
      ړنګول[] name;
}

فراغ compile_net_array(حرف*label, حرف*name, صحيح وروستی, صحيح لومړی)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, لومړی, وروستی, خطأ);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(حافظه_صفر_ورکړه(arr->get_اندازه(), حجم(vpiHandle)));

      شمېر_net_arrays += 1;
      شمېر_net_array_ټکي += arr->get_اندازه();

      حرر(label);
      ړنګول[] name;
}

صنف vvp_fun_arrayport  : عام vvp_net_fun_t {

    عام:
      څرګند vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      څرګند vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, اوږد addr);
      ~vvp_fun_arrayport() override;

      مجازي فراغ check_کلمه_change(بینښه اوږد addr) = 0;

    محمي:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      بینښه اوږد addr_;

      ملګری فراغ array_attach_پورټ(vvp_array_t, vvp_fun_arrayport*);
      ملګری فراغ __vpiArray::کلمه_change(بینښه اوږد);
      vvp_fun_arrayport*بل;
};

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net)
: arr_(mem), net_(net), addr_(0)
{
      بل = 0;
}

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, اوږد addr)
: arr_(mem), net_(net), addr_(addr)
{
      بل = 0;
}

vvp_fun_arrayport::~vvp_fun_arrayport()
{
}

صنف vvp_fun_arrayport_sa  : عام vvp_fun_arrayport {

    عام:
      څرګند vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      څرګند vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, اوږد addr);
      ~vvp_fun_arrayport_sa() override;

      فراغ check_کلمه_change(بینښه اوږد addr) override;

      فراغ recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                     vvp_context_t) override;

    خاص:
};

vvp_fun_arrayport_sa::vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net)
{
}

vvp_fun_arrayport_sa::vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, اوږد addr)
: vvp_fun_arrayport(mem, net, addr)
{
}

vvp_fun_arrayport_sa::~vvp_fun_arrayport_sa()
{
}

فراغ vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                                     vvp_context_t)
{
      منطقي addr_valid_flag;

      بدل (پورټ.پورټ()) {

	  حالة 0: // Address input
	    addr_valid_flag = vector4_منزل_ارزښت(بټ, addr_);
	    إذا (! addr_valid_flag)
		  addr_ = arr_->get_اندازه();
	    إذا (vpi_array_is_real(arr_))
		  پورټ.ptr()->send_real(arr_->get_کلمه_r(addr_), 0);
	    وإلا
		  پورټ.ptr()->send_vec4(arr_->get_کلمه(addr_), 0);

	    اكسر;

	  افتراضي:
	    fprintf(stdout, "XXXX write ports not implemented.\n");
	    أكد(0);
      }
}

فراغ vvp_fun_arrayport_sa::check_کلمه_change(بینښه اوږد addr)
{
      إذا (addr != addr_) أرجع;

      إذا (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_کلمه_r(addr_), 0);
      } وإلا {
	    net_->send_vec4(arr_->get_کلمه(addr_), 0);
      }
}

صنف vvp_fun_arrayport_aa  : عام vvp_fun_arrayport, عام automatic_hooks_s {

    عام:
      څرګند vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net);
      څرګند vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, اوږد addr);
      ~vvp_fun_arrayport_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#که_تعریف_شوی CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#د_که_پای

      فراغ check_کلمه_change(بینښه اوږد addr) override;

      فراغ recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                     vvp_context_t context) override;

    خاص:
      فراغ check_کلمه_change_2(بینښه اوږد addr, vvp_context_t context);

      __vpiScope*context_scope_;
      بینښه context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_جمع_کړه_توکی_منزل_context(هذا, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, اوږد addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_جمع_کړه_توکی_منزل_context(هذا, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

فراغ vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      بینښه اوږد*addr = جديد بینښه اوږد;
      vvp_سېټ_context_توکی(context, context_idx_, addr);

      *addr = addr_;
}

فراغ vvp_fun_arrayport_aa::reset_instance(vvp_context_t context)
{
      بینښه اوږد*addr = static_cast<بینښه اوږد*>
            (vvp_get_context_توکی(context, context_idx_));

      *addr = addr_;
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ vvp_fun_arrayport_aa::free_instance(vvp_context_t context)
{
      بینښه اوږد*addr = static_cast<بینښه اوږد*>
            (vvp_get_context_توکی(context, context_idx_));
      ړنګول addr;
}
#د_که_پای

فراغ vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                                     vvp_context_t context)
{
      إذا (context) {
            بینښه اوږد*addr = static_cast<بینښه اوږد*>
                  (vvp_get_context_توکی(context, context_idx_));

            منطقي addr_valid_flag;

            بدل (پورټ.پورټ()) {

                حالة 0: // Address input
                  addr_valid_flag = vector4_منزل_ارزښت(بټ, *addr);
                  إذا (! addr_valid_flag) *addr = arr_->get_اندازه();
                  إذا (vpi_array_is_real(arr_)) {
			پورټ.ptr()->send_real(arr_->get_کلمه_r(*addr),
					      context);
                  } وإلا {
			پورټ.ptr()->send_vec4(arr_->get_کلمه(*addr),
					      context);
                  }
                  اكسر;

                افتراضي:
                  fprintf(stdout, "XXXX write ports not implemented.\n");
                  أكد(0);
            }
      } وإلا {
            context = context_scope_->ژوندی_contexts;
            طالما (context) {
                  recv_vec4(پورټ, بټ, context);
                  context = vvp_get_بل_context(context);
            }
      }
}

فراغ vvp_fun_arrayport_aa::check_کلمه_change_2(بینښه اوږد addr,
                                              vvp_context_t context)
{
      ثابت بینښه اوږد*پورټ_addr = static_cast<بینښه اوږد*>
            (vvp_get_context_توکی(context, context_idx_));

      إذا (addr != *پورټ_addr)
	    أرجع;

      إذا (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_کلمه_r(addr), context);
      } وإلا {
	    net_->send_vec4(arr_->get_کلمه(addr), context);
      }
}

فراغ vvp_fun_arrayport_aa::check_کلمه_change(بینښه اوږد addr)
{
      إذا (arr_->get_scope()->is_automatic()) {
            أكد(vthread_get_wt_context());
            check_کلمه_change_2(addr, vthread_get_wt_context());
      } وإلا {
            vvp_context_t context = context_scope_->ژوندی_contexts;
            طالما (context) {
                  check_کلمه_change_2(addr, context);
                  context = vvp_get_بل_context(context);
            }
      }
}

ساكن فراغ array_attach_پورټ(vvp_array_t array, vvp_fun_arrayport*fun)
{
      أكد(fun->بل == 0);
      fun->بل = array->ports_;
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
                  vvp_vector4_t tmp(array->vals_پلنوالی, init);
                  مهالوېش_init_propagate(fun->net_, tmp);
            } وإلا {
                  مهالوېش_init_propagate(fun->net_, 0.0);
            }
      }
}

صنف array_کلمه_ارزښت_callback : عام ارزښت_callback {
    عام:
      مضمن څرګند array_کلمه_ارزښت_callback(p_cb_معلومات معلومات, اوږد addr)
      : ارزښت_callback(معلومات), کلمه_addr(addr)
      { }

    عام:
      اوږد کلمه_addr;
};

فراغ __vpiArray::کلمه_change(بینښه اوږد addr)
{
      لكل (vvp_fun_arrayport*cur = ports_; cur; cur = cur->بل)
	    cur->check_کلمه_change(addr);

	// Run callbacks attached to the array itself.
      بنية __vpiCallback *بل_2 = vpi_callbacks;
      بنية __vpiCallback *prev = 0;

      طالما (بل_2) {
	    array_کلمه_ارزښت_callback*cur = dynamic_cast<array_کلمه_ارزښت_callback*>(بل_2);
	    بل_2 = cur->بل_2;

	      // Skip callbacks that are not for me. -1 is for every element.
	    إذا (cur->کلمه_addr != (اوږد)addr && cur->کلمه_addr != -1) {
		  prev = cur;
		  استمر;
	    }

	      // For whole array callbacks we need to set the index.
	    إذا (cur->کلمه_addr == -1) {
		  cur->cb_معلومات.شاخص = (PLI_INT32) ((صحيح)addr +
						    لومړی_addr.get_ارزښت());
	    }

	    إذا (cur->cb_معلومات.cb_rtn != 0) {
		  إذا (cur->test_ارزښت_callback_ready()) {
			إذا (cur->cb_معلومات.value) {
			      إذا (vpi_array_is_real(هذا)) {
				    مزدوج val = 0.0;
				    إذا (addr < vals->get_اندازه())
					  vals->get_کلمه(addr, val);
				    vpip_real_get_ارزښت(val, cur->cb_معلومات.value);
			      } وإلا إذا (vals4) {
				    vpip_vec4_get_ارزښت(vals4->get_کلمه(addr),
							vals_پلنوالی,
							signed_flag,
							cur->cb_معلومات.value);
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
				    إذا (addr < vals->get_اندازه())
					  vals->get_کلمه(addr, val);
				    vpip_vec4_get_ارزښت(val,
							vals_پلنوالی,
							signed_flag,
							cur->cb_معلومات.value);
			      } وإلا {
			            أكد(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } وإلا إذا (prev == 0) {

		  vpi_callbacks = بل_2;
		  cur->بل_2 = 0;
		  ړنګول cur;

	    } وإلا {
		  أكد(prev->بل_2 == cur);
		  prev->بل_2 = بل_2;
		  cur->بل_2 = 0;
		  ړنګول cur;
	    }
      }
}

صنف array_resolv_list_t : عام resolv_list_s {

    عام:
      څرګند array_resolv_list_t(حرف*lab) : resolv_list_s(lab) {
	    array = 0;
      }

      vvp_array_t*array;
      منطقي resolve(منطقي mes) override;

    خاص:
};

منطقي array_resolv_list_t::resolve(منطقي mes)
{
      *array = array_پیدا_کړه(label());
      إذا (*array == 0) {
	    أكد(!mes);
	    أرجع خطأ;
      }
      أرجع ریښتیا;
}

صنف array_پورټ_resolv_list_t : عام resolv_list_s {

    عام:
      څرګند array_پورټ_resolv_list_t(حرف* lab, منطقي use_addr__,
                                        اوږد addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      منطقي use_addr;
      اوږد addr;
      منطقي resolve(منطقي mes) override;

    خاص:
};

array_پورټ_resolv_list_t::array_پورټ_resolv_list_t(حرف *lab, منطقي use_addr__,
                                                   اوږد addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      إذا (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      وإلا
            context_scope = 0;
      ptr = جديد vvp_net_t;
}

منطقي array_پورټ_resolv_list_t::resolve(منطقي mes)
{
      vvp_array_t mem = array_پیدا_کړه(label());
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

      array_attach_پورټ(mem, fun);

      أرجع ریښتیا;
}

صنف array_کلمه_part_callback : عام array_کلمه_ارزښت_callback {
    عام:
      څرګند array_کلمه_part_callback(p_cb_معلومات معلومات, اوږد addr);
      ~array_کلمه_part_callback() override;

      array_کلمه_part_callback(ثابت array_کلمه_part_callback&) = ړنګول;
      array_کلمه_part_callback& چلوونکی=(ثابت array_کلمه_part_callback&) = ړنګول;

      منطقي test_ارزښت_callback_ready(فراغ) override;

    خاص:
      حرف*ارزښت_بټونه;
};

array_کلمه_part_callback::array_کلمه_part_callback(p_cb_معلومات معلومات, اوږد addr)
: array_کلمه_ارزښت_callback(معلومات, addr)
{
	// Get the initial value of the part, to use as a reference.
      بنية __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(معلومات->obj);
      s_vpi_ارزښت tmp_ارزښت;
      tmp_ارزښت.format = vpiBinStrVal;
      apvword->vpi_get_ارزښت(&tmp_ارزښت);

      ارزښت_بټونه = جديد حرف[apvword->part_wid+1];

      حافظه_نقل(ارزښت_بټونه, tmp_ارزښت.value.str, apvword->part_wid);
      ارزښت_بټونه[apvword->part_wid] = 0;
}

array_کلمه_part_callback::~array_کلمه_part_callback()
{
      ړنګول[]ارزښت_بټونه;
}

منطقي array_کلمه_part_callback::test_ارزښت_callback_ready(فراغ)
{
      بنية __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_معلومات.obj);
      أكد(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_ارزښت tmp_ارزښت;
      tmp_ارزښت.format = vpiBinStrVal;
      apvword->vpi_get_ارزښت(&tmp_ارزښت);

      إذا (memcmp(ارزښت_بټونه, tmp_ارزښت.value.str, apvword->part_wid) == 0)
	    أرجع خطأ;

      حافظه_نقل(ارزښت_بټونه, tmp_ارزښت.value.str, apvword->part_wid);
      أرجع ریښتیا;

}

ارزښت_callback*vpip_array_کلمه_change(p_cb_معلومات معلومات)
{
      بنية __vpiArray*پلار = 0;
      array_کلمه_ارزښت_callback*cbh = 0;
      إذا (ثابت بنية __vpiArrayWord*کلمه = array_var_کلمه_from_handle(معلومات->obj)) {
	    پلار = static_cast<__vpiArray*>(کلمه->get_پلار());
	    بینښه addr = کلمه->get_شاخص();
	    cbh = جديد array_کلمه_ارزښت_callback(معلومات, addr);

      } وإلا إذا (بنية __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(معلومات->obj)) {
	    پلار = tword->array;
	    cbh = جديد array_کلمه_ارزښت_callback(معلومات, tword->پته);

      } وإلا إذا (بنية __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(معلومات->obj)) {
	    پلار = apvword->array;
	    cbh = جديد array_کلمه_part_callback(معلومات, apvword->کلمه_sel);
      }

      أكد(cbh);
      أكد(پلار);
      cbh->بل_2 = پلار->vpi_callbacks;
      پلار->vpi_callbacks = cbh;

      أرجع cbh;
}

ارزښت_callback* vpip_array_change(p_cb_معلومات معلومات)
{
      array_کلمه_ارزښت_callback*cbh = جديد array_کلمه_ارزښت_callback(معلومات, -1);
      أكد(معلومات->obj);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(معلومات->obj);
      cbh->بل_2 = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      أرجع cbh;
}

فراغ compile_array_پورټ(حرف*label, حرف*array, حرف*addr)
{
      array_پورټ_resolv_list_t*resolv_mem
	    = جديد array_پورټ_resolv_list_t(array, خطأ, 0);

      define_functor_نښه(label, resolv_mem->ptr);
      حرر(label);
	// Connect the port-0 input as the address.
      ننوت_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

فراغ compile_array_پورټ(حرف*label, حرف*array, اوږد addr)
{
      array_پورټ_resolv_list_t*resolv_mem
	    = جديد array_پورټ_resolv_list_t(array, ریښتیا, addr);

      define_functor_نښه(label, resolv_mem->ptr);
      حرر(label);

      resolv_submit(resolv_mem);
}

فراغ compile_array_alias(حرف*label, حرف*name, حرف*src)
{
      vvp_array_t mem = array_پیدا_کړه(src);
      أكد(mem);

      بنية __vpiArray*obj = جديد __vpiArray;

      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_شمېر = mem->array_شمېر;
      obj->signed_flag = mem->signed_flag;

	// Need to set an accurate range of addresses.
      obj->لومړی_addr = mem->لومړی_addr;
      obj->وروستی_addr  = mem->وروستی_addr;
      obj->بدلون_addr = mem->بدلون_addr;

      obj->msb = mem->msb;
      obj->lsb = mem->lsb;

	// Share the words with the source array.
      obj->nets = mem->nets;
      obj->vals4 = mem->vals4;
      obj->vals  = mem->vals;
      obj->vals_پلنوالی = mem->vals_پلنوالی;
      obj->vals_ټکي = mem->vals_ټکي;

      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

      أكد(array_جدول);
      أكد(!array_پیدا_کړه(label));
      array_جدول->sym_سېټ_ارزښت(label, obj);

      compile_vpi_نښه(label, obj);
      vpip_attach_منزل_current_scope(obj);

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
vpiHandle vpip_make_vthr_A(حرف*label, بینښه addr)
{
      بنية __vpiArrayVthrA*obj = جديد __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = جديد array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->پته_handle = 0;
      obj->پته = addr;

      أرجع obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(حرف*label, حرف*نښه)
{
      بنية __vpiArrayVthrA*obj = جديد __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = جديد array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->پته_handle = 0;
      compile_vpi_lookup(&obj->پته_handle, نښه);
      obj->پته = 0;

      أرجع obj;
}

vpiHandle vpip_make_vthr_A(حرف*label, vpiHandle handle)
{
      بنية __vpiArrayVthrA*obj = جديد __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = جديد array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->پته_handle = handle;
      obj->پته = 0;

      أرجع obj;
}

vpiHandle vpip_make_vthr_APV(حرف*label, بینښه شاخص, بینښه بټ, بینښه wid)
{
      بنية __vpiArrayVthrAPV*obj = جديد __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = جديد array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->کلمه_sel = شاخص;
      obj->part_بټ = بټ;
      obj->part_wid = wid;

      أرجع obj;
}

فراغ compile_array_cleanup(فراغ)
{
      ړنګول array_جدول;
      array_جدول = 0;
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ حافظه_delete(vpiHandle توکی)
{
      بنية __vpiArray*arr = static_cast<بنية __vpiArray*>(توکی);
      إذا (arr->vals_ټکي) ړنګول [] (arr->vals_ټکي-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      ړنګول arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      ړنګول arr->vals;

      إذا (arr->nets) {
	    لكل (بینښه idx = 0; idx < arr->get_اندازه(); idx += 1) {
		  إذا (بنية __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.شاخص);
		    /* These should only be the real words. */
		  } وإلا {
			أكد(arr->nets[idx]->get_type_کوډ() ==
			       vpiRealVar);
			بنية __vpiRealVar *sigr = static_cast<بنية __vpiRealVar *>
			                            (arr->nets[idx]);
			constant_delete(sigr->id.شاخص);
// Why are only the real words still here?
			ړنګول arr->nets[idx];
		  }
	    }
	    حرر(arr->nets);
      }

      طالما (arr->vpi_callbacks) {
	    بنية __vpiCallback*tmp = arr->vpi_callbacks->بل_2;
	    ړنګول arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      ړنګول arr;
}

فراغ A_delete(vpiHandle توکی)
{
      بنية __vpiArrayVthrA*obj = static_cast<بنية __vpiArrayVthrA*>(توکی);
      إذا (obj->پته_handle) {
	    بدل (obj->پته_handle->get_type_کوډ()) {
		حالة vpi_حافظه_کلمه:
		  إذا (vpi_get(_vpiFromThr, obj->پته_handle) == _vpi_at_A) {
			A_delete(obj->پته_handle);
		  }
		  اكسر;
		حالة vpiPartSelect:
		  أكد(vpi_get(_vpiFromThr, obj->پته_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->پته_handle);
		  اكسر;
	    }
      }

      ړنګول obj;
}

فراغ APV_delete(vpiHandle توکی)
{
      بنية __vpiArrayVthrAPV*obj = static_cast<بنية __vpiArrayVthrAPV*>(توکی);
      ړنګول obj;
}
#د_که_پای
