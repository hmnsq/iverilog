#यदि_अपरिभाषितम् IVL_symbols_H
#परिभाषय IVL_symbols_H
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
प्रकारनिर्देश class चिह्नम्_सारणी_s *चिह्नम्_सारणी_t;

प्रकारनिर्देश संरचना चिह्नम्_मानम्_s {
      सङ्घ {
	    vvp_net_t*net;
	    शून्य*ptr;
      };
} चिह्नम्_मानम्_t;


class चिह्नम्_सारणी_s {
    सार्वजनिक:
      explicit चिह्नम्_सारणी_s();
      आभास ~चिह्नम्_सारणी_s();
      चिह्नम्_सारणी_s(ध्रुव चिह्नम्_सारणी_s&) = delete;
      चिह्नम्_सारणी_s& संक्रियकः=(ध्रुव चिह्नम्_सारणी_s&) = delete;

	// This method locates the value in the symbol table and sets its
	// value. If the key doesn't yet exist, create it.
      शून्य sym_समुच्चयः_मानम्(ध्रुव वर्ण*कुञ्चिका, चिह्नम्_मानम्_t val);

	// This method locates the value in the symbol table and returns
	// it. If the value does not exist, create it, initialize it with
	// zero and return the zero value.
      चिह्नम्_मानम्_t sym_get_मानम्(ध्रुव वर्ण*कुञ्चिका);

    गुप्त:
      संरचना वृक्ष_ग्रन्थिः*मूलम्;
      संरचना कुञ्चिका_वाक्यानि*str_chunk;
      अचिह्नित str_प्रयुक्तम्;

      चिह्नम्_मानम्_t find_मानम्(संरचना वृक्ष_ग्रन्थिः*cur,
				 ध्रुव वर्ण*कुञ्चिका, चिह्नम्_मानम्_t val,
				 bool force_flag);
      वर्ण*कुञ्चिका_strdup(ध्रुव वर्ण*str);
};

/*
 * Create a new symbol table or release an existing one. A new symbol
 * table has no keys and no values. As a symbol table is built up, it
 * consumes more and more memory. When the table is no longer needed,
 * the delete_symbol_table method will delete the table, including all
 * the space for the keys.
 */
अन्तःस्थ चिह्नम्_सारणी_t new_चिह्नम्_सारणी(शून्य) { प्रत्यागम new चिह्नम्_सारणी_s; }
अन्तःस्थ शून्य delete_चिह्नम्_सारणी(चिह्नम्_सारणी_t tbl) { delete tbl; }

// These are obsolete, and here only to support older code.
अन्तःस्थ शून्य sym_समुच्चयः_मानम्(चिह्नम्_सारणी_t tbl, ध्रुव वर्ण*कुञ्चिका, चिह्नम्_मानम्_t val)
{ tbl->sym_समुच्चयः_मानम्(कुञ्चिका, val); }

अन्तःस्थ चिह्नम्_मानम्_t sym_get_मानम्(चिह्नम्_सारणी_t tbl, ध्रुव वर्ण*कुञ्चिका)
{ प्रत्यागम tbl->sym_get_मानम्(कुञ्चिका); }

/*
 * This template is a type-safe interface to the symbol table.
 */
प्रतिरूपम् <class T> class चिह्नम्_map_s : गुप्त चिह्नम्_सारणी_s {

    सार्वजनिक:
      शून्य sym_समुच्चयः_मानम्(ध्रुव वर्ण*कुञ्चिका, T*val)
      { चिह्नम्_मानम्_t tmp;
	tmp.ptr = val;
	चिह्नम्_सारणी_s::sym_समुच्चयः_मानम्(कुञ्चिका, tmp);
      }

      T* sym_get_मानम्(ध्रुव वर्ण*कुञ्चिका)
      { चिह्नम्_मानम्_t val = चिह्नम्_सारणी_s::sym_get_मानम्(कुञ्चिका);
	प्रत्यागम reinterpret_cast<T*>(val.ptr);
      }
};

#यदि_अन्तः /* IVL_symbols_H */
