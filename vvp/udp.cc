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

#অন্তর্ভাব "udp.h"
#অন্তর্ভাব "schedule.h"
#অন্তর্ভাব "symbols.h"
#অন্তর্ভাব "compile.h"
#অন্তর্ভাব "config.h"
#অন্তর্ভাব <cassert>
#অন্তর্ভাব <cstdlib>
#অন্তর্ভাব <cstring>
#অন্তর্ভাব <cstdio>
#অন্তর্ভাব <iostream>
#ifdef CHECK_WITH_VALGRIND
#অন্তর্ভাব "vvp_cleanup.h"
#অন্তর্ভাব "ivl_alloc.h"
#endif

using নামাকাশ std;

// We may need these later when we build the VPI interface to
// UDP definitions.
#ifdef CHECK_WITH_VALGRIND
স্থির vvp_udp_s **udp_defns = 0;
স্থির unsigned udp_defns_গুনতি = 0;

শূন্য udp_defns_delete()
{
      জন্য (unsigned idx = 0; idx < udp_defns_গুনতি; idx += 1) {
	    যদি (udp_defns[idx]->is_sequential()) {
		  vvp_udp_seq_s *obj = static_cast<vvp_udp_seq_s *>
		                       (udp_defns[idx]);
		  delete obj;
	    } অন্যথা {
		  vvp_udp_comb_s *obj = static_cast<vvp_udp_comb_s *>
		                        (udp_defns[idx]);
		  delete obj;
	    }
      }
      মুক্ত(udp_defns);
      udp_defns = 0;
      udp_defns_গুনতি = 0;
}
#endif

স্থির প্রতীক_table_t udp_table;

শূন্য delete_udp_symbols()
{
      delete_প্রতীক_table(udp_table);
      udp_table = 0;
}

কাঠামো vvp_udp_s *udp_find(ধ্রুবক অক্ষর *label)
{
      প্রতীক_মান_t v = sym_get_মান(udp_table, label);
      ফেরত (কাঠামো vvp_udp_s *)v.ptr;
}

ostream& operator <<(ostream&o, ধ্রুবক কাঠামো udp_levels_table&table)
{
      o << "[" << hex << table.mask0
	<< "/" << hex << table.mask1
	<< "/" << hex << table.maskx << "]";
      ফেরত o;
}

vvp_udp_s::vvp_udp_s(ধ্রুবক অক্ষর*label, অক্ষর*name__, unsigned ports,
                     vvp_bit4_t init, যৌক্তিক type)
: name_(name__), ports_(ports), init_(init), seq_(type)
{
      যদি (!udp_table)
	    udp_table = new_প্রতীক_table();

      প্রতিজ্ঞা( !udp_find(label) );

      প্রতীক_মান_t v;
      v.ptr = এই;
      sym_set_মান(udp_table, label, v);

#ifdef CHECK_WITH_VALGRIND
      udp_defns_গুনতি += 1;
      udp_defns = static_cast<vvp_udp_s **>(realloc(udp_defns,
                  udp_defns_গুনতি*আকার(vvp_udp_s **)));
      udp_defns[udp_defns_গুনতি-1] = এই;
#endif
}

vvp_udp_s::~vvp_udp_s()
{
      delete[] name_;
}

unsigned vvp_udp_s::port_গুনতি() ধ্রুবক
{
      ফেরত ports_;
}

vvp_bit4_t vvp_udp_s::get_init() ধ্রুবক
{
      ফেরত init_;
}

vvp_udp_comb_s::vvp_udp_comb_s(ধ্রুবক অক্ষর*label, অক্ষর*name__, unsigned ports)
: vvp_udp_s(label, name__, ports, BIT4_X, মিথ্যা)
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
vvp_bit4_t vvp_udp_comb_s::test_levels(ধ্রুবক udp_levels_table&cur)
{
	/* To test for a row match, test that the mask0, mask1 and
	   maskx vectors all have bits set where the matching
	   cur.mask0/1/x vectors have bits set. It is possible that a
	   levels0_[idx] vector has more bits set then the cur mask,
	   but this is OK and these bits are to be ignored. */

      জন্য (unsigned idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    যদি (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  চালু;
	    যদি (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  চালু;
	    যদি (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  চালু;

	    ফেরত BIT4_0;
      }

      জন্য (unsigned idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    যদি (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  চালু;
	    যদি (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  চালু;
	    যদি (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  চালু;

	    ফেরত BIT4_1;
      }

      ফেরত BIT4_X;
}

vvp_bit4_t vvp_udp_comb_s::calculate_output(ধ্রুবক udp_levels_table&cur,
					    ধ্রুবক udp_levels_table&,
					    vvp_bit4_t)
{
      ফেরত test_levels(cur);
}

স্থির শূন্য or_based_on_char(udp_levels_table&cur, অক্ষর flag,
			     unsigned long mask_bit)
{
      নির্বাচন (flag) {
	  ক্ষেত্র '0':
	    cur.mask0 |= mask_bit;
	    বিরতি;
	  ক্ষেত্র '1':
	    cur.mask1 |= mask_bit;
	    বিরতি;
	  ক্ষেত্র 'x':
	    cur.maskx |= mask_bit;
	    বিরতি;
	  ক্ষেত্র 'b':
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    বিরতি;
	  ক্ষেত্র 'l':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    বিরতি;
	  ক্ষেত্র 'h':
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    বিরতি;
	  ক্ষেত্র '?':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    বিরতি;
	  পূর্বনির্ধারিত:
	    fprintf(stderr, "Unsupported flag %c(%d).\n", flag, flag);
	    প্রতিজ্ঞা(0);
      }
}

শূন্য vvp_udp_comb_s::compile_table(অক্ষর**tab)
{
      unsigned nrows0 = 0, nrows1 = 0;

	/* First run through the table to figure out the number of
	   rows I need for each kind of table. */
      জন্য (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    প্রতিজ্ঞা(strlen(tab[idx]) == port_গুনতি() + 1);
	    নির্বাচন (tab[idx][port_গুনতি()]) {
		ক্ষেত্র '0':
		  nrows0 += 1;
		  বিরতি;
		ক্ষেত্র '1':
		  nrows1 += 1;
		  বিরতি;
		ক্ষেত্র 'x':
		  বিরতি;
		পূর্বনির্ধারিত:
		  fprintf(stderr, "Unsupported entry %c(%d).\n",
		          tab[idx][port_গুনতি()], tab[idx][port_গুনতি()]);
		  প্রতিজ্ঞা(0);
	    }
      }

      nlevels0_ = nrows0;
      levels0_ = নতুন udp_levels_table[nlevels0_];

      nlevels1_ = nrows1;
      levels1_ = নতুন udp_levels_table[nlevels1_];

      nrows0 = 0;
      nrows1 = 0;
      জন্য (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    কাঠামো udp_levels_table cur;
	    cur.mask0 = 0;
	    cur.mask1 = 0;
	    cur.maskx = 0;
	    যদি (port_গুনতি() > 8*আকার(cur.mask0)) {
		  fprintf(stderr, "internal error: primitive port count=%u "
			  " > %zu\n", port_গুনতি(), আকার(cur.mask0));
		  প্রতিজ্ঞা(port_গুনতি() <= 8*আকার(cur.mask0));
	    }
	    জন্য (unsigned pp = 0 ;  pp < port_গুনতি() ;  pp += 1) {
		  unsigned long mask_bit = 1UL << pp;
		  or_based_on_char(cur, tab[idx][pp], mask_bit);
	    }

	    নির্বাচন (tab[idx][port_গুনতি()]) {
		ক্ষেত্র '0':
		  levels0_[nrows0++] = cur;
		  বিরতি;
		ক্ষেত্র '1':
		  levels1_[nrows1++] = cur;
		  বিরতি;
		পূর্বনির্ধারিত:
		  বিরতি;
	    }
	    delete[] tab[idx];
      }
      মুক্ত(tab);

      প্রতিজ্ঞা(nrows0 == nlevels0_);
      প্রতিজ্ঞা(nrows1 == nlevels1_);
}

vvp_udp_seq_s::vvp_udp_seq_s(ধ্রুবক অক্ষর*label, অক্ষর*name__,
			     unsigned ports, vvp_bit4_t init)
: vvp_udp_s(label, name__, ports, init, সত্য)
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

শূন্য edge_based_on_char(কাঠামো udp_edges_table&cur, অক্ষর chr, unsigned pos)
{
      unsigned long mask_bit = 1UL << pos;

      নির্বাচন (chr) {
	  ক্ষেত্র '0':
	    cur.mask0 |= mask_bit;
	    বিরতি;
	  ক্ষেত্র '1':
	    cur.mask1 |= mask_bit;
	    বিরতি;
	  ক্ষেত্র 'x':
	    cur.maskx |= mask_bit;
	    বিরতি;
	  ক্ষেত্র 'b':
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    বিরতি;
	  ক্ষেত্র 'l':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    বিরতি;
	  ক্ষেত্র 'h':
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    বিরতি;
	  ক্ষেত্র '?':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    বিরতি;

	  ক্ষেত্র 'B': // (x?) edge
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    বিরতি;
	  ক্ষেত্র 'f': // (10) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    বিরতি;
	  ক্ষেত্র 'F': // (x0) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    বিরতি;
	  ক্ষেত্র 'M': // (1x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    বিরতি;
	  ক্ষেত্র 'N': // (1x) and (10) edge
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    বিরতি;
	  ক্ষেত্র 'P': // (0x) and (01) edge
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    বিরতি;
	  ক্ষেত্র 'q': // (bx) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    বিরতি;
	  ক্ষেত্র 'Q': // (0x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    বিরতি;
	  ক্ষেত্র 'r': // (01) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    বিরতি;
	  ক্ষেত্র 'R': // (x1) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    বিরতি;
	  ক্ষেত্র '%': // (?x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    বিরতি;
	  ক্ষেত্র '+': // (?1) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    বিরতি;
	  ক্ষেত্র '_': // (?0) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 1;
	    বিরতি;
	  পূর্বনির্ধারিত:
	    fprintf(stderr, "internal error: unknown edge code: %c\n", chr);
	    প্রতিজ্ঞা(0);
      }
}

শূন্য vvp_udp_seq_s::compile_table(অক্ষর**tab)
{

      জন্য (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    ধ্রুবক অক্ষর*row = tab[idx];
	    প্রতিজ্ঞা(strlen(row) == port_গুনতি() + 2);

	    যদি (strspn(row, "01xblh?") >= port_গুনতি()+1) {

		  নির্বাচন (row[port_গুনতি()+1]) {
		      ক্ষেত্র '0':
			nlevels0_ += 1;
			বিরতি;
		      ক্ষেত্র '1':
			nlevels1_ += 1;
			বিরতি;
		      ক্ষেত্র 'x':
			nlevelsx_ += 1;
			বিরতি;
		      ক্ষেত্র '-':
			nlevelsL_ += 1;
			বিরতি;
		      পূর্বনির্ধারিত:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_গুনতি()+1], row[port_গুনতি()+1]);
			প্রতিজ্ঞা(0);
			বিরতি;
		  }

	    } অন্যথা {
		    /* Rows that have n or p edges will need to be
		       expanded into 2 rows. */
		  unsigned extra = 0;
		  যদি (strchr(row,'n'))
			extra = 1;
		  যদি (strchr(row,'p'))
			extra = 1;
		  যদি (strchr(row,'*'))
			extra = 2;

		  নির্বাচন (row[port_গুনতি()+1]) {
		      ক্ষেত্র '0':
			nedges0_ += 1 + extra;
			বিরতি;
		      ক্ষেত্র '1':
			nedges1_ += 1 + extra;
			বিরতি;
		      ক্ষেত্র 'x':
			বিরতি;
		      ক্ষেত্র '-':
			nedgesL_ += 1 + extra;
			বিরতি;
		      পূর্বনির্ধারিত:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_গুনতি()+1], row[port_গুনতি()+1]);
			প্রতিজ্ঞা(0);
			বিরতি;
		  }
	    }
      }

      levels0_ = নতুন udp_levels_table[nlevels0_];
      levels1_ = নতুন udp_levels_table[nlevels1_];
      levelsx_ = নতুন udp_levels_table[nlevelsx_];
      levelsL_ = নতুন udp_levels_table[nlevelsL_];
      edges0_ = নতুন udp_edges_table[nedges0_];
      edges1_ = নতুন udp_edges_table[nedges1_];
      edgesL_ = নতুন udp_edges_table[nedgesL_];

      unsigned idx_lev0 = 0;
      unsigned idx_lev1 = 0;
      unsigned idx_levx = 0;
      unsigned idx_levL = 0;
      unsigned idx_edg0 = 0;
      unsigned idx_edg1 = 0;
      unsigned idx_edgL = 0;

      জন্য (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    ধ্রুবক অক্ষর*row = tab[idx];

	    যদি (strspn(row, "01xblh?") >= port_গুনতি()+1) {
		  কাঠামো udp_levels_table cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  জন্য (unsigned pp = 0 ;  pp < port_গুনতি() ;  pp += 1) {
			unsigned long mask_bit = 1UL << pp;
			or_based_on_char(cur, row[pp+1], mask_bit);
		  }

		  or_based_on_char(cur, row[0], 1UL << port_গুনতি());

		  নির্বাচন (row[port_গুনতি()+1]) {
		      ক্ষেত্র '0':
			levels0_[idx_lev0++] = cur;
			বিরতি;
		      ক্ষেত্র '1':
			levels1_[idx_lev1++] = cur;
			বিরতি;
		      ক্ষেত্র 'x':
			levelsx_[idx_levx++] = cur;
			বিরতি;
		      ক্ষেত্র '-':
			levelsL_[idx_levL++] = cur;
			বিরতি;
		      পূর্বনির্ধারিত:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_গুনতি()+1], row[port_গুনতি()+1]);
			প্রতিজ্ঞা(0);
			বিরতি;
		  }

	    } অন্যথা {
		  কাঠামো udp_edges_table cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  cur.edge_position = 0;
		  cur.edge_mask0 = 0;
		  cur.edge_mask1 = 0;
		  cur.edge_maskx = 0;

		  যৌক্তিক need_ext0_table = মিথ্যা;
		  কাঠামো udp_edges_table ext0;
		  ext0.mask0 = 0;
		  ext0.mask1 = 0;
		  ext0.maskx = 0;
		  ext0.edge_position = 0;
		  ext0.edge_mask0 = 0;
		  ext0.edge_mask1 = 0;
		  ext0.edge_maskx = 0;

		  যৌক্তিক need_ext1_table = মিথ্যা;
		  কাঠামো udp_edges_table ext1;
		  ext1.mask0 = 0;
		  ext1.mask1 = 0;
		  ext1.maskx = 0;
		  ext1.edge_position = 0;
		  ext1.edge_mask0 = 0;
		  ext1.edge_mask1 = 0;
		  ext1.edge_maskx = 0;

		  জন্য (unsigned pp = 0 ;  pp < port_গুনতি() ; pp += 1) {
			নির্বাচন (row[pp+1]) {
			    ক্ষেত্র 'n':
			      edge_based_on_char(cur, 'N', pp);
			      edge_based_on_char(ext0, '_', pp);
			      need_ext0_table = সত্য;
			      বিরতি;
			    ক্ষেত্র 'p':
			      edge_based_on_char(cur, 'P', pp);
			      edge_based_on_char(ext0, '+', pp);
			      need_ext0_table = সত্য;
			      বিরতি;
			    ক্ষেত্র '*':
			      edge_based_on_char(cur, 'P', pp);
			      edge_based_on_char(ext0, 'N', pp);
			      edge_based_on_char(ext1, 'B', pp);
			      need_ext0_table = সত্য;
			      need_ext1_table = সত্য;
			      বিরতি;
			    পূর্বনির্ধারিত:
			      edge_based_on_char(cur, row[pp+1], pp);
			      edge_based_on_char(ext0, row[pp+1], pp);
			      edge_based_on_char(ext1, row[pp+1], pp);
			      বিরতি;
			}
		  }
		  edge_based_on_char(cur, row[0], port_গুনতি());
		  edge_based_on_char(ext0, row[0], port_গুনতি());
		  edge_based_on_char(ext1, row[0], port_গুনতি());

		  নির্বাচন (row[port_গুনতি()+1]) {
		      ক্ষেত্র '0':
			edges0_[idx_edg0++] = cur;
			যদি (need_ext0_table)
			      edges0_[idx_edg0++] = ext0;
			যদি (need_ext1_table)
			      edges0_[idx_edg0++] = ext1;
			বিরতি;
		      ক্ষেত্র '1':
			edges1_[idx_edg1++] = cur;
			যদি (need_ext0_table)
			      edges1_[idx_edg1++] = ext0;
			যদি (need_ext1_table)
			      edges1_[idx_edg1++] = ext1;
			বিরতি;
		      ক্ষেত্র 'x':
			বিরতি;
		      ক্ষেত্র '-':
			edgesL_[idx_edgL++] = cur;
			যদি (need_ext0_table)
			      edgesL_[idx_edgL++] = ext0;
			যদি (need_ext1_table)
			      edgesL_[idx_edgL++] = ext1;
			বিরতি;
		      পূর্বনির্ধারিত:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_গুনতি()+1], row[port_গুনতি()+1]);
			প্রতিজ্ঞা(0);
			বিরতি;
		  }

	    }
	    delete[] tab[idx];
      }
      মুক্ত(tab);

      প্রতিজ্ঞা(idx_edg0 == nedges0_);
      প্রতিজ্ঞা(idx_edg1 == nedges1_);
      প্রতিজ্ঞা(idx_edgL == nedgesL_);

}

যৌক্তিক operator == (ধ্রুবক udp_levels_table&a, ধ্রুবক udp_levels_table&b)
{
      যদি (a.mask0 != b.mask0)
	    ফেরত মিথ্যা;
      যদি (a.mask1 != b.mask1)
	    ফেরত মিথ্যা;
      যদি (a.maskx != b.maskx)
	    ফেরত মিথ্যা;
      ফেরত সত্য;
}

vvp_bit4_t vvp_udp_seq_s::calculate_output(ধ্রুবক udp_levels_table&cur,
					   ধ্রুবক udp_levels_table&prev,
					   vvp_bit4_t cur_out)
{
      যদি (cur == prev)
	    ফেরত cur_out;

      udp_levels_table cur_tmp = cur;

      unsigned long mask_out = 1UL << port_গুনতি();
      নির্বাচন (cur_out) {
	  ক্ষেত্র BIT4_0:
	    cur_tmp.mask0 |= mask_out;
	    বিরতি;
	  ক্ষেত্র BIT4_1:
	    cur_tmp.mask1 |= mask_out;
	    বিরতি;
	  পূর্বনির্ধারিত:
	    cur_tmp.maskx |= mask_out;
	    বিরতি;
      }

      vvp_bit4_t lev = test_levels_(cur_tmp);
      যদি (lev == BIT4_Z) {
	    lev = test_edges_(cur_tmp, prev);
      }

      ফেরত lev;
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
vvp_bit4_t vvp_udp_seq_s::test_levels_(ধ্রুবক udp_levels_table&cur)
{
      জন্য (unsigned idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    যদি (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  চালু;
	    যদি (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  চালু;
	    যদি (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  চালু;

	    ফেরত BIT4_0;
      }

      জন্য (unsigned idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    যদি (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  চালু;
	    যদি (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  চালু;
	    যদি (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  চালু;

	    ফেরত BIT4_1;
      }

	/* We need to test against an explicit X-output table, since
	   we need to distinguish from an X output and no match. */
      জন্য (unsigned idx = 0 ;  idx < nlevelsx_ ;  idx += 1) {
	    যদি (cur.mask0 != (cur.mask0 & levelsx_[idx].mask0))
		  চালু;
	    যদি (cur.mask1 != (cur.mask1 & levelsx_[idx].mask1))
		  চালু;
	    যদি (cur.maskx != (cur.maskx & levelsx_[idx].maskx))
		  চালু;

	    ফেরত BIT4_X;
      }

	/* Test the table that requests the next output be the same as
	   the current output. This gets the current output from the
	   levels table that was passed in. */
      জন্য (unsigned idx = 0 ;  idx < nlevelsL_ ;  idx += 1) {
	    যদি (cur.mask0 != (cur.mask0 & levelsL_[idx].mask0))
		  চালু;
	    যদি (cur.mask1 != (cur.mask1 & levelsL_[idx].mask1))
		  চালু;
	    যদি (cur.maskx != (cur.maskx & levelsL_[idx].maskx))
		  চালু;

	    যদি (cur.mask0 & (1 << port_গুনতি()))
		  ফেরত BIT4_0;
	    যদি (cur.mask1 & (1 << port_গুনতি()))
		  ফেরত BIT4_1;
	    যদি (cur.maskx & (1 << port_গুনতি()))
		  ফেরত BIT4_X;

	    প্রতিজ্ঞা(0);
	    ফেরত BIT4_X;
      }

	/* No explicit levels entry match. Return a Z to signal that
	   further testing is needed. */
      ফেরত BIT4_Z;
}

vvp_bit4_t vvp_udp_seq_s::test_edges_(ধ্রুবক udp_levels_table&cur,
				      ধ্রুবক udp_levels_table&prev)
{
	/* The edge_mask is true for all bits that are different in
	   the cur and prev tables. */
      unsigned long edge0_mask = cur.mask0 ^ prev.mask0;
      unsigned long edgex_mask = cur.maskx ^ prev.maskx;
      unsigned long edge1_mask = cur.mask1 ^ prev.mask1;

      unsigned long edge_mask = edge0_mask|edgex_mask|edge1_mask;
      edge_mask &= ~ (-1UL << port_গুনতি());

	/* If there are no differences, then there are no edges. Give
	   up now. */
      যদি (edge_mask == 0)
	    ফেরত BIT4_X;

      unsigned edge_position = 0;
      যখন ((edge_mask&1) == 0) {
	    edge_mask >>= 1;
	    edge_position += 1;
      }

	/* We expect that there is exactly one edge in here. */
      প্রতিজ্ঞা(edge_mask == 1);

      edge_mask = 1UL << edge_position;

      unsigned edge_mask0 = (prev.mask0&edge_mask)? 1 : 0;
      unsigned edge_maskx = (prev.maskx&edge_mask)? 1 : 0;
      unsigned edge_mask1 = (prev.mask1&edge_mask)? 1 : 0;


	/* Now the edge_position and edge_mask* variables have the
	   values we use to test the applicability of the edge_table
	   entries. */

      জন্য (unsigned idx = 0 ;  idx < nedges0_ ;  idx += 1) {
	    ধ্রুবক কাঠামো udp_edges_table*row = edges0_ + idx;

	    যদি (row->edge_position != edge_position)
		  চালু;
	    যদি (edge_mask0 && !row->edge_mask0)
		  চালু;
	    যদি (edge_maskx && !row->edge_maskx)
		  চালু;
	    যদি (edge_mask1 && !row->edge_mask1)
		  চালু;
	    যদি (cur.mask0 != (cur.mask0 & row->mask0))
		  চালু;
	    যদি (cur.maskx != (cur.maskx & row->maskx))
		  চালু;
	    যদি (cur.mask1 != (cur.mask1 & row->mask1))
		  চালু;

	    ফেরত BIT4_0;
      }

      জন্য (unsigned idx = 0 ;  idx < nedges1_ ;  idx += 1) {
	    ধ্রুবক কাঠামো udp_edges_table*row = edges1_ + idx;

	    যদি (row->edge_position != edge_position)
		  চালু;
	    যদি (edge_mask0 && !row->edge_mask0)
		  চালু;
	    যদি (edge_maskx && !row->edge_maskx)
		  চালু;
	    যদি (edge_mask1 && !row->edge_mask1)
		  চালু;
	    যদি (cur.mask0 != (cur.mask0 & row->mask0))
		  চালু;
	    যদি (cur.maskx != (cur.maskx & row->maskx))
		  চালু;
	    যদি (cur.mask1 != (cur.mask1 & row->mask1))
		  চালু;

	    ফেরত BIT4_1;
      }

      জন্য (unsigned idx = 0 ;  idx < nedgesL_ ;  idx += 1) {
	    ধ্রুবক কাঠামো udp_edges_table*row = edgesL_ + idx;

	    যদি (row->edge_position != edge_position)
		  চালু;
	    যদি (edge_mask0 && !row->edge_mask0)
		  চালু;
	    যদি (edge_maskx && !row->edge_maskx)
		  চালু;
	    যদি (edge_mask1 && !row->edge_mask1)
		  চালু;
	    যদি (cur.mask0 != (cur.mask0 & row->mask0))
		  চালু;
	    যদি (cur.maskx != (cur.maskx & row->maskx))
		  চালু;
	    যদি (cur.mask1 != (cur.mask1 & row->mask1))
		  চালু;

	    যদি (cur.mask0 & (1 << port_গুনতি()))
		  ফেরত BIT4_0;
	    যদি (cur.mask1 & (1 << port_গুনতি()))
		  ফেরত BIT4_1;
	    যদি (cur.maskx & (1 << port_গুনতি()))
		  ফেরত BIT4_X;

	    প্রতিজ্ঞা(0);
	    ফেরত BIT4_X;
      }

      ফেরত BIT4_X;
}

vvp_udp_fun_core::vvp_udp_fun_core(vvp_net_t*net, vvp_udp_s*def)
: vvp_wide_fun_core(net, def->port_গুনতি())
{
      def_ = def;
      cur_out_ = def_->get_init();
	// Assume initially that all the inputs are 1'bx
      current_.mask0 = 0;
      current_.mask1 = 0;
      current_.maskx = ~ ((-1UL) << port_গুনতি());

        // If the initial value is 0 or 1, schedule the initial assignment
        // normally, so that any sensitive always processes can be started
        // first.
      যদি (cur_out_ != BIT4_X)
	    schedule_generic(এই, 0, মিথ্যা);
      অন্যথা
	    schedule_functor(এই);
}

vvp_udp_fun_core::~vvp_udp_fun_core()
{
}

/*
 * This method is used to propagate the initial value on startup.
 */
শূন্য vvp_udp_fun_core::run_run()
{
      vvp_vector4_t tmp (1);
      tmp.set_bit(0, cur_out_);
      propagate_vec4(tmp);
}

শূন্য vvp_udp_fun_core::recv_vec4_from_inputs(unsigned port)
{
	/* For now, assume udps are 1-bit wide. */
      unsigned port_width = value(port).size();
      প্রতিজ্ঞা(port_width == 1);

      unsigned long mask = 1UL << port;

      udp_levels_table prev = current_;

      নির্বাচন (value(port).value(0)) {

	  ক্ষেত্র BIT4_0:
	    current_.mask0 |= mask;
	    current_.mask1 &= ~mask;
	    current_.maskx &= ~mask;
	    বিরতি;
	  ক্ষেত্র BIT4_1:
	    current_.mask0 &= ~mask;
	    current_.mask1 |= mask;
	    current_.maskx &= ~mask;
	    বিরতি;
	  পূর্বনির্ধারিত:
	    current_.mask0 &= ~mask;
	    current_.mask1 &= ~mask;
	    current_.maskx |= mask;
	    বিরতি;
      }

      vvp_bit4_t out_bit = def_->calculate_output(current_, prev, cur_out_);

      যদি (out_bit == cur_out_)
	    ফেরত;

      cur_out_ = out_bit;
      schedule_functor(এই);
}


/*
 * This function is called by the parser in response to a .udp
 * node. We create the nodes needed to integrate the UDP into the
 * netlist. The definition should be parsed already.
 */
শূন্য compile_udp_functor(অক্ষর*label, অক্ষর*type,
			 unsigned argc, কাঠামো symb_s*argv)
{
      কাঠামো vvp_udp_s *def = udp_find(type);
      প্রতিজ্ঞা(def);
      মুক্ত(type);

      vvp_net_t*ptr = নতুন vvp_net_t;
      vvp_udp_fun_core*core = নতুন vvp_udp_fun_core(ptr, def);
      ptr->fun = core;

      define_functor_প্রতীক(label, ptr);
      মুক্ত(label);

      wide_inputs_connect(core, argc, argv);
      মুক্ত(argv);
}
