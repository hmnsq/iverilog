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

ਅਚਲ ਅੱਖਰ*yypath;
ਬਿਨਾਨਿਸ਼ਾਨੀ yyline;

ਖਾਲੀ yyerror(ਅਚਲ ਅੱਖਰ*ਸੁਨੇਹਾ)
{
      fprintf(stderr, "%s:%u: %s\n", yypath, yyline, ਸੁਨੇਹਾ);
}

ਖਾਲੀ symbv_init(ਬਣਤਰ symbv_s*obj)
{
      obj->cnt = 0;
      obj->vect = 0;
}

ਖਾਲੀ symbv_ਜੋੜੋ(ਬਣਤਰ symbv_s*obj, ਬਣਤਰ symb_s ਵਸਤੂ)
{
      obj->vect = static_cast<ਬਣਤਰ symb_s*>
                             (realloc(obj->vect, (obj->cnt+1) * ਆਕਾਰ(ਬਣਤਰ symb_s)));
      obj->vect[obj->cnt] = ਵਸਤੂ;
      obj->cnt += 1;
}

ਖਾਲੀ numbv_init(ਬਣਤਰ numbv_s*obj)
{
      obj->cnt = 0;
      obj->nvec = 0;
}

ਖਾਲੀ numbv_ਜੋੜੋ(ਬਣਤਰ numbv_s*obj, ਲੰਮਾ ਵਸਤੂ)
{
      obj->nvec = static_cast<ਲੰਮਾ*>(realloc(obj->nvec, (obj->cnt+1) * ਆਕਾਰ(ਲੰਮਾ)));
      obj->nvec[obj->cnt] = ਵਸਤੂ;
      obj->cnt += 1;
}

ਖਾਲੀ numbv_clear(ਬਣਤਰ numbv_s*obj)
{
      ਮੁਕਤ(obj->nvec);
      obj->nvec = 0;
      obj->cnt = 0;
}

ਖਾਲੀ argv_init(ਬਣਤਰ argv_s*obj)
{
      obj->argc = 0;
      obj->argv = 0;
      obj->syms = 0;
}

ਖਾਲੀ argv_ਜੋੜੋ(ਬਣਤਰ argv_s*obj, vpiHandle ਵਸਤੂ)
{
      obj->argv = static_cast<vpiHandle*>
                             (realloc(obj->argv, (obj->argc+1)*ਆਕਾਰ(vpiHandle)));
      obj->argv[obj->argc] = ਵਸਤੂ;
      obj->argc += 1;
}

ਖਾਲੀ argv_sym_ਜੋੜੋ(ਬਣਤਰ argv_s*obj, ਅੱਖਰ *ਵਸਤੂ)
{
      argv_ਜੋੜੋ(obj, 0x0);
      obj->syms = static_cast<ਅੱਖਰ**>
                             (realloc(obj->syms, (obj->argc)*ਆਕਾਰ(ਅੱਖਰ*)));
      obj->syms[obj->argc-1] = ਵਸਤੂ;
}

ਖਾਲੀ argv_sym_lookup(ਬਣਤਰ argv_s*obj)
{
      ਜੇ (!obj->syms)
	    ਵਾਪਸ;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i=0; i < obj->argc; i++)
	    ਜੇ (!obj->argv[i])
		  compile_vpi_lookup(&obj->argv[i], obj->syms[i]);
      ਮੁਕਤ(obj->syms);
}
