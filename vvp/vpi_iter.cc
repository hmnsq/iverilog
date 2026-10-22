/*
 * Copyright (c) 1999-2012 Stephen Williams (steve@icarus.com)
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

/*
 * Find here the methods functions in support of iterator objects.
 */

# include  "vpi_priv.h"
# include  <cstdlib>
# include  <cassert>
# include  "ivl_alloc.h"

സ്ഥിതം പൂർണ്ണസംഖ്യ iterator_free_object(vpiHandle ref)
{
      ഘടന __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref);
      പ്രതിജ്ഞാ(hp);

      എങ്കിൽ (hp->free_args_flag)
	    സ്വതന്ത്രം(hp->args);

      delete hp;
      തിരികെ 1;
}

വരിയിൽ __vpiIterator::__vpiIterator()
{ }

പൂർണ്ണസംഖ്യ __vpiIterator::get_type_code(ശൂന്യം) സ്ഥിരം
{ തിരികെ vpiIterator; }

__vpiHandle::free_object_fun_t __vpiIterator::free_object_fun(ശൂന്യം)
{ തിരികെ &iterator_free_object; }

vpiHandle vpip_make_iterator(unsigned nargs, vpiHandle*args,
			     തർക്കം free_args_flag)
{
      ഘടന __vpiIterator*res = പുതിയ __vpiIterator;
      res->args = args;
      res->nargs = nargs;
      res->next  = 0;

      res->free_args_flag = free_args_flag;

      തിരികെ res;
}

/*
 * The vpi_scan function only applies to iterators. It returns the
 * next vpiHandle in the iterated list.
 */
vpiHandle vpi_scan(vpiHandle ref)
{
      എങ്കിൽ (ref == 0) {
	    fprintf(stderr, "ERROR: NULL handle passed to vpi_scan.\n");
	    പ്രതിജ്ഞാ(0);
	    തിരികെ 0;
      }

      എങ്കിൽ (ഘടന __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref)) {
	    എങ്കിൽ (hp->next == hp->nargs) {
		  vpi_free_object(ref);
		  തിരികെ 0;
	    }

	    തിരികെ hp->args[hp->next++];
      }

      തിരികെ ref->vpi_index(0);
}
