#अगर_अघोषित IVL_slab_H
#घोषणा IVL_slab_H
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

ढाँचा <माप_प्रकार SLAB_आकार, माप_प्रकार CHUNK_गिनती> class slab_t {

      जोड़ item_cell_u {
	    item_cell_u*अग्र_2;
	    अक्षर space[SLAB_आकार];
      };

    खुला:
      slab_t();

      व्योम* alloc_slab();
      व्योम  free_slab(व्योम*);
#अगर_घोषित CHECK_WITH_VALGRIND
	// If we have allocated memory then we need to delete it to make
	// valgrind happy.
      व्योम delete_pool(व्योम);
#पूर्ण_अगर

      अचिन्हित दीर्घ pool;

    निजी:
      item_cell_u*heap_;
      item_cell_u initial_chunk_[CHUNK_गिनती];
#अगर_घोषित CHECK_WITH_VALGRIND
	// Each slab needs a pointer to the allocated space.
      item_cell_u**slab_pool;
      अचिन्हित slab_pool_गिनती;
#पूर्ण_अगर
};

ढाँचा <माप_प्रकार SLAB_आकार, माप_प्रकार CHUNK_गिनती>
slab_t<SLAB_आकार,CHUNK_गिनती>::slab_t()
{
      pool = CHUNK_गिनती;
      heap_ = initial_chunk_;
      क्रम (अचिन्हित idx = 0 ; idx < CHUNK_गिनती-1 ; idx += 1)
	    initial_chunk_[idx].अग्र_2 = initial_chunk_+idx+1;

      initial_chunk_[CHUNK_गिनती-1].अग्र_2 = 0;
#अगर_घोषित CHECK_WITH_VALGRIND
	// Initially we have no allocated space.
      slab_pool = शून्य;
      slab_pool_गिनती = 0;
#पूर्ण_अगर
}

ढाँचा <माप_प्रकार SLAB_आकार, माप_प्रकार CHUNK_गिनती>
अंतरभूत व्योम* slab_t<SLAB_आकार,CHUNK_गिनती>::alloc_slab()
{
      अगर (heap_ == 0) {
	    item_cell_u*chunk = new item_cell_u[CHUNK_गिनती];
#अगर_घोषित CHECK_WITH_VALGRIND
	    slab_pool_गिनती += 1;
	    slab_pool = static_cast<item_cell_u **>(पुनः_स्मृति(slab_pool,
	                slab_pool_गिनती*माप(item_cell_u **)));
	    slab_pool[slab_pool_गिनती-1] = chunk;
#पूर्ण_अगर
	    क्रम (अचिन्हित idx = 0 ; idx < CHUNK_गिनती ; idx += 1) {
		  chunk[idx].अग्र_2 = heap_;
		  heap_ = chunk+idx;
	    }
	    pool += CHUNK_गिनती;
      }

      item_cell_u*cur = heap_;
      heap_ = heap_->अग्र_2;
      वापस cur;
}

ढाँचा <माप_प्रकार SLAB_आकार, माप_प्रकार CHUNK_गिनती>
अंतरभूत व्योम slab_t<SLAB_आकार,CHUNK_गिनती>::free_slab(व्योम*ptr)
{
      item_cell_u*cur = reinterpret_cast<item_cell_u*> (ptr);
      cur->अग्र_2 = heap_;
      heap_ = cur;
}

#अगर_घोषित CHECK_WITH_VALGRIND
ढाँचा <माप_प्रकार SLAB_आकार, माप_प्रकार CHUNK_गिनती>
अंतरभूत व्योम slab_t<SLAB_आकार,CHUNK_गिनती>::delete_pool(व्योम)
{
      क्रम (अचिन्हित idx = 0; idx < slab_pool_गिनती; idx += 1) {
	    delete [] slab_pool[idx];
      }
      मुक्त(slab_pool);
      slab_pool = शून्य;
      slab_pool_गिनती = 0;
}
#पूर्ण_अगर


#पूर्ण_अगर /* IVL_slab_H */
