#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_symbols_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_symbols_H
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ class ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_s *ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਚਿੰਨ੍ਹ_ਮੁੱਲ_s {
      ਸੰਘ {
	    vvp_net_t*net;
	    ਖਾਲੀ*ptr;
      };
} ਚਿੰਨ੍ਹ_ਮੁੱਲ_t;


class ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_s {
    ਜਨਤਕ:
      explicit ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_s();
      ਆਭਾਸੀ ~ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_s();
      ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_s(ਅਚਲ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_s&) = delete;
      ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_s& ਸੰਚਾਲਕ=(ਅਚਲ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_s&) = delete;

	// This method locates the value in the symbol table and sets its
	// value. If the key doesn't yet exist, create it.
      ਖਾਲੀ sym_ਸੈੱਟ_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ*ਕੁੰਜੀ, ਚਿੰਨ੍ਹ_ਮੁੱਲ_t val);

	// This method locates the value in the symbol table and returns
	// it. If the value does not exist, create it, initialize it with
	// zero and return the zero value.
      ਚਿੰਨ੍ਹ_ਮੁੱਲ_t sym_get_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ*ਕੁੰਜੀ);

    ਨਿਜੀ:
      ਬਣਤਰ ਰੁੱਖ_ਗੰਢ*ਜੜ੍ਹ;
      ਬਣਤਰ ਕੁੰਜੀ_ਲੜੀਆਂ*str_chunk;
      ਬਿਨਾਨਿਸ਼ਾਨੀ str_ਵਰਤਿਆ;

      ਚਿੰਨ੍ਹ_ਮੁੱਲ_t ਲੱਭੋ_ਮੁੱਲ(ਬਣਤਰ ਰੁੱਖ_ਗੰਢ*cur,
				 ਅਚਲ ਅੱਖਰ*ਕੁੰਜੀ, ਚਿੰਨ੍ਹ_ਮੁੱਲ_t val,
				 bool force_flag);
      ਅੱਖਰ*ਕੁੰਜੀ_strdup(ਅਚਲ ਅੱਖਰ*str);
};

/*
 * Create a new symbol table or release an existing one. A new symbol
 * table has no keys and no values. As a symbol table is built up, it
 * consumes more and more memory. When the table is no longer needed,
 * the delete_symbol_table method will delete the table, including all
 * the space for the keys.
 */
ਸਤਰਅੰਦਰ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_t new_ਚਿੰਨ੍ਹ_ਸਾਰਣੀ(ਖਾਲੀ) { ਵਾਪਸ new ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_s; }
ਸਤਰਅੰਦਰ ਖਾਲੀ delete_ਚਿੰਨ੍ਹ_ਸਾਰਣੀ(ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_t tbl) { delete tbl; }

// These are obsolete, and here only to support older code.
ਸਤਰਅੰਦਰ ਖਾਲੀ sym_ਸੈੱਟ_ਮੁੱਲ(ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_t tbl, ਅਚਲ ਅੱਖਰ*ਕੁੰਜੀ, ਚਿੰਨ੍ਹ_ਮੁੱਲ_t val)
{ tbl->sym_ਸੈੱਟ_ਮੁੱਲ(ਕੁੰਜੀ, val); }

ਸਤਰਅੰਦਰ ਚਿੰਨ੍ਹ_ਮੁੱਲ_t sym_get_ਮੁੱਲ(ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_t tbl, ਅਚਲ ਅੱਖਰ*ਕੁੰਜੀ)
{ ਵਾਪਸ tbl->sym_get_ਮੁੱਲ(ਕੁੰਜੀ); }

/*
 * This template is a type-safe interface to the symbol table.
 */
ਸਾਂਚਾ <class T> class ਚਿੰਨ੍ਹ_map_s : ਨਿਜੀ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_s {

    ਜਨਤਕ:
      ਖਾਲੀ sym_ਸੈੱਟ_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ*ਕੁੰਜੀ, T*val)
      { ਚਿੰਨ੍ਹ_ਮੁੱਲ_t tmp;
	tmp.ptr = val;
	ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_s::sym_ਸੈੱਟ_ਮੁੱਲ(ਕੁੰਜੀ, tmp);
      }

      T* sym_get_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ*ਕੁੰਜੀ)
      { ਚਿੰਨ੍ਹ_ਮੁੱਲ_t val = ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_s::sym_get_ਮੁੱਲ(ਕੁੰਜੀ);
	ਵਾਪਸ reinterpret_cast<T*>(val.ptr);
      }
};

#ਜੇ_ਅੰਤ /* IVL_symbols_H */
