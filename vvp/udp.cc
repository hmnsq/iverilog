/*
 * Copyright (c) 2005-2026 Stephen Williams (steve@icarus.com)
 *
 * (This is a rewrite of code that was ...
 * Copyright (c) 2001 Stephan Boettcher <stephan@nevis.columbia.edu>)
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

#അന്തര്ഭാവ "udp.h"
#അന്തര്ഭാവ "schedule.h"
#അന്തര്ഭാവ "symbols.h"
#അന്തര്ഭാവ "compile.h"
#അന്തര്ഭാവ "config.h"
#അന്തര്ഭാവ <cassert>
#അന്തര്ഭാവ <cstdlib>
#അന്തര്ഭാവ <cstring>
#അന്തര്ഭാവ <cstdio>
#അന്തര്ഭാവ <iostream>
#ifdef CHECK_WITH_VALGRIND
#അന്തര്ഭാവ "vvp_cleanup.h"
#അന്തര്ഭാവ "ivl_alloc.h"
#endif

using നാമാകാശ std;

// We may need these later when we build the VPI interface to
// UDP definitions.
#ifdef CHECK_WITH_VALGRIND
സ്ഥിതം vvp_udp_s **udp_defns = 0;
സ്ഥിതം unsigned udp_defns_എണ്ണം = 0;

ശൂന്യം udp_defns_delete()
{
      വേണ്ടി (unsigned idx = 0; idx < udp_defns_എണ്ണം; idx += 1) {
	    എങ്കിൽ (udp_defns[idx]->is_sequential()) {
		  vvp_udp_seq_s *obj = static_cast<vvp_udp_seq_s *>
		                       (udp_defns[idx]);
		  delete obj;
	    } അല്ലെങ്കിൽ {
		  vvp_udp_comb_s *obj = static_cast<vvp_udp_comb_s *>
		                        (udp_defns[idx]);
		  delete obj;
	    }
      }
      സ്വതന്ത്രം(udp_defns);
      udp_defns = 0;
      udp_defns_എണ്ണം = 0;
}
#endif

സ്ഥിതം ചിഹ്നം_table_t udp_table;

ശൂന്യം delete_udp_symbols()
{
      delete_ചിഹ്നം_table(udp_table);
      udp_table = 0;
}

ഘടന vvp_udp_s *udp_find(സ്ഥിരം അക്ഷരം *label)
{
      ചിഹ്നം_മൂല്യം_t v = sym_get_മൂല്യം(udp_table, label);
      തിരികെ (ഘടന vvp_udp_s *)v.ptr;
}

ostream& operator <<(ostream&o, സ്ഥിരം ഘടന udp_levels_table&table)
{
      o << "[" << hex << table.mask0
	<< "/" << hex << table.mask1
	<< "/" << hex << table.maskx << "]";
      തിരികെ o;
}

vvp_udp_s::vvp_udp_s(സ്ഥിരം അക്ഷരം*label, അക്ഷരം*name__, unsigned ports,
                     vvp_bit4_t init, തർക്കം type)
: name_(name__), ports_(ports), init_(init), seq_(type)
{
      എങ്കിൽ (!udp_table)
	    udp_table = new_ചിഹ്നം_table();

      പ്രതിജ്ഞാ( !udp_find(label) );

      ചിഹ്നം_മൂല്യം_t v;
      v.ptr = ഇത്;
      sym_set_മൂല്യം(udp_table, label, v);

#ifdef CHECK_WITH_VALGRIND
      udp_defns_എണ്ണം += 1;
      udp_defns = static_cast<vvp_udp_s **>(realloc(udp_defns,
                  udp_defns_എണ്ണം*വലുപ്പം(vvp_udp_s **)));
      udp_defns[udp_defns_എണ്ണം-1] = ഇത്;
#endif
}

vvp_udp_s::~vvp_udp_s()
{
      delete[] name_;
}

unsigned vvp_udp_s::port_എണ്ണം() സ്ഥിരം
{
      തിരികെ ports_;
}

vvp_bit4_t vvp_udp_s::get_init() സ്ഥിരം
{
      തിരികെ init_;
}

vvp_udp_comb_s::vvp_udp_comb_s(സ്ഥിരം അക്ഷരം*label, അക്ഷരം*name__, unsigned ports)
: vvp_udp_s(label, name__, ports, BIT4_X, അസത്യം)
{
      levels0_ = 0;
      levels1_ = 0;
      nlevels0_ = 0;
      nlevels1_ = 0;
}

vvp_udp_comb_s::~vvp_udp_comb_s()
{
      delete[] levels0_;
      delete[] levels1_;
}

/*
 * The cur table that is passed in must have for every valid bit
 * position exactly one of the three mask bits set. This represents an
 * actual vector of inputs to be tested.
 *
 * The levels0_ and levels1_ tables have levels_table objects that
 * each represent a single row. For the row to match the input vector,
 * all the bits that are set in the cur table must also be set in the
 * row being tested.
 *
 * It is possible for a row to match multiple different vectors. This
 * is seen from the compile_table function, where bit positions for
 * multiple masks can be test for certain row positions. For example,
 * if the row bit position is '?', then mask 0/1/x are all set in the
 * row for that bit position. This means it doesn't matter which of
 * the three bit positions is set in the cur input table, the bit
 * position will generate a match.
 */
vvp_bit4_t vvp_udp_comb_s::test_levels(സ്ഥിരം udp_levels_table&cur)
{
	/* To test for a row match, test that the mask0, mask1 and
	   maskx vectors all have bits set where the matching
	   cur.mask0/1/x vectors have bits set. It is possible that a
	   levels0_[idx] vector has more bits set then the cur mask,
	   but this is OK and these bits are to be ignored. */

      വേണ്ടി (unsigned idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    എങ്കിൽ (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  തുടരുക;
	    എങ്കിൽ (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  തുടരുക;
	    എങ്കിൽ (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  തുടരുക;

	    തിരികെ BIT4_0;
      }

      വേണ്ടി (unsigned idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    എങ്കിൽ (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  തുടരുക;
	    എങ്കിൽ (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  തുടരുക;
	    എങ്കിൽ (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  തുടരുക;

	    തിരികെ BIT4_1;
      }

      തിരികെ BIT4_X;
}

vvp_bit4_t vvp_udp_comb_s::calculate_output(സ്ഥിരം udp_levels_table&cur,
					    സ്ഥിരം udp_levels_table&,
					    vvp_bit4_t)
{
      തിരികെ test_levels(cur);
}

സ്ഥിതം ശൂന്യം or_based_on_char(udp_levels_table&cur, അക്ഷരം flag,
			     unsigned long mask_bit)
{
      തിരഞ്ഞെടുപ്പ് (flag) {
	  അവസ്ഥ '0':
	    cur.mask0 |= mask_bit;
	    നിർത്തുക;
	  അവസ്ഥ '1':
	    cur.mask1 |= mask_bit;
	    നിർത്തുക;
	  അവസ്ഥ 'x':
	    cur.maskx |= mask_bit;
	    നിർത്തുക;
	  അവസ്ഥ 'b':
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    നിർത്തുക;
	  അവസ്ഥ 'l':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    നിർത്തുക;
	  അവസ്ഥ 'h':
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    നിർത്തുക;
	  അവസ്ഥ '?':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    നിർത്തുക;
	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "Unsupported flag %c(%d).\n", flag, flag);
	    പ്രതിജ്ഞാ(0);
      }
}

ശൂന്യം vvp_udp_comb_s::compile_table(അക്ഷരം**tab)
{
      unsigned nrows0 = 0, nrows1 = 0;

	/* First run through the table to figure out the number of
	   rows I need for each kind of table. */
      വേണ്ടി (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    പ്രതിജ്ഞാ(strlen(tab[idx]) == port_എണ്ണം() + 1);
	    തിരഞ്ഞെടുപ്പ് (tab[idx][port_എണ്ണം()]) {
		അവസ്ഥ '0':
		  nrows0 += 1;
		  നിർത്തുക;
		അവസ്ഥ '1':
		  nrows1 += 1;
		  നിർത്തുക;
		അവസ്ഥ 'x':
		  നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		  fprintf(stderr, "Unsupported entry %c(%d).\n",
		          tab[idx][port_എണ്ണം()], tab[idx][port_എണ്ണം()]);
		  പ്രതിജ്ഞാ(0);
	    }
      }

      nlevels0_ = nrows0;
      levels0_ = പുതിയ udp_levels_table[nlevels0_];

      nlevels1_ = nrows1;
      levels1_ = പുതിയ udp_levels_table[nlevels1_];

      nrows0 = 0;
      nrows1 = 0;
      വേണ്ടി (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    ഘടന udp_levels_table cur;
	    cur.mask0 = 0;
	    cur.mask1 = 0;
	    cur.maskx = 0;
	    എങ്കിൽ (port_എണ്ണം() > 8*വലുപ്പം(cur.mask0)) {
		  fprintf(stderr, "internal error: primitive port count=%u "
			  " > %zu\n", port_എണ്ണം(), വലുപ്പം(cur.mask0));
		  പ്രതിജ്ഞാ(port_എണ്ണം() <= 8*വലുപ്പം(cur.mask0));
	    }
	    വേണ്ടി (unsigned pp = 0 ;  pp < port_എണ്ണം() ;  pp += 1) {
		  unsigned long mask_bit = 1UL << pp;
		  or_based_on_char(cur, tab[idx][pp], mask_bit);
	    }

	    തിരഞ്ഞെടുപ്പ് (tab[idx][port_എണ്ണം()]) {
		അവസ്ഥ '0':
		  levels0_[nrows0++] = cur;
		  നിർത്തുക;
		അവസ്ഥ '1':
		  levels1_[nrows1++] = cur;
		  നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		  നിർത്തുക;
	    }
	    delete[] tab[idx];
      }
      സ്വതന്ത്രം(tab);

      പ്രതിജ്ഞാ(nrows0 == nlevels0_);
      പ്രതിജ്ഞാ(nrows1 == nlevels1_);
}

vvp_udp_seq_s::vvp_udp_seq_s(സ്ഥിരം അക്ഷരം*label, അക്ഷരം*name__,
			     unsigned ports, vvp_bit4_t init)
: vvp_udp_s(label, name__, ports, init, സത്യം)
{
      levels0_ = 0;
      levels1_ = 0;
      levelsx_ = 0;
      levelsL_ = 0;

      nlevels0_ = 0;
      nlevels1_ = 0;
      nlevelsx_ = 0;
      nlevelsL_ = 0;

      edges0_ = 0;
      edges1_ = 0;
      edgesL_ = 0;

      nedges0_ = 0;
      nedges1_ = 0;
      nedgesL_ = 0;
}

vvp_udp_seq_s::~vvp_udp_seq_s()
{
      delete[] levels0_;
      delete[] levels1_;
      delete[] levelsx_;
      delete[] levelsL_;
      delete[] edges0_;
      delete[] edges1_;
      delete[] edgesL_;
}

ശൂന്യം edge_based_on_char(ഘടന udp_edges_table&cur, അക്ഷരം chr, unsigned pos)
{
      unsigned long mask_bit = 1UL << pos;

      തിരഞ്ഞെടുപ്പ് (chr) {
	  അവസ്ഥ '0':
	    cur.mask0 |= mask_bit;
	    നിർത്തുക;
	  അവസ്ഥ '1':
	    cur.mask1 |= mask_bit;
	    നിർത്തുക;
	  അവസ്ഥ 'x':
	    cur.maskx |= mask_bit;
	    നിർത്തുക;
	  അവസ്ഥ 'b':
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    നിർത്തുക;
	  അവസ്ഥ 'l':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    നിർത്തുക;
	  അവസ്ഥ 'h':
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    നിർത്തുക;
	  അവസ്ഥ '?':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    നിർത്തുക;

	  അവസ്ഥ 'B': // (x?) edge
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    നിർത്തുക;
	  അവസ്ഥ 'f': // (10) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    നിർത്തുക;
	  അവസ്ഥ 'F': // (x0) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    നിർത്തുക;
	  അവസ്ഥ 'M': // (1x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    നിർത്തുക;
	  അവസ്ഥ 'N': // (1x) and (10) edge
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    നിർത്തുക;
	  അവസ്ഥ 'P': // (0x) and (01) edge
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    നിർത്തുക;
	  അവസ്ഥ 'q': // (bx) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    നിർത്തുക;
	  അവസ്ഥ 'Q': // (0x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    നിർത്തുക;
	  അവസ്ഥ 'r': // (01) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    നിർത്തുക;
	  അവസ്ഥ 'R': // (x1) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    നിർത്തുക;
	  അവസ്ഥ '%': // (?x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    നിർത്തുക;
	  അവസ്ഥ '+': // (?1) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    നിർത്തുക;
	  അവസ്ഥ '_': // (?0) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 1;
	    നിർത്തുക;
	  സ്ഥിരസ്ഥിതി:
	    fprintf(stderr, "internal error: unknown edge code: %c\n", chr);
	    പ്രതിജ്ഞാ(0);
      }
}

ശൂന്യം vvp_udp_seq_s::compile_table(അക്ഷരം**tab)
{

      വേണ്ടി (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    സ്ഥിരം അക്ഷരം*row = tab[idx];
	    പ്രതിജ്ഞാ(strlen(row) == port_എണ്ണം() + 2);

	    എങ്കിൽ (strspn(row, "01xblh?") >= port_എണ്ണം()+1) {

		  തിരഞ്ഞെടുപ്പ് (row[port_എണ്ണം()+1]) {
		      അവസ്ഥ '0':
			nlevels0_ += 1;
			നിർത്തുക;
		      അവസ്ഥ '1':
			nlevels1_ += 1;
			നിർത്തുക;
		      അവസ്ഥ 'x':
			nlevelsx_ += 1;
			നിർത്തുക;
		      അവസ്ഥ '-':
			nlevelsL_ += 1;
			നിർത്തുക;
		      സ്ഥിരസ്ഥിതി:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_എണ്ണം()+1], row[port_എണ്ണം()+1]);
			പ്രതിജ്ഞാ(0);
			നിർത്തുക;
		  }

	    } അല്ലെങ്കിൽ {
		    /* Rows that have n or p edges will need to be
		       expanded into 2 rows. */
		  unsigned extra = 0;
		  എങ്കിൽ (strchr(row,'n'))
			extra = 1;
		  എങ്കിൽ (strchr(row,'p'))
			extra = 1;
		  എങ്കിൽ (strchr(row,'*'))
			extra = 2;

		  തിരഞ്ഞെടുപ്പ് (row[port_എണ്ണം()+1]) {
		      അവസ്ഥ '0':
			nedges0_ += 1 + extra;
			നിർത്തുക;
		      അവസ്ഥ '1':
			nedges1_ += 1 + extra;
			നിർത്തുക;
		      അവസ്ഥ 'x':
			നിർത്തുക;
		      അവസ്ഥ '-':
			nedgesL_ += 1 + extra;
			നിർത്തുക;
		      സ്ഥിരസ്ഥിതി:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_എണ്ണം()+1], row[port_എണ്ണം()+1]);
			പ്രതിജ്ഞാ(0);
			നിർത്തുക;
		  }
	    }
      }

      levels0_ = പുതിയ udp_levels_table[nlevels0_];
      levels1_ = പുതിയ udp_levels_table[nlevels1_];
      levelsx_ = പുതിയ udp_levels_table[nlevelsx_];
      levelsL_ = പുതിയ udp_levels_table[nlevelsL_];
      edges0_ = പുതിയ udp_edges_table[nedges0_];
      edges1_ = പുതിയ udp_edges_table[nedges1_];
      edgesL_ = പുതിയ udp_edges_table[nedgesL_];

      unsigned idx_lev0 = 0;
      unsigned idx_lev1 = 0;
      unsigned idx_levx = 0;
      unsigned idx_levL = 0;
      unsigned idx_edg0 = 0;
      unsigned idx_edg1 = 0;
      unsigned idx_edgL = 0;

      വേണ്ടി (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    സ്ഥിരം അക്ഷരം*row = tab[idx];

	    എങ്കിൽ (strspn(row, "01xblh?") >= port_എണ്ണം()+1) {
		  ഘടന udp_levels_table cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  വേണ്ടി (unsigned pp = 0 ;  pp < port_എണ്ണം() ;  pp += 1) {
			unsigned long mask_bit = 1UL << pp;
			or_based_on_char(cur, row[pp+1], mask_bit);
		  }

		  or_based_on_char(cur, row[0], 1UL << port_എണ്ണം());

		  തിരഞ്ഞെടുപ്പ് (row[port_എണ്ണം()+1]) {
		      അവസ്ഥ '0':
			levels0_[idx_lev0++] = cur;
			നിർത്തുക;
		      അവസ്ഥ '1':
			levels1_[idx_lev1++] = cur;
			നിർത്തുക;
		      അവസ്ഥ 'x':
			levelsx_[idx_levx++] = cur;
			നിർത്തുക;
		      അവസ്ഥ '-':
			levelsL_[idx_levL++] = cur;
			നിർത്തുക;
		      സ്ഥിരസ്ഥിതി:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_എണ്ണം()+1], row[port_എണ്ണം()+1]);
			പ്രതിജ്ഞാ(0);
			നിർത്തുക;
		  }

	    } അല്ലെങ്കിൽ {
		  ഘടന udp_edges_table cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  cur.edge_position = 0;
		  cur.edge_mask0 = 0;
		  cur.edge_mask1 = 0;
		  cur.edge_maskx = 0;

		  തർക്കം need_ext0_table = അസത്യം;
		  ഘടന udp_edges_table ext0;
		  ext0.mask0 = 0;
		  ext0.mask1 = 0;
		  ext0.maskx = 0;
		  ext0.edge_position = 0;
		  ext0.edge_mask0 = 0;
		  ext0.edge_mask1 = 0;
		  ext0.edge_maskx = 0;

		  തർക്കം need_ext1_table = അസത്യം;
		  ഘടന udp_edges_table ext1;
		  ext1.mask0 = 0;
		  ext1.mask1 = 0;
		  ext1.maskx = 0;
		  ext1.edge_position = 0;
		  ext1.edge_mask0 = 0;
		  ext1.edge_mask1 = 0;
		  ext1.edge_maskx = 0;

		  വേണ്ടി (unsigned pp = 0 ;  pp < port_എണ്ണം() ; pp += 1) {
			തിരഞ്ഞെടുപ്പ് (row[pp+1]) {
			    അവസ്ഥ 'n':
			      edge_based_on_char(cur, 'N', pp);
			      edge_based_on_char(ext0, '_', pp);
			      need_ext0_table = സത്യം;
			      നിർത്തുക;
			    അവസ്ഥ 'p':
			      edge_based_on_char(cur, 'P', pp);
			      edge_based_on_char(ext0, '+', pp);
			      need_ext0_table = സത്യം;
			      നിർത്തുക;
			    അവസ്ഥ '*':
			      edge_based_on_char(cur, 'P', pp);
			      edge_based_on_char(ext0, 'N', pp);
			      edge_based_on_char(ext1, 'B', pp);
			      need_ext0_table = സത്യം;
			      need_ext1_table = സത്യം;
			      നിർത്തുക;
			    സ്ഥിരസ്ഥിതി:
			      edge_based_on_char(cur, row[pp+1], pp);
			      edge_based_on_char(ext0, row[pp+1], pp);
			      edge_based_on_char(ext1, row[pp+1], pp);
			      നിർത്തുക;
			}
		  }
		  edge_based_on_char(cur, row[0], port_എണ്ണം());
		  edge_based_on_char(ext0, row[0], port_എണ്ണം());
		  edge_based_on_char(ext1, row[0], port_എണ്ണം());

		  തിരഞ്ഞെടുപ്പ് (row[port_എണ്ണം()+1]) {
		      അവസ്ഥ '0':
			edges0_[idx_edg0++] = cur;
			എങ്കിൽ (need_ext0_table)
			      edges0_[idx_edg0++] = ext0;
			എങ്കിൽ (need_ext1_table)
			      edges0_[idx_edg0++] = ext1;
			നിർത്തുക;
		      അവസ്ഥ '1':
			edges1_[idx_edg1++] = cur;
			എങ്കിൽ (need_ext0_table)
			      edges1_[idx_edg1++] = ext0;
			എങ്കിൽ (need_ext1_table)
			      edges1_[idx_edg1++] = ext1;
			നിർത്തുക;
		      അവസ്ഥ 'x':
			നിർത്തുക;
		      അവസ്ഥ '-':
			edgesL_[idx_edgL++] = cur;
			എങ്കിൽ (need_ext0_table)
			      edgesL_[idx_edgL++] = ext0;
			എങ്കിൽ (need_ext1_table)
			      edgesL_[idx_edgL++] = ext1;
			നിർത്തുക;
		      സ്ഥിരസ്ഥിതി:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_എണ്ണം()+1], row[port_എണ്ണം()+1]);
			പ്രതിജ്ഞാ(0);
			നിർത്തുക;
		  }

	    }
	    delete[] tab[idx];
      }
      സ്വതന്ത്രം(tab);

      പ്രതിജ്ഞാ(idx_edg0 == nedges0_);
      പ്രതിജ്ഞാ(idx_edg1 == nedges1_);
      പ്രതിജ്ഞാ(idx_edgL == nedgesL_);

}

തർക്കം operator == (സ്ഥിരം udp_levels_table&a, സ്ഥിരം udp_levels_table&b)
{
      എങ്കിൽ (a.mask0 != b.mask0)
	    തിരികെ അസത്യം;
      എങ്കിൽ (a.mask1 != b.mask1)
	    തിരികെ അസത്യം;
      എങ്കിൽ (a.maskx != b.maskx)
	    തിരികെ അസത്യം;
      തിരികെ സത്യം;
}

vvp_bit4_t vvp_udp_seq_s::calculate_output(സ്ഥിരം udp_levels_table&cur,
					   സ്ഥിരം udp_levels_table&prev,
					   vvp_bit4_t cur_out)
{
      എങ്കിൽ (cur == prev)
	    തിരികെ cur_out;

      udp_levels_table cur_tmp = cur;

      unsigned long mask_out = 1UL << port_എണ്ണം();
      തിരഞ്ഞെടുപ്പ് (cur_out) {
	  അവസ്ഥ BIT4_0:
	    cur_tmp.mask0 |= mask_out;
	    നിർത്തുക;
	  അവസ്ഥ BIT4_1:
	    cur_tmp.mask1 |= mask_out;
	    നിർത്തുക;
	  സ്ഥിരസ്ഥിതി:
	    cur_tmp.maskx |= mask_out;
	    നിർത്തുക;
      }

      vvp_bit4_t lev = test_levels_(cur_tmp);
      എങ്കിൽ (lev == BIT4_Z) {
	    lev = test_edges_(cur_tmp, prev);
      }

      തിരികെ lev;
}

/*
 * This function tests the levels of the input with the additional
 * check match for the current output. It uses this to calculate a
 * next output, or Z if there was no match. (This is different from
 * the combinational version of this function, which returns X for the
 * cases that don't match.) This method assumes that the caller has
 * set the mask bit in bit position [port_count()] to reflect the
 * current output.
 */
vvp_bit4_t vvp_udp_seq_s::test_levels_(സ്ഥിരം udp_levels_table&cur)
{
      വേണ്ടി (unsigned idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    എങ്കിൽ (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  തുടരുക;
	    എങ്കിൽ (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  തുടരുക;
	    എങ്കിൽ (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  തുടരുക;

	    തിരികെ BIT4_0;
      }

      വേണ്ടി (unsigned idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    എങ്കിൽ (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  തുടരുക;
	    എങ്കിൽ (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  തുടരുക;
	    എങ്കിൽ (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  തുടരുക;

	    തിരികെ BIT4_1;
      }

	/* We need to test against an explicit X-output table, since
	   we need to distinguish from an X output and no match. */
      വേണ്ടി (unsigned idx = 0 ;  idx < nlevelsx_ ;  idx += 1) {
	    എങ്കിൽ (cur.mask0 != (cur.mask0 & levelsx_[idx].mask0))
		  തുടരുക;
	    എങ്കിൽ (cur.mask1 != (cur.mask1 & levelsx_[idx].mask1))
		  തുടരുക;
	    എങ്കിൽ (cur.maskx != (cur.maskx & levelsx_[idx].maskx))
		  തുടരുക;

	    തിരികെ BIT4_X;
      }

	/* Test the table that requests the next output be the same as
	   the current output. This gets the current output from the
	   levels table that was passed in. */
      വേണ്ടി (unsigned idx = 0 ;  idx < nlevelsL_ ;  idx += 1) {
	    എങ്കിൽ (cur.mask0 != (cur.mask0 & levelsL_[idx].mask0))
		  തുടരുക;
	    എങ്കിൽ (cur.mask1 != (cur.mask1 & levelsL_[idx].mask1))
		  തുടരുക;
	    എങ്കിൽ (cur.maskx != (cur.maskx & levelsL_[idx].maskx))
		  തുടരുക;

	    എങ്കിൽ (cur.mask0 & (1 << port_എണ്ണം()))
		  തിരികെ BIT4_0;
	    എങ്കിൽ (cur.mask1 & (1 << port_എണ്ണം()))
		  തിരികെ BIT4_1;
	    എങ്കിൽ (cur.maskx & (1 << port_എണ്ണം()))
		  തിരികെ BIT4_X;

	    പ്രതിജ്ഞാ(0);
	    തിരികെ BIT4_X;
      }

	/* No explicit levels entry match. Return a Z to signal that
	   further testing is needed. */
      തിരികെ BIT4_Z;
}

vvp_bit4_t vvp_udp_seq_s::test_edges_(സ്ഥിരം udp_levels_table&cur,
				      സ്ഥിരം udp_levels_table&prev)
{
	/* The edge_mask is true for all bits that are different in
	   the cur and prev tables. */
      unsigned long edge0_mask = cur.mask0 ^ prev.mask0;
      unsigned long edgex_mask = cur.maskx ^ prev.maskx;
      unsigned long edge1_mask = cur.mask1 ^ prev.mask1;

      unsigned long edge_mask = edge0_mask|edgex_mask|edge1_mask;
      edge_mask &= ~ (-1UL << port_എണ്ണം());

	/* If there are no differences, then there are no edges. Give
	   up now. */
      എങ്കിൽ (edge_mask == 0)
	    തിരികെ BIT4_X;

      unsigned edge_position = 0;
      ആകുമ്പോൾ ((edge_mask&1) == 0) {
	    edge_mask >>= 1;
	    edge_position += 1;
      }

	/* We expect that there is exactly one edge in here. */
      പ്രതിജ്ഞാ(edge_mask == 1);

      edge_mask = 1UL << edge_position;

      unsigned edge_mask0 = (prev.mask0&edge_mask)? 1 : 0;
      unsigned edge_maskx = (prev.maskx&edge_mask)? 1 : 0;
      unsigned edge_mask1 = (prev.mask1&edge_mask)? 1 : 0;


	/* Now the edge_position and edge_mask* variables have the
	   values we use to test the applicability of the edge_table
	   entries. */

      വേണ്ടി (unsigned idx = 0 ;  idx < nedges0_ ;  idx += 1) {
	    സ്ഥിരം ഘടന udp_edges_table*row = edges0_ + idx;

	    എങ്കിൽ (row->edge_position != edge_position)
		  തുടരുക;
	    എങ്കിൽ (edge_mask0 && !row->edge_mask0)
		  തുടരുക;
	    എങ്കിൽ (edge_maskx && !row->edge_maskx)
		  തുടരുക;
	    എങ്കിൽ (edge_mask1 && !row->edge_mask1)
		  തുടരുക;
	    എങ്കിൽ (cur.mask0 != (cur.mask0 & row->mask0))
		  തുടരുക;
	    എങ്കിൽ (cur.maskx != (cur.maskx & row->maskx))
		  തുടരുക;
	    എങ്കിൽ (cur.mask1 != (cur.mask1 & row->mask1))
		  തുടരുക;

	    തിരികെ BIT4_0;
      }

      വേണ്ടി (unsigned idx = 0 ;  idx < nedges1_ ;  idx += 1) {
	    സ്ഥിരം ഘടന udp_edges_table*row = edges1_ + idx;

	    എങ്കിൽ (row->edge_position != edge_position)
		  തുടരുക;
	    എങ്കിൽ (edge_mask0 && !row->edge_mask0)
		  തുടരുക;
	    എങ്കിൽ (edge_maskx && !row->edge_maskx)
		  തുടരുക;
	    എങ്കിൽ (edge_mask1 && !row->edge_mask1)
		  തുടരുക;
	    എങ്കിൽ (cur.mask0 != (cur.mask0 & row->mask0))
		  തുടരുക;
	    എങ്കിൽ (cur.maskx != (cur.maskx & row->maskx))
		  തുടരുക;
	    എങ്കിൽ (cur.mask1 != (cur.mask1 & row->mask1))
		  തുടരുക;

	    തിരികെ BIT4_1;
      }

      വേണ്ടി (unsigned idx = 0 ;  idx < nedgesL_ ;  idx += 1) {
	    സ്ഥിരം ഘടന udp_edges_table*row = edgesL_ + idx;

	    എങ്കിൽ (row->edge_position != edge_position)
		  തുടരുക;
	    എങ്കിൽ (edge_mask0 && !row->edge_mask0)
		  തുടരുക;
	    എങ്കിൽ (edge_maskx && !row->edge_maskx)
		  തുടരുക;
	    എങ്കിൽ (edge_mask1 && !row->edge_mask1)
		  തുടരുക;
	    എങ്കിൽ (cur.mask0 != (cur.mask0 & row->mask0))
		  തുടരുക;
	    എങ്കിൽ (cur.maskx != (cur.maskx & row->maskx))
		  തുടരുക;
	    എങ്കിൽ (cur.mask1 != (cur.mask1 & row->mask1))
		  തുടരുക;

	    എങ്കിൽ (cur.mask0 & (1 << port_എണ്ണം()))
		  തിരികെ BIT4_0;
	    എങ്കിൽ (cur.mask1 & (1 << port_എണ്ണം()))
		  തിരികെ BIT4_1;
	    എങ്കിൽ (cur.maskx & (1 << port_എണ്ണം()))
		  തിരികെ BIT4_X;

	    പ്രതിജ്ഞാ(0);
	    തിരികെ BIT4_X;
      }

      തിരികെ BIT4_X;
}

vvp_udp_fun_core::vvp_udp_fun_core(vvp_net_t*net, vvp_udp_s*def)
: vvp_wide_fun_core(net, def->port_എണ്ണം())
{
      def_ = def;
      cur_out_ = def_->get_init();
	// Assume initially that all the inputs are 1'bx
      current_.mask0 = 0;
      current_.mask1 = 0;
      current_.maskx = ~ ((-1UL) << port_എണ്ണം());

        // If the initial value is 0 or 1, schedule the initial assignment
        // normally, so that any sensitive always processes can be started
        // first.
      എങ്കിൽ (cur_out_ != BIT4_X)
	    schedule_generic(ഇത്, 0, അസത്യം);
      അല്ലെങ്കിൽ
	    schedule_functor(ഇത്);
}

vvp_udp_fun_core::~vvp_udp_fun_core()
{
}

/*
 * This method is used to propagate the initial value on startup.
 */
ശൂന്യം vvp_udp_fun_core::run_run()
{
      vvp_vector4_t tmp (1);
      tmp.set_bit(0, cur_out_);
      propagate_vec4(tmp);
}

ശൂന്യം vvp_udp_fun_core::recv_vec4_from_inputs(unsigned port)
{
	/* For now, assume udps are 1-bit wide. */
      unsigned port_width = value(port).size();
      പ്രതിജ്ഞാ(port_width == 1);

      unsigned long mask = 1UL << port;

      udp_levels_table prev = current_;

      തിരഞ്ഞെടുപ്പ് (value(port).value(0)) {

	  അവസ്ഥ BIT4_0:
	    current_.mask0 |= mask;
	    current_.mask1 &= ~mask;
	    current_.maskx &= ~mask;
	    നിർത്തുക;
	  അവസ്ഥ BIT4_1:
	    current_.mask0 &= ~mask;
	    current_.mask1 |= mask;
	    current_.maskx &= ~mask;
	    നിർത്തുക;
	  സ്ഥിരസ്ഥിതി:
	    current_.mask0 &= ~mask;
	    current_.mask1 &= ~mask;
	    current_.maskx |= mask;
	    നിർത്തുക;
      }

      vvp_bit4_t out_bit = def_->calculate_output(current_, prev, cur_out_);

      എങ്കിൽ (out_bit == cur_out_)
	    തിരികെ;

      cur_out_ = out_bit;
      schedule_functor(ഇത്);
}


/*
 * This function is called by the parser in response to a .udp
 * node. We create the nodes needed to integrate the UDP into the
 * netlist. The definition should be parsed already.
 */
ശൂന്യം compile_udp_functor(അക്ഷരം*label, അക്ഷരം*type,
			 unsigned argc, ഘടന symb_s*argv)
{
      ഘടന vvp_udp_s *def = udp_find(type);
      പ്രതിജ്ഞാ(def);
      സ്വതന്ത്രം(type);

      vvp_net_t*ptr = പുതിയ vvp_net_t;
      vvp_udp_fun_core*core = പുതിയ vvp_udp_fun_core(ptr, def);
      ptr->fun = core;

      define_functor_ചിഹ്നം(label, ptr);
      സ്വതന്ത്രം(label);

      wide_inputs_connect(core, argc, argv);
      സ്വതന്ത്രം(argv);
}
