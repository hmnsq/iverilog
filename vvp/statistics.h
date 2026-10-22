#ifndef IVL_statistics_H
#define IVL_statistics_H
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
#ifdef __SUNPRO_CC
# include  <stddef.h>
#אחרת
# include  <cstddef>
#endif

خارجي unsigned long count_opcodes;
خارجي unsigned long count_functors;
خارجي unsigned long count_functors_logic;
خارجي unsigned long count_functors_bufif;
خارجي unsigned long count_functors_resolv;
خارجي unsigned long count_functors_sig;
خارجي unsigned long count_filters;
خارجي unsigned long count_vvp_nets;
خارجي unsigned long count_vpi_nets;
خارجي unsigned long count_vpi_scopes;

خارجي unsigned long count_net_arrays;
خارجي unsigned long count_net_array_words;
خارجي unsigned long count_var_arrays;
خارجي unsigned long count_var_array_words;
خارجي unsigned long count_real_arrays;
خارجي unsigned long count_real_array_words;


خارجي unsigned long count_time_events;
خارجي unsigned long count_time_pool(فراغ);

خارجي unsigned long count_assign_events;
خارجي unsigned long count_assign4_pool(فراغ);
خارجي unsigned long count_assign8_pool(فراغ);
خارجي unsigned long count_assign_real_pool(فراغ);
خارجي unsigned long count_assign_aword_pool(فراغ);
خارجي unsigned long count_assign_arword_pool(فراغ);

خارجي unsigned long count_gen_events;
خارجي unsigned long count_gen_pool(فراغ);

خارجي size_t size_opcodes;
خارجي size_t size_vvp_nets;
خارجي size_t size_vvp_net_funs;

#endif /* IVL_statistics_H */
