#अगर_अघोषित IVL_vvp_cleanup_H
#घोषणा IVL_vvp_cleanup_H
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

बाह्य व्योम codespace_delete(व्योम);
बाह्य व्योम dec_str_delete(व्योम);
बाह्य व्योम def_table_delete(व्योम);
बाह्य व्योम island_delete(व्योम);
बाह्य व्योम vpi_mcd_delete(व्योम);
बाह्य व्योम load_module_delete(व्योम);
बाह्य व्योम modpath_delete(व्योम);
बाह्य व्योम मूल_table_delete(व्योम);
बाह्य व्योम schedule_delete(व्योम);
बाह्य व्योम signal_pool_delete(व्योम);
बाह्य व्योम simulator_cb_delete(व्योम);
बाह्य व्योम udp_defns_delete(व्योम);
बाह्य व्योम vpi_handle_delete(व्योम);
बाह्य व्योम vpi_ढेर_delete(व्योम);
बाह्य व्योम vvp_net_pool_delete(व्योम);
बाह्य व्योम ufunc_pool_delete(व्योम);

बाह्य व्योम A_delete(class __vpiHandle *item);
बाह्य व्योम APV_delete(class __vpiHandle *item);
बाह्य व्योम PV_delete(class __vpiHandle *item);
बाह्य व्योम class_delete(class __vpiHandle *item);
बाह्य व्योम class_def_delete(class_type *item);
बाह्य व्योम constant_delete(class __vpiHandle *item);
बाह्य व्योम contexts_delete(__vpiScope *scope);
बाह्य व्योम array_delete(class __vpiHandle *item);
बाह्य व्योम enum_delete(class __vpiHandle *item);
बाह्य व्योम memory_delete(class __vpiHandle *item);
बाह्य व्योम named_event_delete(class __vpiHandle *item);
बाह्य व्योम parameter_delete(class __vpiHandle *item);
बाह्य व्योम signal_delete(class __vpiHandle *item);
बाह्य व्योम real_delete(class __vpiHandle *item);
बाह्य व्योम string_delete(class __vpiHandle *item);
बाह्य व्योम thread_string_delete(class __vpiHandle *item);
बाह्य व्योम thread_vthr_delete(class __vpiHandle *item);
बाह्य व्योम thread_word_delete(class __vpiHandle *item);
बाह्य व्योम vpi_call_delete(class __vpiHandle *item);
बाह्य व्योम exec_ufunc_delete(vvp_code_t euf_code);
बाह्य व्योम vthreads_delete(__vpiScope*scope);
बाह्य व्योम vvp_net_delete(vvp_net_t *item);


बाह्य व्योम pool_local_net(vvp_net_t *net);

#पूर्ण_अगर /* IVL_vvp_cleanup_H */
