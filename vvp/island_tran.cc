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

ಬಳಸಿ ನಾಮಾಕಾಶ std;

ವರ್ಗ vvp_island_tran : ಸಾರ್ವಜನಿಕ vvp_island {

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ ಸತತ_island() override;
      ಶೂನ್ಯ ಎಣಿಕೆ_drivers(vvp_island_ಪೋರ್ಟ್*ಪೋರ್ಟ್, ಚಿಹ್ನೆರಹಿತ ಬಿಟ್_idx,
                         ಚಿಹ್ನೆರಹಿತ counts[3]) override;
};

ಎಣಿಕೆ tran_state_t {
      tran_disabled,
      tran_enabled,
      tran_unknown
};

ರಚನೆ vvp_island_branch_tran : ಸಾರ್ವಜನಿಕ vvp_island_branch {

      vvp_island_branch_tran(vvp_net_t*en__, ತಾರ್ಕಿಕ ಸಕ್ರಿಯ_ಮೇಲಿನ,
                             ಚಿಹ್ನೆರಹಿತ ಅಗಲ_3, ಚಿಹ್ನೆರಹಿತ part__,
                             ಚಿಹ್ನೆರಹಿತ ಸ್ಥಾನಾಂತರ_3, ತಾರ್ಕಿಕ resistive__);
      ಶೂನ್ಯ ಸತತ_test_enabled();
      ತಾರ್ಕಿಕ rerun_test_enabled();
      ಶೂನ್ಯ ಸತತ_resolution();
      ಶೂನ್ಯ ಸತತ_output();

      vvp_net_t*en;
      ಚಿಹ್ನೆರಹಿತ width, part, ಸ್ಥಾನಾಂತರ_2;
      ತಾರ್ಕಿಕ ಸಕ್ರಿಯ_ಮೇಲಿನ_2, resistive;
      tran_state_t state;
};

vvp_island_branch_tran::vvp_island_branch_tran(vvp_net_t*en__,
                                               ತಾರ್ಕಿಕ ಸಕ್ರಿಯ_ಮೇಲಿನ,
                                               ಚಿಹ್ನೆರಹಿತ ಅಗಲ_3,
                                               ಚಿಹ್ನೆರಹಿತ part__,
                                               ಚಿಹ್ನೆರಹಿತ ಸ್ಥಾನಾಂತರ_3,
                                               ತಾರ್ಕಿಕ resistive__)
: en(en__), width(ಅಗಲ_3), part(part__), ಸ್ಥಾನಾಂತರ_2(ಸ್ಥಾನಾಂತರ_3),
  ಸಕ್ರಿಯ_ಮೇಲಿನ_2(ಸಕ್ರಿಯ_ಮೇಲಿನ), resistive(resistive__)
{
      state = en__ ? tran_disabled : tran_enabled;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ vvp_island_branch_tran* BRANCH_TRAN(vvp_island_branch*tmp)
{
      vvp_island_branch_tran*res = dynamic_cast<vvp_island_branch_tran*>(tmp);
      ಪ್ರತಿಜ್ಞಾ(res);
      ಹಿಂತಿರುಗಿ res;
}

/*
 * The run_island() method is called by the scheduler to run the
 * entire island. We run the island by calling run_resolution() for
 * all the branches in the island.
*/
ಶೂನ್ಯ vvp_island_tran::ಸತತ_island()
{
	// Test to see if any of the branches are enabled. This loop
	// tests the enabled inputs for all the branches and caches
	// the results in the state for each branch.
      ಗಾಗಿ (vvp_island_branch*cur = branches_ ; cur ; cur = cur->ಮುಂದಿನ_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ಪ್ರತಿಜ್ಞಾ(tmp);
	    tmp->ಸತತ_test_enabled();
      }

rerun:
	// Now resolve all the branches in the island.
      ಗಾಗಿ (vvp_island_branch*cur = branches_ ; cur ; cur = cur->ಮುಂದಿನ_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ಪ್ರತಿಜ್ಞಾ(tmp);
	    tmp->ಸತತ_resolution();
      }

	// Now output the resolved values.
      ಗಾಗಿ (vvp_island_branch*cur = branches_ ; cur ; cur = cur->ಮುಂದಿನ_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ಪ್ರತಿಜ್ಞಾ(tmp);
	    tmp->ಸತತ_output();
      }

	// Now check if the enable inputs have been affected by the resolution.
      ತಾರ್ಕಿಕ enable_ಬದಲಾಯಿತು = ಸುಳ್ಳು;
      ಗಾಗಿ (vvp_island_branch*cur = branches_ ; cur ; cur = cur->ಮುಂದಿನ_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ಪ್ರತಿಜ್ಞಾ(tmp);
	    enable_ಬದಲಾಯಿತು |= tmp->rerun_test_enabled();
      }
      ಆದರೆ (enable_ಬದಲಾಯಿತು) ಹೋಗು rerun;
}

ಸ್ಥಿರ ಶೂನ್ಯ ಎಣಿಕೆ_drivers_2(vvp_branch_ptr_t cur, ತಾರ್ಕಿಕ other_side_ಭೇಟಿಯಾದ,
                           ಚಿಹ್ನೆರಹಿತ ಬಿಟ್_idx, ಚಿಹ್ನೆರಹಿತ counts[3])
{
        // First count any value driven into the port associated with
        // the current endpoint.
      vvp_net_t*net = cur.ಪೋರ್ಟ್() ? cur.ptr()->b : cur.ptr()->a;
      vvp_scalar_t ಬಿಟ್ = island_get_ಮೌಲ್ಯ(net).value(ಬಿಟ್_idx);
      update_driver_counts(ಬಿಟ್.value(), counts);

        // Now handle all the branches attached to that port.
      vvp_branch_ptr_t idx = cur;
      ಮಾಡು {
            vvp_island_branch_tran*tmp = BRANCH_TRAN(idx.ptr());

              // If this branch represents a tran gate, we count the
              // value on the other side of the tran (providing it is
              // enabled) as a single driver.
            ಆದರೆ (tmp->width == 0) {
                  ಆದರೆ (tmp->state == tran_enabled) {
                        net = idx.ಪೋರ್ಟ್() ? idx.ptr()->a : idx.ptr()->b;
                        ಬಿಟ್ = island_get_ಕಳುಹಿಸಿದ_ಮೌಲ್ಯ(net).value(ಬಿಟ್_idx);
                        update_driver_counts(ಬಿಟ್.value(), counts);
                  }
                  ಮುಂದುವರಿ;
            }

              // If we get here, this branch is a part select. If we've
              // just come from the other end of the branch, we're done.
            ಆದರೆ ((idx == cur) && other_side_ಭೇಟಿಯಾದ)
                  ಮುಂದುವರಿ;

              // If this is the narrow end of the part select, the other
              // end must include the bit we are interested in. Follow
              // the branch to collect any drivers on the other side.
            ಆದರೆ (idx.ಪೋರ್ಟ್() == 1) {
                  vvp_branch_ptr_t a_side(tmp, 0);
                  ಎಣಿಕೆ_drivers_2(a_side, ನಿಜ, tmp->ಸ್ಥಾನಾಂತರ_2 + ಬಿಟ್_idx, counts);
                  ಮುಂದುವರಿ;
            }

              // If we get here, this branch is the wide end of a part
              // select. If the bit we are interested in is within the
              // selected part, follow the branch to collect any drivers
              // on the other side.
            ಆದರೆ ((ಬಿಟ್_idx >= tmp->ಸ್ಥಾನಾಂತರ_2) && (ಬಿಟ್_idx < tmp->ಸ್ಥಾನಾಂತರ_2+tmp->part)) {
                  vvp_branch_ptr_t b_side(tmp, 1);
                  ಎಣಿಕೆ_drivers_2(b_side, ನಿಜ, ಬಿಟ್_idx - tmp->ಸ್ಥಾನಾಂತರ_2, counts);
                  ಮುಂದುವರಿ;
            }
      } ಆಗ ((idx = ಮುಂದಿನ_2(idx)) != cur);
}

ಶೂನ್ಯ vvp_island_tran::ಎಣಿಕೆ_drivers(vvp_island_ಪೋರ್ಟ್*ಪೋರ್ಟ್, ಚಿಹ್ನೆರಹಿತ ಬಿಟ್_idx,
                                    ಚಿಹ್ನೆರಹಿತ counts[3])
{
        // First we need to find a branch that is attached to the specified
        // port. Unfortunately there's no quick way to do this.
      vvp_island_branch*branch = branches_;
      ಚಿಹ್ನೆರಹಿತ side = 0;
      ಆಗ (branch) {
            ಆದರೆ (branch->a->fun == ಪೋರ್ಟ್) {
                  side = 0;
                  ನಿಲ್ಲಿಸು;
            }
            ಆದರೆ (branch->b->fun == ಪೋರ್ಟ್) {
                  side = 1;
                  ನಿಲ್ಲಿಸು;
            }
            branch = branch->ಮುಂದಿನ_branch;
      }
      ಪ್ರತಿಜ್ಞಾ(branch);

        // Now count the drivers, pushing through the network as necessary.
      vvp_branch_ptr_t endpoint(branch, side);
      ಎಣಿಕೆ_drivers_2(endpoint, ಸುಳ್ಳು, ಬಿಟ್_idx, counts);
}

ಶೂನ್ಯ vvp_island_branch_tran::ಸತತ_test_enabled()
{
      ಸ್ಥಿರಾಂಕ vvp_island_ಪೋರ್ಟ್*ep = en? dynamic_cast<vvp_island_ಪೋರ್ಟ್*> (en->fun) : ಖಾಲಿ;

	// If there is no ep port (no "enabled" input) then this is a
	// tran branch. Assume it is always enabled.
      ಆದರೆ (ep == 0) {
	    state = tran_enabled;
	    ಹಿಂತಿರುಗಿ;
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
      ಆದರೆ (ep->outvalue.size() != 0)
	    enable_val = ep->outvalue.value(0).value();
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ep->invalue.size() == 0)
	    enable_val = BIT4_Z;
      ಇಲ್ಲದಿದ್ದರೆ
	    enable_val = ep->invalue.value(0).value();

      ಆಯ್ಕೆ (enable_val) {
	  ಸ್ಥಿತಿ BIT4_0:
	    state = ಸಕ್ರಿಯ_ಮೇಲಿನ_2 ? tran_disabled : tran_enabled;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_1:
	    state = ಸಕ್ರಿಯ_ಮೇಲಿನ_2 ? tran_enabled : tran_disabled;
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    state = tran_unknown;
	    ನಿಲ್ಲಿಸು;
      }
}

ತಾರ್ಕಿಕ vvp_island_branch_tran::rerun_test_enabled()
{
      ಸ್ಥಿರಾಂಕ vvp_island_ಪೋರ್ಟ್*ep = en? dynamic_cast<vvp_island_ಪೋರ್ಟ್*> (en->fun) : ಖಾಲಿ;

      ಆದರೆ (ep == 0)
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	// We are only looking for changes resulting from running the island
	// resolution. If the outvalue is nil, we know that the enable port
	// is an .import, so won't be affected.
      ಆದರೆ (ep->outvalue.size() == 0)
	    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

      vvp_bit4_t enable_val = ep->outvalue.value(0).value();

      tran_state_t old_state = state;

      ಆಯ್ಕೆ (enable_val) {
	  ಸ್ಥಿತಿ BIT4_0:
	    state = ಸಕ್ರಿಯ_ಮೇಲಿನ_2 ? tran_disabled : tran_enabled;
	    ನಿಲ್ಲಿಸು;
	  ಸ್ಥಿತಿ BIT4_1:
	    state = ಸಕ್ರಿಯ_ಮೇಲಿನ_2 ? tran_enabled : tran_disabled;
	    ನಿಲ್ಲಿಸು;
	  ಪೂರ್ವನಿಯೋಜಿತ:
	    state = tran_unknown;
	    ನಿಲ್ಲಿಸು;
      }

      ಹಿಂತಿರುಗಿ state != old_state;
}

// The IEEE standard does not specify the behaviour when a tranif control
// input is 'x' or 'z'. We use the rules that are given for MOS switches.
ಸಾಲೊಳಗೆ vvp_vector8_t resolve_ambiguous(ಸ್ಥಿರಾಂಕ vvp_vector8_t&a,
                                       ಸ್ಥಿರಾಂಕ vvp_vector8_t&b,
                                       tran_state_t state,
                                       ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ str_map[8])
{
      ಪ್ರತಿಜ್ಞಾ(a.size() == b.size());
      vvp_vector8_t out (a.size());

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_scalar_t a_ಬಿಟ್ = a.value(idx);
	    vvp_scalar_t b_ಬಿಟ್ = b.value(idx);
            b_ಬಿಟ್ = vvp_scalar_t(b_ಬಿಟ್.value(),
                                 str_map[b_ಬಿಟ್.strength0()],
                                 str_map[b_ಬಿಟ್.strength1()]);
	    ಆದರೆ (state == tran_unknown) {
		  ಆಯ್ಕೆ (b_ಬಿಟ್.value()) {
		      ಸ್ಥಿತಿ BIT4_0:
			b_ಬಿಟ್ = vvp_scalar_t(BIT4_X, b_ಬಿಟ್.strength0(), 0);
			ನಿಲ್ಲಿಸು;
		      ಸ್ಥಿತಿ BIT4_1:
			b_ಬಿಟ್ = vvp_scalar_t(BIT4_X, 0, b_ಬಿಟ್.strength1());
			ನಿಲ್ಲಿಸು;
		      ಪೂರ್ವನಿಯೋಜಿತ:
			ನಿಲ್ಲಿಸು;
		  }
	    }
	    out.ಗಣ_ಬಿಟ್(idx, resolve(a_ಬಿಟ್, b_ಬಿಟ್));
      }
      ಹಿಂತಿರುಗಿ out;
}

ಸ್ಥಿರ ಶೂನ್ಯ ತಳ್ಳು_ಮೌಲ್ಯ_through_branches(ಸ್ಥಿರಾಂಕ vvp_vector8_t&val,
					ಸ್ಥಿರಾಂಕ list<vvp_branch_ptr_t>&connections);

ಸ್ಥಿರ ಶೂನ್ಯ ತಳ್ಳು_ಮೌಲ್ಯ_through_branch(ಸ್ಥಿರಾಂಕ vvp_vector8_t&val,
                                      vvp_branch_ptr_t cur)
{
      vvp_island_branch_tran*branch = BRANCH_TRAN(cur.ptr());

        // If the branch is disabled, skip.
      ಆದರೆ (branch->state == tran_disabled)
            ಹಿಂತಿರುಗಿ;

      ಚಿಹ್ನೆರಹಿತ src_ab = cur.ಪೋರ್ಟ್();
      ಚಿಹ್ನೆರಹಿತ dst_ab = src_ab^1;

      vvp_net_t*dst_net = dst_ab? branch->b : branch->a;
      vvp_island_ಪೋರ್ಟ್*dst_ಪೋರ್ಟ್ = dynamic_cast<vvp_island_ಪೋರ್ಟ್*>(dst_net->fun);

      vvp_vector8_t old_val = dst_ಪೋರ್ಟ್->value;

        // If the port on the other side has not yet been visited,
        // get its input value.
      ಆದರೆ (dst_ಪೋರ್ಟ್->value.size() == 0)
            dst_ಪೋರ್ಟ್->value = island_get_ಮೌಲ್ಯ(dst_net);

        // If we don't yet have an initial value for the port, skip.
      ಆದರೆ (dst_ಪೋರ್ಟ್->value.size() == 0)
            ಹಿಂತಿರುಗಿ;

        // Now resolve the pushed value with whatever values we have
        // previously collected (and resolved) for the port.
      ಆದರೆ (branch->width == 0) {
              // There are no part selects.
            dst_ಪೋರ್ಟ್->value = resolve_ambiguous(dst_ಪೋರ್ಟ್->value, val, branch->state,
                                                vvp_switch_strength_map[branch->resistive]);

      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dst_ab == 1) {
              // The other side is a strict subset (part select)
              // of this side.
            vvp_vector8_t tmp = val.subvalue(branch->ಸ್ಥಾನಾಂತರ_2, branch->part);
            dst_ಪೋರ್ಟ್->value = resolve(dst_ಪೋರ್ಟ್->value, tmp);

      } ಇಲ್ಲದಿದ್ದರೆ {
              // The other side is a superset of this side.
            vvp_vector8_t tmp = part_expand(val, branch->width, branch->ಸ್ಥಾನಾಂತರ_2);
            dst_ಪೋರ್ಟ್->value = resolve(dst_ಪೋರ್ಟ್->value, tmp);
      }

        // If the resolved value for the port has changed, push the new
        // value back into the network.
      ಆದರೆ (! dst_ಪೋರ್ಟ್->value.eeq(old_val)) {
            list<vvp_branch_ptr_t> connections;

	    vvp_branch_ptr_t dst_side(branch, dst_ab);
	    island_collect_ಗಂಟು(connections, dst_side);

	    ತಳ್ಳು_ಮೌಲ್ಯ_through_branches(dst_ಪೋರ್ಟ್->value, connections);
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ ತಳ್ಳು_ಮೌಲ್ಯ_through_branches(ಸ್ಥಿರಾಂಕ vvp_vector8_t&val,
					ಸ್ಥಿರಾಂಕ list<vvp_branch_ptr_t>&connections)
{
      ಗಾಗಿ (list<vvp_branch_ptr_t>::const_iterator idx = connections.cbegin()
		 ; idx != connections.cend() ; ++ idx ) {

            ತಳ್ಳು_ಮೌಲ್ಯ_through_branch(val, *idx);
      }
}

/*
 * This method resolves the value for a branch recursively. It uses
 * recursive descent to span the graph of branches, pushing values
 * through the network until a stable state is reached.
 */
ಶೂನ್ಯ vvp_island_branch_tran::ಸತತ_resolution()
{
      list<vvp_branch_ptr_t> connections;
      vvp_island_ಪೋರ್ಟ್*ಪೋರ್ಟ್;

	// If the A side port hasn't already been visited, then push
        // its input value through all the branches connected to it.
      ಪೋರ್ಟ್ = dynamic_cast<vvp_island_ಪೋರ್ಟ್*>(a->fun);
      ಆದರೆ (ಪೋರ್ಟ್->value.size() == 0) {
	    vvp_branch_ptr_t a_side(ಇದು, 0);
	    island_collect_ಗಂಟು(connections, a_side);

	    ಪೋರ್ಟ್->value = island_get_ಮೌಲ್ಯ(a);
            ಆದರೆ (ಪೋರ್ಟ್->value.size() != 0)
	          ತಳ್ಳು_ಮೌಲ್ಯ_through_branches(ಪೋರ್ಟ್->value, connections);

            connections.clear();
      }

	// Do the same for the B side port. Note that if the branch
        // is enabled, the B side port will have already been visited
        // when we resolved the A side port.
      ಪೋರ್ಟ್ = dynamic_cast<vvp_island_ಪೋರ್ಟ್*>(b->fun);
      ಆದರೆ (ಪೋರ್ಟ್->value.size() == 0) {
	    vvp_branch_ptr_t b_side(ಇದು, 1);
	    island_collect_ಗಂಟು(connections, b_side);

	    ಪೋರ್ಟ್->value = island_get_ಮೌಲ್ಯ(b);
	    ಆದರೆ (ಪೋರ್ಟ್->value.size() != 0)
	          ತಳ್ಳು_ಮೌಲ್ಯ_through_branches(ಪೋರ್ಟ್->value, connections);

            connections.clear();
      }
}

ಶೂನ್ಯ vvp_island_branch_tran::ಸತತ_output()
{
      vvp_island_ಪೋರ್ಟ್*ಪೋರ್ಟ್;

	// If the A side port hasn't already been updated, send the
        // resolved value to the output.
      ಪೋರ್ಟ್ = dynamic_cast<vvp_island_ಪೋರ್ಟ್*>(a->fun);
      ಆದರೆ (ಪೋರ್ಟ್->value.size() != 0) {
	    island_send_ಮೌಲ್ಯ(a, ಪೋರ್ಟ್->value);
	    ಪೋರ್ಟ್->value = vvp_vector8_t::nil;
      }

	// Do the same for the B side port.
      ಪೋರ್ಟ್ = dynamic_cast<vvp_island_ಪೋರ್ಟ್*>(b->fun);
      ಆದರೆ (ಪೋರ್ಟ್->value.size() != 0) {
	    island_send_ಮೌಲ್ಯ(b, ಪೋರ್ಟ್->value);
	    ಪೋರ್ಟ್->value = vvp_vector8_t::nil;
      }
}

ಶೂನ್ಯ compile_island_tran(ಅಕ್ಷರ*label)
{
      vvp_island*use_island = ಹೊಸ vvp_island_tran;
      compile_island_ತಳ(label, use_island);
}

ಶೂನ್ಯ compile_island_tranif(ಪೂರ್ಣಾಂಕ sense, ಅಕ್ಷರ*island, ಅಕ್ಷರ*pa, ಅಕ್ಷರ*pb, ಅಕ್ಷರ*pe,
                           ತಾರ್ಕಿಕ resistive)
{
      vvp_island*use_island = compile_ಕಂಡುಹಿಡಿ_island(island);
      ಪ್ರತಿಜ್ಞಾ(use_island);
      ಬಿಡುಗಡೆ(island);

      vvp_net_t*en = ಖಾಲಿ;

      ಆದರೆ (pe) {
	    en = use_island->ಕಂಡುಹಿಡಿ_ಪೋರ್ಟ್(pe);
	    ಪ್ರತಿಜ್ಞಾ(en);
	    ಬಿಡುಗಡೆ(pe);
      }

      vvp_island_branch_tran*br = ಹೊಸ vvp_island_branch_tran(en,
                                                             sense ? ನಿಜ :
                                                                     ಸುಳ್ಳು,
                                                             0, 0, 0, resistive);

      use_island->ಕೂಡಿಸು_branch(br, pa, pb);

      ಬಿಡುಗಡೆ(pa);
      ಬಿಡುಗಡೆ(pb);
}


ಶೂನ್ಯ compile_island_tranvp(ಅಕ್ಷರ*island, ಅಕ್ಷರ*pa, ಅಕ್ಷರ*pb,
			   ಚಿಹ್ನೆರಹಿತ wid, ಚಿಹ್ನೆರಹಿತ par, ಚಿಹ್ನೆರಹಿತ off)
{
      vvp_island*use_island = compile_ಕಂಡುಹಿಡಿ_island(island);
      ಪ್ರತಿಜ್ಞಾ(use_island);
      ಬಿಡುಗಡೆ(island);

      vvp_island_branch_tran*br = ಹೊಸ vvp_island_branch_tran(ಖಾಲಿ, ಸುಳ್ಳು,
                                                             wid, par, off, ಸುಳ್ಳು);

      use_island->ಕೂಡಿಸು_branch(br, pa, pb);

      ಬಿಡುಗಡೆ(pa);
      ಬಿಡುಗಡೆ(pb);
}
