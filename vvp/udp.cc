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
#اگر_تعریف_شده CHECK_WITH_VALGRIND
#ضمن "vvp_cleanup.h"
#ضمن "ivl_alloc.h"
#پایان_اگر

استفاده نطاق std;

// We may need these later when we build the VPI interface to
// UDP definitions.
#اگر_تعریف_شده CHECK_WITH_VALGRIND
ساكن vvp_udp_s **udp_defns = 0;
ساكن بیعلامت udp_defns_شمار = 0;

فراغ udp_defns_delete()
{
      لكل (بیعلامت idx = 0; idx < udp_defns_شمار; idx += 1) {
	    إذا (udp_defns[idx]->is_sequential()) {
		  vvp_udp_seq_s *obj = static_cast<vvp_udp_seq_s *>
		                       (udp_defns[idx]);
		  حذف obj;
	    } وإلا {
		  vvp_udp_comb_s *obj = static_cast<vvp_udp_comb_s *>
		                        (udp_defns[idx]);
		  حذف obj;
	    }
      }
      حرر(udp_defns);
      udp_defns = 0;
      udp_defns_شمار = 0;
}
#پایان_اگر

ساكن نماد_جدول_t udp_جدول;

فراغ delete_udp_symbols()
{
      delete_نماد_جدول(udp_جدول);
      udp_جدول = 0;
}

بنية vvp_udp_s *udp_پیدا_کن(ثابت حرف *label)
{
      نماد_مقدار_t v = sym_get_مقدار(udp_جدول, label);
      أرجع (بنية vvp_udp_s *)v.ptr;
}

ostream& عملگر <<(ostream&o, ثابت بنية udp_سطوح_جدول&جدول)
{
      o << "[" << hex << جدول.mask0
	<< "/" << hex << جدول.mask1
	<< "/" << hex << جدول.maskx << "]";
      أرجع o;
}

vvp_udp_s::vvp_udp_s(ثابت حرف*label, حرف*name__, بیعلامت ports,
                     vvp_bit4_t init, منطقي type)
: name_(name__), ports_(ports), init_(init), seq_(type)
{
      إذا (!udp_جدول)
	    udp_جدول = new_نماد_جدول();

      أكد( !udp_پیدا_کن(label) );

      نماد_مقدار_t v;
      v.ptr = هذا;
      sym_مجموعه_مقدار(udp_جدول, label, v);

#اگر_تعریف_شده CHECK_WITH_VALGRIND
      udp_defns_شمار += 1;
      udp_defns = static_cast<vvp_udp_s **>(realloc(udp_defns,
                  udp_defns_شمار*حجم(vvp_udp_s **)));
      udp_defns[udp_defns_شمار-1] = هذا;
#پایان_اگر
}

vvp_udp_s::~vvp_udp_s()
{
      حذف[] name_;
}

بیعلامت vvp_udp_s::پورت_شمار() ثابت
{
      أرجع ports_;
}

vvp_bit4_t vvp_udp_s::get_init() ثابت
{
      أرجع init_;
}

vvp_udp_comb_s::vvp_udp_comb_s(ثابت حرف*label, حرف*name__, بیعلامت ports)
: vvp_udp_s(label, name__, ports, BIT4_X, خطأ)
{
      levels0_ = 0;
      levels1_ = 0;
      nlevels0_ = 0;
      nlevels1_ = 0;
}

vvp_udp_comb_s::~vvp_udp_comb_s()
{
      حذف[] levels0_;
      حذف[] levels1_;
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
vvp_bit4_t vvp_udp_comb_s::test_سطوح(ثابت udp_سطوح_جدول&cur)
{
	/* To test for a row match, test that the mask0, mask1 and
	   maskx vectors all have bits set where the matching
	   cur.mask0/1/x vectors have bits set. It is possible that a
	   levels0_[idx] vector has more bits set then the cur mask,
	   but this is OK and these bits are to be ignored. */

      لكل (بیعلامت idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    إذا (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  استمر;
	    إذا (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  استمر;
	    إذا (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  استمر;

	    أرجع BIT4_0;
      }

      لكل (بیعلامت idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
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

vvp_bit4_t vvp_udp_comb_s::calculate_output(ثابت udp_سطوح_جدول&cur,
					    ثابت udp_سطوح_جدول&,
					    vvp_bit4_t)
{
      أرجع test_سطوح(cur);
}

ساكن فراغ or_based_on_char(udp_سطوح_جدول&cur, حرف flag,
			     بیعلامت دراز ماسک_بیت)
{
      بدل (flag) {
	  حالة '0':
	    cur.mask0 |= ماسک_بیت;
	    اكسر;
	  حالة '1':
	    cur.mask1 |= ماسک_بیت;
	    اكسر;
	  حالة 'x':
	    cur.maskx |= ماسک_بیت;
	    اكسر;
	  حالة 'b':
	    cur.mask0 |= ماسک_بیت;
	    cur.mask1 |= ماسک_بیت;
	    اكسر;
	  حالة 'l':
	    cur.mask0 |= ماسک_بیت;
	    cur.maskx |= ماسک_بیت;
	    اكسر;
	  حالة 'h':
	    cur.maskx |= ماسک_بیت;
	    cur.mask1 |= ماسک_بیت;
	    اكسر;
	  حالة '?':
	    cur.mask0 |= ماسک_بیت;
	    cur.maskx |= ماسک_بیت;
	    cur.mask1 |= ماسک_بیت;
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "Unsupported flag %c(%d).\n", flag, flag);
	    أكد(0);
      }
}

فراغ vvp_udp_comb_s::compile_جدول(حرف**tab)
{
      بیعلامت nrows0 = 0, nrows1 = 0;

	/* First run through the table to figure out the number of
	   rows I need for each kind of table. */
      لكل (بیعلامت idx = 0 ;  tab[idx] ;  idx += 1) {
	    أكد(طول_رشته(tab[idx]) == پورت_شمار() + 1);
	    بدل (tab[idx][پورت_شمار()]) {
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
		          tab[idx][پورت_شمار()], tab[idx][پورت_شمار()]);
		  أكد(0);
	    }
      }

      nlevels0_ = nrows0;
      levels0_ = جديد udp_سطوح_جدول[nlevels0_];

      nlevels1_ = nrows1;
      levels1_ = جديد udp_سطوح_جدول[nlevels1_];

      nrows0 = 0;
      nrows1 = 0;
      لكل (بیعلامت idx = 0 ;  tab[idx] ;  idx += 1) {
	    بنية udp_سطوح_جدول cur;
	    cur.mask0 = 0;
	    cur.mask1 = 0;
	    cur.maskx = 0;
	    إذا (پورت_شمار() > 8*حجم(cur.mask0)) {
		  fprintf(stderr, "internal error: primitive port count=%u "
			  " > %zu\n", پورت_شمار(), حجم(cur.mask0));
		  أكد(پورت_شمار() <= 8*حجم(cur.mask0));
	    }
	    لكل (بیعلامت pp = 0 ;  pp < پورت_شمار() ;  pp += 1) {
		  بیعلامت دراز ماسک_بیت = 1UL << pp;
		  or_based_on_char(cur, tab[idx][pp], ماسک_بیت);
	    }

	    بدل (tab[idx][پورت_شمار()]) {
		حالة '0':
		  levels0_[nrows0++] = cur;
		  اكسر;
		حالة '1':
		  levels1_[nrows1++] = cur;
		  اكسر;
		افتراضي:
		  اكسر;
	    }
	    حذف[] tab[idx];
      }
      حرر(tab);

      أكد(nrows0 == nlevels0_);
      أكد(nrows1 == nlevels1_);
}

vvp_udp_seq_s::vvp_udp_seq_s(ثابت حرف*label, حرف*name__,
			     بیعلامت ports, vvp_bit4_t init)
: vvp_udp_s(label, name__, ports, init, درست)
{
      levels0_ = 0;
      levels1_ = 0;
      levelsx_ = 0;
      سطوح_L = 0;

      nlevels0_ = 0;
      nlevels1_ = 0;
      nlevelsx_ = 0;
      nlevelsL_ = 0;

      edges0_ = 0;
      edges1_ = 0;
      یالها_L = 0;

      nedges0_ = 0;
      nedges1_ = 0;
      nedgesL_ = 0;
}

vvp_udp_seq_s::~vvp_udp_seq_s()
{
      حذف[] levels0_;
      حذف[] levels1_;
      حذف[] levelsx_;
      حذف[] سطوح_L;
      حذف[] edges0_;
      حذف[] edges1_;
      حذف[] یالها_L;
}

فراغ یال_based_on_char(بنية udp_یالها_جدول&cur, حرف chr, بیعلامت pos)
{
      بیعلامت دراز ماسک_بیت = 1UL << pos;

      بدل (chr) {
	  حالة '0':
	    cur.mask0 |= ماسک_بیت;
	    اكسر;
	  حالة '1':
	    cur.mask1 |= ماسک_بیت;
	    اكسر;
	  حالة 'x':
	    cur.maskx |= ماسک_بیت;
	    اكسر;
	  حالة 'b':
	    cur.mask0 |= ماسک_بیت;
	    cur.mask1 |= ماسک_بیت;
	    اكسر;
	  حالة 'l':
	    cur.mask0 |= ماسک_بیت;
	    cur.maskx |= ماسک_بیت;
	    اكسر;
	  حالة 'h':
	    cur.maskx |= ماسک_بیت;
	    cur.mask1 |= ماسک_بیت;
	    اكسر;
	  حالة '?':
	    cur.mask0 |= ماسک_بیت;
	    cur.maskx |= ماسک_بیت;
	    cur.mask1 |= ماسک_بیت;
	    اكسر;

	  حالة 'B': // (x?) edge
	    cur.mask0 |= ماسک_بیت;
	    cur.mask1 |= ماسک_بیت;
	    cur.یال_position = pos;
	    cur.یال_mask0 = 0;
	    cur.یال_maskx = 1;
	    cur.یال_mask1 = 0;
	    اكسر;
	  حالة 'f': // (10) edge
	    cur.mask0 |= ماسک_بیت;
	    cur.یال_position = pos;
	    cur.یال_mask0 = 0;
	    cur.یال_maskx = 0;
	    cur.یال_mask1 = 1;
	    اكسر;
	  حالة 'F': // (x0) edge
	    cur.mask0 |= ماسک_بیت;
	    cur.یال_position = pos;
	    cur.یال_mask0 = 0;
	    cur.یال_maskx = 1;
	    cur.یال_mask1 = 0;
	    اكسر;
	  حالة 'M': // (1x) edge
	    cur.maskx |= ماسک_بیت;
	    cur.یال_position = pos;
	    cur.یال_mask0 = 0;
	    cur.یال_maskx = 0;
	    cur.یال_mask1 = 1;
	    اكسر;
	  حالة 'N': // (1x) and (10) edge
	    cur.mask0 |= ماسک_بیت;
	    cur.maskx |= ماسک_بیت;
	    cur.یال_position = pos;
	    cur.یال_mask0 = 0;
	    cur.یال_maskx = 0;
	    cur.یال_mask1 = 1;
	    اكسر;
	  حالة 'P': // (0x) and (01) edge
	    cur.maskx |= ماسک_بیت;
	    cur.mask1 |= ماسک_بیت;
	    cur.یال_position = pos;
	    cur.یال_mask0 = 1;
	    cur.یال_maskx = 0;
	    cur.یال_mask1 = 0;
	    اكسر;
	  حالة 'q': // (bx) edge
	    cur.maskx |= ماسک_بیت;
	    cur.یال_position = pos;
	    cur.یال_mask0 = 1;
	    cur.یال_maskx = 0;
	    cur.یال_mask1 = 1;
	    اكسر;
	  حالة 'Q': // (0x) edge
	    cur.maskx |= ماسک_بیت;
	    cur.یال_position = pos;
	    cur.یال_mask0 = 1;
	    cur.یال_maskx = 0;
	    cur.یال_mask1 = 0;
	    اكسر;
	  حالة 'r': // (01) edge
	    cur.mask1 |= ماسک_بیت;
	    cur.یال_position = pos;
	    cur.یال_mask0 = 1;
	    cur.یال_maskx = 0;
	    cur.یال_mask1 = 0;
	    اكسر;
	  حالة 'R': // (x1) edge
	    cur.mask1 |= ماسک_بیت;
	    cur.یال_position = pos;
	    cur.یال_mask0 = 0;
	    cur.یال_maskx = 1;
	    cur.یال_mask1 = 0;
	    اكسر;
	  حالة '%': // (?x) edge
	    cur.maskx |= ماسک_بیت;
	    cur.یال_position = pos;
	    cur.یال_mask0 = 1;
	    cur.یال_maskx = 0;
	    cur.یال_mask1 = 1;
	    اكسر;
	  حالة '+': // (?1) edge
	    cur.mask1 |= ماسک_بیت;
	    cur.یال_position = pos;
	    cur.یال_mask0 = 1;
	    cur.یال_maskx = 1;
	    cur.یال_mask1 = 0;
	    اكسر;
	  حالة '_': // (?0) edge
	    cur.mask0 |= ماسک_بیت;
	    cur.یال_position = pos;
	    cur.یال_mask0 = 0;
	    cur.یال_maskx = 1;
	    cur.یال_mask1 = 1;
	    اكسر;
	  افتراضي:
	    fprintf(stderr, "internal error: unknown edge code: %c\n", chr);
	    أكد(0);
      }
}

فراغ vvp_udp_seq_s::compile_جدول(حرف**tab)
{

      لكل (بیعلامت idx = 0 ;  tab[idx] ;  idx += 1) {
	    ثابت حرف*ردیف = tab[idx];
	    أكد(طول_رشته(ردیف) == پورت_شمار() + 2);

	    إذا (strspn(ردیف, "01xblh?") >= پورت_شمار()+1) {

		  بدل (ردیف[پورت_شمار()+1]) {
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
			        ردیف[پورت_شمار()+1], ردیف[پورت_شمار()+1]);
			أكد(0);
			اكسر;
		  }

	    } وإلا {
		    /* Rows that have n or p edges will need to be
		       expanded into 2 rows. */
		  بیعلامت extra = 0;
		  إذا (strchr(ردیف,'n'))
			extra = 1;
		  إذا (strchr(ردیف,'p'))
			extra = 1;
		  إذا (strchr(ردیف,'*'))
			extra = 2;

		  بدل (ردیف[پورت_شمار()+1]) {
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
			        ردیف[پورت_شمار()+1], ردیف[پورت_شمار()+1]);
			أكد(0);
			اكسر;
		  }
	    }
      }

      levels0_ = جديد udp_سطوح_جدول[nlevels0_];
      levels1_ = جديد udp_سطوح_جدول[nlevels1_];
      levelsx_ = جديد udp_سطوح_جدول[nlevelsx_];
      سطوح_L = جديد udp_سطوح_جدول[nlevelsL_];
      edges0_ = جديد udp_یالها_جدول[nedges0_];
      edges1_ = جديد udp_یالها_جدول[nedges1_];
      یالها_L = جديد udp_یالها_جدول[nedgesL_];

      بیعلامت idx_lev0 = 0;
      بیعلامت idx_lev1 = 0;
      بیعلامت idx_levx = 0;
      بیعلامت idx_levL = 0;
      بیعلامت idx_edg0 = 0;
      بیعلامت idx_edg1 = 0;
      بیعلامت idx_edgL = 0;

      لكل (بیعلامت idx = 0 ;  tab[idx] ;  idx += 1) {
	    ثابت حرف*ردیف = tab[idx];

	    إذا (strspn(ردیف, "01xblh?") >= پورت_شمار()+1) {
		  بنية udp_سطوح_جدول cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  لكل (بیعلامت pp = 0 ;  pp < پورت_شمار() ;  pp += 1) {
			بیعلامت دراز ماسک_بیت = 1UL << pp;
			or_based_on_char(cur, ردیف[pp+1], ماسک_بیت);
		  }

		  or_based_on_char(cur, ردیف[0], 1UL << پورت_شمار());

		  بدل (ردیف[پورت_شمار()+1]) {
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
			سطوح_L[idx_levL++] = cur;
			اكسر;
		      افتراضي:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        ردیف[پورت_شمار()+1], ردیف[پورت_شمار()+1]);
			أكد(0);
			اكسر;
		  }

	    } وإلا {
		  بنية udp_یالها_جدول cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  cur.یال_position = 0;
		  cur.یال_mask0 = 0;
		  cur.یال_mask1 = 0;
		  cur.یال_maskx = 0;

		  منطقي need_ext0_جدول = خطأ;
		  بنية udp_یالها_جدول ext0;
		  ext0.mask0 = 0;
		  ext0.mask1 = 0;
		  ext0.maskx = 0;
		  ext0.یال_position = 0;
		  ext0.یال_mask0 = 0;
		  ext0.یال_mask1 = 0;
		  ext0.یال_maskx = 0;

		  منطقي need_ext1_جدول = خطأ;
		  بنية udp_یالها_جدول ext1;
		  ext1.mask0 = 0;
		  ext1.mask1 = 0;
		  ext1.maskx = 0;
		  ext1.یال_position = 0;
		  ext1.یال_mask0 = 0;
		  ext1.یال_mask1 = 0;
		  ext1.یال_maskx = 0;

		  لكل (بیعلامت pp = 0 ;  pp < پورت_شمار() ; pp += 1) {
			بدل (ردیف[pp+1]) {
			    حالة 'n':
			      یال_based_on_char(cur, 'N', pp);
			      یال_based_on_char(ext0, '_', pp);
			      need_ext0_جدول = درست;
			      اكسر;
			    حالة 'p':
			      یال_based_on_char(cur, 'P', pp);
			      یال_based_on_char(ext0, '+', pp);
			      need_ext0_جدول = درست;
			      اكسر;
			    حالة '*':
			      یال_based_on_char(cur, 'P', pp);
			      یال_based_on_char(ext0, 'N', pp);
			      یال_based_on_char(ext1, 'B', pp);
			      need_ext0_جدول = درست;
			      need_ext1_جدول = درست;
			      اكسر;
			    افتراضي:
			      یال_based_on_char(cur, ردیف[pp+1], pp);
			      یال_based_on_char(ext0, ردیف[pp+1], pp);
			      یال_based_on_char(ext1, ردیف[pp+1], pp);
			      اكسر;
			}
		  }
		  یال_based_on_char(cur, ردیف[0], پورت_شمار());
		  یال_based_on_char(ext0, ردیف[0], پورت_شمار());
		  یال_based_on_char(ext1, ردیف[0], پورت_شمار());

		  بدل (ردیف[پورت_شمار()+1]) {
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
			یالها_L[idx_edgL++] = cur;
			إذا (need_ext0_جدول)
			      یالها_L[idx_edgL++] = ext0;
			إذا (need_ext1_جدول)
			      یالها_L[idx_edgL++] = ext1;
			اكسر;
		      افتراضي:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        ردیف[پورت_شمار()+1], ردیف[پورت_شمار()+1]);
			أكد(0);
			اكسر;
		  }

	    }
	    حذف[] tab[idx];
      }
      حرر(tab);

      أكد(idx_edg0 == nedges0_);
      أكد(idx_edg1 == nedges1_);
      أكد(idx_edgL == nedgesL_);

}

منطقي عملگر == (ثابت udp_سطوح_جدول&a, ثابت udp_سطوح_جدول&b)
{
      إذا (a.mask0 != b.mask0)
	    أرجع خطأ;
      إذا (a.mask1 != b.mask1)
	    أرجع خطأ;
      إذا (a.maskx != b.maskx)
	    أرجع خطأ;
      أرجع درست;
}

vvp_bit4_t vvp_udp_seq_s::calculate_output(ثابت udp_سطوح_جدول&cur,
					   ثابت udp_سطوح_جدول&prev,
					   vvp_bit4_t cur_بیرون)
{
      إذا (cur == prev)
	    أرجع cur_بیرون;

      udp_سطوح_جدول cur_tmp = cur;

      بیعلامت دراز ماسک_بیرون = 1UL << پورت_شمار();
      بدل (cur_بیرون) {
	  حالة BIT4_0:
	    cur_tmp.mask0 |= ماسک_بیرون;
	    اكسر;
	  حالة BIT4_1:
	    cur_tmp.mask1 |= ماسک_بیرون;
	    اكسر;
	  افتراضي:
	    cur_tmp.maskx |= ماسک_بیرون;
	    اكسر;
      }

      vvp_bit4_t lev = test_سطوح_2(cur_tmp);
      إذا (lev == BIT4_Z) {
	    lev = test_یالها(cur_tmp, prev);
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
vvp_bit4_t vvp_udp_seq_s::test_سطوح_2(ثابت udp_سطوح_جدول&cur)
{
      لكل (بیعلامت idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    إذا (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  استمر;
	    إذا (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  استمر;
	    إذا (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  استمر;

	    أرجع BIT4_0;
      }

      لكل (بیعلامت idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
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
      لكل (بیعلامت idx = 0 ;  idx < nlevelsx_ ;  idx += 1) {
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
      لكل (بیعلامت idx = 0 ;  idx < nlevelsL_ ;  idx += 1) {
	    إذا (cur.mask0 != (cur.mask0 & سطوح_L[idx].mask0))
		  استمر;
	    إذا (cur.mask1 != (cur.mask1 & سطوح_L[idx].mask1))
		  استمر;
	    إذا (cur.maskx != (cur.maskx & سطوح_L[idx].maskx))
		  استمر;

	    إذا (cur.mask0 & (1 << پورت_شمار()))
		  أرجع BIT4_0;
	    إذا (cur.mask1 & (1 << پورت_شمار()))
		  أرجع BIT4_1;
	    إذا (cur.maskx & (1 << پورت_شمار()))
		  أرجع BIT4_X;

	    أكد(0);
	    أرجع BIT4_X;
      }

	/* No explicit levels entry match. Return a Z to signal that
	   further testing is needed. */
      أرجع BIT4_Z;
}

vvp_bit4_t vvp_udp_seq_s::test_یالها(ثابت udp_سطوح_جدول&cur,
				      ثابت udp_سطوح_جدول&prev)
{
	/* The edge_mask is true for all bits that are different in
	   the cur and prev tables. */
      بیعلامت دراز edge0_ماسک = cur.mask0 ^ prev.mask0;
      بیعلامت دراز edgex_ماسک = cur.maskx ^ prev.maskx;
      بیعلامت دراز edge1_ماسک = cur.mask1 ^ prev.mask1;

      بیعلامت دراز یال_ماسک = edge0_ماسک|edgex_ماسک|edge1_ماسک;
      یال_ماسک &= ~ (-1UL << پورت_شمار());

	/* If there are no differences, then there are no edges. Give
	   up now. */
      إذا (یال_ماسک == 0)
	    أرجع BIT4_X;

      بیعلامت یال_position = 0;
      طالما ((یال_ماسک&1) == 0) {
	    یال_ماسک >>= 1;
	    یال_position += 1;
      }

	/* We expect that there is exactly one edge in here. */
      أكد(یال_ماسک == 1);

      یال_ماسک = 1UL << یال_position;

      بیعلامت یال_mask0 = (prev.mask0&یال_ماسک)? 1 : 0;
      بیعلامت یال_maskx = (prev.maskx&یال_ماسک)? 1 : 0;
      بیعلامت یال_mask1 = (prev.mask1&یال_ماسک)? 1 : 0;


	/* Now the edge_position and edge_mask* variables have the
	   values we use to test the applicability of the edge_table
	   entries. */

      لكل (بیعلامت idx = 0 ;  idx < nedges0_ ;  idx += 1) {
	    ثابت بنية udp_یالها_جدول*ردیف = edges0_ + idx;

	    إذا (ردیف->یال_position != یال_position)
		  استمر;
	    إذا (یال_mask0 && !ردیف->یال_mask0)
		  استمر;
	    إذا (یال_maskx && !ردیف->یال_maskx)
		  استمر;
	    إذا (یال_mask1 && !ردیف->یال_mask1)
		  استمر;
	    إذا (cur.mask0 != (cur.mask0 & ردیف->mask0))
		  استمر;
	    إذا (cur.maskx != (cur.maskx & ردیف->maskx))
		  استمر;
	    إذا (cur.mask1 != (cur.mask1 & ردیف->mask1))
		  استمر;

	    أرجع BIT4_0;
      }

      لكل (بیعلامت idx = 0 ;  idx < nedges1_ ;  idx += 1) {
	    ثابت بنية udp_یالها_جدول*ردیف = edges1_ + idx;

	    إذا (ردیف->یال_position != یال_position)
		  استمر;
	    إذا (یال_mask0 && !ردیف->یال_mask0)
		  استمر;
	    إذا (یال_maskx && !ردیف->یال_maskx)
		  استمر;
	    إذا (یال_mask1 && !ردیف->یال_mask1)
		  استمر;
	    إذا (cur.mask0 != (cur.mask0 & ردیف->mask0))
		  استمر;
	    إذا (cur.maskx != (cur.maskx & ردیف->maskx))
		  استمر;
	    إذا (cur.mask1 != (cur.mask1 & ردیف->mask1))
		  استمر;

	    أرجع BIT4_1;
      }

      لكل (بیعلامت idx = 0 ;  idx < nedgesL_ ;  idx += 1) {
	    ثابت بنية udp_یالها_جدول*ردیف = یالها_L + idx;

	    إذا (ردیف->یال_position != یال_position)
		  استمر;
	    إذا (یال_mask0 && !ردیف->یال_mask0)
		  استمر;
	    إذا (یال_maskx && !ردیف->یال_maskx)
		  استمر;
	    إذا (یال_mask1 && !ردیف->یال_mask1)
		  استمر;
	    إذا (cur.mask0 != (cur.mask0 & ردیف->mask0))
		  استمر;
	    إذا (cur.maskx != (cur.maskx & ردیف->maskx))
		  استمر;
	    إذا (cur.mask1 != (cur.mask1 & ردیف->mask1))
		  استمر;

	    إذا (cur.mask0 & (1 << پورت_شمار()))
		  أرجع BIT4_0;
	    إذا (cur.mask1 & (1 << پورت_شمار()))
		  أرجع BIT4_1;
	    إذا (cur.maskx & (1 << پورت_شمار()))
		  أرجع BIT4_X;

	    أكد(0);
	    أرجع BIT4_X;
      }

      أرجع BIT4_X;
}

vvp_udp_fun_core::vvp_udp_fun_core(vvp_net_t*net, vvp_udp_s*def)
: vvp_wide_fun_core(net, def->پورت_شمار())
{
      def_ = def;
      cur_بیرون_2 = def_->get_init();
	// Assume initially that all the inputs are 1'bx
      current_.mask0 = 0;
      current_.mask1 = 0;
      current_.maskx = ~ ((-1UL) << پورت_شمار());

        // If the initial value is 0 or 1, schedule the initial assignment
        // normally, so that any sensitive always processes can be started
        // first.
      إذا (cur_بیرون_2 != BIT4_X)
	    زمانبندی_generic(هذا, 0, خطأ);
      وإلا
	    زمانبندی_functor(هذا);
}

vvp_udp_fun_core::~vvp_udp_fun_core()
{
}

/*
 * This method is used to propagate the initial value on startup.
 */
فراغ vvp_udp_fun_core::پیاپی_پیاپی()
{
      vvp_vector4_t tmp (1);
      tmp.مجموعه_بیت(0, cur_بیرون_2);
      propagate_vec4(tmp);
}

فراغ vvp_udp_fun_core::recv_vec4_from_inputs(بیعلامت پورت)
{
	/* For now, assume udps are 1-bit wide. */
      بیعلامت پورت_عرض = value(پورت).size();
      أكد(پورت_عرض == 1);

      بیعلامت دراز ماسک = 1UL << پورت;

      udp_سطوح_جدول prev = current_;

      بدل (value(پورت).value(0)) {

	  حالة BIT4_0:
	    current_.mask0 |= ماسک;
	    current_.mask1 &= ~ماسک;
	    current_.maskx &= ~ماسک;
	    اكسر;
	  حالة BIT4_1:
	    current_.mask0 &= ~ماسک;
	    current_.mask1 |= ماسک;
	    current_.maskx &= ~ماسک;
	    اكسر;
	  افتراضي:
	    current_.mask0 &= ~ماسک;
	    current_.mask1 &= ~ماسک;
	    current_.maskx |= ماسک;
	    اكسر;
      }

      vvp_bit4_t بیرون_بیت = def_->calculate_output(current_, prev, cur_بیرون_2);

      إذا (بیرون_بیت == cur_بیرون_2)
	    أرجع;

      cur_بیرون_2 = بیرون_بیت;
      زمانبندی_functor(هذا);
}


/*
 * This function is called by the parser in response to a .udp
 * node. We create the nodes needed to integrate the UDP into the
 * netlist. The definition should be parsed already.
 */
فراغ compile_udp_functor(حرف*label, حرف*type,
			 بیعلامت argc, بنية symb_s*argv)
{
      بنية vvp_udp_s *def = udp_پیدا_کن(type);
      أكد(def);
      حرر(type);

      vvp_net_t*ptr = جديد vvp_net_t;
      vvp_udp_fun_core*core = جديد vvp_udp_fun_core(ptr, def);
      ptr->fun = core;

      define_functor_نماد(label, ptr);
      حرر(label);

      wide_inputs_connect(core, argc, argv);
      حرر(argv);
}
