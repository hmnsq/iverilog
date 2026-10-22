#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_vvp_cleanup_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_vvp_cleanup_H
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

ਬਾਹਰੀ ਖਾਲੀ codespace_delete(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ dec_str_delete(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ def_ਸਾਰਣੀ_delete(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ island_delete(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ vpi_mcd_delete(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ ਲੱਦੋ_module_delete(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ modpath_delete(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ ਜੜ੍ਹ_ਸਾਰਣੀ_delete(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_delete(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ signal_pool_delete(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ simulator_cb_delete(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ udp_defns_delete(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ vpi_handle_delete(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ vpi_ਢੇਰ_delete(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ vvp_net_pool_delete(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ ufunc_pool_delete(ਖਾਲੀ);

ਬਾਹਰੀ ਖਾਲੀ A_delete(class __vpiHandle *ਵਸਤੂ);
ਬਾਹਰੀ ਖਾਲੀ APV_delete(class __vpiHandle *ਵਸਤੂ);
ਬਾਹਰੀ ਖਾਲੀ PV_delete(class __vpiHandle *ਵਸਤੂ);
ਬਾਹਰੀ ਖਾਲੀ class_delete(class __vpiHandle *ਵਸਤੂ);
ਬਾਹਰੀ ਖਾਲੀ class_def_delete(class_type *ਵਸਤੂ);
ਬਾਹਰੀ ਖਾਲੀ constant_delete(class __vpiHandle *ਵਸਤੂ);
ਬਾਹਰੀ ਖਾਲੀ contexts_delete(__vpiScope *scope);
ਬਾਹਰੀ ਖਾਲੀ array_delete(class __vpiHandle *ਵਸਤੂ);
ਬਾਹਰੀ ਖਾਲੀ enum_delete(class __vpiHandle *ਵਸਤੂ);
ਬਾਹਰੀ ਖਾਲੀ ਸਮ੍ਰਿਤੀ_delete(class __vpiHandle *ਵਸਤੂ);
ਬਾਹਰੀ ਖਾਲੀ named_event_delete(class __vpiHandle *ਵਸਤੂ);
ਬਾਹਰੀ ਖਾਲੀ parameter_delete(class __vpiHandle *ਵਸਤੂ);
ਬਾਹਰੀ ਖਾਲੀ signal_delete(class __vpiHandle *ਵਸਤੂ);
ਬਾਹਰੀ ਖਾਲੀ real_delete(class __vpiHandle *ਵਸਤੂ);
ਬਾਹਰੀ ਖਾਲੀ string_delete(class __vpiHandle *ਵਸਤੂ);
ਬਾਹਰੀ ਖਾਲੀ thread_string_delete(class __vpiHandle *ਵਸਤੂ);
ਬਾਹਰੀ ਖਾਲੀ thread_vthr_delete(class __vpiHandle *ਵਸਤੂ);
ਬਾਹਰੀ ਖਾਲੀ thread_ਸ਼ਬਦ_delete(class __vpiHandle *ਵਸਤੂ);
ਬਾਹਰੀ ਖਾਲੀ vpi_call_delete(class __vpiHandle *ਵਸਤੂ);
ਬਾਹਰੀ ਖਾਲੀ exec_ufunc_delete(vvp_ਕੋਡ_t euf_ਕੋਡ);
ਬਾਹਰੀ ਖਾਲੀ vthreads_delete(__vpiScope*scope);
ਬਾਹਰੀ ਖਾਲੀ vvp_net_delete(vvp_net_t *ਵਸਤੂ);


ਬਾਹਰੀ ਖਾਲੀ pool_local_net(vvp_net_t *net);

#ਜੇ_ਅੰਤ /* IVL_vvp_cleanup_H */
