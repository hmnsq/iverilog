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

#अन्तर्भाव "udp.h"
#अन्तर्भाव "schedule.h"
#अन्तर्भाव "symbols.h"
#अन्तर्भाव "compile.h"
#अन्तर्भाव "config.h"
#अन्तर्भाव <cassert>
#अन्तर्भाव <cstdlib>
#अन्तर्भाव <cstring>
#अन्तर्भाव <cstdio>
#अन्तर्भाव <iostream>
#ifdef CHECK_WITH_VALGRIND
#अन्तर्भाव "vvp_cleanup.h"
#अन्तर्भाव "ivl_alloc.h"
#endif

using नामाकाश std;

// We may need these later when we build the VPI interface to
// UDP definitions.
#ifdef CHECK_WITH_VALGRIND
स्थिर vvp_udp_s **udp_defns = 0;
स्थिर unsigned udp_defns_गणनम् = 0;

शून्य udp_defns_delete()
{
      प्रति (unsigned idx = 0; idx < udp_defns_गणनम्; idx += 1) {
	    जइ (udp_defns[idx]->is_sequential()) {
		  vvp_udp_seq_s *obj = static_cast<vvp_udp_seq_s *>
		                       (udp_defns[idx]);
		  delete obj;
	    } अण्णहा {
		  vvp_udp_comb_s *obj = static_cast<vvp_udp_comb_s *>
		                        (udp_defns[idx]);
		  delete obj;
	    }
      }
      मुक्त(udp_defns);
      udp_defns = 0;
      udp_defns_गणनम् = 0;
}
#endif

स्थिर चिह्नम्_table_t udp_table;

शून्य delete_udp_symbols()
{
      delete_चिह्नम्_table(udp_table);
      udp_table = 0;
}

संरचना vvp_udp_s *udp_find(ध्रुव वर्ण *label)
{
      चिह्नम्_मानम्_t v = sym_get_मानम्(udp_table, label);
      प्रत्यागम (संरचना vvp_udp_s *)v.ptr;
}

ostream& operator <<(ostream&o, ध्रुव संरचना udp_levels_table&table)
{
      o << "[" << hex << table.mask0
	<< "/" << hex << table.mask1
	<< "/" << hex << table.maskx << "]";
      प्रत्यागम o;
}

vvp_udp_s::vvp_udp_s(ध्रुव वर्ण*label, वर्ण*name__, unsigned ports,
                     vvp_bit4_t init, तार्किक type)
: name_(name__), ports_(ports), init_(init), seq_(type)
{
      जइ (!udp_table)
	    udp_table = new_चिह्नम्_table();

      प्रतिज्ञा( !udp_find(label) );

      चिह्नम्_मानम्_t v;
      v.ptr = एतत्;
      sym_set_मानम्(udp_table, label, v);

#ifdef CHECK_WITH_VALGRIND
      udp_defns_गणनम् += 1;
      udp_defns = static_cast<vvp_udp_s **>(realloc(udp_defns,
                  udp_defns_गणनम्*परिमाण(vvp_udp_s **)));
      udp_defns[udp_defns_गणनम्-1] = एतत्;
#endif
}

vvp_udp_s::~vvp_udp_s()
{
      delete[] name_;
}

unsigned vvp_udp_s::port_गणनम्() ध्रुव
{
      प्रत्यागम ports_;
}

vvp_bit4_t vvp_udp_s::get_init() ध्रुव
{
      प्रत्यागम init_;
}

vvp_udp_comb_s::vvp_udp_comb_s(ध्रुव वर्ण*label, वर्ण*name__, unsigned ports)
: vvp_udp_s(label, name__, ports, BIT4_X, मिच्छ)
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
vvp_bit4_t vvp_udp_comb_s::test_levels(ध्रुव udp_levels_table&cur)
{
	/* To test for a row match, test that the mask0, mask1 and
	   maskx vectors all have bits set where the matching
	   cur.mask0/1/x vectors have bits set. It is possible that a
	   levels0_[idx] vector has more bits set then the cur mask,
	   but this is OK and these bits are to be ignored. */

      प्रति (unsigned idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    जइ (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  अनुवर्त;
	    जइ (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  अनुवर्त;
	    जइ (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  अनुवर्त;

	    प्रत्यागम BIT4_0;
      }

      प्रति (unsigned idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    जइ (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  अनुवर्त;
	    जइ (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  अनुवर्त;
	    जइ (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  अनुवर्त;

	    प्रत्यागम BIT4_1;
      }

      प्रत्यागम BIT4_X;
}

vvp_bit4_t vvp_udp_comb_s::calculate_output(ध्रुव udp_levels_table&cur,
					    ध्रुव udp_levels_table&,
					    vvp_bit4_t)
{
      प्रत्यागम test_levels(cur);
}

स्थिर शून्य or_based_on_char(udp_levels_table&cur, वर्ण flag,
			     unsigned long mask_bit)
{
      विकल्प (flag) {
	  पक्ष '0':
	    cur.mask0 |= mask_bit;
	    भङ्ग;
	  पक्ष '1':
	    cur.mask1 |= mask_bit;
	    भङ्ग;
	  पक्ष 'x':
	    cur.maskx |= mask_bit;
	    भङ्ग;
	  पक्ष 'b':
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    भङ्ग;
	  पक्ष 'l':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    भङ्ग;
	  पक्ष 'h':
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    भङ्ग;
	  पक्ष '?':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    भङ्ग;
	  स्वभाव:
	    fprintf(stderr, "Unsupported flag %c(%d).\n", flag, flag);
	    प्रतिज्ञा(0);
      }
}

शून्य vvp_udp_comb_s::compile_table(वर्ण**tab)
{
      unsigned nrows0 = 0, nrows1 = 0;

	/* First run through the table to figure out the number of
	   rows I need for each kind of table. */
      प्रति (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    प्रतिज्ञा(strlen(tab[idx]) == port_गणनम्() + 1);
	    विकल्प (tab[idx][port_गणनम्()]) {
		पक्ष '0':
		  nrows0 += 1;
		  भङ्ग;
		पक्ष '1':
		  nrows1 += 1;
		  भङ्ग;
		पक्ष 'x':
		  भङ्ग;
		स्वभाव:
		  fprintf(stderr, "Unsupported entry %c(%d).\n",
		          tab[idx][port_गणनम्()], tab[idx][port_गणनम्()]);
		  प्रतिज्ञा(0);
	    }
      }

      nlevels0_ = nrows0;
      levels0_ = नूतन udp_levels_table[nlevels0_];

      nlevels1_ = nrows1;
      levels1_ = नूतन udp_levels_table[nlevels1_];

      nrows0 = 0;
      nrows1 = 0;
      प्रति (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    संरचना udp_levels_table cur;
	    cur.mask0 = 0;
	    cur.mask1 = 0;
	    cur.maskx = 0;
	    जइ (port_गणनम्() > 8*परिमाण(cur.mask0)) {
		  fprintf(stderr, "internal error: primitive port count=%u "
			  " > %zu\n", port_गणनम्(), परिमाण(cur.mask0));
		  प्रतिज्ञा(port_गणनम्() <= 8*परिमाण(cur.mask0));
	    }
	    प्रति (unsigned pp = 0 ;  pp < port_गणनम्() ;  pp += 1) {
		  unsigned long mask_bit = 1UL << pp;
		  or_based_on_char(cur, tab[idx][pp], mask_bit);
	    }

	    विकल्प (tab[idx][port_गणनम्()]) {
		पक्ष '0':
		  levels0_[nrows0++] = cur;
		  भङ्ग;
		पक्ष '1':
		  levels1_[nrows1++] = cur;
		  भङ्ग;
		स्वभाव:
		  भङ्ग;
	    }
	    delete[] tab[idx];
      }
      मुक्त(tab);

      प्रतिज्ञा(nrows0 == nlevels0_);
      प्रतिज्ञा(nrows1 == nlevels1_);
}

vvp_udp_seq_s::vvp_udp_seq_s(ध्रुव वर्ण*label, वर्ण*name__,
			     unsigned ports, vvp_bit4_t init)
: vvp_udp_s(label, name__, ports, init, सच्च)
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

शून्य edge_based_on_char(संरचना udp_edges_table&cur, वर्ण chr, unsigned pos)
{
      unsigned long mask_bit = 1UL << pos;

      विकल्प (chr) {
	  पक्ष '0':
	    cur.mask0 |= mask_bit;
	    भङ्ग;
	  पक्ष '1':
	    cur.mask1 |= mask_bit;
	    भङ्ग;
	  पक्ष 'x':
	    cur.maskx |= mask_bit;
	    भङ्ग;
	  पक्ष 'b':
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    भङ्ग;
	  पक्ष 'l':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    भङ्ग;
	  पक्ष 'h':
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    भङ्ग;
	  पक्ष '?':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    भङ्ग;

	  पक्ष 'B': // (x?) edge
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    भङ्ग;
	  पक्ष 'f': // (10) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    भङ्ग;
	  पक्ष 'F': // (x0) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    भङ्ग;
	  पक्ष 'M': // (1x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    भङ्ग;
	  पक्ष 'N': // (1x) and (10) edge
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    भङ्ग;
	  पक्ष 'P': // (0x) and (01) edge
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    भङ्ग;
	  पक्ष 'q': // (bx) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    भङ्ग;
	  पक्ष 'Q': // (0x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    भङ्ग;
	  पक्ष 'r': // (01) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    भङ्ग;
	  पक्ष 'R': // (x1) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    भङ्ग;
	  पक्ष '%': // (?x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    भङ्ग;
	  पक्ष '+': // (?1) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    भङ्ग;
	  पक्ष '_': // (?0) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 1;
	    भङ्ग;
	  स्वभाव:
	    fprintf(stderr, "internal error: unknown edge code: %c\n", chr);
	    प्रतिज्ञा(0);
      }
}

शून्य vvp_udp_seq_s::compile_table(वर्ण**tab)
{

      प्रति (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    ध्रुव वर्ण*row = tab[idx];
	    प्रतिज्ञा(strlen(row) == port_गणनम्() + 2);

	    जइ (strspn(row, "01xblh?") >= port_गणनम्()+1) {

		  विकल्प (row[port_गणनम्()+1]) {
		      पक्ष '0':
			nlevels0_ += 1;
			भङ्ग;
		      पक्ष '1':
			nlevels1_ += 1;
			भङ्ग;
		      पक्ष 'x':
			nlevelsx_ += 1;
			भङ्ग;
		      पक्ष '-':
			nlevelsL_ += 1;
			भङ्ग;
		      स्वभाव:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_गणनम्()+1], row[port_गणनम्()+1]);
			प्रतिज्ञा(0);
			भङ्ग;
		  }

	    } अण्णहा {
		    /* Rows that have n or p edges will need to be
		       expanded into 2 rows. */
		  unsigned extra = 0;
		  जइ (strchr(row,'n'))
			extra = 1;
		  जइ (strchr(row,'p'))
			extra = 1;
		  जइ (strchr(row,'*'))
			extra = 2;

		  विकल्प (row[port_गणनम्()+1]) {
		      पक्ष '0':
			nedges0_ += 1 + extra;
			भङ्ग;
		      पक्ष '1':
			nedges1_ += 1 + extra;
			भङ्ग;
		      पक्ष 'x':
			भङ्ग;
		      पक्ष '-':
			nedgesL_ += 1 + extra;
			भङ्ग;
		      स्वभाव:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_गणनम्()+1], row[port_गणनम्()+1]);
			प्रतिज्ञा(0);
			भङ्ग;
		  }
	    }
      }

      levels0_ = नूतन udp_levels_table[nlevels0_];
      levels1_ = नूतन udp_levels_table[nlevels1_];
      levelsx_ = नूतन udp_levels_table[nlevelsx_];
      levelsL_ = नूतन udp_levels_table[nlevelsL_];
      edges0_ = नूतन udp_edges_table[nedges0_];
      edges1_ = नूतन udp_edges_table[nedges1_];
      edgesL_ = नूतन udp_edges_table[nedgesL_];

      unsigned idx_lev0 = 0;
      unsigned idx_lev1 = 0;
      unsigned idx_levx = 0;
      unsigned idx_levL = 0;
      unsigned idx_edg0 = 0;
      unsigned idx_edg1 = 0;
      unsigned idx_edgL = 0;

      प्रति (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    ध्रुव वर्ण*row = tab[idx];

	    जइ (strspn(row, "01xblh?") >= port_गणनम्()+1) {
		  संरचना udp_levels_table cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  प्रति (unsigned pp = 0 ;  pp < port_गणनम्() ;  pp += 1) {
			unsigned long mask_bit = 1UL << pp;
			or_based_on_char(cur, row[pp+1], mask_bit);
		  }

		  or_based_on_char(cur, row[0], 1UL << port_गणनम्());

		  विकल्प (row[port_गणनम्()+1]) {
		      पक्ष '0':
			levels0_[idx_lev0++] = cur;
			भङ्ग;
		      पक्ष '1':
			levels1_[idx_lev1++] = cur;
			भङ्ग;
		      पक्ष 'x':
			levelsx_[idx_levx++] = cur;
			भङ्ग;
		      पक्ष '-':
			levelsL_[idx_levL++] = cur;
			भङ्ग;
		      स्वभाव:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_गणनम्()+1], row[port_गणनम्()+1]);
			प्रतिज्ञा(0);
			भङ्ग;
		  }

	    } अण्णहा {
		  संरचना udp_edges_table cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  cur.edge_position = 0;
		  cur.edge_mask0 = 0;
		  cur.edge_mask1 = 0;
		  cur.edge_maskx = 0;

		  तार्किक need_ext0_table = मिच्छ;
		  संरचना udp_edges_table ext0;
		  ext0.mask0 = 0;
		  ext0.mask1 = 0;
		  ext0.maskx = 0;
		  ext0.edge_position = 0;
		  ext0.edge_mask0 = 0;
		  ext0.edge_mask1 = 0;
		  ext0.edge_maskx = 0;

		  तार्किक need_ext1_table = मिच्छ;
		  संरचना udp_edges_table ext1;
		  ext1.mask0 = 0;
		  ext1.mask1 = 0;
		  ext1.maskx = 0;
		  ext1.edge_position = 0;
		  ext1.edge_mask0 = 0;
		  ext1.edge_mask1 = 0;
		  ext1.edge_maskx = 0;

		  प्रति (unsigned pp = 0 ;  pp < port_गणनम्() ; pp += 1) {
			विकल्प (row[pp+1]) {
			    पक्ष 'n':
			      edge_based_on_char(cur, 'N', pp);
			      edge_based_on_char(ext0, '_', pp);
			      need_ext0_table = सच्च;
			      भङ्ग;
			    पक्ष 'p':
			      edge_based_on_char(cur, 'P', pp);
			      edge_based_on_char(ext0, '+', pp);
			      need_ext0_table = सच्च;
			      भङ्ग;
			    पक्ष '*':
			      edge_based_on_char(cur, 'P', pp);
			      edge_based_on_char(ext0, 'N', pp);
			      edge_based_on_char(ext1, 'B', pp);
			      need_ext0_table = सच्च;
			      need_ext1_table = सच्च;
			      भङ्ग;
			    स्वभाव:
			      edge_based_on_char(cur, row[pp+1], pp);
			      edge_based_on_char(ext0, row[pp+1], pp);
			      edge_based_on_char(ext1, row[pp+1], pp);
			      भङ्ग;
			}
		  }
		  edge_based_on_char(cur, row[0], port_गणनम्());
		  edge_based_on_char(ext0, row[0], port_गणनम्());
		  edge_based_on_char(ext1, row[0], port_गणनम्());

		  विकल्प (row[port_गणनम्()+1]) {
		      पक्ष '0':
			edges0_[idx_edg0++] = cur;
			जइ (need_ext0_table)
			      edges0_[idx_edg0++] = ext0;
			जइ (need_ext1_table)
			      edges0_[idx_edg0++] = ext1;
			भङ्ग;
		      पक्ष '1':
			edges1_[idx_edg1++] = cur;
			जइ (need_ext0_table)
			      edges1_[idx_edg1++] = ext0;
			जइ (need_ext1_table)
			      edges1_[idx_edg1++] = ext1;
			भङ्ग;
		      पक्ष 'x':
			भङ्ग;
		      पक्ष '-':
			edgesL_[idx_edgL++] = cur;
			जइ (need_ext0_table)
			      edgesL_[idx_edgL++] = ext0;
			जइ (need_ext1_table)
			      edgesL_[idx_edgL++] = ext1;
			भङ्ग;
		      स्वभाव:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_गणनम्()+1], row[port_गणनम्()+1]);
			प्रतिज्ञा(0);
			भङ्ग;
		  }

	    }
	    delete[] tab[idx];
      }
      मुक्त(tab);

      प्रतिज्ञा(idx_edg0 == nedges0_);
      प्रतिज्ञा(idx_edg1 == nedges1_);
      प्रतिज्ञा(idx_edgL == nedgesL_);

}

तार्किक operator == (ध्रुव udp_levels_table&a, ध्रुव udp_levels_table&b)
{
      जइ (a.mask0 != b.mask0)
	    प्रत्यागम मिच्छ;
      जइ (a.mask1 != b.mask1)
	    प्रत्यागम मिच्छ;
      जइ (a.maskx != b.maskx)
	    प्रत्यागम मिच्छ;
      प्रत्यागम सच्च;
}

vvp_bit4_t vvp_udp_seq_s::calculate_output(ध्रुव udp_levels_table&cur,
					   ध्रुव udp_levels_table&prev,
					   vvp_bit4_t cur_out)
{
      जइ (cur == prev)
	    प्रत्यागम cur_out;

      udp_levels_table cur_tmp = cur;

      unsigned long mask_out = 1UL << port_गणनम्();
      विकल्प (cur_out) {
	  पक्ष BIT4_0:
	    cur_tmp.mask0 |= mask_out;
	    भङ्ग;
	  पक्ष BIT4_1:
	    cur_tmp.mask1 |= mask_out;
	    भङ्ग;
	  स्वभाव:
	    cur_tmp.maskx |= mask_out;
	    भङ्ग;
      }

      vvp_bit4_t lev = test_levels_(cur_tmp);
      जइ (lev == BIT4_Z) {
	    lev = test_edges_(cur_tmp, prev);
      }

      प्रत्यागम lev;
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
vvp_bit4_t vvp_udp_seq_s::test_levels_(ध्रुव udp_levels_table&cur)
{
      प्रति (unsigned idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    जइ (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  अनुवर्त;
	    जइ (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  अनुवर्त;
	    जइ (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  अनुवर्त;

	    प्रत्यागम BIT4_0;
      }

      प्रति (unsigned idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    जइ (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  अनुवर्त;
	    जइ (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  अनुवर्त;
	    जइ (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  अनुवर्त;

	    प्रत्यागम BIT4_1;
      }

	/* We need to test against an explicit X-output table, since
	   we need to distinguish from an X output and no match. */
      प्रति (unsigned idx = 0 ;  idx < nlevelsx_ ;  idx += 1) {
	    जइ (cur.mask0 != (cur.mask0 & levelsx_[idx].mask0))
		  अनुवर्त;
	    जइ (cur.mask1 != (cur.mask1 & levelsx_[idx].mask1))
		  अनुवर्त;
	    जइ (cur.maskx != (cur.maskx & levelsx_[idx].maskx))
		  अनुवर्त;

	    प्रत्यागम BIT4_X;
      }

	/* Test the table that requests the next output be the same as
	   the current output. This gets the current output from the
	   levels table that was passed in. */
      प्रति (unsigned idx = 0 ;  idx < nlevelsL_ ;  idx += 1) {
	    जइ (cur.mask0 != (cur.mask0 & levelsL_[idx].mask0))
		  अनुवर्त;
	    जइ (cur.mask1 != (cur.mask1 & levelsL_[idx].mask1))
		  अनुवर्त;
	    जइ (cur.maskx != (cur.maskx & levelsL_[idx].maskx))
		  अनुवर्त;

	    जइ (cur.mask0 & (1 << port_गणनम्()))
		  प्रत्यागम BIT4_0;
	    जइ (cur.mask1 & (1 << port_गणनम्()))
		  प्रत्यागम BIT4_1;
	    जइ (cur.maskx & (1 << port_गणनम्()))
		  प्रत्यागम BIT4_X;

	    प्रतिज्ञा(0);
	    प्रत्यागम BIT4_X;
      }

	/* No explicit levels entry match. Return a Z to signal that
	   further testing is needed. */
      प्रत्यागम BIT4_Z;
}

vvp_bit4_t vvp_udp_seq_s::test_edges_(ध्रुव udp_levels_table&cur,
				      ध्रुव udp_levels_table&prev)
{
	/* The edge_mask is true for all bits that are different in
	   the cur and prev tables. */
      unsigned long edge0_mask = cur.mask0 ^ prev.mask0;
      unsigned long edgex_mask = cur.maskx ^ prev.maskx;
      unsigned long edge1_mask = cur.mask1 ^ prev.mask1;

      unsigned long edge_mask = edge0_mask|edgex_mask|edge1_mask;
      edge_mask &= ~ (-1UL << port_गणनम्());

	/* If there are no differences, then there are no edges. Give
	   up now. */
      जइ (edge_mask == 0)
	    प्रत्यागम BIT4_X;

      unsigned edge_position = 0;
      जाव ((edge_mask&1) == 0) {
	    edge_mask >>= 1;
	    edge_position += 1;
      }

	/* We expect that there is exactly one edge in here. */
      प्रतिज्ञा(edge_mask == 1);

      edge_mask = 1UL << edge_position;

      unsigned edge_mask0 = (prev.mask0&edge_mask)? 1 : 0;
      unsigned edge_maskx = (prev.maskx&edge_mask)? 1 : 0;
      unsigned edge_mask1 = (prev.mask1&edge_mask)? 1 : 0;


	/* Now the edge_position and edge_mask* variables have the
	   values we use to test the applicability of the edge_table
	   entries. */

      प्रति (unsigned idx = 0 ;  idx < nedges0_ ;  idx += 1) {
	    ध्रुव संरचना udp_edges_table*row = edges0_ + idx;

	    जइ (row->edge_position != edge_position)
		  अनुवर्त;
	    जइ (edge_mask0 && !row->edge_mask0)
		  अनुवर्त;
	    जइ (edge_maskx && !row->edge_maskx)
		  अनुवर्त;
	    जइ (edge_mask1 && !row->edge_mask1)
		  अनुवर्त;
	    जइ (cur.mask0 != (cur.mask0 & row->mask0))
		  अनुवर्त;
	    जइ (cur.maskx != (cur.maskx & row->maskx))
		  अनुवर्त;
	    जइ (cur.mask1 != (cur.mask1 & row->mask1))
		  अनुवर्त;

	    प्रत्यागम BIT4_0;
      }

      प्रति (unsigned idx = 0 ;  idx < nedges1_ ;  idx += 1) {
	    ध्रुव संरचना udp_edges_table*row = edges1_ + idx;

	    जइ (row->edge_position != edge_position)
		  अनुवर्त;
	    जइ (edge_mask0 && !row->edge_mask0)
		  अनुवर्त;
	    जइ (edge_maskx && !row->edge_maskx)
		  अनुवर्त;
	    जइ (edge_mask1 && !row->edge_mask1)
		  अनुवर्त;
	    जइ (cur.mask0 != (cur.mask0 & row->mask0))
		  अनुवर्त;
	    जइ (cur.maskx != (cur.maskx & row->maskx))
		  अनुवर्त;
	    जइ (cur.mask1 != (cur.mask1 & row->mask1))
		  अनुवर्त;

	    प्रत्यागम BIT4_1;
      }

      प्रति (unsigned idx = 0 ;  idx < nedgesL_ ;  idx += 1) {
	    ध्रुव संरचना udp_edges_table*row = edgesL_ + idx;

	    जइ (row->edge_position != edge_position)
		  अनुवर्त;
	    जइ (edge_mask0 && !row->edge_mask0)
		  अनुवर्त;
	    जइ (edge_maskx && !row->edge_maskx)
		  अनुवर्त;
	    जइ (edge_mask1 && !row->edge_mask1)
		  अनुवर्त;
	    जइ (cur.mask0 != (cur.mask0 & row->mask0))
		  अनुवर्त;
	    जइ (cur.maskx != (cur.maskx & row->maskx))
		  अनुवर्त;
	    जइ (cur.mask1 != (cur.mask1 & row->mask1))
		  अनुवर्त;

	    जइ (cur.mask0 & (1 << port_गणनम्()))
		  प्रत्यागम BIT4_0;
	    जइ (cur.mask1 & (1 << port_गणनम्()))
		  प्रत्यागम BIT4_1;
	    जइ (cur.maskx & (1 << port_गणनम्()))
		  प्रत्यागम BIT4_X;

	    प्रतिज्ञा(0);
	    प्रत्यागम BIT4_X;
      }

      प्रत्यागम BIT4_X;
}

vvp_udp_fun_core::vvp_udp_fun_core(vvp_net_t*net, vvp_udp_s*def)
: vvp_wide_fun_core(net, def->port_गणनम्())
{
      def_ = def;
      cur_out_ = def_->get_init();
	// Assume initially that all the inputs are 1'bx
      current_.mask0 = 0;
      current_.mask1 = 0;
      current_.maskx = ~ ((-1UL) << port_गणनम्());

        // If the initial value is 0 or 1, schedule the initial assignment
        // normally, so that any sensitive always processes can be started
        // first.
      जइ (cur_out_ != BIT4_X)
	    schedule_generic(एतत्, 0, मिच्छ);
      अण्णहा
	    schedule_functor(एतत्);
}

vvp_udp_fun_core::~vvp_udp_fun_core()
{
}

/*
 * This method is used to propagate the initial value on startup.
 */
शून्य vvp_udp_fun_core::run_run()
{
      vvp_vector4_t tmp (1);
      tmp.set_bit(0, cur_out_);
      propagate_vec4(tmp);
}

शून्य vvp_udp_fun_core::recv_vec4_from_inputs(unsigned port)
{
	/* For now, assume udps are 1-bit wide. */
      unsigned port_width = value(port).size();
      प्रतिज्ञा(port_width == 1);

      unsigned long mask = 1UL << port;

      udp_levels_table prev = current_;

      विकल्प (value(port).value(0)) {

	  पक्ष BIT4_0:
	    current_.mask0 |= mask;
	    current_.mask1 &= ~mask;
	    current_.maskx &= ~mask;
	    भङ्ग;
	  पक्ष BIT4_1:
	    current_.mask0 &= ~mask;
	    current_.mask1 |= mask;
	    current_.maskx &= ~mask;
	    भङ्ग;
	  स्वभाव:
	    current_.mask0 &= ~mask;
	    current_.mask1 &= ~mask;
	    current_.maskx |= mask;
	    भङ्ग;
      }

      vvp_bit4_t out_bit = def_->calculate_output(current_, prev, cur_out_);

      जइ (out_bit == cur_out_)
	    प्रत्यागम;

      cur_out_ = out_bit;
      schedule_functor(एतत्);
}


/*
 * This function is called by the parser in response to a .udp
 * node. We create the nodes needed to integrate the UDP into the
 * netlist. The definition should be parsed already.
 */
शून्य compile_udp_functor(वर्ण*label, वर्ण*type,
			 unsigned argc, संरचना symb_s*argv)
{
      संरचना vvp_udp_s *def = udp_find(type);
      प्रतिज्ञा(def);
      मुक्त(type);

      vvp_net_t*ptr = नूतन vvp_net_t;
      vvp_udp_fun_core*core = नूतन vvp_udp_fun_core(ptr, def);
      ptr->fun = core;

      define_functor_चिह्नम्(label, ptr);
      मुक्त(label);

      wide_inputs_connect(core, argc, argv);
      मुक्त(argv);
}
