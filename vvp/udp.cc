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
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
#ضمن "vvp_cleanup.h"
#ضمن "ivl_alloc.h"
#جے_انت

ورتوں نطاق std;

// We may need these later when we build the VPI interface to
// UDP definitions.
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
ساكن vvp_udp_s **udp_defns = 0;
ساكن بنانشانی udp_defns_گنتی = 0;

فراغ udp_defns_delete()
{
      لكل (بنانشانی idx = 0; idx < udp_defns_گنتی; idx += 1) {
	    إذا (udp_defns[idx]->is_sequential()) {
		  vvp_udp_seq_s *obj = static_cast<vvp_udp_seq_s *>
		                       (udp_defns[idx]);
		  مٹاؤ obj;
	    } وإلا {
		  vvp_udp_comb_s *obj = static_cast<vvp_udp_comb_s *>
		                        (udp_defns[idx]);
		  مٹاؤ obj;
	    }
      }
      حرر(udp_defns);
      udp_defns = 0;
      udp_defns_گنتی = 0;
}
#جے_انت

ساكن علامت_جدول_t udp_جدول;

فراغ delete_udp_symbols()
{
      delete_علامت_جدول(udp_جدول);
      udp_جدول = 0;
}

بنية vvp_udp_s *udp_لبھو(ثابت حرف *label)
{
      علامت_قدر_t v = sym_get_قدر(udp_جدول, label);
      أرجع (بنية vvp_udp_s *)v.ptr;
}

ostream& عامل <<(ostream&o, ثابت بنية udp_پدھراں_جدول&جدول)
{
      o << "[" << hex << جدول.mask0
	<< "/" << hex << جدول.mask1
	<< "/" << hex << جدول.maskx << "]";
      أرجع o;
}

vvp_udp_s::vvp_udp_s(ثابت حرف*label, حرف*name__, بنانشانی ports,
                     vvp_bit4_t init, منطقي type)
: name_(name__), ports_(ports), init_(init), seq_(type)
{
      إذا (!udp_جدول)
	    udp_جدول = new_علامت_جدول();

      أكد( !udp_لبھو(label) );

      علامت_قدر_t v;
      v.ptr = هذا;
      sym_سیٹ_قدر(udp_جدول, label, v);

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
      udp_defns_گنتی += 1;
      udp_defns = static_cast<vvp_udp_s **>(realloc(udp_defns,
                  udp_defns_گنتی*حجم(vvp_udp_s **)));
      udp_defns[udp_defns_گنتی-1] = هذا;
#جے_انت
}

vvp_udp_s::~vvp_udp_s()
{
      مٹاؤ[] name_;
}

بنانشانی vvp_udp_s::پورٹ_گنتی() ثابت
{
      أرجع ports_;
}

vvp_bit4_t vvp_udp_s::get_init() ثابت
{
      أرجع init_;
}

vvp_udp_comb_s::vvp_udp_comb_s(ثابت حرف*label, حرف*name__, بنانشانی ports)
: vvp_udp_s(label, name__, ports, BIT4_X, خطأ)
{
      levels0_ = 0;
      levels1_ = 0;
      nlevels0_ = 0;
      nlevels1_ = 0;
}

vvp_udp_comb_s::~vvp_udp_comb_s()
{
      مٹاؤ[] levels0_;
      مٹاؤ[] levels1_;
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
vvp_bit4_t vvp_udp_comb_s::test_پدھراں(ثابت udp_پدھراں_جدول&cur)
{
	/* To test for a row match, test that the mask0, mask1 and
	   maskx vectors all have bits set where the matching
	   cur.mask0/1/x vectors have bits set. It is possible that a
	   levels0_[idx] vector has more bits set then the cur mask,
	   but this is OK and these bits are to be ignored. */

      لكل (بنانشانی idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    إذا (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  استمر;
	    إذا (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  استمر;
	    إذا (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  استمر;

	    أرجع BIT4_0;
      }

      لكل (بنانشانی idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
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

vvp_bit4_t vvp_udp_comb_s::calculate_output(ثابت udp_پدھراں_جدول&cur,
					    ثابت udp_پدھراں_جدول&,
					    vvp_bit4_t)
{
      أرجع test_پدھراں(cur);
}

ساكن فراغ or_based_on_char(udp_پدھراں_جدول&cur, حرف flag,
			     بنانشانی لما نقاب_ثنائی_ہندسہ)
{
      بدل (flag) {
	  حالة '0':
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    اكسر;
	  حالة '1':
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    اكسر;
	  حالة 'x':
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    اكسر;
	  حالة 'b':
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    اكسر;
	  حالة 'l':
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    اكسر;
	  حالة 'h':
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    اكسر;
	  حالة '?':
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "Unsupported flag %c(%d).\n", flag, flag);
	    أكد(0);
      }
}

فراغ vvp_udp_comb_s::compile_جدول(حرف**tab)
{
      بنانشانی nrows0 = 0, nrows1 = 0;

	/* First run through the table to figure out the number of
	   rows I need for each kind of table. */
      لكل (بنانشانی idx = 0 ;  tab[idx] ;  idx += 1) {
	    أكد(لڑی_لمبائی(tab[idx]) == پورٹ_گنتی() + 1);
	    بدل (tab[idx][پورٹ_گنتی()]) {
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
		          tab[idx][پورٹ_گنتی()], tab[idx][پورٹ_گنتی()]);
		  أكد(0);
	    }
      }

      nlevels0_ = nrows0;
      levels0_ = جديد udp_پدھراں_جدول[nlevels0_];

      nlevels1_ = nrows1;
      levels1_ = جديد udp_پدھراں_جدول[nlevels1_];

      nrows0 = 0;
      nrows1 = 0;
      لكل (بنانشانی idx = 0 ;  tab[idx] ;  idx += 1) {
	    بنية udp_پدھراں_جدول cur;
	    cur.mask0 = 0;
	    cur.mask1 = 0;
	    cur.maskx = 0;
	    إذا (پورٹ_گنتی() > 8*حجم(cur.mask0)) {
		  fprintf(stderr, "internal error: primitive port count=%u "
			  " > %zu\n", پورٹ_گنتی(), حجم(cur.mask0));
		  أكد(پورٹ_گنتی() <= 8*حجم(cur.mask0));
	    }
	    لكل (بنانشانی pp = 0 ;  pp < پورٹ_گنتی() ;  pp += 1) {
		  بنانشانی لما نقاب_ثنائی_ہندسہ = 1UL << pp;
		  or_based_on_char(cur, tab[idx][pp], نقاب_ثنائی_ہندسہ);
	    }

	    بدل (tab[idx][پورٹ_گنتی()]) {
		حالة '0':
		  levels0_[nrows0++] = cur;
		  اكسر;
		حالة '1':
		  levels1_[nrows1++] = cur;
		  اكسر;
		افتراضي:
		  اكسر;
	    }
	    مٹاؤ[] tab[idx];
      }
      حرر(tab);

      أكد(nrows0 == nlevels0_);
      أكد(nrows1 == nlevels1_);
}

vvp_udp_seq_s::vvp_udp_seq_s(ثابت حرف*label, حرف*name__,
			     بنانشانی ports, vvp_bit4_t init)
: vvp_udp_s(label, name__, ports, init, سچ)
{
      levels0_ = 0;
      levels1_ = 0;
      levelsx_ = 0;
      پدھراں_L = 0;

      nlevels0_ = 0;
      nlevels1_ = 0;
      nlevelsx_ = 0;
      nlevelsL_ = 0;

      edges0_ = 0;
      edges1_ = 0;
      کنارے_L = 0;

      nedges0_ = 0;
      nedges1_ = 0;
      nedgesL_ = 0;
}

vvp_udp_seq_s::~vvp_udp_seq_s()
{
      مٹاؤ[] levels0_;
      مٹاؤ[] levels1_;
      مٹاؤ[] levelsx_;
      مٹاؤ[] پدھراں_L;
      مٹاؤ[] edges0_;
      مٹاؤ[] edges1_;
      مٹاؤ[] کنارے_L;
}

فراغ کنارا_based_on_char(بنية udp_کنارے_جدول&cur, حرف chr, بنانشانی pos)
{
      بنانشانی لما نقاب_ثنائی_ہندسہ = 1UL << pos;

      بدل (chr) {
	  حالة '0':
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    اكسر;
	  حالة '1':
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    اكسر;
	  حالة 'x':
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    اكسر;
	  حالة 'b':
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    اكسر;
	  حالة 'l':
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    اكسر;
	  حالة 'h':
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    اكسر;
	  حالة '?':
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    اكسر;

	  حالة 'B': // (x?) edge
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    cur.کنارا_position = pos;
	    cur.کنارا_mask0 = 0;
	    cur.کنارا_maskx = 1;
	    cur.کنارا_mask1 = 0;
	    اكسر;
	  حالة 'f': // (10) edge
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.کنارا_position = pos;
	    cur.کنارا_mask0 = 0;
	    cur.کنارا_maskx = 0;
	    cur.کنارا_mask1 = 1;
	    اكسر;
	  حالة 'F': // (x0) edge
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.کنارا_position = pos;
	    cur.کنارا_mask0 = 0;
	    cur.کنارا_maskx = 1;
	    cur.کنارا_mask1 = 0;
	    اكسر;
	  حالة 'M': // (1x) edge
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    cur.کنارا_position = pos;
	    cur.کنارا_mask0 = 0;
	    cur.کنارا_maskx = 0;
	    cur.کنارا_mask1 = 1;
	    اكسر;
	  حالة 'N': // (1x) and (10) edge
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    cur.کنارا_position = pos;
	    cur.کنارا_mask0 = 0;
	    cur.کنارا_maskx = 0;
	    cur.کنارا_mask1 = 1;
	    اكسر;
	  حالة 'P': // (0x) and (01) edge
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    cur.کنارا_position = pos;
	    cur.کنارا_mask0 = 1;
	    cur.کنارا_maskx = 0;
	    cur.کنارا_mask1 = 0;
	    اكسر;
	  حالة 'q': // (bx) edge
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    cur.کنارا_position = pos;
	    cur.کنارا_mask0 = 1;
	    cur.کنارا_maskx = 0;
	    cur.کنارا_mask1 = 1;
	    اكسر;
	  حالة 'Q': // (0x) edge
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    cur.کنارا_position = pos;
	    cur.کنارا_mask0 = 1;
	    cur.کنارا_maskx = 0;
	    cur.کنارا_mask1 = 0;
	    اكسر;
	  حالة 'r': // (01) edge
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    cur.کنارا_position = pos;
	    cur.کنارا_mask0 = 1;
	    cur.کنارا_maskx = 0;
	    cur.کنارا_mask1 = 0;
	    اكسر;
	  حالة 'R': // (x1) edge
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    cur.کنارا_position = pos;
	    cur.کنارا_mask0 = 0;
	    cur.کنارا_maskx = 1;
	    cur.کنارا_mask1 = 0;
	    اكسر;
	  حالة '%': // (?x) edge
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    cur.کنارا_position = pos;
	    cur.کنارا_mask0 = 1;
	    cur.کنارا_maskx = 0;
	    cur.کنارا_mask1 = 1;
	    اكسر;
	  حالة '+': // (?1) edge
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    cur.کنارا_position = pos;
	    cur.کنارا_mask0 = 1;
	    cur.کنارا_maskx = 1;
	    cur.کنارا_mask1 = 0;
	    اكسر;
	  حالة '_': // (?0) edge
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.کنارا_position = pos;
	    cur.کنارا_mask0 = 0;
	    cur.کنارا_maskx = 1;
	    cur.کنارا_mask1 = 1;
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "internal error: unknown edge code: %c\n", chr);
	    أكد(0);
      }
}

فراغ vvp_udp_seq_s::compile_جدول(حرف**tab)
{

      لكل (بنانشانی idx = 0 ;  tab[idx] ;  idx += 1) {
	    ثابت حرف*صف = tab[idx];
	    أكد(لڑی_لمبائی(صف) == پورٹ_گنتی() + 2);

	    إذا (strspn(صف, "01xblh?") >= پورٹ_گنتی()+1) {

		  بدل (صف[پورٹ_گنتی()+1]) {
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
			        صف[پورٹ_گنتی()+1], صف[پورٹ_گنتی()+1]);
			أكد(0);
			اكسر;
		  }

	    } وإلا {
		    /* Rows that have n or p edges will need to be
		       expanded into 2 rows. */
		  بنانشانی extra = 0;
		  إذا (strchr(صف,'n'))
			extra = 1;
		  إذا (strchr(صف,'p'))
			extra = 1;
		  إذا (strchr(صف,'*'))
			extra = 2;

		  بدل (صف[پورٹ_گنتی()+1]) {
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
			        صف[پورٹ_گنتی()+1], صف[پورٹ_گنتی()+1]);
			أكد(0);
			اكسر;
		  }
	    }
      }

      levels0_ = جديد udp_پدھراں_جدول[nlevels0_];
      levels1_ = جديد udp_پدھراں_جدول[nlevels1_];
      levelsx_ = جديد udp_پدھراں_جدول[nlevelsx_];
      پدھراں_L = جديد udp_پدھراں_جدول[nlevelsL_];
      edges0_ = جديد udp_کنارے_جدول[nedges0_];
      edges1_ = جديد udp_کنارے_جدول[nedges1_];
      کنارے_L = جديد udp_کنارے_جدول[nedgesL_];

      بنانشانی idx_lev0 = 0;
      بنانشانی idx_lev1 = 0;
      بنانشانی idx_levx = 0;
      بنانشانی idx_levL = 0;
      بنانشانی idx_edg0 = 0;
      بنانشانی idx_edg1 = 0;
      بنانشانی idx_edgL = 0;

      لكل (بنانشانی idx = 0 ;  tab[idx] ;  idx += 1) {
	    ثابت حرف*صف = tab[idx];

	    إذا (strspn(صف, "01xblh?") >= پورٹ_گنتی()+1) {
		  بنية udp_پدھراں_جدول cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  لكل (بنانشانی pp = 0 ;  pp < پورٹ_گنتی() ;  pp += 1) {
			بنانشانی لما نقاب_ثنائی_ہندسہ = 1UL << pp;
			or_based_on_char(cur, صف[pp+1], نقاب_ثنائی_ہندسہ);
		  }

		  or_based_on_char(cur, صف[0], 1UL << پورٹ_گنتی());

		  بدل (صف[پورٹ_گنتی()+1]) {
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
			پدھراں_L[idx_levL++] = cur;
			اكسر;
		      افتراضي:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        صف[پورٹ_گنتی()+1], صف[پورٹ_گنتی()+1]);
			أكد(0);
			اكسر;
		  }

	    } وإلا {
		  بنية udp_کنارے_جدول cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  cur.کنارا_position = 0;
		  cur.کنارا_mask0 = 0;
		  cur.کنارا_mask1 = 0;
		  cur.کنارا_maskx = 0;

		  منطقي need_ext0_جدول = خطأ;
		  بنية udp_کنارے_جدول ext0;
		  ext0.mask0 = 0;
		  ext0.mask1 = 0;
		  ext0.maskx = 0;
		  ext0.کنارا_position = 0;
		  ext0.کنارا_mask0 = 0;
		  ext0.کنارا_mask1 = 0;
		  ext0.کنارا_maskx = 0;

		  منطقي need_ext1_جدول = خطأ;
		  بنية udp_کنارے_جدول ext1;
		  ext1.mask0 = 0;
		  ext1.mask1 = 0;
		  ext1.maskx = 0;
		  ext1.کنارا_position = 0;
		  ext1.کنارا_mask0 = 0;
		  ext1.کنارا_mask1 = 0;
		  ext1.کنارا_maskx = 0;

		  لكل (بنانشانی pp = 0 ;  pp < پورٹ_گنتی() ; pp += 1) {
			بدل (صف[pp+1]) {
			    حالة 'n':
			      کنارا_based_on_char(cur, 'N', pp);
			      کنارا_based_on_char(ext0, '_', pp);
			      need_ext0_جدول = سچ;
			      اكسر;
			    حالة 'p':
			      کنارا_based_on_char(cur, 'P', pp);
			      کنارا_based_on_char(ext0, '+', pp);
			      need_ext0_جدول = سچ;
			      اكسر;
			    حالة '*':
			      کنارا_based_on_char(cur, 'P', pp);
			      کنارا_based_on_char(ext0, 'N', pp);
			      کنارا_based_on_char(ext1, 'B', pp);
			      need_ext0_جدول = سچ;
			      need_ext1_جدول = سچ;
			      اكسر;
			    افتراضي:
			      کنارا_based_on_char(cur, صف[pp+1], pp);
			      کنارا_based_on_char(ext0, صف[pp+1], pp);
			      کنارا_based_on_char(ext1, صف[pp+1], pp);
			      اكسر;
			}
		  }
		  کنارا_based_on_char(cur, صف[0], پورٹ_گنتی());
		  کنارا_based_on_char(ext0, صف[0], پورٹ_گنتی());
		  کنارا_based_on_char(ext1, صف[0], پورٹ_گنتی());

		  بدل (صف[پورٹ_گنتی()+1]) {
		      حالة '0':
			edges0_[idx_edg0++] = cur;
			إذا (need_ext0_جدول)
			      edges0_[idx_edg0++] = ext0;
			إذا (need_ext1_جدول)
			      edges0_[idx_edg0++] = ext1;
			اكسر;
		      حالة '1':
			edges1_[idx_edg1++] = cur;
			إذا (need_ext0_جدول)
			      edges1_[idx_edg1++] = ext0;
			إذا (need_ext1_جدول)
			      edges1_[idx_edg1++] = ext1;
			اكسر;
		      حالة 'x':
			اكسر;
		      حالة '-':
			کنارے_L[idx_edgL++] = cur;
			إذا (need_ext0_جدول)
			      کنارے_L[idx_edgL++] = ext0;
			إذا (need_ext1_جدول)
			      کنارے_L[idx_edgL++] = ext1;
			اكسر;
		      افتراضي:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        صف[پورٹ_گنتی()+1], صف[پورٹ_گنتی()+1]);
			أكد(0);
			اكسر;
		  }

	    }
	    مٹاؤ[] tab[idx];
      }
      حرر(tab);

      أكد(idx_edg0 == nedges0_);
      أكد(idx_edg1 == nedges1_);
      أكد(idx_edgL == nedgesL_);

}

منطقي عامل == (ثابت udp_پدھراں_جدول&a, ثابت udp_پدھراں_جدول&b)
{
      إذا (a.mask0 != b.mask0)
	    أرجع خطأ;
      إذا (a.mask1 != b.mask1)
	    أرجع خطأ;
      إذا (a.maskx != b.maskx)
	    أرجع خطأ;
      أرجع سچ;
}

vvp_bit4_t vvp_udp_seq_s::calculate_output(ثابت udp_پدھراں_جدول&cur,
					   ثابت udp_پدھراں_جدول&prev,
					   vvp_bit4_t cur_باہرلا)
{
      إذا (cur == prev)
	    أرجع cur_باہرلا;

      udp_پدھراں_جدول cur_tmp = cur;

      بنانشانی لما نقاب_باہرلا = 1UL << پورٹ_گنتی();
      بدل (cur_باہرلا) {
	  حالة BIT4_0:
	    cur_tmp.mask0 |= نقاب_باہرلا;
	    اكسر;
	  حالة BIT4_1:
	    cur_tmp.mask1 |= نقاب_باہرلا;
	    اكسر;
	  افتراضي:
	    cur_tmp.maskx |= نقاب_باہرلا;
	    اكسر;
      }

      vvp_bit4_t lev = test_پدھراں_2(cur_tmp);
      إذا (lev == BIT4_Z) {
	    lev = test_کنارے(cur_tmp, prev);
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
vvp_bit4_t vvp_udp_seq_s::test_پدھراں_2(ثابت udp_پدھراں_جدول&cur)
{
      لكل (بنانشانی idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    إذا (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  استمر;
	    إذا (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  استمر;
	    إذا (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  استمر;

	    أرجع BIT4_0;
      }

      لكل (بنانشانی idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
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
      لكل (بنانشانی idx = 0 ;  idx < nlevelsx_ ;  idx += 1) {
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
      لكل (بنانشانی idx = 0 ;  idx < nlevelsL_ ;  idx += 1) {
	    إذا (cur.mask0 != (cur.mask0 & پدھراں_L[idx].mask0))
		  استمر;
	    إذا (cur.mask1 != (cur.mask1 & پدھراں_L[idx].mask1))
		  استمر;
	    إذا (cur.maskx != (cur.maskx & پدھراں_L[idx].maskx))
		  استمر;

	    إذا (cur.mask0 & (1 << پورٹ_گنتی()))
		  أرجع BIT4_0;
	    إذا (cur.mask1 & (1 << پورٹ_گنتی()))
		  أرجع BIT4_1;
	    إذا (cur.maskx & (1 << پورٹ_گنتی()))
		  أرجع BIT4_X;

	    أكد(0);
	    أرجع BIT4_X;
      }

	/* No explicit levels entry match. Return a Z to signal that
	   further testing is needed. */
      أرجع BIT4_Z;
}

vvp_bit4_t vvp_udp_seq_s::test_کنارے(ثابت udp_پدھراں_جدول&cur,
				      ثابت udp_پدھراں_جدول&prev)
{
	/* The edge_mask is true for all bits that are different in
	   the cur and prev tables. */
      بنانشانی لما edge0_نقاب = cur.mask0 ^ prev.mask0;
      بنانشانی لما edgex_نقاب = cur.maskx ^ prev.maskx;
      بنانشانی لما edge1_نقاب = cur.mask1 ^ prev.mask1;

      بنانشانی لما کنارا_نقاب = edge0_نقاب|edgex_نقاب|edge1_نقاب;
      کنارا_نقاب &= ~ (-1UL << پورٹ_گنتی());

	/* If there are no differences, then there are no edges. Give
	   up now. */
      إذا (کنارا_نقاب == 0)
	    أرجع BIT4_X;

      بنانشانی کنارا_position = 0;
      طالما ((کنارا_نقاب&1) == 0) {
	    کنارا_نقاب >>= 1;
	    کنارا_position += 1;
      }

	/* We expect that there is exactly one edge in here. */
      أكد(کنارا_نقاب == 1);

      کنارا_نقاب = 1UL << کنارا_position;

      بنانشانی کنارا_mask0 = (prev.mask0&کنارا_نقاب)? 1 : 0;
      بنانشانی کنارا_maskx = (prev.maskx&کنارا_نقاب)? 1 : 0;
      بنانشانی کنارا_mask1 = (prev.mask1&کنارا_نقاب)? 1 : 0;


	/* Now the edge_position and edge_mask* variables have the
	   values we use to test the applicability of the edge_table
	   entries. */

      لكل (بنانشانی idx = 0 ;  idx < nedges0_ ;  idx += 1) {
	    ثابت بنية udp_کنارے_جدول*صف = edges0_ + idx;

	    إذا (صف->کنارا_position != کنارا_position)
		  استمر;
	    إذا (کنارا_mask0 && !صف->کنارا_mask0)
		  استمر;
	    إذا (کنارا_maskx && !صف->کنارا_maskx)
		  استمر;
	    إذا (کنارا_mask1 && !صف->کنارا_mask1)
		  استمر;
	    إذا (cur.mask0 != (cur.mask0 & صف->mask0))
		  استمر;
	    إذا (cur.maskx != (cur.maskx & صف->maskx))
		  استمر;
	    إذا (cur.mask1 != (cur.mask1 & صف->mask1))
		  استمر;

	    أرجع BIT4_0;
      }

      لكل (بنانشانی idx = 0 ;  idx < nedges1_ ;  idx += 1) {
	    ثابت بنية udp_کنارے_جدول*صف = edges1_ + idx;

	    إذا (صف->کنارا_position != کنارا_position)
		  استمر;
	    إذا (کنارا_mask0 && !صف->کنارا_mask0)
		  استمر;
	    إذا (کنارا_maskx && !صف->کنارا_maskx)
		  استمر;
	    إذا (کنارا_mask1 && !صف->کنارا_mask1)
		  استمر;
	    إذا (cur.mask0 != (cur.mask0 & صف->mask0))
		  استمر;
	    إذا (cur.maskx != (cur.maskx & صف->maskx))
		  استمر;
	    إذا (cur.mask1 != (cur.mask1 & صف->mask1))
		  استمر;

	    أرجع BIT4_1;
      }

      لكل (بنانشانی idx = 0 ;  idx < nedgesL_ ;  idx += 1) {
	    ثابت بنية udp_کنارے_جدول*صف = کنارے_L + idx;

	    إذا (صف->کنارا_position != کنارا_position)
		  استمر;
	    إذا (کنارا_mask0 && !صف->کنارا_mask0)
		  استمر;
	    إذا (کنارا_maskx && !صف->کنارا_maskx)
		  استمر;
	    إذا (کنارا_mask1 && !صف->کنارا_mask1)
		  استمر;
	    إذا (cur.mask0 != (cur.mask0 & صف->mask0))
		  استمر;
	    إذا (cur.maskx != (cur.maskx & صف->maskx))
		  استمر;
	    إذا (cur.mask1 != (cur.mask1 & صف->mask1))
		  استمر;

	    إذا (cur.mask0 & (1 << پورٹ_گنتی()))
		  أرجع BIT4_0;
	    إذا (cur.mask1 & (1 << پورٹ_گنتی()))
		  أرجع BIT4_1;
	    إذا (cur.maskx & (1 << پورٹ_گنتی()))
		  أرجع BIT4_X;

	    أكد(0);
	    أرجع BIT4_X;
      }

      أرجع BIT4_X;
}

vvp_udp_fun_core::vvp_udp_fun_core(vvp_net_t*net, vvp_udp_s*def)
: vvp_wide_fun_core(net, def->پورٹ_گنتی())
{
      def_ = def;
      cur_باہرلا_2 = def_->get_init();
	// Assume initially that all the inputs are 1'bx
      current_.mask0 = 0;
      current_.mask1 = 0;
      current_.maskx = ~ ((-1UL) << پورٹ_گنتی());

        // If the initial value is 0 or 1, schedule the initial assignment
        // normally, so that any sensitive always processes can be started
        // first.
      إذا (cur_باہرلا_2 != BIT4_X)
	    ویلا_سارنی_generic(هذا, 0, خطأ);
      وإلا
	    ویلا_سارنی_functor(هذا);
}

vvp_udp_fun_core::~vvp_udp_fun_core()
{
}

/*
 * This method is used to propagate the initial value on startup.
 */
فراغ vvp_udp_fun_core::لگاتار_لگاتار()
{
      vvp_vector4_t tmp (1);
      tmp.سیٹ_ثنائی_ہندسہ(0, cur_باہرلا_2);
      propagate_vec4(tmp);
}

فراغ vvp_udp_fun_core::recv_vec4_from_inputs(بنانشانی پورٹ)
{
	/* For now, assume udps are 1-bit wide. */
      بنانشانی پورٹ_چوڑائی = value(پورٹ).size();
      أكد(پورٹ_چوڑائی == 1);

      بنانشانی لما نقاب = 1UL << پورٹ;

      udp_پدھراں_جدول prev = current_;

      بدل (value(پورٹ).value(0)) {

	  حالة BIT4_0:
	    current_.mask0 |= نقاب;
	    current_.mask1 &= ~نقاب;
	    current_.maskx &= ~نقاب;
	    اكسر;
	  حالة BIT4_1:
	    current_.mask0 &= ~نقاب;
	    current_.mask1 |= نقاب;
	    current_.maskx &= ~نقاب;
	    اكسر;
	  افتراضي:
	    current_.mask0 &= ~نقاب;
	    current_.mask1 &= ~نقاب;
	    current_.maskx |= نقاب;
	    اكسر;
      }

      vvp_bit4_t باہرلا_ثنائی_ہندسہ = def_->calculate_output(current_, prev, cur_باہرلا_2);

      إذا (باہرلا_ثنائی_ہندسہ == cur_باہرلا_2)
	    أرجع;

      cur_باہرلا_2 = باہرلا_ثنائی_ہندسہ;
      ویلا_سارنی_functor(هذا);
}


/*
 * This function is called by the parser in response to a .udp
 * node. We create the nodes needed to integrate the UDP into the
 * netlist. The definition should be parsed already.
 */
فراغ compile_udp_functor(حرف*label, حرف*type,
			 بنانشانی argc, بنية symb_s*argv)
{
      بنية vvp_udp_s *def = udp_لبھو(type);
      أكد(def);
      حرر(type);

      vvp_net_t*ptr = جديد vvp_net_t;
      vvp_udp_fun_core*core = جديد vvp_udp_fun_core(ptr, def);
      ptr->fun = core;

      define_functor_علامت(label, ptr);
      حرر(label);

      wide_inputs_connect(core, argc, argv);
      حرر(argv);
}
