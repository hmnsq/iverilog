#अगर_अघोषित IVL_symbols_H
#घोषणा IVL_symbols_H
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
प्रकार class प्रतीक_table_s *प्रतीक_table_t;

प्रकार काष्ठा प्रतीक_मूल्य_s {
      जोड़ {
	    vvp_net_t*net;
	    व्योम*ptr;
      };
} प्रतीक_मूल्य_t;


class प्रतीक_table_s {
    खुला:
      explicit प्रतीक_table_s();
      भव ~प्रतीक_table_s();
      प्रतीक_table_s(स्थिर प्रतीक_table_s&) = delete;
      प्रतीक_table_s& चालक=(स्थिर प्रतीक_table_s&) = delete;

	// This method locates the value in the symbol table and sets its
	// value. If the key doesn't yet exist, create it.
      व्योम sym_set_मूल्य(स्थिर अक्षर*कुंजी, प्रतीक_मूल्य_t val);

	// This method locates the value in the symbol table and returns
	// it. If the value does not exist, create it, initialize it with
	// zero and return the zero value.
      प्रतीक_मूल्य_t sym_get_मूल्य(स्थिर अक्षर*कुंजी);

    निजी:
      काष्ठा वृक्ष_गाँठ*मूल;
      काष्ठा कुंजी_strings*str_chunk;
      अचिन्हित str_used;

      प्रतीक_मूल्य_t find_मूल्य(काष्ठा वृक्ष_गाँठ*cur,
				 स्थिर अक्षर*कुंजी, प्रतीक_मूल्य_t val,
				 bool force_flag);
      अक्षर*कुंजी_strdup(स्थिर अक्षर*str);
};

/*
 * Create a new symbol table or release an existing one. A new symbol
 * table has no keys and no values. As a symbol table is built up, it
 * consumes more and more memory. When the table is no longer needed,
 * the delete_symbol_table method will delete the table, including all
 * the space for the keys.
 */
अंतरभूत प्रतीक_table_t new_प्रतीक_table(व्योम) { वापस new प्रतीक_table_s; }
अंतरभूत व्योम delete_प्रतीक_table(प्रतीक_table_t tbl) { delete tbl; }

// These are obsolete, and here only to support older code.
अंतरभूत व्योम sym_set_मूल्य(प्रतीक_table_t tbl, स्थिर अक्षर*कुंजी, प्रतीक_मूल्य_t val)
{ tbl->sym_set_मूल्य(कुंजी, val); }

अंतरभूत प्रतीक_मूल्य_t sym_get_मूल्य(प्रतीक_table_t tbl, स्थिर अक्षर*कुंजी)
{ वापस tbl->sym_get_मूल्य(कुंजी); }

/*
 * This template is a type-safe interface to the symbol table.
 */
ढाँचा <class T> class प्रतीक_map_s : निजी प्रतीक_table_s {

    खुला:
      व्योम sym_set_मूल्य(स्थिर अक्षर*कुंजी, T*val)
      { प्रतीक_मूल्य_t tmp;
	tmp.ptr = val;
	प्रतीक_table_s::sym_set_मूल्य(कुंजी, tmp);
      }

      T* sym_get_मूल्य(स्थिर अक्षर*कुंजी)
      { प्रतीक_मूल्य_t val = प्रतीक_table_s::sym_get_मूल्य(कुंजी);
	वापस reinterpret_cast<T*>(val.ptr);
      }
};

#पूर्ण_अगर /* IVL_symbols_H */
