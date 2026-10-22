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
#இல்லையெனில்
# include  <cstddef>
#endif

வெளி unsigned long மொத்தம்_opcodes;
வெளி unsigned long மொத்தம்_functors;
வெளி unsigned long மொத்தம்_functors_logic;
வெளி unsigned long மொத்தம்_functors_bufif;
வெளி unsigned long மொத்தம்_functors_resolv;
வெளி unsigned long மொத்தம்_functors_sig;
வெளி unsigned long மொத்தம்_filters;
வெளி unsigned long மொத்தம்_vvp_nets;
வெளி unsigned long மொத்தம்_vpi_nets;
வெளி unsigned long மொத்தம்_vpi_scopes;

வெளி unsigned long மொத்தம்_net_arrays;
வெளி unsigned long மொத்தம்_net_array_சொற்கள்;
வெளி unsigned long மொத்தம்_var_arrays;
வெளி unsigned long மொத்தம்_var_array_சொற்கள்;
வெளி unsigned long மொத்தம்_real_arrays;
வெளி unsigned long மொத்தம்_real_array_சொற்கள்;


வெளி unsigned long மொத்தம்_time_events;
வெளி unsigned long மொத்தம்_time_pool(வெறுமை);

வெளி unsigned long மொத்தம்_assign_events;
வெளி unsigned long மொத்தம்_assign4_pool(வெறுமை);
வெளி unsigned long மொத்தம்_assign8_pool(வெறுமை);
வெளி unsigned long மொத்தம்_assign_real_pool(வெறுமை);
வெளி unsigned long மொத்தம்_assign_aword_pool(வெறுமை);
வெளி unsigned long மொத்தம்_assign_arword_pool(வெறுமை);

வெளி unsigned long மொத்தம்_gen_events;
வெளி unsigned long மொத்தம்_gen_pool(வெறுமை);

வெளி size_t size_opcodes;
வெளி size_t size_vvp_nets;
வெளி size_t size_vvp_net_funs;

#endif /* IVL_statistics_H */
