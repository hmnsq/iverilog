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
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#جے_انت
# include  <iostream>
# include  <list>
# include  <cassert>
# include  <cstdlib>
# include  <cstring>
# include "ivl_alloc.h"

ورتوں نطاق std;

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
ساكن منطقي at_EOS = خطأ;
#جے_انت

فراغ island_send_قدر(vvp_net_t*net, ثابت vvp_vector8_t&val)
{
      vvp_island_پورٹ*fun = dynamic_cast<vvp_island_پورٹ*>(net->fun);
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
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
      إذا (!at_EOS) أكد(0);
#جے_انت

      طالما (branches_) {
	    vvp_island_branch *اگلا_br = branches_->اگلا_branch;
	    مٹاؤ branches_;
	    branches_ = اگلا_br;
      }
}

فراغ vvp_island::flag_island()
{
      إذا (flagged_ == سچ)
	    أرجع;

      ویلا_سارنی_generic(هذا, 0, خطأ, خطأ);
      flagged_ = سچ;
}

/*
* This method handles the callback from the scheduler. It does basic
* housecleaning and calls the run_island() method implemented by the
* derived class.
*/
فراغ vvp_island::لگاتار_لگاتار()
{
      flagged_ = خطأ;
      لگاتار_island();
}


فراغ vvp_island::جوڑو_پورٹ(ثابت حرف*کلید, vvp_net_t*net)
{
      إذا (ports_ == 0)
	    ports_ = جديد علامت_map_s<vvp_net_t>;

	// each port should have a unique label
      أكد(ports_->sym_get_قدر(کلید) == 0);

      ports_->sym_سیٹ_قدر(کلید, net);
}

فراغ vvp_island::جوڑو_branch(vvp_island_branch*branch, ثابت حرف*pa, ثابت حرف*pb)
{
      vvp_island_branch*cur;
      أكد(ports_);
      branch->a = ports_->sym_get_قدر(pa);
      branch->b = ports_->sym_get_قدر(pb);
      أكد(branch->a && branch->b);

      vvp_branch_ptr_t ptra (branch, 0);
      vvp_branch_ptr_t ptrb (branch, 1);
      إذا (anodes_ == 0)
	    anodes_ = جديد علامت_map_s<vvp_island_branch>;
      إذا (bnodes_ == 0)
	    bnodes_ = جديد علامت_map_s<vvp_island_branch>;

      إذا ((cur = anodes_->sym_get_قدر(pa))) {
	    branch->link[0] = cur->link[0];
	    cur->link[0] = ptra;
      } وإلا إذا ((cur = bnodes_->sym_get_قدر(pa))) {
	    branch->link[0] = cur->link[1];
	    cur->link[1] = ptra;
      } وإلا {
	    branch->link[0] = ptra;
	    anodes_->sym_سیٹ_قدر(pa, branch);
      }

      إذا ((cur = anodes_->sym_get_قدر(pb))) {
	    branch->link[1] = cur->link[0];
	    cur->link[0] = ptrb;
      } وإلا إذا ((cur = bnodes_->sym_get_قدر(pb))) {
	    branch->link[1] = cur->link[1];
	    cur->link[1] = ptrb;
      } وإلا {
	    branch->link[1] = ptrb;
	    bnodes_->sym_سیٹ_قدر(pb, branch);
      }

      branch->اگلا_branch = branches_;
      branches_ = branch;
}

vvp_net_t* vvp_island::لبھو_پورٹ(ثابت حرف*کلید)
{
      إذا (ports_ == 0)
	    أرجع 0;
      وإلا
	    أرجع ports_->sym_get_قدر(کلید);
}

فراغ vvp_island::compile_cleanup()
{
      مٹاؤ ports_;
      ports_ = 0;

      مٹاؤ anodes_;
      anodes_ = 0;

      مٹاؤ bnodes_;
      bnodes_ = 0;
}

vvp_island_پورٹ::vvp_island_پورٹ(vvp_island*ip)
: island_(ip)
{
}

vvp_island_پورٹ::~vvp_island_پورٹ()
{
}

فراغ vvp_island_پورٹ::recv_vec4(vvp_net_ptr_t, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                vvp_context_t)
{
      vvp_vector8_t tmp (ثنائی_ہندسہ, 6, 6);
      إذا (invalue .eeq(tmp))
	    أرجع;

      invalue = tmp;
      island_->flag_island();
}

فراغ vvp_island_پورٹ::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				   بنانشانی ادھار, بنانشانی vwid, vvp_context_t)
{
      vvp_vector8_t tmp(ثنائی_ہندسہ, 6, 6);
      recv_vec8_pv(پورٹ, tmp, ادھار, vwid);
}


فراغ vvp_island_پورٹ::recv_vec8(vvp_net_ptr_t, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      إذا (invalue .eeq(ثنائی_ہندسہ))
	    أرجع;

      invalue = ثنائی_ہندسہ;
      island_->flag_island();
}

فراغ vvp_island_پورٹ::recv_vec8_pv(vvp_net_ptr_t, ثابت vvp_vector8_t&ثنائی_ہندسہ,
                                   بنانشانی ادھار, بنانشانی vwid)
{
      إذا (invalue.size() == 0) {
	    invalue = part_expand(ثنائی_ہندسہ, vwid, ادھار);
      } وإلا {
	    بنانشانی wid = ثنائی_ہندسہ.size();
	    أكد(invalue.size() == vwid);
	    لكل (بنانشانی idx = 0; idx < wid ; idx += 1) {
		  إذا ((ادھار+idx) >= vwid)
			اكسر;
		  invalue.سیٹ_ثنائی_ہندسہ(ادھار+idx, ثنائی_ہندسہ.value(idx));
	    }
      }

      island_->flag_island();
}

فراغ vvp_island_پورٹ::force_flag(منطقي لگاتار_now)
{
      إذا (لگاتار_now)
	    island_->لگاتار_island();
      وإلا
	    island_->flag_island();
}

vvp_island_branch::~vvp_island_branch()
{
}

فراغ island_collect_گرہ(list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur)
{
      conn .push_back(cur);
      لكل (vvp_branch_ptr_t idx = اگلا_2(cur) ; idx != cur ; idx = اگلا_2(idx))
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
ساكن علامت_map_s<vvp_island>* island_جدول = 0;
ساكن vvp_island** island_list = 0;
ساكن بنانشانی island_گنتی = 0;

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ island_delete()
{
      at_EOS = سچ;
      لكل (بنانشانی idx = 0; idx < island_گنتی; idx += 1) {
	    مٹاؤ island_list[idx];
      }
      حرر(island_list);
      island_list = 0;
      island_گنتی = 0;
}
#جے_انت

فراغ compile_island_ادھار(حرف*label, vvp_island*use_island)
{
      إذا (island_جدول == 0)
	    island_جدول = جديد علامت_map_s<vvp_island>;

      island_جدول->sym_سیٹ_قدر(label, use_island);
      island_گنتی += 1;
      island_list = static_cast<vvp_island **>(realloc(island_list,
                                               island_گنتی*حجم(vvp_island **)));
      island_list[island_گنتی-1] = use_island;
      حرر(label);
}

vvp_island* compile_لبھو_island(ثابت حرف*island)
{
      أكد(island_جدول);
      vvp_island*use_island = island_جدول->sym_get_قدر(island);
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
فراغ compile_island_پورٹ(حرف*label, حرف*island, حرف*src)
{
      أكد(island_جدول);
      vvp_island*use_island = island_جدول->sym_get_قدر(island);
      أكد(use_island);
      حرر(island);

      vvp_net_t*net = جديد vvp_net_t;
      vvp_island_پورٹ*fun = جديد vvp_island_پورٹ(use_island);
      net->fun = fun;

	// Get the source from outside the island
      داخلہ_connect(net, 0, src);

	// Define the functor outside the island.
      define_functor_علامت(label, net);

	// Also define it inside the island.
      use_island->جوڑو_پورٹ(label, net);

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
      vvp_island*use_island = island_جدول->sym_get_قدر(island);
      أكد(use_island);
      حرر(island);

      vvp_net_t*net = جديد vvp_net_t;
      vvp_island_پورٹ*fun = جديد vvp_island_پورٹ(use_island);
      net->fun = fun;

	// Get the source from outside the island
      داخلہ_connect(net, 0, src);

	// Define the functor only inside the island.
      use_island->جوڑو_پورٹ(label, net);

      حرر(label);
}

فراغ compile_island_cleanup(فراغ)
{
	// Call the per-island cleanup to get rid of local symbol tables.
      لكل (بنانشانی idx = 0; idx < island_گنتی; idx += 1) {
	    island_list[idx]->compile_cleanup();
      }

	// If we are not doing valgrind checking then free the list.
#جے_بنا_تعریف CHECK_WITH_VALGRIND
      حرر(island_list);
      island_list = 0;
      island_گنتی = 0;
#جے_انت

	// Remove the island symbol table itself.
      مٹاؤ island_جدول;
      island_جدول = 0;
}
