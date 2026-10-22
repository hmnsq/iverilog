#اگر_غیر_تعریف_شدہ IVL_statistics_H
#تعریف_کرو IVL_statistics_H
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
#اگر_تعریف_شدہ __SUNPRO_CC
# include  <stddef.h>
#ورنہ
# include  <cstddef>
#اگر_ختم

خارجی غیرعلامتی طویل گنتی_opcodes;
خارجی غیرعلامتی طویل گنتی_functors;
خارجی غیرعلامتی طویل گنتی_functors_logic;
خارجی غیرعلامتی طویل گنتی_functors_bufif;
خارجی غیرعلامتی طویل گنتی_functors_resolv;
خارجی غیرعلامتی طویل گنتی_functors_sig;
خارجی غیرعلامتی طویل گنتی_filters;
خارجی غیرعلامتی طویل گنتی_vvp_nets;
خارجی غیرعلامتی طویل گنتی_vpi_nets;
خارجی غیرعلامتی طویل گنتی_vpi_scopes;

خارجی غیرعلامتی طویل گنتی_net_arrays;
خارجی غیرعلامتی طویل گنتی_net_array_الفاظ;
خارجی غیرعلامتی طویل گنتی_var_arrays;
خارجی غیرعلامتی طویل گنتی_var_array_الفاظ;
خارجی غیرعلامتی طویل گنتی_real_arrays;
خارجی غیرعلامتی طویل گنتی_real_array_الفاظ;


خارجی غیرعلامتی طویل گنتی_وقت_events;
خارجی غیرعلامتی طویل گنتی_وقت_pool(خالی);

خارجی غیرعلامتی طویل گنتی_تفویض_events;
خارجی غیرعلامتی طویل گنتی_assign4_pool(خالی);
خارجی غیرعلامتی طویل گنتی_assign8_pool(خالی);
خارجی غیرعلامتی طویل گنتی_تفویض_real_pool(خالی);
خارجی غیرعلامتی طویل گنتی_تفویض_aword_pool(خالی);
خارجی غیرعلامتی طویل گنتی_تفویض_arword_pool(خالی);

خارجی غیرعلامتی طویل گنتی_gen_events;
خارجی غیرعلامتی طویل گنتی_gen_pool(خالی);

خارجی size_t سائز_opcodes;
خارجی size_t سائز_vvp_nets;
خارجی size_t سائز_vvp_net_funs;

#اگر_ختم /* IVL_statistics_H */
