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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ਜੇ_ਅੰਤ
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

__vpiDarrayVar::__vpiDarrayVar(__vpiScope*sc, ਅਚਲ ਅੱਖਰ*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

ਬਿਨਾਨਿਸ਼ਾਨੀ __vpiDarrayVar::get_ਮਾਪ() ਅਚਲ
{
      ਅਚਲ vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      ਜੇ(!fun)
        ਵਾਪਸ 0;

      vvp_object_t val = fun->get_object();
      ਅਚਲ vvp_darray*aval = val.peek<vvp_darray>();

      ਜੇ(!aval)
        ਵਾਪਸ 0;

      ਵਾਪਸ aval->get_ਮਾਪ();
}

vpiHandle __vpiDarrayVar::get_ਖੱਬਾ_range()
{
      ਖੱਬਾ_range.ਸੈੱਟ_ਮੁੱਲ(0);
      ਵਾਪਸ &ਖੱਬਾ_range;
}

vpiHandle __vpiDarrayVar::get_ਸੱਜਾ_range()
{
      ਸੱਜਾ_range.ਸੈੱਟ_ਮੁੱਲ(get_ਮਾਪ() - 1);
      ਵਾਪਸ &ਸੱਜਾ_range;
}

ਪੂਰਨ __vpiDarrayVar::get_ਸ਼ਬਦ_ਮਾਪ() ਅਚਲ
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_ਸ਼ਬਦ(0, new_vec);
      ਵਾਪਸ new_vec.size();
}

ਅੱਖਰ*__vpiDarrayVar::get_ਸ਼ਬਦ_str(ਬਣਤਰ __vpiArrayWord*ਸ਼ਬਦ_2, ਪੂਰਨ code)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ = ਸ਼ਬਦ_2->get_ਸੂਚਕਾਂਕ();

      ਜੇ (code == vpiFile) {  // Not implemented for now!
	    ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(file_ਨਾਂ[0]);
      }

      ਅੱਖਰ sidx [64];
      snprintf(sidx, 63, "%d", (ਪੂਰਨ)ਸੂਚਕਾਂਕ);
      ਵਾਪਸ generic_get_str(code, scope_, name_, sidx);
}

ਖਾਲੀ __vpiDarrayVar::get_ਸ਼ਬਦ_ਮੁੱਲ(ਬਣਤਰ __vpiArrayWord*ਸ਼ਬਦ_2, p_vpi_ਮੁੱਲ vp)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ = ਸ਼ਬਦ_2->get_ਸੂਚਕਾਂਕ();
      vvp_darray*aobj = get_vvp_darray();

      ਜੇ(vp->format == vpiObjTypeVal) {
          ਜੇ(dynamic_cast<vvp_darray_real*>(aobj))
              vp->format = vpiRealVal;
          ਨਹੀਂਤਾਂ ਜੇ(dynamic_cast<vvp_darray_string*>(aobj))
              vp->format = vpiStringVal;
          ਨਹੀਂਤਾਂ
              vp->format = vpi_ਸਦਿਸ਼_Val;
      }

      ਚੋਣ(vp->format) {
      ਹਾਲਤ vpiBinStrVal:
      ਹਾਲਤ vpiOctStrVal:
      ਹਾਲਤ vpiDecStrVal:
      ਹਾਲਤ vpiHexStrVal:
      ਹਾਲਤ vpiScalarVal:
      ਹਾਲਤ vpiIntVal:
      ਹਾਲਤ vpi_ਸਦਿਸ਼_Val:
      {
          vvp_vector4_t v;
          aobj->get_ਸ਼ਬਦ(ਸੂਚਕਾਂਕ, v);
          vpip_vec4_get_ਮੁੱਲ(v, v.size(), ਝੂਠ, vp);  // TODO sign?
      }
      ਰੋਕੋ;

      ਹਾਲਤ vpiRealVal:
      {
          ਦੁੱਗਣਾ d;
          aobj->get_ਸ਼ਬਦ(ਸੂਚਕਾਂਕ, d);
          vpip_real_get_ਮੁੱਲ(d, vp);
      }
      ਰੋਕੋ;

      ਹਾਲਤ vpiStringVal:
      {
          string s;
          aobj->get_ਸ਼ਬਦ(ਸੂਚਕਾਂਕ, s);
          vpip_string_get_ਮੁੱਲ(s, vp);
      }
      ਰੋਕੋ;

      ਮੂਲ:
          fprintf(stderr, "vpi sorry: format is not implemented\n");
          ਪ੍ਰਤਿਜ੍ਞਾ(ਝੂਠ);
      }
}

ਖਾਲੀ __vpiDarrayVar::put_ਸ਼ਬਦ_ਮੁੱਲ(ਬਣਤਰ __vpiArrayWord*ਸ਼ਬਦ_2, p_vpi_ਮੁੱਲ vp, ਪੂਰਨ)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ = ਸ਼ਬਦ_2->get_ਸੂਚਕਾਂਕ();
      vvp_darray*aobj = get_vvp_darray();

      ਚੋਣ(vp->format) {
      ਹਾਲਤ vpiScalarVal:
      ਹਾਲਤ vpiIntVal:
      ਹਾਲਤ vpi_ਸਦਿਸ਼_Val: {
	    vvp_vector4_t vec = vec4_from_vpi_ਮੁੱਲ(vp, get_ਸ਼ਬਦ_ਮਾਪ());
	    aobj->ਸੈੱਟ_ਸ਼ਬਦ(ਸੂਚਕਾਂਕ, vec);
      }
      ਰੋਕੋ;

      ਹਾਲਤ vpiRealVal:
        aobj->ਸੈੱਟ_ਸ਼ਬਦ(ਸੂਚਕਾਂਕ, vp->value.real);
        ਰੋਕੋ;

      ਹਾਲਤ vpiStringVal:
        aobj->ਸੈੱਟ_ਸ਼ਬਦ(ਸੂਚਕਾਂਕ, std::string(vp->value.str));
        ਰੋਕੋ;

      ਮੂਲ:
          fprintf(stderr, "vpi sorry: format is not implemented");
          ਪ੍ਰਤਿਜ੍ਞਾ(ਝੂਠ);
      }
}

vpiHandle __vpiDarrayVar::get_iter_ਸੂਚਕਾਂਕ(ਬਣਤਰ __vpiArrayIterator*, ਪੂਰਨ idx)
{
      ਜੇ (vals_ਸ਼ਬਦ == 0) make_vals_ਸ਼ਬਦ();

      ਵਾਪਸ &(vals_ਸ਼ਬਦ[idx].as_ਸ਼ਬਦ);
}

ਪੂਰਨ __vpiDarrayVar::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpiArrayType:
	    ਵਾਪਸ vpiDynamicArray;
	  ਹਾਲਤ vpi_ਖੱਬਾ_Range:
	    ਵਾਪਸ 0;
	  ਹਾਲਤ vpi_ਸੱਜਾ_Range:
	    ਵਾਪਸ get_ਮਾਪ() - 1;
	  ਹਾਲਤ vpi_ਮਾਪ:
            ਵਾਪਸ get_ਮਾਪ();

	  ਮੂਲ:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਝੂਠ);
	    ਵਾਪਸ 0;
      }
}

ਅੱਖਰ* __vpiDarrayVar::vpi_get_str(ਪੂਰਨ code)
{
      ਜੇ (code == vpiFile) {  // Not implemented for now!
            ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(file_ਨਾਂ[0]);
      }

      ਵਾਪਸ generic_get_str(code, scope_, name_, ਨਿਰਾ);
}

vpiHandle __vpiDarrayVar::vpi_handle(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
          ਹਾਲਤ vpi_ਖੱਬਾ_Range:
            ਵਾਪਸ get_ਖੱਬਾ_range();

          ਹਾਲਤ vpi_ਸੱਜਾ_Range:
            ਵਾਪਸ get_ਸੱਜਾ_range();

          ਹਾਲਤ vpiScope:
            ਵਾਪਸ scope_;

          ਹਾਲਤ vpiModule:
            ਵਾਪਸ vpip_module(scope_);
      }

      ਵਾਪਸ 0;
}

vpiHandle __vpiDarrayVar::vpi_ਸੂਚਕਾਂਕ(ਪੂਰਨ ਸੂਚਕਾਂਕ)
{
      ਜੇ (ਸੂਚਕਾਂਕ >= (ਲੰਮਾ) get_ਮਾਪ())
	    ਵਾਪਸ 0;
      ਜੇ (ਸੂਚਕਾਂਕ < 0)
	    ਵਾਪਸ 0;

      ਜੇ (vals_ਸ਼ਬਦ == 0)
	    make_vals_ਸ਼ਬਦ();

      ਵਾਪਸ &(vals_ਸ਼ਬਦ[ਸੂਚਕਾਂਕ].as_ਸ਼ਬਦ);
}

ਖਾਲੀ __vpiDarrayVar::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val)
{
      val->format = vpiSuppressVal;
}

vvp_darray*__vpiDarrayVar::get_vvp_darray() ਅਚਲ
{
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      ਪ੍ਰਤਿਜ੍ਞਾ(fun);
      vvp_object_t obj = fun->get_object();

      ਵਾਪਸ obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(ਅਚਲ ਅੱਖਰ*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ਅਚਲ ਅੱਖਰ*use_name = name ? vpip_name_string(name) : ਨਿਰਾ;

      __vpiDarrayVar*obj = ਨਵਾਂ __vpiDarrayVar(scope, use_name, net);

      ਵਾਪਸ obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, ਅਚਲ ਅੱਖਰ*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

ਪੂਰਨ __vpiQueueVar::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpiArrayType:
	    ਵਾਪਸ vpi_ਕਤਾਰ_Array;
	  ਮੂਲ:
	    ਵਾਪਸ __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_ਕਤਾਰ_var(ਅਚਲ ਅੱਖਰ*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ਅਚਲ ਅੱਖਰ*use_name = name ? vpip_name_string(name) : ਨਿਰਾ;

      __vpiQueueVar*obj = ਨਵਾਂ __vpiQueueVar(scope, use_name, net);

      ਵਾਪਸ obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, ਬਿਨਾਨਿਸ਼ਾਨੀ pidx,
				     ਤਾਰਕਿਕ is_ਕਤਾਰ)
: class_net_(0), prop_idx_(pidx), is_ਕਤਾਰ_2(is_ਕਤਾਰ), scope_(scope)
{
}

ਪੂਰਨ __vpiPropQueueRef::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiArrayVar; }

ਸਥਿਰ vvp_darray* get_ਜੀਵਤ_darray_from_prop(vvp_net_t* class_net, ਬਿਨਾਨਿਸ਼ਾਨੀ pid)
{
      ਜੇ (!class_net) ਵਾਪਸ 0;
      ਅਚਲ vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      ਜੇ (!fun) ਵਾਪਸ 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      ਜੇ (!cobj) ਵਾਪਸ 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      ਵਾਪਸ qobj.peek<vvp_darray>();
}

ਪੂਰਨ __vpiPropQueueRef::vpi_get(ਪੂਰਨ code)
{
      ਅਚਲ vvp_darray* aobj = get_ਜੀਵਤ_darray_from_prop(class_net_, prop_idx_);
      ਚੋਣ (code) {
	  ਹਾਲਤ vpiArrayType:
	    ਵਾਪਸ is_ਕਤਾਰ_2 ? vpi_ਕਤਾਰ_Array : vpiDynamicArray;
	  ਹਾਲਤ vpi_ਖੱਬਾ_Range:
	    ਵਾਪਸ 0;
	  ਹਾਲਤ vpi_ਸੱਜਾ_Range:
	    ਵਾਪਸ aobj ? (ਪੂਰਨ) (aobj->get_ਮਾਪ() - 1) : 0;
	  ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ aobj ? (ਪੂਰਨ) aobj->get_ਮਾਪ() : 0;
	  ਮੂਲ:
	    ਵਾਪਸ 0;
      }
}

ਅੱਖਰ* __vpiPropQueueRef::vpi_get_str(ਪੂਰਨ code)
{
      ਜੇ (code == vpiFile) {
	    ਵਾਪਸ simple_ਸੈੱਟ_rbuf_str(file_ਨਾਂ[0]);
      }
      ਵਾਪਸ generic_get_str(code, scope_, "prop_darray", ਨਿਰਾ);
}

ਖਾਲੀ __vpiPropQueueRef::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val)
{
      val->format = vpiSuppressVal;
}

vpiHandle vpip_make_prop_ਕਤਾਰ_ref(ਅੱਖਰ* class_label, ਬਿਨਾਨਿਸ਼ਾਨੀ prop_idx,
				   ਬਿਨਾਨਿਸ਼ਾਨੀ is_ਕਤਾਰ_flag)
{
      __vpiPropQueueRef* obj = ਨਵਾਂ __vpiPropQueueRef(vpip_peek_current_scope(),
						     prop_idx,
						     is_ਕਤਾਰ_flag != 0);
      functor_ref_lookup(&obj->class_net_, class_label);
      ਵਾਪਸ obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      ਜੇ (!ref) ਵਾਪਸ 0;

      ਜੇ (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    ਵਾਪਸ get_ਜੀਵਤ_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      ਜੇ (ਅਚਲ __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    ਅਚਲ vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    ਜੇ (!fun) ਵਾਪਸ 0;
	    vvp_object_t obj = fun->get_object();
	    ਵਾਪਸ obj.peek<vvp_darray>();
      }

      ਵਾਪਸ 0;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ array_delete(vpiHandle ਵਸਤੂ)
{
      ਜੇ (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(ਵਸਤੂ)) {
	    ਮਿਟਾਓ pq;
	    ਵਾਪਸ;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(ਵਸਤੂ);
      ਜੇ (dobj) {
	    ਜੇ (dobj->vals_ਸ਼ਬਦ) ਮਿਟਾਓ [] (dobj->vals_ਸ਼ਬਦ-1);
	    ਮਿਟਾਓ dobj;
	    ਵਾਪਸ;
      }

      fprintf(stderr, "Need support for deleting array type: %d\n", ਵਸਤੂ->vpi_get(vpiArrayType));
      ਪ੍ਰਤਿਜ੍ਞਾ(0);
}
#ਜੇ_ਅੰਤ
