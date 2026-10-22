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

ਸਥਿਰ ਪੂਰਨ iterator_free_object(vpiHandle ref)
{
      ਬਣਤਰ __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref);
      ਪ੍ਰਤਿਜ੍ਞਾ(hp);

      ਜੇ (hp->free_ਦਲੀਲਾਂ_flag)
	    ਮੁਕਤ(hp->ਦਲੀਲਾਂ);

      ਮਿਟਾਓ hp;
      ਵਾਪਸ 1;
}

ਸਤਰਅੰਦਰ __vpiIterator::__vpiIterator()
{ }

ਪੂਰਨ __vpiIterator::get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ
{ ਵਾਪਸ vpiIterator; }

__vpiHandle::free_object_fun_t __vpiIterator::free_object_fun(ਖਾਲੀ)
{ ਵਾਪਸ &iterator_free_object; }

vpiHandle vpip_make_iterator(ਬਿਨਾਨਿਸ਼ਾਨੀ nargs, vpiHandle*ਦਲੀਲਾਂ,
			     ਤਾਰਕਿਕ free_ਦਲੀਲਾਂ_flag)
{
      ਬਣਤਰ __vpiIterator*res = ਨਵਾਂ __vpiIterator;
      res->ਦਲੀਲਾਂ = ਦਲੀਲਾਂ;
      res->nargs = nargs;
      res->ਅਗਲਾ_2  = 0;

      res->free_ਦਲੀਲਾਂ_flag = free_ਦਲੀਲਾਂ_flag;

      ਵਾਪਸ res;
}

/*
 * The vpi_scan function only applies to iterators. It returns the
 * next vpiHandle in the iterated list.
 */
vpiHandle vpi_ਜਾਂਚ(vpiHandle ref)
{
      ਜੇ (ref == 0) {
	    fprintf(stderr, "ERROR: NULL handle passed to vpi_scan.\n");
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਵਾਪਸ 0;
      }

      ਜੇ (ਬਣਤਰ __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref)) {
	    ਜੇ (hp->ਅਗਲਾ_2 == hp->nargs) {
		  vpi_free_object(ref);
		  ਵਾਪਸ 0;
	    }

	    ਵਾਪਸ hp->ਦਲੀਲਾਂ[hp->ਅਗਲਾ_2++];
      }

      ਵਾਪਸ ref->vpi_ਸੂਚਕਾਂਕ(0);
}
