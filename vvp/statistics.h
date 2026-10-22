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
#ಅನ್ಯಥಾ
# include  <cstddef>
#endif

ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_opcodes;
ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_functors;
ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_functors_logic;
ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_functors_bufif;
ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_functors_resolv;
ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_functors_sig;
ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_filters;
ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_vvp_nets;
ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_vpi_nets;
ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_vpi_scopes;

ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_net_arrays;
ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_net_array_ಪದಗಳು;
ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_var_arrays;
ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_var_array_ಪದಗಳು;
ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_real_arrays;
ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_real_array_ಪದಗಳು;


ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_time_events;
ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_time_pool(ಶೂನ್ಯ);

ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_assign_events;
ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_assign4_pool(ಶೂನ್ಯ);
ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_assign8_pool(ಶೂನ್ಯ);
ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_assign_real_pool(ಶೂನ್ಯ);
ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_assign_aword_pool(ಶೂನ್ಯ);
ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_assign_arword_pool(ಶೂನ್ಯ);

ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_gen_events;
ಬಾಹ್ಯ unsigned long ಎಣಿಕೆ_gen_pool(ಶೂನ್ಯ);

ಬಾಹ್ಯ size_t size_opcodes;
ಬಾಹ್ಯ size_t size_vvp_nets;
ಬಾಹ್ಯ size_t size_vvp_net_funs;

#endif /* IVL_statistics_H */
