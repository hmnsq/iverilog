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

#અન્તર્ભાવ "udp.h"
#અન્તર્ભાવ "schedule.h"
#અન્તર્ભાવ "symbols.h"
#અન્તર્ભાવ "compile.h"
#અન્તર્ભાવ "config.h"
#અન્તર્ભાવ <cassert>
#અન્તર્ભાવ <cstdlib>
#અન્તર્ભાવ <cstring>
#અન્તર્ભાવ <cstdio>
#અન્તર્ભાવ <iostream>
#ifdef CHECK_WITH_VALGRIND
#અન્તર્ભાવ "vvp_cleanup.h"
#અન્તર્ભાવ "ivl_alloc.h"
#endif

using નામાકાશ std;

// We may need these later when we build the VPI interface to
// UDP definitions.
#ifdef CHECK_WITH_VALGRIND
સ્થિર vvp_udp_s **udp_defns = 0;
સ્થિર unsigned udp_defns_ગણતરી = 0;

શૂન્ય udp_defns_delete()
{
      માટે (unsigned idx = 0; idx < udp_defns_ગણતરી; idx += 1) {
	    જો (udp_defns[idx]->is_sequential()) {
		  vvp_udp_seq_s *obj = static_cast<vvp_udp_seq_s *>
		                       (udp_defns[idx]);
		  delete obj;
	    } નહીંતર {
		  vvp_udp_comb_s *obj = static_cast<vvp_udp_comb_s *>
		                        (udp_defns[idx]);
		  delete obj;
	    }
      }
      મુક્ત(udp_defns);
      udp_defns = 0;
      udp_defns_ગણતરી = 0;
}
#endif

સ્થિર પ્રતીક_table_t udp_table;

શૂન્ય delete_udp_symbols()
{
      delete_પ્રતીક_table(udp_table);
      udp_table = 0;
}

સંરચના vvp_udp_s *udp_find(અચલ અક્ષર *label)
{
      પ્રતીક_કિંમત_t v = sym_get_કિંમત(udp_table, label);
      પાછા (સંરચના vvp_udp_s *)v.ptr;
}

ostream& operator <<(ostream&o, અચલ સંરચના udp_levels_table&table)
{
      o << "[" << hex << table.mask0
	<< "/" << hex << table.mask1
	<< "/" << hex << table.maskx << "]";
      પાછા o;
}

vvp_udp_s::vvp_udp_s(અચલ અક્ષર*label, અક્ષર*name__, unsigned ports,
                     vvp_bit4_t init, તાર્કિક type)
: name_(name__), ports_(ports), init_(init), seq_(type)
{
      જો (!udp_table)
	    udp_table = new_પ્રતીક_table();

      પ્રતિજ્ઞા( !udp_find(label) );

      પ્રતીક_કિંમત_t v;
      v.ptr = આ;
      sym_set_કિંમત(udp_table, label, v);

#ifdef CHECK_WITH_VALGRIND
      udp_defns_ગણતરી += 1;
      udp_defns = static_cast<vvp_udp_s **>(realloc(udp_defns,
                  udp_defns_ગણતરી*કદ(vvp_udp_s **)));
      udp_defns[udp_defns_ગણતરી-1] = આ;
#endif
}

vvp_udp_s::~vvp_udp_s()
{
      delete[] name_;
}

unsigned vvp_udp_s::port_ગણતરી() અચલ
{
      પાછા ports_;
}

vvp_bit4_t vvp_udp_s::get_init() અચલ
{
      પાછા init_;
}

vvp_udp_comb_s::vvp_udp_comb_s(અચલ અક્ષર*label, અક્ષર*name__, unsigned ports)
: vvp_udp_s(label, name__, ports, BIT4_X, ખોટું)
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
vvp_bit4_t vvp_udp_comb_s::test_levels(અચલ udp_levels_table&cur)
{
	/* To test for a row match, test that the mask0, mask1 and
	   maskx vectors all have bits set where the matching
	   cur.mask0/1/x vectors have bits set. It is possible that a
	   levels0_[idx] vector has more bits set then the cur mask,
	   but this is OK and these bits are to be ignored. */

      માટે (unsigned idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    જો (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  ચાલુ;
	    જો (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  ચાલુ;
	    જો (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  ચાલુ;

	    પાછા BIT4_0;
      }

      માટે (unsigned idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    જો (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  ચાલુ;
	    જો (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  ચાલુ;
	    જો (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  ચાલુ;

	    પાછા BIT4_1;
      }

      પાછા BIT4_X;
}

vvp_bit4_t vvp_udp_comb_s::calculate_output(અચલ udp_levels_table&cur,
					    અચલ udp_levels_table&,
					    vvp_bit4_t)
{
      પાછા test_levels(cur);
}

સ્થિર શૂન્ય or_based_on_char(udp_levels_table&cur, અક્ષર flag,
			     unsigned long mask_bit)
{
      પસંદગી (flag) {
	  સ્થિતિ '0':
	    cur.mask0 |= mask_bit;
	    તોડો;
	  સ્થિતિ '1':
	    cur.mask1 |= mask_bit;
	    તોડો;
	  સ્થિતિ 'x':
	    cur.maskx |= mask_bit;
	    તોડો;
	  સ્થિતિ 'b':
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    તોડો;
	  સ્થિતિ 'l':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    તોડો;
	  સ્થિતિ 'h':
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    તોડો;
	  સ્થિતિ '?':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    તોડો;
	  મૂળભૂત:
	    fprintf(stderr, "Unsupported flag %c(%d).\n", flag, flag);
	    પ્રતિજ્ઞા(0);
      }
}

શૂન્ય vvp_udp_comb_s::compile_table(અક્ષર**tab)
{
      unsigned nrows0 = 0, nrows1 = 0;

	/* First run through the table to figure out the number of
	   rows I need for each kind of table. */
      માટે (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    પ્રતિજ્ઞા(strlen(tab[idx]) == port_ગણતરી() + 1);
	    પસંદગી (tab[idx][port_ગણતરી()]) {
		સ્થિતિ '0':
		  nrows0 += 1;
		  તોડો;
		સ્થિતિ '1':
		  nrows1 += 1;
		  તોડો;
		સ્થિતિ 'x':
		  તોડો;
		મૂળભૂત:
		  fprintf(stderr, "Unsupported entry %c(%d).\n",
		          tab[idx][port_ગણતરી()], tab[idx][port_ગણતરી()]);
		  પ્રતિજ્ઞા(0);
	    }
      }

      nlevels0_ = nrows0;
      levels0_ = નવું udp_levels_table[nlevels0_];

      nlevels1_ = nrows1;
      levels1_ = નવું udp_levels_table[nlevels1_];

      nrows0 = 0;
      nrows1 = 0;
      માટે (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    સંરચના udp_levels_table cur;
	    cur.mask0 = 0;
	    cur.mask1 = 0;
	    cur.maskx = 0;
	    જો (port_ગણતરી() > 8*કદ(cur.mask0)) {
		  fprintf(stderr, "internal error: primitive port count=%u "
			  " > %zu\n", port_ગણતરી(), કદ(cur.mask0));
		  પ્રતિજ્ઞા(port_ગણતરી() <= 8*કદ(cur.mask0));
	    }
	    માટે (unsigned pp = 0 ;  pp < port_ગણતરી() ;  pp += 1) {
		  unsigned long mask_bit = 1UL << pp;
		  or_based_on_char(cur, tab[idx][pp], mask_bit);
	    }

	    પસંદગી (tab[idx][port_ગણતરી()]) {
		સ્થિતિ '0':
		  levels0_[nrows0++] = cur;
		  તોડો;
		સ્થિતિ '1':
		  levels1_[nrows1++] = cur;
		  તોડો;
		મૂળભૂત:
		  તોડો;
	    }
	    delete[] tab[idx];
      }
      મુક્ત(tab);

      પ્રતિજ્ઞા(nrows0 == nlevels0_);
      પ્રતિજ્ઞા(nrows1 == nlevels1_);
}

vvp_udp_seq_s::vvp_udp_seq_s(અચલ અક્ષર*label, અક્ષર*name__,
			     unsigned ports, vvp_bit4_t init)
: vvp_udp_s(label, name__, ports, init, સાચું)
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

શૂન્ય edge_based_on_char(સંરચના udp_edges_table&cur, અક્ષર chr, unsigned pos)
{
      unsigned long mask_bit = 1UL << pos;

      પસંદગી (chr) {
	  સ્થિતિ '0':
	    cur.mask0 |= mask_bit;
	    તોડો;
	  સ્થિતિ '1':
	    cur.mask1 |= mask_bit;
	    તોડો;
	  સ્થિતિ 'x':
	    cur.maskx |= mask_bit;
	    તોડો;
	  સ્થિતિ 'b':
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    તોડો;
	  સ્થિતિ 'l':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    તોડો;
	  સ્થિતિ 'h':
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    તોડો;
	  સ્થિતિ '?':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    તોડો;

	  સ્થિતિ 'B': // (x?) edge
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    તોડો;
	  સ્થિતિ 'f': // (10) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    તોડો;
	  સ્થિતિ 'F': // (x0) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    તોડો;
	  સ્થિતિ 'M': // (1x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    તોડો;
	  સ્થિતિ 'N': // (1x) and (10) edge
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    તોડો;
	  સ્થિતિ 'P': // (0x) and (01) edge
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    તોડો;
	  સ્થિતિ 'q': // (bx) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    તોડો;
	  સ્થિતિ 'Q': // (0x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    તોડો;
	  સ્થિતિ 'r': // (01) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    તોડો;
	  સ્થિતિ 'R': // (x1) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    તોડો;
	  સ્થિતિ '%': // (?x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    તોડો;
	  સ્થિતિ '+': // (?1) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    તોડો;
	  સ્થિતિ '_': // (?0) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 1;
	    તોડો;
	  મૂળભૂત:
	    fprintf(stderr, "internal error: unknown edge code: %c\n", chr);
	    પ્રતિજ્ઞા(0);
      }
}

શૂન્ય vvp_udp_seq_s::compile_table(અક્ષર**tab)
{

      માટે (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    અચલ અક્ષર*row = tab[idx];
	    પ્રતિજ્ઞા(strlen(row) == port_ગણતરી() + 2);

	    જો (strspn(row, "01xblh?") >= port_ગણતરી()+1) {

		  પસંદગી (row[port_ગણતરી()+1]) {
		      સ્થિતિ '0':
			nlevels0_ += 1;
			તોડો;
		      સ્થિતિ '1':
			nlevels1_ += 1;
			તોડો;
		      સ્થિતિ 'x':
			nlevelsx_ += 1;
			તોડો;
		      સ્થિતિ '-':
			nlevelsL_ += 1;
			તોડો;
		      મૂળભૂત:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_ગણતરી()+1], row[port_ગણતરી()+1]);
			પ્રતિજ્ઞા(0);
			તોડો;
		  }

	    } નહીંતર {
		    /* Rows that have n or p edges will need to be
		       expanded into 2 rows. */
		  unsigned extra = 0;
		  જો (strchr(row,'n'))
			extra = 1;
		  જો (strchr(row,'p'))
			extra = 1;
		  જો (strchr(row,'*'))
			extra = 2;

		  પસંદગી (row[port_ગણતરી()+1]) {
		      સ્થિતિ '0':
			nedges0_ += 1 + extra;
			તોડો;
		      સ્થિતિ '1':
			nedges1_ += 1 + extra;
			તોડો;
		      સ્થિતિ 'x':
			તોડો;
		      સ્થિતિ '-':
			nedgesL_ += 1 + extra;
			તોડો;
		      મૂળભૂત:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_ગણતરી()+1], row[port_ગણતરી()+1]);
			પ્રતિજ્ઞા(0);
			તોડો;
		  }
	    }
      }

      levels0_ = નવું udp_levels_table[nlevels0_];
      levels1_ = નવું udp_levels_table[nlevels1_];
      levelsx_ = નવું udp_levels_table[nlevelsx_];
      levelsL_ = નવું udp_levels_table[nlevelsL_];
      edges0_ = નવું udp_edges_table[nedges0_];
      edges1_ = નવું udp_edges_table[nedges1_];
      edgesL_ = નવું udp_edges_table[nedgesL_];

      unsigned idx_lev0 = 0;
      unsigned idx_lev1 = 0;
      unsigned idx_levx = 0;
      unsigned idx_levL = 0;
      unsigned idx_edg0 = 0;
      unsigned idx_edg1 = 0;
      unsigned idx_edgL = 0;

      માટે (unsigned idx = 0 ;  tab[idx] ;  idx += 1) {
	    અચલ અક્ષર*row = tab[idx];

	    જો (strspn(row, "01xblh?") >= port_ગણતરી()+1) {
		  સંરચના udp_levels_table cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  માટે (unsigned pp = 0 ;  pp < port_ગણતરી() ;  pp += 1) {
			unsigned long mask_bit = 1UL << pp;
			or_based_on_char(cur, row[pp+1], mask_bit);
		  }

		  or_based_on_char(cur, row[0], 1UL << port_ગણતરી());

		  પસંદગી (row[port_ગણતરી()+1]) {
		      સ્થિતિ '0':
			levels0_[idx_lev0++] = cur;
			તોડો;
		      સ્થિતિ '1':
			levels1_[idx_lev1++] = cur;
			તોડો;
		      સ્થિતિ 'x':
			levelsx_[idx_levx++] = cur;
			તોડો;
		      સ્થિતિ '-':
			levelsL_[idx_levL++] = cur;
			તોડો;
		      મૂળભૂત:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_ગણતરી()+1], row[port_ગણતરી()+1]);
			પ્રતિજ્ઞા(0);
			તોડો;
		  }

	    } નહીંતર {
		  સંરચના udp_edges_table cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  cur.edge_position = 0;
		  cur.edge_mask0 = 0;
		  cur.edge_mask1 = 0;
		  cur.edge_maskx = 0;

		  તાર્કિક need_ext0_table = ખોટું;
		  સંરચના udp_edges_table ext0;
		  ext0.mask0 = 0;
		  ext0.mask1 = 0;
		  ext0.maskx = 0;
		  ext0.edge_position = 0;
		  ext0.edge_mask0 = 0;
		  ext0.edge_mask1 = 0;
		  ext0.edge_maskx = 0;

		  તાર્કિક need_ext1_table = ખોટું;
		  સંરચના udp_edges_table ext1;
		  ext1.mask0 = 0;
		  ext1.mask1 = 0;
		  ext1.maskx = 0;
		  ext1.edge_position = 0;
		  ext1.edge_mask0 = 0;
		  ext1.edge_mask1 = 0;
		  ext1.edge_maskx = 0;

		  માટે (unsigned pp = 0 ;  pp < port_ગણતરી() ; pp += 1) {
			પસંદગી (row[pp+1]) {
			    સ્થિતિ 'n':
			      edge_based_on_char(cur, 'N', pp);
			      edge_based_on_char(ext0, '_', pp);
			      need_ext0_table = સાચું;
			      તોડો;
			    સ્થિતિ 'p':
			      edge_based_on_char(cur, 'P', pp);
			      edge_based_on_char(ext0, '+', pp);
			      need_ext0_table = સાચું;
			      તોડો;
			    સ્થિતિ '*':
			      edge_based_on_char(cur, 'P', pp);
			      edge_based_on_char(ext0, 'N', pp);
			      edge_based_on_char(ext1, 'B', pp);
			      need_ext0_table = સાચું;
			      need_ext1_table = સાચું;
			      તોડો;
			    મૂળભૂત:
			      edge_based_on_char(cur, row[pp+1], pp);
			      edge_based_on_char(ext0, row[pp+1], pp);
			      edge_based_on_char(ext1, row[pp+1], pp);
			      તોડો;
			}
		  }
		  edge_based_on_char(cur, row[0], port_ગણતરી());
		  edge_based_on_char(ext0, row[0], port_ગણતરી());
		  edge_based_on_char(ext1, row[0], port_ગણતરી());

		  પસંદગી (row[port_ગણતરી()+1]) {
		      સ્થિતિ '0':
			edges0_[idx_edg0++] = cur;
			જો (need_ext0_table)
			      edges0_[idx_edg0++] = ext0;
			જો (need_ext1_table)
			      edges0_[idx_edg0++] = ext1;
			તોડો;
		      સ્થિતિ '1':
			edges1_[idx_edg1++] = cur;
			જો (need_ext0_table)
			      edges1_[idx_edg1++] = ext0;
			જો (need_ext1_table)
			      edges1_[idx_edg1++] = ext1;
			તોડો;
		      સ્થિતિ 'x':
			તોડો;
		      સ્થિતિ '-':
			edgesL_[idx_edgL++] = cur;
			જો (need_ext0_table)
			      edgesL_[idx_edgL++] = ext0;
			જો (need_ext1_table)
			      edgesL_[idx_edgL++] = ext1;
			તોડો;
		      મૂળભૂત:
			fprintf(stderr, "Unsupported entry %c(%d).\n",
			        row[port_ગણતરી()+1], row[port_ગણતરી()+1]);
			પ્રતિજ્ઞા(0);
			તોડો;
		  }

	    }
	    delete[] tab[idx];
      }
      મુક્ત(tab);

      પ્રતિજ્ઞા(idx_edg0 == nedges0_);
      પ્રતિજ્ઞા(idx_edg1 == nedges1_);
      પ્રતિજ્ઞા(idx_edgL == nedgesL_);

}

તાર્કિક operator == (અચલ udp_levels_table&a, અચલ udp_levels_table&b)
{
      જો (a.mask0 != b.mask0)
	    પાછા ખોટું;
      જો (a.mask1 != b.mask1)
	    પાછા ખોટું;
      જો (a.maskx != b.maskx)
	    પાછા ખોટું;
      પાછા સાચું;
}

vvp_bit4_t vvp_udp_seq_s::calculate_output(અચલ udp_levels_table&cur,
					   અચલ udp_levels_table&prev,
					   vvp_bit4_t cur_out)
{
      જો (cur == prev)
	    પાછા cur_out;

      udp_levels_table cur_tmp = cur;

      unsigned long mask_out = 1UL << port_ગણતરી();
      પસંદગી (cur_out) {
	  સ્થિતિ BIT4_0:
	    cur_tmp.mask0 |= mask_out;
	    તોડો;
	  સ્થિતિ BIT4_1:
	    cur_tmp.mask1 |= mask_out;
	    તોડો;
	  મૂળભૂત:
	    cur_tmp.maskx |= mask_out;
	    તોડો;
      }

      vvp_bit4_t lev = test_levels_(cur_tmp);
      જો (lev == BIT4_Z) {
	    lev = test_edges_(cur_tmp, prev);
      }

      પાછા lev;
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
vvp_bit4_t vvp_udp_seq_s::test_levels_(અચલ udp_levels_table&cur)
{
      માટે (unsigned idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    જો (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  ચાલુ;
	    જો (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  ચાલુ;
	    જો (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  ચાલુ;

	    પાછા BIT4_0;
      }

      માટે (unsigned idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    જો (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  ચાલુ;
	    જો (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  ચાલુ;
	    જો (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  ચાલુ;

	    પાછા BIT4_1;
      }

	/* We need to test against an explicit X-output table, since
	   we need to distinguish from an X output and no match. */
      માટે (unsigned idx = 0 ;  idx < nlevelsx_ ;  idx += 1) {
	    જો (cur.mask0 != (cur.mask0 & levelsx_[idx].mask0))
		  ચાલુ;
	    જો (cur.mask1 != (cur.mask1 & levelsx_[idx].mask1))
		  ચાલુ;
	    જો (cur.maskx != (cur.maskx & levelsx_[idx].maskx))
		  ચાલુ;

	    પાછા BIT4_X;
      }

	/* Test the table that requests the next output be the same as
	   the current output. This gets the current output from the
	   levels table that was passed in. */
      માટે (unsigned idx = 0 ;  idx < nlevelsL_ ;  idx += 1) {
	    જો (cur.mask0 != (cur.mask0 & levelsL_[idx].mask0))
		  ચાલુ;
	    જો (cur.mask1 != (cur.mask1 & levelsL_[idx].mask1))
		  ચાલુ;
	    જો (cur.maskx != (cur.maskx & levelsL_[idx].maskx))
		  ચાલુ;

	    જો (cur.mask0 & (1 << port_ગણતરી()))
		  પાછા BIT4_0;
	    જો (cur.mask1 & (1 << port_ગણતરી()))
		  પાછા BIT4_1;
	    જો (cur.maskx & (1 << port_ગણતરી()))
		  પાછા BIT4_X;

	    પ્રતિજ્ઞા(0);
	    પાછા BIT4_X;
      }

	/* No explicit levels entry match. Return a Z to signal that
	   further testing is needed. */
      પાછા BIT4_Z;
}

vvp_bit4_t vvp_udp_seq_s::test_edges_(અચલ udp_levels_table&cur,
				      અચલ udp_levels_table&prev)
{
	/* The edge_mask is true for all bits that are different in
	   the cur and prev tables. */
      unsigned long edge0_mask = cur.mask0 ^ prev.mask0;
      unsigned long edgex_mask = cur.maskx ^ prev.maskx;
      unsigned long edge1_mask = cur.mask1 ^ prev.mask1;

      unsigned long edge_mask = edge0_mask|edgex_mask|edge1_mask;
      edge_mask &= ~ (-1UL << port_ગણતરી());

	/* If there are no differences, then there are no edges. Give
	   up now. */
      જો (edge_mask == 0)
	    પાછા BIT4_X;

      unsigned edge_position = 0;
      જ્યારે ((edge_mask&1) == 0) {
	    edge_mask >>= 1;
	    edge_position += 1;
      }

	/* We expect that there is exactly one edge in here. */
      પ્રતિજ્ઞા(edge_mask == 1);

      edge_mask = 1UL << edge_position;

      unsigned edge_mask0 = (prev.mask0&edge_mask)? 1 : 0;
      unsigned edge_maskx = (prev.maskx&edge_mask)? 1 : 0;
      unsigned edge_mask1 = (prev.mask1&edge_mask)? 1 : 0;


	/* Now the edge_position and edge_mask* variables have the
	   values we use to test the applicability of the edge_table
	   entries. */

      માટે (unsigned idx = 0 ;  idx < nedges0_ ;  idx += 1) {
	    અચલ સંરચના udp_edges_table*row = edges0_ + idx;

	    જો (row->edge_position != edge_position)
		  ચાલુ;
	    જો (edge_mask0 && !row->edge_mask0)
		  ચાલુ;
	    જો (edge_maskx && !row->edge_maskx)
		  ચાલુ;
	    જો (edge_mask1 && !row->edge_mask1)
		  ચાલુ;
	    જો (cur.mask0 != (cur.mask0 & row->mask0))
		  ચાલુ;
	    જો (cur.maskx != (cur.maskx & row->maskx))
		  ચાલુ;
	    જો (cur.mask1 != (cur.mask1 & row->mask1))
		  ચાલુ;

	    પાછા BIT4_0;
      }

      માટે (unsigned idx = 0 ;  idx < nedges1_ ;  idx += 1) {
	    અચલ સંરચના udp_edges_table*row = edges1_ + idx;

	    જો (row->edge_position != edge_position)
		  ચાલુ;
	    જો (edge_mask0 && !row->edge_mask0)
		  ચાલુ;
	    જો (edge_maskx && !row->edge_maskx)
		  ચાલુ;
	    જો (edge_mask1 && !row->edge_mask1)
		  ચાલુ;
	    જો (cur.mask0 != (cur.mask0 & row->mask0))
		  ચાલુ;
	    જો (cur.maskx != (cur.maskx & row->maskx))
		  ચાલુ;
	    જો (cur.mask1 != (cur.mask1 & row->mask1))
		  ચાલુ;

	    પાછા BIT4_1;
      }

      માટે (unsigned idx = 0 ;  idx < nedgesL_ ;  idx += 1) {
	    અચલ સંરચના udp_edges_table*row = edgesL_ + idx;

	    જો (row->edge_position != edge_position)
		  ચાલુ;
	    જો (edge_mask0 && !row->edge_mask0)
		  ચાલુ;
	    જો (edge_maskx && !row->edge_maskx)
		  ચાલુ;
	    જો (edge_mask1 && !row->edge_mask1)
		  ચાલુ;
	    જો (cur.mask0 != (cur.mask0 & row->mask0))
		  ચાલુ;
	    જો (cur.maskx != (cur.maskx & row->maskx))
		  ચાલુ;
	    જો (cur.mask1 != (cur.mask1 & row->mask1))
		  ચાલુ;

	    જો (cur.mask0 & (1 << port_ગણતરી()))
		  પાછા BIT4_0;
	    જો (cur.mask1 & (1 << port_ગણતરી()))
		  પાછા BIT4_1;
	    જો (cur.maskx & (1 << port_ગણતરી()))
		  પાછા BIT4_X;

	    પ્રતિજ્ઞા(0);
	    પાછા BIT4_X;
      }

      પાછા BIT4_X;
}

vvp_udp_fun_core::vvp_udp_fun_core(vvp_net_t*net, vvp_udp_s*def)
: vvp_wide_fun_core(net, def->port_ગણતરી())
{
      def_ = def;
      cur_out_ = def_->get_init();
	// Assume initially that all the inputs are 1'bx
      current_.mask0 = 0;
      current_.mask1 = 0;
      current_.maskx = ~ ((-1UL) << port_ગણતરી());

        // If the initial value is 0 or 1, schedule the initial assignment
        // normally, so that any sensitive always processes can be started
        // first.
      જો (cur_out_ != BIT4_X)
	    schedule_generic(આ, 0, ખોટું);
      નહીંતર
	    schedule_functor(આ);
}

vvp_udp_fun_core::~vvp_udp_fun_core()
{
}

/*
 * This method is used to propagate the initial value on startup.
 */
શૂન્ય vvp_udp_fun_core::run_run()
{
      vvp_vector4_t tmp (1);
      tmp.set_bit(0, cur_out_);
      propagate_vec4(tmp);
}

શૂન્ય vvp_udp_fun_core::recv_vec4_from_inputs(unsigned port)
{
	/* For now, assume udps are 1-bit wide. */
      unsigned port_width = value(port).size();
      પ્રતિજ્ઞા(port_width == 1);

      unsigned long mask = 1UL << port;

      udp_levels_table prev = current_;

      પસંદગી (value(port).value(0)) {

	  સ્થિતિ BIT4_0:
	    current_.mask0 |= mask;
	    current_.mask1 &= ~mask;
	    current_.maskx &= ~mask;
	    તોડો;
	  સ્થિતિ BIT4_1:
	    current_.mask0 &= ~mask;
	    current_.mask1 |= mask;
	    current_.maskx &= ~mask;
	    તોડો;
	  મૂળભૂત:
	    current_.mask0 &= ~mask;
	    current_.mask1 &= ~mask;
	    current_.maskx |= mask;
	    તોડો;
      }

      vvp_bit4_t out_bit = def_->calculate_output(current_, prev, cur_out_);

      જો (out_bit == cur_out_)
	    પાછા;

      cur_out_ = out_bit;
      schedule_functor(આ);
}


/*
 * This function is called by the parser in response to a .udp
 * node. We create the nodes needed to integrate the UDP into the
 * netlist. The definition should be parsed already.
 */
શૂન્ય compile_udp_functor(અક્ષર*label, અક્ષર*type,
			 unsigned argc, સંરચના symb_s*argv)
{
      સંરચના vvp_udp_s *def = udp_find(type);
      પ્રતિજ્ઞા(def);
      મુક્ત(type);

      vvp_net_t*ptr = નવું vvp_net_t;
      vvp_udp_fun_core*core = નવું vvp_udp_fun_core(ptr, def);
      ptr->fun = core;

      define_functor_પ્રતીક(label, ptr);
      મુક્ત(label);

      wide_inputs_connect(core, argc, argv);
      મુક્ત(argv);
}
