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
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      वर्ण* vpi_get_str(पूर्णाङ्क code) override;
      शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;

      ध्रुव वर्ण*name;
      तार्किक is_signed;
      vvp_vector2_t val2;
      vvp_vector4_t val4;
};

संरचना __vpiEnumTypespec : सार्वजनिक __vpiHandle {
      __vpiEnumTypespec();
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;
      पूर्णाङ्क vpi_get(पूर्णाङ्क code) override;
      vpiHandle vpi_iterate(पूर्णाङ्क code) override;

      std::vector<enumconst_s> names;
      पूर्णाङ्क base_type_code;
      तार्किक is_signed;
};


अन्तःस्थ __vpiEnumTypespec::__vpiEnumTypespec()
{ }

पूर्णाङ्क __vpiEnumTypespec::get_type_code(शून्य) ध्रुव
{ प्रत्यागम vpiEnumTypespec; }

पूर्णाङ्क __vpiEnumTypespec::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiSize:
	    प्रत्यागम names.size();

	    /* This is not currently set correctly. We always use vpiReg for
	     * four state variables and vpiBitVar for two state variables.
	     * This minimal functionality is needed to get the next() and
	     * prev() methods to work correctly with invalid values. */
	  पक्ष vpiBaseTypespec:
	    प्रत्यागम base_type_code;

	  पक्ष vpiSigned:
	    प्रत्यागम is_signed;

	  स्वभाव:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by __vpiEnumTypespec\n", code);
	    प्रतिज्ञा(0);
	    प्रत्यागम 0;
      }
}


vpiHandle __vpiEnumTypespec::vpi_iterate(पूर्णाङ्क code)
{
      यदि (code == vpiEnumConst) {
	    vpiHandle*args = static_cast<vpiHandle*>
	                                (calloc(names.size(), परिमाण(vpiHandle*)));
	    प्रति (size_t idx = 0 ; idx < names.size() ; idx += 1)
		  args[idx] = &names[idx];

	    प्रत्यागम vpip_make_iterator(names.size(), args, सत्य);
      }

      प्रत्यागम 0;
}


अन्तःस्थ enumconst_s::enumconst_s()
{ }

पूर्णाङ्क enumconst_s::get_type_code(शून्य) ध्रुव
{ प्रत्यागम vpiEnumConst; }

पूर्णाङ्क enumconst_s::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiSize:
	    प्रत्यागम val4.size()? val4.size() : val2.size();
	  स्वभाव:
	    प्रत्यागम 0;
      }
}


वर्ण* enumconst_s::vpi_get_str(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiName:
	    प्रत्यागम const_cast<वर्ण*> (name);
	  स्वभाव:
	    प्रत्यागम 0;
      }
}


शून्य enumconst_s::vpi_get_मूल्य(p_vpi_मूल्य val)
{
      यदि (val4.size() > 0) {
	    vpip_vec4_get_मूल्य(val4, val4.size(), is_signed, val);
      } अन्यथा {
	    vpip_vec2_get_मूल्य(val2, val2.size(), is_signed, val);
      }
}


शून्य compile_enum2_type(वर्ण*label, long width, तार्किक signed_flag,
                        std::list<संरचना enum_name_s>*names)
{
      संरचना __vpiEnumTypespec*spec = नूतन संरचना __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiBitVar;

      size_t idx = 0;
      प्रति (list<संरचना enum_name_s>::iterator cur = names->begin()
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

शून्य compile_enum4_type(वर्ण*label, long width, तार्किक signed_flag,
                        std::list<संरचना enum_name_s>*names)
{
      संरचना __vpiEnumTypespec*spec = नूतन संरचना __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiReg;

      size_t idx = 0;
      प्रति (list<संरचना enum_name_s>::iterator cur = names->begin()
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

      प्रति (vector<enumconst_s>::iterator iter = obj->names.begin();
           iter != obj->names.end(); ++ iter ) {
	    delete [] iter->name;
      }

      delete obj;
}
#endif
