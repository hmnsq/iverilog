#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_slab_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_slab_H
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

ਸਾਂਚਾ <size_t SLAB_ਮਾਪ, size_t CHUNK_ਗਿਣਤੀ> class slab_t {

      ਸੰਘ ਵਸਤੂ_cell_u {
	    ਵਸਤੂ_cell_u*ਅਗਲਾ_2;
	    ਅੱਖਰ space[SLAB_ਮਾਪ];
      };

    ਜਨਤਕ:
      slab_t();

      ਖਾਲੀ* alloc_slab();
      ਖਾਲੀ  free_slab(ਖਾਲੀ*);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
	// If we have allocated memory then we need to delete it to make
	// valgrind happy.
      ਖਾਲੀ delete_pool(ਖਾਲੀ);
#ਜੇ_ਅੰਤ

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ pool;

    ਨਿਜੀ:
      ਵਸਤੂ_cell_u*ਢੇਰ;
      ਵਸਤੂ_cell_u initial_chunk_[CHUNK_ਗਿਣਤੀ];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
	// Each slab needs a pointer to the allocated space.
      ਵਸਤੂ_cell_u**slab_pool;
      ਬਿਨਾਨਿਸ਼ਾਨੀ slab_pool_ਗਿਣਤੀ;
#ਜੇ_ਅੰਤ
};

ਸਾਂਚਾ <size_t SLAB_ਮਾਪ, size_t CHUNK_ਗਿਣਤੀ>
slab_t<SLAB_ਮਾਪ,CHUNK_ਗਿਣਤੀ>::slab_t()
{
      pool = CHUNK_ਗਿਣਤੀ;
      ਢੇਰ = initial_chunk_;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < CHUNK_ਗਿਣਤੀ-1 ; idx += 1)
	    initial_chunk_[idx].ਅਗਲਾ_2 = initial_chunk_+idx+1;

      initial_chunk_[CHUNK_ਗਿਣਤੀ-1].ਅਗਲਾ_2 = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
	// Initially we have no allocated space.
      slab_pool = ਨਿਰਾ;
      slab_pool_ਗਿਣਤੀ = 0;
#ਜੇ_ਅੰਤ
}

ਸਾਂਚਾ <size_t SLAB_ਮਾਪ, size_t CHUNK_ਗਿਣਤੀ>
ਸਤਰਅੰਦਰ ਖਾਲੀ* slab_t<SLAB_ਮਾਪ,CHUNK_ਗਿਣਤੀ>::alloc_slab()
{
      ਜੇ (ਢੇਰ == 0) {
	    ਵਸਤੂ_cell_u*chunk = new ਵਸਤੂ_cell_u[CHUNK_ਗਿਣਤੀ];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
	    slab_pool_ਗਿਣਤੀ += 1;
	    slab_pool = static_cast<ਵਸਤੂ_cell_u **>(realloc(slab_pool,
	                slab_pool_ਗਿਣਤੀ*ਆਕਾਰ(ਵਸਤੂ_cell_u **)));
	    slab_pool[slab_pool_ਗਿਣਤੀ-1] = chunk;
#ਜੇ_ਅੰਤ
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < CHUNK_ਗਿਣਤੀ ; idx += 1) {
		  chunk[idx].ਅਗਲਾ_2 = ਢੇਰ;
		  ਢੇਰ = chunk+idx;
	    }
	    pool += CHUNK_ਗਿਣਤੀ;
      }

      ਵਸਤੂ_cell_u*cur = ਢੇਰ;
      ਢੇਰ = ਢੇਰ->ਅਗਲਾ_2;
      ਵਾਪਸ cur;
}

ਸਾਂਚਾ <size_t SLAB_ਮਾਪ, size_t CHUNK_ਗਿਣਤੀ>
ਸਤਰਅੰਦਰ ਖਾਲੀ slab_t<SLAB_ਮਾਪ,CHUNK_ਗਿਣਤੀ>::free_slab(ਖਾਲੀ*ptr)
{
      ਵਸਤੂ_cell_u*cur = reinterpret_cast<ਵਸਤੂ_cell_u*> (ptr);
      cur->ਅਗਲਾ_2 = ਢੇਰ;
      ਢੇਰ = cur;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਸਾਂਚਾ <size_t SLAB_ਮਾਪ, size_t CHUNK_ਗਿਣਤੀ>
ਸਤਰਅੰਦਰ ਖਾਲੀ slab_t<SLAB_ਮਾਪ,CHUNK_ਗਿਣਤੀ>::delete_pool(ਖਾਲੀ)
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < slab_pool_ਗਿਣਤੀ; idx += 1) {
	    delete [] slab_pool[idx];
      }
      ਮੁਕਤ(slab_pool);
      slab_pool = ਨਿਰਾ;
      slab_pool_ਗਿਣਤੀ = 0;
}
#ਜੇ_ਅੰਤ


#ਜੇ_ਅੰਤ /* IVL_slab_H */
