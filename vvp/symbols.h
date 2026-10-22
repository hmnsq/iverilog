#اگر_تعریف_نشده IVL_symbols_H
#تعریف_کن IVL_symbols_H
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
تعريفنوع class نماد_جدول_s *نماد_جدول_t;

تعريفنوع بنية نماد_مقدار_s {
      اتحاد {
	    vvp_net_t*net;
	    فراغ*ptr;
      };
} نماد_مقدار_t;


class نماد_جدول_s {
    عام:
      explicit نماد_جدول_s();
      مجازی ~نماد_جدول_s();
      نماد_جدول_s(ثابت نماد_جدول_s&) = delete;
      نماد_جدول_s& عملگر=(ثابت نماد_جدول_s&) = delete;

	// This method locates the value in the symbol table and sets its
	// value. If the key doesn't yet exist, create it.
      فراغ sym_مجموعه_مقدار(ثابت حرف*کلید, نماد_مقدار_t val);

	// This method locates the value in the symbol table and returns
	// it. If the value does not exist, create it, initialize it with
	// zero and return the zero value.
      نماد_مقدار_t sym_get_مقدار(ثابت حرف*کلید);

    خاص:
      بنية گره_درخت*ریشه;
      بنية کلید_رشتهها*str_chunk;
      بیعلامت str_استفاده_شده;

      نماد_مقدار_t پیدا_کن_مقدار(بنية گره_درخت*cur,
				 ثابت حرف*کلید, نماد_مقدار_t val,
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
مضمن نماد_جدول_t new_نماد_جدول(فراغ) { أرجع new نماد_جدول_s; }
مضمن فراغ delete_نماد_جدول(نماد_جدول_t tbl) { delete tbl; }

// These are obsolete, and here only to support older code.
مضمن فراغ sym_مجموعه_مقدار(نماد_جدول_t tbl, ثابت حرف*کلید, نماد_مقدار_t val)
{ tbl->sym_مجموعه_مقدار(کلید, val); }

مضمن نماد_مقدار_t sym_get_مقدار(نماد_جدول_t tbl, ثابت حرف*کلید)
{ أرجع tbl->sym_get_مقدار(کلید); }

/*
 * This template is a type-safe interface to the symbol table.
 */
قالب <class T> class نماد_map_s : خاص نماد_جدول_s {

    عام:
      فراغ sym_مجموعه_مقدار(ثابت حرف*کلید, T*val)
      { نماد_مقدار_t tmp;
	tmp.ptr = val;
	نماد_جدول_s::sym_مجموعه_مقدار(کلید, tmp);
      }

      T* sym_get_مقدار(ثابت حرف*کلید)
      { نماد_مقدار_t val = نماد_جدول_s::sym_get_مقدار(کلید);
	أرجع reinterpret_cast<T*>(val.ptr);
      }
};

#پایان_اگر /* IVL_symbols_H */
