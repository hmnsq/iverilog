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
بیرونی صحیح compile_design(ثابت نویسه*path);

/*
 * This routine is called to check that the input file has a compatible
 * version.
 */
بیرونی تهی verify_version(نویسه *ivl_ver, نویسه* commit);

/*
 * Set the default delay type for the $sdf_annotate task (min/typ/max).
 */
بیرونی تهی set_delay_selection(ثابت نویسه* sel);

/*
 * various functions shared by the lexor and the parser.
 */
بیرونی صحیح yylex(تهی);
بیرونی تهی yyerror(ثابت نویسه*پیام);

بیرونی تهی destroy_lexor();

/*
 * This is the path of the current source file.
 */
بیرونی ثابت نویسه*yypath;
بیرونی unsigned yyline;

ساختار symb_s {
      نویسه*text;
      unsigned idx;
};

ساختار symbv_s {
      unsigned cnt;
      ساختار symb_s*vect;
};

بیرونی تهی symbv_init(ساختار symbv_s*obj);
بیرونی تهی symbv_add(ساختار symbv_s*obj, ساختار symb_s item);

ساختار numbv_s {
      unsigned cnt;
      long*nvec;
};

ساختار enum_name_s {
      نویسه*text;
      uint64_t val2;
      نویسه*val4;
};

بیرونی تهی numbv_init(ساختار numbv_s*obj);
بیرونی تهی numbv_add(ساختار numbv_s*obj, long item);
بیرونی تهی numbv_clear(ساختار numbv_s*obj);

ساختار argv_s {
      unsigned  argc;
      vpiHandle*argv;
      نویسه    **syms;
};


بیرونی تهی argv_init(ساختار argv_s*obj);
بیرونی تهی argv_add(ساختار argv_s*obj, vpiHandle);
بیرونی تهی argv_sym_add(ساختار argv_s*obj, نویسه *);
بیرونی تهی argv_sym_lookup(ساختار argv_s*obj);

#endif /* IVL_parse_misc_H */
