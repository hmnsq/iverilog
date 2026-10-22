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

#अन्तर्भाव "vpi_priv.h"

संरचना __vpiArrayIterator : सार्वजनिक __vpiHandle {
      पूर्णांक get_type_code(शून्य) स्थिरांक override
      { परत vpiIterator; }

      vpiHandle vpi_index(पूर्णांक idx) override;
      free_object_fun_t free_object_fun(शून्य) override;

      संरचना __vpiArrayBase*array;
      अचिन्हित next;
};

संरचना __vpiArrayIndex : सार्वजनिक __vpiHandle {
      पूर्णांक get_type_code(शून्य) स्थिरांक override
      { परत vpiIterator; }

      vpiHandle vpi_iterate(पूर्णांक code) override;
      vpiHandle vpi_index(पूर्णांक idx) override;
      free_object_fun_t free_object_fun(शून्य) override;

      __vpiDecConst *index;
      अचिन्हित done;
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
संरचना __vpiArrayWord {
      संरचना as_word_t : सार्वजनिक __vpiHandle {
	    पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiMemoryWord; }
	    पूर्णांक vpi_get(पूर्णांक code) override;
	    अक्षर*vpi_get_str(पूर्णांक code) override;
	    शून्य vpi_get_मूल्य(p_vpi_मूल्य vp) override;
	    vpiHandle vpi_put_मूल्य(p_vpi_मूल्य vp, पूर्णांक flags) override;
	    vpiHandle vpi_handle(पूर्णांक code) override;
      } as_word;

      संरचना as_index_t : सार्वजनिक __vpiHandle {
	    पूर्णांक get_type_code(शून्य) स्थिरांक override { परत vpiIndex; }
	    शून्य vpi_get_मूल्य(p_vpi_मूल्य val) override;
      } as_index;

      संघ {
	    संरचना __vpiArrayBase*parent;
	    संरचना __vpiArrayWord*word0;
      };

      ओळीत अचिन्हित get_index() स्थिरांक { परत this - word0; }
      ओळीत संरचना __vpiArrayBase*get_parent() स्थिरांक { परत (word0 - 1)->parent; }
};

संरचना __vpiArrayWord*array_var_word_from_handle(vpiHandle ref);
संरचना __vpiArrayWord*array_var_index_from_handle(vpiHandle ref);

#endif /* ARRAY_COMMON_H */
