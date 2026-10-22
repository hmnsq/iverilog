#నిర్వచితం_కాకపోతే IVL_sfunc_H
#నిర్వచించు IVL_sfunc_H
/*
 * Copyright (c) 2006-2025 Stephen Williams (steve@icarus.com)
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

# include  "config.h"

class sfunc_core : బహిరంగ vvp_wide_fun_core, రక్షిత vvp_gen_event_s {

    బహిరంగ:
      sfunc_core(vvp_net_t*ptr, vpiHandle sys, గుర్తులేని argc, vpiHandle*argv);
      ~sfunc_core() override;

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t context) override;

    అంతరంగ:
      శూన్య recv_vec4_from_inputs(గుర్తులేని పోర్టు) override;
      శూన్య recv_real_from_inputs(గుర్తులేని పోర్టు) override;

      శూన్య నిరంతరం_నిరంతరం() override;


    అంతరంగ:
      vpiHandle sys_;
      గుర్తులేని  argc_;
      vpiHandle*argv_;
};

#అయితే_ముగింపు /* IVL_sfunc_H */
