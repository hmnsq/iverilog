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

using नामाकाश std;

__vpiDarrayVar::__vpiDarrayVar(__vpiScope*sc, स्थिरांक अक्षर*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

अचिन्हित __vpiDarrayVar::get_size() स्थिरांक
{
      स्थिरांक vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      जर(!fun)
        परत 0;

      vvp_object_t val = fun->get_object();
      स्थिरांक vvp_darray*aval = val.peek<vvp_darray>();

      जर(!aval)
        परत 0;

      परत aval->get_size();
}

vpiHandle __vpiDarrayVar::get_डावा_range()
{
      डावा_range.set_मूल्य(0);
      परत &डावा_range;
}

vpiHandle __vpiDarrayVar::get_उजवा_range()
{
      उजवा_range.set_मूल्य(get_size() - 1);
      परत &उजवा_range;
}

पूर्णांक __vpiDarrayVar::get_word_size() स्थिरांक
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_word(0, new_vec);
      परत new_vec.size();
}

अक्षर*__vpiDarrayVar::get_word_str(संरचना __vpiArrayWord*word, पूर्णांक code)
{
      अचिन्हित index = word->get_index();

      जर (code == vpiFile) {  // Not implemented for now!
	    परत simple_set_rbuf_str(file_names[0]);
      }

      अक्षर sidx [64];
      snprintf(sidx, 63, "%d", (पूर्णांक)index);
      परत generic_get_str(code, scope_, name_, sidx);
}

शून्य __vpiDarrayVar::get_word_मूल्य(संरचना __vpiArrayWord*word, p_vpi_मूल्य vp)
{
      अचिन्हित index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      जर(vp->format == vpiObjTypeVal) {
          जर(dynamic_cast<vvp_darray_real*>(aobj))
              vp->format = vpiRealVal;
          अन्यथा जर(dynamic_cast<vvp_darray_string*>(aobj))
              vp->format = vpiStringVal;
          अन्यथा
              vp->format = vpiVectorVal;
      }

      निवड(vp->format) {
      स्थिती vpiBinStrVal:
      स्थिती vpiOctStrVal:
      स्थिती vpiDecStrVal:
      स्थिती vpiHexStrVal:
      स्थिती vpiScalarVal:
      स्थिती vpiIntVal:
      स्थिती vpiVectorVal:
      {
          vvp_vector4_t v;
          aobj->get_word(index, v);
          vpip_vec4_get_मूल्य(v, v.size(), असत्य, vp);  // TODO sign?
      }
      थांबा;

      स्थिती vpiRealVal:
      {
          द्विगुण d;
          aobj->get_word(index, d);
          vpip_real_get_मूल्य(d, vp);
      }
      थांबा;

      स्थिती vpiStringVal:
      {
          string s;
          aobj->get_word(index, s);
          vpip_string_get_मूल्य(s, vp);
      }
      थांबा;

      पूर्वनिर्धारित:
          ख_लिखो(मानक_त्रुटि, "vpi sorry: format is not implemented\n");
          प्रतिज्ञा(असत्य);
      }
}

शून्य __vpiDarrayVar::put_word_मूल्य(संरचना __vpiArrayWord*word, p_vpi_मूल्य vp, पूर्णांक)
{
      अचिन्हित index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      निवड(vp->format) {
      स्थिती vpiScalarVal:
      स्थिती vpiIntVal:
      स्थिती vpiVectorVal: {
	    vvp_vector4_t vec = vec4_from_vpi_मूल्य(vp, get_word_size());
	    aobj->set_word(index, vec);
      }
      थांबा;

      स्थिती vpiRealVal:
        aobj->set_word(index, vp->value.real);
        थांबा;

      स्थिती vpiStringVal:
        aobj->set_word(index, std::string(vp->value.str));
        थांबा;

      पूर्वनिर्धारित:
          ख_लिखो(मानक_त्रुटि, "vpi sorry: format is not implemented");
          प्रतिज्ञा(असत्य);
      }
}

vpiHandle __vpiDarrayVar::get_iter_index(संरचना __vpiArrayIterator*, पूर्णांक idx)
{
      जर (vals_शब्द == 0) make_vals_शब्द();

      परत &(vals_शब्द[idx].as_word);
}

पूर्णांक __vpiDarrayVar::vpi_get(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpiArrayType:
	    परत vpiDynamicArray;
	  स्थिती vpi_डावा_Range:
	    परत 0;
	  स्थिती vpi_उजवा_Range:
	    परत get_size() - 1;
	  स्थिती vpiSize:
            परत get_size();

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "vpi sorry: property is not implemented");
	    प्रतिज्ञा(असत्य);
	    परत 0;
      }
}

अक्षर* __vpiDarrayVar::vpi_get_str(पूर्णांक code)
{
      जर (code == vpiFile) {  // Not implemented for now!
            परत simple_set_rbuf_str(file_names[0]);
      }

      परत generic_get_str(code, scope_, name_, रिक्त);
}

vpiHandle __vpiDarrayVar::vpi_handle(पूर्णांक code)
{
      निवड (code) {
          स्थिती vpi_डावा_Range:
            परत get_डावा_range();

          स्थिती vpi_उजवा_Range:
            परत get_उजवा_range();

          स्थिती vpiScope:
            परत scope_;

          स्थिती vpiModule:
            परत vpip_module(scope_);
      }

      परत 0;
}

vpiHandle __vpiDarrayVar::vpi_index(पूर्णांक index)
{
      जर (index >= (दीर्घ) get_size())
	    परत 0;
      जर (index < 0)
	    परत 0;

      जर (vals_शब्द == 0)
	    make_vals_शब्द();

      परत &(vals_शब्द[index].as_word);
}

शून्य __vpiDarrayVar::vpi_get_मूल्य(p_vpi_मूल्य val)
{
      val->format = vpiSuppressVal;
}

vvp_darray*__vpiDarrayVar::get_vvp_darray() स्थिरांक
{
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      प्रतिज्ञा(fun);
      vvp_object_t obj = fun->get_object();

      परत obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(स्थिरांक अक्षर*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      स्थिरांक अक्षर*use_name = name ? vpip_name_string(name) : रिक्त;

      __vpiDarrayVar*obj = नवीन __vpiDarrayVar(scope, use_name, net);

      परत obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, स्थिरांक अक्षर*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

पूर्णांक __vpiQueueVar::vpi_get(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpiArrayType:
	    परत vpiQueueArray;
	  पूर्वनिर्धारित:
	    परत __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_queue_var(स्थिरांक अक्षर*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      स्थिरांक अक्षर*use_name = name ? vpip_name_string(name) : रिक्त;

      __vpiQueueVar*obj = नवीन __vpiQueueVar(scope, use_name, net);

      परत obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, अचिन्हित pidx,
				     तार्किक is_queue)
: class_net_(0), prop_idx_(pidx), is_queue_(is_queue), scope_(scope)
{
}

पूर्णांक __vpiPropQueueRef::get_type_code(शून्य) स्थिरांक
{ परत vpiArrayVar; }

स्थिर vvp_darray* get_live_darray_from_prop(vvp_net_t* class_net, अचिन्हित pid)
{
      जर (!class_net) परत 0;
      स्थिरांक vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      जर (!fun) परत 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      जर (!cobj) परत 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      परत qobj.peek<vvp_darray>();
}

पूर्णांक __vpiPropQueueRef::vpi_get(पूर्णांक code)
{
      स्थिरांक vvp_darray* aobj = get_live_darray_from_prop(class_net_, prop_idx_);
      निवड (code) {
	  स्थिती vpiArrayType:
	    परत is_queue_ ? vpiQueueArray : vpiDynamicArray;
	  स्थिती vpi_डावा_Range:
	    परत 0;
	  स्थिती vpi_उजवा_Range:
	    परत aobj ? (पूर्णांक) (aobj->get_size() - 1) : 0;
	  स्थिती vpiSize:
	    परत aobj ? (पूर्णांक) aobj->get_size() : 0;
	  पूर्वनिर्धारित:
	    परत 0;
      }
}

अक्षर* __vpiPropQueueRef::vpi_get_str(पूर्णांक code)
{
      जर (code == vpiFile) {
	    परत simple_set_rbuf_str(file_names[0]);
      }
      परत generic_get_str(code, scope_, "prop_darray", रिक्त);
}

शून्य __vpiPropQueueRef::vpi_get_मूल्य(p_vpi_मूल्य val)
{
      val->format = vpiSuppressVal;
}

vpiHandle vpip_make_prop_queue_ref(अक्षर* class_label, अचिन्हित prop_idx,
				   अचिन्हित is_queue_flag)
{
      __vpiPropQueueRef* obj = नवीन __vpiPropQueueRef(vpip_peek_current_scope(),
						     prop_idx,
						     is_queue_flag != 0);
      functor_ref_lookup(&obj->class_net_, class_label);
      परत obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      जर (!ref) परत 0;

      जर (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    परत get_live_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      जर (स्थिरांक __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    स्थिरांक vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    जर (!fun) परत 0;
	    vvp_object_t obj = fun->get_object();
	    परत obj.peek<vvp_darray>();
      }

      परत 0;
}

#ifdef CHECK_WITH_VALGRIND
शून्य array_delete(vpiHandle item)
{
      जर (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(item)) {
	    delete pq;
	    परत;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(item);
      जर (dobj) {
	    जर (dobj->vals_शब्द) delete [] (dobj->vals_शब्द-1);
	    delete dobj;
	    परत;
      }

      ख_लिखो(मानक_त्रुटि, "Need support for deleting array type: %d\n", item->vpi_get(vpiArrayType));
      प्रतिज्ञा(0);
}
#endif
