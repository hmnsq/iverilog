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

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ iterator_free_object(vpiHandle ref)
{
      ರಚನೆ __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref);
      ಪ್ರತಿಜ್ಞಾ(hp);

      ಆದರೆ (hp->free_ವಾದಗಳು_flag)
	    ಬಿಡುಗಡೆ(hp->ವಾದಗಳು);

      ಅಳಿಸು hp;
      ಹಿಂತಿರುಗಿ 1;
}

ಸಾಲೊಳಗೆ __vpiIterator::__vpiIterator()
{ }

ಪೂರ್ಣಾಂಕ __vpiIterator::get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{ ಹಿಂತಿರುಗಿ vpiIterator; }

__vpiHandle::free_object_fun_t __vpiIterator::free_object_fun(ಶೂನ್ಯ)
{ ಹಿಂತಿರುಗಿ &iterator_free_object; }

vpiHandle vpip_make_iterator(ಚಿಹ್ನೆರಹಿತ nargs, vpiHandle*ವಾದಗಳು,
			     ತಾರ್ಕಿಕ free_ವಾದಗಳು_flag)
{
      ರಚನೆ __vpiIterator*res = ಹೊಸ __vpiIterator;
      res->ವಾದಗಳು = ವಾದಗಳು;
      res->nargs = nargs;
      res->ಮುಂದಿನ_2  = 0;

      res->free_ವಾದಗಳು_flag = free_ವಾದಗಳು_flag;

      ಹಿಂತಿರುಗಿ res;
}

/*
 * The vpi_scan function only applies to iterators. It returns the
 * next vpiHandle in the iterated list.
 */
vpiHandle vpi_ಪರಿಶೀಲನೆ(vpiHandle ref)
{
      ಆದರೆ (ref == 0) {
	    fprintf(stderr, "ERROR: NULL handle passed to vpi_scan.\n");
	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಹಿಂತಿರುಗಿ 0;
      }

      ಆದರೆ (ರಚನೆ __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref)) {
	    ಆದರೆ (hp->ಮುಂದಿನ_2 == hp->nargs) {
		  vpi_free_object(ref);
		  ಹಿಂತಿರುಗಿ 0;
	    }

	    ಹಿಂತಿರುಗಿ hp->ವಾದಗಳು[hp->ಮುಂದಿನ_2++];
      }

      ಹಿಂತಿರುಗಿ ref->vpi_ಸೂಚ್ಯಂಕ(0);
}
