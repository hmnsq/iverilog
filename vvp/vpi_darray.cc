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
      nếu(!fun)
        trả_về 0;

      vvp_object_t val = fun->get_object();
      const vvp_darray*aval = val.peek<vvp_darray>();

      nếu(!aval)
        trả_về 0;

      trả_về aval->get_size();
}

vpiHandle __vpiDarrayVar::get_left_range()
{
      left_range_.set_value(0);
      trả_về &left_range_;
}

vpiHandle __vpiDarrayVar::get_right_range()
{
      right_range_.set_value(get_size() - 1);
      trả_về &right_range_;
}

số_nguyên __vpiDarrayVar::get_word_size() const
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_word(0, new_vec);
      trả_về new_vec.size();
}

char*__vpiDarrayVar::get_word_str(struct __vpiArrayWord*word, số_nguyên code)
{
      unsigned index = word->get_index();

      nếu (code == vpiFile) {  // Not implemented for now!
	    trả_về simple_set_rbuf_str(file_names[0]);
      }

      char sidx [64];
      snprintf(sidx, 63, "%d", (số_nguyên)index);
      trả_về generic_get_str(code, scope_, name_, sidx);
}

void __vpiDarrayVar::get_word_value(struct __vpiArrayWord*word, p_vpi_value vp)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      nếu(vp->format == vpiObjTypeVal) {
          nếu(dynamic_cast<vvp_darray_real*>(aobj))
              vp->format = vpiRealVal;
          không_thì nếu(dynamic_cast<vvp_darray_string*>(aobj))
              vp->format = vpiStringVal;
          không_thì
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

void __vpiDarrayVar::put_word_value(struct __vpiArrayWord*word, p_vpi_value vp, số_nguyên)
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

vpiHandle __vpiDarrayVar::get_iter_index(struct __vpiArrayIterator*, số_nguyên idx)
{
      nếu (vals_words == 0) make_vals_words();

      trả_về &(vals_words[idx].as_word);
}

số_nguyên __vpiDarrayVar::vpi_get(số_nguyên code)
{
      switch (code) {
	  case vpiArrayType:
	    trả_về vpiDynamicArray;
	  case vpiLeftRange:
	    trả_về 0;
	  case vpiRightRange:
	    trả_về get_size() - 1;
	  case vpiSize:
            trả_về get_size();

	  default:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    assert(false);
	    trả_về 0;
      }
}

char* __vpiDarrayVar::vpi_get_str(số_nguyên code)
{
      nếu (code == vpiFile) {  // Not implemented for now!
            trả_về simple_set_rbuf_str(file_names[0]);
      }

      trả_về generic_get_str(code, scope_, name_, NULL);
}

vpiHandle __vpiDarrayVar::vpi_handle(số_nguyên code)
{
      switch (code) {
          case vpiLeftRange:
            trả_về get_left_range();

          case vpiRightRange:
            trả_về get_right_range();

          case vpiScope:
            trả_về scope_;

          case vpiModule:
            trả_về vpip_module(scope_);
      }

      trả_về 0;
}

vpiHandle __vpiDarrayVar::vpi_index(số_nguyên index)
{
      nếu (index >= (long) get_size())
	    trả_về 0;
      nếu (index < 0)
	    trả_về 0;

      nếu (vals_words == 0)
	    make_vals_words();

      trả_về &(vals_words[index].as_word);
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

      trả_về obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(const char*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      const char*use_name = name ? vpip_name_string(name) : NULL;

      __vpiDarrayVar*obj = new __vpiDarrayVar(scope, use_name, net);

      trả_về obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, const char*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

số_nguyên __vpiQueueVar::vpi_get(số_nguyên code)
{
      switch (code) {
	  case vpiArrayType:
	    trả_về vpiQueueArray;
	  default:
	    trả_về __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_queue_var(const char*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      const char*use_name = name ? vpip_name_string(name) : NULL;

      __vpiQueueVar*obj = new __vpiQueueVar(scope, use_name, net);

      trả_về obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, unsigned pidx,
				     bool is_queue)
: class_net_(0), prop_idx_(pidx), is_queue_(is_queue), scope_(scope)
{
}

số_nguyên __vpiPropQueueRef::get_type_code(void) const
{ trả_về vpiArrayVar; }

static vvp_darray* get_live_darray_from_prop(vvp_net_t* class_net, unsigned pid)
{
      nếu (!class_net) trả_về 0;
      const vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      nếu (!fun) trả_về 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      nếu (!cobj) trả_về 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      trả_về qobj.peek<vvp_darray>();
}

số_nguyên __vpiPropQueueRef::vpi_get(số_nguyên code)
{
      const vvp_darray* aobj = get_live_darray_from_prop(class_net_, prop_idx_);
      switch (code) {
	  case vpiArrayType:
	    trả_về is_queue_ ? vpiQueueArray : vpiDynamicArray;
	  case vpiLeftRange:
	    trả_về 0;
	  case vpiRightRange:
	    trả_về aobj ? (số_nguyên) (aobj->get_size() - 1) : 0;
	  case vpiSize:
	    trả_về aobj ? (số_nguyên) aobj->get_size() : 0;
	  default:
	    trả_về 0;
      }
}

char* __vpiPropQueueRef::vpi_get_str(số_nguyên code)
{
      nếu (code == vpiFile) {
	    trả_về simple_set_rbuf_str(file_names[0]);
      }
      trả_về generic_get_str(code, scope_, "prop_darray", NULL);
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
      trả_về obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      nếu (!ref) trả_về 0;

      nếu (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    trả_về get_live_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      nếu (const __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    const vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    nếu (!fun) trả_về 0;
	    vvp_object_t obj = fun->get_object();
	    trả_về obj.peek<vvp_darray>();
      }

      trả_về 0;
}

#ifdef CHECK_WITH_VALGRIND
void array_delete(vpiHandle item)
{
      nếu (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(item)) {
	    delete pq;
	    trả_về;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(item);
      nếu (dobj) {
	    nếu (dobj->vals_words) delete [] (dobj->vals_words-1);
	    delete dobj;
	    trả_về;
      }

      fprintf(stderr, "Need support for deleting array type: %d\n", item->vpi_get(vpiArrayType));
      assert(0);
}
#endif
