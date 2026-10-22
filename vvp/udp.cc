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

#అన్తర్భావ "udp.h"
#అన్తర్భావ "schedule.h"
#అన్తర్భావ "symbols.h"
#అన్తర్భావ "compile.h"
#అన్తర్భావ "config.h"
#అన్తర్భావ <cassert>
#అన్తర్భావ <cstdlib>
#అన్తర్భావ <cstring>
#అన్తర్భావ <cstdio>
#అన్తర్భావ <iostream>
#నిర్వచితమైతే CHECK_WITH_VALGRIND
#అన్తర్భావ "vvp_cleanup.h"
#అన్తర్భావ "ivl_alloc.h"
#అయితే_ముగింపు

ఉపయోగించి నామాకాశ std;

// We may need these later when we build the VPI interface to
// UDP definitions.
#నిర్వచితమైతే CHECK_WITH_VALGRIND
స్థిర vvp_udp_s **udp_defns = 0;
స్థిర గుర్తులేని udp_defns_లెక్క = 0;

శూన్య udp_defns_delete()
{
      కొరకు (గుర్తులేని idx = 0; idx < udp_defns_లెక్క; idx += 1) {
	    ఒకవేళ (udp_defns[idx]->is_sequential()) {
		  vvp_udp_seq_s *obj = static_cast<vvp_udp_seq_s *>
		                       (udp_defns[idx]);
		  తొలగించు obj;
	    } లేకపోతే {
		  vvp_udp_comb_s *obj = static_cast<vvp_udp_comb_s *>
		                        (udp_defns[idx]);
		  తొలగించు obj;
	    }
      }
      విడుదల(udp_defns);
      udp_defns = 0;
      udp_defns_లెక్క = 0;
}
#అయితే_ముగింపు

స్థిర చిహ్నం_పట్టిక_t udp_పట్టిక;

శూన్య delete_udp_symbols()
{
      delete_చిహ్నం_పట్టిక(udp_పట్టిక);
      udp_పట్టిక = 0;
}

నిర్మాణం vvp_udp_s *udp_కనుగొను(స్థిరరాశి అక్షరం *label)
{
      చిహ్నం_విలువ_t v = sym_get_విలువ(udp_పట్టిక, label);
      తిరిగి (నిర్మాణం vvp_udp_s *)v.ptr;
}

ostream& క్రియాకారకం <<(ostream&o, స్థిరరాశి నిర్మాణం udp_స్థాయిలు_పట్టిక&పట్టిక)
{
      o << "[" << hex << పట్టిక.mask0
	<< "/" << hex << పట్టిక.mask1
	<< "/" << hex << పట్టిక.maskx << "]";
      తిరిగి o;
}

vvp_udp_s::vvp_udp_s(స్థిరరాశి అక్షరం*label, అక్షరం*name__, గుర్తులేని ports,
                     vvp_bit4_t init, తార్కిక type)
: name_(name__), ports_(ports), init_(init), seq_(type)
{
      ఒకవేళ (!udp_పట్టిక)
	    udp_పట్టిక = new_చిహ్నం_పట్టిక();

      ప్రతిజ్ఞా( !udp_కనుగొను(label) );

      చిహ్నం_విలువ_t v;
      v.ptr = ఇది;
      sym_సమితి_విలువ(udp_పట్టిక, label, v);

#నిర్వచితమైతే CHECK_WITH_VALGRIND
      udp_defns_లెక్క += 1;
      udp_defns = static_cast<vvp_udp_s **>(realloc(udp_defns,
                  udp_defns_లెక్క*పరిమాణం(vvp_udp_s **)));
      udp_defns[udp_defns_లెక్క-1] = ఇది;
#అయితే_ముగింపు
}

vvp_udp_s::~vvp_udp_s()
{
      తొలగించు[] name_;
}

గుర్తులేని vvp_udp_s::పోర్టు_లెక్క() స్థిరరాశి
{
      తిరిగి ports_;
}

vvp_bit4_t vvp_udp_s::get_init() స్థిరరాశి
{
      తిరిగి init_;
}

vvp_udp_comb_s::vvp_udp_comb_s(స్థిరరాశి అక్షరం*label, అక్షరం*name__, గుర్తులేని ports)
: vvp_udp_s(label, name__, ports, BIT4_X, అబద్ధం)
{
      levels0_ = 0;
      levels1_ = 0;
      nlevels0_ = 0;
      nlevels1_ = 0;
}

vvp_udp_comb_s::~vvp_udp_comb_s()
{
      తొలగించు[] levels0_;
      తొలగించు[] levels1_;
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
vvp_bit4_t vvp_udp_comb_s::test_స్థాయిలు(స్థిరరాశి udp_స్థాయిలు_పట్టిక&cur)
{
	/* To test for a row match, test that the mask0, mask1 and
	   maskx vectors all have bits set where the matching
	   cur.mask0/1/x vectors have bits set. It is possible that a
	   levels0_[idx] vector has more bits set then the cur mask,
	   but this is OK and these bits are to be ignored. */

      కొరకు (గుర్తులేని idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    ఒకవేళ (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  కొనసాగు;
	    ఒకవేళ (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  కొనసాగు;
	    ఒకవేళ (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  కొనసాగు;

	    తిరిగి BIT4_0;
      }

      కొరకు (గుర్తులేని idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    ఒకవేళ (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  కొనసాగు;
	    ఒకవేళ (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  కొనసాగు;
	    ఒకవేళ (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  కొనసాగు;

	    తిరిగి BIT4_1;
      }

      తిరిగి BIT4_X;
}

vvp_bit4_t vvp_udp_comb_s::calculate_output(స్థిరరాశి udp_స్థాయిలు_పట్టిక&cur,
					    స్థిరరాశి udp_స్థాయిలు_పట్టిక&,
					    vvp_bit4_t)
{
      తిరిగి test_స్థాయిలు(cur);
}

స్థిర శూన్య or_based_on_char(udp_స్థాయిలు_పట్టిక&cur, అక్షరం flag,
			     గుర్తులేని దీర్ఘ ముసుగు_బిట్టు)
{
      ఎంపిక (flag) {
	  స్థితి '0':
	    cur.mask0 |= ముసుగు_బిట్టు;
	    ఆపు;
	  స్థితి '1':
	    cur.mask1 |= ముసుగు_బిట్టు;
	    ఆపు;
	  స్థితి 'x':
	    cur.maskx |= ముసుగు_బిట్టు;
	    ఆపు;
	  స్థితి 'b':
	    cur.mask0 |= ముసుగు_బిట్టు;
	    cur.mask1 |= ముసుగు_బిట్టు;
	    ఆపు;
	  స్థితి 'l':
	    cur.mask0 |= ముసుగు_బిట్టు;
	    cur.maskx |= ముసుగు_బిట్టు;
	    ఆపు;
	  స్థితి 'h':
	    cur.maskx |= ముసుగు_బిట్టు;
	    cur.mask1 |= ముసుగు_బిట్టు;
	    ఆపు;
	  స్థితి '?':
	    cur.mask0 |= ముసుగు_బిట్టు;
	    cur.maskx |= ముసుగు_బిట్టు;
	    cur.mask1 |= ముసుగు_బిట్టు;
	    ఆపు;
	  అప్రమేయ:
	    fprintf(stderr, "Unsupported flag %c(%d).\n", flag, flag);
	    ప్రతిజ్ఞా(0);
      }
}

శూన్య vvp_udp_comb_s::compile_పట్టిక(అక్షరం**tab)
{
      గుర్తులేని nrows0 = 0, nrows1 = 0;

	/* First run through the table to figure out the number of
	   rows I need for each kind of table. */
      కొరకు (గుర్తులేని idx = 0 ;  tab[idx] ;  idx += 1) {
	    ప్రతిజ్ఞా(అక్షరశ్రేణి_పొడవు(tab[idx]) == పోర్టు_లెక్క() + 1);
	    ఎంపిక (tab[idx][పోర్టు_లెక్క()]) {
		స్థితి '0':
		  nrows0 += 1;
		  ఆపు;
		స్థితి '1':
		  nrows1 += 1;
		  ఆపు;
		స్థితి 'x':
		  ఆపు;
		అప్రమేయ:
		  fprintf(stderr, "Unsupported entry %c(%d).\n",
		          tab[idx][పోర్టు_లెక్క()], tab[idx][పోర్టు_లెక్క()]);
		  ప్రతిజ్ఞా(0);
	    }
      }

      nlevels0_ = nrows0;
      levels0_ = కొత్త udp_స్థాయిలు_పట్టిక[nlevels0_];

      nlevels1_ = nrows1;
      levels1_ = కొత్త udp_స్థాయిలు_పట్టిక[nlevels1_];

      nrows0 = 0;
      nrows1 = 0;
      కొరకు (గుర్తులేని idx = 0 ;  tab[idx] ;  idx += 1) {
	    నిర్మాణం udp_స్థాయిలు_పట్టిక cur;
	    cur.mask0 = 0;
	    cur.mask1 = 0;
	    cur.maskx = 0;
	    ఒకవేళ (పోర్టు_లెక్క() > 8*పరిమాణం(cur.mask0)) {
		  fprintf(stderr, "internal error: primitive port count=%u "
			  " > %zu\n", పోర్టు_లెక్క(), పరిమాణం(cur.mask0));
		  ప్రతిజ్ఞా(పోర్టు_లెక్క() <= 8*పరిమాణం(cur.mask0));
	    }
	    కొరకు (గుర్తులేని pp = 0 ;  pp < పోర్టు_లెక్క() ;  pp += 1) {
		  గుర్తులేని దీర్ఘ ముసుగు_బిట్టు = 1UL << pp;
		  or_based_on_char(cur, tab[idx][pp], ముసుగు_బిట్టు);
	    }

	    ఎంపిక (tab[idx][పోర్టు_లెక్క()]) {
		స్థితి '0':
		  levels0_[nrows0++] = cur;
		  ఆపు;
		స్థితి '1':
		  levels1_[nrows1++] = cur;
		  ఆపు;
		అప్రమేయ:
		  ఆపు;
	    }
	    తొలగించు[] tab[idx];
      }
      విడుదల(tab);

      ప్రతిజ్ఞా(nrows0 == nlevels0_);
      ప్రతిజ్ఞా(nrows1 == nlevels1_);
}

vvp_udp_seq_s::vvp_udp_seq_s(స్థిరరాశి అక్షరం*label, అక్షరం*name__,
			     గుర్తులేని ports, vvp_bit4_t init)
: vvp_udp_s(label, name__, ports, init, నిజం)
{
      levels0_ = 0;
      levels1_ = 0;
      levelsx_ = 0;
      స్థాయిలు_L = 0;

      nlevels0_ = 0;
      nlevels1_ = 0;
      nlevelsx_ = 0;
      nlevelsL_ = 0;

      edges0_ = 0;
      edges1_ = 0;
      అంచులు_L = 0;

      nedges0_ = 0;
      nedges1_ = 0;
      nedgesL_ = 0;
}

vvp_udp_seq_s::~vvp_udp_seq_s()
{
      తొలగించు[] levels0_;
      తొలగించు[] levels1_;
      తొలగించు[] levelsx_;
      తొలగించు[] స్థాయిలు_L;
      తొలగించు[] edges0_;
      తొలగించు[] edges1_;
      తొలగించు[] అంచులు_L;
}

శూన్య అంచు_based_on_char(నిర్మాణం udp_అంచులు_పట్టిక&cur, అక్షరం chr, గుర్తులేని pos)
{
      గుర్తులేని దీర్ఘ ముసుగు_బిట్టు = 1UL << pos;

      ఎంపిక (chr) {
	  స్థితి '0':
	    cur.mask0 |= ముసుగు_బిట్టు;
	    ఆపు;
	  స్థితి '1':
	    cur.mask1 |= ముసుగు_బిట్టు;
	    ఆపు;
	  స్థితి 'x':
	    cur.maskx |= ముసుగు_బిట్టు;
	    ఆపు;
	  స్థితి 'b':
	    cur.mask0 |= ముసుగు_బిట్టు;
	    cur.mask1 |= ముసుగు_బిట్టు;
	    ఆపు;
	  స్థితి 'l':
	    cur.mask0 |= ముసుగు_బిట్టు;
	    cur.maskx |= ముసుగు_బిట్టు;
	    ఆపు;
	  స్థితి 'h':
	    cur.maskx |= ముసుగు_బిట్టు;
	    cur.mask1 |= ముసుగు_బిట్టు;
	    ఆపు;
	  స్థితి '?':
	    cur.mask0 |= ముసుగు_బిట్టు;
	    cur.maskx |= ముసుగు_బిట్టు;
	    cur.mask1 |= ముసుగు_బిట్టు;
	    ఆపు;

	  స్థితి 'B': // (x?) edge
	    cur.mask0 |= ముసుగు_బిట్టు;
	    cur.mask1 |= ముసుగు_బిట్టు;
	    cur.అంచు_position = pos;
	    cur.అంచు_mask0 = 0;
	    cur.అంచు_maskx = 1;
	    cur.అంచు_mask1 = 0;
	    ఆపు;
	  స్థితి 'f': // (10) edge
	    cur.mask0 |= ముసుగు_బిట్టు;
	    cur.అంచు_position = pos;
	    cur.అంచు_mask0 = 0;
	    cur.అంచు_maskx = 0;
	    cur.అంచు_mask1 = 1;
	    ఆపు;
	  స్థితి 'F': // (x0) edge
	    cur.mask0 |= ముసుగు_బిట్టు;
	    cur.అంచు_position = pos;
	    cur.అంచు_mask0 = 0;
	    cur.అంచు_maskx = 1;
	    cur.అంచు_mask1 = 0;
	    ఆపు;
	  స్థితి 'M': // (1x) edge
	    cur.maskx |= ముసుగు_బిట్టు;
	    cur.అంచు_position = pos;
	    cur.అంచు_mask0 = 0;
	    cur.అంచు_maskx = 0;
	    cur.అంచు_mask1 = 1;
	    ఆపు;
	  స్థితి 'N': // (1x) and (10) edge
	    cur.mask0 |= ముసుగు_బిట్టు;
	    cur.maskx |= ముసుగు_బిట్టు;
	    cur.అంచు_position = pos;
	    cur.అంచు_mask0 = 0;
	    cur.అంచు_maskx = 0;
	    cur.అంచు_mask1 = 1;
	    ఆపు;
	  స్థితి 'P': // (0x) and (01) edge
	    cur.maskx |= ముసుగు_బిట్టు;
	    cur.mask1 |= ముసుగు_బిట్టు;
	    cur.అంచు_position = pos;
	    cur.అంచు_mask0 = 1;
	    cur.అంచు_maskx = 0;
	    cur.అంచు_mask1 = 0;
	    ఆపు;
	  స్థితి 'q': // (bx) edge
	    cur.maskx |= ముసుగు_బిట్టు;
	    cur.అంచు_position = pos;
	    cur.అంచు_mask0 = 1;
	    cur.అంచు_maskx = 0;
	    cur.అంచు_mask1 = 1;
	    ఆపు;
	  స్థితి 'Q': // (0x) edge
	    cur.maskx |= ముసుగు_బిట్టు;
	    cur.అంచు_position = pos;
	    cur.అంచు_mask0 = 1;
	    cur.అంచు_maskx = 0;
	    cur.అంచు_mask1 = 0;
	    ఆపు;
	  స్థితి 'r': // (01) edge
	    cur.mask1 |= ముసుగు_బిట్టు;
	    cur.అంచు_position = pos;
	    cur.అంచు_mask0 = 1;
	    cur.అంచు_maskx = 0;
	    cur.అంచు_mask1 = 0;
	    ఆపు;
	  స్థితి 'R': // (x1) edge
	    cur.mask1 |= ముసుగు_బిట్టు;
	    cur.అంచు_position = pos;
	    cur.అంచు_mask0 = 0;
	    cur.అంచు_maskx = 1;
	    cur.అంచు_mask1 = 0;
	    ఆపు;
	  స్థితి '%': // (?x) edge
	    cur.maskx |= ముసుగు_బిట్టు;
	    cur.అంచు_position = pos;
	    cur.అంచు_mask0 = 1;
	    cur.అంచు_maskx = 0;
	    cur.అంచు_mask1 = 1;
	    ఆపు;
	  స్థితి '+': // (?1) edge
	    cur.mask1 |= ముసుగు_బిట్టు;
	    cur.అంచు_position = pos;
	    cur.అంచు_mask0 = 1;
	    cur.అంచు_maskx = 1;
	    cur.అంచు_mask1 = 0;
	    ఆపు;
	  స్థితి '_': // (?0) edge
	    cur.mask0 |= ముసుగు_బిట్టు;
	    cur.అంచు_position = pos;
	    cur.అంచు_mask0 = 0;
	    cur.అంచు_maskx = 1;
	    cur.అంచు_mask1 = 1;
	    ఆపు;
	  అప్రమేయ:
	    fprintf(stderr, "internal error: unknown edge code: %c\n", chr);
	    ప్రతిజ్ఞా(0);
      }
}

శూన్య vvp_udp_seq_s::compile_పట్టిక(అక్షరం**tab)
{

      కొరకు (గుర్తులేని idx = 0 ;  tab[idx] ;  idx += 1) {
	    స్థిరరాశి అక్షరం*అడ్డువరుస = tab[idx];
	    ప్రతిజ్ఞా(అక్షరశ్రేణి_పొడవు(అడ్డువరుస) == పోర్టు_లెక్క() + 2);

	    ఒకవేళ (strspn(అడ్డువరుస, "01xblh?") >= పోర్టు_లెక్క()+1) {

		  ఎంపిక (అడ్డువరుస[పోర్టు_లెక్క()+1]) {
		      స్థితి '0':
			nlevels0_ += 1;
			ఆపు;
		      స్థితి '1':
			nlevels1_ += 1;
			ఆపు;
		      స్థితి 'x':
			nlevelsx_ += 1;
			ఆపు;
		      స్థితి '-':
			nlevelsL_ += 1;
			ఆపు;
		      అప్రమేయ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        అడ్డువరుస[పోర్టు_లెక్క()+1], అడ్డువరుస[పోర్టు_లెక్క()+1]);
			ప్రతిజ్ఞా(0);
			ఆపు;
		  }

	    } లేకపోతే {
		    /* Rows that have n or p edges will need to be
		       expanded into 2 rows. */
		  గుర్తులేని extra = 0;
		  ఒకవేళ (strchr(అడ్డువరుస,'n'))
			extra = 1;
		  ఒకవేళ (strchr(అడ్డువరుస,'p'))
			extra = 1;
		  ఒకవేళ (strchr(అడ్డువరుస,'*'))
			extra = 2;

		  ఎంపిక (అడ్డువరుస[పోర్టు_లెక్క()+1]) {
		      స్థితి '0':
			nedges0_ += 1 + extra;
			ఆపు;
		      స్థితి '1':
			nedges1_ += 1 + extra;
			ఆపు;
		      స్థితి 'x':
			ఆపు;
		      స్థితి '-':
			nedgesL_ += 1 + extra;
			ఆపు;
		      అప్రమేయ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        అడ్డువరుస[పోర్టు_లెక్క()+1], అడ్డువరుస[పోర్టు_లెక్క()+1]);
			ప్రతిజ్ఞా(0);
			ఆపు;
		  }
	    }
      }

      levels0_ = కొత్త udp_స్థాయిలు_పట్టిక[nlevels0_];
      levels1_ = కొత్త udp_స్థాయిలు_పట్టిక[nlevels1_];
      levelsx_ = కొత్త udp_స్థాయిలు_పట్టిక[nlevelsx_];
      స్థాయిలు_L = కొత్త udp_స్థాయిలు_పట్టిక[nlevelsL_];
      edges0_ = కొత్త udp_అంచులు_పట్టిక[nedges0_];
      edges1_ = కొత్త udp_అంచులు_పట్టిక[nedges1_];
      అంచులు_L = కొత్త udp_అంచులు_పట్టిక[nedgesL_];

      గుర్తులేని idx_lev0 = 0;
      గుర్తులేని idx_lev1 = 0;
      గుర్తులేని idx_levx = 0;
      గుర్తులేని idx_levL = 0;
      గుర్తులేని idx_edg0 = 0;
      గుర్తులేని idx_edg1 = 0;
      గుర్తులేని idx_edgL = 0;

      కొరకు (గుర్తులేని idx = 0 ;  tab[idx] ;  idx += 1) {
	    స్థిరరాశి అక్షరం*అడ్డువరుస = tab[idx];

	    ఒకవేళ (strspn(అడ్డువరుస, "01xblh?") >= పోర్టు_లెక్క()+1) {
		  నిర్మాణం udp_స్థాయిలు_పట్టిక cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  కొరకు (గుర్తులేని pp = 0 ;  pp < పోర్టు_లెక్క() ;  pp += 1) {
			గుర్తులేని దీర్ఘ ముసుగు_బిట్టు = 1UL << pp;
			or_based_on_char(cur, అడ్డువరుస[pp+1], ముసుగు_బిట్టు);
		  }

		  or_based_on_char(cur, అడ్డువరుస[0], 1UL << పోర్టు_లెక్క());

		  ఎంపిక (అడ్డువరుస[పోర్టు_లెక్క()+1]) {
		      స్థితి '0':
			levels0_[idx_lev0++] = cur;
			ఆపు;
		      స్థితి '1':
			levels1_[idx_lev1++] = cur;
			ఆపు;
		      స్థితి 'x':
			levelsx_[idx_levx++] = cur;
			ఆపు;
		      స్థితి '-':
			స్థాయిలు_L[idx_levL++] = cur;
			ఆపు;
		      అప్రమేయ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        అడ్డువరుస[పోర్టు_లెక్క()+1], అడ్డువరుస[పోర్టు_లెక్క()+1]);
			ప్రతిజ్ఞా(0);
			ఆపు;
		  }

	    } లేకపోతే {
		  నిర్మాణం udp_అంచులు_పట్టిక cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  cur.అంచు_position = 0;
		  cur.అంచు_mask0 = 0;
		  cur.అంచు_mask1 = 0;
		  cur.అంచు_maskx = 0;

		  తార్కిక need_ext0_పట్టిక = అబద్ధం;
		  నిర్మాణం udp_అంచులు_పట్టిక ext0;
		  ext0.mask0 = 0;
		  ext0.mask1 = 0;
		  ext0.maskx = 0;
		  ext0.అంచు_position = 0;
		  ext0.అంచు_mask0 = 0;
		  ext0.అంచు_mask1 = 0;
		  ext0.అంచు_maskx = 0;

		  తార్కిక need_ext1_పట్టిక = అబద్ధం;
		  నిర్మాణం udp_అంచులు_పట్టిక ext1;
		  ext1.mask0 = 0;
		  ext1.mask1 = 0;
		  ext1.maskx = 0;
		  ext1.అంచు_position = 0;
		  ext1.అంచు_mask0 = 0;
		  ext1.అంచు_mask1 = 0;
		  ext1.అంచు_maskx = 0;

		  కొరకు (గుర్తులేని pp = 0 ;  pp < పోర్టు_లెక్క() ; pp += 1) {
			ఎంపిక (అడ్డువరుస[pp+1]) {
			    స్థితి 'n':
			      అంచు_based_on_char(cur, 'N', pp);
			      అంచు_based_on_char(ext0, '_', pp);
			      need_ext0_పట్టిక = నిజం;
			      ఆపు;
			    స్థితి 'p':
			      అంచు_based_on_char(cur, 'P', pp);
			      అంచు_based_on_char(ext0, '+', pp);
			      need_ext0_పట్టిక = నిజం;
			      ఆపు;
			    స్థితి '*':
			      అంచు_based_on_char(cur, 'P', pp);
			      అంచు_based_on_char(ext0, 'N', pp);
			      అంచు_based_on_char(ext1, 'B', pp);
			      need_ext0_పట్టిక = నిజం;
			      need_ext1_పట్టిక = నిజం;
			      ఆపు;
			    అప్రమేయ:
			      అంచు_based_on_char(cur, అడ్డువరుస[pp+1], pp);
			      అంచు_based_on_char(ext0, అడ్డువరుస[pp+1], pp);
			      అంచు_based_on_char(ext1, అడ్డువరుస[pp+1], pp);
			      ఆపు;
			}
		  }
		  అంచు_based_on_char(cur, అడ్డువరుస[0], పోర్టు_లెక్క());
		  అంచు_based_on_char(ext0, అడ్డువరుస[0], పోర్టు_లెక్క());
		  అంచు_based_on_char(ext1, అడ్డువరుస[0], పోర్టు_లెక్క());

		  ఎంపిక (అడ్డువరుస[పోర్టు_లెక్క()+1]) {
		      స్థితి '0':
			edges0_[idx_edg0++] = cur;
			ఒకవేళ (need_ext0_పట్టిక)
			      edges0_[idx_edg0++] = ext0;
			ఒకవేళ (need_ext1_పట్టిక)
			      edges0_[idx_edg0++] = ext1;
			ఆపు;
		      స్థితి '1':
			edges1_[idx_edg1++] = cur;
			ఒకవేళ (need_ext0_పట్టిక)
			      edges1_[idx_edg1++] = ext0;
			ఒకవేళ (need_ext1_పట్టిక)
			      edges1_[idx_edg1++] = ext1;
			ఆపు;
		      స్థితి 'x':
			ఆపు;
		      స్థితి '-':
			అంచులు_L[idx_edgL++] = cur;
			ఒకవేళ (need_ext0_పట్టిక)
			      అంచులు_L[idx_edgL++] = ext0;
			ఒకవేళ (need_ext1_పట్టిక)
			      అంచులు_L[idx_edgL++] = ext1;
			ఆపు;
		      అప్రమేయ:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        అడ్డువరుస[పోర్టు_లెక్క()+1], అడ్డువరుస[పోర్టు_లెక్క()+1]);
			ప్రతిజ్ఞా(0);
			ఆపు;
		  }

	    }
	    తొలగించు[] tab[idx];
      }
      విడుదల(tab);

      ప్రతిజ్ఞా(idx_edg0 == nedges0_);
      ప్రతిజ్ఞా(idx_edg1 == nedges1_);
      ప్రతిజ్ఞా(idx_edgL == nedgesL_);

}

తార్కిక క్రియాకారకం == (స్థిరరాశి udp_స్థాయిలు_పట్టిక&a, స్థిరరాశి udp_స్థాయిలు_పట్టిక&b)
{
      ఒకవేళ (a.mask0 != b.mask0)
	    తిరిగి అబద్ధం;
      ఒకవేళ (a.mask1 != b.mask1)
	    తిరిగి అబద్ధం;
      ఒకవేళ (a.maskx != b.maskx)
	    తిరిగి అబద్ధం;
      తిరిగి నిజం;
}

vvp_bit4_t vvp_udp_seq_s::calculate_output(స్థిరరాశి udp_స్థాయిలు_పట్టిక&cur,
					   స్థిరరాశి udp_స్థాయిలు_పట్టిక&prev,
					   vvp_bit4_t cur_బహిర్గతం)
{
      ఒకవేళ (cur == prev)
	    తిరిగి cur_బహిర్గతం;

      udp_స్థాయిలు_పట్టిక cur_tmp = cur;

      గుర్తులేని దీర్ఘ ముసుగు_బహిర్గతం = 1UL << పోర్టు_లెక్క();
      ఎంపిక (cur_బహిర్గతం) {
	  స్థితి BIT4_0:
	    cur_tmp.mask0 |= ముసుగు_బహిర్గతం;
	    ఆపు;
	  స్థితి BIT4_1:
	    cur_tmp.mask1 |= ముసుగు_బహిర్గతం;
	    ఆపు;
	  అప్రమేయ:
	    cur_tmp.maskx |= ముసుగు_బహిర్గతం;
	    ఆపు;
      }

      vvp_bit4_t lev = test_స్థాయిలు_2(cur_tmp);
      ఒకవేళ (lev == BIT4_Z) {
	    lev = test_అంచులు(cur_tmp, prev);
      }

      తిరిగి lev;
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
vvp_bit4_t vvp_udp_seq_s::test_స్థాయిలు_2(స్థిరరాశి udp_స్థాయిలు_పట్టిక&cur)
{
      కొరకు (గుర్తులేని idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    ఒకవేళ (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  కొనసాగు;
	    ఒకవేళ (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  కొనసాగు;
	    ఒకవేళ (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  కొనసాగు;

	    తిరిగి BIT4_0;
      }

      కొరకు (గుర్తులేని idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    ఒకవేళ (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  కొనసాగు;
	    ఒకవేళ (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  కొనసాగు;
	    ఒకవేళ (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  కొనసాగు;

	    తిరిగి BIT4_1;
      }

	/* We need to test against an explicit X-output table, since
	   we need to distinguish from an X output and no match. */
      కొరకు (గుర్తులేని idx = 0 ;  idx < nlevelsx_ ;  idx += 1) {
	    ఒకవేళ (cur.mask0 != (cur.mask0 & levelsx_[idx].mask0))
		  కొనసాగు;
	    ఒకవేళ (cur.mask1 != (cur.mask1 & levelsx_[idx].mask1))
		  కొనసాగు;
	    ఒకవేళ (cur.maskx != (cur.maskx & levelsx_[idx].maskx))
		  కొనసాగు;

	    తిరిగి BIT4_X;
      }

	/* Test the table that requests the next output be the same as
	   the current output. This gets the current output from the
	   levels table that was passed in. */
      కొరకు (గుర్తులేని idx = 0 ;  idx < nlevelsL_ ;  idx += 1) {
	    ఒకవేళ (cur.mask0 != (cur.mask0 & స్థాయిలు_L[idx].mask0))
		  కొనసాగు;
	    ఒకవేళ (cur.mask1 != (cur.mask1 & స్థాయిలు_L[idx].mask1))
		  కొనసాగు;
	    ఒకవేళ (cur.maskx != (cur.maskx & స్థాయిలు_L[idx].maskx))
		  కొనసాగు;

	    ఒకవేళ (cur.mask0 & (1 << పోర్టు_లెక్క()))
		  తిరిగి BIT4_0;
	    ఒకవేళ (cur.mask1 & (1 << పోర్టు_లెక్క()))
		  తిరిగి BIT4_1;
	    ఒకవేళ (cur.maskx & (1 << పోర్టు_లెక్క()))
		  తిరిగి BIT4_X;

	    ప్రతిజ్ఞా(0);
	    తిరిగి BIT4_X;
      }

	/* No explicit levels entry match. Return a Z to signal that
	   further testing is needed. */
      తిరిగి BIT4_Z;
}

vvp_bit4_t vvp_udp_seq_s::test_అంచులు(స్థిరరాశి udp_స్థాయిలు_పట్టిక&cur,
				      స్థిరరాశి udp_స్థాయిలు_పట్టిక&prev)
{
	/* The edge_mask is true for all bits that are different in
	   the cur and prev tables. */
      గుర్తులేని దీర్ఘ edge0_ముసుగు = cur.mask0 ^ prev.mask0;
      గుర్తులేని దీర్ఘ edgex_ముసుగు = cur.maskx ^ prev.maskx;
      గుర్తులేని దీర్ఘ edge1_ముసుగు = cur.mask1 ^ prev.mask1;

      గుర్తులేని దీర్ఘ అంచు_ముసుగు = edge0_ముసుగు|edgex_ముసుగు|edge1_ముసుగు;
      అంచు_ముసుగు &= ~ (-1UL << పోర్టు_లెక్క());

	/* If there are no differences, then there are no edges. Give
	   up now. */
      ఒకవేళ (అంచు_ముసుగు == 0)
	    తిరిగి BIT4_X;

      గుర్తులేని అంచు_position = 0;
      అయితే ((అంచు_ముసుగు&1) == 0) {
	    అంచు_ముసుగు >>= 1;
	    అంచు_position += 1;
      }

	/* We expect that there is exactly one edge in here. */
      ప్రతిజ్ఞా(అంచు_ముసుగు == 1);

      అంచు_ముసుగు = 1UL << అంచు_position;

      గుర్తులేని అంచు_mask0 = (prev.mask0&అంచు_ముసుగు)? 1 : 0;
      గుర్తులేని అంచు_maskx = (prev.maskx&అంచు_ముసుగు)? 1 : 0;
      గుర్తులేని అంచు_mask1 = (prev.mask1&అంచు_ముసుగు)? 1 : 0;


	/* Now the edge_position and edge_mask* variables have the
	   values we use to test the applicability of the edge_table
	   entries. */

      కొరకు (గుర్తులేని idx = 0 ;  idx < nedges0_ ;  idx += 1) {
	    స్థిరరాశి నిర్మాణం udp_అంచులు_పట్టిక*అడ్డువరుస = edges0_ + idx;

	    ఒకవేళ (అడ్డువరుస->అంచు_position != అంచు_position)
		  కొనసాగు;
	    ఒకవేళ (అంచు_mask0 && !అడ్డువరుస->అంచు_mask0)
		  కొనసాగు;
	    ఒకవేళ (అంచు_maskx && !అడ్డువరుస->అంచు_maskx)
		  కొనసాగు;
	    ఒకవేళ (అంచు_mask1 && !అడ్డువరుస->అంచు_mask1)
		  కొనసాగు;
	    ఒకవేళ (cur.mask0 != (cur.mask0 & అడ్డువరుస->mask0))
		  కొనసాగు;
	    ఒకవేళ (cur.maskx != (cur.maskx & అడ్డువరుస->maskx))
		  కొనసాగు;
	    ఒకవేళ (cur.mask1 != (cur.mask1 & అడ్డువరుస->mask1))
		  కొనసాగు;

	    తిరిగి BIT4_0;
      }

      కొరకు (గుర్తులేని idx = 0 ;  idx < nedges1_ ;  idx += 1) {
	    స్థిరరాశి నిర్మాణం udp_అంచులు_పట్టిక*అడ్డువరుస = edges1_ + idx;

	    ఒకవేళ (అడ్డువరుస->అంచు_position != అంచు_position)
		  కొనసాగు;
	    ఒకవేళ (అంచు_mask0 && !అడ్డువరుస->అంచు_mask0)
		  కొనసాగు;
	    ఒకవేళ (అంచు_maskx && !అడ్డువరుస->అంచు_maskx)
		  కొనసాగు;
	    ఒకవేళ (అంచు_mask1 && !అడ్డువరుస->అంచు_mask1)
		  కొనసాగు;
	    ఒకవేళ (cur.mask0 != (cur.mask0 & అడ్డువరుస->mask0))
		  కొనసాగు;
	    ఒకవేళ (cur.maskx != (cur.maskx & అడ్డువరుస->maskx))
		  కొనసాగు;
	    ఒకవేళ (cur.mask1 != (cur.mask1 & అడ్డువరుస->mask1))
		  కొనసాగు;

	    తిరిగి BIT4_1;
      }

      కొరకు (గుర్తులేని idx = 0 ;  idx < nedgesL_ ;  idx += 1) {
	    స్థిరరాశి నిర్మాణం udp_అంచులు_పట్టిక*అడ్డువరుస = అంచులు_L + idx;

	    ఒకవేళ (అడ్డువరుస->అంచు_position != అంచు_position)
		  కొనసాగు;
	    ఒకవేళ (అంచు_mask0 && !అడ్డువరుస->అంచు_mask0)
		  కొనసాగు;
	    ఒకవేళ (అంచు_maskx && !అడ్డువరుస->అంచు_maskx)
		  కొనసాగు;
	    ఒకవేళ (అంచు_mask1 && !అడ్డువరుస->అంచు_mask1)
		  కొనసాగు;
	    ఒకవేళ (cur.mask0 != (cur.mask0 & అడ్డువరుస->mask0))
		  కొనసాగు;
	    ఒకవేళ (cur.maskx != (cur.maskx & అడ్డువరుస->maskx))
		  కొనసాగు;
	    ఒకవేళ (cur.mask1 != (cur.mask1 & అడ్డువరుస->mask1))
		  కొనసాగు;

	    ఒకవేళ (cur.mask0 & (1 << పోర్టు_లెక్క()))
		  తిరిగి BIT4_0;
	    ఒకవేళ (cur.mask1 & (1 << పోర్టు_లెక్క()))
		  తిరిగి BIT4_1;
	    ఒకవేళ (cur.maskx & (1 << పోర్టు_లెక్క()))
		  తిరిగి BIT4_X;

	    ప్రతిజ్ఞా(0);
	    తిరిగి BIT4_X;
      }

      తిరిగి BIT4_X;
}

vvp_udp_fun_core::vvp_udp_fun_core(vvp_net_t*net, vvp_udp_s*def)
: vvp_wide_fun_core(net, def->పోర్టు_లెక్క())
{
      def_ = def;
      cur_బహిర్గతం_2 = def_->get_init();
	// Assume initially that all the inputs are 1'bx
      current_.mask0 = 0;
      current_.mask1 = 0;
      current_.maskx = ~ ((-1UL) << పోర్టు_లెక్క());

        // If the initial value is 0 or 1, schedule the initial assignment
        // normally, so that any sensitive always processes can be started
        // first.
      ఒకవేళ (cur_బహిర్గతం_2 != BIT4_X)
	    కాలపట్టిక_generic(ఇది, 0, అబద్ధం);
      లేకపోతే
	    కాలపట్టిక_functor(ఇది);
}

vvp_udp_fun_core::~vvp_udp_fun_core()
{
}

/*
 * This method is used to propagate the initial value on startup.
 */
శూన్య vvp_udp_fun_core::నిరంతరం_నిరంతరం()
{
      vvp_vector4_t tmp (1);
      tmp.సమితి_బిట్టు(0, cur_బహిర్గతం_2);
      propagate_vec4(tmp);
}

శూన్య vvp_udp_fun_core::recv_vec4_from_inputs(గుర్తులేని పోర్టు)
{
	/* For now, assume udps are 1-bit wide. */
      గుర్తులేని పోర్టు_వెడల్పు = value(పోర్టు).size();
      ప్రతిజ్ఞా(పోర్టు_వెడల్పు == 1);

      గుర్తులేని దీర్ఘ ముసుగు = 1UL << పోర్టు;

      udp_స్థాయిలు_పట్టిక prev = current_;

      ఎంపిక (value(పోర్టు).value(0)) {

	  స్థితి BIT4_0:
	    current_.mask0 |= ముసుగు;
	    current_.mask1 &= ~ముసుగు;
	    current_.maskx &= ~ముసుగు;
	    ఆపు;
	  స్థితి BIT4_1:
	    current_.mask0 &= ~ముసుగు;
	    current_.mask1 |= ముసుగు;
	    current_.maskx &= ~ముసుగు;
	    ఆపు;
	  అప్రమేయ:
	    current_.mask0 &= ~ముసుగు;
	    current_.mask1 &= ~ముసుగు;
	    current_.maskx |= ముసుగు;
	    ఆపు;
      }

      vvp_bit4_t బహిర్గతం_బిట్టు = def_->calculate_output(current_, prev, cur_బహిర్గతం_2);

      ఒకవేళ (బహిర్గతం_బిట్టు == cur_బహిర్గతం_2)
	    తిరిగి;

      cur_బహిర్గతం_2 = బహిర్గతం_బిట్టు;
      కాలపట్టిక_functor(ఇది);
}


/*
 * This function is called by the parser in response to a .udp
 * node. We create the nodes needed to integrate the UDP into the
 * netlist. The definition should be parsed already.
 */
శూన్య compile_udp_functor(అక్షరం*label, అక్షరం*type,
			 గుర్తులేని argc, నిర్మాణం symb_s*argv)
{
      నిర్మాణం vvp_udp_s *def = udp_కనుగొను(type);
      ప్రతిజ్ఞా(def);
      విడుదల(type);

      vvp_net_t*ptr = కొత్త vvp_net_t;
      vvp_udp_fun_core*core = కొత్త vvp_udp_fun_core(ptr, def);
      ptr->fun = core;

      define_functor_చిహ్నం(label, ptr);
      విడుదల(label);

      wide_inputs_connect(core, argc, argv);
      విడుదల(argv);
}
