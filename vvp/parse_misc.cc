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

स्थिरांक अक्षर*yypath;
अचिन्हित yyline;

शून्य yyerror(स्थिरांक अक्षर*संदेश)
{
      ख_लिखो(मानक_त्रुटि, "%s:%u: %s\n", yypath, yyline, संदेश);
}

शून्य symbv_init(संरचना symbv_s*obj)
{
      obj->cnt = 0;
      obj->vect = 0;
}

शून्य symbv_add(संरचना symbv_s*obj, संरचना symb_s item)
{
      obj->vect = static_cast<संरचना symb_s*>
                             (पुनः_स्मृति(obj->vect, (obj->cnt+1) * आकार(संरचना symb_s)));
      obj->vect[obj->cnt] = item;
      obj->cnt += 1;
}

शून्य numbv_init(संरचना numbv_s*obj)
{
      obj->cnt = 0;
      obj->nvec = 0;
}

शून्य numbv_add(संरचना numbv_s*obj, दीर्घ item)
{
      obj->nvec = static_cast<दीर्घ*>(पुनः_स्मृति(obj->nvec, (obj->cnt+1) * आकार(दीर्घ)));
      obj->nvec[obj->cnt] = item;
      obj->cnt += 1;
}

शून्य numbv_clear(संरचना numbv_s*obj)
{
      मुक्त(obj->nvec);
      obj->nvec = 0;
      obj->cnt = 0;
}

शून्य argv_init(संरचना argv_s*obj)
{
      obj->argc = 0;
      obj->argv = 0;
      obj->syms = 0;
}

शून्य argv_add(संरचना argv_s*obj, vpiHandle item)
{
      obj->argv = static_cast<vpiHandle*>
                             (पुनः_स्मृति(obj->argv, (obj->argc+1)*आकार(vpiHandle)));
      obj->argv[obj->argc] = item;
      obj->argc += 1;
}

शून्य argv_sym_add(संरचना argv_s*obj, अक्षर *item)
{
      argv_add(obj, 0x0);
      obj->syms = static_cast<अक्षर**>
                             (पुनः_स्मृति(obj->syms, (obj->argc)*आकार(अक्षर*)));
      obj->syms[obj->argc-1] = item;
}

शून्य argv_sym_lookup(संरचना argv_s*obj)
{
      जर (!obj->syms)
	    परत;
      साठी (अचिन्हित i=0; i < obj->argc; i++)
	    जर (!obj->argv[i])
		  compile_vpi_lookup(&obj->argv[i], obj->syms[i]);
      मुक्त(obj->syms);
}
