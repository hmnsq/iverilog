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

#ಅನ್ತರ್ಭಾವ "udp.h"
#ಅನ್ತರ್ಭಾವ "schedule.h"
#ಅನ್ತರ್ಭಾವ "symbols.h"
#ಅನ್ತರ್ಭಾವ "compile.h"
#ಅನ್ತರ್ಭಾವ "config.h"
#ಅನ್ತರ್ಭಾವ <cassert>
#ಅನ್ತರ್ಭಾವ <cstdlib>
#ಅನ್ತರ್ಭಾವ <cstring>
#ಅನ್ತರ್ಭಾವ <cstdio>
#ಅನ್ತರ್ಭಾವ <iostream>
#ifdef CHECK_WITH_VALGRIND
#ಅನ್ತರ್ಭಾವ "vvp_cleanup.h"
#ಅನ್ತರ್ಭಾವ "ivl_alloc.h"
#endif

using ನಾಮಾಕಾಶ std;

// We may need these later when we build the VPI interface to
// UDP definitions.
#ifdef CHECK_WITH_VALGRIND
ಸ್ಥಿರ vvp_udp_s **udp_defns = 0;
ಸ್ಥಿರ unsigned udp_defns_ಎಣಿಕೆ = 0;

ಶೂನ್ಯ udp_defns_delete()
{
      ಪ್ರತಿ (unsigned idx = 0; idx < udp_defns_ಎಣಿಕೆ; idx += 1) {
	    ಯದಿ (udp_defns[idx]->is_sequential()) {
		  vvp_udp_seq_s *obj = static_cast<vvp_udp_seq_s *>
		                       (udp_defns[idx]);
		  delete obj;
	    } ಅನ್ಯಥಾ {
		  vvp_udp_comb_s *obj = static_cast<vvp_udp_comb_s *>
		                        (udp_defns[idx]);
		  delete obj;
	    }
      }
      ಮುಕ್ತ(udp_defns);
      udp_defns = 0;
      udp_defns_ಎಣಿಕೆ = 0;
}
#endif

ಸ್ಥಿರ ಚಿಹ್ನೆ_table_t udp_table;

ಶೂನ್ಯ delete_udp_symbols()
{
      delete_ಚಿಹ್ನೆ_table(udp_table);
      udp_table = 0;
}

ಸಂರಚನಾ vvp_udp_s *udp_find(ಧ್ರುವ ವರ್ಣ *label)
{
      ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t v = sym_get_ಮೌಲ್ಯ(udp_table, label);
      ಪ್ರತ್ಯಾಗಮ (ಸಂರಚನಾ vvp_udp_s *)v.ptr;
}

ostream& operator <<(ostream&o, ಧ್ರುವ ಸಂರಚನಾ udp_levels_table&table)
{
      o << "[" << hex << table.mask0
	<< "/" << hex << table.mask1
	<< "/" << hex << table.maskx << "]";
      ಪ್ರತ್ಯಾಗಮ o;
}

vvp_udp_s::vvp_udp_s(ಧ್ರುವ ವರ್ಣ*label, ವರ್ಣ*name__, unsigned ports,
                     vvp_bit4_t init, ತಾರ್ಕಿಕ type)
: name_(name__), ports_(ports), init_(init), seq_(type)
{
      ಯದಿ (!udp_table)
	    udp_table = new_ಚಿಹ್ನೆ_table();

      ಪ್ರತಿಜ್ಞಾ( !udp_find(label) );

      ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t v;
      v.ptr = ಏತತ್;
      sym_set_ಮೌಲ್ಯ(udp_table, label, v);

#ifdef CHECK_WITH_VALGRIND
      udp_defns_ಎಣಿಕೆ += 1;
      udp_defns = static_cast<vvp_udp_s **>(realloc(udp_defns,
                  udp_defns_ಎಣಿಕೆ*ಪರಿಮಾಣ(vvp_udp_s **)));
      udp_defns[udp_defns_ಎಣಿಕೆ-1] = ಏತತ್;
#endif
}

vvp_udp_s::~vvp_udp_s()
{
      delete[] name_;
}

unsigned vvp_udp_s::port_ಎಣಿಕೆ() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ ports_;
}

vvp_bit4_t vvp_udp_s::get_init() ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ init_;
}

vvp_udp_comb_s::vvp_udp_comb_s(ಧ್ರುವ ವರ್ಣ*label, ವರ್ಣ*name__, unsigned ports)
: vvp_udp_s(label, name__, ports, BIT4_X, ಅಸತ್ಯ)
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
vvp_bit4_t vvp_udp_comb_s::test_levels(ಧ್ರುವ udp_levels_table&cur)
{
	/* To test for a row match, test that the mask0, mask1 and
	   maskx vectors all have bits set where the matching
	   cur.mask0/1/x vectors have bits set. It is possible that a
	   levels0_[idx] vector has more bits set then the cur mask,
	   but this is OK and these bits are to be ignored. */

      ಪ್ರತಿ (unsigned idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    ಯದಿ (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  ಅನುವರ್ತ;
	    ಯದಿ (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  ಅನುವರ್ತ;
	    ಯದಿ (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  ಅನುವರ್ತ;

	    ಪ್ರತ್ಯಾಗಮ BIT4_0;
      }

      ಪ್ರತಿ (unsigned idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    ಯದಿ (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  ಅನುವರ್ತ;
	    ಯದಿ (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  ಅನುವರ್ತ;
	    ಯದಿ (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  ಅನುವರ್ತ;

	    ಪ್ರತ್ಯಾಗಮ BIT4_1;
      }

      ಪ್ರತ್ಯಾಗಮ BIT4_X;
}

vvp_bit4_t vvp_udp_comb_s::calculate_output(ಧ್ರುವ udp_levels_table&cur,
					    ಧ್ರುವ udp_levels_table&,
					    vvp_bit4_t)
{
      ಪ್ರತ್ಯಾಗಮ test_levels(cur);
}

ಸ್ಥಿರ ಶೂನ್ಯ or_based_on_char(udp_levels_table&cur, ವರ್ಣ flag,
			     unsigned long mask_bit)
{
      ವಿಕಲ್ಪ (flag) {
	  ಪಕ್ಷ '0':
	    cur.mask0 |= mask_bit;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ '1':
	    cur.mask1 |= mask_bit;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'x':
	    cur.maskx |= mask_bit;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'b':
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'l':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'h':
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ '?':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    fprintf(stderr, "Unsupported flag %c(%d).\n", flag, flag);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }
}

ಶೂನ್ಯ vvp_udp_comb_s::compile_table(ವರ್ಣ**tab)
{
      unsigned nrows0 = 0, nrows1 = 0;

	/* First run through the table to figure out the number of
	   rows I need for each kind of table. */
      ಪ್ರತಿ (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    ಪ್ರತಿಜ್ಞಾ(strlen(tab[idx]) == port_ಎಣಿಕೆ() + 1);
	    ವಿಕಲ್ಪ (tab[idx][port_ಎಣಿಕೆ()]) {
		ಪಕ್ಷ '0':
		  nrows0 += 1;
		  ಭಙ್ಗ;
		ಪಕ್ಷ '1':
		  nrows1 += 1;
		  ಭಙ್ಗ;
		ಪಕ್ಷ 'x':
		  ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		  fprintf(stderr, "Unsupported entry %c(%d).\n",
		          tab[idx][port_ಎಣಿಕೆ()], tab[idx][port_ಎಣಿಕೆ()]);
		  ಪ್ರತಿಜ್ಞಾ(0);
	    }
      }

      nlevels0_ = nrows0;
      levels0_ = ನೂತನ udp_levels_table[nlevels0_];

      nlevels1_ = nrows1;
      levels1_ = ನೂತನ udp_levels_table[nlevels1_];

      nrows0 = 0;
      nrows1 = 0;
      ಪ್ರತಿ (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    ಸಂರಚನಾ udp_levels_table cur;
	    cur.mask0 = 0;
	    cur.mask1 = 0;
	    cur.maskx = 0;
	    ಯದಿ (port_ಎಣಿಕೆ() > 8*ಪರಿಮಾಣ(cur.mask0)) {
		  fprintf(stderr, "internal error: primitive port count=%u "
			  " > %zu\n", port_ಎಣಿಕೆ(), ಪರಿಮಾಣ(cur.mask0));
		  ಪ್ರತಿಜ್ಞಾ(port_ಎಣಿಕೆ() <= 8*ಪರಿಮಾಣ(cur.mask0));
	    }
	    ಪ್ರತಿ (unsigned pp = 0 ;  pp < port_ಎಣಿಕೆ() ;  pp += 1) {
		  unsigned long mask_bit = 1UL << pp;
		  or_based_on_char(cur, tab[idx][pp], mask_bit);
	    }

	    ವಿಕಲ್ಪ (tab[idx][port_ಎಣಿಕೆ()]) {
		ಪಕ್ಷ '0':
		  levels0_[nrows0++] = cur;
		  ಭಙ್ಗ;
		ಪಕ್ಷ '1':
		  levels1_[nrows1++] = cur;
		  ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		  ಭಙ್ಗ;
	    }
	    delete[] tab[idx];
      }
      ಮುಕ್ತ(tab);

      ಪ್ರತಿಜ್ಞಾ(nrows0 == nlevels0_);
      ಪ್ರತಿಜ್ಞಾ(nrows1 == nlevels1_);
}

vvp_udp_seq_s::vvp_udp_seq_s(ಧ್ರುವ ವರ್ಣ*label, ವರ್ಣ*name__,
			     unsigned ports, vvp_bit4_t init)
: vvp_udp_s(label, name__, ports, init, ಸತ್ಯ)
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

ಶೂನ್ಯ edge_based_on_char(ಸಂರಚನಾ udp_edges_table&cur, ವರ್ಣ chr, unsigned pos)
{
      unsigned long mask_bit = 1UL << pos;

      ವಿಕಲ್ಪ (chr) {
	  ಪಕ್ಷ '0':
	    cur.mask0 |= mask_bit;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ '1':
	    cur.mask1 |= mask_bit;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'x':
	    cur.maskx |= mask_bit;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'b':
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'l':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'h':
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ '?':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    ಭಙ್ಗ;

	  ಪಕ್ಷ 'B': // (x?) edge
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'f': // (10) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'F': // (x0) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'M': // (1x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'N': // (1x) and (10) edge
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'P': // (0x) and (01) edge
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'q': // (bx) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'Q': // (0x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'r': // (01) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ 'R': // (x1) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ '%': // (?x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ '+': // (?1) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ '_': // (?0) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 1;
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    fprintf(stderr, "internal error: unknown edge code: %c\n", chr);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }
}

ಶೂನ್ಯ vvp_udp_seq_s::compile_table(ವರ್ಣ**tab)
{

      ಪ್ರತಿ (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    ಧ್ರುವ ವರ್ಣ*row = tab[idx];
	    ಪ್ರತಿಜ್ಞಾ(strlen(row) == port_ಎಣಿಕೆ() + 2);

	    ಯದಿ (strspn(row, "01xblh?") >= port_ಎಣಿಕೆ()+1) {

		  ವಿಕಲ್ಪ (row[port_ಎಣಿಕೆ()+1]) {
		      ಪಕ್ಷ '0':
			nlevels0_ += 1;
			ಭಙ್ಗ;
		      ಪಕ್ಷ '1':
			nlevels1_ += 1;
			ಭಙ್ಗ;
		      ಪಕ್ಷ 'x':
			nlevelsx_ += 1;
			ಭಙ್ಗ;
		      ಪಕ್ಷ '-':
			nlevelsL_ += 1;
			ಭಙ್ಗ;
		      ಸ್ವಭಾವ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_ಎಣಿಕೆ()+1], row[port_ಎಣಿಕೆ()+1]);
			ಪ್ರತಿಜ್ಞಾ(0);
			ಭಙ್ಗ;
		  }

	    } ಅನ್ಯಥಾ {
		    /* Rows that have n or p edges will need to be
		       expanded into 2 rows. */
		  unsigned extra = 0;
		  ಯದಿ (strchr(row,'n'))
			extra = 1;
		  ಯದಿ (strchr(row,'p'))
			extra = 1;
		  ಯದಿ (strchr(row,'*'))
			extra = 2;

		  ವಿಕಲ್ಪ (row[port_ಎಣಿಕೆ()+1]) {
		      ಪಕ್ಷ '0':
			nedges0_ += 1 + extra;
			ಭಙ್ಗ;
		      ಪಕ್ಷ '1':
			nedges1_ += 1 + extra;
			ಭಙ್ಗ;
		      ಪಕ್ಷ 'x':
			ಭಙ್ಗ;
		      ಪಕ್ಷ '-':
			nedgesL_ += 1 + extra;
			ಭಙ್ಗ;
		      ಸ್ವಭಾವ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_ಎಣಿಕೆ()+1], row[port_ಎಣಿಕೆ()+1]);
			ಪ್ರತಿಜ್ಞಾ(0);
			ಭಙ್ಗ;
		  }
	    }
      }

      levels0_ = ನೂತನ udp_levels_table[nlevels0_];
      levels1_ = ನೂತನ udp_levels_table[nlevels1_];
      levelsx_ = ನೂತನ udp_levels_table[nlevelsx_];
      levelsL_ = ನೂತನ udp_levels_table[nlevelsL_];
      edges0_ = ನೂತನ udp_edges_table[nedges0_];
      edges1_ = ನೂತನ udp_edges_table[nedges1_];
      edgesL_ = ನೂತನ udp_edges_table[nedgesL_];

      unsigned idx_lev0 = 0;
      unsigned idx_lev1 = 0;
      unsigned idx_levx = 0;
      unsigned idx_levL = 0;
      unsigned idx_edg0 = 0;
      unsigned idx_edg1 = 0;
      unsigned idx_edgL = 0;

      ಪ್ರತಿ (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    ಧ್ರುವ ವರ್ಣ*row = tab[idx];

	    ಯದಿ (strspn(row, "01xblh?") >= port_ಎಣಿಕೆ()+1) {
		  ಸಂರಚನಾ udp_levels_table cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  ಪ್ರತಿ (unsigned pp = 0 ;  pp < port_ಎಣಿಕೆ() ;  pp += 1) {
			unsigned long mask_bit = 1UL << pp;
			or_based_on_char(cur, row[pp+1], mask_bit);
		  }

		  or_based_on_char(cur, row[0], 1UL << port_ಎಣಿಕೆ());

		  ವಿಕಲ್ಪ (row[port_ಎಣಿಕೆ()+1]) {
		      ಪಕ್ಷ '0':
			levels0_[idx_lev0++] = cur;
			ಭಙ್ಗ;
		      ಪಕ್ಷ '1':
			levels1_[idx_lev1++] = cur;
			ಭಙ್ಗ;
		      ಪಕ್ಷ 'x':
			levelsx_[idx_levx++] = cur;
			ಭಙ್ಗ;
		      ಪಕ್ಷ '-':
			levelsL_[idx_levL++] = cur;
			ಭಙ್ಗ;
		      ಸ್ವಭಾವ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_ಎಣಿಕೆ()+1], row[port_ಎಣಿಕೆ()+1]);
			ಪ್ರತಿಜ್ಞಾ(0);
			ಭಙ್ಗ;
		  }

	    } ಅನ್ಯಥಾ {
		  ಸಂರಚನಾ udp_edges_table cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  cur.edge_position = 0;
		  cur.edge_mask0 = 0;
		  cur.edge_mask1 = 0;
		  cur.edge_maskx = 0;

		  ತಾರ್ಕಿಕ need_ext0_table = ಅಸತ್ಯ;
		  ಸಂರಚನಾ udp_edges_table ext0;
		  ext0.mask0 = 0;
		  ext0.mask1 = 0;
		  ext0.maskx = 0;
		  ext0.edge_position = 0;
		  ext0.edge_mask0 = 0;
		  ext0.edge_mask1 = 0;
		  ext0.edge_maskx = 0;

		  ತಾರ್ಕಿಕ need_ext1_table = ಅಸತ್ಯ;
		  ಸಂರಚನಾ udp_edges_table ext1;
		  ext1.mask0 = 0;
		  ext1.mask1 = 0;
		  ext1.maskx = 0;
		  ext1.edge_position = 0;
		  ext1.edge_mask0 = 0;
		  ext1.edge_mask1 = 0;
		  ext1.edge_maskx = 0;

		  ಪ್ರತಿ (unsigned pp = 0 ;  pp < port_ಎಣಿಕೆ() ; pp += 1) {
			ವಿಕಲ್ಪ (row[pp+1]) {
			    ಪಕ್ಷ 'n':
			      edge_based_on_char(cur, 'N', pp);
			      edge_based_on_char(ext0, '_', pp);
			      need_ext0_table = ಸತ್ಯ;
			      ಭಙ್ಗ;
			    ಪಕ್ಷ 'p':
			      edge_based_on_char(cur, 'P', pp);
			      edge_based_on_char(ext0, '+', pp);
			      need_ext0_table = ಸತ್ಯ;
			      ಭಙ್ಗ;
			    ಪಕ್ಷ '*':
			      edge_based_on_char(cur, 'P', pp);
			      edge_based_on_char(ext0, 'N', pp);
			      edge_based_on_char(ext1, 'B', pp);
			      need_ext0_table = ಸತ್ಯ;
			      need_ext1_table = ಸತ್ಯ;
			      ಭಙ್ಗ;
			    ಸ್ವಭಾವ:
			      edge_based_on_char(cur, row[pp+1], pp);
			      edge_based_on_char(ext0, row[pp+1], pp);
			      edge_based_on_char(ext1, row[pp+1], pp);
			      ಭಙ್ಗ;
			}
		  }
		  edge_based_on_char(cur, row[0], port_ಎಣಿಕೆ());
		  edge_based_on_char(ext0, row[0], port_ಎಣಿಕೆ());
		  edge_based_on_char(ext1, row[0], port_ಎಣಿಕೆ());

		  ವಿಕಲ್ಪ (row[port_ಎಣಿಕೆ()+1]) {
		      ಪಕ್ಷ '0':
			edges0_[idx_edg0++] = cur;
			ಯದಿ (need_ext0_table)
			      edges0_[idx_edg0++] = ext0;
			ಯದಿ (need_ext1_table)
			      edges0_[idx_edg0++] = ext1;
			ಭಙ್ಗ;
		      ಪಕ್ಷ '1':
			edges1_[idx_edg1++] = cur;
			ಯದಿ (need_ext0_table)
			      edges1_[idx_edg1++] = ext0;
			ಯದಿ (need_ext1_table)
			      edges1_[idx_edg1++] = ext1;
			ಭಙ್ಗ;
		      ಪಕ್ಷ 'x':
			ಭಙ್ಗ;
		      ಪಕ್ಷ '-':
			edgesL_[idx_edgL++] = cur;
			ಯದಿ (need_ext0_table)
			      edgesL_[idx_edgL++] = ext0;
			ಯದಿ (need_ext1_table)
			      edgesL_[idx_edgL++] = ext1;
			ಭಙ್ಗ;
		      ಸ್ವಭಾವ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_ಎಣಿಕೆ()+1], row[port_ಎಣಿಕೆ()+1]);
			ಪ್ರತಿಜ್ಞಾ(0);
			ಭಙ್ಗ;
		  }

	    }
	    delete[] tab[idx];
      }
      ಮುಕ್ತ(tab);

      ಪ್ರತಿಜ್ಞಾ(idx_edg0 == nedges0_);
      ಪ್ರತಿಜ್ಞಾ(idx_edg1 == nedges1_);
      ಪ್ರತಿಜ್ಞಾ(idx_edgL == nedgesL_);

}

ತಾರ್ಕಿಕ operator == (ಧ್ರುವ udp_levels_table&a, ಧ್ರುವ udp_levels_table&b)
{
      ಯದಿ (a.mask0 != b.mask0)
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      ಯದಿ (a.mask1 != b.mask1)
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      ಯದಿ (a.maskx != b.maskx)
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
      ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

vvp_bit4_t vvp_udp_seq_s::calculate_output(ಧ್ರುವ udp_levels_table&cur,
					   ಧ್ರುವ udp_levels_table&prev,
					   vvp_bit4_t cur_out)
{
      ಯದಿ (cur == prev)
	    ಪ್ರತ್ಯಾಗಮ cur_out;

      udp_levels_table cur_tmp = cur;

      unsigned long mask_out = 1UL << port_ಎಣಿಕೆ();
      ವಿಕಲ್ಪ (cur_out) {
	  ಪಕ್ಷ BIT4_0:
	    cur_tmp.mask0 |= mask_out;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ BIT4_1:
	    cur_tmp.mask1 |= mask_out;
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    cur_tmp.maskx |= mask_out;
	    ಭಙ್ಗ;
      }

      vvp_bit4_t lev = test_levels_(cur_tmp);
      ಯದಿ (lev == BIT4_Z) {
	    lev = test_edges_(cur_tmp, prev);
      }

      ಪ್ರತ್ಯಾಗಮ lev;
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
vvp_bit4_t vvp_udp_seq_s::test_levels_(ಧ್ರುವ udp_levels_table&cur)
{
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    ಯದಿ (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  ಅನುವರ್ತ;
	    ಯದಿ (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  ಅನುವರ್ತ;
	    ಯದಿ (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  ಅನುವರ್ತ;

	    ಪ್ರತ್ಯಾಗಮ BIT4_0;
      }

      ಪ್ರತಿ (unsigned idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    ಯದಿ (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  ಅನುವರ್ತ;
	    ಯದಿ (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  ಅನುವರ್ತ;
	    ಯದಿ (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  ಅನುವರ್ತ;

	    ಪ್ರತ್ಯಾಗಮ BIT4_1;
      }

	/* We need to test against an explicit X-output table, since
	   we need to distinguish from an X output and no match. */
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < nlevelsx_ ;  idx += 1) {
	    ಯದಿ (cur.mask0 != (cur.mask0 & levelsx_[idx].mask0))
		  ಅನುವರ್ತ;
	    ಯದಿ (cur.mask1 != (cur.mask1 & levelsx_[idx].mask1))
		  ಅನುವರ್ತ;
	    ಯದಿ (cur.maskx != (cur.maskx & levelsx_[idx].maskx))
		  ಅನುವರ್ತ;

	    ಪ್ರತ್ಯಾಗಮ BIT4_X;
      }

	/* Test the table that requests the next output be the same as
	   the current output. This gets the current output from the
	   levels table that was passed in. */
      ಪ್ರತಿ (unsigned idx = 0 ;  idx < nlevelsL_ ;  idx += 1) {
	    ಯದಿ (cur.mask0 != (cur.mask0 & levelsL_[idx].mask0))
		  ಅನುವರ್ತ;
	    ಯದಿ (cur.mask1 != (cur.mask1 & levelsL_[idx].mask1))
		  ಅನುವರ್ತ;
	    ಯದಿ (cur.maskx != (cur.maskx & levelsL_[idx].maskx))
		  ಅನುವರ್ತ;

	    ಯದಿ (cur.mask0 & (1 << port_ಎಣಿಕೆ()))
		  ಪ್ರತ್ಯಾಗಮ BIT4_0;
	    ಯದಿ (cur.mask1 & (1 << port_ಎಣಿಕೆ()))
		  ಪ್ರತ್ಯಾಗಮ BIT4_1;
	    ಯದಿ (cur.maskx & (1 << port_ಎಣಿಕೆ()))
		  ಪ್ರತ್ಯಾಗಮ BIT4_X;

	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಪ್ರತ್ಯಾಗಮ BIT4_X;
      }

	/* No explicit levels entry match. Return a Z to signal that
	   further testing is needed. */
      ಪ್ರತ್ಯಾಗಮ BIT4_Z;
}

vvp_bit4_t vvp_udp_seq_s::test_edges_(ಧ್ರುವ udp_levels_table&cur,
				      ಧ್ರುವ udp_levels_table&prev)
{
	/* The edge_mask is true for all bits that are different in
	   the cur and prev tables. */
      unsigned long edge0_mask = cur.mask0 ^ prev.mask0;
      unsigned long edgex_mask = cur.maskx ^ prev.maskx;
      unsigned long edge1_mask = cur.mask1 ^ prev.mask1;

      unsigned long edge_mask = edge0_mask|edgex_mask|edge1_mask;
      edge_mask &= ~ (-1UL << port_ಎಣಿಕೆ());

	/* If there are no differences, then there are no edges. Give
	   up now. */
      ಯದಿ (edge_mask == 0)
	    ಪ್ರತ್ಯಾಗಮ BIT4_X;

      unsigned edge_position = 0;
      ಯಾವತ್ ((edge_mask&1) == 0) {
	    edge_mask >>= 1;
	    edge_position += 1;
      }

	/* We expect that there is exactly one edge in here. */
      ಪ್ರತಿಜ್ಞಾ(edge_mask == 1);

      edge_mask = 1UL << edge_position;

      unsigned edge_mask0 = (prev.mask0&edge_mask)? 1 : 0;
      unsigned edge_maskx = (prev.maskx&edge_mask)? 1 : 0;
      unsigned edge_mask1 = (prev.mask1&edge_mask)? 1 : 0;


	/* Now the edge_position and edge_mask* variables have the
	   values we use to test the applicability of the edge_table
	   entries. */

      ಪ್ರತಿ (unsigned idx = 0 ;  idx < nedges0_ ;  idx += 1) {
	    ಧ್ರುವ ಸಂರಚನಾ udp_edges_table*row = edges0_ + idx;

	    ಯದಿ (row->edge_position != edge_position)
		  ಅನುವರ್ತ;
	    ಯದಿ (edge_mask0 && !row->edge_mask0)
		  ಅನುವರ್ತ;
	    ಯದಿ (edge_maskx && !row->edge_maskx)
		  ಅನುವರ್ತ;
	    ಯದಿ (edge_mask1 && !row->edge_mask1)
		  ಅನುವರ್ತ;
	    ಯದಿ (cur.mask0 != (cur.mask0 & row->mask0))
		  ಅನುವರ್ತ;
	    ಯದಿ (cur.maskx != (cur.maskx & row->maskx))
		  ಅನುವರ್ತ;
	    ಯದಿ (cur.mask1 != (cur.mask1 & row->mask1))
		  ಅನುವರ್ತ;

	    ಪ್ರತ್ಯಾಗಮ BIT4_0;
      }

      ಪ್ರತಿ (unsigned idx = 0 ;  idx < nedges1_ ;  idx += 1) {
	    ಧ್ರುವ ಸಂರಚನಾ udp_edges_table*row = edges1_ + idx;

	    ಯದಿ (row->edge_position != edge_position)
		  ಅನುವರ್ತ;
	    ಯದಿ (edge_mask0 && !row->edge_mask0)
		  ಅನುವರ್ತ;
	    ಯದಿ (edge_maskx && !row->edge_maskx)
		  ಅನುವರ್ತ;
	    ಯದಿ (edge_mask1 && !row->edge_mask1)
		  ಅನುವರ್ತ;
	    ಯದಿ (cur.mask0 != (cur.mask0 & row->mask0))
		  ಅನುವರ್ತ;
	    ಯದಿ (cur.maskx != (cur.maskx & row->maskx))
		  ಅನುವರ್ತ;
	    ಯದಿ (cur.mask1 != (cur.mask1 & row->mask1))
		  ಅನುವರ್ತ;

	    ಪ್ರತ್ಯಾಗಮ BIT4_1;
      }

      ಪ್ರತಿ (unsigned idx = 0 ;  idx < nedgesL_ ;  idx += 1) {
	    ಧ್ರುವ ಸಂರಚನಾ udp_edges_table*row = edgesL_ + idx;

	    ಯದಿ (row->edge_position != edge_position)
		  ಅನುವರ್ತ;
	    ಯದಿ (edge_mask0 && !row->edge_mask0)
		  ಅನುವರ್ತ;
	    ಯದಿ (edge_maskx && !row->edge_maskx)
		  ಅನುವರ್ತ;
	    ಯದಿ (edge_mask1 && !row->edge_mask1)
		  ಅನುವರ್ತ;
	    ಯದಿ (cur.mask0 != (cur.mask0 & row->mask0))
		  ಅನುವರ್ತ;
	    ಯದಿ (cur.maskx != (cur.maskx & row->maskx))
		  ಅನುವರ್ತ;
	    ಯದಿ (cur.mask1 != (cur.mask1 & row->mask1))
		  ಅನುವರ್ತ;

	    ಯದಿ (cur.mask0 & (1 << port_ಎಣಿಕೆ()))
		  ಪ್ರತ್ಯಾಗಮ BIT4_0;
	    ಯದಿ (cur.mask1 & (1 << port_ಎಣಿಕೆ()))
		  ಪ್ರತ್ಯಾಗಮ BIT4_1;
	    ಯದಿ (cur.maskx & (1 << port_ಎಣಿಕೆ()))
		  ಪ್ರತ್ಯಾಗಮ BIT4_X;

	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಪ್ರತ್ಯಾಗಮ BIT4_X;
      }

      ಪ್ರತ್ಯಾಗಮ BIT4_X;
}

vvp_udp_fun_core::vvp_udp_fun_core(vvp_net_t*net, vvp_udp_s*def)
: vvp_wide_fun_core(net, def->port_ಎಣಿಕೆ())
{
      def_ = def;
      cur_out_ = def_->get_init();
	// Assume initially that all the inputs are 1'bx
      current_.mask0 = 0;
      current_.mask1 = 0;
      current_.maskx = ~ ((-1UL) << port_ಎಣಿಕೆ());

        // If the initial value is 0 or 1, schedule the initial assignment
        // normally, so that any sensitive always processes can be started
        // first.
      ಯದಿ (cur_out_ != BIT4_X)
	    schedule_generic(ಏತತ್, 0, ಅಸತ್ಯ);
      ಅನ್ಯಥಾ
	    schedule_functor(ಏತತ್);
}

vvp_udp_fun_core::~vvp_udp_fun_core()
{
}

/*
 * This method is used to propagate the initial value on startup.
 */
ಶೂನ್ಯ vvp_udp_fun_core::run_run()
{
      vvp_vector4_t tmp (1);
      tmp.set_bit(0, cur_out_);
      propagate_vec4(tmp);
}

ಶೂನ್ಯ vvp_udp_fun_core::recv_vec4_from_inputs(unsigned port)
{
	/* For now, assume udps are 1-bit wide. */
      unsigned port_width = value(port).size();
      ಪ್ರತಿಜ್ಞಾ(port_width == 1);

      unsigned long mask = 1UL << port;

      udp_levels_table prev = current_;

      ವಿಕಲ್ಪ (value(port).value(0)) {

	  ಪಕ್ಷ BIT4_0:
	    current_.mask0 |= mask;
	    current_.mask1 &= ~mask;
	    current_.maskx &= ~mask;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ BIT4_1:
	    current_.mask0 &= ~mask;
	    current_.mask1 |= mask;
	    current_.maskx &= ~mask;
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    current_.mask0 &= ~mask;
	    current_.mask1 &= ~mask;
	    current_.maskx |= mask;
	    ಭಙ್ಗ;
      }

      vvp_bit4_t out_bit = def_->calculate_output(current_, prev, cur_out_);

      ಯದಿ (out_bit == cur_out_)
	    ಪ್ರತ್ಯಾಗಮ;

      cur_out_ = out_bit;
      schedule_functor(ಏತತ್);
}


/*
 * This function is called by the parser in response to a .udp
 * node. We create the nodes needed to integrate the UDP into the
 * netlist. The definition should be parsed already.
 */
ಶೂನ್ಯ compile_udp_functor(ವರ್ಣ*label, ವರ್ಣ*type,
			 unsigned argc, ಸಂರಚನಾ symb_s*argv)
{
      ಸಂರಚನಾ vvp_udp_s *def = udp_find(type);
      ಪ್ರತಿಜ್ಞಾ(def);
      ಮುಕ್ತ(type);

      vvp_net_t*ptr = ನೂತನ vvp_net_t;
      vvp_udp_fun_core*core = ನೂತನ vvp_udp_fun_core(ptr, def);
      ptr->fun = core;

      define_functor_ಚಿಹ್ನೆ(label, ptr);
      ಮುಕ್ತ(label);

      wide_inputs_connect(core, argc, argv);
      ಮುಕ್ತ(argv);
}
