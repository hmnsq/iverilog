#यदि_अपरिभाषितम् IVL_vvp_cleanup_H
#परिभाषय IVL_vvp_cleanup_H
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

बाह्य शून्य codespace_delete(शून्य);
बाह्य शून्य dec_str_delete(शून्य);
बाह्य शून्य def_सारणी_delete(शून्य);
बाह्य शून्य island_delete(शून्य);
बाह्य शून्य vpi_mcd_delete(शून्य);
बाह्य शून्य आरोपय_module_delete(शून्य);
बाह्य शून्य modpath_delete(शून्य);
बाह्य शून्य मूलम्_सारणी_delete(शून्य);
बाह्य शून्य कालसारणी_delete(शून्य);
बाह्य शून्य signal_pool_delete(शून्य);
बाह्य शून्य simulator_cb_delete(शून्य);
बाह्य शून्य udp_defns_delete(शून्य);
बाह्य शून्य vpi_handle_delete(शून्य);
बाह्य शून्य vpi_राशिः_delete(शून्य);
बाह्य शून्य vvp_net_pool_delete(शून्य);
बाह्य शून्य ufunc_pool_delete(शून्य);

बाह्य शून्य A_delete(class __vpiHandle *item);
बाह्य शून्य APV_delete(class __vpiHandle *item);
बाह्य शून्य PV_delete(class __vpiHandle *item);
बाह्य शून्य class_delete(class __vpiHandle *item);
बाह्य शून्य class_def_delete(class_type *item);
बाह्य शून्य constant_delete(class __vpiHandle *item);
बाह्य शून्य contexts_delete(__vpiScope *scope);
बाह्य शून्य array_delete(class __vpiHandle *item);
बाह्य शून्य enum_delete(class __vpiHandle *item);
बाह्य शून्य स्मृतिः_delete(class __vpiHandle *item);
बाह्य शून्य named_event_delete(class __vpiHandle *item);
बाह्य शून्य parameter_delete(class __vpiHandle *item);
बाह्य शून्य signal_delete(class __vpiHandle *item);
बाह्य शून्य real_delete(class __vpiHandle *item);
बाह्य शून्य string_delete(class __vpiHandle *item);
बाह्य शून्य thread_string_delete(class __vpiHandle *item);
बाह्य शून्य thread_vthr_delete(class __vpiHandle *item);
बाह्य शून्य thread_word_delete(class __vpiHandle *item);
बाह्य शून्य vpi_call_delete(class __vpiHandle *item);
बाह्य शून्य exec_ufunc_delete(vvp_संहिता_t euf_संहिता);
बाह्य शून्य vthreads_delete(__vpiScope*scope);
बाह्य शून्य vvp_net_delete(vvp_net_t *item);


बाह्य शून्य pool_local_net(vvp_net_t *net);

#यदि_अन्तः /* IVL_vvp_cleanup_H */
