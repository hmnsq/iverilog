#اگر_غیر_تعریف_شدہ IVL_parse_misc_H
#تعریف_کرو IVL_parse_misc_H
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
خارجی صحیح compile_design(ثابت حرف*راستہ);

/*
 * This routine is called to check that the input file has a compatible
 * version.
 */
خارجی خالی verify_نسخہ(حرف *ivl_ver, حرف* commit);

/*
 * Set the default delay type for the $sdf_annotate task (min/typ/max).
 */
خارجی خالی سیٹ_delay_selection(ثابت حرف* sel);

/*
 * various functions shared by the lexor and the parser.
 */
خارجی صحیح yylex(خالی);
خارجی خالی yyerror(ثابت حرف*پیغام);

خارجی خالی destroy_lexor();

/*
 * This is the path of the current source file.
 */
خارجی ثابت حرف*yypath;
خارجی غیرعلامتی yyline;

ساخت symb_s {
      حرف*متن;
      غیرعلامتی idx;
};

ساخت symbv_s {
      غیرعلامتی cnt;
      ساخت symb_s*vect;
};

خارجی خالی symbv_init(ساخت symbv_s*obj);
خارجی خالی symbv_جوڑو(ساخت symbv_s*obj, ساخت symb_s شے);

ساخت numbv_s {
      غیرعلامتی cnt;
      طویل*nvec;
};

ساخت enum_name_s {
      حرف*متن;
      uint64_t val2;
      حرف*val4;
};

خارجی خالی numbv_init(ساخت numbv_s*obj);
خارجی خالی numbv_جوڑو(ساخت numbv_s*obj, طویل شے);
خارجی خالی numbv_clear(ساخت numbv_s*obj);

ساخت argv_s {
      غیرعلامتی  argc;
      vpiHandle*argv;
      حرف    **syms;
};


خارجی خالی argv_init(ساخت argv_s*obj);
خارجی خالی argv_جوڑو(ساخت argv_s*obj, vpiHandle);
خارجی خالی argv_sym_جوڑو(ساخت argv_s*obj, حرف *);
خارجی خالی argv_sym_lookup(ساخت argv_s*obj);

#اگر_ختم /* IVL_parse_misc_H */
