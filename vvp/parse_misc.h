#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_parse_misc_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_parse_misc_H
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
ਬਾਹਰੀ ਪੂਰਨ compile_design(ਅਚਲ ਅੱਖਰ*ਰਸਤਾ);

/*
 * This routine is called to check that the input file has a compatible
 * version.
 */
ਬਾਹਰੀ ਖਾਲੀ verify_ਸੰਸਕਰਣ(ਅੱਖਰ *ivl_ver, ਅੱਖਰ* commit);

/*
 * Set the default delay type for the $sdf_annotate task (min/typ/max).
 */
ਬਾਹਰੀ ਖਾਲੀ ਸੈੱਟ_delay_selection(ਅਚਲ ਅੱਖਰ* sel);

/*
 * various functions shared by the lexor and the parser.
 */
ਬਾਹਰੀ ਪੂਰਨ yylex(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ yyerror(ਅਚਲ ਅੱਖਰ*ਸੁਨੇਹਾ);

ਬਾਹਰੀ ਖਾਲੀ destroy_lexor();

/*
 * This is the path of the current source file.
 */
ਬਾਹਰੀ ਅਚਲ ਅੱਖਰ*yypath;
ਬਾਹਰੀ ਬਿਨਾਨਿਸ਼ਾਨੀ yyline;

ਬਣਤਰ symb_s {
      ਅੱਖਰ*ਪਾਠ;
      ਬਿਨਾਨਿਸ਼ਾਨੀ idx;
};

ਬਣਤਰ symbv_s {
      ਬਿਨਾਨਿਸ਼ਾਨੀ cnt;
      ਬਣਤਰ symb_s*vect;
};

ਬਾਹਰੀ ਖਾਲੀ symbv_init(ਬਣਤਰ symbv_s*obj);
ਬਾਹਰੀ ਖਾਲੀ symbv_ਜੋੜੋ(ਬਣਤਰ symbv_s*obj, ਬਣਤਰ symb_s ਵਸਤੂ);

ਬਣਤਰ numbv_s {
      ਬਿਨਾਨਿਸ਼ਾਨੀ cnt;
      ਲੰਮਾ*nvec;
};

ਬਣਤਰ enum_name_s {
      ਅੱਖਰ*ਪਾਠ;
      uint64_t val2;
      ਅੱਖਰ*val4;
};

ਬਾਹਰੀ ਖਾਲੀ numbv_init(ਬਣਤਰ numbv_s*obj);
ਬਾਹਰੀ ਖਾਲੀ numbv_ਜੋੜੋ(ਬਣਤਰ numbv_s*obj, ਲੰਮਾ ਵਸਤੂ);
ਬਾਹਰੀ ਖਾਲੀ numbv_clear(ਬਣਤਰ numbv_s*obj);

ਬਣਤਰ argv_s {
      ਬਿਨਾਨਿਸ਼ਾਨੀ  argc;
      vpiHandle*argv;
      ਅੱਖਰ    **syms;
};


ਬਾਹਰੀ ਖਾਲੀ argv_init(ਬਣਤਰ argv_s*obj);
ਬਾਹਰੀ ਖਾਲੀ argv_ਜੋੜੋ(ਬਣਤਰ argv_s*obj, vpiHandle);
ਬਾਹਰੀ ਖਾਲੀ argv_sym_ਜੋੜੋ(ਬਣਤਰ argv_s*obj, ਅੱਖਰ *);
ਬਾਹਰੀ ਖਾਲੀ argv_sym_lookup(ਬਣਤਰ argv_s*obj);

#ਜੇ_ਅੰਤ /* IVL_parse_misc_H */
