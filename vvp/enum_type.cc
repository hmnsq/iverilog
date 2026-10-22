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

using नामाकाश std;

संरचना enumconst_s : सार्वजनिक __vpiHandle {
      enumconst_s();
      पूर्णांक get_type_code(शून्य) स्थिरांक override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर* vpi_get_str(पूर्णांक code) override;
      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;

      स्थिरांक अक्षर*name;
      तार्किक is_signed;
      vvp_vector2_t val2;
      vvp_vector4_t val4;
};

संरचना __vpiEnumTypespec : सार्वजनिक __vpiHandle {
      __vpiEnumTypespec();
      पूर्णांक get_type_code(शून्य) स्थिरांक override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      vpiHandle vpi_iterate(पूर्णांक code) override;

      std::vector<enumconst_s> names;
      पूर्णांक base_type_code;
      तार्किक is_signed;
};


ओळीत __vpiEnumTypespec::__vpiEnumTypespec()
{ }

पूर्णांक __vpiEnumTypespec::get_type_code(शून्य) स्थिरांक
{ परत vpiEnumTypespec; }

पूर्णांक __vpiEnumTypespec::vpi_get(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpiSize:
	    परत names.size();

	    /* This is not currently set correctly. We always use vpiReg for
	     * four state variables and vpiBitVar for two state variables.
	     * This minimal functionality is needed to get the next() and
	     * prev() methods to work correctly with invalid values. */
	  स्थिती vpiBaseTypespec:
	    परत base_type_code;

	  स्थिती vpiSigned:
	    परत is_signed;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "vvp error: get %d not supported "
		    "by __vpiEnumTypespec\n", code);
	    प्रतिज्ञा(0);
	    परत 0;
      }
}


vpiHandle __vpiEnumTypespec::vpi_iterate(पूर्णांक code)
{
      जर (code == vpiEnumConst) {
	    vpiHandle*args = static_cast<vpiHandle*>
	                                (सुस्मृति(names.size(), आकार(vpiHandle*)));
	    साठी (माप_प्रकार idx = 0 ; idx < names.size() ; idx += 1)
		  args[idx] = &names[idx];

	    परत vpip_make_iterator(names.size(), args, सत्य);
      }

      परत 0;
}


ओळीत enumconst_s::enumconst_s()
{ }

पूर्णांक enumconst_s::get_type_code(शून्य) स्थिरांक
{ परत vpiEnumConst; }

पूर्णांक enumconst_s::vpi_get(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpiSize:
	    परत val4.size()? val4.size() : val2.size();
	  पूर्वनिर्धारित:
	    परत 0;
      }
}


अक्षर* enumconst_s::vpi_get_str(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpiName:
	    परत const_cast<अक्षर*> (name);
	  पूर्वनिर्धारित:
	    परत 0;
      }
}


शून्य enumconst_s::vpi_get_मूल्य(p_vpi_मूल्य val)
{
      जर (val4.size() > 0) {
	    vpip_vec4_get_मूल्य(val4, val4.size(), is_signed, val);
      } अन्यथा {
	    vpip_vec2_get_मूल्य(val2, val2.size(), is_signed, val);
      }
}


शून्य compile_enum2_type(अक्षर*label, दीर्घ width, तार्किक signed_flag,
                        std::list<संरचना enum_name_s>*names)
{
      संरचना __vpiEnumTypespec*spec = नवीन संरचना __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiBitVar;

      माप_प्रकार idx = 0;
      साठी (list<संरचना enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    प्रतिज्ञा(cur->val4 == 0);
	    spec->names[idx].name = cur->text;
	    spec->names[idx].val2 = vvp_vector2_t(cur->val2, width);
	    spec->names[idx].is_signed = signed_flag;
      }

      प्रतिज्ञा(idx == spec->names.size());
      compile_vpi_चिन्ह(label, spec);
      vpip_attach_to_current_scope(spec);

      मुक्त(label);
      delete names;
}

शून्य compile_enum4_type(अक्षर*label, दीर्घ width, तार्किक signed_flag,
                        std::list<संरचना enum_name_s>*names)
{
      संरचना __vpiEnumTypespec*spec = नवीन संरचना __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiReg;

      माप_प्रकार idx = 0;
      साठी (list<संरचना enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    spec->names[idx].name = cur->text;
	    प्रतिज्ञा(cur->val4);
	    spec->names[idx].val4 = vector4_from_text(cur->val4, width);
	    spec->names[idx].is_signed = signed_flag;
	    मुक्त(cur->val4);
	    cur->val4 = 0;
      }

      प्रतिज्ञा(idx == spec->names.size());
      compile_vpi_चिन्ह(label, spec);
      vpip_attach_to_current_scope(spec);

      मुक्त(label);
      delete names;
}

#ifdef CHECK_WITH_VALGRIND
शून्य enum_delete(vpiHandle item)
{
      संरचना __vpiEnumTypespec*obj = dynamic_cast<__vpiEnumTypespec*>(item);

      साठी (vector<enumconst_s>::iterator iter = obj->names.begin();
           iter != obj->names.end(); ++ iter ) {
	    delete [] iter->name;
      }

      delete obj;
}
#endif
