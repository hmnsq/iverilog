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

#অন্তর্ভাব "vpi_priv.h"

কাঠামো __vpiArrayIterator : সর্বজনীন __vpiHandle {
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override
      { ফেরত vpiIterator; }

      vpiHandle vpi_সূচক(পূর্ণসংখ্যা idx) override;
      free_object_fun_t free_object_fun(শূন্য) override;

      কাঠামো __vpiArrayBase*array;
      unsigned পরবর্তী_2;
};

কাঠামো __vpiArrayIndex : সর্বজনীন __vpiHandle {
      পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override
      { ফেরত vpiIterator; }

      vpiHandle vpi_iterate(পূর্ণসংখ্যা code) override;
      vpiHandle vpi_সূচক(পূর্ণসংখ্যা idx) override;
      free_object_fun_t free_object_fun(শূন্য) override;

      __vpiDecConst *সূচক;
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
কাঠামো __vpiArrayWord {
      কাঠামো as_word_t : সর্বজনীন __vpiHandle {
	    পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpiMemoryWord; }
	    পূর্ণসংখ্যা vpi_get(পূর্ণসংখ্যা code) override;
	    অক্ষর*vpi_get_str(পূর্ণসংখ্যা code) override;
	    শূন্য vpi_get_মান(p_vpi_মান vp) override;
	    vpiHandle vpi_put_মান(p_vpi_মান vp, পূর্ণসংখ্যা flags) override;
	    vpiHandle vpi_handle(পূর্ণসংখ্যা code) override;
      } as_word;

      কাঠামো as_সূচক_t : সর্বজনীন __vpiHandle {
	    পূর্ণসংখ্যা get_type_code(শূন্য) ধ্রুবক override { ফেরত vpi_সূচক_2; }
	    শূন্য vpi_get_মান(p_vpi_মান val) override;
      } as_সূচক;

      সংঘ {
	    কাঠামো __vpiArrayBase*parent;
	    কাঠামো __vpiArrayWord*word0;
      };

      অন্তঃরেখ unsigned get_সূচক() ধ্রুবক { ফেরত this - word0; }
      অন্তঃরেখ কাঠামো __vpiArrayBase*get_parent() ধ্রুবক { ফেরত (word0 - 1)->parent; }
};

কাঠামো __vpiArrayWord*array_var_word_from_handle(vpiHandle ref);
কাঠামো __vpiArrayWord*array_var_সূচক_from_handle(vpiHandle ref);

#endif /* ARRAY_COMMON_H */
