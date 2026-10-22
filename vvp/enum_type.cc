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

using ନାମାକାଶ std;

ସଂରଚନା enumconst_s : ସର୍ବସାଧାରଣ __vpiHandle {
      enumconst_s();
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଅକ୍ଷର* vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;

      ଅଚଳ ଅକ୍ଷର*name;
      ତାର୍କିକ is_signed;
      vvp_vector2_t val2;
      vvp_vector4_t val4;
};

ସଂରଚନା __vpiEnumTypespec : ସର୍ବସାଧାରଣ __vpiHandle {
      __vpiEnumTypespec();
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;

      std::vector<enumconst_s> names;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା base_type_code;
      ତାର୍କିକ is_signed;
};


ଧାଡ଼ିଭିତରେ __vpiEnumTypespec::__vpiEnumTypespec()
{ }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiEnumTypespec::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiEnumTypespec; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiEnumTypespec::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ names.size();

	    /* This is not currently set correctly. We always use vpiReg for
	     * four state variables and vpiBitVar for two state variables.
	     * This minimal functionality is needed to get the next() and
	     * prev() methods to work correctly with invalid values. */
	  ସ୍ଥିତି vpiBaseTypespec:
	    ଫେରନ୍ତୁ base_type_code;

	  ସ୍ଥିତି vpiSigned:
	    ଫେରନ୍ତୁ is_signed;

	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by __vpiEnumTypespec\n", code);
	    ପ୍ରତିଜ୍ଞା(0);
	    ଫେରନ୍ତୁ 0;
      }
}


vpiHandle __vpiEnumTypespec::vpi_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ଯଦି (code == vpiEnumConst) {
	    vpiHandle*args = static_cast<vpiHandle*>
	                                (calloc(names.size(), ଆକାର(vpiHandle*)));
	    ପାଇଁ (size_t idx = 0 ; idx < names.size() ; idx += 1)
		  args[idx] = &names[idx];

	    ଫେରନ୍ତୁ vpip_make_iterator(names.size(), args, ସତ୍ୟ);
      }

      ଫେରନ୍ତୁ 0;
}


ଧାଡ଼ିଭିତରେ enumconst_s::enumconst_s()
{ }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା enumconst_s::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiEnumConst; }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା enumconst_s::vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiSize:
	    ଫେରନ୍ତୁ val4.size()? val4.size() : val2.size();
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ 0;
      }
}


ଅକ୍ଷର* enumconst_s::vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
      ବାଛ (code) {
	  ସ୍ଥିତି vpiName:
	    ଫେରନ୍ତୁ const_cast<ଅକ୍ଷର*> (name);
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ 0;
      }
}


ଶୂନ୍ୟ enumconst_s::vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val)
{
      ଯଦି (val4.size() > 0) {
	    vpip_vec4_get_ମୂଲ୍ୟ(val4, val4.size(), is_signed, val);
      } ଅନ୍ୟଥା {
	    vpip_vec2_get_ମୂଲ୍ୟ(val2, val2.size(), is_signed, val);
      }
}


ଶୂନ୍ୟ compile_enum2_type(ଅକ୍ଷର*label, long width, ତାର୍କିକ signed_flag,
                        std::list<ସଂରଚନା enum_name_s>*names)
{
      ସଂରଚନା __vpiEnumTypespec*spec = ନୂତନ ସଂରଚନା __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiBitVar;

      size_t idx = 0;
      ପାଇଁ (list<ସଂରଚନା enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    ପ୍ରତିଜ୍ଞା(cur->val4 == 0);
	    spec->names[idx].name = cur->text;
	    spec->names[idx].val2 = vvp_vector2_t(cur->val2, width);
	    spec->names[idx].is_signed = signed_flag;
      }

      ପ୍ରତିଜ୍ଞା(idx == spec->names.size());
      compile_vpi_ପ୍ରତୀକ(label, spec);
      vpip_attach_to_current_scope(spec);

      ମୁକ୍ତ(label);
      delete names;
}

ଶୂନ୍ୟ compile_enum4_type(ଅକ୍ଷର*label, long width, ତାର୍କିକ signed_flag,
                        std::list<ସଂରଚନା enum_name_s>*names)
{
      ସଂରଚନା __vpiEnumTypespec*spec = ନୂତନ ସଂରଚନା __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiReg;

      size_t idx = 0;
      ପାଇଁ (list<ସଂରଚନା enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    spec->names[idx].name = cur->text;
	    ପ୍ରତିଜ୍ଞା(cur->val4);
	    spec->names[idx].val4 = vector4_from_text(cur->val4, width);
	    spec->names[idx].is_signed = signed_flag;
	    ମୁକ୍ତ(cur->val4);
	    cur->val4 = 0;
      }

      ପ୍ରତିଜ୍ଞା(idx == spec->names.size());
      compile_vpi_ପ୍ରତୀକ(label, spec);
      vpip_attach_to_current_scope(spec);

      ମୁକ୍ତ(label);
      delete names;
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ enum_delete(vpiHandle item)
{
      ସଂରଚନା __vpiEnumTypespec*obj = dynamic_cast<__vpiEnumTypespec*>(item);

      ପାଇଁ (vector<enumconst_s>::iterator iter = obj->names.begin();
           iter != obj->names.end(); ++ iter ) {
	    delete [] iter->name;
      }

      delete obj;
}
#endif
