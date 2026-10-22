#ifndef IVL_vvp_island_H
#define IVL_vvp_island_H
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

ସଂରଚନା vvp_island_branch;
class vvp_island_port;

class vvp_island  : ବ୍ୟକ୍ତିଗତ vvp_gen_event_s {

    ସର୍ବସାଧାରଣ:
      vvp_island();
      ଆଭାସୀ ~vvp_island() override;

      vvp_island(ଅଚଳ vvp_island&) = delete;
      vvp_island& operator=(ଅଚଳ vvp_island&) = delete;

	// Ports call this method to flag that something happened at
	// the input. The island will use this to create an active
	// event. The run_run() method will then be called by the
	// scheduler to process whatever happened.
      ଶୂନ୍ୟ flag_island();

	// This is the method that is called, eventually, to process
	// whatever happened. The derived island class implements this
	// method to give the island its character.
      ଆଭାସୀ ଶୂନ୍ୟ run_island() =0;

        // Support for $countdrivers.
      ଆଭାସୀ ଶୂନ୍ୟ ଗଣନା_drivers(vvp_island_port*port, unsigned bit_idx,
                                 unsigned counts[3]) =0;

    ସୁରକ୍ଷିତ:
	// The base class collects a list of all the branches in the
	// island. The derived island class can access this list for
	// scanning the mesh.
      vvp_island_branch*branches_;

    ସର୍ବସାଧାରଣ: /* These methods are used during linking. */

	// Add a port to the island. The key is added to the island
	// ports symbol table.
      ଶୂନ୍ୟ add_port(ଅଚଳ ଅକ୍ଷର*ଚାବି, vvp_net_t*net);
	// Add a branch to the island.
      ଶୂନ୍ୟ add_branch(vvp_island_branch*branch, ଅଚଳ ଅକ୍ଷର*pa, ଅଚଳ ଅକ୍ଷର*pb);

      vvp_net_t* find_port(ଅଚଳ ଅକ୍ଷର*ଚାବି);

	// Call this method when linking is done.
      ଶୂନ୍ୟ compile_cleanup(ଶୂନ୍ୟ);

    ବ୍ୟକ୍ତିଗତ:
      ଶୂନ୍ୟ run_run() override;
      bool flagged_;

    ବ୍ୟକ୍ତିଗତ:
	// During link, the vvp_island keeps these symbol tables for
	// mapping labels local to the island. When linking is done,
	// the compile_cleanup() method removes these tables.
      ପ୍ରତୀକ_map_s<vvp_net_t>*ports_;
      ପ୍ରତୀକ_map_s<vvp_island_branch>*anodes_;
      ପ୍ରତୀକ_map_s<vvp_island_branch>*bnodes_;
};

/*
* An island port is a functor that connects to the ddiscrete
* discipline outside the island. (There is also a vvp_net_t object
* that refers to this port.) When data comes to the port from outside,
* it is collected and saved, and the island is notified. When code
* inside the island sends data out of the island, it uses the "out"
* pointer from the vvp_net_t that refers to this object.
*/

class vvp_island_port  : ସର୍ବସାଧାରଣ vvp_net_fun_t {

    ସର୍ବସାଧାରଣ:
      explicit vvp_island_port(vvp_island*ip);
      ~vvp_island_port() override;

    ସର୍ବସାଧାରଣ: // Implement vvp_net_fun_t methods
      ଆଭାସୀ ଶୂନ୍ୟ recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                             vvp_context_t) override;
      ଆଭାସୀ ଶୂନ୍ୟ recv_vec4_pv(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
				unsigned base, unsigned vwid, vvp_context_t) override;
      ଆଭାସୀ ଶୂନ୍ୟ recv_vec8(vvp_net_ptr_t port, ଅଚଳ vvp_vector8_t&bit) override;
      ଆଭାସୀ ଶୂନ୍ୟ recv_vec8_pv(vvp_net_ptr_t p, ଅଚଳ vvp_vector8_t&bit,
				unsigned base, unsigned vwid) override;

	// This is painful, but necessary. If the island is connected
	// to a forced net, we need to rerun the calculations whenever
	// a force/release happens to the net. If run_now is true, we
	// rerun immediately, otherwise we schedule it for the end of
	// the current time slot.
      ଆଭାସୀ ଶୂନ୍ୟ force_flag(bool run_now) override;

    ସର୍ବସାଧାରଣ:
      vvp_vector8_t invalue;
      vvp_vector8_t outvalue;
      vvp_vector8_t ମୂଲ୍ୟ;

    ବ୍ୟକ୍ତିଗତ:
      vvp_island*island_;

    ସର୍ବସାଧାରଣ: // Support for $countdrivers.
      ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ ଗଣନା_drivers(unsigned bit_idx, unsigned counts[3])
      {
            island_->ଗଣନା_drivers(this, bit_idx, counts);
      }

    ବ୍ୟକ୍ତିଗତ: // not implemented
      vvp_island_port(ଅଚଳ vvp_island_port&);
      vvp_island_port& operator = (ଅଚଳ vvp_island_port&);
};

ଧାଡ଼ିଭିତରେ vvp_vector8_t island_get_ମୂଲ୍ୟ(vvp_net_t*net)
{
      ଅଚଳ vvp_island_port*fun = dynamic_cast<vvp_island_port*>(net->fun);
      ଅଚଳ vvp_wire_vec8*fil = dynamic_cast<vvp_wire_vec8*>(net->fil);

      ଯଦି (fil == 0) {
	    ଫେରନ୍ତୁ fun->invalue;
      } ଅନ୍ୟଥା {
	      // This is painful, but necessary. If the island is
	      // connected to a forced net, then run the input through
	      // the force filter first. The island must used the
	      // forced value for its deliberations.
	    vvp_vector8_t rep;
	    ବାଛ (fil->filter_ନିବେଶ_vec8(fun->invalue, rep)) {
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ସ୍ଥିତି vvp_net_fil_t::PROP:
		  ଫେରନ୍ତୁ fun->invalue;
		ସ୍ଥିତି vvp_net_fil_t::REPL:
		  ଫେରନ୍ତୁ rep;
	    }
      }
}

ଧାଡ଼ିଭିତରେ vvp_vector8_t island_get_sent_ମୂଲ୍ୟ(vvp_net_t*net)
{
      ଅଚଳ vvp_island_port*fun = dynamic_cast<vvp_island_port*>(net->fun);
      ଫେରନ୍ତୁ fun->outvalue;
}

ବାହ୍ୟ ଶୂନ୍ୟ island_send_ମୂଲ୍ୟ(vvp_net_t*net, ଅଚଳ vvp_vector8_t&val);

/*
* Branches are connected together to form a mesh of branches. Each
* endpoint (there are two) connects circularly to other branch
* endpoints that are connected together. This list of endpoints forms
* a node. Thus it is possible for branches to fully specify the mesh
* of the island.
*/

ପ୍ରକାରବ୍ୟାଖ୍ୟା vvp_sub_pointer_t<vvp_island_branch> vvp_branch_ptr_t;

ସଂରଚନା vvp_island_branch {
      ଆଭାସୀ ~vvp_island_branch();
	// Keep a list of branches in the island.
      vvp_island_branch*next_branch;
	// branch mesh connectivity. There is a pointer for each end
	// that participates in a circular list.
      vvp_sub_pointer_t<vvp_island_branch> link[2];
	// Port connections
      vvp_net_t*a;
      vvp_net_t*b;
};

ସ୍ଥିର ଧାଡ଼ିଭିତରେ vvp_branch_ptr_t next(vvp_branch_ptr_t cur)
{
      ଅଚଳ vvp_island_branch*ptr = cur.ptr();
      unsigned ab = cur.port();
      ଫେରନ୍ତୁ ptr->link[ab];
}

/*
 * This function collects into the conn list all the branch ends
 * that are connected together with the reference branch endpoint
 * cur. All these branch ends together form a "node" of the branch
 * network. (In other words, this does *not* follow the branches.)
 */
ବାହ୍ୟ ଶୂନ୍ୟ island_collect_ଗଣ୍ଠି(std::list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur);

/*
 * These functions support compile/linking.
 */
ବାହ୍ୟ ଶୂନ୍ୟ compile_island_base(ଅକ୍ଷର*label, vvp_island*use_island);
ବାହ୍ୟ vvp_island* compile_find_island(ଅଚଳ ଅକ୍ଷର*island_name);

#endif /* IVL_vvp_island_H */
