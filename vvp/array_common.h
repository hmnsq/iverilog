/*
 * Copyright (c) 2014-2026 Stephen Williams (steve@icarus.com)
 * Copyright (c) 2014 CERN
 * @author Maciej Suminski <maciej.suminski@cern.ch>
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

#ifndef ARRAY_COMMON_H
#define ARRAY_COMMON_H

#ಅನ್ತರ್ಭಾವ "vpi_priv.h"

ಸಂರಚನಾ __vpiArrayIterator : ಸಾರ್ವಜನಿಕ __vpiHandle {
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override
      { ಪ್ರತ್ಯಾಗಮ vpiIterator; }

      vpiHandle vpi_index(ಪೂರ್ಣಾಙ್ಕ idx) override;
      free_object_fun_t free_object_fun(ಶೂನ್ಯ) override;

      ಸಂರಚನಾ __vpiArrayBase*array;
      unsigned next;
};

ಸಂರಚನಾ __vpiArrayIndex : ಸಾರ್ವಜನಿಕ __vpiHandle {
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override
      { ಪ್ರತ್ಯಾಗಮ vpiIterator; }

      vpiHandle vpi_iterate(ಪೂರ್ಣಾಙ್ಕ code) override;
      vpiHandle vpi_index(ಪೂರ್ಣಾಙ್ಕ idx) override;
      free_object_fun_t free_object_fun(ಶೂನ್ಯ) override;

      __vpiDecConst *index;
      unsigned done;
};

/*
 * The vpiArrayWord is magic. It is used as the handle to return when
 * vpi code tries to index or scan an array of variable words. The
 * array word handle contains no actual data. It is just a hook for
 * the vpi methods and to point to the parent.
 *
 * How the point to the parent works is tricky. The vpiArrayWord
 * objects for an array are themselves allocated as an array. All the
 * ArrayWord objects in the array have a word0 that points to the base
 * of the array. Thus, the position into the array (and the index into
 * the memory) is calculated by subtracting word0 from the ArrayWord
 * pointer.
 *
 * To then get to the parent, use word0[-1].parent.
 *
 * The vpiArrayWord is also used as a handle for the index (vpiIndex)
 * for the word. To make that work, return the pointer to the as_index
 * member instead of the as_word member. The result is a different set
 * of vpi functions is bound to the same structure. All the details
 * for the word also apply when treating this as an index.
 */
ಸಂರಚನಾ __vpiArrayWord {
      ಸಂರಚನಾ as_word_t : ಸಾರ್ವಜನಿಕ __vpiHandle {
	    ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiMemoryWord; }
	    ಪೂರ್ಣಾಙ್ಕ vpi_get(ಪೂರ್ಣಾಙ್ಕ code) override;
	    ವರ್ಣ*vpi_get_str(ಪೂರ್ಣಾಙ್ಕ code) override;
	    ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp) override;
	    vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಙ್ಕ flags) override;
	    vpiHandle vpi_handle(ಪೂರ್ಣಾಙ್ಕ code) override;
      } as_word;

      ಸಂರಚನಾ as_index_t : ಸಾರ್ವಜನಿಕ __vpiHandle {
	    ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ vpiIndex; }
	    ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      } as_index;

      ಸಙ್ಘ {
	    ಸಂರಚನಾ __vpiArrayBase*parent;
	    ಸಂರಚನಾ __vpiArrayWord*word0;
      };

      ಅನ್ತಃಸ್ಥ unsigned get_index() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ this - word0; }
      ಅನ್ತಃಸ್ಥ ಸಂರಚನಾ __vpiArrayBase*get_parent() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ (word0 - 1)->parent; }
};

ಸಂರಚನಾ __vpiArrayWord*array_var_word_from_handle(vpiHandle ref);
ಸಂರಚನಾ __vpiArrayWord*array_var_index_from_handle(vpiHandle ref);

#endif /* ARRAY_COMMON_H */
