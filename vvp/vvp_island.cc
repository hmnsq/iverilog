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

using നാമാകാശ std;

#ifdef CHECK_WITH_VALGRIND
സ്ഥിതം തർക്കം at_EOS = അസത്യം;
#endif

ശൂന്യം island_send_മൂല്യം(vvp_net_t*net, സ്ഥിരം vvp_vector8_t&val)
{
      vvp_island_port*fun = dynamic_cast<vvp_island_port*>(net->fun);
      എങ്കിൽ (fun->outvalue .eeq(val))
	    തിരികെ;

      fun->outvalue = val;
      net->send_vec8(fun->outvalue);
}

/*
 * Implementations...
 */
vvp_island::vvp_island()
{
      flagged_ = അസത്യം;
      branches_ = 0;
      ports_ = 0;
      anodes_ = 0;
      bnodes_ = 0;
}

vvp_island::~vvp_island()
{
	// We can only delete islands at the end of simulation.
#ifdef CHECK_WITH_VALGRIND
      എങ്കിൽ (!at_EOS) പ്രതിജ്ഞാ(0);
#endif

      ആകുമ്പോൾ (branches_) {
	    vvp_island_branch *next_br = branches_->next_branch;
	    delete branches_;
	    branches_ = next_br;
      }
}

ശൂന്യം vvp_island::flag_island()
{
      എങ്കിൽ (flagged_ == സത്യം)
	    തിരികെ;

      schedule_generic(ഇത്, 0, അസത്യം, അസത്യം);
      flagged_ = സത്യം;
}

/*
* This method handles the callback from the scheduler. It does basic
* housecleaning and calls the run_island() method implemented by the
* derived class.
*/
ശൂന്യം vvp_island::run_run()
{
      flagged_ = അസത്യം;
      run_island();
}


ശൂന്യം vvp_island::add_port(സ്ഥിരം അക്ഷരം*താക്കോൽ, vvp_net_t*net)
{
      എങ്കിൽ (ports_ == 0)
	    ports_ = പുതിയ ചിഹ്നം_map_s<vvp_net_t>;

	// each port should have a unique label
      പ്രതിജ്ഞാ(ports_->sym_get_മൂല്യം(താക്കോൽ) == 0);

      ports_->sym_set_മൂല്യം(താക്കോൽ, net);
}

ശൂന്യം vvp_island::add_branch(vvp_island_branch*branch, സ്ഥിരം അക്ഷരം*pa, സ്ഥിരം അക്ഷരം*pb)
{
      vvp_island_branch*cur;
      പ്രതിജ്ഞാ(ports_);
      branch->a = ports_->sym_get_മൂല്യം(pa);
      branch->b = ports_->sym_get_മൂല്യം(pb);
      പ്രതിജ്ഞാ(branch->a && branch->b);

      vvp_branch_ptr_t ptra (branch, 0);
      vvp_branch_ptr_t ptrb (branch, 1);
      എങ്കിൽ (anodes_ == 0)
	    anodes_ = പുതിയ ചിഹ്നം_map_s<vvp_island_branch>;
      എങ്കിൽ (bnodes_ == 0)
	    bnodes_ = പുതിയ ചിഹ്നം_map_s<vvp_island_branch>;

      എങ്കിൽ ((cur = anodes_->sym_get_മൂല്യം(pa))) {
	    branch->link[0] = cur->link[0];
	    cur->link[0] = ptra;
      } അല്ലെങ്കിൽ എങ്കിൽ ((cur = bnodes_->sym_get_മൂല്യം(pa))) {
	    branch->link[0] = cur->link[1];
	    cur->link[1] = ptra;
      } അല്ലെങ്കിൽ {
	    branch->link[0] = ptra;
	    anodes_->sym_set_മൂല്യം(pa, branch);
      }

      എങ്കിൽ ((cur = anodes_->sym_get_മൂല്യം(pb))) {
	    branch->link[1] = cur->link[0];
	    cur->link[0] = ptrb;
      } അല്ലെങ്കിൽ എങ്കിൽ ((cur = bnodes_->sym_get_മൂല്യം(pb))) {
	    branch->link[1] = cur->link[1];
	    cur->link[1] = ptrb;
      } അല്ലെങ്കിൽ {
	    branch->link[1] = ptrb;
	    bnodes_->sym_set_മൂല്യം(pb, branch);
      }

      branch->next_branch = branches_;
      branches_ = branch;
}

vvp_net_t* vvp_island::find_port(സ്ഥിരം അക്ഷരം*താക്കോൽ)
{
      എങ്കിൽ (ports_ == 0)
	    തിരികെ 0;
      അല്ലെങ്കിൽ
	    തിരികെ ports_->sym_get_മൂല്യം(താക്കോൽ);
}

ശൂന്യം vvp_island::compile_cleanup()
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

ശൂന്യം vvp_island_port::recv_vec4(vvp_net_ptr_t, സ്ഥിരം vvp_vector4_t&bit,
                                vvp_context_t)
{
      vvp_vector8_t tmp (bit, 6, 6);
      എങ്കിൽ (invalue .eeq(tmp))
	    തിരികെ;

      invalue = tmp;
      island_->flag_island();
}

ശൂന്യം vvp_island_port::recv_vec4_pv(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector8_t tmp(bit, 6, 6);
      recv_vec8_pv(port, tmp, base, vwid);
}


ശൂന്യം vvp_island_port::recv_vec8(vvp_net_ptr_t, സ്ഥിരം vvp_vector8_t&bit)
{
      എങ്കിൽ (invalue .eeq(bit))
	    തിരികെ;

      invalue = bit;
      island_->flag_island();
}

ശൂന്യം vvp_island_port::recv_vec8_pv(vvp_net_ptr_t, സ്ഥിരം vvp_vector8_t&bit,
                                   unsigned base, unsigned vwid)
{
      എങ്കിൽ (invalue.size() == 0) {
	    invalue = part_expand(bit, vwid, base);
      } അല്ലെങ്കിൽ {
	    unsigned wid = bit.size();
	    പ്രതിജ്ഞാ(invalue.size() == vwid);
	    വേണ്ടി (unsigned idx = 0; idx < wid ; idx += 1) {
		  എങ്കിൽ ((base+idx) >= vwid)
			നിർത്തുക;
		  invalue.set_bit(base+idx, bit.value(idx));
	    }
      }

      island_->flag_island();
}

ശൂന്യം vvp_island_port::force_flag(തർക്കം run_now)
{
      എങ്കിൽ (run_now)
	    island_->run_island();
      അല്ലെങ്കിൽ
	    island_->flag_island();
}

vvp_island_branch::~vvp_island_branch()
{
}

ശൂന്യം island_collect_കണ്ണി(list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur)
{
      conn .push_back(cur);
      വേണ്ടി (vvp_branch_ptr_t idx = next(cur) ; idx != cur ; idx = next(idx))
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
സ്ഥിതം ചിഹ്നം_map_s<vvp_island>* island_table = 0;
സ്ഥിതം vvp_island** island_list = 0;
സ്ഥിതം unsigned island_എണ്ണം = 0;

#ifdef CHECK_WITH_VALGRIND
ശൂന്യം island_delete()
{
      at_EOS = സത്യം;
      വേണ്ടി (unsigned idx = 0; idx < island_എണ്ണം; idx += 1) {
	    delete island_list[idx];
      }
      സ്വതന്ത്രം(island_list);
      island_list = 0;
      island_എണ്ണം = 0;
}
#endif

ശൂന്യം compile_island_base(അക്ഷരം*label, vvp_island*use_island)
{
      എങ്കിൽ (island_table == 0)
	    island_table = പുതിയ ചിഹ്നം_map_s<vvp_island>;

      island_table->sym_set_മൂല്യം(label, use_island);
      island_എണ്ണം += 1;
      island_list = static_cast<vvp_island **>(realloc(island_list,
                                               island_എണ്ണം*വലുപ്പം(vvp_island **)));
      island_list[island_എണ്ണം-1] = use_island;
      സ്വതന്ത്രം(label);
}

vvp_island* compile_find_island(സ്ഥിരം അക്ഷരം*island)
{
      പ്രതിജ്ഞാ(island_table);
      vvp_island*use_island = island_table->sym_get_മൂല്യം(island);
      പ്രതിജ്ഞാ(use_island);
      തിരികെ use_island;
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
ശൂന്യം compile_island_port(അക്ഷരം*label, അക്ഷരം*island, അക്ഷരം*src)
{
      പ്രതിജ്ഞാ(island_table);
      vvp_island*use_island = island_table->sym_get_മൂല്യം(island);
      പ്രതിജ്ഞാ(use_island);
      സ്വതന്ത്രം(island);

      vvp_net_t*net = പുതിയ vvp_net_t;
      vvp_island_port*fun = പുതിയ vvp_island_port(use_island);
      net->fun = fun;

	// Get the source from outside the island
      നിവേശം_connect(net, 0, src);

	// Define the functor outside the island.
      define_functor_ചിഹ്നം(label, net);

	// Also define it inside the island.
      use_island->add_port(label, net);

      സ്വതന്ത്രം(label);
}

ശൂന്യം compile_island_export(അക്ഷരം*label, അക്ഷരം*island)
{
      fprintf(stderr, "XXXX %s .export %s;\n", label, island);
      സ്വതന്ത്രം(label);
      സ്വതന്ത്രം(island);
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
ശൂന്യം compile_island_import(അക്ഷരം*label, അക്ഷരം*island, അക്ഷരം*src)
{
      പ്രതിജ്ഞാ(island_table);
      vvp_island*use_island = island_table->sym_get_മൂല്യം(island);
      പ്രതിജ്ഞാ(use_island);
      സ്വതന്ത്രം(island);

      vvp_net_t*net = പുതിയ vvp_net_t;
      vvp_island_port*fun = പുതിയ vvp_island_port(use_island);
      net->fun = fun;

	// Get the source from outside the island
      നിവേശം_connect(net, 0, src);

	// Define the functor only inside the island.
      use_island->add_port(label, net);

      സ്വതന്ത്രം(label);
}

ശൂന്യം compile_island_cleanup(ശൂന്യം)
{
	// Call the per-island cleanup to get rid of local symbol tables.
      വേണ്ടി (unsigned idx = 0; idx < island_എണ്ണം; idx += 1) {
	    island_list[idx]->compile_cleanup();
      }

	// If we are not doing valgrind checking then free the list.
#ifndef CHECK_WITH_VALGRIND
      സ്വതന്ത്രം(island_list);
      island_list = 0;
      island_എണ്ണം = 0;
#endif

	// Remove the island symbol table itself.
      delete island_table;
      island_table = 0;
}
