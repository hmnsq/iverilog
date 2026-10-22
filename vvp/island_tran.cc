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

using नामाकाश std;

वर्ग vvp_island_tran : सार्वजनिक vvp_island {

    सार्वजनिक:
      शून्य run_island() override;
      शून्य मोजणी_drivers(vvp_island_port*port, अचिन्हित bit_idx,
                         अचिन्हित counts[3]) override;
};

गणना tran_state_t {
      tran_disabled,
      tran_enabled,
      tran_unknown
};

संरचना vvp_island_branch_tran : सार्वजनिक vvp_island_branch {

      vvp_island_branch_tran(vvp_net_t*en__, तार्किक active_वरचा,
                             अचिन्हित width__, अचिन्हित part__,
                             अचिन्हित offset__, तार्किक resistive__);
      शून्य run_test_enabled();
      तार्किक rerun_test_enabled();
      शून्य run_resolution();
      शून्य run_output();

      vvp_net_t*en;
      अचिन्हित width, part, offset;
      तार्किक active_वरचा_2, resistive;
      tran_state_t state;
};

vvp_island_branch_tran::vvp_island_branch_tran(vvp_net_t*en__,
                                               तार्किक active_वरचा,
                                               अचिन्हित width__,
                                               अचिन्हित part__,
                                               अचिन्हित offset__,
                                               तार्किक resistive__)
: en(en__), width(width__), part(part__), offset(offset__),
  active_वरचा_2(active_वरचा), resistive(resistive__)
{
      state = en__ ? tran_disabled : tran_enabled;
}

स्थिर ओळीत vvp_island_branch_tran* BRANCH_TRAN(vvp_island_branch*tmp)
{
      vvp_island_branch_tran*res = dynamic_cast<vvp_island_branch_tran*>(tmp);
      प्रतिज्ञा(res);
      परत res;
}

/*
 * The run_island() method is called by the scheduler to run the
 * entire island. We run the island by calling run_resolution() for
 * all the branches in the island.
*/
शून्य vvp_island_tran::run_island()
{
	// Test to see if any of the branches are enabled. This loop
	// tests the enabled inputs for all the branches and caches
	// the results in the state for each branch.
      साठी (vvp_island_branch*cur = branches_ ; cur ; cur = cur->next_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    प्रतिज्ञा(tmp);
	    tmp->run_test_enabled();
      }

rerun:
	// Now resolve all the branches in the island.
      साठी (vvp_island_branch*cur = branches_ ; cur ; cur = cur->next_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    प्रतिज्ञा(tmp);
	    tmp->run_resolution();
      }

	// Now output the resolved values.
      साठी (vvp_island_branch*cur = branches_ ; cur ; cur = cur->next_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    प्रतिज्ञा(tmp);
	    tmp->run_output();
      }

	// Now check if the enable inputs have been affected by the resolution.
      तार्किक enable_changed = असत्य;
      साठी (vvp_island_branch*cur = branches_ ; cur ; cur = cur->next_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    प्रतिज्ञा(tmp);
	    enable_changed |= tmp->rerun_test_enabled();
      }
      जर (enable_changed) जा rerun;
}

स्थिर शून्य मोजणी_drivers_2(vvp_branch_ptr_t cur, तार्किक other_side_visited,
                           अचिन्हित bit_idx, अचिन्हित counts[3])
{
        // First count any value driven into the port associated with
        // the current endpoint.
      vvp_net_t*net = cur.port() ? cur.ptr()->b : cur.ptr()->a;
      vvp_scalar_t bit = island_get_मूल्य(net).value(bit_idx);
      update_driver_counts(bit.value(), counts);

        // Now handle all the branches attached to that port.
      vvp_branch_ptr_t idx = cur;
      करा {
            vvp_island_branch_tran*tmp = BRANCH_TRAN(idx.ptr());

              // If this branch represents a tran gate, we count the
              // value on the other side of the tran (providing it is
              // enabled) as a single driver.
            जर (tmp->width == 0) {
                  जर (tmp->state == tran_enabled) {
                        net = idx.port() ? idx.ptr()->a : idx.ptr()->b;
                        bit = island_get_sent_मूल्य(net).value(bit_idx);
                        update_driver_counts(bit.value(), counts);
                  }
                  सुरूठेवा;
            }

              // If we get here, this branch is a part select. If we've
              // just come from the other end of the branch, we're done.
            जर ((idx == cur) && other_side_visited)
                  सुरूठेवा;

              // If this is the narrow end of the part select, the other
              // end must include the bit we are interested in. Follow
              // the branch to collect any drivers on the other side.
            जर (idx.port() == 1) {
                  vvp_branch_ptr_t a_side(tmp, 0);
                  मोजणी_drivers_2(a_side, सत्य, tmp->offset + bit_idx, counts);
                  सुरूठेवा;
            }

              // If we get here, this branch is the wide end of a part
              // select. If the bit we are interested in is within the
              // selected part, follow the branch to collect any drivers
              // on the other side.
            जर ((bit_idx >= tmp->offset) && (bit_idx < tmp->offset+tmp->part)) {
                  vvp_branch_ptr_t b_side(tmp, 1);
                  मोजणी_drivers_2(b_side, सत्य, bit_idx - tmp->offset, counts);
                  सुरूठेवा;
            }
      } पर्यंत ((idx = next(idx)) != cur);
}

शून्य vvp_island_tran::मोजणी_drivers(vvp_island_port*port, अचिन्हित bit_idx,
                                    अचिन्हित counts[3])
{
        // First we need to find a branch that is attached to the specified
        // port. Unfortunately there's no quick way to do this.
      vvp_island_branch*branch = branches_;
      अचिन्हित side = 0;
      पर्यंत (branch) {
            जर (branch->a->fun == port) {
                  side = 0;
                  थांबा;
            }
            जर (branch->b->fun == port) {
                  side = 1;
                  थांबा;
            }
            branch = branch->next_branch;
      }
      प्रतिज्ञा(branch);

        // Now count the drivers, pushing through the network as necessary.
      vvp_branch_ptr_t endpoint(branch, side);
      मोजणी_drivers_2(endpoint, असत्य, bit_idx, counts);
}

शून्य vvp_island_branch_tran::run_test_enabled()
{
      स्थिरांक vvp_island_port*ep = en? dynamic_cast<vvp_island_port*> (en->fun) : रिक्त;

	// If there is no ep port (no "enabled" input) then this is a
	// tran branch. Assume it is always enabled.
      जर (ep == 0) {
	    state = tran_enabled;
	    परत;
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
      जर (ep->outvalue.size() != 0)
	    enable_val = ep->outvalue.value(0).value();
      अन्यथा जर (ep->invalue.size() == 0)
	    enable_val = BIT4_Z;
      अन्यथा
	    enable_val = ep->invalue.value(0).value();

      निवड (enable_val) {
	  स्थिती BIT4_0:
	    state = active_वरचा_2 ? tran_disabled : tran_enabled;
	    थांबा;
	  स्थिती BIT4_1:
	    state = active_वरचा_2 ? tran_enabled : tran_disabled;
	    थांबा;
	  पूर्वनिर्धारित:
	    state = tran_unknown;
	    थांबा;
      }
}

तार्किक vvp_island_branch_tran::rerun_test_enabled()
{
      स्थिरांक vvp_island_port*ep = en? dynamic_cast<vvp_island_port*> (en->fun) : रिक्त;

      जर (ep == 0)
	    परत असत्य;

	// We are only looking for changes resulting from running the island
	// resolution. If the outvalue is nil, we know that the enable port
	// is an .import, so won't be affected.
      जर (ep->outvalue.size() == 0)
	    परत असत्य;

      vvp_bit4_t enable_val = ep->outvalue.value(0).value();

      tran_state_t old_state = state;

      निवड (enable_val) {
	  स्थिती BIT4_0:
	    state = active_वरचा_2 ? tran_disabled : tran_enabled;
	    थांबा;
	  स्थिती BIT4_1:
	    state = active_वरचा_2 ? tran_enabled : tran_disabled;
	    थांबा;
	  पूर्वनिर्धारित:
	    state = tran_unknown;
	    थांबा;
      }

      परत state != old_state;
}

// The IEEE standard does not specify the behaviour when a tranif control
// input is 'x' or 'z'. We use the rules that are given for MOS switches.
ओळीत vvp_vector8_t resolve_ambiguous(स्थिरांक vvp_vector8_t&a,
                                       स्थिरांक vvp_vector8_t&b,
                                       tran_state_t state,
                                       स्थिरांक अचिन्हित str_map[8])
{
      प्रतिज्ञा(a.size() == b.size());
      vvp_vector8_t out (a.size());

      साठी (अचिन्हित idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_scalar_t a_bit = a.value(idx);
	    vvp_scalar_t b_bit = b.value(idx);
            b_bit = vvp_scalar_t(b_bit.value(),
                                 str_map[b_bit.strength0()],
                                 str_map[b_bit.strength1()]);
	    जर (state == tran_unknown) {
		  निवड (b_bit.value()) {
		      स्थिती BIT4_0:
			b_bit = vvp_scalar_t(BIT4_X, b_bit.strength0(), 0);
			थांबा;
		      स्थिती BIT4_1:
			b_bit = vvp_scalar_t(BIT4_X, 0, b_bit.strength1());
			थांबा;
		      पूर्वनिर्धारित:
			थांबा;
		  }
	    }
	    out.set_bit(idx, resolve(a_bit, b_bit));
      }
      परत out;
}

स्थिर शून्य ढकला_मूल्य_through_branches(स्थिरांक vvp_vector8_t&val,
					स्थिरांक list<vvp_branch_ptr_t>&connections);

स्थिर शून्य ढकला_मूल्य_through_branch(स्थिरांक vvp_vector8_t&val,
                                      vvp_branch_ptr_t cur)
{
      vvp_island_branch_tran*branch = BRANCH_TRAN(cur.ptr());

        // If the branch is disabled, skip.
      जर (branch->state == tran_disabled)
            परत;

      अचिन्हित src_ab = cur.port();
      अचिन्हित dst_ab = src_ab^1;

      vvp_net_t*dst_net = dst_ab? branch->b : branch->a;
      vvp_island_port*dst_port = dynamic_cast<vvp_island_port*>(dst_net->fun);

      vvp_vector8_t old_val = dst_port->value;

        // If the port on the other side has not yet been visited,
        // get its input value.
      जर (dst_port->value.size() == 0)
            dst_port->value = island_get_मूल्य(dst_net);

        // If we don't yet have an initial value for the port, skip.
      जर (dst_port->value.size() == 0)
            परत;

        // Now resolve the pushed value with whatever values we have
        // previously collected (and resolved) for the port.
      जर (branch->width == 0) {
              // There are no part selects.
            dst_port->value = resolve_ambiguous(dst_port->value, val, branch->state,
                                                vvp_switch_strength_map[branch->resistive]);

      } अन्यथा जर (dst_ab == 1) {
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
      जर (! dst_port->value.eeq(old_val)) {
            list<vvp_branch_ptr_t> connections;

	    vvp_branch_ptr_t dst_side(branch, dst_ab);
	    island_collect_गाठ(connections, dst_side);

	    ढकला_मूल्य_through_branches(dst_port->value, connections);
      }
}

स्थिर शून्य ढकला_मूल्य_through_branches(स्थिरांक vvp_vector8_t&val,
					स्थिरांक list<vvp_branch_ptr_t>&connections)
{
      साठी (list<vvp_branch_ptr_t>::const_iterator idx = connections.cbegin()
		 ; idx != connections.cend() ; ++ idx ) {

            ढकला_मूल्य_through_branch(val, *idx);
      }
}

/*
 * This method resolves the value for a branch recursively. It uses
 * recursive descent to span the graph of branches, pushing values
 * through the network until a stable state is reached.
 */
शून्य vvp_island_branch_tran::run_resolution()
{
      list<vvp_branch_ptr_t> connections;
      vvp_island_port*port;

	// If the A side port hasn't already been visited, then push
        // its input value through all the branches connected to it.
      port = dynamic_cast<vvp_island_port*>(a->fun);
      जर (port->value.size() == 0) {
	    vvp_branch_ptr_t a_side(हे, 0);
	    island_collect_गाठ(connections, a_side);

	    port->value = island_get_मूल्य(a);
            जर (port->value.size() != 0)
	          ढकला_मूल्य_through_branches(port->value, connections);

            connections.clear();
      }

	// Do the same for the B side port. Note that if the branch
        // is enabled, the B side port will have already been visited
        // when we resolved the A side port.
      port = dynamic_cast<vvp_island_port*>(b->fun);
      जर (port->value.size() == 0) {
	    vvp_branch_ptr_t b_side(हे, 1);
	    island_collect_गाठ(connections, b_side);

	    port->value = island_get_मूल्य(b);
	    जर (port->value.size() != 0)
	          ढकला_मूल्य_through_branches(port->value, connections);

            connections.clear();
      }
}

शून्य vvp_island_branch_tran::run_output()
{
      vvp_island_port*port;

	// If the A side port hasn't already been updated, send the
        // resolved value to the output.
      port = dynamic_cast<vvp_island_port*>(a->fun);
      जर (port->value.size() != 0) {
	    island_send_मूल्य(a, port->value);
	    port->value = vvp_vector8_t::nil;
      }

	// Do the same for the B side port.
      port = dynamic_cast<vvp_island_port*>(b->fun);
      जर (port->value.size() != 0) {
	    island_send_मूल्य(b, port->value);
	    port->value = vvp_vector8_t::nil;
      }
}

शून्य compile_island_tran(अक्षर*label)
{
      vvp_island*use_island = नवीन vvp_island_tran;
      compile_island_base(label, use_island);
}

शून्य compile_island_tranif(पूर्णांक sense, अक्षर*island, अक्षर*pa, अक्षर*pb, अक्षर*pe,
                           तार्किक resistive)
{
      vvp_island*use_island = compile_find_island(island);
      प्रतिज्ञा(use_island);
      मुक्त(island);

      vvp_net_t*en = रिक्त;

      जर (pe) {
	    en = use_island->find_port(pe);
	    प्रतिज्ञा(en);
	    मुक्त(pe);
      }

      vvp_island_branch_tran*br = नवीन vvp_island_branch_tran(en,
                                                             sense ? सत्य :
                                                                     असत्य,
                                                             0, 0, 0, resistive);

      use_island->add_branch(br, pa, pb);

      मुक्त(pa);
      मुक्त(pb);
}


शून्य compile_island_tranvp(अक्षर*island, अक्षर*pa, अक्षर*pb,
			   अचिन्हित wid, अचिन्हित par, अचिन्हित off)
{
      vvp_island*use_island = compile_find_island(island);
      प्रतिज्ञा(use_island);
      मुक्त(island);

      vvp_island_branch_tran*br = नवीन vvp_island_branch_tran(रिक्त, असत्य,
                                                             wid, par, off, असत्य);

      use_island->add_branch(br, pa, pb);

      मुक्त(pa);
      मुक्त(pb);
}
