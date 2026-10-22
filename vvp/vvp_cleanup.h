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

ബാഹ്യ ശൂന്യം codespace_delete(ശൂന്യം);
ബാഹ്യ ശൂന്യം dec_str_delete(ശൂന്യം);
ബാഹ്യ ശൂന്യം def_table_delete(ശൂന്യം);
ബാഹ്യ ശൂന്യം island_delete(ശൂന്യം);
ബാഹ്യ ശൂന്യം vpi_mcd_delete(ശൂന്യം);
ബാഹ്യ ശൂന്യം load_module_delete(ശൂന്യം);
ബാഹ്യ ശൂന്യം modpath_delete(ശൂന്യം);
ബാഹ്യ ശൂന്യം വേര്_table_delete(ശൂന്യം);
ബാഹ്യ ശൂന്യം schedule_delete(ശൂന്യം);
ബാഹ്യ ശൂന്യം signal_pool_delete(ശൂന്യം);
ബാഹ്യ ശൂന്യം simulator_cb_delete(ശൂന്യം);
ബാഹ്യ ശൂന്യം udp_defns_delete(ശൂന്യം);
ബാഹ്യ ശൂന്യം vpi_handle_delete(ശൂന്യം);
ബാഹ്യ ശൂന്യം vpi_അട്ടി_delete(ശൂന്യം);
ബാഹ്യ ശൂന്യം vvp_net_pool_delete(ശൂന്യം);
ബാഹ്യ ശൂന്യം ufunc_pool_delete(ശൂന്യം);

ബാഹ്യ ശൂന്യം A_delete(class __vpiHandle *item);
ബാഹ്യ ശൂന്യം APV_delete(class __vpiHandle *item);
ബാഹ്യ ശൂന്യം PV_delete(class __vpiHandle *item);
ബാഹ്യ ശൂന്യം class_delete(class __vpiHandle *item);
ബാഹ്യ ശൂന്യം class_def_delete(class_type *item);
ബാഹ്യ ശൂന്യം constant_delete(class __vpiHandle *item);
ബാഹ്യ ശൂന്യം contexts_delete(__vpiScope *scope);
ബാഹ്യ ശൂന്യം array_delete(class __vpiHandle *item);
ബാഹ്യ ശൂന്യം enum_delete(class __vpiHandle *item);
ബാഹ്യ ശൂന്യം memory_delete(class __vpiHandle *item);
ബാഹ്യ ശൂന്യം named_event_delete(class __vpiHandle *item);
ബാഹ്യ ശൂന്യം parameter_delete(class __vpiHandle *item);
ബാഹ്യ ശൂന്യം signal_delete(class __vpiHandle *item);
ബാഹ്യ ശൂന്യം real_delete(class __vpiHandle *item);
ബാഹ്യ ശൂന്യം string_delete(class __vpiHandle *item);
ബാഹ്യ ശൂന്യം thread_string_delete(class __vpiHandle *item);
ബാഹ്യ ശൂന്യം thread_vthr_delete(class __vpiHandle *item);
ബാഹ്യ ശൂന്യം thread_word_delete(class __vpiHandle *item);
ബാഹ്യ ശൂന്യം vpi_call_delete(class __vpiHandle *item);
ബാഹ്യ ശൂന്യം exec_ufunc_delete(vvp_code_t euf_code);
ബാഹ്യ ശൂന്യം vthreads_delete(__vpiScope*scope);
ബാഹ്യ ശൂന്യം vvp_net_delete(vvp_net_t *item);


ബാഹ്യ ശൂന്യം pool_local_net(vvp_net_t *net);

#endif /* IVL_vvp_cleanup_H */
