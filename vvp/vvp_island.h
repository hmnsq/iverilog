#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_vvp_island_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_vvp_island_H
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

ರಚನೆ vvp_island_branch;
class vvp_island_ಪೋರ್ಟ್;

class vvp_island  : ಖಾಸಗಿ vvp_gen_event_s {

    ಸಾರ್ವಜನಿಕ:
      vvp_island();
      ವಾಸ್ತವಿಕ ~vvp_island() override;

      vvp_island(ಸ್ಥಿರಾಂಕ vvp_island&) = delete;
      vvp_island& ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ vvp_island&) = delete;

	// Ports call this method to flag that something happened at
	// the input. The island will use this to create an active
	// event. The run_run() method will then be called by the
	// scheduler to process whatever happened.
      ಶೂನ್ಯ flag_island();

	// This is the method that is called, eventually, to process
	// whatever happened. The derived island class implements this
	// method to give the island its character.
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಸತತ_island() =0;

        // Support for $countdrivers.
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಎಣಿಕೆ_drivers(vvp_island_ಪೋರ್ಟ್*ಪೋರ್ಟ್, ಚಿಹ್ನೆರಹಿತ ಬಿಟ್_idx,
                                 ಚಿಹ್ನೆರಹಿತ counts[3]) =0;

    ಸಂರಕ್ಷಿತ:
	// The base class collects a list of all the branches in the
	// island. The derived island class can access this list for
	// scanning the mesh.
      vvp_island_branch*branches_;

    ಸಾರ್ವಜನಿಕ: /* These methods are used during linking. */

	// Add a port to the island. The key is added to the island
	// ports symbol table.
      ಶೂನ್ಯ ಕೂಡಿಸು_ಪೋರ್ಟ್(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಕೀಲಿ, vvp_net_t*net);
	// Add a branch to the island.
      ಶೂನ್ಯ ಕೂಡಿಸು_branch(vvp_island_branch*branch, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*pa, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*pb);

      vvp_net_t* ಕಂಡುಹಿಡಿ_ಪೋರ್ಟ್(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಕೀಲಿ);

	// Call this method when linking is done.
      ಶೂನ್ಯ compile_cleanup(ಶೂನ್ಯ);

    ಖಾಸಗಿ:
      ಶೂನ್ಯ ಸತತ_ಸತತ() override;
      bool flagged_;

    ಖಾಸಗಿ:
	// During link, the vvp_island keeps these symbol tables for
	// mapping labels local to the island. When linking is done,
	// the compile_cleanup() method removes these tables.
      ಚಿಹ್ನೆ_map_s<vvp_net_t>*ports_;
      ಚಿಹ್ನೆ_map_s<vvp_island_branch>*anodes_;
      ಚಿಹ್ನೆ_map_s<vvp_island_branch>*bnodes_;
};

/*
* An island port is a functor that connects to the ddiscrete
* discipline outside the island. (There is also a vvp_net_t object
* that refers to this port.) When data comes to the port from outside,
* it is collected and saved, and the island is notified. When code
* inside the island sends data out of the island, it uses the "out"
* pointer from the vvp_net_t that refers to this object.
*/

class vvp_island_ಪೋರ್ಟ್  : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_island_ಪೋರ್ಟ್(vvp_island*ip);
      ~vvp_island_ಪೋರ್ಟ್() override;

    ಸಾರ್ವಜನಿಕ: // Implement vvp_net_fun_t methods
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                             vvp_context_t) override;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t) override;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ recv_vec8(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್) override;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ recv_vec8_pv(vvp_net_ptr_t p, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್,
				ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid) override;

	// This is painful, but necessary. If the island is connected
	// to a forced net, we need to rerun the calculations whenever
	// a force/release happens to the net. If run_now is true, we
	// rerun immediately, otherwise we schedule it for the end of
	// the current time slot.
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ force_flag(bool ಸತತ_now) override;

    ಸಾರ್ವಜನಿಕ:
      vvp_vector8_t invalue;
      vvp_vector8_t outvalue;
      vvp_vector8_t ಮೌಲ್ಯ;

    ಖಾಸಗಿ:
      vvp_island*island_;

    ಸಾರ್ವಜನಿಕ: // Support for $countdrivers.
      ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಎಣಿಕೆ_drivers(ಚಿಹ್ನೆರಹಿತ ಬಿಟ್_idx, ಚಿಹ್ನೆರಹಿತ counts[3])
      {
            island_->ಎಣಿಕೆ_drivers(this, ಬಿಟ್_idx, counts);
      }

    ಖಾಸಗಿ: // not implemented
      vvp_island_ಪೋರ್ಟ್(ಸ್ಥಿರಾಂಕ vvp_island_ಪೋರ್ಟ್&);
      vvp_island_ಪೋರ್ಟ್& ಕ್ರಿಯಾಕಾರಕ = (ಸ್ಥಿರಾಂಕ vvp_island_ಪೋರ್ಟ್&);
};

ಸಾಲೊಳಗೆ vvp_vector8_t island_get_ಮೌಲ್ಯ(vvp_net_t*net)
{
      ಸ್ಥಿರಾಂಕ vvp_island_ಪೋರ್ಟ್*fun = dynamic_cast<vvp_island_ಪೋರ್ಟ್*>(net->fun);
      ಸ್ಥಿರಾಂಕ vvp_wire_vec8*fil = dynamic_cast<vvp_wire_vec8*>(net->fil);

      ಆದರೆ (fil == 0) {
	    ಹಿಂತಿರುಗಿ fun->invalue;
      } ಇಲ್ಲದಿದ್ದರೆ {
	      // This is painful, but necessary. If the island is
	      // connected to a forced net, then run the input through
	      // the force filter first. The island must used the
	      // forced value for its deliberations.
	    vvp_vector8_t rep;
	    ಆಯ್ಕೆ (fil->filter_ಒಳಹರಿವು_vec8(fun->invalue, rep)) {
		ಪೂರ್ವನಿಯೋಜಿತ:
		ಸ್ಥಿತಿ vvp_net_fil_t::PROP:
		  ಹಿಂತಿರುಗಿ fun->invalue;
		ಸ್ಥಿತಿ vvp_net_fil_t::REPL:
		  ಹಿಂತಿರುಗಿ rep;
	    }
      }
}

ಸಾಲೊಳಗೆ vvp_vector8_t island_get_ಕಳುಹಿಸಿದ_ಮೌಲ್ಯ(vvp_net_t*net)
{
      ಸ್ಥಿರಾಂಕ vvp_island_ಪೋರ್ಟ್*fun = dynamic_cast<vvp_island_ಪೋರ್ಟ್*>(net->fun);
      ಹಿಂತಿರುಗಿ fun->outvalue;
}

ಬಾಹ್ಯ ಶೂನ್ಯ island_send_ಮೌಲ್ಯ(vvp_net_t*net, ಸ್ಥಿರಾಂಕ vvp_vector8_t&val);

/*
* Branches are connected together to form a mesh of branches. Each
* endpoint (there are two) connects circularly to other branch
* endpoints that are connected together. This list of endpoints forms
* a node. Thus it is possible for branches to fully specify the mesh
* of the island.
*/

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ vvp_sub_ಸೂಚಕ_t<vvp_island_branch> vvp_branch_ptr_t;

ರಚನೆ vvp_island_branch {
      ವಾಸ್ತವಿಕ ~vvp_island_branch();
	// Keep a list of branches in the island.
      vvp_island_branch*ಮುಂದಿನ_branch;
	// branch mesh connectivity. There is a pointer for each end
	// that participates in a circular list.
      vvp_sub_ಸೂಚಕ_t<vvp_island_branch> link[2];
	// Port connections
      vvp_net_t*a;
      vvp_net_t*b;
};

ಸ್ಥಿರ ಸಾಲೊಳಗೆ vvp_branch_ptr_t ಮುಂದಿನ_2(vvp_branch_ptr_t cur)
{
      ಸ್ಥಿರಾಂಕ vvp_island_branch*ptr = cur.ptr();
      ಚಿಹ್ನೆರಹಿತ ab = cur.ಪೋರ್ಟ್();
      ಹಿಂತಿರುಗಿ ptr->link[ab];
}

/*
 * This function collects into the conn list all the branch ends
 * that are connected together with the reference branch endpoint
 * cur. All these branch ends together form a "node" of the branch
 * network. (In other words, this does *not* follow the branches.)
 */
ಬಾಹ್ಯ ಶೂನ್ಯ island_collect_ಗಂಟು(std::list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur);

/*
 * These functions support compile/linking.
 */
ಬಾಹ್ಯ ಶೂನ್ಯ compile_island_ತಳ(ಅಕ್ಷರ*label, vvp_island*use_island);
ಬಾಹ್ಯ vvp_island* compile_ಕಂಡುಹಿಡಿ_island(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*island_name);

#ಆದರೆ_ಅಂತ್ಯ /* IVL_vvp_island_H */
