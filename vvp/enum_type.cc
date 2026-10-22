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

কাঠামো enumconst_s : সর্বজনীন __vpiHandle {
      enumconst_s();
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override;
      পূর্ণসংখ্যা vpi_get(পূর্ণসংখ্যা code) override;
      অক্ষর* vpi_get_str(পূর্ণসংখ্যা code) override;
      শূন্য vpi_get_মান(p_vpi_মান val) override;

      ধ্রুবক অক্ষর*name;
      যৌক্তিক is_signed;
      vvp_vector2_t val2;
      vvp_vector4_t val4;
};

কাঠামো __vpiEnumTypespec : সর্বজনীন __vpiHandle {
      __vpiEnumTypespec();
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override;
      পূর্ণসংখ্যা vpi_get(পূর্ণসংখ্যা code) override;
      vpiHandle vpi_iterate(পূর্ণসংখ্যা code) override;

      std::vector<enumconst_s> names;
      পূর্ণসংখ্যা base_type_code;
      যৌক্তিক is_signed;
};


অন্তঃরেখ __vpiEnumTypespec::__vpiEnumTypespec()
{ }

পূর্ণসংখ্যা __vpiEnumTypespec::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiEnumTypespec; }

পূর্ণসংখ্যা __vpiEnumTypespec::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpi_আকার:
	    ফেরত names.size();

	    /* This is not currently set correctly. We always use vpiReg for
	     * four state variables and vpiBitVar for two state variables.
	     * This minimal functionality is needed to get the next() and
	     * prev() methods to work correctly with invalid values. */
	  ক্ষেত্র vpiBaseTypespec:
	    ফেরত base_type_code;

	  ক্ষেত্র vpiSigned:
	    ফেরত is_signed;

	  পূর্বনির্ধারিত:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by __vpiEnumTypespec\n", code);
	    প্রতিজ্ঞা(0);
	    ফেরত 0;
      }
}


vpiHandle __vpiEnumTypespec::vpi_iterate(পূর্ণসংখ্যা code)
{
      যদি (code == vpiEnumConst) {
	    vpiHandle*args = static_cast<vpiHandle*>
	                                (calloc(names.size(), আকার(vpiHandle*)));
	    জন্য (size_t idx = 0 ; idx < names.size() ; idx += 1)
		  args[idx] = &names[idx];

	    ফেরত vpip_make_iterator(names.size(), args, সত্য);
      }

      ফেরত 0;
}


অন্তঃরেখ enumconst_s::enumconst_s()
{ }

পূর্ণসংখ্যা enumconst_s::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiEnumConst; }

পূর্ণসংখ্যা enumconst_s::vpi_get(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpi_আকার:
	    ফেরত val4.size()? val4.size() : val2.size();
	  পূর্বনির্ধারিত:
	    ফেরত 0;
      }
}


অক্ষর* enumconst_s::vpi_get_str(পূর্ণসংখ্যা code)
{
      নির্বাচন (code) {
	  ক্ষেত্র vpiName:
	    ফেরত const_cast<অক্ষর*> (name);
	  পূর্বনির্ধারিত:
	    ফেরত 0;
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


শূন্য compile_enum2_type(অক্ষর*label, long width, যৌক্তিক signed_flag,
                        std::list<কাঠামো enum_name_s>*names)
{
      কাঠামো __vpiEnumTypespec*spec = নতুন কাঠামো __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiBitVar;

      size_t idx = 0;
      জন্য (list<কাঠামো enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    প্রতিজ্ঞা(cur->val4 == 0);
	    spec->names[idx].name = cur->text;
	    spec->names[idx].val2 = vvp_vector2_t(cur->val2, width);
	    spec->names[idx].is_signed = signed_flag;
      }

      প্রতিজ্ঞা(idx == spec->names.size());
      compile_vpi_প্রতীক(label, spec);
      vpip_attach_to_current_scope(spec);

      মুক্ত(label);
      delete names;
}

শূন্য compile_enum4_type(অক্ষর*label, long width, যৌক্তিক signed_flag,
                        std::list<কাঠামো enum_name_s>*names)
{
      কাঠামো __vpiEnumTypespec*spec = নতুন কাঠামো __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiReg;

      size_t idx = 0;
      জন্য (list<কাঠামো enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    spec->names[idx].name = cur->text;
	    প্রতিজ্ঞা(cur->val4);
	    spec->names[idx].val4 = vector4_from_text(cur->val4, width);
	    spec->names[idx].is_signed = signed_flag;
	    মুক্ত(cur->val4);
	    cur->val4 = 0;
      }

      প্রতিজ্ঞা(idx == spec->names.size());
      compile_vpi_প্রতীক(label, spec);
      vpip_attach_to_current_scope(spec);

      মুক্ত(label);
      delete names;
}

#ifdef CHECK_WITH_VALGRIND
শূন্য enum_delete(vpiHandle item)
{
      কাঠামো __vpiEnumTypespec*obj = dynamic_cast<__vpiEnumTypespec*>(item);

      জন্য (vector<enumconst_s>::iterator iter = obj->names.begin();
           iter != obj->names.end(); ++ iter ) {
	    delete [] iter->name;
      }

      delete obj;
}
#endif
