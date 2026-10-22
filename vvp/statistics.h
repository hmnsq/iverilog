#यदि_अपरिभाषितम् IVL_statistics_H
#परिभाषय IVL_statistics_H
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
#यदि_परिभाषितम् __SUNPRO_CC
# include  <stddef.h>
#अन्यथा
# include  <cstddef>
#यदि_अन्तः

बाह्य अचिह्नित दीर्घ गणनम्_opcodes;
बाह्य अचिह्नित दीर्घ गणनम्_functors;
बाह्य अचिह्नित दीर्घ गणनम्_functors_logic;
बाह्य अचिह्नित दीर्घ गणनम्_functors_bufif;
बाह्य अचिह्नित दीर्घ गणनम्_functors_resolv;
बाह्य अचिह्नित दीर्घ गणनम्_functors_sig;
बाह्य अचिह्नित दीर्घ गणनम्_filters;
बाह्य अचिह्नित दीर्घ गणनम्_vvp_nets;
बाह्य अचिह्नित दीर्घ गणनम्_vpi_nets;
बाह्य अचिह्नित दीर्घ गणनम्_vpi_scopes;

बाह्य अचिह्नित दीर्घ गणनम्_net_arrays;
बाह्य अचिह्नित दीर्घ गणनम्_net_array_शब्दाः;
बाह्य अचिह्नित दीर्घ गणनम्_var_arrays;
बाह्य अचिह्नित दीर्घ गणनम्_var_array_शब्दाः;
बाह्य अचिह्नित दीर्घ गणनम्_real_arrays;
बाह्य अचिह्नित दीर्घ गणनम्_real_array_शब्दाः;


बाह्य अचिह्नित दीर्घ गणनम्_time_events;
बाह्य अचिह्नित दीर्घ गणनम्_time_pool(शून्य);

बाह्य अचिह्नित दीर्घ गणनम्_नियोजय_events;
बाह्य अचिह्नित दीर्घ गणनम्_assign4_pool(शून्य);
बाह्य अचिह्नित दीर्घ गणनम्_assign8_pool(शून्य);
बाह्य अचिह्नित दीर्घ गणनम्_नियोजय_real_pool(शून्य);
बाह्य अचिह्नित दीर्घ गणनम्_नियोजय_aword_pool(शून्य);
बाह्य अचिह्नित दीर्घ गणनम्_नियोजय_arword_pool(शून्य);

बाह्य अचिह्नित दीर्घ गणनम्_gen_events;
बाह्य अचिह्नित दीर्घ गणनम्_gen_pool(शून्य);

बाह्य size_t आकार_opcodes;
बाह्य size_t आकार_vvp_nets;
बाह्य size_t आकार_vvp_net_funs;

#यदि_अन्तः /* IVL_statistics_H */
