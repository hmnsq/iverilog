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

স্থির পূর্ণসংখ্যা iterator_free_object(vpiHandle ref)
{
      কাঠামো __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref);
      প্রতিজ্ঞা(hp);

      যদি (hp->free_args_flag)
	    মুক্ত(hp->args);

      delete hp;
      ফেরত 1;
}

অন্তঃরেখ __vpiIterator::__vpiIterator()
{ }

পূর্ণসংখ্যা __vpiIterator::get_type_code(শূন্য) ধ্রুবক
{ ফেরত vpiIterator; }

__vpiHandle::free_object_fun_t __vpiIterator::free_object_fun(শূন্য)
{ ফেরত &iterator_free_object; }

vpiHandle vpip_make_iterator(unsigned nargs, vpiHandle*args,
			     যৌক্তিক free_args_flag)
{
      কাঠামো __vpiIterator*res = নতুন __vpiIterator;
      res->args = args;
      res->nargs = nargs;
      res->পরবর্তী_2  = 0;

      res->free_args_flag = free_args_flag;

      ফেরত res;
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
	    ফেরত 0;
      }

      যদি (কাঠামো __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref)) {
	    যদি (hp->পরবর্তী_2 == hp->nargs) {
		  vpi_free_object(ref);
		  ফেরত 0;
	    }

	    ফেরত hp->args[hp->পরবর্তী_2++];
      }

      ফেরত ref->vpi_সূচক(0);
}
