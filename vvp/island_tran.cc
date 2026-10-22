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

using ನಾಮಾಕಾಶ std;

ವರ್ಗ vvp_island_tran : ಸಾರ್ವಜನಿಕ vvp_island {

    ಸಾರ್ವಜನಿಕ:
      ಶೂನ್ಯ run_island() override;
      ಶೂನ್ಯ ಎಣಿಕೆ_drivers(vvp_island_port*port, unsigned bit_idx,
                         unsigned counts[3]) override;
};

ಗಣನಾ tran_state_t {
      tran_disabled,
      tran_enabled,
      tran_unknown
};

ಸಂರಚನಾ vvp_island_branch_tran : ಸಾರ್ವಜನಿಕ vvp_island_branch {

      vvp_island_branch_tran(vvp_net_t*en__, ತಾರ್ಕಿಕ active_ಮೇಲಿನ,
                             unsigned width__, unsigned part__,
                             unsigned offset__, ತಾರ್ಕಿಕ resistive__);
      ಶೂನ್ಯ run_test_enabled();
      ತಾರ್ಕಿಕ rerun_test_enabled();
      ಶೂನ್ಯ run_resolution();
      ಶೂನ್ಯ run_output();

      vvp_net_t*en;
      unsigned width, part, offset;
      ತಾರ್ಕಿಕ active_ಮೇಲಿನ_2, resistive;
      tran_state_t state;
};

vvp_island_branch_tran::vvp_island_branch_tran(vvp_net_t*en__,
                                               ತಾರ್ಕಿಕ active_ಮೇಲಿನ,
                                               unsigned width__,
                                               unsigned part__,
                                               unsigned offset__,
                                               ತಾರ್ಕಿಕ resistive__)
: en(en__), width(width__), part(part__), offset(offset__),
  active_ಮೇಲಿನ_2(active_ಮೇಲಿನ), resistive(resistive__)
{
      state = en__ ? tran_disabled : tran_enabled;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ vvp_island_branch_tran* BRANCH_TRAN(vvp_island_branch*tmp)
{
      vvp_island_branch_tran*res = dynamic_cast<vvp_island_branch_tran*>(tmp);
      ಪ್ರತಿಜ್ಞಾ(res);
      ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * The run_island() method is called by the scheduler to run the
 * entire island. We run the island by calling run_resolution() for
 * all the branches in the island.
*/
ಶೂನ್ಯ vvp_island_tran::run_island()
{
	// Test to see if any of the branches are enabled. This loop
	// tests the enabled inputs for all the branches and caches
	// the results in the state for each branch.
      ಪ್ರತಿ (vvp_island_branch*cur = branches_ ; cur ; cur = cur->next_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ಪ್ರತಿಜ್ಞಾ(tmp);
	    tmp->run_test_enabled();
      }

rerun:
	// Now resolve all the branches in the island.
      ಪ್ರತಿ (vvp_island_branch*cur = branches_ ; cur ; cur = cur->next_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ಪ್ರತಿಜ್ಞಾ(tmp);
	    tmp->run_resolution();
      }

	// Now output the resolved values.
      ಪ್ರತಿ (vvp_island_branch*cur = branches_ ; cur ; cur = cur->next_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ಪ್ರತಿಜ್ಞಾ(tmp);
	    tmp->run_output();
      }

	// Now check if the enable inputs have been affected by the resolution.
      ತಾರ್ಕಿಕ enable_changed = ಅಸತ್ಯ;
      ಪ್ರತಿ (vvp_island_branch*cur = branches_ ; cur ; cur = cur->next_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ಪ್ರತಿಜ್ಞಾ(tmp);
	    enable_changed |= tmp->rerun_test_enabled();
      }
      ಯದಿ (enable_changed) ಗಚ್ಛ rerun;
}

ಸ್ಥಿರ ಶೂನ್ಯ ಎಣಿಕೆ_drivers_2(vvp_branch_ptr_t cur, ತಾರ್ಕಿಕ other_side_visited,
                           unsigned bit_idx, unsigned counts[3])
{
        // First count any value driven into the port associated with
        // the current endpoint.
      vvp_net_t*net = cur.port() ? cur.ptr()->b : cur.ptr()->a;
      vvp_scalar_t bit = island_get_ಮೌಲ್ಯ(net).value(bit_idx);
      update_driver_counts(bit.value(), counts);

        // Now handle all the branches attached to that port.
      vvp_branch_ptr_t idx = cur;
      ಕುರು {
            vvp_island_branch_tran*tmp = BRANCH_TRAN(idx.ptr());

              // If this branch represents a tran gate, we count the
              // value on the other side of the tran (providing it is
              // enabled) as a single driver.
            ಯದಿ (tmp->width == 0) {
                  ಯದಿ (tmp->state == tran_enabled) {
                        net = idx.port() ? idx.ptr()->a : idx.ptr()->b;
                        bit = island_get_sent_ಮೌಲ್ಯ(net).value(bit_idx);
                        update_driver_counts(bit.value(), counts);
                  }
                  ಅನುವರ್ತ;
            }

              // If we get here, this branch is a part select. If we've
              // just come from the other end of the branch, we're done.
            ಯದಿ ((idx == cur) && other_side_visited)
                  ಅನುವರ್ತ;

              // If this is the narrow end of the part select, the other
              // end must include the bit we are interested in. Follow
              // the branch to collect any drivers on the other side.
            ಯದಿ (idx.port() == 1) {
                  vvp_branch_ptr_t a_side(tmp, 0);
                  ಎಣಿಕೆ_drivers_2(a_side, ಸತ್ಯ, tmp->offset + bit_idx, counts);
                  ಅನುವರ್ತ;
            }

              // If we get here, this branch is the wide end of a part
              // select. If the bit we are interested in is within the
              // selected part, follow the branch to collect any drivers
              // on the other side.
            ಯದಿ ((bit_idx >= tmp->offset) && (bit_idx < tmp->offset+tmp->part)) {
                  vvp_branch_ptr_t b_side(tmp, 1);
                  ಎಣಿಕೆ_drivers_2(b_side, ಸತ್ಯ, bit_idx - tmp->offset, counts);
                  ಅನುವರ್ತ;
            }
      } ಯಾವತ್ ((idx = next(idx)) != cur);
}

ಶೂನ್ಯ vvp_island_tran::ಎಣಿಕೆ_drivers(vvp_island_port*port, unsigned bit_idx,
                                    unsigned counts[3])
{
        // First we need to find a branch that is attached to the specified
        // port. Unfortunately there's no quick way to do this.
      vvp_island_branch*branch = branches_;
      unsigned side = 0;
      ಯಾವತ್ (branch) {
            ಯದಿ (branch->a->fun == port) {
                  side = 0;
                  ಭಙ್ಗ;
            }
            ಯದಿ (branch->b->fun == port) {
                  side = 1;
                  ಭಙ್ಗ;
            }
            branch = branch->next_branch;
      }
      ಪ್ರತಿಜ್ಞಾ(branch);

        // Now count the drivers, pushing through the network as necessary.
      vvp_branch_ptr_t endpoint(branch, side);
      ಎಣಿಕೆ_drivers_2(endpoint, ಅಸತ್ಯ, bit_idx, counts);
}

ಶೂನ್ಯ vvp_island_branch_tran::run_test_enabled()
{
      ಧ್ರುವ vvp_island_port*ep = en? dynamic_cast<vvp_island_port*> (en->fun) : ರಿಕ್ತ;

	// If there is no ep port (no "enabled" input) then this is a
	// tran branch. Assume it is always enabled.
      ಯದಿ (ep == 0) {
	    state = tran_enabled;
	    ಪ್ರತ್ಯಾಗಮ;
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
      ಯದಿ (ep->outvalue.size() != 0)
	    enable_val = ep->outvalue.value(0).value();
      ಅನ್ಯಥಾ ಯದಿ (ep->invalue.size() == 0)
	    enable_val = BIT4_Z;
      ಅನ್ಯಥಾ
	    enable_val = ep->invalue.value(0).value();

      ವಿಕಲ್ಪ (enable_val) {
	  ಪಕ್ಷ BIT4_0:
	    state = active_ಮೇಲಿನ_2 ? tran_disabled : tran_enabled;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ BIT4_1:
	    state = active_ಮೇಲಿನ_2 ? tran_enabled : tran_disabled;
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    state = tran_unknown;
	    ಭಙ್ಗ;
      }
}

ತಾರ್ಕಿಕ vvp_island_branch_tran::rerun_test_enabled()
{
      ಧ್ರುವ vvp_island_port*ep = en? dynamic_cast<vvp_island_port*> (en->fun) : ರಿಕ್ತ;

      ಯದಿ (ep == 0)
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	// We are only looking for changes resulting from running the island
	// resolution. If the outvalue is nil, we know that the enable port
	// is an .import, so won't be affected.
      ಯದಿ (ep->outvalue.size() == 0)
	    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

      vvp_bit4_t enable_val = ep->outvalue.value(0).value();

      tran_state_t old_state = state;

      ವಿಕಲ್ಪ (enable_val) {
	  ಪಕ್ಷ BIT4_0:
	    state = active_ಮೇಲಿನ_2 ? tran_disabled : tran_enabled;
	    ಭಙ್ಗ;
	  ಪಕ್ಷ BIT4_1:
	    state = active_ಮೇಲಿನ_2 ? tran_enabled : tran_disabled;
	    ಭಙ್ಗ;
	  ಸ್ವಭಾವ:
	    state = tran_unknown;
	    ಭಙ್ಗ;
      }

      ಪ್ರತ್ಯಾಗಮ state != old_state;
}

// The IEEE standard does not specify the behaviour when a tranif control
// input is 'x' or 'z'. We use the rules that are given for MOS switches.
ಅನ್ತಃಸ್ಥ vvp_vector8_t resolve_ambiguous(ಧ್ರುವ vvp_vector8_t&a,
                                       ಧ್ರುವ vvp_vector8_t&b,
                                       tran_state_t state,
                                       ಧ್ರುವ unsigned str_map[8])
{
      ಪ್ರತಿಜ್ಞಾ(a.size() == b.size());
      vvp_vector8_t out (a.size());

      ಪ್ರತಿ (unsigned idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_scalar_t a_bit = a.value(idx);
	    vvp_scalar_t b_bit = b.value(idx);
            b_bit = vvp_scalar_t(b_bit.value(),
                                 str_map[b_bit.strength0()],
                                 str_map[b_bit.strength1()]);
	    ಯದಿ (state == tran_unknown) {
		  ವಿಕಲ್ಪ (b_bit.value()) {
		      ಪಕ್ಷ BIT4_0:
			b_bit = vvp_scalar_t(BIT4_X, b_bit.strength0(), 0);
			ಭಙ್ಗ;
		      ಪಕ್ಷ BIT4_1:
			b_bit = vvp_scalar_t(BIT4_X, 0, b_bit.strength1());
			ಭಙ್ಗ;
		      ಸ್ವಭಾವ:
			ಭಙ್ಗ;
		  }
	    }
	    out.set_bit(idx, resolve(a_bit, b_bit));
      }
      ಪ್ರತ್ಯಾಗಮ out;
}

ಸ್ಥಿರ ಶೂನ್ಯ ತಳ್ಳು_ಮೌಲ್ಯ_through_branches(ಧ್ರುವ vvp_vector8_t&val,
					ಧ್ರುವ list<vvp_branch_ptr_t>&connections);

ಸ್ಥಿರ ಶೂನ್ಯ ತಳ್ಳು_ಮೌಲ್ಯ_through_branch(ಧ್ರುವ vvp_vector8_t&val,
                                      vvp_branch_ptr_t cur)
{
      vvp_island_branch_tran*branch = BRANCH_TRAN(cur.ptr());

        // If the branch is disabled, skip.
      ಯದಿ (branch->state == tran_disabled)
            ಪ್ರತ್ಯಾಗಮ;

      unsigned src_ab = cur.port();
      unsigned dst_ab = src_ab^1;

      vvp_net_t*dst_net = dst_ab? branch->b : branch->a;
      vvp_island_port*dst_port = dynamic_cast<vvp_island_port*>(dst_net->fun);

      vvp_vector8_t old_val = dst_port->value;

        // If the port on the other side has not yet been visited,
        // get its input value.
      ಯದಿ (dst_port->value.size() == 0)
            dst_port->value = island_get_ಮೌಲ್ಯ(dst_net);

        // If we don't yet have an initial value for the port, skip.
      ಯದಿ (dst_port->value.size() == 0)
            ಪ್ರತ್ಯಾಗಮ;

        // Now resolve the pushed value with whatever values we have
        // previously collected (and resolved) for the port.
      ಯದಿ (branch->width == 0) {
              // There are no part selects.
            dst_port->value = resolve_ambiguous(dst_port->value, val, branch->state,
                                                vvp_switch_strength_map[branch->resistive]);

      } ಅನ್ಯಥಾ ಯದಿ (dst_ab == 1) {
              // The other side is a strict subset (part select)
              // of this side.
            vvp_vector8_t tmp = val.subvalue(branch->offset, branch->part);
            dst_port->value = resolve(dst_port->value, tmp);

      } ಅನ್ಯಥಾ {
              // The other side is a superset of this side.
            vvp_vector8_t tmp = part_expand(val, branch->width, branch->offset);
            dst_port->value = resolve(dst_port->value, tmp);
      }

        // If the resolved value for the port has changed, push the new
        // value back into the network.
      ಯದಿ (! dst_port->value.eeq(old_val)) {
            list<vvp_branch_ptr_t> connections;

	    vvp_branch_ptr_t dst_side(branch, dst_ab);
	    island_collect_ಗಂಟು(connections, dst_side);

	    ತಳ್ಳು_ಮೌಲ್ಯ_through_branches(dst_port->value, connections);
      }
}

ಸ್ಥಿರ ಶೂನ್ಯ ತಳ್ಳು_ಮೌಲ್ಯ_through_branches(ಧ್ರುವ vvp_vector8_t&val,
					ಧ್ರುವ list<vvp_branch_ptr_t>&connections)
{
      ಪ್ರತಿ (list<vvp_branch_ptr_t>::const_iterator idx = connections.cbegin()
		 ; idx != connections.cend() ; ++ idx ) {

            ತಳ್ಳು_ಮೌಲ್ಯ_through_branch(val, *idx);
      }
}

/*
 * This method resolves the value for a branch recursively. It uses
 * recursive descent to span the graph of branches, pushing values
 * through the network until a stable state is reached.
 */
ಶೂನ್ಯ vvp_island_branch_tran::run_resolution()
{
      list<vvp_branch_ptr_t> connections;
      vvp_island_port*port;

	// If the A side port hasn't already been visited, then push
        // its input value through all the branches connected to it.
      port = dynamic_cast<vvp_island_port*>(a->fun);
      ಯದಿ (port->value.size() == 0) {
	    vvp_branch_ptr_t a_side(ಏತತ್, 0);
	    island_collect_ಗಂಟು(connections, a_side);

	    port->value = island_get_ಮೌಲ್ಯ(a);
            ಯದಿ (port->value.size() != 0)
	          ತಳ್ಳು_ಮೌಲ್ಯ_through_branches(port->value, connections);

            connections.clear();
      }

	// Do the same for the B side port. Note that if the branch
        // is enabled, the B side port will have already been visited
        // when we resolved the A side port.
      port = dynamic_cast<vvp_island_port*>(b->fun);
      ಯದಿ (port->value.size() == 0) {
	    vvp_branch_ptr_t b_side(ಏತತ್, 1);
	    island_collect_ಗಂಟು(connections, b_side);

	    port->value = island_get_ಮೌಲ್ಯ(b);
	    ಯದಿ (port->value.size() != 0)
	          ತಳ್ಳು_ಮೌಲ್ಯ_through_branches(port->value, connections);

            connections.clear();
      }
}

ಶೂನ್ಯ vvp_island_branch_tran::run_output()
{
      vvp_island_port*port;

	// If the A side port hasn't already been updated, send the
        // resolved value to the output.
      port = dynamic_cast<vvp_island_port*>(a->fun);
      ಯದಿ (port->value.size() != 0) {
	    island_send_ಮೌಲ್ಯ(a, port->value);
	    port->value = vvp_vector8_t::nil;
      }

	// Do the same for the B side port.
      port = dynamic_cast<vvp_island_port*>(b->fun);
      ಯದಿ (port->value.size() != 0) {
	    island_send_ಮೌಲ್ಯ(b, port->value);
	    port->value = vvp_vector8_t::nil;
      }
}

ಶೂನ್ಯ compile_island_tran(ವರ್ಣ*label)
{
      vvp_island*use_island = ನೂತನ vvp_island_tran;
      compile_island_base(label, use_island);
}

ಶೂನ್ಯ compile_island_tranif(ಪೂರ್ಣಾಙ್ಕ sense, ವರ್ಣ*island, ವರ್ಣ*pa, ವರ್ಣ*pb, ವರ್ಣ*pe,
                           ತಾರ್ಕಿಕ resistive)
{
      vvp_island*use_island = compile_find_island(island);
      ಪ್ರತಿಜ್ಞಾ(use_island);
      ಮುಕ್ತ(island);

      vvp_net_t*en = ರಿಕ್ತ;

      ಯದಿ (pe) {
	    en = use_island->find_port(pe);
	    ಪ್ರತಿಜ್ಞಾ(en);
	    ಮುಕ್ತ(pe);
      }

      vvp_island_branch_tran*br = ನೂತನ vvp_island_branch_tran(en,
                                                             sense ? ಸತ್ಯ :
                                                                     ಅಸತ್ಯ,
                                                             0, 0, 0, resistive);

      use_island->add_branch(br, pa, pb);

      ಮುಕ್ತ(pa);
      ಮುಕ್ತ(pb);
}


ಶೂನ್ಯ compile_island_tranvp(ವರ್ಣ*island, ವರ್ಣ*pa, ವರ್ಣ*pb,
			   unsigned wid, unsigned par, unsigned off)
{
      vvp_island*use_island = compile_find_island(island);
      ಪ್ರತಿಜ್ಞಾ(use_island);
      ಮುಕ್ತ(island);

      vvp_island_branch_tran*br = ನೂತನ vvp_island_branch_tran(ರಿಕ್ತ, ಅಸತ್ಯ,
                                                             wid, par, off, ಅಸತ್ಯ);

      use_island->add_branch(br, pa, pb);

      ಮುಕ್ತ(pa);
      ಮುಕ್ತ(pb);
}
