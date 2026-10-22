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

ఉపయోగించి నామాకాశ std;

తరగతి vvp_island_tran : బహిరంగ vvp_island {

    బహిరంగ:
      శూన్య నిరంతరం_island() override;
      శూన్య లెక్క_drivers(vvp_island_పోర్టు*పోర్టు, గుర్తులేని బిట్టు_idx,
                         గుర్తులేని counts[3]) override;
};

గణన tran_state_t {
      tran_disabled,
      tran_enabled,
      tran_unknown
};

నిర్మాణం vvp_island_branch_tran : బహిరంగ vvp_island_branch {

      vvp_island_branch_tran(vvp_net_t*en__, తార్కిక చురుకైన_ఎగువ,
                             గుర్తులేని వెడల్పు_3, గుర్తులేని part__,
                             గుర్తులేని స్థానభ్రంశం_3, తార్కిక resistive__);
      శూన్య నిరంతరం_test_enabled();
      తార్కిక rerun_test_enabled();
      శూన్య నిరంతరం_resolution();
      శూన్య నిరంతరం_output();

      vvp_net_t*en;
      గుర్తులేని width, part, స్థానభ్రంశం_2;
      తార్కిక చురుకైన_ఎగువ_2, resistive;
      tran_state_t state;
};

vvp_island_branch_tran::vvp_island_branch_tran(vvp_net_t*en__,
                                               తార్కిక చురుకైన_ఎగువ,
                                               గుర్తులేని వెడల్పు_3,
                                               గుర్తులేని part__,
                                               గుర్తులేని స్థానభ్రంశం_3,
                                               తార్కిక resistive__)
: en(en__), width(వెడల్పు_3), part(part__), స్థానభ్రంశం_2(స్థానభ్రంశం_3),
  చురుకైన_ఎగువ_2(చురుకైన_ఎగువ), resistive(resistive__)
{
      state = en__ ? tran_disabled : tran_enabled;
}

స్థిర అంతఃవరుస vvp_island_branch_tran* BRANCH_TRAN(vvp_island_branch*tmp)
{
      vvp_island_branch_tran*res = dynamic_cast<vvp_island_branch_tran*>(tmp);
      ప్రతిజ్ఞా(res);
      తిరిగి res;
}

/*
 * The run_island() method is called by the scheduler to run the
 * entire island. We run the island by calling run_resolution() for
 * all the branches in the island.
*/
శూన్య vvp_island_tran::నిరంతరం_island()
{
	// Test to see if any of the branches are enabled. This loop
	// tests the enabled inputs for all the branches and caches
	// the results in the state for each branch.
      కొరకు (vvp_island_branch*cur = branches_ ; cur ; cur = cur->తదుపరి_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ప్రతిజ్ఞా(tmp);
	    tmp->నిరంతరం_test_enabled();
      }

rerun:
	// Now resolve all the branches in the island.
      కొరకు (vvp_island_branch*cur = branches_ ; cur ; cur = cur->తదుపరి_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ప్రతిజ్ఞా(tmp);
	    tmp->నిరంతరం_resolution();
      }

	// Now output the resolved values.
      కొరకు (vvp_island_branch*cur = branches_ ; cur ; cur = cur->తదుపరి_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ప్రతిజ్ఞా(tmp);
	    tmp->నిరంతరం_output();
      }

	// Now check if the enable inputs have been affected by the resolution.
      తార్కిక enable_మారింది = అబద్ధం;
      కొరకు (vvp_island_branch*cur = branches_ ; cur ; cur = cur->తదుపరి_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    ప్రతిజ్ఞా(tmp);
	    enable_మారింది |= tmp->rerun_test_enabled();
      }
      ఒకవేళ (enable_మారింది) వెళ్ళు rerun;
}

స్థిర శూన్య లెక్క_drivers_2(vvp_branch_ptr_t cur, తార్కిక other_side_సందర్శించిన,
                           గుర్తులేని బిట్టు_idx, గుర్తులేని counts[3])
{
        // First count any value driven into the port associated with
        // the current endpoint.
      vvp_net_t*net = cur.పోర్టు() ? cur.ptr()->b : cur.ptr()->a;
      vvp_scalar_t బిట్టు = island_get_విలువ(net).value(బిట్టు_idx);
      update_driver_counts(బిట్టు.value(), counts);

        // Now handle all the branches attached to that port.
      vvp_branch_ptr_t idx = cur;
      చేయి {
            vvp_island_branch_tran*tmp = BRANCH_TRAN(idx.ptr());

              // If this branch represents a tran gate, we count the
              // value on the other side of the tran (providing it is
              // enabled) as a single driver.
            ఒకవేళ (tmp->width == 0) {
                  ఒకవేళ (tmp->state == tran_enabled) {
                        net = idx.పోర్టు() ? idx.ptr()->a : idx.ptr()->b;
                        బిట్టు = island_get_పంపిన_విలువ(net).value(బిట్టు_idx);
                        update_driver_counts(బిట్టు.value(), counts);
                  }
                  కొనసాగు;
            }

              // If we get here, this branch is a part select. If we've
              // just come from the other end of the branch, we're done.
            ఒకవేళ ((idx == cur) && other_side_సందర్శించిన)
                  కొనసాగు;

              // If this is the narrow end of the part select, the other
              // end must include the bit we are interested in. Follow
              // the branch to collect any drivers on the other side.
            ఒకవేళ (idx.పోర్టు() == 1) {
                  vvp_branch_ptr_t a_side(tmp, 0);
                  లెక్క_drivers_2(a_side, నిజం, tmp->స్థానభ్రంశం_2 + బిట్టు_idx, counts);
                  కొనసాగు;
            }

              // If we get here, this branch is the wide end of a part
              // select. If the bit we are interested in is within the
              // selected part, follow the branch to collect any drivers
              // on the other side.
            ఒకవేళ ((బిట్టు_idx >= tmp->స్థానభ్రంశం_2) && (బిట్టు_idx < tmp->స్థానభ్రంశం_2+tmp->part)) {
                  vvp_branch_ptr_t b_side(tmp, 1);
                  లెక్క_drivers_2(b_side, నిజం, బిట్టు_idx - tmp->స్థానభ్రంశం_2, counts);
                  కొనసాగు;
            }
      } అయితే ((idx = తదుపరి_2(idx)) != cur);
}

శూన్య vvp_island_tran::లెక్క_drivers(vvp_island_పోర్టు*పోర్టు, గుర్తులేని బిట్టు_idx,
                                    గుర్తులేని counts[3])
{
        // First we need to find a branch that is attached to the specified
        // port. Unfortunately there's no quick way to do this.
      vvp_island_branch*branch = branches_;
      గుర్తులేని side = 0;
      అయితే (branch) {
            ఒకవేళ (branch->a->fun == పోర్టు) {
                  side = 0;
                  ఆపు;
            }
            ఒకవేళ (branch->b->fun == పోర్టు) {
                  side = 1;
                  ఆపు;
            }
            branch = branch->తదుపరి_branch;
      }
      ప్రతిజ్ఞా(branch);

        // Now count the drivers, pushing through the network as necessary.
      vvp_branch_ptr_t endpoint(branch, side);
      లెక్క_drivers_2(endpoint, అబద్ధం, బిట్టు_idx, counts);
}

శూన్య vvp_island_branch_tran::నిరంతరం_test_enabled()
{
      స్థిరరాశి vvp_island_పోర్టు*ep = en? dynamic_cast<vvp_island_పోర్టు*> (en->fun) : ఖాళీ;

	// If there is no ep port (no "enabled" input) then this is a
	// tran branch. Assume it is always enabled.
      ఒకవేళ (ep == 0) {
	    state = tran_enabled;
	    తిరిగి;
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
      ఒకవేళ (ep->outvalue.size() != 0)
	    enable_val = ep->outvalue.value(0).value();
      లేకపోతే ఒకవేళ (ep->invalue.size() == 0)
	    enable_val = BIT4_Z;
      లేకపోతే
	    enable_val = ep->invalue.value(0).value();

      ఎంపిక (enable_val) {
	  స్థితి BIT4_0:
	    state = చురుకైన_ఎగువ_2 ? tran_disabled : tran_enabled;
	    ఆపు;
	  స్థితి BIT4_1:
	    state = చురుకైన_ఎగువ_2 ? tran_enabled : tran_disabled;
	    ఆపు;
	  అప్రమేయ:
	    state = tran_unknown;
	    ఆపు;
      }
}

తార్కిక vvp_island_branch_tran::rerun_test_enabled()
{
      స్థిరరాశి vvp_island_పోర్టు*ep = en? dynamic_cast<vvp_island_పోర్టు*> (en->fun) : ఖాళీ;

      ఒకవేళ (ep == 0)
	    తిరిగి అబద్ధం;

	// We are only looking for changes resulting from running the island
	// resolution. If the outvalue is nil, we know that the enable port
	// is an .import, so won't be affected.
      ఒకవేళ (ep->outvalue.size() == 0)
	    తిరిగి అబద్ధం;

      vvp_bit4_t enable_val = ep->outvalue.value(0).value();

      tran_state_t old_state = state;

      ఎంపిక (enable_val) {
	  స్థితి BIT4_0:
	    state = చురుకైన_ఎగువ_2 ? tran_disabled : tran_enabled;
	    ఆపు;
	  స్థితి BIT4_1:
	    state = చురుకైన_ఎగువ_2 ? tran_enabled : tran_disabled;
	    ఆపు;
	  అప్రమేయ:
	    state = tran_unknown;
	    ఆపు;
      }

      తిరిగి state != old_state;
}

// The IEEE standard does not specify the behaviour when a tranif control
// input is 'x' or 'z'. We use the rules that are given for MOS switches.
అంతఃవరుస vvp_vector8_t resolve_ambiguous(స్థిరరాశి vvp_vector8_t&a,
                                       స్థిరరాశి vvp_vector8_t&b,
                                       tran_state_t state,
                                       స్థిరరాశి గుర్తులేని str_map[8])
{
      ప్రతిజ్ఞా(a.size() == b.size());
      vvp_vector8_t out (a.size());

      కొరకు (గుర్తులేని idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_scalar_t a_బిట్టు = a.value(idx);
	    vvp_scalar_t b_బిట్టు = b.value(idx);
            b_బిట్టు = vvp_scalar_t(b_బిట్టు.value(),
                                 str_map[b_బిట్టు.strength0()],
                                 str_map[b_బిట్టు.strength1()]);
	    ఒకవేళ (state == tran_unknown) {
		  ఎంపిక (b_బిట్టు.value()) {
		      స్థితి BIT4_0:
			b_బిట్టు = vvp_scalar_t(BIT4_X, b_బిట్టు.strength0(), 0);
			ఆపు;
		      స్థితి BIT4_1:
			b_బిట్టు = vvp_scalar_t(BIT4_X, 0, b_బిట్టు.strength1());
			ఆపు;
		      అప్రమేయ:
			ఆపు;
		  }
	    }
	    out.సమితి_బిట్టు(idx, resolve(a_బిట్టు, b_బిట్టు));
      }
      తిరిగి out;
}

స్థిర శూన్య నెట్టు_విలువ_through_branches(స్థిరరాశి vvp_vector8_t&val,
					స్థిరరాశి list<vvp_branch_ptr_t>&connections);

స్థిర శూన్య నెట్టు_విలువ_through_branch(స్థిరరాశి vvp_vector8_t&val,
                                      vvp_branch_ptr_t cur)
{
      vvp_island_branch_tran*branch = BRANCH_TRAN(cur.ptr());

        // If the branch is disabled, skip.
      ఒకవేళ (branch->state == tran_disabled)
            తిరిగి;

      గుర్తులేని src_ab = cur.పోర్టు();
      గుర్తులేని dst_ab = src_ab^1;

      vvp_net_t*dst_net = dst_ab? branch->b : branch->a;
      vvp_island_పోర్టు*dst_పోర్టు = dynamic_cast<vvp_island_పోర్టు*>(dst_net->fun);

      vvp_vector8_t old_val = dst_పోర్టు->value;

        // If the port on the other side has not yet been visited,
        // get its input value.
      ఒకవేళ (dst_పోర్టు->value.size() == 0)
            dst_పోర్టు->value = island_get_విలువ(dst_net);

        // If we don't yet have an initial value for the port, skip.
      ఒకవేళ (dst_పోర్టు->value.size() == 0)
            తిరిగి;

        // Now resolve the pushed value with whatever values we have
        // previously collected (and resolved) for the port.
      ఒకవేళ (branch->width == 0) {
              // There are no part selects.
            dst_పోర్టు->value = resolve_ambiguous(dst_పోర్టు->value, val, branch->state,
                                                vvp_switch_strength_map[branch->resistive]);

      } లేకపోతే ఒకవేళ (dst_ab == 1) {
              // The other side is a strict subset (part select)
              // of this side.
            vvp_vector8_t tmp = val.subvalue(branch->స్థానభ్రంశం_2, branch->part);
            dst_పోర్టు->value = resolve(dst_పోర్టు->value, tmp);

      } లేకపోతే {
              // The other side is a superset of this side.
            vvp_vector8_t tmp = part_expand(val, branch->width, branch->స్థానభ్రంశం_2);
            dst_పోర్టు->value = resolve(dst_పోర్టు->value, tmp);
      }

        // If the resolved value for the port has changed, push the new
        // value back into the network.
      ఒకవేళ (! dst_పోర్టు->value.eeq(old_val)) {
            list<vvp_branch_ptr_t> connections;

	    vvp_branch_ptr_t dst_side(branch, dst_ab);
	    island_collect_కణుపు(connections, dst_side);

	    నెట్టు_విలువ_through_branches(dst_పోర్టు->value, connections);
      }
}

స్థిర శూన్య నెట్టు_విలువ_through_branches(స్థిరరాశి vvp_vector8_t&val,
					స్థిరరాశి list<vvp_branch_ptr_t>&connections)
{
      కొరకు (list<vvp_branch_ptr_t>::const_iterator idx = connections.cbegin()
		 ; idx != connections.cend() ; ++ idx ) {

            నెట్టు_విలువ_through_branch(val, *idx);
      }
}

/*
 * This method resolves the value for a branch recursively. It uses
 * recursive descent to span the graph of branches, pushing values
 * through the network until a stable state is reached.
 */
శూన్య vvp_island_branch_tran::నిరంతరం_resolution()
{
      list<vvp_branch_ptr_t> connections;
      vvp_island_పోర్టు*పోర్టు;

	// If the A side port hasn't already been visited, then push
        // its input value through all the branches connected to it.
      పోర్టు = dynamic_cast<vvp_island_పోర్టు*>(a->fun);
      ఒకవేళ (పోర్టు->value.size() == 0) {
	    vvp_branch_ptr_t a_side(ఇది, 0);
	    island_collect_కణుపు(connections, a_side);

	    పోర్టు->value = island_get_విలువ(a);
            ఒకవేళ (పోర్టు->value.size() != 0)
	          నెట్టు_విలువ_through_branches(పోర్టు->value, connections);

            connections.clear();
      }

	// Do the same for the B side port. Note that if the branch
        // is enabled, the B side port will have already been visited
        // when we resolved the A side port.
      పోర్టు = dynamic_cast<vvp_island_పోర్టు*>(b->fun);
      ఒకవేళ (పోర్టు->value.size() == 0) {
	    vvp_branch_ptr_t b_side(ఇది, 1);
	    island_collect_కణుపు(connections, b_side);

	    పోర్టు->value = island_get_విలువ(b);
	    ఒకవేళ (పోర్టు->value.size() != 0)
	          నెట్టు_విలువ_through_branches(పోర్టు->value, connections);

            connections.clear();
      }
}

శూన్య vvp_island_branch_tran::నిరంతరం_output()
{
      vvp_island_పోర్టు*పోర్టు;

	// If the A side port hasn't already been updated, send the
        // resolved value to the output.
      పోర్టు = dynamic_cast<vvp_island_పోర్టు*>(a->fun);
      ఒకవేళ (పోర్టు->value.size() != 0) {
	    island_send_విలువ(a, పోర్టు->value);
	    పోర్టు->value = vvp_vector8_t::nil;
      }

	// Do the same for the B side port.
      పోర్టు = dynamic_cast<vvp_island_పోర్టు*>(b->fun);
      ఒకవేళ (పోర్టు->value.size() != 0) {
	    island_send_విలువ(b, పోర్టు->value);
	    పోర్టు->value = vvp_vector8_t::nil;
      }
}

శూన్య compile_island_tran(అక్షరం*label)
{
      vvp_island*use_island = కొత్త vvp_island_tran;
      compile_island_పునాది(label, use_island);
}

శూన్య compile_island_tranif(పూర్ణాంకం sense, అక్షరం*island, అక్షరం*pa, అక్షరం*pb, అక్షరం*pe,
                           తార్కిక resistive)
{
      vvp_island*use_island = compile_కనుగొను_island(island);
      ప్రతిజ్ఞా(use_island);
      విడుదల(island);

      vvp_net_t*en = ఖాళీ;

      ఒకవేళ (pe) {
	    en = use_island->కనుగొను_పోర్టు(pe);
	    ప్రతిజ్ఞా(en);
	    విడుదల(pe);
      }

      vvp_island_branch_tran*br = కొత్త vvp_island_branch_tran(en,
                                                             sense ? నిజం :
                                                                     అబద్ధం,
                                                             0, 0, 0, resistive);

      use_island->కూడు_branch(br, pa, pb);

      విడుదల(pa);
      విడుదల(pb);
}


శూన్య compile_island_tranvp(అక్షరం*island, అక్షరం*pa, అక్షరం*pb,
			   గుర్తులేని wid, గుర్తులేని par, గుర్తులేని off)
{
      vvp_island*use_island = compile_కనుగొను_island(island);
      ప్రతిజ్ఞా(use_island);
      విడుదల(island);

      vvp_island_branch_tran*br = కొత్త vvp_island_branch_tran(ఖాళీ, అబద్ధం,
                                                             wid, par, off, అబద్ధం);

      use_island->కూడు_branch(br, pa, pb);

      విడుదల(pa);
      విడుదల(pb);
}
