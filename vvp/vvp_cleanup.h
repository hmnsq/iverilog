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

ବାହ୍ୟ ଶୂନ୍ୟ codespace_delete(ଶୂନ୍ୟ);
ବାହ୍ୟ ଶୂନ୍ୟ dec_str_delete(ଶୂନ୍ୟ);
ବାହ୍ୟ ଶୂନ୍ୟ def_table_delete(ଶୂନ୍ୟ);
ବାହ୍ୟ ଶୂନ୍ୟ island_delete(ଶୂନ୍ୟ);
ବାହ୍ୟ ଶୂନ୍ୟ vpi_mcd_delete(ଶୂନ୍ୟ);
ବାହ୍ୟ ଶୂନ୍ୟ load_module_delete(ଶୂନ୍ୟ);
ବାହ୍ୟ ଶୂନ୍ୟ modpath_delete(ଶୂନ୍ୟ);
ବାହ୍ୟ ଶୂନ୍ୟ ମୂଳ_table_delete(ଶୂନ୍ୟ);
ବାହ୍ୟ ଶୂନ୍ୟ schedule_delete(ଶୂନ୍ୟ);
ବାହ୍ୟ ଶୂନ୍ୟ signal_pool_delete(ଶୂନ୍ୟ);
ବାହ୍ୟ ଶୂନ୍ୟ simulator_cb_delete(ଶୂନ୍ୟ);
ବାହ୍ୟ ଶୂନ୍ୟ udp_defns_delete(ଶୂନ୍ୟ);
ବାହ୍ୟ ଶୂନ୍ୟ vpi_handle_delete(ଶୂନ୍ୟ);
ବାହ୍ୟ ଶୂନ୍ୟ vpi_ଗଦା_delete(ଶୂନ୍ୟ);
ବାହ୍ୟ ଶୂନ୍ୟ vvp_net_pool_delete(ଶୂନ୍ୟ);
ବାହ୍ୟ ଶୂନ୍ୟ ufunc_pool_delete(ଶୂନ୍ୟ);

ବାହ୍ୟ ଶୂନ୍ୟ A_delete(class __vpiHandle *item);
ବାହ୍ୟ ଶୂନ୍ୟ APV_delete(class __vpiHandle *item);
ବାହ୍ୟ ଶୂନ୍ୟ PV_delete(class __vpiHandle *item);
ବାହ୍ୟ ଶୂନ୍ୟ class_delete(class __vpiHandle *item);
ବାହ୍ୟ ଶୂନ୍ୟ class_def_delete(class_type *item);
ବାହ୍ୟ ଶୂନ୍ୟ constant_delete(class __vpiHandle *item);
ବାହ୍ୟ ଶୂନ୍ୟ contexts_delete(__vpiScope *scope);
ବାହ୍ୟ ଶୂନ୍ୟ array_delete(class __vpiHandle *item);
ବାହ୍ୟ ଶୂନ୍ୟ enum_delete(class __vpiHandle *item);
ବାହ୍ୟ ଶୂନ୍ୟ memory_delete(class __vpiHandle *item);
ବାହ୍ୟ ଶୂନ୍ୟ named_event_delete(class __vpiHandle *item);
ବାହ୍ୟ ଶୂନ୍ୟ parameter_delete(class __vpiHandle *item);
ବାହ୍ୟ ଶୂନ୍ୟ signal_delete(class __vpiHandle *item);
ବାହ୍ୟ ଶୂନ୍ୟ real_delete(class __vpiHandle *item);
ବାହ୍ୟ ଶୂନ୍ୟ string_delete(class __vpiHandle *item);
ବାହ୍ୟ ଶୂନ୍ୟ thread_string_delete(class __vpiHandle *item);
ବାହ୍ୟ ଶୂନ୍ୟ thread_vthr_delete(class __vpiHandle *item);
ବାହ୍ୟ ଶୂନ୍ୟ thread_word_delete(class __vpiHandle *item);
ବାହ୍ୟ ଶୂନ୍ୟ vpi_call_delete(class __vpiHandle *item);
ବାହ୍ୟ ଶୂନ୍ୟ exec_ufunc_delete(vvp_code_t euf_code);
ବାହ୍ୟ ଶୂନ୍ୟ vthreads_delete(__vpiScope*scope);
ବାହ୍ୟ ଶୂନ୍ୟ vvp_net_delete(vvp_net_t *item);


ବାହ୍ୟ ଶୂନ୍ୟ pool_local_net(vvp_net_t *net);

#endif /* IVL_vvp_cleanup_H */
