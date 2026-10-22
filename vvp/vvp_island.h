#اگر_غیر_تعریف_شدہ IVL_vvp_island_H
#تعریف_کرو IVL_vvp_island_H
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

ساخت vvp_island_branch;
class vvp_island_پورٹ;

class vvp_island  : نجی vvp_gen_event_s {

    عوامی:
      vvp_island();
      مجازی ~vvp_island() override;

      vvp_island(ثابت vvp_island&) = delete;
      vvp_island& عامل=(ثابت vvp_island&) = delete;

	// Ports call this method to flag that something happened at
	// the input. The island will use this to create an active
	// event. The run_run() method will then be called by the
	// scheduler to process whatever happened.
      خالی flag_island();

	// This is the method that is called, eventually, to process
	// whatever happened. The derived island class implements this
	// method to give the island its character.
      مجازی خالی تسلسل_island() =0;

        // Support for $countdrivers.
      مجازی خالی گنتی_drivers(vvp_island_پورٹ*پورٹ, غیرعلامتی ثنائی_ہندسہ_idx,
                                 غیرعلامتی counts[3]) =0;

    محفوظ:
	// The base class collects a list of all the branches in the
	// island. The derived island class can access this list for
	// scanning the mesh.
      vvp_island_branch*branches_;

    عوامی: /* These methods are used during linking. */

	// Add a port to the island. The key is added to the island
	// ports symbol table.
      خالی جوڑو_پورٹ(ثابت حرف*کلید, vvp_net_t*net);
	// Add a branch to the island.
      خالی جوڑو_branch(vvp_island_branch*branch, ثابت حرف*pa, ثابت حرف*pb);

      vvp_net_t* ڈھونڈو_پورٹ(ثابت حرف*کلید);

	// Call this method when linking is done.
      خالی compile_cleanup(خالی);

    نجی:
      خالی تسلسل_تسلسل() override;
      bool flagged_;

    نجی:
	// During link, the vvp_island keeps these symbol tables for
	// mapping labels local to the island. When linking is done,
	// the compile_cleanup() method removes these tables.
      علامت_map_s<vvp_net_t>*ports_;
      علامت_map_s<vvp_island_branch>*anodes_;
      علامت_map_s<vvp_island_branch>*bnodes_;
};

/*
* An island port is a functor that connects to the ddiscrete
* discipline outside the island. (There is also a vvp_net_t object
* that refers to this port.) When data comes to the port from outside,
* it is collected and saved, and the island is notified. When code
* inside the island sends data out of the island, it uses the "out"
* pointer from the vvp_net_t that refers to this object.
*/

class vvp_island_پورٹ  : عوامی vvp_net_fun_t {

    عوامی:
      explicit vvp_island_پورٹ(vvp_island*ip);
      ~vvp_island_پورٹ() override;

    عوامی: // Implement vvp_net_fun_t methods
      مجازی خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                             vvp_context_t) override;
      مجازی خالی recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t) override;
      مجازی خالی recv_vec8(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ) override;
      مجازی خالی recv_vec8_pv(vvp_net_ptr_t p, ثابت vvp_vector8_t&ثنائی_ہندسہ,
				غیرعلامتی بنیاد, غیرعلامتی vwid) override;

	// This is painful, but necessary. If the island is connected
	// to a forced net, we need to rerun the calculations whenever
	// a force/release happens to the net. If run_now is true, we
	// rerun immediately, otherwise we schedule it for the end of
	// the current time slot.
      مجازی خالی force_flag(bool تسلسل_now) override;

    عوامی:
      vvp_vector8_t invalue;
      vvp_vector8_t outvalue;
      vvp_vector8_t قدر;

    نجی:
      vvp_island*island_;

    عوامی: // Support for $countdrivers.
      اندرونی خالی گنتی_drivers(غیرعلامتی ثنائی_ہندسہ_idx, غیرعلامتی counts[3])
      {
            island_->گنتی_drivers(this, ثنائی_ہندسہ_idx, counts);
      }

    نجی: // not implemented
      vvp_island_پورٹ(ثابت vvp_island_پورٹ&);
      vvp_island_پورٹ& عامل = (ثابت vvp_island_پورٹ&);
};

اندرونی vvp_vector8_t island_get_قدر(vvp_net_t*net)
{
      ثابت vvp_island_پورٹ*fun = dynamic_cast<vvp_island_پورٹ*>(net->fun);
      ثابت vvp_wire_vec8*fil = dynamic_cast<vvp_wire_vec8*>(net->fil);

      اگر (fil == 0) {
	    واپس fun->invalue;
      } ورنہ {
	      // This is painful, but necessary. If the island is
	      // connected to a forced net, then run the input through
	      // the force filter first. The island must used the
	      // forced value for its deliberations.
	    vvp_vector8_t rep;
	    انتخاب (fil->filter_داخلہ_vec8(fun->invalue, rep)) {
		طےشدہ:
		حالت vvp_net_fil_t::PROP:
		  واپس fun->invalue;
		حالت vvp_net_fil_t::REPL:
		  واپس rep;
	    }
      }
}

اندرونی vvp_vector8_t island_get_بھیجا_قدر(vvp_net_t*net)
{
      ثابت vvp_island_پورٹ*fun = dynamic_cast<vvp_island_پورٹ*>(net->fun);
      واپس fun->outvalue;
}

خارجی خالی island_send_قدر(vvp_net_t*net, ثابت vvp_vector8_t&val);

/*
* Branches are connected together to form a mesh of branches. Each
* endpoint (there are two) connects circularly to other branch
* endpoints that are connected together. This list of endpoints forms
* a node. Thus it is possible for branches to fully specify the mesh
* of the island.
*/

قسمتعریف vvp_sub_اشارہ_گر_t<vvp_island_branch> vvp_branch_ptr_t;

ساخت vvp_island_branch {
      مجازی ~vvp_island_branch();
	// Keep a list of branches in the island.
      vvp_island_branch*اگلا_branch;
	// branch mesh connectivity. There is a pointer for each end
	// that participates in a circular list.
      vvp_sub_اشارہ_گر_t<vvp_island_branch> link[2];
	// Port connections
      vvp_net_t*a;
      vvp_net_t*b;
};

ساکن اندرونی vvp_branch_ptr_t اگلا_2(vvp_branch_ptr_t cur)
{
      ثابت vvp_island_branch*ptr = cur.ptr();
      غیرعلامتی ab = cur.پورٹ();
      واپس ptr->link[ab];
}

/*
 * This function collects into the conn list all the branch ends
 * that are connected together with the reference branch endpoint
 * cur. All these branch ends together form a "node" of the branch
 * network. (In other words, this does *not* follow the branches.)
 */
خارجی خالی island_collect_گرہ(std::list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur);

/*
 * These functions support compile/linking.
 */
خارجی خالی compile_island_بنیاد(حرف*label, vvp_island*use_island);
خارجی vvp_island* compile_ڈھونڈو_island(ثابت حرف*island_name);

#اگر_ختم /* IVL_vvp_island_H */
