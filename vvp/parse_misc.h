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
வெளி முழு compile_design(மாறிலி எழுத்து*path);

/*
 * This routine is called to check that the input file has a compatible
 * version.
 */
வெளி வெறுமை verify_version(எழுத்து *ivl_ver, எழுத்து* commit);

/*
 * Set the default delay type for the $sdf_annotate task (min/typ/max).
 */
வெளி வெறுமை set_delay_selection(மாறிலி எழுத்து* sel);

/*
 * various functions shared by the lexor and the parser.
 */
வெளி முழு yylex(வெறுமை);
வெளி வெறுமை yyerror(மாறிலி எழுத்து*செய்தி);

வெளி வெறுமை destroy_lexor();

/*
 * This is the path of the current source file.
 */
வெளி மாறிலி எழுத்து*yypath;
வெளி unsigned yyline;

கட்டமைப்பு symb_s {
      எழுத்து*text;
      unsigned idx;
};

கட்டமைப்பு symbv_s {
      unsigned cnt;
      கட்டமைப்பு symb_s*vect;
};

வெளி வெறுமை symbv_init(கட்டமைப்பு symbv_s*obj);
வெளி வெறுமை symbv_add(கட்டமைப்பு symbv_s*obj, கட்டமைப்பு symb_s item);

கட்டமைப்பு numbv_s {
      unsigned cnt;
      long*nvec;
};

கட்டமைப்பு enum_name_s {
      எழுத்து*text;
      uint64_t val2;
      எழுத்து*val4;
};

வெளி வெறுமை numbv_init(கட்டமைப்பு numbv_s*obj);
வெளி வெறுமை numbv_add(கட்டமைப்பு numbv_s*obj, long item);
வெளி வெறுமை numbv_clear(கட்டமைப்பு numbv_s*obj);

கட்டமைப்பு argv_s {
      unsigned  argc;
      vpiHandle*argv;
      எழுத்து    **syms;
};


வெளி வெறுமை argv_init(கட்டமைப்பு argv_s*obj);
வெளி வெறுமை argv_add(கட்டமைப்பு argv_s*obj, vpiHandle);
வெளி வெறுமை argv_sym_add(கட்டமைப்பு argv_s*obj, எழுத்து *);
வெளி வெறுமை argv_sym_lookup(கட்டமைப்பு argv_s*obj);

#endif /* IVL_parse_misc_H */
