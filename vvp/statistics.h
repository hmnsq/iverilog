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
#অন্যথা
# include  <cstddef>
#endif

বাহ্য unsigned long গুনতি_opcodes;
বাহ্য unsigned long গুনতি_functors;
বাহ্য unsigned long গুনতি_functors_logic;
বাহ্য unsigned long গুনতি_functors_bufif;
বাহ্য unsigned long গুনতি_functors_resolv;
বাহ্য unsigned long গুনতি_functors_sig;
বাহ্য unsigned long গুনতি_filters;
বাহ্য unsigned long গুনতি_vvp_nets;
বাহ্য unsigned long গুনতি_vpi_nets;
বাহ্য unsigned long গুনতি_vpi_scopes;

বাহ্য unsigned long গুনতি_net_arrays;
বাহ্য unsigned long গুনতি_net_array_শব্দগুলি;
বাহ্য unsigned long গুনতি_var_arrays;
বাহ্য unsigned long গুনতি_var_array_শব্দগুলি;
বাহ্য unsigned long গুনতি_real_arrays;
বাহ্য unsigned long গুনতি_real_array_শব্দগুলি;


বাহ্য unsigned long গুনতি_time_events;
বাহ্য unsigned long গুনতি_time_pool(শূন্য);

বাহ্য unsigned long গুনতি_assign_events;
বাহ্য unsigned long গুনতি_assign4_pool(শূন্য);
বাহ্য unsigned long গুনতি_assign8_pool(শূন্য);
বাহ্য unsigned long গুনতি_assign_real_pool(শূন্য);
বাহ্য unsigned long গুনতি_assign_aword_pool(শূন্য);
বাহ্য unsigned long গুনতি_assign_arword_pool(শূন্য);

বাহ্য unsigned long গুনতি_gen_events;
বাহ্য unsigned long গুনতি_gen_pool(শূন্য);

বাহ্য size_t size_opcodes;
বাহ্য size_t size_vvp_nets;
বাহ্য size_t size_vvp_net_funs;

#endif /* IVL_statistics_H */
