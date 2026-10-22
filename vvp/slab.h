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

template <size_t SLAB_SIZE, size_t CHUNK_എണ്ണം> class slab_t {

      സംഘം item_cell_u {
	    item_cell_u*next;
	    അക്ഷരം space[SLAB_SIZE];
      };

    പൊതു:
      slab_t();

      ശൂന്യം* alloc_slab();
      ശൂന്യം  free_slab(ശൂന്യം*);
#ifdef CHECK_WITH_VALGRIND
	// If we have allocated memory then we need to delete it to make
	// valgrind happy.
      ശൂന്യം delete_pool(ശൂന്യം);
#endif

      unsigned long pool;

    സ്വകാര്യ:
      item_cell_u*heap_;
      item_cell_u initial_chunk_[CHUNK_എണ്ണം];
#ifdef CHECK_WITH_VALGRIND
	// Each slab needs a pointer to the allocated space.
      item_cell_u**slab_pool;
      unsigned slab_pool_എണ്ണം;
#endif
};

template <size_t SLAB_SIZE, size_t CHUNK_എണ്ണം>
slab_t<SLAB_SIZE,CHUNK_എണ്ണം>::slab_t()
{
      pool = CHUNK_എണ്ണം;
      heap_ = initial_chunk_;
      വേണ്ടി (unsigned idx = 0 ; idx < CHUNK_എണ്ണം-1 ; idx += 1)
	    initial_chunk_[idx].next = initial_chunk_+idx+1;

      initial_chunk_[CHUNK_എണ്ണം-1].next = 0;
#ifdef CHECK_WITH_VALGRIND
	// Initially we have no allocated space.
      slab_pool = ശൂന്യ;
      slab_pool_എണ്ണം = 0;
#endif
}

template <size_t SLAB_SIZE, size_t CHUNK_എണ്ണം>
വരിയിൽ ശൂന്യം* slab_t<SLAB_SIZE,CHUNK_എണ്ണം>::alloc_slab()
{
      എങ്കിൽ (heap_ == 0) {
	    item_cell_u*chunk = new item_cell_u[CHUNK_എണ്ണം];
#ifdef CHECK_WITH_VALGRIND
	    slab_pool_എണ്ണം += 1;
	    slab_pool = static_cast<item_cell_u **>(realloc(slab_pool,
	                slab_pool_എണ്ണം*വലുപ്പം(item_cell_u **)));
	    slab_pool[slab_pool_എണ്ണം-1] = chunk;
#endif
	    വേണ്ടി (unsigned idx = 0 ; idx < CHUNK_എണ്ണം ; idx += 1) {
		  chunk[idx].next = heap_;
		  heap_ = chunk+idx;
	    }
	    pool += CHUNK_എണ്ണം;
      }

      item_cell_u*cur = heap_;
      heap_ = heap_->next;
      തിരികെ cur;
}

template <size_t SLAB_SIZE, size_t CHUNK_എണ്ണം>
വരിയിൽ ശൂന്യം slab_t<SLAB_SIZE,CHUNK_എണ്ണം>::free_slab(ശൂന്യം*ptr)
{
      item_cell_u*cur = reinterpret_cast<item_cell_u*> (ptr);
      cur->next = heap_;
      heap_ = cur;
}

#ifdef CHECK_WITH_VALGRIND
template <size_t SLAB_SIZE, size_t CHUNK_എണ്ണം>
വരിയിൽ ശൂന്യം slab_t<SLAB_SIZE,CHUNK_എണ്ണം>::delete_pool(ശൂന്യം)
{
      വേണ്ടി (unsigned idx = 0; idx < slab_pool_എണ്ണം; idx += 1) {
	    delete [] slab_pool[idx];
      }
      സ്വതന്ത്രം(slab_pool);
      slab_pool = ശൂന്യ;
      slab_pool_എണ്ണം = 0;
}
#endif


#endif /* IVL_slab_H */
