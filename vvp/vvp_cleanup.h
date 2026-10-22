#నిర్వచితం_కాకపోతే IVL_vvp_cleanup_H
#నిర్వచించు IVL_vvp_cleanup_H
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

బాహ్య శూన్య codespace_delete(శూన్య);
బాహ్య శూన్య dec_str_delete(శూన్య);
బాహ్య శూన్య def_పట్టిక_delete(శూన్య);
బాహ్య శూన్య island_delete(శూన్య);
బాహ్య శూన్య vpi_mcd_delete(శూన్య);
బాహ్య శూన్య ఎక్కించు_module_delete(శూన్య);
బాహ్య శూన్య modpath_delete(శూన్య);
బాహ్య శూన్య మూలం_పట్టిక_delete(శూన్య);
బాహ్య శూన్య కాలపట్టిక_delete(శూన్య);
బాహ్య శూన్య signal_pool_delete(శూన్య);
బాహ్య శూన్య simulator_cb_delete(శూన్య);
బాహ్య శూన్య udp_defns_delete(శూన్య);
బాహ్య శూన్య vpi_handle_delete(శూన్య);
బాహ్య శూన్య vpi_దొంతర_delete(శూన్య);
బాహ్య శూన్య vvp_net_pool_delete(శూన్య);
బాహ్య శూన్య ufunc_pool_delete(శూన్య);

బాహ్య శూన్య A_delete(class __vpiHandle *అంశం);
బాహ్య శూన్య APV_delete(class __vpiHandle *అంశం);
బాహ్య శూన్య PV_delete(class __vpiHandle *అంశం);
బాహ్య శూన్య class_delete(class __vpiHandle *అంశం);
బాహ్య శూన్య class_def_delete(class_type *అంశం);
బాహ్య శూన్య constant_delete(class __vpiHandle *అంశం);
బాహ్య శూన్య contexts_delete(__vpiScope *scope);
బాహ్య శూన్య array_delete(class __vpiHandle *అంశం);
బాహ్య శూన్య enum_delete(class __vpiHandle *అంశం);
బాహ్య శూన్య జ్ఞాపకం_delete(class __vpiHandle *అంశం);
బాహ్య శూన్య named_event_delete(class __vpiHandle *అంశం);
బాహ్య శూన్య parameter_delete(class __vpiHandle *అంశం);
బాహ్య శూన్య signal_delete(class __vpiHandle *అంశం);
బాహ్య శూన్య real_delete(class __vpiHandle *అంశం);
బాహ్య శూన్య string_delete(class __vpiHandle *అంశం);
బాహ్య శూన్య thread_string_delete(class __vpiHandle *అంశం);
బాహ్య శూన్య thread_vthr_delete(class __vpiHandle *అంశం);
బాహ్య శూన్య thread_మాట_delete(class __vpiHandle *అంశం);
బాహ్య శూన్య vpi_call_delete(class __vpiHandle *అంశం);
బాహ్య శూన్య exec_ufunc_delete(vvp_సంకేతం_t euf_సంకేతం);
బాహ్య శూన్య vthreads_delete(__vpiScope*scope);
బాహ్య శూన్య vvp_net_delete(vvp_net_t *అంశం);


బాహ్య శూన్య pool_local_net(vvp_net_t *net);

#అయితే_ముగింపు /* IVL_vvp_cleanup_H */
