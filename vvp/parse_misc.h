#यदि_अपरिभाषितम् IVL_parse_misc_H
#परिभाषय IVL_parse_misc_H
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
बाह्य पूर्णाङ्क compile_design(ध्रुव वर्ण*मार्गः);

/*
 * This routine is called to check that the input file has a compatible
 * version.
 */
बाह्य शून्य verify_संस्करणम्(वर्ण *ivl_ver, वर्ण* commit);

/*
 * Set the default delay type for the $sdf_annotate task (min/typ/max).
 */
बाह्य शून्य समुच्चयः_delay_selection(ध्रुव वर्ण* sel);

/*
 * various functions shared by the lexor and the parser.
 */
बाह्य पूर्णाङ्क yylex(शून्य);
बाह्य शून्य yyerror(ध्रुव वर्ण*सन्देशः);

बाह्य शून्य destroy_lexor();

/*
 * This is the path of the current source file.
 */
बाह्य ध्रुव वर्ण*yypath;
बाह्य अचिह्नित yyline;

संरचना symb_s {
      वर्ण*text;
      अचिह्नित idx;
};

संरचना symbv_s {
      अचिह्नित cnt;
      संरचना symb_s*vect;
};

बाह्य शून्य symbv_init(संरचना symbv_s*obj);
बाह्य शून्य symbv_सङ्कलय(संरचना symbv_s*obj, संरचना symb_s item);

संरचना numbv_s {
      अचिह्नित cnt;
      दीर्घ*nvec;
};

संरचना enum_name_s {
      वर्ण*text;
      uint64_t val2;
      वर्ण*val4;
};

बाह्य शून्य numbv_init(संरचना numbv_s*obj);
बाह्य शून्य numbv_सङ्कलय(संरचना numbv_s*obj, दीर्घ item);
बाह्य शून्य numbv_clear(संरचना numbv_s*obj);

संरचना argv_s {
      अचिह्नित  argc;
      vpiHandle*argv;
      वर्ण    **syms;
};


बाह्य शून्य argv_init(संरचना argv_s*obj);
बाह्य शून्य argv_सङ्कलय(संरचना argv_s*obj, vpiHandle);
बाह्य शून्य argv_sym_सङ्कलय(संरचना argv_s*obj, वर्ण *);
बाह्य शून्य argv_sym_lookup(संरचना argv_s*obj);

#यदि_अन्तः /* IVL_parse_misc_H */
