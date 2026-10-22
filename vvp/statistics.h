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
#ଅନ୍ୟଥା
# include  <cstddef>
#endif

ବାହ୍ୟ unsigned long ଗଣନା_opcodes;
ବାହ୍ୟ unsigned long ଗଣନା_functors;
ବାହ୍ୟ unsigned long ଗଣନା_functors_logic;
ବାହ୍ୟ unsigned long ଗଣନା_functors_bufif;
ବାହ୍ୟ unsigned long ଗଣନା_functors_resolv;
ବାହ୍ୟ unsigned long ଗଣନା_functors_sig;
ବାହ୍ୟ unsigned long ଗଣନା_filters;
ବାହ୍ୟ unsigned long ଗଣନା_vvp_nets;
ବାହ୍ୟ unsigned long ଗଣନା_vpi_nets;
ବାହ୍ୟ unsigned long ଗଣନା_vpi_scopes;

ବାହ୍ୟ unsigned long ଗଣନା_net_arrays;
ବାହ୍ୟ unsigned long ଗଣନା_net_array_ଶବ୍ଦଗୁଡ଼ିକ;
ବାହ୍ୟ unsigned long ଗଣନା_var_arrays;
ବାହ୍ୟ unsigned long ଗଣନା_var_array_ଶବ୍ଦଗୁଡ଼ିକ;
ବାହ୍ୟ unsigned long ଗଣନା_real_arrays;
ବାହ୍ୟ unsigned long ଗଣନା_real_array_ଶବ୍ଦଗୁଡ଼ିକ;


ବାହ୍ୟ unsigned long ଗଣନା_time_events;
ବାହ୍ୟ unsigned long ଗଣନା_time_pool(ଶୂନ୍ୟ);

ବାହ୍ୟ unsigned long ଗଣନା_assign_events;
ବାହ୍ୟ unsigned long ଗଣନା_assign4_pool(ଶୂନ୍ୟ);
ବାହ୍ୟ unsigned long ଗଣନା_assign8_pool(ଶୂନ୍ୟ);
ବାହ୍ୟ unsigned long ଗଣନା_assign_real_pool(ଶୂନ୍ୟ);
ବାହ୍ୟ unsigned long ଗଣନା_assign_aword_pool(ଶୂନ୍ୟ);
ବାହ୍ୟ unsigned long ଗଣନା_assign_arword_pool(ଶୂନ୍ୟ);

ବାହ୍ୟ unsigned long ଗଣନା_gen_events;
ବାହ୍ୟ unsigned long ଗଣନା_gen_pool(ଶୂନ୍ୟ);

ବାହ୍ୟ size_t size_opcodes;
ବାହ୍ୟ size_t size_vvp_nets;
ବାହ୍ୟ size_t size_vvp_net_funs;

#endif /* IVL_statistics_H */
