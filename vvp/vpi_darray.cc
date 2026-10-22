/*
 * Copyright (c) 2012-2026 Stephen Williams (steve@icarus.com)
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


# include  "compile.h"
# include  "vpi_priv.h"
# include  "vvp_net_sig.h"
# include  "vvp_darray.h"
# include  "vvp_cobject.h"
# include  "array_common.h"
# include  "schedule.h"
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#اگر_ختم
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

استعمال نامخلا std;

__vpiDarrayVar::__vpiDarrayVar(__vpiScope*sc, ثابت حرف*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

غیرعلامتی __vpiDarrayVar::get_سائز() ثابت
{
      ثابت vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      اگر(!fun)
        واپس 0;

      vvp_object_t val = fun->get_object();
      ثابت vvp_darray*aval = val.peek<vvp_darray>();

      اگر(!aval)
        واپس 0;

      واپس aval->get_سائز();
}

vpiHandle __vpiDarrayVar::get_بایاں_range()
{
      بایاں_range.سیٹ_قدر(0);
      واپس &بایاں_range;
}

vpiHandle __vpiDarrayVar::get_دایاں_range()
{
      دایاں_range.سیٹ_قدر(get_سائز() - 1);
      واپس &دایاں_range;
}

صحیح __vpiDarrayVar::get_لفظ_سائز() ثابت
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_لفظ(0, new_vec);
      واپس new_vec.size();
}

حرف*__vpiDarrayVar::get_لفظ_str(ساخت __vpiArrayWord*لفظ, صحیح code)
{
      غیرعلامتی اشاریہ = لفظ->get_اشاریہ();

      اگر (code == vpiFile) {  // Not implemented for now!
	    واپس simple_سیٹ_rbuf_str(file_نام[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%d", (صحیح)اشاریہ);
      واپس generic_get_str(code, scope_, name_, sidx);
}

خالی __vpiDarrayVar::get_لفظ_قدر(ساخت __vpiArrayWord*لفظ, p_vpi_قدر vp)
{
      غیرعلامتی اشاریہ = لفظ->get_اشاریہ();
      vvp_darray*aobj = get_vvp_darray();

      اگر(vp->format == vpiObjTypeVal) {
          اگر(dynamic_cast<vvp_darray_real*>(aobj))
              vp->format = vpiRealVal;
          ورنہ اگر(dynamic_cast<vvp_darray_string*>(aobj))
              vp->format = vpiStringVal;
          ورنہ
              vp->format = vpi_سمتیہ_Val;
      }

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
          aobj->get_لفظ(اشاریہ, v);
          vpip_vec4_get_قدر(v, v.size(), جھوٹ, vp);  // TODO sign?
      }
      توڑو;

      حالت vpiRealVal:
      {
          دوہرا d;
          aobj->get_لفظ(اشاریہ, d);
          vpip_real_get_قدر(d, vp);
      }
      توڑو;

      حالت vpiStringVal:
      {
          string s;
          aobj->get_لفظ(اشاریہ, s);
          vpip_string_get_قدر(s, vp);
      }
      توڑو;

      طےشدہ:
          fprintf(stderr, "vpi sorry: format is not implemented\n");
          أكد(جھوٹ);
      }
}

خالی __vpiDarrayVar::put_لفظ_قدر(ساخت __vpiArrayWord*لفظ, p_vpi_قدر vp, صحیح)
{
      غیرعلامتی اشاریہ = لفظ->get_اشاریہ();
      vvp_darray*aobj = get_vvp_darray();

      انتخاب(vp->format) {
      حالت vpiScalarVal:
      حالت vpiIntVal:
      حالت vpi_سمتیہ_Val: {
	    vvp_vector4_t vec = vec4_from_vpi_قدر(vp, get_لفظ_سائز());
	    aobj->سیٹ_لفظ(اشاریہ, vec);
      }
      توڑو;

      حالت vpiRealVal:
        aobj->سیٹ_لفظ(اشاریہ, vp->value.real);
        توڑو;

      حالت vpiStringVal:
        aobj->سیٹ_لفظ(اشاریہ, std::string(vp->value.str));
        توڑو;

      طےشدہ:
          fprintf(stderr, "vpi sorry: format is not implemented");
          أكد(جھوٹ);
      }
}

vpiHandle __vpiDarrayVar::get_iter_اشاریہ(ساخت __vpiArrayIterator*, صحیح idx)
{
      اگر (vals_الفاظ == 0) make_vals_الفاظ();

      واپس &(vals_الفاظ[idx].as_لفظ);
}

صحیح __vpiDarrayVar::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpiArrayType:
	    واپس vpiDynamicArray;
	  حالت vpi_بایاں_Range:
	    واپس 0;
	  حالت vpi_دایاں_Range:
	    واپس get_سائز() - 1;
	  حالت vpi_سائز:
            واپس get_سائز();

	  طےشدہ:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    أكد(جھوٹ);
	    واپس 0;
      }
}

حرف* __vpiDarrayVar::vpi_get_str(صحیح code)
{
      اگر (code == vpiFile) {  // Not implemented for now!
            واپس simple_سیٹ_rbuf_str(file_نام[0]);
      }

      واپس generic_get_str(code, scope_, name_, تہی);
}

vpiHandle __vpiDarrayVar::vpi_handle(صحیح code)
{
      انتخاب (code) {
          حالت vpi_بایاں_Range:
            واپس get_بایاں_range();

          حالت vpi_دایاں_Range:
            واپس get_دایاں_range();

          حالت vpiScope:
            واپس scope_;

          حالت vpiModule:
            واپس vpip_module(scope_);
      }

      واپس 0;
}

vpiHandle __vpiDarrayVar::vpi_اشاریہ(صحیح اشاریہ)
{
      اگر (اشاریہ >= (طویل) get_سائز())
	    واپس 0;
      اگر (اشاریہ < 0)
	    واپس 0;

      اگر (vals_الفاظ == 0)
	    make_vals_الفاظ();

      واپس &(vals_الفاظ[اشاریہ].as_لفظ);
}

خالی __vpiDarrayVar::vpi_get_قدر(p_vpi_قدر val)
{
      val->format = vpiSuppressVal;
}

vvp_darray*__vpiDarrayVar::get_vvp_darray() ثابت
{
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      أكد(fun);
      vvp_object_t obj = fun->get_object();

      واپس obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(ثابت حرف*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ثابت حرف*use_name = name ? vpip_name_string(name) : تہی;

      __vpiDarrayVar*obj = نیا __vpiDarrayVar(scope, use_name, net);

      واپس obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, ثابت حرف*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

صحیح __vpiQueueVar::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpiArrayType:
	    واپس vpi_قطار_Array;
	  طےشدہ:
	    واپس __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_قطار_var(ثابت حرف*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ثابت حرف*use_name = name ? vpip_name_string(name) : تہی;

      __vpiQueueVar*obj = نیا __vpiQueueVar(scope, use_name, net);

      واپس obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, غیرعلامتی pidx,
				     منطقی is_قطار)
: class_net_(0), prop_idx_(pidx), is_قطار_2(is_قطار), scope_(scope)
{
}

صحیح __vpiPropQueueRef::get_type_رمز(خالی) ثابت
{ واپس vpiArrayVar; }

ساکن vvp_darray* get_زندہ_darray_from_prop(vvp_net_t* class_net, غیرعلامتی pid)
{
      اگر (!class_net) واپس 0;
      ثابت vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      اگر (!fun) واپس 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      اگر (!cobj) واپس 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      واپس qobj.peek<vvp_darray>();
}

صحیح __vpiPropQueueRef::vpi_get(صحیح code)
{
      ثابت vvp_darray* aobj = get_زندہ_darray_from_prop(class_net_, prop_idx_);
      انتخاب (code) {
	  حالت vpiArrayType:
	    واپس is_قطار_2 ? vpi_قطار_Array : vpiDynamicArray;
	  حالت vpi_بایاں_Range:
	    واپس 0;
	  حالت vpi_دایاں_Range:
	    واپس aobj ? (صحیح) (aobj->get_سائز() - 1) : 0;
	  حالت vpi_سائز:
	    واپس aobj ? (صحیح) aobj->get_سائز() : 0;
	  طےشدہ:
	    واپس 0;
      }
}

حرف* __vpiPropQueueRef::vpi_get_str(صحیح code)
{
      اگر (code == vpiFile) {
	    واپس simple_سیٹ_rbuf_str(file_نام[0]);
      }
      واپس generic_get_str(code, scope_, "prop_darray", تہی);
}

خالی __vpiPropQueueRef::vpi_get_قدر(p_vpi_قدر val)
{
      val->format = vpiSuppressVal;
}

vpiHandle vpip_make_prop_قطار_ref(حرف* class_label, غیرعلامتی prop_idx,
				   غیرعلامتی is_قطار_flag)
{
      __vpiPropQueueRef* obj = نیا __vpiPropQueueRef(vpip_peek_current_scope(),
						     prop_idx,
						     is_قطار_flag != 0);
      functor_ref_lookup(&obj->class_net_, class_label);
      واپس obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      اگر (!ref) واپس 0;

      اگر (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    واپس get_زندہ_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      اگر (ثابت __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    ثابت vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    اگر (!fun) واپس 0;
	    vvp_object_t obj = fun->get_object();
	    واپس obj.peek<vvp_darray>();
      }

      واپس 0;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی array_delete(vpiHandle شے)
{
      اگر (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(شے)) {
	    حذف pq;
	    واپس;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(شے);
      اگر (dobj) {
	    اگر (dobj->vals_الفاظ) حذف [] (dobj->vals_الفاظ-1);
	    حذف dobj;
	    واپس;
      }

      fprintf(stderr, "Need support for deleting array type: %d\n", شے->vpi_get(vpiArrayType));
      أكد(0);
}
#اگر_ختم
