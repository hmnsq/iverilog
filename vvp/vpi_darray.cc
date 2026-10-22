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
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#جے_انت
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

ورتوں نطاق std;

__vpiDarrayVar::__vpiDarrayVar(__vpiScope*sc, ثابت حرف*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

بنانشانی __vpiDarrayVar::get_ناپ() ثابت
{
      ثابت vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      إذا(!fun)
        أرجع 0;

      vvp_object_t val = fun->get_object();
      ثابت vvp_darray*aval = val.peek<vvp_darray>();

      إذا(!aval)
        أرجع 0;

      أرجع aval->get_ناپ();
}

vpiHandle __vpiDarrayVar::get_بایاں_range()
{
      بایاں_range.سیٹ_قدر(0);
      أرجع &بایاں_range;
}

vpiHandle __vpiDarrayVar::get_دایاں_range()
{
      دایاں_range.سیٹ_قدر(get_ناپ() - 1);
      أرجع &دایاں_range;
}

صحيح __vpiDarrayVar::get_لفظ_ناپ() ثابت
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_لفظ(0, new_vec);
      أرجع new_vec.size();
}

حرف*__vpiDarrayVar::get_لفظ_str(بنية __vpiArrayWord*لفظ, صحيح code)
{
      بنانشانی اشاریہ = لفظ->get_اشاریہ();

      إذا (code == vpiFile) {  // Not implemented for now!
	    أرجع simple_سیٹ_rbuf_str(file_ناں[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%d", (صحيح)اشاریہ);
      أرجع generic_get_str(code, scope_, name_, sidx);
}

فراغ __vpiDarrayVar::get_لفظ_قدر(بنية __vpiArrayWord*لفظ, p_vpi_قدر vp)
{
      بنانشانی اشاریہ = لفظ->get_اشاریہ();
      vvp_darray*aobj = get_vvp_darray();

      إذا(vp->format == vpiObjTypeVal) {
          إذا(dynamic_cast<vvp_darray_real*>(aobj))
              vp->format = vpiRealVal;
          وإلا إذا(dynamic_cast<vvp_darray_string*>(aobj))
              vp->format = vpiStringVal;
          وإلا
              vp->format = vpi_سمتیہ_Val;
      }

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
          aobj->get_لفظ(اشاریہ, v);
          vpip_vec4_get_قدر(v, v.size(), خطأ, vp);  // TODO sign?
      }
      اكسر;

      حالة vpiRealVal:
      {
          مزدوج d;
          aobj->get_لفظ(اشاریہ, d);
          vpip_real_get_قدر(d, vp);
      }
      اكسر;

      حالة vpiStringVal:
      {
          string s;
          aobj->get_لفظ(اشاریہ, s);
          vpip_string_get_قدر(s, vp);
      }
      اكسر;

      افتراضي:
          fprintf(stderr, "vpi sorry: format is not implemented\n");
          أكد(خطأ);
      }
}

فراغ __vpiDarrayVar::put_لفظ_قدر(بنية __vpiArrayWord*لفظ, p_vpi_قدر vp, صحيح)
{
      بنانشانی اشاریہ = لفظ->get_اشاریہ();
      vvp_darray*aobj = get_vvp_darray();

      بدل(vp->format) {
      حالة vpiScalarVal:
      حالة vpiIntVal:
      حالة vpi_سمتیہ_Val: {
	    vvp_vector4_t vec = vec4_from_vpi_قدر(vp, get_لفظ_ناپ());
	    aobj->سیٹ_لفظ(اشاریہ, vec);
      }
      اكسر;

      حالة vpiRealVal:
        aobj->سیٹ_لفظ(اشاریہ, vp->value.real);
        اكسر;

      حالة vpiStringVal:
        aobj->سیٹ_لفظ(اشاریہ, std::string(vp->value.str));
        اكسر;

      افتراضي:
          fprintf(stderr, "vpi sorry: format is not implemented");
          أكد(خطأ);
      }
}

vpiHandle __vpiDarrayVar::get_iter_اشاریہ(بنية __vpiArrayIterator*, صحيح idx)
{
      إذا (vals_الفاظ == 0) make_vals_الفاظ();

      أرجع &(vals_الفاظ[idx].as_لفظ);
}

صحيح __vpiDarrayVar::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiArrayType:
	    أرجع vpiDynamicArray;
	  حالة vpi_بایاں_Range:
	    أرجع 0;
	  حالة vpi_دایاں_Range:
	    أرجع get_ناپ() - 1;
	  حالة vpi_ناپ:
            أرجع get_ناپ();

	  افتراضي:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    أكد(خطأ);
	    أرجع 0;
      }
}

حرف* __vpiDarrayVar::vpi_get_str(صحيح code)
{
      إذا (code == vpiFile) {  // Not implemented for now!
            أرجع simple_سیٹ_rbuf_str(file_ناں[0]);
      }

      أرجع generic_get_str(code, scope_, name_, عدم);
}

vpiHandle __vpiDarrayVar::vpi_handle(صحيح code)
{
      بدل (code) {
          حالة vpi_بایاں_Range:
            أرجع get_بایاں_range();

          حالة vpi_دایاں_Range:
            أرجع get_دایاں_range();

          حالة vpiScope:
            أرجع scope_;

          حالة vpiModule:
            أرجع vpip_module(scope_);
      }

      أرجع 0;
}

vpiHandle __vpiDarrayVar::vpi_اشاریہ(صحيح اشاریہ)
{
      إذا (اشاریہ >= (لما) get_ناپ())
	    أرجع 0;
      إذا (اشاریہ < 0)
	    أرجع 0;

      إذا (vals_الفاظ == 0)
	    make_vals_الفاظ();

      أرجع &(vals_الفاظ[اشاریہ].as_لفظ);
}

فراغ __vpiDarrayVar::vpi_get_قدر(p_vpi_قدر val)
{
      val->format = vpiSuppressVal;
}

vvp_darray*__vpiDarrayVar::get_vvp_darray() ثابت
{
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      أكد(fun);
      vvp_object_t obj = fun->get_object();

      أرجع obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(ثابت حرف*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ثابت حرف*use_name = name ? vpip_name_string(name) : عدم;

      __vpiDarrayVar*obj = جديد __vpiDarrayVar(scope, use_name, net);

      أرجع obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, ثابت حرف*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

صحيح __vpiQueueVar::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiArrayType:
	    أرجع vpi_قطار_Array;
	  افتراضي:
	    أرجع __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_قطار_var(ثابت حرف*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ثابت حرف*use_name = name ? vpip_name_string(name) : عدم;

      __vpiQueueVar*obj = جديد __vpiQueueVar(scope, use_name, net);

      أرجع obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, بنانشانی pidx,
				     منطقي is_قطار)
: class_net_(0), prop_idx_(pidx), is_قطار_2(is_قطار), scope_(scope)
{
}

صحيح __vpiPropQueueRef::get_type_کوڈ(فراغ) ثابت
{ أرجع vpiArrayVar; }

ساكن vvp_darray* get_جیوندا_darray_from_prop(vvp_net_t* class_net, بنانشانی pid)
{
      إذا (!class_net) أرجع 0;
      ثابت vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      إذا (!fun) أرجع 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      إذا (!cobj) أرجع 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      أرجع qobj.peek<vvp_darray>();
}

صحيح __vpiPropQueueRef::vpi_get(صحيح code)
{
      ثابت vvp_darray* aobj = get_جیوندا_darray_from_prop(class_net_, prop_idx_);
      بدل (code) {
	  حالة vpiArrayType:
	    أرجع is_قطار_2 ? vpi_قطار_Array : vpiDynamicArray;
	  حالة vpi_بایاں_Range:
	    أرجع 0;
	  حالة vpi_دایاں_Range:
	    أرجع aobj ? (صحيح) (aobj->get_ناپ() - 1) : 0;
	  حالة vpi_ناپ:
	    أرجع aobj ? (صحيح) aobj->get_ناپ() : 0;
	  افتراضي:
	    أرجع 0;
      }
}

حرف* __vpiPropQueueRef::vpi_get_str(صحيح code)
{
      إذا (code == vpiFile) {
	    أرجع simple_سیٹ_rbuf_str(file_ناں[0]);
      }
      أرجع generic_get_str(code, scope_, "prop_darray", عدم);
}

فراغ __vpiPropQueueRef::vpi_get_قدر(p_vpi_قدر val)
{
      val->format = vpiSuppressVal;
}

vpiHandle vpip_make_prop_قطار_ref(حرف* class_label, بنانشانی prop_idx,
				   بنانشانی is_قطار_flag)
{
      __vpiPropQueueRef* obj = جديد __vpiPropQueueRef(vpip_peek_current_scope(),
						     prop_idx,
						     is_قطار_flag != 0);
      functor_ref_lookup(&obj->class_net_, class_label);
      أرجع obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      إذا (!ref) أرجع 0;

      إذا (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    أرجع get_جیوندا_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      إذا (ثابت __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    ثابت vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    إذا (!fun) أرجع 0;
	    vvp_object_t obj = fun->get_object();
	    أرجع obj.peek<vvp_darray>();
      }

      أرجع 0;
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ array_delete(vpiHandle شے)
{
      إذا (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(شے)) {
	    مٹاؤ pq;
	    أرجع;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(شے);
      إذا (dobj) {
	    إذا (dobj->vals_الفاظ) مٹاؤ [] (dobj->vals_الفاظ-1);
	    مٹاؤ dobj;
	    أرجع;
      }

      fprintf(stderr, "Need support for deleting array type: %d\n", شے->vpi_get(vpiArrayType));
      أكد(0);
}
#جے_انت
