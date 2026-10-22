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
      если(!fun)
        вернуть 0;

      vvp_object_t val = fun->get_object();
      const vvp_darray*aval = val.peek<vvp_darray>();

      если(!aval)
        вернуть 0;

      вернуть aval->get_size();
}

vpiHandle __vpiDarrayVar::get_left_range()
{
      left_range_.set_value(0);
      вернуть &left_range_;
}

vpiHandle __vpiDarrayVar::get_right_range()
{
      right_range_.set_value(get_size() - 1);
      вернуть &right_range_;
}

целое __vpiDarrayVar::get_word_size() const
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_word(0, new_vec);
      вернуть new_vec.size();
}

char*__vpiDarrayVar::get_word_str(struct __vpiArrayWord*word, целое code)
{
      unsigned index = word->get_index();

      если (code == vpiFile) {  // Not implemented for now!
	    вернуть simple_set_rbuf_str(file_names[0]);
      }

      char sidx [64];
      snprintf(sidx, 63, "%d", (целое)index);
      вернуть generic_get_str(code, scope_, name_, sidx);
}

void __vpiDarrayVar::get_word_value(struct __vpiArrayWord*word, p_vpi_value vp)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      если(vp->format == vpiObjTypeVal) {
          если(dynamic_cast<vvp_darray_real*>(aobj))
              vp->format = vpiRealVal;
          иначе если(dynamic_cast<vvp_darray_string*>(aobj))
              vp->format = vpiStringVal;
          иначе
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

void __vpiDarrayVar::put_word_value(struct __vpiArrayWord*word, p_vpi_value vp, целое)
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

vpiHandle __vpiDarrayVar::get_iter_index(struct __vpiArrayIterator*, целое idx)
{
      если (vals_words == 0) make_vals_words();

      вернуть &(vals_words[idx].as_word);
}

целое __vpiDarrayVar::vpi_get(целое code)
{
      switch (code) {
	  case vpiArrayType:
	    вернуть vpiDynamicArray;
	  case vpiLeftRange:
	    вернуть 0;
	  case vpiRightRange:
	    вернуть get_size() - 1;
	  case vpiSize:
            вернуть get_size();

	  default:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    assert(false);
	    вернуть 0;
      }
}

char* __vpiDarrayVar::vpi_get_str(целое code)
{
      если (code == vpiFile) {  // Not implemented for now!
            вернуть simple_set_rbuf_str(file_names[0]);
      }

      вернуть generic_get_str(code, scope_, name_, NULL);
}

vpiHandle __vpiDarrayVar::vpi_handle(целое code)
{
      switch (code) {
          case vpiLeftRange:
            вернуть get_left_range();

          case vpiRightRange:
            вернуть get_right_range();

          case vpiScope:
            вернуть scope_;

          case vpiModule:
            вернуть vpip_module(scope_);
      }

      вернуть 0;
}

vpiHandle __vpiDarrayVar::vpi_index(целое index)
{
      если (index >= (long) get_size())
	    вернуть 0;
      если (index < 0)
	    вернуть 0;

      если (vals_words == 0)
	    make_vals_words();

      вернуть &(vals_words[index].as_word);
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

      вернуть obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(const char*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      const char*use_name = name ? vpip_name_string(name) : NULL;

      __vpiDarrayVar*obj = new __vpiDarrayVar(scope, use_name, net);

      вернуть obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, const char*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

целое __vpiQueueVar::vpi_get(целое code)
{
      switch (code) {
	  case vpiArrayType:
	    вернуть vpiQueueArray;
	  default:
	    вернуть __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_queue_var(const char*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      const char*use_name = name ? vpip_name_string(name) : NULL;

      __vpiQueueVar*obj = new __vpiQueueVar(scope, use_name, net);

      вернуть obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, unsigned pidx,
				     bool is_queue)
: class_net_(0), prop_idx_(pidx), is_queue_(is_queue), scope_(scope)
{
}

целое __vpiPropQueueRef::get_type_code(void) const
{ вернуть vpiArrayVar; }

static vvp_darray* get_live_darray_from_prop(vvp_net_t* class_net, unsigned pid)
{
      если (!class_net) вернуть 0;
      const vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      если (!fun) вернуть 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      если (!cobj) вернуть 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      вернуть qobj.peek<vvp_darray>();
}

целое __vpiPropQueueRef::vpi_get(целое code)
{
      const vvp_darray* aobj = get_live_darray_from_prop(class_net_, prop_idx_);
      switch (code) {
	  case vpiArrayType:
	    вернуть is_queue_ ? vpiQueueArray : vpiDynamicArray;
	  case vpiLeftRange:
	    вернуть 0;
	  case vpiRightRange:
	    вернуть aobj ? (целое) (aobj->get_size() - 1) : 0;
	  case vpiSize:
	    вернуть aobj ? (целое) aobj->get_size() : 0;
	  default:
	    вернуть 0;
      }
}

char* __vpiPropQueueRef::vpi_get_str(целое code)
{
      если (code == vpiFile) {
	    вернуть simple_set_rbuf_str(file_names[0]);
      }
      вернуть generic_get_str(code, scope_, "prop_darray", NULL);
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
      вернуть obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      если (!ref) вернуть 0;

      если (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    вернуть get_live_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      если (const __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    const vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    если (!fun) вернуть 0;
	    vvp_object_t obj = fun->get_object();
	    вернуть obj.peek<vvp_darray>();
      }

      вернуть 0;
}

#ifdef CHECK_WITH_VALGRIND
void array_delete(vpiHandle item)
{
      если (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(item)) {
	    delete pq;
	    вернуть;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(item);
      если (dobj) {
	    если (dobj->vals_words) delete [] (dobj->vals_words-1);
	    delete dobj;
	    вернуть;
      }

      fprintf(stderr, "Need support for deleting array type: %d\n", item->vpi_get(vpiArrayType));
      assert(0);
}
#endif
