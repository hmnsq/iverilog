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
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
#شامل  "vvp_cleanup.h"
#اگر_ختم
# include  <cstdlib>
# include  <cstring>
# include  <climits>
# include  <iostream>
# include  "compile.h"
# include  <cassert>
# include  "ivl_alloc.h"

استعمال نامخلا std;

غیرعلامتی طویل گنتی_net_arrays = 0;
غیرعلامتی طویل گنتی_net_array_الفاظ = 0;
غیرعلامتی طویل گنتی_var_arrays = 0;
غیرعلامتی طویل گنتی_var_array_الفاظ = 0;
غیرعلامتی طویل گنتی_real_arrays = 0;
غیرعلامتی طویل گنتی_real_array_الفاظ = 0;

ساکن علامت_map_s<ساخت __vpiArray>* array_جدول =0;

جماعت vvp_fun_arrayport;
ساکن خالی array_attach_پورٹ(vvp_array_t, vvp_fun_arrayport*);

vvp_array_t array_ڈھونڈو(ثابت حرف*label)
{
      اگر (array_جدول == 0)
	    واپس 0;

      vvp_array_t v = array_جدول->sym_get_قدر(label);
      واپس v;
}

ساخت __vpiArrayVthrA : عوامی __vpiHandle {
      صحیح get_type_رمز(خالی) ثابت override { واپس vpi_حافظہ_لفظ; }
      صحیح vpi_get(صحیح code) override;
      حرف* vpi_get_str(صحیح code) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_put_قدر(p_vpi_قدر val, صحیح flags) override;
      vpiHandle vpi_handle(صحیح code) override;

      ساخت __vpiArray*array;
	// If this is set, then use it to get the index value.
      vpiHandle پتہ_handle;
	// If wid==0, then address is the address into the array.
      غیرعلامتی پتہ;

      غیرعلامتی get_پتہ() ثابت
      {
	    اگر (پتہ_handle) {
		  s_vpi_قدر vp;
		    /* Check to see if the value is defined. */
		  vp.format = vpi_سمتیہ_Val;
		  پتہ_handle->vpi_get_قدر(&vp);
		  صحیح الفاظ = (پتہ_handle->vpi_get(vpi_سائز)-1)/32 + 1;
		  برائے(صحیح idx = 0; idx < الفاظ; idx += 1) {
			  /* Return UINT_MAX to indicate an X base. */
			اگر (vp.value.vector[idx].bval != 0) واپس UINT_MAX;
		  }
		    /* The value is defined so get and return it. */
		  vp.format = vpiIntVal;
		  پتہ_handle->vpi_get_قدر(&vp);
		  واپس vp.value.integer;
	    }

	    واپس پتہ;
      }
};


ساخت __vpiArrayVthrAPV : عوامی __vpiHandle {
      صحیح get_type_رمز(خالی) ثابت override { واپس vpiPartSelect; }
      صحیح vpi_get(صحیح code) override;
      حرف* vpi_get_str(صحیح code) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;
      vpiHandle vpi_handle(صحیح code) override;

      ساخت __vpiArray*array;
      غیرعلامتی لفظ_sel;
      غیرعلامتی part_ثنائی_ہندسہ;
      غیرعلامتی part_wid;
};

منطقی is_net_array(vpiHandle obj)
{
      ساخت __vpiArray*rfp = dynamic_cast<__vpiArray*> (obj);
      أكد(rfp);

      اگر (rfp->nets != 0) واپس سچ;
      واپس جھوٹ;
}

// This function return true if the underlying array words are real.
ساکن منطقی vpi_array_is_real(ثابت vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      اگر (arr->vals4 != 0)  // A bit based variable/register array.
	    واپس جھوٹ;

      اگر (dynamic_cast<vvp_darray_real*> (arr->vals))
	    واپس سچ;

      اگر (arr->vals != 0)
	    واپس جھوٹ;

	// This must be a net array so look at element 0 to find the type.
      أكد(arr->nets != 0);
      أكد(arr->get_سائز() > 0);
      ثابت ساخت __vpiRealVar*rsig = dynamic_cast<__vpiRealVar*>(arr->nets[0]);
      اگر (rsig) {
	    واپس سچ;
      }

      واپس جھوٹ;
}

ساکن منطقی vpi_array_is_string(ثابت vvp_array_t arr)
{
	// Check to see if this is a variable/register array.
      اگر (arr->vals4 != 0)  // A bit based variable/register array.
	    واپس جھوٹ;

      اگر (dynamic_cast<vvp_darray_string*> (arr->vals))
	    واپس سچ;

      واپس جھوٹ;
}

صحیح __vpiArray::get_لفظ_سائز() ثابت
{
      غیرعلامتی width;

      أكد(get_سائز() > 0);
	/* For a net array we need to get the width from the first element. */
      اگر (nets) {
	    أكد(vals4 == 0 && vals == 0);
	    ثابت ساخت __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(nets[0]);
	    أكد(vsig);
	    width = vpip_سائز(vsig);
	/* For a variable array we can get the width from vals_width. */
      } ورنہ {
	    أكد(vals4 || vals);
	    width = vals_چوڑائی;
      }

      واپس width;
}

حرف*__vpiArray::get_لفظ_str(ساخت __vpiArrayWord*لفظ, صحیح code)
{
      غیرعلامتی اشاریہ = لفظ->get_اشاریہ();

      اگر (code == vpiFile) {  // Not implemented for now!
	    واپس simple_سیٹ_rbuf_str(file_نام[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%d", (صحیح)اشاریہ + پہلا_addr.get_قدر());
      واپس generic_get_str(code, get_scope(), name, sidx);
}

خالی __vpiArray::get_لفظ_قدر(ساخت __vpiArrayWord*لفظ, p_vpi_قدر vp)
{
      غیرعلامتی اشاریہ = لفظ->get_اشاریہ();

    // Determine the appropriate format (The Verilog PLI Handbook 5.2.10)
      اگر(vp->format == vpiObjTypeVal) {
          اگر(vpi_array_is_real(یہ))
              vp->format = vpiRealVal;
          ورنہ اگر(vpi_array_is_string(یہ))
              vp->format = vpiStringVal;
          ورنہ
              vp->format = vpiIntVal;
      }

      اگر(vals4) {
          vpip_vec4_get_قدر(vals4->get_لفظ(اشاریہ),
                              vals4->width(), signed_flag, vp);
      } ورنہ اگر(vals) {
          انتخاب(vp->format) {
            حالت vpiBinStrVal:
            حالت vpiOctStrVal:
            حالت vpiDecStrVal:
            حالت vpiHexStrVal:
            حالت vpiScalarVal:
            حالت vpiIntVal:
            حالت vpi_سمتیہ_Val:
            {
                vvp_vector4_t v;
                vals->get_لفظ(اشاریہ, v);
                vpip_vec4_get_قدر(v, vals_چوڑائی, signed_flag, vp);
            }
            توڑو;

            حالت vpiRealVal:
            {
                دوہرا d;
                vals->get_لفظ(اشاریہ, d);
                vpip_real_get_قدر(d, vp);
            }
            توڑو;

            حالت vpiStringVal:
            {
                string s;
                vals->get_لفظ(اشاریہ, s);
                vpip_string_get_قدر(s, vp);
            }
            توڑو;

            طےشدہ:
                fprintf(stderr, "vpi sorry: format is not implemented\n");
                أكد(جھوٹ);
          }
      }
}

خالی __vpiArray::put_لفظ_قدر(ساخت __vpiArrayWord*لفظ, p_vpi_قدر vp, صحیح)
{
      غیرعلامتی اشاریہ = لفظ->get_اشاریہ();
      vvp_vector4_t val = vec4_from_vpi_قدر(vp, vals_چوڑائی);
      سیٹ_لفظ(اشاریہ, 0, val);
}

vpiHandle __vpiArray::get_iter_اشاریہ(ساخت __vpiArrayIterator*, صحیح idx)
{
      اگر (nets) واپس nets[idx];

      أكد(vals4 || vals);

      اگر (vals_الفاظ == 0) make_vals_الفاظ();

      واپس &(vals_الفاظ[idx].as_لفظ);
}

صحیح __vpiArray::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpi_سطر_No:
	    واپس 0; // Not implemented for now!

	  حالت vpi_سائز:
	    واپس get_سائز();

	  حالت vpiAutomatic:
	    واپس scope->is_automatic()? 1 : 0;

	  طےشدہ:
	    واپس 0;
      }
}

حرف* __vpiArray::vpi_get_str(صحیح code)
{
      اگر (code == vpiFile) {  // Not implemented for now!
            واپس simple_سیٹ_rbuf_str(file_نام[0]);
      }

      واپس generic_get_str(code, scope, name, تہی);
}

vpiHandle __vpiArray::vpi_handle(صحیح code)
{
      انتخاب (code) {
          حالت vpi_بایاں_Range:
            اگر (تبادلہ_addr) واپس &آخری_addr;
            ورنہ واپس &پہلا_addr;

          حالت vpi_دایاں_Range:
            اگر (تبادلہ_addr) واپس &پہلا_addr;
            ورنہ واپس &آخری_addr;

	  حالت vpiScope:
	    واپس scope;

	  حالت vpiModule:
	    واپس vpip_module(scope);
      }

      واپس 0;
}

/*
* VPI code passes indices that are not yet converted to canonical
* form, so this index function does it here.
*/
vpiHandle __vpiArray::vpi_اشاریہ(صحیح اشاریہ)
{
      اشاریہ -= پہلا_addr.get_قدر();
      اگر (اشاریہ >= (طویل) get_سائز())
	    واپس 0;
      اگر (اشاریہ < 0)
	    واپس 0;

      اگر (nets != 0) {
	    واپس nets[اشاریہ];
      }

      اگر (vals_الفاظ == 0)
	    make_vals_الفاظ();

      واپس &(vals_الفاظ[اشاریہ].as_لفظ);
}

صحیح __vpiArrayWord::as_لفظ_t::vpi_get(صحیح code)
{
      ساخت __vpiArrayWord*obj = array_var_لفظ_from_handle(یہ);
      أكد(obj);
      ساخت __vpiArrayBase*my_والد = obj->get_والد();
      t_vpi_قدر val;

      انتخاب (code) {
	  حالت vpi_سطر_No:
	    واپس 0; // Not implemented for now!

	  حالت vpi_سائز:
	    واپس my_والد->get_لفظ_سائز();

	  حالت vpi_بایاں_Range:
            val.format = vpiIntVal;
	    my_والد->get_بایاں_range()->vpi_get_قدر(&val);
            أكد(val.format == vpiIntVal);
            واپس val.value.integer;

	  حالت vpi_دایاں_Range:
            val.format = vpiIntVal;
	    my_والد->get_دایاں_range()->vpi_get_قدر(&val);
            أكد(val.format == vpiIntVal);
            واپس val.value.integer;

	  حالت vpi_اشاریہ_2:
	    {
		  صحیح بنیاد_انحراف = 0;
		  ساخت __vpiArray*بنیاد = dynamic_cast<__vpiArray*> (my_والد);
		  اگر (بنیاد) {
			val.format = vpiIntVal;
			بنیاد->پہلا_addr.vpi_get_قدر(&val);
			بنیاد_انحراف += val.value.integer;
		  }
		  val.format = vpiIntVal;
		  obj->as_اشاریہ.vpi_get_قدر(&val);
		  أكد(val.format == vpiIntVal);
		  واپس val.value.integer + بنیاد_انحراف;
	    }

	  حالت vpiAutomatic:
	    واپس my_والد->get_scope()->is_automatic()? 1 : 0;

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالت _vpiFromThr:
	    واپس _vpiNoThr;
#اگر_ختم

	  طےشدہ:
	    واپس 0;
      }
}


صحیح __vpiArrayVthrA::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpi_سطر_No:
	    واپس 0; // Not implemented for now!

	  حالت vpi_سائز:
	    واپس array->get_لفظ_سائز();

	  حالت vpi_بایاں_Range:
	    واپس array->msb.get_قدر();

	  حالت vpi_دایاں_Range:
	    واپس array->lsb.get_قدر();

	  حالت vpi_اشاریہ_2:
	    واپس (صحیح)get_پتہ() + array->پہلا_addr.get_قدر();

	  حالت vpiAutomatic:
	    واپس array->get_scope()->is_automatic() ? 1 : 0;

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالت _vpiFromThr:
	    واپس _vpi_at_A;
#اگر_ختم

	  // If address_handle is not zero we definitely have a
	  // variable.
	  حالت vpiConstantSelect:
	    واپس پتہ_handle == 0;

	  طےشدہ:
	    واپس 0;
      }
}

حرف* __vpiArrayVthrA::vpi_get_str(صحیح code)
{
      اگر (code == vpiFile) {  // Not implemented for now!
            واپس simple_سیٹ_rbuf_str(file_نام[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%d", (صحیح)get_پتہ() + array->پہلا_addr.get_قدر());
      واپس generic_get_str(code, array->get_scope(), array->name, sidx);
}

خالی __vpiArrayVthrA::vpi_get_قدر(p_vpi_قدر vp)
{
      أكد(array);

      غیرعلامتی اشاریہ = get_پتہ();
      اگر (vpi_array_is_real(array)) {
	    دوہرا tmp = array->get_لفظ_r(اشاریہ);
	    vpip_real_get_قدر(tmp, vp);
      } ورنہ اگر (vpi_array_is_string(array)) {
	    string tmp = array->get_لفظ_str(اشاریہ);
	    vpip_string_get_قدر(tmp, vp);
      } ورنہ {
	    vvp_vector4_t tmp = array->get_لفظ(اشاریہ);
	    غیرعلامتی width = array->get_لفظ_سائز();
	    vpip_vec4_get_قدر(tmp, width, array->signed_flag, vp);
      }
}

vpiHandle __vpiArrayVthrA::vpi_put_قدر(p_vpi_قدر vp, صحیح)
{
      غیرعلامتی اشاریہ = get_پتہ();

      أكد(array);
      أكد(اشاریہ < array->get_سائز());

      اگر (vpi_array_is_real(array)) {
	    دوہرا val = real_from_vpi_قدر(vp);
	    array->سیٹ_لفظ(اشاریہ, val);
      } ورنہ {
	    غیرعلامتی width = array->get_لفظ_سائز();
	    vvp_vector4_t val = vec4_from_vpi_قدر(vp, width);
	    array->سیٹ_لفظ(اشاریہ, 0, val);
      }

      واپس یہ;
}

vpiHandle __vpiArrayVthrA::vpi_handle(صحیح code)
{
      انتخاب (code) {
	  حالت vpi_اشاریہ_2:
	    توڑو;  // Not implemented!

	  حالت vpi_بایاں_Range:
	    واپس &array->msb;

	  حالت vpi_دایاں_Range:
	    واپس &array->lsb;

	  حالت vpi_والد:
	  حالت vpiArray:
	    واپس array;

	  حالت vpiScope:
	    واپس array->get_scope();

	  حالت vpiModule:
	    واپس vpip_module(array->get_scope());
      }

      واپس 0;
}


صحیح __vpiArrayVthrAPV::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpi_سطر_No:
	    واپس 0; // Not implemented for now!

	  حالت vpi_سائز:
	    واپس part_wid;

	  حالت vpi_بایاں_Range:
	    واپس part_ثنائی_ہندسہ + part_wid - 1;

	  حالت vpi_دایاں_Range:
	    واپس part_ثنائی_ہندسہ;

	  حالت vpiAutomatic:
	    واپس array->get_scope()->is_automatic() ? 1 : 0;

#اگر defined(CHECK_WITH_VALGRIND) || defined(BR916_STOPGAP_FIX)
	  حالت _vpiFromThr:
	    واپس _vpi_at_APV;
#اگر_ختم

	  حالت vpiConstantSelect:
	    واپس 1;

	  طےشدہ:
	    واپس 0;
      }
}

حرف* __vpiArrayVthrAPV::vpi_get_str(صحیح code)
{
      اگر (code == vpiFile) {  // Not implemented for now!
            واپس simple_سیٹ_rbuf_str(file_نام[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%u", لفظ_sel + array->پہلا_addr.get_قدر());
      واپس generic_get_str(code, array->get_scope(), array->name, sidx);
}

خالی __vpiArrayVthrAPV::vpi_get_قدر(p_vpi_قدر vp)
{
      أكد(array);

      غیرعلامتی اشاریہ = لفظ_sel;
      اگر (vpi_array_is_real(array)) {
	    دوہرا tmp = array->get_لفظ_r(اشاریہ);
	    vpip_real_get_قدر(tmp, vp);
      } ورنہ {
	    vvp_vector4_t tmp = array->get_لفظ(اشاریہ);
	    tmp = tmp.subvalue(part_ثنائی_ہندسہ, part_wid);
	    vpip_vec4_get_قدر(tmp, part_wid, جھوٹ, vp);
      }
}

vpiHandle __vpiArrayVthrAPV::vpi_handle(صحیح code)
{
      انتخاب (code) {
            // Not currently implemented. We would need to create a VPI
            // object for the memory word.
	  حالت vpi_والد:
	    واپس 0;

            // Not part of the Verilog standard. We use this internally.
	  حالت vpiArray:
	    واپس array;

	  حالت vpiScope:
	    واپس array->get_scope();

	  حالت vpiModule:
	    واپس vpip_module(array->get_scope());
      }

      واپس 0;
}

خالی __vpiArray::سیٹ_لفظ(غیرعلامتی پتہ, غیرعلامتی part_off, ثابت vvp_vector4_t&val)
{
      اگر (پتہ >= get_سائز())
	    واپس;

      اگر (vals4) {
	    أكد(nets == 0);
	    اگر (part_off != 0 || val.size() != vals_چوڑائی) {
		  vvp_vector4_t tmp = vals4->get_لفظ(پتہ);
		  اگر ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_چوڑائی << سطر_ختم;
			أكد(0);
		  }
		  tmp.سیٹ_vec(part_off, val);
		  vals4->سیٹ_لفظ(پتہ, tmp);
	    } ورنہ {
		  vals4->سیٹ_لفظ(پتہ, val);
	    }
	    لفظ_change(پتہ);
	    واپس;
      }

      اگر (vals) {
	    أكد(nets == 0);
	    اگر (part_off != 0 || val.size() != vals_چوڑائی) {
		  vvp_vector4_t tmp;
		  vals->get_لفظ(پتہ, tmp);
		  اگر ((part_off + val.size()) > tmp.size()) {
			cerr << "part_off=" << part_off
			     << " val.size()=" << val.size()
			     << " vals[address].size()=" << tmp.size()
			     << " vals_width=" << vals_چوڑائی << سطر_ختم;
			أكد(0);
		  }
		  tmp.سیٹ_vec(part_off, val);
		  vals->سیٹ_لفظ(پتہ, tmp);
	    } ورنہ {
		  vals->سیٹ_لفظ(پتہ, val);
	    }
	    لفظ_change(پتہ);
	    واپس;
      }

      أكد(nets != 0);

	// Select the word of the array that we affect.
      vpiHandle لفظ = nets[پتہ];
      ثابت ساخت __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(لفظ);
      أكد(vsig);

      vsig->گرہ->send_vec4_pv(val, part_off, vpip_سائز(vsig), 0);
      لفظ_change(پتہ);
}

خالی __vpiArray::سیٹ_لفظ(غیرعلامتی پتہ, دوہرا val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      اگر (پتہ >= vals->get_سائز())
	    واپس;

      vals->سیٹ_لفظ(پتہ, val);
      لفظ_change(پتہ);
}

خالی __vpiArray::سیٹ_لفظ(غیرعلامتی پتہ, ثابت string&val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      اگر (پتہ >= vals->get_سائز())
	    واپس;

      vals->سیٹ_لفظ(پتہ, val);
      لفظ_change(پتہ);
}

خالی __vpiArray::سیٹ_لفظ(غیرعلامتی پتہ, ثابت vvp_object_t&val)
{
      أكد(vals != 0);
      أكد(nets == 0);

      اگر (پتہ >= vals->get_سائز())
	    واپس;

      vals->سیٹ_لفظ(پتہ, val);
      لفظ_change(پتہ);
}

vvp_vector4_t __vpiArray::get_لفظ(غیرعلامتی پتہ)
{
      اگر (vals4) {
	    أكد(nets == 0);
	    أكد(vals == 0);
	    واپس vals4->get_لفظ(پتہ);
      }

      اگر (vals) {
	    أكد(nets == 0);
	    أكد(vals4== 0);
	    اگر (پتہ >= vals->get_سائز())
		  واپس vvp_vector4_t(vals_چوڑائی, BIT4_X);

	    vvp_vector4_t val;
	    vals->get_لفظ(پتہ, val);
	    واپس val;
      }

      أكد(vals4 == 0);
      أكد(vals == 0);
      أكد(nets != 0);

      اگر (پتہ >= get_سائز()) {
	      // Reading outside the array. Return X's but get the
	      // width by looking at a word that we know is present.
	    أكد(get_سائز() > 0);
	    vpiHandle لفظ = nets[0];
	    أكد(لفظ);
	    ساخت __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(لفظ);
	    أكد(vsig);
	    vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (vsig->گرہ->fil);
	    أكد(sig);
	    واپس vvp_vector4_t(sig->قدر_سائز(), BIT4_X);
      }

      vpiHandle لفظ = nets[پتہ];
      ساخت __vpiSignal*vsig = dynamic_cast<__vpiSignal*>(لفظ);
      أكد(vsig);
      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (vsig->گرہ->fil);
      أكد(sig);

      vvp_vector4_t val;
      sig->vec4_قدر(val);
      واپس val;
}

دوہرا __vpiArray::get_لفظ_r(غیرعلامتی پتہ)
{
      اگر (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    اگر (پتہ >= vals->get_سائز())
		  واپس 0.0;

	    دوہرا val;
	    vals->get_لفظ(پتہ, val);
	    واپس val;
      }

      اگر (پتہ >= get_سائز())
	    واپس 0.0;

      أكد(nets);
      vpiHandle لفظ = nets[پتہ];
      ساخت __vpiRealVar*vsig = dynamic_cast<__vpiRealVar*>(لفظ);
      أكد(vsig);
      vvp_signal_قدر*sig = dynamic_cast<vvp_signal_قدر*> (vsig->net->fil);
      أكد(sig);

      دوہرا val = sig->real_قدر();
      واپس val;
}

خالی __vpiArray::get_لفظ_obj(غیرعلامتی پتہ, vvp_object_t&val)
{
      اگر (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    اگر (پتہ >= vals->get_سائز()) {
		  val = vvp_object_t();
		  واپس;
	    }

	    vals->get_لفظ(پتہ, val);
	    واپس;
      }

      أكد(nets);
	// Arrays of string nets not implemented!
      أكد(0);
      واپس;
}

string __vpiArray::get_لفظ_str(غیرعلامتی پتہ)
{
      اگر (vals) {
	    أكد(vals4 == 0);
	    أكد(nets  == 0);
	      // In this context, address out of bounds returns 0.0
	      // instead of an error.
	    اگر (پتہ >= vals->get_سائز())
		  واپس "";

	    string val;
	    vals->get_لفظ(پتہ, val);
	    واپس val;
      }

      أكد(nets);
	// Arrays of string nets not implemented!
      أكد(0);
      واپس "";
}

vpiHandle vpip_make_array(ثابت حرف*label, ثابت حرف*name,
                          صحیح پہلا_addr, صحیح آخری_addr,
                          منطقی signed_flag)
{
      ساخت __vpiArray*obj = نیا __vpiArray;

      obj->signed_flag = signed_flag;

	// Assume increasing addresses.
      اگر (آخری_addr >= پہلا_addr) {
	    obj->تبادلہ_addr = جھوٹ;
      } ورنہ {
	    obj->تبادلہ_addr = سچ;
	    صحیح tmp = آخری_addr;
	    آخری_addr = پہلا_addr;
	    پہلا_addr = tmp;
      }
      أكد(آخری_addr >= پہلا_addr);
      غیرعلامتی array_گنتی = آخری_addr+1-پہلا_addr;

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
      اگر (!array_جدول)
	    array_جدول = نیا علامت_map_s<ساخت __vpiArray>;

      أكد(!array_ڈھونڈو(label));
      array_جدول->sym_سیٹ_قدر(label, obj);

	/* Add this into the table of VPI objects. This is used for
	   contexts that try to look up VPI objects in
	   general. (i.e. arguments to vpi_task calls.) */
      compile_vpi_علامت(label, obj);

	/* Blindly attach to the scope as an object. */
      vpip_attach_مقصد_current_scope(obj);

      واپس obj;
}

خالی __vpiArray::alias_لفظ(غیرعلامتی طویل addr, vpiHandle لفظ, صحیح msb_, صحیح lsb_)
{
      أكد(msb.get_قدر() == msb_);
      أكد(lsb.get_قدر() == lsb_);
      أكد(addr < get_سائز());
      أكد(nets);
      nets[addr] = لفظ;
}

خالی __vpiArray::attach_لفظ(غیرعلامتی addr, vpiHandle لفظ)
{
      أكد(addr < get_سائز());
      أكد(nets);
      nets[addr] = لفظ;

      اگر (ساخت __vpiSignal*sig = dynamic_cast<__vpiSignal*>(لفظ)) {
	    vvp_net_t*net = sig->گرہ;
	    أكد(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    أكد(fun);
	    fun->attach_as_لفظ(یہ, addr);
	    sig->is_netarray = 1;
	    sig->within.والد = یہ;
	    sig->id.اشاریہ = نیا __vpiDecConst(addr + پہلا_addr.get_قدر());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = sig->signed_flag;
	    واپس;
      }

      اگر (ساخت __vpiRealVar*sig = dynamic_cast<__vpiRealVar*>(لفظ)) {
	    vvp_net_t*net = sig->net;
	    أكد(net);
	    vvp_vpi_callback*fun = dynamic_cast<vvp_vpi_callback*>(net->fil);
	    أكد(fun);
	    fun->attach_as_لفظ(یہ, addr);
	    sig->is_netarray = 1;
	    sig->within.والد = یہ;
	    sig->id.اشاریہ = نیا __vpiDecConst(addr + پہلا_addr.get_قدر());
	      // Now we know the data type, update the array signed_flag.
	    signed_flag = سچ;
	    واپس;
      }
}

خالی compile_var_array(حرف*label, حرف*name, صحیح آخری, صحیح پہلا,
		   صحیح msb, صحیح lsb, حرف signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, پہلا, آخری,
                                      signed_flag != 0);

      ساخت __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals_چوڑائی = labs(msb-lsb) + 1;
      اگر (vpip_peek_current_scope()->is_automatic()) {
            arr->vals4 = نیا vvp_vector4array_aa(arr->vals_چوڑائی,
						 arr->get_سائز());
      } ورنہ {
            arr->vals4 = نیا vvp_vector4array_sa(arr->vals_چوڑائی,
						 arr->get_سائز());
      }
      arr->msb.سیٹ_قدر(msb);
      arr->lsb.سیٹ_قدر(lsb);

      گنتی_var_arrays += 1;
      گنتی_var_array_الفاظ += arr->get_سائز();

      آزاد(label);
      حذف[] name;
}

خالی compile_var2_array(حرف*label, حرف*name, صحیح آخری, صحیح پہلا,
		   صحیح msb, صحیح lsb, منطقی signed_flag)
{
      vpiHandle obj = vpip_make_array(label, name, پہلا, آخری, signed_flag);

      ساخت __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->msb.سیٹ_قدر(msb);
      arr->lsb.سیٹ_قدر(lsb);
      arr->vals_چوڑائی = labs(msb-lsb) + 1;

      أكد(! arr->nets);
      اگر (lsb == 0 && msb == 7 && signed_flag) {
	    arr->vals = نیا vvp_darray_atom<int8_t>(arr->get_سائز());
      } ورنہ اگر (lsb == 0 && msb == 7 && !signed_flag) {
	    arr->vals = نیا vvp_darray_atom<uint8_t>(arr->get_سائز());
      } ورنہ اگر (lsb == 0 && msb == 15 && signed_flag) {
	    arr->vals = نیا vvp_darray_atom<int16_t>(arr->get_سائز());
      } ورنہ اگر (lsb == 0 && msb == 15 && !signed_flag) {
	    arr->vals = نیا vvp_darray_atom<uint16_t>(arr->get_سائز());
      } ورنہ اگر (lsb == 0 && msb == 31 && signed_flag) {
	    arr->vals = نیا vvp_darray_atom<int32_t>(arr->get_سائز());
      } ورنہ اگر (lsb == 0 && msb == 31 && !signed_flag) {
	    arr->vals = نیا vvp_darray_atom<uint32_t>(arr->get_سائز());
      } ورنہ اگر (lsb == 0 && msb == 63 && signed_flag) {
	    arr->vals = نیا vvp_darray_atom<int64_t>(arr->get_سائز());
      } ورنہ اگر (lsb == 0 && msb == 63 && !signed_flag) {
	    arr->vals = نیا vvp_darray_atom<uint64_t>(arr->get_سائز());
      } ورنہ {
	    arr->vals = نیا vvp_darray_vec2(arr->get_سائز(), arr->vals_چوڑائی);
      }
      گنتی_var_arrays += 1;
      گنتی_var_array_الفاظ += arr->get_سائز();

      آزاد(label);
      حذف[] name;
}

خالی compile_real_array(حرف*label, حرف*name, صحیح آخری, صحیح پہلا)
{
      vpiHandle obj = vpip_make_array(label, name, پہلا, آخری, سچ);

      ساخت __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = نیا vvp_darray_real(arr->get_سائز());
      arr->vals_چوڑائی = 1;

      گنتی_real_arrays += 1;
      گنتی_real_array_الفاظ += arr->get_سائز();

      آزاد(label);
      حذف[] name;
}

خالی compile_string_array(حرف*label, حرف*name, صحیح آخری, صحیح پہلا)
{
      vpiHandle obj = vpip_make_array(label, name, پہلا, آخری, سچ);

      ساخت __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = نیا vvp_darray_string(arr->get_سائز());
      arr->vals_چوڑائی = 1;

      گنتی_real_arrays += 1;
      گنتی_real_array_الفاظ += arr->get_سائز();

      آزاد(label);
      حذف[] name;
}

خالی compile_object_array(حرف*label, حرف*name, صحیح آخری, صحیح پہلا)
{
      vpiHandle obj = vpip_make_array(label, name, پہلا, آخری, سچ);

      ساخت __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);

	/* Make the words. */
      arr->vals = نیا vvp_darray_object(arr->get_سائز());
      arr->vals_چوڑائی = 1;

      گنتی_real_arrays += 1;
      گنتی_real_array_الفاظ += arr->get_سائز();

      آزاد(label);
      حذف[] name;
}

خالی compile_net_array(حرف*label, حرف*name, صحیح آخری, صحیح پہلا)
{
	// At this point we don't know the array data type, so we
	// initialise signed_flag to false. This will be corrected
	// (if necessary) when we attach words to the array.
      vpiHandle obj = vpip_make_array(label, name, پہلا, آخری, جھوٹ);

      ساخت __vpiArray*arr = dynamic_cast<__vpiArray*>(obj);
      arr->nets = static_cast<vpiHandle*>(صفر_حافظہ_دو(arr->get_سائز(), حجم(vpiHandle)));

      گنتی_net_arrays += 1;
      گنتی_net_array_الفاظ += arr->get_سائز();

      آزاد(label);
      حذف[] name;
}

جماعت vvp_fun_arrayport  : عوامی vvp_net_fun_t {

    عوامی:
      صریح vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net);
      صریح vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, طویل addr);
      ~vvp_fun_arrayport() override;

      مجازی خالی check_لفظ_change(غیرعلامتی طویل addr) = 0;

    محفوظ:
      vvp_array_t arr_;
      vvp_net_t  *net_;
      غیرعلامتی طویل addr_;

      دوست خالی array_attach_پورٹ(vvp_array_t, vvp_fun_arrayport*);
      دوست خالی __vpiArray::لفظ_change(غیرعلامتی طویل);
      vvp_fun_arrayport*اگلا;
};

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net)
: arr_(mem), net_(net), addr_(0)
{
      اگلا = 0;
}

vvp_fun_arrayport::vvp_fun_arrayport(vvp_array_t mem, vvp_net_t*net, طویل addr)
: arr_(mem), net_(net), addr_(addr)
{
      اگلا = 0;
}

vvp_fun_arrayport::~vvp_fun_arrayport()
{
}

جماعت vvp_fun_arrayport_sa  : عوامی vvp_fun_arrayport {

    عوامی:
      صریح vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net);
      صریح vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, طویل addr);
      ~vvp_fun_arrayport_sa() override;

      خالی check_لفظ_change(غیرعلامتی طویل addr) override;

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;

    نجی:
};

vvp_fun_arrayport_sa::vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net)
{
}

vvp_fun_arrayport_sa::vvp_fun_arrayport_sa(vvp_array_t mem, vvp_net_t*net, طویل addr)
: vvp_fun_arrayport(mem, net, addr)
{
}

vvp_fun_arrayport_sa::~vvp_fun_arrayport_sa()
{
}

خالی vvp_fun_arrayport_sa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                     vvp_context_t)
{
      منطقی addr_valid_flag;

      انتخاب (پورٹ.پورٹ()) {

	  حالت 0: // Address input
	    addr_valid_flag = vector4_مقصد_قدر(ثنائی_ہندسہ, addr_);
	    اگر (! addr_valid_flag)
		  addr_ = arr_->get_سائز();
	    اگر (vpi_array_is_real(arr_))
		  پورٹ.ptr()->send_real(arr_->get_لفظ_r(addr_), 0);
	    ورنہ
		  پورٹ.ptr()->send_vec4(arr_->get_لفظ(addr_), 0);

	    توڑو;

	  طےشدہ:
	    fprintf(stdout, "XXXX write ports not implemented.\n");
	    أكد(0);
      }
}

خالی vvp_fun_arrayport_sa::check_لفظ_change(غیرعلامتی طویل addr)
{
      اگر (addr != addr_) واپس;

      اگر (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_لفظ_r(addr_), 0);
      } ورنہ {
	    net_->send_vec4(arr_->get_لفظ(addr_), 0);
      }
}

جماعت vvp_fun_arrayport_aa  : عوامی vvp_fun_arrayport, عوامی automatic_hooks_s {

    عوامی:
      صریح vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net);
      صریح vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, طویل addr);
      ~vvp_fun_arrayport_aa() override;

      خالی alloc_instance(vvp_context_t context) override;
      خالی reset_instance(vvp_context_t context) override;
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      خالی free_instance(vvp_context_t context) override;
#اگر_ختم

      خالی check_لفظ_change(غیرعلامتی طویل addr) override;

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;

    نجی:
      خالی check_لفظ_change_2(غیرعلامتی طویل addr, vvp_context_t context);

      __vpiScope*context_scope_;
      غیرعلامتی context_idx_;
};

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net)
: vvp_fun_arrayport(mem, net), context_scope_(context_scope)
{
      context_idx_ = vpip_جوڑو_شے_مقصد_context(یہ, context_scope);
}

vvp_fun_arrayport_aa::vvp_fun_arrayport_aa(__vpiScope*context_scope, vvp_array_t mem, vvp_net_t*net, طویل addr)
: vvp_fun_arrayport(mem, net, addr), context_scope_(context_scope)
{
      context_idx_ = vpip_جوڑو_شے_مقصد_context(یہ, context_scope);
}

vvp_fun_arrayport_aa::~vvp_fun_arrayport_aa()
{
}

خالی vvp_fun_arrayport_aa::alloc_instance(vvp_context_t context)
{
      غیرعلامتی طویل*addr = نیا غیرعلامتی طویل;
      vvp_سیٹ_context_شے(context, context_idx_, addr);

      *addr = addr_;
}

خالی vvp_fun_arrayport_aa::reset_instance(vvp_context_t context)
{
      غیرعلامتی طویل*addr = static_cast<غیرعلامتی طویل*>
            (vvp_get_context_شے(context, context_idx_));

      *addr = addr_;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی vvp_fun_arrayport_aa::free_instance(vvp_context_t context)
{
      غیرعلامتی طویل*addr = static_cast<غیرعلامتی طویل*>
            (vvp_get_context_شے(context, context_idx_));
      حذف addr;
}
#اگر_ختم

خالی vvp_fun_arrayport_aa::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                     vvp_context_t context)
{
      اگر (context) {
            غیرعلامتی طویل*addr = static_cast<غیرعلامتی طویل*>
                  (vvp_get_context_شے(context, context_idx_));

            منطقی addr_valid_flag;

            انتخاب (پورٹ.پورٹ()) {

                حالت 0: // Address input
                  addr_valid_flag = vector4_مقصد_قدر(ثنائی_ہندسہ, *addr);
                  اگر (! addr_valid_flag) *addr = arr_->get_سائز();
                  اگر (vpi_array_is_real(arr_)) {
			پورٹ.ptr()->send_real(arr_->get_لفظ_r(*addr),
					      context);
                  } ورنہ {
			پورٹ.ptr()->send_vec4(arr_->get_لفظ(*addr),
					      context);
                  }
                  توڑو;

                طےشدہ:
                  fprintf(stdout, "XXXX write ports not implemented.\n");
                  أكد(0);
            }
      } ورنہ {
            context = context_scope_->زندہ_contexts;
            جب (context) {
                  recv_vec4(پورٹ, ثنائی_ہندسہ, context);
                  context = vvp_get_اگلا_context(context);
            }
      }
}

خالی vvp_fun_arrayport_aa::check_لفظ_change_2(غیرعلامتی طویل addr,
                                              vvp_context_t context)
{
      ثابت غیرعلامتی طویل*پورٹ_addr = static_cast<غیرعلامتی طویل*>
            (vvp_get_context_شے(context, context_idx_));

      اگر (addr != *پورٹ_addr)
	    واپس;

      اگر (vpi_array_is_real(arr_)) {
	    net_->send_real(arr_->get_لفظ_r(addr), context);
      } ورنہ {
	    net_->send_vec4(arr_->get_لفظ(addr), context);
      }
}

خالی vvp_fun_arrayport_aa::check_لفظ_change(غیرعلامتی طویل addr)
{
      اگر (arr_->get_scope()->is_automatic()) {
            أكد(vthread_get_wt_context());
            check_لفظ_change_2(addr, vthread_get_wt_context());
      } ورنہ {
            vvp_context_t context = context_scope_->زندہ_contexts;
            جب (context) {
                  check_لفظ_change_2(addr, context);
                  context = vvp_get_اگلا_context(context);
            }
      }
}

ساکن خالی array_attach_پورٹ(vvp_array_t array, vvp_fun_arrayport*fun)
{
      أكد(fun->اگلا == 0);
      fun->اگلا = array->ports_;
      array->ports_ = fun;
      اگر (!array->get_scope()->is_automatic() &&
          (array->vals4 || array->vals)) {
              /* propagate initial values for variable arrays */
            اگر (!vpi_array_is_real(array)) {
                  vvp_bit4_t init;
                  اگر (array->vals4)
                      init = BIT4_X;
                  ورنہ
                      init = BIT4_0;
                  vvp_vector4_t tmp(array->vals_چوڑائی, init);
                  جدول_وقت_init_propagate(fun->net_, tmp);
            } ورنہ {
                  جدول_وقت_init_propagate(fun->net_, 0.0);
            }
      }
}

جماعت array_لفظ_قدر_callback : عوامی قدر_callback {
    عوامی:
      اندرونی صریح array_لفظ_قدر_callback(p_cb_ڈیٹا ڈیٹا, طویل addr)
      : قدر_callback(ڈیٹا), لفظ_addr(addr)
      { }

    عوامی:
      طویل لفظ_addr;
};

خالی __vpiArray::لفظ_change(غیرعلامتی طویل addr)
{
      برائے (vvp_fun_arrayport*cur = ports_; cur; cur = cur->اگلا)
	    cur->check_لفظ_change(addr);

	// Run callbacks attached to the array itself.
      ساخت __vpiCallback *اگلا_2 = vpi_callbacks;
      ساخت __vpiCallback *prev = 0;

      جب (اگلا_2) {
	    array_لفظ_قدر_callback*cur = dynamic_cast<array_لفظ_قدر_callback*>(اگلا_2);
	    اگلا_2 = cur->اگلا_2;

	      // Skip callbacks that are not for me. -1 is for every element.
	    اگر (cur->لفظ_addr != (طویل)addr && cur->لفظ_addr != -1) {
		  prev = cur;
		  جاری;
	    }

	      // For whole array callbacks we need to set the index.
	    اگر (cur->لفظ_addr == -1) {
		  cur->cb_ڈیٹا.اشاریہ = (PLI_INT32) ((صحیح)addr +
						    پہلا_addr.get_قدر());
	    }

	    اگر (cur->cb_ڈیٹا.cb_rtn != 0) {
		  اگر (cur->test_قدر_callback_ready()) {
			اگر (cur->cb_ڈیٹا.value) {
			      اگر (vpi_array_is_real(یہ)) {
				    دوہرا val = 0.0;
				    اگر (addr < vals->get_سائز())
					  vals->get_لفظ(addr, val);
				    vpip_real_get_قدر(val, cur->cb_ڈیٹا.value);
			      } ورنہ اگر (vals4) {
				    vpip_vec4_get_قدر(vals4->get_لفظ(addr),
							vals_چوڑائی,
							signed_flag,
							cur->cb_ڈیٹا.value);
			      } ورنہ اگر (dynamic_cast<vvp_darray_atom<int8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<int64_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint8_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint16_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint32_t>*>(vals)
				      || dynamic_cast<vvp_darray_atom<uint64_t>*>(vals)
				      || dynamic_cast<vvp_darray_vec2*>(vals)) {
				    vvp_vector4_t val;
				    اگر (addr < vals->get_سائز())
					  vals->get_لفظ(addr, val);
				    vpip_vec4_get_قدر(val,
							vals_چوڑائی,
							signed_flag,
							cur->cb_ڈیٹا.value);
			      } ورنہ {
			            أكد(0);
			      }
			}

			callback_execute(cur);
		  }

		  prev = cur;

	    } ورنہ اگر (prev == 0) {

		  vpi_callbacks = اگلا_2;
		  cur->اگلا_2 = 0;
		  حذف cur;

	    } ورنہ {
		  أكد(prev->اگلا_2 == cur);
		  prev->اگلا_2 = اگلا_2;
		  cur->اگلا_2 = 0;
		  حذف cur;
	    }
      }
}

جماعت array_resolv_list_t : عوامی resolv_list_s {

    عوامی:
      صریح array_resolv_list_t(حرف*lab) : resolv_list_s(lab) {
	    array = 0;
      }

      vvp_array_t*array;
      منطقی resolve(منطقی mes) override;

    نجی:
};

منطقی array_resolv_list_t::resolve(منطقی mes)
{
      *array = array_ڈھونڈو(label());
      اگر (*array == 0) {
	    أكد(!mes);
	    واپس جھوٹ;
      }
      واپس سچ;
}

جماعت array_پورٹ_resolv_list_t : عوامی resolv_list_s {

    عوامی:
      صریح array_پورٹ_resolv_list_t(حرف* lab, منطقی use_addr__,
                                        طویل addr__);

      __vpiScope*context_scope;
      vvp_net_t*ptr;
      منطقی use_addr;
      طویل addr;
      منطقی resolve(منطقی mes) override;

    نجی:
};

array_پورٹ_resolv_list_t::array_پورٹ_resolv_list_t(حرف *lab, منطقی use_addr__,
                                                   طویل addr__)
: resolv_list_s(lab), use_addr(use_addr__), addr(addr__)
{
      اگر (vpip_peek_current_scope()->is_automatic())
            context_scope = vpip_peek_context_scope();
      ورنہ
            context_scope = 0;
      ptr = نیا vvp_net_t;
}

منطقی array_پورٹ_resolv_list_t::resolve(منطقی mes)
{
      vvp_array_t mem = array_ڈھونڈو(label());
      اگر (mem == 0) {
	    أكد(mem || !mes);
	    واپس جھوٹ;
      }

      vvp_fun_arrayport*fun;
      اگر (use_addr)
            اگر (context_scope)
                  fun = نیا vvp_fun_arrayport_aa(context_scope, mem, ptr, addr);
            ورنہ
                  fun = نیا vvp_fun_arrayport_sa(mem, ptr, addr);
      ورنہ
            اگر (context_scope)
                  fun = نیا vvp_fun_arrayport_aa(context_scope, mem, ptr);
            ورنہ
                  fun = نیا vvp_fun_arrayport_sa(mem, ptr);
      ptr->fun = fun;

      array_attach_پورٹ(mem, fun);

      واپس سچ;
}

جماعت array_لفظ_part_callback : عوامی array_لفظ_قدر_callback {
    عوامی:
      صریح array_لفظ_part_callback(p_cb_ڈیٹا ڈیٹا, طویل addr);
      ~array_لفظ_part_callback() override;

      array_لفظ_part_callback(ثابت array_لفظ_part_callback&) = حذف;
      array_لفظ_part_callback& عامل=(ثابت array_لفظ_part_callback&) = حذف;

      منطقی test_قدر_callback_ready(خالی) override;

    نجی:
      حرف*قدر_بٹ;
};

array_لفظ_part_callback::array_لفظ_part_callback(p_cb_ڈیٹا ڈیٹا, طویل addr)
: array_لفظ_قدر_callback(ڈیٹا, addr)
{
	// Get the initial value of the part, to use as a reference.
      ساخت __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(ڈیٹا->obj);
      s_vpi_قدر tmp_قدر;
      tmp_قدر.format = vpiBinStrVal;
      apvword->vpi_get_قدر(&tmp_قدر);

      قدر_بٹ = نیا حرف[apvword->part_wid+1];

      حافظہ_نقل(قدر_بٹ, tmp_قدر.value.str, apvword->part_wid);
      قدر_بٹ[apvword->part_wid] = 0;
}

array_لفظ_part_callback::~array_لفظ_part_callback()
{
      حذف[]قدر_بٹ;
}

منطقی array_لفظ_part_callback::test_قدر_callback_ready(خالی)
{
      ساخت __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(cb_ڈیٹا.obj);
      أكد(apvword);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_قدر tmp_قدر;
      tmp_قدر.format = vpiBinStrVal;
      apvword->vpi_get_قدر(&tmp_قدر);

      اگر (memcmp(قدر_بٹ, tmp_قدر.value.str, apvword->part_wid) == 0)
	    واپس جھوٹ;

      حافظہ_نقل(قدر_بٹ, tmp_قدر.value.str, apvword->part_wid);
      واپس سچ;

}

قدر_callback*vpip_array_لفظ_change(p_cb_ڈیٹا ڈیٹا)
{
      ساخت __vpiArray*والد = 0;
      array_لفظ_قدر_callback*cbh = 0;
      اگر (ثابت ساخت __vpiArrayWord*لفظ = array_var_لفظ_from_handle(ڈیٹا->obj)) {
	    والد = static_cast<__vpiArray*>(لفظ->get_والد());
	    غیرعلامتی addr = لفظ->get_اشاریہ();
	    cbh = نیا array_لفظ_قدر_callback(ڈیٹا, addr);

      } ورنہ اگر (ساخت __vpiArrayVthrA*tword = dynamic_cast<__vpiArrayVthrA*>(ڈیٹا->obj)) {
	    والد = tword->array;
	    cbh = نیا array_لفظ_قدر_callback(ڈیٹا, tword->پتہ);

      } ورنہ اگر (ساخت __vpiArrayVthrAPV*apvword = dynamic_cast<__vpiArrayVthrAPV*>(ڈیٹا->obj)) {
	    والد = apvword->array;
	    cbh = نیا array_لفظ_part_callback(ڈیٹا, apvword->لفظ_sel);
      }

      أكد(cbh);
      أكد(والد);
      cbh->اگلا_2 = والد->vpi_callbacks;
      والد->vpi_callbacks = cbh;

      واپس cbh;
}

قدر_callback* vpip_array_change(p_cb_ڈیٹا ڈیٹا)
{
      array_لفظ_قدر_callback*cbh = نیا array_لفظ_قدر_callback(ڈیٹا, -1);
      أكد(ڈیٹا->obj);

      ساخت __vpiArray*arr = dynamic_cast<__vpiArray*>(ڈیٹا->obj);
      cbh->اگلا_2 = arr->vpi_callbacks;
      arr->vpi_callbacks = cbh;
      واپس cbh;
}

خالی compile_array_پورٹ(حرف*label, حرف*array, حرف*addr)
{
      array_پورٹ_resolv_list_t*resolv_mem
	    = نیا array_پورٹ_resolv_list_t(array, جھوٹ, 0);

      define_functor_علامت(label, resolv_mem->ptr);
      آزاد(label);
	// Connect the port-0 input as the address.
      داخلہ_connect(resolv_mem->ptr, 0, addr);

      resolv_submit(resolv_mem);
}

خالی compile_array_پورٹ(حرف*label, حرف*array, طویل addr)
{
      array_پورٹ_resolv_list_t*resolv_mem
	    = نیا array_پورٹ_resolv_list_t(array, سچ, addr);

      define_functor_علامت(label, resolv_mem->ptr);
      آزاد(label);

      resolv_submit(resolv_mem);
}

خالی compile_array_alias(حرف*label, حرف*name, حرف*src)
{
      vvp_array_t mem = array_ڈھونڈو(src);
      أكد(mem);

      ساخت __vpiArray*obj = نیا __vpiArray;

      obj->scope = vpip_peek_current_scope();
      obj->name  = vpip_name_string(name);
      obj->array_گنتی = mem->array_گنتی;
      obj->signed_flag = mem->signed_flag;

	// Need to set an accurate range of addresses.
      obj->پہلا_addr = mem->پہلا_addr;
      obj->آخری_addr  = mem->آخری_addr;
      obj->تبادلہ_addr = mem->تبادلہ_addr;

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
      أكد(!array_ڈھونڈو(label));
      array_جدول->sym_سیٹ_قدر(label, obj);

      compile_vpi_علامت(label, obj);
      vpip_attach_مقصد_current_scope(obj);

      آزاد(label);
      آزاد(name);
      آزاد(src);
}

/*
 * &A<label,addr>
 * This represents a VPI handle for an addressed array. This comes
 * from expressions like "label[addr]" where "label" is the array and
 * "addr" is the canonical address of the desired word.
 */
vpiHandle vpip_make_vthr_A(حرف*label, غیرعلامتی addr)
{
      ساخت __vpiArrayVthrA*obj = نیا __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = نیا array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->پتہ_handle = 0;
      obj->پتہ = addr;

      واپس obj;
}

/*
 * &A<label,symbol>
 * This represents a VPI handle for an addressed word, where the
 * word address is calculated from the VPI object that symbol
 * represents. The expression that leads to this looks like label[symbol].
 */
vpiHandle vpip_make_vthr_A(حرف*label, حرف*علامت)
{
      ساخت __vpiArrayVthrA*obj = نیا __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = نیا array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->پتہ_handle = 0;
      compile_vpi_lookup(&obj->پتہ_handle, علامت);
      obj->پتہ = 0;

      واپس obj;
}

vpiHandle vpip_make_vthr_A(حرف*label, vpiHandle handle)
{
      ساخت __vpiArrayVthrA*obj = نیا __vpiArrayVthrA;

      array_resolv_list_t*resolv_mem
	    = نیا array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->پتہ_handle = handle;
      obj->پتہ = 0;

      واپس obj;
}

vpiHandle vpip_make_vthr_APV(حرف*label, غیرعلامتی اشاریہ, غیرعلامتی ثنائی_ہندسہ, غیرعلامتی wid)
{
      ساخت __vpiArrayVthrAPV*obj = نیا __vpiArrayVthrAPV;

      array_resolv_list_t*resolv_mem
	    = نیا array_resolv_list_t(label);

      resolv_mem->array = &obj->array;
      resolv_submit(resolv_mem);

      obj->لفظ_sel = اشاریہ;
      obj->part_ثنائی_ہندسہ = ثنائی_ہندسہ;
      obj->part_wid = wid;

      واپس obj;
}

خالی compile_array_cleanup(خالی)
{
      حذف array_جدول;
      array_جدول = 0;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی حافظہ_delete(vpiHandle شے)
{
      ساخت __vpiArray*arr = static_cast<ساخت __vpiArray*>(شے);
      اگر (arr->vals_الفاظ) حذف [] (arr->vals_الفاظ-1);

//      if (arr->vals4) {}
// Delete the individual words?
// constant_delete(handle)?
      حذف arr->vals4;

//      if (arr->vals) {}
// Delete the individual words?
// constant_delete(handle)?
      حذف arr->vals;

      اگر (arr->nets) {
	    برائے (غیرعلامتی idx = 0; idx < arr->get_سائز(); idx += 1) {
		  اگر (ساخت __vpiSignal*sig =
		      dynamic_cast<__vpiSignal*>(arr->nets[idx])) {
// Delete the individual words?
			constant_delete(sig->id.اشاریہ);
		    /* These should only be the real words. */
		  } ورنہ {
			أكد(arr->nets[idx]->get_type_رمز() ==
			       vpiRealVar);
			ساخت __vpiRealVar *sigr = static_cast<ساخت __vpiRealVar *>
			                            (arr->nets[idx]);
			constant_delete(sigr->id.اشاریہ);
// Why are only the real words still here?
			حذف arr->nets[idx];
		  }
	    }
	    آزاد(arr->nets);
      }

      جب (arr->vpi_callbacks) {
	    ساخت __vpiCallback*tmp = arr->vpi_callbacks->اگلا_2;
	    حذف arr->vpi_callbacks;
	    arr->vpi_callbacks = tmp;
      }

      حذف arr;
}

خالی A_delete(vpiHandle شے)
{
      ساخت __vpiArrayVthrA*obj = static_cast<ساخت __vpiArrayVthrA*>(شے);
      اگر (obj->پتہ_handle) {
	    انتخاب (obj->پتہ_handle->get_type_رمز()) {
		حالت vpi_حافظہ_لفظ:
		  اگر (vpi_get(_vpiFromThr, obj->پتہ_handle) == _vpi_at_A) {
			A_delete(obj->پتہ_handle);
		  }
		  توڑو;
		حالت vpiPartSelect:
		  أكد(vpi_get(_vpiFromThr, obj->پتہ_handle) ==
		         _vpi_at_PV);
		  PV_delete(obj->پتہ_handle);
		  توڑو;
	    }
      }

      حذف obj;
}

خالی APV_delete(vpiHandle شے)
{
      ساخت __vpiArrayVthrAPV*obj = static_cast<ساخت __vpiArrayVthrAPV*>(شے);
      حذف obj;
}
#اگر_ختم
