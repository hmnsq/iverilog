#اگر_تعریف_نشده IVL_statistics_H
#تعریف_کن IVL_statistics_H
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
#اگر_تعریف_شده __SUNPRO_CC
# include  <stddef.h>
#وإلا
# include  <cstddef>
#پایان_اگر

خارجي بیعلامت دراز شمار_opcodes;
خارجي بیعلامت دراز شمار_functors;
خارجي بیعلامت دراز شمار_functors_logic;
خارجي بیعلامت دراز شمار_functors_bufif;
خارجي بیعلامت دراز شمار_functors_resolv;
خارجي بیعلامت دراز شمار_functors_sig;
خارجي بیعلامت دراز شمار_filters;
خارجي بیعلامت دراز شمار_vvp_nets;
خارجي بیعلامت دراز شمار_vpi_nets;
خارجي بیعلامت دراز شمار_vpi_scopes;

خارجي بیعلامت دراز شمار_net_arrays;
خارجي بیعلامت دراز شمار_net_array_واژگان;
خارجي بیعلامت دراز شمار_var_arrays;
خارجي بیعلامت دراز شمار_var_array_واژگان;
خارجي بیعلامت دراز شمار_real_arrays;
خارجي بیعلامت دراز شمار_real_array_واژگان;


خارجي بیعلامت دراز شمار_زمان_events;
خارجي بیعلامت دراز شمار_زمان_pool(فراغ);

خارجي بیعلامت دراز شمار_تخصیص_بده_events;
خارجي بیعلامت دراز شمار_assign4_pool(فراغ);
خارجي بیعلامت دراز شمار_assign8_pool(فراغ);
خارجي بیعلامت دراز شمار_تخصیص_بده_real_pool(فراغ);
خارجي بیعلامت دراز شمار_تخصیص_بده_aword_pool(فراغ);
خارجي بیعلامت دراز شمار_تخصیص_بده_arword_pool(فراغ);

خارجي بیعلامت دراز شمار_gen_events;
خارجي بیعلامت دراز شمار_gen_pool(فراغ);

خارجي size_t اندازه_opcodes;
خارجي size_t اندازه_vvp_nets;
خارجي size_t اندازه_vvp_net_funs;

#پایان_اگر /* IVL_statistics_H */
