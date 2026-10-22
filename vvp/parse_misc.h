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
બાહ્ય પૂર્ણાંક compile_design(અચલ અક્ષર*path);

/*
 * This routine is called to check that the input file has a compatible
 * version.
 */
બાહ્ય શૂન્ય verify_version(અક્ષર *ivl_ver, અક્ષર* commit);

/*
 * Set the default delay type for the $sdf_annotate task (min/typ/max).
 */
બાહ્ય શૂન્ય set_delay_selection(અચલ અક્ષર* sel);

/*
 * various functions shared by the lexor and the parser.
 */
બાહ્ય પૂર્ણાંક yylex(શૂન્ય);
બાહ્ય શૂન્ય yyerror(અચલ અક્ષર*સંદેશ);

બાહ્ય શૂન્ય destroy_lexor();

/*
 * This is the path of the current source file.
 */
બાહ્ય અચલ અક્ષર*yypath;
બાહ્ય unsigned yyline;

સંરચના symb_s {
      અક્ષર*text;
      unsigned idx;
};

સંરચના symbv_s {
      unsigned cnt;
      સંરચના symb_s*vect;
};

બાહ્ય શૂન્ય symbv_init(સંરચના symbv_s*obj);
બાહ્ય શૂન્ય symbv_add(સંરચના symbv_s*obj, સંરચના symb_s item);

સંરચના numbv_s {
      unsigned cnt;
      long*nvec;
};

સંરચના enum_name_s {
      અક્ષર*text;
      uint64_t val2;
      અક્ષર*val4;
};

બાહ્ય શૂન્ય numbv_init(સંરચના numbv_s*obj);
બાહ્ય શૂન્ય numbv_add(સંરચના numbv_s*obj, long item);
બાહ્ય શૂન્ય numbv_clear(સંરચના numbv_s*obj);

સંરચના argv_s {
      unsigned  argc;
      vpiHandle*argv;
      અક્ષર    **syms;
};


બાહ્ય શૂન્ય argv_init(સંરચના argv_s*obj);
બાહ્ય શૂન્ય argv_add(સંરચના argv_s*obj, vpiHandle);
બાહ્ય શૂન્ય argv_sym_add(સંરચના argv_s*obj, અક્ષર *);
બાહ્ય શૂન્ય argv_sym_lookup(સંરચના argv_s*obj);

#endif /* IVL_parse_misc_H */
