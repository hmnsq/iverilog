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

സ്ഥിരം അക്ഷരം*yypath;
unsigned yyline;

ശൂന്യം yyerror(സ്ഥിരം അക്ഷരം*സന്ദേശം)
{
      fprintf(stderr, "%s:%u: %s\n", yypath, yyline, സന്ദേശം);
}

ശൂന്യം symbv_init(ഘടന symbv_s*obj)
{
      obj->cnt = 0;
      obj->vect = 0;
}

ശൂന്യം symbv_add(ഘടന symbv_s*obj, ഘടന symb_s item)
{
      obj->vect = static_cast<ഘടന symb_s*>
                             (realloc(obj->vect, (obj->cnt+1) * വലുപ്പം(ഘടന symb_s)));
      obj->vect[obj->cnt] = item;
      obj->cnt += 1;
}

ശൂന്യം numbv_init(ഘടന numbv_s*obj)
{
      obj->cnt = 0;
      obj->nvec = 0;
}

ശൂന്യം numbv_add(ഘടന numbv_s*obj, long item)
{
      obj->nvec = static_cast<long*>(realloc(obj->nvec, (obj->cnt+1) * വലുപ്പം(long)));
      obj->nvec[obj->cnt] = item;
      obj->cnt += 1;
}

ശൂന്യം numbv_clear(ഘടന numbv_s*obj)
{
      സ്വതന്ത്രം(obj->nvec);
      obj->nvec = 0;
      obj->cnt = 0;
}

ശൂന്യം argv_init(ഘടന argv_s*obj)
{
      obj->argc = 0;
      obj->argv = 0;
      obj->syms = 0;
}

ശൂന്യം argv_add(ഘടന argv_s*obj, vpiHandle item)
{
      obj->argv = static_cast<vpiHandle*>
                             (realloc(obj->argv, (obj->argc+1)*വലുപ്പം(vpiHandle)));
      obj->argv[obj->argc] = item;
      obj->argc += 1;
}

ശൂന്യം argv_sym_add(ഘടന argv_s*obj, അക്ഷരം *item)
{
      argv_add(obj, 0x0);
      obj->syms = static_cast<അക്ഷരം**>
                             (realloc(obj->syms, (obj->argc)*വലുപ്പം(അക്ഷരം*)));
      obj->syms[obj->argc-1] = item;
}

ശൂന്യം argv_sym_lookup(ഘടന argv_s*obj)
{
      എങ്കിൽ (!obj->syms)
	    തിരികെ;
      വേണ്ടി (unsigned i=0; i < obj->argc; i++)
	    എങ്കിൽ (!obj->argv[i])
		  compile_vpi_lookup(&obj->argv[i], obj->syms[i]);
      സ്വതന്ത്രം(obj->syms);
}
