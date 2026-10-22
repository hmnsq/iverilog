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
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#جيڪڏهن_پورو
# include  <iostream>
# include  <cassert>

استعمال نطاق std;

بنية enumconst_s : عام __vpiHandle {
      enumconst_s();
      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      فراغ vpi_get_قدر(p_vpi_قدر val) override;

      ثابت حرف*name;
      منطقي is_signed;
      vvp_vector2_t val2;
      vvp_vector4_t val4;
};

بنية __vpiEnumTypespec : عام __vpiHandle {
      __vpiEnumTypespec();
      صحيح get_type_ڪوڊ(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      vpiHandle vpi_iterate(صحيح code) override;

      std::vector<enumconst_s> نالا;
      صحيح بنياد_type_ڪوڊ;
      منطقي is_signed;
};


مضمن __vpiEnumTypespec::__vpiEnumTypespec()
{ }

صحيح __vpiEnumTypespec::get_type_ڪوڊ(فراغ) ثابت
{ أرجع vpiEnumTypespec; }

صحيح __vpiEnumTypespec::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_سائيز:
	    أرجع نالا.size();

	    /* This is not currently set correctly. We always use vpiReg for
	     * four state variables and vpiBitVar for two state variables.
	     * This minimal functionality is needed to get the next() and
	     * prev() methods to work correctly with invalid values. */
	  حالة vpi_بنياد_Typespec:
	    أرجع بنياد_type_ڪوڊ;

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
	    vpiHandle*دليلون = static_cast<vpiHandle*>
	                                (صفر_يادگيري_ڏيو(نالا.size(), حجم(vpiHandle*)));
	    لكل (size_t idx = 0 ; idx < نالا.size() ; idx += 1)
		  دليلون[idx] = &نالا[idx];

	    أرجع vpip_make_iterator(نالا.size(), دليلون, سچ);
      }

      أرجع 0;
}


مضمن enumconst_s::enumconst_s()
{ }

صحيح enumconst_s::get_type_ڪوڊ(فراغ) ثابت
{ أرجع vpiEnumConst; }

صحيح enumconst_s::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpi_سائيز:
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


فراغ enumconst_s::vpi_get_قدر(p_vpi_قدر val)
{
      إذا (val4.size() > 0) {
	    vpip_vec4_get_قدر(val4, val4.size(), is_signed, val);
      } وإلا {
	    vpip_vec2_get_قدر(val2, val2.size(), is_signed, val);
      }
}


فراغ compile_enum2_type(حرف*label, ڊگهو width, منطقي signed_flag,
                        std::list<بنية enum_name_s>*نالا)
{
      بنية __vpiEnumTypespec*spec = جديد بنية __vpiEnumTypespec;
      spec->نالا = std::vector<enumconst_s> (نالا->size());
      spec->is_signed = signed_flag;
      spec->بنياد_type_ڪوڊ = vpi_ٻٽو_انگ_Var;

      size_t idx = 0;
      لكل (list<بنية enum_name_s>::iterator cur = نالا->begin()
		 ; cur != نالا->پڇاڙي() ;  ++cur, ++idx) {
	    أكد(cur->val4 == 0);
	    spec->نالا[idx].name = cur->متن;
	    spec->نالا[idx].val2 = vvp_vector2_t(cur->val2, width);
	    spec->نالا[idx].is_signed = signed_flag;
      }

      أكد(idx == spec->نالا.size());
      compile_vpi_نشان(label, spec);
      vpip_attach_منزل_current_scope(spec);

      حرر(label);
      ميٽيو نالا;
}

فراغ compile_enum4_type(حرف*label, ڊگهو width, منطقي signed_flag,
                        std::list<بنية enum_name_s>*نالا)
{
      بنية __vpiEnumTypespec*spec = جديد بنية __vpiEnumTypespec;
      spec->نالا = std::vector<enumconst_s> (نالا->size());
      spec->is_signed = signed_flag;
      spec->بنياد_type_ڪوڊ = vpiReg;

      size_t idx = 0;
      لكل (list<بنية enum_name_s>::iterator cur = نالا->begin()
		 ; cur != نالا->پڇاڙي() ;  ++cur, ++idx) {
	    spec->نالا[idx].name = cur->متن;
	    أكد(cur->val4);
	    spec->نالا[idx].val4 = vector4_from_متن(cur->val4, width);
	    spec->نالا[idx].is_signed = signed_flag;
	    حرر(cur->val4);
	    cur->val4 = 0;
      }

      أكد(idx == spec->نالا.size());
      compile_vpi_نشان(label, spec);
      vpip_attach_منزل_current_scope(spec);

      حرر(label);
      ميٽيو نالا;
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ enum_delete(vpiHandle شي)
{
      بنية __vpiEnumTypespec*obj = dynamic_cast<__vpiEnumTypespec*>(شي);

      لكل (vector<enumconst_s>::iterator iter = obj->نالا.begin();
           iter != obj->نالا.پڇاڙي(); ++ iter ) {
	    ميٽيو [] iter->name;
      }

      ميٽيو obj;
}
#جيڪڏهن_پورو
