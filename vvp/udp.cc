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

#ضمن "udp.h"
#ضمن "schedule.h"
#ضمن "symbols.h"
#ضمن "compile.h"
#ضمن "config.h"
#ضمن <cassert>
#ضمن <cstdlib>
#ضمن <cstring>
#ضمن <cstdio>
#ضمن <iostream>
#ifdef CHECK_WITH_VALGRIND
#ضمن "vvp_cleanup.h"
#ضمن "ivl_alloc.h"
#endif

using نطاق std;

// We may need these later when we build the VPI interface to
// UDP definitions.
#ifdef CHECK_WITH_VALGRIND
ساكن vvp_udp_s **udp_defns = 0;
ساكن unsigned udp_defns_عدد = 0;

فراغ udp_defns_delete()
{
      لكل (unsigned idx = 0; idx < udp_defns_عدد; idx += 1) {
	    إذا (udp_defns[idx]->is_sequential()) {
		  vvp_udp_seq_s *obj = static_cast<vvp_udp_seq_s *>
		                       (udp_defns[idx]);
		  delete obj;
	    } وإلا {
		  vvp_udp_comb_s *obj = static_cast<vvp_udp_comb_s *>
		                        (udp_defns[idx]);
		  delete obj;
	    }
      }
      حرر(udp_defns);
      udp_defns = 0;
      udp_defns_عدد = 0;
}
#endif

ساكن رمز_table_t udp_table;

فراغ delete_udp_symbols()
{
      delete_رمز_table(udp_table);
      udp_table = 0;
}

بنية vvp_udp_s *udp_find(ثابت حرف *label)
{
      رمز_قيمة_t v = sym_get_قيمة(udp_table, label);
      أرجع (بنية vvp_udp_s *)v.ptr;
}

ostream& operator <<(ostream&o, ثابت بنية udp_levels_table&table)
{
      o << "[" << hex << table.mask0
	<< "/" << hex << table.mask1
	<< "/" << hex << table.maskx << "]";
      أرجع o;
}

vvp_udp_s::vvp_udp_s(ثابت حرف*label, حرف*name__, unsigned ports,
                     vvp_bit4_t init, منطقي type)
: name_(name__), ports_(ports), init_(init), seq_(type)
{
      إذا (!udp_table)
	    udp_table = new_رمز_table();

      أكد( !udp_find(label) );

      رمز_قيمة_t v;
      v.ptr = هذا;
      sym_set_قيمة(udp_table, label, v);

#ifdef CHECK_WITH_VALGRIND
      udp_defns_عدد += 1;
      udp_defns = static_cast<vvp_udp_s **>(realloc(udp_defns,
                  udp_defns_عدد*حجم(vvp_udp_s **)));
      udp_defns[udp_defns_عدد-1] = هذا;
#endif
}

vvp_udp_s::~vvp_udp_s()
{
      delete[] name_;
}

unsigned vvp_udp_s::port_عدد() ثابت
{
      أرجع ports_;
}

vvp_bit4_t vvp_udp_s::get_init() ثابت
{
      أرجع init_;
}

vvp_udp_comb_s::vvp_udp_comb_s(ثابت حرف*label, حرف*name__, unsigned ports)
: vvp_udp_s(label, name__, ports, BIT4_X, خطأ)
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
vvp_bit4_t vvp_udp_comb_s::test_levels(ثابت udp_levels_table&cur)
{
	/* To test for a row match, test that the mask0, mask1 and
	   maskx vectors all have bits set where the matching
	   cur.mask0/1/x vectors have bits set. It is possible that a
	   levels0_[idx] vector has more bits set then the cur mask,
	   but this is OK and these bits are to be ignored. */

      لكل (unsigned idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    إذا (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  استمر;
	    إذا (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  استمر;
	    إذا (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  استمر;

	    أرجع BIT4_0;
      }

      لكل (unsigned idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    إذا (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  استمر;
	    إذا (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  استمر;
	    إذا (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  استمر;

	    أرجع BIT4_1;
      }

      أرجع BIT4_X;
}

vvp_bit4_t vvp_udp_comb_s::calculate_output(ثابت udp_levels_table&cur,
					    ثابت udp_levels_table&,
					    vvp_bit4_t)
{
      أرجع test_levels(cur);
}

ساكن فراغ or_based_on_char(udp_levels_table&cur, حرف flag,
			     unsigned long mask_bit)
{
      بدل (flag) {
	  حالة '0':
	    cur.mask0 |= mask_bit;
	    اكسر;
	  حالة '1':
	    cur.mask1 |= mask_bit;
	    اكسر;
	  حالة 'x':
	    cur.maskx |= mask_bit;
	    اكسر;
	  حالة 'b':
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    اكسر;
	  حالة 'l':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    اكسر;
	  حالة 'h':
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    اكسر;
	  حالة '?':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "Unsupported flag %c(%d).\n", flag, flag);
	    أكد(0);
      }
}

فراغ vvp_udp_comb_s::compile_table(حرف**tab)
{
      unsigned nrows0 = 0, nrows1 = 0;

	/* First run through the table to figure out the number of
	   rows I need for each kind of table. */
      لكل (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    أكد(strlen(tab[idx]) == port_عدد() + 1);
	    بدل (tab[idx][port_عدد()]) {
		حالة '0':
		  nrows0 += 1;
		  اكسر;
		حالة '1':
		  nrows1 += 1;
		  اكسر;
		حالة 'x':
		  اكسر;
		افتراضي:
		  fprintf(stderr, "Unsupported entry %c(%d).\n",
		          tab[idx][port_عدد()], tab[idx][port_عدد()]);
		  أكد(0);
	    }
      }

      nlevels0_ = nrows0;
      levels0_ = جديد udp_levels_table[nlevels0_];

      nlevels1_ = nrows1;
      levels1_ = جديد udp_levels_table[nlevels1_];

      nrows0 = 0;
      nrows1 = 0;
      لكل (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    بنية udp_levels_table cur;
	    cur.mask0 = 0;
	    cur.mask1 = 0;
	    cur.maskx = 0;
	    إذا (port_عدد() > 8*حجم(cur.mask0)) {
		  fprintf(stderr, "internal error: primitive port count=%u "
			  " > %zu\n", port_عدد(), حجم(cur.mask0));
		  أكد(port_عدد() <= 8*حجم(cur.mask0));
	    }
	    لكل (unsigned pp = 0 ;  pp < port_عدد() ;  pp += 1) {
		  unsigned long mask_bit = 1UL << pp;
		  or_based_on_char(cur, tab[idx][pp], mask_bit);
	    }

	    بدل (tab[idx][port_عدد()]) {
		حالة '0':
		  levels0_[nrows0++] = cur;
		  اكسر;
		حالة '1':
		  levels1_[nrows1++] = cur;
		  اكسر;
		افتراضي:
		  اكسر;
	    }
	    delete[] tab[idx];
      }
      حرر(tab);

      أكد(nrows0 == nlevels0_);
      أكد(nrows1 == nlevels1_);
}

vvp_udp_seq_s::vvp_udp_seq_s(ثابت حرف*label, حرف*name__,
			     unsigned ports, vvp_bit4_t init)
: vvp_udp_s(label, name__, ports, init, true)
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

فراغ edge_based_on_char(بنية udp_edges_table&cur, حرف chr, unsigned pos)
{
      unsigned long mask_bit = 1UL << pos;

      بدل (chr) {
	  حالة '0':
	    cur.mask0 |= mask_bit;
	    اكسر;
	  حالة '1':
	    cur.mask1 |= mask_bit;
	    اكسر;
	  حالة 'x':
	    cur.maskx |= mask_bit;
	    اكسر;
	  حالة 'b':
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    اكسر;
	  حالة 'l':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    اكسر;
	  حالة 'h':
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    اكسر;
	  حالة '?':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    اكسر;

	  حالة 'B': // (x?) edge
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    اكسر;
	  حالة 'f': // (10) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    اكسر;
	  حالة 'F': // (x0) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    اكسر;
	  حالة 'M': // (1x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    اكسر;
	  حالة 'N': // (1x) and (10) edge
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    اكسر;
	  حالة 'P': // (0x) and (01) edge
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    اكسر;
	  حالة 'q': // (bx) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    اكسر;
	  حالة 'Q': // (0x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    اكسر;
	  حالة 'r': // (01) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    اكسر;
	  حالة 'R': // (x1) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    اكسر;
	  حالة '%': // (?x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    اكسر;
	  حالة '+': // (?1) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    اكسر;
	  حالة '_': // (?0) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 1;
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "internal error: unknown edge code: %c\n", chr);
	    أكد(0);
      }
}

فراغ vvp_udp_seq_s::compile_table(حرف**tab)
{

      لكل (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    ثابت حرف*row = tab[idx];
	    أكد(strlen(row) == port_عدد() + 2);

	    إذا (strspn(row, "01xblh?") >= port_عدد()+1) {

		  بدل (row[port_عدد()+1]) {
		      حالة '0':
			nlevels0_ += 1;
			اكسر;
		      حالة '1':
			nlevels1_ += 1;
			اكسر;
		      حالة 'x':
			nlevelsx_ += 1;
			اكسر;
		      حالة '-':
			nlevelsL_ += 1;
			اكسر;
		      افتراضي:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_عدد()+1], row[port_عدد()+1]);
			أكد(0);
			اكسر;
		  }

	    } وإلا {
		    /* Rows that have n or p edges will need to be
		       expanded into 2 rows. */
		  unsigned extra = 0;
		  إذا (strchr(row,'n'))
			extra = 1;
		  إذا (strchr(row,'p'))
			extra = 1;
		  إذا (strchr(row,'*'))
			extra = 2;

		  بدل (row[port_عدد()+1]) {
		      حالة '0':
			nedges0_ += 1 + extra;
			اكسر;
		      حالة '1':
			nedges1_ += 1 + extra;
			اكسر;
		      حالة 'x':
			اكسر;
		      حالة '-':
			nedgesL_ += 1 + extra;
			اكسر;
		      افتراضي:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_عدد()+1], row[port_عدد()+1]);
			أكد(0);
			اكسر;
		  }
	    }
      }

      levels0_ = جديد udp_levels_table[nlevels0_];
      levels1_ = جديد udp_levels_table[nlevels1_];
      levelsx_ = جديد udp_levels_table[nlevelsx_];
      levelsL_ = جديد udp_levels_table[nlevelsL_];
      edges0_ = جديد udp_edges_table[nedges0_];
      edges1_ = جديد udp_edges_table[nedges1_];
      edgesL_ = جديد udp_edges_table[nedgesL_];

      unsigned idx_lev0 = 0;
      unsigned idx_lev1 = 0;
      unsigned idx_levx = 0;
      unsigned idx_levL = 0;
      unsigned idx_edg0 = 0;
      unsigned idx_edg1 = 0;
      unsigned idx_edgL = 0;

      لكل (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    ثابت حرف*row = tab[idx];

	    إذا (strspn(row, "01xblh?") >= port_عدد()+1) {
		  بنية udp_levels_table cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  لكل (unsigned pp = 0 ;  pp < port_عدد() ;  pp += 1) {
			unsigned long mask_bit = 1UL << pp;
			or_based_on_char(cur, row[pp+1], mask_bit);
		  }

		  or_based_on_char(cur, row[0], 1UL << port_عدد());

		  بدل (row[port_عدد()+1]) {
		      حالة '0':
			levels0_[idx_lev0++] = cur;
			اكسر;
		      حالة '1':
			levels1_[idx_lev1++] = cur;
			اكسر;
		      حالة 'x':
			levelsx_[idx_levx++] = cur;
			اكسر;
		      حالة '-':
			levelsL_[idx_levL++] = cur;
			اكسر;
		      افتراضي:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_عدد()+1], row[port_عدد()+1]);
			أكد(0);
			اكسر;
		  }

	    } وإلا {
		  بنية udp_edges_table cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  cur.edge_position = 0;
		  cur.edge_mask0 = 0;
		  cur.edge_mask1 = 0;
		  cur.edge_maskx = 0;

		  منطقي need_ext0_table = خطأ;
		  بنية udp_edges_table ext0;
		  ext0.mask0 = 0;
		  ext0.mask1 = 0;
		  ext0.maskx = 0;
		  ext0.edge_position = 0;
		  ext0.edge_mask0 = 0;
		  ext0.edge_mask1 = 0;
		  ext0.edge_maskx = 0;

		  منطقي need_ext1_table = خطأ;
		  بنية udp_edges_table ext1;
		  ext1.mask0 = 0;
		  ext1.mask1 = 0;
		  ext1.maskx = 0;
		  ext1.edge_position = 0;
		  ext1.edge_mask0 = 0;
		  ext1.edge_mask1 = 0;
		  ext1.edge_maskx = 0;

		  لكل (unsigned pp = 0 ;  pp < port_عدد() ; pp += 1) {
			بدل (row[pp+1]) {
			    حالة 'n':
			      edge_based_on_char(cur, 'N', pp);
			      edge_based_on_char(ext0, '_', pp);
			      need_ext0_table = true;
			      اكسر;
			    حالة 'p':
			      edge_based_on_char(cur, 'P', pp);
			      edge_based_on_char(ext0, '+', pp);
			      need_ext0_table = true;
			      اكسر;
			    حالة '*':
			      edge_based_on_char(cur, 'P', pp);
			      edge_based_on_char(ext0, 'N', pp);
			      edge_based_on_char(ext1, 'B', pp);
			      need_ext0_table = true;
			      need_ext1_table = true;
			      اكسر;
			    افتراضي:
			      edge_based_on_char(cur, row[pp+1], pp);
			      edge_based_on_char(ext0, row[pp+1], pp);
			      edge_based_on_char(ext1, row[pp+1], pp);
			      اكسر;
			}
		  }
		  edge_based_on_char(cur, row[0], port_عدد());
		  edge_based_on_char(ext0, row[0], port_عدد());
		  edge_based_on_char(ext1, row[0], port_عدد());

		  بدل (row[port_عدد()+1]) {
		      حالة '0':
			edges0_[idx_edg0++] = cur;
			إذا (need_ext0_table)
			      edges0_[idx_edg0++] = ext0;
			إذا (need_ext1_table)
			      edges0_[idx_edg0++] = ext1;
			اكسر;
		      حالة '1':
			edges1_[idx_edg1++] = cur;
			إذا (need_ext0_table)
			      edges1_[idx_edg1++] = ext0;
			إذا (need_ext1_table)
			      edges1_[idx_edg1++] = ext1;
			اكسر;
		      حالة 'x':
			اكسر;
		      حالة '-':
			edgesL_[idx_edgL++] = cur;
			إذا (need_ext0_table)
			      edgesL_[idx_edgL++] = ext0;
			إذا (need_ext1_table)
			      edgesL_[idx_edgL++] = ext1;
			اكسر;
		      افتراضي:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_عدد()+1], row[port_عدد()+1]);
			أكد(0);
			اكسر;
		  }

	    }
	    delete[] tab[idx];
      }
      حرر(tab);

      أكد(idx_edg0 == nedges0_);
      أكد(idx_edg1 == nedges1_);
      أكد(idx_edgL == nedgesL_);

}

منطقي operator == (ثابت udp_levels_table&a, ثابت udp_levels_table&b)
{
      إذا (a.mask0 != b.mask0)
	    أرجع خطأ;
      إذا (a.mask1 != b.mask1)
	    أرجع خطأ;
      إذا (a.maskx != b.maskx)
	    أرجع خطأ;
      أرجع true;
}

vvp_bit4_t vvp_udp_seq_s::calculate_output(ثابت udp_levels_table&cur,
					   ثابت udp_levels_table&prev,
					   vvp_bit4_t cur_out)
{
      إذا (cur == prev)
	    أرجع cur_out;

      udp_levels_table cur_tmp = cur;

      unsigned long mask_out = 1UL << port_عدد();
      بدل (cur_out) {
	  حالة BIT4_0:
	    cur_tmp.mask0 |= mask_out;
	    اكسر;
	  حالة BIT4_1:
	    cur_tmp.mask1 |= mask_out;
	    اكسر;
	  افتراضي:
	    cur_tmp.maskx |= mask_out;
	    اكسر;
      }

      vvp_bit4_t lev = test_levels_(cur_tmp);
      إذا (lev == BIT4_Z) {
	    lev = test_edges_(cur_tmp, prev);
      }

      أرجع lev;
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
vvp_bit4_t vvp_udp_seq_s::test_levels_(ثابت udp_levels_table&cur)
{
      لكل (unsigned idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    إذا (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  استمر;
	    إذا (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  استمر;
	    إذا (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  استمر;

	    أرجع BIT4_0;
      }

      لكل (unsigned idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    إذا (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  استمر;
	    إذا (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  استمر;
	    إذا (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  استمر;

	    أرجع BIT4_1;
      }

	/* We need to test against an explicit X-output table, since
	   we need to distinguish from an X output and no match. */
      لكل (unsigned idx = 0 ;  idx < nlevelsx_ ;  idx += 1) {
	    إذا (cur.mask0 != (cur.mask0 & levelsx_[idx].mask0))
		  استمر;
	    إذا (cur.mask1 != (cur.mask1 & levelsx_[idx].mask1))
		  استمر;
	    إذا (cur.maskx != (cur.maskx & levelsx_[idx].maskx))
		  استمر;

	    أرجع BIT4_X;
      }

	/* Test the table that requests the next output be the same as
	   the current output. This gets the current output from the
	   levels table that was passed in. */
      لكل (unsigned idx = 0 ;  idx < nlevelsL_ ;  idx += 1) {
	    إذا (cur.mask0 != (cur.mask0 & levelsL_[idx].mask0))
		  استمر;
	    إذا (cur.mask1 != (cur.mask1 & levelsL_[idx].mask1))
		  استمر;
	    إذا (cur.maskx != (cur.maskx & levelsL_[idx].maskx))
		  استمر;

	    إذا (cur.mask0 & (1 << port_عدد()))
		  أرجع BIT4_0;
	    إذا (cur.mask1 & (1 << port_عدد()))
		  أرجع BIT4_1;
	    إذا (cur.maskx & (1 << port_عدد()))
		  أرجع BIT4_X;

	    أكد(0);
	    أرجع BIT4_X;
      }

	/* No explicit levels entry match. Return a Z to signal that
	   further testing is needed. */
      أرجع BIT4_Z;
}

vvp_bit4_t vvp_udp_seq_s::test_edges_(ثابت udp_levels_table&cur,
				      ثابت udp_levels_table&prev)
{
	/* The edge_mask is true for all bits that are different in
	   the cur and prev tables. */
      unsigned long edge0_mask = cur.mask0 ^ prev.mask0;
      unsigned long edgex_mask = cur.maskx ^ prev.maskx;
      unsigned long edge1_mask = cur.mask1 ^ prev.mask1;

      unsigned long edge_mask = edge0_mask|edgex_mask|edge1_mask;
      edge_mask &= ~ (-1UL << port_عدد());

	/* If there are no differences, then there are no edges. Give
	   up now. */
      إذا (edge_mask == 0)
	    أرجع BIT4_X;

      unsigned edge_position = 0;
      طالما ((edge_mask&1) == 0) {
	    edge_mask >>= 1;
	    edge_position += 1;
      }

	/* We expect that there is exactly one edge in here. */
      أكد(edge_mask == 1);

      edge_mask = 1UL << edge_position;

      unsigned edge_mask0 = (prev.mask0&edge_mask)? 1 : 0;
      unsigned edge_maskx = (prev.maskx&edge_mask)? 1 : 0;
      unsigned edge_mask1 = (prev.mask1&edge_mask)? 1 : 0;


	/* Now the edge_position and edge_mask* variables have the
	   values we use to test the applicability of the edge_table
	   entries. */

      لكل (unsigned idx = 0 ;  idx < nedges0_ ;  idx += 1) {
	    ثابت بنية udp_edges_table*row = edges0_ + idx;

	    إذا (row->edge_position != edge_position)
		  استمر;
	    إذا (edge_mask0 && !row->edge_mask0)
		  استمر;
	    إذا (edge_maskx && !row->edge_maskx)
		  استمر;
	    إذا (edge_mask1 && !row->edge_mask1)
		  استمر;
	    إذا (cur.mask0 != (cur.mask0 & row->mask0))
		  استمر;
	    إذا (cur.maskx != (cur.maskx & row->maskx))
		  استمر;
	    إذا (cur.mask1 != (cur.mask1 & row->mask1))
		  استمر;

	    أرجع BIT4_0;
      }

      لكل (unsigned idx = 0 ;  idx < nedges1_ ;  idx += 1) {
	    ثابت بنية udp_edges_table*row = edges1_ + idx;

	    إذا (row->edge_position != edge_position)
		  استمر;
	    إذا (edge_mask0 && !row->edge_mask0)
		  استمر;
	    إذا (edge_maskx && !row->edge_maskx)
		  استمر;
	    إذا (edge_mask1 && !row->edge_mask1)
		  استمر;
	    إذا (cur.mask0 != (cur.mask0 & row->mask0))
		  استمر;
	    إذا (cur.maskx != (cur.maskx & row->maskx))
		  استمر;
	    إذا (cur.mask1 != (cur.mask1 & row->mask1))
		  استمر;

	    أرجع BIT4_1;
      }

      لكل (unsigned idx = 0 ;  idx < nedgesL_ ;  idx += 1) {
	    ثابت بنية udp_edges_table*row = edgesL_ + idx;

	    إذا (row->edge_position != edge_position)
		  استمر;
	    إذا (edge_mask0 && !row->edge_mask0)
		  استمر;
	    إذا (edge_maskx && !row->edge_maskx)
		  استمر;
	    إذا (edge_mask1 && !row->edge_mask1)
		  استمر;
	    إذا (cur.mask0 != (cur.mask0 & row->mask0))
		  استمر;
	    إذا (cur.maskx != (cur.maskx & row->maskx))
		  استمر;
	    إذا (cur.mask1 != (cur.mask1 & row->mask1))
		  استمر;

	    إذا (cur.mask0 & (1 << port_عدد()))
		  أرجع BIT4_0;
	    إذا (cur.mask1 & (1 << port_عدد()))
		  أرجع BIT4_1;
	    إذا (cur.maskx & (1 << port_عدد()))
		  أرجع BIT4_X;

	    أكد(0);
	    أرجع BIT4_X;
      }

      أرجع BIT4_X;
}

vvp_udp_fun_core::vvp_udp_fun_core(vvp_net_t*net, vvp_udp_s*def)
: vvp_wide_fun_core(net, def->port_عدد())
{
      def_ = def;
      cur_out_ = def_->get_init();
	// Assume initially that all the inputs are 1'bx
      current_.mask0 = 0;
      current_.mask1 = 0;
      current_.maskx = ~ ((-1UL) << port_عدد());

        // If the initial value is 0 or 1, schedule the initial assignment
        // normally, so that any sensitive always processes can be started
        // first.
      إذا (cur_out_ != BIT4_X)
	    schedule_generic(هذا, 0, خطأ);
      وإلا
	    schedule_functor(هذا);
}

vvp_udp_fun_core::~vvp_udp_fun_core()
{
}

/*
 * This method is used to propagate the initial value on startup.
 */
فراغ vvp_udp_fun_core::run_run()
{
      vvp_vector4_t tmp (1);
      tmp.set_bit(0, cur_out_);
      propagate_vec4(tmp);
}

فراغ vvp_udp_fun_core::recv_vec4_from_inputs(unsigned port)
{
	/* For now, assume udps are 1-bit wide. */
      unsigned port_width = value(port).size();
      أكد(port_width == 1);

      unsigned long mask = 1UL << port;

      udp_levels_table prev = current_;

      بدل (value(port).value(0)) {

	  حالة BIT4_0:
	    current_.mask0 |= mask;
	    current_.mask1 &= ~mask;
	    current_.maskx &= ~mask;
	    اكسر;
	  حالة BIT4_1:
	    current_.mask0 &= ~mask;
	    current_.mask1 |= mask;
	    current_.maskx &= ~mask;
	    اكسر;
	  افتراضي:
	    current_.mask0 &= ~mask;
	    current_.mask1 &= ~mask;
	    current_.maskx |= mask;
	    اكسر;
      }

      vvp_bit4_t out_bit = def_->calculate_output(current_, prev, cur_out_);

      إذا (out_bit == cur_out_)
	    أرجع;

      cur_out_ = out_bit;
      schedule_functor(هذا);
}


/*
 * This function is called by the parser in response to a .udp
 * node. We create the nodes needed to integrate the UDP into the
 * netlist. The definition should be parsed already.
 */
فراغ compile_udp_functor(حرف*label, حرف*type,
			 unsigned argc, بنية symb_s*argv)
{
      بنية vvp_udp_s *def = udp_find(type);
      أكد(def);
      حرر(type);

      vvp_net_t*ptr = جديد vvp_net_t;
      vvp_udp_fun_core*core = جديد vvp_udp_fun_core(ptr, def);
      ptr->fun = core;

      define_functor_رمز(label, ptr);
      حرر(label);

      wide_inputs_connect(core, argc, argv);
      حرر(argv);
}
