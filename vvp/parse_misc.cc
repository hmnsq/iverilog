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

ಧ್ರುವ ವರ್ಣ*yypath;
unsigned yyline;

ಶೂನ್ಯ yyerror(ಧ್ರುವ ವರ್ಣ*ಸಂದೇಶ)
{
      fprintf(stderr, "%s:%u: %s\n", yypath, yyline, ಸಂದೇಶ);
}

ಶೂನ್ಯ symbv_init(ಸಂರಚನಾ symbv_s*obj)
{
      obj->cnt = 0;
      obj->vect = 0;
}

ಶೂನ್ಯ symbv_add(ಸಂರಚನಾ symbv_s*obj, ಸಂರಚನಾ symb_s item)
{
      obj->vect = static_cast<ಸಂರಚನಾ symb_s*>
                             (realloc(obj->vect, (obj->cnt+1) * ಪರಿಮಾಣ(ಸಂರಚನಾ symb_s)));
      obj->vect[obj->cnt] = item;
      obj->cnt += 1;
}

ಶೂನ್ಯ numbv_init(ಸಂರಚನಾ numbv_s*obj)
{
      obj->cnt = 0;
      obj->nvec = 0;
}

ಶೂನ್ಯ numbv_add(ಸಂರಚನಾ numbv_s*obj, long item)
{
      obj->nvec = static_cast<long*>(realloc(obj->nvec, (obj->cnt+1) * ಪರಿಮಾಣ(long)));
      obj->nvec[obj->cnt] = item;
      obj->cnt += 1;
}

ಶೂನ್ಯ numbv_clear(ಸಂರಚನಾ numbv_s*obj)
{
      ಮುಕ್ತ(obj->nvec);
      obj->nvec = 0;
      obj->cnt = 0;
}

ಶೂನ್ಯ argv_init(ಸಂರಚನಾ argv_s*obj)
{
      obj->argc = 0;
      obj->argv = 0;
      obj->syms = 0;
}

ಶೂನ್ಯ argv_add(ಸಂರಚನಾ argv_s*obj, vpiHandle item)
{
      obj->argv = static_cast<vpiHandle*>
                             (realloc(obj->argv, (obj->argc+1)*ಪರಿಮಾಣ(vpiHandle)));
      obj->argv[obj->argc] = item;
      obj->argc += 1;
}

ಶೂನ್ಯ argv_sym_add(ಸಂರಚನಾ argv_s*obj, ವರ್ಣ *item)
{
      argv_add(obj, 0x0);
      obj->syms = static_cast<ವರ್ಣ**>
                             (realloc(obj->syms, (obj->argc)*ಪರಿಮಾಣ(ವರ್ಣ*)));
      obj->syms[obj->argc-1] = item;
}

ಶೂನ್ಯ argv_sym_lookup(ಸಂರಚನಾ argv_s*obj)
{
      ಯದಿ (!obj->syms)
	    ಪ್ರತ್ಯಾಗಮ;
      ಪ್ರತಿ (unsigned i=0; i < obj->argc; i++)
	    ಯದಿ (!obj->argv[i])
		  compile_vpi_lookup(&obj->argv[i], obj->syms[i]);
      ಮುಕ್ತ(obj->syms);
}
