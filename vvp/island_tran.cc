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

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

ਵਰਗ vvp_island_tran : ਜਨਤਕ vvp_island {

    ਜਨਤਕ:
      ਖਾਲੀ ਲੜੀਵਾਰ_island() override;
      ਖਾਲੀ ਗਿਣਤੀ_drivers(vvp_island_ਪੋਰਟ*ਪੋਰਟ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ_idx,
                         ਬਿਨਾਨਿਸ਼ਾਨੀ counts[3]) override;
};

ਗਿਣਤੀ tran_state_t {
      tran_disabled,
      tran_enabled,
      tran_unknown
};

ਬਣਤਰ vvp_island_branch_tran : ਜਨਤਕ vvp_island_branch {

      vvp_island_branch_tran(vvp_net_t*en__, ਤਾਰਕਿਕ ਸਰਗਰਮ_ਉੱਚਾ,
                             ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ_3, ਬਿਨਾਨਿਸ਼ਾਨੀ part__,
                             ਬਿਨਾਨਿਸ਼ਾਨੀ ਵਿੱਥ_3, ਤਾਰਕਿਕ resistive__);
      ਖਾਲੀ ਲੜੀਵਾਰ_test_enabled();
      ਤਾਰਕਿਕ rerun_test_enabled();
      ਖਾਲੀ ਲੜੀਵਾਰ_resolution();
      ਖਾਲੀ ਲੜੀਵਾਰ_output();

      vvp_net_t*en;
      ਬਿਨਾਨਿਸ਼ਾਨੀ width, part, ਵਿੱਥ_2;
      ਤਾਰਕਿਕ ਸਰਗਰਮ_ਉੱਚਾ_2, resistive;
      tran_state_t state;
};

vvp_island_branch_tran::vvp_island_branch_tran(vvp_net_t*en__,
                                               ਤਾਰਕਿਕ ਸਰਗਰਮ_ਉੱਚਾ,
                                               ਬਿਨਾਨਿਸ਼ਾਨੀ ਚੌੜਾਈ_3,
                                               ਬਿਨਾਨਿਸ਼ਾਨੀ part__,
                                               ਬਿਨਾਨਿਸ਼ਾਨੀ ਵਿੱਥ_3,
                                               ਤਾਰਕਿਕ resistive__)
: en(en__), width(ਚੌੜਾਈ_3), part(part__), ਵਿੱਥ_2(ਵਿੱਥ_3),
  ਸਰਗਰਮ_ਉੱਚਾ_2(ਸਰਗਰਮ_ਉੱਚਾ), resistive(resistive__)
{
      state = en__ ? tran_disabled : tran_enabled;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ vvp_island_branch_tran* BRANCH_TRAN(vvp_island_branch*tmp)
{
      vvp_island_branch_tran*res = dynamic_cast<vvp_island_branch_tran*>(tmp);
      ਪ੍ਰਤਿਜ੍ਞਾ(res);
      ਵਾਪਸ res;
}

/*
 * The run_island() method is called by the scheduler to run the
 * entire island. We run the island by calling run_resolution() for
 * all the branches in the island.
*/
ਖਾਲੀ vvp_island_tran::ਲੜੀਵਾਰ_island()
{
	// Test to see if any of the branches are enabled. This loop
	// tests the enabled inputs for all the branches and caches
	// the results in the state for each branch.
      ਲਈ (vvp_island_branch*cur = branches_ ; cur ; cur = cur->ਅਗਲਾ_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ਪ੍ਰਤਿਜ੍ਞਾ(tmp);
	    tmp->ਲੜੀਵਾਰ_test_enabled();
      }

rerun:
	// Now resolve all the branches in the island.
      ਲਈ (vvp_island_branch*cur = branches_ ; cur ; cur = cur->ਅਗਲਾ_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ਪ੍ਰਤਿਜ੍ਞਾ(tmp);
	    tmp->ਲੜੀਵਾਰ_resolution();
      }

	// Now output the resolved values.
      ਲਈ (vvp_island_branch*cur = branches_ ; cur ; cur = cur->ਅਗਲਾ_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ਪ੍ਰਤਿਜ੍ਞਾ(tmp);
	    tmp->ਲੜੀਵਾਰ_output();
      }

	// Now check if the enable inputs have been affected by the resolution.
      ਤਾਰਕਿਕ enable_ਬਦਲਿਆ = ਝੂਠ;
      ਲਈ (vvp_island_branch*cur = branches_ ; cur ; cur = cur->ਅਗਲਾ_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ਪ੍ਰਤਿਜ੍ਞਾ(tmp);
	    enable_ਬਦਲਿਆ |= tmp->rerun_test_enabled();
      }
      ਜੇ (enable_ਬਦਲਿਆ) ਜਾਓ rerun;
}

ਸਥਿਰ ਖਾਲੀ ਗਿਣਤੀ_drivers_2(vvp_branch_ptr_t cur, ਤਾਰਕਿਕ other_side_ਦੇਖਿਆ,
                           ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ_idx, ਬਿਨਾਨਿਸ਼ਾਨੀ counts[3])
{
        // First count any value driven into the port associated with
        // the current endpoint.
      vvp_net_t*net = cur.ਪੋਰਟ() ? cur.ptr()->b : cur.ptr()->a;
      vvp_scalar_t ਦੋਆਧਾਰੀ_ਅੰਕ = island_get_ਮੁੱਲ(net).value(ਦੋਆਧਾਰੀ_ਅੰਕ_idx);
      update_driver_counts(ਦੋਆਧਾਰੀ_ਅੰਕ.value(), counts);

        // Now handle all the branches attached to that port.
      vvp_branch_ptr_t idx = cur;
      ਕਰੋ {
            vvp_island_branch_tran*tmp = BRANCH_TRAN(idx.ptr());

              // If this branch represents a tran gate, we count the
              // value on the other side of the tran (providing it is
              // enabled) as a single driver.
            ਜੇ (tmp->width == 0) {
                  ਜੇ (tmp->state == tran_enabled) {
                        net = idx.ਪੋਰਟ() ? idx.ptr()->a : idx.ptr()->b;
                        ਦੋਆਧਾਰੀ_ਅੰਕ = island_get_ਭੇਜਿਆ_ਮੁੱਲ(net).value(ਦੋਆਧਾਰੀ_ਅੰਕ_idx);
                        update_driver_counts(ਦੋਆਧਾਰੀ_ਅੰਕ.value(), counts);
                  }
                  ਜਾਰੀ;
            }

              // If we get here, this branch is a part select. If we've
              // just come from the other end of the branch, we're done.
            ਜੇ ((idx == cur) && other_side_ਦੇਖਿਆ)
                  ਜਾਰੀ;

              // If this is the narrow end of the part select, the other
              // end must include the bit we are interested in. Follow
              // the branch to collect any drivers on the other side.
            ਜੇ (idx.ਪੋਰਟ() == 1) {
                  vvp_branch_ptr_t a_side(tmp, 0);
                  ਗਿਣਤੀ_drivers_2(a_side, ਸੱਚ, tmp->ਵਿੱਥ_2 + ਦੋਆਧਾਰੀ_ਅੰਕ_idx, counts);
                  ਜਾਰੀ;
            }

              // If we get here, this branch is the wide end of a part
              // select. If the bit we are interested in is within the
              // selected part, follow the branch to collect any drivers
              // on the other side.
            ਜੇ ((ਦੋਆਧਾਰੀ_ਅੰਕ_idx >= tmp->ਵਿੱਥ_2) && (ਦੋਆਧਾਰੀ_ਅੰਕ_idx < tmp->ਵਿੱਥ_2+tmp->part)) {
                  vvp_branch_ptr_t b_side(tmp, 1);
                  ਗਿਣਤੀ_drivers_2(b_side, ਸੱਚ, ਦੋਆਧਾਰੀ_ਅੰਕ_idx - tmp->ਵਿੱਥ_2, counts);
                  ਜਾਰੀ;
            }
      } ਜਦੋਂ ((idx = ਅਗਲਾ_2(idx)) != cur);
}

ਖਾਲੀ vvp_island_tran::ਗਿਣਤੀ_drivers(vvp_island_ਪੋਰਟ*ਪੋਰਟ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਦੋਆਧਾਰੀ_ਅੰਕ_idx,
                                    ਬਿਨਾਨਿਸ਼ਾਨੀ counts[3])
{
        // First we need to find a branch that is attached to the specified
        // port. Unfortunately there's no quick way to do this.
      vvp_island_branch*branch = branches_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ side = 0;
      ਜਦੋਂ (branch) {
            ਜੇ (branch->a->fun == ਪੋਰਟ) {
                  side = 0;
                  ਰੋਕੋ;
            }
            ਜੇ (branch->b->fun == ਪੋਰਟ) {
                  side = 1;
                  ਰੋਕੋ;
            }
            branch = branch->ਅਗਲਾ_branch;
      }
      ਪ੍ਰਤਿਜ੍ਞਾ(branch);

        // Now count the drivers, pushing through the network as necessary.
      vvp_branch_ptr_t endpoint(branch, side);
      ਗਿਣਤੀ_drivers_2(endpoint, ਝੂਠ, ਦੋਆਧਾਰੀ_ਅੰਕ_idx, counts);
}

ਖਾਲੀ vvp_island_branch_tran::ਲੜੀਵਾਰ_test_enabled()
{
      ਅਚਲ vvp_island_ਪੋਰਟ*ep = en? dynamic_cast<vvp_island_ਪੋਰਟ*> (en->fun) : ਨਿਰਾ;

	// If there is no ep port (no "enabled" input) then this is a
	// tran branch. Assume it is always enabled.
      ਜੇ (ep == 0) {
	    state = tran_enabled;
	    ਵਾਪਸ;
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
      ਜੇ (ep->outvalue.size() != 0)
	    enable_val = ep->outvalue.value(0).value();
      ਨਹੀਂਤਾਂ ਜੇ (ep->invalue.size() == 0)
	    enable_val = BIT4_Z;
      ਨਹੀਂਤਾਂ
	    enable_val = ep->invalue.value(0).value();

      ਚੋਣ (enable_val) {
	  ਹਾਲਤ BIT4_0:
	    state = ਸਰਗਰਮ_ਉੱਚਾ_2 ? tran_disabled : tran_enabled;
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_1:
	    state = ਸਰਗਰਮ_ਉੱਚਾ_2 ? tran_enabled : tran_disabled;
	    ਰੋਕੋ;
	  ਮੂਲ:
	    state = tran_unknown;
	    ਰੋਕੋ;
      }
}

ਤਾਰਕਿਕ vvp_island_branch_tran::rerun_test_enabled()
{
      ਅਚਲ vvp_island_ਪੋਰਟ*ep = en? dynamic_cast<vvp_island_ਪੋਰਟ*> (en->fun) : ਨਿਰਾ;

      ਜੇ (ep == 0)
	    ਵਾਪਸ ਝੂਠ;

	// We are only looking for changes resulting from running the island
	// resolution. If the outvalue is nil, we know that the enable port
	// is an .import, so won't be affected.
      ਜੇ (ep->outvalue.size() == 0)
	    ਵਾਪਸ ਝੂਠ;

      vvp_bit4_t enable_val = ep->outvalue.value(0).value();

      tran_state_t old_state = state;

      ਚੋਣ (enable_val) {
	  ਹਾਲਤ BIT4_0:
	    state = ਸਰਗਰਮ_ਉੱਚਾ_2 ? tran_disabled : tran_enabled;
	    ਰੋਕੋ;
	  ਹਾਲਤ BIT4_1:
	    state = ਸਰਗਰਮ_ਉੱਚਾ_2 ? tran_enabled : tran_disabled;
	    ਰੋਕੋ;
	  ਮੂਲ:
	    state = tran_unknown;
	    ਰੋਕੋ;
      }

      ਵਾਪਸ state != old_state;
}

// The IEEE standard does not specify the behaviour when a tranif control
// input is 'x' or 'z'. We use the rules that are given for MOS switches.
ਸਤਰਅੰਦਰ vvp_vector8_t resolve_ambiguous(ਅਚਲ vvp_vector8_t&a,
                                       ਅਚਲ vvp_vector8_t&b,
                                       tran_state_t state,
                                       ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ str_map[8])
{
      ਪ੍ਰਤਿਜ੍ਞਾ(a.size() == b.size());
      vvp_vector8_t out (a.size());

      ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_scalar_t a_ਦੋਆਧਾਰੀ_ਅੰਕ = a.value(idx);
	    vvp_scalar_t b_ਦੋਆਧਾਰੀ_ਅੰਕ = b.value(idx);
            b_ਦੋਆਧਾਰੀ_ਅੰਕ = vvp_scalar_t(b_ਦੋਆਧਾਰੀ_ਅੰਕ.value(),
                                 str_map[b_ਦੋਆਧਾਰੀ_ਅੰਕ.strength0()],
                                 str_map[b_ਦੋਆਧਾਰੀ_ਅੰਕ.strength1()]);
	    ਜੇ (state == tran_unknown) {
		  ਚੋਣ (b_ਦੋਆਧਾਰੀ_ਅੰਕ.value()) {
		      ਹਾਲਤ BIT4_0:
			b_ਦੋਆਧਾਰੀ_ਅੰਕ = vvp_scalar_t(BIT4_X, b_ਦੋਆਧਾਰੀ_ਅੰਕ.strength0(), 0);
			ਰੋਕੋ;
		      ਹਾਲਤ BIT4_1:
			b_ਦੋਆਧਾਰੀ_ਅੰਕ = vvp_scalar_t(BIT4_X, 0, b_ਦੋਆਧਾਰੀ_ਅੰਕ.strength1());
			ਰੋਕੋ;
		      ਮੂਲ:
			ਰੋਕੋ;
		  }
	    }
	    out.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(idx, resolve(a_ਦੋਆਧਾਰੀ_ਅੰਕ, b_ਦੋਆਧਾਰੀ_ਅੰਕ));
      }
      ਵਾਪਸ out;
}

ਸਥਿਰ ਖਾਲੀ ਧੱਕੋ_ਮੁੱਲ_through_branches(ਅਚਲ vvp_vector8_t&val,
					ਅਚਲ list<vvp_branch_ptr_t>&connections);

ਸਥਿਰ ਖਾਲੀ ਧੱਕੋ_ਮੁੱਲ_through_branch(ਅਚਲ vvp_vector8_t&val,
                                      vvp_branch_ptr_t cur)
{
      vvp_island_branch_tran*branch = BRANCH_TRAN(cur.ptr());

        // If the branch is disabled, skip.
      ਜੇ (branch->state == tran_disabled)
            ਵਾਪਸ;

      ਬਿਨਾਨਿਸ਼ਾਨੀ src_ab = cur.ਪੋਰਟ();
      ਬਿਨਾਨਿਸ਼ਾਨੀ dst_ab = src_ab^1;

      vvp_net_t*dst_net = dst_ab? branch->b : branch->a;
      vvp_island_ਪੋਰਟ*dst_ਪੋਰਟ = dynamic_cast<vvp_island_ਪੋਰਟ*>(dst_net->fun);

      vvp_vector8_t old_val = dst_ਪੋਰਟ->value;

        // If the port on the other side has not yet been visited,
        // get its input value.
      ਜੇ (dst_ਪੋਰਟ->value.size() == 0)
            dst_ਪੋਰਟ->value = island_get_ਮੁੱਲ(dst_net);

        // If we don't yet have an initial value for the port, skip.
      ਜੇ (dst_ਪੋਰਟ->value.size() == 0)
            ਵਾਪਸ;

        // Now resolve the pushed value with whatever values we have
        // previously collected (and resolved) for the port.
      ਜੇ (branch->width == 0) {
              // There are no part selects.
            dst_ਪੋਰਟ->value = resolve_ambiguous(dst_ਪੋਰਟ->value, val, branch->state,
                                                vvp_switch_strength_map[branch->resistive]);

      } ਨਹੀਂਤਾਂ ਜੇ (dst_ab == 1) {
              // The other side is a strict subset (part select)
              // of this side.
            vvp_vector8_t tmp = val.subvalue(branch->ਵਿੱਥ_2, branch->part);
            dst_ਪੋਰਟ->value = resolve(dst_ਪੋਰਟ->value, tmp);

      } ਨਹੀਂਤਾਂ {
              // The other side is a superset of this side.
            vvp_vector8_t tmp = part_expand(val, branch->width, branch->ਵਿੱਥ_2);
            dst_ਪੋਰਟ->value = resolve(dst_ਪੋਰਟ->value, tmp);
      }

        // If the resolved value for the port has changed, push the new
        // value back into the network.
      ਜੇ (! dst_ਪੋਰਟ->value.eeq(old_val)) {
            list<vvp_branch_ptr_t> connections;

	    vvp_branch_ptr_t dst_side(branch, dst_ab);
	    island_collect_ਗੰਢ(connections, dst_side);

	    ਧੱਕੋ_ਮੁੱਲ_through_branches(dst_ਪੋਰਟ->value, connections);
      }
}

ਸਥਿਰ ਖਾਲੀ ਧੱਕੋ_ਮੁੱਲ_through_branches(ਅਚਲ vvp_vector8_t&val,
					ਅਚਲ list<vvp_branch_ptr_t>&connections)
{
      ਲਈ (list<vvp_branch_ptr_t>::const_iterator idx = connections.cbegin()
		 ; idx != connections.cend() ; ++ idx ) {

            ਧੱਕੋ_ਮੁੱਲ_through_branch(val, *idx);
      }
}

/*
 * This method resolves the value for a branch recursively. It uses
 * recursive descent to span the graph of branches, pushing values
 * through the network until a stable state is reached.
 */
ਖਾਲੀ vvp_island_branch_tran::ਲੜੀਵਾਰ_resolution()
{
      list<vvp_branch_ptr_t> connections;
      vvp_island_ਪੋਰਟ*ਪੋਰਟ;

	// If the A side port hasn't already been visited, then push
        // its input value through all the branches connected to it.
      ਪੋਰਟ = dynamic_cast<vvp_island_ਪੋਰਟ*>(a->fun);
      ਜੇ (ਪੋਰਟ->value.size() == 0) {
	    vvp_branch_ptr_t a_side(ਇਹ, 0);
	    island_collect_ਗੰਢ(connections, a_side);

	    ਪੋਰਟ->value = island_get_ਮੁੱਲ(a);
            ਜੇ (ਪੋਰਟ->value.size() != 0)
	          ਧੱਕੋ_ਮੁੱਲ_through_branches(ਪੋਰਟ->value, connections);

            connections.clear();
      }

	// Do the same for the B side port. Note that if the branch
        // is enabled, the B side port will have already been visited
        // when we resolved the A side port.
      ਪੋਰਟ = dynamic_cast<vvp_island_ਪੋਰਟ*>(b->fun);
      ਜੇ (ਪੋਰਟ->value.size() == 0) {
	    vvp_branch_ptr_t b_side(ਇਹ, 1);
	    island_collect_ਗੰਢ(connections, b_side);

	    ਪੋਰਟ->value = island_get_ਮੁੱਲ(b);
	    ਜੇ (ਪੋਰਟ->value.size() != 0)
	          ਧੱਕੋ_ਮੁੱਲ_through_branches(ਪੋਰਟ->value, connections);

            connections.clear();
      }
}

ਖਾਲੀ vvp_island_branch_tran::ਲੜੀਵਾਰ_output()
{
      vvp_island_ਪੋਰਟ*ਪੋਰਟ;

	// If the A side port hasn't already been updated, send the
        // resolved value to the output.
      ਪੋਰਟ = dynamic_cast<vvp_island_ਪੋਰਟ*>(a->fun);
      ਜੇ (ਪੋਰਟ->value.size() != 0) {
	    island_send_ਮੁੱਲ(a, ਪੋਰਟ->value);
	    ਪੋਰਟ->value = vvp_vector8_t::nil;
      }

	// Do the same for the B side port.
      ਪੋਰਟ = dynamic_cast<vvp_island_ਪੋਰਟ*>(b->fun);
      ਜੇ (ਪੋਰਟ->value.size() != 0) {
	    island_send_ਮੁੱਲ(b, ਪੋਰਟ->value);
	    ਪੋਰਟ->value = vvp_vector8_t::nil;
      }
}

ਖਾਲੀ compile_island_tran(ਅੱਖਰ*label)
{
      vvp_island*use_island = ਨਵਾਂ vvp_island_tran;
      compile_island_ਅਧਾਰ(label, use_island);
}

ਖਾਲੀ compile_island_tranif(ਪੂਰਨ sense, ਅੱਖਰ*island, ਅੱਖਰ*pa, ਅੱਖਰ*pb, ਅੱਖਰ*pe,
                           ਤਾਰਕਿਕ resistive)
{
      vvp_island*use_island = compile_ਲੱਭੋ_island(island);
      ਪ੍ਰਤਿਜ੍ਞਾ(use_island);
      ਮੁਕਤ(island);

      vvp_net_t*en = ਨਿਰਾ;

      ਜੇ (pe) {
	    en = use_island->ਲੱਭੋ_ਪੋਰਟ(pe);
	    ਪ੍ਰਤਿਜ੍ਞਾ(en);
	    ਮੁਕਤ(pe);
      }

      vvp_island_branch_tran*br = ਨਵਾਂ vvp_island_branch_tran(en,
                                                             sense ? ਸੱਚ :
                                                                     ਝੂਠ,
                                                             0, 0, 0, resistive);

      use_island->ਜੋੜੋ_branch(br, pa, pb);

      ਮੁਕਤ(pa);
      ਮੁਕਤ(pb);
}


ਖਾਲੀ compile_island_tranvp(ਅੱਖਰ*island, ਅੱਖਰ*pa, ਅੱਖਰ*pb,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ wid, ਬਿਨਾਨਿਸ਼ਾਨੀ par, ਬਿਨਾਨਿਸ਼ਾਨੀ off)
{
      vvp_island*use_island = compile_ਲੱਭੋ_island(island);
      ਪ੍ਰਤਿਜ੍ਞਾ(use_island);
      ਮੁਕਤ(island);

      vvp_island_branch_tran*br = ਨਵਾਂ vvp_island_branch_tran(ਨਿਰਾ, ਝੂਠ,
                                                             wid, par, off, ਝੂਠ);

      use_island->ਜੋੜੋ_branch(br, pa, pb);

      ਮੁਕਤ(pa);
      ਮੁਕਤ(pb);
}
