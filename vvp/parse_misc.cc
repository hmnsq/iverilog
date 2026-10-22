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

ধ্রুবক অক্ষর*yypath;
unsigned yyline;

শূন্য yyerror(ধ্রুবক অক্ষর*বার্তা)
{
      fprintf(stderr, "%s:%u: %s\n", yypath, yyline, বার্তা);
}

শূন্য symbv_init(কাঠামো symbv_s*obj)
{
      obj->cnt = 0;
      obj->vect = 0;
}

শূন্য symbv_add(কাঠামো symbv_s*obj, কাঠামো symb_s item)
{
      obj->vect = static_cast<কাঠামো symb_s*>
                             (realloc(obj->vect, (obj->cnt+1) * আকার(কাঠামো symb_s)));
      obj->vect[obj->cnt] = item;
      obj->cnt += 1;
}

শূন্য numbv_init(কাঠামো numbv_s*obj)
{
      obj->cnt = 0;
      obj->nvec = 0;
}

শূন্য numbv_add(কাঠামো numbv_s*obj, long item)
{
      obj->nvec = static_cast<long*>(realloc(obj->nvec, (obj->cnt+1) * আকার(long)));
      obj->nvec[obj->cnt] = item;
      obj->cnt += 1;
}

শূন্য numbv_clear(কাঠামো numbv_s*obj)
{
      মুক্ত(obj->nvec);
      obj->nvec = 0;
      obj->cnt = 0;
}

শূন্য argv_init(কাঠামো argv_s*obj)
{
      obj->argc = 0;
      obj->argv = 0;
      obj->syms = 0;
}

শূন্য argv_add(কাঠামো argv_s*obj, vpiHandle item)
{
      obj->argv = static_cast<vpiHandle*>
                             (realloc(obj->argv, (obj->argc+1)*আকার(vpiHandle)));
      obj->argv[obj->argc] = item;
      obj->argc += 1;
}

শূন্য argv_sym_add(কাঠামো argv_s*obj, অক্ষর *item)
{
      argv_add(obj, 0x0);
      obj->syms = static_cast<অক্ষর**>
                             (realloc(obj->syms, (obj->argc)*আকার(অক্ষর*)));
      obj->syms[obj->argc-1] = item;
}

শূন্য argv_sym_lookup(কাঠামো argv_s*obj)
{
      যদি (!obj->syms)
	    ফেরত;
      জন্য (unsigned i=0; i < obj->argc; i++)
	    যদি (!obj->argv[i])
		  compile_vpi_lookup(&obj->argv[i], obj->syms[i]);
      মুক্ত(obj->syms);
}
