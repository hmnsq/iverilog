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

#اگر_غیر_تعریف_شدہ ARRAY_COMMON_H
#تعریف_کرو ARRAY_COMMON_H

#شامل "vpi_priv.h"

ساخت __vpiArrayIterator : عوامی __vpiHandle {
      صحیح get_type_رمز(خالی) ثابت override
      { واپس vpiIterator; }

      vpiHandle vpi_اشاریہ(صحیح idx) override;
      free_object_fun_t free_object_fun(خالی) override;

      ساخت __vpiArrayBase*array;
      غیرعلامتی اگلا_2;
};

ساخت __vpiArrayIndex : عوامی __vpiHandle {
      صحیح get_type_رمز(خالی) ثابت override
      { واپس vpiIterator; }

      vpiHandle vpi_iterate(صحیح رمز) override;
      vpiHandle vpi_اشاریہ(صحیح idx) override;
      free_object_fun_t free_object_fun(خالی) override;

      __vpiDecConst *اشاریہ;
      غیرعلامتی مکمل;
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
ساخت __vpiArrayWord {
      ساخت as_لفظ_t : عوامی __vpiHandle {
	    صحیح get_type_رمز(خالی) ثابت override { واپس vpi_حافظہ_لفظ; }
	    صحیح vpi_get(صحیح رمز) override;
	    حرف*vpi_get_str(صحیح رمز) override;
	    خالی vpi_get_قدر(p_vpi_قدر vp) override;
	    vpiHandle vpi_put_قدر(p_vpi_قدر vp, صحیح flags) override;
	    vpiHandle vpi_handle(صحیح رمز) override;
      } as_لفظ;

      ساخت as_اشاریہ_t : عوامی __vpiHandle {
	    صحیح get_type_رمز(خالی) ثابت override { واپس vpi_اشاریہ_2; }
	    خالی vpi_get_قدر(p_vpi_قدر val) override;
      } as_اشاریہ;

      اتحاد {
	    ساخت __vpiArrayBase*والد;
	    ساخت __vpiArrayWord*word0;
      };

      اندرونی غیرعلامتی get_اشاریہ() ثابت { واپس this - word0; }
      اندرونی ساخت __vpiArrayBase*get_والد() ثابت { واپس (word0 - 1)->والد; }
};

ساخت __vpiArrayWord*array_var_لفظ_from_handle(vpiHandle ref);
ساخت __vpiArrayWord*array_var_اشاریہ_from_handle(vpiHandle ref);

#اگر_ختم /* ARRAY_COMMON_H */
