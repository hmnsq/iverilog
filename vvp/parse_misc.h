#ifndef IVL_parse_misc_H
#define IVL_parse_misc_H
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
extern පූර්ණ_සංඛ්‍යා compile_design(const var_na*path);

/*
 * This routine is called to check that the input file has a compatible
 * version.
 */
extern void verify_version(var_na *ivl_ver, var_na* commit);

/*
 * Set the default delay type for the $sdf_annotate task (min/typ/max).
 */
extern void set_delay_selection(const var_na* sel);

/*
 * various functions shared by the lexor and the parser.
 */
extern පූර්ණ_සංඛ්‍යා yylex(void);
extern void yyerror(const var_na*msg);

extern void destroy_lexor();

/*
 * This is the path of the current source file.
 */
extern const var_na*yypath;
extern unsigned yyline;

sa_nracanaa symb_s {
      var_na*text;
      unsigned idx;
};

sa_nracanaa symbv_s {
      unsigned cnt;
      sa_nracanaa symb_s*vect;
};

extern void symbv_init(sa_nracanaa symbv_s*obj);
extern void symbv_add(sa_nracanaa symbv_s*obj, sa_nracanaa symb_s item);

sa_nracanaa numbv_s {
      unsigned cnt;
      long*nvec;
};

sa_nracanaa enum_name_s {
      var_na*text;
      uint64_t val2;
      var_na*val4;
};

extern void numbv_init(sa_nracanaa numbv_s*obj);
extern void numbv_add(sa_nracanaa numbv_s*obj, long item);
extern void numbv_clear(sa_nracanaa numbv_s*obj);

sa_nracanaa argv_s {
      unsigned  argc;
      vpiHandle*argv;
      var_na    **syms;
};


extern void argv_init(sa_nracanaa argv_s*obj);
extern void argv_add(sa_nracanaa argv_s*obj, vpiHandle);
extern void argv_sym_add(sa_nracanaa argv_s*obj, var_na *);
extern void argv_sym_lookup(sa_nracanaa argv_s*obj);

#endif /* IVL_parse_misc_H */
