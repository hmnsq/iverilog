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

using ನಾಮಾಕಾಶ std;

ಸಂರಚನಾ enumconst_s : ಸಾರ್ವಜನಿಕ __vpiHandle {
      enumconst_s();
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      ವರ್ಣ* vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;

      ಧ್ರುವ ವರ್ಣ*name;
      ತಾರ್ಕಿಕ is_signed;
      vvp_vector2_t val2;
      vvp_vector4_t val4;
};

ಸಂರಚನಾ __vpiEnumTypespec : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiEnumTypespec();
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;
      ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_iterate(ಪೂರ್ಣಾಙ್ಕ code) override;

      std::vector<enumconst_s> names;
      ಪೂರ್ಣಾಙ್ಕ base_type_code;
      ತಾರ್ಕಿಕ is_signed;
};


ಅನ್ತಃಸ್ಥ __vpiEnumTypespec::__vpiEnumTypespec()
{ }

ಪೂರ್ಣಾಙ್ಕ __vpiEnumTypespec::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiEnumTypespec; }

ಪೂರ್ಣಾಙ್ಕ __vpiEnumTypespec::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ names.size();

	    /* This is not currently set correctly. We always use vpiReg for
	     * four state variables and vpiBitVar for two state variables.
	     * This minimal functionality is needed to get the next() and
	     * prev() methods to work correctly with invalid values. */
	  ಪಕ್ಷ vpiBaseTypespec:
	    ಪ್ರತ್ಯಾಗಮ base_type_code;

	  ಪಕ್ಷ vpiSigned:
	    ಪ್ರತ್ಯಾಗಮ is_signed;

	  ಸ್ವಭಾವ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by __vpiEnumTypespec\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}


vpiHandle __vpiEnumTypespec::vpi_iterate(ಪೂರ್ಣಾಙ್ಕ code)
{
      ಯದಿ (code == vpiEnumConst) {
	    vpiHandle*args = static_cast<vpiHandle*>
	                                (calloc(names.size(), ಪರಿಮಾಣ(vpiHandle*)));
	    ಪ್ರತಿ (size_t idx = 0 ; idx < names.size() ; idx += 1)
		  args[idx] = &names[idx];

	    ಪ್ರತ್ಯಾಗಮ vpip_make_iterator(names.size(), args, ಸತ್ಯ);
      }

      ಪ್ರತ್ಯಾಗಮ 0;
}


ಅನ್ತಃಸ್ಥ enumconst_s::enumconst_s()
{ }

ಪೂರ್ಣಾಙ್ಕ enumconst_s::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiEnumConst; }

ಪೂರ್ಣಾಙ್ಕ enumconst_s::vpi_get(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiSize:
	    ಪ್ರತ್ಯಾಗಮ val4.size()? val4.size() : val2.size();
	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}


ವರ್ಣ* enumconst_s::vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code)
{
      ವಿಕಲ್ಪ (code) {
	  ಪಕ್ಷ vpiName:
	    ಪ್ರತ್ಯಾಗಮ const_cast<ವರ್ಣ*> (name);
	  ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ 0;
      }
}


ಶೂನ್ಯ enumconst_s::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{
      ಯದಿ (val4.size() > 0) {
	    vpip_vec4_get_ಮೌಲ್ಯ(val4, val4.size(), is_signed, val);
      } ಅನ್ಯಥಾ {
	    vpip_vec2_get_ಮೌಲ್ಯ(val2, val2.size(), is_signed, val);
      }
}


ಶೂನ್ಯ compile_enum2_type(ವರ್ಣ*label, long width, ತಾರ್ಕಿಕ signed_flag,
                        std::list<ಸಂರಚನಾ enum_name_s>*names)
{
      ಸಂರಚನಾ __vpiEnumTypespec*spec = ನೂತನ ಸಂರಚನಾ __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiBitVar;

      size_t idx = 0;
      ಪ್ರತಿ (list<ಸಂರಚನಾ enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    ಪ್ರತಿಜ್ಞಾ(cur->val4 == 0);
	    spec->names[idx].name = cur->text;
	    spec->names[idx].val2 = vvp_vector2_t(cur->val2, width);
	    spec->names[idx].is_signed = signed_flag;
      }

      ಪ್ರತಿಜ್ಞಾ(idx == spec->names.size());
      compile_vpi_ಚಿಹ್ನೆ(label, spec);
      vpip_attach_to_current_scope(spec);

      ಮುಕ್ತ(label);
      delete names;
}

ಶೂನ್ಯ compile_enum4_type(ವರ್ಣ*label, long width, ತಾರ್ಕಿಕ signed_flag,
                        std::list<ಸಂರಚನಾ enum_name_s>*names)
{
      ಸಂರಚನಾ __vpiEnumTypespec*spec = ನೂತನ ಸಂರಚನಾ __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiReg;

      size_t idx = 0;
      ಪ್ರತಿ (list<ಸಂರಚನಾ enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    spec->names[idx].name = cur->text;
	    ಪ್ರತಿಜ್ಞಾ(cur->val4);
	    spec->names[idx].val4 = vector4_from_text(cur->val4, width);
	    spec->names[idx].is_signed = signed_flag;
	    ಮುಕ್ತ(cur->val4);
	    cur->val4 = 0;
      }

      ಪ್ರತಿಜ್ಞಾ(idx == spec->names.size());
      compile_vpi_ಚಿಹ್ನೆ(label, spec);
      vpip_attach_to_current_scope(spec);

      ಮುಕ್ತ(label);
      delete names;
}

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ enum_delete(vpiHandle item)
{
      ಸಂರಚನಾ __vpiEnumTypespec*obj = dynamic_cast<__vpiEnumTypespec*>(item);

      ಪ್ರತಿ (vector<enumconst_s>::iterator iter = obj->names.begin();
           iter != obj->names.end(); ++ iter ) {
	    delete [] iter->name;
      }

      delete obj;
}
#endif
