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

__vpiDarrayVar::__vpiDarrayVar(__vpiScope*sc, अचर वर्ण*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

अचिन्हित __vpiDarrayVar::get_size() अचर
{
      अचर vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      यदि(!fun)
        फर्काऊ 0;

      vvp_object_t val = fun->get_object();
      अचर vvp_darray*aval = val.peek<vvp_darray>();

      यदि(!aval)
        फर्काऊ 0;

      फर्काऊ aval->get_size();
}

vpiHandle __vpiDarrayVar::get_बायाँ_range()
{
      बायाँ_range.set_मान(0);
      फर्काऊ &बायाँ_range;
}

vpiHandle __vpiDarrayVar::get_दायाँ_range()
{
      दायाँ_range.set_मान(get_size() - 1);
      फर्काऊ &दायाँ_range;
}

पूर्णाङ्क __vpiDarrayVar::get_word_size() अचर
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_word(0, new_vec);
      फर्काऊ new_vec.size();
}

वर्ण*__vpiDarrayVar::get_word_str(संरचना __vpiArrayWord*word, पूर्णाङ्क code)
{
      अचिन्हित index = word->get_index();

      यदि (code == vpiFile) {  // Not implemented for now!
	    फर्काऊ simple_set_rbuf_str(file_names[0]);
      }

      वर्ण sidx [64];
      snprintf(sidx, 63, "%d", (पूर्णाङ्क)index);
      फर्काऊ generic_get_str(code, scope_, name_, sidx);
}

शून्य __vpiDarrayVar::get_word_मान(संरचना __vpiArrayWord*word, p_vpi_मान vp)
{
      अचिन्हित index = word->get_index();
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
          vpip_vec4_get_मान(v, v.size(), असत्य, vp);  // TODO sign?
      }
      रोक;

      पक्ष vpiRealVal:
      {
          द्विगुण d;
          aobj->get_word(index, d);
          vpip_real_get_मान(d, vp);
      }
      रोक;

      पक्ष vpiStringVal:
      {
          string s;
          aobj->get_word(index, s);
          vpip_string_get_मान(s, vp);
      }
      रोक;

      स्वभाव:
          ख_लिखो(मानक_त्रुटि, "vpi sorry: format is not implemented\n");
          प्रतिज्ञा(असत्य);
      }
}

शून्य __vpiDarrayVar::put_word_मान(संरचना __vpiArrayWord*word, p_vpi_मान vp, पूर्णाङ्क)
{
      अचिन्हित index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      विकल्प(vp->format) {
      पक्ष vpiScalarVal:
      पक्ष vpiIntVal:
      पक्ष vpiVectorVal: {
	    vvp_vector4_t vec = vec4_from_vpi_मान(vp, get_word_size());
	    aobj->set_word(index, vec);
      }
      रोक;

      पक्ष vpiRealVal:
        aobj->set_word(index, vp->value.real);
        रोक;

      पक्ष vpiStringVal:
        aobj->set_word(index, std::string(vp->value.str));
        रोक;

      स्वभाव:
          ख_लिखो(मानक_त्रुटि, "vpi sorry: format is not implemented");
          प्रतिज्ञा(असत्य);
      }
}

vpiHandle __vpiDarrayVar::get_iter_index(संरचना __vpiArrayIterator*, पूर्णाङ्क idx)
{
      यदि (vals_शब्दहरू == 0) make_vals_शब्दहरू();

      फर्काऊ &(vals_शब्दहरू[idx].as_word);
}

पूर्णाङ्क __vpiDarrayVar::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiArrayType:
	    फर्काऊ vpiDynamicArray;
	  पक्ष vpi_बायाँ_Range:
	    फर्काऊ 0;
	  पक्ष vpi_दायाँ_Range:
	    फर्काऊ get_size() - 1;
	  पक्ष vpiSize:
            फर्काऊ get_size();

	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "vpi sorry: property is not implemented");
	    प्रतिज्ञा(असत्य);
	    फर्काऊ 0;
      }
}

वर्ण* __vpiDarrayVar::vpi_get_str(पूर्णाङ्क code)
{
      यदि (code == vpiFile) {  // Not implemented for now!
            फर्काऊ simple_set_rbuf_str(file_names[0]);
      }

      फर्काऊ generic_get_str(code, scope_, name_, रिक्त);
}

vpiHandle __vpiDarrayVar::vpi_handle(पूर्णाङ्क code)
{
      विकल्प (code) {
          पक्ष vpi_बायाँ_Range:
            फर्काऊ get_बायाँ_range();

          पक्ष vpi_दायाँ_Range:
            फर्काऊ get_दायाँ_range();

          पक्ष vpiScope:
            फर्काऊ scope_;

          पक्ष vpiModule:
            फर्काऊ vpip_module(scope_);
      }

      फर्काऊ 0;
}

vpiHandle __vpiDarrayVar::vpi_index(पूर्णाङ्क index)
{
      यदि (index >= (दीर्घ) get_size())
	    फर्काऊ 0;
      यदि (index < 0)
	    फर्काऊ 0;

      यदि (vals_शब्दहरू == 0)
	    make_vals_शब्दहरू();

      फर्काऊ &(vals_शब्दहरू[index].as_word);
}

शून्य __vpiDarrayVar::vpi_get_मान(p_vpi_मान val)
{
      val->format = vpiSuppressVal;
}

vvp_darray*__vpiDarrayVar::get_vvp_darray() अचर
{
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      प्रतिज्ञा(fun);
      vvp_object_t obj = fun->get_object();

      फर्काऊ obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(अचर वर्ण*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      अचर वर्ण*use_name = name ? vpip_name_string(name) : रिक्त;

      __vpiDarrayVar*obj = नूतन __vpiDarrayVar(scope, use_name, net);

      फर्काऊ obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, अचर वर्ण*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

पूर्णाङ्क __vpiQueueVar::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiArrayType:
	    फर्काऊ vpiQueueArray;
	  स्वभाव:
	    फर्काऊ __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_queue_var(अचर वर्ण*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      अचर वर्ण*use_name = name ? vpip_name_string(name) : रिक्त;

      __vpiQueueVar*obj = नूतन __vpiQueueVar(scope, use_name, net);

      फर्काऊ obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, अचिन्हित pidx,
				     तार्किक is_queue)
: class_net_(0), prop_idx_(pidx), is_queue_(is_queue), scope_(scope)
{
}

पूर्णाङ्क __vpiPropQueueRef::get_type_code(शून्य) अचर
{ फर्काऊ vpiArrayVar; }

स्थिर vvp_darray* get_live_darray_from_prop(vvp_net_t* class_net, अचिन्हित pid)
{
      यदि (!class_net) फर्काऊ 0;
      अचर vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      यदि (!fun) फर्काऊ 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      यदि (!cobj) फर्काऊ 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      फर्काऊ qobj.peek<vvp_darray>();
}

पूर्णाङ्क __vpiPropQueueRef::vpi_get(पूर्णाङ्क code)
{
      अचर vvp_darray* aobj = get_live_darray_from_prop(class_net_, prop_idx_);
      विकल्प (code) {
	  पक्ष vpiArrayType:
	    फर्काऊ is_queue_ ? vpiQueueArray : vpiDynamicArray;
	  पक्ष vpi_बायाँ_Range:
	    फर्काऊ 0;
	  पक्ष vpi_दायाँ_Range:
	    फर्काऊ aobj ? (पूर्णाङ्क) (aobj->get_size() - 1) : 0;
	  पक्ष vpiSize:
	    फर्काऊ aobj ? (पूर्णाङ्क) aobj->get_size() : 0;
	  स्वभाव:
	    फर्काऊ 0;
      }
}

वर्ण* __vpiPropQueueRef::vpi_get_str(पूर्णाङ्क code)
{
      यदि (code == vpiFile) {
	    फर्काऊ simple_set_rbuf_str(file_names[0]);
      }
      फर्काऊ generic_get_str(code, scope_, "prop_darray", रिक्त);
}

शून्य __vpiPropQueueRef::vpi_get_मान(p_vpi_मान val)
{
      val->format = vpiSuppressVal;
}

vpiHandle vpip_make_prop_queue_ref(वर्ण* class_label, अचिन्हित prop_idx,
				   अचिन्हित is_queue_flag)
{
      __vpiPropQueueRef* obj = नूतन __vpiPropQueueRef(vpip_peek_current_scope(),
						     prop_idx,
						     is_queue_flag != 0);
      functor_ref_lookup(&obj->class_net_, class_label);
      फर्काऊ obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      यदि (!ref) फर्काऊ 0;

      यदि (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    फर्काऊ get_live_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      यदि (अचर __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    अचर vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    यदि (!fun) फर्काऊ 0;
	    vvp_object_t obj = fun->get_object();
	    फर्काऊ obj.peek<vvp_darray>();
      }

      फर्काऊ 0;
}

#ifdef CHECK_WITH_VALGRIND
शून्य array_delete(vpiHandle item)
{
      यदि (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(item)) {
	    delete pq;
	    फर्काऊ;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(item);
      यदि (dobj) {
	    यदि (dobj->vals_शब्दहरू) delete [] (dobj->vals_शब्दहरू-1);
	    delete dobj;
	    फर्काऊ;
      }

      ख_लिखो(मानक_त्रुटि, "Need support for deleting array type: %d\n", item->vpi_get(vpiArrayType));
      प्रतिज्ञा(0);
}
#endif
