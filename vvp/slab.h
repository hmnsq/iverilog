#నిర్వచితం_కాకపోతే IVL_slab_H
#నిర్వచించు IVL_slab_H
/*
 * Copyright (c) 2008-2015 Picture Elements, Inc.
 *    Stephen Williams (steve@icarus.com)
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


# include  "config.h"

మూస <size_t SLAB_కొలత, size_t CHUNK_లెక్క> class slab_t {

      సంఘం అంశం_cell_u {
	    అంశం_cell_u*తదుపరి_2;
	    అక్షరం space[SLAB_కొలత];
      };

    బహిరంగ:
      slab_t();

      శూన్య* alloc_slab();
      శూన్య  free_slab(శూన్య*);
#నిర్వచితమైతే CHECK_WITH_VALGRIND
	// If we have allocated memory then we need to delete it to make
	// valgrind happy.
      శూన్య delete_pool(శూన్య);
#అయితే_ముగింపు

      గుర్తులేని దీర్ఘ pool;

    అంతరంగ:
      అంశం_cell_u*కుప్ప;
      అంశం_cell_u initial_chunk_[CHUNK_లెక్క];
#నిర్వచితమైతే CHECK_WITH_VALGRIND
	// Each slab needs a pointer to the allocated space.
      అంశం_cell_u**slab_pool;
      గుర్తులేని slab_pool_లెక్క;
#అయితే_ముగింపు
};

మూస <size_t SLAB_కొలత, size_t CHUNK_లెక్క>
slab_t<SLAB_కొలత,CHUNK_లెక్క>::slab_t()
{
      pool = CHUNK_లెక్క;
      కుప్ప = initial_chunk_;
      కొరకు (గుర్తులేని idx = 0 ; idx < CHUNK_లెక్క-1 ; idx += 1)
	    initial_chunk_[idx].తదుపరి_2 = initial_chunk_+idx+1;

      initial_chunk_[CHUNK_లెక్క-1].తదుపరి_2 = 0;
#నిర్వచితమైతే CHECK_WITH_VALGRIND
	// Initially we have no allocated space.
      slab_pool = ఖాళీ;
      slab_pool_లెక్క = 0;
#అయితే_ముగింపు
}

మూస <size_t SLAB_కొలత, size_t CHUNK_లెక్క>
అంతఃవరుస శూన్య* slab_t<SLAB_కొలత,CHUNK_లెక్క>::alloc_slab()
{
      ఒకవేళ (కుప్ప == 0) {
	    అంశం_cell_u*chunk = new అంశం_cell_u[CHUNK_లెక్క];
#నిర్వచితమైతే CHECK_WITH_VALGRIND
	    slab_pool_లెక్క += 1;
	    slab_pool = static_cast<అంశం_cell_u **>(realloc(slab_pool,
	                slab_pool_లెక్క*పరిమాణం(అంశం_cell_u **)));
	    slab_pool[slab_pool_లెక్క-1] = chunk;
#అయితే_ముగింపు
	    కొరకు (గుర్తులేని idx = 0 ; idx < CHUNK_లెక్క ; idx += 1) {
		  chunk[idx].తదుపరి_2 = కుప్ప;
		  కుప్ప = chunk+idx;
	    }
	    pool += CHUNK_లెక్క;
      }

      అంశం_cell_u*cur = కుప్ప;
      కుప్ప = కుప్ప->తదుపరి_2;
      తిరిగి cur;
}

మూస <size_t SLAB_కొలత, size_t CHUNK_లెక్క>
అంతఃవరుస శూన్య slab_t<SLAB_కొలత,CHUNK_లెక్క>::free_slab(శూన్య*ptr)
{
      అంశం_cell_u*cur = reinterpret_cast<అంశం_cell_u*> (ptr);
      cur->తదుపరి_2 = కుప్ప;
      కుప్ప = cur;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
మూస <size_t SLAB_కొలత, size_t CHUNK_లెక్క>
అంతఃవరుస శూన్య slab_t<SLAB_కొలత,CHUNK_లెక్క>::delete_pool(శూన్య)
{
      కొరకు (గుర్తులేని idx = 0; idx < slab_pool_లెక్క; idx += 1) {
	    delete [] slab_pool[idx];
      }
      విడుదల(slab_pool);
      slab_pool = ఖాళీ;
      slab_pool_లెక్క = 0;
}
#అయితే_ముగింపు


#అయితే_ముగింపు /* IVL_slab_H */
