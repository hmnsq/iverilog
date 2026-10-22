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

__vpiDarrayVar::__vpiDarrayVar(__vpiScope*sc, ধ্রুবক অক্ষর*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

unsigned __vpiDarrayVar::get_আকার() ধ্রুবক
{
      ধ্রুবক vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      যদি(!fun)
        ফেরত 0;

      vvp_object_t val = fun->get_object();
      ধ্রুবক vvp_darray*aval = val.peek<vvp_darray>();

      যদি(!aval)
        ফেরত 0;

      ফেরত aval->get_আকার();
}

vpiHandle __vpiDarrayVar::get_বাম_range()
{
      বাম_range.set_মান(0);
      ফেরত &বাম_range;
}

vpiHandle __vpiDarrayVar::get_ডান_range()
{
      ডান_range.set_মান(get_আকার() - 1);
      ফেরত &ডান_range;
}

পূর্ণসংখ্যা __vpiDarrayVar::get_word_আকার() ধ্রুবক
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_word(0, new_vec);
      ফেরত new_vec.size();
}

অক্ষর*__vpiDarrayVar::get_word_str(কাঠামো __vpiArrayWord*word, পূর্ণসংখ্যা code)
{
      unsigned সূচক = word->get_সূচক();

      যদি (code == vpiFile) {  // Not implemented for now!
	    ফেরত simple_set_rbuf_str(file_names[0]);
      }

      অক্ষর sidx [64];
      snprintf(sidx, 63, "%d", (পূর্ণসংখ্যা)সূচক);
      ফেরত generic_get_str(code, scope_, name_, sidx);
}

শূন্য __vpiDarrayVar::get_word_মান(কাঠামো __vpiArrayWord*word, p_vpi_মান vp)
{
      unsigned সূচক = word->get_সূচক();
      vvp_darray*aobj = get_vvp_darray();

      যদি(vp->format == vpiObjTypeVal) {
          যদি(dynamic_cast<vvp_darray_real*>(aobj))
              vp->format = vpiRealVal;
          অন্যথা যদি(dynamic_cast<vvp_darray_string*>(aobj))
              vp->format = vpiStringVal;
          অন্যথা
              vp->format = vpiVectorVal;
      }

      নির্বাচন(vp->format) {
      ক্ষেত্র vpiBinStrVal:
      ক্ষেত্র vpiOctStrVal:
      ক্ষেত্র vpiDecStrVal:
      ক্ষেত্র vpiHexStrVal:
      ক্ষেত্র vpiScalarVal:
      ক্ষেত্র vpiIntVal:
      ক্ষেত্র vpiVectorVal:
      {
          vvp_vector4_t v;
          aobj->get_word(সূচক, v);
          vpip_vec4_get_মান(v, v.size(), মিথ্যা, vp);  // TODO sign?
      }
      বিরতি;

      ক্ষেত্র vpiRealVal:
      {
          দ্বিগুণ d;
          aobj->get_word(সূচক, d);
          vpip_real_get_মান(d, vp);
      }
      বিরতি;

      ক্ষেত্র vpiStringVal:
      {
          string s;
          aobj->get_word(সূচক, s);
          vpip_string_get_মান(s, vp);
      }
      বিরতি;

      পূর্বনির্ধারিত:
          fprintf(stderr, "vpi sorry: format is not implemented\n");
          প্রতিজ্ঞা(মিথ্যা);
      }
}

শূন্য __vpiDarrayVar::put_word_মান(কাঠামো __vpiArrayWord*word, p_vpi_মান vp, পূর্ণসংখ্যা)
{
      unsigned সূচক = word->get_সূচক();
      vvp_darray*aobj = get_vvp_darray();

      নির্বাচন(vp->format) {
      ক্ষেত্র vpiScalarVal:
      ক্ষেত্র vpiIntVal:
      ক্ষেত্র vpiVectorVal: {
	    vvp_vector4_t vec = vec4_from_vpi_মান(vp, get_word_আকার());
	    aobj->set_word(সূচক, vec);
      }
      বিরতি;

      ক্ষেত্র vpiRealVal:
        aobj->set_word(সূচক, vp->value.real);
        বিরতি;

      ক্ষেত্র vpiStringVal:
        aobj->set_word(সূচক, std::string(vp->value.str));
        বিরতি;

      পূর্বনির্ধারিত:
          fprintf(stderr, "vpi sorry: format is not implemented");
          প্রতিজ্ঞা(মিথ্যা);
      }
}

vpiHandle __vpiDarrayVar::get_iter_সূচক(কাঠামো __vpiArrayIterator*, পূর্ণসংখ্যা idx)
{
      যদি (vals_শব্দগুলি == 0) make_vals_শব্দগুলি();

      ফেরত &(vals_শব্দগুলি[idx].as_word);
}

পূর্ণসংখ্যা __vpiDarrayVar::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpiArrayType:
	    ফেরত vpiDynamicArray;
	  ক্ষেত্র vpi_বাম_Range:
	    ফেরত 0;
	  ক্ষেত্র vpi_ডান_Range:
	    ফেরত get_আকার() - 1;
	  ক্ষেত্র vpi_আকার:
            ফেরত get_আকার();

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    প্রতিজ্ঞা(মিথ্যা);
	    ফেরত 0;
      }
}

অক্ষর* __vpiDarrayVar::vpi_get_str(পূর্ণসংখ্যা code)
{
      যদি (code == vpiFile) {  // Not implemented for now!
            ফেরত simple_set_rbuf_str(file_names[0]);
      }

      ফেরত generic_get_str(code, scope_, name_, ফাঁকা);
}

vpiHandle __vpiDarrayVar::vpi_handle(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
          ক্ষেত্র vpi_বাম_Range:
            ফেরত get_বাম_range();

          ক্ষেত্র vpi_ডান_Range:
            ফেরত get_ডান_range();

          ক্ষেত্র vpiScope:
            ফেরত scope_;

          ক্ষেত্র vpiModule:
            ফেরত vpip_module(scope_);
      }

      ফেরত 0;
}

vpiHandle __vpiDarrayVar::vpi_সূচক(পূর্ণসংখ্যা সূচক)
{
      যদি (সূচক >= (long) get_আকার())
	    ফেরত 0;
      যদি (সূচক < 0)
	    ফেরত 0;

      যদি (vals_শব্দগুলি == 0)
	    make_vals_শব্দগুলি();

      ফেরত &(vals_শব্দগুলি[সূচক].as_word);
}

শূন্য __vpiDarrayVar::vpi_get_মান(p_vpi_মান val)
{
      val->format = vpiSuppressVal;
}

vvp_darray*__vpiDarrayVar::get_vvp_darray() ধ্রুবক
{
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      প্রতিজ্ঞা(fun);
      vvp_object_t obj = fun->get_object();

      ফেরত obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(ধ্রুবক অক্ষর*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ধ্রুবক অক্ষর*use_name = name ? vpip_name_string(name) : ফাঁকা;

      __vpiDarrayVar*obj = নতুন __vpiDarrayVar(scope, use_name, net);

      ফেরত obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, ধ্রুবক অক্ষর*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

পূর্ণসংখ্যা __vpiQueueVar::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpiArrayType:
	    ফেরত vpiQueueArray;
	  পূর্বনির্ধারিত:
	    ফেরত __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_queue_var(ধ্রুবক অক্ষর*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ধ্রুবক অক্ষর*use_name = name ? vpip_name_string(name) : ফাঁকা;

      __vpiQueueVar*obj = নতুন __vpiQueueVar(scope, use_name, net);

      ফেরত obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, unsigned pidx,
				     যৌক্তিক is_queue)
: class_net_(0), prop_idx_(pidx), is_queue_(is_queue), scope_(scope)
{
}

পূর্ণসংখ্যা __vpiPropQueueRef::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiArrayVar; }

স্থির vvp_darray* get_live_darray_from_prop(vvp_net_t* class_net, unsigned pid)
{
      যদি (!class_net) ফেরত 0;
      ধ্রুবক vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      যদি (!fun) ফেরত 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      যদি (!cobj) ফেরত 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      ফেরত qobj.peek<vvp_darray>();
}

পূর্ণসংখ্যা __vpiPropQueueRef::vpi_get(পূর্ণসংখ্যা code)
{
      ধ্রুবক vvp_darray* aobj = get_live_darray_from_prop(class_net_, prop_idx_);
      নির্বাচন (code) {
	  ক্ষেত্র vpiArrayType:
	    ফেরত is_queue_ ? vpiQueueArray : vpiDynamicArray;
	  ক্ষেত্র vpi_বাম_Range:
	    ফেরত 0;
	  ক্ষেত্র vpi_ডান_Range:
	    ফেরত aobj ? (পূর্ণসংখ্যা) (aobj->get_আকার() - 1) : 0;
	  ক্ষেত্র vpi_আকার:
	    ফেরত aobj ? (পূর্ণসংখ্যা) aobj->get_আকার() : 0;
	  পূর্বনির্ধারিত:
	    ফেরত 0;
      }
}

অক্ষর* __vpiPropQueueRef::vpi_get_str(পূর্ণসংখ্যা code)
{
      যদি (code == vpiFile) {
	    ফেরত simple_set_rbuf_str(file_names[0]);
      }
      ফেরত generic_get_str(code, scope_, "prop_darray", ফাঁকা);
}

শূন্য __vpiPropQueueRef::vpi_get_মান(p_vpi_মান val)
{
      val->format = vpiSuppressVal;
}

vpiHandle vpip_make_prop_queue_ref(অক্ষর* class_label, unsigned prop_idx,
				   unsigned is_queue_flag)
{
      __vpiPropQueueRef* obj = নতুন __vpiPropQueueRef(vpip_peek_current_scope(),
						     prop_idx,
						     is_queue_flag != 0);
      functor_ref_lookup(&obj->class_net_, class_label);
      ফেরত obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      যদি (!ref) ফেরত 0;

      যদি (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    ফেরত get_live_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      যদি (ধ্রুবক __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    ধ্রুবক vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    যদি (!fun) ফেরত 0;
	    vvp_object_t obj = fun->get_object();
	    ফেরত obj.peek<vvp_darray>();
      }

      ফেরত 0;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য array_delete(vpiHandle item)
{
      যদি (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(item)) {
	    delete pq;
	    ফেরত;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(item);
      যদি (dobj) {
	    যদি (dobj->vals_শব্দগুলি) delete [] (dobj->vals_শব্দগুলি-1);
	    delete dobj;
	    ফেরত;
      }

      fprintf(stderr, "Need support for deleting array type: %d\n", item->vpi_get(vpiArrayType));
      প্রতিজ্ঞা(0);
}
#endif
