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

#جيڪڏهن_وصف_نه_ٿيل ARRAY_COMMON_H
#وصف_ڪريو ARRAY_COMMON_H

#ضمن "vpi_priv.h"

بنية __vpiArrayIterator : عام __vpiHandle {
      صحيح get_type_ڪوڊ(فراغ) ثابت override
      { أرجع vpiIterator; }

      vpiHandle vpi_اشاريو(صحيح idx) override;
      free_object_fun_t free_object_fun(فراغ) override;

      بنية __vpiArrayBase*array;
      بنانشان ايندڙ_2;
};

بنية __vpiArrayIndex : عام __vpiHandle {
      صحيح get_type_ڪوڊ(فراغ) ثابت override
      { أرجع vpiIterator; }

      vpiHandle vpi_iterate(صحيح ڪوڊ) override;
      vpiHandle vpi_اشاريو(صحيح idx) override;
      free_object_fun_t free_object_fun(فراغ) override;

      __vpiDecConst *اشاريو;
      بنانشان پورو;
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
بنية __vpiArrayWord {
      بنية as_لفظ_t : عام __vpiHandle {
	    صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpi_يادگيري_لفظ; }
	    صحيح vpi_get(صحيح ڪوڊ) override;
	    حرف*vpi_get_str(صحيح ڪوڊ) override;
	    فراغ vpi_get_قدر(p_vpi_قدر vp) override;
	    vpiHandle vpi_put_قدر(p_vpi_قدر vp, صحيح flags) override;
	    vpiHandle vpi_handle(صحيح ڪوڊ) override;
      } as_لفظ;

      بنية as_اشاريو_t : عام __vpiHandle {
	    صحيح get_type_ڪوڊ(فراغ) ثابت override { أرجع vpi_اشاريو_2; }
	    فراغ vpi_get_قدر(p_vpi_قدر val) override;
      } as_اشاريو;

      اتحاد {
	    بنية __vpiArrayBase*والد;
	    بنية __vpiArrayWord*word0;
      };

      مضمن بنانشان get_اشاريو() ثابت { أرجع this - word0; }
      مضمن بنية __vpiArrayBase*get_والد() ثابت { أرجع (word0 - 1)->والد; }
};

بنية __vpiArrayWord*array_var_لفظ_from_handle(vpiHandle ref);
بنية __vpiArrayWord*array_var_اشاريو_from_handle(vpiHandle ref);

#جيڪڏهن_پورو /* ARRAY_COMMON_H */
