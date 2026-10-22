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

using ನಾಮಾಕಾಶ std;

__vpiDarrayVar::__vpiDarrayVar(__vpiScope*sc, ಧ್ರುವ ವರ್ಣ*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

unsigned __vpiDarrayVar::get_size() ಧ್ರುವ
{
      ಧ್ರುವ vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      ಯದಿ(!fun)
        ಪ್ರತ್ಯಾಗಮ 0;

      vvp_object_t val = fun->get_object();
      ಧ್ರುವ vvp_darray*aval = val.peek<vvp_darray>();

      ಯದಿ(!aval)
        ಪ್ರತ್ಯಾಗಮ 0;

      ಪ್ರತ್ಯಾಗಮ aval->get_size();
}

vpiHandle __vpiDarrayVar::get_ಎಡ_range()
{
      ಎಡ_range.set_ಮೌಲ್ಯ(0);
      ಪ್ರತ್ಯಾಗಮ &ಎಡ_range;
}

vpiHandle __vpiDarrayVar::get_ಬಲ_range()
{
      ಬಲ_range.set_ಮೌಲ್ಯ(get_size() - 1);
      ಪ್ರತ್ಯಾಗಮ &ಬಲ_range;
}

ಪೂರ್ಣಾಙ್ಕ __vpiDarrayVar::get_word_size() ಧ್ರುವ
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_word(0, new_vec);
      ಪ್ರತ್ಯಾಗಮ new_vec.size();
}

ವರ್ಣ*__vpiDarrayVar::get_word_str(ಸಂರಚನಾ __vpiArrayWord*word, ಪೂರ್ಣಾಙ್ಕ code)
{
      unsigned index = word->get_index();

      ಯದಿ (code == vpiFile) {  // Not implemented for now!
	    ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(file_names[0]);
      }

      ವರ್ಣ sidx [64];
      snprintf(sidx, 63, "%d", (ಪೂರ್ಣಾಙ್ಕ)index);
      ಪ್ರತ್ಯಾಗಮ generic_get_str(code, scope_, name_, sidx);
}

ಶೂನ್ಯ __vpiDarrayVar::get_word_ಮೌಲ್ಯ(ಸಂರಚನಾ __vpiArrayWord*word, p_vpi_ಮೌಲ್ಯ vp)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      ಯದಿ(vp->format == vpiObjTypeVal) {
          ಯದಿ(dynamic_cast<vvp_darray_real*>(aobj))
              vp->format = vpiRealVal;
          ಅನ್ಯಥಾ ಯದಿ(dynamic_cast<vvp_darray_string*>(aobj))
              vp->format = vpiStringVal;
          ಅನ್ಯಥಾ
              vp->format = vpiVectorVal;
      }

      ವಿಕಲ್ಪ(vp->format) {
      ಪಕ್ಷ vpiBinStrVal:
      ಪಕ್ಷ vpiOctStrVal:
      ಪಕ್ಷ vpiDecStrVal:
      ಪಕ್ಷ vpiHexStrVal:
      ಪಕ್ಷ vpiScalarVal:
      ಪಕ್ಷ vpiIntVal:
      ಪಕ್ಷ vpiVectorVal:
      {
          vvp_vector4_t v;
          aobj->get_word(index, v);
          vpip_vec4_get_ಮೌಲ್ಯ(v, v.size(), ಅಸತ್ಯ, vp);  // TODO sign?
      }
      ಭಙ್ಗ;

      ಪಕ್ಷ vpiRealVal:
      {
          ದ್ವಿಗುಣ d;
          aobj->get_word(index, d);
          vpip_real_get_ಮೌಲ್ಯ(d, vp);
      }
      ಭಙ್ಗ;

      ಪಕ್ಷ vpiStringVal:
      {
          string s;
          aobj->get_word(index, s);
          vpip_string_get_ಮೌಲ್ಯ(s, vp);
      }
      ಭಙ್ಗ;

      ಸ್ವಭಾವ:
          fprintf(stderr, "vpi sorry: format is not implemented\n");
          ಪ್ರತಿಜ್ಞಾ(ಅಸತ್ಯ);
      }
}

ಶೂನ್ಯ __vpiDarrayVar::put_word_ಮೌಲ್ಯ(ಸಂರಚನಾ __vpiArrayWord*word, p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಙ್ಕ)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      ವಿಕಲ್ಪ(vp->format) {
      ಪಕ್ಷ vpiScalarVal:
      ಪಕ್ಷ vpiIntVal:
      ಪಕ್ಷ vpiVectorVal: {
	    vvp_vector4_t vec = vec4_from_vpi_ಮೌಲ್ಯ(vp, get_word_size());
	    aobj->set_word(index, vec);
      }
      ಭಙ್ಗ;

      ಪಕ್ಷ vpiRealVal:
        aobj->set_word(index, vp->value.real);
        ಭಙ್ಗ;

      ಪಕ್ಷ vpiStringVal:
        aobj->set_word(index, std::string(vp->value.str));
        ಭಙ್ಗ;

      ಸ್ವಭಾವ:
          fprintf(stderr, "vpi sorry: format is not implemented");
          ಪ್ರತಿಜ್ಞಾ(ಅಸತ್ಯ);
      }
}

vpiHandle __vpiDarrayVar::get_iter_index(ಸಂರಚನಾ __vpiArrayIterator*, ಪೂರ್ಣಾಙ್ಕ idx)
{
      ಯದಿ (vals_ಪದಗಳು == 0) make_vals_ಪದಗಳು();

      ಪ್ರತ್ಯಾಗಮ &(vals_ಪದಗಳು[idx].as_word);
}

ಪೂರ್ಣಾಙ್ಕ __vpiDarrayVar::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiArrayType:
	    ಪ್ರತ್ಯಾಗಮ vpiDynamicArray;
	  ಪಕ್ಷ vpi_ಎಡ_Range:
	    ಪ್ರತ್ಯಾಗಮ 0;
	  ಪಕ್ಷ vpi_ಬಲ_Range:
	    ಪ್ರತ್ಯಾಗಮ get_size() - 1;
	  ಪಕ್ಷ vpiSize:
            ಪ್ರತ್ಯಾಗಮ get_size();

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    ಪ್ರತಿಜ್ಞಾ(ಅಸತ್ಯ);
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}

ವರ್ಣ* __vpiDarrayVar::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{
      ಯದಿ (code == vpiFile) {  // Not implemented for now!
            ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(file_names[0]);
      }

      ಪ್ರತ್ಯಾಗಮ generic_get_str(code, scope_, name_, ರಿಕ್ತ);
}

vpiHandle __vpiDarrayVar::vpi_handle(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
          ಪಕ್ಷ vpi_ಎಡ_Range:
            ಪ್ರತ್ಯಾಗಮ get_ಎಡ_range();

          ಪಕ್ಷ vpi_ಬಲ_Range:
            ಪ್ರತ್ಯಾಗಮ get_ಬಲ_range();

          ಪಕ್ಷ vpiScope:
            ಪ್ರತ್ಯಾಗಮ scope_;

          ಪಕ್ಷ vpiModule:
            ಪ್ರತ್ಯಾಗಮ vpip_module(scope_);
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

vpiHandle __vpiDarrayVar::vpi_index(ಪೂರ್ಣಾಙ್ಕ index)
{
      ಯದಿ (index >= (long) get_size())
	    ಪ್ರತ್ಯಾಗಮ 0;
      ಯದಿ (index < 0)
	    ಪ್ರತ್ಯಾಗಮ 0;

      ಯದಿ (vals_ಪದಗಳು == 0)
	    make_vals_ಪದಗಳು();

      ಪ್ರತ್ಯಾಗಮ &(vals_ಪದಗಳು[index].as_word);
}

ಶೂನ್ಯ __vpiDarrayVar::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{
      val->format = vpiSuppressVal;
}

vvp_darray*__vpiDarrayVar::get_vvp_darray() ಧ್ರುವ
{
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      ಪ್ರತಿಜ್ಞಾ(fun);
      vvp_object_t obj = fun->get_object();

      ಪ್ರತ್ಯಾಗಮ obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(ಧ್ರುವ ವರ್ಣ*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ಧ್ರುವ ವರ್ಣ*use_name = name ? vpip_name_string(name) : ರಿಕ್ತ;

      __vpiDarrayVar*obj = ನೂತನ __vpiDarrayVar(scope, use_name, net);

      ಪ್ರತ್ಯಾಗಮ obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, ಧ್ರುವ ವರ್ಣ*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

ಪೂರ್ಣಾಙ್ಕ __vpiQueueVar::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiArrayType:
	    ಪ್ರತ್ಯಾಗಮ vpiQueueArray;
	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_queue_var(ಧ್ರುವ ವರ್ಣ*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ಧ್ರುವ ವರ್ಣ*use_name = name ? vpip_name_string(name) : ರಿಕ್ತ;

      __vpiQueueVar*obj = ನೂತನ __vpiQueueVar(scope, use_name, net);

      ಪ್ರತ್ಯಾಗಮ obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, unsigned pidx,
				     ತಾರ್ಕಿಕ is_queue)
: class_net_(0), prop_idx_(pidx), is_queue_(is_queue), scope_(scope)
{
}

ಪೂರ್ಣಾಙ್ಕ __vpiPropQueueRef::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiArrayVar; }

ಸ್ಥಿರ vvp_darray* get_live_darray_from_prop(vvp_net_t* class_net, unsigned pid)
{
      ಯದಿ (!class_net) ಪ್ರತ್ಯಾಗಮ 0;
      ಧ್ರುವ vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      ಯದಿ (!fun) ಪ್ರತ್ಯಾಗಮ 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      ಯದಿ (!cobj) ಪ್ರತ್ಯಾಗಮ 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      ಪ್ರತ್ಯಾಗಮ qobj.peek<vvp_darray>();
}

ಪೂರ್ಣಾಙ್ಕ __vpiPropQueueRef::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ಧ್ರುವ vvp_darray* aobj = get_live_darray_from_prop(class_net_, prop_idx_);
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiArrayType:
	    ಪ್ರತ್ಯಾಗಮ is_queue_ ? vpiQueueArray : vpiDynamicArray;
	  ಪಕ್ಷ vpi_ಎಡ_Range:
	    ಪ್ರತ್ಯಾಗಮ 0;
	  ಪಕ್ಷ vpi_ಬಲ_Range:
	    ಪ್ರತ್ಯಾಗಮ aobj ? (ಪೂರ್ಣಾಙ್ಕ) (aobj->get_size() - 1) : 0;
	  ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ aobj ? (ಪೂರ್ಣಾಙ್ಕ) aobj->get_size() : 0;
	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}

ವರ್ಣ* __vpiPropQueueRef::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{
      ಯದಿ (code == vpiFile) {
	    ಪ್ರತ್ಯಾಗಮ simple_set_rbuf_str(file_names[0]);
      }
      ಪ್ರತ್ಯಾಗಮ generic_get_str(code, scope_, "prop_darray", ರಿಕ್ತ);
}

ಶೂನ್ಯ __vpiPropQueueRef::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{
      val->format = vpiSuppressVal;
}

vpiHandle vpip_make_prop_queue_ref(ವರ್ಣ* class_label, unsigned prop_idx,
				   unsigned is_queue_flag)
{
      __vpiPropQueueRef* obj = ನೂತನ __vpiPropQueueRef(vpip_peek_current_scope(),
						     prop_idx,
						     is_queue_flag != 0);
      functor_ref_lookup(&obj->class_net_, class_label);
      ಪ್ರತ್ಯಾಗಮ obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      ಯದಿ (!ref) ಪ್ರತ್ಯಾಗಮ 0;

      ಯದಿ (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    ಪ್ರತ್ಯಾಗಮ get_live_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      ಯದಿ (ಧ್ರುವ __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    ಧ್ರುವ vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    ಯದಿ (!fun) ಪ್ರತ್ಯಾಗಮ 0;
	    vvp_object_t obj = fun->get_object();
	    ಪ್ರತ್ಯಾಗಮ obj.peek<vvp_darray>();
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ array_delete(vpiHandle item)
{
      ಯದಿ (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(item)) {
	    delete pq;
	    ಪ್ರತ್ಯಾಗಮ;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(item);
      ಯದಿ (dobj) {
	    ಯದಿ (dobj->vals_ಪದಗಳು) delete [] (dobj->vals_ಪದಗಳು-1);
	    delete dobj;
	    ಪ್ರತ್ಯಾಗಮ;
      }

      fprintf(stderr, "Need support for deleting array type: %d\n", item->vpi_get(vpiArrayType));
      ಪ್ರತಿಜ್ಞಾ(0);
}
#endif
