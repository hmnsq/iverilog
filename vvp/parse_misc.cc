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

અચલ અક્ષર*yypath;
unsigned yyline;

શૂન્ય yyerror(અચલ અક્ષર*સંદેશ)
{
      fprintf(stderr, "%s:%u: %s\n", yypath, yyline, સંદેશ);
}

શૂન્ય symbv_init(સંરચના symbv_s*obj)
{
      obj->cnt = 0;
      obj->vect = 0;
}

શૂન્ય symbv_add(સંરચના symbv_s*obj, સંરચના symb_s item)
{
      obj->vect = static_cast<સંરચના symb_s*>
                             (realloc(obj->vect, (obj->cnt+1) * કદ(સંરચના symb_s)));
      obj->vect[obj->cnt] = item;
      obj->cnt += 1;
}

શૂન્ય numbv_init(સંરચના numbv_s*obj)
{
      obj->cnt = 0;
      obj->nvec = 0;
}

શૂન્ય numbv_add(સંરચના numbv_s*obj, long item)
{
      obj->nvec = static_cast<long*>(realloc(obj->nvec, (obj->cnt+1) * કદ(long)));
      obj->nvec[obj->cnt] = item;
      obj->cnt += 1;
}

શૂન્ય numbv_clear(સંરચના numbv_s*obj)
{
      મુક્ત(obj->nvec);
      obj->nvec = 0;
      obj->cnt = 0;
}

શૂન્ય argv_init(સંરચના argv_s*obj)
{
      obj->argc = 0;
      obj->argv = 0;
      obj->syms = 0;
}

શૂન્ય argv_add(સંરચના argv_s*obj, vpiHandle item)
{
      obj->argv = static_cast<vpiHandle*>
                             (realloc(obj->argv, (obj->argc+1)*કદ(vpiHandle)));
      obj->argv[obj->argc] = item;
      obj->argc += 1;
}

શૂન્ય argv_sym_add(સંરચના argv_s*obj, અક્ષર *item)
{
      argv_add(obj, 0x0);
      obj->syms = static_cast<અક્ષર**>
                             (realloc(obj->syms, (obj->argc)*કદ(અક્ષર*)));
      obj->syms[obj->argc-1] = item;
}

શૂન્ય argv_sym_lookup(સંરચના argv_s*obj)
{
      જો (!obj->syms)
	    પાછા;
      માટે (unsigned i=0; i < obj->argc; i++)
	    જો (!obj->argv[i])
		  compile_vpi_lookup(&obj->argv[i], obj->syms[i]);
      મુક્ત(obj->syms);
}
