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

अटल पूर्णांक iterator_free_object(vpiHandle ref)
{
      काष्ठा __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref);
      निश्चित(hp);

      अगर (hp->free_args_flag)
	    मुक्त(hp->args);

      मिटाओ hp;
      वापस 1;
}

अंतरभूत __vpiIterator::__vpiIterator()
{ }

पूर्णांक __vpiIterator::get_type_code(व्योम) स्थिर
{ वापस vpiIterator; }

__vpiHandle::free_object_fun_t __vpiIterator::free_object_fun(व्योम)
{ वापस &iterator_free_object; }

vpiHandle vpip_make_iterator(अचिन्हित nargs, vpiHandle*args,
			     bool free_args_flag)
{
      काष्ठा __vpiIterator*res = नया __vpiIterator;
      res->args = args;
      res->nargs = nargs;
      res->अग्र_2  = 0;

      res->free_args_flag = free_args_flag;

      वापस res;
}

/*
 * The vpi_scan function only applies to iterators. It returns the
 * next vpiHandle in the iterated list.
 */
vpiHandle vpi_scan(vpiHandle ref)
{
      अगर (ref == 0) {
	    ख_लिखो(मानक_त्रुटि, "ERROR: NULL handle passed to vpi_scan.\n");
	    निश्चित(0);
	    वापस 0;
      }

      अगर (काष्ठा __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref)) {
	    अगर (hp->अग्र_2 == hp->nargs) {
		  vpi_free_object(ref);
		  वापस 0;
	    }

	    वापस hp->args[hp->अग्र_2++];
      }

      वापस ref->vpi_सूचकांक(0);
}
