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

બાહ્ય શૂન્ય codespace_delete(શૂન્ય);
બાહ્ય શૂન્ય dec_str_delete(શૂન્ય);
બાહ્ય શૂન્ય def_table_delete(શૂન્ય);
બાહ્ય શૂન્ય island_delete(શૂન્ય);
બાહ્ય શૂન્ય vpi_mcd_delete(શૂન્ય);
બાહ્ય શૂન્ય load_module_delete(શૂન્ય);
બાહ્ય શૂન્ય modpath_delete(શૂન્ય);
બાહ્ય શૂન્ય મૂળ_table_delete(શૂન્ય);
બાહ્ય શૂન્ય schedule_delete(શૂન્ય);
બાહ્ય શૂન્ય signal_pool_delete(શૂન્ય);
બાહ્ય શૂન્ય simulator_cb_delete(શૂન્ય);
બાહ્ય શૂન્ય udp_defns_delete(શૂન્ય);
બાહ્ય શૂન્ય vpi_handle_delete(શૂન્ય);
બાહ્ય શૂન્ય vpi_ઢગલો_delete(શૂન્ય);
બાહ્ય શૂન્ય vvp_net_pool_delete(શૂન્ય);
બાહ્ય શૂન્ય ufunc_pool_delete(શૂન્ય);

બાહ્ય શૂન્ય A_delete(class __vpiHandle *item);
બાહ્ય શૂન્ય APV_delete(class __vpiHandle *item);
બાહ્ય શૂન્ય PV_delete(class __vpiHandle *item);
બાહ્ય શૂન્ય class_delete(class __vpiHandle *item);
બાહ્ય શૂન્ય class_def_delete(class_type *item);
બાહ્ય શૂન્ય constant_delete(class __vpiHandle *item);
બાહ્ય શૂન્ય contexts_delete(__vpiScope *scope);
બાહ્ય શૂન્ય array_delete(class __vpiHandle *item);
બાહ્ય શૂન્ય enum_delete(class __vpiHandle *item);
બાહ્ય શૂન્ય memory_delete(class __vpiHandle *item);
બાહ્ય શૂન્ય named_event_delete(class __vpiHandle *item);
બાહ્ય શૂન્ય parameter_delete(class __vpiHandle *item);
બાહ્ય શૂન્ય signal_delete(class __vpiHandle *item);
બાહ્ય શૂન્ય real_delete(class __vpiHandle *item);
બાહ્ય શૂન્ય string_delete(class __vpiHandle *item);
બાહ્ય શૂન્ય thread_string_delete(class __vpiHandle *item);
બાહ્ય શૂન્ય thread_vthr_delete(class __vpiHandle *item);
બાહ્ય શૂન્ય thread_word_delete(class __vpiHandle *item);
બાહ્ય શૂન્ય vpi_call_delete(class __vpiHandle *item);
બાહ્ય શૂન્ય exec_ufunc_delete(vvp_code_t euf_code);
બાહ્ય શૂન્ય vthreads_delete(__vpiScope*scope);
બાહ્ય શૂન્ય vvp_net_delete(vvp_net_t *item);


બાહ્ય શૂન્ય pool_local_net(vvp_net_t *net);

#endif /* IVL_vvp_cleanup_H */
