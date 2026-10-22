#که_نه_تعریف_شوی IVL_statistics_H
#تعریف_کړه IVL_statistics_H
/*
 * Copyright (c) 2002-2014 Stephen Williams (steve@icarus.com)
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

// The SunPro C++ compiler is broken and does not define size_t in cstddef.
#که_تعریف_شوی __SUNPRO_CC
# include  <stddef.h>
#وإلا
# include  <cstddef>
#د_که_پای

خارجي بینښه اوږد شمېر_opcodes;
خارجي بینښه اوږد شمېر_functors;
خارجي بینښه اوږد شمېر_functors_logic;
خارجي بینښه اوږد شمېر_functors_bufif;
خارجي بینښه اوږد شمېر_functors_resolv;
خارجي بینښه اوږد شمېر_functors_sig;
خارجي بینښه اوږد شمېر_filters;
خارجي بینښه اوږد شمېر_vvp_nets;
خارجي بینښه اوږد شمېر_vpi_nets;
خارجي بینښه اوږد شمېر_vpi_scopes;

خارجي بینښه اوږد شمېر_net_arrays;
خارجي بینښه اوږد شمېر_net_array_ټکي;
خارجي بینښه اوږد شمېر_var_arrays;
خارجي بینښه اوږد شمېر_var_array_ټکي;
خارجي بینښه اوږد شمېر_real_arrays;
خارجي بینښه اوږد شمېر_real_array_ټکي;


خارجي بینښه اوږد شمېر_وخت_events;
خارجي بینښه اوږد شمېر_وخت_pool(فراغ);

خارجي بینښه اوږد شمېر_ټاکل_events;
خارجي بینښه اوږد شمېر_assign4_pool(فراغ);
خارجي بینښه اوږد شمېر_assign8_pool(فراغ);
خارجي بینښه اوږد شمېر_ټاکل_real_pool(فراغ);
خارجي بینښه اوږد شمېر_ټاکل_aword_pool(فراغ);
خارجي بینښه اوږد شمېر_ټاکل_arword_pool(فراغ);

خارجي بینښه اوږد شمېر_gen_events;
خارجي بینښه اوږد شمېر_gen_pool(فراغ);

خارجي size_t اندازه_opcodes;
خارجي size_t اندازه_vvp_nets;
خارجي size_t اندازه_vvp_net_funs;

#د_که_پای /* IVL_statistics_H */
