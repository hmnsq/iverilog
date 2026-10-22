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

using namespace std;

__vpiDarrayVar::__vpiDarrayVar(__vpiScope*sc, const char*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

unsigned __vpiDarrayVar::get_size() const
{
      const vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      만약(!fun)
        반환 0;

      vvp_object_t val = fun->get_object();
      const vvp_darray*aval = val.peek<vvp_darray>();

      만약(!aval)
        반환 0;

      반환 aval->get_size();
}

vpiHandle __vpiDarrayVar::get_left_range()
{
      left_range_.set_value(0);
      반환 &left_range_;
}

vpiHandle __vpiDarrayVar::get_right_range()
{
      right_range_.set_value(get_size() - 1);
      반환 &right_range_;
}

정수 __vpiDarrayVar::get_word_size() const
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_word(0, new_vec);
      반환 new_vec.size();
}

char*__vpiDarrayVar::get_word_str(struct __vpiArrayWord*word, 정수 code)
{
      unsigned index = word->get_index();

      만약 (code == vpiFile) {  // Not implemented for now!
	    반환 simple_set_rbuf_str(file_names[0]);
      }

      char sidx [64];
      snprintf(sidx, 63, "%d", (정수)index);
      반환 generic_get_str(code, scope_, name_, sidx);
}

void __vpiDarrayVar::get_word_value(struct __vpiArrayWord*word, p_vpi_value vp)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      만약(vp->format == vpiObjTypeVal) {
          만약(dynamic_cast<vvp_darray_real*>(aobj))
              vp->format = vpiRealVal;
          아니면 만약(dynamic_cast<vvp_darray_string*>(aobj))
              vp->format = vpiStringVal;
          아니면
              vp->format = vpiVectorVal;
      }

      switch(vp->format) {
      case vpiBinStrVal:
      case vpiOctStrVal:
      case vpiDecStrVal:
      case vpiHexStrVal:
      case vpiScalarVal:
      case vpiIntVal:
      case vpiVectorVal:
      {
          vvp_vector4_t v;
          aobj->get_word(index, v);
          vpip_vec4_get_value(v, v.size(), false, vp);  // TODO sign?
      }
      break;

      case vpiRealVal:
      {
          double d;
          aobj->get_word(index, d);
          vpip_real_get_value(d, vp);
      }
      break;

      case vpiStringVal:
      {
          string s;
          aobj->get_word(index, s);
          vpip_string_get_value(s, vp);
      }
      break;

      default:
          fprintf(stderr, "vpi sorry: format is not implemented\n");
          assert(false);
      }
}

void __vpiDarrayVar::put_word_value(struct __vpiArrayWord*word, p_vpi_value vp, 정수)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      switch(vp->format) {
      case vpiScalarVal:
      case vpiIntVal:
      case vpiVectorVal: {
	    vvp_vector4_t vec = vec4_from_vpi_value(vp, get_word_size());
	    aobj->set_word(index, vec);
      }
      break;

      case vpiRealVal:
        aobj->set_word(index, vp->value.real);
        break;

      case vpiStringVal:
        aobj->set_word(index, std::string(vp->value.str));
        break;

      default:
          fprintf(stderr, "vpi sorry: format is not implemented");
          assert(false);
      }
}

vpiHandle __vpiDarrayVar::get_iter_index(struct __vpiArrayIterator*, 정수 idx)
{
      만약 (vals_words == 0) make_vals_words();

      반환 &(vals_words[idx].as_word);
}

정수 __vpiDarrayVar::vpi_get(정수 code)
{
      switch (code) {
	  case vpiArrayType:
	    반환 vpiDynamicArray;
	  case vpiLeftRange:
	    반환 0;
	  case vpiRightRange:
	    반환 get_size() - 1;
	  case vpiSize:
            반환 get_size();

	  default:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    assert(false);
	    반환 0;
      }
}

char* __vpiDarrayVar::vpi_get_str(정수 code)
{
      만약 (code == vpiFile) {  // Not implemented for now!
            반환 simple_set_rbuf_str(file_names[0]);
      }

      반환 generic_get_str(code, scope_, name_, NULL);
}

vpiHandle __vpiDarrayVar::vpi_handle(정수 code)
{
      switch (code) {
          case vpiLeftRange:
            반환 get_left_range();

          case vpiRightRange:
            반환 get_right_range();

          case vpiScope:
            반환 scope_;

          case vpiModule:
            반환 vpip_module(scope_);
      }

      반환 0;
}

vpiHandle __vpiDarrayVar::vpi_index(정수 index)
{
      만약 (index >= (long) get_size())
	    반환 0;
      만약 (index < 0)
	    반환 0;

      만약 (vals_words == 0)
	    make_vals_words();

      반환 &(vals_words[index].as_word);
}

void __vpiDarrayVar::vpi_get_value(p_vpi_value val)
{
      val->format = vpiSuppressVal;
}

vvp_darray*__vpiDarrayVar::get_vvp_darray() const
{
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      assert(fun);
      vvp_object_t obj = fun->get_object();

      반환 obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(const char*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      const char*use_name = name ? vpip_name_string(name) : NULL;

      __vpiDarrayVar*obj = new __vpiDarrayVar(scope, use_name, net);

      반환 obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, const char*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

정수 __vpiQueueVar::vpi_get(정수 code)
{
      switch (code) {
	  case vpiArrayType:
	    반환 vpiQueueArray;
	  default:
	    반환 __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_queue_var(const char*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      const char*use_name = name ? vpip_name_string(name) : NULL;

      __vpiQueueVar*obj = new __vpiQueueVar(scope, use_name, net);

      반환 obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, unsigned pidx,
				     bool is_queue)
: class_net_(0), prop_idx_(pidx), is_queue_(is_queue), scope_(scope)
{
}

정수 __vpiPropQueueRef::get_type_code(void) const
{ 반환 vpiArrayVar; }

static vvp_darray* get_live_darray_from_prop(vvp_net_t* class_net, unsigned pid)
{
      만약 (!class_net) 반환 0;
      const vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      만약 (!fun) 반환 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      만약 (!cobj) 반환 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      반환 qobj.peek<vvp_darray>();
}

정수 __vpiPropQueueRef::vpi_get(정수 code)
{
      const vvp_darray* aobj = get_live_darray_from_prop(class_net_, prop_idx_);
      switch (code) {
	  case vpiArrayType:
	    반환 is_queue_ ? vpiQueueArray : vpiDynamicArray;
	  case vpiLeftRange:
	    반환 0;
	  case vpiRightRange:
	    반환 aobj ? (정수) (aobj->get_size() - 1) : 0;
	  case vpiSize:
	    반환 aobj ? (정수) aobj->get_size() : 0;
	  default:
	    반환 0;
      }
}

char* __vpiPropQueueRef::vpi_get_str(정수 code)
{
      만약 (code == vpiFile) {
	    반환 simple_set_rbuf_str(file_names[0]);
      }
      반환 generic_get_str(code, scope_, "prop_darray", NULL);
}

void __vpiPropQueueRef::vpi_get_value(p_vpi_value val)
{
      val->format = vpiSuppressVal;
}

vpiHandle vpip_make_prop_queue_ref(char* class_label, unsigned prop_idx,
				   unsigned is_queue_flag)
{
      __vpiPropQueueRef* obj = new __vpiPropQueueRef(vpip_peek_current_scope(),
						     prop_idx,
						     is_queue_flag != 0);
      functor_ref_lookup(&obj->class_net_, class_label);
      반환 obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      만약 (!ref) 반환 0;

      만약 (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    반환 get_live_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      만약 (const __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    const vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    만약 (!fun) 반환 0;
	    vvp_object_t obj = fun->get_object();
	    반환 obj.peek<vvp_darray>();
      }

      반환 0;
}

#ifdef CHECK_WITH_VALGRIND
void array_delete(vpiHandle item)
{
      만약 (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(item)) {
	    delete pq;
	    반환;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(item);
      만약 (dobj) {
	    만약 (dobj->vals_words) delete [] (dobj->vals_words-1);
	    delete dobj;
	    반환;
      }

      fprintf(stderr, "Need support for deleting array type: %d\n", item->vpi_get(vpiArrayType));
      assert(0);
}
#endif
