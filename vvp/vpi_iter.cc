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

స్థిర పూర్ణాంకం iterator_free_object(vpiHandle ref)
{
      నిర్మాణం __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref);
      ప్రతిజ్ఞా(hp);

      ఒకవేళ (hp->free_వాదనలు_flag)
	    విడుదల(hp->వాదనలు);

      తొలగించు hp;
      తిరిగి 1;
}

అంతఃవరుస __vpiIterator::__vpiIterator()
{ }

పూర్ణాంకం __vpiIterator::get_type_సంకేతం(శూన్య) స్థిరరాశి
{ తిరిగి vpiIterator; }

__vpiHandle::free_object_fun_t __vpiIterator::free_object_fun(శూన్య)
{ తిరిగి &iterator_free_object; }

vpiHandle vpip_make_iterator(గుర్తులేని nargs, vpiHandle*వాదనలు,
			     తార్కిక free_వాదనలు_flag)
{
      నిర్మాణం __vpiIterator*res = కొత్త __vpiIterator;
      res->వాదనలు = వాదనలు;
      res->nargs = nargs;
      res->తదుపరి_2  = 0;

      res->free_వాదనలు_flag = free_వాదనలు_flag;

      తిరిగి res;
}

/*
 * The vpi_scan function only applies to iterators. It returns the
 * next vpiHandle in the iterated list.
 */
vpiHandle vpi_పరిశీలన(vpiHandle ref)
{
      ఒకవేళ (ref == 0) {
	    fprintf(stderr, "ERROR: NULL handle passed to vpi_scan.\n");
	    ప్రతిజ్ఞా(0);
	    తిరిగి 0;
      }

      ఒకవేళ (నిర్మాణం __vpiIterator*hp = dynamic_cast<__vpiIterator*>(ref)) {
	    ఒకవేళ (hp->తదుపరి_2 == hp->nargs) {
		  vpi_free_object(ref);
		  తిరిగి 0;
	    }

	    తిరిగి hp->వాదనలు[hp->తదుపరి_2++];
      }

      తిరిగి ref->vpi_సూచీ(0);
}
