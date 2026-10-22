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

ورتوں نطاق std;

صنف vvp_island_tran : عام vvp_island {

    عام:
      فراغ لگاتار_island() override;
      فراغ گنتی_drivers(vvp_island_پورٹ*پورٹ, بنانشانی ثنائی_ہندسہ_idx,
                         بنانشانی counts[3]) override;
};

تعداد tran_state_t {
      tran_disabled,
      tran_enabled,
      tran_unknown
};

بنية vvp_island_branch_tran : عام vvp_island_branch {

      vvp_island_branch_tran(vvp_net_t*en__, منطقي سرگرم_بلند,
                             بنانشانی چوڑائی_3, بنانشانی part__,
                             بنانشانی وتھ_3, منطقي resistive__);
      فراغ لگاتار_test_enabled();
      منطقي rerun_test_enabled();
      فراغ لگاتار_resolution();
      فراغ لگاتار_output();

      vvp_net_t*en;
      بنانشانی width, part, وتھ_2;
      منطقي سرگرم_بلند_2, resistive;
      tran_state_t state;
};

vvp_island_branch_tran::vvp_island_branch_tran(vvp_net_t*en__,
                                               منطقي سرگرم_بلند,
                                               بنانشانی چوڑائی_3,
                                               بنانشانی part__,
                                               بنانشانی وتھ_3,
                                               منطقي resistive__)
: en(en__), width(چوڑائی_3), part(part__), وتھ_2(وتھ_3),
  سرگرم_بلند_2(سرگرم_بلند), resistive(resistive__)
{
      state = en__ ? tran_disabled : tran_enabled;
}

ساكن مضمن vvp_island_branch_tran* BRANCH_TRAN(vvp_island_branch*tmp)
{
      vvp_island_branch_tran*res = dynamic_cast<vvp_island_branch_tran*>(tmp);
      أكد(res);
      أرجع res;
}

/*
 * The run_island() method is called by the scheduler to run the
 * entire island. We run the island by calling run_resolution() for
 * all the branches in the island.
*/
فراغ vvp_island_tran::لگاتار_island()
{
	// Test to see if any of the branches are enabled. This loop
	// tests the enabled inputs for all the branches and caches
	// the results in the state for each branch.
      لكل (vvp_island_branch*cur = branches_ ; cur ; cur = cur->اگلا_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    أكد(tmp);
	    tmp->لگاتار_test_enabled();
      }

rerun:
	// Now resolve all the branches in the island.
      لكل (vvp_island_branch*cur = branches_ ; cur ; cur = cur->اگلا_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    أكد(tmp);
	    tmp->لگاتار_resolution();
      }

	// Now output the resolved values.
      لكل (vvp_island_branch*cur = branches_ ; cur ; cur = cur->اگلا_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    أكد(tmp);
	    tmp->لگاتار_output();
      }

	// Now check if the enable inputs have been affected by the resolution.
      منطقي enable_بدلیا = خطأ;
      لكل (vvp_island_branch*cur = branches_ ; cur ; cur = cur->اگلا_branch) {
	    vvp_island_branch_tran*tmp = dynamic_cast<vvp_island_branch_tran*>(cur);
	    أكد(tmp);
	    enable_بدلیا |= tmp->rerun_test_enabled();
      }
      إذا (enable_بدلیا) اذهب rerun;
}

ساكن فراغ گنتی_drivers_2(vvp_branch_ptr_t cur, منطقي other_side_ویکھیا,
                           بنانشانی ثنائی_ہندسہ_idx, بنانشانی counts[3])
{
        // First count any value driven into the port associated with
        // the current endpoint.
      vvp_net_t*net = cur.پورٹ() ? cur.ptr()->b : cur.ptr()->a;
      vvp_scalar_t ثنائی_ہندسہ = island_get_قدر(net).value(ثنائی_ہندسہ_idx);
      update_driver_counts(ثنائی_ہندسہ.value(), counts);

        // Now handle all the branches attached to that port.
      vvp_branch_ptr_t idx = cur;
      افعل {
            vvp_island_branch_tran*tmp = BRANCH_TRAN(idx.ptr());

              // If this branch represents a tran gate, we count the
              // value on the other side of the tran (providing it is
              // enabled) as a single driver.
            إذا (tmp->width == 0) {
                  إذا (tmp->state == tran_enabled) {
                        net = idx.پورٹ() ? idx.ptr()->a : idx.ptr()->b;
                        ثنائی_ہندسہ = island_get_گھلیا_قدر(net).value(ثنائی_ہندسہ_idx);
                        update_driver_counts(ثنائی_ہندسہ.value(), counts);
                  }
                  استمر;
            }

              // If we get here, this branch is a part select. If we've
              // just come from the other end of the branch, we're done.
            إذا ((idx == cur) && other_side_ویکھیا)
                  استمر;

              // If this is the narrow end of the part select, the other
              // end must include the bit we are interested in. Follow
              // the branch to collect any drivers on the other side.
            إذا (idx.پورٹ() == 1) {
                  vvp_branch_ptr_t a_side(tmp, 0);
                  گنتی_drivers_2(a_side, سچ, tmp->وتھ_2 + ثنائی_ہندسہ_idx, counts);
                  استمر;
            }

              // If we get here, this branch is the wide end of a part
              // select. If the bit we are interested in is within the
              // selected part, follow the branch to collect any drivers
              // on the other side.
            إذا ((ثنائی_ہندسہ_idx >= tmp->وتھ_2) && (ثنائی_ہندسہ_idx < tmp->وتھ_2+tmp->part)) {
                  vvp_branch_ptr_t b_side(tmp, 1);
                  گنتی_drivers_2(b_side, سچ, ثنائی_ہندسہ_idx - tmp->وتھ_2, counts);
                  استمر;
            }
      } طالما ((idx = اگلا_2(idx)) != cur);
}

فراغ vvp_island_tran::گنتی_drivers(vvp_island_پورٹ*پورٹ, بنانشانی ثنائی_ہندسہ_idx,
                                    بنانشانی counts[3])
{
        // First we need to find a branch that is attached to the specified
        // port. Unfortunately there's no quick way to do this.
      vvp_island_branch*branch = branches_;
      بنانشانی side = 0;
      طالما (branch) {
            إذا (branch->a->fun == پورٹ) {
                  side = 0;
                  اكسر;
            }
            إذا (branch->b->fun == پورٹ) {
                  side = 1;
                  اكسر;
            }
            branch = branch->اگلا_branch;
      }
      أكد(branch);

        // Now count the drivers, pushing through the network as necessary.
      vvp_branch_ptr_t endpoint(branch, side);
      گنتی_drivers_2(endpoint, خطأ, ثنائی_ہندسہ_idx, counts);
}

فراغ vvp_island_branch_tran::لگاتار_test_enabled()
{
      ثابت vvp_island_پورٹ*ep = en? dynamic_cast<vvp_island_پورٹ*> (en->fun) : عدم;

	// If there is no ep port (no "enabled" input) then this is a
	// tran branch. Assume it is always enabled.
      إذا (ep == 0) {
	    state = tran_enabled;
	    أرجع;
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
      إذا (ep->outvalue.size() != 0)
	    enable_val = ep->outvalue.value(0).value();
      وإلا إذا (ep->invalue.size() == 0)
	    enable_val = BIT4_Z;
      وإلا
	    enable_val = ep->invalue.value(0).value();

      بدل (enable_val) {
	  حالة BIT4_0:
	    state = سرگرم_بلند_2 ? tran_disabled : tran_enabled;
	    اكسر;
	  حالة BIT4_1:
	    state = سرگرم_بلند_2 ? tran_enabled : tran_disabled;
	    اكسر;
	  افتراضي:
	    state = tran_unknown;
	    اكسر;
      }
}

منطقي vvp_island_branch_tran::rerun_test_enabled()
{
      ثابت vvp_island_پورٹ*ep = en? dynamic_cast<vvp_island_پورٹ*> (en->fun) : عدم;

      إذا (ep == 0)
	    أرجع خطأ;

	// We are only looking for changes resulting from running the island
	// resolution. If the outvalue is nil, we know that the enable port
	// is an .import, so won't be affected.
      إذا (ep->outvalue.size() == 0)
	    أرجع خطأ;

      vvp_bit4_t enable_val = ep->outvalue.value(0).value();

      tran_state_t old_state = state;

      بدل (enable_val) {
	  حالة BIT4_0:
	    state = سرگرم_بلند_2 ? tran_disabled : tran_enabled;
	    اكسر;
	  حالة BIT4_1:
	    state = سرگرم_بلند_2 ? tran_enabled : tran_disabled;
	    اكسر;
	  افتراضي:
	    state = tran_unknown;
	    اكسر;
      }

      أرجع state != old_state;
}

// The IEEE standard does not specify the behaviour when a tranif control
// input is 'x' or 'z'. We use the rules that are given for MOS switches.
مضمن vvp_vector8_t resolve_ambiguous(ثابت vvp_vector8_t&a,
                                       ثابت vvp_vector8_t&b,
                                       tran_state_t state,
                                       ثابت بنانشانی str_map[8])
{
      أكد(a.size() == b.size());
      vvp_vector8_t out (a.size());

      لكل (بنانشانی idx = 0 ;  idx < out.size() ;  idx += 1) {
	    vvp_scalar_t a_ثنائی_ہندسہ = a.value(idx);
	    vvp_scalar_t b_ثنائی_ہندسہ = b.value(idx);
            b_ثنائی_ہندسہ = vvp_scalar_t(b_ثنائی_ہندسہ.value(),
                                 str_map[b_ثنائی_ہندسہ.strength0()],
                                 str_map[b_ثنائی_ہندسہ.strength1()]);
	    إذا (state == tran_unknown) {
		  بدل (b_ثنائی_ہندسہ.value()) {
		      حالة BIT4_0:
			b_ثنائی_ہندسہ = vvp_scalar_t(BIT4_X, b_ثنائی_ہندسہ.strength0(), 0);
			اكسر;
		      حالة BIT4_1:
			b_ثنائی_ہندسہ = vvp_scalar_t(BIT4_X, 0, b_ثنائی_ہندسہ.strength1());
			اكسر;
		      افتراضي:
			اكسر;
		  }
	    }
	    out.سیٹ_ثنائی_ہندسہ(idx, resolve(a_ثنائی_ہندسہ, b_ثنائی_ہندسہ));
      }
      أرجع out;
}

ساكن فراغ ڈالو_قدر_through_branches(ثابت vvp_vector8_t&val,
					ثابت list<vvp_branch_ptr_t>&connections);

ساكن فراغ ڈالو_قدر_through_branch(ثابت vvp_vector8_t&val,
                                      vvp_branch_ptr_t cur)
{
      vvp_island_branch_tran*branch = BRANCH_TRAN(cur.ptr());

        // If the branch is disabled, skip.
      إذا (branch->state == tran_disabled)
            أرجع;

      بنانشانی src_ab = cur.پورٹ();
      بنانشانی dst_ab = src_ab^1;

      vvp_net_t*dst_net = dst_ab? branch->b : branch->a;
      vvp_island_پورٹ*dst_پورٹ = dynamic_cast<vvp_island_پورٹ*>(dst_net->fun);

      vvp_vector8_t old_val = dst_پورٹ->value;

        // If the port on the other side has not yet been visited,
        // get its input value.
      إذا (dst_پورٹ->value.size() == 0)
            dst_پورٹ->value = island_get_قدر(dst_net);

        // If we don't yet have an initial value for the port, skip.
      إذا (dst_پورٹ->value.size() == 0)
            أرجع;

        // Now resolve the pushed value with whatever values we have
        // previously collected (and resolved) for the port.
      إذا (branch->width == 0) {
              // There are no part selects.
            dst_پورٹ->value = resolve_ambiguous(dst_پورٹ->value, val, branch->state,
                                                vvp_switch_strength_map[branch->resistive]);

      } وإلا إذا (dst_ab == 1) {
              // The other side is a strict subset (part select)
              // of this side.
            vvp_vector8_t tmp = val.subvalue(branch->وتھ_2, branch->part);
            dst_پورٹ->value = resolve(dst_پورٹ->value, tmp);

      } وإلا {
              // The other side is a superset of this side.
            vvp_vector8_t tmp = part_expand(val, branch->width, branch->وتھ_2);
            dst_پورٹ->value = resolve(dst_پورٹ->value, tmp);
      }

        // If the resolved value for the port has changed, push the new
        // value back into the network.
      إذا (! dst_پورٹ->value.eeq(old_val)) {
            list<vvp_branch_ptr_t> connections;

	    vvp_branch_ptr_t dst_side(branch, dst_ab);
	    island_collect_گرہ(connections, dst_side);

	    ڈالو_قدر_through_branches(dst_پورٹ->value, connections);
      }
}

ساكن فراغ ڈالو_قدر_through_branches(ثابت vvp_vector8_t&val,
					ثابت list<vvp_branch_ptr_t>&connections)
{
      لكل (list<vvp_branch_ptr_t>::const_iterator idx = connections.cbegin()
		 ; idx != connections.cend() ; ++ idx ) {

            ڈالو_قدر_through_branch(val, *idx);
      }
}

/*
 * This method resolves the value for a branch recursively. It uses
 * recursive descent to span the graph of branches, pushing values
 * through the network until a stable state is reached.
 */
فراغ vvp_island_branch_tran::لگاتار_resolution()
{
      list<vvp_branch_ptr_t> connections;
      vvp_island_پورٹ*پورٹ;

	// If the A side port hasn't already been visited, then push
        // its input value through all the branches connected to it.
      پورٹ = dynamic_cast<vvp_island_پورٹ*>(a->fun);
      إذا (پورٹ->value.size() == 0) {
	    vvp_branch_ptr_t a_side(هذا, 0);
	    island_collect_گرہ(connections, a_side);

	    پورٹ->value = island_get_قدر(a);
            إذا (پورٹ->value.size() != 0)
	          ڈالو_قدر_through_branches(پورٹ->value, connections);

            connections.clear();
      }

	// Do the same for the B side port. Note that if the branch
        // is enabled, the B side port will have already been visited
        // when we resolved the A side port.
      پورٹ = dynamic_cast<vvp_island_پورٹ*>(b->fun);
      إذا (پورٹ->value.size() == 0) {
	    vvp_branch_ptr_t b_side(هذا, 1);
	    island_collect_گرہ(connections, b_side);

	    پورٹ->value = island_get_قدر(b);
	    إذا (پورٹ->value.size() != 0)
	          ڈالو_قدر_through_branches(پورٹ->value, connections);

            connections.clear();
      }
}

فراغ vvp_island_branch_tran::لگاتار_output()
{
      vvp_island_پورٹ*پورٹ;

	// If the A side port hasn't already been updated, send the
        // resolved value to the output.
      پورٹ = dynamic_cast<vvp_island_پورٹ*>(a->fun);
      إذا (پورٹ->value.size() != 0) {
	    island_send_قدر(a, پورٹ->value);
	    پورٹ->value = vvp_vector8_t::nil;
      }

	// Do the same for the B side port.
      پورٹ = dynamic_cast<vvp_island_پورٹ*>(b->fun);
      إذا (پورٹ->value.size() != 0) {
	    island_send_قدر(b, پورٹ->value);
	    پورٹ->value = vvp_vector8_t::nil;
      }
}

فراغ compile_island_tran(حرف*label)
{
      vvp_island*use_island = جديد vvp_island_tran;
      compile_island_ادھار(label, use_island);
}

فراغ compile_island_tranif(صحيح sense, حرف*island, حرف*pa, حرف*pb, حرف*pe,
                           منطقي resistive)
{
      vvp_island*use_island = compile_لبھو_island(island);
      أكد(use_island);
      حرر(island);

      vvp_net_t*en = عدم;

      إذا (pe) {
	    en = use_island->لبھو_پورٹ(pe);
	    أكد(en);
	    حرر(pe);
      }

      vvp_island_branch_tran*br = جديد vvp_island_branch_tran(en,
                                                             sense ? سچ :
                                                                     خطأ,
                                                             0, 0, 0, resistive);

      use_island->جوڑو_branch(br, pa, pb);

      حرر(pa);
      حرر(pb);
}


فراغ compile_island_tranvp(حرف*island, حرف*pa, حرف*pb,
			   بنانشانی wid, بنانشانی par, بنانشانی off)
{
      vvp_island*use_island = compile_لبھو_island(island);
      أكد(use_island);
      حرر(island);

      vvp_island_branch_tran*br = جديد vvp_island_branch_tran(عدم, خطأ,
                                                             wid, par, off, خطأ);

      use_island->جوڑو_branch(br, pa, pb);

      حرر(pa);
      حرر(pb);
}
