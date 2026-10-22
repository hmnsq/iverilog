#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_vvp_cleanup_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_vvp_cleanup_H
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

ಬಾಹ್ಯ ಶೂನ್ಯ codespace_delete(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ dec_str_delete(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ def_ಕೋಷ್ಟಕ_delete(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ island_delete(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ vpi_mcd_delete(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ ತುಂಬು_module_delete(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ modpath_delete(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ ಬೇರು_ಕೋಷ್ಟಕ_delete(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_delete(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ signal_pool_delete(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ simulator_cb_delete(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ udp_defns_delete(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ vpi_handle_delete(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ vpi_ರಾಶಿ_delete(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ vvp_net_pool_delete(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ ufunc_pool_delete(ಶೂನ್ಯ);

ಬಾಹ್ಯ ಶೂನ್ಯ A_delete(class __vpiHandle *ಅಂಶ);
ಬಾಹ್ಯ ಶೂನ್ಯ APV_delete(class __vpiHandle *ಅಂಶ);
ಬಾಹ್ಯ ಶೂನ್ಯ PV_delete(class __vpiHandle *ಅಂಶ);
ಬಾಹ್ಯ ಶೂನ್ಯ class_delete(class __vpiHandle *ಅಂಶ);
ಬಾಹ್ಯ ಶೂನ್ಯ class_def_delete(class_type *ಅಂಶ);
ಬಾಹ್ಯ ಶೂನ್ಯ constant_delete(class __vpiHandle *ಅಂಶ);
ಬಾಹ್ಯ ಶೂನ್ಯ contexts_delete(__vpiScope *scope);
ಬಾಹ್ಯ ಶೂನ್ಯ array_delete(class __vpiHandle *ಅಂಶ);
ಬಾಹ್ಯ ಶೂನ್ಯ enum_delete(class __vpiHandle *ಅಂಶ);
ಬಾಹ್ಯ ಶೂನ್ಯ ಸ್ಮರಣೆ_delete(class __vpiHandle *ಅಂಶ);
ಬಾಹ್ಯ ಶೂನ್ಯ named_event_delete(class __vpiHandle *ಅಂಶ);
ಬಾಹ್ಯ ಶೂನ್ಯ parameter_delete(class __vpiHandle *ಅಂಶ);
ಬಾಹ್ಯ ಶೂನ್ಯ signal_delete(class __vpiHandle *ಅಂಶ);
ಬಾಹ್ಯ ಶೂನ್ಯ real_delete(class __vpiHandle *ಅಂಶ);
ಬಾಹ್ಯ ಶೂನ್ಯ string_delete(class __vpiHandle *ಅಂಶ);
ಬಾಹ್ಯ ಶೂನ್ಯ thread_string_delete(class __vpiHandle *ಅಂಶ);
ಬಾಹ್ಯ ಶೂನ್ಯ thread_vthr_delete(class __vpiHandle *ಅಂಶ);
ಬಾಹ್ಯ ಶೂನ್ಯ thread_ಶಬ್ದ_delete(class __vpiHandle *ಅಂಶ);
ಬಾಹ್ಯ ಶೂನ್ಯ vpi_call_delete(class __vpiHandle *ಅಂಶ);
ಬಾಹ್ಯ ಶೂನ್ಯ exec_ufunc_delete(vvp_ಸಂಕೇತ_t euf_ಸಂಕೇತ);
ಬಾಹ್ಯ ಶೂನ್ಯ vthreads_delete(__vpiScope*scope);
ಬಾಹ್ಯ ಶೂನ್ಯ vvp_net_delete(vvp_net_t *ಅಂಶ);


ಬಾಹ್ಯ ಶೂನ್ಯ pool_local_net(vvp_net_t *net);

#ಆದರೆ_ಅಂತ್ಯ /* IVL_vvp_cleanup_H */
