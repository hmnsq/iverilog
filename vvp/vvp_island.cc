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

using નામાકાશ std;

#ifdef CHECK_WITH_VALGRIND
સ્થિર તાર્કિક at_EOS = ખોટું;
#endif

શૂન્ય island_send_કિંમત(vvp_net_t*net, અચલ vvp_vector8_t&val)
{
      vvp_island_port*fun = dynamic_cast<vvp_island_port*>(net->fun);
      જો (fun->outvalue .eeq(val))
	    પાછા;

      fun->outvalue = val;
      net->send_vec8(fun->outvalue);
}

/*
 * Implementations...
 */
vvp_island::vvp_island()
{
      flagged_ = ખોટું;
      branches_ = 0;
      ports_ = 0;
      anodes_ = 0;
      bnodes_ = 0;
}

vvp_island::~vvp_island()
{
	// We can only delete islands at the end of simulation.
#ifdef CHECK_WITH_VALGRIND
      જો (!at_EOS) પ્રતિજ્ઞા(0);
#endif

      જ્યારે (branches_) {
	    vvp_island_branch *next_br = branches_->next_branch;
	    delete branches_;
	    branches_ = next_br;
      }
}

શૂન્ય vvp_island::flag_island()
{
      જો (flagged_ == સાચું)
	    પાછા;

      schedule_generic(આ, 0, ખોટું, ખોટું);
      flagged_ = સાચું;
}

/*
* This method handles the callback from the scheduler. It does basic
* housecleaning and calls the run_island() method implemented by the
* derived class.
*/
શૂન્ય vvp_island::run_run()
{
      flagged_ = ખોટું;
      run_island();
}


શૂન્ય vvp_island::add_port(અચલ અક્ષર*ચાવી, vvp_net_t*net)
{
      જો (ports_ == 0)
	    ports_ = નવું પ્રતીક_map_s<vvp_net_t>;

	// each port should have a unique label
      પ્રતિજ્ઞા(ports_->sym_get_કિંમત(ચાવી) == 0);

      ports_->sym_set_કિંમત(ચાવી, net);
}

શૂન્ય vvp_island::add_branch(vvp_island_branch*branch, અચલ અક્ષર*pa, અચલ અક્ષર*pb)
{
      vvp_island_branch*cur;
      પ્રતિજ્ઞા(ports_);
      branch->a = ports_->sym_get_કિંમત(pa);
      branch->b = ports_->sym_get_કિંમત(pb);
      પ્રતિજ્ઞા(branch->a && branch->b);

      vvp_branch_ptr_t ptra (branch, 0);
      vvp_branch_ptr_t ptrb (branch, 1);
      જો (anodes_ == 0)
	    anodes_ = નવું પ્રતીક_map_s<vvp_island_branch>;
      જો (bnodes_ == 0)
	    bnodes_ = નવું પ્રતીક_map_s<vvp_island_branch>;

      જો ((cur = anodes_->sym_get_કિંમત(pa))) {
	    branch->link[0] = cur->link[0];
	    cur->link[0] = ptra;
      } નહીંતર જો ((cur = bnodes_->sym_get_કિંમત(pa))) {
	    branch->link[0] = cur->link[1];
	    cur->link[1] = ptra;
      } નહીંતર {
	    branch->link[0] = ptra;
	    anodes_->sym_set_કિંમત(pa, branch);
      }

      જો ((cur = anodes_->sym_get_કિંમત(pb))) {
	    branch->link[1] = cur->link[0];
	    cur->link[0] = ptrb;
      } નહીંતર જો ((cur = bnodes_->sym_get_કિંમત(pb))) {
	    branch->link[1] = cur->link[1];
	    cur->link[1] = ptrb;
      } નહીંતર {
	    branch->link[1] = ptrb;
	    bnodes_->sym_set_કિંમત(pb, branch);
      }

      branch->next_branch = branches_;
      branches_ = branch;
}

vvp_net_t* vvp_island::find_port(અચલ અક્ષર*ચાવી)
{
      જો (ports_ == 0)
	    પાછા 0;
      નહીંતર
	    પાછા ports_->sym_get_કિંમત(ચાવી);
}

શૂન્ય vvp_island::compile_cleanup()
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

શૂન્ય vvp_island_port::recv_vec4(vvp_net_ptr_t, અચલ vvp_vector4_t&bit,
                                vvp_context_t)
{
      vvp_vector8_t tmp (bit, 6, 6);
      જો (invalue .eeq(tmp))
	    પાછા;

      invalue = tmp;
      island_->flag_island();
}

શૂન્ય vvp_island_port::recv_vec4_pv(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector8_t tmp(bit, 6, 6);
      recv_vec8_pv(port, tmp, base, vwid);
}


શૂન્ય vvp_island_port::recv_vec8(vvp_net_ptr_t, અચલ vvp_vector8_t&bit)
{
      જો (invalue .eeq(bit))
	    પાછા;

      invalue = bit;
      island_->flag_island();
}

શૂન્ય vvp_island_port::recv_vec8_pv(vvp_net_ptr_t, અચલ vvp_vector8_t&bit,
                                   unsigned base, unsigned vwid)
{
      જો (invalue.size() == 0) {
	    invalue = part_expand(bit, vwid, base);
      } નહીંતર {
	    unsigned wid = bit.size();
	    પ્રતિજ્ઞા(invalue.size() == vwid);
	    માટે (unsigned idx = 0; idx < wid ; idx += 1) {
		  જો ((base+idx) >= vwid)
			તોડો;
		  invalue.set_bit(base+idx, bit.value(idx));
	    }
      }

      island_->flag_island();
}

શૂન્ય vvp_island_port::force_flag(તાર્કિક run_now)
{
      જો (run_now)
	    island_->run_island();
      નહીંતર
	    island_->flag_island();
}

vvp_island_branch::~vvp_island_branch()
{
}

શૂન્ય island_collect_ગાંઠ(list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur)
{
      conn .push_back(cur);
      માટે (vvp_branch_ptr_t idx = next(cur) ; idx != cur ; idx = next(idx))
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
સ્થિર પ્રતીક_map_s<vvp_island>* island_table = 0;
સ્થિર vvp_island** island_list = 0;
સ્થિર unsigned island_ગણતરી = 0;

#ifdef CHECK_WITH_VALGRIND
શૂન્ય island_delete()
{
      at_EOS = સાચું;
      માટે (unsigned idx = 0; idx < island_ગણતરી; idx += 1) {
	    delete island_list[idx];
      }
      મુક્ત(island_list);
      island_list = 0;
      island_ગણતરી = 0;
}
#endif

શૂન્ય compile_island_base(અક્ષર*label, vvp_island*use_island)
{
      જો (island_table == 0)
	    island_table = નવું પ્રતીક_map_s<vvp_island>;

      island_table->sym_set_કિંમત(label, use_island);
      island_ગણતરી += 1;
      island_list = static_cast<vvp_island **>(realloc(island_list,
                                               island_ગણતરી*કદ(vvp_island **)));
      island_list[island_ગણતરી-1] = use_island;
      મુક્ત(label);
}

vvp_island* compile_find_island(અચલ અક્ષર*island)
{
      પ્રતિજ્ઞા(island_table);
      vvp_island*use_island = island_table->sym_get_કિંમત(island);
      પ્રતિજ્ઞા(use_island);
      પાછા use_island;
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
શૂન્ય compile_island_port(અક્ષર*label, અક્ષર*island, અક્ષર*src)
{
      પ્રતિજ્ઞા(island_table);
      vvp_island*use_island = island_table->sym_get_કિંમત(island);
      પ્રતિજ્ઞા(use_island);
      મુક્ત(island);

      vvp_net_t*net = નવું vvp_net_t;
      vvp_island_port*fun = નવું vvp_island_port(use_island);
      net->fun = fun;

	// Get the source from outside the island
      આગત_connect(net, 0, src);

	// Define the functor outside the island.
      define_functor_પ્રતીક(label, net);

	// Also define it inside the island.
      use_island->add_port(label, net);

      મુક્ત(label);
}

શૂન્ય compile_island_export(અક્ષર*label, અક્ષર*island)
{
      fprintf(stderr, "XXXX %s .export %s;\n", label, island);
      મુક્ત(label);
      મુક્ત(island);
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
શૂન્ય compile_island_import(અક્ષર*label, અક્ષર*island, અક્ષર*src)
{
      પ્રતિજ્ઞા(island_table);
      vvp_island*use_island = island_table->sym_get_કિંમત(island);
      પ્રતિજ્ઞા(use_island);
      મુક્ત(island);

      vvp_net_t*net = નવું vvp_net_t;
      vvp_island_port*fun = નવું vvp_island_port(use_island);
      net->fun = fun;

	// Get the source from outside the island
      આગત_connect(net, 0, src);

	// Define the functor only inside the island.
      use_island->add_port(label, net);

      મુક્ત(label);
}

શૂન્ય compile_island_cleanup(શૂન્ય)
{
	// Call the per-island cleanup to get rid of local symbol tables.
      માટે (unsigned idx = 0; idx < island_ગણતરી; idx += 1) {
	    island_list[idx]->compile_cleanup();
      }

	// If we are not doing valgrind checking then free the list.
#ifndef CHECK_WITH_VALGRIND
      મુક્ત(island_list);
      island_list = 0;
      island_ગણતરી = 0;
#endif

	// Remove the island symbol table itself.
      delete island_table;
      island_table = 0;
}
