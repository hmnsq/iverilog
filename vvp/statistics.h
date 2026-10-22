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
#નહીંતર
# include  <cstddef>
#endif

બાહ્ય unsigned long ગણતરી_opcodes;
બાહ્ય unsigned long ગણતરી_functors;
બાહ્ય unsigned long ગણતરી_functors_logic;
બાહ્ય unsigned long ગણતરી_functors_bufif;
બાહ્ય unsigned long ગણતરી_functors_resolv;
બાહ્ય unsigned long ગણતરી_functors_sig;
બાહ્ય unsigned long ગણતરી_filters;
બાહ્ય unsigned long ગણતરી_vvp_nets;
બાહ્ય unsigned long ગણતરી_vpi_nets;
બાહ્ય unsigned long ગણતરી_vpi_scopes;

બાહ્ય unsigned long ગણતરી_net_arrays;
બાહ્ય unsigned long ગણતરી_net_array_શબ્દો;
બાહ્ય unsigned long ગણતરી_var_arrays;
બાહ્ય unsigned long ગણતરી_var_array_શબ્દો;
બાહ્ય unsigned long ગણતરી_real_arrays;
બાહ્ય unsigned long ગણતરી_real_array_શબ્દો;


બાહ્ય unsigned long ગણતરી_time_events;
બાહ્ય unsigned long ગણતરી_time_pool(શૂન્ય);

બાહ્ય unsigned long ગણતરી_assign_events;
બાહ્ય unsigned long ગણતરી_assign4_pool(શૂન્ય);
બાહ્ય unsigned long ગણતરી_assign8_pool(શૂન્ય);
બાહ્ય unsigned long ગણતરી_assign_real_pool(શૂન્ય);
બાહ્ય unsigned long ગણતરી_assign_aword_pool(શૂન્ય);
બાહ્ય unsigned long ગણતરી_assign_arword_pool(શૂન્ય);

બાહ્ય unsigned long ગણતરી_gen_events;
બાહ્ય unsigned long ગણતરી_gen_pool(શૂન્ય);

બાહ્ય size_t size_opcodes;
બાહ્ય size_t size_vvp_nets;
બાહ્ય size_t size_vvp_net_funs;

#endif /* IVL_statistics_H */
