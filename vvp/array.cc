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
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
#ضمن  "vvp_cleanup.h"
#جے_انت
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <iostream>
# include  "compile.h"
# include  <cassert>
# include  "ivl_alloc.h"

ورتوں نطاق std;

بنانشانی لما گنتی_net_arrays = 0;
بنانشانی لما گنتی_net_array_الفاظ = 0;
بنانشانی لما گنتی_var_arrays = 0;
بنانشانی لما گنتی_var_array_الفاظ = 0;
بنانشانی لما گنتی_real_arrays = 0;
بنانشانی لما گنتی_real_array_الفاظ = 0;

ساكن علامت_map_s<بنية __vpiArray>* array_جدول =0;

صنف vvp_fun_arrayport;
ساكن فراغ array_attach_پورٹ(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_لبھو(ثابت حرف*label)
{
      إذا (array_جدول == 0)
	    أرجع 0;

      vvp_array_t v = array_جدول->sym_get_قدر(label);
      أرجع v;
}

بنية __vpiArrayVthrA : عام __vpiHandle {
      صحيح get_type_کوڈ(فراغ) ثابت override { أرجع vpi_حافظہ_لفظ; }
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح code) override;

      بنية __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle پتہ_handle;
	// If wid==0, then address is the address into the array.
      بنانشانی پتہ;

      بنانشانی get_پتہ() ثابت
      {
	    إذا (پتہ_handle) {
		  s_vpi_قدر vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpi_سمتیہ_Val;
		  پتہ_handle->vpi_get_قدر(&vp);
		  صحيح الفاظ = (پتہ_handle->vpi_get(vpi_ناپ)-1)/32 + 1;
		  لكل(صحيح idx = 0; idx < الفاظ; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			إذا (vp.value.vector[idx].bval != 0) أرجع UINT_MAX;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  پتہ_handle->vpi_get_قدر(&vp);
		  أرجع vp.value.integer;
	    }

	    أرجع پتہ;
      }
};


بنية __vpiArrayVthrAPV : عام __vpiHandle {
      صحيح get_type_کوڈ(فراغ) ثابت override { أرجع vpiPartSelect; }
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_handle(صحيح code) override;

      بنية __vpiArray*array;
      بنانشانی لفظ_sel;
      بنانشانی part_ثنائی_ہندسہ;
      بنانشانی part_wid;
};

منطقي is_net_array(vpiHandle obj)
{
      بنية __vpiArray*rfp = dynamic_cast<__vpiArray*> (obj);
      أكد(rfp);

      إذا (rfp->nets != 0) أرجع سچ;
      أرجع خطأ;
}

// This function return true if the underlying array words are real.
ساكن منطقي vpi_array_is_real(ثابت vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      إذا (arr->vals4 != 0)  // A bit based variable/register array.
	    أرجع خطأ;

      إذا (dynamic_cast<vvp_darray_real*> (arr->vals))
	    أرجع سچ;

      إذا (arr->vals != 0)
	    أرجع خطأ;

	// This must be a net array so look at element 0 to find the type.
      أكد(arr->nets != 0);
      أكد(arr->get_ناپ() > 0);
      ثابت بنية __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      إذا (rsig) {
	    أرجع سچ;
      }

      أرجع خطأ;
}

ساكن منطقي vpi_array_is_string(ثابت vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      إذا (arr->vals4 != 0)  // A bit based variable/register array.
	    أرجع خطأ;

      إذا (dynamic_cast<vvp_darray_string*> (arr->vals))
	    أرجع سچ;

      أرجع خطأ;
}

صحيح __vpiArray::get_لفظ_ناپ() ثابت
{
      بنانشانی width;

      أكد(get_ناپ() > 0);
	/* For a net array we need to get the width from the first element. */
      إذا (nets) {
	    أكد(vals4 == 0 && vals == 0);
	    ثابت بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    أكد(vsig);
	    width = vpip_ناپ(vsig);
	/* For a variable array we can get the width from vals_width. */
      } وإلا {
	    أكد(vals4 || vals);
	    width = vals_چوڑائی;
      }

      أرجع width;
}

حرف*__vpiArray::get_لفظ_str(بنية __vpiArrayWord*لفظ, صحيح code)
{
      بنانشانی اشاریہ = لفظ->get_اشاریہ();

      إذا (code == vpiFile) {  // Not implemented for now!
	    أرجع simple_سیٹ_rbuf_str(file_ناں[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%d", (صحيح)اشاریہ + پہلا_addr.get_قدر());
      أرجع generic_get_str(code, get_scope(), name, sidx);
}

فراغ __vpiArray::get_لفظ_قدر(بنية __vpiArrayWord*لفظ, p_vpi_قدر vp)
{
      بنانشانی اشاریہ = لفظ->get_اشاریہ();

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
          vpip_vec4_get_قدر(vals4->get_لفظ(اشاریہ),
                              vals4->width(), signed_flag, vp);
      } وإلا إذا(vals) {
          بدل(vp->format) {
            حالة vpiBinStrVal:
            حالة vpiOctStrVal:
            حالة vpiDecStrVal:
            حالة vpiHexStrVal:
            حالة vpiScalarVal:
            حالة vpiIntVal:
            حالة vpi_سمتیہ_Val:
            {
                vvp_vector4_t v;
                vals->get_لفظ(اشاریہ, v);
                vpip_vec4_get_قدر(v, vals_چوڑائی, signed_flag, vp);
            }
            اكسر;

            حالة vpiRealVal:
            {
                مزدوج d;
                vals->get_لفظ(اشاریہ, d);
                vpip_real_get_قدر(d, vp);
            }
            اكسر;

            حالة vpiStringVal:
            {
                string s;
                vals->get_لفظ(اشاریہ, s);
                vpip_string_get_قدر(s, vp);
            }
            اكسر;

            افتراضي:
                fprintf(stderr, "vpi sorry: format is not implemented\n");
                أكد(خطأ);
          }
      }
}

فراغ __vpiArray::put_لفظ_قدر(بنية __vpiArrayWord*لفظ, p_vpi_قدر vp, صحيح)
{
      بنانشانی اشاریہ = لفظ->get_اشاریہ();
      vvp_vector4_t val = vec4_from_vpi_قدر(vp, vals_چوڑائی);
      سیٹ_لفظ(اشاریہ, 0, val);
}

vpiHandle __vpiArray::get_iter_اشاریہ(بنية __vpiArrayIterator*, صحيح idx)
{
      إذا (nets) أرجع nets[idx];

      أكد(vals4 || vals);

      إذا (vals_الفاظ == 0) make_vals_الفاظ();

      أرجع &(vals_الفاظ[idx].as_لفظ);
}

صحيح __vpiArray::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_سطر_No:
	    أرجع 0; // Not implemented for now!

	  حالة vpi_ناپ:
	    أرجع get_ناپ();

	  حالة vpiAutomatic:
	    أرجع scope->is_automatic()? 1 : 0;

	  افتراضي:
	    أرجع 0;
      }
}

حرف* __vpiArray::vpi_get_str(صحيح code)
{
      إذا (code == vpiFile) {  // Not implemented for now!
            أرجع simple_سیٹ_rbuf_str(file_ناں[0]);
      }

      أرجع generic_get_str(code, scope, name, عدم);
}

vpiHandle __vpiArray::vpi_handle(صحيح code)
{
      بدل (code) {
          حالة vpi_بایاں_Range:
            إذا (وٹاندرا_addr) أرجع &آخری_addr;
            وإلا أرجع &پہلا_addr;

          حالة vpi_دایاں_Range:
            إذا (وٹاندرا_addr) أرجع &پہلا_addr;
            وإلا أرجع &آخری_addr;

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
vpiHandle __vpiArray::vpi_اشاریہ(صحيح اشاریہ)
{
      اشاریہ -= پہلا_addr.get_قدر();
      إذا (اشاریہ >= (لما) get_ناپ())
	    أرجع 0;
      إذا (اشاریہ < 0)
	    أرجع 0;

      إذا (nets != 0) {
	    أرجع nets[اشاریہ];
      }

      إذا (vals_الفاظ == 0)
	    make_vals_الفاظ();

      أرجع &(vals_الفاظ[اشاریہ].as_لفظ);
}

صحيح __vpiArrayWord::as_لفظ_t::vpi_get(صحيح code)
{
      بنية __vpiArrayWord*obj = array_var_لفظ_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_ماپا = obj->get_ماپا();
      t_vpi_قدر val;

      بدل (code) {
	  حالة vpi_سطر_No:
	    أرجع 0; // Not implemented for now!

	  حالة vpi_ناپ:
	    أرجع my_ماپا->get_لفظ_ناپ();

	  حالة vpi_بایاں_Range:
            val.format = vpiIntVal;
	    my_ماپا->get_بایاں_range()->vpi_get_قدر(&val);
            أكد(val.format == vpiIntVal);
            أرجع val.value.integer;

	  حالة vpi_دایاں_Range:
            val.format = vpiIntVal;
	    my_ماپا->get_دایاں_range()->vpi_get_قدر(&val);
            أكد(val.format == vpiIntVal);
            أرجع val.value.integer;

	  حالة vpi_اشاریہ_2:
	    {
		  صحيح ادھار_وتھ = 0;
		  بنية __vpiArray*ادھار = dynamic_cast<__vpiArray*> (my_ماپا);
		  إذا (ادھار) {
			val.format = vpiIntVal;
			ادھار->پہلا_addr.vpi_get_قدر(&val);
			ادھار_وتھ += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_اشاریہ.vpi_get_قدر(&val);
		  أكد(val.format == vpiIntVal);
		  أرجع val.value.integer + ادھار_وتھ;
	    }

	  حالة vpiAutomatic:
	    أرجع my_ماپا->get_scope()->is_automatic()? 1 : 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    أرجع _vpiNoThr;
#جے_انت

	  افتراضي:
	    أرجع 0;
      }
}


صحيح __vpiArrayVthrA::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_سطر_No:
	    أرجع 0; // Not implemented for now!

	  حالة vpi_ناپ:
	    أرجع array->get_لفظ_ناپ();

	  حالة vpi_بایاں_Range:
	    أرجع array->msb.get_قدر();

	  حالة vpi_دایاں_Range:
	    أرجع array->lsb.get_قدر();

	  حالة vpi_اشاریہ_2:
	    أرجع (صحيح)get_پتہ() + array->پہلا_addr.get_قدر();

	  حالة vpiAutomatic:
	    أرجع array->get_scope()->is_automatic() ? 1 : 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    أرجع _vpi_at_A;
#جے_انت

	  // If address_handle is not zero we definitely have a
	  // variable.
	  حالة vpiConstantSelect:
	    أرجع پتہ_handle == 0;

	  افتراضي:
	    أرجع 0;
      }
}

حرف* __vpiArrayVthrA::vpi_get_str(صحيح code)
{
      إذا (code == vpiFile) {  // Not implemented for now!
            أرجع simple_سیٹ_rbuf_str(file_ناں[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%d", (صحيح)get_پتہ() + array->پہلا_addr.get_قدر());
      أرجع generic_get_str(code, array->get_scope(), array->name, sidx);
}

فراغ __vpiArrayVthrA::vpi_get_قدر(p_vpi_قدر vp)
{
      أكد(array);

      بنانشانی اشاریہ = get_پتہ();
      إذا (vpi_array_is_real(array)) {
	    مزدوج tmp = array->get_لفظ_r(اشاریہ);
	    vpip_real_get_قدر(tmp, vp);
      } وإلا إذا (vpi_array_is_string(array)) {
	    string tmp = array->get_لفظ_str(اشاریہ);
	    vpip_string_get_قدر(tmp, vp);
      } وإلا {
	    vvp_vector4_t tmp = array->get_لفظ(اشاریہ);
	    بنانشانی width = array->get_لفظ_ناپ();
	    vpip_vec4_get_قدر(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_قدر(p_vpi_قدر vp, صحيح)
{
      بنانشانی اشاریہ = get_پتہ();

      أكد(array);
      أكد(اشاریہ < array->get_ناپ());

      إذا (vpi_array_is_real(array)) {
	    مزدوج val = real_from_vpi_قدر(vp);
	    array->سیٹ_لفظ(اشاریہ, val);
      } وإلا {
	    بنانشانی width = array->get_لفظ_ناپ();
	    vvp_vector4_t val = vec4_from_vpi_قدر(vp, width);
	    array->سیٹ_لفظ(اشاریہ, 0, val);
      }

      أرجع هذا;
}

vpiHandle __vpiArrayVthrA::vpi_handle(صحيح code)
{
      بدل (code) {
	  حالة vpi_اشاریہ_2:
	    اكسر;  // Not implemented!

	  حالة vpi_بایاں_Range:
	    أرجع &array->msb;

	  حالة vpi_دایاں_Range:
	    أرجع &array->lsb;

	  حالة vpi_ماپا:
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

	  حالة vpi_ناپ:
	    أرجع part_wid;

	  حالة vpi_بایاں_Range:
	    أرجع part_ثنائی_ہندسہ + part_wid - 1;

	  حالة vpi_دایاں_Range:
	    أرجع part_ثنائی_ہندسہ;

	  حالة vpiAutomatic:
	    أرجع array->get_scope()->is_automatic() ? 1 : 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    أرجع _vpi_at_APV;
#جے_انت

	  حالة vpiConstantSelect:
	    أرجع 1;

	  افتراضي:
	    أرجع 0;
      }
}

حرف* __vpiArrayVthrAPV::vpi_get_str(صحيح code)
{
      إذا (code == vpiFile) {  // Not implemented for now!
            أرجع simple_سیٹ_rbuf_str(file_ناں[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%u", لفظ_sel + array->پہلا_addr.get_قدر());
      أرجع generic_get_str(code, array->get_scope(), array->name, sidx);
}

فراغ __vpiArrayVthrAPV::vpi_get_قدر(p_vpi_قدر vp)
{
      أكد(array);

      بنانشانی اشاریہ = لفظ_sel;
      إذا (vpi_array_is_real(array)) {
	    مزدوج tmp = array->get_لفظ_r(اشاریہ);
	    vpip_real_get_قدر(tmp, vp);
      } وإلا {
	    vvp_vector4_t tmp = array->get_لفظ(اشاریہ);
	    tmp = tmp.subvalue(part_ثنائی_ہندسہ, part_wid);
	    vpip_vec4_get_قدر(tmp, part_wid, خطأ, vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(صحيح code)
{
      بدل (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  حالة vpi_ماپا:
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

فراغ __vpiArray::سیٹ_لفظ(بنانشانی پتہ, بنانشانی part_off, ثابت vvp_vector4_t&val)
{
      إذا (پتہ >= get_ناپ())
	    أرجع;

      إذا (vals4) {
	    أكد(nets == 0);
	    إذا (part_off != 0 || val.size() != vals_چوڑائی) {
		  vvp_vector4_t tmp = vals4->get_لفظ(پتہ);
		  إذا ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_چوڑائی << سطر_انت;
			أكد(0);
		  }
		  tmp.سیٹ_vec(part_off, val);
		  vals4->سیٹ_لفظ(پتہ, tmp);
	    } وإلا {
		  vals4->سیٹ_لفظ(پتہ, val);
	    }
	    لفظ_change(پتہ);
	    أرجع;
      }

      إذا (vals) {
	    أكد(nets == 0);
	    إذا (part_off != 0 || val.size() != vals_چوڑائی) {
		  vvp_vector4_t tmp;
		  vals->get_لفظ(پتہ, tmp);
		  إذا ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_چوڑائی << سطر_انت;
			أكد(0);
		  }
		  tmp.سیٹ_vec(part_off, val);
		  vals->سیٹ_لفظ(پتہ, tmp);
	    } وإلا {
		  vals->سیٹ_لفظ(پتہ, val);
	    }
	    لفظ_change(پتہ);
	    أرجع;
      }

      أكد(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle لفظ = nets[پتہ];
      ثابت بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(لفظ);
      أكد(vsig);

      vsig->گرہ->send_vec4_pv(val, part_off, vpip_ناپ(vsig), 0);
      لفظ_change(پتہ);
}

فراغ __vpiArray::سیٹ_لفظ(بنانشانی پتہ, مزدوج val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      إذا (پتہ >= vals->get_ناپ())
	    أرجع;

      vals->سیٹ_لفظ(پتہ, val);
      لفظ_change(پتہ);
}

فراغ __vpiArray::سیٹ_لفظ(بنانشانی پتہ, ثابت string&val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      إذا (پتہ >= vals->get_ناپ())
	    أرجع;

      vals->سیٹ_لفظ(پتہ, val);
      لفظ_change(پتہ);
}

فراغ __vpiArray::سیٹ_لفظ(بنانشانی پتہ, ثابت vvp_object_t&val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      إذا (پتہ >= vals->get_ناپ())
	    أرجع;

      vals->سیٹ_لفظ(پتہ, val);
      لفظ_change(پتہ);
}

vvp_vector4_t __vpiArray::get_لفظ(بنانشانی پتہ)
{
      إذا (vals4) {
	    أكد(nets == 0);
	    أكد(vals == 0);
	    أرجع vals4->get_لفظ(پتہ);
      }

      إذا (vals) {
	    أكد(nets == 0);
	    أكد(vals4== 0);
	    إذا (پتہ >= vals->get_ناپ())
		  أرجع vvp_vector4_t(vals_چوڑائی, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_لفظ(پتہ, val);
	    أرجع val;
      }

      أكد(vals4 == 0);
      أكد(vals == 0);
      أكد(nets != 0);

      إذا (پتہ >= get_ناپ()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    أكد(get_ناپ() > 0);
	    vpiHandle لفظ = nets[0];
	    أكد(لفظ);
	    بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(لفظ);
	    أكد(vsig);
	    vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (vsig->گرہ->fil);
	    أكد(sig);
	    أرجع vvp_vector4_t(sig->قدر_ناپ(), BIT4_X);
      }

      vpiHandle لفظ = nets[پتہ];
      بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(لفظ);
      أكد(vsig);
      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (vsig->گرہ->fil);
      أكد(sig);

      vvp_vector4_t val;
      sig->vec4_قدر(val);
      أرجع val;
}

مزدوج __vpiArray::get_لفظ_r(بنانشانی پتہ)
{
      إذا (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    إذا (پتہ >= vals->get_ناپ())
		  أرجع 0.0;

	    مزدوج val;
	    vals->get_لفظ(پتہ, val);
	    أرجع val;
      }

      إذا (پتہ >= get_ناپ())
	    أرجع 0.0;

      أكد(nets);
      vpiHandle لفظ = nets[پتہ];
      بنية __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(لفظ);
      أكد(vsig);
      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (vsig->net->fil);
      أكد(sig);

      مزدوج val = sig->real_قدر();
      أرجع val;
}

فراغ __vpiArray::get_لفظ_obj(بنانشانی پتہ, vvp_object_t&val)
{
      إذا (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    إذا (پتہ >= vals->get_ناپ()) {
		  val = vvp_object_t();
		  أرجع;
	    }

	    vals->get_لفظ(پتہ, val);
	    أرجع;
      }

      أكد(nets);
	// Arrays of string nets not implemented!
      أكد(0);
      أرجع;
}

string __vpiArray::get_لفظ_str(بنانشانی پتہ)
{
      إذا (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    إذا (پتہ >= vals->get_ناپ())
		  أرجع "";

	    string val;
	    vals->get_لفظ(پتہ, val);
	    أرجع val;
      }

      أكد(nets);
	// Arrays of string nets not implemented!
      أكد(0);
      أرجع "";
}

vpiHandle vpip_make_array(ثابت حرف*label, ثابت حرف*name,
                          صحيح پہلا_addr, صحيح آخری_addr,
                          منطقي signed_flag)
{
      بنية __vpiArray*obj = جديد __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      إذا (آخری_addr >= پہلا_addr) {
	    obj->وٹاندرا_addr = خطأ;
      } وإلا {
	    obj->وٹاندرا_addr = سچ;
	    صحيح tmp = آخری_addr;
	    آخری_addr = پہلا_addr;
	    پہلا_addr = tmp;
      }
      أكد(آخری_addr >= پہلا_addr);
      بنانشانی array_گنتی = آخری_addr+1-پہلا_addr;

	// For now, treat all arrays as memories. This is not quite
	// correct, as arrays are arrays with memories a special case.
      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_گنتی = array_گنتی;

      obj->پہلا_addr.سیٹ_قدر(پہلا_addr);
      obj->آخری_addr .سیٹ_قدر(آخری_addr);

	// Start off now knowing if we are nets or variables.
      obj->nets = 0;
      obj->vals4 = 0;
      obj->vals  = 0;
      obj->vals_چوڑائی = 0;
      obj->vals_الفاظ = 0;

	// Initialize (clear) the read-ports list.
      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

	/* Add this symbol to the array_symbols table for later lookup. */
      إذا (!array_جدول)
	    array_جدول = جديد علامت_map_s<بنية __vpiArray>;

      أكد(!array_لبھو(label));
      array_جدول->sym_سیٹ_قدر(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_علامت(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_منزل_current_scope(obj);

      أرجع obj;
}

فراغ __vpiArray::alias_لفظ(بنانشانی لما addr, vpiHandle لفظ, صحيح msb_, صحيح lsb_)
{
      أكد(msb.get_قدر() == msb_);
      أكد(lsb.get_قدر() == lsb_);
      أكد(addr < get_ناپ());
      أكد(nets);
      nets[addr] = لفظ;
}

فراغ __vpiArray::attach_لفظ(بنانشانی addr, vpiHandle لفظ)
{
      أكد(addr < get_ناپ());
      أكد(nets);
      nets[addr] = لفظ;

      إذا (بنية __vpiSignal*sig = dynamic_cast<__vpiSignal*>(لفظ)) {
	    vvp_net_t*net = sig->گرہ;
	    أكد(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    أكد(fun);
	    fun->attach_as_لفظ(هذا, addr);
	    sig->is_netarray = 1;
	    sig->within.ماپا = هذا;
	    sig->id.اشاریہ = جديد __vpiDecConst(addr + پہلا_addr.get_قدر());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    أرجع;
      }

      إذا (بنية __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(لفظ)) {
	    vvp_net_t*net = sig->net;
	    أكد(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    أكد(fun);
	    fun->attach_as_لفظ(هذا, addr);
	    sig->is_netarray = 1;
	    sig->within.ماپا = هذا;
	    sig->id.اشاریہ = جديد __vpiDecConst(addr + پہلا_addr.get_قدر());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = سچ;
	    أرجع;
      }
}

فراغ compile_var_array(حرف*label, حرف*name, صحيح آخری, صحيح پہلا,
		   صحيح msb, صحيح lsb, حرف signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, پہلا, آخری,
                                      signed_flag != 0);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_چوڑائی = labs(msb-lsb) + 1;
      إذا (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = جديد vvp_vector4array_aa(arr->vals_چوڑائی,
						 arr->get_ناپ());
      } وإلا {
            arr->vals4 = جديد vvp_vector4array_sa(arr->vals_چوڑائی,
						 arr->get_ناپ());
      }
      arr->msb.سیٹ_قدر(msb);
      arr->lsb.سیٹ_قدر(lsb);

      گنتی_var_arrays += 1;
      گنتی_var_array_الفاظ += arr->get_ناپ();

      حرر(label);
      مٹاؤ[] name;
}

فراغ compile_var2_array(حرف*label, حرف*name, صحيح آخری, صحيح پہلا,
		   صحيح msb, صحيح lsb, منطقي signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, پہلا, آخری, signed_flag);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.سیٹ_قدر(msb);
      arr->lsb.سیٹ_قدر(lsb);
      arr->vals_چوڑائی = labs(msb-lsb) + 1;

      أكد(! arr->nets);
      إذا (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = جديد vvp_darray_atom<int8_t>(arr->get_ناپ());
      } وإلا إذا (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = جديد vvp_darray_atom<uint8_t>(arr->get_ناپ());
      } وإلا إذا (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = جديد vvp_darray_atom<int16_t>(arr->get_ناپ());
      } وإلا إذا (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = جديد vvp_darray_atom<uint16_t>(arr->get_ناپ());
      } وإلا إذا (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = جديد vvp_darray_atom<int32_t>(arr->get_ناپ());
      } وإلا إذا (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = جديد vvp_darray_atom<uint32_t>(arr->get_ناپ());
      } وإلا إذا (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = جديد vvp_darray_atom<int64_t>(arr->get_ناپ());
      } وإلا إذا (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = جديد vvp_darray_atom<uint64_t>(arr->get_ناپ());
      } وإلا {
	    arr->vals = جديد vvp_darray_vec2(arr->get_ناپ(), arr->vals_چوڑائی);
      }
      گنتی_var_arrays += 1;
      گنتی_var_array_الفاظ += arr->get_ناپ();

      حرر(label);
      مٹاؤ[] name;
}

فراغ compile_real_array(حرف*label, حرف*name, صحيح آخری, صحيح پہلا)
{
      vpiHandle obj = vpip_make_array(label, name, پہلا, آخری, سچ);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = جديد vvp_darray_real(arr->get_ناپ());
      arr->vals_چوڑائی = 1;

      گنتی_real_arrays += 1;
      گنتی_real_array_الفاظ += arr->get_ناپ();

      حرر(label);
      مٹاؤ[] name;
}

فراغ compile_string_array(حرف*label, حرف*name, صحيح آخری, صحيح پہلا)
{
      vpiHandle obj = vpip_make_array(label, name, پہلا, آخری, سچ);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = جديد vvp_darray_string(arr->get_ناپ());
      arr->vals_چوڑائی = 1;

      گنتی_real_arrays += 1;
      گنتی_real_array_الفاظ += arr->get_ناپ();

      حرر(label);
      مٹاؤ[] name;
}

فراغ compile_object_array(حرف*label, حرف*name, صحيح آخری, صحيح پہلا)
{
      vpiHandle obj = vpip_make_array(label, name, پہلا, آخری, سچ);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = جديد vvp_darray_object(arr->get_ناپ());
      arr->vals_چوڑائی = 1;

      گنتی_real_arrays += 1;
      گنتی_real_array_الفاظ += arr->get_ناپ();

      حرر(label);
      مٹاؤ[] name;
}

فراغ compile_net_array(حرف*label, حرف*name, صحيح آخری, صحيح پہلا)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, پہلا, آخری, خطأ);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(صفر_حافظہ_دیو(arr->get_ناپ(), حجم(vpiHandle)));

      گنتی_net_arrays += 1;
      گنتی_net_array_الفاظ += arr->get_ناپ();

      حرر(label);
      مٹاؤ[] name;
}

صنف vvp_fun_arrayport  : عام vvp_net_fun_t {

    عام:
      واضح vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      واضح vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, لما addr);
      ~vvp_fun_arrayport() override;

      مجازی فراغ check_لفظ_change(بنانشانی لما addr) = 0;

    محمي:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      بنانشانی لما addr_;

      یار فراغ array_attach_پورٹ(vvp_array_t, vvp_fun_arrayport*);
      یار فراغ __vpiArray::لفظ_change(بنانشانی لما);
      vvp_fun_arrayport*اگلا;
};

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net)
: arr_(mem), net_(net), addr_(0)
{
      اگلا = 0;
}

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, لما addr)
: arr_(mem), net_(net), addr_(addr)
{
      اگلا = 0;
}

vvp_fun_arrayport::~vvp_fun_arrayport()
{
}

صنف vvp_fun_arrayport_sa  : عام vvp_fun_arrayport {

    عام:
      واضح vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      واضح vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, لما addr);
      ~vvp_fun_arrayport_sa() override;

      فراغ check_لفظ_change(بنانشانی لما addr) override;

      فراغ recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;

    خاص:
};

vvp_fun_arrayport_sa::vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net)
{
}

vvp_fun_arrayport_sa::vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, لما addr)
: vvp_fun_arrayport(mem, net, addr)
{
}

vvp_fun_arrayport_sa::~vvp_fun_arrayport_sa()
{
}

فراغ vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                     vvp_context_t)
{
      منطقي addr_valid_flag;

      بدل (پورٹ.پورٹ()) {

	  حالة 0: // Address input
	    addr_valid_flag = vector4_منزل_قدر(ثنائی_ہندسہ, addr_);
	    إذا (! addr_valid_flag)
		  addr_ = arr_->get_ناپ();
	    إذا (vpi_array_is_real(arr_))
		  پورٹ.ptr()->send_real(arr_->get_لفظ_r(addr_), 0);
	    وإلا
		  پورٹ.ptr()->send_vec4(arr_->get_لفظ(addr_), 0);

	    اكسر;

	  افتراضي:
	    fprintf(stdout, "XXXX write ports not implemented.\n");
	    أكد(0);
      }
}

فراغ vvp_fun_arrayport_sa::check_لفظ_change(بنانشانی لما addr)
{
      إذا (addr != addr_) أرجع;

      إذا (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_لفظ_r(addr_), 0);
      } وإلا {
	    net_->send_vec4(arr_->get_لفظ(addr_), 0);
      }
}

صنف vvp_fun_arrayport_aa  : عام vvp_fun_arrayport, عام automatic_hooks_s {

    عام:
      واضح vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net);
      واضح vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, لما addr);
      ~vvp_fun_arrayport_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#جے_انت

      فراغ check_لفظ_change(بنانشانی لما addr) override;

      فراغ recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;

    خاص:
      فراغ check_لفظ_change_2(بنانشانی لما addr, vvp_context_t context);

      __vpiScope*context_scope_;
      بنانشانی context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_جوڑو_شے_منزل_context(هذا, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, لما addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_جوڑو_شے_منزل_context(هذا, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

فراغ vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      بنانشانی لما*addr = جديد بنانشانی لما;
      vvp_سیٹ_context_شے(context, context_idx_, addr);

      *addr = addr_;
}

فراغ vvp_fun_arrayport_aa::reset_instance(vvp_context_t context)
{
      بنانشانی لما*addr = static_cast<بنانشانی لما*>
            (vvp_get_context_شے(context, context_idx_));

      *addr = addr_;
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ vvp_fun_arrayport_aa::free_instance(vvp_context_t context)
{
      بنانشانی لما*addr = static_cast<بنانشانی لما*>
            (vvp_get_context_شے(context, context_idx_));
      مٹاؤ addr;
}
#جے_انت

فراغ vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                     vvp_context_t context)
{
      إذا (context) {
            بنانشانی لما*addr = static_cast<بنانشانی لما*>
                  (vvp_get_context_شے(context, context_idx_));

            منطقي addr_valid_flag;

            بدل (پورٹ.پورٹ()) {

                حالة 0: // Address input
                  addr_valid_flag = vector4_منزل_قدر(ثنائی_ہندسہ, *addr);
                  إذا (! addr_valid_flag) *addr = arr_->get_ناپ();
                  إذا (vpi_array_is_real(arr_)) {
			پورٹ.ptr()->send_real(arr_->get_لفظ_r(*addr),
					      context);
                  } وإلا {
			پورٹ.ptr()->send_vec4(arr_->get_لفظ(*addr),
					      context);
                  }
                  اكسر;

                افتراضي:
                  fprintf(stdout, "XXXX write ports not implemented.\n");
                  أكد(0);
            }
      } وإلا {
            context = context_scope_->جیوندا_contexts;
            طالما (context) {
                  recv_vec4(پورٹ, ثنائی_ہندسہ, context);
                  context = vvp_get_اگلا_context(context);
            }
      }
}

فراغ vvp_fun_arrayport_aa::check_لفظ_change_2(بنانشانی لما addr,
                                              vvp_context_t context)
{
      ثابت بنانشانی لما*پورٹ_addr = static_cast<بنانشانی لما*>
            (vvp_get_context_شے(context, context_idx_));

      إذا (addr != *پورٹ_addr)
	    أرجع;

      إذا (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_لفظ_r(addr), context);
      } وإلا {
	    net_->send_vec4(arr_->get_لفظ(addr), context);
      }
}

فراغ vvp_fun_arrayport_aa::check_لفظ_change(بنانشانی لما addr)
{
      إذا (arr_->get_scope()->is_automatic()) {
            أكد(vthread_get_wt_context());
            check_لفظ_change_2(addr, vthread_get_wt_context());
      } وإلا {
            vvp_context_t context = context_scope_->جیوندا_contexts;
            طالما (context) {
                  check_لفظ_change_2(addr, context);
                  context = vvp_get_اگلا_context(context);
            }
      }
}

ساكن فراغ array_attach_پورٹ(vvp_array_t array, vvp_fun_arrayport*fun)
{
      أكد(fun->اگلا == 0);
      fun->اگلا = array->ports_;
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
                  vvp_vector4_t tmp(array->vals_چوڑائی, init);
                  ویلا_سارنی_init_propagate(fun->net_, tmp);
            } وإلا {
                  ویلا_سارنی_init_propagate(fun->net_, 0.0);
            }
      }
}

صنف array_لفظ_قدر_callback : عام قدر_callback {
    عام:
      مضمن واضح array_لفظ_قدر_callback(p_cb_انکڑے انکڑے, لما addr)
      : قدر_callback(انکڑے), لفظ_addr(addr)
      { }

    عام:
      لما لفظ_addr;
};

فراغ __vpiArray::لفظ_change(بنانشانی لما addr)
{
      لكل (vvp_fun_arrayport*cur = ports_; cur; cur = cur->اگلا)
	    cur->check_لفظ_change(addr);

	// Run callbacks attached to the array itself.
      بنية __vpiCallback *اگلا_2 = vpi_callbacks;
      بنية __vpiCallback *prev = 0;

      طالما (اگلا_2) {
	    array_لفظ_قدر_callback*cur = dynamic_cast<array_لفظ_قدر_callback*>(اگلا_2);
	    اگلا_2 = cur->اگلا_2;

	      // Skip callbacks that are not for me. -1 is for every element.
	    إذا (cur->لفظ_addr != (لما)addr && cur->لفظ_addr != -1) {
		  prev = cur;
		  استمر;
	    }

	      // For whole array callbacks we need to set the index.
	    إذا (cur->لفظ_addr == -1) {
		  cur->cb_انکڑے.اشاریہ = (PLI_INT32) ((صحيح)addr +
						    پہلا_addr.get_قدر());
	    }

	    إذا (cur->cb_انکڑے.cb_rtn != 0) {
		  إذا (cur->test_قدر_callback_ready()) {
			إذا (cur->cb_انکڑے.value) {
			      إذا (vpi_array_is_real(هذا)) {
				    مزدوج val = 0.0;
				    إذا (addr < vals->get_ناپ())
					  vals->get_لفظ(addr, val);
				    vpip_real_get_قدر(val, cur->cb_انکڑے.value);
			      } وإلا إذا (vals4) {
				    vpip_vec4_get_قدر(vals4->get_لفظ(addr),
							vals_چوڑائی,
							signed_flag,
							cur->cb_انکڑے.value);
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
				    إذا (addr < vals->get_ناپ())
					  vals->get_لفظ(addr, val);
				    vpip_vec4_get_قدر(val,
							vals_چوڑائی,
							signed_flag,
							cur->cb_انکڑے.value);
			      } وإلا {
			            أكد(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } وإلا إذا (prev == 0) {

		  vpi_callbacks = اگلا_2;
		  cur->اگلا_2 = 0;
		  مٹاؤ cur;

	    } وإلا {
		  أكد(prev->اگلا_2 == cur);
		  prev->اگلا_2 = اگلا_2;
		  cur->اگلا_2 = 0;
		  مٹاؤ cur;
	    }
      }
}

صنف array_resolv_list_t : عام resolv_list_s {

    عام:
      واضح array_resolv_list_t(حرف*lab) : resolv_list_s(lab) {
	    array = 0;
      }

      vvp_array_t*array;
      منطقي resolve(منطقي mes) override;

    خاص:
};

منطقي array_resolv_list_t::resolve(منطقي mes)
{
      *array = array_لبھو(label());
      إذا (*array == 0) {
	    أكد(!mes);
	    أرجع خطأ;
      }
      أرجع سچ;
}

صنف array_پورٹ_resolv_list_t : عام resolv_list_s {

    عام:
      واضح array_پورٹ_resolv_list_t(حرف* lab, منطقي use_addr__,
                                        لما addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      منطقي use_addr;
      لما addr;
      منطقي resolve(منطقي mes) override;

    خاص:
};

array_پورٹ_resolv_list_t::array_پورٹ_resolv_list_t(حرف *lab, منطقي use_addr__,
                                                   لما addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      إذا (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      وإلا
            context_scope = 0;
      ptr = جديد vvp_net_t;
}

منطقي array_پورٹ_resolv_list_t::resolve(منطقي mes)
{
      vvp_array_t mem = array_لبھو(label());
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

      array_attach_پورٹ(mem, fun);

      أرجع سچ;
}

صنف array_لفظ_part_callback : عام array_لفظ_قدر_callback {
    عام:
      واضح array_لفظ_part_callback(p_cb_انکڑے انکڑے, لما addr);
      ~array_لفظ_part_callback() override;

      array_لفظ_part_callback(ثابت array_لفظ_part_callback&) = مٹاؤ;
      array_لفظ_part_callback& عامل=(ثابت array_لفظ_part_callback&) = مٹاؤ;

      منطقي test_قدر_callback_ready(فراغ) override;

    خاص:
      حرف*قدر_بٹ;
};

array_لفظ_part_callback::array_لفظ_part_callback(p_cb_انکڑے انکڑے, لما addr)
: array_لفظ_قدر_callback(انکڑے, addr)
{
	// Get the initial value of the part, to use as a reference.
      بنية __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(انکڑے->obj);
      s_vpi_قدر tmp_قدر;
      tmp_قدر.format = vpiBinStrVal;
      apvword->vpi_get_قدر(&tmp_قدر);

      قدر_بٹ = جديد حرف[apvword->part_wid+1];

      حافظہ_نقل(قدر_بٹ, tmp_قدر.value.str, apvword->part_wid);
      قدر_بٹ[apvword->part_wid] = 0;
}

array_لفظ_part_callback::~array_لفظ_part_callback()
{
      مٹاؤ[]قدر_بٹ;
}

منطقي array_لفظ_part_callback::test_قدر_callback_ready(فراغ)
{
      بنية __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_انکڑے.obj);
      أكد(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_قدر tmp_قدر;
      tmp_قدر.format = vpiBinStrVal;
      apvword->vpi_get_قدر(&tmp_قدر);

      إذا (memcmp(قدر_بٹ, tmp_قدر.value.str, apvword->part_wid) == 0)
	    أرجع خطأ;

      حافظہ_نقل(قدر_بٹ, tmp_قدر.value.str, apvword->part_wid);
      أرجع سچ;

}

قدر_callback*vpip_array_لفظ_change(p_cb_انکڑے انکڑے)
{
      بنية __vpiArray*ماپا = 0;
      array_لفظ_قدر_callback*cbh = 0;
      إذا (ثابت بنية __vpiArrayWord*لفظ = array_var_لفظ_from_handle(انکڑے->obj)) {
	    ماپا = static_cast<__vpiArray*>(لفظ->get_ماپا());
	    بنانشانی addr = لفظ->get_اشاریہ();
	    cbh = جديد array_لفظ_قدر_callback(انکڑے, addr);

      } وإلا إذا (بنية __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(انکڑے->obj)) {
	    ماپا = tword->array;
	    cbh = جديد array_لفظ_قدر_callback(انکڑے, tword->پتہ);

      } وإلا إذا (بنية __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(انکڑے->obj)) {
	    ماپا = apvword->array;
	    cbh = جديد array_لفظ_part_callback(انکڑے, apvword->لفظ_sel);
      }

      أكد(cbh);
      أكد(ماپا);
      cbh->اگلا_2 = ماپا->vpi_callbacks;
      ماپا->vpi_callbacks = cbh;

      أرجع cbh;
}

قدر_callback* vpip_array_change(p_cb_انکڑے انکڑے)
{
      array_لفظ_قدر_callback*cbh = جديد array_لفظ_قدر_callback(انکڑے, -1);
      أكد(انکڑے->obj);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(انکڑے->obj);
      cbh->اگلا_2 = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      أرجع cbh;
}

فراغ compile_array_پورٹ(حرف*label, حرف*array, حرف*addr)
{
      array_پورٹ_resolv_list_t*resolv_mem
	    = جديد array_پورٹ_resolv_list_t(array, خطأ, 0);

      define_functor_علامت(label, resolv_mem->ptr);
      حرر(label);
	// Connect the port-0 input as the address.
      داخلہ_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

فراغ compile_array_پورٹ(حرف*label, حرف*array, لما addr)
{
      array_پورٹ_resolv_list_t*resolv_mem
	    = جديد array_پورٹ_resolv_list_t(array, سچ, addr);

      define_functor_علامت(label, resolv_mem->ptr);
      حرر(label);

      resolv_submit(resolv_mem);
}

فراغ compile_array_alias(حرف*label, حرف*name, حرف*src)
{
      vvp_array_t mem = array_لبھو(src);
      أكد(mem);

      بنية __vpiArray*obj = جديد __vpiArray;

      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_گنتی = mem->array_گنتی;
      obj->signed_flag = mem->signed_flag;

	// Need to set an accurate range of addresses.
      obj->پہلا_addr = mem->پہلا_addr;
      obj->آخری_addr  = mem->آخری_addr;
      obj->وٹاندرا_addr = mem->وٹاندرا_addr;

      obj->msb = mem->msb;
      obj->lsb = mem->lsb;

	// Share the words with the source array.
      obj->nets = mem->nets;
      obj->vals4 = mem->vals4;
      obj->vals  = mem->vals;
      obj->vals_چوڑائی = mem->vals_چوڑائی;
      obj->vals_الفاظ = mem->vals_الفاظ;

      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

      أكد(array_جدول);
      أكد(!array_لبھو(label));
      array_جدول->sym_سیٹ_قدر(label, obj);

      compile_vpi_علامت(label, obj);
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
vpiHandle vpip_make_vthr_A(حرف*label, بنانشانی addr)
{
      بنية __vpiArrayVthrA*obj = جديد __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = جديد array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->پتہ_handle = 0;
      obj->پتہ = addr;

      أرجع obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(حرف*label, حرف*علامت)
{
      بنية __vpiArrayVthrA*obj = جديد __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = جديد array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->پتہ_handle = 0;
      compile_vpi_lookup(&obj->پتہ_handle, علامت);
      obj->پتہ = 0;

      أرجع obj;
}

vpiHandle vpip_make_vthr_A(حرف*label, vpiHandle handle)
{
      بنية __vpiArrayVthrA*obj = جديد __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = جديد array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->پتہ_handle = handle;
      obj->پتہ = 0;

      أرجع obj;
}

vpiHandle vpip_make_vthr_APV(حرف*label, بنانشانی اشاریہ, بنانشانی ثنائی_ہندسہ, بنانشانی wid)
{
      بنية __vpiArrayVthrAPV*obj = جديد __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = جديد array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->لفظ_sel = اشاریہ;
      obj->part_ثنائی_ہندسہ = ثنائی_ہندسہ;
      obj->part_wid = wid;

      أرجع obj;
}

فراغ compile_array_cleanup(فراغ)
{
      مٹاؤ array_جدول;
      array_جدول = 0;
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ حافظہ_delete(vpiHandle شے)
{
      بنية __vpiArray*arr = static_cast<بنية __vpiArray*>(شے);
      إذا (arr->vals_الفاظ) مٹاؤ [] (arr->vals_الفاظ-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      مٹاؤ arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      مٹاؤ arr->vals;

      إذا (arr->nets) {
	    لكل (بنانشانی idx = 0; idx < arr->get_ناپ(); idx += 1) {
		  إذا (بنية __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.اشاریہ);
		    /* These should only be the real words. */
		  } وإلا {
			أكد(arr->nets[idx]->get_type_کوڈ() ==
			       vpiRealVar);
			بنية __vpiRealVar *sigr = static_cast<بنية __vpiRealVar *>
			                            (arr->nets[idx]);
			constant_delete(sigr->id.اشاریہ);
// Why are only the real words still here?
			مٹاؤ arr->nets[idx];
		  }
	    }
	    حرر(arr->nets);
      }

      طالما (arr->vpi_callbacks) {
	    بنية __vpiCallback*tmp = arr->vpi_callbacks->اگلا_2;
	    مٹاؤ arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      مٹاؤ arr;
}

فراغ A_delete(vpiHandle شے)
{
      بنية __vpiArrayVthrA*obj = static_cast<بنية __vpiArrayVthrA*>(شے);
      إذا (obj->پتہ_handle) {
	    بدل (obj->پتہ_handle->get_type_کوڈ()) {
		حالة vpi_حافظہ_لفظ:
		  إذا (vpi_get(_vpiFromThr, obj->پتہ_handle) == _vpi_at_A) {
			A_delete(obj->پتہ_handle);
		  }
		  اكسر;
		حالة vpiPartSelect:
		  أكد(vpi_get(_vpiFromThr, obj->پتہ_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->پتہ_handle);
		  اكسر;
	    }
      }

      مٹاؤ obj;
}

فراغ APV_delete(vpiHandle شے)
{
      بنية __vpiArrayVthrAPV*obj = static_cast<بنية __vpiArrayVthrAPV*>(شے);
      مٹاؤ obj;
}
#جے_انت
