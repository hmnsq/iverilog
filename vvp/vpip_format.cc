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

స్థిర స్థిరరాశి అక్షరం str_char1_పట్టిక[257] = {
      ".HS1M222" "W3333333" /* 00 0F */ "L4444444" "44444444" /* 10 1F */
      "P5555555" "55555555" /* 20 2F */ "55555555" "55555555" /* 30 3F */
      "S6666666" "66666666" /* 40 4F */ "66666666" "66666666" /* 50 5F */
      "66666666" "66666666" /* 60 6F */ "66666666" "66666666" /* 70 7F */
      "S7777777" "77777777" /* 80 8F */ "77777777" "77777777" /* 90 9F */
      "77777777" "77777777" /* A0 AF */ "77777777" "77777777" /* B0 BF */
      "77777777" "77777777" /* C0 CF */ "77777777" "77777777" /* D0 DF */
      "77777777" "77777777" /* E0 EF */ "77777777" "77777777" /* F0 FF */ };

స్థిర స్థిరరాశి అక్షరం str_char2_పట్టిక[257] = {
      ".im0e010" "e0102010" /* 00 0F */ "a0102010" "30102010" /* 10 1F */
      "u0102010" "30102010" /* 20 2F */ "40102010" "30102010" /* 30 3F */
      "t0102010" "30102010" /* 40 4F */ "40102010" "30102010" /* 50 5F */
      "50102010" "30102010" /* 60 6F */ "40102010" "30102010" /* 70 7F */
      "u0102010" "30102010" /* 80 8F */ "40102010" "30102010" /* 90 9F */
      "50102010" "30102010" /* A0 AF */ "40102010" "30102010" /* B0 BF */
      "60102010" "30102010" /* C0 CF */ "40102010" "30102010" /* D0 DF */
      "50102010" "30102010" /* E0 EF */ "40102010" "30102010" /* F0 FF */ };


బాహ్య "C" శూన్య vpip_format_strength(అక్షరం*str, s_vpi_విలువ*value, గుర్తులేని బిట్టు)
{
      str[0] = '.';
      str[1] = '.';
      str[2] = '.';
      str[3] = 0;

      ప్రతిజ్ఞా(value->format == vpiStrengthVal);

      ఎంపిక (value->value.strength[బిట్టు].logic) {
	  స్థితి vpi0:
	    str[0] = str_char1_పట్టిక[value->value.strength[బిట్టు].s0];
	    str[1] = str_char2_పట్టిక[value->value.strength[బిట్టు].s0];
	    str[2] = '0';
	    ఆపు;
	  స్థితి vpi1:
	    str[0] = str_char1_పట్టిక[value->value.strength[బిట్టు].s1];
	    str[1] = str_char2_పట్టిక[value->value.strength[బిట్టు].s1];
	    str[2] = '1';
	    ఆపు;
	  స్థితి vpiX:
	    ఒకవేళ (value->value.strength[బిట్టు].s0 == 1) {
		  str[0] = str_char1_పట్టిక[value->value.strength[బిట్టు].s1];
		  str[1] = str_char2_పట్టిక[value->value.strength[బిట్టు].s1];
		  str[2] = 'H';
	    } లేకపోతే ఒకవేళ (value->value.strength[బిట్టు].s1 == 1) {
		  str[0] = str_char1_పట్టిక[value->value.strength[బిట్టు].s0];
		  str[1] = str_char2_పట్టిక[value->value.strength[బిట్టు].s0];
		  str[2] = 'L';
	    } లేకపోతే ఒకవేళ (value->value.strength[బిట్టు].s1 ==
		       value->value.strength[బిట్టు].s0) {
		  str[0] = str_char1_పట్టిక[value->value.strength[బిట్టు].s0];
		  str[1] = str_char2_పట్టిక[value->value.strength[బిట్టు].s0];
		  str[2] = 'X';
	    } లేకపోతే {
		  పూర్ణాంకం ss;

		  str[0] = '0';
		  ss = value->value.strength[బిట్టు].s0;
		  అయితే (ss > 1) {
			str[0] += 1;
			ss >>= 1;
		  }
		  str[1] = '0';
		  ss = value->value.strength[బిట్టు].s1;
		  అయితే (ss > 1) {
			str[1] += 1;
			ss >>= 1;
		  }
		  str[2] = 'X';
	    }
	    ఆపు;
	  స్థితి vpiZ:
	    str[0] = 'H';
	    str[1] = 'i';
	    str[2] = 'Z';
	    ఆపు;
	  అప్రమేయ:
	    fprintf(stderr, "Unsupported type %d.\n",
	            (పూర్ణాంకం)value->value.strength[బిట్టు].logic);
	    ప్రతిజ్ఞా(0);
      }
}

స్థిర అక్షరం* vpip_format_pretty_string(స్థిరరాశి అక్షరం*సందేశం)
{
      తిరిగి strdup(సందేశం);
}

స్థిర అక్షరం* format_darray_pretty(vvp_darray* aobj)
{
      ఒకవేళ (!aobj || aobj->get_కొలత() == 0) తిరిగి strdup("{}");

      std::string out = "{";

      కొరకు (size_t i = 0; i < aobj->get_కొలత(); i += 1) {
	    ఒకవేళ (i > 0) out += ", ";
	    ఒకవేళ (dynamic_cast<vvp_darray_real*>(aobj)) {
		  ద్విగుణ d;
		  aobj->get_మాట((గుర్తులేని) i, d);
		  అక్షరం buf[256];
		  snprintf(buf, పరిమాణం buf, "%g", d);
		  out += buf;
	    } లేకపోతే ఒకవేళ (dynamic_cast<vvp_darray_string*>(aobj)) {
		  std::string s;
		  aobj->get_మాట((గుర్తులేని) i, s);
		  out += "\"";
		  out += s;
		  out += "\"";
	    } లేకపోతే {
		  vvp_vector4_t v;
		  aobj->get_మాట((గుర్తులేని) i, v);
		  s_vpi_విలువ val;
		  val.format = vpiDecStrVal;
		  vpip_vec4_get_విలువ(v, v.size(), అబద్ధం, &val);
		  out += val.value.str;
	    }
      }

      out += "}";
      తిరిగి strdup(out.c_str());
}

బాహ్య "C" అక్షరం* vpip_format_pretty(vpiHandle ref)
{
      ఒకవేళ (!ref) {
	    తిరిగి vpip_format_pretty_string("Handle is NULL");
	  }

      ఒకవేళ (dynamic_cast<__vpiPropQueueRef*>(ref) ||
          dynamic_cast<__vpiDarrayVar*>(ref)) {
	    vvp_darray* aobj = vpip_vpi_darray_from_handle(ref);
	    ఒకవేళ (!aobj) {
		  తిరిగి vpip_format_pretty_string("Object not found");
		}
	    తిరిగి format_darray_pretty(aobj);
      }

      తిరిగి vpip_format_pretty_string("Unsupported object type");
}
