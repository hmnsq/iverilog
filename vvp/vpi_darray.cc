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

__vpiDarrayVar::__vpiDarrayVar(__vpiScope*sc, ধ্রুৱ ৱর্ণ*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

unsigned __vpiDarrayVar::get_size() ধ্রুৱ
{
      ধ্রুৱ vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      যদি(!fun)
        ঘূৰাই 0;

      vvp_object_t val = fun->get_object();
      ধ্রুৱ vvp_darray*aval = val.peek<vvp_darray>();

      যদি(!aval)
        ঘূৰাই 0;

      ঘূৰাই aval->get_size();
}

vpiHandle __vpiDarrayVar::get_বাম_range()
{
      বাম_range.set_মান(0);
      ঘূৰাই &বাম_range;
}

vpiHandle __vpiDarrayVar::get_ডান_range()
{
      ডান_range.set_মান(get_size() - 1);
      ঘূৰাই &ডান_range;
}

পূর্ণাঙ্ক __vpiDarrayVar::get_word_size() ধ্রুৱ
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_word(0, new_vec);
      ঘূৰাই new_vec.size();
}

ৱর্ণ*__vpiDarrayVar::get_word_str(সংরচনা __vpiArrayWord*word, পূর্ণাঙ্ক code)
{
      unsigned index = word->get_index();

      যদি (code == vpiFile) {  // Not implemented for now!
	    ঘূৰাই simple_set_rbuf_str(file_names[0]);
      }

      ৱর্ণ sidx [64];
      snprintf(sidx, 63, "%d", (পূর্ণাঙ্ক)index);
      ঘূৰাই generic_get_str(code, scope_, name_, sidx);
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

      ৱিকল্প(vp->format) {
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
          vpip_vec4_get_মান(v, v.size(), মিছা, vp);  // TODO sign?
      }
      ভঙ্গ;

      পক্ষ vpiRealVal:
      {
          দ্ৱিগুণ d;
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

      স্ৱভাৱ:
          fprintf(stderr, "vpi sorry: format is not implemented\n");
          প্রতিজ্ঞা(মিছা);
      }
}

শূন্য __vpiDarrayVar::put_word_মান(সংরচনা __vpiArrayWord*word, p_vpi_মান vp, পূর্ণাঙ্ক)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      ৱিকল্প(vp->format) {
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

      স্ৱভাৱ:
          fprintf(stderr, "vpi sorry: format is not implemented");
          প্রতিজ্ঞা(মিছা);
      }
}

vpiHandle __vpiDarrayVar::get_iter_index(সংরচনা __vpiArrayIterator*, পূর্ণাঙ্ক idx)
{
      যদি (vals_শব্দগুলি == 0) make_vals_শব্দগুলি();

      ঘূৰাই &(vals_শব্দগুলি[idx].as_word);
}

পূর্ণাঙ্ক __vpiDarrayVar::vpi_get(পূর্ণাঙ্ক code)
{
      ৱিকল্প (code) {
	  পক্ষ vpiArrayType:
	    ঘূৰাই vpiDynamicArray;
	  পক্ষ vpi_বাম_Range:
	    ঘূৰাই 0;
	  পক্ষ vpi_ডান_Range:
	    ঘূৰাই get_size() - 1;
	  পক্ষ vpiSize:
            ঘূৰাই get_size();

	  স্ৱভাৱ:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    প্রতিজ্ঞা(মিছা);
	    ঘূৰাই 0;
      }
}

ৱর্ণ* __vpiDarrayVar::vpi_get_str(পূর্ণাঙ্ক code)
{
      যদি (code == vpiFile) {  // Not implemented for now!
            ঘূৰাই simple_set_rbuf_str(file_names[0]);
      }

      ঘূৰাই generic_get_str(code, scope_, name_, রিক্ত);
}

vpiHandle __vpiDarrayVar::vpi_handle(পূর্ণাঙ্ক code)
{
      ৱিকল্প (code) {
          পক্ষ vpi_বাম_Range:
            ঘূৰাই get_বাম_range();

          পক্ষ vpi_ডান_Range:
            ঘূৰাই get_ডান_range();

          পক্ষ vpiScope:
            ঘূৰাই scope_;

          পক্ষ vpiModule:
            ঘূৰাই vpip_module(scope_);
      }

      ঘূৰাই 0;
}

vpiHandle __vpiDarrayVar::vpi_index(পূর্ণাঙ্ক index)
{
      যদি (index >= (long) get_size())
	    ঘূৰাই 0;
      যদি (index < 0)
	    ঘূৰাই 0;

      যদি (vals_শব্দগুলি == 0)
	    make_vals_শব্দগুলি();

      ঘূৰাই &(vals_শব্দগুলি[index].as_word);
}

শূন্য __vpiDarrayVar::vpi_get_মান(p_vpi_মান val)
{
      val->format = vpiSuppressVal;
}

vvp_darray*__vpiDarrayVar::get_vvp_darray() ধ্রুৱ
{
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      প্রতিজ্ঞা(fun);
      vvp_object_t obj = fun->get_object();

      ঘূৰাই obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(ধ্রুৱ ৱর্ণ*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ধ্রুৱ ৱর্ণ*use_name = name ? vpip_name_string(name) : রিক্ত;

      __vpiDarrayVar*obj = নূতন __vpiDarrayVar(scope, use_name, net);

      ঘূৰাই obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, ধ্রুৱ ৱর্ণ*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

পূর্ণাঙ্ক __vpiQueueVar::vpi_get(পূর্ণাঙ্ক code)
{
      ৱিকল্প (code) {
	  পক্ষ vpiArrayType:
	    ঘূৰাই vpiQueueArray;
	  স্ৱভাৱ:
	    ঘূৰাই __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_queue_var(ধ্রুৱ ৱর্ণ*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ধ্রুৱ ৱর্ণ*use_name = name ? vpip_name_string(name) : রিক্ত;

      __vpiQueueVar*obj = নূতন __vpiQueueVar(scope, use_name, net);

      ঘূৰাই obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, unsigned pidx,
				     তার্কিক is_queue)
: class_net_(0), prop_idx_(pidx), is_queue_(is_queue), scope_(scope)
{
}

পূর্ণাঙ্ক __vpiPropQueueRef::get_type_code(শূন্য) ধ্রুৱ
{ ঘূৰাই vpiArrayVar; }

স্থির vvp_darray* get_live_darray_from_prop(vvp_net_t* class_net, unsigned pid)
{
      যদি (!class_net) ঘূৰাই 0;
      ধ্রুৱ vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      যদি (!fun) ঘূৰাই 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      যদি (!cobj) ঘূৰাই 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      ঘূৰাই qobj.peek<vvp_darray>();
}

পূর্ণাঙ্ক __vpiPropQueueRef::vpi_get(পূর্ণাঙ্ক code)
{
      ধ্রুৱ vvp_darray* aobj = get_live_darray_from_prop(class_net_, prop_idx_);
      ৱিকল্প (code) {
	  পক্ষ vpiArrayType:
	    ঘূৰাই is_queue_ ? vpiQueueArray : vpiDynamicArray;
	  পক্ষ vpi_বাম_Range:
	    ঘূৰাই 0;
	  পক্ষ vpi_ডান_Range:
	    ঘূৰাই aobj ? (পূর্ণাঙ্ক) (aobj->get_size() - 1) : 0;
	  পক্ষ vpiSize:
	    ঘূৰাই aobj ? (পূর্ণাঙ্ক) aobj->get_size() : 0;
	  স্ৱভাৱ:
	    ঘূৰাই 0;
      }
}

ৱর্ণ* __vpiPropQueueRef::vpi_get_str(পূর্ণাঙ্ক code)
{
      যদি (code == vpiFile) {
	    ঘূৰাই simple_set_rbuf_str(file_names[0]);
      }
      ঘূৰাই generic_get_str(code, scope_, "prop_darray", রিক্ত);
}

শূন্য __vpiPropQueueRef::vpi_get_মান(p_vpi_মান val)
{
      val->format = vpiSuppressVal;
}

vpiHandle vpip_make_prop_queue_ref(ৱর্ণ* class_label, unsigned prop_idx,
				   unsigned is_queue_flag)
{
      __vpiPropQueueRef* obj = নূতন __vpiPropQueueRef(vpip_peek_current_scope(),
						     prop_idx,
						     is_queue_flag != 0);
      functor_ref_lookup(&obj->class_net_, class_label);
      ঘূৰাই obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      যদি (!ref) ঘূৰাই 0;

      যদি (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    ঘূৰাই get_live_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      যদি (ধ্রুৱ __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    ধ্রুৱ vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    যদি (!fun) ঘূৰাই 0;
	    vvp_object_t obj = fun->get_object();
	    ঘূৰাই obj.peek<vvp_darray>();
      }

      ঘূৰাই 0;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য array_delete(vpiHandle item)
{
      যদি (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(item)) {
	    delete pq;
	    ঘূৰাই;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(item);
      যদি (dobj) {
	    যদি (dobj->vals_শব্দগুলি) delete [] (dobj->vals_শব্দগুলি-1);
	    delete dobj;
	    ঘূৰাই;
      }

      fprintf(stderr, "Need support for deleting array type: %d\n", item->vpi_get(vpiArrayType));
      প্রতিজ্ঞা(0);
}
#endif
