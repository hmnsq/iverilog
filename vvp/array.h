#నిర్వచితం_కాకపోతే IVL_array_H
#నిర్వచించు IVL_array_H
/*
 * Copyright (c) 2007-2014 Stephen Williams (steve@icarus.com)
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

#అన్తర్భావ "vvp_net.h"
#అన్తర్భావ "vpi_user.h"

రకనిర్వచనం నిర్మాణం __vpiArray* vvp_array_t;
class విలువ_callback;

/*
 * This function tries to find the array (by label) in the global
 * table of all the arrays in the design.
 */
బాహ్య vvp_array_t array_కనుగొను(స్థిరరాశి అక్షరం*label);

/* VPI hooks */
బాహ్య విలువ_callback* vpip_array_మాట_change(p_cb_దత్తాంశం దత్తాంశం);
బాహ్య విలువ_callback* vpip_array_change(p_cb_దత్తాంశం దత్తాంశం);

#అయితే_ముగింపు /* IVL_array_H */
