#جيڪڏهن_وصف_نه_ٿيل IVL_symbols_H
#وصف_ڪريو IVL_symbols_H
/*
 * Copyright (c) 2001-2026 Stephen Williams (steve@icarus.com)
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

/*
 * The symbol_table_t is intended as a means to hold and quickly index
 * large symbol tables with small symbol values. That is, the value
 * should fit in a 32bit unsigned integer (not even necessarily a
 * pointer.)
 *
 * The key is an unstructured ASCII string, terminated by a
 * null. Items added to the table are not removed, unless the entire
 * table is deleted.
 *
 * The compiler uses symbol tables to help match up operands to
 * referenced objects in the source. The compiler knows by the context
 * that the symbol appears what kind of thing is referenced, and so
 * what symbol table to look in.
 */

# include  "config.h"
# include  "vvp_net.h"

/*
 * This is the basic type of a symbol table. It is opaque. Don't even
 * try to look inside. The actual implementation is given in the
 * symbols.cc source file.
 */
تعريفنوع class نشان_جدول_s *نشان_جدول_t;

تعريفنوع بنية نشان_قدر_s {
      اتحاد {
	    vvp_net_t*net;
	    فراغ*ptr;
      };
} نشان_قدر_t;


class نشان_جدول_s {
    عام:
      explicit نشان_جدول_s();
      مجازي ~نشان_جدول_s();
      نشان_جدول_s(ثابت نشان_جدول_s&) = delete;
      نشان_جدول_s& عامل=(ثابت نشان_جدول_s&) = delete;

	// This method locates the value in the symbol table and sets its
	// value. If the key doesn't yet exist, create it.
      فراغ sym_سيٽ_قدر(ثابت حرف*ڪنجي, نشان_قدر_t val);

	// This method locates the value in the symbol table and returns
	// it. If the value does not exist, create it, initialize it with
	// zero and return the zero value.
      نشان_قدر_t sym_get_قدر(ثابت حرف*ڪنجي);

    خاص:
      بنية وڻ_ڳنڍ*پاڙ;
      بنية ڪنجي_لڙيون*str_chunk;
      بنانشان str_استعمال_ٿيل;

      نشان_قدر_t ڳوليو_قدر(بنية وڻ_ڳنڍ*cur,
				 ثابت حرف*ڪنجي, نشان_قدر_t val,
				 bool force_flag);
      حرف*ڪنجي_strdup(ثابت حرف*str);
};

/*
 * Create a new symbol table or release an existing one. A new symbol
 * table has no keys and no values. As a symbol table is built up, it
 * consumes more and more memory. When the table is no longer needed,
 * the delete_symbol_table method will delete the table, including all
 * the space for the keys.
 */
مضمن نشان_جدول_t new_نشان_جدول(فراغ) { أرجع new نشان_جدول_s; }
مضمن فراغ delete_نشان_جدول(نشان_جدول_t tbl) { delete tbl; }

// These are obsolete, and here only to support older code.
مضمن فراغ sym_سيٽ_قدر(نشان_جدول_t tbl, ثابت حرف*ڪنجي, نشان_قدر_t val)
{ tbl->sym_سيٽ_قدر(ڪنجي, val); }

مضمن نشان_قدر_t sym_get_قدر(نشان_جدول_t tbl, ثابت حرف*ڪنجي)
{ أرجع tbl->sym_get_قدر(ڪنجي); }

/*
 * This template is a type-safe interface to the symbol table.
 */
سانچو <class T> class نشان_map_s : خاص نشان_جدول_s {

    عام:
      فراغ sym_سيٽ_قدر(ثابت حرف*ڪنجي, T*val)
      { نشان_قدر_t tmp;
	tmp.ptr = val;
	نشان_جدول_s::sym_سيٽ_قدر(ڪنجي, tmp);
      }

      T* sym_get_قدر(ثابت حرف*ڪنجي)
      { نشان_قدر_t val = نشان_جدول_s::sym_get_قدر(ڪنجي);
	أرجع reinterpret_cast<T*>(val.ptr);
      }
};

#جيڪڏهن_پورو /* IVL_symbols_H */
