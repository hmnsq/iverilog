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
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#అయితే_ముగింపు
# include  <iostream>
# include  <list>
# include  <cassert>
# include  <cstdlib>
# include  <cstring>
# include "ivl_alloc.h"

ఉపయోగించి నామాకాశ std;

#నిర్వచితమైతే CHECK_WITH_VALGRIND
స్థిర తార్కిక at_EOS = అబద్ధం;
#అయితే_ముగింపు

శూన్య island_send_విలువ(vvp_net_t*net, స్థిరరాశి vvp_vector8_t&val)
{
      vvp_island_పోర్టు*fun = dynamic_cast<vvp_island_పోర్టు*>(net->fun);
      ఒకవేళ (fun->outvalue .eeq(val))
	    తిరిగి;

      fun->outvalue = val;
      net->send_vec8(fun->outvalue);
}

/*
 * Implementations...
 */
vvp_island::vvp_island()
{
      flagged_ = అబద్ధం;
      branches_ = 0;
      ports_ = 0;
      anodes_ = 0;
      bnodes_ = 0;
}

vvp_island::~vvp_island()
{
	// We can only delete islands at the end of simulation.
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      ఒకవేళ (!at_EOS) ప్రతిజ్ఞా(0);
#అయితే_ముగింపు

      అయితే (branches_) {
	    vvp_island_branch *తదుపరి_br = branches_->తదుపరి_branch;
	    తొలగించు branches_;
	    branches_ = తదుపరి_br;
      }
}

శూన్య vvp_island::flag_island()
{
      ఒకవేళ (flagged_ == నిజం)
	    తిరిగి;

      కాలపట్టిక_generic(ఇది, 0, అబద్ధం, అబద్ధం);
      flagged_ = నిజం;
}

/*
* This method handles the callback from the scheduler. It does basic
* housecleaning and calls the run_island() method implemented by the
* derived class.
*/
శూన్య vvp_island::నిరంతరం_నిరంతరం()
{
      flagged_ = అబద్ధం;
      నిరంతరం_island();
}


శూన్య vvp_island::కూడు_పోర్టు(స్థిరరాశి అక్షరం*కీలకం, vvp_net_t*net)
{
      ఒకవేళ (ports_ == 0)
	    ports_ = కొత్త చిహ్నం_map_s<vvp_net_t>;

	// each port should have a unique label
      ప్రతిజ్ఞా(ports_->sym_get_విలువ(కీలకం) == 0);

      ports_->sym_సమితి_విలువ(కీలకం, net);
}

శూన్య vvp_island::కూడు_branch(vvp_island_branch*branch, స్థిరరాశి అక్షరం*pa, స్థిరరాశి అక్షరం*pb)
{
      vvp_island_branch*cur;
      ప్రతిజ్ఞా(ports_);
      branch->a = ports_->sym_get_విలువ(pa);
      branch->b = ports_->sym_get_విలువ(pb);
      ప్రతిజ్ఞా(branch->a && branch->b);

      vvp_branch_ptr_t ptra (branch, 0);
      vvp_branch_ptr_t ptrb (branch, 1);
      ఒకవేళ (anodes_ == 0)
	    anodes_ = కొత్త చిహ్నం_map_s<vvp_island_branch>;
      ఒకవేళ (bnodes_ == 0)
	    bnodes_ = కొత్త చిహ్నం_map_s<vvp_island_branch>;

      ఒకవేళ ((cur = anodes_->sym_get_విలువ(pa))) {
	    branch->link[0] = cur->link[0];
	    cur->link[0] = ptra;
      } లేకపోతే ఒకవేళ ((cur = bnodes_->sym_get_విలువ(pa))) {
	    branch->link[0] = cur->link[1];
	    cur->link[1] = ptra;
      } లేకపోతే {
	    branch->link[0] = ptra;
	    anodes_->sym_సమితి_విలువ(pa, branch);
      }

      ఒకవేళ ((cur = anodes_->sym_get_విలువ(pb))) {
	    branch->link[1] = cur->link[0];
	    cur->link[0] = ptrb;
      } లేకపోతే ఒకవేళ ((cur = bnodes_->sym_get_విలువ(pb))) {
	    branch->link[1] = cur->link[1];
	    cur->link[1] = ptrb;
      } లేకపోతే {
	    branch->link[1] = ptrb;
	    bnodes_->sym_సమితి_విలువ(pb, branch);
      }

      branch->తదుపరి_branch = branches_;
      branches_ = branch;
}

vvp_net_t* vvp_island::కనుగొను_పోర్టు(స్థిరరాశి అక్షరం*కీలకం)
{
      ఒకవేళ (ports_ == 0)
	    తిరిగి 0;
      లేకపోతే
	    తిరిగి ports_->sym_get_విలువ(కీలకం);
}

శూన్య vvp_island::compile_cleanup()
{
      తొలగించు ports_;
      ports_ = 0;

      తొలగించు anodes_;
      anodes_ = 0;

      తొలగించు bnodes_;
      bnodes_ = 0;
}

vvp_island_పోర్టు::vvp_island_పోర్టు(vvp_island*ip)
: island_(ip)
{
}

vvp_island_పోర్టు::~vvp_island_పోర్టు()
{
}

శూన్య vvp_island_పోర్టు::recv_vec4(vvp_net_ptr_t, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                vvp_context_t)
{
      vvp_vector8_t tmp (బిట్టు, 6, 6);
      ఒకవేళ (invalue .eeq(tmp))
	    తిరిగి;

      invalue = tmp;
      island_->flag_island();
}

శూన్య vvp_island_పోర్టు::recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
				   గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t)
{
      vvp_vector8_t tmp(బిట్టు, 6, 6);
      recv_vec8_pv(పోర్టు, tmp, పునాది, vwid);
}


శూన్య vvp_island_పోర్టు::recv_vec8(vvp_net_ptr_t, స్థిరరాశి vvp_vector8_t&బిట్టు)
{
      ఒకవేళ (invalue .eeq(బిట్టు))
	    తిరిగి;

      invalue = బిట్టు;
      island_->flag_island();
}

శూన్య vvp_island_పోర్టు::recv_vec8_pv(vvp_net_ptr_t, స్థిరరాశి vvp_vector8_t&బిట్టు,
                                   గుర్తులేని పునాది, గుర్తులేని vwid)
{
      ఒకవేళ (invalue.size() == 0) {
	    invalue = part_expand(బిట్టు, vwid, పునాది);
      } లేకపోతే {
	    గుర్తులేని wid = బిట్టు.size();
	    ప్రతిజ్ఞా(invalue.size() == vwid);
	    కొరకు (గుర్తులేని idx = 0; idx < wid ; idx += 1) {
		  ఒకవేళ ((పునాది+idx) >= vwid)
			ఆపు;
		  invalue.సమితి_బిట్టు(పునాది+idx, బిట్టు.value(idx));
	    }
      }

      island_->flag_island();
}

శూన్య vvp_island_పోర్టు::force_flag(తార్కిక నిరంతరం_now)
{
      ఒకవేళ (నిరంతరం_now)
	    island_->నిరంతరం_island();
      లేకపోతే
	    island_->flag_island();
}

vvp_island_branch::~vvp_island_branch()
{
}

శూన్య island_collect_కణుపు(list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur)
{
      conn .push_back(cur);
      కొరకు (vvp_branch_ptr_t idx = తదుపరి_2(cur) ; idx != cur ; idx = తదుపరి_2(idx))
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
స్థిర చిహ్నం_map_s<vvp_island>* island_పట్టిక = 0;
స్థిర vvp_island** island_list = 0;
స్థిర గుర్తులేని island_లెక్క = 0;

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య island_delete()
{
      at_EOS = నిజం;
      కొరకు (గుర్తులేని idx = 0; idx < island_లెక్క; idx += 1) {
	    తొలగించు island_list[idx];
      }
      విడుదల(island_list);
      island_list = 0;
      island_లెక్క = 0;
}
#అయితే_ముగింపు

శూన్య compile_island_పునాది(అక్షరం*label, vvp_island*use_island)
{
      ఒకవేళ (island_పట్టిక == 0)
	    island_పట్టిక = కొత్త చిహ్నం_map_s<vvp_island>;

      island_పట్టిక->sym_సమితి_విలువ(label, use_island);
      island_లెక్క += 1;
      island_list = static_cast<vvp_island **>(realloc(island_list,
                                               island_లెక్క*పరిమాణం(vvp_island **)));
      island_list[island_లెక్క-1] = use_island;
      విడుదల(label);
}

vvp_island* compile_కనుగొను_island(స్థిరరాశి అక్షరం*island)
{
      ప్రతిజ్ఞా(island_పట్టిక);
      vvp_island*use_island = island_పట్టిక->sym_get_విలువ(island);
      ప్రతిజ్ఞా(use_island);
      తిరిగి use_island;
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
శూన్య compile_island_పోర్టు(అక్షరం*label, అక్షరం*island, అక్షరం*src)
{
      ప్రతిజ్ఞా(island_పట్టిక);
      vvp_island*use_island = island_పట్టిక->sym_get_విలువ(island);
      ప్రతిజ్ఞా(use_island);
      విడుదల(island);

      vvp_net_t*net = కొత్త vvp_net_t;
      vvp_island_పోర్టు*fun = కొత్త vvp_island_పోర్టు(use_island);
      net->fun = fun;

	// Get the source from outside the island
      ఆదానం_connect(net, 0, src);

	// Define the functor outside the island.
      define_functor_చిహ్నం(label, net);

	// Also define it inside the island.
      use_island->కూడు_పోర్టు(label, net);

      విడుదల(label);
}

శూన్య compile_island_export(అక్షరం*label, అక్షరం*island)
{
      fprintf(stderr, "XXXX %s .export %s;\n", label, island);
      విడుదల(label);
      విడుదల(island);
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
శూన్య compile_island_import(అక్షరం*label, అక్షరం*island, అక్షరం*src)
{
      ప్రతిజ్ఞా(island_పట్టిక);
      vvp_island*use_island = island_పట్టిక->sym_get_విలువ(island);
      ప్రతిజ్ఞా(use_island);
      విడుదల(island);

      vvp_net_t*net = కొత్త vvp_net_t;
      vvp_island_పోర్టు*fun = కొత్త vvp_island_పోర్టు(use_island);
      net->fun = fun;

	// Get the source from outside the island
      ఆదానం_connect(net, 0, src);

	// Define the functor only inside the island.
      use_island->కూడు_పోర్టు(label, net);

      విడుదల(label);
}

శూన్య compile_island_cleanup(శూన్య)
{
	// Call the per-island cleanup to get rid of local symbol tables.
      కొరకు (గుర్తులేని idx = 0; idx < island_లెక్క; idx += 1) {
	    island_list[idx]->compile_cleanup();
      }

	// If we are not doing valgrind checking then free the list.
#నిర్వచితం_కాకపోతే CHECK_WITH_VALGRIND
      విడుదల(island_list);
      island_list = 0;
      island_లెక్క = 0;
#అయితే_ముగింపు

	// Remove the island symbol table itself.
      తొలగించు island_పట్టిక;
      island_పట్టిక = 0;
}
