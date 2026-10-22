/*
 * Copyright (c) 2008-2025 Stephen Williams (steve@icarus.com)
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

# include  "vvp_island.h"
# include  "compile.h"
# include  "symbols.h"
# include  "schedule.h"
# include  "config.h"
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif
# include  <iostream>
# include  <list>
# include  <cassert>
# include  <cstdlib>
# include  <cstring>
# include "ivl_alloc.h"

using ನಾಮಾಕಾಶ std;

#ifdef CHECK_WITH_VALGRIND
ಸ್ಥಿರ ತಾರ್ಕಿಕ at_EOS = ಅಸತ್ಯ;
#endif

ಶೂನ್ಯ island_send_ಮೌಲ್ಯ(vvp_net_t*net, ಧ್ರುವ vvp_vector8_t&val)
{
      vvp_island_port*fun = dynamic_cast<vvp_island_port*>(net->fun);
      ಯದಿ (fun->outvalue .eeq(val))
	    ಪ್ರತ್ಯಾಗಮ;

      fun->outvalue = val;
      net->send_vec8(fun->outvalue);
}

/*
 * Implementations...
 */
vvp_island::vvp_island()
{
      flagged_ = ಅಸತ್ಯ;
      branches_ = 0;
      ports_ = 0;
      anodes_ = 0;
      bnodes_ = 0;
}

vvp_island::~vvp_island()
{
	// We can only delete islands at the end of simulation.
#ifdef CHECK_WITH_VALGRIND
      ಯದಿ (!at_EOS) ಪ್ರತಿಜ್ಞಾ(0);
#endif

      ಯಾವತ್ (branches_) {
	    vvp_island_branch *next_br = branches_->next_branch;
	    delete branches_;
	    branches_ = next_br;
      }
}

ಶೂನ್ಯ vvp_island::flag_island()
{
      ಯದಿ (flagged_ == ಸತ್ಯ)
	    ಪ್ರತ್ಯಾಗಮ;

      schedule_generic(ಏತತ್, 0, ಅಸತ್ಯ, ಅಸತ್ಯ);
      flagged_ = ಸತ್ಯ;
}

/*
* This method handles the callback from the scheduler. It does basic
* housecleaning and calls the run_island() method implemented by the
* derived class.
*/
ಶೂನ್ಯ vvp_island::run_run()
{
      flagged_ = ಅಸತ್ಯ;
      run_island();
}


ಶೂನ್ಯ vvp_island::add_port(ಧ್ರುವ ವರ್ಣ*ಕೀಲಿ, vvp_net_t*net)
{
      ಯದಿ (ports_ == 0)
	    ports_ = ನೂತನ ಚಿಹ್ನೆ_map_s<vvp_net_t>;

	// each port should have a unique label
      ಪ್ರತಿಜ್ಞಾ(ports_->sym_get_ಮೌಲ್ಯ(ಕೀಲಿ) == 0);

      ports_->sym_set_ಮೌಲ್ಯ(ಕೀಲಿ, net);
}

ಶೂನ್ಯ vvp_island::add_branch(vvp_island_branch*branch, ಧ್ರುವ ವರ್ಣ*pa, ಧ್ರುವ ವರ್ಣ*pb)
{
      vvp_island_branch*cur;
      ಪ್ರತಿಜ್ಞಾ(ports_);
      branch->a = ports_->sym_get_ಮೌಲ್ಯ(pa);
      branch->b = ports_->sym_get_ಮೌಲ್ಯ(pb);
      ಪ್ರತಿಜ್ಞಾ(branch->a && branch->b);

      vvp_branch_ptr_t ptra (branch, 0);
      vvp_branch_ptr_t ptrb (branch, 1);
      ಯದಿ (anodes_ == 0)
	    anodes_ = ನೂತನ ಚಿಹ್ನೆ_map_s<vvp_island_branch>;
      ಯದಿ (bnodes_ == 0)
	    bnodes_ = ನೂತನ ಚಿಹ್ನೆ_map_s<vvp_island_branch>;

      ಯದಿ ((cur = anodes_->sym_get_ಮೌಲ್ಯ(pa))) {
	    branch->link[0] = cur->link[0];
	    cur->link[0] = ptra;
      } ಅನ್ಯಥಾ ಯದಿ ((cur = bnodes_->sym_get_ಮೌಲ್ಯ(pa))) {
	    branch->link[0] = cur->link[1];
	    cur->link[1] = ptra;
      } ಅನ್ಯಥಾ {
	    branch->link[0] = ptra;
	    anodes_->sym_set_ಮೌಲ್ಯ(pa, branch);
      }

      ಯದಿ ((cur = anodes_->sym_get_ಮೌಲ್ಯ(pb))) {
	    branch->link[1] = cur->link[0];
	    cur->link[0] = ptrb;
      } ಅನ್ಯಥಾ ಯದಿ ((cur = bnodes_->sym_get_ಮೌಲ್ಯ(pb))) {
	    branch->link[1] = cur->link[1];
	    cur->link[1] = ptrb;
      } ಅನ್ಯಥಾ {
	    branch->link[1] = ptrb;
	    bnodes_->sym_set_ಮೌಲ್ಯ(pb, branch);
      }

      branch->next_branch = branches_;
      branches_ = branch;
}

vvp_net_t* vvp_island::find_port(ಧ್ರುವ ವರ್ಣ*ಕೀಲಿ)
{
      ಯದಿ (ports_ == 0)
	    ಪ್ರತ್ಯಾಗಮ 0;
      ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ ports_->sym_get_ಮೌಲ್ಯ(ಕೀಲಿ);
}

ಶೂನ್ಯ vvp_island::compile_cleanup()
{
      delete ports_;
      ports_ = 0;

      delete anodes_;
      anodes_ = 0;

      delete bnodes_;
      bnodes_ = 0;
}

vvp_island_port::vvp_island_port(vvp_island*ip)
: island_(ip)
{
}

vvp_island_port::~vvp_island_port()
{
}

ಶೂನ್ಯ vvp_island_port::recv_vec4(vvp_net_ptr_t, ಧ್ರುವ vvp_vector4_t&bit,
                                vvp_context_t)
{
      vvp_vector8_t tmp (bit, 6, 6);
      ಯದಿ (invalue .eeq(tmp))
	    ಪ್ರತ್ಯಾಗಮ;

      invalue = tmp;
      island_->flag_island();
}

ಶೂನ್ಯ vvp_island_port::recv_vec4_pv(vvp_net_ptr_t port, ಧ್ರುವ vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector8_t tmp(bit, 6, 6);
      recv_vec8_pv(port, tmp, base, vwid);
}


ಶೂನ್ಯ vvp_island_port::recv_vec8(vvp_net_ptr_t, ಧ್ರುವ vvp_vector8_t&bit)
{
      ಯದಿ (invalue .eeq(bit))
	    ಪ್ರತ್ಯಾಗಮ;

      invalue = bit;
      island_->flag_island();
}

ಶೂನ್ಯ vvp_island_port::recv_vec8_pv(vvp_net_ptr_t, ಧ್ರುವ vvp_vector8_t&bit,
                                   unsigned base, unsigned vwid)
{
      ಯದಿ (invalue.size() == 0) {
	    invalue = part_expand(bit, vwid, base);
      } ಅನ್ಯಥಾ {
	    unsigned wid = bit.size();
	    ಪ್ರತಿಜ್ಞಾ(invalue.size() == vwid);
	    ಪ್ರತಿ (unsigned idx = 0; idx < wid ; idx += 1) {
		  ಯದಿ ((base+idx) >= vwid)
			ಭಙ್ಗ;
		  invalue.set_bit(base+idx, bit.value(idx));
	    }
      }

      island_->flag_island();
}

ಶೂನ್ಯ vvp_island_port::force_flag(ತಾರ್ಕಿಕ run_now)
{
      ಯದಿ (run_now)
	    island_->run_island();
      ಅನ್ಯಥಾ
	    island_->flag_island();
}

vvp_island_branch::~vvp_island_branch()
{
}

ಶೂನ್ಯ island_collect_ಗಂಟು(list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur)
{
      conn .push_back(cur);
      ಪ್ರತಿ (vvp_branch_ptr_t idx = next(cur) ; idx != cur ; idx = next(idx))
	    conn.push_back(idx);
}

/* **** COMPILE/LINK SUPPORT **** */

/*
* We need to keep an island symbol table to make island labels to
* islands, and we need a list of the islands that we can run through
* during cleanup. After linking is done, the compile_island_cleanup() is
* called to erase the symbol table, we still need the list to cleanup the
* island memory at EOS.
*/
ಸ್ಥಿರ ಚಿಹ್ನೆ_map_s<vvp_island>* island_table = 0;
ಸ್ಥಿರ vvp_island** island_list = 0;
ಸ್ಥಿರ unsigned island_ಎಣಿಕೆ = 0;

#ifdef CHECK_WITH_VALGRIND
ಶೂನ್ಯ island_delete()
{
      at_EOS = ಸತ್ಯ;
      ಪ್ರತಿ (unsigned idx = 0; idx < island_ಎಣಿಕೆ; idx += 1) {
	    delete island_list[idx];
      }
      ಮುಕ್ತ(island_list);
      island_list = 0;
      island_ಎಣಿಕೆ = 0;
}
#endif

ಶೂನ್ಯ compile_island_base(ವರ್ಣ*label, vvp_island*use_island)
{
      ಯದಿ (island_table == 0)
	    island_table = ನೂತನ ಚಿಹ್ನೆ_map_s<vvp_island>;

      island_table->sym_set_ಮೌಲ್ಯ(label, use_island);
      island_ಎಣಿಕೆ += 1;
      island_list = static_cast<vvp_island **>(realloc(island_list,
                                               island_ಎಣಿಕೆ*ಪರಿಮಾಣ(vvp_island **)));
      island_list[island_ಎಣಿಕೆ-1] = use_island;
      ಮುಕ್ತ(label);
}

vvp_island* compile_find_island(ಧ್ರುವ ವರ್ಣ*island)
{
      ಪ್ರತಿಜ್ಞಾ(island_table);
      vvp_island*use_island = island_table->sym_get_ಮೌಲ್ಯ(island);
      ಪ್ರತಿಜ್ಞಾ(use_island);
      ಪ್ರತ್ಯಾಗಮ use_island;
}

/*
 * This handles the compile of a .port record. A .port is a 2-way port
 * between the island and the outside. For example,
 *
 *   <label> .port <island> <src> ;
 *
 * The <src> is a label in the domain outside the island, and the
 * <label> is in the domain inside the island. Since this port is
 * bi-directional, the <label> is also available in the domain outside
 * the island. The outside should use the <label> to access the nexus
 * that this port represents, because the island will resolve internal
 * drivers with the external driver and make the output available on
 * <label>.
 */
ಶೂನ್ಯ compile_island_port(ವರ್ಣ*label, ವರ್ಣ*island, ವರ್ಣ*src)
{
      ಪ್ರತಿಜ್ಞಾ(island_table);
      vvp_island*use_island = island_table->sym_get_ಮೌಲ್ಯ(island);
      ಪ್ರತಿಜ್ಞಾ(use_island);
      ಮುಕ್ತ(island);

      vvp_net_t*net = ನೂತನ vvp_net_t;
      vvp_island_port*fun = ನೂತನ vvp_island_port(use_island);
      net->fun = fun;

	// Get the source from outside the island
      ಒಳಹರಿವು_connect(net, 0, src);

	// Define the functor outside the island.
      define_functor_ಚಿಹ್ನೆ(label, net);

	// Also define it inside the island.
      use_island->add_port(label, net);

      ಮುಕ್ತ(label);
}

ಶೂನ್ಯ compile_island_export(ವರ್ಣ*label, ವರ್ಣ*island)
{
      fprintf(stderr, "XXXX %s .export %s;\n", label, island);
      ಮುಕ್ತ(label);
      ಮುಕ್ತ(island);
}

/*
 * This handles the compile of a .import record. A .import is an input
 * port into the island from the outside domain. For example,
 *
 *   <label> .import <island> <src> ;
 *
 * The <src> is a label in the domain outside the island, and the
 * <label> is in the domain inside the island. Branches within the
 * island use the <label>.
 */
ಶೂನ್ಯ compile_island_import(ವರ್ಣ*label, ವರ್ಣ*island, ವರ್ಣ*src)
{
      ಪ್ರತಿಜ್ಞಾ(island_table);
      vvp_island*use_island = island_table->sym_get_ಮೌಲ್ಯ(island);
      ಪ್ರತಿಜ್ಞಾ(use_island);
      ಮುಕ್ತ(island);

      vvp_net_t*net = ನೂತನ vvp_net_t;
      vvp_island_port*fun = ನೂತನ vvp_island_port(use_island);
      net->fun = fun;

	// Get the source from outside the island
      ಒಳಹರಿವು_connect(net, 0, src);

	// Define the functor only inside the island.
      use_island->add_port(label, net);

      ಮುಕ್ತ(label);
}

ಶೂನ್ಯ compile_island_cleanup(ಶೂನ್ಯ)
{
	// Call the per-island cleanup to get rid of local symbol tables.
      ಪ್ರತಿ (unsigned idx = 0; idx < island_ಎಣಿಕೆ; idx += 1) {
	    island_list[idx]->compile_cleanup();
      }

	// If we are not doing valgrind checking then free the list.
#ifndef CHECK_WITH_VALGRIND
      ಮುಕ್ತ(island_list);
      island_list = 0;
      island_ಎಣಿಕೆ = 0;
#endif

	// Remove the island symbol table itself.
      delete island_table;
      island_table = 0;
}
