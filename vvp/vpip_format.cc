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

ساكن ثابت حرف str_char1_جدول[257] = {
      ".HS1M222" "W3333333" /* 00 0F */ "L4444444" "44444444" /* 10 1F */
      "P5555555" "55555555" /* 20 2F */ "55555555" "55555555" /* 30 3F */
      "S6666666" "66666666" /* 40 4F */ "66666666" "66666666" /* 50 5F */
      "66666666" "66666666" /* 60 6F */ "66666666" "66666666" /* 70 7F */
      "S7777777" "77777777" /* 80 8F */ "77777777" "77777777" /* 90 9F */
      "77777777" "77777777" /* A0 AF */ "77777777" "77777777" /* B0 BF */
      "77777777" "77777777" /* C0 CF */ "77777777" "77777777" /* D0 DF */
      "77777777" "77777777" /* E0 EF */ "77777777" "77777777" /* F0 FF */ };

ساكن ثابت حرف str_char2_جدول[257] = {
      ".im0e010" "e0102010" /* 00 0F */ "a0102010" "30102010" /* 10 1F */
      "u0102010" "30102010" /* 20 2F */ "40102010" "30102010" /* 30 3F */
      "t0102010" "30102010" /* 40 4F */ "40102010" "30102010" /* 50 5F */
      "50102010" "30102010" /* 60 6F */ "40102010" "30102010" /* 70 7F */
      "u0102010" "30102010" /* 80 8F */ "40102010" "30102010" /* 90 9F */
      "50102010" "30102010" /* A0 AF */ "40102010" "30102010" /* B0 BF */
      "60102010" "30102010" /* C0 CF */ "40102010" "30102010" /* D0 DF */
      "50102010" "30102010" /* E0 EF */ "40102010" "30102010" /* F0 FF */ };


خارجي "C" فراغ vpip_format_strength(حرف*str, s_vpi_قدر*value, بنانشان ٻٽو_انگ)
{
      str[0] = '.';
      str[1] = '.';
      str[2] = '.';
      str[3] = 0;

      أكد(value->format == vpiStrengthVal);

      بدل (value->value.strength[ٻٽو_انگ].logic) {
	  حالة vpi0:
	    str[0] = str_char1_جدول[value->value.strength[ٻٽو_انگ].s0];
	    str[1] = str_char2_جدول[value->value.strength[ٻٽو_انگ].s0];
	    str[2] = '0';
	    اكسر;
	  حالة vpi1:
	    str[0] = str_char1_جدول[value->value.strength[ٻٽو_انگ].s1];
	    str[1] = str_char2_جدول[value->value.strength[ٻٽو_انگ].s1];
	    str[2] = '1';
	    اكسر;
	  حالة vpiX:
	    إذا (value->value.strength[ٻٽو_انگ].s0 == 1) {
		  str[0] = str_char1_جدول[value->value.strength[ٻٽو_انگ].s1];
		  str[1] = str_char2_جدول[value->value.strength[ٻٽو_انگ].s1];
		  str[2] = 'H';
	    } وإلا إذا (value->value.strength[ٻٽو_انگ].s1 == 1) {
		  str[0] = str_char1_جدول[value->value.strength[ٻٽو_انگ].s0];
		  str[1] = str_char2_جدول[value->value.strength[ٻٽو_انگ].s0];
		  str[2] = 'L';
	    } وإلا إذا (value->value.strength[ٻٽو_انگ].s1 ==
		       value->value.strength[ٻٽو_انگ].s0) {
		  str[0] = str_char1_جدول[value->value.strength[ٻٽو_انگ].s0];
		  str[1] = str_char2_جدول[value->value.strength[ٻٽو_انگ].s0];
		  str[2] = 'X';
	    } وإلا {
		  صحيح ss;

		  str[0] = '0';
		  ss = value->value.strength[ٻٽو_انگ].s0;
		  طالما (ss > 1) {
			str[0] += 1;
			ss >>= 1;
		  }
		  str[1] = '0';
		  ss = value->value.strength[ٻٽو_انگ].s1;
		  طالما (ss > 1) {
			str[1] += 1;
			ss >>= 1;
		  }
		  str[2] = 'X';
	    }
	    اكسر;
	  حالة vpiZ:
	    str[0] = 'H';
	    str[1] = 'i';
	    str[2] = 'Z';
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "Unsupported type %d.\n",
	            (صحيح)value->value.strength[ٻٽو_انگ].logic);
	    أكد(0);
      }
}

ساكن حرف* vpip_format_pretty_string(ثابت حرف*نياپو)
{
      أرجع strdup(نياپو);
}

ساكن حرف* format_darray_pretty(vvp_darray* aobj)
{
      إذا (!aobj || aobj->get_سائيز() == 0) أرجع strdup("{}");

      std::string out = "{";

      لكل (size_t i = 0; i < aobj->get_سائيز(); i += 1) {
	    إذا (i > 0) out += ", ";
	    إذا (dynamic_cast<vvp_darray_real*>(aobj)) {
		  مزدوج d;
		  aobj->get_لفظ((بنانشان) i, d);
		  حرف buf[256];
		  snprintf(buf, حجم buf, "%g", d);
		  out += buf;
	    } وإلا إذا (dynamic_cast<vvp_darray_string*>(aobj)) {
		  std::string s;
		  aobj->get_لفظ((بنانشان) i, s);
		  out += "\"";
		  out += s;
		  out += "\"";
	    } وإلا {
		  vvp_vector4_t v;
		  aobj->get_لفظ((بنانشان) i, v);
		  s_vpi_قدر val;
		  val.format = vpiDecStrVal;
		  vpip_vec4_get_قدر(v, v.size(), خطأ, &val);
		  out += val.value.str;
	    }
      }

      out += "}";
      أرجع strdup(out.c_str());
}

خارجي "C" حرف* vpip_format_pretty(vpiHandle ref)
{
      إذا (!ref) {
	    أرجع vpip_format_pretty_string("Handle is NULL");
	  }

      إذا (dynamic_cast<__vpiPropQueueRef*>(ref) ||
          dynamic_cast<__vpiDarrayVar*>(ref)) {
	    vvp_darray* aobj = vpip_vpi_darray_from_handle(ref);
	    إذا (!aobj) {
		  أرجع vpip_format_pretty_string("Object not found");
		}
	    أرجع format_darray_pretty(aobj);
      }

      أرجع vpip_format_pretty_string("Unsupported object type");
}
