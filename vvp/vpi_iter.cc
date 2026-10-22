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

ساكن صحيح iterator_free_object(vpiHandle ref)
{
      بنية __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref);
      أكد(hp);

      إذا (hp->free_دلیلونه_flag)
	    حرر(hp->دلیلونه);

      ړنګول hp;
      أرجع 1;
}

مضمن __vpiIterator::__vpiIterator()
{ }

صحيح __vpiIterator::get_type_کوډ(فراغ) ثابت
{ أرجع vpiIterator; }

__vpiHandle::free_object_fun_t __vpiIterator::free_object_fun(فراغ)
{ أرجع &iterator_free_object; }

vpiHandle vpip_make_iterator(بینښه nargs, vpiHandle*دلیلونه,
			     منطقي free_دلیلونه_flag)
{
      بنية __vpiIterator*res = جديد __vpiIterator;
      res->دلیلونه = دلیلونه;
      res->nargs = nargs;
      res->بل_2  = 0;

      res->free_دلیلونه_flag = free_دلیلونه_flag;

      أرجع res;
}

/*
 * The vpi_scan function only applies to iterators. It returns the
 * next vpiHandle in the iterated list.
 */
vpiHandle vpi_لټون(vpiHandle ref)
{
      إذا (ref == 0) {
	    fprintf(stderr, "ERROR: NULL handle passed to vpi_scan.\n");
	    أكد(0);
	    أرجع 0;
      }

      إذا (بنية __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref)) {
	    إذا (hp->بل_2 == hp->nargs) {
		  vpi_free_object(ref);
		  أرجع 0;
	    }

	    أرجع hp->دلیلونه[hp->بل_2++];
      }

      أرجع ref->vpi_شاخص(0);
}
