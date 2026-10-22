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
বাহ্যিক পূর্ণসংখ্যা compile_design(ধ্রুবক অক্ষর*path);

/*
 * This routine is called to check that the input file has a compatible
 * version.
 */
বাহ্যিক শূন্য verify_version(অক্ষর *ivl_ver, অক্ষর* commit);

/*
 * Set the default delay type for the $sdf_annotate task (min/typ/max).
 */
বাহ্যিক শূন্য set_delay_selection(ধ্রুবক অক্ষর* sel);

/*
 * various functions shared by the lexor and the parser.
 */
বাহ্যিক পূর্ণসংখ্যা yylex(শূন্য);
বাহ্যিক শূন্য yyerror(ধ্রুবক অক্ষর*বার্তা);

বাহ্যিক শূন্য destroy_lexor();

/*
 * This is the path of the current source file.
 */
বাহ্যিক ধ্রুবক অক্ষর*yypath;
বাহ্যিক unsigned yyline;

কাঠামো symb_s {
      অক্ষর*text;
      unsigned idx;
};

কাঠামো symbv_s {
      unsigned cnt;
      কাঠামো symb_s*vect;
};

বাহ্যিক শূন্য symbv_init(কাঠামো symbv_s*obj);
বাহ্যিক শূন্য symbv_add(কাঠামো symbv_s*obj, কাঠামো symb_s item);

কাঠামো numbv_s {
      unsigned cnt;
      long*nvec;
};

কাঠামো enum_name_s {
      অক্ষর*text;
      uint64_t val2;
      অক্ষর*val4;
};

বাহ্যিক শূন্য numbv_init(কাঠামো numbv_s*obj);
বাহ্যিক শূন্য numbv_add(কাঠামো numbv_s*obj, long item);
বাহ্যিক শূন্য numbv_clear(কাঠামো numbv_s*obj);

কাঠামো argv_s {
      unsigned  argc;
      vpiHandle*argv;
      অক্ষর    **syms;
};


বাহ্যিক শূন্য argv_init(কাঠামো argv_s*obj);
বাহ্যিক শূন্য argv_add(কাঠামো argv_s*obj, vpiHandle);
বাহ্যিক শূন্য argv_sym_add(কাঠামো argv_s*obj, অক্ষর *);
বাহ্যিক শূন্য argv_sym_lookup(কাঠামো argv_s*obj);

#endif /* IVL_parse_misc_H */
