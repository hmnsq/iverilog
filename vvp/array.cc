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
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
#ضمن  "vvp_cleanup.h"
#جيڪڏهن_پورو
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <iostream>
# include  "compile.h"
# include  <cassert>
# include  "ivl_alloc.h"

استعمال نطاق std;

بنانشان ڊگهو ڳڻپ_net_arrays = 0;
بنانشان ڊگهو ڳڻپ_net_array_لفظ = 0;
بنانشان ڊگهو ڳڻپ_var_arrays = 0;
بنانشان ڊگهو ڳڻپ_var_array_لفظ = 0;
بنانشان ڊگهو ڳڻپ_real_arrays = 0;
بنانشان ڊگهو ڳڻپ_real_array_لفظ = 0;

ساكن نشان_map_s<بنية __vpiArray>* array_جدول =0;

صنف vvp_fun_arrayport;
ساكن فراغ array_attach_پورٽ(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_ڳوليو(ثابت حرف*label)
{
      إذا (array_جدول == 0)
	    أرجع 0;

      vvp_array_t v = array_جدول->sym_get_قدر(label);
      أرجع v;
}

بنية __vpiArrayVthrA : عام __vpiHandle {
      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpi_يادگيري_لفظ; }
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح code) override;

      بنية __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle پتو_handle;
	// If wid==0, then address is the address into the array.
      بنانشان پتو;

      بنانشان get_پتو() ثابت
      {
	    إذا (پتو_handle) {
		  s_vpi_قدر vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpi_ويڪٽر_Val;
		  پتو_handle->vpi_get_قدر(&vp);
		  صحيح لفظ = (پتو_handle->vpi_get(vpi_سائيز)-1)/32 + 1;
		  لكل(صحيح idx = 0; idx < لفظ; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			إذا (vp.value.vector[idx].bval != 0) أرجع UINT_MAX;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  پتو_handle->vpi_get_قدر(&vp);
		  أرجع vp.value.integer;
	    }

	    أرجع پتو;
      }
};


بنية __vpiArrayVthrAPV : عام __vpiHandle {
      صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpiPartSelect; }
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_handle(صحيح code) override;

      بنية __vpiArray*array;
      بنانشان لفظ_sel;
      بنانشان part_ٻٽو_انگ;
      بنانشان part_wid;
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
      أكد(arr->get_سائيز() > 0);
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

صحيح __vpiArray::get_لفظ_سائيز() ثابت
{
      بنانشان width;

      أكد(get_سائيز() > 0);
	/* For a net array we need to get the width from the first element. */
      إذا (nets) {
	    أكد(vals4 == 0 && vals == 0);
	    ثابت بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    أكد(vsig);
	    width = vpip_سائيز(vsig);
	/* For a variable array we can get the width from vals_width. */
      } وإلا {
	    أكد(vals4 || vals);
	    width = vals_ويڪر;
      }

      أرجع width;
}

حرف*__vpiArray::get_لفظ_str(بنية __vpiArrayWord*لفظ_2, صحيح code)
{
      بنانشان اشاريو = لفظ_2->get_اشاريو();

      إذا (code == vpiFile) {  // Not implemented for now!
	    أرجع simple_سيٽ_rbuf_str(file_نالا[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%d", (صحيح)اشاريو + پهريون_addr.get_قدر());
      أرجع generic_get_str(code, get_scope(), name, sidx);
}

فراغ __vpiArray::get_لفظ_قدر(بنية __vpiArrayWord*لفظ_2, p_vpi_قدر vp)
{
      بنانشان اشاريو = لفظ_2->get_اشاريو();

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
          vpip_vec4_get_قدر(vals4->get_لفظ(اشاريو),
                              vals4->width(), signed_flag, vp);
      } وإلا إذا(vals) {
          بدل(vp->format) {
            حالة vpiBinStrVal:
            حالة vpiOctStrVal:
            حالة vpiDecStrVal:
            حالة vpiHexStrVal:
            حالة vpiScalarVal:
            حالة vpiIntVal:
            حالة vpi_ويڪٽر_Val:
            {
                vvp_vector4_t v;
                vals->get_لفظ(اشاريو, v);
                vpip_vec4_get_قدر(v, vals_ويڪر, signed_flag, vp);
            }
            اكسر;

            حالة vpiRealVal:
            {
                مزدوج d;
                vals->get_لفظ(اشاريو, d);
                vpip_real_get_قدر(d, vp);
            }
            اكسر;

            حالة vpiStringVal:
            {
                string s;
                vals->get_لفظ(اشاريو, s);
                vpip_string_get_قدر(s, vp);
            }
            اكسر;

            افتراضي:
                fprintf(stderr, "vpi sorry: format is not implemented\n");
                أكد(خطأ);
          }
      }
}

فراغ __vpiArray::put_لفظ_قدر(بنية __vpiArrayWord*لفظ_2, p_vpi_قدر vp, صحيح)
{
      بنانشان اشاريو = لفظ_2->get_اشاريو();
      vvp_vector4_t val = vec4_from_vpi_قدر(vp, vals_ويڪر);
      سيٽ_لفظ(اشاريو, 0, val);
}

vpiHandle __vpiArray::get_iter_اشاريو(بنية __vpiArrayIterator*, صحيح idx)
{
      إذا (nets) أرجع nets[idx];

      أكد(vals4 || vals);

      إذا (vals_لفظ == 0) make_vals_لفظ();

      أرجع &(vals_لفظ[idx].as_لفظ);
}

صحيح __vpiArray::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_سٽ_No:
	    أرجع 0; // Not implemented for now!

	  حالة vpi_سائيز:
	    أرجع get_سائيز();

	  حالة vpiAutomatic:
	    أرجع scope->is_automatic()? 1 : 0;

	  افتراضي:
	    أرجع 0;
      }
}

حرف* __vpiArray::vpi_get_str(صحيح code)
{
      إذا (code == vpiFile) {  // Not implemented for now!
            أرجع simple_سيٽ_rbuf_str(file_نالا[0]);
      }

      أرجع generic_get_str(code, scope, name, عدم);
}

vpiHandle __vpiArray::vpi_handle(صحيح code)
{
      بدل (code) {
          حالة vpi_کاٻو_Range:
            إذا (مٽا_سٽا_addr) أرجع &آخري_addr;
            وإلا أرجع &پهريون_addr;

          حالة vpi_ساڄو_Range:
            إذا (مٽا_سٽا_addr) أرجع &پهريون_addr;
            وإلا أرجع &آخري_addr;

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
vpiHandle __vpiArray::vpi_اشاريو(صحيح اشاريو)
{
      اشاريو -= پهريون_addr.get_قدر();
      إذا (اشاريو >= (ڊگهو) get_سائيز())
	    أرجع 0;
      إذا (اشاريو < 0)
	    أرجع 0;

      إذا (nets != 0) {
	    أرجع nets[اشاريو];
      }

      إذا (vals_لفظ == 0)
	    make_vals_لفظ();

      أرجع &(vals_لفظ[اشاريو].as_لفظ);
}

صحيح __vpiArrayWord::as_لفظ_t::vpi_get(صحيح code)
{
      بنية __vpiArrayWord*obj = array_var_لفظ_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_والد = obj->get_والد();
      t_vpi_قدر val;

      بدل (code) {
	  حالة vpi_سٽ_No:
	    أرجع 0; // Not implemented for now!

	  حالة vpi_سائيز:
	    أرجع my_والد->get_لفظ_سائيز();

	  حالة vpi_کاٻو_Range:
            val.format = vpiIntVal;
	    my_والد->get_کاٻو_range()->vpi_get_قدر(&val);
            أكد(val.format == vpiIntVal);
            أرجع val.value.integer;

	  حالة vpi_ساڄو_Range:
            val.format = vpiIntVal;
	    my_والد->get_ساڄو_range()->vpi_get_قدر(&val);
            أكد(val.format == vpiIntVal);
            أرجع val.value.integer;

	  حالة vpi_اشاريو_2:
	    {
		  صحيح بنياد_وٿي = 0;
		  بنية __vpiArray*بنياد = dynamic_cast<__vpiArray*> (my_والد);
		  إذا (بنياد) {
			val.format = vpiIntVal;
			بنياد->پهريون_addr.vpi_get_قدر(&val);
			بنياد_وٿي += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_اشاريو.vpi_get_قدر(&val);
		  أكد(val.format == vpiIntVal);
		  أرجع val.value.integer + بنياد_وٿي;
	    }

	  حالة vpiAutomatic:
	    أرجع my_والد->get_scope()->is_automatic()? 1 : 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    أرجع _vpiNoThr;
#جيڪڏهن_پورو

	  افتراضي:
	    أرجع 0;
      }
}


صحيح __vpiArrayVthrA::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_سٽ_No:
	    أرجع 0; // Not implemented for now!

	  حالة vpi_سائيز:
	    أرجع array->get_لفظ_سائيز();

	  حالة vpi_کاٻو_Range:
	    أرجع array->msb.get_قدر();

	  حالة vpi_ساڄو_Range:
	    أرجع array->lsb.get_قدر();

	  حالة vpi_اشاريو_2:
	    أرجع (صحيح)get_پتو() + array->پهريون_addr.get_قدر();

	  حالة vpiAutomatic:
	    أرجع array->get_scope()->is_automatic() ? 1 : 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    أرجع _vpi_at_A;
#جيڪڏهن_پورو

	  // If address_handle is not zero we definitely have a
	  // variable.
	  حالة vpiConstantSelect:
	    أرجع پتو_handle == 0;

	  افتراضي:
	    أرجع 0;
      }
}

حرف* __vpiArrayVthrA::vpi_get_str(صحيح code)
{
      إذا (code == vpiFile) {  // Not implemented for now!
            أرجع simple_سيٽ_rbuf_str(file_نالا[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%d", (صحيح)get_پتو() + array->پهريون_addr.get_قدر());
      أرجع generic_get_str(code, array->get_scope(), array->name, sidx);
}

فراغ __vpiArrayVthrA::vpi_get_قدر(p_vpi_قدر vp)
{
      أكد(array);

      بنانشان اشاريو = get_پتو();
      إذا (vpi_array_is_real(array)) {
	    مزدوج tmp = array->get_لفظ_r(اشاريو);
	    vpip_real_get_قدر(tmp, vp);
      } وإلا إذا (vpi_array_is_string(array)) {
	    string tmp = array->get_لفظ_str(اشاريو);
	    vpip_string_get_قدر(tmp, vp);
      } وإلا {
	    vvp_vector4_t tmp = array->get_لفظ(اشاريو);
	    بنانشان width = array->get_لفظ_سائيز();
	    vpip_vec4_get_قدر(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_قدر(p_vpi_قدر vp, صحيح)
{
      بنانشان اشاريو = get_پتو();

      أكد(array);
      أكد(اشاريو < array->get_سائيز());

      إذا (vpi_array_is_real(array)) {
	    مزدوج val = real_from_vpi_قدر(vp);
	    array->سيٽ_لفظ(اشاريو, val);
      } وإلا {
	    بنانشان width = array->get_لفظ_سائيز();
	    vvp_vector4_t val = vec4_from_vpi_قدر(vp, width);
	    array->سيٽ_لفظ(اشاريو, 0, val);
      }

      أرجع هذا;
}

vpiHandle __vpiArrayVthrA::vpi_handle(صحيح code)
{
      بدل (code) {
	  حالة vpi_اشاريو_2:
	    اكسر;  // Not implemented!

	  حالة vpi_کاٻو_Range:
	    أرجع &array->msb;

	  حالة vpi_ساڄو_Range:
	    أرجع &array->lsb;

	  حالة vpi_والد:
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
	  حالة vpi_سٽ_No:
	    أرجع 0; // Not implemented for now!

	  حالة vpi_سائيز:
	    أرجع part_wid;

	  حالة vpi_کاٻو_Range:
	    أرجع part_ٻٽو_انگ + part_wid - 1;

	  حالة vpi_ساڄو_Range:
	    أرجع part_ٻٽو_انگ;

	  حالة vpiAutomatic:
	    أرجع array->get_scope()->is_automatic() ? 1 : 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    أرجع _vpi_at_APV;
#جيڪڏهن_پورو

	  حالة vpiConstantSelect:
	    أرجع 1;

	  افتراضي:
	    أرجع 0;
      }
}

حرف* __vpiArrayVthrAPV::vpi_get_str(صحيح code)
{
      إذا (code == vpiFile) {  // Not implemented for now!
            أرجع simple_سيٽ_rbuf_str(file_نالا[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%u", لفظ_sel + array->پهريون_addr.get_قدر());
      أرجع generic_get_str(code, array->get_scope(), array->name, sidx);
}

فراغ __vpiArrayVthrAPV::vpi_get_قدر(p_vpi_قدر vp)
{
      أكد(array);

      بنانشان اشاريو = لفظ_sel;
      إذا (vpi_array_is_real(array)) {
	    مزدوج tmp = array->get_لفظ_r(اشاريو);
	    vpip_real_get_قدر(tmp, vp);
      } وإلا {
	    vvp_vector4_t tmp = array->get_لفظ(اشاريو);
	    tmp = tmp.subvalue(part_ٻٽو_انگ, part_wid);
	    vpip_vec4_get_قدر(tmp, part_wid, خطأ, vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(صحيح code)
{
      بدل (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  حالة vpi_والد:
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

فراغ __vpiArray::سيٽ_لفظ(بنانشان پتو, بنانشان part_off, ثابت vvp_vector4_t&val)
{
      إذا (پتو >= get_سائيز())
	    أرجع;

      إذا (vals4) {
	    أكد(nets == 0);
	    إذا (part_off != 0 || val.size() != vals_ويڪر) {
		  vvp_vector4_t tmp = vals4->get_لفظ(پتو);
		  إذا ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_ويڪر << سٽ_پڄاڻي;
			أكد(0);
		  }
		  tmp.سيٽ_vec(part_off, val);
		  vals4->سيٽ_لفظ(پتو, tmp);
	    } وإلا {
		  vals4->سيٽ_لفظ(پتو, val);
	    }
	    لفظ_change(پتو);
	    أرجع;
      }

      إذا (vals) {
	    أكد(nets == 0);
	    إذا (part_off != 0 || val.size() != vals_ويڪر) {
		  vvp_vector4_t tmp;
		  vals->get_لفظ(پتو, tmp);
		  إذا ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_ويڪر << سٽ_پڄاڻي;
			أكد(0);
		  }
		  tmp.سيٽ_vec(part_off, val);
		  vals->سيٽ_لفظ(پتو, tmp);
	    } وإلا {
		  vals->سيٽ_لفظ(پتو, val);
	    }
	    لفظ_change(پتو);
	    أرجع;
      }

      أكد(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle لفظ_2 = nets[پتو];
      ثابت بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(لفظ_2);
      أكد(vsig);

      vsig->ڳنڍ->send_vec4_pv(val, part_off, vpip_سائيز(vsig), 0);
      لفظ_change(پتو);
}

فراغ __vpiArray::سيٽ_لفظ(بنانشان پتو, مزدوج val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      إذا (پتو >= vals->get_سائيز())
	    أرجع;

      vals->سيٽ_لفظ(پتو, val);
      لفظ_change(پتو);
}

فراغ __vpiArray::سيٽ_لفظ(بنانشان پتو, ثابت string&val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      إذا (پتو >= vals->get_سائيز())
	    أرجع;

      vals->سيٽ_لفظ(پتو, val);
      لفظ_change(پتو);
}

فراغ __vpiArray::سيٽ_لفظ(بنانشان پتو, ثابت vvp_object_t&val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      إذا (پتو >= vals->get_سائيز())
	    أرجع;

      vals->سيٽ_لفظ(پتو, val);
      لفظ_change(پتو);
}

vvp_vector4_t __vpiArray::get_لفظ(بنانشان پتو)
{
      إذا (vals4) {
	    أكد(nets == 0);
	    أكد(vals == 0);
	    أرجع vals4->get_لفظ(پتو);
      }

      إذا (vals) {
	    أكد(nets == 0);
	    أكد(vals4== 0);
	    إذا (پتو >= vals->get_سائيز())
		  أرجع vvp_vector4_t(vals_ويڪر, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_لفظ(پتو, val);
	    أرجع val;
      }

      أكد(vals4 == 0);
      أكد(vals == 0);
      أكد(nets != 0);

      إذا (پتو >= get_سائيز()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    أكد(get_سائيز() > 0);
	    vpiHandle لفظ_2 = nets[0];
	    أكد(لفظ_2);
	    بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(لفظ_2);
	    أكد(vsig);
	    vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (vsig->ڳنڍ->fil);
	    أكد(sig);
	    أرجع vvp_vector4_t(sig->قدر_سائيز(), BIT4_X);
      }

      vpiHandle لفظ_2 = nets[پتو];
      بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(لفظ_2);
      أكد(vsig);
      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (vsig->ڳنڍ->fil);
      أكد(sig);

      vvp_vector4_t val;
      sig->vec4_قدر(val);
      أرجع val;
}

مزدوج __vpiArray::get_لفظ_r(بنانشان پتو)
{
      إذا (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    إذا (پتو >= vals->get_سائيز())
		  أرجع 0.0;

	    مزدوج val;
	    vals->get_لفظ(پتو, val);
	    أرجع val;
      }

      إذا (پتو >= get_سائيز())
	    أرجع 0.0;

      أكد(nets);
      vpiHandle لفظ_2 = nets[پتو];
      بنية __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(لفظ_2);
      أكد(vsig);
      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (vsig->net->fil);
      أكد(sig);

      مزدوج val = sig->real_قدر();
      أرجع val;
}

فراغ __vpiArray::get_لفظ_obj(بنانشان پتو, vvp_object_t&val)
{
      إذا (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    إذا (پتو >= vals->get_سائيز()) {
		  val = vvp_object_t();
		  أرجع;
	    }

	    vals->get_لفظ(پتو, val);
	    أرجع;
      }

      أكد(nets);
	// Arrays of string nets not implemented!
      أكد(0);
      أرجع;
}

string __vpiArray::get_لفظ_str(بنانشان پتو)
{
      إذا (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    إذا (پتو >= vals->get_سائيز())
		  أرجع "";

	    string val;
	    vals->get_لفظ(پتو, val);
	    أرجع val;
      }

      أكد(nets);
	// Arrays of string nets not implemented!
      أكد(0);
      أرجع "";
}

vpiHandle vpip_make_array(ثابت حرف*label, ثابت حرف*name,
                          صحيح پهريون_addr, صحيح آخري_addr,
                          منطقي signed_flag)
{
      بنية __vpiArray*obj = جديد __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      إذا (آخري_addr >= پهريون_addr) {
	    obj->مٽا_سٽا_addr = خطأ;
      } وإلا {
	    obj->مٽا_سٽا_addr = سچ;
	    صحيح tmp = آخري_addr;
	    آخري_addr = پهريون_addr;
	    پهريون_addr = tmp;
      }
      أكد(آخري_addr >= پهريون_addr);
      بنانشان array_ڳڻپ = آخري_addr+1-پهريون_addr;

	// For now, treat all arrays as memories. This is not quite
	// correct, as arrays are arrays with memories a special case.
      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_ڳڻپ = array_ڳڻپ;

      obj->پهريون_addr.سيٽ_قدر(پهريون_addr);
      obj->آخري_addr .سيٽ_قدر(آخري_addr);

	// Start off now knowing if we are nets or variables.
      obj->nets = 0;
      obj->vals4 = 0;
      obj->vals  = 0;
      obj->vals_ويڪر = 0;
      obj->vals_لفظ = 0;

	// Initialize (clear) the read-ports list.
      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

	/* Add this symbol to the array_symbols table for later lookup. */
      إذا (!array_جدول)
	    array_جدول = جديد نشان_map_s<بنية __vpiArray>;

      أكد(!array_ڳوليو(label));
      array_جدول->sym_سيٽ_قدر(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_نشان(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_منزل_current_scope(obj);

      أرجع obj;
}

فراغ __vpiArray::alias_لفظ(بنانشان ڊگهو addr, vpiHandle لفظ_2, صحيح msb_, صحيح lsb_)
{
      أكد(msb.get_قدر() == msb_);
      أكد(lsb.get_قدر() == lsb_);
      أكد(addr < get_سائيز());
      أكد(nets);
      nets[addr] = لفظ_2;
}

فراغ __vpiArray::attach_لفظ(بنانشان addr, vpiHandle لفظ_2)
{
      أكد(addr < get_سائيز());
      أكد(nets);
      nets[addr] = لفظ_2;

      إذا (بنية __vpiSignal*sig = dynamic_cast<__vpiSignal*>(لفظ_2)) {
	    vvp_net_t*net = sig->ڳنڍ;
	    أكد(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    أكد(fun);
	    fun->attach_as_لفظ(هذا, addr);
	    sig->is_netarray = 1;
	    sig->within.والد = هذا;
	    sig->id.اشاريو = جديد __vpiDecConst(addr + پهريون_addr.get_قدر());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    أرجع;
      }

      إذا (بنية __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(لفظ_2)) {
	    vvp_net_t*net = sig->net;
	    أكد(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    أكد(fun);
	    fun->attach_as_لفظ(هذا, addr);
	    sig->is_netarray = 1;
	    sig->within.والد = هذا;
	    sig->id.اشاريو = جديد __vpiDecConst(addr + پهريون_addr.get_قدر());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = سچ;
	    أرجع;
      }
}

فراغ compile_var_array(حرف*label, حرف*name, صحيح آخري, صحيح پهريون,
		   صحيح msb, صحيح lsb, حرف signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, پهريون, آخري,
                                      signed_flag != 0);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_ويڪر = labs(msb-lsb) + 1;
      إذا (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = جديد vvp_vector4array_aa(arr->vals_ويڪر,
						 arr->get_سائيز());
      } وإلا {
            arr->vals4 = جديد vvp_vector4array_sa(arr->vals_ويڪر,
						 arr->get_سائيز());
      }
      arr->msb.سيٽ_قدر(msb);
      arr->lsb.سيٽ_قدر(lsb);

      ڳڻپ_var_arrays += 1;
      ڳڻپ_var_array_لفظ += arr->get_سائيز();

      حرر(label);
      ميٽيو[] name;
}

فراغ compile_var2_array(حرف*label, حرف*name, صحيح آخري, صحيح پهريون,
		   صحيح msb, صحيح lsb, منطقي signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, پهريون, آخري, signed_flag);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.سيٽ_قدر(msb);
      arr->lsb.سيٽ_قدر(lsb);
      arr->vals_ويڪر = labs(msb-lsb) + 1;

      أكد(! arr->nets);
      إذا (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = جديد vvp_darray_atom<int8_t>(arr->get_سائيز());
      } وإلا إذا (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = جديد vvp_darray_atom<uint8_t>(arr->get_سائيز());
      } وإلا إذا (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = جديد vvp_darray_atom<int16_t>(arr->get_سائيز());
      } وإلا إذا (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = جديد vvp_darray_atom<uint16_t>(arr->get_سائيز());
      } وإلا إذا (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = جديد vvp_darray_atom<int32_t>(arr->get_سائيز());
      } وإلا إذا (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = جديد vvp_darray_atom<uint32_t>(arr->get_سائيز());
      } وإلا إذا (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = جديد vvp_darray_atom<int64_t>(arr->get_سائيز());
      } وإلا إذا (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = جديد vvp_darray_atom<uint64_t>(arr->get_سائيز());
      } وإلا {
	    arr->vals = جديد vvp_darray_vec2(arr->get_سائيز(), arr->vals_ويڪر);
      }
      ڳڻپ_var_arrays += 1;
      ڳڻپ_var_array_لفظ += arr->get_سائيز();

      حرر(label);
      ميٽيو[] name;
}

فراغ compile_real_array(حرف*label, حرف*name, صحيح آخري, صحيح پهريون)
{
      vpiHandle obj = vpip_make_array(label, name, پهريون, آخري, سچ);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = جديد vvp_darray_real(arr->get_سائيز());
      arr->vals_ويڪر = 1;

      ڳڻپ_real_arrays += 1;
      ڳڻپ_real_array_لفظ += arr->get_سائيز();

      حرر(label);
      ميٽيو[] name;
}

فراغ compile_string_array(حرف*label, حرف*name, صحيح آخري, صحيح پهريون)
{
      vpiHandle obj = vpip_make_array(label, name, پهريون, آخري, سچ);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = جديد vvp_darray_string(arr->get_سائيز());
      arr->vals_ويڪر = 1;

      ڳڻپ_real_arrays += 1;
      ڳڻپ_real_array_لفظ += arr->get_سائيز();

      حرر(label);
      ميٽيو[] name;
}

فراغ compile_object_array(حرف*label, حرف*name, صحيح آخري, صحيح پهريون)
{
      vpiHandle obj = vpip_make_array(label, name, پهريون, آخري, سچ);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = جديد vvp_darray_object(arr->get_سائيز());
      arr->vals_ويڪر = 1;

      ڳڻپ_real_arrays += 1;
      ڳڻپ_real_array_لفظ += arr->get_سائيز();

      حرر(label);
      ميٽيو[] name;
}

فراغ compile_net_array(حرف*label, حرف*name, صحيح آخري, صحيح پهريون)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, پهريون, آخري, خطأ);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(صفر_يادگيري_ڏيو(arr->get_سائيز(), حجم(vpiHandle)));

      ڳڻپ_net_arrays += 1;
      ڳڻپ_net_array_لفظ += arr->get_سائيز();

      حرر(label);
      ميٽيو[] name;
}

صنف vvp_fun_arrayport  : عام vvp_net_fun_t {

    عام:
      واضح vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      واضح vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, ڊگهو addr);
      ~vvp_fun_arrayport() override;

      مجازي فراغ check_لفظ_change(بنانشان ڊگهو addr) = 0;

    محمي:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      بنانشان ڊگهو addr_;

      دوست فراغ array_attach_پورٽ(vvp_array_t, vvp_fun_arrayport*);
      دوست فراغ __vpiArray::لفظ_change(بنانشان ڊگهو);
      vvp_fun_arrayport*ايندڙ;
};

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net)
: arr_(mem), net_(net), addr_(0)
{
      ايندڙ = 0;
}

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, ڊگهو addr)
: arr_(mem), net_(net), addr_(addr)
{
      ايندڙ = 0;
}

vvp_fun_arrayport::~vvp_fun_arrayport()
{
}

صنف vvp_fun_arrayport_sa  : عام vvp_fun_arrayport {

    عام:
      واضح vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      واضح vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, ڊگهو addr);
      ~vvp_fun_arrayport_sa() override;

      فراغ check_لفظ_change(بنانشان ڊگهو addr) override;

      فراغ recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                     vvp_context_t) override;

    خاص:
};

vvp_fun_arrayport_sa::vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net)
{
}

vvp_fun_arrayport_sa::vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, ڊگهو addr)
: vvp_fun_arrayport(mem, net, addr)
{
}

vvp_fun_arrayport_sa::~vvp_fun_arrayport_sa()
{
}

فراغ vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                                     vvp_context_t)
{
      منطقي addr_valid_flag;

      بدل (پورٽ.پورٽ()) {

	  حالة 0: // Address input
	    addr_valid_flag = vector4_منزل_قدر(ٻٽو_انگ, addr_);
	    إذا (! addr_valid_flag)
		  addr_ = arr_->get_سائيز();
	    إذا (vpi_array_is_real(arr_))
		  پورٽ.ptr()->send_real(arr_->get_لفظ_r(addr_), 0);
	    وإلا
		  پورٽ.ptr()->send_vec4(arr_->get_لفظ(addr_), 0);

	    اكسر;

	  افتراضي:
	    fprintf(stdout, "XXXX write ports not implemented.\n");
	    أكد(0);
      }
}

فراغ vvp_fun_arrayport_sa::check_لفظ_change(بنانشان ڊگهو addr)
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
      واضح vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, ڊگهو addr);
      ~vvp_fun_arrayport_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#جيڪڏهن_پورو

      فراغ check_لفظ_change(بنانشان ڊگهو addr) override;

      فراغ recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                     vvp_context_t context) override;

    خاص:
      فراغ check_لفظ_change_2(بنانشان ڊگهو addr, vvp_context_t context);

      __vpiScope*context_scope_;
      بنانشان context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_جوڙيو_شي_منزل_context(هذا, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, ڊگهو addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_جوڙيو_شي_منزل_context(هذا, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

فراغ vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      بنانشان ڊگهو*addr = جديد بنانشان ڊگهو;
      vvp_سيٽ_context_شي(context, context_idx_, addr);

      *addr = addr_;
}

فراغ vvp_fun_arrayport_aa::reset_instance(vvp_context_t context)
{
      بنانشان ڊگهو*addr = static_cast<بنانشان ڊگهو*>
            (vvp_get_context_شي(context, context_idx_));

      *addr = addr_;
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ vvp_fun_arrayport_aa::free_instance(vvp_context_t context)
{
      بنانشان ڊگهو*addr = static_cast<بنانشان ڊگهو*>
            (vvp_get_context_شي(context, context_idx_));
      ميٽيو addr;
}
#جيڪڏهن_پورو

فراغ vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t پورٽ, ثابت vvp_vector4_t&ٻٽو_انگ,
                                     vvp_context_t context)
{
      إذا (context) {
            بنانشان ڊگهو*addr = static_cast<بنانشان ڊگهو*>
                  (vvp_get_context_شي(context, context_idx_));

            منطقي addr_valid_flag;

            بدل (پورٽ.پورٽ()) {

                حالة 0: // Address input
                  addr_valid_flag = vector4_منزل_قدر(ٻٽو_انگ, *addr);
                  إذا (! addr_valid_flag) *addr = arr_->get_سائيز();
                  إذا (vpi_array_is_real(arr_)) {
			پورٽ.ptr()->send_real(arr_->get_لفظ_r(*addr),
					      context);
                  } وإلا {
			پورٽ.ptr()->send_vec4(arr_->get_لفظ(*addr),
					      context);
                  }
                  اكسر;

                افتراضي:
                  fprintf(stdout, "XXXX write ports not implemented.\n");
                  أكد(0);
            }
      } وإلا {
            context = context_scope_->جيئرو_contexts;
            طالما (context) {
                  recv_vec4(پورٽ, ٻٽو_انگ, context);
                  context = vvp_get_ايندڙ_context(context);
            }
      }
}

فراغ vvp_fun_arrayport_aa::check_لفظ_change_2(بنانشان ڊگهو addr,
                                              vvp_context_t context)
{
      ثابت بنانشان ڊگهو*پورٽ_addr = static_cast<بنانشان ڊگهو*>
            (vvp_get_context_شي(context, context_idx_));

      إذا (addr != *پورٽ_addr)
	    أرجع;

      إذا (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_لفظ_r(addr), context);
      } وإلا {
	    net_->send_vec4(arr_->get_لفظ(addr), context);
      }
}

فراغ vvp_fun_arrayport_aa::check_لفظ_change(بنانشان ڊگهو addr)
{
      إذا (arr_->get_scope()->is_automatic()) {
            أكد(vthread_get_wt_context());
            check_لفظ_change_2(addr, vthread_get_wt_context());
      } وإلا {
            vvp_context_t context = context_scope_->جيئرو_contexts;
            طالما (context) {
                  check_لفظ_change_2(addr, context);
                  context = vvp_get_ايندڙ_context(context);
            }
      }
}

ساكن فراغ array_attach_پورٽ(vvp_array_t array, vvp_fun_arrayport*fun)
{
      أكد(fun->ايندڙ == 0);
      fun->ايندڙ = array->ports_;
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
                  vvp_vector4_t tmp(array->vals_ويڪر, init);
                  وقت_جدول_init_propagate(fun->net_, tmp);
            } وإلا {
                  وقت_جدول_init_propagate(fun->net_, 0.0);
            }
      }
}

صنف array_لفظ_قدر_callback : عام قدر_callback {
    عام:
      مضمن واضح array_لفظ_قدر_callback(p_cb_معلومات معلومات, ڊگهو addr)
      : قدر_callback(معلومات), لفظ_addr(addr)
      { }

    عام:
      ڊگهو لفظ_addr;
};

فراغ __vpiArray::لفظ_change(بنانشان ڊگهو addr)
{
      لكل (vvp_fun_arrayport*cur = ports_; cur; cur = cur->ايندڙ)
	    cur->check_لفظ_change(addr);

	// Run callbacks attached to the array itself.
      بنية __vpiCallback *ايندڙ_2 = vpi_callbacks;
      بنية __vpiCallback *prev = 0;

      طالما (ايندڙ_2) {
	    array_لفظ_قدر_callback*cur = dynamic_cast<array_لفظ_قدر_callback*>(ايندڙ_2);
	    ايندڙ_2 = cur->ايندڙ_2;

	      // Skip callbacks that are not for me. -1 is for every element.
	    إذا (cur->لفظ_addr != (ڊگهو)addr && cur->لفظ_addr != -1) {
		  prev = cur;
		  استمر;
	    }

	      // For whole array callbacks we need to set the index.
	    إذا (cur->لفظ_addr == -1) {
		  cur->cb_معلومات.اشاريو = (PLI_INT32) ((صحيح)addr +
						    پهريون_addr.get_قدر());
	    }

	    إذا (cur->cb_معلومات.cb_rtn != 0) {
		  إذا (cur->test_قدر_callback_ready()) {
			إذا (cur->cb_معلومات.value) {
			      إذا (vpi_array_is_real(هذا)) {
				    مزدوج val = 0.0;
				    إذا (addr < vals->get_سائيز())
					  vals->get_لفظ(addr, val);
				    vpip_real_get_قدر(val, cur->cb_معلومات.value);
			      } وإلا إذا (vals4) {
				    vpip_vec4_get_قدر(vals4->get_لفظ(addr),
							vals_ويڪر,
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
				    إذا (addr < vals->get_سائيز())
					  vals->get_لفظ(addr, val);
				    vpip_vec4_get_قدر(val,
							vals_ويڪر,
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

		  vpi_callbacks = ايندڙ_2;
		  cur->ايندڙ_2 = 0;
		  ميٽيو cur;

	    } وإلا {
		  أكد(prev->ايندڙ_2 == cur);
		  prev->ايندڙ_2 = ايندڙ_2;
		  cur->ايندڙ_2 = 0;
		  ميٽيو cur;
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
      *array = array_ڳوليو(label());
      إذا (*array == 0) {
	    أكد(!mes);
	    أرجع خطأ;
      }
      أرجع سچ;
}

صنف array_پورٽ_resolv_list_t : عام resolv_list_s {

    عام:
      واضح array_پورٽ_resolv_list_t(حرف* lab, منطقي use_addr__,
                                        ڊگهو addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      منطقي use_addr;
      ڊگهو addr;
      منطقي resolve(منطقي mes) override;

    خاص:
};

array_پورٽ_resolv_list_t::array_پورٽ_resolv_list_t(حرف *lab, منطقي use_addr__,
                                                   ڊگهو addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      إذا (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      وإلا
            context_scope = 0;
      ptr = جديد vvp_net_t;
}

منطقي array_پورٽ_resolv_list_t::resolve(منطقي mes)
{
      vvp_array_t mem = array_ڳوليو(label());
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

      array_attach_پورٽ(mem, fun);

      أرجع سچ;
}

صنف array_لفظ_part_callback : عام array_لفظ_قدر_callback {
    عام:
      واضح array_لفظ_part_callback(p_cb_معلومات معلومات, ڊگهو addr);
      ~array_لفظ_part_callback() override;

      array_لفظ_part_callback(ثابت array_لفظ_part_callback&) = ميٽيو;
      array_لفظ_part_callback& عامل=(ثابت array_لفظ_part_callback&) = ميٽيو;

      منطقي test_قدر_callback_ready(فراغ) override;

    خاص:
      حرف*قدر_بٽ;
};

array_لفظ_part_callback::array_لفظ_part_callback(p_cb_معلومات معلومات, ڊگهو addr)
: array_لفظ_قدر_callback(معلومات, addr)
{
	// Get the initial value of the part, to use as a reference.
      بنية __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(معلومات->obj);
      s_vpi_قدر tmp_قدر;
      tmp_قدر.format = vpiBinStrVal;
      apvword->vpi_get_قدر(&tmp_قدر);

      قدر_بٽ = جديد حرف[apvword->part_wid+1];

      يادگيري_نقل(قدر_بٽ, tmp_قدر.value.str, apvword->part_wid);
      قدر_بٽ[apvword->part_wid] = 0;
}

array_لفظ_part_callback::~array_لفظ_part_callback()
{
      ميٽيو[]قدر_بٽ;
}

منطقي array_لفظ_part_callback::test_قدر_callback_ready(فراغ)
{
      بنية __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_معلومات.obj);
      أكد(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_قدر tmp_قدر;
      tmp_قدر.format = vpiBinStrVal;
      apvword->vpi_get_قدر(&tmp_قدر);

      إذا (memcmp(قدر_بٽ, tmp_قدر.value.str, apvword->part_wid) == 0)
	    أرجع خطأ;

      يادگيري_نقل(قدر_بٽ, tmp_قدر.value.str, apvword->part_wid);
      أرجع سچ;

}

قدر_callback*vpip_array_لفظ_change(p_cb_معلومات معلومات)
{
      بنية __vpiArray*والد = 0;
      array_لفظ_قدر_callback*cbh = 0;
      إذا (ثابت بنية __vpiArrayWord*لفظ_2 = array_var_لفظ_from_handle(معلومات->obj)) {
	    والد = static_cast<__vpiArray*>(لفظ_2->get_والد());
	    بنانشان addr = لفظ_2->get_اشاريو();
	    cbh = جديد array_لفظ_قدر_callback(معلومات, addr);

      } وإلا إذا (بنية __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(معلومات->obj)) {
	    والد = tword->array;
	    cbh = جديد array_لفظ_قدر_callback(معلومات, tword->پتو);

      } وإلا إذا (بنية __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(معلومات->obj)) {
	    والد = apvword->array;
	    cbh = جديد array_لفظ_part_callback(معلومات, apvword->لفظ_sel);
      }

      أكد(cbh);
      أكد(والد);
      cbh->ايندڙ_2 = والد->vpi_callbacks;
      والد->vpi_callbacks = cbh;

      أرجع cbh;
}

قدر_callback* vpip_array_change(p_cb_معلومات معلومات)
{
      array_لفظ_قدر_callback*cbh = جديد array_لفظ_قدر_callback(معلومات, -1);
      أكد(معلومات->obj);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(معلومات->obj);
      cbh->ايندڙ_2 = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      أرجع cbh;
}

فراغ compile_array_پورٽ(حرف*label, حرف*array, حرف*addr)
{
      array_پورٽ_resolv_list_t*resolv_mem
	    = جديد array_پورٽ_resolv_list_t(array, خطأ, 0);

      define_functor_نشان(label, resolv_mem->ptr);
      حرر(label);
	// Connect the port-0 input as the address.
      داخلا_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

فراغ compile_array_پورٽ(حرف*label, حرف*array, ڊگهو addr)
{
      array_پورٽ_resolv_list_t*resolv_mem
	    = جديد array_پورٽ_resolv_list_t(array, سچ, addr);

      define_functor_نشان(label, resolv_mem->ptr);
      حرر(label);

      resolv_submit(resolv_mem);
}

فراغ compile_array_alias(حرف*label, حرف*name, حرف*src)
{
      vvp_array_t mem = array_ڳوليو(src);
      أكد(mem);

      بنية __vpiArray*obj = جديد __vpiArray;

      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_ڳڻپ = mem->array_ڳڻپ;
      obj->signed_flag = mem->signed_flag;

	// Need to set an accurate range of addresses.
      obj->پهريون_addr = mem->پهريون_addr;
      obj->آخري_addr  = mem->آخري_addr;
      obj->مٽا_سٽا_addr = mem->مٽا_سٽا_addr;

      obj->msb = mem->msb;
      obj->lsb = mem->lsb;

	// Share the words with the source array.
      obj->nets = mem->nets;
      obj->vals4 = mem->vals4;
      obj->vals  = mem->vals;
      obj->vals_ويڪر = mem->vals_ويڪر;
      obj->vals_لفظ = mem->vals_لفظ;

      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

      أكد(array_جدول);
      أكد(!array_ڳوليو(label));
      array_جدول->sym_سيٽ_قدر(label, obj);

      compile_vpi_نشان(label, obj);
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
vpiHandle vpip_make_vthr_A(حرف*label, بنانشان addr)
{
      بنية __vpiArrayVthrA*obj = جديد __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = جديد array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->پتو_handle = 0;
      obj->پتو = addr;

      أرجع obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(حرف*label, حرف*نشان)
{
      بنية __vpiArrayVthrA*obj = جديد __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = جديد array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->پتو_handle = 0;
      compile_vpi_lookup(&obj->پتو_handle, نشان);
      obj->پتو = 0;

      أرجع obj;
}

vpiHandle vpip_make_vthr_A(حرف*label, vpiHandle handle)
{
      بنية __vpiArrayVthrA*obj = جديد __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = جديد array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->پتو_handle = handle;
      obj->پتو = 0;

      أرجع obj;
}

vpiHandle vpip_make_vthr_APV(حرف*label, بنانشان اشاريو, بنانشان ٻٽو_انگ, بنانشان wid)
{
      بنية __vpiArrayVthrAPV*obj = جديد __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = جديد array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->لفظ_sel = اشاريو;
      obj->part_ٻٽو_انگ = ٻٽو_انگ;
      obj->part_wid = wid;

      أرجع obj;
}

فراغ compile_array_cleanup(فراغ)
{
      ميٽيو array_جدول;
      array_جدول = 0;
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ يادگيري_delete(vpiHandle شي)
{
      بنية __vpiArray*arr = static_cast<بنية __vpiArray*>(شي);
      إذا (arr->vals_لفظ) ميٽيو [] (arr->vals_لفظ-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      ميٽيو arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      ميٽيو arr->vals;

      إذا (arr->nets) {
	    لكل (بنانشان idx = 0; idx < arr->get_سائيز(); idx += 1) {
		  إذا (بنية __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.اشاريو);
		    /* These should only be the real words. */
		  } وإلا {
			أكد(arr->nets[idx]->get_type_ڪوڊ() ==
			       vpiRealVar);
			بنية __vpiRealVar *sigr = static_cast<بنية __vpiRealVar *>
			                            (arr->nets[idx]);
			constant_delete(sigr->id.اشاريو);
// Why are only the real words still here?
			ميٽيو arr->nets[idx];
		  }
	    }
	    حرر(arr->nets);
      }

      طالما (arr->vpi_callbacks) {
	    بنية __vpiCallback*tmp = arr->vpi_callbacks->ايندڙ_2;
	    ميٽيو arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      ميٽيو arr;
}

فراغ A_delete(vpiHandle شي)
{
      بنية __vpiArrayVthrA*obj = static_cast<بنية __vpiArrayVthrA*>(شي);
      إذا (obj->پتو_handle) {
	    بدل (obj->پتو_handle->get_type_ڪوڊ()) {
		حالة vpi_يادگيري_لفظ:
		  إذا (vpi_get(_vpiFromThr, obj->پتو_handle) == _vpi_at_A) {
			A_delete(obj->پتو_handle);
		  }
		  اكسر;
		حالة vpiPartSelect:
		  أكد(vpi_get(_vpiFromThr, obj->پتو_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->پتو_handle);
		  اكسر;
	    }
      }

      ميٽيو obj;
}

فراغ APV_delete(vpiHandle شي)
{
      بنية __vpiArrayVthrAPV*obj = static_cast<بنية __vpiArrayVthrAPV*>(شي);
      ميٽيو obj;
}
#جيڪڏهن_پورو
