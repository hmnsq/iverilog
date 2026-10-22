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

#அந்தர்பாவ "udp.h"
#அந்தர்பாவ "schedule.h"
#அந்தர்பாவ "symbols.h"
#அந்தர்பாவ "compile.h"
#அந்தர்பாவ "config.h"
#அந்தர்பாவ <cassert>
#அந்தர்பாவ <cstdlib>
#அந்தர்பாவ <cstring>
#அந்தர்பாவ <cstdio>
#அந்தர்பாவ <iostream>
#ifdef CHECK_WITH_VALGRIND
#அந்தர்பாவ "vvp_cleanup.h"
#அந்தர்பாவ "ivl_alloc.h"
#endif

using நாமாகாஶ std;

// We may need these later when we build the VPI interface to
// UDP definitions.
#ifdef CHECK_WITH_VALGRIND
நிலையான vvp_udp_s **udp_defns = 0;
நிலையான unsigned udp_defns_மொத்தம் = 0;

வெறுமை udp_defns_delete()
{
      க்கு (unsigned idx = 0; idx < udp_defns_மொத்தம்; idx += 1) {
	    எனில் (udp_defns[idx]->is_sequential()) {
		  vvp_udp_seq_s *obj = static_cast<vvp_udp_seq_s *>
		                       (udp_defns[idx]);
		  delete obj;
	    } இல்லையெனில் {
		  vvp_udp_comb_s *obj = static_cast<vvp_udp_comb_s *>
		                        (udp_defns[idx]);
		  delete obj;
	    }
      }
      விடுவி(udp_defns);
      udp_defns = 0;
      udp_defns_மொத்தம் = 0;
}
#endif

நிலையான குறியீடு_table_t udp_table;

வெறுமை delete_udp_symbols()
{
      delete_குறியீடு_table(udp_table);
      udp_table = 0;
}

கட்டமைப்பு vvp_udp_s *udp_find(மாறிலி எழுத்து *label)
{
      குறியீடு_மதிப்பு_t v = sym_get_மதிப்பு(udp_table, label);
      திரும்பு (கட்டமைப்பு vvp_udp_s *)v.ptr;
}

ostream& operator <<(ostream&o, மாறிலி கட்டமைப்பு udp_levels_table&table)
{
      o << "[" << hex << table.mask0
	<< "/" << hex << table.mask1
	<< "/" << hex << table.maskx << "]";
      திரும்பு o;
}

vvp_udp_s::vvp_udp_s(மாறிலி எழுத்து*label, எழுத்து*name__, unsigned ports,
                     vvp_bit4_t init, தருக்க type)
: name_(name__), ports_(ports), init_(init), seq_(type)
{
      எனில் (!udp_table)
	    udp_table = new_குறியீடு_table();

      ப்ரதிஜ்ஞா( !udp_find(label) );

      குறியீடு_மதிப்பு_t v;
      v.ptr = இது;
      sym_set_மதிப்பு(udp_table, label, v);

#ifdef CHECK_WITH_VALGRIND
      udp_defns_மொத்தம் += 1;
      udp_defns = static_cast<vvp_udp_s **>(realloc(udp_defns,
                  udp_defns_மொத்தம்*அளவு(vvp_udp_s **)));
      udp_defns[udp_defns_மொத்தம்-1] = இது;
#endif
}

vvp_udp_s::~vvp_udp_s()
{
      delete[] name_;
}

unsigned vvp_udp_s::port_மொத்தம்() மாறிலி
{
      திரும்பு ports_;
}

vvp_bit4_t vvp_udp_s::get_init() மாறிலி
{
      திரும்பு init_;
}

vvp_udp_comb_s::vvp_udp_comb_s(மாறிலி எழுத்து*label, எழுத்து*name__, unsigned ports)
: vvp_udp_s(label, name__, ports, BIT4_X, பொய்)
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
vvp_bit4_t vvp_udp_comb_s::test_levels(மாறிலி udp_levels_table&cur)
{
	/* To test for a row match, test that the mask0, mask1 and
	   maskx vectors all have bits set where the matching
	   cur.mask0/1/x vectors have bits set. It is possible that a
	   levels0_[idx] vector has more bits set then the cur mask,
	   but this is OK and these bits are to be ignored. */

      க்கு (unsigned idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    எனில் (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  தொடர்;
	    எனில் (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  தொடர்;
	    எனில் (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  தொடர்;

	    திரும்பு BIT4_0;
      }

      க்கு (unsigned idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    எனில் (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  தொடர்;
	    எனில் (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  தொடர்;
	    எனில் (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  தொடர்;

	    திரும்பு BIT4_1;
      }

      திரும்பு BIT4_X;
}

vvp_bit4_t vvp_udp_comb_s::calculate_output(மாறிலி udp_levels_table&cur,
					    மாறிலி udp_levels_table&,
					    vvp_bit4_t)
{
      திரும்பு test_levels(cur);
}

நிலையான வெறுமை or_based_on_char(udp_levels_table&cur, எழுத்து flag,
			     unsigned long mask_bit)
{
      தேர்வு (flag) {
	  நிலை '0':
	    cur.mask0 |= mask_bit;
	    நிறுத்து;
	  நிலை '1':
	    cur.mask1 |= mask_bit;
	    நிறுத்து;
	  நிலை 'x':
	    cur.maskx |= mask_bit;
	    நிறுத்து;
	  நிலை 'b':
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    நிறுத்து;
	  நிலை 'l':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    நிறுத்து;
	  நிலை 'h':
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    நிறுத்து;
	  நிலை '?':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    நிறுத்து;
	  இயல்பு:
	    fprintf(stderr, "Unsupported flag %c(%d).\n", flag, flag);
	    ப்ரதிஜ்ஞா(0);
      }
}

வெறுமை vvp_udp_comb_s::compile_table(எழுத்து**tab)
{
      unsigned nrows0 = 0, nrows1 = 0;

	/* First run through the table to figure out the number of
	   rows I need for each kind of table. */
      க்கு (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    ப்ரதிஜ்ஞா(strlen(tab[idx]) == port_மொத்தம்() + 1);
	    தேர்வு (tab[idx][port_மொத்தம்()]) {
		நிலை '0':
		  nrows0 += 1;
		  நிறுத்து;
		நிலை '1':
		  nrows1 += 1;
		  நிறுத்து;
		நிலை 'x':
		  நிறுத்து;
		இயல்பு:
		  fprintf(stderr, "Unsupported entry %c(%d).\n",
		          tab[idx][port_மொத்தம்()], tab[idx][port_மொத்தம்()]);
		  ப்ரதிஜ்ஞா(0);
	    }
      }

      nlevels0_ = nrows0;
      levels0_ = புதிய udp_levels_table[nlevels0_];

      nlevels1_ = nrows1;
      levels1_ = புதிய udp_levels_table[nlevels1_];

      nrows0 = 0;
      nrows1 = 0;
      க்கு (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    கட்டமைப்பு udp_levels_table cur;
	    cur.mask0 = 0;
	    cur.mask1 = 0;
	    cur.maskx = 0;
	    எனில் (port_மொத்தம்() > 8*அளவு(cur.mask0)) {
		  fprintf(stderr, "internal error: primitive port count=%u "
			  " > %zu\n", port_மொத்தம்(), அளவு(cur.mask0));
		  ப்ரதிஜ்ஞா(port_மொத்தம்() <= 8*அளவு(cur.mask0));
	    }
	    க்கு (unsigned pp = 0 ;  pp < port_மொத்தம்() ;  pp += 1) {
		  unsigned long mask_bit = 1UL << pp;
		  or_based_on_char(cur, tab[idx][pp], mask_bit);
	    }

	    தேர்வு (tab[idx][port_மொத்தம்()]) {
		நிலை '0':
		  levels0_[nrows0++] = cur;
		  நிறுத்து;
		நிலை '1':
		  levels1_[nrows1++] = cur;
		  நிறுத்து;
		இயல்பு:
		  நிறுத்து;
	    }
	    delete[] tab[idx];
      }
      விடுவி(tab);

      ப்ரதிஜ்ஞா(nrows0 == nlevels0_);
      ப்ரதிஜ்ஞா(nrows1 == nlevels1_);
}

vvp_udp_seq_s::vvp_udp_seq_s(மாறிலி எழுத்து*label, எழுத்து*name__,
			     unsigned ports, vvp_bit4_t init)
: vvp_udp_s(label, name__, ports, init, உண்மை)
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

வெறுமை edge_based_on_char(கட்டமைப்பு udp_edges_table&cur, எழுத்து chr, unsigned pos)
{
      unsigned long mask_bit = 1UL << pos;

      தேர்வு (chr) {
	  நிலை '0':
	    cur.mask0 |= mask_bit;
	    நிறுத்து;
	  நிலை '1':
	    cur.mask1 |= mask_bit;
	    நிறுத்து;
	  நிலை 'x':
	    cur.maskx |= mask_bit;
	    நிறுத்து;
	  நிலை 'b':
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    நிறுத்து;
	  நிலை 'l':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    நிறுத்து;
	  நிலை 'h':
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    நிறுத்து;
	  நிலை '?':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    நிறுத்து;

	  நிலை 'B': // (x?) edge
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    நிறுத்து;
	  நிலை 'f': // (10) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    நிறுத்து;
	  நிலை 'F': // (x0) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    நிறுத்து;
	  நிலை 'M': // (1x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    நிறுத்து;
	  நிலை 'N': // (1x) and (10) edge
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    நிறுத்து;
	  நிலை 'P': // (0x) and (01) edge
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    நிறுத்து;
	  நிலை 'q': // (bx) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    நிறுத்து;
	  நிலை 'Q': // (0x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    நிறுத்து;
	  நிலை 'r': // (01) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    நிறுத்து;
	  நிலை 'R': // (x1) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    நிறுத்து;
	  நிலை '%': // (?x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    நிறுத்து;
	  நிலை '+': // (?1) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    நிறுத்து;
	  நிலை '_': // (?0) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 1;
	    நிறுத்து;
	  இயல்பு:
	    fprintf(stderr, "internal error: unknown edge code: %c\n", chr);
	    ப்ரதிஜ்ஞா(0);
      }
}

வெறுமை vvp_udp_seq_s::compile_table(எழுத்து**tab)
{

      க்கு (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    மாறிலி எழுத்து*row = tab[idx];
	    ப்ரதிஜ்ஞா(strlen(row) == port_மொத்தம்() + 2);

	    எனில் (strspn(row, "01xblh?") >= port_மொத்தம்()+1) {

		  தேர்வு (row[port_மொத்தம்()+1]) {
		      நிலை '0':
			nlevels0_ += 1;
			நிறுத்து;
		      நிலை '1':
			nlevels1_ += 1;
			நிறுத்து;
		      நிலை 'x':
			nlevelsx_ += 1;
			நிறுத்து;
		      நிலை '-':
			nlevelsL_ += 1;
			நிறுத்து;
		      இயல்பு:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_மொத்தம்()+1], row[port_மொத்தம்()+1]);
			ப்ரதிஜ்ஞா(0);
			நிறுத்து;
		  }

	    } இல்லையெனில் {
		    /* Rows that have n or p edges will need to be
		       expanded into 2 rows. */
		  unsigned extra = 0;
		  எனில் (strchr(row,'n'))
			extra = 1;
		  எனில் (strchr(row,'p'))
			extra = 1;
		  எனில் (strchr(row,'*'))
			extra = 2;

		  தேர்வு (row[port_மொத்தம்()+1]) {
		      நிலை '0':
			nedges0_ += 1 + extra;
			நிறுத்து;
		      நிலை '1':
			nedges1_ += 1 + extra;
			நிறுத்து;
		      நிலை 'x':
			நிறுத்து;
		      நிலை '-':
			nedgesL_ += 1 + extra;
			நிறுத்து;
		      இயல்பு:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_மொத்தம்()+1], row[port_மொத்தம்()+1]);
			ப்ரதிஜ்ஞா(0);
			நிறுத்து;
		  }
	    }
      }

      levels0_ = புதிய udp_levels_table[nlevels0_];
      levels1_ = புதிய udp_levels_table[nlevels1_];
      levelsx_ = புதிய udp_levels_table[nlevelsx_];
      levelsL_ = புதிய udp_levels_table[nlevelsL_];
      edges0_ = புதிய udp_edges_table[nedges0_];
      edges1_ = புதிய udp_edges_table[nedges1_];
      edgesL_ = புதிய udp_edges_table[nedgesL_];

      unsigned idx_lev0 = 0;
      unsigned idx_lev1 = 0;
      unsigned idx_levx = 0;
      unsigned idx_levL = 0;
      unsigned idx_edg0 = 0;
      unsigned idx_edg1 = 0;
      unsigned idx_edgL = 0;

      க்கு (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    மாறிலி எழுத்து*row = tab[idx];

	    எனில் (strspn(row, "01xblh?") >= port_மொத்தம்()+1) {
		  கட்டமைப்பு udp_levels_table cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  க்கு (unsigned pp = 0 ;  pp < port_மொத்தம்() ;  pp += 1) {
			unsigned long mask_bit = 1UL << pp;
			or_based_on_char(cur, row[pp+1], mask_bit);
		  }

		  or_based_on_char(cur, row[0], 1UL << port_மொத்தம்());

		  தேர்வு (row[port_மொத்தம்()+1]) {
		      நிலை '0':
			levels0_[idx_lev0++] = cur;
			நிறுத்து;
		      நிலை '1':
			levels1_[idx_lev1++] = cur;
			நிறுத்து;
		      நிலை 'x':
			levelsx_[idx_levx++] = cur;
			நிறுத்து;
		      நிலை '-':
			levelsL_[idx_levL++] = cur;
			நிறுத்து;
		      இயல்பு:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_மொத்தம்()+1], row[port_மொத்தம்()+1]);
			ப்ரதிஜ்ஞா(0);
			நிறுத்து;
		  }

	    } இல்லையெனில் {
		  கட்டமைப்பு udp_edges_table cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  cur.edge_position = 0;
		  cur.edge_mask0 = 0;
		  cur.edge_mask1 = 0;
		  cur.edge_maskx = 0;

		  தருக்க need_ext0_table = பொய்;
		  கட்டமைப்பு udp_edges_table ext0;
		  ext0.mask0 = 0;
		  ext0.mask1 = 0;
		  ext0.maskx = 0;
		  ext0.edge_position = 0;
		  ext0.edge_mask0 = 0;
		  ext0.edge_mask1 = 0;
		  ext0.edge_maskx = 0;

		  தருக்க need_ext1_table = பொய்;
		  கட்டமைப்பு udp_edges_table ext1;
		  ext1.mask0 = 0;
		  ext1.mask1 = 0;
		  ext1.maskx = 0;
		  ext1.edge_position = 0;
		  ext1.edge_mask0 = 0;
		  ext1.edge_mask1 = 0;
		  ext1.edge_maskx = 0;

		  க்கு (unsigned pp = 0 ;  pp < port_மொத்தம்() ; pp += 1) {
			தேர்வு (row[pp+1]) {
			    நிலை 'n':
			      edge_based_on_char(cur, 'N', pp);
			      edge_based_on_char(ext0, '_', pp);
			      need_ext0_table = உண்மை;
			      நிறுத்து;
			    நிலை 'p':
			      edge_based_on_char(cur, 'P', pp);
			      edge_based_on_char(ext0, '+', pp);
			      need_ext0_table = உண்மை;
			      நிறுத்து;
			    நிலை '*':
			      edge_based_on_char(cur, 'P', pp);
			      edge_based_on_char(ext0, 'N', pp);
			      edge_based_on_char(ext1, 'B', pp);
			      need_ext0_table = உண்மை;
			      need_ext1_table = உண்மை;
			      நிறுத்து;
			    இயல்பு:
			      edge_based_on_char(cur, row[pp+1], pp);
			      edge_based_on_char(ext0, row[pp+1], pp);
			      edge_based_on_char(ext1, row[pp+1], pp);
			      நிறுத்து;
			}
		  }
		  edge_based_on_char(cur, row[0], port_மொத்தம்());
		  edge_based_on_char(ext0, row[0], port_மொத்தம்());
		  edge_based_on_char(ext1, row[0], port_மொத்தம்());

		  தேர்வு (row[port_மொத்தம்()+1]) {
		      நிலை '0':
			edges0_[idx_edg0++] = cur;
			எனில் (need_ext0_table)
			      edges0_[idx_edg0++] = ext0;
			எனில் (need_ext1_table)
			      edges0_[idx_edg0++] = ext1;
			நிறுத்து;
		      நிலை '1':
			edges1_[idx_edg1++] = cur;
			எனில் (need_ext0_table)
			      edges1_[idx_edg1++] = ext0;
			எனில் (need_ext1_table)
			      edges1_[idx_edg1++] = ext1;
			நிறுத்து;
		      நிலை 'x':
			நிறுத்து;
		      நிலை '-':
			edgesL_[idx_edgL++] = cur;
			எனில் (need_ext0_table)
			      edgesL_[idx_edgL++] = ext0;
			எனில் (need_ext1_table)
			      edgesL_[idx_edgL++] = ext1;
			நிறுத்து;
		      இயல்பு:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_மொத்தம்()+1], row[port_மொத்தம்()+1]);
			ப்ரதிஜ்ஞா(0);
			நிறுத்து;
		  }

	    }
	    delete[] tab[idx];
      }
      விடுவி(tab);

      ப்ரதிஜ்ஞா(idx_edg0 == nedges0_);
      ப்ரதிஜ்ஞா(idx_edg1 == nedges1_);
      ப்ரதிஜ்ஞா(idx_edgL == nedgesL_);

}

தருக்க operator == (மாறிலி udp_levels_table&a, மாறிலி udp_levels_table&b)
{
      எனில் (a.mask0 != b.mask0)
	    திரும்பு பொய்;
      எனில் (a.mask1 != b.mask1)
	    திரும்பு பொய்;
      எனில் (a.maskx != b.maskx)
	    திரும்பு பொய்;
      திரும்பு உண்மை;
}

vvp_bit4_t vvp_udp_seq_s::calculate_output(மாறிலி udp_levels_table&cur,
					   மாறிலி udp_levels_table&prev,
					   vvp_bit4_t cur_out)
{
      எனில் (cur == prev)
	    திரும்பு cur_out;

      udp_levels_table cur_tmp = cur;

      unsigned long mask_out = 1UL << port_மொத்தம்();
      தேர்வு (cur_out) {
	  நிலை BIT4_0:
	    cur_tmp.mask0 |= mask_out;
	    நிறுத்து;
	  நிலை BIT4_1:
	    cur_tmp.mask1 |= mask_out;
	    நிறுத்து;
	  இயல்பு:
	    cur_tmp.maskx |= mask_out;
	    நிறுத்து;
      }

      vvp_bit4_t lev = test_levels_(cur_tmp);
      எனில் (lev == BIT4_Z) {
	    lev = test_edges_(cur_tmp, prev);
      }

      திரும்பு lev;
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
vvp_bit4_t vvp_udp_seq_s::test_levels_(மாறிலி udp_levels_table&cur)
{
      க்கு (unsigned idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    எனில் (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  தொடர்;
	    எனில் (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  தொடர்;
	    எனில் (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  தொடர்;

	    திரும்பு BIT4_0;
      }

      க்கு (unsigned idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    எனில் (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  தொடர்;
	    எனில் (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  தொடர்;
	    எனில் (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  தொடர்;

	    திரும்பு BIT4_1;
      }

	/* We need to test against an explicit X-output table, since
	   we need to distinguish from an X output and no match. */
      க்கு (unsigned idx = 0 ;  idx < nlevelsx_ ;  idx += 1) {
	    எனில் (cur.mask0 != (cur.mask0 & levelsx_[idx].mask0))
		  தொடர்;
	    எனில் (cur.mask1 != (cur.mask1 & levelsx_[idx].mask1))
		  தொடர்;
	    எனில் (cur.maskx != (cur.maskx & levelsx_[idx].maskx))
		  தொடர்;

	    திரும்பு BIT4_X;
      }

	/* Test the table that requests the next output be the same as
	   the current output. This gets the current output from the
	   levels table that was passed in. */
      க்கு (unsigned idx = 0 ;  idx < nlevelsL_ ;  idx += 1) {
	    எனில் (cur.mask0 != (cur.mask0 & levelsL_[idx].mask0))
		  தொடர்;
	    எனில் (cur.mask1 != (cur.mask1 & levelsL_[idx].mask1))
		  தொடர்;
	    எனில் (cur.maskx != (cur.maskx & levelsL_[idx].maskx))
		  தொடர்;

	    எனில் (cur.mask0 & (1 << port_மொத்தம்()))
		  திரும்பு BIT4_0;
	    எனில் (cur.mask1 & (1 << port_மொத்தம்()))
		  திரும்பு BIT4_1;
	    எனில் (cur.maskx & (1 << port_மொத்தம்()))
		  திரும்பு BIT4_X;

	    ப்ரதிஜ்ஞா(0);
	    திரும்பு BIT4_X;
      }

	/* No explicit levels entry match. Return a Z to signal that
	   further testing is needed. */
      திரும்பு BIT4_Z;
}

vvp_bit4_t vvp_udp_seq_s::test_edges_(மாறிலி udp_levels_table&cur,
				      மாறிலி udp_levels_table&prev)
{
	/* The edge_mask is true for all bits that are different in
	   the cur and prev tables. */
      unsigned long edge0_mask = cur.mask0 ^ prev.mask0;
      unsigned long edgex_mask = cur.maskx ^ prev.maskx;
      unsigned long edge1_mask = cur.mask1 ^ prev.mask1;

      unsigned long edge_mask = edge0_mask|edgex_mask|edge1_mask;
      edge_mask &= ~ (-1UL << port_மொத்தம்());

	/* If there are no differences, then there are no edges. Give
	   up now. */
      எனில் (edge_mask == 0)
	    திரும்பு BIT4_X;

      unsigned edge_position = 0;
      போது ((edge_mask&1) == 0) {
	    edge_mask >>= 1;
	    edge_position += 1;
      }

	/* We expect that there is exactly one edge in here. */
      ப்ரதிஜ்ஞா(edge_mask == 1);

      edge_mask = 1UL << edge_position;

      unsigned edge_mask0 = (prev.mask0&edge_mask)? 1 : 0;
      unsigned edge_maskx = (prev.maskx&edge_mask)? 1 : 0;
      unsigned edge_mask1 = (prev.mask1&edge_mask)? 1 : 0;


	/* Now the edge_position and edge_mask* variables have the
	   values we use to test the applicability of the edge_table
	   entries. */

      க்கு (unsigned idx = 0 ;  idx < nedges0_ ;  idx += 1) {
	    மாறிலி கட்டமைப்பு udp_edges_table*row = edges0_ + idx;

	    எனில் (row->edge_position != edge_position)
		  தொடர்;
	    எனில் (edge_mask0 && !row->edge_mask0)
		  தொடர்;
	    எனில் (edge_maskx && !row->edge_maskx)
		  தொடர்;
	    எனில் (edge_mask1 && !row->edge_mask1)
		  தொடர்;
	    எனில் (cur.mask0 != (cur.mask0 & row->mask0))
		  தொடர்;
	    எனில் (cur.maskx != (cur.maskx & row->maskx))
		  தொடர்;
	    எனில் (cur.mask1 != (cur.mask1 & row->mask1))
		  தொடர்;

	    திரும்பு BIT4_0;
      }

      க்கு (unsigned idx = 0 ;  idx < nedges1_ ;  idx += 1) {
	    மாறிலி கட்டமைப்பு udp_edges_table*row = edges1_ + idx;

	    எனில் (row->edge_position != edge_position)
		  தொடர்;
	    எனில் (edge_mask0 && !row->edge_mask0)
		  தொடர்;
	    எனில் (edge_maskx && !row->edge_maskx)
		  தொடர்;
	    எனில் (edge_mask1 && !row->edge_mask1)
		  தொடர்;
	    எனில் (cur.mask0 != (cur.mask0 & row->mask0))
		  தொடர்;
	    எனில் (cur.maskx != (cur.maskx & row->maskx))
		  தொடர்;
	    எனில் (cur.mask1 != (cur.mask1 & row->mask1))
		  தொடர்;

	    திரும்பு BIT4_1;
      }

      க்கு (unsigned idx = 0 ;  idx < nedgesL_ ;  idx += 1) {
	    மாறிலி கட்டமைப்பு udp_edges_table*row = edgesL_ + idx;

	    எனில் (row->edge_position != edge_position)
		  தொடர்;
	    எனில் (edge_mask0 && !row->edge_mask0)
		  தொடர்;
	    எனில் (edge_maskx && !row->edge_maskx)
		  தொடர்;
	    எனில் (edge_mask1 && !row->edge_mask1)
		  தொடர்;
	    எனில் (cur.mask0 != (cur.mask0 & row->mask0))
		  தொடர்;
	    எனில் (cur.maskx != (cur.maskx & row->maskx))
		  தொடர்;
	    எனில் (cur.mask1 != (cur.mask1 & row->mask1))
		  தொடர்;

	    எனில் (cur.mask0 & (1 << port_மொத்தம்()))
		  திரும்பு BIT4_0;
	    எனில் (cur.mask1 & (1 << port_மொத்தம்()))
		  திரும்பு BIT4_1;
	    எனில் (cur.maskx & (1 << port_மொத்தம்()))
		  திரும்பு BIT4_X;

	    ப்ரதிஜ்ஞா(0);
	    திரும்பு BIT4_X;
      }

      திரும்பு BIT4_X;
}

vvp_udp_fun_core::vvp_udp_fun_core(vvp_net_t*net, vvp_udp_s*def)
: vvp_wide_fun_core(net, def->port_மொத்தம்())
{
      def_ = def;
      cur_out_ = def_->get_init();
	// Assume initially that all the inputs are 1'bx
      current_.mask0 = 0;
      current_.mask1 = 0;
      current_.maskx = ~ ((-1UL) << port_மொத்தம்());

        // If the initial value is 0 or 1, schedule the initial assignment
        // normally, so that any sensitive always processes can be started
        // first.
      எனில் (cur_out_ != BIT4_X)
	    schedule_generic(இது, 0, பொய்);
      இல்லையெனில்
	    schedule_functor(இது);
}

vvp_udp_fun_core::~vvp_udp_fun_core()
{
}

/*
 * This method is used to propagate the initial value on startup.
 */
வெறுமை vvp_udp_fun_core::run_run()
{
      vvp_vector4_t tmp (1);
      tmp.set_bit(0, cur_out_);
      propagate_vec4(tmp);
}

வெறுமை vvp_udp_fun_core::recv_vec4_from_inputs(unsigned port)
{
	/* For now, assume udps are 1-bit wide. */
      unsigned port_width = value(port).size();
      ப்ரதிஜ்ஞா(port_width == 1);

      unsigned long mask = 1UL << port;

      udp_levels_table prev = current_;

      தேர்வு (value(port).value(0)) {

	  நிலை BIT4_0:
	    current_.mask0 |= mask;
	    current_.mask1 &= ~mask;
	    current_.maskx &= ~mask;
	    நிறுத்து;
	  நிலை BIT4_1:
	    current_.mask0 &= ~mask;
	    current_.mask1 |= mask;
	    current_.maskx &= ~mask;
	    நிறுத்து;
	  இயல்பு:
	    current_.mask0 &= ~mask;
	    current_.mask1 &= ~mask;
	    current_.maskx |= mask;
	    நிறுத்து;
      }

      vvp_bit4_t out_bit = def_->calculate_output(current_, prev, cur_out_);

      எனில் (out_bit == cur_out_)
	    திரும்பு;

      cur_out_ = out_bit;
      schedule_functor(இது);
}


/*
 * This function is called by the parser in response to a .udp
 * node. We create the nodes needed to integrate the UDP into the
 * netlist. The definition should be parsed already.
 */
வெறுமை compile_udp_functor(எழுத்து*label, எழுத்து*type,
			 unsigned argc, கட்டமைப்பு symb_s*argv)
{
      கட்டமைப்பு vvp_udp_s *def = udp_find(type);
      ப்ரதிஜ்ஞா(def);
      விடுவி(type);

      vvp_net_t*ptr = புதிய vvp_net_t;
      vvp_udp_fun_core*core = புதிய vvp_udp_fun_core(ptr, def);
      ptr->fun = core;

      define_functor_குறியீடு(label, ptr);
      விடுவி(label);

      wide_inputs_connect(core, argc, argv);
      விடுவி(argv);
}
