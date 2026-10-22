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

using નામાકાશ std;

__vpiDarrayVar::__vpiDarrayVar(__vpiScope*sc, અચલ અક્ષર*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

unsigned __vpiDarrayVar::get_size() અચલ
{
      અચલ vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      જો(!fun)
        પાછા 0;

      vvp_object_t val = fun->get_object();
      અચલ vvp_darray*aval = val.peek<vvp_darray>();

      જો(!aval)
        પાછા 0;

      પાછા aval->get_size();
}

vpiHandle __vpiDarrayVar::get_ડાબું_range()
{
      ડાબું_range.set_કિંમત(0);
      પાછા &ડાબું_range;
}

vpiHandle __vpiDarrayVar::get_જમણું_range()
{
      જમણું_range.set_કિંમત(get_size() - 1);
      પાછા &જમણું_range;
}

પૂર્ણાંક __vpiDarrayVar::get_word_size() અચલ
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_word(0, new_vec);
      પાછા new_vec.size();
}

અક્ષર*__vpiDarrayVar::get_word_str(સંરચના __vpiArrayWord*word, પૂર્ણાંક code)
{
      unsigned index = word->get_index();

      જો (code == vpiFile) {  // Not implemented for now!
	    પાછા simple_set_rbuf_str(file_names[0]);
      }

      અક્ષર sidx [64];
      snprintf(sidx, 63, "%d", (પૂર્ણાંક)index);
      પાછા generic_get_str(code, scope_, name_, sidx);
}

શૂન્ય __vpiDarrayVar::get_word_કિંમત(સંરચના __vpiArrayWord*word, p_vpi_કિંમત vp)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      જો(vp->format == vpiObjTypeVal) {
          જો(dynamic_cast<vvp_darray_real*>(aobj))
              vp->format = vpiRealVal;
          નહીંતર જો(dynamic_cast<vvp_darray_string*>(aobj))
              vp->format = vpiStringVal;
          નહીંતર
              vp->format = vpiVectorVal;
      }

      પસંદગી(vp->format) {
      સ્થિતિ vpiBinStrVal:
      સ્થિતિ vpiOctStrVal:
      સ્થિતિ vpiDecStrVal:
      સ્થિતિ vpiHexStrVal:
      સ્થિતિ vpiScalarVal:
      સ્થિતિ vpiIntVal:
      સ્થિતિ vpiVectorVal:
      {
          vvp_vector4_t v;
          aobj->get_word(index, v);
          vpip_vec4_get_કિંમત(v, v.size(), ખોટું, vp);  // TODO sign?
      }
      તોડો;

      સ્થિતિ vpiRealVal:
      {
          દ્વિગુણ d;
          aobj->get_word(index, d);
          vpip_real_get_કિંમત(d, vp);
      }
      તોડો;

      સ્થિતિ vpiStringVal:
      {
          string s;
          aobj->get_word(index, s);
          vpip_string_get_કિંમત(s, vp);
      }
      તોડો;

      મૂળભૂત:
          fprintf(stderr, "vpi sorry: format is not implemented\n");
          પ્રતિજ્ઞા(ખોટું);
      }
}

શૂન્ય __vpiDarrayVar::put_word_કિંમત(સંરચના __vpiArrayWord*word, p_vpi_કિંમત vp, પૂર્ણાંક)
{
      unsigned index = word->get_index();
      vvp_darray*aobj = get_vvp_darray();

      પસંદગી(vp->format) {
      સ્થિતિ vpiScalarVal:
      સ્થિતિ vpiIntVal:
      સ્થિતિ vpiVectorVal: {
	    vvp_vector4_t vec = vec4_from_vpi_કિંમત(vp, get_word_size());
	    aobj->set_word(index, vec);
      }
      તોડો;

      સ્થિતિ vpiRealVal:
        aobj->set_word(index, vp->value.real);
        તોડો;

      સ્થિતિ vpiStringVal:
        aobj->set_word(index, std::string(vp->value.str));
        તોડો;

      મૂળભૂત:
          fprintf(stderr, "vpi sorry: format is not implemented");
          પ્રતિજ્ઞા(ખોટું);
      }
}

vpiHandle __vpiDarrayVar::get_iter_index(સંરચના __vpiArrayIterator*, પૂર્ણાંક idx)
{
      જો (vals_શબ્દો == 0) make_vals_શબ્દો();

      પાછા &(vals_શબ્દો[idx].as_word);
}

પૂર્ણાંક __vpiDarrayVar::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiArrayType:
	    પાછા vpiDynamicArray;
	  સ્થિતિ vpi_ડાબું_Range:
	    પાછા 0;
	  સ્થિતિ vpi_જમણું_Range:
	    પાછા get_size() - 1;
	  સ્થિતિ vpiSize:
            પાછા get_size();

	  મૂળભૂત:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    પ્રતિજ્ઞા(ખોટું);
	    પાછા 0;
      }
}

અક્ષર* __vpiDarrayVar::vpi_get_str(પૂર્ણાંક code)
{
      જો (code == vpiFile) {  // Not implemented for now!
            પાછા simple_set_rbuf_str(file_names[0]);
      }

      પાછા generic_get_str(code, scope_, name_, ખાલી);
}

vpiHandle __vpiDarrayVar::vpi_handle(પૂર્ણાંક code)
{
      પસંદગી (code) {
          સ્થિતિ vpi_ડાબું_Range:
            પાછા get_ડાબું_range();

          સ્થિતિ vpi_જમણું_Range:
            પાછા get_જમણું_range();

          સ્થિતિ vpiScope:
            પાછા scope_;

          સ્થિતિ vpiModule:
            પાછા vpip_module(scope_);
      }

      પાછા 0;
}

vpiHandle __vpiDarrayVar::vpi_index(પૂર્ણાંક index)
{
      જો (index >= (long) get_size())
	    પાછા 0;
      જો (index < 0)
	    પાછા 0;

      જો (vals_શબ્દો == 0)
	    make_vals_શબ્દો();

      પાછા &(vals_શબ્દો[index].as_word);
}

શૂન્ય __vpiDarrayVar::vpi_get_કિંમત(p_vpi_કિંમત val)
{
      val->format = vpiSuppressVal;
}

vvp_darray*__vpiDarrayVar::get_vvp_darray() અચલ
{
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      પ્રતિજ્ઞા(fun);
      vvp_object_t obj = fun->get_object();

      પાછા obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(અચલ અક્ષર*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      અચલ અક્ષર*use_name = name ? vpip_name_string(name) : ખાલી;

      __vpiDarrayVar*obj = નવું __vpiDarrayVar(scope, use_name, net);

      પાછા obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, અચલ અક્ષર*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

પૂર્ણાંક __vpiQueueVar::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiArrayType:
	    પાછા vpiQueueArray;
	  મૂળભૂત:
	    પાછા __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_queue_var(અચલ અક્ષર*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      અચલ અક્ષર*use_name = name ? vpip_name_string(name) : ખાલી;

      __vpiQueueVar*obj = નવું __vpiQueueVar(scope, use_name, net);

      પાછા obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, unsigned pidx,
				     તાર્કિક is_queue)
: class_net_(0), prop_idx_(pidx), is_queue_(is_queue), scope_(scope)
{
}

પૂર્ણાંક __vpiPropQueueRef::get_type_code(શૂન્ય) અચલ
{ પાછા vpiArrayVar; }

સ્થિર vvp_darray* get_live_darray_from_prop(vvp_net_t* class_net, unsigned pid)
{
      જો (!class_net) પાછા 0;
      અચલ vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      જો (!fun) પાછા 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      જો (!cobj) પાછા 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      પાછા qobj.peek<vvp_darray>();
}

પૂર્ણાંક __vpiPropQueueRef::vpi_get(પૂર્ણાંક code)
{
      અચલ vvp_darray* aobj = get_live_darray_from_prop(class_net_, prop_idx_);
      પસંદગી (code) {
	  સ્થિતિ vpiArrayType:
	    પાછા is_queue_ ? vpiQueueArray : vpiDynamicArray;
	  સ્થિતિ vpi_ડાબું_Range:
	    પાછા 0;
	  સ્થિતિ vpi_જમણું_Range:
	    પાછા aobj ? (પૂર્ણાંક) (aobj->get_size() - 1) : 0;
	  સ્થિતિ vpiSize:
	    પાછા aobj ? (પૂર્ણાંક) aobj->get_size() : 0;
	  મૂળભૂત:
	    પાછા 0;
      }
}

અક્ષર* __vpiPropQueueRef::vpi_get_str(પૂર્ણાંક code)
{
      જો (code == vpiFile) {
	    પાછા simple_set_rbuf_str(file_names[0]);
      }
      પાછા generic_get_str(code, scope_, "prop_darray", ખાલી);
}

શૂન્ય __vpiPropQueueRef::vpi_get_કિંમત(p_vpi_કિંમત val)
{
      val->format = vpiSuppressVal;
}

vpiHandle vpip_make_prop_queue_ref(અક્ષર* class_label, unsigned prop_idx,
				   unsigned is_queue_flag)
{
      __vpiPropQueueRef* obj = નવું __vpiPropQueueRef(vpip_peek_current_scope(),
						     prop_idx,
						     is_queue_flag != 0);
      functor_ref_lookup(&obj->class_net_, class_label);
      પાછા obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      જો (!ref) પાછા 0;

      જો (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    પાછા get_live_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      જો (અચલ __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    અચલ vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    જો (!fun) પાછા 0;
	    vvp_object_t obj = fun->get_object();
	    પાછા obj.peek<vvp_darray>();
      }

      પાછા 0;
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય array_delete(vpiHandle item)
{
      જો (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(item)) {
	    delete pq;
	    પાછા;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(item);
      જો (dobj) {
	    જો (dobj->vals_શબ્દો) delete [] (dobj->vals_શબ્દો-1);
	    delete dobj;
	    પાછા;
      }

      fprintf(stderr, "Need support for deleting array type: %d\n", item->vpi_get(vpiArrayType));
      પ્રતિજ્ઞા(0);
}
#endif
