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

using فضاینام std;

ساختار enumconst_s : عمومی __vpiHandle {
      enumconst_s();
      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      نویسه* vpi_get_str(صحیح code) override;
      تهی vpi_get_مقدار(p_vpi_مقدار val) override;

      ثابت نویسه*name;
      منطقی is_signed;
      vvp_vector2_t val2;
      vvp_vector4_t val4;
};

ساختار __vpiEnumTypespec : عمومی __vpiHandle {
      __vpiEnumTypespec();
      صحیح get_type_code(تهی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      vpiHandle vpi_iterate(صحیح code) override;

      std::vector<enumconst_s> names;
      صحیح base_type_code;
      منطقی is_signed;
};


درونخطی __vpiEnumTypespec::__vpiEnumTypespec()
{ }

صحیح __vpiEnumTypespec::get_type_code(تهی) ثابت
{ بازگشت vpiEnumTypespec; }

صحیح __vpiEnumTypespec::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpiSize:
	    بازگشت names.size();

	    /* This is not currently set correctly. We always use vpiReg for
	     * four state variables and vpiBitVar for two state variables.
	     * This minimal functionality is needed to get the next() and
	     * prev() methods to work correctly with invalid values. */
	  حالت vpiBaseTypespec:
	    بازگشت base_type_code;

	  حالت vpiSigned:
	    بازگشت is_signed;

	  پیشفرض:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by __vpiEnumTypespec\n", code);
	    أكد(0);
	    بازگشت 0;
      }
}


vpiHandle __vpiEnumTypespec::vpi_iterate(صحیح code)
{
      اگر (code == vpiEnumConst) {
	    vpiHandle*args = static_cast<vpiHandle*>
	                                (calloc(names.size(), اندازه(vpiHandle*)));
	    برای (size_t idx = 0 ; idx < names.size() ; idx += 1)
		  args[idx] = &names[idx];

	    بازگشت vpip_make_iterator(names.size(), args, درست);
      }

      بازگشت 0;
}


درونخطی enumconst_s::enumconst_s()
{ }

صحیح enumconst_s::get_type_code(تهی) ثابت
{ بازگشت vpiEnumConst; }

صحیح enumconst_s::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpiSize:
	    بازگشت val4.size()? val4.size() : val2.size();
	  پیشفرض:
	    بازگشت 0;
      }
}


نویسه* enumconst_s::vpi_get_str(صحیح code)
{
      انتخاب (code) {
	  حالت vpiName:
	    بازگشت const_cast<نویسه*> (name);
	  پیشفرض:
	    بازگشت 0;
      }
}


تهی enumconst_s::vpi_get_مقدار(p_vpi_مقدار val)
{
      اگر (val4.size() > 0) {
	    vpip_vec4_get_مقدار(val4, val4.size(), is_signed, val);
      } وگرنه {
	    vpip_vec2_get_مقدار(val2, val2.size(), is_signed, val);
      }
}


تهی compile_enum2_type(نویسه*label, long width, منطقی signed_flag,
                        std::list<ساختار enum_name_s>*names)
{
      ساختار __vpiEnumTypespec*spec = جدید ساختار __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiBitVar;

      size_t idx = 0;
      برای (list<ساختار enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    أكد(cur->val4 == 0);
	    spec->names[idx].name = cur->text;
	    spec->names[idx].val2 = vvp_vector2_t(cur->val2, width);
	    spec->names[idx].is_signed = signed_flag;
      }

      أكد(idx == spec->names.size());
      compile_vpi_نماد(label, spec);
      vpip_attach_to_current_scope(spec);

      آزاد(label);
      delete names;
}

تهی compile_enum4_type(نویسه*label, long width, منطقی signed_flag,
                        std::list<ساختار enum_name_s>*names)
{
      ساختار __vpiEnumTypespec*spec = جدید ساختار __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiReg;

      size_t idx = 0;
      برای (list<ساختار enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    spec->names[idx].name = cur->text;
	    أكد(cur->val4);
	    spec->names[idx].val4 = vector4_from_text(cur->val4, width);
	    spec->names[idx].is_signed = signed_flag;
	    آزاد(cur->val4);
	    cur->val4 = 0;
      }

      أكد(idx == spec->names.size());
      compile_vpi_نماد(label, spec);
      vpip_attach_to_current_scope(spec);

      آزاد(label);
      delete names;
}

#ifdef CHECK_WITH_VALGRIND
تهی enum_delete(vpiHandle item)
{
      ساختار __vpiEnumTypespec*obj = dynamic_cast<__vpiEnumTypespec*>(item);

      برای (vector<enumconst_s>::iterator iter = obj->names.begin();
           iter != obj->names.end(); ++ iter ) {
	    delete [] iter->name;
      }

      delete obj;
}
#endif
