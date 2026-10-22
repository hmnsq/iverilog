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

using নামাকাশ std;

__vpiDarrayVar::__vpiDarrayVar(__vpiScope*sc, ধ্রুব বর্ণ*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

unsigned __vpiDarrayVar::get_size() ধ্রুব
{
      ধ্রুব vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      যদি(!fun)
        প্রত্যাগম 0;

      vvp_object_t val = fun->get_object();
      ধ্রুব vvp_darray*aval = val.peek<vvp_darray>();

      যদি(!aval)
        প্রত্যাগম 0;

      প্রত্যাগম aval->get_size();
}

vpiHandle __vpiDarrayVar::get_বাম_range()
{
      বাম_range.set_মান(0);
      প্রত্যাগম &বাম_range;
}

vpiHandle __vpiDarrayVar::get_ডান_range()
{
      ডান_range.set_মান(get_size() - 1);
      প্রত্যাগম &ডান_range;
}

পূর্ণাঙ্ক __vpiDarrayVar::get_word_size() ধ্রুব
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_word(0, new_vec);
      প্রত্যাগম new_vec.size();
}

বর্ণ*__vpiDarrayVar::get_word_str(সংরচনা __vpiArrayWord*word, পূর্ণাঙ্ক code)
{
      unsigned index = word->get_index();

      যদি (code == vpiFile) {  // Not implemented for now!
	    প্রত্যাগম simple_set_rbuf_str(file_names[0]);
      }

      বর্ণ sidx [64];
      snprintf(sidx, 63, "%d", (পূর্ণাঙ্ক)index);
      প্রত্যাগম generic_get_str(code, scope_, name_, sidx);
}

শূন্য __vpiDarrayVar::get_word_মান(সংরচনা __vpiArrayWord*word, p_vpi_মান vp)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      যদি(vp->format == vpiObjTypeVal) {
          যদি(dynamic_cast<vvp_darray_real*>(aobj))
              vp->format = vpiRealVal;
          অন্যথা যদি(dynamic_cast<vvp_darray_string*>(aobj))
              vp->format = vpiStringVal;
          অন্যথা
              vp->format = vpiVectorVal;
      }

      বিকল্প(vp->format) {
      পক্ষ vpiBinStrVal:
      পক্ষ vpiOctStrVal:
      পক্ষ vpiDecStrVal:
      পক্ষ vpiHexStrVal:
      পক্ষ vpiScalarVal:
      পক্ষ vpiIntVal:
      পক্ষ vpiVectorVal:
      {
          vvp_vector4_t v;
          aobj->get_word(index, v);
          vpip_vec4_get_মান(v, v.size(), অসত্য, vp);  // TODO sign?
      }
      ভঙ্গ;

      পক্ষ vpiRealVal:
      {
          দ্বিগুণ d;
          aobj->get_word(index, d);
          vpip_real_get_মান(d, vp);
      }
      ভঙ্গ;

      পক্ষ vpiStringVal:
      {
          string s;
          aobj->get_word(index, s);
          vpip_string_get_মান(s, vp);
      }
      ভঙ্গ;

      স্বভাব:
          fprintf(stderr, "vpi sorry: format is not implemented\n");
          প্রতিজ্ঞা(অসত্য);
      }
}

শূন্য __vpiDarrayVar::put_word_মান(সংরচনা __vpiArrayWord*word, p_vpi_মান vp, পূর্ণাঙ্ক)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      বিকল্প(vp->format) {
      পক্ষ vpiScalarVal:
      পক্ষ vpiIntVal:
      পক্ষ vpiVectorVal: {
	    vvp_vector4_t vec = vec4_from_vpi_মান(vp, get_word_size());
	    aobj->set_word(index, vec);
      }
      ভঙ্গ;

      পক্ষ vpiRealVal:
        aobj->set_word(index, vp->value.real);
        ভঙ্গ;

      পক্ষ vpiStringVal:
        aobj->set_word(index, std::string(vp->value.str));
        ভঙ্গ;

      স্বভাব:
          fprintf(stderr, "vpi sorry: format is not implemented");
          প্রতিজ্ঞা(অসত্য);
      }
}

vpiHandle __vpiDarrayVar::get_iter_index(সংরচনা __vpiArrayIterator*, পূর্ণাঙ্ক idx)
{
      যদি (vals_শব্দগুলি == 0) make_vals_শব্দগুলি();

      প্রত্যাগম &(vals_শব্দগুলি[idx].as_word);
}

পূর্ণাঙ্ক __vpiDarrayVar::vpi_get(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
	  পক্ষ vpiArrayType:
	    প্রত্যাগম vpiDynamicArray;
	  পক্ষ vpi_বাম_Range:
	    প্রত্যাগম 0;
	  পক্ষ vpi_ডান_Range:
	    প্রত্যাগম get_size() - 1;
	  পক্ষ vpiSize:
            প্রত্যাগম get_size();

	  স্বভাব:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    প্রতিজ্ঞা(অসত্য);
	    প্রত্যাগম 0;
      }
}

বর্ণ* __vpiDarrayVar::vpi_get_str(পূর্ণাঙ্ক code)
{
      যদি (code == vpiFile) {  // Not implemented for now!
            প্রত্যাগম simple_set_rbuf_str(file_names[0]);
      }

      প্রত্যাগম generic_get_str(code, scope_, name_, রিক্ত);
}

vpiHandle __vpiDarrayVar::vpi_handle(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
          পক্ষ vpi_বাম_Range:
            প্রত্যাগম get_বাম_range();

          পক্ষ vpi_ডান_Range:
            প্রত্যাগম get_ডান_range();

          পক্ষ vpiScope:
            প্রত্যাগম scope_;

          পক্ষ vpiModule:
            প্রত্যাগম vpip_module(scope_);
      }

      প্রত্যাগম 0;
}

vpiHandle __vpiDarrayVar::vpi_index(পূর্ণাঙ্ক index)
{
      যদি (index >= (long) get_size())
	    প্রত্যাগম 0;
      যদি (index < 0)
	    প্রত্যাগম 0;

      যদি (vals_শব্দগুলি == 0)
	    make_vals_শব্দগুলি();

      প্রত্যাগম &(vals_শব্দগুলি[index].as_word);
}

শূন্য __vpiDarrayVar::vpi_get_মান(p_vpi_মান val)
{
      val->format = vpiSuppressVal;
}

vvp_darray*__vpiDarrayVar::get_vvp_darray() ধ্রুব
{
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      প্রতিজ্ঞা(fun);
      vvp_object_t obj = fun->get_object();

      প্রত্যাগম obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(ধ্রুব বর্ণ*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ধ্রুব বর্ণ*use_name = name ? vpip_name_string(name) : রিক্ত;

      __vpiDarrayVar*obj = নূতন __vpiDarrayVar(scope, use_name, net);

      প্রত্যাগম obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, ধ্রুব বর্ণ*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

পূর্ণাঙ্ক __vpiQueueVar::vpi_get(পূর্ণাঙ্ক code)
{
      বিকল্প (code) {
	  পক্ষ vpiArrayType:
	    প্রত্যাগম vpiQueueArray;
	  স্বভাব:
	    প্রত্যাগম __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_queue_var(ধ্রুব বর্ণ*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ধ্রুব বর্ণ*use_name = name ? vpip_name_string(name) : রিক্ত;

      __vpiQueueVar*obj = নূতন __vpiQueueVar(scope, use_name, net);

      প্রত্যাগম obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, unsigned pidx,
				     তার্কিক is_queue)
: class_net_(0), prop_idx_(pidx), is_queue_(is_queue), scope_(scope)
{
}

পূর্ণাঙ্ক __vpiPropQueueRef::get_type_code(শূন্য) ধ্রুব
{ প্রত্যাগম vpiArrayVar; }

স্থির vvp_darray* get_live_darray_from_prop(vvp_net_t* class_net, unsigned pid)
{
      যদি (!class_net) প্রত্যাগম 0;
      ধ্রুব vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      যদি (!fun) প্রত্যাগম 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      যদি (!cobj) প্রত্যাগম 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      প্রত্যাগম qobj.peek<vvp_darray>();
}

পূর্ণাঙ্ক __vpiPropQueueRef::vpi_get(পূর্ণাঙ্ক code)
{
      ধ্রুব vvp_darray* aobj = get_live_darray_from_prop(class_net_, prop_idx_);
      বিকল্প (code) {
	  পক্ষ vpiArrayType:
	    প্রত্যাগম is_queue_ ? vpiQueueArray : vpiDynamicArray;
	  পক্ষ vpi_বাম_Range:
	    প্রত্যাগম 0;
	  পক্ষ vpi_ডান_Range:
	    প্রত্যাগম aobj ? (পূর্ণাঙ্ক) (aobj->get_size() - 1) : 0;
	  পক্ষ vpiSize:
	    প্রত্যাগম aobj ? (পূর্ণাঙ্ক) aobj->get_size() : 0;
	  স্বভাব:
	    প্রত্যাগম 0;
      }
}

বর্ণ* __vpiPropQueueRef::vpi_get_str(পূর্ণাঙ্ক code)
{
      যদি (code == vpiFile) {
	    প্রত্যাগম simple_set_rbuf_str(file_names[0]);
      }
      প্রত্যাগম generic_get_str(code, scope_, "prop_darray", রিক্ত);
}

শূন্য __vpiPropQueueRef::vpi_get_মান(p_vpi_মান val)
{
      val->format = vpiSuppressVal;
}

vpiHandle vpip_make_prop_queue_ref(বর্ণ* class_label, unsigned prop_idx,
				   unsigned is_queue_flag)
{
      __vpiPropQueueRef* obj = নূতন __vpiPropQueueRef(vpip_peek_current_scope(),
						     prop_idx,
						     is_queue_flag != 0);
      functor_ref_lookup(&obj->class_net_, class_label);
      প্রত্যাগম obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      যদি (!ref) প্রত্যাগম 0;

      যদি (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    প্রত্যাগম get_live_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      যদি (ধ্রুব __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    ধ্রুব vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    যদি (!fun) প্রত্যাগম 0;
	    vvp_object_t obj = fun->get_object();
	    প্রত্যাগম obj.peek<vvp_darray>();
      }

      প্রত্যাগম 0;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য array_delete(vpiHandle item)
{
      যদি (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(item)) {
	    delete pq;
	    প্রত্যাগম;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(item);
      যদি (dobj) {
	    যদি (dobj->vals_শব্দগুলি) delete [] (dobj->vals_শব্দগুলি-1);
	    delete dobj;
	    প্রত্যাগম;
      }

      fprintf(stderr, "Need support for deleting array type: %d\n", item->vpi_get(vpiArrayType));
      প্রতিজ্ঞা(0);
}
#endif
