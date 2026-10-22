#جيڪڏهن_وصف_نه_ٿيل IVL_slab_H
#وصف_ڪريو IVL_slab_H
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

سانچو <size_t SLAB_سائيز, size_t CHUNK_ڳڻپ> class slab_t {

      اتحاد شي_cell_u {
	    شي_cell_u*ايندڙ_2;
	    حرف space[SLAB_سائيز];
      };

    عام:
      slab_t();

      فراغ* alloc_slab();
      فراغ  free_slab(فراغ*);
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
	// If we have allocated memory then we need to delete it to make
	// valgrind happy.
      فراغ delete_pool(فراغ);
#جيڪڏهن_پورو

      بنانشان ڊگهو pool;

    خاص:
      شي_cell_u*ڍير;
      شي_cell_u initial_chunk_[CHUNK_ڳڻپ];
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
	// Each slab needs a pointer to the allocated space.
      شي_cell_u**slab_pool;
      بنانشان slab_pool_ڳڻپ;
#جيڪڏهن_پورو
};

سانچو <size_t SLAB_سائيز, size_t CHUNK_ڳڻپ>
slab_t<SLAB_سائيز,CHUNK_ڳڻپ>::slab_t()
{
      pool = CHUNK_ڳڻپ;
      ڍير = initial_chunk_;
      لكل (بنانشان idx = 0 ; idx < CHUNK_ڳڻپ-1 ; idx += 1)
	    initial_chunk_[idx].ايندڙ_2 = initial_chunk_+idx+1;

      initial_chunk_[CHUNK_ڳڻپ-1].ايندڙ_2 = 0;
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
	// Initially we have no allocated space.
      slab_pool = عدم;
      slab_pool_ڳڻپ = 0;
#جيڪڏهن_پورو
}

سانچو <size_t SLAB_سائيز, size_t CHUNK_ڳڻپ>
مضمن فراغ* slab_t<SLAB_سائيز,CHUNK_ڳڻپ>::alloc_slab()
{
      إذا (ڍير == 0) {
	    شي_cell_u*chunk = new شي_cell_u[CHUNK_ڳڻپ];
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
	    slab_pool_ڳڻپ += 1;
	    slab_pool = static_cast<شي_cell_u **>(realloc(slab_pool,
	                slab_pool_ڳڻپ*حجم(شي_cell_u **)));
	    slab_pool[slab_pool_ڳڻپ-1] = chunk;
#جيڪڏهن_پورو
	    لكل (بنانشان idx = 0 ; idx < CHUNK_ڳڻپ ; idx += 1) {
		  chunk[idx].ايندڙ_2 = ڍير;
		  ڍير = chunk+idx;
	    }
	    pool += CHUNK_ڳڻپ;
      }

      شي_cell_u*cur = ڍير;
      ڍير = ڍير->ايندڙ_2;
      أرجع cur;
}

سانچو <size_t SLAB_سائيز, size_t CHUNK_ڳڻپ>
مضمن فراغ slab_t<SLAB_سائيز,CHUNK_ڳڻپ>::free_slab(فراغ*ptr)
{
      شي_cell_u*cur = reinterpret_cast<شي_cell_u*> (ptr);
      cur->ايندڙ_2 = ڍير;
      ڍير = cur;
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
سانچو <size_t SLAB_سائيز, size_t CHUNK_ڳڻپ>
مضمن فراغ slab_t<SLAB_سائيز,CHUNK_ڳڻپ>::delete_pool(فراغ)
{
      لكل (بنانشان idx = 0; idx < slab_pool_ڳڻپ; idx += 1) {
	    delete [] slab_pool[idx];
      }
      حرر(slab_pool);
      slab_pool = عدم;
      slab_pool_ڳڻپ = 0;
}
#جيڪڏهن_پورو


#جيڪڏهن_پورو /* IVL_slab_H */
