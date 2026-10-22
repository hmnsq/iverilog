#اگر_تعریف_نشده IVL_slab_H
#تعریف_کن IVL_slab_H
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

قالب <size_t SLAB_اندازه, size_t CHUNK_شمار> class slab_t {

      اتحاد قلم_cell_u {
	    قلم_cell_u*بعدی_2;
	    حرف space[SLAB_اندازه];
      };

    عام:
      slab_t();

      فراغ* alloc_slab();
      فراغ  free_slab(فراغ*);
#اگر_تعریف_شده CHECK_WITH_VALGRIND
	// If we have allocated memory then we need to delete it to make
	// valgrind happy.
      فراغ delete_pool(فراغ);
#پایان_اگر

      بیعلامت دراز pool;

    خاص:
      قلم_cell_u*انباشته;
      قلم_cell_u initial_chunk_[CHUNK_شمار];
#اگر_تعریف_شده CHECK_WITH_VALGRIND
	// Each slab needs a pointer to the allocated space.
      قلم_cell_u**slab_pool;
      بیعلامت slab_pool_شمار;
#پایان_اگر
};

قالب <size_t SLAB_اندازه, size_t CHUNK_شمار>
slab_t<SLAB_اندازه,CHUNK_شمار>::slab_t()
{
      pool = CHUNK_شمار;
      انباشته = initial_chunk_;
      لكل (بیعلامت idx = 0 ; idx < CHUNK_شمار-1 ; idx += 1)
	    initial_chunk_[idx].بعدی_2 = initial_chunk_+idx+1;

      initial_chunk_[CHUNK_شمار-1].بعدی_2 = 0;
#اگر_تعریف_شده CHECK_WITH_VALGRIND
	// Initially we have no allocated space.
      slab_pool = عدم;
      slab_pool_شمار = 0;
#پایان_اگر
}

قالب <size_t SLAB_اندازه, size_t CHUNK_شمار>
مضمن فراغ* slab_t<SLAB_اندازه,CHUNK_شمار>::alloc_slab()
{
      إذا (انباشته == 0) {
	    قلم_cell_u*chunk = new قلم_cell_u[CHUNK_شمار];
#اگر_تعریف_شده CHECK_WITH_VALGRIND
	    slab_pool_شمار += 1;
	    slab_pool = static_cast<قلم_cell_u **>(realloc(slab_pool,
	                slab_pool_شمار*حجم(قلم_cell_u **)));
	    slab_pool[slab_pool_شمار-1] = chunk;
#پایان_اگر
	    لكل (بیعلامت idx = 0 ; idx < CHUNK_شمار ; idx += 1) {
		  chunk[idx].بعدی_2 = انباشته;
		  انباشته = chunk+idx;
	    }
	    pool += CHUNK_شمار;
      }

      قلم_cell_u*cur = انباشته;
      انباشته = انباشته->بعدی_2;
      أرجع cur;
}

قالب <size_t SLAB_اندازه, size_t CHUNK_شمار>
مضمن فراغ slab_t<SLAB_اندازه,CHUNK_شمار>::free_slab(فراغ*ptr)
{
      قلم_cell_u*cur = reinterpret_cast<قلم_cell_u*> (ptr);
      cur->بعدی_2 = انباشته;
      انباشته = cur;
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
قالب <size_t SLAB_اندازه, size_t CHUNK_شمار>
مضمن فراغ slab_t<SLAB_اندازه,CHUNK_شمار>::delete_pool(فراغ)
{
      لكل (بیعلامت idx = 0; idx < slab_pool_شمار; idx += 1) {
	    delete [] slab_pool[idx];
      }
      حرر(slab_pool);
      slab_pool = عدم;
      slab_pool_شمار = 0;
}
#پایان_اگر


#پایان_اگر /* IVL_slab_H */
