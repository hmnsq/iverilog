#که_نه_تعریف_شوی IVL_parse_misc_H
#تعریف_کړه IVL_parse_misc_H
/*
 * Copyright (c) 2001-2014 Stephen Williams (steve@icarus.com)
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

# include  "vpi_priv.h"

/*
 * This method is called to compile the design file. The input is read
 * and a list of statements is created.
 */
خارجي صحيح compile_design(ثابت حرف*لار);

/*
 * This routine is called to check that the input file has a compatible
 * version.
 */
خارجي فراغ verify_نسخه(حرف *ivl_ver, حرف* commit);

/*
 * Set the default delay type for the $sdf_annotate task (min/typ/max).
 */
خارجي فراغ سېټ_delay_selection(ثابت حرف* sel);

/*
 * various functions shared by the lexor and the parser.
 */
خارجي صحيح yylex(فراغ);
خارجي فراغ yyerror(ثابت حرف*پیغام);

خارجي فراغ destroy_lexor();

/*
 * This is the path of the current source file.
 */
خارجي ثابت حرف*yypath;
خارجي بینښه yyline;

بنية symb_s {
      حرف*متن;
      بینښه idx;
};

بنية symbv_s {
      بینښه cnt;
      بنية symb_s*vect;
};

خارجي فراغ symbv_init(بنية symbv_s*obj);
خارجي فراغ symbv_جمع_کړه(بنية symbv_s*obj, بنية symb_s توکی);

بنية numbv_s {
      بینښه cnt;
      اوږد*nvec;
};

بنية enum_name_s {
      حرف*متن;
      uint64_t val2;
      حرف*val4;
};

خارجي فراغ numbv_init(بنية numbv_s*obj);
خارجي فراغ numbv_جمع_کړه(بنية numbv_s*obj, اوږد توکی);
خارجي فراغ numbv_clear(بنية numbv_s*obj);

بنية argv_s {
      بینښه  argc;
      vpiHandle*argv;
      حرف    **syms;
};


خارجي فراغ argv_init(بنية argv_s*obj);
خارجي فراغ argv_جمع_کړه(بنية argv_s*obj, vpiHandle);
خارجي فراغ argv_sym_جمع_کړه(بنية argv_s*obj, حرف *);
خارجي فراغ argv_sym_lookup(بنية argv_s*obj);

#د_که_پای /* IVL_parse_misc_H */
