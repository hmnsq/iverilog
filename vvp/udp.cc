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
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
#अन्तर्भाव "vvp_cleanup.h"
#अन्तर्भाव "ivl_alloc.h"
#यदि_अन्तः

उपयुज्य नामाकाश std;

// We may need these later when we build the VPI interface to
// UDP definitions.
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
स्थिर vvp_udp_s **udp_defns = 0;
स्थिर अचिह्नित udp_defns_गणनम् = 0;

शून्य udp_defns_delete()
{
      प्रति (अचिह्नित idx = 0; idx < udp_defns_गणनम्; idx += 1) {
	    यदि (udp_defns[idx]->is_sequential()) {
		  vvp_udp_seq_s *obj = static_cast<vvp_udp_seq_s *>
		                       (udp_defns[idx]);
		  विलोपय obj;
	    } अन्यथा {
		  vvp_udp_comb_s *obj = static_cast<vvp_udp_comb_s *>
		                        (udp_defns[idx]);
		  विलोपय obj;
	    }
      }
      मुक्त(udp_defns);
      udp_defns = 0;
      udp_defns_गणनम् = 0;
}
#यदि_अन्तः

स्थिर चिह्नम्_सारणी_t udp_सारणी;

शून्य delete_udp_symbols()
{
      delete_चिह्नम्_सारणी(udp_सारणी);
      udp_सारणी = 0;
}

संरचना vvp_udp_s *udp_find(ध्रुव वर्ण *label)
{
      चिह्नम्_मानम्_t v = sym_get_मानम्(udp_सारणी, label);
      प्रत्यागम (संरचना vvp_udp_s *)v.ptr;
}

ostream& संक्रियकः <<(ostream&o, ध्रुव संरचना udp_स्तराः_सारणी&सारणी)
{
      o << "[" << hex << सारणी.mask0
	<< "/" << hex << सारणी.mask1
	<< "/" << hex << सारणी.maskx << "]";
      प्रत्यागम o;
}

vvp_udp_s::vvp_udp_s(ध्रुव वर्ण*label, वर्ण*name__, अचिह्नित ports,
                     vvp_bit4_t init, तार्किक type)
: name_(name__), ports_(ports), init_(init), seq_(type)
{
      यदि (!udp_सारणी)
	    udp_सारणी = new_चिह्नम्_सारणी();

      प्रतिज्ञा( !udp_find(label) );

      चिह्नम्_मानम्_t v;
      v.ptr = एतत्;
      sym_समुच्चयः_मानम्(udp_सारणी, label, v);

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
      udp_defns_गणनम् += 1;
      udp_defns = static_cast<vvp_udp_s **>(realloc(udp_defns,
                  udp_defns_गणनम्*परिमाण(vvp_udp_s **)));
      udp_defns[udp_defns_गणनम्-1] = एतत्;
#यदि_अन्तः
}

vvp_udp_s::~vvp_udp_s()
{
      विलोपय[] name_;
}

अचिह्नित vvp_udp_s::पत्तनम्_गणनम्() ध्रुव
{
      प्रत्यागम ports_;
}

vvp_bit4_t vvp_udp_s::get_init() ध्रुव
{
      प्रत्यागम init_;
}

vvp_udp_comb_s::vvp_udp_comb_s(ध्रुव वर्ण*label, वर्ण*name__, अचिह्नित ports)
: vvp_udp_s(label, name__, ports, BIT4_X, असत्य)
{
      levels0_ = 0;
      levels1_ = 0;
      nlevels0_ = 0;
      nlevels1_ = 0;
}

vvp_udp_comb_s::~vvp_udp_comb_s()
{
      विलोपय[] levels0_;
      विलोपय[] levels1_;
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
vvp_bit4_t vvp_udp_comb_s::test_स्तराः(ध्रुव udp_स्तराः_सारणी&cur)
{
	/* To test for a row match, test that the mask0, mask1 and
	   maskx vectors all have bits set where the matching
	   cur.mask0/1/x vectors have bits set. It is possible that a
	   levels0_[idx] vector has more bits set then the cur mask,
	   but this is OK and these bits are to be ignored. */

      प्रति (अचिह्नित idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    यदि (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  अनुवर्त;
	    यदि (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  अनुवर्त;
	    यदि (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  अनुवर्त;

	    प्रत्यागम BIT4_0;
      }

      प्रति (अचिह्नित idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    यदि (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  अनुवर्त;
	    यदि (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  अनुवर्त;
	    यदि (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  अनुवर्त;

	    प्रत्यागम BIT4_1;
      }

      प्रत्यागम BIT4_X;
}

vvp_bit4_t vvp_udp_comb_s::calculate_output(ध्रुव udp_स्तराः_सारणी&cur,
					    ध्रुव udp_स्तराः_सारणी&,
					    vvp_bit4_t)
{
      प्रत्यागम test_स्तराः(cur);
}

स्थिर शून्य or_based_on_char(udp_स्तराः_सारणी&cur, वर्ण flag,
			     अचिह्नित दीर्घ आवरणम्_bit)
{
      विकल्प (flag) {
	  पक्ष '0':
	    cur.mask0 |= आवरणम्_bit;
	    भङ्ग;
	  पक्ष '1':
	    cur.mask1 |= आवरणम्_bit;
	    भङ्ग;
	  पक्ष 'x':
	    cur.maskx |= आवरणम्_bit;
	    भङ्ग;
	  पक्ष 'b':
	    cur.mask0 |= आवरणम्_bit;
	    cur.mask1 |= आवरणम्_bit;
	    भङ्ग;
	  पक्ष 'l':
	    cur.mask0 |= आवरणम्_bit;
	    cur.maskx |= आवरणम्_bit;
	    भङ्ग;
	  पक्ष 'h':
	    cur.maskx |= आवरणम्_bit;
	    cur.mask1 |= आवरणम्_bit;
	    भङ्ग;
	  पक्ष '?':
	    cur.mask0 |= आवरणम्_bit;
	    cur.maskx |= आवरणम्_bit;
	    cur.mask1 |= आवरणम्_bit;
	    भङ्ग;
	  स्वभाव:
	    fprintf(stderr, "Unsupported flag %c(%d).\n", flag, flag);
	    प्रतिज्ञा(0);
      }
}

शून्य vvp_udp_comb_s::compile_सारणी(वर्ण**tab)
{
      अचिह्नित nrows0 = 0, nrows1 = 0;

	/* First run through the table to figure out the number of
	   rows I need for each kind of table. */
      प्रति (अचिह्नित idx = 0 ;  tab[idx] ;  idx += 1) {
	    प्रतिज्ञा(वाक्यदैर्घ्यम्(tab[idx]) == पत्तनम्_गणनम्() + 1);
	    विकल्प (tab[idx][पत्तनम्_गणनम्()]) {
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
		          tab[idx][पत्तनम्_गणनम्()], tab[idx][पत्तनम्_गणनम्()]);
		  प्रतिज्ञा(0);
	    }
      }

      nlevels0_ = nrows0;
      levels0_ = नूतन udp_स्तराः_सारणी[nlevels0_];

      nlevels1_ = nrows1;
      levels1_ = नूतन udp_स्तराः_सारणी[nlevels1_];

      nrows0 = 0;
      nrows1 = 0;
      प्रति (अचिह्नित idx = 0 ;  tab[idx] ;  idx += 1) {
	    संरचना udp_स्तराः_सारणी cur;
	    cur.mask0 = 0;
	    cur.mask1 = 0;
	    cur.maskx = 0;
	    यदि (पत्तनम्_गणनम्() > 8*परिमाण(cur.mask0)) {
		  fprintf(stderr, "internal error: primitive port count=%u "
			  " > %zu\n", पत्तनम्_गणनम्(), परिमाण(cur.mask0));
		  प्रतिज्ञा(पत्तनम्_गणनम्() <= 8*परिमाण(cur.mask0));
	    }
	    प्रति (अचिह्नित pp = 0 ;  pp < पत्तनम्_गणनम्() ;  pp += 1) {
		  अचिह्नित दीर्घ आवरणम्_bit = 1UL << pp;
		  or_based_on_char(cur, tab[idx][pp], आवरणम्_bit);
	    }

	    विकल्प (tab[idx][पत्तनम्_गणनम्()]) {
		पक्ष '0':
		  levels0_[nrows0++] = cur;
		  भङ्ग;
		पक्ष '1':
		  levels1_[nrows1++] = cur;
		  भङ्ग;
		स्वभाव:
		  भङ्ग;
	    }
	    विलोपय[] tab[idx];
      }
      मुक्त(tab);

      प्रतिज्ञा(nrows0 == nlevels0_);
      प्रतिज्ञा(nrows1 == nlevels1_);
}

vvp_udp_seq_s::vvp_udp_seq_s(ध्रुव वर्ण*label, वर्ण*name__,
			     अचिह्नित ports, vvp_bit4_t init)
: vvp_udp_s(label, name__, ports, init, सत्य)
{
      levels0_ = 0;
      levels1_ = 0;
      levelsx_ = 0;
      स्तराः_L = 0;

      nlevels0_ = 0;
      nlevels1_ = 0;
      nlevelsx_ = 0;
      nlevelsL_ = 0;

      edges0_ = 0;
      edges1_ = 0;
      धाराः_L = 0;

      nedges0_ = 0;
      nedges1_ = 0;
      nedgesL_ = 0;
}

vvp_udp_seq_s::~vvp_udp_seq_s()
{
      विलोपय[] levels0_;
      विलोपय[] levels1_;
      विलोपय[] levelsx_;
      विलोपय[] स्तराः_L;
      विलोपय[] edges0_;
      विलोपय[] edges1_;
      विलोपय[] धाराः_L;
}

शून्य edge_based_on_char(संरचना udp_धाराः_सारणी&cur, वर्ण chr, अचिह्नित pos)
{
      अचिह्नित दीर्घ आवरणम्_bit = 1UL << pos;

      विकल्प (chr) {
	  पक्ष '0':
	    cur.mask0 |= आवरणम्_bit;
	    भङ्ग;
	  पक्ष '1':
	    cur.mask1 |= आवरणम्_bit;
	    भङ्ग;
	  पक्ष 'x':
	    cur.maskx |= आवरणम्_bit;
	    भङ्ग;
	  पक्ष 'b':
	    cur.mask0 |= आवरणम्_bit;
	    cur.mask1 |= आवरणम्_bit;
	    भङ्ग;
	  पक्ष 'l':
	    cur.mask0 |= आवरणम्_bit;
	    cur.maskx |= आवरणम्_bit;
	    भङ्ग;
	  पक्ष 'h':
	    cur.maskx |= आवरणम्_bit;
	    cur.mask1 |= आवरणम्_bit;
	    भङ्ग;
	  पक्ष '?':
	    cur.mask0 |= आवरणम्_bit;
	    cur.maskx |= आवरणम्_bit;
	    cur.mask1 |= आवरणम्_bit;
	    भङ्ग;

	  पक्ष 'B': // (x?) edge
	    cur.mask0 |= आवरणम्_bit;
	    cur.mask1 |= आवरणम्_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    भङ्ग;
	  पक्ष 'f': // (10) edge
	    cur.mask0 |= आवरणम्_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    भङ्ग;
	  पक्ष 'F': // (x0) edge
	    cur.mask0 |= आवरणम्_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    भङ्ग;
	  पक्ष 'M': // (1x) edge
	    cur.maskx |= आवरणम्_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    भङ्ग;
	  पक्ष 'N': // (1x) and (10) edge
	    cur.mask0 |= आवरणम्_bit;
	    cur.maskx |= आवरणम्_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    भङ्ग;
	  पक्ष 'P': // (0x) and (01) edge
	    cur.maskx |= आवरणम्_bit;
	    cur.mask1 |= आवरणम्_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    भङ्ग;
	  पक्ष 'q': // (bx) edge
	    cur.maskx |= आवरणम्_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    भङ्ग;
	  पक्ष 'Q': // (0x) edge
	    cur.maskx |= आवरणम्_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    भङ्ग;
	  पक्ष 'r': // (01) edge
	    cur.mask1 |= आवरणम्_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    भङ्ग;
	  पक्ष 'R': // (x1) edge
	    cur.mask1 |= आवरणम्_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    भङ्ग;
	  पक्ष '%': // (?x) edge
	    cur.maskx |= आवरणम्_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    भङ्ग;
	  पक्ष '+': // (?1) edge
	    cur.mask1 |= आवरणम्_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    भङ्ग;
	  पक्ष '_': // (?0) edge
	    cur.mask0 |= आवरणम्_bit;
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

शून्य vvp_udp_seq_s::compile_सारणी(वर्ण**tab)
{

      प्रति (अचिह्नित idx = 0 ;  tab[idx] ;  idx += 1) {
	    ध्रुव वर्ण*row = tab[idx];
	    प्रतिज्ञा(वाक्यदैर्घ्यम्(row) == पत्तनम्_गणनम्() + 2);

	    यदि (strspn(row, "01xblh?") >= पत्तनम्_गणनम्()+1) {

		  विकल्प (row[पत्तनम्_गणनम्()+1]) {
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
			        row[पत्तनम्_गणनम्()+1], row[पत्तनम्_गणनम्()+1]);
			प्रतिज्ञा(0);
			भङ्ग;
		  }

	    } अन्यथा {
		    /* Rows that have n or p edges will need to be
		       expanded into 2 rows. */
		  अचिह्नित extra = 0;
		  यदि (strchr(row,'n'))
			extra = 1;
		  यदि (strchr(row,'p'))
			extra = 1;
		  यदि (strchr(row,'*'))
			extra = 2;

		  विकल्प (row[पत्तनम्_गणनम्()+1]) {
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
			        row[पत्तनम्_गणनम्()+1], row[पत्तनम्_गणनम्()+1]);
			प्रतिज्ञा(0);
			भङ्ग;
		  }
	    }
      }

      levels0_ = नूतन udp_स्तराः_सारणी[nlevels0_];
      levels1_ = नूतन udp_स्तराः_सारणी[nlevels1_];
      levelsx_ = नूतन udp_स्तराः_सारणी[nlevelsx_];
      स्तराः_L = नूतन udp_स्तराः_सारणी[nlevelsL_];
      edges0_ = नूतन udp_धाराः_सारणी[nedges0_];
      edges1_ = नूतन udp_धाराः_सारणी[nedges1_];
      धाराः_L = नूतन udp_धाराः_सारणी[nedgesL_];

      अचिह्नित idx_lev0 = 0;
      अचिह्नित idx_lev1 = 0;
      अचिह्नित idx_levx = 0;
      अचिह्नित idx_levL = 0;
      अचिह्नित idx_edg0 = 0;
      अचिह्नित idx_edg1 = 0;
      अचिह्नित idx_edgL = 0;

      प्रति (अचिह्नित idx = 0 ;  tab[idx] ;  idx += 1) {
	    ध्रुव वर्ण*row = tab[idx];

	    यदि (strspn(row, "01xblh?") >= पत्तनम्_गणनम्()+1) {
		  संरचना udp_स्तराः_सारणी cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  प्रति (अचिह्नित pp = 0 ;  pp < पत्तनम्_गणनम्() ;  pp += 1) {
			अचिह्नित दीर्घ आवरणम्_bit = 1UL << pp;
			or_based_on_char(cur, row[pp+1], आवरणम्_bit);
		  }

		  or_based_on_char(cur, row[0], 1UL << पत्तनम्_गणनम्());

		  विकल्प (row[पत्तनम्_गणनम्()+1]) {
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
			स्तराः_L[idx_levL++] = cur;
			भङ्ग;
		      स्वभाव:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[पत्तनम्_गणनम्()+1], row[पत्तनम्_गणनम्()+1]);
			प्रतिज्ञा(0);
			भङ्ग;
		  }

	    } अन्यथा {
		  संरचना udp_धाराः_सारणी cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  cur.edge_position = 0;
		  cur.edge_mask0 = 0;
		  cur.edge_mask1 = 0;
		  cur.edge_maskx = 0;

		  तार्किक need_ext0_सारणी = असत्य;
		  संरचना udp_धाराः_सारणी ext0;
		  ext0.mask0 = 0;
		  ext0.mask1 = 0;
		  ext0.maskx = 0;
		  ext0.edge_position = 0;
		  ext0.edge_mask0 = 0;
		  ext0.edge_mask1 = 0;
		  ext0.edge_maskx = 0;

		  तार्किक need_ext1_सारणी = असत्य;
		  संरचना udp_धाराः_सारणी ext1;
		  ext1.mask0 = 0;
		  ext1.mask1 = 0;
		  ext1.maskx = 0;
		  ext1.edge_position = 0;
		  ext1.edge_mask0 = 0;
		  ext1.edge_mask1 = 0;
		  ext1.edge_maskx = 0;

		  प्रति (अचिह्नित pp = 0 ;  pp < पत्तनम्_गणनम्() ; pp += 1) {
			विकल्प (row[pp+1]) {
			    पक्ष 'n':
			      edge_based_on_char(cur, 'N', pp);
			      edge_based_on_char(ext0, '_', pp);
			      need_ext0_सारणी = सत्य;
			      भङ्ग;
			    पक्ष 'p':
			      edge_based_on_char(cur, 'P', pp);
			      edge_based_on_char(ext0, '+', pp);
			      need_ext0_सारणी = सत्य;
			      भङ्ग;
			    पक्ष '*':
			      edge_based_on_char(cur, 'P', pp);
			      edge_based_on_char(ext0, 'N', pp);
			      edge_based_on_char(ext1, 'B', pp);
			      need_ext0_सारणी = सत्य;
			      need_ext1_सारणी = सत्य;
			      भङ्ग;
			    स्वभाव:
			      edge_based_on_char(cur, row[pp+1], pp);
			      edge_based_on_char(ext0, row[pp+1], pp);
			      edge_based_on_char(ext1, row[pp+1], pp);
			      भङ्ग;
			}
		  }
		  edge_based_on_char(cur, row[0], पत्तनम्_गणनम्());
		  edge_based_on_char(ext0, row[0], पत्तनम्_गणनम्());
		  edge_based_on_char(ext1, row[0], पत्तनम्_गणनम्());

		  विकल्प (row[पत्तनम्_गणनम्()+1]) {
		      पक्ष '0':
			edges0_[idx_edg0++] = cur;
			यदि (need_ext0_सारणी)
			      edges0_[idx_edg0++] = ext0;
			यदि (need_ext1_सारणी)
			      edges0_[idx_edg0++] = ext1;
			भङ्ग;
		      पक्ष '1':
			edges1_[idx_edg1++] = cur;
			यदि (need_ext0_सारणी)
			      edges1_[idx_edg1++] = ext0;
			यदि (need_ext1_सारणी)
			      edges1_[idx_edg1++] = ext1;
			भङ्ग;
		      पक्ष 'x':
			भङ्ग;
		      पक्ष '-':
			धाराः_L[idx_edgL++] = cur;
			यदि (need_ext0_सारणी)
			      धाराः_L[idx_edgL++] = ext0;
			यदि (need_ext1_सारणी)
			      धाराः_L[idx_edgL++] = ext1;
			भङ्ग;
		      स्वभाव:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[पत्तनम्_गणनम्()+1], row[पत्तनम्_गणनम्()+1]);
			प्रतिज्ञा(0);
			भङ्ग;
		  }

	    }
	    विलोपय[] tab[idx];
      }
      मुक्त(tab);

      प्रतिज्ञा(idx_edg0 == nedges0_);
      प्रतिज्ञा(idx_edg1 == nedges1_);
      प्रतिज्ञा(idx_edgL == nedgesL_);

}

तार्किक संक्रियकः == (ध्रुव udp_स्तराः_सारणी&a, ध्रुव udp_स्तराः_सारणी&b)
{
      यदि (a.mask0 != b.mask0)
	    प्रत्यागम असत्य;
      यदि (a.mask1 != b.mask1)
	    प्रत्यागम असत्य;
      यदि (a.maskx != b.maskx)
	    प्रत्यागम असत्य;
      प्रत्यागम सत्य;
}

vvp_bit4_t vvp_udp_seq_s::calculate_output(ध्रुव udp_स्तराः_सारणी&cur,
					   ध्रुव udp_स्तराः_सारणी&prev,
					   vvp_bit4_t cur_बहिर्गमनम्)
{
      यदि (cur == prev)
	    प्रत्यागम cur_बहिर्गमनम्;

      udp_स्तराः_सारणी cur_tmp = cur;

      अचिह्नित दीर्घ आवरणम्_बहिर्गमनम् = 1UL << पत्तनम्_गणनम्();
      विकल्प (cur_बहिर्गमनम्) {
	  पक्ष BIT4_0:
	    cur_tmp.mask0 |= आवरणम्_बहिर्गमनम्;
	    भङ्ग;
	  पक्ष BIT4_1:
	    cur_tmp.mask1 |= आवरणम्_बहिर्गमनम्;
	    भङ्ग;
	  स्वभाव:
	    cur_tmp.maskx |= आवरणम्_बहिर्गमनम्;
	    भङ्ग;
      }

      vvp_bit4_t lev = test_स्तराः_2(cur_tmp);
      यदि (lev == BIT4_Z) {
	    lev = test_धाराः(cur_tmp, prev);
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
vvp_bit4_t vvp_udp_seq_s::test_स्तराः_2(ध्रुव udp_स्तराः_सारणी&cur)
{
      प्रति (अचिह्नित idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    यदि (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  अनुवर्त;
	    यदि (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  अनुवर्त;
	    यदि (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  अनुवर्त;

	    प्रत्यागम BIT4_0;
      }

      प्रति (अचिह्नित idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    यदि (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  अनुवर्त;
	    यदि (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  अनुवर्त;
	    यदि (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  अनुवर्त;

	    प्रत्यागम BIT4_1;
      }

	/* We need to test against an explicit X-output table, since
	   we need to distinguish from an X output and no match. */
      प्रति (अचिह्नित idx = 0 ;  idx < nlevelsx_ ;  idx += 1) {
	    यदि (cur.mask0 != (cur.mask0 & levelsx_[idx].mask0))
		  अनुवर्त;
	    यदि (cur.mask1 != (cur.mask1 & levelsx_[idx].mask1))
		  अनुवर्त;
	    यदि (cur.maskx != (cur.maskx & levelsx_[idx].maskx))
		  अनुवर्त;

	    प्रत्यागम BIT4_X;
      }

	/* Test the table that requests the next output be the same as
	   the current output. This gets the current output from the
	   levels table that was passed in. */
      प्रति (अचिह्नित idx = 0 ;  idx < nlevelsL_ ;  idx += 1) {
	    यदि (cur.mask0 != (cur.mask0 & स्तराः_L[idx].mask0))
		  अनुवर्त;
	    यदि (cur.mask1 != (cur.mask1 & स्तराः_L[idx].mask1))
		  अनुवर्त;
	    यदि (cur.maskx != (cur.maskx & स्तराः_L[idx].maskx))
		  अनुवर्त;

	    यदि (cur.mask0 & (1 << पत्तनम्_गणनम्()))
		  प्रत्यागम BIT4_0;
	    यदि (cur.mask1 & (1 << पत्तनम्_गणनम्()))
		  प्रत्यागम BIT4_1;
	    यदि (cur.maskx & (1 << पत्तनम्_गणनम्()))
		  प्रत्यागम BIT4_X;

	    प्रतिज्ञा(0);
	    प्रत्यागम BIT4_X;
      }

	/* No explicit levels entry match. Return a Z to signal that
	   further testing is needed. */
      प्रत्यागम BIT4_Z;
}

vvp_bit4_t vvp_udp_seq_s::test_धाराः(ध्रुव udp_स्तराः_सारणी&cur,
				      ध्रुव udp_स्तराः_सारणी&prev)
{
	/* The edge_mask is true for all bits that are different in
	   the cur and prev tables. */
      अचिह्नित दीर्घ edge0_आवरणम् = cur.mask0 ^ prev.mask0;
      अचिह्नित दीर्घ edgex_आवरणम् = cur.maskx ^ prev.maskx;
      अचिह्नित दीर्घ edge1_आवरणम् = cur.mask1 ^ prev.mask1;

      अचिह्नित दीर्घ edge_आवरणम् = edge0_आवरणम्|edgex_आवरणम्|edge1_आवरणम्;
      edge_आवरणम् &= ~ (-1UL << पत्तनम्_गणनम्());

	/* If there are no differences, then there are no edges. Give
	   up now. */
      यदि (edge_आवरणम् == 0)
	    प्रत्यागम BIT4_X;

      अचिह्नित edge_position = 0;
      यावत् ((edge_आवरणम्&1) == 0) {
	    edge_आवरणम् >>= 1;
	    edge_position += 1;
      }

	/* We expect that there is exactly one edge in here. */
      प्रतिज्ञा(edge_आवरणम् == 1);

      edge_आवरणम् = 1UL << edge_position;

      अचिह्नित edge_mask0 = (prev.mask0&edge_आवरणम्)? 1 : 0;
      अचिह्नित edge_maskx = (prev.maskx&edge_आवरणम्)? 1 : 0;
      अचिह्नित edge_mask1 = (prev.mask1&edge_आवरणम्)? 1 : 0;


	/* Now the edge_position and edge_mask* variables have the
	   values we use to test the applicability of the edge_table
	   entries. */

      प्रति (अचिह्नित idx = 0 ;  idx < nedges0_ ;  idx += 1) {
	    ध्रुव संरचना udp_धाराः_सारणी*row = edges0_ + idx;

	    यदि (row->edge_position != edge_position)
		  अनुवर्त;
	    यदि (edge_mask0 && !row->edge_mask0)
		  अनुवर्त;
	    यदि (edge_maskx && !row->edge_maskx)
		  अनुवर्त;
	    यदि (edge_mask1 && !row->edge_mask1)
		  अनुवर्त;
	    यदि (cur.mask0 != (cur.mask0 & row->mask0))
		  अनुवर्त;
	    यदि (cur.maskx != (cur.maskx & row->maskx))
		  अनुवर्त;
	    यदि (cur.mask1 != (cur.mask1 & row->mask1))
		  अनुवर्त;

	    प्रत्यागम BIT4_0;
      }

      प्रति (अचिह्नित idx = 0 ;  idx < nedges1_ ;  idx += 1) {
	    ध्रुव संरचना udp_धाराः_सारणी*row = edges1_ + idx;

	    यदि (row->edge_position != edge_position)
		  अनुवर्त;
	    यदि (edge_mask0 && !row->edge_mask0)
		  अनुवर्त;
	    यदि (edge_maskx && !row->edge_maskx)
		  अनुवर्त;
	    यदि (edge_mask1 && !row->edge_mask1)
		  अनुवर्त;
	    यदि (cur.mask0 != (cur.mask0 & row->mask0))
		  अनुवर्त;
	    यदि (cur.maskx != (cur.maskx & row->maskx))
		  अनुवर्त;
	    यदि (cur.mask1 != (cur.mask1 & row->mask1))
		  अनुवर्त;

	    प्रत्यागम BIT4_1;
      }

      प्रति (अचिह्नित idx = 0 ;  idx < nedgesL_ ;  idx += 1) {
	    ध्रुव संरचना udp_धाराः_सारणी*row = धाराः_L + idx;

	    यदि (row->edge_position != edge_position)
		  अनुवर्त;
	    यदि (edge_mask0 && !row->edge_mask0)
		  अनुवर्त;
	    यदि (edge_maskx && !row->edge_maskx)
		  अनुवर्त;
	    यदि (edge_mask1 && !row->edge_mask1)
		  अनुवर्त;
	    यदि (cur.mask0 != (cur.mask0 & row->mask0))
		  अनुवर्त;
	    यदि (cur.maskx != (cur.maskx & row->maskx))
		  अनुवर्त;
	    यदि (cur.mask1 != (cur.mask1 & row->mask1))
		  अनुवर्त;

	    यदि (cur.mask0 & (1 << पत्तनम्_गणनम्()))
		  प्रत्यागम BIT4_0;
	    यदि (cur.mask1 & (1 << पत्तनम्_गणनम्()))
		  प्रत्यागम BIT4_1;
	    यदि (cur.maskx & (1 << पत्तनम्_गणनम्()))
		  प्रत्यागम BIT4_X;

	    प्रतिज्ञा(0);
	    प्रत्यागम BIT4_X;
      }

      प्रत्यागम BIT4_X;
}

vvp_udp_fun_core::vvp_udp_fun_core(vvp_net_t*net, vvp_udp_s*def)
: vvp_wide_fun_core(net, def->पत्तनम्_गणनम्())
{
      def_ = def;
      cur_बहिर्गमनम्_2 = def_->get_init();
	// Assume initially that all the inputs are 1'bx
      current_.mask0 = 0;
      current_.mask1 = 0;
      current_.maskx = ~ ((-1UL) << पत्तनम्_गणनम्());

        // If the initial value is 0 or 1, schedule the initial assignment
        // normally, so that any sensitive always processes can be started
        // first.
      यदि (cur_बहिर्गमनम्_2 != BIT4_X)
	    कालसारणी_generic(एतत्, 0, असत्य);
      अन्यथा
	    कालसारणी_functor(एतत्);
}

vvp_udp_fun_core::~vvp_udp_fun_core()
{
}

/*
 * This method is used to propagate the initial value on startup.
 */
शून्य vvp_udp_fun_core::सततम्_सततम्()
{
      vvp_vector4_t tmp (1);
      tmp.समुच्चयः_bit(0, cur_बहिर्गमनम्_2);
      propagate_vec4(tmp);
}

शून्य vvp_udp_fun_core::recv_vec4_from_inputs(अचिह्नित पत्तनम्)
{
	/* For now, assume udps are 1-bit wide. */
      अचिह्नित पत्तनम्_विस्तारः = value(पत्तनम्).size();
      प्रतिज्ञा(पत्तनम्_विस्तारः == 1);

      अचिह्नित दीर्घ आवरणम् = 1UL << पत्तनम्;

      udp_स्तराः_सारणी prev = current_;

      विकल्प (value(पत्तनम्).value(0)) {

	  पक्ष BIT4_0:
	    current_.mask0 |= आवरणम्;
	    current_.mask1 &= ~आवरणम्;
	    current_.maskx &= ~आवरणम्;
	    भङ्ग;
	  पक्ष BIT4_1:
	    current_.mask0 &= ~आवरणम्;
	    current_.mask1 |= आवरणम्;
	    current_.maskx &= ~आवरणम्;
	    भङ्ग;
	  स्वभाव:
	    current_.mask0 &= ~आवरणम्;
	    current_.mask1 &= ~आवरणम्;
	    current_.maskx |= आवरणम्;
	    भङ्ग;
      }

      vvp_bit4_t बहिर्गमनम्_bit = def_->calculate_output(current_, prev, cur_बहिर्गमनम्_2);

      यदि (बहिर्गमनम्_bit == cur_बहिर्गमनम्_2)
	    प्रत्यागम;

      cur_बहिर्गमनम्_2 = बहिर्गमनम्_bit;
      कालसारणी_functor(एतत्);
}


/*
 * This function is called by the parser in response to a .udp
 * node. We create the nodes needed to integrate the UDP into the
 * netlist. The definition should be parsed already.
 */
शून्य compile_udp_functor(वर्ण*label, वर्ण*type,
			 अचिह्नित argc, संरचना symb_s*argv)
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
