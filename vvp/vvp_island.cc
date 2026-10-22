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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ಆದರೆ_ಅಂತ್ಯ
# include  <iostream>
# include  <list>
# include  <cassert>
# include  <cstdlib>
# include  <cstring>
# include "ivl_alloc.h"

ಬಳಸಿ ನಾಮಾಕಾಶ std;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಸ್ಥಿರ ತಾರ್ಕಿಕ at_EOS = ಸುಳ್ಳು;
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ island_send_ಮೌಲ್ಯ(vvp_net_t*net, ಸ್ಥಿರಾಂಕ vvp_vector8_t&val)
{
      vvp_island_ಪೋರ್ಟ್*fun = dynamic_cast<vvp_island_ಪೋರ್ಟ್*>(net->fun);
      ಆದರೆ (fun->outvalue .eeq(val))
	    ಹಿಂತಿರುಗಿ;

      fun->outvalue = val;
      net->send_vec8(fun->outvalue);
}

/*
 * Implementations...
 */
vvp_island::vvp_island()
{
      flagged_ = ಸುಳ್ಳು;
      branches_ = 0;
      ports_ = 0;
      anodes_ = 0;
      bnodes_ = 0;
}

vvp_island::~vvp_island()
{
	// We can only delete islands at the end of simulation.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      ಆದರೆ (!at_EOS) ಪ್ರತಿಜ್ಞಾ(0);
#ಆದರೆ_ಅಂತ್ಯ

      ಆಗ (branches_) {
	    vvp_island_branch *ಮುಂದಿನ_br = branches_->ಮುಂದಿನ_branch;
	    ಅಳಿಸು branches_;
	    branches_ = ಮುಂದಿನ_br;
      }
}

ಶೂನ್ಯ vvp_island::flag_island()
{
      ಆದರೆ (flagged_ == ನಿಜ)
	    ಹಿಂತಿರುಗಿ;

      ವೇಳಾಪಟ್ಟಿ_generic(ಇದು, 0, ಸುಳ್ಳು, ಸುಳ್ಳು);
      flagged_ = ನಿಜ;
}

/*
* This method handles the callback from the scheduler. It does basic
* housecleaning and calls the run_island() method implemented by the
* derived class.
*/
ಶೂನ್ಯ vvp_island::ಸತತ_ಸತತ()
{
      flagged_ = ಸುಳ್ಳು;
      ಸತತ_island();
}


ಶೂನ್ಯ vvp_island::ಕೂಡಿಸು_ಪೋರ್ಟ್(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಕೀಲಿ, vvp_net_t*net)
{
      ಆದರೆ (ports_ == 0)
	    ports_ = ಹೊಸ ಚಿಹ್ನೆ_map_s<vvp_net_t>;

	// each port should have a unique label
      ಪ್ರತಿಜ್ಞಾ(ports_->sym_get_ಮೌಲ್ಯ(ಕೀಲಿ) == 0);

      ports_->sym_ಗಣ_ಮೌಲ್ಯ(ಕೀಲಿ, net);
}

ಶೂನ್ಯ vvp_island::ಕೂಡಿಸು_branch(vvp_island_branch*branch, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*pa, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*pb)
{
      vvp_island_branch*cur;
      ಪ್ರತಿಜ್ಞಾ(ports_);
      branch->a = ports_->sym_get_ಮೌಲ್ಯ(pa);
      branch->b = ports_->sym_get_ಮೌಲ್ಯ(pb);
      ಪ್ರತಿಜ್ಞಾ(branch->a && branch->b);

      vvp_branch_ptr_t ptra (branch, 0);
      vvp_branch_ptr_t ptrb (branch, 1);
      ಆದರೆ (anodes_ == 0)
	    anodes_ = ಹೊಸ ಚಿಹ್ನೆ_map_s<vvp_island_branch>;
      ಆದರೆ (bnodes_ == 0)
	    bnodes_ = ಹೊಸ ಚಿಹ್ನೆ_map_s<vvp_island_branch>;

      ಆದರೆ ((cur = anodes_->sym_get_ಮೌಲ್ಯ(pa))) {
	    branch->link[0] = cur->link[0];
	    cur->link[0] = ptra;
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((cur = bnodes_->sym_get_ಮೌಲ್ಯ(pa))) {
	    branch->link[0] = cur->link[1];
	    cur->link[1] = ptra;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    branch->link[0] = ptra;
	    anodes_->sym_ಗಣ_ಮೌಲ್ಯ(pa, branch);
      }

      ಆದರೆ ((cur = anodes_->sym_get_ಮೌಲ್ಯ(pb))) {
	    branch->link[1] = cur->link[0];
	    cur->link[0] = ptrb;
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((cur = bnodes_->sym_get_ಮೌಲ್ಯ(pb))) {
	    branch->link[1] = cur->link[1];
	    cur->link[1] = ptrb;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    branch->link[1] = ptrb;
	    bnodes_->sym_ಗಣ_ಮೌಲ್ಯ(pb, branch);
      }

      branch->ಮುಂದಿನ_branch = branches_;
      branches_ = branch;
}

vvp_net_t* vvp_island::ಕಂಡುಹಿಡಿ_ಪೋರ್ಟ್(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಕೀಲಿ)
{
      ಆದರೆ (ports_ == 0)
	    ಹಿಂತಿರುಗಿ 0;
      ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ ports_->sym_get_ಮೌಲ್ಯ(ಕೀಲಿ);
}

ಶೂನ್ಯ vvp_island::compile_cleanup()
{
      ಅಳಿಸು ports_;
      ports_ = 0;

      ಅಳಿಸು anodes_;
      anodes_ = 0;

      ಅಳಿಸು bnodes_;
      bnodes_ = 0;
}

vvp_island_ಪೋರ್ಟ್::vvp_island_ಪೋರ್ಟ್(vvp_island*ip)
: island_(ip)
{
}

vvp_island_ಪೋರ್ಟ್::~vvp_island_ಪೋರ್ಟ್()
{
}

ಶೂನ್ಯ vvp_island_ಪೋರ್ಟ್::recv_vec4(vvp_net_ptr_t, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                                vvp_context_t)
{
      vvp_vector8_t tmp (ಬಿಟ್, 6, 6);
      ಆದರೆ (invalue .eeq(tmp))
	    ಹಿಂತಿರುಗಿ;

      invalue = tmp;
      island_->flag_island();
}

ಶೂನ್ಯ vvp_island_ಪೋರ್ಟ್::recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
				   ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t)
{
      vvp_vector8_t tmp(ಬಿಟ್, 6, 6);
      recv_vec8_pv(ಪೋರ್ಟ್, tmp, ತಳ, vwid);
}


ಶೂನ್ಯ vvp_island_ಪೋರ್ಟ್::recv_vec8(vvp_net_ptr_t, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್)
{
      ಆದರೆ (invalue .eeq(ಬಿಟ್))
	    ಹಿಂತಿರುಗಿ;

      invalue = ಬಿಟ್;
      island_->flag_island();
}

ಶೂನ್ಯ vvp_island_ಪೋರ್ಟ್::recv_vec8_pv(vvp_net_ptr_t, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್,
                                   ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid)
{
      ಆದರೆ (invalue.size() == 0) {
	    invalue = part_expand(ಬಿಟ್, vwid, ತಳ);
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಚಿಹ್ನೆರಹಿತ wid = ಬಿಟ್.size();
	    ಪ್ರತಿಜ್ಞಾ(invalue.size() == vwid);
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < wid ; idx += 1) {
		  ಆದರೆ ((ತಳ+idx) >= vwid)
			ನಿಲ್ಲಿಸು;
		  invalue.ಗಣ_ಬಿಟ್(ತಳ+idx, ಬಿಟ್.value(idx));
	    }
      }

      island_->flag_island();
}

ಶೂನ್ಯ vvp_island_ಪೋರ್ಟ್::force_flag(ತಾರ್ಕಿಕ ಸತತ_now)
{
      ಆದರೆ (ಸತತ_now)
	    island_->ಸತತ_island();
      ಇಲ್ಲದಿದ್ದರೆ
	    island_->flag_island();
}

vvp_island_branch::~vvp_island_branch()
{
}

ಶೂನ್ಯ island_collect_ಗಂಟು(list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur)
{
      conn .push_back(cur);
      ಗಾಗಿ (vvp_branch_ptr_t idx = ಮುಂದಿನ_2(cur) ; idx != cur ; idx = ಮುಂದಿನ_2(idx))
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
ಸ್ಥಿರ ಚಿಹ್ನೆ_map_s<vvp_island>* island_ಕೋಷ್ಟಕ = 0;
ಸ್ಥಿರ vvp_island** island_list = 0;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ island_ಎಣಿಕೆ = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ island_delete()
{
      at_EOS = ನಿಜ;
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < island_ಎಣಿಕೆ; idx += 1) {
	    ಅಳಿಸು island_list[idx];
      }
      ಬಿಡುಗಡೆ(island_list);
      island_list = 0;
      island_ಎಣಿಕೆ = 0;
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ compile_island_ತಳ(ಅಕ್ಷರ*label, vvp_island*use_island)
{
      ಆದರೆ (island_ಕೋಷ್ಟಕ == 0)
	    island_ಕೋಷ್ಟಕ = ಹೊಸ ಚಿಹ್ನೆ_map_s<vvp_island>;

      island_ಕೋಷ್ಟಕ->sym_ಗಣ_ಮೌಲ್ಯ(label, use_island);
      island_ಎಣಿಕೆ += 1;
      island_list = static_cast<vvp_island **>(realloc(island_list,
                                               island_ಎಣಿಕೆ*ಗಾತ್ರ(vvp_island **)));
      island_list[island_ಎಣಿಕೆ-1] = use_island;
      ಬಿಡುಗಡೆ(label);
}

vvp_island* compile_ಕಂಡುಹಿಡಿ_island(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*island)
{
      ಪ್ರತಿಜ್ಞಾ(island_ಕೋಷ್ಟಕ);
      vvp_island*use_island = island_ಕೋಷ್ಟಕ->sym_get_ಮೌಲ್ಯ(island);
      ಪ್ರತಿಜ್ಞಾ(use_island);
      ಹಿಂತಿರುಗಿ use_island;
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
ಶೂನ್ಯ compile_island_ಪೋರ್ಟ್(ಅಕ್ಷರ*label, ಅಕ್ಷರ*island, ಅಕ್ಷರ*src)
{
      ಪ್ರತಿಜ್ಞಾ(island_ಕೋಷ್ಟಕ);
      vvp_island*use_island = island_ಕೋಷ್ಟಕ->sym_get_ಮೌಲ್ಯ(island);
      ಪ್ರತಿಜ್ಞಾ(use_island);
      ಬಿಡುಗಡೆ(island);

      vvp_net_t*net = ಹೊಸ vvp_net_t;
      vvp_island_ಪೋರ್ಟ್*fun = ಹೊಸ vvp_island_ಪೋರ್ಟ್(use_island);
      net->fun = fun;

	// Get the source from outside the island
      ಒಳಹರಿವು_connect(net, 0, src);

	// Define the functor outside the island.
      define_functor_ಚಿಹ್ನೆ(label, net);

	// Also define it inside the island.
      use_island->ಕೂಡಿಸು_ಪೋರ್ಟ್(label, net);

      ಬಿಡುಗಡೆ(label);
}

ಶೂನ್ಯ compile_island_export(ಅಕ್ಷರ*label, ಅಕ್ಷರ*island)
{
      fprintf(stderr, "XXXX %s .export %s;\n", label, island);
      ಬಿಡುಗಡೆ(label);
      ಬಿಡುಗಡೆ(island);
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
ಶೂನ್ಯ compile_island_import(ಅಕ್ಷರ*label, ಅಕ್ಷರ*island, ಅಕ್ಷರ*src)
{
      ಪ್ರತಿಜ್ಞಾ(island_ಕೋಷ್ಟಕ);
      vvp_island*use_island = island_ಕೋಷ್ಟಕ->sym_get_ಮೌಲ್ಯ(island);
      ಪ್ರತಿಜ್ಞಾ(use_island);
      ಬಿಡುಗಡೆ(island);

      vvp_net_t*net = ಹೊಸ vvp_net_t;
      vvp_island_ಪೋರ್ಟ್*fun = ಹೊಸ vvp_island_ಪೋರ್ಟ್(use_island);
      net->fun = fun;

	// Get the source from outside the island
      ಒಳಹರಿವು_connect(net, 0, src);

	// Define the functor only inside the island.
      use_island->ಕೂಡಿಸು_ಪೋರ್ಟ್(label, net);

      ಬಿಡುಗಡೆ(label);
}

ಶೂನ್ಯ compile_island_cleanup(ಶೂನ್ಯ)
{
	// Call the per-island cleanup to get rid of local symbol tables.
      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < island_ಎಣಿಕೆ; idx += 1) {
	    island_list[idx]->compile_cleanup();
      }

	// If we are not doing valgrind checking then free the list.
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CHECK_WITH_VALGRIND
      ಬಿಡುಗಡೆ(island_list);
      island_list = 0;
      island_ಎಣಿಕೆ = 0;
#ಆದರೆ_ಅಂತ್ಯ

	// Remove the island symbol table itself.
      ಅಳಿಸು island_ಕೋಷ್ಟಕ;
      island_ಕೋಷ್ಟಕ = 0;
}
