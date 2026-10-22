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

#ଅନ୍ତର୍ଭାଵ "udp.h"
#ଅନ୍ତର୍ଭାଵ "schedule.h"
#ଅନ୍ତର୍ଭାଵ "symbols.h"
#ଅନ୍ତର୍ଭାଵ "compile.h"
#ଅନ୍ତର୍ଭାଵ "config.h"
#ଅନ୍ତର୍ଭାଵ <cassert>
#ଅନ୍ତର୍ଭାଵ <cstdlib>
#ଅନ୍ତର୍ଭାଵ <cstring>
#ଅନ୍ତର୍ଭାଵ <cstdio>
#ଅନ୍ତର୍ଭାଵ <iostream>
#ifdef CHECK_WITH_VALGRIND
#ଅନ୍ତର୍ଭାଵ "vvp_cleanup.h"
#ଅନ୍ତର୍ଭାଵ "ivl_alloc.h"
#endif

using ନାମାକାଶ std;

// We may need these later when we build the VPI interface to
// UDP definitions.
#ifdef CHECK_WITH_VALGRIND
ସ୍ଥିର vvp_udp_s **udp_defns = 0;
ସ୍ଥିର unsigned udp_defns_ଗଣନା = 0;

ଶୂନ୍ୟ udp_defns_delete()
{
      ପାଇଁ (unsigned idx = 0; idx < udp_defns_ଗଣନା; idx += 1) {
	    ଯଦି (udp_defns[idx]->is_sequential()) {
		  vvp_udp_seq_s *obj = static_cast<vvp_udp_seq_s *>
		                       (udp_defns[idx]);
		  delete obj;
	    } ଅନ୍ୟଥା {
		  vvp_udp_comb_s *obj = static_cast<vvp_udp_comb_s *>
		                        (udp_defns[idx]);
		  delete obj;
	    }
      }
      ମୁକ୍ତ(udp_defns);
      udp_defns = 0;
      udp_defns_ଗଣନା = 0;
}
#endif

ସ୍ଥିର ପ୍ରତୀକ_table_t udp_table;

ଶୂନ୍ୟ delete_udp_symbols()
{
      delete_ପ୍ରତୀକ_table(udp_table);
      udp_table = 0;
}

ସଂରଚନା vvp_udp_s *udp_find(ଅଚଳ ଅକ୍ଷର *label)
{
      ପ୍ରତୀକ_ମୂଲ୍ୟ_t v = sym_get_ମୂଲ୍ୟ(udp_table, label);
      ଫେରନ୍ତୁ (ସଂରଚନା vvp_udp_s *)v.ptr;
}

ostream& operator <<(ostream&o, ଅଚଳ ସଂରଚନା udp_levels_table&table)
{
      o << "[" << hex << table.mask0
	<< "/" << hex << table.mask1
	<< "/" << hex << table.maskx << "]";
      ଫେରନ୍ତୁ o;
}

vvp_udp_s::vvp_udp_s(ଅଚଳ ଅକ୍ଷର*label, ଅକ୍ଷର*name__, unsigned ports,
                     vvp_bit4_t init, ତାର୍କିକ type)
: name_(name__), ports_(ports), init_(init), seq_(type)
{
      ଯଦି (!udp_table)
	    udp_table = new_ପ୍ରତୀକ_table();

      ପ୍ରତିଜ୍ଞା( !udp_find(label) );

      ପ୍ରତୀକ_ମୂଲ୍ୟ_t v;
      v.ptr = ଏହି;
      sym_set_ମୂଲ୍ୟ(udp_table, label, v);

#ifdef CHECK_WITH_VALGRIND
      udp_defns_ଗଣନା += 1;
      udp_defns = static_cast<vvp_udp_s **>(realloc(udp_defns,
                  udp_defns_ଗଣନା*ଆକାର(vvp_udp_s **)));
      udp_defns[udp_defns_ଗଣନା-1] = ଏହି;
#endif
}

vvp_udp_s::~vvp_udp_s()
{
      delete[] name_;
}

unsigned vvp_udp_s::port_ଗଣନା() ଅଚଳ
{
      ଫେରନ୍ତୁ ports_;
}

vvp_bit4_t vvp_udp_s::get_init() ଅଚଳ
{
      ଫେରନ୍ତୁ init_;
}

vvp_udp_comb_s::vvp_udp_comb_s(ଅଚଳ ଅକ୍ଷର*label, ଅକ୍ଷର*name__, unsigned ports)
: vvp_udp_s(label, name__, ports, BIT4_X, ମିଥ୍ୟା)
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
vvp_bit4_t vvp_udp_comb_s::test_levels(ଅଚଳ udp_levels_table&cur)
{
	/* To test for a row match, test that the mask0, mask1 and
	   maskx vectors all have bits set where the matching
	   cur.mask0/1/x vectors have bits set. It is possible that a
	   levels0_[idx] vector has more bits set then the cur mask,
	   but this is OK and these bits are to be ignored. */

      ପାଇଁ (unsigned idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    ଯଦି (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  ଜାରି;
	    ଯଦି (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  ଜାରି;
	    ଯଦି (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  ଜାରି;

	    ଫେରନ୍ତୁ BIT4_0;
      }

      ପାଇଁ (unsigned idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    ଯଦି (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  ଜାରି;
	    ଯଦି (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  ଜାରି;
	    ଯଦି (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  ଜାରି;

	    ଫେରନ୍ତୁ BIT4_1;
      }

      ଫେରନ୍ତୁ BIT4_X;
}

vvp_bit4_t vvp_udp_comb_s::calculate_output(ଅଚଳ udp_levels_table&cur,
					    ଅଚଳ udp_levels_table&,
					    vvp_bit4_t)
{
      ଫେରନ୍ତୁ test_levels(cur);
}

ସ୍ଥିର ଶୂନ୍ୟ or_based_on_char(udp_levels_table&cur, ଅକ୍ଷର flag,
			     unsigned long mask_bit)
{
      ବାଛ (flag) {
	  ସ୍ଥିତି '0':
	    cur.mask0 |= mask_bit;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି '1':
	    cur.mask1 |= mask_bit;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'x':
	    cur.maskx |= mask_bit;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'b':
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'l':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'h':
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି '?':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "Unsupported flag %c(%d).\n", flag, flag);
	    ପ୍ରତିଜ୍ଞା(0);
      }
}

ଶୂନ୍ୟ vvp_udp_comb_s::compile_table(ଅକ୍ଷର**tab)
{
      unsigned nrows0 = 0, nrows1 = 0;

	/* First run through the table to figure out the number of
	   rows I need for each kind of table. */
      ପାଇଁ (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    ପ୍ରତିଜ୍ଞା(strlen(tab[idx]) == port_ଗଣନା() + 1);
	    ବାଛ (tab[idx][port_ଗଣନା()]) {
		ସ୍ଥିତି '0':
		  nrows0 += 1;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି '1':
		  nrows1 += 1;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି 'x':
		  ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		  fprintf(stderr, "Unsupported entry %c(%d).\n",
		          tab[idx][port_ଗଣନା()], tab[idx][port_ଗଣନା()]);
		  ପ୍ରତିଜ୍ଞା(0);
	    }
      }

      nlevels0_ = nrows0;
      levels0_ = ନୂତନ udp_levels_table[nlevels0_];

      nlevels1_ = nrows1;
      levels1_ = ନୂତନ udp_levels_table[nlevels1_];

      nrows0 = 0;
      nrows1 = 0;
      ପାଇଁ (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    ସଂରଚନା udp_levels_table cur;
	    cur.mask0 = 0;
	    cur.mask1 = 0;
	    cur.maskx = 0;
	    ଯଦି (port_ଗଣନା() > 8*ଆକାର(cur.mask0)) {
		  fprintf(stderr, "internal error: primitive port count=%u "
			  " > %zu\n", port_ଗଣନା(), ଆକାର(cur.mask0));
		  ପ୍ରତିଜ୍ଞା(port_ଗଣନା() <= 8*ଆକାର(cur.mask0));
	    }
	    ପାଇଁ (unsigned pp = 0 ;  pp < port_ଗଣନା() ;  pp += 1) {
		  unsigned long mask_bit = 1UL << pp;
		  or_based_on_char(cur, tab[idx][pp], mask_bit);
	    }

	    ବାଛ (tab[idx][port_ଗଣନା()]) {
		ସ୍ଥିତି '0':
		  levels0_[nrows0++] = cur;
		  ଭାଙ୍ଗ;
		ସ୍ଥିତି '1':
		  levels1_[nrows1++] = cur;
		  ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		  ଭାଙ୍ଗ;
	    }
	    delete[] tab[idx];
      }
      ମୁକ୍ତ(tab);

      ପ୍ରତିଜ୍ଞା(nrows0 == nlevels0_);
      ପ୍ରତିଜ୍ଞା(nrows1 == nlevels1_);
}

vvp_udp_seq_s::vvp_udp_seq_s(ଅଚଳ ଅକ୍ଷର*label, ଅକ୍ଷର*name__,
			     unsigned ports, vvp_bit4_t init)
: vvp_udp_s(label, name__, ports, init, ସତ୍ୟ)
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

ଶୂନ୍ୟ edge_based_on_char(ସଂରଚନା udp_edges_table&cur, ଅକ୍ଷର chr, unsigned pos)
{
      unsigned long mask_bit = 1UL << pos;

      ବାଛ (chr) {
	  ସ୍ଥିତି '0':
	    cur.mask0 |= mask_bit;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି '1':
	    cur.mask1 |= mask_bit;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'x':
	    cur.maskx |= mask_bit;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'b':
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'l':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'h':
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି '?':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    ଭାଙ୍ଗ;

	  ସ୍ଥିତି 'B': // (x?) edge
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'f': // (10) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'F': // (x0) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'M': // (1x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'N': // (1x) and (10) edge
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'P': // (0x) and (01) edge
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'q': // (bx) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'Q': // (0x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'r': // (01) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି 'R': // (x1) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି '%': // (?x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି '+': // (?1) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି '_': // (?0) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 1;
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    fprintf(stderr, "internal error: unknown edge code: %c\n", chr);
	    ପ୍ରତିଜ୍ଞା(0);
      }
}

ଶୂନ୍ୟ vvp_udp_seq_s::compile_table(ଅକ୍ଷର**tab)
{

      ପାଇଁ (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    ଅଚଳ ଅକ୍ଷର*row = tab[idx];
	    ପ୍ରତିଜ୍ଞା(strlen(row) == port_ଗଣନା() + 2);

	    ଯଦି (strspn(row, "01xblh?") >= port_ଗଣନା()+1) {

		  ବାଛ (row[port_ଗଣନା()+1]) {
		      ସ୍ଥିତି '0':
			nlevels0_ += 1;
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି '1':
			nlevels1_ += 1;
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି 'x':
			nlevelsx_ += 1;
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି '-':
			nlevelsL_ += 1;
			ଭାଙ୍ଗ;
		      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_ଗଣନା()+1], row[port_ଗଣନା()+1]);
			ପ୍ରତିଜ୍ଞା(0);
			ଭାଙ୍ଗ;
		  }

	    } ଅନ୍ୟଥା {
		    /* Rows that have n or p edges will need to be
		       expanded into 2 rows. */
		  unsigned extra = 0;
		  ଯଦି (strchr(row,'n'))
			extra = 1;
		  ଯଦି (strchr(row,'p'))
			extra = 1;
		  ଯଦି (strchr(row,'*'))
			extra = 2;

		  ବାଛ (row[port_ଗଣନା()+1]) {
		      ସ୍ଥିତି '0':
			nedges0_ += 1 + extra;
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି '1':
			nedges1_ += 1 + extra;
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି 'x':
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି '-':
			nedgesL_ += 1 + extra;
			ଭାଙ୍ଗ;
		      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_ଗଣନା()+1], row[port_ଗଣନା()+1]);
			ପ୍ରତିଜ୍ଞା(0);
			ଭାଙ୍ଗ;
		  }
	    }
      }

      levels0_ = ନୂତନ udp_levels_table[nlevels0_];
      levels1_ = ନୂତନ udp_levels_table[nlevels1_];
      levelsx_ = ନୂତନ udp_levels_table[nlevelsx_];
      levelsL_ = ନୂତନ udp_levels_table[nlevelsL_];
      edges0_ = ନୂତନ udp_edges_table[nedges0_];
      edges1_ = ନୂତନ udp_edges_table[nedges1_];
      edgesL_ = ନୂତନ udp_edges_table[nedgesL_];

      unsigned idx_lev0 = 0;
      unsigned idx_lev1 = 0;
      unsigned idx_levx = 0;
      unsigned idx_levL = 0;
      unsigned idx_edg0 = 0;
      unsigned idx_edg1 = 0;
      unsigned idx_edgL = 0;

      ପାଇଁ (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    ଅଚଳ ଅକ୍ଷର*row = tab[idx];

	    ଯଦି (strspn(row, "01xblh?") >= port_ଗଣନା()+1) {
		  ସଂରଚନା udp_levels_table cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  ପାଇଁ (unsigned pp = 0 ;  pp < port_ଗଣନା() ;  pp += 1) {
			unsigned long mask_bit = 1UL << pp;
			or_based_on_char(cur, row[pp+1], mask_bit);
		  }

		  or_based_on_char(cur, row[0], 1UL << port_ଗଣନା());

		  ବାଛ (row[port_ଗଣନା()+1]) {
		      ସ୍ଥିତି '0':
			levels0_[idx_lev0++] = cur;
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି '1':
			levels1_[idx_lev1++] = cur;
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି 'x':
			levelsx_[idx_levx++] = cur;
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି '-':
			levelsL_[idx_levL++] = cur;
			ଭାଙ୍ଗ;
		      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_ଗଣନା()+1], row[port_ଗଣନା()+1]);
			ପ୍ରତିଜ୍ଞା(0);
			ଭାଙ୍ଗ;
		  }

	    } ଅନ୍ୟଥା {
		  ସଂରଚନା udp_edges_table cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  cur.edge_position = 0;
		  cur.edge_mask0 = 0;
		  cur.edge_mask1 = 0;
		  cur.edge_maskx = 0;

		  ତାର୍କିକ need_ext0_table = ମିଥ୍ୟା;
		  ସଂରଚନା udp_edges_table ext0;
		  ext0.mask0 = 0;
		  ext0.mask1 = 0;
		  ext0.maskx = 0;
		  ext0.edge_position = 0;
		  ext0.edge_mask0 = 0;
		  ext0.edge_mask1 = 0;
		  ext0.edge_maskx = 0;

		  ତାର୍କିକ need_ext1_table = ମିଥ୍ୟା;
		  ସଂରଚନା udp_edges_table ext1;
		  ext1.mask0 = 0;
		  ext1.mask1 = 0;
		  ext1.maskx = 0;
		  ext1.edge_position = 0;
		  ext1.edge_mask0 = 0;
		  ext1.edge_mask1 = 0;
		  ext1.edge_maskx = 0;

		  ପାଇଁ (unsigned pp = 0 ;  pp < port_ଗଣନା() ; pp += 1) {
			ବାଛ (row[pp+1]) {
			    ସ୍ଥିତି 'n':
			      edge_based_on_char(cur, 'N', pp);
			      edge_based_on_char(ext0, '_', pp);
			      need_ext0_table = ସତ୍ୟ;
			      ଭାଙ୍ଗ;
			    ସ୍ଥିତି 'p':
			      edge_based_on_char(cur, 'P', pp);
			      edge_based_on_char(ext0, '+', pp);
			      need_ext0_table = ସତ୍ୟ;
			      ଭାଙ୍ଗ;
			    ସ୍ଥିତି '*':
			      edge_based_on_char(cur, 'P', pp);
			      edge_based_on_char(ext0, 'N', pp);
			      edge_based_on_char(ext1, 'B', pp);
			      need_ext0_table = ସତ୍ୟ;
			      need_ext1_table = ସତ୍ୟ;
			      ଭାଙ୍ଗ;
			    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			      edge_based_on_char(cur, row[pp+1], pp);
			      edge_based_on_char(ext0, row[pp+1], pp);
			      edge_based_on_char(ext1, row[pp+1], pp);
			      ଭାଙ୍ଗ;
			}
		  }
		  edge_based_on_char(cur, row[0], port_ଗଣନା());
		  edge_based_on_char(ext0, row[0], port_ଗଣନା());
		  edge_based_on_char(ext1, row[0], port_ଗଣନା());

		  ବାଛ (row[port_ଗଣନା()+1]) {
		      ସ୍ଥିତି '0':
			edges0_[idx_edg0++] = cur;
			ଯଦି (need_ext0_table)
			      edges0_[idx_edg0++] = ext0;
			ଯଦି (need_ext1_table)
			      edges0_[idx_edg0++] = ext1;
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି '1':
			edges1_[idx_edg1++] = cur;
			ଯଦି (need_ext0_table)
			      edges1_[idx_edg1++] = ext0;
			ଯଦି (need_ext1_table)
			      edges1_[idx_edg1++] = ext1;
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି 'x':
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି '-':
			edgesL_[idx_edgL++] = cur;
			ଯଦି (need_ext0_table)
			      edgesL_[idx_edgL++] = ext0;
			ଯଦି (need_ext1_table)
			      edgesL_[idx_edgL++] = ext1;
			ଭାଙ୍ଗ;
		      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_ଗଣନା()+1], row[port_ଗଣନା()+1]);
			ପ୍ରତିଜ୍ଞା(0);
			ଭାଙ୍ଗ;
		  }

	    }
	    delete[] tab[idx];
      }
      ମୁକ୍ତ(tab);

      ପ୍ରତିଜ୍ଞା(idx_edg0 == nedges0_);
      ପ୍ରତିଜ୍ଞା(idx_edg1 == nedges1_);
      ପ୍ରତିଜ୍ଞା(idx_edgL == nedgesL_);

}

ତାର୍କିକ operator == (ଅଚଳ udp_levels_table&a, ଅଚଳ udp_levels_table&b)
{
      ଯଦି (a.mask0 != b.mask0)
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      ଯଦି (a.mask1 != b.mask1)
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      ଯଦି (a.maskx != b.maskx)
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;
      ଫେରନ୍ତୁ ସତ୍ୟ;
}

vvp_bit4_t vvp_udp_seq_s::calculate_output(ଅଚଳ udp_levels_table&cur,
					   ଅଚଳ udp_levels_table&prev,
					   vvp_bit4_t cur_out)
{
      ଯଦି (cur == prev)
	    ଫେରନ୍ତୁ cur_out;

      udp_levels_table cur_tmp = cur;

      unsigned long mask_out = 1UL << port_ଗଣନା();
      ବାଛ (cur_out) {
	  ସ୍ଥିତି BIT4_0:
	    cur_tmp.mask0 |= mask_out;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_1:
	    cur_tmp.mask1 |= mask_out;
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    cur_tmp.maskx |= mask_out;
	    ଭାଙ୍ଗ;
      }

      vvp_bit4_t lev = test_levels_(cur_tmp);
      ଯଦି (lev == BIT4_Z) {
	    lev = test_edges_(cur_tmp, prev);
      }

      ଫେରନ୍ତୁ lev;
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
vvp_bit4_t vvp_udp_seq_s::test_levels_(ଅଚଳ udp_levels_table&cur)
{
      ପାଇଁ (unsigned idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    ଯଦି (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  ଜାରି;
	    ଯଦି (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  ଜାରି;
	    ଯଦି (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  ଜାରି;

	    ଫେରନ୍ତୁ BIT4_0;
      }

      ପାଇଁ (unsigned idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    ଯଦି (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  ଜାରି;
	    ଯଦି (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  ଜାରି;
	    ଯଦି (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  ଜାରି;

	    ଫେରନ୍ତୁ BIT4_1;
      }

	/* We need to test against an explicit X-output table, since
	   we need to distinguish from an X output and no match. */
      ପାଇଁ (unsigned idx = 0 ;  idx < nlevelsx_ ;  idx += 1) {
	    ଯଦି (cur.mask0 != (cur.mask0 & levelsx_[idx].mask0))
		  ଜାରି;
	    ଯଦି (cur.mask1 != (cur.mask1 & levelsx_[idx].mask1))
		  ଜାରି;
	    ଯଦି (cur.maskx != (cur.maskx & levelsx_[idx].maskx))
		  ଜାରି;

	    ଫେରନ୍ତୁ BIT4_X;
      }

	/* Test the table that requests the next output be the same as
	   the current output. This gets the current output from the
	   levels table that was passed in. */
      ପାଇଁ (unsigned idx = 0 ;  idx < nlevelsL_ ;  idx += 1) {
	    ଯଦି (cur.mask0 != (cur.mask0 & levelsL_[idx].mask0))
		  ଜାରି;
	    ଯଦି (cur.mask1 != (cur.mask1 & levelsL_[idx].mask1))
		  ଜାରି;
	    ଯଦି (cur.maskx != (cur.maskx & levelsL_[idx].maskx))
		  ଜାରି;

	    ଯଦି (cur.mask0 & (1 << port_ଗଣନା()))
		  ଫେରନ୍ତୁ BIT4_0;
	    ଯଦି (cur.mask1 & (1 << port_ଗଣନା()))
		  ଫେରନ୍ତୁ BIT4_1;
	    ଯଦି (cur.maskx & (1 << port_ଗଣନା()))
		  ଫେରନ୍ତୁ BIT4_X;

	    ପ୍ରତିଜ୍ଞା(0);
	    ଫେରନ୍ତୁ BIT4_X;
      }

	/* No explicit levels entry match. Return a Z to signal that
	   further testing is needed. */
      ଫେରନ୍ତୁ BIT4_Z;
}

vvp_bit4_t vvp_udp_seq_s::test_edges_(ଅଚଳ udp_levels_table&cur,
				      ଅଚଳ udp_levels_table&prev)
{
	/* The edge_mask is true for all bits that are different in
	   the cur and prev tables. */
      unsigned long edge0_mask = cur.mask0 ^ prev.mask0;
      unsigned long edgex_mask = cur.maskx ^ prev.maskx;
      unsigned long edge1_mask = cur.mask1 ^ prev.mask1;

      unsigned long edge_mask = edge0_mask|edgex_mask|edge1_mask;
      edge_mask &= ~ (-1UL << port_ଗଣନା());

	/* If there are no differences, then there are no edges. Give
	   up now. */
      ଯଦି (edge_mask == 0)
	    ଫେରନ୍ତୁ BIT4_X;

      unsigned edge_position = 0;
      ଯେତେବେଳେ ((edge_mask&1) == 0) {
	    edge_mask >>= 1;
	    edge_position += 1;
      }

	/* We expect that there is exactly one edge in here. */
      ପ୍ରତିଜ୍ଞା(edge_mask == 1);

      edge_mask = 1UL << edge_position;

      unsigned edge_mask0 = (prev.mask0&edge_mask)? 1 : 0;
      unsigned edge_maskx = (prev.maskx&edge_mask)? 1 : 0;
      unsigned edge_mask1 = (prev.mask1&edge_mask)? 1 : 0;


	/* Now the edge_position and edge_mask* variables have the
	   values we use to test the applicability of the edge_table
	   entries. */

      ପାଇଁ (unsigned idx = 0 ;  idx < nedges0_ ;  idx += 1) {
	    ଅଚଳ ସଂରଚନା udp_edges_table*row = edges0_ + idx;

	    ଯଦି (row->edge_position != edge_position)
		  ଜାରି;
	    ଯଦି (edge_mask0 && !row->edge_mask0)
		  ଜାରି;
	    ଯଦି (edge_maskx && !row->edge_maskx)
		  ଜାରି;
	    ଯଦି (edge_mask1 && !row->edge_mask1)
		  ଜାରି;
	    ଯଦି (cur.mask0 != (cur.mask0 & row->mask0))
		  ଜାରି;
	    ଯଦି (cur.maskx != (cur.maskx & row->maskx))
		  ଜାରି;
	    ଯଦି (cur.mask1 != (cur.mask1 & row->mask1))
		  ଜାରି;

	    ଫେରନ୍ତୁ BIT4_0;
      }

      ପାଇଁ (unsigned idx = 0 ;  idx < nedges1_ ;  idx += 1) {
	    ଅଚଳ ସଂରଚନା udp_edges_table*row = edges1_ + idx;

	    ଯଦି (row->edge_position != edge_position)
		  ଜାରି;
	    ଯଦି (edge_mask0 && !row->edge_mask0)
		  ଜାରି;
	    ଯଦି (edge_maskx && !row->edge_maskx)
		  ଜାରି;
	    ଯଦି (edge_mask1 && !row->edge_mask1)
		  ଜାରି;
	    ଯଦି (cur.mask0 != (cur.mask0 & row->mask0))
		  ଜାରି;
	    ଯଦି (cur.maskx != (cur.maskx & row->maskx))
		  ଜାରି;
	    ଯଦି (cur.mask1 != (cur.mask1 & row->mask1))
		  ଜାରି;

	    ଫେରନ୍ତୁ BIT4_1;
      }

      ପାଇଁ (unsigned idx = 0 ;  idx < nedgesL_ ;  idx += 1) {
	    ଅଚଳ ସଂରଚନା udp_edges_table*row = edgesL_ + idx;

	    ଯଦି (row->edge_position != edge_position)
		  ଜାରି;
	    ଯଦି (edge_mask0 && !row->edge_mask0)
		  ଜାରି;
	    ଯଦି (edge_maskx && !row->edge_maskx)
		  ଜାରି;
	    ଯଦି (edge_mask1 && !row->edge_mask1)
		  ଜାରି;
	    ଯଦି (cur.mask0 != (cur.mask0 & row->mask0))
		  ଜାରି;
	    ଯଦି (cur.maskx != (cur.maskx & row->maskx))
		  ଜାରି;
	    ଯଦି (cur.mask1 != (cur.mask1 & row->mask1))
		  ଜାରି;

	    ଯଦି (cur.mask0 & (1 << port_ଗଣନା()))
		  ଫେରନ୍ତୁ BIT4_0;
	    ଯଦି (cur.mask1 & (1 << port_ଗଣନା()))
		  ଫେରନ୍ତୁ BIT4_1;
	    ଯଦି (cur.maskx & (1 << port_ଗଣନା()))
		  ଫେରନ୍ତୁ BIT4_X;

	    ପ୍ରତିଜ୍ଞା(0);
	    ଫେରନ୍ତୁ BIT4_X;
      }

      ଫେରନ୍ତୁ BIT4_X;
}

vvp_udp_fun_core::vvp_udp_fun_core(vvp_net_t*net, vvp_udp_s*def)
: vvp_wide_fun_core(net, def->port_ଗଣନା())
{
      def_ = def;
      cur_out_ = def_->get_init();
	// Assume initially that all the inputs are 1'bx
      current_.mask0 = 0;
      current_.mask1 = 0;
      current_.maskx = ~ ((-1UL) << port_ଗଣନା());

        // If the initial value is 0 or 1, schedule the initial assignment
        // normally, so that any sensitive always processes can be started
        // first.
      ଯଦି (cur_out_ != BIT4_X)
	    schedule_generic(ଏହି, 0, ମିଥ୍ୟା);
      ଅନ୍ୟଥା
	    schedule_functor(ଏହି);
}

vvp_udp_fun_core::~vvp_udp_fun_core()
{
}

/*
 * This method is used to propagate the initial value on startup.
 */
ଶୂନ୍ୟ vvp_udp_fun_core::run_run()
{
      vvp_vector4_t tmp (1);
      tmp.set_bit(0, cur_out_);
      propagate_vec4(tmp);
}

ଶୂନ୍ୟ vvp_udp_fun_core::recv_vec4_from_inputs(unsigned port)
{
	/* For now, assume udps are 1-bit wide. */
      unsigned port_width = value(port).size();
      ପ୍ରତିଜ୍ଞା(port_width == 1);

      unsigned long mask = 1UL << port;

      udp_levels_table prev = current_;

      ବାଛ (value(port).value(0)) {

	  ସ୍ଥିତି BIT4_0:
	    current_.mask0 |= mask;
	    current_.mask1 &= ~mask;
	    current_.maskx &= ~mask;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_1:
	    current_.mask0 &= ~mask;
	    current_.mask1 |= mask;
	    current_.maskx &= ~mask;
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    current_.mask0 &= ~mask;
	    current_.mask1 &= ~mask;
	    current_.maskx |= mask;
	    ଭାଙ୍ଗ;
      }

      vvp_bit4_t out_bit = def_->calculate_output(current_, prev, cur_out_);

      ଯଦି (out_bit == cur_out_)
	    ଫେରନ୍ତୁ;

      cur_out_ = out_bit;
      schedule_functor(ଏହି);
}


/*
 * This function is called by the parser in response to a .udp
 * node. We create the nodes needed to integrate the UDP into the
 * netlist. The definition should be parsed already.
 */
ଶୂନ୍ୟ compile_udp_functor(ଅକ୍ଷର*label, ଅକ୍ଷର*type,
			 unsigned argc, ସଂରଚନା symb_s*argv)
{
      ସଂରଚନା vvp_udp_s *def = udp_find(type);
      ପ୍ରତିଜ୍ଞା(def);
      ମୁକ୍ତ(type);

      vvp_net_t*ptr = ନୂତନ vvp_net_t;
      vvp_udp_fun_core*core = ନୂତନ vvp_udp_fun_core(ptr, def);
      ptr->fun = core;

      define_functor_ପ୍ରତୀକ(label, ptr);
      ମୁକ୍ତ(label);

      wide_inputs_connect(core, argc, argv);
      ମୁକ୍ତ(argv);
}
