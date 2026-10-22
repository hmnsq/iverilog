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

using નામાકાશ std;

સંરચના enumconst_s : જાહેર __vpiHandle {
      enumconst_s();
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override;
      પૂર્ણાંક vpi_get(પૂર્ણાંક code) override;
      અક્ષર* vpi_get_str(પૂર્ણાંક code) override;
      શૂન્ય vpi_get_કિંમત(p_vpi_કિંમત val) override;

      અચલ અક્ષર*name;
      તાર્કિક is_signed;
      vvp_vector2_t val2;
      vvp_vector4_t val4;
};

સંરચના __vpiEnumTypespec : જાહેર __vpiHandle {
      __vpiEnumTypespec();
      પૂર્ણાંક get_type_code(શૂન્ય) અચલ override;
      પૂર્ણાંક vpi_get(પૂર્ણાંક code) override;
      vpiHandle vpi_iterate(પૂર્ણાંક code) override;

      std::vector<enumconst_s> names;
      પૂર્ણાંક base_type_code;
      તાર્કિક is_signed;
};


અંતઃરેખ __vpiEnumTypespec::__vpiEnumTypespec()
{ }

પૂર્ણાંક __vpiEnumTypespec::get_type_code(શૂન્ય) અચલ
{ પાછા vpiEnumTypespec; }

પૂર્ણાંક __vpiEnumTypespec::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiSize:
	    પાછા names.size();

	    /* This is not currently set correctly. We always use vpiReg for
	     * four state variables and vpiBitVar for two state variables.
	     * This minimal functionality is needed to get the next() and
	     * prev() methods to work correctly with invalid values. */
	  સ્થિતિ vpiBaseTypespec:
	    પાછા base_type_code;

	  સ્થિતિ vpiSigned:
	    પાછા is_signed;

	  મૂળભૂત:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by __vpiEnumTypespec\n", code);
	    પ્રતિજ્ઞા(0);
	    પાછા 0;
      }
}


vpiHandle __vpiEnumTypespec::vpi_iterate(પૂર્ણાંક code)
{
      જો (code == vpiEnumConst) {
	    vpiHandle*args = static_cast<vpiHandle*>
	                                (calloc(names.size(), કદ(vpiHandle*)));
	    માટે (size_t idx = 0 ; idx < names.size() ; idx += 1)
		  args[idx] = &names[idx];

	    પાછા vpip_make_iterator(names.size(), args, સાચું);
      }

      પાછા 0;
}


અંતઃરેખ enumconst_s::enumconst_s()
{ }

પૂર્ણાંક enumconst_s::get_type_code(શૂન્ય) અચલ
{ પાછા vpiEnumConst; }

પૂર્ણાંક enumconst_s::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiSize:
	    પાછા val4.size()? val4.size() : val2.size();
	  મૂળભૂત:
	    પાછા 0;
      }
}


અક્ષર* enumconst_s::vpi_get_str(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiName:
	    પાછા const_cast<અક્ષર*> (name);
	  મૂળભૂત:
	    પાછા 0;
      }
}


શૂન્ય enumconst_s::vpi_get_કિંમત(p_vpi_કિંમત val)
{
      જો (val4.size() > 0) {
	    vpip_vec4_get_કિંમત(val4, val4.size(), is_signed, val);
      } નહીંતર {
	    vpip_vec2_get_કિંમત(val2, val2.size(), is_signed, val);
      }
}


શૂન્ય compile_enum2_type(અક્ષર*label, long width, તાર્કિક signed_flag,
                        std::list<સંરચના enum_name_s>*names)
{
      સંરચના __vpiEnumTypespec*spec = નવું સંરચના __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiBitVar;

      size_t idx = 0;
      માટે (list<સંરચના enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    પ્રતિજ્ઞા(cur->val4 == 0);
	    spec->names[idx].name = cur->text;
	    spec->names[idx].val2 = vvp_vector2_t(cur->val2, width);
	    spec->names[idx].is_signed = signed_flag;
      }

      પ્રતિજ્ઞા(idx == spec->names.size());
      compile_vpi_પ્રતીક(label, spec);
      vpip_attach_to_current_scope(spec);

      મુક્ત(label);
      delete names;
}

શૂન્ય compile_enum4_type(અક્ષર*label, long width, તાર્કિક signed_flag,
                        std::list<સંરચના enum_name_s>*names)
{
      સંરચના __vpiEnumTypespec*spec = નવું સંરચના __vpiEnumTypespec;
      spec->names = std::vector<enumconst_s> (names->size());
      spec->is_signed = signed_flag;
      spec->base_type_code = vpiReg;

      size_t idx = 0;
      માટે (list<સંરચના enum_name_s>::iterator cur = names->begin()
		 ; cur != names->end() ;  ++cur, ++idx) {
	    spec->names[idx].name = cur->text;
	    પ્રતિજ્ઞા(cur->val4);
	    spec->names[idx].val4 = vector4_from_text(cur->val4, width);
	    spec->names[idx].is_signed = signed_flag;
	    મુક્ત(cur->val4);
	    cur->val4 = 0;
      }

      પ્રતિજ્ઞા(idx == spec->names.size());
      compile_vpi_પ્રતીક(label, spec);
      vpip_attach_to_current_scope(spec);

      મુક્ત(label);
      delete names;
}

#ifdef CHECK_WITH_VALGRIND
શૂન્ય enum_delete(vpiHandle item)
{
      સંરચના __vpiEnumTypespec*obj = dynamic_cast<__vpiEnumTypespec*>(item);

      માટે (vector<enumconst_s>::iterator iter = obj->names.begin();
           iter != obj->names.end(); ++ iter ) {
	    delete [] iter->name;
      }

      delete obj;
}
#endif
