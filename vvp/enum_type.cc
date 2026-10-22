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
#अगर_घोषित CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#पूर्ण_अगर
# include  <iostream>
# include  <cassert>

using namespace std;

काष्ठा enumconst_s : खुला __vpiHandle {
      enumconst_s();
      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      अक्षर* vpi_get_str(पूर्णांक code) override;
      व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;

      स्थिर अक्षर*name;
      bool is_signed;
      vvp_vector2_t val2;
      vvp_vector4_t val4;
};

काष्ठा __vpiEnumTypespec : खुला __vpiHandle {
      __vpiEnumTypespec();
      पूर्णांक get_type_code(व्योम) स्थिर override;
      पूर्णांक vpi_get(पूर्णांक code) override;
      vpiHandle vpi_iterate(पूर्णांक code) override;

      std::vector<enumconst_s> names;
      पूर्णांक base_type_code;
      bool is_signed;
};


अंतरभूत __vpiEnumTypespec::__vpiEnumTypespec()
{ }

पूर्णांक __vpiEnumTypespec::get_type_code(व्योम) स्थिर
{ वापस vpiEnumTypespec; }

पूर्णांक __vpiEnumTypespec::vpi_get(पूर्णांक code)
{
      चयन (code) {
	  हाल vpi_आकार:
	    वापस names.size();

	    /* This is not currently set correctly. We always use vpiReg for
	     * four state variables and vpiBitVar for two state variables.
	     * This minimal functionality is needed to get the next() and
	     * prev() methods to work correctly with invalid values. */
	  हाल vpiBaseTypespec:
	    वापस base_type_code;

	  हाल vpiSigned:
	    वापस is_signed;

	  शेष:
	    ख_लिखो(मानक_त्रुटि, "vvp error: get %d not supported "
		    "by __vpiEnumTypespec\n", code);
	    निश्चित(0);
	    वापस 0;
      }
}


vpiHandle __vpiEnumTypespec::vpi_iterate(पूर्णांक code)
{
      अगर (code == vpiEnumConst) {
	    vpiHandle*args = static_cast<vpiHandle*>
	                                (सुस्मृति(names.size(), माप(vpiHandle*)));
	    क्रम (माप_प्रकार idx = 0 ; idx < names.size() ; idx += 1)
		  args[idx] = &names[idx];

	    वापस vpip_make_iterator(names.size(), args, true);
      }

      वापस 0;
}


अंतरभूत enumconst_s::enumconst_s()
{ }

पूर्णांक enumconst_s::get_type_code(व्योम) स्थिर
{ वापस vpiEnumConst; }

पूर्णांक enumconst_s::vpi_get(पूर्णांक code)
{
      चयन (code) {
	  हाल vpi_आकार:
	    वापस val4.size()? val4.size() : val2.size();
	  शेष:
	    वापस 0;
      }
}


अक्षर* enumconst_s::vpi_get_str(पूर्णांक code)
{
      चयन (code) {
	  हाल vpiName:
	    वापस const_cast<अक्षर*> (name);
	  शेष:
	    वापस 0;
      }
}


व्योम enumconst_s::vpi_get_मूल्य(p_vpi_मूल्य val)
{
      अगर (val4.size() > 0) {
	    vpip_vec4_get_मूल्य(val4, val4.size(), is_signed, val);
      } अन्यथा {
	    vpip_vec2_get_मूल्य(val2, val2.size(), is_signed, val);
      }
}


व्योम compile_enum2_type(अक्षर*label, दीर्घ width, bool signed_flag,
                        std::list<काष्ठा enum_name_s>*names)
{
      काष्ठा __vpiEnumTypespec*spec = नया काष्ठा __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiBitVar;

      माप_प्रकार idx = 0;
      क्रम (list<काष्ठा enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    निश्चित(cur->val4 == 0);
	    spec->names[idx].name = cur->text;
	    spec->names[idx].val2 = vvp_vector2_t(cur->val2, width);
	    spec->names[idx].is_signed = signed_flag;
      }

      निश्चित(idx == spec->names.size());
      compile_vpi_प्रतीक(label, spec);
      vpip_attach_to_current_scope(spec);

      मुक्त(label);
      मिटाओ names;
}

व्योम compile_enum4_type(अक्षर*label, दीर्घ width, bool signed_flag,
                        std::list<काष्ठा enum_name_s>*names)
{
      काष्ठा __vpiEnumTypespec*spec = नया काष्ठा __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiReg;

      माप_प्रकार idx = 0;
      क्रम (list<काष्ठा enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    spec->names[idx].name = cur->text;
	    निश्चित(cur->val4);
	    spec->names[idx].val4 = vector4_from_text(cur->val4, width);
	    spec->names[idx].is_signed = signed_flag;
	    मुक्त(cur->val4);
	    cur->val4 = 0;
      }

      निश्चित(idx == spec->names.size());
      compile_vpi_प्रतीक(label, spec);
      vpip_attach_to_current_scope(spec);

      मुक्त(label);
      मिटाओ names;
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम enum_delete(vpiHandle item)
{
      काष्ठा __vpiEnumTypespec*obj = dynamic_cast<__vpiEnumTypespec*>(item);

      क्रम (vector<enumconst_s>::iterator iter = obj->names.begin();
           iter != obj->names.end(); ++ iter ) {
	    मिटाओ [] iter->name;
      }

      मिटाओ obj;
}
#पूर्ण_अगर
