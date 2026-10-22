#अगर_अघोषित IVL_parse_misc_H
#घोषणा IVL_parse_misc_H
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
बाह्य पूर्णांक compile_design(स्थिर अक्षर*path);

/*
 * This routine is called to check that the input file has a compatible
 * version.
 */
बाह्य व्योम verify_version(अक्षर *ivl_ver, अक्षर* commit);

/*
 * Set the default delay type for the $sdf_annotate task (min/typ/max).
 */
बाह्य व्योम set_delay_selection(स्थिर अक्षर* sel);

/*
 * various functions shared by the lexor and the parser.
 */
बाह्य पूर्णांक yylex(व्योम);
बाह्य व्योम yyerror(स्थिर अक्षर*संदेश);

बाह्य व्योम destroy_lexor();

/*
 * This is the path of the current source file.
 */
बाह्य स्थिर अक्षर*yypath;
बाह्य अचिन्हित yyline;

काष्ठा symb_s {
      अक्षर*text;
      अचिन्हित idx;
};

काष्ठा symbv_s {
      अचिन्हित cnt;
      काष्ठा symb_s*vect;
};

बाह्य व्योम symbv_init(काष्ठा symbv_s*obj);
बाह्य व्योम symbv_add(काष्ठा symbv_s*obj, काष्ठा symb_s item);

काष्ठा numbv_s {
      अचिन्हित cnt;
      दीर्घ*nvec;
};

काष्ठा enum_name_s {
      अक्षर*text;
      uint64_t val2;
      अक्षर*val4;
};

बाह्य व्योम numbv_init(काष्ठा numbv_s*obj);
बाह्य व्योम numbv_add(काष्ठा numbv_s*obj, दीर्घ item);
बाह्य व्योम numbv_clear(काष्ठा numbv_s*obj);

काष्ठा argv_s {
      अचिन्हित  argc;
      vpiHandle*argv;
      अक्षर    **syms;
};


बाह्य व्योम argv_init(काष्ठा argv_s*obj);
बाह्य व्योम argv_add(काष्ठा argv_s*obj, vpiHandle);
बाह्य व्योम argv_sym_add(काष्ठा argv_s*obj, अक्षर *);
बाह्य व्योम argv_sym_lookup(काष्ठा argv_s*obj);

#पूर्ण_अगर /* IVL_parse_misc_H */
