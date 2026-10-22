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

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ str_char1_ಕೋಷ್ಟಕ[257] = {
      ".HS1M222" "W3333333" /* 00 0F */ "L4444444" "44444444" /* 10 1F */
      "P5555555" "55555555" /* 20 2F */ "55555555" "55555555" /* 30 3F */
      "S6666666" "66666666" /* 40 4F */ "66666666" "66666666" /* 50 5F */
      "66666666" "66666666" /* 60 6F */ "66666666" "66666666" /* 70 7F */
      "S7777777" "77777777" /* 80 8F */ "77777777" "77777777" /* 90 9F */
      "77777777" "77777777" /* A0 AF */ "77777777" "77777777" /* B0 BF */
      "77777777" "77777777" /* C0 CF */ "77777777" "77777777" /* D0 DF */
      "77777777" "77777777" /* E0 EF */ "77777777" "77777777" /* F0 FF */ };

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ str_char2_ಕೋಷ್ಟಕ[257] = {
      ".im0e010" "e0102010" /* 00 0F */ "a0102010" "30102010" /* 10 1F */
      "u0102010" "30102010" /* 20 2F */ "40102010" "30102010" /* 30 3F */
      "t0102010" "30102010" /* 40 4F */ "40102010" "30102010" /* 50 5F */
      "50102010" "30102010" /* 60 6F */ "40102010" "30102010" /* 70 7F */
      "u0102010" "30102010" /* 80 8F */ "40102010" "30102010" /* 90 9F */
      "50102010" "30102010" /* A0 AF */ "40102010" "30102010" /* B0 BF */
      "60102010" "30102010" /* C0 CF */ "40102010" "30102010" /* D0 DF */
      "50102010" "30102010" /* E0 EF */ "40102010" "30102010" /* F0 FF */ };


ಬಾಹ್ಯ "C" ಶೂನ್ಯ vpip_format_strength(ಅಕ್ಷರ*str, s_vpi_ಮೌಲ್ಯ*value, ಚಿಹ್ನೆರಹಿತ ಬಿಟ್)
{
      str[0] = '.';
      str[1] = '.';
      str[2] = '.';
      str[3] = 0;

      ಪ್ರತಿಜ್ಞಾ(value->format == vpiStrengthVal);

      ಆಯ್ಕೆ (value->value.strength[ಬಿಟ್].logic) {
	  ಸ್ಥಿತಿ vpi0:
	    str[0] = str_char1_ಕೋಷ್ಟಕ[value->value.strength[ಬಿಟ್].s0];
	    str[1] = str_char2_ಕೋಷ್ಟಕ[value->value.strength[ಬಿಟ್].s0];
	    str[2] = '0';
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpi1:
	    str[0] = str_char1_ಕೋಷ್ಟಕ[value->value.strength[ಬಿಟ್].s1];
	    str[1] = str_char2_ಕೋಷ್ಟಕ[value->value.strength[ಬಿಟ್].s1];
	    str[2] = '1';
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiX:
	    ಆದರೆ (value->value.strength[ಬಿಟ್].s0 == 1) {
		  str[0] = str_char1_ಕೋಷ್ಟಕ[value->value.strength[ಬಿಟ್].s1];
		  str[1] = str_char2_ಕೋಷ್ಟಕ[value->value.strength[ಬಿಟ್].s1];
		  str[2] = 'H';
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (value->value.strength[ಬಿಟ್].s1 == 1) {
		  str[0] = str_char1_ಕೋಷ್ಟಕ[value->value.strength[ಬಿಟ್].s0];
		  str[1] = str_char2_ಕೋಷ್ಟಕ[value->value.strength[ಬಿಟ್].s0];
		  str[2] = 'L';
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (value->value.strength[ಬಿಟ್].s1 ==
		       value->value.strength[ಬಿಟ್].s0) {
		  str[0] = str_char1_ಕೋಷ್ಟಕ[value->value.strength[ಬಿಟ್].s0];
		  str[1] = str_char2_ಕೋಷ್ಟಕ[value->value.strength[ಬಿಟ್].s0];
		  str[2] = 'X';
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ಪೂರ್ಣಾಂಕ ss;

		  str[0] = '0';
		  ss = value->value.strength[ಬಿಟ್].s0;
		  ಆಗ (ss > 1) {
			str[0] += 1;
			ss >>= 1;
		  }
		  str[1] = '0';
		  ss = value->value.strength[ಬಿಟ್].s1;
		  ಆಗ (ss > 1) {
			str[1] += 1;
			ss >>= 1;
		  }
		  str[2] = 'X';
	    }
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ vpiZ:
	    str[0] = 'H';
	    str[1] = 'i';
	    str[2] = 'Z';
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Unsupported type %d.\n",
	            (ಪೂರ್ಣಾಂಕ)value->value.strength[ಬಿಟ್].logic);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }
}

ಸ್ಥಿರ ಅಕ್ಷರ* vpip_format_pretty_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಸಂದೇಶ)
{
      ಹಿಂತಿರುಗಿ strdup(ಸಂದೇಶ);
}

ಸ್ಥಿರ ಅಕ್ಷರ* format_darray_pretty(vvp_darray* aobj)
{
      ಆದರೆ (!aobj || aobj->get_ಗಾತ್ರ() == 0) ಹಿಂತಿರುಗಿ strdup("{}");

      std::string out = "{";

      ಗಾಗಿ (size_t i = 0; i < aobj->get_ಗಾತ್ರ(); i += 1) {
	    ಆದರೆ (i > 0) out += ", ";
	    ಆದರೆ (dynamic_cast<vvp_darray_real*>(aobj)) {
		  ದ್ವಿಗುಣ d;
		  aobj->get_ಶಬ್ದ((ಚಿಹ್ನೆರಹಿತ) i, d);
		  ಅಕ್ಷರ buf[256];
		  snprintf(buf, ಗಾತ್ರ buf, "%g", d);
		  out += buf;
	    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dynamic_cast<vvp_darray_string*>(aobj)) {
		  std::string s;
		  aobj->get_ಶಬ್ದ((ಚಿಹ್ನೆರಹಿತ) i, s);
		  out += "\"";
		  out += s;
		  out += "\"";
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  vvp_vector4_t v;
		  aobj->get_ಶಬ್ದ((ಚಿಹ್ನೆರಹಿತ) i, v);
		  s_vpi_ಮೌಲ್ಯ val;
		  val.format = vpiDecStrVal;
		  vpip_vec4_get_ಮೌಲ್ಯ(v, v.size(), ಸುಳ್ಳು, &val);
		  out += val.value.str;
	    }
      }

      out += "}";
      ಹಿಂತಿರುಗಿ strdup(out.c_str());
}

ಬಾಹ್ಯ "C" ಅಕ್ಷರ* vpip_format_pretty(vpiHandle ref)
{
      ಆದರೆ (!ref) {
	    ಹಿಂತಿರುಗಿ vpip_format_pretty_string("Handle is NULL");
	  }

      ಆದರೆ (dynamic_cast<__vpiPropQueueRef*>(ref) ||
          dynamic_cast<__vpiDarrayVar*>(ref)) {
	    vvp_darray* aobj = vpip_vpi_darray_from_handle(ref);
	    ಆದರೆ (!aobj) {
		  ಹಿಂತಿರುಗಿ vpip_format_pretty_string("Object not found");
		}
	    ಹಿಂತಿರುಗಿ format_darray_pretty(aobj);
      }

      ಹಿಂತಿರುಗಿ vpip_format_pretty_string("Unsupported object type");
}
