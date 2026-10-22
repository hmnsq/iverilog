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
বাহ্য পূর্ণাঙ্ক compile_design(ধ্রুৱ ৱর্ণ*path);

/*
 * This routine is called to check that the input file has a compatible
 * version.
 */
বাহ্য শূন্য verify_version(ৱর্ণ *ivl_ver, ৱর্ণ* commit);

/*
 * Set the default delay type for the $sdf_annotate task (min/typ/max).
 */
বাহ্য শূন্য set_delay_selection(ধ্রুৱ ৱর্ণ* sel);

/*
 * various functions shared by the lexor and the parser.
 */
বাহ্য পূর্ণাঙ্ক yylex(শূন্য);
বাহ্য শূন্য yyerror(ধ্রুৱ ৱর্ণ*বাৰ্তা);

বাহ্য শূন্য destroy_lexor();

/*
 * This is the path of the current source file.
 */
বাহ্য ধ্রুৱ ৱর্ণ*yypath;
বাহ্য unsigned yyline;

সংরচনা symb_s {
      ৱর্ণ*text;
      unsigned idx;
};

সংরচনা symbv_s {
      unsigned cnt;
      সংরচনা symb_s*vect;
};

বাহ্য শূন্য symbv_init(সংরচনা symbv_s*obj);
বাহ্য শূন্য symbv_add(সংরচনা symbv_s*obj, সংরচনা symb_s item);

সংরচনা numbv_s {
      unsigned cnt;
      long*nvec;
};

সংরচনা enum_name_s {
      ৱর্ণ*text;
      uint64_t val2;
      ৱর্ণ*val4;
};

বাহ্য শূন্য numbv_init(সংরচনা numbv_s*obj);
বাহ্য শূন্য numbv_add(সংরচনা numbv_s*obj, long item);
বাহ্য শূন্য numbv_clear(সংরচনা numbv_s*obj);

সংরচনা argv_s {
      unsigned  argc;
      vpiHandle*argv;
      ৱর্ণ    **syms;
};


বাহ্য শূন্য argv_init(সংরচনা argv_s*obj);
বাহ্য শূন্য argv_add(সংরচনা argv_s*obj, vpiHandle);
বাহ্য শূন্য argv_sym_add(সংরচনা argv_s*obj, ৱর্ণ *);
বাহ্য শূন্য argv_sym_lookup(সংরচনা argv_s*obj);

#endif /* IVL_parse_misc_H */
