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
ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ compile_design(ಧ್ರುವ ವರ್ಣ*path);

/*
 * This routine is called to check that the input file has a compatible
 * version.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ verify_version(ವರ್ಣ *ivl_ver, ವರ್ಣ* commit);

/*
 * Set the default delay type for the $sdf_annotate task (min/typ/max).
 */
ಬಾಹ್ಯ ಶೂನ್ಯ set_delay_selection(ಧ್ರುವ ವರ್ಣ* sel);

/*
 * various functions shared by the lexor and the parser.
 */
ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ yylex(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ yyerror(ಧ್ರುವ ವರ್ಣ*ಸಂದೇಶ);

ಬಾಹ್ಯ ಶೂನ್ಯ destroy_lexor();

/*
 * This is the path of the current source file.
 */
ಬಾಹ್ಯ ಧ್ರುವ ವರ್ಣ*yypath;
ಬಾಹ್ಯ unsigned yyline;

ಸಂರಚನಾ symb_s {
      ವರ್ಣ*text;
      unsigned idx;
};

ಸಂರಚನಾ symbv_s {
      unsigned cnt;
      ಸಂರಚನಾ symb_s*vect;
};

ಬಾಹ್ಯ ಶೂನ್ಯ symbv_init(ಸಂರಚನಾ symbv_s*obj);
ಬಾಹ್ಯ ಶೂನ್ಯ symbv_add(ಸಂರಚನಾ symbv_s*obj, ಸಂರಚನಾ symb_s item);

ಸಂರಚನಾ numbv_s {
      unsigned cnt;
      long*nvec;
};

ಸಂರಚನಾ enum_name_s {
      ವರ್ಣ*text;
      uint64_t val2;
      ವರ್ಣ*val4;
};

ಬಾಹ್ಯ ಶೂನ್ಯ numbv_init(ಸಂರಚನಾ numbv_s*obj);
ಬಾಹ್ಯ ಶೂನ್ಯ numbv_add(ಸಂರಚನಾ numbv_s*obj, long item);
ಬಾಹ್ಯ ಶೂನ್ಯ numbv_clear(ಸಂರಚನಾ numbv_s*obj);

ಸಂರಚನಾ argv_s {
      unsigned  argc;
      vpiHandle*argv;
      ವರ್ಣ    **syms;
};


ಬಾಹ್ಯ ಶೂನ್ಯ argv_init(ಸಂರಚನಾ argv_s*obj);
ಬಾಹ್ಯ ಶೂನ್ಯ argv_add(ಸಂರಚನಾ argv_s*obj, vpiHandle);
ಬಾಹ್ಯ ಶೂನ್ಯ argv_sym_add(ಸಂರಚನಾ argv_s*obj, ವರ್ಣ *);
ಬಾಹ್ಯ ಶೂನ್ಯ argv_sym_lookup(ಸಂರಚನಾ argv_s*obj);

#endif /* IVL_parse_misc_H */
