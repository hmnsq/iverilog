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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ਜੇ_ਅੰਤ
# include  <iostream>
# include  <list>
# include  <cassert>
# include  <cstdlib>
# include  <cstring>
# include "ivl_alloc.h"

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਸਥਿਰ ਤਾਰਕਿਕ at_EOS = ਝੂਠ;
#ਜੇ_ਅੰਤ

ਖਾਲੀ island_send_ਮੁੱਲ(vvp_net_t*net, ਅਚਲ vvp_vector8_t&val)
{
      vvp_island_ਪੋਰਟ*fun = dynamic_cast<vvp_island_ਪੋਰਟ*>(net->fun);
      ਜੇ (fun->outvalue .eeq(val))
	    ਵਾਪਸ;

      fun->outvalue = val;
      net->send_vec8(fun->outvalue);
}

/*
 * Implementations...
 */
vvp_island::vvp_island()
{
      flagged_ = ਝੂਠ;
      branches_ = 0;
      ports_ = 0;
      anodes_ = 0;
      bnodes_ = 0;
}

vvp_island::~vvp_island()
{
	// We can only delete islands at the end of simulation.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਜੇ (!at_EOS) ਪ੍ਰਤਿਜ੍ਞਾ(0);
#ਜੇ_ਅੰਤ

      ਜਦੋਂ (branches_) {
	    vvp_island_branch *ਅਗਲਾ_br = branches_->ਅਗਲਾ_branch;
	    ਮਿਟਾਓ branches_;
	    branches_ = ਅਗਲਾ_br;
      }
}

ਖਾਲੀ vvp_island::flag_island()
{
      ਜੇ (flagged_ == ਸੱਚ)
	    ਵਾਪਸ;

      ਸਮਾਂ_ਸਾਰਣੀ_generic(ਇਹ, 0, ਝੂਠ, ਝੂਠ);
      flagged_ = ਸੱਚ;
}

/*
* This method handles the callback from the scheduler. It does basic
* housecleaning and calls the run_island() method implemented by the
* derived class.
*/
ਖਾਲੀ vvp_island::ਲੜੀਵਾਰ_ਲੜੀਵਾਰ()
{
      flagged_ = ਝੂਠ;
      ਲੜੀਵਾਰ_island();
}


ਖਾਲੀ vvp_island::ਜੋੜੋ_ਪੋਰਟ(ਅਚਲ ਅੱਖਰ*ਕੁੰਜੀ, vvp_net_t*net)
{
      ਜੇ (ports_ == 0)
	    ports_ = ਨਵਾਂ ਚਿੰਨ੍ਹ_map_s<vvp_net_t>;

	// each port should have a unique label
      ਪ੍ਰਤਿਜ੍ਞਾ(ports_->sym_get_ਮੁੱਲ(ਕੁੰਜੀ) == 0);

      ports_->sym_ਸੈੱਟ_ਮੁੱਲ(ਕੁੰਜੀ, net);
}

ਖਾਲੀ vvp_island::ਜੋੜੋ_branch(vvp_island_branch*branch, ਅਚਲ ਅੱਖਰ*pa, ਅਚਲ ਅੱਖਰ*pb)
{
      vvp_island_branch*cur;
      ਪ੍ਰਤਿਜ੍ਞਾ(ports_);
      branch->a = ports_->sym_get_ਮੁੱਲ(pa);
      branch->b = ports_->sym_get_ਮੁੱਲ(pb);
      ਪ੍ਰਤਿਜ੍ਞਾ(branch->a && branch->b);

      vvp_branch_ptr_t ptra (branch, 0);
      vvp_branch_ptr_t ptrb (branch, 1);
      ਜੇ (anodes_ == 0)
	    anodes_ = ਨਵਾਂ ਚਿੰਨ੍ਹ_map_s<vvp_island_branch>;
      ਜੇ (bnodes_ == 0)
	    bnodes_ = ਨਵਾਂ ਚਿੰਨ੍ਹ_map_s<vvp_island_branch>;

      ਜੇ ((cur = anodes_->sym_get_ਮੁੱਲ(pa))) {
	    branch->link[0] = cur->link[0];
	    cur->link[0] = ptra;
      } ਨਹੀਂਤਾਂ ਜੇ ((cur = bnodes_->sym_get_ਮੁੱਲ(pa))) {
	    branch->link[0] = cur->link[1];
	    cur->link[1] = ptra;
      } ਨਹੀਂਤਾਂ {
	    branch->link[0] = ptra;
	    anodes_->sym_ਸੈੱਟ_ਮੁੱਲ(pa, branch);
      }

      ਜੇ ((cur = anodes_->sym_get_ਮੁੱਲ(pb))) {
	    branch->link[1] = cur->link[0];
	    cur->link[0] = ptrb;
      } ਨਹੀਂਤਾਂ ਜੇ ((cur = bnodes_->sym_get_ਮੁੱਲ(pb))) {
	    branch->link[1] = cur->link[1];
	    cur->link[1] = ptrb;
      } ਨਹੀਂਤਾਂ {
	    branch->link[1] = ptrb;
	    bnodes_->sym_ਸੈੱਟ_ਮੁੱਲ(pb, branch);
      }

      branch->ਅਗਲਾ_branch = branches_;
      branches_ = branch;
}

vvp_net_t* vvp_island::ਲੱਭੋ_ਪੋਰਟ(ਅਚਲ ਅੱਖਰ*ਕੁੰਜੀ)
{
      ਜੇ (ports_ == 0)
	    ਵਾਪਸ 0;
      ਨਹੀਂਤਾਂ
	    ਵਾਪਸ ports_->sym_get_ਮੁੱਲ(ਕੁੰਜੀ);
}

ਖਾਲੀ vvp_island::compile_cleanup()
{
      ਮਿਟਾਓ ports_;
      ports_ = 0;

      ਮਿਟਾਓ anodes_;
      anodes_ = 0;

      ਮਿਟਾਓ bnodes_;
      bnodes_ = 0;
}

vvp_island_ਪੋਰਟ::vvp_island_ਪੋਰਟ(vvp_island*ip)
: island_(ip)
{
}

vvp_island_ਪੋਰਟ::~vvp_island_ਪੋਰਟ()
{
}

ਖਾਲੀ vvp_island_ਪੋਰਟ::recv_vec4(vvp_net_ptr_t, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                vvp_context_t)
{
      vvp_vector8_t tmp (ਦੋਆਧਾਰੀ_ਅੰਕ, 6, 6);
      ਜੇ (invalue .eeq(tmp))
	    ਵਾਪਸ;

      invalue = tmp;
      island_->flag_island();
}

ਖਾਲੀ vvp_island_ਪੋਰਟ::recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
				   ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t)
{
      vvp_vector8_t tmp(ਦੋਆਧਾਰੀ_ਅੰਕ, 6, 6);
      recv_vec8_pv(ਪੋਰਟ, tmp, ਅਧਾਰ, vwid);
}


ਖਾਲੀ vvp_island_ਪੋਰਟ::recv_vec8(vvp_net_ptr_t, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ)
{
      ਜੇ (invalue .eeq(ਦੋਆਧਾਰੀ_ਅੰਕ))
	    ਵਾਪਸ;

      invalue = ਦੋਆਧਾਰੀ_ਅੰਕ;
      island_->flag_island();
}

ਖਾਲੀ vvp_island_ਪੋਰਟ::recv_vec8_pv(vvp_net_ptr_t, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                                   ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid)
{
      ਜੇ (invalue.size() == 0) {
	    invalue = part_expand(ਦੋਆਧਾਰੀ_ਅੰਕ, vwid, ਅਧਾਰ);
      } ਨਹੀਂਤਾਂ {
	    ਬਿਨਾਨਿਸ਼ਾਨੀ wid = ਦੋਆਧਾਰੀ_ਅੰਕ.size();
	    ਪ੍ਰਤਿਜ੍ਞਾ(invalue.size() == vwid);
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < wid ; idx += 1) {
		  ਜੇ ((ਅਧਾਰ+idx) >= vwid)
			ਰੋਕੋ;
		  invalue.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਧਾਰ+idx, ਦੋਆਧਾਰੀ_ਅੰਕ.value(idx));
	    }
      }

      island_->flag_island();
}

ਖਾਲੀ vvp_island_ਪੋਰਟ::force_flag(ਤਾਰਕਿਕ ਲੜੀਵਾਰ_now)
{
      ਜੇ (ਲੜੀਵਾਰ_now)
	    island_->ਲੜੀਵਾਰ_island();
      ਨਹੀਂਤਾਂ
	    island_->flag_island();
}

vvp_island_branch::~vvp_island_branch()
{
}

ਖਾਲੀ island_collect_ਗੰਢ(list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur)
{
      conn .push_back(cur);
      ਲਈ (vvp_branch_ptr_t idx = ਅਗਲਾ_2(cur) ; idx != cur ; idx = ਅਗਲਾ_2(idx))
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
ਸਥਿਰ ਚਿੰਨ੍ਹ_map_s<vvp_island>* island_ਸਾਰਣੀ = 0;
ਸਥਿਰ vvp_island** island_list = 0;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ island_ਗਿਣਤੀ = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ island_delete()
{
      at_EOS = ਸੱਚ;
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < island_ਗਿਣਤੀ; idx += 1) {
	    ਮਿਟਾਓ island_list[idx];
      }
      ਮੁਕਤ(island_list);
      island_list = 0;
      island_ਗਿਣਤੀ = 0;
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ compile_island_ਅਧਾਰ(ਅੱਖਰ*label, vvp_island*use_island)
{
      ਜੇ (island_ਸਾਰਣੀ == 0)
	    island_ਸਾਰਣੀ = ਨਵਾਂ ਚਿੰਨ੍ਹ_map_s<vvp_island>;

      island_ਸਾਰਣੀ->sym_ਸੈੱਟ_ਮੁੱਲ(label, use_island);
      island_ਗਿਣਤੀ += 1;
      island_list = static_cast<vvp_island **>(realloc(island_list,
                                               island_ਗਿਣਤੀ*ਆਕਾਰ(vvp_island **)));
      island_list[island_ਗਿਣਤੀ-1] = use_island;
      ਮੁਕਤ(label);
}

vvp_island* compile_ਲੱਭੋ_island(ਅਚਲ ਅੱਖਰ*island)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(island_ਸਾਰਣੀ);
      vvp_island*use_island = island_ਸਾਰਣੀ->sym_get_ਮੁੱਲ(island);
      ਪ੍ਰਤਿਜ੍ਞਾ(use_island);
      ਵਾਪਸ use_island;
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
ਖਾਲੀ compile_island_ਪੋਰਟ(ਅੱਖਰ*label, ਅੱਖਰ*island, ਅੱਖਰ*src)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(island_ਸਾਰਣੀ);
      vvp_island*use_island = island_ਸਾਰਣੀ->sym_get_ਮੁੱਲ(island);
      ਪ੍ਰਤਿਜ੍ਞਾ(use_island);
      ਮੁਕਤ(island);

      vvp_net_t*net = ਨਵਾਂ vvp_net_t;
      vvp_island_ਪੋਰਟ*fun = ਨਵਾਂ vvp_island_ਪੋਰਟ(use_island);
      net->fun = fun;

	// Get the source from outside the island
      ਨਿਵੇਸ਼_connect(net, 0, src);

	// Define the functor outside the island.
      define_functor_ਚਿੰਨ੍ਹ(label, net);

	// Also define it inside the island.
      use_island->ਜੋੜੋ_ਪੋਰਟ(label, net);

      ਮੁਕਤ(label);
}

ਖਾਲੀ compile_island_export(ਅੱਖਰ*label, ਅੱਖਰ*island)
{
      fprintf(stderr, "XXXX %s .export %s;\n", label, island);
      ਮੁਕਤ(label);
      ਮੁਕਤ(island);
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
ਖਾਲੀ compile_island_import(ਅੱਖਰ*label, ਅੱਖਰ*island, ਅੱਖਰ*src)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(island_ਸਾਰਣੀ);
      vvp_island*use_island = island_ਸਾਰਣੀ->sym_get_ਮੁੱਲ(island);
      ਪ੍ਰਤਿਜ੍ਞਾ(use_island);
      ਮੁਕਤ(island);

      vvp_net_t*net = ਨਵਾਂ vvp_net_t;
      vvp_island_ਪੋਰਟ*fun = ਨਵਾਂ vvp_island_ਪੋਰਟ(use_island);
      net->fun = fun;

	// Get the source from outside the island
      ਨਿਵੇਸ਼_connect(net, 0, src);

	// Define the functor only inside the island.
      use_island->ਜੋੜੋ_ਪੋਰਟ(label, net);

      ਮੁਕਤ(label);
}

ਖਾਲੀ compile_island_cleanup(ਖਾਲੀ)
{
	// Call the per-island cleanup to get rid of local symbol tables.
      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0; idx < island_ਗਿਣਤੀ; idx += 1) {
	    island_list[idx]->compile_cleanup();
      }

	// If we are not doing valgrind checking then free the list.
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਮੁਕਤ(island_list);
      island_list = 0;
      island_ਗਿਣਤੀ = 0;
#ਜੇ_ਅੰਤ

	// Remove the island symbol table itself.
      ਮਿਟਾਓ island_ਸਾਰਣੀ;
      island_ਸਾਰਣੀ = 0;
}
