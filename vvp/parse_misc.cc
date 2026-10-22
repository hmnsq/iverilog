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

const var_na*yypath;
unsigned yyline;

void yyerror(const var_na*msg)
{
      fprintf(stderr, "%s:%u: %s\n", yypath, yyline, msg);
}

void symbv_init(sa_nracanaa symbv_s*obj)
{
      obj->cnt = 0;
      obj->vect = 0;
}

void symbv_add(sa_nracanaa symbv_s*obj, sa_nracanaa symb_s item)
{
      obj->vect = static_cast<sa_nracanaa symb_s*>
                             (realloc(obj->vect, (obj->cnt+1) * parimaa_na(sa_nracanaa symb_s)));
      obj->vect[obj->cnt] = item;
      obj->cnt += 1;
}

void numbv_init(sa_nracanaa numbv_s*obj)
{
      obj->cnt = 0;
      obj->nvec = 0;
}

void numbv_add(sa_nracanaa numbv_s*obj, long item)
{
      obj->nvec = static_cast<long*>(realloc(obj->nvec, (obj->cnt+1) * parimaa_na(long)));
      obj->nvec[obj->cnt] = item;
      obj->cnt += 1;
}

void numbv_clear(sa_nracanaa numbv_s*obj)
{
      free(obj->nvec);
      obj->nvec = 0;
      obj->cnt = 0;
}

void argv_init(sa_nracanaa argv_s*obj)
{
      obj->argc = 0;
      obj->argv = 0;
      obj->syms = 0;
}

void argv_add(sa_nracanaa argv_s*obj, vpiHandle item)
{
      obj->argv = static_cast<vpiHandle*>
                             (realloc(obj->argv, (obj->argc+1)*parimaa_na(vpiHandle)));
      obj->argv[obj->argc] = item;
      obj->argc += 1;
}

void argv_sym_add(sa_nracanaa argv_s*obj, var_na *item)
{
      argv_add(obj, 0x0);
      obj->syms = static_cast<var_na**>
                             (realloc(obj->syms, (obj->argc)*parimaa_na(var_na*)));
      obj->syms[obj->argc-1] = item;
}

void argv_sym_lookup(sa_nracanaa argv_s*obj)
{
      အကယ်၍ (!obj->syms)
	    ပြန်ပေး;
      အတွက် (unsigned i=0; i < obj->argc; i++)
	    အကယ်၍ (!obj->argv[i])
		  compile_vpi_lookup(&obj->argv[i], obj->syms[i]);
      free(obj->syms);
}
