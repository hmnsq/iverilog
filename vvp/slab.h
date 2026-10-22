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

template <size_t SLAB_SIZE, size_t CHUNK_ژماردن> class slab_t {

      اتحاد item_cell_u {
	    item_cell_u*next;
	    حرف space[SLAB_SIZE];
      };

    عام:
      slab_t();

      فراغ* alloc_slab();
      فراغ  free_slab(فراغ*);
#ifdef CHECK_WITH_VALGRIND
	// If we have allocated memory then we need to delete it to make
	// valgrind happy.
      فراغ delete_pool(فراغ);
#endif

      unsigned long pool;

    خاص:
      item_cell_u*heap_;
      item_cell_u initial_chunk_[CHUNK_ژماردن];
#ifdef CHECK_WITH_VALGRIND
	// Each slab needs a pointer to the allocated space.
      item_cell_u**slab_pool;
      unsigned slab_pool_ژماردن;
#endif
};

template <size_t SLAB_SIZE, size_t CHUNK_ژماردن>
slab_t<SLAB_SIZE,CHUNK_ژماردن>::slab_t()
{
      pool = CHUNK_ژماردن;
      heap_ = initial_chunk_;
      لكل (unsigned idx = 0 ; idx < CHUNK_ژماردن-1 ; idx += 1)
	    initial_chunk_[idx].next = initial_chunk_+idx+1;

      initial_chunk_[CHUNK_ژماردن-1].next = 0;
#ifdef CHECK_WITH_VALGRIND
	// Initially we have no allocated space.
      slab_pool = عدم;
      slab_pool_ژماردن = 0;
#endif
}

template <size_t SLAB_SIZE, size_t CHUNK_ژماردن>
مضمن فراغ* slab_t<SLAB_SIZE,CHUNK_ژماردن>::alloc_slab()
{
      إذا (heap_ == 0) {
	    item_cell_u*chunk = new item_cell_u[CHUNK_ژماردن];
#ifdef CHECK_WITH_VALGRIND
	    slab_pool_ژماردن += 1;
	    slab_pool = static_cast<item_cell_u **>(realloc(slab_pool,
	                slab_pool_ژماردن*حجم(item_cell_u **)));
	    slab_pool[slab_pool_ژماردن-1] = chunk;
#endif
	    لكل (unsigned idx = 0 ; idx < CHUNK_ژماردن ; idx += 1) {
		  chunk[idx].next = heap_;
		  heap_ = chunk+idx;
	    }
	    pool += CHUNK_ژماردن;
      }

      item_cell_u*cur = heap_;
      heap_ = heap_->next;
      أرجع cur;
}

template <size_t SLAB_SIZE, size_t CHUNK_ژماردن>
مضمن فراغ slab_t<SLAB_SIZE,CHUNK_ژماردن>::free_slab(فراغ*ptr)
{
      item_cell_u*cur = reinterpret_cast<item_cell_u*> (ptr);
      cur->next = heap_;
      heap_ = cur;
}

#ifdef CHECK_WITH_VALGRIND
template <size_t SLAB_SIZE, size_t CHUNK_ژماردن>
مضمن فراغ slab_t<SLAB_SIZE,CHUNK_ژماردن>::delete_pool(فراغ)
{
      لكل (unsigned idx = 0; idx < slab_pool_ژماردن; idx += 1) {
	    delete [] slab_pool[idx];
      }
      حرر(slab_pool);
      slab_pool = عدم;
      slab_pool_ژماردن = 0;
}
#endif


#endif /* IVL_slab_H */
