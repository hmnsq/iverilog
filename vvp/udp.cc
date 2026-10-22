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

#समावेश "udp.h"
#समावेश "schedule.h"
#समावेश "symbols.h"
#समावेश "compile.h"
#समावेश "config.h"
#समावेश <cassert>
#समावेश <cstdlib>
#समावेश <cstring>
#समावेश <cstdio>
#समावेश <iostream>
#अगर_घोषित CHECK_WITH_VALGRIND
#समावेश "vvp_cleanup.h"
#समावेश "ivl_alloc.h"
#पूर्ण_अगर

using namespace std;

// We may need these later when we build the VPI interface to
// UDP definitions.
#अगर_घोषित CHECK_WITH_VALGRIND
अटल vvp_udp_s **udp_defns = 0;
अटल अचिन्हित udp_defns_गिनती = 0;

व्योम udp_defns_delete()
{
      क्रम (अचिन्हित idx = 0; idx < udp_defns_गिनती; idx += 1) {
	    अगर (udp_defns[idx]->is_sequential()) {
		  vvp_udp_seq_s *obj = static_cast<vvp_udp_seq_s *>
		                       (udp_defns[idx]);
		  मिटाओ obj;
	    } अन्यथा {
		  vvp_udp_comb_s *obj = static_cast<vvp_udp_comb_s *>
		                        (udp_defns[idx]);
		  मिटाओ obj;
	    }
      }
      मुक्त(udp_defns);
      udp_defns = 0;
      udp_defns_गिनती = 0;
}
#पूर्ण_अगर

अटल प्रतीक_table_t udp_table;

व्योम delete_udp_symbols()
{
      delete_प्रतीक_table(udp_table);
      udp_table = 0;
}

काष्ठा vvp_udp_s *udp_find(स्थिर अक्षर *label)
{
      प्रतीक_मूल्य_t v = sym_get_मूल्य(udp_table, label);
      वापस (काष्ठा vvp_udp_s *)v.ptr;
}

ब_प्रवाह& चालक <<(ब_प्रवाह&o, स्थिर काष्ठा udp_levels_table&table)
{
      o << "[" << षष्ठ << table.mask0
	<< "/" << षष्ठ << table.mask1
	<< "/" << षष्ठ << table.maskx << "]";
      वापस o;
}

vvp_udp_s::vvp_udp_s(स्थिर अक्षर*label, अक्षर*name__, अचिन्हित ports,
                     vvp_bit4_t init, bool type)
: name_(name__), ports_(ports), init_(init), seq_(type)
{
      अगर (!udp_table)
	    udp_table = new_प्रतीक_table();

      निश्चित( !udp_find(label) );

      प्रतीक_मूल्य_t v;
      v.ptr = यह;
      sym_set_मूल्य(udp_table, label, v);

#अगर_घोषित CHECK_WITH_VALGRIND
      udp_defns_गिनती += 1;
      udp_defns = static_cast<vvp_udp_s **>(पुनः_स्मृति(udp_defns,
                  udp_defns_गिनती*माप(vvp_udp_s **)));
      udp_defns[udp_defns_गिनती-1] = यह;
#पूर्ण_अगर
}

vvp_udp_s::~vvp_udp_s()
{
      मिटाओ[] name_;
}

अचिन्हित vvp_udp_s::port_गिनती() स्थिर
{
      वापस ports_;
}

vvp_bit4_t vvp_udp_s::get_init() स्थिर
{
      वापस init_;
}

vvp_udp_comb_s::vvp_udp_comb_s(स्थिर अक्षर*label, अक्षर*name__, अचिन्हित ports)
: vvp_udp_s(label, name__, ports, BIT4_X, false)
{
      levels0_ = 0;
      levels1_ = 0;
      nlevels0_ = 0;
      nlevels1_ = 0;
}

vvp_udp_comb_s::~vvp_udp_comb_s()
{
      मिटाओ[] levels0_;
      मिटाओ[] levels1_;
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
vvp_bit4_t vvp_udp_comb_s::test_levels(स्थिर udp_levels_table&cur)
{
	/* To test for a row match, test that the mask0, mask1 and
	   maskx vectors all have bits set where the matching
	   cur.mask0/1/x vectors have bits set. It is possible that a
	   levels0_[idx] vector has more bits set then the cur mask,
	   but this is OK and these bits are to be ignored. */

      क्रम (अचिन्हित idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    अगर (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  जारी;
	    अगर (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  जारी;
	    अगर (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  जारी;

	    वापस BIT4_0;
      }

      क्रम (अचिन्हित idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    अगर (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  जारी;
	    अगर (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  जारी;
	    अगर (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  जारी;

	    वापस BIT4_1;
      }

      वापस BIT4_X;
}

vvp_bit4_t vvp_udp_comb_s::calculate_output(स्थिर udp_levels_table&cur,
					    स्थिर udp_levels_table&,
					    vvp_bit4_t)
{
      वापस test_levels(cur);
}

अटल व्योम or_based_on_char(udp_levels_table&cur, अक्षर flag,
			     अचिन्हित दीर्घ mask_bit)
{
      चयन (flag) {
	  हाल '0':
	    cur.mask0 |= mask_bit;
	    अवरोध;
	  हाल '1':
	    cur.mask1 |= mask_bit;
	    अवरोध;
	  हाल 'x':
	    cur.maskx |= mask_bit;
	    अवरोध;
	  हाल 'b':
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    अवरोध;
	  हाल 'l':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    अवरोध;
	  हाल 'h':
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    अवरोध;
	  हाल '?':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    अवरोध;
	  शेष:
	    ख_लिखो(मानक_त्रुटि, "Unsupported flag %c(%d).\n", flag, flag);
	    निश्चित(0);
      }
}

व्योम vvp_udp_comb_s::compile_table(अक्षर**tab)
{
      अचिन्हित nrows0 = 0, nrows1 = 0;

	/* First run through the table to figure out the number of
	   rows I need for each kind of table. */
      क्रम (अचिन्हित idx = 0 ;  tab[idx] ;  idx += 1) {
	    निश्चित(म_माप(tab[idx]) == port_गिनती() + 1);
	    चयन (tab[idx][port_गिनती()]) {
		हाल '0':
		  nrows0 += 1;
		  अवरोध;
		हाल '1':
		  nrows1 += 1;
		  अवरोध;
		हाल 'x':
		  अवरोध;
		शेष:
		  ख_लिखो(मानक_त्रुटि, "Unsupported entry %c(%d).\n",
		          tab[idx][port_गिनती()], tab[idx][port_गिनती()]);
		  निश्चित(0);
	    }
      }

      nlevels0_ = nrows0;
      levels0_ = नया udp_levels_table[nlevels0_];

      nlevels1_ = nrows1;
      levels1_ = नया udp_levels_table[nlevels1_];

      nrows0 = 0;
      nrows1 = 0;
      क्रम (अचिन्हित idx = 0 ;  tab[idx] ;  idx += 1) {
	    काष्ठा udp_levels_table cur;
	    cur.mask0 = 0;
	    cur.mask1 = 0;
	    cur.maskx = 0;
	    अगर (port_गिनती() > 8*माप(cur.mask0)) {
		  ख_लिखो(मानक_त्रुटि, "internal error: primitive port count=%u "
			  " > %zu\n", port_गिनती(), माप(cur.mask0));
		  निश्चित(port_गिनती() <= 8*माप(cur.mask0));
	    }
	    क्रम (अचिन्हित pp = 0 ;  pp < port_गिनती() ;  pp += 1) {
		  अचिन्हित दीर्घ mask_bit = 1UL << pp;
		  or_based_on_char(cur, tab[idx][pp], mask_bit);
	    }

	    चयन (tab[idx][port_गिनती()]) {
		हाल '0':
		  levels0_[nrows0++] = cur;
		  अवरोध;
		हाल '1':
		  levels1_[nrows1++] = cur;
		  अवरोध;
		शेष:
		  अवरोध;
	    }
	    मिटाओ[] tab[idx];
      }
      मुक्त(tab);

      निश्चित(nrows0 == nlevels0_);
      निश्चित(nrows1 == nlevels1_);
}

vvp_udp_seq_s::vvp_udp_seq_s(स्थिर अक्षर*label, अक्षर*name__,
			     अचिन्हित ports, vvp_bit4_t init)
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
      मिटाओ[] levels0_;
      मिटाओ[] levels1_;
      मिटाओ[] levelsx_;
      मिटाओ[] levelsL_;
      मिटाओ[] edges0_;
      मिटाओ[] edges1_;
      मिटाओ[] edgesL_;
}

व्योम edge_based_on_char(काष्ठा udp_edges_table&cur, अक्षर chr, अचिन्हित pos)
{
      अचिन्हित दीर्घ mask_bit = 1UL << pos;

      चयन (chr) {
	  हाल '0':
	    cur.mask0 |= mask_bit;
	    अवरोध;
	  हाल '1':
	    cur.mask1 |= mask_bit;
	    अवरोध;
	  हाल 'x':
	    cur.maskx |= mask_bit;
	    अवरोध;
	  हाल 'b':
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    अवरोध;
	  हाल 'l':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    अवरोध;
	  हाल 'h':
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    अवरोध;
	  हाल '?':
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    अवरोध;

	  हाल 'B': // (x?) edge
	    cur.mask0 |= mask_bit;
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    अवरोध;
	  हाल 'f': // (10) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    अवरोध;
	  हाल 'F': // (x0) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    अवरोध;
	  हाल 'M': // (1x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    अवरोध;
	  हाल 'N': // (1x) and (10) edge
	    cur.mask0 |= mask_bit;
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    अवरोध;
	  हाल 'P': // (0x) and (01) edge
	    cur.maskx |= mask_bit;
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    अवरोध;
	  हाल 'q': // (bx) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    अवरोध;
	  हाल 'Q': // (0x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    अवरोध;
	  हाल 'r': // (01) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 0;
	    अवरोध;
	  हाल 'R': // (x1) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    अवरोध;
	  हाल '%': // (?x) edge
	    cur.maskx |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 0;
	    cur.edge_mask1 = 1;
	    अवरोध;
	  हाल '+': // (?1) edge
	    cur.mask1 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 1;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 0;
	    अवरोध;
	  हाल '_': // (?0) edge
	    cur.mask0 |= mask_bit;
	    cur.edge_position = pos;
	    cur.edge_mask0 = 0;
	    cur.edge_maskx = 1;
	    cur.edge_mask1 = 1;
	    अवरोध;
	  शेष:
	    ख_लिखो(मानक_त्रुटि, "internal error: unknown edge code: %c\n", chr);
	    निश्चित(0);
      }
}

व्योम vvp_udp_seq_s::compile_table(अक्षर**tab)
{

      क्रम (अचिन्हित idx = 0 ;  tab[idx] ;  idx += 1) {
	    स्थिर अक्षर*row = tab[idx];
	    निश्चित(म_माप(row) == port_गिनती() + 2);

	    अगर (म_अखोज(row, "01xblh?") >= port_गिनती()+1) {

		  चयन (row[port_गिनती()+1]) {
		      हाल '0':
			nlevels0_ += 1;
			अवरोध;
		      हाल '1':
			nlevels1_ += 1;
			अवरोध;
		      हाल 'x':
			nlevelsx_ += 1;
			अवरोध;
		      हाल '-':
			nlevelsL_ += 1;
			अवरोध;
		      शेष:
			ख_लिखो(मानक_त्रुटि, "Unsupported entry %c(%d).\n",
			        row[port_गिनती()+1], row[port_गिनती()+1]);
			निश्चित(0);
			अवरोध;
		  }

	    } अन्यथा {
		    /* Rows that have n or p edges will need to be
		       expanded into 2 rows. */
		  अचिन्हित extra = 0;
		  अगर (म_अक्षर(row,'n'))
			extra = 1;
		  अगर (म_अक्षर(row,'p'))
			extra = 1;
		  अगर (म_अक्षर(row,'*'))
			extra = 2;

		  चयन (row[port_गिनती()+1]) {
		      हाल '0':
			nedges0_ += 1 + extra;
			अवरोध;
		      हाल '1':
			nedges1_ += 1 + extra;
			अवरोध;
		      हाल 'x':
			अवरोध;
		      हाल '-':
			nedgesL_ += 1 + extra;
			अवरोध;
		      शेष:
			ख_लिखो(मानक_त्रुटि, "Unsupported entry %c(%d).\n",
			        row[port_गिनती()+1], row[port_गिनती()+1]);
			निश्चित(0);
			अवरोध;
		  }
	    }
      }

      levels0_ = नया udp_levels_table[nlevels0_];
      levels1_ = नया udp_levels_table[nlevels1_];
      levelsx_ = नया udp_levels_table[nlevelsx_];
      levelsL_ = नया udp_levels_table[nlevelsL_];
      edges0_ = नया udp_edges_table[nedges0_];
      edges1_ = नया udp_edges_table[nedges1_];
      edgesL_ = नया udp_edges_table[nedgesL_];

      अचिन्हित idx_lev0 = 0;
      अचिन्हित idx_lev1 = 0;
      अचिन्हित idx_levx = 0;
      अचिन्हित idx_levL = 0;
      अचिन्हित idx_edg0 = 0;
      अचिन्हित idx_edg1 = 0;
      अचिन्हित idx_edgL = 0;

      क्रम (अचिन्हित idx = 0 ;  tab[idx] ;  idx += 1) {
	    स्थिर अक्षर*row = tab[idx];

	    अगर (म_अखोज(row, "01xblh?") >= port_गिनती()+1) {
		  काष्ठा udp_levels_table cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  क्रम (अचिन्हित pp = 0 ;  pp < port_गिनती() ;  pp += 1) {
			अचिन्हित दीर्घ mask_bit = 1UL << pp;
			or_based_on_char(cur, row[pp+1], mask_bit);
		  }

		  or_based_on_char(cur, row[0], 1UL << port_गिनती());

		  चयन (row[port_गिनती()+1]) {
		      हाल '0':
			levels0_[idx_lev0++] = cur;
			अवरोध;
		      हाल '1':
			levels1_[idx_lev1++] = cur;
			अवरोध;
		      हाल 'x':
			levelsx_[idx_levx++] = cur;
			अवरोध;
		      हाल '-':
			levelsL_[idx_levL++] = cur;
			अवरोध;
		      शेष:
			ख_लिखो(मानक_त्रुटि, "Unsupported entry %c(%d).\n",
			        row[port_गिनती()+1], row[port_गिनती()+1]);
			निश्चित(0);
			अवरोध;
		  }

	    } अन्यथा {
		  काष्ठा udp_edges_table cur;
		  cur.mask0 = 0;
		  cur.mask1 = 0;
		  cur.maskx = 0;
		  cur.edge_position = 0;
		  cur.edge_mask0 = 0;
		  cur.edge_mask1 = 0;
		  cur.edge_maskx = 0;

		  bool need_ext0_table = false;
		  काष्ठा udp_edges_table ext0;
		  ext0.mask0 = 0;
		  ext0.mask1 = 0;
		  ext0.maskx = 0;
		  ext0.edge_position = 0;
		  ext0.edge_mask0 = 0;
		  ext0.edge_mask1 = 0;
		  ext0.edge_maskx = 0;

		  bool need_ext1_table = false;
		  काष्ठा udp_edges_table ext1;
		  ext1.mask0 = 0;
		  ext1.mask1 = 0;
		  ext1.maskx = 0;
		  ext1.edge_position = 0;
		  ext1.edge_mask0 = 0;
		  ext1.edge_mask1 = 0;
		  ext1.edge_maskx = 0;

		  क्रम (अचिन्हित pp = 0 ;  pp < port_गिनती() ; pp += 1) {
			चयन (row[pp+1]) {
			    हाल 'n':
			      edge_based_on_char(cur, 'N', pp);
			      edge_based_on_char(ext0, '_', pp);
			      need_ext0_table = true;
			      अवरोध;
			    हाल 'p':
			      edge_based_on_char(cur, 'P', pp);
			      edge_based_on_char(ext0, '+', pp);
			      need_ext0_table = true;
			      अवरोध;
			    हाल '*':
			      edge_based_on_char(cur, 'P', pp);
			      edge_based_on_char(ext0, 'N', pp);
			      edge_based_on_char(ext1, 'B', pp);
			      need_ext0_table = true;
			      need_ext1_table = true;
			      अवरोध;
			    शेष:
			      edge_based_on_char(cur, row[pp+1], pp);
			      edge_based_on_char(ext0, row[pp+1], pp);
			      edge_based_on_char(ext1, row[pp+1], pp);
			      अवरोध;
			}
		  }
		  edge_based_on_char(cur, row[0], port_गिनती());
		  edge_based_on_char(ext0, row[0], port_गिनती());
		  edge_based_on_char(ext1, row[0], port_गिनती());

		  चयन (row[port_गिनती()+1]) {
		      हाल '0':
			edges0_[idx_edg0++] = cur;
			अगर (need_ext0_table)
			      edges0_[idx_edg0++] = ext0;
			अगर (need_ext1_table)
			      edges0_[idx_edg0++] = ext1;
			अवरोध;
		      हाल '1':
			edges1_[idx_edg1++] = cur;
			अगर (need_ext0_table)
			      edges1_[idx_edg1++] = ext0;
			अगर (need_ext1_table)
			      edges1_[idx_edg1++] = ext1;
			अवरोध;
		      हाल 'x':
			अवरोध;
		      हाल '-':
			edgesL_[idx_edgL++] = cur;
			अगर (need_ext0_table)
			      edgesL_[idx_edgL++] = ext0;
			अगर (need_ext1_table)
			      edgesL_[idx_edgL++] = ext1;
			अवरोध;
		      शेष:
			ख_लिखो(मानक_त्रुटि, "Unsupported entry %c(%d).\n",
			        row[port_गिनती()+1], row[port_गिनती()+1]);
			निश्चित(0);
			अवरोध;
		  }

	    }
	    मिटाओ[] tab[idx];
      }
      मुक्त(tab);

      निश्चित(idx_edg0 == nedges0_);
      निश्चित(idx_edg1 == nedges1_);
      निश्चित(idx_edgL == nedgesL_);

}

bool चालक == (स्थिर udp_levels_table&a, स्थिर udp_levels_table&b)
{
      अगर (a.mask0 != b.mask0)
	    वापस false;
      अगर (a.mask1 != b.mask1)
	    वापस false;
      अगर (a.maskx != b.maskx)
	    वापस false;
      वापस true;
}

vvp_bit4_t vvp_udp_seq_s::calculate_output(स्थिर udp_levels_table&cur,
					   स्थिर udp_levels_table&prev,
					   vvp_bit4_t cur_out)
{
      अगर (cur == prev)
	    वापस cur_out;

      udp_levels_table cur_tmp = cur;

      अचिन्हित दीर्घ mask_out = 1UL << port_गिनती();
      चयन (cur_out) {
	  हाल BIT4_0:
	    cur_tmp.mask0 |= mask_out;
	    अवरोध;
	  हाल BIT4_1:
	    cur_tmp.mask1 |= mask_out;
	    अवरोध;
	  शेष:
	    cur_tmp.maskx |= mask_out;
	    अवरोध;
      }

      vvp_bit4_t lev = test_levels_(cur_tmp);
      अगर (lev == BIT4_Z) {
	    lev = test_edges_(cur_tmp, prev);
      }

      वापस lev;
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
vvp_bit4_t vvp_udp_seq_s::test_levels_(स्थिर udp_levels_table&cur)
{
      क्रम (अचिन्हित idx = 0 ;  idx < nlevels0_ ;  idx += 1) {
	    अगर (cur.mask0 != (cur.mask0 & levels0_[idx].mask0))
		  जारी;
	    अगर (cur.mask1 != (cur.mask1 & levels0_[idx].mask1))
		  जारी;
	    अगर (cur.maskx != (cur.maskx & levels0_[idx].maskx))
		  जारी;

	    वापस BIT4_0;
      }

      क्रम (अचिन्हित idx = 0 ;  idx < nlevels1_ ;  idx += 1) {
	    अगर (cur.mask0 != (cur.mask0 & levels1_[idx].mask0))
		  जारी;
	    अगर (cur.mask1 != (cur.mask1 & levels1_[idx].mask1))
		  जारी;
	    अगर (cur.maskx != (cur.maskx & levels1_[idx].maskx))
		  जारी;

	    वापस BIT4_1;
      }

	/* We need to test against an explicit X-output table, since
	   we need to distinguish from an X output and no match. */
      क्रम (अचिन्हित idx = 0 ;  idx < nlevelsx_ ;  idx += 1) {
	    अगर (cur.mask0 != (cur.mask0 & levelsx_[idx].mask0))
		  जारी;
	    अगर (cur.mask1 != (cur.mask1 & levelsx_[idx].mask1))
		  जारी;
	    अगर (cur.maskx != (cur.maskx & levelsx_[idx].maskx))
		  जारी;

	    वापस BIT4_X;
      }

	/* Test the table that requests the next output be the same as
	   the current output. This gets the current output from the
	   levels table that was passed in. */
      क्रम (अचिन्हित idx = 0 ;  idx < nlevelsL_ ;  idx += 1) {
	    अगर (cur.mask0 != (cur.mask0 & levelsL_[idx].mask0))
		  जारी;
	    अगर (cur.mask1 != (cur.mask1 & levelsL_[idx].mask1))
		  जारी;
	    अगर (cur.maskx != (cur.maskx & levelsL_[idx].maskx))
		  जारी;

	    अगर (cur.mask0 & (1 << port_गिनती()))
		  वापस BIT4_0;
	    अगर (cur.mask1 & (1 << port_गिनती()))
		  वापस BIT4_1;
	    अगर (cur.maskx & (1 << port_गिनती()))
		  वापस BIT4_X;

	    निश्चित(0);
	    वापस BIT4_X;
      }

	/* No explicit levels entry match. Return a Z to signal that
	   further testing is needed. */
      वापस BIT4_Z;
}

vvp_bit4_t vvp_udp_seq_s::test_edges_(स्थिर udp_levels_table&cur,
				      स्थिर udp_levels_table&prev)
{
	/* The edge_mask is true for all bits that are different in
	   the cur and prev tables. */
      अचिन्हित दीर्घ edge0_mask = cur.mask0 ^ prev.mask0;
      अचिन्हित दीर्घ edgex_mask = cur.maskx ^ prev.maskx;
      अचिन्हित दीर्घ edge1_mask = cur.mask1 ^ prev.mask1;

      अचिन्हित दीर्घ edge_mask = edge0_mask|edgex_mask|edge1_mask;
      edge_mask &= ~ (-1UL << port_गिनती());

	/* If there are no differences, then there are no edges. Give
	   up now. */
      अगर (edge_mask == 0)
	    वापस BIT4_X;

      अचिन्हित edge_position = 0;
      जबतक ((edge_mask&1) == 0) {
	    edge_mask >>= 1;
	    edge_position += 1;
      }

	/* We expect that there is exactly one edge in here. */
      निश्चित(edge_mask == 1);

      edge_mask = 1UL << edge_position;

      अचिन्हित edge_mask0 = (prev.mask0&edge_mask)? 1 : 0;
      अचिन्हित edge_maskx = (prev.maskx&edge_mask)? 1 : 0;
      अचिन्हित edge_mask1 = (prev.mask1&edge_mask)? 1 : 0;


	/* Now the edge_position and edge_mask* variables have the
	   values we use to test the applicability of the edge_table
	   entries. */

      क्रम (अचिन्हित idx = 0 ;  idx < nedges0_ ;  idx += 1) {
	    स्थिर काष्ठा udp_edges_table*row = edges0_ + idx;

	    अगर (row->edge_position != edge_position)
		  जारी;
	    अगर (edge_mask0 && !row->edge_mask0)
		  जारी;
	    अगर (edge_maskx && !row->edge_maskx)
		  जारी;
	    अगर (edge_mask1 && !row->edge_mask1)
		  जारी;
	    अगर (cur.mask0 != (cur.mask0 & row->mask0))
		  जारी;
	    अगर (cur.maskx != (cur.maskx & row->maskx))
		  जारी;
	    अगर (cur.mask1 != (cur.mask1 & row->mask1))
		  जारी;

	    वापस BIT4_0;
      }

      क्रम (अचिन्हित idx = 0 ;  idx < nedges1_ ;  idx += 1) {
	    स्थिर काष्ठा udp_edges_table*row = edges1_ + idx;

	    अगर (row->edge_position != edge_position)
		  जारी;
	    अगर (edge_mask0 && !row->edge_mask0)
		  जारी;
	    अगर (edge_maskx && !row->edge_maskx)
		  जारी;
	    अगर (edge_mask1 && !row->edge_mask1)
		  जारी;
	    अगर (cur.mask0 != (cur.mask0 & row->mask0))
		  जारी;
	    अगर (cur.maskx != (cur.maskx & row->maskx))
		  जारी;
	    अगर (cur.mask1 != (cur.mask1 & row->mask1))
		  जारी;

	    वापस BIT4_1;
      }

      क्रम (अचिन्हित idx = 0 ;  idx < nedgesL_ ;  idx += 1) {
	    स्थिर काष्ठा udp_edges_table*row = edgesL_ + idx;

	    अगर (row->edge_position != edge_position)
		  जारी;
	    अगर (edge_mask0 && !row->edge_mask0)
		  जारी;
	    अगर (edge_maskx && !row->edge_maskx)
		  जारी;
	    अगर (edge_mask1 && !row->edge_mask1)
		  जारी;
	    अगर (cur.mask0 != (cur.mask0 & row->mask0))
		  जारी;
	    अगर (cur.maskx != (cur.maskx & row->maskx))
		  जारी;
	    अगर (cur.mask1 != (cur.mask1 & row->mask1))
		  जारी;

	    अगर (cur.mask0 & (1 << port_गिनती()))
		  वापस BIT4_0;
	    अगर (cur.mask1 & (1 << port_गिनती()))
		  वापस BIT4_1;
	    अगर (cur.maskx & (1 << port_गिनती()))
		  वापस BIT4_X;

	    निश्चित(0);
	    वापस BIT4_X;
      }

      वापस BIT4_X;
}

vvp_udp_fun_core::vvp_udp_fun_core(vvp_net_t*net, vvp_udp_s*def)
: vvp_wide_fun_core(net, def->port_गिनती())
{
      def_ = def;
      cur_out_ = def_->get_init();
	// Assume initially that all the inputs are 1'bx
      current_.mask0 = 0;
      current_.mask1 = 0;
      current_.maskx = ~ ((-1UL) << port_गिनती());

        // If the initial value is 0 or 1, schedule the initial assignment
        // normally, so that any sensitive always processes can be started
        // first.
      अगर (cur_out_ != BIT4_X)
	    schedule_generic(यह, 0, false);
      अन्यथा
	    schedule_functor(यह);
}

vvp_udp_fun_core::~vvp_udp_fun_core()
{
}

/*
 * This method is used to propagate the initial value on startup.
 */
व्योम vvp_udp_fun_core::run_run()
{
      vvp_vector4_t tmp (1);
      tmp.set_bit(0, cur_out_);
      propagate_vec4(tmp);
}

व्योम vvp_udp_fun_core::recv_vec4_from_inputs(अचिन्हित port)
{
	/* For now, assume udps are 1-bit wide. */
      अचिन्हित port_width = value(port).size();
      निश्चित(port_width == 1);

      अचिन्हित दीर्घ mask = 1UL << port;

      udp_levels_table prev = current_;

      चयन (value(port).value(0)) {

	  हाल BIT4_0:
	    current_.mask0 |= mask;
	    current_.mask1 &= ~mask;
	    current_.maskx &= ~mask;
	    अवरोध;
	  हाल BIT4_1:
	    current_.mask0 &= ~mask;
	    current_.mask1 |= mask;
	    current_.maskx &= ~mask;
	    अवरोध;
	  शेष:
	    current_.mask0 &= ~mask;
	    current_.mask1 &= ~mask;
	    current_.maskx |= mask;
	    अवरोध;
      }

      vvp_bit4_t out_bit = def_->calculate_output(current_, prev, cur_out_);

      अगर (out_bit == cur_out_)
	    वापस;

      cur_out_ = out_bit;
      schedule_functor(यह);
}


/*
 * This function is called by the parser in response to a .udp
 * node. We create the nodes needed to integrate the UDP into the
 * netlist. The definition should be parsed already.
 */
व्योम compile_udp_functor(अक्षर*label, अक्षर*type,
			 अचिन्हित argc, काष्ठा symb_s*argv)
{
      काष्ठा vvp_udp_s *def = udp_find(type);
      निश्चित(def);
      मुक्त(type);

      vvp_net_t*ptr = नया vvp_net_t;
      vvp_udp_fun_core*core = नया vvp_udp_fun_core(ptr, def);
      ptr->fun = core;

      define_functor_प्रतीक(label, ptr);
      मुक्त(label);

      wide_inputs_connect(core, argc, argv);
      मुक्त(argv);
}
