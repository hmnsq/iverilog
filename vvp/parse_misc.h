#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_parse_misc_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_parse_misc_H
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
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ compile_design(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಮಾರ್ಗ);

/*
 * This routine is called to check that the input file has a compatible
 * version.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ verify_ಆವೃತ್ತಿ(ಅಕ್ಷರ *ivl_ver, ಅಕ್ಷರ* commit);

/*
 * Set the default delay type for the $sdf_annotate task (min/typ/max).
 */
ಬಾಹ್ಯ ಶೂನ್ಯ ಗಣ_delay_selection(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* sel);

/*
 * various functions shared by the lexor and the parser.
 */
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ yylex(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ yyerror(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಸಂದೇಶ);

ಬಾಹ್ಯ ಶೂನ್ಯ destroy_lexor();

/*
 * This is the path of the current source file.
 */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*yypath;
ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ yyline;

ರಚನೆ symb_s {
      ಅಕ್ಷರ*ಪಠ್ಯ;
      ಚಿಹ್ನೆರಹಿತ idx;
};

ರಚನೆ symbv_s {
      ಚಿಹ್ನೆರಹಿತ cnt;
      ರಚನೆ symb_s*vect;
};

ಬಾಹ್ಯ ಶೂನ್ಯ symbv_init(ರಚನೆ symbv_s*obj);
ಬಾಹ್ಯ ಶೂನ್ಯ symbv_ಕೂಡಿಸು(ರಚನೆ symbv_s*obj, ರಚನೆ symb_s ಅಂಶ);

ರಚನೆ numbv_s {
      ಚಿಹ್ನೆರಹಿತ cnt;
      ದೀರ್ಘ*nvec;
};

ರಚನೆ enum_name_s {
      ಅಕ್ಷರ*ಪಠ್ಯ;
      uint64_t val2;
      ಅಕ್ಷರ*val4;
};

ಬಾಹ್ಯ ಶೂನ್ಯ numbv_init(ರಚನೆ numbv_s*obj);
ಬಾಹ್ಯ ಶೂನ್ಯ numbv_ಕೂಡಿಸು(ರಚನೆ numbv_s*obj, ದೀರ್ಘ ಅಂಶ);
ಬಾಹ್ಯ ಶೂನ್ಯ numbv_clear(ರಚನೆ numbv_s*obj);

ರಚನೆ argv_s {
      ಚಿಹ್ನೆರಹಿತ  argc;
      vpiHandle*argv;
      ಅಕ್ಷರ    **syms;
};


ಬಾಹ್ಯ ಶೂನ್ಯ argv_init(ರಚನೆ argv_s*obj);
ಬಾಹ್ಯ ಶೂನ್ಯ argv_ಕೂಡಿಸು(ರಚನೆ argv_s*obj, vpiHandle);
ಬಾಹ್ಯ ಶೂನ್ಯ argv_sym_ಕೂಡಿಸು(ರಚನೆ argv_s*obj, ಅಕ್ಷರ *);
ಬಾಹ್ಯ ಶೂನ್ಯ argv_sym_lookup(ರಚನೆ argv_s*obj);

#ಆದರೆ_ಅಂತ್ಯ /* IVL_parse_misc_H */
