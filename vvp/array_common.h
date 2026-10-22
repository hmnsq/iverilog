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

#अगर_अघोषित ARRAY_COMMON_H
#घोषणा ARRAY_COMMON_H

#समावेश "vpi_priv.h"

काष्ठा __vpiArrayIterator : खुला __vpiHandle {
      पूर्णांक get_type_code(व्योम) स्थिर override
      { वापस vpiIterator; }

      vpiHandle vpi_सूचकांक(पूर्णांक idx) override;
      free_object_fun_t free_object_fun(व्योम) override;

      काष्ठा __vpiArrayBase*array;
      अचिन्हित अग्र_2;
};

काष्ठा __vpiArrayIndex : खुला __vpiHandle {
      पूर्णांक get_type_code(व्योम) स्थिर override
      { वापस vpiIterator; }

      vpiHandle vpi_iterate(पूर्णांक code) override;
      vpiHandle vpi_सूचकांक(पूर्णांक idx) override;
      free_object_fun_t free_object_fun(व्योम) override;

      __vpiDecConst *सूचकांक;
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
काष्ठा __vpiArrayWord {
      काष्ठा as_word_t : खुला __vpiHandle {
	    पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpiMemoryWord; }
	    पूर्णांक vpi_get(पूर्णांक code) override;
	    अक्षर*vpi_get_str(पूर्णांक code) override;
	    व्योम vpi_get_मूल्य(p_vpi_मूल्य vp) override;
	    vpiHandle vpi_put_मूल्य(p_vpi_मूल्य vp, पूर्णांक flags) override;
	    vpiHandle vpi_handle(पूर्णांक code) override;
      } as_word;

      काष्ठा as_सूचकांक_t : खुला __vpiHandle {
	    पूर्णांक get_type_code(व्योम) स्थिर override { वापस vpi_सूचकांक_2; }
	    व्योम vpi_get_मूल्य(p_vpi_मूल्य val) override;
      } as_सूचकांक;

      जोड़ {
	    काष्ठा __vpiArrayBase*parent;
	    काष्ठा __vpiArrayWord*word0;
      };

      अंतरभूत अचिन्हित get_सूचकांक() स्थिर { वापस this - word0; }
      अंतरभूत काष्ठा __vpiArrayBase*get_parent() स्थिर { वापस (word0 - 1)->parent; }
};

काष्ठा __vpiArrayWord*array_var_word_from_handle(vpiHandle ref);
काष्ठा __vpiArrayWord*array_var_सूचकांक_from_handle(vpiHandle ref);

#पूर्ण_अगर /* ARRAY_COMMON_H */
