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

using ନାମାକାଶ std;

ଶ୍ରେଣୀ vvp_island_tran : ସର୍ବସାଧାରଣ vvp_island {

    ସର୍ବସାଧାରଣ:
      ଶୂନ୍ୟ run_island() override;
      ଶୂନ୍ୟ ଗଣନା_drivers(vvp_island_port*port, unsigned bit_idx,
                         unsigned counts[3]) override;
};

ଗଣନା tran_state_t {
      tran_disabled,
      tran_enabled,
      tran_unknown
};

ସଂରଚନା vvp_island_branch_tran : ସର୍ବସାଧାରଣ vvp_island_branch {

      vvp_island_branch_tran(vvp_net_t*en__, ତାର୍କିକ active_ଉପର,
                             unsigned width__, unsigned part__,
                             unsigned offset__, ତାର୍କିକ resistive__);
      ଶୂନ୍ୟ run_test_enabled();
      ତାର୍କିକ rerun_test_enabled();
      ଶୂନ୍ୟ run_resolution();
      ଶୂନ୍ୟ run_output();

      vvp_net_t*en;
      unsigned width, part, offset;
      ତାର୍କିକ active_ଉପର_2, resistive;
      tran_state_t state;
};

vvp_island_branch_tran::vvp_island_branch_tran(vvp_net_t*en__,
                                               ତାର୍କିକ active_ଉପର,
                                               unsigned width__,
                                               unsigned part__,
                                               unsigned offset__,
                                               ତାର୍କିକ resistive__)
: en(en__), width(width__), part(part__), offset(offset__),
  active_ଉପର_2(active_ଉପର), resistive(resistive__)
{
      state = en__ ? tran_disabled : tran_enabled;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ vvp_island_branch_tran* BRANCH_TRAN(vvp_island_branch*tmp)
{
      vvp_island_branch_tran*res = dynamic_cast<vvp_island_branch_tran*>(tmp);
      ପ୍ରତିଜ୍ଞା(res);
      ଫେରନ୍ତୁ res;
}

/*
 * The run_island() method is called by the scheduler to run the
 * entire island. We run the island by calling run_resolution() for
 * all the branches in the island.
*/
ଶୂନ୍ୟ vvp_island_tran::run_island()
{
	// Test to see if any of the branches are enabled. This loop
	// tests the enabled inputs for all the branches and caches
	// the results in the state for each branch.
      ପାଇଁ (vvp_island_branch*cur = branches_ ; cur ; cur = cur->next_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ପ୍ରତିଜ୍ଞା(tmp);
	    tmp->run_test_enabled();
      }

rerun:
	// Now resolve all the branches in the island.
      ପାଇଁ (vvp_island_branch*cur = branches_ ; cur ; cur = cur->next_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ପ୍ରତିଜ୍ଞା(tmp);
	    tmp->run_resolution();
      }

	// Now output the resolved values.
      ପାଇଁ (vvp_island_branch*cur = branches_ ; cur ; cur = cur->next_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ପ୍ରତିଜ୍ଞା(tmp);
	    tmp->run_output();
      }

	// Now check if the enable inputs have been affected by the resolution.
      ତାର୍କିକ enable_changed = ମିଥ୍ୟା;
      ପାଇଁ (vvp_island_branch*cur = branches_ ; cur ; cur = cur->next_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ପ୍ରତିଜ୍ଞା(tmp);
	    enable_changed |= tmp->rerun_test_enabled();
      }
      ଯଦି (enable_changed) ଯାଅ rerun;
}

ସ୍ଥିର ଶୂନ୍ୟ ଗଣନା_drivers_2(vvp_branch_ptr_t cur, ତାର୍କିକ other_side_visited,
                           unsigned bit_idx, unsigned counts[3])
{
        // First count any value driven into the port associated with
        // the current endpoint.
      vvp_net_t*net = cur.port() ? cur.ptr()->b : cur.ptr()->a;
      vvp_scalar_t bit = island_get_ମୂଲ୍ୟ(net).value(bit_idx);
      update_driver_counts(bit.value(), counts);

        // Now handle all the branches attached to that port.
      vvp_branch_ptr_t idx = cur;
      କର {
            vvp_island_branch_tran*tmp = BRANCH_TRAN(idx.ptr());

              // If this branch represents a tran gate, we count the
              // value on the other side of the tran (providing it is
              // enabled) as a single driver.
            ଯଦି (tmp->width == 0) {
                  ଯଦି (tmp->state == tran_enabled) {
                        net = idx.port() ? idx.ptr()->a : idx.ptr()->b;
                        bit = island_get_sent_ମୂଲ୍ୟ(net).value(bit_idx);
                        update_driver_counts(bit.value(), counts);
                  }
                  ଜାରି;
            }

              // If we get here, this branch is a part select. If we've
              // just come from the other end of the branch, we're done.
            ଯଦି ((idx == cur) && other_side_visited)
                  ଜାରି;

              // If this is the narrow end of the part select, the other
              // end must include the bit we are interested in. Follow
              // the branch to collect any drivers on the other side.
            ଯଦି (idx.port() == 1) {
                  vvp_branch_ptr_t a_side(tmp, 0);
                  ଗଣନା_drivers_2(a_side, ସତ୍ୟ, tmp->offset + bit_idx, counts);
                  ଜାରି;
            }

              // If we get here, this branch is the wide end of a part
              // select. If the bit we are interested in is within the
              // selected part, follow the branch to collect any drivers
              // on the other side.
            ଯଦି ((bit_idx >= tmp->offset) && (bit_idx < tmp->offset+tmp->part)) {
                  vvp_branch_ptr_t b_side(tmp, 1);
                  ଗଣନା_drivers_2(b_side, ସତ୍ୟ, bit_idx - tmp->offset, counts);
                  ଜାରି;
            }
      } ଯେତେବେଳେ ((idx = next(idx)) != cur);
}

ଶୂନ୍ୟ vvp_island_tran::ଗଣନା_drivers(vvp_island_port*port, unsigned bit_idx,
                                    unsigned counts[3])
{
        // First we need to find a branch that is attached to the specified
        // port. Unfortunately there's no quick way to do this.
      vvp_island_branch*branch = branches_;
      unsigned side = 0;
      ଯେତେବେଳେ (branch) {
            ଯଦି (branch->a->fun == port) {
                  side = 0;
                  ଭାଙ୍ଗ;
            }
            ଯଦି (branch->b->fun == port) {
                  side = 1;
                  ଭାଙ୍ଗ;
            }
            branch = branch->next_branch;
      }
      ପ୍ରତିଜ୍ଞା(branch);

        // Now count the drivers, pushing through the network as necessary.
      vvp_branch_ptr_t endpoint(branch, side);
      ଗଣନା_drivers_2(endpoint, ମିଥ୍ୟା, bit_idx, counts);
}

ଶୂନ୍ୟ vvp_island_branch_tran::run_test_enabled()
{
      ଅଚଳ vvp_island_port*ep = en? dynamic_cast<vvp_island_port*> (en->fun) : ଖାଲି;

	// If there is no ep port (no "enabled" input) then this is a
	// tran branch. Assume it is always enabled.
      ଯଦି (ep == 0) {
	    state = tran_enabled;
	    ଫେରନ୍ତୁ;
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
      ଯଦି (ep->outvalue.size() != 0)
	    enable_val = ep->outvalue.value(0).value();
      ଅନ୍ୟଥା ଯଦି (ep->invalue.size() == 0)
	    enable_val = BIT4_Z;
      ଅନ୍ୟଥା
	    enable_val = ep->invalue.value(0).value();

      ବାଛ (enable_val) {
	  ସ୍ଥିତି BIT4_0:
	    state = active_ଉପର_2 ? tran_disabled : tran_enabled;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_1:
	    state = active_ଉପର_2 ? tran_enabled : tran_disabled;
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    state = tran_unknown;
	    ଭାଙ୍ଗ;
      }
}

ତାର୍କିକ vvp_island_branch_tran::rerun_test_enabled()
{
      ଅଚଳ vvp_island_port*ep = en? dynamic_cast<vvp_island_port*> (en->fun) : ଖାଲି;

      ଯଦି (ep == 0)
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;

	// We are only looking for changes resulting from running the island
	// resolution. If the outvalue is nil, we know that the enable port
	// is an .import, so won't be affected.
      ଯଦି (ep->outvalue.size() == 0)
	    ଫେରନ୍ତୁ ମିଥ୍ୟା;

      vvp_bit4_t enable_val = ep->outvalue.value(0).value();

      tran_state_t old_state = state;

      ବାଛ (enable_val) {
	  ସ୍ଥିତି BIT4_0:
	    state = active_ଉପର_2 ? tran_disabled : tran_enabled;
	    ଭାଙ୍ଗ;
	  ସ୍ଥିତି BIT4_1:
	    state = active_ଉପର_2 ? tran_enabled : tran_disabled;
	    ଭାଙ୍ଗ;
	  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    state = tran_unknown;
	    ଭାଙ୍ଗ;
      }

      ଫେରନ୍ତୁ state != old_state;
}

// The IEEE standard does not specify the behaviour when a tranif control
// input is 'x' or 'z'. We use the rules that are given for MOS switches.
ଧାଡ଼ିଭିତରେ vvp_vector8_t resolve_ambiguous(ଅଚଳ vvp_vector8_t&a,
                                       ଅଚଳ vvp_vector8_t&b,
                                       tran_state_t state,
                                       ଅଚଳ unsigned str_map[8])
{
      ପ୍ରତିଜ୍ଞା(a.size() == b.size());
      vvp_vector8_t out (a.size());

      ପାଇଁ (unsigned idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_scalar_t a_bit = a.value(idx);
	    vvp_scalar_t b_bit = b.value(idx);
            b_bit = vvp_scalar_t(b_bit.value(),
                                 str_map[b_bit.strength0()],
                                 str_map[b_bit.strength1()]);
	    ଯଦି (state == tran_unknown) {
		  ବାଛ (b_bit.value()) {
		      ସ୍ଥିତି BIT4_0:
			b_bit = vvp_scalar_t(BIT4_X, b_bit.strength0(), 0);
			ଭାଙ୍ଗ;
		      ସ୍ଥିତି BIT4_1:
			b_bit = vvp_scalar_t(BIT4_X, 0, b_bit.strength1());
			ଭାଙ୍ଗ;
		      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଭାଙ୍ଗ;
		  }
	    }
	    out.set_bit(idx, resolve(a_bit, b_bit));
      }
      ଫେରନ୍ତୁ out;
}

ସ୍ଥିର ଶୂନ୍ୟ ଠେଲ_ମୂଲ୍ୟ_through_branches(ଅଚଳ vvp_vector8_t&val,
					ଅଚଳ list<vvp_branch_ptr_t>&connections);

ସ୍ଥିର ଶୂନ୍ୟ ଠେଲ_ମୂଲ୍ୟ_through_branch(ଅଚଳ vvp_vector8_t&val,
                                      vvp_branch_ptr_t cur)
{
      vvp_island_branch_tran*branch = BRANCH_TRAN(cur.ptr());

        // If the branch is disabled, skip.
      ଯଦି (branch->state == tran_disabled)
            ଫେରନ୍ତୁ;

      unsigned src_ab = cur.port();
      unsigned dst_ab = src_ab^1;

      vvp_net_t*dst_net = dst_ab? branch->b : branch->a;
      vvp_island_port*dst_port = dynamic_cast<vvp_island_port*>(dst_net->fun);

      vvp_vector8_t old_val = dst_port->value;

        // If the port on the other side has not yet been visited,
        // get its input value.
      ଯଦି (dst_port->value.size() == 0)
            dst_port->value = island_get_ମୂଲ୍ୟ(dst_net);

        // If we don't yet have an initial value for the port, skip.
      ଯଦି (dst_port->value.size() == 0)
            ଫେରନ୍ତୁ;

        // Now resolve the pushed value with whatever values we have
        // previously collected (and resolved) for the port.
      ଯଦି (branch->width == 0) {
              // There are no part selects.
            dst_port->value = resolve_ambiguous(dst_port->value, val, branch->state,
                                                vvp_switch_strength_map[branch->resistive]);

      } ଅନ୍ୟଥା ଯଦି (dst_ab == 1) {
              // The other side is a strict subset (part select)
              // of this side.
            vvp_vector8_t tmp = val.subvalue(branch->offset, branch->part);
            dst_port->value = resolve(dst_port->value, tmp);

      } ଅନ୍ୟଥା {
              // The other side is a superset of this side.
            vvp_vector8_t tmp = part_expand(val, branch->width, branch->offset);
            dst_port->value = resolve(dst_port->value, tmp);
      }

        // If the resolved value for the port has changed, push the new
        // value back into the network.
      ଯଦି (! dst_port->value.eeq(old_val)) {
            list<vvp_branch_ptr_t> connections;

	    vvp_branch_ptr_t dst_side(branch, dst_ab);
	    island_collect_ଗଣ୍ଠି(connections, dst_side);

	    ଠେଲ_ମୂଲ୍ୟ_through_branches(dst_port->value, connections);
      }
}

ସ୍ଥିର ଶୂନ୍ୟ ଠେଲ_ମୂଲ୍ୟ_through_branches(ଅଚଳ vvp_vector8_t&val,
					ଅଚଳ list<vvp_branch_ptr_t>&connections)
{
      ପାଇଁ (list<vvp_branch_ptr_t>::const_iterator idx = connections.cbegin()
		 ; idx != connections.cend() ; ++ idx ) {

            ଠେଲ_ମୂଲ୍ୟ_through_branch(val, *idx);
      }
}

/*
 * This method resolves the value for a branch recursively. It uses
 * recursive descent to span the graph of branches, pushing values
 * through the network until a stable state is reached.
 */
ଶୂନ୍ୟ vvp_island_branch_tran::run_resolution()
{
      list<vvp_branch_ptr_t> connections;
      vvp_island_port*port;

	// If the A side port hasn't already been visited, then push
        // its input value through all the branches connected to it.
      port = dynamic_cast<vvp_island_port*>(a->fun);
      ଯଦି (port->value.size() == 0) {
	    vvp_branch_ptr_t a_side(ଏହି, 0);
	    island_collect_ଗଣ୍ଠି(connections, a_side);

	    port->value = island_get_ମୂଲ୍ୟ(a);
            ଯଦି (port->value.size() != 0)
	          ଠେଲ_ମୂଲ୍ୟ_through_branches(port->value, connections);

            connections.clear();
      }

	// Do the same for the B side port. Note that if the branch
        // is enabled, the B side port will have already been visited
        // when we resolved the A side port.
      port = dynamic_cast<vvp_island_port*>(b->fun);
      ଯଦି (port->value.size() == 0) {
	    vvp_branch_ptr_t b_side(ଏହି, 1);
	    island_collect_ଗଣ୍ଠି(connections, b_side);

	    port->value = island_get_ମୂଲ୍ୟ(b);
	    ଯଦି (port->value.size() != 0)
	          ଠେଲ_ମୂଲ୍ୟ_through_branches(port->value, connections);

            connections.clear();
      }
}

ଶୂନ୍ୟ vvp_island_branch_tran::run_output()
{
      vvp_island_port*port;

	// If the A side port hasn't already been updated, send the
        // resolved value to the output.
      port = dynamic_cast<vvp_island_port*>(a->fun);
      ଯଦି (port->value.size() != 0) {
	    island_send_ମୂଲ୍ୟ(a, port->value);
	    port->value = vvp_vector8_t::nil;
      }

	// Do the same for the B side port.
      port = dynamic_cast<vvp_island_port*>(b->fun);
      ଯଦି (port->value.size() != 0) {
	    island_send_ମୂଲ୍ୟ(b, port->value);
	    port->value = vvp_vector8_t::nil;
      }
}

ଶୂନ୍ୟ compile_island_tran(ଅକ୍ଷର*label)
{
      vvp_island*use_island = ନୂତନ vvp_island_tran;
      compile_island_base(label, use_island);
}

ଶୂନ୍ୟ compile_island_tranif(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sense, ଅକ୍ଷର*island, ଅକ୍ଷର*pa, ଅକ୍ଷର*pb, ଅକ୍ଷର*pe,
                           ତାର୍କିକ resistive)
{
      vvp_island*use_island = compile_find_island(island);
      ପ୍ରତିଜ୍ଞା(use_island);
      ମୁକ୍ତ(island);

      vvp_net_t*en = ଖାଲି;

      ଯଦି (pe) {
	    en = use_island->find_port(pe);
	    ପ୍ରତିଜ୍ଞା(en);
	    ମୁକ୍ତ(pe);
      }

      vvp_island_branch_tran*br = ନୂତନ vvp_island_branch_tran(en,
                                                             sense ? ସତ୍ୟ :
                                                                     ମିଥ୍ୟା,
                                                             0, 0, 0, resistive);

      use_island->add_branch(br, pa, pb);

      ମୁକ୍ତ(pa);
      ମୁକ୍ତ(pb);
}


ଶୂନ୍ୟ compile_island_tranvp(ଅକ୍ଷର*island, ଅକ୍ଷର*pa, ଅକ୍ଷର*pb,
			   unsigned wid, unsigned par, unsigned off)
{
      vvp_island*use_island = compile_find_island(island);
      ପ୍ରତିଜ୍ଞା(use_island);
      ମୁକ୍ତ(island);

      vvp_island_branch_tran*br = ନୂତନ vvp_island_branch_tran(ଖାଲି, ମିଥ୍ୟା,
                                                             wid, par, off, ମିଥ୍ୟା);

      use_island->add_branch(br, pa, pb);

      ମୁକ୍ତ(pa);
      ମୁକ୍ତ(pb);
}
