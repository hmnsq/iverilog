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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ARRAY_COMMON_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARRAY_COMMON_H

#ਅਨ੍ਤਰ੍ਭਾਵ "vpi_priv.h"

ਬਣਤਰ __vpiArrayIterator : ਜਨਤਕ __vpiHandle {
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override
      { ਵਾਪਸ vpiIterator; }

      vpiHandle vpi_ਸੂਚਕਾਂਕ(ਪੂਰਨ idx) override;
      free_object_fun_t free_object_fun(ਖਾਲੀ) override;

      ਬਣਤਰ __vpiArrayBase*array;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਗਲਾ_2;
};

ਬਣਤਰ __vpiArrayIndex : ਜਨਤਕ __vpiHandle {
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override
      { ਵਾਪਸ vpiIterator; }

      vpiHandle vpi_iterate(ਪੂਰਨ ਕੋਡ) override;
      vpiHandle vpi_ਸੂਚਕਾਂਕ(ਪੂਰਨ idx) override;
      free_object_fun_t free_object_fun(ਖਾਲੀ) override;

      __vpiDecConst *ਸੂਚਕਾਂਕ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਾ;
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
ਬਣਤਰ __vpiArrayWord {
      ਬਣਤਰ as_ਸ਼ਬਦ_t : ਜਨਤਕ __vpiHandle {
	    ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpi_ਸਮ੍ਰਿਤੀ_ਸ਼ਬਦ; }
	    ਪੂਰਨ vpi_get(ਪੂਰਨ ਕੋਡ) override;
	    ਅੱਖਰ*vpi_get_str(ਪੂਰਨ ਕੋਡ) override;
	    ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ vp) override;
	    vpiHandle vpi_put_ਮੁੱਲ(p_vpi_ਮੁੱਲ vp, ਪੂਰਨ flags) override;
	    vpiHandle vpi_handle(ਪੂਰਨ ਕੋਡ) override;
      } as_ਸ਼ਬਦ;

      ਬਣਤਰ as_ਸੂਚਕਾਂਕ_t : ਜਨਤਕ __vpiHandle {
	    ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override { ਵਾਪਸ vpi_ਸੂਚਕਾਂਕ_2; }
	    ਖਾਲੀ vpi_get_ਮੁੱਲ(p_vpi_ਮੁੱਲ val) override;
      } as_ਸੂਚਕਾਂਕ;

      ਸੰਘ {
	    ਬਣਤਰ __vpiArrayBase*ਮਾਪਾ;
	    ਬਣਤਰ __vpiArrayWord*word0;
      };

      ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ get_ਸੂਚਕਾਂਕ() ਅਚਲ { ਵਾਪਸ this - word0; }
      ਸਤਰਅੰਦਰ ਬਣਤਰ __vpiArrayBase*get_ਮਾਪਾ() ਅਚਲ { ਵਾਪਸ (word0 - 1)->ਮਾਪਾ; }
};

ਬਣਤਰ __vpiArrayWord*array_var_ਸ਼ਬਦ_from_handle(vpiHandle ref);
ਬਣਤਰ __vpiArrayWord*array_var_ਸੂਚਕਾਂਕ_from_handle(vpiHandle ref);

#ਜੇ_ਅੰਤ /* ARRAY_COMMON_H */
