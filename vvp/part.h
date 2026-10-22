#నిర్వచితం_కాకపోతే IVL_part_H
#నిర్వచించు IVL_part_H
/*
 * Copyright (c) 2005-2025 Stephen Williams (steve@icarus.com)
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

# include  "schedule.h"
# include  "config.h"

/* vvp_fun_part
 * This node takes a part select of the input vector. Input 0 is the
 * vector to be selected from, and input 1 is the location where the
 * select starts. Input 2, which is typically constant, is the width
 * of the result.
 */
class vvp_fun_part  : బహిరంగ vvp_net_fun_t {

    బహిరంగ:
      vvp_fun_part(గుర్తులేని పునాది, గుర్తులేని wid);
      ~vvp_fun_part() override;

      గుర్తులేని get_పునాది() స్థిరరాశి { తిరిగి పునాది_2; }
      గుర్తులేని get_wid() స్థిరరాశి { తిరిగి wid_; }

    రక్షిత:
      గుర్తులేని పునాది_2;
      గుర్తులేని wid_;
};

/*
 * Statically allocated vvp_fun_part.
 */
class vvp_fun_part_sa  : బహిరంగ vvp_fun_part, బహిరంగ vvp_gen_event_s {

    బహిరంగ:
      vvp_fun_part_sa(గుర్తులేని పునాది, గుర్తులేని wid);
      ~vvp_fun_part_sa() override;

    బహిరంగ:
      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;

      శూన్య recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t) override;

    అంతరంగ:
      శూన్య నిరంతరం_నిరంతరం() override;

    అంతరంగ:
      vvp_vector4_t val_;
      vvp_net_t*net_;
};

/*
 * Automatically allocated vvp_fun_part.
 */
class vvp_fun_part_aa  : బహిరంగ vvp_fun_part, బహిరంగ automatic_hooks_s {

    బహిరంగ:
      vvp_fun_part_aa(గుర్తులేని పునాది, గుర్తులేని wid);
      ~vvp_fun_part_aa() override;

    బహిరంగ:
      శూన్య alloc_instance(vvp_context_t context) override;
      శూన్య reset_instance(vvp_context_t context) override;
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      శూన్య free_instance(vvp_context_t context) override;
#అయితే_ముగింపు

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t context) override;

      శూన్య recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t context) override;

    అంతరంగ:
      __vpiScope*context_scope_;
      గుర్తులేని context_idx_;
};

/* vvp_fun_part_pv
 * This node takes a vector input and turns it into the part select of
 * a wider output network. It used the recv_vec4_pv methods of the
 * destination nodes to propagate the part select. It can be used in
 * both statically and automatically allocated scopes, as it has no
 * dynamic state.
 */
class vvp_fun_part_pv  : బహిరంగ vvp_net_fun_t {

    బహిరంగ:
      vvp_fun_part_pv(గుర్తులేని పునాది, గుర్తులేని wid, గుర్తులేని vec_wid);
      ~vvp_fun_part_pv() override;

    బహిరంగ:
      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t context) override;

      శూన్య recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                        గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t) override;

      శూన్య recv_vec8(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector8_t&బిట్టు) override;

    అంతరంగ:
      గుర్తులేని పునాది_2;
      గుర్తులేని wid_;
      గుర్తులేని vwid_;
};

/*
 * This part select is more flexible in that it takes the vector to
 * part in port 0, and the base of the part in port 1. The width of
 * the part to take out is fixed.
 */
class vvp_fun_part_var  : బహిరంగ vvp_net_fun_t {

    బహిరంగ:
      explicit vvp_fun_part_var(గుర్తులేని wid, bool is_signed);
      ~vvp_fun_part_var() override;

    రక్షిత:
      bool recv_vec4_(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                      పూర్ణాంకం&పునాది, vvp_vector4_t&ఆధారం,
                      vvp_vector4_t&ref);

      గుర్తులేని wid_;
      bool is_signed_;
};

/*
 * Statically allocated vvp_fun_part_var.
 */
class vvp_fun_part_var_sa  : బహిరంగ vvp_fun_part_var {

    బహిరంగ:
      explicit vvp_fun_part_var_sa(గుర్తులేని wid, bool is_signed);
      ~vvp_fun_part_var_sa() override;

    బహిరంగ:
      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;

      శూన్య recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t) override;

    అంతరంగ:
      పూర్ణాంకం పునాది_2;
      vvp_vector4_t ఆధారం_2;
	// Save the last output, for detecting change.
      vvp_vector4_t ref_;
};

/*
 * Automatically allocated vvp_fun_part_var.
 */
class vvp_fun_part_var_aa  : బహిరంగ vvp_fun_part_var, బహిరంగ automatic_hooks_s {

    బహిరంగ:
      explicit vvp_fun_part_var_aa(గుర్తులేని wid, bool is_signed);
      ~vvp_fun_part_var_aa() override;

    బహిరంగ:
      శూన్య alloc_instance(vvp_context_t context) override;
      శూన్య reset_instance(vvp_context_t context) override;
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      శూన్య free_instance(vvp_context_t context) override;
#అయితే_ముగింపు

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t context) override;

      శూన్య recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t context) override;

    అంతరంగ:
      __vpiScope*context_scope_;
      గుర్తులేని context_idx_;
};

#అయితే_ముగింపు /* IVL_part_H */
