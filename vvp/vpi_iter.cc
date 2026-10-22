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

static ចំនួនគត់ iterator_free_object(vpiHandle ref)
{
      sa_nracanaa __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref);
      assert(hp);

      ប្រសិនបើ (hp->free_args_flag)
	    free(hp->args);

      delete hp;
      ត្រឡប់ 1;
}

anta_hstha __vpiIterator::__vpiIterator()
{ }

ចំនួនគត់ __vpiIterator::get_type_code(void) const
{ ត្រឡប់ vpiIterator; }

__vpiHandle::free_object_fun_t __vpiIterator::free_object_fun(void)
{ ត្រឡប់ &iterator_free_object; }

vpiHandle vpip_make_iterator(unsigned nargs, vpiHandle*args,
			     bool free_args_flag)
{
      sa_nracanaa __vpiIterator*res = new __vpiIterator;
      res->args = args;
      res->nargs = nargs;
      res->next  = 0;

      res->free_args_flag = free_args_flag;

      ត្រឡប់ res;
}

/*
 * The vpi_scan function only applies to iterators. It returns the
 * next vpiHandle in the iterated list.
 */
vpiHandle vpi_scan(vpiHandle ref)
{
      ប្រសិនបើ (ref == 0) {
	    fprintf(stderr, "ERROR: NULL handle passed to vpi_scan.\n");
	    assert(0);
	    ត្រឡប់ 0;
      }

      ប្រសិនបើ (sa_nracanaa __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref)) {
	    ប្រសិនបើ (hp->next == hp->nargs) {
		  vpi_free_object(ref);
		  ត្រឡប់ 0;
	    }

	    ត្រឡប់ hp->args[hp->next++];
      }

      ត្រឡប់ ref->vpi_index(0);
}
