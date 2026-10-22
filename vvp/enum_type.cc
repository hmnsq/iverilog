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
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#اگر_ختم
# include  <iostream>
# include  <cassert>

استعمال نامخلا std;

ساخت enumconst_s : عوامی __vpiHandle {
      enumconst_s();
      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      حرف* vpi_get_str(صحیح code) override;
      خالی vpi_get_قدر(p_vpi_قدر val) override;

      ثابت حرف*name;
      منطقی is_signed;
      vvp_vector2_t val2;
      vvp_vector4_t val4;
};

ساخت __vpiEnumTypespec : عوامی __vpiHandle {
      __vpiEnumTypespec();
      صحیح get_type_رمز(خالی) ثابت override;
      صحیح vpi_get(صحیح code) override;
      vpiHandle vpi_iterate(صحیح code) override;

      std::vector<enumconst_s> نام;
      صحیح بنیاد_type_رمز;
      منطقی is_signed;
};


اندرونی __vpiEnumTypespec::__vpiEnumTypespec()
{ }

صحیح __vpiEnumTypespec::get_type_رمز(خالی) ثابت
{ واپس vpiEnumTypespec; }

صحیح __vpiEnumTypespec::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpi_سائز:
	    واپس نام.size();

	    /* This is not currently set correctly. We always use vpiReg for
	     * four state variables and vpiBitVar for two state variables.
	     * This minimal functionality is needed to get the next() and
	     * prev() methods to work correctly with invalid values. */
	  حالت vpi_بنیاد_Typespec:
	    واپس بنیاد_type_رمز;

	  حالت vpiSigned:
	    واپس is_signed;

	  طےشدہ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by __vpiEnumTypespec\n", code);
	    أكد(0);
	    واپس 0;
      }
}


vpiHandle __vpiEnumTypespec::vpi_iterate(صحیح code)
{
      اگر (code == vpiEnumConst) {
	    vpiHandle*دلائل = static_cast<vpiHandle*>
	                                (صفر_حافظہ_دو(نام.size(), حجم(vpiHandle*)));
	    برائے (size_t idx = 0 ; idx < نام.size() ; idx += 1)
		  دلائل[idx] = &نام[idx];

	    واپس vpip_make_iterator(نام.size(), دلائل, سچ);
      }

      واپس 0;
}


اندرونی enumconst_s::enumconst_s()
{ }

صحیح enumconst_s::get_type_رمز(خالی) ثابت
{ واپس vpiEnumConst; }

صحیح enumconst_s::vpi_get(صحیح code)
{
      انتخاب (code) {
	  حالت vpi_سائز:
	    واپس val4.size()? val4.size() : val2.size();
	  طےشدہ:
	    واپس 0;
      }
}


حرف* enumconst_s::vpi_get_str(صحیح code)
{
      انتخاب (code) {
	  حالت vpiName:
	    واپس const_cast<حرف*> (name);
	  طےشدہ:
	    واپس 0;
      }
}


خالی enumconst_s::vpi_get_قدر(p_vpi_قدر val)
{
      اگر (val4.size() > 0) {
	    vpip_vec4_get_قدر(val4, val4.size(), is_signed, val);
      } ورنہ {
	    vpip_vec2_get_قدر(val2, val2.size(), is_signed, val);
      }
}


خالی compile_enum2_type(حرف*label, طویل width, منطقی signed_flag,
                        std::list<ساخت enum_name_s>*نام)
{
      ساخت __vpiEnumTypespec*spec = نیا ساخت __vpiEnumTypespec;
      spec->نام = std::vector<enumconst_s> (نام->size());
      spec->is_signed = signed_flag;
      spec->بنیاد_type_رمز = vpi_ثنائی_ہندسہ_Var;

      size_t idx = 0;
      برائے (list<ساخت enum_name_s>::iterator cur = نام->begin()
		 ; cur != نام->انتہا() ;  ++cur, ++idx) {
	    أكد(cur->val4 == 0);
	    spec->نام[idx].name = cur->متن;
	    spec->نام[idx].val2 = vvp_vector2_t(cur->val2, width);
	    spec->نام[idx].is_signed = signed_flag;
      }

      أكد(idx == spec->نام.size());
      compile_vpi_علامت(label, spec);
      vpip_attach_مقصد_current_scope(spec);

      آزاد(label);
      حذف نام;
}

خالی compile_enum4_type(حرف*label, طویل width, منطقی signed_flag,
                        std::list<ساخت enum_name_s>*نام)
{
      ساخت __vpiEnumTypespec*spec = نیا ساخت __vpiEnumTypespec;
      spec->نام = std::vector<enumconst_s> (نام->size());
      spec->is_signed = signed_flag;
      spec->بنیاد_type_رمز = vpiReg;

      size_t idx = 0;
      برائے (list<ساخت enum_name_s>::iterator cur = نام->begin()
		 ; cur != نام->انتہا() ;  ++cur, ++idx) {
	    spec->نام[idx].name = cur->متن;
	    أكد(cur->val4);
	    spec->نام[idx].val4 = vector4_from_متن(cur->val4, width);
	    spec->نام[idx].is_signed = signed_flag;
	    آزاد(cur->val4);
	    cur->val4 = 0;
      }

      أكد(idx == spec->نام.size());
      compile_vpi_علامت(label, spec);
      vpip_attach_مقصد_current_scope(spec);

      آزاد(label);
      حذف نام;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی enum_delete(vpiHandle شے)
{
      ساخت __vpiEnumTypespec*obj = dynamic_cast<__vpiEnumTypespec*>(شے);

      برائے (vector<enumconst_s>::iterator iter = obj->نام.begin();
           iter != obj->نام.انتہا(); ++ iter ) {
	    حذف [] iter->name;
      }

      حذف obj;
}
#اگر_ختم
