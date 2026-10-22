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
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#అయితే_ముగింపు
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

ఉపయోగించి నామాకాశ std;

__vpiDarrayVar::__vpiDarrayVar(__vpiScope*sc, స్థిరరాశి అక్షరం*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

గుర్తులేని __vpiDarrayVar::get_కొలత() స్థిరరాశి
{
      స్థిరరాశి vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      ఒకవేళ(!fun)
        తిరిగి 0;

      vvp_object_t val = fun->get_object();
      స్థిరరాశి vvp_darray*aval = val.peek<vvp_darray>();

      ఒకవేళ(!aval)
        తిరిగి 0;

      తిరిగి aval->get_కొలత();
}

vpiHandle __vpiDarrayVar::get_ఎడమ_range()
{
      ఎడమ_range.సమితి_విలువ(0);
      తిరిగి &ఎడమ_range;
}

vpiHandle __vpiDarrayVar::get_కుడి_range()
{
      కుడి_range.సమితి_విలువ(get_కొలత() - 1);
      తిరిగి &కుడి_range;
}

పూర్ణాంకం __vpiDarrayVar::get_మాట_కొలత() స్థిరరాశి
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_మాట(0, new_vec);
      తిరిగి new_vec.size();
}

అక్షరం*__vpiDarrayVar::get_మాట_str(నిర్మాణం __vpiArrayWord*మాట, పూర్ణాంకం code)
{
      గుర్తులేని సూచీ = మాట->get_సూచీ();

      ఒకవేళ (code == vpiFile) {  // Not implemented for now!
	    తిరిగి simple_సమితి_rbuf_str(file_పేర్లు[0]);
      }

      అక్షరం sidx [64];
      snprintf(sidx, 63, "%d", (పూర్ణాంకం)సూచీ);
      తిరిగి generic_get_str(code, scope_, name_, sidx);
}

శూన్య __vpiDarrayVar::get_మాట_విలువ(నిర్మాణం __vpiArrayWord*మాట, p_vpi_విలువ vp)
{
      గుర్తులేని సూచీ = మాట->get_సూచీ();
      vvp_darray*aobj = get_vvp_darray();

      ఒకవేళ(vp->format == vpiObjTypeVal) {
          ఒకవేళ(dynamic_cast<vvp_darray_real*>(aobj))
              vp->format = vpiRealVal;
          లేకపోతే ఒకవేళ(dynamic_cast<vvp_darray_string*>(aobj))
              vp->format = vpiStringVal;
          లేకపోతే
              vp->format = vpi_సదిశ_Val;
      }

      ఎంపిక(vp->format) {
      స్థితి vpiBinStrVal:
      స్థితి vpiOctStrVal:
      స్థితి vpiDecStrVal:
      స్థితి vpiHexStrVal:
      స్థితి vpiScalarVal:
      స్థితి vpiIntVal:
      స్థితి vpi_సదిశ_Val:
      {
          vvp_vector4_t v;
          aobj->get_మాట(సూచీ, v);
          vpip_vec4_get_విలువ(v, v.size(), అబద్ధం, vp);  // TODO sign?
      }
      ఆపు;

      స్థితి vpiRealVal:
      {
          ద్విగుణ d;
          aobj->get_మాట(సూచీ, d);
          vpip_real_get_విలువ(d, vp);
      }
      ఆపు;

      స్థితి vpiStringVal:
      {
          string s;
          aobj->get_మాట(సూచీ, s);
          vpip_string_get_విలువ(s, vp);
      }
      ఆపు;

      అప్రమేయ:
          fprintf(stderr, "vpi sorry: format is not implemented\n");
          ప్రతిజ్ఞా(అబద్ధం);
      }
}

శూన్య __vpiDarrayVar::put_మాట_విలువ(నిర్మాణం __vpiArrayWord*మాట, p_vpi_విలువ vp, పూర్ణాంకం)
{
      గుర్తులేని సూచీ = మాట->get_సూచీ();
      vvp_darray*aobj = get_vvp_darray();

      ఎంపిక(vp->format) {
      స్థితి vpiScalarVal:
      స్థితి vpiIntVal:
      స్థితి vpi_సదిశ_Val: {
	    vvp_vector4_t vec = vec4_from_vpi_విలువ(vp, get_మాట_కొలత());
	    aobj->సమితి_మాట(సూచీ, vec);
      }
      ఆపు;

      స్థితి vpiRealVal:
        aobj->సమితి_మాట(సూచీ, vp->value.real);
        ఆపు;

      స్థితి vpiStringVal:
        aobj->సమితి_మాట(సూచీ, std::string(vp->value.str));
        ఆపు;

      అప్రమేయ:
          fprintf(stderr, "vpi sorry: format is not implemented");
          ప్రతిజ్ఞా(అబద్ధం);
      }
}

vpiHandle __vpiDarrayVar::get_iter_సూచీ(నిర్మాణం __vpiArrayIterator*, పూర్ణాంకం idx)
{
      ఒకవేళ (vals_పదాలు == 0) make_vals_పదాలు();

      తిరిగి &(vals_పదాలు[idx].as_మాట);
}

పూర్ణాంకం __vpiDarrayVar::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpiArrayType:
	    తిరిగి vpiDynamicArray;
	  స్థితి vpi_ఎడమ_Range:
	    తిరిగి 0;
	  స్థితి vpi_కుడి_Range:
	    తిరిగి get_కొలత() - 1;
	  స్థితి vpi_కొలత:
            తిరిగి get_కొలత();

	  అప్రమేయ:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    ప్రతిజ్ఞా(అబద్ధం);
	    తిరిగి 0;
      }
}

అక్షరం* __vpiDarrayVar::vpi_get_str(పూర్ణాంకం code)
{
      ఒకవేళ (code == vpiFile) {  // Not implemented for now!
            తిరిగి simple_సమితి_rbuf_str(file_పేర్లు[0]);
      }

      తిరిగి generic_get_str(code, scope_, name_, ఖాళీ);
}

vpiHandle __vpiDarrayVar::vpi_handle(పూర్ణాంకం code)
{
      ఎంపిక (code) {
          స్థితి vpi_ఎడమ_Range:
            తిరిగి get_ఎడమ_range();

          స్థితి vpi_కుడి_Range:
            తిరిగి get_కుడి_range();

          స్థితి vpiScope:
            తిరిగి scope_;

          స్థితి vpiModule:
            తిరిగి vpip_module(scope_);
      }

      తిరిగి 0;
}

vpiHandle __vpiDarrayVar::vpi_సూచీ(పూర్ణాంకం సూచీ)
{
      ఒకవేళ (సూచీ >= (దీర్ఘ) get_కొలత())
	    తిరిగి 0;
      ఒకవేళ (సూచీ < 0)
	    తిరిగి 0;

      ఒకవేళ (vals_పదాలు == 0)
	    make_vals_పదాలు();

      తిరిగి &(vals_పదాలు[సూచీ].as_మాట);
}

శూన్య __vpiDarrayVar::vpi_get_విలువ(p_vpi_విలువ val)
{
      val->format = vpiSuppressVal;
}

vvp_darray*__vpiDarrayVar::get_vvp_darray() స్థిరరాశి
{
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      ప్రతిజ్ఞా(fun);
      vvp_object_t obj = fun->get_object();

      తిరిగి obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(స్థిరరాశి అక్షరం*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      స్థిరరాశి అక్షరం*use_name = name ? vpip_name_string(name) : ఖాళీ;

      __vpiDarrayVar*obj = కొత్త __vpiDarrayVar(scope, use_name, net);

      తిరిగి obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, స్థిరరాశి అక్షరం*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

పూర్ణాంకం __vpiQueueVar::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpiArrayType:
	    తిరిగి vpi_వరుస_Array;
	  అప్రమేయ:
	    తిరిగి __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_వరుస_var(స్థిరరాశి అక్షరం*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      స్థిరరాశి అక్షరం*use_name = name ? vpip_name_string(name) : ఖాళీ;

      __vpiQueueVar*obj = కొత్త __vpiQueueVar(scope, use_name, net);

      తిరిగి obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, గుర్తులేని pidx,
				     తార్కిక is_వరుస)
: class_net_(0), prop_idx_(pidx), is_వరుస_2(is_వరుస), scope_(scope)
{
}

పూర్ణాంకం __vpiPropQueueRef::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiArrayVar; }

స్థిర vvp_darray* get_సజీవ_darray_from_prop(vvp_net_t* class_net, గుర్తులేని pid)
{
      ఒకవేళ (!class_net) తిరిగి 0;
      స్థిరరాశి vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      ఒకవేళ (!fun) తిరిగి 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      ఒకవేళ (!cobj) తిరిగి 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      తిరిగి qobj.peek<vvp_darray>();
}

పూర్ణాంకం __vpiPropQueueRef::vpi_get(పూర్ణాంకం code)
{
      స్థిరరాశి vvp_darray* aobj = get_సజీవ_darray_from_prop(class_net_, prop_idx_);
      ఎంపిక (code) {
	  స్థితి vpiArrayType:
	    తిరిగి is_వరుస_2 ? vpi_వరుస_Array : vpiDynamicArray;
	  స్థితి vpi_ఎడమ_Range:
	    తిరిగి 0;
	  స్థితి vpi_కుడి_Range:
	    తిరిగి aobj ? (పూర్ణాంకం) (aobj->get_కొలత() - 1) : 0;
	  స్థితి vpi_కొలత:
	    తిరిగి aobj ? (పూర్ణాంకం) aobj->get_కొలత() : 0;
	  అప్రమేయ:
	    తిరిగి 0;
      }
}

అక్షరం* __vpiPropQueueRef::vpi_get_str(పూర్ణాంకం code)
{
      ఒకవేళ (code == vpiFile) {
	    తిరిగి simple_సమితి_rbuf_str(file_పేర్లు[0]);
      }
      తిరిగి generic_get_str(code, scope_, "prop_darray", ఖాళీ);
}

శూన్య __vpiPropQueueRef::vpi_get_విలువ(p_vpi_విలువ val)
{
      val->format = vpiSuppressVal;
}

vpiHandle vpip_make_prop_వరుస_ref(అక్షరం* class_label, గుర్తులేని prop_idx,
				   గుర్తులేని is_వరుస_flag)
{
      __vpiPropQueueRef* obj = కొత్త __vpiPropQueueRef(vpip_peek_current_scope(),
						     prop_idx,
						     is_వరుస_flag != 0);
      functor_ref_lookup(&obj->class_net_, class_label);
      తిరిగి obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      ఒకవేళ (!ref) తిరిగి 0;

      ఒకవేళ (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    తిరిగి get_సజీవ_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      ఒకవేళ (స్థిరరాశి __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    స్థిరరాశి vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    ఒకవేళ (!fun) తిరిగి 0;
	    vvp_object_t obj = fun->get_object();
	    తిరిగి obj.peek<vvp_darray>();
      }

      తిరిగి 0;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య array_delete(vpiHandle అంశం)
{
      ఒకవేళ (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(అంశం)) {
	    తొలగించు pq;
	    తిరిగి;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(అంశం);
      ఒకవేళ (dobj) {
	    ఒకవేళ (dobj->vals_పదాలు) తొలగించు [] (dobj->vals_పదాలు-1);
	    తొలగించు dobj;
	    తిరిగి;
      }

      fprintf(stderr, "Need support for deleting array type: %d\n", అంశం->vpi_get(vpiArrayType));
      ప్రతిజ్ఞా(0);
}
#అయితే_ముగింపు
