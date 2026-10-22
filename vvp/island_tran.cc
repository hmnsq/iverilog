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
# include  <list>

# include  <iostream>

using namespace std;

श्रेणी vvp_island_tran : खुला vvp_island {

    खुला:
      व्योम run_island() override;
      व्योम गिनती_drivers(vvp_island_port*port, अचिन्हित bit_idx,
                         अचिन्हित counts[3]) override;
};

क्रमागत tran_state_t {
      tran_disabled,
      tran_enabled,
      tran_unknown
};

काष्ठा vvp_island_branch_tran : खुला vvp_island_branch {

      vvp_island_branch_tran(vvp_net_t*en__, bool active_ऊपरी,
                             अचिन्हित width__, अचिन्हित part__,
                             अचिन्हित offset__, bool resistive__);
      व्योम run_test_enabled();
      bool rerun_test_enabled();
      व्योम run_resolution();
      व्योम run_output();

      vvp_net_t*en;
      अचिन्हित width, part, offset;
      bool active_ऊपरी_2, resistive;
      tran_state_t state;
};

vvp_island_branch_tran::vvp_island_branch_tran(vvp_net_t*en__,
                                               bool active_ऊपरी,
                                               अचिन्हित width__,
                                               अचिन्हित part__,
                                               अचिन्हित offset__,
                                               bool resistive__)
: en(en__), width(width__), part(part__), offset(offset__),
  active_ऊपरी_2(active_ऊपरी), resistive(resistive__)
{
      state = en__ ? tran_disabled : tran_enabled;
}

अटल अंतरभूत vvp_island_branch_tran* BRANCH_TRAN(vvp_island_branch*tmp)
{
      vvp_island_branch_tran*res = dynamic_cast<vvp_island_branch_tran*>(tmp);
      निश्चित(res);
      वापस res;
}

/*
 * The run_island() method is called by the scheduler to run the
 * entire island. We run the island by calling run_resolution() for
 * all the branches in the island.
*/
व्योम vvp_island_tran::run_island()
{
	// Test to see if any of the branches are enabled. This loop
	// tests the enabled inputs for all the branches and caches
	// the results in the state for each branch.
      क्रम (vvp_island_branch*cur = branches_ ; cur ; cur = cur->अग्र_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    निश्चित(tmp);
	    tmp->run_test_enabled();
      }

rerun:
	// Now resolve all the branches in the island.
      क्रम (vvp_island_branch*cur = branches_ ; cur ; cur = cur->अग्र_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    निश्चित(tmp);
	    tmp->run_resolution();
      }

	// Now output the resolved values.
      क्रम (vvp_island_branch*cur = branches_ ; cur ; cur = cur->अग्र_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    निश्चित(tmp);
	    tmp->run_output();
      }

	// Now check if the enable inputs have been affected by the resolution.
      bool enable_changed = false;
      क्रम (vvp_island_branch*cur = branches_ ; cur ; cur = cur->अग्र_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    निश्चित(tmp);
	    enable_changed |= tmp->rerun_test_enabled();
      }
      अगर (enable_changed) जाओ rerun;
}

अटल व्योम गिनती_drivers_2(vvp_branch_ptr_t cur, bool other_side_visited,
                           अचिन्हित bit_idx, अचिन्हित counts[3])
{
        // First count any value driven into the port associated with
        // the current endpoint.
      vvp_net_t*net = cur.port() ? cur.ptr()->b : cur.ptr()->a;
      vvp_scalar_t bit = island_get_मूल्य(net).value(bit_idx);
      update_driver_counts(bit.value(), counts);

        // Now handle all the branches attached to that port.
      vvp_branch_ptr_t idx = cur;
      करो {
            vvp_island_branch_tran*tmp = BRANCH_TRAN(idx.ptr());

              // If this branch represents a tran gate, we count the
              // value on the other side of the tran (providing it is
              // enabled) as a single driver.
            अगर (tmp->width == 0) {
                  अगर (tmp->state == tran_enabled) {
                        net = idx.port() ? idx.ptr()->a : idx.ptr()->b;
                        bit = island_get_sent_मूल्य(net).value(bit_idx);
                        update_driver_counts(bit.value(), counts);
                  }
                  जारी;
            }

              // If we get here, this branch is a part select. If we've
              // just come from the other end of the branch, we're done.
            अगर ((idx == cur) && other_side_visited)
                  जारी;

              // If this is the narrow end of the part select, the other
              // end must include the bit we are interested in. Follow
              // the branch to collect any drivers on the other side.
            अगर (idx.port() == 1) {
                  vvp_branch_ptr_t a_side(tmp, 0);
                  गिनती_drivers_2(a_side, true, tmp->offset + bit_idx, counts);
                  जारी;
            }

              // If we get here, this branch is the wide end of a part
              // select. If the bit we are interested in is within the
              // selected part, follow the branch to collect any drivers
              // on the other side.
            अगर ((bit_idx >= tmp->offset) && (bit_idx < tmp->offset+tmp->part)) {
                  vvp_branch_ptr_t b_side(tmp, 1);
                  गिनती_drivers_2(b_side, true, bit_idx - tmp->offset, counts);
                  जारी;
            }
      } जबतक ((idx = अग्र_2(idx)) != cur);
}

व्योम vvp_island_tran::गिनती_drivers(vvp_island_port*port, अचिन्हित bit_idx,
                                    अचिन्हित counts[3])
{
        // First we need to find a branch that is attached to the specified
        // port. Unfortunately there's no quick way to do this.
      vvp_island_branch*branch = branches_;
      अचिन्हित side = 0;
      जबतक (branch) {
            अगर (branch->a->fun == port) {
                  side = 0;
                  अवरोध;
            }
            अगर (branch->b->fun == port) {
                  side = 1;
                  अवरोध;
            }
            branch = branch->अग्र_branch;
      }
      निश्चित(branch);

        // Now count the drivers, pushing through the network as necessary.
      vvp_branch_ptr_t endpoint(branch, side);
      गिनती_drivers_2(endpoint, false, bit_idx, counts);
}

व्योम vvp_island_branch_tran::run_test_enabled()
{
      स्थिर vvp_island_port*ep = en? dynamic_cast<vvp_island_port*> (en->fun) : शून्य;

	// If there is no ep port (no "enabled" input) then this is a
	// tran branch. Assume it is always enabled.
      अगर (ep == 0) {
	    state = tran_enabled;
	    वापस;
      }

	// Get the input that is driving this enable.
	// SPECIAL NOTE: Try to get the input value from the
	// *outvalue* of the port. If the enable is connected to a
	// .port (instead of a .import) then there may be feedback
	// going on, and we need to be looking at the resolved input,
	// not the event input. For example:
	//
	//   tranif1 (pin, X, pin);
	//
	// In this case, when we test the value for "pin", we need to
	// look at the value that is resolved from this
	// island. Reading the outvalue will do the trick.
	//
	// If the outvalue is nil, then we know that this port is a
	// .import after all, so just read the invalue.
      vvp_bit4_t enable_val;
      अगर (ep->outvalue.size() != 0)
	    enable_val = ep->outvalue.value(0).value();
      अन्यथा अगर (ep->invalue.size() == 0)
	    enable_val = BIT4_Z;
      अन्यथा
	    enable_val = ep->invalue.value(0).value();

      चयन (enable_val) {
	  हाल BIT4_0:
	    state = active_ऊपरी_2 ? tran_disabled : tran_enabled;
	    अवरोध;
	  हाल BIT4_1:
	    state = active_ऊपरी_2 ? tran_enabled : tran_disabled;
	    अवरोध;
	  शेष:
	    state = tran_unknown;
	    अवरोध;
      }
}

bool vvp_island_branch_tran::rerun_test_enabled()
{
      स्थिर vvp_island_port*ep = en? dynamic_cast<vvp_island_port*> (en->fun) : शून्य;

      अगर (ep == 0)
	    वापस false;

	// We are only looking for changes resulting from running the island
	// resolution. If the outvalue is nil, we know that the enable port
	// is an .import, so won't be affected.
      अगर (ep->outvalue.size() == 0)
	    वापस false;

      vvp_bit4_t enable_val = ep->outvalue.value(0).value();

      tran_state_t old_state = state;

      चयन (enable_val) {
	  हाल BIT4_0:
	    state = active_ऊपरी_2 ? tran_disabled : tran_enabled;
	    अवरोध;
	  हाल BIT4_1:
	    state = active_ऊपरी_2 ? tran_enabled : tran_disabled;
	    अवरोध;
	  शेष:
	    state = tran_unknown;
	    अवरोध;
      }

      वापस state != old_state;
}

// The IEEE standard does not specify the behaviour when a tranif control
// input is 'x' or 'z'. We use the rules that are given for MOS switches.
अंतरभूत vvp_vector8_t resolve_ambiguous(स्थिर vvp_vector8_t&a,
                                       स्थिर vvp_vector8_t&b,
                                       tran_state_t state,
                                       स्थिर अचिन्हित str_map[8])
{
      निश्चित(a.size() == b.size());
      vvp_vector8_t out (a.size());

      क्रम (अचिन्हित idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_scalar_t a_bit = a.value(idx);
	    vvp_scalar_t b_bit = b.value(idx);
            b_bit = vvp_scalar_t(b_bit.value(),
                                 str_map[b_bit.strength0()],
                                 str_map[b_bit.strength1()]);
	    अगर (state == tran_unknown) {
		  चयन (b_bit.value()) {
		      हाल BIT4_0:
			b_bit = vvp_scalar_t(BIT4_X, b_bit.strength0(), 0);
			अवरोध;
		      हाल BIT4_1:
			b_bit = vvp_scalar_t(BIT4_X, 0, b_bit.strength1());
			अवरोध;
		      शेष:
			अवरोध;
		  }
	    }
	    out.set_bit(idx, resolve(a_bit, b_bit));
      }
      वापस out;
}

अटल व्योम धकेलो_मूल्य_through_branches(स्थिर vvp_vector8_t&val,
					स्थिर list<vvp_branch_ptr_t>&connections);

अटल व्योम धकेलो_मूल्य_through_branch(स्थिर vvp_vector8_t&val,
                                      vvp_branch_ptr_t cur)
{
      vvp_island_branch_tran*branch = BRANCH_TRAN(cur.ptr());

        // If the branch is disabled, skip.
      अगर (branch->state == tran_disabled)
            वापस;

      अचिन्हित src_ab = cur.port();
      अचिन्हित dst_ab = src_ab^1;

      vvp_net_t*dst_net = dst_ab? branch->b : branch->a;
      vvp_island_port*dst_port = dynamic_cast<vvp_island_port*>(dst_net->fun);

      vvp_vector8_t old_val = dst_port->value;

        // If the port on the other side has not yet been visited,
        // get its input value.
      अगर (dst_port->value.size() == 0)
            dst_port->value = island_get_मूल्य(dst_net);

        // If we don't yet have an initial value for the port, skip.
      अगर (dst_port->value.size() == 0)
            वापस;

        // Now resolve the pushed value with whatever values we have
        // previously collected (and resolved) for the port.
      अगर (branch->width == 0) {
              // There are no part selects.
            dst_port->value = resolve_ambiguous(dst_port->value, val, branch->state,
                                                vvp_switch_strength_map[branch->resistive]);

      } अन्यथा अगर (dst_ab == 1) {
              // The other side is a strict subset (part select)
              // of this side.
            vvp_vector8_t tmp = val.subvalue(branch->offset, branch->part);
            dst_port->value = resolve(dst_port->value, tmp);

      } अन्यथा {
              // The other side is a superset of this side.
            vvp_vector8_t tmp = part_expand(val, branch->width, branch->offset);
            dst_port->value = resolve(dst_port->value, tmp);
      }

        // If the resolved value for the port has changed, push the new
        // value back into the network.
      अगर (! dst_port->value.eeq(old_val)) {
            list<vvp_branch_ptr_t> connections;

	    vvp_branch_ptr_t dst_side(branch, dst_ab);
	    island_collect_गाँठ(connections, dst_side);

	    धकेलो_मूल्य_through_branches(dst_port->value, connections);
      }
}

अटल व्योम धकेलो_मूल्य_through_branches(स्थिर vvp_vector8_t&val,
					स्थिर list<vvp_branch_ptr_t>&connections)
{
      क्रम (list<vvp_branch_ptr_t>::const_iterator idx = connections.cbegin()
		 ; idx != connections.cend() ; ++ idx ) {

            धकेलो_मूल्य_through_branch(val, *idx);
      }
}

/*
 * This method resolves the value for a branch recursively. It uses
 * recursive descent to span the graph of branches, pushing values
 * through the network until a stable state is reached.
 */
व्योम vvp_island_branch_tran::run_resolution()
{
      list<vvp_branch_ptr_t> connections;
      vvp_island_port*port;

	// If the A side port hasn't already been visited, then push
        // its input value through all the branches connected to it.
      port = dynamic_cast<vvp_island_port*>(a->fun);
      अगर (port->value.size() == 0) {
	    vvp_branch_ptr_t a_side(यह, 0);
	    island_collect_गाँठ(connections, a_side);

	    port->value = island_get_मूल्य(a);
            अगर (port->value.size() != 0)
	          धकेलो_मूल्य_through_branches(port->value, connections);

            connections.सफा();
      }

	// Do the same for the B side port. Note that if the branch
        // is enabled, the B side port will have already been visited
        // when we resolved the A side port.
      port = dynamic_cast<vvp_island_port*>(b->fun);
      अगर (port->value.size() == 0) {
	    vvp_branch_ptr_t b_side(यह, 1);
	    island_collect_गाँठ(connections, b_side);

	    port->value = island_get_मूल्य(b);
	    अगर (port->value.size() != 0)
	          धकेलो_मूल्य_through_branches(port->value, connections);

            connections.सफा();
      }
}

व्योम vvp_island_branch_tran::run_output()
{
      vvp_island_port*port;

	// If the A side port hasn't already been updated, send the
        // resolved value to the output.
      port = dynamic_cast<vvp_island_port*>(a->fun);
      अगर (port->value.size() != 0) {
	    island_send_मूल्य(a, port->value);
	    port->value = vvp_vector8_t::nil;
      }

	// Do the same for the B side port.
      port = dynamic_cast<vvp_island_port*>(b->fun);
      अगर (port->value.size() != 0) {
	    island_send_मूल्य(b, port->value);
	    port->value = vvp_vector8_t::nil;
      }
}

व्योम compile_island_tran(अक्षर*label)
{
      vvp_island*use_island = नया vvp_island_tran;
      compile_island_base(label, use_island);
}

व्योम compile_island_tranif(पूर्णांक sense, अक्षर*island, अक्षर*pa, अक्षर*pb, अक्षर*pe,
                           bool resistive)
{
      vvp_island*use_island = compile_find_island(island);
      निश्चित(use_island);
      मुक्त(island);

      vvp_net_t*en = शून्य;

      अगर (pe) {
	    en = use_island->find_port(pe);
	    निश्चित(en);
	    मुक्त(pe);
      }

      vvp_island_branch_tran*br = नया vvp_island_branch_tran(en,
                                                             sense ? true :
                                                                     false,
                                                             0, 0, 0, resistive);

      use_island->add_branch(br, pa, pb);

      मुक्त(pa);
      मुक्त(pb);
}


व्योम compile_island_tranvp(अक्षर*island, अक्षर*pa, अक्षर*pb,
			   अचिन्हित wid, अचिन्हित par, अचिन्हित off)
{
      vvp_island*use_island = compile_find_island(island);
      निश्चित(use_island);
      मुक्त(island);

      vvp_island_branch_tran*br = नया vvp_island_branch_tran(शून्य, false,
                                                             wid, par, off, false);

      use_island->add_branch(br, pa, pb);

      मुक्त(pa);
      मुक्त(pb);
}
