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

#ਅਨ੍ਤਰ੍ਭਾਵ "udp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "schedule.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "symbols.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "compile.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <cassert>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstdlib>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstring>
#ਅਨ੍ਤਰ੍ਭਾਵ <cstdio>
#ਅਨ੍ਤਰ੍ਭਾਵ <iostream>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
#ਅਨ੍ਤਰ੍ਭਾਵ "vvp_cleanup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ivl_alloc.h"
#ਜੇ_ਅੰਤ

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

// We may need these later when we build the VPI interface to
// UDP definitions.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਸਥਿਰ vvp_udp_s **udp_defns = 0;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ udp_defns_ਗਿਣਤੀ = 0;

ਖਾਲੀ udp_defns_delete()
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < udp_defns_ਗਿਣਤੀ; idx += 1) {
	    ਜੇ (udp_defns[idx]->is_sequential()) {
		  vvp_udp_seq_s *obj = static_cast<vvp_udp_seq_s *>
		                       (udp_defns[idx]);
		  ਮਿਟਾਓ obj;
	    } ਨਹੀਂਤਾਂ {
		  vvp_udp_comb_s *obj = static_cast<vvp_udp_comb_s *>
		                        (udp_defns[idx]);
		  ਮਿਟਾਓ obj;
	    }
      }
      ਮੁਕਤ(udp_defns);
      udp_defns = 0;
      udp_defns_ਗਿਣਤੀ = 0;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ_t udp_ਸਾਰਣੀ;

ਖਾਲੀ delete_udp_symbols()
{
      delete_ਚਿੰਨ੍ਹ_ਸਾਰਣੀ(udp_ਸਾਰਣੀ);
      udp_ਸਾਰਣੀ = 0;
}

ਬਣਤਰ vvp_udp_s *udp_ਲੱਭੋ(ਅਚਲ ਅੱਖਰ *label)
{
      ਚਿੰਨ੍ਹ_ਮੁੱਲ_t v = sym_get_ਮੁੱਲ(udp_ਸਾਰਣੀ, label);
      ਵਾਪਸ (ਬਣਤਰ vvp_udp_s *)v.ptr;
}

ostream& ਸੰਚਾਲਕ <<(ostream&o, ਅਚਲ ਬਣਤਰ udp_ਪੱਧਰ_ਸਾਰਣੀ&ਸਾਰਣੀ)
{
      o << "[" << hex << ਸਾਰਣੀ.mask0
	<< "/" << hex << ਸਾਰਣੀ.mask1
	<< "/" << hex << ਸਾਰਣੀ.maskx << "]";
      ਵਾਪਸ o;
}

vvp_udp_s::vvp_udp_s(ਅਚਲ ਅੱਖਰ*label, ਅੱਖਰ*name__, ਬਿਨਾਨਿਸ਼ਾਨੀ ports,
                     vvp_bit4_t init, ਤਾਰਕਿਕ type)
: name_(name__), ports_(ports), init_(init), seq_(type)
{
      ਜੇ (!udp_ਸਾਰਣੀ)
	    udp_ਸਾਰਣੀ = new_ਚਿੰਨ੍ਹ_ਸਾਰਣੀ();

      ਪ੍ਰਤਿਜ੍ਞਾ( !udp_ਲੱਭੋ(label) );

      ਚਿੰਨ੍ਹ_ਮੁੱਲ_t v;
      v.ptr = ਇਹ;
      sym_ਸੈੱਟ_ਮੁੱਲ(udp_ਸਾਰਣੀ, label, v);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      udp_defns_ਗਿਣਤੀ += 1;
      udp_defns = static_cast<vvp_udp_s **>(realloc(udp_defns,
                  udp_defns_ਗਿਣਤੀ*ਆਕਾਰ(vvp_udp_s **)));
      udp_defns[udp_defns_ਗਿਣਤੀ-1] = ਇਹ;
#ਜੇ_ਅੰਤ
}

vvp_udp_s::~vvp_udp_s()
{
      ਮਿਟਾਓ[] name_;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ vvp_udp_s::ਪੋਰਟ_ਗਿਣਤੀ() ਅਚਲ
{
      ਵਾਪਸ ports_;
}

vvp_bit4_t vvp_udp_s::get_init() ਅਚਲ
{
      ਵਾਪਸ init_;
}

vvp_udp_comb_s::vvp_udp_comb_s(ਅਚਲ ਅੱਖਰ*label, ਅੱਖਰ*name__, ਬਿਨਾਨਿਸ਼ਾਨੀ ports)
: vvp_udp_s(label, name__, ports, BIT4_X, ਝੂਠ)
{
      levels0_ = 0;
      levels1_ = 0;
      nlevels0_ = 0;
      nlevels1_ = 0;
}

vvp_udp_comb_s::~vvp_udp_comb_s()
{
      ਮਿਟਾਓ[] levels0_;
      ਮਿਟਾਓ[] levels1_;
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
vvp_bit4_t vvp_udp_comb_s::test_ਪੱਧਰ(ਅਚਲ udp_ਪੱਧਰ_ਸਾਰਣੀ&cur)
{
	/* To test for a row match, test that the mask0, mask1 and
	   maskx vectors all have bits set where the matching
	   cur.mask0/1/x vectors have bits set. It is possible that a
	   levels0_[idx] vector has more bits set then the cur mask,
	   but this is OK and these bits are to be ignored. */

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    ਜੇ (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  ਜਾਰੀ;
	    ਜੇ (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  ਜਾਰੀ;
	    ਜੇ (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  ਜਾਰੀ;

	    ਵਾਪਸ BIT4_0;
      }

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    ਜੇ (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  ਜਾਰੀ;
	    ਜੇ (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  ਜਾਰੀ;
	    ਜੇ (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  ਜਾਰੀ;

	    ਵਾਪਸ BIT4_1;
      }

      ਵਾਪਸ BIT4_X;
}

vvp_bit4_t vvp_udp_comb_s::calculate_output(ਅਚਲ udp_ਪੱਧਰ_ਸਾਰਣੀ&cur,
					    ਅਚਲ udp_ਪੱਧਰ_ਸਾਰਣੀ&,
					    vvp_bit4_t)
{
      ਵਾਪਸ test_ਪੱਧਰ(cur);
}

ਸਥਿਰ ਖਾਲੀ or_based_on_char(udp_ਪੱਧਰ_ਸਾਰਣੀ&cur, ਅੱਖਰ flag,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਚੋਣ (flag) {
	  ਹਾਲਤ '0':
	    cur.mask0 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;
	  ਹਾਲਤ '1':
	    cur.mask1 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;
	  ਹਾਲਤ 'x':
	    cur.maskx |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;
	  ਹਾਲਤ 'b':
	    cur.mask0 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.mask1 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;
	  ਹਾਲਤ 'l':
	    cur.mask0 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.maskx |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;
	  ਹਾਲਤ 'h':
	    cur.maskx |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.mask1 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;
	  ਹਾਲਤ '?':
	    cur.mask0 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.maskx |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.mask1 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;
	  ਮੂਲ:
	    fprintf(stderr, "Unsupported flag %c(%d).\n", flag, flag);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }
}

ਖਾਲੀ vvp_udp_comb_s::compile_ਸਾਰਣੀ(ਅੱਖਰ**tab)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ nrows0 = 0, nrows1 = 0;

	/* First run through the table to figure out the number of
	   rows I need for each kind of table. */
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  tab[idx] ;  idx += 1) {
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਲੜੀ_ਲੰਬਾਈ(tab[idx]) == ਪੋਰਟ_ਗਿਣਤੀ() + 1);
	    ਚੋਣ (tab[idx][ਪੋਰਟ_ਗਿਣਤੀ()]) {
		ਹਾਲਤ '0':
		  nrows0 += 1;
		  ਰੋਕੋ;
		ਹਾਲਤ '1':
		  nrows1 += 1;
		  ਰੋਕੋ;
		ਹਾਲਤ 'x':
		  ਰੋਕੋ;
		ਮੂਲ:
		  fprintf(stderr, "Unsupported entry %c(%d).\n",
		          tab[idx][ਪੋਰਟ_ਗਿਣਤੀ()], tab[idx][ਪੋਰਟ_ਗਿਣਤੀ()]);
		  ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    }
      }

      nlevels0_ = nrows0;
      levels0_ = ਨਵਾਂ udp_ਪੱਧਰ_ਸਾਰਣੀ[nlevels0_];

      nlevels1_ = nrows1;
      levels1_ = ਨਵਾਂ udp_ਪੱਧਰ_ਸਾਰਣੀ[nlevels1_];

      nrows0 = 0;
      nrows1 = 0;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  tab[idx] ;  idx += 1) {
	    ਬਣਤਰ udp_ਪੱਧਰ_ਸਾਰਣੀ cur;
	    cur.mask0 = 0;
	    cur.mask1 = 0;
	    cur.maskx = 0;
	    ਜੇ (ਪੋਰਟ_ਗਿਣਤੀ() > 8*ਆਕਾਰ(cur.mask0)) {
		  fprintf(stderr, "internal error: primitive port count=%u "
			  " > %zu\n", ਪੋਰਟ_ਗਿਣਤੀ(), ਆਕਾਰ(cur.mask0));
		  ਪ੍ਰਤਿਜ੍ਞਾ(ਪੋਰਟ_ਗਿਣਤੀ() <= 8*ਆਕਾਰ(cur.mask0));
	    }
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ pp = 0 ;  pp < ਪੋਰਟ_ਗਿਣਤੀ() ;  pp += 1) {
		  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ = 1UL << pp;
		  or_based_on_char(cur, tab[idx][pp], ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ);
	    }

	    ਚੋਣ (tab[idx][ਪੋਰਟ_ਗਿਣਤੀ()]) {
		ਹਾਲਤ '0':
		  levels0_[nrows0++] = cur;
		  ਰੋਕੋ;
		ਹਾਲਤ '1':
		  levels1_[nrows1++] = cur;
		  ਰੋਕੋ;
		ਮੂਲ:
		  ਰੋਕੋ;
	    }
	    ਮਿਟਾਓ[] tab[idx];
      }
      ਮੁਕਤ(tab);

      ਪ੍ਰਤਿਜ੍ਞਾ(nrows0 == nlevels0_);
      ਪ੍ਰਤਿਜ੍ਞਾ(nrows1 == nlevels1_);
}

vvp_udp_seq_s::vvp_udp_seq_s(ਅਚਲ ਅੱਖਰ*label, ਅੱਖਰ*name__,
			     ਬਿਨਾਨਿਸ਼ਾਨੀ ports, vvp_bit4_t init)
: vvp_udp_s(label, name__, ports, init, ਸੱਚ)
{
      levels0_ = 0;
      levels1_ = 0;
      levelsx_ = 0;
      ਪੱਧਰ_L = 0;

      nlevels0_ = 0;
      nlevels1_ = 0;
      nlevelsx_ = 0;
      nlevelsL_ = 0;

      edges0_ = 0;
      edges1_ = 0;
      ਕਿਨਾਰੇ_L = 0;

      nedges0_ = 0;
      nedges1_ = 0;
      nedgesL_ = 0;
}

vvp_udp_seq_s::~vvp_udp_seq_s()
{
      ਮਿਟਾਓ[] levels0_;
      ਮਿਟਾਓ[] levels1_;
      ਮਿਟਾਓ[] levelsx_;
      ਮਿਟਾਓ[] ਪੱਧਰ_L;
      ਮਿਟਾਓ[] edges0_;
      ਮਿਟਾਓ[] edges1_;
      ਮਿਟਾਓ[] ਕਿਨਾਰੇ_L;
}

ਖਾਲੀ ਕਿਨਾਰਾ_based_on_char(ਬਣਤਰ udp_ਕਿਨਾਰੇ_ਸਾਰਣੀ&cur, ਅੱਖਰ chr, ਬਿਨਾਨਿਸ਼ਾਨੀ pos)
{
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ = 1UL << pos;

      ਚੋਣ (chr) {
	  ਹਾਲਤ '0':
	    cur.mask0 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;
	  ਹਾਲਤ '1':
	    cur.mask1 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;
	  ਹਾਲਤ 'x':
	    cur.maskx |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;
	  ਹਾਲਤ 'b':
	    cur.mask0 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.mask1 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;
	  ਹਾਲਤ 'l':
	    cur.mask0 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.maskx |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;
	  ਹਾਲਤ 'h':
	    cur.maskx |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.mask1 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;
	  ਹਾਲਤ '?':
	    cur.mask0 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.maskx |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.mask1 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    ਰੋਕੋ;

	  ਹਾਲਤ 'B': // (x?) edge
	    cur.mask0 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.mask1 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.ਕਿਨਾਰਾ_position = pos;
	    cur.ਕਿਨਾਰਾ_mask0 = 0;
	    cur.ਕਿਨਾਰਾ_maskx = 1;
	    cur.ਕਿਨਾਰਾ_mask1 = 0;
	    ਰੋਕੋ;
	  ਹਾਲਤ 'f': // (10) edge
	    cur.mask0 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.ਕਿਨਾਰਾ_position = pos;
	    cur.ਕਿਨਾਰਾ_mask0 = 0;
	    cur.ਕਿਨਾਰਾ_maskx = 0;
	    cur.ਕਿਨਾਰਾ_mask1 = 1;
	    ਰੋਕੋ;
	  ਹਾਲਤ 'F': // (x0) edge
	    cur.mask0 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.ਕਿਨਾਰਾ_position = pos;
	    cur.ਕਿਨਾਰਾ_mask0 = 0;
	    cur.ਕਿਨਾਰਾ_maskx = 1;
	    cur.ਕਿਨਾਰਾ_mask1 = 0;
	    ਰੋਕੋ;
	  ਹਾਲਤ 'M': // (1x) edge
	    cur.maskx |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.ਕਿਨਾਰਾ_position = pos;
	    cur.ਕਿਨਾਰਾ_mask0 = 0;
	    cur.ਕਿਨਾਰਾ_maskx = 0;
	    cur.ਕਿਨਾਰਾ_mask1 = 1;
	    ਰੋਕੋ;
	  ਹਾਲਤ 'N': // (1x) and (10) edge
	    cur.mask0 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.maskx |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.ਕਿਨਾਰਾ_position = pos;
	    cur.ਕਿਨਾਰਾ_mask0 = 0;
	    cur.ਕਿਨਾਰਾ_maskx = 0;
	    cur.ਕਿਨਾਰਾ_mask1 = 1;
	    ਰੋਕੋ;
	  ਹਾਲਤ 'P': // (0x) and (01) edge
	    cur.maskx |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.mask1 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.ਕਿਨਾਰਾ_position = pos;
	    cur.ਕਿਨਾਰਾ_mask0 = 1;
	    cur.ਕਿਨਾਰਾ_maskx = 0;
	    cur.ਕਿਨਾਰਾ_mask1 = 0;
	    ਰੋਕੋ;
	  ਹਾਲਤ 'q': // (bx) edge
	    cur.maskx |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.ਕਿਨਾਰਾ_position = pos;
	    cur.ਕਿਨਾਰਾ_mask0 = 1;
	    cur.ਕਿਨਾਰਾ_maskx = 0;
	    cur.ਕਿਨਾਰਾ_mask1 = 1;
	    ਰੋਕੋ;
	  ਹਾਲਤ 'Q': // (0x) edge
	    cur.maskx |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.ਕਿਨਾਰਾ_position = pos;
	    cur.ਕਿਨਾਰਾ_mask0 = 1;
	    cur.ਕਿਨਾਰਾ_maskx = 0;
	    cur.ਕਿਨਾਰਾ_mask1 = 0;
	    ਰੋਕੋ;
	  ਹਾਲਤ 'r': // (01) edge
	    cur.mask1 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.ਕਿਨਾਰਾ_position = pos;
	    cur.ਕਿਨਾਰਾ_mask0 = 1;
	    cur.ਕਿਨਾਰਾ_maskx = 0;
	    cur.ਕਿਨਾਰਾ_mask1 = 0;
	    ਰੋਕੋ;
	  ਹਾਲਤ 'R': // (x1) edge
	    cur.mask1 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.ਕਿਨਾਰਾ_position = pos;
	    cur.ਕਿਨਾਰਾ_mask0 = 0;
	    cur.ਕਿਨਾਰਾ_maskx = 1;
	    cur.ਕਿਨਾਰਾ_mask1 = 0;
	    ਰੋਕੋ;
	  ਹਾਲਤ '%': // (?x) edge
	    cur.maskx |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.ਕਿਨਾਰਾ_position = pos;
	    cur.ਕਿਨਾਰਾ_mask0 = 1;
	    cur.ਕਿਨਾਰਾ_maskx = 0;
	    cur.ਕਿਨਾਰਾ_mask1 = 1;
	    ਰੋਕੋ;
	  ਹਾਲਤ '+': // (?1) edge
	    cur.mask1 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.ਕਿਨਾਰਾ_position = pos;
	    cur.ਕਿਨਾਰਾ_mask0 = 1;
	    cur.ਕਿਨਾਰਾ_maskx = 1;
	    cur.ਕਿਨਾਰਾ_mask1 = 0;
	    ਰੋਕੋ;
	  ਹਾਲਤ '_': // (?0) edge
	    cur.mask0 |= ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ;
	    cur.ਕਿਨਾਰਾ_position = pos;
	    cur.ਕਿਨਾਰਾ_mask0 = 0;
	    cur.ਕਿਨਾਰਾ_maskx = 1;
	    cur.ਕਿਨਾਰਾ_mask1 = 1;
	    ਰੋਕੋ;
	  ਮੂਲ:
	    fprintf(stderr, "internal error: unknown edge code: %c\n", chr);
	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
      }
}

ਖਾਲੀ vvp_udp_seq_s::compile_ਸਾਰਣੀ(ਅੱਖਰ**tab)
{

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  tab[idx] ;  idx += 1) {
	    ਅਚਲ ਅੱਖਰ*ਪੰਕਤੀ = tab[idx];
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਲੜੀ_ਲੰਬਾਈ(ਪੰਕਤੀ) == ਪੋਰਟ_ਗਿਣਤੀ() + 2);

	    ਜੇ (strspn(ਪੰਕਤੀ, "01xblh?") >= ਪੋਰਟ_ਗਿਣਤੀ()+1) {

		  ਚੋਣ (ਪੰਕਤੀ[ਪੋਰਟ_ਗਿਣਤੀ()+1]) {
		      ਹਾਲਤ '0':
			nlevels0_ += 1;
			ਰੋਕੋ;
		      ਹਾਲਤ '1':
			nlevels1_ += 1;
			ਰੋਕੋ;
		      ਹਾਲਤ 'x':
			nlevelsx_ += 1;
			ਰੋਕੋ;
		      ਹਾਲਤ '-':
			nlevelsL_ += 1;
			ਰੋਕੋ;
		      ਮੂਲ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        ਪੰਕਤੀ[ਪੋਰਟ_ਗਿਣਤੀ()+1], ਪੰਕਤੀ[ਪੋਰਟ_ਗਿਣਤੀ()+1]);
			ਪ੍ਰਤਿਜ੍ਞਾ(0);
			ਰੋਕੋ;
		  }

	    } ਨਹੀਂਤਾਂ {
		    /* Rows that have n or p edges will need to be
		       expanded into 2 rows. */
		  ਬਿਨਾਨਿਸ਼ਾਨੀ extra = 0;
		  ਜੇ (strchr(ਪੰਕਤੀ,'n'))
			extra = 1;
		  ਜੇ (strchr(ਪੰਕਤੀ,'p'))
			extra = 1;
		  ਜੇ (strchr(ਪੰਕਤੀ,'*'))
			extra = 2;

		  ਚੋਣ (ਪੰਕਤੀ[ਪੋਰਟ_ਗਿਣਤੀ()+1]) {
		      ਹਾਲਤ '0':
			nedges0_ += 1 + extra;
			ਰੋਕੋ;
		      ਹਾਲਤ '1':
			nedges1_ += 1 + extra;
			ਰੋਕੋ;
		      ਹਾਲਤ 'x':
			ਰੋਕੋ;
		      ਹਾਲਤ '-':
			nedgesL_ += 1 + extra;
			ਰੋਕੋ;
		      ਮੂਲ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        ਪੰਕਤੀ[ਪੋਰਟ_ਗਿਣਤੀ()+1], ਪੰਕਤੀ[ਪੋਰਟ_ਗਿਣਤੀ()+1]);
			ਪ੍ਰਤਿਜ੍ਞਾ(0);
			ਰੋਕੋ;
		  }
	    }
      }

      levels0_ = ਨਵਾਂ udp_ਪੱਧਰ_ਸਾਰਣੀ[nlevels0_];
      levels1_ = ਨਵਾਂ udp_ਪੱਧਰ_ਸਾਰਣੀ[nlevels1_];
      levelsx_ = ਨਵਾਂ udp_ਪੱਧਰ_ਸਾਰਣੀ[nlevelsx_];
      ਪੱਧਰ_L = ਨਵਾਂ udp_ਪੱਧਰ_ਸਾਰਣੀ[nlevelsL_];
      edges0_ = ਨਵਾਂ udp_ਕਿਨਾਰੇ_ਸਾਰਣੀ[nedges0_];
      edges1_ = ਨਵਾਂ udp_ਕਿਨਾਰੇ_ਸਾਰਣੀ[nedges1_];
      ਕਿਨਾਰੇ_L = ਨਵਾਂ udp_ਕਿਨਾਰੇ_ਸਾਰਣੀ[nedgesL_];

      ਬਿਨਾਨਿਸ਼ਾਨੀ idx_lev0 = 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx_lev1 = 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx_levx = 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx_levL = 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx_edg0 = 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx_edg1 = 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx_edgL = 0;

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  tab[idx] ;  idx += 1) {
	    ਅਚਲ ਅੱਖਰ*ਪੰਕਤੀ = tab[idx];

	    ਜੇ (strspn(ਪੰਕਤੀ, "01xblh?") >= ਪੋਰਟ_ਗਿਣਤੀ()+1) {
		  ਬਣਤਰ udp_ਪੱਧਰ_ਸਾਰਣੀ cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ pp = 0 ;  pp < ਪੋਰਟ_ਗਿਣਤੀ() ;  pp += 1) {
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ = 1UL << pp;
			or_based_on_char(cur, ਪੰਕਤੀ[pp+1], ਨਕਾਬ_ਦੋਆਧਾਰੀ_ਅੰਕ);
		  }

		  or_based_on_char(cur, ਪੰਕਤੀ[0], 1UL << ਪੋਰਟ_ਗਿਣਤੀ());

		  ਚੋਣ (ਪੰਕਤੀ[ਪੋਰਟ_ਗਿਣਤੀ()+1]) {
		      ਹਾਲਤ '0':
			levels0_[idx_lev0++] = cur;
			ਰੋਕੋ;
		      ਹਾਲਤ '1':
			levels1_[idx_lev1++] = cur;
			ਰੋਕੋ;
		      ਹਾਲਤ 'x':
			levelsx_[idx_levx++] = cur;
			ਰੋਕੋ;
		      ਹਾਲਤ '-':
			ਪੱਧਰ_L[idx_levL++] = cur;
			ਰੋਕੋ;
		      ਮੂਲ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        ਪੰਕਤੀ[ਪੋਰਟ_ਗਿਣਤੀ()+1], ਪੰਕਤੀ[ਪੋਰਟ_ਗਿਣਤੀ()+1]);
			ਪ੍ਰਤਿਜ੍ਞਾ(0);
			ਰੋਕੋ;
		  }

	    } ਨਹੀਂਤਾਂ {
		  ਬਣਤਰ udp_ਕਿਨਾਰੇ_ਸਾਰਣੀ cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  cur.ਕਿਨਾਰਾ_position = 0;
		  cur.ਕਿਨਾਰਾ_mask0 = 0;
		  cur.ਕਿਨਾਰਾ_mask1 = 0;
		  cur.ਕਿਨਾਰਾ_maskx = 0;

		  ਤਾਰਕਿਕ need_ext0_ਸਾਰਣੀ = ਝੂਠ;
		  ਬਣਤਰ udp_ਕਿਨਾਰੇ_ਸਾਰਣੀ ext0;
		  ext0.mask0 = 0;
		  ext0.mask1 = 0;
		  ext0.maskx = 0;
		  ext0.ਕਿਨਾਰਾ_position = 0;
		  ext0.ਕਿਨਾਰਾ_mask0 = 0;
		  ext0.ਕਿਨਾਰਾ_mask1 = 0;
		  ext0.ਕਿਨਾਰਾ_maskx = 0;

		  ਤਾਰਕਿਕ need_ext1_ਸਾਰਣੀ = ਝੂਠ;
		  ਬਣਤਰ udp_ਕਿਨਾਰੇ_ਸਾਰਣੀ ext1;
		  ext1.mask0 = 0;
		  ext1.mask1 = 0;
		  ext1.maskx = 0;
		  ext1.ਕਿਨਾਰਾ_position = 0;
		  ext1.ਕਿਨਾਰਾ_mask0 = 0;
		  ext1.ਕਿਨਾਰਾ_mask1 = 0;
		  ext1.ਕਿਨਾਰਾ_maskx = 0;

		  ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ pp = 0 ;  pp < ਪੋਰਟ_ਗਿਣਤੀ() ; pp += 1) {
			ਚੋਣ (ਪੰਕਤੀ[pp+1]) {
			    ਹਾਲਤ 'n':
			      ਕਿਨਾਰਾ_based_on_char(cur, 'N', pp);
			      ਕਿਨਾਰਾ_based_on_char(ext0, '_', pp);
			      need_ext0_ਸਾਰਣੀ = ਸੱਚ;
			      ਰੋਕੋ;
			    ਹਾਲਤ 'p':
			      ਕਿਨਾਰਾ_based_on_char(cur, 'P', pp);
			      ਕਿਨਾਰਾ_based_on_char(ext0, '+', pp);
			      need_ext0_ਸਾਰਣੀ = ਸੱਚ;
			      ਰੋਕੋ;
			    ਹਾਲਤ '*':
			      ਕਿਨਾਰਾ_based_on_char(cur, 'P', pp);
			      ਕਿਨਾਰਾ_based_on_char(ext0, 'N', pp);
			      ਕਿਨਾਰਾ_based_on_char(ext1, 'B', pp);
			      need_ext0_ਸਾਰਣੀ = ਸੱਚ;
			      need_ext1_ਸਾਰਣੀ = ਸੱਚ;
			      ਰੋਕੋ;
			    ਮੂਲ:
			      ਕਿਨਾਰਾ_based_on_char(cur, ਪੰਕਤੀ[pp+1], pp);
			      ਕਿਨਾਰਾ_based_on_char(ext0, ਪੰਕਤੀ[pp+1], pp);
			      ਕਿਨਾਰਾ_based_on_char(ext1, ਪੰਕਤੀ[pp+1], pp);
			      ਰੋਕੋ;
			}
		  }
		  ਕਿਨਾਰਾ_based_on_char(cur, ਪੰਕਤੀ[0], ਪੋਰਟ_ਗਿਣਤੀ());
		  ਕਿਨਾਰਾ_based_on_char(ext0, ਪੰਕਤੀ[0], ਪੋਰਟ_ਗਿਣਤੀ());
		  ਕਿਨਾਰਾ_based_on_char(ext1, ਪੰਕਤੀ[0], ਪੋਰਟ_ਗਿਣਤੀ());

		  ਚੋਣ (ਪੰਕਤੀ[ਪੋਰਟ_ਗਿਣਤੀ()+1]) {
		      ਹਾਲਤ '0':
			edges0_[idx_edg0++] = cur;
			ਜੇ (need_ext0_ਸਾਰਣੀ)
			      edges0_[idx_edg0++] = ext0;
			ਜੇ (need_ext1_ਸਾਰਣੀ)
			      edges0_[idx_edg0++] = ext1;
			ਰੋਕੋ;
		      ਹਾਲਤ '1':
			edges1_[idx_edg1++] = cur;
			ਜੇ (need_ext0_ਸਾਰਣੀ)
			      edges1_[idx_edg1++] = ext0;
			ਜੇ (need_ext1_ਸਾਰਣੀ)
			      edges1_[idx_edg1++] = ext1;
			ਰੋਕੋ;
		      ਹਾਲਤ 'x':
			ਰੋਕੋ;
		      ਹਾਲਤ '-':
			ਕਿਨਾਰੇ_L[idx_edgL++] = cur;
			ਜੇ (need_ext0_ਸਾਰਣੀ)
			      ਕਿਨਾਰੇ_L[idx_edgL++] = ext0;
			ਜੇ (need_ext1_ਸਾਰਣੀ)
			      ਕਿਨਾਰੇ_L[idx_edgL++] = ext1;
			ਰੋਕੋ;
		      ਮੂਲ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        ਪੰਕਤੀ[ਪੋਰਟ_ਗਿਣਤੀ()+1], ਪੰਕਤੀ[ਪੋਰਟ_ਗਿਣਤੀ()+1]);
			ਪ੍ਰਤਿਜ੍ਞਾ(0);
			ਰੋਕੋ;
		  }

	    }
	    ਮਿਟਾਓ[] tab[idx];
      }
      ਮੁਕਤ(tab);

      ਪ੍ਰਤਿਜ੍ਞਾ(idx_edg0 == nedges0_);
      ਪ੍ਰਤਿਜ੍ਞਾ(idx_edg1 == nedges1_);
      ਪ੍ਰਤਿਜ੍ਞਾ(idx_edgL == nedgesL_);

}

ਤਾਰਕਿਕ ਸੰਚਾਲਕ == (ਅਚਲ udp_ਪੱਧਰ_ਸਾਰਣੀ&a, ਅਚਲ udp_ਪੱਧਰ_ਸਾਰਣੀ&b)
{
      ਜੇ (a.mask0 != b.mask0)
	    ਵਾਪਸ ਝੂਠ;
      ਜੇ (a.mask1 != b.mask1)
	    ਵਾਪਸ ਝੂਠ;
      ਜੇ (a.maskx != b.maskx)
	    ਵਾਪਸ ਝੂਠ;
      ਵਾਪਸ ਸੱਚ;
}

vvp_bit4_t vvp_udp_seq_s::calculate_output(ਅਚਲ udp_ਪੱਧਰ_ਸਾਰਣੀ&cur,
					   ਅਚਲ udp_ਪੱਧਰ_ਸਾਰਣੀ&prev,
					   vvp_bit4_t cur_ਬਾਹਰਲਾ)
{
      ਜੇ (cur == prev)
	    ਵਾਪਸ cur_ਬਾਹਰਲਾ;

      udp_ਪੱਧਰ_ਸਾਰਣੀ cur_tmp = cur;

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ_ਬਾਹਰਲਾ = 1UL << ਪੋਰਟ_ਗਿਣਤੀ();
      ਚੋਣ (cur_ਬਾਹਰਲਾ) {
	  ਹਾਲਤ BIT4_0:
	    cur_tmp.mask0 |= ਨਕਾਬ_ਬਾਹਰਲਾ;
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_1:
	    cur_tmp.mask1 |= ਨਕਾਬ_ਬਾਹਰਲਾ;
	    ਰੋਕੋ;
	  ਮੂਲ:
	    cur_tmp.maskx |= ਨਕਾਬ_ਬਾਹਰਲਾ;
	    ਰੋਕੋ;
      }

      vvp_bit4_t lev = test_ਪੱਧਰ_2(cur_tmp);
      ਜੇ (lev == BIT4_Z) {
	    lev = test_ਕਿਨਾਰੇ(cur_tmp, prev);
      }

      ਵਾਪਸ lev;
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
vvp_bit4_t vvp_udp_seq_s::test_ਪੱਧਰ_2(ਅਚਲ udp_ਪੱਧਰ_ਸਾਰਣੀ&cur)
{
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    ਜੇ (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  ਜਾਰੀ;
	    ਜੇ (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  ਜਾਰੀ;
	    ਜੇ (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  ਜਾਰੀ;

	    ਵਾਪਸ BIT4_0;
      }

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    ਜੇ (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  ਜਾਰੀ;
	    ਜੇ (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  ਜਾਰੀ;
	    ਜੇ (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  ਜਾਰੀ;

	    ਵਾਪਸ BIT4_1;
      }

	/* We need to test against an explicit X-output table, since
	   we need to distinguish from an X output and no match. */
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < nlevelsx_ ;  idx += 1) {
	    ਜੇ (cur.mask0 != (cur.mask0 & levelsx_[idx].mask0))
		  ਜਾਰੀ;
	    ਜੇ (cur.mask1 != (cur.mask1 & levelsx_[idx].mask1))
		  ਜਾਰੀ;
	    ਜੇ (cur.maskx != (cur.maskx & levelsx_[idx].maskx))
		  ਜਾਰੀ;

	    ਵਾਪਸ BIT4_X;
      }

	/* Test the table that requests the next output be the same as
	   the current output. This gets the current output from the
	   levels table that was passed in. */
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < nlevelsL_ ;  idx += 1) {
	    ਜੇ (cur.mask0 != (cur.mask0 & ਪੱਧਰ_L[idx].mask0))
		  ਜਾਰੀ;
	    ਜੇ (cur.mask1 != (cur.mask1 & ਪੱਧਰ_L[idx].mask1))
		  ਜਾਰੀ;
	    ਜੇ (cur.maskx != (cur.maskx & ਪੱਧਰ_L[idx].maskx))
		  ਜਾਰੀ;

	    ਜੇ (cur.mask0 & (1 << ਪੋਰਟ_ਗਿਣਤੀ()))
		  ਵਾਪਸ BIT4_0;
	    ਜੇ (cur.mask1 & (1 << ਪੋਰਟ_ਗਿਣਤੀ()))
		  ਵਾਪਸ BIT4_1;
	    ਜੇ (cur.maskx & (1 << ਪੋਰਟ_ਗਿਣਤੀ()))
		  ਵਾਪਸ BIT4_X;

	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਵਾਪਸ BIT4_X;
      }

	/* No explicit levels entry match. Return a Z to signal that
	   further testing is needed. */
      ਵਾਪਸ BIT4_Z;
}

vvp_bit4_t vvp_udp_seq_s::test_ਕਿਨਾਰੇ(ਅਚਲ udp_ਪੱਧਰ_ਸਾਰਣੀ&cur,
				      ਅਚਲ udp_ਪੱਧਰ_ਸਾਰਣੀ&prev)
{
	/* The edge_mask is true for all bits that are different in
	   the cur and prev tables. */
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ edge0_ਨਕਾਬ = cur.mask0 ^ prev.mask0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ edgex_ਨਕਾਬ = cur.maskx ^ prev.maskx;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ edge1_ਨਕਾਬ = cur.mask1 ^ prev.mask1;

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਕਿਨਾਰਾ_ਨਕਾਬ = edge0_ਨਕਾਬ|edgex_ਨਕਾਬ|edge1_ਨਕਾਬ;
      ਕਿਨਾਰਾ_ਨਕਾਬ &= ~ (-1UL << ਪੋਰਟ_ਗਿਣਤੀ());

	/* If there are no differences, then there are no edges. Give
	   up now. */
      ਜੇ (ਕਿਨਾਰਾ_ਨਕਾਬ == 0)
	    ਵਾਪਸ BIT4_X;

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਕਿਨਾਰਾ_position = 0;
      ਜਦੋਂ ((ਕਿਨਾਰਾ_ਨਕਾਬ&1) == 0) {
	    ਕਿਨਾਰਾ_ਨਕਾਬ >>= 1;
	    ਕਿਨਾਰਾ_position += 1;
      }

	/* We expect that there is exactly one edge in here. */
      ਪ੍ਰਤਿਜ੍ਞਾ(ਕਿਨਾਰਾ_ਨਕਾਬ == 1);

      ਕਿਨਾਰਾ_ਨਕਾਬ = 1UL << ਕਿਨਾਰਾ_position;

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਕਿਨਾਰਾ_mask0 = (prev.mask0&ਕਿਨਾਰਾ_ਨਕਾਬ)? 1 : 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਕਿਨਾਰਾ_maskx = (prev.maskx&ਕਿਨਾਰਾ_ਨਕਾਬ)? 1 : 0;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਕਿਨਾਰਾ_mask1 = (prev.mask1&ਕਿਨਾਰਾ_ਨਕਾਬ)? 1 : 0;


	/* Now the edge_position and edge_mask* variables have the
	   values we use to test the applicability of the edge_table
	   entries. */

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < nedges0_ ;  idx += 1) {
	    ਅਚਲ ਬਣਤਰ udp_ਕਿਨਾਰੇ_ਸਾਰਣੀ*ਪੰਕਤੀ = edges0_ + idx;

	    ਜੇ (ਪੰਕਤੀ->ਕਿਨਾਰਾ_position != ਕਿਨਾਰਾ_position)
		  ਜਾਰੀ;
	    ਜੇ (ਕਿਨਾਰਾ_mask0 && !ਪੰਕਤੀ->ਕਿਨਾਰਾ_mask0)
		  ਜਾਰੀ;
	    ਜੇ (ਕਿਨਾਰਾ_maskx && !ਪੰਕਤੀ->ਕਿਨਾਰਾ_maskx)
		  ਜਾਰੀ;
	    ਜੇ (ਕਿਨਾਰਾ_mask1 && !ਪੰਕਤੀ->ਕਿਨਾਰਾ_mask1)
		  ਜਾਰੀ;
	    ਜੇ (cur.mask0 != (cur.mask0 & ਪੰਕਤੀ->mask0))
		  ਜਾਰੀ;
	    ਜੇ (cur.maskx != (cur.maskx & ਪੰਕਤੀ->maskx))
		  ਜਾਰੀ;
	    ਜੇ (cur.mask1 != (cur.mask1 & ਪੰਕਤੀ->mask1))
		  ਜਾਰੀ;

	    ਵਾਪਸ BIT4_0;
      }

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < nedges1_ ;  idx += 1) {
	    ਅਚਲ ਬਣਤਰ udp_ਕਿਨਾਰੇ_ਸਾਰਣੀ*ਪੰਕਤੀ = edges1_ + idx;

	    ਜੇ (ਪੰਕਤੀ->ਕਿਨਾਰਾ_position != ਕਿਨਾਰਾ_position)
		  ਜਾਰੀ;
	    ਜੇ (ਕਿਨਾਰਾ_mask0 && !ਪੰਕਤੀ->ਕਿਨਾਰਾ_mask0)
		  ਜਾਰੀ;
	    ਜੇ (ਕਿਨਾਰਾ_maskx && !ਪੰਕਤੀ->ਕਿਨਾਰਾ_maskx)
		  ਜਾਰੀ;
	    ਜੇ (ਕਿਨਾਰਾ_mask1 && !ਪੰਕਤੀ->ਕਿਨਾਰਾ_mask1)
		  ਜਾਰੀ;
	    ਜੇ (cur.mask0 != (cur.mask0 & ਪੰਕਤੀ->mask0))
		  ਜਾਰੀ;
	    ਜੇ (cur.maskx != (cur.maskx & ਪੰਕਤੀ->maskx))
		  ਜਾਰੀ;
	    ਜੇ (cur.mask1 != (cur.mask1 & ਪੰਕਤੀ->mask1))
		  ਜਾਰੀ;

	    ਵਾਪਸ BIT4_1;
      }

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < nedgesL_ ;  idx += 1) {
	    ਅਚਲ ਬਣਤਰ udp_ਕਿਨਾਰੇ_ਸਾਰਣੀ*ਪੰਕਤੀ = ਕਿਨਾਰੇ_L + idx;

	    ਜੇ (ਪੰਕਤੀ->ਕਿਨਾਰਾ_position != ਕਿਨਾਰਾ_position)
		  ਜਾਰੀ;
	    ਜੇ (ਕਿਨਾਰਾ_mask0 && !ਪੰਕਤੀ->ਕਿਨਾਰਾ_mask0)
		  ਜਾਰੀ;
	    ਜੇ (ਕਿਨਾਰਾ_maskx && !ਪੰਕਤੀ->ਕਿਨਾਰਾ_maskx)
		  ਜਾਰੀ;
	    ਜੇ (ਕਿਨਾਰਾ_mask1 && !ਪੰਕਤੀ->ਕਿਨਾਰਾ_mask1)
		  ਜਾਰੀ;
	    ਜੇ (cur.mask0 != (cur.mask0 & ਪੰਕਤੀ->mask0))
		  ਜਾਰੀ;
	    ਜੇ (cur.maskx != (cur.maskx & ਪੰਕਤੀ->maskx))
		  ਜਾਰੀ;
	    ਜੇ (cur.mask1 != (cur.mask1 & ਪੰਕਤੀ->mask1))
		  ਜਾਰੀ;

	    ਜੇ (cur.mask0 & (1 << ਪੋਰਟ_ਗਿਣਤੀ()))
		  ਵਾਪਸ BIT4_0;
	    ਜੇ (cur.mask1 & (1 << ਪੋਰਟ_ਗਿਣਤੀ()))
		  ਵਾਪਸ BIT4_1;
	    ਜੇ (cur.maskx & (1 << ਪੋਰਟ_ਗਿਣਤੀ()))
		  ਵਾਪਸ BIT4_X;

	    ਪ੍ਰਤਿਜ੍ਞਾ(0);
	    ਵਾਪਸ BIT4_X;
      }

      ਵਾਪਸ BIT4_X;
}

vvp_udp_fun_core::vvp_udp_fun_core(vvp_net_t*net, vvp_udp_s*def)
: vvp_wide_fun_core(net, def->ਪੋਰਟ_ਗਿਣਤੀ())
{
      def_ = def;
      cur_ਬਾਹਰਲਾ_2 = def_->get_init();
	// Assume initially that all the inputs are 1'bx
      current_.mask0 = 0;
      current_.mask1 = 0;
      current_.maskx = ~ ((-1UL) << ਪੋਰਟ_ਗਿਣਤੀ());

        // If the initial value is 0 or 1, schedule the initial assignment
        // normally, so that any sensitive always processes can be started
        // first.
      ਜੇ (cur_ਬਾਹਰਲਾ_2 != BIT4_X)
	    ਸਮਾਂ_ਸਾਰਣੀ_generic(ਇਹ, 0, ਝੂਠ);
      ਨਹੀਂਤਾਂ
	    ਸਮਾਂ_ਸਾਰਣੀ_functor(ਇਹ);
}

vvp_udp_fun_core::~vvp_udp_fun_core()
{
}

/*
 * This method is used to propagate the initial value on startup.
 */
ਖਾਲੀ vvp_udp_fun_core::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      vvp_vector4_t tmp (1);
      tmp.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(0, cur_ਬਾਹਰਲਾ_2);
      propagate_vec4(tmp);
}

ਖਾਲੀ vvp_udp_fun_core::recv_vec4_from_inputs(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ)
{
	/* For now, assume udps are 1-bit wide. */
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੋਰਟ_ਚੌੜਾਈ = value(ਪੋਰਟ).size();
      ਪ੍ਰਤਿਜ੍ਞਾ(ਪੋਰਟ_ਚੌੜਾਈ == 1);

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਨਕਾਬ = 1UL << ਪੋਰਟ;

      udp_ਪੱਧਰ_ਸਾਰਣੀ prev = current_;

      ਚੋਣ (value(ਪੋਰਟ).value(0)) {

	  ਹਾਲਤ BIT4_0:
	    current_.mask0 |= ਨਕਾਬ;
	    current_.mask1 &= ~ਨਕਾਬ;
	    current_.maskx &= ~ਨਕਾਬ;
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_1:
	    current_.mask0 &= ~ਨਕਾਬ;
	    current_.mask1 |= ਨਕਾਬ;
	    current_.maskx &= ~ਨਕਾਬ;
	    ਰੋਕੋ;
	  ਮੂਲ:
	    current_.mask0 &= ~ਨਕਾਬ;
	    current_.mask1 &= ~ਨਕਾਬ;
	    current_.maskx |= ਨਕਾਬ;
	    ਰੋਕੋ;
      }

      vvp_bit4_t ਬਾਹਰਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ = def_->calculate_output(current_, prev, cur_ਬਾਹਰਲਾ_2);

      ਜੇ (ਬਾਹਰਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ == cur_ਬਾਹਰਲਾ_2)
	    ਵਾਪਸ;

      cur_ਬਾਹਰਲਾ_2 = ਬਾਹਰਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ;
      ਸਮਾਂ_ਸਾਰਣੀ_functor(ਇਹ);
}


/*
 * This function is called by the parser in response to a .udp
 * node. We create the nodes needed to integrate the UDP into the
 * netlist. The definition should be parsed already.
 */
ਖਾਲੀ compile_udp_functor(ਅੱਖਰ*label, ਅੱਖਰ*type,
			 ਬਿਨਾਨਿਸ਼ਾਨੀ argc, ਬਣਤਰ symb_s*argv)
{
      ਬਣਤਰ vvp_udp_s *def = udp_ਲੱਭੋ(type);
      ਪ੍ਰਤਿਜ੍ਞਾ(def);
      ਮੁਕਤ(type);

      vvp_net_t*ptr = ਨਵਾਂ vvp_net_t;
      vvp_udp_fun_core*core = ਨਵਾਂ vvp_udp_fun_core(ptr, def);
      ptr->fun = core;

      define_functor_ਚਿੰਨ੍ਹ(label, ptr);
      ਮੁਕਤ(label);

      wide_inputs_connect(core, argc, argv);
      ਮੁਕਤ(argv);
}
