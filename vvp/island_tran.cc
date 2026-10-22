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

उपयुज्य नामाकाश std;

वर्ग vvp_island_tran : सार्वजनिक vvp_island {

    सार्वजनिक:
      शून्य सततम्_island() override;
      शून्य गणनम्_drivers(vvp_island_पत्तनम्*पत्तनम्, अचिह्नित bit_idx,
                         अचिह्नित counts[3]) override;
};

गणना tran_state_t {
      tran_disabled,
      tran_enabled,
      tran_unknown
};

संरचना vvp_island_branch_tran : सार्वजनिक vvp_island_branch {

      vvp_island_branch_tran(vvp_net_t*en__, तार्किक सक्रिय_उच्चः,
                             अचिह्नित विस्तारः_3, अचिह्नित part__,
                             अचिह्नित विस्थापनम्_3, तार्किक resistive__);
      शून्य सततम्_test_enabled();
      तार्किक rerun_test_enabled();
      शून्य सततम्_resolution();
      शून्य सततम्_output();

      vvp_net_t*en;
      अचिह्नित width, part, विस्थापनम्_2;
      तार्किक सक्रिय_उच्चः_2, resistive;
      tran_state_t state;
};

vvp_island_branch_tran::vvp_island_branch_tran(vvp_net_t*en__,
                                               तार्किक सक्रिय_उच्चः,
                                               अचिह्नित विस्तारः_3,
                                               अचिह्नित part__,
                                               अचिह्नित विस्थापनम्_3,
                                               तार्किक resistive__)
: en(en__), width(विस्तारः_3), part(part__), विस्थापनम्_2(विस्थापनम्_3),
  सक्रिय_उच्चः_2(सक्रिय_उच्चः), resistive(resistive__)
{
      state = en__ ? tran_disabled : tran_enabled;
}

स्थिर अन्तःस्थ vvp_island_branch_tran* BRANCH_TRAN(vvp_island_branch*tmp)
{
      vvp_island_branch_tran*res = dynamic_cast<vvp_island_branch_tran*>(tmp);
      प्रतिज्ञा(res);
      प्रत्यागम res;
}

/*
 * The run_island() method is called by the scheduler to run the
 * entire island. We run the island by calling run_resolution() for
 * all the branches in the island.
*/
शून्य vvp_island_tran::सततम्_island()
{
	// Test to see if any of the branches are enabled. This loop
	// tests the enabled inputs for all the branches and caches
	// the results in the state for each branch.
      प्रति (vvp_island_branch*cur = branches_ ; cur ; cur = cur->अग्र_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    प्रतिज्ञा(tmp);
	    tmp->सततम्_test_enabled();
      }

rerun:
	// Now resolve all the branches in the island.
      प्रति (vvp_island_branch*cur = branches_ ; cur ; cur = cur->अग्र_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    प्रतिज्ञा(tmp);
	    tmp->सततम्_resolution();
      }

	// Now output the resolved values.
      प्रति (vvp_island_branch*cur = branches_ ; cur ; cur = cur->अग्र_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    प्रतिज्ञा(tmp);
	    tmp->सततम्_output();
      }

	// Now check if the enable inputs have been affected by the resolution.
      तार्किक enable_परिवर्तितम् = असत्य;
      प्रति (vvp_island_branch*cur = branches_ ; cur ; cur = cur->अग्र_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    प्रतिज्ञा(tmp);
	    enable_परिवर्तितम् |= tmp->rerun_test_enabled();
      }
      यदि (enable_परिवर्तितम्) गच्छ rerun;
}

स्थिर शून्य गणनम्_drivers_2(vvp_branch_ptr_t cur, तार्किक other_side_visited,
                           अचिह्नित bit_idx, अचिह्नित counts[3])
{
        // First count any value driven into the port associated with
        // the current endpoint.
      vvp_net_t*net = cur.पत्तनम्() ? cur.ptr()->b : cur.ptr()->a;
      vvp_scalar_t bit = island_get_मानम्(net).value(bit_idx);
      update_driver_counts(bit.value(), counts);

        // Now handle all the branches attached to that port.
      vvp_branch_ptr_t idx = cur;
      कुरु {
            vvp_island_branch_tran*tmp = BRANCH_TRAN(idx.ptr());

              // If this branch represents a tran gate, we count the
              // value on the other side of the tran (providing it is
              // enabled) as a single driver.
            यदि (tmp->width == 0) {
                  यदि (tmp->state == tran_enabled) {
                        net = idx.पत्तनम्() ? idx.ptr()->a : idx.ptr()->b;
                        bit = island_get_प्रेषितम्_मानम्(net).value(bit_idx);
                        update_driver_counts(bit.value(), counts);
                  }
                  अनुवर्त;
            }

              // If we get here, this branch is a part select. If we've
              // just come from the other end of the branch, we're done.
            यदि ((idx == cur) && other_side_visited)
                  अनुवर्त;

              // If this is the narrow end of the part select, the other
              // end must include the bit we are interested in. Follow
              // the branch to collect any drivers on the other side.
            यदि (idx.पत्तनम्() == 1) {
                  vvp_branch_ptr_t a_side(tmp, 0);
                  गणनम्_drivers_2(a_side, सत्य, tmp->विस्थापनम्_2 + bit_idx, counts);
                  अनुवर्त;
            }

              // If we get here, this branch is the wide end of a part
              // select. If the bit we are interested in is within the
              // selected part, follow the branch to collect any drivers
              // on the other side.
            यदि ((bit_idx >= tmp->विस्थापनम्_2) && (bit_idx < tmp->विस्थापनम्_2+tmp->part)) {
                  vvp_branch_ptr_t b_side(tmp, 1);
                  गणनम्_drivers_2(b_side, सत्य, bit_idx - tmp->विस्थापनम्_2, counts);
                  अनुवर्त;
            }
      } यावत् ((idx = अग्र_2(idx)) != cur);
}

शून्य vvp_island_tran::गणनम्_drivers(vvp_island_पत्तनम्*पत्तनम्, अचिह्नित bit_idx,
                                    अचिह्नित counts[3])
{
        // First we need to find a branch that is attached to the specified
        // port. Unfortunately there's no quick way to do this.
      vvp_island_branch*branch = branches_;
      अचिह्नित side = 0;
      यावत् (branch) {
            यदि (branch->a->fun == पत्तनम्) {
                  side = 0;
                  भङ्ग;
            }
            यदि (branch->b->fun == पत्तनम्) {
                  side = 1;
                  भङ्ग;
            }
            branch = branch->अग्र_branch;
      }
      प्रतिज्ञा(branch);

        // Now count the drivers, pushing through the network as necessary.
      vvp_branch_ptr_t endpoint(branch, side);
      गणनम्_drivers_2(endpoint, असत्य, bit_idx, counts);
}

शून्य vvp_island_branch_tran::सततम्_test_enabled()
{
      ध्रुव vvp_island_पत्तनम्*ep = en? dynamic_cast<vvp_island_पत्तनम्*> (en->fun) : रिक्त;

	// If there is no ep port (no "enabled" input) then this is a
	// tran branch. Assume it is always enabled.
      यदि (ep == 0) {
	    state = tran_enabled;
	    प्रत्यागम;
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
      यदि (ep->outvalue.size() != 0)
	    enable_val = ep->outvalue.value(0).value();
      अन्यथा यदि (ep->invalue.size() == 0)
	    enable_val = BIT4_Z;
      अन्यथा
	    enable_val = ep->invalue.value(0).value();

      विकल्प (enable_val) {
	  पक्ष BIT4_0:
	    state = सक्रिय_उच्चः_2 ? tran_disabled : tran_enabled;
	    भङ्ग;
	  पक्ष BIT4_1:
	    state = सक्रिय_उच्चः_2 ? tran_enabled : tran_disabled;
	    भङ्ग;
	  स्वभाव:
	    state = tran_unknown;
	    भङ्ग;
      }
}

तार्किक vvp_island_branch_tran::rerun_test_enabled()
{
      ध्रुव vvp_island_पत्तनम्*ep = en? dynamic_cast<vvp_island_पत्तनम्*> (en->fun) : रिक्त;

      यदि (ep == 0)
	    प्रत्यागम असत्य;

	// We are only looking for changes resulting from running the island
	// resolution. If the outvalue is nil, we know that the enable port
	// is an .import, so won't be affected.
      यदि (ep->outvalue.size() == 0)
	    प्रत्यागम असत्य;

      vvp_bit4_t enable_val = ep->outvalue.value(0).value();

      tran_state_t old_state = state;

      विकल्प (enable_val) {
	  पक्ष BIT4_0:
	    state = सक्रिय_उच्चः_2 ? tran_disabled : tran_enabled;
	    भङ्ग;
	  पक्ष BIT4_1:
	    state = सक्रिय_उच्चः_2 ? tran_enabled : tran_disabled;
	    भङ्ग;
	  स्वभाव:
	    state = tran_unknown;
	    भङ्ग;
      }

      प्रत्यागम state != old_state;
}

// The IEEE standard does not specify the behaviour when a tranif control
// input is 'x' or 'z'. We use the rules that are given for MOS switches.
अन्तःस्थ vvp_vector8_t resolve_ambiguous(ध्रुव vvp_vector8_t&a,
                                       ध्रुव vvp_vector8_t&b,
                                       tran_state_t state,
                                       ध्रुव अचिह्नित str_map[8])
{
      प्रतिज्ञा(a.size() == b.size());
      vvp_vector8_t out (a.size());

      प्रति (अचिह्नित idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_scalar_t a_bit = a.value(idx);
	    vvp_scalar_t b_bit = b.value(idx);
            b_bit = vvp_scalar_t(b_bit.value(),
                                 str_map[b_bit.strength0()],
                                 str_map[b_bit.strength1()]);
	    यदि (state == tran_unknown) {
		  विकल्प (b_bit.value()) {
		      पक्ष BIT4_0:
			b_bit = vvp_scalar_t(BIT4_X, b_bit.strength0(), 0);
			भङ्ग;
		      पक्ष BIT4_1:
			b_bit = vvp_scalar_t(BIT4_X, 0, b_bit.strength1());
			भङ्ग;
		      स्वभाव:
			भङ्ग;
		  }
	    }
	    out.समुच्चयः_bit(idx, resolve(a_bit, b_bit));
      }
      प्रत्यागम out;
}

स्थिर शून्य प्रक्षिप_मानम्_through_branches(ध्रुव vvp_vector8_t&val,
					ध्रुव list<vvp_branch_ptr_t>&connections);

स्थिर शून्य प्रक्षिप_मानम्_through_branch(ध्रुव vvp_vector8_t&val,
                                      vvp_branch_ptr_t cur)
{
      vvp_island_branch_tran*branch = BRANCH_TRAN(cur.ptr());

        // If the branch is disabled, skip.
      यदि (branch->state == tran_disabled)
            प्रत्यागम;

      अचिह्नित src_ab = cur.पत्तनम्();
      अचिह्नित dst_ab = src_ab^1;

      vvp_net_t*dst_net = dst_ab? branch->b : branch->a;
      vvp_island_पत्तनम्*dst_पत्तनम् = dynamic_cast<vvp_island_पत्तनम्*>(dst_net->fun);

      vvp_vector8_t old_val = dst_पत्तनम्->value;

        // If the port on the other side has not yet been visited,
        // get its input value.
      यदि (dst_पत्तनम्->value.size() == 0)
            dst_पत्तनम्->value = island_get_मानम्(dst_net);

        // If we don't yet have an initial value for the port, skip.
      यदि (dst_पत्तनम्->value.size() == 0)
            प्रत्यागम;

        // Now resolve the pushed value with whatever values we have
        // previously collected (and resolved) for the port.
      यदि (branch->width == 0) {
              // There are no part selects.
            dst_पत्तनम्->value = resolve_ambiguous(dst_पत्तनम्->value, val, branch->state,
                                                vvp_switch_strength_map[branch->resistive]);

      } अन्यथा यदि (dst_ab == 1) {
              // The other side is a strict subset (part select)
              // of this side.
            vvp_vector8_t tmp = val.subvalue(branch->विस्थापनम्_2, branch->part);
            dst_पत्तनम्->value = resolve(dst_पत्तनम्->value, tmp);

      } अन्यथा {
              // The other side is a superset of this side.
            vvp_vector8_t tmp = part_expand(val, branch->width, branch->विस्थापनम्_2);
            dst_पत्तनम्->value = resolve(dst_पत्तनम्->value, tmp);
      }

        // If the resolved value for the port has changed, push the new
        // value back into the network.
      यदि (! dst_पत्तनम्->value.eeq(old_val)) {
            list<vvp_branch_ptr_t> connections;

	    vvp_branch_ptr_t dst_side(branch, dst_ab);
	    island_collect_ग्रन्थिः(connections, dst_side);

	    प्रक्षिप_मानम्_through_branches(dst_पत्तनम्->value, connections);
      }
}

स्थिर शून्य प्रक्षिप_मानम्_through_branches(ध्रुव vvp_vector8_t&val,
					ध्रुव list<vvp_branch_ptr_t>&connections)
{
      प्रति (list<vvp_branch_ptr_t>::const_iterator idx = connections.cbegin()
		 ; idx != connections.cend() ; ++ idx ) {

            प्रक्षिप_मानम्_through_branch(val, *idx);
      }
}

/*
 * This method resolves the value for a branch recursively. It uses
 * recursive descent to span the graph of branches, pushing values
 * through the network until a stable state is reached.
 */
शून्य vvp_island_branch_tran::सततम्_resolution()
{
      list<vvp_branch_ptr_t> connections;
      vvp_island_पत्तनम्*पत्तनम्;

	// If the A side port hasn't already been visited, then push
        // its input value through all the branches connected to it.
      पत्तनम् = dynamic_cast<vvp_island_पत्तनम्*>(a->fun);
      यदि (पत्तनम्->value.size() == 0) {
	    vvp_branch_ptr_t a_side(एतत्, 0);
	    island_collect_ग्रन्थिः(connections, a_side);

	    पत्तनम्->value = island_get_मानम्(a);
            यदि (पत्तनम्->value.size() != 0)
	          प्रक्षिप_मानम्_through_branches(पत्तनम्->value, connections);

            connections.clear();
      }

	// Do the same for the B side port. Note that if the branch
        // is enabled, the B side port will have already been visited
        // when we resolved the A side port.
      पत्तनम् = dynamic_cast<vvp_island_पत्तनम्*>(b->fun);
      यदि (पत्तनम्->value.size() == 0) {
	    vvp_branch_ptr_t b_side(एतत्, 1);
	    island_collect_ग्रन्थिः(connections, b_side);

	    पत्तनम्->value = island_get_मानम्(b);
	    यदि (पत्तनम्->value.size() != 0)
	          प्रक्षिप_मानम्_through_branches(पत्तनम्->value, connections);

            connections.clear();
      }
}

शून्य vvp_island_branch_tran::सततम्_output()
{
      vvp_island_पत्तनम्*पत्तनम्;

	// If the A side port hasn't already been updated, send the
        // resolved value to the output.
      पत्तनम् = dynamic_cast<vvp_island_पत्तनम्*>(a->fun);
      यदि (पत्तनम्->value.size() != 0) {
	    island_send_मानम्(a, पत्तनम्->value);
	    पत्तनम्->value = vvp_vector8_t::nil;
      }

	// Do the same for the B side port.
      पत्तनम् = dynamic_cast<vvp_island_पत्तनम्*>(b->fun);
      यदि (पत्तनम्->value.size() != 0) {
	    island_send_मानम्(b, पत्तनम्->value);
	    पत्तनम्->value = vvp_vector8_t::nil;
      }
}

शून्य compile_island_tran(वर्ण*label)
{
      vvp_island*use_island = नूतन vvp_island_tran;
      compile_island_आधारः(label, use_island);
}

शून्य compile_island_tranif(पूर्णाङ्क sense, वर्ण*island, वर्ण*pa, वर्ण*pb, वर्ण*pe,
                           तार्किक resistive)
{
      vvp_island*use_island = compile_find_island(island);
      प्रतिज्ञा(use_island);
      मुक्त(island);

      vvp_net_t*en = रिक्त;

      यदि (pe) {
	    en = use_island->find_पत्तनम्(pe);
	    प्रतिज्ञा(en);
	    मुक्त(pe);
      }

      vvp_island_branch_tran*br = नूतन vvp_island_branch_tran(en,
                                                             sense ? सत्य :
                                                                     असत्य,
                                                             0, 0, 0, resistive);

      use_island->सङ्कलय_branch(br, pa, pb);

      मुक्त(pa);
      मुक्त(pb);
}


शून्य compile_island_tranvp(वर्ण*island, वर्ण*pa, वर्ण*pb,
			   अचिह्नित wid, अचिह्नित par, अचिह्नित off)
{
      vvp_island*use_island = compile_find_island(island);
      प्रतिज्ञा(use_island);
      मुक्त(island);

      vvp_island_branch_tran*br = नूतन vvp_island_branch_tran(रिक्त, असत्य,
                                                             wid, par, off, असत्य);

      use_island->सङ्कलय_branch(br, pa, pb);

      मुक्त(pa);
      मुक्त(pb);
}
