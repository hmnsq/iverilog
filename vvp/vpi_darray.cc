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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ಆದರೆ_ಅಂತ್ಯ
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <cassert>
# include  "ivl_alloc.h"

ಬಳಸಿ ನಾಮಾಕಾಶ std;

__vpiDarrayVar::__vpiDarrayVar(__vpiScope*sc, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*na, vvp_net_t*ne)
: __vpiBaseVar(sc, na, ne)
{
}

ಚಿಹ್ನೆರಹಿತ __vpiDarrayVar::get_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ
{
      ಸ್ಥಿರಾಂಕ vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      ಆದರೆ(!fun)
        ಹಿಂತಿರುಗಿ 0;

      vvp_object_t val = fun->get_object();
      ಸ್ಥಿರಾಂಕ vvp_darray*aval = val.peek<vvp_darray>();

      ಆದರೆ(!aval)
        ಹಿಂತಿರುಗಿ 0;

      ಹಿಂತಿರುಗಿ aval->get_ಗಾತ್ರ();
}

vpiHandle __vpiDarrayVar::get_ಎಡ_range()
{
      ಎಡ_range.ಗಣ_ಮೌಲ್ಯ(0);
      ಹಿಂತಿರುಗಿ &ಎಡ_range;
}

vpiHandle __vpiDarrayVar::get_ಬಲ_range()
{
      ಬಲ_range.ಗಣ_ಮೌಲ್ಯ(get_ಗಾತ್ರ() - 1);
      ಹಿಂತಿರುಗಿ &ಬಲ_range;
}

ಪೂರ್ಣಾಂಕ __vpiDarrayVar::get_ಶಬ್ದ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ
{
      vvp_vector4_t new_vec;
      vvp_darray*aobj = get_vvp_darray();
      aobj->get_ಶಬ್ದ(0, new_vec);
      ಹಿಂತಿರುಗಿ new_vec.size();
}

ಅಕ್ಷರ*__vpiDarrayVar::get_ಶಬ್ದ_str(ರಚನೆ __vpiArrayWord*ಶಬ್ದ, ಪೂರ್ಣಾಂಕ code)
{
      ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ = ಶಬ್ದ->get_ಸೂಚ್ಯಂಕ();

      ಆದರೆ (code == vpiFile) {  // Not implemented for now!
	    ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str(file_ಹೆಸರುಗಳು[0]);
      }

      ಅಕ್ಷರ sidx [64];
      snprintf(sidx, 63, "%d", (ಪೂರ್ಣಾಂಕ)ಸೂಚ್ಯಂಕ);
      ಹಿಂತಿರುಗಿ generic_get_str(code, scope_, name_, sidx);
}

ಶೂನ್ಯ __vpiDarrayVar::get_ಶಬ್ದ_ಮೌಲ್ಯ(ರಚನೆ __vpiArrayWord*ಶಬ್ದ, p_vpi_ಮೌಲ್ಯ vp)
{
      ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ = ಶಬ್ದ->get_ಸೂಚ್ಯಂಕ();
      vvp_darray*aobj = get_vvp_darray();

      ಆದರೆ(vp->format == vpiObjTypeVal) {
          ಆದರೆ(dynamic_cast<vvp_darray_real*>(aobj))
              vp->format = vpiRealVal;
          ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dynamic_cast<vvp_darray_string*>(aobj))
              vp->format = vpiStringVal;
          ಇಲ್ಲದಿದ್ದರೆ
              vp->format = vpi_ಸದಿಶ_Val;
      }

      ಆಯ್ಕೆ(vp->format) {
      ಸ್ಥಿತಿ vpiBinStrVal:
      ಸ್ಥಿತಿ vpiOctStrVal:
      ಸ್ಥಿತಿ vpiDecStrVal:
      ಸ್ಥಿತಿ vpiHexStrVal:
      ಸ್ಥಿತಿ vpiScalarVal:
      ಸ್ಥಿತಿ vpiIntVal:
      ಸ್ಥಿತಿ vpi_ಸದಿಶ_Val:
      {
          vvp_vector4_t v;
          aobj->get_ಶಬ್ದ(ಸೂಚ್ಯಂಕ, v);
          vpip_vec4_get_ಮೌಲ್ಯ(v, v.size(), ಸುಳ್ಳು, vp);  // TODO sign?
      }
      ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ vpiRealVal:
      {
          ದ್ವಿಗುಣ d;
          aobj->get_ಶಬ್ದ(ಸೂಚ್ಯಂಕ, d);
          vpip_real_get_ಮೌಲ್ಯ(d, vp);
      }
      ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ vpiStringVal:
      {
          string s;
          aobj->get_ಶಬ್ದ(ಸೂಚ್ಯಂಕ, s);
          vpip_string_get_ಮೌಲ್ಯ(s, vp);
      }
      ನಿಲ್ಲಿಸು;

      ಪೂರ್ವನಿಯೋಜಿತ:
          fprintf(stderr, "vpi sorry: format is not implemented\n");
          ಪ್ರತಿಜ್ಞಾ(ಸುಳ್ಳು);
      }
}

ಶೂನ್ಯ __vpiDarrayVar::put_ಶಬ್ದ_ಮೌಲ್ಯ(ರಚನೆ __vpiArrayWord*ಶಬ್ದ, p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಂಕ)
{
      ಚಿಹ್ನೆರಹಿತ ಸೂಚ್ಯಂಕ = ಶಬ್ದ->get_ಸೂಚ್ಯಂಕ();
      vvp_darray*aobj = get_vvp_darray();

      ಆಯ್ಕೆ(vp->format) {
      ಸ್ಥಿತಿ vpiScalarVal:
      ಸ್ಥಿತಿ vpiIntVal:
      ಸ್ಥಿತಿ vpi_ಸದಿಶ_Val: {
	    vvp_vector4_t vec = vec4_from_vpi_ಮೌಲ್ಯ(vp, get_ಶಬ್ದ_ಗಾತ್ರ());
	    aobj->ಗಣ_ಶಬ್ದ(ಸೂಚ್ಯಂಕ, vec);
      }
      ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ vpiRealVal:
        aobj->ಗಣ_ಶಬ್ದ(ಸೂಚ್ಯಂಕ, vp->value.real);
        ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ vpiStringVal:
        aobj->ಗಣ_ಶಬ್ದ(ಸೂಚ್ಯಂಕ, std::string(vp->value.str));
        ನಿಲ್ಲಿಸು;

      ಪೂರ್ವನಿಯೋಜಿತ:
          fprintf(stderr, "vpi sorry: format is not implemented");
          ಪ್ರತಿಜ್ಞಾ(ಸುಳ್ಳು);
      }
}

vpiHandle __vpiDarrayVar::get_iter_ಸೂಚ್ಯಂಕ(ರಚನೆ __vpiArrayIterator*, ಪೂರ್ಣಾಂಕ idx)
{
      ಆದರೆ (vals_ಪದಗಳು == 0) make_vals_ಪದಗಳು();

      ಹಿಂತಿರುಗಿ &(vals_ಪದಗಳು[idx].as_ಶಬ್ದ);
}

ಪೂರ್ಣಾಂಕ __vpiDarrayVar::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpiArrayType:
	    ಹಿಂತಿರುಗಿ vpiDynamicArray;
	  ಸ್ಥಿತಿ vpi_ಎಡ_Range:
	    ಹಿಂತಿರುಗಿ 0;
	  ಸ್ಥಿತಿ vpi_ಬಲ_Range:
	    ಹಿಂತಿರುಗಿ get_ಗಾತ್ರ() - 1;
	  ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
            ಹಿಂತಿರುಗಿ get_ಗಾತ್ರ();

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vpi sorry: property is not implemented");
	    ಪ್ರತಿಜ್ಞಾ(ಸುಳ್ಳು);
	    ಹಿಂತಿರುಗಿ 0;
      }
}

ಅಕ್ಷರ* __vpiDarrayVar::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{
      ಆದರೆ (code == vpiFile) {  // Not implemented for now!
            ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str(file_ಹೆಸರುಗಳು[0]);
      }

      ಹಿಂತಿರುಗಿ generic_get_str(code, scope_, name_, ಖಾಲಿ);
}

vpiHandle __vpiDarrayVar::vpi_handle(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
          ಸ್ಥಿತಿ vpi_ಎಡ_Range:
            ಹಿಂತಿರುಗಿ get_ಎಡ_range();

          ಸ್ಥಿತಿ vpi_ಬಲ_Range:
            ಹಿಂತಿರುಗಿ get_ಬಲ_range();

          ಸ್ಥಿತಿ vpiScope:
            ಹಿಂತಿರುಗಿ scope_;

          ಸ್ಥಿತಿ vpiModule:
            ಹಿಂತಿರುಗಿ vpip_module(scope_);
      }

      ಹಿಂತಿರುಗಿ 0;
}

vpiHandle __vpiDarrayVar::vpi_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ)
{
      ಆದರೆ (ಸೂಚ್ಯಂಕ >= (ದೀರ್ಘ) get_ಗಾತ್ರ())
	    ಹಿಂತಿರುಗಿ 0;
      ಆದರೆ (ಸೂಚ್ಯಂಕ < 0)
	    ಹಿಂತಿರುಗಿ 0;

      ಆದರೆ (vals_ಪದಗಳು == 0)
	    make_vals_ಪದಗಳು();

      ಹಿಂತಿರುಗಿ &(vals_ಪದಗಳು[ಸೂಚ್ಯಂಕ].as_ಶಬ್ದ);
}

ಶೂನ್ಯ __vpiDarrayVar::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{
      val->format = vpiSuppressVal;
}

vvp_darray*__vpiDarrayVar::get_vvp_darray() ಸ್ಥಿರಾಂಕ
{
      vvp_fun_signal_object*fun = dynamic_cast<vvp_fun_signal_object*> (get_net()->fun);
      ಪ್ರತಿಜ್ಞಾ(fun);
      vvp_object_t obj = fun->get_object();

      ಹಿಂತಿರುಗಿ obj.peek<vvp_darray>();
}

vpiHandle vpip_make_darray_var(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*use_name = name ? vpip_name_string(name) : ಖಾಲಿ;

      __vpiDarrayVar*obj = ಹೊಸ __vpiDarrayVar(scope, use_name, net);

      ಹಿಂತಿರುಗಿ obj;
}

__vpiQueueVar::__vpiQueueVar(__vpiScope*sc, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*na, vvp_net_t*ne)
: __vpiDarrayVar(sc, na, ne)
{
}

ಪೂರ್ಣಾಂಕ __vpiQueueVar::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpiArrayType:
	    ಹಿಂತಿರುಗಿ vpi_ಸರತಿ_Array;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ __vpiDarrayVar::vpi_get(code);
      }
}

vpiHandle vpip_make_ಸರತಿ_var(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name, vvp_net_t*net)
{
      __vpiScope*scope = vpip_peek_current_scope();
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*use_name = name ? vpip_name_string(name) : ಖಾಲಿ;

      __vpiQueueVar*obj = ಹೊಸ __vpiQueueVar(scope, use_name, net);

      ಹಿಂತಿರುಗಿ obj;
}

__vpiPropQueueRef::__vpiPropQueueRef(__vpiScope*scope, ಚಿಹ್ನೆರಹಿತ pidx,
				     ತಾರ್ಕಿಕ is_ಸರತಿ)
: class_net_(0), prop_idx_(pidx), is_ಸರತಿ_2(is_ಸರತಿ), scope_(scope)
{
}

ಪೂರ್ಣಾಂಕ __vpiPropQueueRef::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiArrayVar; }

ಸ್ಥಿರ vvp_darray* get_ಜೀವಂತ_darray_from_prop(vvp_net_t* class_net, ಚಿಹ್ನೆರಹಿತ pid)
{
      ಆದರೆ (!class_net) ಹಿಂತಿರುಗಿ 0;
      ಸ್ಥಿರಾಂಕ vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(class_net->fun);
      ಆದರೆ (!fun) ಹಿಂತಿರುಗಿ 0;
      vvp_object_t obj = fun->get_object();
      vvp_cobject* cobj = obj.peek<vvp_cobject>();
      ಆದರೆ (!cobj) ಹಿಂತಿರುಗಿ 0;
      vvp_object_t qobj;
      cobj->get_object(pid, qobj, 0);
      ಹಿಂತಿರುಗಿ qobj.peek<vvp_darray>();
}

ಪೂರ್ಣಾಂಕ __vpiPropQueueRef::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಸ್ಥಿರಾಂಕ vvp_darray* aobj = get_ಜೀವಂತ_darray_from_prop(class_net_, prop_idx_);
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpiArrayType:
	    ಹಿಂತಿರುಗಿ is_ಸರತಿ_2 ? vpi_ಸರತಿ_Array : vpiDynamicArray;
	  ಸ್ಥಿತಿ vpi_ಎಡ_Range:
	    ಹಿಂತಿರುಗಿ 0;
	  ಸ್ಥಿತಿ vpi_ಬಲ_Range:
	    ಹಿಂತಿರುಗಿ aobj ? (ಪೂರ್ಣಾಂಕ) (aobj->get_ಗಾತ್ರ() - 1) : 0;
	  ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ aobj ? (ಪೂರ್ಣಾಂಕ) aobj->get_ಗಾತ್ರ() : 0;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ 0;
      }
}

ಅಕ್ಷರ* __vpiPropQueueRef::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{
      ಆದರೆ (code == vpiFile) {
	    ಹಿಂತಿರುಗಿ simple_ಗಣ_rbuf_str(file_ಹೆಸರುಗಳು[0]);
      }
      ಹಿಂತಿರುಗಿ generic_get_str(code, scope_, "prop_darray", ಖಾಲಿ);
}

ಶೂನ್ಯ __vpiPropQueueRef::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{
      val->format = vpiSuppressVal;
}

vpiHandle vpip_make_prop_ಸರತಿ_ref(ಅಕ್ಷರ* class_label, ಚಿಹ್ನೆರಹಿತ prop_idx,
				   ಚಿಹ್ನೆರಹಿತ is_ಸರತಿ_flag)
{
      __vpiPropQueueRef* obj = ಹೊಸ __vpiPropQueueRef(vpip_peek_current_scope(),
						     prop_idx,
						     is_ಸರತಿ_flag != 0);
      functor_ref_lookup(&obj->class_net_, class_label);
      ಹಿಂತಿರುಗಿ obj;
}

vvp_darray* vpip_vpi_darray_from_handle(vpiHandle ref)
{
      ಆದರೆ (!ref) ಹಿಂತಿರುಗಿ 0;

      ಆದರೆ (__vpiPropQueueRef* pr = dynamic_cast<__vpiPropQueueRef*>(ref)) {
	    ಹಿಂತಿರುಗಿ get_ಜೀವಂತ_darray_from_prop(pr->class_net_, pr->prop_idx_);
      }

      ಆದರೆ (ಸ್ಥಿರಾಂಕ __vpiDarrayVar* dv = dynamic_cast<__vpiDarrayVar*>(ref)) {
	    ಸ್ಥಿರಾಂಕ vvp_fun_signal_object* fun = dynamic_cast<vvp_fun_signal_object*>(dv->get_net()->fun);
	    ಆದರೆ (!fun) ಹಿಂತಿರುಗಿ 0;
	    vvp_object_t obj = fun->get_object();
	    ಹಿಂತಿರುಗಿ obj.peek<vvp_darray>();
      }

      ಹಿಂತಿರುಗಿ 0;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ array_delete(vpiHandle ಅಂಶ)
{
      ಆದರೆ (__vpiPropQueueRef* pq = dynamic_cast<__vpiPropQueueRef*>(ಅಂಶ)) {
	    ಅಳಿಸು pq;
	    ಹಿಂತಿರುಗಿ;
      }

      __vpiDarrayVar*dobj = dynamic_cast<__vpiDarrayVar*>(ಅಂಶ);
      ಆದರೆ (dobj) {
	    ಆದರೆ (dobj->vals_ಪದಗಳು) ಅಳಿಸು [] (dobj->vals_ಪದಗಳು-1);
	    ಅಳಿಸು dobj;
	    ಹಿಂತಿರುಗಿ;
      }

      fprintf(stderr, "Need support for deleting array type: %d\n", ಅಂಶ->vpi_get(vpiArrayType));
      ಪ್ರತಿಜ್ಞಾ(0);
}
#ಆದರೆ_ಅಂತ್ಯ
