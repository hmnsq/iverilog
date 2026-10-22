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

using நாமாகாஶ std;

வகுப்பு vvp_island_tran : பொது vvp_island {

    பொது:
      வெறுமை run_island() override;
      வெறுமை மொத்தம்_drivers(vvp_island_port*port, unsigned bit_idx,
                         unsigned counts[3]) override;
};

எண்ணிக்கை tran_state_t {
      tran_disabled,
      tran_enabled,
      tran_unknown
};

கட்டமைப்பு vvp_island_branch_tran : பொது vvp_island_branch {

      vvp_island_branch_tran(vvp_net_t*en__, தருக்க active_உயர்வு,
                             unsigned width__, unsigned part__,
                             unsigned offset__, தருக்க resistive__);
      வெறுமை run_test_enabled();
      தருக்க rerun_test_enabled();
      வெறுமை run_resolution();
      வெறுமை run_output();

      vvp_net_t*en;
      unsigned width, part, offset;
      தருக்க active_உயர்வு_2, resistive;
      tran_state_t state;
};

vvp_island_branch_tran::vvp_island_branch_tran(vvp_net_t*en__,
                                               தருக்க active_உயர்வு,
                                               unsigned width__,
                                               unsigned part__,
                                               unsigned offset__,
                                               தருக்க resistive__)
: en(en__), width(width__), part(part__), offset(offset__),
  active_உயர்வு_2(active_உயர்வு), resistive(resistive__)
{
      state = en__ ? tran_disabled : tran_enabled;
}

நிலையான உள்வரி vvp_island_branch_tran* BRANCH_TRAN(vvp_island_branch*tmp)
{
      vvp_island_branch_tran*res = dynamic_cast<vvp_island_branch_tran*>(tmp);
      ப்ரதிஜ்ஞா(res);
      திரும்பு res;
}

/*
 * The run_island() method is called by the scheduler to run the
 * entire island. We run the island by calling run_resolution() for
 * all the branches in the island.
*/
வெறுமை vvp_island_tran::run_island()
{
	// Test to see if any of the branches are enabled. This loop
	// tests the enabled inputs for all the branches and caches
	// the results in the state for each branch.
      க்கு (vvp_island_branch*cur = branches_ ; cur ; cur = cur->next_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ப்ரதிஜ்ஞா(tmp);
	    tmp->run_test_enabled();
      }

rerun:
	// Now resolve all the branches in the island.
      க்கு (vvp_island_branch*cur = branches_ ; cur ; cur = cur->next_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ப்ரதிஜ்ஞா(tmp);
	    tmp->run_resolution();
      }

	// Now output the resolved values.
      க்கு (vvp_island_branch*cur = branches_ ; cur ; cur = cur->next_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ப்ரதிஜ்ஞா(tmp);
	    tmp->run_output();
      }

	// Now check if the enable inputs have been affected by the resolution.
      தருக்க enable_changed = பொய்;
      க்கு (vvp_island_branch*cur = branches_ ; cur ; cur = cur->next_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ப்ரதிஜ்ஞா(tmp);
	    enable_changed |= tmp->rerun_test_enabled();
      }
      எனில் (enable_changed) செல் rerun;
}

நிலையான வெறுமை மொத்தம்_drivers_2(vvp_branch_ptr_t cur, தருக்க other_side_visited,
                           unsigned bit_idx, unsigned counts[3])
{
        // First count any value driven into the port associated with
        // the current endpoint.
      vvp_net_t*net = cur.port() ? cur.ptr()->b : cur.ptr()->a;
      vvp_scalar_t bit = island_get_மதிப்பு(net).value(bit_idx);
      update_driver_counts(bit.value(), counts);

        // Now handle all the branches attached to that port.
      vvp_branch_ptr_t idx = cur;
      செய் {
            vvp_island_branch_tran*tmp = BRANCH_TRAN(idx.ptr());

              // If this branch represents a tran gate, we count the
              // value on the other side of the tran (providing it is
              // enabled) as a single driver.
            எனில் (tmp->width == 0) {
                  எனில் (tmp->state == tran_enabled) {
                        net = idx.port() ? idx.ptr()->a : idx.ptr()->b;
                        bit = island_get_sent_மதிப்பு(net).value(bit_idx);
                        update_driver_counts(bit.value(), counts);
                  }
                  தொடர்;
            }

              // If we get here, this branch is a part select. If we've
              // just come from the other end of the branch, we're done.
            எனில் ((idx == cur) && other_side_visited)
                  தொடர்;

              // If this is the narrow end of the part select, the other
              // end must include the bit we are interested in. Follow
              // the branch to collect any drivers on the other side.
            எனில் (idx.port() == 1) {
                  vvp_branch_ptr_t a_side(tmp, 0);
                  மொத்தம்_drivers_2(a_side, உண்மை, tmp->offset + bit_idx, counts);
                  தொடர்;
            }

              // If we get here, this branch is the wide end of a part
              // select. If the bit we are interested in is within the
              // selected part, follow the branch to collect any drivers
              // on the other side.
            எனில் ((bit_idx >= tmp->offset) && (bit_idx < tmp->offset+tmp->part)) {
                  vvp_branch_ptr_t b_side(tmp, 1);
                  மொத்தம்_drivers_2(b_side, உண்மை, bit_idx - tmp->offset, counts);
                  தொடர்;
            }
      } போது ((idx = next(idx)) != cur);
}

வெறுமை vvp_island_tran::மொத்தம்_drivers(vvp_island_port*port, unsigned bit_idx,
                                    unsigned counts[3])
{
        // First we need to find a branch that is attached to the specified
        // port. Unfortunately there's no quick way to do this.
      vvp_island_branch*branch = branches_;
      unsigned side = 0;
      போது (branch) {
            எனில் (branch->a->fun == port) {
                  side = 0;
                  நிறுத்து;
            }
            எனில் (branch->b->fun == port) {
                  side = 1;
                  நிறுத்து;
            }
            branch = branch->next_branch;
      }
      ப்ரதிஜ்ஞா(branch);

        // Now count the drivers, pushing through the network as necessary.
      vvp_branch_ptr_t endpoint(branch, side);
      மொத்தம்_drivers_2(endpoint, பொய், bit_idx, counts);
}

வெறுமை vvp_island_branch_tran::run_test_enabled()
{
      மாறிலி vvp_island_port*ep = en? dynamic_cast<vvp_island_port*> (en->fun) : வெற்று;

	// If there is no ep port (no "enabled" input) then this is a
	// tran branch. Assume it is always enabled.
      எனில் (ep == 0) {
	    state = tran_enabled;
	    திரும்பு;
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
      எனில் (ep->outvalue.size() != 0)
	    enable_val = ep->outvalue.value(0).value();
      இல்லையெனில் எனில் (ep->invalue.size() == 0)
	    enable_val = BIT4_Z;
      இல்லையெனில்
	    enable_val = ep->invalue.value(0).value();

      தேர்வு (enable_val) {
	  நிலை BIT4_0:
	    state = active_உயர்வு_2 ? tran_disabled : tran_enabled;
	    நிறுத்து;
	  நிலை BIT4_1:
	    state = active_உயர்வு_2 ? tran_enabled : tran_disabled;
	    நிறுத்து;
	  இயல்பு:
	    state = tran_unknown;
	    நிறுத்து;
      }
}

தருக்க vvp_island_branch_tran::rerun_test_enabled()
{
      மாறிலி vvp_island_port*ep = en? dynamic_cast<vvp_island_port*> (en->fun) : வெற்று;

      எனில் (ep == 0)
	    திரும்பு பொய்;

	// We are only looking for changes resulting from running the island
	// resolution. If the outvalue is nil, we know that the enable port
	// is an .import, so won't be affected.
      எனில் (ep->outvalue.size() == 0)
	    திரும்பு பொய்;

      vvp_bit4_t enable_val = ep->outvalue.value(0).value();

      tran_state_t old_state = state;

      தேர்வு (enable_val) {
	  நிலை BIT4_0:
	    state = active_உயர்வு_2 ? tran_disabled : tran_enabled;
	    நிறுத்து;
	  நிலை BIT4_1:
	    state = active_உயர்வு_2 ? tran_enabled : tran_disabled;
	    நிறுத்து;
	  இயல்பு:
	    state = tran_unknown;
	    நிறுத்து;
      }

      திரும்பு state != old_state;
}

// The IEEE standard does not specify the behaviour when a tranif control
// input is 'x' or 'z'. We use the rules that are given for MOS switches.
உள்வரி vvp_vector8_t resolve_ambiguous(மாறிலி vvp_vector8_t&a,
                                       மாறிலி vvp_vector8_t&b,
                                       tran_state_t state,
                                       மாறிலி unsigned str_map[8])
{
      ப்ரதிஜ்ஞா(a.size() == b.size());
      vvp_vector8_t out (a.size());

      க்கு (unsigned idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_scalar_t a_bit = a.value(idx);
	    vvp_scalar_t b_bit = b.value(idx);
            b_bit = vvp_scalar_t(b_bit.value(),
                                 str_map[b_bit.strength0()],
                                 str_map[b_bit.strength1()]);
	    எனில் (state == tran_unknown) {
		  தேர்வு (b_bit.value()) {
		      நிலை BIT4_0:
			b_bit = vvp_scalar_t(BIT4_X, b_bit.strength0(), 0);
			நிறுத்து;
		      நிலை BIT4_1:
			b_bit = vvp_scalar_t(BIT4_X, 0, b_bit.strength1());
			நிறுத்து;
		      இயல்பு:
			நிறுத்து;
		  }
	    }
	    out.set_bit(idx, resolve(a_bit, b_bit));
      }
      திரும்பு out;
}

நிலையான வெறுமை தள்ளு_மதிப்பு_through_branches(மாறிலி vvp_vector8_t&val,
					மாறிலி list<vvp_branch_ptr_t>&connections);

நிலையான வெறுமை தள்ளு_மதிப்பு_through_branch(மாறிலி vvp_vector8_t&val,
                                      vvp_branch_ptr_t cur)
{
      vvp_island_branch_tran*branch = BRANCH_TRAN(cur.ptr());

        // If the branch is disabled, skip.
      எனில் (branch->state == tran_disabled)
            திரும்பு;

      unsigned src_ab = cur.port();
      unsigned dst_ab = src_ab^1;

      vvp_net_t*dst_net = dst_ab? branch->b : branch->a;
      vvp_island_port*dst_port = dynamic_cast<vvp_island_port*>(dst_net->fun);

      vvp_vector8_t old_val = dst_port->value;

        // If the port on the other side has not yet been visited,
        // get its input value.
      எனில் (dst_port->value.size() == 0)
            dst_port->value = island_get_மதிப்பு(dst_net);

        // If we don't yet have an initial value for the port, skip.
      எனில் (dst_port->value.size() == 0)
            திரும்பு;

        // Now resolve the pushed value with whatever values we have
        // previously collected (and resolved) for the port.
      எனில் (branch->width == 0) {
              // There are no part selects.
            dst_port->value = resolve_ambiguous(dst_port->value, val, branch->state,
                                                vvp_switch_strength_map[branch->resistive]);

      } இல்லையெனில் எனில் (dst_ab == 1) {
              // The other side is a strict subset (part select)
              // of this side.
            vvp_vector8_t tmp = val.subvalue(branch->offset, branch->part);
            dst_port->value = resolve(dst_port->value, tmp);

      } இல்லையெனில் {
              // The other side is a superset of this side.
            vvp_vector8_t tmp = part_expand(val, branch->width, branch->offset);
            dst_port->value = resolve(dst_port->value, tmp);
      }

        // If the resolved value for the port has changed, push the new
        // value back into the network.
      எனில் (! dst_port->value.eeq(old_val)) {
            list<vvp_branch_ptr_t> connections;

	    vvp_branch_ptr_t dst_side(branch, dst_ab);
	    island_collect_முனை(connections, dst_side);

	    தள்ளு_மதிப்பு_through_branches(dst_port->value, connections);
      }
}

நிலையான வெறுமை தள்ளு_மதிப்பு_through_branches(மாறிலி vvp_vector8_t&val,
					மாறிலி list<vvp_branch_ptr_t>&connections)
{
      க்கு (list<vvp_branch_ptr_t>::const_iterator idx = connections.cbegin()
		 ; idx != connections.cend() ; ++ idx ) {

            தள்ளு_மதிப்பு_through_branch(val, *idx);
      }
}

/*
 * This method resolves the value for a branch recursively. It uses
 * recursive descent to span the graph of branches, pushing values
 * through the network until a stable state is reached.
 */
வெறுமை vvp_island_branch_tran::run_resolution()
{
      list<vvp_branch_ptr_t> connections;
      vvp_island_port*port;

	// If the A side port hasn't already been visited, then push
        // its input value through all the branches connected to it.
      port = dynamic_cast<vvp_island_port*>(a->fun);
      எனில் (port->value.size() == 0) {
	    vvp_branch_ptr_t a_side(இது, 0);
	    island_collect_முனை(connections, a_side);

	    port->value = island_get_மதிப்பு(a);
            எனில் (port->value.size() != 0)
	          தள்ளு_மதிப்பு_through_branches(port->value, connections);

            connections.clear();
      }

	// Do the same for the B side port. Note that if the branch
        // is enabled, the B side port will have already been visited
        // when we resolved the A side port.
      port = dynamic_cast<vvp_island_port*>(b->fun);
      எனில் (port->value.size() == 0) {
	    vvp_branch_ptr_t b_side(இது, 1);
	    island_collect_முனை(connections, b_side);

	    port->value = island_get_மதிப்பு(b);
	    எனில் (port->value.size() != 0)
	          தள்ளு_மதிப்பு_through_branches(port->value, connections);

            connections.clear();
      }
}

வெறுமை vvp_island_branch_tran::run_output()
{
      vvp_island_port*port;

	// If the A side port hasn't already been updated, send the
        // resolved value to the output.
      port = dynamic_cast<vvp_island_port*>(a->fun);
      எனில் (port->value.size() != 0) {
	    island_send_மதிப்பு(a, port->value);
	    port->value = vvp_vector8_t::nil;
      }

	// Do the same for the B side port.
      port = dynamic_cast<vvp_island_port*>(b->fun);
      எனில் (port->value.size() != 0) {
	    island_send_மதிப்பு(b, port->value);
	    port->value = vvp_vector8_t::nil;
      }
}

வெறுமை compile_island_tran(எழுத்து*label)
{
      vvp_island*use_island = புதிய vvp_island_tran;
      compile_island_base(label, use_island);
}

வெறுமை compile_island_tranif(முழு sense, எழுத்து*island, எழுத்து*pa, எழுத்து*pb, எழுத்து*pe,
                           தருக்க resistive)
{
      vvp_island*use_island = compile_find_island(island);
      ப்ரதிஜ்ஞா(use_island);
      விடுவி(island);

      vvp_net_t*en = வெற்று;

      எனில் (pe) {
	    en = use_island->find_port(pe);
	    ப்ரதிஜ்ஞா(en);
	    விடுவி(pe);
      }

      vvp_island_branch_tran*br = புதிய vvp_island_branch_tran(en,
                                                             sense ? உண்மை :
                                                                     பொய்,
                                                             0, 0, 0, resistive);

      use_island->add_branch(br, pa, pb);

      விடுவி(pa);
      விடுவி(pb);
}


வெறுமை compile_island_tranvp(எழுத்து*island, எழுத்து*pa, எழுத்து*pb,
			   unsigned wid, unsigned par, unsigned off)
{
      vvp_island*use_island = compile_find_island(island);
      ப்ரதிஜ்ஞா(use_island);
      விடுவி(island);

      vvp_island_branch_tran*br = புதிய vvp_island_branch_tran(வெற்று, பொய்,
                                                             wid, par, off, பொய்);

      use_island->add_branch(br, pa, pb);

      விடுவி(pa);
      விடுவி(pb);
}
