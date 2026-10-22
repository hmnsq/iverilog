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
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#اگر_ختم
# include  <iostream>
# include  <list>
# include  <cassert>
# include  <cstdlib>
# include  <cstring>
# include "ivl_alloc.h"

استعمال نامخلا std;

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
ساکن منطقی at_EOS = جھوٹ;
#اگر_ختم

خالی island_send_قدر(vvp_net_t*net, ثابت vvp_vector8_t&val)
{
      vvp_island_پورٹ*fun = dynamic_cast<vvp_island_پورٹ*>(net->fun);
      اگر (fun->outvalue .eeq(val))
	    واپس;

      fun->outvalue = val;
      net->send_vec8(fun->outvalue);
}

/*
 * Implementations...
 */
vvp_island::vvp_island()
{
      flagged_ = جھوٹ;
      branches_ = 0;
      ports_ = 0;
      anodes_ = 0;
      bnodes_ = 0;
}

vvp_island::~vvp_island()
{
	// We can only delete islands at the end of simulation.
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      اگر (!at_EOS) أكد(0);
#اگر_ختم

      جب (branches_) {
	    vvp_island_branch *اگلا_br = branches_->اگلا_branch;
	    حذف branches_;
	    branches_ = اگلا_br;
      }
}

خالی vvp_island::flag_island()
{
      اگر (flagged_ == سچ)
	    واپس;

      جدول_وقت_generic(یہ, 0, جھوٹ, جھوٹ);
      flagged_ = سچ;
}

/*
* This method handles the callback from the scheduler. It does basic
* housecleaning and calls the run_island() method implemented by the
* derived class.
*/
خالی vvp_island::تسلسل_تسلسل()
{
      flagged_ = جھوٹ;
      تسلسل_island();
}


خالی vvp_island::جوڑو_پورٹ(ثابت حرف*کلید, vvp_net_t*net)
{
      اگر (ports_ == 0)
	    ports_ = نیا علامت_map_s<vvp_net_t>;

	// each port should have a unique label
      أكد(ports_->sym_get_قدر(کلید) == 0);

      ports_->sym_سیٹ_قدر(کلید, net);
}

خالی vvp_island::جوڑو_branch(vvp_island_branch*branch, ثابت حرف*pa, ثابت حرف*pb)
{
      vvp_island_branch*cur;
      أكد(ports_);
      branch->a = ports_->sym_get_قدر(pa);
      branch->b = ports_->sym_get_قدر(pb);
      أكد(branch->a && branch->b);

      vvp_branch_ptr_t ptra (branch, 0);
      vvp_branch_ptr_t ptrb (branch, 1);
      اگر (anodes_ == 0)
	    anodes_ = نیا علامت_map_s<vvp_island_branch>;
      اگر (bnodes_ == 0)
	    bnodes_ = نیا علامت_map_s<vvp_island_branch>;

      اگر ((cur = anodes_->sym_get_قدر(pa))) {
	    branch->link[0] = cur->link[0];
	    cur->link[0] = ptra;
      } ورنہ اگر ((cur = bnodes_->sym_get_قدر(pa))) {
	    branch->link[0] = cur->link[1];
	    cur->link[1] = ptra;
      } ورنہ {
	    branch->link[0] = ptra;
	    anodes_->sym_سیٹ_قدر(pa, branch);
      }

      اگر ((cur = anodes_->sym_get_قدر(pb))) {
	    branch->link[1] = cur->link[0];
	    cur->link[0] = ptrb;
      } ورنہ اگر ((cur = bnodes_->sym_get_قدر(pb))) {
	    branch->link[1] = cur->link[1];
	    cur->link[1] = ptrb;
      } ورنہ {
	    branch->link[1] = ptrb;
	    bnodes_->sym_سیٹ_قدر(pb, branch);
      }

      branch->اگلا_branch = branches_;
      branches_ = branch;
}

vvp_net_t* vvp_island::ڈھونڈو_پورٹ(ثابت حرف*کلید)
{
      اگر (ports_ == 0)
	    واپس 0;
      ورنہ
	    واپس ports_->sym_get_قدر(کلید);
}

خالی vvp_island::compile_cleanup()
{
      حذف ports_;
      ports_ = 0;

      حذف anodes_;
      anodes_ = 0;

      حذف bnodes_;
      bnodes_ = 0;
}

vvp_island_پورٹ::vvp_island_پورٹ(vvp_island*ip)
: island_(ip)
{
}

vvp_island_پورٹ::~vvp_island_پورٹ()
{
}

خالی vvp_island_پورٹ::recv_vec4(vvp_net_ptr_t, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                vvp_context_t)
{
      vvp_vector8_t tmp (ثنائی_ہندسہ, 6, 6);
      اگر (invalue .eeq(tmp))
	    واپس;

      invalue = tmp;
      island_->flag_island();
}

خالی vvp_island_پورٹ::recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				   غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t)
{
      vvp_vector8_t tmp(ثنائی_ہندسہ, 6, 6);
      recv_vec8_pv(پورٹ, tmp, بنیاد, vwid);
}


خالی vvp_island_پورٹ::recv_vec8(vvp_net_ptr_t, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      اگر (invalue .eeq(ثنائی_ہندسہ))
	    واپس;

      invalue = ثنائی_ہندسہ;
      island_->flag_island();
}

خالی vvp_island_پورٹ::recv_vec8_pv(vvp_net_ptr_t, ثابت vvp_vector8_t&ثنائی_ہندسہ,
                                   غیرعلامتی بنیاد, غیرعلامتی vwid)
{
      اگر (invalue.size() == 0) {
	    invalue = part_expand(ثنائی_ہندسہ, vwid, بنیاد);
      } ورنہ {
	    غیرعلامتی wid = ثنائی_ہندسہ.size();
	    أكد(invalue.size() == vwid);
	    برائے (غیرعلامتی idx = 0; idx < wid ; idx += 1) {
		  اگر ((بنیاد+idx) >= vwid)
			توڑو;
		  invalue.سیٹ_ثنائی_ہندسہ(بنیاد+idx, ثنائی_ہندسہ.value(idx));
	    }
      }

      island_->flag_island();
}

خالی vvp_island_پورٹ::force_flag(منطقی تسلسل_now)
{
      اگر (تسلسل_now)
	    island_->تسلسل_island();
      ورنہ
	    island_->flag_island();
}

vvp_island_branch::~vvp_island_branch()
{
}

خالی island_collect_گرہ(list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur)
{
      conn .push_back(cur);
      برائے (vvp_branch_ptr_t idx = اگلا_2(cur) ; idx != cur ; idx = اگلا_2(idx))
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
ساکن علامت_map_s<vvp_island>* island_جدول = 0;
ساکن vvp_island** island_list = 0;
ساکن غیرعلامتی island_گنتی = 0;

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی island_delete()
{
      at_EOS = سچ;
      برائے (غیرعلامتی idx = 0; idx < island_گنتی; idx += 1) {
	    حذف island_list[idx];
      }
      آزاد(island_list);
      island_list = 0;
      island_گنتی = 0;
}
#اگر_ختم

خالی compile_island_بنیاد(حرف*label, vvp_island*use_island)
{
      اگر (island_جدول == 0)
	    island_جدول = نیا علامت_map_s<vvp_island>;

      island_جدول->sym_سیٹ_قدر(label, use_island);
      island_گنتی += 1;
      island_list = static_cast<vvp_island **>(realloc(island_list,
                                               island_گنتی*حجم(vvp_island **)));
      island_list[island_گنتی-1] = use_island;
      آزاد(label);
}

vvp_island* compile_ڈھونڈو_island(ثابت حرف*island)
{
      أكد(island_جدول);
      vvp_island*use_island = island_جدول->sym_get_قدر(island);
      أكد(use_island);
      واپس use_island;
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
خالی compile_island_پورٹ(حرف*label, حرف*island, حرف*src)
{
      أكد(island_جدول);
      vvp_island*use_island = island_جدول->sym_get_قدر(island);
      أكد(use_island);
      آزاد(island);

      vvp_net_t*net = نیا vvp_net_t;
      vvp_island_پورٹ*fun = نیا vvp_island_پورٹ(use_island);
      net->fun = fun;

	// Get the source from outside the island
      داخلہ_connect(net, 0, src);

	// Define the functor outside the island.
      define_functor_علامت(label, net);

	// Also define it inside the island.
      use_island->جوڑو_پورٹ(label, net);

      آزاد(label);
}

خالی compile_island_export(حرف*label, حرف*island)
{
      fprintf(stderr, "XXXX %s .export %s;\n", label, island);
      آزاد(label);
      آزاد(island);
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
خالی compile_island_import(حرف*label, حرف*island, حرف*src)
{
      أكد(island_جدول);
      vvp_island*use_island = island_جدول->sym_get_قدر(island);
      أكد(use_island);
      آزاد(island);

      vvp_net_t*net = نیا vvp_net_t;
      vvp_island_پورٹ*fun = نیا vvp_island_پورٹ(use_island);
      net->fun = fun;

	// Get the source from outside the island
      داخلہ_connect(net, 0, src);

	// Define the functor only inside the island.
      use_island->جوڑو_پورٹ(label, net);

      آزاد(label);
}

خالی compile_island_cleanup(خالی)
{
	// Call the per-island cleanup to get rid of local symbol tables.
      برائے (غیرعلامتی idx = 0; idx < island_گنتی; idx += 1) {
	    island_list[idx]->compile_cleanup();
      }

	// If we are not doing valgrind checking then free the list.
#اگر_غیر_تعریف_شدہ CHECK_WITH_VALGRIND
      آزاد(island_list);
      island_list = 0;
      island_گنتی = 0;
#اگر_ختم

	// Remove the island symbol table itself.
      حذف island_جدول;
      island_جدول = 0;
}
