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

using নামাকাশ std;

#ifdef CHECK_WITH_VALGRIND
স্থির যৌক্তিক at_EOS = মিথ্যা;
#endif

শূন্য island_send_মান(vvp_net_t*net, ধ্রুবক vvp_vector8_t&val)
{
      vvp_island_port*fun = dynamic_cast<vvp_island_port*>(net->fun);
      যদি (fun->outvalue .eeq(val))
	    ফেরত;

      fun->outvalue = val;
      net->send_vec8(fun->outvalue);
}

/*
 * Implementations...
 */
vvp_island::vvp_island()
{
      flagged_ = মিথ্যা;
      branches_ = 0;
      ports_ = 0;
      anodes_ = 0;
      bnodes_ = 0;
}

vvp_island::~vvp_island()
{
	// We can only delete islands at the end of simulation.
#ifdef CHECK_WITH_VALGRIND
      যদি (!at_EOS) প্রতিজ্ঞা(0);
#endif

      যখন (branches_) {
	    vvp_island_branch *পরবর্তী_br = branches_->পরবর্তী_branch;
	    delete branches_;
	    branches_ = পরবর্তী_br;
      }
}

শূন্য vvp_island::flag_island()
{
      যদি (flagged_ == সত্য)
	    ফেরত;

      schedule_generic(এই, 0, মিথ্যা, মিথ্যা);
      flagged_ = সত্য;
}

/*
* This method handles the callback from the scheduler. It does basic
* housecleaning and calls the run_island() method implemented by the
* derived class.
*/
শূন্য vvp_island::run_run()
{
      flagged_ = মিথ্যা;
      run_island();
}


শূন্য vvp_island::add_port(ধ্রুবক অক্ষর*চাবি, vvp_net_t*net)
{
      যদি (ports_ == 0)
	    ports_ = নতুন প্রতীক_map_s<vvp_net_t>;

	// each port should have a unique label
      প্রতিজ্ঞা(ports_->sym_get_মান(চাবি) == 0);

      ports_->sym_set_মান(চাবি, net);
}

শূন্য vvp_island::add_branch(vvp_island_branch*branch, ধ্রুবক অক্ষর*pa, ধ্রুবক অক্ষর*pb)
{
      vvp_island_branch*cur;
      প্রতিজ্ঞা(ports_);
      branch->a = ports_->sym_get_মান(pa);
      branch->b = ports_->sym_get_মান(pb);
      প্রতিজ্ঞা(branch->a && branch->b);

      vvp_branch_ptr_t ptra (branch, 0);
      vvp_branch_ptr_t ptrb (branch, 1);
      যদি (anodes_ == 0)
	    anodes_ = নতুন প্রতীক_map_s<vvp_island_branch>;
      যদি (bnodes_ == 0)
	    bnodes_ = নতুন প্রতীক_map_s<vvp_island_branch>;

      যদি ((cur = anodes_->sym_get_মান(pa))) {
	    branch->link[0] = cur->link[0];
	    cur->link[0] = ptra;
      } অন্যথা যদি ((cur = bnodes_->sym_get_মান(pa))) {
	    branch->link[0] = cur->link[1];
	    cur->link[1] = ptra;
      } অন্যথা {
	    branch->link[0] = ptra;
	    anodes_->sym_set_মান(pa, branch);
      }

      যদি ((cur = anodes_->sym_get_মান(pb))) {
	    branch->link[1] = cur->link[0];
	    cur->link[0] = ptrb;
      } অন্যথা যদি ((cur = bnodes_->sym_get_মান(pb))) {
	    branch->link[1] = cur->link[1];
	    cur->link[1] = ptrb;
      } অন্যথা {
	    branch->link[1] = ptrb;
	    bnodes_->sym_set_মান(pb, branch);
      }

      branch->পরবর্তী_branch = branches_;
      branches_ = branch;
}

vvp_net_t* vvp_island::find_port(ধ্রুবক অক্ষর*চাবি)
{
      যদি (ports_ == 0)
	    ফেরত 0;
      অন্যথা
	    ফেরত ports_->sym_get_মান(চাবি);
}

শূন্য vvp_island::compile_cleanup()
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

শূন্য vvp_island_port::recv_vec4(vvp_net_ptr_t, ধ্রুবক vvp_vector4_t&bit,
                                vvp_context_t)
{
      vvp_vector8_t tmp (bit, 6, 6);
      যদি (invalue .eeq(tmp))
	    ফেরত;

      invalue = tmp;
      island_->flag_island();
}

শূন্য vvp_island_port::recv_vec4_pv(vvp_net_ptr_t port, ধ্রুবক vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector8_t tmp(bit, 6, 6);
      recv_vec8_pv(port, tmp, base, vwid);
}


শূন্য vvp_island_port::recv_vec8(vvp_net_ptr_t, ধ্রুবক vvp_vector8_t&bit)
{
      যদি (invalue .eeq(bit))
	    ফেরত;

      invalue = bit;
      island_->flag_island();
}

শূন্য vvp_island_port::recv_vec8_pv(vvp_net_ptr_t, ধ্রুবক vvp_vector8_t&bit,
                                   unsigned base, unsigned vwid)
{
      যদি (invalue.size() == 0) {
	    invalue = part_expand(bit, vwid, base);
      } অন্যথা {
	    unsigned wid = bit.size();
	    প্রতিজ্ঞা(invalue.size() == vwid);
	    জন্য (unsigned idx = 0; idx < wid ; idx += 1) {
		  যদি ((base+idx) >= vwid)
			বিরতি;
		  invalue.set_bit(base+idx, bit.value(idx));
	    }
      }

      island_->flag_island();
}

শূন্য vvp_island_port::force_flag(যৌক্তিক run_now)
{
      যদি (run_now)
	    island_->run_island();
      অন্যথা
	    island_->flag_island();
}

vvp_island_branch::~vvp_island_branch()
{
}

শূন্য island_collect_গ্রন্থি(list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur)
{
      conn .push_back(cur);
      জন্য (vvp_branch_ptr_t idx = পরবর্তী_2(cur) ; idx != cur ; idx = পরবর্তী_2(idx))
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
স্থির প্রতীক_map_s<vvp_island>* island_table = 0;
স্থির vvp_island** island_list = 0;
স্থির unsigned island_গুনতি = 0;

#ifdef CHECK_WITH_VALGRIND
শূন্য island_delete()
{
      at_EOS = সত্য;
      জন্য (unsigned idx = 0; idx < island_গুনতি; idx += 1) {
	    delete island_list[idx];
      }
      মুক্ত(island_list);
      island_list = 0;
      island_গুনতি = 0;
}
#endif

শূন্য compile_island_base(অক্ষর*label, vvp_island*use_island)
{
      যদি (island_table == 0)
	    island_table = নতুন প্রতীক_map_s<vvp_island>;

      island_table->sym_set_মান(label, use_island);
      island_গুনতি += 1;
      island_list = static_cast<vvp_island **>(realloc(island_list,
                                               island_গুনতি*আকার(vvp_island **)));
      island_list[island_গুনতি-1] = use_island;
      মুক্ত(label);
}

vvp_island* compile_find_island(ধ্রুবক অক্ষর*island)
{
      প্রতিজ্ঞা(island_table);
      vvp_island*use_island = island_table->sym_get_মান(island);
      প্রতিজ্ঞা(use_island);
      ফেরত use_island;
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
শূন্য compile_island_port(অক্ষর*label, অক্ষর*island, অক্ষর*src)
{
      প্রতিজ্ঞা(island_table);
      vvp_island*use_island = island_table->sym_get_মান(island);
      প্রতিজ্ঞা(use_island);
      মুক্ত(island);

      vvp_net_t*net = নতুন vvp_net_t;
      vvp_island_port*fun = নতুন vvp_island_port(use_island);
      net->fun = fun;

	// Get the source from outside the island
      প্রবেশ_connect(net, 0, src);

	// Define the functor outside the island.
      define_functor_প্রতীক(label, net);

	// Also define it inside the island.
      use_island->add_port(label, net);

      মুক্ত(label);
}

শূন্য compile_island_export(অক্ষর*label, অক্ষর*island)
{
      fprintf(stderr, "XXXX %s .export %s;\n", label, island);
      মুক্ত(label);
      মুক্ত(island);
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
শূন্য compile_island_import(অক্ষর*label, অক্ষর*island, অক্ষর*src)
{
      প্রতিজ্ঞা(island_table);
      vvp_island*use_island = island_table->sym_get_মান(island);
      প্রতিজ্ঞা(use_island);
      মুক্ত(island);

      vvp_net_t*net = নতুন vvp_net_t;
      vvp_island_port*fun = নতুন vvp_island_port(use_island);
      net->fun = fun;

	// Get the source from outside the island
      প্রবেশ_connect(net, 0, src);

	// Define the functor only inside the island.
      use_island->add_port(label, net);

      মুক্ত(label);
}

শূন্য compile_island_cleanup(শূন্য)
{
	// Call the per-island cleanup to get rid of local symbol tables.
      জন্য (unsigned idx = 0; idx < island_গুনতি; idx += 1) {
	    island_list[idx]->compile_cleanup();
      }

	// If we are not doing valgrind checking then free the list.
#ifndef CHECK_WITH_VALGRIND
      মুক্ত(island_list);
      island_list = 0;
      island_গুনতি = 0;
#endif

	// Remove the island symbol table itself.
      delete island_table;
      island_table = 0;
}
