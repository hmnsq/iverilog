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

بیرونی تهی codespace_delete(تهی);
بیرونی تهی dec_str_delete(تهی);
بیرونی تهی def_table_delete(تهی);
بیرونی تهی island_delete(تهی);
بیرونی تهی vpi_mcd_delete(تهی);
بیرونی تهی load_module_delete(تهی);
بیرونی تهی modpath_delete(تهی);
بیرونی تهی ریشه_table_delete(تهی);
بیرونی تهی schedule_delete(تهی);
بیرونی تهی signal_pool_delete(تهی);
بیرونی تهی simulator_cb_delete(تهی);
بیرونی تهی udp_defns_delete(تهی);
بیرونی تهی vpi_handle_delete(تهی);
بیرونی تهی vpi_پشته_delete(تهی);
بیرونی تهی vvp_net_pool_delete(تهی);
بیرونی تهی ufunc_pool_delete(تهی);

بیرونی تهی A_delete(class __vpiHandle *item);
بیرونی تهی APV_delete(class __vpiHandle *item);
بیرونی تهی PV_delete(class __vpiHandle *item);
بیرونی تهی class_delete(class __vpiHandle *item);
بیرونی تهی class_def_delete(class_type *item);
بیرونی تهی constant_delete(class __vpiHandle *item);
بیرونی تهی contexts_delete(__vpiScope *scope);
بیرونی تهی array_delete(class __vpiHandle *item);
بیرونی تهی enum_delete(class __vpiHandle *item);
بیرونی تهی memory_delete(class __vpiHandle *item);
بیرونی تهی named_event_delete(class __vpiHandle *item);
بیرونی تهی parameter_delete(class __vpiHandle *item);
بیرونی تهی signal_delete(class __vpiHandle *item);
بیرونی تهی real_delete(class __vpiHandle *item);
بیرونی تهی string_delete(class __vpiHandle *item);
بیرونی تهی thread_string_delete(class __vpiHandle *item);
بیرونی تهی thread_vthr_delete(class __vpiHandle *item);
بیرونی تهی thread_word_delete(class __vpiHandle *item);
بیرونی تهی vpi_call_delete(class __vpiHandle *item);
بیرونی تهی exec_ufunc_delete(vvp_code_t euf_code);
بیرونی تهی vthreads_delete(__vpiScope*scope);
بیرونی تهی vvp_net_delete(vvp_net_t *item);


بیرونی تهی pool_local_net(vvp_net_t *net);

#endif /* IVL_vvp_cleanup_H */
