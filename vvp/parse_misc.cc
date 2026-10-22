/*
 * Copyright (c) 2001-2025  Stephen Williams (steve@icarus.com)
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

# include  "parse_misc.h"
# include  "compile.h"
# include  <cstdio>
# include  <cstdlib>
# include  "ivl_alloc.h"

స్థిరరాశి అక్షరం*yypath;
గుర్తులేని yyline;

శూన్య yyerror(స్థిరరాశి అక్షరం*సందేశం)
{
      fprintf(stderr, "%s:%u: %s\n", yypath, yyline, సందేశం);
}

శూన్య symbv_init(నిర్మాణం symbv_s*obj)
{
      obj->cnt = 0;
      obj->vect = 0;
}

శూన్య symbv_కూడు(నిర్మాణం symbv_s*obj, నిర్మాణం symb_s అంశం)
{
      obj->vect = static_cast<నిర్మాణం symb_s*>
                             (realloc(obj->vect, (obj->cnt+1) * పరిమాణం(నిర్మాణం symb_s)));
      obj->vect[obj->cnt] = అంశం;
      obj->cnt += 1;
}

శూన్య numbv_init(నిర్మాణం numbv_s*obj)
{
      obj->cnt = 0;
      obj->nvec = 0;
}

శూన్య numbv_కూడు(నిర్మాణం numbv_s*obj, దీర్ఘ అంశం)
{
      obj->nvec = static_cast<దీర్ఘ*>(realloc(obj->nvec, (obj->cnt+1) * పరిమాణం(దీర్ఘ)));
      obj->nvec[obj->cnt] = అంశం;
      obj->cnt += 1;
}

శూన్య numbv_clear(నిర్మాణం numbv_s*obj)
{
      విడుదల(obj->nvec);
      obj->nvec = 0;
      obj->cnt = 0;
}

శూన్య argv_init(నిర్మాణం argv_s*obj)
{
      obj->argc = 0;
      obj->argv = 0;
      obj->syms = 0;
}

శూన్య argv_కూడు(నిర్మాణం argv_s*obj, vpiHandle అంశం)
{
      obj->argv = static_cast<vpiHandle*>
                             (realloc(obj->argv, (obj->argc+1)*పరిమాణం(vpiHandle)));
      obj->argv[obj->argc] = అంశం;
      obj->argc += 1;
}

శూన్య argv_sym_కూడు(నిర్మాణం argv_s*obj, అక్షరం *అంశం)
{
      argv_కూడు(obj, 0x0);
      obj->syms = static_cast<అక్షరం**>
                             (realloc(obj->syms, (obj->argc)*పరిమాణం(అక్షరం*)));
      obj->syms[obj->argc-1] = అంశం;
}

శూన్య argv_sym_lookup(నిర్మాణం argv_s*obj)
{
      ఒకవేళ (!obj->syms)
	    తిరిగి;
      కొరకు (గుర్తులేని i=0; i < obj->argc; i++)
	    ఒకవేళ (!obj->argv[i])
		  compile_vpi_lookup(&obj->argv[i], obj->syms[i]);
      విడుదల(obj->syms);
}
