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
#अगर_घोषित CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#पूर्ण_अगर
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

using namespace std;

__vpiDarrayVar::__vpiDarrayVar(__vpiScope*sc, स्थिर अक्षर*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

अचिन्हित __vpiDarrayVar::get_आकार() स्थिर
{
      स्थिर vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      अगर(!fun)
        वापस 0;

      vvp_object_t val = fun->get_object();
      स्थिर vvp_darray*aval = val.peek<vvp_darray>();

      अगर(!aval)
        वापस 0;

      वापस aval->get_आकार();
}

vpiHandle __vpiDarrayVar::get_बायाँ_range()
{
      बायाँ_range.set_मूल्य(0);
      वापस &बायाँ_range;
}

vpiHandle __vpiDarrayVar::get_दायाँ_range()
{
      दायाँ_range.set_मूल्य(get_आकार() - 1);
      वापस &दायाँ_range;
}

पूर्णांक __vpiDarrayVar::get_word_आकार() स्थिर
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_word(0, new_vec);
      वापस new_vec.size();
}

अक्षर*__vpiDarrayVar::get_word_str(काष्ठा __vpiArrayWord*word, पूर्णांक code)
{
      अचिन्हित सूचकांक = word->get_सूचकांक();

      अगर (code == vpiFile) {  // Not implemented for now!
	    वापस simple_set_rbuf_str(file_names[0]);
      }

      अक्षर sidx [64];
      snprintf(sidx, 63, "%d", (पूर्णांक)सूचकांक);
      वापस generic_get_str(code, scope_, name_, sidx);
}

व्योम __vpiDarrayVar::get_word_मूल्य(काष्ठा __vpiArrayWord*word, p_vpi_मूल्य vp)
{
      अचिन्हित सूचकांक = word->get_सूचकांक();
      vvp_darray*aobj = get_vvp_darray();

      अगर(vp->format == vpiObjTypeVal) {
          अगर(dynamic_cast<vvp_darray_real*>(aobj))
              vp->format = vpiRealVal;
          अन्यथा अगर(dynamic_cast<vvp_darray_string*>(aobj))
              vp->format = vpiStringVal;
          अन्यथा
              vp->format = vpiVectorVal;
      }

      चयन(vp->format) {
      हाल vpiBinStrVal:
      हाल vpiOctStrVal:
      हाल vpiDecStrVal:
      हाल vpiHexStrVal:
      हाल vpiScalarVal:
      हाल vpiIntVal:
      हाल vpiVectorVal:
      {
          vvp_vector4_t v;
          aobj->get_word(सूचकांक, v);
          vpip_vec4_get_मूल्य(v, v.size(), false, vp);  // TODO sign?
      }
      अवरोध;

      हाल vpiRealVal:
      {
          द्विगुन d;
          aobj->get_word(सूचकांक, d);
          vpip_real_get_मूल्य(d, vp);
      }
      अवरोध;

      हाल vpiStringVal:
      {
          string s;
          aobj->get_word(सूचकांक, s);
          vpip_string_get_मूल्य(s, vp);
      }
      अवरोध;

      शेष:
          ख_लिखो(मानक_त्रुटि, "vpi sorry: format is not implemented\n");
          निश्चित(false);
      }
}

व्योम __vpiDarrayVar::put_word_मूल्य(काष्ठा __vpiArrayWord*word, p_vpi_मूल्य vp, पूर्णांक)
{
      अचिन्हित सूचकांक = word->get_सूचकांक();
      vvp_darray*aobj = get_vvp_darray();

      चयन(vp->format) {
      हाल vpiScalarVal:
      हाल vpiIntVal:
      हाल vpiVectorVal: {
	    vvp_vector4_t vec = vec4_from_vpi_मूल्य(vp, get_word_आकार());
	    aobj->set_word(सूचकांक, vec);
      }
      अवरोध;

      हाल vpiRealVal:
        aobj->set_word(सूचकांक, vp->value.real);
        अवरोध;

      हाल vpiStringVal:
        aobj->set_word(सूचकांक, std::string(vp->value.str));
        अवरोध;

      शेष:
          ख_लिखो(मानक_त्रुटि, "vpi sorry: format is not implemented");
          निश्चित(false);
      }
}

vpiHandle __vpiDarrayVar::get_iter_सूचकांक(काष्ठा __vpiArrayIterator*, पूर्णांक idx)
{
      अगर (vals_शब्दगण == 0) make_vals_शब्दगण();

      वापस &(vals_शब्दगण[idx].as_word);
}

पूर्णांक __vpiDarrayVar::vpi_get(पूर्णांक code)
{
      चयन (code) {
	  हाल vpiArrayType:
	    वापस vpiDynamicArray;
	  हाल vpi_बायाँ_Range:
	    वापस 0;
	  हाल vpi_दायाँ_Range:
	    वापस get_आकार() - 1;
	  हाल vpi_आकार:
            वापस get_आकार();

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "vpi sorry: property is not implemented");
	    निश्चित(false);
	    वापस 0;
      }
}

अक्षर* __vpiDarrayVar::vpi_get_str(पूर्णांक code)
{
      अगर (code == vpiFile) {  // Not implemented for now!
            वापस simple_set_rbuf_str(file_names[0]);
      }

      वापस generic_get_str(code, scope_, name_, शून्य);
}

vpiHandle __vpiDarrayVar::vpi_handle(पूर्णांक code)
{
      चयन (code) {
          हाल vpi_बायाँ_Range:
            वापस get_बायाँ_range();

          हाल vpi_दायाँ_Range:
            वापस get_दायाँ_range();

          हाल vpiScope:
            वापस scope_;

          हाल vpiModule:
            वापस vpip_module(scope_);
      }

      वापस 0;
}

vpiHandle __vpiDarrayVar::vpi_सूचकांक(पूर्णांक सूचकांक)
{
      अगर (सूचकांक >= (दीर्घ) get_आकार())
	    वापस 0;
      अगर (सूचकांक < 0)
	    वापस 0;

      अगर (vals_शब्दगण == 0)
	    make_vals_शब्दगण();

      वापस &(vals_शब्दगण[सूचकांक].as_word);
}

व्योम __vpiDarrayVar::vpi_get_मूल्य(p_vpi_मूल्य val)
{
      val->format = vpiSuppressVal;
}

vvp_darray*__vpiDarrayVar::get_vvp_darray() स्थिर
{
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      निश्चित(fun);
      vvp_object_t obj = fun->get_object();

      वापस obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(स्थिर अक्षर*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      स्थिर अक्षर*use_name = name ? vpip_name_string(name) : शून्य;

      __vpiDarrayVar*obj = नया __vpiDarrayVar(scope, use_name, net);

      वापस obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, स्थिर अक्षर*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

पूर्णांक __vpiQueueVar::vpi_get(पूर्णांक code)
{
      चयन (code) {
	  हाल vpiArrayType:
	    वापस vpiQueueArray;
	  शेष:
	    वापस __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_queue_var(स्थिर अक्षर*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      स्थिर अक्षर*use_name = name ? vpip_name_string(name) : शून्य;

      __vpiQueueVar*obj = नया __vpiQueueVar(scope, use_name, net);

      वापस obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, अचिन्हित pidx,
				     bool is_queue)
: class_net_(0), prop_idx_(pidx), is_queue_(is_queue), scope_(scope)
{
}

पूर्णांक __vpiPropQueueRef::get_type_code(व्योम) स्थिर
{ वापस vpiArrayVar; }

अटल vvp_darray* get_live_darray_from_prop(vvp_net_t* class_net, अचिन्हित pid)
{
      अगर (!class_net) वापस 0;
      स्थिर vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      अगर (!fun) वापस 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      अगर (!cobj) वापस 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      वापस qobj.peek<vvp_darray>();
}

पूर्णांक __vpiPropQueueRef::vpi_get(पूर्णांक code)
{
      स्थिर vvp_darray* aobj = get_live_darray_from_prop(class_net_, prop_idx_);
      चयन (code) {
	  हाल vpiArrayType:
	    वापस is_queue_ ? vpiQueueArray : vpiDynamicArray;
	  हाल vpi_बायाँ_Range:
	    वापस 0;
	  हाल vpi_दायाँ_Range:
	    वापस aobj ? (पूर्णांक) (aobj->get_आकार() - 1) : 0;
	  हाल vpi_आकार:
	    वापस aobj ? (पूर्णांक) aobj->get_आकार() : 0;
	  शेष:
	    वापस 0;
      }
}

अक्षर* __vpiPropQueueRef::vpi_get_str(पूर्णांक code)
{
      अगर (code == vpiFile) {
	    वापस simple_set_rbuf_str(file_names[0]);
      }
      वापस generic_get_str(code, scope_, "prop_darray", शून्य);
}

व्योम __vpiPropQueueRef::vpi_get_मूल्य(p_vpi_मूल्य val)
{
      val->format = vpiSuppressVal;
}

vpiHandle vpip_make_prop_queue_ref(अक्षर* class_label, अचिन्हित prop_idx,
				   अचिन्हित is_queue_flag)
{
      __vpiPropQueueRef* obj = नया __vpiPropQueueRef(vpip_peek_current_scope(),
						     prop_idx,
						     is_queue_flag != 0);
      functor_ref_lookup(&obj->class_net_, class_label);
      वापस obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      अगर (!ref) वापस 0;

      अगर (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    वापस get_live_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      अगर (स्थिर __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    स्थिर vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    अगर (!fun) वापस 0;
	    vvp_object_t obj = fun->get_object();
	    वापस obj.peek<vvp_darray>();
      }

      वापस 0;
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम array_delete(vpiHandle item)
{
      अगर (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(item)) {
	    मिटाओ pq;
	    वापस;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(item);
      अगर (dobj) {
	    अगर (dobj->vals_शब्दगण) मिटाओ [] (dobj->vals_शब्दगण-1);
	    मिटाओ dobj;
	    वापस;
      }

      ख_लिखो(मानक_त्रुटि, "Need support for deleting array type: %d\n", item->vpi_get(vpiArrayType));
      निश्चित(0);
}
#पूर्ण_अगर
