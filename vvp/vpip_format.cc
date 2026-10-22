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

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ str_char1_table[257] = {
      ".HS1M222" "W3333333" /* 00 0F */ "L4444444" "44444444" /* 10 1F */
      "P5555555" "55555555" /* 20 2F */ "55555555" "55555555" /* 30 3F */
      "S6666666" "66666666" /* 40 4F */ "66666666" "66666666" /* 50 5F */
      "66666666" "66666666" /* 60 6F */ "66666666" "66666666" /* 70 7F */
      "S7777777" "77777777" /* 80 8F */ "77777777" "77777777" /* 90 9F */
      "77777777" "77777777" /* A0 AF */ "77777777" "77777777" /* B0 BF */
      "77777777" "77777777" /* C0 CF */ "77777777" "77777777" /* D0 DF */
      "77777777" "77777777" /* E0 EF */ "77777777" "77777777" /* F0 FF */ };

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ str_char2_table[257] = {
      ".im0e010" "e0102010" /* 00 0F */ "a0102010" "30102010" /* 10 1F */
      "u0102010" "30102010" /* 20 2F */ "40102010" "30102010" /* 30 3F */
      "t0102010" "30102010" /* 40 4F */ "40102010" "30102010" /* 50 5F */
      "50102010" "30102010" /* 60 6F */ "40102010" "30102010" /* 70 7F */
      "u0102010" "30102010" /* 80 8F */ "40102010" "30102010" /* 90 9F */
      "50102010" "30102010" /* A0 AF */ "40102010" "30102010" /* B0 BF */
      "60102010" "30102010" /* C0 CF */ "40102010" "30102010" /* D0 DF */
      "50102010" "30102010" /* E0 EF */ "40102010" "30102010" /* F0 FF */ };


ಬಾಹ್ಯ "C" ಶೂನ್ಯ vpip_format_strength(ವರ್ಣ*str, s_vpi_ಮೌಲ್ಯ*value, unsigned bit)
{
      str[0] = '.';
      str[1] = '.';
      str[2] = '.';
      str[3] = 0;

      ಪ್ರತಿಜ್ಞಾ(value->format == vpiStrengthVal);

      ವಿಕಲ್ಪ (value->value.strength[bit].logic) {
	  ಪಕ್ಷ vpi0:
	    str[0] = str_char1_table[value->value.strength[bit].s0];
	    str[1] = str_char2_table[value->value.strength[bit].s0];
	    str[2] = '0';
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpi1:
	    str[0] = str_char1_table[value->value.strength[bit].s1];
	    str[1] = str_char2_table[value->value.strength[bit].s1];
	    str[2] = '1';
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiX:
	    ಯದಿ (value->value.strength[bit].s0 == 1) {
		  str[0] = str_char1_table[value->value.strength[bit].s1];
		  str[1] = str_char2_table[value->value.strength[bit].s1];
		  str[2] = 'H';
	    } ಅನ್ಯಥಾ ಯದಿ (value->value.strength[bit].s1 == 1) {
		  str[0] = str_char1_table[value->value.strength[bit].s0];
		  str[1] = str_char2_table[value->value.strength[bit].s0];
		  str[2] = 'L';
	    } ಅನ್ಯಥಾ ಯದಿ (value->value.strength[bit].s1 ==
		       value->value.strength[bit].s0) {
		  str[0] = str_char1_table[value->value.strength[bit].s0];
		  str[1] = str_char2_table[value->value.strength[bit].s0];
		  str[2] = 'X';
	    } ಅನ್ಯಥಾ {
		  ಪೂರ್ಣಾಙ್ಕ ss;

		  str[0] = '0';
		  ss = value->value.strength[bit].s0;
		  ಯಾವತ್ (ss > 1) {
			str[0] += 1;
			ss >>= 1;
		  }
		  str[1] = '0';
		  ss = value->value.strength[bit].s1;
		  ಯಾವತ್ (ss > 1) {
			str[1] += 1;
			ss >>= 1;
		  }
		  str[2] = 'X';
	    }
	    ಭಙ್ಗ;
	  ಪಕ್ಷ vpiZ:
	    str[0] = 'H';
	    str[1] = 'i';
	    str[2] = 'Z';
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    fprintf(stderr, "Unsupported type %d.\n",
	            (ಪೂರ್ಣಾಙ್ಕ)value->value.strength[bit].logic);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }
}

ಸ್ಥಿರ ವರ್ಣ* vpip_format_pretty_string(ಧ್ರುವ ವರ್ಣ*ಸಂದೇಶ)
{
      ಪ್ರತ್ಯಾಗಮ strdup(ಸಂದೇಶ);
}

ಸ್ಥಿರ ವರ್ಣ* format_darray_pretty(vvp_darray* aobj)
{
      ಯದಿ (!aobj || aobj->get_size() == 0) ಪ್ರತ್ಯಾಗಮ strdup("{}");

      std::string out = "{";

      ಪ್ರತಿ (size_t i = 0; i < aobj->get_size(); i += 1) {
	    ಯದಿ (i > 0) out += ", ";
	    ಯದಿ (dynamic_cast<vvp_darray_real*>(aobj)) {
		  ದ್ವಿಗುಣ d;
		  aobj->get_word((unsigned) i, d);
		  ವರ್ಣ buf[256];
		  snprintf(buf, ಪರಿಮಾಣ buf, "%g", d);
		  out += buf;
	    } ಅನ್ಯಥಾ ಯದಿ (dynamic_cast<vvp_darray_string*>(aobj)) {
		  std::string s;
		  aobj->get_word((unsigned) i, s);
		  out += "\"";
		  out += s;
		  out += "\"";
	    } ಅನ್ಯಥಾ {
		  vvp_vector4_t v;
		  aobj->get_word((unsigned) i, v);
		  s_vpi_ಮೌಲ್ಯ val;
		  val.format = vpiDecStrVal;
		  vpip_vec4_get_ಮೌಲ್ಯ(v, v.size(), ಅಸತ್ಯ, &val);
		  out += val.value.str;
	    }
      }

      out += "}";
      ಪ್ರತ್ಯಾಗಮ strdup(out.c_str());
}

ಬಾಹ್ಯ "C" ವರ್ಣ* vpip_format_pretty(vpiHandle ref)
{
      ಯದಿ (!ref) {
	    ಪ್ರತ್ಯಾಗಮ vpip_format_pretty_string("Handle is NULL");
	  }

      ಯದಿ (dynamic_cast<__vpiPropQueueRef*>(ref) ||
          dynamic_cast<__vpiDarrayVar*>(ref)) {
	    vvp_darray* aobj = vpip_vpi_darray_from_handle(ref);
	    ಯದಿ (!aobj) {
		  ಪ್ರತ್ಯಾಗಮ vpip_format_pretty_string("Object not found");
		}
	    ಪ್ರತ್ಯಾಗಮ format_darray_pretty(aobj);
      }

      ಪ್ರತ್ಯಾಗಮ vpip_format_pretty_string("Unsupported object type");
}
