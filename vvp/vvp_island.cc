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

using नामाकाश std;

#ifdef CHECK_WITH_VALGRIND
स्थिर तार्किक at_EOS = असत्य;
#endif

शून्य island_send_मूल्य(vvp_net_t*net, ध्रुव vvp_vector8_t&val)
{
      vvp_island_port*fun = dynamic_cast<vvp_island_port*>(net->fun);
      यदि (fun->outvalue .eeq(val))
	    प्रत्यागम;

      fun->outvalue = val;
      net->send_vec8(fun->outvalue);
}

/*
 * Implementations...
 */
vvp_island::vvp_island()
{
      flagged_ = असत्य;
      branches_ = 0;
      ports_ = 0;
      anodes_ = 0;
      bnodes_ = 0;
}

vvp_island::~vvp_island()
{
	// We can only delete islands at the end of simulation.
#ifdef CHECK_WITH_VALGRIND
      यदि (!at_EOS) प्रतिज्ञा(0);
#endif

      यावत् (branches_) {
	    vvp_island_branch *next_br = branches_->next_branch;
	    delete branches_;
	    branches_ = next_br;
      }
}

शून्य vvp_island::flag_island()
{
      यदि (flagged_ == सत्य)
	    प्रत्यागम;

      schedule_generic(एतत्, 0, असत्य, असत्य);
      flagged_ = सत्य;
}

/*
* This method handles the callback from the scheduler. It does basic
* housecleaning and calls the run_island() method implemented by the
* derived class.
*/
शून्य vvp_island::run_run()
{
      flagged_ = असत्य;
      run_island();
}


शून्य vvp_island::add_port(ध्रुव वर्ण*किल्ली, vvp_net_t*net)
{
      यदि (ports_ == 0)
	    ports_ = नूतन चिन्ह_map_s<vvp_net_t>;

	// each port should have a unique label
      प्रतिज्ञा(ports_->sym_get_मूल्य(किल्ली) == 0);

      ports_->sym_set_मूल्य(किल्ली, net);
}

शून्य vvp_island::add_branch(vvp_island_branch*branch, ध्रुव वर्ण*pa, ध्रुव वर्ण*pb)
{
      vvp_island_branch*cur;
      प्रतिज्ञा(ports_);
      branch->a = ports_->sym_get_मूल्य(pa);
      branch->b = ports_->sym_get_मूल्य(pb);
      प्रतिज्ञा(branch->a && branch->b);

      vvp_branch_ptr_t ptra (branch, 0);
      vvp_branch_ptr_t ptrb (branch, 1);
      यदि (anodes_ == 0)
	    anodes_ = नूतन चिन्ह_map_s<vvp_island_branch>;
      यदि (bnodes_ == 0)
	    bnodes_ = नूतन चिन्ह_map_s<vvp_island_branch>;

      यदि ((cur = anodes_->sym_get_मूल्य(pa))) {
	    branch->link[0] = cur->link[0];
	    cur->link[0] = ptra;
      } अन्यथा यदि ((cur = bnodes_->sym_get_मूल्य(pa))) {
	    branch->link[0] = cur->link[1];
	    cur->link[1] = ptra;
      } अन्यथा {
	    branch->link[0] = ptra;
	    anodes_->sym_set_मूल्य(pa, branch);
      }

      यदि ((cur = anodes_->sym_get_मूल्य(pb))) {
	    branch->link[1] = cur->link[0];
	    cur->link[0] = ptrb;
      } अन्यथा यदि ((cur = bnodes_->sym_get_मूल्य(pb))) {
	    branch->link[1] = cur->link[1];
	    cur->link[1] = ptrb;
      } अन्यथा {
	    branch->link[1] = ptrb;
	    bnodes_->sym_set_मूल्य(pb, branch);
      }

      branch->next_branch = branches_;
      branches_ = branch;
}

vvp_net_t* vvp_island::find_port(ध्रुव वर्ण*किल्ली)
{
      यदि (ports_ == 0)
	    प्रत्यागम 0;
      अन्यथा
	    प्रत्यागम ports_->sym_get_मूल्य(किल्ली);
}

शून्य vvp_island::compile_cleanup()
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

शून्य vvp_island_port::recv_vec4(vvp_net_ptr_t, ध्रुव vvp_vector4_t&bit,
                                vvp_context_t)
{
      vvp_vector8_t tmp (bit, 6, 6);
      यदि (invalue .eeq(tmp))
	    प्रत्यागम;

      invalue = tmp;
      island_->flag_island();
}

शून्य vvp_island_port::recv_vec4_pv(vvp_net_ptr_t port, ध्रुव vvp_vector4_t&bit,
				   unsigned base, unsigned vwid, vvp_context_t)
{
      vvp_vector8_t tmp(bit, 6, 6);
      recv_vec8_pv(port, tmp, base, vwid);
}


शून्य vvp_island_port::recv_vec8(vvp_net_ptr_t, ध्रुव vvp_vector8_t&bit)
{
      यदि (invalue .eeq(bit))
	    प्रत्यागम;

      invalue = bit;
      island_->flag_island();
}

शून्य vvp_island_port::recv_vec8_pv(vvp_net_ptr_t, ध्रुव vvp_vector8_t&bit,
                                   unsigned base, unsigned vwid)
{
      यदि (invalue.size() == 0) {
	    invalue = part_expand(bit, vwid, base);
      } अन्यथा {
	    unsigned wid = bit.size();
	    प्रतिज्ञा(invalue.size() == vwid);
	    प्रति (unsigned idx = 0; idx < wid ; idx += 1) {
		  यदि ((base+idx) >= vwid)
			भङ्ग;
		  invalue.set_bit(base+idx, bit.value(idx));
	    }
      }

      island_->flag_island();
}

शून्य vvp_island_port::force_flag(तार्किक run_now)
{
      यदि (run_now)
	    island_->run_island();
      अन्यथा
	    island_->flag_island();
}

vvp_island_branch::~vvp_island_branch()
{
}

शून्य island_collect_गाठ(list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur)
{
      conn .push_back(cur);
      प्रति (vvp_branch_ptr_t idx = next(cur) ; idx != cur ; idx = next(idx))
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
स्थिर चिन्ह_map_s<vvp_island>* island_table = 0;
स्थिर vvp_island** island_list = 0;
स्थिर unsigned island_मोजणी = 0;

#ifdef CHECK_WITH_VALGRIND
शून्य island_delete()
{
      at_EOS = सत्य;
      प्रति (unsigned idx = 0; idx < island_मोजणी; idx += 1) {
	    delete island_list[idx];
      }
      मुक्त(island_list);
      island_list = 0;
      island_मोजणी = 0;
}
#endif

शून्य compile_island_base(वर्ण*label, vvp_island*use_island)
{
      यदि (island_table == 0)
	    island_table = नूतन चिन्ह_map_s<vvp_island>;

      island_table->sym_set_मूल्य(label, use_island);
      island_मोजणी += 1;
      island_list = static_cast<vvp_island **>(realloc(island_list,
                                               island_मोजणी*परिमाण(vvp_island **)));
      island_list[island_मोजणी-1] = use_island;
      मुक्त(label);
}

vvp_island* compile_find_island(ध्रुव वर्ण*island)
{
      प्रतिज्ञा(island_table);
      vvp_island*use_island = island_table->sym_get_मूल्य(island);
      प्रतिज्ञा(use_island);
      प्रत्यागम use_island;
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
शून्य compile_island_port(वर्ण*label, वर्ण*island, वर्ण*src)
{
      प्रतिज्ञा(island_table);
      vvp_island*use_island = island_table->sym_get_मूल्य(island);
      प्रतिज्ञा(use_island);
      मुक्त(island);

      vvp_net_t*net = नूतन vvp_net_t;
      vvp_island_port*fun = नूतन vvp_island_port(use_island);
      net->fun = fun;

	// Get the source from outside the island
      आदान_connect(net, 0, src);

	// Define the functor outside the island.
      define_functor_चिन्ह(label, net);

	// Also define it inside the island.
      use_island->add_port(label, net);

      मुक्त(label);
}

शून्य compile_island_export(वर्ण*label, वर्ण*island)
{
      fprintf(stderr, "XXXX %s .export %s;\n", label, island);
      मुक्त(label);
      मुक्त(island);
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
शून्य compile_island_import(वर्ण*label, वर्ण*island, वर्ण*src)
{
      प्रतिज्ञा(island_table);
      vvp_island*use_island = island_table->sym_get_मूल्य(island);
      प्रतिज्ञा(use_island);
      मुक्त(island);

      vvp_net_t*net = नूतन vvp_net_t;
      vvp_island_port*fun = नूतन vvp_island_port(use_island);
      net->fun = fun;

	// Get the source from outside the island
      आदान_connect(net, 0, src);

	// Define the functor only inside the island.
      use_island->add_port(label, net);

      मुक्त(label);
}

शून्य compile_island_cleanup(शून्य)
{
	// Call the per-island cleanup to get rid of local symbol tables.
      प्रति (unsigned idx = 0; idx < island_मोजणी; idx += 1) {
	    island_list[idx]->compile_cleanup();
      }

	// If we are not doing valgrind checking then free the list.
#ifndef CHECK_WITH_VALGRIND
      मुक्त(island_list);
      island_list = 0;
      island_मोजणी = 0;
#endif

	// Remove the island symbol table itself.
      delete island_table;
      island_table = 0;
}
