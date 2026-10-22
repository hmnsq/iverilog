#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_slab_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_slab_H
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

ಮಾದರಿ <size_t SLAB_ಗಾತ್ರ, size_t CHUNK_ಎಣಿಕೆ> class slab_t {

      ಒಕ್ಕೂಟ ಅಂಶ_cell_u {
	    ಅಂಶ_cell_u*ಮುಂದಿನ_2;
	    ಅಕ್ಷರ space[SLAB_ಗಾತ್ರ];
      };

    ಸಾರ್ವಜನಿಕ:
      slab_t();

      ಶೂನ್ಯ* alloc_slab();
      ಶೂನ್ಯ  free_slab(ಶೂನ್ಯ*);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
	// If we have allocated memory then we need to delete it to make
	// valgrind happy.
      ಶೂನ್ಯ delete_pool(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ pool;

    ಖಾಸಗಿ:
      ಅಂಶ_cell_u*ರಾಶಿ;
      ಅಂಶ_cell_u initial_chunk_[CHUNK_ಎಣಿಕೆ];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
	// Each slab needs a pointer to the allocated space.
      ಅಂಶ_cell_u**slab_pool;
      ಚಿಹ್ನೆರಹಿತ slab_pool_ಎಣಿಕೆ;
#ಆದರೆ_ಅಂತ್ಯ
};

ಮಾದರಿ <size_t SLAB_ಗಾತ್ರ, size_t CHUNK_ಎಣಿಕೆ>
slab_t<SLAB_ಗಾತ್ರ,CHUNK_ಎಣಿಕೆ>::slab_t()
{
      pool = CHUNK_ಎಣಿಕೆ;
      ರಾಶಿ = initial_chunk_;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < CHUNK_ಎಣಿಕೆ-1 ; idx += 1)
	    initial_chunk_[idx].ಮುಂದಿನ_2 = initial_chunk_+idx+1;

      initial_chunk_[CHUNK_ಎಣಿಕೆ-1].ಮುಂದಿನ_2 = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
	// Initially we have no allocated space.
      slab_pool = ಖಾಲಿ;
      slab_pool_ಎಣಿಕೆ = 0;
#ಆದರೆ_ಅಂತ್ಯ
}

ಮಾದರಿ <size_t SLAB_ಗಾತ್ರ, size_t CHUNK_ಎಣಿಕೆ>
ಸಾಲೊಳಗೆ ಶೂನ್ಯ* slab_t<SLAB_ಗಾತ್ರ,CHUNK_ಎಣಿಕೆ>::alloc_slab()
{
      ಆದರೆ (ರಾಶಿ == 0) {
	    ಅಂಶ_cell_u*chunk = new ಅಂಶ_cell_u[CHUNK_ಎಣಿಕೆ];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
	    slab_pool_ಎಣಿಕೆ += 1;
	    slab_pool = static_cast<ಅಂಶ_cell_u **>(realloc(slab_pool,
	                slab_pool_ಎಣಿಕೆ*ಗಾತ್ರ(ಅಂಶ_cell_u **)));
	    slab_pool[slab_pool_ಎಣಿಕೆ-1] = chunk;
#ಆದರೆ_ಅಂತ್ಯ
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < CHUNK_ಎಣಿಕೆ ; idx += 1) {
		  chunk[idx].ಮುಂದಿನ_2 = ರಾಶಿ;
		  ರಾಶಿ = chunk+idx;
	    }
	    pool += CHUNK_ಎಣಿಕೆ;
      }

      ಅಂಶ_cell_u*cur = ರಾಶಿ;
      ರಾಶಿ = ರಾಶಿ->ಮುಂದಿನ_2;
      ಹಿಂತಿರುಗಿ cur;
}

ಮಾದರಿ <size_t SLAB_ಗಾತ್ರ, size_t CHUNK_ಎಣಿಕೆ>
ಸಾಲೊಳಗೆ ಶೂನ್ಯ slab_t<SLAB_ಗಾತ್ರ,CHUNK_ಎಣಿಕೆ>::free_slab(ಶೂನ್ಯ*ptr)
{
      ಅಂಶ_cell_u*cur = reinterpret_cast<ಅಂಶ_cell_u*> (ptr);
      cur->ಮುಂದಿನ_2 = ರಾಶಿ;
      ರಾಶಿ = cur;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಮಾದರಿ <size_t SLAB_ಗಾತ್ರ, size_t CHUNK_ಎಣಿಕೆ>
ಸಾಲೊಳಗೆ ಶೂನ್ಯ slab_t<SLAB_ಗಾತ್ರ,CHUNK_ಎಣಿಕೆ>::delete_pool(ಶೂನ್ಯ)
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < slab_pool_ಎಣಿಕೆ; idx += 1) {
	    delete [] slab_pool[idx];
      }
      ಬಿಡುಗಡೆ(slab_pool);
      slab_pool = ಖಾಲಿ;
      slab_pool_ಎಣಿಕೆ = 0;
}
#ಆದರೆ_ಅಂತ್ಯ


#ಆದರೆ_ಅಂತ್ಯ /* IVL_slab_H */
