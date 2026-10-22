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
#अन्यथा
# include  <cstddef>
#endif

बाह्य अचिन्हित दीर्घ मोजणी_opcodes;
बाह्य अचिन्हित दीर्घ मोजणी_functors;
बाह्य अचिन्हित दीर्घ मोजणी_functors_logic;
बाह्य अचिन्हित दीर्घ मोजणी_functors_bufif;
बाह्य अचिन्हित दीर्घ मोजणी_functors_resolv;
बाह्य अचिन्हित दीर्घ मोजणी_functors_sig;
बाह्य अचिन्हित दीर्घ मोजणी_filters;
बाह्य अचिन्हित दीर्घ मोजणी_vvp_nets;
बाह्य अचिन्हित दीर्घ मोजणी_vpi_nets;
बाह्य अचिन्हित दीर्घ मोजणी_vpi_scopes;

बाह्य अचिन्हित दीर्घ मोजणी_net_arrays;
बाह्य अचिन्हित दीर्घ मोजणी_net_array_शब्द;
बाह्य अचिन्हित दीर्घ मोजणी_var_arrays;
बाह्य अचिन्हित दीर्घ मोजणी_var_array_शब्द;
बाह्य अचिन्हित दीर्घ मोजणी_real_arrays;
बाह्य अचिन्हित दीर्घ मोजणी_real_array_शब्द;


बाह्य अचिन्हित दीर्घ मोजणी_time_events;
बाह्य अचिन्हित दीर्घ मोजणी_time_pool(शून्य);

बाह्य अचिन्हित दीर्घ मोजणी_assign_events;
बाह्य अचिन्हित दीर्घ मोजणी_assign4_pool(शून्य);
बाह्य अचिन्हित दीर्घ मोजणी_assign8_pool(शून्य);
बाह्य अचिन्हित दीर्घ मोजणी_assign_real_pool(शून्य);
बाह्य अचिन्हित दीर्घ मोजणी_assign_aword_pool(शून्य);
बाह्य अचिन्हित दीर्घ मोजणी_assign_arword_pool(शून्य);

बाह्य अचिन्हित दीर्घ मोजणी_gen_events;
बाह्य अचिन्हित दीर्घ मोजणी_gen_pool(शून्य);

बाह्य माप_प्रकार size_opcodes;
बाह्य माप_प्रकार size_vvp_nets;
बाह्य माप_प्रकार size_vvp_net_funs;

#endif /* IVL_statistics_H */
