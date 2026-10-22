#నిర్వచితం_కాకపోతే IVL_parse_misc_H
#నిర్వచించు IVL_parse_misc_H
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
బాహ్య పూర్ణాంకం compile_design(స్థిరరాశి అక్షరం*మార్గం);

/*
 * This routine is called to check that the input file has a compatible
 * version.
 */
బాహ్య శూన్య verify_సంస్కరణ(అక్షరం *ivl_ver, అక్షరం* commit);

/*
 * Set the default delay type for the $sdf_annotate task (min/typ/max).
 */
బాహ్య శూన్య సమితి_delay_selection(స్థిరరాశి అక్షరం* sel);

/*
 * various functions shared by the lexor and the parser.
 */
బాహ్య పూర్ణాంకం yylex(శూన్య);
బాహ్య శూన్య yyerror(స్థిరరాశి అక్షరం*సందేశం);

బాహ్య శూన్య destroy_lexor();

/*
 * This is the path of the current source file.
 */
బాహ్య స్థిరరాశి అక్షరం*yypath;
బాహ్య గుర్తులేని yyline;

నిర్మాణం symb_s {
      అక్షరం*పాఠ్యం;
      గుర్తులేని idx;
};

నిర్మాణం symbv_s {
      గుర్తులేని cnt;
      నిర్మాణం symb_s*vect;
};

బాహ్య శూన్య symbv_init(నిర్మాణం symbv_s*obj);
బాహ్య శూన్య symbv_కూడు(నిర్మాణం symbv_s*obj, నిర్మాణం symb_s అంశం);

నిర్మాణం numbv_s {
      గుర్తులేని cnt;
      దీర్ఘ*nvec;
};

నిర్మాణం enum_name_s {
      అక్షరం*పాఠ్యం;
      uint64_t val2;
      అక్షరం*val4;
};

బాహ్య శూన్య numbv_init(నిర్మాణం numbv_s*obj);
బాహ్య శూన్య numbv_కూడు(నిర్మాణం numbv_s*obj, దీర్ఘ అంశం);
బాహ్య శూన్య numbv_clear(నిర్మాణం numbv_s*obj);

నిర్మాణం argv_s {
      గుర్తులేని  argc;
      vpiHandle*argv;
      అక్షరం    **syms;
};


బాహ్య శూన్య argv_init(నిర్మాణం argv_s*obj);
బాహ్య శూన్య argv_కూడు(నిర్మాణం argv_s*obj, vpiHandle);
బాహ్య శూన్య argv_sym_కూడు(నిర్మాణం argv_s*obj, అక్షరం *);
బాహ్య శూన్య argv_sym_lookup(నిర్మాణం argv_s*obj);

#అయితే_ముగింపు /* IVL_parse_misc_H */
