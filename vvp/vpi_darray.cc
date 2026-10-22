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
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

using نطاق std;

__vpiDarrayVar::__vpiDarrayVar(__vpiScope*sc, ثابت حرف*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

unsigned __vpiDarrayVar::get_size() ثابت
{
      ثابت vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      إذا(!fun)
        إرجاع 0;

      vvp_object_t val = fun->get_object();
      ثابت vvp_darray*aval = val.peek<vvp_darray>();

      إذا(!aval)
        إرجاع 0;

      إرجاع aval->get_size();
}

vpiHandle __vpiDarrayVar::get_يسار_range()
{
      يسار_range.set_قيمة(0);
      إرجاع &يسار_range;
}

vpiHandle __vpiDarrayVar::get_يمين_range()
{
      يمين_range.set_قيمة(get_size() - 1);
      إرجاع &يمين_range;
}

صحيح __vpiDarrayVar::get_word_size() ثابت
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_word(0, new_vec);
      إرجاع new_vec.size();
}

حرف*__vpiDarrayVar::get_word_str(بنية __vpiArrayWord*word, صحيح code)
{
      unsigned index = word->get_index();

      إذا (code == vpiFile) {  // Not implemented for now!
	    إرجاع simple_set_rbuf_str(file_names[0]);
      }

      حرف sidx [64];
      snprintf(sidx, 63, "%d", (صحيح)index);
      إرجاع generic_get_str(code, scope_, name_, sidx);
}

فراغ __vpiDarrayVar::get_word_قيمة(بنية __vpiArrayWord*word, p_vpi_قيمة vp)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      إذا(vp->format == vpiObjTypeVal) {
          إذا(dynamic_cast<vvp_darray_real*>(aobj))
              vp->format = vpiRealVal;
          وإلا إذا(dynamic_cast<vvp_darray_string*>(aobj))
              vp->format = vpiStringVal;
          وإلا
              vp->format = vpiVectorVal;
      }

      بدل(vp->format) {
      حالة vpiBinStrVal:
      حالة vpiOctStrVal:
      حالة vpiDecStrVal:
      حالة vpiHexStrVal:
      حالة vpiScalarVal:
      حالة vpiIntVal:
      حالة vpiVectorVal:
      {
          vvp_vector4_t v;
          aobj->get_word(index, v);
          vpip_vec4_get_قيمة(v, v.size(), خطأ, vp);  // TODO sign?
      }
      اكسر;

      حالة vpiRealVal:
      {
          مزدوج d;
          aobj->get_word(index, d);
          vpip_real_get_قيمة(d, vp);
      }
      اكسر;

      حالة vpiStringVal:
      {
          string s;
          aobj->get_word(index, s);
          vpip_string_get_قيمة(s, vp);
      }
      اكسر;

      افتراضي:
          fprintf(stderr, "vpi sorry: format is not implemented\n");
          أكد(خطأ);
      }
}

فراغ __vpiDarrayVar::put_word_قيمة(بنية __vpiArrayWord*word, p_vpi_قيمة vp, صحيح)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      بدل(vp->format) {
      حالة vpiScalarVal:
      حالة vpiIntVal:
      حالة vpiVectorVal: {
	    vvp_vector4_t vec = vec4_from_vpi_قيمة(vp, get_word_size());
	    aobj->set_word(index, vec);
      }
      اكسر;

      حالة vpiRealVal:
        aobj->set_word(index, vp->value.real);
        اكسر;

      حالة vpiStringVal:
        aobj->set_word(index, std::string(vp->value.str));
        اكسر;

      افتراضي:
          fprintf(stderr, "vpi sorry: format is not implemented");
          أكد(خطأ);
      }
}

vpiHandle __vpiDarrayVar::get_iter_index(بنية __vpiArrayIterator*, صحيح idx)
{
      إذا (vals_كلمات == 0) make_vals_كلمات();

      إرجاع &(vals_كلمات[idx].as_word);
}

صحيح __vpiDarrayVar::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiArrayType:
	    إرجاع vpiDynamicArray;
	  حالة vpi_يسار_Range:
	    إرجاع 0;
	  حالة vpi_يمين_Range:
	    إرجاع get_size() - 1;
	  حالة vpiSize:
            إرجاع get_size();

	  افتراضي:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    أكد(خطأ);
	    إرجاع 0;
      }
}

حرف* __vpiDarrayVar::vpi_get_str(صحيح code)
{
      إذا (code == vpiFile) {  // Not implemented for now!
            إرجاع simple_set_rbuf_str(file_names[0]);
      }

      إرجاع generic_get_str(code, scope_, name_, عدم);
}

vpiHandle __vpiDarrayVar::vpi_handle(صحيح code)
{
      بدل (code) {
          حالة vpi_يسار_Range:
            إرجاع get_يسار_range();

          حالة vpi_يمين_Range:
            إرجاع get_يمين_range();

          حالة vpiScope:
            إرجاع scope_;

          حالة vpiModule:
            إرجاع vpip_module(scope_);
      }

      إرجاع 0;
}

vpiHandle __vpiDarrayVar::vpi_index(صحيح index)
{
      إذا (index >= (long) get_size())
	    إرجاع 0;
      إذا (index < 0)
	    إرجاع 0;

      إذا (vals_كلمات == 0)
	    make_vals_كلمات();

      إرجاع &(vals_كلمات[index].as_word);
}

فراغ __vpiDarrayVar::vpi_get_قيمة(p_vpi_قيمة val)
{
      val->format = vpiSuppressVal;
}

vvp_darray*__vpiDarrayVar::get_vvp_darray() ثابت
{
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      أكد(fun);
      vvp_object_t obj = fun->get_object();

      إرجاع obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(ثابت حرف*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ثابت حرف*use_name = name ? vpip_name_string(name) : عدم;

      __vpiDarrayVar*obj = جديد __vpiDarrayVar(scope, use_name, net);

      إرجاع obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, ثابت حرف*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

صحيح __vpiQueueVar::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiArrayType:
	    إرجاع vpiQueueArray;
	  افتراضي:
	    إرجاع __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_queue_var(ثابت حرف*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ثابت حرف*use_name = name ? vpip_name_string(name) : عدم;

      __vpiQueueVar*obj = جديد __vpiQueueVar(scope, use_name, net);

      إرجاع obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, unsigned pidx,
				     منطقي is_queue)
: class_net_(0), prop_idx_(pidx), is_queue_(is_queue), scope_(scope)
{
}

صحيح __vpiPropQueueRef::get_type_code(فراغ) ثابت
{ إرجاع vpiArrayVar; }

ساكن vvp_darray* get_live_darray_from_prop(vvp_net_t* class_net, unsigned pid)
{
      إذا (!class_net) إرجاع 0;
      ثابت vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      إذا (!fun) إرجاع 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      إذا (!cobj) إرجاع 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      إرجاع qobj.peek<vvp_darray>();
}

صحيح __vpiPropQueueRef::vpi_get(صحيح code)
{
      ثابت vvp_darray* aobj = get_live_darray_from_prop(class_net_, prop_idx_);
      بدل (code) {
	  حالة vpiArrayType:
	    إرجاع is_queue_ ? vpiQueueArray : vpiDynamicArray;
	  حالة vpi_يسار_Range:
	    إرجاع 0;
	  حالة vpi_يمين_Range:
	    إرجاع aobj ? (صحيح) (aobj->get_size() - 1) : 0;
	  حالة vpiSize:
	    إرجاع aobj ? (صحيح) aobj->get_size() : 0;
	  افتراضي:
	    إرجاع 0;
      }
}

حرف* __vpiPropQueueRef::vpi_get_str(صحيح code)
{
      إذا (code == vpiFile) {
	    إرجاع simple_set_rbuf_str(file_names[0]);
      }
      إرجاع generic_get_str(code, scope_, "prop_darray", عدم);
}

فراغ __vpiPropQueueRef::vpi_get_قيمة(p_vpi_قيمة val)
{
      val->format = vpiSuppressVal;
}

vpiHandle vpip_make_prop_queue_ref(حرف* class_label, unsigned prop_idx,
				   unsigned is_queue_flag)
{
      __vpiPropQueueRef* obj = جديد __vpiPropQueueRef(vpip_peek_current_scope(),
						     prop_idx,
						     is_queue_flag != 0);
      functor_ref_lookup(&obj->class_net_, class_label);
      إرجاع obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      إذا (!ref) إرجاع 0;

      إذا (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    إرجاع get_live_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      إذا (ثابت __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    ثابت vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    إذا (!fun) إرجاع 0;
	    vvp_object_t obj = fun->get_object();
	    إرجاع obj.peek<vvp_darray>();
      }

      إرجاع 0;
}

#ifdef CHECK_WITH_VALGRIND
فراغ array_delete(vpiHandle item)
{
      إذا (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(item)) {
	    delete pq;
	    إرجاع;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(item);
      إذا (dobj) {
	    إذا (dobj->vals_كلمات) delete [] (dobj->vals_كلمات-1);
	    delete dobj;
	    إرجاع;
      }

      fprintf(stderr, "Need support for deleting array type: %d\n", item->vpi_get(vpiArrayType));
      أكد(0);
}
#endif
