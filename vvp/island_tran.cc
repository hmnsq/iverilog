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

using فضاینام std;

رده vvp_island_tran : عمومی vvp_island {

    عمومی:
      تهی run_island() override;
      تهی شمار_drivers(vvp_island_port*port, unsigned bit_idx,
                         unsigned counts[3]) override;
};

شمارش tran_state_t {
      tran_disabled,
      tran_enabled,
      tran_unknown
};

ساختار vvp_island_branch_tran : عمومی vvp_island_branch {

      vvp_island_branch_tran(vvp_net_t*en__, منطقی active_بالا,
                             unsigned width__, unsigned part__,
                             unsigned offset__, منطقی resistive__);
      تهی run_test_enabled();
      منطقی rerun_test_enabled();
      تهی run_resolution();
      تهی run_output();

      vvp_net_t*en;
      unsigned width, part, offset;
      منطقی active_بالا_2, resistive;
      tran_state_t state;
};

vvp_island_branch_tran::vvp_island_branch_tran(vvp_net_t*en__,
                                               منطقی active_بالا,
                                               unsigned width__,
                                               unsigned part__,
                                               unsigned offset__,
                                               منطقی resistive__)
: en(en__), width(width__), part(part__), offset(offset__),
  active_بالا_2(active_بالا), resistive(resistive__)
{
      state = en__ ? tran_disabled : tran_enabled;
}

ایستا درونخطی vvp_island_branch_tran* BRANCH_TRAN(vvp_island_branch*tmp)
{
      vvp_island_branch_tran*res = dynamic_cast<vvp_island_branch_tran*>(tmp);
      أكد(res);
      بازگشت res;
}

/*
 * The run_island() method is called by the scheduler to run the
 * entire island. We run the island by calling run_resolution() for
 * all the branches in the island.
*/
تهی vvp_island_tran::run_island()
{
	// Test to see if any of the branches are enabled. This loop
	// tests the enabled inputs for all the branches and caches
	// the results in the state for each branch.
      برای (vvp_island_branch*cur = branches_ ; cur ; cur = cur->next_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    أكد(tmp);
	    tmp->run_test_enabled();
      }

rerun:
	// Now resolve all the branches in the island.
      برای (vvp_island_branch*cur = branches_ ; cur ; cur = cur->next_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    أكد(tmp);
	    tmp->run_resolution();
      }

	// Now output the resolved values.
      برای (vvp_island_branch*cur = branches_ ; cur ; cur = cur->next_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    أكد(tmp);
	    tmp->run_output();
      }

	// Now check if the enable inputs have been affected by the resolution.
      منطقی enable_changed = نادرست;
      برای (vvp_island_branch*cur = branches_ ; cur ; cur = cur->next_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    أكد(tmp);
	    enable_changed |= tmp->rerun_test_enabled();
      }
      اگر (enable_changed) برو rerun;
}

ایستا تهی شمار_drivers_2(vvp_branch_ptr_t cur, منطقی other_side_visited,
                           unsigned bit_idx, unsigned counts[3])
{
        // First count any value driven into the port associated with
        // the current endpoint.
      vvp_net_t*net = cur.port() ? cur.ptr()->b : cur.ptr()->a;
      vvp_scalar_t bit = island_get_مقدار(net).value(bit_idx);
      update_driver_counts(bit.value(), counts);

        // Now handle all the branches attached to that port.
      vvp_branch_ptr_t idx = cur;
      انجامبده {
            vvp_island_branch_tran*tmp = BRANCH_TRAN(idx.ptr());

              // If this branch represents a tran gate, we count the
              // value on the other side of the tran (providing it is
              // enabled) as a single driver.
            اگر (tmp->width == 0) {
                  اگر (tmp->state == tran_enabled) {
                        net = idx.port() ? idx.ptr()->a : idx.ptr()->b;
                        bit = island_get_sent_مقدار(net).value(bit_idx);
                        update_driver_counts(bit.value(), counts);
                  }
                  ادامه;
            }

              // If we get here, this branch is a part select. If we've
              // just come from the other end of the branch, we're done.
            اگر ((idx == cur) && other_side_visited)
                  ادامه;

              // If this is the narrow end of the part select, the other
              // end must include the bit we are interested in. Follow
              // the branch to collect any drivers on the other side.
            اگر (idx.port() == 1) {
                  vvp_branch_ptr_t a_side(tmp, 0);
                  شمار_drivers_2(a_side, درست, tmp->offset + bit_idx, counts);
                  ادامه;
            }

              // If we get here, this branch is the wide end of a part
              // select. If the bit we are interested in is within the
              // selected part, follow the branch to collect any drivers
              // on the other side.
            اگر ((bit_idx >= tmp->offset) && (bit_idx < tmp->offset+tmp->part)) {
                  vvp_branch_ptr_t b_side(tmp, 1);
                  شمار_drivers_2(b_side, درست, bit_idx - tmp->offset, counts);
                  ادامه;
            }
      } وقتی ((idx = next(idx)) != cur);
}

تهی vvp_island_tran::شمار_drivers(vvp_island_port*port, unsigned bit_idx,
                                    unsigned counts[3])
{
        // First we need to find a branch that is attached to the specified
        // port. Unfortunately there's no quick way to do this.
      vvp_island_branch*branch = branches_;
      unsigned side = 0;
      وقتی (branch) {
            اگر (branch->a->fun == port) {
                  side = 0;
                  بشکن;
            }
            اگر (branch->b->fun == port) {
                  side = 1;
                  بشکن;
            }
            branch = branch->next_branch;
      }
      أكد(branch);

        // Now count the drivers, pushing through the network as necessary.
      vvp_branch_ptr_t endpoint(branch, side);
      شمار_drivers_2(endpoint, نادرست, bit_idx, counts);
}

تهی vvp_island_branch_tran::run_test_enabled()
{
      ثابت vvp_island_port*ep = en? dynamic_cast<vvp_island_port*> (en->fun) : پوچ;

	// If there is no ep port (no "enabled" input) then this is a
	// tran branch. Assume it is always enabled.
      اگر (ep == 0) {
	    state = tran_enabled;
	    بازگشت;
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
      اگر (ep->outvalue.size() != 0)
	    enable_val = ep->outvalue.value(0).value();
      وگرنه اگر (ep->invalue.size() == 0)
	    enable_val = BIT4_Z;
      وگرنه
	    enable_val = ep->invalue.value(0).value();

      انتخاب (enable_val) {
	  حالت BIT4_0:
	    state = active_بالا_2 ? tran_disabled : tran_enabled;
	    بشکن;
	  حالت BIT4_1:
	    state = active_بالا_2 ? tran_enabled : tran_disabled;
	    بشکن;
	  پیشفرض:
	    state = tran_unknown;
	    بشکن;
      }
}

منطقی vvp_island_branch_tran::rerun_test_enabled()
{
      ثابت vvp_island_port*ep = en? dynamic_cast<vvp_island_port*> (en->fun) : پوچ;

      اگر (ep == 0)
	    بازگشت نادرست;

	// We are only looking for changes resulting from running the island
	// resolution. If the outvalue is nil, we know that the enable port
	// is an .import, so won't be affected.
      اگر (ep->outvalue.size() == 0)
	    بازگشت نادرست;

      vvp_bit4_t enable_val = ep->outvalue.value(0).value();

      tran_state_t old_state = state;

      انتخاب (enable_val) {
	  حالت BIT4_0:
	    state = active_بالا_2 ? tran_disabled : tran_enabled;
	    بشکن;
	  حالت BIT4_1:
	    state = active_بالا_2 ? tran_enabled : tran_disabled;
	    بشکن;
	  پیشفرض:
	    state = tran_unknown;
	    بشکن;
      }

      بازگشت state != old_state;
}

// The IEEE standard does not specify the behaviour when a tranif control
// input is 'x' or 'z'. We use the rules that are given for MOS switches.
درونخطی vvp_vector8_t resolve_ambiguous(ثابت vvp_vector8_t&a,
                                       ثابت vvp_vector8_t&b,
                                       tran_state_t state,
                                       ثابت unsigned str_map[8])
{
      أكد(a.size() == b.size());
      vvp_vector8_t out (a.size());

      برای (unsigned idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_scalar_t a_bit = a.value(idx);
	    vvp_scalar_t b_bit = b.value(idx);
            b_bit = vvp_scalar_t(b_bit.value(),
                                 str_map[b_bit.strength0()],
                                 str_map[b_bit.strength1()]);
	    اگر (state == tran_unknown) {
		  انتخاب (b_bit.value()) {
		      حالت BIT4_0:
			b_bit = vvp_scalar_t(BIT4_X, b_bit.strength0(), 0);
			بشکن;
		      حالت BIT4_1:
			b_bit = vvp_scalar_t(BIT4_X, 0, b_bit.strength1());
			بشکن;
		      پیشفرض:
			بشکن;
		  }
	    }
	    out.set_bit(idx, resolve(a_bit, b_bit));
      }
      بازگشت out;
}

ایستا تهی افزودن_مقدار_through_branches(ثابت vvp_vector8_t&val,
					ثابت list<vvp_branch_ptr_t>&connections);

ایستا تهی افزودن_مقدار_through_branch(ثابت vvp_vector8_t&val,
                                      vvp_branch_ptr_t cur)
{
      vvp_island_branch_tran*branch = BRANCH_TRAN(cur.ptr());

        // If the branch is disabled, skip.
      اگر (branch->state == tran_disabled)
            بازگشت;

      unsigned src_ab = cur.port();
      unsigned dst_ab = src_ab^1;

      vvp_net_t*dst_net = dst_ab? branch->b : branch->a;
      vvp_island_port*dst_port = dynamic_cast<vvp_island_port*>(dst_net->fun);

      vvp_vector8_t old_val = dst_port->value;

        // If the port on the other side has not yet been visited,
        // get its input value.
      اگر (dst_port->value.size() == 0)
            dst_port->value = island_get_مقدار(dst_net);

        // If we don't yet have an initial value for the port, skip.
      اگر (dst_port->value.size() == 0)
            بازگشت;

        // Now resolve the pushed value with whatever values we have
        // previously collected (and resolved) for the port.
      اگر (branch->width == 0) {
              // There are no part selects.
            dst_port->value = resolve_ambiguous(dst_port->value, val, branch->state,
                                                vvp_switch_strength_map[branch->resistive]);

      } وگرنه اگر (dst_ab == 1) {
              // The other side is a strict subset (part select)
              // of this side.
            vvp_vector8_t tmp = val.subvalue(branch->offset, branch->part);
            dst_port->value = resolve(dst_port->value, tmp);

      } وگرنه {
              // The other side is a superset of this side.
            vvp_vector8_t tmp = part_expand(val, branch->width, branch->offset);
            dst_port->value = resolve(dst_port->value, tmp);
      }

        // If the resolved value for the port has changed, push the new
        // value back into the network.
      اگر (! dst_port->value.eeq(old_val)) {
            list<vvp_branch_ptr_t> connections;

	    vvp_branch_ptr_t dst_side(branch, dst_ab);
	    island_collect_گره(connections, dst_side);

	    افزودن_مقدار_through_branches(dst_port->value, connections);
      }
}

ایستا تهی افزودن_مقدار_through_branches(ثابت vvp_vector8_t&val,
					ثابت list<vvp_branch_ptr_t>&connections)
{
      برای (list<vvp_branch_ptr_t>::const_iterator idx = connections.cbegin()
		 ; idx != connections.cend() ; ++ idx ) {

            افزودن_مقدار_through_branch(val, *idx);
      }
}

/*
 * This method resolves the value for a branch recursively. It uses
 * recursive descent to span the graph of branches, pushing values
 * through the network until a stable state is reached.
 */
تهی vvp_island_branch_tran::run_resolution()
{
      list<vvp_branch_ptr_t> connections;
      vvp_island_port*port;

	// If the A side port hasn't already been visited, then push
        // its input value through all the branches connected to it.
      port = dynamic_cast<vvp_island_port*>(a->fun);
      اگر (port->value.size() == 0) {
	    vvp_branch_ptr_t a_side(این, 0);
	    island_collect_گره(connections, a_side);

	    port->value = island_get_مقدار(a);
            اگر (port->value.size() != 0)
	          افزودن_مقدار_through_branches(port->value, connections);

            connections.clear();
      }

	// Do the same for the B side port. Note that if the branch
        // is enabled, the B side port will have already been visited
        // when we resolved the A side port.
      port = dynamic_cast<vvp_island_port*>(b->fun);
      اگر (port->value.size() == 0) {
	    vvp_branch_ptr_t b_side(این, 1);
	    island_collect_گره(connections, b_side);

	    port->value = island_get_مقدار(b);
	    اگر (port->value.size() != 0)
	          افزودن_مقدار_through_branches(port->value, connections);

            connections.clear();
      }
}

تهی vvp_island_branch_tran::run_output()
{
      vvp_island_port*port;

	// If the A side port hasn't already been updated, send the
        // resolved value to the output.
      port = dynamic_cast<vvp_island_port*>(a->fun);
      اگر (port->value.size() != 0) {
	    island_send_مقدار(a, port->value);
	    port->value = vvp_vector8_t::nil;
      }

	// Do the same for the B side port.
      port = dynamic_cast<vvp_island_port*>(b->fun);
      اگر (port->value.size() != 0) {
	    island_send_مقدار(b, port->value);
	    port->value = vvp_vector8_t::nil;
      }
}

تهی compile_island_tran(نویسه*label)
{
      vvp_island*use_island = جدید vvp_island_tran;
      compile_island_base(label, use_island);
}

تهی compile_island_tranif(صحیح sense, نویسه*island, نویسه*pa, نویسه*pb, نویسه*pe,
                           منطقی resistive)
{
      vvp_island*use_island = compile_find_island(island);
      أكد(use_island);
      آزاد(island);

      vvp_net_t*en = پوچ;

      اگر (pe) {
	    en = use_island->find_port(pe);
	    أكد(en);
	    آزاد(pe);
      }

      vvp_island_branch_tran*br = جدید vvp_island_branch_tran(en,
                                                             sense ? درست :
                                                                     نادرست,
                                                             0, 0, 0, resistive);

      use_island->add_branch(br, pa, pb);

      آزاد(pa);
      آزاد(pb);
}


تهی compile_island_tranvp(نویسه*island, نویسه*pa, نویسه*pb,
			   unsigned wid, unsigned par, unsigned off)
{
      vvp_island*use_island = compile_find_island(island);
      أكد(use_island);
      آزاد(island);

      vvp_island_branch_tran*br = جدید vvp_island_branch_tran(پوچ, نادرست,
                                                             wid, par, off, نادرست);

      use_island->add_branch(br, pa, pb);

      آزاد(pa);
      آزاد(pb);
}
