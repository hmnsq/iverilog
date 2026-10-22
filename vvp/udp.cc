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

#شامل "udp.h"
#شامل "schedule.h"
#شامل "symbols.h"
#شامل "compile.h"
#شامل "config.h"
#شامل <cassert>
#شامل <cstdlib>
#شامل <cstring>
#شامل <cstdio>
#شامل <iostream>
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
#شامل "vvp_cleanup.h"
#شامل "ivl_alloc.h"
#اگر_ختم

استعمال نامخلا std;

// We may need these later when we build the VPI interface to
// UDP definitions.
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
ساکن vvp_udp_s **udp_defns = 0;
ساکن غیرعلامتی udp_defns_گنتی = 0;

خالی udp_defns_delete()
{
      برائے (غیرعلامتی idx = 0; idx < udp_defns_گنتی; idx += 1) {
	    اگر (udp_defns[idx]->is_sequential()) {
		  vvp_udp_seq_s *obj = static_cast<vvp_udp_seq_s *>
		                       (udp_defns[idx]);
		  حذف obj;
	    } ورنہ {
		  vvp_udp_comb_s *obj = static_cast<vvp_udp_comb_s *>
		                        (udp_defns[idx]);
		  حذف obj;
	    }
      }
      آزاد(udp_defns);
      udp_defns = 0;
      udp_defns_گنتی = 0;
}
#اگر_ختم

ساکن علامت_جدول_t udp_جدول;

خالی delete_udp_symbols()
{
      delete_علامت_جدول(udp_جدول);
      udp_جدول = 0;
}

ساخت vvp_udp_s *udp_ڈھونڈو(ثابت حرف *label)
{
      علامت_قدر_t v = sym_get_قدر(udp_جدول, label);
      واپس (ساخت vvp_udp_s *)v.ptr;
}

ostream& عامل <<(ostream&o, ثابت ساخت udp_سطحیں_جدول&جدول)
{
      o << "[" << hex << جدول.mask0
	<< "/" << hex << جدول.mask1
	<< "/" << hex << جدول.maskx << "]";
      واپس o;
}

vvp_udp_s::vvp_udp_s(ثابت حرف*label, حرف*name__, غیرعلامتی ports,
                     vvp_bit4_t init, منطقی type)
: name_(name__), ports_(ports), init_(init), seq_(type)
{
      اگر (!udp_جدول)
	    udp_جدول = new_علامت_جدول();

      أكد( !udp_ڈھونڈو(label) );

      علامت_قدر_t v;
      v.ptr = یہ;
      sym_سیٹ_قدر(udp_جدول, label, v);

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      udp_defns_گنتی += 1;
      udp_defns = static_cast<vvp_udp_s **>(realloc(udp_defns,
                  udp_defns_گنتی*حجم(vvp_udp_s **)));
      udp_defns[udp_defns_گنتی-1] = یہ;
#اگر_ختم
}

vvp_udp_s::~vvp_udp_s()
{
      حذف[] name_;
}

غیرعلامتی vvp_udp_s::پورٹ_گنتی() ثابت
{
      واپس ports_;
}

vvp_bit4_t vvp_udp_s::get_init() ثابت
{
      واپس init_;
}

vvp_udp_comb_s::vvp_udp_comb_s(ثابت حرف*label, حرف*name__, غیرعلامتی ports)
: vvp_udp_s(label, name__, ports, BIT4_X, جھوٹ)
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
vvp_bit4_t vvp_udp_comb_s::test_سطحیں(ثابت udp_سطحیں_جدول&cur)
{
	/* To test for a row match, test that the mask0, mask1 and
	   maskx vectors all have bits set where the matching
	   cur.mask0/1/x vectors have bits set. It is possible that a
	   levels0_[idx] vector has more bits set then the cur mask,
	   but this is OK and these bits are to be ignored. */

      برائے (غیرعلامتی idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    اگر (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  جاری;
	    اگر (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  جاری;
	    اگر (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  جاری;

	    واپس BIT4_0;
      }

      برائے (غیرعلامتی idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    اگر (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  جاری;
	    اگر (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  جاری;
	    اگر (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  جاری;

	    واپس BIT4_1;
      }

      واپس BIT4_X;
}

vvp_bit4_t vvp_udp_comb_s::calculate_output(ثابت udp_سطحیں_جدول&cur,
					    ثابت udp_سطحیں_جدول&,
					    vvp_bit4_t)
{
      واپس test_سطحیں(cur);
}

ساکن خالی or_based_on_char(udp_سطحیں_جدول&cur, حرف flag,
			     غیرعلامتی طویل نقاب_ثنائی_ہندسہ)
{
      انتخاب (flag) {
	  حالت '0':
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    توڑو;
	  حالت '1':
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    توڑو;
	  حالت 'x':
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    توڑو;
	  حالت 'b':
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    توڑو;
	  حالت 'l':
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    توڑو;
	  حالت 'h':
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    توڑو;
	  حالت '?':
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    توڑو;
	  طےشدہ:
	    fprintf(stderr, "Unsupported flag %c(%d).\n", flag, flag);
	    أكد(0);
      }
}

خالی vvp_udp_comb_s::compile_جدول(حرف**tab)
{
      غیرعلامتی nrows0 = 0, nrows1 = 0;

	/* First run through the table to figure out the number of
	   rows I need for each kind of table. */
      برائے (غیرعلامتی idx = 0 ;  tab[idx] ;  idx += 1) {
	    أكد(لڑی_لمبائی(tab[idx]) == پورٹ_گنتی() + 1);
	    انتخاب (tab[idx][پورٹ_گنتی()]) {
		حالت '0':
		  nrows0 += 1;
		  توڑو;
		حالت '1':
		  nrows1 += 1;
		  توڑو;
		حالت 'x':
		  توڑو;
		طےشدہ:
		  fprintf(stderr, "Unsupported entry %c(%d).\n",
		          tab[idx][پورٹ_گنتی()], tab[idx][پورٹ_گنتی()]);
		  أكد(0);
	    }
      }

      nlevels0_ = nrows0;
      levels0_ = نیا udp_سطحیں_جدول[nlevels0_];

      nlevels1_ = nrows1;
      levels1_ = نیا udp_سطحیں_جدول[nlevels1_];

      nrows0 = 0;
      nrows1 = 0;
      برائے (غیرعلامتی idx = 0 ;  tab[idx] ;  idx += 1) {
	    ساخت udp_سطحیں_جدول cur;
	    cur.mask0 = 0;
	    cur.mask1 = 0;
	    cur.maskx = 0;
	    اگر (پورٹ_گنتی() > 8*حجم(cur.mask0)) {
		  fprintf(stderr, "internal error: primitive port count=%u "
			  " > %zu\n", پورٹ_گنتی(), حجم(cur.mask0));
		  أكد(پورٹ_گنتی() <= 8*حجم(cur.mask0));
	    }
	    برائے (غیرعلامتی pp = 0 ;  pp < پورٹ_گنتی() ;  pp += 1) {
		  غیرعلامتی طویل نقاب_ثنائی_ہندسہ = 1UL << pp;
		  or_based_on_char(cur, tab[idx][pp], نقاب_ثنائی_ہندسہ);
	    }

	    انتخاب (tab[idx][پورٹ_گنتی()]) {
		حالت '0':
		  levels0_[nrows0++] = cur;
		  توڑو;
		حالت '1':
		  levels1_[nrows1++] = cur;
		  توڑو;
		طےشدہ:
		  توڑو;
	    }
	    حذف[] tab[idx];
      }
      آزاد(tab);

      أكد(nrows0 == nlevels0_);
      أكد(nrows1 == nlevels1_);
}

vvp_udp_seq_s::vvp_udp_seq_s(ثابت حرف*label, حرف*name__,
			     غیرعلامتی ports, vvp_bit4_t init)
: vvp_udp_s(label, name__, ports, init, سچ)
{
      levels0_ = 0;
      levels1_ = 0;
      levelsx_ = 0;
      سطحیں_L = 0;

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
      حذف[] levels0_;
      حذف[] levels1_;
      حذف[] levelsx_;
      حذف[] سطحیں_L;
      حذف[] edges0_;
      حذف[] edges1_;
      حذف[] کنارے_L;
}

خالی کنارہ_based_on_char(ساخت udp_کنارے_جدول&cur, حرف chr, غیرعلامتی pos)
{
      غیرعلامتی طویل نقاب_ثنائی_ہندسہ = 1UL << pos;

      انتخاب (chr) {
	  حالت '0':
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    توڑو;
	  حالت '1':
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    توڑو;
	  حالت 'x':
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    توڑو;
	  حالت 'b':
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    توڑو;
	  حالت 'l':
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    توڑو;
	  حالت 'h':
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    توڑو;
	  حالت '?':
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    توڑو;

	  حالت 'B': // (x?) edge
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    cur.کنارہ_position = pos;
	    cur.کنارہ_mask0 = 0;
	    cur.کنارہ_maskx = 1;
	    cur.کنارہ_mask1 = 0;
	    توڑو;
	  حالت 'f': // (10) edge
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.کنارہ_position = pos;
	    cur.کنارہ_mask0 = 0;
	    cur.کنارہ_maskx = 0;
	    cur.کنارہ_mask1 = 1;
	    توڑو;
	  حالت 'F': // (x0) edge
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.کنارہ_position = pos;
	    cur.کنارہ_mask0 = 0;
	    cur.کنارہ_maskx = 1;
	    cur.کنارہ_mask1 = 0;
	    توڑو;
	  حالت 'M': // (1x) edge
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    cur.کنارہ_position = pos;
	    cur.کنارہ_mask0 = 0;
	    cur.کنارہ_maskx = 0;
	    cur.کنارہ_mask1 = 1;
	    توڑو;
	  حالت 'N': // (1x) and (10) edge
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    cur.کنارہ_position = pos;
	    cur.کنارہ_mask0 = 0;
	    cur.کنارہ_maskx = 0;
	    cur.کنارہ_mask1 = 1;
	    توڑو;
	  حالت 'P': // (0x) and (01) edge
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    cur.کنارہ_position = pos;
	    cur.کنارہ_mask0 = 1;
	    cur.کنارہ_maskx = 0;
	    cur.کنارہ_mask1 = 0;
	    توڑو;
	  حالت 'q': // (bx) edge
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    cur.کنارہ_position = pos;
	    cur.کنارہ_mask0 = 1;
	    cur.کنارہ_maskx = 0;
	    cur.کنارہ_mask1 = 1;
	    توڑو;
	  حالت 'Q': // (0x) edge
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    cur.کنارہ_position = pos;
	    cur.کنارہ_mask0 = 1;
	    cur.کنارہ_maskx = 0;
	    cur.کنارہ_mask1 = 0;
	    توڑو;
	  حالت 'r': // (01) edge
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    cur.کنارہ_position = pos;
	    cur.کنارہ_mask0 = 1;
	    cur.کنارہ_maskx = 0;
	    cur.کنارہ_mask1 = 0;
	    توڑو;
	  حالت 'R': // (x1) edge
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    cur.کنارہ_position = pos;
	    cur.کنارہ_mask0 = 0;
	    cur.کنارہ_maskx = 1;
	    cur.کنارہ_mask1 = 0;
	    توڑو;
	  حالت '%': // (?x) edge
	    cur.maskx |= نقاب_ثنائی_ہندسہ;
	    cur.کنارہ_position = pos;
	    cur.کنارہ_mask0 = 1;
	    cur.کنارہ_maskx = 0;
	    cur.کنارہ_mask1 = 1;
	    توڑو;
	  حالت '+': // (?1) edge
	    cur.mask1 |= نقاب_ثنائی_ہندسہ;
	    cur.کنارہ_position = pos;
	    cur.کنارہ_mask0 = 1;
	    cur.کنارہ_maskx = 1;
	    cur.کنارہ_mask1 = 0;
	    توڑو;
	  حالت '_': // (?0) edge
	    cur.mask0 |= نقاب_ثنائی_ہندسہ;
	    cur.کنارہ_position = pos;
	    cur.کنارہ_mask0 = 0;
	    cur.کنارہ_maskx = 1;
	    cur.کنارہ_mask1 = 1;
	    توڑو;
	  طےشدہ:
	    fprintf(stderr, "internal error: unknown edge code: %c\n", chr);
	    أكد(0);
      }
}

خالی vvp_udp_seq_s::compile_جدول(حرف**tab)
{

      برائے (غیرعلامتی idx = 0 ;  tab[idx] ;  idx += 1) {
	    ثابت حرف*صف = tab[idx];
	    أكد(لڑی_لمبائی(صف) == پورٹ_گنتی() + 2);

	    اگر (strspn(صف, "01xblh?") >= پورٹ_گنتی()+1) {

		  انتخاب (صف[پورٹ_گنتی()+1]) {
		      حالت '0':
			nlevels0_ += 1;
			توڑو;
		      حالت '1':
			nlevels1_ += 1;
			توڑو;
		      حالت 'x':
			nlevelsx_ += 1;
			توڑو;
		      حالت '-':
			nlevelsL_ += 1;
			توڑو;
		      طےشدہ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        صف[پورٹ_گنتی()+1], صف[پورٹ_گنتی()+1]);
			أكد(0);
			توڑو;
		  }

	    } ورنہ {
		    /* Rows that have n or p edges will need to be
		       expanded into 2 rows. */
		  غیرعلامتی extra = 0;
		  اگر (strchr(صف,'n'))
			extra = 1;
		  اگر (strchr(صف,'p'))
			extra = 1;
		  اگر (strchr(صف,'*'))
			extra = 2;

		  انتخاب (صف[پورٹ_گنتی()+1]) {
		      حالت '0':
			nedges0_ += 1 + extra;
			توڑو;
		      حالت '1':
			nedges1_ += 1 + extra;
			توڑو;
		      حالت 'x':
			توڑو;
		      حالت '-':
			nedgesL_ += 1 + extra;
			توڑو;
		      طےشدہ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        صف[پورٹ_گنتی()+1], صف[پورٹ_گنتی()+1]);
			أكد(0);
			توڑو;
		  }
	    }
      }

      levels0_ = نیا udp_سطحیں_جدول[nlevels0_];
      levels1_ = نیا udp_سطحیں_جدول[nlevels1_];
      levelsx_ = نیا udp_سطحیں_جدول[nlevelsx_];
      سطحیں_L = نیا udp_سطحیں_جدول[nlevelsL_];
      edges0_ = نیا udp_کنارے_جدول[nedges0_];
      edges1_ = نیا udp_کنارے_جدول[nedges1_];
      کنارے_L = نیا udp_کنارے_جدول[nedgesL_];

      غیرعلامتی idx_lev0 = 0;
      غیرعلامتی idx_lev1 = 0;
      غیرعلامتی idx_levx = 0;
      غیرعلامتی idx_levL = 0;
      غیرعلامتی idx_edg0 = 0;
      غیرعلامتی idx_edg1 = 0;
      غیرعلامتی idx_edgL = 0;

      برائے (غیرعلامتی idx = 0 ;  tab[idx] ;  idx += 1) {
	    ثابت حرف*صف = tab[idx];

	    اگر (strspn(صف, "01xblh?") >= پورٹ_گنتی()+1) {
		  ساخت udp_سطحیں_جدول cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  برائے (غیرعلامتی pp = 0 ;  pp < پورٹ_گنتی() ;  pp += 1) {
			غیرعلامتی طویل نقاب_ثنائی_ہندسہ = 1UL << pp;
			or_based_on_char(cur, صف[pp+1], نقاب_ثنائی_ہندسہ);
		  }

		  or_based_on_char(cur, صف[0], 1UL << پورٹ_گنتی());

		  انتخاب (صف[پورٹ_گنتی()+1]) {
		      حالت '0':
			levels0_[idx_lev0++] = cur;
			توڑو;
		      حالت '1':
			levels1_[idx_lev1++] = cur;
			توڑو;
		      حالت 'x':
			levelsx_[idx_levx++] = cur;
			توڑو;
		      حالت '-':
			سطحیں_L[idx_levL++] = cur;
			توڑو;
		      طےشدہ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        صف[پورٹ_گنتی()+1], صف[پورٹ_گنتی()+1]);
			أكد(0);
			توڑو;
		  }

	    } ورنہ {
		  ساخت udp_کنارے_جدول cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  cur.کنارہ_position = 0;
		  cur.کنارہ_mask0 = 0;
		  cur.کنارہ_mask1 = 0;
		  cur.کنارہ_maskx = 0;

		  منطقی need_ext0_جدول = جھوٹ;
		  ساخت udp_کنارے_جدول ext0;
		  ext0.mask0 = 0;
		  ext0.mask1 = 0;
		  ext0.maskx = 0;
		  ext0.کنارہ_position = 0;
		  ext0.کنارہ_mask0 = 0;
		  ext0.کنارہ_mask1 = 0;
		  ext0.کنارہ_maskx = 0;

		  منطقی need_ext1_جدول = جھوٹ;
		  ساخت udp_کنارے_جدول ext1;
		  ext1.mask0 = 0;
		  ext1.mask1 = 0;
		  ext1.maskx = 0;
		  ext1.کنارہ_position = 0;
		  ext1.کنارہ_mask0 = 0;
		  ext1.کنارہ_mask1 = 0;
		  ext1.کنارہ_maskx = 0;

		  برائے (غیرعلامتی pp = 0 ;  pp < پورٹ_گنتی() ; pp += 1) {
			انتخاب (صف[pp+1]) {
			    حالت 'n':
			      کنارہ_based_on_char(cur, 'N', pp);
			      کنارہ_based_on_char(ext0, '_', pp);
			      need_ext0_جدول = سچ;
			      توڑو;
			    حالت 'p':
			      کنارہ_based_on_char(cur, 'P', pp);
			      کنارہ_based_on_char(ext0, '+', pp);
			      need_ext0_جدول = سچ;
			      توڑو;
			    حالت '*':
			      کنارہ_based_on_char(cur, 'P', pp);
			      کنارہ_based_on_char(ext0, 'N', pp);
			      کنارہ_based_on_char(ext1, 'B', pp);
			      need_ext0_جدول = سچ;
			      need_ext1_جدول = سچ;
			      توڑو;
			    طےشدہ:
			      کنارہ_based_on_char(cur, صف[pp+1], pp);
			      کنارہ_based_on_char(ext0, صف[pp+1], pp);
			      کنارہ_based_on_char(ext1, صف[pp+1], pp);
			      توڑو;
			}
		  }
		  کنارہ_based_on_char(cur, صف[0], پورٹ_گنتی());
		  کنارہ_based_on_char(ext0, صف[0], پورٹ_گنتی());
		  کنارہ_based_on_char(ext1, صف[0], پورٹ_گنتی());

		  انتخاب (صف[پورٹ_گنتی()+1]) {
		      حالت '0':
			edges0_[idx_edg0++] = cur;
			اگر (need_ext0_جدول)
			      edges0_[idx_edg0++] = ext0;
			اگر (need_ext1_جدول)
			      edges0_[idx_edg0++] = ext1;
			توڑو;
		      حالت '1':
			edges1_[idx_edg1++] = cur;
			اگر (need_ext0_جدول)
			      edges1_[idx_edg1++] = ext0;
			اگر (need_ext1_جدول)
			      edges1_[idx_edg1++] = ext1;
			توڑو;
		      حالت 'x':
			توڑو;
		      حالت '-':
			کنارے_L[idx_edgL++] = cur;
			اگر (need_ext0_جدول)
			      کنارے_L[idx_edgL++] = ext0;
			اگر (need_ext1_جدول)
			      کنارے_L[idx_edgL++] = ext1;
			توڑو;
		      طےشدہ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        صف[پورٹ_گنتی()+1], صف[پورٹ_گنتی()+1]);
			أكد(0);
			توڑو;
		  }

	    }
	    حذف[] tab[idx];
      }
      آزاد(tab);

      أكد(idx_edg0 == nedges0_);
      أكد(idx_edg1 == nedges1_);
      أكد(idx_edgL == nedgesL_);

}

منطقی عامل == (ثابت udp_سطحیں_جدول&a, ثابت udp_سطحیں_جدول&b)
{
      اگر (a.mask0 != b.mask0)
	    واپس جھوٹ;
      اگر (a.mask1 != b.mask1)
	    واپس جھوٹ;
      اگر (a.maskx != b.maskx)
	    واپس جھوٹ;
      واپس سچ;
}

vvp_bit4_t vvp_udp_seq_s::calculate_output(ثابت udp_سطحیں_جدول&cur,
					   ثابت udp_سطحیں_جدول&prev,
					   vvp_bit4_t cur_برآمد)
{
      اگر (cur == prev)
	    واپس cur_برآمد;

      udp_سطحیں_جدول cur_tmp = cur;

      غیرعلامتی طویل نقاب_برآمد = 1UL << پورٹ_گنتی();
      انتخاب (cur_برآمد) {
	  حالت BIT4_0:
	    cur_tmp.mask0 |= نقاب_برآمد;
	    توڑو;
	  حالت BIT4_1:
	    cur_tmp.mask1 |= نقاب_برآمد;
	    توڑو;
	  طےشدہ:
	    cur_tmp.maskx |= نقاب_برآمد;
	    توڑو;
      }

      vvp_bit4_t lev = test_سطحیں_2(cur_tmp);
      اگر (lev == BIT4_Z) {
	    lev = test_کنارے(cur_tmp, prev);
      }

      واپس lev;
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
vvp_bit4_t vvp_udp_seq_s::test_سطحیں_2(ثابت udp_سطحیں_جدول&cur)
{
      برائے (غیرعلامتی idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    اگر (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  جاری;
	    اگر (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  جاری;
	    اگر (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  جاری;

	    واپس BIT4_0;
      }

      برائے (غیرعلامتی idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    اگر (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  جاری;
	    اگر (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  جاری;
	    اگر (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  جاری;

	    واپس BIT4_1;
      }

	/* We need to test against an explicit X-output table, since
	   we need to distinguish from an X output and no match. */
      برائے (غیرعلامتی idx = 0 ;  idx < nlevelsx_ ;  idx += 1) {
	    اگر (cur.mask0 != (cur.mask0 & levelsx_[idx].mask0))
		  جاری;
	    اگر (cur.mask1 != (cur.mask1 & levelsx_[idx].mask1))
		  جاری;
	    اگر (cur.maskx != (cur.maskx & levelsx_[idx].maskx))
		  جاری;

	    واپس BIT4_X;
      }

	/* Test the table that requests the next output be the same as
	   the current output. This gets the current output from the
	   levels table that was passed in. */
      برائے (غیرعلامتی idx = 0 ;  idx < nlevelsL_ ;  idx += 1) {
	    اگر (cur.mask0 != (cur.mask0 & سطحیں_L[idx].mask0))
		  جاری;
	    اگر (cur.mask1 != (cur.mask1 & سطحیں_L[idx].mask1))
		  جاری;
	    اگر (cur.maskx != (cur.maskx & سطحیں_L[idx].maskx))
		  جاری;

	    اگر (cur.mask0 & (1 << پورٹ_گنتی()))
		  واپس BIT4_0;
	    اگر (cur.mask1 & (1 << پورٹ_گنتی()))
		  واپس BIT4_1;
	    اگر (cur.maskx & (1 << پورٹ_گنتی()))
		  واپس BIT4_X;

	    أكد(0);
	    واپس BIT4_X;
      }

	/* No explicit levels entry match. Return a Z to signal that
	   further testing is needed. */
      واپس BIT4_Z;
}

vvp_bit4_t vvp_udp_seq_s::test_کنارے(ثابت udp_سطحیں_جدول&cur,
				      ثابت udp_سطحیں_جدول&prev)
{
	/* The edge_mask is true for all bits that are different in
	   the cur and prev tables. */
      غیرعلامتی طویل edge0_نقاب = cur.mask0 ^ prev.mask0;
      غیرعلامتی طویل edgex_نقاب = cur.maskx ^ prev.maskx;
      غیرعلامتی طویل edge1_نقاب = cur.mask1 ^ prev.mask1;

      غیرعلامتی طویل کنارہ_نقاب = edge0_نقاب|edgex_نقاب|edge1_نقاب;
      کنارہ_نقاب &= ~ (-1UL << پورٹ_گنتی());

	/* If there are no differences, then there are no edges. Give
	   up now. */
      اگر (کنارہ_نقاب == 0)
	    واپس BIT4_X;

      غیرعلامتی کنارہ_position = 0;
      جب ((کنارہ_نقاب&1) == 0) {
	    کنارہ_نقاب >>= 1;
	    کنارہ_position += 1;
      }

	/* We expect that there is exactly one edge in here. */
      أكد(کنارہ_نقاب == 1);

      کنارہ_نقاب = 1UL << کنارہ_position;

      غیرعلامتی کنارہ_mask0 = (prev.mask0&کنارہ_نقاب)? 1 : 0;
      غیرعلامتی کنارہ_maskx = (prev.maskx&کنارہ_نقاب)? 1 : 0;
      غیرعلامتی کنارہ_mask1 = (prev.mask1&کنارہ_نقاب)? 1 : 0;


	/* Now the edge_position and edge_mask* variables have the
	   values we use to test the applicability of the edge_table
	   entries. */

      برائے (غیرعلامتی idx = 0 ;  idx < nedges0_ ;  idx += 1) {
	    ثابت ساخت udp_کنارے_جدول*صف = edges0_ + idx;

	    اگر (صف->کنارہ_position != کنارہ_position)
		  جاری;
	    اگر (کنارہ_mask0 && !صف->کنارہ_mask0)
		  جاری;
	    اگر (کنارہ_maskx && !صف->کنارہ_maskx)
		  جاری;
	    اگر (کنارہ_mask1 && !صف->کنارہ_mask1)
		  جاری;
	    اگر (cur.mask0 != (cur.mask0 & صف->mask0))
		  جاری;
	    اگر (cur.maskx != (cur.maskx & صف->maskx))
		  جاری;
	    اگر (cur.mask1 != (cur.mask1 & صف->mask1))
		  جاری;

	    واپس BIT4_0;
      }

      برائے (غیرعلامتی idx = 0 ;  idx < nedges1_ ;  idx += 1) {
	    ثابت ساخت udp_کنارے_جدول*صف = edges1_ + idx;

	    اگر (صف->کنارہ_position != کنارہ_position)
		  جاری;
	    اگر (کنارہ_mask0 && !صف->کنارہ_mask0)
		  جاری;
	    اگر (کنارہ_maskx && !صف->کنارہ_maskx)
		  جاری;
	    اگر (کنارہ_mask1 && !صف->کنارہ_mask1)
		  جاری;
	    اگر (cur.mask0 != (cur.mask0 & صف->mask0))
		  جاری;
	    اگر (cur.maskx != (cur.maskx & صف->maskx))
		  جاری;
	    اگر (cur.mask1 != (cur.mask1 & صف->mask1))
		  جاری;

	    واپس BIT4_1;
      }

      برائے (غیرعلامتی idx = 0 ;  idx < nedgesL_ ;  idx += 1) {
	    ثابت ساخت udp_کنارے_جدول*صف = کنارے_L + idx;

	    اگر (صف->کنارہ_position != کنارہ_position)
		  جاری;
	    اگر (کنارہ_mask0 && !صف->کنارہ_mask0)
		  جاری;
	    اگر (کنارہ_maskx && !صف->کنارہ_maskx)
		  جاری;
	    اگر (کنارہ_mask1 && !صف->کنارہ_mask1)
		  جاری;
	    اگر (cur.mask0 != (cur.mask0 & صف->mask0))
		  جاری;
	    اگر (cur.maskx != (cur.maskx & صف->maskx))
		  جاری;
	    اگر (cur.mask1 != (cur.mask1 & صف->mask1))
		  جاری;

	    اگر (cur.mask0 & (1 << پورٹ_گنتی()))
		  واپس BIT4_0;
	    اگر (cur.mask1 & (1 << پورٹ_گنتی()))
		  واپس BIT4_1;
	    اگر (cur.maskx & (1 << پورٹ_گنتی()))
		  واپس BIT4_X;

	    أكد(0);
	    واپس BIT4_X;
      }

      واپس BIT4_X;
}

vvp_udp_fun_core::vvp_udp_fun_core(vvp_net_t*net, vvp_udp_s*def)
: vvp_wide_fun_core(net, def->پورٹ_گنتی())
{
      def_ = def;
      cur_برآمد_2 = def_->get_init();
	// Assume initially that all the inputs are 1'bx
      current_.mask0 = 0;
      current_.mask1 = 0;
      current_.maskx = ~ ((-1UL) << پورٹ_گنتی());

        // If the initial value is 0 or 1, schedule the initial assignment
        // normally, so that any sensitive always processes can be started
        // first.
      اگر (cur_برآمد_2 != BIT4_X)
	    جدول_وقت_generic(یہ, 0, جھوٹ);
      ورنہ
	    جدول_وقت_functor(یہ);
}

vvp_udp_fun_core::~vvp_udp_fun_core()
{
}

/*
 * This method is used to propagate the initial value on startup.
 */
خالی vvp_udp_fun_core::تسلسل_تسلسل()
{
      vvp_vector4_t tmp (1);
      tmp.سیٹ_ثنائی_ہندسہ(0, cur_برآمد_2);
      propagate_vec4(tmp);
}

خالی vvp_udp_fun_core::recv_vec4_from_inputs(غیرعلامتی پورٹ)
{
	/* For now, assume udps are 1-bit wide. */
      غیرعلامتی پورٹ_چوڑائی = value(پورٹ).size();
      أكد(پورٹ_چوڑائی == 1);

      غیرعلامتی طویل نقاب = 1UL << پورٹ;

      udp_سطحیں_جدول prev = current_;

      انتخاب (value(پورٹ).value(0)) {

	  حالت BIT4_0:
	    current_.mask0 |= نقاب;
	    current_.mask1 &= ~نقاب;
	    current_.maskx &= ~نقاب;
	    توڑو;
	  حالت BIT4_1:
	    current_.mask0 &= ~نقاب;
	    current_.mask1 |= نقاب;
	    current_.maskx &= ~نقاب;
	    توڑو;
	  طےشدہ:
	    current_.mask0 &= ~نقاب;
	    current_.mask1 &= ~نقاب;
	    current_.maskx |= نقاب;
	    توڑو;
      }

      vvp_bit4_t برآمد_ثنائی_ہندسہ = def_->calculate_output(current_, prev, cur_برآمد_2);

      اگر (برآمد_ثنائی_ہندسہ == cur_برآمد_2)
	    واپس;

      cur_برآمد_2 = برآمد_ثنائی_ہندسہ;
      جدول_وقت_functor(یہ);
}


/*
 * This function is called by the parser in response to a .udp
 * node. We create the nodes needed to integrate the UDP into the
 * netlist. The definition should be parsed already.
 */
خالی compile_udp_functor(حرف*label, حرف*type,
			 غیرعلامتی argc, ساخت symb_s*argv)
{
      ساخت vvp_udp_s *def = udp_ڈھونڈو(type);
      أكد(def);
      آزاد(type);

      vvp_net_t*ptr = نیا vvp_net_t;
      vvp_udp_fun_core*core = نیا vvp_udp_fun_core(ptr, def);
      ptr->fun = core;

      define_functor_علامت(label, ptr);
      آزاد(label);

      wide_inputs_connect(core, argc, argv);
      آزاد(argv);
}
