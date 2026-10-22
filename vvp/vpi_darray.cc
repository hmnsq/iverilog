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

using நாமாகாஶ std;

__vpiDarrayVar::__vpiDarrayVar(__vpiScope*sc, மாறிலி எழுத்து*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

unsigned __vpiDarrayVar::get_size() மாறிலி
{
      மாறிலி vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      எனில்(!fun)
        திரும்பு 0;

      vvp_object_t val = fun->get_object();
      மாறிலி vvp_darray*aval = val.peek<vvp_darray>();

      எனில்(!aval)
        திரும்பு 0;

      திரும்பு aval->get_size();
}

vpiHandle __vpiDarrayVar::get_இடது_range()
{
      இடது_range.set_மதிப்பு(0);
      திரும்பு &இடது_range;
}

vpiHandle __vpiDarrayVar::get_வலது_range()
{
      வலது_range.set_மதிப்பு(get_size() - 1);
      திரும்பு &வலது_range;
}

முழு __vpiDarrayVar::get_word_size() மாறிலி
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_word(0, new_vec);
      திரும்பு new_vec.size();
}

எழுத்து*__vpiDarrayVar::get_word_str(கட்டமைப்பு __vpiArrayWord*word, முழு code)
{
      unsigned index = word->get_index();

      எனில் (code == vpiFile) {  // Not implemented for now!
	    திரும்பு simple_set_rbuf_str(file_names[0]);
      }

      எழுத்து sidx [64];
      snprintf(sidx, 63, "%d", (முழு)index);
      திரும்பு generic_get_str(code, scope_, name_, sidx);
}

வெறுமை __vpiDarrayVar::get_word_மதிப்பு(கட்டமைப்பு __vpiArrayWord*word, p_vpi_மதிப்பு vp)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      எனில்(vp->format == vpiObjTypeVal) {
          எனில்(dynamic_cast<vvp_darray_real*>(aobj))
              vp->format = vpiRealVal;
          இல்லையெனில் எனில்(dynamic_cast<vvp_darray_string*>(aobj))
              vp->format = vpiStringVal;
          இல்லையெனில்
              vp->format = vpiVectorVal;
      }

      தேர்வு(vp->format) {
      நிலை vpiBinStrVal:
      நிலை vpiOctStrVal:
      நிலை vpiDecStrVal:
      நிலை vpiHexStrVal:
      நிலை vpiScalarVal:
      நிலை vpiIntVal:
      நிலை vpiVectorVal:
      {
          vvp_vector4_t v;
          aobj->get_word(index, v);
          vpip_vec4_get_மதிப்பு(v, v.size(), பொய், vp);  // TODO sign?
      }
      நிறுத்து;

      நிலை vpiRealVal:
      {
          இரட்டை d;
          aobj->get_word(index, d);
          vpip_real_get_மதிப்பு(d, vp);
      }
      நிறுத்து;

      நிலை vpiStringVal:
      {
          string s;
          aobj->get_word(index, s);
          vpip_string_get_மதிப்பு(s, vp);
      }
      நிறுத்து;

      இயல்பு:
          fprintf(stderr, "vpi sorry: format is not implemented\n");
          ப்ரதிஜ்ஞா(பொய்);
      }
}

வெறுமை __vpiDarrayVar::put_word_மதிப்பு(கட்டமைப்பு __vpiArrayWord*word, p_vpi_மதிப்பு vp, முழு)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      தேர்வு(vp->format) {
      நிலை vpiScalarVal:
      நிலை vpiIntVal:
      நிலை vpiVectorVal: {
	    vvp_vector4_t vec = vec4_from_vpi_மதிப்பு(vp, get_word_size());
	    aobj->set_word(index, vec);
      }
      நிறுத்து;

      நிலை vpiRealVal:
        aobj->set_word(index, vp->value.real);
        நிறுத்து;

      நிலை vpiStringVal:
        aobj->set_word(index, std::string(vp->value.str));
        நிறுத்து;

      இயல்பு:
          fprintf(stderr, "vpi sorry: format is not implemented");
          ப்ரதிஜ்ஞா(பொய்);
      }
}

vpiHandle __vpiDarrayVar::get_iter_index(கட்டமைப்பு __vpiArrayIterator*, முழு idx)
{
      எனில் (vals_சொற்கள் == 0) make_vals_சொற்கள்();

      திரும்பு &(vals_சொற்கள்[idx].as_word);
}

முழு __vpiDarrayVar::vpi_get(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiArrayType:
	    திரும்பு vpiDynamicArray;
	  நிலை vpi_இடது_Range:
	    திரும்பு 0;
	  நிலை vpi_வலது_Range:
	    திரும்பு get_size() - 1;
	  நிலை vpiSize:
            திரும்பு get_size();

	  இயல்பு:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    ப்ரதிஜ்ஞா(பொய்);
	    திரும்பு 0;
      }
}

எழுத்து* __vpiDarrayVar::vpi_get_str(முழு code)
{
      எனில் (code == vpiFile) {  // Not implemented for now!
            திரும்பு simple_set_rbuf_str(file_names[0]);
      }

      திரும்பு generic_get_str(code, scope_, name_, வெற்று);
}

vpiHandle __vpiDarrayVar::vpi_handle(முழு code)
{
      தேர்வு (code) {
          நிலை vpi_இடது_Range:
            திரும்பு get_இடது_range();

          நிலை vpi_வலது_Range:
            திரும்பு get_வலது_range();

          நிலை vpiScope:
            திரும்பு scope_;

          நிலை vpiModule:
            திரும்பு vpip_module(scope_);
      }

      திரும்பு 0;
}

vpiHandle __vpiDarrayVar::vpi_index(முழு index)
{
      எனில் (index >= (long) get_size())
	    திரும்பு 0;
      எனில் (index < 0)
	    திரும்பு 0;

      எனில் (vals_சொற்கள் == 0)
	    make_vals_சொற்கள்();

      திரும்பு &(vals_சொற்கள்[index].as_word);
}

வெறுமை __vpiDarrayVar::vpi_get_மதிப்பு(p_vpi_மதிப்பு val)
{
      val->format = vpiSuppressVal;
}

vvp_darray*__vpiDarrayVar::get_vvp_darray() மாறிலி
{
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      ப்ரதிஜ்ஞா(fun);
      vvp_object_t obj = fun->get_object();

      திரும்பு obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(மாறிலி எழுத்து*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      மாறிலி எழுத்து*use_name = name ? vpip_name_string(name) : வெற்று;

      __vpiDarrayVar*obj = புதிய __vpiDarrayVar(scope, use_name, net);

      திரும்பு obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, மாறிலி எழுத்து*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

முழு __vpiQueueVar::vpi_get(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiArrayType:
	    திரும்பு vpiQueueArray;
	  இயல்பு:
	    திரும்பு __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_queue_var(மாறிலி எழுத்து*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      மாறிலி எழுத்து*use_name = name ? vpip_name_string(name) : வெற்று;

      __vpiQueueVar*obj = புதிய __vpiQueueVar(scope, use_name, net);

      திரும்பு obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, unsigned pidx,
				     தருக்க is_queue)
: class_net_(0), prop_idx_(pidx), is_queue_(is_queue), scope_(scope)
{
}

முழு __vpiPropQueueRef::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiArrayVar; }

நிலையான vvp_darray* get_live_darray_from_prop(vvp_net_t* class_net, unsigned pid)
{
      எனில் (!class_net) திரும்பு 0;
      மாறிலி vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      எனில் (!fun) திரும்பு 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      எனில் (!cobj) திரும்பு 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      திரும்பு qobj.peek<vvp_darray>();
}

முழு __vpiPropQueueRef::vpi_get(முழு code)
{
      மாறிலி vvp_darray* aobj = get_live_darray_from_prop(class_net_, prop_idx_);
      தேர்வு (code) {
	  நிலை vpiArrayType:
	    திரும்பு is_queue_ ? vpiQueueArray : vpiDynamicArray;
	  நிலை vpi_இடது_Range:
	    திரும்பு 0;
	  நிலை vpi_வலது_Range:
	    திரும்பு aobj ? (முழு) (aobj->get_size() - 1) : 0;
	  நிலை vpiSize:
	    திரும்பு aobj ? (முழு) aobj->get_size() : 0;
	  இயல்பு:
	    திரும்பு 0;
      }
}

எழுத்து* __vpiPropQueueRef::vpi_get_str(முழு code)
{
      எனில் (code == vpiFile) {
	    திரும்பு simple_set_rbuf_str(file_names[0]);
      }
      திரும்பு generic_get_str(code, scope_, "prop_darray", வெற்று);
}

வெறுமை __vpiPropQueueRef::vpi_get_மதிப்பு(p_vpi_மதிப்பு val)
{
      val->format = vpiSuppressVal;
}

vpiHandle vpip_make_prop_queue_ref(எழுத்து* class_label, unsigned prop_idx,
				   unsigned is_queue_flag)
{
      __vpiPropQueueRef* obj = புதிய __vpiPropQueueRef(vpip_peek_current_scope(),
						     prop_idx,
						     is_queue_flag != 0);
      functor_ref_lookup(&obj->class_net_, class_label);
      திரும்பு obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      எனில் (!ref) திரும்பு 0;

      எனில் (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    திரும்பு get_live_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      எனில் (மாறிலி __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    மாறிலி vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    எனில் (!fun) திரும்பு 0;
	    vvp_object_t obj = fun->get_object();
	    திரும்பு obj.peek<vvp_darray>();
      }

      திரும்பு 0;
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை array_delete(vpiHandle item)
{
      எனில் (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(item)) {
	    delete pq;
	    திரும்பு;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(item);
      எனில் (dobj) {
	    எனில் (dobj->vals_சொற்கள்) delete [] (dobj->vals_சொற்கள்-1);
	    delete dobj;
	    திரும்பு;
      }

      fprintf(stderr, "Need support for deleting array type: %d\n", item->vpi_get(vpiArrayType));
      ப்ரதிஜ்ஞா(0);
}
#endif
