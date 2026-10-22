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
ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା compile_design(ଅଚଳ ଅକ୍ଷର*path);

/*
 * This routine is called to check that the input file has a compatible
 * version.
 */
ବାହ୍ୟ ଶୂନ୍ୟ verify_version(ଅକ୍ଷର *ivl_ver, ଅକ୍ଷର* commit);

/*
 * Set the default delay type for the $sdf_annotate task (min/typ/max).
 */
ବାହ୍ୟ ଶୂନ୍ୟ set_delay_selection(ଅଚଳ ଅକ୍ଷର* sel);

/*
 * various functions shared by the lexor and the parser.
 */
ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା yylex(ଶୂନ୍ୟ);
ବାହ୍ୟ ଶୂନ୍ୟ yyerror(ଅଚଳ ଅକ୍ଷର*ସନ୍ଦେଶ);

ବାହ୍ୟ ଶୂନ୍ୟ destroy_lexor();

/*
 * This is the path of the current source file.
 */
ବାହ୍ୟ ଅଚଳ ଅକ୍ଷର*yypath;
ବାହ୍ୟ unsigned yyline;

ସଂରଚନା symb_s {
      ଅକ୍ଷର*text;
      unsigned idx;
};

ସଂରଚନା symbv_s {
      unsigned cnt;
      ସଂରଚନା symb_s*vect;
};

ବାହ୍ୟ ଶୂନ୍ୟ symbv_init(ସଂରଚନା symbv_s*obj);
ବାହ୍ୟ ଶୂନ୍ୟ symbv_add(ସଂରଚନା symbv_s*obj, ସଂରଚନା symb_s item);

ସଂରଚନା numbv_s {
      unsigned cnt;
      long*nvec;
};

ସଂରଚନା enum_name_s {
      ଅକ୍ଷର*text;
      uint64_t val2;
      ଅକ୍ଷର*val4;
};

ବାହ୍ୟ ଶୂନ୍ୟ numbv_init(ସଂରଚନା numbv_s*obj);
ବାହ୍ୟ ଶୂନ୍ୟ numbv_add(ସଂରଚନା numbv_s*obj, long item);
ବାହ୍ୟ ଶୂନ୍ୟ numbv_clear(ସଂରଚନା numbv_s*obj);

ସଂରଚନା argv_s {
      unsigned  argc;
      vpiHandle*argv;
      ଅକ୍ଷର    **syms;
};


ବାହ୍ୟ ଶୂନ୍ୟ argv_init(ସଂରଚନା argv_s*obj);
ବାହ୍ୟ ଶୂନ୍ୟ argv_add(ସଂରଚନା argv_s*obj, vpiHandle);
ବାହ୍ୟ ଶୂନ୍ୟ argv_sym_add(ସଂରଚନା argv_s*obj, ଅକ୍ଷର *);
ବାହ୍ୟ ଶୂନ୍ୟ argv_sym_lookup(ସଂରଚନା argv_s*obj);

#endif /* IVL_parse_misc_H */
