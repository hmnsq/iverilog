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

using namespace std;

struct enumconst_s : public __vpiHandle {
      enumconst_s();
      số_nguyên get_type_code(void) const override;
      số_nguyên vpi_get(số_nguyên code) override;
      char* vpi_get_str(số_nguyên code) override;
      void vpi_get_value(p_vpi_value val) override;

      const char*name;
      bool is_signed;
      vvp_vector2_t val2;
      vvp_vector4_t val4;
};

struct __vpiEnumTypespec : public __vpiHandle {
      __vpiEnumTypespec();
      số_nguyên get_type_code(void) const override;
      số_nguyên vpi_get(số_nguyên code) override;
      vpiHandle vpi_iterate(số_nguyên code) override;

      std::vector<enumconst_s> names;
      số_nguyên base_type_code;
      bool is_signed;
};


inline __vpiEnumTypespec::__vpiEnumTypespec()
{ }

số_nguyên __vpiEnumTypespec::get_type_code(void) const
{ trả_về vpiEnumTypespec; }

số_nguyên __vpiEnumTypespec::vpi_get(số_nguyên code)
{
      switch (code) {
	  case vpiSize:
	    trả_về names.size();

	    /* This is not currently set correctly. We always use vpiReg for
	     * four state variables and vpiBitVar for two state variables.
	     * This minimal functionality is needed to get the next() and
	     * prev() methods to work correctly with invalid values. */
	  case vpiBaseTypespec:
	    trả_về base_type_code;

	  case vpiSigned:
	    trả_về is_signed;

	  default:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by __vpiEnumTypespec\n", code);
	    assert(0);
	    trả_về 0;
      }
}


vpiHandle __vpiEnumTypespec::vpi_iterate(số_nguyên code)
{
      nếu (code == vpiEnumConst) {
	    vpiHandle*args = static_cast<vpiHandle*>
	                                (calloc(names.size(), sizeof(vpiHandle*)));
	    cho (size_t idx = 0 ; idx < names.size() ; idx += 1)
		  args[idx] = &names[idx];

	    trả_về vpip_make_iterator(names.size(), args, true);
      }

      trả_về 0;
}


inline enumconst_s::enumconst_s()
{ }

số_nguyên enumconst_s::get_type_code(void) const
{ trả_về vpiEnumConst; }

số_nguyên enumconst_s::vpi_get(số_nguyên code)
{
      switch (code) {
	  case vpiSize:
	    trả_về val4.size()? val4.size() : val2.size();
	  default:
	    trả_về 0;
      }
}


char* enumconst_s::vpi_get_str(số_nguyên code)
{
      switch (code) {
	  case vpiName:
	    trả_về const_cast<char*> (name);
	  default:
	    trả_về 0;
      }
}


void enumconst_s::vpi_get_value(p_vpi_value val)
{
      nếu (val4.size() > 0) {
	    vpip_vec4_get_value(val4, val4.size(), is_signed, val);
      } không_thì {
	    vpip_vec2_get_value(val2, val2.size(), is_signed, val);
      }
}


void compile_enum2_type(char*label, long width, bool signed_flag,
                        std::list<struct enum_name_s>*names)
{
      struct __vpiEnumTypespec*spec = new struct __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiBitVar;

      size_t idx = 0;
      cho (list<struct enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    assert(cur->val4 == 0);
	    spec->names[idx].name = cur->text;
	    spec->names[idx].val2 = vvp_vector2_t(cur->val2, width);
	    spec->names[idx].is_signed = signed_flag;
      }

      assert(idx == spec->names.size());
      compile_vpi_symbol(label, spec);
      vpip_attach_to_current_scope(spec);

      free(label);
      delete names;
}

void compile_enum4_type(char*label, long width, bool signed_flag,
                        std::list<struct enum_name_s>*names)
{
      struct __vpiEnumTypespec*spec = new struct __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiReg;

      size_t idx = 0;
      cho (list<struct enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    spec->names[idx].name = cur->text;
	    assert(cur->val4);
	    spec->names[idx].val4 = vector4_from_text(cur->val4, width);
	    spec->names[idx].is_signed = signed_flag;
	    free(cur->val4);
	    cur->val4 = 0;
      }

      assert(idx == spec->names.size());
      compile_vpi_symbol(label, spec);
      vpip_attach_to_current_scope(spec);

      free(label);
      delete names;
}

#ifdef CHECK_WITH_VALGRIND
void enum_delete(vpiHandle item)
{
      struct __vpiEnumTypespec*obj = dynamic_cast<__vpiEnumTypespec*>(item);

      cho (vector<enumconst_s>::iterator iter = obj->names.begin();
           iter != obj->names.end(); ++ iter ) {
	    delete [] iter->name;
      }

      delete obj;
}
#endif
