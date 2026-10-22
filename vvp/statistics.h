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
#अण्णहा
# include  <cstddef>
#endif

बाह्य unsigned long गणनम्_opcodes;
बाह्य unsigned long गणनम्_functors;
बाह्य unsigned long गणनम्_functors_logic;
बाह्य unsigned long गणनम्_functors_bufif;
बाह्य unsigned long गणनम्_functors_resolv;
बाह्य unsigned long गणनम्_functors_sig;
बाह्य unsigned long गणनम्_filters;
बाह्य unsigned long गणनम्_vvp_nets;
बाह्य unsigned long गणनम्_vpi_nets;
बाह्य unsigned long गणनम्_vpi_scopes;

बाह्य unsigned long गणनम्_net_arrays;
बाह्य unsigned long गणनम्_net_array_शब्दाः;
बाह्य unsigned long गणनम्_var_arrays;
बाह्य unsigned long गणनम्_var_array_शब्दाः;
बाह्य unsigned long गणनम्_real_arrays;
बाह्य unsigned long गणनम्_real_array_शब्दाः;


बाह्य unsigned long गणनम्_time_events;
बाह्य unsigned long गणनम्_time_pool(शून्य);

बाह्य unsigned long गणनम्_assign_events;
बाह्य unsigned long गणनम्_assign4_pool(शून्य);
बाह्य unsigned long गणनम्_assign8_pool(शून्य);
बाह्य unsigned long गणनम्_assign_real_pool(शून्य);
बाह्य unsigned long गणनम्_assign_aword_pool(शून्य);
बाह्य unsigned long गणनम्_assign_arword_pool(शून्य);

बाह्य unsigned long गणनम्_gen_events;
बाह्य unsigned long गणनम्_gen_pool(शून्य);

बाह्य size_t size_opcodes;
बाह्य size_t size_vvp_nets;
बाह्य size_t size_vvp_net_funs;

#endif /* IVL_statistics_H */
