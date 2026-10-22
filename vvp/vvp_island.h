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

ഘടന vvp_island_branch;
class vvp_island_port;

class vvp_island  : സ്വകാര്യ vvp_gen_event_s {

    പൊതു:
      vvp_island();
      മായാ ~vvp_island() override;

      vvp_island(സ്ഥിരം vvp_island&) = delete;
      vvp_island& operator=(സ്ഥിരം vvp_island&) = delete;

	// Ports call this method to flag that something happened at
	// the input. The island will use this to create an active
	// event. The run_run() method will then be called by the
	// scheduler to process whatever happened.
      ശൂന്യം flag_island();

	// This is the method that is called, eventually, to process
	// whatever happened. The derived island class implements this
	// method to give the island its character.
      മായാ ശൂന്യം run_island() =0;

        // Support for $countdrivers.
      മായാ ശൂന്യം എണ്ണം_drivers(vvp_island_port*port, unsigned bit_idx,
                                 unsigned counts[3]) =0;

    സംരക്ഷിത:
	// The base class collects a list of all the branches in the
	// island. The derived island class can access this list for
	// scanning the mesh.
      vvp_island_branch*branches_;

    പൊതു: /* These methods are used during linking. */

	// Add a port to the island. The key is added to the island
	// ports symbol table.
      ശൂന്യം add_port(സ്ഥിരം അക്ഷരം*താക്കോൽ, vvp_net_t*net);
	// Add a branch to the island.
      ശൂന്യം add_branch(vvp_island_branch*branch, സ്ഥിരം അക്ഷരം*pa, സ്ഥിരം അക്ഷരം*pb);

      vvp_net_t* find_port(സ്ഥിരം അക്ഷരം*താക്കോൽ);

	// Call this method when linking is done.
      ശൂന്യം compile_cleanup(ശൂന്യം);

    സ്വകാര്യ:
      ശൂന്യം run_run() override;
      bool flagged_;

    സ്വകാര്യ:
	// During link, the vvp_island keeps these symbol tables for
	// mapping labels local to the island. When linking is done,
	// the compile_cleanup() method removes these tables.
      ചിഹ്നം_map_s<vvp_net_t>*ports_;
      ചിഹ്നം_map_s<vvp_island_branch>*anodes_;
      ചിഹ്നം_map_s<vvp_island_branch>*bnodes_;
};

/*
* An island port is a functor that connects to the ddiscrete
* discipline outside the island. (There is also a vvp_net_t object
* that refers to this port.) When data comes to the port from outside,
* it is collected and saved, and the island is notified. When code
* inside the island sends data out of the island, it uses the "out"
* pointer from the vvp_net_t that refers to this object.
*/

class vvp_island_port  : പൊതു vvp_net_fun_t {

    പൊതു:
      explicit vvp_island_port(vvp_island*ip);
      ~vvp_island_port() override;

    പൊതു: // Implement vvp_net_fun_t methods
      മായാ ശൂന്യം recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                             vvp_context_t) override;
      മായാ ശൂന്യം recv_vec4_pv(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
				unsigned base, unsigned vwid, vvp_context_t) override;
      മായാ ശൂന്യം recv_vec8(vvp_net_ptr_t port, സ്ഥിരം vvp_vector8_t&bit) override;
      മായാ ശൂന്യം recv_vec8_pv(vvp_net_ptr_t p, സ്ഥിരം vvp_vector8_t&bit,
				unsigned base, unsigned vwid) override;

	// This is painful, but necessary. If the island is connected
	// to a forced net, we need to rerun the calculations whenever
	// a force/release happens to the net. If run_now is true, we
	// rerun immediately, otherwise we schedule it for the end of
	// the current time slot.
      മായാ ശൂന്യം force_flag(bool run_now) override;

    പൊതു:
      vvp_vector8_t invalue;
      vvp_vector8_t outvalue;
      vvp_vector8_t മൂല്യം;

    സ്വകാര്യ:
      vvp_island*island_;

    പൊതു: // Support for $countdrivers.
      വരിയിൽ ശൂന്യം എണ്ണം_drivers(unsigned bit_idx, unsigned counts[3])
      {
            island_->എണ്ണം_drivers(this, bit_idx, counts);
      }

    സ്വകാര്യ: // not implemented
      vvp_island_port(സ്ഥിരം vvp_island_port&);
      vvp_island_port& operator = (സ്ഥിരം vvp_island_port&);
};

വരിയിൽ vvp_vector8_t island_get_മൂല്യം(vvp_net_t*net)
{
      സ്ഥിരം vvp_island_port*fun = dynamic_cast<vvp_island_port*>(net->fun);
      സ്ഥിരം vvp_wire_vec8*fil = dynamic_cast<vvp_wire_vec8*>(net->fil);

      എങ്കിൽ (fil == 0) {
	    തിരികെ fun->invalue;
      } അല്ലെങ്കിൽ {
	      // This is painful, but necessary. If the island is
	      // connected to a forced net, then run the input through
	      // the force filter first. The island must used the
	      // forced value for its deliberations.
	    vvp_vector8_t rep;
	    തിരഞ്ഞെടുപ്പ് (fil->filter_നിവേശം_vec8(fun->invalue, rep)) {
		സ്ഥിരസ്ഥിതി:
		അവസ്ഥ vvp_net_fil_t::PROP:
		  തിരികെ fun->invalue;
		അവസ്ഥ vvp_net_fil_t::REPL:
		  തിരികെ rep;
	    }
      }
}

വരിയിൽ vvp_vector8_t island_get_sent_മൂല്യം(vvp_net_t*net)
{
      സ്ഥിരം vvp_island_port*fun = dynamic_cast<vvp_island_port*>(net->fun);
      തിരികെ fun->outvalue;
}

ബാഹ്യ ശൂന്യം island_send_മൂല്യം(vvp_net_t*net, സ്ഥിരം vvp_vector8_t&val);

/*
* Branches are connected together to form a mesh of branches. Each
* endpoint (there are two) connects circularly to other branch
* endpoints that are connected together. This list of endpoints forms
* a node. Thus it is possible for branches to fully specify the mesh
* of the island.
*/

തരനിർവചനം vvp_sub_pointer_t<vvp_island_branch> vvp_branch_ptr_t;

ഘടന vvp_island_branch {
      മായാ ~vvp_island_branch();
	// Keep a list of branches in the island.
      vvp_island_branch*next_branch;
	// branch mesh connectivity. There is a pointer for each end
	// that participates in a circular list.
      vvp_sub_pointer_t<vvp_island_branch> link[2];
	// Port connections
      vvp_net_t*a;
      vvp_net_t*b;
};

സ്ഥിതം വരിയിൽ vvp_branch_ptr_t next(vvp_branch_ptr_t cur)
{
      സ്ഥിരം vvp_island_branch*ptr = cur.ptr();
      unsigned ab = cur.port();
      തിരികെ ptr->link[ab];
}

/*
 * This function collects into the conn list all the branch ends
 * that are connected together with the reference branch endpoint
 * cur. All these branch ends together form a "node" of the branch
 * network. (In other words, this does *not* follow the branches.)
 */
ബാഹ്യ ശൂന്യം island_collect_കണ്ണി(std::list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur);

/*
 * These functions support compile/linking.
 */
ബാഹ്യ ശൂന്യം compile_island_base(അക്ഷരം*label, vvp_island*use_island);
ബാഹ്യ vvp_island* compile_find_island(സ്ഥിരം അക്ഷരം*island_name);

#endif /* IVL_vvp_island_H */
