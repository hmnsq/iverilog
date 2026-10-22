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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ iterator_free_object(vpiHandle ref)
{
      ಸಂರಚನಾ __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref);
      ಪ್ರತಿಜ್ಞಾ(hp);

      ಯದಿ (hp->free_args_flag)
	    ಮುಕ್ತ(hp->args);

      delete hp;
      ಪ್ರತ್ಯಾಗಮ 1;
}

ಅನ್ತಃಸ್ಥ __vpiIterator::__vpiIterator()
{ }

ಪೂರ್ಣಾಙ್ಕ __vpiIterator::get_type_code(ಶೂನ್ಯ) ಧ್ರುವ
{ ಪ್ರತ್ಯಾಗಮ vpiIterator; }

__vpiHandle::free_object_fun_t __vpiIterator::free_object_fun(ಶೂನ್ಯ)
{ ಪ್ರತ್ಯಾಗಮ &iterator_free_object; }

vpiHandle vpip_make_iterator(unsigned nargs, vpiHandle*args,
			     ತಾರ್ಕಿಕ free_args_flag)
{
      ಸಂರಚನಾ __vpiIterator*res = ನೂತನ __vpiIterator;
      res->args = args;
      res->nargs = nargs;
      res->next  = 0;

      res->free_args_flag = free_args_flag;

      ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * The vpi_scan function only applies to iterators. It returns the
 * next vpiHandle in the iterated list.
 */
vpiHandle vpi_scan(vpiHandle ref)
{
      ಯದಿ (ref == 0) {
	    fprintf(stderr, "ERROR: NULL handle passed to vpi_scan.\n");
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಪ್ರತ್ಯಾಗಮ 0;
      }

      ಯದಿ (ಸಂರಚನಾ __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref)) {
	    ಯದಿ (hp->next == hp->nargs) {
		  vpi_free_object(ref);
		  ಪ್ರತ್ಯಾಗಮ 0;
	    }

	    ಪ್ರತ್ಯಾಗಮ hp->args[hp->next++];
      }

      ಪ್ರತ್ಯಾಗಮ ref->vpi_index(0);
}
