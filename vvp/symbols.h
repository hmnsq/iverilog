#ifndef IVL_symbols_H
#define IVL_symbols_H
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
வகைவரை class குறியீடு_table_s *குறியீடு_table_t;

வகைவரை கட்டமைப்பு குறியீடு_மதிப்பு_s {
      ஒன்றிப்பு {
	    vvp_net_t*net;
	    வெறுமை*ptr;
      };
} குறியீடு_மதிப்பு_t;


class குறியீடு_table_s {
    பொது:
      explicit குறியீடு_table_s();
      மெய்நிகர் ~குறியீடு_table_s();
      குறியீடு_table_s(மாறிலி குறியீடு_table_s&) = delete;
      குறியீடு_table_s& operator=(மாறிலி குறியீடு_table_s&) = delete;

	// This method locates the value in the symbol table and sets its
	// value. If the key doesn't yet exist, create it.
      வெறுமை sym_set_மதிப்பு(மாறிலி எழுத்து*சாவி, குறியீடு_மதிப்பு_t val);

	// This method locates the value in the symbol table and returns
	// it. If the value does not exist, create it, initialize it with
	// zero and return the zero value.
      குறியீடு_மதிப்பு_t sym_get_மதிப்பு(மாறிலி எழுத்து*சாவி);

    தனிப்பட்ட:
      கட்டமைப்பு மர_முனை*வேர்;
      கட்டமைப்பு சாவி_strings*str_chunk;
      unsigned str_used;

      குறியீடு_மதிப்பு_t find_மதிப்பு(கட்டமைப்பு மர_முனை*cur,
				 மாறிலி எழுத்து*சாவி, குறியீடு_மதிப்பு_t val,
				 bool force_flag);
      எழுத்து*சாவி_strdup(மாறிலி எழுத்து*str);
};

/*
 * Create a new symbol table or release an existing one. A new symbol
 * table has no keys and no values. As a symbol table is built up, it
 * consumes more and more memory. When the table is no longer needed,
 * the delete_symbol_table method will delete the table, including all
 * the space for the keys.
 */
உள்வரி குறியீடு_table_t new_குறியீடு_table(வெறுமை) { திரும்பு new குறியீடு_table_s; }
உள்வரி வெறுமை delete_குறியீடு_table(குறியீடு_table_t tbl) { delete tbl; }

// These are obsolete, and here only to support older code.
உள்வரி வெறுமை sym_set_மதிப்பு(குறியீடு_table_t tbl, மாறிலி எழுத்து*சாவி, குறியீடு_மதிப்பு_t val)
{ tbl->sym_set_மதிப்பு(சாவி, val); }

உள்வரி குறியீடு_மதிப்பு_t sym_get_மதிப்பு(குறியீடு_table_t tbl, மாறிலி எழுத்து*சாவி)
{ திரும்பு tbl->sym_get_மதிப்பு(சாவி); }

/*
 * This template is a type-safe interface to the symbol table.
 */
template <class T> class குறியீடு_map_s : தனிப்பட்ட குறியீடு_table_s {

    பொது:
      வெறுமை sym_set_மதிப்பு(மாறிலி எழுத்து*சாவி, T*val)
      { குறியீடு_மதிப்பு_t tmp;
	tmp.ptr = val;
	குறியீடு_table_s::sym_set_மதிப்பு(சாவி, tmp);
      }

      T* sym_get_மதிப்பு(மாறிலி எழுத்து*சாவி)
      { குறியீடு_மதிப்பு_t val = குறியீடு_table_s::sym_get_மதிப்பு(சாவி);
	திரும்பு reinterpret_cast<T*>(val.ptr);
      }
};

#endif /* IVL_symbols_H */
