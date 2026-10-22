#ifndef IVL_vvp_cleanup_H
#define IVL_vvp_cleanup_H
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

வெளி வெறுமை codespace_delete(வெறுமை);
வெளி வெறுமை dec_str_delete(வெறுமை);
வெளி வெறுமை def_table_delete(வெறுமை);
வெளி வெறுமை island_delete(வெறுமை);
வெளி வெறுமை vpi_mcd_delete(வெறுமை);
வெளி வெறுமை load_module_delete(வெறுமை);
வெளி வெறுமை modpath_delete(வெறுமை);
வெளி வெறுமை வேர்_table_delete(வெறுமை);
வெளி வெறுமை schedule_delete(வெறுமை);
வெளி வெறுமை signal_pool_delete(வெறுமை);
வெளி வெறுமை simulator_cb_delete(வெறுமை);
வெளி வெறுமை udp_defns_delete(வெறுமை);
வெளி வெறுமை vpi_handle_delete(வெறுமை);
வெளி வெறுமை vpi_அடுக்கு_delete(வெறுமை);
வெளி வெறுமை vvp_net_pool_delete(வெறுமை);
வெளி வெறுமை ufunc_pool_delete(வெறுமை);

வெளி வெறுமை A_delete(class __vpiHandle *item);
வெளி வெறுமை APV_delete(class __vpiHandle *item);
வெளி வெறுமை PV_delete(class __vpiHandle *item);
வெளி வெறுமை class_delete(class __vpiHandle *item);
வெளி வெறுமை class_def_delete(class_type *item);
வெளி வெறுமை constant_delete(class __vpiHandle *item);
வெளி வெறுமை contexts_delete(__vpiScope *scope);
வெளி வெறுமை array_delete(class __vpiHandle *item);
வெளி வெறுமை enum_delete(class __vpiHandle *item);
வெளி வெறுமை memory_delete(class __vpiHandle *item);
வெளி வெறுமை named_event_delete(class __vpiHandle *item);
வெளி வெறுமை parameter_delete(class __vpiHandle *item);
வெளி வெறுமை signal_delete(class __vpiHandle *item);
வெளி வெறுமை real_delete(class __vpiHandle *item);
வெளி வெறுமை string_delete(class __vpiHandle *item);
வெளி வெறுமை thread_string_delete(class __vpiHandle *item);
வெளி வெறுமை thread_vthr_delete(class __vpiHandle *item);
வெளி வெறுமை thread_word_delete(class __vpiHandle *item);
வெளி வெறுமை vpi_call_delete(class __vpiHandle *item);
வெளி வெறுமை exec_ufunc_delete(vvp_code_t euf_code);
வெளி வெறுமை vthreads_delete(__vpiScope*scope);
வெளி வெறுமை vvp_net_delete(vvp_net_t *item);


வெளி வெறுமை pool_local_net(vvp_net_t *net);

#endif /* IVL_vvp_cleanup_H */
