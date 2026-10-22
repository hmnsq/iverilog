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

സ്ഥിതം സ്ഥിരം അക്ഷരം str_char1_table[257] = {
      ".HS1M222" "W3333333" /* 00 0F */ "L4444444" "44444444" /* 10 1F */
      "P5555555" "55555555" /* 20 2F */ "55555555" "55555555" /* 30 3F */
      "S6666666" "66666666" /* 40 4F */ "66666666" "66666666" /* 50 5F */
      "66666666" "66666666" /* 60 6F */ "66666666" "66666666" /* 70 7F */
      "S7777777" "77777777" /* 80 8F */ "77777777" "77777777" /* 90 9F */
      "77777777" "77777777" /* A0 AF */ "77777777" "77777777" /* B0 BF */
      "77777777" "77777777" /* C0 CF */ "77777777" "77777777" /* D0 DF */
      "77777777" "77777777" /* E0 EF */ "77777777" "77777777" /* F0 FF */ };

സ്ഥിതം സ്ഥിരം അക്ഷരം str_char2_table[257] = {
      ".im0e010" "e0102010" /* 00 0F */ "a0102010" "30102010" /* 10 1F */
      "u0102010" "30102010" /* 20 2F */ "40102010" "30102010" /* 30 3F */
      "t0102010" "30102010" /* 40 4F */ "40102010" "30102010" /* 50 5F */
      "50102010" "30102010" /* 60 6F */ "40102010" "30102010" /* 70 7F */
      "u0102010" "30102010" /* 80 8F */ "40102010" "30102010" /* 90 9F */
      "50102010" "30102010" /* A0 AF */ "40102010" "30102010" /* B0 BF */
      "60102010" "30102010" /* C0 CF */ "40102010" "30102010" /* D0 DF */
      "50102010" "30102010" /* E0 EF */ "40102010" "30102010" /* F0 FF */ };


ബാഹ്യ "C" ശൂന്യം vpip_format_strength(അക്ഷരം*str, s_vpi_മൂല്യം*value, unsigned bit)
{
      str[0] = '.';
      str[1] = '.';
      str[2] = '.';
      str[3] = 0;

      പ്രതിജ്ഞാ(value->format == vpiStrengthVal);

      തിരഞ്ഞെടുപ്പ് (value->value.strength[bit].logic) {
	  അവസ്ഥ vpi0:
	    str[0] = str_char1_table[value->value.strength[bit].s0];
	    str[1] = str_char2_table[value->value.strength[bit].s0];
	    str[2] = '0';
	    നിർത്തുക;
	  അവസ്ഥ vpi1:
	    str[0] = str_char1_table[value->value.strength[bit].s1];
	    str[1] = str_char2_table[value->value.strength[bit].s1];
	    str[2] = '1';
	    നിർത്തുക;
	  അവസ്ഥ vpiX:
	    എങ്കിൽ (value->value.strength[bit].s0 == 1) {
		  str[0] = str_char1_table[value->value.strength[bit].s1];
		  str[1] = str_char2_table[value->value.strength[bit].s1];
		  str[2] = 'H';
	    } അല്ലെങ്കിൽ എങ്കിൽ (value->value.strength[bit].s1 == 1) {
		  str[0] = str_char1_table[value->value.strength[bit].s0];
		  str[1] = str_char2_table[value->value.strength[bit].s0];
		  str[2] = 'L';
	    } അല്ലെങ്കിൽ എങ്കിൽ (value->value.strength[bit].s1 ==
		       value->value.strength[bit].s0) {
		  str[0] = str_char1_table[value->value.strength[bit].s0];
		  str[1] = str_char2_table[value->value.strength[bit].s0];
		  str[2] = 'X';
	    } അല്ലെങ്കിൽ {
		  പൂർണ്ണസംഖ്യ ss;

		  str[0] = '0';
		  ss = value->value.strength[bit].s0;
		  ആകുമ്പോൾ (ss > 1) {
			str[0] += 1;
			ss >>= 1;
		  }
		  str[1] = '0';
		  ss = value->value.strength[bit].s1;
		  ആകുമ്പോൾ (ss > 1) {
			str[1] += 1;
			ss >>= 1;
		  }
		  str[2] = 'X';
	    }
	    നിർത്തുക;
	  അവസ്ഥ vpiZ:
	    str[0] = 'H';
	    str[1] = 'i';
	    str[2] = 'Z';
	    നിർത്തുക;
	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "Unsupported type %d.\n",
	            (പൂർണ്ണസംഖ്യ)value->value.strength[bit].logic);
	    പ്രതിജ്ഞാ(0);
      }
}

സ്ഥിതം അക്ഷരം* vpip_format_pretty_string(സ്ഥിരം അക്ഷരം*സന്ദേശം)
{
      തിരികെ strdup(സന്ദേശം);
}

സ്ഥിതം അക്ഷരം* format_darray_pretty(vvp_darray* aobj)
{
      എങ്കിൽ (!aobj || aobj->get_size() == 0) തിരികെ strdup("{}");

      std::string out = "{";

      വേണ്ടി (size_t i = 0; i < aobj->get_size(); i += 1) {
	    എങ്കിൽ (i > 0) out += ", ";
	    എങ്കിൽ (dynamic_cast<vvp_darray_real*>(aobj)) {
		  ഇരട്ട d;
		  aobj->get_word((unsigned) i, d);
		  അക്ഷരം buf[256];
		  snprintf(buf, വലുപ്പം buf, "%g", d);
		  out += buf;
	    } അല്ലെങ്കിൽ എങ്കിൽ (dynamic_cast<vvp_darray_string*>(aobj)) {
		  std::string s;
		  aobj->get_word((unsigned) i, s);
		  out += "\"";
		  out += s;
		  out += "\"";
	    } അല്ലെങ്കിൽ {
		  vvp_vector4_t v;
		  aobj->get_word((unsigned) i, v);
		  s_vpi_മൂല്യം val;
		  val.format = vpiDecStrVal;
		  vpip_vec4_get_മൂല്യം(v, v.size(), അസത്യം, &val);
		  out += val.value.str;
	    }
      }

      out += "}";
      തിരികെ strdup(out.c_str());
}

ബാഹ്യ "C" അക്ഷരം* vpip_format_pretty(vpiHandle ref)
{
      എങ്കിൽ (!ref) {
	    തിരികെ vpip_format_pretty_string("Handle is NULL");
	  }

      എങ്കിൽ (dynamic_cast<__vpiPropQueueRef*>(ref) ||
          dynamic_cast<__vpiDarrayVar*>(ref)) {
	    vvp_darray* aobj = vpip_vpi_darray_from_handle(ref);
	    എങ്കിൽ (!aobj) {
		  തിരികെ vpip_format_pretty_string("Object not found");
		}
	    തിരികെ format_darray_pretty(aobj);
      }

      തിരികെ vpip_format_pretty_string("Unsupported object type");
}
