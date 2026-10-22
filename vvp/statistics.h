#నిర్వచితం_కాకపోతే IVL_statistics_H
#నిర్వచించు IVL_statistics_H
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
#నిర్వచితమైతే __SUNPRO_CC
# include  <stddef.h>
#లేకపోతే
# include  <cstddef>
#అయితే_ముగింపు

బాహ్య గుర్తులేని దీర్ఘ లెక్క_opcodes;
బాహ్య గుర్తులేని దీర్ఘ లెక్క_functors;
బాహ్య గుర్తులేని దీర్ఘ లెక్క_functors_logic;
బాహ్య గుర్తులేని దీర్ఘ లెక్క_functors_bufif;
బాహ్య గుర్తులేని దీర్ఘ లెక్క_functors_resolv;
బాహ్య గుర్తులేని దీర్ఘ లెక్క_functors_sig;
బాహ్య గుర్తులేని దీర్ఘ లెక్క_filters;
బాహ్య గుర్తులేని దీర్ఘ లెక్క_vvp_nets;
బాహ్య గుర్తులేని దీర్ఘ లెక్క_vpi_nets;
బాహ్య గుర్తులేని దీర్ఘ లెక్క_vpi_scopes;

బాహ్య గుర్తులేని దీర్ఘ లెక్క_net_arrays;
బాహ్య గుర్తులేని దీర్ఘ లెక్క_net_array_పదాలు;
బాహ్య గుర్తులేని దీర్ఘ లెక్క_var_arrays;
బాహ్య గుర్తులేని దీర్ఘ లెక్క_var_array_పదాలు;
బాహ్య గుర్తులేని దీర్ఘ లెక్క_real_arrays;
బాహ్య గుర్తులేని దీర్ఘ లెక్క_real_array_పదాలు;


బాహ్య గుర్తులేని దీర్ఘ లెక్క_సమయం_events;
బాహ్య గుర్తులేని దీర్ఘ లెక్క_సమయం_pool(శూన్య);

బాహ్య గుర్తులేని దీర్ఘ లెక్క_కేటాయించు_events;
బాహ్య గుర్తులేని దీర్ఘ లెక్క_assign4_pool(శూన్య);
బాహ్య గుర్తులేని దీర్ఘ లెక్క_assign8_pool(శూన్య);
బాహ్య గుర్తులేని దీర్ఘ లెక్క_కేటాయించు_real_pool(శూన్య);
బాహ్య గుర్తులేని దీర్ఘ లెక్క_కేటాయించు_aword_pool(శూన్య);
బాహ్య గుర్తులేని దీర్ఘ లెక్క_కేటాయించు_arword_pool(శూన్య);

బాహ్య గుర్తులేని దీర్ఘ లెక్క_gen_events;
బాహ్య గుర్తులేని దీర్ఘ లెక్క_gen_pool(శూన్య);

బాహ్య size_t కొలత_opcodes;
బాహ్య size_t కొలత_vvp_nets;
బాహ్య size_t కొలత_vvp_net_funs;

#అయితే_ముగింపు /* IVL_statistics_H */
