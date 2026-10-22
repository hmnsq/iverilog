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
ബാഹ്യ പൂർണ്ണസംഖ്യ compile_design(സ്ഥിരം അക്ഷരം*path);

/*
 * This routine is called to check that the input file has a compatible
 * version.
 */
ബാഹ്യ ശൂന്യം verify_version(അക്ഷരം *ivl_ver, അക്ഷരം* commit);

/*
 * Set the default delay type for the $sdf_annotate task (min/typ/max).
 */
ബാഹ്യ ശൂന്യം set_delay_selection(സ്ഥിരം അക്ഷരം* sel);

/*
 * various functions shared by the lexor and the parser.
 */
ബാഹ്യ പൂർണ്ണസംഖ്യ yylex(ശൂന്യം);
ബാഹ്യ ശൂന്യം yyerror(സ്ഥിരം അക്ഷരം*സന്ദേശം);

ബാഹ്യ ശൂന്യം destroy_lexor();

/*
 * This is the path of the current source file.
 */
ബാഹ്യ സ്ഥിരം അക്ഷരം*yypath;
ബാഹ്യ unsigned yyline;

ഘടന symb_s {
      അക്ഷരം*text;
      unsigned idx;
};

ഘടന symbv_s {
      unsigned cnt;
      ഘടന symb_s*vect;
};

ബാഹ്യ ശൂന്യം symbv_init(ഘടന symbv_s*obj);
ബാഹ്യ ശൂന്യം symbv_add(ഘടന symbv_s*obj, ഘടന symb_s item);

ഘടന numbv_s {
      unsigned cnt;
      long*nvec;
};

ഘടന enum_name_s {
      അക്ഷരം*text;
      uint64_t val2;
      അക്ഷരം*val4;
};

ബാഹ്യ ശൂന്യം numbv_init(ഘടന numbv_s*obj);
ബാഹ്യ ശൂന്യം numbv_add(ഘടന numbv_s*obj, long item);
ബാഹ്യ ശൂന്യം numbv_clear(ഘടന numbv_s*obj);

ഘടന argv_s {
      unsigned  argc;
      vpiHandle*argv;
      അക്ഷരം    **syms;
};


ബാഹ്യ ശൂന്യം argv_init(ഘടന argv_s*obj);
ബാഹ്യ ശൂന്യം argv_add(ഘടന argv_s*obj, vpiHandle);
ബാഹ്യ ശൂന്യം argv_sym_add(ഘടന argv_s*obj, അക്ഷരം *);
ബാഹ്യ ശൂന്യം argv_sym_lookup(ഘടന argv_s*obj);

#endif /* IVL_parse_misc_H */
