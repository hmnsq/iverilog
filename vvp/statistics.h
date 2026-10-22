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
#وگرنه
# include  <cstddef>
#endif

بیرونی unsigned long شمار_opcodes;
بیرونی unsigned long شمار_functors;
بیرونی unsigned long شمار_functors_logic;
بیرونی unsigned long شمار_functors_bufif;
بیرونی unsigned long شمار_functors_resolv;
بیرونی unsigned long شمار_functors_sig;
بیرونی unsigned long شمار_filters;
بیرونی unsigned long شمار_vvp_nets;
بیرونی unsigned long شمار_vpi_nets;
بیرونی unsigned long شمار_vpi_scopes;

بیرونی unsigned long شمار_net_arrays;
بیرونی unsigned long شمار_net_array_واژگان;
بیرونی unsigned long شمار_var_arrays;
بیرونی unsigned long شمار_var_array_واژگان;
بیرونی unsigned long شمار_real_arrays;
بیرونی unsigned long شمار_real_array_واژگان;


بیرونی unsigned long شمار_time_events;
بیرونی unsigned long شمار_time_pool(تهی);

بیرونی unsigned long شمار_assign_events;
بیرونی unsigned long شمار_assign4_pool(تهی);
بیرونی unsigned long شمار_assign8_pool(تهی);
بیرونی unsigned long شمار_assign_real_pool(تهی);
بیرونی unsigned long شمار_assign_aword_pool(تهی);
بیرونی unsigned long شمار_assign_arword_pool(تهی);

بیرونی unsigned long شمار_gen_events;
بیرونی unsigned long شمار_gen_pool(تهی);

بیرونی size_t size_opcodes;
بیرونی size_t size_vvp_nets;
بیرونی size_t size_vvp_net_funs;

#endif /* IVL_statistics_H */
