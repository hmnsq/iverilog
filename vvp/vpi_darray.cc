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

using നാമാകാശ std;

__vpiDarrayVar::__vpiDarrayVar(__vpiScope*sc, സ്ഥിരം അക്ഷരം*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

unsigned __vpiDarrayVar::get_size() സ്ഥിരം
{
      സ്ഥിരം vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      എങ്കിൽ(!fun)
        തിരികെ 0;

      vvp_object_t val = fun->get_object();
      സ്ഥിരം vvp_darray*aval = val.peek<vvp_darray>();

      എങ്കിൽ(!aval)
        തിരികെ 0;

      തിരികെ aval->get_size();
}

vpiHandle __vpiDarrayVar::get_ഇടത്_range()
{
      ഇടത്_range.set_മൂല്യം(0);
      തിരികെ &ഇടത്_range;
}

vpiHandle __vpiDarrayVar::get_വലത്_range()
{
      വലത്_range.set_മൂല്യം(get_size() - 1);
      തിരികെ &വലത്_range;
}

പൂർണ്ണസംഖ്യ __vpiDarrayVar::get_word_size() സ്ഥിരം
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_word(0, new_vec);
      തിരികെ new_vec.size();
}

അക്ഷരം*__vpiDarrayVar::get_word_str(ഘടന __vpiArrayWord*word, പൂർണ്ണസംഖ്യ code)
{
      unsigned index = word->get_index();

      എങ്കിൽ (code == vpiFile) {  // Not implemented for now!
	    തിരികെ simple_set_rbuf_str(file_names[0]);
      }

      അക്ഷരം sidx [64];
      snprintf(sidx, 63, "%d", (പൂർണ്ണസംഖ്യ)index);
      തിരികെ generic_get_str(code, scope_, name_, sidx);
}

ശൂന്യം __vpiDarrayVar::get_word_മൂല്യം(ഘടന __vpiArrayWord*word, p_vpi_മൂല്യം vp)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      എങ്കിൽ(vp->format == vpiObjTypeVal) {
          എങ്കിൽ(dynamic_cast<vvp_darray_real*>(aobj))
              vp->format = vpiRealVal;
          അല്ലെങ്കിൽ എങ്കിൽ(dynamic_cast<vvp_darray_string*>(aobj))
              vp->format = vpiStringVal;
          അല്ലെങ്കിൽ
              vp->format = vpiVectorVal;
      }

      തിരഞ്ഞെടുപ്പ്(vp->format) {
      അവസ്ഥ vpiBinStrVal:
      അവസ്ഥ vpiOctStrVal:
      അവസ്ഥ vpiDecStrVal:
      അവസ്ഥ vpiHexStrVal:
      അവസ്ഥ vpiScalarVal:
      അവസ്ഥ vpiIntVal:
      അവസ്ഥ vpiVectorVal:
      {
          vvp_vector4_t v;
          aobj->get_word(index, v);
          vpip_vec4_get_മൂല്യം(v, v.size(), അസത്യം, vp);  // TODO sign?
      }
      നിർത്തുക;

      അവസ്ഥ vpiRealVal:
      {
          ഇരട്ട d;
          aobj->get_word(index, d);
          vpip_real_get_മൂല്യം(d, vp);
      }
      നിർത്തുക;

      അവസ്ഥ vpiStringVal:
      {
          string s;
          aobj->get_word(index, s);
          vpip_string_get_മൂല്യം(s, vp);
      }
      നിർത്തുക;

      സ്ഥിരസ്ഥിതി:
          fprintf(stderr, "vpi sorry: format is not implemented\n");
          പ്രതിജ്ഞാ(അസത്യം);
      }
}

ശൂന്യം __vpiDarrayVar::put_word_മൂല്യം(ഘടന __vpiArrayWord*word, p_vpi_മൂല്യം vp, പൂർണ്ണസംഖ്യ)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      തിരഞ്ഞെടുപ്പ്(vp->format) {
      അവസ്ഥ vpiScalarVal:
      അവസ്ഥ vpiIntVal:
      അവസ്ഥ vpiVectorVal: {
	    vvp_vector4_t vec = vec4_from_vpi_മൂല്യം(vp, get_word_size());
	    aobj->set_word(index, vec);
      }
      നിർത്തുക;

      അവസ്ഥ vpiRealVal:
        aobj->set_word(index, vp->value.real);
        നിർത്തുക;

      അവസ്ഥ vpiStringVal:
        aobj->set_word(index, std::string(vp->value.str));
        നിർത്തുക;

      സ്ഥിരസ്ഥിതി:
          fprintf(stderr, "vpi sorry: format is not implemented");
          പ്രതിജ്ഞാ(അസത്യം);
      }
}

vpiHandle __vpiDarrayVar::get_iter_index(ഘടന __vpiArrayIterator*, പൂർണ്ണസംഖ്യ idx)
{
      എങ്കിൽ (vals_വാക്കുകൾ == 0) make_vals_വാക്കുകൾ();

      തിരികെ &(vals_വാക്കുകൾ[idx].as_word);
}

പൂർണ്ണസംഖ്യ __vpiDarrayVar::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiArrayType:
	    തിരികെ vpiDynamicArray;
	  അവസ്ഥ vpi_ഇടത്_Range:
	    തിരികെ 0;
	  അവസ്ഥ vpi_വലത്_Range:
	    തിരികെ get_size() - 1;
	  അവസ്ഥ vpiSize:
            തിരികെ get_size();

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    പ്രതിജ്ഞാ(അസത്യം);
	    തിരികെ 0;
      }
}

അക്ഷരം* __vpiDarrayVar::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{
      എങ്കിൽ (code == vpiFile) {  // Not implemented for now!
            തിരികെ simple_set_rbuf_str(file_names[0]);
      }

      തിരികെ generic_get_str(code, scope_, name_, ശൂന്യ);
}

vpiHandle __vpiDarrayVar::vpi_handle(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
          അവസ്ഥ vpi_ഇടത്_Range:
            തിരികെ get_ഇടത്_range();

          അവസ്ഥ vpi_വലത്_Range:
            തിരികെ get_വലത്_range();

          അവസ്ഥ vpiScope:
            തിരികെ scope_;

          അവസ്ഥ vpiModule:
            തിരികെ vpip_module(scope_);
      }

      തിരികെ 0;
}

vpiHandle __vpiDarrayVar::vpi_index(പൂർണ്ണസംഖ്യ index)
{
      എങ്കിൽ (index >= (long) get_size())
	    തിരികെ 0;
      എങ്കിൽ (index < 0)
	    തിരികെ 0;

      എങ്കിൽ (vals_വാക്കുകൾ == 0)
	    make_vals_വാക്കുകൾ();

      തിരികെ &(vals_വാക്കുകൾ[index].as_word);
}

ശൂന്യം __vpiDarrayVar::vpi_get_മൂല്യം(p_vpi_മൂല്യം val)
{
      val->format = vpiSuppressVal;
}

vvp_darray*__vpiDarrayVar::get_vvp_darray() സ്ഥിരം
{
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      പ്രതിജ്ഞാ(fun);
      vvp_object_t obj = fun->get_object();

      തിരികെ obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(സ്ഥിരം അക്ഷരം*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      സ്ഥിരം അക്ഷരം*use_name = name ? vpip_name_string(name) : ശൂന്യ;

      __vpiDarrayVar*obj = പുതിയ __vpiDarrayVar(scope, use_name, net);

      തിരികെ obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, സ്ഥിരം അക്ഷരം*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

പൂർണ്ണസംഖ്യ __vpiQueueVar::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiArrayType:
	    തിരികെ vpiQueueArray;
	  സ്ഥിരസ്ഥിതി:
	    തിരികെ __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_queue_var(സ്ഥിരം അക്ഷരം*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      സ്ഥിരം അക്ഷരം*use_name = name ? vpip_name_string(name) : ശൂന്യ;

      __vpiQueueVar*obj = പുതിയ __vpiQueueVar(scope, use_name, net);

      തിരികെ obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, unsigned pidx,
				     തർക്കം is_queue)
: class_net_(0), prop_idx_(pidx), is_queue_(is_queue), scope_(scope)
{
}

പൂർണ്ണസംഖ്യ __vpiPropQueueRef::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiArrayVar; }

സ്ഥിതം vvp_darray* get_live_darray_from_prop(vvp_net_t* class_net, unsigned pid)
{
      എങ്കിൽ (!class_net) തിരികെ 0;
      സ്ഥിരം vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      എങ്കിൽ (!fun) തിരികെ 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      എങ്കിൽ (!cobj) തിരികെ 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      തിരികെ qobj.peek<vvp_darray>();
}

പൂർണ്ണസംഖ്യ __vpiPropQueueRef::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      സ്ഥിരം vvp_darray* aobj = get_live_darray_from_prop(class_net_, prop_idx_);
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiArrayType:
	    തിരികെ is_queue_ ? vpiQueueArray : vpiDynamicArray;
	  അവസ്ഥ vpi_ഇടത്_Range:
	    തിരികെ 0;
	  അവസ്ഥ vpi_വലത്_Range:
	    തിരികെ aobj ? (പൂർണ്ണസംഖ്യ) (aobj->get_size() - 1) : 0;
	  അവസ്ഥ vpiSize:
	    തിരികെ aobj ? (പൂർണ്ണസംഖ്യ) aobj->get_size() : 0;
	  സ്ഥിരസ്ഥിതി:
	    തിരികെ 0;
      }
}

അക്ഷരം* __vpiPropQueueRef::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{
      എങ്കിൽ (code == vpiFile) {
	    തിരികെ simple_set_rbuf_str(file_names[0]);
      }
      തിരികെ generic_get_str(code, scope_, "prop_darray", ശൂന്യ);
}

ശൂന്യം __vpiPropQueueRef::vpi_get_മൂല്യം(p_vpi_മൂല്യം val)
{
      val->format = vpiSuppressVal;
}

vpiHandle vpip_make_prop_queue_ref(അക്ഷരം* class_label, unsigned prop_idx,
				   unsigned is_queue_flag)
{
      __vpiPropQueueRef* obj = പുതിയ __vpiPropQueueRef(vpip_peek_current_scope(),
						     prop_idx,
						     is_queue_flag != 0);
      functor_ref_lookup(&obj->class_net_, class_label);
      തിരികെ obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      എങ്കിൽ (!ref) തിരികെ 0;

      എങ്കിൽ (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    തിരികെ get_live_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      എങ്കിൽ (സ്ഥിരം __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    സ്ഥിരം vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    എങ്കിൽ (!fun) തിരികെ 0;
	    vvp_object_t obj = fun->get_object();
	    തിരികെ obj.peek<vvp_darray>();
      }

      തിരികെ 0;
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം array_delete(vpiHandle item)
{
      എങ്കിൽ (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(item)) {
	    delete pq;
	    തിരികെ;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(item);
      എങ്കിൽ (dobj) {
	    എങ്കിൽ (dobj->vals_വാക്കുകൾ) delete [] (dobj->vals_വാക്കുകൾ-1);
	    delete dobj;
	    തിരികെ;
      }

      fprintf(stderr, "Need support for deleting array type: %d\n", item->vpi_get(vpiArrayType));
      പ്രതിജ്ഞാ(0);
}
#endif
