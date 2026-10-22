#که_نه_تعریف_شوی IVL_slab_H
#تعریف_کړه IVL_slab_H
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

کينډۍ <size_t SLAB_اندازه, size_t CHUNK_شمېر> class slab_t {

      اتحاد توکی_cell_u {
	    توکی_cell_u*بل_2;
	    حرف space[SLAB_اندازه];
      };

    عام:
      slab_t();

      فراغ* alloc_slab();
      فراغ  free_slab(فراغ*);
#که_تعریف_شوی CHECK_WITH_VALGRIND
	// If we have allocated memory then we need to delete it to make
	// valgrind happy.
      فراغ delete_pool(فراغ);
#د_که_پای

      بینښه اوږد pool;

    خاص:
      توکی_cell_u*ډېرۍ;
      توکی_cell_u initial_chunk_[CHUNK_شمېر];
#که_تعریف_شوی CHECK_WITH_VALGRIND
	// Each slab needs a pointer to the allocated space.
      توکی_cell_u**slab_pool;
      بینښه slab_pool_شمېر;
#د_که_پای
};

کينډۍ <size_t SLAB_اندازه, size_t CHUNK_شمېر>
slab_t<SLAB_اندازه,CHUNK_شمېر>::slab_t()
{
      pool = CHUNK_شمېر;
      ډېرۍ = initial_chunk_;
      لكل (بینښه idx = 0 ; idx < CHUNK_شمېر-1 ; idx += 1)
	    initial_chunk_[idx].بل_2 = initial_chunk_+idx+1;

      initial_chunk_[CHUNK_شمېر-1].بل_2 = 0;
#که_تعریف_شوی CHECK_WITH_VALGRIND
	// Initially we have no allocated space.
      slab_pool = عدم;
      slab_pool_شمېر = 0;
#د_که_پای
}

کينډۍ <size_t SLAB_اندازه, size_t CHUNK_شمېر>
مضمن فراغ* slab_t<SLAB_اندازه,CHUNK_شمېر>::alloc_slab()
{
      إذا (ډېرۍ == 0) {
	    توکی_cell_u*chunk = new توکی_cell_u[CHUNK_شمېر];
#که_تعریف_شوی CHECK_WITH_VALGRIND
	    slab_pool_شمېر += 1;
	    slab_pool = static_cast<توکی_cell_u **>(realloc(slab_pool,
	                slab_pool_شمېر*حجم(توکی_cell_u **)));
	    slab_pool[slab_pool_شمېر-1] = chunk;
#د_که_پای
	    لكل (بینښه idx = 0 ; idx < CHUNK_شمېر ; idx += 1) {
		  chunk[idx].بل_2 = ډېرۍ;
		  ډېرۍ = chunk+idx;
	    }
	    pool += CHUNK_شمېر;
      }

      توکی_cell_u*cur = ډېرۍ;
      ډېرۍ = ډېرۍ->بل_2;
      أرجع cur;
}

کينډۍ <size_t SLAB_اندازه, size_t CHUNK_شمېر>
مضمن فراغ slab_t<SLAB_اندازه,CHUNK_شمېر>::free_slab(فراغ*ptr)
{
      توکی_cell_u*cur = reinterpret_cast<توکی_cell_u*> (ptr);
      cur->بل_2 = ډېرۍ;
      ډېرۍ = cur;
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
کينډۍ <size_t SLAB_اندازه, size_t CHUNK_شمېر>
مضمن فراغ slab_t<SLAB_اندازه,CHUNK_شمېر>::delete_pool(فراغ)
{
      لكل (بینښه idx = 0; idx < slab_pool_شمېر; idx += 1) {
	    delete [] slab_pool[idx];
      }
      حرر(slab_pool);
      slab_pool = عدم;
      slab_pool_شمېر = 0;
}
#د_که_پای


#د_که_پای /* IVL_slab_H */
