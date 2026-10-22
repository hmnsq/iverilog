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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
#ಅನ್ತರ್ಭಾವ "vvp_cleanup.h"
#ಅನ್ತರ್ಭಾವ "ivl_alloc.h"
#ಆದರೆ_ಅಂತ್ಯ

ಬಳಸಿ ನಾಮಾಕಾಶ std;

// We may need these later when we build the VPI interface to
// UDP definitions.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಸ್ಥಿರ vvp_udp_s **udp_defns = 0;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ udp_defns_ಎಣಿಕೆ = 0;

ಶೂನ್ಯ udp_defns_delete()
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < udp_defns_ಎಣಿಕೆ; idx += 1) {
	    ಆದರೆ (udp_defns[idx]->is_sequential()) {
		  vvp_udp_seq_s *obj = static_cast<vvp_udp_seq_s *>
		                       (udp_defns[idx]);
		  ಅಳಿಸು obj;
	    } ಇಲ್ಲದಿದ್ದರೆ {
		  vvp_udp_comb_s *obj = static_cast<vvp_udp_comb_s *>
		                        (udp_defns[idx]);
		  ಅಳಿಸು obj;
	    }
      }
      ಬಿಡುಗಡೆ(udp_defns);
      udp_defns = 0;
      udp_defns_ಎಣಿಕೆ = 0;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_t udp_ಕೋಷ್ಟಕ;

ಶೂನ್ಯ delete_udp_symbols()
{
      delete_ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ(udp_ಕೋಷ್ಟಕ);
      udp_ಕೋಷ್ಟಕ = 0;
}

ರಚನೆ vvp_udp_s *udp_ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label)
{
      ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t v = sym_get_ಮೌಲ್ಯ(udp_ಕೋಷ್ಟಕ, label);
      ಹಿಂತಿರುಗಿ (ರಚನೆ vvp_udp_s *)v.ptr;
}

ostream& ಕ್ರಿಯಾಕಾರಕ <<(ostream&o, ಸ್ಥಿರಾಂಕ ರಚನೆ udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ&ಕೋಷ್ಟಕ)
{
      o << "[" << hex << ಕೋಷ್ಟಕ.mask0
	<< "/" << hex << ಕೋಷ್ಟಕ.mask1
	<< "/" << hex << ಕೋಷ್ಟಕ.maskx << "]";
      ಹಿಂತಿರುಗಿ o;
}

vvp_udp_s::vvp_udp_s(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*label, ಅಕ್ಷರ*name__, ಚಿಹ್ನೆರಹಿತ ports,
                     vvp_bit4_t init, ತಾರ್ಕಿಕ type)
: name_(name__), ports_(ports), init_(init), seq_(type)
{
      ಆದರೆ (!udp_ಕೋಷ್ಟಕ)
	    udp_ಕೋಷ್ಟಕ = new_ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ();

      ಪ್ರತಿಜ್ಞಾ( !udp_ಕಂಡುಹಿಡಿ(label) );

      ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t v;
      v.ptr = ಇದು;
      sym_ಗಣ_ಮೌಲ್ಯ(udp_ಕೋಷ್ಟಕ, label, v);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      udp_defns_ಎಣಿಕೆ += 1;
      udp_defns = static_cast<vvp_udp_s **>(realloc(udp_defns,
                  udp_defns_ಎಣಿಕೆ*ಗಾತ್ರ(vvp_udp_s **)));
      udp_defns[udp_defns_ಎಣಿಕೆ-1] = ಇದು;
#ಆದರೆ_ಅಂತ್ಯ
}

vvp_udp_s::~vvp_udp_s()
{
      ಅಳಿಸು[] name_;
}

ಚಿಹ್ನೆರಹಿತ vvp_udp_s::ಪೋರ್ಟ್_ಎಣಿಕೆ() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ ports_;
}

vvp_bit4_t vvp_udp_s::get_init() ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ init_;
}

vvp_udp_comb_s::vvp_udp_comb_s(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*label, ಅಕ್ಷರ*name__, ಚಿಹ್ನೆರಹಿತ ports)
: vvp_udp_s(label, name__, ports, BIT4_X, ಸುಳ್ಳು)
{
      levels0_ = 0;
      levels1_ = 0;
      nlevels0_ = 0;
      nlevels1_ = 0;
}

vvp_udp_comb_s::~vvp_udp_comb_s()
{
      ಅಳಿಸು[] levels0_;
      ಅಳಿಸು[] levels1_;
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
vvp_bit4_t vvp_udp_comb_s::test_ಮಟ್ಟಗಳು(ಸ್ಥಿರಾಂಕ udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ&cur)
{
	/* To test for a row match, test that the mask0, mask1 and
	   maskx vectors all have bits set where the matching
	   cur.mask0/1/x vectors have bits set. It is possible that a
	   levels0_[idx] vector has more bits set then the cur mask,
	   but this is OK and these bits are to be ignored. */

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    ಆದರೆ (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  ಮುಂದುವರಿ;
	    ಆದರೆ (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  ಮುಂದುವರಿ;
	    ಆದರೆ (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  ಮುಂದುವರಿ;

	    ಹಿಂತಿರುಗಿ BIT4_0;
      }

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    ಆದರೆ (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  ಮುಂದುವರಿ;
	    ಆದರೆ (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  ಮುಂದುವರಿ;
	    ಆದರೆ (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  ಮುಂದುವರಿ;

	    ಹಿಂತಿರುಗಿ BIT4_1;
      }

      ಹಿಂತಿರುಗಿ BIT4_X;
}

vvp_bit4_t vvp_udp_comb_s::calculate_output(ಸ್ಥಿರಾಂಕ udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ&cur,
					    ಸ್ಥಿರಾಂಕ udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ&,
					    vvp_bit4_t)
{
      ಹಿಂತಿರುಗಿ test_ಮಟ್ಟಗಳು(cur);
}

ಸ್ಥಿರ ಶೂನ್ಯ or_based_on_char(udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ&cur, ಅಕ್ಷರ flag,
			     ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ_ಬಿಟ್)
{
      ಆಯ್ಕೆ (flag) {
	  ಸ್ಥಿತಿ '0':
	    cur.mask0 |= ಮುಖವಾಡ_ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ '1':
	    cur.mask1 |= ಮುಖವಾಡ_ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'x':
	    cur.maskx |= ಮುಖವಾಡ_ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'b':
	    cur.mask0 |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.mask1 |= ಮುಖವಾಡ_ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'l':
	    cur.mask0 |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.maskx |= ಮುಖವಾಡ_ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'h':
	    cur.maskx |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.mask1 |= ಮುಖವಾಡ_ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ '?':
	    cur.mask0 |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.maskx |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.mask1 |= ಮುಖವಾಡ_ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "Unsupported flag %c(%d).\n", flag, flag);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }
}

ಶೂನ್ಯ vvp_udp_comb_s::compile_ಕೋಷ್ಟಕ(ಅಕ್ಷರ**tab)
{
      ಚಿಹ್ನೆರಹಿತ nrows0 = 0, nrows1 = 0;

	/* First run through the table to figure out the number of
	   rows I need for each kind of table. */
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  tab[idx] ;  idx += 1) {
	    ಪ್ರತಿಜ್ಞಾ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(tab[idx]) == ಪೋರ್ಟ್_ಎಣಿಕೆ() + 1);
	    ಆಯ್ಕೆ (tab[idx][ಪೋರ್ಟ್_ಎಣಿಕೆ()]) {
		ಸ್ಥಿತಿ '0':
		  nrows0 += 1;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '1':
		  nrows1 += 1;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'x':
		  ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		  fprintf(stderr, "Unsupported entry %c(%d).\n",
		          tab[idx][ಪೋರ್ಟ್_ಎಣಿಕೆ()], tab[idx][ಪೋರ್ಟ್_ಎಣಿಕೆ()]);
		  ಪ್ರತಿಜ್ಞಾ(0);
	    }
      }

      nlevels0_ = nrows0;
      levels0_ = ಹೊಸ udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ[nlevels0_];

      nlevels1_ = nrows1;
      levels1_ = ಹೊಸ udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ[nlevels1_];

      nrows0 = 0;
      nrows1 = 0;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  tab[idx] ;  idx += 1) {
	    ರಚನೆ udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ cur;
	    cur.mask0 = 0;
	    cur.mask1 = 0;
	    cur.maskx = 0;
	    ಆದರೆ (ಪೋರ್ಟ್_ಎಣಿಕೆ() > 8*ಗಾತ್ರ(cur.mask0)) {
		  fprintf(stderr, "internal error: primitive port count=%u "
			  " > %zu\n", ಪೋರ್ಟ್_ಎಣಿಕೆ(), ಗಾತ್ರ(cur.mask0));
		  ಪ್ರತಿಜ್ಞಾ(ಪೋರ್ಟ್_ಎಣಿಕೆ() <= 8*ಗಾತ್ರ(cur.mask0));
	    }
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ pp = 0 ;  pp < ಪೋರ್ಟ್_ಎಣಿಕೆ() ;  pp += 1) {
		  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ_ಬಿಟ್ = 1UL << pp;
		  or_based_on_char(cur, tab[idx][pp], ಮುಖವಾಡ_ಬಿಟ್);
	    }

	    ಆಯ್ಕೆ (tab[idx][ಪೋರ್ಟ್_ಎಣಿಕೆ()]) {
		ಸ್ಥಿತಿ '0':
		  levels0_[nrows0++] = cur;
		  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '1':
		  levels1_[nrows1++] = cur;
		  ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		  ನಿಲ್ಲಿಸು;
	    }
	    ಅಳಿಸು[] tab[idx];
      }
      ಬಿಡುಗಡೆ(tab);

      ಪ್ರತಿಜ್ಞಾ(nrows0 == nlevels0_);
      ಪ್ರತಿಜ್ಞಾ(nrows1 == nlevels1_);
}

vvp_udp_seq_s::vvp_udp_seq_s(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*label, ಅಕ್ಷರ*name__,
			     ಚಿಹ್ನೆರಹಿತ ports, vvp_bit4_t init)
: vvp_udp_s(label, name__, ports, init, ನಿಜ)
{
      levels0_ = 0;
      levels1_ = 0;
      levelsx_ = 0;
      ಮಟ್ಟಗಳು_L = 0;

      nlevels0_ = 0;
      nlevels1_ = 0;
      nlevelsx_ = 0;
      nlevelsL_ = 0;

      edges0_ = 0;
      edges1_ = 0;
      ಅಂಚುಗಳು_L = 0;

      nedges0_ = 0;
      nedges1_ = 0;
      nedgesL_ = 0;
}

vvp_udp_seq_s::~vvp_udp_seq_s()
{
      ಅಳಿಸು[] levels0_;
      ಅಳಿಸು[] levels1_;
      ಅಳಿಸು[] levelsx_;
      ಅಳಿಸು[] ಮಟ್ಟಗಳು_L;
      ಅಳಿಸು[] edges0_;
      ಅಳಿಸು[] edges1_;
      ಅಳಿಸು[] ಅಂಚುಗಳು_L;
}

ಶೂನ್ಯ ಅಂಚು_based_on_char(ರಚನೆ udp_ಅಂಚುಗಳು_ಕೋಷ್ಟಕ&cur, ಅಕ್ಷರ chr, ಚಿಹ್ನೆರಹಿತ pos)
{
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ_ಬಿಟ್ = 1UL << pos;

      ಆಯ್ಕೆ (chr) {
	  ಸ್ಥಿತಿ '0':
	    cur.mask0 |= ಮುಖವಾಡ_ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ '1':
	    cur.mask1 |= ಮುಖವಾಡ_ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'x':
	    cur.maskx |= ಮುಖವಾಡ_ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'b':
	    cur.mask0 |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.mask1 |= ಮುಖವಾಡ_ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'l':
	    cur.mask0 |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.maskx |= ಮುಖವಾಡ_ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'h':
	    cur.maskx |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.mask1 |= ಮುಖವಾಡ_ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ '?':
	    cur.mask0 |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.maskx |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.mask1 |= ಮುಖವಾಡ_ಬಿಟ್;
	    ನಿಲ್ಲಿಸು;

	  ಸ್ಥಿತಿ 'B': // (x?) edge
	    cur.mask0 |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.mask1 |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.ಅಂಚು_position = pos;
	    cur.ಅಂಚು_mask0 = 0;
	    cur.ಅಂಚು_maskx = 1;
	    cur.ಅಂಚು_mask1 = 0;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'f': // (10) edge
	    cur.mask0 |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.ಅಂಚು_position = pos;
	    cur.ಅಂಚು_mask0 = 0;
	    cur.ಅಂಚು_maskx = 0;
	    cur.ಅಂಚು_mask1 = 1;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'F': // (x0) edge
	    cur.mask0 |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.ಅಂಚು_position = pos;
	    cur.ಅಂಚು_mask0 = 0;
	    cur.ಅಂಚು_maskx = 1;
	    cur.ಅಂಚು_mask1 = 0;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'M': // (1x) edge
	    cur.maskx |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.ಅಂಚು_position = pos;
	    cur.ಅಂಚು_mask0 = 0;
	    cur.ಅಂಚು_maskx = 0;
	    cur.ಅಂಚು_mask1 = 1;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'N': // (1x) and (10) edge
	    cur.mask0 |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.maskx |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.ಅಂಚು_position = pos;
	    cur.ಅಂಚು_mask0 = 0;
	    cur.ಅಂಚು_maskx = 0;
	    cur.ಅಂಚು_mask1 = 1;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'P': // (0x) and (01) edge
	    cur.maskx |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.mask1 |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.ಅಂಚು_position = pos;
	    cur.ಅಂಚು_mask0 = 1;
	    cur.ಅಂಚು_maskx = 0;
	    cur.ಅಂಚು_mask1 = 0;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'q': // (bx) edge
	    cur.maskx |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.ಅಂಚು_position = pos;
	    cur.ಅಂಚು_mask0 = 1;
	    cur.ಅಂಚು_maskx = 0;
	    cur.ಅಂಚು_mask1 = 1;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'Q': // (0x) edge
	    cur.maskx |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.ಅಂಚು_position = pos;
	    cur.ಅಂಚು_mask0 = 1;
	    cur.ಅಂಚು_maskx = 0;
	    cur.ಅಂಚು_mask1 = 0;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'r': // (01) edge
	    cur.mask1 |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.ಅಂಚು_position = pos;
	    cur.ಅಂಚು_mask0 = 1;
	    cur.ಅಂಚು_maskx = 0;
	    cur.ಅಂಚು_mask1 = 0;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ 'R': // (x1) edge
	    cur.mask1 |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.ಅಂಚು_position = pos;
	    cur.ಅಂಚು_mask0 = 0;
	    cur.ಅಂಚು_maskx = 1;
	    cur.ಅಂಚು_mask1 = 0;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ '%': // (?x) edge
	    cur.maskx |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.ಅಂಚು_position = pos;
	    cur.ಅಂಚು_mask0 = 1;
	    cur.ಅಂಚು_maskx = 0;
	    cur.ಅಂಚು_mask1 = 1;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ '+': // (?1) edge
	    cur.mask1 |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.ಅಂಚು_position = pos;
	    cur.ಅಂಚು_mask0 = 1;
	    cur.ಅಂಚು_maskx = 1;
	    cur.ಅಂಚು_mask1 = 0;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ '_': // (?0) edge
	    cur.mask0 |= ಮುಖವಾಡ_ಬಿಟ್;
	    cur.ಅಂಚು_position = pos;
	    cur.ಅಂಚು_mask0 = 0;
	    cur.ಅಂಚು_maskx = 1;
	    cur.ಅಂಚು_mask1 = 1;
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    fprintf(stderr, "internal error: unknown edge code: %c\n", chr);
	    ಪ್ರತಿಜ್ಞಾ(0);
      }
}

ಶೂನ್ಯ vvp_udp_seq_s::compile_ಕೋಷ್ಟಕ(ಅಕ್ಷರ**tab)
{

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  tab[idx] ;  idx += 1) {
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಅಡ್ಡಸಾಲು = tab[idx];
	    ಪ್ರತಿಜ್ಞಾ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಅಡ್ಡಸಾಲು) == ಪೋರ್ಟ್_ಎಣಿಕೆ() + 2);

	    ಆದರೆ (strspn(ಅಡ್ಡಸಾಲು, "01xblh?") >= ಪೋರ್ಟ್_ಎಣಿಕೆ()+1) {

		  ಆಯ್ಕೆ (ಅಡ್ಡಸಾಲು[ಪೋರ್ಟ್_ಎಣಿಕೆ()+1]) {
		      ಸ್ಥಿತಿ '0':
			nlevels0_ += 1;
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ '1':
			nlevels1_ += 1;
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ 'x':
			nlevelsx_ += 1;
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ '-':
			nlevelsL_ += 1;
			ನಿಲ್ಲಿಸು;
		      ಪೂರ್ವನಿಯೋಜಿತ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        ಅಡ್ಡಸಾಲು[ಪೋರ್ಟ್_ಎಣಿಕೆ()+1], ಅಡ್ಡಸಾಲು[ಪೋರ್ಟ್_ಎಣಿಕೆ()+1]);
			ಪ್ರತಿಜ್ಞಾ(0);
			ನಿಲ್ಲಿಸು;
		  }

	    } ಇಲ್ಲದಿದ್ದರೆ {
		    /* Rows that have n or p edges will need to be
		       expanded into 2 rows. */
		  ಚಿಹ್ನೆರಹಿತ extra = 0;
		  ಆದರೆ (strchr(ಅಡ್ಡಸಾಲು,'n'))
			extra = 1;
		  ಆದರೆ (strchr(ಅಡ್ಡಸಾಲು,'p'))
			extra = 1;
		  ಆದರೆ (strchr(ಅಡ್ಡಸಾಲು,'*'))
			extra = 2;

		  ಆಯ್ಕೆ (ಅಡ್ಡಸಾಲು[ಪೋರ್ಟ್_ಎಣಿಕೆ()+1]) {
		      ಸ್ಥಿತಿ '0':
			nedges0_ += 1 + extra;
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ '1':
			nedges1_ += 1 + extra;
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ 'x':
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ '-':
			nedgesL_ += 1 + extra;
			ನಿಲ್ಲಿಸು;
		      ಪೂರ್ವನಿಯೋಜಿತ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        ಅಡ್ಡಸಾಲು[ಪೋರ್ಟ್_ಎಣಿಕೆ()+1], ಅಡ್ಡಸಾಲು[ಪೋರ್ಟ್_ಎಣಿಕೆ()+1]);
			ಪ್ರತಿಜ್ಞಾ(0);
			ನಿಲ್ಲಿಸು;
		  }
	    }
      }

      levels0_ = ಹೊಸ udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ[nlevels0_];
      levels1_ = ಹೊಸ udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ[nlevels1_];
      levelsx_ = ಹೊಸ udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ[nlevelsx_];
      ಮಟ್ಟಗಳು_L = ಹೊಸ udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ[nlevelsL_];
      edges0_ = ಹೊಸ udp_ಅಂಚುಗಳು_ಕೋಷ್ಟಕ[nedges0_];
      edges1_ = ಹೊಸ udp_ಅಂಚುಗಳು_ಕೋಷ್ಟಕ[nedges1_];
      ಅಂಚುಗಳು_L = ಹೊಸ udp_ಅಂಚುಗಳು_ಕೋಷ್ಟಕ[nedgesL_];

      ಚಿಹ್ನೆರಹಿತ idx_lev0 = 0;
      ಚಿಹ್ನೆರಹಿತ idx_lev1 = 0;
      ಚಿಹ್ನೆರಹಿತ idx_levx = 0;
      ಚಿಹ್ನೆರಹಿತ idx_levL = 0;
      ಚಿಹ್ನೆರಹಿತ idx_edg0 = 0;
      ಚಿಹ್ನೆರಹಿತ idx_edg1 = 0;
      ಚಿಹ್ನೆರಹಿತ idx_edgL = 0;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  tab[idx] ;  idx += 1) {
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಅಡ್ಡಸಾಲು = tab[idx];

	    ಆದರೆ (strspn(ಅಡ್ಡಸಾಲು, "01xblh?") >= ಪೋರ್ಟ್_ಎಣಿಕೆ()+1) {
		  ರಚನೆ udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ pp = 0 ;  pp < ಪೋರ್ಟ್_ಎಣಿಕೆ() ;  pp += 1) {
			ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ_ಬಿಟ್ = 1UL << pp;
			or_based_on_char(cur, ಅಡ್ಡಸಾಲು[pp+1], ಮುಖವಾಡ_ಬಿಟ್);
		  }

		  or_based_on_char(cur, ಅಡ್ಡಸಾಲು[0], 1UL << ಪೋರ್ಟ್_ಎಣಿಕೆ());

		  ಆಯ್ಕೆ (ಅಡ್ಡಸಾಲು[ಪೋರ್ಟ್_ಎಣಿಕೆ()+1]) {
		      ಸ್ಥಿತಿ '0':
			levels0_[idx_lev0++] = cur;
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ '1':
			levels1_[idx_lev1++] = cur;
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ 'x':
			levelsx_[idx_levx++] = cur;
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ '-':
			ಮಟ್ಟಗಳು_L[idx_levL++] = cur;
			ನಿಲ್ಲಿಸು;
		      ಪೂರ್ವನಿಯೋಜಿತ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        ಅಡ್ಡಸಾಲು[ಪೋರ್ಟ್_ಎಣಿಕೆ()+1], ಅಡ್ಡಸಾಲು[ಪೋರ್ಟ್_ಎಣಿಕೆ()+1]);
			ಪ್ರತಿಜ್ಞಾ(0);
			ನಿಲ್ಲಿಸು;
		  }

	    } ಇಲ್ಲದಿದ್ದರೆ {
		  ರಚನೆ udp_ಅಂಚುಗಳು_ಕೋಷ್ಟಕ cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  cur.ಅಂಚು_position = 0;
		  cur.ಅಂಚು_mask0 = 0;
		  cur.ಅಂಚು_mask1 = 0;
		  cur.ಅಂಚು_maskx = 0;

		  ತಾರ್ಕಿಕ need_ext0_ಕೋಷ್ಟಕ = ಸುಳ್ಳು;
		  ರಚನೆ udp_ಅಂಚುಗಳು_ಕೋಷ್ಟಕ ext0;
		  ext0.mask0 = 0;
		  ext0.mask1 = 0;
		  ext0.maskx = 0;
		  ext0.ಅಂಚು_position = 0;
		  ext0.ಅಂಚು_mask0 = 0;
		  ext0.ಅಂಚು_mask1 = 0;
		  ext0.ಅಂಚು_maskx = 0;

		  ತಾರ್ಕಿಕ need_ext1_ಕೋಷ್ಟಕ = ಸುಳ್ಳು;
		  ರಚನೆ udp_ಅಂಚುಗಳು_ಕೋಷ್ಟಕ ext1;
		  ext1.mask0 = 0;
		  ext1.mask1 = 0;
		  ext1.maskx = 0;
		  ext1.ಅಂಚು_position = 0;
		  ext1.ಅಂಚು_mask0 = 0;
		  ext1.ಅಂಚು_mask1 = 0;
		  ext1.ಅಂಚು_maskx = 0;

		  ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ pp = 0 ;  pp < ಪೋರ್ಟ್_ಎಣಿಕೆ() ; pp += 1) {
			ಆಯ್ಕೆ (ಅಡ್ಡಸಾಲು[pp+1]) {
			    ಸ್ಥಿತಿ 'n':
			      ಅಂಚು_based_on_char(cur, 'N', pp);
			      ಅಂಚು_based_on_char(ext0, '_', pp);
			      need_ext0_ಕೋಷ್ಟಕ = ನಿಜ;
			      ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ 'p':
			      ಅಂಚು_based_on_char(cur, 'P', pp);
			      ಅಂಚು_based_on_char(ext0, '+', pp);
			      need_ext0_ಕೋಷ್ಟಕ = ನಿಜ;
			      ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ '*':
			      ಅಂಚು_based_on_char(cur, 'P', pp);
			      ಅಂಚು_based_on_char(ext0, 'N', pp);
			      ಅಂಚು_based_on_char(ext1, 'B', pp);
			      need_ext0_ಕೋಷ್ಟಕ = ನಿಜ;
			      need_ext1_ಕೋಷ್ಟಕ = ನಿಜ;
			      ನಿಲ್ಲಿಸು;
			    ಪೂರ್ವನಿಯೋಜಿತ:
			      ಅಂಚು_based_on_char(cur, ಅಡ್ಡಸಾಲು[pp+1], pp);
			      ಅಂಚು_based_on_char(ext0, ಅಡ್ಡಸಾಲು[pp+1], pp);
			      ಅಂಚು_based_on_char(ext1, ಅಡ್ಡಸಾಲು[pp+1], pp);
			      ನಿಲ್ಲಿಸು;
			}
		  }
		  ಅಂಚು_based_on_char(cur, ಅಡ್ಡಸಾಲು[0], ಪೋರ್ಟ್_ಎಣಿಕೆ());
		  ಅಂಚು_based_on_char(ext0, ಅಡ್ಡಸಾಲು[0], ಪೋರ್ಟ್_ಎಣಿಕೆ());
		  ಅಂಚು_based_on_char(ext1, ಅಡ್ಡಸಾಲು[0], ಪೋರ್ಟ್_ಎಣಿಕೆ());

		  ಆಯ್ಕೆ (ಅಡ್ಡಸಾಲು[ಪೋರ್ಟ್_ಎಣಿಕೆ()+1]) {
		      ಸ್ಥಿತಿ '0':
			edges0_[idx_edg0++] = cur;
			ಆದರೆ (need_ext0_ಕೋಷ್ಟಕ)
			      edges0_[idx_edg0++] = ext0;
			ಆದರೆ (need_ext1_ಕೋಷ್ಟಕ)
			      edges0_[idx_edg0++] = ext1;
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ '1':
			edges1_[idx_edg1++] = cur;
			ಆದರೆ (need_ext0_ಕೋಷ್ಟಕ)
			      edges1_[idx_edg1++] = ext0;
			ಆದರೆ (need_ext1_ಕೋಷ್ಟಕ)
			      edges1_[idx_edg1++] = ext1;
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ 'x':
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ '-':
			ಅಂಚುಗಳು_L[idx_edgL++] = cur;
			ಆದರೆ (need_ext0_ಕೋಷ್ಟಕ)
			      ಅಂಚುಗಳು_L[idx_edgL++] = ext0;
			ಆದರೆ (need_ext1_ಕೋಷ್ಟಕ)
			      ಅಂಚುಗಳು_L[idx_edgL++] = ext1;
			ನಿಲ್ಲಿಸು;
		      ಪೂರ್ವನಿಯೋಜಿತ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        ಅಡ್ಡಸಾಲು[ಪೋರ್ಟ್_ಎಣಿಕೆ()+1], ಅಡ್ಡಸಾಲು[ಪೋರ್ಟ್_ಎಣಿಕೆ()+1]);
			ಪ್ರತಿಜ್ಞಾ(0);
			ನಿಲ್ಲಿಸು;
		  }

	    }
	    ಅಳಿಸು[] tab[idx];
      }
      ಬಿಡುಗಡೆ(tab);

      ಪ್ರತಿಜ್ಞಾ(idx_edg0 == nedges0_);
      ಪ್ರತಿಜ್ಞಾ(idx_edg1 == nedges1_);
      ಪ್ರತಿಜ್ಞಾ(idx_edgL == nedgesL_);

}

ತಾರ್ಕಿಕ ಕ್ರಿಯಾಕಾರಕ == (ಸ್ಥಿರಾಂಕ udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ&a, ಸ್ಥಿರಾಂಕ udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ&b)
{
      ಆದರೆ (a.mask0 != b.mask0)
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      ಆದರೆ (a.mask1 != b.mask1)
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      ಆದರೆ (a.maskx != b.maskx)
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
      ಹಿಂತಿರುಗಿ ನಿಜ;
}

vvp_bit4_t vvp_udp_seq_s::calculate_output(ಸ್ಥಿರಾಂಕ udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ&cur,
					   ಸ್ಥಿರಾಂಕ udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ&prev,
					   vvp_bit4_t cur_ಹೊರಹೋಗು)
{
      ಆದರೆ (cur == prev)
	    ಹಿಂತಿರುಗಿ cur_ಹೊರಹೋಗು;

      udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ cur_tmp = cur;

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ_ಹೊರಹೋಗು = 1UL << ಪೋರ್ಟ್_ಎಣಿಕೆ();
      ಆಯ್ಕೆ (cur_ಹೊರಹೋಗು) {
	  ಸ್ಥಿತಿ BIT4_0:
	    cur_tmp.mask0 |= ಮುಖವಾಡ_ಹೊರಹೋಗು;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_1:
	    cur_tmp.mask1 |= ಮುಖವಾಡ_ಹೊರಹೋಗು;
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    cur_tmp.maskx |= ಮುಖವಾಡ_ಹೊರಹೋಗು;
	    ನಿಲ್ಲಿಸು;
      }

      vvp_bit4_t lev = test_ಮಟ್ಟಗಳು_2(cur_tmp);
      ಆದರೆ (lev == BIT4_Z) {
	    lev = test_ಅಂಚುಗಳು(cur_tmp, prev);
      }

      ಹಿಂತಿರುಗಿ lev;
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
vvp_bit4_t vvp_udp_seq_s::test_ಮಟ್ಟಗಳು_2(ಸ್ಥಿರಾಂಕ udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ&cur)
{
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    ಆದರೆ (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  ಮುಂದುವರಿ;
	    ಆದರೆ (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  ಮುಂದುವರಿ;
	    ಆದರೆ (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  ಮುಂದುವರಿ;

	    ಹಿಂತಿರುಗಿ BIT4_0;
      }

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    ಆದರೆ (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  ಮುಂದುವರಿ;
	    ಆದರೆ (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  ಮುಂದುವರಿ;
	    ಆದರೆ (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  ಮುಂದುವರಿ;

	    ಹಿಂತಿರುಗಿ BIT4_1;
      }

	/* We need to test against an explicit X-output table, since
	   we need to distinguish from an X output and no match. */
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < nlevelsx_ ;  idx += 1) {
	    ಆದರೆ (cur.mask0 != (cur.mask0 & levelsx_[idx].mask0))
		  ಮುಂದುವರಿ;
	    ಆದರೆ (cur.mask1 != (cur.mask1 & levelsx_[idx].mask1))
		  ಮುಂದುವರಿ;
	    ಆದರೆ (cur.maskx != (cur.maskx & levelsx_[idx].maskx))
		  ಮುಂದುವರಿ;

	    ಹಿಂತಿರುಗಿ BIT4_X;
      }

	/* Test the table that requests the next output be the same as
	   the current output. This gets the current output from the
	   levels table that was passed in. */
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < nlevelsL_ ;  idx += 1) {
	    ಆದರೆ (cur.mask0 != (cur.mask0 & ಮಟ್ಟಗಳು_L[idx].mask0))
		  ಮುಂದುವರಿ;
	    ಆದರೆ (cur.mask1 != (cur.mask1 & ಮಟ್ಟಗಳು_L[idx].mask1))
		  ಮುಂದುವರಿ;
	    ಆದರೆ (cur.maskx != (cur.maskx & ಮಟ್ಟಗಳು_L[idx].maskx))
		  ಮುಂದುವರಿ;

	    ಆದರೆ (cur.mask0 & (1 << ಪೋರ್ಟ್_ಎಣಿಕೆ()))
		  ಹಿಂತಿರುಗಿ BIT4_0;
	    ಆದರೆ (cur.mask1 & (1 << ಪೋರ್ಟ್_ಎಣಿಕೆ()))
		  ಹಿಂತಿರುಗಿ BIT4_1;
	    ಆದರೆ (cur.maskx & (1 << ಪೋರ್ಟ್_ಎಣಿಕೆ()))
		  ಹಿಂತಿರುಗಿ BIT4_X;

	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಹಿಂತಿರುಗಿ BIT4_X;
      }

	/* No explicit levels entry match. Return a Z to signal that
	   further testing is needed. */
      ಹಿಂತಿರುಗಿ BIT4_Z;
}

vvp_bit4_t vvp_udp_seq_s::test_ಅಂಚುಗಳು(ಸ್ಥಿರಾಂಕ udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ&cur,
				      ಸ್ಥಿರಾಂಕ udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ&prev)
{
	/* The edge_mask is true for all bits that are different in
	   the cur and prev tables. */
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ edge0_ಮುಖವಾಡ = cur.mask0 ^ prev.mask0;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ edgex_ಮುಖವಾಡ = cur.maskx ^ prev.maskx;
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ edge1_ಮುಖವಾಡ = cur.mask1 ^ prev.mask1;

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಅಂಚು_ಮುಖವಾಡ = edge0_ಮುಖವಾಡ|edgex_ಮುಖವಾಡ|edge1_ಮುಖವಾಡ;
      ಅಂಚು_ಮುಖವಾಡ &= ~ (-1UL << ಪೋರ್ಟ್_ಎಣಿಕೆ());

	/* If there are no differences, then there are no edges. Give
	   up now. */
      ಆದರೆ (ಅಂಚು_ಮುಖವಾಡ == 0)
	    ಹಿಂತಿರುಗಿ BIT4_X;

      ಚಿಹ್ನೆರಹಿತ ಅಂಚು_position = 0;
      ಆಗ ((ಅಂಚು_ಮುಖವಾಡ&1) == 0) {
	    ಅಂಚು_ಮುಖವಾಡ >>= 1;
	    ಅಂಚು_position += 1;
      }

	/* We expect that there is exactly one edge in here. */
      ಪ್ರತಿಜ್ಞಾ(ಅಂಚು_ಮುಖವಾಡ == 1);

      ಅಂಚು_ಮುಖವಾಡ = 1UL << ಅಂಚು_position;

      ಚಿಹ್ನೆರಹಿತ ಅಂಚು_mask0 = (prev.mask0&ಅಂಚು_ಮುಖವಾಡ)? 1 : 0;
      ಚಿಹ್ನೆರಹಿತ ಅಂಚು_maskx = (prev.maskx&ಅಂಚು_ಮುಖವಾಡ)? 1 : 0;
      ಚಿಹ್ನೆರಹಿತ ಅಂಚು_mask1 = (prev.mask1&ಅಂಚು_ಮುಖವಾಡ)? 1 : 0;


	/* Now the edge_position and edge_mask* variables have the
	   values we use to test the applicability of the edge_table
	   entries. */

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < nedges0_ ;  idx += 1) {
	    ಸ್ಥಿರಾಂಕ ರಚನೆ udp_ಅಂಚುಗಳು_ಕೋಷ್ಟಕ*ಅಡ್ಡಸಾಲು = edges0_ + idx;

	    ಆದರೆ (ಅಡ್ಡಸಾಲು->ಅಂಚು_position != ಅಂಚು_position)
		  ಮುಂದುವರಿ;
	    ಆದರೆ (ಅಂಚು_mask0 && !ಅಡ್ಡಸಾಲು->ಅಂಚು_mask0)
		  ಮುಂದುವರಿ;
	    ಆದರೆ (ಅಂಚು_maskx && !ಅಡ್ಡಸಾಲು->ಅಂಚು_maskx)
		  ಮುಂದುವರಿ;
	    ಆದರೆ (ಅಂಚು_mask1 && !ಅಡ್ಡಸಾಲು->ಅಂಚು_mask1)
		  ಮುಂದುವರಿ;
	    ಆದರೆ (cur.mask0 != (cur.mask0 & ಅಡ್ಡಸಾಲು->mask0))
		  ಮುಂದುವರಿ;
	    ಆದರೆ (cur.maskx != (cur.maskx & ಅಡ್ಡಸಾಲು->maskx))
		  ಮುಂದುವರಿ;
	    ಆದರೆ (cur.mask1 != (cur.mask1 & ಅಡ್ಡಸಾಲು->mask1))
		  ಮುಂದುವರಿ;

	    ಹಿಂತಿರುಗಿ BIT4_0;
      }

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < nedges1_ ;  idx += 1) {
	    ಸ್ಥಿರಾಂಕ ರಚನೆ udp_ಅಂಚುಗಳು_ಕೋಷ್ಟಕ*ಅಡ್ಡಸಾಲು = edges1_ + idx;

	    ಆದರೆ (ಅಡ್ಡಸಾಲು->ಅಂಚು_position != ಅಂಚು_position)
		  ಮುಂದುವರಿ;
	    ಆದರೆ (ಅಂಚು_mask0 && !ಅಡ್ಡಸಾಲು->ಅಂಚು_mask0)
		  ಮುಂದುವರಿ;
	    ಆದರೆ (ಅಂಚು_maskx && !ಅಡ್ಡಸಾಲು->ಅಂಚು_maskx)
		  ಮುಂದುವರಿ;
	    ಆದರೆ (ಅಂಚು_mask1 && !ಅಡ್ಡಸಾಲು->ಅಂಚು_mask1)
		  ಮುಂದುವರಿ;
	    ಆದರೆ (cur.mask0 != (cur.mask0 & ಅಡ್ಡಸಾಲು->mask0))
		  ಮುಂದುವರಿ;
	    ಆದರೆ (cur.maskx != (cur.maskx & ಅಡ್ಡಸಾಲು->maskx))
		  ಮುಂದುವರಿ;
	    ಆದರೆ (cur.mask1 != (cur.mask1 & ಅಡ್ಡಸಾಲು->mask1))
		  ಮುಂದುವರಿ;

	    ಹಿಂತಿರುಗಿ BIT4_1;
      }

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < nedgesL_ ;  idx += 1) {
	    ಸ್ಥಿರಾಂಕ ರಚನೆ udp_ಅಂಚುಗಳು_ಕೋಷ್ಟಕ*ಅಡ್ಡಸಾಲು = ಅಂಚುಗಳು_L + idx;

	    ಆದರೆ (ಅಡ್ಡಸಾಲು->ಅಂಚು_position != ಅಂಚು_position)
		  ಮುಂದುವರಿ;
	    ಆದರೆ (ಅಂಚು_mask0 && !ಅಡ್ಡಸಾಲು->ಅಂಚು_mask0)
		  ಮುಂದುವರಿ;
	    ಆದರೆ (ಅಂಚು_maskx && !ಅಡ್ಡಸಾಲು->ಅಂಚು_maskx)
		  ಮುಂದುವರಿ;
	    ಆದರೆ (ಅಂಚು_mask1 && !ಅಡ್ಡಸಾಲು->ಅಂಚು_mask1)
		  ಮುಂದುವರಿ;
	    ಆದರೆ (cur.mask0 != (cur.mask0 & ಅಡ್ಡಸಾಲು->mask0))
		  ಮುಂದುವರಿ;
	    ಆದರೆ (cur.maskx != (cur.maskx & ಅಡ್ಡಸಾಲು->maskx))
		  ಮುಂದುವರಿ;
	    ಆದರೆ (cur.mask1 != (cur.mask1 & ಅಡ್ಡಸಾಲು->mask1))
		  ಮುಂದುವರಿ;

	    ಆದರೆ (cur.mask0 & (1 << ಪೋರ್ಟ್_ಎಣಿಕೆ()))
		  ಹಿಂತಿರುಗಿ BIT4_0;
	    ಆದರೆ (cur.mask1 & (1 << ಪೋರ್ಟ್_ಎಣಿಕೆ()))
		  ಹಿಂತಿರುಗಿ BIT4_1;
	    ಆದರೆ (cur.maskx & (1 << ಪೋರ್ಟ್_ಎಣಿಕೆ()))
		  ಹಿಂತಿರುಗಿ BIT4_X;

	    ಪ್ರತಿಜ್ಞಾ(0);
	    ಹಿಂತಿರುಗಿ BIT4_X;
      }

      ಹಿಂತಿರುಗಿ BIT4_X;
}

vvp_udp_fun_core::vvp_udp_fun_core(vvp_net_t*net, vvp_udp_s*def)
: vvp_wide_fun_core(net, def->ಪೋರ್ಟ್_ಎಣಿಕೆ())
{
      def_ = def;
      cur_ಹೊರಹೋಗು_2 = def_->get_init();
	// Assume initially that all the inputs are 1'bx
      current_.mask0 = 0;
      current_.mask1 = 0;
      current_.maskx = ~ ((-1UL) << ಪೋರ್ಟ್_ಎಣಿಕೆ());

        // If the initial value is 0 or 1, schedule the initial assignment
        // normally, so that any sensitive always processes can be started
        // first.
      ಆದರೆ (cur_ಹೊರಹೋಗು_2 != BIT4_X)
	    ವೇಳಾಪಟ್ಟಿ_generic(ಇದು, 0, ಸುಳ್ಳು);
      ಇಲ್ಲದಿದ್ದರೆ
	    ವೇಳಾಪಟ್ಟಿ_functor(ಇದು);
}

vvp_udp_fun_core::~vvp_udp_fun_core()
{
}

/*
 * This method is used to propagate the initial value on startup.
 */
ಶೂನ್ಯ vvp_udp_fun_core::ಸತತ_ಸತತ()
{
      vvp_vector4_t tmp (1);
      tmp.ಗಣ_ಬಿಟ್(0, cur_ಹೊರಹೋಗು_2);
      propagate_vec4(tmp);
}

ಶೂನ್ಯ vvp_udp_fun_core::recv_vec4_from_inputs(ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್)
{
	/* For now, assume udps are 1-bit wide. */
      ಚಿಹ್ನೆರಹಿತ ಪೋರ್ಟ್_ಅಗಲ = value(ಪೋರ್ಟ್).size();
      ಪ್ರತಿಜ್ಞಾ(ಪೋರ್ಟ್_ಅಗಲ == 1);

      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮುಖವಾಡ = 1UL << ಪೋರ್ಟ್;

      udp_ಮಟ್ಟಗಳು_ಕೋಷ್ಟಕ prev = current_;

      ಆಯ್ಕೆ (value(ಪೋರ್ಟ್).value(0)) {

	  ಸ್ಥಿತಿ BIT4_0:
	    current_.mask0 |= ಮುಖವಾಡ;
	    current_.mask1 &= ~ಮುಖವಾಡ;
	    current_.maskx &= ~ಮುಖವಾಡ;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_1:
	    current_.mask0 &= ~ಮುಖವಾಡ;
	    current_.mask1 |= ಮುಖವಾಡ;
	    current_.maskx &= ~ಮುಖವಾಡ;
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    current_.mask0 &= ~ಮುಖವಾಡ;
	    current_.mask1 &= ~ಮುಖವಾಡ;
	    current_.maskx |= ಮುಖವಾಡ;
	    ನಿಲ್ಲಿಸು;
      }

      vvp_bit4_t ಹೊರಹೋಗು_ಬಿಟ್ = def_->calculate_output(current_, prev, cur_ಹೊರಹೋಗು_2);

      ಆದರೆ (ಹೊರಹೋಗು_ಬಿಟ್ == cur_ಹೊರಹೋಗು_2)
	    ಹಿಂತಿರುಗಿ;

      cur_ಹೊರಹೋಗು_2 = ಹೊರಹೋಗು_ಬಿಟ್;
      ವೇಳಾಪಟ್ಟಿ_functor(ಇದು);
}


/*
 * This function is called by the parser in response to a .udp
 * node. We create the nodes needed to integrate the UDP into the
 * netlist. The definition should be parsed already.
 */
ಶೂನ್ಯ compile_udp_functor(ಅಕ್ಷರ*label, ಅಕ್ಷರ*type,
			 ಚಿಹ್ನೆರಹಿತ argc, ರಚನೆ symb_s*argv)
{
      ರಚನೆ vvp_udp_s *def = udp_ಕಂಡುಹಿಡಿ(type);
      ಪ್ರತಿಜ್ಞಾ(def);
      ಬಿಡುಗಡೆ(type);

      vvp_net_t*ptr = ಹೊಸ vvp_net_t;
      vvp_udp_fun_core*core = ಹೊಸ vvp_udp_fun_core(ptr, def);
      ptr->fun = core;

      define_functor_ಚಿಹ್ನೆ(label, ptr);
      ಬಿಡುಗಡೆ(label);

      wide_inputs_connect(core, argc, argv);
      ಬಿಡುಗಡೆ(argv);
}
