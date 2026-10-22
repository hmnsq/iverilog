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

ثابت حرف*yypath;
بنانشانی yyline;

فراغ yyerror(ثابت حرف*پیغام)
{
      fprintf(stderr, "%s:%u: %s\n", yypath, yyline, پیغام);
}

فراغ symbv_init(بنية symbv_s*obj)
{
      obj->cnt = 0;
      obj->vect = 0;
}

فراغ symbv_جوڑو(بنية symbv_s*obj, بنية symb_s شے)
{
      obj->vect = static_cast<بنية symb_s*>
                             (realloc(obj->vect, (obj->cnt+1) * حجم(بنية symb_s)));
      obj->vect[obj->cnt] = شے;
      obj->cnt += 1;
}

فراغ numbv_init(بنية numbv_s*obj)
{
      obj->cnt = 0;
      obj->nvec = 0;
}

فراغ numbv_جوڑو(بنية numbv_s*obj, لما شے)
{
      obj->nvec = static_cast<لما*>(realloc(obj->nvec, (obj->cnt+1) * حجم(لما)));
      obj->nvec[obj->cnt] = شے;
      obj->cnt += 1;
}

فراغ numbv_clear(بنية numbv_s*obj)
{
      حرر(obj->nvec);
      obj->nvec = 0;
      obj->cnt = 0;
}

فراغ argv_init(بنية argv_s*obj)
{
      obj->argc = 0;
      obj->argv = 0;
      obj->syms = 0;
}

فراغ argv_جوڑو(بنية argv_s*obj, vpiHandle شے)
{
      obj->argv = static_cast<vpiHandle*>
                             (realloc(obj->argv, (obj->argc+1)*حجم(vpiHandle)));
      obj->argv[obj->argc] = شے;
      obj->argc += 1;
}

فراغ argv_sym_جوڑو(بنية argv_s*obj, حرف *شے)
{
      argv_جوڑو(obj, 0x0);
      obj->syms = static_cast<حرف**>
                             (realloc(obj->syms, (obj->argc)*حجم(حرف*)));
      obj->syms[obj->argc-1] = شے;
}

فراغ argv_sym_lookup(بنية argv_s*obj)
{
      إذا (!obj->syms)
	    أرجع;
      لكل (بنانشانی i=0; i < obj->argc; i++)
	    إذا (!obj->argv[i])
		  compile_vpi_lookup(&obj->argv[i], obj->syms[i]);
      حرر(obj->syms);
}
