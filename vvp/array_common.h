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

#include "vpi_priv.h"

struct __vpiArrayIterator : public __vpiHandle {
      整数 get_type_code(void) const override
      { 返回 vpiIterator; }

      vpiHandle vpi_index(整数 idx) override;
      free_object_fun_t free_object_fun(void) override;

      struct __vpiArrayBase*array;
      unsigned next;
};

struct __vpiArrayIndex : public __vpiHandle {
      整数 get_type_code(void) const override
      { 返回 vpiIterator; }

      vpiHandle vpi_iterate(整数 code) override;
      vpiHandle vpi_index(整数 idx) override;
      free_object_fun_t free_object_fun(void) override;

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
struct __vpiArrayWord {
      struct as_word_t : public __vpiHandle {
	    整数 get_type_code(void) const override { 返回 vpiMemoryWord; }
	    整数 vpi_get(整数 code) override;
	    char*vpi_get_str(整数 code) override;
	    void vpi_get_value(p_vpi_value vp) override;
	    vpiHandle vpi_put_value(p_vpi_value vp, 整数 flags) override;
	    vpiHandle vpi_handle(整数 code) override;
      } as_word;

      struct as_index_t : public __vpiHandle {
	    整数 get_type_code(void) const override { 返回 vpiIndex; }
	    void vpi_get_value(p_vpi_value val) override;
      } as_index;

      union {
	    struct __vpiArrayBase*parent;
	    struct __vpiArrayWord*word0;
      };

      inline unsigned get_index() const { 返回 this - word0; }
      inline struct __vpiArrayBase*get_parent() const { 返回 (word0 - 1)->parent; }
};

struct __vpiArrayWord*array_var_word_from_handle(vpiHandle ref);
struct __vpiArrayWord*array_var_index_from_handle(vpiHandle ref);

#endif /* ARRAY_COMMON_H */
