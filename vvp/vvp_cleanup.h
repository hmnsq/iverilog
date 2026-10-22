#اگر_غیر_تعریف_شدہ IVL_vvp_cleanup_H
#تعریف_کرو IVL_vvp_cleanup_H
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

خارجی خالی codespace_delete(خالی);
خارجی خالی dec_str_delete(خالی);
خارجی خالی def_جدول_delete(خالی);
خارجی خالی island_delete(خالی);
خارجی خالی vpi_mcd_delete(خالی);
خارجی خالی بھرو_module_delete(خالی);
خارجی خالی modpath_delete(خالی);
خارجی خالی جڑ_جدول_delete(خالی);
خارجی خالی جدول_وقت_delete(خالی);
خارجی خالی signal_pool_delete(خالی);
خارجی خالی simulator_cb_delete(خالی);
خارجی خالی udp_defns_delete(خالی);
خارجی خالی vpi_handle_delete(خالی);
خارجی خالی vpi_ڈھیر_delete(خالی);
خارجی خالی vvp_net_pool_delete(خالی);
خارجی خالی ufunc_pool_delete(خالی);

خارجی خالی A_delete(class __vpiHandle *شے);
خارجی خالی APV_delete(class __vpiHandle *شے);
خارجی خالی PV_delete(class __vpiHandle *شے);
خارجی خالی class_delete(class __vpiHandle *شے);
خارجی خالی class_def_delete(class_type *شے);
خارجی خالی constant_delete(class __vpiHandle *شے);
خارجی خالی contexts_delete(__vpiScope *scope);
خارجی خالی array_delete(class __vpiHandle *شے);
خارجی خالی enum_delete(class __vpiHandle *شے);
خارجی خالی حافظہ_delete(class __vpiHandle *شے);
خارجی خالی named_event_delete(class __vpiHandle *شے);
خارجی خالی parameter_delete(class __vpiHandle *شے);
خارجی خالی signal_delete(class __vpiHandle *شے);
خارجی خالی real_delete(class __vpiHandle *شے);
خارجی خالی string_delete(class __vpiHandle *شے);
خارجی خالی thread_string_delete(class __vpiHandle *شے);
خارجی خالی thread_vthr_delete(class __vpiHandle *شے);
خارجی خالی thread_لفظ_delete(class __vpiHandle *شے);
خارجی خالی vpi_call_delete(class __vpiHandle *شے);
خارجی خالی exec_ufunc_delete(vvp_رمز_t euf_رمز);
خارجی خالی vthreads_delete(__vpiScope*scope);
خارجی خالی vvp_net_delete(vvp_net_t *شے);


خارجی خالی pool_local_net(vvp_net_t *net);

#اگر_ختم /* IVL_vvp_cleanup_H */
