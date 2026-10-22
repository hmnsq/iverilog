#अगर_अघोषित IVL_vvp_island_H
#घोषणा IVL_vvp_island_H
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

काष्ठा vvp_island_branch;
class vvp_island_port;

class vvp_island  : निजी vvp_gen_event_s {

    खुला:
      vvp_island();
      भव ~vvp_island() override;

      vvp_island(स्थिर vvp_island&) = delete;
      vvp_island& चालक=(स्थिर vvp_island&) = delete;

	// Ports call this method to flag that something happened at
	// the input. The island will use this to create an active
	// event. The run_run() method will then be called by the
	// scheduler to process whatever happened.
      व्योम flag_island();

	// This is the method that is called, eventually, to process
	// whatever happened. The derived island class implements this
	// method to give the island its character.
      भव व्योम run_island() =0;

        // Support for $countdrivers.
      भव व्योम गिनती_drivers(vvp_island_port*port, अचिन्हित bit_idx,
                                 अचिन्हित counts[3]) =0;

    रक्षित:
	// The base class collects a list of all the branches in the
	// island. The derived island class can access this list for
	// scanning the mesh.
      vvp_island_branch*branches_;

    खुला: /* These methods are used during linking. */

	// Add a port to the island. The key is added to the island
	// ports symbol table.
      व्योम add_port(स्थिर अक्षर*कुंजी, vvp_net_t*net);
	// Add a branch to the island.
      व्योम add_branch(vvp_island_branch*branch, स्थिर अक्षर*pa, स्थिर अक्षर*pb);

      vvp_net_t* find_port(स्थिर अक्षर*कुंजी);

	// Call this method when linking is done.
      व्योम compile_cleanup(व्योम);

    निजी:
      व्योम run_run() override;
      bool flagged_;

    निजी:
	// During link, the vvp_island keeps these symbol tables for
	// mapping labels local to the island. When linking is done,
	// the compile_cleanup() method removes these tables.
      प्रतीक_map_s<vvp_net_t>*ports_;
      प्रतीक_map_s<vvp_island_branch>*anodes_;
      प्रतीक_map_s<vvp_island_branch>*bnodes_;
};

/*
* An island port is a functor that connects to the ddiscrete
* discipline outside the island. (There is also a vvp_net_t object
* that refers to this port.) When data comes to the port from outside,
* it is collected and saved, and the island is notified. When code
* inside the island sends data out of the island, it uses the "out"
* pointer from the vvp_net_t that refers to this object.
*/

class vvp_island_port  : खुला vvp_net_fun_t {

    खुला:
      explicit vvp_island_port(vvp_island*ip);
      ~vvp_island_port() override;

    खुला: // Implement vvp_net_fun_t methods
      भव व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                             vvp_context_t) override;
      भव व्योम recv_vec4_pv(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
				अचिन्हित base, अचिन्हित vwid, vvp_context_t) override;
      भव व्योम recv_vec8(vvp_net_ptr_t port, स्थिर vvp_vector8_t&bit) override;
      भव व्योम recv_vec8_pv(vvp_net_ptr_t p, स्थिर vvp_vector8_t&bit,
				अचिन्हित base, अचिन्हित vwid) override;

	// This is painful, but necessary. If the island is connected
	// to a forced net, we need to rerun the calculations whenever
	// a force/release happens to the net. If run_now is true, we
	// rerun immediately, otherwise we schedule it for the end of
	// the current time slot.
      भव व्योम force_flag(bool run_now) override;

    खुला:
      vvp_vector8_t invalue;
      vvp_vector8_t outvalue;
      vvp_vector8_t मूल्य;

    निजी:
      vvp_island*island_;

    खुला: // Support for $countdrivers.
      अंतरभूत व्योम गिनती_drivers(अचिन्हित bit_idx, अचिन्हित counts[3])
      {
            island_->गिनती_drivers(this, bit_idx, counts);
      }

    निजी: // not implemented
      vvp_island_port(स्थिर vvp_island_port&);
      vvp_island_port& चालक = (स्थिर vvp_island_port&);
};

अंतरभूत vvp_vector8_t island_get_मूल्य(vvp_net_t*net)
{
      स्थिर vvp_island_port*fun = dynamic_cast<vvp_island_port*>(net->fun);
      स्थिर vvp_wire_vec8*fil = dynamic_cast<vvp_wire_vec8*>(net->fil);

      अगर (fil == 0) {
	    वापस fun->invalue;
      } अन्यथा {
	      // This is painful, but necessary. If the island is
	      // connected to a forced net, then run the input through
	      // the force filter first. The island must used the
	      // forced value for its deliberations.
	    vvp_vector8_t rep;
	    चयन (fil->filter_निवेश_vec8(fun->invalue, rep)) {
		शेष:
		हाल vvp_net_fil_t::PROP:
		  वापस fun->invalue;
		हाल vvp_net_fil_t::REPL:
		  वापस rep;
	    }
      }
}

अंतरभूत vvp_vector8_t island_get_sent_मूल्य(vvp_net_t*net)
{
      स्थिर vvp_island_port*fun = dynamic_cast<vvp_island_port*>(net->fun);
      वापस fun->outvalue;
}

बाह्य व्योम island_send_मूल्य(vvp_net_t*net, स्थिर vvp_vector8_t&val);

/*
* Branches are connected together to form a mesh of branches. Each
* endpoint (there are two) connects circularly to other branch
* endpoints that are connected together. This list of endpoints forms
* a node. Thus it is possible for branches to fully specify the mesh
* of the island.
*/

प्रकार vvp_sub_pointer_t<vvp_island_branch> vvp_branch_ptr_t;

काष्ठा vvp_island_branch {
      भव ~vvp_island_branch();
	// Keep a list of branches in the island.
      vvp_island_branch*अग्र_branch;
	// branch mesh connectivity. There is a pointer for each end
	// that participates in a circular list.
      vvp_sub_pointer_t<vvp_island_branch> link[2];
	// Port connections
      vvp_net_t*a;
      vvp_net_t*b;
};

अटल अंतरभूत vvp_branch_ptr_t अग्र_2(vvp_branch_ptr_t cur)
{
      स्थिर vvp_island_branch*ptr = cur.ptr();
      अचिन्हित ab = cur.port();
      वापस ptr->link[ab];
}

/*
 * This function collects into the conn list all the branch ends
 * that are connected together with the reference branch endpoint
 * cur. All these branch ends together form a "node" of the branch
 * network. (In other words, this does *not* follow the branches.)
 */
बाह्य व्योम island_collect_गाँठ(std::list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur);

/*
 * These functions support compile/linking.
 */
बाह्य व्योम compile_island_base(अक्षर*label, vvp_island*use_island);
बाह्य vvp_island* compile_find_island(स्थिर अक्षर*island_name);

#पूर्ण_अगर /* IVL_vvp_island_H */
