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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ARRAY_COMMON_H
#ವ್ಯಾಖ್ಯಾನಿಸು ARRAY_COMMON_H

#ಅನ್ತರ್ಭಾವ "vpi_priv.h"

ರಚನೆ __vpiArrayIterator : ಸಾರ್ವಜನಿಕ __vpiHandle {
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override
      { ಹಿಂತಿರುಗಿ vpiIterator; }

      vpiHandle vpi_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ idx) override;
      free_object_fun_t free_object_fun(ಶೂನ್ಯ) override;

      ರಚನೆ __vpiArrayBase*array;
      ಚಿಹ್ನೆರಹಿತ ಮುಂದಿನ_2;
};

ರಚನೆ __vpiArrayIndex : ಸಾರ್ವಜನಿಕ __vpiHandle {
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override
      { ಹಿಂತಿರುಗಿ vpiIterator; }

      vpiHandle vpi_iterate(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      vpiHandle vpi_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ idx) override;
      free_object_fun_t free_object_fun(ಶೂನ್ಯ) override;

      __vpiDecConst *ಸೂಚ್ಯಂಕ;
      ಚಿಹ್ನೆರಹಿತ ಮುಗಿದ;
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
ರಚನೆ __vpiArrayWord {
      ರಚನೆ as_ಶಬ್ದ_t : ಸಾರ್ವಜನಿಕ __vpiHandle {
	    ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpi_ಸ್ಮರಣೆ_ಶಬ್ದ; }
	    ಪೂರ್ಣಾಂಕ vpi_get(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
	    ಅಕ್ಷರ*vpi_get_str(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
	    ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp) override;
	    vpiHandle vpi_put_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ vp, ಪೂರ್ಣಾಂಕ flags) override;
	    vpiHandle vpi_handle(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) override;
      } as_ಶಬ್ದ;

      ರಚನೆ as_ಸೂಚ್ಯಂಕ_t : ಸಾರ್ವಜನಿಕ __vpiHandle {
	    ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ vpi_ಸೂಚ್ಯಂಕ_2; }
	    ಶೂನ್ಯ vpi_get_ಮೌಲ್ಯ(p_vpi_ಮೌಲ್ಯ val) override;
      } as_ಸೂಚ್ಯಂಕ;

      ಒಕ್ಕೂಟ {
	    ರಚನೆ __vpiArrayBase*ಪೋಷಕ;
	    ರಚನೆ __vpiArrayWord*word0;
      };

      ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ get_ಸೂಚ್ಯಂಕ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ this - word0; }
      ಸಾಲೊಳಗೆ ರಚನೆ __vpiArrayBase*get_ಪೋಷಕ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ (word0 - 1)->ಪೋಷಕ; }
};

ರಚನೆ __vpiArrayWord*array_var_ಶಬ್ದ_from_handle(vpiHandle ref);
ರಚನೆ __vpiArrayWord*array_var_ಸೂಚ್ಯಂಕ_from_handle(vpiHandle ref);

#ಆದರೆ_ಅಂತ್ಯ /* ARRAY_COMMON_H */
