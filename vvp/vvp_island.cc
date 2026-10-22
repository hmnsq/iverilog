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

using نطاق std;

#ifdef CHECK_WITH_VALGRIND
ساكن منطقي at_EOS = خطأ;
#endif

فراغ island_send_value(vvp_net_t*net, ثابت vvp_vector8_t&val)
{
      vvp_island_port*fun = dynamic_cast<vvp_island_port*>(net->fun);
      𐤀𐤌 (fun->outvalue .eeq(val))
	    أرجع;

      fun->outvalue = val;
      net->send_vec8(fun->outvalue);
}

/*
 * Implementations...
 */
vvp_island::vvp_island()
{
      flagged_ = خطأ;
      branches_ = 0;
      ports_ = 0;
      anodes_ = 0;
      bnodes_ = 0;
}

vvp_island::~vvp_island()
{
	// We can only delete islands at the end of simulation.
#ifdef CHECK_WITH_VALGRIND
      𐤀𐤌 (!at_EOS) أكد(0);
#endif

      طالما (branches_) {
	    vvp_island_branch *next_br = branches_->next_branch;
	    delete branches_;
	    branches_ = next_br;
      }
}

فراغ vvp_island::flag_island()
{
      𐤀𐤌 (flagged_ == true)
	    أرجع;

      schedule_generic(هذا, 0, خطأ, خطأ);
      flagged_ = true;
}

/*
* This method handles the callback from the scheduler. It does basic
* housecleaning and calls the run_island() method implemented by the
* derived class.
*/
فراغ vvp_island::run_run()
{
      flagged_ = خطأ;
      run_island();
}


فراغ vvp_island::add_port(ثابت حرف*key, vvp_net_t*net)
{
      𐤀𐤌 (ports_ == 0)
	    ports_ = جديد symbol_map_s<vvp_net_t>;

	// each port should have a unique label
      أكد(ports_->sym_get_value(key) == 0);

      ports_->sym_set_value(key, net);
}

فراغ vvp_island::add_branch(vvp_island_branch*branch, ثابت حرف*pa, ثابت حرف*pb)
{
      vvp_island_branch*cur;
      أكد(ports_);
      branch->a = ports_->sym_get_value(pa);
      branch->b = ports_->sym_get_value(pb);
      أكد(branch->a && branch->b);

      vvp_branch_ptr_t ptra (branch, 0);
      vvp_branch_ptr_t ptrb (branch, 1);
      𐤀𐤌 (anodes_ == 0)
	    anodes_ = جديد symbol_map_s<vvp_island_branch>;
      𐤀𐤌 (bnodes_ == 0)
	    bnodes_ = جديد symbol_map_s<vvp_island_branch>;

      𐤀𐤌 ((cur = anodes_->sym_get_value(pa))) {
	    branch->link[0] = cur->link[0];
	    cur->link[0] = ptra;
      } وإلا 𐤀𐤌 ((cur = bnodes_->sym_get_value(pa))) {
	    branch->link[0] = cur->link[1];
	    cur->link[1] = ptra;
      } وإلا {
	    branch->link[0] = ptra;
	    anodes_->sym_set_value(pa, branch);
      }

      𐤀𐤌 ((cur = anodes_->sym_get_value(pb))) {
	    branch->link[1] = cur->link[0];
	    cur->link[0] = ptrb;
      } وإلا 𐤀𐤌 ((cur = bnodes_->sym_get_value(pb))) {
	    branch->link[1] = cur->link[1];
	    cur->link[1] = ptrb;
      } وإلا {
	    branch->link[1] = ptrb;
	    bnodes_->sym_set_value(pb, branch);
      }

      branch->next_branch = branches_;
      branches_ = branch;
}

vvp_net_t* vvp_island::find_port(ثابت حرف*key)
{
      𐤀𐤌 (ports_ == 0)
	    أرجع 0;
      وإلا
	    أرجع ports_->sym_get_value(key);
}

فراغ vvp_island::compile_cleanup()
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

فراغ vvp_island_port::recv_vec4(vvp_net_ptr_t, ثابت vvp_vector4_t&bit,
                                vvp_context_t)
{
      vvp_vector8_t tmp (bit, 6, 6);
      𐤀𐤌 (invalue .eeq(tmp))
	    أرجع;

      invalue = tmp;
      island_->flag_island();
}

فراغ vvp_island_port::recv_vec4_pv(vvp_net_ptr_t port, ثابت vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector8_t tmp(bit, 6, 6);
      recv_vec8_pv(port, tmp, base, vwid);
}


فراغ vvp_island_port::recv_vec8(vvp_net_ptr_t, ثابت vvp_vector8_t&bit)
{
      𐤀𐤌 (invalue .eeq(bit))
	    أرجع;

      invalue = bit;
      island_->flag_island();
}

فراغ vvp_island_port::recv_vec8_pv(vvp_net_ptr_t, ثابت vvp_vector8_t&bit,
                                   unsigned base, unsigned vwid)
{
      𐤀𐤌 (invalue.size() == 0) {
	    invalue = part_expand(bit, vwid, base);
      } وإلا {
	    unsigned wid = bit.size();
	    أكد(invalue.size() == vwid);
	    لكل (unsigned idx = 0; idx < wid ; idx += 1) {
		  𐤀𐤌 ((base+idx) >= vwid)
			اكسر;
		  invalue.set_bit(base+idx, bit.value(idx));
	    }
      }

      island_->flag_island();
}

فراغ vvp_island_port::force_flag(منطقي run_now)
{
      𐤀𐤌 (run_now)
	    island_->run_island();
      وإلا
	    island_->flag_island();
}

vvp_island_branch::~vvp_island_branch()
{
}

فراغ island_collect_node(list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur)
{
      conn .push_back(cur);
      لكل (vvp_branch_ptr_t idx = next(cur) ; idx != cur ; idx = next(idx))
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
ساكن symbol_map_s<vvp_island>* island_table = 0;
ساكن vvp_island** island_list = 0;
ساكن unsigned island_count = 0;

#ifdef CHECK_WITH_VALGRIND
فراغ island_delete()
{
      at_EOS = true;
      لكل (unsigned idx = 0; idx < island_count; idx += 1) {
	    delete island_list[idx];
      }
      حرر(island_list);
      island_list = 0;
      island_count = 0;
}
#endif

فراغ compile_island_base(حرف*label, vvp_island*use_island)
{
      𐤀𐤌 (island_table == 0)
	    island_table = جديد symbol_map_s<vvp_island>;

      island_table->sym_set_value(label, use_island);
      island_count += 1;
      island_list = static_cast<vvp_island **>(realloc(island_list,
                                               island_count*حجم(vvp_island **)));
      island_list[island_count-1] = use_island;
      حرر(label);
}

vvp_island* compile_find_island(ثابت حرف*island)
{
      أكد(island_table);
      vvp_island*use_island = island_table->sym_get_value(island);
      أكد(use_island);
      أرجع use_island;
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
فراغ compile_island_port(حرف*label, حرف*island, حرف*src)
{
      أكد(island_table);
      vvp_island*use_island = island_table->sym_get_value(island);
      أكد(use_island);
      حرر(island);

      vvp_net_t*net = جديد vvp_net_t;
      vvp_island_port*fun = جديد vvp_island_port(use_island);
      net->fun = fun;

	// Get the source from outside the island
      input_connect(net, 0, src);

	// Define the functor outside the island.
      define_functor_symbol(label, net);

	// Also define it inside the island.
      use_island->add_port(label, net);

      حرر(label);
}

فراغ compile_island_export(حرف*label, حرف*island)
{
      fprintf(stderr, "XXXX %s .export %s;\n", label, island);
      حرر(label);
      حرر(island);
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
فراغ compile_island_import(حرف*label, حرف*island, حرف*src)
{
      أكد(island_table);
      vvp_island*use_island = island_table->sym_get_value(island);
      أكد(use_island);
      حرر(island);

      vvp_net_t*net = جديد vvp_net_t;
      vvp_island_port*fun = جديد vvp_island_port(use_island);
      net->fun = fun;

	// Get the source from outside the island
      input_connect(net, 0, src);

	// Define the functor only inside the island.
      use_island->add_port(label, net);

      حرر(label);
}

فراغ compile_island_cleanup(فراغ)
{
	// Call the per-island cleanup to get rid of local symbol tables.
      لكل (unsigned idx = 0; idx < island_count; idx += 1) {
	    island_list[idx]->compile_cleanup();
      }

	// If we are not doing valgrind checking then free the list.
#ifndef CHECK_WITH_VALGRIND
      حرر(island_list);
      island_list = 0;
      island_count = 0;
#endif

	// Remove the island symbol table itself.
      delete island_table;
      island_table = 0;
}
