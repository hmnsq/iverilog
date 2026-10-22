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

স্থির পূর্ণাঙ্ক iterator_free_object(vpiHandle ref)
{
      সংরচনা __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref);
      প্রতিজ্ঞা(hp);

      যদি (hp->free_args_flag)
	    মুক্ত(hp->args);

      delete hp;
      প্রত্যাগম 1;
}

অন্তঃস্থ __vpiIterator::__vpiIterator()
{ }

পূর্ণাঙ্ক __vpiIterator::get_type_code(শূন্য) ধ্রুব
{ প্রত্যাগম vpiIterator; }

__vpiHandle::free_object_fun_t __vpiIterator::free_object_fun(শূন্য)
{ প্রত্যাগম &iterator_free_object; }

vpiHandle vpip_make_iterator(unsigned nargs, vpiHandle*args,
			     তার্কিক free_args_flag)
{
      সংরচনা __vpiIterator*res = নূতন __vpiIterator;
      res->args = args;
      res->nargs = nargs;
      res->next  = 0;

      res->free_args_flag = free_args_flag;

      প্রত্যাগম res;
}

/*
 * The vpi_scan function only applies to iterators. It returns the
 * next vpiHandle in the iterated list.
 */
vpiHandle vpi_scan(vpiHandle ref)
{
      যদি (ref == 0) {
	    fprintf(stderr, "ERROR: NULL handle passed to vpi_scan.\n");
	    প্রতিজ্ঞা(0);
	    প্রত্যাগম 0;
      }

      যদি (সংরচনা __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref)) {
	    যদি (hp->next == hp->nargs) {
		  vpi_free_object(ref);
		  প্রত্যাগম 0;
	    }

	    প্রত্যাগম hp->args[hp->next++];
      }

      প্রত্যাগম ref->vpi_index(0);
}
