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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ಆದರೆ_ಅಂತ್ಯ
# include  <iostream>
# include  <cassert>

ಬಳಸಿ ನಾಮಾಕಾಶ std;

ರಚನೆ enumconst_s : ಸಾರ್ವಜನಿಕ __vpiHandle {
      enumconst_s();
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ code) override;
      ಅಕ್ಷರ* vpi_get_str(ಪೂರ್ಣಾಂಕ code) override;
      ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;

      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*name;
      ತಾರ್ಕಿಕ is_signed;
      vvp_vector2_t val2;
      vvp_vector4_t val4;
};

ರಚನೆ __vpiEnumTypespec : ಸಾರ್ವಜನಿಕ __vpiHandle {
      __vpiEnumTypespec();
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;
      ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ code) override;
      vpiHandle vpi_iterate(ಪೂರ್ಣಾಂಕ code) override;

      std::vector<enumconst_s> ಹೆಸರುಗಳು;
      ಪೂರ್ಣಾಂಕ ತಳ_type_ಸಂಕೇತ;
      ತಾರ್ಕಿಕ is_signed;
};


ಸಾಲೊಳಗೆ __vpiEnumTypespec::__vpiEnumTypespec()
{ }

ಪೂರ್ಣಾಂಕ __vpiEnumTypespec::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiEnumTypespec; }

ಪೂರ್ಣಾಂಕ __vpiEnumTypespec::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ ಹೆಸರುಗಳು.size();

	    /* This is not currently set correctly. We always use vpiReg for
	     * four state variables and vpiBitVar for two state variables.
	     * This minimal functionality is needed to get the next() and
	     * prev() methods to work correctly with invalid values. */
	  ಸ್ಥಿತಿ vpi_ತಳ_Typespec:
	    ಹಿಂತಿರುಗಿ ತಳ_type_ಸಂಕೇತ;

	  ಸ್ಥಿತಿ vpiSigned:
	    ಹಿಂತಿರುಗಿ is_signed;

	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by __vpiEnumTypespec\n", code);
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಹಿಂತಿರುಗಿ 0;
      }
}


vpiHandle __vpiEnumTypespec::vpi_iterate(ಪೂರ್ಣಾಂಕ code)
{
      ಆದರೆ (code == vpiEnumConst) {
	    vpiHandle*ವಾದಗಳು = static_cast<vpiHandle*>
	                                (ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು(ಹೆಸರುಗಳು.size(), ಗಾತ್ರ(vpiHandle*)));
	    ಗಾಗಿ (size_t idx = 0 ; idx < ಹೆಸರುಗಳು.size() ; idx += 1)
		  ವಾದಗಳು[idx] = &ಹೆಸರುಗಳು[idx];

	    ಹಿಂತಿರುಗಿ vpip_make_iterator(ಹೆಸರುಗಳು.size(), ವಾದಗಳು, ನಿಜ);
      }

      ಹಿಂತಿರುಗಿ 0;
}


ಸಾಲೊಳಗೆ enumconst_s::enumconst_s()
{ }

ಪೂರ್ಣಾಂಕ enumconst_s::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiEnumConst; }

ಪೂರ್ಣಾಂಕ enumconst_s::vpi_get(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpi_ಗಾತ್ರ:
	    ಹಿಂತಿರುಗಿ val4.size()? val4.size() : val2.size();
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ 0;
      }
}


ಅಕ್ಷರ* enumconst_s::vpi_get_str(ಪೂರ್ಣಾಂಕ code)
{
      ಆಯ್ಕೆ (code) {
	  ಸ್ಥಿತಿ vpiName:
	    ಹಿಂತಿರುಗಿ const_cast<ಅಕ್ಷರ*> (name);
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ 0;
      }
}


ಶೂನ್ಯ enumconst_s::vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val)
{
      ಆದರೆ (val4.size() > 0) {
	    vpip_vec4_get_ಮೌಲ್ಯ(val4, val4.size(), is_signed, val);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    vpip_vec2_get_ಮೌಲ್ಯ(val2, val2.size(), is_signed, val);
      }
}


ಶೂನ್ಯ compile_enum2_type(ಅಕ್ಷರ*label, ದೀರ್ಘ width, ತಾರ್ಕಿಕ signed_flag,
                        std::list<ರಚನೆ enum_name_s>*ಹೆಸರುಗಳು)
{
      ರಚನೆ __vpiEnumTypespec*spec = ಹೊಸ ರಚನೆ __vpiEnumTypespec;
      spec->ಹೆಸರುಗಳು = std::vector<enumconst_s> (ಹೆಸರುಗಳು->size());
      spec->is_signed = signed_flag;
      spec->ತಳ_type_ಸಂಕೇತ = vpi_ಬಿಟ್_Var;

      size_t idx = 0;
      ಗಾಗಿ (list<ರಚನೆ enum_name_s>::iterator cur = ಹೆಸರುಗಳು->begin()
		 ; cur != ಹೆಸರುಗಳು->ಕೊನೆ() ;  ++cur, ++idx) {
	    ಪ್ರತಿಜ್ಞಾ(cur->val4 == 0);
	    spec->ಹೆಸರುಗಳು[idx].name = cur->ಪಠ್ಯ;
	    spec->ಹೆಸರುಗಳು[idx].val2 = vvp_vector2_t(cur->val2, width);
	    spec->ಹೆಸರುಗಳು[idx].is_signed = signed_flag;
      }

      ಪ್ರತಿಜ್ಞಾ(idx == spec->ಹೆಸರುಗಳು.size());
      compile_vpi_ಚಿಹ್ನೆ(label, spec);
      vpip_attach_ಗಮ್ಯ_current_scope(spec);

      ಬಿಡುಗಡೆ(label);
      ಅಳಿಸು ಹೆಸರುಗಳು;
}

ಶೂನ್ಯ compile_enum4_type(ಅಕ್ಷರ*label, ದೀರ್ಘ width, ತಾರ್ಕಿಕ signed_flag,
                        std::list<ರಚನೆ enum_name_s>*ಹೆಸರುಗಳು)
{
      ರಚನೆ __vpiEnumTypespec*spec = ಹೊಸ ರಚನೆ __vpiEnumTypespec;
      spec->ಹೆಸರುಗಳು = std::vector<enumconst_s> (ಹೆಸರುಗಳು->size());
      spec->is_signed = signed_flag;
      spec->ತಳ_type_ಸಂಕೇತ = vpiReg;

      size_t idx = 0;
      ಗಾಗಿ (list<ರಚನೆ enum_name_s>::iterator cur = ಹೆಸರುಗಳು->begin()
		 ; cur != ಹೆಸರುಗಳು->ಕೊನೆ() ;  ++cur, ++idx) {
	    spec->ಹೆಸರುಗಳು[idx].name = cur->ಪಠ್ಯ;
	    ಪ್ರತಿಜ್ಞಾ(cur->val4);
	    spec->ಹೆಸರುಗಳು[idx].val4 = vector4_from_ಪಠ್ಯ(cur->val4, width);
	    spec->ಹೆಸರುಗಳು[idx].is_signed = signed_flag;
	    ಬಿಡುಗಡೆ(cur->val4);
	    cur->val4 = 0;
      }

      ಪ್ರತಿಜ್ಞಾ(idx == spec->ಹೆಸರುಗಳು.size());
      compile_vpi_ಚಿಹ್ನೆ(label, spec);
      vpip_attach_ಗಮ್ಯ_current_scope(spec);

      ಬಿಡುಗಡೆ(label);
      ಅಳಿಸು ಹೆಸರುಗಳು;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ enum_delete(vpiHandle ಅಂಶ)
{
      ರಚನೆ __vpiEnumTypespec*obj = dynamic_cast<__vpiEnumTypespec*>(ಅಂಶ);

      ಗಾಗಿ (vector<enumconst_s>::iterator iter = obj->ಹೆಸರುಗಳು.begin();
           iter != obj->ಹೆಸರುಗಳು.ಕೊನೆ(); ++ iter ) {
	    ಅಳಿಸು [] iter->name;
      }

      ಅಳಿಸು obj;
}
#ಆದರೆ_ಅಂತ್ಯ
