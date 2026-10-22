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
غیرعلامتی yyline;

خالی yyerror(ثابت حرف*پیغام)
{
      fprintf(stderr, "%s:%u: %s\n", yypath, yyline, پیغام);
}

خالی symbv_init(ساخت symbv_s*obj)
{
      obj->cnt = 0;
      obj->vect = 0;
}

خالی symbv_جوڑو(ساخت symbv_s*obj, ساخت symb_s شے)
{
      obj->vect = static_cast<ساخت symb_s*>
                             (realloc(obj->vect, (obj->cnt+1) * حجم(ساخت symb_s)));
      obj->vect[obj->cnt] = شے;
      obj->cnt += 1;
}

خالی numbv_init(ساخت numbv_s*obj)
{
      obj->cnt = 0;
      obj->nvec = 0;
}

خالی numbv_جوڑو(ساخت numbv_s*obj, طویل شے)
{
      obj->nvec = static_cast<طویل*>(realloc(obj->nvec, (obj->cnt+1) * حجم(طویل)));
      obj->nvec[obj->cnt] = شے;
      obj->cnt += 1;
}

خالی numbv_clear(ساخت numbv_s*obj)
{
      آزاد(obj->nvec);
      obj->nvec = 0;
      obj->cnt = 0;
}

خالی argv_init(ساخت argv_s*obj)
{
      obj->argc = 0;
      obj->argv = 0;
      obj->syms = 0;
}

خالی argv_جوڑو(ساخت argv_s*obj, vpiHandle شے)
{
      obj->argv = static_cast<vpiHandle*>
                             (realloc(obj->argv, (obj->argc+1)*حجم(vpiHandle)));
      obj->argv[obj->argc] = شے;
      obj->argc += 1;
}

خالی argv_sym_جوڑو(ساخت argv_s*obj, حرف *شے)
{
      argv_جوڑو(obj, 0x0);
      obj->syms = static_cast<حرف**>
                             (realloc(obj->syms, (obj->argc)*حجم(حرف*)));
      obj->syms[obj->argc-1] = شے;
}

خالی argv_sym_lookup(ساخت argv_s*obj)
{
      اگر (!obj->syms)
	    واپس;
      برائے (غیرعلامتی i=0; i < obj->argc; i++)
	    اگر (!obj->argv[i])
		  compile_vpi_lookup(&obj->argv[i], obj->syms[i]);
      آزاد(obj->syms);
}
