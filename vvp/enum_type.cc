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

sa_nracanaa enumconst_s : public __vpiHandle {
      enumconst_s();
      ចំនួនគត់ get_type_code(void) const override;
      ចំនួនគត់ vpi_get(ចំនួនគត់ code) override;
      var_na* vpi_get_str(ចំនួនគត់ code) override;
      void vpi_get_value(p_vpi_value val) override;

      const var_na*name;
      bool is_signed;
      vvp_vector2_t val2;
      vvp_vector4_t val4;
};

sa_nracanaa __vpiEnumTypespec : public __vpiHandle {
      __vpiEnumTypespec();
      ចំនួនគត់ get_type_code(void) const override;
      ចំនួនគត់ vpi_get(ចំនួនគត់ code) override;
      vpiHandle vpi_iterate(ចំនួនគត់ code) override;

      std::vector<enumconst_s> names;
      ចំនួនគត់ base_type_code;
      bool is_signed;
};


anta_hstha __vpiEnumTypespec::__vpiEnumTypespec()
{ }

ចំនួនគត់ __vpiEnumTypespec::get_type_code(void) const
{ ត្រឡប់ vpiEnumTypespec; }

ចំនួនគត់ __vpiEnumTypespec::vpi_get(ចំនួនគត់ code)
{
      switch (code) {
	  pak_sa vpiSize:
	    ត្រឡប់ names.size();

	    /* This is not currently set correctly. We always use vpiReg for
	     * four state variables and vpiBitVar for two state variables.
	     * This minimal functionality is needed to get the next() and
	     * prev() methods to work correctly with invalid values. */
	  pak_sa vpiBaseTypespec:
	    ត្រឡប់ base_type_code;

	  pak_sa vpiSigned:
	    ត្រឡប់ is_signed;

	  default:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by __vpiEnumTypespec\n", code);
	    assert(0);
	    ត្រឡប់ 0;
      }
}


vpiHandle __vpiEnumTypespec::vpi_iterate(ចំនួនគត់ code)
{
      ប្រសិនបើ (code == vpiEnumConst) {
	    vpiHandle*args = static_cast<vpiHandle*>
	                                (calloc(names.size(), parimaa_na(vpiHandle*)));
	    សម្រាប់ (size_t idx = 0 ; idx < names.size() ; idx += 1)
		  args[idx] = &names[idx];

	    ត្រឡប់ vpip_make_iterator(names.size(), args, true);
      }

      ត្រឡប់ 0;
}


anta_hstha enumconst_s::enumconst_s()
{ }

ចំនួនគត់ enumconst_s::get_type_code(void) const
{ ត្រឡប់ vpiEnumConst; }

ចំនួនគត់ enumconst_s::vpi_get(ចំនួនគត់ code)
{
      switch (code) {
	  pak_sa vpiSize:
	    ត្រឡប់ val4.size()? val4.size() : val2.size();
	  default:
	    ត្រឡប់ 0;
      }
}


var_na* enumconst_s::vpi_get_str(ចំនួនគត់ code)
{
      switch (code) {
	  pak_sa vpiName:
	    ត្រឡប់ const_cast<var_na*> (name);
	  default:
	    ត្រឡប់ 0;
      }
}


void enumconst_s::vpi_get_value(p_vpi_value val)
{
      ប្រសិនបើ (val4.size() > 0) {
	    vpip_vec4_get_value(val4, val4.size(), is_signed, val);
      } បើមិនដូច្នេះ {
	    vpip_vec2_get_value(val2, val2.size(), is_signed, val);
      }
}


void compile_enum2_type(var_na*label, long width, bool signed_flag,
                        std::list<sa_nracanaa enum_name_s>*names)
{
      sa_nracanaa __vpiEnumTypespec*spec = new sa_nracanaa __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiBitVar;

      size_t idx = 0;
      សម្រាប់ (list<sa_nracanaa enum_name_s>::iterator cur = names->begin()
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

void compile_enum4_type(var_na*label, long width, bool signed_flag,
                        std::list<sa_nracanaa enum_name_s>*names)
{
      sa_nracanaa __vpiEnumTypespec*spec = new sa_nracanaa __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiReg;

      size_t idx = 0;
      សម្រាប់ (list<sa_nracanaa enum_name_s>::iterator cur = names->begin()
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
      sa_nracanaa __vpiEnumTypespec*obj = dynamic_cast<__vpiEnumTypespec*>(item);

      សម្រាប់ (vector<enumconst_s>::iterator iter = obj->names.begin();
           iter != obj->names.end(); ++ iter ) {
	    delete [] iter->name;
      }

      delete obj;
}
#endif
