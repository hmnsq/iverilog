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
#اگر_تعریف_شده CHECK_WITH_VALGRIND
#ضمن  "vvp_cleanup.h"
#پایان_اگر
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <iostream>
# include  "compile.h"
# include  <cassert>
# include  "ivl_alloc.h"

استفاده نطاق std;

بیعلامت دراز شمار_net_arrays = 0;
بیعلامت دراز شمار_net_array_واژگان = 0;
بیعلامت دراز شمار_var_arrays = 0;
بیعلامت دراز شمار_var_array_واژگان = 0;
بیعلامت دراز شمار_real_arrays = 0;
بیعلامت دراز شمار_real_array_واژگان = 0;

ساكن نماد_map_s<بنية __vpiArray>* array_جدول =0;

صنف vvp_fun_arrayport;
ساكن فراغ array_attach_پورت(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_پیدا_کن(ثابت حرف*label)
{
      إذا (array_جدول == 0)
	    أرجع 0;

      vvp_array_t v = array_جدول->sym_get_مقدار(label);
      أرجع v;
}

بنية __vpiArrayVthrA : عام __vpiHandle {
      صحيح get_type_کد(فراغ) ثابت override { أرجع vpi_حافظه_کلمه; }
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      فراغ vpi_get_مقدار(p_vpi_مقدار val) override;
      vpiHandle vpi_put_مقدار(p_vpi_مقدار val, صحيح flags) override;
      vpiHandle vpi_handle(صحيح code) override;

      بنية __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle آدرس_handle;
	// If wid==0, then address is the address into the array.
      بیعلامت آدرس;

      بیعلامت get_آدرس() ثابت
      {
	    إذا (آدرس_handle) {
		  s_vpi_مقدار vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpi_وکتور_Val;
		  آدرس_handle->vpi_get_مقدار(&vp);
		  صحيح واژگان = (آدرس_handle->vpi_get(vpi_اندازه)-1)/32 + 1;
		  لكل(صحيح idx = 0; idx < واژگان; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			إذا (vp.value.vector[idx].bval != 0) أرجع UINT_MAX;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  آدرس_handle->vpi_get_مقدار(&vp);
		  أرجع vp.value.integer;
	    }

	    أرجع آدرس;
      }
};


بنية __vpiArrayVthrAPV : عام __vpiHandle {
      صحيح get_type_کد(فراغ) ثابت override { أرجع vpiPartSelect; }
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      فراغ vpi_get_مقدار(p_vpi_مقدار val) override;
      vpiHandle vpi_handle(صحيح code) override;

      بنية __vpiArray*array;
      بیعلامت کلمه_sel;
      بیعلامت part_بیت;
      بیعلامت part_wid;
};

منطقي is_net_array(vpiHandle obj)
{
      بنية __vpiArray*rfp = dynamic_cast<__vpiArray*> (obj);
      أكد(rfp);

      إذا (rfp->nets != 0) أرجع درست;
      أرجع خطأ;
}

// This function return true if the underlying array words are real.
ساكن منطقي vpi_array_is_real(ثابت vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      إذا (arr->vals4 != 0)  // A bit based variable/register array.
	    أرجع خطأ;

      إذا (dynamic_cast<vvp_darray_real*> (arr->vals))
	    أرجع درست;

      إذا (arr->vals != 0)
	    أرجع خطأ;

	// This must be a net array so look at element 0 to find the type.
      أكد(arr->nets != 0);
      أكد(arr->get_اندازه() > 0);
      ثابت بنية __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      إذا (rsig) {
	    أرجع درست;
      }

      أرجع خطأ;
}

ساكن منطقي vpi_array_is_string(ثابت vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      إذا (arr->vals4 != 0)  // A bit based variable/register array.
	    أرجع خطأ;

      إذا (dynamic_cast<vvp_darray_string*> (arr->vals))
	    أرجع درست;

      أرجع خطأ;
}

صحيح __vpiArray::get_کلمه_اندازه() ثابت
{
      بیعلامت width;

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
	    width = vals_عرض;
      }

      أرجع width;
}

حرف*__vpiArray::get_کلمه_str(بنية __vpiArrayWord*کلمه, صحيح code)
{
      بیعلامت شاخص = کلمه->get_شاخص();

      إذا (code == vpiFile) {  // Not implemented for now!
	    أرجع simple_مجموعه_rbuf_str(file_نامها[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%d", (صحيح)شاخص + اولی_addr.get_مقدار());
      أرجع generic_get_str(code, get_scope(), name, sidx);
}

فراغ __vpiArray::get_کلمه_مقدار(بنية __vpiArrayWord*کلمه, p_vpi_مقدار vp)
{
      بیعلامت شاخص = کلمه->get_شاخص();

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
          vpip_vec4_get_مقدار(vals4->get_کلمه(شاخص),
                              vals4->width(), signed_flag, vp);
      } وإلا إذا(vals) {
          بدل(vp->format) {
            حالة vpiBinStrVal:
            حالة vpiOctStrVal:
            حالة vpiDecStrVal:
            حالة vpiHexStrVal:
            حالة vpiScalarVal:
            حالة vpiIntVal:
            حالة vpi_وکتور_Val:
            {
                vvp_vector4_t v;
                vals->get_کلمه(شاخص, v);
                vpip_vec4_get_مقدار(v, vals_عرض, signed_flag, vp);
            }
            اكسر;

            حالة vpiRealVal:
            {
                مزدوج d;
                vals->get_کلمه(شاخص, d);
                vpip_real_get_مقدار(d, vp);
            }
            اكسر;

            حالة vpiStringVal:
            {
                string s;
                vals->get_کلمه(شاخص, s);
                vpip_string_get_مقدار(s, vp);
            }
            اكسر;

            افتراضي:
                fprintf(stderr, "vpi sorry: format is not implemented\n");
                أكد(خطأ);
          }
      }
}

فراغ __vpiArray::put_کلمه_مقدار(بنية __vpiArrayWord*کلمه, p_vpi_مقدار vp, صحيح)
{
      بیعلامت شاخص = کلمه->get_شاخص();
      vvp_vector4_t val = vec4_from_vpi_مقدار(vp, vals_عرض);
      مجموعه_کلمه(شاخص, 0, val);
}

vpiHandle __vpiArray::get_iter_شاخص(بنية __vpiArrayIterator*, صحيح idx)
{
      إذا (nets) أرجع nets[idx];

      أكد(vals4 || vals);

      إذا (vals_واژگان == 0) make_vals_واژگان();

      أرجع &(vals_واژگان[idx].as_کلمه);
}

صحيح __vpiArray::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_سطر_No:
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
            أرجع simple_مجموعه_rbuf_str(file_نامها[0]);
      }

      أرجع generic_get_str(code, scope, name, عدم);
}

vpiHandle __vpiArray::vpi_handle(صحيح code)
{
      بدل (code) {
          حالة vpi_چپ_Range:
            إذا (جابجا_addr) أرجع &آخرین_addr;
            وإلا أرجع &اولی_addr;

          حالة vpi_راست_Range:
            إذا (جابجا_addr) أرجع &اولی_addr;
            وإلا أرجع &آخرین_addr;

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
      شاخص -= اولی_addr.get_مقدار();
      إذا (شاخص >= (دراز) get_اندازه())
	    أرجع 0;
      إذا (شاخص < 0)
	    أرجع 0;

      إذا (nets != 0) {
	    أرجع nets[شاخص];
      }

      إذا (vals_واژگان == 0)
	    make_vals_واژگان();

      أرجع &(vals_واژگان[شاخص].as_کلمه);
}

صحيح __vpiArrayWord::as_کلمه_t::vpi_get(صحيح code)
{
      بنية __vpiArrayWord*obj = array_var_کلمه_from_handle(هذا);
      أكد(obj);
      بنية __vpiArrayBase*my_والد = obj->get_والد();
      t_vpi_مقدار val;

      بدل (code) {
	  حالة vpi_سطر_No:
	    أرجع 0; // Not implemented for now!

	  حالة vpi_اندازه:
	    أرجع my_والد->get_کلمه_اندازه();

	  حالة vpi_چپ_Range:
            val.format = vpiIntVal;
	    my_والد->get_چپ_range()->vpi_get_مقدار(&val);
            أكد(val.format == vpiIntVal);
            أرجع val.value.integer;

	  حالة vpi_راست_Range:
            val.format = vpiIntVal;
	    my_والد->get_راست_range()->vpi_get_مقدار(&val);
            أكد(val.format == vpiIntVal);
            أرجع val.value.integer;

	  حالة vpi_شاخص_2:
	    {
		  صحيح پایه_جابه_جایی = 0;
		  بنية __vpiArray*پایه = dynamic_cast<__vpiArray*> (my_والد);
		  إذا (پایه) {
			val.format = vpiIntVal;
			پایه->اولی_addr.vpi_get_مقدار(&val);
			پایه_جابه_جایی += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_شاخص.vpi_get_مقدار(&val);
		  أكد(val.format == vpiIntVal);
		  أرجع val.value.integer + پایه_جابه_جایی;
	    }

	  حالة vpiAutomatic:
	    أرجع my_والد->get_scope()->is_automatic()? 1 : 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    أرجع _vpiNoThr;
#پایان_اگر

	  افتراضي:
	    أرجع 0;
      }
}


صحيح __vpiArrayVthrA::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_سطر_No:
	    أرجع 0; // Not implemented for now!

	  حالة vpi_اندازه:
	    أرجع array->get_کلمه_اندازه();

	  حالة vpi_چپ_Range:
	    أرجع array->msb.get_مقدار();

	  حالة vpi_راست_Range:
	    أرجع array->lsb.get_مقدار();

	  حالة vpi_شاخص_2:
	    أرجع (صحيح)get_آدرس() + array->اولی_addr.get_مقدار();

	  حالة vpiAutomatic:
	    أرجع array->get_scope()->is_automatic() ? 1 : 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    أرجع _vpi_at_A;
#پایان_اگر

	  // If address_handle is not zero we definitely have a
	  // variable.
	  حالة vpiConstantSelect:
	    أرجع آدرس_handle == 0;

	  افتراضي:
	    أرجع 0;
      }
}

حرف* __vpiArrayVthrA::vpi_get_str(صحيح code)
{
      إذا (code == vpiFile) {  // Not implemented for now!
            أرجع simple_مجموعه_rbuf_str(file_نامها[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%d", (صحيح)get_آدرس() + array->اولی_addr.get_مقدار());
      أرجع generic_get_str(code, array->get_scope(), array->name, sidx);
}

فراغ __vpiArrayVthrA::vpi_get_مقدار(p_vpi_مقدار vp)
{
      أكد(array);

      بیعلامت شاخص = get_آدرس();
      إذا (vpi_array_is_real(array)) {
	    مزدوج tmp = array->get_کلمه_r(شاخص);
	    vpip_real_get_مقدار(tmp, vp);
      } وإلا إذا (vpi_array_is_string(array)) {
	    string tmp = array->get_کلمه_str(شاخص);
	    vpip_string_get_مقدار(tmp, vp);
      } وإلا {
	    vvp_vector4_t tmp = array->get_کلمه(شاخص);
	    بیعلامت width = array->get_کلمه_اندازه();
	    vpip_vec4_get_مقدار(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_مقدار(p_vpi_مقدار vp, صحيح)
{
      بیعلامت شاخص = get_آدرس();

      أكد(array);
      أكد(شاخص < array->get_اندازه());

      إذا (vpi_array_is_real(array)) {
	    مزدوج val = real_from_vpi_مقدار(vp);
	    array->مجموعه_کلمه(شاخص, val);
      } وإلا {
	    بیعلامت width = array->get_کلمه_اندازه();
	    vvp_vector4_t val = vec4_from_vpi_مقدار(vp, width);
	    array->مجموعه_کلمه(شاخص, 0, val);
      }

      أرجع هذا;
}

vpiHandle __vpiArrayVthrA::vpi_handle(صحيح code)
{
      بدل (code) {
	  حالة vpi_شاخص_2:
	    اكسر;  // Not implemented!

	  حالة vpi_چپ_Range:
	    أرجع &array->msb;

	  حالة vpi_راست_Range:
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
	  حالة vpi_سطر_No:
	    أرجع 0; // Not implemented for now!

	  حالة vpi_اندازه:
	    أرجع part_wid;

	  حالة vpi_چپ_Range:
	    أرجع part_بیت + part_wid - 1;

	  حالة vpi_راست_Range:
	    أرجع part_بیت;

	  حالة vpiAutomatic:
	    أرجع array->get_scope()->is_automatic() ? 1 : 0;

#إذا defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالة _vpiFromThr:
	    أرجع _vpi_at_APV;
#پایان_اگر

	  حالة vpiConstantSelect:
	    أرجع 1;

	  افتراضي:
	    أرجع 0;
      }
}

حرف* __vpiArrayVthrAPV::vpi_get_str(صحيح code)
{
      إذا (code == vpiFile) {  // Not implemented for now!
            أرجع simple_مجموعه_rbuf_str(file_نامها[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%u", کلمه_sel + array->اولی_addr.get_مقدار());
      أرجع generic_get_str(code, array->get_scope(), array->name, sidx);
}

فراغ __vpiArrayVthrAPV::vpi_get_مقدار(p_vpi_مقدار vp)
{
      أكد(array);

      بیعلامت شاخص = کلمه_sel;
      إذا (vpi_array_is_real(array)) {
	    مزدوج tmp = array->get_کلمه_r(شاخص);
	    vpip_real_get_مقدار(tmp, vp);
      } وإلا {
	    vvp_vector4_t tmp = array->get_کلمه(شاخص);
	    tmp = tmp.subvalue(part_بیت, part_wid);
	    vpip_vec4_get_مقدار(tmp, part_wid, خطأ, vp);
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

فراغ __vpiArray::مجموعه_کلمه(بیعلامت آدرس, بیعلامت part_off, ثابت vvp_vector4_t&val)
{
      إذا (آدرس >= get_اندازه())
	    أرجع;

      إذا (vals4) {
	    أكد(nets == 0);
	    إذا (part_off != 0 || val.size() != vals_عرض) {
		  vvp_vector4_t tmp = vals4->get_کلمه(آدرس);
		  إذا ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_عرض << پایان_سطر;
			أكد(0);
		  }
		  tmp.مجموعه_vec(part_off, val);
		  vals4->مجموعه_کلمه(آدرس, tmp);
	    } وإلا {
		  vals4->مجموعه_کلمه(آدرس, val);
	    }
	    کلمه_change(آدرس);
	    أرجع;
      }

      إذا (vals) {
	    أكد(nets == 0);
	    إذا (part_off != 0 || val.size() != vals_عرض) {
		  vvp_vector4_t tmp;
		  vals->get_کلمه(آدرس, tmp);
		  إذا ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_عرض << پایان_سطر;
			أكد(0);
		  }
		  tmp.مجموعه_vec(part_off, val);
		  vals->مجموعه_کلمه(آدرس, tmp);
	    } وإلا {
		  vals->مجموعه_کلمه(آدرس, val);
	    }
	    کلمه_change(آدرس);
	    أرجع;
      }

      أكد(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle کلمه = nets[آدرس];
      ثابت بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(کلمه);
      أكد(vsig);

      vsig->گره->send_vec4_pv(val, part_off, vpip_اندازه(vsig), 0);
      کلمه_change(آدرس);
}

فراغ __vpiArray::مجموعه_کلمه(بیعلامت آدرس, مزدوج val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      إذا (آدرس >= vals->get_اندازه())
	    أرجع;

      vals->مجموعه_کلمه(آدرس, val);
      کلمه_change(آدرس);
}

فراغ __vpiArray::مجموعه_کلمه(بیعلامت آدرس, ثابت string&val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      إذا (آدرس >= vals->get_اندازه())
	    أرجع;

      vals->مجموعه_کلمه(آدرس, val);
      کلمه_change(آدرس);
}

فراغ __vpiArray::مجموعه_کلمه(بیعلامت آدرس, ثابت vvp_object_t&val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      إذا (آدرس >= vals->get_اندازه())
	    أرجع;

      vals->مجموعه_کلمه(آدرس, val);
      کلمه_change(آدرس);
}

vvp_vector4_t __vpiArray::get_کلمه(بیعلامت آدرس)
{
      إذا (vals4) {
	    أكد(nets == 0);
	    أكد(vals == 0);
	    أرجع vals4->get_کلمه(آدرس);
      }

      إذا (vals) {
	    أكد(nets == 0);
	    أكد(vals4== 0);
	    إذا (آدرس >= vals->get_اندازه())
		  أرجع vvp_vector4_t(vals_عرض, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_کلمه(آدرس, val);
	    أرجع val;
      }

      أكد(vals4 == 0);
      أكد(vals == 0);
      أكد(nets != 0);

      إذا (آدرس >= get_اندازه()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    أكد(get_اندازه() > 0);
	    vpiHandle کلمه = nets[0];
	    أكد(کلمه);
	    بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(کلمه);
	    أكد(vsig);
	    vvp_signal_مقدار*sig = dynamic_cast<vvp_signal_مقدار*> (vsig->گره->fil);
	    أكد(sig);
	    أرجع vvp_vector4_t(sig->مقدار_اندازه(), BIT4_X);
      }

      vpiHandle کلمه = nets[آدرس];
      بنية __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(کلمه);
      أكد(vsig);
      vvp_signal_مقدار*sig = dynamic_cast<vvp_signal_مقدار*> (vsig->گره->fil);
      أكد(sig);

      vvp_vector4_t val;
      sig->vec4_مقدار(val);
      أرجع val;
}

مزدوج __vpiArray::get_کلمه_r(بیعلامت آدرس)
{
      إذا (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    إذا (آدرس >= vals->get_اندازه())
		  أرجع 0.0;

	    مزدوج val;
	    vals->get_کلمه(آدرس, val);
	    أرجع val;
      }

      إذا (آدرس >= get_اندازه())
	    أرجع 0.0;

      أكد(nets);
      vpiHandle کلمه = nets[آدرس];
      بنية __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(کلمه);
      أكد(vsig);
      vvp_signal_مقدار*sig = dynamic_cast<vvp_signal_مقدار*> (vsig->net->fil);
      أكد(sig);

      مزدوج val = sig->real_مقدار();
      أرجع val;
}

فراغ __vpiArray::get_کلمه_obj(بیعلامت آدرس, vvp_object_t&val)
{
      إذا (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    إذا (آدرس >= vals->get_اندازه()) {
		  val = vvp_object_t();
		  أرجع;
	    }

	    vals->get_کلمه(آدرس, val);
	    أرجع;
      }

      أكد(nets);
	// Arrays of string nets not implemented!
      أكد(0);
      أرجع;
}

string __vpiArray::get_کلمه_str(بیعلامت آدرس)
{
      إذا (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    إذا (آدرس >= vals->get_اندازه())
		  أرجع "";

	    string val;
	    vals->get_کلمه(آدرس, val);
	    أرجع val;
      }

      أكد(nets);
	// Arrays of string nets not implemented!
      أكد(0);
      أرجع "";
}

vpiHandle vpip_make_array(ثابت حرف*label, ثابت حرف*name,
                          صحيح اولی_addr, صحيح آخرین_addr,
                          منطقي signed_flag)
{
      بنية __vpiArray*obj = جديد __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      إذا (آخرین_addr >= اولی_addr) {
	    obj->جابجا_addr = خطأ;
      } وإلا {
	    obj->جابجا_addr = درست;
	    صحيح tmp = آخرین_addr;
	    آخرین_addr = اولی_addr;
	    اولی_addr = tmp;
      }
      أكد(آخرین_addr >= اولی_addr);
      بیعلامت array_شمار = آخرین_addr+1-اولی_addr;

	// For now, treat all arrays as memories. This is not quite
	// correct, as arrays are arrays with memories a special case.
      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_شمار = array_شمار;

      obj->اولی_addr.مجموعه_مقدار(اولی_addr);
      obj->آخرین_addr .مجموعه_مقدار(آخرین_addr);

	// Start off now knowing if we are nets or variables.
      obj->nets = 0;
      obj->vals4 = 0;
      obj->vals  = 0;
      obj->vals_عرض = 0;
      obj->vals_واژگان = 0;

	// Initialize (clear) the read-ports list.
      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

	/* Add this symbol to the array_symbols table for later lookup. */
      إذا (!array_جدول)
	    array_جدول = جديد نماد_map_s<بنية __vpiArray>;

      أكد(!array_پیدا_کن(label));
      array_جدول->sym_مجموعه_مقدار(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_نماد(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_مقصد_current_scope(obj);

      أرجع obj;
}

فراغ __vpiArray::alias_کلمه(بیعلامت دراز addr, vpiHandle کلمه, صحيح msb_, صحيح lsb_)
{
      أكد(msb.get_مقدار() == msb_);
      أكد(lsb.get_مقدار() == lsb_);
      أكد(addr < get_اندازه());
      أكد(nets);
      nets[addr] = کلمه;
}

فراغ __vpiArray::attach_کلمه(بیعلامت addr, vpiHandle کلمه)
{
      أكد(addr < get_اندازه());
      أكد(nets);
      nets[addr] = کلمه;

      إذا (بنية __vpiSignal*sig = dynamic_cast<__vpiSignal*>(کلمه)) {
	    vvp_net_t*net = sig->گره;
	    أكد(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    أكد(fun);
	    fun->attach_as_کلمه(هذا, addr);
	    sig->is_netarray = 1;
	    sig->within.والد = هذا;
	    sig->id.شاخص = جديد __vpiDecConst(addr + اولی_addr.get_مقدار());
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
	    sig->within.والد = هذا;
	    sig->id.شاخص = جديد __vpiDecConst(addr + اولی_addr.get_مقدار());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = درست;
	    أرجع;
      }
}

فراغ compile_var_array(حرف*label, حرف*name, صحيح آخرین, صحيح اولی,
		   صحيح msb, صحيح lsb, حرف signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, اولی, آخرین,
                                      signed_flag != 0);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_عرض = labs(msb-lsb) + 1;
      إذا (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = جديد vvp_vector4array_aa(arr->vals_عرض,
						 arr->get_اندازه());
      } وإلا {
            arr->vals4 = جديد vvp_vector4array_sa(arr->vals_عرض,
						 arr->get_اندازه());
      }
      arr->msb.مجموعه_مقدار(msb);
      arr->lsb.مجموعه_مقدار(lsb);

      شمار_var_arrays += 1;
      شمار_var_array_واژگان += arr->get_اندازه();

      حرر(label);
      حذف[] name;
}

فراغ compile_var2_array(حرف*label, حرف*name, صحيح آخرین, صحيح اولی,
		   صحيح msb, صحيح lsb, منطقي signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, اولی, آخرین, signed_flag);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.مجموعه_مقدار(msb);
      arr->lsb.مجموعه_مقدار(lsb);
      arr->vals_عرض = labs(msb-lsb) + 1;

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
	    arr->vals = جديد vvp_darray_vec2(arr->get_اندازه(), arr->vals_عرض);
      }
      شمار_var_arrays += 1;
      شمار_var_array_واژگان += arr->get_اندازه();

      حرر(label);
      حذف[] name;
}

فراغ compile_real_array(حرف*label, حرف*name, صحيح آخرین, صحيح اولی)
{
      vpiHandle obj = vpip_make_array(label, name, اولی, آخرین, درست);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = جديد vvp_darray_real(arr->get_اندازه());
      arr->vals_عرض = 1;

      شمار_real_arrays += 1;
      شمار_real_array_واژگان += arr->get_اندازه();

      حرر(label);
      حذف[] name;
}

فراغ compile_string_array(حرف*label, حرف*name, صحيح آخرین, صحيح اولی)
{
      vpiHandle obj = vpip_make_array(label, name, اولی, آخرین, درست);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = جديد vvp_darray_string(arr->get_اندازه());
      arr->vals_عرض = 1;

      شمار_real_arrays += 1;
      شمار_real_array_واژگان += arr->get_اندازه();

      حرر(label);
      حذف[] name;
}

فراغ compile_object_array(حرف*label, حرف*name, صحيح آخرین, صحيح اولی)
{
      vpiHandle obj = vpip_make_array(label, name, اولی, آخرین, درست);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = جديد vvp_darray_object(arr->get_اندازه());
      arr->vals_عرض = 1;

      شمار_real_arrays += 1;
      شمار_real_array_واژگان += arr->get_اندازه();

      حرر(label);
      حذف[] name;
}

فراغ compile_net_array(حرف*label, حرف*name, صحيح آخرین, صحيح اولی)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, اولی, آخرین, خطأ);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(حافظه_صفر_بده(arr->get_اندازه(), حجم(vpiHandle)));

      شمار_net_arrays += 1;
      شمار_net_array_واژگان += arr->get_اندازه();

      حرر(label);
      حذف[] name;
}

صنف vvp_fun_arrayport  : عام vvp_net_fun_t {

    عام:
      صریح vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      صریح vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, دراز addr);
      ~vvp_fun_arrayport() override;

      مجازی فراغ check_کلمه_change(بیعلامت دراز addr) = 0;

    محمي:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      بیعلامت دراز addr_;

      دوست فراغ array_attach_پورت(vvp_array_t, vvp_fun_arrayport*);
      دوست فراغ __vpiArray::کلمه_change(بیعلامت دراز);
      vvp_fun_arrayport*بعدی;
};

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net)
: arr_(mem), net_(net), addr_(0)
{
      بعدی = 0;
}

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, دراز addr)
: arr_(mem), net_(net), addr_(addr)
{
      بعدی = 0;
}

vvp_fun_arrayport::~vvp_fun_arrayport()
{
}

صنف vvp_fun_arrayport_sa  : عام vvp_fun_arrayport {

    عام:
      صریح vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      صریح vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, دراز addr);
      ~vvp_fun_arrayport_sa() override;

      فراغ check_کلمه_change(بیعلامت دراز addr) override;

      فراغ recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                     vvp_context_t) override;

    خاص:
};

vvp_fun_arrayport_sa::vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net)
{
}

vvp_fun_arrayport_sa::vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, دراز addr)
: vvp_fun_arrayport(mem, net, addr)
{
}

vvp_fun_arrayport_sa::~vvp_fun_arrayport_sa()
{
}

فراغ vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                                     vvp_context_t)
{
      منطقي addr_valid_flag;

      بدل (پورت.پورت()) {

	  حالة 0: // Address input
	    addr_valid_flag = vector4_مقصد_مقدار(بیت, addr_);
	    إذا (! addr_valid_flag)
		  addr_ = arr_->get_اندازه();
	    إذا (vpi_array_is_real(arr_))
		  پورت.ptr()->send_real(arr_->get_کلمه_r(addr_), 0);
	    وإلا
		  پورت.ptr()->send_vec4(arr_->get_کلمه(addr_), 0);

	    اكسر;

	  افتراضي:
	    fprintf(stdout, "XXXX write ports not implemented.\n");
	    أكد(0);
      }
}

فراغ vvp_fun_arrayport_sa::check_کلمه_change(بیعلامت دراز addr)
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
      صریح vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net);
      صریح vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, دراز addr);
      ~vvp_fun_arrayport_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#اگر_تعریف_شده CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#پایان_اگر

      فراغ check_کلمه_change(بیعلامت دراز addr) override;

      فراغ recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                     vvp_context_t context) override;

    خاص:
      فراغ check_کلمه_change_2(بیعلامت دراز addr, vvp_context_t context);

      __vpiScope*context_scope_;
      بیعلامت context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_جمع_کن_قلم_مقصد_context(هذا, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, دراز addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_جمع_کن_قلم_مقصد_context(هذا, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

فراغ vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      بیعلامت دراز*addr = جديد بیعلامت دراز;
      vvp_مجموعه_context_قلم(context, context_idx_, addr);

      *addr = addr_;
}

فراغ vvp_fun_arrayport_aa::reset_instance(vvp_context_t context)
{
      بیعلامت دراز*addr = static_cast<بیعلامت دراز*>
            (vvp_get_context_قلم(context, context_idx_));

      *addr = addr_;
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
فراغ vvp_fun_arrayport_aa::free_instance(vvp_context_t context)
{
      بیعلامت دراز*addr = static_cast<بیعلامت دراز*>
            (vvp_get_context_قلم(context, context_idx_));
      حذف addr;
}
#پایان_اگر

فراغ vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t پورت, ثابت vvp_vector4_t&بیت,
                                     vvp_context_t context)
{
      إذا (context) {
            بیعلامت دراز*addr = static_cast<بیعلامت دراز*>
                  (vvp_get_context_قلم(context, context_idx_));

            منطقي addr_valid_flag;

            بدل (پورت.پورت()) {

                حالة 0: // Address input
                  addr_valid_flag = vector4_مقصد_مقدار(بیت, *addr);
                  إذا (! addr_valid_flag) *addr = arr_->get_اندازه();
                  إذا (vpi_array_is_real(arr_)) {
			پورت.ptr()->send_real(arr_->get_کلمه_r(*addr),
					      context);
                  } وإلا {
			پورت.ptr()->send_vec4(arr_->get_کلمه(*addr),
					      context);
                  }
                  اكسر;

                افتراضي:
                  fprintf(stdout, "XXXX write ports not implemented.\n");
                  أكد(0);
            }
      } وإلا {
            context = context_scope_->زنده_contexts;
            طالما (context) {
                  recv_vec4(پورت, بیت, context);
                  context = vvp_get_بعدی_context(context);
            }
      }
}

فراغ vvp_fun_arrayport_aa::check_کلمه_change_2(بیعلامت دراز addr,
                                              vvp_context_t context)
{
      ثابت بیعلامت دراز*پورت_addr = static_cast<بیعلامت دراز*>
            (vvp_get_context_قلم(context, context_idx_));

      إذا (addr != *پورت_addr)
	    أرجع;

      إذا (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_کلمه_r(addr), context);
      } وإلا {
	    net_->send_vec4(arr_->get_کلمه(addr), context);
      }
}

فراغ vvp_fun_arrayport_aa::check_کلمه_change(بیعلامت دراز addr)
{
      إذا (arr_->get_scope()->is_automatic()) {
            أكد(vthread_get_wt_context());
            check_کلمه_change_2(addr, vthread_get_wt_context());
      } وإلا {
            vvp_context_t context = context_scope_->زنده_contexts;
            طالما (context) {
                  check_کلمه_change_2(addr, context);
                  context = vvp_get_بعدی_context(context);
            }
      }
}

ساكن فراغ array_attach_پورت(vvp_array_t array, vvp_fun_arrayport*fun)
{
      أكد(fun->بعدی == 0);
      fun->بعدی = array->ports_;
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
                  vvp_vector4_t tmp(array->vals_عرض, init);
                  زمانبندی_init_propagate(fun->net_, tmp);
            } وإلا {
                  زمانبندی_init_propagate(fun->net_, 0.0);
            }
      }
}

صنف array_کلمه_مقدار_callback : عام مقدار_callback {
    عام:
      مضمن صریح array_کلمه_مقدار_callback(p_cb_داده داده, دراز addr)
      : مقدار_callback(داده), کلمه_addr(addr)
      { }

    عام:
      دراز کلمه_addr;
};

فراغ __vpiArray::کلمه_change(بیعلامت دراز addr)
{
      لكل (vvp_fun_arrayport*cur = ports_; cur; cur = cur->بعدی)
	    cur->check_کلمه_change(addr);

	// Run callbacks attached to the array itself.
      بنية __vpiCallback *بعدی_2 = vpi_callbacks;
      بنية __vpiCallback *prev = 0;

      طالما (بعدی_2) {
	    array_کلمه_مقدار_callback*cur = dynamic_cast<array_کلمه_مقدار_callback*>(بعدی_2);
	    بعدی_2 = cur->بعدی_2;

	      // Skip callbacks that are not for me. -1 is for every element.
	    إذا (cur->کلمه_addr != (دراز)addr && cur->کلمه_addr != -1) {
		  prev = cur;
		  استمر;
	    }

	      // For whole array callbacks we need to set the index.
	    إذا (cur->کلمه_addr == -1) {
		  cur->cb_داده.شاخص = (PLI_INT32) ((صحيح)addr +
						    اولی_addr.get_مقدار());
	    }

	    إذا (cur->cb_داده.cb_rtn != 0) {
		  إذا (cur->test_مقدار_callback_ready()) {
			إذا (cur->cb_داده.value) {
			      إذا (vpi_array_is_real(هذا)) {
				    مزدوج val = 0.0;
				    إذا (addr < vals->get_اندازه())
					  vals->get_کلمه(addr, val);
				    vpip_real_get_مقدار(val, cur->cb_داده.value);
			      } وإلا إذا (vals4) {
				    vpip_vec4_get_مقدار(vals4->get_کلمه(addr),
							vals_عرض,
							signed_flag,
							cur->cb_داده.value);
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
				    vpip_vec4_get_مقدار(val,
							vals_عرض,
							signed_flag,
							cur->cb_داده.value);
			      } وإلا {
			            أكد(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } وإلا إذا (prev == 0) {

		  vpi_callbacks = بعدی_2;
		  cur->بعدی_2 = 0;
		  حذف cur;

	    } وإلا {
		  أكد(prev->بعدی_2 == cur);
		  prev->بعدی_2 = بعدی_2;
		  cur->بعدی_2 = 0;
		  حذف cur;
	    }
      }
}

صنف array_resolv_list_t : عام resolv_list_s {

    عام:
      صریح array_resolv_list_t(حرف*lab) : resolv_list_s(lab) {
	    array = 0;
      }

      vvp_array_t*array;
      منطقي resolve(منطقي mes) override;

    خاص:
};

منطقي array_resolv_list_t::resolve(منطقي mes)
{
      *array = array_پیدا_کن(label());
      إذا (*array == 0) {
	    أكد(!mes);
	    أرجع خطأ;
      }
      أرجع درست;
}

صنف array_پورت_resolv_list_t : عام resolv_list_s {

    عام:
      صریح array_پورت_resolv_list_t(حرف* lab, منطقي use_addr__,
                                        دراز addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      منطقي use_addr;
      دراز addr;
      منطقي resolve(منطقي mes) override;

    خاص:
};

array_پورت_resolv_list_t::array_پورت_resolv_list_t(حرف *lab, منطقي use_addr__,
                                                   دراز addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      إذا (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      وإلا
            context_scope = 0;
      ptr = جديد vvp_net_t;
}

منطقي array_پورت_resolv_list_t::resolve(منطقي mes)
{
      vvp_array_t mem = array_پیدا_کن(label());
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

      array_attach_پورت(mem, fun);

      أرجع درست;
}

صنف array_کلمه_part_callback : عام array_کلمه_مقدار_callback {
    عام:
      صریح array_کلمه_part_callback(p_cb_داده داده, دراز addr);
      ~array_کلمه_part_callback() override;

      array_کلمه_part_callback(ثابت array_کلمه_part_callback&) = حذف;
      array_کلمه_part_callback& عملگر=(ثابت array_کلمه_part_callback&) = حذف;

      منطقي test_مقدار_callback_ready(فراغ) override;

    خاص:
      حرف*مقدار_بیتها;
};

array_کلمه_part_callback::array_کلمه_part_callback(p_cb_داده داده, دراز addr)
: array_کلمه_مقدار_callback(داده, addr)
{
	// Get the initial value of the part, to use as a reference.
      بنية __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(داده->obj);
      s_vpi_مقدار tmp_مقدار;
      tmp_مقدار.format = vpiBinStrVal;
      apvword->vpi_get_مقدار(&tmp_مقدار);

      مقدار_بیتها = جديد حرف[apvword->part_wid+1];

      کپی_حافظه(مقدار_بیتها, tmp_مقدار.value.str, apvword->part_wid);
      مقدار_بیتها[apvword->part_wid] = 0;
}

array_کلمه_part_callback::~array_کلمه_part_callback()
{
      حذف[]مقدار_بیتها;
}

منطقي array_کلمه_part_callback::test_مقدار_callback_ready(فراغ)
{
      بنية __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_داده.obj);
      أكد(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_مقدار tmp_مقدار;
      tmp_مقدار.format = vpiBinStrVal;
      apvword->vpi_get_مقدار(&tmp_مقدار);

      إذا (memcmp(مقدار_بیتها, tmp_مقدار.value.str, apvword->part_wid) == 0)
	    أرجع خطأ;

      کپی_حافظه(مقدار_بیتها, tmp_مقدار.value.str, apvword->part_wid);
      أرجع درست;

}

مقدار_callback*vpip_array_کلمه_change(p_cb_داده داده)
{
      بنية __vpiArray*والد = 0;
      array_کلمه_مقدار_callback*cbh = 0;
      إذا (ثابت بنية __vpiArrayWord*کلمه = array_var_کلمه_from_handle(داده->obj)) {
	    والد = static_cast<__vpiArray*>(کلمه->get_والد());
	    بیعلامت addr = کلمه->get_شاخص();
	    cbh = جديد array_کلمه_مقدار_callback(داده, addr);

      } وإلا إذا (بنية __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(داده->obj)) {
	    والد = tword->array;
	    cbh = جديد array_کلمه_مقدار_callback(داده, tword->آدرس);

      } وإلا إذا (بنية __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(داده->obj)) {
	    والد = apvword->array;
	    cbh = جديد array_کلمه_part_callback(داده, apvword->کلمه_sel);
      }

      أكد(cbh);
      أكد(والد);
      cbh->بعدی_2 = والد->vpi_callbacks;
      والد->vpi_callbacks = cbh;

      أرجع cbh;
}

مقدار_callback* vpip_array_change(p_cb_داده داده)
{
      array_کلمه_مقدار_callback*cbh = جديد array_کلمه_مقدار_callback(داده, -1);
      أكد(داده->obj);

      بنية __vpiArray*arr = dynamic_cast<__vpiArray*>(داده->obj);
      cbh->بعدی_2 = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      أرجع cbh;
}

فراغ compile_array_پورت(حرف*label, حرف*array, حرف*addr)
{
      array_پورت_resolv_list_t*resolv_mem
	    = جديد array_پورت_resolv_list_t(array, خطأ, 0);

      define_functor_نماد(label, resolv_mem->ptr);
      حرر(label);
	// Connect the port-0 input as the address.
      درون_داد_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

فراغ compile_array_پورت(حرف*label, حرف*array, دراز addr)
{
      array_پورت_resolv_list_t*resolv_mem
	    = جديد array_پورت_resolv_list_t(array, درست, addr);

      define_functor_نماد(label, resolv_mem->ptr);
      حرر(label);

      resolv_submit(resolv_mem);
}

فراغ compile_array_alias(حرف*label, حرف*name, حرف*src)
{
      vvp_array_t mem = array_پیدا_کن(src);
      أكد(mem);

      بنية __vpiArray*obj = جديد __vpiArray;

      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_شمار = mem->array_شمار;
      obj->signed_flag = mem->signed_flag;

	// Need to set an accurate range of addresses.
      obj->اولی_addr = mem->اولی_addr;
      obj->آخرین_addr  = mem->آخرین_addr;
      obj->جابجا_addr = mem->جابجا_addr;

      obj->msb = mem->msb;
      obj->lsb = mem->lsb;

	// Share the words with the source array.
      obj->nets = mem->nets;
      obj->vals4 = mem->vals4;
      obj->vals  = mem->vals;
      obj->vals_عرض = mem->vals_عرض;
      obj->vals_واژگان = mem->vals_واژگان;

      obj->ports_ = 0;
      obj->vpi_callbacks = 0;

      أكد(array_جدول);
      أكد(!array_پیدا_کن(label));
      array_جدول->sym_مجموعه_مقدار(label, obj);

      compile_vpi_نماد(label, obj);
      vpip_attach_مقصد_current_scope(obj);

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
vpiHandle vpip_make_vthr_A(حرف*label, بیعلامت addr)
{
      بنية __vpiArrayVthrA*obj = جديد __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = جديد array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->آدرس_handle = 0;
      obj->آدرس = addr;

      أرجع obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(حرف*label, حرف*نماد)
{
      بنية __vpiArrayVthrA*obj = جديد __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = جديد array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->آدرس_handle = 0;
      compile_vpi_lookup(&obj->آدرس_handle, نماد);
      obj->آدرس = 0;

      أرجع obj;
}

vpiHandle vpip_make_vthr_A(حرف*label, vpiHandle handle)
{
      بنية __vpiArrayVthrA*obj = جديد __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = جديد array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->آدرس_handle = handle;
      obj->آدرس = 0;

      أرجع obj;
}

vpiHandle vpip_make_vthr_APV(حرف*label, بیعلامت شاخص, بیعلامت بیت, بیعلامت wid)
{
      بنية __vpiArrayVthrAPV*obj = جديد __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = جديد array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->کلمه_sel = شاخص;
      obj->part_بیت = بیت;
      obj->part_wid = wid;

      أرجع obj;
}

فراغ compile_array_cleanup(فراغ)
{
      حذف array_جدول;
      array_جدول = 0;
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
فراغ حافظه_delete(vpiHandle قلم)
{
      بنية __vpiArray*arr = static_cast<بنية __vpiArray*>(قلم);
      إذا (arr->vals_واژگان) حذف [] (arr->vals_واژگان-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      حذف arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      حذف arr->vals;

      إذا (arr->nets) {
	    لكل (بیعلامت idx = 0; idx < arr->get_اندازه(); idx += 1) {
		  إذا (بنية __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.شاخص);
		    /* These should only be the real words. */
		  } وإلا {
			أكد(arr->nets[idx]->get_type_کد() ==
			       vpiRealVar);
			بنية __vpiRealVar *sigr = static_cast<بنية __vpiRealVar *>
			                            (arr->nets[idx]);
			constant_delete(sigr->id.شاخص);
// Why are only the real words still here?
			حذف arr->nets[idx];
		  }
	    }
	    حرر(arr->nets);
      }

      طالما (arr->vpi_callbacks) {
	    بنية __vpiCallback*tmp = arr->vpi_callbacks->بعدی_2;
	    حذف arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      حذف arr;
}

فراغ A_delete(vpiHandle قلم)
{
      بنية __vpiArrayVthrA*obj = static_cast<بنية __vpiArrayVthrA*>(قلم);
      إذا (obj->آدرس_handle) {
	    بدل (obj->آدرس_handle->get_type_کد()) {
		حالة vpi_حافظه_کلمه:
		  إذا (vpi_get(_vpiFromThr, obj->آدرس_handle) == _vpi_at_A) {
			A_delete(obj->آدرس_handle);
		  }
		  اكسر;
		حالة vpiPartSelect:
		  أكد(vpi_get(_vpiFromThr, obj->آدرس_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->آدرس_handle);
		  اكسر;
	    }
      }

      حذف obj;
}

فراغ APV_delete(vpiHandle قلم)
{
      بنية __vpiArrayVthrAPV*obj = static_cast<بنية __vpiArrayVthrAPV*>(قلم);
      حذف obj;
}
#پایان_اگر
