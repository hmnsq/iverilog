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

using ନାମାକାଶ std;

#ifdef CHECK_WITH_VALGRIND
ସ୍ଥିର ତାର୍କିକ at_EOS = ମିଥ୍ୟା;
#endif

ଶୂନ୍ୟ island_send_ମୂଲ୍ୟ(vvp_net_t*net, ଅଚଳ vvp_vector8_t&val)
{
      vvp_island_port*fun = dynamic_cast<vvp_island_port*>(net->fun);
      ଯଦି (fun->outvalue .eeq(val))
	    ଫେରନ୍ତୁ;

      fun->outvalue = val;
      net->send_vec8(fun->outvalue);
}

/*
 * Implementations...
 */
vvp_island::vvp_island()
{
      flagged_ = ମିଥ୍ୟା;
      branches_ = 0;
      ports_ = 0;
      anodes_ = 0;
      bnodes_ = 0;
}

vvp_island::~vvp_island()
{
	// We can only delete islands at the end of simulation.
#ifdef CHECK_WITH_VALGRIND
      ଯଦି (!at_EOS) ପ୍ରତିଜ୍ଞା(0);
#endif

      ଯେତେବେଳେ (branches_) {
	    vvp_island_branch *next_br = branches_->next_branch;
	    delete branches_;
	    branches_ = next_br;
      }
}

ଶୂନ୍ୟ vvp_island::flag_island()
{
      ଯଦି (flagged_ == ସତ୍ୟ)
	    ଫେରନ୍ତୁ;

      schedule_generic(ଏହି, 0, ମିଥ୍ୟା, ମିଥ୍ୟା);
      flagged_ = ସତ୍ୟ;
}

/*
* This method handles the callback from the scheduler. It does basic
* housecleaning and calls the run_island() method implemented by the
* derived class.
*/
ଶୂନ୍ୟ vvp_island::run_run()
{
      flagged_ = ମିଥ୍ୟା;
      run_island();
}


ଶୂନ୍ୟ vvp_island::add_port(ଅଚଳ ଅକ୍ଷର*ଚାବି, vvp_net_t*net)
{
      ଯଦି (ports_ == 0)
	    ports_ = ନୂତନ ପ୍ରତୀକ_map_s<vvp_net_t>;

	// each port should have a unique label
      ପ୍ରତିଜ୍ଞା(ports_->sym_get_ମୂଲ୍ୟ(ଚାବି) == 0);

      ports_->sym_set_ମୂଲ୍ୟ(ଚାବି, net);
}

ଶୂନ୍ୟ vvp_island::add_branch(vvp_island_branch*branch, ଅଚଳ ଅକ୍ଷର*pa, ଅଚଳ ଅକ୍ଷର*pb)
{
      vvp_island_branch*cur;
      ପ୍ରତିଜ୍ଞା(ports_);
      branch->a = ports_->sym_get_ମୂଲ୍ୟ(pa);
      branch->b = ports_->sym_get_ମୂଲ୍ୟ(pb);
      ପ୍ରତିଜ୍ଞା(branch->a && branch->b);

      vvp_branch_ptr_t ptra (branch, 0);
      vvp_branch_ptr_t ptrb (branch, 1);
      ଯଦି (anodes_ == 0)
	    anodes_ = ନୂତନ ପ୍ରତୀକ_map_s<vvp_island_branch>;
      ଯଦି (bnodes_ == 0)
	    bnodes_ = ନୂତନ ପ୍ରତୀକ_map_s<vvp_island_branch>;

      ଯଦି ((cur = anodes_->sym_get_ମୂଲ୍ୟ(pa))) {
	    branch->link[0] = cur->link[0];
	    cur->link[0] = ptra;
      } ଅନ୍ୟଥା ଯଦି ((cur = bnodes_->sym_get_ମୂଲ୍ୟ(pa))) {
	    branch->link[0] = cur->link[1];
	    cur->link[1] = ptra;
      } ଅନ୍ୟଥା {
	    branch->link[0] = ptra;
	    anodes_->sym_set_ମୂଲ୍ୟ(pa, branch);
      }

      ଯଦି ((cur = anodes_->sym_get_ମୂଲ୍ୟ(pb))) {
	    branch->link[1] = cur->link[0];
	    cur->link[0] = ptrb;
      } ଅନ୍ୟଥା ଯଦି ((cur = bnodes_->sym_get_ମୂଲ୍ୟ(pb))) {
	    branch->link[1] = cur->link[1];
	    cur->link[1] = ptrb;
      } ଅନ୍ୟଥା {
	    branch->link[1] = ptrb;
	    bnodes_->sym_set_ମୂଲ୍ୟ(pb, branch);
      }

      branch->next_branch = branches_;
      branches_ = branch;
}

vvp_net_t* vvp_island::find_port(ଅଚଳ ଅକ୍ଷର*ଚାବି)
{
      ଯଦି (ports_ == 0)
	    ଫେରନ୍ତୁ 0;
      ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ ports_->sym_get_ମୂଲ୍ୟ(ଚାବି);
}

ଶୂନ୍ୟ vvp_island::compile_cleanup()
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

ଶୂନ୍ୟ vvp_island_port::recv_vec4(vvp_net_ptr_t, ଅଚଳ vvp_vector4_t&bit,
                                vvp_context_t)
{
      vvp_vector8_t tmp (bit, 6, 6);
      ଯଦି (invalue .eeq(tmp))
	    ଫେରନ୍ତୁ;

      invalue = tmp;
      island_->flag_island();
}

ଶୂନ୍ୟ vvp_island_port::recv_vec4_pv(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector8_t tmp(bit, 6, 6);
      recv_vec8_pv(port, tmp, base, vwid);
}


ଶୂନ୍ୟ vvp_island_port::recv_vec8(vvp_net_ptr_t, ଅଚଳ vvp_vector8_t&bit)
{
      ଯଦି (invalue .eeq(bit))
	    ଫେରନ୍ତୁ;

      invalue = bit;
      island_->flag_island();
}

ଶୂନ୍ୟ vvp_island_port::recv_vec8_pv(vvp_net_ptr_t, ଅଚଳ vvp_vector8_t&bit,
                                   unsigned base, unsigned vwid)
{
      ଯଦି (invalue.size() == 0) {
	    invalue = part_expand(bit, vwid, base);
      } ଅନ୍ୟଥା {
	    unsigned wid = bit.size();
	    ପ୍ରତିଜ୍ଞା(invalue.size() == vwid);
	    ପାଇଁ (unsigned idx = 0; idx < wid ; idx += 1) {
		  ଯଦି ((base+idx) >= vwid)
			ଭାଙ୍ଗ;
		  invalue.set_bit(base+idx, bit.value(idx));
	    }
      }

      island_->flag_island();
}

ଶୂନ୍ୟ vvp_island_port::force_flag(ତାର୍କିକ run_now)
{
      ଯଦି (run_now)
	    island_->run_island();
      ଅନ୍ୟଥା
	    island_->flag_island();
}

vvp_island_branch::~vvp_island_branch()
{
}

ଶୂନ୍ୟ island_collect_ଗଣ୍ଠି(list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur)
{
      conn .push_back(cur);
      ପାଇଁ (vvp_branch_ptr_t idx = next(cur) ; idx != cur ; idx = next(idx))
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
ସ୍ଥିର ପ୍ରତୀକ_map_s<vvp_island>* island_table = 0;
ସ୍ଥିର vvp_island** island_list = 0;
ସ୍ଥିର unsigned island_ଗଣନା = 0;

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ island_delete()
{
      at_EOS = ସତ୍ୟ;
      ପାଇଁ (unsigned idx = 0; idx < island_ଗଣନା; idx += 1) {
	    delete island_list[idx];
      }
      ମୁକ୍ତ(island_list);
      island_list = 0;
      island_ଗଣନା = 0;
}
#endif

ଶୂନ୍ୟ compile_island_base(ଅକ୍ଷର*label, vvp_island*use_island)
{
      ଯଦି (island_table == 0)
	    island_table = ନୂତନ ପ୍ରତୀକ_map_s<vvp_island>;

      island_table->sym_set_ମୂଲ୍ୟ(label, use_island);
      island_ଗଣନା += 1;
      island_list = static_cast<vvp_island **>(realloc(island_list,
                                               island_ଗଣନା*ଆକାର(vvp_island **)));
      island_list[island_ଗଣନା-1] = use_island;
      ମୁକ୍ତ(label);
}

vvp_island* compile_find_island(ଅଚଳ ଅକ୍ଷର*island)
{
      ପ୍ରତିଜ୍ଞା(island_table);
      vvp_island*use_island = island_table->sym_get_ମୂଲ୍ୟ(island);
      ପ୍ରତିଜ୍ଞା(use_island);
      ଫେରନ୍ତୁ use_island;
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
ଶୂନ୍ୟ compile_island_port(ଅକ୍ଷର*label, ଅକ୍ଷର*island, ଅକ୍ଷର*src)
{
      ପ୍ରତିଜ୍ଞା(island_table);
      vvp_island*use_island = island_table->sym_get_ମୂଲ୍ୟ(island);
      ପ୍ରତିଜ୍ଞା(use_island);
      ମୁକ୍ତ(island);

      vvp_net_t*net = ନୂତନ vvp_net_t;
      vvp_island_port*fun = ନୂତନ vvp_island_port(use_island);
      net->fun = fun;

	// Get the source from outside the island
      ନିବେଶ_connect(net, 0, src);

	// Define the functor outside the island.
      define_functor_ପ୍ରତୀକ(label, net);

	// Also define it inside the island.
      use_island->add_port(label, net);

      ମୁକ୍ତ(label);
}

ଶୂନ୍ୟ compile_island_export(ଅକ୍ଷର*label, ଅକ୍ଷର*island)
{
      fprintf(stderr, "XXXX %s .export %s;\n", label, island);
      ମୁକ୍ତ(label);
      ମୁକ୍ତ(island);
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
ଶୂନ୍ୟ compile_island_import(ଅକ୍ଷର*label, ଅକ୍ଷର*island, ଅକ୍ଷର*src)
{
      ପ୍ରତିଜ୍ଞା(island_table);
      vvp_island*use_island = island_table->sym_get_ମୂଲ୍ୟ(island);
      ପ୍ରତିଜ୍ଞା(use_island);
      ମୁକ୍ତ(island);

      vvp_net_t*net = ନୂତନ vvp_net_t;
      vvp_island_port*fun = ନୂତନ vvp_island_port(use_island);
      net->fun = fun;

	// Get the source from outside the island
      ନିବେଶ_connect(net, 0, src);

	// Define the functor only inside the island.
      use_island->add_port(label, net);

      ମୁକ୍ତ(label);
}

ଶୂନ୍ୟ compile_island_cleanup(ଶୂନ୍ୟ)
{
	// Call the per-island cleanup to get rid of local symbol tables.
      ପାଇଁ (unsigned idx = 0; idx < island_ଗଣନା; idx += 1) {
	    island_list[idx]->compile_cleanup();
      }

	// If we are not doing valgrind checking then free the list.
#ifndef CHECK_WITH_VALGRIND
      ମୁକ୍ତ(island_list);
      island_list = 0;
      island_ଗଣନା = 0;
#endif

	// Remove the island symbol table itself.
      delete island_table;
      island_table = 0;
}
