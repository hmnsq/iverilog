#اگر_تعریف_نشده IVL_vvp_cleanup_H
#تعریف_کن IVL_vvp_cleanup_H
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
خارجي فراغ بارگیری_module_delete(فراغ);
خارجي فراغ modpath_delete(فراغ);
خارجي فراغ ریشه_جدول_delete(فراغ);
خارجي فراغ زمانبندی_delete(فراغ);
خارجي فراغ signal_pool_delete(فراغ);
خارجي فراغ simulator_cb_delete(فراغ);
خارجي فراغ udp_defns_delete(فراغ);
خارجي فراغ vpi_handle_delete(فراغ);
خارجي فراغ vpi_پشته_delete(فراغ);
خارجي فراغ vvp_net_pool_delete(فراغ);
خارجي فراغ ufunc_pool_delete(فراغ);

خارجي فراغ A_delete(class __vpiHandle *قلم);
خارجي فراغ APV_delete(class __vpiHandle *قلم);
خارجي فراغ PV_delete(class __vpiHandle *قلم);
خارجي فراغ class_delete(class __vpiHandle *قلم);
خارجي فراغ class_def_delete(class_type *قلم);
خارجي فراغ constant_delete(class __vpiHandle *قلم);
خارجي فراغ contexts_delete(__vpiScope *scope);
خارجي فراغ array_delete(class __vpiHandle *قلم);
خارجي فراغ enum_delete(class __vpiHandle *قلم);
خارجي فراغ حافظه_delete(class __vpiHandle *قلم);
خارجي فراغ named_event_delete(class __vpiHandle *قلم);
خارجي فراغ parameter_delete(class __vpiHandle *قلم);
خارجي فراغ signal_delete(class __vpiHandle *قلم);
خارجي فراغ real_delete(class __vpiHandle *قلم);
خارجي فراغ string_delete(class __vpiHandle *قلم);
خارجي فراغ thread_string_delete(class __vpiHandle *قلم);
خارجي فراغ thread_vthr_delete(class __vpiHandle *قلم);
خارجي فراغ thread_کلمه_delete(class __vpiHandle *قلم);
خارجي فراغ vpi_call_delete(class __vpiHandle *قلم);
خارجي فراغ exec_ufunc_delete(vvp_کد_t euf_کد);
خارجي فراغ vthreads_delete(__vpiScope*scope);
خارجي فراغ vvp_net_delete(vvp_net_t *قلم);


خارجي فراغ pool_local_net(vvp_net_t *net);

#پایان_اگر /* IVL_vvp_cleanup_H */
