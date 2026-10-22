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

using நாமாகாஶ std;

கட்டமைப்பு enumconst_s : பொது __vpiHandle {
      enumconst_s();
      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      எழுத்து* vpi_get_str(முழு code) override;
      வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;

      மாறிலி எழுத்து*name;
      தருக்க is_signed;
      vvp_vector2_t val2;
      vvp_vector4_t val4;
};

கட்டமைப்பு __vpiEnumTypespec : பொது __vpiHandle {
      __vpiEnumTypespec();
      முழு get_type_code(வெறுமை) மாறிலி override;
      முழு vpi_get(முழு code) override;
      vpiHandle vpi_iterate(முழு code) override;

      std::vector<enumconst_s> names;
      முழு base_type_code;
      தருக்க is_signed;
};


உள்வரி __vpiEnumTypespec::__vpiEnumTypespec()
{ }

முழு __vpiEnumTypespec::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiEnumTypespec; }

முழு __vpiEnumTypespec::vpi_get(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiSize:
	    திரும்பு names.size();

	    /* This is not currently set correctly. We always use vpiReg for
	     * four state variables and vpiBitVar for two state variables.
	     * This minimal functionality is needed to get the next() and
	     * prev() methods to work correctly with invalid values. */
	  நிலை vpiBaseTypespec:
	    திரும்பு base_type_code;

	  நிலை vpiSigned:
	    திரும்பு is_signed;

	  இயல்பு:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by __vpiEnumTypespec\n", code);
	    ப்ரதிஜ்ஞா(0);
	    திரும்பு 0;
      }
}


vpiHandle __vpiEnumTypespec::vpi_iterate(முழு code)
{
      எனில் (code == vpiEnumConst) {
	    vpiHandle*args = static_cast<vpiHandle*>
	                                (calloc(names.size(), அளவு(vpiHandle*)));
	    க்கு (size_t idx = 0 ; idx < names.size() ; idx += 1)
		  args[idx] = &names[idx];

	    திரும்பு vpip_make_iterator(names.size(), args, உண்மை);
      }

      திரும்பு 0;
}


உள்வரி enumconst_s::enumconst_s()
{ }

முழு enumconst_s::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiEnumConst; }

முழு enumconst_s::vpi_get(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiSize:
	    திரும்பு val4.size()? val4.size() : val2.size();
	  இயல்பு:
	    திரும்பு 0;
      }
}


எழுத்து* enumconst_s::vpi_get_str(முழு code)
{
      தேர்வு (code) {
	  நிலை vpiName:
	    திரும்பு const_cast<எழுத்து*> (name);
	  இயல்பு:
	    திரும்பு 0;
      }
}


வெறுமை enumconst_s::vpi_get_மதிப்பு(p_vpi_மதிப்பு val)
{
      எனில் (val4.size() > 0) {
	    vpip_vec4_get_மதிப்பு(val4, val4.size(), is_signed, val);
      } இல்லையெனில் {
	    vpip_vec2_get_மதிப்பு(val2, val2.size(), is_signed, val);
      }
}


வெறுமை compile_enum2_type(எழுத்து*label, long width, தருக்க signed_flag,
                        std::list<கட்டமைப்பு enum_name_s>*names)
{
      கட்டமைப்பு __vpiEnumTypespec*spec = புதிய கட்டமைப்பு __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiBitVar;

      size_t idx = 0;
      க்கு (list<கட்டமைப்பு enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    ப்ரதிஜ்ஞா(cur->val4 == 0);
	    spec->names[idx].name = cur->text;
	    spec->names[idx].val2 = vvp_vector2_t(cur->val2, width);
	    spec->names[idx].is_signed = signed_flag;
      }

      ப்ரதிஜ்ஞா(idx == spec->names.size());
      compile_vpi_குறியீடு(label, spec);
      vpip_attach_to_current_scope(spec);

      விடுவி(label);
      delete names;
}

வெறுமை compile_enum4_type(எழுத்து*label, long width, தருக்க signed_flag,
                        std::list<கட்டமைப்பு enum_name_s>*names)
{
      கட்டமைப்பு __vpiEnumTypespec*spec = புதிய கட்டமைப்பு __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiReg;

      size_t idx = 0;
      க்கு (list<கட்டமைப்பு enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    spec->names[idx].name = cur->text;
	    ப்ரதிஜ்ஞா(cur->val4);
	    spec->names[idx].val4 = vector4_from_text(cur->val4, width);
	    spec->names[idx].is_signed = signed_flag;
	    விடுவி(cur->val4);
	    cur->val4 = 0;
      }

      ப்ரதிஜ்ஞா(idx == spec->names.size());
      compile_vpi_குறியீடு(label, spec);
      vpip_attach_to_current_scope(spec);

      விடுவி(label);
      delete names;
}

#ifdef CHECK_WITH_VALGRIND
வெறுமை enum_delete(vpiHandle item)
{
      கட்டமைப்பு __vpiEnumTypespec*obj = dynamic_cast<__vpiEnumTypespec*>(item);

      க்கு (vector<enumconst_s>::iterator iter = obj->names.begin();
           iter != obj->names.end(); ++ iter ) {
	    delete [] iter->name;
      }

      delete obj;
}
#endif
