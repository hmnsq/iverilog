#جيڪڏهن_وصف_نه_ٿيل IVL_statistics_H
#وصف_ڪريو IVL_statistics_H
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
#جيڪڏهن_وصف_ٿيل __SUNPRO_CC
# include  <stddef.h>
#وإلا
# include  <cstddef>
#جيڪڏهن_پورو

خارجي بنانشان ڊگهو ڳڻپ_opcodes;
خارجي بنانشان ڊگهو ڳڻپ_functors;
خارجي بنانشان ڊگهو ڳڻپ_functors_logic;
خارجي بنانشان ڊگهو ڳڻپ_functors_bufif;
خارجي بنانشان ڊگهو ڳڻپ_functors_resolv;
خارجي بنانشان ڊگهو ڳڻپ_functors_sig;
خارجي بنانشان ڊگهو ڳڻپ_filters;
خارجي بنانشان ڊگهو ڳڻپ_vvp_nets;
خارجي بنانشان ڊگهو ڳڻپ_vpi_nets;
خارجي بنانشان ڊگهو ڳڻپ_vpi_scopes;

خارجي بنانشان ڊگهو ڳڻپ_net_arrays;
خارجي بنانشان ڊگهو ڳڻپ_net_array_لفظ;
خارجي بنانشان ڊگهو ڳڻپ_var_arrays;
خارجي بنانشان ڊگهو ڳڻپ_var_array_لفظ;
خارجي بنانشان ڊگهو ڳڻپ_real_arrays;
خارجي بنانشان ڊگهو ڳڻپ_real_array_لفظ;


خارجي بنانشان ڊگهو ڳڻپ_وقت_events;
خارجي بنانشان ڊگهو ڳڻپ_وقت_pool(فراغ);

خارجي بنانشان ڊگهو ڳڻپ_سونپيو_events;
خارجي بنانشان ڊگهو ڳڻپ_assign4_pool(فراغ);
خارجي بنانشان ڊگهو ڳڻپ_assign8_pool(فراغ);
خارجي بنانشان ڊگهو ڳڻپ_سونپيو_real_pool(فراغ);
خارجي بنانشان ڊگهو ڳڻپ_سونپيو_aword_pool(فراغ);
خارجي بنانشان ڊگهو ڳڻپ_سونپيو_arword_pool(فراغ);

خارجي بنانشان ڊگهو ڳڻپ_gen_events;
خارجي بنانشان ڊگهو ڳڻپ_gen_pool(فراغ);

خارجي size_t سائيز_opcodes;
خارجي size_t سائيز_vvp_nets;
خارجي size_t سائيز_vvp_net_funs;

#جيڪڏهن_پورو /* IVL_statistics_H */
