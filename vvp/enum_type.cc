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
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#అయితే_ముగింపు
# include  <iostream>
# include  <cassert>

ఉపయోగించి నామాకాశ std;

నిర్మాణం enumconst_s : బహిరంగ __vpiHandle {
      enumconst_s();
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం code) override;
      అక్షరం* vpi_get_str(పూర్ణాంకం code) override;
      శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;

      స్థిరరాశి అక్షరం*name;
      తార్కిక is_signed;
      vvp_vector2_t val2;
      vvp_vector4_t val4;
};

నిర్మాణం __vpiEnumTypespec : బహిరంగ __vpiHandle {
      __vpiEnumTypespec();
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;
      పూర్ణాంకం vpi_get(పూర్ణాంకం code) override;
      vpiHandle vpi_iterate(పూర్ణాంకం code) override;

      std::vector<enumconst_s> పేర్లు;
      పూర్ణాంకం పునాది_type_సంకేతం;
      తార్కిక is_signed;
};


అంతఃవరుస __vpiEnumTypespec::__vpiEnumTypespec()
{ }

పూర్ణాంకం __vpiEnumTypespec::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiEnumTypespec; }

పూర్ణాంకం __vpiEnumTypespec::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpi_కొలత:
	    తిరిగి పేర్లు.size();

	    /* This is not currently set correctly. We always use vpiReg for
	     * four state variables and vpiBitVar for two state variables.
	     * This minimal functionality is needed to get the next() and
	     * prev() methods to work correctly with invalid values. */
	  స్థితి vpi_పునాది_Typespec:
	    తిరిగి పునాది_type_సంకేతం;

	  స్థితి vpiSigned:
	    తిరిగి is_signed;

	  అప్రమేయ:
	    fprintf(stderr, "vvp error: get %d not supported "
		    "by __vpiEnumTypespec\n", code);
	    ప్రతిజ్ఞా(0);
	    తిరిగి 0;
      }
}


vpiHandle __vpiEnumTypespec::vpi_iterate(పూర్ణాంకం code)
{
      ఒకవేళ (code == vpiEnumConst) {
	    vpiHandle*వాదనలు = static_cast<vpiHandle*>
	                                (సున్నా_జ్ఞాపకం_ఇవ్వు(పేర్లు.size(), పరిమాణం(vpiHandle*)));
	    కొరకు (size_t idx = 0 ; idx < పేర్లు.size() ; idx += 1)
		  వాదనలు[idx] = &పేర్లు[idx];

	    తిరిగి vpip_make_iterator(పేర్లు.size(), వాదనలు, నిజం);
      }

      తిరిగి 0;
}


అంతఃవరుస enumconst_s::enumconst_s()
{ }

పూర్ణాంకం enumconst_s::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiEnumConst; }

పూర్ణాంకం enumconst_s::vpi_get(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpi_కొలత:
	    తిరిగి val4.size()? val4.size() : val2.size();
	  అప్రమేయ:
	    తిరిగి 0;
      }
}


అక్షరం* enumconst_s::vpi_get_str(పూర్ణాంకం code)
{
      ఎంపిక (code) {
	  స్థితి vpiName:
	    తిరిగి const_cast<అక్షరం*> (name);
	  అప్రమేయ:
	    తిరిగి 0;
      }
}


శూన్య enumconst_s::vpi_get_విలువ(p_vpi_విలువ val)
{
      ఒకవేళ (val4.size() > 0) {
	    vpip_vec4_get_విలువ(val4, val4.size(), is_signed, val);
      } లేకపోతే {
	    vpip_vec2_get_విలువ(val2, val2.size(), is_signed, val);
      }
}


శూన్య compile_enum2_type(అక్షరం*label, దీర్ఘ width, తార్కిక signed_flag,
                        std::list<నిర్మాణం enum_name_s>*పేర్లు)
{
      నిర్మాణం __vpiEnumTypespec*spec = కొత్త నిర్మాణం __vpiEnumTypespec;
      spec->పేర్లు = std::vector<enumconst_s> (పేర్లు->size());
      spec->is_signed = signed_flag;
      spec->పునాది_type_సంకేతం = vpi_బిట్టు_Var;

      size_t idx = 0;
      కొరకు (list<నిర్మాణం enum_name_s>::iterator cur = పేర్లు->begin()
		 ; cur != పేర్లు->చివర() ;  ++cur, ++idx) {
	    ప్రతిజ్ఞా(cur->val4 == 0);
	    spec->పేర్లు[idx].name = cur->పాఠ్యం;
	    spec->పేర్లు[idx].val2 = vvp_vector2_t(cur->val2, width);
	    spec->పేర్లు[idx].is_signed = signed_flag;
      }

      ప్రతిజ్ఞా(idx == spec->పేర్లు.size());
      compile_vpi_చిహ్నం(label, spec);
      vpip_attach_గమ్యం_current_scope(spec);

      విడుదల(label);
      తొలగించు పేర్లు;
}

శూన్య compile_enum4_type(అక్షరం*label, దీర్ఘ width, తార్కిక signed_flag,
                        std::list<నిర్మాణం enum_name_s>*పేర్లు)
{
      నిర్మాణం __vpiEnumTypespec*spec = కొత్త నిర్మాణం __vpiEnumTypespec;
      spec->పేర్లు = std::vector<enumconst_s> (పేర్లు->size());
      spec->is_signed = signed_flag;
      spec->పునాది_type_సంకేతం = vpiReg;

      size_t idx = 0;
      కొరకు (list<నిర్మాణం enum_name_s>::iterator cur = పేర్లు->begin()
		 ; cur != పేర్లు->చివర() ;  ++cur, ++idx) {
	    spec->పేర్లు[idx].name = cur->పాఠ్యం;
	    ప్రతిజ్ఞా(cur->val4);
	    spec->పేర్లు[idx].val4 = vector4_from_పాఠ్యం(cur->val4, width);
	    spec->పేర్లు[idx].is_signed = signed_flag;
	    విడుదల(cur->val4);
	    cur->val4 = 0;
      }

      ప్రతిజ్ఞా(idx == spec->పేర్లు.size());
      compile_vpi_చిహ్నం(label, spec);
      vpip_attach_గమ్యం_current_scope(spec);

      విడుదల(label);
      తొలగించు పేర్లు;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య enum_delete(vpiHandle అంశం)
{
      నిర్మాణం __vpiEnumTypespec*obj = dynamic_cast<__vpiEnumTypespec*>(అంశం);

      కొరకు (vector<enumconst_s>::iterator iter = obj->పేర్లు.begin();
           iter != obj->పేర్లు.చివర(); ++ iter ) {
	    తొలగించు [] iter->name;
      }

      తొలగించు obj;
}
#అయితే_ముగింపు
