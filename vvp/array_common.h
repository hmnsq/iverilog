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

#അന്തര്ഭാവ "vpi_priv.h"

ഘടന __vpiArrayIterator : പൊതു __vpiHandle {
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override
      { തിരികെ vpiIterator; }

      vpiHandle vpi_index(പൂർണ്ണസംഖ്യ idx) override;
      free_object_fun_t free_object_fun(ശൂന്യം) override;

      ഘടന __vpiArrayBase*array;
      unsigned next;
};

ഘടന __vpiArrayIndex : പൊതു __vpiHandle {
      പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override
      { തിരികെ vpiIterator; }

      vpiHandle vpi_iterate(പൂർണ്ണസംഖ്യ code) override;
      vpiHandle vpi_index(പൂർണ്ണസംഖ്യ idx) override;
      free_object_fun_t free_object_fun(ശൂന്യം) override;

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
ഘടന __vpiArrayWord {
      ഘടന as_word_t : പൊതു __vpiHandle {
	    പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiMemoryWord; }
	    പൂർണ്ണസംഖ്യ vpi_get(പൂർണ്ണസംഖ്യ code) override;
	    അക്ഷരം*vpi_get_str(പൂർണ്ണസംഖ്യ code) override;
	    ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം vp) override;
	    vpiHandle vpi_put_മൂല്യം(p_vpi_മൂല്യം vp, പൂർണ്ണസംഖ്യ flags) override;
	    vpiHandle vpi_handle(പൂർണ്ണസംഖ്യ code) override;
      } as_word;

      ഘടന as_index_t : പൊതു __vpiHandle {
	    പൂർണ്ണസംഖ്യ get_type_code(ശൂന്യം) സ്ഥിരം override { തിരികെ vpiIndex; }
	    ശൂന്യം vpi_get_മൂല്യം(p_vpi_മൂല്യം val) override;
      } as_index;

      സംഘം {
	    ഘടന __vpiArrayBase*parent;
	    ഘടന __vpiArrayWord*word0;
      };

      വരിയിൽ unsigned get_index() സ്ഥിരം { തിരികെ this - word0; }
      വരിയിൽ ഘടന __vpiArrayBase*get_parent() സ്ഥിരം { തിരികെ (word0 - 1)->parent; }
};

ഘടന __vpiArrayWord*array_var_word_from_handle(vpiHandle ref);
ഘടന __vpiArrayWord*array_var_index_from_handle(vpiHandle ref);

#endif /* ARRAY_COMMON_H */
