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
#अगर_घोषित CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#पूर्ण_अगर
# include  <iostream>
# include  <list>
# include  <cassert>
# include  <cstdlib>
# include  <cstring>
# include "ivl_alloc.h"

using namespace std;

#अगर_घोषित CHECK_WITH_VALGRIND
अटल bool at_EOS = false;
#पूर्ण_अगर

व्योम island_send_मूल्य(vvp_net_t*net, स्थिर vvp_vector8_t&val)
{
      vvp_island_port*fun = dynamic_cast<vvp_island_port*>(net->fun);
      अगर (fun->outvalue .eeq(val))
	    वापस;

      fun->outvalue = val;
      net->send_vec8(fun->outvalue);
}

/*
 * Implementations...
 */
vvp_island::vvp_island()
{
      flagged_ = false;
      branches_ = 0;
      ports_ = 0;
      anodes_ = 0;
      bnodes_ = 0;
}

vvp_island::~vvp_island()
{
	// We can only delete islands at the end of simulation.
#अगर_घोषित CHECK_WITH_VALGRIND
      अगर (!at_EOS) निश्चित(0);
#पूर्ण_अगर

      जबतक (branches_) {
	    vvp_island_branch *अग्र_br = branches_->अग्र_branch;
	    मिटाओ branches_;
	    branches_ = अग्र_br;
      }
}

व्योम vvp_island::flag_island()
{
      अगर (flagged_ == true)
	    वापस;

      schedule_generic(यह, 0, false, false);
      flagged_ = true;
}

/*
* This method handles the callback from the scheduler. It does basic
* housecleaning and calls the run_island() method implemented by the
* derived class.
*/
व्योम vvp_island::run_run()
{
      flagged_ = false;
      run_island();
}


व्योम vvp_island::add_port(स्थिर अक्षर*कुंजी, vvp_net_t*net)
{
      अगर (ports_ == 0)
	    ports_ = नया प्रतीक_map_s<vvp_net_t>;

	// each port should have a unique label
      निश्चित(ports_->sym_get_मूल्य(कुंजी) == 0);

      ports_->sym_set_मूल्य(कुंजी, net);
}

व्योम vvp_island::add_branch(vvp_island_branch*branch, स्थिर अक्षर*pa, स्थिर अक्षर*pb)
{
      vvp_island_branch*cur;
      निश्चित(ports_);
      branch->a = ports_->sym_get_मूल्य(pa);
      branch->b = ports_->sym_get_मूल्य(pb);
      निश्चित(branch->a && branch->b);

      vvp_branch_ptr_t ptra (branch, 0);
      vvp_branch_ptr_t ptrb (branch, 1);
      अगर (anodes_ == 0)
	    anodes_ = नया प्रतीक_map_s<vvp_island_branch>;
      अगर (bnodes_ == 0)
	    bnodes_ = नया प्रतीक_map_s<vvp_island_branch>;

      अगर ((cur = anodes_->sym_get_मूल्य(pa))) {
	    branch->link[0] = cur->link[0];
	    cur->link[0] = ptra;
      } अन्यथा अगर ((cur = bnodes_->sym_get_मूल्य(pa))) {
	    branch->link[0] = cur->link[1];
	    cur->link[1] = ptra;
      } अन्यथा {
	    branch->link[0] = ptra;
	    anodes_->sym_set_मूल्य(pa, branch);
      }

      अगर ((cur = anodes_->sym_get_मूल्य(pb))) {
	    branch->link[1] = cur->link[0];
	    cur->link[0] = ptrb;
      } अन्यथा अगर ((cur = bnodes_->sym_get_मूल्य(pb))) {
	    branch->link[1] = cur->link[1];
	    cur->link[1] = ptrb;
      } अन्यथा {
	    branch->link[1] = ptrb;
	    bnodes_->sym_set_मूल्य(pb, branch);
      }

      branch->अग्र_branch = branches_;
      branches_ = branch;
}

vvp_net_t* vvp_island::find_port(स्थिर अक्षर*कुंजी)
{
      अगर (ports_ == 0)
	    वापस 0;
      अन्यथा
	    वापस ports_->sym_get_मूल्य(कुंजी);
}

व्योम vvp_island::compile_cleanup()
{
      मिटाओ ports_;
      ports_ = 0;

      मिटाओ anodes_;
      anodes_ = 0;

      मिटाओ bnodes_;
      bnodes_ = 0;
}

vvp_island_port::vvp_island_port(vvp_island*ip)
: island_(ip)
{
}

vvp_island_port::~vvp_island_port()
{
}

व्योम vvp_island_port::recv_vec4(vvp_net_ptr_t, स्थिर vvp_vector4_t&bit,
                                vvp_context_t)
{
      vvp_vector8_t tmp (bit, 6, 6);
      अगर (invalue .eeq(tmp))
	    वापस;

      invalue = tmp;
      island_->flag_island();
}

व्योम vvp_island_port::recv_vec4_pv(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
				   अचिन्हित base, अचिन्हित vwid, vvp_context_t)
{
      vvp_vector8_t tmp(bit, 6, 6);
      recv_vec8_pv(port, tmp, base, vwid);
}


व्योम vvp_island_port::recv_vec8(vvp_net_ptr_t, स्थिर vvp_vector8_t&bit)
{
      अगर (invalue .eeq(bit))
	    वापस;

      invalue = bit;
      island_->flag_island();
}

व्योम vvp_island_port::recv_vec8_pv(vvp_net_ptr_t, स्थिर vvp_vector8_t&bit,
                                   अचिन्हित base, अचिन्हित vwid)
{
      अगर (invalue.size() == 0) {
	    invalue = part_expand(bit, vwid, base);
      } अन्यथा {
	    अचिन्हित wid = bit.size();
	    निश्चित(invalue.size() == vwid);
	    क्रम (अचिन्हित idx = 0; idx < wid ; idx += 1) {
		  अगर ((base+idx) >= vwid)
			अवरोध;
		  invalue.set_bit(base+idx, bit.value(idx));
	    }
      }

      island_->flag_island();
}

व्योम vvp_island_port::force_flag(bool run_now)
{
      अगर (run_now)
	    island_->run_island();
      अन्यथा
	    island_->flag_island();
}

vvp_island_branch::~vvp_island_branch()
{
}

व्योम island_collect_गाँठ(list<vvp_branch_ptr_t>&conn, vvp_branch_ptr_t cur)
{
      conn .push_back(cur);
      क्रम (vvp_branch_ptr_t idx = अग्र_2(cur) ; idx != cur ; idx = अग्र_2(idx))
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
अटल प्रतीक_map_s<vvp_island>* island_table = 0;
अटल vvp_island** island_list = 0;
अटल अचिन्हित island_गिनती = 0;

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम island_delete()
{
      at_EOS = true;
      क्रम (अचिन्हित idx = 0; idx < island_गिनती; idx += 1) {
	    मिटाओ island_list[idx];
      }
      मुक्त(island_list);
      island_list = 0;
      island_गिनती = 0;
}
#पूर्ण_अगर

व्योम compile_island_base(अक्षर*label, vvp_island*use_island)
{
      अगर (island_table == 0)
	    island_table = नया प्रतीक_map_s<vvp_island>;

      island_table->sym_set_मूल्य(label, use_island);
      island_गिनती += 1;
      island_list = static_cast<vvp_island **>(पुनः_स्मृति(island_list,
                                               island_गिनती*माप(vvp_island **)));
      island_list[island_गिनती-1] = use_island;
      मुक्त(label);
}

vvp_island* compile_find_island(स्थिर अक्षर*island)
{
      निश्चित(island_table);
      vvp_island*use_island = island_table->sym_get_मूल्य(island);
      निश्चित(use_island);
      वापस use_island;
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
व्योम compile_island_port(अक्षर*label, अक्षर*island, अक्षर*src)
{
      निश्चित(island_table);
      vvp_island*use_island = island_table->sym_get_मूल्य(island);
      निश्चित(use_island);
      मुक्त(island);

      vvp_net_t*net = नया vvp_net_t;
      vvp_island_port*fun = नया vvp_island_port(use_island);
      net->fun = fun;

	// Get the source from outside the island
      निवेश_connect(net, 0, src);

	// Define the functor outside the island.
      define_functor_प्रतीक(label, net);

	// Also define it inside the island.
      use_island->add_port(label, net);

      मुक्त(label);
}

व्योम compile_island_export(अक्षर*label, अक्षर*island)
{
      ख_लिखो(मानक_त्रुटि, "XXXX %s .export %s;\n", label, island);
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
व्योम compile_island_import(अक्षर*label, अक्षर*island, अक्षर*src)
{
      निश्चित(island_table);
      vvp_island*use_island = island_table->sym_get_मूल्य(island);
      निश्चित(use_island);
      मुक्त(island);

      vvp_net_t*net = नया vvp_net_t;
      vvp_island_port*fun = नया vvp_island_port(use_island);
      net->fun = fun;

	// Get the source from outside the island
      निवेश_connect(net, 0, src);

	// Define the functor only inside the island.
      use_island->add_port(label, net);

      मुक्त(label);
}

व्योम compile_island_cleanup(व्योम)
{
	// Call the per-island cleanup to get rid of local symbol tables.
      क्रम (अचिन्हित idx = 0; idx < island_गिनती; idx += 1) {
	    island_list[idx]->compile_cleanup();
      }

	// If we are not doing valgrind checking then free the list.
#अगर_अघोषित CHECK_WITH_VALGRIND
      मुक्त(island_list);
      island_list = 0;
      island_गिनती = 0;
#पूर्ण_अगर

	// Remove the island symbol table itself.
      मिटाओ island_table;
      island_table = 0;
}
