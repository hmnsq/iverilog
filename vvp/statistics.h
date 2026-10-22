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
#അല്ലെങ്കിൽ
# include  <cstddef>
#endif

ബാഹ്യ unsigned long എണ്ണം_opcodes;
ബാഹ്യ unsigned long എണ്ണം_functors;
ബാഹ്യ unsigned long എണ്ണം_functors_logic;
ബാഹ്യ unsigned long എണ്ണം_functors_bufif;
ബാഹ്യ unsigned long എണ്ണം_functors_resolv;
ബാഹ്യ unsigned long എണ്ണം_functors_sig;
ബാഹ്യ unsigned long എണ്ണം_filters;
ബാഹ്യ unsigned long എണ്ണം_vvp_nets;
ബാഹ്യ unsigned long എണ്ണം_vpi_nets;
ബാഹ്യ unsigned long എണ്ണം_vpi_scopes;

ബാഹ്യ unsigned long എണ്ണം_net_arrays;
ബാഹ്യ unsigned long എണ്ണം_net_array_വാക്കുകൾ;
ബാഹ്യ unsigned long എണ്ണം_var_arrays;
ബാഹ്യ unsigned long എണ്ണം_var_array_വാക്കുകൾ;
ബാഹ്യ unsigned long എണ്ണം_real_arrays;
ബാഹ്യ unsigned long എണ്ണം_real_array_വാക്കുകൾ;


ബാഹ്യ unsigned long എണ്ണം_time_events;
ബാഹ്യ unsigned long എണ്ണം_time_pool(ശൂന്യം);

ബാഹ്യ unsigned long എണ്ണം_assign_events;
ബാഹ്യ unsigned long എണ്ണം_assign4_pool(ശൂന്യം);
ബാഹ്യ unsigned long എണ്ണം_assign8_pool(ശൂന്യം);
ബാഹ്യ unsigned long എണ്ണം_assign_real_pool(ശൂന്യം);
ബാഹ്യ unsigned long എണ്ണം_assign_aword_pool(ശൂന്യം);
ബാഹ്യ unsigned long എണ്ണം_assign_arword_pool(ശൂന്യം);

ബാഹ്യ unsigned long എണ്ണം_gen_events;
ബാഹ്യ unsigned long എണ്ണം_gen_pool(ശൂന്യം);

ബാഹ്യ size_t size_opcodes;
ബാഹ്യ size_t size_vvp_nets;
ബാഹ്യ size_t size_vvp_net_funs;

#endif /* IVL_statistics_H */
