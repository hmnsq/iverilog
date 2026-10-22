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

ढाँचा <माप_प्रकार SLAB_SIZE, माप_प्रकार CHUNK_मोजणी> class slab_t {

      संघ item_cell_u {
	    item_cell_u*next;
	    अक्षर space[SLAB_SIZE];
      };

    सार्वजनिक:
      slab_t();

      शून्य* alloc_slab();
      शून्य  free_slab(शून्य*);
#ifdef CHECK_WITH_VALGRIND
	// If we have allocated memory then we need to delete it to make
	// valgrind happy.
      शून्य delete_pool(शून्य);
#endif

      अचिन्हित दीर्घ pool;

    खाजगी:
      item_cell_u*heap_;
      item_cell_u initial_chunk_[CHUNK_मोजणी];
#ifdef CHECK_WITH_VALGRIND
	// Each slab needs a pointer to the allocated space.
      item_cell_u**slab_pool;
      अचिन्हित slab_pool_मोजणी;
#endif
};

ढाँचा <माप_प्रकार SLAB_SIZE, माप_प्रकार CHUNK_मोजणी>
slab_t<SLAB_SIZE,CHUNK_मोजणी>::slab_t()
{
      pool = CHUNK_मोजणी;
      heap_ = initial_chunk_;
      साठी (अचिन्हित idx = 0 ; idx < CHUNK_मोजणी-1 ; idx += 1)
	    initial_chunk_[idx].next = initial_chunk_+idx+1;

      initial_chunk_[CHUNK_मोजणी-1].next = 0;
#ifdef CHECK_WITH_VALGRIND
	// Initially we have no allocated space.
      slab_pool = रिक्त;
      slab_pool_मोजणी = 0;
#endif
}

ढाँचा <माप_प्रकार SLAB_SIZE, माप_प्रकार CHUNK_मोजणी>
ओळीत शून्य* slab_t<SLAB_SIZE,CHUNK_मोजणी>::alloc_slab()
{
      जर (heap_ == 0) {
	    item_cell_u*chunk = new item_cell_u[CHUNK_मोजणी];
#ifdef CHECK_WITH_VALGRIND
	    slab_pool_मोजणी += 1;
	    slab_pool = static_cast<item_cell_u **>(पुनः_स्मृति(slab_pool,
	                slab_pool_मोजणी*आकार(item_cell_u **)));
	    slab_pool[slab_pool_मोजणी-1] = chunk;
#endif
	    साठी (अचिन्हित idx = 0 ; idx < CHUNK_मोजणी ; idx += 1) {
		  chunk[idx].next = heap_;
		  heap_ = chunk+idx;
	    }
	    pool += CHUNK_मोजणी;
      }

      item_cell_u*cur = heap_;
      heap_ = heap_->next;
      परत cur;
}

ढाँचा <माप_प्रकार SLAB_SIZE, माप_प्रकार CHUNK_मोजणी>
ओळीत शून्य slab_t<SLAB_SIZE,CHUNK_मोजणी>::free_slab(शून्य*ptr)
{
      item_cell_u*cur = reinterpret_cast<item_cell_u*> (ptr);
      cur->next = heap_;
      heap_ = cur;
}

#ifdef CHECK_WITH_VALGRIND
ढाँचा <माप_प्रकार SLAB_SIZE, माप_प्रकार CHUNK_मोजणी>
ओळीत शून्य slab_t<SLAB_SIZE,CHUNK_मोजणी>::delete_pool(शून्य)
{
      साठी (अचिन्हित idx = 0; idx < slab_pool_मोजणी; idx += 1) {
	    delete [] slab_pool[idx];
      }
      मुक्त(slab_pool);
      slab_pool = रिक्त;
      slab_pool_मोजणी = 0;
}
#endif


#endif /* IVL_slab_H */
