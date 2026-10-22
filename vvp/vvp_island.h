#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_vvp_island_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_vvp_island_H
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

ਬਣਤਰ vvp_island_branch;
class vvp_island_ਪੋਰਟ;

class vvp_island  : ਨਿਜੀ vvp_gen_event_s {

    ਜਨਤਕ:
      vvp_island();
      ਆਭਾਸੀ ~vvp_island() override;

      vvp_island(ਅਚਲ vvp_island&) = delete;
      vvp_island& ਸੰਚਾਲਕ=(ਅਚਲ vvp_island&) = delete;

	// Ports call this method to flag that something happened at
	// the input. The island will use this to create an active
	// event. The run_run() method will then be called by the
	// scheduler to process whatever happened.
      ਖਾਲੀ flag_island();

	// This is the method that is called, eventually, to process
	// whatever happened. The derived island class implements this
	// method to give the island its character.
      ਆਭਾਸੀ ਖਾਲੀ ਲੜੀਵਾਰ_island() =0;

        // Support for $countdrivers.
      ਆਭਾਸੀ ਖਾਲੀ ਗਿਣਤੀ_drivers(vvp_island_ਪੋਰਟ*ਪੋਰਟ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ_idx,
                                 ਬਿਨਾਨਿਸ਼ਾਨੀ counts[3]) =0;

    ਸੁਰੱਖਿਅਤ:
	// The base class collects a list of all the branches in the
	// island. The derived island class can access this list for
	// scanning the mesh.
      vvp_island_branch*branches_;

    ਜਨਤਕ: /* These methods are used during linking. */

	// Add a port to the island. The key is added to the island
	// ports symbol table.
      ਖਾਲੀ ਜੋੜੋ_ਪੋਰਟ(ਅਚਲ ਅੱਖਰ*ਕੁੰਜੀ, vvp_net_t*net);
	// Add a branch to the island.
      ਖਾਲੀ ਜੋੜੋ_branch(vvp_island_branch*branch, ਅਚਲ ਅੱਖਰ*pa, ਅਚਲ ਅੱਖਰ*pb);

      vvp_net_t* ਲੱਭੋ_ਪੋਰਟ(ਅਚਲ ਅੱਖਰ*ਕੁੰਜੀ);

	// Call this method when linking is done.
      ਖਾਲੀ compile_cleanup(ਖਾਲੀ);

    ਨਿਜੀ:
      ਖਾਲੀ ਲੜੀਵਾਰ_ਲੜੀਵਾਰ() override;
      bool flagged_;

    ਨਿਜੀ:
	// During link, the vvp_island keeps these symbol tables for
	// mapping labels local to the island. When linking is done,
	// the compile_cleanup() method removes these tables.
      ਚਿੰਨ੍ਹ_map_s<vvp_net_t>*ports_;
      ਚਿੰਨ੍ਹ_map_s<vvp_island_branch>*anodes_;
      ਚਿੰਨ੍ਹ_map_s<vvp_island_branch>*bnodes_;
};

/*
* An island port is a functor that connects to the ddiscrete
* discipline outside the island. (There is also a vvp_net_t object
* that refers to this port.) When data comes to the port from outside,
* it is collected and saved, and the island is notified. When code
* inside the island sends data out of the island, it uses the "out"
* pointer from the vvp_net_t that refers to this object.
*/

class vvp_island_ਪੋਰਟ  : ਜਨਤਕ vvp_net_fun_t {

    ਜਨਤਕ:
      explicit vvp_island_ਪੋਰਟ(vvp_island*ip);
      ~vvp_island_ਪੋਰਟ() override;

    ਜਨਤਕ: // Implement vvp_net_fun_t methods
      ਆਭਾਸੀ ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                             vvp_context_t) override;
      ਆਭਾਸੀ ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t) override;
      ਆਭਾਸੀ ਖਾਲੀ recv_vec8(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ) override;
      ਆਭਾਸੀ ਖਾਲੀ recv_vec8_pv(vvp_net_ptr_t p, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid) override;

	// This is painful, but necessary. If the island is connected
	// to a forced net, we need to rerun the calculations whenever
	// a force/release happens to the net. If run_now is true, we
	// rerun immediately, otherwise we schedule it for the end of
	// the current time slot.
      ਆਭਾਸੀ ਖਾਲੀ force_flag(bool ਲੜੀਵਾਰ_now) override;

    ਜਨਤਕ:
      vvp_vector8_t invalue;
      vvp_vector8_t outvalue;
      vvp_vector8_t ਮੁੱਲ;

    ਨਿਜੀ:
      vvp_island*island_;

    ਜਨਤਕ: // Support for $countdrivers.
      ਸਤਰਅੰਦਰ ਖਾਲੀ ਗਿਣਤੀ_drivers(ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ_idx, ਬਿਨਾਨਿਸ਼ਾਨੀ counts[3])
      {
            island_->ਗਿਣਤੀ_drivers(this, ਦੋਆਧਾਰੀ_ਅੰਕ_idx, counts);
      }

    ਨਿਜੀ: // not implemented
      vvp_island_ਪੋਰਟ(ਅਚਲ vvp_island_ਪੋਰਟ&);
      vvp_island_ਪੋਰਟ& ਸੰਚਾਲਕ = (ਅਚਲ vvp_island_ਪੋਰਟ&);
};

ਸਤਰਅੰਦਰ vvp_vector8_t island_get_ਮੁੱਲ(vvp_net_t*net)
{
      ਅਚਲ vvp_island_ਪੋਰਟ*fun = dynamic_cast<vvp_island_ਪੋਰਟ*>(net->fun);
      ਅਚਲ vvp_wire_vec8*fil = dynamic_cast<vvp_wire_vec8*>(net->fil);

      ਜੇ (fil == 0) {
	    ਵਾਪਸ fun->invalue;
      } ਨਹੀਂਤਾਂ {
	      // This is painful, but necessary. If the island is
	      // connected to a forced net, then run the input through
	      // the force filter first. The island must used the
	      // forced value for its deliberations.
	    vvp_vector8_t rep;
	    ਚੋਣ (fil->filter_ਨਿਵੇਸ਼_vec8(fun->invalue, rep)) {
		ਮੂਲ:
		ਹਾਲਤ vvp_net_fil_t::PROP:
		  ਵਾਪਸ fun->invalue;
		ਹਾਲਤ vvp_net_fil_t::REPL:
		  ਵਾਪਸ rep;
	    }
      }
}

ਸਤਰਅੰਦਰ vvp_vector8_t island_get_ਭੇਜਿਆ_ਮੁੱਲ(vvp_net_t*net)
{
      ਅਚਲ vvp_island_ਪੋਰਟ*fun = dynamic_cast<vvp_island_ਪੋਰਟ*>(net->fun);
      ਵਾਪਸ fun->outvalue;
}

ਬਾਹਰੀ ਖਾਲੀ island_send_ਮੁੱਲ(vvp_net_t*net, ਅਚਲ vvp_vector8_t&val);

/*
* Branches are connected together to form a mesh of branches. Each
* endpoint (there are two) connects circularly to other branch
* endpoints that are connected together. This list of endpoints forms
* a node. Thus it is possible for branches to fully specify the mesh
* of the island.
*/

ਕਿਸਮਪਰਿਭਾਸ਼ਾ vvp_sub_ਸੂਚਕ_t<vvp_island_branch> vvp_branch_ptr_t;

ਬਣਤਰ vvp_island_branch {
      ਆਭਾਸੀ ~vvp_island_branch();
	// Keep a list of branches in the island.
      vvp_island_branch*ਅਗਲਾ_branch;
	// branch mesh connectivity. There is a pointer for each end
	// that participates in a circular list.
      vvp_sub_ਸੂਚਕ_t<vvp_island_branch> link[2];
	// Port connections
      vvp_net_t*a;
      vvp_net_t*b;
};

ਸਥਿਰ ਸਤਰਅੰਦਰ vvp_branch_ptr_t ਅਗਲਾ_2(vvp_branch_ptr_t cur)
{
      ਅਚਲ vvp_island_branch*ptr = cur.ptr();
      ਬਿਨਾਨਿਸ਼ਾਨੀ ab = cur.ਪੋਰਟ();
      ਵਾਪਸ ptr->link[ab];
}

/*
 * This function collects into the conn list all the branch ends
 * that are connected together with the reference branch endpoint
 * cur. All these branch ends together form a "node" of the branch
 * network. (In other words, this does *not* follow the branches.)
 */
ਬਾਹਰੀ ਖਾਲੀ island_collect_ਗੰਢ(std::list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur);

/*
 * These functions support compile/linking.
 */
ਬਾਹਰੀ ਖਾਲੀ compile_island_ਅਧਾਰ(ਅੱਖਰ*label, vvp_island*use_island);
ਬਾਹਰੀ vvp_island* compile_ਲੱਭੋ_island(ਅਚਲ ਅੱਖਰ*island_name);

#ਜੇ_ਅੰਤ /* IVL_vvp_island_H */
