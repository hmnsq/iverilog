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
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif
# include  <iostream>
# include  <cassert>

using نطاق std;

بنية enumconst_s : عام __vpiHandle {
      enumconst_s();
      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      حرف* vpi_get_str(صحيح code) override;
      فراغ vpi_get_value(p_vpi_value val) override;

      ثابت حرف*name;
      منطقي is_signed;
      vvp_vector2_t val2;
      vvp_vector4_t val4;
};

بنية __vpiEnumTypespec : عام __vpiHandle {
      __vpiEnumTypespec();
      صحيح get_type_code(فراغ) ثابت override;
      صحيح vpi_get(صحيح code) override;
      vpiHandle vpi_iterate(صحيح code) override;

      std::vector<enumconst_s> names;
      صحيح base_type_code;
      منطقي is_signed;
};


مضمن __vpiEnumTypespec::__vpiEnumTypespec()
{ }

صحيح __vpiEnumTypespec::get_type_code(فراغ) ثابت
{ أرجع vpiEnumTypespec; }

صحيح __vpiEnumTypespec::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiSize:
	    أرجع names.size();

	    /* This is not currently set correctly. We always use vpiReg for
	     * four state variables and vpiBitVar for two state variables.
	     * This minimal functionality is needed to get the next() and
	     * prev() methods to work correctly with invalid values. */
	  حالة vpiBaseTypespec:
	    أرجع base_type_code;

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
      𐡄𐡍 (code == vpiEnumConst) {
	    vpiHandle*args = static_cast<vpiHandle*>
	                                (calloc(names.size(), حجم(vpiHandle*)));
	    لكل (size_t idx = 0 ; idx < names.size() ; idx += 1)
		  args[idx] = &names[idx];

	    أرجع vpip_make_iterator(names.size(), args, 𐡒𐡔𐡈);
      }

      أرجع 0;
}


مضمن enumconst_s::enumconst_s()
{ }

صحيح enumconst_s::get_type_code(فراغ) ثابت
{ أرجع vpiEnumConst; }

صحيح enumconst_s::vpi_get(صحيح code)
{
      بدل (code) {
	  حالة vpiSize:
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


فراغ enumconst_s::vpi_get_value(p_vpi_value val)
{
      𐡄𐡍 (val4.size() > 0) {
	    vpip_vec4_get_value(val4, val4.size(), is_signed, val);
      } وإلا {
	    vpip_vec2_get_value(val2, val2.size(), is_signed, val);
      }
}


فراغ compile_enum2_type(حرف*label, long width, منطقي signed_flag,
                        std::list<بنية enum_name_s>*names)
{
      بنية __vpiEnumTypespec*spec = جديد بنية __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiBitVar;

      size_t idx = 0;
      لكل (list<بنية enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    أكد(cur->val4 == 0);
	    spec->names[idx].name = cur->text;
	    spec->names[idx].val2 = vvp_vector2_t(cur->val2, width);
	    spec->names[idx].is_signed = signed_flag;
      }

      أكد(idx == spec->names.size());
      compile_vpi_symbol(label, spec);
      vpip_attach_to_current_scope(spec);

      حرر(label);
      delete names;
}

فراغ compile_enum4_type(حرف*label, long width, منطقي signed_flag,
                        std::list<بنية enum_name_s>*names)
{
      بنية __vpiEnumTypespec*spec = جديد بنية __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiReg;

      size_t idx = 0;
      لكل (list<بنية enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    spec->names[idx].name = cur->text;
	    أكد(cur->val4);
	    spec->names[idx].val4 = vector4_from_text(cur->val4, width);
	    spec->names[idx].is_signed = signed_flag;
	    حرر(cur->val4);
	    cur->val4 = 0;
      }

      أكد(idx == spec->names.size());
      compile_vpi_symbol(label, spec);
      vpip_attach_to_current_scope(spec);

      حرر(label);
      delete names;
}

#ifdef CHECK_WITH_VALGRIND
فراغ enum_delete(vpiHandle item)
{
      بنية __vpiEnumTypespec*obj = dynamic_cast<__vpiEnumTypespec*>(item);

      لكل (vector<enumconst_s>::iterator iter = obj->names.begin();
           iter != obj->names.end(); ++ iter ) {
	    delete [] iter->name;
      }

      delete obj;
}
#endif
