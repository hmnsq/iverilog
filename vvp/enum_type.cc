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
#که_تعریف_شوی CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#د_که_پای
# include  <iostream>
# include  <cassert>

کارول نطاق std;

بنية enumconst_s : عام __vpiHandle {
      enumconst_s();
      صحيح get_type_کوډ(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      فراغ vpi_get_ارزښت(p_vpi_ارزښت val) override;

      ثابت حرف*name;
      منطقي is_signed;
      vvp_vector2_t val2;
      vvp_vector4_t val4;
};

بنية __vpiEnumTypespec : عام __vpiHandle {
      __vpiEnumTypespec();
      صحيح get_type_کوډ(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      vpiHandle vpi_iterate(صحيح code) override;

      std::vector<enumconst_s> نومونه;
      صحيح بنسټ_type_کوډ;
      منطقي is_signed;
};


مضمن __vpiEnumTypespec::__vpiEnumTypespec()
{ }

صحيح __vpiEnumTypespec::get_type_کوډ(فراغ) ثابت
{ أرجع vpiEnumTypespec; }

صحيح __vpiEnumTypespec::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_اندازه:
	    أرجع نومونه.size();

	    /* This is not currently set correctly. We always use vpiReg for
	     * four state variables and vpiBitVar for two state variables.
	     * This minimal functionality is needed to get the next() and
	     * prev() methods to work correctly with invalid values. */
	  حالة vpi_بنسټ_Typespec:
	    أرجع بنسټ_type_کوډ;

	  حالة vpiSigned:
	    أرجع is_signed;

	  افتراضي:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by __vpiEnumTypespec\n", code);
	    أكد(0);
	    أرجع 0;
      }
}


vpiHandle __vpiEnumTypespec::vpi_iterate(صحيح code)
{
      إذا (code == vpiEnumConst) {
	    vpiHandle*دلیلونه = static_cast<vpiHandle*>
	                                (حافظه_صفر_ورکړه(نومونه.size(), حجم(vpiHandle*)));
	    لكل (size_t idx = 0 ; idx < نومونه.size() ; idx += 1)
		  دلیلونه[idx] = &نومونه[idx];

	    أرجع vpip_make_iterator(نومونه.size(), دلیلونه, ریښتیا);
      }

      أرجع 0;
}


مضمن enumconst_s::enumconst_s()
{ }

صحيح enumconst_s::get_type_کوډ(فراغ) ثابت
{ أرجع vpiEnumConst; }

صحيح enumconst_s::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_اندازه:
	    أرجع val4.size()? val4.size() : val2.size();
	  افتراضي:
	    أرجع 0;
      }
}


حرف* enumconst_s::vpi_get_str(صحيح code)
{
      بدل (code) {
	  حالة vpiName:
	    أرجع const_cast<حرف*> (name);
	  افتراضي:
	    أرجع 0;
      }
}


فراغ enumconst_s::vpi_get_ارزښت(p_vpi_ارزښت val)
{
      إذا (val4.size() > 0) {
	    vpip_vec4_get_ارزښت(val4, val4.size(), is_signed, val);
      } وإلا {
	    vpip_vec2_get_ارزښت(val2, val2.size(), is_signed, val);
      }
}


فراغ compile_enum2_type(حرف*label, اوږد width, منطقي signed_flag,
                        std::list<بنية enum_name_s>*نومونه)
{
      بنية __vpiEnumTypespec*spec = جديد بنية __vpiEnumTypespec;
      spec->نومونه = std::vector<enumconst_s> (نومونه->size());
      spec->is_signed = signed_flag;
      spec->بنسټ_type_کوډ = vpi_بټ_Var;

      size_t idx = 0;
      لكل (list<بنية enum_name_s>::iterator cur = نومونه->begin()
		 ; cur != نومونه->پای_ټکی() ;  ++cur, ++idx) {
	    أكد(cur->val4 == 0);
	    spec->نومونه[idx].name = cur->متن;
	    spec->نومونه[idx].val2 = vvp_vector2_t(cur->val2, width);
	    spec->نومونه[idx].is_signed = signed_flag;
      }

      أكد(idx == spec->نومونه.size());
      compile_vpi_نښه(label, spec);
      vpip_attach_منزل_current_scope(spec);

      حرر(label);
      ړنګول نومونه;
}

فراغ compile_enum4_type(حرف*label, اوږد width, منطقي signed_flag,
                        std::list<بنية enum_name_s>*نومونه)
{
      بنية __vpiEnumTypespec*spec = جديد بنية __vpiEnumTypespec;
      spec->نومونه = std::vector<enumconst_s> (نومونه->size());
      spec->is_signed = signed_flag;
      spec->بنسټ_type_کوډ = vpiReg;

      size_t idx = 0;
      لكل (list<بنية enum_name_s>::iterator cur = نومونه->begin()
		 ; cur != نومونه->پای_ټکی() ;  ++cur, ++idx) {
	    spec->نومونه[idx].name = cur->متن;
	    أكد(cur->val4);
	    spec->نومونه[idx].val4 = vector4_from_متن(cur->val4, width);
	    spec->نومونه[idx].is_signed = signed_flag;
	    حرر(cur->val4);
	    cur->val4 = 0;
      }

      أكد(idx == spec->نومونه.size());
      compile_vpi_نښه(label, spec);
      vpip_attach_منزل_current_scope(spec);

      حرر(label);
      ړنګول نومونه;
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ enum_delete(vpiHandle توکی)
{
      بنية __vpiEnumTypespec*obj = dynamic_cast<__vpiEnumTypespec*>(توکی);

      لكل (vector<enumconst_s>::iterator iter = obj->نومونه.begin();
           iter != obj->نومونه.پای_ټکی(); ++ iter ) {
	    ړنګول [] iter->name;
      }

      ړنګول obj;
}
#د_که_پای
