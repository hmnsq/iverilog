#నిర్వచితం_కాకపోతే IVL_vvp_island_H
#నిర్వచించు IVL_vvp_island_H
/*
 * Copyright (c) 2008-2026 Stephen Williams (steve@icarus.com)
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
# include  "vvp_net.h"
# include  "vvp_net_sig.h"
# include  "symbols.h"
# include  "schedule.h"
# include  <list>
# include  <cassert>

/*
* Islands are mutually connected bidirectional meshes that have a
* discipline other than the implicit ddiscipline of the rest of the
* run time.
*
* In the vvp input, an island is created with this record:
*
*    <label> .island ;
*
* The <label> is the name given to the island. Records after this
* build up the contents of the island. Ports are created like this:
*
*    <label> .port <island>, <src> ;
*    <label> .import <island>, <src> ;
*    <label> .export <island> ;
*
* The .port, .import and .export records create I/O, input and output
* ports. The <label> is the name that branches within the island can
* use to link to the port, and the <island> is the label for the
* island. The input and I/O ports have a <src> label that links to the
* source net from the ddiscrete domain.
*
* Branches within the island may only reference labels within the
* island. This keeps the nets of the ocean of digital away from the
* branches of analog within the island.
*/

నిర్మాణం vvp_island_branch;
class vvp_island_పోర్టు;

class vvp_island  : అంతరంగ vvp_gen_event_s {

    బహిరంగ:
      vvp_island();
      మాయా ~vvp_island() override;

      vvp_island(స్థిరరాశి vvp_island&) = delete;
      vvp_island& క్రియాకారకం=(స్థిరరాశి vvp_island&) = delete;

	// Ports call this method to flag that something happened at
	// the input. The island will use this to create an active
	// event. The run_run() method will then be called by the
	// scheduler to process whatever happened.
      శూన్య flag_island();

	// This is the method that is called, eventually, to process
	// whatever happened. The derived island class implements this
	// method to give the island its character.
      మాయా శూన్య నిరంతరం_island() =0;

        // Support for $countdrivers.
      మాయా శూన్య లెక్క_drivers(vvp_island_పోర్టు*పోర్టు, గుర్తులేని బిట్టు_idx,
                                 గుర్తులేని counts[3]) =0;

    రక్షిత:
	// The base class collects a list of all the branches in the
	// island. The derived island class can access this list for
	// scanning the mesh.
      vvp_island_branch*branches_;

    బహిరంగ: /* These methods are used during linking. */

	// Add a port to the island. The key is added to the island
	// ports symbol table.
      శూన్య కూడు_పోర్టు(స్థిరరాశి అక్షరం*కీలకం, vvp_net_t*net);
	// Add a branch to the island.
      శూన్య కూడు_branch(vvp_island_branch*branch, స్థిరరాశి అక్షరం*pa, స్థిరరాశి అక్షరం*pb);

      vvp_net_t* కనుగొను_పోర్టు(స్థిరరాశి అక్షరం*కీలకం);

	// Call this method when linking is done.
      శూన్య compile_cleanup(శూన్య);

    అంతరంగ:
      శూన్య నిరంతరం_నిరంతరం() override;
      bool flagged_;

    అంతరంగ:
	// During link, the vvp_island keeps these symbol tables for
	// mapping labels local to the island. When linking is done,
	// the compile_cleanup() method removes these tables.
      చిహ్నం_map_s<vvp_net_t>*ports_;
      చిహ్నం_map_s<vvp_island_branch>*anodes_;
      చిహ్నం_map_s<vvp_island_branch>*bnodes_;
};

/*
* An island port is a functor that connects to the ddiscrete
* discipline outside the island. (There is also a vvp_net_t object
* that refers to this port.) When data comes to the port from outside,
* it is collected and saved, and the island is notified. When code
* inside the island sends data out of the island, it uses the "out"
* pointer from the vvp_net_t that refers to this object.
*/

class vvp_island_పోర్టు  : బహిరంగ vvp_net_fun_t {

    బహిరంగ:
      explicit vvp_island_పోర్టు(vvp_island*ip);
      ~vvp_island_పోర్టు() override;

    బహిరంగ: // Implement vvp_net_fun_t methods
      మాయా శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                             vvp_context_t) override;
      మాయా శూన్య recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
				గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t) override;
      మాయా శూన్య recv_vec8(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector8_t&బిట్టు) override;
      మాయా శూన్య recv_vec8_pv(vvp_net_ptr_t p, స్థిరరాశి vvp_vector8_t&బిట్టు,
				గుర్తులేని పునాది, గుర్తులేని vwid) override;

	// This is painful, but necessary. If the island is connected
	// to a forced net, we need to rerun the calculations whenever
	// a force/release happens to the net. If run_now is true, we
	// rerun immediately, otherwise we schedule it for the end of
	// the current time slot.
      మాయా శూన్య force_flag(bool నిరంతరం_now) override;

    బహిరంగ:
      vvp_vector8_t invalue;
      vvp_vector8_t outvalue;
      vvp_vector8_t విలువ;

    అంతరంగ:
      vvp_island*island_;

    బహిరంగ: // Support for $countdrivers.
      అంతఃవరుస శూన్య లెక్క_drivers(గుర్తులేని బిట్టు_idx, గుర్తులేని counts[3])
      {
            island_->లెక్క_drivers(this, బిట్టు_idx, counts);
      }

    అంతరంగ: // not implemented
      vvp_island_పోర్టు(స్థిరరాశి vvp_island_పోర్టు&);
      vvp_island_పోర్టు& క్రియాకారకం = (స్థిరరాశి vvp_island_పోర్టు&);
};

అంతఃవరుస vvp_vector8_t island_get_విలువ(vvp_net_t*net)
{
      స్థిరరాశి vvp_island_పోర్టు*fun = dynamic_cast<vvp_island_పోర్టు*>(net->fun);
      స్థిరరాశి vvp_wire_vec8*fil = dynamic_cast<vvp_wire_vec8*>(net->fil);

      ఒకవేళ (fil == 0) {
	    తిరిగి fun->invalue;
      } లేకపోతే {
	      // This is painful, but necessary. If the island is
	      // connected to a forced net, then run the input through
	      // the force filter first. The island must used the
	      // forced value for its deliberations.
	    vvp_vector8_t rep;
	    ఎంపిక (fil->filter_ఆదానం_vec8(fun->invalue, rep)) {
		అప్రమేయ:
		స్థితి vvp_net_fil_t::PROP:
		  తిరిగి fun->invalue;
		స్థితి vvp_net_fil_t::REPL:
		  తిరిగి rep;
	    }
      }
}

అంతఃవరుస vvp_vector8_t island_get_పంపిన_విలువ(vvp_net_t*net)
{
      స్థిరరాశి vvp_island_పోర్టు*fun = dynamic_cast<vvp_island_పోర్టు*>(net->fun);
      తిరిగి fun->outvalue;
}

బాహ్య శూన్య island_send_విలువ(vvp_net_t*net, స్థిరరాశి vvp_vector8_t&val);

/*
* Branches are connected together to form a mesh of branches. Each
* endpoint (there are two) connects circularly to other branch
* endpoints that are connected together. This list of endpoints forms
* a node. Thus it is possible for branches to fully specify the mesh
* of the island.
*/

రకనిర్వచనం vvp_sub_సూచిక_t<vvp_island_branch> vvp_branch_ptr_t;

నిర్మాణం vvp_island_branch {
      మాయా ~vvp_island_branch();
	// Keep a list of branches in the island.
      vvp_island_branch*తదుపరి_branch;
	// branch mesh connectivity. There is a pointer for each end
	// that participates in a circular list.
      vvp_sub_సూచిక_t<vvp_island_branch> link[2];
	// Port connections
      vvp_net_t*a;
      vvp_net_t*b;
};

స్థిర అంతఃవరుస vvp_branch_ptr_t తదుపరి_2(vvp_branch_ptr_t cur)
{
      స్థిరరాశి vvp_island_branch*ptr = cur.ptr();
      గుర్తులేని ab = cur.పోర్టు();
      తిరిగి ptr->link[ab];
}

/*
 * This function collects into the conn list all the branch ends
 * that are connected together with the reference branch endpoint
 * cur. All these branch ends together form a "node" of the branch
 * network. (In other words, this does *not* follow the branches.)
 */
బాహ్య శూన్య island_collect_కణుపు(std::list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur);

/*
 * These functions support compile/linking.
 */
బాహ్య శూన్య compile_island_పునాది(అక్షరం*label, vvp_island*use_island);
బాహ్య vvp_island* compile_కనుగొను_island(స్థిరరాశి అక్షరం*island_name);

#అయితే_ముగింపు /* IVL_vvp_island_H */
