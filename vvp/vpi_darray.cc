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

__vpiDarrayVar::__vpiDarrayVar(__vpiScope*sc, ध्रुव वर्ण*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

unsigned __vpiDarrayVar::get_size() ध्रुव
{
      ध्रुव vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      यदि(!fun)
        प्रत्यागम 0;

      vvp_object_t val = fun->get_object();
      ध्रुव vvp_darray*aval = val.peek<vvp_darray>();

      यदि(!aval)
        प्रत्यागम 0;

      प्रत्यागम aval->get_size();
}

vpiHandle __vpiDarrayVar::get_डावा_range()
{
      डावा_range.set_मूल्य(0);
      प्रत्यागम &डावा_range;
}

vpiHandle __vpiDarrayVar::get_उजवा_range()
{
      उजवा_range.set_मूल्य(get_size() - 1);
      प्रत्यागम &उजवा_range;
}

पूर्णाङ्क __vpiDarrayVar::get_word_size() ध्रुव
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_word(0, new_vec);
      प्रत्यागम new_vec.size();
}

वर्ण*__vpiDarrayVar::get_word_str(संरचना __vpiArrayWord*word, पूर्णाङ्क code)
{
      unsigned index = word->get_index();

      यदि (code == vpiFile) {  // Not implemented for now!
	    प्रत्यागम simple_set_rbuf_str(file_names[0]);
      }

      वर्ण sidx [64];
      snprintf(sidx, 63, "%d", (पूर्णाङ्क)index);
      प्रत्यागम generic_get_str(code, scope_, name_, sidx);
}

शून्य __vpiDarrayVar::get_word_मूल्य(संरचना __vpiArrayWord*word, p_vpi_मूल्य vp)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      यदि(vp->format == vpiObjTypeVal) {
          यदि(dynamic_cast<vvp_darray_real*>(aobj))
              vp->format = vpiRealVal;
          अन्यथा यदि(dynamic_cast<vvp_darray_string*>(aobj))
              vp->format = vpiStringVal;
          अन्यथा
              vp->format = vpiVectorVal;
      }

      विकल्प(vp->format) {
      पक्ष vpiBinStrVal:
      पक्ष vpiOctStrVal:
      पक्ष vpiDecStrVal:
      पक्ष vpiHexStrVal:
      पक्ष vpiScalarVal:
      पक्ष vpiIntVal:
      पक्ष vpiVectorVal:
      {
          vvp_vector4_t v;
          aobj->get_word(index, v);
          vpip_vec4_get_मूल्य(v, v.size(), असत्य, vp);  // TODO sign?
      }
      भङ्ग;

      पक्ष vpiRealVal:
      {
          द्विगुण d;
          aobj->get_word(index, d);
          vpip_real_get_मूल्य(d, vp);
      }
      भङ्ग;

      पक्ष vpiStringVal:
      {
          string s;
          aobj->get_word(index, s);
          vpip_string_get_मूल्य(s, vp);
      }
      भङ्ग;

      स्वभाव:
          fprintf(stderr, "vpi sorry: format is not implemented\n");
          प्रतिज्ञा(असत्य);
      }
}

शून्य __vpiDarrayVar::put_word_मूल्य(संरचना __vpiArrayWord*word, p_vpi_मूल्य vp, पूर्णाङ्क)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      विकल्प(vp->format) {
      पक्ष vpiScalarVal:
      पक्ष vpiIntVal:
      पक्ष vpiVectorVal: {
	    vvp_vector4_t vec = vec4_from_vpi_मूल्य(vp, get_word_size());
	    aobj->set_word(index, vec);
      }
      भङ्ग;

      पक्ष vpiRealVal:
        aobj->set_word(index, vp->value.real);
        भङ्ग;

      पक्ष vpiStringVal:
        aobj->set_word(index, std::string(vp->value.str));
        भङ्ग;

      स्वभाव:
          fprintf(stderr, "vpi sorry: format is not implemented");
          प्रतिज्ञा(असत्य);
      }
}

vpiHandle __vpiDarrayVar::get_iter_index(संरचना __vpiArrayIterator*, पूर्णाङ्क idx)
{
      यदि (vals_शब्द == 0) make_vals_शब्द();

      प्रत्यागम &(vals_शब्द[idx].as_word);
}

पूर्णाङ्क __vpiDarrayVar::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiArrayType:
	    प्रत्यागम vpiDynamicArray;
	  पक्ष vpi_डावा_Range:
	    प्रत्यागम 0;
	  पक्ष vpi_उजवा_Range:
	    प्रत्यागम get_size() - 1;
	  पक्ष vpiSize:
            प्रत्यागम get_size();

	  स्वभाव:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    प्रतिज्ञा(असत्य);
	    प्रत्यागम 0;
      }
}

वर्ण* __vpiDarrayVar::vpi_get_str(पूर्णाङ्क code)
{
      यदि (code == vpiFile) {  // Not implemented for now!
            प्रत्यागम simple_set_rbuf_str(file_names[0]);
      }

      प्रत्यागम generic_get_str(code, scope_, name_, रिक्त);
}

vpiHandle __vpiDarrayVar::vpi_handle(पूर्णाङ्क code)
{
      विकल्प (code) {
          पक्ष vpi_डावा_Range:
            प्रत्यागम get_डावा_range();

          पक्ष vpi_उजवा_Range:
            प्रत्यागम get_उजवा_range();

          पक्ष vpiScope:
            प्रत्यागम scope_;

          पक्ष vpiModule:
            प्रत्यागम vpip_module(scope_);
      }

      प्रत्यागम 0;
}

vpiHandle __vpiDarrayVar::vpi_index(पूर्णाङ्क index)
{
      यदि (index >= (long) get_size())
	    प्रत्यागम 0;
      यदि (index < 0)
	    प्रत्यागम 0;

      यदि (vals_शब्द == 0)
	    make_vals_शब्द();

      प्रत्यागम &(vals_शब्द[index].as_word);
}

शून्य __vpiDarrayVar::vpi_get_मूल्य(p_vpi_मूल्य val)
{
      val->format = vpiSuppressVal;
}

vvp_darray*__vpiDarrayVar::get_vvp_darray() ध्रुव
{
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      प्रतिज्ञा(fun);
      vvp_object_t obj = fun->get_object();

      प्रत्यागम obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(ध्रुव वर्ण*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ध्रुव वर्ण*use_name = name ? vpip_name_string(name) : रिक्त;

      __vpiDarrayVar*obj = नूतन __vpiDarrayVar(scope, use_name, net);

      प्रत्यागम obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, ध्रुव वर्ण*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

पूर्णाङ्क __vpiQueueVar::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiArrayType:
	    प्रत्यागम vpiQueueArray;
	  स्वभाव:
	    प्रत्यागम __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_queue_var(ध्रुव वर्ण*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ध्रुव वर्ण*use_name = name ? vpip_name_string(name) : रिक्त;

      __vpiQueueVar*obj = नूतन __vpiQueueVar(scope, use_name, net);

      प्रत्यागम obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, unsigned pidx,
				     तार्किक is_queue)
: class_net_(0), prop_idx_(pidx), is_queue_(is_queue), scope_(scope)
{
}

पूर्णाङ्क __vpiPropQueueRef::get_type_code(शून्य) ध्रुव
{ प्रत्यागम vpiArrayVar; }

स्थिर vvp_darray* get_live_darray_from_prop(vvp_net_t* class_net, unsigned pid)
{
      यदि (!class_net) प्रत्यागम 0;
      ध्रुव vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      यदि (!fun) प्रत्यागम 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      यदि (!cobj) प्रत्यागम 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      प्रत्यागम qobj.peek<vvp_darray>();
}

पूर्णाङ्क __vpiPropQueueRef::vpi_get(पूर्णाङ्क code)
{
      ध्रुव vvp_darray* aobj = get_live_darray_from_prop(class_net_, prop_idx_);
      विकल्प (code) {
	  पक्ष vpiArrayType:
	    प्रत्यागम is_queue_ ? vpiQueueArray : vpiDynamicArray;
	  पक्ष vpi_डावा_Range:
	    प्रत्यागम 0;
	  पक्ष vpi_उजवा_Range:
	    प्रत्यागम aobj ? (पूर्णाङ्क) (aobj->get_size() - 1) : 0;
	  पक्ष vpiSize:
	    प्रत्यागम aobj ? (पूर्णाङ्क) aobj->get_size() : 0;
	  स्वभाव:
	    प्रत्यागम 0;
      }
}

वर्ण* __vpiPropQueueRef::vpi_get_str(पूर्णाङ्क code)
{
      यदि (code == vpiFile) {
	    प्रत्यागम simple_set_rbuf_str(file_names[0]);
      }
      प्रत्यागम generic_get_str(code, scope_, "prop_darray", रिक्त);
}

शून्य __vpiPropQueueRef::vpi_get_मूल्य(p_vpi_मूल्य val)
{
      val->format = vpiSuppressVal;
}

vpiHandle vpip_make_prop_queue_ref(वर्ण* class_label, unsigned prop_idx,
				   unsigned is_queue_flag)
{
      __vpiPropQueueRef* obj = नूतन __vpiPropQueueRef(vpip_peek_current_scope(),
						     prop_idx,
						     is_queue_flag != 0);
      functor_ref_lookup(&obj->class_net_, class_label);
      प्रत्यागम obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      यदि (!ref) प्रत्यागम 0;

      यदि (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    प्रत्यागम get_live_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      यदि (ध्रुव __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    ध्रुव vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    यदि (!fun) प्रत्यागम 0;
	    vvp_object_t obj = fun->get_object();
	    प्रत्यागम obj.peek<vvp_darray>();
      }

      प्रत्यागम 0;
}

#ifdef CHECK_WITH_VALGRIND
शून्य array_delete(vpiHandle item)
{
      यदि (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(item)) {
	    delete pq;
	    प्रत्यागम;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(item);
      यदि (dobj) {
	    यदि (dobj->vals_शब्द) delete [] (dobj->vals_शब्द-1);
	    delete dobj;
	    प्रत्यागम;
      }

      fprintf(stderr, "Need support for deleting array type: %d\n", item->vpi_get(vpiArrayType));
      प्रतिज्ञा(0);
}
#endif
