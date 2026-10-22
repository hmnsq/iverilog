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
#که_تعریف_شوی CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#د_که_پای
# include  <iostream>
# include  <list>
# include  <cassert>
# include  <cstdlib>
# include  <cstring>
# include "ivl_alloc.h"

کارول نطاق std;

#که_تعریف_شوی CHECK_WITH_VALGRIND
ساكن منطقي at_EOS = خطأ;
#د_که_پای

فراغ island_send_ارزښت(vvp_net_t*net, ثابت vvp_vector8_t&val)
{
      vvp_island_پورټ*fun = dynamic_cast<vvp_island_پورټ*>(net->fun);
      إذا (fun->outvalue .eeq(val))
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
#که_تعریف_شوی CHECK_WITH_VALGRIND
      إذا (!at_EOS) أكد(0);
#د_که_پای

      طالما (branches_) {
	    vvp_island_branch *بل_br = branches_->بل_branch;
	    ړنګول branches_;
	    branches_ = بل_br;
      }
}

فراغ vvp_island::flag_island()
{
      إذا (flagged_ == ریښتیا)
	    أرجع;

      مهالوېش_generic(هذا, 0, خطأ, خطأ);
      flagged_ = ریښتیا;
}

/*
* This method handles the callback from the scheduler. It does basic
* housecleaning and calls the run_island() method implemented by the
* derived class.
*/
فراغ vvp_island::پرلپسې_پرلپسې()
{
      flagged_ = خطأ;
      پرلپسې_island();
}


فراغ vvp_island::جمع_کړه_پورټ(ثابت حرف*کلی, vvp_net_t*net)
{
      إذا (ports_ == 0)
	    ports_ = جديد نښه_map_s<vvp_net_t>;

	// each port should have a unique label
      أكد(ports_->sym_get_ارزښت(کلی) == 0);

      ports_->sym_سېټ_ارزښت(کلی, net);
}

فراغ vvp_island::جمع_کړه_branch(vvp_island_branch*branch, ثابت حرف*pa, ثابت حرف*pb)
{
      vvp_island_branch*cur;
      أكد(ports_);
      branch->a = ports_->sym_get_ارزښت(pa);
      branch->b = ports_->sym_get_ارزښت(pb);
      أكد(branch->a && branch->b);

      vvp_branch_ptr_t ptra (branch, 0);
      vvp_branch_ptr_t ptrb (branch, 1);
      إذا (anodes_ == 0)
	    anodes_ = جديد نښه_map_s<vvp_island_branch>;
      إذا (bnodes_ == 0)
	    bnodes_ = جديد نښه_map_s<vvp_island_branch>;

      إذا ((cur = anodes_->sym_get_ارزښت(pa))) {
	    branch->link[0] = cur->link[0];
	    cur->link[0] = ptra;
      } وإلا إذا ((cur = bnodes_->sym_get_ارزښت(pa))) {
	    branch->link[0] = cur->link[1];
	    cur->link[1] = ptra;
      } وإلا {
	    branch->link[0] = ptra;
	    anodes_->sym_سېټ_ارزښت(pa, branch);
      }

      إذا ((cur = anodes_->sym_get_ارزښت(pb))) {
	    branch->link[1] = cur->link[0];
	    cur->link[0] = ptrb;
      } وإلا إذا ((cur = bnodes_->sym_get_ارزښت(pb))) {
	    branch->link[1] = cur->link[1];
	    cur->link[1] = ptrb;
      } وإلا {
	    branch->link[1] = ptrb;
	    bnodes_->sym_سېټ_ارزښت(pb, branch);
      }

      branch->بل_branch = branches_;
      branches_ = branch;
}

vvp_net_t* vvp_island::پیدا_کړه_پورټ(ثابت حرف*کلی)
{
      إذا (ports_ == 0)
	    أرجع 0;
      وإلا
	    أرجع ports_->sym_get_ارزښت(کلی);
}

فراغ vvp_island::compile_cleanup()
{
      ړنګول ports_;
      ports_ = 0;

      ړنګول anodes_;
      anodes_ = 0;

      ړنګول bnodes_;
      bnodes_ = 0;
}

vvp_island_پورټ::vvp_island_پورټ(vvp_island*ip)
: island_(ip)
{
}

vvp_island_پورټ::~vvp_island_پورټ()
{
}

فراغ vvp_island_پورټ::recv_vec4(vvp_net_ptr_t, ثابت vvp_vector4_t&بټ,
                                vvp_context_t)
{
      vvp_vector8_t tmp (بټ, 6, 6);
      إذا (invalue .eeq(tmp))
	    أرجع;

      invalue = tmp;
      island_->flag_island();
}

فراغ vvp_island_پورټ::recv_vec4_pv(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
				   بینښه بنسټ, بینښه vwid, vvp_context_t)
{
      vvp_vector8_t tmp(بټ, 6, 6);
      recv_vec8_pv(پورټ, tmp, بنسټ, vwid);
}


فراغ vvp_island_پورټ::recv_vec8(vvp_net_ptr_t, ثابت vvp_vector8_t&بټ)
{
      إذا (invalue .eeq(بټ))
	    أرجع;

      invalue = بټ;
      island_->flag_island();
}

فراغ vvp_island_پورټ::recv_vec8_pv(vvp_net_ptr_t, ثابت vvp_vector8_t&بټ,
                                   بینښه بنسټ, بینښه vwid)
{
      إذا (invalue.size() == 0) {
	    invalue = part_expand(بټ, vwid, بنسټ);
      } وإلا {
	    بینښه wid = بټ.size();
	    أكد(invalue.size() == vwid);
	    لكل (بینښه idx = 0; idx < wid ; idx += 1) {
		  إذا ((بنسټ+idx) >= vwid)
			اكسر;
		  invalue.سېټ_بټ(بنسټ+idx, بټ.value(idx));
	    }
      }

      island_->flag_island();
}

فراغ vvp_island_پورټ::force_flag(منطقي پرلپسې_now)
{
      إذا (پرلپسې_now)
	    island_->پرلپسې_island();
      وإلا
	    island_->flag_island();
}

vvp_island_branch::~vvp_island_branch()
{
}

فراغ island_collect_غوټه(list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur)
{
      conn .push_back(cur);
      لكل (vvp_branch_ptr_t idx = بل_2(cur) ; idx != cur ; idx = بل_2(idx))
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
ساكن نښه_map_s<vvp_island>* island_جدول = 0;
ساكن vvp_island** island_list = 0;
ساكن بینښه island_شمېر = 0;

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ island_delete()
{
      at_EOS = ریښتیا;
      لكل (بینښه idx = 0; idx < island_شمېر; idx += 1) {
	    ړنګول island_list[idx];
      }
      حرر(island_list);
      island_list = 0;
      island_شمېر = 0;
}
#د_که_پای

فراغ compile_island_بنسټ(حرف*label, vvp_island*use_island)
{
      إذا (island_جدول == 0)
	    island_جدول = جديد نښه_map_s<vvp_island>;

      island_جدول->sym_سېټ_ارزښت(label, use_island);
      island_شمېر += 1;
      island_list = static_cast<vvp_island **>(realloc(island_list,
                                               island_شمېر*حجم(vvp_island **)));
      island_list[island_شمېر-1] = use_island;
      حرر(label);
}

vvp_island* compile_پیدا_کړه_island(ثابت حرف*island)
{
      أكد(island_جدول);
      vvp_island*use_island = island_جدول->sym_get_ارزښت(island);
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
فراغ compile_island_پورټ(حرف*label, حرف*island, حرف*src)
{
      أكد(island_جدول);
      vvp_island*use_island = island_جدول->sym_get_ارزښت(island);
      أكد(use_island);
      حرر(island);

      vvp_net_t*net = جديد vvp_net_t;
      vvp_island_پورټ*fun = جديد vvp_island_پورټ(use_island);
      net->fun = fun;

	// Get the source from outside the island
      ننوت_connect(net, 0, src);

	// Define the functor outside the island.
      define_functor_نښه(label, net);

	// Also define it inside the island.
      use_island->جمع_کړه_پورټ(label, net);

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
      أكد(island_جدول);
      vvp_island*use_island = island_جدول->sym_get_ارزښت(island);
      أكد(use_island);
      حرر(island);

      vvp_net_t*net = جديد vvp_net_t;
      vvp_island_پورټ*fun = جديد vvp_island_پورټ(use_island);
      net->fun = fun;

	// Get the source from outside the island
      ننوت_connect(net, 0, src);

	// Define the functor only inside the island.
      use_island->جمع_کړه_پورټ(label, net);

      حرر(label);
}

فراغ compile_island_cleanup(فراغ)
{
	// Call the per-island cleanup to get rid of local symbol tables.
      لكل (بینښه idx = 0; idx < island_شمېر; idx += 1) {
	    island_list[idx]->compile_cleanup();
      }

	// If we are not doing valgrind checking then free the list.
#که_نه_تعریف_شوی CHECK_WITH_VALGRIND
      حرر(island_list);
      island_list = 0;
      island_شمېر = 0;
#د_که_پای

	// Remove the island symbol table itself.
      ړنګول island_جدول;
      island_جدول = 0;
}
