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

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର str_char1_table[257] = {
      ".HS1M222" "W3333333" /* 00 0F */ "L4444444" "44444444" /* 10 1F */
      "P5555555" "55555555" /* 20 2F */ "55555555" "55555555" /* 30 3F */
      "S6666666" "66666666" /* 40 4F */ "66666666" "66666666" /* 50 5F */
      "66666666" "66666666" /* 60 6F */ "66666666" "66666666" /* 70 7F */
      "S7777777" "77777777" /* 80 8F */ "77777777" "77777777" /* 90 9F */
      "77777777" "77777777" /* A0 AF */ "77777777" "77777777" /* B0 BF */
      "77777777" "77777777" /* C0 CF */ "77777777" "77777777" /* D0 DF */
      "77777777" "77777777" /* E0 EF */ "77777777" "77777777" /* F0 FF */ };

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର str_char2_table[257] = {
      ".im0e010" "e0102010" /* 00 0F */ "a0102010" "30102010" /* 10 1F */
      "u0102010" "30102010" /* 20 2F */ "40102010" "30102010" /* 30 3F */
      "t0102010" "30102010" /* 40 4F */ "40102010" "30102010" /* 50 5F */
      "50102010" "30102010" /* 60 6F */ "40102010" "30102010" /* 70 7F */
      "u0102010" "30102010" /* 80 8F */ "40102010" "30102010" /* 90 9F */
      "50102010" "30102010" /* A0 AF */ "40102010" "30102010" /* B0 BF */
      "60102010" "30102010" /* C0 CF */ "40102010" "30102010" /* D0 DF */
      "50102010" "30102010" /* E0 EF */ "40102010" "30102010" /* F0 FF */ };


ବାହ୍ୟ "C" ଶୂନ୍ୟ vpip_format_strength(ଅକ୍ଷର*str, s_vpi_ମୂଲ୍ୟ*value, unsigned bit)
{
      str[0] = '.';
      str[1] = '.';
      str[2] = '.';
      str[3] = 0;

      ପ୍ରତିଜ୍ଞା(value->format == vpiStrengthVal);

      ବାଛ (value->value.strength[bit].logic) {
	  ସ୍ଥିତି vpi0:
	    str[0] = str_char1_table[value->value.strength[bit].s0];
	    str[1] = str_char2_table[value->value.strength[bit].s0];
	    str[2] = '0';
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpi1:
	    str[0] = str_char1_table[value->value.strength[bit].s1];
	    str[1] = str_char2_table[value->value.strength[bit].s1];
	    str[2] = '1';
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiX:
	    ଯଦି (value->value.strength[bit].s0 == 1) {
		  str[0] = str_char1_table[value->value.strength[bit].s1];
		  str[1] = str_char2_table[value->value.strength[bit].s1];
		  str[2] = 'H';
	    } ଅନ୍ୟଥା ଯଦି (value->value.strength[bit].s1 == 1) {
		  str[0] = str_char1_table[value->value.strength[bit].s0];
		  str[1] = str_char2_table[value->value.strength[bit].s0];
		  str[2] = 'L';
	    } ଅନ୍ୟଥା ଯଦି (value->value.strength[bit].s1 ==
		       value->value.strength[bit].s0) {
		  str[0] = str_char1_table[value->value.strength[bit].s0];
		  str[1] = str_char2_table[value->value.strength[bit].s0];
		  str[2] = 'X';
	    } ଅନ୍ୟଥା {
		  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ss;

		  str[0] = '0';
		  ss = value->value.strength[bit].s0;
		  ଯେତେବେଳେ (ss > 1) {
			str[0] += 1;
			ss >>= 1;
		  }
		  str[1] = '0';
		  ss = value->value.strength[bit].s1;
		  ଯେତେବେଳେ (ss > 1) {
			str[1] += 1;
			ss >>= 1;
		  }
		  str[2] = 'X';
	    }
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି vpiZ:
	    str[0] = 'H';
	    str[1] = 'i';
	    str[2] = 'Z';
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Unsupported type %d.\n",
	            (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)value->value.strength[bit].logic);
	    ପ୍ରତିଜ୍ଞା(0);
      }
}

ସ୍ଥିର ଅକ୍ଷର* vpip_format_pretty_string(ଅଚଳ ଅକ୍ଷର*ସନ୍ଦେଶ)
{
      ଫେରନ୍ତୁ strdup(ସନ୍ଦେଶ);
}

ସ୍ଥିର ଅକ୍ଷର* format_darray_pretty(vvp_darray* aobj)
{
      ଯଦି (!aobj || aobj->get_size() == 0) ଫେରନ୍ତୁ strdup("{}");

      std::string out = "{";

      ପାଇଁ (size_t i = 0; i < aobj->get_size(); i += 1) {
	    ଯଦି (i > 0) out += ", ";
	    ଯଦି (dynamic_cast<vvp_darray_real*>(aobj)) {
		  ଦ୍ୱିଗୁଣ d;
		  aobj->get_word((unsigned) i, d);
		  ଅକ୍ଷର buf[256];
		  snprintf(buf, ଆକାର buf, "%g", d);
		  out += buf;
	    } ଅନ୍ୟଥା ଯଦି (dynamic_cast<vvp_darray_string*>(aobj)) {
		  std::string s;
		  aobj->get_word((unsigned) i, s);
		  out += "\"";
		  out += s;
		  out += "\"";
	    } ଅନ୍ୟଥା {
		  vvp_vector4_t v;
		  aobj->get_word((unsigned) i, v);
		  s_vpi_ମୂଲ୍ୟ val;
		  val.format = vpiDecStrVal;
		  vpip_vec4_get_ମୂଲ୍ୟ(v, v.size(), ମିଥ୍ୟା, &val);
		  out += val.value.str;
	    }
      }

      out += "}";
      ଫେରନ୍ତୁ strdup(out.c_str());
}

ବାହ୍ୟ "C" ଅକ୍ଷର* vpip_format_pretty(vpiHandle ref)
{
      ଯଦି (!ref) {
	    ଫେରନ୍ତୁ vpip_format_pretty_string("Handle is NULL");
	  }

      ଯଦି (dynamic_cast<__vpiPropQueueRef*>(ref) ||
          dynamic_cast<__vpiDarrayVar*>(ref)) {
	    vvp_darray* aobj = vpip_vpi_darray_from_handle(ref);
	    ଯଦି (!aobj) {
		  ଫେରନ୍ତୁ vpip_format_pretty_string("Object not found");
		}
	    ଫେରନ୍ତୁ format_darray_pretty(aobj);
      }

      ଫେରନ୍ତୁ vpip_format_pretty_string("Unsupported object type");
}
