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

স্থির ধ্রুব বর্ণ str_char1_table[257] = {
      ".HS1M222" "W3333333" /* 00 0F */ "L4444444" "44444444" /* 10 1F */
      "P5555555" "55555555" /* 20 2F */ "55555555" "55555555" /* 30 3F */
      "S6666666" "66666666" /* 40 4F */ "66666666" "66666666" /* 50 5F */
      "66666666" "66666666" /* 60 6F */ "66666666" "66666666" /* 70 7F */
      "S7777777" "77777777" /* 80 8F */ "77777777" "77777777" /* 90 9F */
      "77777777" "77777777" /* A0 AF */ "77777777" "77777777" /* B0 BF */
      "77777777" "77777777" /* C0 CF */ "77777777" "77777777" /* D0 DF */
      "77777777" "77777777" /* E0 EF */ "77777777" "77777777" /* F0 FF */ };

স্থির ধ্রুব বর্ণ str_char2_table[257] = {
      ".im0e010" "e0102010" /* 00 0F */ "a0102010" "30102010" /* 10 1F */
      "u0102010" "30102010" /* 20 2F */ "40102010" "30102010" /* 30 3F */
      "t0102010" "30102010" /* 40 4F */ "40102010" "30102010" /* 50 5F */
      "50102010" "30102010" /* 60 6F */ "40102010" "30102010" /* 70 7F */
      "u0102010" "30102010" /* 80 8F */ "40102010" "30102010" /* 90 9F */
      "50102010" "30102010" /* A0 AF */ "40102010" "30102010" /* B0 BF */
      "60102010" "30102010" /* C0 CF */ "40102010" "30102010" /* D0 DF */
      "50102010" "30102010" /* E0 EF */ "40102010" "30102010" /* F0 FF */ };


বাহ্য "C" শূন্য vpip_format_strength(বর্ণ*str, s_vpi_মান*value, unsigned bit)
{
      str[0] = '.';
      str[1] = '.';
      str[2] = '.';
      str[3] = 0;

      প্রতিজ্ঞা(value->format == vpiStrengthVal);

      বিকল্প (value->value.strength[bit].logic) {
	  পক্ষ vpi0:
	    str[0] = str_char1_table[value->value.strength[bit].s0];
	    str[1] = str_char2_table[value->value.strength[bit].s0];
	    str[2] = '0';
	    ভঙ্গ;
	  পক্ষ vpi1:
	    str[0] = str_char1_table[value->value.strength[bit].s1];
	    str[1] = str_char2_table[value->value.strength[bit].s1];
	    str[2] = '1';
	    ভঙ্গ;
	  পক্ষ vpiX:
	    যদি (value->value.strength[bit].s0 == 1) {
		  str[0] = str_char1_table[value->value.strength[bit].s1];
		  str[1] = str_char2_table[value->value.strength[bit].s1];
		  str[2] = 'H';
	    } অন্যথা যদি (value->value.strength[bit].s1 == 1) {
		  str[0] = str_char1_table[value->value.strength[bit].s0];
		  str[1] = str_char2_table[value->value.strength[bit].s0];
		  str[2] = 'L';
	    } অন্যথা যদি (value->value.strength[bit].s1 ==
		       value->value.strength[bit].s0) {
		  str[0] = str_char1_table[value->value.strength[bit].s0];
		  str[1] = str_char2_table[value->value.strength[bit].s0];
		  str[2] = 'X';
	    } অন্যথা {
		  পূর্ণাঙ্ক ss;

		  str[0] = '0';
		  ss = value->value.strength[bit].s0;
		  যাবত্ (ss > 1) {
			str[0] += 1;
			ss >>= 1;
		  }
		  str[1] = '0';
		  ss = value->value.strength[bit].s1;
		  যাবত্ (ss > 1) {
			str[1] += 1;
			ss >>= 1;
		  }
		  str[2] = 'X';
	    }
	    ভঙ্গ;
	  পক্ষ vpiZ:
	    str[0] = 'H';
	    str[1] = 'i';
	    str[2] = 'Z';
	    ভঙ্গ;
	  স্বভাব:
	    fprintf(stderr, "Unsupported type %d.\n",
	            (পূর্ণাঙ্ক)value->value.strength[bit].logic);
	    প্রতিজ্ঞা(0);
      }
}

স্থির বর্ণ* vpip_format_pretty_string(ধ্রুব বর্ণ*বার্তা)
{
      প্রত্যাগম strdup(বার্তা);
}

স্থির বর্ণ* format_darray_pretty(vvp_darray* aobj)
{
      যদি (!aobj || aobj->get_size() == 0) প্রত্যাগম strdup("{}");

      std::string out = "{";

      প্রতি (size_t i = 0; i < aobj->get_size(); i += 1) {
	    যদি (i > 0) out += ", ";
	    যদি (dynamic_cast<vvp_darray_real*>(aobj)) {
		  দ্বিগুণ d;
		  aobj->get_word((unsigned) i, d);
		  বর্ণ buf[256];
		  snprintf(buf, পরিমাণ buf, "%g", d);
		  out += buf;
	    } অন্যথা যদি (dynamic_cast<vvp_darray_string*>(aobj)) {
		  std::string s;
		  aobj->get_word((unsigned) i, s);
		  out += "\"";
		  out += s;
		  out += "\"";
	    } অন্যথা {
		  vvp_vector4_t v;
		  aobj->get_word((unsigned) i, v);
		  s_vpi_মান val;
		  val.format = vpiDecStrVal;
		  vpip_vec4_get_মান(v, v.size(), অসত্য, &val);
		  out += val.value.str;
	    }
      }

      out += "}";
      প্রত্যাগম strdup(out.c_str());
}

বাহ্য "C" বর্ণ* vpip_format_pretty(vpiHandle ref)
{
      যদি (!ref) {
	    প্রত্যাগম vpip_format_pretty_string("Handle is NULL");
	  }

      যদি (dynamic_cast<__vpiPropQueueRef*>(ref) ||
          dynamic_cast<__vpiDarrayVar*>(ref)) {
	    vvp_darray* aobj = vpip_vpi_darray_from_handle(ref);
	    যদি (!aobj) {
		  প্রত্যাগম vpip_format_pretty_string("Object not found");
		}
	    প্রত্যাগম format_darray_pretty(aobj);
      }

      প্রত্যাগম vpip_format_pretty_string("Unsupported object type");
}
