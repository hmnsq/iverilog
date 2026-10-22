#اگر_غیر_تعریف_شدہ IVL_symbols_H
#تعریف_کرو IVL_symbols_H
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
قسمتعریف class علامت_جدول_s *علامت_جدول_t;

قسمتعریف ساخت علامت_قدر_s {
      اتحاد {
	    vvp_net_t*net;
	    خالی*ptr;
      };
} علامت_قدر_t;


class علامت_جدول_s {
    عوامی:
      explicit علامت_جدول_s();
      مجازی ~علامت_جدول_s();
      علامت_جدول_s(ثابت علامت_جدول_s&) = delete;
      علامت_جدول_s& عامل=(ثابت علامت_جدول_s&) = delete;

	// This method locates the value in the symbol table and sets its
	// value. If the key doesn't yet exist, create it.
      خالی sym_سیٹ_قدر(ثابت حرف*کلید, علامت_قدر_t val);

	// This method locates the value in the symbol table and returns
	// it. If the value does not exist, create it, initialize it with
	// zero and return the zero value.
      علامت_قدر_t sym_get_قدر(ثابت حرف*کلید);

    نجی:
      ساخت درخت_گرہ*جڑ;
      ساخت کلید_لڑیاں*str_chunk;
      غیرعلامتی str_مستعمل;

      علامت_قدر_t ڈھونڈو_قدر(ساخت درخت_گرہ*cur,
				 ثابت حرف*کلید, علامت_قدر_t val,
				 bool force_flag);
      حرف*کلید_strdup(ثابت حرف*str);
};

/*
 * Create a new symbol table or release an existing one. A new symbol
 * table has no keys and no values. As a symbol table is built up, it
 * consumes more and more memory. When the table is no longer needed,
 * the delete_symbol_table method will delete the table, including all
 * the space for the keys.
 */
اندرونی علامت_جدول_t new_علامت_جدول(خالی) { واپس new علامت_جدول_s; }
اندرونی خالی delete_علامت_جدول(علامت_جدول_t tbl) { delete tbl; }

// These are obsolete, and here only to support older code.
اندرونی خالی sym_سیٹ_قدر(علامت_جدول_t tbl, ثابت حرف*کلید, علامت_قدر_t val)
{ tbl->sym_سیٹ_قدر(کلید, val); }

اندرونی علامت_قدر_t sym_get_قدر(علامت_جدول_t tbl, ثابت حرف*کلید)
{ واپس tbl->sym_get_قدر(کلید); }

/*
 * This template is a type-safe interface to the symbol table.
 */
سانچہ <class T> class علامت_map_s : نجی علامت_جدول_s {

    عوامی:
      خالی sym_سیٹ_قدر(ثابت حرف*کلید, T*val)
      { علامت_قدر_t tmp;
	tmp.ptr = val;
	علامت_جدول_s::sym_سیٹ_قدر(کلید, tmp);
      }

      T* sym_get_قدر(ثابت حرف*کلید)
      { علامت_قدر_t val = علامت_جدول_s::sym_get_قدر(کلید);
	واپس reinterpret_cast<T*>(val.ptr);
      }
};

#اگر_ختم /* IVL_symbols_H */
