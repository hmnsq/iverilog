/*
 * Copyright (c) 2003-2026 Stephen Williams (steve@icarus.com)
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

# include  "vpi_user.h"
# include  "vpi_priv.h"
# include  "vvp_darray.h"
# include  "vvp_net_sig.h"
# include  <cassert>
# include  <cstdio>
# include  <cstdlib>
# include  <cstring>
# include  <string>
# include  "ivl_alloc.h"

स्थिर अचर वर्ण str_char1_table[257] = {
      ".HS1M222" "W3333333" /* 00 0F */ "L4444444" "44444444" /* 10 1F */
      "P5555555" "55555555" /* 20 2F */ "55555555" "55555555" /* 30 3F */
      "S6666666" "66666666" /* 40 4F */ "66666666" "66666666" /* 50 5F */
      "66666666" "66666666" /* 60 6F */ "66666666" "66666666" /* 70 7F */
      "S7777777" "77777777" /* 80 8F */ "77777777" "77777777" /* 90 9F */
      "77777777" "77777777" /* A0 AF */ "77777777" "77777777" /* B0 BF */
      "77777777" "77777777" /* C0 CF */ "77777777" "77777777" /* D0 DF */
      "77777777" "77777777" /* E0 EF */ "77777777" "77777777" /* F0 FF */ };

स्थिर अचर वर्ण str_char2_table[257] = {
      ".im0e010" "e0102010" /* 00 0F */ "a0102010" "30102010" /* 10 1F */
      "u0102010" "30102010" /* 20 2F */ "40102010" "30102010" /* 30 3F */
      "t0102010" "30102010" /* 40 4F */ "40102010" "30102010" /* 50 5F */
      "50102010" "30102010" /* 60 6F */ "40102010" "30102010" /* 70 7F */
      "u0102010" "30102010" /* 80 8F */ "40102010" "30102010" /* 90 9F */
      "50102010" "30102010" /* A0 AF */ "40102010" "30102010" /* B0 BF */
      "60102010" "30102010" /* C0 CF */ "40102010" "30102010" /* D0 DF */
      "50102010" "30102010" /* E0 EF */ "40102010" "30102010" /* F0 FF */ };


बाह्य "C" शून्य vpip_format_strength(वर्ण*str, s_vpi_मान*value, अचिन्हित bit)
{
      str[0] = '.';
      str[1] = '.';
      str[2] = '.';
      str[3] = 0;

      प्रतिज्ञा(value->format == vpiStrengthVal);

      विकल्प (value->value.strength[bit].logic) {
	  पक्ष vpi0:
	    str[0] = str_char1_table[value->value.strength[bit].s0];
	    str[1] = str_char2_table[value->value.strength[bit].s0];
	    str[2] = '0';
	    रोक;
	  पक्ष vpi1:
	    str[0] = str_char1_table[value->value.strength[bit].s1];
	    str[1] = str_char2_table[value->value.strength[bit].s1];
	    str[2] = '1';
	    रोक;
	  पक्ष vpiX:
	    यदि (value->value.strength[bit].s0 == 1) {
		  str[0] = str_char1_table[value->value.strength[bit].s1];
		  str[1] = str_char2_table[value->value.strength[bit].s1];
		  str[2] = 'H';
	    } अन्यथा यदि (value->value.strength[bit].s1 == 1) {
		  str[0] = str_char1_table[value->value.strength[bit].s0];
		  str[1] = str_char2_table[value->value.strength[bit].s0];
		  str[2] = 'L';
	    } अन्यथा यदि (value->value.strength[bit].s1 ==
		       value->value.strength[bit].s0) {
		  str[0] = str_char1_table[value->value.strength[bit].s0];
		  str[1] = str_char2_table[value->value.strength[bit].s0];
		  str[2] = 'X';
	    } अन्यथा {
		  पूर्णाङ्क ss;

		  str[0] = '0';
		  ss = value->value.strength[bit].s0;
		  जबसम्म (ss > 1) {
			str[0] += 1;
			ss >>= 1;
		  }
		  str[1] = '0';
		  ss = value->value.strength[bit].s1;
		  जबसम्म (ss > 1) {
			str[1] += 1;
			ss >>= 1;
		  }
		  str[2] = 'X';
	    }
	    रोक;
	  पक्ष vpiZ:
	    str[0] = 'H';
	    str[1] = 'i';
	    str[2] = 'Z';
	    रोक;
	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "Unsupported type %d.\n",
	            (पूर्णाङ्क)value->value.strength[bit].logic);
	    प्रतिज्ञा(0);
      }
}

स्थिर वर्ण* vpip_format_pretty_string(अचर वर्ण*सन्देश)
{
      फर्काऊ strdup(सन्देश);
}

स्थिर वर्ण* format_darray_pretty(vvp_darray* aobj)
{
      यदि (!aobj || aobj->get_size() == 0) फर्काऊ strdup("{}");

      std::string out = "{";

      लागि (माप_प्रकार i = 0; i < aobj->get_size(); i += 1) {
	    यदि (i > 0) out += ", ";
	    यदि (dynamic_cast<vvp_darray_real*>(aobj)) {
		  द्विगुण d;
		  aobj->get_word((अचिन्हित) i, d);
		  वर्ण buf[256];
		  snprintf(buf, परिमाण buf, "%g", d);
		  out += buf;
	    } अन्यथा यदि (dynamic_cast<vvp_darray_string*>(aobj)) {
		  std::string s;
		  aobj->get_word((अचिन्हित) i, s);
		  out += "\"";
		  out += s;
		  out += "\"";
	    } अन्यथा {
		  vvp_vector4_t v;
		  aobj->get_word((अचिन्हित) i, v);
		  s_vpi_मान val;
		  val.format = vpiDecStrVal;
		  vpip_vec4_get_मान(v, v.size(), असत्य, &val);
		  out += val.value.str;
	    }
      }

      out += "}";
      फर्काऊ strdup(out.c_str());
}

बाह्य "C" वर्ण* vpip_format_pretty(vpiHandle ref)
{
      यदि (!ref) {
	    फर्काऊ vpip_format_pretty_string("Handle is NULL");
	  }

      यदि (dynamic_cast<__vpiPropQueueRef*>(ref) ||
          dynamic_cast<__vpiDarrayVar*>(ref)) {
	    vvp_darray* aobj = vpip_vpi_darray_from_handle(ref);
	    यदि (!aobj) {
		  फर्काऊ vpip_format_pretty_string("Object not found");
		}
	    फर्काऊ format_darray_pretty(aobj);
      }

      फर्काऊ vpip_format_pretty_string("Unsupported object type");
}
