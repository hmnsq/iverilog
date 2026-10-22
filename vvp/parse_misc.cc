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

ثابت نویسه*yypath;
unsigned yyline;

تهی yyerror(ثابت نویسه*پیام)
{
      fprintf(stderr, "%s:%u: %s\n", yypath, yyline, پیام);
}

تهی symbv_init(ساختار symbv_s*obj)
{
      obj->cnt = 0;
      obj->vect = 0;
}

تهی symbv_add(ساختار symbv_s*obj, ساختار symb_s item)
{
      obj->vect = static_cast<ساختار symb_s*>
                             (realloc(obj->vect, (obj->cnt+1) * اندازه(ساختار symb_s)));
      obj->vect[obj->cnt] = item;
      obj->cnt += 1;
}

تهی numbv_init(ساختار numbv_s*obj)
{
      obj->cnt = 0;
      obj->nvec = 0;
}

تهی numbv_add(ساختار numbv_s*obj, long item)
{
      obj->nvec = static_cast<long*>(realloc(obj->nvec, (obj->cnt+1) * اندازه(long)));
      obj->nvec[obj->cnt] = item;
      obj->cnt += 1;
}

تهی numbv_clear(ساختار numbv_s*obj)
{
      آزاد(obj->nvec);
      obj->nvec = 0;
      obj->cnt = 0;
}

تهی argv_init(ساختار argv_s*obj)
{
      obj->argc = 0;
      obj->argv = 0;
      obj->syms = 0;
}

تهی argv_add(ساختار argv_s*obj, vpiHandle item)
{
      obj->argv = static_cast<vpiHandle*>
                             (realloc(obj->argv, (obj->argc+1)*اندازه(vpiHandle)));
      obj->argv[obj->argc] = item;
      obj->argc += 1;
}

تهی argv_sym_add(ساختار argv_s*obj, نویسه *item)
{
      argv_add(obj, 0x0);
      obj->syms = static_cast<نویسه**>
                             (realloc(obj->syms, (obj->argc)*اندازه(نویسه*)));
      obj->syms[obj->argc-1] = item;
}

تهی argv_sym_lookup(ساختار argv_s*obj)
{
      اگر (!obj->syms)
	    بازگشت;
      برای (unsigned i=0; i < obj->argc; i++)
	    اگر (!obj->argv[i])
		  compile_vpi_lookup(&obj->argv[i], obj->syms[i]);
      آزاد(obj->syms);
}
