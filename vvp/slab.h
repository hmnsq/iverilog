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

template <size_t SLAB_আকার, size_t CHUNK_গুনতি> class slab_t {

      সংঘ item_cell_u {
	    item_cell_u*পরবর্তী_2;
	    অক্ষর space[SLAB_আকার];
      };

    সর্বজনীন:
      slab_t();

      শূন্য* alloc_slab();
      শূন্য  free_slab(শূন্য*);
#ifdef CHECK_WITH_VALGRIND
	// If we have allocated memory then we need to delete it to make
	// valgrind happy.
      শূন্য delete_pool(শূন্য);
#endif

      unsigned long pool;

    ব্যক্তিগত:
      item_cell_u*heap_;
      item_cell_u initial_chunk_[CHUNK_গুনতি];
#ifdef CHECK_WITH_VALGRIND
	// Each slab needs a pointer to the allocated space.
      item_cell_u**slab_pool;
      unsigned slab_pool_গুনতি;
#endif
};

template <size_t SLAB_আকার, size_t CHUNK_গুনতি>
slab_t<SLAB_আকার,CHUNK_গুনতি>::slab_t()
{
      pool = CHUNK_গুনতি;
      heap_ = initial_chunk_;
      জন্য (unsigned idx = 0 ; idx < CHUNK_গুনতি-1 ; idx += 1)
	    initial_chunk_[idx].পরবর্তী_2 = initial_chunk_+idx+1;

      initial_chunk_[CHUNK_গুনতি-1].পরবর্তী_2 = 0;
#ifdef CHECK_WITH_VALGRIND
	// Initially we have no allocated space.
      slab_pool = ফাঁকা;
      slab_pool_গুনতি = 0;
#endif
}

template <size_t SLAB_আকার, size_t CHUNK_গুনতি>
অন্তঃরেখ শূন্য* slab_t<SLAB_আকার,CHUNK_গুনতি>::alloc_slab()
{
      যদি (heap_ == 0) {
	    item_cell_u*chunk = new item_cell_u[CHUNK_গুনতি];
#ifdef CHECK_WITH_VALGRIND
	    slab_pool_গুনতি += 1;
	    slab_pool = static_cast<item_cell_u **>(realloc(slab_pool,
	                slab_pool_গুনতি*আকার(item_cell_u **)));
	    slab_pool[slab_pool_গুনতি-1] = chunk;
#endif
	    জন্য (unsigned idx = 0 ; idx < CHUNK_গুনতি ; idx += 1) {
		  chunk[idx].পরবর্তী_2 = heap_;
		  heap_ = chunk+idx;
	    }
	    pool += CHUNK_গুনতি;
      }

      item_cell_u*cur = heap_;
      heap_ = heap_->পরবর্তী_2;
      ফেরত cur;
}

template <size_t SLAB_আকার, size_t CHUNK_গুনতি>
অন্তঃরেখ শূন্য slab_t<SLAB_আকার,CHUNK_গুনতি>::free_slab(শূন্য*ptr)
{
      item_cell_u*cur = reinterpret_cast<item_cell_u*> (ptr);
      cur->পরবর্তী_2 = heap_;
      heap_ = cur;
}

#ifdef CHECK_WITH_VALGRIND
template <size_t SLAB_আকার, size_t CHUNK_গুনতি>
অন্তঃরেখ শূন্য slab_t<SLAB_আকার,CHUNK_গুনতি>::delete_pool(শূন্য)
{
      জন্য (unsigned idx = 0; idx < slab_pool_গুনতি; idx += 1) {
	    delete [] slab_pool[idx];
      }
      মুক্ত(slab_pool);
      slab_pool = ফাঁকা;
      slab_pool_গুনতি = 0;
}
#endif


#endif /* IVL_slab_H */
