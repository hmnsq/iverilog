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
ପ୍ରକାରବ୍ୟାଖ୍ୟା class ପ୍ରତୀକ_table_s *ପ୍ରତୀକ_table_t;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା ପ୍ରତୀକ_ମୂଲ୍ୟ_s {
      ସଂଘ {
	    vvp_net_t*net;
	    ଶୂନ୍ୟ*ptr;
      };
} ପ୍ରତୀକ_ମୂଲ୍ୟ_t;


class ପ୍ରତୀକ_table_s {
    ସର୍ବସାଧାରଣ:
      explicit ପ୍ରତୀକ_table_s();
      ଆଭାସୀ ~ପ୍ରତୀକ_table_s();
      ପ୍ରତୀକ_table_s(ଅଚଳ ପ୍ରତୀକ_table_s&) = delete;
      ପ୍ରତୀକ_table_s& operator=(ଅଚଳ ପ୍ରତୀକ_table_s&) = delete;

	// This method locates the value in the symbol table and sets its
	// value. If the key doesn't yet exist, create it.
      ଶୂନ୍ୟ sym_set_ମୂଲ୍ୟ(ଅଚଳ ଅକ୍ଷର*ଚାବି, ପ୍ରତୀକ_ମୂଲ୍ୟ_t val);

	// This method locates the value in the symbol table and returns
	// it. If the value does not exist, create it, initialize it with
	// zero and return the zero value.
      ପ୍ରତୀକ_ମୂଲ୍ୟ_t sym_get_ମୂଲ୍ୟ(ଅଚଳ ଅକ୍ଷର*ଚାବି);

    ବ୍ୟକ୍ତିଗତ:
      ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି*ମୂଳ;
      ସଂରଚନା ଚାବି_strings*str_chunk;
      unsigned str_used;

      ପ୍ରତୀକ_ମୂଲ୍ୟ_t find_ମୂଲ୍ୟ(ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି*cur,
				 ଅଚଳ ଅକ୍ଷର*ଚାବି, ପ୍ରତୀକ_ମୂଲ୍ୟ_t val,
				 bool force_flag);
      ଅକ୍ଷର*ଚାବି_strdup(ଅଚଳ ଅକ୍ଷର*str);
};

/*
 * Create a new symbol table or release an existing one. A new symbol
 * table has no keys and no values. As a symbol table is built up, it
 * consumes more and more memory. When the table is no longer needed,
 * the delete_symbol_table method will delete the table, including all
 * the space for the keys.
 */
ଧାଡ଼ିଭିତରେ ପ୍ରତୀକ_table_t new_ପ୍ରତୀକ_table(ଶୂନ୍ୟ) { ଫେରନ୍ତୁ new ପ୍ରତୀକ_table_s; }
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ delete_ପ୍ରତୀକ_table(ପ୍ରତୀକ_table_t tbl) { delete tbl; }

// These are obsolete, and here only to support older code.
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ sym_set_ମୂଲ୍ୟ(ପ୍ରତୀକ_table_t tbl, ଅଚଳ ଅକ୍ଷର*ଚାବି, ପ୍ରତୀକ_ମୂଲ୍ୟ_t val)
{ tbl->sym_set_ମୂଲ୍ୟ(ଚାବି, val); }

ଧାଡ଼ିଭିତରେ ପ୍ରତୀକ_ମୂଲ୍ୟ_t sym_get_ମୂଲ୍ୟ(ପ୍ରତୀକ_table_t tbl, ଅଚଳ ଅକ୍ଷର*ଚାବି)
{ ଫେରନ୍ତୁ tbl->sym_get_ମୂଲ୍ୟ(ଚାବି); }

/*
 * This template is a type-safe interface to the symbol table.
 */
template <class T> class ପ୍ରତୀକ_map_s : ବ୍ୟକ୍ତିଗତ ପ୍ରତୀକ_table_s {

    ସର୍ବସାଧାରଣ:
      ଶୂନ୍ୟ sym_set_ମୂଲ୍ୟ(ଅଚଳ ଅକ୍ଷର*ଚାବି, T*val)
      { ପ୍ରତୀକ_ମୂଲ୍ୟ_t tmp;
	tmp.ptr = val;
	ପ୍ରତୀକ_table_s::sym_set_ମୂଲ୍ୟ(ଚାବି, tmp);
      }

      T* sym_get_ମୂଲ୍ୟ(ଅଚଳ ଅକ୍ଷର*ଚାବି)
      { ପ୍ରତୀକ_ମୂଲ୍ୟ_t val = ପ୍ରତୀକ_table_s::sym_get_ମୂଲ୍ୟ(ଚାବି);
	ଫେରନ୍ତୁ reinterpret_cast<T*>(val.ptr);
      }
};

#endif /* IVL_symbols_H */
