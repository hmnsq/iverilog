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

स्थिर पूर्णाङ्क iterator_free_object(vpiHandle ref)
{
      संरचना __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref);
      प्रतिज्ञा(hp);

      सचे (hp->free_args_flag)
	    मुक्त(hp->args);

      delete hp;
      पटिनिवत्त 1;
}

अन्तःस्थ __vpiIterator::__vpiIterator()
{ }

पूर्णाङ्क __vpiIterator::get_type_code(शून्य) ध्रुव
{ पटिनिवत्त vpiIterator; }

__vpiHandle::free_object_fun_t __vpiIterator::free_object_fun(शून्य)
{ पटिनिवत्त &iterator_free_object; }

vpiHandle vpip_make_iterator(unsigned nargs, vpiHandle*args,
			     तार्किक free_args_flag)
{
      संरचना __vpiIterator*res = नूतन __vpiIterator;
      res->args = args;
      res->nargs = nargs;
      res->next  = 0;

      res->free_args_flag = free_args_flag;

      पटिनिवत्त res;
}

/*
 * The vpi_scan function only applies to iterators. It returns the
 * next vpiHandle in the iterated list.
 */
vpiHandle vpi_scan(vpiHandle ref)
{
      सचे (ref == 0) {
	    fprintf(stderr, "ERROR: NULL handle passed to vpi_scan.\n");
	    प्रतिज्ञा(0);
	    पटिनिवत्त 0;
      }

      सचे (संरचना __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref)) {
	    सचे (hp->next == hp->nargs) {
		  vpi_free_object(ref);
		  पटिनिवत्त 0;
	    }

	    पटिनिवत्त hp->args[hp->next++];
      }

      पटिनिवत्त ref->vpi_index(0);
}
