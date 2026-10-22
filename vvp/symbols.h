#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_symbols_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_symbols_H
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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ class ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_s *ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಚಿಹ್ನೆ_ಮೌಲ್ಯ_s {
      ಒಕ್ಕೂಟ {
	    vvp_net_t*net;
	    ಶೂನ್ಯ*ptr;
      };
} ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t;


class ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_s {
    ಸಾರ್ವಜನಿಕ:
      explicit ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_s();
      ವಾಸ್ತವಿಕ ~ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_s();
      ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_s(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_s&) = delete;
      ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_s& ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_s&) = delete;

	// This method locates the value in the symbol table and sets its
	// value. If the key doesn't yet exist, create it.
      ಶೂನ್ಯ sym_ಗಣ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಕೀಲಿ, ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t val);

	// This method locates the value in the symbol table and returns
	// it. If the value does not exist, create it, initialize it with
	// zero and return the zero value.
      ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t sym_get_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಕೀಲಿ);

    ಖಾಸಗಿ:
      ರಚನೆ ವೃಕ್ಷ_ಗಂಟು*ಬೇರು;
      ರಚನೆ ಕೀಲಿ_ಅಕ್ಷರಸರಣಿಗಳು*str_chunk;
      ಚಿಹ್ನೆರಹಿತ str_ಬಳಸಿದ;

      ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t ಕಂಡುಹಿಡಿ_ಮೌಲ್ಯ(ರಚನೆ ವೃಕ್ಷ_ಗಂಟು*cur,
				 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಕೀಲಿ, ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t val,
				 bool force_flag);
      ಅಕ್ಷರ*ಕೀಲಿ_strdup(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str);
};

/*
 * Create a new symbol table or release an existing one. A new symbol
 * table has no keys and no values. As a symbol table is built up, it
 * consumes more and more memory. When the table is no longer needed,
 * the delete_symbol_table method will delete the table, including all
 * the space for the keys.
 */
ಸಾಲೊಳಗೆ ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_t new_ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ(ಶೂನ್ಯ) { ಹಿಂತಿರುಗಿ new ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_s; }
ಸಾಲೊಳಗೆ ಶೂನ್ಯ delete_ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ(ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_t tbl) { delete tbl; }

// These are obsolete, and here only to support older code.
ಸಾಲೊಳಗೆ ಶೂನ್ಯ sym_ಗಣ_ಮೌಲ್ಯ(ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_t tbl, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಕೀಲಿ, ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t val)
{ tbl->sym_ಗಣ_ಮೌಲ್ಯ(ಕೀಲಿ, val); }

ಸಾಲೊಳಗೆ ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t sym_get_ಮೌಲ್ಯ(ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_t tbl, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಕೀಲಿ)
{ ಹಿಂತಿರುಗಿ tbl->sym_get_ಮೌಲ್ಯ(ಕೀಲಿ); }

/*
 * This template is a type-safe interface to the symbol table.
 */
ಮಾದರಿ <class T> class ಚಿಹ್ನೆ_map_s : ಖಾಸಗಿ ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_s {

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ sym_ಗಣ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಕೀಲಿ, T*val)
      { ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t tmp;
	tmp.ptr = val;
	ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_s::sym_ಗಣ_ಮೌಲ್ಯ(ಕೀಲಿ, tmp);
      }

      T* sym_get_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಕೀಲಿ)
      { ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t val = ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_s::sym_get_ಮೌಲ್ಯ(ಕೀಲಿ);
	ಹಿಂತಿರುಗಿ reinterpret_cast<T*>(val.ptr);
      }
};

#ಆದರೆ_ಅಂತ್ಯ /* IVL_symbols_H */
