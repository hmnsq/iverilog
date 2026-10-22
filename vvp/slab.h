#यदि_अपरिभाषितम् IVL_slab_H
#परिभाषय IVL_slab_H
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

प्रतिरूपम् <size_t SLAB_आकार, size_t CHUNK_गणनम्> class slab_t {

      सङ्घ item_cell_u {
	    item_cell_u*अग्र_2;
	    वर्ण space[SLAB_आकार];
      };

    सार्वजनिक:
      slab_t();

      शून्य* alloc_slab();
      शून्य  free_slab(शून्य*);
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
	// If we have allocated memory then we need to delete it to make
	// valgrind happy.
      शून्य delete_pool(शून्य);
#यदि_अन्तः

      अचिह्नित दीर्घ pool;

    गुप्त:
      item_cell_u*heap_;
      item_cell_u initial_chunk_[CHUNK_गणनम्];
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
	// Each slab needs a pointer to the allocated space.
      item_cell_u**slab_pool;
      अचिह्नित slab_pool_गणनम्;
#यदि_अन्तः
};

प्रतिरूपम् <size_t SLAB_आकार, size_t CHUNK_गणनम्>
slab_t<SLAB_आकार,CHUNK_गणनम्>::slab_t()
{
      pool = CHUNK_गणनम्;
      heap_ = initial_chunk_;
      प्रति (अचिह्नित idx = 0 ; idx < CHUNK_गणनम्-1 ; idx += 1)
	    initial_chunk_[idx].अग्र_2 = initial_chunk_+idx+1;

      initial_chunk_[CHUNK_गणनम्-1].अग्र_2 = 0;
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
	// Initially we have no allocated space.
      slab_pool = रिक्त;
      slab_pool_गणनम् = 0;
#यदि_अन्तः
}

प्रतिरूपम् <size_t SLAB_आकार, size_t CHUNK_गणनम्>
अन्तःस्थ शून्य* slab_t<SLAB_आकार,CHUNK_गणनम्>::alloc_slab()
{
      यदि (heap_ == 0) {
	    item_cell_u*chunk = new item_cell_u[CHUNK_गणनम्];
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
	    slab_pool_गणनम् += 1;
	    slab_pool = static_cast<item_cell_u **>(realloc(slab_pool,
	                slab_pool_गणनम्*परिमाण(item_cell_u **)));
	    slab_pool[slab_pool_गणनम्-1] = chunk;
#यदि_अन्तः
	    प्रति (अचिह्नित idx = 0 ; idx < CHUNK_गणनम् ; idx += 1) {
		  chunk[idx].अग्र_2 = heap_;
		  heap_ = chunk+idx;
	    }
	    pool += CHUNK_गणनम्;
      }

      item_cell_u*cur = heap_;
      heap_ = heap_->अग्र_2;
      प्रत्यागम cur;
}

प्रतिरूपम् <size_t SLAB_आकार, size_t CHUNK_गणनम्>
अन्तःस्थ शून्य slab_t<SLAB_आकार,CHUNK_गणनम्>::free_slab(शून्य*ptr)
{
      item_cell_u*cur = reinterpret_cast<item_cell_u*> (ptr);
      cur->अग्र_2 = heap_;
      heap_ = cur;
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
प्रतिरूपम् <size_t SLAB_आकार, size_t CHUNK_गणनम्>
अन्तःस्थ शून्य slab_t<SLAB_आकार,CHUNK_गणनम्>::delete_pool(शून्य)
{
      प्रति (अचिह्नित idx = 0; idx < slab_pool_गणनम्; idx += 1) {
	    delete [] slab_pool[idx];
      }
      मुक्त(slab_pool);
      slab_pool = रिक्त;
      slab_pool_गणनम् = 0;
}
#यदि_अन्तः


#यदि_अन्तः /* IVL_slab_H */
