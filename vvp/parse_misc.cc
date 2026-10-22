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

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*yypath;
ಚಿಹ್ನೆರಹಿತ yyline;

ಶೂನ್ಯ yyerror(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಸಂದೇಶ)
{
      fprintf(stderr, "%s:%u: %s\n", yypath, yyline, ಸಂದೇಶ);
}

ಶೂನ್ಯ symbv_init(ರಚನೆ symbv_s*obj)
{
      obj->cnt = 0;
      obj->vect = 0;
}

ಶೂನ್ಯ symbv_ಕೂಡಿಸು(ರಚನೆ symbv_s*obj, ರಚನೆ symb_s ಅಂಶ)
{
      obj->vect = static_cast<ರಚನೆ symb_s*>
                             (realloc(obj->vect, (obj->cnt+1) * ಗಾತ್ರ(ರಚನೆ symb_s)));
      obj->vect[obj->cnt] = ಅಂಶ;
      obj->cnt += 1;
}

ಶೂನ್ಯ numbv_init(ರಚನೆ numbv_s*obj)
{
      obj->cnt = 0;
      obj->nvec = 0;
}

ಶೂನ್ಯ numbv_ಕೂಡಿಸು(ರಚನೆ numbv_s*obj, ದೀರ್ಘ ಅಂಶ)
{
      obj->nvec = static_cast<ದೀರ್ಘ*>(realloc(obj->nvec, (obj->cnt+1) * ಗಾತ್ರ(ದೀರ್ಘ)));
      obj->nvec[obj->cnt] = ಅಂಶ;
      obj->cnt += 1;
}

ಶೂನ್ಯ numbv_clear(ರಚನೆ numbv_s*obj)
{
      ಬಿಡುಗಡೆ(obj->nvec);
      obj->nvec = 0;
      obj->cnt = 0;
}

ಶೂನ್ಯ argv_init(ರಚನೆ argv_s*obj)
{
      obj->argc = 0;
      obj->argv = 0;
      obj->syms = 0;
}

ಶೂನ್ಯ argv_ಕೂಡಿಸು(ರಚನೆ argv_s*obj, vpiHandle ಅಂಶ)
{
      obj->argv = static_cast<vpiHandle*>
                             (realloc(obj->argv, (obj->argc+1)*ಗಾತ್ರ(vpiHandle)));
      obj->argv[obj->argc] = ಅಂಶ;
      obj->argc += 1;
}

ಶೂನ್ಯ argv_sym_ಕೂಡಿಸು(ರಚನೆ argv_s*obj, ಅಕ್ಷರ *ಅಂಶ)
{
      argv_ಕೂಡಿಸು(obj, 0x0);
      obj->syms = static_cast<ಅಕ್ಷರ**>
                             (realloc(obj->syms, (obj->argc)*ಗಾತ್ರ(ಅಕ್ಷರ*)));
      obj->syms[obj->argc-1] = ಅಂಶ;
}

ಶೂನ್ಯ argv_sym_lookup(ರಚನೆ argv_s*obj)
{
      ಆದರೆ (!obj->syms)
	    ಹಿಂತಿರುಗಿ;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i=0; i < obj->argc; i++)
	    ಆದರೆ (!obj->argv[i])
		  compile_vpi_lookup(&obj->argv[i], obj->syms[i]);
      ಬಿಡುಗಡೆ(obj->syms);
}
