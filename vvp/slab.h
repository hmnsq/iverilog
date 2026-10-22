#اگر_غیر_تعریف_شدہ IVL_slab_H
#تعریف_کرو IVL_slab_H
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

سانچہ <size_t SLAB_سائز, size_t CHUNK_گنتی> class slab_t {

      اتحاد شے_cell_u {
	    شے_cell_u*اگلا_2;
	    حرف space[SLAB_سائز];
      };

    عوامی:
      slab_t();

      خالی* alloc_slab();
      خالی  free_slab(خالی*);
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
	// If we have allocated memory then we need to delete it to make
	// valgrind happy.
      خالی delete_pool(خالی);
#اگر_ختم

      غیرعلامتی طویل pool;

    نجی:
      شے_cell_u*ڈھیر;
      شے_cell_u initial_chunk_[CHUNK_گنتی];
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
	// Each slab needs a pointer to the allocated space.
      شے_cell_u**slab_pool;
      غیرعلامتی slab_pool_گنتی;
#اگر_ختم
};

سانچہ <size_t SLAB_سائز, size_t CHUNK_گنتی>
slab_t<SLAB_سائز,CHUNK_گنتی>::slab_t()
{
      pool = CHUNK_گنتی;
      ڈھیر = initial_chunk_;
      برائے (غیرعلامتی idx = 0 ; idx < CHUNK_گنتی-1 ; idx += 1)
	    initial_chunk_[idx].اگلا_2 = initial_chunk_+idx+1;

      initial_chunk_[CHUNK_گنتی-1].اگلا_2 = 0;
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
	// Initially we have no allocated space.
      slab_pool = تہی;
      slab_pool_گنتی = 0;
#اگر_ختم
}

سانچہ <size_t SLAB_سائز, size_t CHUNK_گنتی>
اندرونی خالی* slab_t<SLAB_سائز,CHUNK_گنتی>::alloc_slab()
{
      اگر (ڈھیر == 0) {
	    شے_cell_u*chunk = new شے_cell_u[CHUNK_گنتی];
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
	    slab_pool_گنتی += 1;
	    slab_pool = static_cast<شے_cell_u **>(realloc(slab_pool,
	                slab_pool_گنتی*حجم(شے_cell_u **)));
	    slab_pool[slab_pool_گنتی-1] = chunk;
#اگر_ختم
	    برائے (غیرعلامتی idx = 0 ; idx < CHUNK_گنتی ; idx += 1) {
		  chunk[idx].اگلا_2 = ڈھیر;
		  ڈھیر = chunk+idx;
	    }
	    pool += CHUNK_گنتی;
      }

      شے_cell_u*cur = ڈھیر;
      ڈھیر = ڈھیر->اگلا_2;
      واپس cur;
}

سانچہ <size_t SLAB_سائز, size_t CHUNK_گنتی>
اندرونی خالی slab_t<SLAB_سائز,CHUNK_گنتی>::free_slab(خالی*ptr)
{
      شے_cell_u*cur = reinterpret_cast<شے_cell_u*> (ptr);
      cur->اگلا_2 = ڈھیر;
      ڈھیر = cur;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
سانچہ <size_t SLAB_سائز, size_t CHUNK_گنتی>
اندرونی خالی slab_t<SLAB_سائز,CHUNK_گنتی>::delete_pool(خالی)
{
      برائے (غیرعلامتی idx = 0; idx < slab_pool_گنتی; idx += 1) {
	    delete [] slab_pool[idx];
      }
      آزاد(slab_pool);
      slab_pool = تہی;
      slab_pool_گنتی = 0;
}
#اگر_ختم


#اگر_ختم /* IVL_slab_H */
