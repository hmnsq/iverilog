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

ਸਥਿਰ ਅਚਲ ਅੱਖਰ str_char1_ਸਾਰਣੀ[257] = {
      ".HS1M222" "W3333333" /* 00 0F */ "L4444444" "44444444" /* 10 1F */
      "P5555555" "55555555" /* 20 2F */ "55555555" "55555555" /* 30 3F */
      "S6666666" "66666666" /* 40 4F */ "66666666" "66666666" /* 50 5F */
      "66666666" "66666666" /* 60 6F */ "66666666" "66666666" /* 70 7F */
      "S7777777" "77777777" /* 80 8F */ "77777777" "77777777" /* 90 9F */
      "77777777" "77777777" /* A0 AF */ "77777777" "77777777" /* B0 BF */
      "77777777" "77777777" /* C0 CF */ "77777777" "77777777" /* D0 DF */
      "77777777" "77777777" /* E0 EF */ "77777777" "77777777" /* F0 FF */ };

ਸਥਿਰ ਅਚਲ ਅੱਖਰ str_char2_ਸਾਰਣੀ[257] = {
      ".im0e010" "e0102010" /* 00 0F */ "a0102010" "30102010" /* 10 1F */
      "u0102010" "30102010" /* 20 2F */ "40102010" "30102010" /* 30 3F */
      "t0102010" "30102010" /* 40 4F */ "40102010" "30102010" /* 50 5F */
      "50102010" "30102010" /* 60 6F */ "40102010" "30102010" /* 70 7F */
      "u0102010" "30102010" /* 80 8F */ "40102010" "30102010" /* 90 9F */
      "50102010" "30102010" /* A0 AF */ "40102010" "30102010" /* B0 BF */
      "60102010" "30102010" /* C0 CF */ "40102010" "30102010" /* D0 DF */
      "50102010" "30102010" /* E0 EF */ "40102010" "30102010" /* F0 FF */ };


ਬਾਹਰੀ "C" ਖਾਲੀ vpip_format_strength(ਅੱਖਰ*str, s_vpi_ਮੁੱਲ*value, ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      str[0] = '.';
      str[1] = '.';
      str[2] = '.';
      str[3] = 0;

      ਪ੍ਰਤਿਜ੍ਞਾ(value->format == vpiStrengthVal);

      ਚੋਣ (value->value.strength[ਦੋਆਧਾਰੀ_ਅੰਕ].logic) {
	  ਹਾਲਤ vpi0:
	    str[0] = str_char1_ਸਾਰਣੀ[value->value.strength[ਦੋਆਧਾਰੀ_ਅੰਕ].s0];
	    str[1] = str_char2_ਸਾਰਣੀ[value->value.strength[ਦੋਆਧਾਰੀ_ਅੰਕ].s0];
	    str[2] = '0';
	    ਰੋਕੋ;
	  ਹਾਲਤ vpi1:
	    str[0] = str_char1_ਸਾਰਣੀ[value->value.strength[ਦੋਆਧਾਰੀ_ਅੰਕ].s1];
	    str[1] = str_char2_ਸਾਰਣੀ[value->value.strength[ਦੋਆਧਾਰੀ_ਅੰਕ].s1];
	    str[2] = '1';
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiX:
	    ਜੇ (value->value.strength[ਦੋਆਧਾਰੀ_ਅੰਕ].s0 == 1) {
		  str[0] = str_char1_ਸਾਰਣੀ[value->value.strength[ਦੋਆਧਾਰੀ_ਅੰਕ].s1];
		  str[1] = str_char2_ਸਾਰਣੀ[value->value.strength[ਦੋਆਧਾਰੀ_ਅੰਕ].s1];
		  str[2] = 'H';
	    } ਨਹੀਂਤਾਂ ਜੇ (value->value.strength[ਦੋਆਧਾਰੀ_ਅੰਕ].s1 == 1) {
		  str[0] = str_char1_ਸਾਰਣੀ[value->value.strength[ਦੋਆਧਾਰੀ_ਅੰਕ].s0];
		  str[1] = str_char2_ਸਾਰਣੀ[value->value.strength[ਦੋਆਧਾਰੀ_ਅੰਕ].s0];
		  str[2] = 'L';
	    } ਨਹੀਂਤਾਂ ਜੇ (value->value.strength[ਦੋਆਧਾਰੀ_ਅੰਕ].s1 ==
		       value->value.strength[ਦੋਆਧਾਰੀ_ਅੰਕ].s0) {
		  str[0] = str_char1_ਸਾਰਣੀ[value->value.strength[ਦੋਆਧਾਰੀ_ਅੰਕ].s0];
		  str[1] = str_char2_ਸਾਰਣੀ[value->value.strength[ਦੋਆਧਾਰੀ_ਅੰਕ].s0];
		  str[2] = 'X';
	    } ਨਹੀਂਤਾਂ {
		  ਪੂਰਨ ss;

		  str[0] = '0';
		  ss = value->value.strength[ਦੋਆਧਾਰੀ_ਅੰਕ].s0;
		  ਜਦੋਂ (ss > 1) {
			str[0] += 1;
			ss >>= 1;
		  }
		  str[1] = '0';
		  ss = value->value.strength[ਦੋਆਧਾਰੀ_ਅੰਕ].s1;
		  ਜਦੋਂ (ss > 1) {
			str[1] += 1;
			ss >>= 1;
		  }
		  str[2] = 'X';
	    }
	    ਰੋਕੋ;
	  ਹਾਲਤ vpiZ:
	    str[0] = 'H';
	    str[1] = 'i';
	    str[2] = 'Z';
	    ਰੋਕੋ;
	  ਮੂਲ:
	    fprintf(stderr, "Unsupported type %d.\n",
	            (ਪੂਰਨ)value->value.strength[ਦੋਆਧਾਰੀ_ਅੰਕ].logic);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }
}

ਸਥਿਰ ਅੱਖਰ* vpip_format_pretty_string(ਅਚਲ ਅੱਖਰ*ਸੁਨੇਹਾ)
{
      ਵਾਪਸ strdup(ਸੁਨੇਹਾ);
}

ਸਥਿਰ ਅੱਖਰ* format_darray_pretty(vvp_darray* aobj)
{
      ਜੇ (!aobj || aobj->get_ਮਾਪ() == 0) ਵਾਪਸ strdup("{}");

      std::string out = "{";

      ਲਈ (size_t i = 0; i < aobj->get_ਮਾਪ(); i += 1) {
	    ਜੇ (i > 0) out += ", ";
	    ਜੇ (dynamic_cast<vvp_darray_real*>(aobj)) {
		  ਦੁੱਗਣਾ d;
		  aobj->get_ਸ਼ਬਦ((ਬਿਨਾਨਿਸ਼ਾਨੀ) i, d);
		  ਅੱਖਰ buf[256];
		  snprintf(buf, ਆਕਾਰ buf, "%g", d);
		  out += buf;
	    } ਨਹੀਂਤਾਂ ਜੇ (dynamic_cast<vvp_darray_string*>(aobj)) {
		  std::string s;
		  aobj->get_ਸ਼ਬਦ((ਬਿਨਾਨਿਸ਼ਾਨੀ) i, s);
		  out += "\"";
		  out += s;
		  out += "\"";
	    } ਨਹੀਂਤਾਂ {
		  vvp_vector4_t v;
		  aobj->get_ਸ਼ਬਦ((ਬਿਨਾਨਿਸ਼ਾਨੀ) i, v);
		  s_vpi_ਮੁੱਲ val;
		  val.format = vpiDecStrVal;
		  vpip_vec4_get_ਮੁੱਲ(v, v.size(), ਝੂਠ, &val);
		  out += val.value.str;
	    }
      }

      out += "}";
      ਵਾਪਸ strdup(out.c_str());
}

ਬਾਹਰੀ "C" ਅੱਖਰ* vpip_format_pretty(vpiHandle ref)
{
      ਜੇ (!ref) {
	    ਵਾਪਸ vpip_format_pretty_string("Handle is NULL");
	  }

      ਜੇ (dynamic_cast<__vpiPropQueueRef*>(ref) ||
          dynamic_cast<__vpiDarrayVar*>(ref)) {
	    vvp_darray* aobj = vpip_vpi_darray_from_handle(ref);
	    ਜੇ (!aobj) {
		  ਵਾਪਸ vpip_format_pretty_string("Object not found");
		}
	    ਵਾਪਸ format_darray_pretty(aobj);
      }

      ਵਾਪਸ vpip_format_pretty_string("Unsupported object type");
}
