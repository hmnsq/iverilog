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

#அந்தர்பாவ "vpi_priv.h"

கட்டமைப்பு __vpiArrayIterator : பொது __vpiHandle {
      முழு get_type_code(வெறுமை) மாறிலி override
      { திரும்பு vpiIterator; }

      vpiHandle vpi_index(முழு idx) override;
      free_object_fun_t free_object_fun(வெறுமை) override;

      கட்டமைப்பு __vpiArrayBase*array;
      unsigned next;
};

கட்டமைப்பு __vpiArrayIndex : பொது __vpiHandle {
      முழு get_type_code(வெறுமை) மாறிலி override
      { திரும்பு vpiIterator; }

      vpiHandle vpi_iterate(முழு code) override;
      vpiHandle vpi_index(முழு idx) override;
      free_object_fun_t free_object_fun(வெறுமை) override;

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
கட்டமைப்பு __vpiArrayWord {
      கட்டமைப்பு as_word_t : பொது __vpiHandle {
	    முழு get_type_code(வெறுமை) மாறிலி override { திரும்பு vpiMemoryWord; }
	    முழு vpi_get(முழு code) override;
	    எழுத்து*vpi_get_str(முழு code) override;
	    வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு vp) override;
	    vpiHandle vpi_put_மதிப்பு(p_vpi_மதிப்பு vp, முழு flags) override;
	    vpiHandle vpi_handle(முழு code) override;
      } as_word;

      கட்டமைப்பு as_index_t : பொது __vpiHandle {
	    முழு get_type_code(வெறுமை) மாறிலி override { திரும்பு vpiIndex; }
	    வெறுமை vpi_get_மதிப்பு(p_vpi_மதிப்பு val) override;
      } as_index;

      ஒன்றிப்பு {
	    கட்டமைப்பு __vpiArrayBase*parent;
	    கட்டமைப்பு __vpiArrayWord*word0;
      };

      உள்வரி unsigned get_index() மாறிலி { திரும்பு this - word0; }
      உள்வரி கட்டமைப்பு __vpiArrayBase*get_parent() மாறிலி { திரும்பு (word0 - 1)->parent; }
};

கட்டமைப்பு __vpiArrayWord*array_var_word_from_handle(vpiHandle ref);
கட்டமைப்பு __vpiArrayWord*array_var_index_from_handle(vpiHandle ref);

#endif /* ARRAY_COMMON_H */
