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

using നാമാകാശ std;

ഘടന enumconst_s : പൊതു __vpiHandle {
      enumconst_s();
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      അക്ഷരം* vpi_get_str(പൂർണ്ണസംഖ്യ code) override;
      ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;

      സ്ഥിരം അക്ഷരം*name;
      തർക്കം is_signed;
      vvp_vector2_t val2;
      vvp_vector4_t val4;
};

ഘടന __vpiEnumTypespec : പൊതു __vpiHandle {
      __vpiEnumTypespec();
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override;
      പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_iterate(പൂർണ്ണസംഖ്യ code) override;

      std::vector<enumconst_s> names;
      പൂർണ്ണസംഖ്യ base_type_code;
      തർക്കം is_signed;
};


വരിയിൽ __vpiEnumTypespec::__vpiEnumTypespec()
{ }

പൂർണ്ണസംഖ്യ __vpiEnumTypespec::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiEnumTypespec; }

പൂർണ്ണസംഖ്യ __vpiEnumTypespec::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiSize:
	    തിരികെ names.size();

	    /* This is not currently set correctly. We always use vpiReg for
	     * four state variables and vpiBitVar for two state variables.
	     * This minimal functionality is needed to get the next() and
	     * prev() methods to work correctly with invalid values. */
	  അവസ്ഥ vpiBaseTypespec:
	    തിരികെ base_type_code;

	  അവസ്ഥ vpiSigned:
	    തിരികെ is_signed;

	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by __vpiEnumTypespec\n", code);
	    പ്രതിജ്ഞാ(0);
	    തിരികെ 0;
      }
}


vpiHandle __vpiEnumTypespec::vpi_iterate(പൂർണ്ണസംഖ്യ code)
{
      എങ്കിൽ (code == vpiEnumConst) {
	    vpiHandle*args = static_cast<vpiHandle*>
	                                (calloc(names.size(), വലുപ്പം(vpiHandle*)));
	    വേണ്ടി (size_t idx = 0 ; idx < names.size() ; idx += 1)
		  args[idx] = &names[idx];

	    തിരികെ vpip_make_iterator(names.size(), args, സത്യം);
      }

      തിരികെ 0;
}


വരിയിൽ enumconst_s::enumconst_s()
{ }

പൂർണ്ണസംഖ്യ enumconst_s::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiEnumConst; }

പൂർണ്ണസംഖ്യ enumconst_s::vpi_get(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiSize:
	    തിരികെ val4.size()? val4.size() : val2.size();
	  സ്ഥിരസ്ഥിതി:
	    തിരികെ 0;
      }
}


അക്ഷരം* enumconst_s::vpi_get_str(പൂർണ്ണസംഖ്യ code)
{
      തിരഞ്ഞെടുപ്പ് (code) {
	  അവസ്ഥ vpiName:
	    തിരികെ const_cast<അക്ഷരം*> (name);
	  സ്ഥിരസ്ഥിതി:
	    തിരികെ 0;
      }
}


ശൂന്യം enumconst_s::vpi_get_മൂല്യം(p_vpi_മൂല്യം val)
{
      എങ്കിൽ (val4.size() > 0) {
	    vpip_vec4_get_മൂല്യം(val4, val4.size(), is_signed, val);
      } അല്ലെങ്കിൽ {
	    vpip_vec2_get_മൂല്യം(val2, val2.size(), is_signed, val);
      }
}


ശൂന്യം compile_enum2_type(അക്ഷരം*label, long width, തർക്കം signed_flag,
                        std::list<ഘടന enum_name_s>*names)
{
      ഘടന __vpiEnumTypespec*spec = പുതിയ ഘടന __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiBitVar;

      size_t idx = 0;
      വേണ്ടി (list<ഘടന enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    പ്രതിജ്ഞാ(cur->val4 == 0);
	    spec->names[idx].name = cur->text;
	    spec->names[idx].val2 = vvp_vector2_t(cur->val2, width);
	    spec->names[idx].is_signed = signed_flag;
      }

      പ്രതിജ്ഞാ(idx == spec->names.size());
      compile_vpi_ചിഹ്നം(label, spec);
      vpip_attach_to_current_scope(spec);

      സ്വതന്ത്രം(label);
      delete names;
}

ശൂന്യം compile_enum4_type(അക്ഷരം*label, long width, തർക്കം signed_flag,
                        std::list<ഘടന enum_name_s>*names)
{
      ഘടന __vpiEnumTypespec*spec = പുതിയ ഘടന __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiReg;

      size_t idx = 0;
      വേണ്ടി (list<ഘടന enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    spec->names[idx].name = cur->text;
	    പ്രതിജ്ഞാ(cur->val4);
	    spec->names[idx].val4 = vector4_from_text(cur->val4, width);
	    spec->names[idx].is_signed = signed_flag;
	    സ്വതന്ത്രം(cur->val4);
	    cur->val4 = 0;
      }

      പ്രതിജ്ഞാ(idx == spec->names.size());
      compile_vpi_ചിഹ്നം(label, spec);
      vpip_attach_to_current_scope(spec);

      സ്വതന്ത്രം(label);
      delete names;
}

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം enum_delete(vpiHandle item)
{
      ഘടന __vpiEnumTypespec*obj = dynamic_cast<__vpiEnumTypespec*>(item);

      വേണ്ടി (vector<enumconst_s>::iterator iter = obj->names.begin();
           iter != obj->names.end(); ++ iter ) {
	    delete [] iter->name;
      }

      delete obj;
}
#endif
