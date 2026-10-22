#جيڪڏهن_وصف_نه_ٿيل IVL_vvp_cleanup_H
#وصف_ڪريو IVL_vvp_cleanup_H
/*
 * Copyright (c) 2009-2026 Cary R. (cygcary@yahoo.com)
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

# include "config.h"
# include "vpi_priv.h"
# include "vvp_net.h"

/* Routines used to cleanup the runtime memory when it is all finished. */

خارجي فراغ codespace_delete(فراغ);
خارجي فراغ dec_str_delete(فراغ);
خارجي فراغ def_جدول_delete(فراغ);
خارجي فراغ island_delete(فراغ);
خارجي فراغ vpi_mcd_delete(فراغ);
خارجي فراغ ڀريو_module_delete(فراغ);
خارجي فراغ modpath_delete(فراغ);
خارجي فراغ پاڙ_جدول_delete(فراغ);
خارجي فراغ وقت_جدول_delete(فراغ);
خارجي فراغ signal_pool_delete(فراغ);
خارجي فراغ simulator_cb_delete(فراغ);
خارجي فراغ udp_defns_delete(فراغ);
خارجي فراغ vpi_handle_delete(فراغ);
خارجي فراغ vpi_ڍير_delete(فراغ);
خارجي فراغ vvp_net_pool_delete(فراغ);
خارجي فراغ ufunc_pool_delete(فراغ);

خارجي فراغ A_delete(class __vpiHandle *شي);
خارجي فراغ APV_delete(class __vpiHandle *شي);
خارجي فراغ PV_delete(class __vpiHandle *شي);
خارجي فراغ class_delete(class __vpiHandle *شي);
خارجي فراغ class_def_delete(class_type *شي);
خارجي فراغ constant_delete(class __vpiHandle *شي);
خارجي فراغ contexts_delete(__vpiScope *scope);
خارجي فراغ array_delete(class __vpiHandle *شي);
خارجي فراغ enum_delete(class __vpiHandle *شي);
خارجي فراغ يادگيري_delete(class __vpiHandle *شي);
خارجي فراغ named_event_delete(class __vpiHandle *شي);
خارجي فراغ parameter_delete(class __vpiHandle *شي);
خارجي فراغ signal_delete(class __vpiHandle *شي);
خارجي فراغ real_delete(class __vpiHandle *شي);
خارجي فراغ string_delete(class __vpiHandle *شي);
خارجي فراغ thread_string_delete(class __vpiHandle *شي);
خارجي فراغ thread_vthr_delete(class __vpiHandle *شي);
خارجي فراغ thread_لفظ_delete(class __vpiHandle *شي);
خارجي فراغ vpi_call_delete(class __vpiHandle *شي);
خارجي فراغ exec_ufunc_delete(vvp_ڪوڊ_t euf_ڪوڊ);
خارجي فراغ vthreads_delete(__vpiScope*scope);
خارجي فراغ vvp_net_delete(vvp_net_t *شي);


خارجي فراغ pool_local_net(vvp_net_t *net);

#جيڪڏهن_پورو /* IVL_vvp_cleanup_H */
