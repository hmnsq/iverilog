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

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା iterator_free_object(vpiHandle ref)
{
      ସଂରଚନା __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref);
      ପ୍ରତିଜ୍ଞା(hp);

      ଯଦି (hp->free_args_flag)
	    ମୁକ୍ତ(hp->args);

      delete hp;
      ଫେରନ୍ତୁ 1;
}

ଧାଡ଼ିଭିତରେ __vpiIterator::__vpiIterator()
{ }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା __vpiIterator::get_type_code(ଶୂନ୍ୟ) ଅଚଳ
{ ଫେରନ୍ତୁ vpiIterator; }

__vpiHandle::free_object_fun_t __vpiIterator::free_object_fun(ଶୂନ୍ୟ)
{ ଫେରନ୍ତୁ &iterator_free_object; }

vpiHandle vpip_make_iterator(unsigned nargs, vpiHandle*args,
			     ତାର୍କିକ free_args_flag)
{
      ସଂରଚନା __vpiIterator*res = ନୂତନ __vpiIterator;
      res->args = args;
      res->nargs = nargs;
      res->next  = 0;

      res->free_args_flag = free_args_flag;

      ଫେରନ୍ତୁ res;
}

/*
 * The vpi_scan function only applies to iterators. It returns the
 * next vpiHandle in the iterated list.
 */
vpiHandle vpi_scan(vpiHandle ref)
{
      ଯଦି (ref == 0) {
	    fprintf(stderr, "ERROR: NULL handle passed to vpi_scan.\n");
	    ପ୍ରତିଜ୍ଞା(0);
	    ଫେରନ୍ତୁ 0;
      }

      ଯଦି (ସଂରଚନା __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref)) {
	    ଯଦି (hp->next == hp->nargs) {
		  vpi_free_object(ref);
		  ଫେରନ୍ତୁ 0;
	    }

	    ଫେରନ୍ତୁ hp->args[hp->next++];
      }

      ଫେରନ୍ତୁ ref->vpi_index(0);
}
