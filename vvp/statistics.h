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
#وإلا
# include  <cstddef>
#endif

خارجي unsigned long گنتی_opcodes;
خارجي unsigned long گنتی_functors;
خارجي unsigned long گنتی_functors_logic;
خارجي unsigned long گنتی_functors_bufif;
خارجي unsigned long گنتی_functors_resolv;
خارجي unsigned long گنتی_functors_sig;
خارجي unsigned long گنتی_filters;
خارجي unsigned long گنتی_vvp_nets;
خارجي unsigned long گنتی_vpi_nets;
خارجي unsigned long گنتی_vpi_scopes;

خارجي unsigned long گنتی_net_arrays;
خارجي unsigned long گنتی_net_array_الفاظ;
خارجي unsigned long گنتی_var_arrays;
خارجي unsigned long گنتی_var_array_الفاظ;
خارجي unsigned long گنتی_real_arrays;
خارجي unsigned long گنتی_real_array_الفاظ;


خارجي unsigned long گنتی_time_events;
خارجي unsigned long گنتی_time_pool(فراغ);

خارجي unsigned long گنتی_assign_events;
خارجي unsigned long گنتی_assign4_pool(فراغ);
خارجي unsigned long گنتی_assign8_pool(فراغ);
خارجي unsigned long گنتی_assign_real_pool(فراغ);
خارجي unsigned long گنتی_assign_aword_pool(فراغ);
خارجي unsigned long گنتی_assign_arword_pool(فراغ);

خارجي unsigned long گنتی_gen_events;
خارجي unsigned long گنتی_gen_pool(فراغ);

خارجي size_t size_opcodes;
خارجي size_t size_vvp_nets;
خارجي size_t size_vvp_net_funs;

#endif /* IVL_statistics_H */
