/*
 * Copyright (c) 2010-2025 Stephen Williams (steve@icarus.com)
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
# include  "enum_type.h"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ਜੇ_ਅੰਤ
# include  <iostream>
# include  <cassert>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

ਬਣਤਰ enumconst_s : ਜਨਤਕ __vpiHandle {
      enumconst_s();
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ code) override;
      ਅੱਖਰ* vpi_get_str(ਪੂਰਨ code) override;
      ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;

      ਅਚਲ ਅੱਖਰ*name;
      ਤਾਰਕਿਕ is_signed;
      vvp_vector2_t val2;
      vvp_vector4_t val4;
};

ਬਣਤਰ __vpiEnumTypespec : ਜਨਤਕ __vpiHandle {
      __vpiEnumTypespec();
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;
      ਪੂਰਨ vpi_get(ਪੂਰਨ code) override;
      vpiHandle vpi_iterate(ਪੂਰਨ code) override;

      std::vector<enumconst_s> ਨਾਂ;
      ਪੂਰਨ ਅਧਾਰ_type_ਕੋਡ;
      ਤਾਰਕਿਕ is_signed;
};


ਸਤਰਅੰਦਰ __vpiEnumTypespec::__vpiEnumTypespec()
{ }

ਪੂਰਨ __vpiEnumTypespec::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiEnumTypespec; }

ਪੂਰਨ __vpiEnumTypespec::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ ਨਾਂ.size();

	    /* This is not currently set correctly. We always use vpiReg for
	     * four state variables and vpiBitVar for two state variables.
	     * This minimal functionality is needed to get the next() and
	     * prev() methods to work correctly with invalid values. */
	  ਹਾਲਤ vpi_ਅਧਾਰ_Typespec:
	    ਵਾਪਸ ਅਧਾਰ_type_ਕੋਡ;

	  ਹਾਲਤ vpiSigned:
	    ਵਾਪਸ is_signed;

	  ਮੂਲ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by __vpiEnumTypespec\n", code);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਵਾਪਸ 0;
      }
}


vpiHandle __vpiEnumTypespec::vpi_iterate(ਪੂਰਨ code)
{
      ਜੇ (code == vpiEnumConst) {
	    vpiHandle*ਦਲੀਲਾਂ = static_cast<vpiHandle*>
	                                (ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਨਾਂ.size(), ਆਕਾਰ(vpiHandle*)));
	    ਲਈ (size_t idx = 0 ; idx < ਨਾਂ.size() ; idx += 1)
		  ਦਲੀਲਾਂ[idx] = &ਨਾਂ[idx];

	    ਵਾਪਸ vpip_make_iterator(ਨਾਂ.size(), ਦਲੀਲਾਂ, ਸੱਚ);
      }

      ਵਾਪਸ 0;
}


ਸਤਰਅੰਦਰ enumconst_s::enumconst_s()
{ }

ਪੂਰਨ enumconst_s::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiEnumConst; }

ਪੂਰਨ enumconst_s::vpi_get(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpi_ਮਾਪ:
	    ਵਾਪਸ val4.size()? val4.size() : val2.size();
	  ਮੂਲ:
	    ਵਾਪਸ 0;
      }
}


ਅੱਖਰ* enumconst_s::vpi_get_str(ਪੂਰਨ code)
{
      ਚੋਣ (code) {
	  ਹਾਲਤ vpiName:
	    ਵਾਪਸ const_cast<ਅੱਖਰ*> (name);
	  ਮੂਲ:
	    ਵਾਪਸ 0;
      }
}


ਖਾਲੀ enumconst_s::vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val)
{
      ਜੇ (val4.size() > 0) {
	    vpip_vec4_get_ਮੁੱਲ(val4, val4.size(), is_signed, val);
      } ਨਹੀਂਤਾਂ {
	    vpip_vec2_get_ਮੁੱਲ(val2, val2.size(), is_signed, val);
      }
}


ਖਾਲੀ compile_enum2_type(ਅੱਖਰ*label, ਲੰਮਾ width, ਤਾਰਕਿਕ signed_flag,
                        std::list<ਬਣਤਰ enum_name_s>*ਨਾਂ)
{
      ਬਣਤਰ __vpiEnumTypespec*spec = ਨਵਾਂ ਬਣਤਰ __vpiEnumTypespec;
      spec->ਨਾਂ = std::vector<enumconst_s> (ਨਾਂ->size());
      spec->is_signed = signed_flag;
      spec->ਅਧਾਰ_type_ਕੋਡ = vpi_ਦੋਆਧਾਰੀ_ਅੰਕ_Var;

      size_t idx = 0;
      ਲਈ (list<ਬਣਤਰ enum_name_s>::iterator cur = ਨਾਂ->begin()
		 ; cur != ਨਾਂ->ਸਿਰਾ() ;  ++cur, ++idx) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(cur->val4 == 0);
	    spec->ਨਾਂ[idx].name = cur->ਪਾਠ;
	    spec->ਨਾਂ[idx].val2 = vvp_vector2_t(cur->val2, width);
	    spec->ਨਾਂ[idx].is_signed = signed_flag;
      }

      ਪ੍ਰਤਿਜ੍ਞਾ(idx == spec->ਨਾਂ.size());
      compile_vpi_ਚਿੰਨ੍ਹ(label, spec);
      vpip_attach_ਮੰਜ਼ਿਲ_current_scope(spec);

      ਮੁਕਤ(label);
      ਮਿਟਾਓ ਨਾਂ;
}

ਖਾਲੀ compile_enum4_type(ਅੱਖਰ*label, ਲੰਮਾ width, ਤਾਰਕਿਕ signed_flag,
                        std::list<ਬਣਤਰ enum_name_s>*ਨਾਂ)
{
      ਬਣਤਰ __vpiEnumTypespec*spec = ਨਵਾਂ ਬਣਤਰ __vpiEnumTypespec;
      spec->ਨਾਂ = std::vector<enumconst_s> (ਨਾਂ->size());
      spec->is_signed = signed_flag;
      spec->ਅਧਾਰ_type_ਕੋਡ = vpiReg;

      size_t idx = 0;
      ਲਈ (list<ਬਣਤਰ enum_name_s>::iterator cur = ਨਾਂ->begin()
		 ; cur != ਨਾਂ->ਸਿਰਾ() ;  ++cur, ++idx) {
	    spec->ਨਾਂ[idx].name = cur->ਪਾਠ;
	    ਪ੍ਰਤਿਜ੍ਞਾ(cur->val4);
	    spec->ਨਾਂ[idx].val4 = vector4_from_ਪਾਠ(cur->val4, width);
	    spec->ਨਾਂ[idx].is_signed = signed_flag;
	    ਮੁਕਤ(cur->val4);
	    cur->val4 = 0;
      }

      ਪ੍ਰਤਿਜ੍ਞਾ(idx == spec->ਨਾਂ.size());
      compile_vpi_ਚਿੰਨ੍ਹ(label, spec);
      vpip_attach_ਮੰਜ਼ਿਲ_current_scope(spec);

      ਮੁਕਤ(label);
      ਮਿਟਾਓ ਨਾਂ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ enum_delete(vpiHandle ਵਸਤੂ)
{
      ਬਣਤਰ __vpiEnumTypespec*obj = dynamic_cast<__vpiEnumTypespec*>(ਵਸਤੂ);

      ਲਈ (vector<enumconst_s>::iterator iter = obj->ਨਾਂ.begin();
           iter != obj->ਨਾਂ.ਸਿਰਾ(); ++ iter ) {
	    ਮਿਟਾਓ [] iter->name;
      }

      ਮਿਟਾਓ obj;
}
#ਜੇ_ਅੰਤ
