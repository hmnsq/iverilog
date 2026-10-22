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

using ନାମାକାଶ std;

__vpiDarrayVar::__vpiDarrayVar(__vpiScope*sc, ଅଚଳ ଅକ୍ଷର*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

unsigned __vpiDarrayVar::get_size() ଅଚଳ
{
      ଅଚଳ vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      ଯଦି(!fun)
        ଫେରନ୍ତୁ 0;

      vvp_object_t val = fun->get_object();
      ଅଚଳ vvp_darray*aval = val.peek<vvp_darray>();

      ଯଦି(!aval)
        ଫେରନ୍ତୁ 0;

      ଫେରନ୍ତୁ aval->get_size();
}

vpiHandle __vpiDarrayVar::get_ବାମ_range()
{
      ବାମ_range.set_ମୂଲ୍ୟ(0);
      ଫେରନ୍ତୁ &ବାମ_range;
}

vpiHandle __vpiDarrayVar::get_ଡାହାଣ_range()
{
      ଡାହାଣ_range.set_ମୂଲ୍ୟ(get_size() - 1);
      ଫେରନ୍ତୁ &ଡାହାଣ_range;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiDarrayVar::get_word_size() ଅଚଳ
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_word(0, new_vec);
      ଫେରନ୍ତୁ new_vec.size();
}

ଅକ୍ଷର*__vpiDarrayVar::get_word_str(ସଂରଚନା __vpiArrayWord*word, ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      unsigned index = word->get_index();

      ଯଦି (code == vpiFile) {  // Not implemented for now!
	    ଫେରନ୍ତୁ simple_set_rbuf_str(file_names[0]);
      }

      ଅକ୍ଷର sidx [64];
      snprintf(sidx, 63, "%d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)index);
      ଫେରନ୍ତୁ generic_get_str(code, scope_, name_, sidx);
}

ଶୂନ୍ୟ __vpiDarrayVar::get_word_ମୂଲ୍ୟ(ସଂରଚନା __vpiArrayWord*word, p_vpi_ମୂଲ୍ୟ vp)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      ଯଦି(vp->format == vpiObjTypeVal) {
          ଯଦି(dynamic_cast<vvp_darray_real*>(aobj))
              vp->format = vpiRealVal;
          ଅନ୍ୟଥା ଯଦି(dynamic_cast<vvp_darray_string*>(aobj))
              vp->format = vpiStringVal;
          ଅନ୍ୟଥା
              vp->format = vpiVectorVal;
      }

      ବାଛ(vp->format) {
      ସ୍ଥିତି vpiBinStrVal:
      ସ୍ଥିତି vpiOctStrVal:
      ସ୍ଥିତି vpiDecStrVal:
      ସ୍ଥିତି vpiHexStrVal:
      ସ୍ଥିତି vpiScalarVal:
      ସ୍ଥିତି vpiIntVal:
      ସ୍ଥିତି vpiVectorVal:
      {
          vvp_vector4_t v;
          aobj->get_word(index, v);
          vpip_vec4_get_ମୂଲ୍ୟ(v, v.size(), ମିଥ୍ୟା, vp);  // TODO sign?
      }
      ଭାଙ୍ଗ;

      ସ୍ଥିତି vpiRealVal:
      {
          ଦ୍ୱିଗୁଣ d;
          aobj->get_word(index, d);
          vpip_real_get_ମୂଲ୍ୟ(d, vp);
      }
      ଭାଙ୍ଗ;

      ସ୍ଥିତି vpiStringVal:
      {
          string s;
          aobj->get_word(index, s);
          vpip_string_get_ମୂଲ୍ୟ(s, vp);
      }
      ଭାଙ୍ଗ;

      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
          fprintf(stderr, "vpi sorry: format is not implemented\n");
          ପ୍ରତିଜ୍ଞା(ମିଥ୍ୟା);
      }
}

ଶୂନ୍ୟ __vpiDarrayVar::put_word_ମୂଲ୍ୟ(ସଂରଚନା __vpiArrayWord*word, p_vpi_ମୂଲ୍ୟ vp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      ବାଛ(vp->format) {
      ସ୍ଥିତି vpiScalarVal:
      ସ୍ଥିତି vpiIntVal:
      ସ୍ଥିତି vpiVectorVal: {
	    vvp_vector4_t vec = vec4_from_vpi_ମୂଲ୍ୟ(vp, get_word_size());
	    aobj->set_word(index, vec);
      }
      ଭାଙ୍ଗ;

      ସ୍ଥିତି vpiRealVal:
        aobj->set_word(index, vp->value.real);
        ଭାଙ୍ଗ;

      ସ୍ଥିତି vpiStringVal:
        aobj->set_word(index, std::string(vp->value.str));
        ଭାଙ୍ଗ;

      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
          fprintf(stderr, "vpi sorry: format is not implemented");
          ପ୍ରତିଜ୍ଞା(ମିଥ୍ୟା);
      }
}

vpiHandle __vpiDarrayVar::get_iter_index(ସଂରଚନା __vpiArrayIterator*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
      ଯଦି (vals_ଶବ୍ଦଗୁଡ଼ିକ == 0) make_vals_ଶବ୍ଦଗୁଡ଼ିକ();

      ଫେରନ୍ତୁ &(vals_ଶବ୍ଦଗୁଡ଼ିକ[idx].as_word);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiDarrayVar::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiArrayType:
	    ଫେରନ୍ତୁ vpiDynamicArray;
	  ସ୍ଥିତି vpi_ବାମ_Range:
	    ଫେରନ୍ତୁ 0;
	  ସ୍ଥିତି vpi_ଡାହାଣ_Range:
	    ଫେରନ୍ତୁ get_size() - 1;
	  ସ୍ଥିତି vpiSize:
            ଫେରନ୍ତୁ get_size();

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    ପ୍ରତିଜ୍ଞା(ମିଥ୍ୟା);
	    ଫେରନ୍ତୁ 0;
      }
}

ଅକ୍ଷର* __vpiDarrayVar::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ଯଦି (code == vpiFile) {  // Not implemented for now!
            ଫେରନ୍ତୁ simple_set_rbuf_str(file_names[0]);
      }

      ଫେରନ୍ତୁ generic_get_str(code, scope_, name_, ଖାଲି);
}

vpiHandle __vpiDarrayVar::vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
          ସ୍ଥିତି vpi_ବାମ_Range:
            ଫେରନ୍ତୁ get_ବାମ_range();

          ସ୍ଥିତି vpi_ଡାହାଣ_Range:
            ଫେରନ୍ତୁ get_ଡାହାଣ_range();

          ସ୍ଥିତି vpiScope:
            ଫେରନ୍ତୁ scope_;

          ସ୍ଥିତି vpiModule:
            ଫେରନ୍ତୁ vpip_module(scope_);
      }

      ଫେରନ୍ତୁ 0;
}

vpiHandle __vpiDarrayVar::vpi_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index)
{
      ଯଦି (index >= (long) get_size())
	    ଫେରନ୍ତୁ 0;
      ଯଦି (index < 0)
	    ଫେରନ୍ତୁ 0;

      ଯଦି (vals_ଶବ୍ଦଗୁଡ଼ିକ == 0)
	    make_vals_ଶବ୍ଦଗୁଡ଼ିକ();

      ଫେରନ୍ତୁ &(vals_ଶବ୍ଦଗୁଡ଼ିକ[index].as_word);
}

ଶୂନ୍ୟ __vpiDarrayVar::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val)
{
      val->format = vpiSuppressVal;
}

vvp_darray*__vpiDarrayVar::get_vvp_darray() ଅଚଳ
{
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      ପ୍ରତିଜ୍ଞା(fun);
      vvp_object_t obj = fun->get_object();

      ଫେରନ୍ତୁ obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(ଅଚଳ ଅକ୍ଷର*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ଅଚଳ ଅକ୍ଷର*use_name = name ? vpip_name_string(name) : ଖାଲି;

      __vpiDarrayVar*obj = ନୂତନ __vpiDarrayVar(scope, use_name, net);

      ଫେରନ୍ତୁ obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, ଅଚଳ ଅକ୍ଷର*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiQueueVar::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiArrayType:
	    ଫେରନ୍ତୁ vpiQueueArray;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_queue_var(ଅଚଳ ଅକ୍ଷର*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ଅଚଳ ଅକ୍ଷର*use_name = name ? vpip_name_string(name) : ଖାଲି;

      __vpiQueueVar*obj = ନୂତନ __vpiQueueVar(scope, use_name, net);

      ଫେରନ୍ତୁ obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, unsigned pidx,
				     ତାର୍କିକ is_queue)
: class_net_(0), prop_idx_(pidx), is_queue_(is_queue), scope_(scope)
{
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiPropQueueRef::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiArrayVar; }

ସ୍ଥିର vvp_darray* get_live_darray_from_prop(vvp_net_t* class_net, unsigned pid)
{
      ଯଦି (!class_net) ଫେରନ୍ତୁ 0;
      ଅଚଳ vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      ଯଦି (!fun) ଫେରନ୍ତୁ 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      ଯଦି (!cobj) ଫେରନ୍ତୁ 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      ଫେରନ୍ତୁ qobj.peek<vvp_darray>();
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiPropQueueRef::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ଅଚଳ vvp_darray* aobj = get_live_darray_from_prop(class_net_, prop_idx_);
      ବାଛ (code) {
	  ସ୍ଥିତି vpiArrayType:
	    ଫେରନ୍ତୁ is_queue_ ? vpiQueueArray : vpiDynamicArray;
	  ସ୍ଥିତି vpi_ବାମ_Range:
	    ଫେରନ୍ତୁ 0;
	  ସ୍ଥିତି vpi_ଡାହାଣ_Range:
	    ଫେରନ୍ତୁ aobj ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) (aobj->get_size() - 1) : 0;
	  ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ aobj ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) aobj->get_size() : 0;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ 0;
      }
}

ଅକ୍ଷର* __vpiPropQueueRef::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ଯଦି (code == vpiFile) {
	    ଫେରନ୍ତୁ simple_set_rbuf_str(file_names[0]);
      }
      ଫେରନ୍ତୁ generic_get_str(code, scope_, "prop_darray", ଖାଲି);
}

ଶୂନ୍ୟ __vpiPropQueueRef::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val)
{
      val->format = vpiSuppressVal;
}

vpiHandle vpip_make_prop_queue_ref(ଅକ୍ଷର* class_label, unsigned prop_idx,
				   unsigned is_queue_flag)
{
      __vpiPropQueueRef* obj = ନୂତନ __vpiPropQueueRef(vpip_peek_current_scope(),
						     prop_idx,
						     is_queue_flag != 0);
      functor_ref_lookup(&obj->class_net_, class_label);
      ଫେରନ୍ତୁ obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      ଯଦି (!ref) ଫେରନ୍ତୁ 0;

      ଯଦି (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    ଫେରନ୍ତୁ get_live_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      ଯଦି (ଅଚଳ __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    ଅଚଳ vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    ଯଦି (!fun) ଫେରନ୍ତୁ 0;
	    vvp_object_t obj = fun->get_object();
	    ଫେରନ୍ତୁ obj.peek<vvp_darray>();
      }

      ଫେରନ୍ତୁ 0;
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ array_delete(vpiHandle item)
{
      ଯଦି (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(item)) {
	    delete pq;
	    ଫେରନ୍ତୁ;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(item);
      ଯଦି (dobj) {
	    ଯଦି (dobj->vals_ଶବ୍ଦଗୁଡ଼ିକ) delete [] (dobj->vals_ଶବ୍ଦଗୁଡ଼ିକ-1);
	    delete dobj;
	    ଫେରନ୍ତୁ;
      }

      fprintf(stderr, "Need support for deleting array type: %d\n", item->vpi_get(vpiArrayType));
      ପ୍ରତିଜ୍ଞା(0);
}
#endif
