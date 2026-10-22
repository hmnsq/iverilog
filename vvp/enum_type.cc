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

using নামাকাশ std;

সংরচনা enumconst_s : সার্ৱজনিক __vpiHandle {
      enumconst_s();
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুৱ override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      ৱর্ণ* vpi_get_str(পূর্ণাঙ্ক code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;

      ধ্রুৱ ৱর্ণ*name;
      তার্কিক is_signed;
      vvp_vector2_t val2;
      vvp_vector4_t val4;
};

সংরচনা __vpiEnumTypespec : সার্ৱজনিক __vpiHandle {
      __vpiEnumTypespec();
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুৱ override;
      পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_iterate(পূর্ণাঙ্ক code) override;

      std::vector<enumconst_s> names;
      পূর্ণাঙ্ক base_type_code;
      তার্কিক is_signed;
};


অন্তঃস্থ __vpiEnumTypespec::__vpiEnumTypespec()
{ }

পূর্ণাঙ্ক __vpiEnumTypespec::get_type_code(শূন্য) ধ্রুৱ
{ ঘূৰাই vpiEnumTypespec; }

পূর্ণাঙ্ক __vpiEnumTypespec::vpi_get(পূর্ণাঙ্ক code)
{
      ৱিকল্প (code) {
	  পক্ষ vpiSize:
	    ঘূৰাই names.size();

	    /* This is not currently set correctly. We always use vpiReg for
	     * four state variables and vpiBitVar for two state variables.
	     * This minimal functionality is needed to get the next() and
	     * prev() methods to work correctly with invalid values. */
	  পক্ষ vpiBaseTypespec:
	    ঘূৰাই base_type_code;

	  পক্ষ vpiSigned:
	    ঘূৰাই is_signed;

	  স্ৱভাৱ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by __vpiEnumTypespec\n", code);
	    প্রতিজ্ঞা(0);
	    ঘূৰাই 0;
      }
}


vpiHandle __vpiEnumTypespec::vpi_iterate(পূর্ণাঙ্ক code)
{
      যদি (code == vpiEnumConst) {
	    vpiHandle*args = static_cast<vpiHandle*>
	                                (calloc(names.size(), পরিমাণ(vpiHandle*)));
	    বাবে (size_t idx = 0 ; idx < names.size() ; idx += 1)
		  args[idx] = &names[idx];

	    ঘূৰাই vpip_make_iterator(names.size(), args, সঁচা);
      }

      ঘূৰাই 0;
}


অন্তঃস্থ enumconst_s::enumconst_s()
{ }

পূর্ণাঙ্ক enumconst_s::get_type_code(শূন্য) ধ্রুৱ
{ ঘূৰাই vpiEnumConst; }

পূর্ণাঙ্ক enumconst_s::vpi_get(পূর্ণাঙ্ক code)
{
      ৱিকল্প (code) {
	  পক্ষ vpiSize:
	    ঘূৰাই val4.size()? val4.size() : val2.size();
	  স্ৱভাৱ:
	    ঘূৰাই 0;
      }
}


ৱর্ণ* enumconst_s::vpi_get_str(পূর্ণাঙ্ক code)
{
      ৱিকল্প (code) {
	  পক্ষ vpiName:
	    ঘূৰাই const_cast<ৱর্ণ*> (name);
	  স্ৱভাৱ:
	    ঘূৰাই 0;
      }
}


শূন্য enumconst_s::vpi_get_মান(p_vpi_মান val)
{
      যদি (val4.size() > 0) {
	    vpip_vec4_get_মান(val4, val4.size(), is_signed, val);
      } অন্যথা {
	    vpip_vec2_get_মান(val2, val2.size(), is_signed, val);
      }
}


শূন্য compile_enum2_type(ৱর্ণ*label, long width, তার্কিক signed_flag,
                        std::list<সংরচনা enum_name_s>*names)
{
      সংরচনা __vpiEnumTypespec*spec = নূতন সংরচনা __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiBitVar;

      size_t idx = 0;
      বাবে (list<সংরচনা enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    প্রতিজ্ঞা(cur->val4 == 0);
	    spec->names[idx].name = cur->text;
	    spec->names[idx].val2 = vvp_vector2_t(cur->val2, width);
	    spec->names[idx].is_signed = signed_flag;
      }

      প্রতিজ্ঞা(idx == spec->names.size());
      compile_vpi_প্ৰতীক(label, spec);
      vpip_attach_to_current_scope(spec);

      মুক্ত(label);
      delete names;
}

শূন্য compile_enum4_type(ৱর্ণ*label, long width, তার্কিক signed_flag,
                        std::list<সংরচনা enum_name_s>*names)
{
      সংরচনা __vpiEnumTypespec*spec = নূতন সংরচনা __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiReg;

      size_t idx = 0;
      বাবে (list<সংরচনা enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    spec->names[idx].name = cur->text;
	    প্রতিজ্ঞা(cur->val4);
	    spec->names[idx].val4 = vector4_from_text(cur->val4, width);
	    spec->names[idx].is_signed = signed_flag;
	    মুক্ত(cur->val4);
	    cur->val4 = 0;
      }

      প্রতিজ্ঞা(idx == spec->names.size());
      compile_vpi_প্ৰতীক(label, spec);
      vpip_attach_to_current_scope(spec);

      মুক্ত(label);
      delete names;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য enum_delete(vpiHandle item)
{
      সংরচনা __vpiEnumTypespec*obj = dynamic_cast<__vpiEnumTypespec*>(item);

      বাবে (vector<enumconst_s>::iterator iter = obj->names.begin();
           iter != obj->names.end(); ++ iter ) {
	    delete [] iter->name;
      }

      delete obj;
}
#endif
