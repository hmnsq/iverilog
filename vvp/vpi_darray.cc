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

using فضاینام std;

__vpiDarrayVar::__vpiDarrayVar(__vpiScope*sc, ثابت نویسه*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

unsigned __vpiDarrayVar::get_size() ثابت
{
      ثابت vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      اگر(!fun)
        بازگشت 0;

      vvp_object_t val = fun->get_object();
      ثابت vvp_darray*aval = val.peek<vvp_darray>();

      اگر(!aval)
        بازگشت 0;

      بازگشت aval->get_size();
}

vpiHandle __vpiDarrayVar::get_چپ_range()
{
      چپ_range.set_مقدار(0);
      بازگشت &چپ_range;
}

vpiHandle __vpiDarrayVar::get_راست_range()
{
      راست_range.set_مقدار(get_size() - 1);
      بازگشت &راست_range;
}

صحیح __vpiDarrayVar::get_word_size() ثابت
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_word(0, new_vec);
      بازگشت new_vec.size();
}

نویسه*__vpiDarrayVar::get_word_str(ساختار __vpiArrayWord*word, صحیح code)
{
      unsigned index = word->get_index();

      اگر (code == vpiFile) {  // Not implemented for now!
	    بازگشت simple_set_rbuf_str(file_names[0]);
      }

      نویسه sidx [64];
      snprintf(sidx, 63, "%d", (صحیح)index);
      بازگشت generic_get_str(code, scope_, name_, sidx);
}

تهی __vpiDarrayVar::get_word_مقدار(ساختار __vpiArrayWord*word, p_vpi_مقدار vp)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      اگر(vp->format == vpiObjTypeVal) {
          اگر(dynamic_cast<vvp_darray_real*>(aobj))
              vp->format = vpiRealVal;
          وگرنه اگر(dynamic_cast<vvp_darray_string*>(aobj))
              vp->format = vpiStringVal;
          وگرنه
              vp->format = vpiVectorVal;
      }

      انتخاب(vp->format) {
      حالت vpiBinStrVal:
      حالت vpiOctStrVal:
      حالت vpiDecStrVal:
      حالت vpiHexStrVal:
      حالت vpiScalarVal:
      حالت vpiIntVal:
      حالت vpiVectorVal:
      {
          vvp_vector4_t v;
          aobj->get_word(index, v);
          vpip_vec4_get_مقدار(v, v.size(), نادرست, vp);  // TODO sign?
      }
      بشکن;

      حالت vpiRealVal:
      {
          دوگانه d;
          aobj->get_word(index, d);
          vpip_real_get_مقدار(d, vp);
      }
      بشکن;

      حالت vpiStringVal:
      {
          string s;
          aobj->get_word(index, s);
          vpip_string_get_مقدار(s, vp);
      }
      بشکن;

      پیشفرض:
          fprintf(stderr, "vpi sorry: format is not implemented\n");
          أكد(نادرست);
      }
}

تهی __vpiDarrayVar::put_word_مقدار(ساختار __vpiArrayWord*word, p_vpi_مقدار vp, صحیح)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      انتخاب(vp->format) {
      حالت vpiScalarVal:
      حالت vpiIntVal:
      حالت vpiVectorVal: {
	    vvp_vector4_t vec = vec4_from_vpi_مقدار(vp, get_word_size());
	    aobj->set_word(index, vec);
      }
      بشکن;

      حالت vpiRealVal:
        aobj->set_word(index, vp->value.real);
        بشکن;

      حالت vpiStringVal:
        aobj->set_word(index, std::string(vp->value.str));
        بشکن;

      پیشفرض:
          fprintf(stderr, "vpi sorry: format is not implemented");
          أكد(نادرست);
      }
}

vpiHandle __vpiDarrayVar::get_iter_index(ساختار __vpiArrayIterator*, صحیح idx)
{
      اگر (vals_واژگان == 0) make_vals_واژگان();

      بازگشت &(vals_واژگان[idx].as_word);
}

صحیح __vpiDarrayVar::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpiArrayType:
	    بازگشت vpiDynamicArray;
	  حالت vpi_چپ_Range:
	    بازگشت 0;
	  حالت vpi_راست_Range:
	    بازگشت get_size() - 1;
	  حالت vpiSize:
            بازگشت get_size();

	  پیشفرض:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    أكد(نادرست);
	    بازگشت 0;
      }
}

نویسه* __vpiDarrayVar::vpi_get_str(صحیح code)
{
      اگر (code == vpiFile) {  // Not implemented for now!
            بازگشت simple_set_rbuf_str(file_names[0]);
      }

      بازگشت generic_get_str(code, scope_, name_, پوچ);
}

vpiHandle __vpiDarrayVar::vpi_handle(صحیح code)
{
      انتخاب (code) {
          حالت vpi_چپ_Range:
            بازگشت get_چپ_range();

          حالت vpi_راست_Range:
            بازگشت get_راست_range();

          حالت vpiScope:
            بازگشت scope_;

          حالت vpiModule:
            بازگشت vpip_module(scope_);
      }

      بازگشت 0;
}

vpiHandle __vpiDarrayVar::vpi_index(صحیح index)
{
      اگر (index >= (long) get_size())
	    بازگشت 0;
      اگر (index < 0)
	    بازگشت 0;

      اگر (vals_واژگان == 0)
	    make_vals_واژگان();

      بازگشت &(vals_واژگان[index].as_word);
}

تهی __vpiDarrayVar::vpi_get_مقدار(p_vpi_مقدار val)
{
      val->format = vpiSuppressVal;
}

vvp_darray*__vpiDarrayVar::get_vvp_darray() ثابت
{
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      أكد(fun);
      vvp_object_t obj = fun->get_object();

      بازگشت obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(ثابت نویسه*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ثابت نویسه*use_name = name ? vpip_name_string(name) : پوچ;

      __vpiDarrayVar*obj = جدید __vpiDarrayVar(scope, use_name, net);

      بازگشت obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, ثابت نویسه*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

صحیح __vpiQueueVar::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpiArrayType:
	    بازگشت vpiQueueArray;
	  پیشفرض:
	    بازگشت __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_queue_var(ثابت نویسه*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ثابت نویسه*use_name = name ? vpip_name_string(name) : پوچ;

      __vpiQueueVar*obj = جدید __vpiQueueVar(scope, use_name, net);

      بازگشت obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, unsigned pidx,
				     منطقی is_queue)
: class_net_(0), prop_idx_(pidx), is_queue_(is_queue), scope_(scope)
{
}

صحیح __vpiPropQueueRef::get_type_code(تهی) ثابت
{ بازگشت vpiArrayVar; }

ایستا vvp_darray* get_live_darray_from_prop(vvp_net_t* class_net, unsigned pid)
{
      اگر (!class_net) بازگشت 0;
      ثابت vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      اگر (!fun) بازگشت 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      اگر (!cobj) بازگشت 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      بازگشت qobj.peek<vvp_darray>();
}

صحیح __vpiPropQueueRef::vpi_get(صحیح code)
{
      ثابت vvp_darray* aobj = get_live_darray_from_prop(class_net_, prop_idx_);
      انتخاب (code) {
	  حالت vpiArrayType:
	    بازگشت is_queue_ ? vpiQueueArray : vpiDynamicArray;
	  حالت vpi_چپ_Range:
	    بازگشت 0;
	  حالت vpi_راست_Range:
	    بازگشت aobj ? (صحیح) (aobj->get_size() - 1) : 0;
	  حالت vpiSize:
	    بازگشت aobj ? (صحیح) aobj->get_size() : 0;
	  پیشفرض:
	    بازگشت 0;
      }
}

نویسه* __vpiPropQueueRef::vpi_get_str(صحیح code)
{
      اگر (code == vpiFile) {
	    بازگشت simple_set_rbuf_str(file_names[0]);
      }
      بازگشت generic_get_str(code, scope_, "prop_darray", پوچ);
}

تهی __vpiPropQueueRef::vpi_get_مقدار(p_vpi_مقدار val)
{
      val->format = vpiSuppressVal;
}

vpiHandle vpip_make_prop_queue_ref(نویسه* class_label, unsigned prop_idx,
				   unsigned is_queue_flag)
{
      __vpiPropQueueRef* obj = جدید __vpiPropQueueRef(vpip_peek_current_scope(),
						     prop_idx,
						     is_queue_flag != 0);
      functor_ref_lookup(&obj->class_net_, class_label);
      بازگشت obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      اگر (!ref) بازگشت 0;

      اگر (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    بازگشت get_live_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      اگر (ثابت __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    ثابت vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    اگر (!fun) بازگشت 0;
	    vvp_object_t obj = fun->get_object();
	    بازگشت obj.peek<vvp_darray>();
      }

      بازگشت 0;
}

#ifdef CHECK_WITH_VALGRIND
تهی array_delete(vpiHandle item)
{
      اگر (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(item)) {
	    delete pq;
	    بازگشت;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(item);
      اگر (dobj) {
	    اگر (dobj->vals_واژگان) delete [] (dobj->vals_واژگان-1);
	    delete dobj;
	    بازگشت;
      }

      fprintf(stderr, "Need support for deleting array type: %d\n", item->vpi_get(vpiArrayType));
      أكد(0);
}
#endif
