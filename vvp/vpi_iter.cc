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

સ્થિર પૂર્ણાંક iterator_free_object(vpiHandle ref)
{
      સંરચના __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref);
      પ્રતિજ્ઞા(hp);

      જો (hp->free_args_flag)
	    મુક્ત(hp->args);

      delete hp;
      પાછા 1;
}

અંતઃરેખ __vpiIterator::__vpiIterator()
{ }

પૂર્ણાંક __vpiIterator::get_type_code(શૂન્ય) અચલ
{ પાછા vpiIterator; }

__vpiHandle::free_object_fun_t __vpiIterator::free_object_fun(શૂન્ય)
{ પાછા &iterator_free_object; }

vpiHandle vpip_make_iterator(unsigned nargs, vpiHandle*args,
			     તાર્કિક free_args_flag)
{
      સંરચના __vpiIterator*res = નવું __vpiIterator;
      res->args = args;
      res->nargs = nargs;
      res->next  = 0;

      res->free_args_flag = free_args_flag;

      પાછા res;
}

/*
 * The vpi_scan function only applies to iterators. It returns the
 * next vpiHandle in the iterated list.
 */
vpiHandle vpi_scan(vpiHandle ref)
{
      જો (ref == 0) {
	    fprintf(stderr, "ERROR: NULL handle passed to vpi_scan.\n");
	    પ્રતિજ્ઞા(0);
	    પાછા 0;
      }

      જો (સંરચના __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref)) {
	    જો (hp->next == hp->nargs) {
		  vpi_free_object(ref);
		  પાછા 0;
	    }

	    પાછા hp->args[hp->next++];
      }

      પાછા ref->vpi_index(0);
}
