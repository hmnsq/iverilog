#ifndef IVL_slab_H
#define IVL_slab_H
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

template <size_t SLAB_SIZE, size_t CHUNK_شمار> class slab_t {

      اجتماع item_cell_u {
	    item_cell_u*next;
	    نویسه space[SLAB_SIZE];
      };

    عمومی:
      slab_t();

      تهی* alloc_slab();
      تهی  free_slab(تهی*);
#ifdef CHECK_WITH_VALGRIND
	// If we have allocated memory then we need to delete it to make
	// valgrind happy.
      تهی delete_pool(تهی);
#endif

      unsigned long pool;

    خصوصی:
      item_cell_u*heap_;
      item_cell_u initial_chunk_[CHUNK_شمار];
#ifdef CHECK_WITH_VALGRIND
	// Each slab needs a pointer to the allocated space.
      item_cell_u**slab_pool;
      unsigned slab_pool_شمار;
#endif
};

template <size_t SLAB_SIZE, size_t CHUNK_شمار>
slab_t<SLAB_SIZE,CHUNK_شمار>::slab_t()
{
      pool = CHUNK_شمار;
      heap_ = initial_chunk_;
      برای (unsigned idx = 0 ; idx < CHUNK_شمار-1 ; idx += 1)
	    initial_chunk_[idx].next = initial_chunk_+idx+1;

      initial_chunk_[CHUNK_شمار-1].next = 0;
#ifdef CHECK_WITH_VALGRIND
	// Initially we have no allocated space.
      slab_pool = پوچ;
      slab_pool_شمار = 0;
#endif
}

template <size_t SLAB_SIZE, size_t CHUNK_شمار>
درونخطی تهی* slab_t<SLAB_SIZE,CHUNK_شمار>::alloc_slab()
{
      اگر (heap_ == 0) {
	    item_cell_u*chunk = new item_cell_u[CHUNK_شمار];
#ifdef CHECK_WITH_VALGRIND
	    slab_pool_شمار += 1;
	    slab_pool = static_cast<item_cell_u **>(realloc(slab_pool,
	                slab_pool_شمار*اندازه(item_cell_u **)));
	    slab_pool[slab_pool_شمار-1] = chunk;
#endif
	    برای (unsigned idx = 0 ; idx < CHUNK_شمار ; idx += 1) {
		  chunk[idx].next = heap_;
		  heap_ = chunk+idx;
	    }
	    pool += CHUNK_شمار;
      }

      item_cell_u*cur = heap_;
      heap_ = heap_->next;
      بازگشت cur;
}

template <size_t SLAB_SIZE, size_t CHUNK_شمار>
درونخطی تهی slab_t<SLAB_SIZE,CHUNK_شمار>::free_slab(تهی*ptr)
{
      item_cell_u*cur = reinterpret_cast<item_cell_u*> (ptr);
      cur->next = heap_;
      heap_ = cur;
}

#ifdef CHECK_WITH_VALGRIND
template <size_t SLAB_SIZE, size_t CHUNK_شمار>
درونخطی تهی slab_t<SLAB_SIZE,CHUNK_شمار>::delete_pool(تهی)
{
      برای (unsigned idx = 0; idx < slab_pool_شمار; idx += 1) {
	    delete [] slab_pool[idx];
      }
      آزاد(slab_pool);
      slab_pool = پوچ;
      slab_pool_شمار = 0;
}
#endif


#endif /* IVL_slab_H */
