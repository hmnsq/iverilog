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

বাহ্য শূন্য codespace_delete(শূন্য);
বাহ্য শূন্য dec_str_delete(শূন্য);
বাহ্য শূন্য def_table_delete(শূন্য);
বাহ্য শূন্য island_delete(শূন্য);
বাহ্য শূন্য vpi_mcd_delete(শূন্য);
বাহ্য শূন্য load_module_delete(শূন্য);
বাহ্য শূন্য modpath_delete(শূন্য);
বাহ্য শূন্য মূল_table_delete(শূন্য);
বাহ্য শূন্য schedule_delete(শূন্য);
বাহ্য শূন্য signal_pool_delete(শূন্য);
বাহ্য শূন্য simulator_cb_delete(শূন্য);
বাহ্য শূন্য udp_defns_delete(শূন্য);
বাহ্য শূন্য vpi_handle_delete(শূন্য);
বাহ্য শূন্য vpi_স্তূপ_delete(শূন্য);
বাহ্য শূন্য vvp_net_pool_delete(শূন্য);
বাহ্য শূন্য ufunc_pool_delete(শূন্য);

বাহ্য শূন্য A_delete(class __vpiHandle *item);
বাহ্য শূন্য APV_delete(class __vpiHandle *item);
বাহ্য শূন্য PV_delete(class __vpiHandle *item);
বাহ্য শূন্য class_delete(class __vpiHandle *item);
বাহ্য শূন্য class_def_delete(class_type *item);
বাহ্য শূন্য constant_delete(class __vpiHandle *item);
বাহ্য শূন্য contexts_delete(__vpiScope *scope);
বাহ্য শূন্য array_delete(class __vpiHandle *item);
বাহ্য শূন্য enum_delete(class __vpiHandle *item);
বাহ্য শূন্য memory_delete(class __vpiHandle *item);
বাহ্য শূন্য named_event_delete(class __vpiHandle *item);
বাহ্য শূন্য parameter_delete(class __vpiHandle *item);
বাহ্য শূন্য signal_delete(class __vpiHandle *item);
বাহ্য শূন্য real_delete(class __vpiHandle *item);
বাহ্য শূন্য string_delete(class __vpiHandle *item);
বাহ্য শূন্য thread_string_delete(class __vpiHandle *item);
বাহ্য শূন্য thread_vthr_delete(class __vpiHandle *item);
বাহ্য শূন্য thread_word_delete(class __vpiHandle *item);
বাহ্য শূন্য vpi_call_delete(class __vpiHandle *item);
বাহ্য শূন্য exec_ufunc_delete(vvp_code_t euf_code);
বাহ্য শূন্য vthreads_delete(__vpiScope*scope);
বাহ্য শূন্য vvp_net_delete(vvp_net_t *item);


বাহ্য শূন্য pool_local_net(vvp_net_t *net);

#endif /* IVL_vvp_cleanup_H */
