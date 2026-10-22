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

using நாமாகாஶ std;

#ifdef CHECK_WITH_VALGRIND
நிலையான தருக்க at_EOS = பொய்;
#endif

வெறுமை island_send_மதிப்பு(vvp_net_t*net, மாறிலி vvp_vector8_t&val)
{
      vvp_island_port*fun = dynamic_cast<vvp_island_port*>(net->fun);
      எனில் (fun->outvalue .eeq(val))
	    திரும்பு;

      fun->outvalue = val;
      net->send_vec8(fun->outvalue);
}

/*
 * Implementations...
 */
vvp_island::vvp_island()
{
      flagged_ = பொய்;
      branches_ = 0;
      ports_ = 0;
      anodes_ = 0;
      bnodes_ = 0;
}

vvp_island::~vvp_island()
{
	// We can only delete islands at the end of simulation.
#ifdef CHECK_WITH_VALGRIND
      எனில் (!at_EOS) ப்ரதிஜ்ஞா(0);
#endif

      போது (branches_) {
	    vvp_island_branch *next_br = branches_->next_branch;
	    delete branches_;
	    branches_ = next_br;
      }
}

வெறுமை vvp_island::flag_island()
{
      எனில் (flagged_ == உண்மை)
	    திரும்பு;

      schedule_generic(இது, 0, பொய், பொய்);
      flagged_ = உண்மை;
}

/*
* This method handles the callback from the scheduler. It does basic
* housecleaning and calls the run_island() method implemented by the
* derived class.
*/
வெறுமை vvp_island::run_run()
{
      flagged_ = பொய்;
      run_island();
}


வெறுமை vvp_island::add_port(மாறிலி எழுத்து*சாவி, vvp_net_t*net)
{
      எனில் (ports_ == 0)
	    ports_ = புதிய குறியீடு_map_s<vvp_net_t>;

	// each port should have a unique label
      ப்ரதிஜ்ஞா(ports_->sym_get_மதிப்பு(சாவி) == 0);

      ports_->sym_set_மதிப்பு(சாவி, net);
}

வெறுமை vvp_island::add_branch(vvp_island_branch*branch, மாறிலி எழுத்து*pa, மாறிலி எழுத்து*pb)
{
      vvp_island_branch*cur;
      ப்ரதிஜ்ஞா(ports_);
      branch->a = ports_->sym_get_மதிப்பு(pa);
      branch->b = ports_->sym_get_மதிப்பு(pb);
      ப்ரதிஜ்ஞா(branch->a && branch->b);

      vvp_branch_ptr_t ptra (branch, 0);
      vvp_branch_ptr_t ptrb (branch, 1);
      எனில் (anodes_ == 0)
	    anodes_ = புதிய குறியீடு_map_s<vvp_island_branch>;
      எனில் (bnodes_ == 0)
	    bnodes_ = புதிய குறியீடு_map_s<vvp_island_branch>;

      எனில் ((cur = anodes_->sym_get_மதிப்பு(pa))) {
	    branch->link[0] = cur->link[0];
	    cur->link[0] = ptra;
      } இல்லையெனில் எனில் ((cur = bnodes_->sym_get_மதிப்பு(pa))) {
	    branch->link[0] = cur->link[1];
	    cur->link[1] = ptra;
      } இல்லையெனில் {
	    branch->link[0] = ptra;
	    anodes_->sym_set_மதிப்பு(pa, branch);
      }

      எனில் ((cur = anodes_->sym_get_மதிப்பு(pb))) {
	    branch->link[1] = cur->link[0];
	    cur->link[0] = ptrb;
      } இல்லையெனில் எனில் ((cur = bnodes_->sym_get_மதிப்பு(pb))) {
	    branch->link[1] = cur->link[1];
	    cur->link[1] = ptrb;
      } இல்லையெனில் {
	    branch->link[1] = ptrb;
	    bnodes_->sym_set_மதிப்பு(pb, branch);
      }

      branch->next_branch = branches_;
      branches_ = branch;
}

vvp_net_t* vvp_island::find_port(மாறிலி எழுத்து*சாவி)
{
      எனில் (ports_ == 0)
	    திரும்பு 0;
      இல்லையெனில்
	    திரும்பு ports_->sym_get_மதிப்பு(சாவி);
}

வெறுமை vvp_island::compile_cleanup()
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

வெறுமை vvp_island_port::recv_vec4(vvp_net_ptr_t, மாறிலி vvp_vector4_t&bit,
                                vvp_context_t)
{
      vvp_vector8_t tmp (bit, 6, 6);
      எனில் (invalue .eeq(tmp))
	    திரும்பு;

      invalue = tmp;
      island_->flag_island();
}

வெறுமை vvp_island_port::recv_vec4_pv(vvp_net_ptr_t port, மாறிலி vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector8_t tmp(bit, 6, 6);
      recv_vec8_pv(port, tmp, base, vwid);
}


வெறுமை vvp_island_port::recv_vec8(vvp_net_ptr_t, மாறிலி vvp_vector8_t&bit)
{
      எனில் (invalue .eeq(bit))
	    திரும்பு;

      invalue = bit;
      island_->flag_island();
}

வெறுமை vvp_island_port::recv_vec8_pv(vvp_net_ptr_t, மாறிலி vvp_vector8_t&bit,
                                   unsigned base, unsigned vwid)
{
      எனில் (invalue.size() == 0) {
	    invalue = part_expand(bit, vwid, base);
      } இல்லையெனில் {
	    unsigned wid = bit.size();
	    ப்ரதிஜ்ஞா(invalue.size() == vwid);
	    க்கு (unsigned idx = 0; idx < wid ; idx += 1) {
		  எனில் ((base+idx) >= vwid)
			நிறுத்து;
		  invalue.set_bit(base+idx, bit.value(idx));
	    }
      }

      island_->flag_island();
}

வெறுமை vvp_island_port::force_flag(தருக்க run_now)
{
      எனில் (run_now)
	    island_->run_island();
      இல்லையெனில்
	    island_->flag_island();
}

vvp_island_branch::~vvp_island_branch()
{
}

வெறுமை island_collect_முனை(list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur)
{
      conn .push_back(cur);
      க்கு (vvp_branch_ptr_t idx = next(cur) ; idx != cur ; idx = next(idx))
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
நிலையான குறியீடு_map_s<vvp_island>* island_table = 0;
நிலையான vvp_island** island_list = 0;
நிலையான unsigned island_மொத்தம் = 0;

#ifdef CHECK_WITH_VALGRIND
வெறுமை island_delete()
{
      at_EOS = உண்மை;
      க்கு (unsigned idx = 0; idx < island_மொத்தம்; idx += 1) {
	    delete island_list[idx];
      }
      விடுவி(island_list);
      island_list = 0;
      island_மொத்தம் = 0;
}
#endif

வெறுமை compile_island_base(எழுத்து*label, vvp_island*use_island)
{
      எனில் (island_table == 0)
	    island_table = புதிய குறியீடு_map_s<vvp_island>;

      island_table->sym_set_மதிப்பு(label, use_island);
      island_மொத்தம் += 1;
      island_list = static_cast<vvp_island **>(realloc(island_list,
                                               island_மொத்தம்*அளவு(vvp_island **)));
      island_list[island_மொத்தம்-1] = use_island;
      விடுவி(label);
}

vvp_island* compile_find_island(மாறிலி எழுத்து*island)
{
      ப்ரதிஜ்ஞா(island_table);
      vvp_island*use_island = island_table->sym_get_மதிப்பு(island);
      ப்ரதிஜ்ஞா(use_island);
      திரும்பு use_island;
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
வெறுமை compile_island_port(எழுத்து*label, எழுத்து*island, எழுத்து*src)
{
      ப்ரதிஜ்ஞா(island_table);
      vvp_island*use_island = island_table->sym_get_மதிப்பு(island);
      ப்ரதிஜ்ஞா(use_island);
      விடுவி(island);

      vvp_net_t*net = புதிய vvp_net_t;
      vvp_island_port*fun = புதிய vvp_island_port(use_island);
      net->fun = fun;

	// Get the source from outside the island
      உள்ளீடு_connect(net, 0, src);

	// Define the functor outside the island.
      define_functor_குறியீடு(label, net);

	// Also define it inside the island.
      use_island->add_port(label, net);

      விடுவி(label);
}

வெறுமை compile_island_export(எழுத்து*label, எழுத்து*island)
{
      fprintf(stderr, "XXXX %s .export %s;\n", label, island);
      விடுவி(label);
      விடுவி(island);
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
வெறுமை compile_island_import(எழுத்து*label, எழுத்து*island, எழுத்து*src)
{
      ப்ரதிஜ்ஞா(island_table);
      vvp_island*use_island = island_table->sym_get_மதிப்பு(island);
      ப்ரதிஜ்ஞா(use_island);
      விடுவி(island);

      vvp_net_t*net = புதிய vvp_net_t;
      vvp_island_port*fun = புதிய vvp_island_port(use_island);
      net->fun = fun;

	// Get the source from outside the island
      உள்ளீடு_connect(net, 0, src);

	// Define the functor only inside the island.
      use_island->add_port(label, net);

      விடுவி(label);
}

வெறுமை compile_island_cleanup(வெறுமை)
{
	// Call the per-island cleanup to get rid of local symbol tables.
      க்கு (unsigned idx = 0; idx < island_மொத்தம்; idx += 1) {
	    island_list[idx]->compile_cleanup();
      }

	// If we are not doing valgrind checking then free the list.
#ifndef CHECK_WITH_VALGRIND
      விடுவி(island_list);
      island_list = 0;
      island_மொத்தம் = 0;
#endif

	// Remove the island symbol table itself.
      delete island_table;
      island_table = 0;
}
