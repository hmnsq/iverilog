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

#অন্তর্ভাৱ "vpi_priv.h"

সংরচনা __vpiArrayIterator : সার্ৱজনিক __vpiHandle {
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুৱ override
      { ঘূৰাই vpiIterator; }

      vpiHandle vpi_index(পূর্ণাঙ্ক idx) override;
      free_object_fun_t free_object_fun(শূন্য) override;

      সংরচনা __vpiArrayBase*array;
      unsigned next;
};

সংরচনা __vpiArrayIndex : সার্ৱজনিক __vpiHandle {
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুৱ override
      { ঘূৰাই vpiIterator; }

      vpiHandle vpi_iterate(পূর্ণাঙ্ক code) override;
      vpiHandle vpi_index(পূর্ণাঙ্ক idx) override;
      free_object_fun_t free_object_fun(শূন্য) override;

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
সংরচনা __vpiArrayWord {
      সংরচনা as_word_t : সার্ৱজনিক __vpiHandle {
	    পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুৱ override { ঘূৰাই vpiMemoryWord; }
	    পূর্ণাঙ্ক vpi_get(পূর্ণাঙ্ক code) override;
	    ৱর্ণ*vpi_get_str(পূর্ণাঙ্ক code) override;
	    শূন্য vpi_get_মান(p_vpi_মান vp) override;
	    vpiHandle vpi_put_মান(p_vpi_মান vp, পূর্ণাঙ্ক flags) override;
	    vpiHandle vpi_handle(পূর্ণাঙ্ক code) override;
      } as_word;

      সংরচনা as_index_t : সার্ৱজনিক __vpiHandle {
	    পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুৱ override { ঘূৰাই vpiIndex; }
	    শূন্য vpi_get_মান(p_vpi_মান val) override;
      } as_index;

      সঙ্ঘ {
	    সংরচনা __vpiArrayBase*parent;
	    সংরচনা __vpiArrayWord*word0;
      };

      অন্তঃস্থ unsigned get_index() ধ্রুৱ { ঘূৰাই this - word0; }
      অন্তঃস্থ সংরচনা __vpiArrayBase*get_parent() ধ্রুৱ { ঘূৰাই (word0 - 1)->parent; }
};

সংরচনা __vpiArrayWord*array_var_word_from_handle(vpiHandle ref);
সংরচনা __vpiArrayWord*array_var_index_from_handle(vpiHandle ref);

#endif /* ARRAY_COMMON_H */
