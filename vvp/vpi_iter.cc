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

நிலையான முழு iterator_free_object(vpiHandle ref)
{
      கட்டமைப்பு __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref);
      ப்ரதிஜ்ஞா(hp);

      எனில் (hp->free_args_flag)
	    விடுவி(hp->args);

      delete hp;
      திரும்பு 1;
}

உள்வரி __vpiIterator::__vpiIterator()
{ }

முழு __vpiIterator::get_type_code(வெறுமை) மாறிலி
{ திரும்பு vpiIterator; }

__vpiHandle::free_object_fun_t __vpiIterator::free_object_fun(வெறுமை)
{ திரும்பு &iterator_free_object; }

vpiHandle vpip_make_iterator(unsigned nargs, vpiHandle*args,
			     தருக்க free_args_flag)
{
      கட்டமைப்பு __vpiIterator*res = புதிய __vpiIterator;
      res->args = args;
      res->nargs = nargs;
      res->next  = 0;

      res->free_args_flag = free_args_flag;

      திரும்பு res;
}

/*
 * The vpi_scan function only applies to iterators. It returns the
 * next vpiHandle in the iterated list.
 */
vpiHandle vpi_scan(vpiHandle ref)
{
      எனில் (ref == 0) {
	    fprintf(stderr, "ERROR: NULL handle passed to vpi_scan.\n");
	    ப்ரதிஜ்ஞா(0);
	    திரும்பு 0;
      }

      எனில் (கட்டமைப்பு __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref)) {
	    எனில் (hp->next == hp->nargs) {
		  vpi_free_object(ref);
		  திரும்பு 0;
	    }

	    திரும்பு hp->args[hp->next++];
      }

      திரும்பு ref->vpi_index(0);
}
