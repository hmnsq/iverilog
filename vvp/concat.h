#నిర్వచితం_కాకపోతే IVL_concat_H
#నిర్వచించు IVL_concat_H
/*
 * Copyright (c) 2004-2025 Stephen Williams (steve@icarus.com)
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

# include "vvp_net.h"

/* vvp_fun_concat
 * This node function creates vectors (vvp_vector4_t) from the
 * concatenation of the inputs. The inputs (4) may be vector or
 * vector8 objects, but they are reduced to vector4 values and
 * strength information lost.
 *
 * The expected widths of the input vectors must be given up front so
 * that the positions in the output vector (and also the size of the
 * output vector) can be worked out. The input vectors must match the
 * expected width.
 */
class vvp_fun_concat  : బహిరంగ vvp_net_fun_t, రక్షిత vvp_gen_event_s  {

    బహిరంగ:
      vvp_fun_concat(గుర్తులేని w0, గుర్తులేని w1,
		     గుర్తులేని w2, గుర్తులేని w3);
      ~vvp_fun_concat() override;

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t context) final;

      శూన్య recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t) final;
    అంతరంగ:
      శూన్య నిరంతరం_నిరంతరం() final;

      గుర్తులేని wid_[4];
      vvp_vector4_t val_;
      vvp_net_t *net_ = nullptr;
};

class vvp_fun_concat8  : బహిరంగ vvp_net_fun_t, రక్షిత vvp_gen_event_s {

    బహిరంగ:
      vvp_fun_concat8(గుర్తులేని w0, గుర్తులేని w1,
		     గుర్తులేని w2, గుర్తులేని w3);
      ~vvp_fun_concat8() override;

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t context) final;
      శూన్య recv_vec8(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector8_t&బిట్టు) final;

      శూన్య recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t) final;
      శూన్య recv_vec8_pv(vvp_net_ptr_t p, స్థిరరాశి vvp_vector8_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid) final;

    అంతరంగ:
      శూన్య నిరంతరం_నిరంతరం() final;

      గుర్తులేని wid_[4];
      vvp_vector8_t val_;
      vvp_net_t *net_ = nullptr;
};

/* vvp_fun_repeat
 * This node function create vectors by repeating the input. The width
 * is the width of the output vector, and the repeat is the number of
 * times to repeat the input. The width of the input vector is
 * implicit from these values.
 */
class vvp_fun_repeat  : బహిరంగ vvp_net_fun_t {

    బహిరంగ:
      vvp_fun_repeat(గుర్తులేని వెడల్పు, గుర్తులేని repeat);
      ~vvp_fun_repeat() override;

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t context) override;
      శూన్య recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పూర్ణాంకం పునాది, గుర్తులేని పూర్ణాంకం vwid,
			vvp_context_t context) final;

    అంతరంగ:
      గుర్తులేని wid_;
      గుర్తులేని rep_;
};

#అయితే_ముగింపు
