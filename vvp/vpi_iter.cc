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

ساکن صحیح iterator_free_object(vpiHandle ref)
{
      ساخت __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref);
      أكد(hp);

      اگر (hp->free_دلائل_flag)
	    آزاد(hp->دلائل);

      حذف hp;
      واپس 1;
}

اندرونی __vpiIterator::__vpiIterator()
{ }

صحیح __vpiIterator::get_type_رمز(خالی) ثابت
{ واپس vpiIterator; }

__vpiHandle::free_object_fun_t __vpiIterator::free_object_fun(خالی)
{ واپس &iterator_free_object; }

vpiHandle vpip_make_iterator(غیرعلامتی nargs, vpiHandle*دلائل,
			     منطقی free_دلائل_flag)
{
      ساخت __vpiIterator*res = نیا __vpiIterator;
      res->دلائل = دلائل;
      res->nargs = nargs;
      res->اگلا_2  = 0;

      res->free_دلائل_flag = free_دلائل_flag;

      واپس res;
}

/*
 * The vpi_scan function only applies to iterators. It returns the
 * next vpiHandle in the iterated list.
 */
vpiHandle vpi_جائزہ(vpiHandle ref)
{
      اگر (ref == 0) {
	    fprintf(stderr, "ERROR: NULL handle passed to vpi_scan.\n");
	    أكد(0);
	    واپس 0;
      }

      اگر (ساخت __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref)) {
	    اگر (hp->اگلا_2 == hp->nargs) {
		  vpi_free_object(ref);
		  واپس 0;
	    }

	    واپس hp->دلائل[hp->اگلا_2++];
      }

      واپس ref->vpi_اشاریہ(0);
}
